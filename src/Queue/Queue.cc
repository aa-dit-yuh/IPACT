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

#include "Queue.h"

namespace epon {
Define_Module(Queue);

Queue::Queue() {

}

Queue::~Queue() {
    for(int i = 0;i < p;i++) delete queue[i];
}

void Queue::initialize() {
    //parameters
    p = intpar("FramePriorities");
    nonstrict = false;
    forwardLength = new int[p];
    for(int i = 0;i < p;i++) forwardLength[i] = 0;
    forwardSize = new int[p];
    for(int i = 0;i < p;i++) forwardSize[i] = 0;

    //components
    for(int i = 0;i < p;i++) queue.push_back(new omnetpp::cPacketQueue("queue"));
}

void Queue::setNonStrict(bool NonStrict) {
    nonstrict = NonStrict;
}

void Queue::setPopForwarded(bool b) {
    popForwarded = b;
}

//forward
void Queue::forward(int priority) {
    if(priority < 0 || priority > p - 1) throw omnetpp::cRuntimeError("[%s::forward] Cannot identify priority(priority=%d).",cSimpleModule::getName(),priority);
    for(int j = 0;j < queue[priority]->getLength();++j)
    {
        DataFrame* data = omnetpp::check_and_cast<DataFrame*>(queue[priority]->get(j));
        data->setForwarded(true);
        data->setS1Delay (omnetpp::simTime() - data->getRxTime());
        forwardLength[priority] = forwardLength[priority] + 1;
        forwardSize[priority] += queue[priority]->get(j)->getBitLength();
    }
}

int Queue::getForwardLength() {
    int sum = 0;
    for(int i = 0;i < p;i++) sum += getForwardLength(i);
    return sum;
}

int Queue::getForwardLength(int priority) {
    if(priority < 0 || priority > p - 1) throw omnetpp::cRuntimeError("[%s::getforwardLength] Cannot identify priority(priority=%d).",cSimpleModule::getName(),priority);
    return forwardLength[priority];
}

int Queue::getForwardSize() {
    int sum = 0;
    for(int i = 0;i < p;i++) sum += getForwardSize(i);
    return sum;
}

int Queue::getForwardSize(int priority) {
    if(priority < 0 || priority > p - 1) throw omnetpp::cRuntimeError("[%s::getforwardSize] Cannot identify priority(priority=%d).",cSimpleModule::getName(),priority);
    return forwardSize[priority];
}

//insertion and removal functions
void Queue::insert(omnetpp::cPacket *packet) {
    int priority = packet->getKind();
    if(priority < 0 || priority > p - 1)
        throw omnetpp::cRuntimeError("[%s::insert] Cannot identify priority(priority=%d,p=%d).",cSimpleModule::getName(),priority,p);

    //insert packet
    queue[priority]->insert(packet);
}

omnetpp::cPacket* Queue::remove(omnetpp::cPacket *packet) {
    omnetpp::cPacket* removePacket = NULL;
    for(int i = 0;i < int(queue.size());++i)
    {
        removePacket = queue[i]->remove(packet);
        if(removePacket != NULL)  return removePacket;
    }

    //packet not found
    return removePacket;
}

omnetpp::cPacket* Queue::pop() {
    omnetpp::cPacket* packet = NULL;

    if(nonstrict)
        for(int i = 0;i < int(queue.size());++i)
        {
            if(forwardLength[i] > 0 && !queue[i]->isEmpty())
            {
                packet = queue[i]->pop();
                --forwardLength[i];
                forwardSize[i] -= packet->getBitLength();
                i = queue.size();
            }
        }

    if(popForwarded) return packet;

    //not nonstrict or nonstrict and has no reported packets
    if(packet == NULL)
        for(int i = 0;i < int(queue.size());++i)
            if(!queue[i]->isEmpty())
            {
                packet = queue[i]->pop();
                i = queue.size();
            }

    return packet;
}

omnetpp::cPacket* Queue::popByPriority(int priority) {
    if(priority < 0 || priority > p - 1)
        throw omnetpp::cRuntimeError("[%s::popByPriority] Cannot identify priority(priority=%d,p=%d).",cSimpleModule::getName(),priority,p);
    return queue[priority]->pop();
}

//query functions
bool Queue::isEmpty() {
    for(int i = 0;i < int(queue.size());++i)
        if(!queue[i]->isEmpty()) return false;
    return true;
}

bool Queue::isEmptyByPriority(int priority) {
    if(priority < 0 || priority > p - 1)
        throw omnetpp::cRuntimeError("[%s::isEmptyByPriority] Cannot identify priority(priority=%d,p=%d).",cSimpleModule::getName(),priority,p);
    return queue[priority]->isEmpty();
}

int Queue::getLength() {
    int s = 0;
    for(int i = 0;i < int(queue.size());++i) s += queue[i]->getLength();
    return s;
}

int Queue::getLengthByPriority(int priority) {
    if(priority < 0 || priority > p - 1)
            throw omnetpp::cRuntimeError("[%s::getLengthByPriority] Cannot identify priority(priority=%d,p=%d).",cSimpleModule::getName(),priority,p);
    return queue[priority]->getLength();
}

int64_t Queue::getBitLength() {
    int s = 0;
    for(int i = 0;i < int(queue.size());++i) s += queue[i]->getBitLength();
    return s;
}

int64_t Queue::getBitLengthByPriority(int priority) {
    if(priority < 0 || priority > p - 1)
            throw omnetpp::cRuntimeError("[%s::getLengthByPriority] Cannot identify priority(priority=%d,p=%d).",cSimpleModule::getName(),priority,p);
    return queue[priority]->getBitLength();
}

omnetpp::cPacket* Queue::front() {
    if(nonstrict)
    {
        for(int i = 0;i < int(queue.size());++i) if(forwardLength[i] > 0 && !queue[i]->isEmpty()) return queue[i]->front();
    }
    if(popForwarded) return NULL;
    for(int i = 0;i < int(queue.size());++i)
    {
        //std::cerr << "[Queue::front] empty=" << queue[i]->isEmpty() << " length=" << queue[i]->getLength() <<  std::endl;
        if(!queue[i]->isEmpty()) return queue[i]->front();
    }
    return NULL;
}

omnetpp::cPacket* Queue::back() {
    for(int i = queue.size() - 1;i >= 0;--i) if(!queue[i]->isEmpty()) return queue[i]->back();
    return NULL;
}

void Queue::list() {
    for(int i = 0;i < int(queue.size());++i)
    {
        std::cerr << "Packet kind=" << i << "\tlength=" << queue[i]->getLength() << std::endl;
        for(int j = 0;j < queue[i]->getLength();++j)
            std::cerr << j << "\tn=" << queue[i]->get(j)->getName() << "\tk=" << queue[i]->get(j)->getKind() << std::endl;
    }
}

} /* namespace epon */
