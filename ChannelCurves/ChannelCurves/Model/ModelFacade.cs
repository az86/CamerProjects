using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.Model
{
    class ModelFacade
    {
        private DataReceiver.DataReceiver _receiver;

        public DataReceiver.DataReceiver.OnDataReceive OnDataReceiveEvent
        {
            get;set;
        }

        public ModelFacade()
        {
        }

        public void ConnectToCyUSBDevice(int cyUsbIndex)
        {
            var _usbDevices = new CyUSB.USBDeviceList(CyUSB.CyConst.DEVICES_CYUSB);
            var dev = new Model.Device.CyUSBDevice(_usbDevices[cyUsbIndex] as CyUSB.CyUSBDevice);
            _receiver = new DataReceiver.DataReceiver(dev);
            _receiver.OnDataArriveEvent = OnDataReceiveEvent;
        }

        void OnDataArrive(byte[] buf)
        {
            OnDataReceiveEvent?.Invoke(buf);
        }
        internal void Start()
        {
            _receiver.StartAsync();
        }

        public void ConnectToMockDevice()
        {
            var dev = new Model.Device.MockDevice();
            _receiver = new DataReceiver.DataReceiver(dev);
            _receiver.OnDataArriveEvent = OnDataArrive;
        }
    }
}
