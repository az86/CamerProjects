#include <windows.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <opencv2\opencv.hpp> 

using namespace cv;
using namespace std;
double alpha = 1.0; // contrast 对比度
double beta = 0; // brightness 亮度

void ContrastImage(double contrast, IplImage *img, IplImage *newImg);

int main()
{
	VideoCapture capture;
	if (capture.open(0))
	{
		cout << "capture open OK" << endl;
	}
	capture.set(CAP_PROP_FPS, 60);
	capture.set(CAP_PROP_FRAME_WIDTH, 800);
	capture.set(CAP_PROP_FRAME_HEIGHT, 600);
	
	Mat frame;
	capture >> frame;
 	std::cout << frame.rows << ", " << frame.cols << std::endl;
 	cvNamedWindow("摄像头", 1);
 	//cvNamedWindow("contrast", 1);
	while (cvWaitKey(1) != 'q')
	{
		capture >> frame;

		static int i = 0;
		cout << i++ << endl;
		IplImage image(frame);
		cvShowImage("摄像头", &image); //显示一帧图像

//   		cv::Mat mat(frame);
//   		Mat newMat = Mat::zeros(mat.size(), mat.type()); // 存放亮度转换后的图像
//   		mat.convertTo(newMat, -1, 1.5, 0);
//   		IplImage *frame0 = &IplImage(newMat);
//  		cvShowImage("contrast", frame0); //显示一帧图像
 
 	}
 
  	cvDestroyWindow("摄像头");
	return 0;
}
