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
using Installer.UserControls.ViewModels;

namespace Installer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private MainWindowViewModel ViewModel
        {
            get => (MainWindowViewModel) DataContext;
            set => DataContext = value;
        }
        public MainWindow()
        {
            ViewModel = new MainWindowViewModel();
            InitializeComponent();
        }

        private void OnInstallClick(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void OnCloseClick(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void OnCheckedLicense(object sender, RoutedEventArgs e)
        {
        }
    }
}
