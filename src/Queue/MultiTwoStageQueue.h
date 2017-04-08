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

#ifndef MULTITWOSTAGEQUEUE_H_
#define MULTITWOSTAGEQUEUE_H_
#include <TwoStageQueue.h>

namespace epon {

class MultiTwoStageQueue : public cTDMPON {
public:
    MultiTwoStageQueue();
    virtual ~MultiTwoStageQueue();

    void initialize();

    //forward
    void forward(int LLID,int priority);
    bool stepForward(int LLID,int priority);

    //
    int getS2Length(int LLID);
    int getS2Length(int LLID,int priority);
    int getS2Size(int LLID);
    int getS2Size(int LLID,int priority);

    //insertion and removal functions
    void insert(int LLID, omnetpp::cPacket *packet);
    omnetpp::cPacket* remove(int LLID, omnetpp::cPacket *packet);
    omnetpp::cPacket* pop(int LLID);
    omnetpp::cPacket* pop(int LLID,int priority);
    omnetpp::cPacket* popS2(int LLID);
    omnetpp::cPacket* popS2(int LLID,int priority);

    //query functions
    bool isEmpty(int LLID);
    bool isEmpty(int LLID,int priority);
    int getLength(int LLID);
    int getLength(int LLID,int priority);
    int64_t getBitLength(int LLID);
    int64_t getBitLength(int LLID,int priority);
    omnetpp::cPacket* front(int LLID);
    omnetpp::cPacket* s2front(int LLID);
    omnetpp::cPacket* s1front(int LLID);
    omnetpp::cPacket* s1front(int LLID,int priority);
    omnetpp::cPacket* back(int LLID);
    omnetpp::cPacket* s2back(int LLID);

    //debug
protected:
    //components
    std::vector<TwoStageQueue*> queue;
};

} /* namespace epon */
#endif /* MULTITWOSTAGEQUEUE_H_ */
