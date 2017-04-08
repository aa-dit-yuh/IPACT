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

#include "Dispatcher.h"

namespace epon {
Define_Module(Dispatcher);

Dispatcher::Dispatcher() {
}

Dispatcher::~Dispatcher() {
    delete sdPacketSize;
    delete sArrival;
}

void Dispatcher::initialize() {
    //paremeters
    tScheduled = 0;
    sdPacketSize = new  omnetpp::cStdDev("sdPacketSize");
    //txSplitter = omnetpp::check_and_cast<omnetpp::cDatarateChannel*>(getParentModule()->gate("txSplitter")->getChannel());

    //debug
    //std::cerr << getParentModule()->getName() << getParentModule()->getIndex() << " load=" << this->getAncestorPar("load").doubleValue() << std::endl;
}

void Dispatcher::handleMessage (omnetpp::cMessage* msg) {
    if(msg->arrivedOn("in"))
    {
        omnetpp::cPacket* packet = omnetpp::check_and_cast<omnetpp::cPacket*>(msg);
        sdPacketSize->collect(packet->getBitLength());
        //if (omnetpp::simTime() < txSplitter->getTransmissionFinishTime()) sendDelayed(msg,txSplitter->getTransmissionFinishTime() -  omnetpp::simTime(),"out");
        //else send(msg,"out");

        //schedule packets
        if (omnetpp::simTime() < tScheduled)
        {
            sendDelayed(msg,tScheduled -  omnetpp::simTime(),"out");
            tScheduled += packet->getBitLength() / dblpar("cEth");
        }
        else
        {
            send(msg,"out");
            tScheduled =  omnetpp::simTime() + packet->getBitLength() / dblpar("cEth");
        }
    }
}

void Dispatcher::finish() {
    std::vector<std::string> t;
    t.push_back("UpONUArrivalRate");
    t.push_back("DownONUArrivalRate");
    t.push_back("UpONUReceiverBits");
    t.push_back("DownONUReceiverBits");
    t.push_back("UpONUSystemLoad");
    t.push_back("DownONUSystemLoad");
    sArrival = new cStat("Arrival",t);

    if(strcmp(getParentModule()->getName(),"service") == 0)
    {
        rec()->addDownArrivalRate(sdPacketSize->getCount() /  omnetpp::simTime().dbl());
        sArrival->collect(1,sdPacketSize->getCount() /  omnetpp::simTime().dbl());
        sArrival->collect(3,sdPacketSize->getSum());
        sArrival->collect(5,sdPacketSize->getSum() /  (omnetpp::simTime().dbl() * intpar("cPON")));
    }
    else if(strcmp(getParentModule()->getName(),"user") == 0)
    {
        rec()->addUpArrivalRate(sdPacketSize->getCount() /  omnetpp::simTime().dbl());
        sArrival->collect(0,sdPacketSize->getCount() /  omnetpp::simTime().dbl());
        sArrival->collect(2,sdPacketSize->getSum());
        sArrival->collect(4,sdPacketSize->getSum() /  (omnetpp::simTime().dbl() * intpar("cPON")));
    }

    recordScalar("sent bits",sdPacketSize->getSum(),"b");
    recordScalar("frame departure rate",sdPacketSize->getCount() /  omnetpp::simTime().dbl(),"");
    recordScalar("load",sdPacketSize->getSum() /  (omnetpp::simTime().dbl() * getAncestorPar("cPON").doubleValue()),"");
}

} /* namespace epon */
