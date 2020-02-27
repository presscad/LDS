#if !defined(AFX_DEFPLANEDLG_H__A599FCB3_3916_4BF1_BA7C_7BDA53531477__INCLUDED_)
#define AFX_DEFPLANEDLG_H__A599FCB3_3916_4BF1_BA7C_7BDA53531477__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefPlaneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefPlaneDlg dialog

class CDefPlaneDlg : public CDialog
{
// Construction
public:
	CString m_sDlgCaption;
	CDefPlaneDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bReadOnly;

// Dialog Data
	//{{AFX_DATA(CDefPlaneDlg)
	enum { IDD = IDD_DEF_PLANE_DLG };
	double	m_fBaseX;
	double	m_fBaseY;
	double	m_fBaseZ;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefPlaneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefPlaneDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopyBase();
	afx_msg void OnBnCopyNorm();
	afx_msg void OnBnPasteBase();
	afx_msg void OnBnPasteNorm();
	afx_msg void OnCopyPlane();
	afx_msg void OnPastePlane();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFPLANEDLG_H__A599FCB3_3916_4BF1_BA7C_7BDA53531477__INCLUDED_)
