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
// CLDSObjectId用于彻底程序中，某些指向对象的指针，因对象被清除时未得到通知继续使用时出现异常死机问题
///////////////////////////////////////////////////////
class CLDSObject;
class CLDSObjectId{
	friend class CHashObjIdList;
	DWORD id;			//句柄标识，不允许外界修改，只能在CHashObjIdList中修改，
	CLDSObject* m_pObj;	//指向关联数据对象的指针
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
    DWORD NodeNum;			// 总个数
    DATA_TYPE* cursor;		// 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
    CHashObjIdList();
    virtual ~CHashObjIdList();

//2.公有成员变量定义
public:
//3.私有成员函数定义
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
			hashSize=50;	//最小哈希表大小为50;
		hashTable.Empty();
		hashTable.CreateHashTable(hashSize);
		DATA_TYPE* temptr=cursor;	//push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
			hashTable.SetValueAt(cursor->atom.id,&cursor->atom);
		cursor=temptr;				//pop_stack();
	}
//4.公有成员函数定义
public:
	void SetHashTableGrowSize(ULONG growSize){
		m_uHashTableGrowSize=growSize;
		RebuildHashtable();
	}
    LDS_HANDLE Add(DWORD key)//在节点链表的末尾添加节点
	{
		CLDSObjectId *pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//空链表
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
    LDS_HANDLE SetValue(DWORD key, CLDSObject *obj)//在节点链表的末尾添加节点
	{
		CLDSObjectId *pObjId=GetValue(key);
		if(pObjId!=NULL)
		{
			pObjId->m_pObj=obj;
			return pObjId;
		}
		else if(NodeNum<=0)//空链表
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
    void Empty()		// 清空当前类实例的所有元素
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
