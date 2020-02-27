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
	static const DWORD SPEC_ANGLE_G  = 1;	//指定了正侧面心距
	static const DWORD SPEC_BASE_POS = 2;	//指定了基础根开及Z位置
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
	BYTE biBasePlateType;	//0.交于上平面 1.交于下平面 2.交于中间
	WORD wiBasePlateThick;	//基板厚度
	WORD wiBasePlateWidth;	//基板宽度
	WORD wiBasePlateHoleD;	//基板孔径
	static const BYTE LAYOUT_4BOLT	= 0;
	static const BYTE LAYOUT_8BOLT	= 1;	
	static const BYTE LAYOUT_NBOLT	= 2;	
	BYTE biLayoutType;		//地脚螺栓布置类型：0.4螺栓 1.8螺栓
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
	WORD wiBoltD;			//地脚螺栓直径
	WORD wiBoltSapce,S2;	//螺栓半间距S1(wiBoltSpace),S2
	WORD wiFoundationWidth;	//基础宽度
	CHashList<POSTRODFLAG>hashPostRodFlags;	//用于存储塔向主材心距及接腿根开的自动更新设定
public:	//塔脚板靴板/肋板设计参数
	BYTE cnMainAngleBoltNum, cnMaxSingularRowBolts,cnRows;	//主材连接螺栓数及布置排数
	WORD wiShoeThick, wiRibThick, wiRibMargin;				//底板厚度、靴板厚度，肋板厚度、边距
	WORD wRibWidth, wRibOuterHeight, wRibInnerHeight;		//肋板宽度(底板焊接边),高度（分外侧，内侧）
public:
	FOUNDATION_PARA();
	void InitAnchorsLocation();
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	DECLARE_PROP_FUNC(FOUNDATION_PARA)
};
