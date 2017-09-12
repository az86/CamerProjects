using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;

namespace ChannelCurves.Controls
{
    /// <summary>
    /// ChannelCurve.xaml 的交互逻辑
    /// </summary>
    public partial class ChannelCurve : UserControl
    {
        private static int Count;

        private readonly int Index;

        public static readonly DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof (string), typeof (ChannelCurve));

        public string Title
        {
            get { return Config.UIConfigs.Instance.CHsDesc[Index]; }
            set
            {
                SetValue(TitleProperty, value);
                Config.UIConfigs.Instance.CHsDesc[Index] = value;
            }
        }

        private const int _bufferLen = 10 * 1024;
        // private readonly ObservableDataSource<Point> _dataSource = new ObservableDataSource<Point>(); 
        private readonly int[] _xCoor = new int[_bufferLen];
        private readonly int[] _ptVals = new int[_bufferLen];

        private EnumerableDataSource<int> _dsX;
        private EnumerableDataSource<int> _dsY;

        public ChannelCurve()
        {
            InitializeComponent();
            Index = Count++ % Config.UIConfigs.Instance.CHsDesc.Count();
            Title = Config.UIConfigs.Instance.CHsDesc[Index];
            //chartPlotter.AddLineGraph(_dataSource);
            for(int i = 0; i != _bufferLen; i++)
            {
                _xCoor[i] = i;
            }
            _dsX = new EnumerableDataSource<int>(_xCoor);
            _dsY = new EnumerableDataSource<int>(_ptVals);
            _dsX.SetXMapping(x => x);
            _dsY.SetYMapping(y => y);
            chartPlotter.AddLineGraph(new CompositeDataSource(_dsX, _dsY));
            chartPlotter.Legend.Visibility = Visibility.Hidden;
        }

        public void Start()
        {
            //_dataSource.Collection.Clear();
            _ptCount = 0;
        }
        private void TextBlock_MouseClick(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount == 2)
            {
                tb.Visibility = Visibility.Visible;
                tb.Focus();
            }
        }

        private void TextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            tb.Visibility = Visibility.Collapsed;
        }

        private void TextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                tb.Visibility = Visibility.Collapsed;
            }
            Title = tb.Text;
        }

        private int _ptCount;

        private const int _samplingMaskcode = 0xF;

        int count10K;
        public void AddValue(int val)
        {
            if ((_ptCount & 0xF) == 0)
            {
                int x = (_ptCount/0x10) % _bufferLen;
                _ptVals[x] = val;

                if ((x+1) == _bufferLen)
                {
                    count10K++;
                    if ((count10K % 3) == 0)
                    {
                        for (int k = 0; k != _bufferLen; k++)
                        {
                            _xCoor[k] = (_ptCount + k * 0x10);
                        }

                        App.Current.Dispatcher.Invoke(_dsY.RaiseDataChanged);
                        Console.WriteLine(_ptCount);
                    }
                }

            }
            _ptCount++;
        }
    }
}
