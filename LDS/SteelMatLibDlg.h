#if !defined(AFX_STEELMATLIBDLG_H__9E67C5D7_FE61_4704_B30D_E2728F681664__INCLUDED_)
#define AFX_STEELMATLIBDLG_H__9E67C5D7_FE61_4704_B30D_E2728F681664__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SteelMatLibDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSteelMatLibDlg dialog
#include "supergridctrl.h"
class CSteelMatLibDlg : public CDialog
{
// Construction
public:
	int m_iContentsType;	//0.显示材质1.显示风振系数插值2.螺栓选择条件3.杆塔覆冰重力荷载系数
	CSteelMatLibDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Refresh();
// Dialog Data
	//{{AFX_DATA(CSteelMatLibDlg)
	enum { IDD = IDD_STEEL_MAT_LIB_DLG };
	CSuperGridCtrl	m_listSteelMat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSteelMatLibDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSteelMatLibDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPasteData();
	afx_msg void OnDelData();
	afx_msg void OnBnImport();
	afx_msg void OnBnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEELMATLIBDLG_H__9E67C5D7_FE61_4704_B30D_E2728F681664__INCLUDED_)
