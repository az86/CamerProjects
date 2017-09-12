using System;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using AZ.Basic;
using CGSDK;

using DeviceHandle = System.IntPtr;

namespace SpeedCamera.ViewModel
{
    public class CameraControlVm
    {
        public ObservableCollection<ComboBoxItem> Cameras { get; set; }

        public ComboBoxItem SelectedCamera { get; set; }

        public Command OpenCommand { get; set; }

        DeviceHandle mDeviceHandle = IntPtr.Zero;
        public CameraControlVm()
        {
            Cameras = new ObservableCollection<ComboBoxItem>();
            OpenCommand = new Command();
            OpenCommand.Invoked += OpenCommand_OnClicked;
        }

        private void OpenCommand_OnClicked(object sender, EventArgs e)
        {
            byte byDev = (byte)((int)SelectedCamera.Value & 0xFF);
            var devStatus = CGAPI.OpenDeviceByUSBAddress(byDev, ref mDeviceHandle);
            CGAPI.DeviceInit(mDeviceHandle, IntPtr.Zero, true, true);
            CGAPI.DeviceStart(mDeviceHandle);
            IntPtr buf = IntPtr.Zero;
            DeviceFrameInfo info = new DeviceFrameInfo();
            CGAPI.DeviceGetImageBuffer(mDeviceHandle, ref buf, 1, ref info);
        }

        public void SearchDevices()
        {
            var devSatus = CGAPI.DeviceInitialSDK(IntPtr.Zero, bUsedStatus: false);
            if (DeviceStatus.STATUS_OK != devSatus) return;
            var iCameraCounts = 0;
            devSatus = CGAPI.EnumDevice(IntPtr.Zero, ref iCameraCounts);
            if (DeviceStatus.STATUS_OK != devSatus) return;
            var ptr = Marshal.AllocHGlobal(Marshal.SizeOf(new EnumDeviceParam()) * iCameraCounts);
            devSatus = CGAPI.EnumDevice(ptr, ref iCameraCounts);
            if (DeviceStatus.STATUS_OK == devSatus)
            {
                for (int i = 0; i < iCameraCounts; i++)
                {
                    var item = new ComboBoxItem();
                    var edp = (EnumDeviceParam)Marshal.PtrToStructure((IntPtr)((int)ptr + i * Marshal.SizeOf(new EnumDeviceParam())), typeof(EnumDeviceParam));
                    var strDevice = string.Format("{0} : {1}", edp.lpDeviceDesc, edp.devIndex);
                    item.Text = strDevice;
                    item.Value = ((edp.devIndex << 8) | edp.usbAddress);
                    Cameras.Add(item);
                }
            }
            Marshal.FreeHGlobal(ptr);
        }
    }


    public class ComboBoxItem
    {
        public string Text { get; set; }
        public object Value { get; set; }

        public override string ToString()
        {
            return Text;
        }
    }
}
