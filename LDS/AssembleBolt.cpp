#include "stdafx.h"
#include "Tower.h"
#include "EmendBoltGasket.h"
#include "SortFunc.h"
//#include "TowerModel.h"
//#include "CreatePartSolidBody.h"

//////////////////////////////////////////////////////////////////////////
//CBoltRelaPartList

CBoltRelaPartList::CBoltRelaPartList()
{
	m_idAssmBolt=m_idAnchorPart=0;
	m_pContext=NULL;
	m_cErrorType=0;
	//m_idFilterInstance=0;
}
IMPLEMENT_GET(CBoltRelaPartList,pExistGasket)
{
	BOLT_RELA_PART* pRelaPart=NULL;
	for(pRelaPart=EnumFirstRelaPart();pRelaPart;pRelaPart=EnumNextRelaPart())
	{
		if(pRelaPart->IsWasherPlate())
			return pRelaPart;
	}
	return NULL;
}
BOLT_RELA_PART* CBoltRelaPartList::AddRelaPart(DWORD idAssmPart,double zDepth,short siThick,UINT idInstance/*=0*/,BOOL bSort/*=TRUE*/)
{
	int iInsertIndex=-1,iCurIndex=0;
	BOLT_RELA_PART* pRelaPart=NULL;
	for(pRelaPart=EnumFirstRelaPart();pRelaPart;pRelaPart=EnumNextRelaPart())
	{
		if(pRelaPart->hPart==idAssmPart)
			break;
		if(bSort&&iInsertIndex==-1&&(pRelaPart->zDepth+pRelaPart->siThick*0.5)>(zDepth+siThick*0.5))
			iInsertIndex=iCurIndex;
		iCurIndex++;
	}
	if(pRelaPart==NULL)
	{
		LIST_NODE<BOLT_RELA_PART> *pNode=listRelaObjs.InsertNode(iInsertIndex);
		pRelaPart=&pNode->data;
		pRelaPart->hPart=idAssmPart;
	}
	pRelaPart->zDepth=zDepth;
	pRelaPart->siThick=siThick;
	if(idInstance)
		pRelaPart->cfgword.AddSpecNoWord(idInstance);
	else
	{
		for(int j=0;j<6;j++)
			pRelaPart->cfgword.flag.word[j]=0xffffffff;
	}
	return pRelaPart;
}
bool CBoltRelaPartList::RemoveRelaPart(DWORD idAssmPart,UINT idInstance/*=0*/)
{
	for(BOLT_RELA_PART* pRelaPart=EnumFirstRelaPart();pRelaPart;pRelaPart=EnumNextRelaPart())
	{
		if(pRelaPart->hPart!=idAssmPart)
			continue;
		if(idInstance>0)
			pRelaPart->cfgword.RemoveSpecNoWord(idInstance);
		else if(idInstance==0)
			pRelaPart->cfgword.Clear();
		if(pRelaPart->cfgword.IsNull())
			listRelaObjs.DeleteCursor();
		return true;
	}
	return false;
}
BOLT_RELA_PART* CBoltRelaPartList::EnumFirstRelaPart()
{
	BOLT_RELA_PART *pRelaPart=listRelaObjs.EnumObjectFirst();
	//while(m_idFilterInstance>0&&pRelaPart)
	//{
	//	if(pRelaPart->cfgword.IsHasNo(m_idFilterInstance))
	//		break;
	//	pRelaPart=listRelaObjs.EnumObjectNext();
	//}
	return pRelaPart;
}
BOLT_RELA_PART* CBoltRelaPartList::EnumNextRelaPart()
{
	BOLT_RELA_PART *pRelaPart=listRelaObjs.EnumObjectNext();
	//while(m_idFilterInstance>0&&pRelaPart)
	//{
	//	if(pRelaPart->cfgword.IsHasNo(m_idFilterInstance))
	//		break;
	//	pRelaPart=listRelaObjs.EnumObjectNext();
	//}
	return pRelaPart;
}
/*
DWORD CBoltRelaPartList::IntelliRecogAnchorAssmPart()	//������ѵ���˨êλ��׼����
{
	//����ָ���������:�˼��˵�Ϊ���ƽڵ�ĸ˼�1�ֽ� > �˼���λ��˨1�ֽ�(��λ�����ٷֱ�) & ����һ��һ����˨�еİ�(Ҫ����˨Ϊ�˶���˨)1�ֽ�
	CMinDouble minDist2Tip;	//��˨����ͷ��С����
	CMaxDouble maxDist2Tip;	//��˨����ͷ������
	CAssemblePlate *pAssmPlate=NULL;
	CAssembleAngle *pAssmAngle=NULL,*pAssmAngle1=NULL,*pAssmAngle2=NULL,*pAssmAngle3=NULL;
	CXhSimpleListStack<BOLT_RELA_PART> stack(&listRelaObjs);
	for(BOLT_RELA_PART* phRelaPart=EnumFirstRelaPart();phRelaPart;phRelaPart=EnumNextRelaPart())
	{
		CAssemblePart* pAssmPart=m_pModel->FromPartRecId(phRelaPart->idAssmPart);
		if(pAssmPart==NULL)
			continue;	//
		if(pAssmPart->IsRodRec())
		{
			CAssembleRod* pAssmRod=(CAssembleRod*)pAssmPart;
			if(pAssmRod->pStart==NULL||pAssmRod->pEnd==NULL)
				continue;	//�̽Ǹ�
			HOLE_REF* pHoleRef=pAssmRod->FindHoleRefByBoltRecId(m_idAssmBolt);
			if(pHoleRef==NULL)
				continue;
			float halfLength=pAssmRod->GetLength()*0.5f;
			if( (pHoleRef->LocalCoordX<halfLength&&pAssmRod->cStartCoordType>0)||
				(pHoleRef->LocalCoordX>halfLength&&pAssmRod->cEndCoordType>0))
				return pAssmPart->Id;	//�˼���ǰ���Ӳ�˵�Ϊ���ƽڵ�
			//����˼���λ��˨(��λ�����ٷֱ�)
			float index=halfLength-fabs(pHoleRef->LocalCoordX-halfLength);	//��˨���ͷ����
			minDist2Tip.Update(index,pAssmRod);
			maxDist2Tip.Update(index,pAssmRod);
			//pAssmAngle=NULL;
			//if(pAssmPart->IsAngleRec())
			//	pAssmAngle=(CAssembleAngle*)pAssmPart;
			if(pAssmPart->IsAngleRec()&&pAssmAngle1==NULL)
				pAssmAngle1=(CAssembleAngle*)pAssmPart;
			else if(pAssmPart->IsAngleRec()&&pAssmAngle2==NULL)
				pAssmAngle2=(CAssembleAngle*)pAssmPart;
			else if(pAssmPart->IsAngleRec()&&pAssmAngle3==NULL)
				pAssmAngle3=(CAssembleAngle*)pAssmPart;
		}
		else if(pAssmPlate==NULL&&pAssmPart->IsPlateRec())
		{
			CProcessPlate* pProcessPlate=(CProcessPlate*)pAssmPart->GetPart();
			if( pProcessPlate->m_cFuncType!=CProcessPlate::FUNC_COMMON&&
				pProcessPlate->m_cFuncType!=CProcessPlate::FUNC_FOOTPLATE)
				continue;
			pAssmPlate=(CAssemblePlate*)pAssmPart;
		}
	}
	if(pAssmAngle1!=NULL&&pAssmAngle2==NULL&&pAssmPlate!=NULL)
	{	//һ���Ǹ�һ�����Ӹְ�
		if(minDist2Tip.number<100)
			return pAssmPlate->Id;	//��˨�ڽǸֶ�ͷ,��ʱ�Ըְ�Ϊ��λ��׼
		else
			return pAssmAngle1->Id;	//��˨���ڽǸֶ�ͷʱ���ԽǸ�Ϊ��λ��׼ wht 17-04-10
	}
	else if(pAssmAngle2!=NULL&&pAssmAngle3==NULL&&pAssmPlate!=NULL)
	{
		GECS acs1=pAssmAngle1->GetAcs();
		GECS acs2=pAssmAngle2->GetAcs();
		if(fabs(acs1.axis_z*acs2.axis_z)>EPS_COS)
		{
			HOLE_REF* pHoleRef1=pAssmAngle1->FindHoleRefByBoltRecId(m_idAssmBolt);
			HOLE_REF* pHoleRef2=pAssmAngle2->FindHoleRefByBoltRecId(m_idAssmBolt);
			if(pHoleRef1->vHoleAxisVec*pHoleRef2->vHoleAxisVec<0)
				return pAssmPlate->Id;	//˫���Ǹֶ�ͷ�������ʱ���м�а�Ϊ��λ��׼(������ѥ���ϵ�Vб��)
		}
	}
	else if(maxDist2Tip.IsInited()&&maxDist2Tip.number>200)
		return ((CAssemblePart*)maxDist2Tip.m_pRelaObj)->Id;
	return 0;
}
IMPLEMENT_GET(CBoltRelaPartList,AnchorPartId)
{
	if(m_idAnchorPart==0)
		m_idAnchorPart=IntelliRecogAnchorAssmPart();
	return m_idAnchorPart;
}
IMPLEMENT_SET(CBoltRelaPartList,AnchorPartId)
{
	m_idAnchorPart=value;
}*/
CAssemblePart::CAssemblePart()
{
	m_hPart=0;
	m_pPart=NULL;
}
void CAssemblePart::SetContext(CEmendBoltGasket* pContainer,CLDSPart* pPart)
{
	m_pContext=pContainer;
	if(pPart!=NULL&&pPart->handle==m_hPart)
		m_pPart=pPart;
	else
		m_pPart=m_pContext->FromPartHandle(m_hPart);
}
BOOL CAssemblePart::DeleteHole(DWORD hBolt)
{
	return hashHoles.DeleteNode(hBolt);
}
CLDSPart::HOLE* CAssemblePart::GetBoltHole(DWORD hBolt)
{
	return hashHoles.GetValue(hBolt);
}
UINT CAssemblePart::StatHoles()
{
	return m_pPart->GetHoleSet(&hashHoles,true);
}
CAssemblePart* CEmendBoltGasket::GetAssemblePart(DWORD hPart)
{
	if(hashAssmParts.GetNodeNum()==0)
		UpdateBoltRelaObjs();
	CAssemblePart *pAssmPart=hashAssmParts.GetValue(hPart);
	//if(pAssmPart)
	//	pAssmPart->FilterInstanceId=0;
	return pAssmPart;
}
CAssemblePart* CEmendBoltGasket::AddAssemblePart(CLDSPart* pPart)
{
	if(pPart==0)
		return NULL;
	CAssemblePart* pAssmPart=hashAssmParts.Add(pPart->handle);
	if(pAssmPart->pSmartPart.IsNULL())
	{
		pAssmPart->SetContext(this,pPart);
		pAssmPart->StatHoles();
	}
	return pAssmPart;
}
CBoltRelaPartList* CEmendBoltGasket::GetBoltRelaParts(DWORD hBolt)
{
	if(hashBoltRelaObjs.GetNodeNum()==0)
		UpdateBoltRelaObjs();
	CBoltRelaPartList *pRelaPartList=hashBoltRelaObjs.GetValue(hBolt);
	//if(pRelaPartList)
	//	pRelaPartList->FilterInstanceId=0;
	return pRelaPartList;
}
CBoltRelaPartList* CEmendBoltGasket::AddBoltRelaParts(DWORD hBolt)
{
	if(hBolt==0)
		return NULL;
	CBoltRelaPartList* pList=hashBoltRelaObjs.Add(hBolt);
	pList->SetContext(this);
	return pList;
}
void CEmendBoltGasket::UpdateBoltHoles(DWORD* phAssmParts/*=NULL*/,UINT count/*=1*/)
{
	UINT i;
	PARTSET partset;
	SmartPartPtr pPart;
	if(phAssmParts!=NULL)
	{
		for(i=0;i<count;i++)
		{
			pPart=m_pModel->FromPartHandle(phAssmParts[i]);
			if(pPart.IsNULL()||pPart->GetLsRefList()==NULL)
				continue;	//����˨
			partset.append(pPart);
		}
	}
	else
	{
		hashAssmParts.Empty();
		for(pPart=m_pModel->EnumPartFirst();pPart.IsHasPtr();pPart=m_pModel->EnumPartNext())
		{
			if(pPart->GetLsRefList()==NULL)
				continue;
			partset.append(pPart);
		}
	}
	for(pPart=partset.GetFirst();pPart.IsHasPtr();pPart=partset.GetNext())
		this->AddAssemblePart(pPart);
}
void CEmendBoltGasket::UpdateBoltRelaObjs(DWORD* pAssmBoltIds/*=NULL*/,UINT count/*=0*/)
{
	CLDSBolt* pBolt;
	CHashSet<CLDSBolt*> assmbolts;
	if(pAssmBoltIds!=NULL)
	{	//����ָ����˨���ϵĹ������������Ϣ wjh-2017.12.3
		for(UINT i=0;i<count;i++)
		{
			pBolt=(CLDSBolt*)m_pModel->FromPartHandle(pAssmBoltIds[i],CLS_BOLT);
			if(pBolt==NULL)
				continue;
			assmbolts.SetValue(pBolt->handle,pBolt);
			AddBoltRelaParts(pBolt->handle);
		}
	}
	else //if(pAssmBoltIds==NULL)
	{	//����ȫ����˨�Ĺ������������Ϣ wjh-2017.12.3
		hashBoltRelaObjs.Empty();
		for (pBolt=(CLDSBolt*)m_pModel->EnumPartFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)m_pModel->EnumPartNext(CLS_BOLT))
		{
			assmbolts.SetValue(pBolt->handle,pBolt);
			AddBoltRelaParts(pBolt->handle);
		}
	}

	for (SmartPartPtr pPart=m_pModel->EnumPartFirst();pPart.IsHasPtr();pPart=m_pModel->EnumPartNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT)
			continue;
		short siThick=(short)pPart->GetThick();
		CAssemblePart* pAssmPart=this->AddAssemblePart(pPart);
		for(CLDSPart::HOLE* pHole=pAssmPart->EnumHoleRefFirst();pHole;pHole=pAssmPart->EnumHoleRefNext())
		{
			CBoltRelaPartList* pRelaList=AddBoltRelaParts(pHole->pBolt->handle);
			if(pRelaList)
			{
				GEPOINT xPosition=pHole->xMcsPosition;
				double zDepth=(xPosition-GEPOINT(pHole->pBolt->ucs.origin))*pHole->pBolt->ucs.axis_z;
				double ddz=pHole->vHoleAxisVec*pHole->pBolt->ucs.axis_z;
				if(fabs(ddz)<0.707)
				{
					continue;	//�׷�������˨����ƫ�������˨�����ܴ�����ǰ�� wht 17-07-05
				}
				int sign=((pPart->IsAngle()&&ddz>0)||(pPart->IsPlate()&&ddz<0))?-1:1;
				pRelaList->AddRelaPart(pPart->handle,zDepth,siThick*sign);
			}
		}
	}
}
CAssemblePart* CEmendBoltGasket::FromAssmPartHandle(long hPart,int idClassType/*=0*/)
{
	CAssemblePart* pAssmPart=this->hashAssmParts.GetValue(hPart);
	if(pAssmPart==NULL||(pAssmPart->pSmartPart->GetClassTypeId()!=idClassType&&idClassType>0))
		return NULL;
	return pAssmPart;
}
void CEmendBoltGasket::StatWireAngles()
{
	hashWireAngles.Empty();
	SmartPartPtr pBackAngle=NULL;
	CHashList<bool> hashCheckedAngles;
	for(SmartPartPtr pPart=m_pModel->EnumPartFirst();pPart.IsHasPtr();pPart=m_pModel->EnumPartNext())
	{
		if(!pPart->IsAngle())
			continue;
		if(hashCheckedAngles.GetValue(pPart->handle)!=NULL)
			continue;	//�Ѵ����
		CLsRef* pHoleRef;	//CLDSPart::HOLE
		long idXBackAngleRec=0,idYBackAngleRec=0;
		bool IsXWireAngle=true,IsYWireAngle=true;
		BYTE ciWireWing=0,ciBackWireWing=0;	//'X':X֫Ϊ�ҿ�֫;'Y':Y֫Ϊ�ҿ�֫
		for(pHoleRef=pPart.pAngle->GetFirstLsRef();pHoleRef;pHoleRef=pPart.pAngle->GetNextLsRef())
		{
			CBoltRelaPartList* pRelaList=GetBoltRelaParts(pHoleRef->pBolt->handle);
			if(pRelaList==NULL)
				continue;
			char ciWingX0Y1='X';
			if(fabs(pPart.pAngle->vxWingNorm*pHoleRef->pBolt->ucs.axis_z)>fabs(pPart.pAngle->vyWingNorm*pHoleRef->pBolt->ucs.axis_z))
				ciWingX0Y1='X';
			else
				ciWingX0Y1='Y';
			for(BOLT_RELA_PART* pRelaPart=pRelaList->EnumFirstRelaPart();(IsXWireAngle||IsYWireAngle)&&pRelaPart;pRelaPart=pRelaList->EnumNextRelaPart())
			{
				if(pRelaPart->hPart==pPart->handle)
					continue;
				//���߽ǸֵĹ����ǣ����߿׽�������������ƽ���ұ������ĽǸ� wjh-2017.12.20
				if(ciWingX0Y1=='X')	//X֫
				{
					if(idXBackAngleRec==0)
						idXBackAngleRec=pRelaPart->hPart;
					else if(idXBackAngleRec!=pRelaPart->hPart)
						IsXWireAngle=false;	//���������ʾ���ǹ��߽Ǹ�
				}
				else
				{
					if(idYBackAngleRec==0)
						idYBackAngleRec=pRelaPart->hPart;
					else if(idYBackAngleRec!=pRelaPart->hPart)
						IsYWireAngle=false;
				}
			}
			if(!IsXWireAngle&&!IsYWireAngle)
				break;
		}
		CLDSLineAngle *pBackAssmAngle=NULL;
		if(idXBackAngleRec>0&&IsXWireAngle)
		{
			pBackAssmAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(idXBackAngleRec,CLS_LINEANGLE);
			ciWireWing='X';
		}
		else if(idYBackAngleRec>0&&IsYWireAngle)
		{
			pBackAssmAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(idYBackAngleRec,CLS_LINEANGLE);
			ciWireWing='Y';
		}
		if(pBackAssmAngle==NULL)
			continue;
		BOOL IsBackXWireAngle=true,IsBackYWireAngle=true;
		for(pHoleRef=pBackAssmAngle->GetFirstLsRef();pHoleRef;pHoleRef=pBackAssmAngle->GetNextLsRef())
		{
			CBoltRelaPartList* pRelaList=GetBoltRelaParts(pHoleRef->pBolt->handle);
			if(pRelaList==NULL)
				continue;
			if(fabs(pBackAssmAngle->vxWingNorm*pHoleRef->pBolt->ucs.axis_z)>fabs(pBackAssmAngle->vyWingNorm*pHoleRef->pBolt->ucs.axis_z))
				ciBackWireWing='X';
			else
				ciBackWireWing='Y';
			for(BOLT_RELA_PART* pRelaPart=pRelaList->EnumFirstRelaPart();(IsBackXWireAngle||IsBackYWireAngle)&&pRelaPart;pRelaPart=pRelaList->EnumNextRelaPart())
			{
				if(pRelaPart->hPart==pBackAssmAngle->handle)
					continue;
				if(ciBackWireWing=='X'&&pRelaPart->hPart!=pPart->handle)
					IsBackXWireAngle=false;
				else if(ciBackWireWing=='Y'&&pRelaPart->hPart!=pPart->handle)
					IsBackYWireAngle=false;
			}
			if(!IsBackXWireAngle&&!IsBackYWireAngle)
				break;
		}
		if(pBackAssmAngle)
			hashCheckedAngles.Add(pBackAssmAngle->handle);
		GEPOINT lenStdVec =pPart.pAngle->End()-pPart.pAngle->Start();
		GEPOINT lenStdVec2=pBackAssmAngle->End()-pBackAssmAngle->Start();
		normalize(lenStdVec);
		normalize(lenStdVec2);
		if( fabs(lenStdVec*lenStdVec2)>EPS_COS2&&
			(ciWireWing=='X'||ciWireWing=='Y')&&(ciBackWireWing=='X'||ciBackWireWing=='Y'))
		{
			hashWireAngles.SetValue(pPart->handle,WIRE_ANGLE(pPart->handle,ciWireWing));
			hashWireAngles.SetValue(idXBackAngleRec,WIRE_ANGLE(idXBackAngleRec,ciBackWireWing));
		}
	}
}
BYTE CEmendBoltGasket::GetAngleWireWing(long idAssmAngle)//0:�ǹ��߽Ǹ�;'X':X֫Ϊ����֫;'Y':Y֫Ϊ����֫
{
	WIRE_ANGLE* pWireAngle=hashWireAngles.GetValue(idAssmAngle);
	if(pWireAngle!=NULL)
		return pWireAngle->ciWireWing;
	else
		return 0;
}
//�Ƿ�Ϊ����ף����Ŷ��������һ����������˨��������ϽǸ��ϵĹ��߿״������Ǹ֣�
bool CBoltRelaPartList::IsVirtualHole()
{
	if(m_pContext==NULL)
		return false;
	int count=0;
	BOLT_RELA_PART* phRelaPart,arrRelaPartId[2];
	for(phRelaPart=EnumFirstRelaPart();count<2&&phRelaPart;phRelaPart=EnumNextRelaPart(),count++)
		arrRelaPartId[count]=*phRelaPart;
	if(count>2)
		return false;	//����������˨��Խ������һ��������������˨��
	if(count==0)
	{
		//logerr.Log("error");
		return false;
	}
	if(count==1)
	{
		CLDSBolt* pAssmBolt=(CLDSBolt*)m_pContext->FromPartHandle(m_idAssmBolt,CLS_BOLT);
		if(pAssmBolt&&pAssmBolt->IsFootNail())
			return false;	//��˨Ϊ�Ŷ�����
		return true;
	}
	else if(count==2)
	{	//�жϹ��߽Ǹ�
		CLDSPart* pAssmPart1=m_pContext->FromPartHandle(arrRelaPartId[0].hPart);
		CLDSPart* pAssmPart2=m_pContext->FromPartHandle(arrRelaPartId[1].hPart);
		if(pAssmPart1==NULL||pAssmPart2==NULL||!pAssmPart1->IsAngle()||!pAssmPart2->IsAngle())
			return false;
		BYTE ciAngleWireWing1=m_pContext->GetAngleWireWing(pAssmPart1->handle);
		BYTE ciAngleWireWing2=m_pContext->GetAngleWireWing(pAssmPart2->handle);
		if( ciAngleWireWing1==0||ciAngleWireWing2==0)
			return false;
		//�ж��������Ǹ��Ƿ�Ϊ�������Ĺ��߽Ǹ�
		//if(pAssmPart1->assemblyPos)
		CLDSLineAngle* pProcessAngle=(CLDSLineAngle*)pAssmPart1;
		CLsRef* pHoleRef=pProcessAngle->FindLsByHandle(m_idAssmBolt);
		if(pHoleRef==NULL)
		{
			//logerr.Log()
			return false;
		}
		CLDSBolt* pBolt=pHoleRef->pBolt;
		double cosax=fabs(pBolt->ucs.axis_z*pProcessAngle->vxWingNorm);
		double cosay=fabs(pBolt->ucs.axis_z*pProcessAngle->vyWingNorm);
		if(cosax>cosay&&ciAngleWireWing1=='X')
			return true;
		if(cosax>cosay&&ciAngleWireWing1=='Y')
			return true;
		/**Ӧ��ֻ����һ�����߽ǸֵĹ���֫�жϼ��ɡ�wjh-2017.3.21
		pHoleRef=pAssmPart2->FindHoleRefByBoltRecId(idAssmBolt);
		pProcessAngle=(CProcessPart*)pAssmPart2->GetPart();
		pBoltInfo=pProcessAngle->FromHoleId(pHoleRef->holeId);
		if(pBoltInfo->posY<0&&pWireAngle2->ciWireWing!='X')
			return false;
		if(pBoltInfo->posY>0&&pWireAngle2->ciWireWing!='Y')
			return false;
		*/
		return false;
	}
	return false;
}

