#if !defined(AFX_ARRANGEANDDRAWPLATESETDLG_H__31EF8E51_4335_490E_A09D_18E468D1451D__INCLUDED_)
#define AFX_ARRANGEANDDRAWPLATESETDLG_H__31EF8E51_4335_490E_A09D_18E468D1451D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArrangeAndDrawPlateSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArrangeAndDrawPlateSetDlg dialog
#include "resource.h"
class CArrangeAndDrawPlateSetDlg : public CDialog
{
// Construction
public:
	BOOL m_bAutoArrange;
	//
	CArrangeAndDrawPlateSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArrangeAndDrawPlateSetDlg)
	enum { IDD = IDD_PART_DRAWING_PARA_DLG };
	int	m_nMinDistance;
	int	m_nPaperW;
	int m_nPaperL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArrangeAndDrawPlateSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArrangeAndDrawPlateSetDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARRANGEANDDRAWPLATESETDLG_H__31EF8E51_4335_490E_A09D_18E468D1451D__INCLUDED_)
