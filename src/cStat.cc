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

#include "cStat.h"

namespace epon {
double cStat::upArrRateSum = 0;
double cStat::downArrRateSum = 0;
std::map<std::string,int> cStat::count;
std::map<std::string,int> cStat::c;
std::map<std::string,std::vector<double> > cStat::sum;

cStat::cStat(std::string name,std::vector<std::string> title) {
    cNamedObject(name.c_str(),true);
    index = count[name];
    count[name]++;
    c[name] = count[name];
    if(sum.find(name) == sum.end()) sum[name] = std::vector<double>(int(title.size()),0);
    this->name = name;
    this->title = title;
    separator = ",";
    for(int i = 0;i < int(title.size());++i)
    {
        data.push_back(new omnetpp::cStdDev("data"));
        temp.push_back(new omnetpp::cStdDev("data"));
    }
    tRecord.assign(int(title.size()),0);
}

cStat::~cStat() {
    //collect vector sum of the same name
    for(int i = 0;i < int(sum[name].size());++i)
    {
        //std::cerr << name << "." << title[i] << "=" << data[i]->getMean() << std::endl;
        sum[name].at(i) += data[i]->getMean();
    }

    //write vector file
    open("vector",true);
    for(int i = 0;i < int(data.size());++i) fprintf(output,"%s%le",separator.c_str(),temp[i]->getCount());
    fprintf(output,"%s%d%s%s",separator.c_str(),index,separator.c_str(),"count");
    fprintf(output,"%s%s%s%d",separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveConfigName(),separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveRunNumber());
    close();
    open("vector",true);
    for(int i = 0;i < int(data.size());++i) fprintf(output,"%s%le",separator.c_str(),temp[i]->getMean());
    fprintf(output,"%s%d%s%s",separator.c_str(),index,separator.c_str(),"mean");
    fprintf(output,"%s%s%s%d",separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveConfigName(),separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveRunNumber());
    close();
    open("vector",true);
    for(int i = 0;i < int(data.size());++i) fprintf(output,"%s%le",separator.c_str(),temp[i]->getStddev());
    fprintf(output,"%s%d%s%s",separator.c_str(),index,separator.c_str(),"stddev");
    fprintf(output,"%s%s%s%d",separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveConfigName(),separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveRunNumber());
    close();
    open("vector",true);
    for(int i = 0;i < int(data.size());++i) fprintf(output,"%s%le",separator.c_str(),temp[i]->getSum());
    fprintf(output,"%s%d%s%s",separator.c_str(),index,separator.c_str(),"sum");
    fprintf(output,"%s%s%s%d",separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveConfigName(),separator.c_str(),omnetpp::getEnvir()->getConfigEx()->getActiveRunNumber());
    close();


    //write average file
    c[name]--;
    if(c[name] == 0)
    {
        open("average",true);
        for(int i = 0;i < int(data.size());++i) fprintf(output,"%s%le",separator.c_str(),sum[name].at(i) / count[name]);
        fprintf(output,"\t%s\t%d",omnetpp::getEnvir()->getConfigEx()->getActiveConfigName(),omnetpp::getEnvir()->getConfigEx()->getActiveRunNumber());
        close();
    }
    for(int i = 0;i < int(data.size());++i)
    {
        delete data[i];
        delete temp[i];
    }
}
/*
void cStat::addUpArrivalRate(double rate) {
    upArrRateSum += rate;
}

void cStat::addDownArrivalRate(double rate) {
    downArrRateSum += rate;
}
*/
void cStat::collect(int i, omnetpp::simtime_t value) {
    collect(i,value.dbl());
}

void cStat::collect(int i,double value) {
    if(omnetpp::simTime() >= omnetpp::getSimulation()->getWarmupPeriod()) data[i]->collect(value);
    temp[i]->collect(value);
    if(i == 0 && index == 0 && (omnetpp::simTime() - tRecord[i]) > 1)
    {
        open("timeline",true);
        for(int j = 0;j < int(data.size());++j)
        {
            fprintf(output,"\t%le",temp[j]->getMean());
            temp[j]->clearResult();
        }
        fprintf(output,"\t%d\t%d",int(omnetpp::simTime().dbl()), omnetpp::getEnvir()->getConfigEx()->getActiveRunNumber());
        close();
        tRecord[i] = omnetpp::simTime();
    }
}

double cStat::getCount(int i) {
    return data[i]->getCount();
}

double cStat::getMean(int i) {
    return data[i]->getMean();
}

double cStat::getStddev(int i) {
    return data[i]->getStddev();
}

double cStat::getSum(int i) {
    return data[i]->getSum();
}

void cStat::open(std::string description,bool showTitle) {
    //file name
    const char *file;
    std::string str;
    std::stringstream ss(str);
    ss << "_" << name << "_" << getTag() << "_" << description << ".csv";
    str = ss.str();
    file = str.c_str();

    //open output file
    bool b = exist(file);
    output = fopen(file,"a");
    if(!b && showTitle)
    {
        //write title
        fprintf(output,"up%sdown",separator.c_str());
        for(int i = 0;i < int(title.size());++i) fprintf(output,"%s%s",separator.c_str(),title[i].c_str());
        fprintf(output,"\n");
    }
    omnetpp::cModule* s = omnetpp::getSimulation()->getSystemModule();
    double u = s->par("upLoad").doubleValue();
    double d = s->par("downLoad").doubleValue();
    fprintf(output,"%8f\t%8f",u,d);
}

void cStat::close() {
    fprintf(output,"\n");
    fclose(output);
}

bool cStat::exist(const char *file)
{
    if(FILE *f = fopen(file,"r"))
    {
        fclose(f);
        return true;
    }
    else return false;
}

std::string cStat::getTag() {
    std::stringstream sstag;
    omnetpp::cModule* s = omnetpp::getSimulation()->getSystemModule();
    sstag << "RTT" << (s->par("tProp").doubleValue() * 2000) << "ms_rth" << s->par("remainThreshold").doubleValue() << "_EF" << s->par("tEFDC").doubleValue() << "_AF" << s->par("tAFDC").doubleValue() << "_BE" << s->par("tBEDC").doubleValue();
    return sstag.str();
}
} /* namespace epon */