double CBoltRelaPartList::GetMaxSpaceAndSumThroughThick(WORD *pwSumThick/*=NULL*/,
														BOLT_RELA_PART **ppBtmRelaPart1/*=NULL*/,
														BOLT_RELA_PART **ppTopRelaPart2/*=NULL*/,
														BOLT_RELA_PART **ppGasketRelaPart/*=NULL*/,
														BOOL bIncGasketThick/*=TRUE*/)
{
	WORD wSumThick=0;
	double fMaxSpace=0;
	BOLT_RELA_PART *pPrevRelaPart=NULL,*pRelaPart1=NULL,*pRelaPart2=NULL,*pGasketRelaPart=NULL;
	for(BOLT_RELA_PART *pRelaPart=EnumFirstRelaPart();pRelaPart;pRelaPart=EnumNextRelaPart())
	{
		if(!bIncGasketThick)
		{
			SmartPartPtr pPart=m_pContext->FromPartHandle(pRelaPart->hPart);
			if(pPart.IsHasPtr()&&pPart->IsPlate()&&pPart.pBasicPlate->wiType==CLDSPlate::WASHER_PLATE)
			{	//������岻����
				pGasketRelaPart=pRelaPart;
				continue;
			}
		}
		if(pPrevRelaPart)
		{
			double zPrevDepth=pPrevRelaPart->siThick>0?pPrevRelaPart->zDepth+pPrevRelaPart->siThick:pPrevRelaPart->zDepth;
			double zCurrDepth=pRelaPart->siThick<0?pRelaPart->zDepth+pRelaPart->siThick:pRelaPart->zDepth;
			double fSpace=zCurrDepth-zPrevDepth;
			if(fSpace>fMaxSpace)
			{
				pRelaPart1=pPrevRelaPart;
				pRelaPart2=pRelaPart;
				fMaxSpace=fSpace;
			}
		}
		wSumThick+=abs(pRelaPart->siThick);
		pPrevRelaPart=pRelaPart;
	}
	if(bIncGasketThick)
	{
		CLDSBolt *pBolt=(CLDSBolt*)m_pContext->FromPartHandle(m_idAssmBolt,CLS_BOLT);
		if(pBolt)
		{
			WORD wDianQuanThick=pBolt->DianQuan.N*pBolt->DianQuan.thick;//GetDianQuanThick();
			if(wDianQuanThick>0)
				fMaxSpace-=wDianQuanThick;
		}
	}
	if(pwSumThick)
		*pwSumThick=wSumThick;
	if(ppBtmRelaPart1)
		*ppBtmRelaPart1=pRelaPart1;
	if(ppTopRelaPart2)
		*ppTopRelaPart2=pRelaPart2;
	if(ppGasketRelaPart)
		*ppGasketRelaPart=pGasketRelaPart;
	return fMaxSpace;
}
bool CBoltRelaPartList::InitializeBoltGasket(CBoltGasket* pBoltGasket)
{
	if(listRelaObjs.Count<=0)
		return false;
	BOLT_RELA_PART* pDatumRelaPart=NULL,*pBtmRelaPart=NULL,*pTopRelaPart=NULL,*pGasketRelaPart=NULL;
	for(BOLT_RELA_PART* pRelaPart=listRelaObjs.EnumObjectFirst();pRelaPart;pRelaPart=listRelaObjs.EnumObjectNext())
	{
		if(pBoltGasket->pBolt->des_base_pos.hPart==pRelaPart->hPart)
		{
			pDatumRelaPart=pRelaPart;
			break;
		}
	}
	if(pDatumRelaPart==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("It can't find the datum part by the bolt's reference",pBoltGasket->pBolt->handle);
#else 
		logerr.Log("���ݴ���˨����0X%X�Ҳ�����׼����",pBoltGasket->pBolt->handle);
#endif
		return false;
	}
	WORD wSumThroughThick=0;
	double maxSpace=GetMaxSpaceAndSumThroughThick(&wSumThroughThick,&pBtmRelaPart,&pTopRelaPart,NULL,false);
	if(pBtmRelaPart==NULL||pTopRelaPart==NULL||fabs(maxSpace)<CEmendBoltGasket::MINSPACING)
		return false;
	pBoltGasket->hBtmGasketRelaPart=pBtmRelaPart->hPart;
	pBoltGasket->hTopGasketRelaPart=pTopRelaPart->hPart;
	pBoltGasket->gasketThick = maxSpace;
	pBoltGasket->gasketOffset=pBtmRelaPart->zDepth-pDatumRelaPart->zDepth;
	if(pBtmRelaPart->siThick>0)
		pBoltGasket->gasketOffset+=pBtmRelaPart->siThick;
	CAssemblePart* pDatumAssmPart=m_pContext->FromAssmPartHandle(pDatumRelaPart->hPart);
	CLDSPart::HOLE* pDatumHole=pDatumAssmPart->GetBoltHole(pBoltGasket->pBolt->handle);
	//�� typeӦ����ΪDIANQUAN::�µľ�̬���� 
	if(CLDSBolt::CalPadPara(pBoltGasket->pBolt->get_d(),maxSpace)==DIANQUAN_STRU::DIANQUAN) //��Ȧ:ƫ��������˨����,��Ȧ���ߺ���˨����һ��
		pBoltGasket->washerOffset=pBtmRelaPart->zDepth+max(0,pBtmRelaPart->siThick);
	else if(pDatumHole->vHoleAxisVec*pBoltGasket->pBolt->ucs.axis_z<0)
	{	//��壺ƫ�����Ի�׼��������,��Ƭ�������׼��������һ��
		//TODO: δ�����巨�����׼�������߲�һ�����
		pBoltGasket->gasketOffset*=-1;
		//if(pDatumRelaPart->siThick<0
	}
	return true;
}
bool CBoltRelaPartList::IsThroughGroupAngle()
{
	int nSumCount=GetRelaPartCount();
	if(nSumCount<=0)
		return false;
	int nCount=0;
	DYN_ARRAY<CLDSLineAngle*> angleRecArr(nSumCount);
	CXhSimpleListStack<BOLT_RELA_PART> stack(&listRelaObjs);
	for(BOLT_RELA_PART* phRelaPart=EnumFirstRelaPart();phRelaPart;phRelaPart=EnumNextRelaPart())
	{
		CLDSLineAngle* pAssmAngle=(CLDSLineAngle*)m_pContext->FromPartHandle(phRelaPart->hPart,CLS_LINEANGLE);
		if(pAssmAngle==NULL)
			continue;	//
		if(pAssmAngle->pStart==NULL||pAssmAngle->pEnd==NULL)
			continue;	//�̽Ǹ�
		int i=0;
		for(i=0;i<nCount;i++)
		{
			if( pAssmAngle->pStart->xPreliftPos==angleRecArr[i]->pStart->xPreliftPos&&
				pAssmAngle->pEnd->xPreliftPos==angleRecArr[i]->pEnd->xPreliftPos)
			{
				angleRecArr[i]->feature=(long)pAssmAngle;
				break;
			}
		}
		if(i==nCount)
		{
			pAssmAngle->feature=0;
			angleRecArr[nCount]=pAssmAngle;
			nCount++;
		}
	}
	for(int i=0;i<nCount;i++)
	{
		if(angleRecArr[i]->feature!=0)
			return true;
	}
	return false;
}
int CBoltRelaPartList::GetRelaPartCount(bool bIncExistGasket/*=true*/)
{
	int nCount=0;
	CXhSimpleListStack<BOLT_RELA_PART> stack(&listRelaObjs);
	for(BOLT_RELA_PART* pRelaPart=EnumFirstRelaPart();pRelaPart;pRelaPart=EnumNextRelaPart())
	{
		if(!bIncExistGasket&&pRelaPart->IsWasherPlate())
			continue;
		nCount++;
	}
	return nCount;
}

