#if !defined(AFX_FOOTDESIGNTEMPLDLG_H__C4DA9304_F2FE_4042_AB6B_CA562E7E1C3C__INCLUDED_)
#define AFX_FOOTDESIGNTEMPLDLG_H__C4DA9304_F2FE_4042_AB6B_CA562E7E1C3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FootDesignTemplDlg.h : header file
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

#include "list.h"
#include "CallBackDialog.h"
#include "PropertyList.h"
#include "afxcmn.h"
#include "supergridctrl.h"
#include "I_DrawSolid.h"
#include "FootDesignTemplate.h"

//将CFootTempl移到FootDesignTemplate.h中
/////////////////////////////////////////////////////////////////////////////
// CFootDesignTemplDlg dialog
class CFootDesignTemplDlg : public CCallBackDialog
{
	CBitmap m_xCurSketchBmp;
	UINT m_uidDiBanSketchBmp;
	ATOM_LIST<TREEITEM_INFO>itemInfoList;
public:
	const static int ITEM_TYPE_HOLE			= 1;	//孔
	const static int ITEM_TYPE_SHOEPLATE	= 2;	//靴板
	const static int ITEM_TYPE_LINKANGLE	= 3;	//射线角钢
	const static int ITEM_TYPE_RIBPLATE		= 4;	//肋板
// Construction
public:
	const static int DISP_BASE_PLATE  = 1;
	const static int DISP_SHOE_PLATE  = 2;
	const static int DISP_RIB_PLATE	  = 3;
	const static int DISP_OTHER		  = 4 ;
public:
	CFootDesignTemplDlg(CWnd* pParent = NULL);   // standard constructor
	~CFootDesignTemplDlg();   // standard destructor
	BOOL m_bUpAndDownSide;	//上下两侧都有靴板
	CFootTempl foottempl;
	CStringArray m_arrPropGroupLabel;
	CTabCtrl m_ctrlPropGroup;
	CPropertyList m_xPropList;
	CSuperGridCtrl m_listCtrlShoePlates;
	CSuperGridCtrl m_listCtrlLinkAngles;
	UCS_STRU rgn_ucs;
	CRect m_rectValidRgn;
	int m_iDispType;
	int m_iActiveTabPage;
	int m_iCmdParaType;
	DWORD m_idActiveRibPlate;
	IDrawSolid* m_pDrawSolid;
// Dialog Data
	//{{AFX_DATA(CFootDesignTemplDlg)
	enum { IDD = IDD_FOOT_DESIGN_TEMPL_DLG };
	//}}AFX_DATA
	BOOL m_bSaveData;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFootDesignTemplDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFootDesignTemplDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnPan();			//平移
	afx_msg void OnBnOsnap();		//捕捉
	afx_msg void OnBnZoom();		//缩放
	afx_msg void OnBnOther();		//空闲
	afx_msg void OnAddHole();
	afx_msg void OnAddLinkAngle();
	afx_msg void OnBnClickedBnAddRibPlate();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	DECLARE_PROP_FUNC(CFootDesignTemplDlg);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值字符串
	//
	void InitDrawSolidInfo();
	void UpdateSketchMap();
	CFootTempl::RIBPLATE* ActiveRibPlate(){return foottempl.listRibPlates.GetValue(m_idActiveRibPlate);}
	BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	void DisplayDesignFootProperty();
	void UpdateBasePlateOriginProperty();
	void UpdateLinkAngleProperty(CPropTreeItem *pPareItem,BOOL bUpJg=TRUE);
	void UpdateShoePlateProperty(CPropTreeItem *pPareItem,BOOL bUpShoePlate=TRUE);
	void UpdateRibPlateProperty(CPropTreeItem *pParentItem,CFootTempl::RIBPLATE* pRibPlate=NULL);
	void RefreshLinkAngleListCtrl(CFootTempl::SHOEPLATE* pShoePlate=NULL,void *pSelectedItemData=NULL);
	void RefreshShoePlateListCtrl();
	void UpdateShoePlateInfo(CFootTempl::SHOEPLATE* pShoePlate);
	void DeleteItemInfo(TREEITEM_INFO *pItemInfo);
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOOTDESIGNTEMPLDLG_H__C4DA9304_F2FE_4042_AB6B_CA562E7E1C3C__INCLUDED_)
