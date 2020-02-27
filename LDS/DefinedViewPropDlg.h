#if !defined(AFX_DEFINEDVIEWPROPDLG_H__F3A36ECC_042A_431A_8042_3B5154E570D9__INCLUDED_)
#define AFX_DEFINEDVIEWPROPDLG_H__F3A36ECC_042A_431A_8042_3B5154E570D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefinedViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefinedViewPropDlg dialog
#include "ListViewComparer.h"
#include "XhListCtrl.h"
#include "PropertyList.h"
#include "CallBackDialog.h"
#include "NativePropertyList.h"

class CDefinedViewPropDlg : public CCallBackDialog
{
// Construction
	CListViewComparer listViewComparer;
	SEGI m_iSegI;
	void ResetDlgSize();
	CDisplayView backupview;
public:
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	BOOL m_bLayerSegView;	//以图层视图分段属性模式显示
	UCS_STRU ucs;
	CDrawingAssistant *m_pDrawingAssistant;
	CDisplayView *m_pDefinedView;
	CDefinedViewPropDlg(CWnd* pParent = NULL);   // standard constructor

	void DisplayDrawingAssistProperty(CDrawingAssistant *pAssis=NULL);
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void UpdateDatumPointPropItem(CPropTreeItem *pParentItem,CLDSPoint *pPoint,BOOL bUpdate);
	void UpdateDatumVectorPropItem(CPropTreeItem *pParentItem,BOOL bUpdate);
	//
	void InitPropHashtable();						//初始化属性哈希表
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	int GetUcsPropValueStr(long id, char *valueStr);
	static CHashStrList<PROPLIST_ITEM>propHashtable;	//属性字符串哈希表
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	static PROPLIST_ITEM* AddPropItem(char* keyStr,PROPLIST_ITEM item);
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static long GetPropID(char* propStr);			//得到属性ID
// Dialog Data
	//{{AFX_DATA(CDefinedViewPropDlg)
	enum { IDD = IDD_DEFINED_VIEW_PROP_DLG };
	CXhListCtrl	m_xDrawingAssistantList;
	CPropertyList	m_propList;
	CDefinedViewPropertyList m_propListParaCS;
	BOOL	m_bSetDrawingAssistant;
	int		m_iStyle;
	//}}AFX_DATA
	CListCtrl* m_pListCtrl;
	CStringArray m_arrHeaderLabel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefinedViewPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefinedViewPropDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDblclkListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawBySelPartUcs();
	afx_msg void OnItemchangedListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSpreadDrawAllPlate();
	afx_msg void OnAutoJudgeDrawAllPlate();
	afx_msg void OnAddDrawingAssistant();
	afx_msg void OnDeleteDrawingAssistant();
	afx_msg void OnSetDrawingAssistant();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListDrawingAssistant(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListDrawingAssistant(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCmbSeg();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateViewPartList();
	void SelectPart(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void ModifyItem(int iCurSel);
	int AppendRecord(int iItem, CStringArray &str_arr);
	void SetAllPlateDrawFlag(BOOL bSpreadDraw);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFINEDVIEWPROPDLG_H__F3A36ECC_042A_431A_8042_3B5154E570D9__INCLUDED_)
