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

#include "TwoStageQueue.h"

namespace epon {
Define_Module(TwoStageQueue);

TwoStageQueue::TwoStageQueue() {

}

TwoStageQueue::~TwoStageQueue() {
    for(int i = 0;i < p;i++)
    {
        delete queue[i];
        delete sdDelay[i];
    }
}

void TwoStageQueue::initialize() {
    //parameters
    p = intpar("FramePriorities");
    s2Length = new int[p];
    s2Size = new int[p];
    for(int i = 0;i < p;++i) s2Length[i] = 0;
    for(int i = 0;i < p;++i) s2Size[i] = 0;

    //components
    for(int i = 0;i < p;++i) queue.push_back(new omnetpp::cPacketQueue("queue"));

    testFwd = false;
    //initialize
    tCycle = 0;
    tDC.assign(intpar("FramePriorities"),0);
    tNextFwd.assign(intpar("FramePriorities"),0);
    rAdjust.assign(intpar("FramePriorities"),0);
    for(int i = 0;i < intpar("FramePriorities");++i) sdDelay.push_back(new  omnetpp::cStdDev("sdDelay"));
    if(testFwd) for(int i = intpar("EF");i < intpar("FramePriorities");++i) forward(i);
}

void TwoStageQueue::handleMessage (omnetpp::cMessage* msg) {
    if(msg->isSelfMessage()) forward(int(msg->par("priority")));
    delete msg;
}

//forward functions
void TwoStageQueue::setCycle (omnetpp::simtime_t t) {
    tCycle = t;
}

void TwoStageQueue::setDC(int priority, omnetpp::simtime_t t) {
    priorityCheck(priority);
    tDC[priority] = t;
}

void TwoStageQueue::forward(int priority) {
    priorityCheck(priority);
    for(int j = s2Length[priority];j < queue[priority]->getLength();++j) stepForward(priority);

    if(testFwd)
    {
        omnetpp::cMessage* msg = new omnetpp::cMessage();
         omnetpp::cMsgPar* par = new  omnetpp::cMsgPar("priority");
        par->setLongValue(priority);
        msg->addPar(par);
        omnetpp::simtime_t t = getFwdCycle(priority);
        tNextFwd[priority] += t;
        scheduleAt(tNextFwd[priority],msg);

        //use sendDirect to wake up ONU or inform OLT that packets in stage-two
        msg = new omnetpp::cMessage("Interrupt",intpar("Interrupt"));
        sendDirect(msg,getParentModule()->getSubmodule("scheduler"),"rxSignal");
    }

}

bool TwoStageQueue::stepForward(int priority) {
    priorityCheck(priority);
    if((queue[priority]->getLength() - s2Length[priority]) > 0)
    {
        int j = s2Length[priority];
        DataFrame* data = omnetpp::check_and_cast<DataFrame*>(queue[priority]->get(j));
        data->setForwarded(true);
        data->setS1Delay (omnetpp::simTime() - data->getRxTime());
        ++s2Length[priority];
        s2Size[priority] += queue[priority]->get(j)->getBitLength();
        return true;
    }
    return false;
}

omnetpp::simtime_t TwoStageQueue::getFwdCycle(int priority) {
    //adjust
    omnetpp::simtime_t d = sdDelay[priority]->getMean();
    if(d > tDC[priority] && rAdjust[priority] > dblpar("rMin")) rAdjust[priority] -= dblpar("rDelta");
    else if(d < tDC[priority] && rAdjust[priority] < dblpar("rMax")) rAdjust[priority] += dblpar("rDelta");

    //calculate forward cycle
    omnetpp::simtime_t tFwdDelay = 2 * tCycle + timepar("tOverhead");
    omnetpp::simtime_t t = 2 * (tDC[priority] - tFwdDelay);
    t *= rAdjust[priority] * (getOfflineMaxWindow() / (getOfflineMaxWindow() + getDownONULambda(priority) * tCycle * getDownFirstMoment(priority)));
    return t;
}

omnetpp::simtime_t TwoStageQueue::getRemainTime() {
    return std::min(std::min(tNextFwd[intpar("EF")],tNextFwd[intpar("AF")]),tNextFwd[intpar("BE")]) -  omnetpp::simTime();
}

//
int TwoStageQueue::getS2Length() {
    int sum = 0;
    for(int i = 0;i < p;i++) sum += getS2Length(i);
    return sum;
}

int TwoStageQueue::getS2Length(int priority) {
    priorityCheck(priority);
    return s2Length[priority];
}

int TwoStageQueue::getS2Size() {
    int sum = 0;
    for(int i = 0;i < p;i++) sum += getS2Size(i);
    return sum;
}

int TwoStageQueue::getS2Size(int priority) {
    priorityCheck(priority);
    return s2Size[priority];
}

//insertion and removal functions
void TwoStageQueue::insert(omnetpp::cPacket *packet) {
    int priority = packet->getKind();
    priorityCheck(priority);

    //bDropMax is the total size (in bits) of queues which has lesser or equal priority than the packet
    int bDropMax = 0;
    for(int i = intpar("BE");i >= packet->getKind();--i) bDropMax += queue[i]->getBitLength();
    while(getBitLength() + packet->getBitLength() > intpar("bufferSize") && bDropMax >= packet->getBitLength())
    {
        int bDrop = 0;
        for(int i = intpar("BE");i >= packet->getKind();--i)
            if(!queue[i]->isEmpty())
            {
                bDrop = front(i)->getBitLength();
                delete queue[i]->remove(front(i));
                i = 0;
            }
        bDropMax -= bDrop;
    }
    queue[priority]->insert(packet);
}

omnetpp::cPacket* TwoStageQueue::remove(omnetpp::cPacket *packet) {
    omnetpp::cPacket* temp = NULL;
    for(int i = 0;i < int(queue.size());++i)
    {
        temp = queue[i]->remove(packet);
        if(temp != NULL)
        {
            DataFrame* data = omnetpp::check_and_cast<DataFrame*>(temp);
            if(data->getForwarded())
            {
                --s2Length[data->getKind()];
                s2Size[data->getKind()] -= data->getBitLength();
            }
            return temp;
        }
    }
    return temp;
}

omnetpp::cPacket* TwoStageQueue::pop() {
    //for stage-two packet and size calculation
    omnetpp::cPacket* temp = popS2();
    if(temp != NULL) return temp;
    for(int i = 0;i < p;++i)
    {
        temp = pop(i);
        if(temp != NULL) return temp;
    }
    return temp;
}

omnetpp::cPacket* TwoStageQueue::pop(int priority) {
    priorityCheck(priority);
    //for stage-two packet and size calculation
    omnetpp::cPacket* temp = popS2(priority);
    if(temp != NULL) return temp;
    if(!queue[priority]->isEmpty())
    {
        sdDelay[temp->getKind()]->collect (omnetpp::simTime() - (omnetpp::check_and_cast<DataFrame*>(temp))->getRxTime());
        return queue[priority]->pop();
    }
    return NULL;
}

omnetpp::cPacket* TwoStageQueue::popS2() {
    for(int i = 0;i < p;++i)
    {
        omnetpp::cPacket* temp = popS2(i);
        if(temp != NULL) return temp;
    }
    return NULL;
}

omnetpp::cPacket* TwoStageQueue::popS2(int priority) {
    priorityCheck(priority);
    if(s2Length[priority] > 0)
    {
        DataFrame* temp = omnetpp::check_and_cast<DataFrame*>(queue[priority]->pop());
        sdDelay[temp->getKind()]->collect (omnetpp::simTime() - (omnetpp::check_and_cast<DataFrame*>(temp))->getRxTime());
        --s2Length[priority];
        s2Size[priority] -= temp->getBitLength();
        return temp;
    }
    return NULL;
}

//query functions
bool TwoStageQueue::isEmpty() {
    for(int i = 0;i < int(queue.size());++i) if(!queue[i]->isEmpty()) return false;
    return true;
}

bool TwoStageQueue::isEmpty(int priority) {
    priorityCheck(priority);
    return queue[priority]->isEmpty();
}

int TwoStageQueue::getLength() {
    int s = 0;
    for(int i = 0;i < int(queue.size());++i) s += queue[i]->getLength();
    return s;
}

int TwoStageQueue::getLength(int priority) {
    priorityCheck(priority);
    return queue[priority]->getLength();
}

int64_t TwoStageQueue::getBitLength() {
    int s = 0;
    for(int i = 0;i < int(queue.size());++i) s += queue[i]->getBitLength();
    return s;
}

int64_t TwoStageQueue::getBitLength(int priority) {
    priorityCheck(priority);
    return queue[priority]->getBitLength();
}

omnetpp::cPacket* TwoStageQueue::front() {
    for(int i = 0;i < int(queue.size());++i) if(!queue[i]->isEmpty()) return queue[i]->front();
    return NULL;
}

omnetpp::cPacket* TwoStageQueue::front(int priority) {
    priorityCheck(priority);
    return queue[priority]->front();
}

omnetpp::cPacket* TwoStageQueue::s2front() {
    for(int i = 0;i < int(queue.size());++i) if(s2Length[i] > 0) return queue[i]->front();
    return NULL;
}

omnetpp::cPacket* TwoStageQueue::s1front() {
    for(int i = 0;i < int(queue.size());++i)
    {
        omnetpp::cPacket* packet = s1front(i);
        if(packet != NULL) return packet;
    }
    return NULL;
}

omnetpp::cPacket* TwoStageQueue::s1front(int priority) {
    priorityCheck(priority);
    if((getLength(priority) - s2Length[priority]) > 0) return queue[priority]->get(s2Length[priority]);
    return NULL;
}

omnetpp::cPacket* TwoStageQueue::back() {
    for(int i = queue.size() - 1;i >= 0;--i) if(!queue[i]->isEmpty()) return queue[i]->back();
    return NULL;
}

omnetpp::cPacket* TwoStageQueue::s2back() {
    for(int i = queue.size() - 1;i >= 0;--i) if(s2Length[i] > 0) return queue[i]->get(s2Length[i] - 1);
    return NULL;
}


void TwoStageQueue::priorityCheck(int priority) {
    if(priority < 0 || priority > p - 1) throw omnetpp::cRuntimeError("%s Cannot identify priority(priority=%d,p=%d).",cSimpleModule::getName(),priority,p);
}

} /* namespace epon */
