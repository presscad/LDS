#pragma once
#include "resource.h"

// CCompulsiveBreakDlg 对话框

class CCompulsiveBreakDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCompulsiveBreakDlg)

public:
	HANDLE m_hPipeSrcReadDup;
	CCompulsiveBreakDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCompulsiveBreakDlg();

// 对话框数据
	enum { IDD = IDD_COMPULSIVE_BREAK_LOADCAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnCancel();
	virtual void OnDestroy();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
