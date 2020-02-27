#include "stdafx.h"

#pragma once

struct DBOBJ_PROPID{
	static const WORD PROP_LOCATION		=1;	//装配位置
	static const WORD PROP_SIZESPEC		=2;	//构件宽度
	static const WORD PROP_WIDTH		=3;	//构件宽度
	static const WORD PROP_THICK		=4;	//构件宽度
	static const WORD PROP_LENGTH		=5;	//构件长度
	static const WORD PROP_MATERIAL		=6;	//构件材质
	static const WORD PROP_NORMAL		=7;	//构件法线
	static const WORD PROP_WINGX_NORMAL=8;	//角钢X肢法线
	static const WORD PROP_WINGY_NORMAL=9;	//角钢Y肢法线
	static const WORD PROP_POS_START	=10;//杆件始端位置
	static const WORD PROP_POS_END		=11;//杆件终端位置
	static const WORD PROP_PROFILE		=12;//钢板外形
};
struct PLATE_PROPID : public DBOBJ_PROPID
{
	struct BASE{	//基座板
		//圆形基座板
		static const WORD R = 13; 
		//方形基座板
		static const WORD XA = 14;
		static const WORD XB = 15;
		static const WORD YA = 16;
		static const WORD YB = 17;
	};
	struct STATIONCONNPLATE{
		static const WORD PROP_HOLE_R=13;	//梢钉孔直径
		static const WORD PROP_HOLE_H=14;	//梢钉孔高度
	};
	struct STATIONSHOEPLATE{
		static const WORD PROP_HOLE_R=13;	//梢钉孔直径
		static const WORD PROP_HOLE_H=14;	//梢钉孔高度
	};
	struct STATIONCROSSPLATE{
		static const WORD PROP_PARA_H=13;	//插板高度
	};
};
