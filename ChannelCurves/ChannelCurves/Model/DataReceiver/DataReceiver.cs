using CyUSB;
using System;
using System.Collections.Generic;
using ChannelCurves.Basic;
using System.Threading;

namespace ChannelCurves.Model.DataReceiver
{
    class DataReceiver : AbstractDataReceiver
    {
        AbstractDevice _dev;

        public DataReceiver(AbstractDevice dev)
        {
            _dev = dev;
        }

        protected override byte[] OnXferData()
        {
            return _dev.XferData();
        }

    }
}