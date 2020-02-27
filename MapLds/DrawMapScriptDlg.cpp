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
	m_listCmdSuperCtrl.AddColumnHeader("��");
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
	m_listCmdCommonCtrl.AddColumnHeader("��");
	m_listCmdCommonCtrl.AddColumnHeader("sequence");
#else
	m_listCmdSuperCtrl.AddColumnHeader("��ʶ");
	m_listCmdSuperCtrl.AddColumnHeader("������");
	m_listCmdSuperCtrl.AddColumnHeader("����");
	//
	m_listCmdCommonCtrl.AddColumnHeader("�ڵ�");
	m_listCmdCommonCtrl.AddColumnHeader("����");
	m_listCmdCommonCtrl.AddColumnHeader("���");
	m_listCmdCommonCtrl.AddColumnHeader("����");
	m_listCmdCommonCtrl.AddColumnHeader("��ʽ");
	m_listCmdCommonCtrl.AddColumnHeader("��");
	m_listCmdCommonCtrl.AddColumnHeader("˳��");
#endif
	m_pCurCmd=NULL;
	m_iOperType=-1;
	m_iOldGroup=0;
	m_pSpreadLine=NULL;
	m_idEventProp=0;
	m_pDrawing=NULL;
	m_pComponent=NULL;
	m_bCallByDrawingDlg=FALSE;	//ͨ����ͼ�Ի������
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

const long PROPID_SETSIZE = 50;	//���Լ���Ĭ�ϳߴ� wht 12-03-22
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
CStringKeyHashTable<long> CDrawMapScriptDlg::propHashtable;
CHashTable<DWORD> CDrawMapScriptDlg::propStatusHashtable;	//����״̬��ϣ��
const int GROUP_DRAW_PART=1,GROUP_DIM_ODDMENT=2,GROUP_DIM_SIMPOLY=3,GROUP_SPREADLINE=4;
//���¸˼���������˨��Ϣ
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
	CXhChar16 sName("��˨");
	if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
		sName.Copy("����");
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
		{	//ʼ������ͷ��עλ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "starting position offset";
			lpInfo->m_strPropHelp.Format("starting %s dimension position offset value relative to the annotation basis points .",(char*)sName);
#else
			lpInfo->m_strPropName = "ʼ��λ��ƫ��";
			lpInfo->m_strPropHelp.Format("ʼ��%s��עλ������ڱ�ע�����ƫ�ơ�",(char*)sName);
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_bHideChildren=TRUE;
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//ʼ������ͷ��עλ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("starting %s dimension position offset in axis X direction.",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("ʼ��%s��עλ��X����ƫ�ơ�",(char*)sName);
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetX");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pDrawMapDlg->IsPropEqual(pSonPropItem->m_idProp,2);
			//ʼ������ͷ��עλ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("starting %s dimension position offset in axis Y.",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("ʼ��%s��עλ��Y����ƫ�ơ�",(char*)sName);
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fStartOddPosOffsetY");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pDrawMapDlg->IsPropEqual(pSonPropItem->m_idProp,2);
		}
		if(pCmd->m_paraBoltOrFlInfo.bDimEndOdd)
		{	//�ն�����ͷ��עλ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "ending position offset value";
			lpInfo->m_strPropHelp.Format("ending %s dimension position offset value relative to dimension basis points.",(char*)sName);
#else
			lpInfo->m_strPropName = "�ն�λ��ƫ��";
			lpInfo->m_strPropHelp.Format("�ն�%s��עλ������ڱ�ע�����ƫ�ơ�",(char*)sName);
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_bHideChildren=TRUE;
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//�ն�����ͷ��עλ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("ending %s dimension position offset in axis X.",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("�ն�%s��עλ��X����ƫ�ơ�",(char*)sName);
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,bUpdate);
			pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.fEndOddPosOffsetX");
			if(pDrawMapDlg->GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pDrawMapDlg->IsPropEqual(pSonPropItem->m_idProp,2);
			//�ն�����ͷ��עλ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp.Format("ending %s dimension position offset in axis Y .",(char*)sName);
#else
			lpInfo->m_strPropHelp.Format("�ն�%s��עλ��Y����ƫ�ơ�",(char*)sName);
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
		//������ϸ��ע����
		if(CDrawMapScriptDlg::GetPropID("m_bExecute")==pItem->m_idProp)	
		{	//�Ƿ�ִ�и�����
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
				if(valueStr.CompareNoCase("��")==0)
#endif
					(*ppSonCmd)->m_bExecute=TRUE;
				else 
					(*ppSonCmd)->m_bExecute=FALSE;
				if((*ppSonCmd)->m_bExecute!=bOld)
				{
					if(bOld)	//ɾ�������Ӧ��ʵ��
						(*ppSonCmd)->EmptyEntList();
					else		//�������и�����
						pFirstCmd->Rerun();
				}
			}
			if(pFirstCmd)	//���±�ע������ϸ����
				pScriptDlg->UpdatePartSpecPropertyList(pItem,pFirstCmd,TRUE,TRUE);
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX")==pItem->m_idProp)	//��עλ��X����ƫ��ֵ
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY")==pItem->m_idProp)	//��עλ��Y����ƫ��ֵ
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX")==pItem->m_idProp)	//��עλ��X����ƫ��ֵ
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY")==pItem->m_idProp)	//��עλ��Y����ƫ��ֵ
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX")==pItem->m_idProp)	//����עλ��X����ƫ��ֵ
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY")==pItem->m_idProp)	//����עλ��Y����ƫ��ֵ
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iDimPosType")==pItem->m_idProp)		//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)  2.ѡ��λ��
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDrawRefLine")==pItem->m_idProp)//�Ƿ����������
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
				if(valueStr.CompareNoCase("��")==0)
#endif
					(*ppSonCmd)->m_paraPartSpec.bDrawRefLine=TRUE;
				else 
					(*ppSonCmd)->m_paraPartSpec.bDrawRefLine=FALSE;
				if(bOldDrawRefLine!=(*ppSonCmd)->m_paraPartSpec.bDrawRefLine)
					(*ppSonCmd)->Rerun();	//ɾ���������ɵ�ԭ��ʵ��,�������� wht 11-07-18
			}
			//���½�����ʾ
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
				{	//����������ϸλ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("indication line and its detailed location");
					lpInfo->m_strPropHelp = "used to adjust the location of indication line and parts.";
#else
					lpInfo->m_strPropName.Format("�����߼���ϸλ��");
					lpInfo->m_strPropHelp = "����������������ߺ͹�����ϸλ�á�";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.MovePartSpec");
				}
#ifdef AFX_TARG_ENU_ENGLISH
				char sTemp[10]="number";
				if(pPart)
					strcpy(sTemp,"details");
#else
				char sTemp[10]="���";
				if(pPart)
					strcpy(sTemp,"��ϸ");
#endif
				//���λ��ƫ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("%s position offset",sTemp);
				lpInfo->m_strPropHelp.Format("%s position offset value",sTemp);
#else
				lpInfo->m_strPropName.Format("%sλ��ƫ����",sTemp);
				lpInfo->m_strPropHelp.Format("%sλ��ƫ������",sTemp);
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//���λ��X����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp.Format("%s dimension position offset value in axis X direction.",sTemp);
#else
				lpInfo->m_strPropName.Format("X�᷽��");
				lpInfo->m_strPropHelp.Format("%s��עλ��X�᷽��ƫ������",sTemp);
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX");
				if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				//���λ��Y����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp.Format("%s dimension position offset value in axis Y.",sTemp);
#else
				lpInfo->m_strPropName.Format("Y�᷽��");
				lpInfo->m_strPropHelp.Format("%s��עλ��Y�᷽��ƫ������",sTemp);
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY");
				if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				//
				if(pPart&&pPart->IsPlate())
				{	//���λ��ƫ����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("spec position offset");
					lpInfo->m_strPropHelp = "specifications dimension position offset value.";
#else
					lpInfo->m_strPropName.Format("���λ��ƫ����");
					lpInfo->m_strPropHelp = "����עλ��ƫ������";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.GuigeOffset");
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//����עλ��X����ƫ��ֵ
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis X");
					lpInfo->m_strPropHelp = "specifications dimension position offset value relative to number position in axis X direction";
#else
					lpInfo->m_strPropName.Format("X�᷽��");
					lpInfo->m_strPropHelp = "����עλ���������λ����X�᷽���ƫ������";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
					//����עλ��Y����ƫ��ֵ
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis Y");
					lpInfo->m_strPropHelp = "specifications dimension position offset value relative to number position in axis Y direction";
#else
					lpInfo->m_strPropName.Format("Y�᷽��");
					lpInfo->m_strPropHelp = "����עλ���������λ����Y�᷽���ƫ������";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				}
				if(pFirstCmd->m_paraPartSpec.bDrawRefLine)
				{	//���ݵ�λ��ƫ���� 
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("data point position offset");
					lpInfo->m_strPropHelp = "data point offset value";
#else
					lpInfo->m_strPropName.Format("���ݵ�λ��ƫ����");
					lpInfo->m_strPropHelp = "���ݵ�λ��ƫ������";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.BasePosOffset");
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//��עλ��X����ƫ��ֵ
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis X");
					lpInfo->m_strPropHelp = "number dimension position offset value in axis X.";
#else
					lpInfo->m_strPropName.Format("X�᷽��");
					lpInfo->m_strPropHelp = "��ű�עλ��X�᷽��ƫ������";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
					//��עλ��Y����ƫ��ֵ
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("Axis Y");
					lpInfo->m_strPropHelp = "number dimension position offset in axis Y direction";
#else
					lpInfo->m_strPropName.Format("Y�᷽��");
					lpInfo->m_strPropHelp = "��ű�עλ��Y�᷽��ƫ������";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
					pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY");
					if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pSonPropItem->m_idProp,1);
				}
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fPoleDimPosCoef")==pItem->m_idProp)//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
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
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iMirMsg")==pItem->m_idProp)//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
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
				if(iOldMirMsg!=(*ppSonCmd)->m_paraPartSpec.iMirMsg)	//�޸ĶԳƷ�ʽ�����±�ע�ù���
					(*ppSonCmd)->Rerun();	//ɾ���������ɵ�ԭ��ʵ��,��������
			}
		}
		else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimPoleSpec")==pItem->m_idProp)	//��ע�Ǹֳ���
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
				if(valueStr.CompareNoCase("��")==0)
#endif
					(*ppSonCmd)->m_paraPartSpec.bOnlyDimPartNo=FALSE;
				else 
					(*ppSonCmd)->m_paraPartSpec.bOnlyDimPartNo=TRUE;
				if(bOld!=(*ppSonCmd)->m_paraPartSpec.bOnlyDimPartNo)	//�޸ĶԳƷ�ʽ�����±�ע�ù���
					(*ppSonCmd)->Rerun();			//ɾ���������ɵ�ԭ��ʵ��,��������
			}
		}
	}
	else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
	{	//�˼���˨��Ϣ������Ϣ
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
	{	//չ����
		if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iDefType")==pItem->m_idProp)		
		{	//0.ָ���˼� 1.ֱ��ָ��
			pScriptDlg->m_pSpreadLine->m_iDefType=valueStr[0]-'0';
			pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole"));
			pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing"));
			pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_fOffsetDist"));
			CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDrawMapScriptDlg::GetPropID("SPREAD_LINE"),NULL);
			if(pScriptDlg->m_pSpreadLine->m_iDefType==0&&pParentItem)
			{	//��׼�˼� ��׼�˼����Ϊ0ʱ��ʾΪ�Զ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("datum angle");
				lpInfo->m_strPropHelp = "spread axis positioning datum angle.";
#else
				lpInfo->m_strPropName.Format("��׼�Ǹ�");
				lpInfo->m_strPropHelp = "չ���߶�λ��׼�Ǹ֡�";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,2,TRUE);
				pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole");
				if(pScriptDlg->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				//��׼�˼�Ϊ�Ǹ�ʱ���ø�����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="0.X Leg|1.Y Leg|";
				lpInfo->m_strPropName.Format("locating leg");
				lpInfo->m_strPropHelp = "spread axis locating leg of datum angle.";
#else
				lpInfo->m_cmbItems="0.X֫|1.Y֫|";
				lpInfo->m_strPropName.Format("��λ֫");
				lpInfo->m_strPropHelp = "չ���߻�׼�Ǹֶ�λ֫��";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,3,TRUE);
				pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing");
				if(pScriptDlg->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				//ƫ�ƾ���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("offset");
				lpInfo->m_strPropHelp = "distance between offset line and angle's ridge.";
#else
				lpInfo->m_strPropName.Format("ƫ�ƾ���");
				lpInfo->m_strPropHelp = "ƫ������Ǹ�����֮��ľ��롣";
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
			pScriptDlg->m_pSpreadLine->m_sLineName.Printf("%s",valueStr);//չ��������
			//�����Ҳ������б�������
			int iCurSel=pScriptDlg->m_listCmdSuperCtrl.GetSelectedItem();
			CSuperGridCtrl::CTreeItem *pCurItem=NULL;
			pCurItem=pScriptDlg->m_listCmdSuperCtrl.GetTreeItem(iCurSel);
			if(pCurItem==NULL||(pCurItem&&pCurItem->m_idProp!=(long)pScriptDlg->m_pSpreadLine))
				pCurItem=pScriptDlg->m_listCmdSuperCtrl.FindTreeItemById((long)pScriptDlg->m_pSpreadLine);
			if(pCurItem&&pCurItem->m_lpNodeInfo)
				pScriptDlg->m_listCmdSuperCtrl.SetSubItemText(pCurItem,2,valueStr);
		}
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole")==pItem->m_idProp)
		{	//��׼�˼� ��׼�˼����Ϊ0ʱ��ʾΪ�Զ���
			sscanf(valueStr,"%X",&pScriptDlg->m_pSpreadLine->m_hDatumPole);
			CLDSLinePart *pLinePart=(CLDSLinePart*)Ta.FromPartHandle(pScriptDlg->m_pSpreadLine->m_hDatumPole,CLS_LINEPART);
			if(pLinePart==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("handle input is not legitimate part, please input again!");
#else
				AfxMessageBox("���������ǺϷ��˼�����������!");
#endif
				::SetFocus(pPropList->GetSafeHwnd());	//��������ý���
				pPropList->SetCurSel(pItem->m_iIndex);	//ѡ�е�ǰ��
				return FALSE;
			}
			if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("temporarily only support the situation that datum parts are angles, please input handle again!");
#else
				AfxMessageBox("��ʱ��֧�ֻ�׼�˼�Ϊ�Ǹֵ������������������!");
#endif
				::SetFocus(pPropList->GetSafeHwnd());	//��������ý���
				pPropList->SetCurSel(pItem->m_iIndex);		//ѡ�е�ǰ��
				return FALSE;
			}
		}
		if((CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing")==pItem->m_idProp)
			||(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_fOffsetDist")==pItem->m_idProp))
		{
			if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_iCurWing")==pItem->m_idProp)
				pScriptDlg->m_pSpreadLine->m_iCurWing=valueStr[0]-'0';	//��׼�˼�Ϊ�Ǹ�ʱ���ø�����	
			else //if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_fOffsetDist")==pItem->m_idProp)	
				pScriptDlg->m_pSpreadLine->m_fOffsetDist=atof(valueStr);//ƫ�ƾ���
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
			pScriptDlg->m_pSpreadLine->m_lineSpread.startPt.x=atof(valueStr);//չ�������X����
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.y")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.startPt.y=atof(valueStr);//չ�������Y����
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.startPt.z")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.startPt.z=atof(valueStr);//չ�������Z����
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.x")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.endPt.x=atof(valueStr);	//չ�����յ�X����
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.y")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.endPt.y=atof(valueStr);	//չ�����յ�Y����
		else if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.endPt.z")==pItem->m_idProp)		
			pScriptDlg->m_pSpreadLine->m_lineSpread.endPt.z=atof(valueStr);	//չ�����յ�Z����
	}
	else if(pScriptDlg->m_pCurCmd)
	{	//�������
		CDrawingCommand **ppCmd=NULL;
		long idNodeIndexStart=-1,idNodeIndexEnd=-1;
		idNodeIndexStart=CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.Node0");
		idNodeIndexEnd=CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.Node%d",PROPID_SETSIZE-1);
		if(CDrawMapScriptDlg::GetPropID("m_eCmdType")==pItem->m_idProp)
		{	//��������
		}
		else if(CDrawMapScriptDlg::GetPropID("m_stateCmd")==pItem->m_idProp)		
		{	//����״̬
		}
		//ͨ���Ҽ��˵������޸�
		/*else if(CDrawMapScriptDlg::GetPropID("m_bExecute")==pItem->m_idProp)	
		{	//�Ƿ�ִ�и�����
			for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
#endif
					(*ppCmd)->m_bExecute=TRUE;
				else 
					(*ppCmd)->m_bExecute=FALSE;
			}
		}*/
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
		{	//�������Ʋ���
			if(CDrawMapScriptDlg::GetPropID("PART.angle.extendStyle")==pItem->m_idProp)//�Ǹ�֫����չ��ʽ
			{
				//pScriptDlg->m_pCurCmd->m_paraPart.para.angle.extendStyle=valueStr[0]-'0';
				for(CDrawingCommand **ppCurCmd=pScriptDlg->cmdList.GetFirst();ppCurCmd;ppCurCmd=pScriptDlg->cmdList.GetNext())
					(*ppCurCmd)->m_paraPart.para.angle.extendStyle=valueStr[0]-'0';
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.angle.bDrawAxisLine")==pItem->m_idProp)//�Ǹ�֫����չ��ʽ
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
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
			else if(CDrawMapScriptDlg::GetPropID("PART.fPoleAxisLineStartOutSpreadLen")==pItem->m_idProp)//�˼�����ʼ�����쳤��
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
					if(fabs(fOldStartLen-pPartPara->fPoleAxisLineStartOutSpreadLen)>0	//ʼ���������б䶯
						&&pPartPara->pPart&&pPartPara->pPart->IsLinePart())				//��ǰ����Ϊ�˼�
					{	//�����˼����߳���
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
				//���½���
				actrTransactionManager->flushGraphics(); //�رմ򿪵�ʵ���ſɳɹ�ˢ�»���
				acedUpdateDisplay();
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.fPoleAxisLineEndOutSpreadLen")==pItem->m_idProp)	//�˼������ն����쳤��
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
					if(fabs(fOldEndLen-pPartPara->fPoleAxisLineEndOutSpreadLen)>0	//ʼ���������б䶯
						&&pPartPara->pPart&&pPartPara->pPart->IsLinePart())			//��ǰ����Ϊ�˼�
					{	//�����˼����߳���
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
				//���½���
				actrTransactionManager->flushGraphics(); //�رմ򿪵�ʵ���ſɳɹ�ˢ�»���
				acedUpdateDisplay();
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.bolt.bDrawSideBoltLine")==pItem->m_idProp)	//�˼������ն����쳤��
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
				//���½���
				actrTransactionManager->flushGraphics(); //�رմ򿪵�ʵ���ſɳɹ�ˢ�»���
				acedUpdateDisplay();
			}
			else if(CDrawMapScriptDlg::GetPropID("PART.bolt.bDimHoleDiameter")==pItem->m_idProp)	//�˼������ն����쳤��
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
					if(valueStr.CompareNoCase("��")==0)
#endif
						pCurCmd->m_paraPart.para.bolt.bDimHoleDiameter=true;
					else
						pCurCmd->m_paraPart.para.bolt.bDimHoleDiameter=false;
					pCurCmd->Rerun();
				}
				//���½���
				actrTransactionManager->flushGraphics(); //�رմ򿪵�ʵ���ſɳɹ�ˢ�»���
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
			else if(CDrawMapScriptDlg::GetPropID("PART.angle.nXWingDatumAxisG")==pItem->m_idProp)//X֫��׼�ľ�
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
			else if(CDrawMapScriptDlg::GetPropID("PART.angle.nYWingDatumAxisG")==pItem->m_idProp)//Y֫��׼�ľ�
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
					(*ppCmd)->m_paraPart.para.angle.nYWingDatumAxisG=atoi(valueStr);
			}
			else 
				return ModifySonItemValue(pPropList,pItem,valueStr);
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_LINEPART)
		{	//���Ƹ˼����߲���
			if(CDrawMapScriptDlg::GetPropID("LINEPART.bSum")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_LINEPART)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("��")==0)
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
					if(valueStr.CompareNoCase("��")==0)
#endif
						(*ppCmd)->m_paraLinePart.bDimByAnalysisMode=TRUE;
					else 
						(*ppCmd)->m_paraLinePart.bDimByAnalysisMode=FALSE;
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DRAW_LINEPART)
		{	//�����Ʋ���
			if(CDrawMapScriptDlg::GetPropID("TABLE.nLocatePos")==pItem->m_idProp)	//0.���� 1.���� 2.���� 3.����
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
					(*ppCmd)->m_paraBOMTable.nLocatePos=valueStr[0]-'0';
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.bSelectPos")==pItem->m_idProp)	//ѡ����λ��
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("��")==0)
#endif
						(*ppCmd)->m_paraBOMTable.bSelectPos=TRUE;
					else 
						(*ppCmd)->m_paraBOMTable.bSelectPos=FALSE;
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.bDisplayDlg")==pItem->m_idProp)	//��ʾѡ��λ��ĶԻ���
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("��")==0)
#endif
						(*ppCmd)->m_paraBOMTable.bDisplayDlg=TRUE;
					else 
						(*ppCmd)->m_paraBOMTable.bDisplayDlg=FALSE;
				}
				pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("TABLE.nLocatePos"));
				if(!pScriptDlg->m_pCurCmd->m_paraBOMTable.bDisplayDlg)
				{	//���λλ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_cmbItems="0.Upper Left|1.Upper Right|2.Lower Left|3.Lower Right|";
					lpInfo->m_strPropName.Format("table positioning location");
					lpInfo->m_strPropHelp = "table positioning location";
#else
					lpInfo->m_cmbItems="0.����|1.����|2.����|3.����|";
					lpInfo->m_strPropName.Format("���λλ��");
					lpInfo->m_strPropHelp = "���λλ��";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
					pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("TABLE.nLocatePos");
					if(pScriptDlg->GetPropValueStr(pPropItem->m_idProp,tem_str)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.fDimPosX")==pItem->m_idProp)//���λ��X����		
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DRAW_TABLE)
						continue;
					(*ppCmd)->m_paraBOMTable.fDimPosX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("TABLE.fDimPosY")==pItem->m_idProp)//���λ��Y����
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
		{	//������ϸ��ע����
			if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetX")==pItem->m_idProp)	//��עλ��X����ƫ��ֵ
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fBasePosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fBasePosOffsetY")==pItem->m_idProp)	//��עλ��Y����ƫ��ֵ
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fBasePosOffsetY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetX")==pItem->m_idProp)	//��עλ��X����ƫ��ֵ
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fDimPosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fDimPosOffsetY")==pItem->m_idProp)	//��עλ��Y����ƫ��ֵ
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fDimPosOffsetY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetX")==pItem->m_idProp)	//����עλ��X����ƫ��ֵ
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fGuigeOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fGuigeOffsetY")==pItem->m_idProp)	//����עλ��Y����ƫ��ֵ
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fGuigeOffsetY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iDimPosType")==pItem->m_idProp)		//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)  2.ѡ��λ��
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.iDimPosType=valueStr[0]-'0';
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDrawRefLine")==pItem->m_idProp)//�Ƿ����������
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					BOOL bOldDrawRefLine=(*ppCmd)->m_paraPartSpec.bDrawRefLine;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("��")==0)
#endif
						(*ppCmd)->m_paraPartSpec.bDrawRefLine=TRUE;
					else 
						(*ppCmd)->m_paraPartSpec.bDrawRefLine=FALSE;
					if(bOldDrawRefLine!=(*ppCmd)->m_paraPartSpec.bDrawRefLine)
						(*ppCmd)->Rerun();	//ɾ���������ɵ�ԭ��ʵ��,�������� wht 11-07-18
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.fPoleDimPosCoef")==pItem->m_idProp)//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					(*ppCmd)->m_paraPartSpec.fPoleDimPosCoef=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.iMirMsg")==pItem->m_idProp)//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					int iOldMirMsg=(*ppCmd)->m_paraPartSpec.iMirMsg;
					(*ppCmd)->m_paraPartSpec.iMirMsg=valueStr[0]-'0';
					if(iOldMirMsg!=(*ppCmd)->m_paraPartSpec.iMirMsg)	//�޸ĶԳƷ�ʽ�����±�ע�ù���
						(*ppCmd)->Rerun();	//ɾ���������ɵ�ԭ��ʵ��,��������
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.bDimPoleSpec")==pItem->m_idProp)	//����ע���
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_PARTSPEC)
						continue;
					BOOL bOld=(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("��")==0)
