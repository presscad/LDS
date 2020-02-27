//<LOCALE_TRANSLATE BY wbt />
#include "StdAfx.h"
#include "env_def.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "GlobalFunc.h"
#include "EmendBoltGasket.h"
#include "LogFile.h"
#include "SortFunc.h"
#include "VertexsLifeObject.h"
#include "Hash64Table.h"

//#ifdef __SMART_DRAW_INC_
BOLT_RELA_PART::BOLT_RELA_PART(CLDSPart* pRelaPart/*=NULL*/)
{
	_pRelaPart=pRelaPart;
	hPart=pRelaPart!=NULL?pRelaPart->handle:0;
	_zDepth=siThick=0;
}
bool BOLT_RELA_PART::IsWasherPlate()
{
	if(_pRelaPart==NULL||!_pRelaPart->IsPlate())
		return false;
	return ((CLDSGeneralPlate*)_pRelaPart)->wiType==CLDSPlate::WASHER_PLATE;
}
CLDSPart* BOLT_RELA_PART::get_pPart()
{
	if(_pRelaPart!=NULL&&_pRelaPart->handle==hPart)
		return _pRelaPart;
	return _pRelaPart=Ta.Parts.FromHandle(hPart);
}
void BOLT_RELA_PART::set_pPart(CLDSPart* pRelaPart)
{
	_pRelaPart=pRelaPart;
	this->hPart=pRelaPart!=NULL?pRelaPart->handle:0;
}
static double CalSignedDistance(f3dPoint startPt,f3dPoint endPt,f3dPoint norm)
{
	if((endPt-startPt)*norm<0)
		return DISTANCE(startPt,endPt)*-1;
	else 
		return DISTANCE(startPt,endPt);
}
//���㹹���м�㵽��˨ԭ��ľ��룬���ݾ����С�������򲢽�������浽�б���
static BOOL CalAndSortIntersPoint(ATOM_LIST<INTERS_POINT> &interPointList,CLDSPart *pPart,CLDSBolt* pBolt);
static void InitBoltGasketThickAndOffset(ATOM_LIST<INTERS_POINT> &InterPointList,CBoltGasket* pBoltGasket);//��ʼ����˨����Ȧ�ĺ�Ⱥ�ƫ�ƾ���Ϣ
CLDSLineAngle *GetBoltGasketCutAngle(CLDSBolt* pBolt);	//�õ���˨�������нǵĽǸ�
static CLDSNode* GetCrossNode(CHash64Set<CLDSNode*> &hashCrossNodes,CBoltGasket* pBoltGasket1,CBoltGasket* pBoltGasket2);//�ж���˨��׼��������˼��Ƿ���ڵ�������˼���ͬ
static int compare_func1(const BOLTGASKET_PTR &pGasket1,const BOLTGASKET_PTR &pGasket2)	//ͬһ��׼�����ϵ���˨����ȽϺ���
{	return int(pGasket1->boltLengthPos-pGasket2->boltLengthPos); }
static CLDSPlate* DesignCrossPlate(CRawGasket *pRawGasket);	//��ƽ�����
static void AppendOutputInfoToList(GASKET_WARNING_RECORD* pRecordPart,CTaskOutputList* pInfoList);//�������Ϣ���浽�����Ϣ�б���

