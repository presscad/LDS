#if !defined(AFX_OPENSIZEDLG_H__F273D2F9_402D_4708_A0FA_DF51EDF1DE6A__INCLUDED_)
#define AFX_OPENSIZEDLG_H__F273D2F9_402D_4708_A0FA_DF51EDF1DE6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenSizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenSizeDlg dialog

class COpenSizeDlg : public CDialog
{
// Construction
public:
	COpenSizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenSizeDlg)
	enum { IDD = IDD_OPENING_SIZE_DLG };
	double	m_fFrontOpen;
	double	m_fSideOpen;
	double	m_fVerticalHigh;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenSizeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENSIZEDLG_H__F273D2F9_402D_4708_A0FA_DF51EDF1DE6A__INCLUDED_)
