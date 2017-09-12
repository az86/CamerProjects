// DebugSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DebugSetDlg.h"
#include "afxdialogex.h"
#include "SetInfoHelper.h"
#include "SystemHelper.h"
#include "DebugViewToolBar.h"
#include "MainView.h"


// DebugSetDlg dialog

IMPLEMENT_DYNAMIC(DebugSetDlg, CDialogEx)

DebugSetDlg::DebugSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(DebugSetDlg::IDD, pParent)
	, m_iAVDDVol(0)
	, m_iDOVDDVol(0)
	, m_iDVDDVol(0)
	, m_ClockValue(0)
	, m_ClockHzValue(0)
	, m_PictureFormat(0)
	, m_ADDRCString(_T("0000"))
	, m_REGCString(_T("0000"))
	, m_VALUECString(_T("0000"))
	, m_PhotoesPath(_T(""))
	, m_Gpio1(1)
	, m_Gpio2(1)
{

}

DebugSetDlg::~DebugSetDlg()
{
}

void DebugSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DIAODIANCTRL, m_DiaodianControlComb);
	DDX_Radio(pDX, IDC_RADIO1, m_iAVDDVol);
	DDX_Radio(pDX, IDC_RADIO4, m_iDOVDDVol);
	DDX_Radio(pDX, IDC_RADIO7, m_iDVDDVol);
	DDX_Radio(pDX, IDC_RADIO10, m_ClockValue);
	DDX_Text(pDX, IDC_EDIT_HZVAULE, m_ClockHzValue);
	DDX_Radio(pDX, IDC_RADIO15, m_PictureFormat);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_ADDRCString);
	DDX_Text(pDX, IDC_EDIT_REG, m_REGCString);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_VALUECString);
	DDX_Text(pDX, IDC_EDIT_PATHVALUE, m_PhotoesPath);
	DDX_Radio(pDX, IDC_RADIO17, m_Gpio1);
	DDX_Radio(pDX, IDC_RADIO19, m_Gpio2);
}


BEGIN_MESSAGE_MAP(DebugSetDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BT_DIAODIANCTRL, &DebugSetDlg::OnBnClickedBtDiaodianctrl)
	ON_BN_CLICKED(IDC_BT_RESET, &DebugSetDlg::OnBnClickedBtReset)
	ON_BN_CLICKED(IDC_BT_CLOCKCTRL, &DebugSetDlg::OnBnClickedBtClockctrl)

	ON_BN_CLICKED(IDC_RADIO1, &DebugSetDlg::OnClickedRadioAVDD)
	ON_BN_CLICKED(IDC_RADIO2, &DebugSetDlg::OnClickedRadioAVDD)
	ON_BN_CLICKED(IDC_RADIO3, &DebugSetDlg::OnClickedRadioAVDD)

	ON_BN_CLICKED(IDC_RADIO4, &DebugSetDlg::OnClickedRadioDOVDD)
	ON_BN_CLICKED(IDC_RADIO5, &DebugSetDlg::OnClickedRadioDOVDD)
	ON_BN_CLICKED(IDC_RADIO6, &DebugSetDlg::OnClickedRadioDOVDD)
	
	ON_BN_CLICKED(IDC_RADIO7, &DebugSetDlg::OnClickedRadioDVDD)
	ON_BN_CLICKED(IDC_RADIO8, &DebugSetDlg::OnClickedRadioDVDD)
	ON_BN_CLICKED(IDC_RADIO9, &DebugSetDlg::OnClickedRadioDVDD)

	ON_BN_CLICKED(IDC_RADIO10, &DebugSetDlg::OnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO11, &DebugSetDlg::OnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO12, &DebugSetDlg::OnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO13, &DebugSetDlg::OnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO14, &DebugSetDlg::OnClickedRadio10)
	ON_BN_CLICKED(IDOK, &DebugSetDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_HZVAULE, &DebugSetDlg::OnChangeEditHzvaule)
	ON_BN_CLICKED(IDC_BTN_I2CR, &DebugSetDlg::OnBnClickedBtnI2cr)
	ON_BN_CLICKED(IDC_BTN_I2CW, &DebugSetDlg::OnBnClickedBtnI2cw)

	ON_BN_CLICKED(IDC_RADIO15, &DebugSetDlg::OnClickedRadio15)
	ON_BN_CLICKED(IDC_RADIO16, &DebugSetDlg::OnClickedRadio15)
	ON_BN_CLICKED(IDC_BT_PHPATH, &DebugSetDlg::OnBnClickedBtPhpath)
	ON_BN_CLICKED(IDC_RADIO17, &DebugSetDlg::OnClickedRadio17)
	ON_BN_CLICKED(IDC_RADIO18, &DebugSetDlg::OnClickedRadio17)

	ON_BN_CLICKED(IDC_RADIO19, &DebugSetDlg::OnClickedRadio19)
	ON_BN_CLICKED(IDC_RADIO20, &DebugSetDlg::OnClickedRadio19)
