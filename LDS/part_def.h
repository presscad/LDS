#ifndef __PART_DEF_H_
#define __PART_DEF_H_
#include "f_ent.h"
#include "f_ent_list.h"

#define MAX_CHAR_GUIGE_16 16
#ifndef THANDLE_DEF
typedef long THANDLE;
#endif

enum TMAPART_TYPE{
	TA_NODE			= 0,		// 节点
	TA_JG			= 1,		// 角钢
	TA_LS			= 2,		// 螺栓
	TA_PLANK		= 3,		// 钢板
	TA_JOINT		= 4,		// 接头
	TA_FOOT			= 5,		// 塔脚
	TA_TUBE			= 6,		// 钢管
	TA_FLAT			= 7,		// 扁铁
	TA_SLOT			= 8,		// 槽钢
	TA_PARAMPLANK	= 3,		// 钢板
	TA_OTHER	= 100,
};
enum TMAJG_SUBTYPE{
	TA_JG_COMMON		= 0x00000001,// 普通直角钢
	TA_JG_VIRTUAL		= 0x00000002,// 虚角钢（仅为构造需要）
	TA_JG_WRAP			= 0x00000004,// 包角钢
	TA_JG_EXPLODE		= 0x00000008,// 被拆分的角钢
	TA_JG_SHORT			= 0x00000010,// 短角钢
	TA_JG_HUOQU			= 0x00000020,// 火曲角钢
	TA_JG_HUOQUSEG		= 0x00000040,// 火曲角钢
	TA_JG_GROUP			= 0x00000080,// 组合角钢
	TA_JG_GROUPSON		= 0x00000100,// 组合子角钢
	TA_JG_OTHER			= 0x00000000,//未知
};
enum TMAPLANK_SUBTYPE{
	TA_PLANK_LJB			= 0,
	TA_PLANK_WRAPPLATE		= 1,
	TA_PLANK_DIANBAN		= 2,
	TA_PLANK_GUAXIANBAN		= 3,
	TA_PLANK_TIANBAN		= 4,
	TA_PLANK_XUEBAN			= 5,
	TA_PLANK_LEIBAN			= 6,
};
TMAPART_TYPE IntToPartType(int iType);
TMAJG_SUBTYPE IntToJgSubType(int iType);
TMAPLANK_SUBTYPE IntToPlankSubType(int iType);
#endif