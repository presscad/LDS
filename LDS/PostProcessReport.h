// PostProcessReport.h: interface for the CPostProcessReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSTPROCESSREPORT_H__018685B9_DF29_4ECB_B67A_686B9CC5C84E__INCLUDED_)
#define AFX_POSTPROCESSREPORT_H__018685B9_DF29_4ECB_B67A_686B9CC5C84E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "PostProcessor.h"
#include "LogFile.h"
#include "HashSetBuffer.h"

#if defined(__LDS_)||defined(__TSA_)
struct POSTRPT_NODE
{
	CLDSNode *pNode;	//为提升效率，临时存储
	bool bFatherNode;	//父节点(对称生成的大号子节点此值为false)
	CXhChar16 symmetry;	//对称信息
};
//杆件两端螺栓连接载系数（仅用于测试时比对验算结果用）
struct ROD_BEARLOADCOEF
{
	long hRod;				//杆件句柄
	double fStartBearCoef;	//始端螺栓连接承载系数
	double fEndBearCoef;	//终端螺栓连接承载系数
};
union QUAD_LEG{	//四角所配的接腿号
	BYTE  arrQuad[4];
	DWORD dwQuadLeg;
	QUAD_LEG(DWORD dwLeg=0){dwQuadLeg=dwLeg;};
};
struct RSLTOBJ_HEADER{
protected:
	union{
		struct COMPOSITE
		{
			WORD wiModule;	//呼高号，1 based
			WORD wDataFlag;	//结果记录中包含数据内容的类型标识位
		}composite;
		DWORD dwModuleAndFlag;
	}_ModuleAndFlag;
	void WriteKeyId(BUFFER_IO* pBuff,DWORD version);//将键值信息写入缓存
public:
	bool ReadKeyId(BUFFER_IO* pBuff,DWORD version);	//从缓存读入键值信息
public:
	DWORD id;			//对象句柄
	DWORD m_dwStatusNo;	//荷载所对应的工况标识号
	QUAD_LEG legword;	//四角所配的接腿号
	DWORD dwAddr;		//文件中的存储位置
	XHUINT64 GetCompositeId() const;
	void FromCompositeId(XHUINT64 idid);
	CXhChar50 GetCompositeIdString() const;
	virtual UINT GetRsltObjBuffSize(DWORD version){return 8;}
//属性
	DWORD get_m_iModuleNo(){return _ModuleAndFlag.composite.wiModule;}
	void  set_m_iModuleNo(DWORD val){_ModuleAndFlag.composite.wiModule=(WORD)val;}
	//荷载所对应的模型标识号
	__declspec( property(put=set_m_iModuleNo,get=get_m_iModuleNo)) DWORD m_iModuleNo;
};
struct RSLTOBJ_KEYPOS{
	DWORD dwAddr;	//文件中的存储位置
	XHUINT64 keyid64;	//64位keyid
	RSLTOBJ_KEYPOS(XHUINT64 key64=0,DWORD addr=0){dwAddr=addr;keyid64=key64;}
	RSLTOBJ_KEYPOS(RSLTOBJ_HEADER* header){dwAddr=header->dwAddr;keyid64=header->GetCompositeId();}
};
typedef struct CNodeAnalyzeInfo : public IHashSetBufferObject,RSLTOBJ_HEADER{
	CLDSNode* pNode;	//临时变量
public:
	virtual CXhChar50 GetKeyString() const;
	virtual XHUINT64 GetCompositeKey() const{return GetCompositeId();}
	virtual bool ReadObject(BUFFER_IO* pBuff,DWORD version);
	virtual bool WriteObject(BUFFER_IO* pBuff,DWORD version);
public:	//键值区
	PROPERTY(DWORD,m_hNode);	//节点句柄
	GET(m_hNode){return id;}
	SET(m_hNode){id=value;}
	static UINT KeyHeadInfoSize(DWORD version);	//Read/WriteObjectKey读写的缓存大小
	static UINT BufferSize(DWORD version);
	virtual UINT GetRsltObjBuffSize(DWORD version);
public:	//数据区
	bool m_bFemNode;		//是否为力学计算时的受力节点
	GEPOINT offset;		//节点的刚体位移
	GEPOINT rot_offset;	//节点的旋转位移
	GEPOINT internal_force,external_force,unbalance_force,internal_moment,windload;
	CNodeAnalyzeInfo(long hNode=0,long iModuleNo=0,DWORD dwQuadLeg=0,SUBID dwStatusNo=0){
		pNode=NULL;
		m_bFemNode=true;
		m_hNode=hNode;
		m_dwStatusNo=(WORD)dwStatusNo.Id();
		_ModuleAndFlag.dwModuleAndFlag=iModuleNo;
		legword.dwQuadLeg=dwQuadLeg;
	}
	void SetZero(){
		offset=rot_offset=internal_force=external_force=unbalance_force=internal_moment=windload=GEPOINT();
	}
	bool ReadObjectKey(BUFFER_IO* pBuff,DWORD version){return ReadKeyId(pBuff,version);}	//从缓存读入键值信息
	void WriteObjectKey(BUFFER_IO* pBuff,DWORD version){WriteKeyId(pBuff,version);}	//将键值信息写入缓存
	void ToBuffer(BUFFER_IO* pBuff,DWORD version);		//不含键值信息
	void FromBuffer(BUFFER_IO* pBuff,DWORD version);	//不含键值信息
protected:	//精简内存版
	void ToThinBuffer(BUFFER_IO* pBuff);		//不含键值信息
	void FromThinBuffer(BUFFER_IO* pBuff);	//不含键值信息
}*CNodeAnalyzeInfoPtr;

