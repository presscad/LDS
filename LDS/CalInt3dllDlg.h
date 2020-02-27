#if !defined(AFX_CALINT3DLLDLG_H__0A421208_80BD_489B_8CD2_2F01D0131EC8__INCLUDED_)
#define AFX_CALINT3DLLDLG_H__0A421208_80BD_489B_8CD2_2F01D0131EC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalInt3dllDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalInt3dllDlg dialog

class CCalInt3dllDlg : public CDialog
{
// Construction
public:
	CCalInt3dllDlg(CWnd* pWnd);   // standard constructor
	CWnd * m_pWnd;
	BOOL Create();

// Dialog Data
	//{{AFX_DATA(CCalInt3dllDlg)
	enum { IDD = IDD_INT3D_LL_DLG };
	double	m_fEndX1;
	double	m_fEndX2;
	double	m_fEndY1;
	double	m_fEndY2;
	double	m_fEndZ1;
	double	m_fEndZ2;
	double	m_fIntersX;
	double	m_fIntersY;
	double	m_fIntersZ;
	double	m_fStartX1;
	double	m_fStartX2;
	double	m_fStartY1;
	double	m_fStartY2;
	double	m_fStartZ1;
	double	m_fStartZ2;
	CString	m_sResultReport;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalInt3dllDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalInt3dllDlg)
	afx_msg void OnCalInters();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnPasteStart1();
	afx_msg void OnBnPasteStart2();
	afx_msg void OnBnPasteEnd1();
	afx_msg void OnBnPasteEnd2();
	afx_msg void OnCopyIntersPt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALINT3DLLDLG_H__0A421208_80BD_489B_8CD2_2F01D0131EC8__INCLUDED_)
