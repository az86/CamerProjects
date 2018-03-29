using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CyUSB;

namespace ChannelCurves
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            //DataContext = Config.UIConfigs.Instance;
            //Load();
        }

        //void Load()
        //{
        //    curvesGrid.RowDefinitions[0].Height = Config.UIConfigs.Instance.CB0IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);
        //    curvesGrid.RowDefinitions[1].Height = Config.UIConfigs.Instance.CB1IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);
        //    curvesGrid.RowDefinitions[2].Height = Config.UIConfigs.Instance.CB2IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);
        //    curvesGrid.RowDefinitions[3].Height = Config.UIConfigs.Instance.CB3IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);
        //    curvesGrid.RowDefinitions[4].Height = Config.UIConfigs.Instance.CB4IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);
        //    curvesGrid.RowDefinitions[5].Height = Config.UIConfigs.Instance.CB5IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);
        //    curvesGrid.RowDefinitions[6].Height = Config.UIConfigs.Instance.CB6IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);
        //    curvesGrid.RowDefinitions[7].Height = Config.UIConfigs.Instance.CB7IsChecked ? new GridLength(1.0, GridUnitType.Star) : new GridLength(0);

        //    DataDistributer.Instance.SetWorkStatus(ch0, Config.UIConfigs.Instance.CB0IsChecked);
        //    DataDistributer.Instance.SetWorkStatus(ch1, Config.UIConfigs.Instance.CB1IsChecked);
        //    DataDistributer.Instance.SetWorkStatus(ch2, Config.UIConfigs.Instance.CB2IsChecked);
        //    DataDistributer.Instance.SetWorkStatus(ch3, Config.UIConfigs.Instance.CB3IsChecked);
        //    DataDistributer.Instance.SetWorkStatus(ch4, Config.UIConfigs.Instance.CB4IsChecked);
        //    DataDistributer.Instance.SetWorkStatus(ch5, Config.UIConfigs.Instance.CB5IsChecked);
        //    DataDistributer.Instance.SetWorkStatus(ch6, Config.UIConfigs.Instance.CB6IsChecked);
        //    DataDistributer.Instance.SetWorkStatus(ch7, Config.UIConfigs.Instance.CB7IsChecked);

        //    stopBtn.IsEnabled = false;
        //}

        //private void CheckBoxStatusChanged(object sender, RoutedEventArgs e)
        //{
        //    Load();
        //}

        //private void StartXferButton_Click(object sender, RoutedEventArgs e)
        //{
        //    if (devicesBox.SelectedDeviceIndex == -1)
        //    {
        //        MessageBox.Show("请选择设备！");
        //        return;
        //    }
        //    try
        //    {
        //        DataDistributer.Instance.Start();
        //        _isRun = true;
        //        _thd = new Thread(() =>
        //        {
        //            var dev = devicesBox.SelectedUSBDevice as CyUSBDevice;
        //            if (dev != null)
        //            {
        //                const int bufLen = 4096;
        //                const int bufCount = 1024;
        //                var cache = new byte[bufCount * bufLen];
        //                var buf = new byte[bufLen];

        //                for (int j = 0; j != 1000 && _isRun; j++)
        //                {
        //                    for (int i = 0; i != bufCount; i++)
        //                    {
        //                        int recvLen = bufLen;
        //                        if (dev.BulkInEndPt.XferData(ref buf, ref recvLen))
        //                        {
        //                            Buffer.BlockCopy(buf, 0, cache, bufLen * i, bufLen);
        //                        }
        //                    }
        //                    DataDistributer.Instance.AddData(cache);
        //                }
        //            }
        //            Console.WriteLine("Xfer thread exited");
        //        });
        //        _thd.IsBackground = true;
        //        //_thd.Priority = ThreadPriority.Highest;
        //        _thd.Start();
        //        startBtn.IsEnabled = false;
        //        stopBtn.IsEnabled = true;
        //    }
        //    catch (Exception ex)
        //    {
        //        DataDistributer.Instance.Stop();
        //        Console.WriteLine(ex.Message);
        //    }
        //}

        //private void StopButtonClick(object sender, RoutedEventArgs e)
        //{
        //    DataDistributer.Instance.Stop();
        //    _isRun = false;
        //    _thd.Join();
        //    Console.WriteLine("Xfer thread exited");
        //    stopBtn.IsEnabled = false;
        //    startBtn.IsEnabled = true;

        //}

        private void ExitButtonClick(object sender, RoutedEventArgs e)
        {
            Environment.Exit(0);
        }
    }
}