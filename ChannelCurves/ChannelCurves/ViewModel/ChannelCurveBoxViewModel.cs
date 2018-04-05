using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.ViewModel
{
    class ChannelCurveBoxViewModel
    {
        public ChannelCurveViewModel[] _channelCurveViewModels = new[] { new ChannelCurveViewModel(0), new ChannelCurveViewModel(1) };
        public ChannelCurveViewModel ChannelCurveViewModel0 { get { return _channelCurveViewModels[0]; } }
        public ChannelCurveViewModel ChannelCurveViewModel1 { get { return _channelCurveViewModels[1]; } }

        public ChannelCurveBoxViewModel()
        {
        }
        public int ChannelsCount { get; set; } = 2;

        public void ShowData(byte[] buf)
        {
            Console.WriteLine("show data length {0}", buf.Length);
            var tmp = new ushort[buf.Length / 2];
            Parallel.For(0, buf.Length / 2, i =>
            {
                  tmp[i] = BitConverter.ToUInt16(buf, i * 2);
            });

            int chIndex = 0;
            Console.WriteLine("show data (short) length {0}", tmp.Length);
            for(int i = 0; i< tmp.Length; i++)
            {
                _channelCurveViewModels[chIndex++].Sample(tmp[i]);
                if (chIndex >= ChannelsCount)
                    chIndex = 0;
            }
        }

        public void SetShowSamplingRate(int n)
        {
            foreach(var ccvm in _channelCurveViewModels)
            {
                ccvm.SamplingRate = n;
            }
        }

        public void Restart()
        {
            foreach (var ccvm in _channelCurveViewModels)
            {
                ccvm.Restart();
            }
        }
    }
}
