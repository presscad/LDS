//<LOCALE_TRANSLATE/>
// DrawMapScriptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DrawMapScriptDlg.h"
#include "XhCharString.h"
#include "DragEntSet.h"
#include "AdsTools.h"
#include "SortFunc.h"
#include "DrawTaMap.h"
#include "AdjustPartDrawingSerialNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawMapScriptDlg dialog

#ifdef __SMART_DRAW_INC_
CDrawMapScriptDlg::CDrawMapScriptDlg(CWnd* pParent /*=NULL*/)
	: CCADCallBackDlg(CDrawMapScriptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawMapScriptDlg)
	m_sScriptName = _T("");
	m_sKeyWord = _T("");
	//}}AFX_DATA_INIT
	m_listCmdSuperCtrl.AddColumnHeader("△");
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCmdSuperCtrl.AddColumnHeader("identification");
	m_listCmdSuperCtrl.AddColumnHeader("drawable item");
	m_listCmdSuperCtrl.AddColumnHeader("enable");
	//
	m_listCmdCommonCtrl.AddColumnHeader("node");
	m_listCmdCommonCtrl.AddColumnHeader("parts");
	m_listCmdCommonCtrl.AddColumnHeader("number");
	m_listCmdCommonCtrl.AddColumnHeader("type");
	m_listCmdCommonCtrl.AddColumnHeader("way");
	m_listCmdCommonCtrl.AddColumnHeader("√");
	m_listCmdCommonCtrl.AddColumnHeader("sequence");
#else
	m_listCmdSuperCtrl.AddColumnHeader("标识");
	m_listCmdSuperCtrl.AddColumnHeader("绘制项");
	m_listCmdSuperCtrl.AddColumnHeader("启用");
	//
	m_listCmdCommonCtrl.AddColumnHeader("节点");
	m_listCmdCommonCtrl.AddColumnHeader("构件");
	m_listCmdCommonCtrl.AddColumnHeader("编号");
	m_listCmdCommonCtrl.AddColumnHeader("类型");
	m_listCmdCommonCtrl.AddColumnHeader("方式");
	m_listCmdCommonCtrl.AddColumnHeader("√");
	m_listCmdCommonCtrl.AddColumnHeader("顺序");
#endif
	m_pCurCmd=NULL;
	m_iOperType=-1;
	m_iOldGroup=0;
	m_pSpreadLine=NULL;
	m_idEventProp=0;
	m_pDrawing=NULL;
	m_pComponent=NULL;
	m_bCallByDrawingDlg=FALSE;	//通过绘图对话框调用
	m_bPdmodeIs34=FALSE;
	m_nMaxCmdDrawingSerialNo=0;
	cmdList.Empty();
}


void CDrawMapScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawMapScriptDlg)
	DDX_Control(pDX, IDC_TAB_CMD_TYPE, m_tabCmdType);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_CMD_LIST_SUPER, m_listCmdSuperCtrl);
	DDX_Control(pDX, IDC_CMD_LIST_COMMON, m_listCmdCommonCtrl);
	DDX_Text(pDX, IDC_S_SCRIPT_NAME, m_sScriptName);
	DDX_Text(pDX, IDC_E_KEY_WORD, m_sKeyWord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawMapScriptDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawMapScriptDlg)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnClose)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CMD_TYPE, OnSelchangeTabCmdType)
	ON_EN_CHANGE(IDC_E_KEY_WORD, OnChangeKeyWord)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CMD_LIST_COMMON, OnItemchangedCmdListCommon)
	ON_NOTIFY(NM_DBLCLK, IDC_CMD_LIST_COMMON, OnDblclkCmdListCommon)
	ON_NOTIFY(NM_RCLICK, IDC_CMD_LIST_COMMON, OnRclickCmdListCommon)
	ON_COMMAND(ID_MOVE_BEFORE, OnMoveBefore)
	ON_COMMAND(ID_MOVE_AFTER, OnMoveAfter)
	ON_COMMAND(ID_ADJUST_DRAWING_SERIAL_NO, OnAdjustDrawingSerialNo)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ENABLE_ALL_ITEM, OnEnableAllItem)
	ON_COMMAND(ID_DISABLE_ALL_ITEM, OnDisableAllItem)
	ON_COMMAND(ID_ENABLE_SELECTED_ITEM, OnEnableSelectedItem)
	ON_COMMAND(ID_DISABLE_SELECTED_ITEM, OnDisableSelectedItem)
	ON_COMMAND(ID_ADD_ITEM, OnAddItem)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawMapScriptDlg message handlers

const long PROPID_SETSIZE = 50;	//属性集合默认尺寸 wht 12-03-22
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
CStringKeyHashTable<long> CDrawMapScriptDlg::propHashtable;
CHashTable<DWORD> CDrawMapScriptDlg::propStatusHashtable;	//属性状态哈希表
const int GROUP_DRAW_PART=1,GROUP_DIM_ODDMENT=2,GROUP_DIM_SIMPOLY=3,GROUP_SPREADLINE=4;
//更新杆件法兰或螺栓信息
static void UpdateRodStartEndInfoPropList(CPropertyList *pPropList,CPropTreeItem *pGroupItem,CDrawingCommand *pCmd)
{
	if(pPropList==NULL||pGroupItem==NULL||pCmd==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DIM_BOLTORFL_INFO||pCmd->m_paraBoltOrFlInfo.pLinePart==NULL)
		return;
	CDrawMapScriptDlg *pDrawMapDlg=(CDrawMapScriptDlg*)pPropList->GetParent();
	
	char valueStr[200]="";
	BOOL bUpdate=TRUE;
	CItemInfo* lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar16 sName("bolt");
	if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
		sName.Copy("flange");
#else
	CXhChar16 sName("螺栓");
	if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
		sName.Copy("法兰");
#endif
	pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset"));
	pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX"));
	pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY"));
	pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset"));
	pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX"));
	pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetY"));
	if(pCmd->m_paraBoltOrFlInfo.iDimPosType==1)
	{
		if(pCmd->m_paraBoltOrFlInfo.bDimStartOdd)
		{	//始端正负头标注位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "starting position offset";
			lpInfo->m_strPropHelp.Format("starting %s dimension position offset value relative to the annotation basis points .",(char*)sName);
#else
			lpInfo->m_strPropName = "始端位置偏移";
			lpInfo->m_strPropHelp.Format("始端%s标注位置相对于标注基点的偏移。",(char*)sName);
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_bHideChildren=TRUE;
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//始端正负头标注位置X坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("starting %s dimension position offset in axis X direction.",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("始端%s标注位置X坐标偏移。",(char*)sName);
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pDrawMapDlg->IsPropEqual(pSonPropItem->m_idProp,2);
			//始端正负头标注位置Y坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("starting %s dimension position offset in axis Y.",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("始端%s标注位置Y坐标偏移。",(char*)sName);
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pDrawMapDlg->IsPropEqual(pSonPropItem->m_idProp,2);
		}
		if(pCmd->m_paraBoltOrFlInfo.bDimEndOdd)
		{	//终端正负头标注位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "ending position offset value";
			lpInfo->m_strPropHelp.Format("ending %s dimension position offset value relative to dimension basis points.",(char*)sName);
#else
			lpInfo->m_strPropName = "终端位置偏移";
			lpInfo->m_strPropHelp.Format("终端%s标注位置相对于标注基点的偏移。",(char*)sName);
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_bHideChildren=TRUE;
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//终端正负头标注位置X坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("ending %s dimension position offset in axis X.",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("终端%s标注位置X坐标偏移。",(char*)sName);
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pDrawMapDlg->IsPropEqual(pSonPropItem->m_idProp,2);
			//终端正负头标注位置Y坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("ending %s dimension position offset in axis Y .",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("终端%s标注位置Y坐标偏移。",(char*)sName);
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetY");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pDrawMapDlg->IsPropEqual(pSonPropItem->m_idProp,2);
		}
	}
}
static BOOL ModifySonItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pPropList->GetParent();
	if(pScriptDlg==NULL)
		return FALSE;
	
	CDrawingCommand **ppCmd=NULL;
	if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
	{
		//构件明细标注参数
		if(CDrawMapScriptDlg::GetPropID("m_bExecute")==pItem->m_idProp)	
		{	//是否执行该命令
			CDrawingCommand *pFirstCmd=NULL;
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				pFirstCmd=*ppSonCmd;
				BOOL bOld=(*ppSonCmd)->m_bExecute;
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
					(*ppSonCmd)->m_bExecute=TRUE;
				else 
					(*ppSonCmd)->m_bExecute=FALSE;
				if((*ppSonCmd)->m_bExecute!=bOld)
				{
					if(bOld)	//删除命令对应的实体
						(*ppSonCmd)->EmptyEntList();
					else		//重新运行该命令
						pFirstCmd->Rerun();
				}
			}
			if(pFirstCmd)	//更新标注构件明细属性
				pScriptDlg->UpdatePartSpecPropertyList(pItem,pFirstCmd,TRUE,TRUE);
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX")==pItem->m_idProp)	//标注位置X坐标偏移值
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.fBasePosOffsetX=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY")==pItem->m_idProp)	//标注位置Y坐标偏移值
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.fBasePosOffsetY=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX")==pItem->m_idProp)	//标注位置X坐标偏移值
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.fDimPosOffsetX=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY")==pItem->m_idProp)	//标注位置Y坐标偏移值
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.fDimPosOffsetY=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX")==pItem->m_idProp)	//规格标注位置X坐标偏移值
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.fGuigeOffsetX=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY")==pItem->m_idProp)	//规格标注位置Y坐标偏移值
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.fGuigeOffsetY=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iDimPosType")==pItem->m_idProp)		//0.自动搜索位置 1.指定位置(保存指定位置与原标注位置的偏移值)  2.选择位置
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.iDimPosType=valueStr[0]-'0';
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDrawRefLine")==pItem->m_idProp)//是否绘制引出线
		{
			CDrawingCommand *pFirstCmd=NULL;
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				pFirstCmd=*ppSonCmd;
				BOOL bOldDrawRefLine=(*ppSonCmd)->m_paraPartSpec.bDrawRefLine;
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
					(*ppSonCmd)->m_paraPartSpec.bDrawRefLine=TRUE;
				else 
					(*ppSonCmd)->m_paraPartSpec.bDrawRefLine=FALSE;
				if(bOldDrawRefLine!=(*ppSonCmd)->m_paraPartSpec.bDrawRefLine)
					(*ppSonCmd)->Rerun();	//删除命令生成的原有实体,重新生成 wht 11-07-18
			}
			//更新界面显示
			CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDrawMapScriptDlg::GetPropID("m_bExecute"),NULL);
			if(pParentItem&&pFirstCmd)
			{
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.DimPosOffset"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.GuigeOffset"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.BasePosOffset"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY"));
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("PARTSPEC.MovePartSpec"));
				
				char tem_str[100]="";
				CItemInfo *lpInfo=NULL;
				CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
				CLDSPart *pPart=NULL;
				if(pFirstCmd->m_paraPartSpec.pObj&&pFirstCmd->m_paraPartSpec.pObj->IsPart())
					pPart=(CLDSPart*)pFirstCmd->m_paraPartSpec.pObj;
				if(pFirstCmd->m_paraPartSpec.bDrawRefLine)
				{	//调整构件明细位置
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("indication line and its detailed location");
					lpInfo->m_strPropHelp = "used to adjust the location of indication line and parts.";
#else
					lpInfo->m_strPropName.Format("引出线及明细位置");
					lpInfo->m_strPropHelp = "用来整体调整引出线和构件明细位置。";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.MovePartSpec");
				}
#ifdef AFX_TARG_ENU_ENGLISH
				char sTemp[10]="number";
				if(pPart)
					strcpy(sTemp,"details");
#else
				char sTemp[10]="编号";
				if(pPart)
					strcpy(sTemp,"明细");
#endif
				//编号位置偏移量
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("%s position offset",sTemp);
				lpInfo->m_strPropHelp.Format("%s position offset value",sTemp);
#else
				lpInfo->m_strPropName.Format("%s位置偏移量",sTemp);
				lpInfo->m_strPropHelp.Format("%s位置偏移量。",sTemp);
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//编号位置X坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp.Format("%s dimension position offset value in axis X direction.",sTemp);
#else
				lpInfo->m_strPropName.Format("X轴方向");
				lpInfo->m_strPropHelp.Format("%s标注位置X轴方向偏移量。",sTemp);
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX");
				if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				//编号位置Y坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp.Format("%s dimension position offset value in axis Y.",sTemp);
#else
				lpInfo->m_strPropName.Format("Y轴方向");
				lpInfo->m_strPropHelp.Format("%s标注位置Y轴方向偏移量。",sTemp);
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY");
				if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				//
				if(pPart&&pPart->IsPlate())
				{	//规格位置偏移量
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("spec position offset");
					lpInfo->m_strPropHelp = "specifications dimension position offset value.";
#else
					lpInfo->m_strPropName.Format("规格位置偏移量");
					lpInfo->m_strPropHelp = "规格标注位置偏移量。";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.GuigeOffset");
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//规格标注位置X坐标偏移值
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis X");
					lpInfo->m_strPropHelp = "specifications dimension position offset value relative to number position in axis X direction";
#else
					lpInfo->m_strPropName.Format("X轴方向");
					lpInfo->m_strPropHelp = "规格标注位置相对与编号位置在X轴方向的偏移量。";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
					//规格标注位置Y坐标偏移值
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis Y");
					lpInfo->m_strPropHelp = "specifications dimension position offset value relative to number position in axis Y direction";
#else
					lpInfo->m_strPropName.Format("Y轴方向");
					lpInfo->m_strPropHelp = "规格标注位置相对与编号位置在Y轴方向的偏移量。";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				}
				if(pFirstCmd->m_paraPartSpec.bDrawRefLine)
				{	//数据点位置偏移量 
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("data point position offset");
					lpInfo->m_strPropHelp = "data point offset value";
#else
					lpInfo->m_strPropName.Format("数据点位置偏移量");
					lpInfo->m_strPropHelp = "数据点位置偏移量。";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.BasePosOffset");
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//标注位置X坐标偏移值
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis X");
					lpInfo->m_strPropHelp = "number dimension position offset value in axis X.";
#else
					lpInfo->m_strPropName.Format("X轴方向");
					lpInfo->m_strPropHelp = "编号标注位置X轴方向偏移量。";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
					//标注位置Y坐标偏移值
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis Y");
					lpInfo->m_strPropHelp = "number dimension position offset in axis Y direction";
#else
					lpInfo->m_strPropName.Format("Y轴方向");
					lpInfo->m_strPropHelp = "编号标注位置Y轴方向偏移量。";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				}
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fPoleDimPosCoef")==pItem->m_idProp)//杆件标注位置系数 //LTMA 0.3  TAS 0.5
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				(*ppSonCmd)->m_paraPartSpec.fPoleDimPosCoef=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iMirMsg")==pItem->m_idProp)//0:X轴对称；1:Y轴对称;其余不对称处理
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				if((*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				int iOldMirMsg=(*ppSonCmd)->m_paraPartSpec.iMirMsg;
				(*ppSonCmd)->m_paraPartSpec.iMirMsg=valueStr[0]-'0';
				if(iOldMirMsg!=(*ppSonCmd)->m_paraPartSpec.iMirMsg)	//修改对称方式后重新标注该构件
					(*ppSonCmd)->Rerun();	//删除命令生成的原有实体,重新生成
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimPoleSpec")==pItem->m_idProp)	//标注角钢长度
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				if((*ppSonCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				BOOL bOld=(*ppSonCmd)->m_paraPartSpec.bOnlyDimPartNo;
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
					(*ppSonCmd)->m_paraPartSpec.bOnlyDimPartNo=FALSE;
				else 
					(*ppSonCmd)->m_paraPartSpec.bOnlyDimPartNo=TRUE;
				if(bOld!=(*ppSonCmd)->m_paraPartSpec.bOnlyDimPartNo)	//修改对称方式后重新标注该构件
					(*ppSonCmd)->Rerun();			//删除命令生成的原有实体,重新生成
			}
		}
	}
	else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
	{	//杆件螺栓信息或法兰信息
		if(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.iDimPosType")==pItem->m_idProp)
		{
			int iDimPosType=0;
			CDrawingCommand *pFirstCmd=NULL;
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
					continue;
				if(pFirstCmd==NULL)
				{
					pFirstCmd=*ppSonCmd;
					iDimPosType=pFirstCmd->m_paraBoltOrFlInfo.iDimPosType;
				}
				(*ppSonCmd)->m_paraBoltOrFlInfo.iDimPosType=atoi(valueStr);
			}
			if(pFirstCmd&&pFirstCmd->m_paraBoltOrFlInfo.iDimPosType!=iDimPosType)
				UpdateRodStartEndInfoPropList(pPropList,pItem->m_pParent,pFirstCmd);
		}
		else if(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX")==pItem->m_idProp)
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
					continue;
				(*ppSonCmd)->m_paraBoltOrFlInfo.fStartOddPosOffsetX=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY")==pItem->m_idProp)
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
					continue;
				(*ppSonCmd)->m_paraBoltOrFlInfo.fStartOddPosOffsetY=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX")==pItem->m_idProp)
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
					continue;
				(*ppSonCmd)->m_paraBoltOrFlInfo.fEndOddPosOffsetX=atof(valueStr);
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetY")==pItem->m_idProp)
		{
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
				if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
					continue;
				CDrawingCommand **ppSonCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
				if(ppSonCmd==NULL||(*ppSonCmd)->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
					continue;
				(*ppSonCmd)->m_paraBoltOrFlInfo.fEndOddPosOffsetY=atof(valueStr);
			}
		}
	}
	return TRUE;
}

static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pPropList->GetParent();
	if(pScriptDlg==NULL)
		return FALSE;
	char tem_str[100]="";
	CItemInfo *lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL;

	int iCurGroup=pScriptDlg->GetCurCmdGroup();
	if(pScriptDlg->IsSpreadLinePropGroup(iCurGroup)&&pScriptDlg->m_pSpreadLine)
	{	//展开线
		if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iDefType")==pItem->m_idProp)		
		{	//0.指定杆件 1.直接指定
			pScriptDlg->m_pSpreadLine->m_iDefType=valueStr[0]-'0';
			pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole"));
			pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing"));
			pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_fOffsetDist"));
			CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE"),NULL);
			if(pScriptDlg->m_pSpreadLine->m_iDefType==0&&pParentItem)
			{	//基准杆件 基准杆件句柄为0时表示为自定义
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("datum angle");
				lpInfo->m_strPropHelp = "spread axis positioning datum angle.";
#else
				lpInfo->m_strPropName.Format("基准角钢");
				lpInfo->m_strPropHelp = "展开线定位基准角钢。";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,TRUE);
				pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole");
				if(pScriptDlg->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				//基准杆件为角钢时启用该属性
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="0.X Leg|1.Y Leg|";
				lpInfo->m_strPropName.Format("locating leg");
				lpInfo->m_strPropHelp = "spread axis locating leg of datum angle.";
#else
				lpInfo->m_cmbItems="0.X肢|1.Y肢|";
				lpInfo->m_strPropName.Format("定位肢");
				lpInfo->m_strPropHelp = "展开线基准角钢定位肢。";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,3,TRUE);
				pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing");
				if(pScriptDlg->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				//偏移距离
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("offset");
				lpInfo->m_strPropHelp = "distance between offset line and angle's ridge.";
#else
				lpInfo->m_strPropName.Format("偏移距离");
				lpInfo->m_strPropHelp = "偏移线与角钢楞线之间的距离。";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,4,TRUE);
				pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_fOffsetDist");
				if(pScriptDlg->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
			}
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.x"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.y"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.z"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.x"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.y"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
			pPropList->SetItemReadOnly(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.z"),pScriptDlg->m_pSpreadLine->m_iDefType==0);
		}
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_sLineName")==pItem->m_idProp)
		{
			pScriptDlg->m_pSpreadLine->m_sLineName.Printf("%s",valueStr);//展开线名称
			//更新右侧命令列表命令名
			int iCurSel=pScriptDlg->m_listCmdSuperCtrl.GetSelectedItem();
			CSuperGridCtrl::CTreeItem *pCurItem=NULL;
			pCurItem=pScriptDlg->m_listCmdSuperCtrl.GetTreeItem(iCurSel);
			if(pCurItem==NULL||(pCurItem&&pCurItem->m_idProp!=(long)pScriptDlg->m_pSpreadLine))
				pCurItem=pScriptDlg->m_listCmdSuperCtrl.FindTreeItemById((long)pScriptDlg->m_pSpreadLine);
			if(pCurItem&&pCurItem->m_lpNodeInfo)
				pScriptDlg->m_listCmdSuperCtrl.SetSubItemText(pCurItem,2,valueStr);
		}
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole")==pItem->m_idProp)
		{	//基准杆件 基准杆件句柄为0时表示为自定义
			sscanf(valueStr,"%X",&pScriptDlg->m_pSpreadLine->m_hDatumPole);
			CLDSLinePart *pLinePart=(CLDSLinePart*)Ta.FromPartHandle(pScriptDlg->m_pSpreadLine->m_hDatumPole,CLS_LINEPART);
			if(pLinePart==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("handle input is not legitimate part, please input again!");
#else
				AfxMessageBox("输入句柄不是合法杆件请重新输入!");
#endif
				::SetFocus(pPropList->GetSafeHwnd());	//属性栏获得焦点
				pPropList->SetCurSel(pItem->m_iIndex);	//选中当前项
				return FALSE;
			}
			if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("temporarily only support the situation that datum parts are angles, please input handle again!");
#else
				AfxMessageBox("暂时仅支持基准杆件为角钢的情况，请重新输入句柄!");
#endif
				::SetFocus(pPropList->GetSafeHwnd());	//属性栏获得焦点
				pPropList->SetCurSel(pItem->m_iIndex);		//选中当前项
				return FALSE;
			}
		}
		if((CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing")==pItem->m_idProp)
			||(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_fOffsetDist")==pItem->m_idProp))
		{
			if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing")==pItem->m_idProp)
				pScriptDlg->m_pSpreadLine->m_iCurWing=valueStr[0]-'0';	//基准杆件为角钢时启用该属性	
			else //if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_fOffsetDist")==pItem->m_idProp)	
				pScriptDlg->m_pSpreadLine->m_fOffsetDist=atof(valueStr);//偏移距离
			pScriptDlg->UpdateSpreadLinePos();
			char tem_str[100]="";
			long id=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.x");
			if(pScriptDlg->GetPropValueStr(id,tem_str)>0)
				pScriptDlg->m_propList.SetItemPropValue(id,CString(tem_str));
			id=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.y");
			if(pScriptDlg->GetPropValueStr(id,tem_str)>0)
				pScriptDlg->m_propList.SetItemPropValue(id,CString(tem_str));
			id=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.z");
			if(pScriptDlg->GetPropValueStr(id,tem_str)>0)
				pScriptDlg->m_propList.SetItemPropValue(id,CString(tem_str));
			id=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.x");
			if(pScriptDlg->GetPropValueStr(id,tem_str)>0)
				pScriptDlg->m_propList.SetItemPropValue(id,CString(tem_str));
			id=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.y");
			if(pScriptDlg->GetPropValueStr(id,tem_str)>0)
				pScriptDlg->m_propList.SetItemPropValue(id,CString(tem_str));
			id=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.z");
			if(pScriptDlg->GetPropValueStr(id,tem_str)>0)
				pScriptDlg->m_propList.SetItemPropValue(id,CString(tem_str));
		}
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.x")==pItem->m_idProp)
			pScriptDlg->m_pSpreadLine->m_lineSpread.startPt.x=atof(valueStr);//展开线起点X坐标
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.y")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.startPt.y=atof(valueStr);//展开线起点Y坐标
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.z")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.startPt.z=atof(valueStr);//展开线起点Z坐标
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.x")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.endPt.x=atof(valueStr);	//展开线终点X坐标
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.y")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.endPt.y=atof(valueStr);	//展开线终点Y坐标
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.z")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.endPt.z=atof(valueStr);	//展开线终点Z坐标
	}
	else if(pScriptDlg->m_pCurCmd)
	{	//命令参数
		CDrawingCommand **ppCmd=NULL;
		long idNodeIndexStart=-1,idNodeIndexEnd=-1;
		idNodeIndexStart=CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.Node0");
		idNodeIndexEnd=CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.Node%d",PROPID_SETSIZE-1);
		if(CDrawMapScriptDlg::GetPropID("m_eCmdType")==pItem->m_idProp)
		{	//命令类型
		}
		else if(CDrawMapScriptDlg::GetPropID("m_stateCmd")==pItem->m_idProp)		
		{	//命令状态
		}
		//通过右键菜单进行修改
		/*else if(CDrawMapScriptDlg::GetPropID("m_bExecute")==pItem->m_idProp)	
		{	//是否执行该命令
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
					(*ppCmd)->m_bExecute=TRUE;
				else 
					(*ppCmd)->m_bExecute=FALSE;
			}
		}*/
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
		{	//构件绘制参数
			if(CDrawMapScriptDlg::GetPropID("PART.angle.extendStyle")==pItem->m_idProp)//角钢肢宽拓展方式
			{
				//pScriptDlg->m_pCurCmd->m_paraPart.para.angle.extendStyle=valueStr[0]-'0';
				for(CDrawingCommand **ppCurCmd=pScriptDlg->cmdList.GetFirst();ppCurCmd;ppCurCmd=pScriptDlg->cmdList.GetNext())
					(*ppCurCmd)->m_paraPart.para.angle.extendStyle=valueStr[0]-'0';
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.angle.bDrawAxisLine")==pItem->m_idProp)//角钢肢宽拓展方式
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("是")==0)
#endif
				{
					//pScriptDlg->m_pCurCmd->m_paraPart.para.angle.bDrawAxisLine=TRUE;
					for(CDrawingCommand **ppCurCmd=pScriptDlg->cmdList.GetFirst();ppCurCmd;ppCurCmd=pScriptDlg->cmdList.GetNext())
						(*ppCurCmd)->m_paraPart.para.angle.bDrawAxisLine=TRUE;
				}
				else
				{
					//pScriptDlg->m_pCurCmd->m_paraPart.para.angle.bDrawAxisLine=FALSE;
					for(CDrawingCommand **ppCurCmd=pScriptDlg->cmdList.GetFirst();ppCurCmd;ppCurCmd=pScriptDlg->cmdList.GetNext())
						(*ppCurCmd)->m_paraPart.para.angle.bDrawAxisLine=FALSE;
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.fPoleAxisLineStartOutSpreadLen")==pItem->m_idProp)//杆件心线始端延伸长度
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					CDrawingCommand *pCurCmd=*ppCmd;
					if(pCurCmd->m_eCmdType!=CMD_DRAW_PART)
						continue;
					if((*ppCmd)->m_paraPart.pPart==NULL||!(*ppCmd)->m_paraPart.pPart->IsAngle())
						continue;
					CDrawingCommand::PART_PARA *pPartPara=&(pCurCmd->m_paraPart);
					double fOldStartLen=pCurCmd->m_paraPart.fPoleAxisLineStartOutSpreadLen;
					pCurCmd->m_paraPart.fPoleAxisLineStartOutSpreadLen=atof(valueStr);
					if(fabs(fOldStartLen-pPartPara->fPoleAxisLineStartOutSpreadLen)>0	//始端延伸量有变动
						&&pPartPara->pPart&&pPartPara->pPart->IsLinePart())				//当前构件为杆件
					{	//调整杆件心线长度
						double fLen=pPartPara->fPoleAxisLineStartOutSpreadLen-fOldStartLen;
						fLen*=pCurCmd->m_pDrawingCom->m_fScale;
						AcDbObjectId *pEntId=pCurCmd->m_pDrawingCom->keyEntIdList.GetValue(pPartPara->hPart);
						if(pEntId==NULL)
							continue;
						AcDbEntity *pEnt=NULL;
						acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite);
						if(pEnt&&pEnt->isKindOf(AcDbLine::desc()))
						{
							AcDbLine *pLine=(AcDbLine*)pEnt;
							f3dPoint start_pt,end_pt;
							Cpy_Pnt(start_pt,pLine->startPoint());
							Cpy_Pnt(end_pt,pLine->endPoint());
							f3dPoint len_vec=start_pt-end_pt;
							normalize(len_vec);
							start_pt+=len_vec*fLen;
							pLine->setStartPoint(AcGePoint3d(start_pt.x,start_pt.y,start_pt.z));
							pLine->close();	
						}
						else if(pEnt)
							pEnt->close();
					}
				}
				//更新界面
				actrTransactionManager->flushGraphics(); //关闭打开的实体后才可成功刷新缓存
				acedUpdateDisplay();
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.fPoleAxisLineEndOutSpreadLen")==pItem->m_idProp)	//杆件心线终端延伸长度
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					CDrawingCommand *pCurCmd=*ppCmd;
					if(pCurCmd->m_eCmdType!=CMD_DRAW_PART)
						continue;
					if((*ppCmd)->m_paraPart.pPart==NULL||!(*ppCmd)->m_paraPart.pPart->IsAngle())
						continue;
					CDrawingCommand::PART_PARA *pPartPara=&(pCurCmd->m_paraPart);
					double fOldEndLen=pCurCmd->m_paraPart.fPoleAxisLineEndOutSpreadLen;
					pCurCmd->m_paraPart.fPoleAxisLineEndOutSpreadLen=atof(valueStr);
					if(fabs(fOldEndLen-pPartPara->fPoleAxisLineEndOutSpreadLen)>0	//始端延伸量有变动
						&&pPartPara->pPart&&pPartPara->pPart->IsLinePart())			//当前构件为杆件
					{	//调整杆件心线长度
						double fLen=pPartPara->fPoleAxisLineEndOutSpreadLen-fOldEndLen;
						fLen*=pCurCmd->m_pDrawingCom->m_fScale;
						AcDbObjectId *pEntId=pCurCmd->m_pDrawingCom->keyEntIdList.GetValue(pPartPara->hPart);
						if(pEntId==NULL)
							continue;
						AcDbEntity *pEnt=NULL;
						acdbOpenObject(pEnt,*pEntId,AcDb::kForWrite);
						if(pEnt&&pEnt->isKindOf(AcDbLine::desc()))
						{
							AcDbLine *pLine=(AcDbLine*)pEnt;
							f3dPoint start_pt,end_pt;
							Cpy_Pnt(start_pt,pLine->startPoint());
							Cpy_Pnt(end_pt,pLine->endPoint());
							f3dPoint len_vec=end_pt-start_pt;
							normalize(len_vec);
							end_pt+=len_vec*fLen;
							pLine->setEndPoint(AcGePoint3d(end_pt.x,end_pt.y,end_pt.z));
							pLine->close();
						}
						else if(pEnt)
							pEnt->close();
					}
				}
				//更新界面
				actrTransactionManager->flushGraphics(); //关闭打开的实体后才可成功刷新缓存
				acedUpdateDisplay();
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.bolt.bDrawSideBoltLine")==pItem->m_idProp)	//杆件心线终端延伸长度
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					CDrawingCommand *pCurCmd=*ppCmd;
					if(pCurCmd->m_eCmdType!=CMD_DRAW_PART)
						continue;
					if((*ppCmd)->m_paraPart.pPart==NULL||(*ppCmd)->m_paraPart.pPart->GetClassTypeId()!=CLS_BOLT)
						continue;
					if(valueStr[0]=='0')
						pCurCmd->m_paraPart.para.bolt.bDrawSideBoltLine=false;
					else
						pCurCmd->m_paraPart.para.bolt.bDrawSideBoltLine=true;
				}
				//更新界面
				actrTransactionManager->flushGraphics(); //关闭打开的实体后才可成功刷新缓存
				acedUpdateDisplay();
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.bolt.bDimHoleDiameter")==pItem->m_idProp)	//杆件心线终端延伸长度
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					CDrawingCommand *pCurCmd=*ppCmd;
					if(pCurCmd->m_eCmdType!=CMD_DRAW_PART)
						continue;
					if((*ppCmd)->m_paraPart.pPart==NULL||(*ppCmd)->m_paraPart.pPart->GetClassTypeId()!=CLS_BOLT||!(*ppCmd)->m_paraPart.pPart->m_bVirtualPart)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						pCurCmd->m_paraPart.para.bolt.bDimHoleDiameter=true;
					else
						pCurCmd->m_paraPart.para.bolt.bDimHoleDiameter=false;
					pCurCmd->Rerun();
				}
				//更新界面
				actrTransactionManager->flushGraphics(); //关闭打开的实体后才可成功刷新缓存
				acedUpdateDisplay();
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.cDrawFlag")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
						continue;
					if(valueStr[0]=='X'||valueStr[0]=='Y'||valueStr[0]=='Z')
						(*ppCmd)->m_paraPart.cDrawFlag=valueStr[0];
					else
						(*ppCmd)->m_paraPart.cDrawFlag='0';
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.angle.nXWingDatumAxisG")==pItem->m_idProp)//X肢基准心距
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_PART)
						continue;
					if((*ppCmd)->m_paraPart.pPart==NULL||!(*ppCmd)->m_paraPart.pPart->IsAngle())
						continue;
					(*ppCmd)->m_paraPart.para.angle.nXWingDatumAxisG=atoi(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.angle.nYWingDatumAxisG")==pItem->m_idProp)//Y肢基准心距
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
					(*ppCmd)->m_paraPart.para.angle.nYWingDatumAxisG=atoi(valueStr);
			}
			else 
				return ModifySonItemValue(pPropList,pItem,valueStr);
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_LINEPART)
		{	//绘制杆件单线参数
			if(CDrawMapScriptDlg::GetPropID("LINEPART.bSum")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_LINEPART)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraLinePart.bSumMap=TRUE;
					else 
						(*ppCmd)->m_paraLinePart.bSumMap=FALSE;
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("LINEPART.bDimByAnalysisMode")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_LINEPART)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraLinePart.bDimByAnalysisMode=TRUE;
					else 
						(*ppCmd)->m_paraLinePart.bDimByAnalysisMode=FALSE;
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_LINEPART)
		{	//表格绘制参数
			if(CDrawMapScriptDlg::GetPropID("TABLE.nLocatePos")==pItem->m_idProp)	//0.左上 1.右上 2.左下 3.右下
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
					(*ppCmd)->m_paraBOMTable.nLocatePos=valueStr[0]-'0';
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.bSelectPos")==pItem->m_idProp)	//选择表格位置
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraBOMTable.bSelectPos=TRUE;
					else 
						(*ppCmd)->m_paraBOMTable.bSelectPos=FALSE;
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.bDisplayDlg")==pItem->m_idProp)	//显示选择定位点的对话框
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraBOMTable.bDisplayDlg=TRUE;
					else 
						(*ppCmd)->m_paraBOMTable.bDisplayDlg=FALSE;
				}
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("TABLE.nLocatePos"));
				if(!pScriptDlg->m_pCurCmd->m_paraBOMTable.bDisplayDlg)
				{	//表格定位位置
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_cmbItems="0.Upper Left|1.Upper Right|2.Lower Left|3.Lower Right|";
					lpInfo->m_strPropName.Format("table positioning location");
					lpInfo->m_strPropHelp = "table positioning location";
#else
					lpInfo->m_cmbItems="0.左上|1.右上|2.左下|3.右下|";
					lpInfo->m_strPropName.Format("表格定位位置");
					lpInfo->m_strPropHelp = "表格定位位置";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("TABLE.nLocatePos");
					if(pScriptDlg->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.fDimPosX")==pItem->m_idProp)//表格位置X坐标		
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
					(*ppCmd)->m_paraBOMTable.fDimPosX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.fDimPosY")==pItem->m_idProp)//表格位置Y坐标
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
					(*ppCmd)->m_paraBOMTable.fDimPosY=atof(valueStr);
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
		{	//构件明细标注参数
			if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX")==pItem->m_idProp)	//标注位置X坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fBasePosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY")==pItem->m_idProp)	//标注位置Y坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fBasePosOffsetY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX")==pItem->m_idProp)	//标注位置X坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fDimPosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY")==pItem->m_idProp)	//标注位置Y坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fDimPosOffsetY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX")==pItem->m_idProp)	//规格标注位置X坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fGuigeOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY")==pItem->m_idProp)	//规格标注位置Y坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fGuigeOffsetY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iDimPosType")==pItem->m_idProp)		//0.自动搜索位置 1.指定位置(保存指定位置与原标注位置的偏移值)  2.选择位置
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.iDimPosType=valueStr[0]-'0';
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDrawRefLine")==pItem->m_idProp)//是否绘制引出线
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					BOOL bOldDrawRefLine=(*ppCmd)->m_paraPartSpec.bDrawRefLine;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraPartSpec.bDrawRefLine=TRUE;
					else 
						(*ppCmd)->m_paraPartSpec.bDrawRefLine=FALSE;
					if(bOldDrawRefLine!=(*ppCmd)->m_paraPartSpec.bDrawRefLine)
						(*ppCmd)->Rerun();	//删除命令生成的原有实体,重新生成 wht 11-07-18
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fPoleDimPosCoef")==pItem->m_idProp)//杆件标注位置系数 //LTMA 0.3  TAS 0.5
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fPoleDimPosCoef=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iMirMsg")==pItem->m_idProp)//0:X轴对称；1:Y轴对称;其余不对称处理
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					int iOldMirMsg=(*ppCmd)->m_paraPartSpec.iMirMsg;
					(*ppCmd)->m_paraPartSpec.iMirMsg=valueStr[0]-'0';
					if(iOldMirMsg!=(*ppCmd)->m_paraPartSpec.iMirMsg)	//修改对称方式后重新标注该构件
						(*ppCmd)->Rerun();	//删除命令生成的原有实体,重新生成
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimPoleSpec")==pItem->m_idProp)	//仅标注编号
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					BOOL bOld=(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo=FALSE;
					else 
						(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo=TRUE;
					if(bOld!=(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo)
						(*ppCmd)->Rerun();			//删除命令生成的原有实体,重新生成
				}
			}
			else 
				return ModifySonItemValue(pPropList,pItem,valueStr);
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SEGI)
		{	//标注段号
			if(CDrawMapScriptDlg::GetPropID("DIMSEGI.bSelectPos")==pItem->m_idProp)	//选择表格位置
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SEGI)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraDimSegI.bSelectPos=TRUE;
					else 
						(*ppCmd)->m_paraDimSegI.bSelectPos=FALSE;
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSEGI.nSegI")==pItem->m_idProp)		//关联段号
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SEGI)
						continue;
					(*ppCmd)->m_paraDimSegI.nSegI=SEGI(valueStr.GetBuffer());
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSEGI.fDimPosX")==pItem->m_idProp)	//标注位置X坐标
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SEGI)
						continue;
					(*ppCmd)->m_paraDimSegI.fDimPosX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSEGI.fDimPosY")==pItem->m_idProp)	//标注位置Y坐标
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SEGI)
						continue;
					(*ppCmd)->m_paraDimSegI.fDimPosY=atof(valueStr);
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_ODDMENT)
		{	//正负头标注参数
			if(CDrawMapScriptDlg::GetPropID("ODDMENT.iDimPosType")==pItem->m_idProp)
			{
				int iDimPosType=0;
				CDrawingCommand *pFirstCmd=NULL;
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ODDMENT)
						continue;
					if(pFirstCmd==NULL)
					{
						pFirstCmd=*ppCmd;
						iDimPosType=pFirstCmd->m_paraOddment.iDimPosType;
					}
					(*ppCmd)->m_paraOddment.iDimPosType=atoi(valueStr);
				}
				CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDrawMapScriptDlg::GetPropID("DrawPara"),NULL);
				if(pFirstCmd&&pFirstCmd->m_paraOddment.iDimPosType!=iDimPosType&&pParentItem)
				{	//更新属性显示界面
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.StartOddPosOffset"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetX"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetY"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.EndOddPosOffset"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetX"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetY"));
					if(pFirstCmd->m_paraOddment.iDimPosType==1)
					{	//始端正负头标注位置
						CPropTreeItem *pSonPropItem=NULL;
						if(pFirstCmd->m_paraOddment.bDimStartOdd)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "starting position offset";
							lpInfo->m_strPropHelp = "dimension position offset value of starting ends.";
#else
							lpInfo->m_strPropName = "始端位置偏移";
							lpInfo->m_strPropHelp = "始端正负头标注位置偏移量。";
#endif
							pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
							pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.StartOddPosOffset");
							if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
								pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
							//始端正负头标注位置X坐标
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of starting ends in axis X direction.";
#else
							lpInfo->m_strPropHelp = "始端正负头标注位置偏移量X坐标。";
#endif
							pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
							pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetX");
							if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
								pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
							pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
							//始端正负头标注位置Y坐标
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of starting ends in axis Y diection.";
#else
							lpInfo->m_strPropHelp = "始端正负头标注位置偏移量Y坐标。";
#endif
							pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
							pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetY");
							if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
								pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
							pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
						}
						if(pFirstCmd->m_paraOddment.bDimEndOdd)
						{	//终端正负头标注位置
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "ending position offset";
							lpInfo->m_strPropHelp = "dimension position offset value of ending's ends relative to dimension basis points.";
#else
							lpInfo->m_strPropName = "终端位置偏移";
							lpInfo->m_strPropHelp = "终端正负头标注位置相对于标注基点的偏移量。";
#endif
							pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
							pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.EndOddPosOffset");
							if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
								pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
							//终端正负头标注位置X坐标
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of ending's ends in axis X diection.";
#else
							lpInfo->m_strPropHelp = "终端正负头标注位置偏移量X坐标。";
#endif
							pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
							pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetX");
							if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
								pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
							pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
							//终端正负头标注位置Y坐标
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of ending's ends in axis Y diection.";
#else
							lpInfo->m_strPropHelp = "终端正负头标注位置偏移量Y坐标。";
#endif
							pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
							pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetY");
							if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
								pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
							pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
						}
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetX")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ODDMENT)
						continue;
					(*ppCmd)->m_paraOddment.fStartOddPosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetY")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ODDMENT)
						continue;
					(*ppCmd)->m_paraOddment.fStartOddPosOffsetY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetX")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ODDMENT)
						continue;
					(*ppCmd)->m_paraOddment.fEndOddPosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetY")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ODDMENT)
						continue;
					(*ppCmd)->m_paraOddment.fEndOddPosOffsetY=atof(valueStr);
				}
			}
		}
