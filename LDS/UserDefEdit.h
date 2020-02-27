#if !defined(AFX_USERDEFEDIT_H__1D9E77E5_D1F7_412F_9D52_B04F0EEF733C__INCLUDED_)
#define AFX_USERDEFEDIT_H__1D9E77E5_D1F7_412F_9D52_B04F0EEF733C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserDefEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserDefEdit window

class CUserDefEdit : public CEdit
{
// Construction
public:
	CUserDefEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserDefEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUserDefEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUserDefEdit)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERDEFEDIT_H__1D9E77E5_D1F7_412F_9D52_B04F0EEF733C__INCLUDED_)
