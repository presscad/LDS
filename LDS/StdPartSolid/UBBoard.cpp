#include "StdAfx.h"
#include "UBBoard.h"
#include "HoleMaker.h"
#include "f_alg_fun.h"
#include "StdMetaSolid.h"

UBBoard::UBBoard(FITTING_UB *pUBFitting/*=NULL*/)
{
	if (pUBFitting == NULL)
	{
		IFittingLibrary<FITTING_UB>* pUBLib = FittingLibrary<FITTING_UB>();
		ub = *pUBLib->GetAt(1);
	}
	else
		ub = *pUBFitting;
	//
	m_fNutDs = ub.dfTrunnionD / 3 * 5;
	m_fNutDe = m_fNutDs / cos(Pi / 6);
	m_fNutH = 0.5*m_fNutDs;
	acs.InitStdCS();
}

UBBoard::~UBBoard(void)
{
}

int UBBoard::CreatUBBoard(fBody* pBody, COLORREF crColor)
{
	//前|后侧挂板轮廓点
	GECS plate_cs;
	plate_cs.InitStdCS();
	GEPOINT ptTopL, ptTopR, ptBtmL, ptBtmR,center;
	ptTopL.Set(-ub.dfBoardW*0.5, 0);
	ptTopR.Set(ub.dfBoardW*0.5, 0);
	ptBtmL.Set(-ub.dfBoardW*0.5, -ub.dfBoardH);
	ptBtmR.Set(ub.dfBoardW*0.5, -ub.dfBoardH);
	center.Set(0, -ub.dfBoardH);
	f3dArcLine arcline;
	if (!arcline.CreateMethod3(ptBtmL, ptBtmR, f3dPoint(0, 0, 1), ub.dfBoardW*0.5, center))
		return 0;
	int slices = 32;
	double dfSliceAngle = arcline.SectorAngle() / slices;
	ARRAY_LIST<GEPOINT> ptArr;
	ptArr.SetSize(0, slices + 4);
	ptArr.append(ptTopL);
	ptArr.append(ptBtmL);
	for (int i = 1; i < slices; i++)
		ptArr.append(arcline.PositionInAngle(dfSliceAngle*i));
	ptArr.append(ptBtmR);
	ptArr.append(ptTopR);
	double fPlateT = ub.dfNamedL*0.5;
	for (int i = 0; i < 2; i++)
	{
		if (i == 0)	//前侧板
			plate_cs.origin.Set(0, 0, ub.dfNamedL*0.5);
		else       //后侧板
			plate_cs.origin.Set(0, 0, -ub.dfNamedL*0.5 - fPlateT);
		ARRAY_LIST<METASOLID::PROFVERTEX> arrVertexe;
		arrVertexe.SetSize(ptArr.GetSize());
		for (int i = 0; i < ptArr.GetSize(); i++)
			arrVertexe[i].pos = ptArr[i];
		long liInitFaceIndex = pBody->faceList.GetNodeNum();
		METASOLID::AppendPolygonExtrudeSolid(pBody, plate_cs, fPlateT, arrVertexe.m_pData, arrVertexe.Count, crColor);
		//进行打孔操作
		f3dPolyFace *pTopFace = pBody->faceList.GetByIndex(liInitFaceIndex);
		f3dPolyFace *pBtmFace = pBody->faceList.GetByIndex(liInitFaceIndex + 1);
		GECS hole_cs;
		hole_cs.InitStdCS();
		hole_cs.origin.Set(0, -ub.dfBoardH, 0);
		CHoleMaker hole_maker(ub.dfTrunnionD);
		hole_maker.Create(hole_cs,pTopFace, pBtmFace, pBody, crColor);
	}
	//过渡半圆弧实体
	plate_cs.origin.Set(-ub.dfBoardW*0.5, 0);
	plate_cs.axis_x.Set(0, 0, -1);
	plate_cs.axis_y.Set(0, 1, 0);
	plate_cs.axis_z.Set(1, 0, 0);
	double fInnR = ub.dfNamedL*0.5;
	double fOutR = fInnR + fPlateT;
	ptArr.Empty();
	ptArr.SetSize(0, 2*slices + 4);
	for (int i = 0; i < 2; i++)
	{
		GEPOINT ptS = (i == 0) ? GEPOINT(fOutR, 0) : GEPOINT(-fInnR, 0);
		GEPOINT ptE = (i == 0) ? GEPOINT(-fOutR, 0) : GEPOINT(fInnR, 0);
		GEPOINT norm = (i == 0) ? GEPOINT(0, 0, 1) : GEPOINT(0, 0, -1);
		double fR = (i == 0) ? fOutR : fInnR;
		arcline.CreateMethod3(ptS, ptE, norm, fR, GEPOINT(0,0,0));
		dfSliceAngle = arcline.SectorAngle() / slices;
		ptArr.append(ptS);
		for (int i = 1; i < slices; i++)
			ptArr.append(arcline.PositionInAngle(dfSliceAngle*i));
		ptArr.append(ptE);
	}
	ARRAY_LIST<METASOLID::PROFVERTEX> arrVertexe;
	arrVertexe.SetSize(ptArr.GetSize());
	for (int i = 0; i < ptArr.GetSize(); i++)
		arrVertexe[i].pos = ptArr[i];
	long liInitFaceIndex = pBody->faceList.GetNodeNum();
	METASOLID::AppendPolygonExtrudeSolid(pBody, plate_cs, ub.dfBoardW, arrVertexe.m_pData, arrVertexe.Count, crColor);
	return pBody->vertex.GetNodeNum();
}
BOOL UBBoard::Create3dSolidModel(CSolidBody *pSolidBody)
{
	fBody body;
	if (!AppendSolidBody(&body))
		return FALSE;
	pSolidBody->ConvertFrom(&body);
	return TRUE;
}
BOOL UBBoard::AppendSolidBody(fBody * pBody, bool blTransToWorkEnv /*= true*/)
{
	COLORREF crBoard = RGB(145, 149, 145), crLs = RGB(91, 91, 91);
	//挂板主体
	CreatUBBoard(pBody, crBoard);
	//过渡弧处的螺栓
	GECS pcs(GEPOINT(-ub.dfBoardW*0.5, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0), GEPOINT(1, 0, 0));
	double fTrunnionL = ub.dfBoardW + ub.siThroughThick * 2 + m_fNutH * 2;
	METASOLID::AppendDiskSolid(pBody, ub.dfTrunnionD, fTrunnionL, pcs, 32, crLs);
	pcs.origin = -ub.dfBoardW*0.5 - m_fNutH - ub.siThroughThick;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	pcs.origin = ub.dfBoardW*0.5 + ub.siThroughThick;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	//挂板挂孔处的螺栓
	pcs.InitStdCS();
	pcs.origin.Set(0, -ub.dfBoardH, -ub.dfNamedL);
	fTrunnionL = ub.dfNamedL * 2 + m_fNutH + 20;
	METASOLID::AppendDiskSolid(pBody, ub.dfTrunnionD, fTrunnionL, pcs, 32, crLs);
	pcs.origin.z = -ub.dfNamedL - m_fNutH;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	pcs.origin.z = ub.dfNamedL;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);
	//装配到模型实际位置
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