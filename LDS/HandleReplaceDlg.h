#pragma once
#include "resource.h"
#include "CallBackDialog.h"

// CHandleReplaceDlg 对话框

class CHandleReplaceDlg : public CCallBackDialog
{
	//DECLARE_DYNAMIC(CHandleReplaceDlg)

public:
	CHandleReplaceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHandleReplaceDlg();

// 对话框数据
	enum { IDD = IDD_HANDLE_REPLACE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPickDestObj();
	CString m_hOldObject;
	CString m_hNewObject;
};
