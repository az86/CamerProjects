#pragma once
class SetInfo
{
public:
	SetInfo(void);
	~SetInfo(void);

	CString Name;

	CString Direction;
	CString ReqType;
	CString Target;
	CString Reqcode;
	CString wValues;
	CString wIndexs;

	bool IsValueSeq;
	bool IsIndexSeq;
};

