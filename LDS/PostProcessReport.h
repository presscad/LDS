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
	CLDSNode *pNode;	//Ϊ����Ч�ʣ���ʱ�洢
	bool bFatherNode;	//���ڵ�(�Գ����ɵĴ���ӽڵ��ֵΪfalse)
	CXhChar16 symmetry;	//�Գ���Ϣ
};
//�˼�������˨������ϵ���������ڲ���ʱ�ȶ��������ã�
struct ROD_BEARLOADCOEF
{
	long hRod;				//�˼����
	double fStartBearCoef;	//ʼ����˨���ӳ���ϵ��
	double fEndBearCoef;	//�ն���˨���ӳ���ϵ��
};
union QUAD_LEG{	//�Ľ�����Ľ��Ⱥ�
	BYTE  arrQuad[4];
	DWORD dwQuadLeg;
	QUAD_LEG(DWORD dwLeg=0){dwQuadLeg=dwLeg;};
};
struct RSLTOBJ_HEADER{
protected:
	union{
		struct COMPOSITE
		{
			WORD wiModule;	//���ߺţ�1 based
			WORD wDataFlag;	//�����¼�а����������ݵ����ͱ�ʶλ
		}composite;
		DWORD dwModuleAndFlag;
	}_ModuleAndFlag;
	void WriteKeyId(BUFFER_IO* pBuff,DWORD version);//����ֵ��Ϣд�뻺��
public:
	bool ReadKeyId(BUFFER_IO* pBuff,DWORD version);	//�ӻ�������ֵ��Ϣ
public:
	DWORD id;			//������
	DWORD m_dwStatusNo;	//��������Ӧ�Ĺ�����ʶ��
	QUAD_LEG legword;	//�Ľ�����Ľ��Ⱥ�
	DWORD dwAddr;		//�ļ��еĴ洢λ��
	XHUINT64 GetCompositeId() const;
	void FromCompositeId(XHUINT64 idid);
	CXhChar50 GetCompositeIdString() const;
	virtual UINT GetRsltObjBuffSize(DWORD version){return 8;}
//����
	DWORD get_m_iModuleNo(){return _ModuleAndFlag.composite.wiModule;}
	void  set_m_iModuleNo(DWORD val){_ModuleAndFlag.composite.wiModule=(WORD)val;}
	//��������Ӧ��ģ�ͱ�ʶ��
	__declspec( property(put=set_m_iModuleNo,get=get_m_iModuleNo)) DWORD m_iModuleNo;
};
struct RSLTOBJ_KEYPOS{
	DWORD dwAddr;	//�ļ��еĴ洢λ��
	XHUINT64 keyid64;	//64λkeyid
	RSLTOBJ_KEYPOS(XHUINT64 key64=0,DWORD addr=0){dwAddr=addr;keyid64=key64;}
	RSLTOBJ_KEYPOS(RSLTOBJ_HEADER* header){dwAddr=header->dwAddr;keyid64=header->GetCompositeId();}
};
typedef struct CNodeAnalyzeInfo : public IHashSetBufferObject,RSLTOBJ_HEADER{
	CLDSNode* pNode;	//��ʱ����
public:
	virtual CXhChar50 GetKeyString() const;
	virtual XHUINT64 GetCompositeKey() const{return GetCompositeId();}
	virtual bool ReadObject(BUFFER_IO* pBuff,DWORD version);
	virtual bool WriteObject(BUFFER_IO* pBuff,DWORD version);
public:	//��ֵ��
	PROPERTY(DWORD,m_hNode);	//�ڵ���
	GET(m_hNode){return id;}
	SET(m_hNode){id=value;}
	static UINT KeyHeadInfoSize(DWORD version);	//Read/WriteObjectKey��д�Ļ����С
	static UINT BufferSize(DWORD version);
	virtual UINT GetRsltObjBuffSize(DWORD version);
public:	//������
	bool m_bFemNode;		//�Ƿ�Ϊ��ѧ����ʱ�������ڵ�
	GEPOINT offset;		//�ڵ�ĸ���λ��
	GEPOINT rot_offset;	//�ڵ����תλ��
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
	bool ReadObjectKey(BUFFER_IO* pBuff,DWORD version){return ReadKeyId(pBuff,version);}	//�ӻ�������ֵ��Ϣ
	void WriteObjectKey(BUFFER_IO* pBuff,DWORD version){WriteKeyId(pBuff,version);}	//����ֵ��Ϣд�뻺��
	void ToBuffer(BUFFER_IO* pBuff,DWORD version);		//������ֵ��Ϣ
	void FromBuffer(BUFFER_IO* pBuff,DWORD version);	//������ֵ��Ϣ
protected:	//�����ڴ��
	void ToThinBuffer(BUFFER_IO* pBuff);		//������ֵ��Ϣ
	void FromThinBuffer(BUFFER_IO* pBuff);	//������ֵ��Ϣ
}*CNodeAnalyzeInfoPtr;

