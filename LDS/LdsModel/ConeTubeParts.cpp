// ConeTubeParts.cpp: implementation of the CLDSSlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConeTubeParts.h"
#include "CreateFace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#ifdef __GGG_
CLDSConeTubeFl::CLDSConeTubeFl()
{
	m_fDownRadius = 100;
	m_fUpRadius =50;
	m_fThick = 10;
	m_nFaceNum = 6;
	m_basePt.Set(0,0,0);
	m_fOutsideR = 400;
}
CLDSConeTubeFl::CLDSConeTubeFl(double upRadius,double downRadius,double thick,int faceNum,double outsideR)
{
	m_fDownRadius = downRadius;
	m_fUpRadius =upRadius;
	m_fThick = thick;
	m_nFaceNum = faceNum;
	m_basePt.Set(0,0,0);
	m_fOutsideR = outsideR;
}
CLDSConeTubeFl::~CLDSConeTubeFl()
{

}
void CLDSConeTubeFl::SetDownRadius(double nR)
{
	m_fDownRadius = nR;
}
void CLDSConeTubeFl::SetUpRadius(double nR)
{
	m_fUpRadius = nR;
}
void CLDSConeTubeFl::SetThick(double nT)
{
	m_fThick = nT;
}
void CLDSConeTubeFl::SetOutsideR(double nR)
{
	m_fOutsideR = nR;
}
void CLDSConeTubeFl::SetBasePt(f3dPoint pt)
{
	m_basePt = pt;
}
void CLDSConeTubeFl::SetFaceNum(int nNum)
{
	m_nFaceNum = nNum;
}
static void CreateLsHole(CLDSParamPlate *pParamPlate,fBody *pBody,f3dPolyFace *pTopFace,f3dPolyFace *pBottomFace)
{
	if(pParamPlate==NULL)
		return;
	CHoleMaker holeMaker;
	GECS hole_ucs;
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{
		hole_ucs.origin=(*pLsRef)->ucs.origin;
		hole_ucs.axis_z=(*pLsRef)->get_norm();
		hole_ucs.axis_x=(*pLsRef)->ucs.axis_x;
		//hole_ucs.axis_x=inters_vec(hole_ucs.axis_z);
		//hole_ucs.axis_y=hole_ucs.axis_z^hole_ucs.axis_x;
		//hole_ucs.axis_x=hole_ucs.axis_y^hole_ucs.axis_z;
		COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,(*pLsRef)->get_d(),' ');
		holeMaker.Create(hole_ucs,(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment,pLsRef->waistLen,pTopFace,pBottomFace,pBody,crLsHole);
	}
}

