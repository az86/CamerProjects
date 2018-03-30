using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using CyUSB;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;

namespace ChannelCurves.View
{
    /// <summary>
    /// DevicesBox.xaml 的交互逻辑
    /// </summary>
    public partial class DevicesBox : UserControl
    {
        private readonly ObservableCollection<string> _Devices = new ObservableCollection<string>();
        public ObservableCollection<string> Devices { get { return _Devices; } }

        private readonly USBDeviceList _usbDevices = new USBDeviceList(CyConst.DEVICES_CYUSB);

        public USBDevice SelectedUSBDevice { get { return _usbDevices[SelectedDeviceIndex]; } }
        public int SelectedDeviceIndex { get; set; }

        public DevicesBox()
        {
            InitializeComponent();

            _usbDevices.DeviceAttached += UsbDevices_DeviceAttached;
            _usbDevices.DeviceRemoved += UsbDevices_DeviceRemoved;
            foreach (CyUSBDevice dev in _usbDevices)
            {
                Devices.Add(dev.FriendlyName);
            }
            if (Devices.Any())
                cb.SelectedIndex = Devices.Count() - 1;
        }
        private void UsbDevices_DeviceRemoved(object sender, EventArgs e)
        {
                Devices.Clear();
                foreach (CyUSBDevice dev in _usbDevices)
                {
                    Devices.Add(dev.FriendlyName);
                }
        }

        private void UsbDevices_DeviceAttached(object sender, EventArgs e)
        {
            var args = e as CyUSB.USBEventArgs;
            if (args != null)
                Devices.Add(args.Device.FriendlyName);

            if (Devices.Any())
                cb.SelectedIndex = Devices.Count() - 1;
        }
    }
}
