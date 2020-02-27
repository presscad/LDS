#if !defined(AFX_ADDFILLPLANKDLG_H__58423B99_D033_41E7_98AE_A672A8D82A1C__INCLUDED_)
#define AFX_ADDFILLPLANKDLG_H__58423B99_D033_41E7_98AE_A672A8D82A1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFillPlankDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddFillPlankDlg dialog

class CAddFillPlankDlg : public CDialog
{
// Construction
public:
	CAddFillPlankDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddFillPlankDlg)
	enum { IDD = IDD_ADD_FILL_PLANK_DLG };
	double	m_fHigh;
	CString	m_sPartNo;
	double	m_fBaseX;
	double	m_fBaseY;
	double	m_fThick;
	double	m_fWidth;
	int		m_iPlankDirect;
	double	m_fBaseZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFillPlankDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddFillPlankDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFILLPLANKDLG_H__58423B99_D033_41E7_98AE_A672A8D82A1C__INCLUDED_)
