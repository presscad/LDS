#include "stdafx.h"
#include "lds_part.h"
#include "ArrayList.h"
#include "./FittingLibrary.h"

//////////////////////////////////////////////////////////////////////////
//EB挂板规格数据
static const int MAX_EB_SIZE_COUNT =8;
int CURRENT_EB_SIZE_COUNT = 6;
FITTING_EB _xarrPresetEBSizeTbl[MAX_EB_SIZE_COUNT] =
{	//size,b,S(预留挂线角钢通厚),M,Ф,L,H,R
	{"EB-10/21-100",16,16,24,20, 80,100,24},
	{"EB-16/21-100",18,16,24,26, 80,100,32},
	{"EB-16/42-100",18,16,36,26,100,100,32},
	{"EB-21/42-100",20,16,36,26,100,100,32},
	{"EB-21/42-120",20,16,36,26,112,120,32},
	{"EB-32/42-100",28,16,36,33,100,100,40},
};
ARRAY_LIST<FITTING_EB> xarrEBSizeTbl;
CFittingEBLibrary globalFittingEBLibrary;
CFittingEBLibrary::CFittingEBLibrary()
{
	if (xarrEBSizeTbl.Count==0)
		xarrEBSizeTbl.Attach(_xarrPresetEBSizeTbl,CURRENT_EB_SIZE_COUNT,MAX_EB_SIZE_COUNT);
}
bool CFittingEBLibrary::GlobalInitialze() {
	if (xarrEBSizeTbl.Count==0)
		xarrEBSizeTbl.Attach(_xarrPresetEBSizeTbl,CURRENT_EB_SIZE_COUNT,MAX_EB_SIZE_COUNT);
	else
		return false;
	return true;
}
int CFittingEBLibrary::RecCount()
{	//记录数
	return xarrEBSizeTbl.Count;
}
int CFittingEBLibrary::SetRecCount(int count)
{
	xarrEBSizeTbl.SetSize(count);
	return count;
}
FITTING_EB* CFittingEBLibrary::GetAt(int index)
{
	return xarrEBSizeTbl.GetAt(index);
}
FITTING_EB* CFittingEBLibrary::SetAt(int index,const FITTING_EB& sizerec)
{
	if (index<xarrEBSizeTbl.Count)
		xarrEBSizeTbl[index]=sizerec;
	return xarrEBSizeTbl.GetAt(index);
}
FITTING_EB* CFittingEBLibrary::AppendSizeRecord(const FITTING_EB& sizerec)
{
	int count=xarrEBSizeTbl.Count+1;
	xarrEBSizeTbl.SetSize(count);
	xarrEBSizeTbl[count-1]=sizerec;
	return xarrEBSizeTbl.GetAt(count-1);
}

