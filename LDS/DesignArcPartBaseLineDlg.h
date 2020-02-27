#if !defined(AFX_DESIGNARCPARTBASELINEDLG_H__E6CF6965_ABD6_43E2_B180_B763403934E4__INCLUDED_)
#define AFX_DESIGNARCPARTBASELINEDLG_H__E6CF6965_ABD6_43E2_B180_B763403934E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignArcPartBaseLineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignArcPartBaseLineDlg dialog

class CDesignArcPartBaseLineDlg : public CDialog
{
// Construction
public:
	CDesignArcPartBaseLineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignArcPartBaseLineDlg)
	enum { IDD = IDD_DESIGN_ARC_PART_BASELINE };
	double	m_fWorkNormX;
	double	m_fWorkNormY;
	double	m_fWorkNormZ;
	double	m_fCenterX;
	double	m_fCenterY;
	double	m_fCenterZ;
	double	m_fStartPosX;
	double	m_fStartPosY;
	double	m_fStartPosZ;
	double	m_fStartAngle;
	double	m_fArcAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignArcPartBaseLineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignArcPartBaseLineDlg)
	afx_msg void OnBnPasteWorkNorm();
	afx_msg void OnBnPasteCenter();
	afx_msg void OnBnPasteStartPos();
	afx_msg void OnBnCopyWorkNorm();
	afx_msg void OnBnCopyCenter();
	afx_msg void OnBnCopyStartPos();
	afx_msg void OnChangeCenterPosZ();
	afx_msg void OnChangeStartPosZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNARCPARTBASELINEDLG_H__E6CF6965_ABD6_43E2_B180_B763403934E4__INCLUDED_)
