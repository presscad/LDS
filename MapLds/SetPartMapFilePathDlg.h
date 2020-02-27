#if !defined(AFX_SETPARTMAPFILEPATHDLG_H__8B88F9A4_BB62_494F_AB6A_29131FEF4D12__INCLUDED_)
#define AFX_SETPARTMAPFILEPATHDLG_H__8B88F9A4_BB62_494F_AB6A_29131FEF4D12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetPartMapFilePathDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetPartMapFilePathDlg dialog
#include "resource.h"

class CSetPartMapFilePathDlg : public CDialog
{
// Construction
public:
	CSetPartMapFilePathDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetPartMapFilePathDlg)
	enum { IDD = IDD_SET_PART_MAP_FILE_PATH_DLG };
	double	m_fDrawRectLen;
	double	m_fDrawRectWidth;
	CString	m_sPartDrawingPath;
	BOOL	m_bUseProcessCard;
	BOOL	m_bDimPartInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetPartMapFilePathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetPartMapFilePathDlg)
	afx_msg void OnBntSetPath();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkUseProcessCard();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPARTMAPFILEPATHDLG_H__8B88F9A4_BB62_494F_AB6A_29131FEF4D12__INCLUDED_)
