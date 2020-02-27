// CompulsiveBreakDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "CompulsiveBreakDlg.h"
#include "afxdialogex.h"


// CCompulsiveBreakDlg �Ի���

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


// CCompulsiveBreakDlg ��Ϣ�������

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
