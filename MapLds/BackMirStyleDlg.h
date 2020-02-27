#if !defined(AFX_BACKMIRSTYLEDLG_H__13B5D320_5438_47A7_80D5_A03135F9668F__INCLUDED_)
#define AFX_BACKMIRSTYLEDLG_H__13B5D320_5438_47A7_80D5_A03135F9668F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackMirStyleDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CBackMirStyleDlg dialog

class CBackMirStyleDlg : public CDialog
{
// Construction
public:
	CBackMirStyleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBackMirStyleDlg)
	enum { IDD = IDD_BACK_MIR_STYLE_DLG };
	int		m_iBackMirStyle;
	BOOL	m_bDrawRefLine;
	BOOL	m_bDimOnceRoundSamePart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackMirStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBackMirStyleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKMIRSTYLEDLG_H__13B5D320_5438_47A7_80D5_A03135F9668F__INCLUDED_)
