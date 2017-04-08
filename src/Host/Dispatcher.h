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

#ifndef DISPATCHER_H_
#define DISPATCHER_H_
#include "cTDMPON.h"

namespace epon {

class Dispatcher : public cTDMPON {
public:
    Dispatcher();
    virtual ~Dispatcher();

    void initialize();
    void handleMessage (omnetpp::cMessage* msg);
    void finish();

protected:
    //paremeters
    omnetpp::simtime_t tScheduled;
     omnetpp::cStdDev* sdPacketSize;
    //cDatarateChannel* txSplitter;
    cStat* sArrival;
};

} /* namespace epon */
#endif /* DISPATCHER_H_ */
