#pragma once

#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "PartLib.h"
#ifndef _LEGACY_LICENSE
#include "XhLdsLm.h"
#else
#include "Lic.h"
#endif
#include "HashTable.h"
#include "SmartPtr.h"
#include "Buffer.h"
#include "macro.h"
#include "ldsptr_list.h"
#include "Tower.h"

#ifdef __GGG_
struct SLOTARMINFO
{
	double cover_plate_width;		//�ǰ��
	double cover_plate_thick;		//�ǰ��
	double side_plate_thick;		//����
	double root_width;				//������
	double root_high;				//������
	double end_high;				//�Ҳ���
	double root_insert_lenght;		//������ӳ���
	double rail_lever_length;		//�ᵣˮƽ����
	double rib_plate_thick;			//����ˮƽ�߰��
	double rib_plate_length;		//����ˮƽ�߰峤
};
class CLDSSlotArm : public CLDSPart
{
private:
	//f3dPoint connect_bolt_pos;		//�����˨��λ

	f3dPoint orient_x;                  //��λx����
public:
	SLOTARMINFO m_designInfo;
public:
	CLDSSlotArm();
	void Init(SLOTARMINFO designInfo);
	double GetCoverPlateWidth();
	double GetCoverPlateThick();
	double GetSidePlateThick();
	double GetRootWidth();
	double GetRootHigh();
	double GetEndHigh();
	double GetRootInsertLen();
	double GetRailLeverLen();
	double GetRibPlateThick();
	double GetRibPlateLen();
	void Create3dSolidModel();
	CSolidPart GetSolidPartObject();
	//����
	static int m_iCurDisplayPropGroup;				//��¼��������ǰ��ʾ���ID
	static CHashStrTable<long>propHashtable;	//�����ַ�����ϣ��
	static CHashTable<DWORD>propStatusHashtable;	//����״̬��ϣ��
	int GetPropValueStr(long id,char* valueStr);	//�õ�����ֵ
	static void InitSlotArmPropHashtable();
	static int GetPropStr(long id,char* propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬

	~CLDSSlotArm(void);
};
void NewSlotArm();
void EditSlotArm();
#endif