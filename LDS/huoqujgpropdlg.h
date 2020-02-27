#if !defined(AFX_HUOQUJGPROP_H__7E8ECE31_3E72_4AC8_BD12_6145FD14B34A__INCLUDED_)
#define AFX_HUOQUJGPROP_H__7E8ECE31_3E72_4AC8_BD12_6145FD14B34A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HuoQuJgPropDlg.h : header file
//
#include "tma_co_part.h"
/////////////////////////////////////////////////////////////////////////////
// CHuoQuJgPropDlg dialog

class CHuoQuJgPropDlg : public CDialog
{
// Construction
public:
	CLDSLineAngle* m_pNextJg;
	CLDSLineAngle* m_pStartJg;
	CLDSPolyAngle* m_pHuoQuJg;
	CHuoQuJgPropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHuoQuJgPropDlg)
	enum { IDD = IDD_HUOQU_JG_PROP_DLG };
	double	m_fHuoQuR;
	int		m_iSegI;
	CString	m_sJgGuige;
	int		m_iJgMat;
	CString	m_sCfgPartNo;
	CString	m_sNotes;
	CString	m_sPartNo;
	double	m_fRealLen;
	UINT	m_uStatMatCoef;
	int		m_iStatMatNo;
	CString	m_hNextJg;
	CString	m_hStartJg;
	CString	m_hHuoQuJgHandle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHuoQuJgPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHuoQuJgPropDlg)
	afx_msg void OnBnCfgPartNo();
	afx_msg void OnBnHeadHuoquJoint();
	afx_msg void OnBnNextHuoquJoint();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeESegJgHuoquR();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void UpdateCfgPartNoString();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HUOQUJGPROP_H__7E8ECE31_3E72_4AC8_BD12_6145FD14B34A__INCLUDED_)
