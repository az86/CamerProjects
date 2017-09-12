#include "StdAfx.h"
#include "CameraDataProcess.h"
#include "LogTool.h"

#define B_OFFSET    0
#define G_OFFSET    1
#define R_OFFSET    2
#define ODD(i)      ((i)&1)


CameraDataProcess::CameraDataProcess()
{
	m_RgbData = NULL;
	m_pDisplay = NULL;
	m_PictureFormat = 0;

	m_IsCapturing = false;
	m_MirrorStatus = MirrorNon;

}
void CameraDataProcess::SetWidthHeight(int w,int h)
{
	m_Width = w;
	m_Height = h;

	if(w>1300)
	{
		memset(&m_BitmapInfo,0,sizeof(BITMAPINFO));
		m_BitmapInfo.bmiHeader.biBitCount=24;
		m_BitmapInfo.bmiHeader.biClrImportant=0;
		m_BitmapInfo.bmiHeader.biClrUsed=0;
		m_BitmapInfo.bmiHeader.biCompression=0;
		m_BitmapInfo.bmiHeader.biHeight=m_Height/2;
		m_BitmapInfo.bmiHeader.biPlanes=1;
		m_BitmapInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		m_BitmapInfo.bmiHeader.biSizeImage=m_Width/2*m_Height/2*3;					//图片实际数据字节数
		m_BitmapInfo.bmiHeader.biWidth=m_Width/2;
		m_BitmapInfo.bmiHeader.biXPelsPerMeter=0;
		m_BitmapInfo.bmiHeader.biYPelsPerMeter=0;
	}
	else
	{
		memset(&m_BitmapInfo,0,sizeof(BITMAPINFO));
		m_BitmapInfo.bmiHeader.biBitCount=24;
		m_BitmapInfo.bmiHeader.biClrImportant=0;
		m_BitmapInfo.bmiHeader.biClrUsed=0;
		m_BitmapInfo.bmiHeader.biCompression=0;
		m_BitmapInfo.bmiHeader.biHeight=m_Height;
		m_BitmapInfo.bmiHeader.biPlanes=1;
		m_BitmapInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		m_BitmapInfo.bmiHeader.biSizeImage=m_Width*m_Height*3;					//图片实际数据字节数
		m_BitmapInfo.bmiHeader.biWidth=m_Width;
		m_BitmapInfo.bmiHeader.biXPelsPerMeter=0;
		m_BitmapInfo.bmiHeader.biYPelsPerMeter=0;
	}
	
	if(m_pDisplay!=NULL)
	{
		m_pDisplay->Close();
		delete m_pDisplay;
	}

	m_pDisplay=new CDisplay();
	
}
void CameraDataProcess::SetVideoWidthHeight(int w,int h)
{
	memset(&m_BitmapInfo,0,sizeof(BITMAPINFO));
	m_BitmapInfo.bmiHeader.biBitCount=24;
	m_BitmapInfo.bmiHeader.biClrImportant=0;
	m_BitmapInfo.bmiHeader.biClrUsed=0;
	m_BitmapInfo.bmiHeader.biCompression=0;
	m_BitmapInfo.bmiHeader.biHeight=h;
	m_BitmapInfo.bmiHeader.biPlanes=1;
	m_BitmapInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_BitmapInfo.bmiHeader.biSizeImage=w*h*3;					//图片实际数据字节数
	m_BitmapInfo.bmiHeader.biWidth=w;
	m_BitmapInfo.bmiHeader.biXPelsPerMeter=0;
	m_BitmapInfo.bmiHeader.biYPelsPerMeter=0;
}
int CameraDataProcess::GetBitmapInfoWidth()
{
	return m_BitmapInfo.bmiHeader.biWidth;
}
void CameraDataProcess::SetDisplayWidthHeight(int w, int h)
{
	this->m_DisplayWidth = w;
	this->m_DisplayHeight = h;
}


CameraDataProcess::~CameraDataProcess(void)
{
	if(m_RgbData!=NULL)
	{
		delete [] m_RgbData;
	}
	if(m_pDisplay!=NULL)
	{
		m_pDisplay->Close();
		delete m_pDisplay;
	}
}
int CameraDataProcess::Open(HDC hDC)
{
	m_hDC = hDC;
	
	return 0;
}
int CameraDataProcess::ProcessData(byte *pIn)
{
	
	byte *tpIn = MoveData1(pIn,m_Width*m_Height*4);
	//byte *tpIn = MoveData1(pIn,m_Width*m_Height*4);

	//LogTool::SaveSourceRowData(tpIn,0);

	unsigned char *d = MoveData(tpIn,m_Width*m_Height*4);
	//收集左边10个像素原始数据
	//LogTool::FormatYuvFileSorceData(pIn, 10,m_Height);
	//LogTool::FormatYuvFileSourceRowData(pIn,0);

	unsigned char *rgb24 = new unsigned char[m_Width*m_Height*3];
	memset(rgb24,0,m_Width*m_Height*3);
	
	//收集左边10个像素数据
	//LogTool::FormatYuvFileData(d, 10,m_Height);
	//LogTool::FormatYuvFileRowData(d,0);

	liyuv422_to_rgb_buffer(d,rgb24, m_Width,m_Height);
	//yuv422_to_rgb24(d,rgb24, m_Width,m_Height);
	//ByteToRGB( d ,rgb24 );
	unsigned char *newrgb24 = RGBReCreate(rgb24, 0, m_Width, m_Height);
	//收集左边10个像素RGB数据
	//LogTool::FormatRGBFileData(newrgb24, 10,m_Height);
	//LogTool::FormatRGBFileRowData(newrgb24,0);
	//保存RGB数据
	LogTool::SaveDIB2Bmp(m_Width,m_Height, newrgb24);
	LogTool::SaveDIB2Bmp(m_Width,m_Height, rgb24);

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width,m_Height));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width,m_Height,0,0,m_Width,m_Height,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();



	delete [] rgb24;
	delete [] d;

	delete [] tpIn;

	return 0;
}
//图像缩放 按图像长宽进行缩放 YUV
int CameraDataProcess::ProcessData1(byte *pIn,int size)
{
	//LogTool::CreateYuvFile(pIn,size);
	//CString logstr;

	unsigned char *tpIn = MoveData1(pIn,size);//m_Width*m_Height*4
	
	//LogTool::FormatYuvFileSourceRowData("2048",tpIn,2048,0);

	unsigned char *tpInmin = MoveData3(tpIn,size,m_Width*m_Height*4);
	delete [] tpIn;
	
	//LogTool::FormatYuvFileSourceRowData("1024",tpInmin,1024,0);

	unsigned char *d = MoveData(tpInmin,m_Width*m_Height*4);
	delete [] tpInmin;

	//logstr = LogTool::FormatLog(d,2048);
	//LogTool::Writelog(logstr,"2048d");

	//收集左边10个像素原始数据
	//LogTool::FormatYuvFileSorceData(pIn, 10,m_Height);
	//LogTool::FormatYuvFileSourceRowData(pIn,0);

	unsigned char *rgb24 = new unsigned char[m_Width*m_Height*3];
	memset(rgb24,0,m_Width*m_Height*3);
	liyuv422_to_rgb_buffer(d,rgb24, m_Width,m_Height);
	delete [] d;

	//yuv422_to_rgb24(d,rgb24, m_Width,m_Height);
	//ByteToRGB( d ,rgb24 );
	//unsigned char *newrgb24 = RGBReCreate(rgb24, 0, m_Width, m_Height);
	//收集左边10个像素RGB数据
	//LogTool::FormatRGBFileData(newrgb24, 10,m_Height);
	//LogTool::FormatRGBFileRowData("1024",rgb24,1024,0);
	//保存RGB数据
	//LogTool::SaveDIB2Bmp(m_Width,m_Height, newrgb24);
	//LogTool::SaveDIB2Bmp(m_Width,m_Height, rgb24);

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width,m_Height));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width,m_Height,0,0,m_Width,m_Height,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	delete [] rgb24;
	
	return 0;
}
//优化算法
int CameraDataProcess::ProcessData4(byte *pIn,int size)
{
	unsigned char *tpIn = MoveData1(pIn,size);

	unsigned char *rgb = new unsigned char[m_Width*m_Height*3];
	memset(rgb,0,m_Width*m_Height*3);

	int j = 0;
	for(int i=0;i<m_Width*m_Height*2*2;i+=8)
	{
		/*
		unsigned short y0 = d[i];
		unsigned short u = d[i+1];
		unsigned short y1 = d[i+2];
		unsigned short v = d[i+3];
		*/
		unsigned short y0 = (tpIn[i+1]*0x100+tpIn[i])>>2;
		unsigned short u = (tpIn[i+3]*0x100+tpIn[i+2])>>2;
		unsigned short y1 = (tpIn[i+5]*0x100+tpIn[i+4])>>2;
		unsigned short v = (tpIn[i+7]*0x100+tpIn[i+6])>>2;

		int R1 = y0 + 1.4075*(v-128);
		int G1 = y0-0.3455*(u-128)-0.7169 *(v-128);
		int B1 = y0 + 1.779*(u-128);

		int R2 = y1 + 1.4075*(v-128);
		int G2 = y1-0.3455*(u-128)-0.7169 *(v-128);
		int B2 = y1 + 1.779*(u-128);

		R1 = R1>255?255:R1;
		R1 = R1<0?0:R1;
		G1 = G1>255?255:G1;
		G1 = G1<0?0:G1;
		B1 = B1>255?255:B1;
		B1 = B1<0?0:B1;

		R2 = R2>255?255:R2;
		R2 = R2<0?0:R2;
		G2 = G2>255?255:G2;
		G2 = G2<0?0:G2;
		B2 = B2>255?255:B2;
		B2 = B2<0?0:B2;

		rgb[j] = B1;
		rgb[j+1] = G1;
		rgb[j+2] = R1;

		rgb[j+3] = B2;
		rgb[j+4] = G2;
		rgb[j+5] = R2;

		j+=6;
	}
	delete []tpIn;

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width,m_Height));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width,m_Height,0,0,m_Width,m_Height,rgb,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}

	delete [] rgb;

	LogTool::WriteTimelog("end..","ProcessData4");

	return 1;

}
unsigned char * CameraDataProcess::GetYUVRgbBuf(byte *pIn,int size)
{
	unsigned char *tpIn = MoveData1(pIn,size);

	unsigned char *rgb = new unsigned char[m_Width*m_Height*3];
	memset(rgb,0,m_Width*m_Height*3);

	int j = 0;
	for(int i=0;i<m_Width*m_Height*2*2;i+=8)
	{
		unsigned short y0 = (tpIn[i+1]*0x100+tpIn[i])>>2;
		unsigned short u = (tpIn[i+3]*0x100+tpIn[i+2])>>2;
		unsigned short y1 = (tpIn[i+5]*0x100+tpIn[i+4])>>2;
		unsigned short v = (tpIn[i+7]*0x100+tpIn[i+6])>>2;

		int R1 = y0 + 1.4075*(v-128);
		int G1 = y0-0.3455*(u-128)-0.7169 *(v-128);
		int B1 = y0 + 1.779*(u-128);

		int R2 = y1 + 1.4075*(v-128);
		int G2 = y1-0.3455*(u-128)-0.7169 *(v-128);
		int B2 = y1 + 1.779*(u-128);

		R1 = R1>255?255:R1;
		R1 = R1<0?0:R1;
		G1 = G1>255?255:G1;
		G1 = G1<0?0:G1;
		B1 = B1>255?255:B1;
		B1 = B1<0?0:B1;

		R2 = R2>255?255:R2;
		R2 = R2<0?0:R2;
		G2 = G2>255?255:G2;
		G2 = G2<0?0:G2;
		B2 = B2>255?255:B2;
		B2 = B2<0?0:B2;

		rgb[j] = B1;
		rgb[j+1] = G1;
		rgb[j+2] = R1;

		rgb[j+3] = B2;
		rgb[j+4] = G2;
		rgb[j+5] = R2;

		j+=6;
	}
	delete []tpIn;

	return rgb;
}
unsigned char * CameraDataProcess::GetRAWRgbBuf(byte *pIn,int size)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;
	int imagesize = w * h * 2;
	//LogTool::CreateRAWFile(pIn,size);
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	unsigned short *d = ChangeByteToInt(d1,imagesize);
	unsigned char *rgb24 = new unsigned char[w*h*3];
	memset(rgb24,0,w*h*3);

	BayerConversion11(d, rgb24, w, h);

	delete []d1;
	delete [] d;

	return rgb24;
}
unsigned char * CameraDataProcess::GetRAWRgbBuf2(byte *pIn,int size)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;

	int imagesize = w * h;

	unsigned short *pInt = new unsigned short[imagesize];
	memset(pInt,0x00,imagesize);
	for(int j=0;j<size;j++)
	{
		unsigned char d2 = pIn[j];
		unsigned short d3 = 0x00+d2;

		pInt[j] = d3;

	}
	unsigned char *rgb24 = new unsigned char[w*h*3];
	memset(rgb24,0,w*h*3);

	BayerConversion14(pInt, rgb24, w, h);

	delete []pInt;

	return rgb24;
}
//YUV /2
int CameraDataProcess::ProcessData5(byte *pIn,int size)
{
	unsigned char *tpIn = MoveData1(pIn,size);

	int w = this->m_Width/2;
	int h = this->m_Height/2;
	unsigned char *rgb = new unsigned char[w*h*3];
	memset(rgb,0,w*h*3);

	int j = 0;
	for(int i=0;i<size;i+=16)
	{
		unsigned short y0 = (tpIn[i+1]*0x100+tpIn[i])>>2;
		unsigned short u = (tpIn[i+3]*0x100+tpIn[i+2])>>2;
		unsigned short y1 = (tpIn[i+5]*0x100+tpIn[i+4])>>2;
		unsigned short v = (tpIn[i+7]*0x100+tpIn[i+6])>>2;

		int R1 = y0 + 1.4075*(v-128);
		int G1 = y0-0.3455*(u-128)-0.7169 *(v-128);
		int B1 = y0 + 1.779*(u-128);

		int R2 = y1 + 1.4075*(v-128);
		int G2 = y1-0.3455*(u-128)-0.7169 *(v-128);
		int B2 = y1 + 1.779*(u-128);

		R1 = R1>255?255:R1;
		R1 = R1<0?0:R1;
		G1 = G1>255?255:G1;
		G1 = G1<0?0:G1;
		B1 = B1>255?255:B1;
		B1 = B1<0?0:B1;

		R2 = R2>255?255:R2;
		R2 = R2<0?0:R2;
		G2 = G2>255?255:G2;
		G2 = G2<0?0:G2;
		B2 = B2>255?255:B2;
		B2 = B2<0?0:B2;

		rgb[j] = B1;
		rgb[j+1] = G1;
		rgb[j+2] = R1;

		rgb[j+3] = B2;
		rgb[j+4] = G2;
		rgb[j+5] = R2;

		j+=6;

		int z1 = i%(this->m_Width*4);
		int z2 = i/(this->m_Width*4);
		if(z1==0)
		{
			int z3 = z2%2;
			if(z3==0)
			{
				i = i + this->m_Width*4;
			}
		}
		
	}
	delete []tpIn;

	//LogTool::WriteTimelog("start2..","yuvtimer");
	//LogTool::SaveDIB2Bmp(m_Width/2,m_Height/2, rgb);

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width/2,m_Height/2));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width/2,m_Height/2,0,0,m_Width/2,m_Height/2,rgb,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(w,h, rgb);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}
	//LogTool::WriteTimelog("start3..","yuvtimer");

	delete [] rgb;

	//LogTool::WriteTimelog("end..","ProcessData5");

	return 1;
}
//YUV 1/2数据处理，自适应显示
int CameraDataProcess::ProcessData7(byte *pIn,int size)
{
	unsigned char *tpIn = MoveData1(pIn,size);

	int w = this->m_Width/2;
	int h = this->m_Height/2;
	unsigned char *rgb = new unsigned char[w*h*3];
	memset(rgb,0,w*h*3);

	int j = 0;
	for(int i=0;i<size;i+=16)
	{
		unsigned short y0 = (tpIn[i+1]*0x100+tpIn[i])>>2;
		unsigned short u = (tpIn[i+3]*0x100+tpIn[i+2])>>2;
		unsigned short y1 = (tpIn[i+5]*0x100+tpIn[i+4])>>2;
		unsigned short v = (tpIn[i+7]*0x100+tpIn[i+6])>>2;

		int R1 = y0 + 1.4075*(v-128);
		int G1 = y0-0.3455*(u-128)-0.7169 *(v-128);
		int B1 = y0 + 1.779*(u-128);

		int R2 = y1 + 1.4075*(v-128);
		int G2 = y1-0.3455*(u-128)-0.7169 *(v-128);
		int B2 = y1 + 1.779*(u-128);

		R1 = R1>255?255:R1;
		R1 = R1<0?0:R1;
		G1 = G1>255?255:G1;
		G1 = G1<0?0:G1;
		B1 = B1>255?255:B1;
		B1 = B1<0?0:B1;

		R2 = R2>255?255:R2;
		R2 = R2<0?0:R2;
		G2 = G2>255?255:G2;
		G2 = G2<0?0:G2;
		B2 = B2>255?255:B2;
		B2 = B2<0?0:B2;

		rgb[j] = B1;
		rgb[j+1] = G1;
		rgb[j+2] = R1;

		rgb[j+3] = B2;
		rgb[j+4] = G2;
		rgb[j+5] = R2;

		j+=6;

		int z1 = i%(this->m_Width*4);
		int z2 = i/(this->m_Width*4);
		if(z1==0)
		{
			int z3 = z2%2;
			if(z3==0)
			{
				i = i + this->m_Width*4;
			}
		}
		
	}
	delete []tpIn;

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width/2,m_Height/2));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width/2,m_Height/2,0,0,m_Width/2,m_Height/2,rgb,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(w,h, rgb);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}
	//LogTool::WriteTimelog("start3..","yuvtimer");

	delete [] rgb;

	//LogTool::WriteTimelog("end..","ProcessData5");

	return 1;
}
//高分辨率显示提高速度
int CameraDataProcess::ProcessData10(byte *pIn,int size)
{
	unsigned char *tpIn = MoveData1(pIn,size);

	int w = this->m_Width/2;
	int h = this->m_Height/2;
	unsigned char *rgb = new unsigned char[w*h*3];
	memset(rgb,0,w*h*3);

	int j = 0;
	for(int i=0;i<size;i+=16)
	{
		unsigned short y0 = (tpIn[i+1]*0x100+tpIn[i])>>2;
		unsigned short u = (tpIn[i+3]*0x100+tpIn[i+2])>>2;
		unsigned short y1 = (tpIn[i+5]*0x100+tpIn[i+4])>>2;
		unsigned short v = (tpIn[i+7]*0x100+tpIn[i+6])>>2;

		int R1 = y0 + 1.4075*(v-128);
		int G1 = y0-0.3455*(u-128)-0.7169 *(v-128);
		int B1 = y0 + 1.779*(u-128);

		int R2 = y1 + 1.4075*(v-128);
		int G2 = y1-0.3455*(u-128)-0.7169 *(v-128);
		int B2 = y1 + 1.779*(u-128);

		R1 = R1>255?255:R1;
		R1 = R1<0?0:R1;
		G1 = G1>255?255:G1;
		G1 = G1<0?0:G1;
		B1 = B1>255?255:B1;
		B1 = B1<0?0:B1;

		R2 = R2>255?255:R2;
		R2 = R2<0?0:R2;
		G2 = G2>255?255:G2;
		G2 = G2<0?0:G2;
		B2 = B2>255?255:B2;
		B2 = B2<0?0:B2;

		rgb[j] = B1;
		rgb[j+1] = G1;
		rgb[j+2] = R1;

		rgb[j+3] = B2;
		rgb[j+4] = G2;
		rgb[j+5] = R2;

		j+=6;

		int z1 = i%(this->m_Width*4);
		int z2 = i/(this->m_Width*4);
		if(z1==0)
		{
			int z3 = z2%2;
			if(z3==0)
			{
				i = i + this->m_Width*4;
			}
		}
		
	}
	delete []tpIn;

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,this->m_DisplayWidth,this->m_DisplayHeight));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);//COLORONCOLOR
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,this->m_DisplayWidth,this->m_DisplayHeight,0,0,m_Width/2,m_Height/2,rgb,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		unsigned char *desrgb24 = GetYUVRgbBuf(pIn,size);
		
		CString filename = LogTool::SaveBmp(m_Width,m_Height, desrgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}

		delete [] desrgb24;
		
		m_IsCapturing = false;
	}

	delete [] rgb;

	//LogTool::WriteTimelog("end..","ProcessData5");

	return 1;
}

