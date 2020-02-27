#if !defined(AFX_DESCTRLPTDLG_H__726BB3BA_97A9_4623_9CCE_72A7F54956EB__INCLUDED_)
#define AFX_DESCTRLPTDLG_H__726BB3BA_97A9_4623_9CCE_72A7F54956EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesCtrlPtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesCtrlPtDlg dialog

class CDesCtrlPtDlg : public CDialog
{
// Construction
public:
	CDesCtrlPtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesCtrlPtDlg)
	enum { IDD = IDD_CTRL_PT_POS_DLG };
	int		m_iPosSpecStyle;
	CString	m_hBaseNode;
	int		m_iJgOffsetVec;
	int		m_iOffsetWing;
	CString	m_hBaseJg;
	double	m_fLenOffset;
	double	m_fWingOffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesCtrlPtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesCtrlPtDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESCTRLPTDLG_H__726BB3BA_97A9_4623_9CCE_72A7F54956EB__INCLUDED_)
