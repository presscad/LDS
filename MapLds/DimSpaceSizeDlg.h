#if !defined(AFX_DIMSPACESIZEDLG_H__148B7538_40F3_4CEE_A7D8_2243FADDEDCA__INCLUDED_)
#define AFX_DIMSPACESIZEDLG_H__148B7538_40F3_4CEE_A7D8_2243FADDEDCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DimSpaceSizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDimSpaceSizeDlg dialog
#include "resource.h"
class CDimSpaceSizeDlg : public CDialog
{
// Construction
public:
	CDimSpaceSizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDimSpaceSizeDlg)
	enum { IDD = IDD_DIM_SPACE_SIZE_DLG };
	int		m_iDimDirection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDimSpaceSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDimSpaceSizeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMSPACESIZEDLG_H__148B7538_40F3_4CEE_A7D8_2243FADDEDCA__INCLUDED_)
