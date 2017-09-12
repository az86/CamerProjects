using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace CyCamera
{
    class Player
    {
        public const int ImageHeight = 1024;

        public const int ImageWidth = 1280;

        public ImageStream DataSource;

        public System.Windows.Controls.Image Image;

        private volatile bool _isRuning;

        private Thread _thd;

        public void Play(ImageStream ds, System.Windows.Controls.Image image)
        {
            DataSource = ds;
            Image = image;
            _isRuning = true;
            _thd = new Thread(CorePlay) {IsBackground = true};
            _thd.Start();
        }

        public void Stop()
        {
            _isRuning = false;
            _thd.Join();
        }
        private void CorePlay()
        {
            while (_isRuning)
            {
                var frame = DataSource.GetOneImageFrame();
                var m = new Emgu.CV.Mat(ImageHeight, ImageWidth, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
                m.SetTo(frame);
                var m2 = new Emgu.CV.Mat(ImageHeight, ImageWidth, Emgu.CV.CvEnum.DepthType.Cv8U, 3);

                Emgu.CV.CvInvoke.CvtColor(m, m2, Emgu.CV.CvEnum.ColorConversion.BayerRg2Rgb);
                Image.Dispatcher.Invoke(() =>
                {
                    Image.Source = Emgu.CV.WPF.BitmapSourceConvert.ToBitmapSource(m2);
                });
            }
        }
    }
}
