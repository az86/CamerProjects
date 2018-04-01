using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.Model.Device
{
    class MockDevice : AbstractDevice
    {
        int _startIndex;
        protected override byte[] OnXferData()
        {
            var buf = new byte[1024*16];
            for(short i = 0;i < buf.Length/2; i++)
            {
                var tmp = BitConverter.GetBytes((ushort)((Math.Sin(_startIndex++/1000.0) + 1) *0x7FFF));
                if ((i & 1) == 0)
                {
                    buf[i * 2] = tmp[0];
                    buf[i * 2 + 1] = tmp[1];
                }
                else
                {
                    buf[i * 2] =  0;
                    buf[i * 2 + 1] = 0xFF;
                }
            }
            System.Threading.Thread.Sleep(10);
            return buf;
        }
    }
}
