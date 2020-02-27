// SubstationPostPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SubstationPostPage.h"
#include "afxdialogex.h"


// CSubstationPostPage 对话框

IMPLEMENT_DYNAMIC(CSubstationPostPage, CDialogEx)

CSubstationPostPage::CSubstationPostPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubstationPostPage::IDD, pParent)
{
	m_fTopWidth=150;
	m_fBtmWidth=3500;
	m_fLinkHigh=5000;
	m_fTopSideWidth=100;
	m_fBtmSideWidth=3000;
}

CSubstationPostPage::~CSubstationPostPage()
{
}

void CSubstationPostPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_TOP_W, m_fTopWidth);
	DDX_Text(pDX, IDC_E_BTM_W, m_fBtmWidth);
	DDX_Text(pDX, IDC_E_LINK_H, m_fLinkHigh);
	DDX_Text(pDX, IDC_E_TOP_DIS, m_fTopSideWidth);
	DDX_Text(pDX, IDC_E_BTM_DIS,m_fBtmSideWidth);
}


BEGIN_MESSAGE_MAP(CSubstationPostPage, CDialogEx)
END_MESSAGE_MAP()


// CSubstationPostPage 消息处理程序
