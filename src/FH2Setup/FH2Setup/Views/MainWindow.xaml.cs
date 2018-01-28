using System.Windows;

namespace FH2Setup.Views
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        private MainWindowViewModel ViewModel
        {
            set => DataContext = value;
            get => (MainWindowViewModel) DataContext;
        }
        public MainWindow()
        {
            ViewModel = new MainWindowViewModel();
            InitializeComponent();
        }

        private void OnCloseClick(object sender, RoutedEventArgs e)
        {
         
            Close();
        }

        private void OnInstallClick(object sender, RoutedEventArgs e)
        {
            throw new System.NotImplementedException();
        }
    }
}