#if !defined(AFX_XHLISTBOX_H__A62926EB_250F_4735_907F_F586127519E0__INCLUDED_)
#define AFX_XHLISTBOX_H__A62926EB_250F_4735_907F_F586127519E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XhListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXhListBox window

class CXhListBox : public CListBox
{
// Construction
public:
	CXhListBox();
	COLORREF crColor;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXhListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXhListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXhListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XHLISTBOX_H__A62926EB_250F_4735_907F_F586127519E0__INCLUDED_)
