#if !defined(AFX_JGNODEDAOWEIDLG_H__D330B746_3B3A_4870_9B75_5D6624EC0066__INCLUDED_)
#define AFX_JGNODEDAOWEIDLG_H__D330B746_3B3A_4870_9B75_5D6624EC0066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JgNodeDaoWeiDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJgNodePrjDlg dialog

class CJgNodePrjDlg : public CDialog
{
// Construction
public:
	CJgNodePrjDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJgNodePrjDlg)
	enum { IDD = IDD_JG_NODE_DAOWEI_DLG };
	int		m_iPosStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJgNodePrjDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJgNodePrjDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JGNODEDAOWEIDLG_H__D330B746_3B3A_4870_9B75_5D6624EC0066__INCLUDED_)
