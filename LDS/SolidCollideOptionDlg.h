#if !defined(AFX_SOLIDCOLLIDEOPTIONDLG_H__6863034B_2D49_4F67_8275_1195A3529381__INCLUDED_)
#define AFX_SOLIDCOLLIDEOPTIONDLG_H__6863034B_2D49_4F67_8275_1195A3529381__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SolidCollideOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSolidCollideOptionDlg dialog

class CSolidCollideOptionDlg : public CDialog
{
// Construction
public:
	CSolidCollideOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSolidCollideOptionDlg)
	enum { IDD = IDD_SOLID_COLLIDE_OPTION_DLG };
	BOOL	m_bJgVsJg;
	BOOL	m_bJgVsLjb;
	BOOL	m_bJgVsLs;
	BOOL	m_bLjbVsLjb;
	BOOL	m_bLjbVsLs;
	BOOL	m_bLsVsLs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSolidCollideOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSolidCollideOptionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOLIDCOLLIDEOPTIONDLG_H__6863034B_2D49_4F67_8275_1195A3529381__INCLUDED_)
