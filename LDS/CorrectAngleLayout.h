#pragma once
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "StdFacePanel.h"

class CCorrectAngleLayout;
class NODE_RODS{
private:
	BOOL m_bCorrected;			//已修正
	//cBerStyle 0.楞线远离基准角钢 1.楞线靠近基准角钢 2.保证不喝水
	//			横隔面角钢按远离靠近基准角钢计算，仅前后左右面角钢支持cBerStyle==2
	BOOL CorrectAngleLayoutPos(CCorrectAngleLayout *pCorrectPos,CLDSLineAngle *pAngle,BYTE cLayout,BYTE cBerStyle,
							   f3dPoint work_norm,CLDSLineAngle *pDatumAngle1,CLDSLineAngle *pDatumAngle2=NULL);
	BOOL Correct2RayRodsNode(CCorrectAngleLayout *pCorrectPos,BOOL bDiaphragm);
	BOOL CorrectDiaphragm(CCorrectAngleLayout *pCorrectPos);
	BOOL CorrecBodyAuxRod(CCorrectAngleLayout *pCorrectPos);
public:
	CLDSNode *m_pNode;
	JGSET angleSetArr[5];		//前、后、左、右、隔面

	NODE_RODS(){m_pNode=NULL;m_bCorrected=FALSE;}
	NODE_RODS(CLDSNode *pNode){m_pNode=pNode;m_bCorrected=FALSE;}
	void Add(CLDSLineAngle *pAngle,bool bDiaphragm=false);
	BOOL IsCorrected(CCorrectAngleLayout *pCorrectPos);
	BOOL IsCanCorrect(CCorrectAngleLayout *pCorrectPos);
	BOOL RelayoutRayAngles(CCorrectAngleLayout *pCorrectPos,BOOL bDiaphragm);
};
struct LAYOUT_POS
{
	static const BYTE LAYOUT_NO		= 0;
	static const BYTE LAYOUT_INSIDE	= 0x01;
	static const BYTE LAYOUT_OUTSIDE= 0x02;
	static const BYTE LAYOUT_H		= 0xFE;
	static const BYTE LAYOUT_Z		= 0xFF;
	BYTE cLayout;	//0.未修正 1.里铁 2.外铁 0xFE.横材 0xFF.主材(主材横材需要根据视图计算里外铁)
	//
	BOOL bStartPos;
	BOOL bEndPos;
	LAYOUT_POS(){cLayout=0;bStartPos=bEndPos=FALSE;}
	LAYOUT_POS(BYTE layout,BOOL start,BOOL end){cLayout=layout;bStartPos=start;bEndPos=end;}
	BOOL IsFinished(){return cLayout==0xFF||cLayout==0xFE||(cLayout>0&&bStartPos&&bEndPos);}
};
class CCorrectAngleLayout
{
	friend class NODE_RODS;
private:
	ARRAY_LIST<CFacePanelNode> arrFaceNodeTemp;
	CHashStrList<CLDSNode*> matchNodeHashList;
	CHashStrList<CLDSLineAngle*> matchJgHashList;
private:
	LINEPARTSET *m_pRodSet,m_xRodSet;
	CHashList<ATOM_LIST<DWORD>> m_hashIntersRodsById;	//交叉杆件
	CHashList<LAYOUT_POS> m_hashRodCorrectedState;		//杆件修正状态 
	CHashList<NODE_RODS> m_hashNodeRodsById;			//节点杆件集合
	//将杆件按节点分组
	void InitNodeRodsList(LINEPARTSET &rodSet);
	//初始化交叉杆件集合
	void InitIntersRodMap(LINEPARTSET &rodSet,PROGRESS_CONTEXT *pDisplayProcess=NULL);
	//初始化杆件修订状态
	void InitParentRodCorrectState(BOOL bInitIncRodX);	
	//对齐正侧面隔面横材角钢的外皮工作平面
	void AlignFrontSideHsectAnglesPos();
	BOOL ExecuteLayoutByFacePanel(CFacePanel *pFacePanel);
public:
	CCorrectAngleLayout(LINEPARTSET *pRodSet);
	void AddExcludeAngleLayout(long hAngle,BYTE cbExcludeFlag);
	BOOL IsCorrectedLayoutPos(long handle);
	BOOL CorrectByFacePanel(ATOM_LIST<CFacePanel>& templ_list,CLDSNode* pNode,bool bWing2Upward=false);
	void Correct(BOOL bDiaphragm=FALSE,PROGRESS_CONTEXT *pDisplayProcess=NULL);
};

class CCorrectModelAngleLayout
{
	bool CorrectDiagonalAnglePos(CLDSLineAngle *pAngle);	//修正斜材位置
	bool CorrectHorizAnglePos(CLDSLineAngle *pAngle,const double* vcNearWorkNormal=NULL);		//修正横材位置
	GEPOINT GetNearTrunkHorizAngleNormal(CLDSNode* pEndNode,char ciDetectArmDirection='X', double TANGENT_TOLERANCE=0.05);
public:
	CCorrectModelAngleLayout(){;}
	void CorrectDiaphragm();			//修正横隔面角钢位置
	void CorrectBodyDiagonalRodPos();	//修正塔身斜材角钢位置
	void CorrectAuxRodPosition();		//修正辅材角钢摆放位置
	void CorrectVPanelRodPos();			//修正塔腿V面角钢摆放位置
};