#if !defined(AFX_JGGUIGELIBDLG_H__B7F40F00_5266_4C6B_A150_38A615F4ED1A__INCLUDED_)
#define AFX_JGGUIGELIBDLG_H__B7F40F00_5266_4C6B_A150_38A615F4ED1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JgGuiGeLibDlg.h : header file
//
#include "supergridctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CJgZhunJuLibDlg dialog

class CJgZhunJuLibDlg : public CDialog
{
// Construction
public:
	CJgZhunJuLibDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Refresh();
	void WriteDataBase();
// Dialog Data
	//{{AFX_DATA(CJgZhunJuLibDlg)
	enum { IDD = IDD_JG_ZHUN_JU_LIB_DLG };
	CSuperGridCtrl	m_JgZhunjuList;
	CButton	m_Import;
	CButton	m_Export;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJgZhunJuLibDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJgZhunJuLibDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPasteData();
	afx_msg void OnDelData();
	afx_msg void OnBnImport();
	afx_msg void OnBnExport();
	virtual void OnOK();
	afx_msg void OnCannel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JGGUIGELIBDLG_H__B7F40F00_5266_4C6B_A150_38A615F4ED1A__INCLUDED_)
