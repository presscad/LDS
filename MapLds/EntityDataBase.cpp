// EntityDataBase.cpp: implementation of the CEntityDataBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "EntityDataBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifndef __LOCALDEF_ENTITYDATABASE_
CEntityDataBase::CEntityDataBase()
{
	ID = 0;
	EXT_ID = 0;
	m_bActive = TRUE;
	cur_oper_env.view_rect.topLeft.Set(-1e7,-1e7);
	cur_oper_env.view_rect.bottomRight.Set(1e7,1e7);
	cur_oper_env.m_fScalePortToUser = 1.0;
	cur_oper_env.m_ucs3dPortInUser.axis_x.Set(1,0,0);
	cur_oper_env.m_ucs3dPortInUser.axis_y.Set(0,1,0);
	cur_oper_env.m_ucs3dPortInUser.axis_x.Set(0,0,1);
	cur_oper_env.m_ucs2dPortInUser.axis_x.Set(1,0);
	cur_oper_env.m_ucs2dPortInUser.axis_y.Set(0,1);
	cur_oper_env.view_rect.SetRect(f2dPoint(0,0),f2dPoint(640,480));
}

CEntityDataBase::~CEntityDataBase()
{
	Empty();
}

BOOL CEntityDataBase::GetScopeSize(SCOPE_STRU *scope)
{
	int i,n;
	BOOL IsHasObject = FALSE;
	f3dPoint *point;
	f3dLine *line;
	f3dCircle *circle;
	f3dRect *rect;
	scope->fMinX = scope->fMinY = scope->fMinZ = 1.0e99;//无穷大
	scope->fMaxX = scope->fMaxY = scope->fMaxZ =-1.0e99;//无穷小

	n = m_arrPoint3d.GetSize();
	for(i=0;i<n;i++)
	{
		IsHasObject = TRUE;
		point = &m_arrPoint3d[i];
		scope->fMaxX = __max(scope->fMaxX, point->x);
		scope->fMaxY = __max(scope->fMaxY, point->y);
		scope->fMaxZ = __max(scope->fMaxZ, point->z);
		scope->fMinX = __min(scope->fMinX, point->x);
		scope->fMinY = __min(scope->fMinY, point->y);
		scope->fMinZ = __min(scope->fMinZ, point->z);
	}
	n = m_arrLine3d.GetSize();
	for(i=0;i<n;i++)
	{
		IsHasObject = TRUE;
		line = &m_arrLine3d[i];
		scope->fMaxX = __max(scope->fMaxX, line->startPt.x);
		scope->fMaxY = __max(scope->fMaxY, line->startPt.y);
		scope->fMaxZ = __max(scope->fMaxZ, line->startPt.z);
		scope->fMinX = __min(scope->fMinX, line->startPt.x);
		scope->fMinY = __min(scope->fMinY, line->startPt.y);
		scope->fMinZ = __min(scope->fMinZ, line->startPt.z);

		scope->fMaxX = __max(scope->fMaxX, line->endPt.x);
		scope->fMaxY = __max(scope->fMaxY, line->endPt.y);
		scope->fMaxZ = __max(scope->fMaxZ, line->endPt.z);
		scope->fMinX = __min(scope->fMinX, line->endPt.x);
		scope->fMinY = __min(scope->fMinY, line->endPt.y);
		scope->fMinZ = __min(scope->fMinZ, line->endPt.z);
	}
	n = m_arrCircle3d.GetSize();
	for(i=0;i<n;i++)
	{
		IsHasObject = TRUE;
		circle = &m_arrCircle3d[i];
		scope->fMaxX = __max(scope->fMaxX, circle->centre.x);
		scope->fMaxY = __max(scope->fMaxY, circle->centre.y);
		scope->fMaxZ = __max(scope->fMaxZ, circle->centre.z);
		scope->fMinX = __min(scope->fMinX, circle->centre.x);
		scope->fMinY = __min(scope->fMinY, circle->centre.y);
		scope->fMinZ = __min(scope->fMinZ, circle->centre.z);
	}
	n = m_arrRect3d.GetSize();
	for(i=0;i<n;i++)
	{
		IsHasObject = TRUE;
		rect = &m_arrRect3d[i];
		scope->fMaxX = __max(scope->fMaxX, rect->topLeft.x);
		scope->fMaxY = __max(scope->fMaxY, rect->topLeft.y);
		scope->fMaxZ = __max(scope->fMaxZ, rect->topLeft.z);
		scope->fMinX = __min(scope->fMinX, rect->topLeft.x);
		scope->fMinY = __min(scope->fMinY, rect->topLeft.y);
		scope->fMinZ = __min(scope->fMinZ, rect->topLeft.z);

		scope->fMaxX = __max(scope->fMaxX, rect->bottomRight.x);
		scope->fMaxY = __max(scope->fMaxY, rect->bottomRight.y);
		scope->fMaxZ = __max(scope->fMaxZ, rect->bottomRight.z);
		scope->fMinX = __min(scope->fMinX, rect->bottomRight.x);
		scope->fMinY = __min(scope->fMinY, rect->bottomRight.y);
		scope->fMinZ = __min(scope->fMinZ, rect->bottomRight.z);
	}
	if(!IsHasObject)
	{
		scope->fMinX = scope->fMinY = scope->fMinZ = -500;//无穷大
		scope->fMaxX = scope->fMaxY = scope->fMaxZ =  500;//无穷小
	}
	return IsHasObject;
}

void CEntityDataBase::Empty()
{
	m_arrLine3d.RemoveAll();
	m_arrPoint3d.RemoveAll();
	m_arrCircle3d.RemoveAll();
	m_arrRect3d.RemoveAll();
	m_arrMark.RemoveAll();
}
//const BYTE IDbEntity::DbPoint	= 1;
//const BYTE IDbEntity::DbLine	= 2;
//const BYTE IDbEntity::DbArcline	= 3;
//const BYTE IDbEntity::DbRect	= 4;
//const BYTE IDbEntity::DbArrow	= 5;
//const BYTE IDbEntity::DbMark	= 6;
//const BYTE IDbEntity::DbPolyline= 7;
//const BYTE IDbEntity::DbCircle	= 8;
//const BYTE IDbEntity::DbSpline	= 9;
//fAtom* CEntityDataBase::AppendDbEntity(int idDbEntType)
//{
//	if(idDbEntType==IDbEntity::DbLine)
//		return m_arrLine3d.Add(f3dLine());
//	else
//		return NULL;
//}
#endif