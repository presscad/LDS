#if !defined(AFX_INSERTMAPFRMDLG_H__BDA417CA_1D61_46EE_ACAC_046BDF937F76__INCLUDED_)
#define AFX_INSERTMAPFRMDLG_H__BDA417CA_1D61_46EE_ACAC_046BDF937F76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InsertMapFrmDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CInsertMapFrmDlg dialog

class CInsertMapFrmDlg : public CDialog
{
// Construction
public:
	CInsertMapFrmDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInsertMapFrmDlg)
	enum { IDD = IDD_INSERT_MAP_FRAME_DLG };
	int		m_iMapFrmStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertMapFrmDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInsertMapFrmDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTMAPFRMDLG_H__BDA417CA_1D61_46EE_ACAC_046BDF937F76__INCLUDED_)
