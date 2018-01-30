using FH2Setup.Common;

namespace Installer.UserControls.ViewModels
{
    public class MainWindowViewModel : BaseViewModel
    {
        private bool _isLicenseAccepted;


        public bool IsLicenseAccepted
        {
            get => _isLicenseAccepted;
            set
            {
                _isLicenseAccepted = value;
                Notify();
            }
        }
    }
}