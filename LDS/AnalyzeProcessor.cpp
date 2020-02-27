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
{	//Lo_r>0�ű�ʾ�ó�ϸ�ȼ����б�����Ч
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
	elem_type = 0;	//Ĭ��Ϊ�����˻򾭵���
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
	//������ʼֵ�ᵼ�²�����������ʼ��Ϊfalse���յ����޷���ʼ����Ԫ������ wjh-2017.2.25
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
	elem_type = element.elem_type;	//Ĭ��Ϊ�����˻򾭵���
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
//���Ƴ�CElemInfo::relaElem����ʶ֮���ԭʼ�����Ϣ��ԭʼ��Ƶ�Ԫ��
void CElement::CopyBasicInfoTo(CElemInfo* pElemInfo,bool copyCalLenInfo/*=true*/)
{
	pElemInfo->m_hStartNode=pStart->handle;
	pElemInfo->m_hEndNode=pEnd->handle;
	pElemInfo->m_hParentRod=pOrgPole->handle;
	//ʵʱ���ɶ���ʵʱ����ʱ���£���Ӧ�ڴ˴����£���Ϊ��ʱ��CElement�п���δ����ʵʱ���ɶ�) wjh-2017.7.14
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
		//TODO:pElemInfo->horiElem��δ����wjh-2016.6.06
		memcpy(pElemInfo->smcItems,smcinfo.smcItems,sizeof(SMC_ITEM)*3);
	}
	//CSuperSmartPtr<CLDSPart> pHorizPole=pElem->pOrgPole->BelongModel()->FromPartHandle(pElem->pOrgPole->CalLenCoef.hHorizPole);
	//horiElem.hStart
}
bool CElement::IsHasMirInfo()
{	return relaElem.idElemMirX>0||relaElem.idElemMirY>0||relaElem.idElemMirZ>0;}
extern int MirPointI(int quad,MIRMSG msg);
//�Ƿ���pElem�жԳƹ���
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
//�Ƚ�(�Գ�)��Ԫ��ʼĩ�ڵ��
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
	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ���Ծ�(mm4)
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
			if(para==0)	//����ƽ����Ծ�
				return pType->Ix*10000;	//cm4->mm4
			else		//������С����Ծ�
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
	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ�������ģ��(mm3)
