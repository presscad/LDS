// SubstationQuartetBeamPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SubstationSquareBeamPage.h"
#include "afxdialogex.h"


// CSubstationSquareBeamPage 对话框

IMPLEMENT_DYNAMIC(CSubstationSquareBeamPage, CDialogEx)

CSubstationSquareBeamPage::CSubstationSquareBeamPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubstationSquareBeamPage::IDD, pParent)
{
	m_fBeamWidth=2000;
	m_fBeamHeight=2000;
	m_nInternodeNum=10;
	m_iPoleType=1;
}

CSubstationSquareBeamPage::~CSubstationSquareBeamPage()
{
}

void CSubstationSquareBeamPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_E_BEAM_WIDTH, m_fBeamWidth);
	DDX_Text(pDX, IDC_E_BEAM_HIGH, m_fBeamHeight);
	DDX_Text(pDX, IDC_E_INTERNODE_NUM, m_nInternodeNum);
	DDX_CBIndex(pDX, IDC_CMB_POLE_TYPE, m_iPoleType);
}


BEGIN_MESSAGE_MAP(CSubstationSquareBeamPage, CDialogEx)
END_MESSAGE_MAP()


// CSubstationSquareBeamPage 消息处理程序
