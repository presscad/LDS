#if !defined(AFX_CUTANGLEDLG_H__1EB4FD93_C670_4F92_8572_7248CC688652__INCLUDED_)
#define AFX_CUTANGLEDLG_H__1EB4FD93_C670_4F92_8572_7248CC688652__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CutAngleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCutAngleDlg dialog

class CCutAngleDlg : public CDialog
{
// Construction
public:
	CCutAngleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCutAngleDlg)
	enum { IDD = IDD_CUT_ANGLE_DLG };
	double	m_fLen2;
	double	m_fRadius;
	int		m_iOperType;
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCutAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCutAngleDlg)
	afx_msg void OnOperType();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTANGLEDLG_H__1EB4FD93_C670_4F92_8572_7248CC688652__INCLUDED_)
