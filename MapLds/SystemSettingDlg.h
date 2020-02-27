#if !defined(AFX_SYSTEMSETTINGDLG_H__3D173AEA_DB2E_447B_A95F_9BC1A05338D0__INCLUDED_)
#define AFX_SYSTEMSETTINGDLG_H__3D173AEA_DB2E_447B_A95F_9BC1A05338D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemSettingDlg.h : header file
//
#ifdef __DRAWING_CONTEXT_
#include "Tower.h"
#include "resource.h"
#include "RxTools.h"
#include "SysPara.h"
#include "SysDrawingGraph.h"
#endif
#include "PropertyList.h"
#include "PropListItem.h"

/////////////////////////////////////////////////////////////////////////////
// CSystemSettingDlg dialog

class CSystemSettingDlg : public CDialog
{
#ifdef __DRAWING_CONTEXT_
// Construction
	int m_nMargin,m_nWorkPaneW;
	static int m_iCurrentTabPage;
#ifndef __BP_ONLY_
	CSysDrawingGraph  m_xGraphManager;
#endif
public:
	int  m_iDistType;			//0.只显示属性栏 1.显示图片 2.实时绘制
	CBitmap m_bBitMap;
	static MAP_FRAME_SIZE *m_pCurMapFrm;
	static ARRAY_LIST<long> m_xSpecialLayerIdArr;
private:
	void RelayoutWnd();
public:
	CSystemSettingDlg(CWnd* pParent = NULL);   // standard constructor
	//显示示意图
	void SetPromptPicture(long bitmapId);
	void SetPromptPicture(const char* bmpfile);
	void SetDrawingPara(DRAWING_PARA &draw_para);
	void SetOtherOper();
	//
	void Relayout(BOOL bMoveMainWnd=TRUE);
	CPropTreeItem* InsertFontPropItem(char *propKeyStr,CPropTreeItem *pPropItem,int iMapType,int size0_wtoh1=0);
	void InsertOrUpdateLayerItem(TMADRAWING_LAYER *pLayer,CPropTreeItem *pParentItem);
	void UpdateSpecialLayerItem();
	void DisplaySystemSetting();
	BOOL IsUseLayer(TMADRAWING_LAYER *pLayer);
#endif
public:
	union KEY2WORD{
		struct{
			WORD idProp;
			WORD idGroup;
		};
		DWORD idComplex;
		//
		KEY2WORD(int idSign,int idCommonProp){idGroup=idSign;idProp=idCommonProp;}
		KEY2WORD(long idValue){idComplex=idValue;}
		operator long(){return (long)idComplex;}
	};
	//属性管理
	static int MIN_SYSSETTING_PROPID;
	DECLARE_PROP_FUNC(CSystemSettingDlg)
	static void InitEnglishPropHashtable();
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100,CPropTreeItem *pItem=NULL);	//根据属性ID得到属性值
#ifdef __DRAWING_CONTEXT_
// Dialog Data
	//{{AFX_DATA(CSystemSettingDlg)
	enum { IDD = IDD_SYSTEM_SETTING_DLG };
	CPropertyList	m_propList;
	CTabCtrl	m_ctrlPropGroup;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemSettingDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSystemSettingDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMSETTINGDLG_H__3D173AEA_DB2E_447B_A95F_9BC1A05338D0__INCLUDED_)
