#if !defined(AFX_MODFIYPARTNODLG_H__54BC98A0_2539_41F1_8209_C90561AE0ED0__INCLUDED_)
#define AFX_MODFIYPARTNODLG_H__54BC98A0_2539_41F1_8209_C90561AE0ED0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModfiyPartNoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModfiyPartNoDlg dialog

class CModfiyPartNoDlg : public CDialog
{
// Construction
public:
	CModfiyPartNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModfiyPartNoDlg)
	enum { IDD = IDD_MODIFY_NOINFO_DLG };
	CEdit	m_editPartNo;
	CString	m_sType;
	CString	m_sPartNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModfiyPartNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModfiyPartNoDlg)
	afx_msg void OnSelchangePartNoType();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODFIYPARTNODLG_H__54BC98A0_2539_41F1_8209_C90561AE0ED0__INCLUDED_)
