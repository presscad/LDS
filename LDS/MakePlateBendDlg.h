#if !defined(AFX_MAKEPLATEBENDDLG_H__353047A1_7437_4C0B_B8E5_2F10FCF7AD54__INCLUDED_)
#define AFX_MAKEPLATEBENDDLG_H__353047A1_7437_4C0B_B8E5_2F10FCF7AD54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MakePlateBendDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMakePlateBendDlg dialog
#ifndef __TSA_
#include "RememberLocationDlg.h"
class CMakePlateBendDlg : public CRememberLocationDlg
{
// Construction
	CBitmap m_curBitmap1,m_curBitmap2;
public:
	CMakePlateBendDlg(CWnd* pParent = NULL);   // standard constructor
	CLDSPlate *m_pPlate;
	f3dPoint huoqu_norm,huoqu_pos;
	f3dPoint datum_plane_pos,datum_plane_norm;
// Dialog Data
	//{{AFX_DATA(CMakePlateBendDlg)
	enum { IDD = IDD_MAKE_PLATE_BEND_DLG };
	double	m_fBaseX;
	double	m_fBaseY;
	double	m_fBaseZ;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	int		m_iHuoquType;
	//}}AFX_DATA`


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakePlateBendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMakePlateBendDlg)
	afx_msg void OnPastePlane();
	afx_msg void OnPasteNorm();
	afx_msg void OnPasteBase();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancle();
	virtual void OnOK();
	afx_msg void OnReverseBase();
	afx_msg void OnReverseNorm();
	afx_msg void OnReverseBaseNorm();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKEPLATEBENDDLG_H__353047A1_7437_4C0B_B8E5_2F10FCF7AD54__INCLUDED_)
