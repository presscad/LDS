// 3dPlank.cpp: implementation of the C3dPlank class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Tower.h"
#ifdef __GGG_
#include "SlotArm.h"
#include "ConeTubeParts.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLdsPartList::CLdsPartList()
{// 构造函数
	NodeNum=0;
	index = 0;
	cursor = head = tail = NULL;
	hashTable.CreateHashTable(10000);
	IsValidModifyPerm=NULL;
	LoadDefaultObjectInfo=NULL;
	PreDeletePartHookFunc=NULL;
}

CLdsPartList::DATA_TYPE *CLdsPartList::CreateNewDataType(int CLASS_TYPEID)
{
	DATA_TYPE *pData= new DATA_TYPE;
	switch(CLASS_TYPEID)
	{
	case CLS_LINEPART:
	case CLS_CABLE:
		pData->pPart = new CLDSLinePart();
		break;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	case CLS_BOLT:
		pData->pPart = new CLDSBolt();
		break;
	case CLS_ANCHORBOLT:
		pData->pPart = new CLDSAnchorBolt();
		break;
	case CLS_LINEANGLE:
		pData->pPart = new CLDSLineAngle();
		break;
	/*case CLS_GROUPPOLYANGLE:
		pData->pPart = new CLDSGroupPolyAngle();
		break;*/
	case CLS_LINEFLAT:
		pData->pPart = new CLDSLineFlat();
		break;
	case CLS_LINESLOT:
		pData->pPart = new CLDSLineSlot();
		break;
#ifdef __H_SHAPE_STEEL_
	case CLS_LINEHSHAPE:
		pData->pPart = new CLDSLineHShape();
		break;
#endif
	case CLS_POLYJOINT:
		pData->pPart = new CLDSPolyJoint();
		break;
	case CLS_GROUPLINEANGLE:
		pData->pPart = new CLDSGroupLineAngle();
		break;
	case CLS_POLYPART:
		pData->pPart = new CLDSPolyPart();
		break;
	case CLS_JOINT:
		pData->pPart = new CJoint();
		break;
	case CLS_FOOT:
		pData->pPart = new CFoot();
		break;
	case CLS_LINETUBE:
		pData->pPart = new CLDSLineTube();
		break;
#ifdef __GGG_
	case CLS_CONETUBE:
		pData->pPart = new CLDSConeTube();
		break;
	case CLS_CONETUBEFL:
		pData->pPart = new CLDSConeTubeFl();
		break;
#endif
	case CLS_WELDLINE:
		pData->pPart = new CLDSWeldLine();
		break;
	case CLS_PLATE:
		pData->pPart = new CLDSPlate();
		break;
	case CLS_PARAMPLATE:
		pData->pPart = new CLDSParamPlate();
		break;
	case CLS_SPHERE:
		pData->pPart = new CLDSSphere();
		break;
	case CLS_ARCANGLE:
		pData->pPart = new CLDSArcAngle();
		break;
	case CLS_ARCFLAT:
		pData->pPart = new CLDSArcFlat();
		break;
	case CLS_ARCSLOT:
		pData->pPart = new CLDSArcSlot();
		break;
#ifdef __GGG_
	case CLS_SLOTARM:
		pData->pPart = new CLDSSlotArm();
		break;
#endif
	case CLS_EXTERNALPART:
		pData->pPart = new CLDSExternalPart();
		break;
	case CLS_INSULATOR:
		pData->pPart = new CLDSInsulator();
		break;
	case CLS_FITTINGS:
		pData->pPart = new CLDSFitting();
		break;
#endif
	default:
		delete pData;
		return NULL;
	}
	return pData;
}

