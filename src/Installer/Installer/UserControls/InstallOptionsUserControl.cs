using System;
using System.Collections.Generic;
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
        public ValueObservable<string> PathSelected = new ValueObservable<string>(@"C:\Games\FH2Enh");

        public InstallOptionsUserControl()
        {
            InitializeComponent();
            
            PathSelected.AddObserver(v => textBox1.Text = v);
            object[] items = ConstStrings.Packs.Select(it => ((object) it)).ToArray();
            checkedListBox1.Items.AddRange(items);

            SetAllWithCheck(true);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var folderBrowserDialog1 = new FolderBrowserDialog();
            var result = folderBrowserDialog1.ShowDialog();
            if (result == DialogResult.Cancel)
                return;
            PathSelected.Value =folderBrowserDialog1.SelectedPath;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SetAllWithCheck(true);
        }

        private void SetAllWithCheck(bool isEnabled)
        {
            var size = checkedListBox1.Items.Count;
            for (int i = 0; i < size; i++)
            {
                checkedListBox1.SetItemChecked(i, isEnabled);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SetAllWithCheck(false);
        }

        public string[] GetItemsToDownload()
        {
            var listResult = new List<string>();

            var size = checkedListBox1.Items.Count;
            for (int i = 0; i < size; i++)
            {
                if (checkedListBox1.GetItemChecked(i))
                {
                    listResult.Add(ConstStrings.Packs[i]);
                }
            }
            return listResult.ToArray();
        }
    }
}
