#pragma once
class SystemHelper
{
public:
	SystemHelper(void);
	~SystemHelper(void);

	static CString GetModulePath(HMODULE hModule  = NULL );
	static void Split(CString strSource,CStringArray& strDest,TCHAR sep);
	static int ChangeNum(CString str,int length);
};

