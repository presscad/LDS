// TowerType.cpp: implementation of the CTowerBOM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TowerBOM.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "SortFunc.h"
#include "ComparePartNoString.h"
#include "env_def.h"
#include "LibraryQuery.h"
#include "ArrayList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __ANGLE_PART_INC_

static BOMPART* CreateBomPart(int idClsType,const char* key,void* pContext)
{
	BOMPART *pPart=NULL;
	switch (idClsType){
	case BOMPART::PLATE:
		pPart=new PART_PLATE();
		break;
	case BOMPART::ANGLE:
		pPart=new PART_ANGLE();
		break;
	case BOMPART::TUBE:
		pPart=new PART_TUBE();
		break;
	case BOMPART::SLOT:
		pPart=new PART_SLOT();
		break;
	default:
		pPart=new BOMPART();
		break;
	}
	return pPart;
}
static BOOL DeleteBomPart(BOMPART *pPart)
{
	if(pPart==NULL)
		return FALSE;
	switch (pPart->cPartType){
	case BOMPART::PLATE:
		delete (PART_PLATE*)pPart;
		break;
	case BOMPART::ANGLE:
		delete (PART_ANGLE*)pPart;
		break;
	case BOMPART::TUBE:
		delete (PART_TUBE*)pPart;
		break;
	case BOMPART::SLOT:
		delete (PART_SLOT*)pPart;
		break;
	default:
		delete pPart;
		break;
	}
	return TRUE;
}

//��˨���ַ���
typedef struct tagHOLE_INFO
{
	int d;					//��˨ֱ��
	double hole_d_increment;	//������
	int nCount;				//��ֱ����˨����
}HOLE_INFO;

CTowerBOM::CTowerBOM()
{
	m_pTower=NULL;
	m_pModel=NULL;
	hashParts.CreateNewAtom=CreateBomPart;
	hashParts.DeleteAtom=DeleteBomPart;
}