int CLdsPartList::push_stack()
{
	if(cursor)
		return POS_STACK.push(PUSH_ATOM(cursor,index));
	else
		return 0;
}
bool CLdsPartList::pop_stack(int push/*=-1*/)
{
	PUSH_ATOM atom;
	if(push==0)
		return false;	//压栈时未成功
	else if(push>0&&push!=POS_STACK.GetPushSize())
		return false;	//压栈位置与出栈位置不配对
	if(POS_STACK.pop(atom))
	{
		cursor=atom.cursor;
		index=atom.i;
		return true;
	}
	else
	{
		cursor = NULL;
		return false;
	}
}
CLDSPart* CLdsPartList::append(int CLASS_TYPEID,BOOL bGetNewHandle/*=TRUE)*/)//在节点链表的末尾添加节点
{
	if(NodeNum<=0)//空链表
	{
		cursor = tail = head = CreateNewDataType(CLASS_TYPEID);
		cursor->bDeleted = FALSE;
		cursor->prev = NULL;
		cursor->next = NULL;
		NodeNum=1;
		index = NodeNum-1;
	}
	else
	{
		DATA_TYPE* ptr = CreateNewDataType(CLASS_TYPEID);
		ptr->bDeleted = FALSE;
		//GetTail();//cursor is equal to tail now.   --00.05.25
		cursor=tail;
		ptr->prev = cursor;
		ptr->next = NULL;
		cursor->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		index = NodeNum-1;
	}
	if(m_pModified)
	{
		*m_pModified=TRUE;
		cursor->pPart->SetDocModifiedFlagAddress(m_pModified);
		cursor->pPart->SetBelongModel(m_pModel);
	}
	if(LoadDefaultObjectInfo!=NULL)
		LoadDefaultObjectInfo(cursor->pPart);
	if(!bGetNewHandle)	//不用自动获得句柄
		return cursor->pPart;
	{	//下面一行还未完成，由于涉及到同时装载多个塔模型的问题，留到未来解决　wjh—2007/09/05
		cursor->pPart->handle = cursor->pPart->BelongModel()->GetNewHandle();
	}
	if(hashTable.GetHashTableSize()==0)
		hashTable.CreateHashTable(10000);
	hashTable.SetValueAt(cursor->pPart->handle,cursor->pPart);
	if((NodeNum/(double)hashTable.GetHashTableSize())>1.5)
		RebuildHashtable();

	return cursor->pPart;
}
bool CLdsPartList::UpdateHashtable(long handle, CLDSPart* pPart)
{
	if(pPart)
		return hashTable.SetValueAt(handle,pPart);
	else
		return hashTable.Delete(handle);
}
//ii==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
/*TYPE* insert(const TYPE &atom, long ii=-1)
{
	DATA_TYPE* ptr = CreateNewDataType(CLASS_TYPEID);
	ptr->atom = atom;

	if(NodeNum<=0)//空链表
	{
    	cursor = head = tail = ptr;
		cursor->prev = NULL;
		cursor->next = NULL;
		NodeNum++;
		index = NodeNum-1;//将新添节点置为当前节点
		return &ptr->atom;
	}

	if(ii == -1)//以当前节点位置为新节点插入位置
	{
		//index = GetCurrentIndex();
		//cursor = &GetByIndex( index );
        	//建立新添节点与链表内原点间的连接关系
		ptr->next = cursor;
  		ptr->prev = cursor->prev;
			//原当前节点处理
		if(cursor->prev!=NULL)
    		cursor->prev->next = ptr;
		else	//cursor原为首节点
			head = ptr;
		cursor->prev = ptr;

		index = ii;//将新添节点置为当前节点
	}
	else if(ii>=0&&ii<NodeNum)//指定插入点位置
	{
		int i=0;
		for(cursor=head;cursor;cursor=cursor->next,i++)
			if(i==ii)
				break;
        	//建立新添节点与链表内原点间的连接关系
		ptr->next = cursor;
  		ptr->prev = cursor->prev;
			//插入位置处原节点处理
		if(cursor->prev!=NULL)
    		cursor->prev->next = ptr;
		else if(cursor->prev==NULL)//指定插入点原为链表首节点
			head = ptr;
		cursor->prev = ptr;

		index = ii;//将新添节点置为当前节点
	}
	else if(ii<0)
	{
  		//GetFirst();//cursor is equal to tail now.   --00.05.25
		cursor=head;
		ptr->prev = NULL;
		ptr->next = cursor;
		cursor->prev = ptr;
		cursor = head =ptr;

		index = 0;//将新添节点置为当前节点
	}
	else //(ii>=NodeNum)
	{
  		//GetTail();//cursor is equal to tail now.   --00.05.25
		cursor=tail;
		ptr->prev = cursor;
		ptr->next = NULL;
		cursor->next = ptr;
		cursor = tail = ptr;

		index = NodeNum;//将新添节点置为当前节点
	}
	NodeNum++;
	return &ptr->atom;
}*/

CLDSPart* CLdsPartList::GetByIndex(long ii)const
{
	long i;

	if(ii<0)
    	return NULL;
	else if(ii>=NodeNum)
    	return NULL;
	else if(ii==0)
		return head->pPart;
	else if(ii==NodeNum-1)
		return tail->pPart;

	DATA_TYPE *tmp_cursor;
   	tmp_cursor = head;
	for( i=0;i<ii;i++)
	{
    	if(tmp_cursor->next!=NULL)
	    	tmp_cursor = tmp_cursor->next;
	}

	return tmp_cursor->pPart;
}
CLDSPart* CLdsPartList::FromHandle(THANDLE handle,int CLASS_TYPEID/*=0*/)	//根据句柄获得节点
{
	CLDSPart* pPart;
	if (handle <= 0)	//防止因为输入0句柄导致大量遍历工件链表 wjh-2019.5.30
		return NULL;
	if(hashTable.m_nHashSize>0&&hashTable.GetValueAt(handle,pPart)&&
		pPart->handle==handle)
	{
		if(CLASS_TYPEID==CLS_LINEPART)
		{
			if(pPart&&pPart->IsLinePart())
				return pPart;
		}
		else if(CLASS_TYPEID==CLS_ARCPART)
		{
			if(pPart&&pPart->IsArcPart())
				return pPart;
		}	
		else if(pPart&&(pPart->GetClassTypeId()==CLASS_TYPEID||CLASS_TYPEID<=0))
			return pPart;
		else
			return NULL;
	}
	SINGULAR_STACK stack(&cursor,&index);	//避免FromPart影响当前游标变量cursor的指向状态
	for(pPart=GetFirst();pPart!=NULL;pPart=GetNext())
	{
		if(pPart->handle == handle)
		{
			if(CLASS_TYPEID==CLS_LINEPART)
			{
				if(pPart&&pPart->IsLinePart())
				{
					UpdateHashtable(handle,pPart);
					return pPart;
				}
				else
					return NULL;
			}
			else if(CLASS_TYPEID==CLS_ARCPART)
			{
				if(pPart&&pPart->IsArcPart())
				{
					UpdateHashtable(handle,pPart);
					return pPart;
				}
				else
					return NULL;
			}	
			else if(pPart->GetClassTypeId()==CLASS_TYPEID||CLASS_TYPEID<=0)
			{
				UpdateHashtable(handle,pPart);
				return pPart;
			}
			else
				return NULL;
		}
	}
	return NULL;
}
CLDSPart* CLdsPartList::FromHandle(THANDLE handle,int CLASS_TYPEID1,int CLASS_TYPEID2)	//根据句柄获得节点
{
	CLDSPart* pPart;
	if(hashTable.GetValueAt(handle,pPart))
	{
		if(pPart&&pPart->IsLinePart()&&(CLASS_TYPEID1==CLS_LINEPART||CLASS_TYPEID2==CLS_LINEPART))
			return pPart;
		if(pPart&&(pPart->GetClassTypeId()==CLASS_TYPEID1||
			pPart->GetClassTypeId()==CLASS_TYPEID2||CLASS_TYPEID1==0||CLASS_TYPEID2==0))
			return pPart;
		else
			return NULL;
	}
	SINGULAR_STACK stack(&cursor,&index);	//避免FromPart影响当前游标变量cursor的指向状态
	for(pPart=GetFirst();pPart!=NULL;pPart=GetNext())
	{
		if(pPart->handle == handle)
		{
			if(pPart->IsLinePart()&&(CLASS_TYPEID1==CLS_LINEPART||CLASS_TYPEID2==CLS_LINEPART))
				return pPart;
			if(pPart->GetClassTypeId()==CLASS_TYPEID1||
				pPart->GetClassTypeId()==CLASS_TYPEID2||CLASS_TYPEID1==0||CLASS_TYPEID2==0)
				return pPart;
			else
				return NULL;
		}
	}
	return NULL;
}

