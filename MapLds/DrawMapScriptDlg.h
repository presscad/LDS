#if !defined(AFX_DRAWMAPSCRIPTDLG_H__1757B2B0_437C_4ED5_B48D_04FB29C14DE2__INCLUDED_)
#define AFX_DRAWMAPSCRIPTDLG_H__1757B2B0_437C_4ED5_B48D_04FB29C14DE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawMapScriptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrawMapScriptDlg dialog
#include "StdArx.h"
#include "resource.h"
#include "PropertyList.h"
#include "supergridctrl.h"
#include "Drawing.h"
#include "CADCallBackDlg.h"
#include "Drawing.h"
#include "XhListCtrl.h"

#ifdef __SMART_DRAW_INC_
class CDrawMapScriptDlg : public CCADCallBackDlg
{
// Construction
	int m_nMaxCmdDrawingSerialNo;
	BOOL m_bSuperListCtrlInUsing; //判断当前ListCtrl类型 wht 12-02-22
	int m_iOperType;	//操作类型 0.运行(遇断点停止) 1.上一步 2.下一步 3.延伸杆件心线 4.添加辅助线 5.添加相似形
	void ExecutableCommand(int iOperType);	//执行指定操作 //3.延伸杆件心线 4.添加辅助线 5.添加相似形
	void DisplaySuperCtrlCmdList();
	void FinishSelectObjOper();				//完成选择对象的后续操作
	void EnableAllItem(BOOL bFlag);			//启用\禁用所有项
	void EnableSelectedItem(BOOL bFlag);	//启用\禁用选中项
	void DisplayCommonCtrlCmdList(int iGroup=0);//使用普通ListView显示命令列表
	int InsertCmdToListCtrl(int iGroup,CDrawingCommand *pCmd,int iInsertIndex);
public:
	CDrawMapScriptDlg(CWnd* pParent = NULL);   // standard constructor
	~CDrawMapScriptDlg();
	int GetOperType() const {return m_iOperType;}	//得到操作类型
	CSuperGridCtrl::CTreeItem * GetFirstExecutableItem();	//得到第一个可执行的命令行
	CSuperGridCtrl::CTreeItem * GetLastExecutedItem();		//得到最后一个已执行的的命令行
	int GetCurCmdGroup(){return m_tabCmdType.GetCurSel();}	//得到当前命令组
	void SelectEntObj(int nResultEnt=1);					//选择对象节点或线
	void UpdateSpreadLinePos();				//更新展开线位置
	void RefreshCommandList(int iListGroup);//刷新命令列表
	void UpdatePartSpecPropertyList(CPropTreeItem *pParentItem,CDrawingCommand *pCmd,
		BOOL bSonItem,BOOL bUpdate=FALSE);
	void UpdateRodBoltOrFlInfoPropList(CPropTreeItem *pParentItem,
		CDrawingCommand *pCmd,BOOL bUpdate=FALSE);
// Dialog Data
	//{{AFX_DATA(CDrawMapScriptDlg)
	enum { IDD = IDD_DRAW_MAP_SCRIPT_DLG };
	CTabCtrl	m_tabCmdType;
	CPropertyList	m_propList;
	CSuperGridCtrl	m_listCmdSuperCtrl;
	CXhListCtrl		m_listCmdCommonCtrl;
	CString	m_sScriptName;
	CString	m_sKeyWord;
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawMapScriptDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
public:
	CDrawing *m_pDrawing;				//当前图纸
	CDrawingComponent *m_pComponent;	//当前视图

	CHashList<CDrawingCommand*> hashRodBoltOrFlInfoList;	//杆件螺栓或法兰信息标注命令列表
	CHashList<CDrawingCommand*> hashPartSpecList;	//构件明细命令列表
	CHashList<CDrawingCommand*> hashOddmentList;	//正负头命令列表
	CHashList<CDrawingCommand*> hashFootNailSpaceList;//脚钉间距命令列表
	CHashList<CDrawingCommand*> hashFillPlateSpaceList;//填板间距命令列表
	CDrawingCommand *m_pCurCmd;			//当前命令
	ATOM_LIST<CDrawingCommand*> cmdList;//当前选中项对应的命令列表
	CDrawingSpreadLine *m_pSpreadLine;	//当前展开线
	int m_iOldGroup;		//记录前一个属性组
	long m_idEventProp;		//记录触发中断的属性ID,恢复窗口时使用
	BOOL m_bCallByDrawingDlg;	//通过绘图对话框调用
	BOOL m_bPdmodeIs34;			//数据扩展点显示模式为34 wht 11-07-15
public:
	BOOL IsSpreadLinePropGroup(int iCurSel);
	void DisplayPropertyList(int iCurSel=0);
	void HideWnd(BOOL bRestart);	//隐藏窗口支持在某些操作后弹出
	//属性管理
	static CStringKeyHashTable<long>propHashtable;	//属性字符串哈希表
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	int GetPropValueStr(long id, char *valueStr);	//根据属性ID得到属性值
	void InitPropHashtable();						//初始化属性哈希表
	int GetPropStr(long id, char *propStr);			//得到属性字符串
	static long GetPropID(char* propStr,...);		//得到属性ID
	int GetPropValueStr(CDrawingCommand *pCmd,long id,char *valueStr);//得到指定命令指定ID的属性字符串
	BOOL IsPropEqual(long idProp,int iSonCmdType=-1);//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDrawMapScriptDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSelchangeTabCmdType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeKeyWord();
	afx_msg void OnEnableAllItem();
	afx_msg void OnDisableAllItem();
	afx_msg void OnEnableSelectedItem();
	afx_msg void OnDisableSelectedItem();
	afx_msg void OnItemchangedCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMoveBefore();
	afx_msg void OnMoveAfter();
	afx_msg void OnAdjustDrawingSerialNo();
	afx_msg void OnAddItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWMAPSCRIPTDLG_H__1757B2B0_437C_4ED5_B48D_04FB29C14DE2__INCLUDED_)
