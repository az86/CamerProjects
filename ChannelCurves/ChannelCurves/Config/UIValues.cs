using System;

namespace ChannelCurves.Config
{
    [Serializable]
    public class UIConfigs
    {
        private const string FilePath = @"Config\UIConfig.xml";

        public static readonly UIConfigs Instance = Load();

        public string[] CHsDesc = {"CH0", "CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7"};

        public bool CB0IsChecked { get; set; }
        public bool CB1IsChecked { get; set; }
        public bool CB2IsChecked { get; set; }
        public bool CB3IsChecked { get; set; }
        public bool CB4IsChecked { get; set; }
        public bool CB5IsChecked { get; set; }
        public bool CB6IsChecked { get; set; }
        public bool CB7IsChecked { get; set; }

        private int _selecteResolutionIndex;
        public int SelecteResolutionIndex
        {
            get
            {
                return _selecteResolutionIndex;
            }

            set
            {
                _selecteResolutionIndex = value;
                Console.WriteLine("Data length set {0}", _selecteResolutionIndex == 0 ? 8 : 16);
            }
        }


        private UIConfigs()
        {
            System.Diagnostics.Debug.WriteLine("UIConfigs constructed.");
        }

        ~UIConfigs()
        {
            System.Diagnostics.Debug.WriteLine("UIConfigs deconstructed.");
            Save();
        }

        private void Save()
        {
            var dir = System.IO.Path.GetDirectoryName(FilePath);
            if (!System.IO.Directory.Exists(dir))
            {
                System.IO.Directory.CreateDirectory(dir);
            }
            var ser = new System.Xml.Serialization.XmlSerializer(typeof(UIConfigs));
            using (var tw = new System.IO.StreamWriter(FilePath))
            {
                ser.Serialize(tw, Instance);
                System.Diagnostics.Debug.WriteLine("UIConfigs has saved.");
            }
        }

        private static UIConfigs Load()
        {
            if (System.IO.File.Exists(FilePath))
            {
                var ser = new System.Xml.Serialization.XmlSerializer(typeof (UIConfigs));
                using (var sr = new System.IO.StreamReader(FilePath))
                {
                    System.Diagnostics.Debug.WriteLine("UIConfigs load config.");
                    return ser.Deserialize(sr) as UIConfigs;
                }
            }
            System.Diagnostics.Debug.WriteLine("UIConfigs creates new.");
            return new UIConfigs();
        }
    }
}
