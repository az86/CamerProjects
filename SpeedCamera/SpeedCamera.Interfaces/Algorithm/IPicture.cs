using System.Collections.Generic;

namespace SpeedCamera.Interfaces.Algorithm
{
    public interface IPicture
    {
        System.Drawing.Bitmap Picture { get; set; }

        ILaserPoint GreenPoint { get; set; }

        ILaserPoint RedPoint { get; set; }
    }
}
