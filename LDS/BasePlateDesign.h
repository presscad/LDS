#pragma once

//////////////////////////////////////////////////////////////////////////
#ifdef __ANGLE_PART_INC_

struct BASEPLATE{
	long m_hBaseNode;
	long m_hBaseRod;
	BYTE m_cShape;					//0:表示为方板1:表示为圆板
	BYTE m_cNormOffsetType;			//0.上平面;1.下平面;2.中间面;3.指定偏移
	long m_iVectZType;				//Z方向：0.自定义 1.X+ 2.X- 3.Y+ 4.Y- 5.Z+ 6.Z-
	short m_nNormOffset;			//法高偏移
	short m_nThick;					//板厚
	short C;						//角钢端棱点到底板距离(负值表指定负头)
	double m_fR;					//圆形底板的外轮廓圆半径
	BOOL m_bWeldDiZuo;				//焊接底座
	long XA,XB,YA,YB,XC[4],YC[4];	//A表示X(Y)轴正向,B表示X(Y)轴负向,C表示倒角宽
	BASEPLATE(){memset(this,0,sizeof(BASEPLATE));Init();}
	void Init();
};

#endif