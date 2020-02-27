#if !defined(AFX_JOINTBASEJG_H__876BBDB4_B21B_4C14_A242_F56E1CB1140D__INCLUDED_)
#define AFX_JOINTBASEJG_H__876BBDB4_B21B_4C14_A242_F56E1CB1140D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JointBaseJg.h : header file
//
#include "RememberLocationDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CJointBaseJg dialog

class CJointBaseJg : public CRememberLocationDlg
{
// Construction
public:
	CJointBaseJg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJointBaseJg)
	enum { IDD = IDD_JOINT_BASE_JG_DLG };
	CString	m_sLsGuiGe;
	int		m_nLsN;
	int		m_nLsRows;
	CString	m_sJgGuiGe;
	int		m_nLsEndSpace;
	int		m_nLsSpace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJointBaseJg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJointBaseJg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLsGuige();
	afx_msg void OnLsRows();
	afx_msg void OnChangeLsN();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOINTBASEJG_H__876BBDB4_B21B_4C14_A242_F56E1CB1140D__INCLUDED_)
