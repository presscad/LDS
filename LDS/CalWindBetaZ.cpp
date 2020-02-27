#include "StdAfx.h"
#include <math.h>
#include "Tower.h"
#include "ArrayList.h"
#include "CalWindBetaZ.h"
#include "PreProcessor.h"
#include "LibraryQuery.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __PART_DESIGN_INC_
double MASSNODE::MassInWindSeg(long idWindSeg)
{
	if(idWindSeg<=0)
		return 0;
	for(int i=0;i<4;i++)
	{
		if(xarrMassOfWindSeg[i].idWindSeg==idWindSeg)
			return xarrMassOfWindSeg[i].dfAllocMass;
		else if(xarrMassOfWindSeg[i].idWindSeg==0)
			return 0;
	}
	return 0;
}
bool MASSNODE::AddWindSegMass(long idWindSeg,double mass)
{
	int i,idle=-1;
	if(idWindSeg<=0)
		return false;
	for(i=0;i<4;i++)
	{
		if(xarrMassOfWindSeg[i].idWindSeg==idWindSeg)
		{
			xarrMassOfWindSeg[i].dfAllocMass+=mass;
			return true;
		}
		if(idle<0&&xarrMassOfWindSeg[i].idWindSeg==0)
			idle=i;
	}
	if(idle<0)
	{
		logerr.Log("0x%X归属风压段数量超过4个",handle);
		return false;
	}
	xarrMassOfWindSeg[idle].idWindSeg=idWindSeg;
	xarrMassOfWindSeg[idle].dfAllocMass+=mass;
	return true;
}
double MASSNODE::get_dfSummMass() const
{
	double mass=0;
	for(int i=0;i<4;i++)
	{
		//if(xarrMassOfWindSeg[i].idWindSeg>0)
		mass+=xarrMassOfWindSeg[i].dfAllocMass;
	}
	return mass;
}

BYTE WINDCALSEG::CURR_VIBR_FAI_TYPE = WINDCALSEG::FAI_TYPE_X;	//
double WINDCALSEG::get_dfCurrFai() {
	return CURR_VIBR_FAI_TYPE == 0 ? dfVibrFaiX : dfVibrFaiY;
}
void WINDSEGBETAZCAL_EXPORT::ELEMREC::InitFromFemElement(CElement* pFemElem,bool blCalByBodySeg/*=true*/)
{
	startPointI = min(pFemElem->pStart->point_i, pFemElem->pEnd->point_i);
	endPointI = max(pFemElem->pStart->point_i, pFemElem->pEnd->point_i);
	area = pFemElem->dfLength*pFemElem->GetWidth();
	if (pFemElem->pOrgPole->size_idClassType == CLS_GROUPLINEANGLE &&	//对角对称双角钢组合
		(pFemElem->pOrgPole->size_cSubClassType == 'D' || pFemElem->pOrgPole->size_cSubClassType == 'T' ||
			pFemElem->pOrgPole->size_cSubClassType == 'X'))//十字对称四角钢组合
	{
		area *= 2;
		if (blCalByBodySeg&&CPreProcessor::ActiveLoadCode->CodeSerial() == LOAD_CODES::DLT_2018)
			area *= 1.1;
	}
	size = pFemElem->pOrgPole->GetSizeSpec();
	length = pFemElem->Length();
	cMaterial = pFemElem->pOrgPole->cMaterial;
	//muS = pWindSeg->m_fProfileModeCoef;
	//efficArea = pRec->area*pRec->muS;
	double dfUnitWeight = globalLibrary.GetWeightOf1Meter(pFemElem->pOrgPole->size_wide, pFemElem->pOrgPole->size_thick, 0, pFemElem->pOrgPole->size_idClassType, pFemElem->pOrgPole->size_cSubClassType);
	mass = pFemElem->dfLength*dfUnitWeight*0.001;
	massIncRaiseCoef=mass*pFemElem->pOrgPole->m_fWeightRaiseCoef;
}
WINDSEGBETAZCAL_EXPORT::WINDSEGBETAZCAL_EXPORT()
{
	windSegI = 0;	//风压段号
	cType = 0;		//风压段类型0:八点塔身1:多点轮廓2:三角塔身
	cCalStyle = 0;	//0.塔身方式计算1.X方向水平横担2.Y方向水平横担
	cMirType = 0;	//对称信息0.无对称1.左右对称
	dfNamedPosH = 0;
	Wo = 0;	//kN/m2
	A_a = A_b = Af_a = Af_b = 0;	//m2
	areaOfFrontAngles=areaOfFrontTubes=areaOfSideAngles=areaOfSideTubes=0;
	fai_a = fai_b = 0;
	b_a_a = b_a_b = yita_a = yita_b = 1;
	muZ = muSa = muSb = betaZ = K = Cqi_a = Cqi_b = 0;
	Wsa = Wsb = Wsc = 0;	//kN
	Xcoef_Wsa = Xcoef_Wsb = Xcoef_Wsc = 0;
	Ycoef_Wsa = Ycoef_Wsb = Ycoef_Wsc = 0;
	Wx = Wy = 0;
}


