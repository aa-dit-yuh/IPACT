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

#ifndef SRDSOURCE_H_
#define SRDSOURCE_H_
#include <Source.h>

namespace epon {

class SRDSource : public Source {
public:
    SRDSource();
    virtual ~SRDSource();
    void initialize();

protected:
    int64_t getOnLength();
    int64_t getOffLength();
    int64_t getOnPacketBitLength();
    int64_t getOffPacketBitLength();

    //parameter
    int64_t meanOnPackets,meanOffPackets;
};

} /* namespace epon */
#endif /* SRDSOURCE_H_ */
