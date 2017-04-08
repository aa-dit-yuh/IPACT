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

#ifndef SINK_H_
#define SINK_H_
#include "cTDMPON.h"

namespace epon {

class Sink : public cTDMPON {
public:
    Sink();
    virtual ~Sink();

    void initialize();
    void handleMessage (omnetpp::cMessage* msg);
    void finish();

    //statistics
    cStat* sPacketSize;
    cStat* sDelay;
    cStat* sSleepDelay;
    cStat* sS1Delay;
    cStat* sS2Delay;
    cStat* sFwdDelay;
    cStat* sBurstTime;

    //std::vector<cStdDev*> sdAvgSize;
    //std::vector<cStdDev*> sdFwdDelay;
    //std::vector<cStdDev*> sdBurstTime;
    //std::vector<cStdDev*> sdS1Delay;
    //std::vector<cStdDev*> sdS2Delay;

    //std::vector<cStdDev*> sdFrontDelay;
    //std::vector<cStdDev*> sdBackDelay;

    //debug
protected:
};

} /* namespace epon */
#endif /* SINK_H_ */
