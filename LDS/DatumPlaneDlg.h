#if !defined(AFX_DATUMPLANEDLG_H__BC5DA2C3_C45B_4750_AB8A_951F06E713AA__INCLUDED_)
#define AFX_DATUMPLANEDLG_H__BC5DA2C3_C45B_4750_AB8A_951F06E713AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DatumPlaneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDatumPlaneDlg dialog

class CDatumPlaneDlg : public CDialog
{
// Construction
public:
	CDatumPlaneDlg(CWnd* pParent = NULL);   // standard constructor
	THANDLE m_hFirstPoint,m_hSecondPoint,m_hThirdPoint;

// Dialog Data
	//{{AFX_DATA(CDatumPlaneDlg)
	enum { IDD = IDD_DATUM_PLANE_DLG };
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	CString	m_sPlaneDescription;
	CString	m_sFirPointDescription;
	CString	m_sSecPointDescription;
	CString	m_sThirPointDescription;
	CString	m_shDatumPlane;
	int		m_iFirstPoint;
	int		m_iSecPoint;
	int		m_iThirdPoint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumPlaneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatumPlaneDlg)
	afx_msg void OnBnUpdatePlane();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopyPlane();
	afx_msg void OnBnCopyNorm();
	afx_msg void OnSelchangeCmbHDatumPointFir();
	afx_msg void OnSelchangeCmbHDatumPointSec();
	afx_msg void OnSelchangeCmbHDatumPointThir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMPLANEDLG_H__BC5DA2C3_C45B_4750_AB8A_951F06E713AA__INCLUDED_)
