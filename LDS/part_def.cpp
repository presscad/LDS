#include "stdafx.h"
#include "part_def.h"

LDSPART_TYPE IntToPartType(int iType)
{
	switch(iType)
	{
	case TA_NODE:
		return TA_NODE;
	case TA_JG:
		return TA_JG;
	case TA_LS:
		return TA_LS;
	case TA_PLANK:
		return TA_PLANK;
	case TA_JOINT:
		return TA_JOINT;
	case TA_FOOT:
		return TA_FOOT;
	default:
		return TA_OTHER;
	}
}
LDSJG_SUBTYPE IntToJgSubType(int iSubType)
{
	switch(iSubType)
	{
	case TA_JG_COMMON:
		return TA_JG_COMMON;
	case TA_JG_VIRTUAL:
		return TA_JG_VIRTUAL;
	case TA_JG_WRAP:
		return TA_JG_WRAP;
	case TA_JG_EXPLODE:
		return TA_JG_EXPLODE;
	case TA_JG_SHORT:
		return TA_JG_SHORT;
	case TA_JG_HUOQU:
		return TA_JG_HUOQU;
	case TA_JG_HUOQUSEG:
		return TA_JG_HUOQUSEG;
	case TA_JG_GROUP:
		return TA_JG_GROUP;
	default:
		return TA_JG_OTHER;
	}
}
LDSPLANK_SUBTYPE IntToPlankSubType(int iSubType)
{
	switch(iSubType)
	{
	case TA_PLANK_LJB:
		return TA_PLANK_LJB;
	case TA_PLANK_WRAPPLATE:
		return TA_PLANK_WRAPPLATE;
	case TA_PLANK_DIANBAN:
		return TA_PLANK_DIANBAN;
	case TA_PLANK_GUAXIANBAN:
		return TA_PLANK_GUAXIANBAN;
	case TA_PLANK_TIANBAN:
		return TA_PLANK_TIANBAN;
	case TA_PLANK_XUEBAN:
		return TA_PLANK_XUEBAN;
	case TA_PLANK_LEIBAN:
		return TA_PLANK_LEIBAN;
	default:
		return TA_PLANK_LJB;
	}
}