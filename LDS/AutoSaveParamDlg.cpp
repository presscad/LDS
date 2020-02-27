// DisplaySaveFileParameter.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "AutoSaveParamDlg.h"
#include "afxdialogex.h"

// CAutoSaveParamDlg �Ի���
IMPLEMENT_DYNAMIC(CAutoSaveParamDlg, CDialogEx)

CAutoSaveParamDlg::CAutoSaveParamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoSaveParamDlg::IDD, pParent)
	, m_bAnymorePrompt(FALSE)
	, m_sAutoSaveTime(_T(""))
{
	m_sAutoSaveTime='2';
	m_iAutoSaveType=0;
}

CAutoSaveParamDlg::~CAutoSaveParamDlg()
{
}

void CAutoSaveParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_AUTO_SAVE_TIME, m_sAutoSaveTime);
	DDX_Control(pDX, IDC_COM_SAVE_FILE_TYPE, m_cmbSvaeFileType);
}


BEGIN_MESSAGE_MAP(CAutoSaveParamDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAutoSaveParamDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDisplaySaveFileParameter ��Ϣ�������

void CAutoSaveParamDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
	UpdateData(TRUE);
	 m_iAutoSaveType=m_cmbSvaeFileType.GetCurSel();
	if(m_bAnymorePrompt)
		theApp.WriteProfileString("Settings","displayAutoSaveParamDlg","TRUE");
}
BOOL CAutoSaveParamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_cmbSvaeFileType.AddString("���ļ�����");
	m_cmbSvaeFileType.AddString("���ļ�����");
	m_cmbSvaeFileType.SetCurSel(0);
	return TRUE;
}

