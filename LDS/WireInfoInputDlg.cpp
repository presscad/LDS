// WireInfoInputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "WireInfoInputDlg.h"


// CWireInfoInputDlg

IMPLEMENT_DYNAMIC(CWireInfoInputDlg, CDialogEx)

CWireInfoInputDlg::CWireInfoInputDlg() : CDialogEx(CWireInfoInputDlg::IDD)
{
	m_iWireType=0;
	m_iHangStyle=0;
	m_iPhaseSerial=1;
	m_nPostCode=0;
	m_iTentionType=1;
	m_ciWireDir='Y';
	m_iTensionCmb=0;
}

CWireInfoInputDlg::~CWireInfoInputDlg()
{
}
void CWireInfoInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RDO_WIRE_TYPE, m_iWireType);
	DDX_Radio(pDX, IDC_RDO_HANG_STYLE, m_iHangStyle);
	DDX_Text(pDX, IDC_E_POST_CODE, m_nPostCode);
	DDX_CBIndex(pDX,IDC_CMB_TENSION_TYPE,m_iTensionCmb);
}

BEGIN_MESSAGE_MAP(CWireInfoInputDlg, CDialogEx)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_E_POST_CODE, OnChangeEPostCode)
	ON_CBN_SELCHANGE(IDC_CMB_PHASE_SERIAL, OnCbnSelchangeCmbPhaseSerial)
	ON_CBN_EDITCHANGE(IDC_CMB_PHASE_SERIAL, OnCbnEditchangeCmbPhaseSerial)
	ON_CBN_SELCHANGE(IDC_CMB_POSTION_TAG, OnCbnSelchangeCmbPostionTag)
	ON_CBN_SELCHANGE(IDC_CMB_TENSION_TYPE, OnCbnSelchangeCmbTensionType)
END_MESSAGE_MAP()



// CWireInfoInputDlg 消息处理程序
BOOL CWireInfoInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CSpinButtonCtrl* pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_POST_CODE);
	pSpinCtrl->SetBuddy((CEdit*)GetDlgItem(IDC_E_POST_CODE));
	pSpinCtrl->SetRange32(0,20);
	pSpinCtrl->SetPos(m_nPostCode);
	//
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_PHASE_SERIAL);
	while(pCMB->GetCount()>0)
		pCMB->DeleteString(0);	
	for(int i=1;i<=12;i++)
		pCMB->AddString(CXhChar16("%d",i));
	pCMB->SetWindowText(CXhChar16("%d",m_iPhaseSerial));
	//
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_POSTION_TAG);
	pCMB->AddString("Y向");
	pCMB->AddString("X向");
	pCMB->SetCurSel(0);
	//
	pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TENSION_TYPE);
	pCMB->AddString("悬垂");
	pCMB->AddString("耐张");
	pCMB->SetCurSel(m_iTensionCmb);
	UpdateData();
	return TRUE;
}

void CWireInfoInputDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_POST_CODE)
	{
		CString strValue;
		strValue.Format("%d", nPos);
		((CSpinButtonCtrl*) pScrollBar)->GetBuddy()->SetWindowText(strValue);
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWireInfoInputDlg::OnChangeEPostCode() 
{
	UpdateData();
	CSpinButtonCtrl* pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_POST_CODE);
	pSpinCtrl->SetPos(m_nPostCode);
}
void CWireInfoInputDlg::OnCbnSelchangeCmbPhaseSerial()
{
	UpdateData();
	CString sText;
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_PHASE_SERIAL);
	pCMB->GetLBText(pCMB->GetCurSel(),sText);
	m_iPhaseSerial=atoi(sText);
	UpdateData(FALSE);
}

void CWireInfoInputDlg::OnCbnEditchangeCmbPhaseSerial()
{
	UpdateData();
	CString sText;
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_PHASE_SERIAL);
	pCMB->GetWindowText(sText);
	m_iPhaseSerial=atoi(sText);
	UpdateData(FALSE);
}

void CWireInfoInputDlg::OnCbnSelchangeCmbPostionTag()
{
	UpdateData();
	CString sText;
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_POSTION_TAG);
	pCMB->GetWindowText(sText);
	if(sText.CompareNoCase("Y向")==0)
		m_ciWireDir='Y';
	else if(sText.CompareNoCase("X向")==0)
		m_ciWireDir='X';
	UpdateData(FALSE);
}

void CWireInfoInputDlg::OnCbnSelchangeCmbTensionType()
{
	UpdateData();
	CString sText;
	CComboBox* pCMB=(CComboBox*)GetDlgItem(IDC_CMB_TENSION_TYPE);
	pCMB->GetWindowText(sText);
	if(sText.CompareNoCase("悬垂")==0)
		m_iTentionType=1;
	else if(sText.CompareNoCase("耐张")==0)
		m_iTentionType=2;
	UpdateData(FALSE);
}