double CElement::GetWorkW(int para)
{
	if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pType=FindJgType(pOrgPole->size_wide,pOrgPole->size_thick,pOrgPole->size_cSubClassType);
		if(pType)
		{
			if(para==0)	//����ƽ������Ծ�
				return pType->Wx*1000;	//cm4->mm4
			else		//������С����Ծ�
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
	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ��ת�뾶(mm)
double CElement::GetWorkR(int para/*=0*/)
{
	if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pType=(JG_PARA_TYPE*)pParaType;
		if(pType==NULL)
			pType=FindJgType(pOrgPole->size_wide,pOrgPole->size_thick,pOrgPole->size_cSubClassType);
		if(pType)
		{
			if(para==0)	//����ƽ����Ծ�
				return pType->Rx*10;	//cm->mm
			else		//������С����Ծ�
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
	else if(pOrgPole->size_idClassType==CLS_LINEFLAT)	//0.25��һ����������
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
		//���㸲������ϵ��
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
		bodyLoad.selfWeightLoad=0;	//���ߵ�Ԫ���������ز���Ҫ��Ч�ɽڵ���أ�ϵͳ�������ߵ�Ԫ�п���
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
			continue;	//���ĵ�Ԫ
		if(pElem->pStart!=pNode&&pElem->pEnd!=pNode)	//���Դ˽ڵ�Ϊ�˽ڵ�ĸ˼���Ԫ
			continue;
		if(axis_vec.IsZero())
		{
			axis_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
			normalize(axis_vec);
			continue;
		}
		force_face_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
		normalize(force_face_vec);
		if(fabs(axis_vec*force_face_vec)>EPS_COS2)	//����ͬ��˼���Ԫ
			continue;
		if(num_face==0)
		{
			face_norm[0]=axis_vec^force_face_vec;
			normalize(face_norm[0]);
			num_face=1;
		}
		else if(fabs(force_face_vec*face_norm[0])>EPS2)
		{	//���ֵڶ�������˼�
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
			continue;	//����
		else if(pElem->elem_type==1)
		{
			if(pElem->T0<EPS)
				logerr.Log("%d-%d����Ԫȱ��Ԥ�Ž������޷�������������Ԫ����!",pElem->pStart->point_i,pElem->pEnd->point_i);
			m_bHasCableElem=true;	//�ṹ���������ߵ�Ԫ
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
		{	//�Զ��ж�
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
		{	//��������ܵ�Ԫ
			pElem->pFemElem->start_joint_type=JOINT_HINGE;
			pElem->pFemElem->end_joint_type=JOINT_HINGE;
		}
		else if(pElem->pOrgPole->m_iElemType==2)
		{	//��������Ԫ
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
		//�ж����ڵ�����ɶ�
		if(pElem->pFemElem->start_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("ϵͳ��0X%X�˼��ڵ�Ԫʼ����Ϊ���������˸˼������п����������������!",pElem->pOrgPole->handle);
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
				sError.Format("ϵͳ��0X%X�˼��ڵ�Ԫ�ն���Ϊ���������˸˼������п����������������!",pElem->pOrgPole->handle);
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
		//��������֧�Ż�ƽ��֧�Ÿ˼���ӦѰ�Ҵ�Խ�ڵ�Ĺ��ߵ�Ԫ�� wjh-2015.6.3
		for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
		{
			if(pElem->pFemElem==NULL)
				continue;	//����
			if(pElem->pFemElem->pStart!=pFemNode&&pElem->pFemElem->pEnd!=pFemNode)
				continue;
			double prev_justify=0;
			CElement* pJointElem=NULL,*pPerfectJointElem=NULL;
			GEPOINT lenStdVec=pElem->LenStdVec();
			for(GEPOINT *pGeVec=hashVectors.GetFirst();pGeVec;pGeVec=hashVectors.GetNext())
			{
				double justify=fabs(lenStdVec*(*pGeVec));
				if(justify>EPS_COS2)	//����
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
			continue;	//����
		else if(pElem->elem_type==1)
		{
			if(pElem->T0<EPS)
				logerr.Log("%d-%d����Ԫȱ��Ԥ�Ž������޷�������������Ԫ����!",pElem->pStart->point_i,pElem->pEnd->point_i);
			m_bHasCableElem=true;	//�ṹ���������ߵ�Ԫ
			continue;
		}
		pElem->pFemElem->start_joint_type=pElem->real_start_joint_type;
		pElem->pFemElem->end_joint_type=pElem->real_end_joint_type;
		//�ж����ڵ�����ɶ�
		if(pElem->pFemElem->start_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("ϵͳ��0X%X�˼��ڵ�Ԫʼ����Ϊ���������˸˼������п����������������!",pElem->pOrgPole->handle);
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
				sError.Format("ϵͳ��0X%X�˼��ڵ�Ԫ�ն���Ϊ���������˸˼������п����������������!",pElem->pOrgPole->handle);
				throw sError;
			}
			pElem->pFemElem->pEnd->dimension=6;
		}
		else if(pElem->pFemElem->pEnd->dimension!=6)
			pElem->pFemElem->pEnd->dimension=3;
		pElem->ciFreedomStart=(BYTE)pElem->pFemElem->pStart->dimension;
		pElem->ciFreedomEnd  =(BYTE)pElem->pFemElem->pEnd->dimension;
		if(pElem->pOrgPole->layer(0)=='L')	//���ȵ�ԪĬ�ϲ������ӽṹ��
			pResolver->AddElem(*pElem->pFemElem);
	}
}
//<DEVELOP_PROCESS_MARK nameId="CAnalyzeProcessor::ApplyBraceForceToAuxElem">
void CAnalyzeProcessor::ApplyBraceForceToElem()
{
	//���㸨������ѹ����б�Ķ�������С֧��ѹ��
	double coef_p2p=CPreProcessor::ActiveDesignCode->GetBraceCoefOfPrimaryToPost();
	double coef_r2p=CPreProcessor::ActiveDesignCode->GetBraceCoefOfRedundantToPost();
	double coef_r2p2=CPreProcessor::ActiveDesignCode->GetBraceCoefOfRedundantToPrimary();
	for(CElement *pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		//�������涨��DL/T 5154-2012 P79���ģ����ڶԱ�����δ����������ۼ�ʱ�ſ���б�Ķ����ĵĳ�����,
		//	���ۺϿ���ȥ���˴�б����� wjh-2017.3.21
		//if(!(pElem->pOrgPole->IsAuxPole()||(coef_p2p>0&&pElem->pOrgPole->IsPrimaryRod(true))))
		if(!pElem->pOrgPole->IsAuxPole())
			continue;	//���Ļ򸨲�
		double coef_2p=coef_r2p;
		if(pElem->pOrgPole->IsPrimaryRod(true))
			coef_2p=coef_p2p;
		//if(pElem->pOrgPole->IsAuxPole()&&pElem->pOrgPole->sel_mat_method==AUX_ONLY_PRIMARY)
		if(pElem->pOrgPole->sel_mat_method==AUX_ONLY_PRIMARY)
			coef_2p=0;	//��ǰ���Ľ����Ƕ�б��(��ͨ�����ĵ�֧��, ������Ǹ���) wjh-2017.2.8
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
		double maxN=-max(startN,endN);	//�ø����ĳ���ѹ��
		if((pElem->pOrgPole->IsAuxPole()||pElem->N<0)&&maxN<pElem->N)
		{
			pElem->bEffectN=true;
			pElem->N=maxN;
		}
	}
	//��������������б��ʱ���ศ�Ĵ��ݹ�����֧����
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
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem1->pStart->Position(false).y)<EPS)	//X��Գ�
				baseno_start[0]=no_start[0];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem1->pStart->Position(false).y)<EPS)	//Y��Գ�
				baseno_start[0]=no_start[0];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem1->pStart->Position(false).y)<EPS&&fabs(pElem1->pStart->Position(false).y)<EPS)	//Z��Գ�
				baseno_start[0]=no_start[0];
			else
				baseno_start[0]=MirPointI(no_start[0],relaObj.mirInfo);
		}
		if(pElem1->pEnd!=pElem1->pOrgPole->pEnd)
			baseno_end[0]=MirPointI(no_end[0],relaObj.mirInfo);
		else
		{
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem1->pEnd->Position(false).y)<EPS)	//X��Գ�
				baseno_end[0]=no_end[0];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem1->pEnd->Position(false).y)<EPS)	//Y��Գ�
				baseno_end[0]=no_end[0];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem1->pEnd->Position(false).y)<EPS&&fabs(pElem1->pEnd->Position(false).y)<EPS)	//Z��Գ�
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
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem2->pStart->Position(false).y)<EPS)	//X��Գ�
				baseno_start[1]=no_start[1];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem2->pStart->Position(false).y)<EPS)	//Y��Գ�
				baseno_start[1]=no_start[1];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem2->pStart->Position(false).y)<EPS&&fabs(pElem2->pStart->Position(false).y)<EPS)	//Z��Գ�
				baseno_start[1]=no_start[1];
			else
				baseno_start[1]=MirPointI(no_start[1],relaObj.mirInfo);
		}
		if(pElem2->pEnd!=pElem2->pOrgPole->pEnd)
			baseno_end[1]=MirPointI(no_end[1],relaObj.mirInfo);
		else
		{
			if(relaObj.mirInfo.axis_flag==1&&fabs(pElem2->pEnd->Position(false).y)<EPS)	//X��Գ�
				baseno_end[1]=no_end[1];
			else if(relaObj.mirInfo.axis_flag==2&&fabs(pElem2->pEnd->Position(false).y)<EPS)	//Y��Գ�
				baseno_end[1]=no_end[1];
			else if(relaObj.mirInfo.axis_flag==4&&fabs(pElem2->pEnd->Position(false).y)<EPS&&fabs(pElem2->pEnd->Position(false).y)<EPS)	//Z��Գ�
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
		{	//������ͬһ����Ϊ�ǻ���ԳƵĸ˼�
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
//���������ɷ��������ļ��������ǶԳ���ϲ�����
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
		fprintf(fp,"  No.  ��x(mm)  ��y(mm)  ��z(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)  Fu (kN)\n");
		fprintf(fp,"---------------------------------------------------------------------------------------\n");
	}
	else
	{
		fprintf(fp,"  No.  ��x(mm)  ��y(mm)  ��z(mm)   Fex(kN)  Fey(kN)  Fez(kN)  Fwx(kN)  Fwy(kN)\n");
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
	//�˼���������ѡ�ı���
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
//<-2.0ֱ�߶�������;<0.0||>1.0:�㲻�����ϻ�ֱ����;0.0:����ֱ��ʼ���غ�;<1.0����ֱ����;1.0����ֱ���ն��غ�
double CElement::CheckPoint(const f3dPoint& pick)
{
	if(length<EPS)
		CalLength();
	double dd;	//�ж�����
	if(length<EPS)
		return -2.0;	//ֱ�߶�������
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
		else	// �ж�ָ����pick�Ƿ��뵱ǰֱ�߹���
		{
			f3dPoint verifyVec = pick-pStart->Position(false);
			double mod=verifyVec.mod();
			if(verifyVec*lenStdVec<mod*(1-eps))
				return -1.0;	//�㲻��ֱ�߶���
			else
				return dd;	//����ֱ�߶���
		}
	}
	else
		return dd;	//�㲻��ֱ�߶���
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
//�Ƿ��ڱ����������ͬʱ��ѹʱ�ĳ�ϸ��(ֻ��ͬʱ��ѹʱ��ϸ�ȹ���ϵ��<����ϵ��ʱ���)
bool CElement::IsExportBothCompLamda(IMemberDesignCode* pDesignCode/*=NULL*/)
{
	//ͨ���Ƚϸ��淶,����һ�������ѹ���ȶ�mNһ����0.85~1.0֮��,���øֲ����ǿ�����Ҳ��200MPa�ʿ�ȡ���Ӧ��С�ڼ�������Ӧ��(0.85*200=170MPa)��6%(��10MPa),�ɲ�����ͬʱ��ѹ wjh-2017.5.30
	if(fabs(MaxCompStable.sigema)<10)
		return false;
	if(pDesignCode==NULL)
		pDesignCode=CPreProcessor::ActiveDesignCode;
	//Ϊ�˱�֤�����ϸ����ͬʱ��ѹʱѹ��ϵ��һ��,�о������ڱ�֤����ͬʱ��ѹ��ϸ��Ϊ������ݻ��������ͬʱ��ѹ��ϸ�ȸ����� wjh-2017.5.30
	if(pOrgPole->IsAuxPole())	//���渨��
		return MaxCompStable.Lo_r_bothcomp<pDesignCode->GetBraceRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//Ta.SelMatPara.m_nXieTubeSlendernessLimit;
	else						//����б��
		return MaxCompStable.Lo_r_bothcomp<pDesignCode->GetPrimaryRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);//Ta.SelMatPara.m_nXieTubeSlendernessLimit;
	/*
	double limitOfLamda=pDesignCode->GetPrimaryRodKlamdaLimit(pOrgPole->size_idClassType==CLS_LINETUBE);
	double lamdaBearCoef=MaxCompStable.Lo_r_bothcomp/limitOfLamda;
	//����ѹ���ȶ�ǿ���ۼ�ϵ��
	double mN=1.0;
	double t=GetThick();
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pOrgPole->cMaterial);
	double fy=pMatType->fy(t);
	double ft=pMatType->f(t);	//���ǿ��
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
		mN = pDesignCode->CompReduceCoef(D0,t,ft,fy,MaxCompStable.Lo_r_bothcomp,true,false);//ѹ���ȶ�ǿ���ۼ�ϵ��
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
	//ȷ����ϸ�����������ֵ
	if(MaxCompression.N>CElement::COMPRESSION_ZERO_FORCE)	//��ѹ��
		design_info.UltimateLo_r=tensLamdaLim;
	else if(pOrgPole->layer(1)=='Z')
		design_info.UltimateLo_r=mainLamdaLim;
	else if(!pOrgPole->IsAuxPole())
		design_info.UltimateLo_r=xieLamdaLim;
	else
		design_info.UltimateLo_r=auxLamdaLim;
	//ȷ����ϸ�ȼ�������
	if(design_info.iDangerousRule==4)
	{	//ѹ�ȿ��ƶԳ�ϸ�����У������Σ�չ����µĳ�ϸ�ȣ���һ���ǳ�ϸ����������
		if(MaxCompStable.bothComp&&IsExportBothCompLamda())
		{
			//�������涨2002��P38�涨����б�Ļ�������ͬһ������б��Ϊѹ��ʱ������ϸ�ȿ�ȡͬ������
			//�������涨2012��P37�涨��������ͬһ������б��Ϊѹ��ʱ������ϸ�ȿ�ȡͬ������
			BYTE cSMCType=ActiveSmcItem().cSMCType;
			if(CPreProcessor::ActiveDesignCode->CodeSerial()==CODES::DLT_2002)
				design_info.UltimateLo_r=auxLamdaLim;	//2002�漼���涨������б��ͬʱ��ѹȡ�����ĳ�ϸ������ wjh-2017.5.30
			else if(CPreProcessor::ActiveDesignCode->CodeSerial()==CODES::DLT_2012 && 
				pOrgPole->layer(1)!='Z'&&!pOrgPole->IsAuxPole()&&(cSMCType<SMC_ITEM::SMC_REFER_START||cSMCType>SMC_ITEM::SMC_REFER_2ENDS))
				design_info.UltimateLo_r=auxLamdaLim;
			lamdaSelType=MaxCompStable.lamdatype_bothcomp;
		}
		else
			lamdaSelType=MaxCompStable.lamdatype_tenscomp;
	}
	else //������ƹ��������У�ȫȡ��ϸ��ԣ��ϵ�����ֵ���
	{
		if(lamdaRyo.Lo_r>=lamdaRx.Lo_r)//LamdaRc().Lo_r)
			lamdaSelType=0;
		else //if(lamdaRx.Lo_r>=lamdaRyo.Lo_r)//LamdaRc().Lo_r)
			lamdaSelType=1;
	}
	SMC_ITEM* pSmcItem=NULL;
	LAMDA_STABLE_INFO* pLamdaItem=NULL;
	//����Ҫ����ĳ�ϸ�������Ϣ
	if(lamdaSelType==0)
	{	//lamdaRyo����
		design_info.cLamdaTypeR=1;	//0:Rx;1:Ry0
		design_info.lamdaL=lamdaRyo.L;	//���㳤��
		design_info.lamdaR=lamdaRyo.R;	//��ת�뾶
		design_info.Lo_r=lamdaRyo.Lo_r;	//ʵ�ʳ�ϸ��
	}
	else if(lamdaSelType==1)
	{	//lamdaRx����
		design_info.cLamdaTypeR=0;	//0:Rx;1:Ry0
		design_info.lamdaL=lamdaRx.L;	//���㳤��
		design_info.lamdaR=lamdaRx.R;	//��ת�뾶
		design_info.Lo_r=lamdaRx.Lo_r;	//ʵ�ʳ�ϸ��
	}
	else if(lamdaSelType==2)
	{	//lamdaRx2����
		design_info.lamdaL=MaxCompStable.L_bothcomp;	//���㳤��(�����㳤������ϵ��)
		design_info.Lo_r=MaxCompStable.Lo_r_bothcomp;	//ʵ�ʳ�ϸ��
		pSmcItem=&smcinfo.smcItems[smcinfo.iActiveItem];
		if(pSmcItem->cSMCType>=0)
			design_info.cLamdaTypeR=0;	//0:Rx;1:Ry0
		else
			design_info.cLamdaTypeR=1;
		pLamdaItem=&smcinfo.lamdaItems[smcinfo.iActiveItem];
		design_info.lamdaR=pLamdaItem->R;	//��ת�뾶
	}
	//���������ϸ��������ַ���
	CXhChar50 braceStr;
	//if(pOrgPole->layer(0)=='L'&&pOrgPole->layer(1)=='X'&&pOrgPole->CalLenCoef.iTypeNo!=5)
	//m_bLegVRod=trueʱ��1.2����ϵ���Ѱ�����design_info.lamdaL��design_info.Lo_r��,�����ظ�����. wjh-2017.5.30
	double lenCoef= !pOrgPole->m_bLegVRod ? 1.0 : 1.2;
	if(lamdaSelType==0)	//lamdaRyo����
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
	else if(lamdaSelType==1)	//lamdaRx����
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
	else				//lamdaRx2����
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
		design_info.lamdaExpression.Append("/R");	//�ֹܲ���ƽ������С��
	else if(lamdaSelType!=0)
		design_info.lamdaExpression.Append("/Rx");	//ƽ�������
	else if(lamdaSelType==0)
		design_info.lamdaExpression.Append("/Ryo");	//��С�����
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
//һ������ģ�͵����й���������̷�����Ϻ���ã���Ҫ���ڼ���ѹ�������Ƿ����㹻��ת��Լ��
void CAnalyzeProcessor::UpdateInfoAtModuleChange(CLDSModule *pModule)
{
	if(m_pPostProcessor==NULL)
		return;	//�޺����޷�������һ��������ֻ���˳�
	for(CElement* pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)	//���
			continue;
		CElement* pPostElem=NULL;
		if((pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->id))||
		   (pElem->relaElem.idElemMirX>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirX)))||
		   (pElem->relaElem.idElemMirY>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirY)))||
		   (pElem->relaElem.idElemMirZ>0&&(pPostElem=m_pPostProcessor->hashElems.GetValue(pElem->relaElem.idElemMirZ))))
		   ;
		else //if(pPostElem==NULL)
			continue;	//�ںϲ��Գ���ʱ���ܻ���ĳЩ��ǰ������̵�Ԫ�����ں���Ԫ��
		//�����Ƿ���ṩ�㹻��ת��Լ���ն�
		if(  pElem->pOrgPole->IsAuxPole()||	//���Ļ������ѹ�˵�Ԫ
			(toupper(pElem->pOrgPole->layer(1))!='Z'&&pPostElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE))	//
		{	//δ��������µ�Ԫ
			//������ѹ���Ƿ��кϷ��ĸ���Լ��
			f3dPoint base_vec;
			if(pElem->start_joint_type==JOINT_RIGID)
				pPostElem->m_bValidStartRestStiff=TRUE;
			else if(pElem->start_joint_type==JOINT_HINGE)
				pPostElem->m_bValidStartRestStiff=FALSE;
			else if(pElem->start_joint_type==JOINT_AUTOJUSTIFY&&(pPostElem->pStart!=pPostElem->pOrgPole->pStart||pPostElem->pOrgPole->connectStart.wnConnBoltN>=2))
			{	//�˸˼���Ҫ�����ǹ��߸��Ǹֵ�Ԫ�ṩԼ��
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
							{	//��ǰ����ģ���ڵĵ�Ԫ
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pStart||pElemStiff->pElem->pEnd==pElem->pStart)
										support_stiff+=pElemStiff->stiff;	//�жϴ˵�Ԫ�Ƿ�Ϊ��ǰģ���е�Ԫ
									else
										support_stiff+=pElemStiff->stiff*4;	//�ڵ��ڵ�Ԫ�м�֧�Ÿնȳ�4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//�ṩԼ���ĵ�Ԫ
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//�������ڱ�Լ���ĵ�Ԫ
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
			{	//�˸˼���Ҫ�����ǹ��߸��Ǹֵ�Ԫ�ṩԼ��
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
							{	//��ǰ����ģ���ڵĵ�Ԫ
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pEnd||pElemStiff->pElem->pEnd==pElem->pEnd)
										support_stiff+=pElemStiff->stiff;	//�жϴ˵�Ԫ�Ƿ�Ϊ��ǰģ���е�Ԫ
									else
										support_stiff+=pElemStiff->stiff*4;	//�ڵ��ڵ�Ԫ�м�֧�Ÿնȳ�4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//�ṩԼ���ĵ�Ԫ
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//�������ڱ�Լ���ĵ�Ԫ
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
	//	return;	//�ݲ�ȷ���Ƿ���Ҫ���ǽ��渨��ͬʱ��ѹ��� wjh-2017.2.4
	char sError[100]="";
	IMemberDesignCode::BevelRodLamdaPara lamdaPara;
	char sectionType='B';
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE&&pElem->pOrgPole->size_wide<g_sysPara.fTubeWeldRoadMinD)
		sectionType='A';
	double area;	//��λ��Ϊmm��
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pParaType==NULL)
			pElem->pParaType=pParaType=FindTubeType(pElem->GetWidth(),pElem->GetThick());
		if(pParaType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�˼��Ĺ���%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
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
				sprintf(sError,"�������Ҳ���0X%X�˼��Ĺ��L%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			else
				sprintf(sError,"�������Ҳ���0X%X�˼��Ĺ��%CL%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_cSubClassType,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
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
		{	//�ǽ���б��ͬʱ��ѹ��Ԫ��������㶯̬ϵ��
			pElem->smcinfo.lamdaItems[i].muCoefLive=1.0;
			if(pElem->N<0&&pRefElem->N<fabs(CElement::COMPRESSION_ZERO_FORCE))	//���յ�Ԫͬʱ��ѹ��������������ģ�
				pElem->smcinfo.lamdaItems[i].bothCompLive=true;//��ͬʱ��ѹ����bothCompLive
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
		//��ͬʱ��ѹ���ǣ����붯̬��ϸ������ϵ��
		pElem->smcinfo.lamdaItems[i].muCoefLive=lamdaPara.K/0.8;
		//pElem->smcinfo.lamdaItems[i].muCoefBasic=raiseCoef*pElem->smcinfo.smcItems[i].LoRc.coef;
		//���붯̬��ϸ������ϵ����ĳ�ϸ�ȱ�׼ֵ
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
			if(strchr(expr_str,'.')!=NULL)	//�뵥Ԫ���ȱ�ֵΪ������С��
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
//һ������ģ�͵ĵ�ǰ����������̷�����Ϻ����
void CAnalyzeProcessor::UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus)
{
	int i;
	CElement *pElem,*pPostElem,*pRefElem;
	for(pElem=hashElems.GetFirst();pElem;pElem=hashElems.GetNext())
	{
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)	//���ߵ�Ԫ�����
			continue;
		//pElem->lamdaRc.ClearZero();		//��ʼ����ͬʱ��ѹ��Ϣ
		for(i=0;i<3;i++)
		{	//Ϊ������ԭ���������һ�£���ʹ��ͬʱ��ѹ�������Ҳ�õ�һ����Чͬʱ��ѹ�������и�ֵ
			if(pElem->smcinfo.smcItems[i].cSMCType!=SMC_ITEM::SMC_NONE)
			{
				//pElem->lamdaRc=pElem->smcinfo.lamdaItems[i];
				//��һ�д�������CPreProcessor::CalElemCompStableInfo�и��� wjh-2013.9.22
				//pElem->lamdaRc.coef=pElem->smcinfo.smcItems[i].LoRc.coef;
				pElem->MaxCompStable.Afai_bothcomp=pElem->smcinfo.lamdaItems[i].coefAreaFai;
				pElem->MaxCompStable.fai_bothcomp=pElem->smcinfo.lamdaItems[i].coefFai;
				//pElem->MaxCompStable.Lo_r_bothcomp=pElem->LamdaRc().Lo_r;
				//pElem->MaxCompStable.L_bothcomp=pElem->LamdaRc().L;
				//pElem->MaxCompStable.lamdatype_bothcomp
				//TODO: ԭ����������
				//if(pElem->smcinfo.smcItems[i].cSMCType>0)
					pElem->MaxCompStable.lamdatype_bothcomp=2;
				//else
				//	pElem->MaxCompStable.lamdatype_bothcomp=0;
				break;
			}
		}
		if(pElem->N<fabs(CElement::COMPRESSION_ZERO_FORCE))
		{	//��ȫ����������Ҳ��Ϊѹ�ˣ����ڽ��г�ϸ����ֵ����
			double sigema=0;
			bool hasTensComp=false;
			pElem->MaxCompStable.sigema=0;	//��ʼ����Ĭ��ֵ
			CalElemCompStableLiveSMCInfo(pElem);
			for(i=0;i<3;i++)
			{
				if(pElem->smcinfo.smcItems[i].cSMCType==SMC_ITEM::SMC_NONE)
					continue;	//��Ч��
				ELEM_PTR* ppElem=m_pPreProcessor->result.hashElemsMap.GetValue(pElem->smcinfo.smcItems[i].relaElem.hStart,pElem->smcinfo.smcItems[i].relaElem.hEnd);
				//if(pElem->relaElem.idRefElem>0)
				if(ppElem!=NULL)
					pRefElem=hashElems.GetValue((*ppElem)->id);
				else
					pRefElem=NULL;
				//if(lamdaPara.cType=='C')	//��ͬʱ��ѹ����
				//if(pRefElem&&pRefElem->N<fabs(CElement::COMPRESSION_ZERO_FORCE))	//���յ�Ԫͬʱ��ѹ��������������ģ�
				if(pElem->smcinfo.lamdaItems[i].bothCompLive)
				{	//��ȫ����������Ҳ��Ϊѹ�ˣ����ڽ��г�ϸ����ֵ����
					sigema=pElem->N/pElem->smcinfo.lamdaItems[i].coefAreaFai;
					pElem->smcinfo.smcItems[i].enabled=true;
					if(fabs(sigema)>fabs(pElem->MaxCompStable.sigema))
					{
						pElem->MaxCompStable.sigema=sigema;
						pElem->MaxCompStable.Afai_bothcomp=pElem->smcinfo.lamdaItems[i].coefAreaFai;
						pElem->MaxCompStable.fai_bothcomp=pElem->smcinfo.lamdaItems[i].coefFai;
						//�ѹ鲢��pElem->MaxCompStable.bothComp=true�� wjh 2013.9.22
						//pElem->MaxCompStable.fai=pElem->MaxCompStable.fai_bothcomp;
						pElem->MaxCompStable.bothComp=true;
						pElem->smcinfo.hasBothComp=true;	//��Ĭ��ֵ
						pElem->MaxCompStable.L_bothcomp=pElem->smcinfo.lamdaItems[i].L;
						pElem->MaxCompStable.Lo_r_bothcomp=pElem->smcinfo.lamdaItems[i].Lo_r;
						pElem->MaxCompStable.muCoefL_bothcomp=pElem->smcinfo.lamdaItems[i].muCoefBasic*pElem->smcinfo.lamdaItems[i].muCoefLive;
						//pElem->MaxCompStable.cActiveSMCType=pElem->smcinfo.smcItems[i].cSMCType;
						//ֻ��Ϊ��������һ�л�ȡ��ǰ��Ч�Ļ�ת�뾶��Ϣ wjh-2013-5-31
						pElem->smcinfo.iActiveItem=i;
						//pElem->lamdaRc=pElem->smcinfo.lamdaItems[i];
						//��һ�д�������CPreProcessor::CalElemCompStableInfo�и��� wjh-2013.9.22
						//pElem->lamdaRc.coef=pElem->smcinfo.smcItems[i].LoRc.coef;
					}
				}
				else
					hasTensComp=true;
			}
			if(pElem->MaxCompStable.Afai_tenscomp>0&&(hasTensComp||pElem->MaxCompStable.sigema==0))
			{	//ֻҪϵͳ�趨����ͬʱ��ѹ���ʱ�ĳ�ϸ�Ȳ�����LoRy0,LoRx)ֵ����Ҫ���Ƿ�ͬʱ��ѹ������֣�
				sigema=pElem->N/pElem->MaxCompStable.Afai_tenscomp;
				if(fabs(sigema)>fabs(pElem->MaxCompStable.sigema))
				{
					pElem->MaxCompStable.sigema=sigema;
					//�ѹ鲢��pElem->MaxCompStable.bothComp=false�� wjh 2013.9.22
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
			{	//�Ƿ�����������е�Ԫ����ȷ�Ĺ�����ϵ
				if(pPostElem->CompareElemPointI(pElem)>0)
				{	//�µ�Ԫʼĩ�˽ڵ�Ÿ�С�����������������ʾ��С�ڵ�ŵĶԳƵ�Ԫ
					//�����¿��ܵ���У�󴰿�RevisionOperDlg�����쳣 wjh-2017.2.4
					pPostElem=m_pPostProcessor->hashElems.ModifyKeyId(pPostElem->id,pElem->id);
					pPostElem->id=pElem->id;	//�������id�������ֽڵ����id��һ�µ����������
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
			{	//���ָ�������
				pPostElem->MaxTension.N=pElem->N;
				pPostElem->MaxTension.hModule=pModule->handle;
				pPostElem->MaxTension.hWorkStatus=pStatus->GetEiffelStatusId();//handle;
				for(int iLeg=0;iLeg<4;iLeg++)
					pPostElem->MaxTension.leg_no[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
			}
			else if(pElem->N-pPostElem->MaxCompression.N<-CElement::DIFFERENCE_ZERO_FORCE)
			{	//���ָ���ѹ��
				pPostElem->MaxCompression.N=pElem->N;
				pPostElem->MaxCompression.bothComp=pElem->MaxCompStable.bothComp;	//���ѹ��ʱ�϶�������ѹ�ȣ����䵱ǰ������ͬʱ��ѹ״̬��ѹ��һ�� wjh-2017.2.4
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
			//ע�����ڳ�ϸ�ȴ�����ѹ��ϵ�����أ�ѹ�ȼ���Ӧ�����ʱ��һ��ѹ�����
			if(pElem->MaxCompStable.sigema-pPostElem->MaxCompStable.sigema<-CElement::DIFFERENCE_ZERO_STRESS)
			{//�ж��Ƿ�ǰ��Ԫ��ǰ����ѹ�ȼ���Ӧ���Ƿ��ԭ��¼�еľ���ֵ����
				//TODO:ע���ʱ���ܰ�ԭ����hasBothComp���Գ������һ��Ӧ��������ֱ�Ӹ�ֵ
				double fai_bothcomp=pPostElem->MaxCompStable.fai_bothcomp;
				double afai_bothcomp=pPostElem->MaxCompStable.Afai_bothcomp;
				double L_bothcomp=pPostElem->MaxCompStable.L_bothcomp;
				double Lo_r_bothcomp=pPostElem->MaxCompStable.Lo_r_bothcomp;
				double muCoefL_bothcomp=pPostElem->MaxCompStable.muCoefL_bothcomp;
				pPostElem->MaxCompStable=pElem->MaxCompStable;
				if(!pElem->MaxCompStable.bothComp)
				{	//TODO:ֻ����Ϊ��ʱ����ԭʼ�����жԳƸ˼���ͬʱ��ѹ��Ϣ��ͬ(����ȱʧ)�����������
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
		else //if(pPostElem==NULL)	//�¼���˼���Ԫ������ע��
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
			//ֻ��Ϊ��������һ�л�ȡ��ǰ��Ч�Ļ�ת�뾶��Ϣ wjh-2013-5-31
			pPostElem->smcinfo.iActiveItem=pElem->smcinfo.iActiveItem;
			pPostElem->smcinfo.hasBothComp=pElem->smcinfo.hasBothComp;
			pPostElem->MaxCompStable.Afai_bothcomp=pElem->LamdaRc().coefAreaFai;
			pPostElem->MaxCompStable.fai_bothcomp=pElem->LamdaRc().coefFai;
			pPostElem->MaxCompStable.L_bothcomp=pElem->MaxCompStable.L_bothcomp;
			pPostElem->MaxCompStable.Lo_r_bothcomp=pElem->MaxCompStable.Lo_r_bothcomp;
			pPostElem->MaxCompStable.muCoefL_bothcomp=pElem->MaxCompStable.muCoefL_bothcomp;
			if(!pElem->moment.IsZero())
			{	//�������
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
				//ѹ����ͬʱ��ѹ����
				pPostElem->MaxCompStable=pElem->MaxCompStable;
				pPostElem->MaxCompStable.bEffectN=pPostElem->MaxCompression.bEffectN=pElem->bEffectN;
				pPostElem->MaxCompStable.N=pPostElem->MaxCompression.N=pElem->N;
				pPostElem->MaxCompStable.hModule=pPostElem->MaxCompression.hModule=pModule->handle;
				//��ǰ©����һ�д��룬���������һ�γ���ѹ�ȹ���Ϊ����ѹ�ȿ��ƹ���ʱ��ȱʧѹ�ȹ����� wjh-2013.9.20
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
		{	//�˼�ʼ��
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
		{	//�˼�ʼ��
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
