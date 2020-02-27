#if !defined(AFX_DIMSIMPOLYCFGDLG_H__C2C9BC8F_D0C8_42BC_80F9_B7DBF045D8C4__INCLUDED_)
#define AFX_DIMSIMPOLYCFGDLG_H__C2C9BC8F_D0C8_42BC_80F9_B7DBF045D8C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DimSimPolyCfgDlg.h : header file
//
#include "f_ent.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDimSimPolyCfgDlg dialog

class CDimSimPolyCfgDlg : public CDialog
{
// Construction
public:
	CDimSimPolyCfgDlg(CWnd* pParent = NULL);   // standard constructor
	UCS_STRU base_face_ucs;

// Dialog Data
	//{{AFX_DATA(CDimSimPolyCfgDlg)
	enum { IDD = IDD_DIM_SIMPOLY_CFG_DLG };
	BOOL	m_bDimHuoQuFace;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDimSimPolyCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDimSimPolyCfgDlg)
	afx_msg void OnBnMapDrawUcs();
	afx_msg void OnBnPasteWorkNorm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMSIMPOLYCFGDLG_H__C2C9BC8F_D0C8_42BC_80F9_B7DBF045D8C4__INCLUDED_)