void CTowerBOM::SummarizeOneBolt(CLDSBolt *pBolt)
{
	if (pBolt->IsShadowObject())
		pBolt->ShadowDesign();
	CString sGrade,sGuiGe;
	if(pBolt->m_bVirtualPart)	//�������Ե�
		return;
	CLsFamily *pFamily = CLsLibrary::FromHandle(pBolt->m_hFamily);
	if (pFamily == NULL)
		pFamily = CLsLibrary::DefaultFamily();
	//������˨���ͳ��ʱ�����ֶμ�¼����
	SEGI seg_arr[20];
	int seg_n=pBolt->GetStatSegArr(seg_arr,20);
	for(int i=0;i<seg_n;i++)
	{
		BOMBOLT bolt;
		bolt.bTwoCap=pBolt->IsTwoCap()!=0;
		bolt.bFootNail=pBolt->IsFootNail()!=0;
		bolt.d = (short)pBolt->get_d();
		bolt.L=(short)pBolt->get_L();
		bolt.L0=(short)pBolt->get_L0();
		bolt.hFamily = (short)pFamily->GetFamilyHandle();
		bolt.grade = (float)atof(pBolt->Grade());
		bolt.iSeg = pBolt->iSeg;
		bolt.bAntiTheft = pBolt->IsAntiTheft();//(m_dwFlag.IsHasFlag(CLDSBolt::ANTI_THEFT)==TRUE);
		bolt.bAnitLoose = pBolt->IsAntiLoose();//(m_dwFlag.IsHasFlag(CLDSBolt::ANTI_LOOSE)==TRUE);
		//
		bolt.nCommonNut = pFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_COMMON_NUT);		//��ͨ��ĸ
		bolt.nFastenNut = pFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_FASTEN_NUT);		//�۽���ĸ
		bolt.nAntiTheftNut = pFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_ANTITHEFT_NUT);//������ĸ
		bolt.nThinNut = pFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_THIN_NUT);			//����ĸ
		bolt.nCommonGasket = pFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_COMMON_GASKET);//ƽ��Ȧ
		bolt.nSpringGasket = pFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_SPRING_GASKET);//���ɵ�Ȧ
		bolt.nLockClasp = pFamily->GetLsSubPartNum(CLsSubPartFamily::TYPE_LOCK_CLASP);		//���ɿ�
		
		LS_XING_HAO *pXingHao=CLsLibrary::FindLsGuiGeByValidLen(bolt.d,bolt.L,bolt.hFamily);
		if(pXingHao)
			bolt.nNoThreadlen = pXingHao->no_thread_len;
		else
			logerr.Log("�ͺ�Ϊ��");
		BOMBOLT* pLegacyRecord=hashBolts.GetValue(bolt.CombinedKey(m_bStatLsBySegI));
		if(pLegacyRecord==NULL)
		{
			pLegacyRecord=hashBolts.SetValue(bolt.CombinedKey(m_bStatLsBySegI),bolt);
			//ʹ����˨ֱ������˨ͨ������˨����в�����Ӧ����˨��¼
			LS_XING_HAO *pLsType=globalLibrary.FindLsGuiGe(bolt.d,bolt.L0,bolt.hFamily);
			if(pLsType)
				pLegacyRecord->fUnitWeight=(float)pLsType->weight;
		}
		pLegacyRecord->AddPart(pBolt->m_uStatMatCoef);
		//��Ȧ�͵���ͳ��
		if(pBolt->DianQuan.N>0)
		{	//��˨�ϴ��е�Ȧ
			bolt.hFamily=-1;	//��Ȧ
			bolt.L=pBolt->DianQuan.thick;
			pLegacyRecord=hashBolts.GetValue(bolt.CombinedKey(m_bStatLsBySegI));
			if(pLegacyRecord==NULL)
			{
				pLegacyRecord=hashBolts.SetValue(bolt.CombinedKey(m_bStatLsBySegI),bolt);
				pLegacyRecord->fUnitWeight=(float)FindPadWei(bolt.L,bolt.d);
			}
			pLegacyRecord->AddPart(pBolt->m_uStatMatCoef*pBolt->DianQuan.N);
		}
	}
}
void CTowerBOM::SummarizeParts()
{
	CLDSPart *pPart;
	POLYPARTLIST polylist;
	if(m_pModel->IsTowerModel())
		((CTower*)m_pModel)->RetrievePolyPartList(polylist);
	int i=0;
	for(pPart=m_pModel->EnumPartFirst();pPart!=NULL;pPart=m_pModel->EnumPartNext(),i++)
	{
		if(pPart->m_bVirtualPart)	//�����Ǹֲ�ͳ��
			continue;
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//���ϻ���ʱ��ͳ����ϽǸ�(����)���� wht 11-03-11
		else if(pPart->IsLinePart())
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->bHuoQuStart||pLinePart->bHuoQuEnd)
				continue;	//�����ӹ����ɻ�����������ͳ��
		}
		BOOL pushed=m_pModel->PushPartStack();
		SummarizeOnePart(pPart);
		m_pModel->PopPartStack();
	}
	for(CLDSPolyPart *pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
		SummarizeOnePart(pPolyPart);
}
void CTowerBOM::SummarizeOneLineAngle(CLDSLineAngle *pLineAngle)
{
	BOMPART* pBomPart=hashParts.GetValue(pLineAngle->GetPartNo());
	if(pBomPart==NULL)
	{	//0.�ڵ� 1.�Ǹ� 2.��˨ 3.���� 4.�ֹ� 5.��� 6.�۸� 100.����
		pBomPart=hashParts.Add(pLineAngle->GetPartNo(),BOMPART::ANGLE);
		pLineAngle->CloneToBomPart(pBomPart);
	}
	pBomPart->AddPart(pLineAngle->m_uStatMatCoef);
}
void CTowerBOM::SummarizeOneLineTube(CLDSLineTube *pTube)
{
	BOMPART* pBomPart=hashParts.GetValue(pTube->GetPartNo());
	if(pBomPart==NULL)
	{	
		pBomPart=hashParts.Add(pTube->GetPartNo(),BOMPART::TUBE);
		pTube->CloneToBomPart(pBomPart);
	}
	pBomPart->AddPart(pTube->m_uStatMatCoef);
}
void CTowerBOM::SummarizeOneLineSlot(CLDSLineSlot *pLineSlot)
{	
	BOMPART* pBomPart=hashParts.GetValue(pLineSlot->GetPartNo());
	if(pBomPart==NULL)
	{	
		pBomPart=hashParts.Add(pLineSlot->GetPartNo(),BOMPART::SLOT);
		pLineSlot->CloneToBomPart(pBomPart);
	}
	pBomPart->AddPart(pLineSlot->m_uStatMatCoef);
}
void CTowerBOM::SummarizeOneParamPlate(CLDSParamPlate *pPlate)
{
	if (pPlate->IsShadowObject())
		pPlate->ShadowDesign();
	if(pPlate->vertex_list.GetNodeNum()<3)
		pPlate->DesignPlate();
	if(pPlate->vertex_list.GetNodeNum()<3)
		return;
	CXhChar200 key("%s#%s",(char*)pPlate->GetPartNo(),(char*)pPlate->iSeg.ToString());//ͬһ��ŵı�׼���������ڶ�����г���
	if(hashParts.GetValue(pPlate->GetPartNo()))
		logerr.Log("�����쳣");
	BOMPART* pBomPart=hashParts.GetValue(key);
	if(pBomPart==NULL)
	{
		pBomPart=hashParts.Add(key,BOMPART::PLATE);
		pPlate->CloneToBomPart(pBomPart);
	}
	pBomPart->AddPart(pPlate->m_uStatMatCoef);
 }
