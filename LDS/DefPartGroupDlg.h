#if !defined(AFX_DEFPARTGROUPDLG_H__5DFE0C22_E13A_41BA_A84C_0978B88BCCDB__INCLUDED_)
#define AFX_DEFPARTGROUPDLG_H__5DFE0C22_E13A_41BA_A84C_0978B88BCCDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefPartGroupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefPartGroupDlg dialog

class CDefPartGroupDlg : public CDialog
{
// Construction
public:
	CDefPartGroupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefPartGroupDlg)
	enum { IDD = IDD_DEF_PARTGROUP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefPartGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefPartGroupDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFPARTGROUPDLG_H__5DFE0C22_E13A_41BA_A84C_0978B88BCCDB__INCLUDED_)
