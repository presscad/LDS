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
		logerr.Log("0x%X������ѹ����������4��",handle);
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
	if (pFemElem->pOrgPole->size_idClassType == CLS_GROUPLINEANGLE &&	//�ԽǶԳ�˫�Ǹ����
		(pFemElem->pOrgPole->size_cSubClassType == 'D' || pFemElem->pOrgPole->size_cSubClassType == 'T' ||
			pFemElem->pOrgPole->size_cSubClassType == 'X'))//ʮ�ֶԳ��ĽǸ����
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
	windSegI = 0;	//��ѹ�κ�
	cType = 0;		//��ѹ������0:�˵�����1:�������2:��������
	cCalStyle = 0;	//0.����ʽ����1.X����ˮƽ�ᵣ2.Y����ˮƽ�ᵣ
	cMirType = 0;	//�Գ���Ϣ0.�޶Գ�1.���ҶԳ�
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
			logerr.Log("��ѹ��%dȱ�������ڵ�", pWindSeg->iNo);
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
				dfAllocMass=0;	//������������ЧӦ�ѿ��ǵ������ķŴ�ϵ���� wjh-2019.7.18
			if(pRod->IsAuxPole())
				pMassNode->dfSummBraceElemMass+=dfAllocMass;
			else
			{
				pMassNode->dfSummForceElemMass+=dfAllocMass;
				pMassNode->dfSummForceElemNetMass+=dfAllocMass/pRod->m_fWeightRaiseCoef;
			}
			pMassNode->dfSummStatMass+=dfAllocMass;
			//Ϊ����������ᵣ�νӴ����ķ��䵽�ᵣ��ѹ���У��ȼ����������ͷ�ѹ��
			for(pWindSeg=m_pTower->WindLoadSeg.GetFirst();pWindSeg;pWindSeg=m_pTower->WindLoadSeg.GetNext())
			{
				if (pWindSeg->cType == 1)
					continue;	//��ڵ��������ͷ�ѹ��һ�����������ᵣ����ͷ
				int i,nValidNode=pWindSeg->ValidNodeCount();
				for(i=0;i<nValidNode;i++)
				{
					if (!pModule->IsSonPart(pWindSeg->nodePtrArr[i]))
						break;
				}
				if (nValidNode == 0 || i<nValidNode)
					continue;	//�Ǳ�ģ�ͻ����ߵķ�ѹ��
				long* pidWindSeg = pDispatcher->hashElemBelongWindSeg.GetValue(pAlloc->idElem);
				if(pidWindSeg!= NULL&&(*pidWindSeg)!= pWindSeg->iNo)
					continue;	//��ͳ�Ƶ������ѹ��
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
			//����ᵣ���ͷ�ѹ��
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
					continue;	//�Ǳ�ģ�ͻ����ߵķ�ѹ��
				long* pidWindSeg = pDispatcher->hashElemBelongWindSeg.GetValue(pAlloc->idElem);
				if (pidWindSeg != NULL && (*pidWindSeg) != pWindSeg->iNo)
					continue;	//��ͳ�Ƶ������ѹ��
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
	double Wo= pDispatcher->W0=dfMaxWindV*dfMaxWindV /1600;	//�������ϵ��ʱ����׼��ѹ��30m/s����
	double alfa = 0.15;	//Ĭ�ϰ�'B'���òȡֵ
	double kw = 1.0;	//Ĭ�ϰ�'B'���òȡֵ
	double dfI10 = 0.14;//��Ӧ10m�ߴ�����ǿ��
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
	double H = pModule->GetModuleScopeZ();	//��ǰ�����ܸ�
	pDispatcher->epsilonT = 1.0;	//���������ۼ�ϵ��,>=40mʱȡֵ1.0,
	pDispatcher->dfI10 = dfI10;
	if (H <= 20000)	//<=20m
		pDispatcher->epsilonT = 0.6;
	else if (H < 40000)//<40mʱ����ֵ����
		pDispatcher->epsilonT = 0.6 + 0.4*(H / 20000 - 1);
	
	double g=2.5;	//��ֵ���ӣ�һ��ȡ2.5
	double x1 = max(5, 30 * f1 / sqrt(kw*Wo));
	double dfDampCoef = 0.02;	//�ֽṹһ������Ȧ�һ��ȡֵ0.02
	pDispatcher->dfSquareR = Pi * x1*x1 / (6 * dfDampCoef*pow(1 + x1 * x1, 4.0 / 3));	//��������R
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
		double dfSummMass=0;	//������
		GEPOINT xSummWeightPosition;	//�ܼ�Ȩλ��
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
			logerr.Log("��ѹ��<%s>��ȡ����Ϊ0�����ݴ���", (char*)pCalSeg->szLabel);
			continue;
		}
		pCalSeg->massByKg = dfSummMass;	//��ѹ���ż�����(kg)
		//�����ѹ�μ�Ȩƽ�����ĵ�λ��
		GEPOINT xNamedCenter=xSummWeightPosition/dfSummMass;
		GEPOINT xNamedVibrFai = xSummWeightVibrFai / dfSummMass;
		pCalSeg->dfNamedPosH = pExport->dfNamedPosH;	//��ѹ������߶�Zֵ,��λm��δ��ӦͳһΪxNameCenter.z wjh-2019.7.4
		//pCalSeg->dfNamedPosH = pDispatcher->m_fLowestBtmZ - xNamedCenter.z;	//��ѹ������߶�Zֵ,��λm��δ��ӦͳһΪxNameCenter.z wjh-2019.7.4
		pCalSeg->dfMuZ = pExport->muZ;	//��ѹ������ϵ�����ѹ�߶ȱ仯ϵ��
		pCalSeg->Iz = pow(pCalSeg->dfNamedPosH / 10, -alfa);	//����ǿ��ϵ��
		pCalSeg->dfVibrFaiX=xNamedVibrFai.x;//X������ϵ��
		pCalSeg->dfVibrFaiY=xNamedVibrFai.y;//Y������ϵ��
		if (WINDCALSEG::CURR_VIBR_FAI_TYPE == WINDCALSEG::FAI_TYPE_X)
		{
			if (cSymbol == 'A'&&pWindSeg->cCalStyle>0)
			{
				pCalSeg->dfAf = 0.45*pExport->Af_b* pWindSeg->CqiSide;
				pCalSeg->dfMuS = pExport->muSb;		//90�Ȳ���紵����ϵ��
				pExport->b2aSD = pExport->b2aFR;
			}
			else
			{
				pCalSeg->dfAf = pExport->Af_a* pWindSeg->CqiSide;	//��ӭ����ͶӰ���(m2)
				pCalSeg->dfMuS = pExport->muSa;		//90�Ȳ���紵����ϵ��
			}
		}
		else
		{
			pCalSeg->dfAf = pExport->Af_b* pWindSeg->CqiFront;	//��ӭ����ͶӰ���(m2)
			pCalSeg->dfMuS = pExport->muSb;	// 0������紵����ϵ��
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
			xSummWeightPosition.Set();	//�ܼ�Ȩλ��
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
				logerr.Log("��ѹ��<%s>��ȡ����Ϊ0�����ݴ���", (char*)pMirCalSeg->szLabel);
				continue;
			}
			pMirCalSeg->massByKg = dfSummMass;	//��ѹ���ż�����(kg)
			//�����ѹ�μ�Ȩƽ�����ĵ�λ��
			GEPOINT xNamedCenter = xSummWeightPosition / dfSummMass;
			GEPOINT xNamedVibrFai = xSummWeightVibrFai / dfSummMass;
			pMirCalSeg->dfNamedPosH = pExport->dfNamedPosH;	//��ѹ������߶�Zֵ,��λm��δ��ӦͳһΪxNameCenter.z wjh-2019.7.4
			//pMirCalSeg->dfNamedPosH = pDispatcher->m_fLowestBtmZ - xNamedCenter.z;	//��ѹ������߶�Zֵ,��λm��δ��ӦͳһΪxNameCenter.z wjh-2019.7.4
			pMirCalSeg->dfMuZ = pExport->muZ;	//��ѹ������ϵ�����ѹ�߶ȱ仯ϵ��
			pMirCalSeg->Iz = pow(pMirCalSeg->dfNamedPosH / 10, -alfa);	//����ǿ��ϵ��
			pMirCalSeg->dfVibrFaiX = xNamedVibrFai.x;//X������ϵ��
			pMirCalSeg->dfVibrFaiY = xNamedVibrFai.y;//Y������ϵ��
			if (WINDCALSEG::CURR_VIBR_FAI_TYPE == WINDCALSEG::FAI_TYPE_X)
			{
				if (cSymbol == 'A'&&pWindSeg->cCalStyle>0)
				{
					pMirCalSeg->dfAf = 0.45*pExport->Af_b* pWindSeg->CqiSide;
					pMirCalSeg->dfMuS = pExport->muSb;		//90�Ȳ���紵����ϵ��
					pExport->b2aSD = pExport->b2aFR;
				}
				else
				{
					pMirCalSeg->dfAf = pExport->Af_a* pWindSeg->CqiSide;	//��ӭ����ͶӰ���(m2)
					pMirCalSeg->dfMuS = pExport->muSa;		//90�Ȳ���紵����ϵ��
				}
			}
			else
			{
				pMirCalSeg->dfAf = pExport->Af_b* pWindSeg->CqiFront;	//��ӭ����ͶӰ���(m2)
				pMirCalSeg->dfMuS = pExport->muSb;		// 0������紵����ϵ��
			}
		}
		//��STower��ֵ��֤��ʽ�����Ƿ�����
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
	/*	�ֶ����㹫ʽCalBzConstComponent��ȷ�Ե���ʱ���� wjh-2019.7.7
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
			logerr.Log("��ѹ��<%s>����ϵ������ʧ��", (char*)xarrWindCalSeg[i].szLabel);
			if (pWindSegExport)	//��ǰ�Է�ѹ�����ƽ��и�ֵ��������������ж�ʱ���������ȱ������
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
	epsilonT=1.0;	//����ϵ������ʱ���ݸ���ȫ��ȡֵ�ĸ�������������ۼ�ϵ��
	dfI10 = 0.14;	//��Ӧ10m�ߴ�����ǿ��
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
	double zHigh;	//�߶�
	double dfBetaZ;	//����ϵ��
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
{	//��������߷�ѹ�ΰ��߶Ƚ��в�ֵ����
	for(int i=0;i<count;i++)
	{
		if(zHigh<xarrBetaZ[i].zHigh)
		{
			if(i==0)	//С����Ͳ�ֵ��߶�
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
	CString sTime = t.Format("%Y��%m��%d��%Hʱ%M��");
	fprintf(fp, "************************************************************************\n");
	fprintf(fp, "*                                                                      *\n");
	fprintf(fp, "*  LDS��������ϵ������ - (C) 2002-2019 �����ź��������Ƽ����޹�˾    *\n");
	fprintf(fp, "*                                                                      *\n");
	fprintf(fp, "*                        ��������ϵ��������                            *\n");
	fprintf(fp, "************************************************************************\n");
	fprintf(fp, "\n");
	if(pTowerModel !=NULL)
		fprintf(fp, "�����ͺţ�%s\n", pTowerModel->m_sTaType);
	else
		fprintf(fp, "�����ͺţ�  \n");
	fprintf(fp, "���ߣ�%s\n", pCurrCalHeight->description);
	fprintf(fp, "�������ԣ�f1=%.3f, T=%.3f(s)\n", frequency, 1 / frequency);
	//fprintf(fp,"����ǣ�%d��\n",pStatus->WindLoad.azimuth);
	fprintf(fp, "������٣�%.1fm/s\n", dfBetaZCalWindV);
	char cLandscape = pTowerModel != NULL ? pTowerModel->m_cLandscape : 'B';
	double I10 = 0.14;
	if (cLandscape == 'A')
	{
		fprintf(fp, "��ò���ͣ�A:������ɳĮƽ̹����\n");
		I10 = 0.12;
	}
	else if (cLandscape == 'B')
	{
		fprintf(fp, "��ò���ͣ�B:��½������ϡ�����\n");
		I10 = 0.14;
	}
	else if (cLandscape == 'C')
	{
		fprintf(fp, "��ò���ͣ�C:���ܼ�����Ⱥ����\n");
		I10 = 0.23;
	}
	else //if(cLandscape=='D')
	{
		fprintf(fp, "��ò���ͣ�D:���ܼ�����Ⱥ�͸߲㽨������\n");
		I10 = 0.39;
	}
	_BETAZ_H _xarrBetaZ[32];
	ARRAY_LIST<_BETAZ_H> xarrBetaZ(_xarrBetaZ,32);
	fprintf(fp, "\n==================��������ϵ������������̱��� =======================\n");
	fprintf(fp, "��ѹ����    Zi       mi      ��1i  ��si  ��zi  Izi     Ai   Bz    ��z\n");
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
		if(pExp->cCalStyle==0)	//�����
			xarrBetaZ.append(_BETAZ_H(H-pExp->dfNamedPosH+zHighest,pExp->xBetazCal.dfBetaZ));
	}
	CQuickSort<_BETAZ_H>::QuickSort(_xarrBetaZ,xarrBetaZ.GetSize(),_LocalCompareBetaZH);
	fprintf(fp, "\n==================��������ϵ������������ =======================\n");
	fprintf(fp, "��ѹ����                            ��z\n");
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
	//Ԥ�ݼ��㵱ǰ�����¸���ѹ�εķ����ֵ
	try {
		for (MASSNODE* pMassNode = hashNodeMassAllocInWindSeg.GetFirst();
			pMassNode != NULL; pMassNode = hashNodeMassAllocInWindSeg.GetNext())
			pMassNode->pNode = pTowerModel->FromNodeHandle(pMassNode->handle);
		for (WINDSEGBETAZCAL_EXPORT *pExport = hashWindSegExport.GetFirst(); pExport; pExport = hashWindSegExport.GetNext())
		{
			fprintf(fp, "\n================��ѹ��<%s> ======================\n", (char*)pExport->xBetazCal.szLabel);
			fprintf(fp, "\nһ��������Ϣ\n");
			if (pExport->cType == 0)
				fprintf(fp, "�����涨�巽ʽ���˵���������ѹ��\n");
			else if (pExport->cType == 1)
				fprintf(fp, "�����涨�巽ʽ������������ѹ��\n");
			else
				fprintf(fp, "�����涨�巽ʽ�����������ѹ��\n");
			if (pExport->cCalStyle == 0)
				fprintf(fp, "�������ͣ�����\n");
			else if (pExport->cCalStyle == 1)
				fprintf(fp, "�������ͣ�X��ˮƽ�ᵣ\n");
			else if (pExport->cCalStyle == 2)
				fprintf(fp, "�������ͣ�Y��ˮƽ�ᵣ\n");
			if (pExport->cMirType == 0)
				fprintf(fp, "�ԳƷ�ʽ����\n");
			else if (pExport->cMirType == 1)
				fprintf(fp, "�ԳƷ�ʽ��Y��ԳƷ�ѹ��\n");
			else if (pExport->cMirType == 2)
				fprintf(fp, "�ԳƷ�ʽ��X��ԳƷ�ѹ��\n");
			fprintf(fp, "\n��������ˮƽ����ؼ���\n");
			fprintf(fp, "                ӭ����˼���ϸ�嵥\n");
			fprintf(fp, "�˼���ʶ     ���     ����(mm) �������(m2) ����ϵ�� ���� ���� ����(kg) ��Ȩ����(kg)\n");
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
					spec.InsertBefore("��", 0);//��
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
			fprintf(fp, "�ϼƣ�  �������:  %8.3f m2,���� �Ǹ����: %.3f m2, �ֹ����: %.3f m2;\n", sumstat.area, dfAreaOfAngles,dfAreaOfTubes);
			fprintf(fp, "������: %.3f(kg), �ܼ�Ȩ����: %8.3f(kg)\n",dfSummRawMass,dfSummMassIncRaiseCoef);

			fprintf(fp, "��׼��ѹ��%.3fkN/m2\n", pExport->Wo);
			fprintf(fp, "BH  =%7.3fm;��ظ�\n", pExport->dfNamedPosH);
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
				fprintf(fp, "A   =%6.3f  ;��������ͶӰ���,m2\n", dfArea);
				fprintf(fp, "Af  =%6.3f  ;������ܷ�ѹͶӰ���(������ЧӦϵ��Cqi),m2\n", pExport->xBetazCal.dfAf);// Af_a);
				fprintf(fp, "Cqi =%.3f   ;���浲���������ϵ��\n", dfCqi);
				fprintf(fp, "��  =%.3f   ;���浲��ϵ��������ʣ�\n", dfFai);
				fprintf(fp, "��z =%5.3f   ;��ѹ�߶ȱ仯ϵ��\n", pExport->muZ);
				fprintf(fp, "a   =%.3f   ;����ӭ���������,m\n", pExport->b2aSD.a*0.001);
				fprintf(fp, "b   =%.3f   ;����ӭ����������,m\n", pExport->b2aSD.b*0.001);
				fprintf(fp, "b/a =%.3f   ;����ӭ�������/ӭ������\n", dfB2A);
				fprintf(fp, "��  =%.3f   ;���汳������ؽ���ϵ��\n", dfYita);
				fprintf(fp, "��s =%.3f   ;��������ϵ�����Ѻ�1+�ǣ�\n", pExport->xBetazCal.dfMuS);// muSa);
			}
			else
			{
				fprintf(fp, "A   =%6.3f  ;��������ͶӰ���,m2\n", pExport->A_b);
				fprintf(fp, "Af  =%6.3f  ;������ܷ�ѹͶӰ���(������ЧӦϵ��Cqi),m2\n", pExport->xBetazCal.dfAf);
				fprintf(fp, "Cqi =%.3f   ;���浲���������ϵ��\n", pExport->Cqi_b);
				fprintf(fp, "��  =%.3f   ;���浲��ϵ��������ʣ�\n", pExport->fai_b);
				fprintf(fp, "��z =%5.3f   ;��ѹ�߶ȱ仯ϵ��\n", pExport->muZ);
				fprintf(fp, "a   =%.3f   ;����ӭ���������,m\n", pExport->b2aFR.a*0.001);
				fprintf(fp, "b   =%.3f   ;����ӭ����������,m\n", pExport->b2aFR.b*0.001);
				fprintf(fp, "b/a =%.3f   ;����ӭ�������/ӭ������\n", pExport->b_a_b);
				fprintf(fp, "��  =%.3f   ;���汳������ؽ���ϵ��\n", pExport->yita_b);
				fprintf(fp, "��s =%.3f   ;��������ϵ�����Ѻ�1+�ǣ�\n", pExport->muSb);
			}
			fprintf(fp,"\n��������ϵ������\n");
			fprintf(fp, "g   =2.5     ;��ֵ����\n");
			fprintf(fp, "��t =%5.3f   ;��������������ۼ�ϵ��\n", epsilonT);
			fprintf(fp, "R^2 =%5.3f   ;��������ƽ��ֵ\n", dfSquareR);
			fprintf(fp, "Wo  =%5.3f   ;��׼��ѹ\n", W0);
			fprintf(fp, "I10 =%5.3f   ;10m�߶���������ǿ��\n", dfI10);
			fprintf(fp, "Iz  =%5.3f   ;����ǿ���ظ߶ȵķֲ�ϵ��\n", pExport->xBetazCal.Iz);
			fprintf(fp, "Bzi =%5.3f   ;��ѹ�α�������\n", pExport->xBetazCal.Bz);
			fprintf(fp, "��z =%5.3f   ;����ϵ��������̦�z=1+2g*��t*I10*Bzi*SQRT(1+R^2)\n", pExport->xBetazCal.dfBetaZ);
			fprintf(fp, "---------��������ϵ������Ч���������ļ����ϵ������\n");
			fprintf(fp, "��ѹ�ε�Ч����         mi =%.2f(kg)\n", pExport->xBetazCal.massByKg);
			fprintf(fp, "��ѹ�����ĸ߶�         Zi =%.2f(m)\n", pExport->dfNamedPosH);
			fprintf(fp, "��ѹ�����ĵ�Ч����ϵ�� ��1=%.3f\n", pExport->xBetazCal.dfCurrFai);
			fprintf(fp, "Node    M(kg)  ��1      Pos.X(m)   Pos.Y(m)   Pos.Z(m) \n");
			fprintf(fp, "�ڵ��  ����   ����ϵ�� λ��.X(m)  λ��.Y(m)  λ��.Z(m)\n");
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
			fprintf(fp, "�ϼ�������%8.3f(kg)\n", dfSummSegMass);
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