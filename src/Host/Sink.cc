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

#include "Sink.h"

namespace epon {
Define_Module(Sink);

Sink::Sink() {

}

Sink::~Sink() {
    //debug
    delete sPacketSize;
    delete sDelay;
    delete sSleepDelay;
    delete sS1Delay;
    delete sS2Delay;
    delete sFwdDelay;
    delete sBurstTime;
}

void Sink::initialize() {
    //statistics
    std::vector<std::string> t;
    t.push_back("Average");
    t.push_back("EF");
    t.push_back("AF");
    t.push_back("BE");
    if(strcmp(getParentModule()->getName(),"service") == 0)
    {
        sPacketSize = new cStat("PacketSizeUp",t);
        sDelay = new cStat("DelayUp",t);
        sSleepDelay = new cStat("SleepDelayUp",t);
        sS1Delay = new cStat("S1DelayUp",t);
        sS2Delay = new cStat("S2DelayUp",t);
        sFwdDelay = new cStat("FwdDelayUp",t);
        sBurstTime = new cStat("BurstTimeUp",t);
    }
    else if(strcmp(getParentModule()->getName(),"user") == 0)
    {
        sPacketSize = new cStat("PacketSizeDown",t);
        sDelay = new cStat("DelayDown",t);
        sSleepDelay = new cStat("SleepDown",t);
        sS1Delay = new cStat("S1DelayDown",t);
        sS2Delay = new cStat("S2DelayDown",t);
        sFwdDelay = new cStat("FwdDelayDown",t);
        sBurstTime = new cStat("BurstTimeDown",t);
    }
}

void Sink::handleMessage (omnetpp::cMessage* msg) {
    //handle data packet from ONU or OLT
    DataFrame* data = omnetpp::check_and_cast<DataFrame*>(msg);
    int priority = data->getKind();

    sPacketSize->collect(0,data->getBitLength());
    sPacketSize->collect(priority,data->getBitLength());

    omnetpp::simtime_t d = data->getTxTime() - data->getRxTime();
    sDelay->collect(0,d);
    sDelay->collect(priority,d);
    if(data->getTimestamp() == intpar("SLEEP"))
    {
        sSleepDelay->collect(0,d);
        sSleepDelay->collect(priority,d);
    }
    sS1Delay->collect(0,data->getS1Delay());
    sS1Delay->collect(priority,data->getS1Delay());
    sS2Delay->collect(0,data->getTxTime() - data->getRxTime() - data->getS1Delay());
    sS2Delay->collect(priority,data->getTxTime() - data->getRxTime() - data->getS1Delay());
    if(data->getTag() == TagFront || data->getTag() == TagFrontAndBack)
    {
        sFwdDelay->collect(0,data->getTxTime() - data->getRxTime() - data->getS1Delay());
        sFwdDelay->collect(priority,data->getTxTime() - data->getRxTime() - data->getS1Delay());
    }
    if(data->getTag() == TagBack || data->getTag() == TagFrontAndBack)
    {
        sBurstTime->collect(0,data->getBurstTime());
        sBurstTime->collect(priority,data->getBurstTime());
    }
    delete msg;
}

void Sink::finish() {
    //get average packet delay
    omnetpp::simtime_t t =  omnetpp::simTime() - timepar("tProp");
    double packets = sPacketSize->getCount(0);
    double packetSum = sPacketSize->getSum(0);

    recordScalar("received bits",packetSum,"b");
    recordScalar("frame arrival rate",packets / t,"");
    recordScalar("throughput",packetSum / (dblpar("cPON") * t.dbl()),"");
}

} /* namespace epon */
