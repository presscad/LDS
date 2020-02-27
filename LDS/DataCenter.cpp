// MainFrm.cpp : implementation of the CMainFrame class
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "UserDefMsg.h"
#include "Tower.h"
#include "direct.h"
#include "database.h"
#include "Query.h"
#include "MsgDef.h"
#include "SendMsgDlg.h"
#include "dlg.h"
#include "TowerTreeDlg.h"
#include "TowerPropertyDlg.h"
#include "CmdLineDlg.h"
#include "PartLib.h"
#include "DelUndisplayPartsDlg.h"
#include "SortFunc.h"
#include "SysParametersDlg.h"
#include "PartNoListDlg.h"
#include "OutputInfoDlg.h"
#include "UserDefCmdGroupDlg.h"
#include "EmendBoltGasket.h"
#include "LogFile.h"
#include "list.h"
#include "OriginalData.h"
#include "DataCheckDlg.h"
#include "SnapTypeVerify.h"
#include "CheckPartDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
#ifdef __COMMON_PART_INC_
//INFO_CHK_NULLPARTNO 构件号检查共用同一个函数
//根据段号过滤杆件
int GetLinePartSetBySeg(LINEPARTSET &linePartSet,CHashList<SEGI>& segTbl)
{
	int nLinePartNum=0;
	for(CLDSLinePart* pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pLinePart->iSeg.iSeg)==NULL)
			continue;
		linePartSet.append(pLinePart);
		nLinePartNum++;
	}
	return nLinePartNum;
}
int GetPartSetBySeg(PARTSET &partSet,int cls_id,CHashList<SEGI>& segTbl)
{
	int nPartNum=0;
	for(CLDSPart* pPart=Ta.Parts.GetFirst(cls_id);pPart;pPart=Ta.Parts.GetNext(cls_id))
	{
		if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
			continue;
		partSet.append(pPart);
		nPartNum++;
	}
	return nPartNum;
}
int GetNodeSetBySeg(NODESET &nodeSet,CHashList<SEGI>& segTbl)
{
	int nNodeNum=0;
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pNode->iSeg.iSeg)==NULL)
			continue;
		nodeSet.append(pNode);
		nNodeNum++;
	}
	return nNodeNum;
}
//获取影射构件的母构件列表
static void GetMotherPartHashList(CLDSPart* pShadowPart,CHashList<CLDSPart*> &motherHashList)
{
	CLDSPart* pMotherPart = pShadowPart->MotherPart(false);
	while(pMotherPart->IsShadowObject())
	{
		motherHashList.SetValue(pShadowPart->handle,pMotherPart);
		pShadowPart=pMotherPart;
		pMotherPart=pShadowPart->MotherPart(false);
	}
	if(pShadowPart->IsShadowObject())
		motherHashList.SetValue(pShadowPart->handle,pMotherPart);
}
//影射构件与母构件的比较
static BOOL IsEqualShadowAndMother(CLDSPart* pShadowPart,CLDSPart* pMotherPart,char* sError,double epsilon,long &hFirPart,long &hSecPart)
{	
	//1 比较螺栓数
	hFirPart=pShadowPart->handle;
	CLsRefList *pLsRefList=pShadowPart->GetLsRefList();
	if(pLsRefList==NULL||pShadowPart->GetLsCount()>pMotherPart->GetLsCount())
	{	//影射构件上的显示引用螺栓孔数可以少于母构件,因为子构件是母构件的影射,此种情况仅代表可能存在一些螺栓未引用过来不存在冲突 wjh-2013.1.27
		hSecPart=pMotherPart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sError,"The bolt number are different between 0X%X and 0X%X{%s}!",hFirPart,hSecPart,(char*)pShadowPart->GetPartNo());
#else 
		sprintf(sError,"0X%X与0X%X{%s}构件的螺栓数不同!",hFirPart,hSecPart,(char*)pShadowPart->GetPartNo());
#endif
		return FALSE;
	}
	//2 比较螺栓对位
	for(CLsRef* pShadowLsRef=pLsRefList->GetFirst();pShadowLsRef;pShadowLsRef=pLsRefList->GetNext())
	{	//根据螺栓的定位方式进行判断(定位方式不可能是CS_PARAM)
		CLDSBolt* pShadowBolt=pShadowLsRef->GetLsPtr();
		hSecPart=pShadowBolt->handle;
		if(pShadowBolt->GetShadowObjType()==CLDSObject::SHADOW_ISOLATED && pShadowPart->GetShadowObjType()==CLDSObject::SHADOW_ISOLATED)     
		{
			CParaCS *pBoltAcs=pShadowBolt->GetParaCS();
			CParaCS *pHostPartAcs=pShadowPart->GetParaCS();
			CParaCS::PARAMETRIC_INFO bolt_param=pBoltAcs->GetParamInfo();
			CParaCS::PARAMETRIC_INFO host_param=pHostPartAcs->GetParamInfo();
			if(pBoltAcs->GetDefStyle()==CParaCS::CS_MIRTRANS&&pHostPartAcs->GetDefStyle()==CParaCS::CS_MIRTRANS)  //对称旋转
			{	//比较对称方式或旋转角度
				if(!bolt_param.m_xMirInfo.IsSameMirMsg(host_param.m_xMirInfo))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Symmetric mode or rotation Angle are different between 0x%X and 0x%x .",hFirPart,hSecPart);
#else 
					sprintf(sError,"0x%X与0x%x的对称方式或旋转角度不同",hFirPart,hSecPart);
#endif
					return FALSE;
				}
			}
			else if(pBoltAcs->GetDefStyle()==CParaCS::CS_TRANSLATE&&pHostPartAcs->GetDefStyle()==CParaCS::CS_TRANSLATE)	//刚体平移
			{	//比较平移量
				normalize(bolt_param.desAxisX.vector);
				normalize(host_param.desAxisX.vector);
				f3dPoint bolt_offset=bolt_param.desAxisX.vector*bolt_param.m_xMirInfo.origin.x;
				f3dPoint host_offset=host_param.desAxisX.vector*host_param.m_xMirInfo.origin.x;
				if(!bolt_offset.IsEqual(host_offset,epsilon))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"The offset are different between 0x%X and 0x%X!",hFirPart,hSecPart);
#else 
					sprintf(sError,"0x%X与0x%X的平移量不同!",hFirPart,hSecPart);
#endif
					return FALSE;
				}
			}
			else if(pBoltAcs->GetDefStyle()==CParaCS::CS_PARAM)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"The 0x%X bolt's locate mode of 0x%X  is wrong!",hSecPart,hFirPart);
#else 
				sprintf(sError,"0x%X上的螺栓0x%X定位方式不对!",hFirPart,hSecPart);
#endif
				return FALSE;
			}
		}
		else if(pShadowBolt->GetShadowObjType()==CLDSObject::SHADOW_ISOLATED_RELA_LOCATION)
		{	//螺栓依附其他构件来定位
			CLDSPart* pMotherBolt=pShadowBolt->MotherPart(false);
			CLsRefList *pLocalLsRefList=pMotherPart->GetLsRefList();
			if(pLocalLsRefList&&pLocalLsRefList->FromHandle(pMotherBolt->handle)==NULL)
			{
				hSecPart=pMotherPart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"The reference bolts of 0x%X doesn't match with the bolt hole of 0x%X",hFirPart,hSecPart);
#else 
				sprintf(sError,"0x%X上的螺栓引用与0x%X上的螺栓孔不对位!",hFirPart,hSecPart);
#endif
				return FALSE;
			}
		}
	}
	return TRUE;
}
//影射构件与始祖级构件的比较
static BOOL IsEqualShadowAndAncestor(CLDSPart* pShadowPart,char* sError,double epsilon,long& hFirPart,long& hSecPart)
{
	CHashList<CLDSPart*> motherPartHash;
	GetMotherPartHashList(pShadowPart,motherPartHash);
	if(motherPartHash.GetNodeNum()<=0)
		return FALSE;
	for(CLDSPart** ppMotherPart=motherPartHash.GetFirst();ppMotherPart;ppMotherPart=motherPartHash.GetNext())
	{
		CLDSPart* pPart=Ta.Parts.FromHandle(motherPartHash.GetCursorKey());
		if(!IsEqualShadowAndMother(pPart,(*ppMotherPart),sError,epsilon,hFirPart,hSecPart))
			return FALSE;
	}
	return TRUE;
}
//INFO_EMEND_SINGULAR_BOLTLO 修正螺栓通厚
void ExecuteEmendBoltL0Para(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd= pContext!=NULL?(CMainFrame*)pContext->m_pMainWnd:NULL;
	CTaskOutputList* pInfoList= pContext!=NULL?(CTaskOutputList*)pContext->m_pOutputInfoList:NULL;

	if (dwParam != NULL)
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Revise bolt's L0");
#else 
	DisplayProcess(0,"修正螺栓通厚");
#endif
	CUndoOperObject undo(&Ta,true);
	long hCurBolt=0;
	try{
		CLDSBolt *pBolt;
		CHashTable<double>hashL0,hashL;
		hashL0.CreateHashTable(Ta.Parts.GetNodeNum(CLS_BOLT));
		hashL.CreateHashTable(Ta.Parts.GetNodeNum(CLS_BOLT));
		for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			pBolt->feature=0;	//缺少此行将导致后续代码重复添加螺栓通厚 wjh-2016.6.14
			if(pContext!=NULL&&pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pBolt->iSeg))
				continue;	//判断是否在指定查找段号范围内
			hashL0.SetValueAt(pBolt->handle,pBolt->get_L0());
			hashL.SetValueAt(pBolt->handle,pBolt->get_L());
			if(strlen(pBolt->statSegStr)==0)	//个别情况可以通过设定统计段号范围来阻止自动修订螺栓通厚 wjh-2016.9.2
			{
				pBolt->feature=1;	//需要修正
				pBolt->EmptyL0DesignPara();
			}
			else
				pBolt->feature=0;	//多接腿轮换螺栓不参预修正
		}
		for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			CLDSParamPlate *pXEnd=NULL;
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				pXEnd=(CLDSParamPlate*)pPart;
				if(pXEnd->m_iParamType!=TYPE_XEND)
					pXEnd=NULL;
			}
			CLsRefList *pLsRefList=pPart->GetLsRefList();
			if(pLsRefList==NULL)
				continue;
			for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				if(pLsRef->GetLsPtr()->feature==0)
					continue;
				if(pXEnd)
				{	//跳过十字插板上不合理螺栓 wht 12-06-06
					double L=0;
					CLDSBolt *pBolt=pLsRef->GetLsPtr();
					f3dPoint ls_pos=pBolt->ucs.origin;
					coord_trans(ls_pos,pXEnd->ucs,FALSE);
					pXEnd->GetDesignItemValue('L',&L);
					if(fabs(pBolt->get_norm()*pXEnd->ucs.axis_z)<EPS_COS2)
						continue;
					if(ls_pos.x>L)
						continue;
				}
				hCurBolt=pLsRef->GetLsPtr()->handle;
				pLsRef->GetLsPtr()->AddL0Thick(pPart->handle,TRUE);
			}
		}
		//赋值塔身上螺栓（接腿轮换螺栓均应归为塔身螺栓）的相应通厚优先计算参数子项
		for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pBolt->IsLegObj()||pBolt->feature==0)
				continue;
			int j=0;
			//如果螺栓穿过接腿构件，仅通过pPriorModule无法筛选同时穿过多接腿包铁的情况，
			//此里需要指定某一接腿构件为全局优先构件 wjh-2019.11.20
			CLDSModule* pPriorModule=NULL;
			CLDSPart* pPriorLegPart=NULL;
			for(THICK_ITEM item=pBolt->GetL0ThickPara()->GetFirstItem();!item.IsFinalKey();item=pBolt->GetL0ThickPara()->GetNextItem(),j++)
			{
				if(!item.IsHandle())
					continue;
				BOOL pushed=Ta.Parts.push_stack();
				CLDSPart* pPart=Ta.Parts.FromHandle(abs(item.val));
				Ta.Parts.pop_stack(pushed);
				if(pPart==NULL)
					continue;
				if(pPriorModule==NULL){
					for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
					{
						if(pModule->IsSonPart(pPart))
							break;
					}
					if(Ta.Module.GetNodeNum()>1)
						pPriorModule=pModule;
					else if(pModule&&pModule->GetLegCount()>1)
						pPriorModule=pModule;
				}
				if(pPriorModule&&pPriorModule->IsSonPart(pPart))
				{
					item.cItemType=THICK_ITEM::PART_THROUGHnT;	//优先通厚计算子项
					if (pPart->IsLegObj())
					{
						if (pPriorLegPart==NULL)
							pPriorLegPart=pPart;
						else if (!pPriorLegPart->ModuleTogetherWith(pPart))
							item.cItemType=THICK_ITEM::PART_THROUGH;	//与当前激活接腿不共存
					}
					pBolt->GetL0ThickPara()->SetItem(j,item);
				}
			}
		}
		int i=0,nBoltNum=Ta.Parts.GetNodeNum(CLS_BOLT);
		//long hActiveModule=Ta.m_hActiveModule;
		CLDSModule* pActiveModule=Ta.GetActiveModule();
		for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			i++;
			if(dwParam != NULL)
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((i*100)/nBoltNum,"Revise bolt's L0");
#else 
			DisplayProcess((i*100)/nBoltNum,"修正螺栓通厚");
#endif
			if(pBolt->m_bVirtualPart||pBolt->feature==0)
				continue;	//虚拟螺栓仅为打孔用，无实际统计意义，因此也无修正必要wjh-2011.9.3
			else if(pContext!=NULL&&pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pBolt->iSeg))
				continue;	//判断是否在指定查找段号范围内
			BOOL pushed=Ta.Parts.push_stack();
			BOOL bRet=pBolt->CalGuigeAuto();
			//因多接腿轮换螺栓已启用自动修订，故此项临时措施也应一并取消　wjh-2015.4.10
			//BOOL bRet=FALSE;
			//if(pActiveModule&&pActiveModule->IsSonPart(pBolt)&&pBolt->CalGuigeAuto())
			//	bRet=TRUE;
			double L=0,L0=0;
			hashL0.GetValueAt(pBolt->handle,L0);
			hashL.GetValueAt(pBolt->handle,L);
			if(dwParam!=NULL&&(L0!=pBolt->get_L0()||L!=pBolt->get_L()))
			{
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pBolt->handle;
				pInfo->m_uType=INFO_EMEND_SINGULAR_BOLTLO;
				if(bRet)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(pBolt->IsFootNail())
						pInfo->m_sDescription.Format("0X%X foot nail",pBolt->handle);
					else
						pInfo->m_sDescription.Format("0X%X bolt",pBolt->handle);
					CString strTemp="";
					if(L0!=pBolt->get_L0())
						strTemp.Format("L0 revision:%d->%d，",ftoi(L0),ftoi(pBolt->get_L0()));
					else
						strTemp.Format("L0:%d，",ftoi(L0));
					pInfo->m_sDescription += strTemp;
					if(L!=pBolt->get_L())
						strTemp.Format("Revise specification success:M%dX%d->M%dX%d!",pBolt->get_d(),ftoi(L),pBolt->get_d(),ftoi(pBolt->get_L()));
					else
						strTemp.Format("Specification:M%dX%d!",pBolt->get_d(),ftoi(L));
#else 
					if(pBolt->IsFootNail())
						pInfo->m_sDescription.Format("0X%X脚钉",pBolt->handle);
					else
						pInfo->m_sDescription.Format("0X%X螺栓",pBolt->handle);
					CString strTemp="";
					if(L0!=pBolt->get_L0())
						strTemp.Format("通厚修正:%d->%d，",ftoi(L0),ftoi(pBolt->L0));
					else
						strTemp.Format("通厚:%d，",ftoi(L0));
					pInfo->m_sDescription += strTemp;
					if(L!=pBolt->get_L())
						strTemp.Format("规格成功修正:M%dX%d->M%dX%d!",pBolt->get_d(),ftoi(L),pBolt->get_d(),ftoi(pBolt->get_L()));
					else
						strTemp.Format("规格:M%dX%d!",pBolt->get_d(),ftoi(L));
#endif
					pInfo->m_sDescription += strTemp;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("The specifical revision of bolt is failure, not find the specified L0 of bolt(remain specification M%dX%d)!",pBolt->get_d(),ftoi(pBolt->get_L()));
#else 
					pInfo->m_sDescription.Format("螺栓规格修正失败，没有找到符合通厚值的螺栓规格(保留原规格M%dX%d)!",pBolt->get_d(),ftoi(pBolt->get_L()));
#endif
			}
			Ta.Parts.pop_stack(pushed);
		}
	}
	catch(char* sError)
	{
		CString error;
#ifdef AFX_TARG_ENU_ENGLISH
		error.Format("The wrong handle of bolt %s is 0X%X",sError,hCurBolt);
#else 
		error.Format("%s出错螺栓句柄为:0X%X",sError,hCurBolt);
#endif
		AfxMessageBox(error);
	}
	if (dwParam != NULL)
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Revise bolt's L0");
#else 
	DisplayProcess(100,"修正螺栓通厚");
#endif
}
#endif
//#ifdef __SMART_DRAW_INC_
//螺栓垫圈或垫片自动增补修订
void ExecuteEmendBoltGasket(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	
	CWaitCursor wait;
	CEmendBoltGasket emendBoltGasket(&Ta);
	emendBoltGasket.Execute(pContext,false);
	emendBoltGasket.InitOutputInfoList(pInfoList);

}
//#endif
static OUTPUT_INFO_TYPE GetInfoTypeByCmdType(int cmdType)
{
	OUTPUT_INFO_TYPE uType=(OUTPUT_INFO_TYPE)0;
	if(cmdType==CHKPARTNO_NULLPARTNO)
		uType=INFO_CHK_NULLPARTNO;
	else if(cmdType==CHKPARTNO_PARTTYPE_DIFFER)
		uType=INFO_CHK_PARTTYPE_DIFFER;
	else if(cmdType==CHKPARTNO_RODINFO)
		uType=INFO_CHK_RODINFO;
	else if(cmdType==CHKPARTNO_RODBOLTINFO)
		uType=INFO_CHK_RODBOLTINFO;
	else if(cmdType==CHKPARTNO_ANGLECUT)
		uType=INFO_CHK_ANGLECUT;
	else if(cmdType==CHK_PLATE_BASICINFO)
		uType=INFO_CHK_PLATEINFO;
	else if(cmdType==CHK_PLATE_PROFILEANDBOLTINFO)
		uType=INFO_CHK_PLATEPROFILEANDBOLT;
	else if(cmdType==CHK_MIRROD_BASICINFO)
		uType=INFO_CHK_MIRROD;
	else if(cmdType==CHK_MIRROD_LENGTHINFO)
		uType=INFO_CHK_MIRROD_LENGTH;
	return uType;
}
//INFO_EMEND_CROSSRODINFO,	//更新交叉材计算信息
void ExecuteEmendCrossRodInfo(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Update the calculate information of cross-bracing Members");
#else 
	DisplayProcess(0,"更新交叉材计算信息");
#endif
	int i=0,n=Ta.Parts.GetNodeNum(CLS_LINEPART);
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{	//修正有交叉点，但丢失参照杆件的情况
		if(pNode->m_cPosCalType==4)	//交叉节点
		{
			BOOL pushed=Ta.Node.push_stack();
			CLDSNode* pLine1Start=Ta.Node.FromHandle(pNode->arrRelationNode[0]);
			CLDSNode* pLine1End=Ta.Node.FromHandle(pNode->arrRelationNode[1]);
			CLDSNode* pLine2Start=Ta.Node.FromHandle(pNode->arrRelationNode[2]);
			CLDSNode* pLine2End=Ta.Node.FromHandle(pNode->arrRelationNode[3]);
			Ta.Node.pop_stack(pushed);
			if(pLine1Start==NULL||pLine1End==NULL||pLine2Start==NULL||pLine2End==NULL)
				continue;
			f3dLine line1(pLine1Start->Position(false),pLine1End->Position(false));
			f3dLine line2(pLine2Start->Position(false),pLine2End->Position(false));
			CLDSLinePart* pRod1=Ta.FromRodHandle(pNode->arrRelationPole[0]);
			CLDSLinePart* pRod2=Ta.FromRodHandle(pNode->arrRelationPole[1]);
			if( pRod1==NULL||pRod1->pStart==NULL||pRod1->pEnd==NULL||
				line1.PtInLine(pRod1->pStart->Position(false))==0||
				line1.PtInLine(pRod1->pEnd->Position(false))==0)
			{
				for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
				{
					if(pRod->pStart==NULL||pRod->pEnd==NULL)
						continue;
					f3dLine line(pRod->pStart->Position(false),pRod->pEnd->Position(false));
					if( line.PtInLine(pNode->Position(false))>0&&	//交叉点为直接端点或内点
						pNode->cfgword.And(pRod->cfgword)&&			//交叉点与杆件可以共存
						line1.PtInLine(line.startPt)!=0&&line1.PtInLine(line.endPt)!=0)//直线与基准线1共线
					{
						COutputInfo *pInfo=pInfoList->append();
						pInfo->m_uType=INFO_EMEND_CROSSRODINFO;
						pInfo->m_hRelaObj1=pNode->arrRelationPole[0];
						pInfo->m_hRelaObj2=pRod->handle;
#ifdef AFX_TARG_ENU_ENGLISH
						pInfo->m_sDescription.Format("0x%X cross node's No.1 reference rod isn't reasonable,it revised to 0x%X->0x%X rod!",pNode->handle,pNode->arrRelationPole[0],pRod->handle);
#else 
						pInfo->m_sDescription.Format("0x%X交叉节点的1号参照杆件不合理，修正为0x%X->0x%X杆件!",pNode->handle,pNode->arrRelationPole[0],pRod->handle);
#endif
						pNode->arrRelationPole[0]=pRod->handle;
						pRod1=pRod;
						break;
					}
				}
			}
			if( pRod2==NULL||pRod2->pStart==NULL||pRod2->pEnd==NULL||
				line2.PtInLine(pRod2->pStart->Position(false))==0||
				line2.PtInLine(pRod2->pEnd->Position(false))==0)
			{
				for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
				{
					if(pRod->pStart==NULL||pRod->pEnd==NULL)
						continue;
					f3dLine line(pRod->pStart->Position(false),pRod->pEnd->Position(false));
					if( line.PtInLine(pNode->Position(false))>0&&	//交叉点为直接端点或内点
						pNode->cfgword.And(pRod->cfgword)&&			//交叉点与杆件可以共存
						line2.PtInLine(line.startPt)!=0&&line2.PtInLine(line.endPt)!=0)//直线与基准线2共线
					{
						COutputInfo *pInfo=pInfoList->append();
						pInfo->m_uType=INFO_EMEND_CROSSRODINFO;
						pInfo->m_hRelaObj1=pNode->arrRelationPole[1];
						pInfo->m_hRelaObj2=pRod->handle;
						pRod2=pRod;
#ifdef AFX_TARG_ENU_ENGLISH
						pInfo->m_sDescription.Format("0x%X cross node's No.2 reference rod isn't reasonable,it revised to 0x%X->0x%X rod!",pNode->handle,pNode->arrRelationPole[1],pRod->handle);
#else 
						pInfo->m_sDescription.Format("0x%X交叉节点的2号参照杆件不合理，修正为0x%X->0x%X杆件!",pNode->handle,pNode->arrRelationPole[1],pRod->handle);
#endif
						pNode->arrRelationPole[1]=pRod->handle;
						break;
					}
				}
			}
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__TSA_)
			if(pRod1&&pRod2)
			{
				if(pRod1->CalLenCoef.hRefPole==0)
				{
					COutputInfo *pInfo=pInfoList->append();
					pInfo->m_uType=INFO_EMEND_CROSSRODINFO;
					pInfo->m_hRelaObj1=pRod1->handle;
					pInfo->m_hRelaObj2=pRod2->handle;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("0x%X cross-bracing members's pressure reference rod isn't reasonable,it revised to 0x%X->0x%X rod!",pRod1->handle,pRod1->CalLenCoef.hRefPole,pRod2->handle);
#else 
					pInfo->m_sDescription.Format("0x%X交叉材的受压参照杆件，修正为0x%X->0x%X杆件!",pRod1->handle,pRod1->CalLenCoef.hRefPole,pRod2->handle);
#endif
					pRod1->CalLenCoef.hRefPole=pRod2->handle;
				}
				if(pRod2->CalLenCoef.hRefPole==0)
				{
					COutputInfo *pInfo=pInfoList->append();
					pInfo->m_uType=INFO_EMEND_CROSSRODINFO;
					pInfo->m_hRelaObj1=pRod2->handle;
					pInfo->m_hRelaObj2=pRod1->handle;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("0x%X cross-bracing members's pressure reference rod isn't reasonable,it revised to 0x%X->0x%X rod!",pRod2->handle,pRod1->CalLenCoef.hRefPole,pRod1->handle);
#else 
					pInfo->m_sDescription.Format("0x%X交叉材的受压参照杆件，修正为0x%X->0x%X杆件!",pRod2->handle,pRod1->CalLenCoef.hRefPole,pRod1->handle);
#endif
					pRod2->CalLenCoef.hRefPole=pRod1->handle;
				}
			}
#endif
		}
	}
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__TSA_)
	for(CLDSLinePart *pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/n,"Update the calculate information of cross-bracing members");
#else 
		DisplayProcess((i*100)/n,"更新交叉材计算信息");
