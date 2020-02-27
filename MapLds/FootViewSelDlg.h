#if !defined(AFX_FOOTVIEWSELDLG_H__DA8C379A_2186_427B_AE45_9020B5C2AC4D__INCLUDED_)
#define AFX_FOOTVIEWSELDLG_H__DA8C379A_2186_427B_AE45_9020B5C2AC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FootViewSelDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CFootViewSelDlg dialog

class CFootViewSelDlg : public CDialog
{
// Construction
public:
	CFootViewSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFootViewSelDlg)
	enum { IDD = IDD_FOOT_VIEW_DLG };
	int		m_iViewStyle;
	CString	m_hFootHandle;
	int		m_nScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFootViewSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFootViewSelDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOOTVIEWSELDLG_H__DA8C379A_2186_427B_AE45_9020B5C2AC4D__INCLUDED_)