void CTowerBOM::SummarizeOnePart(CLDSPart *pPart,CTower *pTower/*=NULL*/)
{	
	if(m_pModel==NULL)
	{
		m_pModel=pTower;
		m_pTower=pTower;
	}
	if(m_pTower==NULL)
		return;
	//��δ���������ͳ��
	CString sPureGuiGe,cMaterial;
	CLDSPart *pStatDatumPart=pPart;
	if(pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		pStatDatumPart=pPolyPart->segset.GetFirst();
	}
	else if(pPart->IsLinePart())
	{
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->bHuoQuStart||pLinePart->bHuoQuEnd)
			return;	//�����ӹ������������ͳ�Ʋ�����ͳ��
	}
	if(pPart->GetClassTypeId()==CLS_BOLT) 
		SummarizeOneBolt((CLDSBolt*)pPart);
	else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		SummarizeOneLineAngle((CLDSLineAngle*)pPart);
	else if(pPart->GetClassTypeId()==CLS_LINETUBE)
		SummarizeOneLineTube((CLDSLineTube*)pPart);
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
		SummarizeOneLineSlot((CLDSLineSlot*)pPart);
	else if(pPart->GetClassTypeId()==CLS_PLATE)
		SummarizeOnePlate((CLDSPlate*)pPart);
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		SummarizeOneParamPlate((CLDSParamPlate*)pPart);
	else if(pPart->GetClassTypeId()==CLS_POLYPART)
		SummarizeOnePolyPart((CLDSPolyPart*)pPart);
}
void CTowerBOM::SummarizeOnePolyPart(CLDSPolyPart *pPolyPart)
{
	BOMPART* pBomPart=hashParts.GetValue(pPolyPart->GetPartNo());
	CLDSLinePart* pRod=NULL,*pFirstRod=pPolyPart->segset.GetFirst();
	if(pFirstRod==NULL||pFirstRod->GetClassTypeId()!=CLS_LINEANGLE)
		return;
	CXhChar16 sPartNo;
	for(pRod=pPolyPart->segset.GetFirst();pRod&&strlen(pRod->GetPartNo())==0;pRod=pPolyPart->segset.GetNext())
		continue;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
	if(pAngle==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Bending angle's Label is empty(First angle's hadle:0x%X)��",pFirstRod->handle);
#else
		logerr.Log("�����Ǹ�δ���(��һ���ӽǸ־��:0x%X)��",pFirstRod->handle);
#endif
		return;
	}
	if(pBomPart==NULL)
	{
		pBomPart=hashParts.Add(pPolyPart->GetPartNo(),BOMPART::ANGLE);
		pPolyPart->CloneToBomPart(pBomPart);
		pBomPart->length=pPolyPart->GetLength();
		pBomPart->siZhiWan=(short)pPolyPart->segset.GetNodeNum()-1;
	}
	pBomPart->AddPart(pAngle->m_uStatMatCoef);
}
void CTowerBOM::SummarizeOnePlate(CLDSPlate *pPlate)
{
	if (pPlate->IsShadowObject())
		pPlate->ShadowDesign();
	if(((CLDSPlate*)(pPlate->MotherObject()))->vertex_list.GetNodeNum()<3)
		pPlate->DesignPlate();
	if(((CLDSPlate*)(pPlate->MotherObject()))->vertex_list.GetNodeNum()<3)
		return;
	CXhChar16 key("%s#%s",(char*)pPlate->GetPartNo(),(char*)pPlate->iSeg.ToString());
	BOMPART* pBomPart=hashParts.GetValue(key);
	if(pBomPart==NULL)
	{
		pBomPart=hashParts.Add(key,BOMPART::PLATE);
		pPlate->CloneToBomPart(pBomPart);
	}
	pBomPart->AddPart(pPlate->m_uStatMatCoef);
}
typedef CLDSPartGroup* CLDSPartGroupPtr;
static int compare_partgroup_partno(const CLDSPartGroupPtr& partref1,const CLDSPartGroupPtr& partref2)
{
	return ComparePartNoString((char*)partref1->sRootPartNo,(char*)partref2->sRootPartNo);
}
static int compare_partref_partno(const BOM_PARTREF& partref1,const BOM_PARTREF& partref2)
{
	return ComparePartNoString((char*)partref1.sPartNo,(char*)partref2.sPartNo);
}
void CTowerBOM::SummarizePartGroup()
{
	CLDSPartGroup *pPartGroup=NULL;
	ARRAY_LIST<CLDSPartGroupPtr> weldPartArr;
	weldPartArr.SetSize(0,100);
	for(pPartGroup=m_pModel->EnumPartGroupFirst();pPartGroup;pPartGroup=m_pModel->EnumPartGroupNext())
	{
		CLDSPart *pRootPart=m_pModel->FromPartHandle(pPartGroup->handle);
		if(pRootPart==NULL)
			continue;
		strcpy(pPartGroup->sRootPartNo,pRootPart->GetPartNo());
		weldPartArr.append(pPartGroup);
	}
	CQuickSort<CLDSPartGroupPtr>::QuickSort(weldPartArr.m_pData,weldPartArr.GetSize(),compare_partgroup_partno);
	for(int index=0;index<weldPartArr.GetSize();index++)
	{
		pPartGroup=weldPartArr[index];
		CLDSPart *pRootPart=m_pModel->FromPartHandle(pPartGroup->handle);
		if(pRootPart==NULL)
			continue;
		PARTSET partset;
		m_pModel->GetPartGroupPartSet(pPartGroup->handle,partset);
		partset.append(pRootPart);
		BOM_WELDPART *pWeldPart=listWeldParts.append();
		pWeldPart->iSeg=pRootPart->iSeg;	//�麸���κ�
		pWeldPart->nWeldPartNum=(short)pPartGroup->m_uStatMatCoef;
		if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)
			pWeldPart->cWeldPartType=BOM_WELDPART::TYPE_ANGLEFOOT;
		else if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_TUBEFOOT)
			pWeldPart->cWeldPartType=BOM_WELDPART::TYPE_TUBEFOOT;
		else if(pRootPart->IsPlate())	//�ְ��麸��
			pWeldPart->cWeldPartType=BOM_WELDPART::TYPE_PLATES;
		else if(pRootPart->IsAngle()) 
		{	//�Ǹ��麸��
			BOOL bHashLjb=FALSE;	//�����Ӱ�
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				CLsRefList *pLsRefList=pPart->GetLsRefList();
				if(pLsRefList==NULL)
					continue;
				for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
				{
					if(!pLsRef->GetLsPtr()->m_bVirtualPart)
					{
						bHashLjb=TRUE;
						break;
					}
				}
				if(bHashLjb)
					break;
			}
			if(bHashLjb)
				pWeldPart->cWeldPartType=BOM_WELDPART::TYPE_ANGLELJB;
			else
				pWeldPart->cWeldPartType=BOM_WELDPART::TYPE_ANGLERIBPLATE;
		}
		strcpy(pWeldPart->sName,pPartGroup->sGroupName);
		strcpy(pWeldPart->sPartNo,pRootPart->GetPartNo());
		int nPartRef=0;
		LIST_NODE<BOM_PARTREF> *pNode=NULL;
		for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{	
			for(pNode=pWeldPart->listPartRef.EnumFirst();pNode;pNode=pWeldPart->listPartRef.EnumNext())
			{
				if(stricmp(pNode->data.sPartNo,pPart->GetPartNo())==0)
				{
					pNode->data.nPartNum++;	//�˴����ÿ���ͳ��ϵ�����⣬ֻ��ͳ��һ���麸���еĹ�������
					break;
				}
			}
			if(pNode==NULL)
			{
				pNode=pWeldPart->listPartRef.AttachNode();
				strcpy(pNode->data.sPartNo,pPart->GetPartNo());
				pNode->data.nPartNum=1;
				nPartRef++;
				//SummarizePartByModule(limit_str,pRec->iSeg);
			}
		}
		//���麸���еĹ����������������
		ARRAY_LIST<BOM_PARTREF> part_arr;
		part_arr.SetSize(0,nPartRef);
		for(pNode=pWeldPart->listPartRef.EnumFirst();pNode;pNode=pWeldPart->listPartRef.EnumNext())
			part_arr.append(pNode->data);
		CBubbleSort<BOM_PARTREF>::BubSort(part_arr.m_pData,part_arr.GetSize(),compare_partref_partno);
		int i=0;
		for(pNode=pWeldPart->listPartRef.EnumFirst();pNode;pNode=pWeldPart->listPartRef.EnumNext())
		{
			pNode->data=part_arr[i];
			i++;
		}
	}
}
void CTowerBOM::SummarizeSuppPart()	//ͳ�Ƹ��ӹ���
{
	if(m_pTower==NULL)
		return;
	return;	//�ݲ�֧��
}

