#if !defined(AFX_DEFSCALENODEDLG_H__329255FB_F679_4CD5_9878_E2C883A03230__INCLUDED_)
#define AFX_DEFSCALENODEDLG_H__329255FB_F679_4CD5_9878_E2C883A03230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefScaleNodeDlg.h : header file
//
#include "RememberLocationDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDefScaleNodeDlg dialog

class CDefScaleNodeDlg : public CRememberLocationDlg
{
// Construction
	void AdjustWndPosition();
public:
	MIRMSG mirmsg;
	CDefScaleNodeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefScaleNodeDlg)
	enum { IDD = IDD_DEF_SCALE_NODE_DLG };
	double	m_fOriginX;
	double	m_fOriginY;
	double	m_fOriginZ;
	int		m_nArrayNum;
	BOOL	m_bMirAxisX;
	BOOL	m_bMirAxisY;
	BOOL	m_bMirAxisZ;
	BOOL	m_bMirRotation;
	int		m_nDivideByN;
	BOOL	m_bMirAxisXYZ;
	short	m_nRotateAngle;
	BOOL	m_bAdvancedOption;
	BOOL	m_b45MirrorsSymmetry;
	BOOL    m_b135MirrorSymmetry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefScaleNodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefScaleNodeDlg)
	afx_msg void OnBnPasteOrigin();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkMirRotate();
	afx_msg void OnMirAxisXyz();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnAdvancedOption();
	afx_msg void OnChkMir45Plane();
	afx_msg void OnChkMir135Plane();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFSCALENODEDLG_H__329255FB_F679_4CD5_9878_E2C883A03230__INCLUDED_)
