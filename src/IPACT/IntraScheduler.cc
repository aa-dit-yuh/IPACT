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

#include "IntraScheduler.h"

namespace epon {
Define_Module(IntraScheduler);

IntraScheduler::IntraScheduler() {

}

IntraScheduler::~IntraScheduler() {

}

void IntraScheduler::initialize() {
    Scheduler::initialize();

    //parameters
    tStart =  omnetpp::simTime();
    tWindow = 0;

    //components
    queue = omnetpp::check_and_cast<Queue*>(getParentModule()->getSubmodule("queue"));
}

void IntraScheduler::finish() {
    Scheduler::finish();
    delete queue;
}

void IntraScheduler::handleSelfMessage (omnetpp::cMessage* msg) {

    if(msg->getKind() == intpar("SendMPCP")) sendMPCP();
    delete msg;
}

void IntraScheduler::sendMPCP() {
    MPCPReport* report = new MPCPReport("report",intpar("MPCP"));
    report->setBitLength(intpar("bitlenMPCP"));
    report->setSRC(getLLID());
    report->setDST(getLLID());
    report->setRequiredBandwidth(queue->getBitLength());
    send(report,"txSplitter");
}

void IntraScheduler::handleMessageFromSplitter (omnetpp::cMessage* msg) {
    Frame* frame = omnetpp::check_and_cast<Frame*>(msg);
    //std::cerr << "[IntraScheduler" << getLLID() << "::handleMessageFromSplitter] frame.LLID=" << frame->getLLID() << std::endl;
    if(frame->getDST() == getLLID())
    {
        if(frame->getKind() == intpar("MPCP"))
        {
            MPCPGate* gate = omnetpp::check_and_cast<MPCPGate*>(frame);
            handleGate(gate);
            delete gate;
        }
        else if(msg->getKind() > intpar("MPCP") && msg->getKind() < intpar("FramePriorities")) handleDataFromSplitter(frame);
        else delete frame;
    }
    else delete frame;
}

void IntraScheduler::handleMessageFromHost (omnetpp::cMessage* msg) {
    handleDataFromHost(omnetpp::check_and_cast<DataFrame*>(msg));
    sendDataUntil(tStart + tWindow);
}

void IntraScheduler::handleGate(MPCPGate* gate) {

    //update tStart and tWindow
    tStart = gate->getStartTime();
    tWindow = gate->getLength();
    sendDataUntil(tStart + tWindow);
    scheduleAt(tStart + tWindow,new omnetpp::cMessage("SendReport",intpar("SendMPCP")));
}

void IntraScheduler::handleDataFromSplitter(Frame* data) {
    send(data,"txHost");
}

void IntraScheduler::handleDataFromHost(Frame* data) {
    queue->insert(data);
}

int IntraScheduler::getLLID() {
    return getParentModule()->getIndex();
}

} /* namespace epon */
