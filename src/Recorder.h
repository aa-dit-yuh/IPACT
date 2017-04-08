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

#ifndef RECORDER_H_
#define RECORDER_H_
#include <omnetpp.h>
#include "mpcp_m.h"

namespace epon {

class Recorder : public omnetpp::cSimpleModule {
public:
    Recorder();
    virtual ~Recorder();

    void initialize();
    void handleMessage(omnetpp::cMessage* msg);
    void finish();

    void addUpArrivalRate(double FrameArrivalRate);
    void addDownArrivalRate(double FrameArrivalRate);
    void setTag(std::string tag);

    //data operation
    void addRow(std::string tag,std::vector<std::string> titleRow);
    void addDataRow(std::string tag,std::vector<double> row);
    void collect(std::string tag,double value);
    std::vector<double> getDataSheetAverage(std::string tag);
    void output(std::string tag,std::vector<double> value);

    //
    bool exist(const char *file);

    //
    const static int noCurrentONU = -1;
protected:
    //parameters
    double upArrivalRate;
    double downArrivalRate;
    std::string tag;
    std::map<std::string,std::vector< std::vector<double> > > data;
    std::map<std::string,std::vector<omnetpp::cStdDev*> > table;
    std::map<std::string,std::vector<std::string> > title;
};

} /* namespace epon */
#endif /* RECORDER_H_ */