#endif
		if( pLinePart->layer(1)=='Z')
			continue;
		if(pLinePart->CalLenCoef.iTypeNo!=0)
			continue;	//只处理智能计算长细比情况
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//非结构主体杆件(应该是连接杆件,如包铁)
		BOOL pushed=Ta.Parts.push_stack();
		f3dPoint inters_pos;
		CLDSLinePart *pRefLinePart;
		if(pLinePart->CalLenCoef.hRefPole>=0x20)
		{
			pRefLinePart=(CLDSLinePart*)Ta.Parts.FromHandle(pLinePart->CalLenCoef.hRefPole,CLS_LINEPART);
			if(pRefLinePart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Can't find 0X%X rod's compression rod 0X%X at the same time,the rod information has cleared！",pLinePart->handle,pLinePart->CalLenCoef.hRefPole);
#else 
				logerr.Log("杆件0X%X的同时受压参照杆件0X%X找不到, 已清空参照杆件信息！",pLinePart->handle,pLinePart->CalLenCoef.hRefPole);
#endif
			else if(pRefLinePart!=NULL&&pRefLinePart->pStart==NULL||pRefLinePart->pEnd==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("At the same time rod 0X%X with compression reference line rod 0X%X without start or end of the connection node,the reference rod information has cleared!",pLinePart->handle,pLinePart->CalLenCoef.hRefPole);
#else 
				logerr.Log("杆件0X%X的同时受压参照杆件0X%X的缺少始端或末端连接节点, 已清空参照杆件信息",pLinePart->handle,pLinePart->CalLenCoef.hRefPole);
#endif
				pRefLinePart=NULL;
			}
			else if(pRefLinePart!=NULL&&
				Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)!=1)	//无有效交叉点
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("At the same time rod 0X%X with comparession reference rod 0X%X without useful connection node , the reference rod information has cleared!",pLinePart->handle,pLinePart->CalLenCoef.hRefPole);
#else 
				logerr.Log("杆件0X%X与指定同时受压参照杆件0X%X无有效交点, 已清空参照杆件信息",pLinePart->handle,pLinePart->CalLenCoef.hRefPole);
#endif
				pLinePart->CalLenCoef.hRefPole=0;
			}
			if(pRefLinePart==NULL)
				pLinePart->CalLenCoef.hRefPole=0;
		}
		if(pLinePart->CalLenCoef.hRefPole<=0x20)
		{
			bool bHasIntersPt=false;
			pRefLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
			if(pRefLinePart!=NULL&&
				Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//有内交点
			{
				bHasIntersPt=true;
				pLinePart->CalLenCoef.hRefPole=pRefLinePart->handle;
			}
			else
				pRefLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(2));
			if(!bHasIntersPt&&pRefLinePart!=NULL&&
				Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//有内交点
			{
				bHasIntersPt=true;
				pLinePart->CalLenCoef.hRefPole=pRefLinePart->handle;
			}
			else
				pRefLinePart=(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(3));
			if(!bHasIntersPt&&pRefLinePart!=NULL&&
				Int3dll(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false),
				pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//有内交点
			{
				bHasIntersPt=true;
				pLinePart->CalLenCoef.hRefPole=pRefLinePart->handle;
			}
		}
		Ta.Parts.pop_stack(pushed);
	}
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Update calculate information of cross-bracing members");
#else 
	DisplayProcess(100,"更新交叉材计算信息");
#endif
}

//补充关联对称信息
void ExecuteEmendMirRodInfo(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	if(pContext==NULL)
		return;
	CMainFrame* pMainWnd=NULL;
	CTaskOutputList* pInfoList=NULL;
	if(pContext)
	{
		pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
		pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(0,"Supply related symmetrical information");
#else 
		DisplayProcess(0,"补充关联对称信息");
#endif
	}
	CLDSView* pView=pMainWnd->GetLDSView();
	//
	int hits=0,sum=Ta.Node.GetNodeNum()+Ta.Parts.GetNodeNum();
	for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		hits++;
		BOOL bPush=Ta.Node.push_stack();
		pView->EmendMirRodInfo(pNode);
		if(bPush)
			Ta.Node.pop_stack();
		if(pContext)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(hits*100/sum,"Supply related symmetrical information");
#else 
			DisplayProcess(hits*100/sum,"补充关联对称信息");
#endif
		}
	}
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		hits++;
		CLdsPartListStack stack(&Ta);	//原代码在for循环中退出时易疏乎弹出栈　wjh-2017.1.22
		pView->EmendMirRodInfo(pPart);
		//hits++;
		if(pContext)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(hits*100/sum,"Supply related symmetrical information");
#else 
			DisplayProcess(hits*100/sum,"补充关联对称信息");
#endif
		}
	}
	if(pContext)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(100,"Supply related symmetrical information");
#else 
		DisplayProcess(100,"补充关联对称信息");
#endif
	}
}

//检查重复节点
void ExecuteEmendOverlapNode(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	CLDSNode* pNode;
	NODESET nodeSet;
	int i=0,n;
#ifdef __COMMON_PART_INC_
	n=GetNodeSetBySeg(nodeSet,pContext->segTable);
#else
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		nodeSet.append(pNode);
	n=nodeSet.GetNodeNum();
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Repeat checking nodes!");
#else 
	DisplayProcess(0,"重复节点检测进度!");
#endif
	double toler2=pContext->tolerance*pContext->tolerance;
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Repeat checking nodes!");
#else 
		DisplayProcess((100*i)/n,"重复节点检测进度!");
#endif
		int nPush=nodeSet.push_stack();
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
			continue;	//网络协同时，对此节点没有拥有修改权限
		for(CLDSNode *pOthNode=nodeSet.GetNext();pOthNode;pOthNode=nodeSet.GetNext())
		{
			if(!pNode->ModuleTogetherWith(pOthNode))	//至少会在一个模型中同时出现
				continue;
			GEPOINT dv=pNode->xOriginalPos-pOthNode->xOriginalPos;
			if(dv.mod2()>toler2)
				continue;
			COutputInfo *pInfo=pInfoList->append();
			pInfo->m_uType=INFO_EMEND_OVERLAPNODE;
			pInfo->m_hRelaObj1=pNode->handle;
			pInfo->m_hRelaObj2=pOthNode->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			if(pNode->hFatherPart>0x20&&pOthNode->hFatherPart>0x20&&pNode->hFatherPart!=pOthNode->hFatherPart)
				pInfo->m_sDescription.Format("0x%X node and 0x%X node are overlap,but their parent nodes are different!",pNode->handle,pOthNode->handle);
			else
				pInfo->m_sDescription.Format("0x%X node and 0x%X node are overlap!",pNode->handle,pOthNode->handle);
#else 
			if(pNode->hFatherPart>0x20&&pOthNode->hFatherPart>0x20&&pNode->hFatherPart!=pOthNode->hFatherPart)
				pInfo->m_sDescription.Format("0x%X节点与0x%X节点重合, 但两节点父杆件不相同!",pNode->handle,pOthNode->handle);
			else
				pInfo->m_sDescription.Format("0x%X节点与0x%X节点重合!",pNode->handle,pOthNode->handle);
#endif
		}
		nodeSet.pop_stack(nPush);
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Repeat checking nodes!");
#else 
	DisplayProcess(100,"重复节点检测进度!");
#endif
}

//检查重复杆件
void ExecuteEmendOverlapRod(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	LINEPARTSET linePartSet;
	int i=0,n;
#ifdef __COMMON_PART_INC_
	n=GetLinePartSetBySeg(linePartSet,pContext->segTable);
#else
	for(CLDSLinePart* pRod=linePartSet.GetFirst();pRod;pRod=linePartSet.GetNext())
		linePartSet.append(pRod);
	n=linePartSet.GetNodeNum();
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Repeat checking rods!");
#else 
	DisplayProcess(0,"重复杆件检测进度!");
#endif
	CPtInLineCheck lineCheck,lineCheck2;
	CHashList<CPtInLineCheck> hashLineCheck;
	for(CLDSLinePart* pPole=linePartSet.GetFirst();pPole;pPole=linePartSet.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Repeat checking rodes!");
#else 
		DisplayProcess((100*i)/n,"重复杆件检测进度!");
#endif
		CPtInLineCheck lineCheck;
		if(pPole->pStart==NULL||pPole->pEnd==NULL)
			continue;	//不是力学分析线，可能是短角钢等
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPole->dwPermission))
			continue;	//网络协同时，对此节点没有拥有修改权限
		CPtInLineCheck* pLineCheck=hashLineCheck.GetValue(pPole->handle);
		if(hashLineCheck.GetValue(pPole->handle)==NULL)
		{
			lineCheck.SetLine(pPole->pStart->Position(false),pPole->pEnd->Position(false));
			hashLineCheck.SetValue(pPole->handle,lineCheck);
		}
		else
			lineCheck=*pLineCheck;

		CLDSLinePart *pOthPole=NULL;
		BOOL pushed=linePartSet.push_stack();
		bool bIsSonJg=FALSE;
#ifndef __TSA_
		if(pPole->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pPole)->group_father_jg_h>0x20)
			bIsSonJg=TRUE;
#endif
		for(pOthPole=linePartSet.GetNext();pOthPole;pOthPole=linePartSet.GetNext())
		{
#ifndef __TSA_
			if(bIsSonJg&&pOthPole->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pOthPole)->group_father_jg_h>0x20
			&&((CLDSLineAngle*)pPole)->group_father_jg_h==((CLDSLineAngle*)pOthPole)->group_father_jg_h)
				continue;   //组合角钢不合并
			if((bIsSonJg&&((CLDSLineAngle*)pPole)->group_father_jg_h==pOthPole->handle)
			||(pOthPole->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pOthPole)->group_father_jg_h>0x20
			&&((CLDSLineAngle*)pOthPole)->group_father_jg_h==pPole->handle))
				continue;   //不合并组合角钢父角钢基准线
#endif
			if(!(pPole->cfgword.And(pOthPole->cfgword)))
				continue;	//不在同一模型内
			if(pOthPole->pStart==NULL||pOthPole->pEnd==NULL)
				continue;	//不是力学分析线，可能是短角钢等
			if(theApp.m_bCooperativeWork&&pOthPole->dwPermission!=pPole->dwPermission)
				continue;	//分属不同权限不可以合并
			pLineCheck=hashLineCheck.GetValue(pOthPole->handle);
			if(pLineCheck==NULL)
			{
				lineCheck2.SetLine(pOthPole->pStart->Position(false),pOthPole->pEnd->Position(false));
				hashLineCheck.SetValue(pOthPole->handle,lineCheck2);
			}
			else
				lineCheck2=*pLineCheck;
			double scale_start,scale_end;
			if(lineCheck.Length()<=EPS ||lineCheck2.Length()<=EPS)
				continue;	//由于用户更改出错可能导致杆件始末端节点重合，进而导致重叠误判 wjh-2016.7.6
			if(lineCheck.Length()>lineCheck2.Length())
			{
				scale_start=lineCheck.CheckPoint(pOthPole->pStart->Position(false));
				scale_end=lineCheck.CheckPoint(pOthPole->pEnd->Position(false));
			}
			else
			{
				scale_start=lineCheck2.CheckPoint(pPole->pStart->Position(false));
				scale_end=lineCheck2.CheckPoint(pPole->pEnd->Position(false));
			}
			if((scale_start<0||scale_start>1)&&(scale_end<0||scale_end>1))
				continue;	//两杆件不重叠
			bool overlapped=false;
			if((scale_start>=0&&scale_start<=1)&&(scale_end>=0&&scale_end<=1))
				overlapped=true;
			else
			{	//短杆件与长杆件有重叠
				double dd=lineCheck.LineStdVec()*lineCheck2.LineStdVec();
				if(fabs(dd)<EPS_COS)
					continue;	//不共向不可能共线
				if(dd<0)
				{	//反向时调转始末端
					double overlapp=scale_start;
					scale_start=scale_end;
					scale_end=overlapp;
				}
				if(scale_start<0)
					scale_start=0;
				if(scale_end>1)
					scale_end=1;
				if(scale_start<scale_end)
					overlapped=true;
			}
			if(overlapped)
			{
				if(lineCheck.Length()>=lineCheck2.Length())
				{	//当前构件应合并到基准构件
					COutputInfo *pInfo=pInfoList->append();
					pInfo->m_uType = INFO_EMEND_OVERLAPROD;
					pInfo->m_hRelaObj1 = pPole->handle;	//应合并为pPole
					pInfo->m_hRelaObj2 = pOthPole->handle;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("0X%X rod and 0X%X rod are overlap",pPole->handle,pOthPole->handle);
#else 
					pInfo->m_sDescription.Format("0X%X杆件与0X%X杆件有重叠！",pPole->handle,pOthPole->handle);
#endif
				}
				else	//基准构件应合并到当前构件
				{
					COutputInfo *pInfo=pInfoList->append();
					pInfo->m_uType = INFO_EMEND_OVERLAPROD;
					pInfo->m_hRelaObj1 = pOthPole->handle;	//应合并为pOthPole
					pInfo->m_hRelaObj2 = pPole->handle;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("0X%X rod and 0X%X rod are overlap",pInfo->m_hRelaObj1,pInfo->m_hRelaObj2);
#else 
					pInfo->m_sDescription.Format("0X%X杆件与0X%X杆件有重叠！",pInfo->m_hRelaObj1,pInfo->m_hRelaObj2);
#endif
					break;
				}
			}
		}
		linePartSet.pop_stack(pushed);
		if(pOthPole!=NULL)
		{
			COutputInfo *pInfo=pInfoList->append();
			pInfo->m_uType = INFO_EMEND_OVERLAPROD;
			pInfo->m_hRelaObj1 = pOthPole->handle;	//应合并为pOthPole
			pInfo->m_hRelaObj2 = pPole->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("0X%X rod and 0X%X rod are overlap！",pOthPole->handle,pPole->handle);
#else 
			pInfo->m_sDescription.Format("0X%X杆件与0X%X杆件有重叠！",pOthPole->handle,pPole->handle);
#endif
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Repeat checking rodes!");
#else 
	DisplayProcess(100,"重复杆件检测进度!");
#endif
}

void LinePartLenLimitCheck() 
{
	CLDSPart *pPart;
	CLDSLinePart *pLinePart;
	int len,nFindPoleCount=0;
	FILE* fp;
	char filename[MAX_PATH];
	strcpy(filename,APP_PATH);
	strcat(filename,"polelenlim.txt");
	if((fp=fopen(filename,"wt"))==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't save file polelenlim.txt");
#else 
		AfxMessageBox("无法存取文件polelenlim.txt");
#endif
		return;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	fprintf(fp,"Max length of rod in this tower is %dmm\n",(int)Ta.m_fMaxXieJGLen);
#else 
	fprintf(fp,"此塔中杆件长度极限设为%dmm\n",(int)Ta.m_fMaxXieJGLen);
#endif
	for(pPart=Ta.Parts.GetFirst();pPart!=NULL;pPart=Ta.Parts.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		pLinePart=(CLDSLinePart*)pPart;
		len = (int)pLinePart->GetLength();
		if(len>Ta.m_fMaxXieJGLen)
		{
			nFindPoleCount++;
			g_pSolidDraw->SetEntSnapStatus(pPart->handle);
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
				fprintf(fp,"Length of angle 0X%X =%g meters,length beyond the limit of galvanized process and transport %g meters!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				fprintf(fp,"Length of tube 0X%X =%g meters,length beyond the limit of galvanized process and transport %g meters!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				fprintf(fp,"Length of slot 0X%X =%g meters,length beyond the limit of galvanized process and transport %g meters!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
			else
				fprintf(fp,"Length of rod 0X%X =%g meters,length beyond the limit of galvanized process and transport %g meters!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
#else 
				fprintf(fp,"角钢0X%X 的长度=%g米,长度超出镀锌工艺及运输限制%g米!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				fprintf(fp,"钢管0X%X 的长度=%g米,长度超出镀锌工艺及运输限制%g米!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				fprintf(fp,"槽钢0X%X 的长度=%g米,长度超出镀锌工艺及运输限制%g米!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
			else
				fprintf(fp,"杆件0X%X 的长度=%g米,长度超出镀锌工艺及运输限制%g米!\n",pPart->handle,len*0.001,Ta.m_fMaxXieJGLen*0.001);
#endif
		}
	}
	fclose(fp);
	CString sReport,cmdline;
	sReport.Format("有%d根杆件长度,超限,具体情况请查看安装目录下的polelenlim.txt",nFindPoleCount);
	//MessageBox(sReport);
	//cmdline.Format("notepad.exe %s",filename);
	//WinExec(cmdline,SW_SHOW);
}

//INFO_CHK_MIRROD
void ExecuteMirRodInfoCheck(DWORD dwParam)
{	//对称杆件规格、材质、基准长度
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the information of symmetrical rod");
#else 
	DisplayProcess(0,"对称杆件信息检查");
#endif
	LINEPARTSET linePartSet;
	int i=0,nRodNum;
#ifdef __COMMON_PART_INC_
	nRodNum=GetLinePartSetBySeg(linePartSet,pContext->segTable);
#else
	for(CLDSLinePart* pRod=linePartSet.GetFirst();pRod;pRod=linePartSet.GetNext())
		linePartSet.append(pRod);
	nRodNum=linePartSet.GetNodeNum();
#endif
	CHashStrList<long> couplePartHandleHash;	//以句柄对为键值
	for(CLDSLinePart* pRod=linePartSet.GetFirst();pRod;pRod=linePartSet.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nRodNum,"Check the information of symmetrical rod");
#else 
		DisplayProcess((i*100)/nRodNum,"对称杆件信息检查");
#endif
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pRod->dwPermission))
			continue;	//网络协同时，对此构件没有拥有修改权限
		else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pRod->iSeg))
			continue;	//判断是否在指定查找段号范围内
		//根据始终端节点位置，获取当前杆件关于X、Y、Z轴对称及旋转对称的杆件，并进行对比
		LINEPARTSET mirRodSet;
		MIRMSG msg;
		msg.axis_flag=0x01+0x02+0x04; //+0x08正侧面斜材规格可能不同，故某些时候仅需要对比XYZ对称杆件
		pRod->GetMirLinePartsByPos(msg,mirRodSet);
		for(CLDSLinePart *pMirRod=mirRodSet.GetFirst();pMirRod;pMirRod=mirRodSet.GetNext())
		{
			int cmd_type=0;
			char sKey[100]="";
			if(pRod->handle<pMirRod->handle)
				sprintf(sKey,"0x%X-0x%X",pRod->handle,pMirRod->handle);
			else 
				sprintf(sKey,"0x%X-0x%X",pMirRod->handle,pRod->handle);
			if(couplePartHandleHash.GetValue(sKey))
				continue;
			couplePartHandleHash.SetValue(sKey,pRod->handle);
			char sError[200]="";
			if(!pRod->IsEqualWith(pMirRod,pContext->tolerance,sError,&cmd_type,(DWORD*)&pContext->chkStyleFlag))
			{
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_uType=GetInfoTypeByCmdType(cmd_type);
				pInfo->m_hRelaObj1=pRod->handle;
				pInfo->m_hRelaObj2=pMirRod->handle;
				pInfo->m_sDescription.Format("%s",sError);
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the information of symmetrical rod");
#else 
	DisplayProcess(100,"对称杆件信息检查");
#endif
}
#ifdef __COMMON_PART_INC_
//INFO_CHK_MIRPLATE
void ExecuteMirPlateInfoCheck(DWORD dwParam)
{	//对称钢板材质、厚度、外形及螺栓孔
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	PARTSET partSet;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the information of symmetrical plate");
#else 
	DisplayProcess(0,"对称钢板信息检查");
#endif
	CLDSPlate *pPlate=NULL;
	int i=0,nPlateNum=GetPartSetBySeg(partSet,CLS_PLATE,pContext->segTable);
	CHashStrList<long> couplePartHandleHash;	//以句柄对为键值
	for(pPlate=(CLDSPlate*)partSet.GetFirst();pPlate;pPlate=(CLDSPlate*)partSet.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nPlateNum,"Check the information of symmetrical plate");
#else 
		DisplayProcess((i*100)/nPlateNum,"对称钢板信息检查");
#endif
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlate->dwPermission))
			continue;	//网络协同时，对此构件没有拥有修改权限
		else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pPlate->iSeg))
			continue;	//判断是否在指定查找段号范围内
		for(RELATIVE_OBJECT *pItem=pPlate->relativeObjs.GetFirst();pItem;pItem=pPlate->relativeObjs.GetNext())
		{	//先在关联构件列表中查找
			CLDSPlate *pMirPlate=(CLDSPlate*)Ta.Parts.FromHandle(pItem->hObj,CLS_PLATE);
			if(pMirPlate==NULL)
				continue;
			char sKey[100]="";
			if(pPlate->handle<pMirPlate->handle)
				sprintf(sKey,"0x%X-0x%X",pPlate->handle,pMirPlate->handle);
			else 
				sprintf(sKey,"0x%X-0x%X",pMirPlate->handle,pPlate->handle);
			if(couplePartHandleHash.GetValue(sKey))
				continue;
			couplePartHandleHash.SetValue(sKey,pPlate->handle);
			char sError[200]="";
			if(!pPlate->IsEqual(pMirPlate,pContext->tolerance,sError,NULL,(DWORD*)&pContext->chkStyleFlag))
			{
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_uType=INFO_CHK_MIRPLATE;
				pInfo->m_hRelaObj1=pPlate->handle;
				pInfo->m_hRelaObj2=pMirPlate->handle;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0x%X and 0x%X%s",pPlate->handle,pMirPlate->handle,sError);
#else 
				pInfo->m_sDescription.Format("0x%X与0x%X%s",pPlate->handle,pMirPlate->handle,sError);
#endif
				pInfo->tolerance=pContext->tolerance;
			}
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the information of symmetrical plate");
#else 
	DisplayProcess(100,"对称钢板信息检查");
#endif
}
//INFO_CHK_BOLTOVERLAP
void ExecuteBoltOverlapCheck(DWORD dwParam)
{	//重叠螺栓检测
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	PARTSET boltSet;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the information of overlapping bolt");
#else 
	DisplayProcess(0,"重叠螺栓检查");
#endif
	int i=0,bolt_n=GetPartSetBySeg(boltSet,CLS_BOLT,pContext->segTable);
	double nn=0.01*bolt_n*(bolt_n-1)/2;
	for(CLDSBolt* pBolt=(CLDSBolt*)boltSet.GetFirst();pBolt;pBolt=(CLDSBolt*)boltSet.GetNext())
	{
		i++;
		double process=0.5*(2*bolt_n-1-i)*i;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(ftoi(process/nn),"Check the information of overlapping bolt");
#else 
		DisplayProcess(ftoi(process/nn),"重叠螺栓检查");
#endif
		if(pBolt->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBolt->dwPermission))
			continue;	//网络协同时，对此螺栓没有拥有修改权限
		else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pBolt->iSeg))
			continue;	//判断是否在指定查找段号范围内
		BOOL pushed=boltSet.push_stack();
		//由原来的boltSet集合改为全部螺栓集合，以避免不同段内螺栓重叠（给定螺栓段号可能存在错误） wjh-2015.5.29
		for(CLDSBolt* pOtherBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pOtherBolt;pOtherBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pBolt==pOtherBolt || !pOtherBolt->cfgword.And(pBolt->cfgword))
				continue;	//配材号没有交集，不需要检查
			if(DISTANCE(pBolt->ucs.origin,pOtherBolt->ucs.origin)<pBolt->get_d())
			{
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_uType=INFO_CHK_BOLTOVERLAP;
				pInfo->m_hRelaObj1=pBolt->handle;
				pInfo->m_hRelaObj2=pOtherBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0x%X and 0x%X bolt's position is too close ，cause conflicts！",pBolt->handle,pOtherBolt->handle);
#else 
				pInfo->m_sDescription.Format("0x%X与0x%X螺栓位置过近，产生冲突！",pBolt->handle,pOtherBolt->handle);
#endif
			}
		}
		boltSet.pop_stack(pushed);
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the information of overlapping bolt");
#else 
	DisplayProcess(100,"重叠螺栓检查");
#endif
}
//INFO_CHK_HOLEMISSING
void ExecuteHoleMissingCheck(DWORD dwParam)
{	//漏孔检测
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	PARTSET boltSet;
	CLDSBolt *pBolt=NULL;
	int i=0,nn=Ta.Parts.GetNodeNum()+GetPartSetBySeg(boltSet,CLS_BOLT,pContext->segTable);
	CSuperSmartPtr<CLDSPart>pSuperPart;
	for(pBolt=(CLDSBolt*)boltSet.GetFirst();pBolt;pBolt=(CLDSBolt*)boltSet.GetNext())
		pBolt->feature=0;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Leak check");
#else 
	DisplayProcess(0,"漏孔检查");
#endif
	for(pSuperPart=Ta.Parts.GetFirst();pSuperPart.IsHasPtr();pSuperPart=Ta.Parts.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nn,"Leak check");
#else 
		DisplayProcess((i*100)/nn,"漏孔检查");
#endif
		CLsRefList *pLsRefList=pSuperPart->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			(*pLsRef)->feature++;	//统计每个螺栓在螺栓孔中的引用次数
	}
	for(pBolt=(CLDSBolt*)boltSet.GetFirst();pBolt;pBolt=(CLDSBolt*)boltSet.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nn,"Leak check");
#else 
		DisplayProcess((i*100)/nn,"漏孔检查");
#endif
		if(pBolt->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBolt->dwPermission))
			continue;	//网络协同时，对此螺栓没有拥有修改权限
		else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pBolt->iSeg))
			continue;	//判断是否在指定查找段号范围内
		else if(pBolt->feature==1&&!pBolt->IsFootNail())
		{
			COutputInfo *pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_HOLEMISSING;
			pInfo->m_hRelaObj1=pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("0x%X bolt only connects one part，proberly lack of bolt's hole！",pBolt->handle);
#else 
			pInfo->m_sDescription.Format("0x%X螺栓只连接了一个构件，可能缺少螺栓孔！",pBolt->handle);
#endif
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Leak check");
#else 
	DisplayProcess(100,"漏孔检查");
#endif
}
//INFO_CHK_REDUNDANTBOLT
void ExecuteRedundantBoltCheck(DWORD dwParam)
{	//冗余螺栓检测
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	CLDSBolt *pBolt=NULL;
	int i=0,nn=Ta.Parts.GetNodeNum()+Ta.Parts.GetNodeNum(CLS_BOLT);
	CSuperSmartPtr<CLDSPart>pSuperPart;
	for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		pBolt->feature=0;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check redundant bolt.");
#else 
	DisplayProcess(0,"冗余螺栓检查");
#endif
	for(pSuperPart=Ta.Parts.GetFirst();pSuperPart.IsHasPtr();pSuperPart=Ta.Parts.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nn,"Check redundant bolt.");
#else 
		DisplayProcess((i*100)/nn,"冗余螺栓检查");
#endif
		CLsRefList *pLsRefList=pSuperPart->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			(*pLsRef)->feature++;	//统计每个螺栓在螺栓孔中的引用次数
	}
	for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nn,"Check redundant bolt.");
#else 
		DisplayProcess((i*100)/nn,"冗余螺栓检查");
#endif
		if(pBolt->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBolt->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pBolt->iSeg))
			continue;	//判断是否在指定查找段号范围内
		else if(pBolt->feature==0)
		{
			COutputInfo *pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_REDUNDANTBOLT;
			pInfo->m_hRelaObj1=pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("0x%X bolt isn't connect any parts,it may be the redundant bolt！",pBolt->handle);
#else 
			pInfo->m_sDescription.Format("0x%X螺栓未连接任何构件，可能为多余螺栓！",pBolt->handle);
#endif
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check redundant bolt.");
#else 
	DisplayProcess(100,"冗余螺栓检查");
#endif
}

