#pragma once
#include "HashTable.h"
#include "XhCharString.h"
#include "LogFile.h"

/////////////////////////////////////////////////////////////////////////////
struct PROPERTY_ITEM
{
	bool bActive;
	BYTE cType;	//'b':bool;'w':short;'i':int;'d':double
	union{
		bool	bVal;   // 'b'
		__int16	siVal;  // 'w'
		WORD    wVal;	// 'w'
		DWORD   dwVal;	// 'i'
		__int32	iVal;   // 'i'
		double	dblVal; // 'd'
	}val;
	CXhChar16 name;	//属性名称
	PROPERTY_ITEM(){memset(this,0,sizeof(PROPERTY_ITEM));}
};
struct CMDCFG_ITEM{
	BYTE serial;			//标识序号
	BYTE group;				//分属命令组
	PROPERTY_ITEM properties[4];	//调用func时的输入参数,默认最多支持四项属性，其中属性名为空时表示不开放给用户
	CXhChar50 name;
	bool bActive;
	void (*func)(DWORD dwPara);
	CMDCFG_ITEM(){
		serial=group=0;
		func=NULL;
		bActive=TRUE;
	}
};
struct IBasicOutput{
protected:
	int m_idOutputType;	//1.COutputInfo类型
public:
	virtual void SetMessage(const char* message)=0;
	virtual const char* get_Message(){return "";}
	int GetTypeId() const{return m_idOutputType;}
	virtual const char* GetTypeName()=0;
	virtual void SelectRelationObj()=0;
	virtual CString GetKeyStr()=0;
	virtual bool CopyFrom(IBasicOutput* pOutput)=0;
public:
	long m_hRelaObj1;		//关联对象id1
	long m_hRelaObj2;		//关联对象id2
	__declspec(property(get=get_Message)) const char* szMessage;
};
class COutputInfo;
class CTaskOutputList : public CSuperHashList<IBasicOutput>
{
public:
	CTaskOutputList();
	COutputInfo* AppendSimpleOutput();
	//append函数纯粹为是为了继承原ATOM_LIST<COutputInfo>代码 wjh-2017.12.7
	COutputInfo* append(){return AppendSimpleOutput();}
};
class CTaskLogErrorLife : public CLogErrorLife
{
public:
	CTaskLogErrorLife(CLogFile* pLogFile=NULL);
	~CTaskLogErrorLife(){CLogErrorLife::~CLogErrorLife();}
};