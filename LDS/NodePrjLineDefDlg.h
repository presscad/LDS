#if !defined(AFX_NODEPRJLINEDEFDLG_H__BE82744F_D3D8_4B2A_9232_722FDEF324A0__INCLUDED_)
#define AFX_NODEPRJLINEDEFDLG_H__BE82744F_D3D8_4B2A_9232_722FDEF324A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NodePrjLineDefDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNodePrjLineDefDlg dialog

class CNodePrjLineDefDlg : public CDialog
{
// Construction
public:
	CNodePrjLineDefDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNodePrjLineDefDlg)
	enum { IDD = IDD_NODE_PROJECT_LINE_DEF_DLG };
	CString	m_hEndJg;
	CString	m_hStartJg;
	int		m_iStartJgBerStyle;
	int		m_iEndJgBerStyle;
	double	m_fStartJgLengOffsetX;
	double	m_fStartJgLengOffsetY;
	double	m_fEndJgLengOffsetX;
	double 	m_fEndJgLengOffsetY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNodePrjLineDefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNodePrjLineDefDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NODEPRJLINEDEFDLG_H__BE82744F_D3D8_4B2A_9232_722FDEF324A0__INCLUDED_)
