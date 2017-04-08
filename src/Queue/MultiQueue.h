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

#ifndef MULTIQUEUE_H_
#define MULTIQUEUE_H_
#include <Queue.h>

namespace epon {

class MultiQueue : public cTDMPON {
public:
    MultiQueue();
    virtual ~MultiQueue();

    void initialize();

    void setNonStrict(int LLID,bool NonStrict);
    void setPopForwarded(int LLID,bool b);

    //forward
    void forward(int LLID,int priority);
    int getForwardLength(int LLID);
    int getForwardLength(int LLID,int priority);
    int getForwardSize(int LLID);
    int getForwardSize(int LLID,int priority);

    //insertion and removal functions
    void insert(omnetpp::cPacket *packet);
    omnetpp::cPacket* remove(omnetpp::cPacket *packet);
    omnetpp::cPacket* pop();
    omnetpp::cPacket* pop(int LLID);

    //query functions
    bool isEmpty();
    bool isEmpty(int LLID);
    bool isEmptyByPriority(int LLID,int priority);
    int getLength();
    int getLength(int LLID);
    int getLengthByPriority(int LLID,int priority);
    int64_t getBitLength();
    int64_t getBitLength(int LLID);
    int64_t getBitLengthByPriority(int LLID,int priority);
    omnetpp::cPacket* front();
    omnetpp::cPacket* front(int LLID);
    omnetpp::cPacket* back(int LLID);
protected:
    //parameter
    //components
    std::vector<Queue*> queue;
};

} /* namespace epon */
#endif /* MULTIQUEUE_H_ */
