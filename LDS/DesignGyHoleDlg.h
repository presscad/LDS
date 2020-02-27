#if !defined(AFX_DESIGNGYHOLEDLG_H__F55F74AD_F8F0_40C2_BC26_F31DA0499FAB__INCLUDED_)
#define AFX_DESIGNGYHOLEDLG_H__F55F74AD_F8F0_40C2_BC26_F31DA0499FAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignGyHoleDlg.h : header file
//
#include "RememberLocationDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDesignGyHoleDlg dialog

class CDesignGyHoleDlg : public CRememberLocationDlg
{
// Construction
public:
	CDesignGyHoleDlg(CWnd* pParent = NULL);   // standard constructor
	CBitmap m_bmpCirPlate;
// Dialog Data
	//{{AFX_DATA(CDesignGyHoleDlg)
	enum { IDD = IDD_DESIGN_GY_HOLE_DLG };
	CString	m_sBoltSize;
	double	m_fHoleD;
	double	m_fParaH;
	BOOL	m_bIncDownHole;
	BOOL	m_bIncUpHole;
	double	m_fParaS;
	BOOL	m_bIncludeHoleR;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignGyHoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignGyHoleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBoltSize();
	afx_msg void OnChangeHoleParaH();
	afx_msg void OnIncludeHoleR();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNGYHOLEDLG_H__F55F74AD_F8F0_40C2_BC26_F31DA0499FAB__INCLUDED_)
