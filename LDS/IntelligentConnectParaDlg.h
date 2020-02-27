#if !defined(AFX_INTELLIGENTCONNECTPARADLG_H__E7815BEC_0267_4B31_AF25_920E1AE757C2__INCLUDED_)
#define AFX_INTELLIGENTCONNECTPARADLG_H__E7815BEC_0267_4B31_AF25_920E1AE757C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IntelligentConnectParaDlg.h : header file
//
#include "DragListCtrl.h"
#include "PropertyList.h"
/////////////////////////////////////////////////////////////////////////////
// CIntelligentConnectParaDlg dialog

class CIntelligentConnectParaDlg : public CDialog
{
// Construction
	typedef struct tagBITMAP_INFO
	{
		CBitmap bmp;
		CString sName;
	}BITMAP_INFO;
	BITMAP_INFO m_arrBitemapInfo[25];
	CImageList m_ImageList;
	long m_nCurItemData;
	
	CStringArray m_arrL1RayPole;	//角钢主材1根射线杆件的连接 
	CStringArray m_arrL2RayPole;	//角钢主材2根射线杆件的连接 
	CStringArray m_arrL3RayPole;	//角钢主材3根射线杆件的连接 
	CStringArray m_arrLnRayPole;	//角钢主材多根射线杆件的连接
	CStringArray m_arrInsertPlateType;	//插板类型
	CStringArray m_arrFlType;		//法兰类型

	void RefreshListView(int iCurSel);
	void RefreshTabCtrl(int iCurSel);
public:
	int m_nRayPoleNum;
	RECT m_rcClient;
	CLDSLinePart *m_pDatumLinePart;
public:
	CIntelligentConnectParaDlg(CWnd* pParent = NULL);   // standard constructor
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	void InitSysSettingPropHashtable();
	void DisplaySystemSetting();
	static long GetPropID(char* propStr);	//得到属性ID
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
// Dialog Data
	//{{AFX_DATA(CIntelligentConnectParaDlg)
	enum { IDD = IDD_INTELLIGENT_CONNECT_PARA_DLG };
	CPropertyList	m_propList;
	CDragListCtrl	m_listLj;
	CTabCtrl	m_tabDesignType;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIntelligentConnectParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIntelligentConnectParaDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedListLj(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeStypeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnddragList(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
//	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTELLIGENTCONNECTPARADLG_H__E7815BEC_0267_4B31_AF25_920E1AE757C2__INCLUDED_)
