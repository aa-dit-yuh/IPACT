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

#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#include "cTDMPON.h"

namespace epon {

class Scheduler : public cTDMPON {
public:
    Scheduler();
    ~Scheduler();

    virtual void initialize();
    virtual void finish();

    void handleMessage(omnetpp::cMessage* msg);
    virtual void handleSelfMessage(omnetpp::cMessage* msg);
    virtual void handleMessageFromSplitter(omnetpp::cMessage* msg);
    virtual void handleMessageFromHost(omnetpp::cMessage* msg);

    virtual void send(omnetpp::cMessage* msg,const char *gatename);
    virtual void sendDataUntil(omnetpp::simtime_t tUntil);

    virtual void handleDataBeforeSend(DataFrame* data);

    //query
    virtual omnetpp::simtime_t getTransmissionTime(int bitLength);
    virtual bool isTxSplitterBusy();
    virtual omnetpp::simtime_t getTxSplitterSchedTime();
    int getLLID();
    virtual omnetpp::cPacket* getNextDataFrame();
    virtual omnetpp::cPacket* popNextDataFrame();
    virtual bool queueEmpty();

    //set
    virtual void setRxTimeStamp(DataFrame* data);
    virtual void setTxTimeStamp(DataFrame* data, omnetpp::simtime_t txTimeStamp);

    //components
    omnetpp::cDatarateChannel* txSplitter;
protected:
    //parameters
    omnetpp::simtime_t tMPCP,tGuard,RTT;

    //components
    omnetpp::cMessage* msgSendUntil;

    //statistics
    omnetpp::cStdDev* sdPacketSize;
};

} /* namespace epon */
#endif /* SCHEDULER_H_ */