//////////////////////////////////////////////////////////////////////////
const int SPACE_MAX_THRESHOLD = 50;		//���������϶ֵ�����ڴ�ֵ��Ϊ���쳣
const int SPACE_MIN_THRESHOLD=2;		//��С�������϶ֵ��С�ڴ�ֵ��Ϊ����Ҫ���
const int SPACE_MIN_END_THRESHOLD=2;	//��ͷ�����С����϶ֵ��С�ڴ�ֵ��Ϊ����Ҫ���
const int SPACE_MIN_CROSS_THRESHOLD=5;	//���������С����϶ֵ��С�ڴ�ֵ��Ϊ����Ҫ���
const int MAX_BOLT_DISTANCE=400;		//ͬ��������˨�������࣬���ڴ�ֵ��Ϊ������ͬ������˨
//������˨�ڵ�ǰ�Ǹ��ϵĹ�����˨
static CLDSPart::HOLE* GetNearestEndLjRelaHole(CAssemblePart *pAssmPart,CLDSBolt* pBolt,char* pchOnSide=NULL)//DWORD idInstance)
{
	if(pAssmPart==NULL||pAssmPart->pSmartPart.IsNULL()||!pAssmPart->pSmartPart->IsAngle())
		return NULL;
	CLDSPart::HOLE *pCurHole=pAssmPart->GetBoltHole(pBolt->handle);
	CBoltRelaPartList *pCurRelaPartList=pAssmPart->pContext->GetBoltRelaParts(pBolt->handle);
	if(pCurHole==NULL||pCurRelaPartList==NULL)
		return NULL;
	//1.��ȡ��ǰ֫�������������
	CLDSPart::HOLE *pHole=NULL;
	ARRAY_LIST<CLDSPart::HOLE*> holeArr;
	for(pHole=pAssmPart->EnumHoleRefFirst();pHole;pHole=pAssmPart->EnumHoleRefNext())
	{
		if(pCurHole->xLocalPosition.y*pHole->xLocalPosition.y<0)
			continue;	//�뵱ǰ��˨λ�ڲ�ͬ�Ǹ�֫
		for(int i=0;i<holeArr.GetSize();i++)
		{
			if(pHole->xLocalPosition.x>holeArr[i]->xLocalPosition.x)
				continue;//λ�ڵ�i����˨���Ҳ࣬����������һ����˨
			holeArr.insert(pHole,i);
			break;
		}
		if(i==holeArr.GetSize())	//�ÿ�λ�ڵ�ǰȫ��������˨�׵��Ҳ࣬���뵽��ĩβ
			holeArr.insert(pHole,-1);
	}
	//2.�ӵ�ǰ����ǰ\�����ҹ�����˨
	char cSide=0;
	int i=0,iCurIndex=-1;
	CLDSPart::HOLE* pPrevHole=holeArr[0];
	bool matchedCurHole=(pPrevHole==pCurHole);
	if(matchedCurHole)
	{
		iCurIndex=0;
		cSide='S';	//ʼ����˨��
	}
	else
	{
		bool mayOnStart=true,mayOnEnd=true;
		for(i=1;i<holeArr.GetSize();i++)
		{
			if(holeArr[i]->xLocalPosition.x-pPrevHole->xLocalPosition.x>MAX_BOLT_DISTANCE)
			{	//�����Զ������ͬ��������˨
				if(!matchedCurHole)	//δƥ�䵽��ǰ��˨ʱ�ѷ�����˨���жϣ�����������ʼ����˨��
					mayOnStart=false;
				else if(matchedCurHole)
					mayOnEnd=false;	//��ƥ�䵽��ǰ��˨֮������˨���жϣ������������ն���˨��
				if(!mayOnStart&&!mayOnEnd)
					break;			//��˨ֻ�����м佻�����˨
			}
			if(holeArr[i]==pCurHole)
			{
				iCurIndex=i;
				matchedCurHole=true;
				if(mayOnStart)
				{
					cSide='S';
					break;
				}
			}
			pPrevHole=holeArr[i];
		}
		if(cSide==0)
			cSide=mayOnEnd?'E':'M';
	}
	if(pchOnSide)
		*pchOnSide=cSide;
	if(cSide!='S'&&cSide!='E')
		return NULL;	//�м佻����˨�������ڶ�ͷ��˨��
	CLDSPart::HOLE *pStartHoleRef=pCurHole,*pEndHoleRef=NULL;
	//if(pCurRelaPartList)
	//	pCurRelaPartList->FilterInstanceId=idInstance;
	//2 ��ǰ������ͬ�������������˨
	i=(cSide=='S')?iCurIndex-1:iCurIndex+1;
	for(;cSide=='S'?i>=0:i<holeArr.GetSize();i=(cSide=='S')?i-1:i+1)
	{
		CBoltRelaPartList *pRelaPartList=pAssmPart->pContext->GetBoltRelaParts(holeArr[i]->pBolt->handle);
		if(pRelaPartList==NULL)
			continue;
		if(pRelaPartList->GetRelaPartCount(false)>pCurRelaPartList->GetRelaPartCount(false))
			return holeArr[i];	//������˨�����������������仯
	}
	return NULL;
}

