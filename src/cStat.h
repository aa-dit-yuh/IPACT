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

#ifndef CSTAT_H_
#define CSTAT_H_
#include <omnetpp.h>

namespace epon {

class cStat : public omnetpp::cNamedObject {
public:
    cStat(std::string name,std::vector<std::string> title);
    virtual ~cStat();

    //static void addUpArrivalRate(double rate);
    //static void addDownArrivalRate(double rate);

    void collect(int i,omnetpp::simtime_t value);
    void collect(int i,double value);

    double getCount(int i);
    double getMean(int i);
    double getStddev(int i);
    double getSum(int i);

    void open(std::string description,bool showTitle);
    void close();
    bool exist(const char *file);
    std::string getTag();
protected:
    static double upArrRateSum,downArrRateSum;
    static std::map<std::string,int> count,c;
    static std::map<std::string,std::vector<double> > sum;
    int index;
    FILE* output;
    std::string name,separator;
    std::vector<std::string> title;
    std::vector<omnetpp::cStdDev*> data,temp;
    std::vector<omnetpp::simtime_t> tRecord;
};

} /* namespace epon */
#endif /* CSTAT_H_ */
