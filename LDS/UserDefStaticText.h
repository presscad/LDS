#if !defined(AFX_USERDEFSTATICTEXT_H__43532251_744D_47DE_A02E_BB0E82D15D8F__INCLUDED_)
#define AFX_USERDEFSTATICTEXT_H__43532251_744D_47DE_A02E_BB0E82D15D8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserDefStaticText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserDefStaticText window

class CUserDefStaticText : public CStatic
{
// Construction
public:
	CUserDefStaticText();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserDefStaticText)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUserDefStaticText();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUserDefStaticText)
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERDEFSTATICTEXT_H__43532251_744D_47DE_A02E_BB0E82D15D8F__INCLUDED_)
