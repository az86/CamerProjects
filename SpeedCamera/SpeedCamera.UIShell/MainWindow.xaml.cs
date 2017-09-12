using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SpeedCamera.Algorithm;
using SpeedCamera.ViewModel;

namespace SpeedCamera.UIShell
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly SpeedCamera.Interfaces.Algorithm.IPictureStream _pictureStream = new PictureStream();
        public MainWindow()
        {
            InitializeComponent();
            DataContext = new MainViewVm(_pictureStream);
            this.KeyDown += MainWindow_KeyDown;
        }

        private void MainWindow_KeyDown(object sender, KeyEventArgs e)
        {
            mainView.RaiseEvent(e);
        }
    }
}
