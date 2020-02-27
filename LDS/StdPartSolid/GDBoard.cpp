#include "StdAfx.h"
#include "GDBoard.h"
#include "HoleMaker.h"
#include "f_alg_fun.h"
#include "LogFile.h"
#include "StdMetaSolid.h"

CGDBoard::CGDBoard(FITTING_GD *pGDFitting/*=NULL*/)
{
	if (pGDFitting == NULL)
	{
		IFittingLibrary<FITTING_GD>* pGDLib = FittingLibrary<FITTING_GD>();
		gd = *pGDLib->GetAt(1);
	}
	else
		gd = *pGDFitting;
	//以下是根据工程经验系数确定的非主控外形参数
	m_fNutDs = gd.dfTrunnionD / 3 * 4;
	m_fNutDe = m_fNutDs / cos(Pi / 6);
	m_fNutH = 0.5*m_fNutDs;
	m_fNutTaper = 0.35;
	m_fWasherD = m_fNutDe;
	m_fWasherH = 0.1*m_fWasherD;
	m_fBaffleD = m_fNutDs;
	m_fBaffleH = 0.1*m_fBaffleD;
	acs.InitStdCS();
}

CGDBoard::~CGDBoard(void)
{
}

long CGDBoard::GetTopArcVertexes(GEPOINT* parrVertexes, long liMaxSlices)
{
	GEPOINT xUpArcCenter, xChordVecLeft, xChordVecRight, xChordNormal;
	GEPOINT xArcTipLeft(-gd.dfBoardL / 2, this->m_fBaffleD / 2);
	GEPOINT xArcTipRight(gd.dfBoardL / 2, this->m_fBaffleD / 2);
	GEPOINT xArcMidPoint(0, gd.dfBoardH1);
	f3dLine chordline1, chordline2;
	//左弦
	xChordVecLeft = xArcTipLeft - xArcMidPoint;
	xChordNormal.Set(-xChordVecLeft.y, xChordVecLeft.x);	//逆时针转90°
	chordline1.startPt = (xArcTipLeft + xArcMidPoint)*0.5;
	chordline1.endPt = chordline1.startPt + xChordNormal;
	//右弦
	xChordVecRight = xArcMidPoint - xArcTipRight;
	xChordNormal.Set(-xChordVecRight.y, xChordVecRight.x);
	chordline2.startPt = (xArcTipRight + xArcMidPoint)*0.5;
	chordline2.endPt = chordline2.startPt + xChordNormal;
	int retcode = Int2dpl(f2dLine(chordline1.startPt, chordline1.endPt), f2dLine(chordline2.startPt, chordline2.endPt),
		xUpArcCenter.x, xUpArcCenter.y);
	if (retcode <= 0)
	{
		logerr.Log("error");
		return 0;
	}
	//计算上圆弦边的折线样点
	int slices = liMaxSlices;
	double radius = DISTANCE(xUpArcCenter, xArcMidPoint);
	double sectangle = cal_angle_of_2vec(xArcTipLeft - xUpArcCenter, xArcTipRight - xUpArcCenter);
	double slice_angle = sectangle / slices;//2*asin(Arcl/(4*r))/(slices-1);
	double sina = sin(slice_angle);	//扇片角正弦
	double cosa = cos(slice_angle);	//扇片角余弦
	parrVertexes[0] = xArcTipRight;
	for (int i = 1; i < slices; i++)
	{
		GEPOINT vLocalPoint = parrVertexes[i - 1] - xUpArcCenter;
		parrVertexes[i].x = vLocalPoint.x*cosa - vLocalPoint.y*sina;
		parrVertexes[i].y = vLocalPoint.x*sina + vLocalPoint.y*cosa;
		parrVertexes[i].z = 0;
		parrVertexes[i] = parrVertexes[i] + xUpArcCenter;
	}
	parrVertexes[slices] = xArcTipLeft;
	return slices + 1;
}
long CGDBoard::GetChinArcVertexes(GEPOINT* parrVertexes, long liMaxSlices)
{
	GEPOINT xArcTipLeft, xArcTipRight, xArcCenter(0, -gd.dfBoardH2);
	f2dCircle circle(gd.dfBoardH, xArcCenter.x, xArcCenter.y);
	f2dPoint xBasePoint(-gd.dfBoardL*0.5, -m_fBaffleD * 0.5);
	f2dPoint xChinPick(0, -gd.dfBoardH2 - gd.dfBoardH), xTangentPoint;
	if (Tang2dpc(xBasePoint, circle, xChinPick, xTangentPoint) < 1)
	{
		logerr.Log("未找到左侧公切点");
		return 0;
	}
	xArcTipLeft.Set(xTangentPoint.x, xTangentPoint.y);
	xBasePoint.x *= -1;
	if (Tang2dpc(xBasePoint, circle, xChinPick, xTangentPoint) < 1)
	{
		logerr.Log("未找到右侧公切点");
		return 0;
	}
	xArcTipRight.Set(xTangentPoint.x, xTangentPoint.y);
	int slices = liMaxSlices;
	double radius = gd.dfBoardH;
	GEPOINT vVecStart(xArcTipLeft.x, xArcTipLeft.y + gd.dfBoardH2);
	GEPOINT vVecEnd(xArcTipRight.x, xArcTipRight.y + gd.dfBoardH2);
	double sectangle = cal_angle_of_2vec(vVecStart, vVecEnd);
	double slice_angle = sectangle / slices;
	double sina = sin(slice_angle);	//扇片角正弦
	double cosa = cos(slice_angle);	//扇片角余弦
	parrVertexes[0] = xArcTipLeft;
	for (int i = 1; i < slices; i++)
	{
		GEPOINT vLocalPoint = parrVertexes[i - 1] - xArcCenter;
		parrVertexes[i].x = vLocalPoint.x*cosa - vLocalPoint.y*sina;
		parrVertexes[i].y = vLocalPoint.x*sina + vLocalPoint.y*cosa;
		parrVertexes[i].z = 0;
		parrVertexes[i] += xArcCenter;
	}
	parrVertexes[slices] = xArcTipRight;
	return slices + 1;
}

