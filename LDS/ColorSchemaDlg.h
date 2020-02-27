#if !defined(AFX_COLORSCHEMEDLG_H__1E62FA47_0651_439B_8BEA_2C35252245B8__INCLUDED_)
#define AFX_COLORSCHEMEDLG_H__1E62FA47_0651_439B_8BEA_2C35252245B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorSchemeDlg.h : header file
//
#include "PropertyList.h"
/////////////////////////////////////////////////////////////////////////////
// CColorSchemeDlg dialog
#include "supergridctrl.h"
class CColorSchemeDlg : public CDialog
{
// Construction
public:
	CColorSchemeDlg(CWnd* pParent = NULL);   // standard constructor
	void InitPropHashtable();
	void DisplayPropertyListPart();
	int GetPropStr(long id, char *propStr);			//得到属性字符串
	static CHashStrTable<long>propHashtable;	//属性字符串哈希表
	static long GetPropID(char* propStr);			//得到属性ID
	BOOL Refresh(BOOL bJudge);
	void InsertPropItem(CPropTreeItem *pPropItem);
// Dialog Data
	//{{AFX_DATA(CColorSchemeDlg)
	enum { IDD = IDD_COLOR_SCHEME_DIALOG };
	CPropertyList	m_propList;
	CSuperGridCtrl m_listcolor;
	int		m_iSchemeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSchemeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorSchemeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSchemeName();
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOriginalState();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSCHEMEDLG_H__1E62FA47_0651_439B_8BEA_2C35252245B8__INCLUDED_)