#ifdef __PART_DESIGN_INC_
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
		{	//构件明细标注参数
			if(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.iDimPosType")==pItem->m_idProp)		//0.自动搜索位置 1.指定位置
			{
				int iDimPosType=0;
				CDrawingCommand *pFirstCmd=NULL;
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ANGLEJOINT)
						continue;
					if(pFirstCmd==NULL)
					{
						pFirstCmd=*ppCmd;
						iDimPosType=pFirstCmd->m_paraAngleJoint.iDimPosType;
					}
					(*ppCmd)->m_paraAngleJoint.iDimPosType=atoi(valueStr);
				}
				CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDrawMapScriptDlg::GetPropID("DrawPara"),NULL);
				if(pFirstCmd&&pFirstCmd->m_paraAngleJoint.iDimPosType!=iDimPosType&&pParentItem)
				{	//更新属性显示界面
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.DimPosOffset"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetX"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetY"));
					//
					if(pFirstCmd->m_paraAngleJoint.iDimPosType==1)
					{
						CPropTreeItem *pSonPropItem=NULL;
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName.Format("dimension position offset");
						lpInfo->m_strPropHelp = "dimension position offset value.";
#else
						lpInfo->m_strPropName.Format("标注位置偏移量");
						lpInfo->m_strPropHelp = "标注位置偏移量。";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
						pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ANGLEJOINT.DimPosOffset");
						if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
							pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
						//标注位置X坐标
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position X coordinate.";
#else
						lpInfo->m_strPropHelp = "标注位置X坐标。";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetX");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp,3);
						//标注位置Y坐标
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position Y coordinate.";
#else
						lpInfo->m_strPropHelp = "标注位置Y坐标。";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetY");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp,3);
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetX")==pItem->m_idProp)	//标注位置X坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ANGLEJOINT)
						continue;
					double fOld=(*ppCmd)->m_paraAngleJoint.fDimPosOffsetX;
					(*ppCmd)->m_paraAngleJoint.fDimPosOffsetX=atof(valueStr);
					if(fabs(fOld-(*ppCmd)->m_paraAngleJoint.fDimPosOffsetX)>EPS)
						(*ppCmd)->Rerun();
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetY")==pItem->m_idProp)	//标注位置Y坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_ANGLEJOINT)
						continue;
					double fOld=(*ppCmd)->m_paraAngleJoint.fDimPosOffsetY;
					(*ppCmd)->m_paraAngleJoint.fDimPosOffsetY=atof(valueStr);
					if(fabs(fOld-(*ppCmd)->m_paraAngleJoint.fDimPosOffsetY)>EPS)
						(*ppCmd)->Rerun();
				}
			}
		}
