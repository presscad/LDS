#if !defined(AFX_DEFSCOPESTRUDLG_H__E3FD3B3E_C6CD_43E3_B22C_D276035F2A60__INCLUDED_)
#define AFX_DEFSCOPESTRUDLG_H__E3FD3B3E_C6CD_43E3_B22C_D276035F2A60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefScopeStruDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefScopeStruDlg dialog

class CDefScopeStruDlg : public CDialog
{
// Construction
public:
	CDefScopeStruDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefScopeStruDlg)
	enum { IDD = IDD_DEF_SCOPE_STRU_DLG };
	double	m_fMaxX;
	double	m_fMaxY;
	double	m_fMaxZ;
	double	m_fMinX;
	double	m_fMinY;
	double	m_fMinZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefScopeStruDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefScopeStruDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFSCOPESTRUDLG_H__E3FD3B3E_C6CD_43E3_B22C_D276035F2A60__INCLUDED_)
