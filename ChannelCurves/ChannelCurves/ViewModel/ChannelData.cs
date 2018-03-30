using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.ViewModel
{
    class ChannelData
    {
        public int ChannelIndex { get; private set; }

        public int[] PointsOnScreen { get; private set; } = new int[2000];

        /// <summary>
        /// 显示点索引计数，Max = PointsOnScreen.Length - 1
        /// </summary>
        private int _pointsIndex;

        public int SimplingRate { get; set; } = 1;

        /// <summary>
        /// 采样索引计数, Max = SimplingRate.
        /// </summary>
        private int _simplingIndex;

        public ChannelData(int channelIndex)
        {
            ChannelIndex = channelIndex;
        }

        /// <summary>
        /// 外部传入该通道数据
        /// </summary>
        /// <param name="num"></param>
        void Sample(ushort num)
        {
            if(++_simplingIndex == SimplingRate)
            {
                _simplingIndex = 0;
                PointsOnScreen[_pointsIndex++] = num;
                if (_pointsIndex == PointsOnScreen.Length)
                {
                    ResetArrayToZero(PointsOnScreen);
                }
            }
        }

        static void ResetArrayToZero(int[] arr)
        {
            for(int i=0; i < arr.Length; i++)
            {
                arr[i] = 0;
            }
        }
    }
}
