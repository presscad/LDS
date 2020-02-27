// HashObjIdList.h: interface for the CHashObjIdList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HASHOBJIDLIST_H__1C5396D9_D70E_4894_8562_BA19A9AAF928__INCLUDED_)
#define AFX_HASHOBJIDLIST_H__1C5396D9_D70E_4894_8562_BA19A9AAF928__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HashTable.h"
///////////////////////////////////////////////////////
// CLDSObjectId���ڳ��׳����У�ĳЩָ������ָ�룬��������ʱδ�õ�֪ͨ����ʹ��ʱ�����쳣��������
///////////////////////////////////////////////////////
class CLDSObject;
class CLDSObjectId{
	friend class CHashObjIdList;
	DWORD id;			//�����ʶ������������޸ģ�ֻ����CHashObjIdList���޸ģ�
	CLDSObject* m_pObj;	//ָ��������ݶ����ָ��
public:
	static CHashList<CLDSObjectId>* m_pHashIdTbl;
	CLDSObjectId(){id=0;m_pObj=NULL;}
	bool IsEmpty(){return m_pObj==NULL;}
	operator DWORD(){return id;}
	operator CLDSObject*(){return m_pObj;}
	DWORD Id(){return id;}
	CLDSObject* Object(){return m_pObj;}
	void UpdateObject(CLDSObject* pObj){m_pObj=pObj;}
};
typedef CLDSObjectId* LDS_HANDLE;

class CHashObjIdList
{
	typedef struct tagDATA_TYPE
	{
		//DWORD key;
		CLDSObjectId atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		tagDATA_TYPE(){;}
	}DATA_TYPE;
	ULONG m_uHashTableGrowSize;
	CHashTable<CLDSObjectId*> hashTable;
	//CStack<DWORD*> POS_STACK;
    DWORD NodeNum;			// �ܸ���
    DATA_TYPE* cursor;		// ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHashObjIdList();
    virtual ~CHashObjIdList();

//2.���г�Ա��������
public:
//3.˽�г�Ա��������
private:
	void RebuildHashtable()
	{
		int hashSize;
		if(m_uHashTableGrowSize>0)
			hashSize=NodeNum-NodeNum%m_uHashTableGrowSize+m_uHashTableGrowSize;
		else if(NodeNum>0)
			hashSize=NodeNum+NodeNum;
		else
			hashSize=100;
		if(hashSize<50)
			hashSize=50;	//��С��ϣ���СΪ50;
		hashTable.Empty();
		hashTable.CreateHashTable(hashSize);
		DATA_TYPE* temptr=cursor;	//push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
			hashTable.SetValueAt(cursor->atom.id,&cursor->atom);
		cursor=temptr;				//pop_stack();
	}
//4.���г�Ա��������
public:
	void SetHashTableGrowSize(ULONG growSize){
		m_uHashTableGrowSize=growSize;
		RebuildHashtable();
	}
    LDS_HANDLE Add(DWORD key)//�ڽڵ������ĩβ��ӽڵ�
	{
		CLDSObjectId *pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom.id = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,&cursor->atom);
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->atom.id = key;
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &ptr->atom;
	}
    LDS_HANDLE SetValue(DWORD key, CLDSObject *obj)//�ڽڵ������ĩβ��ӽڵ�
	{
		CLDSObjectId *pObjId=GetValue(key);
		if(pObjId!=NULL)
		{
			pObjId->m_pObj=obj;
			return pObjId;
		}
		else if(NodeNum<=0)//������
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom.id = key;
			cursor->atom.m_pObj = obj;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,&cursor->atom);
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->atom.m_pObj = obj;
		ptr->atom.id = key;
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &ptr->atom;
	}

    LDS_HANDLE GetValue(DWORD key)
	{
		LDS_HANDLE pH=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pH);
		return pH;
	}
    LDS_HANDLE GetNext()
	{
		if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			return &cursor->atom;
		}
		else
			return NULL;
	}
    LDS_HANDLE GetPrev()
	{
		if(cursor&&cursor->prev)
		{
	    	cursor = cursor->prev;
			return &cursor->atom;
		}
		else
			return NULL;
	}
    LDS_HANDLE GetFirst()
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			return &cursor->atom;
		}
	}
    LDS_HANDLE GetTail()
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			return &cursor->atom;
		}
	}
    LDS_HANDLE GetCursor()
	{
		if(cursor)
			return &cursor->atom;
		else
			return NULL;
	}
	DWORD GetNodeNum()const{return NodeNum;}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		//POS_STACK.Empty();
		cursor=tail;
		while(cursor!=NULL)
		{
			cursor = cursor->prev;
			delete tail;
			tail = cursor;
		}
		hashTable.EmptyKeys();
		NodeNum=0;
		head = tail = NULL;
		cursor = NULL;
	}
	/*bool push_stack()
	{
		if(cursor)
		{
			POS_STACK.push(cursor);
			return true;
		}
		else
			return false;
	}
	bool pop_stack()
	{
		if(POS_STACK.pop(cursor))
			return true;
		else
		{
			cursor = NULL;
			return false;
		}
	}
	int GetStackRemnantSize(){return POS_STACK.GetRemnantSize();}
	void EmptyStack(){POS_STACK.Empty();}*/
};

#endif // !defined(AFX_HASHOBJIDLIST_H__1C5396D9_D70E_4894_8562_BA19A9AAF928__INCLUDED_)
