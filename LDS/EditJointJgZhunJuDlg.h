#if !defined(AFX_EDITJOINTJGZHUNJUDLG_H__B2F1DC6A_92F4_4B55_A1D8_D265C4E1D18B__INCLUDED_)
#define AFX_EDITJOINTJGZHUNJUDLG_H__B2F1DC6A_92F4_4B55_A1D8_D265C4E1D18B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditJointJgZhunJuDlg.h : header file
//
#include "RememberLocationDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CEditJointJgZhunJuDlg dialog
class CEditJointJgZhunJuDlg : public CRememberLocationDlg
{
// Construction
public:
	JGZJ jgzj2;
	JGZJ jgzj1;
	CEditJointJgZhunJuDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditJointJgZhunJuDlg)
	enum { IDD = IDD_JOINT_JG_ZHUNJU_DLG };
	int		m_nJg1G;
	int		m_nJg1G1;
	int		m_nJg1G2;
	int		m_nJg1G3;
	int		m_nJg2G;
	int		m_nJg2G1;
	int		m_nJg2G2;
	int		m_nJg2G3;
	CString	m_sJg1GuiGe;
	CString	m_sJg2GuiGe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditJointJgZhunJuDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditJointJgZhunJuDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITJOINTJGZHUNJUDLG_H__B2F1DC6A_92F4_4B55_A1D8_D265C4E1D18B__INCLUDED_)
