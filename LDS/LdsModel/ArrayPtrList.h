#ifndef __ARRAYPTR_LIST_H_
#define __ARRAYPTR_LIST_H_
#include "f_ent_list.h"
//固定数组元素指针地址的动态数组模板类
template <class TYPE> class ARRAYPTR_LIST : public ATOM_LIST<TYPE>
{
public:
	typedef TYPE* TYPEPTR;
    ARRAYPTR_LIST()
	{// 构造函数
		indexPtr=0;
		m_nSize=0;
		m_nMaxSize=0;
		m_nGrowBy=1;
		m_pIndexTbl=NULL;
	}
    ~ARRAYPTR_LIST(){Empty();}

//1.私有成员变量定义
private:			// 注:“索引”即“下标”
	long indexPtr;
	long m_nSize;
	long m_nMaxSize;
	long m_nGrowBy;
//2.公有成员变量定义
public:
	TYPEPTR* m_pIndexTbl;   // the actual array of data
//3.私有成员函数定义
private:

//4.公有成员函数定义
public:
    TYPE* append()//在节点链表的末尾添加节点
	{
		TYPE *pTypeObj = ATOM_LIST<TYPE>::append();
		if(m_nSize>=m_nMaxSize)
		{
			TYPEPTR* pData=new TYPEPTR[m_nMaxSize+m_nGrowBy];
			memcpy(pData,m_pIndexTbl,sizeof(TYPEPTR)*m_nMaxSize);
			delete[]m_pIndexTbl;
			m_pIndexTbl=pData;
			m_nMaxSize+=m_nGrowBy;
		}
		m_nSize++;
		indexPtr=m_nSize-1;
		m_pIndexTbl[indexPtr]=pTypeObj;
		return m_pIndexTbl[m_nSize-1];
	}
    TYPE* append(const TYPE &atom)//在节点链表的末尾添加节点
	{
		TYPE *pAtom = append();
		*pAtom = atom;
		return pAtom;
	}
	//ii==-1时，在当前节点的前面添加节点；否则在indexPtr所指向节点前添加
    TYPE* insert(const TYPE &atom, long ii=-1)
	{
		TYPE *pTypeObj = ATOM_LIST<TYPE>::insert(atom,ii);
		if(m_nSize>=m_nMaxSize)
		{
			TYPEPTR* pData=new TYPEPTR[m_nMaxSize+m_nGrowBy];
			memcpy(pData,m_pIndexTbl,sizeof(TYPEPTR)*m_nMaxSize);
			delete[]m_pIndexTbl;
			m_pIndexTbl=pData;
			m_nMaxSize+=m_nGrowBy;
		}
		if(ii>=0&&ii<m_nSize)
		{	//在指定位置插入节点
			indexPtr=ii;
			memmove(m_pIndexTbl+ii+1,m_pIndexTbl+ii,sizeof(TYPEPTR)*(m_nSize-ii));
		}
		else 
			indexPtr=m_nSize;
		m_pIndexTbl[indexPtr]=pTypeObj;
		m_nSize++;
		return m_pIndexTbl[indexPtr];
	}
	long GetCurrentIndex()const//获取当前节点的索引号
	{
		return indexPtr;
	}

    TYPE* GetByIndex(long ii)
	{
		if(ii<0||ii>=m_nSize)
    		return NULL;
		return m_pIndexTbl[ii];
	}
    TYPE& operator[](long ii)//根据索引号获取线段实体
	{
		if(ii<0||ii>=m_nSize)
    		throw "数组地址越界！";
		return *m_pIndexTbl[ii];
	}
    TYPE* GetNext()
	{
		ATOM_LIST<TYPE>::GetNext();
		while(indexPtr<m_nSize-1)
		{
			indexPtr++;
			return m_pIndexTbl[indexPtr];
		}
		return NULL;
	}
    TYPE* GetPrev()
	{
		ATOM_LIST<TYPE>::GetPrev();
		while(indexPtr>0)
		{
			indexPtr--;
			return m_pIndexTbl[indexPtr];
		}
		return NULL;
	}
    TYPE* GetFirst()
	{
		ATOM_LIST<TYPE>::GetFirst();
		indexPtr=0;
		if(m_nSize>0)
			return m_pIndexTbl[indexPtr];
		else
			return NULL;
	}
    TYPE* GetTail()
	{
		ATOM_LIST<TYPE>::GetTail();
		indexPtr=m_nSize-1;
		if(indexPtr>=0)
			return m_pIndexTbl[indexPtr];
		else
		{
			indexPtr=0;
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(indexPtr<m_nSize)
			return m_pIndexTbl[indexPtr];
		else
			return NULL;
	}
	void SetSize(long size,long growBy=1)
	{
		if(size>0)
			m_nSize=size;
		else
			m_nSize=0;
		if(growBy>0)
			m_nGrowBy=growBy;
		if(size>m_nMaxSize)
		{
			m_nMaxSize=size;
			if(m_pIndexTbl)
				delete	[]m_pIndexTbl;
			m_pIndexTbl=new TYPEPTR[size];
		}
		else if(m_nMaxSize==0)
			m_pIndexTbl=NULL;
	}
	long GetSize()const{return m_nSize;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    bool DeleteCursor()
	{
		TYPE* pCurrent=GetCursor();
		if(indexPtr<m_nSize)
		{
			BOOL deleted=false;
			for(TYPE* pType=ATOM_LIST<TYPE>::GetFirst();pType;pType=ATOM_LIST<TYPE>::GetNext())
			{
				if(pType==pCurrent)
				{
					deleted=ATOM_LIST<TYPE>::DeleteCursor(TRUE);
					break;
				}
			}
			if(deleted)
			{
				memmove(m_pIndexTbl+indexPtr,m_pIndexTbl+indexPtr+1,sizeof(TYPEPTR)*(m_nSize-indexPtr-1));
				m_nSize--;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
    bool RemoveAt(long ii)	// 删除一个节点
	{
		indexPtr=ii;
		return DeleteCursor();
	}
    void Destroy()		// 销毁整个当前类实例
	{
		~ARRAY_LIST();
	}
    void Empty()		// 清空当前类实例的所有元素
	{
		ATOM_LIST<TYPE>::Empty();
		delete []m_pIndexTbl;
		m_pIndexTbl=NULL;
		m_nSize=m_nMaxSize=indexPtr=0;
	}
	//根据指定指针对应的索引值 wht 12-03-27
	int GetIndex(const TYPE *pObj)
	{
		for(int i=0;i<m_nSize;i++)
		{
			if(m_pIndexTbl[i]==pObj)
				return i;
		}
		return -1;
	}
};
#endif