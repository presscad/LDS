#if !defined(AFX_PARTAUTHPERMDLG_H__A90794D9_2E83_4200_8795_78CE4F53CC08__INCLUDED_)
#define AFX_PARTAUTHPERMDLG_H__A90794D9_2E83_4200_8795_78CE4F53CC08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartAuthPermDlg.h : header file
//
#include "MsgDef.h"
/////////////////////////////////////////////////////////////////////////////
// CPartAuthPermDlg dialog

class CPartAuthPermDlg : public CDialog
{
// Construction
public:
	BOOL m_bEnableModify;
	int m_dwPermission;
	CPartAuthPermDlg(CWnd* pParent = NULL);   // standard constructor
	BRIEF_USER user_arr[32];

// Dialog Data
	//{{AFX_DATA(CPartAuthPermDlg)
	enum { IDD = IDD_PART_AUTH_PERM_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartAuthPermDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartAuthPermDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTAUTHPERMDLG_H__A90794D9_2E83_4200_8795_78CE4F53CC08__INCLUDED_)
