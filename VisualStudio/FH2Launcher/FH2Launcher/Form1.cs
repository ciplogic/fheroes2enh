using System;
using System.Text;
using System.Windows.Forms;

namespace FH2Launcher
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            comboResolutions.Items.Clear();
            var workingRectangle =Screen.PrimaryScreen.Bounds;
            AddResolution(workingRectangle.Width, workingRectangle.Height);
            AddResolution(640, 480);
            AddResolution(800, 600);
            AddResolution(1024, 768);

            comboResolutions.SelectedIndex = 0;

        }

        private void AddResolution(int width, int height)
        {
            comboResolutions.Items.Add(new Resolution {Width = width, Height = height});
        }

        private void button1_Click(object sender, EventArgs e)
        {

        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            tbxConfig.Text = BuildConfig();
        }

        string BuildConfig()
        {
            var sb = new StringBuilder();
            if (cbxSound.Checked)
            {
                sb.AppendLine("sound = on");
                sb.AppendLine("sound volume = " + numSound.Value);
            }else
                sb.AppendLine("sound = off");

            if (cbxMusic.Checked)
            {
                sb.AppendLine("music = on");
                sb.AppendLine("music volume = " + numVolume.Value);
            }
            else
            {
                sb.AppendLine("music = off");
            }

            if (cbxFullscreen.Checked)
                sb.AppendLine("fullscreen = on");
            else
                sb.AppendLine("fullscreen = off");
            sb.AppendLine("videomode = " + comboResolutions.Text);

            sb.AppendLine("heroes speed = 10");
            sb.AppendLine("ai speed = 10");
            sb.AppendLine("battle speed = 10");
            return sb.ToString();

        }
    }
}
