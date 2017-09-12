#include "StdAfx.h"
#include "SetInfoHelper.h"
#include "SystemHelper.h"

SetInfoHelper::SetInfoHelper(void)
{
}


SetInfoHelper::~SetInfoHelper(void)
{
}

SetInfo SetInfoHelper::GetSetInfo(CString controlname)
{
	SetInfo info;

	info.Name = controlname;
	info.Direction.ReleaseBuffer();
	info.ReqType.ReleaseBuffer();
	info.Target.ReleaseBuffer();
	info.Reqcode.ReleaseBuffer();
	info.wValues.ReleaseBuffer();
	info.wIndexs.ReleaseBuffer();


	CString file = SystemHelper::GetModulePath(NULL) + "\MyDefault.ini";

	GetPrivateProfileString(controlname,"Direction","",info.Direction.GetBuffer(MAX_PATH),MAX_PATH,file);
	GetPrivateProfileString(controlname,"ReqType","",info.ReqType.GetBuffer(MAX_PATH),MAX_PATH,file);
	GetPrivateProfileString(controlname,"Target","",info.Target.GetBuffer(MAX_PATH),MAX_PATH,file);
	
	GetPrivateProfileString(controlname,"Reqcode","",info.Reqcode.GetBuffer(MAX_PATH),MAX_PATH,file);

	GetPrivateProfileString(controlname,"wValues","",info.wValues.GetBuffer(MAX_PATH),MAX_PATH,file);
	GetPrivateProfileString(controlname,"wIndexs","",info.wIndexs.GetBuffer(MAX_PATH),MAX_PATH,file);

	info.IsValueSeq = GetPrivateProfileInt(controlname,"IsValueSeq",0,file);
	info.IsIndexSeq = GetPrivateProfileInt(controlname,"IsIndexSeq",0,file);

	
	return info;
}
int SetInfoHelper::GetPictureFormat()
{
	CString file = SystemHelper::GetModulePath(NULL) + "\MyDefault.ini";

	return GetPrivateProfileInt("PictureFormat","Vaule",0,file);
}
void SetInfoHelper::SetPictureFormat(int f)
{
	CString file = SystemHelper::GetModulePath(NULL) + "\MyDefault.ini";
	CString v;
	v.Format("%d",f);
	WritePrivateProfileString("PictureFormat","Vaule",v,file);
}
CString SetInfoHelper::GetPicturePath()
{
	CString path;
	CString file = SystemHelper::GetModulePath(NULL) + "\MyDefault.ini";
	GetPrivateProfileString("PictureFormat","Path","",path.GetBuffer(MAX_PATH),MAX_PATH,file);

	return path;
}
void  SetInfoHelper::SetPicturePath(CString v)
{
	CString file = SystemHelper::GetModulePath(NULL) + "\MyDefault.ini";

	int idx = v.ReverseFind('\\');
	CString strResult;
	strResult = v.Mid(0, idx);
	//CString v2 = 
	WritePrivateProfileString("PictureFormat","Path",strResult,file);
}