CLDSPart* CLdsPartList::FromHandle(THANDLE handle,int CLASS_TYPEID1,int CLASS_TYPEID2, int CLASS_TYPEID3)	//根据句柄获得节点
{
	CLDSPart* pPart;
	if(hashTable.GetValueAt(handle,pPart))
	{
		if(pPart&&pPart->IsLinePart()&&(CLASS_TYPEID1==CLS_LINEPART||CLASS_TYPEID2==CLS_LINEPART||CLASS_TYPEID3==CLS_LINEPART))
			return pPart;
		if(pPart && 
			(pPart->GetClassTypeId()==CLASS_TYPEID1||
			pPart->GetClassTypeId()==CLASS_TYPEID2||
			pPart->GetClassTypeId()==CLASS_TYPEID3))
			return pPart;
		else
			return NULL;
	}
	SINGULAR_STACK stack(&cursor,&index);	//避免FromPart影响当前游标变量cursor的指向状态
	for(pPart=GetFirst();pPart!=NULL;pPart=GetNext())
	{
		if(pPart->handle == handle)
		{
			if(pPart->IsLinePart()&&(CLASS_TYPEID1==CLS_LINEPART||CLASS_TYPEID2==CLS_LINEPART||CLASS_TYPEID3==CLS_LINEPART))
				return pPart;
			if(pPart->GetClassTypeId()==CLASS_TYPEID1||pPart->GetClassTypeId()==CLASS_TYPEID2||pPart->GetClassTypeId()==CLASS_TYPEID3)
				return pPart;
			else
				return NULL;
		}
	}
	return NULL;
}

long CLdsPartList::GetNodeNum(int CLS_TYPEID/*=0*/)
{
	if(CLS_TYPEID==0)
		return NodeNum;
	else
	{
		CLDSPart *pPart;
		int count=0;
		SINGULAR_STACK stack(&cursor,&index);	//避免FromPart影响当前游标变量cursor的指向状态
		for(pPart=GetFirst();pPart!=NULL;pPart=GetNext())
		{
			if(CLS_TYPEID==CLS_LINEPART)
			{
				if(pPart->IsLinePart())
					count++;
			}
			else if(CLS_TYPEID==CLS_ARCPART)
			{
				if(pPart->IsArcPart())
					count++;
			}	
			else if(pPart->GetClassTypeId()==CLS_TYPEID)
				count++;
		}
		return count;
	}
}

CLDSPart* CLdsPartList::GetNext(int CLASS_TYPEID,BOOL bIterDelete/*=FALSE*/)
{
	while(cursor&&cursor->next!=NULL)
	{
    	index++;
		cursor = cursor->next;
		if((bIterDelete||!cursor->bDeleted)&&(cursor->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0))
			return cursor->pPart;
	}
	return NULL;
}
CLDSPart* CLdsPartList::GetPrev(int CLASS_TYPEID,BOOL bIterDelete/*=FALSE*/)
{
	while(cursor&&cursor->prev!=NULL)
	{
    	index--;
    	cursor = cursor->prev;
		if((bIterDelete||!cursor->bDeleted)&&(cursor->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0))
			return cursor->pPart;
	}
		return NULL;
}
CLDSPart* CLdsPartList::GetFirst(int CLASS_TYPEID,BOOL bIterDelete/*=FALSE*/)
{
	if(head==NULL)
		return NULL;
	else
	{
		cursor = head;
		index = 0;
		while(cursor!=NULL){
			if((bIterDelete||!cursor->bDeleted)&&(cursor->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0))
				return cursor->pPart;
			else
				cursor=cursor->next;
		}
		return NULL;
	}

	/*cursor = head;
	if(head==NULL)
		return NULL;
	else if(head->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0)
	{
		cursor = head;
		index = 0;
		return head->pPart;
	}
	else
		return GetNext(CLASS_TYPEID);*/
}
CLDSPart* CLdsPartList::GetTail(int CLASS_TYPEID,BOOL bIterDelete/*=FALSE*/)
{
	if(tail==NULL)
		return NULL;
	else
	{
		cursor = tail;
		index = NodeNum-1;
		while(cursor!=NULL){
			if((bIterDelete||!cursor->bDeleted)&&(cursor->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0))
				return cursor->pPart;
			else
				cursor=cursor->prev;
		}
		return NULL;
	}
	/*if(tail==NULL)
		return NULL;
	else if(tail->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0)
	{
		cursor = tail;
		index = NodeNum-1;
		return tail->pPart;
	}
	else
		return GetPrev(CLASS_TYPEID);*/
}

