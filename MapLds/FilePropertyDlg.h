#if !defined(AFX_FILEPROPERTYDLG_H__6E978022_9690_46CE_A31C_5C9633471CB3__INCLUDED_)
#define AFX_FILEPROPERTYDLG_H__6E978022_9690_46CE_A31C_5C9633471CB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilePropertyDlg.h : header file
//
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFilePropertyDlg dialog

class CFilePropertyDlg : public CDialog
{
// Construction
public:
	CFilePropertyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilePropertyDlg)
	enum { IDD = IDD_FILE_PROPERTY_DLG };
	int		m_nNodeNum;
	CString	m_sFileVersion;
	CString	m_sPrjName;
	CString	m_sTaType;
	int		m_nTaNum;
	CString	m_sTaAlias;
	CString	m_sTaStampNo;
	CString	m_sTblMaker;
	CString	m_sCritic;
	CString	m_sAuditor;
	CString	m_sCompanyName;
	int		m_iPlankAreaStatType;
	int		m_iCirclePlankAreaType;
	int		m_nPartNum;
	CString m_sTaskNo;
	CString	 m_sTolerance;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilePropertyDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPROPERTYDLG_H__6E978022_9690_46CE_A31C_5C9633471CB3__INCLUDED_)