//螺栓合法性检测
enum BOLT_VALID_FLAG{BOLT_EDGE_SPACE=0x01,BOLT_END_SPACE=0x02,BOLT_L0_VERIFY=0x04,BOLT_SIZESPEC_VERIFY=0x08};
void ExecuteBoltValidCheck(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	PARTSET lineAngleSet,boltSet,plateSet;
	CXhChar200 sError;
	BOOL bCutAngleDist=TRUE,bLsEndSpace=TRUE,bRollingSpace=TRUE,
		bBoltGuiGe=TRUE,bBoltL0=TRUE,bLsPlateEdgeSpace=TRUE;
	COutputInfo *pInfo=NULL;
	int i=0,n=GetPartSetBySeg(lineAngleSet,CLS_LINEANGLE,pContext->segTable)+GetPartSetBySeg(boltSet,CLS_BOLT,pContext->segTable);
	if(pContext->chkStyleFlag&BOLT_EDGE_SPACE)//0x01)
		bCutAngleDist=bRollingSpace=bLsPlateEdgeSpace=TRUE;	//进行螺栓边距校核
	else
		bCutAngleDist=bRollingSpace=bLsPlateEdgeSpace=FALSE;//不进行螺栓边距校核
	if(pContext->chkStyleFlag&BOLT_END_SPACE)//0x02)
		bLsEndSpace=TRUE;	//进行螺栓端距校核
	else
		bLsEndSpace=FALSE;	//不进行螺栓端距校核
	if(pContext->chkStyleFlag&BOLT_SIZESPEC_VERIFY)//0x08)
		bBoltGuiGe=TRUE;	//进行螺栓合法性校核
	else
		bBoltGuiGe=FALSE;	//不进行螺栓合法性校核
	if(bLsPlateEdgeSpace)
	{
		n+=GetPartSetBySeg(plateSet,CLS_PLATE,pContext->segTable);
		n+=GetPartSetBySeg(plateSet,CLS_PARAMPLATE,pContext->segTable);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check bolt's legality！");
#else 
	DisplayProcess(0,"螺栓合法性校核！");
#endif
	//角钢螺栓合法性检测
	for(CLDSLineAngle* pJg=(CLDSLineAngle*)lineAngleSet.GetFirst();pJg;pJg=(CLDSLineAngle*)lineAngleSet.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Check bolt's legality！");
#else 
		DisplayProcess((100*i)/n,"螺栓合法性校核！");
#endif
		if(pJg->m_bVirtualPart)
			continue;
		else if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pJg->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pJg->iSeg))
			continue;	//判断是否在指定查找段号范围内
		if(pJg->ValidLsList(sError,bRollingSpace,FALSE,FALSE,pContext->tolerance)!=0)
		{
			pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_BOLT_ROLLINGSPACE;
			pInfo->m_hRelaObj1=pJg->handle;
			pInfo->m_sDescription = sError;
			pInfo->tolerance=pContext->tolerance;
		}
		if(pJg->ValidLsList(sError,FALSE,bCutAngleDist,FALSE,pContext->tolerance)!=0)
		{
			pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_BOLT_CUTANGLESPACE;
			pInfo->m_hRelaObj1=pJg->handle;
			pInfo->m_sDescription = sError;
			pInfo->tolerance=pContext->tolerance;
		}
		if(pJg->ValidLsList(sError,FALSE,FALSE,bLsEndSpace,pContext->tolerance)!=0)
		{
			pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_BOLT_ANGLE_ENDSPACE;
			pInfo->m_hRelaObj1=pJg->handle;
			pInfo->m_sDescription = sError;
			pInfo->tolerance=pContext->tolerance;
		}
		if(bRollingSpace&&!pJg->ValidLsPushFlat(sError,pContext->tolerance))
		{	//检查螺栓压扁位置是否与螺栓冲突，归为螺栓轧制边距类型，避免重复提示错误信息 wht 17-05-30
			pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_BOLT_ANGLE_FORGEERR;
			pInfo->m_hRelaObj1=pJg->handle;
			pInfo->m_sDescription = sError;
			pInfo->tolerance=pContext->tolerance;
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
	//连板螺栓边距检测
	if(bLsPlateEdgeSpace)
	{
		for(CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)plateSet.GetFirst();pPlate;pPlate=(CLDSGeneralPlate*)plateSet.GetNext())
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check bolt's legality！");
#else 
			DisplayProcess((100*i)/n,"螺栓合法性校核！");
#endif
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPlate->dwPermission))
				continue;	//网络协同时，对此钢板没有拥有修改权限
			else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pPlate->iSeg))
				continue;	//判断是否在指定查找段号范围内
			if(pPlate->m_bVirtualPart)
				continue;	//虚拟钢板不进行边距检测
			int nSize=0;
			CXhChar200 sError;
			DYN_ARRAY<CLDSPlate::EDGEBOLT> boltInfoArr(pPlate->GetLsCount());
			if(!pPlate->ValidateProfile(pContext->tolerance,boltInfoArr,&nSize))	//不合法的连接板边距
			{
				pInfo=pInfoList->append();
				if(boltInfoArr[0].bToHuoquLine)
					pInfo->m_uType=INFO_CHK_BOLT_HUOQULINE_SPACE;
				else
					pInfo->m_uType=INFO_CHK_BOLT_PLATE_ENDSPACE;
				pInfo->m_hRelaObj1=pPlate->handle;
				sError.Empty();
				for(int i=0;i<nSize;i++)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(sError.GetLength()==0)
					{
						if(boltInfoArr[i].needEndSpace<=0)
							sError.Printf("(0x%X not in the plate",boltInfoArr[i].hBolt);
						else
							sError.Printf("(0x%X:standard %d,actual %.1f",boltInfoArr[i].hBolt,boltInfoArr[i].needEndSpace,boltInfoArr[i].actualEndSpace);
					}
					else
					{
						if(boltInfoArr[i].needEndSpace<=0)
							sError.Printf("(%s,0x%X not in the plate",(char*)sError,boltInfoArr[i].hBolt);
						else
							sError.Printf("%s,0x%X:standard %d,actual %.1f",(char*)sError,boltInfoArr[i].hBolt,boltInfoArr[i].needEndSpace,boltInfoArr[i].actualEndSpace);
					}
#else
					if(sError.GetLength()==0)
					{
						if(boltInfoArr[i].needEndSpace<=0)
							sError.Printf("(0x%X不在钢板内",boltInfoArr[i].hBolt);
						else
							sError.Printf("(0x%X标准:%d,实际%:%.1f",boltInfoArr[i].hBolt,boltInfoArr[i].needEndSpace,boltInfoArr[i].actualEndSpace);
					}
					else
					{
						if(boltInfoArr[i].needEndSpace<=0)
							sError.Printf("%s,(0x%X不在钢板内",(char*)sError,boltInfoArr[i].hBolt);
						else
							sError.Printf("%s,0x%X标准:%d,实际:%.1f",(char*)sError,boltInfoArr[i].hBolt,boltInfoArr[i].needEndSpace,boltInfoArr[i].actualEndSpace);
					}
#endif
				}
				if(sError.GetLength()>0)
					sError.Append(")");
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format(" {%s} bolt margin of 0X%X joint plate isn't satisfied%s！",pPlate->handle,(char*)pPlate->GetPartNo(),(char*)sError);
#else 

				pInfo->m_sDescription.Format("0X%X%s{%s}螺栓边距不满足%s！",pPlate->handle,pPlate->wiType==CLDSPlate::CONNECT_PLATE?"连接板":"螺栓垫板",(char*)pPlate->GetPartNo(),(char*)sError);
#endif
				pInfo->tolerance=pContext->tolerance;
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
	}
	
	//螺栓通厚以及螺栓规格合法性检测
	for(CLDSBolt* pBolt=(CLDSBolt*)boltSet.GetFirst();pBolt;pBolt=(CLDSBolt*)boltSet.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Check bolt's legality!");
#else 
		DisplayProcess((100*i)/n,"螺栓合法性校核！");
#endif
		if(pBolt->m_bVirtualPart)
			continue;
		if(pBolt->get_d()>27)
			continue;	//暂时只检测27以下的螺栓 27以上的螺栓规格库不完整
		else if(pContext->segTable.GetNodeNum()>0&&!pContext->segTable.GetValue(pBolt->iSeg))
			continue;	//判断是否在指定查找段号范围内
		LS_XING_HAO *pLsXingHao=CLsLibrary::FindLsGuiGeByValidLen(pBolt->get_d(),pBolt->get_L(),pBolt->m_hFamily);
		if(pContext->chkStyleFlag&0x04 && pLsXingHao)
		{
			if(bBoltL0&&!pBolt->IsFootNail())
			{	//检测螺栓通厚
				if(pBolt->get_L0()<pLsXingHao->down_lim)
				{
					pInfo=pInfoList->append();
					pInfo->m_uType=INFO_CHK_BOLT_L0;
					pInfo->m_hRelaObj1=pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("0X%X bolt's L0 is too small，please check it！",pBolt->handle);
#else 
					pInfo->m_sDescription.Format("0X%X螺栓通厚过小，请核实！",pBolt->handle);
#endif
					pInfo->tolerance=pContext->tolerance;
				}
				else if(pBolt->get_L0()>pLsXingHao->up_lim)
				{
					pInfo=pInfoList->append();
					pInfo->m_uType=INFO_CHK_BOLT_L0;
					pInfo->m_hRelaObj1=pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("0X%X bolt's L0 is too large，please check it！\n",pBolt->handle);
#else 
					pInfo->m_sDescription.Format("0X%X螺栓通厚过大，请核实！\n",pBolt->handle);
#endif
					pInfo->tolerance=pContext->tolerance;
				}
			}
			CXhChar16 grade(pLsXingHao->grade);
			if(!grade.Equal(pBolt->Grade()))
			{
				pInfo=pInfoList->append();
				pInfo->m_uType=INFO_CHK_BOLT_GRADE;
				pInfo->m_hRelaObj1=pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X bolt's grade %s isn't in specifical database %s of bolt ，please check it！",pBolt->handle,pBolt->Grade(),(char*)grade);
#else 
				pInfo->m_sDescription.Format("0X%X螺栓级别%s与规格库中螺栓级别%s不符，请核实！",pBolt->handle,pBolt->Grade(),(char*)grade);
#endif
				pInfo->tolerance=pContext->tolerance;
			}
		}
		else if(bBoltGuiGe&&pLsXingHao==NULL)
		{
			//检测螺栓规格
			pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_BOLT_SPEC;
			pInfo->m_hRelaObj1=pBolt->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("The specification database of 0X%X bolt  hasn't the specification，please check it！",pBolt->handle);
#else 
			pInfo->m_sDescription.Format("0X%X螺栓规格库中无此规格螺栓，请核实！",pBolt->handle);
#endif
			pInfo->tolerance=pContext->tolerance;
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check bolt's legality！");
#else 
	DisplayProcess(100,"螺栓合法性校核！");
#endif
}
#endif
#ifdef __PART_DESIGN_INC_
//杆件节点配材号合法性
void ExecuteTrussNodePoleCompatibleCheck(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	CString sModuleScope="*";
	CHashTable<long>moduleHashTbl;
	GetModuleNoHashTblByModuleStr(sModuleScope,moduleHashTbl);
	//
	int i=0,n=Ta.Parts.GetNodeNum(CLS_LINEPART);
	int nModuleNum=Ta.Module.GetNodeNum();
	if(moduleHashTbl.GetHashTableSize()>0)
		nModuleNum=moduleHashTbl.GetHashTableSize();
	n*=nModuleNum;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check configur word legality of rod's node！");
#else 
	DisplayProcess(0,"杆件节点配材号合法性！");
#endif
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		if(moduleHashTbl.GetHashTableSize()>0&&!moduleHashTbl.ContainsKey(pModule->iNo))
			continue;	//非当前指定呼高
		for(CLDSLinePart *pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check configur word legality of rod's node！");
#else 
			DisplayProcess((100*i)/n,"杆件节点配材号合法性！");
#endif
			if (pPole->pEnd==NULL||pPole->pStart==NULL)
				continue;
			if(pModule->IsSonPart(pPole))
			{
				if(pPole->pStart&&pPole->pEnd&&pModule->IsSonPart(pPole->pStart)&&pModule->IsSonPart(pPole->pEnd))
					continue;
				else
				{
					COutputInfo *pInfo=pInfoList->append();
					pInfo->m_uType=INFO_CHK_MODELINTEGRITY;
					pInfo->m_hRelaObj1=pPole->handle;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("0X%X rod with two end node's model number is not matching in the %d model ！",pPole->handle,pModule->iNo);
#else 
					pInfo->m_sDescription.Format("0X%X杆件与两端节点模型号在%d号模型中不匹配！",pPole->handle,pModule->iNo);
#endif
				}
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		/*for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pModule->IsActivePart(pNode))
			{
				pPole=Ta.Pole.FromHandle(pNode->father_pole_handle);
				if(pPole==NULL)
				{
					nHits++;
					g_pSolidDraw->SetEntSnapStatus(pNode->handle);
					fprintf(fp,"0X%X节点丢失父杆件！\n",pNode->handle);
				}
				else if(pModule->IsActivePart(pPole))
					continue;
				else
				{
					nHits++;
					g_pSolidDraw->SetEntSnapStatus(pNode->handle);
					fprintf(fp,"0X%X节点与父角钢模型号在%d号模型中不匹配！\n",pNode->handle,pModule->iNo);
				}
			}
		}*/
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check configur word legality of rod's node！");
#else 
	DisplayProcess(100,"杆件节点配材号合法性！");
#endif
}
#endif


int CompareRodPointI(CLDSNode* pStart,CLDSNode* pEnd,CLDSNode* pStart2,CLDSNode* pEnd2)
{
	int startI=0,endI=0,startI2=0,endI2=0;
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
	if(pStart)
		startI=pStart->point_i;
	if(pEnd)
		endI=pEnd->point_i;
	if(pStart2)
		startI2=pStart2->point_i;
	if(pEnd2)
		endI2=pEnd2->point_i;
#else
	if(pStart)
		startI=pStart->handle;
	if(pEnd)
		endI=pEnd->handle;
	if(pStart2)
		startI2=pStart2->handle;
	if(pEnd2)
		endI2=pEnd2->handle;
#endif
	long start_i=min(startI,endI);
	long end_i=max(startI,endI);
	long start_i2=min(startI2,endI2);
	long end_i2=max(startI2,endI2);
	if(start_i<start_i2)
		return -1;
	else if(start_i>start_i2)
		return 1;
	else if(end_i<end_i2)
		return -1;
	else if(end_i>end_i2)
		return 1;
	else
		return 0;
}
//LAYER_CHECK_FINISHED 开始
//
//校核图层号信息
#include "UnifyModelLayer.h"
void ExecuteReviseLayerInfoCheck(DWORD dwParam)
{
	//CUnifyModelLayer unify;
	//unify.UnifyObjsLayer();
}
//</DEVELOP_PROCESS_MARK>
//校核关联对称信息
void ExecuteRelativeMirInfoCheck(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	int bad_hits=0,hits=0,sum=Ta.Node.GetNodeNum()+Ta.Parts.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check related symmetrical information!");
#else 
	DisplayProcess(0,"校核关联对称信息!");
#endif
	CLDSView* pView=pMainWnd->GetLDSView();
	for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		hits++;
		BOOL bPush=Ta.Node.push_stack();
		pView->CheckRelativeMirInfo(pNode,&bad_hits,pInfoList);
		if(bPush)
			Ta.Node.pop_stack();
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(hits*100/sum,"Check related symmetrical information!");
#else 
		DisplayProcess(hits*100/sum,"校核关联对称信息!");
#endif
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
	for(CSuperSmartPtr<CLDSPart> pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		hits++;
		BOOL pushed=Ta.Parts.push_stack();
		if(!pPart->IsLinePart())
			continue;
		pView->CheckRelativeMirInfo(&*pPart,&bad_hits,pInfoList);
		Ta.Parts.pop_stack(pushed);
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(hits*100/sum,"Check related symmetrical information!");
#else 
		DisplayProcess(hits*100/sum,"校核关联对称信息!");
#endif
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check related symmetrical information!");
#else 
	DisplayProcess(100,"校核关联对称信息!");
#endif
}
#if defined(__COMPLEX_PART_INC_) && !defined(__LDS_TSA_)
//修正钢管端头连接信息
struct TUBEJOINTERROR{
	long hTube;
	CLDSLineTube* pTube;
	BOOL bStart;
	long hOldEndPlate;
	long hNewEndPlate;
public:
	void SetKey(DWORD key){hTube=(long)key;}
};
void ExecuteTubeEndJointLjInfoCheck(DWORD dwParam)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;

	int bad_hits=0;
	int hits=0,sum=Ta.Parts.GetNodeNum();
	DisplayProcess(0,"校核修正钢管端头连接信息!");
	CHashListEx<TUBEJOINTERROR> hashTubeStartErrors,hashTubeEndErrors;
	CXhSimpleList<TUBEJOINTERROR*> listErrors;

	CLDSLineTube *pTube;
	for(pTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pTube;pTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
	{
		hits++;
		BOOL bPush=Ta.Parts.push_stack();
		if(pTube->m_tJointStart.type>=2)
		{
			CLDSGeneralPlate* pPlate=NULL;
			if(pTube->m_tJointStart.hLinkObj>0x20)
				pPlate=(CLDSGeneralPlate*)Ta.Parts.FromHandle(pTube->m_tJointStart.hLinkObj,CLS_PLATE,CLS_PARAMPLATE);
			double distToStart=pTube->GetLength();
			double distToEnd=distToStart;
			if(pPlate)
			{
				distToStart=DISTANCE(pPlate->ucs.origin,pTube->Start());
				distToEnd=DISTANCE(pPlate->ucs.origin,pTube->End());
			}
			if(pPlate&&(pPlate->m_hPartWeldParent!=pTube->handle||distToStart>distToEnd))
			{
				TUBEJOINTERROR* pError=hashTubeStartErrors.Add(pTube->handle);
				listErrors.AttachNode(pError);
				pError->pTube=pTube;
				pError->bStart=TRUE;
				pError->hOldEndPlate=pTube->m_tJointStart.hLinkObj;
				pError->hNewEndPlate=0;
			}
		}
		if(pTube->m_tJointEnd.type>=2)
		{
			CLDSGeneralPlate* pPlate=NULL;
			if(pTube->m_tJointEnd.hLinkObj>0x20)
				pPlate=(CLDSGeneralPlate*)Ta.Parts.FromHandle(pTube->m_tJointEnd.hLinkObj,CLS_PLATE,CLS_PARAMPLATE);
			double distToStart=pTube->GetLength();
			double distToEnd=distToStart;
			if(pPlate)
			{
				distToStart=DISTANCE(pPlate->ucs.origin,pTube->Start());
				distToEnd=DISTANCE(pPlate->ucs.origin,pTube->End());
			}
			if(pPlate&&(pPlate->m_hPartWeldParent!=pTube->handle||distToStart<distToEnd))
			{
				TUBEJOINTERROR* pError=hashTubeEndErrors.Add(pTube->handle);
				listErrors.AttachNode(pError);
				pError->pTube=pTube;
				pError->bStart=FALSE;
				pError->hOldEndPlate=pTube->m_tJointEnd.hLinkObj;
				pError->hNewEndPlate=0;
			}
		}
		if(bPush)
			Ta.Parts.pop_stack();
		DisplayProcess(hits*50/sum,"校核修正钢管端头连接信息!");
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		hits++;
		BOOL bPush=Ta.Parts.push_stack();
		CLDSParamPlate* pPlate=NULL;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)	//将来应添加处理底板等端头普通钢板的情况 wjh-2015.1.30
			pPlate=(CLDSParamPlate*)pPart;
		if(pPlate&&(pPlate->IsFL()||pPlate->IsInsertPlate()))
		{
			TUBEJOINTERROR* pError=hashTubeStartErrors.GetValue(pPlate->m_hPartWeldParent);
			bool corrected=false;
			if(pError)
			{
				double distToStart=DISTANCE(pPlate->ucs.origin,pError->pTube->Start());
				double distToEnd=DISTANCE(pPlate->ucs.origin,pError->pTube->End());
				if(distToStart<distToEnd)
				{
					pError->pTube->m_tJointStart.hLinkObj=pError->hNewEndPlate=pPlate->handle;
					corrected=true;
				}
			}
			if(!corrected&&(pError=hashTubeEndErrors.GetValue(pPlate->m_hPartWeldParent))!=NULL)
			{
				double distToStart=DISTANCE(pPlate->ucs.origin,pError->pTube->Start());
				double distToEnd=DISTANCE(pPlate->ucs.origin,pError->pTube->End());
				if(distToStart>distToEnd)
					pError->pTube->m_tJointEnd.hLinkObj=pError->hNewEndPlate=pPlate->handle;
			}
		}
		if(bPush)
			Ta.Parts.pop_stack();
		DisplayProcess(hits*50/sum,"校核修正钢管端头连接信息!");
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
	for(LIST_NODE<TUBEJOINTERROR*>* pListNode=listErrors.EnumFirst();pListNode;
		pListNode=listErrors.EnumNext())
	{
		TUBEJOINTERROR* pError=pListNode->data;
		COutputInfo *pInfo=pInfoList->append();
		pInfo->m_uType=INFO_EMEND_TUBEENDJOINTLJ;
		pInfo->m_hRelaObj1=pError->hTube;
		pInfo->m_hRelaObj2=0;
		if(pError->bStart&&pError->hNewEndPlate>0x20)
			pInfo->m_sDescription.Format("0X%X钢管的始端端头连接构件进行修正0X%X->0X%X！\n",pError->hTube,pError->hOldEndPlate,pError->hNewEndPlate);
		else if(pError->bStart&&pError->hNewEndPlate<=0x20)
			pInfo->m_sDescription.Format("0X%X钢管的始端端头连接构件0X%X组焊信息有误！\n",pError->hTube,pError->hOldEndPlate);
		else if(!pError->bStart&&pError->hNewEndPlate>0x20)
			pInfo->m_sDescription.Format("0X%X钢管的终端端头连接构件进行修正0X%X->0X%X！\n",pError->hTube,pError->hOldEndPlate,pError->hNewEndPlate);
		else
			pInfo->m_sDescription.Format("0X%X钢管的终端端头连接构件0X%X组焊信息有误！\n",pError->hTube,pError->hOldEndPlate);
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
	DisplayProcess(100,"校核修正钢管端头连接信息!");
}
#endif
#ifdef __COMMON_PART_INC_
//INFO_CHK_NULLPARTNO
void ExecuteNullPartNoCheck(DWORD dwParam)
{	//空件号检查
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;

	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check empty piece number!");
#else 
	DisplayProcess(0,"空件号检查!");
#endif
	int i=0,nn=polylist.GetNodeNum()+Ta.Parts.GetNodeNum();
	CSuperSmartPtr<CLDSPart> pPart;
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nn,"Check empty piece number!");
#else 
		DisplayProcess((i*100)/nn,"空件号检查!");
#endif
		if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
			pPart->GetClassTypeId()==CLS_POLYJOINT||pPart->m_bVirtualPart)
			continue;	//螺栓无编号不参预构件号检查
		else if(pPart->IsLinePart()&&(pPart.LinePartPointer()->bHuoQuStart||pPart.LinePartPointer()->bHuoQuEnd))
			continue;	//
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPart->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		if(strlen(pPart->GetPartNo())<=0)
		{
			COutputInfo *pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_NULLPARTNO;
			pInfo->m_hRelaObj1=pPart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("The piece number of 0x%X%s inputs error,please check it!",pPart->handle,pPart->GetPartTypeName());
#else 
			pInfo->m_sDescription.Format("0x%X%s的构件号未正确输入,请查实!",pPart->handle,pPart->GetPartTypeName());
#endif
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
	}
	//2.重新统计制弯构件并遍历,生成同上哈希表
	CLDSPolyPart *pPolyPart=NULL;
	for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((i*100)/nn,"Check empty piece number!");
#else 
		DisplayProcess((i*100)/nn,"空件号检查!");
#endif
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPolyPart->dwPermission))
			continue;	//网络协同时，对此构件没有拥有修改权限
		CLDSLinePart *pFirstPole=pPolyPart->segset.GetFirst();
		if(pFirstPole->GetClassTypeId()!=CLS_LINEANGLE
			&&pFirstPole->GetClassTypeId()!=CLS_LINETUBE
			&&pFirstPole->GetClassTypeId()!=CLS_LINESLOT
			&&pFirstPole->GetClassTypeId()!=CLS_LINEFLAT)
			continue;
		if(strlen(pPolyPart->GetPartNo())<=0)
		{
			COutputInfo *pInfo=pInfoList->append();
			pInfo->m_uType=INFO_CHK_NULLPARTNO;
			pInfo->m_hRelaObj1=pPolyPart->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("The piece number of bending part 0x%X inputs error,please check it!",pPolyPart->handle);
#else 
			pInfo->m_sDescription.Format("0x%X制弯构件的件号未正确输入,请查实!",pPolyPart->handle);
#endif
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Chech empty piece number!");
#else 
	DisplayProcess(100,"空件号检查!");
#endif
}

//按件号将构件分组，返回构件总数
static int GetPartsHashListPartNo(CHashStrList<PART_PTR_ARR> &partsHasListPartNo,
							CString sSegStr="*",char *sPartNo=NULL)
{
	int nSumPartNum=0;
	CHashList<SEGI>segTbl;
	//分解段号字符串
	GetSegNoHashTblBySegStr(sSegStr,segTbl);
	CSuperSmartPtr<CLDSPart> pPart;
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
			pPart->GetClassTypeId()==CLS_POLYJOINT||pPart->m_bVirtualPart)
			continue;	//螺栓无编号不参预构件号检查
		else if(pPart->IsLinePart()&&(pPart.LinePartPointer()->bHuoQuStart||pPart.LinePartPointer()->bHuoQuEnd))
			continue;	//
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPart->dwPermission))
			continue;	//网络协同时，对此角钢没有拥有修改权限
		if(sPartNo==NULL&&segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
			continue;	//检查是否在指定段号范围内
		else if(sPartNo!=NULL&&stricmp(sPartNo,pPart->GetPartNo())!=0)
			continue;	//检查指定编号构件
		CString sKey="NULL";//件号为空的构件用NULL作为键值
		if(strlen(pPart->GetPartNo())>0)
			sKey=pPart->GetPartNo();
		PART_PTR_ARR *pPartArr=partsHasListPartNo.GetValue(sKey);
		if(pPartArr==NULL)
			pPartArr=partsHasListPartNo.Add(sKey);
		pPartArr->append(pPart);
		nSumPartNum++;
	}
	return nSumPartNum;
}

