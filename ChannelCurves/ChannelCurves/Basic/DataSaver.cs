using System;

namespace ChannelCurves.Basic
{
    class DataSaver : IDisposable
    {

        System.IO.FileStream _fs;

        int _offset;
        
        public DataSaver(string path)
        {
            _fs = new System.IO.FileStream(path, System.IO.FileMode.Create);
        }

        public void Dispose()
        {
            ((IDisposable)_fs).Dispose();
        }

        public void WriteAsync(byte[] buf)
        {
           var offset = System.Threading.Interlocked.Add(ref _offset, buf.Length);
            _fs.WriteAsync(buf, offset - buf.Length, buf.Length);
        }
    }
}
