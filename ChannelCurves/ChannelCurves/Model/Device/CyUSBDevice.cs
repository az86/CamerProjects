namespace ChannelCurves.Model.Device
{
    class CyUSBDevice : AbstractDevice
    {
        private CyUSB.CyUSBDevice _cyUsbDev;

        public CyUSBDevice(CyUSB.CyUSBDevice cyUsbDev)
        {
            _cyUsbDev = cyUsbDev;
        }

        protected override byte[] OnXferData()
        {
            const int bufLen = 4096;
            const int bufCount = 16;
            var cache = new byte[bufCount * bufLen];
            var buf = new byte[bufLen];

            for (int i = 0; i < bufCount; i++)
            {
                int recvLen = bufLen;
                if (_cyUsbDev.BulkInEndPt.XferData(ref buf, ref recvLen))
                {
                    System.Buffer.BlockCopy(buf, 0, cache, bufLen * i, bufLen);
                }
            }
            return cache;
        }
    }
}
