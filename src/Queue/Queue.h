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

#ifndef QUEUE_H_
#define QUEUE_H_
#include "cTDMPON.h"

namespace epon {

class Queue : public cTDMPON {
public:
    Queue();
    virtual ~Queue();

    void initialize();

    void setNonStrict(bool NonStrict);
    void setPopForwarded(bool b);

    //forward
    void forward(int priority);
    int getForwardLength();
    int getForwardLength(int priority);
    int getForwardSize();
    int getForwardSize(int priority);

    //insertion and removal functions
    void insert(omnetpp::cPacket *packet);
    omnetpp::cPacket* remove(omnetpp::cPacket *packet);
    omnetpp::cPacket* pop();
    omnetpp::cPacket* popByPriority(int priority);

    //query functions
    bool isEmpty();
    bool isEmptyByPriority(int priority);
    int getLength();
    int getLengthByPriority(int priority);
    int64_t getBitLength();
    int64_t getBitLengthByPriority(int priority);
    omnetpp::cPacket* front();
    omnetpp::cPacket* back();

    //debug
    void list();
    void enableDebug();
protected:
    //parameters
    bool popForwarded;
    int p;
    bool nonstrict;
    int* forwardLength;
    int* forwardSize;
    double m;

    //components
    std::vector<omnetpp::cPacketQueue*> queue;
};

} /* namespace epon */
#endif /* QUEUE_H_ */
