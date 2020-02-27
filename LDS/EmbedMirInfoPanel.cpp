// MirInfoEmbedPanel.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "EmbedMirInfoPanel.h"

// CEmbedMirInfoPanel �Ի���

IMPLEMENT_DYNAMIC(CEmbedMirInfoPanel, CDialogEx)

BOOL CEmbedMirInfoPanel::m_bGlobalSpecOffset=TRUE;
UINT CEmbedMirInfoPanel::m_nSpecOffset=80;
CEmbedMirInfoPanel::CEmbedMirInfoPanel(CWnd* pParent /*=NULL*/)
	: CCmdLineMiniPanel(CEmbedMirInfoPanel::IDD, pParent)
{
	m_bMirAll=TRUE;
	m_bSpecOffset =m_bGlobalSpecOffset;
}

CEmbedMirInfoPanel::~CEmbedMirInfoPanel()
{
}

void CEmbedMirInfoPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_MIR_ALL, m_bMirAll);
	DDX_Check(pDX, IDC_CHK_SPEC_OFFSET, m_bSpecOffset);
}


BEGIN_MESSAGE_MAP(CEmbedMirInfoPanel, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_MIR_ALL, &CEmbedMirInfoPanel::OnBnClickedChkMirAll)
	ON_BN_CLICKED(IDC_CHK_SPEC_OFFSET, &CEmbedMirInfoPanel::OnClickedChkSpecOffset)
END_MESSAGE_MAP()


// CEmbedMirInfoPanel ��Ϣ�������


void CEmbedMirInfoPanel::OnBnClickedChkMirAll()
{
	UpdateData();
}

void CEmbedMirInfoPanel::FillPanelTitle(const char* sName)
{
	if(GetSafeHwnd()!=NULL)
		GetDlgItem(IDC_CHK_MIR_ALL)->SetWindowText(sName);
}

void CEmbedMirInfoPanel::OnClickedChkSpecOffset()
{
	UpdateData();
	m_bGlobalSpecOffset=m_bSpecOffset;
}
