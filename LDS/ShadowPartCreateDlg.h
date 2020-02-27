#if !defined(AFX_SHADOWPARTCREATEDLG_H__CD9FAE15_2013_43E7_8DA9_9D1DC41139F9__INCLUDED_)
#define AFX_SHADOWPARTCREATEDLG_H__CD9FAE15_2013_43E7_8DA9_9D1DC41139F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShadowPartCreateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShadowPartCreateDlg dialog
#include "PropertyList.h"
class CShadowPartCreateDlg : public CDialog
{
// Construction
public:
	CShadowPartCreateDlg(CWnd* pParent = NULL);   // standard constructor
//属性管理
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	void InitPropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
	void UpdatePropList();

// Dialog Data
	//{{AFX_DATA(CShadowPartCreateDlg)
	enum { IDD = IDD_SHADOW_PART_CREATE_DLG };
	CPropertyList	m_propList;
	int		m_iShadowStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShadowPartCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShadowPartCreateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADOWPARTCREATEDLG_H__CD9FAE15_2013_43E7_8DA9_9D1DC41139F9__INCLUDED_)
