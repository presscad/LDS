#ifndef __CIRCULAR_STACK_H_
#define __CIRCULAR_STACK_H_
template <class TYPE> class CCirStack
{
	struct DATA_NODE
	{
		TYPE data;
		DATA_NODE *pPrev;
		DATA_NODE *pNext;
	};
	DATA_NODE *pHead;
	DATA_NODE *pCursor;
	TYPE *work;
	long m_nCount;
public:
	CCirStack();
	CCirStack(long nCount);
	~CCirStack(){Empty(); delete pHead;}
	long SetSize(long nCount)
	{
		if(nCount<1)
			return m_nCount;
		else
			m_nCount = nCount;
		return m_nCount;
	}
	BOOL IsEmpty()
	{
		if(pCursor==pHead)
			return TRUE;
		else
			return FALSE;
	}
	long GetNodeCount()
	{
		long i=0;
		DATA_NODE *pTemp = pHead;
		while(pTemp!=pCursor)
		{
			pTemp = pTemp->pNext;
			i++;
		}
		return i;
	}
	long GetRemnants()
	{
		return m_nCount-GetNodeCount();
	}
	void Empty()
	{
		while(pHead!=pHead->pNext)
		{
			DATA_NODE *pTemp = pHead->pNext;
			pHead->pNext = pTemp->pNext;
			pTemp->pNext->pPrev = pHead;
			delete pTemp;
		}
		pCursor = pHead;
		pCursor->pNext = pHead;
		pCursor->pPrev = pHead;
	}
	TYPE* TopAtom()		//访问栈顶元素
	{
		return &pCursor->data;
	}
	void push(const TYPE &val)
	{
		if(pCursor->pNext!=pHead)	//循环栈未满
		{
			pCursor = pCursor->pNext;
			pCursor->data = val;
		}
		else	//循环栈已满
		{
			if(GetNodeCount()<m_nCount)//假满，还可以继续分配内存
			{
				DATA_NODE *pTemp = new DATA_NODE;
				pTemp->data = val;
				pTemp->pPrev = pCursor;
				pTemp->pNext = pCursor->pNext;
				pTemp->pNext->pPrev = pTemp;
				pCursor->pNext = pTemp;
				pCursor = pTemp;
			}
			else	//真满，不能再继续分配内存，只能覆盖以前的节点
			{
				pCursor = pHead;
				pCursor->data = val;
				pHead = pHead->pNext;
			}
		}
	}
	BOOL pop(TYPE& res)
	{
		if(pCursor==pHead)	//栈为空
			return false;
		else
		{
			res = pCursor->data;
			pCursor = pCursor->pPrev;
			return true;
		}
	}
};

template <class TYPE> 
CCirStack<TYPE>::CCirStack()
{
	m_nCount = 1024;
	pHead = new DATA_NODE;
	pCursor = pHead;
	pCursor->pNext = pHead;
	pCursor->pPrev = pHead;
}

template <class TYPE> 
CCirStack<TYPE>::CCirStack(long nCount)
{
	m_nCount = nCount;
	pHead = new DATA_NODE;
	pCursor = pHead;
	pCursor->pNext = pHead;
	pCursor->pPrev = pHead;
}
#endif
