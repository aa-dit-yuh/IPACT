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

#ifndef ANALYZER_H_
#define ANALYZER_H_
#include "cTDMPON.h"
#include "Queue.h"

namespace epon {

class Analyzer : public cTDMPON {
public:
    Analyzer();
    virtual ~Analyzer();

    void initialize();
    void handleMessage(omnetpp::cMessage* msg);
    void finish();

    //analysis
    void setCurrentONU(int LLID, omnetpp::simtime_t tStart);
    void setONU(int LLID,Queue* queue, omnetpp::cDatarateChannel* txChannel);

    void addResidualTime(DataFrame* frame);
    void addQueueTime(DataFrame* frame);

    const static int noCurrentONU = -1;
protected:
    //analysis
    int currentONU;
    std::vector<DataFrame*> noResidualFrames;
    std::vector<Queue*> queues;
    std::vector<omnetpp::cDatarateChannel*> channels;
};

} /* namespace epon */
#endif /* ANALYZER_H_ */
