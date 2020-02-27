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
	CXhChar16 name;	//��������
	PROPERTY_ITEM(){memset(this,0,sizeof(PROPERTY_ITEM));}
};
struct CMDCFG_ITEM{
	BYTE serial;			//��ʶ���
	BYTE group;				//����������
	PROPERTY_ITEM properties[4];	//����funcʱ���������,Ĭ�����֧���������ԣ�����������Ϊ��ʱ��ʾ�����Ÿ��û�
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
	int m_idOutputType;	//1.COutputInfo����
public:
	virtual void SetMessage(const char* message)=0;
	virtual const char* get_Message(){return "";}
	int GetTypeId() const{return m_idOutputType;}
	virtual const char* GetTypeName()=0;
	virtual void SelectRelationObj()=0;
	virtual CString GetKeyStr()=0;
	virtual bool CopyFrom(IBasicOutput* pOutput)=0;
public:
	long m_hRelaObj1;		//��������id1
	long m_hRelaObj2;		//��������id2
	__declspec(property(get=get_Message)) const char* szMessage;
};
class COutputInfo;
class CTaskOutputList : public CSuperHashList<IBasicOutput>
{
public:
	CTaskOutputList();
	COutputInfo* AppendSimpleOutput();
	//append��������Ϊ��Ϊ�˼̳�ԭATOM_LIST<COutputInfo>���� wjh-2017.12.7
	COutputInfo* append(){return AppendSimpleOutput();}
};
class CTaskLogErrorLife : public CLogErrorLife
{
public:
	CTaskLogErrorLife(CLogFile* pLogFile=NULL);
	~CTaskLogErrorLife(){CLogErrorLife::~CLogErrorLife();}
};