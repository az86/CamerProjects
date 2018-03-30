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
            const int bufLen = 16 * 1024;
            int recvLen = bufLen;
            var buf = new byte[bufLen];
            if (_cyUsbDev.BulkInEndPt.XferData(ref buf, ref recvLen))
            {
                return buf;
            }
            return null;
        }
    }
}
