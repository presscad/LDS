// SubstationBeamPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SubstationBeamPage.h"
#include "afxdialogex.h"


// CSubstationBeamPage 对话框

IMPLEMENT_DYNAMIC(CSubstationBeamPage, CDialogEx)

CSubstationBeamPage::CSubstationBeamPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubstationBeamPage::IDD, pParent)
	, m_nRightBeamInternodeCount(0)
{
	m_fWidth1=1000;
	m_fWidth2=1600;
	m_fDlWidth=100;
	m_fLeftBeamLength=m_fRightBeamLength=4500;
	m_fBeamHeight=1800;
	m_nInternodeNum1=m_nRightBeamInternodeCount=0;
	m_nInternodeNum2=0;
	m_iPoleType=1;
}

CSubstationBeamPage::~CSubstationBeamPage()
{
}

void CSubstationBeamPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_E_WIDTH1, m_fWidth1);
	DDX_Text(pDX, IDC_E_WIDTH2,m_fWidth2);
	DDX_Text(pDX, IDC_E_LEFT_BEAM_LEN,m_fLeftBeamLength);
	DDX_Text(pDX, IDC_E_RIGHT_BEAM_LEN, m_fRightBeamLength);
	DDX_Text(pDX, IDC_E_TOPBEAM_H, m_fBeamHeight);
	DDX_CBIndex(pDX, IDC_CMB_POLE_TYPE, m_iPoleType);
	DDX_Text(pDX, IDC_E_DL_WIDTH, m_fDlWidth);
}


BEGIN_MESSAGE_MAP(CSubstationBeamPage, CDialogEx)
	ON_EN_CHANGE(IDC_E_LEFT_BEAM_LEN, &CSubstationBeamPage::OnEnChangeELeftBeamLen)
END_MESSAGE_MAP()


// CSubstationBeamPage 消息处理程序


void CSubstationBeamPage::OnEnChangeELeftBeamLen()
{
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_LEFT_BEAM_LEN);
	CEdit* pRightEdit=(CEdit*)GetDlgItem(IDC_E_RIGHT_BEAM_LEN);
	CString rString;
	pEdit->GetWindowText(rString);
	pRightEdit->SetWindowText(rString);
}
