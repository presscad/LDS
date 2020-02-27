#pragma once
#include "MsgDef.h"
#include "SubstationModel.h"

#ifdef __PART_DESIGN_INC_
class CFemSolverLoop
{
public:
	static const BYTE CMD_SOLVEFEMMODEL		= 1;
	static const BYTE CMD_WRITERESULTDATA	= 2;
	static const BYTE CMD_QUERYPROCESS		= 3;
	static const BYTE CMD_EXITLDSPROCESS	= 4;
	static bool m_bQuitNow;	//当前是否需要退出该线程
	static CFemModel femmodel;
public:
	static void SolveFemModel(LDS_MSGHEAD& msghead);
	static void WriteFemResultData();
	static void WriteFemSolveProcessPercent();
public:
	HANDLE hPipeRead,hPipeWrite;
	CFemSolverLoop(void);
	~CFemSolverLoop(void);
	void StartLoop();
};

#endif