#pragma once

//////////////////////////////////////////////////////////////////////////
#ifdef __ANGLE_PART_INC_

struct BASEPLATE{
	long m_hBaseNode;
	long m_hBaseRod;
	BYTE m_cShape;					//0:��ʾΪ����1:��ʾΪԲ��
	BYTE m_cNormOffsetType;			//0.��ƽ��;1.��ƽ��;2.�м���;3.ָ��ƫ��
	long m_iVectZType;				//Z����0.�Զ��� 1.X+ 2.X- 3.Y+ 4.Y- 5.Z+ 6.Z-
	short m_nNormOffset;			//����ƫ��
	short m_nThick;					//���
	short C;						//�Ǹֶ���㵽�װ����(��ֵ��ָ����ͷ)
	double m_fR;					//Բ�εװ��������Բ�뾶
	BOOL m_bWeldDiZuo;				//���ӵ���
	long XA,XB,YA,YB,XC[4],YC[4];	//A��ʾX(Y)������,B��ʾX(Y)�Ḻ��,C��ʾ���ǿ�
	BASEPLATE(){memset(this,0,sizeof(BASEPLATE));Init();}
	void Init();
};

#endif