// CmdLineMiniPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "CmdLineMiniPanel.h"
#include "CmdLineDlg.h"


// CCmdLineMiniPanel 对话框

CCmdLineMiniPanel::CCmdLineMiniPanel(UINT nIDTemplate,CWnd* pParent /*=NULL*/)
	: CDialogEx(nIDTemplate, pParent)
{
	m_pCmdLineDlg=NULL;
	m_nPanelOriginalHeight=m_niBeforeHeight=0;
}
CCmdLineMiniPanel::~CCmdLineMiniPanel()
{
	if(m_pCmdLineDlg)
		m_pCmdLineDlg->DetachMiniCmdOptionPanel();
	m_pCmdLineDlg=NULL;
}
int CCmdLineMiniPanel::GetFixedPanelHeight(){
	if(m_nPanelOriginalHeight==0)
	{
		RECT rcClient;
		GetClientRect(&rcClient);
		m_nPanelOriginalHeight=rcClient.bottom-rcClient.top+1;
	}
	return m_nPanelOriginalHeight;
}