FITTING_EB* CFittingEBLibrary::GetSizeRecTbl()
{
	return xarrEBSizeTbl.m_pData;
}
FITTING_EB* CFittingEBLibrary::QueryFittingRecord(const char* label)
{
	for (int i = 0; i < xarrEBSizeTbl.Count; i++)
	{
		if (stricmp(xarrEBSizeTbl[i].szSizeCode, label) != 0)
			continue;
		return &xarrEBSizeTbl[i];
	}
	return NULL;
}
FITTING_EB* CFittingEBLibrary::MatchFittingRecord(double dfHoleD)
{
	for (int i = 1; i < xarrEBSizeTbl.Count; i++)
	{
		if (xarrEBSizeTbl[i].dfTrunnionD >= dfHoleD)
			return &xarrEBSizeTbl[i - 1];
	}
	return NULL;
}
bool CFittingEBLibrary::LoadSizeParams(const char* label,FITTING_EB* pFittingRecord/*=NULL*/)
{
	for (int i = 0; i < xarrEBSizeTbl.Count; i++)
	{
		if (stricmp(xarrEBSizeTbl[i].szSizeCode, label) != 0)
			continue;
		if(pFittingRecord)
			*pFittingRecord = xarrEBSizeTbl[i];
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////
//GD挂板规格数据
static const int MAX_GD_SIZE_COUNT =16;
int CURRENT_GD_SIZE_COUNT = 10;
FITTING_GD _xarrPresetGDSizeTbl[MAX_GD_SIZE_COUNT] =
{	//size,b,S(预留挂线角钢通厚),M,Ф,H,H1,H2,L,L1
	{"GD-125",16,16,22,24,30.0,30,12,230,112},
	{"GD-215",20,16,24,26,40.0,35,12,250,112},
	{"GD-325",28,16,30,33,40.0,36,12,258,112},
	{"GD-425",32,16,36,39,45.0,36,12,278,130},
	{"GD-505",32,16,36,39,48.0,50,12,278,112},
	{"GD-12" ,16,16,22,24,30.0,35,12,250,112},
	{"GD-21" ,18,16,24,26,35.0,35,12,250,112},
	{"GD-30" ,30,16,30,33,46.5,35,12,264,112},
	{"GD-50" ,34,16,36,44,50.0,40,12,264,112},
	{"GD-60" ,42,16,42,50,50.0,46,12,290,112},
};
ARRAY_LIST<FITTING_GD> xarrGDSizeTbl;
CFittingGDLibrary globalFittingGDLibrary;
CFittingGDLibrary::CFittingGDLibrary()
{
	if (xarrGDSizeTbl.Count==0)
		xarrGDSizeTbl.Attach(_xarrPresetGDSizeTbl,CURRENT_GD_SIZE_COUNT,MAX_GD_SIZE_COUNT);
}
bool CFittingGDLibrary::GlobalInitialze() {
	if (xarrGDSizeTbl.Count==0)
		xarrGDSizeTbl.Attach(_xarrPresetGDSizeTbl,CURRENT_GD_SIZE_COUNT,MAX_GD_SIZE_COUNT);
	else
		return false;
	return true;
}
int CFittingGDLibrary::RecCount()
{	//记录数
	return xarrGDSizeTbl.Count;
}
int CFittingGDLibrary::SetRecCount(int count)
{
	xarrGDSizeTbl.SetSize(count);
	return count;
}
FITTING_GD* CFittingGDLibrary::GetAt(int index)
{
	return xarrGDSizeTbl.GetAt(index);
}
FITTING_GD* CFittingGDLibrary::SetAt(int index,const FITTING_GD& sizerec)
{
	if (index<xarrGDSizeTbl.Count)
		xarrGDSizeTbl[index]=sizerec;
	return xarrGDSizeTbl.GetAt(index);
}
FITTING_GD* CFittingGDLibrary::AppendSizeRecord(const FITTING_GD& sizerec)
{
	int count=xarrGDSizeTbl.Count+1;
	xarrGDSizeTbl.SetSize(count);
	xarrGDSizeTbl[count-1]=sizerec;
	return xarrGDSizeTbl.GetAt(count-1);
}

FITTING_GD* CFittingGDLibrary::GetSizeRecTbl()
{
	return xarrGDSizeTbl.m_pData;
}
FITTING_GD* CFittingGDLibrary::QueryFittingRecord(const char* label)
{
	for (int i = 0; i < xarrGDSizeTbl.Count; i++)
	{
		if (stricmp(xarrGDSizeTbl[i].szSizeCode, label) != 0)
			continue;
		return &xarrGDSizeTbl[i];
	}
	return NULL;
}
FITTING_GD* CFittingGDLibrary::MatchFittingRecord(double dfHoleD)
{
	for (int i = 1; i < xarrGDSizeTbl.Count; i++)
	{
		if (xarrGDSizeTbl[i].dfTrunnionD >= dfHoleD)
			return &xarrGDSizeTbl[i - 1];
	}
	return NULL;
}
bool CFittingGDLibrary::LoadSizeParams(const char* label,FITTING_GD* pFittingRecord/*=NULL*/)
{
	for (int i = 0; i < xarrGDSizeTbl.Count; i++)
	{
		if (stricmp(xarrGDSizeTbl[i].szSizeCode, label) != 0)
			continue;
		if(pFittingRecord)
			*pFittingRecord = xarrGDSizeTbl[i];
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////
//U型环规格数据
static const int MAX_UR_SIZE_COUNT = 10;
int CURRENT_UR_SIZE_COUNT = 8;
FITTING_UR _xarrPresetURSizeTbl[MAX_UR_SIZE_COUNT] =
{
	{"U-7" ,0, 80,16,22, 8,16,20,10},	// 1	
	{"U-10",0, 85,18,24, 9,18,22,11},	// 2
	{"U-12",0, 90,22,30,10,20,24,12},	// 3
	{"U-16",0, 95,24,32,11,22,26,13},	// 4
	{"U-21",0,100,27,36,12,24,30,15},	// 5
	{"U-25",0,110,30,40,13,26,34,17},	// 6
	{"U-30",0,130,36,46,15,30,38,19},	// 7
	{"U-50",0,150,42,55,18,36,44,22},	// 8
};
ARRAY_LIST<FITTING_UR> xarrURSizeTbl;
CFittingURLibrary globalFittingURLibrary;
CFittingURLibrary::CFittingURLibrary()
{
	if (xarrURSizeTbl.Count==0)
		xarrURSizeTbl.Attach(_xarrPresetURSizeTbl,CURRENT_UR_SIZE_COUNT,MAX_UR_SIZE_COUNT);
}
bool CFittingURLibrary::GlobalInitialze()
{
	if (xarrURSizeTbl.Count==0)
		xarrURSizeTbl.Attach(_xarrPresetURSizeTbl,CURRENT_UR_SIZE_COUNT,MAX_UR_SIZE_COUNT);
	else
		return false;
	return true;
}
int CFittingURLibrary::RecCount()		//记录数
{
	return xarrURSizeTbl.Count;
}
int CFittingURLibrary::SetRecCount(int count)
{
	xarrURSizeTbl.SetSize(count);
	return count;
}
FITTING_UR* CFittingURLibrary::GetAt(int index)
{
	return xarrURSizeTbl.GetAt(index);
}
FITTING_UR* CFittingURLibrary::SetAt(int index,const FITTING_UR& sizerec)
{
	if (index<xarrURSizeTbl.Count)
		xarrURSizeTbl[index]=sizerec;
	return xarrURSizeTbl.GetAt(index);
}
FITTING_UR* CFittingURLibrary::AppendSizeRecord(const FITTING_UR& sizerec)
{
	int count=xarrURSizeTbl.Count+1;
	xarrURSizeTbl.SetSize(count);
	xarrURSizeTbl[count-1]=sizerec;
	return xarrURSizeTbl.GetAt(count-1);
}
FITTING_UR* CFittingURLibrary::GetSizeRecTbl()
{
	return xarrURSizeTbl.m_pData;
}
FITTING_UR* CFittingURLibrary::QueryFittingRecord(const char* label)
{
	for (int i = 0; i < xarrURSizeTbl.Count; i++)
	{
		if (stricmp(xarrURSizeTbl[i].szSizeCode, label) != 0)
			continue;
		return &xarrURSizeTbl[i];
	}
	return NULL;
}
FITTING_UR* CFittingURLibrary::MatchFittingRecord(double dfHoleD)
{
	for (int i = 1; i < xarrURSizeTbl.Count; i++)
	{
		if (xarrURSizeTbl[i].dfShaftD >= dfHoleD)
			return &xarrURSizeTbl[i-1];
	}
	return NULL;
}
bool CFittingURLibrary::LoadSizeParams(const char* label,FITTING_UR* pFittingRecord/*=NULL*/)
{
	for (int i = 0; i < xarrURSizeTbl.Count; i++)
	{
		if (stricmp(xarrURSizeTbl[i].szSizeCode, label) != 0)
			continue;
		if(pFittingRecord)
			*pFittingRecord = xarrURSizeTbl[i];
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//U型螺丝规格数据
static const int MAX_US_SIZE_COUNT = 10;
int CURRENT_US_SIZE_COUNT = 3;
FITTING_US _xarrPresetUSSizeTbl[MAX_US_SIZE_COUNT] =
{	//size,类型, C, d, d2, H,  h   S	
	{"U--1880",0,80,18,18,90,  60,16},
	{"U--2080",0,80,20,20,100, 70,16},
	{"U--2280",0,80,22,22,118, 90,16},
};
ARRAY_LIST<FITTING_US> xarrUSSizeTbl;
CFittingUSLibrary globalFittingUSLibrary;
CFittingUSLibrary::CFittingUSLibrary()
{
	if (xarrUSSizeTbl.Count == 0)
		xarrUSSizeTbl.Attach(_xarrPresetUSSizeTbl, CURRENT_US_SIZE_COUNT, MAX_US_SIZE_COUNT);
}
bool CFittingUSLibrary::GlobalInitialze() {
	if (xarrUSSizeTbl.Count == 0)
		xarrUSSizeTbl.Attach(_xarrPresetUSSizeTbl, CURRENT_US_SIZE_COUNT, MAX_US_SIZE_COUNT);
	else
		return false;
	return true;
}
int CFittingUSLibrary::RecCount()
{	//记录数
	return xarrUSSizeTbl.Count;
}
int CFittingUSLibrary::SetRecCount(int count)
{
	xarrUSSizeTbl.SetSize(count);
	return count;
}
FITTING_US* CFittingUSLibrary::GetAt(int index)
{
	return xarrUSSizeTbl.GetAt(index);
}
FITTING_US* CFittingUSLibrary::SetAt(int index, const FITTING_US& sizerec)
{
	if (index < xarrUSSizeTbl.Count)
		xarrUSSizeTbl[index] = sizerec;
	return xarrUSSizeTbl.GetAt(index);
}
FITTING_US* CFittingUSLibrary::AppendSizeRecord(const FITTING_US& sizerec)
{
	int count = xarrUSSizeTbl.Count + 1;
	xarrUSSizeTbl.SetSize(count);
	xarrUSSizeTbl[count - 1] = sizerec;
	return xarrUSSizeTbl.GetAt(count - 1);
}

FITTING_US* CFittingUSLibrary::GetSizeRecTbl()
{
	return xarrUSSizeTbl.m_pData;
}
FITTING_US* CFittingUSLibrary::QueryFittingRecord(const char* label)
{
	for (int i = 0; i < xarrUSSizeTbl.Count; i++)
	{
		if (stricmp(xarrUSSizeTbl[i].szSizeCode, label) != 0)
			continue;
		return &xarrUSSizeTbl[i];
	}
	return NULL;
}
FITTING_US* CFittingUSLibrary::MatchFittingRecord(double dfHoleD)
{
	return NULL;
}
bool CFittingUSLibrary::LoadSizeParams(const char* label, FITTING_US* pFittingRecord/*=NULL*/)
{
	for (int i = 0; i < xarrUSSizeTbl.Count; i++)
	{
		if (stricmp(xarrUSSizeTbl[i].szSizeCode, label) != 0)
			continue;
		if (pFittingRecord)
			*pFittingRecord = xarrUSSizeTbl[i];
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//U型挂板规格数据
static const int MAX_UB_SIZE_COUNT = 14;
int CURRENT_UB_SIZE_COUNT = 10;
FITTING_UB _xarrPresetUBSizeTbl[MAX_UB_SIZE_COUNT] =
{  //size,    类型, C,  d, H,  B
	{"UB--7"	,0, 18, 16,70 ,45,0},
	{"UB--10"	,0, 20, 18,80 ,45,0},
	{"UB--12"	,0, 24, 22,100,60,0},
	{"UB--16"	,0, 26, 24,100,60,0},
	{"UB--21"	,0, 30, 27,120,70,0},
	{"UB--30"	,0, 39, 36,150,70,0},
	{"UB--12T"	,0, 24, 22,100,45,0},
	{"UB--16T"	,0, 26, 24,100,45,0},
	{"UB--21T"	,0, 30, 27,120,60,0},
	{"UB--30T"	,0, 39, 36,150,60,0},
};
ARRAY_LIST<FITTING_UB> xarrUBSizeTbl;
CFittingUBLibrary globalFittingUBLibrary;
CFittingUBLibrary::CFittingUBLibrary()
{
	if (xarrUBSizeTbl.Count == 0)
		xarrUBSizeTbl.Attach(_xarrPresetUBSizeTbl, CURRENT_UB_SIZE_COUNT, MAX_UB_SIZE_COUNT);
}
bool CFittingUBLibrary::GlobalInitialze() {
	if (xarrUBSizeTbl.Count == 0)
		xarrUBSizeTbl.Attach(_xarrPresetUBSizeTbl, CURRENT_UB_SIZE_COUNT, MAX_UB_SIZE_COUNT);
	else
		return false;
	return true;
}
int CFittingUBLibrary::RecCount()
{	//记录数
	return xarrUBSizeTbl.Count;
}
int CFittingUBLibrary::SetRecCount(int count)
{
	xarrUBSizeTbl.SetSize(count);
	return count;
}
FITTING_UB* CFittingUBLibrary::GetAt(int index)
{
	return xarrUBSizeTbl.GetAt(index);
}
FITTING_UB* CFittingUBLibrary::SetAt(int index, const FITTING_UB& sizerec)
{
	if (index < xarrUBSizeTbl.Count)
		xarrUBSizeTbl[index] = sizerec;
	return xarrUBSizeTbl.GetAt(index);
}
FITTING_UB* CFittingUBLibrary::AppendSizeRecord(const FITTING_UB& sizerec)
{
	int count = xarrUBSizeTbl.Count + 1;
	xarrUBSizeTbl.SetSize(count);
	xarrUBSizeTbl[count - 1] = sizerec;
	return xarrUBSizeTbl.GetAt(count - 1);
}

FITTING_UB* CFittingUBLibrary::GetSizeRecTbl()
{
	return xarrUBSizeTbl.m_pData;
}
FITTING_UB* CFittingUBLibrary::QueryFittingRecord(const char* label)
{
	for (int i = 0; i < xarrUBSizeTbl.Count; i++)
	{
		if (stricmp(xarrUBSizeTbl[i].szSizeCode, label) != 0)
			continue;
		return &xarrUBSizeTbl[i];
	}
	return NULL;
}
FITTING_UB* CFittingUBLibrary::MatchFittingRecord(double dfHoleD)
{
	return NULL;
}
bool CFittingUBLibrary::LoadSizeParams(const char* label, FITTING_UB* pFittingRecord/*=NULL*/)
{
	for (int i = 0; i < xarrUBSizeTbl.Count; i++)
	{
		if (stricmp(xarrUBSizeTbl[i].szSizeCode, label) != 0)
			continue;
		if (pFittingRecord)
			*pFittingRecord = xarrUBSizeTbl[i];
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////
void* GetFittingLibrary(UINT idFittingType)
{
	if (idFittingType == TYPE_EB)
		return &globalFittingEBLibrary;
	else if (idFittingType == TYPE_GD)
		return &globalFittingGDLibrary;
	else if (idFittingType == TYPE_UR)
		return &globalFittingURLibrary;
	else if (idFittingType == TYPE_US)
		return &globalFittingUSLibrary;
	else if (idFittingType == TYPE_UB)
		return &globalFittingUBLibrary;
	else
		return NULL;
}
///////////////////////////////////////////////////////////////////
#ifndef __DRAWING_CONTEXT_
static struct GLOBALVARS {
	GLOBALVARS (){
		globalVars.RegisterGlobalVar(Global::VAR(1, &globalFittingEBLibrary));
		globalVars.RegisterGlobalVar(Global::VAR(2, &globalFittingGDLibrary));
		globalVars.RegisterGlobalVar(Global::VAR(3, &globalFittingURLibrary));
		globalVars.RegisterGlobalVar(Global::VAR(4, &globalFittingUSLibrary));
		globalVars.RegisterGlobalVar(Global::VAR(5, &globalFittingUBLibrary));
	}
}InitVars;
#endif
///////////////////////////////////////////////////////////////////