END_MESSAGE_MAP()


// DebugSetDlg message handlers
void DebugSetDlg::OnBnClickedBtDiaodianctrl()
{
	int i = m_DiaodianControlComb.GetCurSel();
    if(i==0xffffffff) return;

	CString valuestr;
	m_DiaodianControlComb.GetLBText( i ,valuestr);

	SetDiaoDian(valuestr);

}
void DebugSetDlg::SetDiaoDian(CString valuestr)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("掉电控制");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	valuestr.ReleaseBuffer();
	int ivalue = SystemHelper::ChangeNum(valuestr,valuestr.GetLength());
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());
	this->m_USB3Camera->WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}

void DebugSetDlg::OnBnClickedBtReset()
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("复位控制");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	this->m_USB3Camera->WriteControl(rcode,atoi(info.wValues),atoi(info.wIndexs),buf,bytesToSend);
}

/*
void DebugSetDlg::OnBnClickedBtDacactrl()
{
	int i = m_DACIndexComb.GetCurSel();
    if(i==0xffffffff) return;

	int j = DACVauleCombox.GetCurSel();
    if(j==0xffffffff) return;

	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA控制");

	CString valuestr;
	DACVauleCombox.GetLBText( j ,valuestr);

	CString indexstr;
	m_DACIndexComb.GetLBText( i ,indexstr);

	int ivalue = 0;
	if(valuestr=="1.202")
	{
		ivalue = 0x75C0;
	}
	if(valuestr=="1.498")
	{
		ivalue = 0x7550;
	}
	if(valuestr=="1.800")
	{
		ivalue = 0x74DC;
	}
	if(valuestr=="2.498")
	{
		ivalue = 0x73D0;
	}
	if(valuestr=="2.803")
	{
		ivalue = 0x735B;
	}
	if(valuestr=="3.243")
	{
		ivalue = 0x7243;
	}
	if(valuestr=="3.244")
	{
		ivalue = 0x7143;
	}
	

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	indexstr.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(indexstr,indexstr.GetLength());
	this->m_USB3Camera->WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}

*/
void DebugSetDlg::OnBnClickedBtClockctrl()
{
	UpdateData(TRUE);
	
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("可编程时钟频率");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());

	int ivalue = 0x0000;

	int v = 384/m_ClockHzValue;
	v -=2 ;
	v += 0x4200;
	ivalue = v;

	this->m_USB3Camera->WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}
