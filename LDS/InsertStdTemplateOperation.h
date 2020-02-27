// InsertStdTemplateOperation.h: interface for the CInsertStdTemplateOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSERTSTDTEMPLATEOPERATION_H__A2B89157_3711_4AC8_8D4F_05754BA1D26D__INCLUDED_)
#define AFX_INSERTSTDTEMPLATEOPERATION_H__A2B89157_3711_4AC8_8D4F_05754BA1D26D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "StdFacePanel.h"
#include "ArrayList.h"
struct STDPANEL_INFO{
	char szLayer[4];	//layer[0]==0表示不指定
	char ciPanelNormalStyle;//0.表示当前工作视图法线为标准面法线正方向（外侧）;1.根据layer[2]判断;2.指定vPanelNormal
	bool blFlipHorizon;		//肢朝向需要水平(左右)翻转，角钢布置方式需1<-->4,2<-->3调换 wjh-2019.4.24
	GEPOINT vPanelNormal;
	STDPANEL_INFO(char* pszSpecLayer=NULL,const double* normal=NULL,char ciNormalStyle=0,bool _blFlipHoriztal=false)
	{
		blFlipHorizon=_blFlipHoriztal;//false;
		if(pszSpecLayer==NULL)
			szLayer[0]=szLayer[1]=szLayer[2]=szLayer[3]=0;
		else
			strncpy(szLayer,pszSpecLayer,4);
		ciPanelNormalStyle=ciNormalStyle;
		if(normal)
		{
			vPanelNormal.Set(normal[0],normal[1],normal[2]);
			ciPanelNormalStyle=2;
		}
	}
};
class CInsertStdTemplateOperation  
{
	ATOM_LIST<TAG_PARANODE_INFO>listTagNodeInfo;		//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
	ATOM_LIST<TAG_PARAROD_INFO>listTagRodInfo;		//临时挂接的附加到构件项{PART_ITEM}上的数据对象链表
	CTower *m_pTower;
	CLDSModule *m_pActiveModule;
	CFacePanel *m_pFacePanel;
	CHeadTemplate *m_pHeadTemplate;
	CParaTemplate *m_pParaTemplate;
	int m_iTemplateGroup;	//0:标准塔头模板段1:塔身标准面2:横隔标准面3:多节间特殊标准面
	int m_iKeyNodeMir;		//关键点的对称输入方式
	int m_iTemplateMir;		//标准段或标准面的对称生成方式
	GEPOINT m_vDefaultAssembleOffset;	//默认装配位置偏移量
	CLDSNode* m_arrShankKeyNode[4];	//原始录入的塔身标准面的四个塔身节点
	ARRAY_LIST<CLDSNode*>m_arrTemplateLdsNode;	//原始生成的标准段或标准面内的节点集合
	ARRAY_LIST<CLDSNode*>m_arrMirSumLdsNode;	//含原始生成及对称生成在内(不含镜像生成)的标准段或标准面内的节点集合
	ARRAY_LIST<CLDSLinePart*>m_arrTemplateLdsLinePart;//原始生成的标准段或标准面内的杆件集合
	ARRAY_LIST<CLDSLinePart*>m_arrMirSumLdsLinePart;	//含原始生成及对称生成在内(不含镜像生成)的标准段或标准面内的杆件集合
	struct PLANE_MAPPING{
		KEY4C labelPlane;
		CLDSPlane* pMappingPlane;
		void SetKey(DWORD dwKey){labelPlane=dwKey;}
	};
	CHashListEx<PLANE_MAPPING> hashPlanes;
	CLDSNode *FindLDSNodeByPos(const f3dPoint& pos,CFGWORD* pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	CLDSNode* ValidateLiveParaModelNode(CParaNode* pParaNode);
	CLDSNode *InsertNewLdsNode(const f3dPoint& pos,char firstLayerChar=0,CParaNode* pParaNode=NULL);
	CLDSLinePart* InsertNewPole(CParaRod *pParaRod);
	//当CParaNode已经有挂接对象时执行TagInfo(...), 否则创建新挂接数据对象后返回
	TAG_PARANODE_INFO* LoadTagInfo(CParaNode* pItem);
	TAG_PARAROD_INFO*  LoadTagInfo(CParaRod*  pItem);
	GECS acs;
	char m_cTransCS;	//0.不需要转换坐标系;'L',需要按左手螺旋坐标系进行转换；'R',需要按右手螺旋坐标系进行转换
	void SolveNodePosExpression(CParaNode *pParaNode,CExpression &expression,BOOL bIgnorePosExpression=FALSE);
	void SolveNodePosExpression(CFacePanelNode *pFaceNode,CExpression &expression,BOOL bIgnorePosExpression=FALSE);
	bool AssignAnglePos(CMultiOffsetPos& desBerPos,CParaAngle::POS desTemplPos);
	bool MirTemplate(char ciQuad=0);
public:
	void FillMirInfo();
	CInsertStdTemplateOperation(CTower *pTower=NULL);
	bool CreateStdTemplate(CHeadTemplate* pHeadTemplate,CLDSNode* keyNodeArr[4],int keyNodeMir,int templateMir);
	bool CreateStdTemplate(CParaTemplate* pParaTemplate,CLDSNode* keyNodeArr[4],int keyNodeMir,int templateMir,const double* axis_x_coords=NULL);
	bool CreateStdTemplate(CFacePanel* pFacePanel,CLDSNode* keyNodeArr[4],int keyNodeMir,int templateMir,STDPANEL_INFO* pPanelInfo=NULL,CFGWORD* pBodyWord=NULL,CFGWORD* pLegWord=NULL);
	virtual ~CInsertStdTemplateOperation();

};
#endif // !defined(AFX_INSERTSTDTEMPLATEOPERATION_H__A2B89157_3711_4AC8_8D4F_05754BA1D26D__INCLUDED_)
