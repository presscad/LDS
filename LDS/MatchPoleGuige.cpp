#include "stdafx.h"
#include "LDS.h"
#include "stdio.h"
#include "math.h"
#include "Tower.h"
#include "database.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "FLSizeOptimizor.h"
#include "PartSizeOptimizor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__TSA_)
#include "PreProcessor.h"

static void CalElemHoleOutNum(CElement *pElem)
{	//计算当前单元的螺栓减孔数
	char sError[100]="";
	JGZJ jgzj;
	double lsn_coef;		//螺栓减孔数
	int ls_n,ls_d;
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE
		||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		//用单元最新测试规格易导致振荡不收敛，而且与实际统材不符，故此处必须用选材前杆件规格
		double wing_wide=pElem->pOrgPole->GetWidth();	//pElem->GetWidth();
		double wing_thick=pElem->pOrgPole->GetThick();	//pElem->GetThick();
		getjgzj(jgzj,wing_wide);//获得角钢准距表
		
		char cType='L';	//普通单角钢
		if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			cType=pElem->pOrgPole->size_cSubClassType;
		JG_PARA_TYPE *pJgType=FindJgType(wing_wide,wing_thick,cType);
		if(pJgType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X的角钢规格%s%.0f*%.0f，螺栓减孔数计算失败！",pElem->pOrgPole->handle,pElem->pOrgPole->GetBriefPartTypeMark(),wing_wide,wing_thick);
			throw sError;
		}
		//JG_PARA_TYPE *pJgType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
		}
		else
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=max(pElem->pOrgPole->connectStart.wnConnBoltN,pElem->pOrgPole->connectEnd.wnConnBoltN);
		}

		//以下代码用于计算螺栓产生的截面减少效应
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,ls_d);
		if(!pElem->pOrgPole->m_bAutoCalHoleOutCoef)	//已由用户指定杆件截面减孔数
			lsn_coef=pElem->pOrgPole->hole_out;
		else	//需要根据螺栓数自动计算杆件截面的减孔数
		{	
			if(pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
			{	//两侧均为单肢连接(仍要考虑另一肢可能有补材螺栓的情形)
				int nMaxLsRows;
				//自动判断杆件上的螺栓排数
				if(ls_n<5)
					nMaxLsRows=1;
				else
					nMaxLsRows=2;
				
				if(nMaxLsRows==1||jgzj.g2-jgzj.g1==0)
				{
					if(toupper(pElem->pOrgPole->layer(1))=='H')
					{	//螺栓单排布置，对于横材仍要考虑另一肢同有单补材螺栓的情形
						double lsn_coef1=1;
						double g=jgzj.g*2-wing_thick;	//对应规定:g=2d1-t
						double lsn_coef2=2-pow(LsSpace.SingleRowSpace/2.0,2)/(4*g*ls_d);
						lsn_coef=max(lsn_coef1,lsn_coef2);
					}
					else
						lsn_coef=1;
				}
				else// if(nMaxLsRows==2)
				{	//螺栓双排布置时
					double lsn_coef1=2;
					double lsn_coef2=3-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*ls_d);
					lsn_coef=max(lsn_coef1,lsn_coef2);
				}
			}
			else
			{	//两侧至少有一侧为双肢连接
				int nMaxLsRows;
				//自动判断杆件上的螺栓排数
				if(ls_n<5)
					nMaxLsRows=1;
				else
					nMaxLsRows=2;

				if(nMaxLsRows==1||jgzj.g2-jgzj.g1==0)
				{	//螺栓单排布置
					double lsn_coef1=1;	//仍要考虑两肢同有螺栓的情形
					double g=jgzj.g*2-wing_thick;	//对应规定:g=2d1-t
					double lsn_coef2=2-pow(LsSpace.SingleRowSpace/2.0,2)/(4*g*ls_d);
					lsn_coef=max(lsn_coef1,lsn_coef2);
				}
				else //if(nMaxLsRows==2)
				{	//螺栓双排布置时
					double lsn_coef1=2;
					double lsn_coef2=3-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*ls_d);
					double g=jgzj.g1*2-wing_thick;	//对应规定:g=2d1-t
					double lsn_coef3=4-pow(LsSpace.doubleRowSpace/2.0,2)*
						(2.0/(jgzj.g2-jgzj.g1)+1/g)/(4*ls_d);
					lsn_coef=max(lsn_coef1,lsn_coef2);
					lsn_coef=max(lsn_coef,lsn_coef3);
				}
			}
			if(pElem->pOrgPole->layer(1)=='Z')
				lsn_coef=max(lsn_coef,2);	//认为主材上两肢连接有可能出现对孔现象
		}
		if(pElem->pOrgPole->m_bAutoCalHoleOutCoef)
			pElem->pOrgPole->hole_out=lsn_coef;
		if(toupper(pJgType->cType)=='T'||toupper(pJgType->cType)=='D')
			lsn_coef*=2;
		else if(toupper(pJgType->cType)=='X')
			lsn_coef*=4;
		pElem->design_info.hole_out=lsn_coef;
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		double wing_wide=pElem->GetWidth();
		double wing_thick=pElem->GetThick();
		getjgzj(jgzj,wing_wide);
		
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
		}
		else
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=max(pElem->pOrgPole->connectStart.wnConnBoltN,pElem->pOrgPole->connectEnd.wnConnBoltN);
		}

		//以下代码用于计算螺栓产生的截面减少效应
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,ls_d);
		if(pElem->pOrgPole->hole_out>0)	//已由用户指定杆件截面减孔数
			lsn_coef=pElem->pOrgPole->hole_out;
		else	//需要根据螺栓数自动计算杆件截面的减孔数
		{	
			//两侧均为单肢连接(仍要考虑另一肢可能有补材螺栓的情形)
			int nMaxLsRows;
			//自动判断杆件上的螺栓排数
			if(ls_n<5)
				nMaxLsRows=1;
			else
				nMaxLsRows=2;
			
			if(nMaxLsRows==1||jgzj.g2-jgzj.g1==0)	//螺栓单排布置
				lsn_coef=1;
			else// if(nMaxLsRows==2)				//螺栓双排布置时
				lsn_coef=2-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*ls_d);
		}
		pElem->pOrgPole->hole_out=pElem->design_info.hole_out=lsn_coef;
	}
	else
		pElem->design_info.hole_out=0;
}
#include "Codes.h"
int MemberCheck2(CElement *pElem,IMemberDesignCode* pDesignCode,BOOL bCheckAll,bool bEqCheck=false,BOOL bUpdateBoltInfo=false)
{
	double new_wing_wide,new_wing_thick;
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
	if(pMatType==NULL)
		return 2;
	char sError[100]="";
	//更新计算各单元的最大轴力
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||
		pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pJgParaType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgParaType)
		{
			new_wing_wide = pJgParaType->wing_wide;
			new_wing_thick= pJgParaType->wing_thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,'L');
			else
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,pElem->pOrgPole->size_cSubClassType);
		}
		if(bUpdateBoltInfo)	//更新角钢端头连接螺栓信息
			CalElemRelaBoltInfo(pElem);
		CalElemHoleOutNum(pElem);	//计算当前受力单元的减孔数
		//必须按单元当前选中规格重新计算每个单元的压稳信息，以便于真实计算各工况的压稳状态
		CPreProcessor::CalElemCompStableInfo(pElem,pDesignCode);		//计算当前受力单元长细比
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pTubeParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeParaType)
		{
			new_wing_wide = pTubeParaType->D;
			new_wing_thick= pTubeParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindTubeType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//计算当前受力单元的减孔数
		//必须按单元当前选中规格重新计算每个单元的压稳信息，以便于真实计算各工况的压稳状态
		CPreProcessor::CalElemCompStableInfo(pElem,pDesignCode);		//计算当前受力单元长细比
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		BIAN_PARA_TYPE *pFlatParaType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pFlatParaType)
		{
			new_wing_wide = pFlatParaType->wide;
			new_wing_thick= pFlatParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindBianType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//计算当前受力单元的减孔数
		//CPreProcessor::CalElemCompStableInfo(pElem,pDesignCode);		//计算当前受力单元长细比
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_CABLE)
	{
		pElem->design_info.fCoefTensStrengthDecrease=1.0;
		pElem->design_info.iDangerousRule= 2;
		if(pElem->MaxTension.N>pElem->pOrgPole->GetWireForcePermit())	//强度控制
			pElem->m_iCtrlRule= 2;
		else
			pElem->m_iCtrlRule=0;
		pElem->design_info.fCoefOfBearLoad=pElem->design_info.fCoefEffic=pElem->MaxTension.N/pElem->pOrgPole->GetWireForcePermit();
		return pElem->m_iCtrlRule;
	}
	//初次确定杆件规格型号
	double L=pElem->Length();
	pElem->m_iCtrlRule=0;
	pElem->design_info.iDangerousRule = pElem->design_info.iDangerousLoadCase = 0;
	pElem->design_info.fCoefEffic=0;
	pElem->design_info.fCoefOfBearLoad = 0;
	double fy=pMatType->fy(new_wing_thick);
	double fv=pMatType->fv(new_wing_thick);
	double ft=pMatType->f(new_wing_thick);	//设计强度
	ft=pDesignCode->GetSteelDesignStrenth(fy,ft);
		
	double coefEqGamaRE=1.0;
	if(bEqCheck)
		coefEqGamaRE=Ta.EarthQuake.gamaRE;	//考虑抗震验算时结构构件抗力调整系数影响
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE
		||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		double An;	//净截面积
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		char cType='L';
		if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			cType=pElem->pOrgPole->size_cSubClassType;
		JG_PARA_TYPE *pJgType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X的角钢规格%s%.0f*%.0f，规格验算失败！",pElem->pOrgPole->handle,pElem->pOrgPole->GetBriefPartTypeMark(),wing_wide,wing_thick);
			throw sError;
		}
		double maxValidTensionN=pElem->MaxTension.N;	//受螺栓强度限制的最大有效拉力，防止因螺栓数导致错误报告块剪超限错误 wjh-2017.7.7
		//即使不需选材的杆件也需重新计算一下螺栓
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;	//承剪承载力
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;	//承压承载力
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			if(pLsType==NULL)
			{
				sprintf(sError,"螺栓库中未找到当前级别{%s}的螺栓类型",pElem->pOrgPole->connectStart.grade);
				throw sError;
			}
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectStart.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				maxValidTensionN=ls_n*min(Nbv,Nbc);
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			if(pLsType==NULL)
			{
				sprintf(sError,"螺栓库中未找到当前级别{%s}的螺栓类型",pElem->pOrgPole->connectStart.grade);
				throw sError;
			}
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectEnd.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				maxValidTensionN=ls_n*min(Nbv,Nbc);
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}
		else
			ls_d=SelBoltDByWingWide(wing_wide,NULL);

		An=pJgType->area*100-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//加1.5是用于考虑螺栓孔径要比螺栓直径大1.5mm
		pElem->design_info.hole_d=ls_d+1.5;
		//辅助材由于受拉比受压时强度折减系数小,故还需要考虑同样工况受同等大小拉力的验算 wjh-2011.9.22
		if(pElem->pOrgPole->IsAuxPole())
		{
			pElem->MaxTension=pElem->MaxCompression;
			pElem->MaxTension.N=fabs(pElem->MaxTension.N);	//同工况下验算同等大小拉力情况
		}
		if(pElem->MaxTension.N>0||pElem->MaxCompression.N>CElement::COMPRESSION_ZERO_FORCE)	//受拉或无受压工况
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			double m;	//构件强度折减系数
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
			{
				if(pDesignCode->CodeSerial()==CODES::ASCE10_97)
				{
					if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
						m=0.9;
					else
						m=1;
				}
				else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
					m=0.85;	//钢管类构件
				else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_JG)
				{	//角钢类构件
					if(wing_wide>40)
						m=0.70;
					else
						m=0.55;
				}
			}
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef = pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
			pElem->design_info.bBlockShearActivated=false;
			if(pDesignCode->IsSupportBlockShearCheck()&&bearcoef<Ta.m_fLimitBearCoef)
			{	//满足常规杆件强度条件时，还需进行块剪验算
				CConnectInfo connect;
				connect.wnConnBoltN=0;
				if(pElem->pStart==pElem->pOrgPole->pStart)
					connect=pElem->pOrgPole->connectStart;
				else if(pElem->pEnd==pElem->pOrgPole->pEnd)
					connect=pElem->pOrgPole->connectEnd;
				if(connect.wnConnBoltN>0&&connect.rows==1)
				{
					JGZJ jgzj;
					getjgzj(jgzj,wing_wide);//获得角钢准距表
					double c=wing_wide-jgzj.g-(connect.d*0.5+1);	//connect.d*0.5+1=孔半径+孔径间隙
					LSSPACE_STRU LsSpace;
					GetLsSpace(LsSpace,connect.d);
					double midSpace=LsSpace.SingleRowSpace*(connect.wnConnBoltN-1);
					//受螺栓最大强度限制,块剪验算时需要输入考虑螺栓承载力后的最大有效拉力 wjh-2017.7.7
					double coef=pDesignCode->BlockShearCheck(maxValidTensionN,LsSpace.EndSpace,midSpace,c,wing_thick,ft,fv);
					if(coef>Ta.m_fLimitBearCoef&&coef>bearcoef)
					{
						pElem->design_info.bBlockShearActivated=true;
						bearcoef=coef;
					}
				}
			}
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//受拉强度控制
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//受压杆
		{
			double m;	//构件强度折减系数
			if(pDesignCode->CodeSerial()==CODES::ASCE10_97)
				m=1.0;
			else if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//受压强度控制
			}
			
			double t=wing_thick;
			double b=pJgType->wing_wide-pJgType->r-t;
			//压杆稳定强度折减系数
			double mN = pDesignCode->CompReduceCoef(b,t,ft,fy,pElem->CompStableLamda(),false,false);
			double fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(),pMatType,new_wing_thick);
			//选材条件3：轴心受压稳定计算
			bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pJgType->area*100*mN*ft);
			if(bearcoef<pElem->design_info.fCoefOfBearLoad)
			{	//非压稳控制时应根据当前最危险的承载指标出现时的同时受压状态重算mN及压稳系数fai wjh-2017.2.4
				bool bothComp=(pElem->design_info.iDangerousLoadCase==3)?pElem->MaxCompression.bothComp:false;
				if(bothComp!=pElem->MaxCompStable.bothComp)	//当前受压控制
					fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(&bothComp),pMatType,new_wing_thick);
			}
			pElem->design_info.mN =mN;
			pElem->design_info.w_t=b/t;
			pElem->design_info.fCoefStablity=fai;
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 4;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 4;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)
			{
				pElem->m_iCtrlRule=4;
				if(!bCheckAll)
					return 4;	//受压稳定控制
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		double D=new_wing_wide;
		double thick=new_wing_thick;
		TUBE_PARA_TYPE *pTubeType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X的钢管规格，规格验算失败！",pElem->pOrgPole->handle);
			throw sError;
		}

		if(pElem->MaxTension.N>0)
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			double m;	//构件强度折减系数
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;	//钢管类构件
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef = pElem->MaxTension.N*coefEqGamaRE/((pTubeType->area*100)*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//受拉强度控制
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//受压杆
		{
			double m;	//构件强度折减系数
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/((pTubeType->area*100)*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//受压强度控制
			}
			
			//选材条件3：轴心受压稳定计算
			double fai,D0,t;	//外径与壁厚
			D0=pElem->GetWidth();
			t =pElem->GetThick();
			double mN = pDesignCode->CompReduceCoef(D0,t,ft,fy,pElem->CompStableLamda(),true,false);//压杆稳定强度折减系数
			if(pTubeType->D<g_sysPara.fTubeWeldRoadMinD)	//无焊道的无缝钢管
				fai=pDesignCode->CompStableCoef('A',pElem->CompStableLamda(),pMatType,new_wing_thick);
			else
				fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(),pMatType,new_wing_thick);
			bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pTubeType->area*100*mN*ft);	//
			if(bearcoef<pElem->design_info.fCoefOfBearLoad)
			{	//非压稳控制时应根据当前最危险的承载指标出现时的同时受压状态重算mN及压稳系数fai wjh-2017.2.4
				bool bothComp=(pElem->design_info.iDangerousLoadCase==3)?pElem->MaxCompression.bothComp:false;
				if(bothComp!=pElem->MaxCompStable.bothComp)	//当前受压控制
				{
					if(pTubeType->D<g_sysPara.fTubeWeldRoadMinD)	//无焊道的无缝钢管
						fai=pDesignCode->CompStableCoef('A',pElem->CompStableLamda(&bothComp),pMatType,new_wing_thick);
					else
						fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(&bothComp),pMatType,new_wing_thick);
				}
			}
			pElem->design_info.mN =mN;
			pElem->design_info.w_t=D0/t;
			pElem->design_info.fCoefStablity=fai;
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 4;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 4;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)
			{
				pElem->m_iCtrlRule=4;
				if(!bCheckAll)
					return 4;	//受压稳定控制
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		double An;	//净截面积
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		BIAN_PARA_TYPE *pBianType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pBianType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X的扁铁规格，规格验算失败！",pElem->pOrgPole->handle);
			throw sError;
		}
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}

		An=pBianType->wide*pBianType->thick-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//加1.5是用于考虑螺栓孔径要比螺栓直径大1.5mm
		
		if(pElem->MaxTension.N>0)
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			double m=1.0;	//构件强度折减系数
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//受拉强度控制
			}
		}
	}
	else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
	{
		double area = pElem->pOrgPole->userdef.area;
		//选材条件1：杆件长细比
		if(pElem->MaxTension.N>0)
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			double m=1.0;	//构件强度折减系数
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//受拉强度控制
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//受压杆
		{
			double m=1.0;	//构件强度折减系数
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef=fabs(pElem->MaxCompression.N)*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.fc);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//受压强度控制
			}
		}
	}
	//更新单元长细比计算过程及结果输出信息
	pElem->UpdateLamdaInfo();
	//核对长细比是否超出最大允许长细比限制
	double bearcoef =pElem->design_info.Lo_r/pElem->design_info.UltimateLo_r;
	if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
	{
		pElem->design_info.fCoefEffic = bearcoef;
		pElem->design_info.iDangerousRule = 1;
	}
	if(bearcoef>1.0)
	{
		pElem->m_iCtrlRule=1;
		if(!bCheckAll)
			return 1;	//长细比控制
	}

	return pElem->m_iCtrlRule;
}
//from database.cpp
//中国规范
double CalStableCoef1(double lamda,char sectionType,char cMaterial);
//美国导则
double CalStableCoef2(double lamda,double E,double fy);
//单元检查。iDesignCode=0:国内电力规范 =1:国内高耸规范 =2:美国ASCE规范
int MemberCheck(CElement *pElem,BOOL bCheckAll,int iDesignCode,bool bEqCheck=false,bool bUpdateBoltInfo=false)
{
	return MemberCheck2(pElem,CPreProcessor::ActiveDesignCode,bCheckAll,bEqCheck,bUpdateBoltInfo);
}
/*{
	double new_wing_wide,new_wing_thick;
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
	if(pMatType==NULL)
		return 2;
	char sError[100]="";
	//更新计算各单元的最大轴力
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||
		pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pJgParaType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgParaType)
		{
			new_wing_wide = pJgParaType->wing_wide;
			new_wing_thick= pJgParaType->wing_thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,'L');
			else
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,pElem->pOrgPole->size_cSubClassType);
		}
		CalElemHoleOutNum(pElem);	//计算当前受力单元的减孔数
		//必须按单元当前选中规格重新计算每个单元的压稳信息，以便于真实计算各工况的压稳状态
		CPreProcessor::CalElemCompStableInfo(pElem,iDesignCode);		//计算当前受力单元长细比
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pTubeParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeParaType)
		{
			new_wing_wide = pTubeParaType->D;
			new_wing_thick= pTubeParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindTubeType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//计算当前受力单元的减孔数
		//必须按单元当前选中规格重新计算每个单元的压稳信息，以便于真实计算各工况的压稳状态
		CPreProcessor::CalElemCompStableInfo(pElem,iDesignCode);		//计算当前受力单元长细比
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		BIAN_PARA_TYPE *pFlatParaType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pFlatParaType)
		{
			new_wing_wide = pFlatParaType->wide;
			new_wing_thick= pFlatParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindBianType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//计算当前受力单元的减孔数
		//CPreProcessor::CalElemCompStableInfo(pElem,iDesignCode);		//计算当前受力单元长细比
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_CABLE)
	{
		pElem->design_info.fCoefTensStrengthDecrease=1.0;
		pElem->design_info.iDangerousRule= 2;
		if(pElem->MaxTension.N>pElem->pOrgPole->GetWireForcePermit())	//强度控制
			pElem->m_iCtrlRule= 2;
		else
			pElem->m_iCtrlRule=0;
		pElem->design_info.fCoefOfBearLoad=pElem->design_info.fCoefEffic=pElem->MaxTension.N/pElem->pOrgPole->GetWireForcePermit();
		return pElem->m_iCtrlRule;
	}
	//初次确定杆件规格型号
	double L=pElem->Length();
	pElem->m_iCtrlRule=0;
	pElem->design_info.iDangerousRule = pElem->design_info.iDangerousLoadCase = 0;
	pElem->design_info.fCoefEffic=0;
	pElem->design_info.fCoefOfBearLoad = 0;
	double ft=pMatType->f(new_wing_thick);
	if(iDesignCode==2)	//ASCE 规范
		ft=0.9*pMatType->fy(new_wing_thick);

	double lamdaMax=pElem->LamdaMax();
	double UltimateLo_r;
	int tensLamdaLim,mainLamdaLim,xieLamdaLim,auxLamdaLim;
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		tensLamdaLim=Ta.SelMatPara.m_nTensiveTubeSlendernessLimit;
		mainLamdaLim=Ta.SelMatPara.m_nMainTubeSlendernessLimit;
		xieLamdaLim =Ta.SelMatPara.m_nXieTubeSlendernessLimit;
		auxLamdaLim =Ta.SelMatPara.m_nAuxTubeSlendernessLimit;
	}
	else
	{
		tensLamdaLim=Ta.SelMatPara.m_nTensivePoleSlendernessLimit;
		mainLamdaLim=Ta.SelMatPara.m_nMainPoleSlendernessLimit;
		xieLamdaLim =Ta.SelMatPara.m_nXiePoleSlendernessLimit;
		auxLamdaLim =Ta.SelMatPara.m_nAuxPoleSlendernessLimit;
	}
	double coefEqGamaRE=1.0;
	if(bEqCheck)
		coefEqGamaRE=Ta.EarthQuake.gamaRE;	//考虑抗震验算时结构构件抗力调整系数影响
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE
		||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		double An;	//净截面积
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		char cType='L';
		if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			cType=pElem->pOrgPole->size_cSubClassType;
		JG_PARA_TYPE *pJgType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X的角钢规格，规格验算失败！",pElem->pOrgPole->handle);
			throw sError;
		}
		//即使不需选材的杆件也需重新计算一下螺栓
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;	//承剪承载力
			ls_n=pElem->pOrgPole->connectStart.N;	//承压承载力
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectStart.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.N;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectEnd.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}
		else
			ls_d=SelBoltDByWingWide(wing_wide,NULL);

		An=pJgType->area*100-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//加1.5是用于考虑螺栓孔径要比螺栓直径大1.5mm
		pElem->design_info.hole_d=ls_d+1.5;
		//辅助材由于受拉比受压时强度折减系数小,故还需要考虑同样工况受同等大小拉力的验算 wjh-2011.9.22
		if(pElem->pOrgPole->IsAuxPole())
		{
			pElem->MaxTension=pElem->MaxCompression;
			pElem->MaxTension.N=fabs(pElem->MaxTension.N);	//同工况下验算同等大小拉力情况
		}
		if(pElem->MaxTension.N>0||pElem->MaxCompression.N>CElement::COMPRESSION_ZERO_FORCE)	//受拉或无受压工况
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			UltimateLo_r=tensLamdaLim;
			double bearcoef=lamdaMax/UltimateLo_r;
			pElem->design_info.fCoefEffic = bearcoef;
			pElem->design_info.iDangerousRule = 1;
			if(bearcoef>1.0)	//长细比不满足选材条件
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//长细比控制
			}
			else 
			{
				double m;	//构件强度折减系数
				if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
					m=1.0;
				else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				{
					if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
						m=0.85;	//钢管类构件
					else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_JG)
					{	//角钢类构件
						if(wing_wide>40)
							m=0.70;
						else
							m=0.55;
					}
				}
				//选材条件2：轴心受力构件强度
				pElem->design_info.fCoefTensStrengthDecrease=m;
				double bearcoef = pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 2;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 2;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
				{
					pElem->m_iCtrlRule=2;
					if(!bCheckAll)
						return 2;	//受拉强度控制
				}
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//受压杆
		{
			double m;	//构件强度折减系数
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//受压强度控制
			}
			
			//选材条件1：构件长细比
			if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20&&
				(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H'))
			{
				double bearCoef1=pElem->lamdaRc.Lo_r/auxLamdaLim;
				double bearCoef2=pElem->lamdaRyo.Lo_r/xieLamdaLim;
				double bearCoef3=pElem->lamdaRx.Lo_r/xieLamdaLim;
				if(!pElem->smcinfo.hasBothComp)
					bearCoef1=0;	//无同时受压情况出现
				if(bearCoef1>bearCoef2&&bearCoef1>bearCoef3)
				{
					bearcoef=bearCoef1;
					UltimateLo_r=auxLamdaLim;	//同时受压时长细比更危险
				}
				else if(bearCoef2>=bearCoef3)
				{
					bearcoef=bearCoef2;
					UltimateLo_r=xieLamdaLim;	//同时受压时长细比更危险
				}
				else
				{
					bearcoef=bearCoef3;
					UltimateLo_r=xieLamdaLim;	//同时受压时长细比更危险
				}
			}
			else
			{
				if(pElem->pOrgPole->layer(1)=='Z')
					UltimateLo_r=mainLamdaLim;	//主材lamda≤150
				else if(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H')
					UltimateLo_r=xieLamdaLim;	//斜材或横材（辅助受力材）lamda≤220
				else
					UltimateLo_r=auxLamdaLim;
				bearcoef = lamdaMax/UltimateLo_r;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 1;
			}
			if(bearcoef>1.0)// 150)
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//长细比控制
			}
			double mN;	//压杆稳定强度折减系数
			if(iDesignCode<2)
			{	//国内规范
				double fai=CalStableCoef1(pElem->CompStableLamda(),'B',pElem->pOrgPole->cMaterial);
				pElem->design_info.fCoefStablity=fai;
				//角钢类
				double t=wing_thick;
				double b=pJgType->wing_wide-pJgType->r-t;
				double bt_lim=202/sqrt(ft);
				if(b/t<=bt_lim)
					mN=1.0;
				else
					mN=1.677-0.677*(b/t)/bt_lim;
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=b/t;
				
				//选材条件3：轴心受压稳定计算
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pJgType->area*100*mN*ft);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 4;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//受压稳定控制
				}
			}
			else
			{	//ASCE规范角钢类
				double t=wing_thick;
				double b=pJgType->wing_wide-pJgType->r-t;
				double bt_lim=209.6/sqrt(pMatType->fy(t));
				if(b/t<=bt_lim)
					mN=1.0;
				else
					mN=1.677-0.677*(b/t)/bt_lim;
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=b/t;
				
				//选材条件3：轴心受压稳定计算
				double fy=pMatType->fy(t);
				double fai=CalStableCoef2(pElem->CompStableLamda(),pMatType->E,fy);
				pElem->design_info.fCoefStablity=fai;//fa/pMatType->fy(t);
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pJgType->area*100*mN*fy);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 4;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//受压稳定控制
				}
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		double D=new_wing_wide;
		double thick=new_wing_thick;
		TUBE_PARA_TYPE *pTubeType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X的钢管规格，规格验算失败！",pElem->pOrgPole->handle);
			throw sError;
		}

		if(pElem->MaxTension.N>0)
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			UltimateLo_r=tensLamdaLim;
			double bearcoef = lamdaMax/UltimateLo_r;
			pElem->design_info.fCoefEffic = bearcoef;
			pElem->design_info.iDangerousRule = 1;

			if(bearcoef>1.0)// 400)	//长细比不满足选材条件
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//长细比控制
			}
			else 
			{
				double m;	//构件强度折减系数
				if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
					m=1.0;
				else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
					m=0.85;	//钢管类构件
				//选材条件2：轴心受力构件强度
				pElem->design_info.fCoefTensStrengthDecrease=m;
				double bearcoef = pElem->MaxTension.N*coefEqGamaRE/((pTubeType->area*100)*m*ft);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 2;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 2;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
				{
					pElem->m_iCtrlRule=2;
					if(!bCheckAll)
						return 2;	//受拉强度控制
				}
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//受压杆
		{
			double m;	//构件强度折减系数
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/((pTubeType->area*100)*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//受压强度控制
			}
			
			//选材条件1：构件长细比
			if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20&&
				(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H'))
			{
				double bearCoef1=pElem->lamdaRc.Lo_r/auxLamdaLim;
				double bearCoef2=pElem->lamdaRyo.Lo_r/xieLamdaLim;
				double bearCoef3=pElem->lamdaRx.Lo_r/xieLamdaLim;
				if(!pElem->smcinfo.hasBothComp)
					bearCoef1=0;	//无同时受压情况出现
				if(bearCoef1>bearCoef2&&bearCoef1>bearCoef3)
				{
					bearcoef=bearCoef1;
					UltimateLo_r=auxLamdaLim;	//同时受压时长细比更危险
				}
				else if(bearCoef2>=bearCoef3)
				{
					bearcoef=bearCoef2;
					UltimateLo_r=xieLamdaLim;	//同时受压时长细比更危险
				}
				else
				{
					bearcoef=bearCoef3;
					UltimateLo_r=xieLamdaLim;	//同时受压时长细比更危险
				}
			}
			else
			{
				if(pElem->pOrgPole->layer(1)=='Z')
					UltimateLo_r=mainLamdaLim;	//主材lamda≤150
				else if(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H')
					UltimateLo_r=xieLamdaLim;	//斜材或横材（辅助受力材）lamda≤220
				else
					UltimateLo_r=auxLamdaLim;
				bearcoef = lamdaMax/UltimateLo_r;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 1;
			}
			if(bearcoef>1.0)// 150)
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//长细比控制
			}
			double mN;	//压杆稳定强度折减系数
			if(iDesignCode<2)
			{	//国内规范
				double fai;
				if(pTubeType->D<g_sysPara.fTubeWeldRoadMinD)	//无焊道的无缝钢管
					fai=CalStableCoef1(pElem->CompStableLamda(),'A',pElem->pOrgPole->cMaterial);
				else
					fai=CalStableCoef1(pElem->CompStableLamda(),'B',pElem->pOrgPole->cMaterial);
				pElem->design_info.fCoefStablity=fai;
				
				//钢管类
				double D0,t;	//外径与壁厚
				D0=pElem->GetWidth();
				t =pElem->GetThick();
				if(D0/t<=24100/pMatType->f(t))
					mN=1.0;
				else
					mN=0.75+6025/(D0*pMatType->f(t)/t);
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=D0/t;
				
				//选材条件3：轴心受压稳定计算
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pTubeType->area*100*mN*ft);	//
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 3;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//受压稳定控制
				}
			}
			else
			{	//ASCE规范 钢管
				double D0,t;	//钢管类外径与壁厚
				D0=pElem->GetWidth();
				t =pElem->GetThick();
				double fy=pMatType->fy(t);
				double fai=CalStableCoef2(pElem->CompStableLamda(),pMatType->E,fy);
				pElem->design_info.fCoefStablity=fai;//fa/pMatType->fy(t);
				
				if(D0/t<=24100/pMatType->f(t))
					mN=1.0;
				else
					mN=0.75+6025/(D0*pMatType->f(t)/t);
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=D0/t;
				
				//选材条件3：轴心受压稳定计算
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pTubeType->area*100*mN*fy);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 3;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//受压稳定控制
				}
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		double An;	//净截面积
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		BIAN_PARA_TYPE *pBianType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pBianType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X的扁铁规格，规格验算失败！",pElem->pOrgPole->handle);
			throw sError;
		}
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.N;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.N;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//螺栓强度不满足
			}
		}

		An=pBianType->wide*pBianType->thick-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//加1.5是用于考虑螺栓孔径要比螺栓直径大1.5mm
		
		if(pElem->MaxTension.N>0)
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			double m=1.0;	//构件强度折减系数
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//受拉强度控制
			}
		}
	}
	else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
	{
		double area = pElem->pOrgPole->userdef.area;
		//选材条件1：杆件长细比
		if(pElem->MaxTension.N>0)
		{	//受拉杆（预拉力的拉杆可不受长细比的限制）
			double m=1.0;	//构件强度折减系数
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//受拉强度控制
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//受压杆
		{
			double m=1.0;	//构件强度折减系数
			//选材条件2：轴心受力构件强度
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef=fabs(pElem->MaxCompression.N)*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.fc);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//上一次是轴心受拉构件强度控制
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//不满足杆件强度条件
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//受压强度控制
			}
		}
	}
	//更新单元长细比计算过程及结果输出信息
	pElem->UpdateLamdaInfo(iDesignCode!=1);
	return pElem->m_iCtrlRule;
}*/
#endif