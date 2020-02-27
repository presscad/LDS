// AnalyzeProcessor.cpp: implementation of the CAnalyzeProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"

#if defined(__LDS_)||defined(__TSA_)
#include "PreProcessor.h"
#include "AnalyzeProcessor.h"
#include "PostProcessor.h"
#include "Query.h"
#include "env_def.h"
#include "LibraryQuery.h"
#include "SortFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAnalyzeProcessor analysis; 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessBodyLoad& CProcessBodyLoad::operator +=(const CProcessBodyLoad& other)
{
	cfgword&=other.cfgword;
	this->iceWeightLoadCoef+=other.iceWeightLoadCoef;
	this->iceWeightLoadCoef2+=other.iceWeightLoadCoef2;
	this->selfWeightLoad+=other.selfWeightLoad;
	return *this;
}
CProcessBodyLoad operator+(const CProcessBodyLoad& v1,const CProcessBodyLoad& v2)
{
	CProcessBodyLoad load=v1;
	load+=v2;
	return load;
}
CProcessBodyLoad& CProcessBodyLoad::operator *=(double coef)
{
	this->iceWeightLoadCoef*=coef;
	this->iceWeightLoadCoef2*=coef;
	this->selfWeightLoad*=coef;
	return *this;
}
CProcessBodyLoad CProcessBodyLoad::operator *(double coef)
{
	CProcessBodyLoad load=*this;
	load.iceWeightLoadCoef*=coef;
	load.iceWeightLoadCoef2*=coef;
	load.selfWeightLoad*=coef;
	return load;
}
LAMDA_STABLE_INFO::LAMDA_STABLE_INFO()
{
	pStart=pEnd=NULL;
	L=R=Lo_r=0;
	bCalByElemLen=bothCompLive=false;
	muCoefBasic=muCoefLive=1.0;
	coefAreaFai=coefFai=0.0;
}
double LAMDA_STABLE_INFO::MuCoef()
{
	if(muCoefLive==1.0)
		return muCoefBasic;
	else if(muCoefBasic==1.0)
		return muCoefLive;
	else //if(muCoefBasic!=1.0&&muCoefLive!=1.0)
		return muCoefBasic*muCoefLive;
}
IMPLEMENT_GET(LAMDA_STABLE_INFO,Validate)
{	//Lo_r>0才表示该长细比计算判别项有效
	return Lo_r>EPS;
}
void LAMDA_STABLE_INFO::ClearZero()
{
	memset(this,0,sizeof(LAMDA_STABLE_INFO));
	muCoefBasic=muCoefLive=1.0;
	coefAreaFai=coefFai=1.0;
}

CElement::SMCITEM_GROUP::SMCITEM_GROUP()
{
	hasBothComp=false;
	iActiveItem=0;
	memset(this,0,sizeof(SMC_ITEM)*3);
}
bool CElement::SMCITEM_GROUP::ValidateActiveItem()
{
	if(iActiveItem<0)
		iActiveItem=0;
	else if(iActiveItem>2)
		iActiveItem=2;
	if(smcItems[iActiveItem].cSMCType!=SMC_ITEM::SMC_NONE)
		return true;
	else
	{
		for(int i=0;i<3;i++)
		{
			if(smcItems[i].cSMCType!=SMC_ITEM::SMC_NONE)
			{
				iActiveItem=i;
				return true;
			}
		}
		return false;
	}
}

double CElement::COMPRESSION_ZERO_FORCE=-0.1;	//N
double CElement::DIFFERENCE_ZERO_FORCE = 0.1;	//N
double CElement::DIFFERENCE_ZERO_STRESS= 0.001;	//MPa
CElement::CElement()
{
	length=0;
	pStart=pEnd=NULL;
	pOrgPole=NULL;
	memset(&relaElem,0,sizeof(RELATIVE_ELEM));
	pFemElem=NULL;
	pStartBraceForceElem=pEndBraceForceElem=NULL;
	ciFreedomStart=ciFreedomEnd=0;
	BearTens1Comp2=0;
	elem_type = 0;	//默认为二力杆或经典梁
	bAnchorEnd=true;
	start_joint_type = JOINT_AUTOJUSTIFY;
	end_joint_type = JOINT_AUTOJUSTIFY;
	real_start_joint_type=real_end_joint_type=JOINT_HINGE;
	start_force_type=end_force_type=ECCENTRIC_FORCE;
	bEffectN=false;
	N=0.0;
	W0=H0=V0=T0=0.0;
	m_fStartBraceNCoef=m_fEndBraceNCoef=0.0;
	m_iSizeIndex=0;
	m_fWeightRaiseCoef=0.0;
	//m_bValidStartRestStiff=m_bValidEndRestStiff=TRUE;
	memset(&design_info,0,sizeof(ELEMDESIGN_INFO));
	design_info.fCoefTensStrengthDecrease=design_info.fCoefCompStrengthDecrease=1.0;
	design_info.fCoefStablity=1.0;
	memset(&MaxTension,0,sizeof(ELEMFORCE_ENV));
	memset(&MaxCompression,0,sizeof(ELEMFORCE_ENV));
	memset(&MaxMoment,0,sizeof(ELEMFORCE_ENV));
	memset(&MaxCompStable,0,sizeof(ELEMCOMPSTABLE_ENV));
	design_info.mN=1.0;
	//m_bTensAndCompElem=TRUE;
	//m_bBothCompElem=TRUE;
	m_bValidStartRestStiff=FALSE;
	m_bValidEndRestStiff=FALSE;
	id=0;
	feature=0;
	//不赋初始值会导致部分情况下因初始化为false最终导致无法初始化单元规格对象 wjh-2017.2.25
	m_xParaType.m_bOutdated=true;
}
CElement& CElement::operator=(const CElement& element)
{
	pStart=element.pStart;
	pEnd=element.pEnd;
	length=element.Length();
	lenVec=pEnd->Position(false)-pStart->Position(false);
	lenStdVec=element.LenStdVec();
	pOrgPole=element.pOrgPole;
	bodyLoad=element.BodyLoad();
	cfgword=element.cfgword;
	relaElem=element.relaElem;
	pFemElem=element.pFemElem;
	pStartBraceForceElem=element.pStartBraceForceElem;
	pEndBraceForceElem=element.pEndBraceForceElem;
	BearTens1Comp2=element.BearTens1Comp2;
	elem_type = element.elem_type;	//默认为二力杆或经典梁
	start_joint_type = element.start_joint_type;
	end_joint_type = element.end_joint_type;
	real_start_joint_type=element.real_start_joint_type;
	real_end_joint_type=element.real_end_joint_type;
	start_force_type=element.start_force_type;
	end_force_type=element.end_force_type;
	bEffectN=element.bEffectN;
	N=element.N;
	W0=element.W0;
	H0=element.H0;
	V0=element.V0;
	T0=element.T0;
	bAnchorEnd=element.bAnchorEnd;
	moment=element.moment;
	lamdaRyo=element.lamdaRyo;
	lamdaRx=element.lamdaRx;
	_lamdaRc=element._lamdaRc;
	smcinfo=element.smcinfo;
	m_fStartBraceNCoef=element.m_fStartBraceNCoef;
	m_fEndBraceNCoef=element.m_fEndBraceNCoef;
	m_iSizeIndex=element.m_iSizeIndex;
	m_iCtrlRule=element.m_iCtrlRule;
	m_fWeightRaiseCoef=element.m_fWeightRaiseCoef;
	m_bValidStartRestStiff=element.m_bValidStartRestStiff;
	m_bValidEndRestStiff=element.m_bValidEndRestStiff;
	memcpy(&design_info,&element.design_info,sizeof(ELEMDESIGN_INFO));
	//design_info.fCoefTensStrengthDecrease=design_info.fCoefCompStrengthDecrease=1.0;
	//design_info.fCoefStablity=1.0;
	//design_info.mN=1.0;
	memcpy(&MaxTension,&element.MaxTension,sizeof(ELEMFORCE_ENV));
	memcpy(&MaxCompression,&element.MaxCompression,sizeof(ELEMFORCE_ENV));
	memcpy(&MaxMoment,&element.MaxMoment,sizeof(ELEMFORCE_ENV));
	memcpy(&MaxCompStable,&element.MaxCompStable,sizeof(ELEMCOMPSTABLE_ENV));
	//m_bTensAndCompElem=TRUE;
	//m_bBothCompElem=TRUE;
	id=element.id;
	feature=element.feature;
	//m_xParaType=element.m_xParaType;
	return *this;
}
SMC_ITEM CElement::ActiveSmcItem()
{
	smcinfo.ValidateActiveItem();
	return smcinfo.smcItems[smcinfo.iActiveItem];
}
LAMDA_STABLE_INFO CElement::LamdaRc()
{
	smcinfo.ValidateActiveItem();
	return smcinfo.lamdaItems[smcinfo.iActiveItem];
	//if(smcinfo.ValidateActiveItem())
	//	return smcinfo.lamdaItems[smcinfo.iActiveItem];
	//else
	//	return LAMDA_STABLE_INFO();
}
//复制除CElemInfo::relaElem及标识之外的原始设计信息到原始设计单元中
void CElement::CopyBasicInfoTo(CElemInfo* pElemInfo,bool copyCalLenInfo/*=true*/)
{
	pElemInfo->m_hStartNode=pStart->handle;
	pElemInfo->m_hEndNode=pEnd->handle;
	pElemInfo->m_hParentRod=pOrgPole->handle;
	//实时自由度在实时计算时更新，不应在此处更新（因为此时的CElement有可能未计算实时自由度) wjh-2017.7.14
	//pElemInfo->ciFreedomStart=ciFreedomStart;
	//pElemInfo->ciFreedomEnd=ciFreedomEnd;
	if(copyCalLenInfo)
	{
		pElemInfo->LoRx.bCalByElemLen=lamdaRx.bCalByElemLen;
		pElemInfo->LoRyo.bCalByElemLen=lamdaRyo.bCalByElemLen;
		pElemInfo->cCalLenType=(char)pOrgPole->CalLenCoef.iTypeNo;
		pElemInfo->LoRx.hStartNode=pElemInfo->LoRx.hEndNode=pElemInfo->LoRyo.hStartNode=pElemInfo->LoRyo.hEndNode=0;
		if(lamdaRx.pStart)
			pElemInfo->LoRx.hStartNode=lamdaRx.pStart->handle;
		if(lamdaRx.pEnd)
			pElemInfo->LoRx.hEndNode=lamdaRx.pEnd->handle;
		pElemInfo->LoRx.coef=lamdaRx.muCoefBasic;
		if(lamdaRyo.pStart)
			pElemInfo->LoRyo.hStartNode=lamdaRyo.pStart->handle;
		if(lamdaRyo.pEnd)
			pElemInfo->LoRyo.hEndNode=lamdaRyo.pEnd->handle;
		pElemInfo->LoRyo.coef=lamdaRyo.muCoefBasic;
		//TODO:pElemInfo->horiElem还未处理　wjh-2016.6.06
		memcpy(pElemInfo->smcItems,smcinfo.smcItems,sizeof(SMC_ITEM)*3);
	}
	//CSuperSmartPtr<CLDSPart> pHorizPole=pElem->pOrgPole->BelongModel()->FromPartHandle(pElem->pOrgPole->CalLenCoef.hHorizPole);
	//horiElem.hStart
}
bool CElement::IsHasMirInfo()
{	return relaElem.idElemMirX>0||relaElem.idElemMirY>0||relaElem.idElemMirZ>0;}
extern int MirPointI(int quad,MIRMSG msg);
//是否与pElem有对称关联
BOOL CElement::IsMirWithElem(CElement *pElem)
{
	RELATIVE_OBJECT *pRelaObj=pOrgPole->GetRelativeObjInfo(pElem->pOrgPole->handle,false);
	if(pRelaObj==NULL)
		return FALSE;
	bool bMirStart=false,bMirEnd=false;
	if((pStart==pOrgPole->pStart&&pElem->pStart==pElem->pOrgPole->pStart) ||
		MirPointI(pStart->point_i,pRelaObj->mirInfo)==pElem->pStart->point_i)
		bMirStart=true;
	if((pEnd==pOrgPole->pEnd&&pElem->pEnd==pElem->pOrgPole->pEnd) ||
		MirPointI(pEnd->point_i,pRelaObj->mirInfo)==pElem->pEnd->point_i)
		bMirEnd=true;
	return bMirStart&&bMirEnd;
}
//比较(对称)单元的始末节点号
int CElement::CompareElemPointI(CElement *pElem)
{
	long start_i=min(pStart->point_i,pEnd->point_i);
	long end_i=max(pStart->point_i,pEnd->point_i);
	long start_i2=min(pElem->pStart->point_i,pElem->pEnd->point_i);
	long end_i2=max(pElem->pStart->point_i,pElem->pEnd->point_i);
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
	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）惯性矩(mm4)
double CElement::GetWorkI(int para)
{
	if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		/*JG_PARA_TYPE *pType;
		if(pParaType==NULL)
			pParaType=pType=FindJgType(pOrgPole->size_wide,pOrgPole->size_thick,pOrgPole->size_cSubClassType);
		else
			pType=(JG_PARA_TYPE*)pParaType;*/
		JG_PARA_TYPE *pType=FindJgType(pOrgPole->size_wide,pOrgPole->size_thick,pOrgPole->size_cSubClassType);
		if(pType)
		{
			if(para==0)	//返回平轴惯性矩
				return pType->Ix*10000;	//cm4->mm4
			else		//返回最小轴惯性矩
				return pType->Iy0*10000;//cm4->mm4
		}
		else
			return 0;
	}
	else if(pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		/*TUBE_PARA_TYPE *pType;
		if(pParaType==NULL)
			pParaType=pType=FindTubeType(pOrgPole->GetWidth(),pOrgPole->GetThick());
		else
			pType=(TUBE_PARA_TYPE*)pParaType;*/
		TUBE_PARA_TYPE *pType=FindTubeType(pOrgPole->GetWidth(),pOrgPole->GetThick());
		if(pType)
			return pType->I*10000;//cm4->mm4
		else
			return 0;
	}
	else if(pOrgPole->size_idClassType==CLS_LINEFLAT)
		return pow(pOrgPole->GetWidth()*pOrgPole->GetThick(),2)*0.0625;
	//else if(pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
	//	return pOrgPole->userdef.area*pOrgPole->userdef.area*0.0625;
	else
		return 0;
}
	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）抗弯截面模量(mm3)
