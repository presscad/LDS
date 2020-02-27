#if !defined(AFX_ADDPARTLISTDLG_H__2B0F0824_6A05_4AAB_BC50_6435FF758C8D__INCLUDED_)
#define AFX_SUPPPARTLISTDLG_H__2B0F0824_6A05_4AAB_BC50_6435FF758C8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuppPartListDlg.h : header file
//
#include "supergridctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSuppPartListDlg dialog

#ifndef __TSA_
class CSuppPartListDlg : public CDialog
{
// Construction
public:
	CSuppPartListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSuppPartListDlg)
	enum { IDD = IDD_SUPP_PART_LIST_DLG };
	CSuperGridCtrl	m_AddPartList;
	//}}AFX_DATA
	CFGWORD cfgword[500];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuppPartListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSuppPartListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteDate();
	virtual void OnOK();
	afx_msg void OnConfirm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPPPARTLISTDLG_H__2B0F0824_6A05_4AAB_BC50_6435FF758C8D__INCLUDED_)
