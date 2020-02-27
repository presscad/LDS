#if !defined(AFX_DESLSWORKNORMDLG_H__C5743ABB_B443_4A6A_8BD3_EC2A172D556B__INCLUDED_)
#define AFX_DESLSWORKNORMDLG_H__C5743ABB_B443_4A6A_8BD3_EC2A172D556B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignVectorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignVectorDlg dialog

class CDesignVectorDlg : public CDialog
{
// Construction
public:
	CDesignVectorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignVectorDlg)
	enum { IDD = IDD_DESIGN_VECTOR_DLG };
	CString	m_hBasePart;
	CString	m_hCrossPart;
	int		m_iBaseJgWing;
	int		m_iDirection;
	double	m_fVecX;
	double	m_fVecY;
	double	m_fVecZ;
	double	m_fNearVecX;
	double	m_fNearVecY;
	double	m_fNearVecZ;
	int		m_iNormStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignVectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignVectorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopy();
	afx_msg void OnBnPaste();
	afx_msg void OnRdoNormStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESLSWORKNORMDLG_H__C5743ABB_B443_4A6A_8BD3_EC2A172D556B__INCLUDED_)
