#if !defined(AFX_LSCHECKPARADLG_H__918525BC_4CD4_4FA5_9553_D9FAAD6F7003__INCLUDED_)
#define AFX_LSCHECKPARADLG_H__918525BC_4CD4_4FA5_9553_D9FAAD6F7003__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LsCheckParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLsCheckParaDlg dialog

class CLsCheckParaDlg : public CDialog
{
// Construction
public:
	CLsCheckParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLsCheckParaDlg)
	enum { IDD = IDD_LS_CHECK_DLG };
	BOOL	m_bCutAngleDist;
	BOOL	m_bLsEndSpace;
	BOOL	m_bRollingSpace;
	double	m_fScope;
	BOOL	m_bBoltGuiGe;
	BOOL	m_bBoltL0;
	BOOL	m_bLsPlateEdgeSpace;
	CString	m_sSegStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLsCheckParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLsCheckParaDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LSCHECKPARADLG_H__918525BC_4CD4_4FA5_9553_D9FAAD6F7003__INCLUDED_)
