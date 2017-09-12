using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SpeedCamera.Interfaces.Algorithm;

namespace SpeedCamera.Algorithm
{
    public class PictureStream : IPictureStream
    {
        public List<IPicture> Pictures
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        private Settings _settings = new Algorithm.Settings();

        public ISettings Settings
        {
            get { return _settings; }
            set { _settings = value as Settings; }
        }

        public void Calculate()
        {
            throw new NotImplementedException();
        }
    }
}
