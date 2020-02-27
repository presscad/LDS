#if !defined(AFX_IMPORTSCRIPTOPTIONDLG_H__F13BBB2B_B868_4230_B38A_BAE5937ACB56__INCLUDED_)
#define AFX_IMPORTSCRIPTOPTIONDLG_H__F13BBB2B_B868_4230_B38A_BAE5937ACB56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportScriptOptionDlg.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CImportScriptOptionDlg dialog

class CImportScriptOptionDlg : public CDialog
{
// Construction
public:
	CImportScriptOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportScriptOptionDlg)
	enum { IDD = IDD_IMPORT_SCRIPT_OPTION_DLG };
	BOOL	m_bStruMap;
	BOOL	m_bAnalysisMap;
	BOOL	m_bSumMap;
	CString	m_sSegStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportScriptOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportScriptOptionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTSCRIPTOPTIONDLG_H__F13BBB2B_B868_4230_B38A_BAE5937ACB56__INCLUDED_)
