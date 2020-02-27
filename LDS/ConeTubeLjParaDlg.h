#if !defined(AFX_CONETUBELJPARADLG_H__37497180_106A_4C56_8B0F_8C1538D44720__INCLUDED_)
#define AFX_CONETUBELJPARADLG_H__37497180_106A_4C56_8B0F_8C1538D44720__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConeTubeLjParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConeTubeLjParaDlg dialog

class CConeTubeLjParaDlg : public CDialog
{
// Construction
public:
	CConeTubeLjParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConeTubeLjParaDlg)
	enum { IDD = IDD_CONETUBE_LJ_PARA_DLG };
	CString	m_sType;
	double	m_fLenght;
	double	m_fFlThick;
	double	m_fOutterRadius;
	long m_handle;   //记录当前所要设计的椎管句柄
	BOOL m_bStartLj; //是否是在起始端进行连接
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConeTubeLjParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConeTubeLjParaDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONETUBELJPARADLG_H__37497180_106A_4C56_8B0F_8C1538D44720__INCLUDED_)
