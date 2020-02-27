#pragma once
#include "lds_part.h"
//////////////////////////////////////////////////////////////////////////
//
struct IKeypointLifeObject{
protected:
	char m_cOldDisplayMode;
public:
	virtual bool AttachDisplayState(CLDSPart* pPart,BYTE cbMode)=0;
	virtual void DetachDisplayState()=0;
	virtual void RestoreDisplayMode()=0;
};
class CAngleDisplayLifeObject : public IKeypointLifeObject{
	CLDSLineAngle* m_pAngle;
public:
	CAngleDisplayLifeObject();
	~CAngleDisplayLifeObject();
public:
	virtual bool AttachDisplayState(CLDSPart* pPart,BYTE cbMode);
	virtual void DetachDisplayState();
	virtual void RestoreDisplayMode();
};
class CPlateDisplayLifeObject : public IKeypointLifeObject{
	CLDSGeneralPlate* m_pPlate;
public:
	CPlateDisplayLifeObject(CLDSGeneralPlate* pPlate=NULL,char cCurrDisplayMode=-1);
	~CPlateDisplayLifeObject();
public:
	virtual bool AttachDisplayState(CLDSPart* pPart,BYTE cbMode);
	virtual void DetachDisplayState();
	void Redraw(char cDisplayMode,bool removeOldEntities=true,BYTE cbMode=0);
	virtual void RestoreDisplayMode();
};
//////////////////////////////////////////////////////////////////////////
//控制节点突出显示
class CDisplayNodeAtFrontLife
{
	bool m_bOriginalEnabled;					//当前单线模型的消隐状态
public:
	CDisplayNodeAtFrontLife();
	~CDisplayNodeAtFrontLife();
	BOOL DisplayNodeAtFront();
	BOOL HideNodeAtFront();
};
//////////////////////////////////////////////////////////////////////////
//控制工作面示意图显示
#include "btc.h"
class CWorkPlaneSketchLifeObject
{
	int nWorkPlaneSketch;
public:
	CWorkPlaneSketchLifeObject();
	~CWorkPlaneSketchLifeObject();
	//
	void Clear();
	void WorkPlaneSketch(GEPOINT center,GEPOINT norm,const char* sName,UINT uiArrowScrLen=200);
	void WorkPlaneSketch(btc::SKETCH_PLANE &sketch,GEPOINT norm,const char* sName,UINT uiArrowScrLen=200);
};
//////////////////////////////////////////////////////////////////////////
//统计后台构件自动更新后，相关构件的更新显示
class CModifiedPartsOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//同时只允许一个CModifiedPartsOperObject对象激活，不允许嵌套　wjh-2017.3.26
	bool m_bObjectOvered;	//是否已手动善后处理结束对象
	CLDSLinePart* m_pOperRod;
	BYTE m_cbOldAngleMovePartsUpdateFlag,m_cbAngleMovePartsUpdateFlag;
public:
	CModifiedPartsOperObject(bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CModifiedPartsOperObject();
	bool Enable(bool bEnableAgain=true);	//重新启动生命期对象，或临时终止对象生效
	bool Terminate(bool bRedrawRelaPartsSolid=true);	//手动提前结束CModifiedPartsOperObject对象
};
//////////////////////////////////////////////////////////////////////////
//控制角钢移动后，相关构件的显示
class CAngleMoveOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//同时只允许一个CAngleMoveOperObject对象激活，不允许嵌套　wjh-2017.3.26
	bool m_bObjectOvered;	//是否已手动善后处理结束对象
	CLDSLinePart* m_pOperRod;
	BYTE m_cbOldAngleMovePartsUpdateFlag,m_cbAngleMovePartsUpdateFlag;
public:
	CAngleMoveOperObject(CLDSLinePart* pAngleOrTube=NULL,bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CAngleMoveOperObject();
	bool Enable(bool bEnableAgain=true);	//重新启动生命期对象，或临时终止对象生效
	bool Terminate(bool bRedrawRelaPartsSolid=true,CLDSLinePart* pAngleOrTube=NULL);	//手动提前结束CAngleMoveOperObject对象
};
//控制节点移动后，相关构件的更新显示
class CNodeMoveOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//同时只允许一个CNodeMoveOperObject对象激活，不允许嵌套　wjh-2017.3.26
	bool m_bObjectOvered;	//是否已手动善后处理结束对象
	BYTE m_cbOldNodeMovePartsUpdateFlag,m_cbNodeMovePartsUpdateFlag;
public:
	CNodeMoveOperObject(bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CNodeMoveOperObject();
	bool Enable(bool bEnableAgain=true);	//重新启动生命期对象，或临时终止对象生效
	bool Terminate(bool bRedrawRelaPartsSolid=true);
};
//控制螺栓增加、删减或移动后，相关构件的更新显示
class CBoltMoveOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//同时只允许一个CBoltMoveOperObject对象激活，不允许嵌套　wjh-2017.3.26
	bool m_bObjectOvered;	//是否已手动善后处理结束对象
	CLDSBolt* m_pOperBolt;
	BYTE m_cbOldBoltMovePartsUpdateFlag,m_cbBoltMovePartsUpdateFlag;
public:
	CBoltMoveOperObject(CLDSBolt* pBolt,bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CBoltMoveOperObject();
	bool Enable(bool bEnableAgain=true);	//重新启动生命期对象，或临时终止对象生效
	bool Terminate(bool bRedrawRelaPartsSolid=true);
};