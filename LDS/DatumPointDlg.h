#if !defined(AFX_DATUMPOINTDLG_H__E5320832_79AD_4AC1_AEFA_3B12C3B7E977__INCLUDED_)
#define AFX_DATUMPOINTDLG_H__E5320832_79AD_4AC1_AEFA_3B12C3B7E977__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DatumPointDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDatumPointDlg dialog

class CDatumPointDlg : public CDialog
{
	THANDLE m_hFirstHandle,m_hSecondHandle,m_hDatumPart;
	long g[2],g1[2],g2[2],g3[2];
// Construction
public:
	CLDSPoint *m_pPoint;
	void UpdateUIStatus();
	CDatumPointDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDatumPointDlg)
	enum { IDD = IDD_DATUM_POINT_DLG };
	CString	m_shDatumPoint;
	CString	m_sPointDescription;
	double	m_fPosX;
	double	m_fPosY;
	double	m_fPosZ;
	double	m_fPosXInPart;
	double	m_fPosYInPart;
	double	m_fPosZInPart;
	CString	m_shFirstHandle;
	CString	m_shSecondHandle;
	int		m_iDatumPosCalStyle;
	int		m_iSpecAxis;
	double	m_fExtendLen;
	double	m_fWingOffsetDist;
	int		m_iOffsetWing;
	int		m_iExtendDirection;
	CString	m_sFirstHandleName;
	CString	m_sSecondHandleName;
	int		m_iWingOffsetStyle;
	BOOL	m_bIncOddEffect;
	BOOL	m_bFaceOffset;
	int		m_iWingOffsetStyle2;
	double	m_fWingOffsetDist2;
	int		m_iOffsetFaceType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumPointDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatumPointDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbDatumPointCalStyle();
	virtual void OnOK();
	afx_msg void OnSelchangeCmbWingOffsetStyle();
	afx_msg void OnSelchangeCmbSpecAxis();
	afx_msg void OnBnNormOffset();
	afx_msg void OnSelchangeCmbWingOffsetStyle2();
	afx_msg void OnChkFaceOffset();
	afx_msg void OnBnPasteVec();
	afx_msg void OnBnPastePosInPart();
	afx_msg void OnBnCopyPosInPart();
	afx_msg void OnBnCopyVec();
	afx_msg void OnSelchangeCmbOffsetFaceType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMPOINTDLG_H__E5320832_79AD_4AC1_AEFA_3B12C3B7E977__INCLUDED_)