//按件号将火曲构件分组，返回构件总数
static int GetPolyPartsHashListPartNo(CHashStrList<POLYPART_PTR_ARR> &polyPartsHasListPartNo,
				POLYPARTLIST& polylist,CString sSegStr="*",char *sPartNo=NULL)
{
	int nSumPolyPartNum=0;
	CHashList<SEGI>segTbl;
	//分解段号字符串
	GetSegNoHashTblBySegStr(sSegStr,segTbl);
	CLDSPolyPart *pPolyPart=NULL;
	for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		if(sPartNo==NULL)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pPolyPart->dwPermission))
				continue;	//网络协同时，对此构件没有拥有修改权限
			if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPolyPart->iSeg.iSeg)==NULL)
				continue;	//检查是否在指点段号范围内
			CLDSLinePart *pFirstPole=pPolyPart->segset.GetFirst();
			if(pFirstPole->GetClassTypeId()!=CLS_LINEANGLE
				&&pFirstPole->GetClassTypeId()!=CLS_LINETUBE
				&&pFirstPole->GetClassTypeId()!=CLS_LINESLOT
				&&pFirstPole->GetClassTypeId()!=CLS_LINEFLAT)
				continue;
		}
		else if(stricmp(sPartNo,pPolyPart->GetPartNo())!=0)
			continue;		//检查指定编号构件
		CString sKey="NULL";//件号为空的构件用NULL作为键值
		if(strlen(pPolyPart->GetPartNo())>0)
			sKey=pPolyPart->GetPartNo();
		POLYPART_PTR_ARR *pPolyPartArr=polyPartsHasListPartNo.GetValue(sKey.GetBuffer(0));
		if(pPolyPartArr==NULL)
			pPolyPartArr=polyPartsHasListPartNo.Add(sKey.GetBuffer(0));
		pPolyPartArr->append(pPolyPart);
		nSumPolyPartNum++;
	}
	return nSumPolyPartNum;
}
void ExecutePartNoCheck(DWORD dwParam)
{
	PARTNO_CHK_CONTEXT *pContext=(PARTNO_CHK_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	if(pContext==NULL)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar50 sTitle="Process";
#else 
	CXhChar50 sTitle="进度";
#endif
	OUTPUT_INFO_TYPE uType=GetInfoTypeByCmdType(pContext->chkStyleFlag);
	if(uType>0)
		sTitle.Printf("%s",COutputInfo::GetTypeName(uType));
	double tolerance=pContext->tolerance;
	int nSumProgress=pContext->nSumPartNum;
	CHashStrList<PART_PTR_ARR> *pPartsHasListPartNo=&(pContext->partsHasListPartNo);
	CHashStrList<POLYPART_PTR_ARR> *pPolyPartsHasListPartNo=&(pContext->polyPartsHasListPartNo);
	try{
		int i=0;
		CString sError;
		COutputInfo *pInfo=NULL;
		DisplayProcess(0);
		//1.对普通构件进行件号检查
		for(PART_PTR_ARR *pPartArr=pPartsHasListPartNo->GetFirst();pPartArr;pPartArr=pPartsHasListPartNo->GetNext())
		{
			POLYPART_PTR_ARR* pPolyPartArr=NULL;
			if(pContext->chkStyleFlag&CHKPARTNO_PARTTYPE_DIFFER)
				pPolyPartArr=pPolyPartsHasListPartNo->GetValue(pPartsHasListPartNo->GetCursorKey());
			if(stricmp(pPartsHasListPartNo->GetCursorKey(),"NULL")==0)
			{	//件号为空的构件
				if(pContext->chkStyleFlag&CHKPARTNO_NULLPARTNO)
				{	//检查空件号
					for(CLDSPart **ppPart=pPartArr->GetFirst();ppPart;ppPart=pPartArr->GetNext())
					{
						if ((*ppPart)->cfgword.IsNull())
							continue;	//配材号为空表示当前任何呼高里都不会用到此构件，很可能是被用户手动关掉隐藏在后台了 wjh-2019.5.25
						CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("The piece number of %s0x%X inputs error,please check it!",sPartName,(*ppPart)->handle);
#else 
						sError.Format("%s0x%X的构件号未正确输入,请查实!",sPartName,(*ppPart)->handle);
#endif
						pInfo=pInfoList->append();
						pInfo->m_hRelaObj1=(*ppPart)->handle;
						pInfo->m_sDescription = sError;
						pInfo->m_uType=INFO_CHK_NULLPARTNO;
						pInfo->tolerance=pContext->tolerance;
						//显示进度条
						i++; 
						DisplayProcess((i*100)/nSumProgress,sTitle);
						if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
							break;
					}
				}
				else 
				{
					i+=pPartArr->GetSize();
					DisplayProcess((i*100)/nSumProgress,sTitle);
				}
			}
			else if(pPolyPartArr)
			{
				CLDSPart **ppFirstPart=pPartArr->GetFirst();
				CString sPartName,sFirstPartName=(*ppFirstPart)->GetPartTypeName();
				long hFirstPart=(*ppFirstPart)->handle;
				CString sPartNo=(*ppFirstPart)->GetPartNo();
				CLDSPolyPart* pPolyPart=*pPolyPartArr->GetFirst();
				CLDSPart* pPart=pPolyPart->segset.GetFirst();
				long hPart=0;
				if(pPart)
				{
					hPart=pPart->handle;
					sPartName=pPart->GetPartTypeName();
				}
#ifdef AFX_TARG_ENU_ENGLISH
				sError.Format("The type of %s0x%X and %s0x%X{%s} is different,but they have same piece number！",
#else 
				sError.Format("%s0x%X与%s0x%X{%s}不是同一类型构件，但编了相同的构件号！",
#endif
					sFirstPartName,hFirstPart,sPartName,hPart,sPartNo);
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=hFirstPart;
				pInfo->m_hRelaObj2=hPart;
				pInfo->m_sDescription = sError;
				pInfo->m_uType=INFO_CHK_PARTTYPE_DIFFER;
				pInfo->tolerance=pContext->tolerance;
			}
			else if(pPartArr->GetSize()>1)
			{	//件号非空且同件号构件数大于1时进行比对
				CLDSPart **ppFirstPart=pPartArr->GetFirst();
				CString sFirstPartName=(*ppFirstPart)->GetPartTypeName();
				CString sPartNo=(*ppFirstPart)->GetPartNo();
				CLDSPart* pFirCommonPart=(*ppFirstPart)->MotherPart();
				long hFirstPart=pFirCommonPart->handle;
				i++;
				CLDSPart **ppPart;
				for(ppPart=pPartArr->GetFirst();ppPart;ppPart=pPartArr->GetNext(),i++)
				{
					if ((*ppPart)->cfgword.IsNull())
						continue;	//配材号为空表示当前任何呼高里都不会用到此构件，很可能是被用户手动关掉隐藏在后台了 wjh-2019.5.25
					int error_type=0;
					char error_info[500]="";
					CLDSPart *pSecCommonPart=(*ppPart)->MotherPart();
					long hPart=pSecCommonPart->handle;
					if(*ppFirstPart==*ppPart)
						;
					else if((*ppFirstPart)->GetClassTypeId()!=(*ppPart)->GetClassTypeId())
					{
						if(pContext->chkStyleFlag&CHKPARTNO_PARTTYPE_DIFFER)
						{	//检查构件类型
							CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
							sError.Format("The type of %s0x%X and %s0x%X{%s} is different ，but they have same piece number！",
#else 
							sError.Format("%s0x%X与%s0x%X{%s}不是同一类型构件，但编了相同的构件号！",
#endif
								sFirstPartName,hFirstPart,sPartName,hPart,(char*)(*ppPart)->GetPartNo());
							pInfo=pInfoList->append();
							pInfo->m_hRelaObj1=hFirstPart;
							pInfo->m_hRelaObj2=hPart;
							pInfo->m_sDescription = sError;
							pInfo->m_uType=INFO_CHK_PARTTYPE_DIFFER;
							pInfo->tolerance=pContext->tolerance;
						}
					}
					else if(!pFirCommonPart->IsEqual(pSecCommonPart,TRUE,tolerance,error_info,&error_type,&pContext->chkStyleFlag))
					{	//非影射构件的比较
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0X%X and 0X%X(%s) have same piece number",sFirstPartName,hFirstPart,hPart,error_info);
#else 
						sError.Format("%s0X%X与0X%X(%s)但编了相同的构件号",sFirstPartName,hFirstPart,hPart,error_info);
#endif
						pInfo=pInfoList->append();
						pInfo->m_hRelaObj1=hFirstPart;
						pInfo->m_hRelaObj2=hPart;
						pInfo->m_sDescription = sError;
						pInfo->m_uType=GetInfoTypeByCmdType(error_type);
						pInfo->tolerance=pContext->tolerance;
					}
					else
					{	//影射构件与其始祖级构件的比较
						if((*ppFirstPart)->IsShadowObject())
						{
							if(!IsEqualShadowAndAncestor((*ppFirstPart),error_info,tolerance,hFirstPart,hPart))
							{
#ifdef AFX_TARG_ENU_ENGLISH
								sError.Format("%s%s,but they have the same  piece number",sFirstPartName,error_info);
#else 
								sError.Format("%s%s,但编了相同的构件号",sFirstPartName,error_info);
#endif
								pInfo=pInfoList->append();
								pInfo->m_hRelaObj1=hFirstPart;
								pInfo->m_hRelaObj2=hPart;
								pInfo->m_sDescription = sError;
								pInfo->m_uType=INFO_CHK_SHADOWPARTINFO;
								pInfo->tolerance=pContext->tolerance;
							}
						}
						if((*ppPart)->IsShadowObject())
						{
							if(!IsEqualShadowAndAncestor((*ppPart),error_info,tolerance,hFirstPart,hPart))
							{
#ifdef AFX_TARG_ENU_ENGLISH
								sError.Format("%s%s,but they have same  piece number",sFirstPartName,error_info);
#else 
								sError.Format("%s%s,但编了相同的构件号",sFirstPartName,error_info);
#endif
								pInfo=pInfoList->append();
								pInfo->m_hRelaObj1=hFirstPart;
								pInfo->m_hRelaObj2=hPart;
								pInfo->m_sDescription = sError;
								pInfo->m_uType=INFO_CHK_SHADOWPARTINFO;
								pInfo->tolerance=pContext->tolerance;
							}
						}
					}
					//显示进度条
					//i++; //移到for语句开始处累加了
					DisplayProcess((i*100)/nSumProgress,sTitle);
					if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
						break;
				}
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		//2.对制弯构件进行件号检查
		POLYPART_PTR_ARR *pPolyPartArr=NULL;
		for(pPolyPartArr=pPolyPartsHasListPartNo->GetFirst();pPolyPartArr;pPolyPartArr=pPolyPartsHasListPartNo->GetNext())
		{
			if(stricmp(pPolyPartsHasListPartNo->GetCursorKey(),"NULL")==0)
			{	//件号为空的构件
				if(pContext->chkStyleFlag&CHKPARTNO_NULLPARTNO)
				{	//检查空件号
					for(CLDSPolyPart **ppPart=pPolyPartArr->GetFirst();ppPart;ppPart=pPolyPartArr->GetNext())
					{
						CString sPartName=(*ppPart)->GetPartTypeName();
						CLDSPart* pFirstPart=(*ppPart)->segset.GetFirst();
						if(pFirstPart)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							sError.Format("The piece number of %s0x%X inputs error,please check it!",sPartName,pFirstPart->handle);
#else 
							sError.Format("%s0x%X的构件号未正确输入,请查实!",sPartName,pFirstPart->handle);
#endif
						}
						pInfo=pInfoList->append();
						pInfo->m_hRelaObj1=pFirstPart->handle;
						pInfo->m_sDescription = sError;
						pInfo->m_uType=INFO_CHK_NULLPARTNO;
						pInfo->tolerance=pContext->tolerance;
						//显示进度条
						i++;
						DisplayProcess((i*100)/nSumProgress,sTitle);
					}
				}
				else 
				{
					i+=pPolyPartArr->GetSize();
					DisplayProcess((i*100)/nSumProgress,sTitle);
				}

			}
			else if(pPolyPartArr->GetSize()>1)
			{	//件号非空且同件号构件数大于1时进行比对
				CLDSPolyPart **ppFirstPart=pPolyPartArr->GetFirst();
				CString sFirstPartName=(*ppFirstPart)->GetPartTypeName();
				long hFirstPart=(*ppFirstPart)->handle;
				if((*ppFirstPart)->segset.GetFirst())
					hFirstPart=(*ppFirstPart)->segset.GetFirst()->handle;
				CString sPartNo=(*ppFirstPart)->GetPartNo();
				i++;
				if(PART_PTR_ARR *pPartArr=pPartsHasListPartNo->GetValue(sPartNo.GetBuffer(0)))
				{	//检查火曲构件编号是否与其他构件编号冲突
					CLDSPart **ppTempPart=pPartArr->GetFirst();
					if(ppTempPart&&(*ppTempPart)->GetClassTypeId()!=(*ppFirstPart)->GetClassTypeId())
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("The type of 0X%X part and 0X%X part {%s} is different，but they have same  piece number！",(*ppTempPart)->handle,hFirstPart,sPartNo);
#else 
						sError.Format("0X%X构件与0X%X构件{%s}不是同一类型构件，但编了相同的构件号！",(*ppTempPart)->handle,hFirstPart,sPartNo);
#endif
						pInfo=pInfoList->append();
						pInfo->m_hRelaObj1=hFirstPart;
						pInfo->m_hRelaObj2=(*ppTempPart)->handle;
						pInfo->m_sDescription = sError;
						pInfo->m_uType=INFO_CHK_PARTTYPE_DIFFER;
						pInfo->tolerance=pContext->tolerance;
					}
				}
				if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
					break;
				for(CLDSPolyPart **ppPart=pPolyPartArr->GetFirst();ppPart;ppPart=pPolyPartArr->GetNext())
				{
					if((*ppPart)->handle==(*ppFirstPart)->handle)
						continue;
					int error_type=0;
					char error_info[500]="";
					long hPart=(*ppPart)->handle;
					if((*ppPart)->segset.GetFirst())
						hPart=(*ppPart)->segset.GetFirst()->handle;
					if((*ppFirstPart)->GetClassTypeId()!=(*ppPart)->GetClassTypeId())
					{
						if(pContext->chkStyleFlag&CHKPARTNO_PARTTYPE_DIFFER)
						{	//检查构件类型
							CString sPartName=(*ppPart)->GetPartTypeName();
#ifdef AFX_TARG_ENU_ENGLISH
							sError.Format("The type of %s0x%X part and %s0x%X{%s} part is different，but they have same  piece number！",
#else 
							sError.Format("%s0x%X与%s0x%X{%s}不是同一类型构件，但编了相同的构件号！",
#endif
								sFirstPartName,hFirstPart,sPartName,hPart,(*ppPart)->GetPartNo());
							pInfo=pInfoList->append();
							pInfo->m_hRelaObj1=hFirstPart;
							pInfo->m_hRelaObj2=hPart;
							pInfo->m_sDescription = sError;
							pInfo->m_uType=INFO_CHK_PARTTYPE_DIFFER;
							pInfo->tolerance=pContext->tolerance;
						}
					}
					else if(!(*ppFirstPart)->IsEqual(*ppPart,tolerance,error_info,&error_type,&pContext->chkStyleFlag))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						sError.Format("%s0X%X and 0X%X(%s) have same piece number.",sFirstPartName,hFirstPart,hPart,error_info);
#else 
						sError.Format("%s0X%X与0X%X(%s)但编了相同的构件号",sFirstPartName,hFirstPart,hPart,error_info);
#endif
						pInfo=pInfoList->append();
						pInfo->m_hRelaObj1=hFirstPart;
						pInfo->m_hRelaObj2=hPart;
						pInfo->m_sDescription = sError;
						pInfo->m_uType=GetInfoTypeByCmdType(error_type);
						pInfo->tolerance=pContext->tolerance;
					}
					//显示进度条
					i++;
					DisplayProcess((i*100)/nSumProgress,sTitle);
					if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
						break;
				}
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		DisplayProcess(100);
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}
#endif
void ExecuteDataValidCheck(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	COutputInfo *pInfo;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	try{
		int i=0;
		int nRodNum=Ta.Parts.GetNodeNum(CLS_LINEPART);
		int n=Ta.Node.GetNodeNum()+Ta.Parts.GetNodeNum()+nRodNum;
	#ifdef __ANGLE_PART_INC_
		n=n+nRodNum+Ta.Parts.GetNodeNum(CLS_PLATE)+Ta.Parts.GetNodeNum(CLS_BOLT)+
			Ta.Parts.GetNodeNum(CLS_POLYJOINT);
	#endif
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(0,"Check integrality of bolt！");
#else 
		DisplayProcess(0,"数据完整性校核！");
#endif
		//节点数据合法性检查
		for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check integrality of bolt！");
#else 
			DisplayProcess((100*i)/n,"数据完整性校核！");
#endif
			BOOL pushed=Ta.Node.push_stack();
			int retCode=pNode->ValidCheck();
			Ta.Node.pop_stack(pushed);
			if(retCode<0)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pNode->handle;
			}
			else
				pInfo=NULL;
			if(retCode==-1)
			{
				pInfo->m_uType=INFO_NODE_LOST_FATHERROD;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X node lacks of parent rod,data error!",pNode->handle);
#else 
				pInfo->m_sDescription.Format("0X%X节点缺少节点父杆件,数据有误!",pNode->handle);
#endif
			}
			else if(retCode==-2)
			{
				pInfo->m_uType=INFO_NODE_LOST_ATTACHNODE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X node lacks of dependent node,data error!",pNode->handle);
#else 
				pInfo->m_sDescription.Format("0X%X节点缺少依附节点,数据有误!",pNode->handle);
#endif
			}
			else if(retCode==-3)
			{
				pInfo->m_uType=INFO_NODE_LOST_ATTACHROD;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X node lacks of dependent rod,data error!",pNode->handle);
#else 
				pInfo->m_sDescription.Format("0X%X节点缺少依附杆件,数据有误!",pNode->handle);
#endif
			}
			else if(retCode==-4)
			{
				pInfo->m_uType=INFO_NODE_LOST_ATTACHPLANE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X node lacks of dependent plane,data error!",pNode->handle);
#else 
				pInfo->m_sDescription.Format("0X%X节点缺少依附平面,数据有误!",pNode->handle);
#endif
			}
			else if(retCode==-5)
			{
				pInfo->m_uType=INFO_NODE_LOST_ATTACHPOINT;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X node lacks of dependent datum point ,data error!",pNode->handle);
#else 
				pInfo->m_sDescription.Format("0X%X节点缺少依附基准点,数据有误!",pNode->handle);
#endif
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check integrality of bolt！");
#else 
			DisplayProcess((100*i)/n,"数据完整性校核！");
#endif
			if(pPart->GetClassTypeId()!=CLS_BOLT&&
				pPart->GetClassTypeId()!=CLS_POLYJOINT&&pPart->cMaterial==0)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPart->handle;
				pInfo->m_uType=INFO_PART_LOST_MATERIAL;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X part lacks of material information,please check it!",pPart->handle);
#else 
				pInfo->m_sDescription.Format("0X%X构件缺少材质信息,请核查!",pPart->handle);
#endif
			}
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__TSA_)
			else if(pPart->IsLinePart())
			{
				STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pPart->cMaterial);
				if(pSteelMat==NULL)
				{
					pInfo=pInfoList->append();
					pInfo->m_hRelaObj1=pPart->handle;
					pInfo->m_uType=INFO_PART_INVALID_MATERIAL;
#ifdef AFX_TARG_ENU_ENGLISH
					pInfo->m_sDescription.Format("The material information of 0X%X part is wrong,please check it!",pPart->handle);
#else 
					pInfo->m_sDescription.Format("0X%X构件材质信息不正确,请核查!",pPart->handle);
#endif
				}
			}
#endif
			
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		CLDSLinePart *pLinePart;
		for(pLinePart=(CLDSLinePart*)Ta.Parts.GetFirst(CLS_LINEPART);pLinePart;pLinePart=(CLDSLinePart*)Ta.Parts.GetNext(CLS_LINEPART))
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check integrality of bolt！");
#else 
			DisplayProcess((100*i)/n,"数据完整性校核！");
#endif
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
				continue;
			if(pLinePart->pStart==NULL)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLinePart->handle;
				pInfo->m_uType=INFO_ROD_LOST_STARTNODE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The start node of 0X%X rod is null,data error!",pLinePart->handle);
#else 
				pInfo->m_sDescription.Format("0X%X杆件始端节点为空,数据有误!",pLinePart->handle);
#endif
			}
			if(pLinePart->pEnd==NULL)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLinePart->handle;
				pInfo->m_uType=INFO_ROD_LOST_ENDNODE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The end node of 0X%X part is null,data error!",pLinePart->handle);
#else 
				pInfo->m_sDescription.Format("0X%X杆件终端节点为空,数据有误!",pLinePart->handle);
#endif
			}
			
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
#ifdef __ANGLE_PART_INC_
		//角钢数据合法性检查
		for(pLinePart=(CLDSLinePart*)Ta.Parts.GetFirst(CLS_LINEPART);pLinePart;pLinePart=(CLDSLinePart*)Ta.Parts.GetNext(CLS_LINEPART))
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check integrality of bolt！");
#else 
			DisplayProcess((100*i)/n,"数据完整性校核！");
#endif
			if(!pLinePart->IsAngle())
				continue;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
			BOOL pushed=Ta.Parts.push_stack();
			int retCode=pLineAngle->ValidCheck();
			Ta.Parts.pop_stack(pushed);
			/*if(retCode==-1)
				fprintf(fp,"0X%X包铁或短角钢缺少始终端节点,数据可能有误!\n",pLineAngle->handle);
			else if(retCode==-2)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPart->handle;
				pInfo->m_uType=INFO_ROD_LOST_ENDNODE;
				pInfo->m_sDescription.Format("0X%X角钢始终端节点为空,数据有误!\n",pLineAngle->handle);
			}
			else */
			if(retCode==-3)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLineAngle->handle;
				pInfo->m_uType=INFO_ANGLE_LOST_LSLIST;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("0X%X angle lack of bolt list ,data error!\n",pLineAngle->handle);
#else 
				pInfo->m_sDescription.Format("0X%X角钢缺少螺栓链表,数据有误!\n",pLineAngle->handle);
#endif
			}
			else if(retCode==-4)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLineAngle->handle;
				pInfo->m_uType=INFO_ANGLESTART_LOST_BENDLINE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The start of 0X%X angle lack of bending datum angle,data error!\n",pLineAngle->handle);
#else 
				pInfo->m_sDescription.Format("0X%X角钢始端缺少火曲基准角钢,数据有误!\n",pLineAngle->handle);
#endif
			}
			else if(retCode==-5)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLineAngle->handle;
				pInfo->m_uType=INFO_ANGLESTART_LOST_PRJLINE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The start of 0X%X angle lack of collinear datum angle,data error!\n",pLineAngle->handle);
#else 
				pInfo->m_sDescription.Format("0X%X角钢始端缺少共线位置基准角钢,数据有误!\n",pLineAngle->handle);
#endif
			}
			else if(retCode==-6)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLineAngle->handle;
				pInfo->m_uType=INFO_ANGLEEND_LOST_BENDLINE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The end of 0X%X angle lack of bending datum angle,data error!\n",pLineAngle->handle);
#else 
				pInfo->m_sDescription.Format("0X%X角钢终端缺少火曲基准角钢,数据有误!\n",pLineAngle->handle);
#endif
			}
			else if(retCode==-7)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLineAngle->handle;
				pInfo->m_uType=INFO_ANGLEEND_LOST_PRJLINE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The end of 0X%X angle lack of collinear datum angle,data error!\n",pLineAngle->handle);
#else 
				pInfo->m_sDescription.Format("0X%X角钢终端缺少共线位置基准角钢,数据有误!\n",pLineAngle->handle);
#endif
			}
			else if(retCode==-8)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLineAngle->handle;
				pInfo->m_uType=INFO_ANGLEWINGX_LOST_REF_ROD;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The process of computing 0X%X angle's X leg normal lack of reference rod,data error!\n",pLineAngle->handle);
#else 
				pInfo->m_sDescription.Format("0X%X角钢X肢法线计算过程中缺少参照杆件,数据有误!\n",pLineAngle->handle);
#endif
			}
			else if(retCode==-9)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLineAngle->handle;
				pInfo->m_uType=INFO_ANGLEWINGY_LOST_REF_ROD;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("Yhe process of computing 0X%X angle's X leg normal lack of reference rod,data error!,data error!\n",pLineAngle->handle);
#else 
				pInfo->m_sDescription.Format("0X%X角钢Y肢法线计算过程中缺少参照杆件,数据有误!\n",pLineAngle->handle);
#endif
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		/*
		for(CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)Ta.Parts.GetFirst(CLS_GROUPLINEANGLE);pGroupJg;pGroupJg=(CLDSGroupLineAngle*)Ta.Parts.GetNext(CLS_GROUPLINEANGLE))
		{
			BOOL pushed=Ta.Parts.push_stack();
			int retCode=pGroupJg->ValidCheck();
			Ta.Parts.pop_stack(pushed);
			//if(retCode==-1)
			//	fprintf(fp,"0X%X包铁或短角钢缺少始终端节点,数据可能有误!\n",pGroupJg->handle);
			//else 
			if(retCode==-2)
				fprintf(fp,"0X%X角钢始终端节点为空,数据有误!\n",pGroupJg->handle);
			else if(retCode==-3)
				fprintf(fp,"0X%X角钢缺少螺栓链表,数据有误!\n",pGroupJg->handle);
			else if(retCode==-4)
				fprintf(fp,"0X%X角钢始端缺少火曲基准角钢,数据有误!\n",pGroupJg->handle);
			else if(retCode==-5)
				fprintf(fp,"0X%X角钢始端缺少共线位置基准角钢,数据有误!\n",pGroupJg->handle);
			else if(retCode==-6)
				fprintf(fp,"0X%X角钢终端缺少火曲基准角钢,数据有误!\n",pGroupJg->handle);
			else if(retCode==-7)
				fprintf(fp,"0X%X角钢终端缺少共线位置基准角钢,数据有误!\n",pGroupJg->handle);
			else if(retCode==-8)
				fprintf(fp,"0X%X角钢X肢法线计算过程中缺少参照角钢,数据有误!\n",pGroupJg->handle);
			else if(retCode==-9)
				fprintf(fp,"0X%X角钢Y肢法线计算过程中缺少参照角钢,数据有误!\n",pGroupJg->handle);
			else
				continue;
		}*/
		for(CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetFirst(CLS_POLYJOINT);pPolyJoint;pPolyJoint=(CLDSPolyJoint*)Ta.Parts.GetNext(CLS_POLYJOINT))
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check integrality of bolt！");
#else 
			DisplayProcess((100*i)/n,"数据完整性校核！");
