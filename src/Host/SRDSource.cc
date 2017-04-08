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

#include "SRDSource.h"

namespace epon {
Define_Module(SRDSource);

SRDSource::SRDSource() {

}

SRDSource::~SRDSource() {

}

void SRDSource::initialize() {
    load = dblpar("load");
    meanOnPackets = (intpar("maxFrameSize") + intpar("minFrameSize")) / 2;
    if(load > 0) meanOffPackets = meanOnPackets * (1 - load) / load;
    //std::cerr << "meanOnPackets=" << meanOnPackets << " meanOffPackets=" << meanOffPackets << " load=" << load << " l=" << ((1 - load) / load) << std::endl;

    Source::initialize();

    //debug
}

int64_t SRDSource::getOnLength() {
    return 1;
}

int64_t SRDSource::getOffLength() {
    return 1;
}

int64_t SRDSource::getOnPacketBitLength() {
    return intuniform(intpar("minFrameSize"), intpar("maxFrameSize"));
}

int64_t SRDSource::getOffPacketBitLength() {
    return exponential(meanOffPackets);
}

} /* namespace epon */
