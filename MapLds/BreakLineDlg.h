#if !defined(AFX_BREAKLINEDLG_H__EBB95DF7_075A_4327_8434_7F10C559D9A1__INCLUDED_)
#define AFX_BREAKLINEDLG_H__EBB95DF7_075A_4327_8434_7F10C559D9A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BreakLineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBreakLineDlg dialog

#include "resource.h"

class CBreakLineDlg : public CDialog
{
// Construction
public:
	CBreakLineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBreakLineDlg)
	enum { IDD = IDD_BREAK_LINE_DLG };
	double	m_fBreakLineLen;
	double	m_fLenThreshold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBreakLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBreakLineDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BREAKLINEDLG_H__EBB95DF7_075A_4327_8434_7F10C559D9A1__INCLUDED_)
