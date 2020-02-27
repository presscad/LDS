#if !defined(AFX_AXISVALFIXNODEDLG_H__FCB4A15E_FAC5_4BF1_B91E_6C06E636A86B__INCLUDED_)
#define AFX_AXISVALFIXNODEDLG_H__FCB4A15E_FAC5_4BF1_B91E_6C06E636A86B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AxisValFixNodeDlg.h : header file
//
#include "RememberLocationDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CAxisValFixNodeDlg dialog

class CAxisValFixNodeDlg : public CRememberLocationDlg
{
// Construction
public:
	BOOL m_bCanFixCoordX,m_bCanFixCoordY,m_bCanFixCoordZ;
	CAxisValFixNodeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAxisValFixNodeDlg)
	enum { IDD = IDD_DEF_AXIS_VAL_FIX_DLG };
	double	m_fFixValX;
	double	m_fFixValY;
	double	m_fFixValZ;
	int		m_iFixAxis;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAxisValFixNodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAxisValFixNodeDlg)
	afx_msg void OnRdoAxis();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AXISVALFIXNODEDLG_H__FCB4A15E_FAC5_4BF1_B91E_6C06E636A86B__INCLUDED_)
