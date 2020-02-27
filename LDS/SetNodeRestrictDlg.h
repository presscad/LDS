#if !defined(AFX_SETNODERESTRICTDLG_H__669CA65B_A2C4_4BC1_AB65_3B0B78FE4E3F__INCLUDED_)
#define AFX_SETNODERESTRICTDLG_H__669CA65B_A2C4_4BC1_AB65_3B0B78FE4E3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetNodeRestrictDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetNodeRestrictDlg dialog

class CSetNodeRestrictDlg : public CDialog
{
// Construction
public:
	BOOL m_bNewRestrictNode;
	CSetNodeRestrictDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetNodeRestrictDlg)
	enum { IDD = IDD_SET_NODE_RISTRICT_DLG };
	BOOL	m_bRestrictX;
	BOOL	m_bRestrictY;
	BOOL	m_bRestrictZ;
	CString	m_hNode;
	long	m_nNodeI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetNodeRestrictDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetNodeRestrictDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETNODERESTRICTDLG_H__669CA65B_A2C4_4BC1_AB65_3B0B78FE4E3F__INCLUDED_)
