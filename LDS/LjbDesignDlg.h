#if !defined(AFX_LJBDESIGNDLG_H__580BCB65_5760_4724_B89A_E289F7226B76__INCLUDED_)
#define AFX_LJBDESIGNDLG_H__580BCB65_5760_4724_B89A_E289F7226B76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LjbDesignDlg.h : header file
//
#ifndef __TSA_
#include "RememberLocationDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CLjbDesignDlg dialog

class CLjbDesignDlg : public CRememberLocationDlg
{
	void IntelliCalNormOffset();
// Construction
public:
	BOOL m_bEnableHuoQuPos;
	int jdb_style;
	int m_nFaceN;
	CLDSLineAngle *m_pBaseLineAngle;	//基准角钢
	BYTE base_x_wing0_y_wing1;
	bool reverseDirection;
	CLjbDesignDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLjbDesignDlg)
	enum { IDD = IDD_JDB_DESIGN_DLG };
	BOOL	m_bHuoQuPosByCtrlPt;
	int		m_iPlankMaterial;
	int		m_iRayJointStyle;
	int		m_nHuoQuDist;
	int		m_nHuoQuDist2;
	int		m_nOffsetZhun;
	int		m_nOffsetZhun2;
	CString	m_sPartNo;
	int		m_nPlankThick;
	int		m_nVertexDist;
	double	m_fNormOffset;
	int		m_iProfileType;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	CString m_sSegI;
	BOOL	m_bWeldLj;
	BOOL m_bIntersAtAngleOuter;
	BOOL m_bIntersAtAngleInner;
	int		m_nNormDirection;		//0.朝外 1.朝里
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLjbDesignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLjbDesignDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCmbRayJointStyle();
	afx_msg void OnBnPasteNorm();
	afx_msg void OnChangeEPartNo();
	afx_msg void OnWeldToDatumJg();
	afx_msg void OnChangePlankThick();
	afx_msg void OnBnClickedChkAtAngleOuter();
	afx_msg void OnBnClickedChkAtAngleInner();
	afx_msg void OnEnChangeENormOffset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LJBDESIGNDLG_H__580BCB65_5760_4724_B89A_E289F7226B76__INCLUDED_)