//图像缩小
int CameraDataProcess::ProcessData8(byte *pIn,int size)
{
	unsigned char *tpIn = MoveData1(pIn,size);

	unsigned char *rgb = new unsigned char[m_Width*m_Height*3];
	memset(rgb,0,m_Width*m_Height*3);

	int j = 0;
	for(int i=0;i<m_Width*m_Height*2*2;i+=8)
	{
		unsigned short y0 = (tpIn[i+1]*0x100+tpIn[i])>>2;
		unsigned short u = (tpIn[i+3]*0x100+tpIn[i+2])>>2;
		unsigned short y1 = (tpIn[i+5]*0x100+tpIn[i+4])>>2;
		unsigned short v = (tpIn[i+7]*0x100+tpIn[i+6])>>2;

		int R1 = y0 + 1.4075*(v-128);
		int G1 = y0-0.3455*(u-128)-0.7169 *(v-128);
		int B1 = y0 + 1.779*(u-128);

		int R2 = y1 + 1.4075*(v-128);
		int G2 = y1-0.3455*(u-128)-0.7169 *(v-128);
		int B2 = y1 + 1.779*(u-128);

		R1 = R1>255?255:R1;
		R1 = R1<0?0:R1;
		G1 = G1>255?255:G1;
		G1 = G1<0?0:G1;
		B1 = B1>255?255:B1;
		B1 = B1<0?0:B1;

		R2 = R2>255?255:R2;
		R2 = R2<0?0:R2;
		G2 = G2>255?255:G2;
		G2 = G2<0?0:G2;
		B2 = B2>255?255:B2;
		B2 = B2<0?0:B2;

		rgb[j] = B1;
		rgb[j+1] = G1;
		rgb[j+2] = R1;

		rgb[j+3] = B2;
		rgb[j+4] = G2;
		rgb[j+5] = R2;

		j+=6;
	}
	delete []tpIn;

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,this->m_DisplayWidth,this->m_DisplayHeight));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,this->m_DisplayWidth,this->m_DisplayHeight,0,0,m_Width,m_Height,rgb,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb);
		switch(m_PictureFormat)
		{
			case 0:
				break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}

	delete [] rgb;

	//LogTool::WriteTimelog("end..","ProcessData4");

	return 1;

}
//RAW 处理
int CameraDataProcess::ProcessData2(byte *pIn,int size)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;
	int imagesize = w * h * 2;
	//LogTool::CreateRAWFile(pIn,size);
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	unsigned short *d = ChangeByteToInt(d1,imagesize);//w*h
	unsigned char *rgb24 = new unsigned char[w*h*3];
	memset(rgb24,0,w*h*3);

	//BayerConversion10(d, rgb24, w, h);
	BayerConversion11(d, rgb24, w, h);

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width,m_Height));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width,m_Height,0,0,m_Width,m_Height,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}

	delete []d1;
	delete []d;
	delete []rgb24;

	return 1;
}
//RAW 处理2
int CameraDataProcess::ProcessData6(byte *pIn,int size)
{
	int w =	this->m_Width;
	int h = this->m_Height;

	
	int imagesize = this->m_Width *this->m_Height * 2;
	//LogTool::CreateRAWFile(pIn,size);
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	unsigned short *d = ChangeByteToInt(d1,imagesize);
	unsigned char *rgb24 = new unsigned char[this->m_Width*this->m_Height*3];
	memset(rgb24,0,this->m_Width*this->m_Height*3);

	BayerConversion10(d, rgb24, w, h);
    //LogTool::SaveBmp(m_Width,m_Height, rgb24);

	unsigned char *rgb241 = new unsigned char[w/2*h/2*3];
	memset(rgb241,0,w/2*h/2*3);
	int c=0;
	int w1 = w*3;
	for(int i=0;i<h;i+=2)
	{
		for(int j=0;j<w1;j+=6)
		{
			rgb241[c] = rgb24[j+i*w1];
			rgb241[c+1] = rgb24[j+1+i*w1];
			rgb241[c+2] = rgb24[j+2+i*w1];
			c+=3;
		}
	}
	//LogTool::SaveBmp(m_Width/2,m_Height/2, rgb241);	

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width/2,m_Height/2));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width/2,m_Height/2,0,0,m_Width/2,m_Height/2,rgb241,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(m_Width/2,m_Height/2, rgb241);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}

	delete []d1;
	delete []d;
	delete []rgb24;
	delete []rgb241;

	return 1;
}
//静态处理RAW处理
int CameraDataProcess::ProcessData3(byte *pIn,int size)
{
	long rowindex =0;
	CString strv;
	int w = 3264;
	int h = 2448;

	int imagesize = 3264 * 2448 * 2;
	//LogTool::CreateRAWFile(pIn,size);
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	//取第一行数据 w *2
	LogTool::CreateDataFile(d1,w*2*2);
	LogTool::CreateDataFile(&d1[w*2*2],w*2*2);

	//unsigned char *d = MoveData(d1,imagesize);
	unsigned short *d = ChangeByteToInt(d1,imagesize);

	//rowindex =0;
	//strv = LogTool::FormatLog(d,w,&rowindex);
	//LogTool::Writelog(strv,"d1");
	//LogTool::CreateDataFile(d,w);

	//rowindex =0;
	//strv = LogTool::FormatLog(&d[w],w,&rowindex);
	//LogTool::Writelog(strv,"d2");
	//LogTool::FormatRAWRowData(d,3264,2);
	//LogTool::CreateDataFile(&d[w],w);

	
	unsigned char *rgb24 = new unsigned char[w*h*3];
	memset(rgb24,0,w*h*3);

	//Bitmap8To242(d,rgb24,w,h);
	//Bayer2rgb24(rgb24, d, w,h);
	//LogTool::CreateRAWFile(d,size/2);
	BayerConversion10(d, rgb24, w, h);
	LogTool::SaveDIB2Bmp(3264,2448, rgb24);

	rowindex =0;
	strv = LogTool::FormatLog(rgb24,w*3,&rowindex);
	LogTool::Writelog(strv,"rgbdata1");

	rowindex =0;
	strv = LogTool::FormatLog(&rgb24[w*3],w*3,&rowindex);
	LogTool::Writelog(strv,"rgbdata2");

	unsigned char *newrgb24 = RGBReCreate(rgb24, 0, w, h);
	LogTool::SaveDIB2Bmp(3264,2448, newrgb24);

	delete []newrgb24;

	return 1;
}
BOOL CameraDataProcess::Bitmap8To24(BYTE* srcImage,BYTE* dstImage,LONG imageWidth,LONG imageHeight)
{
	 LONG lLineBytes24=((imageWidth*24+31)/32*4);
	 LONG lLineBytes8=((imageWidth*8+31)/32*4);
	 int n,j;
	 for(int i=0;i<imageHeight;i++)
	 {
		  for(j=0,n=0;j<lLineBytes8;j++,n++)
		  {
			  if(lLineBytes8*i+j>7962539) return true;

		   BYTE gray=*(srcImage+lLineBytes8*i+j);
		   *(dstImage+lLineBytes24*i+n)=gray;
		   n++;
		   *(dstImage+lLineBytes24*i+n)=gray;
		   n++;
		   *(dstImage+lLineBytes24*i+n)=gray;
		  }
	 }
 
	return true;
}
BOOL  CameraDataProcess::Bayer10ToBitmap24(BYTE* src,LONG imageWidth,LONG imageHeight,BYTE* bitmap)
{
	for(int i=0;i<imageHeight;i++)
	{
		for(int j=0;j<imageWidth;j++)
		{

		}
	}
	return true;
}
BOOL CameraDataProcess::Bitmap8To242(BYTE* image1,BYTE* image3,LONG imageWidth,LONG imageHeight)
{
	 LONG lLineBytes24=((imageWidth*24+31)/32*4);
	 LONG lLineBytes8=((imageWidth*8+31)/32*4);
	 W = lLineBytes8;
	 ColorType ct;
	 UINT iY = 0, iX = 0;
	 RGBSumData SumData = {0};
	 ULONG lIdx = 0;
	 for(int i=0;i<imageHeight;i++)
	 {
		  for(int j=0;j<lLineBytes8;j++)
		  {
				if(lLineBytes8*i+j>7962539-962539) return true;

				if (0 == i%2 && 0 == j%2)
				{
					ct = B;
				}
				else if (0 == i%2 && 1 == j%2)
				{
					ct = Gb;
				}
				else if (1 == i%2 && 0 == j%2)
				{
					ct = Gr;
				}
				else
				{
					ct = R;
				}		

				iY = i;
				iX = j;
		

				switch (ct)
				{
					case B:			
						BToRGB(iY, iX, image1, SumData);
						break;
					case Gb:
						GbToRGB(iY, iX, image1, SumData);
						break;
					case Gr:
						GrToRGB(iY, iX, image1, SumData);
						break;
					case R:	
						RToRGB(iY, iX, image1, SumData);
						break;			
				}

				image3[lIdx]	= (BYTE)SumData.dwBout;
				image3[lIdx+1]	= (BYTE)SumData.dwGout;
				image3[lIdx+2]	= (BYTE)SumData.dwRout;

				lIdx += 3;
		  }
	 }
 
	return true;
}
void CameraDataProcess::BToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData)
{
	BYTE b[9];
	BYTE g[12];
	BYTE r[4];

	b[0] = pImage[iY*W+iX];
	b[1] = pImage[iY*W+iX+2];
	b[2] = pImage[iY*W+iX+4];

	b[3] = pImage[(iY+2)*W+iX];
	b[4] = pImage[(iY+2)*W+iX+2];
	b[5] = pImage[(iY+2)*W+iX+4];

	b[6] = pImage[(iY+4)*W+iX];
	b[7] = pImage[(iY+4)*W+iX+2];
	b[8] = pImage[(iY+4)*W+iX+4];

	SumData.dwBout = b[0] + (b[1])*6 + b[2]
			+ (b[3])*6 + (b[4])*36 + (b[5])*6
			+ b[6] + (b[7])*6 + b[8];
	

	g[0] = pImage[iY*W+iX+1];
	g[1] = pImage[iY*W+iX+3];

	g[2] = pImage[(iY+1)*W+iX];
	g[3] = pImage[(iY+1)*W+iX+2];
	g[4] = pImage[(iY+1)*W+iX+4];

	g[5] = pImage[(iY+2)*W+iX+1];
	g[6] = pImage[(iY+2)*W+iX+3];

	g[7] = pImage[(iY+3)*W+iX];
	g[8] = pImage[(iY+3)*W+iX+2];
	g[9] = pImage[(iY+3)*W+iX+4];

	g[10] = pImage[(iY+4)*W+iX+1];
	g[11] = pImage[(iY+4)*W+iX+3];

	SumData.dwGout = g[0] + g[1]
			+ g[2] + (g[3])*14 + g[4]
			+ (g[5])*14 + (g[6])*14
			+ g[7] + (g[8])*14 + g[9]
			+ g[10] + g[11];
	


	r[0] = pImage[(iY+1)*W+iX+1];
	r[1] = pImage[(iY+1)*W+iX+3];
	r[2] = pImage[(iY+3)*W+iX+1];
	r[3] = pImage[(iY+3)*W+iX+3];

	SumData.dwRout = (r[0] + r[1] + r[2] + r[3])*16;
	
	AjustValue(SumData);
	
}