#endif
			char prev_spec[100]="",next_spec[100]="";
			pPolyJoint->pPrevLinePart->GetSpecification(prev_spec);
			pPolyJoint->pNextLinePart->GetSpecification(next_spec);
			if(pPolyJoint->pPrevLinePart->GetClassTypeId()!=pPolyJoint->pNextLinePart->GetClassTypeId()||stricmp(prev_spec,next_spec)!=0)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPolyJoint->pPrevLinePart->handle;
				pInfo->m_hRelaObj2=pPolyJoint->pNextLinePart->handle;
				pInfo->m_uType=INFO_CHK_PARTTYPE_DIFFER;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The specification which 0X%X bending point connect 0X%X part and 0X%X part  is different,so can't create bending part!",
#else 
				pInfo->m_sDescription.Format("0X%X制弯点所连接0X%X构件与0X%X构件规格类型不一致，无法形成合法的制弯构件!",
#endif
					pPolyJoint->handle,pPolyJoint->pPrevLinePart->handle,pPolyJoint->pNextLinePart->handle);
				
				if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
					break;
			}
		}
		for(CLDSPlate *pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check integrality of bolt！");
#else 
			DisplayProcess((100*i)/n,"数据完整性校核！");
#endif
			int pushed=Ta.Parts.push_stack();
			int retCode=pPlate->ValidCheck();
			Ta.Parts.pop_stack(pushed);
			if(retCode==-1)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPlate->handle;
				pInfo->m_uType=INFO_PLATE_LOST_DATUM_REF_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The datum node's handle of  0X%X plate is null in flexible design,it is illegal.",pPlate->handle);
#else 
				pInfo->m_sDescription.Format("0X%X板的柔性设计参数基准节点句柄为空不合法",pPlate->handle);
#endif
			}
			else if(retCode==-2)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPlate->handle;
				pInfo->m_uType=INFO_PLATE_LOST_NORM_DES_PARA;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The datum node parameters of 0X%X plate is illegal in flexible design .",pPlate->handle);
#else 
				pInfo->m_sDescription.Format("0X%X板的柔性设计基准点参数不合法",pPlate->handle);
#endif
			}
			else if(retCode==-3)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPlate->handle;
				pInfo->m_uType=INFO_PLATE_LOST_NORM_DES_PARA;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The datum node parameters of 0X%X plate is illegal in flexible design .",pPlate->handle);
#else 
				pInfo->m_sDescription.Format("0X%X板的柔性设计基准面法线参数不合法",pPlate->handle);
#endif
			}
			else if(retCode==-4)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPlate->handle;
				pInfo->m_uType=INFO_PLATE_LOST_LINK_REF_ROD;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The parameters of rod joint rod of 0X%X plate is illegal in flexible design.",pPlate->handle);
#else 
				pInfo->m_sDescription.Format("0X%X板的部分连接杆件柔性设计参数不合法",pPlate->handle);
#endif
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
		for(CLDSBolt *pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Check integrality of bolt！");
#else 
			DisplayProcess((100*i)/n,"数据完整性校核！");
#endif
			int pushed=Ta.Parts.push_stack();
			int retCode=pLs->ValidCheck();
			Ta.Parts.pop_stack(pushed);
			if(retCode==-1)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLs->handle;
				pInfo->m_uType=INFO_BOLT_LOST_DATUM_REF_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The position design of bolt 0X%X lack of datum part,data error!",pLs->handle);
#else 
				pInfo->m_sDescription.Format("0X%X螺栓位置设计缺少基准构件,数据有误!",pLs->handle);
#endif
			}
			else if(retCode==-2)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLs->handle;
				pInfo->m_uType=INFO_BOLT_LOST_DATUM_REF_PART;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The position design of bolt 0X%X lack of datum node,data error!",pLs->handle);
#else 
				pInfo->m_sDescription.Format("0X%X螺栓位置设计缺少基准节点,数据有误!",pLs->handle);
#endif
			}
			//else if(retCode==-3)
			//	fprintf(fp,"0X%X螺栓位置设计缺少基准螺栓,数据有误!\n",pLs->handle);
			else if(retCode==-4)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLs->handle;
				pInfo->m_uType=INFO_BOLT_LOST_NORM_DES_PARA;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The normal design of bolt 0X%X lack of datum rod,data error!",pLs->handle);
#else 
				pInfo->m_sDescription.Format("0X%X螺栓法线设计缺少基准构件,数据有误!",pLs->handle);
#endif
			}
			if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				break;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(100,"Check integrality of bolt！");
#else 
		DisplayProcess(100,"数据完整性校核！");
#endif
#endif
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}

#ifdef __COMMON_PART_INC_
CXhChar200 ValidPlateSolid(CLDSGeneralPlate *pPlate)
{
	if(pPlate==NULL)
		return CXhChar200();
	f3dLine line;
	PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=pPlate->vertex_list.GetTail();
	line.startPt=pPrevVertex?pPrevVertex->vertex:f3dPoint();
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		line.endPt=pVertex->vertex;
		if(pPrevVertex->type>0&&line.startPt.IsEqual(line.endPt))
#ifdef AFX_TARG_ENU_ENGLISH
			return CXhChar200("0X%X Plate Arc point coincidence!",pPlate->handle);
#else
			return CXhChar200("0X%X板圆弧起始点与终止点重合!",pPlate->handle);
#endif
		line.startPt=line.endPt;
		pPrevVertex=pVertex;
	}
	return CXhChar200();
}
//实体数据合法性检查
bool IsValid3dPoint(const double coords[3])
{
	if( coords[0]>1000000||coords[0]<-1000000||
		coords[1]>1000000||coords[1]<-1000000||
		coords[2]>1000000||coords[2]<-1000000)
		return false;
	return true;
}
void ExecutePartSolidDataValidCheck(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	
	COutputInfo *pInfo;
	CLDSPart *pPart=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the legality of part's entities！");
#else 
	DisplayProcess(0,"构件实体合法性检查！");
#endif
	int i=0,n=Ta.Parts.GetNodeNum();
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Check the legality of part's entities！");
#else 
		DisplayProcess((100*i)/n,"构件实体合法性检查！");
#endif
		CString tempStr;
		CStringArray str_arr;
		GEPOINT start,end;
		CLDSLinePart* pRod=NULL;
		if(pPart->IsLinePart())
		{
			pRod=(CLDSLinePart*)pPart;
			start=pRod->xPosStart;
			end=pRod->xPosEnd;
		}
		if(pPart->GetClassTypeId()==CLS_BOLT&&pPart->m_bVirtualPart)
			((CLDSBolt*)pPart)->GetUCS();	//更新虚拟螺栓坐标系 wht 11-01-25
		if(pPart->GetClassTypeId()==CLS_BOLT&&pPart->ucs.axis_z.IsZero())
		{
			pInfo=pInfoList->append();
			pInfo->m_hRelaObj1=pPart->handle;
			pInfo->m_uType=INFO_BOLT_NORM_ZERO;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("The normal of 0X%X bolt is null !",pPart->handle);
#else 
			pInfo->m_sDescription.Format("0X%X螺栓法向方向为空!",pPart->handle);
#endif
		}
		else if( !IsValid3dPoint(pPart->ucs.origin)||(pRod&&
				(!IsValid3dPoint(start)||!IsValid3dPoint(end)||abs(pRod->startOdd())>10000||fabs(pRod->endOdd())>10000)))
		{
			pInfo=pInfoList->append();
			pInfo->m_hRelaObj1=pPart->handle;
			pInfo->m_uType=INFO_PART_LOCATION_ABNORMAL;
			char szSectTypeName[16]={0};
			strcpy(szSectTypeName,"构件");
			CXhString szTypeName(szSectTypeName,16);
			if(pPart->GetClassTypeId()==CLS_BOLT)
				szTypeName.Copy("螺栓");
			else if(pPart->IsAngle())
				szTypeName.Copy("角钢");
			else if(pPart->IsPlate())
				szTypeName.Copy("钢板");
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("The position of 0X%X part is abnormal!",pPart->handle);
#else 
			pInfo->m_sDescription.Format("0X%X%s位置异常!",pPart->handle,szSectTypeName);
#endif
		}
		else if(pPart->ucs.axis_x.IsZero()||pPart->ucs.axis_y.IsZero()||pPart->ucs.axis_z.IsZero())
		{
			CString sPartTypeName,sError;
#ifdef AFX_TARG_ENU_ENGLISH
			if(pPart->ucs.axis_x.IsZero())
				sError="X direction is null ";
			else if(pPart->ucs.axis_y.IsZero())
				sError="Y direction is null";
			else if(pPart->ucs.axis_z.IsZero())
				sError="Z direction is null";
#else 
			if(pPart->ucs.axis_x.IsZero())
				sError="X轴方向为空";
			else if(pPart->ucs.axis_y.IsZero())
				sError="Y轴方向为空";
			else if(pPart->ucs.axis_z.IsZero())
				sError="Z轴方向为空";
#endif
			sPartTypeName=pPart->GetPartTypeName();
			pInfo=pInfoList->append();
			pInfo->m_hRelaObj1=pPart->handle;
			pInfo->m_uType=INFO_INVALID_PART_UCS;
			pInfo->m_sDescription.Format("%s0X%X%s!",sPartTypeName,pPart->handle,sError);
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			tempStr="";
#ifdef AFX_TARG_ENU_ENGLISH
			if(pPlate->vertex_list.GetNodeNum()<3)
				tempStr.Format("The vertex number of 0X%X plate is less than 3!",pPart->handle);
			else if(pPlate->GetThick()<=0)
				tempStr.Format("The thickness of 0X%X plate is 0!",pPart->handle);
			else if(pPlate->GetLength()<=0)
				tempStr.Format("The length of 0X%X plate is 0!",pPart->handle);
			else if(pPlate->GetWidth()<=0)
				tempStr.Format("The width of 0X%X plate is 0!",pPart->handle);
#else 
			if(pPlate->vertex_list.GetNodeNum()<3)
				tempStr.Format("钢板0X%X顶点数小于3!",pPart->handle);
			else if(pPlate->GetThick()<=0)
				tempStr.Format("钢板0X%X厚度为0!",pPart->handle);
			else if(pPlate->GetLength()<=0)
				tempStr.Format("钢板0X%X长度为0!",pPart->handle);
			else if(pPlate->GetWidth()<=0)
				tempStr.Format("钢板0X%X宽度为0!",pPart->handle);
#endif
			if(tempStr.GetLength()>0)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPart->handle;
				pInfo->m_uType=INFO_INVALID_PROFILE;
				pInfo->m_sDescription=tempStr;
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->vertex_list.GetNodeNum()<3)
		{
			((CLDSParamPlate*)pPart)->DesignPlate();
			if(((CLDSParamPlate*)pPart)->vertex_list.GetNodeNum()<3)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPart->handle;
				pInfo->m_uType=INFO_INVALID_PROFILE;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("The  vertex number of 0x%X parametric plate is less than 3!",pPart->handle);
#else 
				pInfo->m_sDescription.Format("参数化板0x%X顶点数小于3!",pPart->handle);
#endif
			}
		}
		else if(pPart->IsLinePart()&&((CLDSLinePart*)pPart)->GetLength()==0)
		{
			pInfo=pInfoList->append();
			pInfo->m_hRelaObj1=pPart->handle;
			pInfo->m_uType=INFO_INVALID_PROFILE;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("The length of 0x%X rod is smaller than 0!",pPart->handle);
#else 
			pInfo->m_sDescription.Format("杆件0x%X长度小于0!",pPart->handle);
#endif
		}
		else if(pPart->pSolidBody&&pPart->pSolidBody->KeyPointNum()<2)
		{
			if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				continue;	//跳过组合角钢,组合角钢没有对应的实体只有子角
			pInfo=pInfoList->append();
			pInfo->m_hRelaObj1=pPart->handle;
			pInfo->m_uType=INFO_INVALID_PROFILE;
#ifdef AFX_TARG_ENU_ENGLISH
			pInfo->m_sDescription.Format("The vertex number of %s0x%X is less than 3!",pPart->GetPartTypeName(),pPart->handle);
#else 
			pInfo->m_sDescription.Format("%s0x%X顶点数小于3!",pPart->GetPartTypeName(),pPart->handle);
#endif
		}
		else if(pPart->IsPlate())
		{
			CXhChar200 sInfo=ValidPlateSolid((CLDSGeneralPlate*)pPart);
			if(sInfo.GetLength()>0)
			{
				pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPart->handle;
				pInfo->m_uType=INFO_INVALID_PROFILE;
				pInfo->m_sDescription=sInfo;
			}
		}
		if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the legality of part's entities！");
#else 
	DisplayProcess(100,"构件实体合法性检查！");
#endif
}

//1.同端节点杆件碰撞检测 
//识别判断pRod1与pRod2的连接端，有共同端节点时返回值为0,否则返回距离最近的端点距离
double RodJunctionDiscern(CLDSLinePart* pRod1,CLDSLinePart* pRod2,char* cJunction,bool bOnlySameNode=false);
void ExecuteRodsCollideCheckAndReflect(DWORD dwParam,CHash64List<char> *pHashCheckRodCouples) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=pContext!=NULL?(CMainFrame*)pContext->m_pMainWnd:NULL;
	CTaskOutputList* pInfoList=pContext!=NULL?(CTaskOutputList*)pContext->m_pOutputInfoList:NULL;
	SCOPE_STRU *pScope=pContext!=NULL?(SCOPE_STRU*)pContext->m_pTag:NULL;
	//
	//关于相互焊接的构件间的判断(钢管与板,板与板等)还未完善... 钢管夹板及延伸钢板等
	int i=0,n=0,count=0;
	CLDSLinePart *pRod=NULL,*pElseRod=NULL;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		n++;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the collision between rods!");
#else 
	DisplayProcess(0,"杆件碰撞检测进度!");
#endif
	count=n*(n-1);	//统计杆件最大碰撞检测组合数
	const int JUNCTION_CLEARANCE=200;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext(),i++)
	{
		int least=(n-i)*(n-i-1);
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*(count-least))/count,"Check the collision of rods' same end!");
#else 
		DisplayProcess((100*(count-least))/count,"同端节点杆件碰撞检测进度!");
