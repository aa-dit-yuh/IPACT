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

namespace epon;

enum FrameTag
{
    TagNormal = 0;
    TagDebug = 1;
    TagFront = 2;
    TagBack = 3;
    TagFrontAndBack = 4;
}

enum Response
{
    NOSIGNAL = -1;
  	NONE = 0;
    ACK = 1;
    NACK = 2;
    CS = 3;
    DS = 4;
};

packet Frame
{
	int64_t DST;
	int64_t SRC;
	simtime_t rxTime;
	simtime_t txTime;
	
	//debug
	int tag @enum(FrameTag) = TagNormal;
}

packet DataFrame extends Frame
{
	//delay performance analysis
	bool vacation = false;
    simtime_t residualTime = 0;
    simtime_t queueTime = 0;
	
	//
	bool forwarded = false;
	simtime_t s1Delay = 0;
	simtime_t burstTime = 0;
	
	//simtime_t AFwdDelay = 0;
	//simtime_t AS1Delay = 0;
	//simtime_t AS2Delay = 0;
	//simtime_t ABurstTime = 0;
}

packet MPCPGate extends Frame 
{
    simtime_t startTime;
    simtime_t length;
}

packet MPCPReport extends Frame
{
    int requiredBandwidth;
}

packet SLAGate extends MPCPGate 
{
    //simtime_t pollingCycleTime;
    int signal @enum(Response);
    simtime_t sleepTime;
    simtime_t wakeUpTime;
}

packet SLAReport extends MPCPReport
{
    int signal @enum(Response);
    //simtime_t nextFwdTime = 0;
}

packet CSLAGate extends Frame
{
    //simtime_t pollingCycleTime;
    int Grant1TransceiverMode @enum(Response);
    simtime_t Grant1StartTime;
    simtime_t Grant1Length;
    simtime_t Grant2StartTime;
    simtime_t Grant2Length;
}

packet CSLAReport extends Frame
{
    int Request1Length;
    //simtime_t NextFwdTime;
}

packet GBAGate extends MPCPGate 
{
    simtime_t ONUSleepTime;
}

packet GBAReport extends MPCPReport
{

}
