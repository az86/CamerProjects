#include "StdAfx.h"
#include "LogTool.h"
#include "SetInfoHelper.h"
#include<atlimage.h>


LogTool::LogTool(void)
{
}


LogTool::~LogTool(void)
{
}

CString LogTool::GetModulePath(HMODULE hModule)
{
	TCHAR buf[MAX_PATH] = {'\0'};
	CString strDir, strTemp;

	::GetModuleFileName( hModule, buf, MAX_PATH);
	strTemp = buf;
	strDir = strTemp.Left( strTemp.ReverseFind('\\') + 1 );

	return strDir;
} 
void LogTool::CreateYuvFile(unsigned char *pIn,long size)
 {
	CFile*  pFile;

	CString filePath;
	CString tempstr="";
	
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	tempstr.Format("%4d%02d%02d%02d%02d%02d%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	
	filePath = GetModulePath() + "\\LOG\\"+tempstr+".yuv";

	pFile=new CFile;
	if(!pFile->Open(filePath,CFile::modeCreate|CFile::modeWrite))
	{
		return ;
	}
	pFile->Write(pIn,size);
	pFile->Close();

	delete pFile;

 } 
void LogTool::CreateDataFile(unsigned short *pIn,long size)
 {
	CFile*  pFile;

	CString filePath;
	CString tempstr="";
	
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	tempstr.Format("%4d%02d%02d%02d%02d%02d%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	
	filePath = GetModulePath() + "\\LOG\\"+tempstr+".data";

	pFile=new CFile;
	if(!pFile->Open(filePath,CFile::modeCreate|CFile::modeWrite))
	{
		return ;
	}
	pFile->Write(pIn,size);
	pFile->Close();

	delete pFile;

 } 
void LogTool::CreateDataFile(unsigned char *pIn,long size)
 {
	CFile*  pFile;

	CString filePath;
	CString tempstr="";
	
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	tempstr.Format("%4d%02d%02d%02d%02d%02d%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	
	filePath = GetModulePath() + "\\LOG\\"+tempstr+".data";

	pFile=new CFile;
	if(!pFile->Open(filePath,CFile::modeCreate|CFile::modeWrite))
	{
		return ;
	}
	pFile->Write(pIn,size);
	pFile->Close();

	delete pFile;

 } 
void LogTool::CreateRAWFile(unsigned char *pIn,long size)
 {
	CFile*  pFile;

	CString filePath;
	CString tempstr="";
	
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	tempstr.Format("%4d%02d%02d%02d%02d%02d%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	
	filePath = GetModulePath() + "\\LOG\\"+tempstr+".raw";

	pFile=new CFile;
	if(!pFile->Open(filePath,CFile::modeCreate|CFile::modeWrite))
	{
		return ;
	}
	pFile->Write(pIn,size);
	pFile->Close();

	delete pFile;

 } 
//iWidth：图像宽； iHeight：图像高；pBuffer：图像RGB数据；filePath：存储路径；fileName：保存文件名；fileNum：保存文件编号
//SaveDIB2Bmp(100, "frame", "D://screenshot")
bool LogTool::SaveDIB2Bmp(int iWidth, int iHeight, BYTE *pBuffer)
{
	BITMAPINFOHEADER bih;
	ConstructBih(iWidth,iHeight,bih);
	BITMAPFILEHEADER bhh;
	ContructBhh(iWidth,iHeight,bhh);
 
    CString tempstr,filePath;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	tempstr.Format("%4d%02d%02d%02d%02d%02d%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	
	filePath = GetModulePath() + "\\LOG\\"+tempstr+".bmp";

	 int widthStep = (((iWidth * 24) + 31) & (~31)) / 8 ; //每行实际占用的大小（每行都被填充到一个4字节边界）
	 int DIBSize = widthStep * iHeight ;  //buffer的大小 （字节为单位）


	 CFile file;
	 try
	 {
		  if(file.Open(filePath,CFile::modeWrite | CFile::modeCreate))
		  {
  
			   file.Write((LPSTR)&bhh,sizeof(BITMAPFILEHEADER));
			   file.Write((LPSTR)&bih,sizeof(BITMAPINFOHEADER));
			   file.Write(pBuffer,DIBSize);
			   file.Close();
			   return true;
		}

	 }
	 catch (...) 
	 {
		//AfxMessageBox("MyDC::SaveDIB2Bmp");
	 }
	 return false;
}
CString LogTool::SaveBmp(int iWidth, int iHeight, BYTE *pBuffer)
{
	BITMAPINFOHEADER bih;
	ConstructBih(iWidth,iHeight,bih);
	BITMAPFILEHEADER bhh;
	ContructBhh(iWidth,iHeight,bhh);
 
    CString tempstr,filePath;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	tempstr.Format("%4d%02d%02d%02d%02d%02d%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);

	SetInfoHelper helper;
	CString filePath1;
	filePath1.Format("%s",helper.GetPicturePath());
	filePath = filePath1 + "\\"+tempstr+".bmp";

	 int widthStep = (((iWidth * 24) + 31) & (~31)) / 8 ; //每行实际占用的大小（每行都被填充到一个4字节边界）
	 int DIBSize = widthStep * iHeight ;  //buffer的大小 （字节为单位）


	 CFile file;
	 try
	 {
		 if (file.Open(filePath, CFile::modeWrite | CFile::modeCreate))
		  {
  
			   file.Write((LPSTR)&bhh,sizeof(BITMAPFILEHEADER));
			   file.Write((LPSTR)&bih,sizeof(BITMAPINFOHEADER));
			   file.Write(pBuffer,DIBSize);
			   file.Close();
			   return filePath;
		}

	 }
	 catch (...) 
	 {
		//AfxMessageBox("MyDC::SaveDIB2Bmp");
	 }

	 return NULL;
}
CString LogTool::SaveBmpToJpg(CString fileBmp,int mark)
{
	if(fileBmp.IsEmpty()) 
	{
		return NULL;
	}

	CImage im;    
	im.Load(fileBmp);  
	CString fnJpg; 
	fnJpg = fileBmp.Left(fileBmp.GetLength()-4);
	fnJpg = fnJpg + ".jpg";
	im.Save(fnJpg,Gdiplus::ImageFormatJPEG);        
	if(mark==0)
	{
		::DeleteFile(fileBmp);     
	}

	return fnJpg;
}
//构建BMP位图文件头   
void LogTool::ContructBhh(int nWidth,int nHeight,BITMAPFILEHEADER& bhh) //add 2010-9-04   
{  
	int widthStep = (((nWidth * 24) + 31) & (~31)) / 8 ; //每行实际占用的大小（每行都被填充到一个4字节边界）   
	bhh.bfType = ((WORD) ('M' << 8) | 'B');  //'BM'   
	bhh.bfSize = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + widthStep * nHeight;  
	bhh.bfReserved1 = 0;  
	bhh.bfReserved2 = 0;  
	bhh.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);  
}  
//构建BMP文件信息头   
void LogTool::ConstructBih(int nWidth,int nHeight,BITMAPINFOHEADER& bih)   
{  
	int widthStep = (((nWidth * 24) + 31) & (~31)) / 8 ;   
  
	bih.biSize=40;       // header size   
	bih.biWidth=nWidth;  
	bih.biHeight=nHeight;  
	bih.biPlanes=1;  
	bih.biBitCount=24;     // RGB encoded, 24 bit   
	bih.biCompression=BI_RGB;   // no compression 非压缩   
	bih.biSizeImage=widthStep*nHeight*3;  
	bih.biXPelsPerMeter=0;  
	bih.biYPelsPerMeter=0;  
	bih.biClrUsed=0;  
	bih.biClrImportant=0;   
 }  

void LogTool::WriteTimelog(CString info,CString logfile)
{
	CStdioFile sf;
	CString LogFilePath;
	CString tempstr="";
	
	CTime time0=CTime::GetCurrentTime();
    tempstr=time0.Format("%Y_%m_%d");
	tempstr = logfile +tempstr + ".data";

	LogFilePath = GetModulePath() + "\\LOG\\"+tempstr;

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	tempstr.Format("%4d-%02d-%02d %02d:%02d:%02d %03d:",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
				
	tempstr = tempstr + info;
	
	if (sf.Open(LogFilePath, CFile::modeWrite))
	{ 
		sf.SeekToEnd();
	    sf.WriteString(tempstr);
		sf.WriteString("\t\n");
		sf.Close();
	}
	else
	{
		if (sf.Open(LogFilePath, CFile::modeCreate|CFile::modeWrite))
		{ 
			sf.WriteString(tempstr);
		    sf.WriteString("\t\n");
			sf.Close();
		}
	}
}
void LogTool::Writelog(CString info,CString logfile)
{
	CStdioFile sf;
	CString LogFilePath;
	CString tempstr="";
	
	CTime time0=CTime::GetCurrentTime();
    tempstr=time0.Format("%Y_%m_%d");
	tempstr = logfile +tempstr + ".data";

	LogFilePath = GetModulePath() + "\\LOG\\"+tempstr;

	tempstr = info;
	
	if (sf.Open(LogFilePath, CFile::modeWrite))
	{ 
		sf.SeekToEnd();
	    sf.WriteString(tempstr);
		sf.Close();
	}
	else
	{
		if (sf.Open(LogFilePath, CFile::modeCreate|CFile::modeWrite))
		{ 
			sf.WriteString(tempstr);
			sf.Close();
		}
	}
}
CString LogTool::FormatLog(UCHAR *buf, int len)
{
	CString logstr;
	CString str1;
	CString str2;
	CString str3;
	int index =0;
	for(int i=0;i<len;i=i+32)
	{
		str3 = "";
		for(int j=0;j<32;j++)
		{
			if((j+i)<len)
			{
				str1.Format("%02X ",buf[j+i]);
				str3 = str3 + str1;
			}
			else
			{
				break;
			}
		}
		str2.Format("%d:",index);
		index++;

		str3 = str2 + str3+"\n";

		logstr += str3;
	}

	return logstr;
}
CString LogTool::FormatLog(UCHAR *buf, long len,long *rowindex)
{
	CString logstr;
	CString str1;
	CString str2;
	CString str3;

	logstr.ReleaseBuffer();
	str1.ReleaseBuffer();
	str2.ReleaseBuffer();
	str3.ReleaseBuffer();
	for(long i=0;i<len;i=i+32)
	{
		str3 = "";
		for(long j=0;j<32;j++)
		{
			str1.Format("%02X ",buf[j+i]);
			str3 = str3 + str1;
		}
		str2.Format("%d:",*rowindex);
		*rowindex = *rowindex+1;

		str3 = str2 + str3+"\n";

		logstr += str3;
	}

	return logstr;
}
CString LogTool::FormatLog(unsigned short *buf, long len,long *rowindex)
{
	CString logstr;
	CString str1;
	CString str2;
	CString str3;

	logstr.ReleaseBuffer();
	str1.ReleaseBuffer();
	str2.ReleaseBuffer();
	str3.ReleaseBuffer();
	for(long i=0;i<len;i=i+32)
	{
		str3 = "";
		for(long j=0;j<32;j++)
		{
			str1.Format("%04X ",buf[j+i]);
			str3 = str3 + str1;
		}
		str2.Format("%d:",*rowindex);
		*rowindex = *rowindex+1;

		str3 = str2 + str3+"\n";

		logstr += str3;
	}

	return logstr;
}
CString LogTool::FormatLog1(UCHAR *buf, long len,long *rowindex)
{
	CString logstr;
	CString str1;
	CString str2;
	CString str3;

	logstr.ReleaseBuffer();
	str1.ReleaseBuffer();
	str2.ReleaseBuffer();
	str3.ReleaseBuffer();
	for(long i=0;i<len;i=i+1024)
	{
		str3 = "";
		for(long j=0;j<1024;j++)
		{
			str1.Format("%02X ",buf[j+i]);
			str3 = str3 + str1;
		}
		str2.Format("%d:",*rowindex);
		*rowindex = *rowindex+1;

		str3 = str2 + str3+"\n";

		logstr += str3;
	}

	return logstr;
}
void LogTool::FormatYuvFileSorceData(unsigned char *d, int width,int height)
{
	CString str1;
	CString str2;

	for(int i=0;i<height;i++)
	{
		str2 = "";
		for(long j=0;j<width;j++)
		{
			int n = i*2048*4+j*4;
			str1.Format("%02X%02X%02X%02X ",d[n],d[n+1],d[n+2],d[n+3]);
			str2 = str2 + str1;
		}
		str2 = str2 + "\t\n";
		Writelog(str2,"YuvFileSorceData");
	}
}
//保存行数据 d* 缓存首地址 rowid 行号
void LogTool::FormatYuvFileSourceRowData(unsigned char *d,int rowid)
{
	CString str1;
	CString str2;

	str2 = "";
	for(long j=0;j<2048;j++)
	{
		int n = rowid*2048*4+j*4;
		str1.Format("%02X%02X%02X%02X ",d[n],d[n+1],d[n+2],d[n+3]);
		str2 = str2 + str1;
	}
	str2 = str2 + "\t\n";
	CString fileName;
	fileName.Format("YuvFileSorceData_rowid%d",rowid);
	Writelog(str2,fileName);
}
//RGB d：数据源 nwidth：行长 rowid：第几行 0
void LogTool::FormatRGBRowData(unsigned char *d,int nwidth,int rowid)
{
	int ch1 = nwidth*3/8;

	CString str1;
	CString str2;

	str2 = "";
	for(long j=0;j<nwidth*3;j+=3)
	{
		int n = rowid*nwidth*3 + j;
		str1.Format("%02X%02X%02X ",d[n],d[n+1],d[n+2]);
		str2 = str2 + str1;
		
		if(n>200 && d[n-3]==0x00 && d[n-2]==0xFF && d[n-1] ==0x00 && d[n]==0x00 && d[n+1]==0xFF && d[n+2] ==0xFF)
		{
			int z = n;
		}
		
	}
	str2 = str2 + "\t\n";
	CString fileName;
	fileName.Format("RGBData-rowid%d_",rowid);
	Writelog(str2,fileName);
}
//YUV数据 
void LogTool::FormatYuvFileSourceRowData(CString tag,unsigned char *d,int width,int rowid)
{
	CString str1;
	CString str2;

	str2 = "";
	for(long j=0;j<width;j++)
	{
		int n = rowid*width*4+j*4;
		str1.Format("%02X%02X%02X%02X ",d[n],d[n+1],d[n+2],d[n+3]);
		str2 = str2 + str1;
	}
	str2 = str2 + "\t\n";
	CString fileName;
	fileName.Format("YuvFileSorceData_rowid%d",rowid);
	fileName = tag + fileName;
	Writelog(str2,fileName);
}
void LogTool::FormatYuvFileData(unsigned char *d, int width,int height)
{
	CString str1;
	CString str2;
	for(int i=0;i<height;i++)
	{
		str2 = "";
		for(long j=0;j<width;j++)
		{
			int n= i*2048*2+j*2;
			str1.Format("%02X%02X ",d[n],d[n+1]);
			str2 = str2 + str1;
		}
		str2 = str2 + "\t\n";
		Writelog(str2,"YuvFileData");
	}
}
//保存行数据 d* 缓存首地址 rowid 行号
void LogTool::FormatYuvFileRowData(unsigned char *d,int rowid)
{
	CString str1;
	CString str2;

	str2 = "";
	for(long j=0;j<2048;j++)
	{
		int n = rowid*2048*2+j*2;
		str1.Format("%02X%02X ",d[n],d[n+1]);
		str2 = str2 + str1;
	}
	str2 = str2 + "\t\n";
	CString fileName;
	fileName.Format("YuvFileData_rowid%d",rowid);
	Writelog(str2,fileName);
}

//打印数据
void LogTool::FormatRAWRowData(unsigned short *d,int nWidth,int rowcount)
{
	CString str1;
	CString str2;

	str2 = "";
	for(int j=0;j<rowcount;j++)
	{
		for(int i=0;i<nWidth;i++)
		{
			str1.Format("%d ",d[i+j*nWidth]);
			str2 = str2 + str1;
		}
		str2 = str2 + "\t\n";
	}
	str2 = str2 + "\t\n";
	CString fileName;
	fileName.Format("FormatRAWRowData");
	Writelog(str2,fileName);
}

void LogTool::FormatRGBFileData(unsigned char *d, int width,int height)
{
	CString str1;
	CString str2;
	for(int i=0;i<height;i++)
	{
		str2 = "";
		for(long j=0;j<width;j++)
		{
			int n= i*2048*3+j*3;
			str1.Format("%02X%02X%02X ",d[n],d[n+1],d[n+2]);
			str2 = str2 + str1;
		}
		str2 = str2 + "\t\n";
		Writelog(str2,"RGBFileData");
	}
}
//保存行数据 d* 缓存首地址 rowid 行号
void LogTool::FormatRGBFileRowData(unsigned char *d,int rowid)
{
	CString str1;
	CString str2;

	str2 = "";
	for(long j=0;j<2048;j++)
	{
		int n = rowid*2048*3+j*3;
		str1.Format("%02X%02X%02X ",d[n],d[n+1],d[n+2]);
		str2 = str2 + str1;
	}
	str2 = str2 + "\t\n";
	CString fileName;
	fileName.Format("RGBFileData_rowid%d",rowid);
	Writelog(str2,fileName);
}

void LogTool::FormatRGBFileRowData(CString tag,unsigned char *d,int width,int rowid)
{
	CString str1;
	CString str2;

	str2 = "";
	for(long j=0;j<width;j++)
	{
		int n = rowid*width*3+j*3;
		str1.Format("%02X%02X%02X ",d[n],d[n+1],d[n+2]);
		str2 = str2 + str1;
	}
	str2 = str2 + "\t\n";
	CString fileName;
	fileName.Format("RGBFileData_rowid%d",rowid);
	fileName = tag+fileName;
	Writelog(str2,fileName);
}

void LogTool::SaveSourceRowData(unsigned char *source,int rowid)
{
	CString str1;
	CString str2;

	str2 = "";
	for(long j=0;j<2048;j++)
	{
		int n = rowid*2048*4+j*4;
		str1.Format("%02X%02X%02X%02X ",source[n],source[n+1],source[n+2],source[n+3]);
		str2 = str2 + str1;
	}
	str2 = str2 + "\t\n";

	CString fileName;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	fileName.Format("原始数据第%d行%4d-%02d-%02d-%02d-%02d-%02d-%03d.yuv",rowid,sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	
	WriteRowDatalog(str2,fileName);
}
void LogTool::WriteRowDatalog(CString info,CString filename)
{
	CStdioFile sf;
	CString LogFilePath;
	CString tempstr="";
	
	LogFilePath = GetModulePath() + "\\LOG\\"+filename;
	
	if (sf.Open(LogFilePath, CFile::modeWrite))
	{ 
		sf.SeekToEnd();
	    sf.WriteString(info);
		sf.WriteString("\t\n");
		sf.Close();
	}
	else
	{
		if (sf.Open(LogFilePath, CFile::modeCreate|CFile::modeWrite))
		{ 
			sf.WriteString(info);
		    sf.WriteString("\t\n");
			sf.Close();
		}
	}
}

