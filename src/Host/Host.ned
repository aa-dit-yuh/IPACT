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

package epon.Host;
import epon.EPONModule;
import epon.Host.*;

module Host
{
    parameters:
        int LLID = default(-1);
        int hostDatarate @unit(bps) = default(0);
        int sourceNumber = default(0);
        double load = default(0.0);
        double EFWeight = default(0);
        int EFMaxSize @unit(b) = default(0);
        int EFMinSize @unit(b) = default(0);
        double AFWeight = default(0);
        int AFMaxSize @unit(b) = default(0);
        int AFMinSize @unit(b) = default(0);
        double BEWeight = default(0);
        int BEMaxSize @unit(b) = default(0);
        int BEMinSize @unit(b) = default(0);

        //set submodules parameters
        int EFNumber = max(sourceNumber * EFWeight,1);
        int AFNumber = max(sourceNumber * AFWeight,1);
        int BENumber = max(sourceNumber * BEWeight,1);
        int n = EFNumber + AFNumber + BENumber;
        EFSource[*].LLID = LLID;
        EFSource[*].load = load * EFWeight / EFNumber;
        AFSource[*].LLID = LLID;
        AFSource[*].load = load * AFWeight / AFNumber;
        BESource[*].LLID = LLID;
        BESource[*].load = load * BEWeight / BENumber;
        @display("bgb=392,266");
    gates:
        //input and output for Internal network interface
        input rxSplitter;
        output txSplitter;
    types:
        channel Cable extends ned.DatarateChannel
        {
            delay = 0;
            datarate = hostDatarate;
        }
    submodules:
        sink: Sink {
            @display("p=303,62");
        }
        EFSource[EFNumber]: <> like Source {
            Priority = 1;
            maxFrameSize = EFMaxSize;
            minFrameSize = EFMinSize;
            @display("p=186,62");
        }
        AFSource[AFNumber]: <> like Source {
            Priority = 2;
            maxFrameSize = AFMaxSize;
            minFrameSize = AFMinSize;
            @display("p=79,183");
        }
        BESource[BENumber]: <> like Source {
            Priority = 3;
            maxFrameSize = BEMaxSize;
            minFrameSize = BEMinSize;
            @display("p=303,183");
        }
        dispatcher: Dispatcher {
            inputs = n;
            @display("p=186,183");
        }
    connections:
        for i=0..EFNumber-1 {
            EFSource[i].out --> Cable --> dispatcher.in[i];
        }

        for i=0..AFNumber-1 {
            AFSource[i].out --> Cable --> dispatcher.in[EFNumber + i];
        }

        for i=0..BENumber-1 {
            BESource[i].out --> Cable --> dispatcher.in[EFNumber + AFNumber + i];
        }

        dispatcher.out --> txSplitter;
        rxSplitter --> sink.in;
}
