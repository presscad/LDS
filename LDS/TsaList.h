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
	TYPE* append()//�ڽڵ������ĩβ��ӽڵ�
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
    TYPE* GetByIndex(int index)	//���������Ż�ýڵ�
	{return GetByIndex(index);}
    TYPE& operator [](int index)	//ͬ��
	{return myList[index];}
	TYPE* GetNext()	//�����һ�ڵ�
	{return myList.GetNext();}
    TYPE* GetPrev()	//�����һ�ڵ�
	{return myList.GetPrev();}
    TYPE* GetFirst()	//����׽ڵ�
	{return myList.GetFirst();}
    TYPE* GetTail()	//����׽ڵ�
	{return myList.GetTail();}
	TYPE* FromHandle(THANDLE handle)	//���ݾ����ýڵ�
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