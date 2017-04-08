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

#include "Analyzer.h"

namespace epon {
Define_Module(Analyzer);

Analyzer::Analyzer() {

}

Analyzer::~Analyzer() {

}

void Analyzer::initialize() {
    //analysis
    currentONU = noCurrentONU;
    noResidualFrames = std::vector<DataFrame*>();
    queues = std::vector<Queue*>();
    channels = std::vector<omnetpp::cDatarateChannel*>();
}

void Analyzer::handleMessage(omnetpp::cMessage* msg) {

}

void Analyzer::finish() {

}

//analysis
void Analyzer::setCurrentONU(int LLID, omnetpp::simtime_t tStart) {
    DataFrame* frame = noResidualFrames.back();
    while(currentONU == noCurrentONU && LLID != noCurrentONU && noResidualFrames.size() > 0)
    {
        omnetpp::simtime_t r = tStart - frame->getRxTime();
        //if(r < 0) throw omnetpp::cRuntimeError("setCurrentONU negative residual time");
        frame->setVacation(true);
        frame->setResidualTime(r);
        noResidualFrames.pop_back();
    }
    currentONU = LLID;
}

void Analyzer::setONU(int LLID,Queue* queue, omnetpp::cDatarateChannel* txChannel) {
    queues.push_back(queue);
    channels.push_back(txChannel);
}

void Analyzer::addResidualTime(DataFrame* frame) {
    if(currentONU == noCurrentONU) noResidualFrames.push_back(frame);
    else
    {
        omnetpp::simtime_t r = channels[currentONU]->getTransmissionFinishTime() - frame->getRxTime();
        if(r < 0)
        {
            frame->setVacation(true);
            frame->setResidualTime(-r);
        }
        else frame->setResidualTime(r);
    }

}

void Analyzer::addQueueTime(DataFrame* frame) {
    omnetpp::simtime_t qts = 0;
    for(int i = 0;i < intpar("numONUs");++i) qts += queues[i]->getBitLength() / channels[i]->getDatarate();
    frame->setQueueTime(qts);
}

} /* namespace epon */
