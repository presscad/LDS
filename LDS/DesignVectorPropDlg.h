#if !defined(AFX_DESIGNVECTORPROPDLG_H__1448B412_B5EF_47C7_A393_211FB424A5E6__INCLUDED_)
#define AFX_DESIGNVECTORPROPDLG_H__1448B412_B5EF_47C7_A393_211FB424A5E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignVectorPropDlg.h : header file
//
#include "PropertyList.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDesignVectorPropDlg dialog

class CDesignVectorPropDlg : public CCallBackDialog
{
// Construction
public:
	CDesignVectorPropDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateDataPropList();
	//属性管理
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	void InitPropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
public:
	void FinishSelectObjOper();		//完成选择对象的后续操作
// Dialog Data
	//{{AFX_DATA(CDesignVectorPropDlg)
	enum { IDD = IDD_DESIGN_VECTOR_PROP_DLG };
	CPropertyList	m_propList;
	int		m_iNormCalStyle;
	double	m_fVectorX;
	double	m_fVectorY;
	double	m_fVectorZ;
	//}}AFX_DATA
	//当前正在编辑的构件句柄，保证关闭对话框时仍能选中该构件 wht 11-05-25
	long m_hCurEditPart;
	BOOL m_bSpecAngleWingNormStyle;
	CLDSVector m_xVector;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignVectorPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignVectorPropDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeCmbNormCalStyle();
	afx_msg void OnCopyVector();
	afx_msg void OnPasteVector();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNVECTORPROPDLG_H__1448B412_B5EF_47C7_A393_211FB424A5E6__INCLUDED_)
