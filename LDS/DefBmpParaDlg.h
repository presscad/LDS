#if !defined(AFX_DEFBMPPARADLG_H__7649C24D_CDF4_478D_8670_F59DABB1631C__INCLUDED_)
#define AFX_DEFBMPPARADLG_H__7649C24D_CDF4_478D_8670_F59DABB1631C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefBmpParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefBmpParaDlg dialog

class CDefBmpParaDlg : public CDialog
{
// Construction
public:
	CDefBmpParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefBmpParaDlg)
	enum { IDD = IDD_INPUT_BMP_PARA_DLG };
	long	m_nBmpHeight;
	long	m_nBmpWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefBmpParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefBmpParaDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFBMPPARADLG_H__7649C24D_CDF4_478D_8670_F59DABB1631C__INCLUDED_)
