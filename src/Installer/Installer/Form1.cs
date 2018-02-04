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
        private ValueObservable<bool> LicensesAgreed;
        public Form1()
        {
            InitializeComponent();
            LicensesAgreed = new ValueObservable<bool>(false);
            LicensesAgreed.AddObserver(v => { licensesInstallCheck.Checked = v;});
            LicensesAgreed.AddObserver(v => { installButton.Enabled = v; });
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var destinationPath = installOptionsUserControl1.PathSelected.Value;
            var itemsToDownload = installOptionsUserControl1.GetItemsToDownload();

            DownloadItems(itemsToDownload, destinationPath);

        }

        private static void SelectExeAtFinalInstall(string destinationPath)
        {
            MessageBox.Show("Installation complete!");


            var fH2Launcher = Path.Combine(destinationPath, "FHeroes2Enh.exe");
            // suppose that we have a test.txt at E:\
            if (!File.Exists(fH2Launcher))
            {
                return;
            }

            // combine the arguments together
            // it doesn't matter if there is a space after ','
            string argument = "/select, \"" + fH2Launcher + "\"";

            System.Diagnostics.Process.Start("explorer.exe", argument);
        }

        private void DownloadItems(string[] itemsToDownload, string destinationPath)
        {
            installButton.Enabled = false;
            licensesInstallCheck.Enabled = false;
            var thread = new Thread(() =>
            {
                var wc = new WebClient();
                foreach (var itemToDownload in itemsToDownload)
                {
                    this.UIThread(() => { this.Text = "Downloading: " + itemToDownload; });
                    var urlToDownload = ConstStrings.FullUrlToPacks + itemToDownload;
                    var destinationFile = Path.Combine(destinationPath, itemToDownload);
                    ZipExtractorUtilies.CreateDirectoryForFile(destinationFile);
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
                ControlExtensions.UIThread(this, () =>
                {
                    installButton.Enabled = true;
                    licensesInstallCheck.Enabled = true;

                    SelectExeAtFinalInstall(destinationPath);
                });
            });
            thread.Start();
            

        }


        Stream GetStream(string name)
        {
            var imageStream = Assembly.GetExecutingAssembly().GetManifestResourceStream(name);
            return imageStream;
        }

        private void licensesInstallCheck_CheckedChanged(object sender, EventArgs e)
        {
            LicensesAgreed.Value = licensesInstallCheck.Checked;
        }
    }
}