CLDSLinePart* CLdsPartList::GetNextLinePart()
{
	while(cursor&&cursor->next!=NULL)
	{
    	index++;
		cursor = cursor->next;
		if(!cursor->bDeleted&&cursor->pPart->IsLinePart())
			return (CLDSLinePart*)cursor->pPart;
	}
	return NULL;
}
CLDSLinePart* CLdsPartList::GetPrevLinePart()
{
	while(cursor&&cursor->prev!=NULL)
	{
    	index--;
    	cursor = cursor->prev;
		if(!cursor->bDeleted&&cursor->pPart->IsLinePart())
			return (CLDSLinePart*)cursor->pPart;
	}
		return NULL;
}
CLDSLinePart* CLdsPartList::GetFirstLinePart()
{
	if(head==NULL)
		return NULL;
	else
	{
		cursor = head;
		index = 0;
		while(cursor!=NULL){
			if(!cursor->bDeleted&&cursor->pPart->IsLinePart())
				return (CLDSLinePart*)cursor->pPart;
			else
				cursor=cursor->next;
		}
		return NULL;
	}

	/*cursor = head;
	if(head==NULL)
		return NULL;
	else if(head->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0)
	{
		cursor = head;
		index = 0;
		return head->pPart;
	}
	else
		return GetNext(CLASS_TYPEID);*/
}
CLDSLinePart* CLdsPartList::GetTailLinePart()
{
	if(tail==NULL)
		return NULL;
	else
	{
		cursor = tail;
		index = NodeNum-1;
		while(cursor!=NULL){
			if(!cursor->bDeleted&&cursor->pPart->IsLinePart())
				return (CLDSLinePart*)cursor->pPart;
			else
				cursor=cursor->prev;
		}
		return NULL;
	}
	/*if(tail==NULL)
		return NULL;
	else if(tail->pPart->GetClassTypeId()==CLASS_TYPEID || CLASS_TYPEID==0)
	{
		cursor = tail;
		index = NodeNum-1;
		return tail->pPart;
	}
	else
		return GetPrev(CLASS_TYPEID);*/
}
CLDSPart* CLdsPartList::GetCursor()
{
	if(cursor)
		return cursor->pPart;
	else
		return NULL;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CLDSLinePart* CLdsPartList::InstantiateCursorLinePart()
{
	if(cursor==NULL||!cursor->pPart->IsLinePart())
		return NULL;
	else
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)cursor->pPart;
		CLDSLinePart *pNewLinePart=pLinePart;
		//获取当前组合角钢的类型
		char cSubClassType=' ';
		if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
			if(pGroupAngle->group_style==0)
				cSubClassType='D';
			else if(pGroupAngle->group_style==1)
				cSubClassType='T';
			else if(pGroupAngle->group_style==2)
				cSubClassType='X';
		}
		if(pLinePart->GetClassTypeId()==CLS_LINEPART||pLinePart->size_idClassType!=pLinePart->GetClassTypeId())	//转换构件类型
		{	//对象类型不一致，重新生成一个新杆件对象
			DATA_TYPE *pDataType=CreateNewDataType(pLinePart->size_idClassType);
			if(pDataType==NULL)
				return NULL;
			cursor->pPart=pDataType->pPart;
			delete pDataType;
			if(m_pModified)
			{
				*m_pModified=TRUE;
				cursor->pPart->SetDocModifiedFlagAddress(m_pModified);
				cursor->pPart->SetBelongModel(m_pModel);
			}
			if(LoadDefaultObjectInfo!=NULL)
				LoadDefaultObjectInfo(cursor->pPart);

			pNewLinePart=(CLDSLinePart*)cursor->pPart;
			pNewLinePart->handle=pLinePart->handle;
			if(hashTable.GetHashTableSize()==0)
				hashTable.CreateHashTable(10000);
			hashTable.SetValueAt(pNewLinePart->handle,pNewLinePart);
			if(pNewLinePart->IsAngle())
				((CLDSLineAngle*)pNewLinePart)->dwLayStyle=pLinePart->des_wing_norm.wLayoutStyle;

			pNewLinePart->cfgword=pLinePart->cfgword;
			pNewLinePart->dwPermission=pLinePart->dwPermission;
			pNewLinePart->dwStartPermission=pLinePart->dwStartPermission;
			pNewLinePart->dwEndPermission=pLinePart->dwEndPermission;//始终端的修改权限
			pNewLinePart->iSeg=pLinePart->iSeg;
			pNewLinePart->SetLayer(pLinePart->layer());
			pNewLinePart->SetPartNo(pLinePart->GetPartNo());
			pNewLinePart->m_bVirtualPart=pLinePart->m_bVirtualPart;
			pNewLinePart->m_bDisplayHole=pLinePart->m_bDisplayHole;
			pNewLinePart->m_hPartWeldParent=pLinePart->m_hPartWeldParent;		//焊接父构件
			pNewLinePart->m_bUserSpecColor=pLinePart->m_bUserSpecColor;		//用户指定构件颜色
			pNewLinePart->crMaterial=pLinePart->crMaterial;		//构件颜色
			pNewLinePart->cMaterial=pLinePart->cMaterial;
			pNewLinePart->m_uStatMatCoef=pLinePart->m_uStatMatCoef;
			pNewLinePart->pStart=pLinePart->pStart;
			pNewLinePart->pEnd=pLinePart->pEnd;
			if(pLinePart->pStart)
				pNewLinePart->SetStart(pLinePart->pStart->ActivePosition());
			if(pLinePart->pEnd)
				pNewLinePart->SetEnd(pLinePart->pEnd->ActivePosition());
			//角钢转换为组合角钢时复制法线设计参数 wht 10-09-28
			/*if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pNewLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				((CLDSLineAngle*)pNewLinePart)->des_norm_x=((CLDSLineAngle*)pLinePart)->des_norm_x;
				((CLDSLineAngle*)pNewLinePart)->des_norm_y=((CLDSLineAngle*)pLinePart)->des_norm_y;
				((CLDSLineAngle*)pNewLinePart)->set_norm_x_wing(((CLDSLineAngle*)pLinePart)->get_norm_x_wing());
				((CLDSLineAngle*)pNewLinePart)->set_norm_y_wing(((CLDSLineAngle*)pLinePart)->get_norm_y_wing());
			}*/
			pNewLinePart->SetStartOdd(pLinePart->startOdd());
			pNewLinePart->SetEndOdd(pLinePart->endOdd());
			if(pLinePart->IsStartPosLocked())
				pNewLinePart->LockStartPos();
			if(pLinePart->IsEndPosLocked())
				pNewLinePart->LockEndPos();
			pNewLinePart->SetSizeSpec(pLinePart->GetSizeSpec());
			pNewLinePart->connectStart=pLinePart->connectStart;
			pNewLinePart->connectEnd=pLinePart->connectEnd;
#ifdef __LDS_
			pNewLinePart->m_fPreTension=pLinePart->m_fPreTension;
			pNewLinePart->m_bAutoCalHoleOutCoef=pLinePart->m_bAutoCalHoleOutCoef;	//自动计算螺栓减孔数，否则为指定螺栓减孔数
			pNewLinePart->start_force_type=pLinePart->start_force_type;
			pNewLinePart->end_force_type=pLinePart->end_force_type;	//角钢两端受力状况CENTRIC_FORCE,ECCENTRIC_FORCE
			//0：完全铰接JOINT_HINGE；1：部分刚接JOINT_RIGID（限制除扭转外的转动约束）,2：自动判断JOINT_AUTOJUSTIFY
			pNewLinePart->start_joint_type=pLinePart->start_joint_type;
			pNewLinePart->end_joint_type=pLinePart->end_joint_type;
			pNewLinePart->CalLenCoef=pLinePart->CalLenCoef;
			pNewLinePart->m_iElemType=pLinePart->m_iElemType;		//0表示自动判断;1表示二力杆桁架;2表示经典梁单元;3表示柔索单元
			pNewLinePart->m_uStatMatNo=pLinePart->m_uStatMatNo;		//统材组号
			pNewLinePart->m_bNeedSelMat=pLinePart->m_bNeedSelMat;	//需要选择材质
			pNewLinePart->m_bNeedSelSize=pLinePart->m_bNeedSelSize;	//需要选择规格
			pNewLinePart->m_fWeightRaiseCoef=pLinePart->m_fWeightRaiseCoef;	//重力放大系数，考虑联板、螺栓等对重力的放大作用
			pNewLinePart->hole_out=pLinePart->hole_out;				//杆件上最多螺栓减孔数
#endif
			pNewLinePart->desStartOdd=pLinePart->desStartOdd;
			pNewLinePart->desEndOdd=pLinePart->desEndOdd;
			pNewLinePart->bHuoQuStart=pLinePart->bHuoQuStart;
			pNewLinePart->bHuoQuEnd=pLinePart->bHuoQuEnd;			//始/终端点是否为火曲端
			pNewLinePart->huoqu_handle=pLinePart->huoqu_handle;		//若是火曲杆件段时此项才有效，表示其所归属的火曲杆件
			strcpy(pNewLinePart->sNotes,pLinePart->sNotes);
			for(RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.GetFirst();pMirObj;pMirObj=pLinePart->relativeObjs.GetNext())
				pNewLinePart->relativeObjs.SetValue(pMirObj->hObj,*pMirObj);
		}
		if(pNewLinePart!=pLinePart|| (pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&cSubClassType!=pLinePart->size_cSubClassType))
		{	//生成了新杆件对象或转换组合角钢类型
			BOOL pushed=push_stack();
			if(pNewLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle* pNewGroupAngle=(CLDSGroupLineAngle*)pNewLinePart;
				if(pLinePart->size_cSubClassType=='D')
					pNewGroupAngle->group_style=0;	//对角组合角钢
				else if(pLinePart->size_cSubClassType=='T')
					pNewGroupAngle->group_style=1;	//T型组合角钢
				else if(pLinePart->size_cSubClassType=='X')
					pNewGroupAngle->group_style=2;	//十字型组合角钢
				if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
					for(int i=0;i<4;i++)
						pNewGroupAngle->son_jg_h[i]=pGroupAngle->son_jg_h[i];
				}
				pNewGroupAngle->AppendSonJg();
			}
			else if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
				for(int i=0;i<4;i++)
				{
					if(pGroupAngle->son_jg_h[i]>0x20)
						DeleteNode(pGroupAngle->son_jg_h[i]);
				}
			}
			pop_stack(pushed);
		}
		if(pNewLinePart!=pLinePart)
			DeletePart(pLinePart);
		return (CLDSLinePart*)cursor->pPart;
	}
}
#endif

// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
BOOL CLdsPartList::DeleteCursor(BOOL bClean/*=FALSE*/,BOOL bIncPerm/*=TRUE*/)
{
	if(cursor==NULL||cursor->bDeleted)
		return FALSE;
	if(bIncPerm&&IsValidModifyPerm&&!IsValidModifyPerm(cursor->pPart))
		return FALSE;
	if (PreDeletePartHookFunc!=NULL&&!PreDeletePartHookFunc(cursor->pPart))
		return FALSE;
	hashTable.Delete(cursor->pPart->handle);
	if(bClean)
	{
		DATA_TYPE *cursor2=NULL;
		if(cursor!=head&&cursor!=tail)
		{
			cursor->prev->next=cursor->next;
			cursor->next->prev=cursor->prev;
			cursor2=cursor->prev;
		}
		else
		{
			if(cursor==head)
			{
				head=cursor->next;
				if(head)
					head->prev=NULL;
				cursor2=head;
			}
			if(cursor==tail)
			{
				tail=cursor->prev;
				if(tail)
					tail->next=NULL;
				cursor2=tail;
			}
		}
		DeletePart(cursor->pPart);
		delete cursor;
		cursor=cursor2;
	}
	else
	{
		if(CLDSObject::_console!=NULL)
			CLDSObject::_console->FireDeleteObject(cursor->pPart);
		cursor->bDeleted=TRUE;
	}
	NodeNum--;
	if(NodeNum<0)
		NodeNum=0;
	return TRUE;
}
void CLdsPartList::Clean()
{
	for(cursor=head;cursor;)
	{
		DATA_TYPE *cursor2=cursor->next;
		if(cursor->bDeleted)
		{	//已被删除元素,移位后清除
			if(cursor!=head&&cursor!=tail)
			{
				cursor->prev->next=cursor->next;
				cursor->next->prev=cursor->prev;
			}
			else
			{
				if(cursor==head)
				{
					head=cursor->next;
					if(head)
						head->prev=NULL;
				}
				if(cursor==tail)
				{
					tail=cursor->prev;
					if(tail)
						tail->next=NULL;
				}
			}
			DeletePart(cursor->pPart);
			delete cursor;
		}
		cursor=cursor2;
	}
}
BOOL CLdsPartList::DeleteAt(long ii)	// 删除一个节点
{
	int i=0;
	SINGULAR_STACK stack(&cursor,&index);	//避免FromPart影响当前游标变量cursor的指向状态
	for(CLDSPart* pPart=GetFirst();pPart;pPart=GetNext(),i++)
	{
		if(i==ii)
			return DeleteCursor(TRUE);
	}
	return FALSE;
}
void CLdsPartList::DeletePart(CLDSPart *pPart)
{
	if(CLDSObject::_console!=NULL)
		CLDSObject::_console->FireDeleteObject(pPart);
	hashTable.Delete(pPart->handle);
	switch(pPart->GetClassTypeId())
	{
	case CLS_LINEPART:
		delete (CLDSLinePart*)pPart;
		break;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	case CLS_BOLT:
		delete (CLDSBolt*)pPart;
		break;
	case CLS_ANCHORBOLT:
		delete (CLDSAnchorBolt*)pPart;
		break;
	case CLS_LINEANGLE:
		delete (CLDSLineAngle*)pPart;
		break;
	case CLS_POLYJOINT:
		delete (CLDSPolyJoint*)pPart;
		break;
	case CLS_GROUPLINEANGLE:
		delete (CLDSGroupLineAngle*)pPart;
		break;
	case CLS_ARCANGLE:
		delete (CLDSArcAngle*)pPart;
		break;
	case CLS_ARCSLOT:
		delete (CLDSArcSlot*)pPart;
		break;
	case CLS_ARCFLAT:
		delete (CLDSArcFlat*)pPart;
		break;
	case CLS_POLYPART:
		delete (CLDSPolyPart*)pPart;
		break;
	case CLS_WELDLINE:
		delete (CLDSWeldLine*)pPart;
		break;
	case CLS_JOINT:
		delete (CJoint*)pPart;
		break;
	case CLS_FOOT:
		delete (CFoot*)pPart;
		break;
	/*case CLS_GROUPPOLYANGLE:
		delete (CLDSGroupPolyAngle*)pPart;
		break;*/
	case CLS_LINETUBE:
		delete (CLDSLineTube*)pPart;
		break;
	case CLS_PLATE:
		delete (CLDSPlate*)pPart;
		break;
	case CLS_PARAMPLATE:
		delete (CLDSParamPlate*)pPart;
		break;
	case CLS_LINEFLAT:
		delete (CLDSLineFlat*)pPart;
		break;
	case CLS_LINESLOT:
		delete (CLDSLineSlot*)pPart;
		break;
#ifdef __H_SHAPE_STEEL_
	case CLS_LINEHSHAPE:
		delete (CLDSLineHShape*)pPart;
		break;
#endif
#ifdef __GGG_
	case CLS_SLOTARM:
		delete (CLDSSlotArm*)pPart;
		break;
#endif
	case CLS_EXTERNALPART:
		delete (CLDSExternalPart*)pPart;
		break;
	case CLS_INSULATOR:
		delete (CLDSInsulator*)pPart;
		break;
	case CLS_FITTINGS:
		delete (CLDSFitting*)pPart;
		break;
#endif
	default:
		delete pPart;
	}
}
void CLdsPartList::Empty(int CLASS_TYPEID)		// 清空当前类实例的所有元素
{
	POS_STACK.Empty();
	//GetTail();
	cursor=tail;	//此处不应该使用GetTail()获取尾指针,否则将跳过bDeleted为TRUE的节点无法删除，导致内存泄露 wht 12-09-20
	while(cursor!=NULL)
	{
		if(cursor->pPart->GetClassTypeId()==CLASS_TYPEID||CLASS_TYPEID==0)
		{
			DATA_TYPE *pDelete=cursor;
			cursor = cursor->prev;
			if(pDelete==tail)
				tail = cursor;
			if(pDelete==head)
				head = NULL;
			if(pDelete->next)
				pDelete->next->prev=cursor;
			if(cursor)
				cursor->next = pDelete->next;
			if(pDelete)
				DeletePart(pDelete->pPart);
			delete pDelete;
			NodeNum--;
		}
		else
			cursor = cursor->prev;
	}
	index = 0;
	head = tail = NULL;
	cursor = NULL;
	hashTable.EmptyKeys();
}
BOOL CLdsPartList::DeleteNode(long h,BOOL bIncPerm/*=TRUE*/)// 根据句柄删除一指定构件
{
	SINGULAR_STACK stack(&cursor,&index);	//避免FromPart影响当前游标变量cursor的指向状态
	for(CLDSPart *pPart=GetFirst();pPart;pPart=GetNext())
	{
		if(!IsCursorDeleted()&&pPart->handle==h)
		{
			if(m_pModified)
				*m_pModified=TRUE;
			return DeleteCursor(FALSE,bIncPerm);
		}
	}
	return FALSE;
}
BOOL CLdsPartList::DeleteNodeArr(long* hArr,int nCount,BOOL bIncPerm/*=TRUE*/)// 根据句柄删除一指定构件
{
	SINGULAR_STACK stack(&cursor,&index);	//避免FromPart影响当前游标变量cursor的指向状态
	CHashTable<long> hash(nCount);
	for(int i=0;i<nCount;i++)
		hash.SetValueAt(hArr[i],hArr[i]);
	for(CLDSPart *pPart=GetFirst();pPart;pPart=GetNext())
	{
		if(!IsCursorDeleted()&&hash.ContainsKey(pPart->handle))//pPart->handle==h)
		{
			if(m_pModified)
				*m_pModified=TRUE;
			return DeleteCursor(FALSE,bIncPerm);
		}
	}
	return FALSE;
}

