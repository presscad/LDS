#if !defined(AFX_DESIGNJGWINGNORM_H__8BF3A3BD_D79D_4A8B_B290_3825515E9392__INCLUDED_)
#define AFX_DESIGNJGWINGNORM_H__8BF3A3BD_D79D_4A8B_B290_3825515E9392__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignJgWingNorm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignJgWingNorm dialog

class CDesignJgWingNorm : public CDialog
{
// Construction
public:
	CDesignJgWingNorm(CWnd* pParent = NULL);   // standard constructor
	CLDSVector spec_norm;

// Dialog Data
	//{{AFX_DATA(CDesignJgWingNorm)
	enum { IDD = IDD_JG_WING_NORM_DESIGN_DLG };
	BOOL	m_bCalByOtherWing;
	BOOL	m_bSpecificNorm;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	CString	m_hRefJg;
	double	m_fNearNormX;
	double	m_fNearNormY;
	double	m_fNearNormZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignJgWingNorm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignJgWingNorm)
	virtual void OnOK();
	afx_msg void OnBnDesJgWingNorm();
	virtual BOOL OnInitDialog();
	afx_msg void RefreshDisplayStatus();
	afx_msg void OnBnCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNJGWINGNORM_H__8BF3A3BD_D79D_4A8B_B290_3825515E9392__INCLUDED_)
