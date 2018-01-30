using FH2Setup.Common;

namespace FH2Setup
{
    public class InstallOptionsViewModel : BaseViewModel
    {
        private string _installationPath;

        public string InstallationPath
        {
            get => _installationPath;
            set
            {
                _installationPath = value;
                Notify();
            }
        }
    }
}