WORD CBoltGasket::get_wiCurrDianQuanThick()
{
	if(pBolt==NULL)
		return 0;
	if(pBolt->DianQuan.N>0&&pBolt->DianQuan.thick>0)
		return pBolt->DianQuan.N*pBolt->DianQuan.thick;
	return 0;
}
WORD CBoltGasket::get_wiCurrGasketThick()
{
	for(SmartPartPtr pSmartPart=hashRelaPartPtrs.GetFirst();pSmartPart.IsHasPtr();pSmartPart=hashRelaPartPtrs.GetNext())
	{
		if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSPlate::WASHER_PLATE)
			return (WORD)pSmartPart->GetThick();
	}
	return 0;
}
GASKET_WARNING_RECORD* CEmendBoltGasket::GetWarningRecord(long hBolt)
{
	GASKET_WARNING_RECORD* pWarningRecord=hashErrorRecords.GetValue(hBolt);
	if(pWarningRecord==NULL)
		pWarningRecord=hashErrorRecordsView.GetValue(hBolt);
	return pWarningRecord;
}
//1������������˨��������Ҫ��ӵ�Ȧ�������˨����������˨����Ȧ�ĺ�Ⱥ�λ��
void CEmendBoltGasket::InitGasketBoltSet(bool advanceMode/*=true*/)
{ 
	SmartPartPtr pPart;
	//1.��ʼ��hashBoltGaskets������˨����˨�Ĺ����������浽hashBoltGaskets��
	hashBoltGaskets.Empty();
	if(advanceMode)
	{	//����TAP�е���˨ͨ���϶ʶ����
		/*
		ARRAY_LIST<DWORD> assmbleBoltIdArr;
		if(DATACENTERCMD_CONTEXT::OBJ_TYPE_SELECTED_OBJS==pContext->m_cOperObjType)
		{
			for(CAssembleBolt *pBoltRec=boltRecSet.GetFirst();pBoltRec;pBoltRec=boltRecSet.GetNext())
				assmbleBoltIdArr.append(pBoltRec->Id);
		}
		if(assmbleBoltIdArr.GetSize()>0)*/
		//	UpdateBoltRelaObjs(assmbleBoltIdArr.m_pData,assmbleBoltIdArr.GetSize());
		//else //һ�����ļ��򿪻����ļ�ʱ�Ѹ�����˨�����ϵ,��ȷ����Ҫ���ֶ����Ҳ�ģ�����Ҽ��˵����ֶ�ִ�� wjh-2017.3.23
			UpdateBoltRelaObjs();
		for(pPart=Ta.Parts.GetFirst(CLS_BOLT);pPart.IsHasPtr();pPart=Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pPart.pBolt->m_bVirtualPart||pPart.pBolt->IsFootNail())
				continue;
			CBoltGasket *pBoltGasket=hashBoltGaskets.GetValue(pPart.pBolt->handle);
			if(pBoltGasket==NULL)
			{
				pBoltGasket = hashBoltGaskets.Add(pPart.pBolt->handle);
				pBoltGasket->pBolt=pPart.pBolt;
			}
		}

		//2.����hashBoltGaskets��ͨ�������󽻵ķ�ʽ����ÿ����˨����ĵ���Ȧ��ȼ�λ��
		for(CBoltGasket* pBoltGasket=hashBoltGaskets.GetFirst();pBoltGasket;pBoltGasket=hashBoltGaskets.GetNext())
		{
			CLDSBolt *pBolt = pBoltGasket->pBolt;
			CBoltRelaPartList* pRelaPartList=this->GetBoltRelaParts(pBolt->handle);
			if(pRelaPartList->GetRelaPartCount()==0)
			{	//������˨�϶�����Ҫ��壬��ʾ��Ϣ�ݲ��ڴ���ʾ�����ɳ���У������
				hashBoltGaskets.DeleteCursor();
				continue;
			}
			else if(pRelaPartList->GetRelaPartCount()==1)
			{	//ɾ��ֻ��һ����ع�������˨, ͬʱ��������ʾ��Ϣ�����¼
				GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add(pBoltGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_HOLE_LIKELY;
				pRecordPart->pBolt=pBoltGasket->pBolt;
				BOLT_RELA_PART* pRelaPartInfo=pRelaPartList->EnumFirstRelaPart();
				pRecordPart->pAttachPart=pRelaPartInfo->pPart;
				hashBoltGaskets.DeleteCursor();
				continue;
			}
			//����interPointList����ʼ������Ƭ�ĺ�Ⱥ�ƫ�ƾ�
			if(!pRelaPartList->InitializeBoltGasket(pBoltGasket))
				hashBoltGaskets.DeleteCursor();	//��ȷ������Ҫ����Ȧ(��)����˨ɾ��
		}
	}
	else
	{
		for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_PLATE&&pPart.pPlate->wiType==CLDSPlate::WASHER_PLATE)//jdb_style==4)
				continue;

			CLsRefList *pLsRefList=pPart->GetLsRefList();
			if(pLsRefList==NULL)
				continue;
			for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(pBolt==NULL)
					continue;
				if(pBolt->m_bVirtualPart||pBolt->IsFootNail())
					continue;
				CBoltGasket *pBoltGasket=hashBoltGaskets.GetValue(pBolt->handle);
				if(pBoltGasket==NULL)
				{
					pBoltGasket = hashBoltGaskets.Add(pBolt->handle);
					pBoltGasket->pBolt=pBolt;
				}
				if(pBoltGasket->hashRelaPartPtrs.GetValue(pPart->handle)==NULL)
					pBoltGasket->hashRelaPartPtrs.SetValue(pPart->handle,pPart);
			}
		}

		//2.����hashBoltGaskets��ͨ�������󽻵ķ�ʽ����ÿ����˨����ĵ���Ȧ��ȼ�λ��
		for(CBoltGasket* pBoltGasket=hashBoltGaskets.GetFirst();pBoltGasket;pBoltGasket=hashBoltGaskets.GetNext())
		{
			CLDSBolt *pBolt = pBoltGasket->pBolt;
			if(pBolt==NULL)
				continue;
			//ɾ��ֻ��һ����ع�������˨
			if(pBoltGasket->hashRelaPartPtrs.GetNodeNum()==1)
			{
				GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add(pBoltGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_HOLE_LIKELY;
				pRecordPart->pBolt=pBoltGasket->pBolt;
				pRecordPart->pAttachPart=pBoltGasket->hashRelaPartPtrs.GetFirst();
				hashBoltGaskets.DeleteCursor();
				continue;
			}
			BOOL bInvalid = FALSE;
			ATOM_LIST<INTERS_POINT> interPointList;
			for(pPart=pBoltGasket->hashRelaPartPtrs.GetFirst();pPart.IsHasPtr();pPart=pBoltGasket->hashRelaPartPtrs.GetNext())
			{	//������˨�Ĺ��������м�㵽��˨ԭ��ľ��룬�����ݾ����С�������򣬽�������浽interPointList	
				if(pPart->IsPlate()&&pPart.pBasicPlate->wiType==CLDSPlate::WASHER_PLATE)
					continue;
				if(!CalAndSortIntersPoint(interPointList,pPart,pBolt))
				{
					//logerr.Log("error");
					bInvalid=TRUE;
					break;
				}
			}
			if(bInvalid)
				hashBoltGaskets.DeleteCursor();
			else
			{
				//����interPointList����ʼ������Ƭ�ĺ�Ⱥ�ƫ�ƾ�
				InitBoltGasketThickAndOffset(interPointList,pBoltGasket);
				if(pBoltGasket->gasketThick<=CEmendBoltGasket::MINSPACING)	//������Ҫ����Ȧ����˨ɾ��
					hashBoltGaskets.DeleteCursor();
			}
		}
	}
	hashBoltGaskets.Clean();
}
//3�������Ѵ��ڵĵ�壬����������˨��hashBoltGaskets���Ƴ�
void CEmendBoltGasket::EmendExistingBoltPadSet(bool advanceMode/*=true*/)
{
	//�������иְ壬��ʼ���Ѵ��ڵ���б� jdb_style==4Ϊ��˨���
	existingBoltPadSet.Empty();
	for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if(pPlate->wiType==CLDSPlate::WASHER_PLATE)
			existingBoltPadSet.append(pPlate);
	}
	//��gasketSetɾ�����Ѵ��ڵ���Ǻϻ��������������˨
	if(existingBoltPadSet.GetNodeNum()<1)
		return;
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	for(CLDSPlate* pPlate=(CLDSPlate*)existingBoltPadSet.GetFirst();pPlate;pPlate=(CLDSPlate*)existingBoltPadSet.GetNext())
	{
		CLsRef* pLsRef;
		CBoltGasket *pBoltGasket=NULL;
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			pBoltGasket = hashBoltGaskets.GetValue((*pLsRef)->handle);
			if(pBoltGasket!=NULL)
				break;
		}
		//��׵�Ƭ��ʱ��ȡ��һ����˨�ļ�϶��Ϊ�жϱ�׼
		if(pBoltGasket!=NULL)
		{
			GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pBoltGasket->pBolt->handle);
			if(fabs(pPlate->GetThick()-pBoltGasket->gasketThick)>MINMODIFYDIST)//�������
			{
				pRecordPart->thick = FindPlankThick(pBoltGasket->gasketThick);
				//
				pRecordPart->style=ERROR_GASKET_THICK;
				if(!advanceMode)	//�߼�ģʽʱ����ǰ���������ǽ����û������������������
					pPlate->Thick=pRecordPart->thick;
				pRecordPart->pBolt=pBoltGasket->pBolt;
				pRecordPart->pGasketPlate=pPlate;
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{	//������׼��˨�����⣬���������˨�ĵ��Ԥ����Ϣ
					if(pLsRef->pBolt->handle==pBoltGasket->pBolt->handle)
						continue;
					hashBoltGaskets.DeleteNode((*pLsRef)->handle);
					hashErrorRecords.DeleteNode((*pLsRef)->handle);
					hashErrorRecordsView.SetValue((*pLsRef)->handle,pRecordPart);
				}
			}
			else
			{	//�˴����е��,�ҵ���ȸպ�,����Ҫ����
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					hashErrorRecordsView.DeleteNode((*pLsRef)->handle);
					hashErrorRecords.DeleteNode((*pLsRef)->handle);
				}
				continue;
			}
			if(pPlate->GetLsCount()>1&&pPlate->vertex_list.GetNodeNum()==4)	//���ھ��ε��Ҫ�ж��Ƿ�����нǴ���
			{
				CLDSLineAngle* pCutAngle=GetBoltGasketCutAngle(pBoltGasket->pBolt);
				CVertexsLifeObject manageVertexs(pPlate->vertex_list);
				if(pView->CutBoltPadByAngle(pPlate,pCutAngle))	//�������нǴ���
				{
					GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pBoltGasket->pBolt->handle);
					pRecordPart->style=ERROR_BOLTPAD_CUTANGLE;
					pRecordPart->pBolt=pBoltGasket->pBolt;
					pRecordPart->pGasketPlate=pPlate;
					pRecordPart->pCutAngle=pCutAngle;
				}
				else	//δ�������θı�
					manageVertexs.DisableBackup();
			}
		}
		else 
		{
			GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pPlate->handle);
			pRecordPart->style=ERROR_REDUNDANT_GASKET;
			pRecordPart->pGasketPlate=pPlate;
			existingBoltPadSet.DeleteCursor();
		}
	}
	hashBoltGaskets.Clean();
	existingBoltPadSet.Clean();
}
//4����ʼ��δ�ӹ��ĵ�弯�ϣ�����ʼ�����ģʽ(���ס���׻��׽������)
void CEmendBoltGasket::InitRawGasketSet()
{
	hashRawGaskets.Empty();
	//����ڵ㼯�ϣ�keyֵ�������˼����
	CHash64Set<CLDSNode*> hashCrossNodes;
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->m_cPosCalType==4)//���˼������
		{
			DUALKEY dualkey(pNode->arrRelationPole[0],pNode->arrRelationPole[1],true);
			if(hashCrossNodes.GetValue(dualkey)==NULL)
				hashCrossNodes.SetValue(dualkey,pNode);
		}
	}
	//1��������ͬһ����׼�����ϵ���˨����,��������˨�ڻ�׼������λ�ý�������
	//�����׼�����ǽǸ�,����ͬһ֫���ߵ���˨����
	//��������:1.����;2.�������׼��������,�������������;3.��������
	CHashStrList<CDatumPartBolts> partBoltsHashHandle;	//�ǽǸֹ����Ծ��Ϊ��ֵ���Ǹ��Ծ��+����֫
	CDatumPartBolts* pDatumPartBolts=NULL;
	//1�����ݻ�׼�������Ͷ���˨���з��飨�����׼�����ǽǸ֣���ͬһ֫�����ϵ���˨���ϣ�
	for(CBoltGasket *pGasket=hashBoltGaskets.GetFirst();pGasket;pGasket=hashBoltGaskets.GetNext())
	{
		char key[100]="";
		long hPart=pGasket->pBolt->des_base_pos.hPart;
		CLDSPart* pDatumPart = Ta.FromPartHandle(hPart);
		f3dPoint norm,origin_pos;
		//�ݽ�֧�ֽǸ֣����๹����ʹ�õ�����
		if(pDatumPart->IsAngle())
		{	//Ӧ��Ǹ�����ͶӰ������֤���е���ͬһֱ���ϣ�Ȼ���������
			CLDSLineAngle* pAngle = (CLDSLineAngle*)pDatumPart;
			int x0_y1=2;
			pAngle->getUCS(pAngle->ucs);
			pAngle->IsLsInWing(pGasket->pBolt,&x0_y1);
			if(x0_y1==0)
			{
				sprintf(key,"0x%d+X",hPart);
				norm=pAngle->get_norm_x_wing();
			}
			else if(x0_y1==1)
			{	
				sprintf(key,"0x%d+Y",hPart);
				norm=pAngle->get_norm_y_wing();
			}
			origin_pos=pDatumPart->ucs.origin;
			pDatumPartBolts=partBoltsHashHandle.GetValue(key);
			if(pDatumPartBolts==NULL)
			{
				pDatumPartBolts=partBoltsHashHandle.Add(key);
				pDatumPartBolts->datum_origin=origin_pos;
				pDatumPartBolts->norm=norm;
			}
			f3dPoint ls_pos=pGasket->pBolt->ucs.origin;
			f3dLine ber_line;
			ber_line.startPt=pAngle->Start();
			ber_line.endPt=pAngle->End();
			project_point(ls_pos,origin_pos,norm);
			SnapPerp(&ls_pos,ber_line,ls_pos);
			pGasket->boltLengthPos=DISTANCE(ls_pos,origin_pos);
			pDatumPartBolts->boltGasketSet.append(pGasket);
		}
		else if(pDatumPart->GetClassTypeId()==CLS_PLATE)
		{	//��׼����Ϊ�ְ�ʱӦ�ȼ�������������ο�,Ȼ���򳤱�ͶӰ,������ͶӰ�������
			CLDSPlate* pPlate=(CLDSPlate*)pDatumPart;
			norm=pPlate->ucs.axis_z;
		}
	}
	//2����ͬһ��׼�����ϵ���˨����λ�ý�������
	for(pDatumPartBolts=partBoltsHashHandle.GetFirst();pDatumPartBolts;pDatumPartBolts=partBoltsHashHandle.GetNext())
	{
		ARRAY_LIST<BOLTGASKET_PTR> boltGasketArr;
		boltGasketArr.SetSize(0,pDatumPartBolts->boltGasketSet.GetNodeNum());
		for(CBoltGasket* pGasket=pDatumPartBolts->boltGasketSet.GetFirst();pGasket;pGasket=pDatumPartBolts->boltGasketSet.GetNext())
			boltGasketArr.append(pGasket);
		CQuickSort<BOLTGASKET_PTR>::QuickSort(boltGasketArr.m_pData,boltGasketArr.GetSize(),compare_func1);
		pDatumPartBolts->boltGasketSet.Empty();
		for(int i=0;i<boltGasketArr.GetSize();i++)
			pDatumPartBolts->boltGasketSet.append(boltGasketArr[i]);
	}
	//3�����ݻ�׼������������˨�ľ���ȷ����Ƭģʽ
	CRawGasket* pRawGasket=NULL;
	for(pDatumPartBolts=partBoltsHashHandle.GetFirst();pDatumPartBolts;pDatumPartBolts=partBoltsHashHandle.GetNext())
	{
		if(pDatumPartBolts->boltGasketSet.GetNodeNum()==1)	//��׼��������֫��ֻ�е�����˨
		{
			CBoltGasket* pBoltPad=pDatumPartBolts->boltGasketSet.GetFirst();
			pRawGasket=hashRawGaskets.Append();
			pBoltPad->feature=1;
			pRawGasket->boltGasketSet.SetValue(pBoltPad->pBolt->handle,pBoltPad);
			int type=CLDSBolt::CalPadPara(pBoltPad->pBolt->get_d(),pBoltPad->gasketThick);
			if(type==DIANQUAN_STRU::DIANBAN)
			{	
				pRawGasket->type=CRawGasket::ONEHOLE;//���׵��
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pBoltPad->pBolt);
			}
			else if(type==DIANQUAN_STRU::DIANQUAN)
				pRawGasket->type=CRawGasket::WASHER;//��Ȧ
			continue;
		}
		f3dPoint prebolt_origin,curbolt_origin;
		CBoltGasket* pPrePadBolt=pDatumPartBolts->boltGasketSet.GetFirst();
		prebolt_origin=pPrePadBolt->pBolt->ucs.origin;
		project_point(prebolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
		pRawGasket=hashRawGaskets.Append();
		pRawGasket->boltGasketSet.SetValue(pPrePadBolt->pBolt->handle,pPrePadBolt);
		pRawGasket->pCutAngle=GetBoltGasketCutAngle(pPrePadBolt->pBolt);
		for(CBoltGasket* pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext();pCurPadBolt;pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext())
		{
			curbolt_origin=pCurPadBolt->pBolt->ucs.origin;
			project_point(curbolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
			if(DISTANCE(prebolt_origin,curbolt_origin)<BOLTSPACE_THRESHOLD)//����
			{
				if(pRawGasket->boltGasketSet.GetValue(pPrePadBolt->pBolt->handle)==NULL)
				{
					pPrePadBolt->feature=1;//��ʶ�Ѿ��ж�����˨�����ĵ��ģʽ
					pRawGasket->boltGasketSet.SetValue(pPrePadBolt->pBolt->handle,pPrePadBolt);
				}
				if(pRawGasket->boltGasketSet.GetValue(pCurPadBolt->pBolt->handle)==NULL)
				{
					pCurPadBolt->feature=1;
					pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				}
				if(pRawGasket->type==0)
				{
					CLDSNode* pCrossNode=GetCrossNode(hashCrossNodes,pPrePadBolt,pCurPadBolt);
					if(pCrossNode)
					{
						pRawGasket->type=CRawGasket::CROSSMULTIHOLE;	//������
						pRawGasket->pCrossNode=pCrossNode;
					}
					else 
						pRawGasket->type=CRawGasket::MULTIHOLE;//���
				}
			}
			else
			{
				if(pRawGasket->boltGasketSet.GetNodeNum()==1)
				{
					int type=CLDSBolt::CalPadPara(pPrePadBolt->pBolt->get_d(),pPrePadBolt->gasketThick);
					if(type==DIANQUAN_STRU::DIANBAN)
						pRawGasket->type=CRawGasket::ONEHOLE;//���׵��
					else if(type==DIANQUAN_STRU::DIANQUAN)
						pRawGasket->type=CRawGasket::WASHER;//��Ȧ
				}
				pRawGasket=hashRawGaskets.Append();
				pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pCurPadBolt->pBolt);
				if(pDatumPartBolts->boltGasketSet.GetCurrentIndex()==pDatumPartBolts->boltGasketSet.GetNodeNum()-1)	//�������һ����˨
				{	
					int type=CLDSBolt::CalPadPara(pCurPadBolt->pBolt->get_d(),pCurPadBolt->gasketThick);
					if(type==DIANQUAN_STRU::DIANBAN)
						pRawGasket->type=CRawGasket::ONEHOLE;//���׵��
					else if(type==DIANQUAN_STRU::DIANQUAN)
						pRawGasket->type=CRawGasket::WASHER;//��Ȧ
				}
			}
			pPrePadBolt=pCurPadBolt;
			prebolt_origin=curbolt_origin;
		}
	}
}
void CEmendBoltGasket::InitRawGasketSetAdvance()
{
	hashRawGaskets.Empty();
	//����ڵ㼯�ϣ�keyֵ�������˼����
	CHash64Set<CLDSNode*> hashCrossNodes;
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->m_cPosCalType!=4)//�����˼������
			continue;
		DUALKEY dualkey(pNode->arrRelationPole[0],pNode->arrRelationPole[1],true);
		if(hashCrossNodes.GetValue(dualkey)==NULL)
			hashCrossNodes.SetValue(dualkey,pNode);
	}
	//1��������ͬһ����׼�����ϵ���˨����,��������˨�ڻ�׼������λ�ý�������
	//�����׼�����ǽǸ�,����ͬһ֫���ߵ���˨����
	//��������:1.����;2.�������׼��������,�������������;3.��������
	CHash64List<CDatumPartBolts> hashPartBoltSet;	//�ǽǸֹ����Ծ��Ϊ��ֵ���Ǹ��Ծ��+����֫
	CDatumPartBolts* pDatumPartBolts=NULL;
	//1�����ݻ�׼�������Ͷ���˨���з��飨�����׼�����ǽǸ֣���ͬһ֫�����ϵ���˨���ϣ�
	for(CBoltGasket *pGasket=hashBoltGaskets.GetFirst();pGasket;pGasket=hashBoltGaskets.GetNext())
	{
		long hPart=pGasket->pBolt->des_base_pos.hPart;
		CAssemblePart* pAssmPart=this->FromAssmPartHandle(hPart);
		//�ݽ�֧�ֽǸ֣����๹����ʹ�õ�����
		if(pAssmPart->pSmartPart.IsNULL())
			continue;
		if(pAssmPart->pSmartPart->IsAngle())
		{	//Ӧ��Ǹ�����ͶӰ������֤���е���ͬһֱ���ϣ�Ȼ���������
			CLDSPart::HOLE* pHole=pAssmPart->GetBoltHole(pGasket->pBolt->handle);
			DUALKEY dualkey(pHole->ciResidePlane,pAssmPart->hPart);
			pDatumPartBolts=hashPartBoltSet.Add(dualkey);
			pDatumPartBolts->datum_origin=pAssmPart->pSmartPart.pAngle->Start();
			pDatumPartBolts->norm=pHole->ciResidePlane==0?pAssmPart->pSmartPart.pAngle->vxWingNorm:pAssmPart->pSmartPart.pAngle->vyWingNorm;
			pGasket->boltLengthPos=pHole->xLocalPosition.x;	//��ʱ�洢���ں����������
			pDatumPartBolts->boltGasketSet.append(pGasket);
		}
		else if(pAssmPart->pSmartPart->GetClassTypeId()==CLS_PLATE)
		{	//��׼����Ϊ�ְ�ʱӦ�ȼ�������������ο�,Ȼ���򳤱�ͶӰ,������ͶӰ�������
			//norm=pAssmPart->pSmartPart->ucs.axis_z;
		}
	}
	//2����ͬһ��׼�����ϵ���˨����λ�ý�������
	for(pDatumPartBolts=hashPartBoltSet.GetFirst();pDatumPartBolts;pDatumPartBolts=hashPartBoltSet.GetNext())
	{
		ARRAY_LIST<BOLTGASKET_PTR> boltGasketArr;
		boltGasketArr.SetSize(0,pDatumPartBolts->boltGasketSet.GetNodeNum());
		for(CBoltGasket* pGasket=pDatumPartBolts->boltGasketSet.GetFirst();pGasket;pGasket=pDatumPartBolts->boltGasketSet.GetNext())
			boltGasketArr.append(pGasket);
		CQuickSort<BOLTGASKET_PTR>::QuickSort(boltGasketArr.m_pData,boltGasketArr.GetSize(),compare_func1);
		pDatumPartBolts->boltGasketSet.Empty();
		for(int i=0;i<boltGasketArr.GetSize();i++)
			pDatumPartBolts->boltGasketSet.append(boltGasketArr[i]);
	}
	//3�����ݻ�׼������������˨�ľ���ȷ����Ƭģʽ
	CRawGasket* pRawGasket=NULL;
	GASKET_WARNING_RECORD* pWarnRecord;
	for(pDatumPartBolts=hashPartBoltSet.GetFirst();pDatumPartBolts;pDatumPartBolts=hashPartBoltSet.GetNext())
	{
		if(pDatumPartBolts->boltGasketSet.GetNodeNum()==1)	//��׼��������֫��ֻ�е�����˨
		{
			CBoltGasket* pBoltPad=pDatumPartBolts->boltGasketSet.GetFirst();
			pWarnRecord=hashErrorRecords.GetValue(pBoltPad->pBolt->handle);
			if(pWarnRecord==NULL)
			{	//����Ҫ�κ��޶�
				hashBoltGaskets.DeleteNode(pBoltPad->pBolt->handle);
				hashPartBoltSet.DeleteCursor();
				continue;
			}
			if(pWarnRecord->style!=ERROR_MISSING_CROSSGASKET&&pWarnRecord->style!=ERROR_MISSING_ENDGASKET)
				continue;	//�˴�����Ҫ���ӵ���Ȧ
			pRawGasket=hashRawGaskets.Append();
			pBoltPad->feature=1;
			pRawGasket->boltGasketSet.SetValue(pBoltPad->pBolt->handle,pBoltPad);
			int type=CLDSBolt::CalPadPara(pBoltPad->pBolt->get_d(),pBoltPad->gasketThick);
			if(type==DIANQUAN_STRU::DIANBAN)
			{	
				pRawGasket->type=CRawGasket::ONEHOLE;//���׵��
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pBoltPad->pBolt);
			}
			else if(type==DIANQUAN_STRU::DIANQUAN)
				pRawGasket->type=CRawGasket::WASHER;//��Ȧ
			continue;
		}
		f3dPoint prebolt_origin,curbolt_origin;
		CBoltGasket* pPrePadBolt;
		for(pPrePadBolt=pDatumPartBolts->boltGasketSet.GetFirst();pPrePadBolt!=NULL;pPrePadBolt=pDatumPartBolts->boltGasketSet.GetNext())
		{
			pWarnRecord=GetWarningRecord(pPrePadBolt->pBolt->handle);
			if(pWarnRecord==NULL||(pWarnRecord->style!=ERROR_MISSING_CROSSGASKET&&pWarnRecord->style!=ERROR_MISSING_ENDGASKET))
				continue;
			pRawGasket=hashRawGaskets.Append();
			pRawGasket->boltGasketSet.SetValue(pPrePadBolt->pBolt->handle,pPrePadBolt);
			pRawGasket->pCutAngle=GetBoltGasketCutAngle(pPrePadBolt->pBolt);
			break;
		}
		if(pPrePadBolt==NULL)
			continue;	//��ǰ�˼�����֫ƽ�����˨����Ҫ��ӵ��
		prebolt_origin=pPrePadBolt->pBolt->ucs.origin;
		project_point(prebolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
		for(CBoltGasket* pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext();pCurPadBolt;pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext())
		{
			curbolt_origin=pCurPadBolt->pBolt->ucs.origin;
			project_point(curbolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
			pWarnRecord=GetWarningRecord(pCurPadBolt->pBolt->handle);
			if(pWarnRecord==NULL||(pWarnRecord->style!=ERROR_MISSING_CROSSGASKET&&pWarnRecord->style!=ERROR_MISSING_ENDGASKET))
				pRawGasket=NULL;
			else if(pRawGasket==NULL)
			{
				pRawGasket=hashRawGaskets.Append();
				pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pCurPadBolt->pBolt);
			}
			else if(DISTANCE(prebolt_origin,curbolt_origin)<BOLTSPACE_THRESHOLD)//����
			{
				if(pRawGasket->boltGasketSet.GetValue(pCurPadBolt->pBolt->handle)==NULL)
				{
					pPrePadBolt->feature=1;//��ʶ�Ѿ��ж�����˨�����ĵ��ģʽ
					pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				}
				if(pRawGasket->type==0)
				{
					CLDSNode* pCrossNode=GetCrossNode(hashCrossNodes,pPrePadBolt,pCurPadBolt);
					if(pCrossNode)
					{
						pRawGasket->type=CRawGasket::CROSSMULTIHOLE;	//������
						pRawGasket->pCrossNode=pCrossNode;
					}
					else 
						pRawGasket->type=CRawGasket::MULTIHOLE;//���
				}
			}
			else
			{
				pRawGasket=hashRawGaskets.Append();
				pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pCurPadBolt->pBolt);
			}
			pPrePadBolt=pCurPadBolt;
			prebolt_origin=curbolt_origin;
		}
	}
	for(pRawGasket=hashRawGaskets.GetFirst();pRawGasket;pRawGasket=hashRawGaskets.GetNext())
	{
		CBoltGasket* pDatumGasket=pRawGasket->boltGasketSet.GetFirst();
		GASKET_WARNING_RECORD* pErrRec=hashErrorRecords.Add(pDatumGasket->pBolt->handle);
		pErrRec->thick=pDatumGasket->gasketThick;
		pErrRec->idRawGasket=pRawGasket->Id;
		if(pRawGasket->boltGasketSet.GetNodeNum()==1)
		{
			int type=CLDSBolt::CalPadPara(pDatumGasket->pBolt->get_d(),pDatumGasket->gasketThick);
			if(type==DIANQUAN_STRU::DIANBAN)
				pRawGasket->type=CRawGasket::ONEHOLE;//���׵��
			else if(type==DIANQUAN_STRU::DIANQUAN)
				pRawGasket->type=CRawGasket::WASHER;//��Ȧ
		}
		else
		{	//ͬһ��������������һ������ʾ��Ϣ
			for(CBoltGasket* pNextGasket=pRawGasket->boltGasketSet.GetNext();pNextGasket;pNextGasket=pRawGasket->boltGasketSet.GetNext())
			{
				hashErrorRecords.DeleteNode(pNextGasket->pBolt->handle);
				hashErrorRecordsView.SetValue(pNextGasket->pBolt->handle,pErrRec);
			}
		}
	}
}
//5����������rawGasketSet��������Ƚ�С����˨���е�Ȧ������Ȼ���Ƴ���
void CEmendBoltGasket::EmendBoltWasherInfo()
{
	if(hashBoltGaskets.GetNodeNum()<1)
		return;
	for(CRawGasket* pRaw=hashRawGaskets.GetFirst();pRaw;pRaw=hashRawGaskets.GetNext())
	{
		if(pRaw->type!=CRawGasket::WASHER)
			continue;
		CBoltGasket* pBoltGasket=pRaw->boltGasketSet.GetFirst();
		DIANQUAN_STRU oldDianQuan=pBoltGasket->pBolt->DianQuan;
		pBoltGasket->pBolt->CalPadPara(pBoltGasket->gasketThick);
		pBoltGasket->pBolt->DianQuan.offset=pBoltGasket->washerOffset;
		if(!(oldDianQuan.N==pBoltGasket->pBolt->DianQuan.N&&
			oldDianQuan.thick==pBoltGasket->pBolt->DianQuan.thick))
		{
			GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add(pBoltGasket->pBolt->handle);
			pRecordPart->pPart = pBoltGasket->pBolt;
			if(oldDianQuan.N>0&&oldDianQuan.thick>0)//������Ȧ
			{
				pRecordPart->style= EMEND_GASKET;
				pRecordPart->gasketNum=oldDianQuan.N;
				pRecordPart->thick = oldDianQuan.thick;
			}
			else	//������Ȧ
				pRecordPart->style=NEW_GASKET;
			pBoltGasket->pBolt->SetModified();
			pBoltGasket->pBolt->Create3dSolidModel();
		}
		hashRawGaskets.DeleteCursor();
	}
	hashRawGaskets.Clean();
}
//6������δ�ӹ���Ƭ,����ӵ�newGasketSet��
void CEmendBoltGasket::CreateGaskets()
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pPlate=NULL;
	for(CRawGasket* pRaw=hashRawGaskets.GetFirst();pRaw;pRaw=hashRawGaskets.GetNext())
	{
		CBoltGasket* pFirstBoltGasket=pRaw->boltGasketSet.GetFirst();
		if(pRaw->type!=CRawGasket::CROSSMULTIHOLE)
		{
			BOLTSET boltSet;
			for(CBoltGasket *pBoltGasket=pRaw->boltGasketSet.GetFirst();pBoltGasket;pBoltGasket=pRaw->boltGasketSet.GetNext())
			{
				if(pBoltGasket->pBolt)
				{
					boltSet.append(pBoltGasket->pBolt);
					if(pBoltGasket->pBolt->DianQuan.N>0 ||pBoltGasket->pBolt->DianQuan.thick>0)
					{	//��ȦӦ����Ϊ��崦������ͬ��׼�����ĵ�Ȧ �� ��Ȧ��ȹ���
						pBoltGasket->pBolt->DianQuan.N=0;
						pBoltGasket->pBolt->DianQuan.thick=0;
					}
				}
			}
			CLDSPart *pDatumPart=Ta.Parts.FromHandle(pFirstBoltGasket->pBolt->des_base_pos.hPart);
			pPlate=pView->DesignBoltPad(pDatumPart,pRaw->pCutAngle,boltSet,pFirstBoltGasket->gasketThick,pFirstBoltGasket->gasketOffset);
		}
		else
			pPlate=DesignCrossPlate(pRaw);
		GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pFirstBoltGasket->pBolt->handle);
		pRecordPart->pBolt=pFirstBoltGasket->pBolt;	
		if(pPlate)
		{
			hashNewGaskets.SetValue(pPlate->handle,pPlate);
			pRecordPart->style=NEW_BOLTPAD;
			pRecordPart->pPart=pPlate;
		}
		else 
		{
			GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pFirstBoltGasket->pBolt->handle);
			pRecordPart->style=EMEND_OTHER;
		}
	}
}
//7��.�ϲ�ͬһ����ģʽ��ͬ�ĵ��
void CEmendBoltGasket::MergeSimilarGasketInSameSeg()
{
	//1.������˨���
	ATOM_LIST<CLDSPlate*> boltPadList;
	for(CLDSPlate** ppPlate=hashNewGaskets.GetFirst();ppPlate;ppPlate=hashNewGaskets.GetNext())
	{
		if((*ppPlate)->iSeg.iSeg==0 )
			continue;	//������0�ι���
		boltPadList.append(*ppPlate);
	}
	for(CLDSPlate* pPlate=existingBoltPadSet.GetFirst();pPlate;pPlate=existingBoltPadSet.GetNext())
	{
		if(pPlate->iSeg.iSeg<=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The segment number of new generation plate 0X%X is undefined!",pPlate->handle);
#else 
			logerr.Log("δ�������ɵ��0X%X�ĶκŸ�ֵ!",pPlate->handle);
#endif
			continue;	//������0�ι���
		}
		boltPadList.append(pPlate);
	}
	//2.���ݲ�ͬ�ĵ��ģʽ�Ե����з���
	CHashList<CBoltPadProperty> platePropertyHashId;
	ATOM_LIST<SAMESCHEMA_BOLTPAD> sameSchemaBoltPadList;
	for(CLDSPlate** ppBoltPad=boltPadList.GetFirst();ppBoltPad;ppBoltPad=boltPadList.GetNext())
	{
		CBoltPadProperty *pPadProp = platePropertyHashId.Add((*ppBoltPad)->handle);
		pPadProp->InitProperty(*ppBoltPad);
		if(pPadProp->m_iType==CBoltPadProperty::SPECIALBOLTPAD)
		{
			GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add((*ppBoltPad)->handle);
			pRecordPart->style=ERROR_SPECIAL_GASKET;
			pRecordPart->pPart=(*ppBoltPad);
			continue;
		}
		//MODE->SCHEMA
		SAMESCHEMA_BOLTPAD *pSameSchemaBoltPad=NULL;
		for(pSameSchemaBoltPad=sameSchemaBoltPadList.GetFirst();pSameSchemaBoltPad;pSameSchemaBoltPad=sameSchemaBoltPadList.GetNext())
		{	
			if(pSameSchemaBoltPad->IsSameSpec(pPadProp)==1)
			{	//������ͬģʽ�µĵ�壬���䶥����Ϣ��������ƥ����
				pPadProp->MarkProfileVertexs();
				break;
			}
		}
		if(pSameSchemaBoltPad==NULL)
		{
			pPadProp->MarkProfileVertexs();
			pSameSchemaBoltPad=sameSchemaBoltPadList.append();
			pSameSchemaBoltPad->pDatumProperty=pPadProp;
		}
		pSameSchemaBoltPad->boltPadSet.SetValue((*ppBoltPad)->handle,(*ppBoltPad));
	}
	//3.���ھ�����ͬģʽ�ĵ�壬����С����ԭ��ͳһ�������ζ�������
	for(SAMESCHEMA_BOLTPAD *pSimBoltPad=sameSchemaBoltPadList.GetFirst();pSimBoltPad;pSimBoltPad=sameSchemaBoltPadList.GetNext())
	{
		if(pSimBoltPad->boltPadSet.GetNodeNum()==1)
			continue;
		//1 ��ȡ��С���ζ�������
		CBoltPadProperty *pBoltPadProp=NULL,*pDatumProperty=pSimBoltPad->pDatumProperty;
		CXhChar16 sPartNo;
		for(CLDSPlate **ppPlate=pSimBoltPad->boltPadSet.GetFirst();ppPlate;ppPlate=pSimBoltPad->boltPadSet.GetNext())
		{
			if((*ppPlate)->handle==pDatumProperty->m_pBoltPad->handle)
				continue;
			pBoltPadProp=platePropertyHashId.GetValue((*ppPlate)->handle);
			pBoltPadProp->UpdateMinProfileVertex(pDatumProperty);
			if(sPartNo.GetLength()<=0&&strlen((*ppPlate)->GetPartNo())>0)
				sPartNo.Copy((*ppPlate)->GetPartNo());
		}
		//2 �������ζ�������
		for(CLDSPlate **ppPlate=pSimBoltPad->boltPadSet.GetFirst();ppPlate;ppPlate=pSimBoltPad->boltPadSet.GetNext())
		{
			pBoltPadProp=platePropertyHashId.GetValue((*ppPlate)->handle);
			if(pBoltPadProp==NULL)
				continue;
			if(sPartNo.GetLength()>0)
				(*ppPlate)->SetPartNo(sPartNo);
			BOOL bRetCode = pBoltPadProp->EmendProfileVertexs(pDatumProperty);
			if(bRetCode && hashNewGaskets.GetValue((*ppPlate)->handle)==NULL)
			{	//�������иְ�����
				//TODO:����ʹ���˸ְ�������Ժ���Ҫ�Ľ�
				GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add((*ppPlate)->handle);
				pRecordPart->style=ERROR_GASKET_PROFILE;
				pRecordPart->pPart=(*ppPlate);	
			}
		}
	}
}
//ִ�е���Զ������޶�
void CEmendBoltGasket::Execute(DATACENTERCMD_CONTEXT *pContext,bool advanceMode/*=true*/)
{
	CLogErrorLife loglife(&logerr);
	InitGasketBoltSet(advanceMode);			//����������˨��������Ҫ��ӵ�Ȧ�������˨	
	if(advanceMode)
	{
		StatWireAngles();
		for(CBoltGasket* pGasket=hashBoltGaskets.GetFirst();pGasket;pGasket=hashBoltGaskets.GetNext())
		{
			long hErrPosAngle=0;
			BYTE ciErrorCode=ValidateTheroyGasket(pGasket,&hErrPosAngle);
			if(ciErrorCode==CBoltRelaPartList::TYPE_SUCCESS)	//0.�޶��ɹ�
				continue;
			if(ciErrorCode==CBoltRelaPartList::TYPE_NONE_CONNECTED)	//1.�޹������� 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_ISOLATED_BOLT;
				pRecordPart->pBolt=pGasket->pBolt;	
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_VIRTUAL_BOLT)	//2.�ǽŶ���˨������һ������ 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_HOLE_LIKELY;
				pRecordPart->pBolt=pGasket->pBolt;	
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_WIRE_VIRTUAL_BOLT)	//7.���߿׼���˨
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_WIREHOLE_LIKELY;
				pRecordPart->pAttachPart=FromPartHandle(pGasket->pBolt->des_base_pos.hPart);
				pRecordPart->pBolt=pGasket->pBolt;	
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_MISS_CROSSED_PAD)	//3.����㴦�ٵ�Ƭ 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_MISSING_CROSSGASKET;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->thick=pGasket->gasketThick;
				pRecordPart->pAttachPart=FromPartHandle(pGasket->pBolt->des_base_pos.hPart);
				pRecordPart->gasketOffset=pGasket->gasketOffset;
				pRecordPart->washerOffset=pGasket->washerOffset;
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_MISS_END_PAD)	//4.��ͷ��Ҫ�����˨��Ƭ 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_MISSING_ENDGASKET;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->thick=pGasket->gasketThick;
				pRecordPart->pAttachPart=FromPartHandle(pGasket->pBolt->des_base_pos.hPart);
				pRecordPart->gasketOffset=pGasket->gasketOffset;
				pRecordPart->washerOffset=pGasket->washerOffset;
				//pRecordPart->pCutAngle=GetBoltGasketCutAngle(pGasket->pBolt);	//��Ϊ��Ƶ��ʱʵʱ��ȡ
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_ERROR_GASKET_THICK)	//6.��Ȧ��Ȳ���ȷ 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_GASKET_THICK;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->thick=pGasket->gasketThick;
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_PART_POS_ERROR)	//5.����λ�ô��� 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_PART_POS;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->pAttachPart=this->FromPartHandle(hErrPosAngle);
			}
		}
	}
	EmendExistingBoltPadSet();		//�����Ѵ��ڵĵ�壬����������˨��hashBoltGaskets���Ƴ�
	if(advanceMode)
	{
		InitRawGasketSetAdvance();
		MergeSimilarGasketInSameSeg();	//�ϲ�ͬһ������������ĵ�Ƭ
	}
	else
	{
		InitRawGasketSet();				//��ʼ��δ�ӹ��ĵ�Ƭ���ϣ�����ʼ�����ģʽ(���ס���׻��׽������)
		EmendBoltWasherInfo();			//��������rawGasketSet�е�����Ƚ�С����˨������Ȧ��Ϣ��Ȼ���Ƴ�
		CreateGaskets();				//����δ�ӹ���Ƭ,����ӵ�newGasketSet��
		MergeSimilarGasketInSameSeg();	//�ϲ�ͬһ������������ĵ�Ƭ
	}
}
//��ʼ�������Ϣ�б�
void CEmendBoltGasket::InitOutputInfoList(CTaskOutputList* pInfoList)
{
	//1.��������˨��Ƭ
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if( pRecordPart->feature==0&&(pRecordPart->style==NEW_BOLTPAD||
			pRecordPart->style==ERROR_MISSING_CROSSGASKET||pRecordPart->style==ERROR_MISSING_ENDGASKET))
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//2.��˨��Ƭ����޸�
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_GASKET_THICK&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//3.��˨��Ƭ�нǴ���
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_BOLTPAD_CUTANGLE&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//4.��˨��Ƭ��������(�����нǺ����˨��Ƭ)
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_GASKET_PROFILE&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//5.��������˨��Ȧ
	/*for(RECORD_PART* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==NEW_GASKET&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//6.��˨��Ȧ����
	for(RECORD_PART* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==EMEND_GASKET&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}*/
	//7.��˨��Ƭ����
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_REDUNDANT_GASKET&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//8.ֻ����һ����������˨
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if((pRecordPart->style==ONE_PART_BOLT||pRecordPart->style==ERROR_VIRTUAL_HOLE_LIKELY||
			pRecordPart->style==ERROR_VIRTUAL_WIREHOLE_LIKELY)&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//9.
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_SPECIAL_GASKET && pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//10.����
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
}
BOOL CalAndSortIntersPoint(ATOM_LIST<INTERS_POINT> &interPointList,CLDSPart *pPart,CLDSBolt* pBolt)
{
	if(pPart==NULL)
		return FALSE;
	//1������ָ������˨���ߣ��õ������м��ʰȡ�㼰���߷���
	f3dPoint face_pick,face_norm,bolt_norm;
	bolt_norm=pBolt->ucs.axis_z;
	if(pPart->GetClassTypeId()==CLS_PLATE)
	{	//��ͨ�ְ�
		CLDSPlate *pDatumPlate=(CLDSPlate*)pPart;
		/*int iFace=pDatumPlate->GetNormFaceI(bolt_norm);
		if(iFace==0)
		{
			face_pick = pDatumPlate->ucs.origin;
			face_norm = pDatumPlate->ucs.axis_z;
		}
		else if(iFace==1||iFace==2)
		{
			face_pick=pDatumPlate->HuoQuLine[iFace-1].startPt;
			coord_trans(face_pick,pDatumPlate->ucs,TRUE);
			face_norm = pDatumPlate->huoqufacenorm[iFace-1];
		}
		else
			return FALSE;
		face_pick+=pDatumPlate->ucs.axis_z*(pDatumPlate->m_fNormOffset+0.5*pDatumPlate->GetThick());
		*/
		int iResidePlane=0;
		if(!pDatumPlate->GetBoltIntersPos(pBolt,face_pick,&iResidePlane,CLDSPlate::DATUM_PLANE))
			return FALSE;
		face_norm=iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(iResidePlane);
		face_pick+=face_norm*(0.5*pDatumPlate->GetThick());
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{	//��������
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
		double dd1=fabs(pParamPlate->ucs.axis_z*bolt_norm);
		if(dd1<EPS_COS2)
			return FALSE;
		if(pParamPlate->m_iParamType==TYPE_UEND||
			pParamPlate->m_iParamType==TYPE_XEND||
			pParamPlate->m_iParamType==TYPE_ROLLEND)
			return FALSE;	//U�͡���߲�塢ʮ�ֲ�岻��Ҫ��ӵ�Ƭ
		else 
			face_pick=pParamPlate->ucs.origin;
		face_norm = pParamPlate->ucs.axis_z;
	}
	else if(pPart->IsAngle())
	{
		CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pPart;
		double dd1=fabs(pLineAngle->get_norm_x_wing()*bolt_norm);
		double dd2=fabs(pLineAngle->get_norm_y_wing()*bolt_norm);
		if(dd1>dd2&&dd1>EPS_COS2)
			face_norm = pLineAngle->get_norm_x_wing();
		else if(dd2>dd1&&dd2>EPS_COS2)
			face_norm = pLineAngle->get_norm_y_wing();
		else 
			return FALSE;
		face_pick=pLineAngle->Start()-face_norm*pLineAngle->GetThick()*0.5;
	}
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
	{
		CLDSLineSlot *pSlot=(CLDSLineSlot*)pPart;
		face_norm = pSlot->WorkPlaneNorm();
		pSlot->BuildUCS();
		face_pick=pSlot->ucs.origin-face_norm*pSlot->GetThick()*0.5;
		double dd1=fabs(face_norm*bolt_norm);
		if(dd1<EPS_COS2)
			return FALSE;
	}
	else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
	{
		CLDSLineFlat *pFlat=(CLDSLineFlat*)pPart;
		face_norm = pFlat->WorkPlaneNorm();
		pFlat->BuildUCS();
		face_pick=pFlat->ucs.origin+face_norm*pFlat->GetThick()*0.5;
		double dd1=fabs(face_norm*bolt_norm);
		if(dd1<EPS_COS2)
			return FALSE;
	}
	//2�����㹹���м�㵽��˨ԭ��ľ��룬����������
	f3dPoint inter_pos;
	f3dLine line(pBolt->ucs.origin,pBolt->ucs.origin+pBolt->ucs.axis_z*1000);
	if(Int3dlf(inter_pos,line,face_pick,face_norm)!=1)
		return FALSE;
	INTERS_POINT interPt(pPart,inter_pos,CalSignedDistance(pBolt->ucs.origin,inter_pos,pBolt->ucs.axis_z));
	INTERS_POINT* pInters;
	for(pInters=interPointList.GetFirst();pInters;pInters=interPointList.GetNext())
	{
		if(pInters->distance > interPt.distance)
		{
			interPointList.insert(interPt);
			break;
		}
	}
	if(pInters==NULL)
		interPointList.append(interPt);
	return TRUE;
}
void InitBoltGasketThickAndOffset(ATOM_LIST<INTERS_POINT> &interPointList,CBoltGasket* pBoltGasket)
{
	if(interPointList.GetNodeNum()<=0)
		return;
	INTERS_POINT* pDatumIntersPoint=NULL;
	int iDatumIndex=-1;
	for(INTERS_POINT* pInters=interPointList.GetFirst();pInters;pInters=interPointList.GetNext())
	{
		iDatumIndex++;
		if(pBoltGasket->pBolt->des_base_pos.hPart==pInters->pPart->handle)
		{
			pDatumIntersPoint=pInters;
			break;
		}
	}
	if(pDatumIntersPoint==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("It can't find the datum part by the bolt's reference",pBoltGasket->pBolt->handle);
#else 
		logerr.Log("���ݴ���˨����0X%X�Ҳ�����׼����",pBoltGasket->pBolt->handle);
#endif
		return;
	}
	f3dPoint norm;
	if(pDatumIntersPoint->pPart->IsAngle())
	{
		CLDSLineAngle* pAngle = (CLDSLineAngle*)pDatumIntersPoint->pPart;
		int x0_y1=2;
		if(pAngle->IsLsInWing(pBoltGasket->pBolt,&x0_y1))
		{	
			if(x0_y1==0)
				norm=pAngle->get_norm_x_wing();
			else if(x0_y1==1)
				norm=pAngle->get_norm_y_wing();
		}
	}
	else
		norm=pDatumIntersPoint->pPart->ucs.axis_z;
	INTERS_POINT* pPrevPt = interPointList.GetFirst();
	for(INTERS_POINT *pCurPt = interPointList.GetNext();pCurPt;pCurPt=interPointList.GetNext())
	{
		double fFullDist,fExistDist,fSpacing;
		fFullDist=DISTANCE(pPrevPt->inters_pt,pCurPt->inters_pt);
		fExistDist=pPrevPt->GetHalfThick()+pCurPt->GetHalfThick();
		fSpacing=fFullDist-fExistDist;
		if(fabs(fSpacing)<CEmendBoltGasket::MINSPACING)
		{
			pPrevPt=pCurPt;
			continue;	//�����ǽӽ�0�ļ�϶ֵ
		}
		if(fSpacing > pBoltGasket->gasketThick)
		{
			pBoltGasket->gasketThick = fSpacing;
			//��壺ƫ�����Ի�׼��������,��Ƭ�������׼��������һ��
			//TODO: δ�����巨�����׼�������߲�һ�����
			//TODO: ����д�ɶ�������,��ͬʱ��ɸú����Ĳ��Դ���
			int index=interPointList.GetCurrentIndex();
			if(norm*pBoltGasket->pBolt->ucs.axis_z>0)	//��˨�������׼��������ͬ��
			{
				double halfThick2 = pCurPt->GetHalfThick()+pDatumIntersPoint->GetHalfThick();
				if(iDatumIndex>=index)
					pBoltGasket->gasketOffset=-DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)-pBoltGasket->gasketThick-halfThick2;
				else
					pBoltGasket->gasketOffset=DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)-pBoltGasket->gasketThick-halfThick2;
			}
			else	//��˨�������׼�������߷���
			{
				double halfThick2 = pCurPt->GetHalfThick()-pDatumIntersPoint->GetHalfThick();
				if(iDatumIndex>=index)
					pBoltGasket->gasketOffset=DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)+halfThick2;
				else  
					pBoltGasket->gasketOffset=-DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)+halfThick2;
			}
			//��������ĵ��ƫ�ƾ඼���ԽǸ�Ϊ��׼�����ģ������׼�������ǽǸ֣�ƫ�ƾ໹���ػ�׼�������߷���ƫ��һ�����
			if(!pDatumIntersPoint->pPart->IsAngle())
				pBoltGasket->gasketOffset+=pDatumIntersPoint->pPart->GetThick();
			//�� typeӦ����ΪDIANQUAN::�µľ�̬���� 
			if(CLDSBolt::CalPadPara(pBoltGasket->pBolt->get_d(),fSpacing)==DIANQUAN_STRU::DIANQUAN) //��Ȧ:ƫ��������˨����,��Ȧ���ߺ���˨����һ��
			{
				f3dPoint pos=pBoltGasket->pBolt->ucs.origin;
				pBoltGasket->washerOffset=DISTANCE(pCurPt->inters_pt,pos)-fFullDist+pPrevPt->GetHalfThick();
			}
		}
		pPrevPt=pCurPt;
	}
}
CLDSLineAngle *GetBoltGasketCutAngle(CLDSBolt* pBolt)
{
	CLDSPart *pDatumPart=Ta.Parts.FromHandle(pBolt->des_base_pos.hPart);
	if(pDatumPart->IsLinePart())
	{
		CLDSLinePart *pRod=(CLDSLinePart*)pDatumPart;
		double fStartDist=DISTANCE(pBolt->ucs.origin,pRod->Start());
		double fEndDist=DISTANCE(pBolt->ucs.origin,pRod->End());
		CLDSNode *pNode=NULL;
		if(fStartDist>fEndDist&&pRod->pEnd)
			pNode=pRod->pEnd;
		else if(pRod->pStart)
			pNode=pRod->pStart;
		else 
			return NULL;
		CLDSLineAngle *pCutAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pNode->hFatherPart,CLS_LINEANGLE);
		if(pCutAngle&&pCutAngle->handle!=pDatumPart->handle)
			return pCutAngle;
	}
	return NULL;
}
CLDSNode* GetCrossNode(CHash64Set<CLDSNode*> &hashCrossNodes,CBoltGasket* pBoltGasket1,CBoltGasket* pBoltGasket2)
{
	if(pBoltGasket1->pBolt->des_base_pos.datumPoint.datum_pos_style!=3||
		pBoltGasket1->pBolt->des_base_pos.datumPoint.datum_pos_style!=3)
		return NULL;
	THANDLE handle1_1 =pBoltGasket1->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
	THANDLE handle1_2 =pBoltGasket1->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
	THANDLE handle2_1 =pBoltGasket2->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
	THANDLE handle2_2 =pBoltGasket2->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
	if((handle1_1==handle2_1&&handle1_2==handle2_2)||(handle1_1==handle2_2&&handle1_2==handle2_1))
	{
		DUALKEY dualkey(handle1_1,handle1_2,true);
		return hashCrossNodes.GetValue(dualkey);
	}
	return NULL;
}
//TODO: ������CDesignJdb::DesignCrossPlank��iProfileCalType01==2ʱ������м���
CLDSPlate* DesignCrossPlate(CRawGasket *pRawGasket)
{
	if(pRawGasket==NULL||pRawGasket->type!=3||pRawGasket->pCrossNode==NULL)
		return NULL;
	CLDSNode *pBaseNode=pRawGasket->pCrossNode;
	CBoltGasket *pBoltGasket=pRawGasket->boltGasketSet.GetFirst();
	if(pBoltGasket==NULL)
		return NULL;
	// 1.��ȡ��׼�Ǹּ�����Ǹ֣���֫��ϴ�ĽǸ�Ϊ��׼�Ǹ�
	CLDSLineAngle *pBaseAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pBaseNode->arrRelationPole[0],CLS_LINEANGLE);
	CLDSLineAngle *pAnotherJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pBaseNode->arrRelationPole[1],CLS_LINEANGLE);
	if(pAnotherJg==NULL||pBaseAngle==NULL||pBaseAngle->handle==pAnotherJg->handle)
		return FALSE;
	// 2.�ж�������Ǹ��Ƿ����
	BOOL IsJgCollide=FALSE;
	double base_wing_wide = pBaseAngle->GetWidth();
	double other_wing_wide = pAnotherJg->GetWidth();
	double ddXX = pBaseAngle->get_norm_x_wing()*pAnotherJg->get_norm_x_wing();
	double ddXY = pBaseAngle->get_norm_x_wing()*pAnotherJg->get_norm_y_wing();
	double ddYX = pBaseAngle->get_norm_y_wing()*pAnotherJg->get_norm_x_wing();
	double ddYY = pBaseAngle->get_norm_y_wing()*pAnotherJg->get_norm_y_wing();
	f3dPoint axis_z;
	int norm_wing=0;
	if(IsMaxInAll(ddXX,ddXY,ddYX,ddYY))
	{
		if(ddXX>0)	//������ĽǸ��໥����
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_x_wing();
		norm_wing=0;
	}
	else if(IsMaxInAll(ddXY,ddXX,ddYX,ddYY))
	{
		if(ddXY>0)	//������ĽǸ��໥����
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_x_wing();
		norm_wing=0;
	}
	else if(IsMaxInAll(ddYX,ddXY,ddXX,ddYY))
	{
		if(ddYX>0)	//������ĽǸ��໥����
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_y_wing();
		norm_wing=1;
	}
	else// if(IsMaxInAll(ddYY,ddXY,ddXX,ddYX))
	{
		if(ddYY>0)	//������ĽǸ��໥����
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_y_wing();
		norm_wing=1;
	}
	if(IsJgCollide)	//������ĽǸ��໥����
		return NULL;
	// 3.���ɽ������˨��岢���ò���
	// 3.1 ������˨��岢���û�����Ϣ
	CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pPlate->face_N = 1;
	pPlate->cMaterial = pBaseAngle->cMaterial;
	pPlate->Thick=FindPlankThick(pBoltGasket->gasketThick);
	pPlate->iSeg=pBaseAngle->iSeg;
	pPlate->jdb_style = 4;	//������˨���
	pPlate->m_fNormOffset = ftoi(pBoltGasket->gasketOffset);	//����ƫ����
	// 3.2. ������˨��������Ϣ
	pPlate->designInfo.m_bEnableFlexibleDesign=FALSE;
	pPlate->designInfo.iFaceType=1;
	pPlate->designInfo.iProfileStyle0123=3; 
	pPlate->designInfo.m_hBaseNode = pBaseNode->handle;
	pPlate->designInfo.m_hBasePart=pBaseNode->arrRelationPole[0];
	pPlate->designInfo.norm.norm_style=1;
	pPlate->designInfo.norm.norm_wing=norm_wing;
	pPlate->designInfo.norm.hVicePart=pBaseAngle->handle;
	pPlate->designInfo.norm.direction=0;
	pPlate->designInfo.origin.datum_pos_style=3;
	pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1=pBaseAngle->handle;
	pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2=pAnotherJg->handle;
	// 3.3. �����������������ϵ
	pBaseNode->GetIntersZhun(&pPlate->ucs.origin);
	f3dPoint perp;
	SnapPerp(&perp,pBaseAngle->Start(),pBaseAngle->End(),pPlate->ucs.origin,NULL);
	project_point(pPlate->ucs.origin,perp,axis_z);
	pPlate->ucs.axis_z = axis_z;
	pPlate->ucs.axis_y = pBaseAngle->End()-pBaseAngle->Start();
	pPlate->ucs.axis_x = pPlate->ucs.axis_y^pPlate->ucs.axis_z;
	pPlate->ucs.axis_y = pPlate->ucs.axis_z^pPlate->ucs.axis_x;
	normalize(pPlate->ucs.axis_x);
	normalize(pPlate->ucs.axis_y);
	normalize(pPlate->ucs.axis_z);
	// 3.4. �����˨����
	for(CBoltGasket *pBoltGasket=pRawGasket->boltGasketSet.GetFirst();pBoltGasket;pBoltGasket=pRawGasket->boltGasketSet.GetNext())
	{
		CLDSBolt *pBolt=pBoltGasket->pBolt;
		if(pBolt==NULL)
			continue;
		pBolt->DianQuan.N=0;
		pBolt->DianQuan.thick=0;
		pPlate->AppendLsRef(pBolt->GetLsRef());
		pBolt->AddL0Thick(pPlate->handle,TRUE);
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	//4. ���ɽ�����˨���
	if(!pPlate->DesignRhombusPlate())
		return NULL;
	pPlate->SetModified();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pPlate;
}
void CEmendBoltGasket::AppendOutputInfoToList(GASKET_WARNING_RECORD* pRecordPart,CTaskOutputList* pInfoList)
{
	if(pRecordPart->pBolt==NULL&&pRecordPart->style==0)
	{
		logerr.Log("error");
		return;
	}
	CCheckGasketErrorInfo* pInfo=(CCheckGasketErrorInfo*)pInfoList->Add(0,3);
	pRecordPart->feature=1;
	pInfo->hRelaBolt=pRecordPart->pBolt!=NULL?pRecordPart->pBolt->handle:0;
	pInfo->hRelaGasket=pRecordPart->pGasketPlate!=NULL?pRecordPart->pGasketPlate->handle:0;
	pInfo->m_uType=pRecordPart->style;
	//if(pRecordPart->style==ERROR_ISOLATED_BOLT)		//�޹��������Ĺ�����˨, ����ʽΪ:���Ի�ȷ�����
	//	;
	//if(pRecordPart->style==ERROR_REDUNDANT_GASKET)	//�����壬����ʽΪ��������
	//	;
	if( pRecordPart->style==ERROR_MISSING_CROSSGASKET||	//����㴦���ȱʧ������ʽΪ����ӵ����Ȧ
		pRecordPart->style==ERROR_MISSING_ENDGASKET)	//�����Ӵ����ȱʧ������ʽΪ����ӵ����Ȧ
	{
		CRawGasket* pRawGasket=this->hashRawGaskets.GetValue(pRecordPart->idRawGasket);
		if(pRawGasket==NULL)
			return;	//TODO:������Ӧִ�е��˴� 2017.12.8
		CBoltGasket* pGasket=pRawGasket->boltGasketSet.GetFirst();
		if(pGasket)
			pInfo->siGasketThick=(short)ftoi(pGasket->gasketThick);
		for(;pGasket;pGasket=pRawGasket->boltGasketSet.GetNext())
			pInfo->relaObjIds.Append(pGasket->pBolt->handle);
		if(pRecordPart->pAttachPart)
			pInfo->anchor.hAnchorPart=pRecordPart->pAttachPart->handle;
		pInfo->anchor.gasketOffset=pRecordPart->gasketOffset;
		pInfo->anchor.washerOffset=pRecordPart->washerOffset;
	}
	else if(pRecordPart->style==ERROR_GASKET_THICK)
	{
		pInfo->siGasketThick=(short)(pRecordPart->thick+0.5);
	}
	else if(pRecordPart->style==ERROR_VIRTUAL_HOLE_LIKELY||pRecordPart->style==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{
		if(pRecordPart->pAttachPart)
			pInfo->m_hRelaObj2=pRecordPart->pAttachPart->handle;	//���еĹ�������
	}
	else if(pRecordPart->style==ERROR_PART_POS)			//����λ�ô���
	{
		if(pRecordPart->pAttachPart)
			pInfo->m_hRelaObj2=pRecordPart->pAttachPart->handle;
	}
	else if(pRecordPart->style==ERROR_BOLTPAD_CUTANGLE)	//���ȱ���нǴ���
	{
		if(pRecordPart->pCutAngle)
			pInfo->relaObjIds.Append(pRecordPart->pCutAngle->handle);
	}
	else if(pRecordPart->style==ERROR_SPECIAL_GASKET)	//������
	{
		if(pRecordPart->pGasketPlate)
			pInfo->relaObjIds.Append(pRecordPart->pGasketPlate->handle);
	}
	//else if(pRecordPart->style==ERROR_GASKET_PROFILE)		//������β����ʣ�����ʽΪ�������������
	//	;
	pInfo->SetMessage(pRecordPart->GetDescription());
};
//
GASKET_WARNING_RECORD::GASKET_WARNING_RECORD()
{
	idDatumBolt=0;
	style=EMENDGASKET_TYPE(0);
	thick=0;
	gasketNum=0;
	pPart=pBolt=NULL;
	pAttachPart=NULL;
	pCutAngle=NULL;
	gasketOffset=0;//����Ի�׼��˨�ڻ�׼�����ϵĴ��彻���ƫ��ֵ������ȷ�����λ�ã���׼�����Ĺ�������Ϊƫ������)
	washerOffset=0;//��Ȧ����˨��ƫ�ƾ�
	feature=idRawGasket=0;
}
CXhChar50 GASKET_WARNING_RECORD::GetDescription()
{
	if(style==ERROR_ISOLATED_BOLT)		//�޹��������Ĺ�����˨, ����ʽΪ:���Ի�ȷ�����
		return CXhChar50("0x%XΪ���������˨",pBolt->handle);
	else if(style==ERROR_VIRTUAL_HOLE_LIKELY||style==ONE_PART_BOLT)	//Ŀǰ��һ�����������ķǽŶ���������ף�����ʽΪ��ȷ����Ϊ������˨��
	{
#ifdef AFX_TARG_ENU_ENGLISH
		return CXhChar50("0x%X bolt connects only one part. It's a dummy bolt.",pBolt->handle);
#else 
		return CXhChar50("0x%XΪ���������ӵļ���˨",pBolt->handle);
#endif
	}
	else if(style==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		return CXhChar50("0x%X bolt is wire hole. It's a dummy bolt.",pBolt->handle);
#else 
		return CXhChar50("0x%XΪ���߿׼���˨",pBolt->handle);
#endif
	}
	else if(style==ERROR_MISSING_CROSSGASKET)	//����㴦���ȱʧ������ʽΪ����ӵ����Ȧ
		return CXhChar50("0x%X��˨���������(Ȧ)ȱʧ(-%g)",pBolt->handle,this->thick);
	else if(style==ERROR_MISSING_ENDGASKET)	//�����Ӵ����ȱʧ������ʽΪ����ӵ����Ȧ
		return CXhChar50("0x%X��˨����ͷ���(Ȧ)ȱʧ(-%g)",pBolt->handle,this->thick);
	else if(style==ERROR_GASKET_THICK)			//�����Ȧ��ȴ��󣬴���ʽΪ�������޶������Ȧ���
	{
		if(pGasketPlate)
			return CXhChar50("0x%X����ȴ���(%g->%g)",pGasketPlate->handle,pGasketPlate->Thick,this->thick);
		else
			return CXhChar50("0x%X��˨��Ȧ��ȴ���(%d->%g)",pBolt->handle,pBolt->DianQuan.thick*pBolt->DianQuan.N,this->thick);
	}
	else if(style==ERROR_PART_POS)				//����λ�ô���
		return CXhChar50("0x%X��˨����ͷ����λ�ô���",pBolt->handle);
	else if(style==ERROR_BOLTPAD_CUTANGLE)		//���ȱ���нǴ���
		return CXhChar50("0x%X������뿿���Ǹֽ����нǴ���",pGasketPlate->handle);
	else if(style==ERROR_GASKET_PROFILE)		//������β����ʣ�����ʽΪ�������������
		return CXhChar50("0x%X�����������нǵ��ͳһ������",pGasketPlate->handle);
	else if(style==ERROR_REDUNDANT_GASKET)		//�����壬����ʽΪ��������
		return CXhChar50("0x%X�������",pGasketPlate->handle);
	else if(style==ERROR_SPECIAL_GASKET)
		return CXhChar50("0x%X������",pGasketPlate->handle);
	else if(pPart==NULL)
		return "";
	//
	CString sDescription="";
	if(style==NEW_BOLTPAD)	
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("0X%X  create new bolt's gasket",pPart->handle);
	else if(style==ERROR_GASKET_THICK)
		sDescription.Format("Correct gasket's thickness of 0X%X bolt:%d->%d",pPart->handle,ftoi(thick),ftoi(pPart->GetThick()));
	else if(style==ERROR_BOLTPAD_CUTANGLE)
		sDescription.Format("0X%X bolt's gasket cut angle",pPart->handle);
	else if(style==ERROR_GASKET_PROFILE)
		sDescription.Format("Correct the outline of 0x%X bolt's gasket in %s segment",pPart->handle,(char*)pPart->iSeg.ToString());
	else if(style==ERROR_REDUNDANT_GASKET)
		sDescription.Format("0x%X bolt's gasket is redundant",pPart->handle);
#else 
		sDescription.Format("0X%X������˨��Ƭ",pPart->handle);
	else if(style==ERROR_GASKET_THICK)
		sDescription.Format("0X%X��˨��Ƭ�������:%d->%d",pPart->handle,ftoi(thick),ftoi(pPart->GetThick()));
	else if(style==ERROR_BOLTPAD_CUTANGLE)
		sDescription.Format("0X%X��˨��Ƭ�����нǴ���",pPart->handle);
	else if(style==ERROR_GASKET_PROFILE)
		sDescription.Format("%s��0x%X��˨��Ƭ��������",(char*)pPart->iSeg.ToString(),pPart->handle);
	else if(style==ERROR_REDUNDANT_GASKET)
		sDescription.Format("0x%X��˨��ƬΪ���๹��",pPart->handle);
#endif
	else if(style==NEW_GASKET)
	{
		CLDSBolt* pBolt = (CLDSBolt*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("0x%X create new bolt's washer:%d*%d",pPart->handle,pBolt->DianQuan.N,pBolt->DianQuan.thick);
#else 
		sDescription.Format("0x%X������˨��Ȧ:%d*%d",pPart->handle,pBolt->DianQuan.N,pBolt->DianQuan.thick);
#endif
	}
	else if(style==EMEND_GASKET)
	{
		CLDSBolt* pBolt = (CLDSBolt*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("Correct 0x%X bolt's washer:%d*%d->%d*%d",pBolt->handle,gasketNum,ftoi(thick),pBolt->DianQuan.N,pBolt->DianQuan.thick);
#else 
		sDescription.Format("0x%X��˨��Ȧ����:%d*%d->%d*%d",pBolt->handle,gasketNum,ftoi(thick),pBolt->DianQuan.N,pBolt->DianQuan.thick);
#endif
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("0x%X can't be corrected ",pPart->handle);
#else 
		sDescription.Format("0x%X�޷���������",pPart->handle);
#endif
	return CXhChar50((const char*)sDescription);
}
CBoltPadProperty::CBoltPadProperty()
{
	m_pBoltPad=NULL;
	m_iSignIndex=-1;
	m_iType=-1;
}
void CBoltPadProperty::InitProperty(CLDSPlate* pPlate)
{	
	m_pBoltPad=pPlate;
	double cos90=cos(0.5*Pi);
	//ɾ������е��ظ�����
	PROFILE_VERTEX* pVertex=NULL,*pNextVertex=NULL;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		int nPush=pPlate->vertex_list.push_stack();
		for(pNextVertex=pPlate->vertex_list.GetNext();pNextVertex;pNextVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->vertex.IsEqual(pNextVertex->vertex))
				pPlate->vertex_list.DeleteCursor();
		}
		pPlate->vertex_list.pop_stack(nPush);
	}
	pPlate->vertex_list.Clean();
	//ͳ�Ƶ������ֱ�Ƕ������������б��
	int nRectangularNum=0,n=pPlate->vertex_list.GetNodeNum();
	vertexItemArr.SetSize(0,n);
	for(int i=0;i<n;i++)
	{
		VERTEX_ITEM *pVertexItem=vertexItemArr.append();
		pVertexItem->pVertex=&(pPlate->vertex_list[i]);
		pVertexItem->bRectangular=FALSE;	//ͨ��append()��ӵĶ�������bRectangular=TRUE,�˴�Ҫ���г�ʼ��
		f3dPoint vec_cur = pPlate->vertex_list[(i+1)%n].vertex-pPlate->vertex_list[i].vertex;
		f3dPoint vec_prev= pPlate->vertex_list[(n+i-1)%n].vertex-pPlate->vertex_list[i].vertex;
		double cosa = vec_cur.normalized()*vec_prev.normalized();
		if(fabs(cosa-cos90)<EPS)
		{
			pVertexItem->bRectangular=TRUE;
			nRectangularNum++;
		}
	}
	if(nRectangularNum==3)
	{	//��������ֱ�Ƕ���ĵ�����нǵ��
		m_iType=CUTANGELBOLTPAD;
		InitCutAnglePadProperty();
	}
	else if(nRectangularNum==4)	
	{	//�����ĸ�ֱ�Ƕ���ĵ���Ǿ��ε��
		m_iType=RECTANGLEBOLTPAD;
		InitRectanglePadProperty();
	}
	else
	{	//��������(���ε�������������)
		m_iType=SPECIALBOLTPAD;
	}
}
void CBoltPadProperty::InitCutAnglePadProperty()
{
	//1�����ҵڶ���ֱ�Ƕ��㣬�Դ˶���Ϊԭ�㣬�Գ���ΪX�ᣬ�̱�ΪY�Ὠ������ϵ
	UCS_2dSTRU ucs2d;
	int n=vertexItemArr.GetSize();
	for(int i=0;i<n;i++)
	{
		VERTEX_ITEM *pPrevItem=&vertexItemArr[(n+i-1)%n];
		VERTEX_ITEM *pCurItem=&vertexItemArr[i%n];
		VERTEX_ITEM *pNextItem=&vertexItemArr[(i+1)%n];
		if(pPrevItem->bRectangular&&pCurItem->bRectangular&&pNextItem->bRectangular)
		{	
			f3dPoint curVec=pNextItem->pVertex->vertex-pCurItem->pVertex->vertex;
			double fCurLen=curVec.mod();
			normalize(curVec);
			f3dPoint preVec=pPrevItem->pVertex->vertex-pCurItem->pVertex->vertex;
			double fPreLen=preVec.mod();
			normalize(preVec);
			if(fCurLen>fPreLen)
			{
				ucs2d.axis_x.Set(curVec.x,curVec.y);
				ucs2d.axis_y.Set(preVec.x,preVec.y);
			}
			else
			{
				ucs2d.axis_x.Set(preVec.x,preVec.y);
				ucs2d.axis_y.Set(curVec.x,curVec.y);
			}
			ucs2d.origin.Set(pCurItem->pVertex->vertex.x,pCurItem->pVertex->vertex.y);
			m_iSignIndex=i;
			break;
		}
	}
	//2��������ϵ���˨����ת������������ϵ�£�ȷ�����½���˨����ƥ��ģʽ����ϵ
	ATOM_LIST<f2dPoint>	boltVertexList;
	double fMinX=100,fMinY=100;
	f3dPoint ls_3dPos;
	f2dPoint vertex2d;
	for(CLsRef* pLsRef=m_pBoltPad->GetFirstLsRef();pLsRef;pLsRef=m_pBoltPad->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		Int3dlf(ls_3dPos,pBolt->ucs.origin,pBolt->ucs.axis_z,m_pBoltPad->ucs.origin,m_pBoltPad->ucs.axis_z);
		coord_trans(ls_3dPos,m_pBoltPad->ucs,FALSE);
		vertex2d.Set(ls_3dPos.x,ls_3dPos.y);
		coord_trans(vertex2d,ucs2d,FALSE);
		if(vertex2d.x<=fMinX+0.5 && vertex2d.y<=fMinY+0.5)
		{
			fMinX=vertex2d.x;
			fMinY=vertex2d.y;
		}
		coord_trans(vertex2d,ucs2d,TRUE);
		vertex2d.feature=pBolt->get_d();	//��¼��˨ֱ��
		boltVertexList.append(vertex2d);
	}
	vertex2d.Set(fMinX,fMinY);
	coord_trans(vertex2d,ucs2d,TRUE);
	modeUcs=ucs2d;
	modeUcs.origin=vertex2d;
	//3�����нǵ����˨ת����ƥ��ģʽ����ϵ��
	boltItemArr.SetSize(0,boltVertexList.GetNodeNum());
	for(f2dPoint* pLsPos=boltVertexList.GetFirst();pLsPos;pLsPos=boltVertexList.GetNext())
	{
		coord_trans(*pLsPos,modeUcs,FALSE);
		BOLT_ITEM* pBoltItem=boltItemArr.append();
		pBoltItem->vertex=*pLsPos;
		pBoltItem->d=pLsPos->feature;
	}
}
void CBoltPadProperty::InitRectanglePadProperty()
{
	//1����������ֱ����ĸ�����Ϊԭ�㡢����ΪX�ᡢ�̱�ΪY�ᣬ��������ϵ
	int n=vertexItemArr.GetSize();
	for(int i=0;i<n;i++)
	{
		f3dPoint curVertex=vertexItemArr[i%n].pVertex->vertex;
		f3dPoint curVec=vertexItemArr[(i+1)%n].pVertex->vertex-curVertex;
		double fCurLen=curVec.mod();
		normalize(curVec);
		f3dPoint preVec=vertexItemArr[(n+i-1)%n].pVertex->vertex-curVertex;
		double fPreLen=preVec.mod();
		normalize(preVec);
		if(fCurLen>=fPreLen)
		{
			cs2d[i].axis_x.Set(curVec.x,curVec.y);
			cs2d[i].axis_y.Set(preVec.x,preVec.y);
		}
		else
		{
			cs2d[i].axis_x.Set(preVec.x,preVec.y);
			cs2d[i].axis_y.Set(curVec.x,curVec.y);
		}
		cs2d[i].origin.Set(curVertex.x,curVertex.y);
	}
	//2��������ϵ���˨����ת������������ϵ�£�ȷ�����½���˨������ƥ��ģʽ����ϵ
	ATOM_LIST<f2dPoint> boltVertexList;
	f3dPoint lsPos;
	f2dPoint vertex2d;
	for(CLsRef* pLsRef=m_pBoltPad->GetFirstLsRef();pLsRef;pLsRef=m_pBoltPad->GetNextLsRef())
	{
		CLDSBolt* pBolt=pBolt=pLsRef->GetLsPtr();
		Int3dlf(lsPos,pBolt->ucs.origin,pBolt->ucs.axis_z,m_pBoltPad->ucs.origin,m_pBoltPad->ucs.axis_z);
		coord_trans(lsPos,m_pBoltPad->ucs,FALSE);
		vertex2d.Set(lsPos.x,lsPos.y);
		vertex2d.feature=pBolt->get_d();
		boltVertexList.append(vertex2d);
	}
	for(int i=0;i<4;i++)
	{
		double fMinX=100,fMinY=100;
		for(f2dPoint* pPoint=boltVertexList.GetFirst();pPoint;pPoint=boltVertexList.GetNext())
		{
			vertex2d=*pPoint;
			coord_trans(vertex2d,cs2d[i],FALSE);
			if(vertex2d.x<=fMinX+0.5 && vertex2d.y<=fMinY+0.5)
			{
				fMinX=vertex2d.x;
				fMinY=vertex2d.y;
			}
		}
		vertex2d.Set(fMinX,fMinY);
		coord_trans(vertex2d,cs2d[i],TRUE);
		cs2d[i].origin=vertex2d;
	}
	//3�����������˨ת����ƥ��ģʽ������,���浽��Ӧ���б���
	for(int i=0;i<4;i++)
	{
		boltItemList[i].Empty();	//��˨���б�����մ����������һ��
		for(f2dPoint* pPoint=boltVertexList.GetFirst();pPoint;pPoint=boltVertexList.GetNext())
		{
			vertex2d=*pPoint;
			coord_trans(vertex2d,cs2d[i],FALSE);
			BOLT_ITEM* pBoltItem=boltItemList[i].append();
			pBoltItem->vertex=vertex2d;
			pBoltItem->d=vertex2d.feature;
		}
	}
	//4����cs2d[0]��Ϊ��׼ģʽ����ϵ��������Ӧ����˨���б��浽boltItemArr��
	m_iSignIndex=0;
	modeUcs=cs2d[0];
	boltItemArr.SetSize(0,boltVertexList.GetNodeNum());
	for(BOLT_ITEM* pBoltItem=boltItemList[0].GetFirst();pBoltItem;pBoltItem=boltItemList[0].GetNext())
		boltItemArr.append(*pBoltItem);
}
//����Ƚ���˨��Ϣ����˨ֱ����ƥ��ģʽ����ϵ�µ���˨���꣬��ȷ�����յ�ƥ������ϵ
BOOL CBoltPadProperty::CompareBoltInfo(CBoltPadProperty* pDatumPadProp)
{
	if(pDatumPadProp==NULL)
		return FALSE;
	int nMatchingNum=0;
	BOLT_ITEM* pBoltItem=NULL,*pDatumBoltItem=NULL;
	if(m_iType==CUTANGELBOLTPAD)	//�нǵ�����˨�ȶ�
	{	
		for(pDatumBoltItem=pDatumPadProp->boltItemArr.GetFirst();pDatumBoltItem;pDatumBoltItem=pDatumPadProp->boltItemArr.GetNext())
		{
			for(pBoltItem=boltItemArr.GetFirst();pBoltItem;pBoltItem=boltItemArr.GetNext())
			{
				if(pDatumBoltItem->d==pBoltItem->d &&fabs(pDatumBoltItem->vertex.x-pBoltItem->vertex.x)<EPS2&&
					fabs(pDatumBoltItem->vertex.y-pBoltItem->vertex.y)<EPS2)
					break;
			}			
			if(pBoltItem==NULL)
				return FALSE;
		}
	}
	else //if(pPadProperty->type==CBoltPadProperty::RECTANGLEBOLTPAD)	//���ε�����˨�ȶ�
	{
		for(int i=0;i<4;i++)
		{
			nMatchingNum=0;
			for(pDatumBoltItem=pDatumPadProp->boltItemArr.GetFirst();pDatumBoltItem;pDatumBoltItem=pDatumPadProp->boltItemArr.GetNext())
			{
				for(pBoltItem=boltItemList[i].GetFirst();pBoltItem;pBoltItem=boltItemList->GetNext())
				{
					if(pDatumBoltItem->d==pBoltItem->d&&fabs(pDatumBoltItem->vertex.x-pBoltItem->vertex.x)<EPS2&&
						fabs(pDatumBoltItem->vertex.y-pBoltItem->vertex.y)<EPS2)
					{
						nMatchingNum++;
						break;
					}
				}
			}
			if(nMatchingNum==pDatumPadProp->boltItemArr.GetSize())
			{
				modeUcs=cs2d[i];
				m_iSignIndex=i;
				return TRUE;
			}
		}
		return FALSE;
	}
	return TRUE;
}
static int compare_func3(const VERTEX_ITEM &vertexItem1,const VERTEX_ITEM &vertexItem2)
{	
	return int(vertexItem1.feature-vertexItem2.feature); 
}
//�Ե�m_iSignVertex�������𣬶Ե�嶥����Ϣ���б��
void CBoltPadProperty::MarkProfileVertexs()
{
	//��������ζ���ת����ģʽ�����²�����
	f2dPoint vertex2d;
	VERTEX_ITEM* pVertexItem;
	for(pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
	{
		vertex2d.Set(pVertexItem->pVertex->vertex.x,pVertexItem->pVertex->vertex.y);
		coord_trans(vertex2d,modeUcs,FALSE);
		pVertexItem->vertex2d=vertex2d;
	}
	//�Ե�m_iSignVertex���㿪ʼ�����еĶ�����б�ǲ�����
	int n=vertexItemArr.GetSize();
	for(pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
	{
		int i=vertexItemArr.GetCurrentIndex();
		if(i<m_iSignIndex)
			pVertexItem->feature=n-m_iSignIndex+i;
		else
			pVertexItem->feature=i-m_iSignIndex;
	}
	CQuickSort<VERTEX_ITEM>::QuickSort(vertexItemArr.m_pData,vertexItemArr.GetSize(),compare_func3);
}
//����ͬһģʽ�µ�����С����
void CBoltPadProperty::UpdateMinProfileVertex(CBoltPadProperty* pDatumPadProp)
{
	if(pDatumPadProp==NULL)
		return;
	VERTEX_ITEM* pDatumVertexItem=NULL;
	for(VERTEX_ITEM* pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
	{
		pDatumVertexItem =pDatumPadProp->vertexItemArr.GetByIndex(pVertexItem->feature);
		if((pVertexItem->vertex2d.x<0 && pDatumVertexItem->vertex2d.x<pVertexItem->vertex2d.x) ||
			(pVertexItem->vertex2d.x>0 && pDatumVertexItem->vertex2d.x>pVertexItem->vertex2d.x))
			pDatumVertexItem->vertex2d.x=pVertexItem->vertex2d.x;
		if((pVertexItem->vertex2d.y<0 && pDatumVertexItem->vertex2d.y<pVertexItem->vertex2d.y) ||
			(pVertexItem->vertex2d.y>0 && pDatumVertexItem->vertex2d.y>pVertexItem->vertex2d.y))
			pDatumVertexItem->vertex2d.y=pVertexItem->vertex2d.y;
	}
}
//������С����ͳһ����������ζ���
BOOL CBoltPadProperty::EmendProfileVertexs(CBoltPadProperty* pDatumPadProp)
{
	BOOL bRet=FALSE;
	VERTEX_ITEM* pDatumVertexItem=NULL,*pVertexItem=NULL;
	if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)	//��׼��屾��
	{
		for(pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
		{
			f2dPoint vertex2d(pVertexItem->vertex2d.x,pVertexItem->vertex2d.y);
			coord_trans(vertex2d,modeUcs,TRUE);
			pVertexItem->pVertex->vertex.Set(vertex2d.x,vertex2d.y,0);
		}
		bRet=TRUE;
	}
	else	//ͬģ���µ��������
	{
		for(VERTEX_ITEM* pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
		{
			pDatumVertexItem=pDatumPadProp->vertexItemArr.GetByIndex(pVertexItem->feature);
			if(fabs(pVertexItem->vertex2d.x-pDatumVertexItem->vertex2d.x)>EPS2 || fabs(pVertexItem->vertex2d.y-pDatumVertexItem->vertex2d.y)>EPS)
			{
				pVertexItem->vertex2d.Set(pDatumVertexItem->vertex2d.x,pDatumVertexItem->vertex2d.y);
				coord_trans(pVertexItem->vertex2d,modeUcs,TRUE);
				pVertexItem->pVertex->vertex.Set(pVertexItem->vertex2d.x,pVertexItem->vertex2d.y,0);
				bRet=TRUE;
			}
		}
	}
	if(bRet)
	{
		m_pBoltPad->RetrieveGasketVertexCalType();
		if(m_pBoltPad->designInfo.iProfileStyle0123==1)
			m_pBoltPad->designInfo.iProfileStyle0123=3;
	}
	return bRet;
}
//�ж�������Ƿ�����ͬһģʽ
int SAMESCHEMA_BOLTPAD::IsSameSpec(CBoltPadProperty* pPadProperty)
{
	if(pPadProperty->m_iType!=pDatumProperty->m_iType)	//�Ƚ�����
		return 0;
	if(pPadProperty->m_pBoltPad->iSeg!=pDatumProperty->m_pBoltPad->iSeg)	//�Ƚ϶κ�
		return 0;
	if(pPadProperty->m_pBoltPad->GetLsCount()!=pDatumProperty->m_pBoltPad->GetLsCount())	//�Ƚ���˨��
		return 0;	
	if(pPadProperty->m_pBoltPad->GetThick()!=pDatumProperty->m_pBoltPad->GetThick())	//�Ƚϰ��
		return 0;
	if(!pPadProperty->CompareBoltInfo(pDatumProperty))	//�Ƚ���˨��Ϣ
		return 0;
	return 1;
}
#ifdef __ALFA_TEST_VERSION_
void CBoltPadProperty::Test_InitProperty(int index,CLDSPlate* pBoltPad)
{
	InitProperty(pBoltPad);
	if(index==0)
	{
		if(modeUcs.origin.x!=0||modeUcs.origin.y!=0 ||
			modeUcs.axis_x.x!=-1||modeUcs.axis_x.y!=0||
			modeUcs.axis_y.x!=0||modeUcs.axis_y.y!=-1)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate single-hole rectangular plate: model CSYS is not correct!");
#else 
			logerr.Log("���׾��ε���ʼ��ʧ��:ģʽ����ϵ����ȷ!");
#endif
		if(boltItemArr[0].vertex.x!=0 && boltItemArr[0].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate single-hole rectangular plate:The bolt's coordinate isn't correct in model CSYS!");
#else 
			logerr.Log("���׾��ε���ʼ��ʧ��:ģʽ��������˨���겻��ȷ!");
#endif
	}
	else if(index==1)
	{
		if(modeUcs.origin.x!=10||modeUcs.origin.y!=0 ||
			modeUcs.axis_x.x!=-1||modeUcs.axis_x.y!=0||
			modeUcs.axis_y.x!=0||modeUcs.axis_y.y!=-1)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate multihole rectangular plate: model CSYS is not correct!");
#else 
			logerr.Log("��׾��ε���ʼ��ʧ��:ģʽ����ϵ����ȷ!");
#endif
		if(boltItemArr[0].vertex.x!=10 || boltItemArr[0].vertex.y!=0 ||
			boltItemArr[1].vertex.x!=0 || boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate multihole rectangular plate:the bolt's coordinate isn't correct in model CSYS!");
#else 
			logerr.Log("��׾��ε���ʼ��ʧ��:ģʽ��������˨���겻��ȷ!");
#endif
	}
	else 
	{
		if(modeUcs.origin.x!=-10||modeUcs.origin.y!=0 ||
			modeUcs.axis_x.x!=1||modeUcs.axis_x.y!=0||
			modeUcs.axis_y.x!=0||modeUcs.axis_y.y!=1)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate cut-leg gasket:model CSYS is not correct!");
#else 
			logerr.Log("�нǵ���ʼ��ʧ��:ģʽ����ϵ����ȷ!");
#endif
		if(boltItemArr[0].vertex.x!=0 || boltItemArr[0].vertex.y!=0||
			boltItemArr[1].vertex.x!=20||boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate cut-leg gasket:the bolt's coordinate isn't correct in model CSYS!");
#else 
			logerr.Log("�нǵ���ʼ��ʧ��:ģʽ��������˨���겻��ȷ!");
#endif
	}
}
void CBoltPadProperty::Test_CompareBoltInfo(int index,CBoltPadProperty* pDatumPadProp)
{
	CompareBoltInfo(pDatumPadProp);
	if(index==0)
	{
		BOLT_ITEM datumBoltItem=pDatumPadProp->boltItemArr[0];
		if(boltItemArr[0].d!=datumBoltItem.d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing single-hole rectangular plate's bolts failure: their bolts' diameterg are different!");
#else 
			logerr.Log("���׾��ε����˨�Ƚ�ʧ��:��Ӧ����˨ֱ����ͬ!");
#endif
		if(boltItemArr[0].vertex.x!=datumBoltItem.vertex.x ||
			boltItemArr[0].vertex.y!=datumBoltItem.vertex.y)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing single-hole rectangular plate's bolts failure:The bolt's coordiante is different!");
#else 
			logerr.Log("���׾��ε����˨�Ƚ�ʧ��:��Ӧ����˨���겻ͬ!");
#endif
		if(m_iSignIndex!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing single-hole rectangular plate's bolts failure:The standard model CSYS determine the error!");	
#else 
			logerr.Log("���׾��ε����˨�Ƚ�ʧ��:��׼ģʽ����ϵȷ������!");			
#endif		
	}
	else if(index==1)
	{
		if(boltItemArr[0].d!=pDatumPadProp->boltItemArr[0].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("��׾��ε����˨�Ƚ�ʧ��:��Ӧ����˨ֱ����ͬ!");
#endif
		if(boltItemArr[0].vertex.x!=pDatumPadProp->boltItemArr[0].vertex.x ||
			boltItemArr[0].vertex.y!=pDatumPadProp->boltItemArr[0].vertex.y||
			boltItemArr[0].vertex.x!=10 || boltItemArr[0].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("��׾��ε����˨�Ƚ�ʧ��:��Ӧ����˨�������!");
#endif
		if(boltItemArr[1].d!=pDatumPadProp->boltItemArr[1].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("��׾��ε����˨�Ƚ�ʧ��:��Ӧ����˨ֱ����ͬ!");
#endif
		if(boltItemArr[1].vertex.x!=pDatumPadProp->boltItemArr[1].vertex.x ||
			boltItemArr[1].vertex.y!=pDatumPadProp->boltItemArr[1].vertex.y||
			boltItemArr[1].vertex.x!=0 || boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("��׾��ε����˨�Ƚ�ʧ��:��Ӧ����˨�������!");
#endif
		if(m_iSignIndex!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:The standard model CSYS determine the error!");
#else 
			logerr.Log("��׾��ε����˨�Ƚ�ʧ��:��׼ģʽ����ϵȷ������!");
#endif
	}
	else 
	{
		if(boltItemArr[0].d!=pDatumPadProp->boltItemArr[1].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("�и�����˨�Ƚ�ʧ��:��Ӧ����˨ֱ����ͬ!");
#endif
		if(boltItemArr[0].vertex.x!=pDatumPadProp->boltItemArr[1].vertex.x ||
			boltItemArr[0].vertex.y!=pDatumPadProp->boltItemArr[1].vertex.y||
			boltItemArr[0].vertex.x!=0 || boltItemArr[0].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("�и�����˨�Ƚ�ʧ��:��Ӧ����˨�������!");
#endif
		if(boltItemArr[1].d!=pDatumPadProp->boltItemArr[0].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("�и�����˨�Ƚ�ʧ��:��Ӧ����˨ֱ����ͬ!");
#endif
		if(boltItemArr[1].vertex.x!=pDatumPadProp->boltItemArr[0].vertex.x ||
			boltItemArr[1].vertex.y!=pDatumPadProp->boltItemArr[0].vertex.y||
			boltItemArr[1].vertex.x!=20 || boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("�и�����˨�Ƚ�ʧ��:��Ӧ����˨�������!");
#endif
		if(m_iSignIndex!=2)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:The standard model CSYS determine the error!");
#else 
			logerr.Log("�и�����˨�Ƚ�ʧ��:��׼ģʽ����ϵȷ������!");
#endif
	}
}
void CBoltPadProperty::Test_MarkProfileVertexs(int index)
{
	MarkProfileVertexs();
	if(index==0)
	{
		if(vertexItemArr[0].vertex2d.x!=-20|| vertexItemArr[0].vertex2d.y!=-10||
			vertexItemArr[1].vertex2d.x!=20 || vertexItemArr[1].vertex2d.y!=-10||
			vertexItemArr[2].vertex2d.x!=20 || vertexItemArr[2].vertex2d.y!=10||
			vertexItemArr[3].vertex2d.x!=-20|| vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Single-hole rectangular gasket,transform form the vertices to the mode CSYS,coordinate error!");
#else 
			logerr.Log("���׾��ε��,�����ζ���ת����ģʽ����ϵ,�������!");
#endif
	}
	else if(index==1)
	{
		if(vertexItemArr[0].vertex2d.x!=-10|| vertexItemArr[0].vertex2d.y!=-10||
			vertexItemArr[1].vertex2d.x!=30 || vertexItemArr[1].vertex2d.y!=-10||
			vertexItemArr[2].vertex2d.x!=30 || vertexItemArr[2].vertex2d.y!=10||
			vertexItemArr[3].vertex2d.x!=-10|| vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Multihole rectangular gasket,transform form the vertices to the mode CSYS,coordinate error!");
#else 
			logerr.Log("��׾��ε��,�����ζ���ת����ģʽ����ϵ,�������!");
#endif
	}
	else
	{
		if(vertexItemArr[0].vertex2d.x!=-10|| vertexItemArr[0].vertex2d.y!=-10||
			vertexItemArr[1].vertex2d.x!=30 || vertexItemArr[1].vertex2d.y!=-10||
			vertexItemArr[2].vertex2d.x!=30 || vertexItemArr[2].vertex2d.y!=5||
			vertexItemArr[3].vertex2d.x!=20|| vertexItemArr[3].vertex2d.y!=10||
			vertexItemArr[4].vertex2d.x!=-10|| vertexItemArr[4].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Cut gasket,transform form the vertices to the mode CSYS,coordinate error!");
#else 
			logerr.Log("�и���,�����ζ���ת����ģʽ����ϵ,�������!");
#endif
	}
}
void CBoltPadProperty::Test_UpdateMinProfileVertex(int index,CBoltPadProperty* pDatumPadProp)
{
	UpdateMinProfileVertex(pDatumPadProp);
	if(index==0)
	{
		if(pDatumPadProp->vertexItemArr[0].vertex2d.x!=-15||pDatumPadProp->vertexItemArr[0].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[1].vertex2d.x!=20||pDatumPadProp->vertexItemArr[1].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[2].vertex2d.x!=20||pDatumPadProp->vertexItemArr[2].vertex2d.y!=10||
			pDatumPadProp->vertexItemArr[3].vertex2d.x!=-15||pDatumPadProp->vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Single-hole rectangular gasket,calculate the minimum outline's vertex error!");
#else 
			logerr.Log("���׾��ε��,������С���ζ������!");
#endif
	}
	else if(index==1)
	{
		if(pDatumPadProp->vertexItemArr[0].vertex2d.x!=-10||pDatumPadProp->vertexItemArr[0].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[1].vertex2d.x!=25||pDatumPadProp->vertexItemArr[1].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[2].vertex2d.x!=25||pDatumPadProp->vertexItemArr[2].vertex2d.y!=10||
			pDatumPadProp->vertexItemArr[3].vertex2d.x!=-10||pDatumPadProp->vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Multihole rectangular gasket,calculate the minimum outline's vertex error!");
#else 
			logerr.Log("��׾��ε��,������С���ζ������!");
#endif
	}
	else
	{
		if(pDatumPadProp->vertexItemArr[0].vertex2d.x!=-5||pDatumPadProp->vertexItemArr[0].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[1].vertex2d.x!=30||pDatumPadProp->vertexItemArr[1].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[2].vertex2d.x!=30||pDatumPadProp->vertexItemArr[2].vertex2d.y!=5||
			pDatumPadProp->vertexItemArr[3].vertex2d.x!=20||pDatumPadProp->vertexItemArr[3].vertex2d.y!=10||
			pDatumPadProp->vertexItemArr[4].vertex2d.x!=-5||pDatumPadProp->vertexItemArr[4].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Cut gasket,calculate the minimum outline's vertex error!");
#else 
			logerr.Log("�нǵ��,������С���ζ������!");
#endif
	}
}
void CBoltPadProperty::Test_EmendProfileVertexs(int index,CBoltPadProperty* pDatumPadProp)
{
	EmendProfileVertexs(pDatumPadProp);
	if(index==0)
	{
		if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(20,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-15,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-15,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(20,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Single-hole rectangular gasket,correct vertexs of datum gasket's outline!");
#else 
				logerr.Log("���׾��ε��,������׼������ζ������!");
#endif
		}
		else
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(15,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-20,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-20,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(15,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Single-hole rectangular gasket,correct errors of outline's vertexs!");
#else 
				logerr.Log("���׾��ε��,�������ζ������!");
#endif
		}
	}
	else if(index==1)
	{
		if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(15,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-20,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-20,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(15,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Multihole rectangular gasket,correct vertexs of datum gasket's outline!");
#else 
				logerr.Log("��׾��ε��,������׼������ζ������!");
#endif
		}
		else
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(20,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-15,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-15,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(20,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Multihole rectangular gasket,correct errors of outline's vertexs!");
#else 
				logerr.Log("��׾��ε��,�������ζ������!");
#endif
		}
	}
	else
	{
		if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(15,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-20,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-20,-5,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(-10,-10,0))||
				!vertexItemArr[4].pVertex->vertex.IsEqual(f3dPoint(15,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Cut gasket,correct vertexs of datum gasket's outline!");
#else 
				logerr.Log("�и���,������׼������ζ������!");
#endif
		}
		else
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(-15,-10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(20,-10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(20,5,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(10,10,0))||
				!vertexItemArr[4].pVertex->vertex.IsEqual(f3dPoint(-15,10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Cut gasket,correct errors of outline's vertexs!");
#else 
				logerr.Log("�и���,�������ζ������!");
#endif
		}
	}
}
void Test_BoltPadProperty()
{
	CLDSPlate boltPad[3],datumboltPad[3];
	CBoltPadProperty boltPadProp,datumPadProp;
	f3dPoint vertex;
	vertex.feature=1;
	logerr.ClearContents();
	for(int i=0;i<3;i++)
	{
		boltPad[i].ucs.origin.Set(0,0,0);
		boltPad[i].ucs.axis_x.Set(1,0,0);
		boltPad[i].ucs.axis_y.Set(0,1,0);
		boltPad[i].ucs.axis_z.Set(0,0,1);
		boltPad[i].handle=1;
		datumboltPad[i].ucs.origin.Set(0,0,0);
		datumboltPad[i].ucs.axis_x.Set(1,0,0);
		datumboltPad[i].ucs.axis_y.Set(0,1,0);
		datumboltPad[i].ucs.axis_z.Set(0,0,1);
		datumboltPad[i].handle=2;
		if(i==0)
		{	//���׾��ε��
			vertex.Set(20,10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt bolt;
			bolt.set_d(20);
			bolt.ucs.origin.Set(0,0,0);
			bolt.ucs.axis_z.Set(0,0,1);
			boltPad[0].AppendLsRef(bolt.GetLsRef(),FALSE);
			//
			vertex.Set(20,15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-15,15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-15,-15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt datumBolt;
			datumBolt.set_d(20);
			datumBolt.ucs.origin.Set(5,0,0);
			datumBolt.ucs.axis_z.Set(0,0,1);
			datumboltPad[0].AppendLsRef(datumBolt.GetLsRef(),FALSE);
		}
		else if(i==1)
		{	//��׾��ε��
			vertex.Set(20,10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt bolt[2];
			bolt[0].set_d(20);
			bolt[0].ucs.origin.Set(0,0,0);
			bolt[0].ucs.axis_z.Set(0,0,1);
			boltPad[1].AppendLsRef(bolt[0].GetLsRef(),FALSE);
			bolt[1].set_d(20);
			bolt[1].ucs.origin.Set(10,0,0);
			bolt[1].ucs.axis_z.Set(0,0,1);
			boltPad[1].AppendLsRef(bolt[1].GetLsRef(),FALSE);
			//
			vertex.Set(20,15,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,15,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt datumBolt[2];
			datumBolt[0].set_d(20);
			datumBolt[0].ucs.origin.Set(-5,0,0);
			datumBolt[0].ucs.axis_z.Set(0,0,1);
			datumboltPad[1].AppendLsRef(datumBolt[0].GetLsRef(),FALSE);
			datumBolt[1].set_d(20);
			datumBolt[1].ucs.origin.Set(5,0,0);
			datumBolt[1].ucs.axis_z.Set(0,0,1);
			datumboltPad[1].AppendLsRef(datumBolt[1].GetLsRef(),FALSE);
		}
		else if(i==2)
		{	//����нǵ��
			vertex.Set(10,10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,5,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt bolt[2];
			bolt[0].ucs.origin.Set(-10,0,0);
			bolt[0].ucs.axis_z.Set(0,0,1);
			boltPad[2].AppendLsRef(bolt[0].GetLsRef(),FALSE);
			bolt[1].ucs.origin.Set(10,0,0);
			bolt[1].ucs.axis_z.Set(0,0,1);
			boltPad[2].AppendLsRef(bolt[1].GetLsRef(),FALSE);
			//
			vertex.Set(15,15,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-25,15,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-25,-5,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-15,-20,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(15,-20,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt datumBolt[2];
			datumBolt[0].set_d(20);
			datumBolt[0].ucs.origin.Set(-10,0,0);
			datumBolt[0].ucs.axis_z.Set(0,0,1);
			datumboltPad[2].AppendLsRef(datumBolt[0].GetLsRef(),FALSE);
			datumBolt[1].set_d(20);
			datumBolt[1].ucs.origin.Set(10,0,0);
			datumBolt[1].ucs.axis_z.Set(0,0,1);
			datumboltPad[2].AppendLsRef(datumBolt[1].GetLsRef(),FALSE);
		}
		boltPadProp.Test_InitProperty(i,&boltPad[i]);
		datumPadProp.InitProperty(&datumboltPad[i]);
		boltPadProp.Test_CompareBoltInfo(i,&datumPadProp);
		boltPadProp.Test_MarkProfileVertexs(i);
		datumPadProp.MarkProfileVertexs();
		boltPadProp.Test_UpdateMinProfileVertex(i,&datumPadProp);
		boltPadProp.Test_EmendProfileVertexs(i,&datumPadProp);
		datumPadProp.Test_EmendProfileVertexs(i,&datumPadProp);
	}
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
}
#endif
