using ChannelCurves.Basic;
using System.Collections.Generic;
using System.Threading;

namespace ChannelCurves.Model
{
    abstract class AbstractDataReceiver
    {
        private Queue<byte[]> _buffers = new Queue<byte[]>();

        private bool _isRun;

        private Thread _th;

        private System.Threading.Semaphore _seam = new Semaphore(0, 0xFFFF);

        protected abstract byte[] OnXferData();

        public void StartAsync()
        {
            _th = new Thread(() => CoreStart());
        }

        public byte[] Pop()
        {
            _seam.WaitOne();
            byte[] buf;
            lock(_buffers)
            {
                buf = _buffers.Dequeue();
            }
            return buf;
        }
        private void CoreStart()
        {
            if (_isRun)
            {
                this.WriteLine("DataReceiver has been running.");
                return;
            }
            _isRun = true;
            while (_isRun)
            {
                const int bufLen = 16 * 1024;
                var buf = new byte[bufLen];
                var data = OnXferData();
                if (data == null)
                {
                    this.WriteLine("XferData error");
                    continue;
                }
                lock (_buffers)
                {
                    _buffers.Enqueue(buf);
                }
            }
        }

        void Stop()
        {
            _isRun = false;
            if (_th != null)
                _th.Join();
        }
    }
}
