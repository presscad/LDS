#if !defined(AFX_UPDATEUSERDATADLG_H__0ABE27E0_AD13_4BFA_A92A_9980076E2AF6__INCLUDED_)
#define AFX_UPDATEUSERDATADLG_H__0ABE27E0_AD13_4BFA_A92A_9980076E2AF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdateUserDataDlg.h : header file
//
#include "MsgDef.h"
/////////////////////////////////////////////////////////////////////////////
// CUpdateUserDataDlg dialog

class CUpdateUserDataDlg : public CDialog
{
// Construction
public:
	DWORD m_dwUpdateUser;
	CUpdateUserDataDlg(CWnd* pParent = NULL);   // standard constructor
	BRIEF_USER user_arr[32];

// Dialog Data
	//{{AFX_DATA(CUpdateUserDataDlg)
	enum { IDD = IDD_UPDATE_USER_DATA_DLG };
	CString	m_sCurListString;
	CString	m_sUserStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateUserDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUpdateUserDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEUSERDATADLG_H__0ABE27E0_AD13_4BFA_A92A_9980076E2AF6__INCLUDED_)