double CElement::GetWorkW(int para)
{
	if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pType=FindJgType(pOrgPole->size_wide,pOrgPole->size_thick,pOrgPole->size_cSubClassType);
		if(pType)
		{
			if(para==0)	//返回平行轴惯性矩
				return pType->Wx*1000;	//cm4->mm4
			else		//返回最小轴惯性矩
				return pType->Wy0*1000;//cm4->mm4
		}
		else
			return 0;
	}
	else if(pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pType=FindTubeType(pOrgPole->GetWidth(),pOrgPole->GetThick());
		if(pType)
			return pType->W*1000;//cm4->mm4
		else
			return 0;
	}
	//else if(pOrgPole->size_idClassType==CLS_LINEFLAT)
	//	return pow(pOrgPole->GetWidth()*pOrgPole->GetThick(),2)*0.0625;
	//else if(pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
	//	return pOrgPole->userdef.area*pOrgPole->userdef.area*0.0625;
	else
		return 0;
}
	//返回杆件指定（角钢时para=0平行轴或para=1最小轴）回转半径(mm)
double CElement::GetWorkR(int para/*=0*/)
{
	if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pType=(JG_PARA_TYPE*)pParaType;
		if(pType==NULL)
			pType=FindJgType(pOrgPole->size_wide,pOrgPole->size_thick,pOrgPole->size_cSubClassType);
		if(pType)
		{
			if(para==0)	//返回平轴惯性矩
				return pType->Rx*10;	//cm->mm
			else		//返回最小轴惯性矩
				return pType->Ry0*10;//cm->mm
		}
		else
			return 0;
	}
	else if(pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pType=(TUBE_PARA_TYPE*)pParaType;
		if(pType==NULL)
			pType=FindTubeType(pOrgPole->GetWidth(),pOrgPole->GetThick());
		if(pType)
			return pType->r*10;//cm4->mm4
		else
			return 0;
	}
	else if(pOrgPole->size_idClassType==CLS_LINEFLAT)	//0.25是一个经验数据
		return pOrgPole->GetWidth()*pOrgPole->GetThick()*0.25;
	//else if(pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
	//	return pOrgPole->userdef.area*0.25;
	else
		return 0;
}
// int CElement::GetStartForceNum(f3dPoint *force_face_norm)
// {
// 	f3dPoint axis_vec=pEnd->pos-pStart->pos;
// 	return pStart->GetForceFaceNum(axis_vec,force_face_norm);
// }
// 
// int CElement::GetEndForceNum(f3dPoint *force_face_norm)
// {
// 	f3dPoint axis_vec=pStart->pos-pEnd->pos;
// 	return pEnd->GetForceFaceNum(axis_vec,force_face_norm);
// }
CProcessBodyLoad CElement::CalBodyLoad(double lowest_z,double ice_density/*=900*/)
{
	bodyLoad.cfgword=cfgword;
	//double len = DISTANCE(pStart->pos,pEnd->pos);	//mm
	double weight=0.0;
	if(CPreProcessor::ActiveDesignCode->CodeSerial()==CODES::GB50135_2006)
	{
		//计算覆冰荷载系数
		double alfa2=QueryIceReviseCoefAlfa2(lowest_z-0.0005*(pStart->Position(false).z+pEnd->Position(false).z));
		if(pOrgPole->size_idClassType==CLS_LINETUBE)//||pOrgPole->guige.pole_type==TRUSS_POLE_YUANGANG)
		{
			double alfa1=QueryIceReviseCoefAlfa1(pOrgPole->size_wide);
			double fact=alfa1*alfa2;
			bodyLoad.iceWeightLoadCoef=Length()*Pi*fact*pOrgPole->size_wide*ice_density*9.8e-9;
			bodyLoad.iceWeightLoadCoef2=Length()*Pi*fact*fact*ice_density*9.8e-9;
		}
		else if(pOrgPole->size_idClassType==CLS_LINEANGLE)
			bodyLoad.iceWeightLoadCoef=Length()*23.52e-9*pOrgPole->size_wide*alfa2*ice_density;
		else if(pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(pOrgPole->size_cSubClassType=='T'||pOrgPole->size_cSubClassType=='D')
				bodyLoad.iceWeightLoadCoef=Length()*47.04e-9*pOrgPole->size_wide*alfa2*ice_density;
			else //if(pOrgPole->size_cSubClassType=='X')
				bodyLoad.iceWeightLoadCoef=Length()*94.08e-9*pOrgPole->size_wide*alfa2*ice_density;
		}
		else if(pOrgPole->size_idClassType==CLS_LINEFLAT)
			bodyLoad.iceWeightLoadCoef=Length()*11.76e-9*(pOrgPole->size_wide+pOrgPole->size_thick)*alfa2*ice_density;
		else
			bodyLoad.iceWeightLoadCoef=Length()*47.04e-9*pOrgPole->GetArea()*alfa2*ice_density/pOrgPole->size_wide;
	}
	else
		bodyLoad.iceWeightLoadCoef=bodyLoad.iceWeightLoadCoef2=0.0;
	
	if(pOrgPole->size_idClassType==0)
		bodyLoad.selfWeightLoad=Length()*pOrgPole->userdef.unit_weight*0.0098*pOrgPole->m_fWeightRaiseCoef;
	else if(pOrgPole->size_idClassType==CLS_CABLE)
		bodyLoad.selfWeightLoad=0;	//拉线单元的重力荷载不需要等效成节点荷载，系统会在拉线单元中考虑
	else
	{
		double unit_weight=globalLibrary.GetWeightOf1Meter(pOrgPole->size_wide,pOrgPole->size_thick,0,pOrgPole->size_idClassType,pOrgPole->size_cSubClassType);
		bodyLoad.selfWeightLoad=Length()*unit_weight*0.0098*pOrgPole->m_fWeightRaiseCoef;
	}
	return bodyLoad;
}

CAnalyzeProcessor::CAnalyzeProcessor()
{
	m_pPreProcessor=NULL;
	m_pPostProcessor=NULL;
	m_bHasCableElem=false;
}

