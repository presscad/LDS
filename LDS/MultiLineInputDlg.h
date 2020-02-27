#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "XhCharString.h"

// CMultiLineInputDlg 对话框

class CMultiLineInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CMultiLineInputDlg)

public:
	CString m_sSpaceValue;
public:
	CMultiLineInputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMultiLineInputDlg();

// 对话框数据
	enum { IDD = IDD_MULTI_LINE_INPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_xRichCtrl;
	CString m_sRichSpaceValue;
public:
	afx_msg void OnBnClickedConfirmInput();
};
