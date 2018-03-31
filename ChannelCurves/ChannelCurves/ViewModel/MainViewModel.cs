using ChannelCurves.Basic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.ViewModel
{
    class MainViewModel 
    {
        public ChannelCurveBoxViewModel ChannelCurveBoxViewModel { get; private set; } = new ChannelCurveBoxViewModel();

        private readonly Model.ModelFacade _model = new Model.ModelFacade();

        public Command StartCommand { get; set; } = new Command();

        public Command ChannelSelectCommand { get; set; } = new Command();

        public Command ResolutionSetCommand { get; set; } = new Command();

        public Command NDataShowOneCommand { get; set; } = new Command();

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

        private int _nDataShowOne = 1;

        void OnDataReceive(byte[] buf)
        {
            // ChannelCurveBoxViewModel.ChannelCurveViewModel0.
            ChannelCurveBoxViewModel.ShowData(buf);
        }
        public MainViewModel()
        {
            StartCommand.ExecuteEvent = OnStart;
            ChannelSelectCommand.ExecuteEvent = OnSelectChannel;
            ResolutionSetCommand.ExecuteEvent = OnSetResolution;
            NDataShowOneCommand.ExecuteEvent = OnSetNDataShowOne;
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

        private void OnStart(object parameter)
        {
            this.WriteLine(parameter);
            //_model.ConnectToCyUSBDevice(SelectedDeviceIndex);
            _model.ConnectToMockDevice();
            _model.OnDataReceiveEvent = OnDataReceive;
            _model.Start();
        }
    }
}
