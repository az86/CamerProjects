#include <windows.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <opencv2\opencv.hpp> 

using namespace cv;
using namespace std;
double alpha = 1.0; // contrast �Աȶ�
double beta = 0; // brightness ����

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
 	cvNamedWindow("����ͷ", 1);
 	//cvNamedWindow("contrast", 1);
	while (cvWaitKey(1) != 'q')
	{
		capture >> frame;

		static int i = 0;
		cout << i++ << endl;
		IplImage image(frame);
		cvShowImage("����ͷ", &image); //��ʾһ֡ͼ��

//   		cv::Mat mat(frame);
//   		Mat newMat = Mat::zeros(mat.size(), mat.type()); // �������ת�����ͼ��
//   		mat.convertTo(newMat, -1, 1.5, 0);
//   		IplImage *frame0 = &IplImage(newMat);
//  		cvShowImage("contrast", frame0); //��ʾһ֡ͼ��
 
 	}
 
  	cvDestroyWindow("����ͷ");
	return 0;
}
