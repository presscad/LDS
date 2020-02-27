#pragma once
#include "MemberProperty.h"
#include "IModel.h"
#include "HashTable.h"

class CLDSPart;
struct WELDSONPART{
private:
	long _hPart;
	CLDSPart* _pRelaPart;
	friend class CBriefWeldPart;
public:
	static IModel* globalModel;
	int iFuncType;	//���麸���е��ӹ����������ͣ����庬��ȡ���ڲ�ͬ���麸������
	WELDSONPART(CLDSPart* pPart=NULL);
	WELDSONPART(long handle,CLDSPart* pPart=NULL);
public:	//����
	DECLARE_READONLY_PROPERTY(CLDSPart*, pRelaPart);
	DECLARE_PROPERTY(long, hPart);
};
struct SONPART_NODE{
public:
	UINT serial;		//0 based index
	WELDSONPART data;
	SONPART_NODE* next;
	SONPART_NODE(){serial=0;next=NULL;}
	virtual void Destroy(){delete this;}
};
class CBriefWeldPart
{
	SONPART_NODE root,*current,*current_prev;
	char _ciWeldType;	//�麸������
	void IntelliRecogWeldType();
	bool IdentifyStdXPlates();
	bool IdentifyAngleFoot();
	bool IdentifyTubeConnCoverPlates();
	bool IdentifyTubeCrossPlates();
	IModel* BelongModel(){return WELDSONPART::globalModel;}
public:
	static const short WELD_UNINITIAL	= 0;	//δ��ʼ�������麸��
	static const short WELD_COMMON		= 1;	//��ͨ�麸��
	static const short WELD_XENDPLATES	= 2;	//�ֹ����˲�������(����)�������ӵļ��麸��
	static const short WELD_STDX_PLATES	= 3;	//��ϽǸ��б�׼��ʮ�ִ�ֱ���棩����(��)�麸ͼ
	static const short WELD_ANGLEFOOT	= 4;	//�Ǹ����Ű�
	static const short WELD_TUBEFOOT	= 5;	//�ֹ����Ű�
	static const short WELD_SLOPEJOINT	= 6;	//���µ��֧���麸
	static const short WELD_BUSHING		= 7;	//�׹ܹҰ��麸ͼ
	static const short WELD_TUBECONNPLATES_INC_COVER	= 8;	//�ֹ������������Ӱ庬���¸ǰ�
	static const short WELD_TUBECROSSPLATES				= 9;	//�ֹܽ���㴦���Ӱ庬��Խ����
	//static const short WELD_SPHERE		= 100;	//��ڵ�
	CBriefWeldPart();
	virtual ~CBriefWeldPart();
	void SetKey(DWORD key)
	{
		root.data.hPart=key;
	}
	SONPART_NODE* Current(){return current;}
	WELDSONPART* CurrentObject(){
		if(current)
			return &current->data;
		else
			return NULL;
	}
	SONPART_NODE* EnumFirst(bool incRootNode=false){
		current_prev=NULL;
		if(incRootNode)
			return current=&root;
		else
			return current=root.next;
	}
	SONPART_NODE* EnumNext(){
		if(current&&current->next)
		{
			current_prev=current;
			return current=current->next;
		}
		else
			return NULL;
	}
	SONPART_NODE* EnumTail(){
		if(current==NULL)
		{
			current_prev=&root;
			current=root.next;
		}
		while(current&&current->next!=NULL)
		{
			current_prev=current;
			current=current->next;
		}
		return current;
	};
	WELDSONPART* EnumObjectFirst(bool incRootNode=false){
		if(incRootNode)
		{
			current=&root;
			return &root.data;
		}
		else
		{
			SONPART_NODE *pFirstNode=EnumFirst();
			if(pFirstNode)
				return &pFirstNode->data;
			else
				return NULL;
		}
	}
	WELDSONPART* EnumObjectNext(){
		SONPART_NODE *pNextNode=EnumNext();
		if(pNextNode)
			return &pNextNode->data;
		else
			return NULL;
	}
	WELDSONPART* EnumObjectTail(){
		SONPART_NODE* pTailNode=EnumTail();
		if(pTailNode)
			return &pTailNode->data;
		else
			return NULL;
	};
	bool DeleteCursor()
	{
		if(current==NULL)
			return false;
		if(current_prev==NULL)
			root.next=current->next;
		else
			current_prev->next=current->next;
		SONPART_NODE* iter=current->next;
		if(root.next==current)
			root.next=NULL;
		current->Destroy();
		current=iter;
		while(iter!=NULL)
		{
			iter->serial--;
			iter=iter->next;
		}
		return true;
	}

