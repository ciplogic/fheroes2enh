using System.Windows;
using FH2Setup.Common;

namespace FH2Setup
{
    public class MainWindowViewModel : BaseViewModel
    {
        private bool _isLicenceAccepted;


        public bool IsLicenceAccepted
        {
            get => _isLicenceAccepted;
            set
            {
                _isLicenceAccepted = value;
                Notify();
            }
        }
    }
}