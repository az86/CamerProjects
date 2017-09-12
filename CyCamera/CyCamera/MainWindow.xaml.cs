using CyUSB;
using System;
using System.Threading;
using System.Windows;

namespace CyCamera
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly USBDeviceList _usbDevices;

        private long _count;

        private const int XferFrameLen =1024*1024;

        private readonly CyUSBDevice _device;

        private readonly ImageStream _imageStream = new ImageStream(1024*1280, XferFrameLen);

        private readonly Player _player = new Player();

        public MainWindow()
        {
            InitializeComponent();

            var isDeviceOk = false;
            while (!isDeviceOk)
            {
                try
                {
                    _usbDevices = new USBDeviceList(CyConst.DEVICES_CYUSB);
                    _device = _usbDevices[0] as CyUSBDevice;
                    _device.BulkInEndPt.XferSize = XferFrameLen;

                    var buf = new byte[XferFrameLen];
                    var bufLen = XferFrameLen;
                    if (!_device.BulkInEndPt.XferData(ref buf, ref bufLen))
                    {
                        Console.WriteLine("XferData error");
                        throw new Exception("设备为就绪，第一个数据包传输错误！");
                    }
                    isDeviceOk = true;
                }
                catch (Exception ex)
                {
                    Console.WriteLine("请确认设备已就绪! - {0}", ex.Message);
                    Thread.Sleep(1000);
                }
            }

            _player.Play(_imageStream, image);
            _imageStream.Play();
        
            var thd = new Thread(WorkFunc) { IsBackground = true, Priority=ThreadPriority.Highest};
            thd.Start();

            var t = new System.Timers.Timer(1000);
            t.Elapsed += T_Elapsed;
            t.Start();
        }


        private void T_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            Console.WriteLine("{0}: {1}MB per second.", DateTime.Now ,Interlocked.Exchange(ref _count, 0)/1024/1024);
        }

        void WorkFunc()
        {
            while (true)
            {
                var buf = new byte[XferFrameLen];
                var bufLen = XferFrameLen;
                if (!_device.BulkInEndPt.XferData(ref buf, ref bufLen))
                {
                    Console.WriteLine("XferData error");
                }
                else
                {
                    _imageStream.Enqueue(buf);
                    Interlocked.Add(ref _count, bufLen);
                }
            }
        }
    }
}