#endif
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_BOLTPAD)
		{	//垫圈标注参数
			if(CDrawMapScriptDlg::GetPropID("BOLTPAD.iDimPosType")==pItem->m_idProp)
			{
				int iDimPosType=0;
				CDrawingCommand *pFirstCmd=NULL;
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_BOLTPAD)
						continue;
					if(pFirstCmd==NULL)
					{
						pFirstCmd=*ppCmd;
						iDimPosType=pFirstCmd->m_paraBoltPad.iDimPosType;
					}
					(*ppCmd)->m_paraBoltPad.iDimPosType=atoi(valueStr);
				}
				CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDrawMapScriptDlg::GetPropID("DrawPara"),NULL);
				if(pFirstCmd&&pFirstCmd->m_paraBoltPad.iDimPosType!=iDimPosType&&pParentItem)
				{	//更新属性显示界面
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("BOLTPAD.DimPosOffset"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetX"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetY"));
					//
					if(pFirstCmd->m_paraBoltPad.iDimPosType==1)
					{
						CPropTreeItem *pSonPropItem=NULL;
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName.Format("dimension position offset");
						lpInfo->m_strPropHelp = "dimension position offset value.";
#else
						lpInfo->m_strPropName.Format("标注位置偏移量");
						lpInfo->m_strPropHelp = "标注位置偏移量。";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
						pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTPAD.DimPosOffset");
						if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
							pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
						//标注位置X坐标
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position X coordinate.";
#else
						lpInfo->m_strPropHelp = "标注位置X坐标。";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetX");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
						//标注位置Y坐标
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position Y coordinate.";
#else
						lpInfo->m_strPropHelp = "标注位置Y坐标。";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetY");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetX")==pItem->m_idProp)//标注位置X坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_BOLTPAD)
						continue;
					double fOld=(*ppCmd)->m_paraBoltPad.fDimPosOffsetX;
					(*ppCmd)->m_paraBoltPad.fDimPosOffsetX=atof(valueStr);
					if(fabs(fOld-(*ppCmd)->m_paraBoltPad.fDimPosOffsetX)>EPS)
						(*ppCmd)->Rerun();
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetY")==pItem->m_idProp)//标注位置Y坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_BOLTPAD)
						continue;
					double fOld=(*ppCmd)->m_paraBoltPad.fDimPosOffsetY;
					(*ppCmd)->m_paraBoltPad.fDimPosOffsetY=atof(valueStr);
					if(fabs(fOld-(*ppCmd)->m_paraBoltPad.fDimPosOffsetY)>EPS)
						(*ppCmd)->Rerun();
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
		{	//螺栓标注参数
			if(CDrawMapScriptDlg::GetPropID("BOLTINFO.iDimPosType")==pItem->m_idProp)
			{
				int iDimPosType=0;
				CDrawingCommand *pFirstCmd=NULL;
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_BOLTINFO)
						continue;
					if(pFirstCmd==NULL)
					{
						pFirstCmd=*ppCmd;
						iDimPosType=pFirstCmd->m_paraBoltGroupInfo.iDimPosType;
					}
					(*ppCmd)->m_paraBoltGroupInfo.iDimPosType=atoi(valueStr);
				}
				CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDrawMapScriptDlg::GetPropID("DrawPara"),NULL);
				if(pFirstCmd&&pFirstCmd->m_paraBoltGroupInfo.iDimPosType!=iDimPosType&&pParentItem)
				{	//更新属性显示界面
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("BOLTINFO.DimPosOffset"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetX"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetY"));
					//
					if(pFirstCmd->m_paraBoltGroupInfo.iDimPosType==1)
					{
						CPropTreeItem *pSonPropItem=NULL;
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropName.Format("dimension position offset");
						lpInfo->m_strPropHelp = "dimension position offset value.";
#else
						lpInfo->m_strPropName.Format("标注位置偏移量");
						lpInfo->m_strPropHelp = "标注位置偏移量。";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
						pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTINFO.DimPosOffset");
						if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
							pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
						//标注位置X坐标
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position X coordinate. ";
#else
						lpInfo->m_strPropHelp = "标注位置X坐标。";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetX");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
						//标注位置Y坐标
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position Y coordinate.";
#else
						lpInfo->m_strPropHelp = "标注位置Y坐标。";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetY");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetX")==pItem->m_idProp)//标注位置X坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_BOLTINFO)
						continue;
					double fOld=(*ppCmd)->m_paraBoltGroupInfo.fDimPosOffsetX;
					(*ppCmd)->m_paraBoltGroupInfo.fDimPosOffsetX=atof(valueStr);
					if(fabs(fOld-(*ppCmd)->m_paraBoltGroupInfo.fDimPosOffsetX)>EPS)
						(*ppCmd)->Rerun();
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetY")==pItem->m_idProp)//标注位置Y坐标偏移值
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_BOLTINFO)
						continue;
					double fOld=(*ppCmd)->m_paraBoltGroupInfo.fDimPosOffsetY;
					(*ppCmd)->m_paraBoltGroupInfo.fDimPosOffsetY=atof(valueStr);
					if(fabs(fOld-(*ppCmd)->m_paraBoltGroupInfo.fDimPosOffsetY)>EPS)
						(*ppCmd)->Rerun();
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY)
		{	//相似形标注参数
			if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fScale")==pItem->m_idProp)	//指定相似形比例
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					double fOldScale=(*ppCmd)->m_paraSimPoly.fScale;
					(*ppCmd)->m_paraSimPoly.fScale=atof(valueStr);
					if(fabs(fOldScale-(*ppCmd)->m_paraSimPoly.fScale)>0)
					{
						if((*ppCmd)->m_paraSimPoly.nSimPolyCmdType==2)
						{	//通过拾取点标注的相似形，需要事先设置好图层
						#ifdef _ARX_2007
							acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
							acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
							acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
							CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
							acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
							acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
						#else
							acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
							acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
							acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
							CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
							acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
							acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
						#endif
						}
						(*ppCmd)->Rerun();	
						if((*ppCmd)->m_paraSimPoly.nSimPolyCmdType==2)
						{	//通过拾取点标注的相似形，执行完标注相似形命令后回复原有图层状态
						#ifdef _ARX_2007
							acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
							acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
                        #else
							acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
							acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
                        #endif
						}
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fPickPtX")==pItem->m_idProp)	//相似形拾取点X坐标
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fPickPtX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fPickPtY")==pItem->m_idProp)	//相似形拾取点Y坐标
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fPickPtY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fDimPosOffsetX")==pItem->m_idProp)//相似形位置X坐标		
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fDimPosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fDimPosOffsetY")==pItem->m_idProp)//相似形位置Y坐标
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fDimPosOffsetY=atof(valueStr);
				}
			}
			//相似形轮廓点参数 放在最后
			else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY
				&&pScriptDlg->m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum())
			{
				int i=1;
				BOOL bRerun=FALSE;
				SIMPOLY_VERTEX *pVertex=NULL;
				for(pVertex=pScriptDlg->m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
				pVertex=pScriptDlg->m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetNext())
				{
					char sKey1[50]="",sKey2[50]="",sKey3[50]="",sKey4[50]="";
					sprintf(sKey1,"SIMPOLY.vertex%d.hPrevPole",i);
					sprintf(sKey2,"SIMPOLY.vertex%d.hNextPole",i);
					sprintf(sKey3,"SIMPOLY.vertex%d.x",i);
					sprintf(sKey4,"SIMPOLY.vertex%d.y",i);
					if(CDrawMapScriptDlg::GetPropID(sKey1)==pItem->m_idProp)
					{
						sscanf(valueStr,"%X",&pVertex->hDatumRod);
						break;
					}
					else if(CDrawMapScriptDlg::GetPropID(sKey2)==pItem->m_idProp)
					{
						sscanf(valueStr,"%X",&pVertex->hViceRod);
						break;
					}
					else if(CDrawMapScriptDlg::GetPropID(sKey3)==pItem->m_idProp)
					{
						pVertex->vertex.x=atof(valueStr);
						bRerun=TRUE;
						break;
					}
					else if(CDrawMapScriptDlg::GetPropID(sKey4)==pItem->m_idProp)
					{
						pVertex->vertex.y=atof(valueStr);
						bRerun=TRUE;
						break;
					}
					i++;
				}
				if(bRerun)
				{	//修改顶点位置后重新计算几何中心位置，然后重新生成相似形 wht 11-07-29
					f3dPoint geom_center;
					SIMPOLY_VERTEX *pVertex=NULL;
					for(pVertex=pScriptDlg->m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetFirst();
					pVertex;pVertex=pScriptDlg->m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetNext())
					{
						geom_center+=pVertex->vertex;
					}
					int nVertexNum=pScriptDlg->m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum();
					geom_center.x/=nVertexNum;
					geom_center.y/=nVertexNum;
					pScriptDlg->m_pCurCmd->m_paraSimPoly.fPickPtX=geom_center.x;
					pScriptDlg->m_pCurCmd->m_paraSimPoly.fPickPtY=geom_center.y;
					pScriptDlg->m_pCurCmd->Rerun();
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
		{	//标注模型尺寸
			if(CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MODELSIZE)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimModelSize.fDimDistance;
					(*ppCmd)->m_paraDimModelSize.fDimDistance=atof(valueStr)*(1.0/pScriptDlg->m_pComponent->m_fScale);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimModelSize.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//删除命令相关实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.fSpace")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MODELSIZE)
						continue;
					double fOldSpace=(*ppCmd)->m_paraDimModelSize.fSpace;
					(*ppCmd)->m_paraDimModelSize.fSpace=atof(valueStr)*(1.0/pScriptDlg->m_pComponent->m_fScale);
					if(fabs(fOldSpace-(*ppCmd)->m_paraDimModelSize.fSpace)>EPS2)
						(*ppCmd)->Rerun();	//删除命令相关实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.fMinX")==pItem->m_idProp)	
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MODELSIZE)
						continue;
					double fOldMinX=(*ppCmd)->m_paraDimModelSize.fMinX;
					(*ppCmd)->m_paraDimModelSize.fMinX=atof(valueStr)*(1.0/pScriptDlg->m_pComponent->m_fScale);
					if(fabs(fOldMinX-(*ppCmd)->m_paraDimModelSize.fMinX)>EPS2)
						(*ppCmd)->Rerun();	//删除命令相关实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.iDimType")==pItem->m_idProp)	
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MODELSIZE)
						continue;
					(*ppCmd)->m_paraDimModelSize.iDimType=valueStr[0]-'0';
				}
			}
			else if(pItem->m_idProp>=idNodeIndexStart&&pItem->m_idProp<=idNodeIndexEnd)
			{
				int nNodeNum=pScriptDlg->m_pCurCmd->m_paraDimModelSize.listDimNodes.GetNodeNum();
				long hNode=0;
				sscanf(valueStr,"%X",&hNode);
				CLDSNode *pNode=Ta.FromNodeHandle(hNode);
				for(int i=0;i<PROPID_SETSIZE;i++)
				{
					CXhChar50 sPropStr("DIMMODELSIZE.Node%d",i);
					if(i<nNodeNum&&CDrawMapScriptDlg::GetPropID(sPropStr)==pItem->m_idProp)	
					{
						if(pNode==NULL)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							AfxMessageBox("error in handle input, please input a valid node handle again!");
#else
							AfxMessageBox("输入句柄错误，请重新输入有效节点句柄！");
#endif
							return FALSE;
						}
						pScriptDlg->m_pCurCmd->m_paraDimModelSize.listDimNodes[i].pNode=pNode;
						break;
					}
				}
				SCOPE_STRU scope = pScriptDlg->m_pCurCmd->GetCADEntScope();
				ZoomAcadView(scope,100);
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_MAPSIZE)
		{	//图面尺寸
			if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimStartX")==pItem->m_idProp)//图面尺寸标注起始点(相对坐标系下)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fDimStartX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimStartY")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fDimStartY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimEndX")==pItem->m_idProp)	//图面尺寸标注起始点(相对坐标系下)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fDimEndX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimEndY")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fDimEndY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimPosX")==pItem->m_idProp)	//图面尺寸标注位置(相对坐标系下)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fDimPosX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimPosY")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fDimPosY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fScale")==pItem->m_idProp)	//尺寸比例
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fScale=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.iDimDirect")==pItem->m_idProp)//标注方向 0.X轴方向 1.Y轴方向 2.延伸方向
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.iDimDirect=valueStr[0]-'0';
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		{	//标注角钢特殊准据
			if(CDrawMapScriptDlg::GetPropID("DIM_JGZHUN.fSpace")==pItem->m_idProp)	
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
						continue;
					double fOldSpace=(*ppCmd)->m_paraDimJgZhun.fSpace;
					(*ppCmd)->m_paraDimJgZhun.fSpace=atof(valueStr);
					if(fabs(fOldSpace-(*ppCmd)->m_paraDimJgZhun.fSpace)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIM_JGZHUN.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimJgZhun.fDimDistance;
					(*ppCmd)->m_paraDimJgZhun.fDimDistance=atof(valueStr);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimJgZhun.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIM_JGZHUN.bInside")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
						continue;
					BOOL bInside=(*ppCmd)->m_paraDimJgZhun.bInside;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Internal")==0)
#else
					if(valueStr.CompareNoCase("内侧")==0)
#endif
						(*ppCmd)->m_paraDimJgZhun.bInside=TRUE;
					else 
						(*ppCmd)->m_paraDimJgZhun.bInside=FALSE;
					if((*ppCmd)->m_paraDimJgZhun.bInside!=bInside)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
		{	//标注螺栓特殊间距
			if(CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.fSpace")==pItem->m_idProp)	
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
						continue;
					double fOldSpace=(*ppCmd)->m_paraDimLsSpace.fSpace;
					(*ppCmd)->m_paraDimLsSpace.fSpace=atof(valueStr);
					if(fabs(fOldSpace-(*ppCmd)->m_paraDimLsSpace.fSpace)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimLsSpace.fDimDistance;
					(*ppCmd)->m_paraDimLsSpace.fDimDistance=atof(valueStr);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimLsSpace.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.bInside")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
						continue;
					BOOL bInside=(*ppCmd)->m_paraDimLsSpace.bInside;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Internal")==0)
#else
					if(valueStr.CompareNoCase("内侧")==0)
#endif
						(*ppCmd)->m_paraDimLsSpace.bInside=TRUE;
					else 
						(*ppCmd)->m_paraDimLsSpace.bInside=FALSE;
					if((*ppCmd)->m_paraDimLsSpace.bInside!=bInside)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.bMergeDimLsSpace")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
						continue;
					BOOL bMergeDimLsSpace=(*ppCmd)->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("是")==0)
#endif
						(*ppCmd)->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace=TRUE;
					else 
						(*ppCmd)->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace=FALSE;
					if((*ppCmd)->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace!=bMergeDimLsSpace)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
		{	//脚钉间距标注
			if(CDrawMapScriptDlg::GetPropID("FOOTNAIL_SPACE.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimFootNailSpace.fDimDistance;
					(*ppCmd)->m_paraDimFootNailSpace.fDimDistance=atof(valueStr);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimFootNailSpace.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("FOOTNAIL_SPACE.fSpace")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE)
						continue;
					double fOldSpace=(*ppCmd)->m_paraDimFootNailSpace.fSpace;
					(*ppCmd)->m_paraDimFootNailSpace.fSpace=atof(valueStr);
					if(fabs(fOldSpace-(*ppCmd)->m_paraDimFootNailSpace.fSpace)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
		{	//填板间距标注
			if(CDrawMapScriptDlg::GetPropID("FILLPLATE_SPACE.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimFillPlateSpace.fDimDistance;
					(*ppCmd)->m_paraDimFillPlateSpace.fDimDistance=atof(valueStr);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimFillPlateSpace.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("FILLPLATE_SPACE.fSpace")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE)
						continue;
					double fOldSpace=(*ppCmd)->m_paraDimFillPlateSpace.fSpace;
					(*ppCmd)->m_paraDimFillPlateSpace.fSpace=atof(valueStr);
					if(fabs(fOldSpace-(*ppCmd)->m_paraDimFillPlateSpace.fSpace)>EPS2)
						(*ppCmd)->Rerun();	//删除原有实体重新运行该命令 wht 11-06-20
				}
			}
		}
	}
	return TRUE;
}
//From DrawingCommand.cpp
BOOL IsRightToLeft(double x1,double x2,int nUnderlineLen);
struct TEXT_POSY{
	AcDbObjectId textId;
	double posY;
	TEXT_POSY(){textId=0; posY=0;}
	TEXT_POSY(AcDbObjectId id,double y){textId=id; posY=y;}
};
static BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pPropList->GetParent();
	if(pItem==NULL||pScriptDlg==NULL)
		return FALSE;
	int iCurGroup=pScriptDlg->GetCurCmdGroup();
	if(pScriptDlg->m_pDrawing&&pScriptDlg->m_pDrawing->m_nSegI.iSeg>=0&&	//结构图
		pScriptDlg->IsSpreadLinePropGroup(iCurGroup))		//展开线
	{	//展开线
		if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole")==pItem->m_idProp)
		{	//从界面上选择杆件
			pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
			pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
#ifdef AFX_TARG_ENU_ENGLISH
			pScriptDlg->m_arrCmdPickPrompt.Add("\nplease select an angle (choose center line of the angle)<Enter confirm>:\n");
#else
			pScriptDlg->m_arrCmdPickPrompt.Add("\n请选择一根角钢(选择角钢心线)<Enter确认>:\n");
#endif
			pScriptDlg->SelectEntObj();
		}
	}
	else
	{
		CDrawingCommand *pCmd=pScriptDlg->m_pCurCmd;
		if(pCmd==NULL)
			return FALSE;
		CDrawingCommand *pSonPartSpecCmd=NULL,*pSonBoltOrFlInfoCmd=NULL;
		if(pCmd->m_eCmdType==CMD_DRAW_PART)
		{
			CDrawingCommand **ppCmd=pScriptDlg->hashPartSpecList.GetValue(pCmd->m_paraPart.hPart);
			if(ppCmd)
				pSonPartSpecCmd=*ppCmd;
		}
		else if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)
		{
			CDrawingCommand **ppCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue(pCmd->m_paraPartSpec.hObj);
			if(ppCmd)
				pSonBoltOrFlInfoCmd=*ppCmd;
		}
		if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC||pSonPartSpecCmd||pSonBoltOrFlInfoCmd)
		{
			if(pSonBoltOrFlInfoCmd)
			{	//杆件螺栓或法兰标注信息
				//缩放至子命令对应的实体所在区域 wht 11-07-23
				SCOPE_STRU scope=pSonBoltOrFlInfoCmd->GetCADEntScope();
				ZoomAcadView(scope,20);
				pSonBoltOrFlInfoCmd->SetEntSetHighlight();
				//
				long startOddId=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset");
				long endOddId=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset");
				if((startOddId==pItem->m_idProp&&pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.bDimStartOdd)
					||(endOddId==pItem->m_idProp&&pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.bDimEndOdd))
				{	//杆件螺栓或法兰信息标注
					AcDbText *pFirstOddText=NULL,*pSecOddText=NULL;
					AcDbObjectId *pEntId=NULL,*pFirstEntId=NULL,*pSecEntId=NULL;
					for(pEntId=pSonBoltOrFlInfoCmd->entIdList.GetFirst();pEntId;pEntId=pSonBoltOrFlInfoCmd->entIdList.GetNext())
					{
						AcDbEntity *pEnt=NULL;
						acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
						if(pEnt==NULL)
							continue;
						pEnt->close();
						if(!pEnt->isKindOf(AcDbText::desc()))
							continue;
						if(pFirstOddText==NULL)
						{
							pFirstOddText=(AcDbText*)pEnt;
							pFirstEntId=pEntId;
						}
						else if(pSecOddText==NULL)
						{
							pSecOddText=(AcDbText*)pEnt;
							pSecEntId=pEntId;
						}
						else 
							break;
					}
					DRAGSET.ClearEntSet();
					f3dPoint startPt(pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddBasePosX+pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetX,pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddBasePosY+pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetY);
					f3dPoint endPt(pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddBasePosX+pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetX,pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddBasePosY+pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetY);
					if(pFirstOddText==NULL&&pSecOddText==NULL)
						return FALSE;
					else if(pSecOddText==NULL)
					{
						if(startOddId==pItem->m_idProp&&DISTANCE(startPt,pFirstOddText->position())<=DISTANCE(endPt,pFirstOddText->position()))
							DRAGSET.Add(*pFirstEntId);
						else if(endOddId==pItem->m_idProp&&DISTANCE(startPt,pFirstOddText->position())>=DISTANCE(endPt,pFirstOddText->position()))
							DRAGSET.Add(*pFirstEntId); 
						else
							return FALSE;
					}
					else 
					{
						if(startOddId==pItem->m_idProp)
						{
							if(DISTANCE(startPt,pFirstOddText->position())<DISTANCE(startPt,pSecOddText->position()))
								DRAGSET.Add(*pFirstEntId);
							else 
								DRAGSET.Add(*pSecEntId); 
						}
						else if(endOddId==pItem->m_idProp)
						{
							if(DISTANCE(endPt,pFirstOddText->position())<DISTANCE(endPt,pSecOddText->position()))
								DRAGSET.Add(*pFirstEntId);
							else 
								DRAGSET.Add(*pSecEntId);
						}
					}
					//
					pScriptDlg->HideWnd(TRUE);	
					f3dPoint basepnt;
					if(startOddId==pItem->m_idProp)
						basepnt=startPt;
					else 
						basepnt=endPt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please enter the dimension position\n");
#else
					CXhChar100 sPrompt("请输入标注位置\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//更新构件规格位置
						if(startOddId==pItem->m_idProp)
						{
							pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetX=basepnt.x-pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddBasePosX;
							pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetY=basepnt.y-pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fStartOddBasePosY;
						}
						else 
						{
							pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetX=basepnt.x-pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddBasePosX;
							pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetY=basepnt.y-pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.fEndOddBasePosY;
						}
						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
			if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC||pSonPartSpecCmd)//pItem->m_idProp==CDrawMapScriptDlg::GetPropID("PARTSPEC.DimPosOffset"))
			{
				if(pSonPartSpecCmd)
				{
					pCmd=pSonPartSpecCmd;
					//缩放至子命令对应的实体所在区域 wht 11-07-23
					SCOPE_STRU scope=pSonPartSpecCmd->GetCADEntScope();
					ZoomAcadView(scope,20);
					pSonPartSpecCmd->SetEntSetHighlight();
				}
				if(CDrawMapScriptDlg::GetPropID("PARTSPEC.MovePartSpec")==pItem->m_idProp)
				{	//调整构件明细位置
					pScriptDlg->HideWnd(TRUE);
					//
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//选择构件明细对应的实体集
					pCmd->GetEntSet(DRAGSET.drag_ent_set);
					//选择引出线对应的实体集
					AcDbLine *pRefLine=NULL;
					AcDbObjectId *pEntId=NULL;
					for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
					{
						AcDbEntity *pEnt=NULL;
						acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
						if(pEnt==NULL)
							continue;
						pEnt->close();
						if(!pEnt->isKindOf(AcDbLine::desc()))
							continue;
						pRefLine=(AcDbLine*)pEnt;
						DRAGSET.Add(*pEntId);
						break;
					}
					if(pRefLine)
					{
						f3dPoint basepnt;
						basepnt.Set(pRefLine->startPoint().x,pRefLine->startPoint().y);
						f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
						CXhChar100 sPrompt("please input(%s) the detailed position of parts\n",sPartNo);
#else
						CXhChar100 sPrompt("请输入(%s)构件明细位置\n",sPartNo);
#endif
						int nRetCode=DragEntSet(basepnt,sPrompt);
						if(nRetCode==RTNORM)
						{	//更新构件明细位置
							double fOffsetX=basepnt.x-old_pos.x;
							double fOffsetY=basepnt.y-old_pos.y;
							pCmd->m_paraPartSpec.fBasePosOffsetX+=fOffsetX;
							pCmd->m_paraPartSpec.fBasePosOffsetY+=fOffsetY;
							pCmd->m_paraPartSpec.fDimPosOffsetX+=fOffsetX;
							pCmd->m_paraPartSpec.fDimPosOffsetY+=fOffsetY;
							//更新界面
							actrTransactionManager->flushGraphics();
							acedUpdateDisplay();
						}
					}
				}
				else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.BasePosOffset")==pItem->m_idProp)
				{	//移动数据点位置
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//选择引出线对应的实体集
					AcDbLine *pRefLine=NULL;
					AcDbObjectId *pEntId=NULL;
					for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
					{
						AcDbEntity *pEnt=NULL;
						acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
						if(pEnt==NULL)
							continue;
						pEnt->close();
						if(!pEnt->isKindOf(AcDbLine::desc()))
							continue;
						pRefLine=(AcDbLine*)pEnt;
						DRAGSET.Add(*pEntId);
						break;
					}
					if(pRefLine)
					{
						pScriptDlg->HideWnd(TRUE);	
						//
						f3dPoint basepnt;
						basepnt.Set(pRefLine->startPoint().x,pRefLine->startPoint().y);
						f3dPoint old_pos=basepnt;
						ads_point start_pt,end_pt;
						end_pt[X]=pRefLine->endPoint().x;
						end_pt[Y]=pRefLine->endPoint().y;
						end_pt[Z]=pRefLine->endPoint().z;
#ifdef AFX_TARG_ENU_ENGLISH
						CXhChar100 sPrompt("please iput data extension point position of plate(%s):\n",sPartNo);
#else
						CXhChar100 sPrompt("请输入钢板(%s)数据扩展点位置:\n",sPartNo);
#endif
#ifdef _ARX_2007
						if(acedGetPoint(end_pt,(ACHAR*)_bstr_t(sPrompt),start_pt)==RTNORM)
#else
							if(acedGetPoint(end_pt,sPrompt,start_pt)==RTNORM)
#endif
							{	//更新构件规格位置
								double fOffsetX=start_pt[X]-old_pos.x;
								double fOffsetY=start_pt[Y]-old_pos.y;
								pCmd->m_paraPartSpec.fBasePosOffsetX+=fOffsetX;
								pCmd->m_paraPartSpec.fBasePosOffsetY+=fOffsetY;
								//
								pCmd->Rerun();
							}
					}
				}
				else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.DimPosOffset")==pItem->m_idProp)
				{	//移动编号位置
					pScriptDlg->HideWnd(TRUE);
					//
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					//TODO: 引处pCmd可能为CMD_DIM_BOLTORFL_INFO类型2E2-SZ1塔，0x2db杆件　wjh-2013.12.15
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//选择编号(及规格)对应的实体集
					SCOPE_STRU scope;
					BOOL bDrawRefLine=pCmd->m_paraPartSpec.bDrawRefLine;
					if(pCmd->m_paraPartSpec.bDrawRefLine)
					{	//存在引出线时如果拖放修改编号位置，先按无引出线时重新生成，然后调整位置。
						pCmd->m_paraPartSpec.bDrawRefLine=FALSE;
						pCmd->Rerun();
					}
					pCmd->GetEntSet(DRAGSET.drag_ent_set);
					scope=pCmd->GetCADEntScope();
					
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please input sequence number %s position\n",sPartNo);
#else
					CXhChar100 sPrompt("请输入编号%s位置\n",sPartNo);
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//更新编号位置
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraPartSpec.fDimPosOffsetX+=fOffsetX;
						pCmd->m_paraPartSpec.fDimPosOffsetY+=fOffsetY;
						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
					if(bDrawRefLine)
					{	//移动位置后再次重新生成构件编号 wht 11-07-18
						pCmd->m_paraPartSpec.bDrawRefLine=TRUE;
						pCmd->Rerun();
					}
				}
				else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.GuigeOffset")==pItem->m_idProp)
				{	//移动规格位置
					pScriptDlg->HideWnd(TRUE);	
					//
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//选择编号对应的实体集
					SCOPE_STRU scope;
					AcDbObjectId *pEntId=NULL;
					for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
					{
						AcDbEntity *pEnt=NULL;
						acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
						if(pEnt==NULL)
							continue;
						pEnt->close();
						if(!pEnt->isKindOf(AcDbText::desc()))
							continue;
						AcDbText *pText=(AcDbText*)pEnt;
						CXhChar100 sText;
					#ifdef _ARX_2007
						sText.Copy(_bstr_t(pText->textString()));
					#else
						sText.Copy(pText->textString());
					#endif
						CString sValue=sText;
						if(sValue.CompareNoCase(sPartNo)==0
#ifdef AFX_TARG_ENU_ENGLISH
							||sValue.Find("former")>=0||sValue.Find("latter")>=0)
#else
							||sValue.Find("前")>=0||sValue.Find("后")>=0)
#endif
							continue;
						scope.VerifyVertex(f3dPoint(pText->position().x,pText->position().y));
						DRAGSET.Add(*pEntId);
					}
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please input dimension position of plate(%s)specifications\n",sPartNo);
#else
					CXhChar100 sPrompt("请输入钢板(%s)规格的标注位置\n",sPartNo);
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//更新构件规格位置
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraPartSpec.fGuigeOffsetX+=fOffsetX;
						pCmd->m_paraPartSpec.fGuigeOffsetY+=fOffsetY;
						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)
		{
			if(CDrawMapScriptDlg::GetPropID("SIMPOLY.DimPosOffset")==pItem->m_idProp)
			{	//调整相似形位置
				DRAGSET.ClearEntSet();
				//选择编号对应的实体集
				int nEntNum=pCmd->GetEntSet(DRAGSET.drag_ent_set);
				if(nEntNum>0)
				{
					pScriptDlg->HideWnd(TRUE);	//隐藏窗口
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please select position of similar polygon\n");
#else
					CXhChar100 sPrompt("请选择相似形位置\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//更新相似形符位置
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraSimPoly.fDimPosOffsetX+=fOffsetX;
						pCmd->m_paraSimPoly.fDimPosOffsetY+=fOffsetY;
						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.vertexList")==pItem->m_idProp)
			{	//添加轮廓点
				int nCount=pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum();
				if(nCount>=15)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("only 15 outline vertexts supported currently ");
					return FALSE;
				}
				if(nCount>0&&AfxMessageBox("sure to delete all outline vertexs and select again?(Y/N)",MB_YESNO)==IDNO)
#else
					AfxMessageBox("目前仅最多支持15个轮廓点!");
					return FALSE;
				}
				if(nCount>0&&AfxMessageBox("确实要删除所有轮廓点重新选择吗？(Y/N)",MB_YESNO)==IDNO)
#endif
					return FALSE;
				//依次选择组成相似形轮廓的杆件，第i根与第i+1根杆件求交 最后一根不与第一根求交 wht 11-07-16
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
				pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
				pScriptDlg->m_nPickEntType=2;
				pScriptDlg->SelectEntObj(20);	//设置一个足够多的杆件数量
			}
			//相似形轮廓点参数 放在最后
			else if(pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum())
			{
				int i=1;
				SIMPOLY_VERTEX *pVertex=NULL;
				for(pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
				pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetNext())
				{
					char sKey1[50]="",sKey2[50]="",sKey3[50]="";
					sprintf(sKey1,"SIMPOLY.vertex%d",i);
					sprintf(sKey2,"SIMPOLY.vertex%d.hPrevPole",i);
					sprintf(sKey3,"SIMPOLY.vertex%d.hNextPole",i);
					int dd=CDrawMapScriptDlg::GetPropID(sKey1);
					if(CDrawMapScriptDlg::GetPropID(sKey1)==pItem->m_idProp)
					{	//删除当前轮廓点
#ifdef AFX_TARG_ENU_ENGLISH
						if(AfxMessageBox("are you sure to delete this outline vertex?(Y/N)",MB_YESNO)==IDNO)
#else
						if(AfxMessageBox("你确定要删除该轮廓点吗?(Y/N)",MB_YESNO)==IDNO)
#endif
							return FALSE;
						long prev_id=pItem->m_idProp;
						CString sPrePropName=pItem->m_lpNodeInfo->m_strPropName;
						pCmd->m_paraSimPoly.simPolyVertexList.DeleteCursor();
						pPropList->DeleteItemByPropId(pItem->m_idProp);
						//更新轮廓点ID
						for(int j=i+1;j<=pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum()+1;j++)
						{
							sprintf(sKey1,"SIMPOLY.vertex%d",j);
							long id=CDrawMapScriptDlg::GetPropID(sKey1);
							CPropTreeItem *pFindItem=pPropList->FindItemByPropId(id,NULL);
							if(pFindItem)
							{
								pFindItem->m_idProp=prev_id;
								prev_id=id;
								CString sPropName=pFindItem->m_lpNodeInfo->m_strPropName;
								pFindItem->m_lpNodeInfo->m_strPropName=sPrePropName;
								pFindItem->m_lpNodeInfo->m_strPropHelp=sPrePropName;
								sPrePropName=sPropName;
							}
						}
						pPropList->HideInputCtrl();	//隐藏输入控件
						return FALSE;
					}
					/*else if(CDrawMapScriptDlg::GetPropID(sKey2)==pItem->m_idProp
						||CDrawMapScriptDlg::GetPropID(sKey3)==pItem->m_idProp)
					{
						pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
						pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
						pScriptDlg->m_arrCmdPickPrompt.Add("\n请选择一根杆件(选择杆件心线)<Enter确认>:\n");
						pScriptDlg->SelectEntObj();
						break;
					}*/
					i++;
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_MODELSIZE)
		{
			if(CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.nodeSet")==pItem->m_idProp)
			{	//添加节点
				int nCount=pCmd->m_paraDimModelSize.listDimNodes.GetNodeNum();
				if(nCount>=49)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("49 nodes supported currently.");
#else
					AfxMessageBox("目前仅最多支持49个节点!");
#endif
					return FALSE;
				}
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
				pScriptDlg->m_nPickEntType=1;	//选择数据扩展点对应的节点
				pScriptDlg->SelectEntObj();
			}
			else if(pItem->m_pParent&&pItem->m_pParent->m_idProp==CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.nodeSet"))
			{	//删除当前存在的节点
				int i=0;
				CDrawingCommand::DIMNODE* pDimNode=NULL;
				for(pDimNode=pCmd->m_paraDimModelSize.listDimNodes.GetFirst();pDimNode;
					pDimNode=pCmd->m_paraDimModelSize.listDimNodes.GetNext())
				{
					char sKey[50]="";
					sprintf(sKey,"DIMMODELSIZE.Node%d",i);
					int dd=CDrawMapScriptDlg::GetPropID(sKey);
					if(CDrawMapScriptDlg::GetPropID(sKey)==pItem->m_idProp)
					{	//删除当前节点
#ifdef AFX_TARG_ENU_ENGLISH
						if(AfxMessageBox("are you sure to delete the wheel node?(Y/N)",MB_YESNO)==IDNO)
#else
						if(AfxMessageBox("你确定要删除该轮节点吗?(Y/N)",MB_YESNO)==IDNO)
#endif
							return FALSE;
						long prev_id=pItem->m_idProp;
						CString sPrePropName=pItem->m_lpNodeInfo->m_strPropName;
						pCmd->m_paraDimModelSize.listDimNodes.DeleteCursor();
						//pCmd->m_paraDimModelSize.nodeSet.DeleteNode(pDimNode->hNode);
						pPropList->DeleteItemByPropId(pItem->m_idProp);
						//更新节点对应项的ID
						for(int j=i+1;j<=pCmd->m_paraDimModelSize.listDimNodes.GetNodeNum()+1;j++)
						{
							sprintf(sKey,"DIMMODELSIZE.Node%d",j);
							long id=CDrawMapScriptDlg::GetPropID(sKey);
							CPropTreeItem *pFindItem=pPropList->FindItemByPropId(id,NULL);
							if(pFindItem)
							{
								pFindItem->m_idProp=prev_id;
								prev_id=id;
								CString sPropName=pFindItem->m_lpNodeInfo->m_strPropName;
								pFindItem->m_lpNodeInfo->m_strPropName=sPrePropName;
								pFindItem->m_lpNodeInfo->m_strPropHelp=sPrePropName;
								sPrePropName=sPropName;
							}
						}
						pPropList->HideInputCtrl();	//隐藏输入控件
						return FALSE;
					}
					i++;
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
		{
			long startOddId=CDrawMapScriptDlg::GetPropID("ODDMENT.StartOddPosOffset");
			long endOddId=CDrawMapScriptDlg::GetPropID("ODDMENT.EndOddPosOffset");
			if((startOddId==pItem->m_idProp&&pCmd->m_paraOddment.bDimStartOdd)
				||(endOddId==pItem->m_idProp&&pCmd->m_paraOddment.bDimEndOdd))
			{	//正负头标注
				AcDbText *pFirstOddText=NULL,*pSecOddText=NULL;
				AcDbObjectId *pEntId=NULL,*pFirstEntId=NULL,*pSecEntId=NULL;
				for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
				{
					AcDbEntity *pEnt=NULL;
					acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
					if(pEnt==NULL)
						continue;
					pEnt->close();
					if(!pEnt->isKindOf(AcDbText::desc()))
						continue;
					if(pFirstOddText==NULL)
					{
						pFirstOddText=(AcDbText*)pEnt;
						pFirstEntId=pEntId;
					}
					else if(pSecOddText==NULL)
					{
						pSecOddText=(AcDbText*)pEnt;
						pSecEntId=pEntId;
					}
					else 
						break;
				}
				DRAGSET.ClearEntSet();
				f3dPoint startPt(pCmd->m_paraOddment.fStartOddBasePosX+pCmd->m_paraOddment.fStartOddPosOffsetX,pCmd->m_paraOddment.fStartOddBasePosY+pCmd->m_paraOddment.fStartOddPosOffsetY);
				f3dPoint endPt(pCmd->m_paraOddment.fEndOddBasePosX+pCmd->m_paraOddment.fEndOddPosOffsetX,pCmd->m_paraOddment.fEndOddBasePosY+pCmd->m_paraOddment.fEndOddPosOffsetY);
				if(pFirstOddText==NULL&&pSecOddText==NULL)
					return FALSE;
				else if(pSecOddText==NULL)
				{
					if(startOddId==pItem->m_idProp&&DISTANCE(startPt,pFirstOddText->position())<=DISTANCE(endPt,pFirstOddText->position()))
						DRAGSET.Add(*pFirstEntId);
					else if(endOddId==pItem->m_idProp&&DISTANCE(startPt,pFirstOddText->position())>=DISTANCE(endPt,pFirstOddText->position()))
							DRAGSET.Add(*pFirstEntId); 
					else
						return FALSE;
				}
				else 
				{
					if(startOddId==pItem->m_idProp)
					{
						if(DISTANCE(startPt,pFirstOddText->position())<DISTANCE(startPt,pSecOddText->position()))
							DRAGSET.Add(*pFirstEntId);
						else 
							DRAGSET.Add(*pSecEntId); 
					}
					else if(endOddId==pItem->m_idProp)
					{
						if(DISTANCE(endPt,pFirstOddText->position())<DISTANCE(endPt,pSecOddText->position()))
							DRAGSET.Add(*pFirstEntId);
						else 
							DRAGSET.Add(*pSecEntId);
					}
				}
				//
				pScriptDlg->HideWnd(TRUE);	
				f3dPoint basepnt;
				if(startOddId==pItem->m_idProp)
					basepnt=startPt;
				else 
					basepnt=endPt;
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 sPrompt("please input dimension positions of the two ends\n");
#else
				CXhChar100 sPrompt("请输入正负头标注位置\n");
#endif
				int nRetCode=DragEntSet(basepnt,sPrompt);
				if(nRetCode==RTNORM)
				{	//更新构件规格位置
					if(startOddId==pItem->m_idProp)
					{
						pCmd->m_paraOddment.fStartOddPosOffsetX=basepnt.x-pCmd->m_paraOddment.fStartOddBasePosX;
						pCmd->m_paraOddment.fStartOddPosOffsetY=basepnt.y-pCmd->m_paraOddment.fStartOddBasePosY;
					}
					else 
					{
						pCmd->m_paraOddment.fEndOddPosOffsetX=basepnt.x-pCmd->m_paraOddment.fEndOddBasePosX;
						pCmd->m_paraOddment.fEndOddPosOffsetY=basepnt.y-pCmd->m_paraOddment.fEndOddBasePosY;
					}
					//更新界面
					actrTransactionManager->flushGraphics();
					acedUpdateDisplay();
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_SEGI)
		{	//段号标注
			if(CDrawMapScriptDlg::GetPropID("DIMSEGI.DimPos")==pItem->m_idProp)
			{	//调整相似形位置
				DRAGSET.ClearEntSet();
				//选择编号对应的实体集
				int nEntNum=pCmd->GetEntSet(DRAGSET.drag_ent_set);
				if(nEntNum>0)
				{
					pScriptDlg->HideWnd(TRUE);	//隐藏窗口
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please select position of segment number\n");
#else
					CXhChar100 sPrompt("请选择段号位置\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//更新段号位置
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraDimSegI.fDimPosX+=fOffsetX;
						pCmd->m_paraDimSegI.fDimPosY+=fOffsetY;
						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
		}
#ifdef __PART_DESIGN_INC_
		else if (pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
		{
			if(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.DimPosOffset")==pItem->m_idProp)
			{	
				AcDbObjectId *pEntId=NULL,lineFirstId,lineSecondId,textBoltId,textJointTypeId,textJointPartId;
				//调整角钢标注位置
				if(pCmd->m_pDrawingCom==NULL||pCmd->m_pDrawingCom->m_eClassType!=DRAW_COM_LINEVIEW)
					return FALSE;
				CLineView *pLineView=(CLineView*)pCmd->m_pDrawingCom;
				AcDbEntity *pEnt=NULL;
				pEntId=pLineView->jointEntIdList.GetValue(pCmd->m_paraAngleJoint.idAngleJoint);
				if(pEntId==NULL)
					return FALSE;
				acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
				if(pEnt==NULL)
					return FALSE;
				if(!pEnt->isKindOf(AcDbPolyline::desc()))
				{
					pEnt->close();
					return FALSE;
				}
				AcGePoint3d acad_pnt;
				f3dPoint start,end;
				((AcDbPolyline*)pEnt)->getStartPoint(acad_pnt);
				Cpy_Pnt(start,acad_pnt);
				((AcDbPolyline*)pEnt)->getEndPoint(acad_pnt);
				Cpy_Pnt(end,acad_pnt);
				f3dPoint originPoint;					
				Add_Pnt(originPoint,start,end);
				originPoint /=2.0;
				AcGePoint3d pPadPoint(originPoint.x,originPoint.y,originPoint.z);
				pEnt->close();

				int nUnderLineLen=0;
				ATOM_LIST<TEXT_POSY> textPosYList;
				for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
				{
					acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
					if(pEnt==NULL)
						continue;
					pEnt->close();
					if(pEnt->isKindOf(AcDbLine::desc()))
					{
						AcDbLine *pLine=(AcDbLine*)pEnt;
						if(pLine->startPoint()==pPadPoint)
							lineFirstId=*pEntId;
						else 
						{
							lineSecondId=*pEntId;
							nUnderLineLen=(int)DISTANCE(((AcDbLine*)pEnt)->startPoint(),((AcDbLine*)pEnt)->endPoint());
						}
					}
					else if(pEnt->isKindOf(AcDbText::desc()))
					{
						AcDbText *pText=(AcDbText*)pEnt;
						TEXT_POSY textPosY(*pEntId,pText->position().y);
						bool bInsert=false;
						for(int i=0;i<textPosYList.GetNodeNum();i++)
						{
							if(textPosY.posY>textPosYList[i].posY)
							{
								textPosYList.insert(textPosY);
								bInsert=true;
								break;
							}
						}
						if(!bInsert)
							textPosYList.append(textPosY);
					}
					else
						break;
				}
				if(textPosYList.GetNodeNum()!=3)
					return FALSE;
				textBoltId=textPosYList[0].textId;
				textJointTypeId=textPosYList[1].textId;
				textJointPartId=textPosYList[2].textId;

				pScriptDlg->HideWnd(TRUE);	//隐藏窗口
				int type, track = 1;//设置鼠标拖动控制
				AcGePoint3d acad_end,curPos,dim_pos;
				BOOL bUpdateOffsetPos=FALSE;
				while(track>0)
				{
					struct resbuf result;
					ads_grread(track,&type,&result);
					curPos.x = result.resval.rpoint[X];
					curPos.y = result.resval.rpoint[Y];
					curPos.z = 0;
					AcDbLine *pLine1=NULL,*pLine2=NULL;
					AcDbText *pDimTextBolt=NULL,*pDimTextJointType=NULL,*pDimTextJointPart=NULL;
					double fDimTextHeight=sys.dim_map.fPartGuigeTextSize;//文字高度
					const double fDimRowMargin=1;
					acdbOpenObject(pLine1,lineFirstId,AcDb::kForWrite);
					acdbOpenObject(pLine2,lineSecondId,AcDb::kForWrite);
					acdbOpenObject(pDimTextBolt,textBoltId,AcDb::kForWrite);
					acdbOpenObject(pDimTextJointType,textJointTypeId,AcDb::kForWrite);
					acdbOpenObject(pDimTextJointPart,textJointPartId,AcDb::kForWrite);
					CAcDbObjLife line1Life(pLine1),line2Life(pLine2),dimTextBoltLife(pDimTextBolt),dimTextJointTypeLife(pDimTextJointType),dimTextJointPartLife(pDimTextJointPart);
					if(pLine1&&pLine2&&pDimTextBolt&&pDimTextJointType&&pDimTextJointPart)
					{	
						pLine1->setEndPoint(curPos);
						pLine2->setStartPoint(curPos);
						Cpy_Pnt(acad_end,curPos);
						if(IsRightToLeft(curPos.x,originPoint.x,nUnderLineLen))
						{
							acad_end.x -= nUnderLineLen;
							Cpy_Pnt(dim_pos,acad_end);
						}
						else 
						{
							acad_end.x += nUnderLineLen;
							Cpy_Pnt(dim_pos,curPos);
						}
						pLine2->setEndPoint(acad_end);
						//
						dim_pos.y+=fDimRowMargin;
						pDimTextBolt->setPosition(dim_pos);
						dim_pos.y=curPos.y-fDimTextHeight-fDimRowMargin;
						pDimTextJointType->setPosition(dim_pos);
						dim_pos.y-=(fDimTextHeight+fDimRowMargin);
						pDimTextJointPart->setPosition(dim_pos);

						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
					else
						break;
					if(type==3)
					{
						bUpdateOffsetPos=TRUE;
						break;
					}
				}
				if(bUpdateOffsetPos)
				{	//更新角钢标注位置偏移量
					pCmd->m_paraAngleJoint.fDimPosOffsetX=curPos.x-originPoint.x;
					pCmd->m_paraAngleJoint.fDimPosOffsetY=curPos.y-originPoint.y;
				}
			}
		}
#endif
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
		{
			if(CDrawMapScriptDlg::GetPropID("BOLTPAD.DimPosOffset")==pItem->m_idProp)
			{	//调整垫圈标注位置
				AcDbObjectId *pEntId=NULL,lineId1,lineId2,textId;
				pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraBoltPad.hBolt);
				if(pEntId==NULL)
					return FALSE;
				AcDbEntity *pEnt=NULL;
				acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
				CAcDbObjLife entLift(pEnt);
				if(pEnt==NULL||!pEnt->isKindOf(AcDbPoint::desc()))
					return FALSE;
				//
				const int nUnderlineLen=7;	//垫圈标注下划线长度
				const double fMargin=0.5;	//标注文字与下划线之间的间隙
				AcDbPoint *pPadPoint=(AcDbPoint*)pEnt;
				AcGePoint3d pad_pos = pPadPoint->position();
				for(pEntId=pCmd->entIdList.GetFirst();pEntId;pEntId=pCmd->entIdList.GetNext())
				{
					AcDbEntity *pEnt=NULL;
					acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
					if(pEnt==NULL)
						continue;
					pEnt->close();
					if(pEnt->isKindOf(AcDbText::desc()))
						textId=*pEntId;
					else if(pEnt->isKindOf(AcDbLine::desc()))
					{
						AcDbLine *pLine=(AcDbLine*)pEnt;
						if(pLine->startPoint()==pad_pos)
							lineId1=*pEntId;
						else 
							lineId2=*pEntId;
					}
				}
				pScriptDlg->HideWnd(TRUE);	//隐藏窗口
				int type, track = 1;//设置鼠标拖动控制
				AcGePoint3d acad_end,curPos;
				BOOL bUpdateOffsetPos=FALSE;
				while(track>0)
				{
					struct resbuf result;
					ads_grread(track,&type,&result);
					curPos.x = result.resval.rpoint[X];
					curPos.y = result.resval.rpoint[Y];
					curPos.z = 0;
					AcDbLine *pLine1=NULL,*pLine2=NULL;
					AcDbText *pDimText=NULL;
					acdbOpenObject(pLine1,lineId1,AcDb::kForWrite);
					acdbOpenObject(pLine2,lineId2,AcDb::kForWrite);
					acdbOpenObject(pDimText,textId,AcDb::kForWrite);
					CAcDbObjLife line1Life(pLine1),line2Life(pLine2),dimTextLife(pDimText);
					if(pLine1&&pLine2&&pDimText)
					{	
						pLine1->setEndPoint(curPos);
						pLine2->setStartPoint(curPos);
						Cpy_Pnt(acad_end,curPos);
						if(IsRightToLeft(curPos.x,pad_pos.x,nUnderlineLen))
							acad_end.x -= nUnderlineLen;	
						else 
							acad_end.x += nUnderlineLen;
						pLine2->setEndPoint(acad_end);
						if(IsRightToLeft(curPos.x,pad_pos.x,nUnderlineLen))
						{
							curPos.x = acad_end.x+fMargin;
							curPos.y = acad_end.y+fMargin;
						}
						else 
						{
							curPos.x +=fMargin;
							curPos.y +=fMargin;
						}
						pDimText->setPosition(curPos);
					}
					else
						break;
					if(type==3)
					{
						bUpdateOffsetPos=TRUE;
						break;
					}
				}
				if(bUpdateOffsetPos)
				{	//更新垫圈标注位置偏移量
					pCmd->m_paraBoltPad.fDimPosOffsetX=curPos.x-pad_pos.x;
					pCmd->m_paraBoltPad.fDimPosOffsetY=curPos.y-pad_pos.y;
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
		{
			if(CDrawMapScriptDlg::GetPropID("BOLTINFO.boltSet")==pItem->m_idProp)
			{	//添加螺栓
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
				pScriptDlg->m_nPickEntType=3;				//其余实体
				pScriptDlg->SelectEntObj(20);				//设置一个足够多的数量
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.pDatumRod")==pItem->m_idProp)
			{	//从界面上选择杆件
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
				pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
#ifdef AFX_TARG_ENU_ENGLISH
				pScriptDlg->m_arrCmdPickPrompt.Add("\nplease select a rod(choose its center line)<Enter confirm>:\n");
#else
				pScriptDlg->m_arrCmdPickPrompt.Add("\n请选择一根杆件(选择杆件心线)<Enter确认>:\n");
#endif
				pScriptDlg->SelectEntObj();
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.DimPosOffset")==pItem->m_idProp)
			{	//调整螺栓信息标注位置
				DRAGSET.ClearEntSet();
				int nEntNum=pCmd->GetEntSet(DRAGSET.drag_ent_set);
				if(nEntNum>0)
				{
					pScriptDlg->HideWnd(TRUE);	//隐藏窗口
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please select bolt's information dimension position\n");
#else
					CXhChar100 sPrompt("请选择螺栓信息标注位置\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//更新螺栓信息标注位置
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraBoltGroupInfo.fDimPosOffsetX+=fOffsetX;
						pCmd->m_paraBoltGroupInfo.fDimPosOffsetY+=fOffsetY;
						//更新界面
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
		{
			if( CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.hStartLs")==pItem->m_idProp||
				CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.hEndLs")==pItem->m_idProp)
			{	
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
				pScriptDlg->m_nPickEntType=3;				//其余实体
				pScriptDlg->SelectEntObj(20);				//设置一个足够多的数量
			}
		}
	}
	return TRUE;
}
static BOOL FirePropHelpPrompt(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pPropList->GetParent();
	if(pItem==NULL||pScriptDlg==NULL)
		return FALSE;
	if(pItem->m_pParent&&pItem->m_pParent->m_idProp==CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.nodeSet"))
	{
		long hNode=0;
		sscanf(pItem->m_lpNodeInfo->m_strPropValue,"%X",&hNode);
		if(hNode>0x20)
		{
			AcDbEntity *pEnt=pScriptDlg->m_pCurCmd->m_pDrawingCom->FindCADEntByHandle(hNode);
			if(pEnt&&pEnt->isKindOf(AcDbPoint::desc()))
			{
				SCOPE_STRU scope;
				AcDbPoint *pPoint=(AcDbPoint*)pEnt;
				f3dPoint base_pt;
				Cpy_Pnt(base_pt,pPoint->position());
				scope.VerifyVertex(f3dPoint(base_pt.x-5,base_pt.y,0));
				scope.VerifyVertex(f3dPoint(base_pt.x+5,base_pt.y,0));
				scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y-5,0));
				scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y+5,0));
				ZoomAcadView(scope,50);
			}

		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_PART
		&&(CDrawMapScriptDlg::GetPropID("m_bExecute")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.bDrawRefLine")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimGuige")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.iMirMsg")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimPoleSpec")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fPoleDimPosCoef")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.GuigeOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.DimPosOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.BasePosOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.MovePartSpec")==pItem->m_idProp))
	{
		CDrawingCommand **ppCmd=pScriptDlg->hashPartSpecList.GetValue(pScriptDlg->m_pCurCmd->m_paraPart.hPart);
		if(ppCmd)
		{
			SCOPE_STRU scope=(*ppCmd)->GetCADEntScope();
			ZoomAcadView(scope,20);
			(*ppCmd)->SetEntSetHighlight();
		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC
		&&(CDrawMapScriptDlg::GetPropID("m_bExecute")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.bDrawRefLine")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimGuige")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.iMirMsg")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimPoleSpec")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fPoleDimPosCoef")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.GuigeOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.DimPosOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.BasePosOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("PARTSPEC.MovePartSpec")==pItem->m_idProp))
	{
		SCOPE_STRU scope=pScriptDlg->m_pCurCmd->GetCADEntScope();
		ZoomAcadView(scope,20);
		CDrawingCommand::ReleaseHighlightEnts();
		pScriptDlg->m_pCurCmd->SetEntSetHighlight();
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_PART&&
		CDrawMapScriptDlg::GetPropID("PART.fPoleAxisLineStartOutSpreadLen")==pItem->m_idProp)
	{
		if(pScriptDlg->m_pCurCmd->m_paraPart.pPart->IsAngle()&&
			pScriptDlg->m_pCurCmd->drawAngleInfo.m_xStartDrawPos!=pScriptDlg->m_pCurCmd->drawAngleInfo.m_xEndDrawPos)
		{
			SCOPE_STRU scope;
			f3dPoint start=pScriptDlg->m_pCurCmd->drawAngleInfo.m_xStartDrawPos;
			scope.VerifyVertex(f3dPoint(start.x-10,start.y-10));
			scope.VerifyVertex(f3dPoint(start.x+10,start.y+10));
			ZoomAcadView(scope,20);
		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_PART&&
		CDrawMapScriptDlg::GetPropID("PART.fPoleAxisLineEndOutSpreadLen")==pItem->m_idProp)
	{
		if(pScriptDlg->m_pCurCmd->m_paraPart.pPart->IsAngle()&&
			pScriptDlg->m_pCurCmd->drawAngleInfo.m_xStartDrawPos!=pScriptDlg->m_pCurCmd->drawAngleInfo.m_xEndDrawPos)
		{
			SCOPE_STRU scope;
			f3dPoint end=pScriptDlg->m_pCurCmd->drawAngleInfo.m_xEndDrawPos;
			scope.VerifyVertex(f3dPoint(end.x-10,end.y-10));
			scope.VerifyVertex(f3dPoint(end.x+10,end.y+10));
			ZoomAcadView(scope,20);
		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_ODDMENT)
	{
		if(CDrawMapScriptDlg::GetPropID("ODDMENT.StartOddPosOffset")==pItem->m_idProp
			||CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetX")==pItem->m_idProp
			||CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetY")==pItem->m_idProp)
		{
			SCOPE_STRU scope;
			double fLenght=10;
			f3dPoint start(pScriptDlg->m_pCurCmd->m_paraOddment.fStartOddBasePosX+pScriptDlg->m_pCurCmd->m_paraOddment.fStartOddPosOffsetX,
				pScriptDlg->m_pCurCmd->m_paraOddment.fStartOddBasePosY+pScriptDlg->m_pCurCmd->m_paraOddment.fStartOddPosOffsetY);
			scope.VerifyVertex(f3dPoint(start.x-fLenght,start.y-fLenght));
			scope.VerifyVertex(f3dPoint(start.x+fLenght,start.y-fLenght));
			scope.VerifyVertex(f3dPoint(start.x-fLenght,start.y+fLenght));
			scope.VerifyVertex(f3dPoint(start.x+fLenght,start.y+fLenght));
			ZoomAcadView(scope,20);
			CDrawingCommand::ReleaseHighlightEnts();
			pScriptDlg->m_pCurCmd->SetEntSetHighlight(1);
		}
		else if(CDrawMapScriptDlg::GetPropID("ODDMENT.EndOddPosOffset")==pItem->m_idProp
			||CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetX")==pItem->m_idProp
			||CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetY")==pItem->m_idProp)
		{
			SCOPE_STRU scope;
			double fLenght=10;
			f3dPoint end(pScriptDlg->m_pCurCmd->m_paraOddment.fEndOddBasePosX+pScriptDlg->m_pCurCmd->m_paraOddment.fEndOddPosOffsetX,
				pScriptDlg->m_pCurCmd->m_paraOddment.fEndOddBasePosY+pScriptDlg->m_pCurCmd->m_paraOddment.fEndOddPosOffsetY);
			scope.VerifyVertex(f3dPoint(end.x-fLenght,end.y-fLenght));
			scope.VerifyVertex(f3dPoint(end.x+fLenght,end.y-fLenght));
			scope.VerifyVertex(f3dPoint(end.x-fLenght,end.y+fLenght));
			scope.VerifyVertex(f3dPoint(end.x+fLenght,end.y+fLenght));
			ZoomAcadView(scope,20);
			CDrawingCommand::ReleaseHighlightEnts();
			pScriptDlg->m_pCurCmd->SetEntSetHighlight(2);
		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
	{
		if(CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.hStartLs")==pItem->m_idProp)
			pScriptDlg->m_pCurCmd->SetEntSetHighlight(1);
		else if(CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.hEndLs")==pItem->m_idProp)
			pScriptDlg->m_pCurCmd->SetEntSetHighlight(2);
		else
		{
			SCOPE_STRU scope=pScriptDlg->m_pCurCmd->GetCADEntScope();
			ZoomAcadView(scope,20);	//缩放至命令对应的实体
			pScriptDlg->m_pCurCmd->SetEntSetHighlight();
		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
	{
		if(pItem->m_idProp>HASHTABLESIZE)
		{
			CLsGroupDimUnit::BOLTINFO *pBoltInfo=(CLsGroupDimUnit::BOLTINFO*)pItem->m_idProp;
			if(pBoltInfo&&pBoltInfo->hBolt>0x20)
			{	//高亮显示当前选中螺栓 wht 12-03-22
				CDrawingCommand *pCmd=((CStruView*)pScriptDlg->m_pCurCmd->m_pDrawingCom)->GetDrawingPartCmdFromHandle(pBoltInfo->hBolt);
				if(pCmd)
				{
					CDrawingCommand::ReleaseHighlightEnts();
					//缩放至命令对应的实体
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					ZoomAcadView(scope,20);
					pCmd->SetEntSetHighlight();
				}
			}
		}
		else //高亮显示当前命令对应的实体 
		{	 
			SCOPE_STRU scope=pScriptDlg->m_pCurCmd->GetCADEntScope();
			ZoomAcadView(scope,20);	//缩放至命令对应的实体
			pScriptDlg->m_pCurCmd->SetEntSetHighlight();	
		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC
		&&(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.pLinePart")==pItem->m_idProp		
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.iDimPosType")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX")==pItem->m_idProp
		||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetY")==pItem->m_idProp))
	{
		CDrawingCommand **ppCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue(pScriptDlg->m_pCurCmd->m_paraPartSpec.hObj);
		if(ppCmd)
		{
			if(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset")==pItem->m_idProp
				||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX")==pItem->m_idProp
				||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY")==pItem->m_idProp)
			{
				SCOPE_STRU scope;
				double fLenght=10;
				f3dPoint start((*ppCmd)->m_paraBoltOrFlInfo.fStartOddBasePosX+(*ppCmd)->m_paraBoltOrFlInfo.fStartOddPosOffsetX,
					(*ppCmd)->m_paraBoltOrFlInfo.fStartOddBasePosY+(*ppCmd)->m_paraBoltOrFlInfo.fStartOddPosOffsetY);
				scope.VerifyVertex(f3dPoint(start.x-fLenght,start.y-fLenght));
				scope.VerifyVertex(f3dPoint(start.x+fLenght,start.y-fLenght));
				scope.VerifyVertex(f3dPoint(start.x-fLenght,start.y+fLenght));
				scope.VerifyVertex(f3dPoint(start.x+fLenght,start.y+fLenght));
				ZoomAcadView(scope,20);
				CDrawingCommand::ReleaseHighlightEnts();
				(*ppCmd)->SetEntSetHighlight(1);
			}
			else if(CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset")==pItem->m_idProp
				||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX")==pItem->m_idProp
				||CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetY")==pItem->m_idProp)
			{
				SCOPE_STRU scope;
				double fLenght=10;
				f3dPoint end((*ppCmd)->m_paraBoltOrFlInfo.fEndOddBasePosX+(*ppCmd)->m_paraBoltOrFlInfo.fEndOddPosOffsetX,
					(*ppCmd)->m_paraBoltOrFlInfo.fEndOddBasePosY+(*ppCmd)->m_paraBoltOrFlInfo.fEndOddPosOffsetY);
				scope.VerifyVertex(f3dPoint(end.x-fLenght,end.y-fLenght));
				scope.VerifyVertex(f3dPoint(end.x+fLenght,end.y-fLenght));
				scope.VerifyVertex(f3dPoint(end.x-fLenght,end.y+fLenght));
				scope.VerifyVertex(f3dPoint(end.x+fLenght,end.y+fLenght));
				ZoomAcadView(scope,20);
				CDrawingCommand::ReleaseHighlightEnts();
				(*ppCmd)->SetEntSetHighlight(2);
			}
			else
			{
				SCOPE_STRU scope=(*ppCmd)->GetCADEntScope();
				ZoomAcadView(scope,20);
				CDrawingCommand::ReleaseHighlightEnts();
				(*ppCmd)->SetEntSetHighlight();
			}
		}
	}
	return TRUE;
}
static BOOL FireModifyCellValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,
								int iSubItem,CString &valueStr)
{
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pListCtrl->GetParent();
	if(pItem==NULL||pScriptDlg==NULL)
		return FALSE;
	int iCurGroup=pScriptDlg->GetCurCmdGroup();
	if(pScriptDlg->IsSpreadLinePropGroup(iCurGroup))
	{	//展开线
		CDrawingSpreadLine *pSpeardLine=(CDrawingSpreadLine*)pItem->m_idProp;
		if(pSpeardLine==NULL)
			return FALSE;
		if(iSubItem==2)
		{	//修改展开线名称
			pSpeardLine->m_sLineName.Printf("%s",valueStr);
			pScriptDlg->m_propList.SetItemPropValue(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_sLineName"),valueStr);
		}
	}
	return TRUE;
}

static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{	//选中项发生变化后更新属性栏
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pListCtrl->GetParent();
	if(!pScriptDlg->m_bFireListItemChange)
		return FALSE;	//未启用ItemChange
	if(pItem==NULL||pScriptDlg==NULL||pNMListView==NULL)
		return FALSE;
	int iCurGroup=pScriptDlg->GetCurCmdGroup();
	if(pScriptDlg->IsSpreadLinePropGroup(iCurGroup))	//获取当前选项关联的展开线
		pScriptDlg->m_pSpreadLine=(CDrawingSpreadLine*)pItem->m_idProp;	
	else				//获取当前选项关联的命令
	{
		CDrawingCommand::ReleaseHighlightEnts();
		pScriptDlg->m_pCurCmd=(CDrawingCommand*)pItem->m_idProp;
		pScriptDlg->cmdList.Empty();
		if(pScriptDlg->m_pCurCmd)
			pScriptDlg->cmdList.append(pScriptDlg->m_pCurCmd);
	}
	pScriptDlg->DisplayPropertyList(iCurGroup);//更新属性栏
	if(pScriptDlg->m_pCurCmd)
	{	
		//当前命令为模型尺寸标注时切换数据扩展点显示模式便于查看节点 wht 11-07-13
		/*目前认为没有必要在此时显示节点，只需要在捕捉点时显示节点即可非要显示会搞得操作慢，图面显示乱 wjh-2011.8.31
		if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE&&
			!pScriptDlg->m_bPdmodeIs34)	//如果当前状态为显示X 就不用重复执行以下代码 wht 11-07-27
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"34",RTNONE);//显示点 X
			//acedCommand(RTSTR,"REGEN",RTNONE);
			pScriptDlg->m_bPdmodeIs34=TRUE;	//数据扩展点显示模式为34 wht 11-07-15
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType!=CMD_DIM_MODELSIZE
			&&pScriptDlg->m_bPdmodeIs34)
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//显示点 .
			//acedCommand(RTSTR,"REGEN",RTNONE);
			pScriptDlg->m_bPdmodeIs34=FALSE;
		}*/
		//缩放至命令对应的实体
		SCOPE_STRU scope=pScriptDlg->m_pCurCmd->GetCADEntScope();
		ZoomAcadView(scope,20);
		pScriptDlg->m_pCurCmd->SetEntSetHighlight();
		if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
		{
			CDrawingCommand **ppSonCmd=pScriptDlg->hashRodBoltOrFlInfoList.GetValue(pScriptDlg->m_pCurCmd->m_paraPartSpec.hObj);
			if(ppSonCmd)
				(*ppSonCmd)->SetEntSetHighlight();
		}
	}
	return TRUE;
}

//From DimSimPoly.cpp
void AddDimSimPolyCmd(CDrawingComponent *pDrawingCom,ATOM_LIST<CDrawingCommand*> *pCmdList=NULL);
//左键双击以及单击按钮共用该回调函数
static BOOL FireLButtonDblclk(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem)
{	//双击某项第一列后更改当前命令行状态，用来添加删除断点
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pListCtrl->GetParent();
	if(pItem&&iSubItem==0&&pItem->m_lpNodeInfo)
	{
		CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
		if(pCmd&&(pCmd->m_stateCmd&CMD_STATE_BREAKPOINT))
		{	//移除断点
			pCmd->m_stateCmd^=CMD_STATE_BREAKPOINT;
			pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);
		}
		else if(pCmd)
		{	//添加断点
			pCmd->m_stateCmd|=CMD_STATE_BREAKPOINT;
			pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);
		}
		//双击第一项时会触发OnItemChanged,调用FireItemChanged此时即可更新属性栏
	}
	else if(pItem&&iSubItem==3&&pItem->m_lpNodeInfo)
	{
		BOOL bStartUpAddSimPoly=FALSE;	//启动添加相似形命令
		int iCurGroup=pScriptDlg->GetCurCmdGroup();
		CString sOper=pItem->m_lpNodeInfo->GetSubItemText(iSubItem);
#ifdef AFX_TARG_ENU_ENGLISH
		if(iCurGroup==GROUP_DIM_SIMPOLY-1&&sOper.CompareNoCase("Add")==0)
#else
		if(iCurGroup==GROUP_DIM_SIMPOLY-1&&sOper.CompareNoCase("添加")==0)
#endif
		{
			CString ss=_T("");
			if(pScriptDlg->m_pComponent&&pScriptDlg->m_pCurCmd)
			{
				if(pScriptDlg->m_pCurCmd->m_paraSimPoly.nSimPolyCmdType==1)
				{
					pScriptDlg->m_pCurCmd=pScriptDlg->m_pComponent->cmdList.append();
					pScriptDlg->m_pCurCmd->m_pDrawingCom=pScriptDlg->m_pComponent;
					pScriptDlg->m_pCurCmd->m_eCmdType=CMD_DIM_SIMPOLY;
					pScriptDlg->m_pCurCmd->InitCommandPara();
					pScriptDlg->m_pCurCmd->m_paraSimPoly.simPolyVertexList.Empty();
					pScriptDlg->m_pCurCmd->m_paraSimPoly.nSimPolyCmdType=3;	//3.未指定插入点的相似形
					//添加一行数据
					char sCmdId[100]="",sCmdText[200]="";
					pScriptDlg->m_pCurCmd->GetCmdInfo(sCmdId,sCmdText);
					CListCtrlItemInfo *lpRoot=new CListCtrlItemInfo();
					lpRoot->SetImage(pScriptDlg->m_pCurCmd->m_stateCmd);	//根据命令状态设置不同的图标
					lpRoot->AddSubItemText(_T(""),TRUE);
					lpRoot->AddSubItemText(sCmdId,TRUE);
					lpRoot->AddSubItemText(sCmdText,TRUE);
					if(pScriptDlg->m_pCurCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
						lpRoot->AddSubItemText(_T("√"),TRUE);
#else
						lpRoot->AddSubItemText(_T("√"),TRUE);
#endif
					else
						lpRoot->AddSubItemText(_T("  "),TRUE);//×
					CSuperGridCtrl::CTreeItem *pTreeItem=pListCtrl->InsertRootItem(lpRoot);
					pTreeItem->m_idProp=(long)pScriptDlg->m_pCurCmd;
					pTreeItem->m_dwPropGroup=pItem->m_dwPropGroup;
					pListCtrl->SelectItem(pTreeItem);
					bStartUpAddSimPoly=TRUE;	//启动添加相似形的命令 wht 11-07-16
				}
				else if(pScriptDlg->m_pCurCmd->m_paraSimPoly.nSimPolyCmdType==0)
				{	//启动通过选择拾取点添加相似形命令 wht 11-07-25
					pScriptDlg->HideWnd(TRUE);
					ATOM_LIST<CDrawingCommand*> cmdList;
					AddDimSimPolyCmd(pScriptDlg->m_pComponent,&cmdList);
					CDrawingCommand **ppCmd=NULL;
					for(ppCmd=cmdList.GetFirst();ppCmd;ppCmd=cmdList.GetNext())
					{	//添加一行数据
						char sCmdId[100]="",sCmdText[200]="";
						(*ppCmd)->GetCmdInfo(sCmdId,sCmdText);
						CListCtrlItemInfo *lpRoot=new CListCtrlItemInfo();
						lpRoot->SetImage((*ppCmd)->m_stateCmd);	//根据命令状态设置不同的图标
						lpRoot->AddSubItemText(_T(""),TRUE);
						lpRoot->AddSubItemText(sCmdId,TRUE);
						lpRoot->AddSubItemText(sCmdText,TRUE);
						if((*ppCmd)->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
							lpRoot->AddSubItemText(_T("√"),TRUE);
#else
							lpRoot->AddSubItemText(_T("√"),TRUE);
#endif
						else
							lpRoot->AddSubItemText(_T("  "),TRUE);//×
						CSuperGridCtrl::CTreeItem *pTreeItem=pListCtrl->InsertRootItem(lpRoot);
						pTreeItem->m_idProp=(long)(*ppCmd);
						pTreeItem->m_dwPropGroup=pItem->m_dwPropGroup;
						//pListCtrl->SelectItem(pTreeItem);
					}
				}
			}
		}
		else 
		{
			CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
			if(pCmd&&pCmd->m_bExecute)
			{	//将√改为×
				pCmd->m_bExecute=FALSE;
				pItem->m_lpNodeInfo->SetSubItemText(iSubItem,"  ",TRUE); //×
				pListCtrl->SetSubItemText(pItem,iSubItem,CString("  ")); //×
				pCmd->EmptyEntList();
			}
			else if(pCmd&&!pCmd->m_bExecute)
			{	//将×改为√
				pCmd->m_bExecute=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				pItem->m_lpNodeInfo->SetSubItemText(iSubItem,"√",TRUE);
				pListCtrl->SetSubItemText(pItem,iSubItem,CString("√"));
#else
				pItem->m_lpNodeInfo->SetSubItemText(iSubItem,"√",TRUE);
				pListCtrl->SetSubItemText(pItem,iSubItem,CString("√"));
#endif
				pCmd->Rerun();
			}
		}
		pScriptDlg->DisplayPropertyList(iCurGroup);	//更新属性栏
		if(bStartUpAddSimPoly)
		{	//依次选择组成相似形轮廓的杆件，第i根与第i+1根杆件求交 最后一根不与第一根求交 wht 11-07-16
			CPropTreeItem *pItem=NULL;
			pItem=pScriptDlg->m_propList.FindItemByPropId(CDrawMapScriptDlg::GetPropID("SIMPOLY.vertexList"),NULL);
			if(pItem)
			{
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//记录触发事件的属性ID
				pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
				pScriptDlg->m_nPickEntType=2;
				pScriptDlg->SelectEntObj(20);	//设置一个足够多的杆件数量
			}
		}
	}
	return TRUE;
}

//关联右键菜单
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CDrawMapScriptDlg *pDrawDlg=(CDrawMapScriptDlg*)pListCtrl->GetParent();
	if(pDrawDlg==NULL)
		return FALSE;
	CAcModuleResourceOverride resOverride;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_SCRIPT);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	if(pMenu==NULL)
		return FALSE;
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION);
	//
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_ENABLE_ALL_ITEM,"enable all the items");
	pMenu->AppendMenu(MF_STRING,ID_DISABLE_ALL_ITEM,"disable all the items");
#else
	pMenu->AppendMenu(MF_STRING,ID_ENABLE_ALL_ITEM,"启用所有项");
	pMenu->AppendMenu(MF_STRING,ID_DISABLE_ALL_ITEM,"禁用所有项");
#endif
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pDrawDlg);
	return TRUE;
}

static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem)
{
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pListCtrl->GetParent();
	if(pItem==NULL||pScriptDlg==NULL)
		return FALSE;
	int iCurGroup=pScriptDlg->GetCurCmdGroup();
	if(iCurGroup==GROUP_DIM_SIMPOLY-1)	
	{
		CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
		if(pCmd==NULL||pCmd->m_pDrawingCom==NULL)
			return FALSE;
		if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY&&pCmd->m_paraSimPoly.nSimPolyCmdType>1)
		{
			pCmd->EmptyEntList();	//删除命令对应的CAD实体
			CDrawingCommand *pTempCmd=NULL;
			CDrawingComponent *pDrawingCom=pCmd->m_pDrawingCom;
			for(pTempCmd=pDrawingCom->cmdList.GetFirst();pTempCmd;pTempCmd=pDrawingCom->cmdList.GetNext())
			{
				if(pTempCmd==pCmd)
				{
					pDrawingCom->cmdList.DeleteCursor(TRUE);
					break;
				}
			}
		}
		else 
			return FALSE;
	}
	else 
		return FALSE;
	return TRUE;
}

static BOOL ModifyItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CDrawMapScriptDlg::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}

void CDrawMapScriptDlg::InitPropHashtable()	//初始化属性哈希表
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	//
	propHashtable.SetValueAt("CmdPara",id++);
	propHashtable.SetValueAt("m_eCmdType",id++);		//命令类型
	propHashtable.SetValueAt("m_stateCmd",id++);	//命令状态
	propHashtable.SetValueAt("m_bExecute",id++);	//是否执行该命令
	
	propHashtable.SetValueAt("DrawPara",id++);	//绘制构件参数
	//构件绘制参数
	propHashtable.SetValueAt("PART.pPart",id++);	//脚本记录关联对象
	propHashtable.SetValueAt("PART.PoleAxisLineOutSpreadLen",id++);	//杆件心线延伸长度
	propHashtable.SetValueAt("PART.fPoleAxisLineStartOutSpreadLen",id++);	//杆件心线始端延伸长度
	propHashtable.SetValueAt("PART.fPoleAxisLineEndOutSpreadLen",id++);		//杆件心线终端延伸长度
	propHashtable.SetValueAt("PART.iResidePlane",id++);	//展开面
	propHashtable.SetValueAt("PART.cDrawFlag",id++);	//构件绘制面 'X'.X截面 'Y'.Y截面,'Z'.Z截面 自动判断
	propHashtable.SetValueAt("PART.angle.bDrawAxisLine",id++);		//是否绘制角钢心线
	propHashtable.SetValueAt("PART.angle.extendStyle",id++);		//肢宽拓展方式
	propHashtable.SetValueAt("PART.angle.nXWingDatumAxisG",id++);	//X肢基准心距
	propHashtable.SetValueAt("PART.angle.nYWingDatumAxisG",id++);	//Y肢基准心距
	propHashtable.SetValueAt("PART.bolt.bDrawSideBoltLine",id++);	//Y肢基准心距
	propHashtable.SetValueAt("PART.bolt.bDimHoleDiameter",id++);	//是否标注特殊孔直径
	//绘制杆件单线图
	propHashtable.SetValueAt("LINEPART.pLinePart",id++);	//关联杆件
	propHashtable.SetValueAt("LINEPART.bSum",id++);			//总图
	propHashtable.SetValueAt("LINEPART.bDimByAnalysisMode",id++);	//力学分析模式
	//表格绘制参数 第一次指定表格位置后记录表格位置相对坐标，再次绘制表格时可直接使用
	propHashtable.SetValueAt("TABLE.nLocatePos",id++);			//0.左上 1.右上 2.左下 4.右下
	propHashtable.SetValueAt("TABLE.bSelectPos",id++);			//选择表格位置
	propHashtable.SetValueAt("TABLE.bDisplayDlg",id++);			//显示对话框
	propHashtable.SetValueAt("TABLE.DimPos",id++);				//表格位置
	propHashtable.SetValueAt("TABLE.fDimPosX",id++);			//表格位置X坐标		
	propHashtable.SetValueAt("TABLE.fDimPosY",id++);			//表格位置Y坐标	
	propHashtable.SetValueAt("TABLE.iTableType",id++);			//表格类型
	//构件明细标注参数
	propHashtable.SetValueAt("PublicPara",id++);				//公共参数			
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.pObj",id++);				//当前标注构件
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.iDimPosType",id++);		//0.自动搜索位置 1.指定位置(保存指定位置与原标注位置的偏移值)  2.选择位置
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.bDimGuige",id++);			//标注构件规格
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.bDimJgLength",id++);		//标注角钢长度
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.bDrawRefLine",id++);		//是否绘制引出线
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.fPoleDimPosCoef",id++);	//杆件标注位置系数 //LTMA 0.3  TAS 0.5
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.iMirMsg",id++);			//0:X轴对称；1:Y轴对称;其余不对称处理
	//
	propHashtable.SetValueAt("PARTSPEC.m_bExecute",id++);	//是否执行该命令
	propHashtable.SetValueAt("PARTSPEC.pObj",id++);				//当前标注构件
	propHashtable.SetValueAt("PARTSPEC.BasePosOffset",id++);	//数据点位置偏移量
	propHashtable.SetValueAt("PARTSPEC.fBasePosOffsetX",id++);	//数据点位置X坐标偏移值
	propHashtable.SetValueAt("PARTSPEC.fBasePosOffsetY",id++);	//数据点位置Y坐标偏移值
	propHashtable.SetValueAt("PARTSPEC.DimPosOffset",id++);		//标注位置偏移量
	propHashtable.SetValueAt("PARTSPEC.fDimPosOffsetX",id++);	//标注位置X坐标偏移值
	propHashtable.SetValueAt("PARTSPEC.fDimPosOffsetY",id++);	//标注位置Y坐标偏移值
	propHashtable.SetValueAt("PARTSPEC.GuigeOffset",id++);		//规格标注位置偏移值
	propHashtable.SetValueAt("PARTSPEC.fGuigeOffsetX",id++);	//规格标注位置X坐标偏移值
	propHashtable.SetValueAt("PARTSPEC.fGuigeOffsetY",id++);	//规格标注位置Y坐标偏移值
	propHashtable.SetValueAt("PARTSPEC.MovePartSpec",id++);		//移动构件明细
	propHashtable.SetValueAt("PARTSPEC.iDimPosType",id++);		//0.自动搜索位置 1.指定位置(保存指定位置与原标注位置的偏移值)  2.选择位置
	propHashtable.SetValueAt("PARTSPEC.bDimPoleSpec",id++);		//标注杆件明细
	propHashtable.SetValueAt("PARTSPEC.bDrawRefLine",id++);		//是否绘制引出线
	propHashtable.SetValueAt("PARTSPEC.fPoleDimPosCoef",id++);	//杆件标注位置系数 //LTMA 0.3  TAS 0.5
	propHashtable.SetValueAt("PARTSPEC.iMirMsg",id++);			//0:X轴对称；1:Y轴对称;其余不对称处理
	//角钢接头信息
	propHashtable.SetValueAt("ANGLEJOINT.m_bExecute",id++);		//是否执行该命令
	propHashtable.SetValueAt("ANGLEJOINT.pObj",id++);			//当前标注构件
	propHashtable.SetValueAt("ANGLEJOINT.iDimPosType",id++);	//0.自动搜索位置 1.指定位置
	propHashtable.SetValueAt("ANGLEJOINT.DimPosOffset",id++);	//标注位置偏移值
	propHashtable.SetValueAt("ANGLEJOINT.fDimPosOffsetX",id++);	//标注位置X坐标偏移值
	propHashtable.SetValueAt("ANGLEJOINT.fDimPosOffsetY",id++);	//标注位置Y坐标偏移值
	//标注段号
	propHashtable.SetValueAt("DIMSEGI.bSelectPos",id++);//选择位置
	propHashtable.SetValueAt("DIMSEGI.nSegI",id++);		//关联段号
	propHashtable.SetValueAt("DIMSEGI.DimPos",id++);	//标注位置
	propHashtable.SetValueAt("DIMSEGI.fDimPosX",id++);	//标注位置X坐标
	propHashtable.SetValueAt("DIMSEGI.fDimPosY",id++);	//标注位置Y坐标
	//正负头标注参数
	propHashtable.SetValueAt("ODDMENT.pLinePart",id++);			//正负头关联杆件
	propHashtable.SetValueAt("ODDMENT.iDimPosType",id++);		//0.自动搜索 1.指定位置
	propHashtable.SetValueAt("ODDMENT.StartOddPosOffset",id++);
	propHashtable.SetValueAt("ODDMENT.fStartOddPosOffsetX",id++);
	propHashtable.SetValueAt("ODDMENT.fStartOddPosOffsetY",id++);
	propHashtable.SetValueAt("ODDMENT.EndOddPosOffset",id++);
	propHashtable.SetValueAt("ODDMENT.fEndOddPosOffsetX",id++);
	propHashtable.SetValueAt("ODDMENT.fEndOddPosOffsetY",id++);
	//脚钉间距标注参数
	propHashtable.SetValueAt("FOOTNAIL_SPACE.pLinePart",id++);	//脚钉间距关联杆件
	propHashtable.SetValueAt("FOOTNAIL_SPACE.fDimDistance",id++);//标注点与标注起始点的间距
	propHashtable.SetValueAt("FOOTNAIL_SPACE.fSpace",id++);		//标注起始点与脚钉所在杆件外边缘的间距
	//填板间距标注参数
	propHashtable.SetValueAt("FILLPLATE_SPACE.pLinePart",id++);	//填板间距关联杆件
	propHashtable.SetValueAt("FILLPLATE_SPACE.fDimDistance",id++);//标注点与标注起始点的间距
	propHashtable.SetValueAt("FILLPLATE_SPACE.fSpace",id++);		//标注起始点与脚钉所在杆件外边缘的间距
	//垫圈标注参数		
	propHashtable.SetValueAt("BOLTPAD.pBolt",id++);				//螺栓垫圈关联螺栓
	propHashtable.SetValueAt("BOLTPAD.iDimPosType",id++);		//0.自动搜索 1.指定位置
	propHashtable.SetValueAt("BOLTPAD.DimPosOffset",id++);		//标注位置偏移值
	propHashtable.SetValueAt("BOLTPAD.fDimPosOffsetX",id++);	//标注位置X坐标偏移值
	propHashtable.SetValueAt("BOLTPAD.fDimPosOffsetY",id++);	//标注位置Y坐标偏移值
	propHashtable.SetValueAt("BOLTPAD.sSpec",id++);             //螺栓垫圈规格
	//螺栓标注参数		
	propHashtable.SetValueAt("BOLTINFO.pBolt",id++);			//螺栓标注信息主螺栓
	propHashtable.SetValueAt("BOLTINFO.pDatumRod",id++);		//螺栓标注信息关联杆件
	propHashtable.SetValueAt("BOLTINFO.iDimPosType",id++);		//0.自动搜索 1.指定位置
	propHashtable.SetValueAt("BOLTINFO.DimPosOffset",id++);		//标注位置偏移值
	propHashtable.SetValueAt("BOLTINFO.fDimPosOffsetX",id++);	//标注位置X坐标偏移值
	propHashtable.SetValueAt("BOLTINFO.fDimPosOffsetY",id++);	//标注位置Y坐标偏移值
	propHashtable.SetValueAt("BOLTINFO.boltSet",id++);			//螺栓集合
	propHashtable.SetValueAt("BOLTINFO.sLabel",id++);            //标注内容
	for(int i=0;i<PROPID_SETSIZE;i++)
	{
		char sKey[100]="";
		sprintf(sKey,"BOLTINFO.pBolt%d",i);
		propHashtable.SetValueAt(sKey,id++);
	}
	//相似形标注参数
	propHashtable.SetValueAt("SIMPOLY.fScale",id++);			//指定相似形比例
	propHashtable.SetValueAt("SIMPOLY.PickPt",id++);			//相似形拾取点
	propHashtable.SetValueAt("SIMPOLY.fPickPtX",id++);			//相似形拾取点X坐标
	propHashtable.SetValueAt("SIMPOLY.fPickPtY",id++);			//相似形拾取点Y坐标
	propHashtable.SetValueAt("SIMPOLY.DimPosOffset",id++);		//相似形位置
	propHashtable.SetValueAt("SIMPOLY.fDimPosOffsetX",id++);	//相似形位置X坐标		
	propHashtable.SetValueAt("SIMPOLY.fDimPosOffsetY",id++);	//相似形位置Y坐标
	propHashtable.SetValueAt("SIMPOLY.vertexList",id++);		//相似形轮廓点列表
	propHashtable.SetValueAt("SIMPOLY.m_nSimPolyAddType",id++);	//相似形添加标注方式 0.选择轮廓点 1.选择拾取点
	for(i=1;i<16;i++)
	{
		char sKey[100]="";
		sprintf(sKey,"SIMPOLY.vertex%d",i);
		propHashtable.SetValueAt(sKey,id++);	//相似形轮廓点前一个杆件
		sprintf(sKey,"SIMPOLY.vertex%d.hPrevPole",i);
		propHashtable.SetValueAt(sKey,id++);	//相似形轮廓点前一个杆件
		sprintf(sKey,"SIMPOLY.vertex%d.hNextPole",i);
		propHashtable.SetValueAt(sKey,id++);	//相似形轮廓点下一个杆件
		sprintf(sKey,"SIMPOLY.vertex%d.x",i);
		propHashtable.SetValueAt(sKey,id++);	//X坐标
		sprintf(sKey,"SIMPOLY.vertex%d.y",i);
		propHashtable.SetValueAt(sKey,id++);	//Y坐标
	}
	//标注模型尺寸
	propHashtable.SetValueAt("DIMMODELSIZE.fDimDistance",id++);
	propHashtable.SetValueAt("DIMMODELSIZE.fSpace",id++);
	propHashtable.SetValueAt("DIMMODELSIZE.fMinX",id++);
	propHashtable.SetValueAt("DIMMODELSIZE.iDimType",id++);
	propHashtable.SetValueAt("DIMMODELSIZE.nSpace",id++);
	propHashtable.SetValueAt("DIMMODELSIZE.nodeSet",id++);
	for(i=0;i<PROPID_SETSIZE;i++)
	{
		char sKey[100]="";
		sprintf(sKey,"DIMMODELSIZE.Node%d",i);
		propHashtable.SetValueAt(sKey,id++);
	}
	//图面尺寸
	propHashtable.SetValueAt("DIMSIZE.DimStart",id++);		//起始点
	propHashtable.SetValueAt("DIMSIZE.fDimStartX",id++);	//图面尺寸标注起始点(相对坐标系下)
	propHashtable.SetValueAt("DIMSIZE.fDimStartY",id++);
	propHashtable.SetValueAt("DIMSIZE.DimEnd",id++);		//终止点
	propHashtable.SetValueAt("DIMSIZE.fDimEndX",id++);		//图面尺寸标注起始点(相对坐标系下)
	propHashtable.SetValueAt("DIMSIZE.fDimEndY",id++);
	propHashtable.SetValueAt("DIMSIZE.DimPos",id++);		//标注位置
	propHashtable.SetValueAt("DIMSIZE.fDimPosX",id++);		//图面尺寸标注位置(相对坐标系下)
	propHashtable.SetValueAt("DIMSIZE.fDimPosY",id++);
	propHashtable.SetValueAt("DIMSIZE.fScale",id++);		//尺寸比例
	propHashtable.SetValueAt("DIMSIZE.iDimDirect",id++);	//标注方向 0.X轴方向 1.Y轴方向 2.延伸方向
	//标注角钢特殊准据
	propHashtable.SetValueAt("DIM_JGZHUN.pLineAngle",id++);		//需要标注特殊准据的角钢
	propHashtable.SetValueAt("DIM_JGZHUN.DimPosOffset",id++);	//标注位置偏移值
	propHashtable.SetValueAt("DIM_JGZHUN.fSpace",id++);			//标注起始点与螺栓之间的距离
	propHashtable.SetValueAt("DIM_JGZHUN.bInside",id++);		//标注方向
	propHashtable.SetValueAt("DIM_JGZHUN.fDimDistance",id++);	//标注位置与标注起始点之间的距离
	//标注螺栓特殊间距
	propHashtable.SetValueAt("DIM_LSSPACE.pPlate",id++);			//需要标注特殊螺栓间距的钢板或角钢
	propHashtable.SetValueAt("DIM_LSSPACE.DimPosOffset",id++);	//标注位置偏移值
	propHashtable.SetValueAt("DIM_LSSPACE.fSpace",id++);		//标注起始点与螺栓之间的距离
	propHashtable.SetValueAt("DIM_LSSPACE.bInside",id++);		//标注方向
	propHashtable.SetValueAt("DIM_LSSPACE.fDimDistance",id++);	//标注位置与标注起始点之间的距离
	propHashtable.SetValueAt("DIM_LSSPACE.hStartLs",id++);		//始端螺栓句柄
	propHashtable.SetValueAt("DIM_LSSPACE.hEndLs",id++);		//终端螺栓句柄
	propHashtable.SetValueAt("DIM_LSSPACE.nLsSpaceNum",id++);		//包含的螺栓间距数
	propHashtable.SetValueAt("DIM_LSSPACE.bMergeDimLsSpace",id++);	//合并标注特殊螺栓间距
	//展开线
	propHashtable.SetValueAt("SPREAD_LINE",id++);				//展开线
	propHashtable.SetValueAt("SPREAD_LINE.m_sLineName",id++);	//展开线名称
	propHashtable.SetValueAt("SPREAD_LINE.m_iDefType",id++);	//0.指定杆件 1.直接指定
	propHashtable.SetValueAt("SPREAD_LINE.m_hDatumPole",id++);	//基准杆件 基准杆件句柄为0时表示为自定义
	propHashtable.SetValueAt("SPREAD_LINE.m_iCurWing",id++);	//基准杆件为角钢时启用该属性
	propHashtable.SetValueAt("SPREAD_LINE.m_fOffsetDist",id++);	//偏移距离
	propHashtable.SetValueAt("SPREAD_LINE.startPt",id++);		//展开线起点坐标
	propHashtable.SetValueAt("SPREAD_LINE.startPt.x",id++);		//展开线起点X坐标
	propHashtable.SetValueAt("SPREAD_LINE.startPt.y",id++);		//展开线起点Y坐标
	propHashtable.SetValueAt("SPREAD_LINE.startPt.z",id++);		//展开线起点Y坐标
	propHashtable.SetValueAt("SPREAD_LINE.endPt",id++);			//展开线终点坐标
	propHashtable.SetValueAt("SPREAD_LINE.endPt.x",id++);		//展开线终点X坐标
	propHashtable.SetValueAt("SPREAD_LINE.endPt.y",id++);		//展开线终点Y坐标
	propHashtable.SetValueAt("SPREAD_LINE.endPt.z",id++);		//展开线终点Y坐标
	//杆件螺栓或法兰信息标注参数
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO",id++);
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.pLinePart",id++);		
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.iDimPosType",id++);		//0.自动搜索 1.指定位置
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.StartOddPosOffset",id++);
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.fStartOddPosOffsetX",id++);
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.fStartOddPosOffsetY",id++);
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.EndOddPosOffset",id++);
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.fEndOddPosOffsetX",id++);
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.fEndOddPosOffsetY",id++);
}
BOOL CDrawMapScriptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pDrawing==NULL&&m_pComponent==NULL)
	{
		m_bInernalStart=FALSE;
		return FALSE;
	}
	//初始化命令列表以及属性ID
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	if(!m_bInernalStart)	//内部启动
	{
		m_propList.SetDividerScale(0.62);//设置分割比例
		//设置属性栏回调函数
		m_propList.SetModifyValueFunc(ModifyItemValue);
		m_propList.SetButtonClickFunc(ButtonClickItem);
		m_propList.SetPropHelpPromptFunc(FirePropHelpPrompt);
		m_propList.SetModifyStatusFunc(ModifyItemStatus);
		InitPropHashtable();	//初始化属性哈希表
	}
	//
	long col_wide_arr[4]={25,65,160,45};
	m_listCmdSuperCtrl.InitListCtrl(col_wide_arr);
	m_listCmdSuperCtrl.m_hPromptWnd=GetDlgItem(IDC_E_CMD_HELP_STR)->GetSafeHwnd();
	//设置菜单回调函数
	m_listCmdSuperCtrl.SetContextMenuFunc(FireContextMenu);
	//设置回调函数 当前选中项发生变化时调用
	m_listCmdSuperCtrl.SetItemChangedFunc(FireItemChanged);
	//设置鼠标左键双击回调函数
	m_listCmdSuperCtrl.SetLButtonDblclkFunc(FireLButtonDblclk);
	m_listCmdSuperCtrl.SetButtonClickFunc(FireLButtonDblclk);
	//设置值修改函数
	m_listCmdSuperCtrl.SetModifyValueFunc(FireModifyCellValue);
	//设置删除函数
	m_listCmdSuperCtrl.SetDeleteItemFunc(FireDeleteItem);
	//-1表示无图像链表
	m_listCmdSuperCtrl.InitImageList(IDR_BAR_SCRIPT);
	//
	long col_wide_arr1[7]={50,55,50,40,40,25,40};
	m_listCmdCommonCtrl.InitListCtrl(col_wide_arr1);
	m_listCmdCommonCtrl.m_bEnableSortItems=TRUE;	//支持排序
	//设置第一列为按十六进制数排序
	m_listCmdCommonCtrl.SetColumnSortType(0,SORTTYPE_HEX_NUM);	
	//设置第二列为按十六进制数排序
	m_listCmdCommonCtrl.SetColumnSortType(1,SORTTYPE_HEX_NUM);	
	//设置第三列为按件号排序
	m_listCmdCommonCtrl.SetColumnSortType(2,SORTTYPE_PARTNO);
	//设置第七列为按整数排序
	m_listCmdCommonCtrl.SetColumnSortType(6,SORTTYPE_INTEGER);
	//初始化CTabCtrl
	m_tabCmdType.DeleteAllItems();
	if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_LINEVIEW)
	{	
		CLineView *pLineView=(CLineView*)m_pComponent;
#ifdef AFX_TARG_ENU_ENGLISH
		m_tabCmdType.InsertItem(0,"Draw Parts");
		m_tabCmdType.InsertItem(1,"dimension");
		if(pLineView->m_bAnalysisMode)	
		{	//force line map also needs segment number dimension wht 11-07-15
			m_tabCmdType.InsertItem(2,"parts details");
			m_tabCmdType.InsertItem(3,"seg num dimension");
		}
		else//total map
			m_tabCmdType.InsertItem(2,"seg num dimension");
#else
		m_tabCmdType.InsertItem(0,"绘制构件");
		m_tabCmdType.InsertItem(1,"尺寸标注");
		if(pLineView->m_bAnalysisMode)	
		{	//司令图也需要段号标注 wht 11-07-15
			m_tabCmdType.InsertItem(2,"构件明细");
			m_tabCmdType.InsertItem(3,"段号标注");
		}
		else//总图
			m_tabCmdType.InsertItem(2,"段号标注");
#endif
	}
	else if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_BOM)
	{
		if(m_pDrawing&&m_pDrawing->m_nSegI.iSeg==-1)
#ifdef AFX_TARG_ENU_ENGLISH
			m_tabCmdType.InsertItem(0,"material summary table");
		else 
			m_tabCmdType.InsertItem(0,"details list");
#else
			m_tabCmdType.InsertItem(0,"材料汇总表");
		else 
			m_tabCmdType.InsertItem(0,"明细表");
#endif
	}
	else if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_STRUVIEW)
	{	//结构图
#ifdef AFX_TARG_ENU_ENGLISH
		m_tabCmdType.InsertItem(GROUP_DRAW_PART-1,"Draw Parts");
		m_tabCmdType.InsertItem(GROUP_DIM_SIMPOLY-1,"similar polygon");
		m_tabCmdType.InsertItem(GROUP_DIM_ODDMENT-1,"ends");
#else
		m_tabCmdType.InsertItem(GROUP_DRAW_PART-1,"绘制构件");
		m_tabCmdType.InsertItem(GROUP_DIM_SIMPOLY-1,"相似形");
		m_tabCmdType.InsertItem(GROUP_DIM_ODDMENT-1,"正负头");
#endif
		//m_tabCmdType.InsertItem(GROUP_SPREADLINE-1,"展开线");
	}
	//完成对象选择后，再显示命令列表 wht 12-03-22
	if(m_bInernalStart)	//内部启动
		FinishSelectObjOper();		//完成选择对象的后续操作
	//初始化命令列表
	if(m_listCmdSuperCtrl.GetCount()<=0)
		DisplaySuperCtrlCmdList();
	if(m_listCmdCommonCtrl.GetItemCount()<=0)
		DisplayCommonCtrlCmdList(m_iOldGroup);
	if(m_iOldGroup>=0)
	{	
		m_tabCmdType.SetCurSel(m_iOldGroup);//选中中断前的属性组
		RefreshCommandList(m_iOldGroup);	//刷新命令列表
	}
	if(m_listCmdCommonCtrl.GetSortColunmnIndex()!=-1)	
	{	//重启后按照之前的排序顺序显示列表项
		int iSubItem=m_listCmdCommonCtrl.GetSortColunmnIndex();
		m_listCmdCommonCtrl.SortItemsBySpecSubItem(iSubItem);
	}
	if(m_pCurCmd)
	{	//当前命令行不为空时需选中重启前的命令行
		if(!m_bSuperListCtrlInUsing)
		{	//重启对话框后选中指定项
			int i=0,n=m_listCmdCommonCtrl.GetItemCount();
			for(i=0;i<n;i++)
			{
				CDrawingCommand *pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(i);
				if(pCmd&&pCmd==m_pCurCmd)
				{
					m_listCmdCommonCtrl.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
					m_listCmdCommonCtrl.EnsureVisible(i,FALSE);	//确保选中项可见 wht 11-07-25
					break;
				}
			}
		}
		else 
		{
			CSuperGridCtrl::CTreeItem *pCurItem=NULL;
			int nCount=m_listCmdSuperCtrl.GetItemCount();
			int nItemCount=m_listCmdSuperCtrl.GetRootCount();
			for(int i=0;i<nItemCount;i++)
			{
				CSuperGridCtrl::CTreeItem *pItem=m_listCmdSuperCtrl.GetRootItem(i);
				if(pItem==NULL)
					continue;
				if(pItem->m_dwPropGroup!=GetSingleWord(m_iOldGroup+1))
					continue;	//不属于当前分组
				CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
				if(pCmd&&pCmd==m_pCurCmd)
				{
					pCurItem=pItem;
					break;
				}
			}
			if(pCurItem)
				m_listCmdSuperCtrl.SelectItem(pCurItem);
		}
	}
	//显示属性栏内容
	DisplayPropertyList();
	//移动窗口到右上角
	CSize sizeMaxScr;
	sizeMaxScr.cx = GetSystemMetrics(SM_CXSCREEN);
	sizeMaxScr.cy = GetSystemMetrics(SM_CYSCREEN);
	CRect rect;
	GetWindowRect(rect);
	rect.left = sizeMaxScr.cx-rect.Width();
	rect.right=sizeMaxScr.cx;
	rect.bottom = rect.Height();
	rect.top  =0;
	MoveWindow(rect,TRUE);
	m_bInernalStart=FALSE;
	return TRUE;  
}

//更新展开线位置
void CDrawMapScriptDlg::UpdateSpreadLinePos()
{
	if(m_pSpreadLine==NULL)
		return;
	CLDSLineAngle *pLineAngle=m_pSpreadLine->GetDatumPole();
	if(pLineAngle==NULL)
		return;
	m_pSpreadLine->m_lineSpread.startPt=pLineAngle->Start();
	m_pSpreadLine->m_lineSpread.endPt=pLineAngle->End();
	f3dPoint wing_vec=pLineAngle->GetWingVecX();
	if(m_pSpreadLine->m_iCurWing==1)
		wing_vec=pLineAngle->GetWingVecY();
	if(fabs(m_pSpreadLine->m_fOffsetDist)>0)
	{
		m_pSpreadLine->m_lineSpread.startPt+=wing_vec*m_pSpreadLine->m_fOffsetDist;
		m_pSpreadLine->m_lineSpread.endPt+=wing_vec*m_pSpreadLine->m_fOffsetDist;
	}
}

bool CalLsSpaceDistAndAngle(CDrawingCommand *pCmd)
{
	if( pCmd==NULL||pCmd->m_pDrawingCom==NULL||
		pCmd->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE||
 		pCmd->m_pDrawingCom->m_eClassType!=DRAW_COM_STRUVIEW)
		return false;
	CStruView *pStruView=(CStruView*)pCmd->m_pDrawingCom;
	CLDSLinePart *pRod=pCmd->m_paraDimLsSpace.spaceDim.GetDatumRod();
	CLDSBolt *pStartBolt=(CLDSBolt*)Ta.Parts.FromHandle(pCmd->m_paraDimLsSpace.spaceDim.hStartLs,CLS_BOLT);
	CLDSBolt *pEndBolt=(CLDSBolt*)Ta.Parts.FromHandle(pCmd->m_paraDimLsSpace.spaceDim.hEndLs,CLS_BOLT);
	if(pStartBolt==NULL||pEndBolt==NULL||pRod==NULL)
		return false;
	f3dLine line(pRod->Start(),pRod->End());
	f3dPoint startPos=pStartBolt->ucs.origin,endPos=pEndBolt->ucs.origin;
	SnapPerp(&startPos,line,startPos);
	SnapPerp(&endPos,line,endPos);
	pCmd->m_paraDimLsSpace.spaceDim.dist=DISTANCE(startPos,endPos);
	//
	UCS_STRU dcs=pStruView->GetDCS();
	coord_trans(startPos,dcs,FALSE);
	coord_trans(endPos,dcs,FALSE);
	pCmd->m_paraDimLsSpace.spaceDim.bCalDimAngle=FALSE;
	pCmd->m_paraDimLsSpace.spaceDim.dim_angle=Cal2dLineAng(startPos.x,startPos.y,endPos.x,endPos.y);
	return true; 
}

//完成选择对象的后续操作
void CDrawMapScriptDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	if(m_iBreakType!=1)
		return;
	//由于选择实体内部重启
	CAD_SCREEN_ENT *pCADEnt=resultList.GetFirst();
	CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
	if(pCADEnt&&pItem&&pItem->m_idProp==GetPropID("SPREAD_LINE.m_hDatumPole"))
	{
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//选中指定属性
		if(pCADEnt->m_pObj
			&&pCADEnt->m_pObj->GetClassTypeId()==CLS_LINEANGLE)
		{	//
			CString sValueText="";
			sValueText.Format("0X%X",pCADEnt->m_pObj->handle);
			m_propList.SetItemPropValue(pItem->m_idProp,sValueText);
			if(m_pSpreadLine)
				m_pSpreadLine->m_hDatumPole=pCADEnt->m_pObj->handle;
			UpdateSpreadLinePos();	//更新展开线位置
			char tem_str[100]="";
			long id=GetPropID("SPREAD_LINE.startPt.x");
			if(GetPropValueStr(id,tem_str)>0)
				m_propList.SetItemPropValue(id,CString(tem_str));
			id=GetPropID("SPREAD_LINE.startPt.y");
			if(GetPropValueStr(id,tem_str)>0)
				m_propList.SetItemPropValue(id,CString(tem_str));
			id=GetPropID("SPREAD_LINE.startPt.z");
			if(GetPropValueStr(id,tem_str)>0)
				m_propList.SetItemPropValue(id,CString(tem_str));
			id=GetPropID("SPREAD_LINE.endPt.x");
			if(GetPropValueStr(id,tem_str)>0)
				m_propList.SetItemPropValue(id,CString(tem_str));
			id=GetPropID("SPREAD_LINE.endPt.y");
			if(GetPropValueStr(id,tem_str)>0)
				m_propList.SetItemPropValue(id,CString(tem_str));
			id=GetPropID("SPREAD_LINE.endPt.z");
			if(GetPropValueStr(id,tem_str)>0)
				m_propList.SetItemPropValue(id,CString(tem_str));
		}
	}
	else if(pCADEnt&&pItem&&pItem->m_idProp==GetPropID("BOLTINFO.pDatumRod")
		&&m_pCurCmd&&m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
	{
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//选中指定属性
		CLDSObject *pObj=pCADEnt->m_pObj;
		if(pObj&&pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()
			&&((CLDSPart*)pObj)->IsLinePart())
		{	//
			CString sValueText="";
			sValueText.Format("0X%X",pCADEnt->m_pObj->handle);
			m_propList.SetItemPropValue(pItem->m_idProp,sValueText);
			m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.SetDatumRod((CLDSLinePart*)pObj);
			//选择杆件后重新标注螺栓明细
			m_pCurCmd->Rerun();
		}
	}
	else if(pItem&&m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO
		&&pItem->m_idProp==GetPropID("BOLTINFO.boltSet"))
	{	//添加螺栓到当前螺栓组，并将螺栓从原有螺栓组删除，并重新运行相关命令 wht 12-03-22
		BOLTSET boltSet;
		CAD_SCREEN_ENT *pCADEnt=NULL;
		long bolt_D=m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.GetBoltD();
		long bolt_L=m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.GetBoltL();
		for(pCADEnt=resultList.GetFirst();pCADEnt;pCADEnt=resultList.GetNext())
		{
			if(pCADEnt&&pCADEnt->m_pObj&&pCADEnt->m_pObj->GetClassTypeId()==CLS_BOLT)
			{	
				CLDSBolt *pBolt=(CLDSBolt*)pCADEnt->m_pObj;
				if(m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.ContainsBolt(pBolt->handle))
					continue;	//螺栓已在当前组
				if(bolt_D!=pBolt->get_d()&&bolt_L!=ftoi(pBolt->get_L()))
					continue;	//螺栓规格不同不能添加
				boltSet.append(pBolt);
				//在当前命令中添加选择的螺栓
				m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.AddBolt(pBolt);
			}
		}
		//从其余命令中移除添加到当前组的螺栓，避免重复标注
		CHashList<CDrawingCommand*> cmdHashList;
		CStruView *pStruView=(CStruView*)m_pCurCmd->m_pDrawingCom;
		pStruView->RemovBoltFromDimBoltInfoCmd(m_pCurCmd,boltSet,cmdHashList);
		//重新运行当前命令
		m_pCurCmd->Rerun();
		//重新运行时关联命令
		CDrawingCommand **ppCmd=NULL;
		for(ppCmd=cmdHashList.GetFirst();ppCmd;ppCmd=cmdHashList.GetNext())
			(*ppCmd)->Rerun();
		
	}
	else if(pCADEnt&&pItem&&m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE&&
			(pItem->m_idProp==GetPropID("DIM_LSSPACE.hStartLs")||
			 pItem->m_idProp==GetPropID("DIM_LSSPACE.hEndLs")))
	{
		if(pCADEnt->m_pObj&&pCADEnt->m_pObj->GetClassTypeId()==CLS_BOLT)
		{
			CAD_SCREEN_ENT *pSecCadEnt=resultList.GetNext();
			if(pItem->m_idProp==GetPropID("DIM_LSSPACE.hStartLs"))
			{	
				m_pCurCmd->m_paraDimLsSpace.spaceDim.hStartLs=pCADEnt->m_pObj->handle;
				if(pSecCadEnt&&pSecCadEnt->m_pObj->GetClassTypeId()==CLS_BOLT)
					m_pCurCmd->m_paraDimLsSpace.spaceDim.hEndLs=pSecCadEnt->m_pObj->handle;
			}
			else
			{
				m_pCurCmd->m_paraDimLsSpace.spaceDim.hEndLs=pCADEnt->m_pObj->handle;
				if(pSecCadEnt&&pSecCadEnt->m_pObj->GetClassTypeId()==CLS_BOLT)
					m_pCurCmd->m_paraDimLsSpace.spaceDim.hStartLs=pSecCadEnt->m_pObj->handle;
			}
			CalLsSpaceDistAndAngle(m_pCurCmd);
			m_pCurCmd->Rerun();
		}
	}
	else if(pItem&&m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE
		&&pItem->m_idProp==GetPropID("DIMMODELSIZE.nodeSet"))
	{
		CAD_SCREEN_ENT *pCADEnt=resultList.GetFirst();
		if(pCADEnt&&pCADEnt->m_pObj&&pCADEnt->m_pObj->GetClassTypeId()==CLS_NODE)
		{	//添加节点 wht 11-07-13
			m_pCurCmd->m_paraDimModelSize.AppendDimNode((CLDSNode*)pCADEnt->m_pObj);
		}
	}
	else if(pItem&&m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY&&resultList.GetNodeNum()>=3)
	{	//依次选择组成相似形轮廓的杆件，第i根与第i+1根杆件求交 最后一根不与第一根求交 wht 11-07-16
		m_pCurCmd->m_paraSimPoly.simPolyVertexList.Empty();
		f3dPoint geom_center;
		int i=0,nn=resultList.GetNodeNum();
		//TODO:不知道海涛当初为什么不允许最后一根与第一根求交 wjh-2017.87.25
		//for(i=0;i<nn-1;i++)
		for(i=0;i<nn;i++)
		{
			CAD_SCREEN_ENT *pCurObj=resultList.GetByIndex(i);
			CAD_SCREEN_ENT *pNextObj=resultList.GetByIndex((i+1)%nn);
			if(pCurObj==NULL||pNextObj==NULL)
				break;	
			if(pCurObj->m_pObj==NULL||pNextObj->m_pObj==NULL)
				break;
			AcDbEntity *pCurEnt=NULL,*pNextEnt=NULL;
			pCurEnt=m_pCurCmd->m_pDrawingCom->FindCADEntByHandle(pCurObj->m_pObj->handle);
			pNextEnt=m_pCurCmd->m_pDrawingCom->FindCADEntByHandle(pNextObj->m_pObj->handle);
			if(pCurEnt==NULL||pNextEnt==NULL)
				break;
			if(!pCurEnt->isKindOf(AcDbLine::desc())||!pNextEnt->isKindOf(AcDbLine::desc()))
				return;
			AcDbLine *pCurLine=(AcDbLine*)pCurEnt;
			AcDbLine *pNextLine=(AcDbLine*)pNextEnt;
			f3dLine cur_line,next_line;
			Cpy_Pnt(cur_line.startPt,pCurLine->startPoint());
			Cpy_Pnt(cur_line.endPt,pCurLine->endPoint());
			Cpy_Pnt(next_line.startPt,pNextLine->startPoint());
			Cpy_Pnt(next_line.endPt,pNextLine->endPoint());
			f3dPoint inters_pt;
			int nRetCode=Int3dll(cur_line,next_line,inters_pt);
			if(nRetCode<=0&&nRetCode!=-2)
				break;
			SIMPOLY_VERTEX *pVertex=m_pCurCmd->m_paraSimPoly.simPolyVertexList.append();
			pVertex->hDatumRod=pCurObj->m_pObj->handle;
			pVertex->hViceRod=pCurObj->m_pObj->handle;
			pVertex->vertex=inters_pt;
			geom_center+=inters_pt;
		}
		int NN=nn;	//nn-1
		if(i==NN)
		{
			geom_center.x/=(NN);
			geom_center.y/=(NN);
			m_pCurCmd->m_paraSimPoly.fPickPtX=geom_center.x;
			m_pCurCmd->m_paraSimPoly.fPickPtY=geom_center.y;
			m_pCurCmd->Run();
		}
		else
			m_pCurCmd->m_paraSimPoly.simPolyVertexList.Empty();
	}
	else if(pItem&&pCADEnt&&pCADEnt->m_pObj&&m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY
		&&m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum()>0)
	{
		int i=1;
		SIMPOLY_VERTEX *pVertex=NULL;
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//选中指定属性
		for(pVertex=m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
		pVertex=m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetNext())
		{
			char sKey1[50]="",sKey2[50]="",sKey3[50]="";
			sprintf(sKey1,"SIMPOLY.vertex%d.hPrevPole",i);
			sprintf(sKey2,"SIMPOLY.vertex%d.hNextPole",i);
			sprintf(sKey3,"SIMPOLY.vertex%d",i);
			CString sValueText="";
			if(CDrawMapScriptDlg::GetPropID(sKey1)==pItem->m_idProp
				||CDrawMapScriptDlg::GetPropID(sKey2)==pItem->m_idProp)
			{
				sValueText.Format("0X%X",pCADEnt->m_pObj->handle);
				if(CDrawMapScriptDlg::GetPropID(sKey1)==pItem->m_idProp)
					pVertex->hDatumRod=pCADEnt->m_pObj->handle;
				else 
					pVertex->hViceRod=pCADEnt->m_pObj->handle;
				m_propList.SetItemPropValue(pItem->m_idProp,sValueText);
				break;
			}
			else if(CDrawMapScriptDlg::GetPropID(sKey3)==pItem->m_idProp)
			{
				resultList.GetFirst();
				CAD_SCREEN_ENT *pNextCADEnt=resultList.GetNext();
				CLDSObject *pNextObj=NULL;
				if(pNextCADEnt)
					pNextObj=pNextCADEnt->m_pObj;
				pVertex->hDatumRod=pCADEnt->m_pObj->handle;
				sValueText.Format("0X%X",pCADEnt->m_pObj->handle);
				m_propList.SetItemPropValue(CDrawMapScriptDlg::GetPropID(sKey1),sValueText);
				if(pNextObj)
				{	
					pVertex->hViceRod=pNextObj->handle;
					sValueText.Format("0X%X",pNextObj->handle);
					m_propList.SetItemPropValue(CDrawMapScriptDlg::GetPropID(sKey2),sValueText);
				}
				break;
			}
			i++;
		}
	}
}

void CDrawMapScriptDlg::DisplaySuperCtrlCmdList()
{
	CListCtrlItemInfo *lpRoot=NULL;
	CDrawingCommand *pCmd=NULL;
	CSuperGridCtrl::CTreeItem *pTreeItem=NULL,*pGroupItem=NULL;
	m_listCmdSuperCtrl.DeleteAllItems();	//清空链表
	if(m_pDrawing==NULL||m_pComponent==NULL)
		return;
	hashRodBoltOrFlInfoList.Empty();
	for(pCmd=m_pComponent->cmdList.GetFirst();pCmd;pCmd=m_pComponent->cmdList.GetNext())
	{
		if(pCmd->m_eCmdType==CMD_DIM_BOLTORFL_INFO)	//填充杆件螺栓信息或法兰信息标注命令列表
			hashRodBoltOrFlInfoList.SetValue(pCmd->m_paraBoltOrFlInfo.hLinePart,pCmd);
		char sCmdId[100]="",sCmdText[200]="";
		pCmd->GetCmdInfo(sCmdId,sCmdText);
		lpRoot=new CListCtrlItemInfo();
		lpRoot->SetImage(pCmd->m_stateCmd);	//根据命令状态设置不同的图标
		lpRoot->AddSubItemText(_T(""),TRUE);
		lpRoot->AddSubItemText(sCmdId,TRUE);
		lpRoot->AddSubItemText(sCmdText,TRUE);
		if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY
			&&(pCmd->m_paraSimPoly.nSimPolyCmdType==1
			||pCmd->m_paraSimPoly.nSimPolyCmdType==0))	//添加相似形
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpRoot->SetSubItemText(1,"Assistant",TRUE);
			lpRoot->AddSubItemText("Add");
#else
			lpRoot->SetSubItemText(1,"辅助",TRUE);
			lpRoot->AddSubItemText("添加");
#endif
			lpRoot->SetControlType(3,GCT_GRAYBUTTON);//按钮
		}
		else 
		{
			if(pCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
				lpRoot->AddSubItemText(_T("√"),TRUE);
#else
				lpRoot->AddSubItemText(_T("√"),TRUE);
#endif
			else
				lpRoot->AddSubItemText(_T("  "),TRUE);//×
		}
		pTreeItem=m_listCmdSuperCtrl.InsertRootItem(lpRoot);
		pTreeItem->m_idProp=(long)pCmd;
		if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_LINEVIEW)
		{	//单线图(总图,司令图)
			CLineView *pLineView=(CLineView*)m_pComponent;
			if(pCmd->m_eCmdType==CMD_DRAW_LINEPART)
				pTreeItem->m_dwPropGroup=GetSingleWord(1);
			else if(pCmd->m_eCmdType==CMD_DIM_MODELSIZE)
				pTreeItem->m_dwPropGroup=GetSingleWord(2);
			else if(pCmd->m_eCmdType==CMD_DIM_SEGI)
			{
				if(pLineView->m_bAnalysisMode)	//司令图也需要段号标注信息 wht 11-07-15
					pTreeItem->m_dwPropGroup=GetSingleWord(4);	//司令图段号标注
				else 
					pTreeItem->m_dwPropGroup=GetSingleWord(3);	//总图段号标注
			}
			else if(pLineView->m_bAnalysisMode&&(
				pCmd->m_eCmdType==CMD_DIM_PARTSPEC||pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT))
				pTreeItem->m_dwPropGroup=GetSingleWord(3);	//司令图构件明细
		}
		else if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_BOM)
			pTreeItem->m_dwPropGroup=GetSingleWord(1);
		else if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_STRUVIEW)
		{
			if(pCmd->m_eCmdType==CMD_DRAW_PART||pCmd->m_eCmdType==CMD_DRAW_LINEPART
				||pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN
				||pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE
				||pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE
				||pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
				pTreeItem->m_dwPropGroup=GetSingleWord(GROUP_DRAW_PART);
			else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)
				pTreeItem->m_dwPropGroup=GetSingleWord(GROUP_DIM_SIMPOLY); 
			else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
				pTreeItem->m_dwPropGroup=GetSingleWord(GROUP_DIM_ODDMENT);
		}
		//SuperGridCtrl 设置图标是第三个图标不能正常显示 此处暂做如下处理 wht 10-11-30
		if(pCmd->m_stateCmd<2)
			pTreeItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);	//设置命令行状态
		else 
			pTreeItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd+1);	//设置命令行状态
	}
	//更新视图名称
	m_sScriptName.Format("%s-%s",(char*)m_pDrawing->m_sDrawingName,(char*)m_pComponent->m_sName);
	UpdateData(FALSE);
	m_listCmdSuperCtrl.Redraw();
}

typedef CDrawingCommand* DRWINGCMD_PTR;	
static int CompareCmdFeature(const DRWINGCMD_PTR &pCmd1,const DRWINGCMD_PTR &pCmd2)
{
	if(pCmd1->feature>pCmd2->feature)
		return 1;
	else if(pCmd1->feature<pCmd2->feature)
		return -1;
	return 0;
}
static int CompareCmdDrawingSerailNo(const DRWINGCMD_PTR &pCmd1,const DRWINGCMD_PTR &pCmd2)
{
	if(pCmd1->GetDrawingSerialNo()==0&&pCmd2->GetDrawingSerialNo()>0)
		return 1;
	else if(pCmd1->GetDrawingSerialNo()>0&&pCmd2->GetDrawingSerialNo()==0)
		return -1;	
	else if(pCmd1->GetDrawingSerialNo()>pCmd2->GetDrawingSerialNo())
		return 1;
	else if(pCmd1->GetDrawingSerialNo()<pCmd2->GetDrawingSerialNo())
		return -1;
	return 0;
}
//使用普通ListView显示命令列表
void CDrawMapScriptDlg::DisplayCommonCtrlCmdList(int iGroup/*=0*/)
{
	if(m_pDrawing==NULL||m_pComponent==NULL)
		return;
	hashPartSpecList.Empty();
	m_listCmdCommonCtrl.DeleteAllItems();	//清空链表
	CDrawingCommand *pCmd=NULL;
	int nCmdCount=m_pComponent->cmdList.GetNodeNum();
	if(nCmdCount<=0)
		return;
	BOOL bSortByFeature=TRUE;
	DRWINGCMD_PTR *cmd_arr = new DRWINGCMD_PTR[nCmdCount];
	int i=0,nDrawPart=0;
	m_nMaxCmdDrawingSerialNo=0;
	for(pCmd=m_pComponent->cmdList.GetFirst();pCmd;pCmd=m_pComponent->cmdList.GetNext())
	{
		if(pCmd->feature<0)
			bSortByFeature=FALSE;
		int nCurSerialNo=pCmd->GetDrawingSerialNo();
		if(nCurSerialNo>m_nMaxCmdDrawingSerialNo)
			m_nMaxCmdDrawingSerialNo=nCurSerialNo;
		cmd_arr[i]=pCmd;
		i++;
	}
	if(bSortByFeature)
		CQuickSort<DRWINGCMD_PTR>::QuickSort(cmd_arr,nCmdCount,CompareCmdFeature);
	else				
		CQuickSort<DRWINGCMD_PTR>::QuickSort(cmd_arr,nCmdCount,CompareCmdDrawingSerailNo);
	for(i=0;i<nCmdCount;i++)
	{
		pCmd=cmd_arr[i];
		InsertCmdToListCtrl(iGroup,pCmd,-1);
	}
	delete []cmd_arr;
}
int CDrawMapScriptDlg::InsertCmdToListCtrl(int iGroup,CDrawingCommand *pCmd,int iInsertIndex)
{
	CStringArray str_arr;
	str_arr.SetSize(7);
	{
		if(pCmd==NULL)
			return -1;
		if(iGroup==0)
		{	//构件绘制命令
			if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)		//填充构件明细标注列表
				hashPartSpecList.SetValue(pCmd->m_paraPartSpec.hObj,pCmd);
			else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)	//填充正负头标注列表
				hashOddmentList.SetValue(pCmd->m_paraOddment.hLinePart,pCmd);
			else if(pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)	//填充脚钉间距标注列表
				hashFootNailSpaceList.SetValue(pCmd->m_paraDimFootNailSpace.hLinePart,pCmd);
			else if(pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)	//填充填板间距标注列表
				hashFillPlateSpaceList.SetValue(pCmd->m_paraDimFillPlateSpace.hLinePart,pCmd);
			if(pCmd->m_eCmdType!=CMD_DRAW_LINEPART&&pCmd->m_eCmdType!=CMD_DRAW_PART
				&&pCmd->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN&&pCmd->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE
				&&pCmd->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE&&pCmd->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE
				&&pCmd->m_eCmdType!=CMD_DIM_BOLTPAD&&pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
				return -1;
		}
		else if(iGroup==1)
		{	//标注正负头
			if(pCmd->m_eCmdType!=CMD_DIM_ODDMENT)
				return -1;
			if(pCmd->m_pDrawingCom&&!pCmd->m_pDrawingCom->m_bFirstCreate)
			{	//未生执行过的命令正负头命令均显示
				if(!pCmd->m_paraOddment.bDimStartOdd&&!pCmd->m_paraOddment.bDimEndOdd)
					return -1;	//始终端正负头都不需要标注的杆件不需要显示 wht 11-07-25
			}
		}
		if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO&&pCmd->m_paraBoltGroupInfo.lsGroupDim.GetBoltNum()<=0)
			return -1;	//不显示螺栓个数为0的，螺栓标注信息 wht 12-03-22
		char sCmdId[50]="",sPartNo[16]="";
		long hPart=pCmd->GetCmdInfo(sCmdId,NULL,sPartNo);
		CSuperSmartPtr<CLDSPart>pPart=Ta.FromPartHandle(hPart,CLS_PLATE,CLS_PARAMPLATE);
		if(pPart.IsHasPtr()&&pPart->GetClassTypeId()==CLS_PLATE)
			str_arr[0].Format("0X%X",pPart.PlatePointer()->designInfo.m_hBaseNode);
		else if(pPart.IsHasPtr()&&pPart->GetClassTypeId()==CLS_PARAMPLATE&&pPart.ParamPlatePointer()->GetBaseNode())
			str_arr[0].Format("0X%X",pPart.ParamPlatePointer()->GetBaseNode()->handle);
		else
			str_arr[0]="";
		str_arr[1]=sCmdId;
		if( pCmd->m_eCmdType==CMD_DRAW_PART&&pCmd->m_paraPart.pPart!=NULL&&
			pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT&&pCmd->m_paraPart.para.bolt.front0_side1==0)
		{	//立放螺栓
			CLDSBolt* pDimBolt=(CLDSBolt*)pCmd->m_paraPart.pPart;
			if(pCmd->m_paraPart.para.bolt.bOriginal)
				str_arr[2].Format("M%dX%.0f",pDimBolt->get_d(),pDimBolt->get_L());
			else
				str_arr[2].Format("Φ%.1f孔",pDimBolt->get_d()+pDimBolt->hole_d_increment);
		}
		else
			str_arr[2]=sPartNo;
		str_arr[3]=pCmd->GetDrawPartName();
		if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[3]="Space";
#else
			str_arr[3]="间距";
#endif
		else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		{
			if(!pCmd->m_paraDimJgZhun.zhun_dim.bDimZhunJu)
				return -1;	//只绘制螺栓心线不标注特殊准距的准线绘制命令，不在命令列表中显示
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[3]="G";
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			str_arr[3]="washer";
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			str_arr[3]="blots group";
#else
			str_arr[3]="准据";
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			str_arr[3]="垫圈";
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			str_arr[3]="螺栓组";
#endif
		//
		if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE
			||pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN
			||pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE
			||pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE
			||pCmd->m_eCmdType==CMD_DIM_BOLTPAD
			||pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[4]="dimension";
		else if(pCmd->m_eCmdType==CMD_DRAW_LINEPART)
			str_arr[4]="single line";
		else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
			str_arr[4]="oddment";
#else
			str_arr[4]="标注";
		else if(pCmd->m_eCmdType==CMD_DRAW_LINEPART)
			str_arr[4]="单线";
		else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
			str_arr[4]="负头";
#endif
		else 
		{
			char sDrawType[50]="";
			int h0v1e2=pCmd->GetDrawType(sDrawType);
			str_arr[4]=sDrawType;
		}
		if(pCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[5]="√";
#else
			str_arr[5]="√";
#endif
		else
			str_arr[5]=" ";
		if(pCmd->m_eCmdType==CMD_DRAW_PART&&pCmd->GetDrawingSerialNo()>0)
		{
			str_arr[6].Format("%d",pCmd->GetDrawingSerialNo());
		}
		else 
			str_arr[6]="";
		int iCurSel=m_listCmdCommonCtrl.InsertItemRecord(iInsertIndex,str_arr);
		if(iCurSel>=0)
			m_listCmdCommonCtrl.SetItemData(iCurSel,(DWORD)pCmd);
		return iCurSel;
	}
}

//显示杆件螺栓信息或法兰信息
void CDrawMapScriptDlg::UpdateRodBoltOrFlInfoPropList(CPropTreeItem *pParentItem,
													  CDrawingCommand *pCmd,BOOL bUpdate/*=FALSE*/)
{
	if(pParentItem==NULL||pCmd==NULL||pCmd->m_paraBoltOrFlInfo.pLinePart==NULL||
		pCmd->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
		return;
	if(!pCmd->m_paraBoltOrFlInfo.bDimStartOdd&&!pCmd->m_paraBoltOrFlInfo.bDimEndOdd)
		return;	//始终端都未标注
	char valueStr[100]="";
	CItemInfo* lpInfo=NULL;
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pSonPropItem=NULL;
	if(bUpdate)
	{
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO"));
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.pLinePart"));		
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.iDimPosType"));
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset"));
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX"));
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY"));
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset"));
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX"));
		m_propList.DeleteItemByPropId(GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetY"));
	}
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar16 sName("bolt");
	if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
		sName.Copy("flange");
#else
	CXhChar16 sName("螺栓");
	if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
		sName.Copy("法兰");
#endif
	CXhChar16 sRodName("%s",pCmd->m_paraBoltOrFlInfo.pLinePart->GetPartTypeName());
	//绘制构件参数
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("%s%s message",(char*)sRodName,(char*)sName);
#else
	lpInfo->m_strPropName.Format("%s%s信息",(char*)sRodName,(char*)sName);
#endif
	lpInfo->m_strPropHelp = lpInfo->m_strPropName;
	pGroupItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
	pGroupItem->m_idProp = GetPropID("ROD_BOLTORFL_INFO");
	if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
	//选择位置
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Search Auto|1.Specify";
	lpInfo->m_strPropName.Format("dimension position type");
	lpInfo->m_strPropHelp = "type of dimension position";
#else
	lpInfo->m_cmbItems="0.自动搜索|1.指定位置";
	lpInfo->m_strPropName.Format("标注位置类型");
	lpInfo->m_strPropHelp = "标注位置类型";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
	pPropItem->m_idProp=GetPropID("ROD_BOLTORFL_INFO.iDimPosType");
	if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,2);
	//
	UpdateRodStartEndInfoPropList(&m_propList,pGroupItem,pCmd);
}

//显示构件编号属性
void CDrawMapScriptDlg::UpdatePartSpecPropertyList(CPropTreeItem *pParentItem,CDrawingCommand *pCmd,
												   BOOL bSonItem,BOOL bUpdate/*=FALSE*/)
{
	if(pParentItem==NULL||pCmd==NULL||pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
		return;
	int iSonCmdType=-1;
	if(bSonItem)
		iSonCmdType=1;
	char valueStr[100]="";
	CItemInfo* lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	if(bUpdate)
	{
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.bDrawRefLine"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.bDimGuige"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.iMirMsg"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.bDimPoleSpec"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.fPoleDimPosCoef"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.GuigeOffset"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.fGuigeOffsetX"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.fGuigeOffsetY"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.DimPosOffset"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.fDimPosOffsetX"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.fDimPosOffsetY"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.BasePosOffset"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.fBasePosOffsetX"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.fBasePosOffsetY"));
		m_propList.DeleteItemByPropId(GetPropID("PARTSPEC.MovePartSpec"));
	}
	if(pCmd->m_bExecute)
	{	//是否绘制引出线
		CLDSPart *pPart=NULL;
		if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
			pPart=(CLDSPart*)pCmd->m_paraPartSpec.pObj;
		if(pPart)
		{	//构件明细对称方式 0:X轴对称；1:Y轴对称;其余不对称处理
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Axis Mirror|1.Y Axis Mirror|2.No Mirror|";
			lpInfo->m_strPropName.Format("mirror type");
			lpInfo->m_strPropHelp = "mirror type of parts number dimension";
#else
			lpInfo->m_cmbItems="0.X轴对称|1.Y轴对称|2.无对称|";
			lpInfo->m_strPropName.Format("对称方式");
			lpInfo->m_strPropHelp = "构件编号标注时的对称方式。";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.iMirMsg");
			if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,iSonCmdType);
		}
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName.Format("draw indication line");
		lpInfo->m_strPropHelp = "whether to draw indication line while dimension parts details";
#else
		lpInfo->m_cmbItems="是|否";
		lpInfo->m_strPropName.Format("绘制引出线");
		lpInfo->m_strPropHelp = "标注构件明细时是否绘制引出线。";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp=GetPropID("PARTSPEC.bDrawRefLine");
		if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,iSonCmdType);
		//
		if(pPart&&pPart->IsLinePart())
		{	//标注杆件明细(长度及规格)
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("dimension rod details");
			lpInfo->m_strPropHelp = "dimension the length and specifications of rods";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName.Format("标注杆件明细");
			lpInfo->m_strPropHelp = "标注杆件长度以及规格";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.bDimPoleSpec");
			if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,iSonCmdType);
			//该属性暂不显示
			/*
			//杆件标注位置系数
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("杆件标注位置系数");
			lpInfo->m_strPropHelp = "杆件标注位置系数。";
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.fPoleDimPosCoef");
			if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,iSonCmdType);
			*/
		}
		if(pCmd->m_paraPartSpec.bDrawRefLine)
		{	//调整构件明细位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("indication line and its detailed location");
			lpInfo->m_strPropHelp = "be used to adjust the positions of indication lines and parts.";
#else
			lpInfo->m_strPropName.Format("引出线及明细位置");
			lpInfo->m_strPropHelp = "用来整体调整引出线和构件明细位置。";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.MovePartSpec");
		}

#ifdef AFX_TARG_ENU_ENGLISH
		char sTemp[10]="number";
		if(pPart)
			strcpy(sTemp,"details");
#else
		char sTemp[10]="编号";
		if(pPart)
			strcpy(sTemp,"明细");
#endif
		//编号位置偏移量
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("%s position offset",sTemp);
		lpInfo->m_strPropHelp.Format("%s position offset",sTemp);
#else
		lpInfo->m_strPropName.Format("%s位置偏移量",sTemp);
		lpInfo->m_strPropHelp.Format("%s位置偏移量。",sTemp);
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp=GetPropID("PARTSPEC.DimPosOffset");
		if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
		//编号位置X坐标偏移值
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("Axis X");
		lpInfo->m_strPropHelp.Format("%s dimension position offset value in X axis direction.",sTemp);
#else
		lpInfo->m_strPropName.Format("X轴方向");
		lpInfo->m_strPropHelp.Format("%s标注位置X轴方向偏移量。",sTemp);
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp=GetPropID("PARTSPEC.fDimPosOffsetX");
		if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
		//编号位置Y坐标偏移值
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("Axis Y");
		lpInfo->m_strPropHelp.Format("%s dimension position offset value in Y axis direction.",sTemp);
#else
		lpInfo->m_strPropName.Format("Y轴方向");
		lpInfo->m_strPropHelp.Format("%s标注位置Y轴方向偏移量。",sTemp);
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp=GetPropID("PARTSPEC.fDimPosOffsetY");
		if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
		//
		if(pPart&&pPart->IsPlate())
		{	//规格位置偏移量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("spec dimension position offset");
			lpInfo->m_strPropHelp = "specification dimension position offset value.";
#else
			lpInfo->m_strPropName.Format("规格位置偏移量");
			lpInfo->m_strPropHelp = "规格标注位置偏移量。";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.GuigeOffset");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//规格标注位置X坐标偏移值
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis X");
			lpInfo->m_strPropHelp = "specifications dimension position offset value in axis X relative to number position.";
#else
			lpInfo->m_strPropName.Format("X轴方向");
			lpInfo->m_strPropHelp = "规格标注位置相对与编号位置在X轴方向的偏移量。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp=GetPropID("PARTSPEC.fGuigeOffsetX");
			if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
			//规格标注位置Y坐标偏移值
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis Y");
			lpInfo->m_strPropHelp = "specifications dimension position offset value in axis X relative to number position."; 
#else
			lpInfo->m_strPropName.Format("Y轴方向");
			lpInfo->m_strPropHelp = "规格标注位置相对与编号位置在Y轴方向的偏移量。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp=GetPropID("PARTSPEC.fGuigeOffsetY");
			if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
		}
		//
		if(pCmd->m_paraPartSpec.bDrawRefLine)
		{	//数据点位置偏移量 wht 11-07-25
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("data point position offset");
			lpInfo->m_strPropHelp = "data point position offset value.";
#else
			lpInfo->m_strPropName.Format("数据点位置偏移量");
			lpInfo->m_strPropHelp = "数据点位置偏移量。";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.BasePosOffset");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//标注位置X坐标偏移值
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis X");
			lpInfo->m_strPropHelp = "data point offset value in axis direction.";
#else
			lpInfo->m_strPropName.Format("X轴方向");
			lpInfo->m_strPropHelp = "数据点位置X轴方向偏移量。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp=GetPropID("PARTSPEC.fBasePosOffsetX");
			if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
			//标注位置Y坐标偏移值
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis Y");
			lpInfo->m_strPropHelp = "data point position offset value in axis Y.";
#else
			lpInfo->m_strPropName.Format("Y轴方向");
			lpInfo->m_strPropHelp = "数据点位置Y轴方向偏移量。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp=GetPropID("PARTSPEC.fBasePosOffsetY");
			if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
		}
	}
}

BOOL CDrawMapScriptDlg::IsSpreadLinePropGroup(int iCurSel)
{
	if(iCurSel==GROUP_SPREADLINE-1&&m_pComponent->m_eClassType==DRAW_COM_STRUVIEW)
		return TRUE;
	return FALSE;
}

void CDrawMapScriptDlg::DisplayPropertyList(int iCurSel/*=0*/)
{
	CItemInfo* lpInfo;
	CPropTreeItem *pGroupItem,*pPropItem,*pSonPropItem,*pRoot=m_propList.GetRootItem(); //
	char valueStr[100]="";
	CString sPartName;
	m_propList.CleanTree();
	if(!IsSpreadLinePropGroup(iCurSel)&&iCurSel!=GROUP_DRAW_PART-1)
	{
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "command para";
		lpInfo->m_strPropHelp = "command parameter";
#else
		lpInfo->m_strPropName = "命令参数";
		lpInfo->m_strPropHelp = "命令参数";
#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = GetPropID("CmdPara");
		if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
		//是否执行该命令
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName = "enable this command";
		lpInfo->m_strPropHelp = "can pass this parameter controls, whether to execute the current command.";
#else
		lpInfo->m_cmbItems="是|否";
		lpInfo->m_strPropName = "启用该命令";
		lpInfo->m_strPropHelp = "可以通过该参数控制是否执行当前命令。";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp=GetPropID("m_bExecute");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		pPropItem->SetReadOnly();
	}
	else if(IsSpreadLinePropGroup(iCurSel))
	{	//展开线
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "spreading line";
		lpInfo->m_strPropHelp = "spreading line";
#else
		lpInfo->m_strPropName = "展开线";
		lpInfo->m_strPropHelp = "展开线";
#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = GetPropID("SPREAD_LINE");
		if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
		//展开线名称
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("name");
		lpInfo->m_strPropHelp = "name of spreading line";
#else
		lpInfo->m_strPropName.Format("名称");
		lpInfo->m_strPropHelp = "展开线名称。";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_sLineName");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		//定位方式:0.指定杆件 1.直接指定
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Specify Rod|1.Directly Specify|";
		lpInfo->m_strPropName.Format("positioning way");
		lpInfo->m_strPropHelp = "spread line positioning ";
#else
		lpInfo->m_cmbItems="0.指定杆件|1.直接指定|";
		lpInfo->m_strPropName.Format("定位方式");
		lpInfo->m_strPropHelp = "展开线定位方式。";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_iDefType");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
	}
	if(m_pCurCmd&&!IsSpreadLinePropGroup(iCurSel))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		CString sName="draw parameter";
		if(m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
			sName.Format("parts draw parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DRAW_TABLE)
			sName.Format("chart draw parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
			sName.Format("parts detailed dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY)
			sName.Format("similar polygon dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ODDMENT)
			sName.Format("oddment dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
			sName.Format("foot nail space dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
			sName.Format("filler plate interval dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			sName.Format("bolt washer dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
			sName.Format("angle special criterion dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
			sName.Format("bolt special interval dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MAPSIZE)
			sName.Format("surface size dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
			sName.Format("model size dimension position");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			sName.Format("bolt information dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SEGI)
			sName.Format("segment number dimension parameter");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
			sName.Format("angle joint dimension parameter");
#else
		CString sName="绘制参数";
		if(m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
			sName.Format("构件绘制参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DRAW_TABLE)
			sName.Format("表格绘制参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
			sName.Format("构件明细标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY)
			sName.Format("相似形标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ODDMENT)
			sName.Format("正负头标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
			sName.Format("脚钉间距标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
			sName.Format("填板间距标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			sName.Format("螺栓垫圈标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
			sName.Format("角钢特殊准据标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
			sName.Format("螺栓特殊间距标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MAPSIZE)
			sName.Format("图面尺寸标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
			sName.Format("模型尺寸标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			sName.Format("螺栓信息标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SEGI)
			sName.Format("段号标注参数");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
			sName.Format("角钢接头标注参数");
#endif
		//绘制构件参数
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = sName;
		lpInfo->m_strPropHelp = sName;
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = GetPropID("DrawPara");
		if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
		//
		if(m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
		{	//当前绘制构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle for current drawing part";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前绘制构件的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("PART.pPart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			if(m_pCurCmd->m_paraPart.pPart&&m_pCurCmd->m_paraPart.pPart->IsLinePart())
			{
				//暂时不使用以下两属性  wht 11-07-23
				/*
				if(m_pCurCmd->m_paraPart.pPart->GetClassTypeId()==CLS_LINEANGLE)
				{	//角钢基准心距
					//X肢基准心距
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
					lpInfo->m_strPropName.Format("X肢基准心距");
					lpInfo->m_strPropHelp = "X肢基准心距,绘制角钢心线时按此心距绘制。";
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.angle.nXWingDatumAxisG");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
					//Y肢基准心距
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
					lpInfo->m_strPropName.Format("Y肢基准心距");
					lpInfo->m_strPropHelp = "Y肢基准心距，绘制角钢心线时按此心距绘制。";
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.angle.nXWingDatumAxisG");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}*/
				if(m_pCurCmd->m_paraPart.pPart->GetClassTypeId()==CLS_LINEANGLE)
				{	//肢宽拓展方式
					CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pCurCmd->m_paraPart.pPart;
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("leg width expanding way");
					lpInfo->m_cmbItems="0.Expanding On Both Sides|1.Expanding Legs|2.Expanding Ridge Line";
					lpInfo->m_strPropHelp = "when leg width of angle leg is  exaggeratedly drawn, the way to expanding leg width";
#else
					lpInfo->m_strPropName.Format("肢宽拓展方式");
					lpInfo->m_cmbItems="0.两侧拓展|1.拓展肢翼|2.拓展楞线";
					lpInfo->m_strPropHelp = "角钢肢宽夸大绘制时，肢宽的拓展方式";
#endif
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.angle.extendStyle");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("draw bolt line");
					lpInfo->m_cmbItems="Yes|No";
					lpInfo->m_strPropHelp = "Whether to draw angle's center line, some short angles don't need to draw center lines.";
#else
					lpInfo->m_strPropName.Format("绘制心线");
					lpInfo->m_cmbItems="是|否";
					lpInfo->m_strPropHelp = "是否绘制角钢心线，一些短角钢绘图时不需要绘制心线。";
#endif
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.angle.bDrawAxisLine");
					if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
					{
						pSonPropItem->SetReadOnly();
#ifdef AFX_TARG_ENU_ENGLISH
						pSonPropItem->m_lpNodeInfo->m_strPropValue="No";
#else
						pSonPropItem->m_lpNodeInfo->m_strPropValue="否";
#endif
					}
					else if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
				//杆件心线始端延伸长度
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("start extending");
				lpInfo->m_strPropHelp = "starting end's extending value of rod's center line.";
#else
				lpInfo->m_strPropName.Format("始端延伸量");
				lpInfo->m_strPropHelp = "杆件心线始端延伸量。";
#endif
				pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("PART.fPoleAxisLineStartOutSpreadLen");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//杆件心线终端延伸长度
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("end extending");
				lpInfo->m_strPropHelp = "ending end's extending value of rod's center line.";
#else
				lpInfo->m_strPropName.Format("终端延伸量");
				lpInfo->m_strPropHelp = "杆件心线终端延伸量。";
#endif
				pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("PART.fPoleAxisLineEndOutSpreadLen");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
			
			//展开线类型 0.表示无展开线 i-1表示展开线在展开线在展开列表中的值
			if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_STRUVIEW&&m_pComponent->m_pAttachView)
				//&&m_pComponent->m_pAttachView->m_iViewType==1)
			{	//结构图(目前仅定制视图支持展开绘制)
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="drawing plane";
				lpInfo->m_strPropHelp = "parts' drawing plane, can be set up in the modeling environment.";
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("PART.iResidePlane");
				pPropItem->m_lpNodeInfo->m_strPropValue="0.Datum Plane";
#else
				lpInfo->m_strPropName="绘图平面";
				lpInfo->m_strPropHelp = "构件所在的绘图平面，可在建模环境中设置。";
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("PART.iResidePlane");
				pPropItem->m_lpNodeInfo->m_strPropValue="0.基准面";
#endif
				CDrawingAssistant *pAssist=m_pComponent->m_pAttachView->GetDrawingAssistByHandle(m_pCurCmd->m_paraPart.hPart);
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
				pPropItem->SetReadOnly();
				if(pAssist&&pAssist->m_nStyle==0&&pAssist->iNo>0)
				{
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%d.%s",pAssist->iNo,pAssist->name);
					/*
					//构件绘制面 'X'.X截面 'Y'.Y截面,'Z'.Z截面 自动判断
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbItems="X截面|Y截面|Z截面|自动判断|";
					lpInfo->m_strPropName.Format("绘制面");
					lpInfo->m_strPropHelp = "构件绘制面。";
					pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pPropItem->m_idProp=GetPropID("PART.cDrawFlag");
					if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
					*/
				}
			}
			if(m_pCurCmd->m_paraPart.pPart&&m_pCurCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT)
			{	
				if(m_pCurCmd->m_paraPart.para.bolt.front0_side1==1)
				{	//平放布置的螺栓
					lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("drawing way");
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
					lpInfo->m_cmbItems="0.Picture|1.Straight Line";
					lpInfo->m_strPropHelp = "drawing expression way of flat decorated bolts.";
#else
					lpInfo->m_strPropName.Format("绘制方式");
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
					lpInfo->m_cmbItems="0.图符|1.直线";
					lpInfo->m_strPropHelp = "平放布置螺栓的绘制表达方式。";
#endif
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.bolt.bDrawSideBoltLine");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
				else if(m_pCurCmd->m_paraPart.pPart->m_bVirtualPart)
				{
					lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("dimension hole diameter");
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
					lpInfo->m_cmbItems="Yes|No";
					lpInfo->m_strPropHelp = "whether to dimension special hole's processing diameter.";
#else
					lpInfo->m_strPropName.Format("标注孔径");
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
					lpInfo->m_cmbItems="是|否";
					lpInfo->m_strPropHelp = "是否标注特殊孔加工直径。";
#endif
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.bolt.bDimHoleDiameter");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
			}
			//查找对应的构件明细标注命令并显示属性
			CDrawingCommand **ppCmd=hashPartSpecList.GetValue(m_pCurCmd->m_paraPart.hPart);
			if(ppCmd)
			{
				//是否标注构件明细
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="Yes|No";
				lpInfo->m_strPropName.Format("Dimension Parts Detail");
				lpInfo->m_strPropHelp = "whether to dimension parts details.";
#else
				lpInfo->m_cmbItems="是|否";
				lpInfo->m_strPropName.Format("标注构件明细");
				lpInfo->m_strPropHelp = "是否标注构件明细。";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("m_bExecute");
				if(GetPropValueStr(*ppCmd,pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,1);	
				//显示标注构件明细相关属性
				UpdatePartSpecPropertyList(pPropItem,*ppCmd,TRUE);
			}
		}
		//表格绘制参数
		else if(m_pCurCmd->m_eCmdType==CMD_DRAW_TABLE)
		{	
			//选择表格位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("choose position");
			lpInfo->m_strPropHelp = "choose position for chart";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName.Format("选择位置");
			lpInfo->m_strPropHelp = "选择表格位置";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("TABLE.bSelectPos");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//显示定位位置对话框
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("display dialog");
			lpInfo->m_strPropHelp = "display positioning location dialog.";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName.Format("显示对话框");
			lpInfo->m_strPropHelp = "显示定位位置对话框。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("TABLE.bDisplayDlg");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			if(!m_pCurCmd->m_paraBOMTable.bDisplayDlg)
			{	//表格定位位置
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="0.Upper Left|1.Upper Right|2.Lower Left|3.Lower Right|";
				lpInfo->m_strPropName.Format("chart positioning location");
				lpInfo->m_strPropHelp = "chart positioning location";
#else
				lpInfo->m_cmbItems="0.左上|1.右上|2.左下|3.右下|";
				lpInfo->m_strPropName.Format("表格定位位置");
				lpInfo->m_strPropHelp = "表格定位位置";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("TABLE.nLocatePos");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
			//表格位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("chart position");
			lpInfo->m_strPropHelp = "chart position, the position is the relative position in mapping coordinates.";
#else
			lpInfo->m_strPropName.Format("表格位置");
			lpInfo->m_strPropHelp = "表格位置，该位置为绘图坐标系下的相对位置。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("TABLE.DimPos");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//表格位置X坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "chart position X coordinates, which is the relative coordinate in mapping coordinates.";
#else
			lpInfo->m_strPropHelp = "表格位置X坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("TABLE.fDimPosX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//表格位置Y坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "chart position Y coordinates, which is the relative coordinate in mapping coordinates.";
#else
			lpInfo->m_strPropHelp = "表格位置Y坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("TABLE.fDimPosY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
		}
		//构件明细标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
		{	//当前标注构件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "current dimension parts' handle.";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前标注构件的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("PARTSPEC.pObj");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			UpdatePartSpecPropertyList(pGroupItem,m_pCurCmd,FALSE);
			//显示始终端螺栓信息或法兰信息
			CDrawingCommand **ppCmd=hashRodBoltOrFlInfoList.GetValue(m_pCurCmd->m_paraPartSpec.hObj);
			if(ppCmd)
				UpdateRodBoltOrFlInfoPropList(pRoot,*ppCmd,FALSE);
		}
		//标注段号
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SEGI)
		{
			//关联段号
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "segment number";
			lpInfo->m_strPropHelp = "dimension segment number";
#else
			lpInfo->m_strPropName = "段号";
			lpInfo->m_strPropHelp = "标注段号。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSEGI.nSegI");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//选择位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("select position");
			lpInfo->m_strPropHelp = "select position for segment number.";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName.Format("选择位置");
			lpInfo->m_strPropHelp = "选择段号位置";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSEGI.bSelectPos");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//标注位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "dimension position";
			lpInfo->m_strPropHelp = "segment number dimension position, offset value relative to the top right corner of the box.";
#else
			lpInfo->m_strPropName = "标注位置";
			lpInfo->m_strPropHelp = "段号标注位置，相对于图框右上角的偏移值。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSEGI.DimPos");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//标注位置X坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value relative to X direction of the upper right corner  in the box.";
#else
			lpInfo->m_strPropHelp = "相对于图框右上角X方向的偏移值。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSEGI.fDimPosX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//标注位置Y坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value relative to Y direction of the upper right corner  in the box.";
#else
			lpInfo->m_strPropHelp = "相对于图框右上角Y方向的偏移值。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSEGI.fDimPosY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		//相似形标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY)
		{
			//相似形比例
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("similar polygon scale");
			lpInfo->m_strPropHelp = "polygon proportion, consistent with the proportion set in the system by default.";
#else
			lpInfo->m_strPropName.Format("相似形比例");
			lpInfo->m_strPropHelp = "相似形比例，默认与系统设置中的相似形比例一致。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SIMPOLY.fScale");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			if(m_pCurCmd->m_paraSimPoly.nSimPolyCmdType==2)
			{	//相似形拾取点
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("pick point");
				lpInfo->m_strPropHelp = "with similar figures when pick up points, this position for CAD coordinates.";
#else
				lpInfo->m_strPropName.Format("拾取点");
				lpInfo->m_strPropHelp = "标注相似形时的拾取点,该位置为CAD坐标。";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("SIMPOLY.PickPt");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//拾取点X坐标
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "X coordinate of pick point";
#else
				lpInfo->m_strPropHelp = "拾取点X坐标。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("SIMPOLY.fPickPtX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//拾取点Y坐标
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Y coordinate of pick point";
#else
				lpInfo->m_strPropHelp = "拾取点Y坐标。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("SIMPOLY.fPickPtY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
			//相似形位置偏移量
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("offset");
			lpInfo->m_strPropHelp = "offset value of similar polygon dimension position.";
#else
			lpInfo->m_strPropName.Format("偏移量");
			lpInfo->m_strPropHelp = "相似形标注位置偏移量。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SIMPOLY.DimPosOffset");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//X轴方向偏移值
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value of similar polygon dimension position in axis X.";
#else
			lpInfo->m_strPropHelp = "相似形标注位置X轴方向偏移量。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("SIMPOLY.fDimPosOffsetX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//Y轴方向偏移值
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value of similar polygon dimension position in axis Y";
#else
			lpInfo->m_strPropHelp = "相似形标注位置Y轴方向偏移量。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("SIMPOLY.fDimPosOffsetY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);

			if(m_pCurCmd->m_paraSimPoly.nSimPolyCmdType==3)
			{	//相似形轮廓点列表
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("outline vertex");
				lpInfo->m_strPropHelp = "outline vertex";
#else
				lpInfo->m_strPropName.Format("轮廓点");
				lpInfo->m_strPropHelp = "轮廓点";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("SIMPOLY.vertexList");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//
				int i=1;
				SIMPOLY_VERTEX *pVertex=NULL;
				for(pVertex=m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
				pVertex=m_pCurCmd->m_paraSimPoly.simPolyVertexList.GetNext())
				{
					char sKey[50]="";
					//顶点i
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sKey,"vertex %d",i);
#else
					sprintf(sKey,"顶点%d",i);
#endif
					lpInfo->m_strPropName = sKey;
					lpInfo->m_strPropHelp = sKey;
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					sprintf(sKey,"SIMPOLY.vertex%d",i);
					pSonPropItem->m_idProp=GetPropID(sKey);
					if(CDrawMapScriptDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
						pSonPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pSonPropItem->m_idProp);
					//X坐标
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_buttonType=BDT_FILEPATH;
					lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "X coordinate for outline vertex of similar polygons";
#else
					lpInfo->m_strPropHelp = "相似形轮廓点X坐标";
#endif
					CPropTreeItem *pLeafItem = m_propList.InsertItem(pSonPropItem,lpInfo, -1);
					sprintf(sKey,"SIMPOLY.vertex%d.x",i);
					pLeafItem->m_idProp=GetPropID(sKey);
					if(GetPropValueStr(pLeafItem->m_idProp,valueStr)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pLeafItem->m_idProp);
					//Y坐标
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_buttonType=BDT_FILEPATH;
					lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "Y coordinate for outline vertex of similar polygons";
#else
					lpInfo->m_strPropHelp = "相似形轮廓点Y坐标";
#endif
					pLeafItem = m_propList.InsertItem(pSonPropItem,lpInfo, -1);
					sprintf(sKey,"SIMPOLY.vertex%d.y",i);
					pLeafItem->m_idProp=GetPropID(sKey);
					if(GetPropValueStr(pLeafItem->m_idProp,valueStr)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pLeafItem->m_idProp);
					//
					//基准杆件1
					//lpInfo = new CItemInfo();
					//lpInfo->m_controlType=PIT_BUTTON;
					//lpInfo->m_buttonType=BDT_FILEPATH;
					//lpInfo->m_strPropName.Format("基准杆件1");
					//lpInfo->m_strPropHelp = "轮廓点基准杆件1。";
					//CPropTreeItem *pLeafItem = m_propList.InsertItem(pSonPropItem,lpInfo, -1);
					//sprintf(sKey,"SIMPOLY.vertex%d.hPrevPole",i);
					//pLeafItem->m_idProp=GetPropID(sKey);
					//if(GetPropValueStr(pLeafItem->m_idProp,valueStr)>0)
					//	pLeafItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					//基准杆件2
					//lpInfo = new CItemInfo();
					//lpInfo->m_controlType=PIT_BUTTON;
					//lpInfo->m_buttonType=BDT_FILEPATH;
					//lpInfo->m_strPropName.Format("基准杆件2");
					//lpInfo->m_strPropHelp = "轮廓点基准杆件2。";
					//pLeafItem = m_propList.InsertItem(pSonPropItem,lpInfo, -1);
					//sprintf(sKey,"SIMPOLY.vertex%d.hNextPole",i);
					//pLeafItem->m_idProp=GetPropID(sKey);
					//if(GetPropValueStr(pLeafItem->m_idProp,valueStr)>0)
					//	pLeafItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					
					i++;
					if(i>15)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("only 15 outline vertex supported currently!");
#else
						AfxMessageBox("目前仅最多支持15个轮廓点!");
#endif
						break;
					}
				}
			}
		}
		//绘制杆件单线图
		else if(m_pCurCmd->m_eCmdType==CMD_DRAW_LINEPART)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "currently drawn rod's handle.";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前绘制杆件的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("LINEPART.pLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//是否为总图
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("General Map");
			lpInfo->m_strPropHelp = "singular line map in general map";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName.Format("总图");
			lpInfo->m_strPropHelp = "总图中的单线图";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("LINEPART.bSum");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//力学分析模式
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("mechanical analysis mode");
			lpInfo->m_strPropHelp = "mechanical analysis mode";
#else
			lpInfo->m_cmbItems="是|否";
			lpInfo->m_strPropName.Format("力学分析模式");
			lpInfo->m_strPropHelp = "力学分析模式";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("LINEPART.bDimByAnalysisMode");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		//正负头标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ODDMENT)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts ";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前标注构件的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ODDMENT.pLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			//选择位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
			lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.自动搜索|1.指定位置";
			lpInfo->m_strPropName.Format("标注位置类型");
			lpInfo->m_strPropHelp = "标注位置类型";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ODDMENT.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			if(m_pCurCmd->m_paraOddment.iDimPosType==1)
			{	
				if(m_pCurCmd->m_paraOddment.bDimStartOdd)
				{	//始端正负头标注位置
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "start position offset";
					lpInfo->m_strPropHelp = "dimension position offset between the two relative to dimension basis point";
#else
					lpInfo->m_strPropName = "始端位置偏移";
					lpInfo->m_strPropHelp = "始端正负头标注位置相对于标注基点的偏移。";
#endif
					pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pPropItem->m_idProp=GetPropID("ODDMENT.StartOddPosOffset");
					if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
					pPropItem->m_bHideChildren=TRUE;
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//始端正负头标注位置X坐标
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset in X coordinate of starting two ends";
#else
					lpInfo->m_strPropHelp = "始端正负头标注位置X坐标偏移。";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fStartOddPosOffsetX");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
					//始端正负头标注位置Y坐标
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset in X coordinate of starting two ends";
#else
					lpInfo->m_strPropHelp = "始端正负头标注位置Y坐标偏移。";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fStartOddPosOffsetY");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
				if(m_pCurCmd->m_paraOddment.bDimEndOdd)
				{	//终端正负头标注位置
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "end position offset";
					lpInfo->m_strPropHelp = "dimension position offset of ending two ends relative to dimension basis point.";
#else
					lpInfo->m_strPropName = "终端位置偏移";
					lpInfo->m_strPropHelp = "终端正负头标注位置相对于标注基点的偏移。";
#endif
					pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pPropItem->m_idProp=GetPropID("ODDMENT.EndOddPosOffset");
					if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
					pPropItem->m_bHideChildren=TRUE;
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//终端正负头标注位置X坐标
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset in X coordinate of ending two ends";
#else
					lpInfo->m_strPropHelp = "终端正负头标注位置X坐标偏移。";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fEndOddPosOffsetX");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
					//终端正负头标注位置Y坐标
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset of ending two ends";
#else
					lpInfo->m_strPropHelp = "终端正负头标注位置Y坐标偏移。";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fEndOddPosOffsetY");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
			}
		}
		//脚钉间距标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前标注构件的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FOOTNAIL_SPACE.pLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension text space");
			lpInfo->m_strPropHelp = "space between dimension text and dimension starting and ending points";
#else
			lpInfo->m_strPropName.Format("标注文字间距");
			lpInfo->m_strPropHelp = "标注文字与标注起始点间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FOOTNAIL_SPACE.fDimDistance");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension space of the starting and ending points");
			lpInfo->m_strPropHelp = "dimension space value between the starting and ending points and rod's outside margin.";
#else
			lpInfo->m_strPropName.Format("标注起始点间距");
			lpInfo->m_strPropHelp = "标注起始点与脚钉所在杆件外边缘间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FOOTNAIL_SPACE.fSpace");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		//填板间距标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts. ";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前标注构件的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FILLPLATE_SPACE.pLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension text space");
			lpInfo->m_strPropHelp = "dimension text and dimension the space value between starting and ending points.";
#else
			lpInfo->m_strPropName.Format("标注文字间距");
			lpInfo->m_strPropHelp = "标注文字与标注起始点间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FILLPLATE_SPACE.fDimDistance");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension space between the starting and ending points");
			lpInfo->m_strPropHelp = "dimension space value between the starting and ending points and rod's outside margin.";
#else
			lpInfo->m_strPropName.Format("标注起始点间距");
			lpInfo->m_strPropHelp = "标注起始点与脚钉所在杆件外边缘间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FILLPLATE_SPACE.fSpace");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
#ifdef __PART_DESIGN_INC_
		//角钢接头信息标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
		{	
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts.";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前标注构件的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ANGLEJOINT.pObj");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//标注位置类型
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
                        lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.自动搜索|1.指定位置";
			lpInfo->m_strPropName.Format("标注位置类型");
			lpInfo->m_strPropHelp = "标注位置类型";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ANGLEJOINT.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,3);
			//
			if(m_pCurCmd->m_paraAngleJoint.iDimPosType==1)
			{
				//标注位置偏移量
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("dimension position offset");
				lpInfo->m_strPropHelp = "dimension position offset value";
#else
				lpInfo->m_strPropName.Format("标注位置偏移量");
				lpInfo->m_strPropHelp = "标注位置偏移量。";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("ANGLEJOINT.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//标注位置X坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp = "dimension position offset in axis X direction.";
#else
				lpInfo->m_strPropName.Format("X轴方向");
				lpInfo->m_strPropHelp = "标注位置X轴方向偏移量。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("ANGLEJOINT.fDimPosOffsetX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,3);
				//标注位置Y坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp = "dimension position offset in axis Y direction.";
#else
				lpInfo->m_strPropName.Format("Y轴方向");
				lpInfo->m_strPropHelp = "标注位置Y轴方向偏移量。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("ANGLEJOINT.fDimPosOffsetY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,3);
			}
		}
#endif
		//螺栓垫圈标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTPAD)
		{
			//螺栓垫圈关联螺栓
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension joint bolts.";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "当前标注关联螺栓的句柄。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTPAD.pBolt");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//螺栓垫圈规格
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("bolt's washer spec");
			lpInfo->m_strPropHelp = "specifications of  bolt's washer";
#else
			lpInfo->m_strPropName.Format("螺栓垫圈规格");
			lpInfo->m_strPropHelp = "螺栓垫圈规格";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTPAD.sSpec");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//标注位置类型
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
			lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.自动搜索|1.指定位置";
			lpInfo->m_strPropName.Format("标注位置类型");
			lpInfo->m_strPropHelp = "标注位置类型";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTPAD.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			if(m_pCurCmd->m_paraBoltPad.iDimPosType==1)
			{
				//标注位置偏移量
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("dimension position offset");
				lpInfo->m_strPropHelp = "dimension position offset value";
#else
				lpInfo->m_strPropName.Format("标注位置偏移量");
				lpInfo->m_strPropHelp = "标注位置偏移量。";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("BOLTPAD.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//标注位置X坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp = "dimension position offset value in axis X direction.";
#else
				lpInfo->m_strPropName.Format("X轴方向");
				lpInfo->m_strPropHelp = "标注位置X轴方向偏移量。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTPAD.fDimPosOffsetX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//标注位置Y坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp = "dimension position offset value in axis Y direction.";
#else
				lpInfo->m_strPropName.Format("Y轴方向");
				lpInfo->m_strPropHelp = "标注位置Y轴方向偏移量。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTPAD.fDimPosOffsetY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
		}
		//螺栓信息标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
		{	//基准杆件
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("datum rod");
			lpInfo->m_strPropHelp = "bolt information dimension datum rod，to confirm bolt information dimension direction.";
#else
			lpInfo->m_strPropName.Format("基准杆件");
			lpInfo->m_strPropHelp = "螺栓信息标注基准杆件，用来确定螺栓信息标注方向。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTINFO.pDatumRod");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			//标注内容
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension content");
			lpInfo->m_strPropHelp = "bolt combination dimension content";
#else
			lpInfo->m_strPropName.Format("标注内容");
			lpInfo->m_strPropHelp = "螺栓组标注内容";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTINFO.sLabel");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//螺栓列表
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_sButtonName="Add";
			lpInfo->m_strPropName.Format("bolts list");
			lpInfo->m_strPropHelp = "list of bolts";
#else
			lpInfo->m_sButtonName="添加";
			lpInfo->m_strPropName.Format("螺栓列表");
			lpInfo->m_strPropHelp = "螺栓列表";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTINFO.boltSet");
			//
			int i=0;
			CLsGroupDimUnit::BOLTINFO *pBoltInfo=NULL;
			for(pBoltInfo=m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.GetFirstBoltInfo();
				pBoltInfo;pBoltInfo=m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.GetNxetBoltInfo())
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_sButtonName="delete";
				lpInfo->m_strPropName.Format("bolt %d",i);
				lpInfo->m_strPropHelp = "handle of bolt";
#else
				lpInfo->m_sButtonName="删除";
				lpInfo->m_strPropName.Format("螺栓%d",i);
				lpInfo->m_strPropHelp = "螺栓句柄";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=(long)pBoltInfo;	//使用螺栓信息指针作为id
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				i++;
			}
			//标注位置类型
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
			lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.自动搜索|1.指定位置";
			lpInfo->m_strPropName.Format("标注位置类型");
			lpInfo->m_strPropHelp = "标注位置类型";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTINFO.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			if(m_pCurCmd->m_paraBoltGroupInfo.iDimPosType==1)
			{
				//标注位置偏移量
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("dimension position offset");
				lpInfo->m_strPropHelp = "dimension position offset value";
#else
				lpInfo->m_strPropName.Format("标注位置偏移量");
				lpInfo->m_strPropHelp = "标注位置偏移量。";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("BOLTINFO.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//标注位置X坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp = "dimension position offset value in axis X.";
#else
				lpInfo->m_strPropName.Format("X轴方向");
				lpInfo->m_strPropHelp = "标注位置X轴方向偏移量。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTINFO.fDimPosOffsetX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//标注位置Y坐标偏移值
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp = "dimension position offset value in axis Y";
#else
				lpInfo->m_strPropName.Format("Y轴方向");
				lpInfo->m_strPropHelp = "标注位置Y轴方向偏移量。";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTINFO.fDimPosOffsetY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
		}
		//标注角钢特殊准据
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		{
			//需要标注特殊准据的角钢
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "angles to be marked specific criterion.";
#else
			lpInfo->m_strPropName.Format("%s句柄",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "需要标注特殊准据的角钢。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_JGZHUN.pLineAngle");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension text space");
			lpInfo->m_strPropHelp = "dimension text and dimension the space value between starting and ending points.";
#else
			lpInfo->m_strPropName.Format("标注文字间距");
			lpInfo->m_strPropHelp = "标注文字与标注起始点间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_JGZHUN.fDimDistance");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension the space between starting and ending points");
			lpInfo->m_strPropHelp = "dimension the distance between the starting and ending points and starting dimension position.";
#else
			lpInfo->m_strPropName.Format("标注起始点间距");
			lpInfo->m_strPropHelp = "标注起始点与始端标注位置之间的距离。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_JGZHUN.fSpace");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Internal|External";
			lpInfo->m_strPropName.Format("dimension direction");
			lpInfo->m_strPropHelp = "dimension direction";
#else
			lpInfo->m_cmbItems="内侧|外侧";
			lpInfo->m_strPropName.Format("标注方向");
			lpInfo->m_strPropHelp = "标注方向。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_JGZHUN.bInside");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
		{
			//螺栓特殊间距
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("starting bolt handle");
			lpInfo->m_strPropHelp.Format("bolts specific space, bolt's handle on starting end");
#else
			lpInfo->m_strPropName.Format("始端螺栓句柄");
			lpInfo->m_strPropHelp.Format("螺栓特殊间距,始端螺栓句柄。");
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_LSSPACE.hStartLs");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//pPropItem->SetReadOnly();
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("ending bolt handle");
			lpInfo->m_strPropHelp.Format("bolts specific space, bolt's handle on ending end");
#else
			lpInfo->m_strPropName.Format("终端螺栓句柄");
			lpInfo->m_strPropHelp.Format("螺栓特殊间距,终端螺栓句柄。");
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_LSSPACE.hEndLs");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//pPropItem->SetReadOnly();
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension text space");
			lpInfo->m_strPropHelp = "dimension text and dimension the space between starting points and ending points";
#else
			lpInfo->m_strPropName.Format("标注文字间距");
			lpInfo->m_strPropHelp = "标注文字与标注起始点间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_LSSPACE.fDimDistance");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension the space between starting and ending points");
			lpInfo->m_strPropHelp = "dimension the distance between the starting bolt and starting and ending points.";
#else
			lpInfo->m_strPropName.Format("标注起始点间距");
			lpInfo->m_strPropHelp = "标注起始点与始端螺栓之间的距离。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_LSSPACE.fSpace");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Internal|External";
			lpInfo->m_strPropName.Format("dimension direction");
			lpInfo->m_strPropHelp = "dimension direction";
#else
			lpInfo->m_cmbItems="内侧|外侧";
			lpInfo->m_strPropName.Format("标注方向");
			lpInfo->m_strPropHelp = "标注方向。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_LSSPACE.bInside");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			if (m_pCurCmd->m_paraDimLsSpace.spaceDim.nLsSpaceNum>1)
			{
				//
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("bolt space number");
				lpInfo->m_strPropHelp = "bolt space number contained";
#else
				lpInfo->m_strPropName.Format("螺栓间距数");
				lpInfo->m_strPropHelp = "包含的螺栓间距数。";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("DIM_LSSPACE.nLsSpaceNum");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
				pPropItem->SetReadOnly();
				//
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="Yes|No";
				lpInfo->m_strPropName.Format("combine dimension bolt space");
				lpInfo->m_strPropHelp = "combine dimension bolt space,dimension in the mode of single space × number";
#else
				lpInfo->m_cmbItems="是|否";
				lpInfo->m_strPropName.Format("合并标注螺栓间距");
				lpInfo->m_strPropHelp = "合并标注特殊螺栓间距，标注成单个间距×数量的形式。";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("DIM_LSSPACE.bMergeDimLsSpace");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			}
		}
		//模型尺寸标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
		{
			//模型尺寸标注类型
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "type";
			lpInfo->m_strPropHelp = "model size dimension type";
			lpInfo->m_cmbItems="0.Inter Node Offset|1.Vertical Cross Arm Space|2.Piecewise Interval|3.Tower Height|4.Changing Slope Mouth|5.Cross Arm Horizontal Space|";
#else
			lpInfo->m_strPropName = "类型";
			lpInfo->m_strPropHelp = "模型尺寸标注类型。";
			lpInfo->m_cmbItems="0.节间垂距|1.横担垂直间距|2.分段间距|3.塔高|4.变坡开口|5.横担水平间距|";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMMODELSIZE.iDimType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension text space");
			lpInfo->m_strPropHelp = "dimension text and dimension space value between starting and ending points";
#else
			lpInfo->m_strPropName.Format("标注文字间距");
			lpInfo->m_strPropHelp = "标注文字与标注起始点间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMMODELSIZE.fDimDistance");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension the distance between starting and ending points ");
			lpInfo->m_strPropHelp = "dimension the space value between tower body and starting and ending points";
#else
			lpInfo->m_strPropName.Format("标注起始点间距");
			lpInfo->m_strPropHelp = "标注起始点与塔身间距值";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMMODELSIZE.fSpace");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("nods list");
			lpInfo->m_strPropHelp = "list of nods";
#else
			lpInfo->m_strPropName.Format("节点列表");
			lpInfo->m_strPropHelp = "节点列表";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMMODELSIZE.nodeSet");
			//
			CDrawingCommand::DIMNODE* pDimNode=NULL;
			for(pDimNode=m_pCurCmd->m_paraDimModelSize.listDimNodes.GetFirst();pDimNode;pDimNode=m_pCurCmd->m_paraDimModelSize.listDimNodes.GetNext())
			{
				CLDSNode *pNode=pDimNode->pNode;
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_sButtonName="delete";
				lpInfo->m_strPropName.Format("node %d",m_pCurCmd->m_paraDimModelSize.nodeSet.GetCurrentIndex());
				lpInfo->m_strPropHelp = "handle of nod";
#else
				lpInfo->m_sButtonName="删除";
				lpInfo->m_strPropName.Format("节点%d",m_pCurCmd->m_paraDimModelSize.listDimNodes.GetCurrentIndex());
				lpInfo->m_strPropHelp = "节点句柄";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				CXhChar50 sPropStr("DIMMODELSIZE.Node%d",m_pCurCmd->m_paraDimModelSize.listDimNodes.GetCurrentIndex());
				pSonPropItem->m_idProp=GetPropID(sPropStr);
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
		}
		//图面尺寸标注参数
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MAPSIZE)
		{
			//尺寸比例
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("size dimension scale");
			lpInfo->m_strPropHelp = "size dimension  scale,consistent with the proportion set in the system by default.";
#else
			lpInfo->m_strPropName.Format("尺寸标注比例");
			lpInfo->m_strPropHelp = "尺寸标注比例，默认与系统设置中的结构图绘制比例一致。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.fScale");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//标注方向 0.X轴方向 1.Y轴方向 2.延伸方向
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Axis X|1.Axis Y|2.Extend Direction|";
			lpInfo->m_strPropName.Format("size dimension direction");
			lpInfo->m_strPropHelp = "size dimension direction";
#else
			lpInfo->m_cmbItems="0.X轴方向|1.Y轴方向|2.延伸方向|";
			lpInfo->m_strPropName.Format("尺寸标注方向");
			lpInfo->m_strPropHelp = "尺寸标注方向";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.iDimDirect");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//标注起始点
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension starting and ending points");
			lpInfo->m_strPropHelp = "dimension starting and ending points";
#else
			lpInfo->m_strPropName.Format("标注起始点");
			lpInfo->m_strPropHelp = "标注终止点";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.DimStart");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//标注起始点X坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension X coordinates for the starting point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "标注起始点X坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimStartX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//标注起始点Y坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension Y coordinates for the starting  point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "标注起始点Y坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimStartY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//标注终止点
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension ending point");
			lpInfo->m_strPropHelp = "dimension ending point";
#else
			lpInfo->m_strPropName.Format("标注终止点");
			lpInfo->m_strPropHelp = "标注终止点";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.DimEnd");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//标注终止点X坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension X coordinates for the ending point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "标注终止点X坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimEndX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//标注终止点Y坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension Y coordinates for the ending point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "标注终止点Y坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimEndY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//标注位置
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension position");
			lpInfo->m_strPropHelp = "dimension position";
#else
			lpInfo->m_strPropName.Format("标注位置");
			lpInfo->m_strPropHelp = "标注位置";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.DimPos");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//标注位置X坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension X coordinates for the point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "标注位置X坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimPosX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//标注位置Y坐标
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension Y coordinates for the point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "标注位置Y坐标，该坐标为绘图坐标系下的相对坐标。";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimPosY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
		}
	}
	else if(m_pSpreadLine&&IsSpreadLinePropGroup(iCurSel))
	{	//展开线属性
		if(m_pSpreadLine->m_iDefType==0)
		{	//基准杆件 基准杆件句柄为0时表示为自定义
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_FILEPATH;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("datum angle");
			lpInfo->m_strPropHelp = "spread line positioning datum angle";
#else
			lpInfo->m_strPropName.Format("基准角钢");
			lpInfo->m_strPropHelp = "展开线定位基准角钢。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_hDatumPole");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			//基准杆件为角钢时启用该属性
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg|";
			lpInfo->m_strPropName.Format("positioning leg");
			lpInfo->m_strPropHelp = "spread line datum angle positioning leg.";
#else
			lpInfo->m_cmbItems="0.X肢|1.Y肢|";
			lpInfo->m_strPropName.Format("定位肢");
			lpInfo->m_strPropHelp = "展开线基准角钢定位肢。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_iCurWing");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			//偏移距离
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("offset");
			lpInfo->m_strPropHelp = "distance between spreading line and angle's ridge";
#else
			lpInfo->m_strPropName.Format("偏移距离");
			lpInfo->m_strPropHelp = "展开线与角钢楞线之间的距离。";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_fOffsetDist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		}
		//偏移线起点坐标
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "starting coordinates";
		lpInfo->m_strPropHelp = "spread line starting point coordinates";
#else
		lpInfo->m_strPropName = "起点坐标";
		lpInfo->m_strPropHelp = "展开线起点坐标";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("SPREAD_LINE.startPt");
		pPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "X coordinates, which is the coordinates for the whole tower coordinate system.";
#else
		lpInfo->m_strPropHelp = "X坐标,该坐标为全塔坐标系下的坐标。";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp=GetPropID("SPREAD_LINE.startPt.x");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "Y coordinates, which is the coordinates for the whole tower coordinate system.";
#else
		lpInfo->m_strPropHelp = "Y坐标,该坐标为全塔坐标系下的坐标。";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp=GetPropID("SPREAD_LINE.startPt.y");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName.Format("Z");
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "Z coordinates, which is the coordinates for the whole tower coordinate system.";
#else
		lpInfo->m_strPropHelp = "Z坐标,该坐标为全塔坐标系下的坐标。";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp=GetPropID("SPREAD_LINE.startPt.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		//偏移线终点坐标
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "ending coordinates";
		lpInfo->m_strPropHelp = "spread line ending coordinates";
#else
		lpInfo->m_strPropName = "终点坐标";
		lpInfo->m_strPropHelp = "展开线终点坐标";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = GetPropID("SPREAD_LINE.endPt");
		pPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "X coordinates, which is the coordinates for the whole tower coordinate system.";
#else
		lpInfo->m_strPropHelp = "X坐标,该坐标为全塔坐标系下的坐标。";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp=GetPropID("SPREAD_LINE.endPt.x");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "Y coordinates, which is the coordinates for the whole tower coordinate system.";
#else
		lpInfo->m_strPropHelp = "Y坐标,该坐标为全塔坐标系下的坐标。";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp=GetPropID("SPREAD_LINE.endPt.y");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName.Format("Z");
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp = "Z coordinates, which is the coordinates for the whole tower coordinate system.";
#else
		lpInfo->m_strPropHelp = "Z坐标,该坐标为全塔坐标系下的坐标。";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp=GetPropID("SPREAD_LINE.endPt.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
	}
	m_propList.Redraw();
}

BOOL CDrawMapScriptDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pNMHdr = (NMHDR*)lParam; 
	if(m_listCmdSuperCtrl.GetSafeHwnd()&&pNMHdr->hwndFrom==m_listCmdSuperCtrl.GetHeaderCtrl()-> GetSafeHwnd()) 
	{
		if(pNMHdr->code==HDN_ENDTRACK) 
		{	//结束拖动列分隔符 
			HD_NOTIFY* phdn=(HD_NOTIFY*)pNMHdr; 
			if(phdn->iItem==0)	//锁定第一列宽度为25
				phdn-> pitem->cxy=25;
			else if(phdn->iItem==3)	//锁定第四列宽度为45
				phdn-> pitem->cxy=45;
		} 
		else if(pNMHdr->code==HDN_DIVIDERDBLCLICK) 
		{	//结束双击列分隔符 
			HD_NOTIFY* phdn= (HD_NOTIFY*)pNMHdr; 
			if(phdn->iItem==0)	//锁定第一列宽度为25
				m_listCmdSuperCtrl.SetColumnWidth(0,25);
			else if(phdn->iItem==3)	//锁定第四列宽度为45
				m_listCmdSuperCtrl.SetColumnWidth(3,45);
		}	
	} 
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CDrawMapScriptDlg::OnClose() 
{
	CDialog::OnOK();
	if(m_pComponent)	//初始化命令feature=-1,重启对话框保持顺序时用到 wht 11-07-23
		m_pComponent->InitCmdFeature(-1);	
	//CDialog::OnCancel();
}

//得到指定命令指定ID的属性字符串
int CDrawMapScriptDlg::GetPropValueStr(CDrawingCommand *pCmd,long id,char *valueStr)
{
	long handle=0;
	CXhChar100 sText;
	//
	int iCurGroup=m_tabCmdType.GetCurSel();
	if(pCmd&&!IsSpreadLinePropGroup(iCurGroup))
	{
		long idNodeIndexStart=-1,idNodeIndexEnd=-1,idBoltIndexStart=-1,idBoltIndexEnd=-1;
		idNodeIndexStart=CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.Node0");
		idNodeIndexEnd=CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.Node%d",PROPID_SETSIZE-1);
		idBoltIndexStart=CDrawMapScriptDlg::GetPropID("BOLTINFO.pBolt0");
		idBoltIndexEnd=CDrawMapScriptDlg::GetPropID("BOLTINFO.pBolt%d",PROPID_SETSIZE-1);

		if(GetPropID("m_eCmdType")==id)
		{	//命令类型
			if(pCmd->m_eCmdType==CMD_DRAW_PART)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Draw Parts");
			else if(pCmd->m_eCmdType==CMD_DRAW_TABLE)
				sText.Printf("Draw Table");
			else if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)
				sText.Printf("Dimension Parts Detail");
			else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)
				sText.Printf("Dimension Similar Polygon");
			else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
				sText.Printf("Dimension Oddment");
			else if(pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
				sText.Printf("Dimension Foot Nail Space");
			else if(pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
				sText.Printf("Dimension Filler Plate Space");
			else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
				sText.Printf("Dimension Blot Washer");
			else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
				sText.Printf("Dimension Angle's Special G");
			else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
				sText.Printf("Dimension Bolts Specific Space");
			else if(pCmd->m_eCmdType==CMD_DIM_MAPSIZE)
				sText.Printf("Dimension Map Size");
			else if(pCmd->m_eCmdType==CMD_DIM_MODELSIZE)
				sText.Printf("Dimension Model Size");
			else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
				sText.Printf("Dimension Bolts Information");
			else if(pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
				sText.Printf("Dimension Angle's Joints");
			else 
				sText.Printf("Other Commands");
#else
				sText.Printf("绘制构件");
			else if(pCmd->m_eCmdType==CMD_DRAW_TABLE)
				sText.Printf("绘制表格");
			else if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)
				sText.Printf("标注构件明细");
			else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)
				sText.Printf("标注相似形");
			else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
				sText.Printf("标注正负头");
			else if(pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
				sText.Printf("标注脚钉间距");
			else if(pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
				sText.Printf("标注填板间距");
			else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
				sText.Printf("标注螺栓垫圈");
			else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
				sText.Printf("标注角钢特殊准据");
			else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
				sText.Printf("标注螺栓特殊间距");
			else if(pCmd->m_eCmdType==CMD_DIM_MAPSIZE)
				sText.Printf("标注图面尺寸");
			else if(pCmd->m_eCmdType==CMD_DIM_MODELSIZE)
				sText.Printf("标注模型尺寸");
			else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
				sText.Printf("标注螺栓信息");
			else if(pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
				sText.Printf("标注角钢接头");
			else 
				sText.Printf("其他命令");
#endif
		}
		else if(GetPropID("m_stateCmd")==id)		//命令状态
		{
			if(pCmd->m_stateCmd==0x01)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("current position");
			else if(pCmd->m_stateCmd==0x02)
				sText.Printf("breakpoint");
			else if(pCmd->m_stateCmd==0x03)
				sText.Printf("current position&&breakpoint");
			else if(pCmd->m_stateCmd==0x04)
				sText.Printf("label");
			else if(pCmd->m_stateCmd==0x05)
				sText.Printf("current position&&label");
			else if(pCmd->m_stateCmd==0x06)
				sText.Printf("breakpoint&&label");
			else if(pCmd->m_stateCmd==0x07)
				sText.Printf("current position&&label&&breakpoint");
			else //if(pCmd->m_stateCmd==0x0)
				sText.Printf("free");
#else
				sText.Printf("当前位置");
			else if(pCmd->m_stateCmd==0x02)
				sText.Printf("断点");
			else if(pCmd->m_stateCmd==0x03)
				sText.Printf("当前位置&&断点");
			else if(pCmd->m_stateCmd==0x04)
				sText.Printf("标签");
			else if(pCmd->m_stateCmd==0x05)
				sText.Printf("当前位置&&标签");
			else if(pCmd->m_stateCmd==0x06)
				sText.Printf("断点&&标签");
			else if(pCmd->m_stateCmd==0x07)
				sText.Printf("当前位置&&标签&&断点");
			else //if(pCmd->m_stateCmd==0x0)
				sText.Printf("空闲");
#endif
		}
		else if(GetPropID("m_bExecute")==id)	//是否执行该命令
		{
			if(pCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		//构件绘制参数
		else if(GetPropID("PART.pPart")==id)	//脚本记录关联对象
		{
			if(pCmd->m_paraPart.pPart)
				sText.Printf("0X%X",pCmd->m_paraPart.pPart->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("PART.angle.bDrawAxisLine")==id)	//杆件心线始端延伸长度
		{
			if(pCmd->m_paraPart.para.angle.bDrawAxisLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
		else if(GetPropID("PART.angle.extendStyle")==id)	//杆件心线始端延伸长度
		{
			if(pCmd->m_paraPart.para.angle.extendStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("1.Expanding Legs");
			else if(pCmd->m_paraPart.para.angle.extendStyle==2)
				sText.Copy("2.Expanding Ridge Line");
			else //if(pCmd->m_paraPart.para.angle.extendStyle==0)
				sText.Copy("0.Expanding On Both Sides");
#else
				sText.Copy("1.拓展肢翼");
			else if(pCmd->m_paraPart.para.angle.extendStyle==2)
				sText.Copy("2.拓展楞线");
			else //if(pCmd->m_paraPart.para.angle.extendStyle==0)
				sText.Copy("0.两侧拓展");
#endif
		}
		else if(GetPropID("PART.fPoleAxisLineStartOutSpreadLen")==id)	//杆件心线始端延伸长度
		{
			sText.Printf("%lf",pCmd->m_paraPart.fPoleAxisLineStartOutSpreadLen);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PART.fPoleAxisLineEndOutSpreadLen")==id)		//杆件心线终端延伸长度
		{
			sText.Printf("%lf",pCmd->m_paraPart.fPoleAxisLineEndOutSpreadLen);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PART.bolt.bDrawSideBoltLine")==id)		//杆件心线终端延伸长度
		{
			if(pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT&&
				pCmd->m_paraPart.para.bolt.front0_side1==1&&pCmd->m_paraPart.para.bolt.bDrawSideBoltLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("1.Straight Line");
			else
				sText.Copy("0.Picture");
#else
				sText.Copy("1.直线");
			else
				sText.Copy("0.图符");
#endif
		}
		else if(GetPropID("PART.bolt.bDimHoleDiameter")==id)		//杆件心线终端延伸长度
		{
			if(pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->m_bVirtualPart&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT&&
				pCmd->m_paraPart.para.bolt.front0_side1==0&&pCmd->m_paraPart.para.bolt.bDimHoleDiameter)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("是");
			else
				sText.Copy("否");
#endif
		}
		else if(GetPropID("PART.cDrawFlag")==id)	//构件绘制面 'X'.X截面 'Y'.Y截面,'Z'.Z截面 自动判断
		{
			if(pCmd->m_paraPart.cDrawFlag=='X')
#ifdef AFX_TARG_ENU_ENGLISH
    
				sText.Copy("X cross-section");
			else if(pCmd->m_paraPart.cDrawFlag=='Y')
				sText.Copy("Y cross-section");
			else if(pCmd->m_paraPart.cDrawFlag=='Z')
				sText.Copy("Z cross-section");
			else
				sText.Copy("default");
#else
				sText.Copy("X截面");
			else if(pCmd->m_paraPart.cDrawFlag=='Y')
				sText.Copy("Y截面");
			else if(pCmd->m_paraPart.cDrawFlag=='Z')
				sText.Copy("Z截面");
			else
				sText.Copy("自动判断");
#endif
		}
		else if(GetPropID("PART.angle.nXWingDatumAxisG")==id)	//X肢基准心距
			sText.Printf("%d",pCmd->m_paraPart.para.angle.nXWingDatumAxisG);
		else if(GetPropID("PART.angle.nYWingDatumAxisG")==id)	//Y肢基准心距
			sText.Printf("%d",pCmd->m_paraPart.para.angle.nYWingDatumAxisG);
		//绘制杆件单线
		else if(GetPropID("LINEPART.pLinePart")==id)			//杆件句柄
		{
			if(pCmd->m_paraLinePart.pLinePart)
				sText.Printf("0X%X",pCmd->m_paraLinePart.pLinePart->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("LINEPART.bSum")==id)
		{
			if(pCmd->m_paraLinePart.bSumMap)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		else if(GetPropID("LINEPART.bDimByAnalysisMode")==id)
		{
			if(pCmd->m_paraLinePart.bDimByAnalysisMode)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		//表格绘制参数
		else if(GetPropID("TABLE.nLocatePos")==id)			//0.左上 1.右上 2.左下 3.右下
		{
			if(pCmd->m_paraBOMTable.nLocatePos==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Upper Right");
			else if(pCmd->m_paraBOMTable.nLocatePos==2)
				sText.Printf("2.Lower Left");
			else if(pCmd->m_paraBOMTable.nLocatePos==3)
				sText.Printf("3.Lower Right");
			else //if(pCmd->m_paraBOMTable.nLocatePos==0)
				sText.Printf("0.Upper Left");
#else
				sText.Printf("1.右上");
			else if(pCmd->m_paraBOMTable.nLocatePos==2)
				sText.Printf("2.左下");
			else if(pCmd->m_paraBOMTable.nLocatePos==3)
				sText.Printf("3.右下");
			else //if(pCmd->m_paraBOMTable.nLocatePos==0)
				sText.Printf("0.左上");
#endif
		}
		else if(GetPropID("TABLE.bSelectPos")==id)			//选择表格位置
		{
			if(pCmd->m_paraBOMTable.bSelectPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		else if(GetPropID("TABLE.bDisplayDlg")==id)			//显示对话框
		{
			if(pCmd->m_paraBOMTable.bDisplayDlg)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		else if(GetPropID("TABLE.fDimPosX")==id)			//表格位置X坐标		
		{
			sText.Printf("%lf",pCmd->m_paraBOMTable.fDimPosX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("TABLE.fDimPosY")==id)			//表格位置Y坐标
		{
			sText.Printf("%lf",pCmd->m_paraBOMTable.fDimPosY);
			SimplifiedNumString(sText);
		}
		//构件明细标注参数
		else if(GetPropID("PARTSPEC.pObj")==id)			//当前标注构件
		{
			if(pCmd->m_paraPartSpec.pObj)
				sText.Printf("0X%X",pCmd->m_paraPartSpec.pObj->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("PARTSPEC.fBasePosOffsetX")==id)	//标注位置X坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fBasePosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fBasePosOffsetY")==id)	//标注位置Y坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fBasePosOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fDimPosOffsetX")==id)	//标注位置X坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fDimPosOffsetY")==id)	//标注位置Y坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fGuigeOffsetX")==id)	//规格标注位置X坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fGuigeOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fGuigeOffsetY")==id)	//规格标注位置Y坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fGuigeOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.iDimPosType")==id)		//0.自动搜索位置 1.指定位置(保存指定位置与原标注位置的偏移值)  2.选择位置
		{
			if(pCmd->m_paraPartSpec.iDimPosType==2)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("2.Choose");
			else if(pCmd->m_paraPartSpec.iDimPosType==1)
				sText.Printf("1.Specify");
			else //if(pCmd->m_paraPartSpec.iDimPosType==0)
				sText.Printf("0.Search Auto");
#else
				sText.Printf("2.选择位置");
			else if(pCmd->m_paraPartSpec.iDimPosType==1)
				sText.Printf("1.指定位置");
			else //if(pCmd->m_paraPartSpec.iDimPosType==0)
				sText.Printf("0.自动搜索");
#endif
		}
		else if(GetPropID("PARTSPEC.bDrawRefLine")==id)	//是否绘制引出线
		{
			if(pCmd->m_paraPartSpec.bDrawRefLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		else if(GetPropID("PARTSPEC.fPoleDimPosCoef")==id)	//杆件标注位置系数 //LTMA 0.3  TAS 0.5
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fPoleDimPosCoef);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.iMirMsg")==id)			//0:X轴对称；1:Y轴对称;其余不对称处理
		{
			if(pCmd->m_paraPartSpec.iMirMsg==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.X Axis Mirror");
			else if(pCmd->m_paraPartSpec.iMirMsg==1)
				sText.Printf("1.Y Axis Mirror");
			else 
				sText.Printf("2.No Mirror");
#else
				sText.Printf("0.X轴对称");
			else if(pCmd->m_paraPartSpec.iMirMsg==1)
				sText.Printf("1.Y轴对称");
			else 
				sText.Printf("2.无对称");
#endif
		}
		else if(GetPropID("PARTSPEC.bDimPoleSpec")==id)		//仅标注杆件编号
		{
			if(!pCmd->m_paraPartSpec.bOnlyDimPartNo)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		//标注段号
		else if(GetPropID("DIMSEGI.bSelectPos")==id)
		{
			if(pCmd->m_paraDimSegI.bSelectPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("是");
			else 
				sText.Printf("否");
#endif
		}
		else if(GetPropID("DIMSEGI.nSegI")==id)		//关联段号
			sText.Printf("%s",(char*)pCmd->m_paraDimSegI.nSegI.ToString());
		else if(GetPropID("DIMSEGI.fDimPosX")==id)	//标注位置X坐标
		{
			sText.Printf("%lf",pCmd->m_paraDimSegI.fDimPosX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSEGI.fDimPosY")==id)	//标注位置Y坐标
		{
			sText.Printf("%lf",pCmd->m_paraDimSegI.fDimPosY);
			SimplifiedNumString(sText);
		}
		//正负头标注参数
		else if(GetPropID("ODDMENT.pLinePart")==id)			//正负头关联杆件
		{
			if(pCmd->m_paraOddment.pLinePart)
				sText.Printf("0X%X",pCmd->m_paraOddment.pLinePart->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("ODDMENT.iDimPosType")==id)
		{
			if(pCmd->m_paraOddment.iDimPosType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Search Auto");
			else 
				sText.Copy("1.Specify");
#else
				sText.Copy("0.自动搜索");
			else 
				sText.Copy("1.指定位置");
#endif
		}
		else if(GetPropID("ODDMENT.StartOddPos")==id)			//正负头关联杆件
		{
			if(pCmd->m_paraOddment.pLinePart)
			{
				double startOdd=pCmd->m_paraOddment.pLinePart->startOdd();
				if(pCmd->m_paraOddment.pLinePart->IsAngle())
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pCmd->m_paraOddment.pLinePart;
					//计算角钢当前绘制肢
					int x_wing0_y_wing1=2;
					CDrawLineAngleTagInfo drawInfo;
					CalPartDrawType(pLineAngle,pCmd->m_pDrawingCom->GetDCS(),pCmd->m_pDrawingCom->m_pAttachView,&drawInfo);
					if(toupper(drawInfo.m_cDrawStyle)=='X')
						x_wing0_y_wing1=0;
					else if(toupper(drawInfo.m_cDrawStyle)=='Y')
						x_wing0_y_wing1=1;
					if(x_wing0_y_wing1==0||x_wing0_y_wing1==1)
						startOdd+=pLineAngle->GetStartExtraOdd(x_wing0_y_wing1);
				}
				if(startOdd>0)
					sText.Printf("+%d",ftoi(startOdd));
				else
					sText.Printf("%d",ftoi(startOdd));
			}
			else 
				sText.Copy("");
		}
		else if(GetPropID("ODDMENT.EndOddPos")==id)			//正负头关联杆件
		{
			if(pCmd->m_paraOddment.pLinePart)
			{
				double endOdd=pCmd->m_paraOddment.pLinePart->endOdd();
				if(pCmd->m_paraOddment.pLinePart->IsAngle())
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pCmd->m_paraOddment.pLinePart;
					//计算角钢当前绘制肢
					int x_wing0_y_wing1=2;
					CDrawLineAngleTagInfo drawInfo;
					CalPartDrawType(pLineAngle,pCmd->m_pDrawingCom->GetDCS(),pCmd->m_pDrawingCom->m_pAttachView,&drawInfo);
					if(toupper(drawInfo.m_cDrawStyle)=='X')
						x_wing0_y_wing1=0;
					else if(toupper(drawInfo.m_cDrawStyle)=='Y')
						x_wing0_y_wing1=1;
					if(x_wing0_y_wing1==0||x_wing0_y_wing1==1)
						endOdd+=pLineAngle->GetEndExtraOdd(x_wing0_y_wing1);
				}
				if(endOdd>0)
					sText.Printf("+%d",ftoi(endOdd));
				else
					sText.Printf("%d",ftoi(endOdd));
			}
			else 
				sText.Copy("");
		}
		else if(GetPropID("ODDMENT.fStartOddPosOffsetX")==id)
		{
			sText.Printf("%lf",pCmd->m_paraOddment.fStartOddPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ODDMENT.fStartOddPosOffsetY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraOddment.fStartOddPosOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ODDMENT.fEndOddPosOffsetX")==id)
		{
			sText.Printf("%lf",pCmd->m_paraOddment.fEndOddPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ODDMENT.fEndOddPosOffsetY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraOddment.fEndOddPosOffsetY);
			SimplifiedNumString(sText);
		}
		//杆件螺栓信息或法兰信息标注参数
		else if(GetPropID("ROD_BOLTORFL_INFO.pLinePart")==id)			//正负头关联杆件
		{
			if(pCmd->m_paraBoltOrFlInfo.pLinePart)
				sText.Printf("0X%X",pCmd->m_paraBoltOrFlInfo.pLinePart->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.iDimPosType")==id)
		{
			if(pCmd->m_paraBoltOrFlInfo.iDimPosType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Search Auto");
			else 
				sText.Copy("1.Specify");
#else
				sText.Copy("0.自动搜索");
			else 
				sText.Copy("1.指定位置");
#endif
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset")==id)			//正负头关联杆件
		{
			if(pCmd->m_paraBoltOrFlInfo.pLinePart)
			{
				if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
				{
					if(pCmd->m_paraBoltOrFlInfo.pLinePart->connectStart.m_iConnectType==0)
						sText.Copy(pCmd->m_paraBoltOrFlInfo.pLinePart->connectStart.flNo);
					else
						sText.Copy("");
				}
				else 
				{
					if(pCmd->m_paraBoltOrFlInfo.pLinePart->connectStart.wnSummBoltN>0)
						sText.Printf("M%dx%d",pCmd->m_paraBoltOrFlInfo.pLinePart->connectStart.d,pCmd->m_paraBoltOrFlInfo.pLinePart->connectStart.wnSummBoltN);
					else
						sText.Copy("");
				}
			}
			else 
				sText.Copy("");
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset")==id)			//正负头关联杆件
		{
			if(pCmd->m_paraBoltOrFlInfo.pLinePart)
			{
				if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
				{
					if(pCmd->m_paraBoltOrFlInfo.pLinePart->connectEnd.m_iConnectType==0)
						sText.Copy(pCmd->m_paraBoltOrFlInfo.pLinePart->connectEnd.flNo);
					else
						sText.Copy("");
				}
				else 
				{
					if(pCmd->m_paraBoltOrFlInfo.pLinePart->connectEnd.wnSummBoltN>0)
						sText.Printf("M%dx%d",pCmd->m_paraBoltOrFlInfo.pLinePart->connectEnd.d,pCmd->m_paraBoltOrFlInfo.pLinePart->connectEnd.wnSummBoltN);
					else
						sText.Copy("");
				}
			}
			else 
				sText.Copy("");
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX")==id)
		{
			sText.Printf("%lf",pCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraBoltOrFlInfo.fStartOddPosOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX")==id)
		{
			sText.Printf("%lf",pCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraBoltOrFlInfo.fEndOddPosOffsetY);
			SimplifiedNumString(sText);
		}
		//脚钉间距标注参数
		else if(GetPropID("FOOTNAIL_SPACE.pLinePart")==id)
		{
			if(pCmd->m_paraDimFootNailSpace.pLinePart)
				sText.Printf("0X%X",pCmd->m_paraDimFootNailSpace.pLinePart->handle);
			else 
				sText.Printf("0X0");
		}
		//填板间距标注参数
		else if(GetPropID("FILLPLATE_SPACE.pLinePart")==id)
		{
			if(pCmd->m_paraDimFillPlateSpace.pLinePart)
				sText.Printf("0X%X",pCmd->m_paraDimFillPlateSpace.pLinePart->handle);
			else 
				sText.Printf("0X0");
		}
		//垫圈标注参数		
		else if(GetPropID("BOLTPAD.pBolt")==id)				//螺栓垫圈关联螺栓
		{
			if(pCmd->m_paraBoltPad.pBolt)
				sText.Printf("0X%X",pCmd->m_paraBoltPad.pBolt->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("BOLTPAD.iDimPosType")==id)
		{
			if(pCmd->m_paraBoltPad.iDimPosType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Search Auto");
			else 
				sText.Copy("1.Specify");
#else
				sText.Copy("0.自动搜索");
			else 
				sText.Copy("1.指定位置");
#endif
		}
		else if(GetPropID("BOLTPAD.fDimPosOffsetX")==id)	//标注位置X坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraBoltPad.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("BOLTPAD.fDimPosOffsetY")==id)	//标注位置Y坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraBoltPad.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("BOLTPAD.sSpec")==id)
		{
			CLDSBolt *pBolt=pCmd->m_paraBoltPad.pBolt;
			if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD&&pBolt&&pBolt->DianQuan.N>0&&pBolt->DianQuan.thick>0)
				sprintf(sText,"-%dX%d",pBolt->DianQuan.thick,pBolt->DianQuan.N);
			else 
				sText.Printf("");
		}
		//角钢接头信息
		else if(GetPropID("ANGLEJOINT.pObj")==id)			//当前标注构件
		{
			if(pCmd->m_paraAngleJoint.pJoint)
				sText.Printf("0X%X",pCmd->m_paraAngleJoint.idAngleJoint);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("ANGLEJOINT.iDimPosType")==id)		//0.自动搜索位置 1.指定位置
		{
			if(pCmd->m_paraAngleJoint.iDimPosType==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Specify");
			else 
				sText.Printf("0.Search Auto");
#else
				sText.Printf("1.指定位置");
			else 
				sText.Printf("0.自动搜索");
#endif
		}
		else if(GetPropID("ANGLEJOINT.fDimPosOffsetX")==id)	//标注位置X坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraAngleJoint.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ANGLEJOINT.fDimPosOffsetY")==id)	//标注位置Y坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraAngleJoint.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		//螺栓标注参数		
		else if(GetPropID("BOLTINFO.pBolt")==id)				
		{
			CLDSBolt *pBolt=pCmd->m_paraBoltGroupInfo.lsGroupDim.GetDatumBolt();
			if(pBolt)
				sText.Printf("0X%X",pBolt->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("BOLTINFO.pDatumRod")==id)				
		{
			CLDSLinePart *pRod=pCmd->m_paraBoltGroupInfo.lsGroupDim.GetDatumRod();
			if(pRod)
				sText.Printf("0X%X",pRod->handle);
			else 
				sText.Printf("0X0");
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO&&id>HASHTABLESIZE)
		{	//螺栓标注信息，id大于属性哈希表最大值时为BOLTINFO指针
			CLsGroupDimUnit::BOLTINFO *pBoltInfo=(CLsGroupDimUnit::BOLTINFO*)id;
			if(pBoltInfo&&pBoltInfo->hBolt>0x20)
				sText.Printf("0X%X",pBoltInfo->hBolt);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("BOLTINFO.iDimPosType")==id)
		{
			if(pCmd->m_paraBoltGroupInfo.iDimPosType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Search Auto");
			else 
				sText.Copy("1.Specify");
#else
				sText.Copy("0.自动搜索");
			else 
				sText.Copy("1.指定位置");
#endif
		}
		else if(GetPropID("BOLTINFO.sLabel")==id)
			pCmd->m_paraBoltGroupInfo.lsGroupDim.GetDimText(sText);
		else if(GetPropID("BOLTINFO.fDimPosOffsetX")==id)	//标注位置X坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraBoltGroupInfo.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("BOLTINFO.fDimPosOffsetY")==id)	//标注位置Y坐标偏移值
		{
			sText.Printf("%lf",pCmd->m_paraBoltGroupInfo.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		//相似形标注参数
		else if(GetPropID("SIMPOLY.fScale")==id)			//指定相似形比例
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fScale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fPickPtX")==id)			//相似形拾取点X坐标		
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fPickPtX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fPickPtY")==id)			//相似形拾取点Y坐标
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fPickPtY);	
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fDimPosOffsetX")==id)	//相似形X轴方向偏移量		
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fDimPosOffsetY")==id)	//相似形Y轴方向偏移量
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		//标注模型尺寸
		else if(GetPropID("DIMMODELSIZE.fDimDistance")==id)		
		{
			sText.Printf("%lf",pCmd->m_paraDimModelSize.fDimDistance*pCmd->m_pDrawingCom->m_fScale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMMODELSIZE.fSpace")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimModelSize.fSpace*pCmd->m_pDrawingCom->m_fScale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMMODELSIZE.iDimType")==id)
		{
			if(pCmd->m_paraDimModelSize.iDimType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.Inter Node Offset");
			else if(pCmd->m_paraDimModelSize.iDimType==1)
				sText.Printf("1.Vertical Cross Arm Space");
			else if(pCmd->m_paraDimModelSize.iDimType==2)
				sText.Printf("2.Piecewise Interval");
			else if(pCmd->m_paraDimModelSize.iDimType==3)
				sText.Printf("3.Tower Height");
			else if(pCmd->m_paraDimModelSize.iDimType==4)
				sText.Printf("4.Changing Slope Mouth");
			if(pCmd->m_paraDimModelSize.iDimType==5)
				sText.Printf("5.Cross Arm Horizontal Space");	
#else
				sText.Printf("0.节间垂距");
			else if(pCmd->m_paraDimModelSize.iDimType==1)
				sText.Printf("1.横担垂直间距");
			else if(pCmd->m_paraDimModelSize.iDimType==2)
				sText.Printf("2.分段间距");
			else if(pCmd->m_paraDimModelSize.iDimType==3)
				sText.Printf("3.塔高");
			else if(pCmd->m_paraDimModelSize.iDimType==4)
				sText.Printf("4.变坡开口");
			if(pCmd->m_paraDimModelSize.iDimType==5)
				sText.Printf("5.横担水平间距");
#endif
		}
		else if(id>=idNodeIndexStart&&id<=idNodeIndexEnd)
		{
			int nNodeNum=pCmd->m_paraDimModelSize.listDimNodes.GetNodeNum();
			for(int i=0;i<PROPID_SETSIZE;i++)
			{
				CXhChar50 sPropStr("DIMMODELSIZE.Node%d",i);
				if(i<nNodeNum&&CDrawMapScriptDlg::GetPropID(sPropStr)==id
					&&pCmd->m_paraDimModelSize.listDimNodes[i].pNode)	
					sText.Printf("0X%X",pCmd->m_paraDimModelSize.listDimNodes[i].pNode->handle);
			}
		}
		//图面尺寸
		else if(GetPropID("DIMSIZE.fDimStartX")==id)	//图面尺寸标注起始点(相对坐标系下)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimStartX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimStartY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimStartY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimEndX")==id)		//图面尺寸标注起始点(相对坐标系下)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimEndX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimEndY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimEndY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimPosX")==id)		//图面尺寸标注位置(相对坐标系下)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimPosX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimPosY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimPosY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fScale")==id)		//尺寸比例
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fScale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.iDimDirect")==id)	//标注方向 0.X轴方向 1.Y轴方向 2.延伸方向
		{
			if(pCmd->m_paraDimSize.iDimDirect==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.Axis X");
			else if(pCmd->m_paraDimSize.iDimDirect==1)
				sText.Printf("1.Axis Y");
			else 
				sText.Printf("2.Extend Direction");
#else
				sText.Printf("0.X轴方向");
			else if(pCmd->m_paraDimSize.iDimDirect==1)
				sText.Printf("1.Y轴方向");
			else 
				sText.Printf("2.延伸方向");
#endif
		}
		//标注角钢特殊准据
		else if(GetPropID("DIM_JGZHUN.pLineAngle")==id)	//需要标注特殊准据的角钢
		{
			if(pCmd->m_paraDimJgZhun.pLineAngle)
				sText.Printf("0X%X",pCmd->m_paraDimJgZhun.pLineAngle->handle);
			else
				sText.Printf("0X0");
		}
		else if(GetPropID("DIM_JGZHUN.fSpace")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimJgZhun.fSpace);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIM_JGZHUN.fDimDistance")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimJgZhun.fDimDistance);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIM_JGZHUN.bInside")==id)
		{
			if(pCmd->m_paraDimJgZhun.bInside)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Internal");
			else 
				sText.Copy("External");
#else
				sText.Copy("内侧");
			else 
				sText.Copy("外侧");
#endif
		}
		//标注螺栓特殊间距
		else if(GetPropID("DIM_LSSPACE.pPlate")==id)			//需要标注特殊螺栓间距的钢板或角钢
		{
			if(pCmd->m_paraDimLsSpace.pPlate)
				sText.Printf("0X%X",pCmd->m_paraDimLsSpace.pPlate->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("DIM_LSSPACE.hStartLs")==id)
			sText.Printf("0X%X",pCmd->m_paraDimLsSpace.spaceDim.hStartLs);
		else if(GetPropID("DIM_LSSPACE.hEndLs")==id)
			sText.Printf("0X%X",pCmd->m_paraDimLsSpace.spaceDim.hEndLs);
		else if(GetPropID("DIM_LSSPACE.nLsSpaceNum")==id)
			sText.Printf("%d",pCmd->m_paraDimLsSpace.spaceDim.nLsSpaceNum);
		else if(GetPropID("DIM_LSSPACE.bMergeDimLsSpace")==id)
		{
			if(pCmd->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else 
				sText.Copy("No");
#else
				sText.Copy("是");
			else 
				sText.Copy("否");
#endif
		}
		else if(GetPropID("DIM_LSSPACE.fSpace")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimLsSpace.fSpace);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIM_LSSPACE.fDimDistance")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimLsSpace.fDimDistance);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIM_LSSPACE.bInside")==id)
		{
			if(pCmd->m_paraDimLsSpace.bInside)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Internal");
			else 
				sText.Copy("External");
#else
				sText.Copy("内侧");
			else 
				sText.Copy("外侧");
#endif
		}
		//脚钉间距标注
		else if(GetPropID("FOOTNAIL_SPACE.fDimDistance")==id)		
		{
			sText.Printf("%lf",pCmd->m_paraDimFootNailSpace.fDimDistance);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("FOOTNAIL_SPACE.fSpace")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimFootNailSpace.fSpace);
			SimplifiedNumString(sText);
		}
		//填板间距标注
		else if(GetPropID("FILLPLATE_SPACE.fDimDistance")==id)		
		{
			sText.Printf("%lf",pCmd->m_paraDimFillPlateSpace.fDimDistance);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("FILLPLATE_SPACE.fSpace")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimFillPlateSpace.fSpace);
			SimplifiedNumString(sText);
		}
		//相似形标注
		else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY&&pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum()>0)
		{
			int i=1;
			SIMPOLY_VERTEX *pVertex=NULL;
			for(pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
				pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetNext())
			{
				char sKey1[50]="",sKey2[50]="",sKey3[50]="",sKey4[50]="";
				sprintf(sKey1,"SIMPOLY.vertex%d.hPrevPole",i);
				sprintf(sKey2,"SIMPOLY.vertex%d.hNextPole",i);
				sprintf(sKey3,"SIMPOLY.vertex%d.x",i);
				sprintf(sKey4,"SIMPOLY.vertex%d.y",i);
				if(GetPropID(sKey1)==id)
				{
					sText.Printf("0X%X",pVertex->hDatumRod);
					break;
				}
				else if(GetPropID(sKey2)==id)
				{
					sText.Printf("0X%X",pVertex->hViceRod);
					break;
				}
				else if(GetPropID(sKey3)==id)
				{
					sText.Printf("%f",pVertex->vertex.x);
					SimplifiedNumString(sText);
					break;
				}
				else if(GetPropID(sKey4)==id)
				{
					sText.Printf("%f",pVertex->vertex.y);
					SimplifiedNumString(sText);
					break;
				}
				i++;
			}
		}
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

int CDrawMapScriptDlg::GetPropValueStr(long id,char* valueStr)
{
	long handle=0;
	CXhChar100 sText;
	//
	int iCurGroup=m_tabCmdType.GetCurSel();
	if(m_pCurCmd&&!IsSpreadLinePropGroup(iCurGroup))
		return GetPropValueStr(m_pCurCmd,id,valueStr);
	else if(m_pSpreadLine&&IsSpreadLinePropGroup(iCurGroup))
	{	//展开线
		if(GetPropID("SPREAD_LINE.m_iDefType")==id)	//0.指定杆件 1.直接指定
		{
			if(m_pSpreadLine->m_iDefType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.Specify Rod");
			else 
				sText.Printf("1.Directly Specify");
#else
				sText.Printf("0.指定杆件");
			else 
				sText.Printf("1.直接指定");
#endif
		}
		else if(GetPropID("SPREAD_LINE.m_sLineName")==id)	//展开线名称
			sText.Printf("%s",(char*)m_pSpreadLine->m_sLineName);
		else if(GetPropID("SPREAD_LINE.m_hDatumPole")==id)	//基准杆件 基准杆件句柄为0时表示为自定义
			sText.Printf("0X%X",m_pSpreadLine->m_hDatumPole);
		else if(GetPropID("SPREAD_LINE.m_iCurWing")==id)	//基准杆件为角钢时启用该属性
		{
			if(m_pSpreadLine->m_iCurWing==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.X Leg");
			else 
				sText.Printf("1.Y Leg");
#else
				sText.Printf("0.X肢");
			else 
				sText.Printf("1.Y肢");
#endif
		}
		else if(GetPropID("SPREAD_LINE.m_fOffsetDist")==id)	//偏移距离
		{
			sText.Printf("%lf",m_pSpreadLine->m_fOffsetDist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.startPt.x")==id)		//展开线起点X坐标
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.startPt.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.startPt.y")==id)		//展开线起点Y坐标
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.startPt.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.startPt.z")==id)		//展开线起点Z坐标
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.startPt.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.endPt.x")==id)		//展开线终点X坐标
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.endPt.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.endPt.y")==id)		//展开线终点Y坐标
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.endPt.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.endPt.z")==id)		//展开线终点Z坐标
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.endPt.z);
			SimplifiedNumString(sText);
		}
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}

int CDrawMapScriptDlg::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}

BOOL CDrawMapScriptDlg::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return propStatusHashtable.SetValueAt(id,dwStatus);
}

long CDrawMapScriptDlg::GetPropID(char* propStr,...)
{
	va_list ap;
	va_start(ap, propStr);
	CString sRealProp;
	sRealProp.FormatV(propStr,ap);
	va_end(ap);
	long id;
	if(propHashtable.GetValueAt(sRealProp.GetBuffer(1),id))
		return id;
	else
	{
		/*char prop_str[500];
		strcpy(prop_str,"没找到名为\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"的属性ID！";
		AfxMessageBox(prop_str);*/
		return 0;
	}
}

int CDrawMapScriptDlg::GetPropStr(long id, char *propStr)
{
	
	return 0;
}

void CDrawMapScriptDlg::RefreshCommandList(int iListGroup)
{
	if((m_pComponent->m_eClassType==DRAW_COM_STRUVIEW&&
		(iListGroup==GROUP_DRAW_PART-1||iListGroup==GROUP_DIM_ODDMENT-1))
		||(m_pComponent->m_eClassType==DRAW_COM_LINEVIEW&&iListGroup==GROUP_DRAW_PART-1))
	{	//结构图第1、2个属性页 或 单线图第1个属性页使用CListCtrl
		GetDlgItem(IDC_CMD_LIST_SUPER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMD_LIST_COMMON)->ShowWindow(SW_SHOW);
		//m_listCmdCommonCtrl.RedrawItems(-1,-1);
		m_listCmdCommonCtrl.InitSortHeadCtrl();
		if(m_iOldGroup!=iListGroup)	//切换属性页时需更新命令列表
			DisplayCommonCtrlCmdList(iListGroup);
		m_bSuperListCtrlInUsing=FALSE;	//启用普通ListCtrl wht 12-03-22
	}
	else 	
	{
		GetDlgItem(IDC_CMD_LIST_SUPER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CMD_LIST_COMMON)->ShowWindow(SW_HIDE);
		m_listCmdSuperCtrl.m_iPropGroup=iListGroup;
		//修改列名
		long col_wide_arr[4]={25,65,160,45};
		m_listCmdSuperCtrl.EmptyColumnHeader();
		if(IsSpreadLinePropGroup(iListGroup))	//展开线
		{
#ifdef AFX_TARG_ENU_ENGLISH
			m_listCmdSuperCtrl.AddColumnHeader("△");
			m_listCmdSuperCtrl.AddColumnHeader("dimension");
			m_listCmdSuperCtrl.AddColumnHeader("spreading line");
			m_listCmdSuperCtrl.AddColumnHeader("operation");
#else
			m_listCmdSuperCtrl.AddColumnHeader("△");
			m_listCmdSuperCtrl.AddColumnHeader("标识");
			m_listCmdSuperCtrl.AddColumnHeader("展开线");
			m_listCmdSuperCtrl.AddColumnHeader("操作");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			m_listCmdSuperCtrl.AddColumnHeader("△");
			m_listCmdSuperCtrl.AddColumnHeader("dimension");
			m_listCmdSuperCtrl.AddColumnHeader("drawing items");
			m_listCmdSuperCtrl.AddColumnHeader("enable");
#else
			m_listCmdSuperCtrl.AddColumnHeader("△");
			m_listCmdSuperCtrl.AddColumnHeader("标识");
			m_listCmdSuperCtrl.AddColumnHeader("绘制项");
			m_listCmdSuperCtrl.AddColumnHeader("启用");
#endif
		}
		m_bFireListItemChange=FALSE;	//关掉ItemChange触发器，避免频繁调用FireItemChanged wjh-2011.8.22
		m_listCmdSuperCtrl.InitListCtrl(col_wide_arr);
		//内部重启或不同类型的命令组之间进行切换时需要更新属性栏
		if(m_bInernalStart||(IsSpreadLinePropGroup(m_iOldGroup)&&!IsSpreadLinePropGroup(iListGroup))
			||(!IsSpreadLinePropGroup(m_iOldGroup)&&IsSpreadLinePropGroup(iListGroup)))
			DisplayPropertyList(iListGroup);
		m_listCmdSuperCtrl.Redraw();
		m_bFireListItemChange=TRUE;		//重新打开ItemChange触发器
		m_bSuperListCtrlInUsing=TRUE;	//启用SuperListCtrl wht 12-03-22
	}
	m_iOldGroup=iListGroup;
}

void CDrawMapScriptDlg::OnSelchangeTabCmdType(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//切换属性页时清空当前命令以及当前展开线
	m_pCurCmd=NULL;
	m_pSpreadLine=NULL;
	//取消所有选中项
	m_listCmdCommonCtrl.SetItemState(-1,0,LVIS_SELECTED);
	RefreshCommandList(m_tabCmdType.GetCurSel());
	*pResult = 0;
}

//隐藏窗口
void CDrawMapScriptDlg::HideWnd(BOOL bRestart)
{
	int i=0,n=m_listCmdCommonCtrl.GetItemCount();
	for(i=0;i<m_listCmdCommonCtrl.GetItemCount();i++)
	{
		CDrawingCommand *pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(i);
		if(pCmd)	//隐藏窗口时记录命令所在行索引，重启窗口时用来保持当前顺序 wht 11-07-23
			pCmd->feature=i;
	}
	m_iBreakType=3;
	m_bInernalStart=bRestart;
	CDialog::OnCancel();
}

//选择对象节点或线
void CDrawMapScriptDlg::SelectEntObj(int nResultEnt/*=1*/)
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_iBreakType=1;	//拾取点
	m_nResultEnt=nResultEnt;
	CDialog::OnOK();
}

//得到第一个可执行的命令行
CSuperGridCtrl::CTreeItem *CDrawMapScriptDlg::GetFirstExecutableItem()
{
	CSuperGridCtrl::CTreeItem *pItem=NULL,*pExecutableItem=NULL,*pFirstItem=NULL;
	int nItemCount=m_listCmdSuperCtrl.GetRootCount();
	for(int i=0;i<nItemCount;i++)
	{
		pItem=m_listCmdSuperCtrl.GetRootItem(i);
		if(pItem==NULL)
			continue;
		CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
		if(pCmd==NULL)
			continue;
		if(!pCmd->IsNeedExecute())
			continue;	//该命令不需要执行
		if(pCmd->m_stateCmd&CMD_STATE_NEXTPOS)
		{
			pExecutableItem=pItem;
			break;
		}
		pFirstItem=pItem;
	}
	if(pExecutableItem==NULL)
		pExecutableItem=pFirstItem;
	return pExecutableItem;
}

//得到最后一个已执行的的命令行
CSuperGridCtrl::CTreeItem *CDrawMapScriptDlg::GetLastExecutedItem()
{
	CSuperGridCtrl::CTreeItem *pItem=NULL,*pLastExecutedItem=NULL;
	int nItemCount=m_listCmdSuperCtrl.GetRootCount();
	for(int i=0;i<nItemCount;i++)
	{
		pItem=m_listCmdSuperCtrl.GetRootItem(i);
		if(pItem==NULL)
			continue;
		CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
		if(pCmd==NULL)
			continue;
		if(!pCmd->IsNeedExecute()&&pCmd->m_bExecute)
			pLastExecutedItem=pItem;	//该命令不需要执行并且已启用
		else
			break;
	}
	return pLastExecutedItem;
}

void CDrawMapScriptDlg::OnOK()
{
	//屏蔽回车键
}

//根据关键字查找制定项目
void CDrawMapScriptDlg::OnChangeKeyWord() 
{
	UpdateData();
	if(m_sKeyWord.GetLength()<=0)
		return;
	int iGroup=m_tabCmdType.GetCurSel();
	if(iGroup==0)
	{	//取消所有选中项的选中状态
		m_listCmdCommonCtrl.SetItemState(-1,0,LVNI_SELECTED);
		int i=0,nItemCount=m_listCmdCommonCtrl.GetItemCount();
		for(i=0;i<nItemCount;i++)
		{
			CDrawingCommand *pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(i);
			if(pCmd==NULL)
				continue;
			char sId[50]="",sPartNo[50]="",sHelp[200]="";
			pCmd->GetCmdInfo(sId,sHelp,sPartNo);
			CString ss;
			ss.Format("0x%s",m_sKeyWord);
			//比较字符串是否完全相等
			if(m_sKeyWord.CompareNoCase(sId)==0||m_sKeyWord.CompareNoCase(sPartNo)==0
				||m_sKeyWord.CompareNoCase(sHelp)==0||ss.CompareNoCase(sId)==0)
			{
				m_listCmdCommonCtrl.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
				m_listCmdCommonCtrl.EnsureVisible(i,FALSE);	//确保选中项可见
				break;
			}
		}
	}
	else 
	{
		CSuperGridCtrl::CTreeItem *pCurItem=NULL;
		int nItemCount=m_listCmdSuperCtrl.GetRootCount();
		for(int i=0;i<nItemCount;i++)
		{
			CSuperGridCtrl::CTreeItem *pItem=m_listCmdSuperCtrl.GetRootItem(i);
			if(pItem==NULL)
				continue;
			if(pItem->m_dwPropGroup!=GetSingleWord(m_tabCmdType.GetCurSel()+1))
				continue;	//不属于当前分组
			CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
			if(pCmd==NULL)
				continue;
			char sId[50]="",sPartNo[50]="",sHelp[200]="";
			pCmd->GetCmdInfo(sId,sHelp,sPartNo);
			CString ss;
			ss.Format("0x%s",m_sKeyWord);
			//比较字符串是否完全相等
			if(m_sKeyWord.CompareNoCase(sId)==0||m_sKeyWord.CompareNoCase(sPartNo)==0
				||m_sKeyWord.CompareNoCase(sHelp)==0||ss.CompareNoCase(sId)==0)
			{
				pCurItem=pItem;
				break;
			}
		}
		if(pCurItem)
			m_listCmdSuperCtrl.SelectItem(pCurItem);
	}
	UpdateData(FALSE);
}

void CDrawMapScriptDlg::EnableSelectedItem(BOOL bFlag)
{
	POSITION pos = m_listCmdCommonCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel=m_listCmdCommonCtrl.GetNextSelectedItem(pos);
		CDrawingCommand *pCmd=NULL;
		if(iCurSel<0)
			continue;
		pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(iCurSel);
		if(pCmd)
		{
			pCmd->m_bExecute=bFlag;
			if(pCmd->m_bExecute)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				m_listCmdCommonCtrl.SetItemText(iCurSel,5,CString("√"));
#else
				m_listCmdCommonCtrl.SetItemText(iCurSel,5,CString("√"));
#endif
				//生成命令对应的实体 wht 11-07-23
				pCmd->Rerun();
				if(pCmd->m_eCmdType==CMD_DRAW_PART)
				{	//同时生成标注构件明细命令对应的实体
					CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue(pCmd->m_paraPart.hPart);
					if(ppSonCmd)
					{
						(*ppSonCmd)->m_bExecute=TRUE;
						(*ppSonCmd)->Rerun();
					}
				}
			}
			else
			{
				m_listCmdCommonCtrl.SetItemText(iCurSel,5,CString("  "));
				//删除命令对应的实体 wht 11-07-23
				pCmd->EmptyEntList();
				if(pCmd->m_eCmdType==CMD_DRAW_PART)
				{	//禁用绘制构件命令时同时删除标注构件明细命令对应的实体
					CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue(pCmd->m_paraPart.hPart);
					if(ppSonCmd)
					{
						(*ppSonCmd)->EmptyEntList();
						(*ppSonCmd)->m_bExecute=FALSE;
					}
				}
			}
		}
	}
}

void CDrawMapScriptDlg::OnEnableSelectedItem()
{
	EnableSelectedItem(TRUE);
}

void CDrawMapScriptDlg::OnDisableSelectedItem()
{
	EnableSelectedItem(FALSE);
}

void CDrawMapScriptDlg::EnableAllItem(BOOL bFlag)
{
	int i=0,nCount=m_listCmdSuperCtrl.GetRootCount();
	for(i=0;i<nCount;i++)
	{
		CSuperGridCtrl::CTreeItem *pItem=m_listCmdSuperCtrl.GetTreeItem(i);
		if(pItem==NULL)
			continue;
		if((pItem->m_dwPropGroup&GetSingleWord(m_listCmdSuperCtrl.m_iPropGroup+1))==0)
			continue;	//不属于当前分组 wht 10-12-02
		int iType=0;
		pItem->m_lpNodeInfo->GetControlType(3,iType);
		if(iType==GCT_GRAYBUTTON)
			continue;
		CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
		if(pCmd==NULL)
			continue;
		pCmd->m_bExecute=bFlag;
		if(bFlag)
#ifdef AFX_TARG_ENU_ENGLISH
			m_listCmdSuperCtrl.SetSubItemText(pItem,3,CString("√"));
#else
			m_listCmdSuperCtrl.SetSubItemText(pItem,3,CString("√"));
#endif
		else 
			m_listCmdSuperCtrl.SetSubItemText(pItem,3,CString(""));
	}
}

void CDrawMapScriptDlg::OnEnableAllItem()
{
	EnableAllItem(TRUE);
}

void CDrawMapScriptDlg::OnDisableAllItem()
{
	EnableAllItem(FALSE);
}

//执行指定操作 3.延伸杆件心线 4.添加辅助线 5.添加相似形
void CDrawMapScriptDlg::ExecutableCommand(int iOperType)
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_iBreakType=3;	//执行命令
	m_iOperType=iOperType;
	CDialog::OnOK();
}

void CDrawMapScriptDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	if(m_bInernalStart&&m_iBreakType==3)
		DoModal();
}
CDrawMapScriptDlg::~CDrawMapScriptDlg()
{	
	if(m_bPdmodeIs34)
	{	//数据扩展点显示模式为34
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//显示点 .
	#else
		acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//显示点 .
	#endif
		m_bPdmodeIs34=FALSE;
	}
	//取消高亮显示实体 wht 12-03-22
	CDrawingCommand::ReleaseHighlightEnts();
}
void CDrawMapScriptDlg::OnItemchangedCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->uNewState==pNMListView->uOldState)
		return;	//状态未发生变化时直接返回 wht 11-07-23
	if(!m_bFireListItemChange)
		return;	//未启用ItemChange
	CDrawingCommand **ppSonCmd=NULL;
	int iCurGroup=GetCurCmdGroup();
	if(iCurGroup==GROUP_DRAW_PART-1||iCurGroup==GROUP_DIM_ODDMENT-1)
	{
		cmdList.Empty();
		POSITION pos = m_listCmdCommonCtrl.GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel=m_listCmdCommonCtrl.GetNextSelectedItem(pos);
			CDrawingCommand *pCmd=NULL;
			if(iCurSel>=0)
			{
				pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(iCurSel);
				cmdList.append(pCmd);
			}
		}
		CDrawingCommand::ReleaseHighlightEnts();;
		if(cmdList.GetNodeNum()>0)
			m_pCurCmd=*(cmdList.GetFirst());//当前命令为命令集合中第一个选中项对应的命令		
		DisplayPropertyList(iCurGroup);	//更新属性栏
	}
	if(m_pCurCmd)
	{	//当前命令为模型尺寸标注时切换数据扩展点显示模式便于查看节点 wht 11-07-13
		/*if(m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"34",RTNONE);//显示点 X
			//acedCommand(RTSTR,"REGEN",RTNONE);
			m_bPdmodeIs34=TRUE;	//数据扩展点显示模式为34 wht 11-07-15
		}
		else if(m_bPdmodeIs34)
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//显示点 .
			//acedCommand(RTSTR,"REGEN",RTNONE);
			m_bPdmodeIs34=FALSE;
		}*/
		//缩放至命令对应的实体
		ppSonCmd=NULL;
		m_pCurCmd->SetEntSetHighlight();
		if(m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
			ppSonCmd=hashPartSpecList.GetValue(m_pCurCmd->m_paraPart.hPart);
		if(ppSonCmd)
		{
			SCOPE_STRU scope=(*ppSonCmd)->GetCADEntScope();
			ZoomAcadView(scope,50);
			(*ppSonCmd)->SetEntSetHighlight();		
		}
		else //if(ppSonCmd==NULL)
		{
			SCOPE_STRU scope=m_pCurCmd->GetCADEntScope();
			ZoomAcadView(scope,20);
		}
	}
	*pResult = 0;
}

//检测属性值是否相等
BOOL CDrawMapScriptDlg::IsPropEqual(long idProp,int iSonCmdType/*=-1*/)	
{
	CDrawingCommand **ppCmd=cmdList.GetFirst();
	if(cmdList.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	GetPropValueStr(*ppCmd,idProp,valueStr);
	if(iSonCmdType==1&&(*ppCmd)->m_eCmdType==CMD_DRAW_PART)
	{	//标注构件明细命令对应的属性
		CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
		if(ppSonCmd)
			GetPropValueStr(*ppSonCmd,idProp,valueStr);
	}
	else if(iSonCmdType==2&&(*ppCmd)->m_eCmdType==CMD_DIM_PARTSPEC)
	{	//标注杆件螺栓或法兰信息命令对应的属性
		CDrawingCommand **ppSonCmd=hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
		if(ppSonCmd)
			GetPropValueStr(*ppSonCmd,idProp,valueStr);
	}
	int nPush=cmdList.push_stack();
	for(ppCmd=cmdList.GetNext();ppCmd;ppCmd=cmdList.GetNext())
	{
		char valueStr2[500]="";
		if(iSonCmdType==1&&(*ppCmd)->m_eCmdType==CMD_DRAW_PART)
		{	//标注构件明细命令对应的属性
			CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
			if(ppSonCmd)
				GetPropValueStr(*ppSonCmd,idProp,valueStr2);
		}
		else if(iSonCmdType==2&&(*ppCmd)->m_eCmdType==CMD_DIM_PARTSPEC)
		{	//标注杆件螺栓或法兰信息命令对应的属性
			CDrawingCommand **ppSonCmd=hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
			if(ppSonCmd)
				GetPropValueStr(*ppSonCmd,idProp,valueStr2);
		}
		else
			GetPropValueStr(*ppCmd,idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			cmdList.pop_stack(nPush);
			return FALSE;
		}
	}
	cmdList.pop_stack(nPush);
	return TRUE;
}

void CDrawMapScriptDlg::OnDblclkCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	LVHITTESTINFO ht;
	ht.pt = pNMListView->ptAction;
	m_listCmdCommonCtrl.SubItemHitTest(&ht);
	if(ht.iItem<0)
		return;	//normal selection
	if(ht.iSubItem==5)
	{
		CDrawingCommand *pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(ht.iItem);
		if(pCmd&&pCmd->m_bExecute)
		{	//将√改为×
			pCmd->m_bExecute=FALSE;
			m_listCmdCommonCtrl.SetItemText(ht.iItem,ht.iSubItem,CString(" "));
			//删除命令对应的实体 wht 11-07-23
			pCmd->EmptyEntList();
			if(pCmd->m_eCmdType==CMD_DRAW_PART)
			{	
				CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//禁用绘制构件命令时同时删除标注构件明细命令对应的实体
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
					//更新属性栏
					long idProp=CDrawMapScriptDlg::GetPropID("m_bExecute");
					CPropTreeItem *pFindItem=m_propList.FindItemByPropId(idProp,NULL);
					if(*ppSonCmd&&pFindItem)	//更新标注构件明细属性
					{
						char sText[100]="";
						UpdatePartSpecPropertyList(pFindItem,*ppSonCmd,TRUE,TRUE);
						if(GetPropValueStr(idProp,sText)>0)
							m_propList.SetItemPropValue(idProp,sText);
					}
				}
				ppSonCmd=hashOddmentList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)	
				{	//禁用关联的正负头标注命令，删除该命令关联实体
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
				}
				ppSonCmd=hashFootNailSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//禁用关联的脚钉间距标注命令，删除该命令关联实体
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//更新启用属性
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem," ");
							break;
						}
					}
				}
				ppSonCmd=hashFillPlateSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//禁用关联的填板间距标注命令，删除该命令关联实体
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//更新启用属性
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem," ");
							break;
						}
					}
				}
			}
		}
		else if(pCmd&&!pCmd->m_bExecute)
		{	//将×改为√
			pCmd->m_bExecute=TRUE;
			m_listCmdCommonCtrl.SetItemText(ht.iItem,ht.iSubItem,CString("√"));
			//生成命令对应的实体 wht 11-07-23
			pCmd->Rerun();
			if(pCmd->m_eCmdType==CMD_DRAW_PART)
			{	
				CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//同时生成标注构件明细命令对应的实体
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
					//更新属性栏
					long idProp=CDrawMapScriptDlg::GetPropID("m_bExecute");
					CPropTreeItem *pFindItem=m_propList.FindItemByPropId(idProp,NULL);
					if(*ppSonCmd&&pFindItem)	//更新标注构件明细属性
					{
						char sText[100]="";
						UpdatePartSpecPropertyList(pFindItem,*ppSonCmd,TRUE,TRUE);
						if(GetPropValueStr(idProp,sText)>0)
							m_propList.SetItemPropValue(idProp,sText);
					}
				}
				ppSonCmd=hashOddmentList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)	
				{	//启用关联的正负头标注命令，重新运行该命令关联实体
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
				}
				ppSonCmd=hashFootNailSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//启用关联的脚钉间距标注命令，重新运行该命令关联实体
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//更新启用属性
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem,"√");
							break;
						}
					}
				}
				ppSonCmd=hashFillPlateSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//启用关联的填板间距标注命令，重新运行该命令关联实体
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//更新启用属性
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem,"√");
							break;
						}
					}
				}
			}
		}
	}
	*pResult = 0;
}

void CDrawMapScriptDlg::OnRclickCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CAcModuleResourceOverride resOverride;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CPoint point = pNMListView->ptAction;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_SCRIPT);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	if(pMenu==NULL)
		return;
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION);
	//
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_ENABLE_SELECTED_ITEM,"enable selected items");
	pMenu->AppendMenu(MF_STRING,ID_DISABLE_SELECTED_ITEM,"disable selected items");
#else
	pMenu->AppendMenu(MF_STRING,ID_ENABLE_SELECTED_ITEM,"启用选中项");
	pMenu->AppendMenu(MF_STRING,ID_DISABLE_SELECTED_ITEM,"禁用选中项");
#endif
	int nSelectedCount = m_listCmdCommonCtrl.GetSelectedCount();
	if(nSelectedCount==1)
	{
		POSITION pos=m_listCmdCommonCtrl.GetFirstSelectedItemPosition();
		int iCurSel = m_listCmdCommonCtrl.GetNextSelectedItem(pos);
		CDrawingCommand *pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(iCurSel);
		if(pCmd&&pCmd->GetDrawingSerialNo()>0)
		{
			pMenu->AppendMenu(MF_SEPARATOR);
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_ADJUST_DRAWING_SERIAL_NO,"adjust drawing order");
#else
			pMenu->AppendMenu(MF_STRING,ID_ADJUST_DRAWING_SERIAL_NO,"调整绘制顺序");
#endif
			//pMenu->AppendMenu(MF_STRING,ID_MOVE_AFTER,"移动至指定构件之后");
			//pMenu->AppendMenu(MF_STRING,ID_MOVE_BEFORE,"移动至指定构件之前");
			if(pCmd->m_eCmdType==CMD_DRAW_PART&&pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_PLATE)
				pMenu->AppendMenu(MF_STRING,ID_ADD_ITEM,"添加螺栓间距标注");
		}
	}
	CPoint menu_pos=point;
	m_listCmdCommonCtrl.ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,this);
	*pResult = 0;
}

void CDrawMapScriptDlg::OnMoveBefore() 
{
		
}

void CDrawMapScriptDlg::OnMoveAfter() 
{
	
}

void CDrawMapScriptDlg::OnAdjustDrawingSerialNo() 
{
	if(m_pComponent==NULL||m_pComponent->m_eClassType!=DRAW_COM_STRUVIEW)
		return;
	if(m_pCurCmd==NULL&&m_pCurCmd->GetDrawingSerialNo()<=0)
		return;
	CAcModuleResourceOverride resOverride;
	CAdjustPartDrawingSerialNoDlg dlg;
	dlg.m_nMaxSerialNo=m_nMaxCmdDrawingSerialNo;
	dlg.m_nCurSerialNo=m_pCurCmd->GetDrawingSerialNo();
	if(dlg.DoModal()!=IDOK)
		return;
	((CStruView*)m_pComponent)->MoveSpecCmdSerialNoAfterDestCmdSerialNo(dlg.m_nCurSerialNo,dlg.m_nDestSerialNo);
	//刷新界面
	DisplayCommonCtrlCmdList();
}
void CDrawMapScriptDlg::OnAddItem()
{
	if(m_pComponent==NULL||m_pComponent->m_eClassType!=DRAW_COM_STRUVIEW)
		return;
	if(m_pCurCmd==NULL&&m_pCurCmd->GetDrawingSerialNo()<=0)
		return;
	if( m_pCurCmd->m_eCmdType!=CMD_DRAW_PART||m_pCurCmd->m_paraPart.pPart==NULL||
		m_pCurCmd->m_paraPart.pPart->GetClassTypeId()!=CLS_PLATE)
		return;
	CStruView *pStruView=(CStruView*)m_pComponent;
	CLDSPlate *pPlate=(CLDSPlate*)m_pCurCmd->m_paraPart.pPart;
	if(pPlate==NULL)
		return;
	//添加螺栓标注命令
	CDrawingCommand *pCmd=pStruView->cmdList.append();
	pCmd->m_pDrawingCom=m_pComponent;
	pCmd->m_eCmdType=CMD_DIM_SPECIAL_LSSPACE;
	pCmd->InitCommandPara();
	//pCmd->m_paraDimLsSpace.spaceDim=*pSpaceDim;
	pCmd->m_paraDimLsSpace.fDimDistance=6;
	pCmd->m_paraDimLsSpace.fSpace=0;
	pCmd->m_paraDimLsSpace.spaceDim.hFatherPlate=pPlate->handle;
	pCmd->m_paraDimLsSpace.hPlate=pPlate->handle;
	pCmd->m_paraDimLsSpace.pPlate=pPlate;
	//显示螺栓标注命令参数
	int iItem=m_listCmdCommonCtrl.GetSelectedItem();
	iItem=InsertCmdToListCtrl(0,pCmd,iItem+1);
	m_listCmdCommonCtrl.SetItemState(-1,0,LVIS_SELECTED);
	m_listCmdCommonCtrl.SetItemState(iItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_listCmdCommonCtrl.EnsureVisible(iItem,FALSE);	//确保选中项可见 wht 11-07-25
}
#endif
