#if !defined(AFX_CCIRCULARPLATEDLG_H__70719D38_33C3_4019_B449_41F9312C739A__INCLUDED_)
#define AFX_CCIRCULARPLATEDLG_H__70719D38_33C3_4019_B449_41F9312C739A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCircularPlateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCircularPlateDlg dialog

class CCircularPlateDlg : public CDialog
{
// Construction
public:
	void RefreshDlgCtrlState();
	int	 m_iDlgType;	//0.环向板 1.封头板 2.流水板
	CCircularPlateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCircularPlateDlg)
	enum { IDD = IDD_CIRCULAR_PLATE_DLG };
	int		m_nPlateThick;
	CString	m_sPartNo;
	double	m_fStartAngle;
	double	m_fSectorAngle;
	int		m_iMaterial;
	double	m_fExternalDiameter;
	double	m_fInternalDiameter;
	double	m_fNormalOffset;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	CString m_sSegI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCircularPlateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCircularPlateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPasteNorm();
	afx_msg void OnChangeEPartNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEALENDPLATEDLG_H__70719D38_33C3_4019_B449_41F9312C739A__INCLUDED_)
