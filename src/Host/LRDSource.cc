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

#include "LRDSource.h"

namespace epon {
Define_Module(LRDSource);

LRDSource::LRDSource() {

}

LRDSource::~LRDSource() {

}

void LRDSource::initialize() {
    //parameters
    load = par("load").doubleValue();
    hurst = par("hurst").doubleValue();
    alphaOn = 3 - 2 * hurst;
    betaOn = 1;
    alphaOff = 1.2;
    betaOff = betaOn * (alphaOn / alphaOff) * ((alphaOff - 1) / (alphaOn - 1)) * ((1 - load) / load);

    Source::initialize();
}

int64_t LRDSource::getOnLength() {
    return pareto_shifted(alphaOn,betaOn,0);
}

int64_t LRDSource::getOffLength() {
    return pareto_shifted(alphaOff,betaOff,0);
}

int64_t LRDSource::getOnPacketBitLength() {
    return intuniform(intpar("minFrameSize"), intpar("maxFrameSize"));
}

int64_t LRDSource::getOffPacketBitLength() {
    return exponential((intpar("minFrameSize") + intpar("maxFrameSize")) / 2);
}

} /* namespace epon */
