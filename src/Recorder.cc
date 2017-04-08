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

#include "Recorder.h"

namespace epon {
Define_Module(Recorder);

Recorder::Recorder() {

}

Recorder::~Recorder() {

}

void Recorder::initialize() {
    //parameters

    //frame arrival rate
    upArrivalRate = 0;
    downArrivalRate = 0;

}

void Recorder::handleMessage(omnetpp::cMessage* msg) {

}

void Recorder::finish() {

    //convert to ONU load
    upArrivalRate = upArrivalRate / getAncestorPar("numONUs").doubleValue();
    downArrivalRate = downArrivalRate / getAncestorPar("numONUs").doubleValue();

    //calculate average for all data
    for(std::map<std::string,std::vector< std::vector<double> > >::iterator i = data.begin();i != data.end();++i)
    {
        std::string k = i->first;
        output(k,getDataSheetAverage(k));
    }

}

void Recorder::addUpArrivalRate(double FrameArrivalRate) {
    upArrivalRate += FrameArrivalRate;
}

void Recorder::addDownArrivalRate(double FrameArrivalRate) {
    downArrivalRate += FrameArrivalRate;
}

void Recorder::setTag(std::string tag) {
    this->tag = tag;
}

void Recorder::addRow(std::string tag,std::vector<std::string> titleRow) {
    title[tag] = titleRow;
}

void Recorder::addDataRow(std::string tag,std::vector<double> row) {
    if(data.find(tag) == data.end())
    {
        std::vector< std::vector<double> > dataSheet = std::vector< std::vector<double> >();
        dataSheet.push_back(row);
        data[tag] = dataSheet;
    }
    else
    {
        data[tag].push_back(row);
    }
}

std::vector<double> Recorder::getDataSheetAverage(std::string tag) {
    std::vector<double> avg;
    if(data[tag].size() == 0) throw omnetpp::cRuntimeError("[Recorder::getDataSheetAverage] data sheet is empty");
    int rows = data[tag].size(),cols = data[tag].at(0).size();
    avg.assign(cols,0);
    for(int i = 0;i < cols;++i)
    {
        for(int j = 0;j < rows;++j) avg[i] += data[tag].at(j).at(i);
        avg[i] = avg[i] / rows;
    }
    return avg;
}

void Recorder::output(std::string tag,std::vector<double> data) {
    //file name
    const char *file;
    std::string str;
    std::stringstream ss(str);
    ss << tag << "_" << tag << ".csv";
    str = ss.str();
    file = str.c_str();

    //open output file
    bool b = exist(file);
    FILE *output = fopen(file,"a");
    if(!b)
    {
        //write title
        fprintf(output,"up\tdown");
        if(!(title.find(tag) == title.end())) for(int i = 0;i < int(title[tag].size());++i) fprintf(output,"\t%s",title.at(tag).at(i).c_str());
        fprintf(output,"\n");
    }
    //write data
    fprintf(output,"%12f\t%12f",upArrivalRate,downArrivalRate);
    for(int i = 0;i < int(data.size());++i) fprintf(output,"\t%le",data[i]);
    fprintf(output,"\n");

    fclose(output);
}

bool Recorder::exist(const char *file)
{
    if(FILE *f = fopen(file,"r"))
    {
        fclose(f);
        return true;
    }
    else return false;
}

} /* namespace epon */
