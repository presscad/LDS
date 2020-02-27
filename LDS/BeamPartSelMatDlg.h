#if !defined(AFX_BEAMPARTSELMATDLG_H__1120D11D_0548_4D0C_A2AB_D760836E4D16__INCLUDED_)
#define AFX_BEAMPARTSELMATDLG_H__1120D11D_0548_4D0C_A2AB_D760836E4D16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BeamPartSelMatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBeamPartSelMatDlg dialog
#ifdef __PART_DESIGN_INC_
class CBeamPartSelMatDlg : public CDialog
{
// Construction
public:
	CBeamPartSelMatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBeamPartSelMatDlg)
	enum { IDD = IDD_BEAM_PART_SEL_MAT_DLG };
	CString	m_sLsHoleDiameter;
	int		m_iMat;
	CString	m_sPartSpec;
	double	m_fAxisLoadForce;
	double	m_fDeductLsHoleCoef;
	double	m_fSimpleBeamLength;
	double	m_fVerticalLoadForce;
	double	m_fVerticalLoadPosition;
	int		m_iGyrationStyle;
	CString	m_sCheckResult;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeamPartSelMatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBeamPartSelMatDlg)
	afx_msg void OnBnCheckPart();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEAxisLoadForce();
	afx_msg void OnChangeESimpleBeamLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEAMPARTSELMATDLG_H__1120D11D_0548_4D0C_A2AB_D760836E4D16__INCLUDED_)