typedef struct ELEM_ANALYZE_INFO : public IHashSetBufferObject,RSLTOBJ_HEADER{
	CLDSLinePart *pParentRod;	//父杆件, 临时变量
public:
	virtual CXhChar50 GetKeyString() const;
	virtual XHUINT64 GetCompositeKey() const{return GetCompositeId();}
	virtual bool ReadObject(BUFFER_IO* pBuff,DWORD version);
	virtual bool WriteObject(BUFFER_IO* pBuff,DWORD version);
public:	//键值区
	PROPERTY(DWORD,idElem);	//单元标识
	GET(idElem){return id;}
	SET(idElem){id=value;}
	static UINT KeyHeadInfoSize(DWORD version);	//Read/WriteObjectKey读写的缓存大小
	static UINT BufferSize(DWORD version);
	virtual UINT GetRsltObjBuffSize(DWORD version);
public:	//数据区
	long iStartI,iEndI;	//始终端点编号
	long m_hParentRod;	//单元归属父杆件句柄
	double weight;		//单元自重,kN
	double N;		//单位，N
	GEPOINT moment;	//单位，N*mm
	GEPOINT VFS,VFE;//始末端截面的横向剪力, N
	ELEM_ANALYZE_INFO(){
		pParentRod=NULL;
		weight=iStartI=iEndI=m_hParentRod=0;
		idElem=legword.dwQuadLeg=m_dwStatusNo=_ModuleAndFlag.dwModuleAndFlag=0;
	}
	bool ReadObjectKey(BUFFER_IO* pBuff,DWORD version){return ReadKeyId(pBuff,version);}	//从缓存读入键值信息
	void WriteObjectKey(BUFFER_IO* pBuff,DWORD version){WriteKeyId(pBuff,version);}	//将键值信息写入缓存
	void ToBuffer(BUFFER_IO* pBuff,DWORD version);		//不含键值信息
	void FromBuffer(BUFFER_IO* pBuff,DWORD version);	//不含键值信息
}*ELEM_ANALYZE_INFO_PTR;

