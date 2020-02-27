#if !defined(AFX_LSGUIGETBLDLG_H__105B9902_7DD3_4FBE_B852_94FAA40234AC__INCLUDED_)
#define AFX_LSGUIGETBLDLG_H__105B9902_7DD3_4FBE_B852_94FAA40234AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LsGuiGeTblDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "GridCtrl.h"
// CLsGuiGeTblDlg dialog

class CLsGuiGeTblDlg : public CDialog
{
// Construction
public:
	int nColumnsN;
	int nRowsN;
	CListCtrl* m_pListCtrl;
	CLsGuiGeTblDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLsGuiGeTblDlg)
	enum { IDD = IDD_LS_GUIGE_TABLE_DLG };
	//}}AFX_DATA
	CGridCtrl m_Grid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLsGuiGeTblDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLsGuiGeTblDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnExportLsFile();
	afx_msg void OnBnImportLsFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL Refresh(BOOL bFreshData=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LSGUIGETBLDLG_H__105B9902_7DD3_4FBE_B852_94FAA40234AC__INCLUDED_)
