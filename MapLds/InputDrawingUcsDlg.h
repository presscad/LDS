#if !defined(AFX_INPUTDRAWINGUCSDLG_H__4F3CD450_227A_46C5_88FE_566123C561CD__INCLUDED_)
#define AFX_INPUTDRAWINGUCSDLG_H__4F3CD450_227A_46C5_88FE_566123C561CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDrawingUcsDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CInputDrawingUcsDlg dialog

class CInputDrawingUcsDlg : public CDialog
{
// Construction
public:
	CInputDrawingUcsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputDrawingUcsDlg)
	enum { IDD = IDD_INPUT_DRAWING_UCS_DLG };
	double	m_fAxisX_X;
	double	m_fAxisX_Y;
	double	m_fAxisX_Z;
	double	m_fAxisZ_X;
	double	m_fAxisZ_Y;
	double	m_fAxisZ_Z;
	double	m_fOriginPosX;
	double	m_fOriginPosY;
	double	m_fOriginPosZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDrawingUcsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDrawingUcsDlg)
	afx_msg void OnBnPasteAxisX();
	afx_msg void OnBnPasteAxisZ();
	afx_msg void OnBnPasteOriginPos();
	afx_msg void OnBnCopyAxisX();
	afx_msg void OnBnCopyAxisZ();
	afx_msg void OnBnCopyOriginPos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDRAWINGUCSDLG_H__4F3CD450_227A_46C5_88FE_566123C561CD__INCLUDED_)
