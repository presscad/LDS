#pragma once

#include "resource.h"
// CExportSetDlg 对话框

class CExportSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CExportSetDlg)

public:
	CExportSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExportSetDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPORT_SET_DLG };
#endif
	int m_iModelLevel;
	int m_iLengthUnit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
