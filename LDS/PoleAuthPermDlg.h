#if !defined(AFX_POLEAUTHPERMDLG_H__71B3EBC6_7D8C_4241_9FDB_C56DBFDE1DF8__INCLUDED_)
#define AFX_POLEAUTHPERMDLG_H__71B3EBC6_7D8C_4241_9FDB_C56DBFDE1DF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PoleAuthPermDlg.h : header file
//
#include "MsgDef.h"
/////////////////////////////////////////////////////////////////////////////
// CPoleAuthPermDlg dialog

class CPoleAuthPermDlg : public CDialog
{
// Construction
public:
	BOOL m_bEnableStartModify,m_bEnableBasicModify,m_bEnableEndModify;
	CPoleAuthPermDlg(CWnd* pParent = NULL);   // standard constructor
	DWORD m_dwStartPermission,m_dwBasicPermission,m_dwEndPermission;
	BRIEF_USER user_arr[32];

// Dialog Data
	//{{AFX_DATA(CPoleAuthPermDlg)
	enum { IDD = IDD_POLE_AUTH_PERM_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoleAuthPermDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPoleAuthPermDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLEAUTHPERMDLG_H__71B3EBC6_7D8C_4241_9FDB_C56DBFDE1DF8__INCLUDED_)
