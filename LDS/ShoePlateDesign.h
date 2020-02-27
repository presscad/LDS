#pragma once

//////////////////////////////////////////////////////////////////////////
#ifdef __ANGLE_PART_INC_

struct SHOEPLATE{
	long m_hBasePlate;
	long m_hBaseJg;
	long m_hAssitPlate;
	long m_hXieJg;
	BYTE m_cProfileSide;			//外形类别 0.贯穿底板;1.肢翼单侧;2.楞线单侧
	BYTE m_cLinkType;				//与依赖角钢的连接方式 0.螺栓连接;1.对焊连接
	BYTE m_cAngleWorkWing;			//靴板基准角钢的连接肢 0.X肢;1.Y肢
	short m_nNormOffset;			//法向偏移
	short m_nThick;					//板厚
	short A;						//台口高
	short B;						//顶部余量
	double m_fMargin;				//靴板延伸至底板边缘时的间隙值
	double m_fAlongVecLen;			//靴板延杆件长度方向延伸量
	double m_fCutLen;				//切角值
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
