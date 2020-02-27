#if !defined(AFX_SPECIALSETTINGDLG_H__65A607C2_7676_4C3B_B014_CABE0A497F4B__INCLUDED_)
#define AFX_SPECIALSETTINGDLG_H__65A607C2_7676_4C3B_B014_CABE0A497F4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecialSettingDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSpecialSettingDlg dialog

class CSpecialSettingDlg : public CDialog
{
// Construction
public:
	CSpecialSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpecialSettingDlg)
	enum { IDD = IDD_SPECIAL_SETTING_DLG };
	//}}AFX_DATA
	BOOL	m_arrIsCanUse[5];
	double	m_arrValue[5];
	int txt_size0_wtoh1;
	char m_sProp[100];
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecialSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecialSettingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSumMap();
	afx_msg void OnStruMap();
	afx_msg void OnPartMap();
	afx_msg void OnChiefMap();
	afx_msg void OnWeldMap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECIALSETTINGDLG_H__65A607C2_7676_4C3B_B014_CABE0A497F4B__INCLUDED_)