void CameraDataProcess::GbToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData)
{
	BYTE b[6];
	BYTE g[9];
	BYTE r[6];

	b[0] = pImage[iY*W+iX+1];
	b[1] = pImage[iY*W+iX+3];
	b[2] = pImage[(iY+2)*W+iX+1];
	b[3] = pImage[(iY+2)*W+iX+3];
	b[4] = pImage[(iY+4)*W+iX+1];
	b[5] = pImage[(iY+4)*W+iX+3];

	SumData.dwBout = (b[0] + b[1])*4 
			+ (b[2] + b[3])*24
			+ (b[4] + b[5])*4;

	g[0] = pImage[iY*W+iX+2];

	g[1] = pImage[(iY+1)*W+iX+1];
	g[2] = pImage[(iY+1)*W+iX+3];
	
	g[3] = pImage[(iY+2)*W+iX];
	g[4] = pImage[(iY+2)*W+iX+2];
	g[5] = pImage[(iY+2)*W+iX+4];

	g[6] = pImage[(iY+3)*W+iX+1];
	g[7] = pImage[(iY+3)*W+iX+3];

	g[8] = pImage[(iY+4)*W+iX+2];

	SumData.dwGout = (g[0])*2
			+ (g[1] + g[2])*8
			+ (g[3])*2 + (g[4])*24 + (g[5])*2
			+ (g[6] + g[7])*8
			+ (g[7])*2;

	r[0] = pImage[(iY+1)*W+iX];
	r[1] = pImage[(iY+1)*W+iX+2];
	r[2] = pImage[(iY+1)*W+iX+4];

	r[3] = pImage[(iY+3)*W+iX];
	r[4] = pImage[(iY+3)*W+iX+2];
	r[5] = pImage[(iY+3)*W+iX+4];

	SumData.dwRout = (r[0])*4 + (r[1])*24 + (r[2])*4
			+ (r[3])*4 + (r[4])*24 + (r[5])*4;


	AjustValue(SumData);


}

void CameraDataProcess::GrToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData)
{
	BYTE b[6];
	BYTE g[9];
	BYTE r[6];

	b[0] = pImage[(iY+1)*W+iX];
	b[1] = pImage[(iY+1)*W+iX+2];
	b[2] = pImage[(iY+1)*W+iX+4];

	b[3] = pImage[(iY+3)*W+iX];
	b[4] = pImage[(iY+3)*W+iX+2];
	b[5] = pImage[(iY+3)*W+iX+4];

	SumData.dwBout = (b[0] + b[1] + b[2])*4
			+ (b[3] + b[4] + b[5])*4;

	g[0] = pImage[iY*W+iX+2];
	
	g[1] = pImage[(iY+1)*W+iX+1];
	g[2] = pImage[(iY+1)*W+iX+3];

	g[3] = pImage[(iY+2)*W+iX];
	g[4] = pImage[(iY+2)*W+iX+2];
	g[5] = pImage[(iY+2)*W+iX+4];

	g[6] = pImage[(iY+3)*W+iX+1];
	g[7] = pImage[(iY+3)*W+iX+3];

	g[8] = pImage[(iY+4)*W+iX+2];

	SumData.dwGout = (g[0])*2
			+ (g[1] + g[2])*8
			+ (g[3])*2 + (g[4])*24 + (g[5])*2
			+ (g[6] + g[7])*8
			+ (g[8])*2;

	r[0] = pImage[iY*W+iX];
	r[1] = pImage[iY*W+iX+3];

	r[2] = pImage[(iY+2)*W+iX+1];
	r[3] = pImage[(iY+2)*W+iX+3];

	r[4] = pImage[(iY+4)*W+iX+1];
	r[5] = pImage[(iY+4)*W+iX+3];

	SumData.dwRout = (r[0] + r[1])*4
			+ (r[2] + r[3])*24
			+ (r[4] + r[5])*4;

	AjustValue(SumData);

}

void CameraDataProcess::RToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData)
{
	BYTE r[9];
	BYTE g[12];
	BYTE b[4];
	
	r[0] = pImage[iY*W+iX];
	r[1] = pImage[iY*W+iX+2];
	r[2] = pImage[iY*W+iX+4];

	r[3] = pImage[(iY+2)*W+iX];
	r[4] = pImage[(iY+2)*W+iX+2];
	r[5] = pImage[(iY+2)*W+iX+4];

	r[6] = pImage[(iY+4)*W+iX];
	r[7] = pImage[(iY+4)*W+iX+2];
	r[8] = pImage[(iY+4)*W+iX+4];

	SumData.dwRout = r[0] + (r[1])*6 + r[2]
			+ (r[3])*6 + (r[4])*36 + (r[5])*6
			+ r[6] + (r[7])*6 + r[8];

	g[0] = pImage[iY*W+iX+1];
	g[1] = pImage[iY+W+iX+3];
	
	g[2] = pImage[(iY+1)*W+iX];
	g[3] = pImage[(iY+1)*W+iX+2];
	g[4] = pImage[(iY+1)*W+iX+4];

	g[5] = pImage[(iY+2)*W+iX+1];
	g[6] = pImage[(iY+2)*W+iX+3];

	g[7] = pImage[(iY+3)*W+iX];
	g[8] = pImage[(iY+3)*W+iX+2];
	g[9] = pImage[(iY+3)*W+iX+4];

	g[10] = pImage[(iY+4)*W+iX+1];
	g[11] = pImage[(iY+4)*W+iX+3];

	SumData.dwGout = g[0] + g[1]
		+ g[2] + (g[3])*14 + g[4]
		+ (g[5] + g[6])*14
		+ g[7] + (g[8])*14 + g[9]
		+ g[10] + g[11];

	b[0] = pImage[(iY+1)*W+iX+1];
	b[1] = pImage[(iY+1)*W+iX+3];

	b[2] = pImage[(iY+3)*W+iX+1];
	b[3] = pImage[(iY+3)*W+iX+3];

	SumData.dwBout = (b[0] + b[1] + b[2] + b[3])*16;

	AjustValue(SumData);
	
}


