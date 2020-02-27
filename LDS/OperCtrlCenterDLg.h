#if !defined(AFX_OPERCTRLCENTERDLG_H__22102ECB_F55B_4024_8DC0_5BC179DF7F38__INCLUDED_)
#define AFX_OPERCTRLCENTERDLG_H__22102ECB_F55B_4024_8DC0_5BC179DF7F38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OperCtrlCenterDLg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COperCtrlCenterDLg dialog

class COperCtrlCenterDLg : public CDialog
{
// Construction
public:
	COperCtrlCenterDLg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COperCtrlCenterDLg)
	enum { IDD = IDD_OPER_CTRL_CENTER_DLG };
	double	m_fRotOrgX;
	double	m_fRotOrgY;
	double	m_fRotOrgZ;
	int		m_iZoomCenterStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COperCtrlCenterDLg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COperCtrlCenterDLg)
	afx_msg void OnBnPaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPERCTRLCENTERDLG_H__22102ECB_F55B_4024_8DC0_5BC179DF7F38__INCLUDED_)
