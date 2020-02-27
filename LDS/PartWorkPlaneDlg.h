#if !defined(AFX_PARTWORKPLANEDLG_H__5A18F45A_C231_4C37_A287_D58DEE598ABC__INCLUDED_)
#define AFX_PARTWORKPLANEDLG_H__5A18F45A_C231_4C37_A287_D58DEE598ABC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartWorkPlaneDlg.h : header file
//
#ifndef __TSA_

/////////////////////////////////////////////////////////////////////////////
// CPartWorkPlaneDlg dialog

class CPartWorkPlaneDlg : public CDialog
{
// Construction
	void RefreshDlgCtrl();
	void UpdateDatumPoint(BOOL bInit=FALSE);
public:
	CPartWorkPlaneDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPart *m_pDatumPart;
	BOOL m_bStart;
// Dialog Data
	//{{AFX_DATA(CPartWorkPlaneDlg)
	enum { IDD = IDD_PART_WORK_PLANE_DLG };
	CComboBox	m_cmbDatumPosTyoe;
	CComboBox	m_cmbNormType;
	double	m_fBaseX;
	double	m_fBaseY;
	double	m_fBaseZ;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	int		m_iDatumPosType;
	int		m_iNormType;
	double	m_fOffsetDist;
	double	m_fOffsetAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartWorkPlaneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartWorkPlaneDlg)
	afx_msg void OnCopyBase();
	afx_msg void OnCopyNorm();
	afx_msg void OnPasteBase();
	afx_msg void OnPasteNorm();
	afx_msg void OnCopyPlane();
	afx_msg void OnChangeOffsetDist();
	afx_msg void OnChangeOffsetAngle();
	afx_msg void OnSelChangleNormType();
	afx_msg void OnSelChangleDatumPosType();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTWORKPLANEDLG_H__5A18F45A_C231_4C37_A287_D58DEE598ABC__INCLUDED_)
