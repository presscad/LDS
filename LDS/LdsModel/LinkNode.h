// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////
#pragma once
#include "f_ent.h"
#include "HeapMemory.h"

struct POS2KEY{
	UINT uiScopeX,uiScopeY,uiScopeZ;
	long liMinX, liMinY, liMinZ;
public:
	POS2KEY(SCOPE_STRU* pScope=NULL)
	{
		Init(pScope);
	}
	long F2I(double fVal)
	{	//重复定义F2I, 目的是避免因ftoi引入f_alg_fun.h
		if(fVal>0)
			return (long)(fVal+0.5);
		else
			return (long)(fVal-0.5);
	}
	void Init(SCOPE_STRU* pScope)
	{
		if (pScope != NULL)
		{
			liMinX = F2I(pScope->fMinX);
			liMinY = F2I(pScope->fMinY);
			liMinZ = F2I(pScope->fMinZ);
			uiScopeX = (UINT)F2I(pScope->fMaxX - pScope->fMinX);
			uiScopeY = (UINT)F2I(pScope->fMaxY - pScope->fMinY);
			uiScopeZ = (UINT)F2I(pScope->fMaxZ - pScope->fMinZ);
		}
		else
		{
			liMinX = liMinY = liMinZ = 0;
			uiScopeX = 100000;
			uiScopeY = 50000;
			uiScopeZ = 100000;
		}
	}
	__int64 ToKey64(const double* pos)
	{
		GEPOINT point(pos);
		long x=F2I(point.x),y=F2I(point.y),z=F2I(point.z);
		__int64 lnLayerArea=uiScopeX;
		lnLayerArea*=uiScopeY;
		__int64 i64key=lnLayerArea*F2I(point.z-liMinZ)+F2I(point.y-liMinY)*uiScopeX+F2I(point.x-liMinX);
		return i64key;
	}
};
template<class TYPE> class LIST_QUEUENODE{
	bool _blHeadNodeFlag;
public:
	IHeapMemory* _ClsStackMemory;
	UINT ALLOC_SIZE;
public:
	TYPE xQueueNode;
	LIST_QUEUENODE<TYPE>* pNext;
	bool get_blHeadNodeFlag(){return _blHeadNodeFlag;}
	__declspec(property(get = get_blHeadNodeFlag)) bool blHeadNodeFlag;
	LIST_QUEUENODE(TYPE dataobj = NULL) { 
		_ClsStackMemory=NULL;
		_blHeadNodeFlag=true;
		xQueueNode = dataobj; 
		pNext = NULL;
		ALLOC_SIZE=sizeof(LIST_QUEUENODE<TYPE>);
	}
	~LIST_QUEUENODE()
	{
		if(blHeadNodeFlag)
			this->DeleteListNodes();
	}
	LIST_QUEUENODE<TYPE>* AppendListNode(TYPE dataobj)
	{
		if (xQueueNode == dataobj)
			return this;
		LIST_QUEUENODE<TYPE>* pListNode=this;
		while(pListNode!=NULL)
		{
			if(pListNode->xQueueNode==dataobj)
				return pListNode;
			if (pListNode->pNext == NULL)
			{
				if(_ClsStackMemory!=NULL)
				{
					pListNode->pNext=(LIST_QUEUENODE<TYPE>*)_ClsStackMemory->AllocMemoryBlock(ALLOC_SIZE);
				}
				if(pListNode->pNext==NULL)
					pListNode->pNext = new LIST_QUEUENODE<TYPE>(dataobj);
				char* pHeadByte=(char*)pListNode->pNext;
				*pHeadByte=0;	//往私有变量里通过黑客操作写入子节点标识
				return pListNode->pNext;
			}
			pListNode=pListNode->pNext;
		}
		return NULL;
	}
	//LIST_LDSNODEPTR* GetListNode(TYPE* pNode)
	void DeleteListNodes()
	{
		LIST_QUEUENODE<TYPE>* iter=pNext;
		while(iter!=NULL)
		{
			pNext=iter->pNext;
			if(_ClsStackMemory==NULL||!_ClsStackMemory->CheckMemoryInStack(iter,ALLOC_SIZE))
				delete iter;
			iter=pNext;
		}
	}
};
class CLDSNode;
class LIST_LDSNODEPTR : public LIST_QUEUENODE<CLDSNode*>{
public:
	LIST_LDSNODEPTR(CLDSNode* pLdsNode=NULL){
		xQueueNode=pLdsNode;
	}
	LIST_LDSNODEPTR(LIST_QUEUENODE<CLDSNode*>& _xQueueNode){
		xQueueNode=_xQueueNode.xQueueNode;
	}
	CLDSNode* get_pLdsNode()const { return xQueueNode; };
	CLDSNode* set_pLdsNode(CLDSNode* value) { return xQueueNode = value; }
	__declspec(property(put=set_pLdsNode, get = get_pLdsNode)) CLDSNode* pLdsNode;
};
