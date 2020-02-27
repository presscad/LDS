#ifndef __SCREEN_TOOLS_H
#define __SCREEN_TOOLS_H
// ScrTools.h : header file
#include "resource.h"
//
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
//---00.03.01Wjh programed, Input(char* VarPrompt, ...)全局函数定义--//
int Input(char* VarPrompt, void* VarName, char VarType);
//------------------CGeneralInputDlg class defination-----------------///////
// CGeneralInputDlg dialog

class CGeneralInputDlg : public CDialog
{
// Construction
public:
	CGeneralInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneralInputDlg)
	enum { IDD = IDD_GENERAL_INPUT_DLG };
	CString	m_sVarVal;
	CString	m_sVarPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneralInputDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog
//----00.03.01Wjh programed, PromptBox(char* FormatStr,...)全局函数定义----//
int PromptBox(char* FormatStr,...);
//------------------CGeneralInputDlg class defination-----------------///////
// COneKeyDown class

class COneKeyDown : public CStatic
{
// Construction
public:
	COneKeyDown();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COneKeyDown)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COneKeyDown();

	// Generated message map functions
protected:
	//{{AFX_MSG(COneKeyDown)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CMessageDlg : public CDialog
{
// Construction
public:
	void DestroyDlg();
	CMessageDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CMessageDlg)
	enum { IDD = IDD_TALK_BOX_DLG };
	COneKeyDown	m_OneKeyDown;
	CString	m_sInformationPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageDlg)
	public:
#if defined(_WIN64)
	virtual INT_PTR DoModal();
#else
	virtual int DoModal();
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageDlg)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//---00.03.01Wjh programed, PutMessage(CView* pView,...)全局函数定义--//
int PutMessage(CWnd* pWnd, const char* FormatStr,...);
//------------------CPutMessage class defination-----------------///////
// CPutMessage dialog

class CPutMessage : public CDialog
{
// Construction
public:
	CPutMessage(CWnd* pWnd);
	BOOL Create();
	void Refresh();
private:
	CWnd* m_pWnd;
public:
// Dialog Data
	//{{AFX_DATA(CPutMessage)
	enum { IDD = IDD_PUTMESSAGE_DLG };
	CString	m_sMessageContent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPutMessage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPutMessage)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif// !defined(__SCREEN_TOOLS_H)