#if !defined(AFX_LSSPACEDLG_H__002A03B9_63FD_4D8B_9792_3DC7FAF461F5__INCLUDED_)
#define AFX_LSSPACEDLG_H__002A03B9_63FD_4D8B_9792_3DC7FAF461F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LsSpaceDlg.h : header file
//
#include "GridCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CLsSpaceDlg dialog

class CLsSpaceDlg : public CDialog
{
// Construction
public:
	int nColumnsN;
	int nRowsN;
	CListCtrl* m_pListCtrl;
	CLsSpaceDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Refresh(BOOL bFreshData);

// Dialog Data
	//{{AFX_DATA(CLsSpaceDlg)
	enum { IDD = IDD_LS_SPACE_TABLE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CGridCtrl m_Grid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLsSpaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLsSpaceDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnImportLsSpaceTbl();
	afx_msg void OnExportLsSpaceTbl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LSSPACEDLG_H__002A03B9_63FD_4D8B_9792_3DC7FAF461F5__INCLUDED_)