void DebugSetDlg::InitClockHzValue()
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("可编程时钟频率");

	CStringArray strArray;
	strArray.RemoveAll();

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());
	SystemHelper::Split(info.wValues,strArray,',');
	for(int i=0;i<strArray.GetSize();i++)
	{
	   strArray[i].ReleaseBuffer();
	   int ivalue = SystemHelper::ChangeNum(strArray[i],strArray[i].GetLength());
	   this->m_USB3Camera->WriteControl(rcode,ivalue,iindex,buf,bytesToSend);

	   Sleep(100);
	}
}
BOOL DebugSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	SetInfoHelper helper;
	SetInfo info;

	CStringArray strArray;
	strArray.RemoveAll();
	info =  helper.GetSetInfo("掉电控制");

	SystemHelper::Split(info.wValues,strArray,',');
	for(int i=0;i<strArray.GetSize();i++)
	{
	   m_DiaodianControlComb.AddString(strArray[i]);
	}
	m_DiaodianControlComb.SetCurSel(0);

	//SetDiaoDian("0x0000");

	m_iAVDDVol = 1;
	//SetAVDD(1);
	m_iDOVDDVol = 0;
	//SetDOVDD(0);
	m_iDVDDVol = 0;
	//SetDVDD(0);

	m_ClockValue = 1;
	//初始化时钟
	//m_IsInitClockHzValue = false;
	//SetClock(1);
	this->m_PhotoesPath = helper.GetPicturePath();

	UpdateData(FALSE);
	
	return TRUE;  
}
void DebugSetDlg::SetCamera(USB3Camera *camera)
{
	this->m_USB3Camera = camera;
}
void DebugSetDlg::SetPictureFormat(int f)
{
	this->m_PictureFormat = f;
}

void DebugSetDlg::OnClickedRadioAVDD()
{
	UpdateData(TRUE);

	SetAVDD(m_iAVDDVol);
}
void DebugSetDlg::SetAVDD(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA控制");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	switch(i)
	{
		case 0:ivalue = 0x7243;break;
		case 1:ivalue = 0x735B;break;
		case 2:ivalue = 0x73D0;break;
	}

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;
	this->m_USB3Camera->WriteControl(rcode,ivalue,0x0001,buf,bytesToSend);
}

void DebugSetDlg::OnClickedRadioDOVDD()
{
	UpdateData(TRUE);
	SetDOVDD(m_iDOVDDVol);
}
void DebugSetDlg::SetDOVDD(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA控制");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	switch(i)
	{
		case 0:ivalue = 0x735B;break;//
		case 1:ivalue = 0x73D0;break;//2.5V
		case 2:ivalue = 0x7550;break;//
	}

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	this->m_USB3Camera->WriteControl(rcode,ivalue,0x0002,buf,bytesToSend);
}

void DebugSetDlg::OnClickedRadioDVDD()
{
	UpdateData(TRUE);
	SetDVDD(m_iDVDDVol);
}
void DebugSetDlg::SetDVDD(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA控制");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	switch(i)
	{
		case 0:ivalue = 0x74DC;break;
		case 1:ivalue = 0x7550;break;
		case 2:ivalue = 0x75C0;break;
	}

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	this->m_USB3Camera->WriteControl(rcode,ivalue,0x0004,buf,bytesToSend);
}

void DebugSetDlg::OnClickedRadio10()
{
	UpdateData(TRUE);
	SetClock(m_ClockValue);
}
void  DebugSetDlg::SetClock(int i)
{
	if(m_IsInitClockHzValue==false)
	{
		InitClockHzValue();
		m_IsInitClockHzValue = true;
	}

	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("可编程时钟频率");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());

	int ivalue = 0x0000;

	switch( i)
	{
		case 0:ivalue=0x0C20;break;//12M
		case 1:ivalue=0x0C10;break;//24M
		case 2:ivalue=0x0C08;break;//48M
		case 3:ivalue=0x0C04;break;//96M
		case 4:
			m_ClockHzValue = m_ClockHzValue<=0?10:m_ClockHzValue;
			UpdateData(false);
			int ic = 384/m_ClockHzValue;
			ivalue = 0x0C*0x100+ic;
			break;
	}

	this->m_USB3Camera->WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}

void DebugSetDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void DebugSetDlg::OnChangeEditHzvaule()
{
	UpdateData(true);

	SetClock(4);

}


