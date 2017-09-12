#include "StdAfx.h"
#include "CameraConfigureInfoHelper.h"
#include "SystemHelper.h"
#include "LogTool.h"


CameraConfigureInfoHelper::CameraConfigureInfoHelper(void)
{
}


CameraConfigureInfoHelper::~CameraConfigureInfoHelper(void)
{
}


CameraConfigureInfo CameraConfigureInfoHelper::GetCameraConfigureInfo(CString file)
{
	CameraConfigureInfo info;

	CString name = "SENSOR";
	GetPrivateProfileString(name,"Sensor Name","",info.SensorName.GetBuffer(MAX_PATH),MAX_PATH,file);
	info.SensorName = TrimString(info.SensorName,'/');

	info.MCLKSpeed = GetPrivateProfileInt(name,"MCLK Speed",0,file);
	info.I2CSlaveID = GetPrivateProfileInt(name,"I2C Slave ID",0,file);
	info.I2CMode = GetPrivateProfileInt(name,"I2C Mode",0,file);
	info.PreviewWidth = GetPrivateProfileInt(name,"Preview Width",0,file);
	info.PreviewHeight = GetPrivateProfileInt(name,"Preview Height",0,file);
	info.PreviewType = GetPrivateProfileInt(name,"Preview Type",0,file);
	info.OutFormat = GetPrivateProfileInt(name,"Out Format",0,file);
	info.ResetPin = GetPrivateProfileInt(name,"Reset Pin",0,file);

	info.PwdnPin = GetPrivateProfileInt(name,"Pwdn Pin",0,file);
	info.AFTYPE = GetPrivateProfileInt(name,"AF TYPE",0,file);
	info.I2CFlagReg = GetPrivateProfileInt(name,"I2C Flag Reg",0,file);
	info.I2CFlagValue = GetPrivateProfileInt(name,"I2C Flag Value",0,file);
	info.I2CFlagMask = GetPrivateProfileInt(name,"I2C Flag Mask",0,file);
	info.I2CFlagReg1 = GetPrivateProfileInt(name,"I2C Flag Reg1",0,file);
	info.I2CFlagValue1 = GetPrivateProfileInt(name,"I2C Flag Value1",0,file);
	info.I2CFlagMask1 = GetPrivateProfileInt(name,"I2C Flag Mask1",0,file);

	info.AVDD = GetPrivateProfileInt(name,"AVDD",0,file);
	info.DOVDD = GetPrivateProfileInt(name,"DOVDD",0,file);
	info.DVDD = GetPrivateProfileInt(name,"DVDD",0,file);

	memset(info.DefaultBuf,0x00,16*1024);
	int num= GetPrivateProfileSection("DEFAULT",info.DefaultBuf,16*1024,file);

	return info;
}
//·Ö¸î×Ö·û´®
void CameraConfigureInfoHelper::Split(char *DefaultBuf,int length,CStringArray& strDest)
{
	strDest.RemoveAll();

	CString str;
	CString str1;
	for(int i=0;i<length;i++)
	{
		/*
		CString logstr;
		logstr.Format("i:%d;length:%d",i,length);
		LogTool::Writelog(logstr+"--\t\n","CameraConfigureInfoHelper");
		*/
		if(i==569)
		{
			int z;
			z =1;
		}

		if(DefaultBuf[i]==0x00)
		{
			//AfxMessageBox(str);
			strDest.Add(str);
			//str = "";
			str.Format("");
		}
		else
		{
			str1.Format("%c",DefaultBuf[i]);
			str = str + str1;
		}
	}

	int jj;

}
//ÌÞ³ý//×Ö·ûÒÔºóµÄ×Ö·û´®
CString CameraConfigureInfoHelper::TrimString(CString strSource,CHAR ch)
{
	//strSource.ReleaseBuffer();

	CString tempstr = strSource;
	for(int i=0;i<strSource.GetLength();i++)
	{
		CHAR c = strSource.GetAt(i);
		if(c==ch)
		{
			tempstr = strSource.Left(i);
			break;
		}
	}

	return tempstr;
}
//É¸Ñ¡Êý¾Ý
void CameraConfigureInfoHelper::Split(CStringArray& source,int key,CStringArray& data)
{
	data.RemoveAll();

	CStringArray strdataArray;
	for(int i=0;i<source.GetSize();i++)
	{
		strdataArray.RemoveAll();
		SystemHelper::Split(source[i],strdataArray,',');

		int ivalue = SystemHelper::ChangeNum(strdataArray[i],strdataArray[i].GetLength());
		if(ivalue==key)
		{
			data.Add(source[i]);
		}
	}
}