#pragma once
class LogTool
{
public:
	LogTool(void);
	~LogTool(void);

	static CString GetModulePath(HMODULE hModule  = NULL);
	static void Writelog(CString info,CString logfile);
	static CString FormatLog(UCHAR *buf, int len);
	static void WriteTimelog(CString info,CString logfile);
	static CString LogTool::FormatLog(UCHAR *buf, long len,long *rowindex);
	static CString FormatLog1(UCHAR *buf, long len,long *rowindex);
	static void CreateYuvFile(unsigned char *pIn,long size);
	static void CreateRAWFile(unsigned char *pIn,long size);
	static void FormatYuvFileData(unsigned char *d, int width,int height);
	static void FormatYuvFileSorceData(unsigned char *d, int width,int height);
	static void FormatRGBFileData(unsigned char *d, int width,int height);
	static bool SaveDIB2Bmp(int iWidth, int iHeight, BYTE *pBuffer);
	static void ContructBhh(int nWidth,int nHeight,BITMAPFILEHEADER& bhh);
	static void ConstructBih(int nWidth,int nHeight,BITMAPINFOHEADER& bih) ;
	static void FormatRGBFileRowData(unsigned char *d,int rowid);
	static void FormatYuvFileRowData(unsigned char *d,int rowid);
	static void FormatYuvFileSourceRowData(unsigned char *d,int rowid);
	static void SaveSourceRowData(unsigned char *source,int rowid);
	static void WriteRowDatalog(CString info,CString filename);
	static void FormatYuvFileSourceRowData(CString tag,unsigned char *d,int width,int rowid);
	static void FormatRGBFileRowData(CString tag,unsigned char *d,int width,int rowid);
	static void FormatRAWRowData(unsigned short *d,int nWidth,int rowcount);
	static CString FormatLog(unsigned short *buf, long len,long *rowindex);
	static void CreateDataFile(unsigned short *pIn,long size);
	static void CreateDataFile(unsigned char *pIn,long size);
	static CString SaveBmp(int iWidth, int iHeight, BYTE *pBuffer);
	static CString SaveBmpToJpg(CString fileBmp,int mark=0);
	static void FormatRGBRowData(unsigned char *d,int nwidth,int rowid);
};

