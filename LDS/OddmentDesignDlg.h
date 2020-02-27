#if !defined(AFX_ODDMENTDESIGNDLG_H__AC02F3F9_3FE0_4DB0_850A_02B09A7448BC__INCLUDED_)
#define AFX_ODDMENTDESIGNDLG_H__AC02F3F9_3FE0_4DB0_850A_02B09A7448BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OddmentDesignDlg.h : header file
//
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// COddmentDesignDlg dialog

class COddmentDesignDlg : public CCallBackDialog
{
// Construction
public:
	COddmentDesignDlg(CWnd* pParent = NULL);   // standard constructor
	//
	BOOL m_bDispSelectBtn;
// Dialog Data
	//{{AFX_DATA(COddmentDesignDlg)
	enum { IDD = IDD_ODDMENT_DESIGN_DLG };
	int		m_iOddCalStyle;
	double	m_fCollideDist;
	CString	m_hRefPart1;
	CString	m_hRefPart2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COddmentDesignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COddmentDesignDlg)
		afx_msg void OnBtnSelRefPart1();
		afx_msg void OnBtnSelRefPart2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ODDMENTDESIGNDLG_H__AC02F3F9_3FE0_4DB0_850A_02B09A7448BC__INCLUDED_)