class CListKeyPos{
	struct KEYPOS_PACK{
		static const BYTE KEY_COUNT=56;
		DWORD dwStartAddr;
		struct SON_KEYPOS{
			BYTE cnObjBuffsize;
			XHUINT64 id64;
		}keys[KEY_COUNT];	//504 bytes
		KEYPOS_PACK* next;
		KEYPOS_PACK(){memset(this,0,sizeof(KEYPOS_PACK));}
	} head,*current,*tail;
	char ciCurrent;
	BYTE ciTailCurrent;
	WORD wLocalPosition;	//末尾节点相对dwStartAddr的偏移地址
public:
	RSLTOBJ_KEYPOS CurrentKeyPos;
public:
	CListKeyPos(){tail=current=&head;wLocalPosition=ciTailCurrent=0;ciCurrent=-1;}
	void DeleteList();
	~CListKeyPos();
	bool AppendObject(const RSLTOBJ_KEYPOS& keypos);
	bool ResetToHead();
	bool MoveNext();
};
class CHashNodeAnalyzeInfo : public CHashStrSetBuffer<CNodeAnalyzeInfo>
{
protected:
	CListKeyPos listAddrOfObjs;
	UINT m_nObjCount;
public:
	CHashNodeAnalyzeInfo();
	void EmptyListAddrTempBuff();
	DWORD AppendNodeRsltObject(CNodeAnalyzeInfo& obj);//返回当前对象在缓存文件中的存储起始位置
	virtual bool RebuildHashTable();
	virtual DWORD WriteBuffer(BUFFER_IO *buffer_io,DWORD size,char* pool=NULL,DWORD pool_size=0);
};
class CHashElemAnalyzeInfo : public CHashStrSetBuffer<ELEM_ANALYZE_INFO>
{
protected:
	CListKeyPos listAddrOfObjs;
	UINT m_nObjCount;
public:
	CHashElemAnalyzeInfo();
	void EmptyListAddrTempBuff();
	DWORD AppendElemRsltObject(ELEM_ANALYZE_INFO& obj);//返回当前对象在缓存文件中的存储起始位置
	virtual bool RebuildHashTable();
	virtual DWORD WriteBuffer(BUFFER_IO *buffer_io,DWORD size,char* pool=NULL,DWORD pool_size=0);
};
class CPostProcessReport  
{
	bool m_bHasReportData;
public:
	CXhChar16 m_sRsltItem;		//对应的计算结果项的关键名称标识
	CXhChar16 m_sProductVersion;	//分析软件的产品版本
	bool m_bLinearAnalysis;		//分析的类型是否为线性分析
	int  m_iMemberSizeCheckCode;//分析后杆件验算设计时所执行的设计规范
	bool m_bHasStdFoundationForce;		//是否包含基础作用力标准值信息
	CHashList<POSTRPT_NODE>hashNodes;	//输出节点链表，以节点句柄为主键
	CHashStrList<BASENODE_REACTION>Reaction;	//基础反作用力,	只是计算临时存储,计算结束内容清空
	CHashList<TOWERWEIGHT>TowerWeight;	//各基塔塔重(kg),只是计算临时存储,计算结束内容清空
	CHashList<CElement>hashElems;	// 应力计算时将长杆打碎后的全塔杆件（一个计算操作内的集合），键值为单元标识CElement::id
	CMapList<CElement*>ElemPtrs;	//含辅助材在内的所有单元集合,键值为起末端节点的句柄
	CHashList<ROD_BEARLOADCOEF>hashRodConnect;
	CHashNodeAnalyzeInfo hashNodeAnalBuffer;	//用于快速保存节点的分析过程信息的临时缓存，应在分析之前进行清空
	CHashElemAnalyzeInfo hashElemAnalBuffer;	//用于快速保存单元的分析过程信息的临时缓存，应在分析之前进行清空
	static const UINT CURR_RSLTOBJBUFF_VERSION; //=3
public:
	CPostProcessReport();
	virtual ~CPostProcessReport();
	void Empty();
	void ClearAnalInfoBuffer();
	void AppendToAnalyzeBuffer(CNodeAnalyzeInfo& xNodeAnalyzeInfo);
	void AppendToAnalyzeBuffer(ELEM_ANALYZE_INFO& xElemAnalyzeInfo);
	bool CreateAnalyzeFile(char* file_path,long iModuleNo,DWORD dwQuadLeg,SUBID iStatusIncEiffel);
	bool CreateNodeEquation(char* file_path,long iModuleNo,DWORD dwQuadLeg,SUBID iStatusIncEiffel,long pointI);
	void CreateProcessReport();
	void FromBuffer(CBuffer &buffer);
	void ToBuffer(CBuffer &buffer);
	bool CompareWithLegacyReport(CPostProcessReport& legacy, CLogFile& rpt);
	bool CompareWithOtherProgramReport(CPostProcessReport& outside, CLogFile& rpt);
	bool ReadFromTtaRsltOutFile(LPCTSTR lpszPathName=NULL);
	bool InitProcTempFile(char* filename,DWORD dwRsltObjBuffVersion=0);
};
extern CPostProcessReport postreport;
#endif
#endif // !defined(AFX_POSTPROCESSREPORT_H__018685B9_DF29_4ECB_B67A_686B9CC5C84E__INCLUDED_)
