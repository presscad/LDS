#pragma once

#include "CallBackDialog.h"
#include "supergridctrl.h"
#include "WirePlaceCode.h"
#include "ElectricTower.h"

struct WIRE_NODE{
	long m_hRelatedObj;		//挂点的关联对象：节点、挂孔、金具
	int m_iNo;			//节点序号
	WIREPLACE_CODE m_xWirePlace;
	GEPOINT pos;
	GEPOINT vSketchArrow;
	//
	WIRE_NODE() :m_hRelatedObj(0), m_iNo(0) {}
	void ClonProp(WIRE_NODE* pWireNode)
	{
		m_hRelatedObj = pWireNode->m_hRelatedObj;
		m_iNo = pWireNode->m_iNo;
		m_xWirePlace = pWireNode->m_xWirePlace;
		pos = pWireNode->pos;
		vSketchArrow = pWireNode->vSketchArrow;
	}
};
//////////////////////////////////////////////////////////////////////////
// CDesWireNodeDlg 对话框
class CDesWireNodeDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesWireNodeDlg)
	int m_iCurSel;
	int m_nMarginBottom, m_nMarginRight, m_nBtnHeigh;
	bool m_bEnableWindowMoveListen;
	bool m_bEnableWindowSizeListen;
	static int nScrLocationX,nScrLocationY;
	static int nClientRectW, nClientRectH;
public:
	CDesWireNodeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesWireNodeDlg();

// 对话框数据
	enum { IDD = IDD_DES_WIRE_NODE_DLG };

public:
	CSuperGridCtrl m_listCtrl;
	//
	ATOM_LIST<WIRE_NODE> m_arrWireNode;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void UpdateUIWireNodesToUniWireModel();
	void UpdateUniWireModelToUIWireNodes();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnAddWireNode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnImportWireModel();
	afx_msg void OnBnExportWireModel();
public:
	void ExtractWireNodeFromTa();
	void RefreshListCtrl();
	WIRE_NODE* GetSelWireNode();
	BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	void SetSelectIndex(int iCurSel){m_iCurSel=iCurSel;}
	void LayoutDlg();
};
