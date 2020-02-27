// MaterialTbl.cpp: implementation of the CMaterialTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tma.h"
#include "MaterialTbl.h"
#include "Query.h"
//#include "TowerTreeView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SCOPE_STRU GetRectFromPoly(fPtList &vertex_list)
{
	int nn;
	CRgn rgn;
	f3dPoint *pPoint;
	SCOPE_STRU scope;
	scope.ClearScope();
	nn = vertex_list.GetNodeNum();
	for(pPoint=vertex_list.GetFirst();pPoint!=NULL;pPoint=vertex_list.GetNext())
		scope.VerifyVertex(*pPoint);
	return scope;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
long CLsRecord::GetLsPadCounta()		//获取螺栓垫圈数量 
{
	if(typels0_pad1==1)
		return nPart;
	else
		return 0;
}
long CLsRecord::GetCommonLsCount()		//获取普通螺栓数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return 0;
	else
		return nPart-nTwoCap-nBurglarproof;	//是双帽就不是防盗螺栓
}
long CLsRecord::GetTwoCapLsCount()			//获取双帽螺栓数量
{
	if(typels0_pad1==1)
		return 0;
	else
		return nTwoCap;
}
long CLsRecord::GetCommonNailCount()		//获取普通脚钉数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return nPart-nTwoCap;
	else
		return 0;
}
long CLsRecord::GetBurglarproofLsCount()	//获取防盗螺栓数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return 0;
	else
		return nBurglarproof;
}
long CLsRecord::GetBurglarproofNailCount()	//获取防盗脚钉数量
{
	if(typels0_pad1==1)
		return 0;
	else if(bFootNail)
		return nBurglarproof;
	else
		return 0;
}
int CLsRecord::AddPart(BOOL bTwoCap/*=FALSE*/,
					   BOOL bSpringPad/*=FALSE*/,
					   BOOL bBurglarproof/*=FALSE*/,
					   int add_num/*=1*/)
{
	nPart+=add_num;
	if(bTwoCap)
		nTwoCap++;
	if(bBurglarproof)
		nBurglarproof++;
	if(bSpringPad)
		nSpringPad++;
	if(bTwoCap)
	{
		if(d==12||d==16)
			sum_weight+=FindLsWei(d,L-10,2);
		else if(d==20)
			sum_weight+=FindLsWei(d,L-15,2);
		else if(d==24)
			sum_weight+=FindLsWei(d,L-20,2);
	}
	else if(bSpringPad)
		sum_weight+=FindLsWei(d,L,1);
	else if(unit_weight>0)
		sum_weight+=unit_weight;
	else 
	{
		unit_weight=FindLsWei(d,L,0);
		sum_weight+=unit_weight;
	}
	return nPart;
}
CMaterialTbl::CMaterialTbl()
{
	m_bIncLsSegI=FALSE;
}

