#include "stdafx.h"
#include "LDS.h"
#include "math.h"
#include "Tower.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "GlobalFunc.h"
#include "ScrTools.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
#include "Query.h"
#include "env_def.h"
#include "LDSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if defined(__LDS_)||defined(__TSA_)
#include "PreProcessor.h"
#include "AnalyzeProcessor.h"
#include "PostProcessor.h"
#include "PostProcess.h"
#include "Resolver.h"
#include "SortFunc.h"
#include "PostProcessReport.h"

static void CalReactionForce(CLDSNode *pNode,CLDSModule *pModule,CWorkStatus *pStatus)
{	//基础节点
	BASENODE_REACTION react,*pReact=NULL;
	react.reaction_force_design=pNode->internal_force;
	if(!pNode->IsHasRestrict(X_RESTRICT))
		react.reaction_force_design.x=0.0;
	if(!pNode->IsHasRestrict(Y_RESTRICT))
		react.reaction_force_design.y=0.0;
	if(!pNode->IsHasRestrict(Z_RESTRICT))
		react.reaction_force_design.z=0.0;
	else	//考虑重力对基础节点的作用
	{
		double iceLoad=0;
		if(pStatus->ice_thick>0)
		{
			if(CPreProcessor::ActiveDesignCode->CodeSerial()!=CODES::GB50135_2006)
				iceLoad=pNode->bodyLoad.selfWeightLoad*g_sysPara.IceLoadCoefInterpByThick(pStatus->ice_thick);
			else
				iceLoad=pNode->bodyLoad.IceLoadCoef()*pStatus->ice_thick+
				pNode->bodyLoad.IceLoadCoef2()*pStatus->ice_thick*pStatus->ice_thick;
		}
		double add_weight=pNode->bodyLoad.SelfWeight()*Ta.m_fGamaGForDesign+
			iceLoad*pStatus->GamaQ_I()*pStatus->Kesai_I();
		react.reaction_force_design.z+=-Ta.SafetyCoef()*add_weight;
	}
	CXhChar50 key;
	if(g_sysPara.reactionExportType==0)	//输出全部
	{
		//个别出错情况下pModule->iNo可能不同，但都指向了同一个本体号　wjh-2016.6.6
		key.Printf("%d-%d-%s-%d-%d-%d-%d",pNode->point_i,pModule->GetBodyNo(false),(char*)pStatus->ToCaseNoString(),
			(pModule->m_arrActiveQuadLegNo[0]-1)%8+'A',(pModule->m_arrActiveQuadLegNo[1]-1)%8+'A',
			(pModule->m_arrActiveQuadLegNo[2]-1)%8+'A',(pModule->m_arrActiveQuadLegNo[3]-1)%8+'A');
		pReact=postprocessor.Reaction.GetValue(key);
	}
	else //if(g_sysPara.reactionExportType==1)	//合并高低腿
	{
		//if((bAnalysisForFoundation&&react.reaction_force_std.z>0)||(!bAnalysisForFoundation&&react.reaction_force_design.z>0))
		if(react.reaction_force_design.z>0)
			key.Printf("%d-%d-%s+",pNode->point_i,pModule->GetBodyNo(false),(char*)pStatus->ToCaseNoString());	//上拨力
		else
			key.Printf("%d-%d-%s-",pNode->point_i,pModule->GetBodyNo(false),(char*)pStatus->ToCaseNoString());	//下压力
		pReact=postprocessor.Reaction.GetValue(key);
	}
	if(pReact)
	{
		if(fabs(react.reaction_force_design.z)>fabs(pReact->reaction_force_design.z))
		{
			pReact->reaction_force_design=react.reaction_force_design;
			pReact->pModule=pModule;
			pReact->pStatus=pStatus;
			pReact->pNode=pNode;
			memcpy(pReact->leg_no,pModule->m_arrActiveQuadLegNo,8);//sizeof(WORD)*4);
		}
	}
	else if(pReact==NULL)
	{
		pReact=postprocessor.Reaction.Add(key);
		pReact->pModule=pModule;
		pReact->pStatus=pStatus;
		pReact->pNode=pNode;
		pReact->reaction_force_design=react.reaction_force_design;
		memcpy(pReact->leg_no,pModule->m_arrActiveQuadLegNo,8);//sizeof(WORD)*4);
	}
}
struct REAL_CONSTSET{
	int iNo;
	double area;	//截面积m^2
	double istrn;	//初始应变
	double Ix;		//抗弯贯性矩m^4
};
struct REAL_MAT{
	int iNo;
	double E;	//弹性模量,Pa
	double PRXY;//泊松比
};
typedef FEM_ELEMENT* FEM_ELEMPTR;
//仅用于生成分析过程文件，不考虑对称项合并因素
static int compare_femelem_func(const FEM_ELEMPTR &pElem1,const FEM_ELEMPTR &pElem2)
{
	long no_start[2],no_end[2];
	no_start[0]=min(pElem1->pStart->point_i,pElem1->pEnd->point_i);
	no_end[0]=max(pElem1->pStart->point_i,pElem1->pEnd->point_i);
	no_start[1]=min(pElem2->pStart->point_i,pElem2->pEnd->point_i);
	no_end[1]=max(pElem2->pStart->point_i,pElem2->pEnd->point_i);
	if(no_start[0]<no_start[1])
		return -1;
	else if(no_start[0]==no_start[1])
	{
		if(no_end[0]<no_end[1])
			return -1;
		else if(no_end[0]==no_end[1])
			return 0;
		else
			return 1;
	}
	else
		return 1;
}
void CreateLogFileForANSYS(char *file_name)
{
	char file_path[MAX_PATH];
	strcpy(file_path,file_name);
	strcat(file_path,".log");
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	FILE *fp=fopen(file_path,"wt");
	if(fp==NULL)
	{
		pCmdLine->PromptStrInCmdLine("ANSYS命令行文件生成失败！");
		return;
	}
	FEM_NODE *pFemNode;
	FEM_ELEMENT *pFemElem;
	//生成默认关联材质
	int i=1;
	char ss[100];
	ATOM_LIST<REAL_MAT> realMatList;
	CStringKeyHashTable<REAL_MAT*>hashMatTbl;
	hashMatTbl.CreateHashTable(10);
	//添加普通结构型钢材质
	struct REAL_MAT *pRealMat=realMatList.append();
	pRealMat->iNo=i;
	pRealMat->E=2.06E11;	//206GPa
	pRealMat->PRXY=0.3;
	sprintf(ss,"%E",pRealMat->E);
	hashMatTbl.SetValueAt(ss,pRealMat);
	
	for(pFemElem=analysis.femElemList.GetFirst();pFemElem;pFemElem=analysis.femElemList.GetNext())
	{
		if(pFemElem->elem_type==1)	//拉线柔索
		{
			sprintf(ss,"%E",pFemElem->E*1e6);
			if(!hashMatTbl.GetValueAt(ss,pRealMat))
			{	//添加当前拉线材质，由于拉线的弹性模量与拉线本身型号有关，不同于型钢，故需要单建材质
				i++;
				pRealMat=realMatList.append();
				pRealMat->iNo=i;
				pRealMat->E=pFemElem->E*1e6;	//206GPa
				pRealMat->PRXY=0.3;
				hashMatTbl.SetValueAt(ss,pRealMat);
			}
		}
	}
	fprintf(fp,"/BATCH\n");
	fprintf(fp,"/COM,ANSYS RELEASE 11.0    UP20070125       21:35:46    12/16/2009\n");
	fprintf(fp,"/PREP7\n");
	fprintf(fp,"!*\n");
	//fprintf(fp,"MPTEMP,,,,,,,,  \n");
	//fprintf(fp,"MPTEMP,1,0  \n");
	//fprintf(fp,"MPDE,EX,1   \n");
	//fprintf(fp,"MPDE,PRXY,1 \n");
	//fprintf(fp,"MPDATA,EX,1,,2.06E+011  \n");
	//fprintf(fp,"MPDATA,PRXY,1,,0.3  \n");
	for(pRealMat=realMatList.GetFirst();pRealMat;pRealMat=realMatList.GetNext())
	{
		fprintf(fp,"MP,EX,%d,%E  \n",pRealMat->iNo,pRealMat->E);
		fprintf(fp,"MP,PRXY,%d,%.1f  \n",pRealMat->iNo,pRealMat->PRXY);
	}
	fprintf(fp,"!*\n");
	fprintf(fp,"ET,1,LINK8\n");
	fprintf(fp,"ET,2,BEAM4\n");
	fprintf(fp,"!*\n");
	//TYPE 3,始端铰接终端刚接梁单元
	fprintf(fp,"ET,3,BEAM44 \n");
	fprintf(fp,"!*  \n");
	fprintf(fp,"KEYOPT,3,2,1\n");
	fprintf(fp,"KEYOPT,3,6,0\n");
	fprintf(fp,"KEYOPT,3,9,0\n");
	fprintf(fp,"KEYOPT,3,10,0   \n");
	fprintf(fp,"!*  \n");
	fprintf(fp,"KEYOPT,3,7,111  \n");
	fprintf(fp,"KEYOPT,3,8,0\n");
	fprintf(fp,"!*  \n");
	//TYPE 4,始端刚接终端铰接梁单元
	fprintf(fp,"ET,4,BEAM44 \n");
	fprintf(fp,"!*  \n");
	fprintf(fp,"KEYOPT,4,2,1\n");
	fprintf(fp,"KEYOPT,4,6,0\n");
	fprintf(fp,"KEYOPT,4,9,0\n");
	fprintf(fp,"KEYOPT,4,10,0   \n");
	fprintf(fp,"!*  \n");
	fprintf(fp,"KEYOPT,4,7,0\n");
	fprintf(fp,"KEYOPT,4,8,111  \n");
	fprintf(fp,"!*  \n");
	//TYPE 5,拉线单元
	fprintf(fp,"ET,5,LINK10 \n");
	fprintf(fp,"!*  \n");
	ATOM_LIST<REAL_CONSTSET> realConstList;
	CStringKeyHashTable<REAL_CONSTSET*>realConstHashTbl;
	struct REAL_CONSTSET *pRealConstSet=NULL;
	i=1;
	realConstHashTbl.CreateHashTable(100);
	for(pFemElem=analysis.femElemList.GetFirst();pFemElem;pFemElem=analysis.femElemList.GetNext())
	{
		double Ix=0,istrn=0;
		if(pFemElem->elem_type==1)	//拉线柔索
		{
			istrn=pFemElem->T0/(pFemElem->E*pFemElem->A);
			sprintf(ss,"%e%e",pFemElem->A,istrn);
		}
		else if(pFemElem->start_joint_type==JOINT_RIGID&&pFemElem->end_joint_type==JOINT_RIGID)
		{
			sprintf(ss,"0-%e%e",pFemElem->A,pFemElem->Iz);	//0-表示两端节点都是梁单元
			Ix=pFemElem->Iz*1e-12;
		}
		else if(pFemElem->end_joint_type==JOINT_RIGID)
		{
			sprintf(ss,"1-%e%e",pFemElem->A,pFemElem->Iz);	//1-表示始端铰接终端刚接的梁单元
			Ix=-pFemElem->Iz*1e-12;
		}
		else if(pFemElem->start_joint_type==JOINT_RIGID)
		{
			sprintf(ss,"2-%e%e",pFemElem->A,pFemElem->Iz);	//2-表示始端刚接终端铰接的梁单元
			Ix=-pFemElem->Iz*1e-12;
		}
		else
		{
			sprintf(ss,"%e",pFemElem->A);
			Ix=0;
		}
		if(!realConstHashTbl.GetValueAt(ss,pRealConstSet))
		{
			pRealConstSet=realConstList.append();
			pRealConstSet->iNo=i;
			pRealConstSet->area=pFemElem->A*1e-6;
			pRealConstSet->istrn=istrn;
			pRealConstSet->Ix=Ix;
			realConstHashTbl.SetValueAt(ss,pRealConstSet);
			i++;
		}
	}
	for(pRealConstSet=realConstList.GetFirst();pRealConstSet;pRealConstSet=realConstList.GetNext())
	{
		if(pRealConstSet->Ix==0)
			fprintf(fp,"R,%d,%e,%e,\n",pRealConstSet->iNo,pRealConstSet->area,pRealConstSet->istrn);
		else if(pRealConstSet->Ix>0)
		{
			fprintf(fp,"R,%d,%e,%e,%e,0.090,0.090,0,\n",pRealConstSet->iNo,pRealConstSet->area,pRealConstSet->Ix,pRealConstSet->Ix);
			fprintf(fp,"RMORE,0,%e, , , , ,\n",pRealConstSet->Ix);
		}
		else if(pRealConstSet->Ix<0)
		{	//BEAM44
			fprintf(fp,"*SET,_RC_SET,%d,\n",pRealConstSet->iNo);
			fprintf(fp,"R,_RC_SET,%e,%e,%e,0.090,0.090,%e,  \n",//AREA1,IZ1,IY1,TKZB1,TKYB1,IX1
				pRealConstSet->area,-pRealConstSet->Ix,-pRealConstSet->Ix,-pRealConstSet->Ix);
			fprintf(fp,"RMODIF,_RC_SET,13,0,0,0,\n");		//DX1,DY1,DZ1
			fprintf(fp,"RMODIF,_RC_SET,21,0.090,0.090,\n");	//TKZT1,TKYT1
			fprintf(fp,"RMODIF,_RC_SET,7,%e,%e,%e,0.090,0.090,%e,   \n",//AREA2,IZ2,IY2,TKZB2,TKYB2,IX2
				pRealConstSet->area,-pRealConstSet->Ix,-pRealConstSet->Ix,-pRealConstSet->Ix);
			fprintf(fp,"RMODIF,_RC_SET,16,0,0,0,  \n");		//DX2,DY2,DZ2
			fprintf(fp,"RMODIF,_RC_SET,23,0.090,0.090, \n");//TKZT2,TKYT2
			fprintf(fp,"RMODIF,_RC_SET,19,,,\n");
			fprintf(fp,"RMODIF,_RC_SET,25,,,,,,,\n");
			fprintf(fp,"RMODIF,_RC_SET,31,,,,,,,\n");
			fprintf(fp,"RMODIF,_RC_SET,37,,,,,,,\n");
			fprintf(fp,"RMODIF,_RC_SET,43,,,,,,,\n");
			fprintf(fp,"RMODIF,_RC_SET,49,,,,, , ,  \n");
			fprintf(fp,"RMODIF,_RC_SET,55, ,\n");
			fprintf(fp,"!*  \n");
		}
	}
	fprintf(fp,"!*\n");
	for(pFemNode=analysis.femNodeList.GetFirst();pFemNode;pFemNode=analysis.femNodeList.GetNext())
	{
		char strx[100],stry[100],strz[100];
		sprintf(strx,"%lf",pFemNode->pos.x/1000);
		sprintf(strz,"%lf",pFemNode->pos.y/1000);
		sprintf(stry,"%lf",-pFemNode->pos.z/1000);
		SimplifiedNumString(strx);
		SimplifiedNumString(stry);
		SimplifiedNumString(strz);
		fprintf(fp,"N,%d,%s,%s,%s,,,,\n",pFemNode->point_i,strx,stry,strz);
	}
	fprintf(fp,"!*\n");
	/*fprintf(fp,"MPTEMP,,,,,,,,\n");
	fprintf(fp,"MPTEMP,1,0\n");
	fprintf(fp,"MPDATA,EX,1,,2e11\n");
	fprintf(fp,"MPDATA,PRXY,1,,0.3\n");*/
	FEM_ELEMPTR *elem_arr=new FEM_ELEMPTR[analysis.femElemList.GetNodeNum()];
	i=0;
	for(pFemElem=analysis.femElemList.GetFirst();pFemElem;pFemElem=analysis.femElemList.GetNext())
	{
		elem_arr[i]=pFemElem;
		i++;
	}
	CQuickSort<FEM_ELEMPTR>::QuickSort(elem_arr,analysis.femElemList.GetNodeNum(),compare_femelem_func);
	//for(pFemElem=analysis.femElemList.GetFirst();pFemElem;pFemElem=analysis.femElemList.GetNext())
	for(i=0;i<analysis.femElemList.GetNodeNum();i++)
	{
		pFemElem=elem_arr[i];
		char spec[100];
		if(pFemElem->elem_type==1)	//拉线柔索
		{
			double istrn=pFemElem->T0/(pFemElem->E*pFemElem->A);
			fprintf(fp,"TYPE,5\n");	//梁单元
			sprintf(spec,"%e%e",pFemElem->A,istrn);
			sprintf(ss,"%E",pFemElem->E*1e6);
		}
		else if(pFemElem->start_joint_type==JOINT_RIGID&&pFemElem->end_joint_type==JOINT_RIGID)
		{	//始终端均刚接梁背地里单元
			fprintf(fp,"TYPE,2\n");	//梁单元
			sprintf(spec,"0-%e%e",pFemElem->A,pFemElem->Iz);
		}
		else if(pFemElem->end_joint_type==JOINT_RIGID)
		{	//始端铰接终端刚接梁单元
			fprintf(fp,"TYPE,3\n");	//梁单元
			sprintf(spec,"1-%e%e",pFemElem->A,pFemElem->Iz);
		}
		else if(pFemElem->start_joint_type==JOINT_RIGID)
		{	//始端刚接终端铰接梁单元
			fprintf(fp,"TYPE,4\n");	//梁单元
			sprintf(spec,"2-%e%e",pFemElem->A,pFemElem->Iz);
		}
		else
		{
			fprintf(fp,"TYPE,1\n");	//二力杆单元
			sprintf(spec,"%e",pFemElem->A);
		}
		if(pFemElem->elem_type==1&&hashMatTbl.GetValueAt(ss,pRealMat))
			fprintf(fp,"MAT, %d\n",pRealMat->iNo);
		else
			fprintf(fp,"MAT, 1\n");

		REAL_CONSTSET* pRealConst=NULL;
		if(realConstHashTbl.GetValueAt(spec,pRealConst))
			fprintf(fp,"REAL,%d   \n",pRealConst->iNo);
		fprintf(fp,"ESYS,0\n");		//设定当前单元坐标系
		fprintf(fp,"SECNUM, \n");	//设定当前单元截面属性号，将来应可屏蔽掉
		fprintf(fp,"TSHAP,LINE  \n");	//单元的表面外形
		fprintf(fp,"FLST,2,2,1\n");		//指定拾取方式
		fprintf(fp,"FITEM,2,%d\n",pFemElem->pStart->point_i);
		fprintf(fp,"FITEM,2,%d\n",pFemElem->pEnd->point_i);
		fprintf(fp,"E,P51X\n");			//拾取结束
		fprintf(fp,"!*\n");
	}
	delete []elem_arr;
	for(pFemNode=analysis.femNodeList.GetFirst();pFemNode;pFemNode=analysis.femNodeList.GetNext())
	{
		if(fabs(pFemNode->Fxyz.x)>0)
		{
			fprintf(fp,"FLST,2,1,1,ORDE,1\n");
			fprintf(fp,"FITEM,2,%d\n",pFemNode->point_i);
			fprintf(fp,"!*\n");
			fprintf(fp,"/GO\n");
			fprintf(fp,"F,P51X,FX,%f\n",pFemNode->Fxyz.x);
		}
		if(fabs(pFemNode->Fxyz.z)>0)
		{
			fprintf(fp,"FLST,2,1,1,ORDE,1\n");
			fprintf(fp,"FITEM,2,%d\n",pFemNode->point_i);
			fprintf(fp,"!*\n");
			fprintf(fp,"/GO\n");
			fprintf(fp,"F,P51X,FY,%f\n",-pFemNode->Fxyz.z);
		}
		if(pFemNode->Fxyz.y>0)
		{
			fprintf(fp,"FLST,2,1,1,ORDE,1\n");
			fprintf(fp,"FITEM,2,%d\n",pFemNode->point_i);
			fprintf(fp,"!*\n");
			fprintf(fp,"/GO\n");
			fprintf(fp,"F,P51X,FZ,%f\n",pFemNode->Fxyz.y);
		}
		if(pFemNode->wRestrict&X_RESTRICT)
		{
		}
	}
	for(pFemNode=analysis.femNodeList.GetFirst();pFemNode;pFemNode=analysis.femNodeList.GetNext())
	{
		if(pFemNode->wRestrict&(X_RESTRICT|Y_RESTRICT|Z_RESTRICT|X_ROT_RESTRICT|Y_ROT_RESTRICT|Z_ROT_RESTRICT))
		{
			fprintf(fp,"FLST,2,1,1,ORDE,1   \n");
			fprintf(fp,"FITEM,2,%d\n",pFemNode->point_i);
			fprintf(fp,"!*  \n");
			fprintf(fp,"/GO \n");
			char restrict_ux[3],restrict_uy[3],restrict_uz[3],restrict_rotx[5],restrict_roty[5],restrict_rotz[5];
			if(pFemNode->wRestrict&X_RESTRICT)
				strcpy(restrict_ux,"UX");
			else
				strcpy(restrict_ux," ");
			if(pFemNode->wRestrict&Z_RESTRICT)
				strcpy(restrict_uy,"UY");
			else
				strcpy(restrict_uy," ");
			if(pFemNode->wRestrict&Y_RESTRICT)
				strcpy(restrict_uz,"UZ");
			else
				strcpy(restrict_uz," ");
			if(pFemNode->wRestrict&X_ROT_RESTRICT)
				strcpy(restrict_rotx,"ROTX");
			else
				strcpy(restrict_rotx," ");
			if(pFemNode->wRestrict&Z_ROT_RESTRICT)
				strcpy(restrict_roty,"ROTY");
			else
				strcpy(restrict_roty," ");
			if(pFemNode->wRestrict&Y_ROT_RESTRICT)
				strcpy(restrict_rotz,"ROTZ");
			else
				strcpy(restrict_rotz," ");
			fprintf(fp,"D,P51X, ,0, , , ,%s,%s,%s,%s,%s,%s\n",
				restrict_ux,restrict_uy,restrict_uz,restrict_rotx,restrict_roty,restrict_rotz);
		}
	}
	fprintf(fp,"!*  \n");
	fclose(fp);
	//pCmdLine->PromptStrInCmdLine("ANSYS命令行文件生成完毕！");
}
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
extern CTimerCount timer;
#endif
void FemResolverMonitor(int process,const char* prompt)
{
	DisplayProcess(process,prompt);
}
BOOL truss3d_analysis(void* pFemResolver,CAnalyzeProcessor *pAnalysis,COMPUTE_METHOD method,bool bAnalysisForFoundation,bool bRecurResolve)
{
	int i=0,j=0,k=0;
	CLDSNode *pNode;
	int nNodeNum = analysis.nodeset.GetNodeNum();
	CLDSModule *pModule=Ta.GetActiveModule();
	CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//一、力学分析有限元计算求解
#ifdef __TIMER_COUNT_
	timer.Relay(80);
#endif
	if(method!=SUB_BLOCK_METHOD)
	{
		CFemResolver* pResolver=(CFemResolver*)pFemResolver;
		//pResolver->SetProcessMonitor(FemResolverMonitor);
		if(!bRecurResolve||pAnalysis->m_bHasCableElem||g_sysPara.AnalysisPara.m_iNonlinearStyle==1)
		{
			//拉线塔暂只支持主体线性拉线非线性计算
			bool bNonlinear=!pAnalysis->m_bHasCableElem&&g_sysPara.AnalysisPara.m_iNonlinearStyle==1;
			if(!pResolver->truss_resolve(&pAnalysis->femNodeList,&pAnalysis->femElemList,method,bNonlinear,
				g_sysPara.AnalysisPara.m_nLoadStepCounts,g_sysPara.AnalysisPara.m_nIterationCounts,
				g_sysPara.AnalysisPara.m_fConvergeLim))
			{	//为了显示非线性计算时屈曲变形，不收敛也要更新节点位移
				FEM_NODE *pFemNode=analysis.femNodeList.GetFirst();
				for(pNode=analysis.nodeset.GetFirst();pNode;pNode=analysis.nodeset.GetNext())
				{
					pNode->m_offset=pFemNode->offset;
					pNode->m_rotoffset=pFemNode->rot_offset;
					pNode->unbalance_force= pFemNode->unbalance_force;
					CNodeAnalyzeInfo nodeAnalInfo;
					//初始化键值区
					nodeAnalInfo.m_hNode=pNode->handle;
					nodeAnalInfo.m_iModuleNo=pModule->iNo;
					nodeAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
					nodeAnalInfo.m_dwStatusNo=pStatus->GetEiffelStatusNo();
					//初始化数据区
					nodeAnalInfo.SetZero();
					nodeAnalInfo.offset=pFemNode->offset;
					nodeAnalInfo.rot_offset=pFemNode->rot_offset;
					nodeAnalInfo.unbalance_force=pFemNode->unbalance_force;
					postreport.AppendToAnalyzeBuffer(nodeAnalInfo);
					pFemNode=analysis.femNodeList.GetNext();
				}
				pCmdLine->FillCmdLine("桁架体系为几何可变、瞬变体系或缺少足够的约束,无法进行应力分析","");
				return FALSE;
			}
#ifdef __TIMER_COUNT_
			timer.Relay(81);
#endif
		}
		else
		{
			pResolver->truss_equation_recur(&pAnalysis->femNodeList,&pAnalysis->femElemList);
		}
	}
	else
	{
		CBlockResolver* pBlockResolver=(CBlockResolver*)pFemResolver;
		if(!pBlockResolver->LinearResolve())
		{
			pCmdLine->FillCmdLine("桁架体系为几何可变、瞬变体系或缺少足够的约束,无法进行应力分析","");
			return FALSE;
		}
	}	
#ifdef __TIMER_COUNT_
	timer.Relay(82);
#endif
	//二、更新节点位移和节点内力并计算基础反作用力
	for(CElement *pElem=pAnalysis->hashElems.GetFirst();pElem;pElem=pAnalysis->hashElems.GetNext())
	{
		if(pElem->pOrgPole->IsAuxPole())	//辅助材，只用于统计重量用
			continue;
		pElem->N = pElem->pFemElem->N;
		pElem->bEffectN=false;	//实际结构轴力
		pElem->moment=pElem->pFemElem->moment;
		Sub_Pnt(pElem->VFS,pElem->pFemElem->start_force,pElem->LenStdVec()*(pElem->pFemElem->start_force*pElem->LenStdVec()));
		Sub_Pnt(pElem->VFE,pElem->pFemElem->end_force  ,pElem->LenStdVec()*(pElem->pFemElem->end_force*pElem->LenStdVec()));
	}
#ifdef __TIMER_COUNT_
	timer.Relay(83);
#endif
	FEM_NODE *pFemNode=pAnalysis->femNodeList.GetFirst();
	for(pNode=pAnalysis->nodeset.GetFirst();pNode;pNode=pAnalysis->nodeset.GetNext())
	{
		pNode->m_offset=pFemNode->offset;
		pNode->m_rotoffset=pFemNode->rot_offset;
		if(!bAnalysisForFoundation&&g_sysPara.bRecNodeOffset)	//基础验算时的节点位移不存储用作显示杆塔变形
			//pNode->SetNodeOffset(pModule->iNo,pStatus->iNo,pNode->m_offset);
		{
			CNodeAnalyzeInfo nodeAnalInfo;
			//初始化键值区
			nodeAnalInfo.m_hNode=pNode->handle;
			nodeAnalInfo.m_iModuleNo=pModule->iNo;
			nodeAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
			nodeAnalInfo.m_dwStatusNo=pStatus->GetEiffelStatusNo();
			//初始化数据区
			nodeAnalInfo.offset=pFemNode->offset;
			nodeAnalInfo.rot_offset=pFemNode->rot_offset;
			nodeAnalInfo.internal_force=pNode->internal_force = pFemNode->internal_force;
			nodeAnalInfo.internal_moment=pNode->internal_moment= pFemNode->internal_moment;
			nodeAnalInfo.unbalance_force=pNode->unbalance_force= pFemNode->unbalance_force;
			nodeAnalInfo.external_force=pNode->external_force = pFemNode->Fxyz;
			nodeAnalInfo.windload=pNode->WindLoad;
			postreport.AppendToAnalyzeBuffer(nodeAnalInfo);
		}
		else
		{
			pNode->internal_force = pFemNode->internal_force;
			pNode->internal_moment= pFemNode->internal_moment;
			pNode->unbalance_force= pFemNode->unbalance_force;
			pNode->external_force = pFemNode->Fxyz;
		}
		pFemNode=pAnalysis->femNodeList.GetNext();
		//计算基础反作用力
		if(pNode->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
		{
			BOOL bPush=pAnalysis->nodeset.push_stack();
			CalReactionForce(pNode,pModule,pStatus);
			if(bPush)
				pAnalysis->nodeset.pop_stack();
		}
	}
#ifdef __TIMER_COUNT_
	timer.Relay(84);
#endif
	//三、为了正确显示受力变形后的塔，需要把那些非受力节点的位移根据其父单元的端节点位移折算出来
	if(!bAnalysisForFoundation&&g_sysPara.bRecNodeOffset)	//基础验算时的节点位移不存储用作显示杆塔变形
	{	//
		/*for(pNode=pAnalysis->nodeset.GetFirst();pNode;pNode=pAnalysis->nodeset.GetNext())
			pNode->feature=0;
		for(pElem=pAnalysis->elemList.GetFirst();pElem;pElem=pAnalysis->elemList.GetNext())
		{
			if(pElem->pOrgPole->layer(1)!='F')
			{
				pElem->pStart->feature=1;
				pElem->pEnd->feature=1;
			}
		}*/
		//下面将来应使用pAnalysis->nodeset,现在pAnalysis中还未加入辅助材端节点
		for(pNode=preprocessor.result.allnodeset.GetFirst();pNode;pNode=preprocessor.result.allnodeset.GetNext())
		{
			if(pNode->feature==0&&pModule->IsSonPart(pNode))
			{
				CNodeLinkElems *pNodeLinks=preprocessor.result.hashNodeLinks.GetValue(pNode->handle);
				if(pNodeLinks==NULL)
					continue;	//孤立节点
				for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
				{
					if( pElemStiff->pElem->pOrgPole->handle==pNode->hFatherPart&&
						pElemStiff->pElem->pStart!=pNode&&pElemStiff->pElem->pEnd!=pNode)
					{
						//CPtInLineCheck lineCheck(pElemStiff->pElem->pStart->pos,pElemStiff->pElem->pEnd->pos);
						double justify=pElemStiff->pElem->CheckPoint(pNode->Position(false));
						if(justify>EPS&&justify<1-EPS)
						{	//找到非受力节点的父单元
							CNodeAnalyzeInfo nodeAnalInfo;
							//初始化键值区
							nodeAnalInfo.m_hNode=pNode->handle;
							nodeAnalInfo.m_iModuleNo=pModule->iNo;
							nodeAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
							nodeAnalInfo.m_dwStatusNo=pStatus->GetEiffelStatusNo();
							//初始化数据区
							nodeAnalInfo.m_bFemNode=false;	//辅材材端节点
							nodeAnalInfo.offset=pNode->m_offset=pElemStiff->pElem->pStart->m_offset*(1-justify)+pElemStiff->pElem->pEnd->m_offset*justify;
							postreport.AppendToAnalyzeBuffer(nodeAnalInfo);
							break;
						}
					}
				}
			}
		}
	}
#ifdef __TIMER_COUNT_
	timer.Relay(85);
#endif
	if(!bAnalysisForFoundation&&g_sysPara.bRecNodeOffset)	//基础验算时的节点位移不存储用作显示杆塔变形
	{	//根据安增军建议，anl中输出轴力应为实际结构承力，而不应是考虑规范中支撑力后的效应值 wjh-2016.12.14
		for(pElem=pAnalysis->hashElems.GetFirst();pElem;pElem=pAnalysis->hashElems.GetNext())
		{
			ELEM_ANALYZE_INFO elemAnalInfo;
			//初始化键值区
			elemAnalInfo.idElem=pElem->id;
			elemAnalInfo.m_iModuleNo=pModule->iNo;
			elemAnalInfo.legword.dwQuadLeg=pModule->ActiveQuadLegDword();
			elemAnalInfo.m_dwStatusNo=pStatus->GetEiffelStatusNo();
			//初始化数据区
			elemAnalInfo.N=pElem->N;
			elemAnalInfo.moment=pElem->moment;
			elemAnalInfo.VFS=pElem->VFS;
			elemAnalInfo.VFE=pElem->VFE;
			if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)
				elemAnalInfo.N=0;
			elemAnalInfo.iStartI=pElem->pStart->point_i;
			elemAnalInfo.iEndI=pElem->pEnd->point_i;
			elemAnalInfo.m_hParentRod=pElem->pOrgPole->handle;
			elemAnalInfo.weight=pElem->SelfWeight();
			postreport.AppendToAnalyzeBuffer(elemAnalInfo);
		}
	}
#ifdef __TIMER_COUNT_
	timer.Relay(86);
#endif
	//四、进行辅材及普通斜材对主杆件的支撑力验算
	analysis.ApplyBraceForceToElem();
#ifdef __TIMER_COUNT_
	timer.Relay(87);
#endif
	//五、计算过程完毕更新后处理单元信息
	pAnalysis->UpdateInfoAtStatusChange(pModule,pStatus);
#ifdef __TIMER_COUNT_
	timer.Relay(88);
#endif

	if(!bAnalysisForFoundation&&g_sysPara.bCreateAnsysCmdFile)	//基础验算时的节点位移不存储用作显示杆塔变形
	{
		char file_path[MAX_PATH], file_name[MAX_PATH];
		CString ss=theApp.GetLDSDoc()->GetPathName();
		sprintf(file_path,"%s",ss);
		GetSysPath(file_path,file_path,file_name);
		strcat(file_path,file_name);
		sprintf(file_name,"Step@%d\\%d_%s_Leg%d_%d_%d_%d@S%d",Ta.m_iIteration,pModule->iNo,(char*)pStatus->ToCaseNoString(),
			pModule->m_arrActiveQuadLegNo[0],pModule->m_arrActiveQuadLegNo[1],pModule->m_arrActiveQuadLegNo[2],pModule->m_arrActiveQuadLegNo[3],Ta.m_iIteration);
		strcat(file_path,"\\");
		strcat(file_path,file_name);
		CreateLogFileForANSYS(file_path);
	}
	return TRUE;
}

