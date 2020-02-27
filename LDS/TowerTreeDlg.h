#if !defined(AFX_TOWERTREEDLG_H__6C9A5FBA_F3DA_4DC1_AFD9_9750BC2D3525__INCLUDED_)
#define AFX_TOWERTREEDLG_H__6C9A5FBA_F3DA_4DC1_AFD9_9750BC2D3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TowerTreeDlg.h : header file
//
#include "TowerTreeCtrl.h"
#include "f_ent_list.h"
#include "env_def.h"
/////////////////////////////////////////////////////////////////////////////
// CTowerTreeDlg dialog
enum TOWER_TREE_CMD{TREE_ITEM_ADD,TREE_ITEM_DELETE,TREE_ITEM_REFRESH};

class CTowerTreeDlg : public CDialog
{
	static const int ACTIVE_MODULE	=0;
	static const int ACTIVE_BLOCK	=1;
	static const int ACTIVE_DISPVIEW=2;
// Construction
	ATOM_LIST<TREEITEM_INFO>itemInfoList;
	CHashSet<TREEITEM_INFO*> hashItemInfos;
	HTREEITEM m_hServerContentItem,m_hBodyItem;
	HTREEITEM m_hDataRootItem,m_hActiveModuleItem,m_hActiveBlockItem,m_hActiveViewItem;
	HTREEITEM m_hLayerViewItem;
#ifdef __PART_DESIGN_INC_
	HTREEITEM InsertWorkStatusItem(CWorkStatus *pStatus,HTREEITEM hParentItem,HTREEITEM hInsertAfterItem);
#endif
	void RefreshViewGroup(HTREEITEM hViewGroupItem,IModel* pModel);
	void ShiftActiveItemState(HTREEITEM hActiveModelItem,int nType);
	void ActiveModel(IModel *pModel);
	void RefreshActiveItemStateAndActiveModel(IModel *pModel,HTREEITEM hItem);
	TREEITEM_INFO* InsertOrUpdateItemInfo(TREEITEM_INFO& iteminfo);
	HTREEITEM SearchTreeItem(TREEITEM_TYPE itemType,CLDSObject* pObj=NULL);
public:
	HTREEITEM InsertBlockItem(CBlockModel *pBlock);
	void DeleteBlockRefItem(CBlockReference * pBlockRef);
	void SortItem(HTREEITEM hSelItem);
	CTowerTreeDlg(CWnd* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(CTowerTreeDlg)
	CImageList m_ModelImages;
	CTowerTreeCtrl m_treeCtrl;
	int m_iCurModuleNo;
	int m_iCurStatusNo;
// Operations
	bool ShiftActiveItemByObj(CLDSObject* pObj);
	bool UpdateTreeItemByObj(CLDSObject* pObj,bool bEnusureVisible=false,bool bSetBoldFont=false);
	bool CTowerTreeDlg::AddOrUpdateHeightItem(CLDSModule* pHeightModule,
		bool bEnusureVisible = false, bool bActivate = false);
	bool AddOrUpdateTreeItemByObj(CLDSObject* pObj, bool bEnusureVisible = false, bool bActivate = false);
	bool UpdateTreeItemByType(TREEITEM_TYPE itemType,CLDSObject* pBoldFontObj=NULL);
	HTREEITEM GetModuleItemByNo(long iModuleNo);
	void ContextMenu(CWnd *pWnd, CPoint point);
	HTREEITEM GetFirstHeightGroupItem();
	HTREEITEM GetNextHeightGroupItem(HTREEITEM hItem);
#ifdef __FEM_ANALYZE_INC_//  defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
	void RefreshHeightWindSegSetItem(HTREEITEM hHeightWindSegSetItem,CLDSModule* pHeightGroup);
#endif
	bool RefreshHeightGroupItem(HTREEITEM hHeightGroupItem);
	void RefreshModelItem(HTREEITEM hModelItem,IModel* pModel);
	void RefreshTreeCtrl();
	void RefreshMatTypeItem();
	BOOL ModifyTreeItem(TREEITEM_INFO *pItemInfo, TOWER_TREE_CMD cmd, void *pData);
	void DeleteItem(HTREEITEM hItem);
	//void OnRButtonDown(UINT nFlags, CPoint point);
	CTowerTreeCtrl *GetTreeCtrl();
	void NewItem(){OnNewItem();}
	BOOL RenameSelectedItem(CString sName);
	long InsertArcLiftItem(HTREEITEM hArcLiftGropItem,f3dPoint* pRodOrg=NULL,f3dPoint* pDatumLiftPos=NULL);
	BOOL ActivateLayerViewItem(int iViewType);	//激活图层视图 0.透视图 1.前视图 2.后视图 3.左视图 4.右视图 5.俯视图
// Dialog Data
	//{{AFX_DATA(CTowerTreeDlg)
	enum { IDD = IDD_TOWER_STRUCTURE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTowerTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTowerTreeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExpandingTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditItemPropItem();
	afx_msg void OnSetActiveItem();
	afx_msg void OnNewItem();
	afx_msg void OnCopyItem();
	afx_msg void OnNewItemFolder();
	afx_msg void OnRetrieveUdfViews();
	afx_msg void OnRetrieveDrawingViews();
	afx_msg void OnDeleteItem();
	afx_msg void OnRefreshTree();
	afx_msg void OnIntelliCorrectModelObjsLayer();
	afx_msg void OnUnifyAngleBoltsParamG();
	afx_msg void OnRelayoutRodModel();
	afx_msg void OnSortModuleByHeight();
	afx_msg void OnSetModuleNameByHeight();
	afx_msg void OnModelDesignCal();
	afx_msg void OnModelEqCheck();
	afx_msg void OnAttachWindload();
	afx_msg void OnCalWindBetaZ();
	afx_msg void OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSortItem();
	afx_msg void OnHandoverPermission();
	afx_msg void OnItemListProp();
	//afx_msg void OnNodeLoadList();
	//afx_msg void OnNewItemSet();
	//afx_msg void OnCalWireLoad();
	afx_msg void OnRetrieveBodyWindSegments();
	afx_msg void OnExportLoadcalReport();
	afx_msg void OnLayerviewSegPropItem();
	afx_msg void OnImportBlockFile();
	afx_msg void OnExportBlockFile();
	afx_msg void OnUpdateBlock();
	afx_msg void OnExplodedBlockRef();
	afx_msg void OnIntegrateBlockRef();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//菜单帮助项导出管理
	static CHashStrList<PROPLIST_ITEM>propMenuHashtable;	//属性字符串哈希表
	static CHashSet<PROPLIST_ITEM*>hashMenuHelpItemById;
	static void InitMenuHelpHashtable();
	static PROPLIST_ITEM* AddMenuHelpItem(char* keyStr,PROPLIST_ITEM item);
	PROPLIST_ITEM* GetSelectHelpItem(int menuId=0);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOWERTREEDLG_H__6C9A5FBA_F3DA_4DC1_AFD9_9750BC2D3525__INCLUDED_)
