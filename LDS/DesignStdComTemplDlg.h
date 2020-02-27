#pragma once

#include "CallBackDialog.h"
#include "XhCharString.h"
#include "MemberProperty.h"
#include ".\StdComTempl\StdTemplPropertyList.h"
//////////////////////////////////////////////////////////////////////////
// CDesignStdComTemplDlg 对话框
class CDesignStdComTemplDlg : public CCallBackDialog
{
protected:
	DECLARE_DYNAMIC(CDesignStdComTemplDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CBitmap m_xComSketchBmp;
	//long m_hBaseTube,m_hBaseNode;	//防止undo时取消节点导致指针失效 wjh-2018.10.8
	//long m_hOuterBeamDatumNode,m_hInnerBeamDatumNode;
	//long m_hOuterAngle,m_hInnerAngle;
	virtual CStdComTemplPropertyList* _InternalGetMainPropList(){return NULL;}
public:
	UINT m_idStdComTmpl;
	//
	//bool VerifyRelaRodAndNodes();
	//CStdComTempl *m_pCurrTempl;
	//CTubeWalkWayStdComTempl m_xWalkWayTempl;
public:
	CDesignStdComTemplDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesignStdComTemplDlg();
	// 对话框数据
	enum { IDD = IDD_DESIGN_STD_COM_TEMPL_DLG };
	CNativeSlavePropertyList m_propSubList;
	CStdComTemplPropertyList* get_pMainPropList(){return _InternalGetMainPropList();}
	__declspec(property(get=get_pMainPropList)) CStdComTemplPropertyList* pMainPropList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	//
	virtual void DisplayPropList(){;}
	virtual void DisplaySubPropList(){;}
};
#ifdef __COMPLEX_PART_INC_
#include ".\WalkwayPropertyList.h"
class CDesignTubeWalkWayDlgEx : public CDesignStdComTemplDlg
{
public:
	DECLARE_PROP_FUNC(CDesignTubeWalkWayDlgEx);
	CWalkWayPropertyList m_xTemplUI;

	bool VerifyRelaRodAndNodes(){return m_xTemplUI.VerifyRelaRodAndNodes();}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//DECLARE_MESSAGE_MAP()
	virtual CStdComTemplPropertyList* _InternalGetMainPropList(){return &m_xTemplUI;}
protected:
	void UpdateWalkWayProp(CPropTreeItem* pParentItem);
	void UpdateHolderProp(CPropTreeItem* pParentItem);
	void UpdateAssemblyProp(CPropTreeItem* pParentItem);
public:
	CDesignTubeWalkWayDlgEx();
	PROPERTY(long,m_hBaseNode);
	GET(m_hBaseNode){return m_xTemplUI.m_hBaseNode;}
	SET(m_hBaseNode){m_xTemplUI.m_hBaseNode=value;}
	PROPERTY(long,m_hBaseTube);
	GET(m_hBaseTube){return m_xTemplUI.m_hBaseTube;}
	SET(m_hBaseTube){m_xTemplUI.m_hBaseTube=value;}
	PROPERTY(long,m_hOuterAngle);
	GET(m_hOuterAngle){return m_xTemplUI.m_hOuterAngle;}
	SET(m_hOuterAngle){m_xTemplUI.m_hOuterAngle=value;}
	PROPERTY(long,m_hInnerAngle);
	GET(m_hInnerAngle){return m_xTemplUI.m_hInnerAngle;}
	SET(m_hInnerAngle){m_xTemplUI.m_hInnerAngle=value;}
	PROPERTY(long,m_hOuterBeamDatumNode);
	GET(m_hOuterBeamDatumNode){return m_xTemplUI.m_hOuterBeamDatumNode;}
	SET(m_hOuterBeamDatumNode){m_xTemplUI.m_hOuterBeamDatumNode=value;}
	PROPERTY(long,m_hInnerBeamDatumNode);
	GET(m_hInnerBeamDatumNode){return m_xTemplUI.m_hInnerBeamDatumNode;}
	SET(m_hInnerBeamDatumNode){m_xTemplUI.m_hInnerBeamDatumNode=value;}

	CLDSNode* get_m_pBaseNode(){return m_xTemplUI.params.m_pBaseNode;}
	CLDSNode* set_m_pBaseNode(CLDSNode* value);
	__declspec(property(put=set_m_pBaseNode,get=get_m_pBaseNode)) CLDSNode* m_pBaseNode;

	CLDSLineTube* get_m_pBaseTube(){return m_xTemplUI.params.m_pBaseTube;}
	CLDSLineTube* set_m_pBaseTube(CLDSLineTube* value);
	__declspec(property(put=set_m_pBaseTube,get=get_m_pBaseTube)) CLDSLineTube* m_pBaseTube;

	PROPERTY(CLDSLineAngle*,m_pOuterJg);
	GET(m_pOuterJg){return m_xTemplUI.params.m_pOuterJg;}
	SET(m_pOuterJg){m_xTemplUI.params.m_pOuterJg=value;}
	PROPERTY(CLDSLineAngle*,m_pInnerJg);
	GET(m_pInnerJg){return m_xTemplUI.params.m_pInnerJg;}
	SET(m_pInnerJg){m_xTemplUI.params.m_pInnerJg=value;}
};
//////////////////////////////////////////////////////////////////////////
//钢管安全挂点支架CDesignTubeSafeHolderDlgEx
#include "SafeHolderPropertyList.h"
class CDesignTubeSafeHolderDlgEx : public CDesignStdComTemplDlg
{
	DECLARE_PROP_FUNC(CDesignTubeSafeHolderDlgEx);
public:
	CTubeSafeHolderPropertyList m_xTemplUI;
public:
	CDesignTubeSafeHolderDlgEx();
	//
	bool VerifyRelaRodAndNodes(){return m_xTemplUI.VerifyRelaRodAndNodes();}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//DECLARE_MESSAGE_MAP()
	virtual CStdComTemplPropertyList* _InternalGetMainPropList(){return &m_xTemplUI;}
};
#endif
//////////////////////////////////////////////////////////////////////////
//CDesignWireStruTemplDlgEx
#include "WireStruTemplPropertyList.h"
class CDesignWireStruTemplDlgEx : public CDesignStdComTemplDlg
{
public:
	DECLARE_PROP_FUNC(CDesignWireStruTemplDlgEx);
	CWireStruComPropertyList m_xTemplUI;
	CDesignWireStruTemplDlgEx();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//DECLARE_MESSAGE_MAP()
	virtual CStdComTemplPropertyList* _InternalGetMainPropList(){return &m_xTemplUI;}
protected:
	//void UpdateWalkWayProp(CPropTreeItem* pParentItem);
	//void UpdateHolderProp(CPropTreeItem* pParentItem);
	//void UpdateAssemblyProp(CPropTreeItem* pParentItem);
public:
};
