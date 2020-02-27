#if !defined(AFX_CALWINDBETAZDLG_H__516DFAB9_6370_4A66_88C6_75189B8E02BF__INCLUDED_)
#define AFX_CALWINDBETAZDLG_H__516DFAB9_6370_4A66_88C6_75189B8E02BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if defined(__TSA_)||defined(__LDS_)
// CalWindBetaZDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalWindBetaZDlg dialog

class CCalWindBetaZDlg : public CDialog
{
// Construction
public:
	void CalBetaZ();
	void RefreshDispStatus(bool bUpdateChkBox=false);
	CCalWindBetaZDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalWindBetaZDlg)
	enum { IDD = IDD_CAL_WIND_BETAZ_DLG };
	BOOL	m_bSpecWindComp;
	double	m_fBaseVibrationPeriod;
	double	m_fBottomWidth;
	double	m_fBaseWindComp;
	double	m_fSegHighPos;
	double	m_fSegWidth;
	double	m_fTopWidth;
	double	m_fTotalHigh;
	double	m_fWindBetaZ;
	double	m_fWindVelocity;
	int		m_iTowerSupportType;
	int		m_iBetaZCalType;
	double	m_fWindBetaZForFoundation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalWindBetaZDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalWindBetaZDlg)
	afx_msg void OnChangeETotalHigh();
	afx_msg void OnChangeEBaseVibrationPeriod();
	afx_msg void OnChangeESegHighPos();
	afx_msg void OnChangeESegWidth();
	afx_msg void OnChangeETopWidth();
	afx_msg void OnChangeEBottomWidth();
	afx_msg void OnChangeEWindVelocity();
	afx_msg void OnChangeEBaseWindComp();
	afx_msg void OnChkSpecWindComp();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdoTowerType();
	afx_msg void OnRdoBetaZCalType();
	afx_msg void OnChkSpecWindVelocity();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALWINDBETAZDLG_H__516DFAB9_6370_4A66_88C6_75189B8E02BF__INCLUDED_)
