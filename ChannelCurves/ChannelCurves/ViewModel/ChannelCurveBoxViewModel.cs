using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.ViewModel
{
    class ChannelCurveBoxViewModel
    {
        public ChannelCurveViewModel ChannelCurveViewModel0 { get; private set; } = new ChannelCurveViewModel(0);
        public ChannelCurveViewModel ChannelCurveViewModel1 { get; private set; } = new ChannelCurveViewModel(1);
    }
}
