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
		UCS_STRU m_ucs3dPortInUser;		//������ת���û�����ϵ
		UCS_2dSTRU m_ucs2dPortInUser;	//����ƽ�Ƶ��м�����ϵ(����Ļ����ֻ��Y�ᷴ�������)
		double m_fScalePortToUser;		//��Ļ���û����������(����ͼ�����Ų���)
		f2dRect view_rect;				//��������(��ü�����)
	}OPER_ENV;
	OPER_ENV old_oper_env,cur_oper_env;
public:
	long ID;			//��ʶID  ��������ʱ���ã�
	long EXT_ID;		//��չ��ʶID  ��������ʱ���ã�
	BOOL m_bActive;		//����״̬��������ʱ���ã�
	void Empty();
	BOOL GetScopeSize(SCOPE_STRU *scope);
	CEntityDataBase();
	~CEntityDataBase();
	CArray<f3dLine, f3dLine> m_arrLine3d;
	CArray<f3dPoint, f3dPoint> m_arrPoint3d;
	CArray<f3dRect, f3dRect> m_arrRect3d;
	CArray<f3dCircle, f3dCircle> m_arrCircle3d;
	CArray<CMark, CMark>m_arrMark;			//ϵͳ��־
};

#endif // !defined(AFX_ENTITYDATABASE_H__D8EF80D6_6C10_4C71_A2E2_EDDE2BAB943E__INCLUDED_)
