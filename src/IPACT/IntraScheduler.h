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

#ifndef INTRASCHEDULER_H_
#define INTRASCHEDULER_H_
#include <Scheduler.h>
#include <Queue.h>

namespace epon {

class IntraScheduler : public Scheduler {
public:
    IntraScheduler();
    virtual ~IntraScheduler();

    virtual void initialize();
    virtual void finish();

    virtual void handleSelfMessage (omnetpp::cMessage* msg);
    virtual void sendMPCP();
    virtual void handleMessageFromSplitter (omnetpp::cMessage* msg);
    virtual void handleMessageFromHost (omnetpp::cMessage* msg);

    virtual void handleGate(MPCPGate* gate);
    virtual void handleDataFromSplitter(Frame* data);
    virtual void handleDataFromHost(Frame* data);

    //query
    int getLLID();

    //intra scheduling
    //enum IntraSchduling{STRICT,NONSTRICT};
protected:
    //parameter
    omnetpp::simtime_t tStart,tWindow;

    //components
    Queue* queue;
};

} /* namespace epon */
#endif /* INTRASCHEDULER_H_ */
