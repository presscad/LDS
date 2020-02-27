#if !defined(AFX_SENDMSGDLG_H__4473451C_6D0F_4781_9EC6_3B60951886B1__INCLUDED_)
#define AFX_SENDMSGDLG_H__4473451C_6D0F_4781_9EC6_3B60951886B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendMsgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg dialog

class CSendMsgDlg : public CDialog
{
// Construction
public:
	CString m_sMsgReceiver;
	CSendMsgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendMsgDlg)
	enum { IDD = IDD_SEND_MSG_DLG };
	CString	m_sMsgContent;
	int		m_iMsgReceiver;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendMsgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDMSGDLG_H__4473451C_6D0F_4781_9EC6_3B60951886B1__INCLUDED_)
