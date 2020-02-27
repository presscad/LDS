#if !defined(AFX_ADDDIJIAOLSDLG_H__B3009013_ED9B_4B0A_A255_225441C7A3BA__INCLUDED_)
#define AFX_ADDDIJIAOLSDLG_H__B3009013_ED9B_4B0A_A255_225441C7A3BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddDiJiaoLsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddDiJiaoLsDlg dialog

class CAddDiJiaoLsDlg : public CDialog
{
// Construction
public:
	CString m_sCaption;
	CAddDiJiaoLsDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nDefaultS;
	BOOL m_bAddLs;
	bool m_bEnableRayAngleMode;

// Dialog Data
	//{{AFX_DATA(CAddDiJiaoLsDlg)
	enum { IDD = IDD_ADD_DIJIAO_LS_DLG };
	CString	m_sLsGuiGe;
	double	m_fHoleD;
	double	m_fX;
	double	m_fY;
	int		m_nMirN;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddDiJiaoLsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddDiJiaoLsDlg)
	afx_msg void OnSelchangeCmbLsGuige();
	afx_msg void OnEditchangeCmbLsGuige();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDDIJIAOLSDLG_H__B3009013_ED9B_4B0A_A255_225441C7A3BA__INCLUDED_)
