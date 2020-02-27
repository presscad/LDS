#if !defined(AFX_SELCURFACEDLG_H__5126C9AC_FE42_48A8_8322_795B1F5A1671__INCLUDED_)
#define AFX_SELCURFACEDLG_H__5126C9AC_FE42_48A8_8322_795B1F5A1671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelCurFaceFlagDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelCurFaceFlagDlg dialog
#include "resource.h"

class CSelCurFaceFlagDlg : public CDialog
{
// Construction
public:
	CSelCurFaceFlagDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelCurFaceFlagDlg)
	enum { IDD = IDD_SEL_CUR_FACE_FLAG_DLG };
	int		m_iCurFace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelCurFaceFlagDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelCurFaceFlagDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELCURFACEDLG_H__5126C9AC_FE42_48A8_8322_795B1F5A1671__INCLUDED_)
