#include "StdAfx.h"
#include "SystemHelper.h"


SystemHelper::SystemHelper(void)
{
}


SystemHelper::~SystemHelper(void)
{
}

CString SystemHelper::GetModulePath(HMODULE hModule )
{
	TCHAR buf[MAX_PATH] = {'\0'};
	CString strDir, strTemp;

	::GetModuleFileName( hModule, buf, MAX_PATH);
	strTemp = buf;
	strDir = strTemp.Left( strTemp.ReverseFind('\\') + 1 );

	return strDir;
}

//�ָ��ַ���
void SystemHelper::Split(CString strSource,CStringArray& strDest,TCHAR sep)
{
	strDest.RemoveAll();

	TCHAR seps[] = _T(",");
	TCHAR *token,*next_token;
		
	token = _tcstok_s( (LPTSTR)(LPCTSTR)strSource, seps, &next_token);
	while( token != NULL )
	{
		strDest.Add(CString(token));
		token = _tcstok_s( NULL, seps, &next_token );
	}
}
int SystemHelper::ChangeNum(CString str,int length)   
{  
	char  revstr[16]={0};  //����ʮ�������ַ����ĳ��ȣ�����ע�����鲻ҪԽ��
	int   num[16]={0};   
	int   count=1;   
	int   result=0;   
	strcpy(revstr,str);   
	for   (int i=length-1;i>=0;i--)   
	{   
		if ((revstr[i]>='0') && (revstr[i]<='9'))   
			num[i]=revstr[i]-48;//�ַ�0��ASCIIֵΪ48
		else if ((revstr[i]>='a') && (revstr[i]<='f'))   
			num[i]=revstr[i]-'a'+10;   
		else if ((revstr[i]>='A') && (revstr[i]<='F'))   
			num[i]=revstr[i]-'A'+10;   
		else   
			num[i]=0; 
		result=result+num[i]*count;   
		count=count*16;//ʮ������(����ǰ˽��ƾ����������8)    
	}   
	return result;   
}