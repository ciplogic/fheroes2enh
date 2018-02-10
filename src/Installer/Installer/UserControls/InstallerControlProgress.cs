using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Installer.UserControls
{
    public partial class InstallerControlProgress : UserControl
    {
        public ProgressBar MainProgress;
        public Label MainLabel;

        public InstallerControlProgress()
        {
            InitializeComponent();

            MainProgress = progressBar1;
            MainLabel = this.Message;
        }

        private void progressBar1_Click(object sender, EventArgs e)
        {

        }
    }
}