#endif
		if(pRod->m_bVirtualPart)
			continue;	//被拆分角钢和虚角钢不进行碰撞检测
		if(pScope && !pScope->IsIntersWithLine(pRod->Start(),pRod->End()))
			continue;	//不在指定空间区域内的杆件
		CLdsPartListStack stack_p(&Ta.Parts);
		for(pElseRod=Ta.EnumRodNext();pElseRod;pElseRod=Ta.EnumRodNext())
		{
			if(pElseRod->m_bVirtualPart)
				continue;	//被拆分角钢和虚角钢不进行碰撞检测
			if(!pRod->cfgword.And(pElseRod->cfgword))
				continue;	//不在同一个模型中不必检测碰撞
			if(pRod->GetClassTypeId()==CLS_LINEANGLE&&pElseRod->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pRod->huoqu_handle>0X20&&pRod->huoqu_handle==pElseRod->huoqu_handle)//pJg->sub_type&TA_JG_HUOQUSEG&&pOthJg->sub_type&TA_JG_HUOQUSEG&&
					continue;	//同一根火曲角钢上的火曲子角钢间不判断
				if( ((CLDSLineAngle*)pRod)->group_father_jg_h==((CLDSLineAngle*)pElseRod)->group_father_jg_h && 
					((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
					continue;	//隶属于同一根组合角钢、
				//以下作八行用以剔除合理碰角钢碰撞
				if(pRod->pStart&&pRod->pStart->hFatherPart==pElseRod->handle)
					continue;
				else if(pRod->pEnd&&pRod->pEnd->hFatherPart==pElseRod->handle)
					continue;
				else if(pElseRod->pStart&&pElseRod->pStart->hFatherPart==pRod->handle)
					continue;
				else if(pElseRod->pEnd&&pElseRod->pEnd->hFatherPart==pRod->handle)
					continue;
				CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
				CLDSLineAngle* pElseAngle=(CLDSLineAngle*)pElseRod;
				//以下八行用以剔除短角钢与其依赖角钢间的碰撞情况 wjh-2016.6.15
				if(pElseAngle->pStart==NULL&&pElseAngle->desStartPos.spatialOperationStyle==0&&pElseAngle->desStartPos.datumPoint.des_para.hPart==pAngle->handle)
					continue;
				if(pElseAngle->pEnd==NULL&&pAngle->desEndPos.spatialOperationStyle==0&&pElseAngle->desEndPos.datumPoint.des_para.hPart==pAngle->handle)
					continue;
				if(pElseAngle->pStart==NULL&&pElseAngle->desStartPos.spatialOperationStyle==0&&pElseAngle->desStartPos.datumPoint.des_para.hPart==pAngle->handle)
					continue;
				if(pElseAngle->pEnd==NULL&&pElseAngle->desEndPos.spatialOperationStyle==0&&pElseAngle->desEndPos.datumPoint.des_para.hPart==pAngle->handle)
					continue;
			}
			if(pScope && !pScope->IsIntersWithLine(pElseRod->Start(),pElseRod->End()))
				continue;	//不在指定空间区域内的杆件
			if(pRod->m_hPartWeldParent==pElseRod->handle||pElseRod->m_hPartWeldParent==pRod->handle||
				(pRod->m_hPartWeldParent==pElseRod->m_hPartWeldParent&&pRod->m_hPartWeldParent>0))
				continue;	//两杆件焊接在一起 合理碰撞
			else if(RodJunctionDiscern(pRod,pElseRod,NULL)>max(JUNCTION_CLEARANCE,pRod->size_wide+pElseRod->size_wide))
				continue;	//端点距离太远为提升效率不进行碰撞检测
			if(pRod->cfgword.And(pElseRod->cfgword))
			{	//必须出现在同一基塔上
				f3dPoint collisionPt;
				static const char STATE_CHECK		=1;	//检测碰撞
				static const char STATE_INTERFERE	=2;	//检测到碰撞
				static const char STATE_COLLISION	=3;	//检测到碰撞（但有共同螺栓连接）
				char* pchState=NULL;
				if(pHashCheckRodCouples)
					pchState=pHashCheckRodCouples->SetValue(DUALKEY(pRod->handle,pElseRod->handle,true),STATE_CHECK);
				if (pRod->pSolidBody==NULL)
					pRod->Create3dSolidModel();
				if (pElseRod->pSolidBody==NULL)
					pElseRod->Create3dSolidModel();
				if(pRod->pSolidBody->IsIntWith(pElseRod->pSolidBody,collisionPt))
				{	//两杆件有碰撞
					CLsRefList* pListHoles1=pRod->GetLsRefList();
					bool hasConnect=false;
					if(pListHoles1)
					{
						for(CLsRef* pHole=pListHoles1->GetFirst();pHole;pHole=pListHoles1->GetNext())
						{
							if(pElseRod->FindLsByHandle(pHole->GetLsPtr()->handle))
							{
								hasConnect=true;
								break;
							}
						}
					}
					if(pchState)
						*pchState=hasConnect?STATE_COLLISION:STATE_INTERFERE;
					if(!hasConnect)
					{	//两杆件间无任何螺栓连接
						CString sName=pRod->GetPartTypeName();
						CString sOthName=pElseRod->GetPartTypeName();						
						CXhChar16 sPartNo1(pRod->GetPartNo());
						if(sPartNo1.GetLength()<=0)
							sPartNo1.Printf("0X%X",pRod->handle);
						CXhChar16 sPartNo2(pElseRod->GetPartNo());
						if(sPartNo2.GetLength()<=0)
							sPartNo2.Printf("0X%X",pElseRod->handle);
						if (pInfoList)
						{
							COutputInfo *pInfo=pInfoList->append();
							pInfo->m_hRelaObj1=pRod->handle;
							pInfo->m_hRelaObj2=pElseRod->handle;
							pInfo->collisionPt=collisionPt;
#ifdef AFX_TARG_ENU_ENGLISH
							pInfo->m_sDescription.Format("%sSeg,%s%s  may collide with %s%s .",(char*)pRod->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#else 
							pInfo->m_sDescription.Format("%s段,%s%s与%s%s间可能发生碰撞。",(char*)pRod->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#endif
							pInfo->m_uType=INFO_CHK_COLLIDE_RODS;
						}
						//
						if(pContext!=NULL&&pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
							return;
					}
				}
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the collision between rods!");
#else 
	DisplayProcess(100,"杆件碰撞检测进度!");
#endif
}

void ExecuteRodsCollideCheck(DWORD dwParam)
{
	ExecuteRodsCollideCheckAndReflect(dwParam,NULL);
}
struct CHECKROD {
	CLDSLinePart* pRod;			//连接杆件
	double dfTolerRadius;		//杆件实体碰撞检测的涵盖半径（相对于xTipStart->xTipEnd）
	UINT uidTipStartBelongGlobe;//杆件始端当前归属的碰撞检测球
	UINT uidTipEndBelongGlobe;	//杆件末端当前归属的碰撞检测球
	GEPOINT lenStdVec;			//杆件的标准单位化轴向矢量
	GEPOINT xTipStart,xTipEnd;	//杆件始末端点
	GEPOINT xFrameLineStart,xFrameLineEnd;	//杆件单线始末端点
	CHECKROD() {
		pRod=NULL;
		uidTipStartBelongGlobe=uidTipEndBelongGlobe=0;
		dfTolerRadius=200;
	}
};
#include "ArrayList.h"
struct GLOBECENTER {
protected:
	double _dfRadius;
	double _dfRadiusSquare;	//半径平方（取平方值目的主要是为了避免开方，提升计算效率)
public:
	double get_dfRadius() { return _dfRadius; }
	double set_dfRadius(double value) { 
		_dfRadiusSquare=value*value;
		return _dfRadius=value;
	}
	__declspec(property(put=set_dfRadius,get=get_dfRadius)) double dfRadius;
	double get_dfRadiusSquare() {
		return _dfRadiusSquare;
	}
	__declspec(property(get=get_dfRadiusSquare)) double dfRadiusSquare;
	CLDSNode* pKeyNode;
	CFGWORD cfgword;
	//SIMPLE_ARRAY<CLDSNode*,PRESET_OBJS16> xarrLiveNodes;	//主要用于代表该球的共存空间
	GEPOINT center;
	GLOBECENTER(CLDSNode* pNode=NULL) {
		pKeyNode=pNode;
		dfRadius=300;	//目前取默认值为最大角钢肢宽
		if (pNode)
		{
			center=pNode->xOriginalPos;
			cfgword=pNode->cfgword;
			if (!pNode->IsLegObj())
				cfgword.ToBodyWord();
		}
	};
};
struct CHECK_GLOBE : public GLOBECENTER{
	UINT id;
	CHECK_GLOBE() {
		id=0;
	}
	CHECK_GLOBE(GLOBECENTER& globe) {
		id=0;
		CopyFrom(globe);
	}
	void CopyFrom(GLOBECENTER& globe) {
		center=globe.center;
		_dfRadius=globe.dfRadius;
		_dfRadiusSquare=globe.dfRadiusSquare;	//半径平方（取平方值目的主要是为了避免开方，提升计算效率)
		pKeyNode=globe.pKeyNode;
		cfgword=globe.cfgword;
	}
	void CoverPoint(const double* pxPoint,double dfExpandRadius)
	{
		GEPOINT xPoint(pxPoint);
		GEPOINT vj=xPoint-center;
		double distsquare=vj.x*vj.x+vj.y*vj.y+vj.z*vj.z;
		double bordersquare=dfRadius<=dfExpandRadius?0:(this->dfRadius-dfExpandRadius)*(this->dfRadius-dfExpandRadius);
		if (distsquare<bordersquare)
			return;	//该点原本就被包在检测球范围内
		double dist=sqrt(distsquare);
		vj/=dist;	//单位化
		center=center+vj*((-dfRadius+dist+dfExpandRadius)*0.5);
		dfRadius=0.5*(dfRadius+dist+dfExpandRadius);
	}
	struct ROD {
		char ciChkTipS0E1M2;	//0.检测始端;1.检测终端;2.检测末端
		long hRod;
		CLDSLinePart* pRod;
		double dfTolerRadius;		//杆件实体碰撞检测的涵盖半径（相对于xTipStart->xTipEnd）
		ROD() { hRod=0;pRod=NULL; dfTolerRadius=0; }
		void SetKey(DWORD key) { hRod=key; ciChkTipS0E1M2=2; }
	};
	CHashListEx<ROD> hashGlobeRods;
};
class CRodShareBoltDetector {
	struct BOLTOBJ {
		long hBolt;
		BOLTOBJ* pLinkNext;
		BOLTOBJ() {
			hBolt=NULL;
			pLinkNext=NULL;
		}
		BOOL DeleteSonNode()
		{
			if(pLinkNext!=NULL)
			{
				pLinkNext->DeleteSonNode();
				delete pLinkNext;
				pLinkNext=NULL;
				return TRUE;
			}
			return FALSE;
		}
		~BOLTOBJ() {
			DeleteSonNode();
		}
		BOLTOBJ* GetBolt(CLDSBolt* pBolt) {
			if (pBolt==NULL)
				return NULL;
			if (hBolt==0)
				return false;
			else if (hBolt==pBolt->handle)
				return this;
			else if (pLinkNext!=NULL)
				return pLinkNext->GetBolt(pBolt);
			else //if(pLinkNext==NULL)
				return false;
		}
		BOLTOBJ* AppendBolt(CLDSBolt* pBolt) {
			if (pBolt==NULL)
				return NULL;
			if (hBolt==0)
				hBolt=pBolt->handle;
			else if (hBolt==pBolt->handle)
				return this;
			else if (pLinkNext!=NULL)
				return pLinkNext->AppendBolt(pBolt);
			else //if(pLinkNext==NULL)
			{
				pLinkNext=new BOLTOBJ();
				pLinkNext->hBolt=pBolt->handle;
				return pLinkNext;
			}
			return this;
		}
	};
	BOLTOBJ xarrBolts[256];
public:
	CRodShareBoltDetector(CLDSLinePart* pRod=NULL) {
		InitDetector(pRod);
	}
	void InitDetector(CLDSLinePart* pRod)
	{
		for (int i=0;i<256;i++)
		{
			xarrBolts[i].hBolt=NULL;
			xarrBolts[i].DeleteSonNode();
		}
		for (CLsRef* pLsRef=pRod->GetFirstLsRef();pLsRef;pLsRef=pRod->GetNextLsRef())
		{
			UINT uidHashCode=pLsRef->pBolt->handle&0x000000ff;
			xarrBolts[uidHashCode].AppendBolt(pLsRef->pBolt);
		}
	}
	static bool CheckOverlappedBolt(CLDSBolt* pBolt, CLDSBolt* pExterBolt) {
		if (pExterBolt==NULL||pBolt==NULL)
			return false;
		else if (pBolt==pExterBolt)
			return true;
		GEPOINT vv=pExterBolt->ucs.origin-pBolt->ucs.origin;
		if (fabs(vv.x)+fabs(vv.y)+fabs(vv.z)<pBolt->L+pBolt->L)
		{	//粗判两螺栓有可能重叠
			GEPOINT dv=vv^pBolt->ucs.axis_z;
			double dd=dv.mod();	//原点到另一螺栓轴线的垂直投影距离
			if (dd<pBolt->d&&fabs(vv*pBolt->ucs.axis_z)<pBolt->L)
			{	//两螺栓横向距离小于螺栓直径，轴向距离小于螺栓杆长，认为可能存在螺栓重叠
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X螺栓与0x%X螺栓可能重叠在一起，请核查数据正确性",pBolt->handle,pExterBolt->handle);
				return true;
			}
		}
		return false;
	}
	bool ShareBoltsWith(CLDSLinePart* pElsRod)
	{
		for (CLsRef* pLsRef=pElsRod->GetFirstLsRef();pLsRef;pLsRef=pElsRod->GetNextLsRef())
		{
			UINT uidHashCode=pLsRef->pBolt->handle&0x000000ff;
			if (xarrBolts[uidHashCode].GetBolt(pLsRef->pBolt)!=NULL)
				return true;
		}
		return false;
	}
};
void ExecuteRodsCollideCheckExAndTest(DWORD dwParam,bool blExportTestReport=true)
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=pContext!=NULL?(CMainFrame*)pContext->m_pMainWnd:NULL;
	CTaskOutputList* pInfoList=pContext!=NULL?(CTaskOutputList*)pContext->m_pOutputInfoList:NULL;
	SCOPE_STRU *pScope=pContext!=NULL?(SCOPE_STRU*)pContext->m_pTag:NULL;
	//
#ifdef __ALFA_TEST_VERSION_
	CHash64List<char> hashCheckRodCouples;
	if(blExportTestReport)
		ExecuteRodsCollideCheckAndReflect(0,&hashCheckRodCouples);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the collision between rods!");
#else 
	DisplayProcess(0,"杆件碰撞检测进度-开始准备!");
#endif
	//关于相互焊接的构件间的判断(钢管与板,板与板等)还未完善... 钢管夹板及延伸钢板等
	CLDSLinePart *pRod=NULL,*pElseRod=NULL;
	//汇总交汇节点
	CXhSimpleList<GLOBECENTER> listJoints;
	for (pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{	//清零状态
		if (pRod->pStart)
			pRod->pStart->feature=0;
		if (pRod->pEnd)
			pRod->pEnd->feature=0;
	}
	for (pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if (pRod->pStart&&pRod->pStart->feature==0)
		{
			listJoints.Append(GLOBECENTER(pRod->pStart));
			pRod->pStart->feature=1;
		}
		if (pRod->pEnd&&pRod->pEnd->feature==0)
		{
			listJoints.Append(GLOBECENTER(pRod->pEnd));
			pRod->pEnd->feature=1;
		}
	}
	SIMPLE_STACK<PVOID> liststack;
	listJoints.AttachStack(&liststack);
	GLOBECENTER* pCenter;
	for (pCenter=listJoints.EnumObjectFirst();pCenter;pCenter=listJoints.EnumObjectNext())
	{
		listJoints.push_stack();
		CFGWORD cfgword=pCenter->cfgword;
		for (GLOBECENTER* pElsCenter=listJoints.EnumObjectNext();pElsCenter;pElsCenter=listJoints.EnumObjectNext())
		{	//目前设定间距在120以内的节点自动聚合成一个交汇结点球
			GEPOINT vj=pElsCenter->center-pCenter->center;
			if (fabs(vj.x)+fabs(vj.y)+fabs(vj.z)>210)	//3*(120/sqrt(3)}=207≈210
				continue;
			if(!cfgword.And(pElsCenter->cfgword))
				continue;
			double distsquare=vj.x*vj.x+vj.y*vj.y+vj.z*vj.z;
			if (distsquare>14400)
				continue;	//两节点距离大于120
			if (distsquare<1)
			{
				listJoints.DeleteCursor();
				continue;
			}
			double dist=sqrt(distsquare);
			vj/=dist;	//单位化
			pCenter->center=pCenter->center+vj*((-pCenter->dfRadius+dist+pElsCenter->dfRadius)*0.5);
			pCenter->dfRadius=0.5*(pCenter->dfRadius+dist+pElsCenter->dfRadius);
			pCenter->cfgword.AddSpecWord(pElsCenter->cfgword);
		}
		listJoints.pop_stack();
	}
	UINT id=1;
	CHECK_GLOBE* pGlobe;
	CXhSimpleList<CHECK_GLOBE> listGlobes;
	CHashList<CHECKROD> hashCheckRods;
	for (pCenter=listJoints.EnumObjectFirst();pCenter;pCenter=listJoints.EnumObjectNext(),id++)
	{
		if (pScope && !pScope->IsIncludePoint(pCenter->center,pCenter->dfRadius))
			continue;	//不在指定空间区域内的杆件
		pGlobe=listGlobes.AttachObject();
		pGlobe->id=id;
		pGlobe->CopyFrom(*pCenter);
		for (int m=0;m<2;m++)
		{	//mode=0：先检测短角钢（可能存在扩大检测球半径可能);mode=1:再检测普通有端节点杆件
			for (pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if (pRod->m_bVirtualPart||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
					continue;	//组合角钢和虚角钢不进行碰撞检测
				if (m==0&&pRod->pStart!=NULL&&pRod->pEnd!=NULL)
					continue;	//当前需要检测的是无端节点的短角钢，跳过单线杆件
				else if (m==1&&(pRod->pStart==NULL||pRod->pEnd==NULL))
					continue;	//当前需要检测的是单线杆件（有端节点），跳过短角钢
				if (!pCenter->cfgword.And(pRod->cfgword))
					continue;
				CHECKROD* pCheckRod=hashCheckRods.Add(pRod->handle);
				if (pCheckRod->pRod==NULL)
				{
					pCheckRod->pRod=pRod;
					pCheckRod->lenStdVec=pRod->xPosEnd-pRod->xPosStart;
					normalize(pCheckRod->lenStdVec);
					pCheckRod->xTipStart=pRod->xPosStart-pCheckRod->lenStdVec*pRod->dfStartOdd;
					pCheckRod->xTipEnd=pRod->xPosEnd+pCheckRod->lenStdVec*pRod->dfEndOdd;
					if (pRod->pStart!=NULL&&pRod->pEnd!=NULL)
					{
						if (pRod->dfStartOdd<=0)	//不考虑负头影响，否则会漏掉塔腿斜材的碰撞检测（往往负头较长）
							pCheckRod->xFrameLineStart=pRod->pStart->xOriginalPos;
						else
							pCheckRod->xFrameLineStart=pRod->pStart->xOriginalPos-pCheckRod->lenStdVec*pRod->dfStartOdd;
						if (pRod->dfEndOdd<=0)
							pCheckRod->xFrameLineEnd=pRod->pEnd->xOriginalPos;
						else
							pCheckRod->xFrameLineEnd=pRod->pEnd->xOriginalPos+pCheckRod->lenStdVec*pRod->dfEndOdd;
					}
				}
				GEPOINT vs,ve;
				bool added=false;
				for (int mode=0;mode<2&&!added;mode++)
				{
					if (mode==0&&pCheckRod->xFrameLineStart.IsEqual(pCheckRod->xFrameLineEnd))
						continue;	//不存在单线坐标
					//存在单线坐标时可优先通过单线坐标判断，以避免角钢空间位置移动带来的判断偏差
					GEPOINT xTipStart=mode==0?pCheckRod->xFrameLineStart:pCheckRod->xTipStart;
					GEPOINT xTipEnd=mode==0?pCheckRod->xFrameLineEnd:pCheckRod->xTipEnd;
					vs=xTipStart-pCenter->center;
					ve=xTipEnd-pCenter->center;
					added=false;
					if (pCheckRod->uidTipStartBelongGlobe==0)
					{
						double dfCheckDiameter=2*max(pRod->size_wide,pGlobe->dfRadius);
						if (fabs(vs.x)+fabs(vs.y)+fabs(vs.z)>dfCheckDiameter)
							added=false;	//粗判一定不在碰撞检测球内
						else if (vs.x*vs.x+vs.y*vs.y+vs.z*vs.z>pGlobe->dfRadiusSquare)
							added=false;	//细判确定不在碰撞检测球内
						else
						{
							pCheckRod->uidTipStartBelongGlobe=id;
							CHECK_GLOBE::ROD* pGlobeRod=pGlobe->hashGlobeRods.Add(pRod->handle);
							if (pRod->IsTube())
								pGlobeRod->dfTolerRadius=pRod->size_wide*0.5;
							else
								pGlobeRod->dfTolerRadius=pRod->size_wide;
							pGlobeRod->pRod=pRod;
							pGlobeRod->ciChkTipS0E1M2=0;
							added=true;
						}
					}
					if (!added&&pCheckRod->uidTipEndBelongGlobe==0)
					{
						double dfCheckDiameter=2*max(pRod->size_wide,pGlobe->dfRadius);
						if (fabs(ve.x)+fabs(ve.y)+fabs(ve.z)>dfCheckDiameter)
							added=false;	//粗判一定不在碰撞检测球内
						else if (ve.x*ve.x+ve.y*ve.y+ve.z*ve.z>pGlobe->dfRadiusSquare)
							added=false;	//细判确定不在碰撞检测球内
						else
						{
							pCheckRod->uidTipStartBelongGlobe=id;
							CHECK_GLOBE::ROD* pGlobeRod=pGlobe->hashGlobeRods.Add(pRod->handle);
							if (pRod->IsTube())
								pGlobeRod->dfTolerRadius=pRod->size_wide*0.5;
							else
								pGlobeRod->dfTolerRadius=pRod->size_wide;
							pGlobeRod->pRod=pRod;
							pGlobeRod->ciChkTipS0E1M2=1;
							added=true;
						}
					}
					if (!added)
					{
						GEPOINT vc=vs;//pCenter->center-pCheckRod->xTipStart;
						GEPOINT vv=vc^pCheckRod->lenStdVec;
						if (vv.x*vv.x+vv.y*vv.y+vv.z*vv.z<=pGlobe->dfRadiusSquare)
						{	//细判杆件中间穿过碰撞检测球
							double ds=vs*pCheckRod->lenStdVec;
							double de=ve*pCheckRod->lenStdVec;
							double dfMinDist=0;
							if (ds>=0&&de>=0)
								dfMinDist=min(ds,de);
							else if (ds<=0&&de<=0)
								dfMinDist=min(-ds,-de);
							else
								dfMinDist=0;//已经通过上面的点到线垂线距离平方值进行了精判，此处无须计算距离值
							if (dfMinDist<pGlobe->dfRadius)
							{
								CHECK_GLOBE::ROD* pGlobeRod=pGlobe->hashGlobeRods.Add(pRod->handle);
								if (pRod->IsTube())
									pGlobeRod->dfTolerRadius=pRod->size_wide*0.5;
								else
									pGlobeRod->dfTolerRadius=pRod->size_wide;
								pGlobeRod->pRod=pRod;
								if (fabs(dfMinDist-fabs(ds))<EPS)
									pGlobeRod->ciChkTipS0E1M2=0;
								else if (fabs(dfMinDist-fabs(de))<EPS)
									pGlobeRod->ciChkTipS0E1M2=1;
								else //if (dfMinDist=0)
									pGlobeRod->ciChkTipS0E1M2=2;
								added=true;
							}
						}
					}
					if(added&&(pRod->pStart==NULL||pRod->pEnd==NULL))
					{	//如果是无端节点的短角钢，须将整个短角钢都包在检测球中
						pGlobe->CoverPoint(pCheckRod->xTipStart,pRod->size_wide);
						pGlobe->CoverPoint(pCheckRod->xTipEnd,pRod->size_wide);
					}
				}
			}
		}
	}
	DisplayProcess(0,"杆件碰撞检测进度-开始检测!");
	const int JUNCTION_CLEARANCE=200;
	int i=1,j=1,count=0,hits=1,index=1;
	for (pGlobe=listGlobes.EnumObjectFirst();pGlobe;pGlobe=listGlobes.EnumObjectNext())
		count+=(pGlobe->hashGlobeRods.Count*(pGlobe->hashGlobeRods.Count-1))/2;
	for (pGlobe=listGlobes.EnumObjectFirst();pGlobe;pGlobe=listGlobes.EnumObjectNext(),i++)
	{
		CHECK_GLOBE::ROD* pGlobeRod,*pElsGlobeRod=NULL;
#ifdef __ALFA_TEST_VERSION_
		if(blExportTestReport)
			logerr.Log("%4d#0x%X节点(%.f,%.f,%.f)检测球，检测半径%.f",i,pGlobe->pKeyNode->handle,
				pGlobe->center.x,pGlobe->center.y,pGlobe->center.z,pGlobe->dfRadius);
#endif
		for (pGlobeRod=pGlobe->hashGlobeRods.GetFirst(),j=1;pGlobeRod;pGlobeRod=pGlobe->hashGlobeRods.GetNext(),j++,hits++)
		{
			char ciTipSymbol='S';
			if (pGlobeRod->ciChkTipS0E1M2==1)
				ciTipSymbol='E';
			else if(pGlobeRod->ciChkTipS0E1M2==2)
				ciTipSymbol='M';
#ifdef __ALFA_TEST_VERSION_
			if(blExportTestReport)
				logerr.Log("    %3d#杆件0x%X-%C",j,pGlobeRod->hRod,ciTipSymbol);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess(2+(98*hits)/count,"Check the collision of rods' same end!");
#else 
			DisplayProcess(2+(98*hits)/count,"同端节点杆件碰撞检测进度!");
#endif
			pGlobe->hashGlobeRods.push_stack();
			pRod=pGlobeRod->pRod;
			CRodShareBoltDetector detector(pRod);
			for (pElsGlobeRod=pGlobe->hashGlobeRods.GetNext();pElsGlobeRod;pElsGlobeRod=pGlobe->hashGlobeRods.GetNext())
			{
				CLDSLinePart* pElseRod=pElsGlobeRod->pRod;
				if (!pRod->ModuleTogetherWith(pElseRod))
					continue;	//不在同一个模型塔位实例空间中不必检测碰撞
				if (pRod->m_hPartWeldParent==pElseRod->handle||pElseRod->m_hPartWeldParent==pRod->handle||
					(pRod->m_hPartWeldParent==pElseRod->m_hPartWeldParent&&pRod->m_hPartWeldParent>0))
					continue;	//两杆件焊接在一起 合理碰撞
				if (pRod->GetClassTypeId()==CLS_LINEANGLE&&pElseRod->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
					CLDSLineAngle* pElseAngle=(CLDSLineAngle*)pElseRod;
					if (pRod->huoqu_handle>0X20&&pRod->huoqu_handle==pElseRod->huoqu_handle)//pJg->sub_type&TA_JG_HUOQUSEG&&pOthJg->sub_type&TA_JG_HUOQUSEG&&
						continue;	//同一根火曲角钢上的火曲子角钢间不判断
					if (pAngle->hGroupAngle>0x20&&pAngle->hGroupAngle==pElseAngle->hGroupAngle)
						continue;	//隶属于同一根组合角钢
					if (detector.ShareBoltsWith(pElseAngle))
						continue;	//通过共用连接螺栓来区分合理碰撞比之前通过父杆件及位置搭接关系更严谨准确 wjh-2020.1.18
					//TODO:未处理因为螺栓重叠导致的连接角钢碰撞问题
					//CheckOverlappedBolt()
				}
#ifdef __ALFA_TEST_VERSION_
				if (blExportTestReport)
				{
					char *pchState=hashCheckRodCouples.GetValue(DUALKEY(pRod->handle,pElseRod->handle,true));
					if (pchState!=NULL)
						*pchState|=0x80;
					else
					{
						char ciTipSymbol='S';
						if (pGlobeRod->ciChkTipS0E1M2==1)
							ciTipSymbol='E';
						else if (pGlobeRod->ciChkTipS0E1M2==2)
							ciTipSymbol='M';
						char ciElsTipSymbol='S';
						if (pElsGlobeRod->ciChkTipS0E1M2==1)
							ciElsTipSymbol='E';
						else if (pElsGlobeRod->ciChkTipS0E1M2==2)
							ciElsTipSymbol='M';
						char *label ="",*label2="";
						if (pRod->pStart==NULL||pRod->pEnd==NULL)
							label ="(包铁)";
						else if (pRod->pStart->hFatherPart==pElseRod->handle||pRod->pEnd->hFatherPart==pElseRod->handle)
							label = "(搭接)";
						if (pElseRod->pStart==NULL||pElseRod->pEnd==NULL)
							label2 ="(包铁)";
						else if (pElseRod->pStart->hFatherPart==pRod->handle||pElseRod->pEnd->hFatherPart==pRod->handle)
							label2 ="(搭接)";
						logerr.Log("    补检%3d#杆件0x%X-%C%s与0x%X-%C%s",index,pGlobeRod->hRod,ciTipSymbol,label,pElsGlobeRod->hRod,ciElsTipSymbol,label2);
						index++;
					}
				}
#endif
				f3dPoint collisionPt;
				if (pRod->pSolidBody==NULL)
					pRod->Create3dSolidModel();
				if (pElseRod->pSolidBody==NULL)
					pElseRod->Create3dSolidModel();
				if (pRod->pSolidBody->IsIntWith(pElseRod->pSolidBody,collisionPt))
				{	//两杆件间无任何螺栓连接, 且有实体碰撞
					CString sName=pRod->GetPartTypeName();
					CString sOthName=pElseRod->GetPartTypeName();
					CXhChar16 sPartNo1(pRod->GetPartNo());
					if (sPartNo1.GetLength()<=0)
						sPartNo1.Printf("0X%X",pRod->handle);
					CXhChar16 sPartNo2(pElseRod->GetPartNo());
					if (sPartNo2.GetLength()<=0)
						sPartNo2.Printf("0X%X",pElseRod->handle);
					if (pInfoList)
					{
						COutputInfo *pInfo=pInfoList->append();
						pInfo->m_hRelaObj1=pRod->handle;
						pInfo->m_hRelaObj2=pElseRod->handle;
						pInfo->collisionPt=collisionPt;
#ifdef AFX_TARG_ENU_ENGLISH
						pInfo->m_sDescription.Format("%sSeg,%s%s  may collide with %s%s .",(char*)pRod->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#else 
						pInfo->m_sDescription.Format("%s段,%s%s与%s%s间可能发生碰撞。",(char*)pRod->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#endif
						pInfo->m_uType=INFO_CHK_COLLIDE_RODS;
					}
					//
					if (pContext&&pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
						return;
				}
			}
			pGlobe->hashGlobeRods.pop_stack();
		}
	}
#ifdef __ALFA_TEST_VERSION_
	i=1;
	for (char *pchState=hashCheckRodCouples.GetFirst();blExportTestReport&&pchState!=NULL;pchState=hashCheckRodCouples.GetNext())
	{
		if ((*pchState)&0x80)
			continue;
		DUALKEY dualkey=hashCheckRodCouples.GetCursorKey();
		pRod=Ta.FromRodHandle(dualkey.lowId);
		pElseRod=Ta.FromRodHandle(dualkey.highId);
		char label[32]={ 0 };
		if (pRod!=NULL&&pElseRod!=NULL)
		{
			if (pRod->pStart==NULL||pRod->pEnd==NULL)
				strcpy(label,"(<-包铁)");
			else if (pElseRod->pStart==NULL||pElseRod->pEnd==NULL)
				strcpy(label,"(->包铁)");
			CRodShareBoltDetector detector(pRod);
			if (detector.ShareBoltsWith(pElseRod))
			{
				if (*pchState==3)//STATE_COLLISION=3)
					continue;
				strcpy(label,"(共用螺栓)");
			}
			else
			{
				GEPOINT interspoint;
				if (pRod->pSolidBody->IsIntWith(pElseRod->pSolidBody,interspoint))
					strcat(label,"!!!!!");
			}
		}
		logerr.Log("漏检%3d#杆件0x%X与0x%X,%s",i,dualkey.lowId,dualkey.highId,label);
		i++;
	}
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the collision between rods!");
#else 
	DisplayProcess(100,"杆件碰撞检测进度!");
#endif
}
void ExecuteRodsCollideCheckEx(DWORD dwParam)
{
	ExecuteRodsCollideCheckExAndTest(dwParam,false);
}

//2.杆件螺栓碰撞检测	//慢...
void ExecuteRodBoltCollideCheck(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	SCOPE_STRU *pScope=(SCOPE_STRU*)pContext->m_pTag;

#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the collision between rods and bolts！");
#else 
	DisplayProcess(0,"杆件与螺栓碰撞检测进度！");
#endif
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	f3dPoint ls_pos;
	SCOPE_STRU scope;
	CLDSBolt *pLs=NULL;
	CLDSLinePart *pLinePart=NULL;
	int i=0,n=Ta.Parts.GetNodeNum(CLS_LINEPART);
	for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Check the collision between rods and bolts！");
#else 
		DisplayProcess((100*i)/n,"杆件与螺栓碰撞检测进度！");
#endif
		if(pLinePart->m_bVirtualPart)
			continue;	//被拆分角钢不进行碰撞检测
		if(pScope && !pScope->IsIntersWithLine(pLinePart->Start(),pLinePart->End()))
			continue;	//不在指定空间区域内的杆件
		f3dLine axis_line;
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pJg=(CLDSLineAngle*)pLinePart;
			axis_line.startPt=pJg->Start()+pJg->GetWingVecX()*pJg->GetWidth()/2.0+pJg->GetWingVecY()*pJg->GetWidth()/2.0;
			axis_line.endPt=pJg->End()+pJg->GetWingVecX()*pJg->GetWidth()/2.0+pJg->GetWingVecY()*pJg->GetWidth()/2.0;
		}
		scope.ClearScope();
		int vertex_n=pLinePart->pSolidBody->KeyPointNum();
		//for(f3dPoint *pPt=pLinePart->pBody->vertex.GetFirst();pPt;pPt=pLinePart->pBody->vertex.GetNext())
		for(int vi=0;vi<vertex_n;vi++)
		{
			f3dPoint vertex=pLinePart->pSolidBody->GetKeyPointAt(vi);
			scope.VerifyVertex(vertex);
		}
		CLdsPartListStack stack_p(&Ta.Parts);
		for(pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pLs->m_bVirtualPart)
				continue;	//非统计螺栓不进行碰撞检测
			ls_pos=pLs->ucs.origin;
			if(pScope && !pScope->IsIncludePoint(pLs->ucs.origin))
				continue;	//不在指定空间区域内的螺栓
			if(ls_pos.x<scope.fMinX-pLs->get_L())
				continue;
			else if(ls_pos.x>=scope.fMaxX+pLs->get_L())
				continue;
			else if(ls_pos.y<=scope.fMinY-pLs->get_L())
				continue;
			else if(ls_pos.y>=scope.fMaxY+pLs->get_L())
				continue;
			else if(ls_pos.z<=scope.fMinZ-pLs->get_L())
				continue;
			else if(ls_pos.z>=scope.fMaxZ+pLs->get_L())
				continue;
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(DistOf3dPtLine(ls_pos,axis_line.startPt,axis_line.endPt)>((CLDSLineAngle*)pLinePart)->GetWidth()+pLs->get_L())
					continue;
			}
			if(pLinePart->FindLsByHandle(pLs->handle))
				continue;	//螺栓在杆件上
			if(!pLinePart->cfgword.And(pLs->cfgword))
				continue;	//不在同一个模型中不必检测碰撞
			f3dPoint collisionPt;
			if(pLinePart->pSolidBody->IsIntWith(pLs->pSolidBody,collisionPt))
			{
				CString sName=pLinePart->GetPartTypeName();
				CXhChar16 sPartNo(pLinePart->GetPartNo());
				if(sPartNo.GetLength()<=0)
					sPartNo.Printf("0X%X",pLinePart->handle);
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLinePart->handle;
				pInfo->m_hRelaObj2=pLs->handle;
				pInfo->collisionPt=collisionPt;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("%sSeg,The bolt of %s%s may collide with the bolt of 0X%X 。",(char*)pLinePart->iSeg.ToString(),(char*)sPartNo,sName,pLs->handle);
#else 
				pInfo->m_sDescription.Format("%s段,%s%s与0X%X螺栓间可能发生碰撞。",(char*)pLinePart->iSeg.ToString(),(char*)sPartNo,sName,pLs->handle);
#endif
				pInfo->m_uType=INFO_CHK_COLLIDE_RODBOLT;
				//
				if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					DisplayProcess(100,"Check the collision between rods and bolts！");
#else 
					DisplayProcess(100,"杆件与螺栓碰撞检测进度！");
#endif
					return;
				}
			}
			
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the collision between rods and bolts！");
#else 
	DisplayProcess(100,"杆件与螺栓碰撞检测进度！");
#endif
}
//3.板螺栓碰撞检测进度
void ExecutePlateBoltCollideCheck(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	SCOPE_STRU *pScope=(SCOPE_STRU*)pContext->m_pTag;
	
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the collision between plates and bolts!");
#else 
	DisplayProcess(0,"板与螺栓碰撞检测进度！");
#endif
	int i=0,n=Ta.Parts.GetNodeNum(CLS_PLATE)+Ta.Parts.GetNodeNum(CLS_PARAMPLATE);
	SCOPE_STRU scope;
	CLDSBolt *pLs=NULL;
	CSuperSmartPtr<CLDSPart>pPlate=NULL;
	for(pPlate=Ta.Parts.GetFirst();pPlate.IsHasPtr();pPlate=Ta.Parts.GetNext())
	{
		if(!pPlate->IsPlate())
			continue;
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Check the collision between plates and bolts!！");
#else 
		DisplayProcess((100*i)/n,"板与螺栓碰撞检测进度！");
#endif
		if(pScope && !pScope->IsIncludePoint(pPlate->ucs.origin))
			continue;	//基点不在指定空间区域内的边板
		if(pPlate->GetClassTypeId()==CLS_PLATE)
		{
			if(pPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
				continue;	//无板
		}
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
				continue;	//无板
		}
		scope.ClearScope();
		int vertex_n=pPlate->pSolidBody->KeyPointNum();
		//for(f3dPoint *pPt=pPlate->pBody->vertex.GetFirst();pPt;pPt=pPlate->pBody->vertex.GetNext())
		for(int vi=0;vi<vertex_n;vi++)
		{
			f3dPoint vertex=pPlate->pSolidBody->GetKeyPointAt(vi);
			scope.VerifyVertex(vertex);
		}
		CLdsPartListStack stack_p(&Ta.Parts);
		for(pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pLs->m_bVirtualPart)
				continue;	//非统计螺栓不进行碰撞检测
			if(pScope && !pScope->IsIncludePoint(pLs->ucs.origin))
				continue;	//不在指定空间区域内的螺栓
			f3dPoint ls_pos=pLs->ucs.origin;
			if(ls_pos.x<scope.fMinX-pLs->get_L())
				continue;
			else if(ls_pos.x>=scope.fMaxX+pLs->get_L())
				continue;
			else if(ls_pos.y<=scope.fMinY-pLs->get_L())
				continue;
			else if(ls_pos.y>=scope.fMaxY+pLs->get_L())
				continue;
			else if(ls_pos.z<=scope.fMinZ-pLs->get_L())
				continue;
			else if(ls_pos.z>=scope.fMaxZ+pLs->get_L())
				continue;
			if(pPlate->GetClassTypeId()==CLS_PLATE)
			{
				if(pPlate.PlatePointer()->FindLsByHandle(pLs->handle))
					continue;
			}
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				if(pPlate.ParamPlatePointer()->FindLsByHandle(pLs->handle))
					continue;
			}
			if(!pPlate->cfgword.And(pLs->cfgword))
				continue;	//不在同一个模型中不必检测碰撞
			f3dPoint collisionPt;
			if(pPlate->pSolidBody->IsIntWith(pLs->pSolidBody,collisionPt))
			{
				CString sName=pPlate->GetPartTypeName();
				CXhChar16 sPartNo(pPlate->GetPartNo());
				if(sPartNo.GetLength()<=0)
					sPartNo.Printf("0X%X",pPlate->handle);
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPlate->handle;
				pInfo->m_hRelaObj2=pLs->handle;
				pInfo->collisionPt=collisionPt;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("%sSeg，The bolt of %s%s may collide with the bolt of 0X%X 。",(char*)pPlate->iSeg.ToString(),(char*)sPartNo,sName,pLs->handle);
#else 
				pInfo->m_sDescription.Format("%s段,%s%s与0X%X螺栓间可能发生碰撞。",(char*)pPlate->iSeg.ToString(),(char*)sPartNo,sName,pLs->handle);
#endif
				pInfo->m_uType=INFO_CHK_COLLIDE_PLATEBOLT;
				//
				if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					DisplayProcess(100,"Check the collision between plates and bolts!！");
#else 
					DisplayProcess(100,"板与螺栓碰撞检测进度！");
#endif
					return;
				}
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the collision between plates and bolts!！");
#else 
	DisplayProcess(100,"板与螺栓碰撞检测进度！");
#endif
}
//4.螺栓与螺栓碰撞检测进度
void ExecuteBoltsCollideCheck(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	SCOPE_STRU *pScope=(SCOPE_STRU*)pContext->m_pTag;
	
	int n=Ta.Parts.GetNodeNum(CLS_BOLT);
	long ii=0,nn=n;//*(n-1)/2;
	SCOPE_STRU scope,otherScope;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the collision of bolts!！");
#else 
	DisplayProcess(0,"螺栓与螺栓碰撞检测进度！");
#endif
	for(CLDSBolt *pLs=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pLs;pLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
	{
		ii++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*ii)/nn,"Check the collision of bolts!！");	
#else 
		DisplayProcess((100*ii)/nn,"螺栓与螺栓碰撞检测进度！");	
#endif
		if(pLs->m_bVirtualPart)
			continue;	//非统计螺栓不进行碰撞检测
		if(pScope && !pScope->IsIncludePoint(pLs->ucs.origin))
			continue;	//不在指定空间区域内的螺栓
		scope.ClearScope();
		int vertex_n=pLs->pSolidBody->KeyPointNum();
		for(int vi=0;vi<vertex_n;vi++)
		{
			f3dPoint vertex=pLs->pSolidBody->GetKeyPointAt(vi);
			scope.VerifyVertex(vertex);
		}
		CLdsPartListStack stack_p(&Ta.Parts);
		for(CLDSBolt *pOthLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT);pOthLs;pOthLs=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pOthLs->m_bVirtualPart)
				continue;	//非统计螺栓不进行碰撞检测
			if(pScope && !pScope->IsIncludePoint(pOthLs->ucs.origin))
				continue;	//不在指定空间区域内的螺栓
			//粗判
			if(fabs(pLs->get_norm()*pOthLs->get_norm())>EPS_COS2)
			{
				if(DISTANCE(pLs->ucs.origin,pOthLs->ucs.origin)>0.5*(pLs->get_d()+pOthLs->get_d()))
					continue;
			}
			else if(DISTANCE(pLs->ucs.origin,pOthLs->ucs.origin)>pLs->get_L()+pOthLs->get_L()+0.5*(pLs->get_d()+pOthLs->get_d()))
				continue;	//太远无检测必要，肯定不碰
			if(!pLs->cfgword.And(pOthLs->cfgword))
				continue;	//不在同一个模型中不必检测碰撞
			otherScope.ClearScope();
			vertex_n=pOthLs->pSolidBody->KeyPointNum();
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pOthLs->pSolidBody->GetKeyPointAt(vi);
				otherScope.VerifyVertex(vertex);
			}
			if(!scope.IsIntersWith(otherScope))
				continue;
			f3dPoint collisionPt;
			if(pLs->pSolidBody->IsIntWith(pOthLs->pSolidBody,collisionPt))
			{
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLs->handle;
				pInfo->m_hRelaObj2=pOthLs->handle;
				pInfo->collisionPt=collisionPt;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("%sSeg,0X%X bolt may collide with bolt 0X%X。",(char*)pLs->iSeg.ToString(),pLs->handle,pOthLs->handle);
#else 
				pInfo->m_sDescription.Format("%s段,0X%X螺栓与0X%X螺栓间可能发生碰撞。",(char*)pLs->iSeg.ToString(),pLs->handle,pOthLs->handle);
#endif
				pInfo->m_uType=INFO_CHK_COLLIDE_BOLTS;
				//
				if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					DisplayProcess(100,"Check the collision of bolts!！");
#else 
					DisplayProcess(100,"螺栓与螺栓碰撞检测进度！");
#endif
					return;
				}
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the collision of bolts!！");
#else 
	DisplayProcess(100,"螺栓与螺栓碰撞检测进度！");
#endif
}
//5.杆件与板碰撞检测进度
void ExecuteRodPlateCollideCheck(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	SCOPE_STRU *pScope=(SCOPE_STRU*)pContext->m_pTag;
	
	int i=0,n=Ta.Parts.GetNodeNum(CLS_PLATE)+Ta.Parts.GetNodeNum(CLS_PARAMPLATE);;
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the collision between plates and rods！");
#else 
	DisplayProcess(0,"杆件与板碰撞检测进度！");
#endif
	CSuperSmartPtr<CLDSPart>pPlate=NULL;
	CLDSLinePart *pLinePart=NULL;
	for(pPlate=Ta.Parts.GetFirst();pPlate.IsHasPtr();pPlate=Ta.Parts.GetNext())
	{
		if(!pPlate->IsPlate())
			continue;
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Check the collision between plates and rods！");
#else 
		DisplayProcess((100*i)/n,"杆件与板碰撞检测进度！");
#endif
		if(pScope && !pScope->IsIncludePoint(pPlate->ucs.origin))
			continue;	//基点不在指定空间区域内的边板
		if(pPlate->GetClassTypeId()==CLS_PLATE)
		{
			if(pPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
				continue;	//无板
		}
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
				continue;	//无板
		}
		SCOPE_STRU plate_scope;
		int vertex_n=pPlate->pSolidBody->KeyPointNum();
		//for(f3dPoint *pPt=pPlate->pBody->vertex.GetFirst();pPt;pPt=pPlate->pBody->vertex.GetNext())
		for(int vi=0;vi<vertex_n;vi++)
		{
			f3dPoint vertex=pPlate->pSolidBody->GetKeyPointAt(vi);
			plate_scope.VerifyVertex(vertex);
		}
		CLdsPartListStack stack_p(&Ta.Parts);
		for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
		{
			if(pLinePart->m_bVirtualPart)
				continue;	//被拆分角钢不进行碰撞检测
			if( pLinePart->m_hPartWeldParent==pPlate->handle||pLinePart->handle==pPlate->m_hPartWeldParent||(
				pLinePart->m_hPartWeldParent==pPlate->m_hPartWeldParent&&pLinePart->m_hPartWeldParent>0))
				continue;
			if(pScope && !pScope->IsIntersWithLine(pLinePart->Start(),pLinePart->End()))
				continue;	//两端楞点不在指定空间区域内的角钢
			if(!pPlate->cfgword.And(pLinePart->cfgword))
				continue;	//不在同一个模型中不必检测碰撞
			BOOL bNotChk=FALSE;
			if(pPlate->GetClassTypeId()==CLS_PLATE&&pPlate.PlatePointer()->jdb_style==3)//钢管夹板
			{
				for(CDesignLjPartPara *pLjPartPara=pPlate.PlatePointer()->designInfo.partList.GetFirst();
				pLjPartPara;pLjPartPara=pPlate.PlatePointer()->designInfo.partList.GetNext())
				{
					if(pLjPartPara->hPart==pLinePart->handle)
					{
						bNotChk=TRUE; 
						break;	
					}
				}
			}	
			if(bNotChk)	//钢管夹板与其关联构件为合理碰撞
				continue;
			if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{	//肘板与其基准钢管的碰撞为合理碰撞
				long h=0;
				if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_ELBOWPLATE)
					pPlate.ParamPlatePointer()->GetDesignItemValue('K',&h);
				if(pLinePart->handle==abs(h))
					continue;
			}
			bNotChk=FALSE;	//如同是互相连接的角钢和板间不检测碰撞
			if(pPlate->GetClassTypeId()==CLS_PLATE)
			{
				for(CLsRef *pLsRef=pPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.PlatePointer()->GetNextLsRef())
				{	
					if(pLinePart->FindLsByHandle((*pLsRef)->handle))
					{
						bNotChk=TRUE;
						break;
					}
				}
			}
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				for(CLsRef *pLsRef=pPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.ParamPlatePointer()->GetNextLsRef())
				{	
					if(pLinePart->FindLsByHandle((*pLsRef)->handle))
					{
						bNotChk=TRUE;
						break;
					}
				}
			}
			if(bNotChk)
				continue;	//板与杆件相连接
			if(pPlate->m_hPartWeldParent==pLinePart->handle||pLinePart->m_hPartWeldParent==pPlate->handle)
				continue;	//板与杆件焊接在一起
			SCOPE_STRU pole_scope;
			int vertex_n=pLinePart->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pLinePart->pBody->vertex.GetFirst();pPt;pPt=pLinePart->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pLinePart->pSolidBody->GetKeyPointAt(vi);
				pole_scope.VerifyVertex(vertex);
			}
			if(!plate_scope.IsIntersWith(pole_scope))
				continue;
			f3dPoint collisionPt;
			if(pPlate->pSolidBody->IsIntWith(pLinePart->pSolidBody,collisionPt))
			{
				CString sName=pLinePart->GetPartTypeName();
				CString sOthName=pPlate->GetPartTypeName();
				CXhChar16 sPartNo1(pLinePart->GetPartNo());
				if(sPartNo1.GetLength()<=0)
					sPartNo1.Printf("0X%X",pLinePart->handle);
				CXhChar16 sPartNo2(pPlate->GetPartNo());
				if(sPartNo2.GetLength()<=0)
					sPartNo2.Printf("0X%X",pPlate->handle);
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pLinePart->handle;
				pInfo->m_hRelaObj2=pPlate->handle;
				pInfo->collisionPt=collisionPt;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("%s段,%s%s may collide with %s%s。",(char*)pLinePart->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#else 
				pInfo->m_sDescription.Format("%s段,%s%s与%s%s间可能发生碰撞。",(char*)pLinePart->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#endif
				pInfo->m_uType=INFO_CHK_COLLIDE_RODPLATE;
				//
				if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					DisplayProcess(100,"Check the collision between plates and rods！");
#else 
					DisplayProcess(100,"杆件与板碰撞检测进度！");
#endif
					return;
				}
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the collision between plates and rods！");
#else 
	DisplayProcess(100,"杆件与板碰撞检测进度！");
#endif
}
//6.板与板碰撞检测进度
void ExecutePlatesCollideCheck(DWORD dwParam) 
{
	DATACENTERCMD_CONTEXT *pContext=(DATACENTERCMD_CONTEXT*)dwParam;
	CMainFrame* pMainWnd=(CMainFrame*)pContext->m_pMainWnd;
	CTaskOutputList* pInfoList=(CTaskOutputList*)pContext->m_pOutputInfoList;
	SCOPE_STRU *pScope=(SCOPE_STRU*)pContext->m_pTag;
	
	int i=0,n=Ta.Parts.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Check the collision of plates！");
#else 
	DisplayProcess(0,"板与板碰撞检测进度！");
#endif
	CSuperSmartPtr<CLDSPart>pPlate=NULL,pOthPlate=NULL;
	for(pPlate=Ta.Parts.GetFirst();pPlate.IsHasPtr();pPlate=Ta.Parts.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Check the collision of plates！");
#else 
		DisplayProcess((100*i)/n,"板与板碰撞检测进度！");
#endif
		if(pPlate->GetClassTypeId()!=CLS_PLATE&&pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		if(pScope && !pScope->IsIncludePoint(pPlate->ucs.origin))
			continue;	//基点不在指定空间区域内的边板
		if(pPlate->GetClassTypeId()==CLS_PLATE)
		{
			if(pPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
				continue;	//无板
		}
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
				continue;	//无板
		}
		SCOPE_STRU scope; 
		int vertex_n=pPlate->pSolidBody->KeyPointNum();
		//for(f3dPoint *pPt=pPlate->pBody->vertex.GetFirst();pPt;pPt=pPlate->pBody->vertex.GetNext())
		for(int vi=0;vi<vertex_n;vi++)
		{
			f3dPoint vertex=pPlate->pSolidBody->GetKeyPointAt(vi);
			scope.VerifyVertex(vertex);
		}
		CLdsPartListStack stack_p(&Ta.Parts);
		for(pOthPlate=Ta.Parts.GetNext();pOthPlate.IsHasPtr();pOthPlate=Ta.Parts.GetNext())
		{
			if(pOthPlate->GetClassTypeId()!=CLS_PLATE&&pOthPlate->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			if(!pPlate->cfgword.And(pOthPlate->cfgword))
				continue;	//不在同一个模型中不必检测碰撞
			if(pPlate->m_hPartWeldParent>0X20)
			{	//同一焊接组合件内的板不检测
				if(pPlate->m_hPartWeldParent==pOthPlate->handle)
					continue;
				if(pPlate->m_hPartWeldParent==pOthPlate->m_hPartWeldParent)
					continue;
				CLDSPart* pWeldPart=Ta.FromPartHandle(pPlate->m_hPartWeldParent);
				if(pWeldPart && pWeldPart->m_hPartWeldParent>0 && pWeldPart->m_hPartWeldParent==pOthPlate->handle)
					continue;
			}
			if(pOthPlate->m_hPartWeldParent>0X20)
			{	//同一焊接组合件内的板不检测
				if(pPlate->handle==pOthPlate->m_hPartWeldParent)
					continue;
				CLDSPart* pWeldPart=Ta.FromPartHandle(pOthPlate->m_hPartWeldParent);
				if(pWeldPart && pWeldPart->m_hPartWeldParent>0 && pWeldPart->m_hPartWeldParent==pPlate->handle)
					continue;
			}
			//参数化板与其基准钢板的碰撞为合理碰撞
			if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				long h=0,h1=0;
				if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE)
				{
					pPlate.ParamPlatePointer()->GetDesignItemValue('F',&h);
					pPlate.ParamPlatePointer()->GetDesignItemValue('G',&h1);
				}
				else if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRRIBPLATE||
					pPlate.ParamPlatePointer()->m_iParamType==TYPE_TUBERIBPLATE)
				{
					pPlate.ParamPlatePointer()->GetDesignItemValue('E',&h);		//第一块基准钢板由B改为E wht 11-05-13
					pPlate.ParamPlatePointer()->GetDesignItemValue('F',&h1);	//第二块基准钢板由C改为F wht 11-05-13
				}
				else if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_ELBOWPLATE)
					pPlate.ParamPlatePointer()->GetDesignItemValue('L',&h);
				else if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_RIBPLATE)
				{
					pPlate.ParamPlatePointer()->GetDesignItemValue('C',&h);		//辅助构件1
					pPlate.ParamPlatePointer()->GetDesignItemValue('D',&h1);	//辅助构件2
				}
				if(pOthPlate->handle==h||pOthPlate->handle==h1)
					continue;
			}
			if(pOthPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				long h=0,h1=0;
				if(pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE)
				{
					pOthPlate.ParamPlatePointer()->GetDesignItemValue('F',&h);
					pOthPlate.ParamPlatePointer()->GetDesignItemValue('G',&h1);
				}
				else if(pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_CIRRIBPLATE||
					pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_TUBERIBPLATE)
				{
					pOthPlate.ParamPlatePointer()->GetDesignItemValue('E',&h);	//第一块基准钢板由B改为E wht 11-05-13
					pOthPlate.ParamPlatePointer()->GetDesignItemValue('F',&h1);	//第二块基准钢板由C改为F wht 11-05-13
				}
				else if(pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_ELBOWPLATE)
					pOthPlate.ParamPlatePointer()->GetDesignItemValue('L',&h);
				else if(pOthPlate.ParamPlatePointer()->m_iParamType==TYPE_RIBPLATE)
				{
					pOthPlate.ParamPlatePointer()->GetDesignItemValue('C',&h);	//辅助构件1
					pOthPlate.ParamPlatePointer()->GetDesignItemValue('D',&h1);	//辅助构件2
				}
				if(pPlate->handle==h||pPlate->handle==h1)
					continue;
			} 
			//用螺栓相互连接的板为合理碰撞
			if(pOthPlate->GetClassTypeId()==CLS_PLATE)
			{
				if(pOthPlate.PlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//无板
				BOOL bNotChk=FALSE;	
				if(pPlate->GetClassTypeId()==CLS_PLATE)
				{
					for(CLsRef *pLsRef=pPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.PlatePointer()->GetNextLsRef())
					{	
						if(pOthPlate.PlatePointer()->FindLsByHandle((*pLsRef)->handle))
						{
							bNotChk=TRUE;
							break;
						}
					}
				}
				else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					for(CLsRef *pLsRef=pPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.ParamPlatePointer()->GetNextLsRef())
					{	
						if(pOthPlate.PlatePointer()->FindLsByHandle((*pLsRef)->handle))
						{
							bNotChk=TRUE;
							break;
						}
					}
				} 
				if(bNotChk)
					continue;
			}
			else if(pOthPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				if(pOthPlate.ParamPlatePointer()->vertex_list.GetNodeNum()<3)
					continue;	//无板
				BOOL bNotChk=FALSE;	
				if(pPlate->GetClassTypeId()==CLS_PLATE)
				{
					for(CLsRef *pLsRef=pPlate.PlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.PlatePointer()->GetNextLsRef())
					{	
						if(pOthPlate.ParamPlatePointer()->FindLsByHandle((*pLsRef)->handle))
						{
							bNotChk=TRUE;
							break;
						}
					}
				}
				else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					for(CLsRef *pLsRef=pPlate.ParamPlatePointer()->GetFirstLsRef();pLsRef;pLsRef=pPlate.ParamPlatePointer()->GetNextLsRef())
					{	
						if(pOthPlate.ParamPlatePointer()->FindLsByHandle((*pLsRef)->handle))
						{
							bNotChk=TRUE;
							break;
						}
					}
				} 
				if(bNotChk)
					continue;
			}
			if(pScope && !pScope->IsIncludePoint(pOthPlate->ucs.origin))
				continue;	//基点不在指定空间区域内的边板
			SCOPE_STRU oth_scope;
			int vertex_n=pOthPlate->pSolidBody->KeyPointNum();
			//for(f3dPoint *pPt=pOthPlate->pBody->vertex.GetFirst();pPt;pPt=pOthPlate->pBody->vertex.GetNext())
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pOthPlate->pSolidBody->GetKeyPointAt(vi);
				oth_scope.VerifyVertex(vertex);
			}
			if(!scope.IsIntersWith(oth_scope))
				continue;	//粗判不可能相交
			f3dPoint collisionPt;
			if(pPlate->pSolidBody->IsIntWith(pOthPlate->pSolidBody,collisionPt))
			{
				CString sName=pPlate->GetPartTypeName();
				CString sOthName=pOthPlate->GetPartTypeName();
				CXhChar16 sPartNo1(pPlate->GetPartNo());
				if (sPartNo1.GetLength()<=0)
					sPartNo1.Printf("0X%X",pPlate->handle);
				CXhChar16 sPartNo2(pOthPlate->GetPartNo());
				if(sPartNo2.GetLength()<=0)
					sPartNo2.Printf("0X%X",pOthPlate->handle);
				COutputInfo *pInfo=pInfoList->append();
				pInfo->m_hRelaObj1=pPlate->handle;
				pInfo->m_hRelaObj2=pOthPlate->handle;
				pInfo->collisionPt=collisionPt;
#ifdef AFX_TARG_ENU_ENGLISH
				pInfo->m_sDescription.Format("%sSeg,%s%s may collide with %s%s 。",(char*)pPlate->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#else 
				pInfo->m_sDescription.Format("%s段,%s%s与%s%s间可能发生碰撞。",(char*)pPlate->iSeg.ToString(),(char*)sPartNo1,sName,(char*)sPartNo2,sOthName);
#endif
				pInfo->m_uType=INFO_CHK_COLLIDE_PLATES;
				//
				if(pContext->IsGreaterThanMaxWarnings(pInfoList->GetNodeNum()))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					DisplayProcess(100,"Check the collision of plates！");
#else 
					DisplayProcess(100,"板与板碰撞检测进度！");
#endif
					return;
				}
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Check the collision of plates！");
#else 
	DisplayProcess(100,"板与板碰撞检测进度！");
#endif
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CMainFrame
//当增加命令选项时，用户应先删除原有的cfg文件。
void CMainFrame::InitDataCenterCmdItems()
{
	CMDCFG_ITEM *pCmd;
	cmdCorrectErros.serial=1;
	cmdCorrectErros.nMaxWarnings=0;
	cmdRoutineChk.serial=2;
#ifdef __COMMON_PART_INC_
	cmdPartNoChk.serial=3;
	cmdCollisionChk.serial=4;
	//1.自动纠正数据错误
	//pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	//pCmd->group=cmdCorrectErros.serial;
	//pCmd->serial=(BYTE)cmdItems.GetNodeNum();
	//pCmd->name.Copy("修订螺栓通厚");
	//pCmd->func=ExecuteEmendBoltL0Para;
	//cmdCorrectErros.AddCmd(pCmd);
#endif
	//1.自动纠正数据错误
#ifdef __PART_DESIGN_INC_
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCorrectErros.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Update the calculate information of cross-bracing members");
#else 
	pCmd->name.Copy("更新交叉材计算信息");
#endif
	pCmd->func=ExecuteEmendCrossRodInfo;
	cmdCorrectErros.cmdItems.SetValue(pCmd->serial,true);
#endif

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCorrectErros.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Supply related symmetrical information");
#else 
	pCmd->name.Copy("补充关联对称信息");
#endif
	pCmd->func=ExecuteEmendMirRodInfo;
	cmdCorrectErros.cmdItems.SetValue(pCmd->serial,true);
	
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCorrectErros.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Revise related symmetrical information");
#else 
	pCmd->name.Copy("修正关联对称信息");
#endif
	pCmd->func=ExecuteRelativeMirInfoCheck;
	cmdCorrectErros.cmdItems.SetValue(pCmd->serial,true);
#if defined(__COMPLEX_PART_INC_) && !defined(__LDS_TSA_)
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCorrectErros.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
	pCmd->name.Copy("修正钢管端头连接信息");
	pCmd->func=ExecuteTubeEndJointLjInfoCheck;
	cmdCorrectErros.cmdItems.SetValue(pCmd->serial,true);
#endif
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCorrectErros.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Revise layer information");
#else 
	pCmd->name.Copy("修正图层名称");
#endif
	pCmd->func=ExecuteReviseLayerInfoCheck;
	cmdCorrectErros.cmdItems.SetValue(pCmd->serial,true);

	
	//2.常规校核
#ifdef __SMART_DRAW_INC_
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdRoutineChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Symmetrical rod's specification and material");
#else 
	pCmd->name.Copy("对称杆件规格材质");
#endif
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHK_MIRROD_BASICINFO;	//针对规格材质比对
	pCmd->func=ExecuteMirRodInfoCheck;
	cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
	
#ifndef __LDS_TSA_
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Symmetrical rod's basic length.");
#else 
		pCmd->name.Copy("对称杆件基准长度");
#endif
		pCmd->properties[0].bActive=true;
		pCmd->properties[0].cType='i';
		pCmd->properties[0].val.dwVal=CHK_MIRROD_LENGTHINFO;
		pCmd->properties[1].bActive=true;
		pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->properties[1].name="Permissible error(mm)";
#else 
		pCmd->properties[1].name="允许误差(mm)";
#endif
		pCmd->properties[1].val.dblVal=0.5;
		pCmd->func=ExecuteMirRodInfoCheck;	//针对基准长度比对
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
	}
#endif
#endif
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdRoutineChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Overlapping node");
#else 
	pCmd->name.Copy("重叠节点");
#endif
	pCmd->func=ExecuteEmendOverlapNode;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHKPARTNO_RODINFO;
	pCmd->properties[1].bActive=true;
	pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->properties[1].name="Permissible error(mm)";
#else 
	pCmd->properties[1].name="允许误差(mm)";
#endif
	pCmd->properties[1].val.dblVal=0.5;
	cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdRoutineChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Overlapping rod");
#else 
	pCmd->name.Copy("重叠杆件");
#endif
	pCmd->func=ExecuteEmendOverlapRod;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHKPARTNO_RODINFO;
	pCmd->properties[1].bActive=true;
	pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->properties[1].name="Permissible error(mm)";
#else 
	pCmd->properties[1].name="允许误差(mm)";
#endif
	pCmd->properties[1].val.dblVal=0.5;
	cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdRoutineChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Data integrity");
#else 
	pCmd->name.Copy("数据完整性");
#endif
	pCmd->func=ExecuteDataValidCheck;
	cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
#ifdef __COMMON_PART_INC_
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
#ifdef __SMART_DRAW_INC_
		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Symmetrical plate's material and thickness");
#else 
		pCmd->name.Copy("对称钢板材质厚度");
#endif
		pCmd->properties[0].bActive=true;
		pCmd->properties[0].cType='i';
		pCmd->properties[0].val.dwVal=CHK_PLATE_BASICINFO;
		pCmd->func=ExecuteMirPlateInfoCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);

		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("The profile of symmetric plate and holt information of bolt");
#else 
		pCmd->name.Copy("对称钢板外形及螺栓孔信息");
#endif
		pCmd->properties[0].bActive=true;
		pCmd->properties[0].cType='i';
		pCmd->properties[0].val.dwVal=CHK_PLATE_PROFILEANDBOLTINFO;
		pCmd->properties[1].bActive=true;
		pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->properties[1].name="Permissible error(mm)";
#else 
		pCmd->properties[1].name="允许误差(mm)";
#endif
		pCmd->properties[1].val.dblVal=0.5;
		pCmd->func=ExecuteMirPlateInfoCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
#endif
	
		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Bolt overlapping 、redundancy");
#else 
		pCmd->name.Copy("螺栓重叠、冗余");
#endif
		pCmd->func=ExecuteBoltOverlapCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);

		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Leak");
#else 
		pCmd->name.Copy("漏孔");
#endif
		pCmd->func=ExecuteHoleMissingCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
	}
#endif
//#ifdef __SMART_DRAW_INC_
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Revise bolt's gasket and washer");
#else 
		pCmd->name.Copy("增补修订螺栓垫片及垫圈");
#endif
		pCmd->bActive=FALSE;
		pCmd->func=ExecuteEmendBoltGasket;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,false);
	}