void CLdsPartList::RebuildHashtable()
{
	//if(NodeNum==0)
	//	hashTable.EmptyKeys();
	//else
	{
		hashTable.Empty();
		hashTable.CreateHashTable(NodeNum);
	}
	for(CLDSPart *pPart=GetFirst();pPart;pPart=GetNext())
	{
		if(!IsCursorDeleted())
			hashTable.SetValueAt(pPart->handle,pPart);
	}
}


//////////////////////////////////////////////////////////////////////////
//CLdsSuperList
CLdsSuperList<class TYPE>::CLdsSuperList()
{// 构造函数
	NodeNum=0;
	index = 0;
	cursor = head = tail = NULL;
	CreateNewData=NULL;
	DeleteData=NULL;
}

int CLdsSuperList<class TYPE>::push_stack()
{
	if(cursor)
		return POS_STACK.push(PUSH_ATOM(cursor,index));
	else
		return 0;
}
bool CLdsSuperList<class TYPE>::pop_stack(int push/*=-1*/)
{
	PUSH_ATOM atom;
	if(POS_STACK.pop(atom))
	{
		cursor=atom.cursor;
		index=atom.i;
		return true;
	}
	else
	{
		cursor = NULL;
		return false;
	}
}
TYPE* CLdsSuperList<class TYPE>::append(int CLASS_TYPEID)//在节点链表的末尾添加节点
{
	TYPE *pData=CreateNewData(CLASS_TYPEID);
	if(pData==NULL)
		return NULL;
	DATA_TYPE* ptr = new DATA_TYPE;
	ptr->bDeleted=FALSE;
	ptr->pData=pData;
	if(NodeNum<=0)//空链表
	{
		cursor = tail = head = ptr;
		cursor->prev = NULL;
		cursor->next = NULL;
		NodeNum=1;
		index = NodeNum-1;
	}
	else
	{
		//GetTail();//cursor is equal to tail now.   --00.05.25
		cursor=tail;
		ptr->prev = cursor;
		ptr->next = NULL;
		cursor->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		index = NodeNum-1;
	}
	return cursor->pData;
}

