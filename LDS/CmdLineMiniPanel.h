#pragma once


// CCmdLineMiniPanel ¶Ô»°¿ò

#include "afxdialogex.h"
class CCmdLineDlg;
class CCmdLineMiniPanel : public CDialogEx
{
protected:
	int m_nPanelOriginalHeight,m_niBeforeHeight;
	CCmdLineDlg* m_pCmdLineDlg;
	friend CCmdLineDlg;
	virtual UINT DlgResourceID()=0;
	virtual void SetParentCmdLineDlg(CCmdLineDlg* pParentCmdLine){
		m_pCmdLineDlg=pParentCmdLine;
	}
public:
	CCmdLineMiniPanel(UINT nIDTemplate,CWnd* pParent = NULL);
	virtual ~CCmdLineMiniPanel();
	virtual int DispatchCmdLineMsg(const char* szCmdMessage){return 0;}
	virtual int GetFixedPanelHeight();
	virtual int SetBeforePanelHeight(int niBeforeHeight){return m_niBeforeHeight=niBeforeHeight;}
	virtual int GetBeforePanelHeight(){return m_niBeforeHeight;}
};
