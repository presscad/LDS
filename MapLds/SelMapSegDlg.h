#if !defined(AFX_SELMAPSEGDLG_H__52B87CBB_9335_41B0_98ED_AB9C47EC5A0A__INCLUDED_)
#define AFX_SELMAPSEGDLG_H__52B87CBB_9335_41B0_98ED_AB9C47EC5A0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelMapSegDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelMapSegDlg dialog
#include "resource.h"
class CSelMapSegDlg : public CDialog
{
// Construction
public:
	CString m_sCaption;
	CSelMapSegDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelMapSegDlg)
	enum { IDD = IDD_SEL_MAP_SEG };
	CString	m_sStruSegNo;
	int		m_nSegNum;
	CString	m_sTypeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelMapSegDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelMapSegDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELMAPSEGDLG_H__52B87CBB_9335_41B0_98ED_AB9C47EC5A0A__INCLUDED_)
