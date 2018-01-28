using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace FH2Setup.Common
{
    public class BaseViewModel
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void Notify([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}