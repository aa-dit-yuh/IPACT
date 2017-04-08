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

#include "Scheduler.h"

namespace epon {
Define_Module(Scheduler);

Scheduler::Scheduler() {

}

Scheduler::~Scheduler() {
    delete sdPacketSize;
}

void Scheduler::initialize() {
    //parameters
    tGuard = timepar("tGuard");
    RTT = 2 * timepar("tProp");

    //components
    //queue component is not initialized!
    txSplitter = omnetpp::check_and_cast<omnetpp::cDatarateChannel*>(getParentModule()->gate("txSplitter")->getChannel());
    tMPCP = getTransmissionTime(intpar("MPCPSize"));

    //statistics
    sdPacketSize = new omnetpp::cStdDev("packetSize");
}

void Scheduler::finish() {
    recordScalar("sent bits",sdPacketSize->getSum(),"b");
    recordScalar("frame departure rate",sdPacketSize->getCount() / omnetpp::simTime().dbl(),"");
}

void Scheduler::handleMessage(omnetpp::cMessage* msg) {
    //handle message according to where it is from
    if(msg->isSelfMessage())
    {
        //send data event and self event
        if(msg->getKind() == intpar("SendUntil"))
        {
            sendDataUntil((omnetpp::simtime_t)msg->par("tUntil"));
            delete msg;
        }
        else handleSelfMessage(msg);
    }
    else if(msg->arrivedOn("rxSplitter"))
    {
        handleMessageFromSplitter(msg);
    }
    else if(msg->arrivedOn("rxHost"))
    {
        //add reveice timestamp to incoming packet from host
        DataFrame* data = omnetpp::check_and_cast<DataFrame*>(msg);
        setRxTimeStamp(data);

        //handle packet
        handleMessageFromHost(msg);
    }
    else delete msg;
}

void Scheduler::handleSelfMessage(omnetpp::cMessage* msg) {

}

void Scheduler::handleMessageFromSplitter(omnetpp::cMessage* msg) {

}

void Scheduler::handleMessageFromHost(omnetpp::cMessage* msg) {

}

void Scheduler::send(omnetpp::cMessage* msg,const char *gatename) {
    omnetpp::cSimpleModule::send(msg,gatename);
}

void Scheduler::sendDataUntil(omnetpp::simtime_t tUntil) {
    omnetpp::simtime_t tRemain = tUntil - getTxSplitterSchedTime();
    if(getNextDataFrame() != NULL && tRemain >= getTransmissionTime(getNextDataFrame()->getBitLength()))
    {
        //tx to splitter is not busy
        if(getTxSplitterSchedTime() == omnetpp::simTime())
        {
            DataFrame* data = omnetpp::check_and_cast<DataFrame*>(popNextDataFrame());
            setTxTimeStamp(data, omnetpp::simTime());
            sdPacketSize->collect(data->getBitLength());
            handleDataBeforeSend(data);
            send(data,"txSplitter");
        }

        //schedule next transmission if queue is not empty
        //schedule to send when tx is not busy
        msgSendUntil = new omnetpp::cMessage("msgSendUntil",intpar("SendUntil"));
        omnetpp::cMsgPar* par = new omnetpp::cMsgPar("tUntil");
        par->setDoubleValue(tUntil.dbl());
        msgSendUntil->addPar(par);
        scheduleAt(getTxSplitterSchedTime(),msgSendUntil);
    }
}

void Scheduler::handleDataBeforeSend(DataFrame* data)
{

}

//query
omnetpp::simtime_t Scheduler::getTransmissionTime(int bitLength) {
    omnetpp::simtime_t t = (1.0 * bitLength) / txSplitter->getDatarate();
    return t;
}

bool Scheduler::isTxSplitterBusy() {
    return txSplitter->isBusy();
}

omnetpp::simtime_t Scheduler::getTxSplitterSchedTime() {
    omnetpp::simtime_t t = txSplitter->getTransmissionFinishTime();
    return std::max(t, omnetpp::simTime());
}

int Scheduler::getLLID() {
    return intpar("LLID");
}

omnetpp::cPacket* Scheduler::getNextDataFrame() {
    throw omnetpp::cRuntimeError("getNextDataFrame()");
}

omnetpp::cPacket* Scheduler::popNextDataFrame() {
    throw omnetpp::cRuntimeError("popNextDataFrame()");
}

bool Scheduler::queueEmpty() {
    throw omnetpp::cRuntimeError("queueEmpty()");
}

//set
void Scheduler::setRxTimeStamp(DataFrame* data) {
    data->setRxTime(omnetpp::simTime());
}

void Scheduler::setTxTimeStamp(DataFrame* data, omnetpp::simtime_t txTimeStamp) {
    data->setTxTime(txTimeStamp);
}

} /* namespace epon */
