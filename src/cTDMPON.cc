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

#include "cTDMPON.h"

namespace epon {

cTDMPON::cTDMPON() {

}

cTDMPON::~cTDMPON() {

}

Recorder* cTDMPON::rec() {
    return omnetpp::check_and_cast<Recorder*>(getParentModule()->getParentModule()->getSubmodule("recorder"));
}

void cTDMPON::outputLog() {
    FILE *output;

    //file name
    const char *file;
    std::string str;
    std::stringstream ss(str);
    ss << getParentModule()->getName() << getParentModule()->getIndex() << "." << getName() << getIndex() << "_log";
    str = ss.str();
    file = str.c_str();
    std::string out;
    out = log.str();

    //output data
    output = fopen(file,"a");
    fprintf(output,"%s\n",out.c_str());
    fclose(output);
}

bool cTDMPON::boolpar(const char* parname) {
    if(hasPar(parname)) return bool(par(parname));
    return bool(getAncestorPar(parname));
}

int cTDMPON::intpar(const char* parname) {
    if(hasPar(parname)) return int(par(parname));
    return int(getAncestorPar(parname));
}

double cTDMPON::dblpar(const char* parname) {
    if(hasPar(parname)) return par(parname).doubleValue();
    return getAncestorPar(parname).doubleValue();
}

omnetpp::simtime_t cTDMPON::timepar(const char* parname) {
    if(hasPar(parname)) return omnetpp::simtime_t(par(parname).doubleValue());
    return omnetpp::simtime_t(getAncestorPar(parname).doubleValue());
}

omnetpp::simtime_t cTDMPON::getOnlineMaxWindow() {
    return (timepar("tCycle") - intpar("numONUs") * (timepar("tGuard") + timepar("MPCPSize") / dblpar("cPON"))) / intpar("numONUs");
}

omnetpp::simtime_t cTDMPON::getOfflineMaxWindow() {
    return (timepar("tCycle") - timepar("tProp") * 2 - intpar("numONUs") * (timepar("tGuard") + timepar("MPCPSize") / dblpar("cPON"))) / intpar("numONUs");
}

double cTDMPON::getUpONULambda(int priority) {
    if(priority == intpar("EF")) return ((dblpar("cPON") * dblpar("upLoad") * dblpar("upEFWeight")) / dblpar("numONUs")) / (0.5 * dblpar("upEFMax") + 0.5 * dblpar("upEFMin"));
    if(priority == intpar("AF")) return ((dblpar("cPON") * dblpar("upLoad") * dblpar("upAFWeight")) / dblpar("numONUs")) / (0.5 * dblpar("upAFMax") + 0.5 * dblpar("upAFMin"));
    if(priority == intpar("BE")) return ((dblpar("cPON") * dblpar("upLoad") * dblpar("upBEWeight")) / dblpar("numONUs")) / (0.5 * dblpar("upBEMax") + 0.5 * dblpar("upBEMin"));
    return 0;
}

double cTDMPON::getDownONULambda(int priority) {
    if(priority == intpar("EF")) return ((dblpar("cPON") * dblpar("downLoad") * dblpar("downEFWeight")) / dblpar("numONUs")) / (0.5 * dblpar("downEFMax") + 0.5 * dblpar("downEFMin"));
    if(priority == intpar("AF")) return ((dblpar("cPON") * dblpar("downLoad") * dblpar("downAFWeight")) / dblpar("numONUs")) / (0.5 * dblpar("downAFMax") + 0.5 * dblpar("downAFMin"));
    if(priority == intpar("BE")) return ((dblpar("cPON") * dblpar("downLoad") * dblpar("downBEWeight")) / dblpar("numONUs")) / (0.5 * dblpar("downBEMax") + 0.5 * dblpar("downBEMin"));
    return 0;
}

double cTDMPON::getUniformFirstMoment(double min,double max) {
    return (min + max) / 2;
}

double cTDMPON::getUpFirstMoment(int priority) {
    if(priority == intpar("EF")) return getUniformFirstMoment(dblpar("upEFMax") / dblpar("cPON"),dblpar("upEFMin") / dblpar("cPON"));
    if(priority == intpar("AF")) return getUniformFirstMoment(dblpar("upAFMax") / dblpar("cPON"),dblpar("upAFMin") / dblpar("cPON"));
    if(priority == intpar("BE")) return getUniformFirstMoment(dblpar("upBEMax") / dblpar("cPON"),dblpar("upBEMin") / dblpar("cPON"));
    return 0;
}

double cTDMPON::getDownFirstMoment(int priority) {
    if(priority == intpar("EF")) return getUniformFirstMoment(dblpar("downEFMax") / dblpar("cPON"),dblpar("downEFMin") / dblpar("cPON"));
    if(priority == intpar("AF")) return getUniformFirstMoment(dblpar("downAFMax") / dblpar("cPON"),dblpar("downAFMin") / dblpar("cPON"));
    if(priority == intpar("BE")) return getUniformFirstMoment(dblpar("downBEMax") / dblpar("cPON"),dblpar("downBEMin") / dblpar("cPON"));
    return 0;
}

double cTDMPON::getUniformSecondMoment(double min,double max) {
    double first = getUniformFirstMoment(min,max);
    double variance = getUniformVariance(min,max);
    return first * first + variance;
}

double cTDMPON::getUniformVariance(double min,double max) {
    return (max - min) * (max - min) / 12;
}

double cTDMPON::getUpFirstMoment() {
    return dblpar("upEFWeight") * getUniformFirstMoment(dblpar("upEFMin") / dblpar("cPON"),dblpar("upEFMax") / dblpar("cPON"))
            + dblpar("upAFWeight") * getUniformFirstMoment(dblpar("upAFMin") / dblpar("cPON"),dblpar("upAFMax") / dblpar("cPON"))
            + dblpar("upBEWeight") * getUniformFirstMoment(dblpar("upBEMin") / dblpar("cPON"),dblpar("upBEMax") / dblpar("cPON"));
}

double cTDMPON::getDownFirstMoment() {
    return dblpar("downEFWeight") * getUniformFirstMoment(dblpar("downEFMin") / dblpar("cPON"),dblpar("downEFMax") / dblpar("cPON"))
            + dblpar("downAFWeight") * getUniformFirstMoment(dblpar("downAFMin") / dblpar("cPON"),dblpar("downAFMax") / dblpar("cPON"))
            + dblpar("downBEWeight") * getUniformFirstMoment(dblpar("downBEMin") / dblpar("cPON"),dblpar("downBEMax") / dblpar("cPON"));
}

double cTDMPON::getUpSecondMoment() {
    return dblpar("upEFWeight") * getUniformSecondMoment(dblpar("upEFMin") / dblpar("cPON"),dblpar("upEFMax") / dblpar("cPON"))
            + dblpar("upAFWeight") * getUniformSecondMoment(dblpar("upAFMin") / dblpar("cPON"),dblpar("upAFMax") / dblpar("cPON"))
            + dblpar("upBEWeight") * getUniformSecondMoment(dblpar("upBEMin") / dblpar("cPON"),dblpar("upBEMax") / dblpar("cPON"));
}

double cTDMPON::getDownSecondMoment() {
    return dblpar("downEFWeight") * getUniformSecondMoment(dblpar("downEFMin") / dblpar("cPON"),dblpar("downEFMax") / dblpar("cPON"))
            + dblpar("downAFWeight") * getUniformSecondMoment(dblpar("downAFMin") / dblpar("cPON"),dblpar("downAFMax") / dblpar("cPON"))
            + dblpar("downBEWeight") * getUniformSecondMoment(dblpar("downBEMin") / dblpar("cPON"),dblpar("downBEMax") / dblpar("cPON"));
}

std::string concat(std::string dest,std::string src) {
    std::string str;
    std::stringstream ss(str);
    ss << dest << src;
    str = ss.str();
    return str;
}

} /* namespace epon */
