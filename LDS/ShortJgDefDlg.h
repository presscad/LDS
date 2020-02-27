#if !defined(AFX_SHORTJGDEFDLG_H__5F76B1AC_8283_452C_90E0_6E6DBB2C7A99__INCLUDED_)
#define AFX_SHORTJGDEFDLG_H__5F76B1AC_8283_452C_90E0_6E6DBB2C7A99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShortJgDefDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CShortJgDefDlg dialog
#ifndef __TSA_
#include "XhCharString.h"

struct SHORT_ANGLE_INFO{
	char cJunctionStyle;//0:双肢连接内包铁;1:X肢外背铁;2:Y肢外背铁
	char cMaterial;		//材质简化字符
	CXhChar16 sizeStr;	//短角钢规格字符串, 格式: "L140X10"
	CXhChar16 sPartNo,xPadPartNo,yPadPartNo;
	double manu_space;
};
class CShortJgDefDlg : public CDialog
{
// Construction
public:
	int m_nThickDiff;
	SHORT_ANGLE_INFO info;
	CShortJgDefDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShortJgDefDlg)
	enum { IDD = IDD_SHORTJG_DEF_DLG };
	CString	m_sJgGuiGe;
	int		m_iMaterial;
	int m_iJunctPosStyle;
	CString	m_sPartNo;
	CString	m_sWingXPadPartNo;
	CString	m_sWingYPadPartNo;
	//double	m_fBaseLen;
	//double	m_fOtherLen;
	//double	m_fBaseNormOffset;
	//double	m_fNonBaseNormOffset;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShortJgDefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShortJgDefDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCmbJunctionPosStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	double m_fManuSpace;
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHORTJGDEFDLG_H__5F76B1AC_8283_452C_90E0_6E6DBB2C7A99__INCLUDED_)
