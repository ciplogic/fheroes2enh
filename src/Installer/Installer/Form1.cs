using System;
using System.IO;
using System.Net;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;
using Installer.Commons;

namespace Installer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var destinationPath = installOptionsUserControl1.PathSelected.Value;
            var itemsToDownload = installOptionsUserControl1.GetItemsToDownload();

            installButton.Enabled = false;
            DownloadItems(itemsToDownload, destinationPath);
        }

        private void DownloadItems(string[] itemsToDownload, string destinationPath)
        {
            var thread = new Thread(() =>
            {
                var wc = new WebClient();
                foreach (var itemToDownload in itemsToDownload)
                {
                    this.UIThread(() => { this.Text = "Downloading: " + itemToDownload; });
                    var urlToDownload = ConstStrings.FullUrlToPacks + itemToDownload;
                    var destinationFile = Path.Combine(destinationPath, itemToDownload);
                    wc.DownloadFile(urlToDownload, destinationFile);
                    using (var zipStream = File.Open(destinationFile, FileMode.Open))
                    {
                        ZipExtractorUtilies.UnzipFromStream(zipStream, destinationPath);
                    }
                    File.Delete(destinationFile);
                }
                
                using (var zipStream = GetStream("Installer.fh2enh.zip"))
                {
                    ZipExtractorUtilies.UnzipFromStream(zipStream, destinationPath);
                }
                ControlExtensions.UIThread(this, () => { installButton.Enabled = true; });
            });
            thread.Start();
            

        }


        Stream GetStream(string name)
        {
            var imageStream = Assembly.GetExecutingAssembly().GetManifestResourceStream(name);
            return imageStream;
        }
    }
}
