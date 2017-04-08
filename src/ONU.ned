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

package epon;
import epon.EPONModule;
import epon.Queue.*;

module ONU extends EPONModule
{
    parameters:
        int LLID;
        int debug = default(-1);
        double tEFDC @unit(s) = default(0);
        double tAFDC @unit(s) = default(0);
        double tBEDC @unit(s) = default(0);
        double pActive @unit(W) = default(0);
        double pSleep @unit(W) = default(0);
        double tOverhead @unit(s) = default(0);
        scheduler.LLID = LLID;
    submodules:
        queue: <> like QueueInterface;
        scheduler: <> like Scheduler;
    connections:
        //connections to OLT
        rxSplitter --> scheduler.rxSplitter;
        scheduler.txSplitter --> txSplitter;

        //connections to user
        rxHost --> scheduler.rxHost;
        scheduler.txHost --> txHost;
}
