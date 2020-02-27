#if !defined(AFX_PLATEBASICPARADLG_H__DEDF3D3B_60DE_46AE_A51C_60D6A08F8F12__INCLUDED_)
#define AFX_PLATEBASICPARADLG_H__DEDF3D3B_60DE_46AE_A51C_60D6A08F8F12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlateBasicParaDlg.h : header file
//
#include "RememberLocationDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CPlateBasicParaDlg dialog

class CPlateBasicParaDlg : public CRememberLocationDlg
{
// Construction
public:
	BOOL m_bEnableWeld;
	CPlateBasicParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlateBasicParaDlg)
	enum { IDD = IDD_PLATE_BASIC_PARA_DLG };
	CString	m_sPartNo;
	int		m_nPlankThick;
	int		m_iMaterial;
	CString	m_sSegI;
	int		m_iProfileType;
	int		m_nWeldWidth;
	BOOL	m_bAdjustWeldLen;
	double  m_fEdgeSpace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlateBasicParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlateBasicParaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEPartNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATEBASICPARADLG_H__DEDF3D3B_60DE_46AE_A51C_60D6A08F8F12__INCLUDED_)
