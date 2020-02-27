// EntityDataBase.cpp: implementation of the CEntityDataBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "EntityDataBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntityDataBase::CEntityDataBase()
{

}

CEntityDataBase::~CEntityDataBase()
{
	Empty();
}

void CEntityDataBase::GetScopeSize(SCOPE_STRU *scope)
{
	scope->ClearScope();	//Çå¿ÕÏÔÊ¾·¶Î§
	BOOL bHasPoint=FALSE;
	for(f3dPoint *pPoint=m_arrPoint3d.GetFirst();pPoint;pPoint=m_arrPoint3d.GetNext())
	{
		bHasPoint=TRUE;
		scope->VerifyVertex(*pPoint);
	}
	for(f3dLine *pLine=m_arrLine3d.GetFirst();pLine;pLine=m_arrLine3d.GetNext())
	{
		bHasPoint=TRUE;
		scope->VerifyVertex(pLine->startPt);
		scope->VerifyVertex(pLine->endPt);
	}
	for(f3dCircle *pCir=m_arrCircle3d.GetFirst();pCir;pCir=m_arrCircle3d.GetNext())
	{
		bHasPoint=TRUE;
		f3dPoint pt1(pCir->centre.x-pCir->radius,pCir->centre.y-pCir->radius,pCir->centre.z-pCir->radius);
		f3dPoint pt2(pCir->centre.x+pCir->radius,pCir->centre.y+pCir->radius,pCir->centre.z+pCir->radius);
		scope->VerifyVertex(pt1);
		scope->VerifyVertex(pt2);
	}
	for(f3dRect *pRect=m_arrRect3d.GetFirst();pRect;pRect=m_arrRect3d.GetNext())
	{
		bHasPoint=TRUE;
		scope->VerifyVertex(pRect->topLeft);
		scope->VerifyVertex(pRect->bottomRight);
	}
	if(bHasPoint)
	{
		if(scope->wide()<=0.0)
		{
			scope->fMinX -= 250;
			scope->fMaxX += 250;
		}
		if(scope->high()<=0.0)
		{
			scope->fMinY -= 250;
			scope->fMaxY += 250;
		}
	}
	else
	{
		if(scope->wide()<=0.0)
		{
			scope->fMinX = -250;
			scope->fMaxX =  250;
		}
		if(scope->high()<=0.0)
		{
			scope->fMinY = -250;
			scope->fMaxY =  250;
		}
	}
}

void CEntityDataBase::Empty()
{
	m_arrLine3d.Empty();
	m_arrPoint3d.Empty();
	m_arrCircle3d.Empty();
	m_arrRect3d.Empty();
	m_arrMark.Empty();
}
