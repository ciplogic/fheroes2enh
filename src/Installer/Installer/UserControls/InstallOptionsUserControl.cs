using System;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Installer.Commons;

namespace Installer.UserControls
{
    public partial class InstallOptionsUserControl : UserControl
    {
        public ValueObservable<string> PathSelected = new ValueObservable<string>(@"C:\CS-oss\app");
        public InstallOptionsUserControl()
        {
            InitializeComponent();
            
            PathSelected = new ValueObservable<string>(@"C:\CS-oss\app");
            PathSelected.AddObserver(v => textBox1.Text = v);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var folderBrowserDialog1 = new FolderBrowserDialog();
            var result = folderBrowserDialog1.ShowDialog();
            if (result == DialogResult.Cancel)
                return;
            PathSelected.Value =folderBrowserDialog1.SelectedPath;
        }
    }
}
