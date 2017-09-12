using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
using Emgu.CV;
using Emgu.CV.CvEnum;

namespace SpeedCamera.View
{
    /// <summary>
    /// Interaction logic for Video.xaml
    /// </summary>
    public partial class Video : UserControl
    {
        public Video()
        {
            InitializeComponent();
        }

        public void ProcessFrame(object sender, EventArgs arg)
        {
//             var capture = sender as Emgu.CV.Capture;
//             Mat frame = new Mat();
//             capture.Retrieve(frame, 0);
//             Dispatcher.Invoke(() =>
//             {
//                 img.Source = Emgu.CV.WPF.BitmapSourceConvert.ToBitmapSource(frame);
//               /  capture.c
//             });
        }
    }
}
