#if !defined(AFX_LOADARROWLOCATEDLG_H__D1EE4E1D_418A_4C06_B8F1_EFDD0FBBE2D6__INCLUDED_)
#define AFX_LOADARROWLOCATEDLG_H__D1EE4E1D_418A_4C06_B8F1_EFDD0FBBE2D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadArrowLocateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadArrowLocateDlg dialog
#include "Resource.h"

class CLoadArrowLocateDlg : public CDialog
{
// Construction
public:
	CLoadArrowLocateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadArrowLocateDlg)
	enum { IDD = IDD_LOAD_ARROW_LOCATE_DLG };
	int		m_iXArrowLocate;
	int		m_iYArrowLocate;
	int		m_iZArrowLocate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadArrowLocateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadArrowLocateDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADARROWLOCATEDLG_H__D1EE4E1D_418A_4C06_B8F1_EFDD0FBBE2D6__INCLUDED_)
