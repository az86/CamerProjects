using ChannelCurves.Basic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace ChannelCurves.ViewModel
{
    class MainViewModel : System.ComponentModel.INotifyPropertyChanged
    {
        public ChannelCurveBoxViewModel ChannelCurveBoxViewModel { get; private set; } = new ChannelCurveBoxViewModel();

        private readonly Model.ModelFacade _model = new Model.ModelFacade();

        public Command SwitchCommand { get; set; } = new Command();

        public Command ChannelSelectCommand { get; set; } = new Command();

        public Command ResolutionSetCommand { get; set; } = new Command();

        public Command NDataShowOneCommand { get; set; } = new Command();

        public Command SwitchSaveFileCommand { get; set; } = new Command();

        public Command CreateSaveFilePathCommand { get; set; } = new Command();

        public int SelectedDeviceIndex { get; set; }

        public int SelectedChannelCount
        {
            get { return ChannelCurveBoxViewModel.ChannelsCount; }
            set
            {
                ChannelCurveBoxViewModel.ChannelsCount = value;
                this.WriteLine("SelectedChannelCount is set to {0}", value);
            }
        }

        private int _resolution = 16;

        public int Resolution
        {
            get => _resolution;
            set
            {
                _resolution = value;
                this.WriteLine("Resolution is set to {0}", value);
            }
        }

        public int NDataShowOne
        {
            get => _nDataShowOne; set
            {
                _nDataShowOne = value;
                this.WriteLine("NDataShowOne is set to {0}", value);
            }
        }

        private bool _isRuning;

        public bool IsRuning
        {
            get => _isRuning;
            set
            {
                _isRuning = value;
                NotifyPropertyChanged();
            }
        }

        private bool _isSavingFile;

        public bool IsSavingFile
        {
            get => _isSavingFile;
            set
            {
                _isSavingFile = value;
                NotifyPropertyChanged();
            }
        }


        public string SaveFilePath
        {
            get => _saveFilePath;
            set
            {
                _saveFilePath = value;
                NotifyPropertyChanged();
            }
        }


        private string _saveFilePath;

        private void NotifyPropertyChanged([System.Runtime.CompilerServices.CallerMemberName] String propertyName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private int _nDataShowOne = 1;

        public event PropertyChangedEventHandler PropertyChanged;

        void OnDataReceive(byte[] buf)
        {
            // ChannelCurveBoxViewModel.ChannelCurveViewModel0.
            ChannelCurveBoxViewModel.ShowData(buf);
        }
        public MainViewModel()
        {
            SwitchCommand.ExecuteEvent = OnSwitchChanged;
            ChannelSelectCommand.ExecuteEvent = OnSelectChannel;
            ResolutionSetCommand.ExecuteEvent = OnSetResolution;
            NDataShowOneCommand.ExecuteEvent = OnSetNDataShowOne;
            CreateSaveFilePathCommand.ExecuteEvent = OnCreateSaveFilePath;
            SwitchSaveFileCommand.ExecuteEvent = OnSwitchSaveFile;
        }

        private void OnSwitchSaveFile(object obj)
        {
            if (obj as string == "start")
            {
                StartSaveFile();
                IsSavingFile = true;
            }
            else
            {
                StopSaveFile();
                IsSavingFile = false;
            }
        }

        private void StopSaveFile()
        {
            _model.SaveToFileStop();
        }

        private void StartSaveFile()
        {
            _model.SaveToFile(SaveFilePath);
        }

        private void OnCreateSaveFilePath(object obj)
        {
            var dlg = new Microsoft.Win32.OpenFileDialog()
            {
                CheckFileExists = false,
                FileName = "channelcurves.dat"
            };
            if (dlg.ShowDialog().GetValueOrDefault())
            {
                SaveFilePath = dlg.FileName;
            }
        }

        private void OnSetNDataShowOne(object obj)
        {
            NDataShowOne = int.Parse(obj as string);
            ChannelCurveBoxViewModel.SetShowSamplingRate(NDataShowOne);
        }

        private void OnSetResolution(object obj)
        {
            Resolution = int.Parse(obj as string);
        }

        private void OnSelectChannel(object obj)
        {
            SelectedChannelCount = int.Parse(obj as string);
        }

        private void OnSwitchChanged(object parameter)
        {
            this.WriteLine(parameter);
            if (parameter as string == "start")
            {
                Start();
                IsRuning = true;
            }
            else
            {
                Stop();
                IsRuning = false;
            }
        }

        private void Stop()
        {
            _model.Stop();
        }

        private void Start()
        {
            _model.ConnectToCyUSBDevice(SelectedDeviceIndex);
            //_model.ConnectToMockDevice();
            _model.OnDataReceiveEvent = OnDataReceive;
            _model.Start();
        }
    }
}
