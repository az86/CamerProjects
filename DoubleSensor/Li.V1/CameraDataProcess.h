#pragma once
#include "Display.h"
#include "USB3Camera.h"
#include <omp.h>
#define	Array2_Index(i,j)	  i*m_Width+j
#include "MirrorStatus.h"

struct tagRGB
{
	byte B;
	byte G;
	byte R;
};
enum RgbChangeType//R G B三元素互换处理
{
	Normal_Change,RG_Change,RB_Change,GB_Change
};
typedef struct {
	DWORD dwBout;
	DWORD dwGout;
	DWORD dwRout;
} RGBSumData;
typedef enum { 
	B,				// 定义Raw数据中Blue Data
	Gb,				// 定义Raw数据中Green Data
	Gr,				// 定义Raw数据中Green Data
	R				// 定义Raw数据中Red Data
} ColorType;
class CameraDataProcess
{
public:
	CameraDataProcess();
	CameraDataProcess(int w,int h);
	~CameraDataProcess(void);

public:
	int Open(HDC hDC);
	int Close();
	int ProcessData(byte *pIn);

	void SetWidthHeight(int w,int h);
	void SetVideoWidthHeight(int w,int h);
	void SetDisplayWidthHeight(int w,int h);

	void yuv422_formatchange(unsigned char *yuv422, long size) ;
	void raw_to_rgb24(unsigned char *pIn,unsigned char *pRgb24, int width,int height);
	unsigned char * MoveData(unsigned char *pIn, long size);
	unsigned char * MoveData1(unsigned char *pIn, long size);
	unsigned char * MoveData2(unsigned char *pIn, long size);
	int Convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
	int Convert_yuv_to_rgb_pixel(int y, int u, int v);
	BufData ReadFile(CString filePath);
	unsigned char * RGBReCreate(unsigned char *rgb, int rowid, unsigned int width, unsigned int height);
	unsigned char * MoveData3(unsigned char *pIn, long size,int rate);
	int ProcessData1(byte *pIn,int size);
	int ProcessData2(byte *pIn,int size);
	int ProcessData3(byte *pIn,int size);
	int ProcessData4(byte *pIn,int size);
	int ProcessData5(byte *pIn,int size);
	int ProcessData6(byte *pIn,int size);
	int ProcessData7(byte *pIn,int size);
	int ProcessData8(byte *pIn,int size);
	int ProcessData9(byte *pIn,int size);
	int ProcessData10(byte *pIn,int size);
	int ProcessData11(byte *pIn,int size);
	int ProcessData12(byte *pIn,int size);
	int ProcessData16(byte *pIn,int size);
	int ProcessData17(byte *pIn,int size);
	int ProcessData18(byte *pIn, int size/*, bool isLeft*/);
	int ProcessData19(byte *pIn,int size);

	int ProcessData13(byte *pIn,int size);

	int ProcessYUVData(byte *pIn,int size);
	int ProcessRAWData(byte *pIn,int size);

	int ProcessRAWData(byte *pIn, int size, bool isAll/*, bool isLeft*/);
	int ProcessYUVData(byte *pIn,int size,bool isAll);

	unsigned char * GetYUVRgbBuf(byte *pIn,int size);
	unsigned char * GetRAWRgbBuf(byte *pIn,int size);

	void BayerConversion11(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight);
	void BayerConversion13(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight);
	void BayerConversion14(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight);
	void BayerConversion101(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight);
	void BayerConversion102(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight);

	unsigned char * GetRAWRgbBuf2(byte *pIn,int size);

	int GetBitmapInfoWidth();

	//bool BmpZoom(const char* srcBmp, const char* dstBmp, double rate);
	bool BmpZoom(unsigned char *srcrgb24,int srcwidth,int srcheight,int deswidth,int desheight,unsigned char *desrgb24);

	void RawDataProcess(const BYTE* pInData, BYTE* pOutData, int nHeight, int nWidth);
	void BayerConversion15(const BYTE* pIn, unsigned char *pOut, int nWidth, int nHeight);
	

	int m_PictureFormat;

	BOOL Bitmap8To24(BYTE* srcImage,BYTE* dstImage,LONG imageWidth,LONG imageHeight);
	BOOL Bitmap8To242(BYTE* srcImage,BYTE* dstImage,LONG imageWidth,LONG imageHeight);
	void AjustValue(RGBSumData& SumData);
	void RToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData);
	void GrToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData);
	void GbToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData);
	void BToRGB( const UINT iY, const UINT iX, const BYTE *pImage, RGBSumData& SumData);

	int W;//
	
	bool m_IsCapturing;
	MirrorStatus		m_MirrorStatus;		//AZ add for Mirror status

private:
	int ByteToRGB(byte *pIn ,tagRGB* pOut);
	void RgbChangeProc( tagRGB& DestRgb,const tagRGB& OrgRgb );
	void yuv422_to_rgb24(unsigned char *yuv422,unsigned char *rgb24, int width,int height) ;
	void liyuv422_to_rgb_buffer(unsigned char *data, unsigned char *rgb, unsigned int width, unsigned int height);
	unsigned char * ProcessYUVData(unsigned char *pIn);
	void Convert_yuv_to_rgb_buffer(int *data, unsigned char *rgb, unsigned int width, unsigned int height);
	BOOL Bayer10ToBitmap24(BYTE* src,LONG imageWidth,LONG imageHeight,BYTE* bitmap);
	void Bayer2rgb24(unsigned char *dst, unsigned short *src, long int WIDTH, long int HEIGHT);
	unsigned short * ChangeByteToInt(unsigned char *pIn, long size);
	void BayerConversion10(unsigned short *pIn, unsigned char *pOut, int nWidth, int nHeight);

	BITMAPINFO	m_BitmapInfo;
	HDC			m_hDC;
	CDisplay*   m_pDisplay;

	//图像内存尺寸
	int         m_Width;
	int         m_Height;

	volatile int         m_DisplayWidth;
	volatile int         m_DisplayHeight;

	RgbChangeType m_ChangeType;

	tagRGB		*m_RgbData;
};

