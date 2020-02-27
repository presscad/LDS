#if !defined(AFX_SETACTIVEMODULEDLG_H__3E236B08_BE12_4057_A980_EAC4E2416C56__INCLUDED_)
#define AFX_SETACTIVEMODULEDLG_H__3E236B08_BE12_4057_A980_EAC4E2416C56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetActiveModuleDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSetActiveModuleDlg dialog

class CSetActiveModuleDlg : public CDialog
{
// Construction
public:
	CSetActiveModuleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetActiveModuleDlg)
	enum { IDD = IDD_SET_ACTIVE_MODULE_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetActiveModuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetActiveModuleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCmbModuleNo();
	afx_msg void OnCbnSelchangeCmbLegQuadA();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_iBodyItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETACTIVEMODULEDLG_H__3E236B08_BE12_4057_A980_EAC4E2416C56__INCLUDED_)
