#if !defined(AFX_INPUTWORKNORMDLG_H__3DC26A0C_D379_469D_B198_83F750B6CE1B__INCLUDED_)
#define AFX_INPUTWORKNORMDLG_H__3DC26A0C_D379_469D_B198_83F750B6CE1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputWorkNormDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CInputWorkNormDlg dialog

class CInputWorkNormDlg : public CDialog
{
// Construction
public:
	CInputWorkNormDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputWorkNormDlg)
	enum { IDD = IDD_INPUT_WORK_NORM_DLG };
	BOOL	m_bDefaultWorkNorm;
	double	m_fNormX;
	double	m_fNormY;
	double	m_fNormZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputWorkNormDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputWorkNormDlg)
	afx_msg void OnBnPasteWorkNorm();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkDefaultWorkNorm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTWORKNORMDLG_H__3DC26A0C_D379_469D_B198_83F750B6CE1B__INCLUDED_)
