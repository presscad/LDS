#ifndef __TSA_LIST_H_
#define __TSA_LIST_H_
#include "f_ent_list.h"

template <class TYPE> class CTsaList
{
	BOOL *m_pModified;
	long *base_handle;
	long *work_status_handle;
public:
	CTsaList()
	{
		base_handle = NULL;
		m_pModified = NULL;
	}
	~CTsaList(){;}
	ATOM_LIST<TYPE> myList;
public:
	void push_stack()
	{
		myList.push_stack();
	}
	BOOL pop_stack()
	{
		return myList.pop_stack();
	}
	void SetBaseHandleAddress(long *base_handle_addr)
	{base_handle = base_handle_addr;}
	void SetModifiedFlagAddress(BOOL *pModified)
	{m_pModified = pModified;}
	TYPE* append()//在节点链表的末尾添加节点
	{
		TYPE *pTypeObj = myList.append();
		if(m_pModified)
		{
			*m_pModified=TRUE;
			pTypeObj->SetModifiedFlagAddress(m_pModified);
		}
		if(base_handle)
			pTypeObj->handle = (*base_handle)++;
		return pTypeObj;
	}
    TYPE* GetByIndex(int index)	//根据索引号获得节点
	{return GetByIndex(index);}
    TYPE& operator [](int index)	//同上
	{return myList[index];}
	TYPE* GetNext()	//获得下一节点
	{return myList.GetNext();}
    TYPE* GetPrev()	//获得上一节点
	{return myList.GetPrev();}
    TYPE* GetFirst()	//获得首节点
	{return myList.GetFirst();}
    TYPE* GetTail()	//获得首节点
	{return myList.GetTail();}
	TYPE* FromHandle(THANDLE handle)	//根据句柄获得节点
	{
		TYPE* pType;
		push_stack();
		for(pType=GetFirst();pType!=NULL;pType=GetNext())
		{
			if(pType->handle == handle)
			{
				pop_stack();
				return pType;
			}
		}
		pop_stack();
		return NULL;
	}
	BOOL DeleteAt(int index)
	{
		if(m_pModified&&index>=0&&index<myList.GetNodeNum())
			*m_pModified=TRUE;
		return myList.DeleteNode(index);
	}
	BOOL DeleteNode(THANDLE handle)
	{
		int i=0;
		for(TYPE* pType=GetFirst();pType;pType=GetNext(),i++)
		{
			if(pType->handle == handle)
			{
				if(m_pModified)
					*m_pModified=TRUE;
				return myList.DeleteNode(i);
			}
		}
		return FALSE;
	}
	void Empty(){
		if(m_pModified)
			*m_pModified=TRUE;
		myList.Empty(); 
	}
	int GetNodeNum(){ return myList.GetNodeNum(); }
	void SetNodeNum(int lNewSize)
	{
		Empty();
		for(int i=0;i<lNewSize;i++)
			append();
	}
};

#endif