void CameraDataProcess::AjustValue(RGBSumData& SumData)
{
	SumData.dwBout = SumData.dwBout>>4;
	SumData.dwGout = SumData.dwGout>>4;
	SumData.dwRout = SumData.dwRout>>4;
	

	SumData.dwBout = SumData.dwBout > 255 ? 255 : SumData.dwBout;
	SumData.dwGout = SumData.dwGout > 255 ? 255 : SumData.dwGout;
	SumData.dwRout = SumData.dwRout > 255 ? 255 : SumData.dwRout;
}
int CameraDataProcess::ByteToRGB( byte *pIn ,tagRGB* pOut )
{
	tagRGB sTempRgb;
	for(int i=0;i<m_Height;i+=2)
	{
		for(int j=0;j<m_Width;j+=2)
		{
			sTempRgb.R=pIn[Array2_Index(i,j)];
			sTempRgb.G=(pIn[Array2_Index(i,j+1)]>>1)+(pIn[Array2_Index(i+1,j)]>>1);
			sTempRgb.B=pIn[Array2_Index(i+1,j+1)];
			RgbChangeProc(pOut[Array2_Index(i,j)],sTempRgb);
		}
	}
	for(int i=0;i<m_Height;i+=2)
	{
		for(int j=1;j<m_Width-1;j+=2)
		{
			sTempRgb.R=pIn[Array2_Index(i,j+1)];
			sTempRgb.G=(pIn[Array2_Index(i,j)]>>1)+(pIn[Array2_Index(i+1,j+1)]>>1);
			sTempRgb.B=pIn[Array2_Index(i+1,j)];
			RgbChangeProc(pOut[Array2_Index(i,j)],sTempRgb);
		}
	}
	for(int i=1;i<m_Height-1;i+=2)
	{
		for(int j=0;j<m_Width;j+=2)
		{
			sTempRgb.R=pIn[Array2_Index(i+1,j)];
			sTempRgb.G=(pIn[Array2_Index(i,j)]>>1)+(pIn[Array2_Index(i+1,j+1)]>>1);
			sTempRgb.B=pIn[Array2_Index(i,j+1)];
			RgbChangeProc(pOut[Array2_Index(i,j)],sTempRgb);
		}
	}
	for(int i=1;i<m_Height-1;i+=2)
	{
		for(int j=1;j<m_Width-1;j+=2)
		{
			sTempRgb.R=pIn[Array2_Index(i+1,j+1)];
			sTempRgb.G=(pIn[Array2_Index(i,j+1)]>>1)+(pIn[Array2_Index(i+1,j)]>>1);
			sTempRgb.B=pIn[Array2_Index(i,j)];
			RgbChangeProc(pOut[Array2_Index(i,j)],sTempRgb);
		}
	}
	return 0;
}
void CameraDataProcess::RgbChangeProc( tagRGB& DestRgb,const tagRGB& OrgRgb )
{
	switch(m_ChangeType)
	{
	case RB_Change:
		DestRgb.R=OrgRgb.B;
		DestRgb.G=OrgRgb.G;
		DestRgb.B=OrgRgb.R;
		break;
	case RG_Change:
		DestRgb.R=OrgRgb.G;
		DestRgb.G=OrgRgb.R;
		DestRgb.B=OrgRgb.B;
		break;
	case GB_Change:
		DestRgb.R=OrgRgb.R;
		DestRgb.G=OrgRgb.B;
		DestRgb.B=OrgRgb.G;
		break;
	case Normal_Change:
	default:
		DestRgb.R=OrgRgb.R;
		DestRgb.G=OrgRgb.G;
		DestRgb.B=OrgRgb.B;
		break;
	}
}
void CameraDataProcess::yuv422_to_rgb24(unsigned char *yuv422,unsigned char *rgb24, int width,int height) 
{
	int x,y;
	unsigned char *yuv444;
	yuv444 = (unsigned char *) malloc(sizeof(unsigned char) * width * height * 3);
	for(x = 0,y = 0;x < width*height*2,y < width*height*3;x+=4,y+=6)
	{
		yuv444[y] = yuv422[x];   
		yuv444[y+1] = yuv422[x+1];  
		yuv444[y+2] = yuv422[x+3];   
		yuv444[y+3] = yuv422[x+2];   
		yuv444[y+4] = yuv422[x+1];   
		yuv444[y+5] = yuv422[x+3]; 
	}
	for(x = 0;x < width*height*3;x+=3)
	{
		rgb24[x+2] = yuv444[x] + 1.402*(yuv444[x+2] - 128); 
		rgb24[x+1] = yuv444[x]-0.34414*(yuv444[x+1]-128)-0.71414*(yuv444[x+2]-128);    
		rgb24[x] = yuv444[x] + 1.772*(yuv444[x+1] - 128);

		if (rgb24[x]>255) rgb24[x]=255;      
		if (rgb24[x]<0) rgb24[x]=0;       
		if (rgb24[x+1]>255) rgb24[x+1]=255;      
		if (rgb24[x+1]<0) rgb24[x+1]=0;      
		if (rgb24[x+2]>255) rgb24[x+2]=255;      
		if (rgb24[x+2]<0) rgb24[x+2]=0; 
	}

	free(yuv444); 
}
//移位数据
unsigned char * CameraDataProcess::MoveData(unsigned char *pIn, long size)
{
	unsigned char d1,d2,d4;
	int d3;
	unsigned char *pTemp = new unsigned char[size/2];
	memset(pTemp,0x00,(size/2));
	for(int i=0,j=0;i<size;i+=2,j++)
	{
		d1 = pIn[i+1];
		d2 = pIn[i];
		d3 = d1*0x100+d2;
		d4 = d3>>2;
		pTemp[j] = d4;

	}

	return pTemp;
}
unsigned short * CameraDataProcess::ChangeByteToInt(unsigned char *pIn, long size)
{
	unsigned short *pInt = new unsigned short[size/2];
	memset(pInt,0x00,(size/2));

	for(int i=0,j=0;i<size;i+=2,j++)
	{
		unsigned char d1 = pIn[i+1];
		unsigned char d2 = pIn[i];
		unsigned short d3 = d1*0x100+d2;

		pInt[j] = d3;

	}

	return pInt;
}
//对应马国良下位机程序
unsigned char * CameraDataProcess::MoveData1(unsigned char *pIn, long size)
{
	unsigned char *pTemp = new unsigned char[size];
    
	pTemp[0] = 0xDF;
	pTemp[1] = 0x03;

	memcpy(&pTemp[2],pIn,size-2);

	//delete [] pIn;

	return pTemp;
}
//降低数据量 m_Width*m_Height*4
unsigned char * CameraDataProcess::MoveData3(unsigned char *pIn,long sizedata,int sizeimage)
{
	int r = sizedata/sizeimage;
	r=r/2;
	r = r==0?1:r;
	unsigned char *pTemp = new unsigned char[sizeimage];
	memset(pTemp,0x00,sizeimage);
	for(int i = 0,j=0; i < sizeimage;i+=8, j += 8*r)
	{
		pTemp[i+0] = pIn[j+0];
		pTemp[i+1] = pIn[j+1];
		pTemp[i+2] = pIn[j+2];
		pTemp[i+3] = pIn[j+3];
		pTemp[i+4] = pIn[j+4];
		pTemp[i+5] = pIn[j+5];
		pTemp[i+6] = pIn[j+6];
		pTemp[i+7] = pIn[j+7];
	}

	return pTemp;
}
//对应SDR下位机程序
unsigned char * CameraDataProcess::MoveData2(unsigned char *pIn, long size)
{
	unsigned char *pTemp = new unsigned char[size];
    
	pTemp[0] = 0xDF;
	pTemp[1] = 0x03;
	pTemp[2] = 0x03;
	pTemp[3] = 0x02;

	memcpy(&pTemp[4],&pIn[2],size-4);

	//delete [] pIn;

	return pTemp;
}
int CameraDataProcess::Convert_yuv_to_rgb_pixel(int y, int u, int v)
 {
         unsigned int pixel32 = 0;
         unsigned char *pixel = (unsigned char *)&pixel32;
         int r, g, b;
         r = y + (1.370705 * (v-128));
         g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
         b = y + (1.732446 * (u-128));
         if(r > 255) r = 255;
         if(g > 255) g = 255;
         if(b > 255) b = 255;
         if(r < 0) r = 0;
         if(g < 0) g = 0;
         if(b < 0) b = 0;
         pixel[0] = r ;
         pixel[1] = g ;
         pixel[2] = b ;

         return pixel32;
 }

int CameraDataProcess::Convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
         unsigned int in, out = 0;
         unsigned int pixel_16;
         unsigned char pixel_24[3];
         unsigned int pixel32;
         int y0, u, y1, v;
 
         for(in = 0; in < width * height * 2; in += 4)
         {
                 pixel_16 = yuv[in + 3] << 24 | yuv[in + 2] << 16 | yuv[in + 1] <<  8 | yuv[in + 0];
                 y0 = (pixel_16 & 0x000000ff);
                 u  = (pixel_16 & 0x0000ff00) >>  8;
                 y1 = (pixel_16 & 0x00ff0000) >> 16;
                 v  = (pixel_16 & 0xff000000) >> 24;
                 pixel32 = Convert_yuv_to_rgb_pixel(y0, u, v);
                 pixel_24[0] = (pixel32 & 0x000000ff);
                 pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
                 pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
                 rgb[out++] = pixel_24[0];
                 rgb[out++] = pixel_24[1];
                 rgb[out++] = pixel_24[2];
                 pixel32 = Convert_yuv_to_rgb_pixel(y1, u, v);
                 pixel_24[0] = (pixel32 & 0x000000ff);
                 pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
                 pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
                 rgb[out++] = pixel_24[0];
				 rgb[out++] = pixel_24[1];
                 rgb[out++] = pixel_24[2];
         }
         return 0;
}

BufData CameraDataProcess::ReadFile(CString filePath)
{
	CFile*  pFile = new CFile;

	BufData buf;
		
	if(!pFile->Open(filePath,CFile::modeRead|CFile::typeBinary ))
	{
		buf.pBuf = NULL;
		buf.size = 0;
		return buf;
	}

	pFile-> SeekToBegin();
	long c = pFile->SeekToEnd();
	unsigned char *data = new unsigned char[c];
	memset(data,0x00,c);
	pFile-> SeekToBegin();

	pFile->Read(data,c);
	pFile->Close();

	

	buf.pBuf = data;
	buf.size = c;

	return buf;
 } 
unsigned char * CameraDataProcess::ProcessYUVData(unsigned char *pIn)
{
	long length = m_Width*m_Height*2;
	unsigned char *pTemp = new unsigned char[length];
	memset(pTemp,0x00,length);

	for(int i=0,j=0;i<length;i+=2,j++)
	{
		pTemp[j] = pIn[i+1]*0x100+pIn[i];
	}

	return pTemp;
}
void CameraDataProcess::liyuv422_to_rgb_buffer(unsigned char *data, unsigned char *rgb, unsigned int width, unsigned int height)
{
	unsigned char *Y = new unsigned char[m_Width*m_Height];
	memset(Y,0x00,m_Width*m_Height);
	unsigned char *U = new unsigned char[m_Width*m_Height/2];
	memset(U,0x00,m_Width*m_Height/2);
	unsigned char *V = new unsigned char[m_Width*m_Height/2];
	memset(V,0x00,m_Width*m_Height/2);

	int YJ = 0;
	for(int i=0;i<m_Width*m_Height;i++)
	{
		Y[i] = data[YJ];
		YJ += 2;
	}
	
	int UJ = 1;
	for(int i=0;i<m_Width*m_Height/2;i++)
	{
		U[i] = data[UJ];
		UJ += 4;
	}
	

	int VJ = 3;
	for(int i=0;i<m_Width*m_Height/2;i++)
	{
		V[i] = data[VJ];
		VJ += 4;
	}

	//R = Y + 1.14V
	//G = Y - 0.39U - 0.58V
	//B = Y + 2.03U
	for(int i=0,j=0;i<m_Width*m_Height;i+=2,j+=6)
	{
		unsigned char Y1 = Y[i];
		unsigned char Y2 = Y[i+1];

		unsigned char UD = U[i/2];
		unsigned char VD = V[i/2];

		int R1 = Y1 + 1.4075*(VD-128);
		int G1 = Y1-0.3455*(UD-128)-0.7169 *(VD-128);
		int B1 = Y1 + 1.779*(UD-128);

		int R2 = Y2 + 1.4075*(VD-128);
		int G2 = Y2-0.3455*(UD-128)-0.7169 *(VD-128);
		int B2 = Y2 + 1.779*(UD-128);

		R1 = R1>255?255:R1;
		R1 = R1<0?0:R1;
		G1 = G1>255?255:G1;
		G1 = G1<0?0:G1;
		B1 = B1>255?255:B1;
		B1 = B1<0?0:B1;

		R2 = R2>255?255:R2;
		R2 = R2<0?0:R2;
		G2 = G2>255?255:G2;
		G2 = G2<0?0:G2;
		B2 = B2>255?255:B2;
		B2 = B2<0?0:B2;

		rgb[j] = B1;
		rgb[j+1] = G1;
		rgb[j+2] = R1;

		rgb[j+3] = B2;
		rgb[j+4] = G2;
		rgb[j+5] = R2;
	}

	delete [] Y;
	delete [] U;
	delete [] V;
}

