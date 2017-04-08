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

#include "MultiQueue.h"

namespace epon {
Define_Module(MultiQueue);

MultiQueue::MultiQueue() {

}

MultiQueue::~MultiQueue() {
    for(int i = 0;i < intpar("numONUs");i++) delete queue[i];
}

void MultiQueue::initialize() {
    for (int i = 0;i < intpar("numONUs");i++)
    {
        Queue* q = new Queue();
        q->initialize();
        queue.push_back(q);
    }
}

//report
void MultiQueue::setNonStrict(int LLID,bool NonStrict) {
    queue[LLID]->setNonStrict(NonStrict);
}

void MultiQueue::setPopForwarded(int LLID,bool b) {
    queue[LLID]->setPopForwarded(b);
}

void MultiQueue::forward(int LLID,int priority) {
    queue[LLID]->forward(priority);
}

int MultiQueue::getForwardLength(int LLID) {
    return queue[LLID]->getForwardLength();
}

int MultiQueue::getForwardLength(int LLID,int priority) {
    return queue[LLID]->getForwardLength(priority);
}

int MultiQueue::getForwardSize(int LLID) {
    return queue[LLID]->getForwardSize();
}

int MultiQueue::getForwardSize(int LLID,int priority) {
    return queue[LLID]->getForwardSize(priority);
}

//insertion and removal functions
void MultiQueue::insert(omnetpp::cPacket *packet) {
    DataFrame* data = omnetpp::check_and_cast<DataFrame*>(packet);
    queue[data->getDST()]->insert(data);
}

omnetpp::cPacket* MultiQueue::remove(omnetpp::cPacket *packet) {
    for(int i = 0; i < int(queue.size()); i++) {
        omnetpp::cPacket* p = queue.at(i)->remove(packet);
        if(p != NULL) return p;
    }
    return NULL;
}

omnetpp::cPacket* MultiQueue::pop() {
    if(front() != NULL)
    {
        DataFrame* data = omnetpp::check_and_cast<DataFrame*>(front());
        return queue.at(data->getDST())->pop();
    }
    return NULL;
}

omnetpp::cPacket* MultiQueue::pop(int LLID) {
    if(LLID >= 0 && LLID < intpar("numONUs")) return queue[LLID]->pop();
    throw omnetpp::cRuntimeError("[%s::pop] LLID out of bound ,LLID=", LLID);
}

//query functions
bool MultiQueue::isEmpty(int LLID) {
    return queue[LLID]->isEmpty();
}

bool MultiQueue::isEmpty() {
    for(int i = 0; i < int(queue.size()); i++)
        if(!queue.at(i)->isEmpty()) return false;
    return true;
}

bool MultiQueue::isEmptyByPriority(int LLID,int priority) {
    return queue[LLID]->isEmptyByPriority(priority);
}

int MultiQueue::getLength() {
    int sum = 0;
    for(int i = 0;i < int(queue.size());i++) sum += queue.at(i)->getLength();
    return sum;
}

int MultiQueue::getLength(int LLID) {
    return queue[LLID]->getLength();
}

int MultiQueue::getLengthByPriority(int LLID,int priority) {
    return queue[LLID]->getLengthByPriority(priority);
}

int64_t MultiQueue::getBitLength() {
    int64_t sum = 0;
    for(int i = 0; i < int(queue.size()); i++) sum += queue.at(i)->getBitLength();
    return sum;
}

int64_t MultiQueue::getBitLength(int LLID) {
    return queue[LLID]->getBitLength();
}

int64_t MultiQueue::getBitLengthByPriority(int LLID,int priority) {
    return queue[LLID]->getBitLengthByPriority(priority);
}

omnetpp::cPacket* MultiQueue::front() {
    omnetpp::simtime_t t =  omnetpp::simTime();
    int LLID = -1;
    for(int i = 0; i < int(queue.size()); i++)
        if(front(i) != NULL)
        {
            DataFrame* data = omnetpp::check_and_cast<DataFrame*>(front(i));
            if (data->getRxTime() < t)
                t = data->getRxTime();
            LLID = i;
        }
    if(LLID == -1) return NULL;
    else return front(LLID);
}

omnetpp::cPacket* MultiQueue::front(int LLID) {
    return queue[LLID]->front();
}

omnetpp::cPacket* MultiQueue::back(int LLID) {
    return queue[LLID]->back();
}

} /* namespace epon */
