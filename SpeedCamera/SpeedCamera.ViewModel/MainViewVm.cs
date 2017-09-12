using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SpeedCamera.Interfaces.Algorithm;

namespace SpeedCamera.ViewModel
{
    public class MainViewVm
    {
        public CameraControlVm CameraControlVm { get; set; }

        public SettingsVm SettingsVm { get; set; }

        public MainViewVm(IPictureStream pictureStream)
        {
            CameraControlVm = new CameraControlVm();
            CameraControlVm.SearchDevices();
            SettingsVm = new SettingsVm(pictureStream.Settings);
        }
    }
}
