#include "StdAfx.h"
#include "URing.h"
#include "StdMetaSolid.h"
#include "f_alg_fun.h"

//////////////////////////////////////////////////////////////////////////
//CURing
CURing::CURing(FITTING_UR *pUring/*=NULL*/)
{
	if(pUring==NULL)
	{
		IFittingLibrary<FITTING_UR>* pURLib = FittingLibrary<FITTING_UR>();
		uring = *pURLib->GetAt(4);	//pxSizeTbl[4];
		return ;
	}
	else
		uring = *pUring;
	//以下是根据工程经验系数确定的非主控外形参数
	m_fNutDs = uring.dfRingBs + 8;
	m_fNutDe = m_fNutDs / cos(Pi / 6);
	m_fNutH = 0.5*m_fNutDs;
	m_fNutTaper = 0.35;
}
CURing::~CURing(void)
{
}
bool CURing::LoadSizeParams(const char* label)
{
	IFittingLibrary<FITTING_UR>* pURLib = FittingLibrary<FITTING_UR>();
	return pURLib->LoadSizeParams(label,&uring);
}
int CURing::AppendURBend(fBody* pBody, int slices/* =32 */, COLORREF crColor/* =RGB */)
{
	//半环实体
	GECS ring_cs;
	ring_cs.InitStdCS();
	ring_cs.origin.Set(0, uring.dfNamedL - uring.dfRadiusOfRing, 0);
	double fRingInnR = uring.dfRadiusOfRing;
	double fRingOutR = uring.dfRadiusOfRing + uring.dfRingBs;
	METASOLID::AppendRingSolid(pBody, fRingOutR, fRingInnR, Pi, ring_cs, slices, crColor);
	//直柱实体
	GEPOINT btm_vertex_arr[500];
	slices = METASOLID::CirclePolygonize(uring.dfRingBs*0.5, btm_vertex_arr, 128);
	AppendTube(pBody,btm_vertex_arr, slices, TRUE, crColor);
	AppendTube(pBody, btm_vertex_arr, slices, FALSE, crColor);
	return pBody->vertex.GetNodeNum();
}
int CURing::AppendTube(fBody* pBody, GEPOINT* btm_vertex_arr, int slices, BOOL bLeft /* = TRUE */,COLORREF bolt_color/* =RGB(206,219,215) */)
{
	long liStartIndex = pBody->vertex.GetNodeNum();

	DYN_ARRAY<GEPOINT> org_section_vertex_arr(slices);
	for (int i = 0; i < slices; i++)
		org_section_vertex_arr[i] = btm_vertex_arr[i];
	double fTaper = 0.65;
	double fRadio = uring.dfRingBs / 2;//圆柱半径
	double fLength = uring.dfRingBs / 2 - uring.dfT;//耳板与圆柱相交的弦到圆柱圆心的距离
	double fChord = sqrt(fRadio * fRadio - fLength * fLength);//半弦长
	double fLengthFromBoard = sqrt(uring.dfMdEdgeSpace*uring.dfMdEdgeSpace - fChord * fChord);//相交弦到耳板圆心的距离
	UCS_STRU  face_ucs;
	face_ucs.axis_x.Set(1, 0, 0);
	face_ucs.axis_y.Set(0, 0, 1);
	face_ucs.axis_z.Set(0, 1, 0);
	int nChordVertex[2];//记录特征点索引
	nChordVertex[0] = -1;
	nChordVertex[1] = -1;
	if (bLeft)//左侧斜坡圆柱
	{
		f3dPoint top_origin(-(uring.dfRadiusOfRing + uring.dfRingBs / 2), uring.dfNamedL - uring.dfRadiusOfRing, 0);
		f3dPoint subface_origin(-(uring.dfBoardSpaceC + uring.dfRingBs) / 2, fLengthFromBoard, 0);
		face_ucs.origin = top_origin;
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)
		{	//进行顶部平面点的转换并添加
			f3dPoint vertex = org_section_vertex_arr[j];
			coord_trans(vertex, face_ucs, TRUE);
			pBody->vertex.append(vertex);
		}
		face_ucs.origin = subface_origin;
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)//进行底部平面点的转换（斜坡）
			coord_trans(org_section_vertex_arr[j], face_ucs, TRUE);
		double fChordX = -(uring.dfBoardSpaceC / 2 + uring.dfT);//相交弦的x坐标
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)
		{
			f3dPoint vertex = org_section_vertex_arr[j];
			if (vertex.x < fChordX)//
			{	//U型环在Y的正半轴，所以Y坐标增加
				vertex.y += fabs(vertex.x - fChordX) / fTaper;
				org_section_vertex_arr[j] = vertex;
			}
			else if (vertex.x == fChordX)
			{
				if (nChordVertex[0] < 0)
					nChordVertex[0] = j;
				else if (nChordVertex[1] < 0)
					nChordVertex[1] = j;
			}
			else if (vertex.x > fChordX)
			{	//点的x坐标在相交面内，判断最接近相交弦的点
				f3dPoint vertex1, vertex2;
				int index = -1;
				if (j > 0)
					vertex1 = org_section_vertex_arr[(j - 1) % slices];
				vertex2 = org_section_vertex_arr[(j + 1) % slices];
				if (vertex1.x < fChordX)
					index = (j - 1) % slices;
				else if(vertex2.x < fChordX)
					index = (j + 1) % slices;
				if (nChordVertex[0] < 0)
					nChordVertex[0] = index;
				else if (nChordVertex[1] < 0)
					nChordVertex[1] = index;
			}
		}
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)//进行底部平面点的转换（斜坡）
		{
			f3dPoint vertex = org_section_vertex_arr[j];
			pBody->vertex.append(vertex);
		}
	}
	else
	{
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)//进行底部平面点的转换（斜坡）
		{	//右侧圆柱点经过半环旋转圆点进行翻转
			org_section_vertex_arr[j].x *= -1;
		}
		f3dPoint top_origin(uring.dfRadiusOfRing + uring.dfRingBs / 2, uring.dfNamedL - uring.dfRadiusOfRing, 0);
		f3dPoint subface_origin((uring.dfBoardSpaceC + uring.dfRingBs) / 2, fLengthFromBoard, 0);
		face_ucs.origin = top_origin;
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)
		{	//进行顶部平面点的转换并添加
			f3dPoint vertex = org_section_vertex_arr[j];
			coord_trans(vertex, face_ucs, TRUE);
			pBody->vertex.append(vertex);
		}
		face_ucs.origin = subface_origin;
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)//进行底部平面点的转换（斜坡）
			coord_trans(org_section_vertex_arr[j], face_ucs, TRUE);
		double fChordX = uring.dfBoardSpaceC / 2 + uring.dfT;//相交弦的x坐标
		
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)
		{
			f3dPoint vertex = org_section_vertex_arr[j];
			if (vertex.x > fChordX)//
			{	//U型环在Y的正半轴，所以Y坐标增加
				vertex.y += fabs(vertex.x - fChordX) / fTaper;
				org_section_vertex_arr[j] = vertex;
			}
			else if (vertex.x == fChordX)
			{
				if (nChordVertex[0] < 0)
					nChordVertex[0] = j;
				else if (nChordVertex[1] < 0)
					nChordVertex[1] = j;
			}
			else if (vertex.x < fChordX)
			{	//点的x坐标在相交面内，判断最接近相交弦的点
				f3dPoint vertex1, vertex2;
				int index = -1;
				if (j > 0)
					vertex1 = org_section_vertex_arr[(j - 1) % slices];
				vertex2 = org_section_vertex_arr[(j + 1) % slices];
				if (vertex1.x > fChordX)
					index = (j - 1) % slices;
				else if (vertex2.x > fChordX)
					index = (j + 1) % slices;
				if (nChordVertex[0] < 0)
					nChordVertex[0] = index;
				else if (nChordVertex[1] < 0)
					nChordVertex[1] = index;
			}
		}
		for (DWORD j = 0; j < org_section_vertex_arr.Size(); j++)//进行底部平面点的转换（斜坡）
		{
			f3dPoint vertex = org_section_vertex_arr[j];
			pBody->vertex.append(vertex);
		}
	}
	int minIndex = max(nChordVertex[0], 0);
	int maxIndex = min(nChordVertex[1], slices);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4] = { 0,0,0,0 };
	f3dPolyFace *pFace = pBody->faceList.append();
	
	//圆柱体底面
	pFace->poly_norm.Set(0, -1, 0);
	pFace->material = bolt_color;
	
	for (int i = maxIndex; i >= minIndex; i--)
		createFace.NewOutterEdgeLine(pFace, (slices + i - 1) % slices + liStartIndex + slices, i + liStartIndex + slices);
	//圆柱体侧面
	for (int i = 0; i < slices; i++)
	{
		pFace = pBody->faceList.append();
		pFace->material = bolt_color;
		if (bLeft)
		{
			createFace.NewOutterEdgeLine(pFace, (i + 1) % slices + liStartIndex, i + liStartIndex);
			createFace.NewOutterEdgeLine(pFace, (i + 1) % slices + liStartIndex + slices);
			createFace.NewOutterEdgeLine(pFace, i + liStartIndex + slices);
			createFace.NewOutterEdgeLine(pFace, i + liStartIndex);
		}
		else
		{
			createFace.NewOutterEdgeLine(pFace, i + liStartIndex, (i + 1) % slices + liStartIndex);
			createFace.NewOutterEdgeLine(pFace, i + liStartIndex + slices);
			createFace.NewOutterEdgeLine(pFace, (i + 1) % slices + liStartIndex + slices);
			createFace.NewOutterEdgeLine(pFace, (i + 1) % slices + liStartIndex);
		}
	}
	return pBody->vertex.GetNodeNum();
}
bool CURing::Create3dSolidModel(CSolidBody *pSolidBody)
{
	fBody body;
	if(!AppendSolidBody(&body))
		return FALSE;
	pSolidBody->ConvertFrom(&body);
	return TRUE;
}
bool CURing::AppendSolidBody(fBody *pBody,bool blTransToWorkEnv/*=true*/)
{
	long liInitVertexIndex = pBody->vertex.GetNodeNum();
	long liInitFaceIndex = pBody->faceList.GetNodeNum();
	COLORREF crRing = RGB(145, 149, 145), crLs = RGB(91, 91, 91);
	//环主体
	AppendURBend(pBody, 64, crRing);
	//转轴螺杆
	GECS pcs(GEPOINT(0, 0, 0), GEPOINT(0, 0, -1), GEPOINT(0, 1, 0), GEPOINT(1, 0, 0));
	double fShefLength = uring.dfBoardSpaceC + uring.dfT * 2 + 20;//连接间隙 + 耳板厚度 + 螺栓预留长度；
	pcs.origin.x = -uring.dfBoardSpaceC*0.5 - uring.dfT;
	METASOLID::AppendDiskSolid(pBody, uring.dfShaftD, fShefLength, pcs, 64, crLs);
	//耳板
	double fOutR = uring.dfMdEdgeSpace;
	pcs.origin.x = -uring.dfBoardSpaceC*0.5 - uring.dfT;
	METASOLID::AppendTubeSolid(pBody, fOutR*2, fOutR-uring.dfShaftD*0.5, uring.dfT, pcs, 64, crRing);	//左侧耳板
	pcs.origin.x = uring.dfBoardSpaceC*0.5;
	METASOLID::AppendTubeSolid(pBody, fOutR*2, fOutR-uring.dfShaftD*0.5, uring.dfT, pcs, 64, crRing);	//右侧耳板
	//螺母
	pcs.origin.x = -uring.dfBoardSpaceC*0.5 - uring.dfT - m_fNutH;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);	//头部螺母
	pcs.origin.x = uring.dfBoardSpaceC*0.5 + uring.dfT;
	METASOLID::AppendTaperNutSolid(pBody, m_fNutDe, m_fNutDs, m_fNutH, pcs, 48, crLs);	//尾部螺母
	if (blTransToWorkEnv)
	{	//装配到模型指定(相对)位置
		int i = 0;
		f3dPolyFace* pFace;
		f3dPoint* pVertice;
		for (pVertice = pBody->vertex.GetFirst(),i=0; pVertice; pVertice = pBody->vertex.GetNext(), i++)
		{
			if (i < liInitVertexIndex)
				continue;	//原有顶点
			GEPOINT xActualPoint = this->acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x, xActualPoint.y, xActualPoint.z);
		}
		for (pFace = pBody->faceList.GetFirst(),i=0; pFace; pFace = pBody->faceList.GetNext(),i++)
		{
			if (i < liInitFaceIndex)
				continue;	//原有顶点
			if (pFace->poly_norm.IsZero())
				continue;
			pFace->poly_norm = this->acs.TransVFromCS(pFace->poly_norm);
		}
	}
	return TRUE;
}
