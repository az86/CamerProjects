using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ChannelCurves
{

    public enum DataBitLengths
    {
        Single,
        Double
    }

    internal class DataDistributer
    {
        public static readonly DataDistributer Instance = new DataDistributer();

        private readonly Queue<byte[]> _bufs = new Queue<byte[]>();

        private System.Threading.Semaphore _sema;

        private readonly Controls.ChannelCurve[] _channels = new Controls.ChannelCurve[8];
        public DataBitLengths DataBitLength { get; set; }

        private volatile bool _isRun;

        private long _sumDealed;

        private long _sumCached;

        private Thread _thd;
        private DataDistributer()
        {
            DataBitLength = DataBitLengths.Single;
        }

        public void Start()
        {
            _isRun = true;

            _sema = new System.Threading.Semaphore(0, 0xFFFFF);
            _thd = new System.Threading.Thread(DispatchData) { IsBackground = true };
            _thd.Start();
            DataBitLength = Config.UIConfigs.Instance.SelecteResolutionIndex == 0
                ? DataBitLengths.Single
                : DataBitLengths.Double;
        }

        public void Stop()
        {
            Console.WriteLine("Stop");
            _isRun = false;
          //  _thd.Join();
            Console.WriteLine("Stop jion");

            if (_sema != null)
            {
                _sema.Close();
                _sema.Dispose();
                _bufs.Clear();
            }
            Console.WriteLine("ThreadPool task DataDisstributer worker thread exited!");
            _sumDealed = 0;
            _sumCached = 0;
        }

        public void SetWorkStatus(Controls.ChannelCurve curve, bool isWroking)
        {
            var row = (int)curve.GetValue(System.Windows.Controls.Grid.RowProperty);
            if (isWroking)
                _channels[row] = curve;
            else
                _channels[row] = null;
        }

        public void AddData(byte[] buf)
        {
            if (_isRun)
            {
                var cache = buf.Clone();
                lock (_bufs)
                {
                    _bufs.Enqueue(buf);
                }
                _sumCached += buf.Length;
                Console.WriteLine("{0} data added. Sum cached:{1}", buf.Length, _sumCached);
                _sema.Release();
            }
        }


        private void DispatchData()
        {
            int channelIndex = 0;
            var workingChannels = _channels.Where(ch => ch != null).ToArray();

            foreach (var ch in workingChannels)
            {
            }

            var saver = new DataSaver();
            saver.Start();
            while (_isRun)
            {
                if (!_sema.WaitOne(10))
                    continue;
                byte[] buf = null;
                lock (_bufs)
                {
                    buf = _bufs.Dequeue();
                }
                _sumDealed += buf.Length;
                Console.WriteLine("{0} data got, Sum dealed:{1}", buf.Length, _sumDealed);

                for (int i = 0; i < buf.Length;)
                {
                    if (DataBitLength == DataBitLengths.Double)
                    {
                        var val = (uint)buf[i] + 0x100 * (uint)buf[i + 1];
                        i += 2;
                      //  workingChannels[channelIndex].AddValue((int)val);
                        saver.WriteValue((int)val);
                    }
                    else
                    {
                        var val = (ushort)buf[i++];
                      //  workingChannels[channelIndex].AddValue(val);
                        saver.WriteValue(val);
                    }
                    channelIndex++;
                    if (channelIndex == workingChannels.Count())
                        channelIndex = 0;

                }
            }
            Console.WriteLine("DataDistributer start saving data");
            saver.Stop();
            Console.WriteLine("DataDisstributer worker thread exited!");
        }
    }
}
