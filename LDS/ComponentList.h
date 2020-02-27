// ComponentList.h: interface for the CComponentList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPONENTLIST_H__3CB727F2_FC95_4146_9A83_BB1C8766B977__INCLUDED_)
#define AFX_COMPONENTLIST_H__3CB727F2_FC95_4146_9A83_BB1C8766B977__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tower.h"
typedef ATOM_LIST<CTower> TOWERLIST;

class CComponentList : public TOWERLIST
{
	BOOL *m_pModified;
public:
	long *base_handle;
	CComponentList();
	virtual ~CComponentList();
public:
	void SetBaseHandleAddress(long *base_handle_addr)
	{base_handle = base_handle_addr;}
	void SetDocModifiedFlagAddress(BOOL *pModified)
	{m_pModified = pModified;}
	CTower* append(BOOL bGetNewHandle=TRUE)//在节点链表的末尾添加节点
	{
		CTower *pComp = ATOM_LIST<CTower>::append();
		if(m_pModified)
		{
			*m_pModified=TRUE;
			//pComp->SetModifiedFlagAddress(m_pModified);
		}
		if(!bGetNewHandle)	//不用自动获得句柄
			return pComp;
		if(base_handle)
			pComp->hTower = (*base_handle)++;
		return pComp;
	}
	CTower* FromHandle(THANDLE handle)	//根据句柄获得节点
	{
		CTower* pTower;
		push_stack();
		for(pTower=GetFirst();pTower!=NULL;pTower=GetNext())
		{
			if(pTower->hTower == handle)
			{
				pop_stack();
				return pTower;
			}
		}
		pop_stack();
		return NULL;
	}
	BOOL DeleteAt(long ii)
	{
		if(m_pModified)
			*m_pModified=TRUE;
		return ATOM_LIST<CTower>::DeleteAt(ii);
	}
	BOOL DeleteNode(THANDLE handle,BOOL bIncPerm=TRUE)
	{
		int i=0;
		for(CTower* pTower=GetFirst();pTower;pTower=GetNext(),i++)
		{
			if(pTower->handle == handle)
			{
				if(m_pModified)
					*m_pModified=TRUE;
				return ATOM_LIST<CTower>::DeleteCursor();
			}
		}
		return FALSE;
	}
	void Empty(){
		if(m_pModified)
			*m_pModified=TRUE;
		ATOM_LIST<CTower>::Empty(); 
	}
};

class CStructure
{
public:
	long handle;
	CStructure(){handle=0;}
	~CStructure(){;}
	CComponentList compList;
};

#endif
