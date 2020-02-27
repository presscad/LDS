#if !defined(AFX_NEWTOWERDLG_H__20CD06F9_CFC9_45FD_B408_9D459DD23AC6__INCLUDED_)
#define AFX_NEWTOWERDLG_H__20CD06F9_CFC9_45FD_B408_9D459DD23AC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewTowerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewTowerDlg dialog
#include "Markup.h"
#include "ShankDefCls.h"
#include "supergridctrl.h"
#include "PropertyList.h"
//#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
#include "BitmapGraph.h"
#include "StdFacePanel.h"
#include "..\Draw3d\DrawEngine.h"
#include "MenuButton.h"

//K5800,1600,1600
//	D5400:1200,1300,1300,1600
//	D8900
//K,2600,2600
//	D4900
//	D7700
//		B30m	//呼高分组
//			D6700
//			L6700
//			L8200
//			B33m//呼高分组
//				L11200
enum LISTITEM_TYPE
{
	DATA_SLOPE=1,	//坡段开口 K
	DATA_SEG,		//分段 D
	DATA_MODULE,	//呼高分组	B
	DATA_LEG,		//塔腿 L
	DATA_SECTION,	//节间 J
};

class CNewTowerDlg : public CDialog
{
	// Construction
	BOOL m_bAutoCalTextHeight;
	int	 m_iOldUnit;	//记录前一次使用的单位索引
	int  rcRightMargin,rcBtmMargin;
	CDrawEngine *m_pDraw;
	CBitmapGraph bitmap;	//标准面示意图
	//
	BOOL XMLDataSerialize(CString sFileName, BOOL bWrite);
	bool ImportShankDefFromXML(CMarkup xml,LISTITEM_TYPE data_type,CShankDefineObject *pParent);
	bool ExportShankDefToXML(CMarkup& xml,CShankDefineObject *pParent);
	bool UpdateListOpenCtrlByShankDef(CShankDefineObject *pObj=NULL,CSuperGridCtrl::CTreeItem *pParentItem=NULL);
	BOOL IsIncludeSpecTypeDataInItem(char cObjType,CSuperGridCtrl::CTreeItem *pSpecItem=NULL);
	void ContextMenu(CPoint point);
	void InsertSeg(CSuperGridCtrl::CTreeItem *pParentItem,int index=-1);
	void InsertSection(CSuperGridCtrl::CTreeItem *pParentItem,int index=-1);
	CRect m_rectClient;	//右侧绘图区域
	void UpdateDialogSize(int cx, int cy, bool blReflectEventOnSize = true);
public:
	BOOL m_iViewType;	//0.正面视图;1.侧面视图;2.透视图
	//属性管理
	static CHashStrTable<long>propHashtable;	//属性字符串哈希表
	void DisplayPropertyList(CSuperGridCtrl::CTreeItem* pSelItem);
	int GetPropValueStr(CSuperGridCtrl::CTreeItem* pSelItem,int prop_id,char* valueStr);	//根据属性ID得到属性值
	void InitItemPropHashtable();
	static long GetPropID(char* propStr);	//得到属性ID
public:
	BOOL m_bStartContextMenu;
	bool m_bFireListItemChange;
	CNewTowerDlg(CWnd* pParent = NULL);	// standard constructor
	CShankDefine m_xShankDef;
	BOOL ReadCursorFacePanel(CFacePanel& panel,BOOL bPromptError);
	void UpdateFaceDetailGraph(CFacePanel &panel);
	void UpdateStdPanelSketch(int iPanelType,CString labelStdPanel);
	void ReDrawTowerSketchView();
	CShankDefineObject* GetSelItemRelaObject();
	//cFlag==0x0001表示删除Item；cFlag==0x002表示删除AttachData
	static const DWORD DELETE_ITEM				= 0X000000001;
	static const DWORD DELETE_ATTACHDATA		= 0X000000002;
	BOOL DeleteItemAndAttachData(CSuperGridCtrl::CTreeItem* pSelItem,DWORD dwFlag);
// Dialog Data
	//{{AFX_DATA(CNewTowerDlg)
	enum { IDD = IDD_NEW_TOWER_DLG };
	CPropertyList	m_propList;
	CSuperGridCtrl	m_listBodyOpening;
	int		m_iMainPartType;
	int		m_iSideNum;
	int		m_iUnit;
	double	m_fTextHeight;
	int		m_iMultiLegSchema;
	CMenuButton m_xMenuBtn;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTowerDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNewTowerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeUnit();
	afx_msg void OnSelchangeTowerSideNum();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNewSlope();
	afx_msg void OnNewSeg();
	afx_msg void OnNewModule();
	afx_msg void OnNewLeg();
	afx_msg void OnDeleteItem();
	afx_msg void OnImportXMLFile();
	afx_msg void OnExportXMLFile();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnBtnFrontView();
	afx_msg void OnBtnPerspectiveView();
	afx_msg void OnBtnSideView();
	afx_msg void OnDraw3dZoomAll();
	afx_msg void OnDraw3dRotate();
	afx_msg void OnDraw3dZoom();
	afx_msg void OnDraw3dReset();
	afx_msg void OnDraw3dPan();
	afx_msg void OnDraw3dZoomWnd();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDraw3dIdle();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnChangeETextHeight();
	afx_msg void OnSelchangeCmbLegSchema();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNewSection();
	afx_msg void OnInsertSection();
	afx_msg void OnInsertSeg();
	afx_msg void OnBtnSwitchToClassicUi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTOWERDLG_H__20CD06F9_CFC9_45FD_B408_9D459DD23AC6__INCLUDED_)
