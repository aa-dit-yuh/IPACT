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

#ifndef LRDSOURCE_H_
#define LRDSOURCE_H_
#include <Source.h>

namespace epon {

class LRDSource : public Source {
public:
    LRDSource();
    virtual ~LRDSource();
    void initialize();

protected:
    int64_t getOnLength();
    int64_t getOffLength();
    int64_t getOnPacketBitLength();
    int64_t getOffPacketBitLength();

    //parameters
    double hurst,alphaOn,betaOn,alphaOff,betaOff;
};

} /* namespace epon */
#endif /* LRDSOURCE_H_ */
