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

#include "Splitter.h"

namespace epon {
Define_Module(Splitter);

Splitter::Splitter() {

}

Splitter::~Splitter() {

}

void Splitter::initialize() {
    gate("rxOLT")->setDeliverOnReceptionStart(true);
    for(int i = 0;i < intpar("numONUs");i++) gate("rxONU",i)->setDeliverOnReceptionStart(true);
    upFrames = 0;
    downFrames = 0;
    upBits = new double[intpar("FramePriorities")];
    downBits = new double[intpar("FramePriorities")];
    for(int i = 0;i < intpar("FramePriorities");++i)
    {
        upBits[i] = 0;
        downBits[i] = 0;
    }
}

void Splitter::handleMessage(omnetpp::cMessage* msg) {
    if(msg->arrivedOn("rxOLT"))
    {
        Frame* frame = omnetpp::check_and_cast<Frame*>(msg);
        if(frame->getKind() != 0)
        {
            downBits[frame->getKind()] += frame->getBitLength();
            ++downFrames;
        }
        //if(frame->getDST() == intpar("debug") && strcmp(getName(),"splitter") == 0) if(boolpar("err")) std::cerr << "[splitter::downstream] t=" <<  omnetpp::simTime() << " LLID=" << frame->getDST() << " kind=" << frame->getKind() << " size=" << frame->getBitLength() << std::endl;
        if(boolpar("multicast"))
            for(int i = 0;i < intpar("numONUs");i++) sendDelayed(i == intpar("numONUs") - 1 ? msg : msg->dup(),0,"txONU",i);
        else
        {
            sendDelayed(msg,0,"txONU",frame->getDST());
        }
    }
    else if(msg->arrivedOn("rxONU"))
    {
        Frame* frame = omnetpp::check_and_cast<Frame*>(msg);
        if(frame->getKind() != 0)
        {
            upBits[frame->getKind()] += frame->getBitLength();
            ++upFrames;
        }
        //if(frame->getDST() == intpar("debug") && strcmp(getName(),"splitter") == 0) if(boolpar("err")) std::cerr << "[splitter::upstream] t=" <<  omnetpp::simTime() << " LLID=" << frame->getDST() << " kind=" << frame->getKind() << " size=" << frame->getBitLength() << std::endl;
/*
        if(strcmp(getName(),"splitter") == 0)
            std::cerr << "Splitter::sendToOLT t=" <<  omnetpp::simTime()
                    << " fromLLID=" << frame->getSRC()
                    << " name=" << msg->getName()
                    << " kind=" << msg->getKind()
                    << " size=" << frame->getBitLength()
                    << " end=" <<  (omnetpp::simTime() + frame->getBitLength() / 1000000000.0) << std::endl;
*/
        sendDelayed(msg,0,"txOLT");
    }
}

void Splitter::finish() {
    if(strcmp(getName(),"splitter") == 0)
    {
        recordScalar("0upstream arrival rate",upFrames / omnetpp::simTime(),"");
        recordScalar("1upstream EF bits sum",upBits[1],"");
        recordScalar("2upstream AF bits sum",upBits[2],"");
        recordScalar("3upstream BE bits sum",upBits[3],"");
        recordScalar("4upstream bits sum",upBits[1] + upBits[2] + upBits[3],"");
        recordScalar("5downstream arrival rate",downFrames / omnetpp::simTime(),"");
        recordScalar("6downstream EF bits sum",downBits[1],"");
        recordScalar("7downstream AF bits sum",downBits[2],"");
        recordScalar("8downstream BE bits sum",downBits[3],"");
        recordScalar("9downstream bits sum",downBits[1] + downBits[2] + downBits[3],"");
    }
}

} /* namespace epon */
