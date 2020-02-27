#if !defined(AFX_ADDLEIBANDLG_H__CFE7CDD3_171D_4129_B5D0_210B9616D197__INCLUDED_)
#define AFX_ADDLEIBANDLG_H__CFE7CDD3_171D_4129_B5D0_210B9616D197__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddLeiBanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddLeiBanDlg dialog

class CAddLeiBanDlg : public CDialog
{
// Construction
public:
	CAddLeiBanDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bCircleDiBan;
	CString m_sCaption;
// Dialog Data
	//{{AFX_DATA(CAddLeiBanDlg)
	enum { IDD = IDD_ADD_LEI_BAN_DLG };
	int		m_iHigh;
	int		m_iThick;
	CString	m_sPartNo;
	int		m_nParaA;
	int		m_nParaB;
	int		m_iMaterial;
	double	m_fGradientAngle;
	double	m_fLeiBanWide;
	double	m_fLeiBanRayDist;
	double	m_fWaterSlotWide;
	CString	m_sDatumXueBan;
	BOOL	m_bEnableSecondXueBan;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddLeiBanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddLeiBanDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkEnableSecXueban();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDLEIBANDLG_H__CFE7CDD3_171D_4129_B5D0_210B9616D197__INCLUDED_)
