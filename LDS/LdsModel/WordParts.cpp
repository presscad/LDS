#include "StdAfx.h"
#include "XhLdsLm.h"
#include "WordParts.h"
#include "CreateFace.h"
#include "lds_part.h"

CWordPartsMaker::CWordPartsMaker()
{
	m_fThick = 10;
	m_fRadio = 1;
	m_crWord = RGB(204, 190, 17);
	this->xInitOrgin.Set(47.339,-33.2423,0);
}
void CWordPartsMaker::AppendWords(GECS acs,IModel* pModel)
{
#pragma region 花字
	//草字头
	CLDSPlate* pWordPlateOfHuaStroke1=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(27.0413,-4.8967,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(24.2861,-1.8349,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(23.3231,-3.7832,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(20.1154,-3.7832,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(20.1154,-0.136,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(16.7679,-0.136,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(16.7679,-3.7618,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(13.0696,-3.7618,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(13.0696,-0.126,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(9.6104,-0.126,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(9.6104,-3.7702,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(4.0524,-3.7702,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(4.5474,-6.9964,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(9.6369,-6.9964,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(9.6369,-10.0868,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(12.9907,-10.0868,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(12.9907,-7.0148,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(16.7442,-7.0148,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(16.7442,-9.651,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(20.1195,-9.651,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(20.1195,-7.0289,0));
	pWordPlateOfHuaStroke1->vertex_list.append(PROFILE_VERTEX(26.1535,-7.0289,0));
	pWordPlateOfHuaStroke1->crMaterial=this->m_crWord;
	pWordPlateOfHuaStroke1->Thick=m_fThick;
	pWordPlateOfHuaStroke1->ucs=acs;
	pWordPlateOfHuaStroke1->designInfo.m_bEnableFlexibleDesign=false;
	PROFILE_VERTEX* pVertex;
	for (pVertex=pWordPlateOfHuaStroke1->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfHuaStroke1->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
	//左下单立人
	CLDSPlate* pWordPlateOfHuaStroke2=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(14.4305,-12.4335,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(10.7294,-10.0122,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(10.085,-11.8215,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(8.7001,-15.1237,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(7.3366,-17.8512,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(5.7674,-20.6484,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(4.156 ,-23.0511,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(5.5371,-24.5928,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(8.5469,-21.4057,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(8.5469,-33.2419,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(11.7805,-33.2419,0));
	pWordPlateOfHuaStroke2->vertex_list.append(PROFILE_VERTEX(11.7805,-17.0442,0));
	pWordPlateOfHuaStroke2->crMaterial=this->m_crWord;
	pWordPlateOfHuaStroke2->Thick=m_fThick;
	pWordPlateOfHuaStroke2->ucs=acs;
	pWordPlateOfHuaStroke2->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfHuaStroke2->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfHuaStroke2->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
	//右下'匕'
	CLDSPlate* pWordPlateOfHuaStroke3=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(25.4635,-16.4904,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(22.6375,-12.8503,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(19.2191,-17.4596,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(19.2191,-11.0385,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(15.9966,-11.0385,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(15.9966,-20.8153,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(12.3668,-23.8943,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(13.4548,-26.1202,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(15.992,-24.5833,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(15.992,-29.117,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(16.9255,-31.6781,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(19.1239,-32.5463,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(23.6613,-32.5463,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(25.4473,-31.3206,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(26.4387,-28.0813,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(25.6022,-27.4369,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(25.3765,-24.0292,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(24.101,-23.6041,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(23.4128,-26.7152,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(22.1518,-27.9362,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(20.391,-27.9323,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(19.2205,-26.2479,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(19.2205,-22.2079,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(21.9294,-19.982,0));
	pWordPlateOfHuaStroke3->vertex_list.append(PROFILE_VERTEX(24.2891,-17.7958,0));
	pWordPlateOfHuaStroke3->crMaterial=this->m_crWord;
	pWordPlateOfHuaStroke3->Thick=m_fThick;
	pWordPlateOfHuaStroke3->ucs=acs;
	pWordPlateOfHuaStroke3->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfHuaStroke3->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfHuaStroke3->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
#pragma endregion

#pragma region 千
	CLDSPlate* pWordPlateOfQian=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(56.1748,-5.309,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(54.2191,-0.4291,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(50.8696,-1.7674,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(47.3471,-2.9426,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(43.2823,-4.0619,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(39.7797,-4.858,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(38.2884,-5.1733,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(38.679,-7.5832,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(42.9198,-7.2732,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(45.4214,-6.9925,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(45.4214,-14.2627,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(36.0412,-14.2627,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(36.7179,-17.5978,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(45.419,-17.5978,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(45.419,-33.2423,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(49.2514,-33.2423,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(49.2514,-17.695,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(58.5137,-17.695,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(59.2081,-15.5122,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(55.8839,-11.8727,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(54.5063,-14.2449,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(49.2686,-14.2449,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(49.2686,-6.5962,0));
	pWordPlateOfQian->vertex_list.append(PROFILE_VERTEX(52.0996,-6.1807,0));
	pWordPlateOfQian->crMaterial=this->m_crWord;
	pWordPlateOfQian->Thick=m_fThick;
	pWordPlateOfQian->ucs=acs;
	pWordPlateOfQian->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfQian->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfQian->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
#pragma endregion

#pragma region 谷
	//左上短撇
	CLDSPlate* pWordPlateOfGuStroke1=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(75.6287,-0.0014,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(74.3366,-2.4302,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(72.5437,-5.3079,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(71.0933,-7.3086,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(68.872,-9.9476,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(70.0325,-11.8847,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(71.6483,-10.7453,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(72.5436,-10.0517,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(73.4335,-9.2945,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(77.0496,-5.7748,0));
	pWordPlateOfGuStroke1->vertex_list.append(PROFILE_VERTEX(78.8839,-3.6494,0));
	pWordPlateOfGuStroke1->crMaterial=this->m_crWord;
	pWordPlateOfGuStroke1->Thick=m_fThick;
	pWordPlateOfGuStroke1->ucs=acs;
	pWordPlateOfGuStroke1->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfGuStroke1->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfGuStroke1->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
	//右上钝点
	CLDSPlate* pWordPlateOfGuStroke2=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(82.1779,-1.1182,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(81.201,-3.4919,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(83.8963,-6.3571,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(85.6428,-8.3581,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(87.5613,-10.6287,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(88.926,-10.1975,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(89.6932,-6.328,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(89.3414,-4.6433,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(87.8659,-3.7773,0));
	pWordPlateOfGuStroke2->vertex_list.append(PROFILE_VERTEX(85.0838,-2.4022,0));
	pWordPlateOfGuStroke2->crMaterial=this->m_crWord;
	pWordPlateOfGuStroke2->Thick=m_fThick;
	pWordPlateOfGuStroke2->ucs=acs;
	pWordPlateOfGuStroke2->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfGuStroke2->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfGuStroke2->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
	//中间人
	CLDSPlate* pWordPlateOfGuStroke3=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(78.9423,-6.1633,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(77.4006,-9.3246,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(75.6501,-12.2877,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(73.3875,-15.4778,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(70.2791,-18.767,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(68.0095,-20.8638,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(69.0388,-23.177,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(73.0851,-20.2561,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(73.0851,-33.106,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(76.5256,-33.106,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(76.5256,-17.742,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(75.9226,-17.738,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(78.4152,-14.8805,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(79.9292,-12.7399,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(81.5542,-15.2283,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(83.3187,-17.4631,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(82.7509,-18.5674,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(82.7509,-33.1546,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(86.3532,-33.1546,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(86.3532,-21.778,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(86.9926,-20.8725,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(89.3199,-22.5101,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(90.2098,-19.7113,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(91.5676,-16.772,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(90.0608,-16.2831,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(87.9865,-15.355,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(85.7295,-14.079,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(83.4633,-12.3581,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(81.523,-10.402,0));
	pWordPlateOfGuStroke3->vertex_list.append(PROFILE_VERTEX(82.4399,-8.6952,0));
	pWordPlateOfGuStroke3->crMaterial=this->m_crWord;
	pWordPlateOfGuStroke3->Thick=m_fThick;
	pWordPlateOfGuStroke3->ucs=acs;
	pWordPlateOfGuStroke3->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfGuStroke3->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfGuStroke3->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
	//底部口上横
	CLDSPlate* pWordPlateOfGuStroke4=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfGuStroke4->vertex_list.append(PROFILE_VERTEX(82.7509,-18.5674,0));
	pWordPlateOfGuStroke4->vertex_list.append(PROFILE_VERTEX(76.5256,-18.5674,0));
	pWordPlateOfGuStroke4->vertex_list.append(PROFILE_VERTEX(76.5256,-21.9469,0));
	pWordPlateOfGuStroke4->vertex_list.append(PROFILE_VERTEX(82.7509,-21.9469,0));
	pWordPlateOfGuStroke4->crMaterial=this->m_crWord;
	pWordPlateOfGuStroke4->Thick=m_fThick;
	pWordPlateOfGuStroke4->ucs=acs;
	pWordPlateOfGuStroke4->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfGuStroke4->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfGuStroke4->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
	//底部口下横
	CLDSPlate* pWordPlateOfGuStroke5=(CLDSPlate*)pModel->AppendPart(CLS_PLATE);
	pWordPlateOfGuStroke5->vertex_list.append(PROFILE_VERTEX(82.7509,-27.3746,0));
	pWordPlateOfGuStroke5->vertex_list.append(PROFILE_VERTEX(76.5256,-27.3746,0));
	pWordPlateOfGuStroke5->vertex_list.append(PROFILE_VERTEX(76.5256,-30.5932,0));
	pWordPlateOfGuStroke5->vertex_list.append(PROFILE_VERTEX(82.7509,-30.5932,0));
	pWordPlateOfGuStroke5->crMaterial=this->m_crWord;
	pWordPlateOfGuStroke5->Thick=m_fThick;
	pWordPlateOfGuStroke5->ucs=acs;
	pWordPlateOfGuStroke5->designInfo.m_bEnableFlexibleDesign=false;
	for (pVertex=pWordPlateOfGuStroke5->vertex_list.GetFirst();pVertex;pVertex=pWordPlateOfGuStroke5->vertex_list.GetNext())
		pVertex->vertex-=xInitOrgin;
#pragma endregion
}

CWordPartsMaker::CWordPartsMaker(const char* label)
{
	m_fThick = 10;
	m_fRadio = 1;
	LoadSizeParams(label);
}

CWordPartsMaker::~CWordPartsMaker()
{
}

BOOL CWordPartsMaker::LoadSizeParams(const char* label)
{
	return TRUE;
}

int CWordPartsMaker::CreatWordPart(fBody* pBody, GEPOINT* pGepoint, int nGepoint, double fThick, double fRadio /* = 1 */)
{
	int i;
	long liStartIndex = pBody->vertex.GetNodeNum();
	PRESET_ARRAY128<GEPOINT> xarrVertexes;
	for (i = 0; i < nGepoint; i++)
	{
		GEPOINT point = pGepoint[i];
		xarrVertexes.Append(GEPOINT(point.x*m_fRadio, point.y*m_fRadio, point.z*m_fRadio));
	}
	//添加点到pBody
	for (i = 0; i < (int)xarrVertexes.Count; i++)	
		pBody->vertex.append(f3dPoint(xarrVertexes.At(i).x, xarrVertexes.At(i).y, fThick *0.5));
	for (i = 0; i < (int)xarrVertexes.Count; i++)	
		pBody->vertex.append(f3dPoint(xarrVertexes.At(i).x, xarrVertexes.At(i).y, -fThick *0.5));
	int count = (int)xarrVertexes.Count;

	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4] = { 0,0,0,0 };
	COLORREF crWord = m_crWord;
	f3dPolyFace* pTopFace; 
	f3dPolyFace* pBtmFace;
	//绘制挂板顶面
	pTopFace = pBody->faceList.append();
	pTopFace->material = crWord;
	for (i = 0; i < count; i++)
		createFace.NewOutterEdgeLine(pTopFace, liStartIndex + (i + 1) % count, liStartIndex + i);
	//绘制挂板底面
	pBtmFace = pBody->faceList.append();
	pBtmFace->material = crWord;
	for (i = count - 1; i >= 0; i--)
		createFace.NewOutterEdgeLine(pBtmFace, liStartIndex + count + (count + i - 1) % count, liStartIndex + count + i);
	//绘制挂板侧面
	for (i = 0; i < count; i++)//外侧
	{
		f3dPolyFace* pWallFace = pBody->faceList.append();
		pWallFace->material = crWord;
		createFace.NewOutterEdgeLine(pWallFace, liStartIndex + count + (i + 1) % count, liStartIndex + count + i);
		createFace.NewOutterEdgeLine(pWallFace, liStartIndex + (i + 1) % count);
		createFace.NewOutterEdgeLine(pWallFace, liStartIndex + i);
		createFace.NewOutterEdgeLine(pWallFace, liStartIndex + count + i);
	}
	return pBody->vertex.GetNodeNum();
}

BOOL CWordPartsMaker::Create3dSolidModel(CSolidBody *pSolidBody)
{
	fBody body;
	for (int i=0;i<m_nGepointArr;i++)
	{
		GEPOINT* ppGepoint = m_gepoint[i];
		int nGepointArrCount = m_nGepointArrCount[i];
		CreatWordPart(&body,ppGepoint,nGepointArrCount,m_fThick,m_fRadio);
	}
	pSolidBody->ConvertFrom(&body);
	return TRUE;
}