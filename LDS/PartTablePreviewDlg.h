#if !defined(AFX_PARTTABLEPREVIEWDLG_H__97E355D5_B836_462D_AEFD_C0FF8386B80A__INCLUDED_)
#define AFX_PARTTABLEPREVIEWDLG_H__97E355D5_B836_462D_AEFD_C0FF8386B80A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartTablePreviewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartTablePreviewDlg dialog
#include "XhListCtrl.h"

class CPartTablePreviewDlg : public CDialog
{
// Construction
public:
	CPartTablePreviewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPartTablePreviewDlg)
	enum { IDD = IDD_PART_TABLE_PREVIEW_DLG };
	CXhListCtrl	m_listPart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartTablePreviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartTablePreviewDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTTABLEPREVIEWDLG_H__97E355D5_B836_462D_AEFD_C0FF8386B80A__INCLUDED_)
