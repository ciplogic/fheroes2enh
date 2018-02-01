using System;

namespace Installer
{
    class Program
    {
        [STAThread]
        public static void Main(string[] args)
        {
            var mainWindow = new  Form1();
            mainWindow.ShowDialog();
        }
    }
}
