using ChannelCurves.Basic;
using System.Collections.Generic;
using System.Threading;

namespace ChannelCurves.Model
{
    abstract class AbstractDataReceiver
    {
        public delegate void OnDataReceive(byte[] buf);

        public OnDataReceive OnDataArriveEvent;

        private Queue<byte[]> _buffers = new Queue<byte[]>();

        private bool _isRun;

        private Thread _thXfer;

        private Thread _thDispatch;

        private System.Threading.Semaphore _seam;

        protected abstract byte[] OnXferData();

        public void StartAsync()
        {

            if (_isRun)
            {
                this.WriteLine("DataReceiver has been running.");
                return;
            }

            _buffers.Clear();
            _isRun = true;
            _seam = new Semaphore(0, 0xFFFF);
            _thXfer = new Thread(() => CoreStartXfer()) { IsBackground = true };
            _thDispatch = new Thread(() => CoreStartDispatch()) { IsBackground = true };
            _thDispatch.Start();
            _thXfer.Start();
        }

        private byte[] Pop()
        {
            _seam.WaitOne();
            byte[] buf;
            lock(_buffers)
            {
                buf = _buffers.Dequeue();
            }
            return buf;
        }
        private void CoreStartDispatch()
        {
            while(_isRun)
            {
                var tmp = Pop();
                OnDataArriveEvent?.Invoke(tmp);
            }
        }

        private void CoreStartXfer()
        {
            while (_isRun)
            {
                var buf = OnXferData();
                if (buf == null)
                {
                    this.WriteLine("XferData error");
                    continue;
                }
                lock (_buffers)
                {
                    _buffers.Enqueue(buf);
                }
                _seam.Release();
            }
        }

        public void Stop()
        {
            _isRun = false;
            if (_thDispatch != null)
            {
                _thDispatch.Join();
                this.WriteLine("Dispath thread join.");
            }

            if (_thXfer != null)
            {
                _thXfer.Join();
                this.WriteLine("Xfer thread join.");
            }
        }
    }
}
