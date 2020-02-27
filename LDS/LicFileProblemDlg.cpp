// LicFileProbleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "LicFileProblemDlg.h"
#include "afxdialogex.h"


// CLicFileProbleDlg 对话框

IMPLEMENT_DYNAMIC(CLicFileProblemDlg, CDialogEx)

CLicFileProblemDlg::CLicFileProblemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLicFileProblemDlg::IDD, pParent)
{
	m_sFileName="";
}

CLicFileProblemDlg::~CLicFileProblemDlg()
{
}

void CLicFileProblemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_LIC_FILE_NAME, m_sFileName);
}

BEGIN_MESSAGE_MAP(CLicFileProblemDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_ADD_LIC_FILE, &CLicFileProblemDlg::OnBnClickedAddLicFile)
END_MESSAGE_MAP()

// CLicFileProbleDlg 消息处理程序

void CLicFileProblemDlg::OnBnClickedAddLicFile()
{
#ifdef __LMA_
	CFileDialog dlg(TRUE,"lma",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"证书文件(*.lic)|*.lic|");
#elif defined(__TMA_)
	CFileDialog dlg(TRUE,"tma",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"证书文件(*.lic)|*.lic|");
#elif defined(__LDS_)
	CFileDialog dlg(TRUE,"lds",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"证书文件(*.lic)|*.lic|");
#elif defined(__TDA_)
	CFileDialog dlg(TRUE,"tda",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"证书文件(*.lic)|*.lic|");
#else
	CFileDialog dlg(TRUE,"tsa",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"证书文件(*.lic)|*.lic|");
#endif
	dlg.m_ofn.lpstrTitle="指定引用证书";
	if(dlg.DoModal()==IDOK)
		m_sFileName=dlg.GetPathName();
	else
		return;
	UpdateData(FALSE);
	OnOK();
}

BOOL CLicFileProblemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CEdit   *pFileNameEdit=(CEdit*)GetDlgItem(IDC_E_LIC_FILE_NAME);
	if(m_sFileName.GetLength()<0)
		pFileNameEdit->SetWindowTextA("添加证书");
	pFileNameEdit->SetReadOnly();
	UpdateData(false);
	return TRUE; 
} 