//#endif
#ifdef __COMMON_PART_INC_
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Revise bolt's L0");
#else 
		pCmd->name.Copy("修订螺栓通厚");
#endif
		pCmd->func=ExecuteEmendBoltL0Para;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);

		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Bolt Spec Valid");
#else 
		pCmd->name.Copy("螺栓规格合法性");
#endif
		pCmd->properties[0].bActive=true;
		pCmd->properties[0].cType='i';
		pCmd->properties[0].val.dwVal=BOLT_SIZESPEC_VERIFY;//0x08;
		pCmd->func=ExecuteBoltValidCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
		/*pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
		pCmd->name.Copy("螺栓通厚");
		pCmd->properties[0].bActive=true;
		pCmd->properties[0].cType='i';
		pCmd->properties[0].val.dwVal=BOLT_L0_VERIFY;//0x04;
		pCmd->properties[1].bActive=true;
		pCmd->properties[1].cType='d';
		pCmd->properties[1].name="允许误差(mm)";
		pCmd->properties[1].val.dblVal=0.5;
		pCmd->func=ExecuteBoltValidCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);*/

		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Bolt Edge Space");
#else 
		pCmd->name.Copy("螺栓边距");
#endif
		pCmd->properties[0].bActive=true;
		pCmd->properties[0].cType='i';
		pCmd->properties[0].val.dwVal=BOLT_EDGE_SPACE;	//0x01
		pCmd->properties[1].bActive=true;
		pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->properties[1].name="Allow Error(mm)";