	//ɾ������pStartNode֮�ڵ�֮��ڵ��Ƴ�
	bool DeleteList(SONPART_NODE* pPrevNode=NULL,SONPART_NODE* pDeleteFromNode=NULL)
	{
		if(pDeleteFromNode==NULL&&pPrevNode!=NULL&&pPrevNode->next==NULL)
			return true;	//��ɾ���ڵ���Ϊ��
		SONPART_NODE* iter=root.next;
		if(pDeleteFromNode!=NULL&&pDeleteFromNode!=&root)
		{
			iter=pDeleteFromNode;
			if(pPrevNode==NULL||pPrevNode->next!=pDeleteFromNode)
			{
				pPrevNode=&root;
				while(pPrevNode==NULL||pPrevNode->next!=pDeleteFromNode)
					pPrevNode=pPrevNode->next;
			}
			else if(pPrevNode==NULL)
				return false;
			//else ��֤ͨ��
		}
		else
			pPrevNode=&root;
		while(iter!=NULL)
		{
			pPrevNode->next=iter->next;
			iter->Destroy();
			iter=pPrevNode->next;
		}
		current=pPrevNode;
		return true;
	}
	SONPART_NODE* AssureNextNode()	//ȷ����һ�ڵ���ֵ
	{
		if(current==NULL)
			current=&root;
		if((current=current->next)==NULL)
			AttachNode();
		return current;
	}
	SONPART_NODE* AttachNode()
	{
		SONPART_NODE* sonnode=new SONPART_NODE();
		SONPART_NODE* parent=NULL;
		if(current==NULL)
			current=&root;
		while(current->next!=NULL)
			current=current->next;
		current_prev=current;
		current->next=sonnode;
		sonnode->serial=current->serial+1;
		return current=sonnode;
	}
	SONPART_NODE* AttachNode(WELDSONPART son)
	{
		SONPART_NODE* sonnode=AttachNode();
		sonnode->data=son;
		return sonnode;
	}
	SONPART_NODE* InsertNode(int iInsertIndex=-1)
	{
		if(iInsertIndex==-1)
			return AttachNode();
		SONPART_NODE* sonnode=new SONPART_NODE();
		SONPART_NODE* parent=NULL;
		current_prev=&root;
		current=root.next;
		int index=1;	//root.index=0,���׸���Ч����Ԫ������ֵΪ1
		bool bInserted=false;
		while(current)
		{
			if(index-1==iInsertIndex)
			{
				current_prev->next=sonnode;
				sonnode->serial=index;
				sonnode->next=current;
				current_prev=sonnode;
				bInserted=true;
				index++;
				continue;
			}
			current->serial=index;
			current_prev=current;
			current=current->next;
			index++;
		}
		if(!bInserted)
		{
			sonnode->serial=index;
			current_prev->next=current=sonnode;
		}
		return sonnode;
	}
	SONPART_NODE* InsertNode(WELDSONPART son,int iInsertIndex=-1)
	{
		SONPART_NODE* pInsertNode=InsertNode(iInsertIndex);
		pInsertNode->data=son;
		return pInsertNode;
	}
	WELDSONPART* AttachObject(){ return &(AttachNode()->data);}
	WELDSONPART* AttachObject(WELDSONPART son){ return &(AttachNode(son)->data); }
	WELDSONPART* InsertObject(int iInsertIndex=-1){ return &(InsertNode(iInsertIndex)->data); }
	WELDSONPART* InsertObject(WELDSONPART son,int iInsertIndex=-1){ return &(InsertNode(son,iInsertIndex)->data);}
public:	//����
	BYTE SetConstWeldType(BYTE ciConstWeldType);
	DECLARE_PROPERTY(long, hRootPart);
	DECLARE_READONLY_PROPERTY(BYTE, ciWeldType);	//�麸������
	READONLY_PROPERTY(WELDSONPART*, pRootSonPart);
	GET(pRootSonPart){return &root.data;}
};
//���ڶ�̬����
struct PLACEINDEX{
	UINT id,index;
	void* pObj;
	void SetKey(DWORD key){id=key;}
	PLACEINDEX(){id=index=0;pObj=NULL;}
	//before_id=0��������ǰ;before_id=0xffffffff������ĩβ
	static bool MoveBefore(CHashListEx<PLACEINDEX>* pIndexes,UINT id,UINT before_id);
	static bool MoveAfter(CHashListEx<PLACEINDEX>* pIndexes,UINT id,UINT after_id);
	static bool MoveHead(CHashListEx<PLACEINDEX>* pIndexes,UINT id);
	static bool MoveTail(CHashListEx<PLACEINDEX>* pIndexes,UINT id);
};
