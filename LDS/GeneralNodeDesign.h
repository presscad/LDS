#pragma once 

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "PropListItem.h"

struct GENERAL_NODEDESIGN_RULE
{
	WORD wiSerial;			//���
	CXhChar16 name;
	bool blHasPlate;		//�Ƿ���Ҫʵ�������
	BYTE cnBoltDirectConn;	//�����(ֱ��)������˨����
	BYTE cnBaseAngleBoltRows;	//��׼����������õ���˨����
	bool blPriorMiddleEndLj;	//�������Ӧ�ñ��滻��
	//GENERAL_NODEDESIGN_RULE();
	void InitDefaultParams();
};
struct INTELLI_MODEL_GUIDE{
	long m_nMaxNodeCountOfManualDesign;	//����һ�����ֹ�������Ƶ����ڵ���
	PRESET_ARRAY8 <GENERAL_NODEDESIGN_RULE> xarrL1Ray;	//���Ǹ�����
	PRESET_ARRAY16<GENERAL_NODEDESIGN_RULE> xarrL2Ray;	//˫�Ǹ�����
	PRESET_ARRAY8 <GENERAL_NODEDESIGN_RULE> xarrL3Ray;	//��Ǹ�����
	//ͨ��������ƹ���
	char ciPlateProfileStyle;		//0.��׼����;1.��������
	char ciProcessLegacyConnections;//0.ɾ��;1.����;2.����
	WORD wiInitBranchNodeSpace;		//�����ӽڵ��ֳ�ʼ���
	WORD wiBranchNodeSpaceIncrementStep;//�����ӽڵ��ּ����������
	//��ͷ�ᵣ�������
	bool blIncArmHeadConnPlates;	//��ͷ�ᵣ���Ӱ����
	bool blAddWireAngle;			//��Ҫʱ����������߽Ǹ�
	bool blAddWirePoint;			//��ӹҵ��������
	//����������������
	bool blIncSlopeTransitPlates;	//���°�,��ͷƿ�ڰ�
	bool blIncArmConnPlates;		//����ᵣ���Ӱ�
	bool blSectionCornerPlates;		//����ˮƽ�������ӽǰ�
	//bool blIncJoints
	struct MULTILEG{
		bool blVPlateInside;	//true.V��������ڲ�;false.V����������
		BYTE ciProfileSchema;	//0.��׼����;1.��������;2.���λ���������
		BYTE ciTouchLineType;	//0.������;1.ƫ��׼��
		short siBendLineSpace;	//������϶,mm
		short siBendLineG;		//���߲Ľ���ָ��ƫ��׼��,��cTouchLineType=1ʱ��Ч;
		short siSpecThick;		//0.��ʾ�Զ�������;>0.��ʾͳһָ�����
		short snMinBaseAngleBolts;
	} multileg;
public:
	INTELLI_MODEL_GUIDE();
	void InitNodeDesignRule();
	void CopyFrom(INTELLI_MODEL_GUIDE* pModelGuide);
	void CopyTo(INTELLI_MODEL_GUIDE* pModelGuide);
public:	//���Թ���
	DECLARE_PROP_FUNC(INTELLI_MODEL_GUIDE);
	int GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen = 100);	//��������ID�õ�����ֵ
};
extern INTELLI_MODEL_GUIDE gxModelGuide;