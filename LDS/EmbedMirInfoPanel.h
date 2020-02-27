#pragma once
#include "CmdLineMiniPanel.h"


// CEmbedMirInfoPanel 对话框

class CEmbedMirInfoPanel : public CCmdLineMiniPanel
{
	DECLARE_DYNAMIC(CEmbedMirInfoPanel)

public:
	static BOOL m_bGlobalSpecOffset;
	static UINT m_nSpecOffset;
	CEmbedMirInfoPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEmbedMirInfoPanel();

// 对话框数据
	enum { IDD = IDD_EMBED_MIR_INFO_DLG };
	virtual UINT DlgResourceID(){return IDD_EMBED_MIR_INFO_DLG;}
	void FillPanelTitle(const char* sName);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedChkMirAll();
	afx_msg void OnClickedChkSpecOffset();
public:
	BOOL m_bMirAll;
	BOOL m_bSpecOffset;
};