CAnalyzeProcessor::~CAnalyzeProcessor()
{

}
int CAnalyzeProcessor::GetForceFaceNum(CLDSNode *pNode,f3dPoint axis_vec,f3dPoint *force_face_norm/*=FALSE*/)
{
	f3dPoint force_face_vec,face_norm[2];
	if(!axis_vec.IsZero())
		normalize(axis_vec);
	int num_face=0;
	for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		if(pElem->pFemElem==NULL)
			continue;	//辅材单元
		if(pElem->pStart!=pNode&&pElem->pEnd!=pNode)	//非以此节点为端节点的杆件单元
			continue;
		if(axis_vec.IsZero())
		{
			axis_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
			normalize(axis_vec);
			continue;
		}
		force_face_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
		normalize(force_face_vec);
		if(fabs(axis_vec*force_face_vec)>EPS_COS2)	//共线同向杆件单元
			continue;
		if(num_face==0)
		{
			face_norm[0]=axis_vec^force_face_vec;
			normalize(face_norm[0]);
			num_face=1;
		}
		else if(fabs(force_face_vec*face_norm[0])>EPS2)
		{	//出现第二受力面杆件
			num_face=2;
			break;
		}
	}
	if(force_face_norm)
		*force_face_norm=face_norm[0];
	if(pNode->IsHasRestrict(X_RESTRICT)&&fabs(face_norm[0].x)>EPS2)
		num_face++;
	if(pNode->IsHasRestrict(Y_RESTRICT)&&fabs(face_norm[0].y)>EPS2)
		num_face++;
	if(pNode->IsHasRestrict(Z_RESTRICT)&&fabs(face_norm[0].z)>EPS2)
		num_face++;
	num_face=min(num_face,2);
	return num_face;
}
void CAnalyzeProcessor::AnalyzeElemDegree()
{
	CString sError;
	CHashTable<int>forceSupportTbl;
	int startForceSupport,endForceSupport;
	forceSupportTbl.CreateHashTable(nodeset.GetNodeNum());
	m_bHasCableElem=false;
	for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		if(pElem->pFemElem==NULL)
			continue;	//辅材
		else if(pElem->elem_type==1)
		{
			if(pElem->T0<EPS)
				logerr.Log("%d-%d索单元缺少预张紧力，无法正常进行索单元计算!",pElem->pStart->point_i,pElem->pEnd->point_i);
			m_bHasCableElem=true;	//结构体中有拉线单元
			continue;
		}
		BOOL bPush=hashElems.push_stack();
		if(!forceSupportTbl.GetValueAt(pElem->pStart->point_i,startForceSupport))
		{
			startForceSupport=GetForceFaceNum(pElem->pStart,f3dPoint(0,0,0),NULL);
			forceSupportTbl.SetValueAt(pElem->pStart->point_i,startForceSupport);
		}
		if(!forceSupportTbl.GetValueAt(pElem->pEnd->point_i,endForceSupport))
		{
			endForceSupport=GetForceFaceNum(pElem->pEnd,f3dPoint(0,0,0),NULL);
			forceSupportTbl.SetValueAt(pElem->pEnd->point_i,endForceSupport);
		}
		if(pElem->pOrgPole->m_iElemType==0)
		{	//自动判断
			if(startForceSupport>=2)
				pElem->pFemElem->start_joint_type=JOINT_HINGE;
			else if(pElem->pStart!=pElem->pOrgPole->pStart)
				pElem->pFemElem->start_joint_type=JOINT_RIGID;
			else
				pElem->pFemElem->start_joint_type=JOINT_HINGE;
			if(endForceSupport>=2)
				pElem->pFemElem->end_joint_type=JOINT_HINGE;
			else if(pElem->pEnd!=pElem->pOrgPole->pEnd)
				pElem->pFemElem->end_joint_type=JOINT_RIGID;
			else
				pElem->pFemElem->end_joint_type=JOINT_HINGE;
		}
		else if(pElem->pOrgPole->m_iElemType==1)
		{	//二力杆桁架单元
			pElem->pFemElem->start_joint_type=JOINT_HINGE;
			pElem->pFemElem->end_joint_type=JOINT_HINGE;
		}
		else if(pElem->pOrgPole->m_iElemType==2)
		{	//经典梁单元
			if(pElem->pStart!=pElem->pOrgPole->pStart)
				pElem->pFemElem->start_joint_type=JOINT_RIGID;
			else if(startForceSupport<2||pElem->pOrgPole->start_joint_type==JOINT_RIGID)
				pElem->pFemElem->start_joint_type=JOINT_RIGID;
			else
				pElem->pFemElem->start_joint_type=JOINT_HINGE;
			if(pElem->pEnd!=pElem->pOrgPole->pEnd)
				pElem->pFemElem->end_joint_type=JOINT_RIGID;
			else if(endForceSupport<2||pElem->pOrgPole->end_joint_type==JOINT_RIGID)
				pElem->pFemElem->end_joint_type=JOINT_RIGID;
			else
				pElem->pFemElem->end_joint_type=JOINT_HINGE;
		}
		//判定各节点的自由度
		if(pElem->pFemElem->start_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("系统把0X%X杆件内单元始端作为梁处理，但此杆件不具有抗弯能力，请检查规格表!",pElem->pOrgPole->handle);
				throw sError;
			}
			pElem->pFemElem->pStart->dimension=6;
		}
		else if(pElem->pFemElem->pStart->dimension!=6)
			pElem->pFemElem->pStart->dimension=3;
		if(pElem->pFemElem->end_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("系统把0X%X杆件内单元终端作为梁处理，但此杆件不具有抗弯能力，请检查规格表!",pElem->pOrgPole->handle);
				throw sError;
			}
			pElem->pFemElem->pEnd->dimension=6;
		}
		else if(pElem->pFemElem->pEnd->dimension!=6)
			pElem->pFemElem->pEnd->dimension=3;
		pElem->real_start_joint_type=pElem->pFemElem->start_joint_type;
		pElem->real_end_joint_type=pElem->pFemElem->end_joint_type;
		pElem->ciFreedomStart=pElem->real_start_joint_type==JOINT_RIGID?6:3;
		pElem->ciFreedomEnd  =pElem->real_end_joint_type==JOINT_RIGID?6:3;
		if(bPush)
			hashElems.pop_stack();
	}
	for(FEM_NODE* pFemNode=femNodeList.GetFirst();pFemNode;pFemNode=femNodeList.GetNext())
	{
		int supportDirections=0;
		forceSupportTbl.GetValueAt(pFemNode->point_i,supportDirections);
		if(supportDirections>=2)
			continue;
		CHashList<GEPOINT>hashVectors;
		//对于轴向支撑或平面支撑杆件，应寻找穿越节点的共线单元对 wjh-2015.6.3
		for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
		{
			if(pElem->pFemElem==NULL)
				continue;	//辅材
			if(pElem->pFemElem->pStart!=pFemNode&&pElem->pFemElem->pEnd!=pFemNode)
				continue;
			double prev_justify=0;
			CElement* pJointElem=NULL,*pPerfectJointElem=NULL;
			GEPOINT lenStdVec=pElem->LenStdVec();
			for(GEPOINT *pGeVec=hashVectors.GetFirst();pGeVec;pGeVec=hashVectors.GetNext())
			{
				double justify=fabs(lenStdVec*(*pGeVec));
				if(justify>EPS_COS2)	//共线
				{
					pJointElem=hashElems.GetValue(hashVectors.GetCursorKey());
					if(justify>prev_justify)
					{
						pPerfectJointElem=pJointElem;
						prev_justify=justify;
						break;
					}
				}
			}
			pJointElem=pPerfectJointElem;
			if(pGeVec==NULL)
				hashVectors.SetValue(pElem->id,lenStdVec);
			else if(pJointElem)
			{
				if(pElem->pFemElem->pStart==pFemNode&&pElem->start_joint_type!=JOINT_HINGE)
					pElem->real_start_joint_type=pElem->pFemElem->start_joint_type=JOINT_RIGID;
				else if(pElem->pFemElem->pEnd==pFemNode&&pElem->end_joint_type!=JOINT_HINGE)
					pElem->real_end_joint_type=pElem->pFemElem->end_joint_type=JOINT_RIGID;
				if(pJointElem->pFemElem->pStart==pFemNode&&pJointElem->start_joint_type!=JOINT_HINGE)
					pJointElem->real_start_joint_type=pJointElem->pFemElem->start_joint_type=JOINT_RIGID;
				else if(pJointElem->pFemElem->pEnd==pFemNode&&pJointElem->end_joint_type!=JOINT_HINGE)
					pJointElem->real_end_joint_type=pJointElem->pFemElem->end_joint_type=JOINT_RIGID;
				pFemNode->dimension=6;	//
				break;
			}
		}
	}
}
double CProcessBodyLoad::IceLoadCoef()
{
	return iceWeightLoadCoef;
}
double CProcessBodyLoad::IceLoadCoef2()
{
	return iceWeightLoadCoef2;
}
double CProcessBodyLoad::SelfWeight()
{
	return selfWeightLoad;
}
void CAnalyzeProcessor::AnalyzeElemDegree(CBlockResolver* pResolver)
{
	if(pResolver==NULL)
	{
		AnalyzeElemDegree();
		return;
	}
	CString sError;
	m_bHasCableElem=false;
	pResolver->ClearBodyElems();
	for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		if(pElem->pFemElem==NULL)
			continue;	//辅材
		else if(pElem->elem_type==1)
		{
			if(pElem->T0<EPS)
				logerr.Log("%d-%d索单元缺少预张紧力，无法正常进行索单元计算!",pElem->pStart->point_i,pElem->pEnd->point_i);
			m_bHasCableElem=true;	//结构体中有拉线单元
			continue;
		}
		pElem->pFemElem->start_joint_type=pElem->real_start_joint_type;
		pElem->pFemElem->end_joint_type=pElem->real_end_joint_type;
		//判定各节点的自由度
		if(pElem->pFemElem->start_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("系统把0X%X杆件内单元始端作为梁处理，但此杆件不具有抗弯能力，请检查规格表!",pElem->pOrgPole->handle);
				throw sError;
			}
			pElem->pFemElem->pStart->dimension=6;
		}
		else if(pElem->pFemElem->pStart->dimension!=6)
			pElem->pFemElem->pStart->dimension=3;
		if(pElem->pFemElem->end_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("系统把0X%X杆件内单元终端作为梁处理，但此杆件不具有抗弯能力，请检查规格表!",pElem->pOrgPole->handle);
				throw sError;
			}
			pElem->pFemElem->pEnd->dimension=6;
		}
		else if(pElem->pFemElem->pEnd->dimension!=6)
			pElem->pFemElem->pEnd->dimension=3;
		pElem->ciFreedomStart=(BYTE)pElem->pFemElem->pStart->dimension;
		pElem->ciFreedomEnd  =(BYTE)pElem->pFemElem->pEnd->dimension;
		if(pElem->pOrgPole->layer(0)=='L')	//接腿单元默认不放在子结构中
			pResolver->AddElem(*pElem->pFemElem);
	}
}
//<DEVELOP_PROCESS_MARK nameId="CAnalyzeProcessor::ApplyBraceForceToAuxElem">
void CAnalyzeProcessor::ApplyBraceForceToElem()
{
	//计算辅材验算压力及斜材对主材最小支撑压力
	double coef_p2p=CPreProcessor::ActiveDesignCode->GetBraceCoefOfPrimaryToPost();
	double coef_r2p=CPreProcessor::ActiveDesignCode->GetBraceCoefOfRedundantToPost();
	double coef_r2p2=CPreProcessor::ActiveDesignCode->GetBraceCoefOfRedundantToPrimary();
	for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		//《技术规定》DL/T 5154-2012 P79条文，仅在对变坡塔未考虑塔身风折减时才考虑斜材对主材的承载力,
		//	故综合考虑去掉此处斜材情况 wjh-2017.3.21
		//if(!(pElem->pOrgPole->IsAuxPole()||(coef_p2p>0&&pElem->pOrgPole->IsPrimaryRod(true))))
		if(!pElem->pOrgPole->IsAuxPole())
			continue;	//主材或辅材
		double coef_2p=coef_r2p;
		if(pElem->pOrgPole->IsPrimaryRod(true))
			coef_2p=coef_p2p;
		//if(pElem->pOrgPole->IsAuxPole()&&pElem->pOrgPole->sel_mat_method==AUX_ONLY_PRIMARY)
		if(pElem->pOrgPole->sel_mat_method==AUX_ONLY_PRIMARY)
			coef_2p=0;	//当前辅材仅考虑对斜材(普通受力材的支撑, 如横隔面角辅材) wjh-2017.2.8
		double startN=0,endN=0;
		if(pElem->pStartBraceForceElem&&pElem->pOrgPole->sel_mat_method!=AUX_BY_END&&pElem->pStartBraceForceElem->N<0)
		{
			if(pElem->pStartBraceForceElem->pOrgPole->IsPostRod())//>layer(1)=='Z')
				startN=fabs(pElem->m_fStartBraceNCoef*coef_2p*pElem->pStartBraceForceElem->N);
			else
				startN=fabs(pElem->m_fStartBraceNCoef*coef_r2p2*pElem->pStartBraceForceElem->N);
		}
		if(pElem->pEndBraceForceElem&&pElem->pOrgPole->sel_mat_method!=AUX_BY_START&&pElem->pEndBraceForceElem->N<0)
		{
			if(pElem->pEndBraceForceElem->pOrgPole->IsPostRod())//layer(1)=='Z')
				endN=fabs(pElem->m_fEndBraceNCoef*coef_2p*pElem->pEndBraceForceElem->N);
			else
				endN=fabs(pElem->m_fEndBraceNCoef*coef_r2p2*pElem->pEndBraceForceElem->N);
		}
		double maxN=-max(startN,endN);	//让辅助材承受压力
		if((pElem->pOrgPole->IsAuxPole()||pElem->N<0)&&maxN<pElem->N)
		{
			pElem->bEffectN=true;
			pElem->N=maxN;
		}
	}
	//计算两端连接在斜材时其余辅材传递过来的支撑力
	for(CNodeBraceAuxElems* pBraceElems=m_pPreProcessor->result.listBraceLinks.GetFirst();
		pBraceElems;pBraceElems=m_pPreProcessor->result.listBraceLinks.GetNext())
	{
		if(pBraceElems->SecondaryElemStiff()==NULL)
			continue;
		double secondaryN=0;
		for(AUXELEM_CONST *pAuxStiff=pBraceElems->linkElemsConst.GetFirst();pAuxStiff;
			pAuxStiff=pBraceElems->linkElemsConst.GetNext())
		{
			if(pAuxStiff!=pBraceElems->SecondaryElemStiff())
				secondaryN+=pAuxStiff->pElem->N*pAuxStiff->cosa;
		}
		secondaryN/=pBraceElems->SecondaryElemStiff()->cosa;
		if(secondaryN>pBraceElems->SecondaryElemStiff()->pElem->N)
		{
			pBraceElems->SecondaryElemStiff()->pElem->N=secondaryN;
			pBraceElems->SecondaryElemStiff()->pElem->bEffectN=true;
		}
	}
}
//</DEVELOP_PROCESS_MARK>
int compare_func1(const NODE_PTR& pNode1,const NODE_PTR& pNode2)
{
	if(pNode1->point_i<pNode2->point_i)
		return -1;
	else if(pNode1->point_i==pNode2->point_i)
		return 0;
	else
		return 1;
}

