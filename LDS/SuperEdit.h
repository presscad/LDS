#if !defined(AFX_SUPEREDIT_H__DF1D3591_6E1E_4473_89BD_6ECB158A6403__INCLUDED_)
#define AFX_SUPEREDIT_H__DF1D3591_6E1E_4473_89BD_6ECB158A6403__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuperEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSuperEdit window

class CSuperEdit : public CEdit
{
// Construction
public:
	CSuperEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuperEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSuperEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSuperEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPEREDIT_H__DF1D3591_6E1E_4473_89BD_6ECB158A6403__INCLUDED_)
