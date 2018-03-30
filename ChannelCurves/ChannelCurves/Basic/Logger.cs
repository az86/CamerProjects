using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.Basic
{
    static class Logger
    {
        public static void WriteLine(this object obj, string fmt, params object[] args)
        {
            Console.WriteLine(fmt, args);
        }

        public static void WriteLine(this object obj, object arg)
        {
            Console.WriteLine(arg);
        }
    }
}
