#if !defined(AFX_ANGLEPROFILEPARADLG_H__F1152DC5_F828_400B_8F9C_6F95B0FE5D02__INCLUDED_)
#define AFX_ANGLEPROFILEPARADLG_H__F1152DC5_F828_400B_8F9C_6F95B0FE5D02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AngleProfileParaDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CAngleProfileParaDlg dialog
#include "RememberLocationDlg.h"

class CAngleProfileParaDlg : public CRememberLocationDlg
{
// Construction
	void IntelliUpdateBoltSpace();
public:
	int x_wing0_y_wing1;
	CLDSLinePart *m_pRod;
	CHashSet<CLDSBolt*> *m_pBoltSet;
	CAngleProfileParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAngleProfileParaDlg)
	enum { IDD = IDD_ANGLE_PROFILE_PARA_DLG };
	int		m_nG;
	int		m_nG1;
	int		m_nG2;
	double	m_fBerSpace;
	double	m_fWingSpace;
	BOOL	m_bStdProfile;
	CString	m_sRayAngleSize;
	BOOL	m_bTwoEdgeProfile;
	int		m_iBerSpaceMode;
	int		m_iWingSpaceMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAngleProfileParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAngleProfileParaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkStdProfile();
	afx_msg void OnChangeWingSpace();
	afx_msg void OnChangeBerSpace();
	afx_msg void OnCbnSelchangeBerSpaceMode();
	afx_msg void OnCbnSelchangeWingSpaceMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANGLEPROFILEPARADLG_H__F1152DC5_F828_400B_8F9C_6F95B0FE5D02__INCLUDED_)
