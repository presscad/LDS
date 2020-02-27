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
	char szLayer[4];	//layer[0]==0��ʾ��ָ��
	char ciPanelNormalStyle;//0.��ʾ��ǰ������ͼ����Ϊ��׼�淨����������ࣩ;1.����layer[2]�ж�;2.ָ��vPanelNormal
	bool blFlipHorizon;		//֫������Ҫˮƽ(����)��ת���Ǹֲ��÷�ʽ��1<-->4,2<-->3���� wjh-2019.4.24
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
	ATOM_LIST<TAG_PARANODE_INFO>listTagNodeInfo;		//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
	ATOM_LIST<TAG_PARAROD_INFO>listTagRodInfo;		//��ʱ�ҽӵĸ��ӵ�������{PART_ITEM}�ϵ����ݶ�������
	CTower *m_pTower;
	CLDSModule *m_pActiveModule;
	CFacePanel *m_pFacePanel;
	CHeadTemplate *m_pHeadTemplate;
	CParaTemplate *m_pParaTemplate;
	int m_iTemplateGroup;	//0:��׼��ͷģ���1:�����׼��2:�����׼��3:��ڼ������׼��
	int m_iKeyNodeMir;		//�ؼ���ĶԳ����뷽ʽ
	int m_iTemplateMir;		//��׼�λ��׼��ĶԳ����ɷ�ʽ
	GEPOINT m_vDefaultAssembleOffset;	//Ĭ��װ��λ��ƫ����
	CLDSNode* m_arrShankKeyNode[4];	//ԭʼ¼��������׼����ĸ�����ڵ�
	ARRAY_LIST<CLDSNode*>m_arrTemplateLdsNode;	//ԭʼ���ɵı�׼�λ��׼���ڵĽڵ㼯��
	ARRAY_LIST<CLDSNode*>m_arrMirSumLdsNode;	//��ԭʼ���ɼ��Գ���������(������������)�ı�׼�λ��׼���ڵĽڵ㼯��
	ARRAY_LIST<CLDSLinePart*>m_arrTemplateLdsLinePart;//ԭʼ���ɵı�׼�λ��׼���ڵĸ˼�����
	ARRAY_LIST<CLDSLinePart*>m_arrMirSumLdsLinePart;	//��ԭʼ���ɼ��Գ���������(������������)�ı�׼�λ��׼���ڵĸ˼�����
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
	//��CParaNode�Ѿ��йҽӶ���ʱִ��TagInfo(...), ���򴴽��¹ҽ����ݶ���󷵻�
	TAG_PARANODE_INFO* LoadTagInfo(CParaNode* pItem);
	TAG_PARAROD_INFO*  LoadTagInfo(CParaRod*  pItem);
	GECS acs;
	char m_cTransCS;	//0.����Ҫת������ϵ;'L',��Ҫ��������������ϵ����ת����'R',��Ҫ��������������ϵ����ת��
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