int compare_freqs(const double& f1,const double& f2)
{
	if(f1>f2)
		return 1;
	else if(f1<f2)
		return -1;
	else
		return 0;
}
BOOL freqs_analysis(CFemResolver* pResolver,int nFreqsRank)
{
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	int nNodeNum = analysis.nodeset.GetNodeNum();
	memset(Ta.EarthQuake.modes_arr,0,sizeof(VIBRAMODE_PROP)*Ta.EarthQuake.nFreqsRank);
	int nDimension=0;
	CHashTable<long>dofIndexTbl;
	dofIndexTbl.CreateHashTable(analysis.femNodeList.GetNodeNum());
	CLDSNode* pNode=analysis.nodeset.GetFirst();
	for(FEM_NODE *pFemNode=analysis.femNodeList.GetFirst();pFemNode;pFemNode=analysis.femNodeList.GetNext())
	{
		dofIndexTbl.SetValueAt(pNode->handle,nDimension);
		nDimension+=pFemNode->dimension;
		pNode=analysis.nodeset.GetNext();
	}
	double *eigen_vv = new double[nDimension*nFreqsRank];
	memset(eigen_vv,0,sizeof(double)*nDimension*nFreqsRank);
	if(!pResolver->truss_freqs(&analysis.femNodeList,&analysis.femElemList,FALSE,Ta.EarthQuake.modes_arr,eigen_vv,nFreqsRank,
		g_sysPara.AnalysisPara.m_nSubSpaceIterCounts,g_sysPara.AnalysisPara.m_fFreqsTolerance))
	{
		delete []eigen_vv;
		pCmdLine->FillCmdLine("桁架体系为几何可变、瞬变体系或缺少足够的约束,无法进行应力分析","");
		return FALSE;
	}
	int i=0;
	DYNAMIC_ANAL_NODE *node_arr=new DYNAMIC_ANAL_NODE[analysis.femNodeList.GetNodeNum()];
	for(pFemNode=analysis.femNodeList.GetFirst();pFemNode;pFemNode=analysis.femNodeList.GetNext(),i++)
	{
		node_arr[i].id=pFemNode->point_i;
		node_arr[i].mass=pFemNode->attachMass;
	}
	char file_path[MAX_PATH], file_name[MAX_PATH];
	theApp.GetFileName(file_path);
	strcpy(file_name,file_path);
	strcat(file_name,".frq");
	FILE *fp=fopen(file_name,"wb");
	long version=1;
	fwrite(&version,sizeof(long),1,fp);		//版本号
	fwrite(&nFreqsRank,sizeof(long),1,fp);	//计算振型的阶数
	fwrite(&nNodeNum,sizeof(long),1,fp);	//计算振型的节点数目
	//写入动力特性分析时的等效节点质量，以便将来进行抗震验算分析时调取，共(4+8)*nNodeNum个字节
	for(pNode=analysis.nodeset.GetFirst(),i=0;pNode;pNode=analysis.nodeset.GetNext(),i++)
	{
		fwrite(&pNode->handle,sizeof(long),1,fp);
		fwrite(&node_arr[i].mass,sizeof(double),1,fp);
	}
	for(i=0;i<nFreqsRank;i++)
	{
		fwrite(&Ta.EarthQuake.modes_arr[i].freqs,sizeof(double),1,fp);
		fwrite(&Ta.EarthQuake.modes_arr[i].gama_x,sizeof(double),1,fp);	//X向振型参预系数
		fwrite(&Ta.EarthQuake.modes_arr[i].gama_y,sizeof(double),1,fp);	//Y向振型参预系数
		int j=0;
		Ta.EarthQuake.modes_arr[i].x_arr=new double[analysis.nodeset.GetNodeNum()];
		Ta.EarthQuake.modes_arr[i].y_arr=new double[analysis.nodeset.GetNodeNum()];
		Ta.EarthQuake.modes_arr[i].num=analysis.nodeset.GetNodeNum();
		for(pNode=analysis.nodeset.GetFirst();pNode;pNode=analysis.nodeset.GetNext(),j++)
		{
			fwrite(&pNode->handle,sizeof(long),1,fp);
			long index=0;
			dofIndexTbl.GetValueAt(pNode->handle,index);
			fwrite(&eigen_vv[(index+0)*nFreqsRank+i],sizeof(double),1,fp);	//X
			fwrite(&eigen_vv[(index+1)*nFreqsRank+i],sizeof(double),1,fp);	//Y
			fwrite(&eigen_vv[(index+2)*nFreqsRank+i],sizeof(double),1,fp);	//Z
			Ta.EarthQuake.modes_arr[i].x_arr[j]=eigen_vv[(index+0)*nFreqsRank+i];
			Ta.EarthQuake.modes_arr[i].y_arr[j]=eigen_vv[(index+1)*nFreqsRank+i];
		}
	}
	fclose(fp);
	CreateFreqsAnalRptTxtFileByCN(file_path,node_arr,Ta.EarthQuake.modes_arr,nFreqsRank);
	delete []eigen_vv;
	delete []node_arr;
	for(i=0;i<nFreqsRank;i++)
	{
		delete []Ta.EarthQuake.modes_arr[i].x_arr;
		delete []Ta.EarthQuake.modes_arr[i].y_arr;
	}
	return TRUE;
}
#endif