int compare_func2(const ELEM_PTR &pElem1,const ELEM_PTR &pElem2)
{
	long no_start[2],no_end[2],baseno_start[2],baseno_end[2];//,quad1[2],quad2[2];
	no_start[0]=min(pElem1->pStart->point_i,pElem1->pEnd->point_i);
	no_end[0]=max(pElem1->pStart->point_i,pElem1->pEnd->point_i);
	no_start[1]=min(pElem2->pStart->point_i,pElem2->pEnd->point_i);
	no_end[1]=max(pElem2->pStart->point_i,pElem2->pEnd->point_i);
	RELATIVE_OBJECT relaObj;
	if(pElem1->IsFatherElem())
	{
		baseno_start[0]=no_start[0];
		baseno_end[0]=no_end[0];
	}
	else
	{
		pElem1->pOrgPole->GetFatherMirObj(&relaObj);
		if(pElem1->pStart!=pElem1->pOrgPole->pStart)
			baseno_start[0]=MirPointI(no_start[0],relaObj.mirInfo);
		else
		{
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem1->pStart->Position(false).y)<EPS)	//X轴对称
				baseno_start[0]=no_start[0];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem1->pStart->Position(false).y)<EPS)	//Y轴对称
				baseno_start[0]=no_start[0];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem1->pStart->Position(false).y)<EPS&&fabs(pElem1->pStart->Position(false).y)<EPS)	//Z轴对称
				baseno_start[0]=no_start[0];
			else
				baseno_start[0]=MirPointI(no_start[0],relaObj.mirInfo);
		}
		if(pElem1->pEnd!=pElem1->pOrgPole->pEnd)
			baseno_end[0]=MirPointI(no_end[0],relaObj.mirInfo);
		else
		{
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem1->pEnd->Position(false).y)<EPS)	//X轴对称
				baseno_end[0]=no_end[0];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem1->pEnd->Position(false).y)<EPS)	//Y轴对称
				baseno_end[0]=no_end[0];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem1->pEnd->Position(false).y)<EPS&&fabs(pElem1->pEnd->Position(false).y)<EPS)	//Z轴对称
				baseno_end[0]=no_end[0];
			else
				baseno_end[0]=MirPointI(no_end[0],relaObj.mirInfo);
		}
	}
	if(pElem2->IsFatherElem())
	{
		baseno_start[1]=no_start[1];
		baseno_end[1]=no_end[1];
	}
	else
	{
		pElem2->pOrgPole->GetFatherMirObj(&relaObj);
		if(pElem2->pStart!=pElem2->pOrgPole->pStart)
			baseno_start[1]=MirPointI(no_start[1],relaObj.mirInfo);
		else
		{
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem2->pStart->Position(false).y)<EPS)	//X轴对称
				baseno_start[1]=no_start[1];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem2->pStart->Position(false).y)<EPS)	//Y轴对称
				baseno_start[1]=no_start[1];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem2->pStart->Position(false).y)<EPS&&fabs(pElem2->pStart->Position(false).y)<EPS)	//Z轴对称
				baseno_start[1]=no_start[1];
			else
				baseno_start[1]=MirPointI(no_start[1],relaObj.mirInfo);
		}
		if(pElem2->pEnd!=pElem2->pOrgPole->pEnd)
			baseno_end[1]=MirPointI(no_end[1],relaObj.mirInfo);
		else
		{
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem2->pEnd->Position(false).y)<EPS)	//X轴对称
				baseno_end[1]=no_end[1];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem2->pEnd->Position(false).y)<EPS)	//Y轴对称
				baseno_end[1]=no_end[1];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem2->pEnd->Position(false).y)<EPS&&fabs(pElem2->pEnd->Position(false).y)<EPS)	//Z轴对称
				baseno_end[1]=no_end[1];
			else
				baseno_end[1]=MirPointI(no_end[1],relaObj.mirInfo);
		}
	}
	for(int i=0;i<2;i++)
	{
		if(baseno_start[i]>baseno_end[i])
		{
			int temp=baseno_start[i];
			baseno_start[i]=baseno_end[i];
			baseno_end[i]=temp;
		}
	}
	if(baseno_start[0]<baseno_start[1])
		return -1;
	else if(baseno_start[0]==baseno_start[1])
	{
		if(baseno_end[0]<baseno_end[1])
			return -1;
		else if(baseno_end[0]==baseno_end[1])
		{	//主号相同一般认为是互相对称的杆件
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
		else
			return 1;
	}
	else
		return 1;
}
//仅用于生成分析过程文件，不考虑对称项合并因素
int compare_func4(const ELEM_PTR &pElem1,const ELEM_PTR &pElem2)
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

BOOL CAnalyzeProcessor::CreateAnalyzeFile(char *file_name)
{
	char file_path[MAX_PATH];
	strcpy(file_path,file_name);
	strcat(file_path,".anl");
	FILE *fp=fopen(file_path,"wt");
	if(fp==NULL)
		return FALSE;
	long nNodeNum=nodeset.GetNodeNum();
	long nElemNum=hashElems.GetNodeNum();
	fprintf(fp,"\n");
	if(g_sysPara.AnalysisPara.m_iNonlinearStyle!=0)
	{
		fprintf(fp,"  No.  δx(mm)  δy(mm)  δz(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)  Fu (kN)\n");
		fprintf(fp,"---------------------------------------------------------------------------------------\n");
	}
	else
	{
		fprintf(fp,"  No.  δx(mm)  δy(mm)  δz(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)\n");
		fprintf(fp,"---------------------------------------------------------------------------------\n");
	}
	NODE_PTR *node_arr=new NODE_PTR[nNodeNum];
	int i=0;
	for(CLDSNode *pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		node_arr[i]=pNode;
		i++;
	}
	CQuickSort<NODE_PTR>::QuickSort(node_arr,nNodeNum,compare_func1);
	CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
	for(i=0;i<nNodeNum;i++)
	{
		f3dPoint offset;
		offset=node_arr[i]->m_offset;
		f3dPoint load;
		if(pStatus->hashNodeLoad.GetValue(node_arr[i]->handle))
			load=pStatus->hashNodeLoad[node_arr[i]->handle].Fxyz;
		f3dPoint WindLoad=node_arr[i]->WindLoad;
		WindLoad*=pStatus->Kesai()*pStatus->GamaQ();
		if(Ta.m_siSafetyGrade==0)
			WindLoad*=1.1;
		else if(Ta.m_siSafetyGrade==1)
			WindLoad*=1.0;
		else// if(Ta.m_siSafetyGrade==2)
			WindLoad*=0.9;
		if(g_sysPara.AnalysisPara.m_iNonlinearStyle!=0)
			fprintf(fp,"%5d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",node_arr[i]->point_i,offset.x,offset.y,offset.z,
				node_arr[i]->external_force.x/1000,node_arr[i]->external_force.y/1000,node_arr[i]->external_force.z/1000,
				WindLoad.x/1000,WindLoad.y/1000,node_arr[i]->unbalance_force.mod()/1000);
		else
			fprintf(fp,"%5d %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",node_arr[i]->point_i,offset.x,offset.y,offset.z,
				node_arr[i]->external_force.x/1000,node_arr[i]->external_force.y/1000,node_arr[i]->external_force.z/1000,
				WindLoad.x/1000,WindLoad.y/1000);
	}
	//杆件受力分析选材报告
	ELEM_PTR *elem_arr=new ELEM_PTR[nElemNum];
	i=0;
	for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		elem_arr[i]=pElem;
		i++;
	}
	CQuickSort<ELEM_PTR>::QuickSort(elem_arr,nElemNum,compare_func4);
	fprintf(fp,"\n");
	fprintf(fp,"   ELEMENT     SIZE         N(kN)  WEIGHT(kg)     M.X(N*m)    M.Y(N*m)   M.Z(N*m)\n");
	fprintf(fp,"---------------------------------------------------------------------------------\n");
	for(i=0;i<nElemNum;i++)
	{
		long No[2];
		double elem_N=elem_arr[i]->N;
		f3dPoint moment=elem_arr[i]->moment;
		if(elem_arr[i]->BearTens1Comp2==1||elem_arr[i]->BearTens1Comp2==2)
			elem_N=0;
		No[0]=min(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		No[1]=max(elem_arr[i]->pStart->point_i,elem_arr[i]->pEnd->point_i);
		if(elem_arr[i]->pOrgPole->size_idClassType==0)
		{
			fprintf(fp,"%4d-%4d    U%3.0fX%2.0f  %10.3f    %8.3f\n",No[0],No[1],
				elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
				elem_N/1000,elem_arr[i]->SelfWeight()/9.8);
		}
		else if(elem_arr[i]->pOrgPole->size_idClassType==CLS_CABLE)
		{
			double unitWeight=elem_arr[i]->pOrgPole->GetWireUnitWeight()*1e-6;	//kg/mm
			double length=DISTANCE(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false));
			fprintf(fp,"%4d-%4d    %s  %10.3f    %8.3f\n",No[0],No[1],elem_arr[i]->pOrgPole->GetSizeSpec().spec,
				elem_N/1000,unitWeight*length);
		}
		else if(strlen(elem_arr[i]->pOrgPole->GetBriefPartTypeMark())==1)
		{
			fprintf(fp,"%4d-%4d    %s%3.0fX%2.0f%C %10.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",No[0],No[1],
				elem_arr[i]->pOrgPole->GetBriefPartTypeMark(),elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
				elem_arr[i]->pOrgPole->cMaterial,elem_N/1000,elem_arr[i]->SelfWeight()/9.8,
				moment.x/1000,moment.y/1000,moment.z/1000);
		}
		else //if(strlen(elem_arr[i]->pOrgPole->GetBriefPartTypeMark())==2)
		{
			fprintf(fp,"%4d-%4d   %s%3.0fX%2.0f%C %10.3f    %8.3f    %8.3f    %8.3f    %8.3f\n",No[0],No[1],
				elem_arr[i]->pOrgPole->GetBriefPartTypeMark(),elem_arr[i]->pOrgPole->GetWidth(),elem_arr[i]->pOrgPole->GetThick(),
				elem_arr[i]->pOrgPole->cMaterial,elem_N/1000,elem_arr[i]->SelfWeight()/9.8,
				moment.x/1000,moment.y/1000,moment.z/1000);
		}
	}
	fclose(fp);
	delete []node_arr;
	delete []elem_arr;
	return TRUE;
}

