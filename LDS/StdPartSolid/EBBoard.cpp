#include "StdAfx.h"
#include "EBBoard.h"
#include "f_alg_fun.h"
#include "StdMetaSolid.h"

CEBBoard::CEBBoard(FITTING_EB *pEBFitting/*=NULL*/)
{
	if (pEBFitting == NULL)
	{
		IFittingLibrary<FITTING_EB>* pEBLib = FittingLibrary<FITTING_EB>();
		eb = *pEBLib->GetAt(1);
	}
	else
		eb = *pEBFitting;
	//以下是根据工程经验系数确定的非主控外形参数
	m_fNutDs = eb.dfTrunnionD / 3 * 5;
	m_fNutDe = m_fNutDs / cos(Pi / 6);
	m_fNutH = 0.5*m_fNutDs;
	m_fNutTaper = 0.35;
	m_fWasherD = m_fNutDe;
	m_fWasherH = 0.1*m_fWasherD;
	acs.InitStdCS();
}
CEBBoard::~CEBBoard(void)
{
}

int CEBBoard::CreatEBBoard(fBody* pBody,COLORREF crColor)
{
	//1、生成圆筒实体
	GECS tube_acs;
	tube_acs.origin.Set(-eb.dfBoardL*0.5, 0, 0);
	tube_acs.axis_x.Set(0, 0, -1);
	tube_acs.axis_y.Set(0, 1, 0);
	tube_acs.axis_z.Set(1, 0, 0);
	int nTubeSlices = 64;
	double dfTubeT = eb.siThick;
	double dfTubeR = eb.dfTrunnionD*0.5 + 6 + dfTubeT;
	METASOLID::AppendTubeSolid(pBody, dfTubeR * 2, dfTubeT, eb.dfBoardL, tube_acs, nTubeSlices, crColor);
	//2、生成圆筒与下颚过渡实体
	double dfSectH=sqrt(pow(dfTubeR,2)-pow(0.5*eb.dfTrunnionD,2));//不规则体与圆筒的切面距中轴距离
	double dfFilletR = 0.4*eb.dfTrunnionD;	//过渡弧半径是根据显示效果取的经验值
	double dfDist = dfSectH + dfFilletR * cos(Pi / 6);//不规则体与中轴的距离
	COORD3D cirPtArr[100];
	nTubeSlices = METASOLID::CirclePolygonize(dfTubeR, cirPtArr, nTubeSlices);
	ARRAY_LIST<GEPOINT> tubeBtmPtArr;
	tubeBtmPtArr.SetSize(0, nTubeSlices);
	for (int i = nTubeSlices / 2; i < nTubeSlices; i++)
	{
		if (cirPtArr[i].x >= -eb.siThick &&cirPtArr[i].x <= eb.siThick)
			tubeBtmPtArr.append(GEPOINT(cirPtArr[i]));
	}
	int nTubeBtmPt = tubeBtmPtArr.GetSize();
	GEPOINT ptTopL = tubeBtmPtArr[0];
	GEPOINT ptTopR = tubeBtmPtArr[nTubeBtmPt - 1];
	GEPOINT ptBtmL(-eb.siThick*0.5, -dfDist, 0);
	GEPOINT ptBtmR(eb.siThick*0.5, -dfDist, 0);
	ARRAY_LIST<GEPOINT> ptArr;
	ptArr.SetSize(0, 100);
	for (int i = 0; i < 2; i++)
	{
		f3dPoint ptS = (i == 0) ? ptTopL : ptBtmR;
		f3dPoint ptE = (i == 0) ? ptBtmL : ptTopR;
		f3dPoint center = (ptS + ptE)*0.5;
		center.x += (i == 0) ? (-10) : 10;
		f3dArcLine arcline;
		if(!arcline.CreateMethod3(ptS, ptE, f3dPoint(0, 0, -1), dfFilletR, center))
			continue;
		double dfSectorAngle = arcline.SectorAngle();
		double dfPieceAngle = dfSectorAngle / 12;
		ptArr.append(ptS);
		for (int j = 1; j < 12; j++)
			ptArr.append(arcline.PositionInAngle(dfPieceAngle*j));
		ptArr.append(ptE);
	}
	for (int i = nTubeBtmPt - 2; i > 0; i--)
		ptArr.append(tubeBtmPtArr[i]);
	//通过拉伸生成实体
	ARRAY_LIST<METASOLID::PROFVERTEX> arrVertexe;
	arrVertexe.SetSize(ptArr.GetSize());
	for (int i = 0; i < ptArr.GetSize(); i++)
		arrVertexe[i].pos = ptArr[i];
	METASOLID::AppendPolygonExtrudeSolid(pBody, tube_acs, eb.dfBoardL,arrVertexe.m_pData, arrVertexe.Count, crColor);
	//3、生成下颌平板部分实体
	GECS plate_cs;
	plate_cs.InitStdCS();
	plate_cs.origin.Set(0, 0, -eb.siThick / 2);
	f2dPoint ptTopL2d, ptTopR2d, ptTanL2d, ptTanR2d, ptPick2d;
	ptTopL2d.Set(-eb.dfBoardL*0.5, -dfDist);
	ptTopR2d.Set(eb.dfBoardL*0.5, -dfDist);
	ptPick2d.Set(0, -eb.dfBoardH - 10);
	f2dCircle cir2d(eb.dfChinR, 0, -eb.dfBoardH);
	Tang2dpc(ptTopL2d, cir2d, ptPick2d, ptTanL2d);
	Tang2dpc(ptTopR2d, cir2d, ptPick2d, ptTanR2d);
	GEPOINT ptS(ptTanL2d.x, ptTanL2d.y), ptE(ptTanR2d.x, ptTanR2d.y),center(0, -eb.dfBoardH);
	f3dArcLine arcline;
	if (!arcline.CreateMethod3(ptS, ptE, f3dPoint(0,0,1), eb.dfChinR, center))
		return pBody->vertex.GetNodeNum();
	//将圆弧简化为多段直线段
	int slices = 32;
	double dfSliceAngle = arcline.SectorAngle() / slices;
	arrVertexe.Empty();
	arrVertexe.SetSize(4 + slices - 1);
	arrVertexe[0].pos.Set(ptTopL2d.x, ptTopL2d.y);
	arrVertexe[1].pos.Set(ptTanL2d.x, ptTanL2d.y);
	for (int i = 1; i < slices; i++)
		arrVertexe[i + 1].pos = arcline.PositionInAngle(dfSliceAngle*i);
	arrVertexe[slices + 1].pos.Set(ptTanR2d.x, ptTanR2d.y);
	arrVertexe[slices + 2].pos.Set(ptTopR2d.x, ptTopR2d.y);
	arrVertexe[slices + 2].blSkipEdge = true;
	long liInitFaceIndex = pBody->faceList.GetNodeNum();
	METASOLID::AppendPolygonExtrudeSolid(pBody, plate_cs, eb.siThick,arrVertexe.m_pData, arrVertexe.Count, crColor);
	//进行打孔操作
	f3dPolyFace *pTopFace = pBody->faceList.GetByIndex(liInitFaceIndex);
	f3dPolyFace *pBtmFace = pBody->faceList.GetByIndex(liInitFaceIndex + 1);
	GECS hole_cs;
	hole_cs.InitStdCS();
	hole_cs.origin.Set(0, -eb.dfBoardH, 0);
	CHoleMaker hole_maker;
	hole_maker.Create(hole_cs, eb.dfHoleD,0,pTopFace, pBtmFace, pBody, crColor);
	return pBody->vertex.GetNodeNum();
}

