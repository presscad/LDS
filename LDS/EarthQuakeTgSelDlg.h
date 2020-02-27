#if !defined(AFX_EARTHQUAKETGSELDLG_H__757BF820_8228_49C2_BA3C_72AEA9233842__INCLUDED_)
#define AFX_EARTHQUAKETGSELDLG_H__757BF820_8228_49C2_BA3C_72AEA9233842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EarthQuakeTgSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEarthQuakeTgSelDlg dialog

class CEarthQuakeTgSelDlg : public CDialog
{
// Construction
public:
	double m_fTg;
	CEarthQuakeTgSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEarthQuakeTgSelDlg)
	enum { IDD = IDD_EARTHQUAKE_TG_SEL_DLG };
	int		m_iDesignGroup;
	int		m_iEnvType;
	CString	m_sTg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEarthQuakeTgSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEarthQuakeTgSelDlg)
	afx_msg void OnSelchangeCmbInput();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EARTHQUAKETGSELDLG_H__757BF820_8228_49C2_BA3C_72AEA9233842__INCLUDED_)
