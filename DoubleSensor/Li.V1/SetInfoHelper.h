#pragma once

#include "SetInfo.h"
class SetInfoHelper
{
public:
	SetInfoHelper(void);
	~SetInfoHelper(void);

	SetInfo GetSetInfo(CString controlname);
	void  SetPictureFormat(int f);
	int   GetPictureFormat();
	CString GetPicturePath();
	void  SetPicturePath(CString v);
};

