#pragma once

//////////////////////////////////////////////////////////////////////////
#ifdef __ANGLE_PART_INC_

struct SHOEPLATE{
	long m_hBasePlate;
	long m_hBaseJg;
	long m_hAssitPlate;
	long m_hXieJg;
	BYTE m_cProfileSide;			//������� 0.�ᴩ�װ�;1.֫����;2.���ߵ���
	BYTE m_cLinkType;				//�������Ǹֵ����ӷ�ʽ 0.��˨����;1.�Ժ�����
	BYTE m_cAngleWorkWing;			//ѥ���׼�Ǹֵ�����֫ 0.X֫;1.Y֫
	short m_nNormOffset;			//����ƫ��
	short m_nThick;					//���
	short A;						//̨�ڸ�
	short B;						//��������
	double m_fMargin;				//ѥ���������װ��Եʱ�ļ�϶ֵ
	double m_fAlongVecLen;			//ѥ���Ӹ˼����ȷ���������
	double m_fCutLen;				//�н�ֵ
	SHOEPLATE(){memset(this,0,sizeof(SHOEPLATE));Init();}
	void Init();
};
struct WORKWINGLS
{
	CLDSBolt* pls;
	double fDist;
	WORKWINGLS(){pls=NULL;fDist=0;}
};
#endif
