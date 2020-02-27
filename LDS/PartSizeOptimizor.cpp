//<LOCALE_TRANSLATE Confirm by hxr/>
// PartSizeOptimizor.cpp: implementation of the CPartSizeOptimizor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lds.h"
#include "PartSizeOptimizor.h"
#include "SortFunc.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(__TSA_)||defined(__LDS_)
void CalElemRelaBoltInfo(CElement *pElem)
{
	CXhChar50 error_info;
	double new_wing_wide,new_wing_thick;
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
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
		//计算首末单元的螺栓数
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			int ls_d;
			char lsgrade[5];
			ls_d=SelBoltDByWingWide(new_wing_wide,lsgrade);
			LS_GRADE *pLsType=FindLsMatType(lsgrade);
			if(pLsType==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				error_info.Printf("The set bolt grade '%s' of wing width %.f by system is wrong,fail to find!",new_wing_wide,lsgrade);
#else
				error_info.Printf("系统为肢宽%.f设定的螺栓级别'%s'有误，找不到！",new_wing_wide,lsgrade);
#endif
				throw (char*)error_info;
			}
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double fc=min(pMatType->fc(new_wing_thick),pLsType->fc);
			double Nbc=Ta.m_fLimitBearCoef*ls_d*new_wing_thick*fc;
			double maxN=max(fabs(pElem->MaxCompression.N),fabs(pElem->MaxTension.N));
			strcpy(pElem->pOrgPole->connectStart.grade,lsgrade);
			pElem->pOrgPole->connectStart.d=ls_d;
			pElem->pOrgPole->connectStart.wnSummBoltN=(int)ceil(maxN/min(Nbv,Nbc));
			if(pElem->pOrgPole->connectStart.wnSummBoltN==0)
				pElem->pOrgPole->connectStart.wnSummBoltN = 1;
			if(pElem->pOrgPole->connectStart.wnSummBoltN<2&&pElem->start_joint_type==JOINT_AUTOJUSTIFY)	//没有承受部分转动约束的能力
				pElem->m_bValidStartRestStiff=FALSE;
		}
		if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			int ls_d;
			char lsgrade[5];
			ls_d=SelBoltDByWingWide(new_wing_wide,lsgrade);
			LS_GRADE *pLsType=FindLsMatType(lsgrade);
			if(pLsType==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				error_info.Printf("The set bolt grade '%s' of wing width %.f by system is wrong,fail to find!",new_wing_wide,lsgrade);
#else
				error_info.Printf("系统为肢宽%.f设定的螺栓级别'%s'有误，找不到！",new_wing_wide,lsgrade);
#endif
				throw (char*)error_info;
			}
			double maxN=max(fabs(pElem->MaxCompression.N),fabs(pElem->MaxTension.N));
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double fc=min(pMatType->fc(new_wing_thick),pLsType->fc);
			double Nbc=Ta.m_fLimitBearCoef*ls_d*new_wing_thick*fc;
			pElem->pOrgPole->connectEnd.d=ls_d;
			pElem->pOrgPole->connectEnd.wnSummBoltN=(int)ceil(maxN/min(Nbv,Nbc));
			strcpy(pElem->pOrgPole->connectEnd.grade,lsgrade);
			if(pElem->pOrgPole->connectEnd.wnSummBoltN==0)
				pElem->pOrgPole->connectEnd.wnSummBoltN = 1;
			if(pElem->pOrgPole->connectEnd.wnSummBoltN<2&&pElem->end_joint_type==JOINT_AUTOJUSTIFY)		//没有承受部分转动约束的能力
				pElem->m_bValidEndRestStiff=FALSE;
		}
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
		//计算首末单元的螺栓数
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			int ls_d;
			char lsgrade[5];
			ls_d=SelBoltDByWingWide(new_wing_wide,lsgrade);
			LS_GRADE *pLsType=FindLsMatType(lsgrade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double fc=min(pMatType->fc(new_wing_thick),pLsType->fc);
			double Nbc=Ta.m_fLimitBearCoef*ls_d*new_wing_thick*fc;
			pElem->pOrgPole->connectStart.d=ls_d;
			double maxN=__max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectStart.wnSummBoltN=(int)ceil(maxN/min(Nbv,Nbc));	//允许超过强度设计值的百分之五
			if(pElem->pOrgPole->connectStart.wnSummBoltN==0)
				pElem->pOrgPole->connectStart.wnSummBoltN = 1;
			if(pElem->pOrgPole->connectStart.wnSummBoltN<2)	//没有承受部分转动约束的能力
				pElem->m_bValidStartRestStiff=FALSE;
		}
		if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			int ls_d;
			char lsgrade[5];
			ls_d=SelBoltDByWingWide(new_wing_wide,lsgrade);
			LS_GRADE *pLsType=FindLsMatType(lsgrade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double fc=min(pMatType->fc(new_wing_thick),pLsType->fc);
			double Nbc=Ta.m_fLimitBearCoef*ls_d*new_wing_thick*fc;
			pElem->pOrgPole->connectEnd.d=ls_d;
			double maxN=__max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectEnd.wnSummBoltN=(int)ceil(maxN/min(Nbv,Nbc));	//允许超过强度设计值的百分之五
			if(pElem->pOrgPole->connectEnd.wnSummBoltN==0)
				pElem->pOrgPole->connectEnd.wnSummBoltN = 1;
			if(pElem->pOrgPole->connectEnd.wnSummBoltN<2)		//没有承受部分转动约束的能力
				pElem->m_bValidEndRestStiff=FALSE;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPartSizeOptimizor::CPartSizeOptimizor()
{
	m_fMinSizeArea=0;
	m_pAngleArr=NULL;
	m_pTubeArr=NULL;
	m_pFlatArr=NULL;
	m_pSlotArr=NULL;
	m_nAngleSize=m_nTubeSize=m_nFlatSize=m_nSlotSize=0;
}

CPartSizeOptimizor::~CPartSizeOptimizor()
{
	if(m_pAngleArr)
		delete []m_pAngleArr;
	if(m_pTubeArr)
		delete []m_pTubeArr;
	if(m_pFlatArr)
		delete []m_pFlatArr;
	if(m_pSlotArr)
		delete []m_pSlotArr;
}
int compareAngleType(const JG_PARA_TYPE& type1,const JG_PARA_TYPE& type2)
{
	if(Ta.SelMatPara.m_iSizeOrderRule==0)
	{	//按面积排序
		if(Ta.SelMatPara.m_bPreferSingleAngle)
		{	//单角钢优先
			if(type1.cType=='X'&&type2.cType!='X')
				return 1;
			else if(type1.cType=='D'&&type2.cType=='L')
				return 1;
			else if(type2.cType=='X'&&type1.cType!='X')
				return -1;
			else if(type2.cType=='D'&&type1.cType=='L')
				return -1;
		}
		if(type1.area>type2.area)
			return 1;
		else if(type1.area<type2.area)
			return -1;
		else
			return 0;
	}
	else if(Ta.SelMatPara.m_iSizeOrderRule==1)
	{	//按平行轴回转半径排序
		if(Ta.SelMatPara.m_bPreferSingleAngle)
		{	//单角钢优先
			if(type1.cType=='X'&&type2.cType!='X')
				return 1;
			else if(type1.cType=='D'&&type2.cType=='L')
				return 1;
			else if(type2.cType=='X'&&type1.cType!='X')
				return -1;
			else if(type2.cType=='D'&&type1.cType=='L')
				return -1;
		}
		if(type1.Rx>type2.Rx)
			return 1;
		else if(type1.Rx<type2.Rx)
			return -1;
		else
			return 0;
	}
	else
	{	//按宽度厚度排序
		if(Ta.SelMatPara.m_bPreferSingleAngle)
		{	//单角钢优先
			if(type1.cType=='X'&&type2.cType!='X')
				return 1;
			else if(type1.cType=='D'&&type2.cType=='L')
				return 1;
			else if(type2.cType=='X'&&type1.cType!='X')
				return -1;
			else if(type2.cType=='D'&&type1.cType=='L')
				return -1;
		}
		if(type1.wing_wide>type2.wing_wide)
			return 1;
		else if(type1.wing_wide<type2.wing_wide)
			return -1;
		else if(type1.wing_thick>type2.wing_thick)
			return 1;
		else if(type1.wing_thick<type2.wing_thick)
			return -1;
		else
			return 0;
	}
}
int compareTubeType(const TUBE_PARA_TYPE& type1,const TUBE_PARA_TYPE& type2)
{
	if(Ta.SelMatPara.m_iSizeOrderRule==0)
	{	//按面积排序
		if(type1.area>type2.area)
			return 1;
		else if(type1.area<type2.area)
			return -1;
		else
			return 0;
	}
	else if(Ta.SelMatPara.m_iSizeOrderRule==1)
	{	//按回转半径排序
		if(type1.r>type2.r)
			return 1;
		else if(type1.r<type2.r)
			return -1;
		else
			return 0;
	}
	else
	{	//按宽度厚度排序
		if(type1.D>type2.D)
			return 1;
		else if(type1.D<type2.D)
			return -1;
		else if(type1.thick>type2.thick)
			return 1;
		else if(type1.thick<type2.thick)
			return -1;
		else
			return 0;
	}
}
int compareFlatType(const BIAN_PARA_TYPE& type1,const BIAN_PARA_TYPE& type2)
{
	double area1=type1.wide*type1.thick;
	double area2=type2.wide*type2.thick;
	if(area1>area2)
		return 1;
	else if(area1<area2)
		return -1;
	else
		return 0;
}
int compareSlotType(const SLOT_PARA_TYPE& type1,const SLOT_PARA_TYPE& type2)
{
	if(type1.area>type2.area)
		return 1;
	else if(type1.area<type2.area)
		return -1;
	else
		return 0;
}
void CPartSizeOptimizor::InitPartSizeOrder()
{
	m_nAngleSize=jgguige_N;
	m_pAngleArr=new JG_PARA_TYPE[m_nAngleSize];
	memcpy(m_pAngleArr,jgguige_table,sizeof(JG_PARA_TYPE)*m_nAngleSize);
	m_nTubeSize=tubeguige_N;
	m_pTubeArr=new TUBE_PARA_TYPE[m_nTubeSize];
	memcpy(m_pTubeArr,tubeguige_table,sizeof(TUBE_PARA_TYPE)*m_nTubeSize);
	m_nFlatSize=bianguige_N;
	m_pFlatArr=new BIAN_PARA_TYPE[m_nFlatSize];
	memcpy(m_pFlatArr,bianguige_table,sizeof(BIAN_PARA_TYPE)*m_nFlatSize);
	m_nSlotSize=slotguige_N;
	m_pSlotArr=new SLOT_PARA_TYPE[m_nSlotSize];
	memcpy(m_pSlotArr,slotguige_table,sizeof(SLOT_PARA_TYPE)*m_nSlotSize);
	CHeapSort<JG_PARA_TYPE>::HeapSort(m_pAngleArr,m_nAngleSize,compareAngleType);
	CHeapSort<TUBE_PARA_TYPE>::HeapSort(m_pTubeArr,m_nTubeSize,compareTubeType);
	CHeapSort<BIAN_PARA_TYPE>::HeapSort(m_pFlatArr,m_nFlatSize,compareFlatType);
	CHeapSort<SLOT_PARA_TYPE>::HeapSort(m_pSlotArr,m_nSlotSize,compareSlotType);
}
void CPartSizeOptimizor::UpdateElemSize(CElement *pElem,int iOrder)
{
	if(iOrder<0)
		pElem->pParaType=NULL;
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
		pElem->pParaType=&m_pAngleArr[iOrder];
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
		pElem->pParaType=&m_pTubeArr[iOrder];
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
		pElem->pParaType=&m_pFlatArr[iOrder];
	else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
		pElem->pParaType=&m_pSlotArr[iOrder];
	else
		return;
	pElem->m_iSizeIndex=iOrder;
}
void CPartSizeOptimizor::UpdatePoleSize(CLDSLinePart *pPole,int iOrder)
{
	if(pPole->size_idClassType==CLS_LINEANGLE||pPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		pPole->size_wide=m_pAngleArr[iOrder].wing_wide;
		pPole->size_thick=m_pAngleArr[iOrder].wing_thick;
		pPole->size_cSubClassType=m_pAngleArr[iOrder].cType;
		if(m_pAngleArr[iOrder].cType=='L')
			pPole->size_idClassType=CLS_LINEANGLE;
		else
			pPole->size_idClassType=CLS_GROUPLINEANGLE;
	}
	else if(pPole->size_idClassType==CLS_LINETUBE)
	{
		pPole->size_wide=m_pTubeArr[iOrder].D;
		pPole->size_thick=m_pTubeArr[iOrder].thick;
	}
	else if(pPole->size_idClassType==CLS_LINEFLAT)
	{
		pPole->size_wide=m_pFlatArr[iOrder].wide;
		pPole->size_thick=m_pFlatArr[iOrder].thick;
	}
	else if(pPole->size_idClassType==CLS_LINESLOT)
	{
		pPole->size_height=m_pSlotArr[iOrder].h;
		pPole->size_wide=m_pSlotArr[iOrder].b;
		pPole->size_thick=m_pSlotArr[iOrder].t;
	}
}
int  CPartSizeOptimizor::GetPoleSizeOrderIndex(CLDSLinePart *pPole)
{
	if(pPole->size_idClassType==CLS_LINEANGLE||pPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		for(int i=0;i<m_nAngleSize;i++)
		{
			if( pPole->size_cSubClassType==m_pAngleArr[i].cType&&
				pPole->size_wide==m_pAngleArr[i].wing_wide&&pPole->size_thick==m_pAngleArr[i].wing_thick)
				return i;
		}
	}
	else if(pPole->size_idClassType==CLS_LINETUBE)
	{
		for(int i=0;i<m_nTubeSize;i++)
		{
			if(pPole->size_wide==m_pTubeArr[i].D&&pPole->size_thick==m_pTubeArr[i].thick)
				return i;
		}
	}
	else if(pPole->size_idClassType==CLS_LINEFLAT)
	{
		for(int i=0;i<m_nFlatSize;i++)
		{
			if(pPole->size_wide==m_pFlatArr[i].wide&&pPole->size_thick==m_pFlatArr[i].thick)
				return i;
		}
	}
	else if(pPole->size_idClassType==CLS_LINESLOT)
	{
		for(int i=0;i<m_nSlotSize;i++)
		{
			if(pPole->size_wide==m_pSlotArr[i].b&&pPole->size_thick==m_pSlotArr[i].t&&pPole->size_height==m_pSlotArr[i].h)
				return i;
		}
	}
	return -1;
}
int MemberCheck(CElement *pElem,BOOL bCheckAll,int iDesignCode,bool bEqCheck=false,bool bUpdateBoltInfo=false);
BOOL CPartSizeOptimizor::QuickSelectElemSize(SIZESPEC_SELECT *selectSize,CElement *pElem,int iDesignCode,BOOL bExecuteDichotomy/*=TRUE*/,char cMat/*=0*/)
{
	int iCheck,iStart,iEnd,nSize,iSel=-1;
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		nSize=m_nAngleSize;
		for(iStart=0;iStart<nSize;iStart++)
		{
			if(m_pAngleArr[iStart].area>=m_fMinSizeArea)
				break;
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		nSize=m_nTubeSize;
		for(iStart=0;iStart<nSize;iStart++)
		{
			if(m_pTubeArr[iStart].area>=m_fMinSizeArea)
				break;
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		nSize=m_nFlatSize;
		for(iStart=0;iStart<nSize;iStart++)
		{
			if(m_pFlatArr[iStart].wide*m_pFlatArr[iStart].thick>=m_fMinSizeArea)
				break;
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
	{
		nSize=m_nSlotSize;
		for(iStart=0;iStart<nSize;iStart++)
		{
			if(m_pSlotArr[iStart].area>=m_fMinSizeArea)
				break;
		}
	}
	else
		return FALSE;
	if(bExecuteDichotomy)
	{	//采用二分法快速选材
		iEnd=nSize-1;
		iCheck=iStart;
		do
		{
			UpdateElemSize(pElem,iCheck);
			CalElemRelaBoltInfo(pElem);
			int result=MemberCheck(pElem,FALSE,iDesignCode);
			if(result>0&&result!=6)	//不满足某项构件选材要求
			{
				if(iEnd-iCheck>1)
				{
					iStart=iCheck;
					iCheck=iStart+(iEnd-iStart)/2;
				}
				else
					iSel=iEnd;
			}
			else //if(result==0)
			{
				if(iCheck-iStart>1)
				{
					iEnd=iCheck;
					iCheck=iStart+(iCheck-iStart)/2;
				}
				else
					iSel=iCheck;
			}
		} while (iSel<0);
		UpdateElemSize(pElem,iSel);	//更新材料规格
	}
	else
	{	//按排序逐个验证法
		for(iSel=iStart;iSel<nSize;iSel++)
		{
			UpdateElemSize(pElem,iSel);
			CalElemRelaBoltInfo(pElem);
			int result=MemberCheck(pElem,FALSE,iDesignCode);
			if(result>0&&result!=6)	//不满足某项构件选材要求
				continue;
			else
				break;
		}
		if(iSel==nSize)
			iSel=nSize-1;	//此时强度已不够，只能选最后的一个规格
	}
	selectSize->orderI=iSel;
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pParaType=(JG_PARA_TYPE*)pElem->pParaType;
		selectSize->wide=pParaType->wing_wide;
		selectSize->thick=pParaType->wing_thick;
		if(pParaType->cType=='L')
			selectSize->idClassType=CLS_LINEANGLE;
		else
			selectSize->idClassType=CLS_GROUPLINEANGLE;
		selectSize->cSubClassType=pParaType->cType;
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		selectSize->wide=pParaType->D;
		selectSize->thick=pParaType->thick;
		selectSize->idClassType=CLS_LINETUBE;
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		BIAN_PARA_TYPE *pParaType=(BIAN_PARA_TYPE*)pElem->pParaType;
		selectSize->wide=pParaType->wide;
		selectSize->thick=pParaType->thick;
		selectSize->idClassType=CLS_LINEFLAT;
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINESLOT)
	{
		SLOT_PARA_TYPE *pParaType=(SLOT_PARA_TYPE*)pElem->pParaType;
		selectSize->wide=pParaType->b;
		selectSize->thick=pParaType->d;
		selectSize->thick=pParaType->h;
		selectSize->idClassType=CLS_LINESLOT;
	}
	if(iSel==nSize-1)
	{
		CalElemRelaBoltInfo(pElem);
		if(MemberCheck(pElem,FALSE,iDesignCode)>0)	//还需要验证最后一个规格
			return FALSE;
	}
	return TRUE;
}

#endif
