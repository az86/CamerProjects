using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;

namespace ChannelCurves.View
{
    /// <summary>
    /// ChannelCurve.xaml 的交互逻辑
    /// </summary>
    public partial class ChannelCurve : UserControl
    {

        public static readonly DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof (string), typeof (ChannelCurve));

        public string Title
        {
            get;set;
        }


        private ObservableDataSource<int> _dsX;
        private ObservableDataSource<Point> _dsMax;
        private ObservableDataSource<Point> _dsMin;

        public ChannelCurve()
        {
            InitializeComponent();
            _dsX = new ObservableDataSource<int>();
            _dsMax = new ObservableDataSource<Point>();
            _dsMin = new ObservableDataSource<Point>();
            chartPlotter.AddLineGraph(_dsX);
            chartPlotter.AddLineGraph(_dsMax, System.Windows.Media.Colors.Gray);
            chartPlotter.AddLineGraph(_dsMin, System.Windows.Media.Colors.Gray);
            _dsMax.AppendMany(new Point[] { new Point(0, 0xFFFF), new Point(2000, 0xFFFF) });
            _dsMin.AppendMany(new Point[] { new Point(0, 0x0), new Point(2000, 0x0) });

            chartPlotter.Legend.Visibility = Visibility.Hidden;
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
    }
}
