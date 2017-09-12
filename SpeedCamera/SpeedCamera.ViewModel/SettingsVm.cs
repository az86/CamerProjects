using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SpeedCamera.ViewModel
{
    public class SettingsVm
    {
        private readonly Interfaces.Algorithm.ISettings _settings;

        public SettingsVm(Interfaces.Algorithm.ISettings settings)
        {
            _settings = settings;
        }

        public int RedMinR
        {
            get { return _settings.RedMin.R; }
            set { _settings.RedMin.R = value; }
        }

        public int RedMinG
        {
            get { return _settings.RedMin.G; }
            set { _settings.RedMin.G = value; }
        }

        public int RedMinB
        {
            get { return _settings.RedMin.B; }
            set { _settings.RedMin.B = value; }
        }

        public int RedMaxR
        {
            get { return _settings.RedMax.R; }
            set { _settings.RedMax.R = value; }
        }

        public int RedMaxG
        {
            get { return _settings.RedMax.G; }
            set { _settings.RedMax.G = value; }
        }

        public int RedMaxB
        {
            get { return _settings.RedMax.B; }
            set { _settings.RedMax.B = value; }
        }
      
        public int GreenMinR
        {
            get { return _settings.GreenMin.R; }
            set { _settings.GreenMin.R = value; }
        }

        public int GreenMinG
        {
            get { return _settings.GreenMin.G; }
            set { _settings.GreenMin.G = value; }
        }

        public int GreenMinB
        {
            get { return _settings.GreenMin.B; }
            set { _settings.GreenMin.B = value; }
        }

        public int GreenMaxR
        {
            get { return _settings.GreenMax.R; }
            set { _settings.GreenMax.R = value; }
        }

        public int GreenMaxG
        {
            get { return _settings.GreenMax.G; }
            set { _settings.GreenMax.G = value; }
        }

        public int GreenMaxB
        {
            get { return _settings.GreenMax.B; }
            set { _settings.GreenMax.B = value; }
        }
    }
}
