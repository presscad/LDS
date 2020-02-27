#if !defined(AFX_INPUTPARTNODLG_H__9465D641_4E28_4C1A_8F4F_997CF5AF39E6__INCLUDED_)
#define AFX_INPUTPARTNODLG_H__9465D641_4E28_4C1A_8F4F_997CF5AF39E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputPartNoDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CInputPartNoDlg dialog

class CInputPartNoDlg : public CDialog
{
// Construction
public:
	CString m_sDlgTitle;
	CInputPartNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputPartNoDlg)
	enum { IDD = IDD_INPUT_PART_CODE_DLG };
	CString	m_sPartNo;
	CString	m_sValName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputPartNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputPartNoDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTPARTNODLG_H__9465D641_4E28_4C1A_8F4F_997CF5AF39E6__INCLUDED_)