//用第rowid数据重构RGB数据
unsigned char * CameraDataProcess::RGBReCreate(unsigned char *rgb, int rowid, unsigned int width, unsigned int height)
{
	unsigned char *d = new unsigned char[width*height*3];
	memset(d,0x00,width*height*3);
	
	int n = rowid*width*3;
	unsigned char * t = &rgb[n];
	int l =  width*3;
	for(int i=0;i<height;i++)
	{
		memcpy(&d[i*l],t,l);
	}

	return d;
}
void CameraDataProcess::Bayer2rgb24(unsigned char *dst, unsigned short *src, long int WIDTH, long int HEIGHT)
{
    long int i;
    unsigned short *rawpt;
	unsigned char *scanpt;
    long int size;

    rawpt = src;
    scanpt = dst;
    size = WIDTH*HEIGHT;

    for ( i = 0; i < size; i++ ) 
    {
		if ( (i/WIDTH) % 2 == 0 )      //奇数行
		{
	    	if ( (i % 2) == 0 )      //奇数列
	    	{
				/* B */
				if ( (i > WIDTH) && ((i % WIDTH) > 0) ) 
				{
		    		*scanpt++ = (*(rawpt-WIDTH-1)+*(rawpt-WIDTH+1)+ *(rawpt+WIDTH-1)+*(rawpt+WIDTH+1))/4;	/* R */
		    		*scanpt++ = (*(rawpt-1)+*(rawpt+1)+ *(rawpt+WIDTH)+*(rawpt-WIDTH))/4;	/* G */
		    		*scanpt++ = *rawpt;					/* B */
				} 
				else       /* first line or left column */
				{
		    										
		    		*scanpt++ = *(rawpt+WIDTH+1);		/* R */
		   			*scanpt++ = (*(rawpt+1)+*(rawpt+WIDTH))/2;	/* G */
		   			*scanpt++ = *rawpt;				/* B */
				}
	    	} 
	    	else 
	    	{
				/* (B)G */
				if ( (i > WIDTH) && ((i % WIDTH) < (WIDTH-1)) ) 
				{
		    			*scanpt++ = (*(rawpt+WIDTH)+*(rawpt-WIDTH))/2;	/* R */
		    			*scanpt++ = *rawpt;					/* G */
		    			*scanpt++ = (*(rawpt-1)+*(rawpt+1))/2;		/* B */
				} 
				else /* first line or right column */
				{
		    									
		    		*scanpt++ = *(rawpt+WIDTH);	/* R */
		    		*scanpt++ = *rawpt;		/* G */
		    		*scanpt++ = *(rawpt-1);	/* B */
				}
	    	}
		} 
		else    //偶数行
		{
			if ( (i % 2) == 0 )  //奇数列
			{
				/* G(R) */
				if ( (i < (WIDTH*(HEIGHT-1))) && ((i % WIDTH) > 0) ) 
				{
					*scanpt++ = (*(rawpt-1)+*(rawpt+1))/2;		/* R */
					*scanpt++ = *rawpt;					/* G */
					*scanpt++ = (*(rawpt+WIDTH)+*(rawpt-WIDTH))/2;	/* B */
				} 
				else 
				{
						/* bottom line or left column */
					*scanpt++ = *(rawpt+1);		/* R */
					*scanpt++ = *rawpt;			/* G */
					*scanpt++ = *(rawpt-WIDTH);		/* B */
				}
		} 
		else 
		{
			/* R */
			if ( i < (WIDTH*(HEIGHT-1)) && ((i % WIDTH) < (WIDTH-1)) ) 
			{
					*scanpt++ = *rawpt;					/* R */
					*scanpt++ = (*(rawpt-1)+*(rawpt+1)+ *(rawpt-WIDTH)+*(rawpt+WIDTH))/4;	/* G */
					*scanpt++ = (*(rawpt-WIDTH-1)+*(rawpt-WIDTH+1)+ *(rawpt+WIDTH-1)+*(rawpt+WIDTH+1))/4;	/* B */
			} 
			else 
			{
				/* bottom line or right column */
				*scanpt++ = *rawpt;				/* R */
				*scanpt++ = (*(rawpt-1)+*(rawpt-WIDTH))/2;	/* G */
				*scanpt++ = *(rawpt-WIDTH-1);		/* B */
			}
		}
	}
	rawpt++;
    }
}	
void CameraDataProcess::BayerConversion10(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight)
{
	int g = 0;
	int r = 0;
	int b = 0;
	int c =0;
    for(int i=0;i<nHeight-1;i++)
	{
		g = 0;
		r = 0;
		b = 0;
		if(i%2==0)//0,2,4行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)
				{
					g = pIn[j+i*nWidth];//G
					unsigned short r1 = pIn[j+1+i*nWidth];//R1
					unsigned short r2 = r1;//R2
					if(j-1>=0)
					{
						r2 = pIn[j-1+i*nWidth];//R1
					}
					r = (r1+r2)/2;

					unsigned short b1 = pIn[j+(i+1)*nWidth];//b1
					unsigned short b2 = b1;//b2
					if(i-1>=0)
					{
						b2 = pIn[j+(i-1)*nWidth];//b2
					}
					b = (b1+b2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;

					c += 3;
				}
				else
				{
					r = pIn[j+i*nWidth];//R

					unsigned short g1 = pIn[j+1+i*nWidth];//g1
					unsigned short g2 = pIn[j-1+i*nWidth];//g2
					g = (g1+g2)/2;

					unsigned short b1 = pIn[(j-1)+(i+1)*nWidth];//b1
					unsigned short b2 = pIn[(j+1)+(i+1)*nWidth];//b2

					b =  (b1+b2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
			}
		}
		else//1,3,5换行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)
				{
					b = pIn[j+i*nWidth];//B

					unsigned short g1 = pIn[j+(i-1)*nWidth];//g1
					unsigned short g2 = pIn[j+(i+1)*nWidth];//g2
					g = (g1+g2)/2;

					unsigned short r1 = pIn[j+1+(i+1)*nWidth];//R1
					unsigned short r2 =  pIn[j+1+(i-1)*nWidth];;//R2

					r = (r1+r2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
				else
				{
					g = pIn[j+i*nWidth];//G

					unsigned short r1 = pIn[j+(i-1)*nWidth];//r1
					unsigned short r2 = pIn[j+(i+1)*nWidth];//r2
					r = (r1+r2)/2;

					unsigned short b1 = pIn[(j-1)+nWidth];//b1
					unsigned short b2 = pIn[(j+1)+nWidth];//b2
					b =  (b1+b2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
			}
		}

		
	}
}
//
void CameraDataProcess::BayerConversion11(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight)
{
	int g = 0;
	int r = 0;
	int b = 0;
	int c =0;
	int c1=0;
	unsigned short ga =0;

    for(int i=0;i<nHeight;i++)
	{
		g = 0;
		r = 0;
		b = 0;
		if(i%2==0)//0,2,4行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)//0,2,4列
				{
					g = pIn[j+i*nWidth];//G

					r=0;
					unsigned short r1 = 0;//R1
					unsigned short r2 = 0;//R2
					c1=0;
					ga =0;
					if(j!=nWidth-1 )
					{
						r1 = pIn[(j+1)+i*nWidth];//R1
						ga += r1;
						c1++;
					}
					if(j!=0)
					{
						r2 = pIn[(j-1)+i*nWidth];//R2
						ga += r2;
						c1++;
					}
					r = ga/c1;

					b=0;
					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					c1=0;
					ga =0;
					if(i!=nHeight-1)
					{
						b1 = pIn[j+(i+1)*nWidth];//b1
						ga += b1;
						c1++;
					}
					if(i!=0)
					{
						b2 = pIn[j+(i-1)*nWidth];//b2
						ga += b2;
						c1++;
					}
					b = ga/c1;

					if(c==4893+nWidth*3*i)
					{
						int z=c;
					}

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;

					c += 3;
				}
				else//1,3,5列
				{
					r = pIn[j+i*nWidth];//R

					g=0;
					unsigned short g1 = 0;//g1
					unsigned short g2 =0;//g2
					unsigned short g3 = 0;//g3
					unsigned short g4 = 0;//g4
					c1=0;
					ga =0;
					if(j!=nWidth-1 )
					{
						g1 = pIn[(j+1)+i*nWidth];//g1
						ga += g1;
						c1++;
					}
					if(j!=0 )
					{
						g2 = pIn[(j-1)+i*nWidth];//g2
						ga += g2;
						c1++;
					}
					if(i!=0)
					{
						g3 = pIn[j+(i-1)*nWidth];//g3
						ga += g3;
						c1++;
					}
					if(i!=nHeight-1)
					{
						g4 = pIn[j+(i+1)*nWidth];//g4
						ga += g4;
						c1++;
					}
					g = ga/c1;

					//--------------?
					if(c==4893+nWidth*3*i)
					{
						int z=c;
					}
					b=0;
					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					unsigned short b3 = 0;//b1
					unsigned short b4 = 0;//b2
					c1=0;
					ga =0;
					if(j!=0 && i!=nHeight-1)
					{
						b1 = pIn[(j-1)+(i+1)*nWidth];//b1
						ga += b1;
						c1++;
					}
					/*
					if(j!=nWidth-1 && i!=nHeight-1)
					{
						b2 = pIn[(j+1)+(i+1)*nWidth];//b2
						ga += b2;
						c1++;
					}
					*/
					if(j!=0 && i!=0)
					{
						b3 = pIn[(j-1)+(i-1)*nWidth];//b1
						ga += b3;
						c1++;
					}
					/*
					if(j!=nWidth-1 && i!=0)
					{
						b4 = pIn[(j+1)+(i-1)*nWidth];//b2
						ga += b4;
						c1++;
					}
					*/
					b = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
			}
		}
		else//1,3,5行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)//0,2,4列
				{
					b = pIn[j+i*nWidth];//B

					/*
					if(j==1224)
					{
						int a=11;
					}
					*/

					unsigned short g1 = 0;
					unsigned short g2 = 0;
					unsigned short g3 = 0;
					unsigned short g4 = 0;
					c1=0;
					ga =0;
					if(i!=0)
					{
						g1 = pIn[j+(i-1)*nWidth];
						ga += g1;
						c1++;
					}
					if(i!=nHeight-1)
					{
						g2 = pIn[j+(i+1)*nWidth];
						ga += g2;
						c1++;
					}
					/*
					if(j!=nWidth-1)
					{
						g3 = pIn[(j+1)+i*nWidth];
						ga += g3;
						c1++;
					}
					if(j!=0)
					{
						g4 = pIn[(j-1)+i*nWidth];
						ga += g4;
						c1++;
					}
					*/
					g = ga/c1;


					unsigned short r1 = 0;
					unsigned short r2 = 0;
					unsigned short r3 = 0;
					unsigned short r4 = 0;
					c1=0;
					ga =0;
					if(j!=nWidth-1 && i!=nHeight-1)
					{
						r1 = pIn[(j+1)+(i+1)*nWidth];//R1
						ga += r1;
						c1++;
					}
					if(j!=nWidth-1 && i!=0)
					{
						r2 =  pIn[(j+1)+(i-1)*nWidth];;//R2
						ga += r2;
						c1++;
					}
					if(j!=0 && i!=nHeight-1)
					{
						r3 = pIn[(j-1)+(i+1)*nWidth];//R3
						ga += r3;
						c1++;
					}
					if(j!=0 && i!=0)
					{
						r4 =  pIn[(j-1)+(i-1)*nWidth];//R4
						ga += r4;
						c1++;
					}
					r = ga/c1;

					if(c==4893+nWidth*3*i)
					{
						int z=c;
					}
					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
				else//1,3,5列
				{
					g = pIn[j+i*nWidth];//G

					unsigned short r1 = 0;
					unsigned short r2 = 0;
					c1=0;
					ga =0;
					if(i!=0)
					{
						r1 = pIn[j+(i-1)*nWidth];//r1
						ga += r1;
						c1++;
					}
					if(i!=nHeight-1)
					{
						r2 = pIn[j+(i+1)*nWidth];//r2
						ga += r2;
						c1++;
					}
					r = ga/c1;

					if(c==4893+nWidth*3*i)
					{
						int z=c;
					}
					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					c1=0;
					ga =0;
					if(j!=0)
					{
						b1 = pIn[(j-1)+i*nWidth];//b1
						ga += b1;
						c1++;
					}
					
					/*
					if(j!=nWidth-1 )
					{
						b2 = pIn[(j+1)+i*nWidth];//b2
						ga += b2;
						c1++;
					}
					*/
					
					b = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
			}
		}
	}

	/*
	LogTool::FormatRGBRowData(pOut,nWidth,0);
	LogTool::FormatRGBRowData(pOut,nWidth,1);
	LogTool::FormatRGBRowData(pOut,nWidth,2);
	LogTool::FormatRGBRowData(pOut,nWidth,3);
	LogTool::FormatRGBRowData(pOut,nWidth,4);
	LogTool::FormatRGBRowData(pOut,nWidth,5);
	LogTool::FormatRGBRowData(pOut,nWidth,6);
	LogTool::FormatRGBRowData(pOut,nWidth,7);
	LogTool::FormatRGBRowData(pOut,nWidth,8);
	LogTool::FormatRGBRowData(pOut,nWidth,9);
	LogTool::FormatRGBRowData(pOut,nWidth,10);
	*/
}
void CameraDataProcess::BayerConversion14(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight)
{
	int g = 0;
	int r = 0;
	int b = 0;
	int c =0;
	int c1=0;
	unsigned short ga =0;

	omp_set_num_threads(3);
	#pragma omp parallel for
    for(int i=0;i<nHeight;i++)
	{
		g = 0;
		r = 0;
		b = 0;
		if(i%2==0)//0,2,4行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)//0,2,4列
				{
					r = pIn[j+i*nWidth];//R

					g=0;
					unsigned short g1 = 0;//g1
					unsigned short g2 =0;//g2
					unsigned short g3 = 0;//g3
					unsigned short g4 = 0;//g4
					c1=0;
					ga =0;

					/*
					if(j!=nWidth-1 )
					{
						g1 = pIn[(j+1)+i*nWidth];//g1
						ga += g1;
						c1++;
					}
					if(j!=0 )
					{
						g2 = pIn[(j-1)+i*nWidth];//g2
						ga += g2;
						c1++;
					}
					*/
					if(i!=0)
					{
						g3 = pIn[j+(i-1)*nWidth];//g3
						ga += g3;
						c1++;
					}
					if(i!=nHeight-1)
					{
						g4 = pIn[j+(i+1)*nWidth];//g4
						ga += g4;
						c1++;
					}
					
					g = ga/c1;

					//--------------?
					b=0;
					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					unsigned short b3 = 0;//b1
					unsigned short b4 = 0;//b2
					c1=0;
					ga =0;
					
					if(j!=0 && i!=nHeight-1)
					{
						b1 = pIn[(j-1)+(i+1)*nWidth];//b1
						ga += b1;
						c1++;
					}
					
					if(j!=nWidth-1 && i!=nHeight-1)
					{
						b2 = pIn[(j+1)+(i+1)*nWidth];//b2
						ga += b2;
						c1++;
					}
					
					if(j!=0 && i!=0)
					{
						b3 = pIn[(j-1)+(i-1)*nWidth];//b1
						ga += b3;
						c1++;
					}
					
					/*
					if(j!=nWidth-1 && i!=0)
					{
						b4 = pIn[(j+1)+(i-1)*nWidth];//b2
						ga += b4;
						c1++;
					}
					*/
					
					b = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
				else//1,3,5列
				{
					g = pIn[j+i*nWidth];//G

					r=0;
					unsigned short r1 = 0;//R1
					unsigned short r2 = 0;//R2
					c1=0;
					ga =0;
					if(j!=nWidth-1 )
					{
						r1 = pIn[(j+1)+i*nWidth];//R1
						ga += r1;
						c1++;
					}
					if(j!=0)
					{
						r2 = pIn[(j-1)+i*nWidth];//R2
						ga += r2;
						c1++;
					}
					r = ga/c1;

					b=0;
					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					c1=0;
					ga =0;
					if(i!=nHeight-1)
					{
						b1 = pIn[j+(i+1)*nWidth];//b1
						ga += b1;
						c1++;
					}
					if(i!=0)
					{
						b2 = pIn[j+(i-1)*nWidth];//b2
						ga += b2;
						c1++;
					}
					b = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;

					c += 3;

					
				}
			}
			
		}
		else//1,3,5行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)//0,2,4列
				{
					g = pIn[j+i*nWidth];//G

					unsigned short r1 = 0;
					unsigned short r2 = 0;
					c1=0;
					ga =0;
					if(i!=0)
					{
						r1 = pIn[j+(i-1)*nWidth];//r1
						ga += r1;
						c1++;
					}
					if(i!=nHeight-1)
					{
						r2 = pIn[j+(i+1)*nWidth];//r2
						ga += r2;
						c1++;
					}
					r = ga/c1;


					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					c1=0;
					ga =0;
					if(j!=0)
					{
						b1 = pIn[(j-1)+i*nWidth];//b1
						ga += b1;
						c1++;
					}
					
					
					if(j!=nWidth-1 )
					{
						b2 = pIn[(j+1)+i*nWidth];//b2
						ga += b2;
						c1++;
					}
					
					
					b = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
				else//1,3,5列
				{
					b = pIn[j+i*nWidth];//B

					unsigned short g1 = 0;
					unsigned short g2 = 0;
					unsigned short g3 = 0;
					unsigned short g4 = 0;
					c1=0;
					ga =0;
					if(i!=0)
					{
						g1 = pIn[j+(i-1)*nWidth];
						ga += g1;
						c1++;
					}
					if(i!=nHeight-1)
					{
						g2 = pIn[j+(i+1)*nWidth];
						ga += g2;
						c1++;
					}
					/*
					if(j!=nWidth-1)
					{
						g3 = pIn[(j+1)+i*nWidth];
						ga += g3;
						c1++;
					}
					if(j!=0)
					{
						g4 = pIn[(j-1)+i*nWidth];
						ga += g4;
						c1++;
					}
					*/
					g = ga/c1;


					unsigned short r1 = 0;
					unsigned short r2 = 0;
					unsigned short r3 = 0;
					unsigned short r4 = 0;
					c1=0;
					ga =0;
					/*
					if(j!=nWidth-1 && i!=nHeight-1)
					{
						r1 = pIn[(j+1)+(i+1)*nWidth];//R1
						ga += r1;
						c1++;
					}
					*/
					/*
					if(j!=nWidth-1 && i!=0)
					{
						r2 =  pIn[(j+1)+(i-1)*nWidth];;//R2
						ga += r2;
						c1++;
					}
					*/
					if(j!=0 && i!=nHeight-1)
					{
						r3 = pIn[(j-1)+(i+1)*nWidth];//R3
						ga += r3;
						c1++;
					}
					
					if(j!=0 && i!=0)
					{
						r4 =  pIn[(j-1)+(i-1)*nWidth];//R4
						ga += r4;
						c1++;
					}
					r = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = g>255?255:g ;
					c += 3;

					
				}
			}

			
		}
	}

	/*
	LogTool::FormatRGBRowData(pOut,nWidth,0);
	LogTool::FormatRGBRowData(pOut,nWidth,1);
	LogTool::FormatRGBRowData(pOut,nWidth,2);
	LogTool::FormatRGBRowData(pOut,nWidth,3);
	LogTool::FormatRGBRowData(pOut,nWidth,4);
	LogTool::FormatRGBRowData(pOut,nWidth,5);
	LogTool::FormatRGBRowData(pOut,nWidth,6);
	LogTool::FormatRGBRowData(pOut,nWidth,7);
	LogTool::FormatRGBRowData(pOut,nWidth,8);
	LogTool::FormatRGBRowData(pOut,nWidth,9);
	LogTool::FormatRGBRowData(pOut,nWidth,10);
	*/
}

