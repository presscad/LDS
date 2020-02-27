#if !defined(AFX_WORKSTATUSDLG_H__4E471514_18A2_4069_8560_366850FA2391__INCLUDED_)
#define AFX_WORKSTATUSDLG_H__4E471514_18A2_4069_8560_366850FA2391__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkStatusDlg.h : header file
//
#include "SuperGridCtrl.h"
#include "Tower.h"
/////////////////////////////////////////////////////////////////////////////
// CWorkStatusDlg dialog
#if defined(__LDS_)||defined(__TSA_)

class CWorkStatusDlg : public CDialog
{
// Construction
public:
	void InitListCtrlItems();
	CWorkStatusDlg(CWnd* pParent = NULL);   // standard constructor
	CWorkStatus *m_pWorkStatus;

// Dialog Data
	//{{AFX_DATA(CWorkStatusDlg)
	enum { IDD = IDD_WORK_STATUS_DLG };
	CSuperGridCtrl	m_listCtrlLoadCoef;
	CString	m_sModuleScope;
	long	m_nWindAzimuth;
	CString	m_sStatusName;
	double	m_fIceThick;
	double	m_fWindVelocity;
	double	m_fWindLoadGrowCoefByIce;
	int		m_iMainLoadType;
	BOOL	m_bCheckWorkCond;
	CString	m_sVibrationModeStr;
	BYTE	m_cHoriEqAxisAzimuth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorkStatusDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeCmbMainLoadStyle();
	afx_msg void OnChkCheckWorkCond();
	afx_msg void OnChangeEIceThick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSTATUSDLG_H__4E471514_18A2_4069_8560_366850FA2391__INCLUDED_)
