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
	int status;	//0:表示普通节点,1表示该象限最大基础上拨力,-1表示该象限最大基础下压力,临时变量
	GEPOINT reaction_force_std,reaction_force_design;	//基础反作用力的标准值和设计值
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
	double netWeight;	//未含增大系数的净重(kg)
	double weight;
};
class CPostProcessReport;
class CPostProcessor  
{
public:
	void Empty();
	CHashList<CElement>hashElems;		//应力计算时将长杆打碎后的全塔杆件（一个计算操作内的集合）
	CHashStrList<BASENODE_REACTION>Reaction;	//基础反作用力,	只是计算临时存储,计算结束内容清空
	CHashList<TOWERWEIGHT>TowerWeight;	//各基塔塔重(kg),只是计算临时存储,计算结束内容清空
	CHashList<CElement*>hashRodRelaElem;		//存储所有计算杆件的规格选材关联后处理单元
	CPostProcessor();
	virtual ~CPostProcessor();
	void ApplyBraceForceToElem();	//计算辅材对主斜材支撑力及斜材对主材支撑力
};
extern CPostProcessor postprocessor;
#endif
#endif // !defined(AFX_POSTPROCESSOR_H__16997611_E4C5_4B50_B959_09ED8F6C7C4D__INCLUDED_)
