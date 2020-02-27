#if !defined(AFX_SETNCDRVDLG_H__B059E623_79B9_4029_870E_637602350D85__INCLUDED_)
#define AFX_SETNCDRVDLG_H__B059E623_79B9_4029_870E_637602350D85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetNcDrvDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetNcDrvDlg dialog

class CSetNcDrvDlg : public CDialog
{
// Construction
public:
	CSetNcDrvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetNcDrvDlg)
	enum { IDD = IDD_SET_NC_DRIVER_DLG };
	CString	m_sJgNcDrv;
	CString	m_sOutPutPath;
	CString	m_sPartNoPrefix;
	int m_iNcGroupType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetNcDrvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetNcDrvDlg)
	afx_msg void OnBnFindJgNcDrv();
	afx_msg void OnBnSetNcOutputPath();
	afx_msg void OnCbnSelchangeCmbNcGroupType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETNCDRVDLG_H__B059E623_79B9_4029_870E_637602350D85__INCLUDED_)
