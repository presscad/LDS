#if !defined(AFX_JOINTDESPARADLG_H__1EC9F49B_3BE8_40E3_ADA8_399279FB0402__INCLUDED_)
#define AFX_JOINTDESPARADLG_H__1EC9F49B_3BE8_40E3_ADA8_399279FB0402__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JointDesParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJointDesParaDlg dialog
#ifndef __TSA_
#include "RememberLocationDlg.h"
class CJointDesParaDlg : public CRememberLocationDlg
{
// Construction
	void UpdatePlankEFBerWidth(double fPlateWidth);
public:
	CBitmap m_CurJointBmp;
	BOOL IsValidJointStyle();
	int m_iBaseJgStyle;
	BOOL m_bHavePadPlank;
	CLDSLineAngle *m_pMainJg1,*m_pMainJg2;
	JGZJ base_jgzj1,base_jgzj2;
	LSSPACE_STRU LsSpace1,LsSpace2;
	//E、F板楞线侧距离,供外界调用,m_fPlankEFBerWidth用于界面显示,同一时刻E、F仅有一个需要楞线侧距离 wht 16-11-04
	double m_fPlankEBerWidth,m_fPlankFBerWidth;	
	CJointDesParaDlg(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CJointDesParaDlg)
	enum { IDD = IDD_JOINT_DESIGN_PARA_DLG };
	int		m_iMirMsg;
	int		m_iLsLayStyle;
	int		m_iLsOffsetStyle;
	long	m_nUpOffset;
	int		m_iJointStyle;
	CString	m_sJgAGuige;
	int		m_iJgAMat;
	CString	m_sJgBGuige;
	int		m_iJgBMat;
	CString	m_sJgCGuige;
	int		m_iJgCMat;
	CString	m_sJgDGuige;
	int		m_iJgDMat;
	int		m_iPlankEMat;
	int		m_iPlankFMat;
	int		m_iPlankGMat;
	int		m_iPlankHMat;
	double	m_fJgANormOffsetX;
	double	m_fJgANormOffsetY;
	CString	m_sPartNoA;
	CString	m_sPartNoB;
	CString	m_sPartNoC;
	CString	m_sPartNoD;
	CString	m_sPartNoE;
	CString	m_sPartNoF;
	CString	m_sPartNoG;
	CString	m_sPartNoH;
	double	m_fJgBNormOffsetX;
	double	m_fJgBNormOffsetY;
	double	m_fJgCNormOffsetX;
	double	m_fJgCNormOffsetY;
	double	m_fJgDNormOffsetX;
	double	m_fJgDNormOffsetY;
	int		m_nPlankThickE;
	int		m_nPlankWidthE;
	int		m_nPlankThickF;
	int		m_nPlankWidthF;
	double  m_fPlankEFBerWidth;
	int		m_nPlankThickG;
	int		m_nPlankWidthG;
	int		m_nPlankThickH;
	int		m_nPlankWidthH;
	BOOL	m_bAdjustWrapPlateLen;
	BOOL	m_bWeldingVertPlate;
	CString m_sASegI;
	CString m_sBSegI;
	CString m_sCSegI;
	CString m_sDSegI;
	CString m_sESegI;
	CString m_sFSegI;
	CString m_sGSegI;
	CString m_sHSegI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJointDesParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJointDesParaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbJointStyle();
	afx_msg void OnSelchangeCmbAJgMat();
	afx_msg void OnSelchangeCmbBJgMat();
	afx_msg void OnSelchangeCmbCJgMat();
	afx_msg void OnSelchangeCmbAJgGuige();
	afx_msg void OnSelchangeCmbBJgGuige();
	afx_msg void OnSelchangeCmbCJgGuige();
	afx_msg void OnSelchangeCmbEPlankMat();
	afx_msg void OnSelchangeCmbGPlankMat();
	afx_msg void OnChangeEEPlankThick();
	afx_msg void OnChangeEEPlankWide();
	afx_msg void OnChangeEFPlankWide();
	afx_msg void OnChangeEFBerWide();
	afx_msg void OnChangeEGPlankWide();
	afx_msg void OnChangeEGPlankThick();
	afx_msg void OnChangeAPartNo();
	afx_msg void OnChangeBPartNo();
	afx_msg void OnChangeCPartNo();
	afx_msg void OnChangeDPartNo();
	afx_msg void OnChangeEPartNo();
	afx_msg void OnChangeFPartNo();
	afx_msg void OnChangeGPartNo();
	afx_msg void OnChangeHPartNo();
	afx_msg void OnEnChangeUpoffset();
	afx_msg void OnBnClickedChkTWeldToBasePlate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOINTDESPARADLG_H__1EC9F49B_3BE8_40E3_ADA8_399279FB0402__INCLUDED_)
