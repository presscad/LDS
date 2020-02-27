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
//���ƽڵ�ͻ����ʾ
class CDisplayNodeAtFrontLife
{
	bool m_bOriginalEnabled;					//��ǰ����ģ�͵�����״̬
public:
	CDisplayNodeAtFrontLife();
	~CDisplayNodeAtFrontLife();
	BOOL DisplayNodeAtFront();
	BOOL HideNodeAtFront();
};
//////////////////////////////////////////////////////////////////////////
//���ƹ�����ʾ��ͼ��ʾ
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
//ͳ�ƺ�̨�����Զ����º���ع����ĸ�����ʾ
class CModifiedPartsOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//ͬʱֻ����һ��CModifiedPartsOperObject���󼤻������Ƕ�ס�wjh-2017.3.26
	bool m_bObjectOvered;	//�Ƿ����ֶ��ƺ����������
	CLDSLinePart* m_pOperRod;
	BYTE m_cbOldAngleMovePartsUpdateFlag,m_cbAngleMovePartsUpdateFlag;
public:
	CModifiedPartsOperObject(bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CModifiedPartsOperObject();
	bool Enable(bool bEnableAgain=true);	//�������������ڶ��󣬻���ʱ��ֹ������Ч
	bool Terminate(bool bRedrawRelaPartsSolid=true);	//�ֶ���ǰ����CModifiedPartsOperObject����
};
//////////////////////////////////////////////////////////////////////////
//���ƽǸ��ƶ�����ع�������ʾ
class CAngleMoveOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//ͬʱֻ����һ��CAngleMoveOperObject���󼤻������Ƕ�ס�wjh-2017.3.26
	bool m_bObjectOvered;	//�Ƿ����ֶ��ƺ����������
	CLDSLinePart* m_pOperRod;
	BYTE m_cbOldAngleMovePartsUpdateFlag,m_cbAngleMovePartsUpdateFlag;
public:
	CAngleMoveOperObject(CLDSLinePart* pAngleOrTube=NULL,bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CAngleMoveOperObject();
	bool Enable(bool bEnableAgain=true);	//�������������ڶ��󣬻���ʱ��ֹ������Ч
	bool Terminate(bool bRedrawRelaPartsSolid=true,CLDSLinePart* pAngleOrTube=NULL);	//�ֶ���ǰ����CAngleMoveOperObject����
};
//���ƽڵ��ƶ�����ع����ĸ�����ʾ
class CNodeMoveOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//ͬʱֻ����һ��CNodeMoveOperObject���󼤻������Ƕ�ס�wjh-2017.3.26
	bool m_bObjectOvered;	//�Ƿ����ֶ��ƺ����������
	BYTE m_cbOldNodeMovePartsUpdateFlag,m_cbNodeMovePartsUpdateFlag;
public:
	CNodeMoveOperObject(bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CNodeMoveOperObject();
	bool Enable(bool bEnableAgain=true);	//�������������ڶ��󣬻���ʱ��ֹ������Ч
	bool Terminate(bool bRedrawRelaPartsSolid=true);
};
//������˨���ӡ�ɾ�����ƶ�����ع����ĸ�����ʾ
class CBoltMoveOperObject
{
	static bool OBJECT_INSTANCE_RUNNING;	//ͬʱֻ����һ��CBoltMoveOperObject���󼤻������Ƕ�ס�wjh-2017.3.26
	bool m_bObjectOvered;	//�Ƿ����ֶ��ƺ����������
	CLDSBolt* m_pOperBolt;
	BYTE m_cbOldBoltMovePartsUpdateFlag,m_cbBoltMovePartsUpdateFlag;
public:
	CBoltMoveOperObject(CLDSBolt* pBolt,bool bClearRecordBuff=true,bool bTemporyDisable=false);
	~CBoltMoveOperObject();
	bool Enable(bool bEnableAgain=true);	//�������������ڶ��󣬻���ʱ��ֹ������Ч
	bool Terminate(bool bRedrawRelaPartsSolid=true);
};