#pragma once
#include "resource.h"
#include "supergridctrl.h"
#include "NavigationBar.h"
#include ".\Log\XhLog.h"

// CLogVersionDlg 对话框

class CLogVersionDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogVersionDlg)
	CLogContents *m_pCurContents;
	void InitSuperGridCtrl();
	void InitNavigationBar();
public:
	CLogVersionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogVersionDlg();

// 对话框数据
	enum { IDD = IDD_LOG_VERSION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CSuperGridCtrl m_xLogContentList;
	CNavigationBar m_xNavigationBar;
	virtual BOOL OnInitDialog();
	//
	CXhLogVersion *m_pLogVersion;
	void RefreshSuperGridCtrl();
};