void CElement::CalLength()
{
	lenVec=pEnd->Position(false)-pStart->Position(false);
	length=lenVec.mod();
	lenStdVec=lenVec/length;
}
//<-2.0直线定义有误;<0.0||>1.0:点不在线上或直线内;0.0:点与直线始端重合;<1.0点在直线内;1.0点与直线终端重合
double CElement::CheckPoint(const f3dPoint& pick)
{
	if(length<EPS)
		CalLength();
	double dd;	//判断因子
	if(length<EPS)
		return -2.0;	//直线定义有误
	if(fabs(lenVec.x)>=fabs(lenVec.y)&&fabs(lenVec.x)>=fabs(lenVec.z))
		dd=(pick.x-pStart->Position(false).x)/lenVec.x;
	else if(fabs(lenVec.y)>=fabs(lenVec.x)&&fabs(lenVec.y)>=fabs(lenVec.z))
		dd=(pick.y-pStart->Position(false).y)/lenVec.y;
	else
		dd=(pick.z-pStart->Position(false).z)/lenVec.z;
	if(dd>-eps&&dd<1+eps)
	{
		if(pStart->Position(false)==pick)
			return 0.0;
		else if(pEnd->Position(false)==pick)
			return 1.0;
		else	// 判断指定点pick是否与当前直线共线
		{
			f3dPoint verifyVec = pick-pStart->Position(false);
			double mod=verifyVec.mod();
			if(verifyVec*lenStdVec<mod*(1-eps))
				return -1.0;	//点不在直线段内
			else
				return dd;	//点在直线段内
		}
	}
	else
		return dd;	//点不在直线段内
}
double CElement::GetWidth()
{
	if(pParaType==NULL)
		return pOrgPole->GetWidth();
	else
	{
		if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			return ((JG_PARA_TYPE*)pParaType)->wing_wide;
		else if(pOrgPole->size_idClassType==CLS_LINETUBE)
			return ((TUBE_PARA_TYPE*)pParaType)->D;
		else if(pOrgPole->size_idClassType==CLS_LINEFLAT)
			return ((BIAN_PARA_TYPE*)pParaType)->wide;
		else if(pOrgPole->size_idClassType==CLS_LINESLOT)
			return ((SLOT_PARA_TYPE*)pParaType)->b;
		else
			return 0;
	}
}
char CElement::CalLenType()
{
	CElemInfo* pElemInfo=Ta.Elems.GetValue(pStart->handle,pEnd->handle);
	if(pElemInfo)
		return pElemInfo->cCalLenType;
	else if(pOrgPole)
		return (char)pOrgPole->CalLenCoef.iTypeNo;
	else
		return 0;
}
//是否在报告书中输出同时受压时的长细比(只有同时受压时长细比过载系数<承载系数时输出)
bool CElement::IsExportBothCompLamda(IMemberDesignCode* pDesignCode/*=NULL*/)
{
	//通过比较各规范,发现一般情况下压杆稳定mN一般在0.85~1.0之间,常用钢材设计强度最差也是200MPa故可取设计应力小于极端允许应力(0.85*200=170MPa)的6%(即10MPa),可不考虑同时受压 wjh-2017.5.30
	if(fabs(MaxCompStable.sigema)<10)
		return false;
	if(pDesignCode==NULL)
		pDesignCode=CPreProcessor::ActiveDesignCode;
	//为了保证输出长细比与同时受压时压稳系数一致,感觉还是在保证不以同时受压长细比为设计依据基础上输出同时受压长细比更合理 wjh-2017.5.30
	if(pOrgPole->IsAuxPole())	//交叉辅材
		return MaxCompStable.Lo_r_bothcomp<pDesignCode->GetBraceRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//Ta.SelMatPara.m_nXieTubeSlendernessLimit;
	else						//交叉斜材
		return MaxCompStable.Lo_r_bothcomp<pDesignCode->GetPrimaryRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//Ta.SelMatPara.m_nXieTubeSlendernessLimit;
	/*
	double limitOfLamda=pDesignCode->GetPrimaryRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);
	double lamdaBearCoef=MaxCompStable.Lo_r_bothcomp/limitOfLamda;
	//计算压杆稳定强度折减系数
	double mN=1.0;
	double t=GetThick();
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pOrgPole->cMaterial);
	double fy=pMatType->fy(t);
	double ft=pMatType->f(t);	//设计强度
	if(pOrgPole->IsAngle(TRUE))
	{
		double b=GetWidth()-t;
		if(pParaType)
			b-=((JG_PARA_TYPE*)pParaType)->r;
		mN = pDesignCode->CompReduceCoef(b,t,ft,fy,MaxCompStable.Lo_r_bothcomp,false,false);
	}
	else if(pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		double D0=GetWidth();
		mN = pDesignCode->CompReduceCoef(D0,t,ft,fy,MaxCompStable.Lo_r_bothcomp,true,false);//压杆稳定强度折减系数
	}
	double bearcoef = fabs(MaxCompStable.sigema)/(mN*ft);
	return lamdaBearCoef<bearcoef;*/
}
double CElement::LamdaByType(int lamdaType)
{
	if(lamdaType==0)
		return lamdaRyo.Lo_r;
	else if(lamdaType==1)
		return lamdaRx.Lo_r;
	else if(MaxCompStable.bothComp)
		return MaxCompStable.Lo_r_bothcomp;
	else
		return LamdaRc().Lo_r;
}
double CElement::CompStableLamda(bool* pbSpecBothCompState/*=NULL*/)
{
	BYTE lamdaType;
	bool bothComp=pbSpecBothCompState!=NULL?*pbSpecBothCompState:MaxCompStable.bothComp;
	if( bothComp)//&&
		//LamdaByType(MaxCompStable.lamdatype_bothcomp)>LamdaByType(MaxCompStable.lamdatype_tenscomp))
		lamdaType=MaxCompStable.lamdatype_bothcomp;
	else
		lamdaType=MaxCompStable.lamdatype_tenscomp;
	if(lamdaType==0)
		return lamdaRyo.Lo_r;
	else if(lamdaType==1)
		return lamdaRx.Lo_r;
	else if(MaxCompStable.bothComp)
		return MaxCompStable.Lo_r_bothcomp;
	else
		return LamdaRc().Lo_r;
}
void CElement::UpdateLamdaInfo()
{
	int lamdaSelType=0;
	int tensLamdaLim=Ta.DesignCode()->GetTensionRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//SelMatPara.m_nTensiveTubeSlendernessLimit;
	int mainLamdaLim=Ta.DesignCode()->GetPostRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//Ta.SelMatPara.m_nMainTubeSlendernessLimit;
	int xieLamdaLim =Ta.DesignCode()->GetPrimaryRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//Ta.SelMatPara.m_nXieTubeSlendernessLimit;
	int auxLamdaLim =Ta.DesignCode()->GetBraceRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//Ta.SelMatPara.m_nAuxTubeSlendernessLimit;
	//确定长细比最高允许限值
	if(MaxCompression.N>CElement::COMPRESSION_ZERO_FORCE)	//非压杆
		design_info.UltimateLo_r=tensLamdaLim;
	else if(pOrgPole->layer(1)=='Z')
		design_info.UltimateLo_r=mainLamdaLim;
	else if(!pOrgPole->IsAuxPole())
		design_info.UltimateLo_r=xieLamdaLim;
	else
		design_info.UltimateLo_r=auxLamdaLim;
	//确定长细比计算类型
	if(design_info.iDangerousRule==4)
	{	//压稳控制对长细比敏感，输出最危险工况下的长细比（不一定是长细比最大情况）
		if(MaxCompStable.bothComp&&IsExportBothCompLamda())
		{
			//《技术规定2002》P38规定交叉斜材或交于主材同一点相邻斜材为压杆时，容许长细比可取同辅助材
			//《技术规定2012》P37规定交于主材同一点相邻斜材为压杆时，容许长细比可取同辅助材
			BYTE cSMCType=ActiveSmcItem().cSMCType;
			if(CPreProcessor::ActiveDesignCode->CodeSerial()==CODES::DLT_2002)
				design_info.UltimateLo_r=auxLamdaLim;	//2002版技术规定允许交叉斜材同时受压取辅助材长细比限制 wjh-2017.5.30
			else if(CPreProcessor::ActiveDesignCode->CodeSerial()==CODES::DLT_2012 && 
				pOrgPole->layer(1)!='Z'&&!pOrgPole->IsAuxPole()&&(cSMCType<SMC_ITEM::SMC_REFER_START||cSMCType>SMC_ITEM::SMC_REFER_2ENDS))
				design_info.UltimateLo_r=auxLamdaLim;
			lamdaSelType=MaxCompStable.lamdatype_bothcomp;
		}
		else
			lamdaSelType=MaxCompStable.lamdatype_tenscomp;
	}
	else //其余控制工况不敏感，全取长细比裕度系数最大值输出
	{
		if(lamdaRyo.Lo_r>=lamdaRx.Lo_r)//LamdaRc().Lo_r)
			lamdaSelType=0;
		else //if(lamdaRx.Lo_r>=lamdaRyo.Lo_r)//LamdaRc().Lo_r)
			lamdaSelType=1;
	}
	SMC_ITEM* pSmcItem=NULL;
	LAMDA_STABLE_INFO* pLamdaItem=NULL;
	//更新要输出的长细比相关信息
	if(lamdaSelType==0)
	{	//lamdaRyo控制
		design_info.cLamdaTypeR=1;	//0:Rx;1:Ry0
		design_info.lamdaL=lamdaRyo.L;	//计算长度
		design_info.lamdaR=lamdaRyo.R;	//回转半径
		design_info.Lo_r=lamdaRyo.Lo_r;	//实际长细比
	}
	else if(lamdaSelType==1)
	{	//lamdaRx控制
		design_info.cLamdaTypeR=0;	//0:Rx;1:Ry0
		design_info.lamdaL=lamdaRx.L;	//计算长度
		design_info.lamdaR=lamdaRx.R;	//回转半径
		design_info.Lo_r=lamdaRx.Lo_r;	//实际长细比
	}
	else if(lamdaSelType==2)
	{	//lamdaRx2控制
		design_info.lamdaL=MaxCompStable.L_bothcomp;	//计算长度(含计算长度折算系数)
		design_info.Lo_r=MaxCompStable.Lo_r_bothcomp;	//实际长细比
		pSmcItem=&smcinfo.smcItems[smcinfo.iActiveItem];
		if(pSmcItem->cSMCType>=0)
			design_info.cLamdaTypeR=0;	//0:Rx;1:Ry0
		else
			design_info.cLamdaTypeR=1;
		pLamdaItem=&smcinfo.lamdaItems[smcinfo.iActiveItem];
		design_info.lamdaR=pLamdaItem->R;	//回转半径
	}
	//更新输出长细计算过程字符串
	CXhChar50 braceStr;
	//if(pOrgPole->layer(0)=='L'&&pOrgPole->layer(1)=='X'&&pOrgPole->CalLenCoef.iTypeNo!=5)
	//m_bLegVRod=true时的1.2增大系数已包含在design_info.lamdaL及design_info.Lo_r中,不能重复计入. wjh-2017.5.30
	double lenCoef= !pOrgPole->m_bLegVRod ? 1.0 : 1.2;
	if(lamdaSelType==0)	//lamdaRyo控制
	{
		lenCoef*=lamdaRyo.muCoefBasic;
		if(lenCoef!=1.0)
		{
			if(lamdaRyo.braceStr[1]=='E')
				design_info.LoExpression.Printf(" %s *%3.2f",(char*)lamdaRyo.braceStr,lenCoef);
			else
				design_info.LoExpression.Printf("(%s)*%3.2f",(char*)lamdaRyo.braceStr,lenCoef);
		}
		else
			design_info.LoExpression.Printf(" %s      ",(char*)lamdaRyo.braceStr);
	}
	else if(lamdaSelType==1)	//lamdaRx控制
	{
		lenCoef*=lamdaRx.muCoefBasic;
		if(lenCoef!=1.0)
		{
			if(lamdaRx.braceStr[1]=='E')
				design_info.LoExpression.Printf(" %s *%3.2f",(char*)lamdaRx.braceStr,lenCoef);
			else
				design_info.LoExpression.Printf("(%s)*%3.2f",(char*)lamdaRx.braceStr,lenCoef);
		}
		else
			design_info.LoExpression.Printf(" %s      ",(char*)lamdaRx.braceStr);
	}
	else				//lamdaRx2控制
	{
		double whole_L=0;
		if(pLamdaItem->pStart&&pLamdaItem->pEnd)
			whole_L=DISTANCE(pLamdaItem->pStart->Position(false),pLamdaItem->pEnd->Position(false));
		else
			whole_L=Length();
		lenCoef=design_info.lamdaL/whole_L;
		//lenCoef*=LamdaRc().MuCoef();
		if(lenCoef!=1.0)
		{
			if(pLamdaItem->braceStr[1]=='E')
				design_info.LoExpression.Printf(" %s *%3.2f",(char*)pLamdaItem->braceStr,lenCoef);
			else
				design_info.LoExpression.Printf("(%s)*%3.2f",(char*)pLamdaItem->braceStr,lenCoef);
		}
		else
			design_info.LoExpression.Printf(" %s      ",(char*)pLamdaItem->braceStr);
	}
	if(design_info.LoExpression[design_info.LoExpression.Length-1]=='0')
		design_info.LoExpression[design_info.LoExpression.Length-1]=' ';
	if(CPreProcessor::ActiveDesignCode->IsIncLamdaK())
		design_info.lamdaExpression.Copy("KL");
	else
		design_info.lamdaExpression.Copy("L");
	if(pOrgPole->size_idClassType==CLS_LINETUBE)
		design_info.lamdaExpression.Append("/R");	//钢管不分平行轴最小轴
	else if(lamdaSelType!=0)
		design_info.lamdaExpression.Append("/Rx");	//平行轴计算
	else if(lamdaSelType==0)
		design_info.lamdaExpression.Append("/Ryo");	//最小轴计算
}

