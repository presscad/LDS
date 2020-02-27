#if !defined(AFX_READMSGLISTDLG_H__F68AE029_4A8E_4BC0_B60B_840C3258A833__INCLUDED_)
#define AFX_READMSGLISTDLG_H__F68AE029_4A8E_4BC0_B60B_840C3258A833__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReadMsgListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReadMsgListDlg dialog

class CReadMsgListDlg : public CDialog
{
// Construction
public:
	CReadMsgListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReadMsgListDlg)
	enum { IDD = IDD_READ_MSG_LIST_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadMsgListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReadMsgListDlg)
	afx_msg void OnBnReadMsg();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READMSGLISTDLG_H__F68AE029_4A8E_4BC0_B60B_840C3258A833__INCLUDED_)
