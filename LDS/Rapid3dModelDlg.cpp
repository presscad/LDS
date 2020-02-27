// Rapid3dModelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "image.h"
#include "Rapid3dModelDlg.h"
#include "afxdialogex.h"
#include "PropertyListOper.h"


// CRapid3dModelDlg 对话框

IMPLEMENT_DYNAMIC(CRapid3dModelDlg, CDialogEx)

CRapid3dModelDlg::CRapid3dModelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRapid3dModelDlg::IDD, pParent)
{
	m_propList.m_iPropGroup = 0;
}

CRapid3dModelDlg::~CRapid3dModelDlg()
{
}

void CRapid3dModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIG_TREE, m_xConfigTree);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
}


BEGIN_MESSAGE_MAP(CRapid3dModelDlg, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONFIG_TREE, &CRapid3dModelDlg::OnTvnSelchangedConfigTree)
END_MESSAGE_MAP()


// CRapid3dModelDlg 消息处理程序


BOOL CRapid3dModelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_listImages.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	m_xConfigTree.SetImageList(&m_listImages,TVSIL_NORMAL);
	m_xConfigTree.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);

	RefreshTreeCtrl();
	if (INTELLI_MODEL_GUIDE::hashPropItemById.GetNodeNum() == 0)
		INTELLI_MODEL_GUIDE::InitPropHashtable();
	m_xInternalLocalGuide.CopyFrom(&gxModelGuide);
	//属性栏设置
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_propList.SetDividerScale(0.6);
	DisplayPropertyList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CRapid3dModelDlg::RefreshTreeCtrl()
{
	TREEITEM_INFO *pItemInfo=NULL;
	itemInfoList.Empty();
	hashItemInfos.Empty();
	m_xConfigTree.DeleteAllItems();
	HTREEITEM hItem, hRootItem;
	hRootItem = m_xConfigTree.InsertItem("三维模型", PRJ_IMG_ROOT, PRJ_IMG_ROOT, TVI_ROOT);
	hItem=m_xConfigTree.InsertItem("通用设计规则",PRJ_IMG_PARA,PRJ_IMG_PARA,hRootItem);
	pItemInfo = itemInfoList.append(TREEITEM_INFO(1, 0));
	m_xConfigTree.SetItemData(hItem, (DWORD)pItemInfo);
	hItem = m_xConfigTree.InsertItem("分类连接设计",PRJ_IMG_MODULECASE,PRJ_IMG_MODULECASE,hRootItem);
	pItemInfo = itemInfoList.append(TREEITEM_INFO(2, 0));
	m_xConfigTree.SetItemData(hItem, (DWORD)pItemInfo);
	hItem = m_xConfigTree.InsertItem("多呼高多接腿",PRJ_IMG_TOWERLEG,PRJ_IMG_TOWERLEG,hRootItem);
	pItemInfo = itemInfoList.append(TREEITEM_INFO(3, 0));
	m_xConfigTree.SetItemData(hItem, (DWORD)pItemInfo);
	m_xConfigTree.Expand(hRootItem,TVE_EXPAND);
}
BOOL CRapid3dModelDlg::ModifyInteliiModelGuideProperty(CPropTreeItem* pItem, CString& valueStr)
{
	CPropertyListOper<INTELLI_MODEL_GUIDE> oper(&m_propList, &this->m_xInternalLocalGuide);
	if (INTELLI_MODEL_GUIDE::GetPropID("ciProcessLegacyConnections") == pItem->m_idProp)
		m_xInternalLocalGuide.ciProcessLegacyConnections = valueStr[0]-'0';
	else if (INTELLI_MODEL_GUIDE::GetPropID("ciPlateProfileStyle") == pItem->m_idProp)
		m_xInternalLocalGuide.ciPlateProfileStyle = valueStr[0]-'0';
	//塔头横担连接设计
	else if (INTELLI_MODEL_GUIDE::GetPropID("blIncArmHeadConnPlates") == pItem->m_idProp)
	{	//包含塔头横担连接板设计
		if (valueStr.CompareNoCase("是") == 0)
			m_xInternalLocalGuide.blIncArmHeadConnPlates = true;
		else
			m_xInternalLocalGuide.blIncArmHeadConnPlates = false;
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("blAddWireAngle") == pItem->m_idProp)
	{	//需要时添加外贴挂线角钢
		if (valueStr.CompareNoCase("是") == 0)
			m_xInternalLocalGuide.blAddWireAngle = true;
		else
			m_xInternalLocalGuide.blAddWireAngle = false;
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("blAddWirePoint") == pItem->m_idProp)
	{	//添加挂点连接设计
		if (valueStr.CompareNoCase("是") == 0)
			m_xInternalLocalGuide.blAddWirePoint = true;
		else
			m_xInternalLocalGuide.blAddWirePoint = false;
	}
	//多呼高塔身连接设计
	else if (INTELLI_MODEL_GUIDE::GetPropID("blIncSlopeTransitPlates") == pItem->m_idProp)
	{	//包含变坡板,塔头瓶口板
		if (valueStr.CompareNoCase("是") == 0)
			m_xInternalLocalGuide.blIncSlopeTransitPlates = true;
		else
			m_xInternalLocalGuide.blIncSlopeTransitPlates = false;
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("blIncArmConnPlates") == pItem->m_idProp)
	{	//包含塔身横担连接板
		if (valueStr.CompareNoCase("是") == 0)
			m_xInternalLocalGuide.blIncArmConnPlates = true;
		else
			m_xInternalLocalGuide.blIncArmConnPlates = false;
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("blSectionCornerPlates") == pItem->m_idProp)
	{	//塔身水平隔面连接角板
		if (valueStr.CompareNoCase("是") == 0)
			m_xInternalLocalGuide.blSectionCornerPlates = true;
		else
			m_xInternalLocalGuide.blSectionCornerPlates = false;
	}
	//多接腿设计
	else if (INTELLI_MODEL_GUIDE::GetPropID("VPlate.blVPlateInside") == pItem->m_idProp)
	{	//V面板相对角钢肢的位置
		if(valueStr.CompareNoCase("贴于里皮")==0)
			m_xInternalLocalGuide.multileg.blVPlateInside = true;
		else
			m_xInternalLocalGuide.multileg.blVPlateInside = false;
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("VPlate.cnMinBaseAngleBolts") == pItem->m_idProp)
	{	//V面板基准连接材上的最小螺栓数
		m_xInternalLocalGuide.multileg.snMinBaseAngleBolts = (BYTE)atoi(valueStr);
	}
	//AddPropItem("VPlate.siBendLineGStyle", PROPLIST_ITEM(id++, "V面板交接点", "V面板射材线材交接点", "0.预定准线|1.偏移准线"));
	else if (INTELLI_MODEL_GUIDE::GetPropID("VPlate.siBendLineSpace") == pItem->m_idProp)
	{	//V面板火曲间隙
		m_xInternalLocalGuide.multileg.siBendLineSpace = (BYTE)atoi(valueStr);
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("VPlate.siBendLineG") == pItem->m_idProp)
	{	//V面板偏移准距
		m_xInternalLocalGuide.multileg.siBendLineG = (BYTE)atoi(valueStr);
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("VPlate.ciProfileSchema") == pItem->m_idProp)
	{	//V面板外形定义方式
		m_xInternalLocalGuide.multileg.ciProfileSchema = valueStr[0] - '0';
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("VPlate.ciTouchLineType") == pItem->m_idProp)
	{
		m_xInternalLocalGuide.multileg.ciTouchLineType = valueStr[0] - '0';
		//
		m_propList.DeleteAllSonItems(pItem);
		if(m_xInternalLocalGuide.multileg.ciTouchLineType==1)
			oper.InsertEditPropItem(pItem, "VPlate.siBendLineG", "", "", -1, TRUE);
	}
	else if (INTELLI_MODEL_GUIDE::GetPropID("VPlate.siSpecThick") == pItem->m_idProp)
	{
		m_xInternalLocalGuide.multileg.siSpecThick = atoi(valueStr);
	}
	else
		return false;
	return true;
}
static BOOL _LocalModifyInteliiModelGuideProperty(CPropertyList* pPropList, CPropTreeItem* pItem, CString& valueStr)
{
	CRapid3dModelDlg* pRapidModelDlg = (CRapid3dModelDlg*)pPropList->GetParent();
	if (pRapidModelDlg)
		return pRapidModelDlg->ModifyInteliiModelGuideProperty(pItem, valueStr);
	else
		return false;
}
void CRapid3dModelDlg::DisplayPropertyList()
{
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(_LocalModifyInteliiModelGuideProperty);
	CPropTreeItem *pGroupItem = NULL, *pPropItem = NULL, *pLeafItem = NULL;
	CPropTreeItem *pRoot = m_propList.GetRootItem();
	CPropertyListOper<INTELLI_MODEL_GUIDE> oper(&m_propList, &this->m_xInternalLocalGuide);
	//连板外形
	pPropItem = oper.InsertCmbListPropItem(pRoot, "ciProcessLegacyConnections");
	pPropItem->m_dwPropGroup = 0x01;// GetSingleWord(1);
	pPropItem = oper.InsertCmbListPropItem(pRoot, "ciPlateProfileStyle");
	pPropItem->m_dwPropGroup = 0x01;// GetSingleWord(1);
	//塔头横担连接设计
	pPropItem = oper.InsertCmbListPropItem(pRoot, "blIncArmHeadConnPlates");
	pPropItem->m_dwPropGroup = 0x02;
	pPropItem = oper.InsertCmbListPropItem(pRoot, "blAddWireAngle");
	pPropItem->m_dwPropGroup = 0x02;
	pPropItem = oper.InsertCmbListPropItem(pRoot, "blAddWirePoint");
	pPropItem->m_dwPropGroup = 0x02;
	//多呼高塔身连接设计
	pPropItem = oper.InsertCmbListPropItem(pRoot, "blIncSlopeTransitPlates");
	pPropItem->m_dwPropGroup = 0x02;
	pPropItem = oper.InsertCmbListPropItem(pRoot, "blIncArmConnPlates");
	pPropItem->m_dwPropGroup = 0x02;
	pPropItem = oper.InsertCmbListPropItem(pRoot, "blSectionCornerPlates");
	pPropItem->m_dwPropGroup = 0x02;
	//多接腿设计
	pPropItem = oper.InsertCmbListPropItem(pRoot, "VPlate.ciProfileSchema");
	pPropItem->m_dwPropGroup = 0x04;
	pPropItem = oper.InsertCmbListPropItem(pRoot, "VPlate.blVPlateInside");
	pPropItem->m_dwPropGroup = 0x04;
	pPropItem = oper.InsertEditPropItem(pRoot, "VPlate.siSpecThick");
	pPropItem->m_dwPropGroup = 0x04;
	pPropItem = oper.InsertEditPropItem(pRoot, "VPlate.siBendLineSpace");
	pPropItem->m_dwPropGroup = 0x04;
	pPropItem = oper.InsertCmbListPropItem(pRoot, "VPlate.ciTouchLineType");
	pPropItem->m_dwPropGroup = 0x04;
	if(m_xInternalLocalGuide.multileg.ciTouchLineType==1)
		oper.InsertEditPropItem(pPropItem, "VPlate.siBendLineG");
	pPropItem = oper.InsertEditPropItem(pRoot, "VPlate.cnMinBaseAngleBolts");
	pPropItem->m_dwPropGroup = 0x04;
	m_propList.Redraw();
}

void CRapid3dModelDlg::OnTvnSelchangedConfigTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (itemInfoList.GetNodeNum() == 0)
		return;	//已清空
	HTREEITEM hItemNew = pNMTreeView->itemNew.hItem;
	if(hItemNew)
	{
		TREEITEM_INFO* pInfo = (TREEITEM_INFO*)m_xConfigTree.GetItemData(hItemNew);
		if (pInfo)
			m_propList.m_iPropGroup = pInfo->itemType-1;
		m_propList.Redraw();
	}
	*pResult = 0;
}

void CRapid3dModelDlg::OnOK()
{
	gxModelGuide.CopyFrom(&m_xInternalLocalGuide);

	CDialogEx::OnOK();
}
