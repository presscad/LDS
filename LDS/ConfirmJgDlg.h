#if !defined(AFX_CONFIRMJGDLG_H__DAAE9D0D_2A84_4554_9B77_A67F20918E96__INCLUDED_)
#define AFX_CONFIRMJGDLG_H__DAAE9D0D_2A84_4554_9B77_A67F20918E96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfirmJgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfirmJgDlg dialog
extern CEvent UserConfirm;
extern CEvent UserCancel;

class CConfirmJgDlg : public CDialog
{
// Construction
public:
	BOOL m_bMoudulateView;
	BOOL m_bExtEnd;
	BOOL m_bExtStart;
	CConfirmJgDlg(CWnd* pParent = NULL);   // standard constructor
	CConfirmJgDlg(CView* pView);   // standard constructor
	CView *m_pView;
	BOOL Create();
// Dialog Data
	//{{AFX_DATA(CConfirmJgDlg)
	enum { IDD = IDD_CONFIRM_JG_TIP_NODE_DLG };
	int		m_iJgExtendStyle;
	int		m_iJgStyle;
	int		m_iLocation;
	int		m_iPos;
	CString	m_sGuiGe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmJgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmJgDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnModulateView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIRMJGDLG_H__DAAE9D0D_2A84_4554_9B77_A67F20918E96__INCLUDED_)
