#if !defined(AFX_SIZEDIMENSIONDLG_H__7E6F407A_88E2_4A8C_816D_03CA3F62D46F__INCLUDED_)
#define AFX_SIZEDIMENSIONDLG_H__7E6F407A_88E2_4A8C_816D_03CA3F62D46F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SizeDimensionDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSizeDimensionDlg dialog

class CSizeDimensionDlg : public CDialog
{
// Construction
public:
	CSizeDimensionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSizeDimensionDlg)
	enum { IDD = IDD_SIZE_DIMENSION };
	int		m_nDimDirection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSizeDimensionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSizeDimensionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIZEDIMENSIONDLG_H__7E6F407A_88E2_4A8C_816D_03CA3F62D46F__INCLUDED_)
