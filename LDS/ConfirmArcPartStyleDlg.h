#if !defined(AFX_CONFIRMARCPARTSTYLEDLG_H__A4EFEFF0_34EE_43A6_B858_DE116302CFF6__INCLUDED_)
#define AFX_CONFIRMARCPARTSTYLEDLG_H__A4EFEFF0_34EE_43A6_B858_DE116302CFF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfirmArcPartStyleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfirmArcPartStyleDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CConfirmArcPartStyleDlg : public CDialog
{
// Construction
public:
	int m_iArcPartType;	//0-실멀 1-꾼멀 2-기멀
	CBitmap m_CurPlateBmp;
	CConfirmArcPartStyleDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CConfirmArcPartStyleDlg)
	enum { IDD = IDD_CONFIRM_ARCPART_STYLE_DLG };
	int		m_iArcPartLayStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmArcPartStyleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmArcPartStyleDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIRMARCPARTSTYLEDLG_H__A4EFEFF0_34EE_43A6_B858_DE116302CFF6__INCLUDED_)