int CGDBoard::CreateBoardBody(fBody* pBody,COLORREF crColor)
{
	GECS plate_cs;
	plate_cs.InitStdCS();
	plate_cs.origin.Set(0, 0, -gd.siThick / 2);
	int i, slices = 16;//(count-2)/2;
	f3dPoint under_vertex_arr[50], btm_vertex_arr[100];
	PRESET_ARRAY128<GEPOINT> xarrVertexes;
	GEPOINT xarrTopArcVertexes[50], xarrChinArcVertexes[50];
	pBody->Empty();
	//生成点
	long liTopCount = GetTopArcVertexes(xarrTopArcVertexes, slices);	//计算顶部圆弧边顶点表
	long liChinCount = GetChinArcVertexes(xarrChinArcVertexes, slices);//计算下颌圆弧边顶点表
	//将上圆弧点添加入列表
	for (i = 0; i < liTopCount; i++)
		xarrVertexes.Append(GEPOINT(xarrTopArcVertexes[i].x, xarrTopArcVertexes[i].y, xarrTopArcVertexes[i].z));
	xarrVertexes.Append(GEPOINT(-gd.dfBoardL / 2, -m_fBaffleD / 2));
	//将下圆弧点添加进列表
	for (i = 0; i < liChinCount; i++)
		xarrVertexes.Append(GEPOINT(xarrChinArcVertexes[i].x, xarrChinArcVertexes[i].y, xarrChinArcVertexes[i].z));
	xarrVertexes.Append(GEPOINT(gd.dfBoardL / 2, -m_fBaffleD / 2));
	//通过拉伸生成实体
	int count = (int)xarrVertexes.Count;	//挂板主体二维投影多边形顶点数
	ARRAY_LIST<METASOLID::PROFVERTEX> arrVertexe;
	arrVertexe.SetSize(count);
	for (int i = 0; i < count; i++)
		arrVertexe[i].pos = xarrVertexes[i];
	long liInitFaceIndex = pBody->faceList.GetNodeNum();
	METASOLID::AppendPolygonExtrudeSolid(pBody, plate_cs, gd.siThick, arrVertexe.m_pData, arrVertexe.Count, crColor);
	//进行打孔操作
	f3dPolyFace *pTopFace = pBody->faceList.GetByIndex(liInitFaceIndex);
	f3dPolyFace *pBtmFace = pBody->faceList.GetByIndex(liInitFaceIndex + 1);
	GECS hole_cs;
	hole_cs.InitStdCS();
	hole_cs.origin.Set(0, -gd.dfBoardH2, 0);
	CHoleMaker hole_maker;
	hole_maker.Create(hole_cs,gd.dfHoleD,0,pTopFace, pBtmFace, pBody, crColor, 24);
	return pBody->vertex.GetNodeNum();
}

