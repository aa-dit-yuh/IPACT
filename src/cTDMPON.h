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

#ifndef CTDMPON_H_
#define CTDMPON_H_
#include <omnetpp.h>
#include "mpcp_m.h"
#include "Recorder.h"
#include "cStat.h"

namespace epon {

class cTDMPON : public omnetpp::cSimpleModule {
public:
    cTDMPON();
    virtual ~cTDMPON();

    Recorder* rec();
    void outputLog();

    bool boolpar(const char* parname);
    int intpar(const char* parname);
    double dblpar(const char* parname);
    omnetpp::simtime_t timepar(const char* parname);
    omnetpp::simtime_t getOnlineMaxWindow();
    omnetpp::simtime_t getOfflineMaxWindow();

    double getUpONULambda(int priority);
    double getDownONULambda(int priority);
    double getUniformFirstMoment(double min,double max);
    double getUpFirstMoment(int priority);
    double getDownFirstMoment(int priority);
    double getUniformSecondMoment(double min,double max);
    double getUniformVariance(double min,double max);
    double getUpFirstMoment();
    double getDownFirstMoment();
    double getUpSecondMoment();
    double getDownSecondMoment();

    //statistics
    void addUpArrivalRate(double FrameArrivalRate);
    void addDownArrivalRate(double FrameArrivalRate);

    //
    std::string concat(std::string dest,std::string src);

    //const
    enum OutputDelay{outputNormal,outputSLA};

    //parameters
    std::stringstream log;
};

} /* namespace epon */
#endif /* CTDMPON_H_ */
