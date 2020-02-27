// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StackMemory.h"
#include "LinkNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStackMemory* LIST_LDSNODEPTR::_ClsStackMemory=NULL;
int LIST_LDSNODEPTR::ALLOC_SIZE=sizeof(LIST_LDSNODEPTR);

LIST_LDSNODEPTR::LIST_LDSNODEPTR(CLDSNode* pNode/*=NULL*/) { 
	_blHeadNodeFlag=true;
	pLdsNode = pNode; 
	pNext = NULL;
}
LIST_LDSNODEPTR::~LIST_LDSNODEPTR()
{
	if(blHeadNodeFlag)
		this->DeleteListNodes();
}
LIST_LDSNODEPTR* LIST_LDSNODEPTR::AppendListNode(CLDSNode* pNode)
{
	if (pLdsNode == pNode)
		return this;
	LIST_LDSNODEPTR* pListNode=this;
	while(pListNode!=NULL)
	{
		if(pListNode->pLdsNode==pNode)
			return pListNode;
		if (pListNode->pNext == NULL)
		{
			if(_ClsStackMemory!=NULL)
			{
				pListNode->pNext=(LIST_LDSNODEPTR*)_ClsStackMemory->AllocMemoryBlock(ALLOC_SIZE);
			}
			if(pListNode->pNext==NULL)
				pListNode->pNext = new LIST_LDSNODEPTR(pNode);
			char* pHeadByte=(char*)pListNode->pNext;
			*pHeadByte=0;	//往私有变量里通过黑客操作写入子节点标识
			return pListNode->pNext;
		}
		pListNode=pListNode->pNext;
	}
	return NULL;
}
void LIST_LDSNODEPTR::DeleteListNodes()
{
	int nAllocSize=sizeof(LIST_LDSNODEPTR);
	LIST_LDSNODEPTR* iter=pNext;
	while(iter!=NULL)
	{
		pNext=iter->pNext;
		if(_ClsStackMemory==NULL||!_ClsStackMemory->CheckMemoryInStack(iter,nAllocSize))
			delete iter;
		iter=pNext;
	}
}