BOOL CEBBoard::Create3dSolidModel(CSolidBody *pSolidBody)
{
	fBody body;
	if(!AppendSolidBody(&body))
		return FALSE;
	pSolidBody->ConvertFrom(&body);
	return TRUE;
}
BOOL CEBBoard::AppendSolidBody(fBody *pBody,bool blTransToWorkEnv/*=true*/)
{
	//EB挂板主体
	int Index =CreatEBBoard(pBody);
	//耳轴螺杆
	double fThroughLen=eb.dfBoardL+eb.siThroughThick*3+eb.dfTrunnionD/3*8;
	GECS pcs(GEPOINT(-fThroughLen/2,0,0), GEPOINT(0,0,-1), GEPOINT(0,1,0), GEPOINT(1,0,0));
	Index= METASOLID::AppendDiskSolid(pBody,eb.dfTrunnionD,fThroughLen,pcs,32,RGB(169,169,169));
	//垫片
	pcs.origin.x=eb.dfBoardL/2+eb.siThroughThick;				//上连接处圆片坐标(上螺母垫片位置)
	Index=METASOLID::AppendDiskSolid(pBody,m_fWasherD,m_fWasherH,pcs,32,RGB(91,91,91));
	pcs.origin.x=-eb.dfBoardL/2-eb.siThroughThick-m_fWasherH;	//下连接处圆片坐标(上螺母垫片位置)
	Index=METASOLID::AppendDiskSolid(pBody,m_fWasherD,m_fWasherH,pcs,32,RGB(91,91,91));
	//螺母
	pcs.origin.x =eb.dfBoardL/2+eb.siThroughThick+m_fWasherH;			//上螺母位置
	Index= METASOLID::AppendTaperNutSolid(pBody,m_fNutDe,m_fNutDs,m_fNutH,pcs,48,RGB(91,91,91));
	pcs.origin.x=-eb.dfBoardL/2-eb.siThroughThick-m_fWasherH-m_fNutH;	//下螺母位置
	Index= METASOLID::AppendTaperNutSolid(pBody,m_fNutDe,m_fNutDs,m_fNutH,pcs,48,RGB(91,91,91));
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
			GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
		}
	}
	return TRUE;
}