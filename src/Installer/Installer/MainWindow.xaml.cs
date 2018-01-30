using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Reflection;
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
            using (var zipStream = GetStream("Installer.h2demo.zip"))
            {
                UnzipFromStream(zipStream, "out");
            }
        }
        /// <summary>
        /// Copies the contents of input to output. Doesn't close either stream.
        /// </summary>
        public static void CopyStream(Stream input, Stream output)
        {
            byte[] buffer = new byte[8 * 1024];
            int len;
            while ((len = input.Read(buffer, 0, buffer.Length)) > 0)
            {
                output.Write(buffer, 0, len);
            }
        }
        public void UnzipFromStream(Stream zipStream, string outFolder)
        {
            Directory.CreateDirectory(outFolder);
            var za = new ZipArchive(zipStream, ZipArchiveMode.Read);
            foreach (var zipArchiveEntry in za.Entries)
            {
                var words = zipArchiveEntry.Name.Split('/');
                if(words.Length<2)
                    continue;
                Directory.CreateDirectory(words[0]);
            }

            foreach (var zipArchiveEntry in za.Entries)
            {
                
                using (var childStream = zipArchiveEntry.Open())
                {
                    using (Stream file = File.Create(zipArchiveEntry.Name))
                    {
                        CopyStream(childStream, file);
                    }
                }
            }
        }

        private void OnCloseClick(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void OnCheckedLicense(object sender, RoutedEventArgs e)
        {
        }

        Stream GetStream(string name)
        {
            var imageStream = Assembly.GetExecutingAssembly().GetManifestResourceStream(name);
            return imageStream;
        }
    }
}
