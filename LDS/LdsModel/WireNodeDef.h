#pragma once
#include "WirePlaceCode.h"
#include "XhCharString.h"
#include "ArrayList.h"
#include "f_ent.h"
#include "Hashtable.h"
#include "Tower.h"	//"lds_part.h"

//挂点分组、对接点、挂点
//通用挂点模型(Universal wire point)
//class CLdsWirePoint : public CLDSDbObject
class CUniWirePoint{
protected:
	UINT m_id;
	GECS acs;	//金具插梢的装配位置，Z轴表示正常线缆引出方向
	char m_szStyleName[51];	//串型挂接类型名称（用于筛选可挂接串类型）,如DS(导线双串）、DSX(导线双悬垂串）、DN（导线耐张串）
	virtual GEPOINT _InternalGetWirePointPosition(){return acs.origin;}
public:
	double m_dfHalfConeDegAngle;	//活动范围半锥角
	static const UINT INSULATOR_DS	= 0x00000101;	//导线双串
	static const UINT INSULATOR_DSX	= 0x00010101;	//导线双悬垂串
	static const UINT INSULATOR_DN	= 0x00020001;	//导线耐张
	UINT uiInsulatorStyle;	//串型挂接类型
	CXhChar50 szGimName;	//gim移交时的名称
	WIREPLACE_CODE wireplace;
	void SetKey(DWORD key){m_id=key;}
	CUniWirePoint();
	CXhString get_szInsulatorStyleName(){ return CXhString(m_szStyleName,51); }
	CXhString set_szInsulatorStyleName(const char* szName){
		StrCopy(m_szStyleName,szName,51);
		return CXhString(m_szStyleName,51);
	}
	GEPOINT get_xPosition(){return _InternalGetWirePointPosition();}
	GEPOINT set_xPosition(const double* position){return acs.origin=position;}
	__declspec(property(put=set_xPosition,get=get_xPosition)) GEPOINT xPosition;
	__declspec(property(put=set_szInsulatorStyleName,get=get_szInsulatorStyleName)) CXhString szInsulatorStyleName;
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
};
class CWireConnPoint{	//导地线的对接点，即装配基点
protected:
	UINT m_id;
public:
	CXhChar50 name;		//对接点描述
	void SetKey(DWORD key){m_id=key;}
	PRESET_ARRAY8<long> xarrWirePoints;	//每个对接点挂点分组中最多支持8个挂点（挂点标识）
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	long AppendWirePoint(long hWirePoint);
};
//挂点分组（每相线最终挂接一个挂点分组，每个挂点分组可包括多个挂点）
class CWirePointGroup{
protected:
	UINT m_id;
	char m_szGroupName[17];	//串型挂接类型名称（用于筛选可挂接串类型）,如DS(导线双串）、DSX(导线双悬垂串）、DN（导线耐张串）
public:
	WIREPLACE_CODE wireplace;	//不存储挂接类型
	PRESET_ARRAY4<long> xarrConnPoints;	//同一挂点分组中最多支持4个对接点
	CXhChar50 name;			//挂线描述
	UINT uiHangingStyle;	//串型挂接类型
	CXhString get_szHangingStyleName(){ return CXhString(m_szGroupName,17); }
	CXhString set_szHangingStyleName(const char* szName){
		StrCopy(m_szGroupName,szName,17);
		return CXhString(m_szGroupName,17);
	}
	void SetKey(DWORD key){m_id=key;}
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	long AppendWireConnPoint(long hConnPoint);
	CWirePointGroup();
	~CWirePointGroup();
};
class CSolidBody;
class CXhWirePoint : public CUniWirePoint//,CLDSDbObject
{
protected:
	char _szLayer[4];
public:
	long m_hLdsObj;
	long m_hRelaWireLoadNode;	//与当前导线挂点相关联的LDS模型节点句柄
public:
	bool blWireAngle;	//挂孔是否依附于挂线角钢上，一般不是依附在角钢上就是依附在板上
	float sfHoleD;		//挂孔大小
	GEPOINT vStdBasicAxisVec;	//hole[0]->hole[1]的金具转轴方向
	GEPOINT xBasicPoint;		//hole[0]的内贴面中心点（钢板中性层中心点）
	double dfSpaceWidth;		//挂孔间距（金具宽度+间隙）
	struct HOLE{
		long hBolt,hResidePart;
		HOLE(){hBolt=hResidePart=0;}
	}holes[2];	//实体挂孔，一般由1~2个孔组成
	CSolidBody* pSolidBody;
	char* get_szLayer();
	char* set_szLayer(const char* layer);
	__declspec(property(put=set_szLayer,get=get_szLayer)) char* szLayer;
public:
	CXhWirePoint();
	~CXhWirePoint();
	bool CreateSolidBody();
	virtual void FromBuffer(CBuffer &buffer,long version=NULL,long doc_type=0){;}
	virtual void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0){;}
};
class CPhaseWire
{	//每相导线对应的一组实际的挂接点
protected:
	UINT m_id;	//线缆序号
public:
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	void SetKey(DWORD key){m_id=key;}
public:
	WIREPLACE_CODE wireplace;	//国网GIM挂点属性定义
	long m_idConnPoint;//m_hWirePointGroup;
	//--PhaseWire属性
	BYTE ciCircuitNumber;
	BYTE ciPhaseSerial;	//相序号
	//--InsulatorBranch类型属性
	BYTE m_ciActiveGroupSerial;	//处于激活状态的挂点组号
	WORD wiBranchSerial;
	CXhChar16 m_sInsulatorType;	//="单串"
	CXhChar50 m_sHangWireDesc;	//挂线描述
	///
	CHashList<CWirePointGroup> hashWireGroups;
	CPhaseWire();
	~CPhaseWire();
	void AppendWireConnPoint(UINT uiWireConnPoint,UINT uiWirePointGroup=0);
};
class CUniWireModel{
public:
	CHashListEx<CXhWirePoint> hashWirePoints;	//CUniWirePoint
	CHashListEx<CWireConnPoint> hashConnPoints;
	CHashListEx<CPhaseWire>	hashPhaseWires;
	//根据挂接线缆信息生成杆塔上的唯一线缆序号(based serial 1)
	UINT ToWireSerial(WIREPLACE_CODE wireplace,WORD wiVoltage=0);
	struct CIRCUIT_LAYOUT{
		bool blCircuitDC;	//是否为直流
		BYTE cnCircuitCount;//回路数
		WORD wiVolt;//电压等级(kV)
	public:
		CIRCUIT_LAYOUT(BYTE _cnCircuitCount=2,bool _blDC=false,WORD _wiVolt=500){
			blCircuitDC=_blDC;
			cnCircuitCount=_cnCircuitCount;
			wiVolt=_wiVolt;
		}
		BYTE get_cnWireCount(){
			if(wiVolt==0)
				return cnCircuitCount;
			else if(blCircuitDC)
				return cnCircuitCount*2;
			else
				return cnCircuitCount*3;
		}
		__declspec(property(get=get_cnWireCount)) BYTE cnWireCount;
	};
	BYTE m_cnEarthWireCount;//地线根数
	PRESET_ARRAY4<CIRCUIT_LAYOUT> layouts;
public:
	CUniWireModel();
	bool UpdateWirePointGimName(CUniWirePoint* pWirePoint);
	void InitWireLayout(BYTE cnCircuitCount,bool blCircuitDC=false,WORD wiVoltage=500,BYTE cnEarthWireCount=2);
	bool AppendWireLayout(BYTE cnCircuitCount,bool blCircuitDC=false,WORD wiVoltage=500);
};
