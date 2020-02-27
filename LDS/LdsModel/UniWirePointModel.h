#pragma once
#include "WirePlaceCode.h"
#include "XhCharString.h"
#include "ArrayList.h"
#include "f_ent.h"
#include "Hashtable.h"
#include "Tower.h"	//"lds_part.h"
#include "PropListItem.h"	

class CSolidBody;
struct SOLID_OBJECT{
	HIBERID hiberid;
	long get_m_hPart(){return (long)hiberid.masterId;}
	long set_m_hPart(long hPartId){
		hiberid.masterId=(DWORD)hPartId;
		return hPartId;
	}
	// 构件的句柄
	__declspec(property(put=set_m_hPart,get=get_m_hPart)) long m_hPart;
	int  m_idClassType;	// 构件的类型
	GECS acs;			// 构件的装配坐标系
	char ciLineType;		//0:构件为实体类型；1:构件为直线型构件；2:构件为弧线型构件
	char blShowBriefLine;	//线型构件是否需要在单线图中显示
	double dfSphereRadius;	//显示球体的半径
	GELINE line;		// 单线状态下的简化线
	//f3dArcLine arc;		// 单线状态下的弧线简化线
	CSolidBody *pBody;		// 显示构件的实体显示数据
};
//通用挂点模型(Universal wire point)
class CUniWirePoint {
protected:
	UINT m_id;
	bool m_bSolidModified;
public:
	GECS acs;						//金具插梢的装配位置，Z轴表示正常线缆引出方向
	double m_fHalfConeDegAngle;		//活动范围半锥角
	CXhChar50 m_szStyleName;	//串型挂接类型名称（用于筛选可挂接串类型）,如DS(导线双串）、DSX(导线双悬垂串）、DN（导线耐张串）
	CXhChar50 m_sGimName;			//gim移交时的名称
	WIREPLACE_CODE m_xWirePlace;
	CSolidBody* pSolidBody;
public:	//属性定义
	//挂点位置
	GEPOINT get_xPosition(){return acs.origin;}
	GEPOINT set_xPosition(const double* position){return acs.origin=position;}
	__declspec(property(put=set_xPosition,get=get_xPosition)) GEPOINT xPosition;
	//挂串名称（按西南院内部规则命名）
	CXhString get_szInsulatorStyleName(){ return CXhString(m_szStyleName,51); }
	CXhString set_szInsulatorStyleName(const char* szName){
		StrCopy(m_szStyleName,szName,51);
		return CXhString(m_szStyleName,51);
	}
	__declspec(property(put=set_szInsulatorStyleName,get=get_szInsulatorStyleName)) CXhString szInsulatorStyleName;
	//受力方向
	GEPOINT get_vWirePull(){return acs.axis_z;}
	GEPOINT set_vWirePull(const double* vPull){return acs.axis_z=vPull;}
	__declspec(property(put=set_vWirePull,get=get_vWirePull)) GEPOINT vWirePull;
	//挂点序号
	UINT get_Id(){return m_id;}
	__declspec(property(get=get_Id)) UINT Id;
	//挂点位置标识符号(前|后)
	char get_ciWirePosSymbol() {
		if (m_xWirePlace.ciWireType == 'J' || m_xWirePlace.ciTensionType != 2)
			return 0;	//跳线及悬垂导地线不区分前后
		if (m_xWirePlace.ciWireDirection == 'Y')	//线缆方向为Y即挂点在X横担上
		{	//根据GIM规范要求:X向横担以Y轴正向为前,负向为后;但是建模坐标系的Y轴与GIM模型坐标系的Y轴是反向的
			if (xPosition.y < 0)
				return 'Q';
			else
				return 'H';
		}
		else //if(m_xWirePlace.ciWireDirection=='X') //线缆方向为X即挂点在Y横担上
		{	//根据GIM规范要求:Y向横担以X轴负向为前,正向为后;且建模坐标系的X轴与GIM模型坐标系的X轴同向
			if (xPosition.x < 0)
				return 'Q';
			else
				return 'H';
		}
	}
	__declspec(property(get = get_ciWirePosSymbol)) char ciWirePosSymbol;
public:
	CUniWirePoint()
	{
		m_id=0;
		m_fHalfConeDegAngle=15;
		pSolidBody=NULL;
		m_bSolidModified=true;
	}
	~CUniWirePoint(){;}
	bool IsSolidModified(){return m_bSolidModified;}
	bool SetModified(bool modified){return m_bSolidModified=modified;}
	//
	void SetKey(DWORD key){m_id=key;}
#if !defined(DISABLE_SOLID_BODY)&&!defined(_HANDOVER_TO_CLIENT_)
	virtual bool Create3dSolidModel(UINT slices=6);
	virtual CSolidPart GetSolidPartObject(double dfArrowLength=2000);
#endif
};
//杆塔模型中挂点，用于连接金具插梢
class CXhWirePoint : public CUniWirePoint
{
protected:
	char _szLayer[4];
public:
	BYTE m_ciLevel;				//挂点定义级别：0.Mod(单线节点)|1.Solid(实体挂孔)
	long m_hRelaPart;			//与当前导线挂点相关联构件实体;螺栓(单孔挂点)|金具(双孔挂点)
	long m_hRelaNode;			//与当前导线挂点相关联的LDS模型节点句柄
	bool m_bRelyAngle;			//挂孔是否依附于挂线角钢上，一般不是依附在角钢上就是依附在板上
	float m_fHoleD;				//挂孔大小
	float m_fSpaceWidth;		//挂孔间距（金具宽度+间隙）
	GEPOINT m_vStdBasicAxisVec;	//hole[0]->hole[1]的金具转轴方向
	GEPOINT m_xBasicPoint;		//hole[0]的内贴面中心点（钢板中性层中心点）
	GEPOINT m_vSketchArrow;		//单线模型下的挂线方向示意箭头
	struct HOLE{
		long hBolt,hResidePart;
		HOLE(){hBolt=hResidePart=0;}
	}holes[2];	//实体挂孔，一般由1~2个孔组成
	CSolidBody* pSolidBody;
public:	//属性定义
	char* get_szLayer();
	char* set_szLayer(const char* layer);
	__declspec(property(put=set_szLayer,get=get_szLayer)) char* szLayer;
public:
	CXhWirePoint();
	~CXhWirePoint();
	void CloneProp(CXhWirePoint* pSrcWriePoint);
	//
	virtual bool Create3dSolidModel(UINT slices=6);
	//属性栏
	DECLARE_PROP_FUNC(CXhWirePoint);
	int GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen=100);	//根据属性ID得到属性值
};
//挂点分组，为挂串的一种连接方案，由1个或多个挂点连接一个挂串
class CWirePointGroup{
public:
	UINT m_uiGroupId;		//分组序号
	UINT uiHangingStyle;	//串型挂接类型
	CXhChar16 m_sGroupName;
	PRESET_ARRAY4<long> m_xWirePointArr;	//同一挂点分组中最多支持4个挂点
public:
	CWirePointGroup()
	{
		m_uiGroupId=0;
		m_xWirePointArr.Set(0,1);
	}
	~CWirePointGroup(){;}
};
//挂串分支即一个金具挂串，它可能有多种连接方案（多个挂点分组）
class CInsulatorBranch
{	
public:
	static const UINT INSULATOR_DS	= 0x00000101;	//导线双串
	static const UINT INSULATOR_DSX	= 0x00010101;	//导线双悬垂串
	static const UINT INSULATOR_DN	= 0x00020001;	//导线耐张
	UINT m_uiInsulatorStyle;		//串型挂接类型
	CXhChar16 m_sInsulatorName;		//串型挂接类型名称,如DS(导线双串）、DSX(导线双悬垂串）、DN（导线耐张串）
	UINT m_idConnPoint;				//归属的装配连接点
	UINT m_uiBranchSerial;			//挂串分支序号
	UINT m_uiActiveGroupId;			//处于激活状态的挂点组号
	ARRAY_LIST<CWirePointGroup> m_xWirePtGroupArr;
public:
	CInsulatorBranch();
	~CInsulatorBranch();
	//
	CWirePointGroup* GetActiveWirePointGroup();
};
//导地线的装配连接点，由1个或多个挂串分支连接
//目前只按一个装配点由一个挂串分支连接处理
class CWireConnPoint{
protected:
	UINT m_uiSerial;
public:
	UINT m_uiCircuitId;		//归属回路号
	UINT m_uiPhaseSerial;	//归属相序号
	char m_ciWiretType;  	//挂线类型：'E'地线|'C'导线|'J'跳线
	char m_ciHangingType;  	//挂线点的挂串方式：0.普通单串|'S'.双串|'V'.V串
	GEPOINT m_xPos;			//挂线装配点位置
	CXhChar50 m_sDes;		//挂线描述:地线(前地，后地)、导线(前导，后导)、跳线、
	ARRAY_LIST<CInsulatorBranch> m_xInsulatorBranchArr;
public:	//属性定义
	//装配连接序号
	UINT get_Id(){return m_uiSerial;}
	__declspec(property(get=get_Id)) UINT Id;
public:
	CWireConnPoint();
	~CWireConnPoint();
	//
	void SetKey(DWORD key){m_uiSerial=key;}
	GEPOINT CalAssemblyPos();
};
//每相导线对应的一组实际的挂接点
class CPhaseWire
{	
public:
	BYTE m_ciPhaseSerial;	//相序号
	PRESET_ARRAY4<long> m_xConnPts;		//每相线最多有两个导线连接点(耐张时有前后连接点)和1个跳线连接点
public:
	CPhaseWire() { ; }
	~CPhaseWire() { ; }
};
//导线回路
class CWireCircuit
{
public:
	UINT m_uiSerilaId;	//回路序号
	BYTE m_ciCurrent;	//输电电流类型：0.DC(直流)|1.AC(交流)
	WORD m_wiVoltage;	//输电电压,单位KV
	BYTE m_ciForceType;	//挂线受力方式：0.悬垂|1.耐张
	CXhChar50 m_sName;	//回路描述
	ARRAY_LIST<CPhaseWire> m_xPhaseWireArr;	//同一回路最多有三相线
public:
	CWireCircuit();
	~CWireCircuit(){;}
	//
	UINT GetHangWireCount();
	UINT GetConnPointCount();
};
class CEarthCircuit
{
public:
	PRESET_ARRAY4<long> m_xConnPts;		//单回路地线最多有两个地线连接点(耐张时有前后连接点)
public:
	CEarthCircuit() { ; }
	~CEarthCircuit() { ; }
};
//通用挂点数据模型
class CUniWireModel{
public:
	bool m_bGimWireModelInherited;	//已经从之前Tower模型中把GIM挂点信息继承过来 wjh-2019.4.18
	BYTE m_cnEarthWireCount;	//地线根数
	CXhChar50 m_sTowerType;		//塔型名称
	CHashListEx<CXhWirePoint> m_hashWirePoints;
	CHashListEx<CWireConnPoint> m_hashConnPoints;
	ATOM_LIST<CWireCircuit> m_listWireCircuits;		//导线回路
	ATOM_LIST<CEarthCircuit> m_listEarthCircuits;	//地线回路
public:
	CUniWireModel();
	~CUniWireModel(){;}
	//
	void Empty();
	bool UpdateWirePointGimName(CUniWirePoint* pWirePoint);
	bool ExportUniWireXmlData(const char* sFile);
	bool ImPortUniWireXmlData(const char* sFile);
	void FromBuffer(CBuffer& buffer);
	void ToBuffer(CBuffer& buffer);
	//
	static CXhChar50 GetWirePtGimName(WIREPLACE_CODE wireplace,GEPOINT pos);
};
