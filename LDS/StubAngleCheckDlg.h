#if !defined(AFX_STUBANGLECHECKDLG_H__47F29C94_E73B_4B82_B8F1_8C839D2528E1__INCLUDED_)
#define AFX_STUBANGLECHECKDLG_H__47F29C94_E73B_4B82_B8F1_8C839D2528E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StubAngleCheckDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStubAngleCheckDlg dialog
#ifdef __PART_DESIGN_INC_
class CStubAngleCheckDlg : public CDialog
{
// Construction
public:
	CStubAngleCheckDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStubAngleCheckDlg)
	enum { IDD = IDD_STUB_ANGLE_CHECK_DLG };
	int		m_iBoltD;
	int		m_iBoltGrade;
	double	m_fBoltCanCompForce;
	double	m_fBoltCanShearForce;
	int		m_nBoltN;
	double	m_fMinStubDepth;
	double	m_fMinStubLength;
	double	m_fForceN;
	double	m_fShearForce;
	CString	m_sCalResult;
	double	m_fStubAngleHoleOut;
	CString	m_sStubAngleSize;
	CString	m_sAnchorAngleSize;
	double	m_fAnchorAngleLength;
	int		m_nAnchorAngleN;
	int		m_iConcreteType;
	int		m_iAnchorMat;
	int		m_iStubMat;
	double	m_fConcreteFc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStubAngleCheckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStubAngleCheckDlg)
	afx_msg void OnSelchangeCmbBoltGrade();
	afx_msg void OnSelchangeCmbBoltD();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCalCheck();
	afx_msg void OnSelchangeCmbAngleSize();
	afx_msg void OnSelchangeCmbStubAngleSize();
	afx_msg void OnSelchangeCmbConcreteType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUBANGLECHECKDLG_H__47F29C94_E73B_4B82_B8F1_8C839D2528E1__INCLUDED_)
