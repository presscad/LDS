#if !defined(AFX_EQCHKSCOPEDLG_H__0C6E9AD6_711A_4D52_97E9_18137CE2F360__INCLUDED_)
#define AFX_EQCHKSCOPEDLG_H__0C6E9AD6_711A_4D52_97E9_18137CE2F360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if defined(__LDS_)||defined(__TSA_)
// EqChkScopeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEqChkScopeDlg dialog
class CEqChkScopeDlg : public CDialog
{
// Construction
public:
	CEqChkScopeDlg(CWnd* pParent = NULL);   // standard constructor
	int	m_iSelModelNo;
	CString m_sWorkStatusScope;	//¹¤¿ö×Ö·û´®

// Dialog Data
	//{{AFX_DATA(CEqChkScopeDlg)
	enum { IDD = IDD_EQ_CHECK_SCOPE_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEqChkScopeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEqChkScopeDlg)
	afx_msg void OnBnDelWorkstatus();
	afx_msg void OnBnAddWorkstatus();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListAttachWorkstatus();
	afx_msg void OnDblclkListRemainWorkstatus();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bUpdateBoltInfo;
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQCHKSCOPEDLG_H__0C6E9AD6_711A_4D52_97E9_18137CE2F360__INCLUDED_)
