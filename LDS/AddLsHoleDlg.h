#if !defined(AFX_ADDLSHOLEDLG_H__A6C843EE_4831_4F88_910B_ABDE4E81B6E7__INCLUDED_)
#define AFX_ADDLSHOLEDLG_H__A6C843EE_4831_4F88_910B_ABDE4E81B6E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddLsHoleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddLsHoleDlg dialog

class CAddLsHoleDlg : public CDialog
{
// Construction
public:
	CAddLsHoleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddLsHoleDlg)
	enum { IDD = IDD_ADD_LS_HOLE_DLG };
	int		m_iLsHole;
	double	m_fLsPosX;
	double	m_fLsPosY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddLsHoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddLsHoleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDLSHOLEDLG_H__A6C843EE_4831_4F88_910B_ABDE4E81B6E7__INCLUDED_)
