// CompulsiveBreakDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "CompulsiveBreakDlg.h"
#include "afxdialogex.h"


// CCompulsiveBreakDlg 对话框

IMPLEMENT_DYNAMIC(CCompulsiveBreakDlg, CDialogEx)

CCompulsiveBreakDlg::CCompulsiveBreakDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCompulsiveBreakDlg::IDD, pParent)
{

}

CCompulsiveBreakDlg::~CCompulsiveBreakDlg()
{
}

void CCompulsiveBreakDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCompulsiveBreakDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCompulsiveBreakDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCompulsiveBreakDlg 消息处理程序

void CCompulsiveBreakDlg::OnCancel()
{
	CDialogEx::OnCancel();
	DestroyWindow();
}

void CCompulsiveBreakDlg::OnBnClickedOk()
{
	CloseHandle(m_hPipeSrcReadDup);
	CDialogEx::OnOK();
	DestroyWindow();
}

void CCompulsiveBreakDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	delete this;
}
