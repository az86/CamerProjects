using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ChannelCurves
{
    class DataSaver
    {
        private const string _basicFilePath = @"Data\{0}.txt";

        private readonly Queue<int[]> _buffers = new Queue<int[]>();

        private const int _bufferLen = 1024 * 1024 * 100;

        private int[] _buffer = new int[_bufferLen];

        private int _cacheCount;

        private Semaphore _sema;

        private volatile bool _isRun;

        string _path;

        Thread _thd;
        public void Start()
        {
            _sema = new Semaphore(0, 0xFFFF);
            _isRun = true;
            _thd = new Thread(CoreWrite) {IsBackground = true};
            _thd.Start();
        }

        public void Stop()
        {

            while (true)
            {
                lock(_buffers)
                {
                    if (_buffers.Count() == 0)
                        break;
                    Thread.Sleep(10);
                }
            }

            _isRun = false;
            if (_sema != null)
            {
                _sema.Close();
                _sema.Dispose();
            }
            _buffers.Clear();
            _thd.Join();
            Console.WriteLine("Worker thread saved ok and joined!");
            Console.WriteLine("Main thread save start!{0}", _cacheCount);
            System.IO.File.AppendAllLines(_path, _buffer.Take(_cacheCount).Select(val => val.ToString()));
            Console.WriteLine("Main thread save OK!");
            _cacheCount = 0;
        }

        public void WriteValue(int val)
        {
            _buffer[_cacheCount++] = val;
            if (_cacheCount == _bufferLen)
            {
                lock (_buffers)
                {
                    _buffers.Enqueue(_buffer);                    
                }
                _sema.Release();
                _cacheCount = 0;
                _buffer = new int[_bufferLen];
            }
        }

        private void CoreWrite()
        {
            _path = string.Format(_basicFilePath, DateTime.Now.ToString().Replace('/', '_').Replace(':', '_'));
            var dir = System.IO.Path.GetDirectoryName(_path);
            if (!string.IsNullOrEmpty(dir))
                System.IO.Directory.CreateDirectory(dir);
            using (var sw = new System.IO.StreamWriter(_path))
            {
                while (_isRun)
                {
                    if (_sema.WaitOne(1000))
                    {
                        int[] buffer = null;
                        lock (_buffers)
                        {
                            buffer = _buffers.Dequeue();
                        }
                        for (var i = 0; i < buffer.Length; i += 8)
                        {
                            sw.Write("{0},{1},{2},{3},{4},{5},{6},{7}",
                                buffer[i], buffer[i + 1], buffer[i + 2], buffer[i + 3],
                                buffer[i + 4], buffer[i + 5], buffer[i + 6], buffer[i + 7]);
                        }

                        // System.IO.File.AppendAllLines(_path, buffer.Select(val => val.ToString()));

                        Console.WriteLine("Worker thread saved 100MB data!");
                    }
                }
            }
            Console.WriteLine("Worker thread saved OK!");
        }
    }
}
