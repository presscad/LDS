#pragma once 

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "PropListItem.h"

struct GENERAL_NODEDESIGN_RULE
{
	WORD wiSerial;			//序号
	CXhChar16 name;
	bool blHasPlate;		//是否需要实体板连接
	BYTE cnBoltDirectConn;	//允许端(直接)连接螺栓数量
	BYTE cnBaseAngleBoltRows;	//基准材最多允许布置的螺栓排数
	bool blPriorMiddleEndLj;	//这个属性应该被替换掉
	//GENERAL_NODEDESIGN_RULE();
	void InitDefaultParams();
};
struct INTELLI_MODEL_GUIDE{
	long m_nMaxNodeCountOfManualDesign;	//允许一次性手工交互设计的最多节点数
	PRESET_ARRAY8 <GENERAL_NODEDESIGN_RULE> xarrL1Ray;	//单角钢射线
	PRESET_ARRAY16<GENERAL_NODEDESIGN_RULE> xarrL2Ray;	//双角钢射线
	PRESET_ARRAY8 <GENERAL_NODEDESIGN_RULE> xarrL3Ray;	//多角钢射线
	//通用连接设计规则
	char ciPlateProfileStyle;		//0.标准外形;1.定制外形
	char ciProcessLegacyConnections;//0.删除;1.跳过;2.更新
	WORD wiInitBranchNodeSpace;		//端连接节点拆分初始间距
	WORD wiBranchNodeSpaceIncrementStep;//端连接节点拆分间距增长步长
	//塔头横担连接设计
	bool blIncArmHeadConnPlates;	//塔头横担连接板设计
	bool blAddWireAngle;			//需要时添加外贴挂线角钢
	bool blAddWirePoint;			//添加挂点连接设计
	//多呼高塔身连接设计
	bool blIncSlopeTransitPlates;	//变坡板,塔头瓶口板
	bool blIncArmConnPlates;		//塔身横担连接板
	bool blSectionCornerPlates;		//塔身水平隔面连接角板
	//bool blIncJoints
	struct MULTILEG{
		bool blVPlateInside;	//true.V面板贴于内侧;false.V面板贴于外侧
		BYTE ciProfileSchema;	//0.标准外形;1.定制外形;2.矩形化至火曲线
		BYTE ciTouchLineType;	//0.火曲线;1.偏移准线
		short siBendLineSpace;	//火曲间隙,mm
		short siBendLineG;		//射线材交于指定偏移准距,当cTouchLineType=1时有效;
		short siSpecThick;		//0.表示自动计算板厚;>0.表示统一指定板厚
		short snMinBaseAngleBolts;
	} multileg;
public:
	INTELLI_MODEL_GUIDE();
	void InitNodeDesignRule();
	void CopyFrom(INTELLI_MODEL_GUIDE* pModelGuide);
	void CopyTo(INTELLI_MODEL_GUIDE* pModelGuide);
public:	//属性管理
	DECLARE_PROP_FUNC(INTELLI_MODEL_GUIDE);
	int GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen = 100);	//根据属性ID得到属性值
};
extern INTELLI_MODEL_GUIDE gxModelGuide;