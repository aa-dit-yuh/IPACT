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

#ifndef TWOSTAGEQUEUE_H_
#define TWOSTAGEQUEUE_H_
#include "cTDMPON.h"

namespace epon {

class TwoStageQueue : public cTDMPON {
public:
    TwoStageQueue();
    virtual ~TwoStageQueue();

    void initialize();
    void handleMessage (omnetpp::cMessage* msg);

    //forward functions
    void setCycle (omnetpp::simtime_t t);
    void setDC(int priority, omnetpp::simtime_t t);
    void forward(int priority);
    bool stepForward(int priority);
    virtual omnetpp::simtime_t getFwdCycle(int priority);
    virtual omnetpp::simtime_t getRemainTime();

    //
    int getS2Length();
    int getS2Length(int priority);
    int getS2Size();
    int getS2Size(int priority);

    //insertion and removal functions
    void insert(omnetpp::cPacket *packet);
    omnetpp::cPacket* remove(omnetpp::cPacket *packet);
    omnetpp::cPacket* pop();
    omnetpp::cPacket* pop(int priority);
    omnetpp::cPacket* popS2();
    omnetpp::cPacket* popS2(int priority);

    //query functions
    bool isEmpty();
    bool isEmpty(int priority);
    int getLength();
    int getLength(int priority);
    int64_t getBitLength();
    int64_t getBitLength(int priority);
    omnetpp::cPacket* front();
    omnetpp::cPacket* front(int priority);
    omnetpp::cPacket* s2front();
    omnetpp::cPacket* s1front();
    omnetpp::cPacket* s1front(int priority);
    omnetpp::cPacket* back();
    omnetpp::cPacket* s2back();

    //debug
    void priorityCheck(int priority);
    bool testFwd;
protected:
    //parameters
    int p;
    int* s2Length;
    int* s2Size;

    //forward
    omnetpp::simtime_t tCycle;
    std::vector <omnetpp::simtime_t> tDC;
    std::vector <omnetpp::simtime_t> tNextFwd;
    std::vector<double> rAdjust;
    std::vector<omnetpp::cStdDev*> sdDelay;

    //components
    std::vector<omnetpp::cPacketQueue*> queue;
};

} /* namespace epon */
#endif /* TWOSTAGEQUEUE_H_ */
