// PostProcessor.h: interface for the PostProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSTPROCESSOR_H__16997611_E4C5_4B50_B959_09ED8F6C7C4D__INCLUDED_)
#define AFX_POSTPROCESSOR_H__16997611_E4C5_4B50_B959_09ED8F6C7C4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "AnalyzeProcessor.h"
#include "XhCharString.h"
#include "HashSetBuffer.h"

#if defined(__LDS_)||defined(__TSA_)
typedef struct BASENODE_REACTION : public IHashSetBufferObject
{
	CLDSNode *pNode;
	CLDSModule *pModule;
	CWorkStatus *pStatus;
	WORD leg_no[4];
	int status;	//0:��ʾ��ͨ�ڵ�,1��ʾ�������������ϲ���,-1��ʾ��������������ѹ��,��ʱ����
	GEPOINT reaction_force_std,reaction_force_design;	//�������������ı�׼ֵ�����ֵ
	BASENODE_REACTION(){pNode=NULL;pModule=NULL;pStatus=NULL;status=leg_no[0]=leg_no[1]=leg_no[2]=leg_no[3]=0;}
public:
	virtual CXhChar50 GetKeyString() const;
	virtual XHUINT64 GetCompositeKey() const;
	virtual bool ReadObject(BUFFER_IO* pBuff,DWORD version=0);
	virtual bool WriteObject(BUFFER_IO* pBuff,DWORD version=0);
}*REACTION_PTR;

struct TOWERWEIGHT
{
	long cfg_no;
	int iBody0Leg1;
	double netWeight;	//δ������ϵ���ľ���(kg)
	double weight;
};
class CPostProcessReport;
class CPostProcessor  
{
public:
	void Empty();
	CHashList<CElement>hashElems;		//Ӧ������ʱ�����˴�����ȫ���˼���һ����������ڵļ��ϣ�
	CHashStrList<BASENODE_REACTION>Reaction;	//������������,	ֻ�Ǽ�����ʱ�洢,��������������
	CHashList<TOWERWEIGHT>TowerWeight;	//����������(kg),ֻ�Ǽ�����ʱ�洢,��������������
	CHashList<CElement*>hashRodRelaElem;		//�洢���м���˼��Ĺ��ѡ�Ĺ�������Ԫ
	CPostProcessor();
	virtual ~CPostProcessor();
	void ApplyBraceForceToElem();	//���㸨�Ķ���б��֧������б�Ķ�����֧����
};
extern CPostProcessor postprocessor;
#endif
#endif // !defined(AFX_POSTPROCESSOR_H__16997611_E4C5_4B50_B959_09ED8F6C7C4D__INCLUDED_)
