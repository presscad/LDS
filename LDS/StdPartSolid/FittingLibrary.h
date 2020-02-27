#pragma once
#include "./Fitting.h"
struct CFittingEBLibrary : public IFittingLibrary<FITTING_EB>
{
	CFittingEBLibrary();
	virtual bool GlobalInitialze();
	virtual int RecCount();		//记录数
	virtual int SetRecCount(int count);
	virtual FITTING_EB* GetAt(int index);
	virtual FITTING_EB* SetAt(int index,const FITTING_EB& sizerec);
	virtual FITTING_EB* AppendSizeRecord(const FITTING_EB& sizerec);
	virtual FITTING_EB* GetSizeRecTbl();
	virtual FITTING_EB* QueryFittingRecord(const char* label);
	virtual FITTING_EB* MatchFittingRecord(double dfHoleD);
	virtual bool LoadSizeParams(const char* label,FITTING_EB* pFittingRecord=NULL);
public:	//属性
};
struct CFittingGDLibrary : public IFittingLibrary<FITTING_GD>
{
	CFittingGDLibrary();
	virtual bool GlobalInitialze();
	virtual int RecCount();		//记录数
	virtual int SetRecCount(int count);
	virtual FITTING_GD* GetAt(int index);
	virtual FITTING_GD* SetAt(int index,const FITTING_GD& sizerec);
	virtual FITTING_GD* AppendSizeRecord(const FITTING_GD& sizerec);
	virtual FITTING_GD* GetSizeRecTbl();
	virtual FITTING_GD* QueryFittingRecord(const char* label);
	virtual FITTING_GD* MatchFittingRecord(double dfHoleD);
	virtual bool LoadSizeParams(const char* label,FITTING_GD* pFittingRecord=NULL);
public:	//属性
};


struct CFittingURLibrary : public IFittingLibrary<FITTING_UR>
{
	CFittingURLibrary();
	virtual bool GlobalInitialze();
	virtual int RecCount();		//记录数
	virtual int SetRecCount(int count);
	virtual FITTING_UR* GetAt(int index);
	virtual FITTING_UR* SetAt(int index,const FITTING_UR& sizerec);
	virtual FITTING_UR* AppendSizeRecord(const FITTING_UR& sizerec);
	virtual FITTING_UR* GetSizeRecTbl();
	virtual FITTING_UR* QueryFittingRecord(const char* label);
	virtual FITTING_UR* MatchFittingRecord(double dfHoleD);
	virtual bool LoadSizeParams(const char* label,FITTING_UR* pFittingRecord=NULL);
public:	//属性
};
struct CFittingUSLibrary : public IFittingLibrary<FITTING_US>
{
	CFittingUSLibrary();
	virtual bool GlobalInitialze();
	virtual int RecCount();		//记录数
	virtual int SetRecCount(int count);
	virtual FITTING_US* GetAt(int index);
	virtual FITTING_US* SetAt(int index, const FITTING_US& sizerec);
	virtual FITTING_US* AppendSizeRecord(const FITTING_US& sizerec);
	virtual FITTING_US* GetSizeRecTbl();
	virtual FITTING_US* QueryFittingRecord(const char* label);
	virtual FITTING_US* MatchFittingRecord(double dfHoleD);
	virtual bool LoadSizeParams(const char* label, FITTING_US* pFittingRecord = NULL);
public:	//属性
};

struct CFittingUBLibrary : public IFittingLibrary<FITTING_UB>
{
	CFittingUBLibrary();
	virtual bool GlobalInitialze();
	virtual int RecCount();		//记录数
	virtual int SetRecCount(int count);
	virtual FITTING_UB* GetAt(int index);
	virtual FITTING_UB* SetAt(int index, const FITTING_UB& sizerec);
	virtual FITTING_UB* AppendSizeRecord(const FITTING_UB& sizerec);
	virtual FITTING_UB* GetSizeRecTbl();
	virtual FITTING_UB* QueryFittingRecord(const char* label);
	virtual FITTING_UB* MatchFittingRecord(double dfHoleD);
	virtual bool LoadSizeParams(const char* label, FITTING_UB* pFittingRecord = NULL);
public:	//属性
};
