#if !defined(AFX_WORKSTATUSLISTDLG_H__7889A31E_7087_4ED4_B260_C24686A957A2__INCLUDED_)
#define AFX_WORKSTATUSLISTDLG_H__7889A31E_7087_4ED4_B260_C24686A957A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkStatusListDlg.h : header file
//
#include "SuperGridCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CWorkStatusListDlg dialog

#if defined(__TSA_)||defined(__LDS_)
class CWorkStatusListDlg : public CDialog
{
// Construction
	void PasteDataToListCtrl(CSuperGridCtrl *pListCtr,int nMaxColNum,int iStartRow=0,int iStartCol=0);
public:
	CWorkStatusListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorkStatusListDlg)
	enum { IDD = IDD_WORK_STATUS_LIST_DLG };
	CSuperGridCtrl	m_listCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkStatusListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorkStatusListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnNewItem();
	afx_msg void OnPasteItem();
	afx_msg void OnDeleteItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSTATUSLISTDLG_H__7889A31E_7087_4ED4_B260_C24686A957A2__INCLUDED_)