void CameraDataProcess::BayerConversion15(const BYTE* pIn, unsigned char *pOut, int nWidth, int nHeight)
{
	int g = 0;
	int r = 0;
	int b = 0;
	int c =0;
	int c1=0;
	unsigned short ga =0;

	unsigned char * ptrCurRow = NULL;
	

	omp_set_num_threads(4);
#pragma omp parallel for
    for(int i=0;i<nHeight;i++)
	{
		ptrCurRow = pOut + 3 * nWidth*i;

		g = 0;
		r = 0;
		b = 0;

		if(i%2==0)//0,2,4行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)//0,2,4列
				{
					r = pIn[j+i*nWidth];//R

					g=0;
					unsigned short g1 = 0;//g1
					unsigned short g2 =0;//g2
					unsigned short g3 = 0;//g3
					unsigned short g4 = 0;//g4
					c1=0;
					ga =0;

					if(i!=0)
					{
						g3 = pIn[j+(i-1)*nWidth];//g3
						ga += g3;
						c1++;
					}
					if(i!=nHeight-1)
					{
						g4 = pIn[j+(i+1)*nWidth];//g4
						ga += g4;
						c1++;
					}
					
					g = ga/c1;

					//--------------?
					b=0;
					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					unsigned short b3 = 0;//b1
					unsigned short b4 = 0;//b2
					c1=0;
					ga =0;
					
					if(j!=0 && i!=nHeight-1)
					{
						b1 = pIn[(j-1)+(i+1)*nWidth];//b1
						ga += b1;
						c1++;
					}
					
					if(j!=nWidth-1 && i!=nHeight-1)
					{
						b2 = pIn[(j+1)+(i+1)*nWidth];//b2
						ga += b2;
						c1++;
					}
					
					if(j!=0 && i!=0)
					{
						b3 = pIn[(j-1)+(i-1)*nWidth];//b1
						ga += b3;
						c1++;
					}
					
					
					if(j!=nWidth-1 && i!=0)
					{
						b4 = pIn[(j+1)+(i-1)*nWidth];//b2
						ga += b4;
						c1++;
					}
					
					
					b = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
					
				}
				else//1,3,5列
				{
					g = pIn[j+i*nWidth];//G

					r=0;
					unsigned short r1 = 0;//R1
					unsigned short r2 = 0;//R2
					c1=0;
					ga =0;
					if(j!=nWidth-1 )
					{
						r1 = pIn[(j+1)+i*nWidth];//R1
						ga += r1;
						c1++;
					}
					if(j!=0)
					{
						r2 = pIn[(j-1)+i*nWidth];//R2
						ga += r2;
						c1++;
					}
					r = ga/c1;

					b=0;
					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					c1=0;
					ga =0;
					if(i!=nHeight-1)
					{
						b1 = pIn[j+(i+1)*nWidth];//b1
						ga += b1;
						c1++;
					}
					if(i!=0)
					{
						b2 = pIn[j+(i-1)*nWidth];//b2
						ga += b2;
						c1++;
					}
					b = ga/c1;
		
					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;

				}
			}
			
		}
		else//1,3,5行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)//0,2,4列
				{
					g = pIn[j+i*nWidth];//G

					unsigned short r1 = 0;
					unsigned short r2 = 0;
					c1=0;
					ga =0;
					if(i!=0)
					{
						r1 = pIn[j+(i-1)*nWidth];//r1
						ga += r1;
						c1++;
					}
					if(i!=nHeight-1)
					{
						r2 = pIn[j+(i+1)*nWidth];//r2
						ga += r2;
						c1++;
					}
					r = ga/c1;


					unsigned short b1 = 0;//b1
					unsigned short b2 = 0;//b2
					c1=0;
					ga =0;
					if(j!=0)
					{
						b1 = pIn[(j-1)+i*nWidth];//b1
						ga += b1;
						c1++;
					}
					
					
					if(j!=nWidth-1 )
					{
						b2 = pIn[(j+1)+i*nWidth];//b2
						ga += b2;
						c1++;
					}
					
					
					b = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;

				}
				else//1,3,5列
				{
					b = pIn[j+i*nWidth];//B

					unsigned short g1 = 0;
					unsigned short g2 = 0;
					unsigned short g3 = 0;
					unsigned short g4 = 0;
					c1=0;
					ga =0;
					if(i!=0)
					{
						g1 = pIn[j+(i-1)*nWidth];
						ga += g1;
						c1++;
					}
					if(i!=nHeight-1)
					{
						g2 = pIn[j+(i+1)*nWidth];
						ga += g2;
						c1++;
					}
					
					if(j!=nWidth-1)
					{
						g3 = pIn[(j+1)+i*nWidth];
						ga += g3;
						c1++;
					}
					if(j!=0)
					{
						g4 = pIn[(j-1)+i*nWidth];
						ga += g4;
						c1++;
					}
					
					g = ga/c1;


					unsigned short r1 = 0;
					unsigned short r2 = 0;
					unsigned short r3 = 0;
					unsigned short r4 = 0;
					c1=0;
					ga =0;
					/*
					if(j!=nWidth-1 && i!=nHeight-1)
					{
						r1 = pIn[(j+1)+(i+1)*nWidth];//R1
						ga += r1;
						c1++;
					}
					*/
					/*
					if(j!=nWidth-1 && i!=0)
					{
						r2 =  pIn[(j+1)+(i-1)*nWidth];;//R2
						ga += r2;
						c1++;
					}
					*/
					if(j!=0 && i!=nHeight-1)
					{
						r3 = pIn[(j-1)+(i+1)*nWidth];//R3
						ga += r3;
						c1++;
					}
					
					if(j!=0 && i!=0)
					{
						r4 =  pIn[(j-1)+(i-1)*nWidth];//R4
						ga += r4;
						c1++;
					}
					r = ga/c1;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = g>255?255:g ;
					c += 3;

				}
			}
		}

		/*
		*ptrCurRow++ = b>255?255:b;
		*ptrCurRow++ = g>255?255:g;
		*ptrCurRow++ = r>255?255:r;
		*/
	}
}

void CameraDataProcess::BayerConversion101(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight)
{
    int x,y;
    int yWidth=0, ym1Width = 0, yp1Width;
    int pix;

    /*
     * border cases (fill it in with black)
     */
    for (y=0;y<nHeight;y++)
    {
        x=0;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
        x=nWidth-1;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
    }

    for (x=0;x<nWidth;x++)
    {
        y=0;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
        y=nHeight-1;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
    }

    /*
     * main loop
     */
    for (y=1;y<nHeight-1;y++)
    {
        yWidth += nWidth;
        ym1Width = yWidth - nWidth;
        yp1Width = yWidth + nWidth;

        for (x=1;x<nWidth-1;x++)
        {
            pix = ((x+yWidth)<<2);
            if (ODD(y))
                if (ODD(x))
                {   
                    pOut[pix+B_OFFSET] = ((pIn[x-1+  yWidth] + 
                                           pIn[x+1+  yWidth]) >> 3);
                    pOut[pix+G_OFFSET] =   pIn[x  +  yWidth]  >> 2;
                    pOut[pix+R_OFFSET] = ((pIn[x  +ym1Width] + 
                                           pIn[x  +yp1Width]) >> 3);
                }
                else
                {   // ODD(y) EVEN(x)
                    pOut[pix+B_OFFSET] =   pIn[x  +  yWidth]  >> 2;
                    pOut[pix+G_OFFSET] = ((pIn[x-1+  yWidth] + 
                                           pIn[x+1+  yWidth] + 
                                           pIn[x  +ym1Width] + 
                                           pIn[x  +yp1Width]) >> 4);
                    pOut[pix+R_OFFSET] = ((pIn[x-1+ym1Width] +
                                           pIn[x+1+ym1Width] +
                                           pIn[x-1+yp1Width] +
                                           pIn[x+1+yp1Width]) >> 4);
                }
            else
                if (ODD(x))
                {   // EVEN(y) ODD(x)
                    pOut[pix+B_OFFSET] = ((pIn[x-1+ym1Width] +
                                           pIn[x+1+ym1Width] +
                                           pIn[x-1+yp1Width] +
                                           pIn[x+1+yp1Width]) >> 4);
                    pOut[pix+G_OFFSET] = ((pIn[x-1+  yWidth] +
                                           pIn[x+1+  yWidth] +
                                           pIn[x  +ym1Width] +
                                           pIn[x  +yp1Width]) >> 4);
                    pOut[pix+R_OFFSET] =   pIn[x  +  yWidth]  >> 2;
                }
                else
                {   //EVEN(y) EVEN(x)
                    pOut[pix+B_OFFSET] = ((pIn[x  +ym1Width] +
                                           pIn[x  +yp1Width]) >> 3);
                    pOut[pix+G_OFFSET] =   pIn[x  +  yWidth]  >> 2;
                    pOut[pix+R_OFFSET] = ((pIn[x-1+  yWidth] +
                                           pIn[x+1+  yWidth]) >> 3);
                }
        }
    }
}

