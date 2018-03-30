using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.Model.Device
{
    class MockDevice : AbstractDevice
    {
        protected override byte[] OnXferData()
        {
            throw new NotImplementedException();
        }
    }
}
