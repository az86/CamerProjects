using System;
using System.Windows.Input;

namespace AZ.Basic
{
    public class Command:ICommand
    {
        public string Name { get; set; }

        public event EventHandler CanExecuteChanged;

        public event EventHandler Invoked;

        private bool _isEnable = true;

        public bool IsEnable
        {
            get { return _isEnable; }
            set
            {
                _isEnable = value;
                CanExecuteChanged?.Invoke(this, new EventArgs());
            }
        }


        public bool CanExecute(object parameter)
        {
            return IsEnable;
        }

        public void Execute(object parameter)
        {
            Logger.WriteLine("{0} command is invoked.", Name);
            Invoked?.Invoke(this, EventArgs.Empty);
        }
    }
}
