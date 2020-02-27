#if !defined(AFX_LINKLDSANGLEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_)
#define AFX_LINKLDSANGLEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkLdsAngleDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CLinkLdsAngleDlg dialog

class CLinkLdsAngleDlg : public CDialog
{
// Construction
public:
	double m_fFillThick;
	int group_style;
	int m_nViewFlag;
	CLinkLdsAngleDlg(CWnd* pParent = NULL);   // standard constructor
	char sPartNoArr[4][4][16];	//[...][][]对称位置;[][...][]子角钢序列号
	MIRMSG mirmsg;
	int xy1_yz2_xz3;	//1-XY对称重复 2-YZ对称重复 3-XZ对称重复 其余表示无重复对称
	CLDSNode *m_pStartNode;
	CLDSNode *m_pEndNode;

// Dialog Data
	//{{AFX_DATA(CLinkLdsAngleDlg)
	enum { IDD = IDD_LINK_LDS_ANGLE_DLG };
	int		m_nJgMaterial;
	int		m_nStartLsNum;
	int		m_nEndLsNum;
	CString	m_sEndLsGuiGe;
	CString	m_sStartLsGuiGe;
	BOOL	m_bMirSideFace;
	CString	m_sOriginPartNo;
	CString	m_sMirXOriginPartNo;
	CString	m_sMirYOriginPartNo;
	CString	m_sMirDiagOriginPartNo;
	CString	m_sJgGuiGe;
	BOOL	m_bGroupJg;
	BOOL	m_bStartHoriOffset;
	BOOL	m_bEndHoriOffset;
	BOOL	m_bStartToBer;
	BOOL	m_bEndToBer;
	int		m_iSegNo;
	int		m_iAngleGradeType;
	int		m_iAngleLocation;
	UINT	m_uStatMatNo;
	BOOL	m_bSelSize;
	BOOL	m_bAnalysisInc;
	int		m_iStartForceType;
	int		m_iStartJointType;
	int		m_iEndForceType;
	int		m_iEndJointType;
	int		m_iCalLenType;
	double	m_fLsHoleOut;
	BOOL	m_bMirAxisXYZ;
	BOOL	m_bMirAxisX;
	BOOL	m_bMirAxisY;
	BOOL	m_bMirAxisZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkLdsAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkLdsAngleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void OnSelchangeCmbJgGuiege();
	afx_msg void OnChangeEOriginPartNo();
	afx_msg void OnChangeEditStartLsNum();
	afx_msg void OnChangeEditEndLsNum();
	afx_msg void OnChangeCmbStartLsGuiege();
	afx_msg void OnChangeEMirXOriginPartNo();
	afx_msg void OnBnGroupStyle();
	afx_msg void OnChkGroupJg();
	afx_msg void OnBnNewStatMatNo();
	afx_msg void OnSelchangeCmbStartForceType();
	afx_msg void OnMirAxisXyz();
	afx_msg void OnMirAxisX();
	afx_msg void OnMirAxisY();
	afx_msg void OnMirAxisZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKLDSANGLEDLG_H__3B700A5B_63A8_4147_84F6_20E60BB8237C__INCLUDED_)
