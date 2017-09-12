using AZ.Basic.Graphic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SpeedCamera.Algorithm
{

    [Serializable]
    public class Settings : SpeedCamera.Interfaces.Algorithm.ISettings
    {
        public Color GreenMax { get; set; }
        public Color GreenMin { get; set; }
        public Color RedMax { get; set; }
        public Color RedMin { get; set; }
        public Color CanvasSize { get; set; }
    }
}
