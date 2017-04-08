//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "InterScheduler.h"

namespace epon {
Define_Module(InterScheduler);

InterScheduler::InterScheduler() {

}

InterScheduler::~InterScheduler() {

}

void InterScheduler::initialize() {
    Scheduler::initialize();

    //parameters
    tAvailable =  omnetpp::simTime();
    tMPCP = getTransmissionTime(intpar("bitlenMPCP"));
    tCycleStart = 0;
    tCycleRemain = 0;
    tStart.assign(intpar("numONUs"), omnetpp::simTime());
    tWindow.assign(intpar("numONUs"), 0);
    tRequired.assign(intpar("numONUs"), 0);
    RTT.assign(intpar("numONUs"), 2 * timepar("tProp"));
    currentONU = 0;

    //components
    queue = omnetpp::check_and_cast<MultiQueue*>(getParentModule()->getSubmodule("queue"));

    //statistics
    sdCycleTime = new  omnetpp::cStdDev("cycleTimeRecord");

    //initial schedule
    for(int i = 0;i < intpar("numONUs");i++) bandwidthAlloc(i,0,0);
}

void InterScheduler::finish() {
    Scheduler::finish();
    recordScalar("cycle time avg",sdCycleTime->getMean());
    recordScalar("cycle time stddev",sdCycleTime->getStddev());

    Recorder* r = omnetpp::check_and_cast<Recorder*>(getParentModule()->getParentModule()->getSubmodule("recorder"));
    std::vector<double> v = std::vector<double>();
    v.push_back(sdCycleTime->getMean());
    v.push_back(sdCycleTime->getStddev());
    r->addDataRow("AvgCycleTime",v);

    delete queue;
    delete sdCycleTime;
}

void InterScheduler::handleSelfMessage (omnetpp::cMessage* msg) {
    std::cerr << "OLT::handleSelfMessage t=" <<  omnetpp::simTime() << std::endl;

    if(msg->getKind() == intpar("SendMPCP"))
    {
        //Downstream transmission window start
        currentONU = int(msg->par("LLID"));
        sendMPCP(int(msg->par("LLID")));
        scheduled.pop_back();
        scheduleDownstream();
    }
    delete msg;
}

void InterScheduler::sendMPCP(int LLID) {

    MPCPGate* gate = new MPCPGate("gate");
    gate->setDST(LLID);
    gate->setBitLength(intpar("bitlenMPCP"));
    gate->setStartTime(tStart[LLID]);
    gate->setLength(tWindow[LLID]);
    send(gate,"txSplitter");
}

void InterScheduler::scheduleDownstream() {
    switch(intpar("downstreamScheduling"))
    {
    case FIFO:
        if(scheduled.size() > 0) sendDataUntil(tStart[scheduled.back()] - tMPCP - RTT[scheduled.back()] / 2);
        else sendDataUntil(tAvailable - tMPCP - timepar("tProp") * 2);
        break;
    case TDM:
        sendDataUntil(tStart[currentONU] + tWindow[currentONU] + timepar("tGuard") - RTT[currentONU] / 2);
        break;
    }
}

void InterScheduler::handleMessageFromSplitter (omnetpp::cMessage* msg) {
    Frame* frame = omnetpp::check_and_cast<Frame*>(msg);
    if(frame->getKind() == intpar("MPCP"))
    {
        handleMPCP(msg);
        delete msg;
    }
    else if(msg->getKind() > intpar("MPCP") && msg->getKind() < intpar("FramePriorities")) send(frame,"txHost");
    else delete frame;
}

void InterScheduler::handleMPCP (omnetpp::cMessage* msg) {
    MPCPReport* report = omnetpp::check_and_cast<MPCPReport*>(msg);
    bandwidthAlloc(report->getSRC(),report->getRequiredBandwidth(),queue->getBitLength(report->getSRC()));
}

void InterScheduler::handleMessageFromHost (omnetpp::cMessage* msg) {
    queue->insert(omnetpp::check_and_cast<omnetpp::cPacket*>(msg));
    scheduleDownstream();
}

void InterScheduler::bandwidthAlloc(int LLID,int upRequiredBitlength,int downRequiredBitlength) {
    //time point of cycle start
    //for elastic service to count remain bandwidth in this cycle
    if(LLID == 0)
    {
        sdCycleTime->collect (omnetpp::simTime() - tCycleStart);
        tCycleStart =  omnetpp::simTime();
        tCycleRemain = timepar("tCycle");
    }

    //required bandwidth
    tRequired[LLID] = getTransmissionTime(upRequiredBitlength);
    omnetpp::simtime_t tDownRequired = getTransmissionTime(downRequiredBitlength);
    if(intpar("downstreamScheduling") == TDM) tRequired[LLID] = std::max(tDownRequired,tRequired[LLID]);

    //update tStart and tWindow
    omnetpp::simtime_t tNextAvailable =  omnetpp::simTime() + RTT[LLID] + tMPCP;
    if(tAvailable < tNextAvailable) tAvailable = tNextAvailable;
    tStart[LLID] = tAvailable - RTT[LLID] / 2;

    //inter scheduling service
    omnetpp::simtime_t tCredit;
    switch(intpar("interScheduling"))
    {
    case FIXED:
        tWindow[LLID] = getOnlineMaxWindow();
        break;
    case GATED:
        tWindow[LLID] = tRequired[LLID];
        break;
    case LIMITED:
        tWindow[LLID] = tRequired[LLID] <= getOnlineMaxWindow() ? tRequired[LLID] : getOnlineMaxWindow();
        break;
    case CONSTANT:
        tCredit = tRequired[LLID] + getTransmissionTime(intpar("credit"));
        tWindow[LLID] = tCredit <= getOnlineMaxWindow() ? tCredit : getOnlineMaxWindow();
        break;
    case LINEAR:
        tCredit = tRequired[LLID] * (1 + dblpar("credit"));
        tWindow[LLID] = tCredit <= getOnlineMaxWindow() ? tCredit : getOnlineMaxWindow();
        break;
    case ELASTIC:
        tWindow[LLID] = tRequired[LLID] <= tCycleRemain ? tRequired[LLID] : tCycleRemain;
        tCycleRemain -= tWindow[LLID];
        break;
    }
    tAvailable += tWindow[LLID] + tMPCP + timepar("tGuard");

    //schedule to send gate
    omnetpp::cMessage* msg = new omnetpp::cMessage("SendGate",intpar("SendMPCP"));
     omnetpp::cMsgPar* par = new  omnetpp::cMsgPar("LLID");
    par->setLongValue(LLID);
    msg->addPar(par);
    scheduleAt(tStart[LLID] - tMPCP - RTT[LLID] / 2 + tProcess,msg);
    scheduled.insert(scheduled.begin(),LLID);
}

bool InterScheduler::queueEmpty() {
    switch(intpar("downstreamScheduling"))
    {
        case FIFO:
            return Scheduler::queueEmpty();
            break;
        case TDM:
            return queue->isEmpty(currentONU);
            break;
    }
    throw omnetpp::cRuntimeError("[%s::getNextDataFrame] undefined downstream scheduling mode");
}

omnetpp::cPacket* InterScheduler::getNextDataFrame() {
    switch(intpar("downstreamScheduling"))
    {
        case FIFO:
            return getNextDataFrameFIFO();
            break;
        case TDM:
            return getNextDataFrameTDM();
            break;
    }
    throw omnetpp::cRuntimeError("[%s::getNextDataFrame] undefined downstream scheduling mode");
}

omnetpp::cPacket* InterScheduler::getNextDataFrameFIFO() {
    return Scheduler::getNextDataFrame();
}

omnetpp::cPacket* InterScheduler::getNextDataFrameTDM() {
    return queue->front(currentONU);
}

} /* namespace epon */