BOOL CTowerBOM::RunSummarize(CTower* pTower,bool bIncLsSegI/*=false*/)	//���в���ͳ��
{
	if(pTower==NULL)
		return FALSE;
	m_pModel = pTower;
	m_pTower = pTower;
	m_bStatLsBySegI=bIncLsSegI;
	hashParts.Empty();
	hashBolts.Empty();
	listWeldParts.Empty();

	SummarizeParts();		//ͳ�ƹ��������ڵĲ���
	SummarizeSuppPart();	//ͳ�Ƹ��ӹ����Ĳ���
	SummarizePartGroup();	//ͳ���麸��
	SummarizeBlockRefs();	//ͳ�Ʋ�������
	return TRUE;
}
struct BOMPARTREF : public ICompareClass{
	BOMPART* pBomPart;
	int Compare(const ICompareClass *pCompareObj) const
	{
		BOMPART* pOtherBomPart=((BOMPARTREF*)pCompareObj)->pBomPart;
		return ComparePartNoString(pBomPart->sPartNo,pOtherBomPart->sPartNo);
	}
};
void CTowerBOM::CloneToModelBOM(CModelBOM* pBom)
{
	if(m_pTower)
	{
		pBom->m_sTowerTypeName.Copy(m_pTower->m_sTaType);
		pBom->m_sTaAlias.Copy(m_pTower->m_sTaAlias);
		pBom->m_sPrjCode.Copy(m_pTower->m_sPrjCode);
		pBom->m_sPrjName.Copy(m_pTower->m_sPrjName);
		pBom->m_sTaStampNo.Copy(m_pTower->m_sTaStampNo);
		pBom->m_sCompanyName.Copy(m_pTower->m_sCompanyName);
		pBom->m_sOperator.Copy(m_pTower->m_sOperator);
		pBom->m_sAuditor.Copy(m_pTower->m_sAuditor);
		pBom->m_sCritic.Copy(m_pTower->m_sCritic);
		pBom->m_nTaNum=m_pTower->m_nTaNum;
	}
	pBom->m_iPlankWeightStatType=CLDSPlate::m_iPlankWeightStatType;
	pBom->m_iCirclePlankAreaType=CLDSParamPlate::m_iCirclePlankAreaType;
	pBom->m_biMatCharPosType=g_sysPara.m_nMatCharPosType;
	pBom->m_cMatCharSeparator=g_sysPara.m_cMatCharSeparator;
	pBom->m_nBoltsN=hashBolts.GetNodeNum();
	pBom->m_nPartsN=hashParts.GetNodeNum();
	pBom->m_nWeldPartsN=listWeldParts.GetNodeNum();
	UINT i=0;
	if(pBom->m_nBoltsN>0)
	{
		pBom->m_pBoltRecsArr = new BOMBOLT[pBom->m_nBoltsN];
		for(BOMBOLT* pBomBolt=hashBolts.GetFirst();pBomBolt;pBomBolt=hashBolts.GetNext(),i++)
			pBom->m_pBoltRecsArr[i]=*pBomBolt;
	}
	DYN_ARRAY<BOMPARTREF> arrBomParts(hashParts.GetNodeNum());
	i=0;
	for(BOMPART* pBomPart=hashParts.GetFirst();pBomPart;pBomPart=hashParts.GetNext(),i++)
		arrBomParts[i].pBomPart=pBomPart;
	CHeapSort<BOMPARTREF>::HeapSortClassic(arrBomParts,pBom->m_nPartsN);
	pBom->listParts.DeleteList();
	for(i=0;i<hashParts.GetNodeNum();i++)
	{
		SUPERLIST_NODE<BOMPART> *pNode=pBom->listParts.AttachNode(arrBomParts[i].pBomPart->cPartType);
		CBuffer buffer(1024);
		arrBomParts[i].pBomPart->ToBuffer(buffer);
		buffer.SeekToBegin();
		pNode->pDataObj->FromBuffer(buffer);
	}
	//
	CBuffer buffer(1024);
	pBom->listWeldParts.DeleteList();
	for(BOM_WELDPART *pWeldPart=listWeldParts.GetFirst();pWeldPart;pWeldPart=listWeldParts.GetNext())
	{
		buffer.ClearContents();
		pWeldPart->ToBuffer(buffer);
		buffer.SeekToBegin();
		LIST_NODE<BOM_WELDPART> *pNode=pBom->listWeldParts.AttachNode();
		pNode->data.FromBuffer(buffer);
	}
}
void CTowerBOM::CloneFromModelBom(CModelBOM* pBom)
{
	for(int i=0;i<pBom->m_nBoltsN;i++)
		hashBolts.SetValue(pBom->m_pBoltRecsArr[i].CombinedKey(),pBom->m_pBoltRecsArr[i]);
	//
	hashParts.Empty();
	for(SUPERLIST_NODE<BOMPART> *pNodeItem=pBom->listParts.EnumFirst();pNodeItem;pNodeItem=pBom->listParts.EnumNext())
	{
		BOMPART* pSrcBomPart=(BOMPART*)pNodeItem->pDataObj;
		BOMPART* pDesBomPart=hashParts.Add(pSrcBomPart->sPartNo,pSrcBomPart->cPartType);
		CBuffer buffer(1024);
		pSrcBomPart->ToBuffer(buffer);
		buffer.SeekToBegin();
		pDesBomPart->FromBuffer(buffer);	
	}
	//
	listWeldParts.Empty();
	for(BOM_WELDPART *pWeldPart=pBom->listWeldParts.EnumObjectFirst();pWeldPart;pWeldPart=pBom->listWeldParts.EnumObjectNext())
	{
		BOM_WELDPART *pDesWeldPart=listWeldParts.append();
		CBuffer buffer(1024);
		pWeldPart->ToBuffer(buffer);
		buffer.SeekToBegin();
		pDesWeldPart->FromBuffer(buffer);
	}
}