BYTE CEmendBoltGasket::JustifyErrorEndPosAngle(CAssemblePart* pAssmPart,CBoltGasket* pGasket)
{
	if(!pAssmPart->pSmartPart->IsAngle())
		return 0;
	CLDSPart::HOLE *pHole,*pCurrHole=pAssmPart->GetBoltHole(pGasket->pBolt->handle);
	if(pCurrHole)
	{
		double length=pAssmPart->pSmartPart.pAngle->GetLength();
		double halflen=length/2;
		bool bStartSide=false,bEndSide=false;
		if(pCurrHole->xLocalPosition.x<min(300,halflen))	//������ʼ��������˨,300mmΪ����ֵ
			bStartSide=true;
		else
			bEndSide=pCurrHole->xLocalPosition.x>length-min(300,halflen);
		{
			for(pHole=pAssmPart->EnumHoleRefFirst();pHole;pHole=pAssmPart->EnumHoleRefNext())
			{
				if(bStartSide&&pHole->xLocalPosition.x<pCurrHole->xLocalPosition.x)
					break;
				else if(bEndSide&&pHole->xLocalPosition.x>pCurrHole->xLocalPosition.x)
					break;
			}
			CBoltGasket* pEndGasket=pHole!=NULL?hashBoltGaskets.GetValue(pHole->pBolt->handle):NULL;
			if(pEndGasket)
			{
				BYTE ciError=ValidateTheroyGasket(pEndGasket);
				if(ciError==0)
					return CBoltRelaPartList::TYPE_MISS_CROSSED_PAD;//3.	//�����λ���ٵ�Ƭ
				else
					return 0;
			}
			else
				return CBoltRelaPartList::TYPE_PART_POS_ERROR;
		}
	}
	else
		return 0;	//log("error");
}
//0.�޶��ɹ�
//1.�޹������� 
//2.�ǽŶ���˨������һ������
//3.����㴦�ٵ�Ƭ
//4.��ͷ��Ҫ�����˨��Ƭ
//5.��϶ֵ�����Ҹ�λ�ò���Ҫ��Ƭ
//6.�����Ȧ��Ȳ���ȷ
BYTE CEmendBoltGasket::ValidateTheroyGasket(CBoltGasket* pGasket,long* phErrPosAngle/*=NULL*/)
{
	//DWORD idInstance=pInstance?pInstance->Id:0;
	CLDSBolt* pBolt=pGasket->pBolt;
	CBoltRelaPartList *pBoltRelaPartList=GetBoltRelaParts(pBolt->handle);
	if(pBoltRelaPartList==NULL)
		return CBoltRelaPartList::TYPE_NONE_CONNECTED;//return 1;
	//pBoltRelaPartList->FilterInstanceId=idInstance;
	pBoltRelaPartList->m_cErrorType=0;
	BOLT_RELA_PART *pRelaPart,*pFirstRelaPart=pBoltRelaPartList->EnumFirstRelaPart();
	//1.������˨ͨ��
	if(pBoltRelaPartList->IsVirtualHole())
		return CBoltRelaPartList::TYPE_VIRTUAL_BOLT;	//return 2;
	//pGasket->wiCurrGasketThick��Ҫʵʱʶ����㣬����ȡ���ݴ�Ϊ��ʱ����
	WORD wiCurrGasketThick=pGasket->wiCurrGasketThick;
	if(pBoltRelaPartList->IsThroughGroupAngle()&&pBoltRelaPartList->GetRelaPartCount()>2)
	{	//������ϽǸ�ʱ��ֱ�Ӱ�����������ȼ���ͨ����ȣ�����ͨ������Ϊ�趨��壬��ʱ���������
		//pBoltRelaPartList->GetMaxSpaceAndSumThroughThick(&wSumThick);
		return pBoltRelaPartList->m_cErrorType=0;	//������ϽǸ�
	}
	else if(pGasket->gasketThick<SPACE_MIN_THRESHOLD)
	{
		if(pGasket->wiCurrDianQuanThick>0||wiCurrGasketThick>0)//TODO:�˴�����Ӧ����һ�����Ĵ����ʶ�� wjh-2017.12.7
			return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_ERROR_GASKET_THICK;
		return pBoltRelaPartList->m_cErrorType=0;	//����Ҫ��Ȧ���
	}
	else if(pGasket->gasketThick>=SPACE_MAX_THRESHOLD)	//5.��϶ֵ�����Ҹ�λ�ò���Ҫ��Ƭ
	{
		//if(pOtherRelaPart1&&pOtherRelaPart2&&(
		//	(pOtherRelaPart1->hPart==pAssmPart1->hPart&&pOtherRelaPart2->hPart==pAssmPart2->hPart)||
		//	(pOtherRelaPart1->hPart==pAssmPart2->hPart&&pOtherRelaPart2->hPart==pAssmPart1->hPart)))
		if(phErrPosAngle)
		{
			CAssemblePart *pAssmAngle;
			if((pAssmAngle=FromAssmPartHandle(pGasket->hBtmGasketRelaPart,CLS_LINEANGLE))!=NULL)
				*phErrPosAngle=pAssmAngle->hPart;
			else if((pAssmAngle=FromAssmPartHandle(pGasket->hTopGasketRelaPart,CLS_LINEANGLE))!=NULL)
				*phErrPosAngle=pAssmAngle->hPart;
		}
		return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_PART_POS_ERROR;
	}
	else //if(pGasket->gasketThick>=SPACE_MIN_THRESHOLD&&pGasket->gasketThick<=SPACE_MAX_THRESHOLD)
	{
		bool bWireHoleVirtualBolt=false;
		for(BOLT_RELA_PART* pRelaPart=pBoltRelaPartList->EnumFirstRelaPart();!bWireHoleVirtualBolt&&pRelaPart;
			pRelaPart=pBoltRelaPartList->EnumNextRelaPart())
		{
			if(pRelaPart->pPart&&pRelaPart->pPart->IsAngle())
			{
				CAssemblePart* pAssmAngle=FromAssmPartHandle(pRelaPart->hPart);
				CLDSPart::HOLE* pHole=pAssmAngle!=NULL?pAssmAngle->GetBoltHole(pGasket->pBolt->handle):NULL;
				if(pHole==NULL)
					continue;
				WIRE_ANGLE* pWireAngle=hashWireAngles.GetValue(pRelaPart->hPart);
				if(pWireAngle&&pWireAngle->ciWireWing==pHole->ciResidePlane+'X')
					bWireHoleVirtualBolt=true;
			}
		}
		if(bWireHoleVirtualBolt)
			return CBoltRelaPartList::TYPE_WIRE_VIRTUAL_BOLT;	//return 7;
	}
	char chOnSide=0;
	CAssemblePart *pBtmAssmPart=FromAssmPartHandle(pGasket->hBtmGasketRelaPart);
	CAssemblePart *pTopAssmPart=FromAssmPartHandle(pGasket->hTopGasketRelaPart);
	//������Ҫ��ӵ��
	if( pBtmAssmPart&&pTopAssmPart&&
		((pBtmAssmPart->pSmartPart->IsAngle()&&pTopAssmPart->pSmartPart->IsPlate())||
		 (pBtmAssmPart->pSmartPart->IsPlate()&&pTopAssmPart->pSmartPart->IsAngle())))
	{	//����Ϊ�����ӽǸ�
		CAssemblePart *pAssmAngle=(pBtmAssmPart->pSmartPart->IsAngle())?pBtmAssmPart:pTopAssmPart;
		CLDSPart::HOLE *pEndHole=GetNearestEndLjRelaHole(pAssmAngle,pBolt,&chOnSide);//idInstance);
		CBoltRelaPartList *pEndRelaPartList=pEndHole!=NULL?GetBoltRelaParts(pEndHole?pEndHole->pBolt->handle:0):NULL;
		if(chOnSide=='M')//pEndHole==NULL)
		{
			//if(pBoltRelaPartList->GetRelaPartCount()==2)
				return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_MISS_CROSSED_PAD;//3;	//�����λ���ٵ�Ƭ
		}
		else if(pEndHole==NULL)
		{
			if(phErrPosAngle&&pBtmAssmPart->pSmartPart->IsAngle())
				*phErrPosAngle=pBtmAssmPart->hPart;
			else if(phErrPosAngle&&pTopAssmPart->pSmartPart->IsAngle())
				*phErrPosAngle=pTopAssmPart->hPart;
			return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_PART_POS_ERROR;
		}
		else if(pEndRelaPartList)
		{
			//pEndRelaPartList->FilterInstanceId=idInstance;
			WORD wOtherSumThick=0;
			BOLT_RELA_PART *pOtherRelaPart1=NULL,*pOtherRelaPart2=NULL;
			//double fOtherMaxSpace=pEndRelaPartList->GetMaxSpaceAndSumThroughThick(&wOtherSumThick,&pOtherRelaPart1,&pOtherRelaPart2);
			if(pBoltRelaPartList->pExistGasket!=NULL)
			{	//���ڵ���Ե����Ϊ׼
				//wSumThick+=abs(pGasketRelaPart->siThick);
				double maxspace=pGasket->gasketThick-abs(pBoltRelaPartList->pExistGasket->siThick);
				if(fabs(maxspace)>=SPACE_MIN_END_THRESHOLD)
					return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_ERROR_GASKET_THICK;//6;	//���ڵ�嵫����ȴ���
				return pBoltRelaPartList->m_cErrorType=0;
			}
			else if(pGasket->wiCurrDianQuanThick>0&&pGasket->pBolt->CalPadPara(pGasket->wiCurrDianQuanThick)==DIANQUAN_STRU::DIANQUAN)
			{
				double gap=fabs(pGasket->gasketThick-pGasket->wiCurrDianQuanThick);
				if(gap>=SPACE_MIN_THRESHOLD)
					return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_ERROR_GASKET_THICK;	//��Ȧ��Ȳ���ȷ
				else	//��Ȧ������в���Ҫ�޶�
					return 0;
			}
			else
			{
				pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_MISS_END_PAD;//4;	//�����Ӵ�ȱ�ٵ�Ƭ
				BOLT_RELA_PART *pBtmRelaPart=NULL,*pTopRelaPart=NULL,*pEndLjRelaPart=NULL;
				for(pRelaPart=pEndRelaPartList->EnumFirstRelaPart();pRelaPart;pRelaPart=pEndRelaPartList->EnumNextRelaPart())
				{
					if(pRelaPart->hPart==pGasket->hBtmGasketRelaPart)
						pBtmRelaPart=pRelaPart;
					else if(pBtmRelaPart!=NULL&&pEndLjRelaPart==NULL)
						pEndLjRelaPart=pRelaPart;
					else if(pEndLjRelaPart)
					{
						if(pRelaPart->hPart==pGasket->hTopGasketRelaPart)
							pTopRelaPart=pRelaPart;
						break;
					}
				}
				if(pEndLjRelaPart&&pBtmRelaPart!=NULL&&pTopRelaPart!=NULL)
				{
					//wSumThick+=abs(pEndLjRelaPart->siThick);
					//��¼��Ƭ��ȼ�ƫ����
					pGasket->gasketThick=abs(pEndLjRelaPart->siThick);
					//pGasket->washerOffset����ǰ�ڵ���CBoltRelaPartList::InitializeBoltGasket(pGasket)ʱ��ʼ��
					//pGasket->washerOffset=(pBtmRelaPart->siThick>0)?ftoi(pBtmRelaPart->siThick+pBtmRelaPart->zDepth):ftoi(pBtmRelaPart->zDepth);
				}
				else
				{
					pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_PART_POS_ERROR;//5.����λ�ô���
					if(phErrPosAngle)
						*phErrPosAngle=pAssmAngle->hPart;
				}
			}
		}
	}
	else if(pBtmAssmPart&&pTopAssmPart&&pBtmAssmPart->pSmartPart->IsAngle()&&pTopAssmPart->pSmartPart->IsAngle())
	{
		if(pBoltRelaPartList->pExistGasket)
		{
			double maxspace=pGasket->gasketThick-abs(pBoltRelaPartList->pExistGasket->siThick);
			if(fabs(maxspace)>SPACE_MIN_CROSS_THRESHOLD)
				pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_ERROR_GASKET_THICK;//6;	//���ڵ�嵫����ȴ���
		}
		else if(pGasket->wiCurrDianQuanThick>0)
		{	
			if(fabs(pGasket->gasketThick-pGasket->wiCurrDianQuanThick)>SPACE_MIN_THRESHOLD)
				return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_ERROR_GASKET_THICK;	//��Ȧ��Ȳ���ȷ
		}
		else if(pBoltRelaPartList->GetRelaPartCount()==2)
		{
			BYTE ciTopErrorType=JustifyErrorEndPosAngle(pTopAssmPart,pGasket);
			if(ciTopErrorType==CBoltRelaPartList::TYPE_PART_POS_ERROR)
			{
				if(phErrPosAngle)
					*phErrPosAngle=pTopAssmPart->hPart;
				return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_PART_POS_ERROR;
			}
			BYTE ciBtmErrorType=JustifyErrorEndPosAngle(pBtmAssmPart,pGasket);
			if(ciBtmErrorType==CBoltRelaPartList::TYPE_PART_POS_ERROR)
			{
				if(phErrPosAngle)
					*phErrPosAngle=pBtmAssmPart->hPart;
				return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_PART_POS_ERROR;
			}
			if(ciTopErrorType==ciBtmErrorType&&ciTopErrorType==CBoltRelaPartList::TYPE_MISS_CROSSED_PAD)
				return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_MISS_CROSSED_PAD;
			else
				return 0;
		}
		else
		{
			int anglecount=0;
			for(pRelaPart=pBoltRelaPartList->EnumFirstRelaPart();pRelaPart&&anglecount<3;pRelaPart=pBoltRelaPartList->EnumNextRelaPart())
			{
				if(pRelaPart->pPart&&pRelaPart->pPart->IsAngle())
					anglecount++;
			}
			if(anglecount>=3)
			{
				return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_PART_POS_ERROR;
			}
			//TODO: else �д����ƣ�һ�㲻������������ wjh-2017.12.8
		}
	}
	else
		return pBoltRelaPartList->m_cErrorType=CBoltRelaPartList::TYPE_PART_POS_ERROR;	//5.����λ�ô���	
	return pBoltRelaPartList->m_cErrorType;
}