void CLDSConeTubeFl::Create3dSolidModel(UCS_STRU ucs)
{
	f3dPoint upPt = m_basePt - ucs.axis_z*m_fThick;
	f3dLine vec(m_basePt,upPt);
	f3dPoint firstPt = m_basePt + ucs.axis_x*m_fOutsideR;
	f3dPoint firstPtInside = m_basePt + ucs.axis_x*m_fDownRadius;
	f3dPoint firstPt1 = upPt + ucs.axis_x*m_fOutsideR;
	f3dPoint firstPtInside1 = upPt + ucs.axis_x*m_fUpRadius;
	if(pSolidBody!=NULL&& !IsModified())
		return;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	fBody body;
	fBody* pBody=&body;
	fPtList downLoop,upLoop;
	int slices= CalArcResolution(m_fOutsideR*0.5,2*Pi,1.0,5.0,36);
	pBody->vertex.append(firstPt);
	pBody->vertex.append(firstPt1);
	int i = 0;
	for (i = 0;i<slices;i++)
	{
		f3dPoint nextPt = firstPt;
		rotate_point_around_axis(nextPt,2*Pi/slices,vec);
		pBody->vertex.append(nextPt);
		firstPt = nextPt;
		
		f3dPoint nextPt1 = firstPt1;
		rotate_point_around_axis(nextPt1,2*Pi/slices,vec);
		pBody->vertex.append(nextPt1);
		firstPt1 = nextPt1;
	}
	pBody->vertex.append(firstPtInside);
	pBody->vertex.append(firstPtInside1);
	for (i = 0;i<m_nFaceNum;i++)
	{
		f3dPoint nextInsidePt = firstPtInside;
		rotate_point_around_axis(nextInsidePt,2*Pi/m_nFaceNum,vec);
		pBody->vertex.append(nextInsidePt);
		downLoop.append(firstPtInside);
		firstPtInside = nextInsidePt;
		
		f3dPoint nextInsidePt1 = firstPtInside1;
		rotate_point_around_axis(nextInsidePt1,2*Pi/m_nFaceNum,vec);
		pBody->vertex.append(nextInsidePt1);
		upLoop.append(firstPtInside1);
		firstPtInside1 = nextInsidePt1;
	}
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	f3dPolyFace *pFace;
	COLORREF tube_color=GetColor(CLS_PLATE,3,cMaterial);
	//里外侧面创建
	for (i=0;i<slices+1;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material = tube_color;

		createFace.NewOutterEdgeLine(pFace,((i+1)%(slices+1))*2,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%(slices+1))*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
	int num = slices+m_nFaceNum+2;
	for (i = slices+1;i<num;i++)
	{
		pFace = pBody->faceList.append();
		pFace->material = tube_color;
	
		if (i==num-1)
		{
			createFace.NewOutterEdgeLine(pFace,i*2,(slices+1)*2);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,(slices+1)*2+1);
			createFace.NewOutterEdgeLine(pFace,(slices+1)*2);
		}
		else
		{
			createFace.NewOutterEdgeLine(pFace,i*2,((i+1)%num)*2);
			createFace.NewOutterEdgeLine(pFace,i*2+1);
			createFace.NewOutterEdgeLine(pFace,((i+1)%num)*2+1);
			createFace.NewOutterEdgeLine(pFace,((i+1)%num)*2);
		}	
	}
	f3dPolyFace *pDownFace,*pUpFace;
	pDownFace=pBody->faceList.append();
	pDownFace->material = tube_color;
	fLoop *pDownLoopTemp = pDownFace->inner_loop.append();
	for (f3dPoint* pCurPt = downLoop.GetFirst();pCurPt;pCurPt=downLoop.GetNext())
	{
		f3dPoint* pNextPt = downLoop.GetNext();
		if (pNextPt==NULL)
		{
			pNextPt = downLoop.GetFirst();
			downLoop.GetTail();
		}
		else
			downLoop.GetPrev();
	//	pDownLoopTemp->append(pNextPt,pCurPt);
	}
	createFace.NewOutterEdgeLine(pDownFace,0,2*slices-2);
	for (i=slices-2;i>=0;i--)
		createFace.NewOutterEdgeLine(pDownFace,(i*2)%(2*slices));
	
	pUpFace=pBody->faceList.append();
	pUpFace->material = tube_color;
	fLoop *pUpLoopTemp = pUpFace->inner_loop.append();
	for (pCurPt = upLoop.GetFirst();pCurPt;pCurPt=upLoop.GetNext())
	{
		f3dPoint* pNextPt = upLoop.GetNext();
		if (pNextPt==NULL)
		{
			pNextPt = upLoop.GetFirst();
			upLoop.GetTail();
		}
		else
			upLoop.GetPrev();
	//	pUpLoopTemp->append(pNextPt,pCurPt);
	}
	createFace.NewOutterEdgeLine(pUpFace,1,3);
	for (i=2;i<=slices;i++)
		createFace.NewOutterEdgeLine(pUpFace,(i*2+1)%(2*slices));
	CreateLsHole(this,pBody,pUpFace,pDownFace);
	pSolidBody->ConvertFrom(pBody);
}
#endif
