#if !defined(AFX_TUBEENDLJSTDPARTLIBDLG_H__7AD374C6_9782_48B8_9416_196C3493C0E0__INCLUDED_)
#define AFX_TUBEENDLJSTDPARTLIBDLG_H__7AD374C6_9782_48B8_9416_196C3493C0E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TubeEndLjStdPartLibDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTubeEndLjStdPartLibDlg dialog

#include "env_def.h"
#include "PropertyList.h"
#include "TowerTreeCtrl.h"
#include "supergridctrl.h"
#include "XhListCtrl.h"
#include "PropListItem.h"
#include "RememberLocationDlg.h"

#ifndef __TSA_
class CTubeEndLjStdPartLibDlg : public CRememberLocationDlg
{
	// Construction
	HTREEITEM m_hCurEditItem;	//记录当前可编辑项或新建项
	CBitmap m_CurPlateBmp;
	BOOL m_bModify,m_bNewStdPart;
	int	m_iStdPartType,m_iCurDisplayPartType;
	ATOM_LIST<TREEITEM_INFO>itemInfoList;
	//画笔
	CPen solid_pen,dash_pen,dash_dot_pen,green_solid_pen,green_dash_pen,
		red_solid_pen,blue_solid_pen,purple_solid_pen;
private:
	void ContextMenu(CWnd* pWnd, CPoint point);
	void IntelliSavePreModify();
	void RefreshTreeCtrl();		//刷新树控件
	void InitParam();
	void UpdateLsParas(BOOL bSrcPlateToLsList=TRUE);
	BOOL SaveData();
public:
	CImageList m_imageList;
	BOOL m_bModifyBoltPos;
	INSERT_PLATE_PARAM insert_plate_param;	//插板参数
	ATOM_LIST<LAYOUT_LS_PARAM> lsParaList;
	FL_PARAM fl_param;		//高颈法兰参数
	FL_PARAM2 fl_param2;	//刚性法兰及柔性法兰参数
	//布置螺栓参数
	typedef struct tagLayoutBoltPara 
	{
		int m_nBoltD;		//直径
		int m_nBoltNum;		//个数 
		//矩阵布置螺栓参数
		int m_nRowNum;		//行数
		int m_nColNum;		//列数
		double m_fRowSpace;	//行距
		double m_fColSpace;	//列距
		f3dPoint m_posFirstBolt;//首螺栓位置
		//
		double m_fSpecialSpace;	//特殊间距
		BOOL m_bAutoLayoutDoubleRow;//自动布置为双排螺栓
		BOOL m_bStaggerLayout;		//水平面与竖直面交错布置
		BOOL m_bMirLayout;			//对称布置特殊间距两侧螺栓
		tagLayoutBoltPara(){memset(this,0,sizeof(tagLayoutBoltPara));}
		void CopyLayoutBoltPara(LAYOUT_LS_PARAM& ls_para){
			m_nBoltNum =ls_para.iBoltNum;
			m_nBoltD   =ls_para.iBoltD;
			m_nRowNum  =ls_para.iRow;
			m_nColNum  =ls_para.iColumn;
			m_fColSpace=ls_para.fHorzSpace;
			m_fRowSpace=ls_para.fVertSpace;
			m_posFirstBolt.x=ls_para.fBoltPosX;
			m_posFirstBolt.y=ls_para.fBoltPosY;
			m_bStaggerLayout=(m_nRowNum==-1||m_nColNum==-1);
		}
	}LAYOUT_BOLT_PARA;
	LAYOUT_BOLT_PARA LayoutBoltPara;
public:
	CTubeEndLjStdPartLibDlg(CWnd* pParent = NULL);   // standard constructor
	//
	TREEITEM_INFO* GetCurSelItemInfo(HTREEITEM* pSelItem=NULL,BOOL bInitPartType=FALSE);
	void UpdateStdPartState();
	int  GetStdPartType() const {return m_iStdPartType;}
	void Redraw();
	void RefreshSketch(BOOL bDispRibPlateSketch);
	double CalTheroyWeight(INSERT_PLATE_PARAM param,PARAM_PLATE_TYPE iType);	//根据插板外形计算插板理论重量
	double CalTheroyWeight(FL_PARAM param,PARAM_PLATE_TYPE iType);				//根据法兰外形计算法兰理论重量
	double CalTheroyWeight(FL_PARAM2 param,PARAM_PLATE_TYPE iType);				//根据法兰外形计算法兰理论重量
	//螺栓处理
	void GetParamPlateLayoutBoltParam(CLDSParamPlate *pParamPlate);	//根据螺栓列表反推螺栓布置参数，可能存在误差，需手动调整
	void InsertBoltLayoutParam();		//插入螺栓布置参数
	void UpdateBoltLayoutParam();		//更新螺栓布置参数
	void RefreshBoltLayoutList();		//刷新螺栓布置参数列表
	void RefreshBoltList();				//刷新螺栓列表
	//属性管理
	DECLARE_PROP_FUNC(CTubeEndLjStdPartLibDlg);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);
	void DisplayPropertyList();
	void UpdateBoltLayParaProperty(CPropTreeItem* pGroupItem);
// Dialog Data
	//{{AFX_DATA(CTubeEndLjStdPartLibDlg)
	enum { IDD = IDD_TUBE_ENDLJ_STDPART_LIB_DLG };
	CSuperGridCtrl	m_listBoltParam;
	CTowerTreeCtrl	m_treeCtrl;
	CXhListCtrl		m_listBolt;
	CPropertyList	m_propList;
	CString	m_sOperPrompt;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTubeEndLjStdPartLibDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTubeEndLjStdPartLibDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnCancelModify();
	afx_msg void OnSaveData();
	afx_msg void OnBtnUpdateLib();
	afx_msg void OnImportStdPartLibExcelFile();
	afx_msg void OnImportStdPartLibFile();
	afx_msg void OnExportStdPartLibFile();
	afx_msg void OnNewItem();
	afx_msg void OnDeleteAllItem();
	afx_msg void OnEditItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnCopyAndNewItem();
	afx_msg void OnFlCoupleLevelTable();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnConfirm();
	afx_msg void OnItemchangedBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUBEENDLJSTDPARTLIBDLG_H__7AD374C6_9782_48B8_9416_196C3493C0E0__INCLUDED_)
