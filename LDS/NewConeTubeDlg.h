#if !defined(AFX_NEWCONETUBEDLG_H__1F4D9A42_318F_41C8_8F43_84010B77024A__INCLUDED_)
#define AFX_NEWCONETUBEDLG_H__1F4D9A42_318F_41C8_8F43_84010B77024A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewConeTubeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewConeTubeDlg dialog

class CNewConeTubeDlg : public CDialog
{
// Construction
public:
	CNewConeTubeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewConeTubeDlg)
	enum { IDD = IDD_NEW_CONETUBE_DLG };
	CEdit	m_editSideOpen;
	CEdit	m_editFrontOpen;
	CComboBox	m_cmbMaterial;
	CComboBox	m_cmbType;
	CString	m_sType;
	double	m_fDownRadius;
	double	m_fUpRadius;
	int		m_nFaceNum;
	double	m_fFrontOpen;
	double	m_fLongth;
	double	m_fThick;
	double	m_fX;
	double	m_fY;
	double	m_fZ;
	CString	m_sMaterial;
	double	m_fSideOpen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewConeTubeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewConeTubeDlg)
	afx_msg void OnSelchangeConetubeType();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCONETUBEDLG_H__1F4D9A42_318F_41C8_8F43_84010B77024A__INCLUDED_)
