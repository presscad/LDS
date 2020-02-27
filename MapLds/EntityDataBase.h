// EntityDataBase.h: interface for the CEntityDataBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITYDATABASE_H__D8EF80D6_6C10_4C71_A2E2_EDDE2BAB943E__INCLUDED_)
#define AFX_ENTITYDATABASE_H__D8EF80D6_6C10_4C71_A2E2_EDDE2BAB943E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
class CEntityDataBase
{
	friend class CPlatTbl;
	typedef struct{
		UCS_STRU m_ucs3dPortInUser;		//用于旋转的用户坐标系
		UCS_2dSTRU m_ucs2dPortInUser;	//用于平移的中间坐标系(与屏幕坐标只是Y轴反向的区别)
		double m_fScalePortToUser;		//屏幕到用户的坐标比例(用于图形缩放操作)
		f2dRect view_rect;				//可视区域(或裁剪窗口)
	}OPER_ENV;
	OPER_ENV old_oper_env,cur_oper_env;
public:
	long ID;			//标识ID  （可能暂时无用）
	long EXT_ID;		//扩展标识ID  （可能暂时无用）
	BOOL m_bActive;		//激活状态（可能暂时无用）
	void Empty();
	BOOL GetScopeSize(SCOPE_STRU *scope);
	CEntityDataBase();
	~CEntityDataBase();
	CArray<f3dLine, f3dLine> m_arrLine3d;
	CArray<f3dPoint, f3dPoint> m_arrPoint3d;
	CArray<f3dRect, f3dRect> m_arrRect3d;
	CArray<f3dCircle, f3dCircle> m_arrCircle3d;
	CArray<CMark, CMark>m_arrMark;			//系统标志
};

#endif // !defined(AFX_ENTITYDATABASE_H__D8EF80D6_6C10_4C71_A2E2_EDDE2BAB943E__INCLUDED_)