CMaterialTbl::~CMaterialTbl()
{

}
void CMaterialTbl::SummarizeLsList(CLsRefList *pLsRefList,int iSeg,char* limit_str)
{
	CLsRef *pLsRef;
	CString sGrade,sGuiGe;
	if(pLsRefList==NULL)
		return;
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	for(pLsRef=pLsRefList->GetFirst();pLsRef!=NULL;pLsRef=pLsRefList->GetNext())
	{
		if(pLsRef->ref_type01==0||(*pLsRef)->m_bOnlyHole)	//副本忽略掉
			continue;
		for(int iNo=FindAdaptNo(limit_str);iNo>0;iNo=FindAdaptNo(NULL))
		{
			for(CLsRecord *pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
			{
				if( m_bIncLsSegI&&pLsRec->iSeg!=iSeg)	//考虑螺栓所从属的段号
					continue;
				else if( pLsRec->typels0_pad1==0&&pLsRec->iModuleNo==iNo&&stricmp(pLsRec->grade,(*pLsRef)->grade)==0&&
					pLsRec->d==(*pLsRef)->get_d()&&pLsRec->L==(*pLsRef)->get_L())
				{
					pLsRec->AddPart((*pLsRef)->m_bTwoCapLs,(*pLsRef)->m_bHaveSpringPad,(*pLsRef)->m_bBurglarproof);
					break;
				}
			}
			if(pLsRec==NULL)
			{	//新记录
				pLsRec=ls_recordset.append();
				pLsRec->iModuleNo=iNo;
				for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				{
					if(pModule->iNo==iNo)
					{
						strcpy(pLsRec->sModule,pModule->description);
						break;
					}
				}
				if((*pLsRef)->m_bTwoCapLs)
					pLsRec->nTwoCap=1;
				if((*pLsRef)->m_bHaveSpringPad)
					pLsRec->nSpringPad=1;
				if((*pLsRef)->m_bBurglarproof)
					pLsRec->nBurglarproof=1;
				//strcpy(pLsRec->sModule,Ta.FindLegName
				pLsRec->d=(*pLsRef)->get_d();
				pLsRec->L=(long)(*pLsRef)->get_L();
				strcpy(pLsRec->grade,(*pLsRef)->grade);
				pLsRec->nPart=1;
				pLsRec->iSeg=iSeg;	//段号属性
				pLsRec->bFootNail=(*pLsRef)->m_bFootNail;
				LS_XING_HAO *pLsType=NULL;
				if(!(*pLsRef)->m_bTwoCapLs)
				{
					pLsRec->unit_weight=pLsRec->sum_weight=FindLsWei(pLsRec->d,pLsRec->L);
					pLsType=FindLsGuiGe(pLsRec->d,pLsRec->L);
				}
				else if(pLsRec->d==12||pLsRec->d==16)
				{
					pLsRec->sum_weight=FindLsWei(pLsRec->d,pLsRec->L-10);
					pLsType=FindLsGuiGe(pLsRec->d,pLsRec->L-10);
				}
				else if(pLsRec->d==20)
				{
					pLsRec->sum_weight=FindLsWei(pLsRec->d,pLsRec->L-15);
					pLsType=FindLsGuiGe(pLsRec->d,pLsRec->L-15);
				}
				else if(pLsRec->d==24)
				{
					pLsRec->sum_weight=FindLsWei(pLsRec->d,pLsRec->L-20);
					pLsType=FindLsGuiGe(pLsRec->d,pLsRec->L-20);
				}
				if(pLsType)
				{
					pLsRec->L0_downlim=pLsType->down_lim;
					pLsRec->L0_uplim=pLsType->up_lim;
				}
			}
			//垫圈和垫板的统计
			if((*pLsRef)->LsPad.quan0_ban1==0&&(*pLsRef)->LsPad.DianQuan.N>0)
			{	//螺栓上带有垫圈
				for(pLsRec=ls_recordset.GetFirst();pLsRec;pLsRec=ls_recordset.GetNext())
				{
					if(pLsRec->typels0_pad1==0)
						continue;
					else if( m_bIncLsSegI&&pLsRec->iSeg!=iSeg)	//考虑螺栓所从属的段号
						continue;
					else if(pLsRec->iModuleNo==iNo&&pLsRec->d==(*pLsRef)->get_d()&&pLsRec->L==(*pLsRef)->LsPad.DianQuan.thick)
					{
						pLsRec->AddPart(FALSE,FALSE,FALSE,(*pLsRef)->LsPad.DianQuan.N);
						break;
					}
					else
						continue;
				}
				if(pLsRec==NULL)
				{
					pLsRec=ls_recordset.append();
					pLsRec->typels0_pad1=1;
					pLsRec->iSeg=iSeg;
					pLsRec->d=(*pLsRef)->get_d();
					pLsRec->L=(*pLsRef)->LsPad.DianQuan.thick;
					pLsRec->unit_weight=FindPadWei(pLsRec->L,pLsRec->d);
					pLsRec->iModuleNo=iNo;
					pLsRec->AddPart(FALSE,FALSE,FALSE,(*pLsRef)->LsPad.DianQuan.N-1);

				}
			}
			else if((*pLsRef)->LsPad.quan0_ban1==1&&(*pLsRef)->LsPad.DianBan.bOriginal)
			{	//螺栓上带有垫板正本
				CLDSPlank *pPad=Ta.Plank.FromHandle((*pLsRef)->LsPad.DianBan.hDianBan);
				if(pPad)
					SummarizeOnePlank(pPad,iSeg,limit_str);
			}
		}
	}
}
void CMaterialTbl::SummarizeNodeAttachPart()
{
	for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		limit_str[0]='\0';
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->IsSonPart(pNode))
			{
				char ss[4];
				if(strlen(limit_str)>0)	//已有数据
					_snprintf(ss,3,",%d",pModule->iNo);
				else					//还无数据
					_snprintf(ss,3,"%d",pModule->iNo);
				strcat(limit_str,ss);
			}
		}
		for(CLDSAngle *pJg=pNode->GetFirstJg();pJg;pJg=pNode->GetNextJg())
			SummarizeOneJg(pJg,pNode->head_attach_part.belong_seg_i,limit_str);
		for(CLDSNode::ATTACH_PART *pAttach=&pNode->head_attach_part;pAttach;pAttach=pAttach->next)
		{
			CLDSPlank *pPlank;
			limit_str[0]='\0';
			for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(pModule->IsSonPart(pNode,pAttach->dwCfgPartNo))
				{
					char ss[4];
					if(strlen(limit_str)>0)	//已有数据
						_snprintf(ss,3,",%d",pModule->iNo);
					else					//还无数据
						_snprintf(ss,3,"%d",pModule->iNo);
					strcat(limit_str,ss);
				}
			}
			for(int i=0;i<3;i++)
			{
				if(pAttach->lj_handle_arr[i]>0x20)
				{
					pPlank=Ta.Plank.FromHandle(pAttach->lj_handle_arr[i]);
					if(pPlank)
						SummarizeOnePlank(pPlank,pAttach->belong_seg_i,limit_str);
				}
			}
			if(pAttach->joint_handle>0x20)
			{
				CJoint *pJoint=Ta.Joint.FromHandle(pAttach->joint_handle);
				if(pJoint)
				{
					for(pPlank=pJoint->GetFirstPlank();pPlank;pPlank=pJoint->GetNextPlank())
						SummarizeOnePlank(pPlank,pAttach->belong_seg_i,limit_str);
					for(pJg=pJoint->GetFirstJg();pJg;pJg=pJoint->GetNextJg())
						SummarizeOneJg(pJg,pAttach->belong_seg_i,limit_str);
				}
			}
			if(pAttach->foot_handle>0x20)
			{
				CFoot *pFoot=Ta.Foot.FromHandle(pAttach->foot_handle);
				if(pFoot)
				{
					for(pPlank=pFoot->GetFirstPlank();pPlank;pPlank=pFoot->GetNextPlank())
						SummarizeOnePlank(pPlank,pAttach->belong_seg_i,limit_str);
				}
			}
		}
	}
}
void CMaterialTbl::SummarizeJg()
{
	CObCommonJg *pJg;
	for(pJg=Ta.CommonJg.GetFirst();pJg!=NULL;pJg=Ta.CommonJg.GetNext())
	{
		if(pJg->m_bVirtualJg||pJg->sub_type&TA_JG_VIRTUAL)	//隐含角钢不统计
			continue;
		else if(pJg->sub_type&TA_JG_EXPLODE)	//应由其分解后的子角钢统计
			continue;
		else if(pJg->IsSubJgType(TA_JG_HUOQUSEG))
			continue;
		else if(pJg->pStart==NULL||pJg->pEnd==NULL)
			continue;	//短角钢等应在节点附属构件中进行统计
		limit_str[0]='\0';
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->IsSonPart(pJg))
			{
				char ss[4];
				if(strlen(limit_str)>0)	//已有数据
					_snprintf(ss,3,",%d",pModule->iNo);
				else					//还无数据
					_snprintf(ss,3,"%d",pModule->iNo);
				strcat(limit_str,ss);
			}
		}
		SummarizeOneJg(pJg,pJg->belong_seg_i,limit_str);
	}
	for(CObHuoQuJg *pHuoQuJg=Ta.HuoQuJg.GetFirst();pHuoQuJg;pHuoQuJg=Ta.HuoQuJg.GetNext())
	{
		CObCommonJg *pFirstJg=pHuoQuJg->seg_arr.GetFirst();
		if(pFirstJg)
		{
			limit_str[0]='\0';
			for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(pModule->IsSonPart(pFirstJg))
				{
					char ss[4];
					if(strlen(limit_str)>0)	//已有数据
						_snprintf(ss,3,",%d",pModule->iNo);
					else					//还无数据
						_snprintf(ss,3,"%d",pModule->iNo);
					strcat(limit_str,ss);
				}
			}
			if(pHuoQuJg->seg_arr.GetNodeNum()>1)
				SummarizeOneJg(pHuoQuJg,pHuoQuJg->seg_arr.GetFirst()->belong_seg_i,limit_str);
		}
	}
	for(CObGroupJg *pGroupJg=Ta.GroupJg.GetFirst();pGroupJg!=NULL;pGroupJg=Ta.GroupJg.GetNext())
	{
		limit_str[0]='\0';
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->IsSonPart(pGroupJg))
			{
				char ss[4];
				if(strlen(limit_str)>0)	//已有数据
					_snprintf(ss,3,",%d",pModule->iNo);
				else					//还无数据
					_snprintf(ss,3,"%d",pModule->iNo);
				strcat(limit_str,ss);
			}
		}
		for(CLDSPlank *pPlank=pGroupJg->GetFirstPlank();pPlank;pPlank=pGroupJg->GetNextPlank())
			SummarizeOnePlank(pPlank,pGroupJg->belong_seg_i,limit_str);
	}
}
void CMaterialTbl::SummarizeOneJg(CLDSAngle *pJg,int iSeg,char *limit_str)
{
	int i;
	CString sPureGuiGe,cMaterial;
	CObCommonJg *pCommJg;
	if(pJg->IsSubJgType(TA_JG_GROUP))
	{	//组合角钢
		CObGroupJg *pGroupJg=(CObGroupJg*)pJg;
		for(i=0;i<4;i++)
		{
			pCommJg=Ta.CommonJg.FromHandle(pGroupJg->son_jg_h[i]);
			SummarizeOneJg(pCommJg,iSeg,limit_str);
		}
		for(CLDSPlank *pPlank=pGroupJg->GetFirstPlank();pPlank;pPlank=pGroupJg->GetNextPlank())
			SummarizeOnePlank(pPlank,iSeg,limit_str);
		return;
	}
	else if(pJg->IsSubJgType(TA_JG_HUOQU))
	{
		CObHuoQuJg *pHuoQuJg=(CObHuoQuJg*)pJg;
		for(CObCommonJg *pSegJg=pHuoQuJg->seg_arr.GetFirst();pSegJg;pSegJg=pHuoQuJg->seg_arr.GetNext())
			SummarizeLsList(pSegJg->GetLsRefList(),iSeg,limit_str);
		for(CPartRecord *pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
		{
			if(stricmp(pRec->sPartNo,pHuoQuJg->sPartNo)==0)
			{
				pRec->AddPart(pHuoQuJg->m_uStatMatCoef);
				if(strlen(pRec->sNotes)==0&&strlen(pHuoQuJg->sNotes)>0)
					strcpy(pRec->sNotes,pHuoQuJg->sNotes);
				SummarizePartByModule(limit_str,pRec->id,pHuoQuJg->m_uStatMatCoef);
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=part_recordset.append();
			strcpy(pRec->sPartNo,pHuoQuJg->sPartNo);
			pRec->part_type=TA_JG;
			pRec->wide = pHuoQuJg->GetWidth();
			pRec->thick = pHuoQuJg->GetThick();
			pRec->length=pHuoQuJg->GetLength();
			sprintf(pRec->sSpec,"L%.f*%.f",pRec->wide,pRec->thick);
			pRec->cMaterial=pHuoQuJg->cMaterial;
			pRec->iSeg=pHuoQuJg->seg_arr.GetFirst()->belong_seg_i;
			strcpy(pRec->limit_str,limit_str);
			pRec->fPieceNetWeight = pRec->fPieceMinWeight = pHuoQuJg->GetLength()*
						GetJgUnitLenWei(pHuoQuJg->GetWidth(),pHuoQuJg->GetThick())/1000;
			pRec->nM16Ls=pHuoQuJg->GetLsCount(16);
			pRec->nM20Ls=pHuoQuJg->GetLsCount(20);
			pRec->nM24Ls=pHuoQuJg->GetLsCount(24);
			pRec->nMXLs=pHuoQuJg->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
			pRec->bHuoQu=TRUE;
			pRec->wing_angle=90;
			for(pCommJg=pHuoQuJg->seg_arr.GetFirst();pCommJg;pCommJg=pHuoQuJg->seg_arr.GetNext())
			{
				if(!pRec->bCutAngle&&pCommJg->IsCutAngleOrWing())
					pRec->bCutAngle=TRUE;
				if(!pRec->bCutRoot&&pCommJg->is_cut_back==1)
					pRec->bCutRoot=TRUE;
				else if(!pRec->bCutBer&&pCommJg->is_cut_back%10==2)
					pRec->bCutBer=TRUE;
				if(!pRec->bKaiHeJiao&&fabs(pCommJg->GetDecWingAngle()-90)>2)
					pRec->bKaiHeJiao=TRUE;
				if(strlen(pRec->sNotes)==0&&strlen(pCommJg->sNotes)>0)
					strcpy(pRec->sNotes,pCommJg->sNotes);
				for(CLDSPlank *pPlank=pCommJg->GetFirstPlank();pPlank;pPlank=pCommJg->GetNextPlank())
					SummarizeOnePlank(pPlank,iSeg,limit_str);
			}
			pRec->AddPart(pHuoQuJg->m_uStatMatCoef-1);	//默认就已经统计一件了
			pRec->id=part_recordset.GetNodeNum();		//以当前编号数量作为id;
			SummarizePartByModule(limit_str,pRec->id,pHuoQuJg->m_uStatMatCoef);
		}
		//	SummarizeOneJg(pCommJg,iSeg,dwCfgPartNo);
		return;
	}
	else
		pCommJg=(CObCommonJg*)pJg;

	SummarizeLsList(pCommJg->GetLsRefList(),iSeg,limit_str);
	for(CPartRecord *pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		if(stricmp(pRec->sPartNo,pCommJg->sPartNo)==0)
		{
			pRec->AddPart(pCommJg->m_uStatMatCoef);
			if(strlen(pRec->sNotes)==0&&strlen(pCommJg->sNotes)>0)
				strcpy(pRec->sNotes,pCommJg->sNotes);
			SummarizePartByModule(limit_str,pRec->id,pCommJg->m_uStatMatCoef);
			break;
		}
	}
	if(pRec==NULL)
	{
		pRec=part_recordset.append();
		strcpy(pRec->sPartNo,pCommJg->sPartNo);
		pRec->part_type=TA_JG;
		pRec->wide = pCommJg->GetWidth();
		pRec->thick = pCommJg->GetThick();
		pRec->length=pCommJg->GetLength();
		_snprintf(pRec->sSpec,MAX_CHAR_GUIGE_16,"L%.f*%.f",pRec->wide,pRec->thick);
		pRec->cMaterial=pCommJg->cMaterial;
		pRec->iSeg=pCommJg->belong_seg_i;
		strcpy(pRec->limit_str,limit_str);
		pRec->fPieceNetWeight = pRec->fPieceMinWeight = pCommJg->GetLength()*
					GetJgUnitLenWei(pCommJg->GetWidth(),pCommJg->GetThick())/1000;
		pRec->nM16Ls=pCommJg->GetLsCount(16);
		pRec->nM20Ls=pCommJg->GetLsCount(20);
		pRec->nM24Ls=pCommJg->GetLsCount(24);
		pRec->nMXLs=pCommJg->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
		pRec->bHuoQu=pCommJg->bHuoQuStart||pCommJg->bHuoQuEnd;
		pRec->bCutAngle=pCommJg->IsCutAngleOrWing();
		pRec->wing_angle=pCommJg->GetDecWingAngle();
		if(pCommJg->is_cut_back==1)
			pRec->bCutRoot=TRUE;
		else if(pCommJg->is_cut_back%10==2)
			pRec->bCutBer=TRUE;
		if(fabs(pCommJg->GetDecWingAngle()-90)>2)
			pRec->bKaiHeJiao=TRUE;
		if(strlen(pCommJg->sNotes)>0)
			strcpy(pRec->sNotes,pCommJg->sNotes);
		pRec->id=part_recordset.GetNodeNum();
		pRec->AddPart(pCommJg->m_uStatMatCoef-1);
		SummarizePartByModule(limit_str,pRec->id,pCommJg->m_uStatMatCoef);
	}
	//SummarizeLsList(pCommJg->GetLsRefList(),iSeg,dwCfgPartNo);
	for(CLDSPlank *pPlank=pCommJg->GetFirstPlank();pPlank;pPlank=pCommJg->GetNextPlank())
		SummarizeOnePlank(pPlank,iSeg,limit_str);
}
void CMaterialTbl::SummarizeOnePlank(CLDSPlank *pPlank,int iSeg,char *limit_str)
{
	if(pPlank->IsLocked())
		return;
	if(pPlank->vertex_list.GetNodeNum()>2)
	{	//有板实体
		for(CPartRecord *pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
		{
			if(stricmp(pRec->sPartNo,pPlank->sPartNo)==0)
			{
				pRec->AddPart(pPlank->m_uStatMatCoef);
				if(strlen(pRec->sNotes)==0&&strlen(pPlank->sPartNo)>0)
					strcpy(pRec->sNotes,pPlank->sNotes);
				SummarizePartByModule(limit_str,pRec->id,pPlank->m_uStatMatCoef);
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=part_recordset.append();
			pRec->iSeg=pPlank->belong_seg_i;
			strcpy(pRec->sPartNo,pPlank->sPartNo);
			pRec->part_type=TA_PLANK;
			f2dRect rect = pPlank->GetWrapRect(Ta.m_iPlankWeightStatType);
			pRec->wide = __min(rect.Width(),rect.Height());
			pRec->length = __max(rect.Width(),rect.Height());
			pRec->thick = pPlank->get_thick();
			_snprintf(pRec->sSpec,MAX_CHAR_TEMP_16,"-%d*%.0f",pPlank->get_thick(),pRec->wide);
			pRec->cMaterial=pPlank->cMaterial;
			pRec->iSeg=iSeg;
			strcpy(pRec->limit_str,limit_str);
			pRec->net_area=pPlank->CalAreaOfNetWrap();//CalAreaOfMinWrapRect();
			pRec->fPieceMinWeight = CalSteelPlankWei(pRec->wide*pRec->length*pRec->thick);
			pRec->fPieceNetWeight = CalSteelPlankWei(pRec->net_area*pPlank->get_thick());
			pRec->nM16Ls=pPlank->GetLsCount(16);
			pRec->nM20Ls=pPlank->GetLsCount(20);
			pRec->nM24Ls=pPlank->GetLsCount(24);
			pRec->nMXLs=pPlank->GetLsCount()-pRec->nM16Ls-pRec->nM20Ls-pRec->nM24Ls;
			if(pPlank->face_N>1)
				pRec->bHuoQu=TRUE;
			if(strlen(pPlank->sNotes)>0)
				strcpy(pRec->sNotes,pPlank->sNotes);
			pRec->id=part_recordset.GetNodeNum();
			pRec->AddPart(pPlank->m_uStatMatCoef-1);
			SummarizePartByModule(limit_str,pRec->id,pPlank->m_uStatMatCoef);
		}
	}
	SummarizeLsList(pPlank->GetLsRefList(),iSeg,limit_str);
	for(CLDSPlank *pSubPlank=pPlank->GetFirstPlank();pSubPlank;pSubPlank=pPlank->GetNextPlank())
		SummarizeOnePlank(pSubPlank,iSeg,limit_str);
	pPlank->Lock();
}
void CMaterialTbl::SummarizeSuppPart()	//统计附加构件
{
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		limit_str[0]='\0';
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(GetSingleWord(pModule->dwCfgPartNo)&pSuppPart->dwCfgPartNo)
			{
				char ss[4];
				if(strlen(limit_str)>0)	//已有数据
					_snprintf(ss,3,",%d",pModule->iNo);
				else					//还无数据
					_snprintf(ss,3,"%d",pModule->iNo);
				strcat(limit_str,ss);
			}
		}
		if(pSuppPart->part_type==TA_JG)	//角钢
		{
			for(CPartRecord *pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
			{
				if(stricmp(pRec->sPartNo,pSuppPart->sPartNo)==0)
				{
					pRec->AddPart(pSuppPart->m_uStatMatCoef);
					if(strlen(pRec->sNotes)==0&&strlen(pSuppPart->sNotes)>0)
						strcpy(pRec->sNotes,pSuppPart->sNotes);
					SummarizePartByModule(limit_str,pRec->id,pSuppPart->m_uStatMatCoef);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=part_recordset.append();
				strcpy(pRec->sPartNo,pSuppPart->sPartNo);
				pRec->part_type=TA_JG;
				strcpy(pRec->sSpec,pSuppPart->guige);
				pRec->cMaterial=pSuppPart->cMaterial;
				pRec->iSeg=pSuppPart->iSegNo;
				strcpy(pRec->limit_str,limit_str);
				pRec->length=pSuppPart->length;
				pRec->fPieceNetWeight = pRec->fPieceMinWeight = pSuppPart->piece_weight;
				if(strlen(pSuppPart->sNotes)>0)
					strcpy(pRec->sNotes,pSuppPart->sNotes);
				pRec->id=part_recordset.GetNodeNum();
				pRec->AddPart(pSuppPart->m_uStatMatCoef-1);
				SummarizePartByModule(limit_str,pRec->id,pSuppPart->m_uStatMatCoef);
			}
		}
		else if(pSuppPart->part_type==TA_PLANK)	//钢板
		{
			for(CPartRecord *pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
			{
				if(stricmp(pRec->sPartNo,pSuppPart->sPartNo)==0)
				{
					pRec->AddPart(pSuppPart->m_uStatMatCoef);
					if(strlen(pRec->sNotes)==0&&strlen(pSuppPart->sNotes)>0)
						strcpy(pRec->sNotes,pSuppPart->sNotes);
					SummarizePartByModule(limit_str,pRec->id,pSuppPart->m_uStatMatCoef);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=part_recordset.append();
				strcpy(pRec->sPartNo,pSuppPart->sPartNo);
				pRec->part_type=TA_PLANK;	//连接板
				strcpy(pRec->sSpec,pSuppPart->guige);
				pRec->cMaterial=pSuppPart->cMaterial;
				pRec->iSeg=pSuppPart->iSegNo;
				strcpy(pRec->limit_str,limit_str);
				pRec->length=pSuppPart->length;
				pRec->fPieceNetWeight = pRec->fPieceMinWeight = pSuppPart->piece_weight;
				if(strlen(pSuppPart->sNotes)>0)
					strcpy(pRec->sNotes,pSuppPart->sNotes);
				pRec->id=part_recordset.GetNodeNum();
				pRec->AddPart(pSuppPart->m_uStatMatCoef-1);
				SummarizePartByModule(limit_str,pRec->id,pSuppPart->m_uStatMatCoef);
			}
		}
		else	//其它构件
		{
			for(CPartRecord *pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
			{
				if(stricmp(pRec->sPartNo,pSuppPart->sPartNo)==0)
				{
					pRec->AddPart(pSuppPart->m_uStatMatCoef);
					if(strlen(pRec->sNotes)==0&&strlen(pSuppPart->sNotes)>0)
						strcpy(pRec->sNotes,pSuppPart->sNotes);
					SummarizePartByModule(limit_str,pRec->id,pSuppPart->m_uStatMatCoef);
					break;
				}
			}
			if(pRec==NULL)
			{
				pRec=part_recordset.append();
				strcpy(pRec->sPartNo,pSuppPart->sPartNo);
				pRec->part_type=4;	//其它构件
				strcpy(pRec->sSpec,pSuppPart->guige);
				pRec->cMaterial=pSuppPart->cMaterial;
				pRec->iSeg=pSuppPart->iSegNo;
				strcpy(pRec->limit_str,limit_str);
				pRec->length=pSuppPart->length;
				pRec->fPieceNetWeight = pRec->fPieceMinWeight = pSuppPart->piece_weight;
				if(strlen(pSuppPart->sNotes)>0)
					strcpy(pRec->sNotes,pSuppPart->sNotes);
				pRec->id=part_recordset.GetNodeNum();
				pRec->AddPart(pSuppPart->m_uStatMatCoef-1);
				SummarizePartByModule(limit_str,pRec->id,pSuppPart->m_uStatMatCoef);
			}
		}
	}
}
BOOL CMaterialTbl::RunSummarize(BOOL bIncLsSegI/*=FALSE*/)	//进行材料统计
{
	m_bIncLsSegI=bIncLsSegI;
	SummarizeNodeAttachPart();	//统计短角钢集合内的材料
	SummarizeJg();		//统计角钢集合内的材料
	SummarizeSuppPart();//统计附加构件的材料
	for(CModulePartSet *pModulePart=moduleset.GetFirst();pModulePart;pModulePart=moduleset.GetNext())
	{
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->iNo==pModulePart->iNo)
			{
				strcpy(pModulePart->description,pModule->description);
				break;
			}
		}
	}
	return TRUE;
}
void CMaterialTbl::SortPart(int partno0_guige1/*=0*/,BOOL bDifferMat/*=TRUE*/,BOOL bAscending/*=TRUE*/)
{
	int i=0,part_n=part_recordset.GetNodeNum();
	CPartRecord *part_arr=new CPartRecord[part_n];
	i=0;
	for(CPartRecord *pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		part_arr[i]=*pRec;
		i++;
	}
	if(partno0_guige1==0)
	{	//按构件编号进行排序
		for(i=0;i<part_n-1;i++)
		{
			BOOL bSort=TRUE;
			for(int j=0;j<part_n-i-1;j++)
			{
				if(part_arr[j].iSeg>part_arr[j+1].iSeg)
				{
					CPartRecord tm=part_arr[j];
					part_arr[j]=part_arr[j+1];
					part_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(part_arr[j].iSeg==part_arr[j+1].iSeg)
				{
					if(stricmp(part_arr[j].sPartNo,part_arr[j+1].sPartNo)>0)
					{
						CPartRecord tm=part_arr[j];
						part_arr[j]=part_arr[j+1];
						part_arr[j+1]=tm;
						bSort=FALSE;
					}
				}
				else
					continue;
			}
			if(bSort)	//已有序
				break;
		}
	}
	else if(partno0_guige1=1)
	{	//按规格进行排序(可以分为是否区分材质两种)
		for(i=0;i<part_n-1;i++)
		{
			BOOL bSort=TRUE;
			for(int j=0;j<part_n-i-1;j++)
			{
				if(part_arr[j].iSeg>part_arr[j+1].iSeg)
				{
					CPartRecord tm=part_arr[j];
					part_arr[j]=part_arr[j+1];
					part_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(part_arr[j].iSeg==part_arr[j+1].iSeg)
				{
					if(part_arr[j].part_type>part_arr[j+1].part_type)
					{	//区分塔材类型
						CPartRecord tm=part_arr[j];
						part_arr[j]=part_arr[j+1];
						part_arr[j+1]=tm;
						bSort=FALSE;
					}
					else if(part_arr[j].part_type==part_arr[j+1].part_type)
					{
						if(bDifferMat&&part_arr[j].cMaterial<part_arr[j+1].cMaterial)
						{	//区分材质
							CPartRecord tm=part_arr[j];
							part_arr[j]=part_arr[j+1];
							part_arr[j+1]=tm;
							bSort=FALSE;
						}
						else if(!bDifferMat||part_arr[j].cMaterial==part_arr[j+1].cMaterial)
						{	//材质相同或不区分材质
							char ch;
							double guige_para1=0,guige_para2=0,guige_para3=0,guige_para4=0;
							sscanf(part_arr[j].sSpec,"%c%lf%c%lf",&ch,&guige_para1,&ch,&guige_para2);
							sscanf(part_arr[j+1].sSpec,"%c%lf%c%lf",&ch,&guige_para3,&ch,&guige_para4);
							if(guige_para1>guige_para3)
							{
								CPartRecord tm=part_arr[j];
								part_arr[j]=part_arr[j+1];
								part_arr[j+1]=tm;
								bSort=FALSE;
							}
							else if(guige_para1==guige_para3&&guige_para2>guige_para4)
							{
								CPartRecord tm=part_arr[j];
								part_arr[j]=part_arr[j+1];
								part_arr[j+1]=tm;
								bSort=FALSE;
							}
							else if(guige_para1==guige_para3&&guige_para2==guige_para4&&part_arr[j].length>part_arr[j+1].length)
							{
								CPartRecord tm=part_arr[j];
								part_arr[j]=part_arr[j+1];
								part_arr[j+1]=tm;
								bSort=FALSE;
							}
							else if(guige_para1==guige_para3&&guige_para2==guige_para4&&fabs(part_arr[j].length-part_arr[j+1].length)<0.1)
							{
								if(stricmp(part_arr[j].sPartNo,part_arr[j+1].sPartNo)>0)
								{
									CPartRecord tm=part_arr[j];
									part_arr[j]=part_arr[j+1];
									part_arr[j+1]=tm;
									bSort=FALSE;
								}
							}
						}
						else
							continue;
					}
				}
			}
			if(bSort)	//已有序
				break;
		}
	}
	i=0;
	for(pRec=part_recordset.GetFirst();pRec;pRec=part_recordset.GetNext())
	{
		*pRec=part_arr[i];
		i++;
	}
	delete []part_arr;
}
void CMaterialTbl::SortLs()
{
	int i=0,ls_n=ls_recordset.GetNodeNum();
	CLsRecord *ls_arr=new CLsRecord[ls_n];
	i=0;
	for(CLsRecord *pRec=ls_recordset.GetFirst();pRec;pRec=ls_recordset.GetNext())
	{
		ls_arr[i]=*pRec;
		i++;
	}
	//按规格进行排序(可以分为是否区分材质两种)
	for(i=0;i<ls_n-1;i++)
	{
		BOOL bSort=TRUE;
		for(int j=0;j<ls_n-i-1;j++)
		{
			if(ls_arr[j].iModuleNo>ls_arr[j+1].iModuleNo)
			{
				CLsRecord tm=ls_arr[j];
				ls_arr[j]=ls_arr[j+1];
				ls_arr[j+1]=tm;
				bSort=FALSE;
			}
			else if(ls_arr[j].iModuleNo==ls_arr[j+1].iModuleNo)
			{
				if(m_bIncLsSegI&&ls_arr[j].iSeg>ls_arr[j+1].iSeg)
				{
					CLsRecord tm=ls_arr[j];
					ls_arr[j]=ls_arr[j+1];
					ls_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(m_bIncLsSegI&&ls_arr[j].iSeg<ls_arr[j+1].iSeg)
					continue;
				else if(stricmp(ls_arr[j].grade,ls_arr[j+1].grade)>0)
				{	//区分塔材类型
					CLsRecord tm=ls_arr[j];
					ls_arr[j]=ls_arr[j+1];
					ls_arr[j+1]=tm;
					bSort=FALSE;
				}
				else if(stricmp(ls_arr[j].grade,ls_arr[j+1].grade)==0)
				{
					if(ls_arr[j].d>ls_arr[j+1].d)
					{	//区分直径
						CLsRecord tm=ls_arr[j];
						ls_arr[j]=ls_arr[j+1];
						ls_arr[j+1]=tm;
						bSort=FALSE;
					}
					else if(ls_arr[j].d==ls_arr[j+1].d)
					{	//直径相同区分有效长度
						if(ls_arr[j].L>ls_arr[j+1].L)
						{
							CLsRecord tm=ls_arr[j];
							ls_arr[j]=ls_arr[j+1];
							ls_arr[j+1]=tm;
							bSort=FALSE;
						}
					}
				}
			}
		}
		if(bSort)	//已有序
			break;
	}

	i=0;
	for(pRec=ls_recordset.GetFirst();pRec;pRec=ls_recordset.GetNext())
	{
		*pRec=ls_arr[i];
		i++;
	}
	delete []ls_arr;
}

void CMaterialTbl::SummarizePartByModule(char* limit_str, long id, UINT uStatCoef)
{
	for(int iNo=FindAdaptNo(limit_str);iNo>0;iNo=FindAdaptNo(NULL))
	{
		for(CModulePartSet *pModule=moduleset.GetFirst();pModule;pModule=moduleset.GetNext())
		{
			if(pModule->iNo==iNo)
			{
				for(BRIEF_PART_RECORD *pBriefRec=pModule->GetPartList()->GetFirst();pBriefRec;pBriefRec=pModule->GetPartList()->GetNext())
				{
					if(pBriefRec->id==id)
					{
						pBriefRec->num+=uStatCoef;
						break;
					}
				}
				if(pBriefRec==NULL)
				{
					pBriefRec=pModule->GetPartList()->append();
					pBriefRec->id=id;
					pBriefRec->num=uStatCoef;
				}
				break;
			}
		}
		if(pModule==NULL)
		{
			pModule=moduleset.append();
			pModule->iNo=iNo;
			BRIEF_PART_RECORD *pBriefRec=pModule->GetPartList()->append();
			pBriefRec->id=id;
			pBriefRec->num=uStatCoef;
		}
	}
}
