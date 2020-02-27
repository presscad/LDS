// JgNcDataManager.h: interface for the CJgNcDataManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JGNCDATAMANAGER_H__5A5F4482_9864_42A3_AE86_7BA1525A3662__INCLUDED_)
#define AFX_JGNCDATAMANAGER_H__5A5F4482_9864_42A3_AE86_7BA1525A3662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent_list.h"
#include "Variant.h"
typedef struct tagHOLE_HEAD
{
	int d;
	char alias[10];
}HOLE_HEAD;
class CNCDirectoryFile
{
public:
	char DIR_FILE_NAME[MAX_PATH];
	BOOL m_bDirectoryPrintByASCII;
	ATOM_LIST<CVariant> contents;
	CNCDirectoryFile(){memset(DIR_FILE_NAME,0,MAX_PATH);m_bDirectoryPrintByASCII=FALSE;}
};
class CJgNcDataManager  
{
public:
	char extension[4];
	char sAbsolutePosMark[10],sRelativePosMark[10];
	char sQ235[10],sQ345[10],sQ390[10],sQ420[10],sQ460[10];
	char sQ235BriefMark[10],sQ345BriefMark[10],sQ390BriefMark[10],sQ420BriefMark[10],sQ460BriefMark[10];
	BOOL m_bPrintByASCII;
	int STAMP_MAX_LEN;		//最长钢印(字模)长度,mm
	int STAMP_START_LENPOS;	//钢印(字模)起始搜索位置,mm
	int STAMP_CLEARANCE;	//钢印(字模)与螺栓孔间的最小间隙,mm
	char STAMP_IN_WING;		//钢印号所在肢，未设置时查找两肢都合适的位置(‘X’表示在X肢,‘Y’表示在Y肢,其余为双肢查找)
	BOOL m_bSpecialHoleOutputHoleD;	//生成特殊孔冲头信息时直接输出孔直径 wht 17-05-18
	CJgNcDataManager();
	virtual ~CJgNcDataManager();
	BOOL InitJgNcDriver(char *nc_driver);
	ATOM_LIST<CNCDirectoryFile>directory;
	ATOM_LIST<HOLE_HEAD>hole_head;
	ATOM_LIST<CVariant> print_list;
};

#endif // !defined(AFX_JGNCDATAMANAGER_H__5A5F4482_9864_42A3_AE86_7BA1525A3662__INCLUDED_)
