#pragma once
#include "list.h"
#include "lds_part.h"
#include "IModel.h"

class CNewPartsListen{
	CLdsPartList *m_pList;
	IModel *m_pModel;
	int m_iPushedPartListStackCurr;
	int m_iPushedPartListStackTail;
public:
	ARRAY_LIST<CLDSPart*> xarrNewParts;
	CNewPartsListen(CLdsPartList* pList,CLDSPartPtr* pxarrNewPartsPool=NULL,int nMaxPoolSize=1){
		m_pList=pList;
		m_pModel=NULL;
		m_iPushedPartListStackCurr=pList->push_stack();
		pList->GetTail();
		m_iPushedPartListStackTail=pList->push_stack();
		if (pxarrNewPartsPool != NULL)
			xarrNewParts.Attach(pxarrNewPartsPool, 0, nMaxPoolSize);
		else
			xarrNewParts.SetSize(0,100);
	}
	CNewPartsListen(IModel *pModel,CLDSPartPtr* pxarrNewPartsPool=NULL,int nMaxPoolSize=1){
		m_pList=NULL;
		m_pModel=pModel;
		m_iPushedPartListStackCurr=m_pModel->PushPartStack();
		m_pModel->MoveToTailPart();
		m_iPushedPartListStackTail=m_pModel->PushPartStack();
		if (pxarrNewPartsPool != NULL)
			xarrNewParts.Attach(pxarrNewPartsPool, 0, nMaxPoolSize);
		else
			xarrNewParts.SetSize(0,100);
	}
	bool EndListen(bool blGatherNewParts = true) {
		//恢复到创建新对象之前的状态
		if (m_pList)
			m_pList->pop_stack(m_iPushedPartListStackTail);
		else if (m_pModel)
			m_pModel->PopPartStack(m_iPushedPartListStackTail);
		else
		{
			m_iPushedPartListStackTail = m_iPushedPartListStackCurr = -1;
			return false;
		}
		if (blGatherNewParts)
		{
			CLDSPart* pNewPart = NULL;
			do {
				if (m_pList)
					pNewPart = m_pList->GetNext();
				else if (m_pModel)
					pNewPart = m_pModel->EnumPartNext();
				else
					break;
				if (pNewPart != NULL)
					xarrNewParts.append(pNewPart);
			} while (pNewPart != NULL);
		}
		//恢复到监听前的状态
		if(m_pList)
			m_pList->pop_stack(m_iPushedPartListStackCurr);
		else if(m_pModel)
			m_pModel->PopPartStack(m_iPushedPartListStackCurr);
		else
		{
			m_iPushedPartListStackTail = m_iPushedPartListStackCurr = -1;
			return false;
		}
		m_iPushedPartListStackTail = m_iPushedPartListStackCurr = -1;
		return true;
	}
	~CNewPartsListen(){
		if(m_iPushedPartListStackCurr>=0&&m_iPushedPartListStackTail>=0)
			EndListen(false);
	}
};

