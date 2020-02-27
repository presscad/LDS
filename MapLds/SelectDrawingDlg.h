#if !defined(AFX_SELECTDRAWINGVIEWDLG_H__D6D3A984_313E_412B_9102_5FEFBC9B2120__INCLUDED_)
#define AFX_SELECTDRAWINGVIEWDLG_H__D6D3A984_313E_412B_9102_5FEFBC9B2120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectDrawingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectDrawingDlg dialog

#include "resource.h"
#include "PropertyList.h"
#include "RxTools.h"
#include "Drawing.h"
#include "env_def.h"
#include "TowerTreeCtrl.h"
#include "PropListItem.h"

#ifdef __SMART_DRAW_INC_
typedef struct tagVIEWITEM_INFO
{
	CDisplayView *m_pView;	//关联视图
	int m_nSeg;				//段号
	BOOL m_bDrawView;		//绘制视图	
	BOOL m_bSpecUcs;		//指定坐标系
	BOOL m_bHasScript;		//已存在脚本
	UCS_STRU dcs;			//绘图坐标系
	int	 m_iViewType;		//0.视图 1.视图组
	tagVIEWITEM_INFO(){memset(this,0,sizeof(tagVIEWITEM_INFO)); m_bDrawView=TRUE;}
	tagVIEWITEM_INFO(CDisplayView *pView,int nSeg,int iViewType=0)
	{
		m_pView=pView;
		m_nSeg=nSeg;
		m_iViewType=iViewType;
		m_bDrawView=TRUE;
		m_bSpecUcs=m_bHasScript=FALSE;
	}
}VIEWITEM_INFO;

class CSelectDrawingDlg : public CDialog
{
// Construction
	BOOL m_bRestartDlg;
	BOOL m_bCanKeepFocus;
	ATOM_LIST<TREEITEM_INFO>itemInfoList;
	CFont m_xFont;
	void UpdateDrawingList(BOOL bReset);				//更新图纸列表
	void DeleteItemInfo(CDrawing *pDrawing);			//删除pDrawing及其子项关联的itemInfo
	void DeleteItemInfo(CDrawingComponent *pComponent);	//删除pComponent关联的itemInfo
	void DeleteItemInfo(TREEITEM_INFO *pItemInfo);		//删除pItemInfo关联pComponent或pDrawing的项
														//（当删除的项pItemInfo->itemType==3时需共用此段代码）
	void UpdateItem();
public:
	CSelectDrawingDlg(CWnd* pParent = NULL);   // standard constructor
	CDrawing *m_pDrawing;
	CDrawingComponent *m_pComponent;
	TREEITEM_INFO *m_pItemInfo;
// Dialog Data
	//{{AFX_DATA(CSelectDrawingDlg)
	enum { IDD = IDD_SELECT_DRAWING_DLG };
	CPropertyList	m_propList;
	CTreeCtrl	m_treeDrawing;
	//}}AFX_DATA
	CImageList m_Images;
	BOOL m_bModeless;	//非模态
public:
	void HideWnd(BOOL bRestart);
	BOOL Create();
	void MoveItem(int iType);	//0.置顶 1.上移 2.下移 3.置尾
	void RefreshDrawingTree(CDrawing *pCurDrawing=NULL,CDrawingComponent *pCurDrawingCom=NULL);
	void DisplayDrawingProperty();
	void RebulidItem();
	//属性管理
	DECLARE_PROP_FUNC(CSelectDrawingDlg)
	void InitEnglishPropHashtable();
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDrawingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDrawingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnImportScript();
	afx_msg void OnExportScript();
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnEraseRelaCadEnt();
	afx_msg void OnMoveTop();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveBottom();
	afx_msg void OnAddItem();
	afx_msg void OnAddPartItem();
	afx_msg void OnUpdateItem();
	afx_msg void OnRebuildItem();
	afx_msg void OnDblclkTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSaveScript();
	afx_msg void OnClickTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTreeDrawing(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
#ifdef _WIN64
	afx_msg void OnTimer(UINT_PTR nIDEvent);
#else
	afx_msg void OnTimer(UINT nIDEvent);
#endif
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDisplayCmdDlg();
	afx_msg void OnCreateDrawing();
	//}}AFX_MSG
	afx_msg LRESULT OnAcadKeepFocus(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};
extern CSelectDrawingDlg *g_pSelectDrawingDlg;
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTDRAWINGVIEWDLG_H__D6D3A984_313E_412B_9102_5FEFBC9B2120__INCLUDED_)
