#if !defined(AFX_CHECKPARTNODLG_H__AAE82725_FC2A_4494_94D2_D95C7185FFC3__INCLUDED_)
#define AFX_CHECKPARTNODLG_H__AAE82725_FC2A_4494_94D2_D95C7185FFC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckPartNoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCheckPartNoDlg dialog

class CCheckPartNoDlg : public CDialog
{
// Construction
public:
	CString m_sCaption;
	CCheckPartNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCheckPartNoDlg)
	enum { IDD = IDD_CHECK_PART_NO_DLG };
	double	m_fScope;
	CString	m_sSegStr;
	BOOL	m_bReservFlNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckPartNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCheckPartNoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	bool m_bDisplayFlNoCtrl;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKPARTNODLG_H__AAE82725_FC2A_4494_94D2_D95C7185FFC3__INCLUDED_)
