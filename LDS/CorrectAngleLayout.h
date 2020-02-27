#pragma once
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "StdFacePanel.h"

class CCorrectAngleLayout;
class NODE_RODS{
private:
	BOOL m_bCorrected;			//������
	//cBerStyle 0.����Զ���׼�Ǹ� 1.���߿�����׼�Ǹ� 2.��֤����ˮ
	//			�����Ǹְ�Զ�뿿����׼�Ǹּ��㣬��ǰ��������Ǹ�֧��cBerStyle==2
	BOOL CorrectAngleLayoutPos(CCorrectAngleLayout *pCorrectPos,CLDSLineAngle *pAngle,BYTE cLayout,BYTE cBerStyle,
							   f3dPoint work_norm,CLDSLineAngle *pDatumAngle1,CLDSLineAngle *pDatumAngle2=NULL);
	BOOL Correct2RayRodsNode(CCorrectAngleLayout *pCorrectPos,BOOL bDiaphragm);
	BOOL CorrectDiaphragm(CCorrectAngleLayout *pCorrectPos);
	BOOL CorrecBodyAuxRod(CCorrectAngleLayout *pCorrectPos);
public:
	CLDSNode *m_pNode;
	JGSET angleSetArr[5];		//ǰ�������ҡ�����

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
	BYTE cLayout;	//0.δ���� 1.���� 2.���� 0xFE.��� 0xFF.����(���ĺ����Ҫ������ͼ����������)
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
	CHashList<ATOM_LIST<DWORD>> m_hashIntersRodsById;	//����˼�
	CHashList<LAYOUT_POS> m_hashRodCorrectedState;		//�˼�����״̬ 
	CHashList<NODE_RODS> m_hashNodeRodsById;			//�ڵ�˼�����
	//���˼����ڵ����
	void InitNodeRodsList(LINEPARTSET &rodSet);
	//��ʼ������˼�����
	void InitIntersRodMap(LINEPARTSET &rodSet,PROGRESS_CONTEXT *pDisplayProcess=NULL);
	//��ʼ���˼��޶�״̬
	void InitParentRodCorrectState(BOOL bInitIncRodX);	
	//��������������ĽǸֵ���Ƥ����ƽ��
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
	bool CorrectDiagonalAnglePos(CLDSLineAngle *pAngle);	//����б��λ��
	bool CorrectHorizAnglePos(CLDSLineAngle *pAngle,const double* vcNearWorkNormal=NULL);		//�������λ��
	GEPOINT GetNearTrunkHorizAngleNormal(CLDSNode* pEndNode,char ciDetectArmDirection='X', double TANGENT_TOLERANCE=0.05);
public:
	CCorrectModelAngleLayout(){;}
	void CorrectDiaphragm();			//���������Ǹ�λ��
	void CorrectBodyDiagonalRodPos();	//��������б�ĽǸ�λ��
	void CorrectAuxRodPosition();		//�������ĽǸְڷ�λ��
	void CorrectVPanelRodPos();			//��������V��Ǹְڷ�λ��
};