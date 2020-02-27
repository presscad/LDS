#if !defined(AFX_DESIGNPADPLANKPARADLG_H__BC457067_BD2A_4B70_8876_85DB7FE88311__INCLUDED_)
#define AFX_DESIGNPADPLANKPARADLG_H__BC457067_BD2A_4B70_8876_85DB7FE88311__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignPadPlankParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignPadPlankParaDlg dialog

class CDesignPadPlankParaDlg : public CDialog
{
// Construction
public:
	CDesignPadPlankParaDlg(CWnd* pParent = NULL);   // standard constructor
	CDesignPadPlankPara *m_pPadPlankPara;
// Dialog Data
	//{{AFX_DATA(CDesignPadPlankParaDlg)
	enum { IDD = IDD_DESIGN_PAD_PLANK_PARA_DLG };
	CString	m_hBaseJg;
	CString	m_hCutBaseJg;
	CString	m_sIncLsStr;
	CString	m_sPartNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignPadPlankParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignPadPlankParaDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnThickStr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNPADPLANKPARADLG_H__BC457067_BD2A_4B70_8876_85DB7FE88311__INCLUDED_)
