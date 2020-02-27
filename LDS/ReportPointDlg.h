#if !defined(AFX_REPORTPOINTDLG_H__6D2DAA1B_885A_4662_91D8_793C08CF275C__INCLUDED_)
#define AFX_REPORTPOINTDLG_H__6D2DAA1B_885A_4662_91D8_793C08CF275C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportPointDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportPointDlg dialog

#include "Resource.h"

class CReportPointDlg : public CDialog
{
// Construction
public:
	CString m_sCaption;
	CReportPointDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReportPointDlg)
	enum { IDD = IDD_REPORT_POINT_DLG };
	double	m_fPointX;
	double	m_fPointY;
	double	m_fPointZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportPointDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	BOOL m_bEnableLock;
	BOOL	m_bLockPoint;
	BOOL	m_bReadOnly;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReportPointDlg)
	afx_msg void OnBnCopyPoint();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPastePoint();
	afx_msg void OnBnLockPoint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTPOINTDLG_H__6D2DAA1B_885A_4662_91D8_793C08CF275C__INCLUDED_)
