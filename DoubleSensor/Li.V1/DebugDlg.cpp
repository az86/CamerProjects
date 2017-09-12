// DebugDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DebugDlg.h"
#include "afxdialogex.h"


// DebugDlg dialog

IMPLEMENT_DYNAMIC(DebugDlg, CDialog)

DebugDlg::DebugDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DebugDlg::IDD, pParent)
{

}

DebugDlg::~DebugDlg()
{
}

void DebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DebugDlg, CDialog)
END_MESSAGE_MAP()


// DebugDlg message handlers
