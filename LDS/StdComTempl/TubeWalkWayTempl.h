#pragma once

#include "Tower.h"
#include "SortFunc.h"
#include ".\IStdComTempl.h"
#include ".\StdComModel.h"
//////////////////////////////////////////////////////////////////////////
//
class CTubeWalkWayStdComTempl : public CStdComTempl
{
public:
	static const DWORD WAY_INNER_ANGLE_ID	=2;
	static const DWORD WAY_OUTER_ANGLE_ID	=3;
	static const DWORD BRACE_UPPER_HORI_ANGLE_ID=4;//ÉÏºá³Å½Ç¸Ö
	static const DWORD BRACE_GRADIENT_ANGLE_ID	=5;//Ð±³Å½Ç¸Ö
	static const DWORD BRACE_DOWN_HORI_ANGLE_ID	=6;//ÏÂºá³Å½Ç¸Ö
protected:
	void InitTemplParams();
	void InitWalkWayTempl();
	void InitHolderTempl(int iHolerType);
	void InitPlateHolderTempl();
	void InitTriangleHolderTempl();
	void InitPlatformHolderTempl();
	//
public:
	CTubeWalkWayStdComTempl();
	virtual ~CTubeWalkWayStdComTempl();
	//
	virtual void InitComponentTempl(TEMPL_SOURCE* pTemplSource=NULL);
	virtual bool IsMeetConditions();
};
//////////////////////////////////////////////////////////////////////////
//
class CTubeWalkWayStdComModel : public CStdComModel
{	
protected:
	virtual bool BuildAcsOfAssmComponent();
	void UpdateNodeAndRodBaseInfo();
public:
	CTubeWalkWayStdComModel(IModel* pModel=NULL);
	~CTubeWalkWayStdComModel(void);
	//
	bool BuildModel(CStdComTempl* pStdTempl);
};