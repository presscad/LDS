// SingleRayEndBoltLj.h: interface for the CSingleRayEndBoltLj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLERAYENDBOLTLJ_H__DF16D4AA_19C4_47EC_9BF3_5058869C6E82__INCLUDED_)
#define AFX_SINGLERAYENDBOLTLJ_H__DF16D4AA_19C4_47EC_9BF3_5058869C6E82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __TSA_
#include "BraceConnectPoint.h"

class CSingleRayEndBoltLj  
{
	class CAnglePositionLifeObj{
		struct BACKUP{
			CLDSLineAngle* pAngle;
			BOOL bDataModified;
			GEPOINT xStartPos,xEndPos;
		};
		CXhSimpleList<BACKUP> listBackupAngles;
	public:
		CAnglePositionLifeObj(CLDSLineAngle* pAngle=NULL);
		~CAnglePositionLifeObj();
		bool Backup(CLDSLineAngle* pAngle);
	};
	BRACE_CONNECT_POINT xBraceConnState;	//判断是否为辅材连接点设计的相关信息 wjh-2019.11.10
	UCS_STRU ucs;
	f3dPoint m_arrRayVec[3];
	f3dPoint base_vec,base_wing_vec,base_wing_norm;
	CLDSLineAngle* m_arrRayAngle[3];
	CLDSLineAngle* m_pBaseAngle;
	CLDSNode* m_pBaseNode;
	JGZJ m_jgzjBase;		//基准角钢的当前肢准距
	JGZJ m_arrJgzjRay[3];	//射线材角钢准距
	int m_iBaseAngleWorkWing;		//0:wing x;1:wing y
	bool m_arrCurrRayNode[3];	//0.始端连接，1.终端连接
	BOOL m_arrInsideAngle[3];
	BOOL m_bRayAngleLenOff[3];	//射线角钢沿基准角钢进行偏移
	int m_arrRayAngleWorkWing[3];	//0:wing x;1:wing y
	CConnectInfo* m_arrConnInfo[3];
	f3dPoint m_arrRayAngleWorkWingVec[3];
	f3dPoint m_arrRayAngleWorkWingNorm[3];
protected:
	CLDSBolt* LaySingleRayEndBolt(int index, CLDSBolt* pShareBolt=NULL);
	BOOL UpdateSingRayEndNode(int index);
public:
	struct BRANCH_NODE_TEST{
		char ciRodCurrEndS0E1[2];
		short siLenOffset[2];
		CLDSLinePart* pRods[2];
		CLDSLinePart* pDatumRods[2];
		CLDSNode* pOffsetNodes[2];
		BRANCH_NODE_TEST(){memset(this,0,sizeof(BRANCH_NODE_TEST));}
	};
public:
	CSingleRayEndBoltLj();
	virtual ~CSingleRayEndBoltLj();
	BOOL DesignConnectPoint(CConnectPointInfo *pConnectPoint,BRANCH_NODE_TEST* pOnlyBranchNodeTest=NULL);
	BOOL DesignIntersConnPoint(CConnectPointInfo *pConnectPoint, BRANCH_NODE_TEST* pOnlyBranchNodeTest = NULL);
};
#endif
#endif // !defined(AFX_SINGLERAYENDBOLTLJ_H__DF16D4AA_19C4_47EC_9BF3_5058869C6E82__INCLUDED_)
