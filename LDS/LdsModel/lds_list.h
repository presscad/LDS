#ifndef __TOWER_LIST_H_
#define __TOWER_LIST_H_
#include "f_ent_list.h"
#include "Hashtable.h"
#include "lds_part.h"
#include "lds_co_part.h"
#include "IModel.h"

//from Tower.cpp
void LoadDefaultObjectInfo(CLDSObject *pObj);

template <class TYPE> class CLdsList : public ATOM_LIST<TYPE>
{
	BOOL *m_pModified;
	IModel *m_pModel;
	CHashTable<TYPE*>hashTable;
public:
	CLdsList();
	~CLdsList();
public:
	BOOL (*IsValidModifyPerm)(CLDSObject *pObj);
	void (*LoadDefaultObjectInfo)(CLDSObject *pObj);
	void SetDocModifiedFlagAddress(BOOL *pModified)
	{m_pModified = pModified;}
	void SetBelongModel(IModel *pModel){m_pModel = pModel;}
	void RebuildHashtable();
	bool UpdateHashtable(long handle, TYPE* pObj);
	TYPE* ModifyHandle(DWORD oldkey, DWORD newkey)
	{
		if(newkey==oldkey)	//��ֵδ�����仯
			return FromHandle(newkey);
		else if(FromHandle(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(FromHandle(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
		TYPE* ret_obj_ptr=NULL;
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(cursor->atom.handle==oldkey)
			{
				hashTable.RemoveKey(cursor->atom.handle);
				cursor->atom.handle=newkey;
				hashTable.SetValueAt(cursor->atom.handle,&cursor->atom);
				ret_obj_ptr=&cursor->atom;
				break;
			}
		}
		pop_stack();
		return ret_obj_ptr;
	}
	IXhEnumerator* NewEnumerator()
	{
		CXhInternalTemplEnumerator<void>* pInterEnum=(CXhInternalTemplEnumerator<void>*)IXhEnumerator::NewEnumerator();
		for(TYPE* pTypeObj=GetFirst();pTypeObj;pTypeObj=GetNext())
			pInterEnum->AppendObjPtr(pTypeObj);
		return pInterEnum;
	}
	TYPE* append(BOOL bGetNewHandle=TRUE)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pTypeObj = ATOM_LIST<TYPE>::append();
		if(m_pModified)
		{
			*m_pModified=TRUE;
			pTypeObj->SetDocModifiedFlagAddress(m_pModified);
		}
		pTypeObj->SetBelongModel(m_pModel);
		if(LoadDefaultObjectInfo!=NULL)
			LoadDefaultObjectInfo(pTypeObj);
		if(!bGetNewHandle)	//�����Զ���þ��
			return pTypeObj;
		pTypeObj->handle = pTypeObj->BelongModel()->GetNewHandle();
		if(hashTable.GetHashTableSize()==0)
			hashTable.CreateHashTable(1000);
		hashTable.SetValueAt(pTypeObj->handle,pTypeObj);
		if((GetNodeNum()/(double)hashTable.GetHashTableSize())>1.5)
			RebuildHashtable();
		return pTypeObj;
	}
	TYPE* FromHandle(THANDLE handle)	//���ݾ����ýڵ�
	{
		TYPE* pType;
		if(hashTable.m_nHashSize>0&&hashTable.GetValueAt(handle,pType)&&pType->handle==handle)
			return pType;
		BOOL pushed=push_stack();
		for(pType=GetFirst();pType!=NULL;pType=GetNext())
		{
			if(pType->handle == handle)
			{
				pop_stack(pushed);
				UpdateHashtable(handle,pType);
				return pType;
			}
		}
		pop_stack(pushed);
		return NULL;
	}
	BOOL DeleteAt(long ii)
	{
		int i=0;
		CAtomListStack<TYPE> stack(this);
		for(CLDSPart* pPart=GetFirst();pPart;pPart=GetNext(),i++)
		{
			if(i==ii)
			{
				if(m_pModified)
					*m_pModified=TRUE;
				return DeleteCursor(TRUE);
			}
		}
		return FALSE;
	}
	BOOL DeleteNode(THANDLE handle,BOOL bIncPerm=TRUE)
	{
		CAtomListStack<TYPE> stack(this);
		for(TYPE* pType=GetFirst();pType;pType=GetNext())
		{
			if(!IsCursorDeleted()&&pType->handle == handle)
			{
				if(m_pModified)
					*m_pModified=TRUE;
				 return DeleteCursor(FALSE,bIncPerm);
			}
		}
		return FALSE;
	}
	BOOL DeleteNodeArr(long* hArr,int nCount,BOOL bIncPerm=TRUE)// ���ݾ��ɾ��һָ������
	{
		CAtomListStack<TYPE> stack(this);
		CHashTable<long> hash(nCount);
		for(int i=0;i<nCount;i++)
			hash.SetValueAt(hArr[i],hArr[i]);
		for(TYPE* pType=GetFirst();pType;pType=GetNext())
		{
			if(!IsCursorDeleted()&&hash.ContainsKey(pType->handle))//pType->handle == handle)
			{
				if(m_pModified)
					*m_pModified=TRUE;
				 return DeleteCursor(FALSE,bIncPerm);
			}
		}
		return FALSE;
	}
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteCursor(BOOL bClean=FALSE,BOOL bIncPerm=TRUE)
	{
		TYPE *pType=GetCursor();
		if(pType==NULL)
			return FALSE;
		//else if(bIncPerm&&IsValidModifyPerm&&!IsValidModifyPerm(pType->dwPermission))
		//	return FALSE;
		else
		{
			if(bIncPerm&&IsValidModifyPerm&&!IsValidModifyPerm(pType))
				return FALSE;
			if(CLDSObject::_console!=NULL)
				CLDSObject::_console->FireDeleteObject(pType);
			hashTable.Delete(pType->handle);
		}
		return ATOM_LIST<TYPE>::DeleteCursor(bClean);
	}
	void Empty(){
		if(m_pModified)
			*m_pModified=TRUE;
		ATOM_LIST<TYPE>::Empty(); 
		hashTable.EmptyKeys();
	}
};

template <class TYPE> 
CLdsList<TYPE>::CLdsList()
{
	hashTable.CreateHashTable(1000);
	m_pModified = NULL;
	m_pModel = NULL;
	IsValidModifyPerm=NULL;
	LoadDefaultObjectInfo=NULL;
}

template <class TYPE> 
CLdsList<TYPE>::~CLdsList()
{

}
template <class TYPE> 
void CLdsList<TYPE>::RebuildHashtable()
{
	hashTable.Empty();
	if(GetNodeNum()==0)	//�ؽ���ϣ��ʱ����ڵ����Ϊ0������Ĭ�Ϲ�ϣ�������ٴδ��ļ�ʱ����UpdateHashtable���� wht 12-09-11
		hashTable.CreateHashTable(1000);
	else 
	{
		hashTable.CreateHashTable(GetNodeNum());
		for(TYPE *pObj=GetFirst();pObj;pObj=GetNext())
		{
			if(!IsCursorDeleted())
				hashTable.SetValueAt(pObj->handle,pObj);
		}
	}
}
template <class TYPE> 
bool CLdsList<TYPE>::UpdateHashtable(long handle, TYPE* pObj)
{
	if(pObj)
		return hashTable.SetValueAt(handle,pObj);
	else
		return hashTable.Delete(handle);
}

//�Զ�ѹջ��ջ�࣬�������ں���������ص�ʱ����ѹջ���ջ��ƥ�����
template <class TYPE> class CAtomListStack{
	int pushed;
	ATOM_LIST<TYPE> *m_pList;
public:
	CAtomListStack(ATOM_LIST<TYPE>* pList){
		m_pList=pList;
		pushed=pList->push_stack();
	}
	~CAtomListStack(){
		m_pList->pop_stack(pushed);
	}
};

//��CLdsPartList class definion
//����ģ����
class CLdsPartList
{
public:
    CLdsPartList();
    ~CLdsPartList(){Empty();}

//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	typedef struct tagDATA_TYPE
	{
		CLDSPart *pPart;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		BOOL bDeleted;
	}DATA_TYPE;
	struct PUSH_ATOM
	{
		long i;
		DATA_TYPE* cursor;
		PUSH_ATOM(){;}
		PUSH_ATOM(DATA_TYPE *pData,long index){cursor=pData,i=index;}
	};
	class SINGULAR_STACK{
		PUSH_ATOM backup;
		DATA_TYPE** m_ppDataCursor;
		long* m_pCurrIndex;
	public:
		SINGULAR_STACK(DATA_TYPE** ppDataCursor,long* pCurrIndex=NULL){
			m_ppDataCursor=ppDataCursor;
			m_pCurrIndex=pCurrIndex;
			backup.cursor=*ppDataCursor;
			if(pCurrIndex!=NULL)
				backup.i=*pCurrIndex;
		}
		~SINGULAR_STACK()
		{
			if(m_ppDataCursor)
				*m_ppDataCursor=backup.cursor;
			if(m_pCurrIndex)
				*m_pCurrIndex=backup.i;
		}
	};
	CStack<PUSH_ATOM> POS_STACK;
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��

	BOOL *m_pModified;
	IModel *m_pModel;
	CHashTable<CLDSPart*>hashTable;
//2.���г�Ա��������
public:
	BOOL (*IsValidModifyPerm)(CLDSObject *pObj);
	void (*LoadDefaultObjectInfo)(CLDSObject *pObj);
	BOOL (*PreDeletePartHookFunc)(CLDSPart* pPart);
//3.˽�г�Ա��������
private:
	DATA_TYPE *CreateNewDataType(int CLASS_TYPEID);
	void DeletePart(CLDSPart *pPart);
//4.���г�Ա��������
public:
	void RebuildHashtable();
	void SetDocModifiedFlagAddress(BOOL *pModified)
	{m_pModified = pModified;}
	void SetBelongModel(IModel *pModel){m_pModel=pModel;}
	CLDSPart* ModifyHandle(DWORD oldkey, DWORD newkey)
	{
		if(newkey==oldkey)	//��ֵδ�����仯
			return FromHandle(newkey);
		else if(FromHandle(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(FromHandle(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
		CLDSPart* ret_obj_ptr=NULL;
		BOOL pushed=push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(cursor->pPart->handle==oldkey)
			{
				hashTable.RemoveKey(cursor->pPart->handle);
				cursor->pPart->handle=newkey;
				hashTable.SetValueAt(cursor->pPart->handle,cursor->pPart);
				ret_obj_ptr=cursor->pPart;
				break;
			}
		}
		pop_stack(pushed);
		return ret_obj_ptr;
	}
	int push_stack();
	bool pop_stack(int push=-1);
	int GetStackRemnantSize(){return POS_STACK.GetRemnantSize();}
    CLDSPart* append(int CLASS_TYPEID,BOOL bGetNewHandle=TRUE);//�ڽڵ������ĩβ��ӽڵ�
	bool UpdateHashtable(long handle, CLDSPart* pPart);
	long GetCurrentIndex()const//��ȡ��ǰ�ڵ��������
	{	return index;	}
	IXhEnumerator* NewEnumerator()
	{
		CXhInternalTemplEnumerator<void>* pInterEnum=(CXhInternalTemplEnumerator<void>*)IXhEnumerator::NewEnumerator();
		for(void* pTypeObj=GetFirst();pTypeObj;pTypeObj=GetNext())
			pInterEnum->AppendObjPtr(pTypeObj);
		return pInterEnum;
	}

    CLDSPart* GetByIndex(long ii)const ;
    CLDSPart& operator[](long ii)const//���������Ż�ȡ�߶�ʵ��
	{	return *GetByIndex(ii);	}
	CLDSPart* FromHandle(THANDLE handle,int CLASS_TYPEID=0);	//���ݾ����ýڵ�
	CLDSPart* FromHandle(THANDLE handle,int CLASS_TYPEID1,int CLASS_TYPEID2);	//���ݾ����ýڵ�
    CLDSPart* FromHandle(THANDLE handle,int CLASS_TYPEID1,int CLASS_TYPEID2,int CLASS_TYPEID3);	//���ݾ����ýڵ�
	CLDSPart* GetNext(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE);
    CLDSPart* GetPrev(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE);
    CLDSPart* GetFirst(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE);
    CLDSPart* GetTail(int CLASS_TYPEID=0,BOOL bIterDelete=FALSE);
	CLDSLinePart* GetNextLinePart();
    CLDSLinePart* GetPrevLinePart();
    CLDSLinePart* GetFirstLinePart();
    CLDSLinePart* GetTailLinePart();
    CLDSPart* GetCursor();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSLinePart* InstantiateCursorLinePart();
#endif
	BOOL IsCursorDeleted(){ return cursor->bDeleted;}
	long GetNodeNum(int CLS_TYPEID=0);
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteCursor(BOOL bClean=FALSE,BOOL bIncPerm=TRUE);
	void Clean();
	BOOL DeleteAt(long ii);	// ɾ��һ���ڵ�
	BOOL DeleteNode(long h,BOOL bIncPerm=TRUE);// ���ݾ��ɾ��һָ������
	BOOL DeleteNodeArr(long* hArr,int nCount,BOOL bIncPerm/*=TRUE*/);// ���ݾ��ɾ��һָ������
	void Empty(int CLASS_TYPEID=0);		// ��յ�ǰ��ʵ��������Ԫ��
	void EmptyStack(){POS_STACK.Empty();}
};
//LDS���������Զ�ѹջ��ջ�࣬�������ں���������ص�ʱ����ѹջ���ջ��ƥ�����
class CLdsPartListStack{
	int pushed;
	CLdsPartList *m_pList;
	IModel *m_pModel;
public:
	CLdsPartListStack(CLdsPartList* pList){
		m_pList=pList;
		m_pModel=NULL;
		pushed=pList->push_stack();
	}
	CLdsPartListStack(IModel *pModel){
		m_pList=NULL;
		m_pModel=pModel;
		pushed=m_pModel->PushPartStack();
	}
	bool EarlyPopStack(){
		if(m_pList)
			m_pList->pop_stack(pushed);
		else if(m_pModel)
			m_pModel->PopPartStack(pushed);
		else
			return false;
		pushed=-1;
		return true;
	}
	~CLdsPartListStack(){
		if(pushed<0)
			return;
		if(m_pList)
			m_pList->pop_stack(pushed);
		else if(m_pModel)
			m_pModel->PopPartStack(pushed);
	}
};
//����ģ����
//ʹ�ø�����ʱ��Ҫ���ûص�����:CreateNewData,DeleteData,GetClassTypeId
template <class TYPE> class CLdsSuperList
{
public:
    CLdsSuperList();
    ~CLdsSuperList(){Empty();}

	//1.���г�Ա��������
public:
	TYPE* (*CreateNewData)(int CLASS_TYPEID);
	BOOL (*DeleteData)(TYPE *pData);
	int (*GetClassTypeId)(TYPE *pData);
	//2.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	typedef struct tagDATA_TYPE
	{
		TYPE *pData;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		BOOL bDeleted;
	}DATA_TYPE;
	typedef struct tagPUSH_ATOM
	{
		DATA_TYPE* cursor;
		int i;
		tagPUSH_ATOM(){;}
		tagPUSH_ATOM(DATA_TYPE *pData,int index){cursor=pData,i=index;}
	}PUSH_ATOM;
	CStack<PUSH_ATOM> POS_STACK;
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
	
	//3.˽�г�Ա��������
private:
	//4.���г�Ա��������
public:
	int push_stack();
	bool pop_stack(int push=-1);
	int GetStackRemnantSize(){return POS_STACK.GetRemnantSize();}
    TYPE* append(int CLASS_TYPEID);//�ڽڵ������ĩβ��ӽڵ�
	long GetCurrentIndex()const//��ȡ��ǰ�ڵ��������
	{	return index;	}
	
    TYPE* GetByIndex(long ii)const ;
    TYPE& operator[](long ii)const//���������Ż�ȡ�߶�ʵ��
	{	return *GetByIndex(ii);	}
	TYPE* GetNext(int CLASS_TYPEID=0,BOOL bInterDelete=FALSE);
    TYPE* GetPrev(int CLASS_TYPEID=0,BOOL bInterDelete=FALSE);
    TYPE* GetFirst(int CLASS_TYPEID=0,BOOL bInterDelete=FALSE);
    TYPE* GetTail(int CLASS_TYPEID=0,BOOL bInterDelete=FALSE);
    TYPE* GetCursor();
	BOOL IsCursorDeleted(){ return cursor->bDeleted;}
	long GetNodeNum(int CLASS_TYPEID=0);
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteCursor(BOOL bClean=FALSE);
	void Clean();
	BOOL DeleteAt(long ii);	// ɾ��һ���ڵ�
    void Empty(int CLASS_TYPEID=0);		// ��յ�ǰ��ʵ��������Ԫ��
	void EmptyStack(){POS_STACK.Empty();}
};
#endif