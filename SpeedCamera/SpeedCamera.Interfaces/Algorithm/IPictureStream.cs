using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SpeedCamera.Interfaces.Algorithm
{
    public interface IPictureStream
    {
        List<IPicture> Pictures { get; set; }

        ISettings Settings { get; set; }

        void Calculate();
    }
}
