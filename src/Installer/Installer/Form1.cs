using System;
using System.IO;
using System.Reflection;
using System.Windows.Forms;

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
            using (var zipStream = GetStream("Installer.h2demo.zip"))
            {
                ZipExtractorUtilies.UnzipFromStream(zipStream, destinationPath);
            }
        }



        Stream GetStream(string name)
        {
            var imageStream = Assembly.GetExecutingAssembly().GetManifestResourceStream(name);
            return imageStream;
        }
    }
}