BOOL CEmendBoltGasket::ExecuteEmendBoltSpec(DATACENTERCMD_CONTEXT *pContext)
{
	ATOM_LIST<COutputInfo>* pOutInfoList=(ATOM_LIST<COutputInfo>*)pContext->m_pOutputInfoList;
	ARRAY_LIST<DWORD> assmbleBoltIdArr;
	/*if(DATACENTERCMD_CONTEXT::OBJ_TYPE_SELECTED_OBJS==pContext->m_cOperObjType)
	{
		for(CAssembleBolt *pBoltRec=boltRecSet.GetFirst();pBoltRec;pBoltRec=boltRecSet.GetNext())
			assmbleBoltIdArr.append(pBoltRec->Id);
	}
	if(assmbleBoltIdArr.GetSize()>0)*/
		UpdateBoltRelaObjs(assmbleBoltIdArr.m_pData,assmbleBoltIdArr.GetSize());
	//else //һ�����ļ��򿪻����ļ�ʱ�Ѹ�����˨�����ϵ,��ȷ����Ҫ���ֶ����Ҳ�ģ�����Ҽ��˵����ֶ�ִ�� wjh-2017.3.23
	//	UpdateBoltRelaObjs();

#ifdef _OPEN_TEMP
	CLogErrorLife logErrorLife;
	int i=0,nBoltNum=boltRecSet.GetNodeNum();
	int nSumProcess=pContext->m_nEndProcess-pContext->m_nStartProcess;
	DisplayProcess(pContext->m_nStartProcess,"�޶���˨���");
	CFileModel *pModel=NULL;	//pModel������NULLʱ˵����˨����ѱ����Ҫ����SegGroup
	BOOL bDisplayFinished=pContext->chkStyleFlag&DATACENTERCMD_CONTEXT::EMEND_BOLT_DISPLAY_FINISHED;
	BOOL bDisplayMissGasket=pContext->chkStyleFlag&DATACENTERCMD_CONTEXT::EMEND_BOLT_GASKET;
	struct ERROR_INFO{
		BYTE cRetCode;
		long idRelaAssemPart1;
		long idRelaAssemPart2;
		double fMaxSpace;
		CXhPtrSet<CAssembleBolt> boltRecSet;
		ERROR_INFO(){cRetCode=0;fMaxSpace=0;idRelaAssemPart1=idRelaAssemPart2=0;}
	};
	CMapList<ERROR_INFO> hashErrorInfoByIds;
	for(CAssembleBolt *pBoltRec=boltRecSet.GetFirst();pBoltRec;pBoltRec=boltRecSet.GetNext())
	{
		i++;
		DisplayProcess(pContext->m_nStartProcess+(nSumProcess*i)/nBoltNum,"�޶���˨���");
		if(pContext->m_bInstanceGroup)
		{
			if(pContext->m_hashFlagByTypeAndId.GetValue(CMD_TYPE_EMEND_BOLT_SPEC,pBoltRec->GetId()))
				continue;
			else 
				pContext->m_hashFlagByTypeAndId.SetValue(CMD_TYPE_EMEND_BOLT_SPEC,pBoltRec->GetId(),1);
		}
		GEPOINT oldPos=pBoltRec->Position;
		CBoltAssembly *pBoltAssembly=pBoltRec->BelongModel()->FromBoltAssemblyId(pBoltRec->GetAssemblePlace());
		WORD L0=pBoltAssembly->L0;
		double fMaxSpace=0;
		CAssembleAngle *pErrorAngleRec=NULL;
		CBoltRelaPartList *pRelaPartList=NULL;
		BYTE cRetCode=pBoltRec->RevisionBoltL0nZpos(pInstance,&fMaxSpace,&pErrorAngleRec,&pRelaPartList);
		CStdBolt *pOldBolt=(CStdBolt*)pBoltRec->GetPart();
		CXhChar16 sOldSpec=pOldBolt?pOldBolt->GetPartNo():CXhChar16("");
		pBoltRec->CalBoltSpec();
		CStdBolt *pNewBolt=(CStdBolt*)pBoltRec->GetPart();
		CXhChar16 sNewSpec=pNewBolt?pNewBolt->GetPartNo():CXhChar16("");
		if(pOldBolt!=pNewBolt)
			pModel=pBoltRec->BelongModel();
		if( oldPos!=pBoltRec->Position||L0!=pBoltAssembly->L0||
			pOldBolt!=pBoltRec->GetPart()||!sOldSpec.EqualNoCase(sNewSpec))
		{
			pBoltRec->SetModified();
			RedrawSolidPart(pBoltRec);
		}
		if( pOutInfoList&&(cRetCode!=0||
			(L0!=pBoltAssembly->L0&&bDisplayFinished)||
			(!sOldSpec.Equal(sNewSpec)&&bDisplayFinished)))
		{
			if((cRetCode==3||cRetCode==4)&&pRelaPartList)
			{	//3.��Ҫ�����˨��Ƭ
				BOLT_RELA_PART *pRelaPart1=NULL,*pRelaPart2=NULL;
				pRelaPartList->GetMaxSpaceAndSumThroughThick(NULL,&pRelaPart1,&pRelaPart2,NULL);
				if(pRelaPart1&&pRelaPart2)
				{
					ERROR_INFO *pErrorInfo=hashErrorInfoByIds.GetValue(pRelaPart1->idAssmPart,pRelaPart2->idAssmPart);
					if(pErrorInfo==NULL)
					{
						pErrorInfo=hashErrorInfoByIds.Add(pRelaPart1->idAssmPart,pRelaPart2->idAssmPart);
						pErrorInfo->cRetCode=cRetCode;
						pErrorInfo->idRelaAssemPart1=pRelaPart1->idAssmPart;
						pErrorInfo->idRelaAssemPart2=pRelaPart2->idAssmPart;
						pErrorInfo->fMaxSpace=fMaxSpace;
					}
					pErrorInfo->boltRecSet.append(pBoltRec);
					continue;
				}
			}
			else if(cRetCode==5&&pErrorAngleRec)
			{	//5.����λ�ô���
				ERROR_INFO *pPosError=hashErrorInfoByIds.GetValue(pErrorAngleRec->Id,0);
				if(pPosError==NULL)
				{
					pPosError=hashErrorInfoByIds.Add(pErrorAngleRec->Id,0);
					pPosError->cRetCode=cRetCode;
					pPosError->fMaxSpace=fMaxSpace;
					pPosError->idRelaAssemPart1=pErrorAngleRec->Id;
				}
				pPosError->boltRecSet.append(pBoltRec);
				continue;
			}
			COutputInfo *pOutInfo=pOutInfoList->append();
			pOutInfo->m_cType=COutputInfo::INFO_EMEND_BOLT_SPEC;
			pOutInfo->m_sTypeName="�޶���˨���";
			pOutInfo->m_hRelaObj1=pBoltRec->GetId();
			pOutInfo->m_pTowerModel=pBoltRec->BelongModel();
			pOutInfo->m_pTowerInstance=pInstance;
			pOutInfo->segI=pBoltRec->GetSegI();
			pOutInfo->InitSolvedState(cRetCode==0);
			pOutInfo->m_dwFeature=cRetCode;
			pOutInfo->tolerance=fMaxSpace;
			if(pOutInfo->segI.iSeg>0)
				pOutInfo->m_sDescription.AppendFormat("%s��",(char*)pOutInfo->segI.ToString());
			if(cRetCode==0&&L0!=pBoltAssembly->L0&&bDisplayFinished)
			{
				if(pOutInfo->m_sDescription.GetLength()>0)
					pOutInfo->m_sDescription.Append(",");
				pOutInfo->m_sDescription.AppendFormat("��˨0x%Xͨ��%d->%d",pBoltRec->GetId(),L0,pBoltAssembly->L0);
			}
			if(cRetCode==1)
			{	//1.�޹�������
				pOutInfo->m_sTypeName="δ���ӹ���";
				if(pOutInfo->m_sDescription.GetLength()>0)
					pOutInfo->m_sDescription.Append(",");
				pOutInfo->m_sDescription.AppendFormat("��˨0x%Xδ�����κι���",pBoltRec->GetId());
			}
			else if(cRetCode==2)
			{	//2.�ǽŶ���˨������һ������
				pOutInfo->m_sTypeName="������һ������";
				if(pOutInfo->m_sDescription.GetLength()>0)
					pOutInfo->m_sDescription.Append(",");
				BOLT_RELA_PART *pRelaPart=pRelaPartList->EnumFirstRelaPart();
				CAssemblePart *pPartRec=pBoltRec->BelongModel()->FromPartRecId(pRelaPart?pRelaPart->idAssmPart:0);
				CProcessPart *pPart=pPartRec?(CProcessPart*)pPartRec->GetPart():NULL;
				HOLE_REF *pHoleRef=pPartRec?pPartRec->FindHoleRefByBoltRecId(pBoltRec->Id):NULL;
				if(pPart&&pHoleRef)
				{
					pOutInfo->m_hRelaObj2=pPartRec->Id;
					pOutInfo->m_hRelaObj3=pHoleRef->holeId;
					pOutInfo->m_sDescription.AppendFormat("����(%s)%d��λ��Ӧ����˨0x%X������һ������",pPart->Label,pHoleRef->holeId,pBoltRec->GetId());
				}
				else
					pOutInfo->m_sDescription.AppendFormat("��˨0x%X(�ǽŶ�)������һ������",pBoltRec->GetId());
			}
			else if(cRetCode==3||cRetCode==4)
			{	//3.��Ҫ�����˨��Ƭ
				pOutInfo->m_sTypeName="ȱ�ٵ�Ƭ";
				if(pOutInfo->m_sDescription.GetLength()>0)
					pOutInfo->m_sDescription.Append(",");
				pOutInfo->m_sDescription.AppendFormat("��˨0x%X������Ҫ��ӵ�Ƭ,��϶ֵ%.f,ͨ���ѿ��Ǹü�϶��",pBoltRec->GetId(),fMaxSpace);
			}
			else if(cRetCode==5)
			{	//5.����λ�ô���
				pOutInfo->m_sTypeName="����λ�ô���";
				if(pOutInfo->m_sDescription.GetLength()>0)
					pOutInfo->m_sDescription.Append(",");
				pOutInfo->m_sDescription.AppendFormat("��˨0x%X,����λ�ô���",pBoltRec->GetId());
			}
			else if(cRetCode==6)
			{	//6.��Ȧ��Ȳ���ȷ
				pOutInfo->m_sTypeName="���(Ȧ)��ȴ���";
				if(pOutInfo->m_sDescription.GetLength()>0)
					pOutInfo->m_sDescription.Append(",");
				pOutInfo->m_sDescription.AppendFormat("��˨0x%X��Ȧ����",pBoltRec->GetId());
			}
			if(!sOldSpec.Equal(sNewSpec)&&bDisplayFinished)
			{
				if(pModel==NULL)
					pModel=pBoltRec->BelongModel();
				if(pOutInfo->m_sDescription.GetLength()>0)
					pOutInfo->m_sDescription.Append(",");
				pOutInfo->m_sDescription.AppendFormat("��˨0x%X���%s->%s",pBoltRec->GetId(),(char*)sOldSpec,(char*)sNewSpec);
			}
		}
	}
	for(ERROR_INFO *pErrorInfo=hashErrorInfoByIds.GetFirst();pErrorInfo;pErrorInfo=hashErrorInfoByIds.GetNext())
	{
		if(!bDisplayMissGasket&&(pErrorInfo->cRetCode==3||pErrorInfo->cRetCode==4))
			continue;	//����ʾ��ʧ����¼(����ͨ��ʱ�ѿ��ǵ����)
		CAssembleBolt *pBoltRec=NULL;
		COutputInfo *pOutInfo=pOutInfoList->append();
		pOutInfo->m_cType=COutputInfo::INFO_EMEND_BOLT_SPEC;
		if(pErrorInfo->cRetCode==3||pErrorInfo->cRetCode==4)
			pOutInfo->m_sTypeName="ȱ�ٵ�Ƭ";
		else if(pErrorInfo->cRetCode==5)
		{
			pOutInfo->m_sTypeName="���(Ȧ)��ȴ���";
			pErrorInfo->cRetCode=6;
			const int MAX_END_SPACE=100;
			CAssembleAngle *pAngleRec=model.FromAngleRecId(pErrorInfo->idRelaAssemPart1);
			CProcessAngle *pAngle=pAngleRec?(CProcessAngle*)pAngleRec->GetPart():NULL;
			for(pBoltRec=pErrorInfo->boltRecSet.GetFirst();pAngleRec&&pBoltRec;pBoltRec=pErrorInfo->boltRecSet.GetNext())
			{
				HOLE_REF *pHoleRef=pAngleRec->FindHoleRefByBoltRecId(pBoltRec->Id);
				if(pHoleRef&&(pHoleRef->LocalCoordX<MAX_END_SPACE||pHoleRef->LocalCoordX>pAngle->m_wLength-MAX_END_SPACE))
				{
					pOutInfo->m_sTypeName="����λ�ô���";
					pErrorInfo->cRetCode=5;
					break;
				}	
			}
		}
		pBoltRec=pErrorInfo->boltRecSet.GetFirst();
		pOutInfo->m_hRelaObj1=pBoltRec->Id;
		pOutInfo->m_hRelaObj2=pErrorInfo->idRelaAssemPart1;
		pOutInfo->m_hRelaObj3=pErrorInfo->idRelaAssemPart2;
		pOutInfo->m_pTowerModel=pBoltRec->BelongModel();
		pOutInfo->m_pTowerInstance=pInstance;
		pOutInfo->segI=pBoltRec->GetSegI();
		pOutInfo->InitSolvedState(pErrorInfo->cRetCode==0);
		pOutInfo->m_dwFeature=pErrorInfo->cRetCode;
		pOutInfo->tolerance=pErrorInfo->fMaxSpace;
		if(pOutInfo->segI.iSeg>0)
			pOutInfo->m_sDescription.AppendFormat("%s��",(char*)pOutInfo->segI.ToString());
		if(pOutInfo->m_sDescription.GetLength()>0)
			pOutInfo->m_sDescription.Append(",");
		pOutInfo->m_relaSonObjArr.Empty();
		CXhChar500 sBoltRec;
		for(pBoltRec=pErrorInfo->boltRecSet.GetFirst();pBoltRec;pBoltRec=pErrorInfo->boltRecSet.GetNext())
		{
			sBoltRec.Append(CXhChar16("0x%X",pBoltRec->Id),',');
			pOutInfo->m_relaSonObjArr.append(pBoltRec->Id);
		}
		if(pErrorInfo->cRetCode==3||pErrorInfo->cRetCode==4)
			pOutInfo->m_sDescription.AppendFormat("ȱ��0x%X���(��϶ֵ%.f),������˨(%s)",pErrorInfo->idRelaAssemPart1,pErrorInfo->fMaxSpace,(char*)sBoltRec);
		else if(pErrorInfo->cRetCode==6)
			pOutInfo->m_sDescription.AppendFormat("��˨%s��Ȧ����",(char*)sBoltRec);
		else
		{
			CAssemblePart *pPartRec=model.FromPartRecId(pErrorInfo->idRelaAssemPart1);
			CProcessPart *pPart=pPartRec?(CProcessPart*)pPartRec->GetPart():NULL;
			CXhChar16 sLabel=pPart?pPart->GetPartNo():"";
			pOutInfo->m_sDescription.AppendFormat("�Ǹ�(%s)0x%Xλ�ô���(��϶ֵ%.f),������˨(%s)",(char*)sLabel,
												  pErrorInfo->idRelaAssemPart1,pErrorInfo->fMaxSpace,(char*)sBoltRec);
		}
	}
	g_pSolidDraw->Draw();
	if(pModel&&pModel->IsTowerModel()&&(pContext->m_nEndProcess==100||pContext->dwParam==1))
	{	//��˨�������ˢ��ģ����
		CTowerTreeDlg *pTowerTreeDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
		if(pTowerTreeDlg)
			pTowerTreeDlg->RefreshModelTree(&model);
		for(CTowerInstance *pInstance=((CTowerModel*)pModel)->EnumTowerInstanceFirst();pInstance;
			pInstance=((CTowerModel*)pModel)->EnumTowerInstanceNext())
		{
			if(pTowerTreeDlg)
				pTowerTreeDlg->RefreshTowerInstanceSegScopeItem(*pTowerTreeDlg->GetTreeCtrl(2),NULL,pInstance);
		}
	}
	DisplayProcess(pContext->m_nEndProcess,"�޶���˨���");
#endif
	return TRUE;
}
