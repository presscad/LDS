// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Hashtable.h"
#include "ArrayList.h"
#include "PropListItem.h"
//#include "XhLicAgent.h"

struct POSTRODFLAG{// : public BYTEFLAG{
	BYTE m_cFlag;
	static const DWORD SPEC_ANGLE_G  = 1;	//ָ�����������ľ�
	static const DWORD SPEC_BASE_POS = 2;	//ָ���˻���������Zλ��
	POSTRODFLAG(BYTE cbFlag=0){m_cFlag=cbFlag;}
	bool IsSpcecifyAngleG(){return (m_cFlag&0x01)>0;}
	bool IsSpcecifyBasePos(){return (m_cFlag&0x02)>0;}
	bool SetSpcecifyAngleG(bool bState=true){
		if(bState)
			m_cFlag|=0x01;
		else
			m_cFlag&=0xfe;
	}
	bool SetSpcecifyBasePos(bool bState=true){
		if(bState)
			m_cFlag|=0x02;
		else
			m_cFlag&=0xfd;
	}
};
struct FOUNDATION_PARA
{
public:
	static const BYTE TYPE_TOP = 0;
	static const BYTE TYPE_BTM = 1;
	static const BYTE TYPE_MID = 2;
	BYTE biBasePlateType;	//0.������ƽ�� 1.������ƽ�� 2.�����м�
	WORD wiBasePlateThick;	//������
	WORD wiBasePlateWidth;	//������
	WORD wiBasePlateHoleD;	//����׾�
	static const BYTE LAYOUT_4BOLT	= 0;
	static const BYTE LAYOUT_8BOLT	= 1;	
	static const BYTE LAYOUT_NBOLT	= 2;	
	BYTE biLayoutType;		//�ؽ���˨�������ͣ�0.4��˨ 1.8��˨
	struct LOCATION{
		short siPosX,siPosY;
		LOCATION(short posx=0,short posy=0){
			siPosX=posx;
			siPosY=posy;
		}
		void SetLocation(short posx,short posy)
		{
			siPosX=posx;
			siPosY=posy;
		}
	};
	PRESET_ARRAY8<LOCATION> anchors;
	WORD wiBoltD;			//�ؽ���˨ֱ��
	WORD wiBoltSapce,S2;	//��˨����S1(wiBoltSpace),S2
	WORD wiFoundationWidth;	//�������
	CHashList<POSTRODFLAG>hashPostRodFlags;	//���ڴ洢���������ľ༰���ȸ������Զ������趨
public:	//���Ű�ѥ��/�߰���Ʋ���
	BYTE cnMainAngleBoltNum, cnMaxSingularRowBolts,cnRows;	//����������˨������������
	WORD wiShoeThick, wiRibThick, wiRibMargin;				//�װ��ȡ�ѥ���ȣ��߰��ȡ��߾�
	WORD wRibWidth, wRibOuterHeight, wRibInnerHeight;		//�߰���(�װ庸�ӱ�),�߶ȣ�����࣬�ڲࣩ
public:
	FOUNDATION_PARA();
	void InitAnchorsLocation();
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	DECLARE_PROP_FUNC(FOUNDATION_PARA)
};
