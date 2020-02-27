#if !defined(AFX_TOWERPARTGROUPDLG_H__69D54B62_9A53_4D63_8765_2B21166D6C27__INCLUDED_)
#define AFX_TOWERPARTGROUPDLG_H__69D54B62_9A53_4D63_8765_2B21166D6C27__INCLUDED_
#include "TowerTreeCtrl.h"
#include "f_ent_list.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CTowerPartGroupDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CTowerPartGroupDlg dialog
const int PARTGROUP_SET=1;
const int PARTGROUP_CASE=2;
const int PARTGROUP_STATNUM=3;
const int PARTGROUP_ROOTPART=4;
const int PARTGROUP_VIEWSET=5;
const int PARTGROUP_VIEWCASE=6;
const int PARTGROUP_FOLDER=7; 

struct TREEITEM_PARTGROUP_INFO{
	TREEITEM_PARTGROUP_INFO(){;}
	TREEITEM_PARTGROUP_INFO(long type,void *ptr){itemType=type;pObj=ptr;}
	TREEITEM_PARTGROUP_INFO(long type,void *ptr,long h){itemType=type;pObj=ptr;handle=h;}
	long itemType,handle;
	void *pObj;
	CSectionView* GetVerifiedSectionView(CLDSPartGroup** ppPartGroup=NULL);
};

class CTowerPartGroupDlg : public CDialog
{
// Construction
	ATOM_LIST<TREEITEM_PARTGROUP_INFO>itemInfoList;
	//刷新指定组焊件对应的树结点
	void RefreshPartGroupTreeNode(HTREEITEM hPartGroupItem,CLDSPartGroup *pPartGroup,bool removeOld=true);
	void RefreshPartGroupViewSetNode(HTREEITEM hViewSetItem,CLDSPartGroup *pPartGroup);
	void RefreshPartGroup(const char* incSegStr=NULL);
	HTREEITEM SearchPartGroupItem(long hPart,HTREEITEM hStartItem,UINT nCode=0);
public:
	void RefreshModel();
	CTowerPartGroupDlg(CWnd* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(CTowerPartGroupDlg)
	CImageList m_ModelImages;
	void ContextMenu(CWnd *pWnd, CPoint point);
	CTowerTreeCtrl *GetTreeCtrl(){return &m_treeCtrl;}
	BOOL CheckPartGroupValidAndCorrect(CLDSPartGroup *pPartGroup,BOOL bAutoCorrect,char *sError);
	void DisplaySectionView(CLDSPartGroup *pPartGroup,CSectionView *pSectionView);
private:
	void DeletePartGroup(HTREEITEM hSelItem);
// Dialog Data
	//{{AFX_DATA(CTowerPartGroupDlg)
	enum { IDD = IDD_TOWER_PART_GROUP_DLG };
	CTowerTreeCtrl	m_treeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTowerPartGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTowerPartGroupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefreshPartGroup();
	afx_msg void OnSmartRetrieveWeldline();
	afx_msg void OnStatPartGroupAuto();
	afx_msg void OnSortPartGroup();
	afx_msg void OnDeletePartgroup();
	afx_msg void OnRetrievePartgroupViews();
	afx_msg void OnEditPartgroup();
	afx_msg void OnNewPartgroup();
	afx_msg void OnNewPartgroupFolder();
	afx_msg void OnSetActivePartgroup();
	afx_msg void OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPartGroupLocatePoint();
	afx_msg void OnPartGroupCheckLine();
	afx_msg void OnPartGroupAngular();
	afx_msg void OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopyAndNewItem();
	afx_msg void OnCheckPartGroupValid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOWERPARTGROUPDLG_H__69D54B62_9A53_4D63_8765_2B21166D6C27__INCLUDED_)