void DebugSetDlg::OnBnClickedBtnI2cr()
{
	UpdateData(true);

	int rcode = SystemHelper::ChangeNum(this->m_ADDRCString,this->m_ADDRCString.GetLength());
	int ivalue = SystemHelper::ChangeNum(this->m_REGCString,this->m_REGCString.GetLength());
	//int iindex = SystemHelper::ChangeNum(this->m_VALUECString,this->m_VALUECString.GetLength());
	unsigned char  buf[2];
	ZeroMemory(buf,2);
	long bytesToSend = 2;

	//this->m_USB3Camera->ReadControl(rcode,ivalue,iindex,buf,bytesToSend);
	this->m_USB3Camera->WriteControl(0xBF,rcode,0x0000,buf,bytesToSend);
	ZeroMemory(buf,2);
	this->m_USB3Camera->ReadControl(0xB6,ivalue,0x0000,buf,bytesToSend);
	//this->m_VALUECString
	CString str1;
	CString str2 = "0x";
	for(int i=1;i>=0;i--)
	{
		str1.Format("%02X",buf[i]);
		str2 += str1;
	}
	this->m_VALUECString = str2;

	UpdateData(false);
}


void DebugSetDlg::OnBnClickedBtnI2cw()
{
	UpdateData(true);

	int rcode = SystemHelper::ChangeNum(this->m_ADDRCString,this->m_ADDRCString.GetLength());
	int ivalue = SystemHelper::ChangeNum(this->m_REGCString,this->m_REGCString.GetLength());
	int iindex = SystemHelper::ChangeNum(this->m_VALUECString,this->m_VALUECString.GetLength());
	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	//this->m_USB3Camera->WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
	this->m_USB3Camera->WriteControl(0xBF,rcode,0x0000,buf,bytesToSend);
	this->m_USB3Camera->WriteControl(0xB2,ivalue,iindex,buf,bytesToSend);
}


void DebugSetDlg::OnClickedRadio15()
{
	UpdateData(true);

	int j = m_PictureFormat;

	SetInfoHelper helper;
	helper.SetPictureFormat(j);
}


void DebugSetDlg::OnBnClickedBtPhpath()
{
	UpdateData(true);

	CFileDialog *fileDlg = NULL;
	switch(m_PictureFormat)
	{
		case 0:
			fileDlg = new CFileDialog(FALSE,NULL,"照片.bmp", OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST,"照片 (*.bmp)|*.bmp||",NULL);
			break;
		case 1:
			fileDlg = new CFileDialog(FALSE,NULL,"照片.jpg", OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST,"照片 (*.jpg)|*.jpg||",NULL);
			break;
	}

	CString fileName;
	
	
	if(fileDlg->DoModal()==IDOK)
	{
		fileName = fileDlg->GetPathName();

		SetInfoHelper helper;
		helper.SetPicturePath(fileName);

		this->m_PhotoesPath = helper.GetPicturePath();
	}

	delete [] fileDlg;
	fileDlg = NULL;

	UpdateData(false);
}


void DebugSetDlg::OnClickedRadio17()
{
	UpdateData(true);

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	if(this->m_Gpio1==0)
	{
		this->m_USB3Camera->WriteControl(0xF1,0x0000,0x0000,buf,bytesToSend);
	}
	if(this->m_Gpio1==1)
	{
		this->m_USB3Camera->WriteControl(0xF1,0x0001,0x0000,buf,bytesToSend);
	}
}


void DebugSetDlg::OnClickedRadio19()
{
	UpdateData(true);

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	if(this->m_Gpio2==0)
	{
		this->m_USB3Camera->WriteControl(0xF2,0x0000,0x0000,buf,bytesToSend);
	}
	if(this->m_Gpio2==1)
	{
		this->m_USB3Camera->WriteControl(0xF2,0x0001,0x0000,buf,bytesToSend);
	}
}
