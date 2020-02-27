#pragma once
#include "supergridctrl.h"
#include "ShankDefCls.h"
#include "Markup.h"
#include "env_def.h"
#include "ModelTree.h"

// CTowerStructDlg �Ի���
class CTowerStructDlg : public CDialog
{
	DECLARE_DYNAMIC(CTowerStructDlg)
	void RetrievedTowerModelTree();
public:
	static const int DATA_SLOPE		= 1;	//�¶ο��� K
	static const int DATA_SEGMENT	= 2;	//�ֶ� D
	static const int DATA_HEIGHT	= 3;	//���߷���	B
	static const int DATA_SUBLEG	= 4;	//���� L
	static const int DATA_SECTION	= 5;	//�ڼ� J
protected:
	ATOM_LIST<TREEITEM_INFO>itemInfoList;
	CHashSet<TREEITEM_INFO*> hashItemInfos;
	TREEITEM_INFO* AddOrUpdateItemInfo(TREEITEM_INFO& iteminfo);
public:
	double m_fColinarTolerance;
	CTowerStructDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTowerStructDlg();
// �Ի�������
	enum { IDD = IDD_TOWER_STRUCT_DLG };
	bool m_bFireListItemChange;
	CSuperGridCtrl	m_listBodyOpening;
	void UpdateDialogSize(int cx, int cy);
	static const BYTE OBJ_TYPE_SLOPE	= 1;
	static const BYTE OBJ_TYPE_SEGMENT	= 2;
	static const BYTE OBJ_TYPE_HEIGHT	= 3;
	static const BYTE OBJ_TYPE_LEG		= 4;
	bool UpdateListOpenCtrlByModelTree(void *pObj,CModelSlopeNode *pDownSlope,CSuperGridCtrl::CTreeItem *pParentItem,BYTE cObjType);
protected:
	CSuperGridCtrl::CTreeItem* InsertModelSegmentNode(CSuperGridCtrl *pListCtrl,CModelSegmentNode *pSeg,CSuperGridCtrl::CTreeItem *pParentItem);
	CSuperGridCtrl::CTreeItem* InsertModelHeightNode(CSuperGridCtrl *pListCtrl,CModelHeightNode *pHeight,CSuperGridCtrl::CTreeItem *pParentItem);
	CSuperGridCtrl::CTreeItem* InsertModelSubLegNode(CSuperGridCtrl *pListCtrl,CModelSubLegNode *pLeg,CSuperGridCtrl::CTreeItem *pParentItem);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnUpdateTowerModelTree();
	afx_msg void OnBnClickedBtnAllocSegmentNumber();
	afx_msg void OnBnClickedBtnExportExcel();
	afx_msg void OnOK();
};
