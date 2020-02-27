#include "stdafx.h"
//#include "env_def.h"
#include "XhLdsLm.h"
#include "BriefWeldPart.h"
#include "XhCharString.h"
#include "Tower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IModel* WELDSONPART::globalModel=NULL;
static char _local_error_[64];
IMPLEMENT_GET(WELDSONPART, pRelaPart)
{
	if(_pRelaPart==NULL&&globalModel!=NULL)
	{
		BOOL pushed=globalModel->PushPartStack();
		_pRelaPart=globalModel->FromPartHandle(hPart);
		if(pushed)
			globalModel->PopPartStack(pushed);
	}
	if(_pRelaPart==NULL)
	{
		CXhString error(_local_error_,64);
		error.Printf("0x%X������ʧ��ָ������ʧ��!",hPart);
		throw _local_error_;
	}
	return _pRelaPart;
}
IMPLEMENT_GET(WELDSONPART, hPart){return _hPart;}
IMPLEMENT_SET(WELDSONPART, hPart)
{
	if(_hPart==value)
		return;
	_hPart=value;
	_pRelaPart=NULL;
}

WELDSONPART::WELDSONPART(CLDSPart* pPart/*=NULL*/)
{
	iFuncType=0;
	if(pPart)
	{
		_hPart=pPart->handle;
		_pRelaPart=pPart;
	}
	else
	{
		_hPart=0;
		_pRelaPart=NULL;
	}
}
WELDSONPART::WELDSONPART(long handle,CLDSPart* pPart/*=NULL*/)
{
	iFuncType=0;
	_hPart=handle;
	if(pPart&&pPart->handle==handle)
		_pRelaPart=pPart;
	else
		_pRelaPart=NULL;
}
IMPLEMENT_GET(CBriefWeldPart, hRootPart)
{
	return root.data.hPart;
}
IMPLEMENT_SET(CBriefWeldPart, hRootPart)
{
	root.data.hPart=value;
}
BYTE CBriefWeldPart::SetConstWeldType(BYTE ciConstWeldType)
{
	if(_ciWeldType==0&&ciConstWeldType>0&&ciConstWeldType<=WELD_TUBECROSSPLATES)
		_ciWeldType=ciConstWeldType;
	return _ciWeldType;
}
IMPLEMENT_GET(CBriefWeldPart, ciWeldType)
{
	if(_ciWeldType==0)
		IntelliRecogWeldType();
	return _ciWeldType;
}
CBriefWeldPart::CBriefWeldPart()
{
	current=&root;
	current_prev=NULL;
	_ciWeldType=0;
}
CBriefWeldPart::~CBriefWeldPart()
{
	DeleteList();
}
//ʶ���Ƿ�Ϊ��ϽǸ��б�׼ʮ��������Ϻ���
bool CBriefWeldPart::IdentifyStdXPlates()	
{
	const BYTE STDX_PLATES_BASEPLATE		= 0;//CLDSPlate* STDX_PLATES_PARTGROUP::m_pBasePlate;			//ͨ��
	const BYTE STDX_PLATES_BACKVERTPLATE	= 1;//CLDSPlate* STDX_PLATES_PARTGROUP::m_pBackVertPlate;		//��������
	const BYTE STDX_PLATES_FRONTVERTPLATE	= 2;//CLDSPlate* STDX_PLATES_PARTGROUP::m_pFrontVertPlate;		//��������
	CLDSPartGroup weldpart;
	weldpart.handle=hRootPart;
	STDX_PLATES_PARTGROUP params;
	if(!weldpart.IdentifyStdXPlates(&params))
		return false;
	//ILog2File* pLogFile=params.m_pBasePlate->Log2File();
	root.data.iFuncType=STDX_PLATES_BASEPLATE;
	root.data._pRelaPart=params.m_pBasePlate;
	current=&root;
	//��Ӻ������
	current=AssureNextNode();
	current->data.iFuncType=STDX_PLATES_BACKVERTPLATE;
	current->data.hPart=params.m_pBackVertPlate->handle;
	current->data._pRelaPart=params.m_pBackVertPlate;
	//���ǰ������
	current=AssureNextNode();
	current->data.iFuncType=STDX_PLATES_FRONTVERTPLATE;
	current->data.hPart=params.m_pFrontVertPlate->handle;
	current->data._pRelaPart=params.m_pFrontVertPlate;
	DeleteList(current,current->next);	//�Ƴ�֮����ຸ���ӹ�����һ�����Ҳ����Ҫ�Ƴ�
	return true;
}
//ʶ���Ƿ�Ϊ�Ǹ����Ű�
bool CBriefWeldPart::IdentifyAngleFoot()
{
	const char ANGLEFOOT_BASEPLATE		= 0;//ANGLE_FOOT_PARTGROUP::m_pFootPlate;		//������
	const char ANGLEFOOT_UP_SHOEPLATE	= 5;//ANGLE_FOOT_PARTGROUP::m_pShoePlate;		//�ϲ��ͨ(���)ѥ��
	const char ANGLEFOOT_UP_RIGHTPLATE	= 6;//ANGLE_FOOT_PARTGROUP::m_pRightShoePlate;	//�ϲ��Ҳ�ѥ��
	const char ANGLEFOOT_UP_BACKPLATE	= 7;//ANGLE_FOOT_PARTGROUP::m_pBackShoePlate;	//�ϲ���ѥ��
	const char ANGLEFOOT_UP_FRONTPLATE	= 8;//ANGLE_FOOT_PARTGROUP::m_pFrontShoePlate;	//�ϲ�ǰ��ѥ��
	const char ANGLEFOOT_DOWN_SHOEPLATE	= 1;//ANGLE_FOOT_PARTGROUP::m_pShoePlate;		//�²��ͨ(���)ѥ��
	const char ANGLEFOOT_DOWN_RIGHTPLATE= 2;//ANGLE_FOOT_PARTGROUP::m_pRightShoePlate;	//�²��Ҳ�ѥ��
	const char ANGLEFOOT_DOWN_BACKPLATE	= 3;//ANGLE_FOOT_PARTGROUP::m_pBackShoePlate;	//�²���ѥ��
	const char ANGLEFOOT_DOWN_FRONTPLATE= 4;//ANGLE_FOOT_PARTGROUP::m_pFrontShoePlate;	//�²�ǰ��ѥ��
	//ѥ���Ϻ��ӵ��߰��λ����ѥ�����ͬ����λ��ʰλ��Ϊ�߰������

	WELDSONPART* pSonPart;
	ANGLE_FOOT_PARTGROUP foot,footDown;	//foots[0]�ϲ�������Ϣ��foots[1]ţ����²�������Ϣ
	CLDSPartGroup weldpart;
	weldpart.handle=hRootPart;
	if(!weldpart.IdentifyAngleFoot(&foot))
		return false;
	bool ishoof=weldpart.IdentifyAngleFoot(&footDown,false,foot.m_pShoePlate->ucs.axis_z);	//�ж��Ƿ�Ϊţ���
	CXhSimpleList<WELDSONPART> listPlates;
	for(pSonPart=EnumObjectFirst();pSonPart;pSonPart=EnumObjectNext())
		listPlates.AttachObject(*pSonPart);
	//ILog2File* pLogFile=params.m_pBasePlate->Log2File();
	root.data.iFuncType=ANGLEFOOT_BASEPLATE;
	root.data._pRelaPart=foot.m_pFootPlate;
	current=&root;
	//��ֵѥ��������ֵΪ��Ӧѥ������(wItem����),��ѥ���ϵ�ǰ����߰����(index����)
	CHashList<KEY4C>hashShoeSerial;
	KEY4C baseRibSerial(10);	//���Ӹ���������ѥ���߰�
	if(footDown.m_pShoePlate)
	{	//����²��ͨ(���)ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_DOWN_SHOEPLATE;
		current->data.hPart=footDown.m_pShoePlate->handle;
		current->data._pRelaPart=footDown.m_pShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_DOWN_SHOEPLATE));
	}
	if(footDown.m_pRightShoePlate)
	{	//����²��Ҳ�ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_DOWN_RIGHTPLATE;
		current->data.hPart=footDown.m_pRightShoePlate->handle;
		current->data._pRelaPart=footDown.m_pRightShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_DOWN_RIGHTPLATE));
	}
	if(footDown.m_pBackShoePlate)
	{	//����²���ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_DOWN_BACKPLATE;
		current->data.hPart=footDown.m_pBackShoePlate->handle;
		current->data._pRelaPart=footDown.m_pBackShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_DOWN_BACKPLATE));
	}
	if(footDown.m_pFrontShoePlate)
	{	//����²�ǰ��ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_DOWN_FRONTPLATE;
		current->data.hPart=footDown.m_pFrontShoePlate->handle;
		current->data._pRelaPart=footDown.m_pFrontShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_DOWN_FRONTPLATE));
	}
	if(foot.m_pShoePlate)
	{	//����ϲ��ͨ(���)ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_UP_SHOEPLATE;
		current->data.hPart=foot.m_pShoePlate->handle;
		current->data._pRelaPart=foot.m_pShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_UP_SHOEPLATE));
	}
	if(foot.m_pRightShoePlate)
	{	//����ϲ��Ҳ�ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_UP_RIGHTPLATE;
		current->data.hPart=foot.m_pRightShoePlate->handle;
		current->data._pRelaPart=foot.m_pRightShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_UP_RIGHTPLATE));
	}
	if(foot.m_pBackShoePlate)
	{	//����ϲ���ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_UP_BACKPLATE;
		current->data.hPart=foot.m_pBackShoePlate->handle;
		current->data._pRelaPart=foot.m_pBackShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_UP_BACKPLATE));
	}
	if(foot.m_pFrontShoePlate)
	{	//����ϲ�ǰ��ѥ��
		current=AssureNextNode();
		current->data.iFuncType=ANGLEFOOT_UP_FRONTPLATE;
		current->data.hPart=foot.m_pFrontShoePlate->handle;
		current->data._pRelaPart=foot.m_pFrontShoePlate;
		hashShoeSerial.SetValue(current->data.hPart,KEY4C(ANGLEFOOT_UP_FRONTPLATE));
	}
	//���С�߰�
	for(pSonPart=listPlates.EnumObjectFirst();pSonPart;pSonPart=listPlates.EnumObjectNext())
	{
		KEY4C *pShoeSerial=hashShoeSerial.GetValue(pSonPart->hPart);
		if(pShoeSerial!=NULL||pSonPart->hPart==hRootPart)
			continue;	//֮ǰ�Ѵ���
		pShoeSerial=hashShoeSerial.GetValue(pSonPart->pRelaPart->m_hPartWeldParent);
		if(pShoeSerial==NULL)
			pShoeSerial=&baseRibSerial;
		current=AssureNextNode();
		current->data=*pSonPart;
		current->data.iFuncType=pShoeSerial->pair.wItem*100+pShoeSerial->pair.index;
		pShoeSerial->pair.index++;
	}
	DeleteList(current,current->next);	//�Ƴ�֮����ຸ���ӹ�����һ�����Ҳ����Ҫ�Ƴ�
	return true;
}
bool CBriefWeldPart::IdentifyTubeConnCoverPlates()
{
	return false;
}
bool CBriefWeldPart::IdentifyTubeCrossPlates()
{
	return false;
}
void CBriefWeldPart::IntelliRecogWeldType()
{
	if(root.data.pRelaPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)root.data.pRelaPart)->m_iParamType==TYPE_XEND)
		_ciWeldType=WELD_XENDPLATES;
	if(IdentifyStdXPlates())
		_ciWeldType=WELD_STDX_PLATES;
	else if(IdentifyAngleFoot())
		_ciWeldType=WELD_ANGLEFOOT;
	else if(IdentifyTubeConnCoverPlates())
		_ciWeldType=WELD_TUBECONNPLATES_INC_COVER;
	else if(IdentifyTubeCrossPlates())
		_ciWeldType=WELD_TUBECROSSPLATES;
	//else if(IdentifyBushingPlate())
	//	_ciWeldType=WELD_BUSHING;
	else	//��������ʶ��ʧ�ܣ�Ҳ������ʶ��
		_ciWeldType=WELD_COMMON;
}
bool PLACEINDEX::MoveHead(CHashListEx<PLACEINDEX>* pIndexes,UINT id)
{	//�����б�ͷ�ڵ�
	PLACEINDEX* pCurrIndex=pIndexes->GetValue(id);
	if(pCurrIndex==NULL)
		return false;
	UINT curr_index=pCurrIndex->index;
	for(PLACEINDEX* pIndex=pIndexes->GetFirst();pIndex;pIndex=pIndexes->GetNext())
	{
		if(pIndex->id==id)	//��ǰ�ڵ�������Ϊ0(��ͷ�ڵ�)
			pIndex->index=0;
		else if(pIndex->index<curr_index)
			pIndex->index++;	//ê��֮ǰ�ڵ���������
		//else if(pIndex->index>curr_index)//ê��֮��ڵ���������
	}
	return true;
}
bool PLACEINDEX::MoveTail(CHashListEx<PLACEINDEX>* pIndexes,UINT id)
{
	PLACEINDEX* pCurrIndex=pIndexes->GetValue(id);
	if(pCurrIndex==NULL)
		return false;
	UINT max_index=0;
	for(PLACEINDEX* pIndex=pIndexes->GetFirst();pIndex;pIndex=pIndexes->GetNext())
	{
		if(pIndex->index>max_index)
			max_index=pIndex->index;
		if(pIndex->index<pCurrIndex->index)
			continue;	//ê��֮ǰ�ڵ���������
		else if(pIndex->index>pCurrIndex->index)
			pIndex->index--;	//ê��֮��ڵ�����ǰ��
		else //if(pIndex->id==id)
			continue;	//��ǰ�ڵ������ݲ��裬����ʱ��Ϊ��⵽�����������
	}
	pCurrIndex->index=max_index;
	return true;
}
bool PLACEINDEX::MoveBefore(CHashListEx<PLACEINDEX>* pIndexes,UINT id,UINT before_id)
{
	PLACEINDEX* pAnchorIndex=pIndexes->GetValue(before_id);
	PLACEINDEX* pCurrIndex=pIndexes->GetValue(id);
	if(pCurrIndex==NULL)
		return false;
	if(before_id==0)
		return MoveHead(pIndexes,id);
	else if(before_id==0xffffffff)
		return MoveTail(pIndexes,id);
	else if(pAnchorIndex==NULL)
		return false;
	UINT curr_index=pCurrIndex->index;
	UINT anchor_index=pAnchorIndex->index;
	for(PLACEINDEX* pIndex=pIndexes->GetFirst();pIndex;pIndex=pIndexes->GetNext())
	{
		if(pIndex->id==id)
		{	//��ǰ���ƶ��ڵ�
			if(pIndex->index<anchor_index)
				pIndex->index=anchor_index-1;
			else if(pIndex->index>anchor_index)
				pIndex->index=anchor_index;	//ê��֮����ê��ǰ��
		}
		else if(pIndex->index<curr_index&&pIndex->index<anchor_index)
			continue;	//ͬʱ��ê�㼰�ƶ���֮ǰ�Ľڵ���������
		else if(pIndex->index>curr_index&&pIndex->index<anchor_index)
			pIndex->index--;	//�ƶ���֮��ê��֮ǰ����ǰ��
		else if(pIndex->index>anchor_index&&pIndex->index<curr_index)
			pIndex->index++;	//�ƶ���֮ǰ��ê��֮����������
		else if(pIndex->id==before_id&&anchor_index<curr_index)
			pIndex->index=anchor_index+1;	//ê�����ƶ���֮ǰ��ê�����
		//else	������������
	}
	return true;
}
bool PLACEINDEX::MoveAfter(CHashListEx<PLACEINDEX>* pIndexes,UINT id,UINT after_id)
{	//����ê��֮��
	PLACEINDEX* pAnchorIndex=pIndexes->GetValue(after_id);
	PLACEINDEX* pCurrIndex=pIndexes->GetValue(id);
	if(pCurrIndex==NULL)
		return false;
	if(after_id==0)
		return MoveHead(pIndexes,id);
	else if(after_id==0xffffffff)
		return MoveTail(pIndexes,id);
	else if(pAnchorIndex==NULL)
		return false;
	UINT curr_index=pCurrIndex->index;
	UINT anchor_index=pAnchorIndex->index;
	for(PLACEINDEX* pIndex=pIndexes->GetFirst();pIndex;pIndex=pIndexes->GetNext())
	{
		if(pIndex->id==id)
		{	//��ǰ���ƶ��ڵ�
			if(pIndex->index<anchor_index)
				pIndex->index=anchor_index;
			else if(pIndex->index>anchor_index)
				pIndex->index=anchor_index+1;	//ê��֮����ê��ǰ��
		}
		else if(pIndex->index<curr_index&&pIndex->index<anchor_index)
			continue;	//ͬʱ��ê�㼰�ƶ���֮ǰ�Ľڵ���������
		else if(pIndex->index>curr_index&&pIndex->index<anchor_index)
			pIndex->index--;	//�ƶ���֮��ê��֮ǰ����ǰ��
		else if(pIndex->index>anchor_index&&pIndex->index<curr_index)
			pIndex->index++;	//�ƶ���֮ǰ��ê��֮����������
		else if(pIndex->id==after_id&&anchor_index>curr_index)
			pIndex->index=anchor_index-1;	//ê�����ƶ���֮��ê��ǰ��
		//else	������������
	}
	return true;
}
