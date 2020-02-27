#if !defined(AFX_ADDDIANBANDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_)
#define AFX_ADDDIANBANDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddDianBanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddDianBanDlg dialog

class CAddDianBanDlg : public CDialog
{
// Construction
public:
	CAddDianBanDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddDianBanDlg)
	enum { IDD = IDD_ADD_DIAN_BAN_DLG };
	long	m_nDianBanThick;
	CString	m_sDianBanNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddDianBanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddDianBanDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDDIANBANDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_)
