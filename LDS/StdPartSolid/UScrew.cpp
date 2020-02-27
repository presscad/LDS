#include "StdAfx.h"
#include "f_alg_fun.h"
#include "StdMetaSolid.h"
#include "UScrew.h"

UScrew::UScrew(FITTING_US *pGSFitting/*=NULL*/)
{
	if (pGSFitting == NULL)
	{
		IFittingLibrary<FITTING_US>* pUSLib = FittingLibrary<FITTING_US>();
		us = *pUSLib->GetAt(1);
		return;
	}
	else
		us = *pGSFitting;
	//�����Ǹ��ݹ��̾���ϵ��ȷ���ķ��������β���
	m_fNutDs = us.dfColumnd / 3 * 5;
	m_fNutDe = m_fNutDs / cos(Pi / 6);
	m_fNutH = (us.dfColumnL2-us.siThroughThick)*0.25;
	acs.InitStdCS();
}

UScrew::~UScrew(void)
{
}

bool UScrew::Create3dSolidModel(CSolidBody *pSolidBody)
{
	fBody body;
	if (!AppendSolidBody(&body))
		return FALSE;
	pSolidBody->ConvertFrom(&body);
	return TRUE;
}
bool UScrew::AppendSolidBody(fBody *pBody, bool blTransToWorkEnv /*= true*/)
{
	COLORREF crScrew = RGB(145, 149, 145), crLs = RGB(91, 91, 91);
	//��Բ��
	GECS ring_cs(GEPOINT(0, -m_fNutH, 0), GEPOINT(1, 0, 0), GEPOINT(0, -1, 0), GEPOINT(0, 0, -1));
	double fInnR = us.dfColumnL1-us.dfColumnL2;
	double fOutR = fInnR + us.dfColumnd;
	METASOLID::AppendRingSolid(pBody, fOutR, fInnR, Pi, ring_cs, 64, crScrew);
	//�������
	GECS pcs(GEPOINT(0,0,0), GEPOINT(1, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0));
	pcs.origin.Set(-us.dfNamedC*0.5 ,-m_fNutH);
	METASOLID::AppendDiskSolid(pBody, us.dfColumnM, us.dfColumnL2, pcs, 32, crScrew);
	//�ײ�����ĸ
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	//����˫��ĸ
	pcs.origin.y += m_fNutH + us.siThroughThick;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	pcs.origin.y += m_fNutH;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	//�Ҳ�����
	pcs.origin.Set(us.dfNamedC*0.5, -m_fNutH);
	METASOLID::AppendDiskSolid(pBody, us.dfColumnM, us.dfColumnL2, pcs, 48, crScrew);
	//�ײ�����ĸ
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	//����˫��ĸ
	pcs.origin.y += m_fNutH + us.siThroughThick;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	pcs.origin.y += m_fNutH;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	//װ�䵽ģ��ʵ��λ��
	if (blTransToWorkEnv)
	{
		for (f3dPolyFace* pFace = pBody->faceList.GetFirst(); pFace; pFace = pBody->faceList.GetNext())
		{
			if (pFace->poly_norm.IsZero())
				continue;
			pFace->poly_norm = acs.TransVFromCS(pFace->poly_norm);
		}
		for (f3dPoint* pVertice = pBody->vertex.GetFirst(); pVertice; pVertice = pBody->vertex.GetNext())
		{
			GEPOINT xActualPoint = acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x, xActualPoint.y, xActualPoint.z);
		}
	}
	return TRUE;
}
