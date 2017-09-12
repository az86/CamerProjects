using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CyCamera
{
    class ImageStream
    {
        private readonly Queue<byte[]> _imageFrames = new Queue<byte[]>();

        private readonly Semaphore _seamForCahce = new Semaphore(0, 1024 * 1024);

        private readonly Semaphore _seamForImageFrame = new Semaphore(0, 1024 * 1024);

        private const int StaticHeaderLen = 30;

        private const int VarHeaderLen = 2;

        private const int HeaderLen = StaticHeaderLen + VarHeaderLen;

        private readonly int _imageFrameLength;

        private static readonly byte[] StandHeader = new byte[StaticHeaderLen];

        private volatile bool _isRuning;

        private Thread _thd;

        private readonly int _xferFrameLen;
        private readonly byte[] _cache;
        private int _cacheEndCursor;
        private int _cacheStartCursor;
        private int _cacheDataCount;

        public ImageStream(int imageFrameLength, int xferFrameLen)
        {
            _xferFrameLen = xferFrameLen;
            _cache = new byte[xferFrameLen * 512];
            _imageFrameLength = imageFrameLength;
        }

        public void Enqueue(byte[] data)
        {
            if (_cacheEndCursor + data.Length > _cache.Length)
                _cacheEndCursor = 0;
            Buffer.BlockCopy(data, 0, _cache, _cacheEndCursor, _xferFrameLen);
            _cacheEndCursor += _xferFrameLen;
            _seamForCahce.Release();
        }

        public byte[] GetOneImageFrame()
        {
            byte[] frame = null;
            if (_seamForImageFrame.WaitOne())
            {
                lock (_imageFrames)
                {
                    frame = _imageFrames.Dequeue();
                    Console.WriteLine("img one");
                }
            }
            return frame;
        }

        public void Play()
        {
            _isRuning = true;
            _thd = new Thread(Work) { IsBackground = true, Priority= ThreadPriority.Highest};
            _thd.Start();
        }

        private byte[] GetOneFrame()
        {
            var lframe = new byte[_imageFrameLength];
            while (_seamForCahce.WaitOne())
            {
                _cacheDataCount += _xferFrameLen;
                if (_cacheDataCount < _imageFrameLength + HeaderLen)
                    continue;
                var headerOffset = FindHeader(_cache, _cacheStartCursor, _cacheStartCursor + _cacheDataCount);
                if (headerOffset > 0)
                {
                var copyEndCursor = headerOffset + _imageFrameLength;
                    _cacheDataCount -= copyEndCursor;

                }

                if (copyEndCursor < _cache.Length)
                {
                    Buffer.BlockCopy(_cache, _cacheStartCursor, lframe, 0, _imageFrameLength);
                    _cacheStartCursor = copyEndCursor;
                    _cacheDataCount -= _imageFrameLength + HeaderLen;
                    return lframe;
                }
                _cacheStartCursor = copyEndCursor - _cache.Length;
            }
            return lframe;
        }

        private void Work()
        {
            while (_isRuning)
            {
                var frame = GetOneFrame();
                lock (_imageFrames)
                {
                    _imageFrames.Enqueue(frame);
                }
                _seamForImageFrame.Release();
            }
        }


        private static int FindHeader(byte[] frame, int begin, int end)
        {
            var r = MemMem(frame, begin, end, StandHeader);
            return r > 0 ? r + HeaderLen : int.MinValue;
        }

        private static int MemMem(byte[] src, int begin, int end, byte[] stand)
        {
            for(var i = begin; i< end - stand.Length; i++)
            {
                if (Memcmp(src, i, stand) == 0)
                    return i;
                i++;
            }
            return int.MinValue;
        }

        private static int Memcmp(byte[] src, int offset, byte[] stand)
        {
            if (src.Length - offset < stand.Length)
                return int.MinValue;
            for (var i = 0; i != stand.Length; i++)
            {
                if (src[i + offset] != stand[i])
                    return src[i] - stand[i];
            }
            return 0;
        }
    }
}