void CameraDataProcess::BayerConversion13(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight)
{
	int g = 0;
	int r = 0;
	int b = 0;
	int c =0;
    for(int i=0;i<nHeight;i++)
	{
		g = 0;
		r = 0;
		b = 0;
		if(i%2==0)//0,2,4行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)
				{
					g = pIn[j+i*nWidth];//G
					unsigned short r1 = pIn[j+1+i*nWidth];//R1
					unsigned short r2 = r1;//R2
					if(j-1>=0)
					{
						r2 = pIn[j-1+i*nWidth];//R1
					}
					r = (r1+r2)/2;

					unsigned short b1 = pIn[j+(i+1)*nWidth];//b1
					unsigned short b2 = b1;//b2
					if(i-1>=0)
					{
						b2 = pIn[j+(i-1)*nWidth];//b2
					}
					b = (b1+b2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;

					c += 3;
				}
				else
				{
					r = pIn[j+i*nWidth];//R

					unsigned short g1 = pIn[j+1+i*nWidth];//g1
					unsigned short g2 = pIn[j-1+i*nWidth];//g2
					g = (g1+g2)/2;

					unsigned short b1 = pIn[(j-1)+(i+1)*nWidth];//b1
					unsigned short b2 = pIn[(j+1)+(i+1)*nWidth];//b2

					b =  (b1+b2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
			}
		}
		else//1,3,5换行
		{
			for(int j=0;j<nWidth;j++)
			{
				if(j%2==0)
				{
					b = pIn[j+i*nWidth];//B

					unsigned short g1 = pIn[j+(i-1)*nWidth];//g1
					unsigned short g2 = pIn[j+(i+1)*nWidth];//g2
					g = (g1+g2)/2;

					unsigned short r1 = pIn[j+1+(i+1)*nWidth];//R1
					unsigned short r2 =  pIn[j+1+(i-1)*nWidth];//R2

					r = (r1+r2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
				else
				{
					g = pIn[j+i*nWidth];//G

					unsigned short r1 = pIn[j+(i-1)*nWidth];//r1
					unsigned short r2 = pIn[j+(i+1)*nWidth];//r2
					r = (r1+r2)/2;

					unsigned short b1 = pIn[(j-1)+i*nWidth];//b1
					unsigned short b2 = pIn[(j+1)+i*nWidth];//b2
					b =  (b1+b2)/2;

					pOut[c] = r>255?255:r;
					pOut[c+1] = g>255?255:g;
					pOut[c+2] = b>255?255:b;
					c += 3;
				}
			}
		}
	}
}
int CameraDataProcess::ProcessYUVData(byte *pIn,int size)
{
	/*
	if(this->m_Width>1440)
	{
		ProcessData5(pIn,size);
	}
	else
	{
		ProcessData4(pIn,size);
	}
	*/

	SetVideoWidthHeight(m_Width,m_Height);
	//ProcessData4(pIn,size);
	ProcessData8(pIn,size);

	return 0;
}
int CameraDataProcess::ProcessYUVData(byte *pIn, int size, bool isAll)
{
	if(isAll==true)
	{
		SetVideoWidthHeight(m_Width,m_Height);
		ProcessData4(pIn,size);
	}
	else
	{
		if(this->m_Width>1300)
		{
			SetVideoWidthHeight(m_Width/2,m_Height/2);
			//ProcessData8(pIn,size);
			ProcessData10(pIn,size);
		}
		else
		{
			SetVideoWidthHeight(m_Width,m_Height);
			ProcessData4(pIn,size);
		}
	}

	return 0;
}
int CameraDataProcess::ProcessRAWData(byte *pIn,int size)
{
	/*
	if(this->m_Width>1440)
	{
		ProcessData6(pIn,size);
	}
	else
	{
		ProcessData2(pIn,size);
	}
	*/
	SetVideoWidthHeight(m_Width,m_Height);
	//ProcessData2(pIn,size);
	ProcessData9(pIn,size);

	return 0;
}
//RAW 处理
int CameraDataProcess::ProcessData9(byte *pIn,int size)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;
	int imagesize = w * h * 2;
	//LogTool::CreateRAWFile(pIn,size);
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	unsigned short *d = ChangeByteToInt(d1,imagesize);
	unsigned char *rgb24 = new unsigned char[w*h*3];
	memset(rgb24,0,w*h*3);

	BayerConversion10(d, rgb24, w, h);

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,this->m_DisplayWidth,this->m_DisplayHeight));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,this->m_DisplayWidth,this->m_DisplayHeight,0,0,m_Width,m_Height,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}

	delete []d1;
	delete []d;
	delete []rgb24;

	return 1;
}
int CameraDataProcess::ProcessData12(byte *pIn,int size)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;
	int imagesize = w * h * 2;
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	int i1 = size/2/2/2;
	int pIntSize = (m_Width/2)*2 * (m_Height/2)/2;
	unsigned short *pInt = new unsigned short[pIntSize];
	memset(pInt,0x00,pIntSize);
	int c = 0;
	int r = 0;
	int step = 8;
	for(int i=0;i<m_Height;i++)
	{
		//0,1,2,3,4,5,6,7,8,9,10,11,12
		for(int j=0;j<m_Width*2;j+=step)
		{
			for(int z=0;z<step/2;z+=2)
			{
				unsigned char d1 = pIn[i*m_Width*2+j+z+1];
				unsigned char d2 = pIn[i*m_Width*2+j+z+0];
				unsigned short d3 = d1*0x100+d2;

				pInt[c] = d3;
				c++;
			}
		}

		r++;
		
		if(r==6)
		{
			r=0;
			i+=6;
		}
	}

	unsigned char *rgb24 = new unsigned char[m_Width/2*m_Height/2*3];
	memset(rgb24,0,m_Width/2*m_Height/2*3);
	BayerConversion10(pInt, rgb24, m_Width/2, m_Height/2);
	delete [] pInt;

	/*
	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width/2,m_Height/2));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width/2,m_Height/2,0,0,m_Width/2,m_Height/2,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();
	*/
	
	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,this->m_DisplayWidth,this->m_DisplayHeight));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,this->m_DisplayWidth,this->m_DisplayHeight,0,0,m_Width/2,m_Height/2,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();
	

	if(m_IsCapturing==true)
	{

		unsigned char *desrgb24 = GetRAWRgbBuf(pIn,size);
		
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		delete [] desrgb24;

		m_IsCapturing = false;
	}

	delete []d1;
	//delete []pInt;
	delete []rgb24;

	return 1;
}
int CameraDataProcess::ProcessData16(byte *pIn,int size)
{
	int w =	this->m_Width;
	int h = this->m_Height;

	
	int imagesize = this->m_Width *this->m_Height * 2;
	//LogTool::CreateRAWFile(pIn,size);
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	unsigned short *d = ChangeByteToInt(d1,imagesize);
	unsigned char *rgb24 = new unsigned char[this->m_Width*this->m_Height*3];
	memset(rgb24,0,this->m_Width*this->m_Height*3);

	//BayerConversion10(d, rgb24, w, h);
	BayerConversion11(d, rgb24, w, h);
    //LogTool::SaveBmp(m_Width,m_Height, rgb24);

	unsigned char *rgb241 = new unsigned char[w/2*h/2*3];
	memset(rgb241,0,w/2*h/2*3);
	int c=0;
	int w1 = w*3;
	for(int i=0;i<h;i+=2)
	{
		for(int j=0;j<w1;j+=6)
		{
			rgb241[c] = rgb24[j+i*w1];
			rgb241[c+1] = rgb24[j+1+i*w1];
			rgb241[c+2] = rgb24[j+2+i*w1];
			c+=3;
		}
	}
	//LogTool::SaveBmp(m_Width/2,m_Height/2, rgb241);	
	/*
	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,m_Width/2,m_Height/2));
	StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,m_Width/2,m_Height/2,0,0,m_Width/2,m_Height/2,rgb241,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();
	*/
	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,this->m_DisplayWidth,this->m_DisplayHeight));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,this->m_DisplayWidth,this->m_DisplayHeight,0,0,m_Width/2,m_Height/2,rgb241,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();

	if(m_IsCapturing==true)
	{
		unsigned char *desrgb24 = GetRAWRgbBuf(pIn,size);
		
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		delete [] desrgb24;
		
		m_IsCapturing = false;
	}

	delete []d1;
	delete []d;
	delete []rgb24;
	delete []rgb241;

	return 1;
}
int CameraDataProcess::ProcessData17(byte *pIn,int size)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;

	int imagesize = w * h * 2;
	//unsigned char *d1 = new  unsigned char[imagesize];
	//memset(d1,0x00,imagesize);
	//memcpy(d1,pIn,size);

	//int i1 = size/2/2/2;
	int pIntSize = (m_Width/2)*2 * (m_Height/2)/2;
	unsigned short *pInt = new unsigned short[pIntSize];
	memset(pInt,0x00,pIntSize);
	int c = 0;
	int r = 0;
	int step = 8;
	for(int i=0;i<m_Height;i++)
	{
		//0,1,2,3,4,5,6,7,8,9,10,11,12
		for(int j=0;j<m_Width*2;j+=step)
		{
			for(int z=0;z<step/2;z+=2)
			{
				unsigned char d1 = pIn[i*m_Width*2+j+z+1];
				unsigned char d2 = pIn[i*m_Width*2+j+z+0];
				unsigned short d3 = d1*0x100+d2;

				pInt[c] = d3;
				c++;
			}
		}

		r++;
		
		if(r==2)
		{
			r=0;
			i+=2;
		}
	}

	unsigned char *rgb24 = new unsigned char[m_Width/2*m_Height/2*3];
	memset(rgb24,0,m_Width/2*m_Height/2*3);
	BayerConversion11(pInt, rgb24, m_Width/2, m_Height/2);
	delete [] pInt;
	

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,this->m_DisplayWidth,this->m_DisplayHeight));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,COLORONCOLOR);//HALFTONE STRETCH_HALFTONE
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,this->m_DisplayWidth,this->m_DisplayHeight,0,0,m_Width/2,m_Height/2,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();
	
	//LogTool::SaveBmp(m_Width/2,m_Height/2, rgb24);

	if(m_IsCapturing==true)
	{

		unsigned char *desrgb24 = GetRAWRgbBuf(pIn,size);
		
		CString filename = LogTool::SaveBmp(m_Width,m_Height, desrgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		delete [] desrgb24;

		m_IsCapturing = false;
	}

	//delete []d1;
	delete []rgb24;

	return 1;
}
int CameraDataProcess::ProcessData18(byte *pIn, int size/*, bool isLeft*/)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;
	unsigned char *rgb24 = new unsigned char[m_Width * m_Height*3];
	memset(rgb24,0,m_Width*m_Height*3);
	BayerConversion15(pIn, rgb24, m_Width, m_Height);

	static bool k = true;
	if (k)
	{
		m_pDisplay->Open(CDC::FromHandle(m_hDC), CRect(0, 0, this->m_DisplayWidth, this->m_DisplayHeight));
		k = false;
	}

	int displayWidth = this->m_DisplayWidth;
	int xStart = 0;

	int picStartX = 0;
	int picStartY = 0;
	int picWidth = m_Width;
	int picHeight = m_Height;
	//if (isLeft)
	//{
		switch (m_MirrorStatus)
		{
		case MirrorX:
			picStartX = m_Width;
			picWidth = 0 - m_Width;
			break;
		case MirrorY:
			picStartY = m_Height;
			picHeight = 0 - m_Height;
			break;
		case MirrorXY:
			picStartX = m_Width;
			picWidth = 0 - m_Width;
			picStartY = m_Height;
			picHeight = 0 - m_Height;
			break;
		default:
			break;
		}
	//}
	//else
	//{
	//	xStart = displayWidth+5;
	//}
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);//HALFTONE STRETCH_HALFTONE
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC, xStart, 0, displayWidth, m_DisplayHeight, picStartX, picStartY, picWidth, picHeight, rgb24, &m_BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	//::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC, sp, 0, wp2, this->m_DisplayHeight, 0, 0, m_Width, m_Height, pIn, &m_BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	//if (!isLeft)
	//{
		m_pDisplay->Display();
	//}
	
	//LogTool::SaveBmp(m_Width/2,m_Height/2, rgb24);

	if(m_IsCapturing==true)
	{
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		m_IsCapturing = false;
	}

	delete []rgb24;

	return 1;
}
int CameraDataProcess::ProcessData19(byte *pIn,int size)
{
	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;

	/*
	int imagesize = w * h;

	unsigned short *pInt = new unsigned short[imagesize];
	memset(pInt,0x00,imagesize);
	for(int j=0;j<size;j++)
	{
		unsigned char d2 = pIn[j];
		unsigned short d3 = 0x00+d2;

		pInt[j] = d3;

	}
	*/

	unsigned char *rgb24 = new unsigned char[m_Width * m_Height*3];
	memset(rgb24,0,m_Width*m_Height*3);
	//BayerConversion14(pInt, rgb24, m_Width, m_Height);
	RawDataProcess(pIn, rgb24, m_Width, m_Height);
	
	//delete [] pInt;
	

	m_pDisplay->Open(CDC::FromHandle(m_hDC),CRect(0,0,this->m_DisplayWidth,this->m_DisplayHeight));
	SetStretchBltMode(m_pDisplay->GetMemDC()->m_hDC,STRETCH_HALFTONE);//HALFTONE STRETCH_HALFTONE
	::StretchDIBits(m_pDisplay->GetMemDC()->m_hDC,0,0,this->m_DisplayWidth,this->m_DisplayHeight,0,0,m_Width,m_Height,rgb24,&m_BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
	m_pDisplay->Display();
	
	//LogTool::SaveBmp(m_Width/2,m_Height/2, rgb24);

	if(m_IsCapturing==true)
	{

		//unsigned char *desrgb24 = GetRAWRgbBuf2(pIn,size);
		
		CString filename = LogTool::SaveBmp(m_Width,m_Height, rgb24);
		switch(m_PictureFormat)
		{
			case 0:break;
			case 1:
				LogTool::SaveBmpToJpg(filename,0);
				break;
		}
		
		//delete [] desrgb24;

		m_IsCapturing = false;
	}

	delete []rgb24;

	return 1;
}
void CameraDataProcess::BayerConversion102(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight)
{
    int x,y;
    int yWidth=0, ym1Width = 0, yp1Width;
    int pix;

    /*
     * border cases (fill it in with black)
     */
    for (y=0;y<nHeight;y++)
    {
        x=0;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
        x=nWidth-1;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
    }

    for (x=0;x<nWidth;x++)
    {
        y=0;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
        y=nHeight-1;
        pOut[((x+y*nWidth)<<2)+B_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+G_OFFSET] = 0;
        pOut[((x+y*nWidth)<<2)+R_OFFSET] = 0;
    }

    /*
     * main loop
     */
    for (y=1;y<nHeight-1;y++)
    {
        yWidth += nWidth;
        ym1Width = yWidth - nWidth;
        yp1Width = yWidth + nWidth;

        for (x=1;x<nWidth-1;x++)
        {
            pix = ((x+yWidth)<<2);
            if (ODD(y))
                if (ODD(x))
                {   
                    pOut[pix+B_OFFSET] = ((pIn[x-1+  yWidth] + 
                                           pIn[x+1+  yWidth]) >> 1);
                    pOut[pix+G_OFFSET] =   pIn[x  +  yWidth];
                    pOut[pix+R_OFFSET] = ((pIn[x  +ym1Width] + 
                                           pIn[x  +yp1Width]) >> 1);
                }
                else
                {   // ODD(y) EVEN(x)
                    pOut[pix+B_OFFSET] =   pIn[x  +  yWidth];
                    pOut[pix+G_OFFSET] = ((pIn[x-1+  yWidth] + 
                                           pIn[x+1+  yWidth] + 
                                           pIn[x  +ym1Width] + 
                                           pIn[x  +yp1Width]) >> 2);
                    pOut[pix+R_OFFSET] = ((pIn[x-1+ym1Width] +
                                           pIn[x+1+ym1Width] +
                                           pIn[x-1+yp1Width] +
                                           pIn[x+1+yp1Width]) >> 2);
                }
            else
                if (ODD(x))
                {   // EVEN(y) ODD(x)
                    pOut[pix+B_OFFSET] = ((pIn[x-1+ym1Width] +
                                           pIn[x+1+ym1Width] +
                                           pIn[x-1+yp1Width] +
                                           pIn[x+1+yp1Width]) >> 2);
                    pOut[pix+G_OFFSET] = ((pIn[x-1+  yWidth] +
                                           pIn[x+1+  yWidth] +
                                           pIn[x  +ym1Width] +
                                           pIn[x  +yp1Width]) >> 2);
                    pOut[pix+R_OFFSET] =   pIn[x  +  yWidth];
                }
                else
                {   //EVEN(y) EVEN(x)
                    pOut[pix+B_OFFSET] = ((pIn[x  +ym1Width] +
                                           pIn[x  +yp1Width]) >> 1);
                    pOut[pix+G_OFFSET] =   pIn[x  +  yWidth];
                    pOut[pix+R_OFFSET] = ((pIn[x-1+  yWidth] +
                                           pIn[x+1+  yWidth]) >> 1);
                }
        }
    }
}
int CameraDataProcess::ProcessData11(byte *pIn,int size)
{
	/*
	Preview Width   = 3264
	Preview Height  = 2448
	*/
	m_Width = 3264;
	m_Height = 2448;

	//2M w 1640 h 1224
	/*
	m_Width = 1640;
	m_Height = 1224;
	*/
	//3M w 2048 h 1536
	/*
	m_Width = 2048;
	m_Height = 1536;
	*/

	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;
	int imagesize = w * 2 * h;
	//LogTool::CreateRAWFile(pIn,size);
	//unsigned char *d1 = new  unsigned char[imagesize];
	//memset(d1,0x00,imagesize);
	//memcpy(d1,pIn,size);

	/*
	unsigned short *pInt = new unsigned short[size/2];
	memset(pInt,0x00,(size/2));
	int c = 0;
	for(int i=0;i<m_Height;i++)
	{
		for(int j=0;j<m_Width*2;j+=2)
		{
			unsigned char d1 = pIn[i*m_Width*2+j+1];
			unsigned char d2 = pIn[i*m_Width*2+j];
			unsigned short d3 = d1*0x100+d2;

			pInt[c] = d3;
			c++;
		}
	}
	*/
	int ipIntSize = m_Width/2 *2 * m_Height/2;
	unsigned short *pInt = new unsigned short[ipIntSize];
	memset(pInt,0x00,(ipIntSize));
	int c = 0;
	int r = 0;
	//int hi = 0;
	int step = 8;
	for(int i=0;i<m_Height;i++)
	{
		//0,1,2,3,4,5,6,7,8,9,10,11,12
		for(int j=0;j<m_Width*2;j+=step)
		{
			//hi++;
			for(int z=0;z<step/2;z+=2)
			{
				unsigned char d1 = pIn[i*m_Width*2+j+z+1];
				unsigned char d2 = pIn[i*m_Width*2+j+z+0];
				unsigned short d3 = d1*0x100+d2;

				if(c<ipIntSize)
				{
					pInt[c] = d3;
					c++;
				}
				else
				{
					int z=1;
				}
			}
		}

		r++;
		
		if(r==2)
		{
			r=0;
			i+=2;
		}
	}
	
	//unsigned short *d = ChangeByteToInt(d1,imagesize);

	unsigned char *rgb24 = new unsigned char[m_Width/2*m_Height/2*3];
	memset(rgb24,0,m_Width/2*m_Height/2*3);
	BayerConversion11(pInt, rgb24, w/2, h/2);
	delete [] pInt;

	LogTool::SaveBmp(m_Width/2,m_Height/2, rgb24);

	delete [] rgb24;
	

	return 1;
}
int CameraDataProcess::ProcessData13(byte *pIn,int size)
{
	/*
	Preview Width   = 3264
	Preview Height  = 2448
	*/
	m_Width = 3264;
	m_Height = 2448;

	//2M w 1640 h 1224
	/*
	m_Width = 1640;
	m_Height = 1224;
	*/
	//3M w 2048 h 1536
	/*
	m_Width = 2048;
	m_Height = 1536;
	*/

	int w = 0;
	int h = 0;

	w = m_Width;
	h = m_Height;
	int imagesize = w * h * 2;
	//LogTool::CreateRAWFile(pIn,size);
	unsigned char *d1 = new  unsigned char[imagesize];
	memset(d1,0x00,imagesize);
	memcpy(d1,pIn,size);

	
	unsigned short *pInt = new unsigned short[size/2];
	memset(pInt,0x00,(size/2));
	int c = 0;
	for(int i=0;i<m_Height;i++)
	{
		for(int j=0;j<m_Width*2;j+=2)
		{
			unsigned char d1 = pIn[i*m_Width*2+j+1];
			unsigned char d2 = pIn[i*m_Width*2+j];
			unsigned short d3 = d1*0x100+d2;

			pInt[c] = d3;
			c++;
		}
	}
	
	
	unsigned char *rgb24 = new unsigned char[m_Width*m_Height*3];
	memset(rgb24,0,m_Width*m_Height*3);
	BayerConversion11(pInt, rgb24, w, h);
	delete [] pInt;

	LogTool::SaveBmp(m_Width,m_Height, rgb24);

	delete [] rgb24;
	

	return 1;
}
int CameraDataProcess::ProcessRAWData(byte *pIn, int size, bool isAll/*, bool isLeft*/)
{
  	char lpath[256] = "D:\\left.raw";
//   	if (!isLeft)
//   	{
//   		strcpy(lpath, "D:\\right.raw");
//   	}
  	FILE *pFile = nullptr;
  	fopen_s(&pFile, lpath, "wb");
  	fwrite(pIn, size, 1, pFile);
  	fclose(pFile);
	if(GetBitmapInfoWidth()!=m_Width)
	{
		SetVideoWidthHeight(m_Width,m_Height);
	}

	ProcessData18(pIn, size/*, isLeft*/);

	return 0;
}
//最临近插值算法
bool CameraDataProcess::BmpZoom(unsigned char *srcrgb24,int srcwidth,int srcheight ,int deswidth,int desheight,unsigned char *desrgb24)
{
	int i,j;

	int biBitCount = 24;

	int lineSize = biBitCount * srcwidth / 8;

	//偏移量，windows系统要求每个扫描行按四字节对齐
	int alignBytes = ((srcwidth * biBitCount + 31) & ~31) / 8L - srcwidth * biBitCount / 8L;

	//目标图像缓存
	int desBufSize = ((deswidth * biBitCount + 31) / 32) * 4 * desheight;
	int desLineSize = ((deswidth * biBitCount + 31) / 32) * 4;  
	BYTE *desBuf = new BYTE[desBufSize];
	double rateH = (double)srcheight / desheight;
	double rateW = (double)srcwidth / deswidth;
	for (i = 0; i < desheight; i++)
	{
		//选取最邻近的点
		int tSrcH = (int)(rateH * i + 0.5);
		for (j = 0; j < deswidth; j++)
		{
			int tSrcW = (int)(rateW * j + 0.5);    
			memcpy(&desBuf[i * desLineSize] + j * biBitCount / 8,&srcrgb24[tSrcH * lineSize] + tSrcW * biBitCount / 8,biBitCount / 8);   
		}
	}

	desrgb24 = desBuf;

	return true;
}
void CameraDataProcess::RawDataProcess(const BYTE* pInData, BYTE* pOutData, int nHeight, int nWidth)
 {
	int yRawWidth = 0, ym1RawWidth = 0, yp1RawWidth;
	
 	int nRawWidth = nWidth;
 	memset(pOutData, 0, sizeof(byte)*nWidth*nHeight * 3);
 	byte *ptrCurRow(NULL);
 
 	//unsigned short* pIn = (unsigned short*)pInData;

	byte * pIn = (byte *)pInData;
 	/*
 	* main loop
 	*/
 	omp_set_num_threads(3);
 #pragma omp parallel for
 	for (int y = 1; y<nHeight - 1; y++)
 	{
		unsigned short r, g, b;
 		//yWidth += nWidth;
 		yRawWidth = nRawWidth*y;				// 生数据中的当前行位置
 		ym1RawWidth = yRawWidth - nRawWidth;	// 生数据中的前一行位置
 		yp1RawWidth = yRawWidth + nRawWidth;	// 生数据中的后一行位置
 
 		ptrCurRow = pOutData + 3 * nWidth*y;		// 彩色图像中的当前行位置
 
 
 		for (int x = 1; x<nWidth - 1; x++)
 		{
 			//pix = ((x + yWidth) << 2);        // 像素号
 			if (ODD(y))//1,3,5,7,9行
			{
 				
				if (ODD(x + 1))//1,3,5,7,9列
 				{
					
 					r = (pIn[x + yRawWidth + 1] + pIn[x + yRawWidth - 1]);
 					g = pIn[x + yRawWidth];
 					b = (pIn[x + ym1RawWidth] + pIn[x + yp1RawWidth]);
				}
 				else
 				{
 					r = (pIn[x + yRawWidth]);
					g = (pIn[x + ym1RawWidth] + pIn[x + yp1RawWidth] + pIn[x + yRawWidth + 1] + pIn[x + yRawWidth - 1]);
					b = (pIn[x + ym1RawWidth - 1] + pIn[x + ym1RawWidth + 1] + pIn[x + yp1RawWidth - 1] + pIn[x + yp1RawWidth + 1]);
 				
 				}
			}
 			else//2,4,6,8
			{
 				if (ODD(x + 1))////1,3,5,7,9列
 				{
 					r = (pIn[x + ym1RawWidth - 1] + pIn[x + ym1RawWidth + 1] + pIn[x + yp1RawWidth - 1] + pIn[x + yp1RawWidth + 1]);
 					g = (pIn[x + ym1RawWidth] + pIn[x + yp1RawWidth] + pIn[x + yRawWidth + 1] + pIn[x + yRawWidth - 1]);
 					b = pIn[x + yRawWidth];
 				}
 				else
 				{
					r = (pIn[x + yRawWidth-1] + pIn[x + yRawWidth+1]);
 					g = pIn[x + yRawWidth];
 					b = (pIn[x + yRawWidth + 1] + pIn[x + yRawWidth - 1]);
 				}
			}
			//*ptrCurRow++ = r>255?255:r;
			*ptrCurRow++ = b>255?255:b;
			*ptrCurRow++ = g>255?255:g;
			//*ptrCurRow++ = b>255?255:b;
			*ptrCurRow++ = r>255?255:r;
			//ptrCurRow[3 * x + R_OFFSET] = r>255?255:r;
			//ptrCurRow[3 * x + G_OFFSET] = g>255?255:g;
			//ptrCurRow[3 * x + B_OFFSET] = b>255?255:b;
 		}
 	}
}

