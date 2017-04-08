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

#include "MultiTwoStageQueue.h"

namespace epon {
Define_Module(MultiTwoStageQueue);

MultiTwoStageQueue::MultiTwoStageQueue() {

}

MultiTwoStageQueue::~MultiTwoStageQueue() {
    for(int i = 0;i < intpar("numONUs");i++) delete queue[i];
}

void MultiTwoStageQueue::initialize() {
    for (int i = 0;i < intpar("numONUs");i++)
    {
        TwoStageQueue* q = new TwoStageQueue();
        q->initialize();
        queue.push_back(q);
    }
}

//forward
void MultiTwoStageQueue::forward(int LLID,int priority) {
    queue[LLID]->forward(priority);
}

bool MultiTwoStageQueue::stepForward(int LLID,int priority) {
    return queue[LLID]->stepForward(priority);
}

//
int MultiTwoStageQueue::getS2Length(int LLID) {
    return queue[LLID]->getS2Length();
}

int MultiTwoStageQueue::getS2Length(int LLID,int priority) {
    return queue[LLID]->getS2Length(priority);
}

int MultiTwoStageQueue::getS2Size(int LLID) {
    return queue[LLID]->getS2Size();
}

int MultiTwoStageQueue::getS2Size(int LLID,int priority) {
    return queue[LLID]->getS2Size(priority);
}

//insertion and removal functions
void MultiTwoStageQueue::insert(int LLID, omnetpp::cPacket *packet) {
    queue[LLID]->insert(packet);
}

omnetpp::cPacket* MultiTwoStageQueue::remove(int LLID, omnetpp::cPacket *packet) {
    return queue[LLID]->remove(packet);
}

omnetpp::cPacket* MultiTwoStageQueue::pop(int LLID) {
    return queue[LLID]->pop();
}

omnetpp::cPacket* MultiTwoStageQueue::pop(int LLID,int priority) {
    return queue[LLID]->pop(priority);
}

omnetpp::cPacket* MultiTwoStageQueue::popS2(int LLID) {
    return queue[LLID]->popS2();
}

omnetpp::cPacket* MultiTwoStageQueue::popS2(int LLID,int priority) {
    return queue[LLID]->popS2(priority);
}

//query functions
bool MultiTwoStageQueue::isEmpty(int LLID) {
    return queue[LLID]->isEmpty();
}

bool MultiTwoStageQueue::isEmpty(int LLID,int priority) {
    return queue[LLID]->isEmpty(priority);
}

int MultiTwoStageQueue::getLength(int LLID) {
    return queue[LLID]->getLength();
}

int MultiTwoStageQueue::getLength(int LLID,int priority) {
    return queue[LLID]->getLength(priority);
}

int64_t MultiTwoStageQueue::getBitLength(int LLID) {
    return queue[LLID]->getBitLength();
}

int64_t MultiTwoStageQueue::getBitLength(int LLID,int priority) {
    return queue[LLID]->getBitLength(priority);
}

omnetpp::cPacket* MultiTwoStageQueue::front(int LLID) {
    return queue[LLID]->front();
}

omnetpp::cPacket* MultiTwoStageQueue::s2front(int LLID) {
    return queue[LLID]->s2front();
}

omnetpp::cPacket* MultiTwoStageQueue::s1front(int LLID) {
    return queue[LLID]->s1front();
}

omnetpp::cPacket* MultiTwoStageQueue::s1front(int LLID,int priority) {
    return queue[LLID]->s1front(priority);
}

omnetpp::cPacket* MultiTwoStageQueue::back(int LLID) {
    return queue[LLID]->back();
}

omnetpp::cPacket* MultiTwoStageQueue::s2back(int LLID) {
    return queue[LLID]->s2back();
}
} /* namespace epon */