double CalBzConstComponent(WINDCALSEG* xarrWindSeg, int count)
{
	int j;
	double dfSummVal = 0;
	double dfDenominator = 0;
	for (j = 0; j < count; j++)
	{
		xarrWindSeg[j].dfMusMuzVibrFaiIzAf = xarrWindSeg[j].dfMuS*xarrWindSeg[j].dfMuZ*xarrWindSeg[j].dfCurrFai*xarrWindSeg[j].Iz*xarrWindSeg[j].dfAf;
		dfDenominator += xarrWindSeg[j].massByKg*xarrWindSeg[j].dfCurrFai*xarrWindSeg[j].dfCurrFai;
	}
	for (j = 0; j < count; j++)
	{
		for (int ji = 0; ji < count; ji++)
		{

			double dfCohz = exp(-fabs(xarrWindSeg[j].dfNamedPosH - xarrWindSeg[ji].dfNamedPosH) / 60);
			dfSummVal += xarrWindSeg[j].dfMusMuzVibrFaiIzAf*xarrWindSeg[ji].dfMusMuzVibrFaiIzAf*dfCohz;
		}
	}
	return sqrt(dfSummVal) / dfDenominator;
}
bool CPreProcessor::CalTowerWindSegBetaZ_AllocMass(CLDSModule* pModule, CWindSegDispatcher* pDispatcher/* = NULL*/)
{
	CWindSegment* pWindSeg;
	for (pWindSeg = m_pTower->WindLoadSeg.GetFirst(); pWindSeg; pWindSeg = m_pTower->WindLoadSeg.GetNext())
	{
		if (pWindSeg->ValidNodeCount() == 0)
		{
			logerr.Log("风压段%d缺少轮廓节点", pWindSeg->iNo);
			continue;
		}
	}
	for (CLDSNode* pNode = result.nodeSet.GetFirst(); pNode; pNode = result.nodeSet.GetNext())
	{
		CNodeBodyLoad* pBodyLoad=result.hashNodeBodyLoads.GetValue(pNode->handle);
		MASSNODE* pMassNode=pDispatcher->hashNodeMassAllocInWindSeg.Add(pNode->handle);
		for(ELEM_BODYLOAD_ALLOC* pAlloc=pBodyLoad->hashElemAllocs.GetFirst();pAlloc;pAlloc=pBodyLoad->hashElemAllocs.GetNext())
		{
			CLDSLinePart* pRod=pAlloc->pElem->pOrgPole;
			if(!pModule->IsSonPart(pRod))
				continue;
			double dfUnitWeight=globalLibrary.GetWeightOf1Meter(pRod->size_wide,pRod->size_thick,0,pRod->size_idClassType,pRod->size_cSubClassType);
			double dfElemMass=pAlloc->pElem->dfLength*dfUnitWeight*0.001;
			double dfAllocMass=dfElemMass*pRod->m_fWeightRaiseCoef*pAlloc->fAllocCoef;
			if (m_pTower->m_bWeightAmplifyCoefIncBraceRods&&pRod->IsAuxPole())
				dfAllocMass=0;	//辅材重量增大效应已考虑到受力材放大系数中 wjh-2019.7.18
			if(pRod->IsAuxPole())
				pMassNode->dfSummBraceElemMass+=dfAllocMass;
			else
			{
				pMassNode->dfSummForceElemMass+=dfAllocMass;
				pMassNode->dfSummForceElemNetMass+=dfAllocMass/pRod->m_fWeightRaiseCoef;
			}
			pMassNode->dfSummStatMass+=dfAllocMass;
			//为避免塔身与横担衔接处主材分配到横担风压段中，先计算塔身类型风压段
			for(pWindSeg=m_pTower->WindLoadSeg.GetFirst();pWindSeg;pWindSeg=m_pTower->WindLoadSeg.GetNext())
			{
				if (pWindSeg->cType == 1)
					continue;	//多节点轮廓类型风压段一般用于描述横担或塔头
				int i,nValidNode=pWindSeg->ValidNodeCount();
				for(i=0;i<nValidNode;i++)
				{
					if (!pModule->IsSonPart(pWindSeg->nodePtrArr[i]))
						break;
				}
				if (nValidNode == 0 || i<nValidNode)
					continue;	//非本模型基本高的风压段
				long* pidWindSeg = pDispatcher->hashElemBelongWindSeg.GetValue(pAlloc->idElem);
				if(pidWindSeg!= NULL&&(*pidWindSeg)!= pWindSeg->iNo)
					continue;	//已统计到其余风压段
				if (pWindSeg->IsIncludeElem(m_pTower, pAlloc->pElem->pStart->xOriginalPos, pAlloc->pElem->pEnd->xOriginalPos))
				{
					pDispatcher->hashElemBelongWindSeg.SetValue(pAlloc->idElem, pWindSeg->iNo);
					pMassNode->AddWindSegMass(pWindSeg->iNo, dfAllocMass);
				}
				else if (pWindSeg->pMirArmWindSeg&&
					pWindSeg->pMirArmWindSeg->IsIncludeElem(m_pTower, pAlloc->pElem->pStart->xOriginalPos, pAlloc->pElem->pEnd->xOriginalPos))
				{
					pDispatcher->hashElemBelongWindSeg.SetValue(pAlloc->idElem, (DWORD)pWindSeg->pMirArmWindSeg->iNo);
					pMassNode->AddWindSegMass((DWORD)pWindSeg->pMirArmWindSeg->iNo, dfAllocMass);
				}
			}
			//计算横担类型风压段
			for (pWindSeg = m_pTower->WindLoadSeg.GetFirst(); pWindSeg; pWindSeg = m_pTower->WindLoadSeg.GetNext())
			{
				if (pWindSeg->cType != 1)
					continue;
				int i, nValidNode = pWindSeg->ValidNodeCount();
				for (i = 0; i < nValidNode; i++)
				{
					if (!pModule->IsSonPart(pWindSeg->nodePtrArr[i]))
						break;
				}
				if (nValidNode == 0 || i < nValidNode)
					continue;	//非本模型基本高的风压段
				long* pidWindSeg = pDispatcher->hashElemBelongWindSeg.GetValue(pAlloc->idElem);
				if (pidWindSeg != NULL && (*pidWindSeg) != pWindSeg->iNo)
					continue;	//已统计到其余风压段
				if (pWindSeg->IsIncludeElem(m_pTower, pAlloc->pElem->pStart->xOriginalPos, pAlloc->pElem->pEnd->xOriginalPos))
				{
					pDispatcher->hashElemBelongWindSeg.SetValue(pAlloc->idElem, pWindSeg->iNo);
					pMassNode->AddWindSegMass(pWindSeg->iNo, dfAllocMass);
				}
				else if (pWindSeg->pMirArmWindSeg&&
					pWindSeg->pMirArmWindSeg->IsIncludeElem(m_pTower, pAlloc->pElem->pStart->xOriginalPos, pAlloc->pElem->pEnd->xOriginalPos))
				{
					pDispatcher->hashElemBelongWindSeg.SetValue(pAlloc->idElem, (DWORD)pWindSeg->pMirArmWindSeg->iNo);
					pMassNode->AddWindSegMass((DWORD)pWindSeg->pMirArmWindSeg->iNo, dfAllocMass);
				}
			}
		}
	}
	return true;
}
double Find_MuZ(double high, char cLandEnv);//defined in WindLoad.cpp
bool CPreProcessor::CalTowerWindSegBetaZ_Final(CLDSModule *pModule,
	double dfMaxWindV, double f1, CWindSegDispatcher* pDispatcher)
{
	CTower *pTower = this->m_pTower;
	double Wo= pDispatcher->W0=dfMaxWindV*dfMaxWindV /1600;	//计算风振系数时，基准风压按30m/s考虑
	double alfa = 0.15;	//默认按'B'类地貌取值
	double kw = 1.0;	//默认按'B'类地貌取值
	double dfI10 = 0.14;//对应10m高处湍流强度
	if (pTower->m_cLandscape == 'A')
	{
		alfa = 0.12;
		kw = 1.284;
		dfI10 = 0.12;
	}
	else if (pTower->m_cLandscape == 'B')
	{
		alfa = 0.15;
		kw = 1.0;
		dfI10 = 0.14;
	}
	else if (pTower->m_cLandscape == 'C')
	{
		alfa = 0.22;
		kw = 0.544;
		dfI10 = 0.23;
	}
	else if (pTower->m_cLandscape == 'D')
	{
		alfa = 0.30;
		kw = 0.262;
		dfI10 = 0.39;
	}
	double H = pModule->GetModuleScopeZ();	//当前呼高总高
	pDispatcher->epsilonT = 1.0;	//杆塔脉动折减系数,>=40m时取值1.0,
	pDispatcher->dfI10 = dfI10;
	if (H <= 20000)	//<=20m
		pDispatcher->epsilonT = 0.6;
	else if (H < 40000)//<40m时按插值计算
		pDispatcher->epsilonT = 0.6 + 0.4*(H / 20000 - 1);
	
	double g=2.5;	//峰值因子，一般取2.5
	double x1 = max(5, 30 * f1 / sqrt(kw*Wo));
	double dfDampCoef = 0.02;	//钢结构一阶组尼比ζ一般取值0.02
	pDispatcher->dfSquareR = Pi * x1*x1 / (6 * dfDampCoef*pow(1 + x1 * x1, 4.0 / 3));	//共振因子R
	double dfQuareRootR = sqrt(1+ pDispatcher->dfSquareR);
	//double betaz=1+2*g*epsilonT*I10*Bz*sqrt(1+R*R);
	WINDCALSEG _arrWindCalSegments[32];
	ARRAY_LIST<WINDCALSEG> xarrWindCalSeg(_arrWindCalSegments, 32);
	for (CWindSegment *pWindSeg = pTower->WindLoadSeg.GetFirst(); pWindSeg; pWindSeg = pTower->WindLoadSeg.GetNext())
	{
		WINDSEGBETAZCAL_EXPORT* pExport = pDispatcher->hashWindSegExport.GetValue(pWindSeg->iNo);
		if (pExport == NULL)
			continue;
		WINDCALSEG* pCalSeg = xarrWindCalSeg.append(pExport->xBetazCal);
		pCalSeg->pWindSegment = pWindSeg;
		char cSymbol = 'S';
		if (pWindSeg->cType == 1)
			cSymbol = 'A';
		else if (pWindSeg->nodePtrArr[1] != NULL && pWindSeg->nodePtrArr[1]->IsLegObj())
			cSymbol = 'L';
		pCalSeg->szLabel.Printf("%C-%d", cSymbol, pWindSeg->iNo);
		double dfAllocMass=0;
		double dfSummMass=0;	//总质量
		GEPOINT xSummWeightPosition;	//总加权位置
		GEPOINT xSummWeightVibrFai;
		MASSNODE* pMassNode;
		for(pMassNode=pDispatcher->hashNodeMassAllocInWindSeg.GetFirst();pMassNode;pMassNode=pDispatcher->hashNodeMassAllocInWindSeg.GetNext())
		{
			if((dfAllocMass=pMassNode->MassInWindSeg(pWindSeg->iNo))==0)
				continue;
			CLDSNode* pNode=m_pTower->FromNodeHandle(pMassNode->handle);
			if(pNode==NULL)
				continue;
			dfSummMass+=dfAllocMass;
			xSummWeightPosition+= dfAllocMass * pNode->xOriginalPos;
			xSummWeightVibrFai += dfAllocMass * pNode->m_offsetAmplitude;
		}
		if (dfSummMass <= EPS)
		{
			logerr.Log("风压段<%s>提取质量为0，数据错误", (char*)pCalSeg->szLabel);
			continue;
		}
		pCalSeg->massByKg = dfSummMass;	//风压段团集质量(kg)
		//计算风压段加权平均质心点位置
		GEPOINT xNamedCenter=xSummWeightPosition/dfSummMass;
		GEPOINT xNamedVibrFai = xSummWeightVibrFai / dfSummMass;
		pCalSeg->dfNamedPosH = pExport->dfNamedPosH;	//风压段名义高度Z值,单位m，未来应统一为xNameCenter.z wjh-2019.7.4
		//pCalSeg->dfNamedPosH = pDispatcher->m_fLowestBtmZ - xNamedCenter.z;	//风压段名义高度Z值,单位m，未来应统一为xNameCenter.z wjh-2019.7.4
		pCalSeg->dfMuZ = pExport->muZ;	//风压段体型系数与风压高度变化系数
		pCalSeg->Iz = pow(pCalSeg->dfNamedPosH / 10, -alfa);	//湍流强度系数
		pCalSeg->dfVibrFaiX=xNamedVibrFai.x;//X向振型系数
		pCalSeg->dfVibrFaiY=xNamedVibrFai.y;//Y向振型系数
		if (WINDCALSEG::CURR_VIBR_FAI_TYPE == WINDCALSEG::FAI_TYPE_X)
		{
			if (cSymbol == 'A'&&pWindSeg->cCalStyle>0)
			{
				pCalSeg->dfAf = 0.45*pExport->Af_b* pWindSeg->CqiSide;
				pCalSeg->dfMuS = pExport->muSb;		//90度侧面风吹体型系数
				pExport->b2aSD = pExport->b2aFR;
			}
			else
			{
				pCalSeg->dfAf = pExport->Af_a* pWindSeg->CqiSide;	//侧迎风面投影面积(m2)
				pCalSeg->dfMuS = pExport->muSa;		//90度侧面风吹体型系数
			}
		}
		else
		{
			pCalSeg->dfAf = pExport->Af_b* pWindSeg->CqiFront;	//正迎风面投影面积(m2)
			pCalSeg->dfMuS = pExport->muSb;	// 0度正面风吹体型系数
		}
		if (pWindSeg->pMirArmWindSeg != NULL)
		{
			pExport=pDispatcher->hashWindSegExport.GetValue(pWindSeg->pMirArmWindSeg->iNo);
			WINDCALSEG* pMirCalSeg=xarrWindCalSeg.append(pExport->xBetazCal);
			pMirCalSeg->pWindSegment = pWindSeg->pMirArmWindSeg;
			pMirCalSeg->szLabel=pCalSeg->szLabel;
			if (pWindSeg->cMirType == 1)
				pMirCalSeg->szLabel.Append("!Y");
			else if(pWindSeg->cMirType==2)
				pMirCalSeg->szLabel.Append("!X");
			dfAllocMass = dfSummMass = 0;
			xSummWeightPosition.Set();	//总加权位置
			xSummWeightVibrFai.Set();
			for (pMassNode = pDispatcher->hashNodeMassAllocInWindSeg.GetFirst(); pMassNode; pMassNode = pDispatcher->hashNodeMassAllocInWindSeg.GetNext())
			{
				if ((dfAllocMass = pMassNode->MassInWindSeg(pWindSeg->iNo)) == 0)
					continue;
				CLDSNode* pNode = m_pTower->FromNodeHandle(pMassNode->handle);
				if (pNode == NULL)
					continue;
				dfSummMass += dfAllocMass;
				xSummWeightPosition += dfAllocMass * pNode->xOriginalPos;
				xSummWeightVibrFai += dfAllocMass * pNode->m_offsetAmplitude;
			}
			if (dfSummMass <= EPS)
			{
				logerr.Log("风压段<%s>提取质量为0，数据错误", (char*)pMirCalSeg->szLabel);
				continue;
			}
			pMirCalSeg->massByKg = dfSummMass;	//风压段团集质量(kg)
			//计算风压段加权平均质心点位置
			GEPOINT xNamedCenter = xSummWeightPosition / dfSummMass;
			GEPOINT xNamedVibrFai = xSummWeightVibrFai / dfSummMass;
			pMirCalSeg->dfNamedPosH = pExport->dfNamedPosH;	//风压段名义高度Z值,单位m，未来应统一为xNameCenter.z wjh-2019.7.4
			//pMirCalSeg->dfNamedPosH = pDispatcher->m_fLowestBtmZ - xNamedCenter.z;	//风压段名义高度Z值,单位m，未来应统一为xNameCenter.z wjh-2019.7.4
			pMirCalSeg->dfMuZ = pExport->muZ;	//风压段体型系数与风压高度变化系数
			pMirCalSeg->Iz = pow(pMirCalSeg->dfNamedPosH / 10, -alfa);	//湍流强度系数
			pMirCalSeg->dfVibrFaiX = xNamedVibrFai.x;//X向振型系数
			pMirCalSeg->dfVibrFaiY = xNamedVibrFai.y;//Y向振型系数
			if (WINDCALSEG::CURR_VIBR_FAI_TYPE == WINDCALSEG::FAI_TYPE_X)
			{
				if (cSymbol == 'A'&&pWindSeg->cCalStyle>0)
				{
					pMirCalSeg->dfAf = 0.45*pExport->Af_b* pWindSeg->CqiSide;
					pMirCalSeg->dfMuS = pExport->muSb;		//90度侧面风吹体型系数
					pExport->b2aSD = pExport->b2aFR;
				}
				else
				{
					pMirCalSeg->dfAf = pExport->Af_a* pWindSeg->CqiSide;	//侧迎风面投影面积(m2)
					pMirCalSeg->dfMuS = pExport->muSa;		//90度侧面风吹体型系数
				}
			}
			else
			{
				pMirCalSeg->dfAf = pExport->Af_b* pWindSeg->CqiFront;	//正迎风面投影面积(m2)
				pMirCalSeg->dfMuS = pExport->muSb;		// 0度正面风吹体型系数
			}
		}
		//按STower赋值验证公式计算是否有误
		/*if (abs(pWindSeg->iNo) == 1)
		{
			pCalSeg->dfAf = 3.097;
			pCalSeg->dfMuS = 2.324;
			pCalSeg->dfVibrFaiX = 0.997;
			pCalSeg->massByKg = 3944.47;
		}
		else if (abs(pWindSeg->iNo) == 2)
		{
			pCalSeg->dfAf = 4.546;
			pCalSeg->dfMuS = 1.738;
			pCalSeg->dfVibrFaiX = 0.567;
			pCalSeg->massByKg = 11649.492;
		}
		else if (abs(pWindSeg->iNo) == 3)
		{
			pCalSeg->dfAf = 3.723;
			pCalSeg->dfMuS = 2.262;
			pCalSeg->dfVibrFaiX = 0.571;
			pCalSeg->massByKg = 6873.313;
		}
		else if (pWindSeg->iNo == 4)
		{
			pCalSeg->dfAf = 3.343;
			pCalSeg->dfMuS = 2.325;
			pCalSeg->dfVibrFaiX = 0.995;
			pCalSeg->massByKg = 2954.804;
		}
		else if (pWindSeg->iNo == 5)
		{
			pCalSeg->dfAf = 2.949;
			pCalSeg->dfMuS = 2.402;
			pCalSeg->dfVibrFaiX = 0.848;
			pCalSeg->massByKg = 1948.885;
		}
		else if (pWindSeg->iNo == 6)
		{
			pCalSeg->dfAf = 3.845;
			pCalSeg->dfMuS = 2.313;
			pCalSeg->dfVibrFaiX = 0.728;
			pCalSeg->massByKg = 4065.306;
		}
		else if (pWindSeg->iNo == 7)
		{
			pCalSeg->dfAf = 7.557;
			pCalSeg->dfMuS = 1.984;
			pCalSeg->dfVibrFaiX = 0.621;
			pCalSeg->massByKg = 11494.738;
		}
		else if (pWindSeg->iNo == 8)
		{
			pCalSeg->dfAf = 8.216;
			pCalSeg->dfMuS = 2.046;
			pCalSeg->dfVibrFaiX = 0.512;
			pCalSeg->massByKg = 11674.459;
		}
		else if (pWindSeg->iNo == 9)
		{
			pCalSeg->dfAf = 3.772;
			pCalSeg->dfMuS = 2.157;
			pCalSeg->dfVibrFaiX = 0.411;
			pCalSeg->massByKg = 5014.642;
		}
		else if (pWindSeg->iNo == 10)
		{
			pCalSeg->dfAf = 7.484;
			pCalSeg->dfMuS = 2.281;
			pCalSeg->dfVibrFaiX = 0.371;
			pCalSeg->massByKg = 9869.051;
		}
		else if (pWindSeg->iNo == 23)
		{
			pCalSeg->massByKg = 11803.472;
			pCalSeg->dfAf = 8.361;
			pCalSeg->dfMuS = 2.358;
			pCalSeg->dfVibrFaiX = 0.295;
		}
		else if (pWindSeg->iNo == 24)
		{
			pCalSeg->massByKg = 17842.625;
			pCalSeg->dfAf = 12.713;
			pCalSeg->dfMuS = 2.408;
			pCalSeg->dfVibrFaiX = 0.187;
		}
		else if (pWindSeg->iNo == 25)
		{
			pCalSeg->massByKg = 19790.749;
			pCalSeg->dfAf = 13.536;
			pCalSeg->dfMuS = 2.466;
			pCalSeg->dfVibrFaiX = 0.148;
		}
		else if (pWindSeg->iNo == 26)
		{
			pCalSeg->dfNamedPosH = 23.05;
			pCalSeg->dfMuZ = 1.285;
			pCalSeg->massByKg = 30241.515;
			pCalSeg->dfAf = 19.306;
			pCalSeg->dfMuS = 2.470;
			pCalSeg->dfVibrFaiX = 0.085;
		}
		else if (pWindSeg->iNo == 27)
		{
			pCalSeg->dfNamedPosH = 9.4;
			pCalSeg->dfMuZ = 1.0;
			pCalSeg->dfAf = 32.949;
			pCalSeg->dfMuS = 2.599;
			pCalSeg->dfVibrFaiX = 0.027;
			pCalSeg->massByKg = 38298.886;
		}
		if (pMirCalSeg)
		{
			pMirCalSeg->dfNamedPosH=pCalSeg->dfNamedPosH;
			pMirCalSeg->dfMuZ= pCalSeg->dfMuZ;
			pMirCalSeg->dfAf = pCalSeg->dfAf;
			pMirCalSeg->dfMuS = pCalSeg->dfMuS;
			pMirCalSeg->dfVibrFaiX = pCalSeg->dfVibrFaiX;
			pMirCalSeg->massByKg = pCalSeg->massByKg;
		}*/
	}
	/*	手动核算公式CalBzConstComponent正确性的临时代码 wjh-2019.7.7
	WINDCALSEG xarrTmpSegs[3];
	xarrTmpSegs[0].massByKg = 1.0;
	xarrTmpSegs[0].dfMuS = 1.25;
	xarrTmpSegs[0].dfMuZ = 1.0;
	xarrTmpSegs[0].dfVibrFaiX = 0.8;
	xarrTmpSegs[0].dfAf = 1.0;
	xarrTmpSegs[0].Iz = 1.0;
	xarrTmpSegs[0].dfNamedPosH = 10;
	xarrTmpSegs[0].dfMusMuzVibrFaiIzAf = 1.0;

	xarrTmpSegs[1].massByKg = 1.0;
	xarrTmpSegs[1].dfMuS = 1.25;
	xarrTmpSegs[1].dfMuZ = 1.0;
	xarrTmpSegs[1].dfVibrFaiX = 8;
	xarrTmpSegs[1].dfAf = 1.0;
	xarrTmpSegs[1].Iz = 1.0;
	xarrTmpSegs[1].dfNamedPosH = 20;
	xarrTmpSegs[1].dfMusMuzVibrFaiIzAf = 10;

	xarrTmpSegs[2].massByKg = 1.0;
	xarrTmpSegs[2].dfMuS = 1.25;
	xarrTmpSegs[2].dfMuZ = 1.0;
	xarrTmpSegs[2].dfVibrFaiX = 80;
	xarrTmpSegs[2].dfAf = 1.0;
	xarrTmpSegs[2].Iz = 1.0;
	xarrTmpSegs[2].dfNamedPosH = 30;
	xarrTmpSegs[2].dfMusMuzVibrFaiIzAf = 100;
	double testConst = CalBzConstComponent(xarrTmpSegs, 3);
	*/
	double under, dfBzConstComponent = CalBzConstComponent(xarrWindCalSeg.m_pData, xarrWindCalSeg.GetSize());
	for (int i = 0; i < xarrWindCalSeg.GetSize(); i++)
	{
		WINDSEGBETAZCAL_EXPORT* pWindSegExport = pDispatcher->hashWindSegExport.GetValue(xarrWindCalSeg[i].pWindSegment->iNo);
		if ((under = xarrWindCalSeg[i].dfMuS*xarrWindCalSeg[i].dfMuZ*xarrWindCalSeg[i].dfAf) < EPS)
		{
			logerr.Log("风压段<%s>风振系数计算失败", (char*)xarrWindCalSeg[i].szLabel);
			if (pWindSegExport)	//提前对风压段名称进行赋值，以免后续代码中断时输出报告中缺少名称
				pWindSegExport->xBetazCal.szLabel = xarrWindCalSeg[i].szLabel;
			continue;
		}
		double dfBzVarComponent = xarrWindCalSeg[i].massByKg*xarrWindCalSeg[i].dfCurrFai/ under;
		xarrWindCalSeg[i].Bz = dfBzVarComponent * dfBzConstComponent;
		xarrWindCalSeg[i].dfBetaZ = 1 + 2 * g*pDispatcher->epsilonT*dfI10*xarrWindCalSeg[i].Bz*dfQuareRootR;
		pWindSegExport->xBetazCal = xarrWindCalSeg[i];
	}
	return true;
}
CWindSegDispatcher::CWindSegDispatcher()
{
	dfSquareR = 0;
	epsilonT=1.0;	//风振系数计算时根据杆塔全高取值的杆塔风荷载脉动折减系数
	dfI10 = 0.14;	//对应10m高处湍流强度
	W0=0;
	m_fLowestBtmZ=m_fHighestTopZ=0;
}