TYPE* CLdsSuperList<class TYPE>::GetByIndex(long ii)const
{
	long i;

	if(ii<0)
    	return NULL;
	else if(ii>=NodeNum)
    	return NULL;
	else if(ii==0)
		return head->pData;
	else if(ii==NodeNum-1)
		return tail->pData;

	DATA_TYPE *tmp_cursor;
   	tmp_cursor = head;
	for( i=0;i<ii;i++)
	{
    	if(tmp_cursor->next!=NULL)
	    	tmp_cursor = tmp_cursor->next;
	}

	return tmp_cursor->pData;
}

long CLdsSuperList<class TYPE>::GetNodeNum(int CLASS_TYPEID/*=0*/)
{
	if(CLASS_TYPEID==0)
		return NodeNum;
	else
	{
		TYPE *pData=NULL;
		int count=0;
		int push=push_stack();
		for(pData=GetFirst();pData!=NULL;pData=GetNext())
		{
			if(GetClassTypeId(pData)==CLASS_TYPEID)
				count++;
		}
		pop_stack(push);
		return count;
	}
}

template<>TYPE* CLdsSuperList<class TYPE>::GetNext(int CLASS_TYPEID,BOOL bInterDelete/*=FALSE*/)
{
	while(cursor&&cursor->next!=NULL)
	{
    	index++;
		cursor = cursor->next;
		if((bInterDelete||!cursor->bDeleted)&&(GetClassTypeId(cursor->pData)==CLASS_TYPEID || CLASS_TYPEID==0))
			return cursor->pData;
	}
	return NULL;
}
template<>TYPE* CLdsSuperList<class TYPE>::GetPrev(int CLASS_TYPEID,BOOL bInterDelete/*=FALSE*/)
{
	while(cursor&&cursor->prev!=NULL)
	{
    	index--;
    	cursor = cursor->prev;
		if((bInterDelete||!cursor->bDeleted)&&(GetClassTypeId(cursor->pData)==CLASS_TYPEID || CLASS_TYPEID==0))
			return cursor->pData;
	}
		return NULL;
}
template<>TYPE*  CLdsSuperList<class TYPE>::GetFirst(int CLASS_TYPEID,BOOL bInterDelete/*=FALSE*/)
{
	if(head==NULL)
		return NULL;
	else
	{
		cursor = head;
		index = 0;
		while(cursor!=NULL){
			if((bInterDelete||!cursor->bDeleted)&&(GetClassTypeId(cursor->pData)==CLASS_TYPEID || CLASS_TYPEID==0))
				return cursor->pData;
			else
				cursor=cursor->next;
		}
		return NULL;
	}
}
template<>TYPE* CLdsSuperList<class TYPE>::GetTail(int CLASS_TYPEID,BOOL bInterDelete/*=FALSE*/)
{
	if(tail==NULL)
		return NULL;
	else
	{
		cursor = tail;
		index = NodeNum-1;
		while(cursor!=NULL){
			if((bInterDelete||!cursor->bDeleted)&&(GetClassTypeId(cursor->pData)==CLASS_TYPEID || CLASS_TYPEID==0))
				return cursor->pData;
			else
				cursor=cursor->prev;
		}
		return NULL;
	}
}