#else 
		pCmd->properties[1].name="允许误差(mm)";
#endif
		pCmd->properties[1].val.dblVal=0.5;
		pCmd->func=ExecuteBoltValidCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);

		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("Bolt End Space");
#else 
		pCmd->name.Copy("螺栓端距");
#endif
		pCmd->properties[0].bActive=true;
		pCmd->properties[0].cType='i';
		pCmd->properties[0].val.dwVal=BOLT_END_SPACE;//0x02;
		pCmd->properties[1].bActive=true;
		pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->properties[1].name="Allow Error(mm)";
#else 
		pCmd->properties[1].name="允许误差(mm)";
#endif
		pCmd->properties[1].val.dblVal=0.5;
		pCmd->func=ExecuteBoltValidCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);

		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("The data legality of part entities");
#else 
		pCmd->name.Copy("构件实体数据合法性");
#endif
		pCmd->func=ExecutePartSolidDataValidCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
	}
#endif
	
#ifdef __PART_DESIGN_INC_
	if(!theApp.m_bDrawingWorkEnvOnly)
	{
		pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
		pCmd->group=cmdRoutineChk.serial;
		pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmd->name.Copy("The configure word's legality of rod's nodes");
#else 
		pCmd->name.Copy("杆件节点配材号合法性");
#endif
		pCmd->func=ExecuteTrussNodePoleCompatibleCheck;
		cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);
	}
	
	/*pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdRoutineChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
	pCmd->name.Copy("杆件螺栓连接强度");
	//pCmd->func=
	cmdRoutineChk.cmdItems.SetValue(pCmd->serial,true);*/
#endif

#ifdef __COMMON_PART_INC_
	//3.构件号校核
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdPartNoChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Check empty piece number");
#else 
	pCmd->name.Copy("空件号校核");
#endif
	pCmd->func=ExecutePartNoCheck;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHKPARTNO_NULLPARTNO;
	cmdPartNoChk.cmdItems.SetValue(pCmd->serial,true);
	
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdPartNoChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("The type of part is different.");
#else 
	pCmd->name.Copy("构件类型不同");
#endif
	pCmd->func=ExecutePartNoCheck;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHKPARTNO_PARTTYPE_DIFFER;
	cmdPartNoChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdPartNoChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Rod basic information");
#else 
	pCmd->name.Copy("杆件基本信息");
#endif
	pCmd->func=ExecutePartNoCheck;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHKPARTNO_RODINFO;
	pCmd->properties[1].bActive=true;
	pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->properties[1].name="Permissible error(mm)";
#else 
	pCmd->properties[1].name="允许误差(mm)";
#endif
	pCmd->properties[1].val.dblVal=0.5;
	cmdPartNoChk.cmdItems.SetValue(pCmd->serial,true);

	/*pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdPartNoChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
	pCmd->name.Copy("杆件螺栓信息");
	pCmd->func=ExecutePartNoCheck;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHKPARTNO_RODBOLTINFO;
	pCmd->properties[1].bActive=true;
	pCmd->properties[1].cType='d';
	pCmd->properties[1].name="允许误差(mm)";
	pCmd->properties[1].val.dblVal=0.5;
	cmdPartNoChk.cmdItems.SetValue(pCmd->serial,true);*/

	//杆件压扁切角切肢信息及螺栓信息应一起进行校核，
	//否则可能出现螺栓校核正对比通过，压扁切角切肢校核反对比通过的非法情况 wht 14-03-21
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdPartNoChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Rod flat cut-leg and bolt's information ");
#else 
	pCmd->name.Copy("杆件压扁切角切肢及螺栓信息");
#endif
	pCmd->func=ExecutePartNoCheck;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHKPARTNO_ANGLECUT|CHKPARTNO_RODBOLTINFO;
	pCmd->properties[1].bActive=true;
	pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->properties[1].name="Permissible error(mm)";
#else 
	pCmd->properties[1].name="允许误差(mm)";
#endif
	pCmd->properties[1].val.dblVal=0.5;
	cmdPartNoChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdPartNoChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Plate basic information(Thickness material)");
#else 
	pCmd->name.Copy("钢板基本信息(厚度材质)");
#endif
	pCmd->func=ExecutePartNoCheck;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHK_PLATE_BASICINFO;
	cmdPartNoChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdPartNoChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("Plate's profile and bolt's information");
#else 
	pCmd->name.Copy("钢板外形及螺栓信息");
#endif
	pCmd->func=ExecutePartNoCheck;
	pCmd->properties[0].bActive=true;
	pCmd->properties[0].cType='i';
	pCmd->properties[0].val.dwVal=CHK_PLATE_PROFILEANDBOLTINFO;
	pCmd->properties[1].bActive=true;
	pCmd->properties[1].cType='d';
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->properties[1].name="Permissible error(mm)";
#else 
	pCmd->properties[1].name="允许误差(mm)";
#endif
	pCmd->properties[1].val.dblVal=0.5;
	cmdPartNoChk.cmdItems.SetValue(pCmd->serial,true);

	//4.碰撞(干涉)校核
	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCollisionChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("rod—rod");
#else 
	pCmd->name.Copy("杆件—杆件");
#endif
	pCmd->func=ExecuteRodsCollideCheckEx;
	cmdCollisionChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCollisionChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("rod－bolt");
#else 
	pCmd->name.Copy("杆件－螺栓");
#endif
	pCmd->func=ExecuteRodBoltCollideCheck;
	cmdCollisionChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCollisionChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("plate－bolt");
#else 
	pCmd->name.Copy("钢板－螺栓");
#endif
	pCmd->func=ExecutePlateBoltCollideCheck;
	cmdCollisionChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCollisionChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("bolt－bolt");
#else 
	pCmd->name.Copy("螺栓－螺栓");
#endif
	pCmd->func=ExecuteBoltsCollideCheck;
	cmdCollisionChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCollisionChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("rod－plate");
#else 
	pCmd->name.Copy("杆件－钢板");
#endif
	pCmd->func=ExecuteRodPlateCollideCheck;
	cmdCollisionChk.cmdItems.SetValue(pCmd->serial,true);

	pCmd=cmdItems.Add(cmdItems.GetNodeNum()+1);
	pCmd->group=cmdCollisionChk.serial;
	pCmd->serial=(BYTE)cmdItems.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmd->name.Copy("plate－plate");
#else 
	pCmd->name.Copy("钢板－钢板");
#endif
	pCmd->func=ExecutePlatesCollideCheck;
	cmdCollisionChk.cmdItems.SetValue(pCmd->serial,true);
#endif
	//数据查询
	//指定规格构件清单
	//杆件长度超限校核
	//查询螺栓相关构件
	//合并开窗选择命令
}
//根据配置文件更新命令组命令顺序及激活状态
void CMainFrame::UpdateDataCenterCmdGroupPara()
{
	CMD_GROUP *cmdGroupPtrArr[4]={&cmdCorrectErros,&cmdRoutineChk,&cmdPartNoChk,&cmdCollisionChk};
	for(int i=0;i<4;i++)
	{
		CMD_GROUP *pCmdGroup=cmdGroupPtrArr[i];
		CMD_GROUP_PARA *pCmdGroupPara=&(g_sysPara.cmdGroupParaArr[i]);
		pCmdGroup->nMaxWarnings=pCmdGroupPara->nMaxWarnings;
		pCmdGroup->bOutputToNotepad=pCmdGroupPara->bOutputToNotepad;
		if(pCmdGroupPara->nCmdCount<=0)
			continue;	//当前命令组命令数为0,未保存过自定义数据
		for(int j=0;j<pCmdGroupPara->nCmdCount;j++)
		{
			int cmdSerial=pCmdGroupPara->orders[j];
			CMDCFG_ITEM *pCmdItem=cmdItems.GetValue(cmdSerial);
			if(pCmdItem==NULL || pCmdGroup->cmdItems.GetValue(pCmdItem->serial)==NULL)
				continue;
			if(pCmdItem)
			{
				pCmdItem->bActive=(pCmdGroupPara->actives[j]!=FALSE);
				pCmdGroup->cmdItems.SetValue(pCmdItem->serial,pCmdItem->bActive);
			}
		}
	}
}
void CMainFrame::OnCorrectErrors()
{
	CUserDefCmdGroupDlg dlg;
	dlg.m_pCmdGroup=&cmdCorrectErros;
	dlg.m_pCmdItems=&cmdItems;
	dlg.m_nMaxWarnings=cmdCorrectErros.nMaxWarnings;
	dlg.m_bOutputToNotepad=cmdCorrectErros.bOutputToNotepad;
	if(dlg.DoModal()!=IDOK)
		return;
	CLogErrorLife loglife;
	DATACENTERCMD_CONTEXT context;
	CTaskOutputList infoList;
	context.m_pOutputInfoList=&infoList;
	context.m_pMainWnd=this;
	context.nMaxWarnings=cmdCorrectErros.nMaxWarnings=dlg.m_nMaxWarnings;
	for(bool *pCmdState=cmdCorrectErros.cmdItems.GetFirst();pCmdState;pCmdState=cmdCorrectErros.cmdItems.GetNext())
	{
		CMDCFG_ITEM& cmdItem=cmdItems[cmdCorrectErros.cmdItems.GetCursorKey()];
		if(*pCmdState&&cmdItem.func)
		{
			context.InitByCmd(cmdItem);
			cmdItem.func((DWORD)&context);
		}
		if(context.IsGreaterThanMaxWarnings(infoList.GetNodeNum()))
			break;	//超出最大警示数，中止进行构件号检查
	}
	COutputInfoDlg *pTaskListDlg = GetOutputInfoPage();
	if(!g_sysPara.dock.pageOutput.bDisplay)
		OnDockpageOutput();
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d task",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0 task");
#else 
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d项任务",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0项任务");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	// 将结果输出到记事本
	if(dlg.m_bOutputToNotepad)
	{
		FILE *fp;
		fp = fopen("CorrectErrors.txt","wt");
		int i=1;
		for(IBasicOutput *pInfo=infoList.GetFirst();pInfo;pInfo=infoList.GetNext())
			fprintf(fp,"%4d#  %s\n",i++,pInfo->szMessage);
		fclose(fp);
		if(i>1)
			WinExec("notepad.exe CorrectErrors.txt",SW_SHOW);
	}
}
void CMainFrame::OnRoutineCheck()
{
	CUserDefCmdGroupDlg dlg;
	dlg.m_pCmdGroup=&cmdRoutineChk;
	dlg.m_pCmdItems=&cmdItems;
	dlg.m_nMaxWarnings=cmdRoutineChk.nMaxWarnings;
	dlg.m_bOutputToNotepad=cmdRoutineChk.bOutputToNotepad;
	if(dlg.DoModal()!=IDOK)
		return;
	CLogErrorLife loglife;
	DATACENTERCMD_CONTEXT context;
	CTaskOutputList infoList;
	context.m_pOutputInfoList=&infoList;
	context.m_pMainWnd=this;
	context.nMaxWarnings=cmdRoutineChk.nMaxWarnings=dlg.m_nMaxWarnings;
	GetSegNoHashTblBySegStr(dlg.m_sSegStr,context.segTable);
	for(bool *pCmdState=cmdRoutineChk.cmdItems.GetFirst();pCmdState;pCmdState=cmdRoutineChk.cmdItems.GetNext())
	{
		CMDCFG_ITEM& cmdItem=cmdItems[cmdRoutineChk.cmdItems.GetCursorKey()];
		if(*pCmdState&&cmdItem.func)
		{
			context.InitByCmd(cmdItem);
			cmdItem.func((DWORD)&context);
		}
		if(context.IsGreaterThanMaxWarnings(infoList.GetNodeNum()))
			break;	//超出最大警示数，中止进行构件号检查
	}
	COutputInfoDlg *pTaskListDlg = GetOutputInfoPage();
	if(!g_sysPara.dock.pageOutput.bDisplay)
		OnDockpageOutput();
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d task",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0 task");
#else 
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d项任务",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0项任务");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	//if(infoList.GetNodeNum()==0)	配材号检查未通时目前只是通过对话框报告未输出到任栏 wjh-2013.4.25
	//	MessageBox("通过常规项校核!");
	// 将结果输出到记事本
	if(dlg.m_bOutputToNotepad)
	{
		FILE *fp;
		fp = fopen("RoutineCheck.txt","wt");
		int i=1;
		for(IBasicOutput *pInfo=infoList.GetFirst();pInfo;pInfo=infoList.GetNext())
			fprintf(fp,"%4d#  %s\n",i++,pInfo->szMessage);
		fclose(fp);
		if(i>1)
			WinExec("notepad.exe RoutineCheck.txt",SW_SHOW);
	}
}
void CMainFrame::OnPartNoCheck()
{
#ifdef __COMMON_PART_INC_
	CUserDefCmdGroupDlg dlg;
	dlg.m_pCmdGroup=&cmdPartNoChk;
	dlg.m_pCmdItems=&cmdItems;
	dlg.m_nMaxWarnings=cmdPartNoChk.nMaxWarnings;
	dlg.m_bOutputToNotepad=cmdPartNoChk.bOutputToNotepad;
	if(dlg.DoModal()!=IDOK)
		return;
	PARTNO_CHK_CONTEXT context;
	context.nMaxWarnings=cmdPartNoChk.nMaxWarnings=dlg.m_nMaxWarnings;
	//设置构件号检查参数
	context.nSumPartNum = GetPartsHashListPartNo(context.partsHasListPartNo,dlg.m_sSegStr);
	POLYPARTLIST polylist;
	Ta.RetrievePolyPartList(polylist);
	context.nSumPartNum += GetPolyPartsHashListPartNo(context.polyPartsHasListPartNo,polylist,dlg.m_sSegStr);
	context.m_bOutputToNotepad=dlg.m_bOutputToNotepad;
	//根据指定范围进行编号校核
	CheckPartNoByScope(&context);
}
void CMainFrame::CheckPartNoByScope(void* pContext)
{
	PARTNO_CHK_CONTEXT* pPartNoContext=(PARTNO_CHK_CONTEXT*)pContext;
	CLogErrorLife loglife;
	CTaskOutputList infoList;
	pPartNoContext->m_pOutputInfoList=&infoList;
	pPartNoContext->m_pMainWnd=this;
	//pPartNoContext->nMaxWarnings=cmdPartNoChk.nMaxWarnings=dlg.m_nMaxWarnings;
	//设置构件号检查参数
	if(pPartNoContext&&pPartNoContext->sSpecifyPartNo.GetLength()>0)
		pPartNoContext->nSumPartNum = GetPartsHashListPartNo(pPartNoContext->partsHasListPartNo,"*",pPartNoContext->sSpecifyPartNo);
	//POLYPARTLIST polylist;
	//Ta.RetrievePolyPartList(polylist);
	//context.nSumPartNum += GetPolyPartsHashListPartNo(context.polyPartsHasListPartNo,polylist,dlg.m_sSegStr);
	
	for(bool *pCmdState=cmdPartNoChk.cmdItems.GetFirst();pCmdState;pCmdState=cmdPartNoChk.cmdItems.GetNext())
	{
		CMDCFG_ITEM& cmdItem=cmdItems[cmdPartNoChk.cmdItems.GetCursorKey()];
		if(*pCmdState&&cmdItem.func)
		{
			pPartNoContext->InitByCmd(cmdItem);
			cmdItem.func((DWORD)pPartNoContext);
		}
		if(pPartNoContext->IsGreaterThanMaxWarnings(infoList.GetNodeNum()))
			break;	//超出最大警示数，中止进行构件号检查
	}
	COutputInfoDlg *pTaskListDlg = GetOutputInfoPage();
	if(!g_sysPara.dock.pageOutput.bDisplay)
		OnDockpageOutput();
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d task",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0 task");
#else 
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d项任务",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0项任务");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	// 将结果输出到记事本
	if(pPartNoContext->m_bOutputToNotepad)
	{
		FILE *fp;
		fp = fopen("PartNoCheck.txt","wt");
		int i=1;
		for(IBasicOutput *pInfo=infoList.GetFirst();pInfo;pInfo=infoList.GetNext())
			fprintf(fp,"%4d#  %s\n",i++,pInfo->szMessage);
		fclose(fp);
		if(i>1)
			WinExec("notepad.exe PartNoCheck.txt",SW_SHOW);
	}
#endif
}
void CMainFrame::OnCollisionCheck()
{
	CUserDefCmdGroupDlg dlg;
	dlg.m_pCmdGroup=&cmdCollisionChk;
	dlg.m_pCmdItems=&cmdItems;
	dlg.m_nMaxWarnings=cmdCollisionChk.nMaxWarnings;
	dlg.m_bOutputToNotepad=cmdCollisionChk.bOutputToNotepad;
	//根据选中构件计算默认碰撞区域
	int i=0;
	long *id_arr,id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(id_num>0)
		dlg.m_scope.ClearScope();
	for(i=0;i<id_num;i++)
	{
		CLDSPart *pPart=Ta.FromPartHandle(id_arr[i]);
		if(pPart==NULL)
			continue;
		if(pPart->IsLinePart())
		{
			dlg.m_scope.VerifyVertex(((CLDSLinePart*)pPart)->Start());
			dlg.m_scope.VerifyVertex(((CLDSLinePart*)pPart)->End());
		}
		else
			dlg.m_scope.VerifyVertex(pPart->ucs.origin);
		if(pPart->pSolidBody!=NULL)
		{
			int vertex_n=pPart->pSolidBody->KeyPointNum();
			for(int vi=0;vi<vertex_n;vi++)
			{
				f3dPoint vertex=pPart->pSolidBody->GetKeyPointAt(vi);
				dlg.m_scope.VerifyVertex(vertex);
			}
		}
	}
	if(id_num>0)
	{	//X\Y\Z坐标范围向外扩展100并圆整
		dlg.m_scope.fMaxX=int(dlg.m_scope.fMaxX+100);
		dlg.m_scope.fMinX=int(dlg.m_scope.fMinX-100);
		dlg.m_scope.fMaxY=int(dlg.m_scope.fMaxY+100);
		dlg.m_scope.fMinY=int(dlg.m_scope.fMinY-100);
		dlg.m_scope.fMaxZ=int(dlg.m_scope.fMaxZ+100);
		dlg.m_scope.fMinZ=int(dlg.m_scope.fMinZ-100);
	}
	if(dlg.DoModal()!=IDOK)
		return;
	if(cmdCollisionChk.cmdItems.GetNodeNum()<=0)
		return;
	g_pSolidDraw->ReleaseSnapStatus();
	CLogErrorLife loglife;
	DATACENTERCMD_CONTEXT context;
	CTaskOutputList infoList;
	context.m_pOutputInfoList=&infoList;
	context.m_pMainWnd=this;
	context.nMaxWarnings=cmdCollisionChk.nMaxWarnings=dlg.m_nMaxWarnings;
	if(dlg.m_bSelectScope)
		context.m_pTag=&dlg.m_scope;	//此处指定碰撞校核范围
	// 1.实体检测时处于不显示螺栓孔的状态下（速度快一些）。
	BOOL g_bBakDisplayAllHole=g_sysPara.bDisplayAllHole;
	g_sysPara.bDisplayAllHole=FALSE;	
	int n=Ta.Parts.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"The work is preparing before solid model's collision detection!!");
#else 
	DisplayProcess(0,"正在进行实体碰撞检测前的准备工作!");
#endif
	CLDSPart *pPart;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{	//重新生成不带螺栓孔的实体
		if(pPart->m_bDisplayHole||g_sysPara.bDisplayAllHole)
		{
			bool oldholestyle=pPart->m_bDisplayHole;
			if(pPart->GetClassTypeId()!=CLS_BOLT)
				pPart->m_bDisplayHole=false;
			pPart->SetModified();
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			pPart->m_bDisplayHole=oldholestyle;
		}
		else
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"The work is preparing before solid model's collision detection!!");
#else 
		DisplayProcess((100*i)/n,"正在进行实体碰撞检测前的准备工作!");
#endif
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"The work is preparing before solid model's collision detection!");
#else 
	DisplayProcess(100,"正在进行实体碰撞检测前的准备工作!");
#endif
	// 2.执行碰撞校核
	for(bool *pCmdState=cmdCollisionChk.cmdItems.GetFirst();pCmdState;pCmdState=cmdCollisionChk.cmdItems.GetNext())
	{
		CMDCFG_ITEM& cmdItem=cmdItems[cmdCollisionChk.cmdItems.GetCursorKey()];
		if(*pCmdState&&cmdItem.func)
		{
			context.InitByCmd(cmdItem);
			cmdItem.func((DWORD)&context);
		}
		if(context.IsGreaterThanMaxWarnings(infoList.GetNodeNum()))
		{
			DisplayProcess(100);//结束进度条
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox(CXhChar50("Beyond the maximum output warnings %d in current setting!",context.nMaxWarnings));
#else 
			AfxMessageBox(CXhChar50("超出当前设定的最大输出警示项数量%d!",context.nMaxWarnings));
#endif
			break;	//超出最大警示数，中止进行构件号检查
		}
	}
	// 3.恢复空显示参数
	g_sysPara.bDisplayAllHole=g_bBakDisplayAllHole;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bDisplayHole||g_sysPara.bDisplayAllHole)
			pPart->SetModified();
	}
	// 4.更新任务列表
	COutputInfoDlg *pTaskListDlg = GetOutputInfoPage();
	if(!g_sysPara.dock.pageOutput.bDisplay)
		OnDockpageOutput();
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d task",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0 task");
#else 
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d项任务",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0项任务");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	// 5.将结果输出到记事本
	if(dlg.m_bOutputToNotepad)
	{
		FILE *fp;
		fp = fopen("CollisionCheck.txt","wt");
		int i=1;
		for(IBasicOutput *pInfo=infoList.GetFirst();pInfo;pInfo=infoList.GetNext())
			fprintf(fp,"%4d#  %s\n",i++,pInfo->szMessage);
		fclose(fp);
		if(i>1)
			WinExec("notepad.exe CollisionCheck.txt",SW_SHOW);
	}
}
//对构件进行校审
void CLDSView::OnCheckPartData()
{
#ifdef __TMA_
	CMainFrame* pMainFrm=(CMainFrame*)theApp.GetMainWnd();
	pMainFrm->ActivateDockpage(RUNTIME_CLASS(CDataCheckDlg));
	CDataCheckDlg *pDataCheckDlg=(CDataCheckDlg*)pMainFrm->GetDataCheckPage();
	if(pDataCheckDlg->GetCurSelType()!=1||xDataCheckModel.GetPartCheckNum()<=0)
		pDataCheckDlg->InitDataList();
	//
	Command("CheckPart");
#endif
}
int CLDSView::CheckModel()
{
	if(!PRODUCT_FUNC::IsHasInternalTest())
		return FALSE;
#ifdef __TMA_
	CLDSPart* pSelPart=NULL;
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n>0)
	{
		pSelPart=console.FromPartHandle(id_arr[0]);
		g_pSolidDraw->ReleaseSnapStatus();
		if(pSelPart!=NULL)
			g_pSolidDraw->SetEntSnapStatus(pSelPart->handle);
	}
	if(pSelPart==NULL || pSelPart->GetClassTypeId()==CLS_BOLT)
	{
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("CheckData Please select check part:","");
#else
		pCmdLine->FillCmdLine("CheckData 请选择要校审的构件:","");
#endif
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,SELECT_PART);
		verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
		DWORD dwhObj=0,dwExportFlag=0;
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if(dwhObj==0&&obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			{
				long* id_arr=NULL;
				if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
					dwhObj=id_arr[0];
			}
			pSelPart=console.FromPartHandle(dwhObj);
			if(pSelPart&&pSelPart->GetClassTypeId()!=CLS_BOLT&&strlen(pSelPart->GetPartNo())>0)
				break;
		}
		g_pSolidDraw->SetEntSnapStatus(pSelPart->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0X%X",pSelPart->handle));
		UpdatePropertyPage(pSelPart);
	}
	//
	CCheckPartDataDlg dlg;
	dlg.SetPart(pSelPart);
	if(dlg.DoModal()!=IDOK)
		return FALSE;
	CMainFrame* pMainFrm=(CMainFrame*)theApp.GetMainWnd();
	CDataCheckDlg *pDataCheckDlg=(CDataCheckDlg*)pMainFrm->GetDataCheckPage();
	pDataCheckDlg->MarkPartData(pSelPart->GetPartNo());
	g_pSolidDraw->ReleaseSnapStatus();
#endif
	return 1;
}
