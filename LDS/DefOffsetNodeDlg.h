#if !defined(AFX_DEFOFFSETNODEDLG_H__78744B13_45C9_420D_A245_5E6E12B683B3__INCLUDED_)
#define AFX_DEFOFFSETNODEDLG_H__78744B13_45C9_420D_A245_5E6E12B683B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefOffsetNodeDlg.h : header file
//
#include "RememberLocationDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDefOffsetNodeDlg dialog

class CDefOffsetNodeDlg : public CRememberLocationDlg
{
// Construction
	CLDSPart *m_pPrevDatumPart;
	CLDSPart *m_pDatumPart;
public:
	void InitOffsetType(CLDSPart *pDatumPart);
	void AdjustWndPosition();
	BOOL m_bUpOffset;
	BOOL m_bDownOffset;
	MIRMSG mirmsg;
	CDefOffsetNodeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefOffsetNodeDlg)
	enum { IDD = IDD_DEF_OFFSET_NODE_DLG };
	double	m_fOriginX;
	double	m_fOriginY;
	double	m_fOriginZ;
	int		m_nArrayNum;
	BOOL	m_bMirAxisX;
	BOOL	m_bMirAxisY;
	BOOL	m_bMirAxisZ;
	BOOL	m_bMirRotation;
	double	m_fUpOffset;
	double	m_fDownOffset;
	BOOL	m_bMirAxisXYZ;
	short	m_nRotateAngle;
	BOOL	m_bAdvancedOption;
	BOOL	m_b45MirrorsSymmetry;
	BOOL    m_b135MirrorSymmetry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefOffsetNodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefOffsetNodeDlg)
	afx_msg void OnBnPasteOrigin();
	afx_msg void OnChkMirRotate();
	afx_msg void OnMirAxisXyz();
	afx_msg void OnChkAdvancedOption();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnChkMir45Plane();
	afx_msg void OnChkMir135Plane();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_iOffsetVec;
	BOOL m_bMultipleOffset;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFOFFSETNODEDLG_H__78744B13_45C9_420D_A245_5E6E12B683B3__INCLUDED_)
