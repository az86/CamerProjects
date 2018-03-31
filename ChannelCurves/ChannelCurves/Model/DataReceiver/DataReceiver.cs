using CyUSB;
using System;
using System.Collections.Generic;
using ChannelCurves.Basic;
using System.Threading;
using ChannelCurves.Model.Device;

namespace ChannelCurves.Model.DataReceiver
{
    class DataReceiver : AbstractDataReceiver
    {
        AbstractDevice _dev;

        public DataReceiver(AbstractDevice dev)
        {
            Dev = dev;
        }

        internal AbstractDevice Dev { get => _dev; set => _dev = value; }

        protected override byte[] OnXferData()
        {
            return Dev.XferData();
        }

    }
}