#if !defined(AFX_INPUTJGGDLG_H__5CF3383C_BCA2_4991_8E3C_D00D460AB071__INCLUDED_)
#define AFX_INPUTJGGDLG_H__5CF3383C_BCA2_4991_8E3C_D00D460AB071__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputJgGDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputJgGDlg dialog

class CInputJgGDlg : public CDialog
{
// Construction
public:
	CInputJgGDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputJgGDlg)
	enum { IDD = IDD_INPUT_JG_G_DLG };
	int		m_nXG;
	int		m_nXG1;
	int		m_nXG2;
	int		m_nXG3;
	int		m_nYG;
	int		m_nYG1;
	int		m_nYG2;
	int		m_nYG3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputJgGDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputJgGDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTJGGDLG_H__5CF3383C_BCA2_4991_8E3C_D00D460AB071__INCLUDED_)
