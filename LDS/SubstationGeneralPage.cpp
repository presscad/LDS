// SubstationGeneralPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SubstationGeneralPage.h"
#include "afxdialogex.h"


// CSubstationGeneralPage 对话框

IMPLEMENT_DYNAMIC(CSubstationGeneralPage, CDialogEx)

CSubstationGeneralPage::CSubstationGeneralPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubstationGeneralPage::IDD, pParent)
{
	m_nSpanNum = 1;
	m_fBeamHeight=13630;
	m_fPostHeight=13500;
	m_fDL=100;
	m_iPostType = 0;
	m_iBeamType = 0;
	m_bStartSup = TRUE;
	m_bEndSup = TRUE;
	m_bDesignBeam=TRUE;
	m_bDesignPost=TRUE;
}

CSubstationGeneralPage::~CSubstationGeneralPage()
{
}

void CSubstationGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_COLSPAN, m_nSpanNum);
	DDX_Text(pDX, IDC_E_BH, m_fBeamHeight);
	DDX_Text(pDX, IDC_E_PH, m_fPostHeight);
	DDX_Text(pDX, IDC_E_DL, m_fDL);
	DDX_CBIndex(pDX, IDC_CMB_POST_TYPE, m_iPostType);
	DDX_CBIndex(pDX, IDC_CMB_BEAM_TYPE, m_iBeamType);
	DDX_Check(pDX, IDC_CHK_START, m_bStartSup);
	DDX_Check(pDX, IDC_CHK_END, m_bEndSup);
	DDX_Check(pDX, IDC_CHK_DESIGN_POST, m_bDesignPost);
	DDX_Check(pDX, IDC_CHK_DESIGN_BEAM, m_bDesignBeam);
}


BEGIN_MESSAGE_MAP(CSubstationGeneralPage, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_DESIGN_POST, &CSubstationGeneralPage::OnBnClickedChkDesignPost)
	ON_BN_CLICKED(IDC_CHK_DESIGN_BEAM, &CSubstationGeneralPage::OnBnClickedChkDesignBeam)
END_MESSAGE_MAP()


// CSubstationGeneralPage 消息处理程序


void CSubstationGeneralPage::OnBnClickedChkDesignPost()
{
	m_bDesignPost=!m_bDesignPost;
	GetDlgItem(IDC_CMB_POST_TYPE)->EnableWindow(m_bDesignPost);
	GetDlgItem(IDC_CHK_START)->EnableWindow(m_bDesignPost);
	GetDlgItem(IDC_CHK_END)->EnableWindow(m_bDesignPost);
	GetDlgItem(IDC_E_PH)->EnableWindow(m_bDesignPost);
}


void CSubstationGeneralPage::OnBnClickedChkDesignBeam()
{
	m_bDesignBeam=!m_bDesignBeam;
	GetDlgItem(IDC_CMB_BEAM_TYPE)->EnableWindow(m_bDesignBeam);
	GetDlgItem(IDC_E_BH)->EnableWindow(m_bDesignBeam);
	GetDlgItem(IDC_E_DL)->EnableWindow(m_bDesignBeam);
}
