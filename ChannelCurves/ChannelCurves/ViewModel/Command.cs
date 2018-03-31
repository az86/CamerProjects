using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.ViewModel
{
    class Command : System.Windows.Input.ICommand
    {
        public Action<object> ExecuteEvent;

        public event EventHandler CanExecuteChanged;

        public bool CanExecute(object parameter)
        {
            return true;
        }

        public void Execute(object parameter)
        {
            ExecuteEvent?.Invoke(parameter);
        }
    }
}