double CElement::GetThick()
{
	if(pParaType==NULL)
		return pOrgPole->GetThick();
	else
	{
		if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			return ((JG_PARA_TYPE*)pParaType)->wing_thick;
		else if(pOrgPole->size_idClassType==CLS_LINETUBE)
			return ((TUBE_PARA_TYPE*)pParaType)->thick;
		else if(pOrgPole->size_idClassType==CLS_LINEFLAT)
			return ((BIAN_PARA_TYPE*)pParaType)->thick;
		else if(pOrgPole->size_idClassType==CLS_LINESLOT)
			return ((SLOT_PARA_TYPE*)pParaType)->t;
		else
			return 0;
	}
}
//<DEVELOP_PROCESS_MARK nameId="CAnalyzeProcessor::UpdateInfoAtModuleChange">
//一个计算模型的所有工况计算过程分析完毕后调用，主要用于计算压杆两端是否有足够的转动约束
void CAnalyzeProcessor::UpdateInfoAtModuleChange(CLDSModule *pModule)
{
	if(m_pPostProcessor==NULL)
		return;	//无后处理无法进行下一步工作，只能退出
	for(CElement* pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)	//零杆
			continue;
		CElement* pPostElem=NULL;
		if((pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->id))||
		   (pElem->relaElem.idElemMirX>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirX)))||
		   (pElem->relaElem.idElemMirY>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirY)))||
		   (pElem->relaElem.idElemMirZ>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirZ))))
		   ;
		else //if(pPostElem==NULL)
			continue;	//在合并对称项时可能会有某些当前计算过程单元并不在后处理单元中
		//计算是否可提供足够的转动约束刚度
		if(  pElem->pOrgPole->IsAuxPole()||	//辅材或非主材压杆单元
			(toupper(pElem->pOrgPole->layer(1))!='Z'&&pPostElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE))	//
		{	//未处理过的新单元
			//计算受压杆是否有合法的刚性约束
			f3dPoint base_vec;
			if(pElem->start_joint_type==JOINT_RIGID)
				pPostElem->m_bValidStartRestStiff=TRUE;
			else if(pElem->start_joint_type==JOINT_HINGE)
				pPostElem->m_bValidStartRestStiff=FALSE;
			else if(pElem->start_joint_type==JOINT_AUTOJUSTIFY&&(pPostElem->pStart!=pPostElem->pOrgPole->pStart||pPostElem->pOrgPole->connectStart.wnConnBoltN>=2))
			{	//此杆件需要其它非共线父角钢单元提供约束
				CNodeLinkElems *pNodeLinks=m_pPreProcessor->result.hashNodeLinks.GetValue(pElem->pStart->handle);
				double support_stiff=0;
				double rest_stiff=pElem->GetWorkI()/pElem->Length();
				if(pNodeLinks)
				{
					CLDSLinePart *pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pElem->pStart->hFatherPart);
					if(pFatherRod&&pFatherRod->IsLinePart())
						base_vec=pFatherRod->pEnd->Position(false)-pFatherRod->pStart->Position(false);
					if(pFatherRod&&pFatherRod->IsLinePart())
					{
						normalize(base_vec);
						f3dPoint work_norm=pElem->LenStdVec()^base_vec;
						normalize(work_norm);
						for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
						{
							if(pElemStiff->pElem->id==pElem->id)
								continue;
							else if(hashElems.GetValue(pElemStiff->pElem->id))
							{	//当前计算模型内的单元
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pStart||pElemStiff->pElem->pEnd==pElem->pStart)
										support_stiff+=pElemStiff->stiff;	//判断此单元是否为当前模型中单元
									else
										support_stiff+=pElemStiff->stiff*4;	//节点在单元中间支撑刚度乘4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//提供约束的单元
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//工作面内被约束的单元
									rest_stiff+=pElemStiff->stiff;
							}
						}
					}
				}
				if(support_stiff>=rest_stiff)
					pPostElem->m_bValidStartRestStiff=TRUE;
				else
					pPostElem->m_bValidStartRestStiff=FALSE;
			}
			else
				pPostElem->m_bValidStartRestStiff=FALSE;
			if(pElem->end_joint_type==JOINT_RIGID)
				pPostElem->m_bValidEndRestStiff=TRUE;
			else if(pElem->end_joint_type==JOINT_HINGE)
				pPostElem->m_bValidEndRestStiff=FALSE;
			else if(pElem->end_joint_type==JOINT_AUTOJUSTIFY&&(pElem->pEnd!=pPostElem->pOrgPole->pEnd||pPostElem->pOrgPole->connectEnd.wnConnBoltN>=2))
			{	//此杆件需要其它非共线父角钢单元提供约束
				CNodeLinkElems *pNodeLinks=m_pPreProcessor->result.hashNodeLinks.GetValue(pElem->pEnd->handle);
				double support_stiff=0;
				double rest_stiff=0;
				if(pNodeLinks)
				{
					CLDSLinePart *pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pElem->pEnd->hFatherPart);
					if(pFatherRod&&pFatherRod->IsLinePart())
						base_vec=pFatherRod->pEnd->Position(false)-pFatherRod->pStart->Position(false);
					if(pFatherRod&&pFatherRod->IsLinePart())
					{
						normalize(base_vec);
						f3dPoint work_norm=pElem->LenStdVec()^base_vec;
						normalize(work_norm);
						for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
						{
							if(pElemStiff->pElem->id==pElem->id)
								continue;
							else if(hashElems.GetValue(pElemStiff->pElem->id))
							{	//当前计算模型内的单元
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pEnd||pElemStiff->pElem->pEnd==pElem->pEnd)
										support_stiff+=pElemStiff->stiff;	//判断此单元是否为当前模型中单元
									else
										support_stiff+=pElemStiff->stiff*4;	//节点在单元中间支撑刚度乘4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//提供约束的单元
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//工作面内被约束的单元
									rest_stiff+=pElemStiff->stiff;
							}
						}
					}
				}
				if(support_stiff>=rest_stiff)
					pPostElem->m_bValidEndRestStiff=TRUE;
				else
					pPostElem->m_bValidEndRestStiff=FALSE;
			}
			else
				pPostElem->m_bValidEndRestStiff=FALSE;
		}
	}
}
//</DEVELOP_PROCESS_MARK>
//from PreProcessor.cpp, lamdaType=0:ladmaRyo;1:lamdaRx;2:lamdaRc
double CalKlamda(CElement *pElem,double lamda,CLDSNode* pLoStart,CLDSNode* pLoEnd,bool bIncActualCompForce=true);
void CAnalyzeProcessor::CalElemCompStableLiveSMCInfo(CElement *pElem)
{
	//if(pElem->pOrgPole->IsAuxPole())
	//	return;	//暂不确定是否需要考虑交叉辅材同时受压情况 wjh-2017.2.4
	char sError[100]="";
	IMemberDesignCode::BevelRodLamdaPara lamdaPara;
	char sectionType='B';
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE&&pElem->pOrgPole->size_wide<g_sysPara.fTubeWeldRoadMinD)
		sectionType='A';
	double area;	//单位均为mm级
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pParaType==NULL)
			pElem->pParaType=pParaType=FindTubeType(pElem->GetWidth(),pElem->GetThick());
		if(pParaType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X杆件的规格φ%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			throw sError;
		}
		area=pParaType->area*100;
	}
	else
	{
		JG_PARA_TYPE *pParaType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pParaType==NULL)
			pElem->pParaType=pParaType=FindJgType(pElem->GetWidth(),pElem->GetThick(),pElem->pOrgPole->size_cSubClassType);
		if(pParaType==NULL)
		{
			if(pElem->pOrgPole->size_cSubClassType=='L')
				sprintf(sError,"规格库中找不到0X%X杆件的规格L%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			else
				sprintf(sError,"规格库中找不到0X%X杆件的规格%CL%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_cSubClassType,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			throw sError;
		}
		area=pParaType->area*100;
	}
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
	for(int i=0;i<3;i++)
	{
		if(pElem->smcinfo.smcItems[i].cSMCType==SMC_ITEM::SMC_NONE)
			continue;
		pElem->smcinfo.lamdaItems[i].bothCompLive=false;
		CElement* pRefElem=NULL;
		ELEM_PTR* ppElem=m_pPreProcessor->result.hashElemsMap.GetValue(pElem->smcinfo.smcItems[i].relaElem.hStart,pElem->smcinfo.smcItems[i].relaElem.hEnd);
		//if(pElem->relaElem.idRefElem>0)
		if(ppElem!=NULL)
			pRefElem=hashElems.GetValue((*ppElem)->id);
		if(pRefElem==NULL)
			continue;
		if(pElem->smcinfo.smcItems[i].ratioL2toL3<=0||pElem->smcinfo.smcItems[i].ratioL2toL3>=1)
		{	//非交叉斜材同时受压单元，无需计算动态系数
			pElem->smcinfo.lamdaItems[i].muCoefLive=1.0;
			if(pElem->N<0&&pRefElem->N<fabs(CElement::COMPRESSION_ZERO_FORCE))	//参照单元同时受压工况（常见交叉材）
				pElem->smcinfo.lamdaItems[i].bothCompLive=true;//按同时受压考虑bothCompLive
			continue;
		}
		IMemberDesignCode::BevelRodLamdaPara lamdaPara=
			CPreProcessor::ActiveDesignCode->IntersBevelRodLamdaCal(pElem->N,pRefElem->N,pElem->smcinfo.smcItems[i].ratioL2toL3);
		if(lamdaPara.cType!=0)
			pElem->smcinfo.lamdaItems[i].bothCompLive=true;
		else
		{
			pElem->smcinfo.lamdaItems[i].muCoefLive=1.0;
			continue;
		}
		//按同时受压考虑，计入动态长细比修正系数
		pElem->smcinfo.lamdaItems[i].muCoefLive=lamdaPara.K/0.8;
		//pElem->smcinfo.lamdaItems[i].muCoefBasic=raiseCoef*pElem->smcinfo.smcItems[i].LoRc.coef;
		//计入动态长细比修正系数后的长细比标准值
		if(pElem->smcinfo.lamdaItems[i].pStart&&pElem->smcinfo.lamdaItems[i].pEnd)
			pElem->smcinfo.lamdaItems[i].L=DISTANCE(pElem->smcinfo.lamdaItems[i].pStart->Position(false),pElem->smcinfo.lamdaItems[i].pEnd->Position(false));
		else
			pElem->smcinfo.lamdaItems[i].L=pElem->Length();
		pElem->smcinfo.lamdaItems[i].L*=pElem->smcinfo.lamdaItems[i].muCoefBasic*pElem->smcinfo.lamdaItems[i].muCoefLive;
		double Lo_r=pElem->smcinfo.lamdaItems[i].L/pElem->smcinfo.lamdaItems[i].R;
		pElem->smcinfo.lamdaItems[i].Lo_r=CalKlamda(pElem,Lo_r,pElem->smcinfo.lamdaItems[i].pStart,pElem->smcinfo.lamdaItems[i].pEnd,false);
		pElem->smcinfo.lamdaItems[i].coefFai=CPreProcessor::ActiveDesignCode->CompStableCoef(sectionType,pElem->smcinfo.lamdaItems[i].Lo_r,pMatType,pElem->GetThick());
		pElem->smcinfo.lamdaItems[i].coefAreaFai=area*pElem->smcinfo.lamdaItems[i].coefFai;
		/*if((pElem->smcinfo.lamdaItems[i].pStart==pElem->pStart&&pElem->smcinfo.lamdaItems[i].pEnd==pElem->pEnd)||
			(pElem->smcinfo.lamdaItems[i].pStart==pElem->pEnd&&pElem->smcinfo.lamdaItems[i].pEnd==pElem->pStart))
			pElem->smcinfo.lamdaItems[i].braceStr.Copy(" ELEM_LEN");
		else if(pElem->smcinfo.lamdaItems[i].pStart&&pElem->smcinfo.lamdaItems[i].pEnd)
		{
			double L=DISTANCE(pElem->smcinfo.lamdaItems[i].pStart->Position(false),pElem->smcinfo.lamdaItems[i].pEnd->Position(false));
			double coef=L/pElem->Length();
			CXhChar16 expr_str;
			CExpression::ToExpression(coef,3,expr_str,17);
			if(strchr(expr_str,'.')!=NULL)	//与单元长度比值为不规则小数
			{
				int min_i=min(pElem->lamdaRc.pStart->point_i,pElem->lamdaRc.pEnd->point_i);
				int max_i=max(pElem->lamdaRc.pStart->point_i,pElem->lamdaRc.pEnd->point_i);
				pElem->smcinfo.lamdaItems[i].braceStr.Printf("%4d~%4d",min_i,max_i);
			}
			else
			{
				pElem->smcinfo.lamdaItems[i].braceStr.Printf("ELEM*%s",expr_str);
				pElem->smcinfo.lamdaItems[i].braceStr.ResizeLength(9,true);
			}
		}*/
	}
}
//<DEVELOP_PROCESS_MARK nameId="CAnalyzeProcessor::UpdateInfoAtStatusChange">
//一个计算模型的当前工况计算过程分析完毕后调用
void CAnalyzeProcessor::UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus)
{
	int i;
	CElement *pElem,*pPostElem,*pRefElem;
	for(pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)	//拉线单元或零杆
			continue;
		//pElem->lamdaRc.ClearZero();		//初始清零同时受压信息
		for(i=0;i<3;i++)
		{	//为保持与原有输出报告一致，即使无同时受压情况出现也用第一个有效同时受压检测项进行赋值
			if(pElem->smcinfo.smcItems[i].cSMCType!=SMC_ITEM::SMC_NONE)
			{
				//pElem->lamdaRc=pElem->smcinfo.lamdaItems[i];
				//下一行代码已在CPreProcessor::CalElemCompStableInfo中更新 wjh-2013.9.22
				//pElem->lamdaRc.coef=pElem->smcinfo.smcItems[i].LoRc.coef;
				pElem->MaxCompStable.Afai_bothcomp=pElem->smcinfo.lamdaItems[i].coefAreaFai;
				pElem->MaxCompStable.fai_bothcomp=pElem->smcinfo.lamdaItems[i].coefFai;
				//pElem->MaxCompStable.Lo_r_bothcomp=pElem->LamdaRc().Lo_r;
				//pElem->MaxCompStable.L_bothcomp=pElem->LamdaRc().L;
				//pElem->MaxCompStable.lamdatype_bothcomp
				//TODO: 原代码有问题
				//if(pElem->smcinfo.smcItems[i].cSMCType>0)
					pElem->MaxCompStable.lamdatype_bothcomp=2;
				//else
				//	pElem->MaxCompStable.lamdatype_bothcomp=0;
				break;
			}
		}
		if(pElem->N<fabs(CElement::COMPRESSION_ZERO_FORCE))
		{	//安全起见近似零杆也视为压杆，用于进行长细比阈值验算
			double sigema=0;
			bool hasTensComp=false;
			pElem->MaxCompStable.sigema=0;	//初始清零默认值
			CalElemCompStableLiveSMCInfo(pElem);
			for(i=0;i<3;i++)
			{
				if(pElem->smcinfo.smcItems[i].cSMCType==SMC_ITEM::SMC_NONE)
					continue;	//无效项
				ELEM_PTR* ppElem=m_pPreProcessor->result.hashElemsMap.GetValue(pElem->smcinfo.smcItems[i].relaElem.hStart,pElem->smcinfo.smcItems[i].relaElem.hEnd);
				//if(pElem->relaElem.idRefElem>0)
				if(ppElem!=NULL)
					pRefElem=hashElems.GetValue((*ppElem)->id);
				else
					pRefElem=NULL;
				//if(lamdaPara.cType=='C')	//按同时受压考虑
				//if(pRefElem&&pRefElem->N<fabs(CElement::COMPRESSION_ZERO_FORCE))	//参照单元同时受压工况（常见交叉材）
				if(pElem->smcinfo.lamdaItems[i].bothCompLive)
				{	//安全起见近似零杆也视为压杆，用于进行长细比阈值验算
					sigema=pElem->N/pElem->smcinfo.lamdaItems[i].coefAreaFai;
					pElem->smcinfo.smcItems[i].enabled=true;
					if(fabs(sigema)>fabs(pElem->MaxCompStable.sigema))
					{
						pElem->MaxCompStable.sigema=sigema;
						pElem->MaxCompStable.Afai_bothcomp=pElem->smcinfo.lamdaItems[i].coefAreaFai;
						pElem->MaxCompStable.fai_bothcomp=pElem->smcinfo.lamdaItems[i].coefFai;
						//已归并到pElem->MaxCompStable.bothComp=true中 wjh 2013.9.22
						//pElem->MaxCompStable.fai=pElem->MaxCompStable.fai_bothcomp;
						pElem->MaxCompStable.bothComp=true;
						pElem->smcinfo.hasBothComp=true;	//其默认值
						pElem->MaxCompStable.L_bothcomp=pElem->smcinfo.lamdaItems[i].L;
						pElem->MaxCompStable.Lo_r_bothcomp=pElem->smcinfo.lamdaItems[i].Lo_r;
						pElem->MaxCompStable.muCoefL_bothcomp=pElem->smcinfo.lamdaItems[i].muCoefBasic*pElem->smcinfo.lamdaItems[i].muCoefLive;
						//pElem->MaxCompStable.cActiveSMCType=pElem->smcinfo.smcItems[i].cSMCType;
						//只是为代替上面一行获取当前有效的回转半径信息 wjh-2013-5-31
						pElem->smcinfo.iActiveItem=i;
						//pElem->lamdaRc=pElem->smcinfo.lamdaItems[i];
						//下一行代码已在CPreProcessor::CalElemCompStableInfo中更新 wjh-2013.9.22
						//pElem->lamdaRc.coef=pElem->smcinfo.smcItems[i].LoRc.coef;
					}
				}
				else
					hasTensComp=true;
			}
			if(pElem->MaxCompStable.Afai_tenscomp>0&&(hasTensComp||pElem->MaxCompStable.sigema==0))
			{	//只要系统设定了无同时受压情况时的长细比参数（LoRy0,LoRx)值就需要考虑非同时受压情况出现）
				sigema=pElem->N/pElem->MaxCompStable.Afai_tenscomp;
				if(fabs(sigema)>fabs(pElem->MaxCompStable.sigema))
				{
					pElem->MaxCompStable.sigema=sigema;
					//已归并到pElem->MaxCompStable.bothComp=false中 wjh 2013.9.22
					//pElem->MaxCompStable.fai=pElem->MaxCompStable.fai_tenscomp;
					pElem->MaxCompStable.bothComp=false;
				}
			}
		}
		pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->id);
		if(g_sysPara.bMergeRptFileMirObj&&pPostElem==NULL)
		{
			if((pElem->relaElem.idElemMirX>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirX)))||
			   (pElem->relaElem.idElemMirY>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirY)))||
			   (pElem->relaElem.idElemMirZ>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirZ))))
			{	//是否在与后处理器中单元有正确的关联关系
				if(pPostElem->CompareElemPointI(pElem)>0)
				{	//新单元始末端节点号更小，则在输出报告中显示更小节点号的对称单元
					//不更新可能导致校审窗口RevisionOperDlg操作异常 wjh-2017.2.4
					pPostElem=m_pPostProcessor->hashElems.ModifyKeyId(pPostElem->id,pElem->id);
					pPostElem->id=pElem->id;	//必须更新id否则会出现节点号与id不一致导致输出错误
					pPostElem->pStart=pElem->pStart;
					pPostElem->pEnd=pElem->pEnd;
					pPostElem->relaElem=pElem->relaElem;
					pPostElem->pOrgPole=pElem->pOrgPole;
					pPostElem->design_info=pElem->design_info;
					pPostElem->start_joint_type=pElem->start_joint_type;
					pPostElem->end_joint_type=pElem->end_joint_type;
					pPostElem->real_start_joint_type=pElem->real_start_joint_type;
					pPostElem->real_end_joint_type=pElem->real_end_joint_type;
					pPostElem->start_force_type=pElem->start_force_type;
					pPostElem->end_force_type=pElem->end_force_type;
					pPostElem->lamdaRyo=pElem->lamdaRyo;
					pPostElem->lamdaRx=pElem->lamdaRx;
					//pPostElem->lamdaRc=pElem->lamdaRc;
					pPostElem->smcinfo=pElem->smcinfo;
				}
				m_pPostProcessor->hashRodRelaElem.SetValue(pElem->pOrgPole->handle,pPostElem);
			}
		}
		if(pPostElem!=NULL)
		{
			if(pElem->N-pPostElem->MaxTension.N>CElement::DIFFERENCE_ZERO_FORCE)
			{	//出现更大拉力
				pPostElem->MaxTension.N=pElem->N;
				pPostElem->MaxTension.hModule=pModule->handle;
				pPostElem->MaxTension.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				for(int iLeg=0;iLeg<4;iLeg++)
					pPostElem->MaxTension.leg_no[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
			}
			else if(pElem->N-pPostElem->MaxCompression.N<-CElement::DIFFERENCE_ZERO_FORCE)
			{	//出现更大压力
				pPostElem->MaxCompression.N=pElem->N;
				pPostElem->MaxCompression.bothComp=pElem->MaxCompStable.bothComp;	//最大压力时肯定会验算压稳，故其当前工况下同时受压状态与压稳一致 wjh-2017.2.4
				pPostElem->MaxCompression.hModule=pModule->handle;
				pPostElem->MaxCompression.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				for(int iLeg=0;iLeg<4;iLeg++)
					pPostElem->MaxCompression.leg_no[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
			}
			if(!pElem->moment.IsZero())
			{
				double N=pElem->moment.mod();
				if(N>pPostElem->MaxMoment.N)
				{
					pPostElem->MaxMoment.N=N;
					pPostElem->MaxMoment.hModule=pModule->handle;
					pPostElem->MaxMoment.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
					for(int iLeg=0;iLeg<4;iLeg++)
						pPostElem->MaxMoment.leg_no[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
				}
			}
			//注意由于长细比带来的压稳系数因素，压稳计算应力最大时不一定压力最大
			if(pElem->MaxCompStable.sigema-pPostElem->MaxCompStable.sigema<-CElement::DIFFERENCE_ZERO_STRESS)
			{//判断是否当前单元当前工况压稳计算应力是否比原记录中的绝对值更大
				//TODO:注意此时可能把原来的hasBothComp属性冲掉，下一步应避免这样直接赋值
				double fai_bothcomp=pPostElem->MaxCompStable.fai_bothcomp;
				double afai_bothcomp=pPostElem->MaxCompStable.Afai_bothcomp;
				double L_bothcomp=pPostElem->MaxCompStable.L_bothcomp;
				double Lo_r_bothcomp=pPostElem->MaxCompStable.Lo_r_bothcomp;
				double muCoefL_bothcomp=pPostElem->MaxCompStable.muCoefL_bothcomp;
				pPostElem->MaxCompStable=pElem->MaxCompStable;
				if(!pElem->MaxCompStable.bothComp)
				{	//TODO:只是因为临时避免原始数据中对称杆件的同时受压信息不同(部分缺失)导致输出有误
					pPostElem->MaxCompStable.fai_bothcomp=fai_bothcomp;
					pPostElem->MaxCompStable.Afai_bothcomp=afai_bothcomp;
					pPostElem->MaxCompStable.L_bothcomp=L_bothcomp;
					pPostElem->MaxCompStable.Lo_r_bothcomp=Lo_r_bothcomp;
					pPostElem->MaxCompStable.muCoefL_bothcomp=muCoefL_bothcomp;
				}
				pPostElem->MaxCompStable.bEffectN=pElem->bEffectN;
				pPostElem->MaxCompStable.N=pElem->N;
				pPostElem->MaxCompStable.hModule=pModule->handle;
				pPostElem->MaxCompStable.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				for(int iLeg=0;iLeg<4;iLeg++)
					pPostElem->MaxCompStable.leg_no[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
			}
			if(pElem->smcinfo.hasBothComp&&!pPostElem->smcinfo.hasBothComp)
			{
				pPostElem->smcinfo.hasBothComp=true;
				pPostElem->MaxCompStable.fai_bothcomp=pElem->MaxCompStable.fai_bothcomp;
				pPostElem->MaxCompStable.Afai_bothcomp=pElem->MaxCompStable.Afai_bothcomp;
				pPostElem->MaxCompStable.lamdatype_bothcomp=pElem->MaxCompStable.lamdatype_bothcomp;
				pPostElem->MaxCompStable.L_bothcomp=pElem->MaxCompStable.L_bothcomp;
				pPostElem->MaxCompStable.Lo_r_bothcomp=pElem->MaxCompStable.Lo_r_bothcomp;
				pPostElem->MaxCompStable.muCoefL_bothcomp=pElem->MaxCompStable.muCoefL_bothcomp;
			}
		}
		else //if(pPostElem==NULL)	//新计算杆件单元，进行注册
		{
			pPostElem=m_pPostProcessor->hashElems.Add(pElem->id);
			pPostElem->id=pElem->id;
			pPostElem->pStart=pElem->pStart;
			pPostElem->pEnd=pElem->pEnd;
			pPostElem->relaElem=pElem->relaElem;
			pPostElem->pOrgPole=pElem->pOrgPole;
			pPostElem->CalLength();
			pPostElem->lamdaRyo=pElem->lamdaRyo;
			pPostElem->lamdaRx=pElem->lamdaRx;
			//pPostElem->lamdaRc=pElem->lamdaRc;
			pPostElem->smcinfo=pElem->smcinfo;
			m_pPostProcessor->hashRodRelaElem.SetValue(pElem->pOrgPole->handle,pPostElem);
			pPostElem->start_force_type=pElem->start_force_type;
			pPostElem->end_force_type=pElem->end_force_type;
			pPostElem->start_joint_type=pElem->start_joint_type;
			pPostElem->end_joint_type=pElem->end_joint_type;
			pPostElem->real_start_joint_type=pElem->real_start_joint_type;
			pPostElem->real_end_joint_type=pElem->real_end_joint_type;
			pPostElem->design_info=pElem->design_info;
			if(pPostElem->start_joint_type==JOINT_RIGID)
				pPostElem->m_bValidStartRestStiff=TRUE;
			if(pPostElem->end_joint_type==JOINT_RIGID)
				pPostElem->m_bValidEndRestStiff=TRUE;
			pPostElem->MaxCompStable.lamdatype_bothcomp=pElem->MaxCompStable.lamdatype_bothcomp;
			//pPostElem->MaxCompStable.cActiveSMCType=pElem->MaxCompStable.cActiveSMCType;
			//只是为代替上面一行获取当前有效的回转半径信息 wjh-2013-5-31
			pPostElem->smcinfo.iActiveItem=pElem->smcinfo.iActiveItem;
			pPostElem->smcinfo.hasBothComp=pElem->smcinfo.hasBothComp;
			pPostElem->MaxCompStable.Afai_bothcomp=pElem->LamdaRc().coefAreaFai;
			pPostElem->MaxCompStable.fai_bothcomp=pElem->LamdaRc().coefFai;
			pPostElem->MaxCompStable.L_bothcomp=pElem->MaxCompStable.L_bothcomp;
			pPostElem->MaxCompStable.Lo_r_bothcomp=pElem->MaxCompStable.Lo_r_bothcomp;
			pPostElem->MaxCompStable.muCoefL_bothcomp=pElem->MaxCompStable.muCoefL_bothcomp;
			if(!pElem->moment.IsZero())
			{	//有弯矩项
				pPostElem->MaxMoment.N=pElem->moment.mod();
				pPostElem->MaxMoment.hModule=pModule->handle;
				pPostElem->MaxMoment.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				for(int iLeg=0;iLeg<4;iLeg++)
					pPostElem->MaxMoment.leg_no[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
			}
			if(pElem->N>0)
			{
				pPostElem->MaxTension.N=pElem->N;
				pPostElem->MaxTension.hModule=pModule->handle;
				pPostElem->MaxTension.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				for(int iLeg=0;iLeg<4;iLeg++)
					pPostElem->MaxTension.leg_no[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
			}
			else
			{
				//压稳中同时受压工况
				pPostElem->MaxCompStable=pElem->MaxCompStable;
				pPostElem->MaxCompStable.bEffectN=pPostElem->MaxCompression.bEffectN=pElem->bEffectN;
				pPostElem->MaxCompStable.N=pPostElem->MaxCompression.N=pElem->N;
				pPostElem->MaxCompStable.hModule=pPostElem->MaxCompression.hModule=pModule->handle;
				//以前漏掉下一行代码，导致如果第一次出现压稳工况为最终压稳控制工况时会缺失压稳工况号 wjh-2013.9.20
				pPostElem->MaxCompStable.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				pPostElem->MaxCompression.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				for(int iLeg=0;iLeg<4;iLeg++)
				{
					pPostElem->MaxCompStable.leg_no[iLeg]=pPostElem->MaxCompression.leg_no[iLeg]
						=pModule->m_arrActiveQuadLegNo[iLeg];
				}
			}
		}
	}
}
//</DEVELOP_PROCESS_MARK>
void CAnalyzeProcessor::UpdateElemRelaRodDesignForce()
{
	if(m_pPreProcessor==NULL||m_pPostProcessor==NULL)
		return;
	for(CElement *pElem=m_pPreProcessor->result.elemList.GetFirst();pElem;pElem=m_pPreProcessor->result.elemList.GetNext())
	{
		CElement *pPostElem=NULL;
		if(pElem->pStart==pElem->pOrgPole->pStart||pElem->pEnd==pElem->pOrgPole->pStart)
		{	//杆件始端
			if((pElem->relaElem.idElemMirX>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirX)))||
			   (pElem->relaElem.idElemMirY>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirY)))||
			   (pElem->relaElem.idElemMirZ>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirZ)))||
			   (pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->id)))
			{
				if(pPostElem->MaxTension.N>pElem->pOrgPole->connectStart.maxTension)
					pElem->pOrgPole->connectStart.maxTension=pPostElem->MaxTension.N;
				if(fabs(pPostElem->MaxCompression.N)>fabs(pElem->pOrgPole->connectStart.maxCompression))
					pElem->pOrgPole->connectStart.maxCompression=-fabs(pPostElem->MaxCompression.N);
				if(pPostElem->MaxMoment.N>pElem->pOrgPole->connectStart.maxMoment)
					pElem->pOrgPole->connectStart.maxMoment=pPostElem->MaxMoment.N;
			}
		}
		else if(pElem->pStart==pElem->pOrgPole->pEnd||pElem->pEnd==pElem->pOrgPole->pEnd)
		{	//杆件始端
			if((pElem->relaElem.idElemMirX>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirX)))||
			   (pElem->relaElem.idElemMirY>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirY)))||
			   (pElem->relaElem.idElemMirZ>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirZ)))||
			   (pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->id)))
			{
				if(pPostElem->MaxTension.N>pElem->pOrgPole->connectEnd.maxTension)
					pElem->pOrgPole->connectEnd.maxTension=pPostElem->MaxTension.N;
				if(fabs(pPostElem->MaxCompression.N)>fabs(pElem->pOrgPole->connectEnd.maxCompression))
					pElem->pOrgPole->connectEnd.maxCompression=-fabs(pPostElem->MaxCompression.N);
				if(pPostElem->MaxMoment.N>pElem->pOrgPole->connectEnd.maxMoment)
					pElem->pOrgPole->connectEnd.maxMoment=pPostElem->MaxMoment.N;
			}
		}
	}
}
FEM_NODE_PTR ConnectFemNode(void* pContext,long pointI)
{
	CAnalyzeProcessor* processor=(CAnalyzeProcessor*)pContext;
	if(processor==NULL)
		processor=&analysis;
	FEM_NODE_PTR* ppFemNode=processor->hashFemNodes.GetValue(pointI);
	if(ppFemNode)
		return *ppFemNode;
	else
		return NULL;
}
FEM_ELEMENT_PTR ConnectFemElement(void* pContext,long idElem)
{
	CAnalyzeProcessor* processor=(CAnalyzeProcessor*)pContext;
	if(processor==NULL)
		processor=&analysis;
	CElement *pElem=processor->hashElems.GetValue(idElem);
	if(pElem)
		return pElem->pFemElem;
	else
		return NULL;
}
#endif