typedef struct ELEM_ANALYZE_INFO : public IHashSetBufferObject,RSLTOBJ_HEADER{
	CLDSLinePart *pParentRod;	//���˼�, ��ʱ����
public:
	virtual CXhChar50 GetKeyString() const;
	virtual XHUINT64 GetCompositeKey() const{return GetCompositeId();}
	virtual bool ReadObject(BUFFER_IO* pBuff,DWORD version);
	virtual bool WriteObject(BUFFER_IO* pBuff,DWORD version);
public:	//��ֵ��
	PROPERTY(DWORD,idElem);	//��Ԫ��ʶ
	GET(idElem){return id;}
	SET(idElem){id=value;}
	static UINT KeyHeadInfoSize(DWORD version);	//Read/WriteObjectKey��д�Ļ����С
	static UINT BufferSize(DWORD version);
	virtual UINT GetRsltObjBuffSize(DWORD version);
public:	//������
	long iStartI,iEndI;	//ʼ�ն˵���
	long m_hParentRod;	//��Ԫ�������˼����
	double weight;		//��Ԫ����,kN
	double N;		//��λ��N
	GEPOINT moment;	//��λ��N*mm
	GEPOINT VFS,VFE;//ʼĩ�˽���ĺ������, N
	ELEM_ANALYZE_INFO(){
		pParentRod=NULL;
		weight=iStartI=iEndI=m_hParentRod=0;
		idElem=legword.dwQuadLeg=m_dwStatusNo=_ModuleAndFlag.dwModuleAndFlag=0;
	}
	bool ReadObjectKey(BUFFER_IO* pBuff,DWORD version){return ReadKeyId(pBuff,version);}	//�ӻ�������ֵ��Ϣ
	void WriteObjectKey(BUFFER_IO* pBuff,DWORD version){WriteKeyId(pBuff,version);}	//����ֵ��Ϣд�뻺��
	void ToBuffer(BUFFER_IO* pBuff,DWORD version);		//������ֵ��Ϣ
	void FromBuffer(BUFFER_IO* pBuff,DWORD version);	//������ֵ��Ϣ
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
	WORD wLocalPosition;	//ĩβ�ڵ����dwStartAddr��ƫ�Ƶ�ַ
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
	DWORD AppendNodeRsltObject(CNodeAnalyzeInfo& obj);//���ص�ǰ�����ڻ����ļ��еĴ洢��ʼλ��
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
	DWORD AppendElemRsltObject(ELEM_ANALYZE_INFO& obj);//���ص�ǰ�����ڻ����ļ��еĴ洢��ʼλ��
	virtual bool RebuildHashTable();
	virtual DWORD WriteBuffer(BUFFER_IO *buffer_io,DWORD size,char* pool=NULL,DWORD pool_size=0);
};
class CPostProcessReport  
{
	bool m_bHasReportData;
public:
	CXhChar16 m_sRsltItem;		//��Ӧ�ļ�������Ĺؼ����Ʊ�ʶ
	CXhChar16 m_sProductVersion;	//��������Ĳ�Ʒ�汾
	bool m_bLinearAnalysis;		//�����������Ƿ�Ϊ���Է���
	int  m_iMemberSizeCheckCode;//������˼��������ʱ��ִ�е���ƹ淶
	bool m_bHasStdFoundationForce;		//�Ƿ����������������׼ֵ��Ϣ
	CHashList<POSTRPT_NODE>hashNodes;	//����ڵ������Խڵ���Ϊ����
	CHashStrList<BASENODE_REACTION>Reaction;	//������������,	ֻ�Ǽ�����ʱ�洢,��������������
	CHashList<TOWERWEIGHT>TowerWeight;	//����������(kg),ֻ�Ǽ�����ʱ�洢,��������������
	CHashList<CElement>hashElems;	// Ӧ������ʱ�����˴�����ȫ���˼���һ����������ڵļ��ϣ�����ֵΪ��Ԫ��ʶCElement::id
	CMapList<CElement*>ElemPtrs;	//�����������ڵ����е�Ԫ����,��ֵΪ��ĩ�˽ڵ�ľ��
	CHashList<ROD_BEARLOADCOEF>hashRodConnect;
	CHashNodeAnalyzeInfo hashNodeAnalBuffer;	//���ڿ��ٱ���ڵ�ķ���������Ϣ����ʱ���棬Ӧ�ڷ���֮ǰ�������
	CHashElemAnalyzeInfo hashElemAnalBuffer;	//���ڿ��ٱ��浥Ԫ�ķ���������Ϣ����ʱ���棬Ӧ�ڷ���֮ǰ�������
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
