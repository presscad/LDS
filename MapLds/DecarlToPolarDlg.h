#if !defined(AFX_DECARLTOPOLARDLG_H__BE2131E0_4202_4ED7_B752_655061EC52A5__INCLUDED_)
#define AFX_DECARLTOPOLARDLG_H__BE2131E0_4202_4ED7_B752_655061EC52A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecarlToPolarDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDecarlToPolarDlg dialog

class CDecarlToPolarDlg : public CDialog
{
// Construction
public:
	CDecarlToPolarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDecarlToPolarDlg)
	enum { IDD = IDD_DECARL_TO_POLAR_DLG };
	double	m_fA;
	double	m_fP;
	double	m_fPoleX;
	double	m_fPoleY;
	double	m_fX;
	double	m_fY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecarlToPolarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDecarlToPolarDlg)
	afx_msg void OnBnDecarlToPolar();
	afx_msg void OnBnPolarToDecarl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECARLTOPOLARDLG_H__BE2131E0_4202_4ED7_B752_655061EC52A5__INCLUDED_)