static int _LocalCompareMassNode(const MASSNODE& item1, const MASSNODE& item2)
{
	if (item1.pNode == NULL && item2.pNode != NULL)
		return -1;
	else if (item1.pNode != NULL && item2.pNode == NULL)
		return 1;
	else if (item1.pNode == NULL && item2.pNode == NULL)
		return 0;
	else if (item1.pNode->pointI > item2.pNode->pointI)
		return 1;
	else if (item1.pNode->pointI < item2.pNode->pointI)
		return -1;
	else
		return 0;
}
struct _BETAZ_H{
	double zHigh;	//高度
	double dfBetaZ;	//风振系数
	_BETAZ_H(double z=0,double betaz=1){
		zHigh=z;
		dfBetaZ=betaz;
	}
};
static int _LocalCompareBetaZH(const _BETAZ_H& item1,const _BETAZ_H& item2)
{
	if(item1.zHigh<item2.zHigh)
		return -1;
	else if(item1.zHigh>item2.zHigh)
		return 1;
	else
		return 0;
}
static double _InterpBetazByHigh(_BETAZ_H* xarrBetaZ,int count,double zHigh)
{	//对其余呼高风压段按高度进行插值计算
	for(int i=0;i<count;i++)
	{
		if(zHigh<xarrBetaZ[i].zHigh)
		{
			if(i==0)	//小于最低插值点高度
				return xarrBetaZ[0].dfBetaZ;
			else
				return xarrBetaZ[i-1].dfBetaZ+(xarrBetaZ[i].dfBetaZ-xarrBetaZ[i-1].dfBetaZ)*
					(zHigh-xarrBetaZ[i-1].zHigh)/(xarrBetaZ[i].zHigh-xarrBetaZ[i-1].zHigh);
		}
	}
	return xarrBetaZ[count-1].dfBetaZ;
}
void CWindSegDispatcher::CreateBetaZCalReport(FILE* fp, CLDSModule* pCurrCalHeight,
	double frequency, double dfBetaZCalWindV, CTower* pTowerModel/*=NULL*/)
{
	//pModule->GetModuleScopeZ();
	CTime t = CTime::GetCurrentTime();
	CString sTime = t.Format("%Y年%m月%d日%H时%M分");
	fprintf(fp, "************************************************************************\n");
	fprintf(fp, "*                                                                      *\n");
	fprintf(fp, "*  LDS杆塔风振系数计算 - (C) 2002-2019 北京信狐天诚软件科技有限公司    *\n");
	fprintf(fp, "*                                                                      *\n");
	fprintf(fp, "*                        杆塔风振系数计算书                            *\n");
	fprintf(fp, "************************************************************************\n");
	fprintf(fp, "\n");
	if(pTowerModel !=NULL)
		fprintf(fp, "杆塔型号：%s\n", pTowerModel->m_sTaType);
	else
		fprintf(fp, "杆塔型号：  \n");
	fprintf(fp, "呼高：%s\n", pCurrCalHeight->description);
	fprintf(fp, "振型特性：f1=%.3f, T=%.3f(s)\n", frequency, 1 / frequency);
	//fprintf(fp,"风向角：%d°\n",pStatus->WindLoad.azimuth);
	fprintf(fp, "计算风速：%.1fm/s\n", dfBetaZCalWindV);
	char cLandscape = pTowerModel != NULL ? pTowerModel->m_cLandscape : 'B';
	double I10 = 0.14;
	if (cLandscape == 'A')
	{
		fprintf(fp, "地貌类型：A:近海或沙漠平坦地区\n");
		I10 = 0.12;
	}
	else if (cLandscape == 'B')
	{
		fprintf(fp, "地貌类型：B:内陆建筑物稀疏地区\n");
		I10 = 0.14;
	}
	else if (cLandscape == 'C')
	{
		fprintf(fp, "地貌类型：C:有密集建筑群市区\n");
		I10 = 0.23;
	}
	else //if(cLandscape=='D')
	{
		fprintf(fp, "地貌类型：D:有密集建筑群和高层建筑市区\n");
		I10 = 0.39;
	}
	_BETAZ_H _xarrBetaZ[32];
	ARRAY_LIST<_BETAZ_H> xarrBetaZ(_xarrBetaZ,32);
	fprintf(fp, "\n==================杆塔风振系数简明计算过程报告 =======================\n");
	fprintf(fp, "风压段名    Zi       mi      φ1i  μsi  μzi  Izi     Ai   Bz    βz\n");
	double zHighest=0,zBottom=0;
	double H=pCurrCalHeight->GetModuleScopeZ(&zHighest,&zBottom);
	H*=0.001;
	zHighest*=0.001;
	zBottom*=0.001;
	for (WINDSEGBETAZCAL_EXPORT *pExp = hashWindSegExport.GetFirst(); pExp; pExp = hashWindSegExport.GetNext())
	{
		CXhChar16 label = pExp->xBetazCal.szLabel;
		label.ResizeLength(10);
		double under = pExp->xBetazCal.dfMuS*pExp->xBetazCal.dfMuZ*pExp->xBetazCal.dfAf;
		double dfVarComponent = pExp->xBetazCal.massByKg*pExp->xBetazCal.dfCurrFai / under;
		fprintf(fp, "%s %5.1f %10.3f %5.3f %5.3f %5.3f %5.3f %6.3f %5.3f %5.3f\n", (char*)label, pExp->xBetazCal.dfNamedPosH, pExp->xBetazCal.massByKg, pExp->xBetazCal.dfCurrFai,
			pExp->xBetazCal.dfMuS, pExp->xBetazCal.dfMuZ, pExp->xBetazCal.Iz, pExp->xBetazCal.dfAf, pExp->xBetazCal.Bz, pExp->xBetazCal.dfBetaZ);
		if(pExp->cCalStyle==0)	//塔身段
			xarrBetaZ.append(_BETAZ_H(H-pExp->dfNamedPosH+zHighest,pExp->xBetazCal.dfBetaZ));
	}
	CQuickSort<_BETAZ_H>::QuickSort(_xarrBetaZ,xarrBetaZ.GetSize(),_LocalCompareBetaZH);
	fprintf(fp, "\n==================杆塔风振系数计算结果报告 =======================\n");
	fprintf(fp, "风压段名                            βz\n");
	for(CWindSegment* pWindSeg=pTowerModel->WindLoadSeg.GetFirst();pWindSeg;pWindSeg=pTowerModel->WindLoadSeg.GetNext())
	{
		CXhChar50 szLabel;
		char cSymbol = 'S';
		if (pWindSeg->cType == 1)
			cSymbol = 'A';
		else if (pWindSeg->nodePtrArr[1] != NULL && pWindSeg->nodePtrArr[1]->IsLegObj())
			cSymbol = 'L';
		szLabel.Printf("%C-%d", cSymbol, pWindSeg->iNo);
		szLabel.Append('{');
		int i,nNodeCount=pWindSeg->StatValidNodeCount(pTowerModel);
		double dfSummH=0;
		for(i=0;i<nNodeCount;i++)
		{
			dfSummH+=pWindSeg->nodePtrArr[i]->xOriginalPos.z;
			CXhChar16 nodelabel("%d",pWindSeg->nodePtrArr[i]->pointI);
			szLabel.Append(nodelabel,(i>0?'-':0));
		}
		szLabel.Append('}');
		szLabel.ResizeLength(30);
		WINDSEGBETAZCAL_EXPORT* pExport=hashWindSegExport.GetValue(pWindSeg->iNo);
		double betaz=pExport!=NULL?pExport->xBetazCal.dfBetaZ:0;
		if(betaz==0)
		{
			betaz=_InterpBetazByHigh(_xarrBetaZ,xarrBetaZ.GetSize(),0.001*dfSummH/nNodeCount);
			szLabel.Append("-->");
		}
		else
			szLabel.Append("   ");
		fprintf(fp,"%s  %5.3f\n",(char*)szLabel,betaz);
	}
	//预演计算当前工况下各风压段的风荷载值
	try {
		for (MASSNODE* pMassNode = hashNodeMassAllocInWindSeg.GetFirst();
			pMassNode != NULL; pMassNode = hashNodeMassAllocInWindSeg.GetNext())
			pMassNode->pNode = pTowerModel->FromNodeHandle(pMassNode->handle);
		for (WINDSEGBETAZCAL_EXPORT *pExport = hashWindSegExport.GetFirst(); pExport; pExport = hashWindSegExport.GetNext())
		{
			fprintf(fp, "\n================风压段<%s> ======================\n", (char*)pExport->xBetazCal.szLabel);
			fprintf(fp, "\n一、基本信息\n");
			if (pExport->cType == 0)
				fprintf(fp, "挡风面定义方式：八点分配塔身风压段\n");
			else if (pExport->cType == 1)
				fprintf(fp, "挡风面定义方式：多点轮廓面风压段\n");
			else
				fprintf(fp, "挡风面定义方式：三角塔身风压段\n");
			if (pExport->cCalStyle == 0)
				fprintf(fp, "计算类型：塔身\n");
			else if (pExport->cCalStyle == 1)
				fprintf(fp, "计算类型：X向水平横担\n");
			else if (pExport->cCalStyle == 2)
				fprintf(fp, "计算类型：Y向水平横担\n");
			if (pExport->cMirType == 0)
				fprintf(fp, "对称方式：无\n");
			else if (pExport->cMirType == 1)
				fprintf(fp, "对称方式：Y轴对称风压段\n");
			else if (pExport->cMirType == 2)
				fprintf(fp, "对称方式：X轴对称风压段\n");
			fprintf(fp, "\n二、塔面水平风荷载计算\n");
			fprintf(fp, "                迎风面杆件明细清单\n");
			fprintf(fp, "杆件标识     规格     长度(mm) 挡风面积(m2) 体型系数 正面 侧面 质量(kg) 加权质量(kg)\n");
			fprintf(fp, "------------------------------------------------------------------------\n");
			WINDSEGBETAZCAL_EXPORT::ELEMREC sumstat;
			//for(i=0;i<pExport->elemrecs.();i++)
			double dfSummRawMass=0,dfSummMassIncRaiseCoef=0,dfFrontArea=0,dfSideArea=0;
			for (WINDSEGBETAZCAL_EXPORT::ELEMREC* pRec = pExport->elemrecs.GetFirst(); pRec; pRec = pExport->elemrecs.GetNext())
			{
				CXhChar16 spec("%3.0fx%2.0f%C", pRec->size.wide, pRec->size.thick, pRec->cMaterial);
				if (pRec->size.idClassType == CLS_LINEANGLE || pRec->size.idClassType == CLS_GROUPLINEANGLE)
				{
					spec.InsertBefore(" L", 0);
					if (pRec->size.cSubClassType != 'L')
						spec[0] = pRec->size.cSubClassType;
				}
				else if (pRec->size.idClassType == CLS_LINETUBE)
					spec.InsertBefore("Φ", 0);//φ
				else if (pRec->size.idClassType == CLS_LINESLOT)
					spec.InsertBefore(" [", 0);
				else
					spec.InsertBefore(" U", 0);
				pRec->area = 0.001*ftoi(0.001*pRec->area);
				pRec->efficArea = 0.01*ftoi(100 * pRec->efficArea);
				char front = pRec->frontwind ? 'V' : ' ';
				char side = pRec->sidewind ? 'V' : ' ';
				if(WINDCALSEG::CURR_VIBR_FAI_TYPE==WINDCALSEG::FAI_TYPE_X&&side!='V')
					pRec->muS=0.0;
				else if(WINDCALSEG::CURR_VIBR_FAI_TYPE==WINDCALSEG::FAI_TYPE_Y&&front!='V')
					pRec->muS=0.0;
				fprintf(fp, "%5d-%5d %s %5.0f      %6.3f      %.2f     %c    %c   %6.3f  %6.3f\n", pRec->startPointI, pRec->endPointI, (char*)spec,
					pRec->length, pRec->area, pRec->muS, front, side,pRec->mass,pRec->massIncRaiseCoef);
				if(pRec->frontwind)
					dfFrontArea+=pRec->area;
				if(pRec->sidewind)
					dfSideArea+=pRec->area;
				dfSummRawMass+=pRec->mass;
				dfSummMassIncRaiseCoef+=pRec->massIncRaiseCoef;
			}
			fprintf(fp, "\n");
			if(WINDCALSEG::CURR_VIBR_FAI_TYPE==WINDCALSEG::FAI_TYPE_X)
				sumstat.area=dfSideArea>0?dfSideArea:dfFrontArea;//*0.45;
			else if(WINDCALSEG::CURR_VIBR_FAI_TYPE==WINDCALSEG::FAI_TYPE_Y)
				sumstat.area=dfFrontArea;
			double dfAreaOfAngles=pExport->areaOfFrontAngles*1e-6;
			double dfAreaOfTubes=pExport->areaOfFrontTubes*1e-6;
			if(WINDCALSEG::CURR_VIBR_FAI_TYPE==WINDCALSEG::FAI_TYPE_X)
			{
				if((dfAreaOfAngles=pExport->areaOfSideAngles*1e-6)==0)
					dfAreaOfAngles=pExport->areaOfFrontAngles*0.45*1e-6;
				if((dfAreaOfTubes=pExport->areaOfSideTubes*1e-6)==0)
					dfAreaOfTubes=pExport->areaOfFrontTubes*0.45*1e-6;
			}
			sumstat.area=dfAreaOfAngles+dfAreaOfTubes;
			fprintf(fp, "合计：  挡风面积:  %8.3f m2,其中 角钢面积: %.3f m2, 钢管面积: %.3f m2;\n", sumstat.area, dfAreaOfAngles,dfAreaOfTubes);
			fprintf(fp, "总质量: %.3f(kg), 总加权质量: %8.3f(kg)\n",dfSummRawMass,dfSummMassIncRaiseCoef);

			fprintf(fp, "基准风压：%.3fkN/m2\n", pExport->Wo);
			fprintf(fp, "BH  =%7.3fm;离地高\n", pExport->dfNamedPosH);
			if (WINDCALSEG::CURR_VIBR_FAI_TYPE == WINDCALSEG::FAI_TYPE_X)
			{
				double dfArea=pExport->A_a;
				double dfFai =pExport->fai_a;
				double dfYita=pExport->yita_a;
				double dfCqi =pExport->Cqi_a;
				double dfB2A =pExport->b_a_a;
				if (pExport->cCalStyle == 1)
				{
					dfArea=pExport->A_b*0.45;
					dfFai =pExport->fai_b;
					dfYita=pExport->yita_b;
					dfCqi =pExport->Cqi_b;
					dfB2A =pExport->b_a_b;
				}
				fprintf(fp, "A   =%6.3f  ;侧面轮廓投影面积,m2\n", dfArea);
				fprintf(fp, "Af  =%6.3f  ;侧面承受风压投影面积(含增大效应系数Cqi),m2\n", pExport->xBetazCal.dfAf);// Af_a);
				fprintf(fp, "Cqi =%.3f   ;侧面挡风面积增大系数\n", dfCqi);
				fprintf(fp, "φ  =%.3f   ;侧面挡风系数（填充率）\n", dfFai);
				fprintf(fp, "μz =%5.3f   ;风压高度变化系数\n", pExport->muZ);
				fprintf(fp, "a   =%.3f   ;侧面迎风面计算宽度,m\n", pExport->b2aSD.a*0.001);
				fprintf(fp, "b   =%.3f   ;侧面迎风面计算深度,m\n", pExport->b2aSD.b*0.001);
				fprintf(fp, "b/a =%.3f   ;侧面迎风面深度/迎风面宽度\n", dfB2A);
				fprintf(fp, "η  =%.3f   ;侧面背风面风载降低系数\n", dfYita);
				fprintf(fp, "μs =%.3f   ;侧面体型系数（已含1+η）\n", pExport->xBetazCal.dfMuS);// muSa);
			}
			else
			{
				fprintf(fp, "A   =%6.3f  ;正面轮廓投影面积,m2\n", pExport->A_b);
				fprintf(fp, "Af  =%6.3f  ;正面承受风压投影面积(含增大效应系数Cqi),m2\n", pExport->xBetazCal.dfAf);
				fprintf(fp, "Cqi =%.3f   ;正面挡风面积增大系数\n", pExport->Cqi_b);
				fprintf(fp, "φ  =%.3f   ;正面挡风系数（填充率）\n", pExport->fai_b);
				fprintf(fp, "μz =%5.3f   ;风压高度变化系数\n", pExport->muZ);
				fprintf(fp, "a   =%.3f   ;正面迎风面计算宽度,m\n", pExport->b2aFR.a*0.001);
				fprintf(fp, "b   =%.3f   ;正面迎风面计算深度,m\n", pExport->b2aFR.b*0.001);
				fprintf(fp, "b/a =%.3f   ;正面迎风面深度/迎风面宽度\n", pExport->b_a_b);
				fprintf(fp, "η  =%.3f   ;正面背风面风载降低系数\n", pExport->yita_b);
				fprintf(fp, "μs =%.3f   ;正面体型系数（已含1+η）\n", pExport->muSb);
			}
			fprintf(fp,"\n三、风振系数计算\n");
			fprintf(fp, "g   =2.5     ;峰值因子\n");
			fprintf(fp, "εt =%5.3f   ;杆塔风荷载脉动折减系数\n", epsilonT);
			fprintf(fp, "R^2 =%5.3f   ;共振因子平方值\n", dfSquareR);
			fprintf(fp, "Wo  =%5.3f   ;基准风压\n", W0);
			fprintf(fp, "I10 =%5.3f   ;10m高度名义湍流强度\n", dfI10);
			fprintf(fp, "Iz  =%5.3f   ;湍流强度沿高度的分布系数\n", pExport->xBetazCal.Iz);
			fprintf(fp, "Bzi =%5.3f   ;风压段背景因子\n", pExport->xBetazCal.Bz);
			fprintf(fp, "βz =%5.3f   ;风振系数计算过程βz=1+2g*εt*I10*Bzi*SQRT(1+R^2)\n", pExport->xBetazCal.dfBetaZ);
			fprintf(fp, "---------杆塔振型系数、等效质量与质心及相关系数计算\n");
			fprintf(fp, "风压段等效质量         mi =%.2f(kg)\n", pExport->xBetazCal.massByKg);
			fprintf(fp, "风压段质心高度         Zi =%.2f(m)\n", pExport->dfNamedPosH);
			fprintf(fp, "风压段质心等效振型系数 φ1=%.3f\n", pExport->xBetazCal.dfCurrFai);
			fprintf(fp, "Node    M(kg)  φ1      Pos.X(m)   Pos.Y(m)   Pos.Z(m) \n");
			fprintf(fp, "节点号  质量   振型系数 位置.X(m)  位置.Y(m)  位置.Z(m)\n");
			fprintf(fp, "------------------------------------------------------\n");
			MASSNODE _arrpool[1000];
			ARRAY_LIST<MASSNODE> xarrMassNodes(_arrpool, 1000);
			//CWindSegElemPicker *pFrontElemPicker = (CWindSegElemPicker*)pExport->xBetazCal.pWindSegment->pFrontElemPicker;
			double dfSegMass = 0;
			for (MASSNODE* pMassNode = hashNodeMassAllocInWindSeg.GetFirst();
				pMassNode != NULL; pMassNode = hashNodeMassAllocInWindSeg.GetNext())
			{
				if ((dfSegMass = pMassNode->MassInWindSeg(pExport->windSegI)) > 0)
					xarrMassNodes.append(*pMassNode);
			}
			CQuickSort<MASSNODE>::QuickSort(xarrMassNodes.m_pData, xarrMassNodes.GetSize(), _LocalCompareMassNode);
			double dfSummSegMass=0;
			for(int i=0;i<xarrMassNodes.GetSize();i++)
			{
				CLDSNode* pNode = xarrMassNodes[i].pNode;
				if (pNode == NULL)
				{
					logerr.Log("error");
					continue;
				}
				GEPOINT pos = pNode->xOriginalPos*0.001;
				double dfCurrFai = pNode->m_offsetAmplitude.x;
				dfSegMass = xarrMassNodes[i].MassInWindSeg(pExport->windSegI);
				fprintf(fp, "%6d %6.3f %9.6f %6.3f     %6.3f     %7.3f\n", pNode->pointI,dfSegMass, dfCurrFai,pos.x, pos.y, pos.z);
				dfSummSegMass+=dfSegMass;
			}
			fprintf(fp, "合计质量：%8.3f(kg)\n", dfSummSegMass);
		}
	}
	catch (char* sError)
	{
		AfxMessageBox(sError);
	}
	catch (CXhChar100 sError)
	{
		AfxMessageBox(sError);
	}
	catch (CString sError)
	{
		AfxMessageBox(sError);
	}
}
#endif