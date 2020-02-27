#ifndef __PART_DEF_H_
#define __PART_DEF_H_
#include "f_ent.h"
#include "f_ent_list.h"

#define MAX_CHAR_GUIGE_16 16
#ifndef THANDLE_DEF
typedef long THANDLE;
#endif

enum TMAPART_TYPE{
	TA_NODE			= 0,		// �ڵ�
	TA_JG			= 1,		// �Ǹ�
	TA_LS			= 2,		// ��˨
	TA_PLANK		= 3,		// �ְ�
	TA_JOINT		= 4,		// ��ͷ
	TA_FOOT			= 5,		// ����
	TA_TUBE			= 6,		// �ֹ�
	TA_FLAT			= 7,		// ����
	TA_SLOT			= 8,		// �۸�
	TA_PARAMPLANK	= 3,		// �ְ�
	TA_OTHER	= 100,
};
enum TMAJG_SUBTYPE{
	TA_JG_COMMON		= 0x00000001,// ��ֱͨ�Ǹ�
	TA_JG_VIRTUAL		= 0x00000002,// ��Ǹ֣���Ϊ������Ҫ��
	TA_JG_WRAP			= 0x00000004,// ���Ǹ�
	TA_JG_EXPLODE		= 0x00000008,// ����ֵĽǸ�
	TA_JG_SHORT			= 0x00000010,// �̽Ǹ�
	TA_JG_HUOQU			= 0x00000020,// �����Ǹ�
	TA_JG_HUOQUSEG		= 0x00000040,// �����Ǹ�
	TA_JG_GROUP			= 0x00000080,// ��ϽǸ�
	TA_JG_GROUPSON		= 0x00000100,// ����ӽǸ�
	TA_JG_OTHER			= 0x00000000,//δ֪
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