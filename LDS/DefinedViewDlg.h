#if !defined(AFX_DEFINEDVIEWDLG_H__F3A36ECC_042A_431A_8042_3B5154E570D9__INCLUDED_)
#define AFX_DEFINEDVIEWDLG_H__F3A36ECC_042A_431A_8042_3B5154E570D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefinedViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDefinedViewDlg dialog
#include "ListViewComparer.h"
#include "XhListCtrl.h"
#include "PropertyList.h"
#include "CallBackDialog.h"

class CDefinedViewDlg : public CCallBackDialog
{
// Construction
	CListViewComparer listViewComparer;
	SEGI m_iSegI;
	void ResetDlgSize();
	CDisplayView backupview;
public:
	BOOL m_bLayerSegView;	//以图层视图分段属性模式显示
	UCS_STRU ucs;
	CDrawingAssistant *m_pDrawingAssistant;
	CDisplayView *m_pDefinedView;
	CDefinedViewDlg(CWnd* pParent = NULL);   // standard constructor

	void DisplayDrawingAssistProperty(CDrawingAssistant *pAssis=NULL);
	void FinishSelectObjOper();		//完成选择对象的后续操作
	void UpdateDatumPointPropItem(CPropTreeItem *pParentItem,CLDSPoint *pPoint,BOOL bUpdate);
	void UpdateDatumVectorPropItem(CPropTreeItem *pParentItem,BOOL bUpdate);
	//
	void InitPropHashtable();						//初始化属性哈希表
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
	static long GetPropID(char* propStr);			//得到属性ID
// Dialog Data
	//{{AFX_DATA(CDefinedViewDlg)
	enum { IDD = IDD_DEFINED_VIEW_DLG };
	CXhListCtrl	m_xDrawingAssistantList;
	CPropertyList	m_propList;
	double	m_fAxisX_X;
	double	m_fAxisX_Y;
	double	m_fAxisX_Z;
	double	m_fAxisY_X;
	double	m_fAxisY_Y;
	double	m_fAxisY_Z;
	double	m_fAxisZ_X;
	double	m_fAxisZ_Y;
	double	m_fAxisZ_Z;
	double	m_fOriginPosX;
	double	m_fOriginPosY;
	double	m_fOriginPosZ;
	BOOL	m_bCanModifyAxisX;
	BOOL	m_bCanModifyAxisY;
	BOOL	m_bCanModifyAxisZ;
	CString	m_sViewName;
	CString m_sFolderName;
	CString	m_sDefaultLayer;
	BOOL	m_bSetDrawingAssistant;
	int		m_iStyle;
	//}}AFX_DATA
	CListCtrl* m_pListCtrl;
	CStringArray m_arrHeaderLabel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefinedViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefinedViewDlg)
	afx_msg void OnBnPasteAxisX();
	afx_msg void OnBnPasteAxisY();
	afx_msg void OnBnPasteAxisZ();
	afx_msg void OnBnPasteOriginPos();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnCopyAxisX();
	afx_msg void OnBnCopyAxisY();
	afx_msg void OnBnCopyAxisZ();
	afx_msg void OnBnCopyOriginPos();
	afx_msg void OnBnCpyUcs();
	afx_msg void OnBnPasteUcs();
	afx_msg void OnBnToRightHand();
	afx_msg void OnChkEnableAxisX();
	afx_msg void OnChkEnableAxisY();
	afx_msg void OnChkEnableAxisZ();
	afx_msg void OnDblclkListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawBySelPartUcs();
	afx_msg void OnItemchangedListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSpreadDrawAllPlate();
	afx_msg void OnAutoJudgeDrawAllPlate();
	afx_msg void OnBnReverseAxisX();
	afx_msg void OnBnReverseAxisY();
	afx_msg void OnBnReverseAxisZ();
	afx_msg void OnAddDrawingAssistant();
	afx_msg void OnDeleteDrawingAssistant();
	afx_msg void OnSetDrawingAssistant();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListDrawingAssistant(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListDrawingAssistant(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCmbSeg();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateViewPartList();
	void UpdateUcs(BOOL bFlag=TRUE);
	void SelectPart(int iCurSel);
	void DeleteItem(int iCurSel);
	void RefreshListViewHeader();
	void ModifyItem(int iCurSel);
	int AppendRecord(int iItem, CStringArray &str_arr);
	void SetAllPlateDrawFlag(BOOL bSpreadDraw);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFINEDVIEWDLG_H__F3A36ECC_042A_431A_8042_3B5154E570D9__INCLUDED_)
