#pragma once
#include "CameraConfigureInfo.h"

class CameraConfigureInfoHelper
{
public:
	CameraConfigureInfoHelper(void);
	~CameraConfigureInfoHelper(void);

	CameraConfigureInfo GetCameraConfigureInfo(CString file);
	void Split(char *DefaultBuf,int length,CStringArray& strDest);
	CString TrimString(CString strSource,CHAR ch);
	void Split(CStringArray& source,int key,CStringArray& data);

	
};