void CTowerBOM::SummarizeBlockRefs()
{
	if(m_pTower==NULL)
		return;
	for(CBlockReference *pBlockRef=m_pTower->BlockRef.GetFirst();pBlockRef;pBlockRef=m_pTower->BlockRef.GetNext())
		SummarizeOneBlockRef(pBlockRef);
}
void CTowerBOM::SummarizeOneBlockRef(CBlockReference *pBlockRef)
{
	if(m_pTower==NULL)
		return;
	if(pBlockRef->IsExploded())
		return;	//�����Ĳ������ã��Ὣ��Ӧ�Ĳ�����ӵ���Ӧ�����У����Բ��ø��ݲ������в���ͳ��
	CBlockModel *pBlock = m_pTower->Block.FromHandle(pBlockRef->m_hBlock);
	if(pBlock==NULL)
		return;
	if(pBlock->IsEmbeded())
	{

	}
	else 
	{
		//���ݲ������öκż���ĺŸ��²����ڹ����Ķκż���ĺ�
		pBlockRef->UpdateBlockSegNoAndCfgWord();
		IModel *pOldModel=m_pModel;
		m_pModel=pBlock;
		SummarizeParts();		//ͳ�ƹ��������ڵĲ���
		SummarizePartGroup();	//ͳ���麸��
		m_pModel=pOldModel;
	}
}
#endif