#endif
						(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo=FALSE;
					else 
						(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo=TRUE;
					if(bOld!=(*ppCmd)->m_paraPartSpec.bOnlyDimPartNo)
						(*ppCmd)->Rerun();			//ɾ���������ɵ�ԭ��ʵ��,��������
				}
			}
			else 
				return ModifySonItemValue(pPropList,pItem,valueStr);
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SEGI)
		{	//��ע�κ�
			if(CDrawMapScriptDlg::GetPropID("DIMSEGI.bSelectPos")==pItem->m_idProp)	//ѡ����λ��
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SEGI)
						continue;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Yes")==0)
#else
					if(valueStr.CompareNoCase("��")==0)
#endif
						(*ppCmd)->m_paraDimSegI.bSelectPos=TRUE;
					else 
						(*ppCmd)->m_paraDimSegI.bSelectPos=FALSE;
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSEGI.nSegI")==pItem->m_idProp)		//�����κ�
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SEGI)
						continue;
					(*ppCmd)->m_paraDimSegI.nSegI=SEGI(valueStr.GetBuffer());
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSEGI.fDimPosX")==pItem->m_idProp)	//��עλ��X����
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SEGI)
						continue;
					(*ppCmd)->m_paraDimSegI.fDimPosX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSEGI.fDimPosY")==pItem->m_idProp)	//��עλ��Y����
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
		{	//����ͷ��ע����
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
				{	//����������ʾ����
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.StartOddPosOffset"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetX"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetY"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.EndOddPosOffset"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetX"));
					pPropList->DeleteItemByPropId(CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetY"));
					if(pFirstCmd->m_paraOddment.iDimPosType==1)
					{	//ʼ������ͷ��עλ��
						CPropTreeItem *pSonPropItem=NULL;
						if(pFirstCmd->m_paraOddment.bDimStartOdd)
						{
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "starting position offset";
							lpInfo->m_strPropHelp = "dimension position offset value of starting ends.";
#else
							lpInfo->m_strPropName = "ʼ��λ��ƫ��";
							lpInfo->m_strPropHelp = "ʼ������ͷ��עλ��ƫ������";
#endif
							pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
							pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.StartOddPosOffset");
							if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
								pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
							//ʼ������ͷ��עλ��X����
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of starting ends in axis X direction.";
#else
							lpInfo->m_strPropHelp = "ʼ������ͷ��עλ��ƫ����X���ꡣ";
#endif
							pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
							pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetX");
							if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
								pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
							pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
							//ʼ������ͷ��עλ��Y����
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of starting ends in axis Y diection.";
#else
							lpInfo->m_strPropHelp = "ʼ������ͷ��עλ��ƫ����Y���ꡣ";
#endif
							pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
							pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.fStartOddPosOffsetY");
							if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
								pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
							pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
						}
						if(pFirstCmd->m_paraOddment.bDimEndOdd)
						{	//�ն�����ͷ��עλ��
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropName = "ending position offset";
							lpInfo->m_strPropHelp = "dimension position offset value of ending's ends relative to dimension basis points.";
#else
							lpInfo->m_strPropName = "�ն�λ��ƫ��";
							lpInfo->m_strPropHelp = "�ն�����ͷ��עλ������ڱ�ע�����ƫ������";
#endif
							pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
							pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.EndOddPosOffset");
							if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
								pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
							//�ն�����ͷ��עλ��X����
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of ending's ends in axis X diection.";
#else
							lpInfo->m_strPropHelp = "�ն�����ͷ��עλ��ƫ����X���ꡣ";
#endif
							pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
							pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ODDMENT.fEndOddPosOffsetX");
							if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
								pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
							pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
							//�ն�����ͷ��עλ��Y����
							lpInfo = new CItemInfo();
							lpInfo->m_controlType=PIT_BUTTON;
							lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
							lpInfo->m_strPropHelp = "dimension position offset value of ending's ends in axis Y diection.";
#else
							lpInfo->m_strPropHelp = "�ն�����ͷ��עλ��ƫ����Y���ꡣ";
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
		{	//������ϸ��ע����
			if(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.iDimPosType")==pItem->m_idProp)		//0.�Զ�����λ�� 1.ָ��λ��
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
				{	//����������ʾ����
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
						lpInfo->m_strPropName.Format("��עλ��ƫ����");
						lpInfo->m_strPropHelp = "��עλ��ƫ������";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
						pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ANGLEJOINT.DimPosOffset");
						if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
							pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
						//��עλ��X����
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position X coordinate.";
#else
						lpInfo->m_strPropHelp = "��עλ��X���ꡣ";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetX");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp,3);
						//��עλ��Y����
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position Y coordinate.";
#else
						lpInfo->m_strPropHelp = "��עλ��Y���ꡣ";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetY");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp,3);
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetX")==pItem->m_idProp)	//��עλ��X����ƫ��ֵ
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
			else if(CDrawMapScriptDlg::GetPropID("ANGLEJOINT.fDimPosOffsetY")==pItem->m_idProp)	//��עλ��Y����ƫ��ֵ
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
		{	//��Ȧ��ע����
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
				{	//����������ʾ����
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
						lpInfo->m_strPropName.Format("��עλ��ƫ����");
						lpInfo->m_strPropHelp = "��עλ��ƫ������";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
						pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTPAD.DimPosOffset");
						if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
							pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
						//��עλ��X����
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position X coordinate.";
#else
						lpInfo->m_strPropHelp = "��עλ��X���ꡣ";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetX");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
						//��עλ��Y����
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position Y coordinate.";
#else
						lpInfo->m_strPropHelp = "��עλ��Y���ꡣ";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetY");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetX")==pItem->m_idProp)//��עλ��X����ƫ��ֵ
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
			else if(CDrawMapScriptDlg::GetPropID("BOLTPAD.fDimPosOffsetY")==pItem->m_idProp)//��עλ��Y����ƫ��ֵ
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
		{	//��˨��ע����
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
				{	//����������ʾ����
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
						lpInfo->m_strPropName.Format("��עλ��ƫ����");
						lpInfo->m_strPropHelp = "��עλ��ƫ������";
#endif
						pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
						pPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTINFO.DimPosOffset");
						if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
							pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
						//��עλ��X����
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position X coordinate. ";
#else
						lpInfo->m_strPropHelp = "��עλ��X���ꡣ";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetX");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
						//��עλ��Y����
						lpInfo = new CItemInfo();
						lpInfo->m_controlType=PIT_EDIT;
						lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
						lpInfo->m_strPropHelp = "dimension position Y coordinate.";
#else
						lpInfo->m_strPropHelp = "��עλ��Y���ꡣ";
#endif
						pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
						pSonPropItem->m_idProp=CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetY");
						if(pScriptDlg->GetPropValueStr(pFirstCmd,pSonPropItem->m_idProp,tem_str)>0)
							pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",tem_str);
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !pScriptDlg->IsPropEqual(pPropItem->m_idProp);
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetX")==pItem->m_idProp)//��עλ��X����ƫ��ֵ
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
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.fDimPosOffsetY")==pItem->m_idProp)//��עλ��Y����ƫ��ֵ
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
		{	//�����α�ע����
			if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fScale")==pItem->m_idProp)	//ָ�������α���
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
						{	//ͨ��ʰȡ���ע�������Σ���Ҫ�������ú�ͼ��
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
						{	//ͨ��ʰȡ���ע�������Σ�ִ�����ע�����������ظ�ԭ��ͼ��״̬
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
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fPickPtX")==pItem->m_idProp)	//������ʰȡ��X����
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fPickPtX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fPickPtY")==pItem->m_idProp)	//������ʰȡ��Y����
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fPickPtY=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fDimPosOffsetX")==pItem->m_idProp)//������λ��X����		
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fDimPosOffsetX=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.fDimPosOffsetY")==pItem->m_idProp)//������λ��Y����
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SIMPOLY)
						continue;
					(*ppCmd)->m_paraSimPoly.fDimPosOffsetY=atof(valueStr);
				}
			}
			//��������������� �������
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
				{	//�޸Ķ���λ�ú����¼��㼸������λ�ã�Ȼ���������������� wht 11-07-29
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
		{	//��עģ�ͳߴ�
			if(CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MODELSIZE)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimModelSize.fDimDistance;
					(*ppCmd)->m_paraDimModelSize.fDimDistance=atof(valueStr)*(1.0/pScriptDlg->m_pComponent->m_fScale);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimModelSize.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//ɾ���������ʵ���������и����� wht 11-06-20
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
						(*ppCmd)->Rerun();	//ɾ���������ʵ���������и����� wht 11-06-20
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
						(*ppCmd)->Rerun();	//ɾ���������ʵ���������и����� wht 11-06-20
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
							AfxMessageBox("����������������������Ч�ڵ�����");
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
		{	//ͼ��ߴ�
			if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimStartX")==pItem->m_idProp)//ͼ��ߴ��ע��ʼ��(�������ϵ��)
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
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimEndX")==pItem->m_idProp)	//ͼ��ߴ��ע��ʼ��(�������ϵ��)
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
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fDimPosX")==pItem->m_idProp)	//ͼ��ߴ��עλ��(�������ϵ��)
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
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.fScale")==pItem->m_idProp)	//�ߴ����
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_MAPSIZE)
						continue;
					(*ppCmd)->m_paraDimSize.fScale=atof(valueStr);
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("DIMSIZE.iDimDirect")==pItem->m_idProp)//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��
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
		{	//��ע�Ǹ�����׼��
			if(CDrawMapScriptDlg::GetPropID("DIM_JGZHUN.fSpace")==pItem->m_idProp)	
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN)
						continue;
					double fOldSpace=(*ppCmd)->m_paraDimJgZhun.fSpace;
					(*ppCmd)->m_paraDimJgZhun.fSpace=atof(valueStr);
					if(fabs(fOldSpace-(*ppCmd)->m_paraDimJgZhun.fSpace)>EPS2)
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
					if(valueStr.CompareNoCase("�ڲ�")==0)
#endif
						(*ppCmd)->m_paraDimJgZhun.bInside=TRUE;
					else 
						(*ppCmd)->m_paraDimJgZhun.bInside=FALSE;
					if((*ppCmd)->m_paraDimJgZhun.bInside!=bInside)
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
		{	//��ע��˨������
			if(CDrawMapScriptDlg::GetPropID("DIM_LSSPACE.fSpace")==pItem->m_idProp)	
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE)
						continue;
					double fOldSpace=(*ppCmd)->m_paraDimLsSpace.fSpace;
					(*ppCmd)->m_paraDimLsSpace.fSpace=atof(valueStr);
					if(fabs(fOldSpace-(*ppCmd)->m_paraDimLsSpace.fSpace)>EPS2)
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
					if(valueStr.CompareNoCase("�ڲ�")==0)
#endif
						(*ppCmd)->m_paraDimLsSpace.bInside=TRUE;
					else 
						(*ppCmd)->m_paraDimLsSpace.bInside=FALSE;
					if((*ppCmd)->m_paraDimLsSpace.bInside!=bInside)
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
					if(valueStr.CompareNoCase("��")==0)
#endif
						(*ppCmd)->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace=TRUE;
					else 
						(*ppCmd)->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace=FALSE;
					if((*ppCmd)->m_paraDimLsSpace.spaceDim.bMergeDimLsSpace!=bMergeDimLsSpace)
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
		{	//�Ŷ�����ע
			if(CDrawMapScriptDlg::GetPropID("FOOTNAIL_SPACE.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimFootNailSpace.fDimDistance;
					(*ppCmd)->m_paraDimFootNailSpace.fDimDistance=atof(valueStr);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimFootNailSpace.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
				}
			}
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
		{	//������ע
			if(CDrawMapScriptDlg::GetPropID("FILLPLATE_SPACE.fDimDistance")==pItem->m_idProp)
			{
				for(ppCmd=pScriptDlg->cmdList.GetFirst();ppCmd;ppCmd=pScriptDlg->cmdList.GetNext())
				{
					if((*ppCmd)->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE)
						continue;
					double fOldDist=(*ppCmd)->m_paraDimFillPlateSpace.fDimDistance;
					(*ppCmd)->m_paraDimFillPlateSpace.fDimDistance=atof(valueStr);
					if(fabs(fOldDist-(*ppCmd)->m_paraDimFillPlateSpace.fDimDistance)>EPS2)
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
						(*ppCmd)->Rerun();	//ɾ��ԭ��ʵ���������и����� wht 11-06-20
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
	if(pScriptDlg->m_pDrawing&&pScriptDlg->m_pDrawing->m_nSegI.iSeg>=0&&	//�ṹͼ
		pScriptDlg->IsSpreadLinePropGroup(iCurGroup))		//չ����
	{	//չ����
		if(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_hDatumPole")==pItem->m_idProp)
		{	//�ӽ�����ѡ��˼�
			pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
			pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
#ifdef AFX_TARG_ENU_ENGLISH
			pScriptDlg->m_arrCmdPickPrompt.Add("\nplease select an angle (choose center line of the angle)<Enter confirm>:\n");
#else
			pScriptDlg->m_arrCmdPickPrompt.Add("\n��ѡ��һ���Ǹ�(ѡ��Ǹ�����)<Enterȷ��>:\n");
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
			{	//�˼���˨������ע��Ϣ
				//�������������Ӧ��ʵ���������� wht 11-07-23
				SCOPE_STRU scope=pSonBoltOrFlInfoCmd->GetCADEntScope();
				ZoomAcadView(scope,20);
				pSonBoltOrFlInfoCmd->SetEntSetHighlight();
				//
				long startOddId=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset");
				long endOddId=CDrawMapScriptDlg::GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset");
				if((startOddId==pItem->m_idProp&&pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.bDimStartOdd)
					||(endOddId==pItem->m_idProp&&pSonBoltOrFlInfoCmd->m_paraBoltOrFlInfo.bDimEndOdd))
				{	//�˼���˨������Ϣ��ע
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
					CXhChar100 sPrompt("�������עλ��\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//���¹������λ��
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
						//���½���
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
					//�������������Ӧ��ʵ���������� wht 11-07-23
					SCOPE_STRU scope=pSonPartSpecCmd->GetCADEntScope();
					ZoomAcadView(scope,20);
					pSonPartSpecCmd->SetEntSetHighlight();
				}
				if(CDrawMapScriptDlg::GetPropID("PARTSPEC.MovePartSpec")==pItem->m_idProp)
				{	//����������ϸλ��
					pScriptDlg->HideWnd(TRUE);
					//
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//ѡ�񹹼���ϸ��Ӧ��ʵ�弯
					pCmd->GetEntSet(DRAGSET.drag_ent_set);
					//ѡ�������߶�Ӧ��ʵ�弯
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
						CXhChar100 sPrompt("������(%s)������ϸλ��\n",sPartNo);
#endif
						int nRetCode=DragEntSet(basepnt,sPrompt);
						if(nRetCode==RTNORM)
						{	//���¹�����ϸλ��
							double fOffsetX=basepnt.x-old_pos.x;
							double fOffsetY=basepnt.y-old_pos.y;
							pCmd->m_paraPartSpec.fBasePosOffsetX+=fOffsetX;
							pCmd->m_paraPartSpec.fBasePosOffsetY+=fOffsetY;
							pCmd->m_paraPartSpec.fDimPosOffsetX+=fOffsetX;
							pCmd->m_paraPartSpec.fDimPosOffsetY+=fOffsetY;
							//���½���
							actrTransactionManager->flushGraphics();
							acedUpdateDisplay();
						}
					}
				}
				else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.BasePosOffset")==pItem->m_idProp)
				{	//�ƶ����ݵ�λ��
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//ѡ�������߶�Ӧ��ʵ�弯
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
						CXhChar100 sPrompt("������ְ�(%s)������չ��λ��:\n",sPartNo);
#endif
#ifdef _ARX_2007
						if(acedGetPoint(end_pt,(ACHAR*)_bstr_t(sPrompt),start_pt)==RTNORM)
#else
							if(acedGetPoint(end_pt,sPrompt,start_pt)==RTNORM)
#endif
							{	//���¹������λ��
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
				{	//�ƶ����λ��
					pScriptDlg->HideWnd(TRUE);
					//
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					//TODO: ����pCmd����ΪCMD_DIM_BOLTORFL_INFO����2E2-SZ1����0x2db�˼���wjh-2013.12.15
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//ѡ����(�����)��Ӧ��ʵ�弯
					SCOPE_STRU scope;
					BOOL bDrawRefLine=pCmd->m_paraPartSpec.bDrawRefLine;
					if(pCmd->m_paraPartSpec.bDrawRefLine)
					{	//����������ʱ����Ϸ��޸ı��λ�ã��Ȱ���������ʱ�������ɣ�Ȼ�����λ�á�
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
					CXhChar100 sPrompt("��������%sλ��\n",sPartNo);
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//���±��λ��
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraPartSpec.fDimPosOffsetX+=fOffsetX;
						pCmd->m_paraPartSpec.fDimPosOffsetY+=fOffsetY;
						//���½���
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
					if(bDrawRefLine)
					{	//�ƶ�λ�ú��ٴ��������ɹ������ wht 11-07-18
						pCmd->m_paraPartSpec.bDrawRefLine=TRUE;
						pCmd->Rerun();
					}
				}
				else if(CDrawMapScriptDlg::GetPropID("PARTSPEC.GuigeOffset")==pItem->m_idProp)
				{	//�ƶ����λ��
					pScriptDlg->HideWnd(TRUE);	
					//
					DRAGSET.ClearEntSet();
					char sPartNo[16]="";
					if(pCmd->m_paraPartSpec.pObj==NULL)
						pCmd->m_paraPartSpec.pObj=(CLDSDbObject*)Ta.FromHandle(pCmd->m_paraPartSpec.hObj,TRUE);
					if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
						strcpy(sPartNo,((CLDSPart*)pCmd->m_paraPartSpec.pObj)->GetPartNo());
					//ѡ���Ŷ�Ӧ��ʵ�弯
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
							||sValue.Find("ǰ")>=0||sValue.Find("��")>=0)
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
					CXhChar100 sPrompt("������ְ�(%s)���ı�עλ��\n",sPartNo);
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//���¹������λ��
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraPartSpec.fGuigeOffsetX+=fOffsetX;
						pCmd->m_paraPartSpec.fGuigeOffsetY+=fOffsetY;
						//���½���
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)
		{
			if(CDrawMapScriptDlg::GetPropID("SIMPOLY.DimPosOffset")==pItem->m_idProp)
			{	//����������λ��
				DRAGSET.ClearEntSet();
				//ѡ���Ŷ�Ӧ��ʵ�弯
				int nEntNum=pCmd->GetEntSet(DRAGSET.drag_ent_set);
				if(nEntNum>0)
				{
					pScriptDlg->HideWnd(TRUE);	//���ش���
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please select position of similar polygon\n");
#else
					CXhChar100 sPrompt("��ѡ��������λ��\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//���������η�λ��
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraSimPoly.fDimPosOffsetX+=fOffsetX;
						pCmd->m_paraSimPoly.fDimPosOffsetY+=fOffsetY;
						//���½���
						actrTransactionManager->flushGraphics();
						acedUpdateDisplay();
					}
				}
			}
			else if(CDrawMapScriptDlg::GetPropID("SIMPOLY.vertexList")==pItem->m_idProp)
			{	//���������
				int nCount=pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum();
				if(nCount>=15)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("only 15 outline vertexts supported currently ");
					return FALSE;
				}
				if(nCount>0&&AfxMessageBox("sure to delete all outline vertexs and select again?(Y/N)",MB_YESNO)==IDNO)
#else
					AfxMessageBox("Ŀǰ�����֧��15��������!");
					return FALSE;
				}
				if(nCount>0&&AfxMessageBox("ȷʵҪɾ����������������ѡ����(Y/N)",MB_YESNO)==IDNO)
#endif
					return FALSE;
				//����ѡ����������������ĸ˼�����i�����i+1���˼��� ���һ�������һ���� wht 11-07-16
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
				pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
				pScriptDlg->m_nPickEntType=2;
				pScriptDlg->SelectEntObj(20);	//����һ���㹻��ĸ˼�����
			}
			//��������������� �������
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
					{	//ɾ����ǰ������
#ifdef AFX_TARG_ENU_ENGLISH
						if(AfxMessageBox("are you sure to delete this outline vertex?(Y/N)",MB_YESNO)==IDNO)
#else
						if(AfxMessageBox("��ȷ��Ҫɾ������������?(Y/N)",MB_YESNO)==IDNO)
#endif
							return FALSE;
						long prev_id=pItem->m_idProp;
						CString sPrePropName=pItem->m_lpNodeInfo->m_strPropName;
						pCmd->m_paraSimPoly.simPolyVertexList.DeleteCursor();
						pPropList->DeleteItemByPropId(pItem->m_idProp);
						//����������ID
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
						pPropList->HideInputCtrl();	//��������ؼ�
						return FALSE;
					}
					/*else if(CDrawMapScriptDlg::GetPropID(sKey2)==pItem->m_idProp
						||CDrawMapScriptDlg::GetPropID(sKey3)==pItem->m_idProp)
					{
						pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
						pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
						pScriptDlg->m_arrCmdPickPrompt.Add("\n��ѡ��һ���˼�(ѡ��˼�����)<Enterȷ��>:\n");
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
			{	//��ӽڵ�
				int nCount=pCmd->m_paraDimModelSize.listDimNodes.GetNodeNum();
				if(nCount>=49)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("49 nodes supported currently.");
#else
					AfxMessageBox("Ŀǰ�����֧��49���ڵ�!");
#endif
					return FALSE;
				}
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
				pScriptDlg->m_nPickEntType=1;	//ѡ��������չ���Ӧ�Ľڵ�
				pScriptDlg->SelectEntObj();
			}
			else if(pItem->m_pParent&&pItem->m_pParent->m_idProp==CDrawMapScriptDlg::GetPropID("DIMMODELSIZE.nodeSet"))
			{	//ɾ����ǰ���ڵĽڵ�
				int i=0;
				CDrawingCommand::DIMNODE* pDimNode=NULL;
				for(pDimNode=pCmd->m_paraDimModelSize.listDimNodes.GetFirst();pDimNode;
					pDimNode=pCmd->m_paraDimModelSize.listDimNodes.GetNext())
				{
					char sKey[50]="";
					sprintf(sKey,"DIMMODELSIZE.Node%d",i);
					int dd=CDrawMapScriptDlg::GetPropID(sKey);
					if(CDrawMapScriptDlg::GetPropID(sKey)==pItem->m_idProp)
					{	//ɾ����ǰ�ڵ�
#ifdef AFX_TARG_ENU_ENGLISH
						if(AfxMessageBox("are you sure to delete the wheel node?(Y/N)",MB_YESNO)==IDNO)
#else
						if(AfxMessageBox("��ȷ��Ҫɾ�����ֽڵ���?(Y/N)",MB_YESNO)==IDNO)
#endif
							return FALSE;
						long prev_id=pItem->m_idProp;
						CString sPrePropName=pItem->m_lpNodeInfo->m_strPropName;
						pCmd->m_paraDimModelSize.listDimNodes.DeleteCursor();
						//pCmd->m_paraDimModelSize.nodeSet.DeleteNode(pDimNode->hNode);
						pPropList->DeleteItemByPropId(pItem->m_idProp);
						//���½ڵ��Ӧ���ID
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
						pPropList->HideInputCtrl();	//��������ؼ�
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
			{	//����ͷ��ע
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
				CXhChar100 sPrompt("����������ͷ��עλ��\n");
#endif
				int nRetCode=DragEntSet(basepnt,sPrompt);
				if(nRetCode==RTNORM)
				{	//���¹������λ��
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
					//���½���
					actrTransactionManager->flushGraphics();
					acedUpdateDisplay();
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_SEGI)
		{	//�κű�ע
			if(CDrawMapScriptDlg::GetPropID("DIMSEGI.DimPos")==pItem->m_idProp)
			{	//����������λ��
				DRAGSET.ClearEntSet();
				//ѡ���Ŷ�Ӧ��ʵ�弯
				int nEntNum=pCmd->GetEntSet(DRAGSET.drag_ent_set);
				if(nEntNum>0)
				{
					pScriptDlg->HideWnd(TRUE);	//���ش���
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please select position of segment number\n");
#else
					CXhChar100 sPrompt("��ѡ��κ�λ��\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//���¶κ�λ��
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraDimSegI.fDimPosX+=fOffsetX;
						pCmd->m_paraDimSegI.fDimPosY+=fOffsetY;
						//���½���
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
				//�����Ǹֱ�עλ��
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

				pScriptDlg->HideWnd(TRUE);	//���ش���
				int type, track = 1;//��������϶�����
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
					double fDimTextHeight=sys.dim_map.fPartGuigeTextSize;//���ָ߶�
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

						//���½���
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
				{	//���½Ǹֱ�עλ��ƫ����
					pCmd->m_paraAngleJoint.fDimPosOffsetX=curPos.x-originPoint.x;
					pCmd->m_paraAngleJoint.fDimPosOffsetY=curPos.y-originPoint.y;
				}
			}
		}
#endif
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
		{
			if(CDrawMapScriptDlg::GetPropID("BOLTPAD.DimPosOffset")==pItem->m_idProp)
			{	//������Ȧ��עλ��
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
				const int nUnderlineLen=7;	//��Ȧ��ע�»��߳���
				const double fMargin=0.5;	//��ע�������»���֮��ļ�϶
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
				pScriptDlg->HideWnd(TRUE);	//���ش���
				int type, track = 1;//��������϶�����
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
				{	//���µ�Ȧ��עλ��ƫ����
					pCmd->m_paraBoltPad.fDimPosOffsetX=curPos.x-pad_pos.x;
					pCmd->m_paraBoltPad.fDimPosOffsetY=curPos.y-pad_pos.y;
				}
			}
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
		{
			if(CDrawMapScriptDlg::GetPropID("BOLTINFO.boltSet")==pItem->m_idProp)
			{	//�����˨
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
				pScriptDlg->m_nPickEntType=3;				//����ʵ��
				pScriptDlg->SelectEntObj(20);				//����һ���㹻�������
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.pDatumRod")==pItem->m_idProp)
			{	//�ӽ�����ѡ��˼�
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
				pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
#ifdef AFX_TARG_ENU_ENGLISH
				pScriptDlg->m_arrCmdPickPrompt.Add("\nplease select a rod(choose its center line)<Enter confirm>:\n");
#else
				pScriptDlg->m_arrCmdPickPrompt.Add("\n��ѡ��һ���˼�(ѡ��˼�����)<Enterȷ��>:\n");
#endif
				pScriptDlg->SelectEntObj();
			}
			else if(CDrawMapScriptDlg::GetPropID("BOLTINFO.DimPosOffset")==pItem->m_idProp)
			{	//������˨��Ϣ��עλ��
				DRAGSET.ClearEntSet();
				int nEntNum=pCmd->GetEntSet(DRAGSET.drag_ent_set);
				if(nEntNum>0)
				{
					pScriptDlg->HideWnd(TRUE);	//���ش���
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					f3dPoint basepnt;
					scope.centre(&basepnt.x,&basepnt.y,&basepnt.z);
					f3dPoint old_pos=basepnt;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 sPrompt("please select bolt's information dimension position\n");
#else
					CXhChar100 sPrompt("��ѡ����˨��Ϣ��עλ��\n");
#endif
					int nRetCode=DragEntSet(basepnt,sPrompt);
					if(nRetCode==RTNORM)
					{	//������˨��Ϣ��עλ��
						double fOffsetX=basepnt.x-old_pos.x;
						double fOffsetY=basepnt.y-old_pos.y;
						pCmd->m_paraBoltGroupInfo.fDimPosOffsetX+=fOffsetX;
						pCmd->m_paraBoltGroupInfo.fDimPosOffsetY+=fOffsetY;
						//���½���
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
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
				pScriptDlg->m_nPickEntType=3;				//����ʵ��
				pScriptDlg->SelectEntObj(20);				//����һ���㹻�������
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
			ZoomAcadView(scope,20);	//�����������Ӧ��ʵ��
			pScriptDlg->m_pCurCmd->SetEntSetHighlight();
		}
	}
	else if(pScriptDlg->m_pCurCmd&&pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
	{
		if(pItem->m_idProp>HASHTABLESIZE)
		{
			CLsGroupDimUnit::BOLTINFO *pBoltInfo=(CLsGroupDimUnit::BOLTINFO*)pItem->m_idProp;
			if(pBoltInfo&&pBoltInfo->hBolt>0x20)
			{	//������ʾ��ǰѡ����˨ wht 12-03-22
				CDrawingCommand *pCmd=((CStruView*)pScriptDlg->m_pCurCmd->m_pDrawingCom)->GetDrawingPartCmdFromHandle(pBoltInfo->hBolt);
				if(pCmd)
				{
					CDrawingCommand::ReleaseHighlightEnts();
					//�����������Ӧ��ʵ��
					SCOPE_STRU scope=pCmd->GetCADEntScope();
					ZoomAcadView(scope,20);
					pCmd->SetEntSetHighlight();
				}
			}
		}
		else //������ʾ��ǰ�����Ӧ��ʵ�� 
		{	 
			SCOPE_STRU scope=pScriptDlg->m_pCurCmd->GetCADEntScope();
			ZoomAcadView(scope,20);	//�����������Ӧ��ʵ��
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
	{	//չ����
		CDrawingSpreadLine *pSpeardLine=(CDrawingSpreadLine*)pItem->m_idProp;
		if(pSpeardLine==NULL)
			return FALSE;
		if(iSubItem==2)
		{	//�޸�չ��������
			pSpeardLine->m_sLineName.Printf("%s",valueStr);
			pScriptDlg->m_propList.SetItemPropValue(CDrawMapScriptDlg::GetPropID("SPREAD_LINE.m_sLineName"),valueStr);
		}
	}
	return TRUE;
}

static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{	//ѡ������仯�����������
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pListCtrl->GetParent();
	if(!pScriptDlg->m_bFireListItemChange)
		return FALSE;	//δ����ItemChange
	if(pItem==NULL||pScriptDlg==NULL||pNMListView==NULL)
		return FALSE;
	int iCurGroup=pScriptDlg->GetCurCmdGroup();
	if(pScriptDlg->IsSpreadLinePropGroup(iCurGroup))	//��ȡ��ǰѡ�������չ����
		pScriptDlg->m_pSpreadLine=(CDrawingSpreadLine*)pItem->m_idProp;	
	else				//��ȡ��ǰѡ�����������
	{
		CDrawingCommand::ReleaseHighlightEnts();
		pScriptDlg->m_pCurCmd=(CDrawingCommand*)pItem->m_idProp;
		pScriptDlg->cmdList.Empty();
		if(pScriptDlg->m_pCurCmd)
			pScriptDlg->cmdList.append(pScriptDlg->m_pCurCmd);
	}
	pScriptDlg->DisplayPropertyList(iCurGroup);//����������
	if(pScriptDlg->m_pCurCmd)
	{	
		//��ǰ����Ϊģ�ͳߴ��עʱ�л�������չ����ʾģʽ���ڲ鿴�ڵ� wht 11-07-13
		/*Ŀǰ��Ϊû�б�Ҫ�ڴ�ʱ��ʾ�ڵ㣬ֻ��Ҫ�ڲ�׽��ʱ��ʾ�ڵ㼴�ɷ�Ҫ��ʾ���ò�������ͼ����ʾ�� wjh-2011.8.31
		if(pScriptDlg->m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE&&
			!pScriptDlg->m_bPdmodeIs34)	//�����ǰ״̬Ϊ��ʾX �Ͳ����ظ�ִ�����´��� wht 11-07-27
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"34",RTNONE);//��ʾ�� X
			//acedCommand(RTSTR,"REGEN",RTNONE);
			pScriptDlg->m_bPdmodeIs34=TRUE;	//������չ����ʾģʽΪ34 wht 11-07-15
		}
		else if(pScriptDlg->m_pCurCmd->m_eCmdType!=CMD_DIM_MODELSIZE
			&&pScriptDlg->m_bPdmodeIs34)
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//��ʾ�� .
			//acedCommand(RTSTR,"REGEN",RTNONE);
			pScriptDlg->m_bPdmodeIs34=FALSE;
		}*/
		//�����������Ӧ��ʵ��
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
//���˫���Լ�������ť���øûص�����
static BOOL FireLButtonDblclk(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,int iSubItem)
{	//˫��ĳ���һ�к���ĵ�ǰ������״̬���������ɾ���ϵ�
	CDrawMapScriptDlg *pScriptDlg=(CDrawMapScriptDlg*)pListCtrl->GetParent();
	if(pItem&&iSubItem==0&&pItem->m_lpNodeInfo)
	{
		CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
		if(pCmd&&(pCmd->m_stateCmd&CMD_STATE_BREAKPOINT))
		{	//�Ƴ��ϵ�
			pCmd->m_stateCmd^=CMD_STATE_BREAKPOINT;
			pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);
		}
		else if(pCmd)
		{	//��Ӷϵ�
			pCmd->m_stateCmd|=CMD_STATE_BREAKPOINT;
			pItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);
		}
		//˫����һ��ʱ�ᴥ��OnItemChanged,����FireItemChanged��ʱ���ɸ���������
	}
	else if(pItem&&iSubItem==3&&pItem->m_lpNodeInfo)
	{
		BOOL bStartUpAddSimPoly=FALSE;	//�����������������
		int iCurGroup=pScriptDlg->GetCurCmdGroup();
		CString sOper=pItem->m_lpNodeInfo->GetSubItemText(iSubItem);
#ifdef AFX_TARG_ENU_ENGLISH
		if(iCurGroup==GROUP_DIM_SIMPOLY-1&&sOper.CompareNoCase("Add")==0)
#else
		if(iCurGroup==GROUP_DIM_SIMPOLY-1&&sOper.CompareNoCase("���")==0)
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
					pScriptDlg->m_pCurCmd->m_paraSimPoly.nSimPolyCmdType=3;	//3.δָ��������������
					//���һ������
					char sCmdId[100]="",sCmdText[200]="";
					pScriptDlg->m_pCurCmd->GetCmdInfo(sCmdId,sCmdText);
					CListCtrlItemInfo *lpRoot=new CListCtrlItemInfo();
					lpRoot->SetImage(pScriptDlg->m_pCurCmd->m_stateCmd);	//��������״̬���ò�ͬ��ͼ��
					lpRoot->AddSubItemText(_T(""),TRUE);
					lpRoot->AddSubItemText(sCmdId,TRUE);
					lpRoot->AddSubItemText(sCmdText,TRUE);
					if(pScriptDlg->m_pCurCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
						lpRoot->AddSubItemText(_T("��"),TRUE);
#else
						lpRoot->AddSubItemText(_T("��"),TRUE);
#endif
					else
						lpRoot->AddSubItemText(_T("  "),TRUE);//��
					CSuperGridCtrl::CTreeItem *pTreeItem=pListCtrl->InsertRootItem(lpRoot);
					pTreeItem->m_idProp=(long)pScriptDlg->m_pCurCmd;
					pTreeItem->m_dwPropGroup=pItem->m_dwPropGroup;
					pListCtrl->SelectItem(pTreeItem);
					bStartUpAddSimPoly=TRUE;	//������������ε����� wht 11-07-16
				}
				else if(pScriptDlg->m_pCurCmd->m_paraSimPoly.nSimPolyCmdType==0)
				{	//����ͨ��ѡ��ʰȡ��������������� wht 11-07-25
					pScriptDlg->HideWnd(TRUE);
					ATOM_LIST<CDrawingCommand*> cmdList;
					AddDimSimPolyCmd(pScriptDlg->m_pComponent,&cmdList);
					CDrawingCommand **ppCmd=NULL;
					for(ppCmd=cmdList.GetFirst();ppCmd;ppCmd=cmdList.GetNext())
					{	//���һ������
						char sCmdId[100]="",sCmdText[200]="";
						(*ppCmd)->GetCmdInfo(sCmdId,sCmdText);
						CListCtrlItemInfo *lpRoot=new CListCtrlItemInfo();
						lpRoot->SetImage((*ppCmd)->m_stateCmd);	//��������״̬���ò�ͬ��ͼ��
						lpRoot->AddSubItemText(_T(""),TRUE);
						lpRoot->AddSubItemText(sCmdId,TRUE);
						lpRoot->AddSubItemText(sCmdText,TRUE);
						if((*ppCmd)->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
							lpRoot->AddSubItemText(_T("��"),TRUE);
#else
							lpRoot->AddSubItemText(_T("��"),TRUE);
#endif
						else
							lpRoot->AddSubItemText(_T("  "),TRUE);//��
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
			{	//���̸�Ϊ��
				pCmd->m_bExecute=FALSE;
				pItem->m_lpNodeInfo->SetSubItemText(iSubItem,"  ",TRUE); //��
				pListCtrl->SetSubItemText(pItem,iSubItem,CString("  ")); //��
				pCmd->EmptyEntList();
			}
			else if(pCmd&&!pCmd->m_bExecute)
			{	//������Ϊ��
				pCmd->m_bExecute=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				pItem->m_lpNodeInfo->SetSubItemText(iSubItem,"��",TRUE);
				pListCtrl->SetSubItemText(pItem,iSubItem,CString("��"));
#else
				pItem->m_lpNodeInfo->SetSubItemText(iSubItem,"��",TRUE);
				pListCtrl->SetSubItemText(pItem,iSubItem,CString("��"));
#endif
				pCmd->Rerun();
			}
		}
		pScriptDlg->DisplayPropertyList(iCurGroup);	//����������
		if(bStartUpAddSimPoly)
		{	//����ѡ����������������ĸ˼�����i�����i+1���˼��� ���һ�������һ���� wht 11-07-16
			CPropTreeItem *pItem=NULL;
			pItem=pScriptDlg->m_propList.FindItemByPropId(CDrawMapScriptDlg::GetPropID("SIMPOLY.vertexList"),NULL);
			if(pItem)
			{
				pScriptDlg->m_idEventProp=pItem->m_idProp;	//��¼�����¼�������ID
				pScriptDlg->m_arrCmdPickPrompt.RemoveAll();
				pScriptDlg->m_nPickEntType=2;
				pScriptDlg->SelectEntObj(20);	//����һ���㹻��ĸ˼�����
			}
		}
	}
	return TRUE;
}

//�����Ҽ��˵�
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
	pMenu->AppendMenu(MF_STRING,ID_ENABLE_ALL_ITEM,"����������");
	pMenu->AppendMenu(MF_STRING,ID_DISABLE_ALL_ITEM,"����������");
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
			pCmd->EmptyEntList();	//ɾ�������Ӧ��CADʵ��
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

void CDrawMapScriptDlg::InitPropHashtable()	//��ʼ�����Թ�ϣ��
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	//
	propHashtable.SetValueAt("CmdPara",id++);
	propHashtable.SetValueAt("m_eCmdType",id++);		//��������
	propHashtable.SetValueAt("m_stateCmd",id++);	//����״̬
	propHashtable.SetValueAt("m_bExecute",id++);	//�Ƿ�ִ�и�����
	
	propHashtable.SetValueAt("DrawPara",id++);	//���ƹ�������
	//�������Ʋ���
	propHashtable.SetValueAt("PART.pPart",id++);	//�ű���¼��������
	propHashtable.SetValueAt("PART.PoleAxisLineOutSpreadLen",id++);	//�˼��������쳤��
	propHashtable.SetValueAt("PART.fPoleAxisLineStartOutSpreadLen",id++);	//�˼�����ʼ�����쳤��
	propHashtable.SetValueAt("PART.fPoleAxisLineEndOutSpreadLen",id++);		//�˼������ն����쳤��
	propHashtable.SetValueAt("PART.iResidePlane",id++);	//չ����
	propHashtable.SetValueAt("PART.cDrawFlag",id++);	//���������� 'X'.X���� 'Y'.Y����,'Z'.Z���� �Զ��ж�
	propHashtable.SetValueAt("PART.angle.bDrawAxisLine",id++);		//�Ƿ���ƽǸ�����
	propHashtable.SetValueAt("PART.angle.extendStyle",id++);		//֫����չ��ʽ
	propHashtable.SetValueAt("PART.angle.nXWingDatumAxisG",id++);	//X֫��׼�ľ�
	propHashtable.SetValueAt("PART.angle.nYWingDatumAxisG",id++);	//Y֫��׼�ľ�
	propHashtable.SetValueAt("PART.bolt.bDrawSideBoltLine",id++);	//Y֫��׼�ľ�
	propHashtable.SetValueAt("PART.bolt.bDimHoleDiameter",id++);	//�Ƿ��ע�����ֱ��
	//���Ƹ˼�����ͼ
	propHashtable.SetValueAt("LINEPART.pLinePart",id++);	//�����˼�
	propHashtable.SetValueAt("LINEPART.bSum",id++);			//��ͼ
	propHashtable.SetValueAt("LINEPART.bDimByAnalysisMode",id++);	//��ѧ����ģʽ
	//�����Ʋ��� ��һ��ָ�����λ�ú��¼���λ��������꣬�ٴλ��Ʊ��ʱ��ֱ��ʹ��
	propHashtable.SetValueAt("TABLE.nLocatePos",id++);			//0.���� 1.���� 2.���� 4.����
	propHashtable.SetValueAt("TABLE.bSelectPos",id++);			//ѡ����λ��
	propHashtable.SetValueAt("TABLE.bDisplayDlg",id++);			//��ʾ�Ի���
	propHashtable.SetValueAt("TABLE.DimPos",id++);				//���λ��
	propHashtable.SetValueAt("TABLE.fDimPosX",id++);			//���λ��X����		
	propHashtable.SetValueAt("TABLE.fDimPosY",id++);			//���λ��Y����	
	propHashtable.SetValueAt("TABLE.iTableType",id++);			//�������
	//������ϸ��ע����
	propHashtable.SetValueAt("PublicPara",id++);				//��������			
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.pObj",id++);				//��ǰ��ע����
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.iDimPosType",id++);		//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)  2.ѡ��λ��
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.bDimGuige",id++);			//��ע�������
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.bDimJgLength",id++);		//��ע�Ǹֳ���
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.bDrawRefLine",id++);		//�Ƿ����������
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.fPoleDimPosCoef",id++);	//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
	propHashtable.SetValueAt("PUBLIC:PARTSPEC.iMirMsg",id++);			//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	//
	propHashtable.SetValueAt("PARTSPEC.m_bExecute",id++);	//�Ƿ�ִ�и�����
	propHashtable.SetValueAt("PARTSPEC.pObj",id++);				//��ǰ��ע����
	propHashtable.SetValueAt("PARTSPEC.BasePosOffset",id++);	//���ݵ�λ��ƫ����
	propHashtable.SetValueAt("PARTSPEC.fBasePosOffsetX",id++);	//���ݵ�λ��X����ƫ��ֵ
	propHashtable.SetValueAt("PARTSPEC.fBasePosOffsetY",id++);	//���ݵ�λ��Y����ƫ��ֵ
	propHashtable.SetValueAt("PARTSPEC.DimPosOffset",id++);		//��עλ��ƫ����
	propHashtable.SetValueAt("PARTSPEC.fDimPosOffsetX",id++);	//��עλ��X����ƫ��ֵ
	propHashtable.SetValueAt("PARTSPEC.fDimPosOffsetY",id++);	//��עλ��Y����ƫ��ֵ
	propHashtable.SetValueAt("PARTSPEC.GuigeOffset",id++);		//����עλ��ƫ��ֵ
	propHashtable.SetValueAt("PARTSPEC.fGuigeOffsetX",id++);	//����עλ��X����ƫ��ֵ
	propHashtable.SetValueAt("PARTSPEC.fGuigeOffsetY",id++);	//����עλ��Y����ƫ��ֵ
	propHashtable.SetValueAt("PARTSPEC.MovePartSpec",id++);		//�ƶ�������ϸ
	propHashtable.SetValueAt("PARTSPEC.iDimPosType",id++);		//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)  2.ѡ��λ��
	propHashtable.SetValueAt("PARTSPEC.bDimPoleSpec",id++);		//��ע�˼���ϸ
	propHashtable.SetValueAt("PARTSPEC.bDrawRefLine",id++);		//�Ƿ����������
	propHashtable.SetValueAt("PARTSPEC.fPoleDimPosCoef",id++);	//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
	propHashtable.SetValueAt("PARTSPEC.iMirMsg",id++);			//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
	//�Ǹֽ�ͷ��Ϣ
	propHashtable.SetValueAt("ANGLEJOINT.m_bExecute",id++);		//�Ƿ�ִ�и�����
	propHashtable.SetValueAt("ANGLEJOINT.pObj",id++);			//��ǰ��ע����
	propHashtable.SetValueAt("ANGLEJOINT.iDimPosType",id++);	//0.�Զ�����λ�� 1.ָ��λ��
	propHashtable.SetValueAt("ANGLEJOINT.DimPosOffset",id++);	//��עλ��ƫ��ֵ
	propHashtable.SetValueAt("ANGLEJOINT.fDimPosOffsetX",id++);	//��עλ��X����ƫ��ֵ
	propHashtable.SetValueAt("ANGLEJOINT.fDimPosOffsetY",id++);	//��עλ��Y����ƫ��ֵ
	//��ע�κ�
	propHashtable.SetValueAt("DIMSEGI.bSelectPos",id++);//ѡ��λ��
	propHashtable.SetValueAt("DIMSEGI.nSegI",id++);		//�����κ�
	propHashtable.SetValueAt("DIMSEGI.DimPos",id++);	//��עλ��
	propHashtable.SetValueAt("DIMSEGI.fDimPosX",id++);	//��עλ��X����
	propHashtable.SetValueAt("DIMSEGI.fDimPosY",id++);	//��עλ��Y����
	//����ͷ��ע����
	propHashtable.SetValueAt("ODDMENT.pLinePart",id++);			//����ͷ�����˼�
	propHashtable.SetValueAt("ODDMENT.iDimPosType",id++);		//0.�Զ����� 1.ָ��λ��
	propHashtable.SetValueAt("ODDMENT.StartOddPosOffset",id++);
	propHashtable.SetValueAt("ODDMENT.fStartOddPosOffsetX",id++);
	propHashtable.SetValueAt("ODDMENT.fStartOddPosOffsetY",id++);
	propHashtable.SetValueAt("ODDMENT.EndOddPosOffset",id++);
	propHashtable.SetValueAt("ODDMENT.fEndOddPosOffsetX",id++);
	propHashtable.SetValueAt("ODDMENT.fEndOddPosOffsetY",id++);
	//�Ŷ�����ע����
	propHashtable.SetValueAt("FOOTNAIL_SPACE.pLinePart",id++);	//�Ŷ��������˼�
	propHashtable.SetValueAt("FOOTNAIL_SPACE.fDimDistance",id++);//��ע�����ע��ʼ��ļ��
	propHashtable.SetValueAt("FOOTNAIL_SPACE.fSpace",id++);		//��ע��ʼ����Ŷ����ڸ˼����Ե�ļ��
	//������ע����
	propHashtable.SetValueAt("FILLPLATE_SPACE.pLinePart",id++);	//���������˼�
	propHashtable.SetValueAt("FILLPLATE_SPACE.fDimDistance",id++);//��ע�����ע��ʼ��ļ��
	propHashtable.SetValueAt("FILLPLATE_SPACE.fSpace",id++);		//��ע��ʼ����Ŷ����ڸ˼����Ե�ļ��
	//��Ȧ��ע����		
	propHashtable.SetValueAt("BOLTPAD.pBolt",id++);				//��˨��Ȧ������˨
	propHashtable.SetValueAt("BOLTPAD.iDimPosType",id++);		//0.�Զ����� 1.ָ��λ��
	propHashtable.SetValueAt("BOLTPAD.DimPosOffset",id++);		//��עλ��ƫ��ֵ
	propHashtable.SetValueAt("BOLTPAD.fDimPosOffsetX",id++);	//��עλ��X����ƫ��ֵ
	propHashtable.SetValueAt("BOLTPAD.fDimPosOffsetY",id++);	//��עλ��Y����ƫ��ֵ
	propHashtable.SetValueAt("BOLTPAD.sSpec",id++);             //��˨��Ȧ���
	//��˨��ע����		
	propHashtable.SetValueAt("BOLTINFO.pBolt",id++);			//��˨��ע��Ϣ����˨
	propHashtable.SetValueAt("BOLTINFO.pDatumRod",id++);		//��˨��ע��Ϣ�����˼�
	propHashtable.SetValueAt("BOLTINFO.iDimPosType",id++);		//0.�Զ����� 1.ָ��λ��
	propHashtable.SetValueAt("BOLTINFO.DimPosOffset",id++);		//��עλ��ƫ��ֵ
	propHashtable.SetValueAt("BOLTINFO.fDimPosOffsetX",id++);	//��עλ��X����ƫ��ֵ
	propHashtable.SetValueAt("BOLTINFO.fDimPosOffsetY",id++);	//��עλ��Y����ƫ��ֵ
	propHashtable.SetValueAt("BOLTINFO.boltSet",id++);			//��˨����
	propHashtable.SetValueAt("BOLTINFO.sLabel",id++);            //��ע����
	for(int i=0;i<PROPID_SETSIZE;i++)
	{
		char sKey[100]="";
		sprintf(sKey,"BOLTINFO.pBolt%d",i);
		propHashtable.SetValueAt(sKey,id++);
	}
	//�����α�ע����
	propHashtable.SetValueAt("SIMPOLY.fScale",id++);			//ָ�������α���
	propHashtable.SetValueAt("SIMPOLY.PickPt",id++);			//������ʰȡ��
	propHashtable.SetValueAt("SIMPOLY.fPickPtX",id++);			//������ʰȡ��X����
	propHashtable.SetValueAt("SIMPOLY.fPickPtY",id++);			//������ʰȡ��Y����
	propHashtable.SetValueAt("SIMPOLY.DimPosOffset",id++);		//������λ��
	propHashtable.SetValueAt("SIMPOLY.fDimPosOffsetX",id++);	//������λ��X����		
	propHashtable.SetValueAt("SIMPOLY.fDimPosOffsetY",id++);	//������λ��Y����
	propHashtable.SetValueAt("SIMPOLY.vertexList",id++);		//�������������б�
	propHashtable.SetValueAt("SIMPOLY.m_nSimPolyAddType",id++);	//��������ӱ�ע��ʽ 0.ѡ�������� 1.ѡ��ʰȡ��
	for(i=1;i<16;i++)
	{
		char sKey[100]="";
		sprintf(sKey,"SIMPOLY.vertex%d",i);
		propHashtable.SetValueAt(sKey,id++);	//������������ǰһ���˼�
		sprintf(sKey,"SIMPOLY.vertex%d.hPrevPole",i);
		propHashtable.SetValueAt(sKey,id++);	//������������ǰһ���˼�
		sprintf(sKey,"SIMPOLY.vertex%d.hNextPole",i);
		propHashtable.SetValueAt(sKey,id++);	//��������������һ���˼�
		sprintf(sKey,"SIMPOLY.vertex%d.x",i);
		propHashtable.SetValueAt(sKey,id++);	//X����
		sprintf(sKey,"SIMPOLY.vertex%d.y",i);
		propHashtable.SetValueAt(sKey,id++);	//Y����
	}
	//��עģ�ͳߴ�
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
	//ͼ��ߴ�
	propHashtable.SetValueAt("DIMSIZE.DimStart",id++);		//��ʼ��
	propHashtable.SetValueAt("DIMSIZE.fDimStartX",id++);	//ͼ��ߴ��ע��ʼ��(�������ϵ��)
	propHashtable.SetValueAt("DIMSIZE.fDimStartY",id++);
	propHashtable.SetValueAt("DIMSIZE.DimEnd",id++);		//��ֹ��
	propHashtable.SetValueAt("DIMSIZE.fDimEndX",id++);		//ͼ��ߴ��ע��ʼ��(�������ϵ��)
	propHashtable.SetValueAt("DIMSIZE.fDimEndY",id++);
	propHashtable.SetValueAt("DIMSIZE.DimPos",id++);		//��עλ��
	propHashtable.SetValueAt("DIMSIZE.fDimPosX",id++);		//ͼ��ߴ��עλ��(�������ϵ��)
	propHashtable.SetValueAt("DIMSIZE.fDimPosY",id++);
	propHashtable.SetValueAt("DIMSIZE.fScale",id++);		//�ߴ����
	propHashtable.SetValueAt("DIMSIZE.iDimDirect",id++);	//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��
	//��ע�Ǹ�����׼��
	propHashtable.SetValueAt("DIM_JGZHUN.pLineAngle",id++);		//��Ҫ��ע����׼�ݵĽǸ�
	propHashtable.SetValueAt("DIM_JGZHUN.DimPosOffset",id++);	//��עλ��ƫ��ֵ
	propHashtable.SetValueAt("DIM_JGZHUN.fSpace",id++);			//��ע��ʼ������˨֮��ľ���
	propHashtable.SetValueAt("DIM_JGZHUN.bInside",id++);		//��ע����
	propHashtable.SetValueAt("DIM_JGZHUN.fDimDistance",id++);	//��עλ�����ע��ʼ��֮��ľ���
	//��ע��˨������
	propHashtable.SetValueAt("DIM_LSSPACE.pPlate",id++);			//��Ҫ��ע������˨���ĸְ��Ǹ�
	propHashtable.SetValueAt("DIM_LSSPACE.DimPosOffset",id++);	//��עλ��ƫ��ֵ
	propHashtable.SetValueAt("DIM_LSSPACE.fSpace",id++);		//��ע��ʼ������˨֮��ľ���
	propHashtable.SetValueAt("DIM_LSSPACE.bInside",id++);		//��ע����
	propHashtable.SetValueAt("DIM_LSSPACE.fDimDistance",id++);	//��עλ�����ע��ʼ��֮��ľ���
	propHashtable.SetValueAt("DIM_LSSPACE.hStartLs",id++);		//ʼ����˨���
	propHashtable.SetValueAt("DIM_LSSPACE.hEndLs",id++);		//�ն���˨���
	propHashtable.SetValueAt("DIM_LSSPACE.nLsSpaceNum",id++);		//��������˨�����
	propHashtable.SetValueAt("DIM_LSSPACE.bMergeDimLsSpace",id++);	//�ϲ���ע������˨���
	//չ����
	propHashtable.SetValueAt("SPREAD_LINE",id++);				//չ����
	propHashtable.SetValueAt("SPREAD_LINE.m_sLineName",id++);	//չ��������
	propHashtable.SetValueAt("SPREAD_LINE.m_iDefType",id++);	//0.ָ���˼� 1.ֱ��ָ��
	propHashtable.SetValueAt("SPREAD_LINE.m_hDatumPole",id++);	//��׼�˼� ��׼�˼����Ϊ0ʱ��ʾΪ�Զ���
	propHashtable.SetValueAt("SPREAD_LINE.m_iCurWing",id++);	//��׼�˼�Ϊ�Ǹ�ʱ���ø�����
	propHashtable.SetValueAt("SPREAD_LINE.m_fOffsetDist",id++);	//ƫ�ƾ���
	propHashtable.SetValueAt("SPREAD_LINE.startPt",id++);		//չ�����������
	propHashtable.SetValueAt("SPREAD_LINE.startPt.x",id++);		//չ�������X����
	propHashtable.SetValueAt("SPREAD_LINE.startPt.y",id++);		//չ�������Y����
	propHashtable.SetValueAt("SPREAD_LINE.startPt.z",id++);		//չ�������Y����
	propHashtable.SetValueAt("SPREAD_LINE.endPt",id++);			//չ�����յ�����
	propHashtable.SetValueAt("SPREAD_LINE.endPt.x",id++);		//չ�����յ�X����
	propHashtable.SetValueAt("SPREAD_LINE.endPt.y",id++);		//չ�����յ�Y����
	propHashtable.SetValueAt("SPREAD_LINE.endPt.z",id++);		//չ�����յ�Y����
	//�˼���˨������Ϣ��ע����
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO",id++);
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.pLinePart",id++);		
	propHashtable.SetValueAt("ROD_BOLTORFL_INFO.iDimPosType",id++);		//0.�Զ����� 1.ָ��λ��
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
	//��ʼ�������б��Լ�����ID
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	if(!m_bInernalStart)	//�ڲ�����
	{
		m_propList.SetDividerScale(0.62);//���÷ָ����
		//�����������ص�����
		m_propList.SetModifyValueFunc(ModifyItemValue);
		m_propList.SetButtonClickFunc(ButtonClickItem);
		m_propList.SetPropHelpPromptFunc(FirePropHelpPrompt);
		m_propList.SetModifyStatusFunc(ModifyItemStatus);
		InitPropHashtable();	//��ʼ�����Թ�ϣ��
	}
	//
	long col_wide_arr[4]={25,65,160,45};
	m_listCmdSuperCtrl.InitListCtrl(col_wide_arr);
	m_listCmdSuperCtrl.m_hPromptWnd=GetDlgItem(IDC_E_CMD_HELP_STR)->GetSafeHwnd();
	//���ò˵��ص�����
	m_listCmdSuperCtrl.SetContextMenuFunc(FireContextMenu);
	//���ûص����� ��ǰѡ������仯ʱ����
	m_listCmdSuperCtrl.SetItemChangedFunc(FireItemChanged);
	//����������˫���ص�����
	m_listCmdSuperCtrl.SetLButtonDblclkFunc(FireLButtonDblclk);
	m_listCmdSuperCtrl.SetButtonClickFunc(FireLButtonDblclk);
	//����ֵ�޸ĺ���
	m_listCmdSuperCtrl.SetModifyValueFunc(FireModifyCellValue);
	//����ɾ������
	m_listCmdSuperCtrl.SetDeleteItemFunc(FireDeleteItem);
	//-1��ʾ��ͼ������
	m_listCmdSuperCtrl.InitImageList(IDR_BAR_SCRIPT);
	//
	long col_wide_arr1[7]={50,55,50,40,40,25,40};
	m_listCmdCommonCtrl.InitListCtrl(col_wide_arr1);
	m_listCmdCommonCtrl.m_bEnableSortItems=TRUE;	//֧������
	//���õ�һ��Ϊ��ʮ������������
	m_listCmdCommonCtrl.SetColumnSortType(0,SORTTYPE_HEX_NUM);	
	//���õڶ���Ϊ��ʮ������������
	m_listCmdCommonCtrl.SetColumnSortType(1,SORTTYPE_HEX_NUM);	
	//���õ�����Ϊ����������
	m_listCmdCommonCtrl.SetColumnSortType(2,SORTTYPE_PARTNO);
	//���õ�����Ϊ����������
	m_listCmdCommonCtrl.SetColumnSortType(6,SORTTYPE_INTEGER);
	//��ʼ��CTabCtrl
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
		m_tabCmdType.InsertItem(0,"���ƹ���");
		m_tabCmdType.InsertItem(1,"�ߴ��ע");
		if(pLineView->m_bAnalysisMode)	
		{	//˾��ͼҲ��Ҫ�κű�ע wht 11-07-15
			m_tabCmdType.InsertItem(2,"������ϸ");
			m_tabCmdType.InsertItem(3,"�κű�ע");
		}
		else//��ͼ
			m_tabCmdType.InsertItem(2,"�κű�ע");
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
			m_tabCmdType.InsertItem(0,"���ϻ��ܱ�");
		else 
			m_tabCmdType.InsertItem(0,"��ϸ��");
#endif
	}
	else if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_STRUVIEW)
	{	//�ṹͼ
#ifdef AFX_TARG_ENU_ENGLISH
		m_tabCmdType.InsertItem(GROUP_DRAW_PART-1,"Draw Parts");
		m_tabCmdType.InsertItem(GROUP_DIM_SIMPOLY-1,"similar polygon");
		m_tabCmdType.InsertItem(GROUP_DIM_ODDMENT-1,"ends");
#else
		m_tabCmdType.InsertItem(GROUP_DRAW_PART-1,"���ƹ���");
		m_tabCmdType.InsertItem(GROUP_DIM_SIMPOLY-1,"������");
		m_tabCmdType.InsertItem(GROUP_DIM_ODDMENT-1,"����ͷ");
#endif
		//m_tabCmdType.InsertItem(GROUP_SPREADLINE-1,"չ����");
	}
	//��ɶ���ѡ�������ʾ�����б� wht 12-03-22
	if(m_bInernalStart)	//�ڲ�����
		FinishSelectObjOper();		//���ѡ�����ĺ�������
	//��ʼ�������б�
	if(m_listCmdSuperCtrl.GetCount()<=0)
		DisplaySuperCtrlCmdList();
	if(m_listCmdCommonCtrl.GetItemCount()<=0)
		DisplayCommonCtrlCmdList(m_iOldGroup);
	if(m_iOldGroup>=0)
	{	
		m_tabCmdType.SetCurSel(m_iOldGroup);//ѡ���ж�ǰ��������
		RefreshCommandList(m_iOldGroup);	//ˢ�������б�
	}
	if(m_listCmdCommonCtrl.GetSortColunmnIndex()!=-1)	
	{	//��������֮ǰ������˳����ʾ�б���
		int iSubItem=m_listCmdCommonCtrl.GetSortColunmnIndex();
		m_listCmdCommonCtrl.SortItemsBySpecSubItem(iSubItem);
	}
	if(m_pCurCmd)
	{	//��ǰ�����в�Ϊ��ʱ��ѡ������ǰ��������
		if(!m_bSuperListCtrlInUsing)
		{	//�����Ի����ѡ��ָ����
			int i=0,n=m_listCmdCommonCtrl.GetItemCount();
			for(i=0;i<n;i++)
			{
				CDrawingCommand *pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(i);
				if(pCmd&&pCmd==m_pCurCmd)
				{
					m_listCmdCommonCtrl.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
					m_listCmdCommonCtrl.EnsureVisible(i,FALSE);	//ȷ��ѡ����ɼ� wht 11-07-25
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
					continue;	//�����ڵ�ǰ����
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
	//��ʾ����������
	DisplayPropertyList();
	//�ƶ����ڵ����Ͻ�
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

//����չ����λ��
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

//���ѡ�����ĺ�������
void CDrawMapScriptDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	if(m_iBreakType!=1)
		return;
	//����ѡ��ʵ���ڲ�����
	CAD_SCREEN_ENT *pCADEnt=resultList.GetFirst();
	CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
	if(pCADEnt&&pItem&&pItem->m_idProp==GetPropID("SPREAD_LINE.m_hDatumPole"))
	{
		m_propList.SetFocus();
		m_propList.SetCurSel(pItem->m_iIndex);	//ѡ��ָ������
		if(pCADEnt->m_pObj
			&&pCADEnt->m_pObj->GetClassTypeId()==CLS_LINEANGLE)
		{	//
			CString sValueText="";
			sValueText.Format("0X%X",pCADEnt->m_pObj->handle);
			m_propList.SetItemPropValue(pItem->m_idProp,sValueText);
			if(m_pSpreadLine)
				m_pSpreadLine->m_hDatumPole=pCADEnt->m_pObj->handle;
			UpdateSpreadLinePos();	//����չ����λ��
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
		m_propList.SetCurSel(pItem->m_iIndex);	//ѡ��ָ������
		CLDSObject *pObj=pCADEnt->m_pObj;
		if(pObj&&pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()
			&&((CLDSPart*)pObj)->IsLinePart())
		{	//
			CString sValueText="";
			sValueText.Format("0X%X",pCADEnt->m_pObj->handle);
			m_propList.SetItemPropValue(pItem->m_idProp,sValueText);
			m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.SetDatumRod((CLDSLinePart*)pObj);
			//ѡ��˼������±�ע��˨��ϸ
			m_pCurCmd->Rerun();
		}
	}
	else if(pItem&&m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO
		&&pItem->m_idProp==GetPropID("BOLTINFO.boltSet"))
	{	//�����˨����ǰ��˨�飬������˨��ԭ����˨��ɾ��������������������� wht 12-03-22
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
					continue;	//��˨���ڵ�ǰ��
				if(bolt_D!=pBolt->get_d()&&bolt_L!=ftoi(pBolt->get_L()))
					continue;	//��˨���ͬ�������
				boltSet.append(pBolt);
				//�ڵ�ǰ���������ѡ�����˨
				m_pCurCmd->m_paraBoltGroupInfo.lsGroupDim.AddBolt(pBolt);
			}
		}
		//�������������Ƴ���ӵ���ǰ�����˨�������ظ���ע
		CHashList<CDrawingCommand*> cmdHashList;
		CStruView *pStruView=(CStruView*)m_pCurCmd->m_pDrawingCom;
		pStruView->RemovBoltFromDimBoltInfoCmd(m_pCurCmd,boltSet,cmdHashList);
		//�������е�ǰ����
		m_pCurCmd->Rerun();
		//��������ʱ��������
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
		{	//��ӽڵ� wht 11-07-13
			m_pCurCmd->m_paraDimModelSize.AppendDimNode((CLDSNode*)pCADEnt->m_pObj);
		}
	}
	else if(pItem&&m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY&&resultList.GetNodeNum()>=3)
	{	//����ѡ����������������ĸ˼�����i�����i+1���˼��� ���һ�������һ���� wht 11-07-16
		m_pCurCmd->m_paraSimPoly.simPolyVertexList.Empty();
		f3dPoint geom_center;
		int i=0,nn=resultList.GetNodeNum();
		//TODO:��֪�����ε���Ϊʲô���������һ�����һ���� wjh-2017.87.25
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
		m_propList.SetCurSel(pItem->m_iIndex);	//ѡ��ָ������
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
	m_listCmdSuperCtrl.DeleteAllItems();	//�������
	if(m_pDrawing==NULL||m_pComponent==NULL)
		return;
	hashRodBoltOrFlInfoList.Empty();
	for(pCmd=m_pComponent->cmdList.GetFirst();pCmd;pCmd=m_pComponent->cmdList.GetNext())
	{
		if(pCmd->m_eCmdType==CMD_DIM_BOLTORFL_INFO)	//���˼���˨��Ϣ������Ϣ��ע�����б�
			hashRodBoltOrFlInfoList.SetValue(pCmd->m_paraBoltOrFlInfo.hLinePart,pCmd);
		char sCmdId[100]="",sCmdText[200]="";
		pCmd->GetCmdInfo(sCmdId,sCmdText);
		lpRoot=new CListCtrlItemInfo();
		lpRoot->SetImage(pCmd->m_stateCmd);	//��������״̬���ò�ͬ��ͼ��
		lpRoot->AddSubItemText(_T(""),TRUE);
		lpRoot->AddSubItemText(sCmdId,TRUE);
		lpRoot->AddSubItemText(sCmdText,TRUE);
		if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY
			&&(pCmd->m_paraSimPoly.nSimPolyCmdType==1
			||pCmd->m_paraSimPoly.nSimPolyCmdType==0))	//���������
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpRoot->SetSubItemText(1,"Assistant",TRUE);
			lpRoot->AddSubItemText("Add");
#else
			lpRoot->SetSubItemText(1,"����",TRUE);
			lpRoot->AddSubItemText("���");
#endif
			lpRoot->SetControlType(3,GCT_GRAYBUTTON);//��ť
		}
		else 
		{
			if(pCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
				lpRoot->AddSubItemText(_T("��"),TRUE);
#else
				lpRoot->AddSubItemText(_T("��"),TRUE);
#endif
			else
				lpRoot->AddSubItemText(_T("  "),TRUE);//��
		}
		pTreeItem=m_listCmdSuperCtrl.InsertRootItem(lpRoot);
		pTreeItem->m_idProp=(long)pCmd;
		if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_LINEVIEW)
		{	//����ͼ(��ͼ,˾��ͼ)
			CLineView *pLineView=(CLineView*)m_pComponent;
			if(pCmd->m_eCmdType==CMD_DRAW_LINEPART)
				pTreeItem->m_dwPropGroup=GetSingleWord(1);
			else if(pCmd->m_eCmdType==CMD_DIM_MODELSIZE)
				pTreeItem->m_dwPropGroup=GetSingleWord(2);
			else if(pCmd->m_eCmdType==CMD_DIM_SEGI)
			{
				if(pLineView->m_bAnalysisMode)	//˾��ͼҲ��Ҫ�κű�ע��Ϣ wht 11-07-15
					pTreeItem->m_dwPropGroup=GetSingleWord(4);	//˾��ͼ�κű�ע
				else 
					pTreeItem->m_dwPropGroup=GetSingleWord(3);	//��ͼ�κű�ע
			}
			else if(pLineView->m_bAnalysisMode&&(
				pCmd->m_eCmdType==CMD_DIM_PARTSPEC||pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT))
				pTreeItem->m_dwPropGroup=GetSingleWord(3);	//˾��ͼ������ϸ
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
		//SuperGridCtrl ����ͼ���ǵ�����ͼ�겻��������ʾ �˴��������´��� wht 10-11-30
		if(pCmd->m_stateCmd<2)
			pTreeItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd);	//����������״̬
		else 
			pTreeItem->m_lpNodeInfo->SetImage(pCmd->m_stateCmd+1);	//����������״̬
	}
	//������ͼ����
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
//ʹ����ͨListView��ʾ�����б�
void CDrawMapScriptDlg::DisplayCommonCtrlCmdList(int iGroup/*=0*/)
{
	if(m_pDrawing==NULL||m_pComponent==NULL)
		return;
	hashPartSpecList.Empty();
	m_listCmdCommonCtrl.DeleteAllItems();	//�������
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
		{	//������������
			if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)		//��乹����ϸ��ע�б�
				hashPartSpecList.SetValue(pCmd->m_paraPartSpec.hObj,pCmd);
			else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)	//�������ͷ��ע�б�
				hashOddmentList.SetValue(pCmd->m_paraOddment.hLinePart,pCmd);
			else if(pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)	//���Ŷ�����ע�б�
				hashFootNailSpaceList.SetValue(pCmd->m_paraDimFootNailSpace.hLinePart,pCmd);
			else if(pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)	//���������ע�б�
				hashFillPlateSpaceList.SetValue(pCmd->m_paraDimFillPlateSpace.hLinePart,pCmd);
			if(pCmd->m_eCmdType!=CMD_DRAW_LINEPART&&pCmd->m_eCmdType!=CMD_DRAW_PART
				&&pCmd->m_eCmdType!=CMD_DIM_SPECIAL_JGZHUN&&pCmd->m_eCmdType!=CMD_DIM_SPECIAL_LSSPACE
				&&pCmd->m_eCmdType!=CMD_DIM_FOOTNAIL_SPACE&&pCmd->m_eCmdType!=CMD_DIM_FILLPLATE_SPACE
				&&pCmd->m_eCmdType!=CMD_DIM_BOLTPAD&&pCmd->m_eCmdType!=CMD_DIM_BOLTINFO)
				return -1;
		}
		else if(iGroup==1)
		{	//��ע����ͷ
			if(pCmd->m_eCmdType!=CMD_DIM_ODDMENT)
				return -1;
			if(pCmd->m_pDrawingCom&&!pCmd->m_pDrawingCom->m_bFirstCreate)
			{	//δ��ִ�й�����������ͷ�������ʾ
				if(!pCmd->m_paraOddment.bDimStartOdd&&!pCmd->m_paraOddment.bDimEndOdd)
					return -1;	//ʼ�ն�����ͷ������Ҫ��ע�ĸ˼�����Ҫ��ʾ wht 11-07-25
			}
		}
		if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO&&pCmd->m_paraBoltGroupInfo.lsGroupDim.GetBoltNum()<=0)
			return -1;	//����ʾ��˨����Ϊ0�ģ���˨��ע��Ϣ wht 12-03-22
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
		{	//������˨
			CLDSBolt* pDimBolt=(CLDSBolt*)pCmd->m_paraPart.pPart;
			if(pCmd->m_paraPart.para.bolt.bOriginal)
				str_arr[2].Format("M%dX%.0f",pDimBolt->get_d(),pDimBolt->get_L());
			else
				str_arr[2].Format("��%.1f��",pDimBolt->get_d()+pDimBolt->hole_d_increment);
		}
		else
			str_arr[2]=sPartNo;
		str_arr[3]=pCmd->GetDrawPartName();
		if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[3]="Space";
#else
			str_arr[3]="���";
#endif
		else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		{
			if(!pCmd->m_paraDimJgZhun.zhun_dim.bDimZhunJu)
				return -1;	//ֻ������˨���߲���ע����׼���׼�߻���������������б�����ʾ
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[3]="G";
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			str_arr[3]="washer";
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			str_arr[3]="blots group";
#else
			str_arr[3]="׼��";
		}
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			str_arr[3]="��Ȧ";
		else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			str_arr[3]="��˨��";
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
			str_arr[4]="��ע";
		else if(pCmd->m_eCmdType==CMD_DRAW_LINEPART)
			str_arr[4]="����";
		else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
			str_arr[4]="��ͷ";
#endif
		else 
		{
			char sDrawType[50]="";
			int h0v1e2=pCmd->GetDrawType(sDrawType);
			str_arr[4]=sDrawType;
		}
		if(pCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr[5]="��";
#else
			str_arr[5]="��";
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

//��ʾ�˼���˨��Ϣ������Ϣ
void CDrawMapScriptDlg::UpdateRodBoltOrFlInfoPropList(CPropTreeItem *pParentItem,
													  CDrawingCommand *pCmd,BOOL bUpdate/*=FALSE*/)
{
	if(pParentItem==NULL||pCmd==NULL||pCmd->m_paraBoltOrFlInfo.pLinePart==NULL||
		pCmd->m_eCmdType!=CMD_DIM_BOLTORFL_INFO)
		return;
	if(!pCmd->m_paraBoltOrFlInfo.bDimStartOdd&&!pCmd->m_paraBoltOrFlInfo.bDimEndOdd)
		return;	//ʼ�ն˶�δ��ע
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
	CXhChar16 sName("��˨");
	if(pCmd->m_paraBoltOrFlInfo.pLinePart->IsTube())
		sName.Copy("����");
#endif
	CXhChar16 sRodName("%s",pCmd->m_paraBoltOrFlInfo.pLinePart->GetPartTypeName());
	//���ƹ�������
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName.Format("%s%s message",(char*)sRodName,(char*)sName);
#else
	lpInfo->m_strPropName.Format("%s%s��Ϣ",(char*)sRodName,(char*)sName);
#endif
	lpInfo->m_strPropHelp = lpInfo->m_strPropName;
	pGroupItem = m_propList.InsertItem(pParentItem,lpInfo, -1,bUpdate);
	pGroupItem->m_idProp = GetPropID("ROD_BOLTORFL_INFO");
	if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
	//ѡ��λ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_cmbItems="0.Search Auto|1.Specify";
	lpInfo->m_strPropName.Format("dimension position type");
	lpInfo->m_strPropHelp = "type of dimension position";
#else
	lpInfo->m_cmbItems="0.�Զ�����|1.ָ��λ��";
	lpInfo->m_strPropName.Format("��עλ������");
	lpInfo->m_strPropHelp = "��עλ������";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
	pPropItem->m_idProp=GetPropID("ROD_BOLTORFL_INFO.iDimPosType");
	if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,2);
	//
	UpdateRodStartEndInfoPropList(&m_propList,pGroupItem,pCmd);
}

//��ʾ�����������
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
	{	//�Ƿ����������
		CLDSPart *pPart=NULL;
		if(pCmd->m_paraPartSpec.pObj&&pCmd->m_paraPartSpec.pObj->IsPart())
			pPart=(CLDSPart*)pCmd->m_paraPartSpec.pObj;
		if(pPart)
		{	//������ϸ�ԳƷ�ʽ 0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Axis Mirror|1.Y Axis Mirror|2.No Mirror|";
			lpInfo->m_strPropName.Format("mirror type");
			lpInfo->m_strPropHelp = "mirror type of parts number dimension";
#else
			lpInfo->m_cmbItems="0.X��Գ�|1.Y��Գ�|2.�޶Գ�|";
			lpInfo->m_strPropName.Format("�ԳƷ�ʽ");
			lpInfo->m_strPropHelp = "������ű�עʱ�ĶԳƷ�ʽ��";
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
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName.Format("����������");
		lpInfo->m_strPropHelp = "��ע������ϸʱ�Ƿ���������ߡ�";
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp=GetPropID("PARTSPEC.bDrawRefLine");
		if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,iSonCmdType);
		//
		if(pPart&&pPart->IsLinePart())
		{	//��ע�˼���ϸ(���ȼ����)
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("dimension rod details");
			lpInfo->m_strPropHelp = "dimension the length and specifications of rods";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName.Format("��ע�˼���ϸ");
			lpInfo->m_strPropHelp = "��ע�˼������Լ����";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.bDimPoleSpec");
			if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,iSonCmdType);
			//�������ݲ���ʾ
			/*
			//�˼���עλ��ϵ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("�˼���עλ��ϵ��");
			lpInfo->m_strPropHelp = "�˼���עλ��ϵ����";
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.fPoleDimPosCoef");
			if(GetPropValueStr(pCmd,pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,iSonCmdType);
			*/
		}
		if(pCmd->m_paraPartSpec.bDrawRefLine)
		{	//����������ϸλ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("indication line and its detailed location");
			lpInfo->m_strPropHelp = "be used to adjust the positions of indication lines and parts.";
#else
			lpInfo->m_strPropName.Format("�����߼���ϸλ��");
			lpInfo->m_strPropHelp = "����������������ߺ͹�����ϸλ�á�";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.MovePartSpec");
		}

#ifdef AFX_TARG_ENU_ENGLISH
		char sTemp[10]="number";
		if(pPart)
			strcpy(sTemp,"details");
#else
		char sTemp[10]="���";
		if(pPart)
			strcpy(sTemp,"��ϸ");
#endif
		//���λ��ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("%s position offset",sTemp);
		lpInfo->m_strPropHelp.Format("%s position offset",sTemp);
#else
		lpInfo->m_strPropName.Format("%sλ��ƫ����",sTemp);
		lpInfo->m_strPropHelp.Format("%sλ��ƫ������",sTemp);
#endif
		pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
		pPropItem->m_idProp=GetPropID("PARTSPEC.DimPosOffset");
		if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
		//���λ��X����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("Axis X");
		lpInfo->m_strPropHelp.Format("%s dimension position offset value in X axis direction.",sTemp);
#else
		lpInfo->m_strPropName.Format("X�᷽��");
		lpInfo->m_strPropHelp.Format("%s��עλ��X�᷽��ƫ������",sTemp);
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp=GetPropID("PARTSPEC.fDimPosOffsetX");
		if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
		//���λ��Y����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("Axis Y");
		lpInfo->m_strPropHelp.Format("%s dimension position offset value in Y axis direction.",sTemp);
#else
		lpInfo->m_strPropName.Format("Y�᷽��");
		lpInfo->m_strPropHelp.Format("%s��עλ��Y�᷽��ƫ������",sTemp);
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
		pSonPropItem->m_idProp=GetPropID("PARTSPEC.fDimPosOffsetY");
		if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
		//
		if(pPart&&pPart->IsPlate())
		{	//���λ��ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("spec dimension position offset");
			lpInfo->m_strPropHelp = "specification dimension position offset value.";
#else
			lpInfo->m_strPropName.Format("���λ��ƫ����");
			lpInfo->m_strPropHelp = "����עλ��ƫ������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.GuigeOffset");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//����עλ��X����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis X");
			lpInfo->m_strPropHelp = "specifications dimension position offset value in axis X relative to number position.";
#else
			lpInfo->m_strPropName.Format("X�᷽��");
			lpInfo->m_strPropHelp = "����עλ���������λ����X�᷽���ƫ������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp=GetPropID("PARTSPEC.fGuigeOffsetX");
			if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
			//����עλ��Y����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis Y");
			lpInfo->m_strPropHelp = "specifications dimension position offset value in axis X relative to number position."; 
#else
			lpInfo->m_strPropName.Format("Y�᷽��");
			lpInfo->m_strPropHelp = "����עλ���������λ����Y�᷽���ƫ������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp=GetPropID("PARTSPEC.fGuigeOffsetY");
			if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
		}
		//
		if(pCmd->m_paraPartSpec.bDrawRefLine)
		{	//���ݵ�λ��ƫ���� wht 11-07-25
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("data point position offset");
			lpInfo->m_strPropHelp = "data point position offset value.";
#else
			lpInfo->m_strPropName.Format("���ݵ�λ��ƫ����");
			lpInfo->m_strPropHelp = "���ݵ�λ��ƫ������";
#endif
			pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1,bUpdate);
			pPropItem->m_idProp=GetPropID("PARTSPEC.BasePosOffset");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//��עλ��X����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis X");
			lpInfo->m_strPropHelp = "data point offset value in axis direction.";
#else
			lpInfo->m_strPropName.Format("X�᷽��");
			lpInfo->m_strPropHelp = "���ݵ�λ��X�᷽��ƫ������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1,bUpdate);
			pSonPropItem->m_idProp=GetPropID("PARTSPEC.fBasePosOffsetX");
			if(GetPropValueStr(pCmd,pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,iSonCmdType);
			//��עλ��Y����ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("Axis Y");
			lpInfo->m_strPropHelp = "data point position offset value in axis Y.";
#else
			lpInfo->m_strPropName.Format("Y�᷽��");
			lpInfo->m_strPropHelp = "���ݵ�λ��Y�᷽��ƫ������";
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
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "�������";
#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = GetPropID("CmdPara");
		if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
		//�Ƿ�ִ�и�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName = "enable this command";
		lpInfo->m_strPropHelp = "can pass this parameter controls, whether to execute the current command.";
#else
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName = "���ø�����";
		lpInfo->m_strPropHelp = "����ͨ���ò��������Ƿ�ִ�е�ǰ���";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp=GetPropID("m_bExecute");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		pPropItem->SetReadOnly();
	}
	else if(IsSpreadLinePropGroup(iCurSel))
	{	//չ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "spreading line";
		lpInfo->m_strPropHelp = "spreading line";
#else
		lpInfo->m_strPropName = "չ����";
		lpInfo->m_strPropHelp = "չ����";
#endif
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = GetPropID("SPREAD_LINE");
		if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
		//չ��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("name");
		lpInfo->m_strPropHelp = "name of spreading line";
#else
		lpInfo->m_strPropName.Format("����");
		lpInfo->m_strPropHelp = "չ�������ơ�";
#endif
		pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_sLineName");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		//��λ��ʽ:0.ָ���˼� 1.ֱ��ָ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Specify Rod|1.Directly Specify|";
		lpInfo->m_strPropName.Format("positioning way");
		lpInfo->m_strPropHelp = "spread line positioning ";
#else
		lpInfo->m_cmbItems="0.ָ���˼�|1.ֱ��ָ��|";
		lpInfo->m_strPropName.Format("��λ��ʽ");
		lpInfo->m_strPropHelp = "չ���߶�λ��ʽ��";
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
		CString sName="���Ʋ���";
		if(m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
			sName.Format("�������Ʋ���");
		else if(m_pCurCmd->m_eCmdType==CMD_DRAW_TABLE)
			sName.Format("�����Ʋ���");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
			sName.Format("������ϸ��ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY)
			sName.Format("�����α�ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ODDMENT)
			sName.Format("����ͷ��ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
			sName.Format("�Ŷ�����ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
			sName.Format("������ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTPAD)
			sName.Format("��˨��Ȧ��ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
			sName.Format("�Ǹ�����׼�ݱ�ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
			sName.Format("��˨�������ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MAPSIZE)
			sName.Format("ͼ��ߴ��ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
			sName.Format("ģ�ͳߴ��ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
			sName.Format("��˨��Ϣ��ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SEGI)
			sName.Format("�κű�ע����");
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
			sName.Format("�Ǹֽ�ͷ��ע����");
#endif
		//���ƹ�������
		lpInfo = new CItemInfo();
		lpInfo->m_strPropName = sName;
		lpInfo->m_strPropHelp = sName;
		pGroupItem = m_propList.InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = GetPropID("DrawPara");
		if(CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pGroupItem->m_idProp);
		//
		if(m_pCurCmd->m_eCmdType==CMD_DRAW_PART)
		{	//��ǰ���ƹ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle for current drawing part";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ���ƹ����ľ����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("PART.pPart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			if(m_pCurCmd->m_paraPart.pPart&&m_pCurCmd->m_paraPart.pPart->IsLinePart())
			{
				//��ʱ��ʹ������������  wht 11-07-23
				/*
				if(m_pCurCmd->m_paraPart.pPart->GetClassTypeId()==CLS_LINEANGLE)
				{	//�Ǹֻ�׼�ľ�
					//X֫��׼�ľ�
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
					lpInfo->m_strPropName.Format("X֫��׼�ľ�");
					lpInfo->m_strPropHelp = "X֫��׼�ľ�,���ƽǸ�����ʱ�����ľ���ơ�";
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.angle.nXWingDatumAxisG");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
					//Y֫��׼�ľ�
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
					lpInfo->m_strPropName.Format("Y֫��׼�ľ�");
					lpInfo->m_strPropHelp = "Y֫��׼�ľ࣬���ƽǸ�����ʱ�����ľ���ơ�";
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.angle.nXWingDatumAxisG");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}*/
				if(m_pCurCmd->m_paraPart.pPart->GetClassTypeId()==CLS_LINEANGLE)
				{	//֫����չ��ʽ
					CLDSLineAngle *pAngle=(CLDSLineAngle*)m_pCurCmd->m_paraPart.pPart;
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("leg width expanding way");
					lpInfo->m_cmbItems="0.Expanding On Both Sides|1.Expanding Legs|2.Expanding Ridge Line";
					lpInfo->m_strPropHelp = "when leg width of angle leg is  exaggeratedly drawn, the way to expanding leg width";
#else
					lpInfo->m_strPropName.Format("֫����չ��ʽ");
					lpInfo->m_cmbItems="0.������չ|1.��չ֫��|2.��չ����";
					lpInfo->m_strPropHelp = "�Ǹ�֫�������ʱ��֫�����չ��ʽ";
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
					lpInfo->m_strPropName.Format("��������");
					lpInfo->m_cmbItems="��|��";
					lpInfo->m_strPropHelp = "�Ƿ���ƽǸ����ߣ�һЩ�̽Ǹֻ�ͼʱ����Ҫ�������ߡ�";
#endif
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.angle.bDrawAxisLine");
					if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
					{
						pSonPropItem->SetReadOnly();
#ifdef AFX_TARG_ENU_ENGLISH
						pSonPropItem->m_lpNodeInfo->m_strPropValue="No";
#else
						pSonPropItem->m_lpNodeInfo->m_strPropValue="��";
#endif
					}
					else if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
				//�˼�����ʼ�����쳤��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("start extending");
				lpInfo->m_strPropHelp = "starting end's extending value of rod's center line.";
#else
				lpInfo->m_strPropName.Format("ʼ��������");
				lpInfo->m_strPropHelp = "�˼�����ʼ����������";
#endif
				pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("PART.fPoleAxisLineStartOutSpreadLen");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//�˼������ն����쳤��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("end extending");
				lpInfo->m_strPropHelp = "ending end's extending value of rod's center line.";
#else
				lpInfo->m_strPropName.Format("�ն�������");
				lpInfo->m_strPropHelp = "�˼������ն���������";
#endif
				pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("PART.fPoleAxisLineEndOutSpreadLen");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
			
			//չ�������� 0.��ʾ��չ���� i-1��ʾչ������չ������չ���б��е�ֵ
			if(m_pComponent&&m_pComponent->m_eClassType==DRAW_COM_STRUVIEW&&m_pComponent->m_pAttachView)
				//&&m_pComponent->m_pAttachView->m_iViewType==1)
			{	//�ṹͼ(Ŀǰ��������ͼ֧��չ������)
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="drawing plane";
				lpInfo->m_strPropHelp = "parts' drawing plane, can be set up in the modeling environment.";
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("PART.iResidePlane");
				pPropItem->m_lpNodeInfo->m_strPropValue="0.Datum Plane";
#else
				lpInfo->m_strPropName="��ͼƽ��";
				lpInfo->m_strPropHelp = "�������ڵĻ�ͼƽ�棬���ڽ�ģ���������á�";
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("PART.iResidePlane");
				pPropItem->m_lpNodeInfo->m_strPropValue="0.��׼��";
#endif
				CDrawingAssistant *pAssist=m_pComponent->m_pAttachView->GetDrawingAssistByHandle(m_pCurCmd->m_paraPart.hPart);
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
				pPropItem->SetReadOnly();
				if(pAssist&&pAssist->m_nStyle==0&&pAssist->iNo>0)
				{
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%d.%s",pAssist->iNo,pAssist->name);
					/*
					//���������� 'X'.X���� 'Y'.Y����,'Z'.Z���� �Զ��ж�
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbItems="X����|Y����|Z����|�Զ��ж�|";
					lpInfo->m_strPropName.Format("������");
					lpInfo->m_strPropHelp = "���������档";
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
				{	//ƽ�Ų��õ���˨
					lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName.Format("drawing way");
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
					lpInfo->m_cmbItems="0.Picture|1.Straight Line";
					lpInfo->m_strPropHelp = "drawing expression way of flat decorated bolts.";
#else
					lpInfo->m_strPropName.Format("���Ʒ�ʽ");
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
					lpInfo->m_cmbItems="0.ͼ��|1.ֱ��";
					lpInfo->m_strPropHelp = "ƽ�Ų�����˨�Ļ��Ʊ�﷽ʽ��";
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
					lpInfo->m_strPropName.Format("��ע�׾�");
					lpInfo->m_controlType=PIT_COMBO;
					lpInfo->m_cmbType=CDT_LIST;
					lpInfo->m_cmbItems="��|��";
					lpInfo->m_strPropHelp = "�Ƿ��ע����׼ӹ�ֱ����";
#endif
					pSonPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("PART.bolt.bDimHoleDiameter");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
			}
			//���Ҷ�Ӧ�Ĺ�����ϸ��ע�����ʾ����
			CDrawingCommand **ppCmd=hashPartSpecList.GetValue(m_pCurCmd->m_paraPart.hPart);
			if(ppCmd)
			{
				//�Ƿ��ע������ϸ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="Yes|No";
				lpInfo->m_strPropName.Format("Dimension Parts Detail");
				lpInfo->m_strPropHelp = "whether to dimension parts details.";
#else
				lpInfo->m_cmbItems="��|��";
				lpInfo->m_strPropName.Format("��ע������ϸ");
				lpInfo->m_strPropHelp = "�Ƿ��ע������ϸ��";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("m_bExecute");
				if(GetPropValueStr(*ppCmd,pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,1);	
				//��ʾ��ע������ϸ�������
				UpdatePartSpecPropertyList(pPropItem,*ppCmd,TRUE);
			}
		}
		//�����Ʋ���
		else if(m_pCurCmd->m_eCmdType==CMD_DRAW_TABLE)
		{	
			//ѡ����λ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("choose position");
			lpInfo->m_strPropHelp = "choose position for chart";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName.Format("ѡ��λ��");
			lpInfo->m_strPropHelp = "ѡ����λ��";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("TABLE.bSelectPos");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//��ʾ��λλ�öԻ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("display dialog");
			lpInfo->m_strPropHelp = "display positioning location dialog.";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName.Format("��ʾ�Ի���");
			lpInfo->m_strPropHelp = "��ʾ��λλ�öԻ���";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("TABLE.bDisplayDlg");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			if(!m_pCurCmd->m_paraBOMTable.bDisplayDlg)
			{	//���λλ��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="0.Upper Left|1.Upper Right|2.Lower Left|3.Lower Right|";
				lpInfo->m_strPropName.Format("chart positioning location");
				lpInfo->m_strPropHelp = "chart positioning location";
#else
				lpInfo->m_cmbItems="0.����|1.����|2.����|3.����|";
				lpInfo->m_strPropName.Format("���λλ��");
				lpInfo->m_strPropHelp = "���λλ��";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("TABLE.nLocatePos");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
			//���λ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("chart position");
			lpInfo->m_strPropHelp = "chart position, the position is the relative position in mapping coordinates.";
#else
			lpInfo->m_strPropName.Format("���λ��");
			lpInfo->m_strPropHelp = "���λ�ã���λ��Ϊ��ͼ����ϵ�µ����λ�á�";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("TABLE.DimPos");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//���λ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "chart position X coordinates, which is the relative coordinate in mapping coordinates.";
#else
			lpInfo->m_strPropHelp = "���λ��X���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("TABLE.fDimPosX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//���λ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "chart position Y coordinates, which is the relative coordinate in mapping coordinates.";
#else
			lpInfo->m_strPropHelp = "���λ��Y���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("TABLE.fDimPosY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
		}
		//������ϸ��ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_PARTSPEC)
		{	//��ǰ��ע����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "current dimension parts' handle.";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ��ע�����ľ����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("PARTSPEC.pObj");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			UpdatePartSpecPropertyList(pGroupItem,m_pCurCmd,FALSE);
			//��ʾʼ�ն���˨��Ϣ������Ϣ
			CDrawingCommand **ppCmd=hashRodBoltOrFlInfoList.GetValue(m_pCurCmd->m_paraPartSpec.hObj);
			if(ppCmd)
				UpdateRodBoltOrFlInfoPropList(pRoot,*ppCmd,FALSE);
		}
		//��ע�κ�
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SEGI)
		{
			//�����κ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "segment number";
			lpInfo->m_strPropHelp = "dimension segment number";
#else
			lpInfo->m_strPropName = "�κ�";
			lpInfo->m_strPropHelp = "��ע�κš�";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSEGI.nSegI");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//ѡ��λ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("select position");
			lpInfo->m_strPropHelp = "select position for segment number.";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName.Format("ѡ��λ��");
			lpInfo->m_strPropHelp = "ѡ��κ�λ��";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSEGI.bSelectPos");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//��עλ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "dimension position";
			lpInfo->m_strPropHelp = "segment number dimension position, offset value relative to the top right corner of the box.";
#else
			lpInfo->m_strPropName = "��עλ��";
			lpInfo->m_strPropHelp = "�κű�עλ�ã������ͼ�����Ͻǵ�ƫ��ֵ��";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSEGI.DimPos");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//��עλ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value relative to X direction of the upper right corner  in the box.";
#else
			lpInfo->m_strPropHelp = "�����ͼ�����Ͻ�X�����ƫ��ֵ��";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSEGI.fDimPosX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//��עλ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value relative to Y direction of the upper right corner  in the box.";
#else
			lpInfo->m_strPropHelp = "�����ͼ�����Ͻ�Y�����ƫ��ֵ��";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSEGI.fDimPosY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		//�����α�ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SIMPOLY)
		{
			//�����α���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("similar polygon scale");
			lpInfo->m_strPropHelp = "polygon proportion, consistent with the proportion set in the system by default.";
#else
			lpInfo->m_strPropName.Format("�����α���");
			lpInfo->m_strPropHelp = "�����α�����Ĭ����ϵͳ�����е������α���һ�¡�";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SIMPOLY.fScale");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			if(m_pCurCmd->m_paraSimPoly.nSimPolyCmdType==2)
			{	//������ʰȡ��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("pick point");
				lpInfo->m_strPropHelp = "with similar figures when pick up points, this position for CAD coordinates.";
#else
				lpInfo->m_strPropName.Format("ʰȡ��");
				lpInfo->m_strPropHelp = "��ע������ʱ��ʰȡ��,��λ��ΪCAD���ꡣ";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("SIMPOLY.PickPt");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//ʰȡ��X����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "X coordinate of pick point";
#else
				lpInfo->m_strPropHelp = "ʰȡ��X���ꡣ";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("SIMPOLY.fPickPtX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//ʰȡ��Y����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Y coordinate of pick point";
#else
				lpInfo->m_strPropHelp = "ʰȡ��Y���ꡣ";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("SIMPOLY.fPickPtY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
			//������λ��ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("offset");
			lpInfo->m_strPropHelp = "offset value of similar polygon dimension position.";
#else
			lpInfo->m_strPropName.Format("ƫ����");
			lpInfo->m_strPropHelp = "�����α�עλ��ƫ������";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SIMPOLY.DimPosOffset");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//X�᷽��ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value of similar polygon dimension position in axis X.";
#else
			lpInfo->m_strPropHelp = "�����α�עλ��X�᷽��ƫ������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("SIMPOLY.fDimPosOffsetX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//Y�᷽��ƫ��ֵ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "offset value of similar polygon dimension position in axis Y";
#else
			lpInfo->m_strPropHelp = "�����α�עλ��Y�᷽��ƫ������";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("SIMPOLY.fDimPosOffsetY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);

			if(m_pCurCmd->m_paraSimPoly.nSimPolyCmdType==3)
			{	//�������������б�
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("outline vertex");
				lpInfo->m_strPropHelp = "outline vertex";
#else
				lpInfo->m_strPropName.Format("������");
				lpInfo->m_strPropHelp = "������";
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
					//����i
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sKey,"vertex %d",i);
#else
					sprintf(sKey,"����%d",i);
#endif
					lpInfo->m_strPropName = sKey;
					lpInfo->m_strPropHelp = sKey;
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					sprintf(sKey,"SIMPOLY.vertex%d",i);
					pSonPropItem->m_idProp=GetPropID(sKey);
					if(CDrawMapScriptDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
						pSonPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pSonPropItem->m_idProp);
					//X����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_buttonType=BDT_FILEPATH;
					lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "X coordinate for outline vertex of similar polygons";
#else
					lpInfo->m_strPropHelp = "������������X����";
#endif
					CPropTreeItem *pLeafItem = m_propList.InsertItem(pSonPropItem,lpInfo, -1);
					sprintf(sKey,"SIMPOLY.vertex%d.x",i);
					pLeafItem->m_idProp=GetPropID(sKey);
					if(GetPropValueStr(pLeafItem->m_idProp,valueStr)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pLeafItem->m_idProp);
					//Y����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_buttonType=BDT_FILEPATH;
					lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "Y coordinate for outline vertex of similar polygons";
#else
					lpInfo->m_strPropHelp = "������������Y����";
#endif
					pLeafItem = m_propList.InsertItem(pSonPropItem,lpInfo, -1);
					sprintf(sKey,"SIMPOLY.vertex%d.y",i);
					pLeafItem->m_idProp=GetPropID(sKey);
					if(GetPropValueStr(pLeafItem->m_idProp,valueStr)>0)
						pLeafItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pLeafItem->m_idProp);
					//
					//��׼�˼�1
					//lpInfo = new CItemInfo();
					//lpInfo->m_controlType=PIT_BUTTON;
					//lpInfo->m_buttonType=BDT_FILEPATH;
					//lpInfo->m_strPropName.Format("��׼�˼�1");
					//lpInfo->m_strPropHelp = "�������׼�˼�1��";
					//CPropTreeItem *pLeafItem = m_propList.InsertItem(pSonPropItem,lpInfo, -1);
					//sprintf(sKey,"SIMPOLY.vertex%d.hPrevPole",i);
					//pLeafItem->m_idProp=GetPropID(sKey);
					//if(GetPropValueStr(pLeafItem->m_idProp,valueStr)>0)
					//	pLeafItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					//��׼�˼�2
					//lpInfo = new CItemInfo();
					//lpInfo->m_controlType=PIT_BUTTON;
					//lpInfo->m_buttonType=BDT_FILEPATH;
					//lpInfo->m_strPropName.Format("��׼�˼�2");
					//lpInfo->m_strPropHelp = "�������׼�˼�2��";
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
						AfxMessageBox("Ŀǰ�����֧��15��������!");
#endif
						break;
					}
				}
			}
		}
		//���Ƹ˼�����ͼ
		else if(m_pCurCmd->m_eCmdType==CMD_DRAW_LINEPART)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "currently drawn rod's handle.";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ���Ƹ˼��ľ����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("LINEPART.pLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//�Ƿ�Ϊ��ͼ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("General Map");
			lpInfo->m_strPropHelp = "singular line map in general map";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName.Format("��ͼ");
			lpInfo->m_strPropHelp = "��ͼ�еĵ���ͼ";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("LINEPART.bSum");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//��ѧ����ģʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Yes|No";
			lpInfo->m_strPropName.Format("mechanical analysis mode");
			lpInfo->m_strPropHelp = "mechanical analysis mode";
#else
			lpInfo->m_cmbItems="��|��";
			lpInfo->m_strPropName.Format("��ѧ����ģʽ");
			lpInfo->m_strPropHelp = "��ѧ����ģʽ";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("LINEPART.bDimByAnalysisMode");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		//����ͷ��ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ODDMENT)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts ";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ��ע�����ľ����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ODDMENT.pLinePart");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//
			//ѡ��λ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
			lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.�Զ�����|1.ָ��λ��";
			lpInfo->m_strPropName.Format("��עλ������");
			lpInfo->m_strPropHelp = "��עλ������";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ODDMENT.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			if(m_pCurCmd->m_paraOddment.iDimPosType==1)
			{	
				if(m_pCurCmd->m_paraOddment.bDimStartOdd)
				{	//ʼ������ͷ��עλ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "start position offset";
					lpInfo->m_strPropHelp = "dimension position offset between the two relative to dimension basis point";
#else
					lpInfo->m_strPropName = "ʼ��λ��ƫ��";
					lpInfo->m_strPropHelp = "ʼ������ͷ��עλ������ڱ�ע�����ƫ�ơ�";
#endif
					pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pPropItem->m_idProp=GetPropID("ODDMENT.StartOddPosOffset");
					if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
					pPropItem->m_bHideChildren=TRUE;
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//ʼ������ͷ��עλ��X����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset in X coordinate of starting two ends";
#else
					lpInfo->m_strPropHelp = "ʼ������ͷ��עλ��X����ƫ�ơ�";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fStartOddPosOffsetX");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
					//ʼ������ͷ��עλ��Y����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset in X coordinate of starting two ends";
#else
					lpInfo->m_strPropHelp = "ʼ������ͷ��עλ��Y����ƫ�ơ�";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fStartOddPosOffsetY");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
				if(m_pCurCmd->m_paraOddment.bDimEndOdd)
				{	//�ն�����ͷ��עλ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "end position offset";
					lpInfo->m_strPropHelp = "dimension position offset of ending two ends relative to dimension basis point.";
#else
					lpInfo->m_strPropName = "�ն�λ��ƫ��";
					lpInfo->m_strPropHelp = "�ն�����ͷ��עλ������ڱ�ע�����ƫ�ơ�";
#endif
					pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
					pPropItem->m_idProp=GetPropID("ODDMENT.EndOddPosOffset");
					if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
					pPropItem->m_bHideChildren=TRUE;
					if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
						pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
					//�ն�����ͷ��עλ��X����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset in X coordinate of ending two ends";
#else
					lpInfo->m_strPropHelp = "�ն�����ͷ��עλ��X����ƫ�ơ�";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fEndOddPosOffsetX");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
					//�ն�����ͷ��עλ��Y����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_BUTTON;
					lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp = "dimension position offset of ending two ends";
#else
					lpInfo->m_strPropHelp = "�ն�����ͷ��עλ��Y����ƫ�ơ�";
#endif
					pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp=GetPropID("ODDMENT.fEndOddPosOffsetY");
					if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				}
			}
		}
		//�Ŷ�����ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ��ע�����ľ����";
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
			lpInfo->m_strPropName.Format("��ע���ּ��");
			lpInfo->m_strPropHelp = "��ע�������ע��ʼ����ֵ";
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
			lpInfo->m_strPropName.Format("��ע��ʼ����");
			lpInfo->m_strPropHelp = "��ע��ʼ����Ŷ����ڸ˼����Ե���ֵ";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FOOTNAIL_SPACE.fSpace");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		//������ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts. ";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ��ע�����ľ����";
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
			lpInfo->m_strPropName.Format("��ע���ּ��");
			lpInfo->m_strPropHelp = "��ע�������ע��ʼ����ֵ";
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
			lpInfo->m_strPropName.Format("��ע��ʼ����");
			lpInfo->m_strPropHelp = "��ע��ʼ����Ŷ����ڸ˼����Ե���ֵ";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("FILLPLATE_SPACE.fSpace");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
#ifdef __PART_DESIGN_INC_
		//�Ǹֽ�ͷ��Ϣ��ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
		{	
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension parts.";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ��ע�����ľ����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ANGLEJOINT.pObj");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//��עλ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
                        lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.�Զ�����|1.ָ��λ��";
			lpInfo->m_strPropName.Format("��עλ������");
			lpInfo->m_strPropHelp = "��עλ������";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("ANGLEJOINT.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp,3);
			//
			if(m_pCurCmd->m_paraAngleJoint.iDimPosType==1)
			{
				//��עλ��ƫ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("dimension position offset");
				lpInfo->m_strPropHelp = "dimension position offset value";
#else
				lpInfo->m_strPropName.Format("��עλ��ƫ����");
				lpInfo->m_strPropHelp = "��עλ��ƫ������";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("ANGLEJOINT.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//��עλ��X����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp = "dimension position offset in axis X direction.";
#else
				lpInfo->m_strPropName.Format("X�᷽��");
				lpInfo->m_strPropHelp = "��עλ��X�᷽��ƫ������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("ANGLEJOINT.fDimPosOffsetX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,3);
				//��עλ��Y����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp = "dimension position offset in axis Y direction.";
#else
				lpInfo->m_strPropName.Format("Y�᷽��");
				lpInfo->m_strPropHelp = "��עλ��Y�᷽��ƫ������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("ANGLEJOINT.fDimPosOffsetY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp,3);
			}
		}
#endif
		//��˨��Ȧ��ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTPAD)
		{
			//��˨��Ȧ������˨
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "handle of currently dimension joint bolts.";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��ǰ��ע������˨�ľ����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTPAD.pBolt");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//��˨��Ȧ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("bolt's washer spec");
			lpInfo->m_strPropHelp = "specifications of  bolt's washer";
#else
			lpInfo->m_strPropName.Format("��˨��Ȧ���");
			lpInfo->m_strPropHelp = "��˨��Ȧ���";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTPAD.sSpec");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//��עλ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
			lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.�Զ�����|1.ָ��λ��";
			lpInfo->m_strPropName.Format("��עλ������");
			lpInfo->m_strPropHelp = "��עλ������";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTPAD.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			if(m_pCurCmd->m_paraBoltPad.iDimPosType==1)
			{
				//��עλ��ƫ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("dimension position offset");
				lpInfo->m_strPropHelp = "dimension position offset value";
#else
				lpInfo->m_strPropName.Format("��עλ��ƫ����");
				lpInfo->m_strPropHelp = "��עλ��ƫ������";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("BOLTPAD.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//��עλ��X����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp = "dimension position offset value in axis X direction.";
#else
				lpInfo->m_strPropName.Format("X�᷽��");
				lpInfo->m_strPropHelp = "��עλ��X�᷽��ƫ������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTPAD.fDimPosOffsetX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//��עλ��Y����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp = "dimension position offset value in axis Y direction.";
#else
				lpInfo->m_strPropName.Format("Y�᷽��");
				lpInfo->m_strPropHelp = "��עλ��Y�᷽��ƫ������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTPAD.fDimPosOffsetY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
		}
		//��˨��Ϣ��ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_BOLTINFO)
		{	//��׼�˼�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("datum rod");
			lpInfo->m_strPropHelp = "bolt information dimension datum rod��to confirm bolt information dimension direction.";
#else
			lpInfo->m_strPropName.Format("��׼�˼�");
			lpInfo->m_strPropHelp = "��˨��Ϣ��ע��׼�˼�������ȷ����˨��Ϣ��ע����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTINFO.pDatumRod");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			//��ע����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension content");
			lpInfo->m_strPropHelp = "bolt combination dimension content";
#else
			lpInfo->m_strPropName.Format("��ע����");
			lpInfo->m_strPropHelp = "��˨���ע����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTINFO.sLabel");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			//��˨�б�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_sButtonName="Add";
			lpInfo->m_strPropName.Format("bolts list");
			lpInfo->m_strPropHelp = "list of bolts";
#else
			lpInfo->m_sButtonName="���";
			lpInfo->m_strPropName.Format("��˨�б�");
			lpInfo->m_strPropHelp = "��˨�б�";
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
				lpInfo->m_sButtonName="ɾ��";
				lpInfo->m_strPropName.Format("��˨%d",i);
				lpInfo->m_strPropHelp = "��˨���";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=(long)pBoltInfo;	//ʹ����˨��Ϣָ����Ϊid
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				i++;
			}
			//��עλ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Search Auto|1.Specify";
			lpInfo->m_strPropName.Format("dimension position type");
			lpInfo->m_strPropHelp = "dimension position type";
#else
			lpInfo->m_cmbItems="0.�Զ�����|1.ָ��λ��";
			lpInfo->m_strPropName.Format("��עλ������");
			lpInfo->m_strPropHelp = "��עλ������";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("BOLTINFO.iDimPosType");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//
			if(m_pCurCmd->m_paraBoltGroupInfo.iDimPosType==1)
			{
				//��עλ��ƫ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("dimension position offset");
				lpInfo->m_strPropHelp = "dimension position offset value";
#else
				lpInfo->m_strPropName.Format("��עλ��ƫ����");
				lpInfo->m_strPropHelp = "��עλ��ƫ������";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("BOLTINFO.DimPosOffset");
				if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
				//��עλ��X����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis X");
				lpInfo->m_strPropHelp = "dimension position offset value in axis X.";
#else
				lpInfo->m_strPropName.Format("X�᷽��");
				lpInfo->m_strPropHelp = "��עλ��X�᷽��ƫ������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTINFO.fDimPosOffsetX");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
				//��עλ��Y����ƫ��ֵ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Axis Y");
				lpInfo->m_strPropHelp = "dimension position offset value in axis Y";
#else
				lpInfo->m_strPropName.Format("Y�᷽��");
				lpInfo->m_strPropHelp = "��עλ��Y�᷽��ƫ������";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp=GetPropID("BOLTINFO.fDimPosOffsetY");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
		}
		//��ע�Ǹ�����׼��
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
		{
			//��Ҫ��ע����׼�ݵĽǸ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("%s handle",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "angles to be marked specific criterion.";
#else
			lpInfo->m_strPropName.Format("%s���",m_pCurCmd->GetDrawPartName());
			lpInfo->m_strPropHelp = "��Ҫ��ע����׼�ݵĽǸ֡�";
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
			lpInfo->m_strPropName.Format("��ע���ּ��");
			lpInfo->m_strPropHelp = "��ע�������ע��ʼ����ֵ";
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
			lpInfo->m_strPropName.Format("��ע��ʼ����");
			lpInfo->m_strPropHelp = "��ע��ʼ����ʼ�˱�עλ��֮��ľ��롣";
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
			lpInfo->m_cmbItems="�ڲ�|���";
			lpInfo->m_strPropName.Format("��ע����");
			lpInfo->m_strPropHelp = "��ע����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIM_JGZHUN.bInside");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
		}
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
		{
			//��˨������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("starting bolt handle");
			lpInfo->m_strPropHelp.Format("bolts specific space, bolt's handle on starting end");
#else
			lpInfo->m_strPropName.Format("ʼ����˨���");
			lpInfo->m_strPropHelp.Format("��˨������,ʼ����˨�����");
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
			lpInfo->m_strPropName.Format("�ն���˨���");
			lpInfo->m_strPropHelp.Format("��˨������,�ն���˨�����");
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
			lpInfo->m_strPropName.Format("��ע���ּ��");
			lpInfo->m_strPropHelp = "��ע�������ע��ʼ����ֵ";
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
			lpInfo->m_strPropName.Format("��ע��ʼ����");
			lpInfo->m_strPropHelp = "��ע��ʼ����ʼ����˨֮��ľ��롣";
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
			lpInfo->m_cmbItems="�ڲ�|���";
			lpInfo->m_strPropName.Format("��ע����");
			lpInfo->m_strPropHelp = "��ע����";
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
				lpInfo->m_strPropName.Format("��˨�����");
				lpInfo->m_strPropHelp = "��������˨�������";
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
				lpInfo->m_strPropHelp = "combine dimension bolt space,dimension in the mode of single space �� number";
#else
				lpInfo->m_cmbItems="��|��";
				lpInfo->m_strPropName.Format("�ϲ���ע��˨���");
				lpInfo->m_strPropHelp = "�ϲ���ע������˨��࣬��ע�ɵ���������������ʽ��";
#endif
				pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp=GetPropID("DIM_LSSPACE.bMergeDimLsSpace");
				if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			}
		}
		//ģ�ͳߴ��ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
		{
			//ģ�ͳߴ��ע����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "type";
			lpInfo->m_strPropHelp = "model size dimension type";
			lpInfo->m_cmbItems="0.Inter Node Offset|1.Vertical Cross Arm Space|2.Piecewise Interval|3.Tower Height|4.Changing Slope Mouth|5.Cross Arm Horizontal Space|";
#else
			lpInfo->m_strPropName = "����";
			lpInfo->m_strPropHelp = "ģ�ͳߴ��ע���͡�";
			lpInfo->m_cmbItems="0.�ڼ䴹��|1.�ᵣ��ֱ���|2.�ֶμ��|3.����|4.���¿���|5.�ᵣˮƽ���|";
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
			lpInfo->m_strPropName.Format("��ע���ּ��");
			lpInfo->m_strPropHelp = "��ע�������ע��ʼ����ֵ";
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
			lpInfo->m_strPropName.Format("��ע��ʼ����");
			lpInfo->m_strPropHelp = "��ע��ʼ����������ֵ";
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
			lpInfo->m_strPropName.Format("�ڵ��б�");
			lpInfo->m_strPropHelp = "�ڵ��б�";
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
				lpInfo->m_sButtonName="ɾ��";
				lpInfo->m_strPropName.Format("�ڵ�%d",m_pCurCmd->m_paraDimModelSize.listDimNodes.GetCurrentIndex());
				lpInfo->m_strPropHelp = "�ڵ���";
#endif
				pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
				CXhChar50 sPropStr("DIMMODELSIZE.Node%d",m_pCurCmd->m_paraDimModelSize.listDimNodes.GetCurrentIndex());
				pSonPropItem->m_idProp=GetPropID(sPropStr);
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			}
		}
		//ͼ��ߴ��ע����
		else if(m_pCurCmd->m_eCmdType==CMD_DIM_MAPSIZE)
		{
			//�ߴ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("size dimension scale");
			lpInfo->m_strPropHelp = "size dimension  scale,consistent with the proportion set in the system by default.";
#else
			lpInfo->m_strPropName.Format("�ߴ��ע����");
			lpInfo->m_strPropHelp = "�ߴ��ע������Ĭ����ϵͳ�����еĽṹͼ���Ʊ���һ�¡�";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.fScale");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Axis X|1.Axis Y|2.Extend Direction|";
			lpInfo->m_strPropName.Format("size dimension direction");
			lpInfo->m_strPropHelp = "size dimension direction";
#else
			lpInfo->m_cmbItems="0.X�᷽��|1.Y�᷽��|2.���췽��|";
			lpInfo->m_strPropName.Format("�ߴ��ע����");
			lpInfo->m_strPropHelp = "�ߴ��ע����";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.iDimDirect");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pPropItem->m_idProp);
			//��ע��ʼ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension starting and ending points");
			lpInfo->m_strPropHelp = "dimension starting and ending points";
#else
			lpInfo->m_strPropName.Format("��ע��ʼ��");
			lpInfo->m_strPropHelp = "��ע��ֹ��";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.DimStart");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//��ע��ʼ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension X coordinates for the starting point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "��ע��ʼ��X���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimStartX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//��ע��ʼ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension Y coordinates for the starting  point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "��ע��ʼ��Y���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimStartY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//��ע��ֹ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension ending point");
			lpInfo->m_strPropHelp = "dimension ending point";
#else
			lpInfo->m_strPropName.Format("��ע��ֹ��");
			lpInfo->m_strPropHelp = "��ע��ֹ��";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.DimEnd");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//��ע��ֹ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension X coordinates for the ending point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "��ע��ֹ��X���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimEndX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//��ע��ֹ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension Y coordinates for the ending point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "��ע��ֹ��Y���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimEndY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//��עλ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("dimension position");
			lpInfo->m_strPropHelp = "dimension position";
#else
			lpInfo->m_strPropName.Format("��עλ��");
			lpInfo->m_strPropHelp = "��עλ��";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("DIMSIZE.DimPos");
			if(CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CDrawMapScriptDlg::GetPropStatus(pPropItem->m_idProp);
			//��עλ��X����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("X");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension X coordinates for the point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "��עλ��X���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimPosX");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
			//��עλ��Y����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName.Format("Y");
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "dimension Y coordinates for the point, which is relative coordinates in mapping coordinates system.";
#else
			lpInfo->m_strPropHelp = "��עλ��Y���꣬������Ϊ��ͼ����ϵ�µ�������ꡣ";
#endif
			pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp=GetPropID("DIMSIZE.fDimPosY");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !IsPropEqual(pSonPropItem->m_idProp);
		}
	}
	else if(m_pSpreadLine&&IsSpreadLinePropGroup(iCurSel))
	{	//չ��������
		if(m_pSpreadLine->m_iDefType==0)
		{	//��׼�˼� ��׼�˼����Ϊ0ʱ��ʾΪ�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_FILEPATH;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("datum angle");
			lpInfo->m_strPropHelp = "spread line positioning datum angle";
#else
			lpInfo->m_strPropName.Format("��׼�Ǹ�");
			lpInfo->m_strPropHelp = "չ���߶�λ��׼�Ǹ֡�";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_hDatumPole");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			//��׼�˼�Ϊ�Ǹ�ʱ���ø�����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg|";
			lpInfo->m_strPropName.Format("positioning leg");
			lpInfo->m_strPropHelp = "spread line datum angle positioning leg.";
#else
			lpInfo->m_cmbItems="0.X֫|1.Y֫|";
			lpInfo->m_strPropName.Format("��λ֫");
			lpInfo->m_strPropHelp = "չ���߻�׼�Ǹֶ�λ֫��";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_iCurWing");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName.Format("offset");
			lpInfo->m_strPropHelp = "distance between spreading line and angle's ridge";
#else
			lpInfo->m_strPropName.Format("ƫ�ƾ���");
			lpInfo->m_strPropHelp = "չ������Ǹ�����֮��ľ��롣";
#endif
			pPropItem = m_propList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp=GetPropID("SPREAD_LINE.m_fOffsetDist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		}
		//ƫ�����������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "starting coordinates";
		lpInfo->m_strPropHelp = "spread line starting point coordinates";
#else
		lpInfo->m_strPropName = "�������";
		lpInfo->m_strPropHelp = "չ�����������";
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
		lpInfo->m_strPropHelp = "X����,������Ϊȫ������ϵ�µ����ꡣ";
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
		lpInfo->m_strPropHelp = "Y����,������Ϊȫ������ϵ�µ����ꡣ";
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
		lpInfo->m_strPropHelp = "Z����,������Ϊȫ������ϵ�µ����ꡣ";
#endif
		pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp=GetPropID("SPREAD_LINE.startPt.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue.Format("%s",valueStr);
		pSonPropItem->SetReadOnly(m_pSpreadLine->m_iDefType==0);
		//ƫ�����յ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "ending coordinates";
		lpInfo->m_strPropHelp = "spread line ending coordinates";
#else
		lpInfo->m_strPropName = "�յ�����";
		lpInfo->m_strPropHelp = "չ�����յ�����";
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
		lpInfo->m_strPropHelp = "X����,������Ϊȫ������ϵ�µ����ꡣ";
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
		lpInfo->m_strPropHelp = "Y����,������Ϊȫ������ϵ�µ����ꡣ";
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
		lpInfo->m_strPropHelp = "Z����,������Ϊȫ������ϵ�µ����ꡣ";
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
		{	//�����϶��зָ��� 
			HD_NOTIFY* phdn=(HD_NOTIFY*)pNMHdr; 
			if(phdn->iItem==0)	//������һ�п��Ϊ25
				phdn-> pitem->cxy=25;
			else if(phdn->iItem==3)	//���������п��Ϊ45
				phdn-> pitem->cxy=45;
		} 
		else if(pNMHdr->code==HDN_DIVIDERDBLCLICK) 
		{	//����˫���зָ��� 
			HD_NOTIFY* phdn= (HD_NOTIFY*)pNMHdr; 
			if(phdn->iItem==0)	//������һ�п��Ϊ25
				m_listCmdSuperCtrl.SetColumnWidth(0,25);
			else if(phdn->iItem==3)	//���������п��Ϊ45
				m_listCmdSuperCtrl.SetColumnWidth(3,45);
		}	
	} 
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CDrawMapScriptDlg::OnClose() 
{
	CDialog::OnOK();
	if(m_pComponent)	//��ʼ������feature=-1,�����Ի��򱣳�˳��ʱ�õ� wht 11-07-23
		m_pComponent->InitCmdFeature(-1);	
	//CDialog::OnCancel();
}

//�õ�ָ������ָ��ID�������ַ���
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
		{	//��������
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
				sText.Printf("���ƹ���");
			else if(pCmd->m_eCmdType==CMD_DRAW_TABLE)
				sText.Printf("���Ʊ��");
			else if(pCmd->m_eCmdType==CMD_DIM_PARTSPEC)
				sText.Printf("��ע������ϸ");
			else if(pCmd->m_eCmdType==CMD_DIM_SIMPOLY)
				sText.Printf("��ע������");
			else if(pCmd->m_eCmdType==CMD_DIM_ODDMENT)
				sText.Printf("��ע����ͷ");
			else if(pCmd->m_eCmdType==CMD_DIM_FOOTNAIL_SPACE)
				sText.Printf("��ע�Ŷ����");
			else if(pCmd->m_eCmdType==CMD_DIM_FILLPLATE_SPACE)
				sText.Printf("��ע�����");
			else if(pCmd->m_eCmdType==CMD_DIM_BOLTPAD)
				sText.Printf("��ע��˨��Ȧ");
			else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_JGZHUN)
				sText.Printf("��ע�Ǹ�����׼��");
			else if(pCmd->m_eCmdType==CMD_DIM_SPECIAL_LSSPACE)
				sText.Printf("��ע��˨������");
			else if(pCmd->m_eCmdType==CMD_DIM_MAPSIZE)
				sText.Printf("��עͼ��ߴ�");
			else if(pCmd->m_eCmdType==CMD_DIM_MODELSIZE)
				sText.Printf("��עģ�ͳߴ�");
			else if(pCmd->m_eCmdType==CMD_DIM_BOLTINFO)
				sText.Printf("��ע��˨��Ϣ");
			else if(pCmd->m_eCmdType==CMD_DIM_ANGLEJOINT)
				sText.Printf("��ע�Ǹֽ�ͷ");
			else 
				sText.Printf("��������");
#endif
		}
		else if(GetPropID("m_stateCmd")==id)		//����״̬
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
				sText.Printf("��ǰλ��");
			else if(pCmd->m_stateCmd==0x02)
				sText.Printf("�ϵ�");
			else if(pCmd->m_stateCmd==0x03)
				sText.Printf("��ǰλ��&&�ϵ�");
			else if(pCmd->m_stateCmd==0x04)
				sText.Printf("��ǩ");
			else if(pCmd->m_stateCmd==0x05)
				sText.Printf("��ǰλ��&&��ǩ");
			else if(pCmd->m_stateCmd==0x06)
				sText.Printf("�ϵ�&&��ǩ");
			else if(pCmd->m_stateCmd==0x07)
				sText.Printf("��ǰλ��&&��ǩ&&�ϵ�");
			else //if(pCmd->m_stateCmd==0x0)
				sText.Printf("����");
#endif
		}
		else if(GetPropID("m_bExecute")==id)	//�Ƿ�ִ�и�����
		{
			if(pCmd->m_bExecute)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("��");
			else 
				sText.Printf("��");
#endif
		}
		//�������Ʋ���
		else if(GetPropID("PART.pPart")==id)	//�ű���¼��������
		{
			if(pCmd->m_paraPart.pPart)
				sText.Printf("0X%X",pCmd->m_paraPart.pPart->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("PART.angle.bDrawAxisLine")==id)	//�˼�����ʼ�����쳤��
		{
			if(pCmd->m_paraPart.para.angle.bDrawAxisLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("��");
			else
				sText.Copy("��");
#endif
		}
		else if(GetPropID("PART.angle.extendStyle")==id)	//�˼�����ʼ�����쳤��
		{
			if(pCmd->m_paraPart.para.angle.extendStyle==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("1.Expanding Legs");
			else if(pCmd->m_paraPart.para.angle.extendStyle==2)
				sText.Copy("2.Expanding Ridge Line");
			else //if(pCmd->m_paraPart.para.angle.extendStyle==0)
				sText.Copy("0.Expanding On Both Sides");
#else
				sText.Copy("1.��չ֫��");
			else if(pCmd->m_paraPart.para.angle.extendStyle==2)
				sText.Copy("2.��չ����");
			else //if(pCmd->m_paraPart.para.angle.extendStyle==0)
				sText.Copy("0.������չ");
#endif
		}
		else if(GetPropID("PART.fPoleAxisLineStartOutSpreadLen")==id)	//�˼�����ʼ�����쳤��
		{
			sText.Printf("%lf",pCmd->m_paraPart.fPoleAxisLineStartOutSpreadLen);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PART.fPoleAxisLineEndOutSpreadLen")==id)		//�˼������ն����쳤��
		{
			sText.Printf("%lf",pCmd->m_paraPart.fPoleAxisLineEndOutSpreadLen);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PART.bolt.bDrawSideBoltLine")==id)		//�˼������ն����쳤��
		{
			if(pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT&&
				pCmd->m_paraPart.para.bolt.front0_side1==1&&pCmd->m_paraPart.para.bolt.bDrawSideBoltLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("1.Straight Line");
			else
				sText.Copy("0.Picture");
#else
				sText.Copy("1.ֱ��");
			else
				sText.Copy("0.ͼ��");
#endif
		}
		else if(GetPropID("PART.bolt.bDimHoleDiameter")==id)		//�˼������ն����쳤��
		{
			if(pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->m_bVirtualPart&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_BOLT&&
				pCmd->m_paraPart.para.bolt.front0_side1==0&&pCmd->m_paraPart.para.bolt.bDimHoleDiameter)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Yes");
			else
				sText.Copy("No");
#else
				sText.Copy("��");
			else
				sText.Copy("��");
#endif
		}
		else if(GetPropID("PART.cDrawFlag")==id)	//���������� 'X'.X���� 'Y'.Y����,'Z'.Z���� �Զ��ж�
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
				sText.Copy("X����");
			else if(pCmd->m_paraPart.cDrawFlag=='Y')
				sText.Copy("Y����");
			else if(pCmd->m_paraPart.cDrawFlag=='Z')
				sText.Copy("Z����");
			else
				sText.Copy("�Զ��ж�");
#endif
		}
		else if(GetPropID("PART.angle.nXWingDatumAxisG")==id)	//X֫��׼�ľ�
			sText.Printf("%d",pCmd->m_paraPart.para.angle.nXWingDatumAxisG);
		else if(GetPropID("PART.angle.nYWingDatumAxisG")==id)	//Y֫��׼�ľ�
			sText.Printf("%d",pCmd->m_paraPart.para.angle.nYWingDatumAxisG);
		//���Ƹ˼�����
		else if(GetPropID("LINEPART.pLinePart")==id)			//�˼����
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
				sText.Printf("��");
			else 
				sText.Printf("��");
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
				sText.Printf("��");
			else 
				sText.Printf("��");
#endif
		}
		//�����Ʋ���
		else if(GetPropID("TABLE.nLocatePos")==id)			//0.���� 1.���� 2.���� 3.����
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
				sText.Printf("1.����");
			else if(pCmd->m_paraBOMTable.nLocatePos==2)
				sText.Printf("2.����");
			else if(pCmd->m_paraBOMTable.nLocatePos==3)
				sText.Printf("3.����");
			else //if(pCmd->m_paraBOMTable.nLocatePos==0)
				sText.Printf("0.����");
#endif
		}
		else if(GetPropID("TABLE.bSelectPos")==id)			//ѡ����λ��
		{
			if(pCmd->m_paraBOMTable.bSelectPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("��");
			else 
				sText.Printf("��");
#endif
		}
		else if(GetPropID("TABLE.bDisplayDlg")==id)			//��ʾ�Ի���
		{
			if(pCmd->m_paraBOMTable.bDisplayDlg)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("��");
			else 
				sText.Printf("��");
#endif
		}
		else if(GetPropID("TABLE.fDimPosX")==id)			//���λ��X����		
		{
			sText.Printf("%lf",pCmd->m_paraBOMTable.fDimPosX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("TABLE.fDimPosY")==id)			//���λ��Y����
		{
			sText.Printf("%lf",pCmd->m_paraBOMTable.fDimPosY);
			SimplifiedNumString(sText);
		}
		//������ϸ��ע����
		else if(GetPropID("PARTSPEC.pObj")==id)			//��ǰ��ע����
		{
			if(pCmd->m_paraPartSpec.pObj)
				sText.Printf("0X%X",pCmd->m_paraPartSpec.pObj->handle);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("PARTSPEC.fBasePosOffsetX")==id)	//��עλ��X����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fBasePosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fBasePosOffsetY")==id)	//��עλ��Y����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fBasePosOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fDimPosOffsetX")==id)	//��עλ��X����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fDimPosOffsetY")==id)	//��עλ��Y����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fGuigeOffsetX")==id)	//����עλ��X����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fGuigeOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.fGuigeOffsetY")==id)	//����עλ��Y����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fGuigeOffsetY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.iDimPosType")==id)		//0.�Զ�����λ�� 1.ָ��λ��(����ָ��λ����ԭ��עλ�õ�ƫ��ֵ)  2.ѡ��λ��
		{
			if(pCmd->m_paraPartSpec.iDimPosType==2)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("2.Choose");
			else if(pCmd->m_paraPartSpec.iDimPosType==1)
				sText.Printf("1.Specify");
			else //if(pCmd->m_paraPartSpec.iDimPosType==0)
				sText.Printf("0.Search Auto");
#else
				sText.Printf("2.ѡ��λ��");
			else if(pCmd->m_paraPartSpec.iDimPosType==1)
				sText.Printf("1.ָ��λ��");
			else //if(pCmd->m_paraPartSpec.iDimPosType==0)
				sText.Printf("0.�Զ�����");
#endif
		}
		else if(GetPropID("PARTSPEC.bDrawRefLine")==id)	//�Ƿ����������
		{
			if(pCmd->m_paraPartSpec.bDrawRefLine)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("��");
			else 
				sText.Printf("��");
#endif
		}
		else if(GetPropID("PARTSPEC.fPoleDimPosCoef")==id)	//�˼���עλ��ϵ�� //LTMA 0.3  TAS 0.5
		{
			sText.Printf("%lf",pCmd->m_paraPartSpec.fPoleDimPosCoef);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("PARTSPEC.iMirMsg")==id)			//0:X��Գƣ�1:Y��Գ�;���಻�Գƴ���
		{
			if(pCmd->m_paraPartSpec.iMirMsg==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.X Axis Mirror");
			else if(pCmd->m_paraPartSpec.iMirMsg==1)
				sText.Printf("1.Y Axis Mirror");
			else 
				sText.Printf("2.No Mirror");
#else
				sText.Printf("0.X��Գ�");
			else if(pCmd->m_paraPartSpec.iMirMsg==1)
				sText.Printf("1.Y��Գ�");
			else 
				sText.Printf("2.�޶Գ�");
#endif
		}
		else if(GetPropID("PARTSPEC.bDimPoleSpec")==id)		//����ע�˼����
		{
			if(!pCmd->m_paraPartSpec.bOnlyDimPartNo)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("��");
			else 
				sText.Printf("��");
#endif
		}
		//��ע�κ�
		else if(GetPropID("DIMSEGI.bSelectPos")==id)
		{
			if(pCmd->m_paraDimSegI.bSelectPos)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("Yes");
			else 
				sText.Printf("No");
#else
				sText.Printf("��");
			else 
				sText.Printf("��");
#endif
		}
		else if(GetPropID("DIMSEGI.nSegI")==id)		//�����κ�
			sText.Printf("%s",(char*)pCmd->m_paraDimSegI.nSegI.ToString());
		else if(GetPropID("DIMSEGI.fDimPosX")==id)	//��עλ��X����
		{
			sText.Printf("%lf",pCmd->m_paraDimSegI.fDimPosX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSEGI.fDimPosY")==id)	//��עλ��Y����
		{
			sText.Printf("%lf",pCmd->m_paraDimSegI.fDimPosY);
			SimplifiedNumString(sText);
		}
		//����ͷ��ע����
		else if(GetPropID("ODDMENT.pLinePart")==id)			//����ͷ�����˼�
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
				sText.Copy("0.�Զ�����");
			else 
				sText.Copy("1.ָ��λ��");
#endif
		}
		else if(GetPropID("ODDMENT.StartOddPos")==id)			//����ͷ�����˼�
		{
			if(pCmd->m_paraOddment.pLinePart)
			{
				double startOdd=pCmd->m_paraOddment.pLinePart->startOdd();
				if(pCmd->m_paraOddment.pLinePart->IsAngle())
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pCmd->m_paraOddment.pLinePart;
					//����Ǹֵ�ǰ����֫
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
		else if(GetPropID("ODDMENT.EndOddPos")==id)			//����ͷ�����˼�
		{
			if(pCmd->m_paraOddment.pLinePart)
			{
				double endOdd=pCmd->m_paraOddment.pLinePart->endOdd();
				if(pCmd->m_paraOddment.pLinePart->IsAngle())
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pCmd->m_paraOddment.pLinePart;
					//����Ǹֵ�ǰ����֫
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
		//�˼���˨��Ϣ������Ϣ��ע����
		else if(GetPropID("ROD_BOLTORFL_INFO.pLinePart")==id)			//����ͷ�����˼�
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
				sText.Copy("0.�Զ�����");
			else 
				sText.Copy("1.ָ��λ��");
#endif
		}
		else if(GetPropID("ROD_BOLTORFL_INFO.StartOddPosOffset")==id)			//����ͷ�����˼�
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
		else if(GetPropID("ROD_BOLTORFL_INFO.EndOddPosOffset")==id)			//����ͷ�����˼�
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
		//�Ŷ�����ע����
		else if(GetPropID("FOOTNAIL_SPACE.pLinePart")==id)
		{
			if(pCmd->m_paraDimFootNailSpace.pLinePart)
				sText.Printf("0X%X",pCmd->m_paraDimFootNailSpace.pLinePart->handle);
			else 
				sText.Printf("0X0");
		}
		//������ע����
		else if(GetPropID("FILLPLATE_SPACE.pLinePart")==id)
		{
			if(pCmd->m_paraDimFillPlateSpace.pLinePart)
				sText.Printf("0X%X",pCmd->m_paraDimFillPlateSpace.pLinePart->handle);
			else 
				sText.Printf("0X0");
		}
		//��Ȧ��ע����		
		else if(GetPropID("BOLTPAD.pBolt")==id)				//��˨��Ȧ������˨
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
				sText.Copy("0.�Զ�����");
			else 
				sText.Copy("1.ָ��λ��");
#endif
		}
		else if(GetPropID("BOLTPAD.fDimPosOffsetX")==id)	//��עλ��X����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraBoltPad.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("BOLTPAD.fDimPosOffsetY")==id)	//��עλ��Y����ƫ��ֵ
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
		//�Ǹֽ�ͷ��Ϣ
		else if(GetPropID("ANGLEJOINT.pObj")==id)			//��ǰ��ע����
		{
			if(pCmd->m_paraAngleJoint.pJoint)
				sText.Printf("0X%X",pCmd->m_paraAngleJoint.idAngleJoint);
			else 
				sText.Printf("0X0");
		}
		else if(GetPropID("ANGLEJOINT.iDimPosType")==id)		//0.�Զ�����λ�� 1.ָ��λ��
		{
			if(pCmd->m_paraAngleJoint.iDimPosType==1)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("1.Specify");
			else 
				sText.Printf("0.Search Auto");
#else
				sText.Printf("1.ָ��λ��");
			else 
				sText.Printf("0.�Զ�����");
#endif
		}
		else if(GetPropID("ANGLEJOINT.fDimPosOffsetX")==id)	//��עλ��X����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraAngleJoint.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("ANGLEJOINT.fDimPosOffsetY")==id)	//��עλ��Y����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraAngleJoint.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		//��˨��ע����		
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
		{	//��˨��ע��Ϣ��id�������Թ�ϣ�����ֵʱΪBOLTINFOָ��
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
				sText.Copy("0.�Զ�����");
			else 
				sText.Copy("1.ָ��λ��");
#endif
		}
		else if(GetPropID("BOLTINFO.sLabel")==id)
			pCmd->m_paraBoltGroupInfo.lsGroupDim.GetDimText(sText);
		else if(GetPropID("BOLTINFO.fDimPosOffsetX")==id)	//��עλ��X����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraBoltGroupInfo.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("BOLTINFO.fDimPosOffsetY")==id)	//��עλ��Y����ƫ��ֵ
		{
			sText.Printf("%lf",pCmd->m_paraBoltGroupInfo.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		//�����α�ע����
		else if(GetPropID("SIMPOLY.fScale")==id)			//ָ�������α���
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fScale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fPickPtX")==id)			//������ʰȡ��X����		
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fPickPtX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fPickPtY")==id)			//������ʰȡ��Y����
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fPickPtY);	
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fDimPosOffsetX")==id)	//������X�᷽��ƫ����		
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fDimPosOffsetX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SIMPOLY.fDimPosOffsetY")==id)	//������Y�᷽��ƫ����
		{
			sText.Printf("%lf",pCmd->m_paraSimPoly.fDimPosOffsetY);
			SimplifiedNumString(sText);
		}
		//��עģ�ͳߴ�
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
				sText.Printf("0.�ڼ䴹��");
			else if(pCmd->m_paraDimModelSize.iDimType==1)
				sText.Printf("1.�ᵣ��ֱ���");
			else if(pCmd->m_paraDimModelSize.iDimType==2)
				sText.Printf("2.�ֶμ��");
			else if(pCmd->m_paraDimModelSize.iDimType==3)
				sText.Printf("3.����");
			else if(pCmd->m_paraDimModelSize.iDimType==4)
				sText.Printf("4.���¿���");
			if(pCmd->m_paraDimModelSize.iDimType==5)
				sText.Printf("5.�ᵣˮƽ���");
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
		//ͼ��ߴ�
		else if(GetPropID("DIMSIZE.fDimStartX")==id)	//ͼ��ߴ��ע��ʼ��(�������ϵ��)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimStartX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimStartY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimStartY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimEndX")==id)		//ͼ��ߴ��ע��ʼ��(�������ϵ��)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimEndX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimEndY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimEndY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimPosX")==id)		//ͼ��ߴ��עλ��(�������ϵ��)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimPosX);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fDimPosY")==id)
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fDimPosY);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.fScale")==id)		//�ߴ����
		{
			sText.Printf("%lf",pCmd->m_paraDimSize.fScale);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("DIMSIZE.iDimDirect")==id)	//��ע���� 0.X�᷽�� 1.Y�᷽�� 2.���췽��
		{
			if(pCmd->m_paraDimSize.iDimDirect==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.Axis X");
			else if(pCmd->m_paraDimSize.iDimDirect==1)
				sText.Printf("1.Axis Y");
			else 
				sText.Printf("2.Extend Direction");
#else
				sText.Printf("0.X�᷽��");
			else if(pCmd->m_paraDimSize.iDimDirect==1)
				sText.Printf("1.Y�᷽��");
			else 
				sText.Printf("2.���췽��");
#endif
		}
		//��ע�Ǹ�����׼��
		else if(GetPropID("DIM_JGZHUN.pLineAngle")==id)	//��Ҫ��ע����׼�ݵĽǸ�
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
				sText.Copy("�ڲ�");
			else 
				sText.Copy("���");
#endif
		}
		//��ע��˨������
		else if(GetPropID("DIM_LSSPACE.pPlate")==id)			//��Ҫ��ע������˨���ĸְ��Ǹ�
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
				sText.Copy("��");
			else 
				sText.Copy("��");
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
				sText.Copy("�ڲ�");
			else 
				sText.Copy("���");
#endif
		}
		//�Ŷ�����ע
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
		//������ע
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
		//�����α�ע
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
	{	//չ����
		if(GetPropID("SPREAD_LINE.m_iDefType")==id)	//0.ָ���˼� 1.ֱ��ָ��
		{
			if(m_pSpreadLine->m_iDefType==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.Specify Rod");
			else 
				sText.Printf("1.Directly Specify");
#else
				sText.Printf("0.ָ���˼�");
			else 
				sText.Printf("1.ֱ��ָ��");
#endif
		}
		else if(GetPropID("SPREAD_LINE.m_sLineName")==id)	//չ��������
			sText.Printf("%s",(char*)m_pSpreadLine->m_sLineName);
		else if(GetPropID("SPREAD_LINE.m_hDatumPole")==id)	//��׼�˼� ��׼�˼����Ϊ0ʱ��ʾΪ�Զ���
			sText.Printf("0X%X",m_pSpreadLine->m_hDatumPole);
		else if(GetPropID("SPREAD_LINE.m_iCurWing")==id)	//��׼�˼�Ϊ�Ǹ�ʱ���ø�����
		{
			if(m_pSpreadLine->m_iCurWing==0)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Printf("0.X Leg");
			else 
				sText.Printf("1.Y Leg");
#else
				sText.Printf("0.X֫");
			else 
				sText.Printf("1.Y֫");
#endif
		}
		else if(GetPropID("SPREAD_LINE.m_fOffsetDist")==id)	//ƫ�ƾ���
		{
			sText.Printf("%lf",m_pSpreadLine->m_fOffsetDist);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.startPt.x")==id)		//չ�������X����
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.startPt.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.startPt.y")==id)		//չ�������Y����
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.startPt.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.startPt.z")==id)		//չ�������Z����
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.startPt.z);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.endPt.x")==id)		//չ�����յ�X����
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.endPt.x);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.endPt.y")==id)		//չ�����յ�Y����
		{
			sText.Printf("%lf",m_pSpreadLine->m_lineSpread.endPt.y);
			SimplifiedNumString(sText);
		}
		else if(GetPropID("SPREAD_LINE.endPt.z")==id)		//չ�����յ�Z����
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
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
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
		strcpy(prop_str,"û�ҵ���Ϊ\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"������ID��";
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
	{	//�ṹͼ��1��2������ҳ �� ����ͼ��1������ҳʹ��CListCtrl
		GetDlgItem(IDC_CMD_LIST_SUPER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMD_LIST_COMMON)->ShowWindow(SW_SHOW);
		//m_listCmdCommonCtrl.RedrawItems(-1,-1);
		m_listCmdCommonCtrl.InitSortHeadCtrl();
		if(m_iOldGroup!=iListGroup)	//�л�����ҳʱ����������б�
			DisplayCommonCtrlCmdList(iListGroup);
		m_bSuperListCtrlInUsing=FALSE;	//������ͨListCtrl wht 12-03-22
	}
	else 	
	{
		GetDlgItem(IDC_CMD_LIST_SUPER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CMD_LIST_COMMON)->ShowWindow(SW_HIDE);
		m_listCmdSuperCtrl.m_iPropGroup=iListGroup;
		//�޸�����
		long col_wide_arr[4]={25,65,160,45};
		m_listCmdSuperCtrl.EmptyColumnHeader();
		if(IsSpreadLinePropGroup(iListGroup))	//չ����
		{
#ifdef AFX_TARG_ENU_ENGLISH
			m_listCmdSuperCtrl.AddColumnHeader("��");
			m_listCmdSuperCtrl.AddColumnHeader("dimension");
			m_listCmdSuperCtrl.AddColumnHeader("spreading line");
			m_listCmdSuperCtrl.AddColumnHeader("operation");
#else
			m_listCmdSuperCtrl.AddColumnHeader("��");
			m_listCmdSuperCtrl.AddColumnHeader("��ʶ");
			m_listCmdSuperCtrl.AddColumnHeader("չ����");
			m_listCmdSuperCtrl.AddColumnHeader("����");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			m_listCmdSuperCtrl.AddColumnHeader("��");
			m_listCmdSuperCtrl.AddColumnHeader("dimension");
			m_listCmdSuperCtrl.AddColumnHeader("drawing items");
			m_listCmdSuperCtrl.AddColumnHeader("enable");
#else
			m_listCmdSuperCtrl.AddColumnHeader("��");
			m_listCmdSuperCtrl.AddColumnHeader("��ʶ");
			m_listCmdSuperCtrl.AddColumnHeader("������");
			m_listCmdSuperCtrl.AddColumnHeader("����");
#endif
		}
		m_bFireListItemChange=FALSE;	//�ص�ItemChange������������Ƶ������FireItemChanged wjh-2011.8.22
		m_listCmdSuperCtrl.InitListCtrl(col_wide_arr);
		//�ڲ�������ͬ���͵�������֮������л�ʱ��Ҫ����������
		if(m_bInernalStart||(IsSpreadLinePropGroup(m_iOldGroup)&&!IsSpreadLinePropGroup(iListGroup))
			||(!IsSpreadLinePropGroup(m_iOldGroup)&&IsSpreadLinePropGroup(iListGroup)))
			DisplayPropertyList(iListGroup);
		m_listCmdSuperCtrl.Redraw();
		m_bFireListItemChange=TRUE;		//���´�ItemChange������
		m_bSuperListCtrlInUsing=TRUE;	//����SuperListCtrl wht 12-03-22
	}
	m_iOldGroup=iListGroup;
}

void CDrawMapScriptDlg::OnSelchangeTabCmdType(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//�л�����ҳʱ��յ�ǰ�����Լ���ǰչ����
	m_pCurCmd=NULL;
	m_pSpreadLine=NULL;
	//ȡ������ѡ����
	m_listCmdCommonCtrl.SetItemState(-1,0,LVIS_SELECTED);
	RefreshCommandList(m_tabCmdType.GetCurSel());
	*pResult = 0;
}

//���ش���
void CDrawMapScriptDlg::HideWnd(BOOL bRestart)
{
	int i=0,n=m_listCmdCommonCtrl.GetItemCount();
	for(i=0;i<m_listCmdCommonCtrl.GetItemCount();i++)
	{
		CDrawingCommand *pCmd=(CDrawingCommand*)m_listCmdCommonCtrl.GetItemData(i);
		if(pCmd)	//���ش���ʱ��¼������������������������ʱ�������ֵ�ǰ˳�� wht 11-07-23
			pCmd->feature=i;
	}
	m_iBreakType=3;
	m_bInernalStart=bRestart;
	CDialog::OnCancel();
}

//ѡ�����ڵ����
void CDrawMapScriptDlg::SelectEntObj(int nResultEnt/*=1*/)
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_iBreakType=1;	//ʰȡ��
	m_nResultEnt=nResultEnt;
	CDialog::OnOK();
}

//�õ���һ����ִ�е�������
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
			continue;	//�������Ҫִ��
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

//�õ����һ����ִ�еĵ�������
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
			pLastExecutedItem=pItem;	//�������Ҫִ�в���������
		else
			break;
	}
	return pLastExecutedItem;
}

void CDrawMapScriptDlg::OnOK()
{
	//���λس���
}

//���ݹؼ��ֲ����ƶ���Ŀ
void CDrawMapScriptDlg::OnChangeKeyWord() 
{
	UpdateData();
	if(m_sKeyWord.GetLength()<=0)
		return;
	int iGroup=m_tabCmdType.GetCurSel();
	if(iGroup==0)
	{	//ȡ������ѡ�����ѡ��״̬
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
			//�Ƚ��ַ����Ƿ���ȫ���
			if(m_sKeyWord.CompareNoCase(sId)==0||m_sKeyWord.CompareNoCase(sPartNo)==0
				||m_sKeyWord.CompareNoCase(sHelp)==0||ss.CompareNoCase(sId)==0)
			{
				m_listCmdCommonCtrl.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
				m_listCmdCommonCtrl.EnsureVisible(i,FALSE);	//ȷ��ѡ����ɼ�
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
				continue;	//�����ڵ�ǰ����
			CDrawingCommand *pCmd=(CDrawingCommand*)pItem->m_idProp;
			if(pCmd==NULL)
				continue;
			char sId[50]="",sPartNo[50]="",sHelp[200]="";
			pCmd->GetCmdInfo(sId,sHelp,sPartNo);
			CString ss;
			ss.Format("0x%s",m_sKeyWord);
			//�Ƚ��ַ����Ƿ���ȫ���
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
				m_listCmdCommonCtrl.SetItemText(iCurSel,5,CString("��"));
#else
				m_listCmdCommonCtrl.SetItemText(iCurSel,5,CString("��"));
#endif
				//���������Ӧ��ʵ�� wht 11-07-23
				pCmd->Rerun();
				if(pCmd->m_eCmdType==CMD_DRAW_PART)
				{	//ͬʱ���ɱ�ע������ϸ�����Ӧ��ʵ��
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
				//ɾ�������Ӧ��ʵ�� wht 11-07-23
				pCmd->EmptyEntList();
				if(pCmd->m_eCmdType==CMD_DRAW_PART)
				{	//���û��ƹ�������ʱͬʱɾ����ע������ϸ�����Ӧ��ʵ��
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
			continue;	//�����ڵ�ǰ���� wht 10-12-02
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
			m_listCmdSuperCtrl.SetSubItemText(pItem,3,CString("��"));
#else
			m_listCmdSuperCtrl.SetSubItemText(pItem,3,CString("��"));
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

//ִ��ָ������ 3.����˼����� 4.��Ӹ����� 5.���������
void CDrawMapScriptDlg::ExecutableCommand(int iOperType)
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_iBreakType=3;	//ִ������
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
	{	//������չ����ʾģʽΪ34
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//��ʾ�� .
	#else
		acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//��ʾ�� .
	#endif
		m_bPdmodeIs34=FALSE;
	}
	//ȡ��������ʾʵ�� wht 12-03-22
	CDrawingCommand::ReleaseHighlightEnts();
}
void CDrawMapScriptDlg::OnItemchangedCmdListCommon(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->uNewState==pNMListView->uOldState)
		return;	//״̬δ�����仯ʱֱ�ӷ��� wht 11-07-23
	if(!m_bFireListItemChange)
		return;	//δ����ItemChange
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
			m_pCurCmd=*(cmdList.GetFirst());//��ǰ����Ϊ������е�һ��ѡ�����Ӧ������		
		DisplayPropertyList(iCurGroup);	//����������
	}
	if(m_pCurCmd)
	{	//��ǰ����Ϊģ�ͳߴ��עʱ�л�������չ����ʾģʽ���ڲ鿴�ڵ� wht 11-07-13
		/*if(m_pCurCmd->m_eCmdType==CMD_DIM_MODELSIZE)
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"34",RTNONE);//��ʾ�� X
			//acedCommand(RTSTR,"REGEN",RTNONE);
			m_bPdmodeIs34=TRUE;	//������չ����ʾģʽΪ34 wht 11-07-15
		}
		else if(m_bPdmodeIs34)
		{
			acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//��ʾ�� .
			//acedCommand(RTSTR,"REGEN",RTNONE);
			m_bPdmodeIs34=FALSE;
		}*/
		//�����������Ӧ��ʵ��
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

//�������ֵ�Ƿ����
BOOL CDrawMapScriptDlg::IsPropEqual(long idProp,int iSonCmdType/*=-1*/)	
{
	CDrawingCommand **ppCmd=cmdList.GetFirst();
	if(cmdList.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	GetPropValueStr(*ppCmd,idProp,valueStr);
	if(iSonCmdType==1&&(*ppCmd)->m_eCmdType==CMD_DRAW_PART)
	{	//��ע������ϸ�����Ӧ������
		CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
		if(ppSonCmd)
			GetPropValueStr(*ppSonCmd,idProp,valueStr);
	}
	else if(iSonCmdType==2&&(*ppCmd)->m_eCmdType==CMD_DIM_PARTSPEC)
	{	//��ע�˼���˨������Ϣ�����Ӧ������
		CDrawingCommand **ppSonCmd=hashRodBoltOrFlInfoList.GetValue((*ppCmd)->m_paraPartSpec.hObj);
		if(ppSonCmd)
			GetPropValueStr(*ppSonCmd,idProp,valueStr);
	}
	int nPush=cmdList.push_stack();
	for(ppCmd=cmdList.GetNext();ppCmd;ppCmd=cmdList.GetNext())
	{
		char valueStr2[500]="";
		if(iSonCmdType==1&&(*ppCmd)->m_eCmdType==CMD_DRAW_PART)
		{	//��ע������ϸ�����Ӧ������
			CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue((*ppCmd)->m_paraPart.hPart);
			if(ppSonCmd)
				GetPropValueStr(*ppSonCmd,idProp,valueStr2);
		}
		else if(iSonCmdType==2&&(*ppCmd)->m_eCmdType==CMD_DIM_PARTSPEC)
		{	//��ע�˼���˨������Ϣ�����Ӧ������
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
		{	//���̸�Ϊ��
			pCmd->m_bExecute=FALSE;
			m_listCmdCommonCtrl.SetItemText(ht.iItem,ht.iSubItem,CString(" "));
			//ɾ�������Ӧ��ʵ�� wht 11-07-23
			pCmd->EmptyEntList();
			if(pCmd->m_eCmdType==CMD_DRAW_PART)
			{	
				CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//���û��ƹ�������ʱͬʱɾ����ע������ϸ�����Ӧ��ʵ��
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
					//����������
					long idProp=CDrawMapScriptDlg::GetPropID("m_bExecute");
					CPropTreeItem *pFindItem=m_propList.FindItemByPropId(idProp,NULL);
					if(*ppSonCmd&&pFindItem)	//���±�ע������ϸ����
					{
						char sText[100]="";
						UpdatePartSpecPropertyList(pFindItem,*ppSonCmd,TRUE,TRUE);
						if(GetPropValueStr(idProp,sText)>0)
							m_propList.SetItemPropValue(idProp,sText);
					}
				}
				ppSonCmd=hashOddmentList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)	
				{	//���ù���������ͷ��ע���ɾ�����������ʵ��
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
				}
				ppSonCmd=hashFootNailSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//���ù����ĽŶ�����ע���ɾ�����������ʵ��
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//������������
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem," ");
							break;
						}
					}
				}
				ppSonCmd=hashFillPlateSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//���ù�����������ע���ɾ�����������ʵ��
					(*ppSonCmd)->EmptyEntList();
					(*ppSonCmd)->m_bExecute=FALSE;
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//������������
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem," ");
							break;
						}
					}
				}
			}
		}
		else if(pCmd&&!pCmd->m_bExecute)
		{	//������Ϊ��
			pCmd->m_bExecute=TRUE;
			m_listCmdCommonCtrl.SetItemText(ht.iItem,ht.iSubItem,CString("��"));
			//���������Ӧ��ʵ�� wht 11-07-23
			pCmd->Rerun();
			if(pCmd->m_eCmdType==CMD_DRAW_PART)
			{	
				CDrawingCommand **ppSonCmd=hashPartSpecList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//ͬʱ���ɱ�ע������ϸ�����Ӧ��ʵ��
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
					//����������
					long idProp=CDrawMapScriptDlg::GetPropID("m_bExecute");
					CPropTreeItem *pFindItem=m_propList.FindItemByPropId(idProp,NULL);
					if(*ppSonCmd&&pFindItem)	//���±�ע������ϸ����
					{
						char sText[100]="";
						UpdatePartSpecPropertyList(pFindItem,*ppSonCmd,TRUE,TRUE);
						if(GetPropValueStr(idProp,sText)>0)
							m_propList.SetItemPropValue(idProp,sText);
					}
				}
				ppSonCmd=hashOddmentList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)	
				{	//���ù���������ͷ��ע����������и��������ʵ��
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
				}
				ppSonCmd=hashFootNailSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//���ù����ĽŶ�����ע����������и��������ʵ��
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//������������
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem,"��");
							break;
						}
					}
				}
				ppSonCmd=hashFillPlateSpaceList.GetValue(pCmd->m_paraPart.hPart);
				if(ppSonCmd)
				{	//���ù�����������ע����������и��������ʵ��
					(*ppSonCmd)->m_bExecute=TRUE;
					(*ppSonCmd)->Rerun();
					int n=m_listCmdCommonCtrl.GetItemCount();
					for(int i=0;i<n;i++)
					{
						DWORD dwData=m_listCmdCommonCtrl.GetItemData(i);
						if(dwData==(DWORD)(*ppSonCmd))
						{	//������������
							m_listCmdCommonCtrl.SetItemText(i,ht.iSubItem,"��");
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
	pMenu->AppendMenu(MF_STRING,ID_ENABLE_SELECTED_ITEM,"����ѡ����");
	pMenu->AppendMenu(MF_STRING,ID_DISABLE_SELECTED_ITEM,"����ѡ����");
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
			pMenu->AppendMenu(MF_STRING,ID_ADJUST_DRAWING_SERIAL_NO,"��������˳��");
#endif
			//pMenu->AppendMenu(MF_STRING,ID_MOVE_AFTER,"�ƶ���ָ������֮��");
			//pMenu->AppendMenu(MF_STRING,ID_MOVE_BEFORE,"�ƶ���ָ������֮ǰ");
			if(pCmd->m_eCmdType==CMD_DRAW_PART&&pCmd->m_paraPart.pPart&&pCmd->m_paraPart.pPart->GetClassTypeId()==CLS_PLATE)
				pMenu->AppendMenu(MF_STRING,ID_ADD_ITEM,"�����˨����ע");
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
	//ˢ�½���
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
	//�����˨��ע����
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
	//��ʾ��˨��ע�������
	int iItem=m_listCmdCommonCtrl.GetSelectedItem();
	iItem=InsertCmdToListCtrl(0,pCmd,iItem+1);
	m_listCmdCommonCtrl.SetItemState(-1,0,LVIS_SELECTED);
	m_listCmdCommonCtrl.SetItemState(iItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_listCmdCommonCtrl.EnsureVisible(iItem,FALSE);	//ȷ��ѡ����ɼ� wht 11-07-25
}
#endif
