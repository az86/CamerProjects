using AForge.Video.DirectShow;
using System;
using System.Drawing;
using System.Windows;

namespace MyAmCap
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private static readonly FilterInfoCollection _videoDevInfos = new FilterInfoCollection(FilterCategory.VideoInputDevice);

        private VideoCaptureDevice _videoDevice;

        private AForge.Video.VFW.AVIWriter _aviWriter;

        private bool _beCapture;

        public MainWindow()
        {
            InitializeComponent();
            foreach (FilterInfo devInfo in _videoDevInfos)
            {
                cbDevices.Items.Add(devInfo.Name);
            }
        }

        private void ConnectBtnClick(object sender, RoutedEventArgs e)
        {
            try
            {
                _videoDevice = new VideoCaptureDevice(_videoDevInfos[cbDevices.SelectedIndex].MonikerString);
            }
            catch (Exception ex)
            {
                MessageBox.Show("设备初始化失败，请重新插入摄像头!");
                MessageBox.Show(ex.Message);
                return;
            }


            if (vsp.IsRunning)
                vsp.Stop();
            vsp.VideoSource = _videoDevice;


            _videoDevice.ProvideSnapshots = true;
            _videoDevice.SnapshotFrame += vs_SnapshotFrame;
            _videoDevice.NewFrame += _videoDevice_NewFrame;
            vsp.Start();


            optionBtn.IsEnabled = true;
            captureBtn.IsEnabled = true;
            recordBtn.IsEnabled = true;
            setBtn.IsEnabled = true;
            frameWidth.Text = _videoDevice.VideoCapabilities[0].FrameSize.Width.ToString();
            frameHeight.Text = _videoDevice.VideoCapabilities[0].FrameSize.Height.ToString();
        }

        private void _videoDevice_NewFrame(object sender, AForge.Video.NewFrameEventArgs eventArgs)
        {
          //  Console.WriteLine("{0}x{1}", eventArgs.Frame.Width, eventArgs.Frame.Height);
            if (_beCapture)
            {
                vs_SnapshotFrame(sender, eventArgs);
                _beCapture = false;
            }
        }

        void vsp_NewFrame(object sender, ref Bitmap image)
        {
            _aviWriter.AddFrame((Bitmap)image.Clone());
        }

        void vs_SnapshotFrame(object sender, AForge.Video.NewFrameEventArgs eventArgs)
        {
            var bmp = eventArgs.Frame.Clone() as Bitmap;
            var dlg = new Microsoft.Win32.SaveFileDialog
            {
                DefaultExt = ".bmp",
                Filter = "BMP files(.bmp)|*.bmp"
            };

            if (dlg.ShowDialog().GetValueOrDefault())
            {
                bmp.Save(dlg.FileName);
            }

        }

        private void OptionBtnClick(object sender, RoutedEventArgs e)
        {
            _videoDevice.DisplayPropertyPage(new IntPtr());
        }

        private void CaptureBtnClick(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("CaptureBtnClick");
            _beCapture = true;
            _videoDevice.SimulateTrigger();
        }

        private void RecordBtnClick(object sender, RoutedEventArgs e)
        {
            try
            {
                var dlg = new Microsoft.Win32.SaveFileDialog
                {
                    DefaultExt = ".avi",
                    Filter = "AVI files(.avi)|*.avi"
                };
                if (dlg.ShowDialog().GetValueOrDefault())
                {
                    _aviWriter = new AForge.Video.VFW.AVIWriter();
                    var bmp = vsp.GetCurrentVideoFrame();
                    _aviWriter.Open(dlg.FileName, bmp.Size.Width, bmp.Size.Height);
                    vsp.NewFrame += vsp_NewFrame;
                    StopRecordBtn.IsEnabled = true;
                    recordBtn.IsEnabled = false;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void StopRecordBtnClick(object sender, RoutedEventArgs e)
        {
            try
            {
                vsp.NewFrame -= vsp_NewFrame;
                System.Threading.Thread.Sleep(2000);
                _aviWriter.Close();
                StopRecordBtn.IsEnabled = false;
                recordBtn.IsEnabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void SetBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _videoDevice.Stop();
                foreach(var vc in _videoDevice.VideoCapabilities)
                {
                    Console.WriteLine("{0}x{1}", vc.FrameSize.Width, vc.FrameSize.Height);
                }
                var s = _videoDevice.VideoCapabilities[0].Clone() as VideoCapabilities;
                s.FrameSize.Width = int.Parse(frameWidth.Text);
                s.FrameSize.Height= int.Parse(frameHeight.Text);
                _videoDevice.VideoResolution = s;
                _videoDevice.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }
    }
}
