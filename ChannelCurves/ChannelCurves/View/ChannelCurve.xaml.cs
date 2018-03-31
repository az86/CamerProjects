using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using ChannelCurves.Basic;

namespace ChannelCurves.View
{
    /// <summary>
    /// ChannelCurve.xaml 的交互逻辑
    /// </summary>
    public partial class ChannelCurve : UserControl
    {
        public static readonly DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof(string), typeof(ChannelCurve));

        public string Title { get; set; }

        public static readonly DependencyProperty CurveDataProperty = DependencyProperty.Register("CurveData", typeof(Point[]), typeof(ChannelCurve), new PropertyMetadata(new PropertyChangedCallback(CurveArrayChanged)));

        public Point[] CurveData
        {
            get { return (Point[])GetValue(CurveDataProperty); }
            set { SetValue(CurveDataProperty, value); }
        }

        private System.Timers.Timer _timer = new System.Timers.Timer(50);

        private EnumerableDataSource<Point> _dsCurve;

        public ChannelCurve()
        {
            InitializeComponent();       
            _timer.Elapsed += _timer_Elapsed;
            _timer.Start();
        }

        static void CurveArrayChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            var curveData = args.NewValue as Point[];
            var thisControl = obj as ChannelCurve;
            if (curveData == null || thisControl == null)
                return;
            thisControl.WriteLine("Curve data changed, value: {0}", curveData);
            thisControl.SetCurveArray(curveData);
        }
        private void SetCurveArray(Point[]curveData)
        {
            var dsMax = new ObservableDataSource<Point>(new Point[] { new Point(0, 0xFFFF), new Point(2000, 0xFFFF) });
            var dsMin = new ObservableDataSource<Point>(new Point[] { new Point(0, 0x0), new Point(2000, 0x0) });
            chartPlotter.AddLineGraph(dsMax, System.Windows.Media.Colors.Gray);
            chartPlotter.AddLineGraph(dsMin, System.Windows.Media.Colors.Gray);

            var dsCurve = new EnumerableDataSource<Point>(curveData);
            dsCurve.SetXMapping(point => point.X);
            dsCurve.SetYMapping(point => point.Y);
            chartPlotter.AddLineGraph(dsCurve, System.Windows.Media.Colors.Red);
            chartPlotter.Legend.Visibility = Visibility.Hidden;
            _dsCurve = dsCurve;
        }

        private void _timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            Dispatcher.Invoke(() => UpdateCurve());
        }

        int k;
        private void UpdateCurve()
        {
            var vm = DataContext as ViewModel.ChannelCurveViewModel;
            if (vm == null || _dsCurve == null)
                return;
            _dsCurve.RaiseDataChanged();
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
