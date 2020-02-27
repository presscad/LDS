#if !defined(AFX_GROUPJGSTYLEDLG_H__2E0FDC2A_E2C3_4375_BDBB_17EFB13EB960__INCLUDED_)
#define AFX_GROUPJGSTYLEDLG_H__2E0FDC2A_E2C3_4375_BDBB_17EFB13EB960__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupJgStyleDlg.h : header file
//
#include "MemberProperty.h"
/////////////////////////////////////////////////////////////////////////////
// CGroupJgStyleDlg dialog

class CGroupJgStyleDlg : public CDialog
{
// Construction
	int		m_iJgGroupStyle;
public:
	CGroupJgStyleDlg(CWnd* pParent = NULL);   // standard constructor
	DECLARE_READONLY_PROPERTY(WORD,wSectStyle);
	DECLARE_READONLY_PROPERTY(BYTE,ciMasterIndex);
// Dialog Data
	//{{AFX_DATA(CGroupJgStyleDlg)
	enum { IDD = IDD_GROUP_JG_STYLE_DLG };
	double	m_fFillThick;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupJgStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGroupJgStyleDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPJGSTYLEDLG_H__2E0FDC2A_E2C3_4375_BDBB_17EFB13EB960__INCLUDED_)
