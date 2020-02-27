#if !defined(AFX_WATERSHIELDEDPLATEDLG_H__7031BBC1_DE46_4F29_AB5F_74BC6124DD61__INCLUDED_)
#define AFX_WATERSHIELDEDPLATEDLG_H__7031BBC1_DE46_4F29_AB5F_74BC6124DD61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaterShieldedPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaterShieldedPlateDlg dialog

class CWaterShieldedPlateDlg : public CDialog
{
// Construction
public:
	CWaterShieldedPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaterShieldedPlateDlg)
	enum { IDD = IDD_WATER_SHIELDED_PLATE_DLG };
	int		m_nPlateThick;
	CString	m_sPartNo;
	int		m_iMaterial;
	double	m_fExternalDiameter;
	double	m_fInternalDiameter;
	double	m_fNormalOffset;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	CString m_sSegI;
	double	m_fWaterSlotWidth;
	double	m_fWaterSlotDepth;
	double	m_fWaterSlotLen;
	int		m_iDatumLine;
	double	m_fWaterSlotAngle;
	double	m_fOffsetAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaterShieldedPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaterShieldedPlateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPasteNorm();
	afx_msg void OnChangePartNo();
	afx_msg void OnChangeExternalDiameter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERSHIELDEDPLATEDLG_H__7031BBC1_DE46_4F29_AB5F_74BC6124DD61__INCLUDED_)
