#if !defined(AFX_SEL2CROSSGROUPJGDLG_H__E45F27CC_E66B_4124_8CFF_B61F5D21EB44__INCLUDED_)
#define AFX_SEL2CROSSGROUPJGDLG_H__E45F27CC_E66B_4124_8CFF_B61F5D21EB44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelGroupJgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelGroupJgDlg dialog

class CSel2CrossGroupJgDlg : public CDialog
{
// Construction
public:
	CSel2CrossGroupJgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSel2CrossGroupJgDlg)
	enum { IDD = IDD_SEL_2CROSS_GROUP_SON_DLG };
	int		m_iCurOperJg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSel2CrossGroupJgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSel2CrossGroupJgDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// SelGroupJgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelGroupJgDlg dialog

class CSel4CrossGroupJgDlg : public CDialog
{
// Construction
public:
	CSel4CrossGroupJgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSel4CrossGroupJgDlg)
	enum { IDD = IDD_SEL_4CROSS_GROUP_SON_DLG };
	int		m_iCurOperJg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSel4CrossGroupJgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSel4CrossGroupJgDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSelTGroupJgDlg dialog

class CSelTGroupJgDlg : public CDialog
{
// Construction
public:
	CSelTGroupJgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelTGroupJgDlg)
	enum { IDD = IDD_SEL_T_GROUP_SON_DLG };
	int		m_iCurOperJg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelTGroupJgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelTGroupJgDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEL2CROSSGROUPJGDLG_H__E45F27CC_E66B_4124_8CFF_B61F5D21EB44__INCLUDED_)
