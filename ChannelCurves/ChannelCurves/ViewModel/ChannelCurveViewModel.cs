﻿using ChannelCurves.Basic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChannelCurves.ViewModel
{
    class ChannelCurveViewModel
    {
        public int ChannelIndex { get; private set; }

        public System.Windows.Point[] PointsOnScreen { get; private set; } = new System.Windows.Point[4000];

        /// <summary>
        /// 显示点索引计数，Max = PointsOnScreen.Length - 1
        /// </summary>
        private int _pointsIndex;

        public int SamplingRate { get; set; } = 1;

        public int Threshold { get => threshold; set { threshold = value; } }

        /// <summary>
        /// 采样索引计数, Max = SimplingRate.
        /// </summary>
        private int _simplingIndex;

        private int threshold;

        //数据大于等于阈值是被激发
        private bool _isStarted = false;

        public void Restart()
        {
            _isStarted = false;
        }
        public ChannelCurveViewModel(int channelIndex)
        {
            ChannelIndex = channelIndex;
            for(var i = 0;  i< PointsOnScreen.Length; i++)
            {
                PointsOnScreen[i].X = i;
            }
        }

        /// <summary>
        /// 外部传入该通道数据
        /// </summary>
        /// <param name="num"></param>
        public void Sample(ushort num)
        {
            if (num >= Threshold)
                _isStarted = true;
            if (false == _isStarted)
                return;
            if(++_simplingIndex >= SamplingRate)
            {
                _simplingIndex = 0;
                PointsOnScreen[_pointsIndex++].Y = num;
                if (_pointsIndex == PointsOnScreen.Length)
                {
                    _pointsIndex = 0;
                    ResetArrayToZero(PointsOnScreen);
                }
            }
        }

        static void ResetArrayToZero(System.Windows.Point[] arr)
        {
            for(int i=0; i < arr.Length; i++)
            {
                arr[i].Y = 0;
            }
        }
    }
}
