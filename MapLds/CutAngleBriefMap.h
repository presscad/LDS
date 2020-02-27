#pragma once
#include "StdArx.h"
#include "Tower.h"
#include "FormatTable.h"
#include "JgDrawing.h"
#include "SysPara.h"

void DrawJgCutAngleBriefMap(JGSET &cutanglejgset,BOOL bPromptPos);

class CCutAngleBriefMap
{
	struct MAP_PAGE{
		JGSET cutAngleSet;	
		CFormatTbl xCutAngleTbl;
	};
	JGSET m_cutAngleSet;
	BOOL m_bPromptPos;
	BOOL m_bDrawFrame;
	ATOM_LIST<MAP_PAGE> m_pageList;
	int m_iStartRow;		//起始行
	int m_iBriefMapCol;		//简图所在列
	int m_nRowspann;		//一根角钢所占行数
	//
	double m_textHeight;
	//
	bool Init(JGSET &cutanglejgset,BOOL bPromptPos);
	void PrepareCutAngleTbl();
	void PrepareCutAngleTbl1(CFormatTbl &xCutAngleTbl,JGSET &jgSet);
	void PrepareCutAngleTbl2(CFormatTbl &xCutAngleTbl,JGSET &jgSet);
public:
	CCutAngleBriefMap(JGSET &cutAngleSet,BOOL bPropmtPos);
	void Draw();
	//
	static void Draw(JGSET &cutAngleSet,BOOL bPropmtPos);
	static void GetAngleCutAngleInfo(CLDSLineAngle *pJg,CXhChar50 &xWingCut,CXhChar50 &yWingCut,bool bStart,int iCutAngleDimType);
};