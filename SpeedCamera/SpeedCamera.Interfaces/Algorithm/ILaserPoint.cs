using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace SpeedCamera.Interfaces.Algorithm
{
    public interface ILaserPoint
    {
        System.Drawing.Point PositionInPictrue { get; set; }

        System.Drawing.Point PositionInCanvas { get; set; }

        double Speed { get; set; }
    }
}