BOOL CGDBoard::Create3dSolidModel(CSolidBody *pSolidBody)
{
	fBody body;
	if(!AppendSolidBody(&body))
		return FALSE;
	pSolidBody->ConvertFrom(&body);
	return TRUE;
}
BOOL CGDBoard::AppendSolidBody(fBody *pBody,bool blTransToWorkEnv/*=true*/)
{
	//添加挂板主体
	int Index=CreateBoardBody(pBody);
	//其他部件实体(螺杆+螺母+垫片+圆片)
	GECS pcs(GEPOINT(0, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0), GEPOINT(1, 0, 0));
	COLORREF crLs = RGB(91, 91, 91),crBoard= RGB(145, 149, 145);
	int nSliceLs = 48, nSlice = 32;
	//连接处圆片
	pcs.origin.x = gd.dfBoardL*0.5;					//上连接处位置
	Index = METASOLID::AppendDiskSolid(pBody, m_fBaffleD, m_fBaffleH, pcs, nSlice, crBoard);
	pcs.origin.x = -gd.dfBoardL*0.5 - m_fBaffleH;	//下连接处位置
	Index = METASOLID::AppendDiskSolid(pBody, m_fBaffleD, m_fBaffleH, pcs, nSlice, crBoard);
	//螺杆
	pcs.origin.x = gd.dfBoardL*0.5;		//上螺杆位置
	Index = METASOLID::AppendDiskSolid(pBody, gd.dfTrunnionD, (gd.dfTrunnionL - gd.dfBoardL)*0.5, pcs, nSlice, crBoard);
	pcs.origin.x = -gd.dfTrunnionL*0.5;	//下螺杆位置
	Index = METASOLID::AppendDiskSolid(pBody, gd.dfTrunnionD, (gd.dfTrunnionL - gd.dfBoardL)*0.5, pcs, nSlice, crBoard);
	//垫片
	pcs.origin.x = gd.dfBoardL*0.5 + gd.siThroughThick;					//上螺母垫片位置
	Index = METASOLID::AppendDiskSolid(pBody, m_fWasherD, m_fWasherH, pcs, nSlice, crBoard);
	pcs.origin.x = -gd.dfBoardL*0.5 - gd.siThroughThick - m_fWasherH;	//下螺母垫片位置
	Index = METASOLID::AppendDiskSolid(pBody, m_fWasherD, m_fWasherH, pcs, nSlice, crBoard);
	//倒角螺母
	pcs.origin.x = gd.dfBoardL*0.5 + gd.siThroughThick + m_fWasherH;			//上倒角螺母位置
	Index = METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, nSliceLs, crLs);
	pcs.origin.x = -gd.dfBoardL*0.5 - gd.siThroughThick - m_fWasherH - m_fNutH;	//上倒角螺母位置
	Index = METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, nSliceLs, crLs);
	//装配到模型实际位置
	if (blTransToWorkEnv)
	{	
		UCS_STRU ucs(acs);
		for (f3dPoint* pVertice = pBody->vertex.GetFirst(); pVertice; pVertice = pBody->vertex.GetNext())
		{
			if (coord_trans(*pVertice, ucs, TRUE, TRUE) != TRUE)
				return FALSE;
		}
	}
	return TRUE;
}