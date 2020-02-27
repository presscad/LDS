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
	double cover_plate_width;		//盖板宽
	double cover_plate_thick;		//盖板厚
	double side_plate_thick;		//侧板厚
	double root_width;				//根部宽
	double root_high;				//根部高
	double end_high;				//梢部高
	double root_insert_lenght;		//根部插接长度
	double rail_lever_length;		//横担水平长度
	double rib_plate_thick;			//根部水平肋板厚
	double rib_plate_length;		//根部水平肋板长
};
class CLDSSlotArm : public CLDSPart
{
private:
	//f3dPoint connect_bolt_pos;		//插接螺栓孔位

	f3dPoint orient_x;                  //定位x坐标
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
	//属性
	static int m_iCurDisplayPropGroup;				//记录属性栏当前显示组的ID
	static CHashStrTable<long>propHashtable;	//属性字符串哈希表
	static CHashTable<DWORD>propStatusHashtable;	//属性状态哈希表
	int GetPropValueStr(long id,char* valueStr);	//得到属性值
	static void InitSlotArmPropHashtable();
	static int GetPropStr(long id,char* propStr);		//得到属性字符串
	static long GetPropID(char* propStr);	//得到属性ID
	static BOOL IsPropEqual(HASHOBJSET &selectObjs, long idProp);	//检测属性值是否相等
	static int GetPropStatus(long id);					//得到属性状态
	static BOOL SetPropStatus(long id,BOOL bHide);		//设置属性状态

	~CLDSSlotArm(void);
};
void NewSlotArm();
void EditSlotArm();
#endif