template<>TYPE* CLdsSuperList<class TYPE>::GetCursor()
{
	if(cursor)
		return cursor->pData;
	else
		return NULL;
}

// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
BOOL CLdsSuperList<class TYPE>::DeleteCursor(BOOL bClean/*=FALSE*/)
{
	if(cursor==NULL||cursor->bDeleted)
		return FALSE;
	if(bClean)
	{
		DATA_TYPE *cursor2=NULL;
		if(cursor!=head&&cursor!=tail)
		{
			cursor->prev->next=cursor->next;
			cursor->next->prev=cursor->prev;
			cursor2=cursor->prev;
		}
		else
		{
			if(cursor==head)
			{
				head=cursor->next;
				if(head)
					head->prev=NULL;
				cursor2=head;
			}
			if(cursor==tail)
			{
				tail=cursor->prev;
				if(tail)
					tail->next=NULL;
				cursor2=tail;
			}
		}
		//必须初始化DeleteData()
		DeleteData(cursor->pData);
		delete cursor;
		cursor=cursor2;
	}
	else
		cursor->bDeleted=TRUE;
	NodeNum--;
	if(NodeNum<0)
		NodeNum=0;
	return TRUE;
}
void CLdsSuperList<class TYPE>::Clean()
{
	for(cursor=head;cursor;)
	{
		DATA_TYPE *cursor2=cursor->next;
		if(cursor->bDeleted)
		{	//已被删除元素,移位后清除
			if(cursor!=head&&cursor!=tail)
			{
				cursor->prev->next=cursor->next;
				cursor->next->prev=cursor->prev;
			}
			else
			{
				if(cursor==head)
				{
					head=cursor->next;
					if(head)
						head->prev=NULL;
				}
				if(cursor==tail)
				{
					tail=cursor->prev;
					if(tail)
						tail->next=NULL;
				}
			}
			DeleteData(cursor->pData);
			delete cursor;
		}
		cursor=cursor2;
	}
}
BOOL CLdsSuperList<class TYPE>::DeleteAt(long ii)	// 删除一个节点
{
	int i=0;
	for(TYPE* pData=GetFirst();pData;pData=GetNext(),i++)
	{
		if(i==ii)
			return DeleteCursor(TRUE);
	}
	return FALSE;
}

void CLdsSuperList<class TYPE>::Empty(int CLASS_TYPEID)		// 清空当前类实例的所有元素
{
	POS_STACK.Empty();
	//GetTail();
	cursor=tail;
	while(cursor!=NULL)
	{
		if(GetClassTypeId(cursor->pData)==CLASS_TYPEID||CLASS_TYPEID==0)
		{
			DATA_TYPE *pDelete=cursor;
			cursor = cursor->prev;
			if(pDelete==tail)
				tail = cursor;
			if(pDelete==head)
				head = NULL;
			if(pDelete->next)
				pDelete->next->prev=cursor;
			if(cursor)
				cursor->next = pDelete->next;
			if(pDelete)
				DeleteData(pDelete->pData);
			delete pDelete;
			NodeNum--;
		}
		else
			cursor = cursor->prev;
	}
	index = 0;
	head = tail = NULL;
	cursor = NULL;
}
