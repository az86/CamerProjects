using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AZ.Basic
{
    public class Logger
    {
        private static readonly StreamWriter _sw = new StreamWriter("log.txt");

        private static int _lineNum;

        public static void WriteLine(string format, params object[] arg)
        {
            lock (_sw)
            {
                Console.Write("{0} {1} :", _lineNum, DateTime.Now);
                Console.WriteLine(format, arg);
                _sw.Write("{0} {1} :", _lineNum++, DateTime.Now);
                _sw.WriteLine(format, arg);
                _sw.Flush();
            }
        }
    }
}
