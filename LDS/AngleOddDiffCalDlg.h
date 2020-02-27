#if !defined(AFX_ANGLEODDDIFFCALDLG_H__F2DA5CB1_820A_4CED_8F4A_141802FFC125__INCLUDED_)
#define AFX_ANGLEODDDIFFCALDLG_H__F2DA5CB1_820A_4CED_8F4A_141802FFC125__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AngleOddDiffCalDlg.h : header file
//
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAngleOddDiffCalDlg dialog

class CAngleOddDiffCalDlg : public CDialog
{
// Construction
public:
	CLDSLineAngle *m_pBaseJg;
	CAngleOddDiffCalDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAngleOddDiffCalDlg)
	enum { IDD = IDD_ODDMENT_DIFF_CAL_DLG };
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	double	m_fWingXG;
	double	m_fWingYG;
	CString	m_sOddX;
	CString	m_sOddY;
	CString	m_hAngle;
	CString	m_sAngleSpec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAngleOddDiffCalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAngleOddDiffCalDlg)
	afx_msg void OnBnCalOddment();
	afx_msg void OnBnPaste();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANGLEODDDIFFCALDLG_H__F2DA5CB1_820A_4CED_8F4A_141802FFC125__INCLUDED_)
