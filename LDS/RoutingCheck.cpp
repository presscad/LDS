#include "StdAfx.h"
#include "RoutingCheck.h"
#include "env_def.h"
#include "SortFunc.h"
#include "ComparePartNoString.h"
#include "list.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"

#ifdef __ANGLE_PART_INC_
 SCOPE_STRU GetTaAtomScope(const CLDSPartPtr pObj); // from DrawMapInAcad.cpp 52;
///////////////////////////////////////////////////////////////////////////////////
// ROUTINGCHECK_FLAG
struct ROUTINGCHECK_FLAG{
	//3.��������˨��׼�϶
	bool bExportExterHoleSpace;
	//*6.���������϶��ƫ��׼�౨��
	//ExportHuoQuSpaceAndOffsetG();
	//8.Ԥ���߶ȱ���(���ؼ����ƽڵ�ĵ��߼�ʵ��Ԥ���߶�)���쳣��ʾ
	bool bExportArcLiftWarning;
	//9.���߰弰���߰屨�漰��Բ������
	bool bExportAngleRibPlateAndGroovePlate;
	//10.��߼��
	//ExportRollEdgePart();
	//11.������ϽǸּ�϶����ֵ�������Ӱ��Ĳ���һֵ��
	bool bGroupAngleSpace;
	//12.�нǾ�ʾ��Ϣ
	bool bCutAngleInfo;
	//13.���ָ˼������仯����˨����δ�涯(OK)
	bool bCheckRodVariation;
	//14.�Ǹ��������嵥(�ҳ��ص�˼��嵥)
	bool bExportSpecialAngleTeG;
	//16.��������͸ְ���˨�߾�
	bool bCheckPlateEdgeSpace;
	//17.�Ǹ�����˨�߾���
	bool bCheckJgBoltEndSpace;
	//20.�ԳƸ˼�����ͷ��һ��
	bool bCheckRodOddWarning;
	//25.L63*5���ϽǸָ�ǿ���о�
	bool bCheckSmallSizeHighMaterialAngle;
	//26.�Ǹ�����������ʲ�һ�±���
	bool bExportAngleAndPlateMatDiff;
	//27.�Ǹ�����˾��⣨ֻ����ʼĩ��һ���������׼�⣩
	//28.��������©ȱʧ���
	bool bCheckMissingLabelPart;
	//29.������©��⣨��ͷδ����������ƻ�ȱ�ٽ������˨��
	bool bCheckMissingConnect;
	ROUTINGCHECK_FLAG(){memset(this,0,sizeof(ROUTINGCHECK_FLAG));}
	void EnableAll(){
		//3.��������˨��׼�϶
		bExportExterHoleSpace=true;
		//*6.���������϶��ƫ��׼�౨��
		//ExportHuoQuSpaceAndOffsetG();
		//8.Ԥ���߶ȱ���(���ؼ����ƽڵ�ĵ��߼�ʵ��Ԥ���߶�)���쳣��ʾ
		bExportArcLiftWarning=true;
		//9.���߰弰���߰屨�漰��Բ������
		bExportAngleRibPlateAndGroovePlate=true;
		//10.��߼��
		//ExportRollEdgePart();
		//11.������ϽǸּ�϶����ֵ�������Ӱ��Ĳ���һֵ��
		bGroupAngleSpace=true;
		//12.�нǾ�ʾ��Ϣ
		bCutAngleInfo=true;
		//13.���ָ˼������仯����˨����δ�涯(OK)
		bCheckRodVariation=true;
		//14.�Ǹ��������嵥(�ҳ��ص�˼��嵥)
		bExportSpecialAngleTeG=true;
		//16.��������͸ְ���˨�߾�
		bCheckPlateEdgeSpace=true;
		//17.�Ǹ�����˨�߾���
		bCheckJgBoltEndSpace=true;
		//20.�ԳƸ˼�����ͷ��һ��
		bCheckRodOddWarning=true;
		//25.L63*5���ϽǸָ�ǿ���о�
		bCheckSmallSizeHighMaterialAngle=true;
		//26.�Ǹ�����������ʲ�һ�±���
		bExportAngleAndPlateMatDiff=true;
		//27.�Ǹ�����˾��⣨ֻ����ʼĩ��һ���������׼�⣩
		//28.��������©ȱʧ���
		bCheckMissingLabelPart=true;
		//29.������©��⣨��ͷδ����������ƻ�ȱ�ٽ������˨��
		bCheckMissingConnect=true;
	}
};
/////////////////
//1.��ϽǸֱ�ż���϶�Ŵ�ֵ
WORD GROUPANGLESPACE_CHKINFO::MANU_SPACE = 2;
CXhChar200 GROUPANGLESPACE_CHKINFO::GetErrorMessage()
{
	CXhChar200 message("��ϽǸ�#%s ",(char*)pGroupAngle->m_pDatumSonAngle->Label);
	int manuspace=ftoi(pGroupAngle->jg_space-pPlate->Thick);
	if(manuspace!=MANU_SPACE)
	{
		if(manuspace==0)
			message.Append(CXhChar50("δ���Ǽӹ���϶%dmm\n",MANU_SPACE));
		else
			message.Append(CXhChar50("���Ǽӹ���϶%d!=%dmm\n",manuspace,MANU_SPACE));
	}
	return message;
}

//�����ϽǸּ�϶�Ƿ������˼ӹ���϶�Ŵ�ֵ������Ӧ�ְ�İ�񼰲����Ƿ�����ϽǸ�һ�� wjh-2017.1.20
void CheckGroupAngleSpace(CXhSimpleList<GROUPANGLESPACE_CHKINFO>* pList)
{
	for(CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)Ta.Parts.GetFirst(CLS_GROUPLINEANGLE);
		pGroupAngle;pGroupAngle=(CLDSGroupLineAngle*)Ta.Parts.GetNext(CLS_GROUPLINEANGLE))
	{
		CLDSLineAngle* pDatumSonAngle=pGroupAngle->m_pDatumSonAngle;
		if(pDatumSonAngle==NULL)
			continue;
		bool bActWingX=true,bActWingY=true;
		GEPOINT vxWingNorm=pDatumSonAngle->vxWingNorm;
		GEPOINT vyWingNorm=pDatumSonAngle->vyWingNorm;
		if(pGroupAngle->size_cSubClassType=='T'&&pGroupAngle->ciMasterIndex==0)	//
			bActWingY=false;	//����T����ϽǸֵ�1�Ż�׼�ӽǸ�Y֫����ϼ�϶����֫
		else if(pGroupAngle->size_cSubClassType=='T'&&pGroupAngle->ciMasterIndex==1)	//
			bActWingX=false;	//2#T����ϽǸֵ�2�Ż�׼�ӽǸ�X֫����ϼ�϶����֫
		BOLTSET listBoltSetX,listBoltSetY;
		CLsRef* pBoltRef;
		for(pBoltRef=pDatumSonAngle->GetFirstLsRef();pBoltRef;pBoltRef=pDatumSonAngle->GetNextLsRef())
		{
			if(fabs(pBoltRef->pBolt->Normal*vxWingNorm)>EPS_COS2)
				listBoltSetX.append(pBoltRef->pBolt);
			else if(fabs(pBoltRef->pBolt->Normal*vyWingNorm)>EPS_COS2)
				listBoltSetY.append(pBoltRef->pBolt);
		}
		CLDSLineAngle* pLineAngle=(CLDSLineAngle*) Ta.FromPartHandle(pGroupAngle->m_hDatumSonAngle);
		CLsRef *pLsRef=pLineAngle->GetFirstLsRef();
		if(pLsRef==NULL)
			continue;
		CLDSBolt* pBolt=(pLsRef)?pLsRef->GetLsPtr():NULL;
		int nPush=Ta.Parts.push_stack();
		CHashSet<CLDSGeneralPlate*> hashRelaPlates;
		for(CLDSPlate* pDatumPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pDatumPlate;pDatumPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			bool processed=false;
			if(bActWingX)
			{
				//TODO:δ��ɻ�������Ϊ���������
				if(fabs(pDatumPlate->ucs.axis_z*vxWingNorm)<EPS_COS2)
					continue;	//δ�뵱ǰ֫������
				if(pDatumPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
					continue;	//�������
				for(pBolt=listBoltSetX.GetFirst();pBolt;pBolt=listBoltSetX.GetNext())
				{
					if(pDatumPlate->FindLsByHandle(pBolt->handle))
					{
						hashRelaPlates.SetValue(pDatumPlate->handle,pDatumPlate);
						processed=true;
						break;	
					}
				}
			}
			if(bActWingY&&!processed)
			{
				//TODO:δ��ɻ�������Ϊ���������
				if(fabs(pDatumPlate->ucs.axis_z*vyWingNorm)<EPS_COS2)
					continue;	//δ�뵱ǰ֫������
				if(pDatumPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
					continue;	//�������
				for(pBolt=listBoltSetY.GetFirst();pBolt;pBolt=listBoltSetY.GetNext())
				{
					if(pDatumPlate->FindLsByHandle(pBolt->handle))
					{
						hashRelaPlates.SetValue(pDatumPlate->handle,pDatumPlate);
						break;	
					}
				}
			}
		}
		for(CLDSGeneralPlate* pPlate=hashRelaPlates.GetFirst();pPlate;pPlate=hashRelaPlates.GetNext())
		{
			if(pGroupAngle->jg_space-pPlate->Thick==GROUPANGLESPACE_CHKINFO::MANU_SPACE)
				continue;
			GROUPANGLESPACE_CHKINFO* pErrorInfo=pList->AttachObject();
			pErrorInfo->pGroupAngle=pGroupAngle;
			pErrorInfo->pPlate=pPlate;
		}
		Ta.Parts.pop_stack(nPush);
	}	
}
static int CompareGroupAngleExportInfo(const EXPORT_INFO_PTR& item1,const EXPORT_INFO_PTR& item2)
{
	if(item1->segI.iSeg>item2->segI.iSeg)
		return 1;
	else if(item1->segI.iSeg<item2->segI.iSeg)
		return -1;
	int compare_angle=ComparePartNoString(item1->pGroupAngle->Label,item2->pGroupAngle->Label,"HGP");
	if(compare_angle!=0)
		return compare_angle;
	int compare_plate=ComparePartNoString(item1->pPlate->Label,item2->pPlate->Label,"HGP");
	return compare_plate;
}
typedef CLDSPlate*  CLDSPLATE_PTR;
static int ComparePlateNo(const CLDSPLATE_PTR& item1,const CLDSPLATE_PTR& item2)
{
	return ComparePartNoString(item1->GetPartNo(),item2->GetPartNo());
}

static void ExportCheckGroupAngleInfo(CXhSimpleList<GROUPANGLESPACE_CHKINFO>* pList)
{
	LIST_NODE<GROUPANGLESPACE_CHKINFO>*pTailNode=pList->EnumTail();
	if(pTailNode==NULL)
		return;
	ARRAY_LIST<GROUPANGLESPACE_CHKINFO*> exportInfoArr(0,pTailNode->serial);
	for (GROUPANGLESPACE_CHKINFO*	pInfo=pList->EnumObjectFirst();pInfo;pInfo=pList->EnumObjectNext())
		exportInfoArr.append(pInfo);
	CBubbleSort<GROUPANGLESPACE_CHKINFO*>::BubSort(exportInfoArr.m_pData,exportInfoArr.GetSize(),CompareGroupAngleExportInfo);
	for(int i=0;i<exportInfoArr.GetSize();i++)
	{
		CXhChar200 error=exportInfoArr[i]->GetErrorMessage();
		if(error.Length==0)
			continue;
		logerr.Log("%s",(char*)error);
	}
}

//2.���߰弰���߰屨�漰��Բ������
static int CompareRibExportInfo(const RIB_EXPORT_INFO& item1,const RIB_EXPORT_INFO& item2)
{
	return ComparePartNoString(item1.sLabel,item2.sLabel);
}
void  ExportAngleRibPlateAndGroovePlate()
{	//�ҳ����е��߰�
	ARRAY_LIST<RIB_EXPORT_INFO> ribExportInfoList;
	CHashSet<CLDSGeneralPlate*> hashlibPartByHandel;
	for(CLDSPart* pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
	{
		if(!pPart->IsPlate())
			continue;
		CSuperSmartPtr<CLDSGeneralPlate> pPlate=(CLDSGeneralPlate*)pPart;
		//������
		if(pPlate->GetClassTypeId()==CLS_PARAMPLATE&&(pPlate.pParamPlate->m_iParamType!=TYPE_RIBPLATE&&pPlate.pParamPlate->m_iParamType!=TYPE_ELBOWPLATE))
			continue;
		if(pPlate->vertex_list.GetNodeNum()<3)
			continue;
		BYTE cnHasTwoWeldEdge=0;
		//��ͨ�ְ�
		ATOM_LIST<int> weldEdgelist;
		PROFILE_VERTEX *pPrevVertex=pPlate->vertex_list.GetTail();
		PROFILE_VERTEX *pPrevPrevVertex=pPlate->vertex_list.GetPrev();
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;
			pPrevPrevVertex=pPrevVertex,pPrevVertex=pVertex,pVertex=pPlate->vertex_list.GetNext())
		{
			GEPOINT prevVec=pPrevPrevVertex->vertex-pPrevVertex->vertex;
			GEPOINT currVec=pVertex->vertex-pPrevVertex->vertex;
			if(pPrevVertex->m_bWeldEdge&&pVertex->m_bWeldEdge)
			{	//�������߾�Ϊ���ӱ�
				pPlate->vertex_list.push_stack();
				PROFILE_VERTEX* pNextVertex=pPlate->vertex_list.GetNext();
				if(pNextVertex==NULL)
					pNextVertex=pPlate->vertex_list.GetFirst();
				pPlate->vertex_list.pop_stack();
				if(DistOf2dPtLine(pNextVertex->vertex,pPrevVertex->vertex,pVertex->vertex)<0)
					continue;	//�ڰ����Թ�
				normalize(prevVec);
				normalize(currVec);
				if(prevVec*currVec<-0.707)	//cos(135��)=-0.707
					continue;
				logerr.Log("0x%X�ְ�{%s}�����ӱ߼�δ������(��)",pPlate->handle,(char*)pPlate->Label);
			}
			else if(pPrevPrevVertex->m_bWeldEdge&&!pPrevVertex->m_bWeldEdge&&pVertex->m_bWeldEdge)
			{	//�м�����Ѵ�������ǻ�
				if(DistOf2dPtLine(pVertex->vertex,pPrevPrevVertex->vertex,pPrevVertex->vertex)<0)
					continue;	//�ڰ����Թ�
				pPlate->vertex_list.push_stack();
				PROFILE_VERTEX* pNextVertex=pPlate->vertex_list.GetNext();
				if(pNextVertex==NULL)
					pNextVertex=pPlate->vertex_list.GetFirst();
				pPlate->vertex_list.pop_stack();
				if(DistOf2dPtLine(pNextVertex->vertex,pPrevPrevVertex->vertex,pPrevVertex->vertex)<0)
					continue;	//�ڰ����Թ�
				GEPOINT nextVec=pNextVertex->vertex-pVertex->vertex;
				normalize(prevVec);
				normalize(nextVec);
				if(prevVec*nextVec<-0.707)	//cos(135��)=-0.707
					continue;
				GEPOINT inters;
				double minR=0;
				int retcode=Int2dll(f2dLine(pPrevPrevVertex->vertex,pPrevVertex->vertex),f2dLine(pNextVertex->vertex,pVertex->vertex),inters.x,inters.y);
				if(retcode!=0&&retcode!=-1)
					minR=min(DISTANCE(pPrevVertex->vertex,inters),DISTANCE(pVertex->vertex,inters));
				if(minR<1.2*pPlate->Thick)
					logerr.Log("0x%X�ְ�{%s}�����ӱ߼䵹��(��)�뾶ƫС%d",pPlate->handle,(char*)pPlate->Label,ftoi(minR));
			}
		}
	}
}
//3.��ߴ���
static int CompareRibExportInfo(const ROLL_EDGE_EXPORT_INFO& item1,const ROLL_EDGE_EXPORT_INFO& item2)
{
	return ComparePartNoString(item1.sLabel,item2.sLabel);
}
void  ExportRollEdgePart()
{
	ARRAY_LIST<ROLL_EDGE_EXPORT_INFO> rollEdgeExportInfoList;
	for(CLDSPart* pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
	{
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
		int	 nRollEdge=0,nEdge=0;
		CXhChar100 sRollEdgeHeigth="";
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->m_bRollEdge)
			{
				nRollEdge++;
				if(sRollEdgeHeigth!=""&&sRollEdgeHeigth.GetLength()>0)
					sRollEdgeHeigth+="#";
				sRollEdgeHeigth+=CXhChar16(pVertex->manu_space);
			}
			nEdge++;
		}
		if(nRollEdge>0)
		{
			ROLL_EDGE_EXPORT_INFO rollEdgeInfo;
			rollEdgeInfo.segI=pPlate->iSeg;
			rollEdgeInfo.nRollEdge=nRollEdge;
			rollEdgeInfo.sLabel=pPlate->GetPartNo();
			rollEdgeInfo.cMaterial=pPlate->cMaterial;
			rollEdgeExportInfoList.append(rollEdgeInfo);
			rollEdgeInfo.sRollEdgeHeigth=sRollEdgeHeigth;
			if(pPlate->m_cRollProcessType==1)	
				rollEdgeInfo.sRollEdgeDispose="���ٵ���";
			else if(pPlate->m_cRollProcessType==1)
				rollEdgeInfo.sRollEdgeDispose="�������";
			else
				rollEdgeInfo.sRollEdgeDispose="Ĭ��ֵ";	
			g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
		}
	}
	//��ʱ���
	CBubbleSort<ROLL_EDGE_EXPORT_INFO>::BubSort(rollEdgeExportInfoList.m_pData,rollEdgeExportInfoList.GetSize(),CompareRibExportInfo);
	logerr.ClearContents();
	logerr.Log("�κ�  ����   ��߸߶�    ��ߴ���ʽ   ����  �����   ��� ����");
	for (ROLL_EDGE_EXPORT_INFO* pInfo=rollEdgeExportInfoList.GetFirst();pInfo;pInfo=rollEdgeExportInfoList.GetNext())
	{
		logerr.Log("%s  %s   %s   %s  %d   %d   %c",(char*)pInfo->segI.ToString(),(char*)pInfo->sLabel,
				(char*)pInfo->sRollEdgeHeigth,(char*)pInfo->sRollEdgeDispose,
				"",pInfo->nRollEdge,pInfo->cMaterial);
	}
	logerr.ShowToScreen();
}
//4.��������˨��׼�϶
static int CompareBoltSpacExportInfo(const BOLTEXTERSPACE_CHKINFO& item1,const BOLTEXTERSPACE_CHKINFO& item2)
{
	if(item1.segI.iSeg>item2.segI.iSeg)
		return 1;
	else if(item1.segI.iSeg>item2.segI.iSeg)
		return -1;
	else 
		return 0;

	//return ComparePartNoString(item1.iSeg.ToString(),item2.iSeg.ToString());
}
static BYTE FindMasterBackAngleIndex(CLDSGroupLineAngle *pGroupAngle)
{
	if(pGroupAngle->size_cSubClassType=='D')
		return 1-pGroupAngle->ciMasterIndex;
	else if(pGroupAngle->size_cSubClassType=='T')
		return 1-pGroupAngle->ciMasterIndex;	//T����ϽǸִ���Ļ�����,Ӧ����ǰ��������֫ȥ�ж��Ƿ�����һ������
	else if(pGroupAngle->size_cSubClassType=='X')
	{
		if(pGroupAngle->ciMasterIndex>=2)
			return pGroupAngle->ciMasterIndex-2;
		else
			return pGroupAngle->ciMasterIndex+2;
	}
	return -1;
}
void ExportExterHoleSpace()
{
	CHashList<CHashList<BOLTEXTERSPACE_CHKINFO>>  hashBoltSpacInfo;
	CHashList<WORD> hashBoltdByD;
	hashBoltdByD.SetValue(12,20);
	hashBoltdByD.SetValue(16,20);
	hashBoltdByD.SetValue(20,25);
	hashBoltdByD.SetValue(24,35);
	CHashSet<CLDSPlate*> hashPlateByHandel;
	CMapList<long> hashmapExterEndAngle;
	for(CLDSPlate* pPlate=(CLDSPlate*)Ta.EnumPartFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.EnumPartNext(CLS_PLATE))
	{
		CLDSLineAngle* pDatumAngle=NULL,*pDatumAngle2=NULL;
		pDatumAngle=(CLDSLineAngle*)Ta.FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pDatumAngle)
		{	//Ϊ��ֹ�������������Ʋ��淶����δ��angle.bEndLjJg�Ķ����ӽǸ����,����н�һ��ϸ�� wjh-2017.11.24
			if(pDatumAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle* pDatumGroupAngle=(CLDSGroupLineAngle*)pDatumAngle;
				pDatumAngle =(CLDSLineAngle*)Ta.FromPartHandle(pDatumGroupAngle->son_jg_h[pDatumGroupAngle->ciMasterIndex],CLS_LINEANGLE);
				BYTE ciReverseIndex=FindMasterBackAngleIndex(pDatumGroupAngle);
				if(ciReverseIndex>=0)
					pDatumAngle2=(CLDSLineAngle*)Ta.FromPartHandle(pDatumGroupAngle->son_jg_h[ciReverseIndex],CLS_LINEANGLE);
			}
		}
		for (CDesignLjPartPara* pPara=pPlate->designInfo.partList.GetFirst();pPara;pPara=pPlate->designInfo.partList.GetNext())
		{
			pPara->pCurLinePart=Ta.FromRodHandle(pPara->hPart);
			if(pPara->pCurLinePart==NULL&&pPara->hPart>0x20)
			{
				logerr.Log("0X%X�ְ�{%s}���Ӹ˼�0X%X��ʧ��",pPlate->handle,pPlate->Label, pPara->hPart);
				continue;
			}
			if (pPara->angle.bEndLjJg)
				hashPlateByHandel.SetValue(pPlate->handle,pPlate);
			else if(pDatumAngle!=NULL)
			{	//Ϊ��ֹ�������������Ʋ��淶����δ��angle.bEndLjJg�Ķ����ӽǸ����,����н�һ��ϸ�� wjh-2017.11.24
				CLDSLineAngle* pRayAngle=pPara->pCurLinePart->GetClassTypeId()==CLS_LINEANGLE?(CLDSLineAngle*)pPara->pCurLinePart:NULL;
				bool hasBoltInDatumAngle=false,hasBoltNotInDatumAngle=false;
				for(CLsRef* pLsRef=pRayAngle->GetFirstLsRef();pLsRef;pLsRef=pRayAngle->GetNextLsRef())
				{
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					if(pPlate->FindLsByHandle(pBolt->handle)==NULL)
						continue;	//�ǵ�ǰ�����ϵ���˨
					if(pDatumAngle->FindLsByHandle(pBolt->handle)!=NULL)
						hasBoltInDatumAngle=true;
					else if(pDatumAngle2&&pDatumAngle2->FindLsByHandle(pBolt->handle)!=NULL)
						hasBoltInDatumAngle=true;
					else
						hasBoltNotInDatumAngle=true;
				}
				if(hasBoltInDatumAngle&&hasBoltNotInDatumAngle)
				{	//ͬʱ����λ�ڻ�׼�Ǹּ���λ�ڻ�׼�Ǹ��ϵ���˨�����ýǸ�Ϊ�����ӽǸ�
					hashPlateByHandel.SetValue(pPlate->handle,pPlate);
					hashmapExterEndAngle.Add(pPlate->handle,pRayAngle->handle);
					break;
				}
			}
		}
	}
	for (CLDSPlate* pPlate=hashPlateByHandel.GetFirst();pPlate;pPlate=hashPlateByHandel.GetNext())
	{
		CLDSPart* pPart= Ta.FromPartHandle(pPlate->designInfo.m_hBasePart);
		CLDSLineAngle* pDatumAngle=NULL,* pAngle=NULL; 
		if(pPart->IsAngle())
			pDatumAngle=(CLDSLineAngle*)pPart;
		else
		{
			logerr.Log("0X%X�ְ��Ӹ��˼��쳣��",pDatumAngle->handle);
			continue;
		}
		if(pDatumAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//TODO:�������� wjh-2017.11.24
			CLDSGroupLineAngle* pDatumGroupAngle=(CLDSGroupLineAngle*)pDatumAngle;
			pDatumAngle =(CLDSLineAngle*)Ta.FromPartHandle(pDatumGroupAngle->son_jg_h[pDatumGroupAngle->ciMasterIndex],CLS_LINEANGLE);
		}
		//1.����׼�Ǹ�����˨�������߽Ǹֵ���׼�϶�Ƿ�����Ҫ��
		CLsRefList *pLsRefList=pDatumAngle->GetLsRefList();
		if(pLsRefList==NULL)
			continue;
		for (CDesignLjPartPara* pPara=pPlate->designInfo.partList.GetFirst();pPara;pPara=pPlate->designInfo.partList.GetNext())
		{
			if(pPara->pCurLinePart==NULL||(!pPara->pCurLinePart->IsAngle()||pPara->hPart==pDatumAngle->handle))
				continue;
			if(!pPara->angle.bEndLjJg&&hashmapExterEndAngle.GetValue(pPlate->handle,pPara->hPart)==NULL)
				continue;	//��ǰ���߽ǸַǶ����ӽǸ�,�����ǻ�׼������˨�����߽Ǹּ����׼�϶ wjh-2017.11.24
			pAngle=(CLDSLineAngle*)pPara->pCurLinePart;
			f3dLine line1(pAngle->Start(),pAngle->End()),line2;
			f3dPoint lineStdVec=line1.endPt-line1.startPt;
			normalize(lineStdVec);
			f3dPoint vec=pPara->angle.cur_wing_x0_y1==0?pAngle->GetWingVecX():pAngle->GetWingVecY();
			line2.startPt=line1.startPt+pAngle->GetWidth()*vec;
			line2.endPt=line1.endPt+pAngle->GetWidth()*vec;
			line1.startPt-=pAngle->startOdd()*lineStdVec;
			line2.startPt-=pAngle->startOdd()*lineStdVec;
			line1.endPt+=pAngle->endOdd()*lineStdVec;
			line2.endPt+=pAngle->endOdd()*lineStdVec;

			CPtInLineCheck check(line1.startPt,line1.endPt);
			double fPrevDd=0,fdd1=0,fdd2=0;
			CLDSBolt* pPrevBolt=NULL;
			for (CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				CLDSBolt* pBolt=pLsRef->GetLsPtr();
				CLsRef* pBoltRef=pAngle->FindLsByHandle(pBolt->handle);
				CLsRef* pInPartBoltRef=pPlate->FindLsByHandle(pBolt->handle);
				if(pBoltRef||pInPartBoltRef==NULL)
					continue;
				f3dPoint pos=pBolt->ucs.origin;
				pPlate->GetBoltIntersPos(pBolt,pos);
				double scale=check.CheckPointEx(pos);
				if(scale<0||scale>1)
					continue;
				project_point(pos,line1.startPt,pPlate->ucs.axis_z);
				fdd1=DistOf3dPtLine(pos,line1.startPt,line1.endPt);
				fdd2=DistOf3dPtLine(pos,line2.startPt,line2.endPt);
				double fdd=min(fdd1,fdd2);
				CHashList<BOLTEXTERSPACE_CHKINFO>* pBoltSpacList= hashBoltSpacInfo.GetValue(pAngle->handle);
				if(pBoltSpacList==NULL)
					pBoltSpacList=hashBoltSpacInfo.Add(pAngle->handle);
				if(fPrevDd!=0&&fdd<fPrevDd)
					pBoltSpacList->DeleteNode(pPrevBolt->handle);
				else if(fPrevDd!=0&&fdd>fPrevDd)
					continue;
				BOLTEXTERSPACE_CHKINFO  boltSpacInfo;
				boltSpacInfo.Md=(WORD)pBolt->get_d();
				WORD *pSpace=hashBoltdByD.GetValue(pBolt->get_d());
				boltSpacInfo.minSpace=pSpace!=NULL?*pSpace:20;
				boltSpacInfo.bPassed=(fdd>=boltSpacInfo.minSpace)?TRUE:FALSE;
				boltSpacInfo.clearance=fdd;
				boltSpacInfo.hAngle=pAngle->handle;
				boltSpacInfo.hBolt=pBolt->handle;
				boltSpacInfo.labelAngle.Copy(pAngle->Label);
				boltSpacInfo.labelPlate.Copy(pPlate->Label);
				pBoltSpacList->SetValue(pBolt->handle,boltSpacInfo);
				pPrevBolt=pBolt;
				fPrevDd=fdd;
			}
		}
		//2.����׼�Ǹ�����˨�������߽Ǹֵ���׼�϶�Ƿ�����Ҫ��
		f3dLine lineBer(pDatumAngle->Start(),pDatumAngle->End()),lineWing;
		char ciDatumAngleWing=fabs(pDatumAngle->vxWingNorm*pPlate->ucs.axis_z)>fabs(pDatumAngle->vyWingNorm*pPlate->ucs.axis_z)?0:1;
		GEPOINT vWingVec=ciDatumAngleWing==0?pDatumAngle->GetWingVecX():pDatumAngle->GetWingVecY();
		GEPOINT vWorkNormal=ciDatumAngleWing==0?pDatumAngle->vxWingNorm:pDatumAngle->vyWingNorm;
		lineWing.startPt=lineBer.startPt+pDatumAngle->GetWidth()*vWingVec;
		lineWing.endPt=lineBer.endPt+pDatumAngle->GetWidth()*vWingVec;
		GEPOINT lineStdVec=lineBer.endPt-lineBer.startPt;
		normalize(lineStdVec);
		lineBer.startPt-=pDatumAngle->startOdd()*lineStdVec;
		lineWing.startPt-=pDatumAngle->startOdd()*lineStdVec;
		lineBer.endPt+=pDatumAngle->endOdd()*lineStdVec;
		lineWing.endPt+=pDatumAngle->endOdd()*lineStdVec;
		for (CDesignLjPartPara* pPara=pPlate->designInfo.partList.GetFirst();pPara;pPara=pPlate->designInfo.partList.GetNext())
		{
			if(!pPara->angle.bEndLjJg&&hashmapExterEndAngle.GetValue(pPlate->handle,pPara->hPart)==NULL)
				continue;	//�Ƕ����ӽǸ�(�򲻴������߽Ǹ��������)
			if(pPara->hPart==pDatumAngle->handle||pPara->pCurLinePart==NULL||pPara->pCurLinePart->GetClassTypeId()!=CLS_LINEANGLE)
				continue;
			CLDSLineAngle* pRayAngle=(CLDSLineAngle*)pPara->pCurLinePart;
			CPtInLineCheck check(lineBer.startPt,lineBer.endPt);
			double fPrevDd=0,fdd1,fdd2;
			CLDSBolt* pPrevBolt=NULL;
			for (CLsRef *pLsRef=pRayAngle->GetFirstLsRef();pLsRef;pLsRef=pRayAngle->GetNextLsRef())
			{
				CLDSBolt* pBolt=pLsRef->GetLsPtr();
				if(pPlate->FindLsByHandle(pBolt->handle)==NULL)
					continue;	//����˨���ǵ�ǰ���Ӱ��ϵ�������˨
				if(pDatumAngle->FindLsByHandle(pBolt->handle)!=NULL)
					continue;	//����˨λ�ڻ�׼�Ǹ���,����Ҫ�����׼�϶
				f3dPoint location;
				if(!pPlate->GetBoltIntersPos(pBolt,location))
					continue;
				project_point(location,lineBer.startPt,vWorkNormal);
				double scale=check.CheckPointEx(location);
				if(scale<0||scale>1)
					continue;
				fdd1=DistOf3dPtLine(location,lineWing.startPt,lineWing.endPt);
				fdd2=DistOf3dPtLine(location,lineBer.startPt,lineBer.endPt);
				double fdd=min(fdd1,fdd2);

				CHashList<BOLTEXTERSPACE_CHKINFO>* pBoltSpacList= hashBoltSpacInfo.GetValue(pDatumAngle->handle);
				if(pBoltSpacList==NULL)
					pBoltSpacList=hashBoltSpacInfo.Add(pDatumAngle->handle);
				if(fPrevDd!=0&&fPrevDd>fdd)
				{
					pBoltSpacList->DeleteNode(pPrevBolt->handle);
				}
				else if(fPrevDd!=0&&fdd>fPrevDd)
					continue;
				BOLTEXTERSPACE_CHKINFO  boltSpacInfo;
				boltSpacInfo.Md=(WORD)pBolt->get_d();
				WORD *pSpace=hashBoltdByD.GetValue(pBolt->get_d());
				boltSpacInfo.minSpace=pSpace!=NULL?*pSpace:20;
				boltSpacInfo.bPassed=(fdd>=boltSpacInfo.minSpace)?TRUE:FALSE;
				boltSpacInfo.clearance=fdd;
				boltSpacInfo.hAngle=pDatumAngle->handle;
				boltSpacInfo.hBolt=pBolt->handle;
				boltSpacInfo.labelAngle.Copy(pDatumAngle->Label);
				boltSpacInfo.labelPlate.Copy(pPlate->Label);
				pBoltSpacList->SetValue(pBolt->handle,boltSpacInfo);
				pPrevBolt=pBolt;
				fPrevDd=fdd;
			}
		}
	}
	//��ʱ���
	logerr.ClearContents();
	for (CHashList<BOLTEXTERSPACE_CHKINFO>* pBoltSpacList=hashBoltSpacInfo.GetFirst();pBoltSpacList;pBoltSpacList=hashBoltSpacInfo.GetNext())
	{
		ARRAY_LIST<BOLTEXTERSPACE_CHKINFO> boltSpacInfo;
		for (BOLTEXTERSPACE_CHKINFO* pInfo=pBoltSpacList->GetFirst();pInfo;pInfo=pBoltSpacList->GetNext())
			boltSpacInfo.append(*pInfo);
		CBubbleSort<BOLTEXTERSPACE_CHKINFO>::BubSort(boltSpacInfo.m_pData,boltSpacInfo.GetSize(),CompareBoltSpacExportInfo);
		for (BOLTEXTERSPACE_CHKINFO* pInfo=boltSpacInfo.GetFirst();pInfo;pInfo=boltSpacInfo.GetNext())
		{
			if(pInfo->bPassed)
				continue;
			CXhChar100 label("��˨0X%X(M%d)-->�Ǹ�0X%X{%s}�ڸְ�{%s}��׼�϶: ",pInfo->hBolt,pInfo->Md,pInfo->hAngle,(char*)pInfo->labelAngle,(char*)pInfo->labelPlate);
			label.ResizeLength(40);
			logerr.Log("%s%.1f<%d",(char*)label,pInfo->clearance,pInfo->minSpace);
		}
	}
}


//5.���⹤�տױ��漰�Գ���У��
//void 



//6.�ְ�߾�(��ͨ�ְ�\���ְ�\���)�쳣��ʾ��Ϣ
//defined in Plate.cpp
bool IntelliRetrieveWrapperPlateParms(CLDSPlate* pPlate);
BOOL ValidateWrapperPlateEdgeSpace(CLDSPlate* pWrapPlate,double epsilon/*=1.0*/,CLDSPlate::EDGEBOLT *endBoltInfoArr/*=NULL*/,int *count/*=NULL*/);
BOOL ValidateWasherPlateEdgeSpace(CLDSPlate* pWasherPlate,double epsilon/*=1.0*/,CLDSPlate::EDGEBOLT *endBoltInfoArr/*=NULL*/,int *count/*=NULL*/);
void CheckPlateEdgeSpace(CXhSimpleList<PLATESPACE_CHKINFO>* pListPlateSpaces)
{
	CHashStrList<bool> hashPlateSpaces;
	for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if(hashPlateSpaces.GetValue(pPlate->Label))
			continue;
		PLATESPACE_CHKINFO  check;
		PLATESPACE_CHKINFO* pPlateSpace=NULL;
		int nSize=0;
		DYN_ARRAY<CLDSPlate::EDGEBOLT> boltInfoArr(pPlate->GetLsCount());
		if(IntelliRetrieveWrapperPlateParms(pPlate))
		{
			check.iSubType=CLDSPlate::WRAPPER_PLATE;
			if(ValidateWrapperPlateEdgeSpace(pPlate,0.5,boltInfoArr,&nSize))
				continue;
		}
		else if(pPlate->wiType==CLDSPlate::WASHER_PLATE)
		{
			check.iSubType=CLDSPlate::WASHER_PLATE;
			if(ValidateWasherPlateEdgeSpace(pPlate,0.5,boltInfoArr,&nSize))
				continue;
		}
		else
		{
			check.iSubType=0;//PLATESPACE_CHKINFO::GENERAL_PLATE;
			if(pPlate->ValidateProfile(1.0,boltInfoArr,&nSize))
				continue;
		}
		for(int i=0;i<nSize;i++)
		{
			pPlateSpace=pListPlateSpaces->AttachObject(check);
			pPlateSpace->needEndSpace=boltInfoArr[i].needEndSpace;
			pPlateSpace->actualEndSpace=boltInfoArr[i].actualEndSpace;
			pPlateSpace->hPlate=pPlate->handle;
			pPlateSpace->hBolt=boltInfoArr[i].hBolt;
			pPlateSpace->segI=pPlate->iSeg;
		}
		hashPlateSpaces.SetValue(pPlate->Label,true);
	}
}
CXhChar200 PLATESPACE_CHKINFO::GetErrorMessage()
{
	CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.FromHandle(hPlate,CLS_PLATE);
	static long hPrevPlate,headstr_len;
	CXhChar200 message;
	if(hPrevPlate!=pPlate->handle)
	{
		CXhChar16 label=pPlate->Label;
		label.ResizeLength(5,' ',true);
		if(iSubType==CLDSPlate::WASHER_PLATE)
			message.Printf("�ְ�(���)#%s�� ",(char*)label);
		else if(iSubType==CLDSPlate::WRAPPER_PLATE)
			message.Printf("�ְ�(����)#%s�� ",(char*)label);
		else
			message.Printf("�ְ�#%s�� ",(char*)label);
		headstr_len=message.Length;
		hPrevPlate=pPlate->handle;
	}
	else
		message.ResizeLength(headstr_len);
	if(fabs(needEndSpace-actualEndSpace)<0.1)
		return "";
	CXhChar16 actualSpaceStr("%.1f",actualEndSpace);
	SimplifiedNumString(actualSpaceStr);
	actualSpaceStr.ResizeLength(4);
	CXhChar16 boltstr("0X%X",hBolt);
	boltstr.ResizeLength(6,' ',true);
	message.Append(CXhChar50("��˨%sʵ�ʱ߾�%s%C �涨�߾�%d",(char*)boltstr,(char*)actualSpaceStr,actualEndSpace>needEndSpace?'>':'<',needEndSpace));
	return message;
}
static void ExportPlateEdgeInfo(CXhSimpleList<PLATESPACE_CHKINFO>* pListPlateSpaces)
{
	for (PLATESPACE_CHKINFO* pEdgeSpace=pListPlateSpaces->EnumObjectFirst();pEdgeSpace;pEdgeSpace=pListPlateSpaces->EnumObjectNext())
	{
		CXhChar200 error=pEdgeSpace->GetErrorMessage();
		if(error.Length==0)
			continue;
		logerr.Log("%s",(char*)error);
	}
}
void CheckRodOddWarning()
{
	CLDSLinePart* pRod=NULL,*pMirRod=NULL;
	CHashStrList<bool>hashCheckedCouples;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL&&pRod->pEnd==NULL)
			continue;
		BOOL pushed=Ta.PushPartStack();
		pMirRod=pRod->GetMirRod(MIRMSG(1));
		if(pMirRod&&pMirRod!=pRod)
		{
			long hMin=min(pRod->handle,pMirRod->handle);
			long hMax=max(pRod->handle,pMirRod->handle);
			CXhChar16 key("%X-%X",hMin,hMax);
			if(hashCheckedCouples.GetValue(key)==NULL)
			{
				if(pRod->startOdd()>0&&pMirRod->startOdd()>0&&pRod->startOdd()!=pMirRod->startOdd())
					logerr.Log("0x%X�˼�{%s}����X��ԳƸ˼�0x%X{%s}ʼ����ͷ��ͬ(%g!=%g)",pRod->handle,(char*)pRod->Label,pMirRod->handle,(char*)pMirRod->Label,pRod->startOdd(),pMirRod->startOdd());
				if(pRod->endOdd()>0&&pMirRod->endOdd()>0&&pRod->endOdd()!=pMirRod->endOdd())
					logerr.Log("0x%X�˼�{%s}����X��ԳƸ˼�0x%X{%s}�ն���ͷ��ͬ(%g!=%g)",pRod->handle,(char*)pRod->Label,pMirRod->handle,(char*)pMirRod->Label,pRod->endOdd(),pMirRod->endOdd());
				if(pRod->cMaterial!=pMirRod->cMaterial)
					logerr.Log("0x%X�˼�����X��ԳƸ˼�0x%X���ʲ�ͬ",pRod->handle,pMirRod->handle);
				hashCheckedCouples.SetValue(key,true);
			}
		}
		pMirRod=pRod->GetMirRod(MIRMSG(2));
		if(pMirRod&&pMirRod!=pRod)
		{
			long hMin=min(pRod->handle,pMirRod->handle);
			long hMax=max(pRod->handle,pMirRod->handle);
			CXhChar16 key("%X-%X",hMin,hMax);
			if(hashCheckedCouples.GetValue(key)==NULL)
			{
				if(pRod->startOdd()>0&&pMirRod->startOdd()>0&&pRod->startOdd()!=pMirRod->startOdd())
					logerr.Log("0x%X�˼�{%s}����Y��ԳƸ˼�0x%X{%s}ʼ����ͷ��ͬ(%g!=%g)",pRod->handle,(char*)pRod->Label,pMirRod->handle,(char*)pMirRod->Label,pRod->startOdd(),pMirRod->startOdd());
				if(pRod->endOdd()>0&&pMirRod->endOdd()>0&&pRod->endOdd()!=pMirRod->endOdd())
					logerr.Log("0x%X�˼�{%s}����Y��ԳƸ˼�0x%X{%s}�ն���ͷ��ͬ(%g!=%g)",pRod->handle,(char*)pRod->Label,pMirRod->handle,(char*)pMirRod->Label,pRod->endOdd(),pMirRod->endOdd());
				if(pRod->cMaterial!=pMirRod->cMaterial)
					logerr.Log("0x%X�˼�����Y��ԳƸ˼�0x%X���ʲ�ͬ",pRod->handle,pMirRod->handle);
				hashCheckedCouples.SetValue(key,true);
			}
		}
		pMirRod=pRod->GetMirRod(MIRMSG(3));
		if(pMirRod&&pMirRod!=pRod)
		{
			long hMin=min(pRod->handle,pMirRod->handle);
			long hMax=max(pRod->handle,pMirRod->handle);
			CXhChar16 key("%X-%X",hMin,hMax);
			if(hashCheckedCouples.GetValue(key)==NULL)
			{
				if(pRod->startOdd()>0&&pMirRod->startOdd()>0&&pRod->startOdd()!=pMirRod->startOdd())
					logerr.Log("0x%X�˼�{%s}����Z��ԳƸ˼�0x%X{%s}ʼ����ͷ��ͬ(%g!=%g)",pRod->handle,(char*)pRod->Label,pMirRod->handle,(char*)pMirRod->Label,pRod->startOdd(),pMirRod->startOdd());
				if(pRod->endOdd()>0&&pMirRod->endOdd()>0&&pRod->endOdd()!=pMirRod->endOdd())
					logerr.Log("0x%X�˼�{%s}����Z��ԳƸ˼�0x%X{%s}�ն���ͷ��ͬ(%g!=%g)",pRod->handle,(char*)pRod->Label,pMirRod->handle,(char*)pMirRod->Label,pRod->endOdd(),pMirRod->endOdd());
				if(pRod->cMaterial!=pMirRod->cMaterial)
					logerr.Log("0x%X�˼�����Z��ԳƸ˼�0x%X���ʲ�ͬ",pRod->handle,pMirRod->handle);
				hashCheckedCouples.SetValue(key,true);
			}
		}
		if(!Ta.PopPartStack(pushed))
			logerr.Log("stack error");
	}
}
void CheckSmallSizeHighMaterialAngle(int maxWide)
{
	CLDSLinePart* pRod=NULL;
	CHashStrList<CLDSLinePart*>hashWarningLabels;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		STEELMAT *pSteel=QuerySteelMatByBriefMark(pRod->cMaterial);
		if(pRod->IsTube()&&pRod->m_cFuncType==CLDSLineTube::FUNC_WIREBUSH&&pRod->cMaterial!=CSteelMatLibrary::Q345BriefMark())
		{
			logerr.Log("�׹�0x%X�Ĺ��Ϊ%g*%g������Ϊ%s",pRod->handle,pRod->size_wide,pRod->size_thick,(char*)pRod->Label,pSteel->mark);
		}
		else if(!pRod->IsAngle())
			continue;
		if(pRod->size_wide>maxWide||pRod->cMaterial=='S')
			continue;
		if(hashWarningLabels.GetValue(pRod->Label))
			continue;
		hashWarningLabels.SetValue(pRod->Label,pRod);
		logerr.Log("���Ϊ%g*%g��0x%X�˼�{%s}����Ϊ��ǿ��%s",pRod->size_wide,pRod->size_thick,pRod->handle,(char*)pRod->Label,pSteel->mark);
	}
}
struct RODINFO{GEPOINT xStart,xEnd,vxWingNorm,vyWingNorm;};
struct POSVARIATION_RECORD : public ICompareClass{
	long hPart;
	float primary,secondary;	//λ���ƶ���
	char ciCutType;	//0.�˼�λ��;1.�Ǹַ���;2.��˨λ��
	CXhChar100 warning;
	void Clear(){primary=secondary=0.0;hPart=ciCutType=0;warning.Empty();}
	virtual int Compare(const ICompareClass *pCompareObj)const
	{
		POSVARIATION_RECORD* other=(POSVARIATION_RECORD*)pCompareObj;
		if(ciCutType!=other->ciCutType)
			return ciCutType-other->ciCutType;
		if(fabs(primary)>fabs(other->primary))
			return -1;
		else if(fabs(primary)<fabs(other->primary))
			return 1;
		else if(fabs(secondary)>fabs(other->secondary))
			return -1;
		else if(fabs(secondary)<fabs(other->secondary))
			return 1;
		else if(hPart<other->hPart)
			return -1;
		else if(hPart>other->hPart)
			return 1;
		else 
			return 0;
	};
	POSVARIATION_RECORD(){primary=secondary=0.0;hPart=ciCutType=0;}
};
void CheckRodVariation()
{
	CSuperSmartPtr<CLDSLinePart> pRod;
	CHashList<RODINFO> hashRods;
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
	{
		RODINFO rod;
		rod.xStart=pRod->Start();
		rod.xEnd=pRod->End();
		if(pRod->IsAngle())
		{
			rod.vxWingNorm=pRod.pAngle->vxWingNorm;
			rod.vyWingNorm=pRod.pAngle->vyWingNorm;
		}
		else if(pRod->IsTube())
			rod.vxWingNorm=pRod.pTube->WorkPlaneNorm();
		hashRods.SetValue(pRod->handle,rod);
	}
	ARRAY_LIST<POSVARIATION_RECORD> warnings;
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
	{
		pRod->CalPosition(true);
		if(pRod->IsAngle())
		{
			pRod.pAngle->cal_x_wing_norm(true);
			pRod.pAngle->cal_x_wing_norm(true);
		}
		RODINFO* pRodInfo=hashRods.GetValue(pRod->handle);
		f3dPoint offset_start=pRodInfo->xStart-GEPOINT(pRod->Start());
		f3dPoint offset_end=pRodInfo->xEnd-GEPOINT(pRod->End());
		POSVARIATION_RECORD vary,*pPrevVary=NULL;
		vary.hPart=pRod->handle;
		if(offset_start.mod()>0.1)
		{
			vary.ciCutType=0;
			vary.primary=(float)(fabs(offset_start.x)+fabs(offset_start.y)+fabs(offset_start.z));
			vary.warning.Printf("0X%X�Ǹ�ʼ�˦�x=%f;��y=%f;��z=%f\n",pRod->handle,offset_start.x,offset_start.y,offset_start.z);
			pPrevVary=warnings.append(vary);
		}
		if(offset_end.mod()>0.1)
		{
			vary.ciCutType=0;
			vary.primary=max(vary.primary,(float)(fabs(offset_end.x)+fabs(offset_end.y)+fabs(offset_end.z)));
			vary.warning.Printf("0X%X�Ǹ��ն˦�x=%f;��y=%f;��z=%f\n",pRod->handle,offset_end.x,offset_end.y,offset_end.z);
			if(pPrevVary)
				pPrevVary->primary=vary.primary;
			pPrevVary=warnings.append(vary);
		}
		if(pRod->IsAngle())
		{
			double cosax=pRodInfo->vxWingNorm*pRod.pAngle->vxWingNorm;
			double cosay=pRodInfo->vyWingNorm*pRod.pAngle->vyWingNorm;
			double sinax=SQRT(1-cosax*cosax);
			double sinay=SQRT(1-cosay*cosay);
			pPrevVary=NULL;
			vary.primary=0;
			if(sinax>0.01)
			{
				vary.ciCutType=1;
				vary.primary=(float)sinax;
				vary.warning.Printf("0X%X�Ǹ�X֫���߻ᷢ���仯��sin(a)=%.3f\n",pRod->handle,sinax);
				pPrevVary=warnings.append(vary);
			}
			if(sinay>0.01)
			{
				vary.ciCutType=1;
				vary.primary=max(vary.primary,(float)sinay);
				vary.warning.Printf("0X%X�Ǹ�Y֫���߻ᷢ���仯��sin(a)=%.3f\n",pRod->handle,sinay);
				if(pPrevVary)
					pPrevVary->primary=vary.primary;
				pPrevVary=warnings.append(vary);
			}
		}
	}
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
	{
		RODINFO* pRodInfo=hashRods.GetValue(pRod->handle);
		pRod->SetStart(pRodInfo->xStart);
		pRod->SetEnd(pRodInfo->xEnd);
		if(pRod->IsAngle())
		{
			pRod.pAngle->SetBriefWingNormX(pRodInfo->vxWingNorm,false);
			pRod.pAngle->SetBriefWingNormY(pRodInfo->vyWingNorm,false);
		}
	}
	CSuperSmartPtr<CLDSPart> pPart;
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT)
		{
			GEPOINT pos=pPart.pBolt->ucs.origin;
			GEPOINT normal=pPart.pBolt->ucs.axis_z;
			pPart.pBolt->correct_pos();
			pPart.pBolt->correct_worknorm();
			GEPOINT offset=pos-GEPOINT(pPart.pBolt->ucs.origin);
			double offset_L=offset.mod();
			double offset_z=offset*normal;
			GEPOINT vOffset=offset-GEPOINT(offset_z*normal);
			double offset_h=vOffset.mod();
			if(offset_h>0.1||offset_L>1)
			{
				POSVARIATION_RECORD vary;
				vary.hPart=pPart->handle;
				vary.ciCutType=2;
				vary.primary=ftoi(offset_h*10)*0.1f;
				vary.secondary=(float)fabs(offset_z);
				vary.warning.Printf("0X%X��˨λ�æ�H=%.1f;��z=%.1f\n",pPart->handle,offset_h,offset_z);
				warnings.append(vary);
			}
			pPart.pBolt->ucs.origin=pos;
			pPart.pBolt->ucs.axis_z=normal;
		}
	}
	if(warnings.GetSize()==0)
		return;
	CHeapSort<POSVARIATION_RECORD>::HeapSortClassic(warnings.m_pData,warnings.GetSize());
	logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"*********����λ�ñ䶯��ʾ��Ϣ*********");
	for(int i=0;i<warnings.GetSize();i++)
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,(char*)warnings[i].warning);
}

struct CUTANGLERECORD : public ICompareClass{
	union{
		struct{WORD smallC,bigC;};
		DWORD width;
	};
	char ciCutType;	//0.�н�;1.��֫
	CXhChar100 warning;
	virtual int Compare(const ICompareClass *pCompareObj)const
	{
		CUTANGLERECORD* other=(CUTANGLERECORD*)pCompareObj;
		if(ciCutType!=other->ciCutType)
			return ciCutType-other->ciCutType;
		if(width<other->width)
			return -1;
		else if(width>other->width)
			return 1;
		else
			return 0;
	};
	CUTANGLERECORD(){smallC=bigC=0;ciCutType=0;}
};
static  BOOL CheckCutAnlgeInfo(CLDSLineAngle* pAngle,CUTANGLERECORD& cutting) 
{	//nCutPara��0����	1�����н� 2������֫  
	//����н�
	const double fMIN_CUT_ANGLE_LEN = g_sysPara.m_fMinCutAngleLen;     //��С�н�ֵĬ��Ϊ10��
	CXhChar100 sCutAngle="";
	for(int i=0;i<4;i++)
	{
		char cutPosWing[3]=" W";
		i<=1?cutPosWing[0]='S':cutPosWing[0]='E';
		cutPosWing[1]='X'+i%2;
		if((pAngle->cut_angle[i][0]>0&&pAngle->cut_angle[i][0]<fMIN_CUT_ANGLE_LEN)||
			(pAngle->cut_angle[i][1]>0&&pAngle->cut_angle[i][1]<fMIN_CUT_ANGLE_LEN))
		{
			CXhChar50 cutstr("%s:%.0fX%.0f",cutPosWing,pAngle->cut_angle[i][1],pAngle->cut_angle[i][0]);
			sCutAngle.Append(cutstr,';');
			cutting.smallC=min(ftoi(pAngle->cut_angle[i][1]),ftoi(pAngle->cut_angle[i][0]));
			cutting.bigC=max(ftoi(pAngle->cut_angle[i][1]),ftoi(pAngle->cut_angle[i][0]));
			cutting.ciCutType=0;
		}
	}
	//�����֫
	if(pAngle->cut_wing[0]!=0||pAngle->cut_wing[1]!=0)
	{
		for(int i=0;i<2;i++)
		{
			char cutPosWing[3]=" W";
			i==0?cutPosWing[0]='S':cutPosWing[0]='E';
			if(pAngle->cut_wing[i]==1||pAngle->cut_wing[i]==2)
				cutPosWing[1]+=(char)pAngle->cut_wing[i];
			else
				continue;
			CXhChar50 cutstr="";
			if( (pAngle->cut_wing_para[i][0]>0&&pAngle->cut_wing_para[i][0]<fMIN_CUT_ANGLE_LEN))//||
				//(pAngle->cut_wing_para[i][1]>0&&pAngle->cut_wing_para[i][1]<fMIN_CUT_ANGLE_LEN)||
				//(pAngle->cut_wing_para[i][2]>0&&pAngle->cut_wing_para[i][2]<fMIN_CUT_ANGLE_LEN))	//����֫֫���и��Ȳ�У׼
			{
				int alfa = ftoi(DEGTORAD_COEF*atan((pAngle->cut_wing_para[i][2]-pAngle->cut_wing_para[i][0])/pAngle->GetWidth()));
				CXhChar16 alfa_str("+CO^%d��",alfa);
				if(pAngle->cut_wing_para[i][1]==0)	//����֫
					cutstr.Printf("%s:CF%.0f",cutPosWing,pAngle->cut_wing_para[i][0]);
				else	//����֫	
					cutstr.Printf("%s:BC%.0fX%.0f",cutPosWing,pAngle->cut_wing_para[i][0],pAngle->cut_wing_para[i][1]);
				if(alfa>0)
					cutstr.Append(alfa_str);
				cutting.smallC=min(cutting.smallC,ftoi(pAngle->cut_angle[i][0]));
				cutting.bigC=min(cutting.bigC,100);	//�������һֵ����bigC=0 wjh-2017.3.8
				sCutAngle.Append(cutstr,';');
				cutting.ciCutType=1;
			}
		}
	}
	if(sCutAngle.Length>0)
	{
		CXhChar16 labelstr("0X%X",pAngle->handle);
		labelstr.ResizeLength(6,' ',true);
		labelstr.Append(CXhChar16("{%s}",(char*)pAngle->Label));
		labelstr.ResizeLength(14);
		if(strchr(sCutAngle,'C')!=NULL)
			cutting.warning.Printf("%s�Ǹ���֫��Ϣ��:%s",(char*)labelstr,(char*)sCutAngle);
		else
			cutting.warning.Printf("%s�Ǹ��н���Ϣ��:%s",(char*)labelstr,(char*)sCutAngle);
	}
	return sCutAngle.Length==0;
}

//7.�нǾ�ʾ��Ϣ
void CheckCutAngleInfoWarning()
{
	CHashStrList<bool> hashCutAngle;
	ARRAY_LIST<CUTANGLERECORD> warnings(0,50);
	for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		CUTANGLERECORD cutting;
		if(hashCutAngle.GetValue(pAngle->Label)||CheckCutAnlgeInfo(pAngle,cutting))
			continue;
		if(cutting.warning.Length>0)
		{
			warnings.append(cutting);
			hashCutAngle.SetValue(pAngle->Label,true);
		}
	}
	if(warnings.GetSize()==0)
		return;
	CQuickSort<CUTANGLERECORD>::QuickSortClassic(warnings.m_pData,warnings.GetSize());
	logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"*********�Ǹ��нǾ�ʾ��Ϣ*********");
	for(int i=0;i<warnings.GetSize();i++)
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,(char*)warnings[i].warning);
}
//9.���������϶��ƫ��׼�౨��
void ExportHuoQuSpaceAndOffsetG()
{
	logerr.ClearContents();
	const	int HUOQU_DIST=10,OFFSET_ZHUN=5;
	for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		CHashStrList<CXhChar500> hashLableStrByHouQuDist;
		CHashStrList<CXhChar500> hashLableStrByOffsetZhun;
		int dxs =pPlate->GetFaceN();
		if(pPlate->GetFaceN()<2)
			continue;
		for (CDesignLjPartPara* pLjPartPara=pPlate->GetFirstLjPara();pLjPartPara;pLjPartPara=pPlate->GetNextLjPara())
		{
			if(pLjPartPara->iFaceNo==1)
				continue;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pLjPartPara->hPart,CLS_LINEANGLE);
			if(pAngle==NULL)
				continue;
			CMultiOffsetPos* pDesPos=(pLjPartPara->start0_end1==0)?&pAngle->desStartPos:&pAngle->desEndPos;
			CXhChar200 sLog("%s(0X%X)",(char*)pAngle->GetPartNo(),pAngle->handle);
			if((pDesPos->IsInHuoQuLine()||pDesPos->IsInOffsetZhunLine())&&pDesPos->huoqu_dist!=HUOQU_DIST)
			{
				CXhChar500 *pLogStr=hashLableStrByHouQuDist.GetValue(CXhChar16("%.1f",pDesPos->huoqu_dist));
				if (pLogStr!=NULL&&pLogStr->GetLength()>0)
					pLogStr->Append(";");
				if(pLogStr==NULL)
					pLogStr=hashLableStrByHouQuDist.Add(CXhChar16("%.1f",pDesPos->huoqu_dist));
				pLogStr->Append(sLog);
			}
			if(pDesPos->IsInOffsetZhunLine()&&pDesPos->offset_zhun!=OFFSET_ZHUN)
			{
				CXhChar500 *pLogStr=hashLableStrByOffsetZhun.GetValue(CXhChar16("%.1f",pDesPos->offset_zhun));
				if (pLogStr!=NULL&&pLogStr->GetLength()>0)
					pLogStr->Append(";");
				if(pLogStr==NULL)
					pLogStr=hashLableStrByOffsetZhun.Add(CXhChar16("%.1f",pDesPos->offset_zhun));
				pLogStr->Append(sLog);
			}
		}
		CXhChar200 sLable("%s(0X%X)�ְ���",(char*)pPlate->GetPartNo(),pPlate->handle);
		if(hashLableStrByHouQuDist.GetNodeNum()>0)
		{
			for(CXhChar500* pStr=hashLableStrByHouQuDist.GetFirst();pStr;pStr=hashLableStrByHouQuDist.GetNext())
			{
				CXhChar100 str("������϶Ϊ%s��%s",hashLableStrByHouQuDist.GetCursorKey(),(char*)(*pStr));
				sLable+=str;
			}
			logerr.Log(sLable);
		}
		if(hashLableStrByOffsetZhun.GetNodeNum()>0)
		{
			for(CXhChar500* pStr=hashLableStrByOffsetZhun.GetFirst();pStr;pStr=hashLableStrByOffsetZhun.GetNext())
			{
				CXhChar100 str("ƫ��׼��%s:%s ",hashLableStrByOffsetZhun.GetCursorKey(),(char*)*pStr);
				sLable+=str;
			}
			logerr.Log(sLable);
		}
	}
	logerr.ShowToScreen();
}
//10.�Ǹ�����������ʲ�һ�±���
typedef  CLDSGeneralPlate* CLDSGENERALPLATEPTR; 
static int ComparePartLabel(const CLDSGENERALPLATEPTR& item1,const CLDSGENERALPLATEPTR& item2)
{
	return ComparePartNoString(item1->GetPartNo(),item2->GetPartNo());
}
static int SteelLevel(char cMaterial)
{
	if(cMaterial==CSteelMatLibrary::Q235BriefMark())
		return 1;
	else if(cMaterial==CSteelMatLibrary::Q345BriefMark())
		return 2;
	else if(cMaterial==CSteelMatLibrary::Q390BriefMark())
		return 3;
	else if(cMaterial==CSteelMatLibrary::Q420BriefMark())
		return 4;
	else if(cMaterial==CSteelMatLibrary::Q460BriefMark())
		return 5;
	else
		return 0;
}
static CLDSLineAngle* IntelliRecogSharePlateInsideRayAngle(CLDSPlate* pPlate)
{
	if(pPlate->designInfo.partList.GetNodeNum()!=3)
		return NULL;
	if(fabs(pPlate->ucs.axis_z.z)<0.98&&fabs(pPlate->ucs.axis_z.x)<0.98)
		return NULL;	//��ˮƽ��
	//TODO:δ���ʶ��
	if(pPlate->designInfo.origin.datum_pos_style==3)
	{
		long hAngle1=pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1;
		long hAngle2=pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2;
		CLDSLineAngle* pDatumAngle1=(CLDSLineAngle*)Ta.Parts.FromHandle(hAngle1,CLS_LINEANGLE);
		CLDSLineAngle* pDatumAngle2=(CLDSLineAngle*)Ta.Parts.FromHandle(hAngle2,CLS_LINEANGLE);
		CLDSLineAngle* pInsideRayAngle=NULL;
		for (CDesignLjPartPara* pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			if(pLjPara->hPart==hAngle1&&pLjPara->start0_end1==0&&pDatumAngle1&&pDatumAngle1->startOdd()>0)
				return NULL;	//���ð���һ��׼�˼��ĵ�ǰ���Ӷ˿϶��Ǹ�ͷ
			if(pLjPara->hPart==hAngle1&&pLjPara->start0_end1==1&&pDatumAngle1&&pDatumAngle1->endOdd()>0)
				return NULL;
			if(pLjPara->hPart==hAngle2&&pLjPara->start0_end1==0&&pDatumAngle1&&pDatumAngle2->startOdd()>0)
				return NULL;
			if(pLjPara->hPart==hAngle2&&pLjPara->start0_end1==1&&pDatumAngle1&&pDatumAngle2->endOdd()>0)
				return NULL;
			if(pLjPara->hPart==hAngle1||pLjPara->hPart==hAngle2)
				continue;
			CLDSLinePart* pRod=Ta.FromRodHandle(pLjPara->hPart);
			if(pInsideRayAngle||!pRod->IsAngle())
				return NULL;	//���ð岻�����м�����������߸˼���ǽǸ����߸˼�
			pInsideRayAngle=(CLDSLineAngle*)pRod;
		}
		CLDSNode* pDatumNode=Ta.Node.FromHandle(pPlate->designInfo.m_hBaseNode);
		if(pDatumNode&&(pDatumNode->hFatherPart==hAngle1||pDatumNode->hFatherPart==hAngle2))
			return NULL;	//���ð�Ļ�׼�ڵ�ĸ��˼�����������һ����λ��׼�˼�
		if(pInsideRayAngle)	//���Գ����ж�Ϊ���ð�
			return pInsideRayAngle;
	}
	return NULL;
}
void ExportAngleAndPlateMatDiff()
{  //����ְ�
	CHashStrList<bool> hashPlateByLable;
	ARRAY_LIST<CLDSGeneralPlate*> partArr;
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=(CLDSPart*)Ta.Parts.GetNext())
	{
		if (!pPart->IsPlate()||hashPlateByLable.GetValue(pPart->GetPartNo()))
			continue;
		partArr.append((CLDSGeneralPlate*)pPart);
		hashPlateByLable.SetValue(pPart->GetPartNo(),true);
	}
	CBubbleSort<CLDSGeneralPlate*>::BubSort(partArr.m_pData,partArr.GetSize(),ComparePartLabel);
	//��ȡ�����ʲ�ͬ�Ĺ���
	for (int i=0;i<partArr.GetSize();i++)
	{
		CLDSPart* pPart= partArr[i];
		CXhChar500 sLog="";
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate* pPlate=(CLDSPlate*)pPart;
			CLDSLineAngle* pInsideRayAngle=IntelliRecogSharePlateInsideRayAngle(pPlate);
			CLDSLinePart* pDatumAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pPlate->designInfo.m_hBasePart,CLS_LINEANGLE);
			for (CDesignLjPartPara* pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
			{
				CLDSLinePart* pRod=NULL;
				if(pInsideRayAngle&&pLjPara->hPart!=pInsideRayAngle->handle)
					continue;
				else if(pInsideRayAngle)
					pRod=pInsideRayAngle;
				else
					pRod=Ta.FromRodHandle(pLjPara->hPart);
				if(pRod==NULL||SteelLevel(pPlate->cMaterial)>=SteelLevel(pRod->cMaterial))
					continue;
				if(!((pRod->pStart&&pRod->pStart->handle==pPlate->designInfo.m_hBaseNode)||
					 (pRod->pEnd&&pRod->pEnd->handle==pPlate->designInfo.m_hBaseNode)))
					 continue;
				bool hasPlateConnBolt=false;
				for(CLsRef* pLsRef=pPlate->GetFirstLsRef();pDatumAngle&&pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					if(pRod->FindLsByHandle(pLsRef->pBolt->handle)==NULL)
						continue;
					if(pDatumAngle->FindLsByHandle(pLsRef->pBolt->handle)==NULL)
					{
						hasPlateConnBolt=true;
						break;
					}
				}
				if(!hasPlateConnBolt)
					continue;	//�����߽Ǹ��ϵ�������˨���Ƕ�������˨��ֱ�����ӵ���׼���ϣ�
				if(pRod->handle==pPlate->designInfo.m_hBasePart)
				{
					if(pRod->pStart&&pRod->pStart->handle==pPlate->designInfo.m_hBaseNode&&pRod->startOdd()>-25)
						continue;
					else if(pRod->pEnd&&pRod->pEnd->handle==pPlate->designInfo.m_hBaseNode&&pRod->endOdd()>-25)
						continue;
				}
				if(sLog.Length>0)
					sLog.Append("\n\t\t\t ");
				CXhChar16 label=pRod->GetPartNo();
				label.ResizeLength(8,' ',true);
				CXhChar16 idstr("(0X%X)",pRod->handle);
				idstr.ResizeLength(8,' ',true);
				STEELMAT* pSteel=QuerySteelMatByBriefMark(pRod->cMaterial);
				sLog.Append(CXhChar100("�Ǹ�%s%s����%8s",(char*)label,(char*)idstr,pSteel->mark));
			}
		}
		else //if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*) pPart;
			if(pParamPlate->m_iParamType!=TYPE_SHOEPLATE)
				continue;
			CXhChar16 strArr[2]={"LX","LH"};
			for (int i=0;i<2;i++)
			{
				long hAngle=0;
				pParamPlate->GetDesignItemValue(KEY2C(strArr[i]),&hAngle);
				CLDSPart* pAngle=Ta.FromPartHandle(hAngle);
				if(pAngle==NULL||!pAngle->IsAngle())
					continue;
				if(SteelLevel(pPart->cMaterial)>=SteelLevel(pAngle->cMaterial))
					continue;
				if(sLog.Length>0)
					sLog.Append("\n\t\t\t ");
				CXhChar16 sMat;
				QuerySteelMatMark(pAngle->cMaterial,sMat);
				CXhChar16 sPartHandle("(0X%X)",pAngle->handle);
				sLog.Append(CXhChar100("ѥ��%8s����%8s",(char*)sPartHandle,(char*)sMat));
			}
		}
		if(sLog.Length>0)
		{
			CXhChar16 sMat;
			QuerySteelMatMark(pPart->cMaterial,sMat);
			logerr.Log("�ְ�0X%X(%s)����%s��%s",pPart->handle,(char*)pPart->GetPartNo(),(char*)sMat,(char*)sLog);
		}
	}
}
void FinalFirstWarningIdStr(IXhString *pXhStr,bool* firstWarning)
{
	if(*firstWarning)
	{
		*firstWarning=false;
		int length=pXhStr->Length;
		pXhStr->Empty();
		pXhStr->ResizeLength(length);
	}
}
typedef CLDSLineAngle* CLDSLineAnglePtr;
static int _LocalCompareAngleLabels(const CLDSLineAnglePtr& angleptr1,const CLDSLineAnglePtr& angleptr2)
{
	return ComparePartNoString(angleptr1->Label,angleptr2->Label,"HP");
}
void CheckMissingLabelPart()
{
	CSuperSmartPtr<CLDSPart> pPart;
	struct SEGLABEL : public ICompareClass{
		SEGI idSeg;
		UINT uiMaxSerial,hLabelPart;
		char label[17];
		SEGLABEL(){uiMaxSerial=hLabelPart=0;memset(label,0,17);}
		void SetKey(DWORD key){idSeg=key;}
		int Compare(const ICompareClass *pCompareObj)const
		{
			SEGLABEL* pOther=(SEGLABEL*)pCompareObj;
			if(idSeg.Digital()!=pOther->idSeg.Digital())
				return idSeg.Digital()-pOther->idSeg.Digital();
			else
				return idSeg.iSeg-pOther->idSeg.iSeg;
		}
	};
	SEGLABEL* pSegLabel;
	CHashListEx<SEGLABEL>hashSegMaxLabels;	//��ֵΪ�κ�,����Ϊ�ö��ڼ��ŵ������ˮ��
	for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
	{
		if(pPart->m_bVirtualPart||pPart->GetClassTypeId()==CLS_BOLT||pPart->iSeg.iSeg==0)
			continue;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&(pPart.pParamPlate->IsFL()||pPart.pParamPlate->IsInsertPlate()))
			continue;
		char serialstr[17]={0};
		ParsePartNo(pPart->Label,NULL,serialstr);
		UINT serial=atoi(serialstr);
		pSegLabel=hashSegMaxLabels.Add(pPart->iSeg);
		if(pSegLabel->uiMaxSerial<serial||pSegLabel->uiMaxSerial==0)
		{
			pSegLabel->uiMaxSerial=serial;
			pSegLabel->hLabelPart=pPart->handle;
			StrCopy(pSegLabel->label,pPart->Label,17);
		}
	}
	DYN_ARRAY<SEGLABEL>seglabels(hashSegMaxLabels.GetNodeNum());
	UINT i=0,index=0;
	for(pSegLabel=hashSegMaxLabels.GetFirst(),i=0;pSegLabel;pSegLabel=hashSegMaxLabels.GetNext(),i++)
		seglabels[i]=*pSegLabel;
	CQuickSort<SEGLABEL>::QuickSortClassic(seglabels,seglabels.Size());
	DYN_ARRAY<int>places(100);
	logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"*********��������©ȱʧ��ʾ��Ϣ*********");
	for(UINT segI=0;segI<seglabels.Size();segI++)
	{
		pSegLabel = &seglabels[segI];
		places.Resize(pSegLabel->uiMaxSerial);
		memset((int*)places,0,places.Size()*4);
		for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
		{
			if(pPart->m_bVirtualPart||pPart->GetClassTypeId()==CLS_BOLT||pPart->iSeg!=pSegLabel->idSeg)
				continue;
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&(pPart.pParamPlate->IsFL()||pPart.pParamPlate->IsInsertPlate()))
				continue;
			char serialstr[17]={0};
			ParsePartNo(pPart->Label,NULL,serialstr);
			UINT serial=atoi(serialstr);
			if(serial>0&&serial<=places.Size())
				places[serial-1]+=places[serial-1]+1;
		}
		bool firstWarning=true;
		for(i=0,index=1;i<places.Size();i++)
		{
			if(places[i]==0)
			{
				CXhChar16 label=pSegLabel->idSeg.ToString();
				if(i+1<10)
					label.Append(CXhChar16("0%d",i+1));
				else if(i+1<100)
					label.Append(CXhChar16("%2d",i+1));
				else
					label.Append(CXhChar16("-%2d",i+1));
				if(firstWarning)
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%s�ֶ�������'%s'(0x%X), �ö�ȱʧ����",(char*)pSegLabel->idSeg.ToString(),pSegLabel->label,pSegLabel->hLabelPart);
				firstWarning=false;
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"        %2d#%s",index,(char*)label);
				index++;
			}
		}
		if(firstWarning)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%s�ֶ�������'%s'(0x%X)",(char*)pSegLabel->idSeg.ToString(),pSegLabel->label,pSegLabel->hLabelPart);
	}
}
struct CROSSNODE{
	long hNode,hRod,hCrossRod,nConnBolts;
	void SetKey(DWORD key){hNode=key;}
	CROSSNODE(){hNode=hRod=hCrossRod=nConnBolts=0;}
};
struct CROSSNODEPOS{
	bool bFindConnBolt;
	BYTE ciResideWing;	//'X','Y';
	short siLenPosToStart;	//ʵ�ʼ���ʱ���ڽ�����˨λ�ڽǸ�֮���������Ʊ������۽����)����ʱΪ��ֵ wjh-2017.11.19
	CROSSNODE* pCrossNode;
	CROSSNODEPOS(CROSSNODE* _pCrossNode=NULL,CLDSLineAngle* pCurrAngle=NULL){
		bFindConnBolt=false;
		ciResideWing=0;
		siLenPosToStart=0;
		pCrossNode=_pCrossNode;
		if(pCrossNode&&pCurrAngle)
		{
			CLDSLinePart* pCrossRod=NULL;
			if(pCrossNode->hRod==pCurrAngle->handle)
				pCrossRod=Ta.FromRodHandle(pCrossNode->hCrossRod);
			else
				pCrossRod=Ta.FromRodHandle(pCrossNode->hRod);
			CLDSNode* pDatumNode=Ta.Node.FromHandle(pCrossNode->hNode);
			if(pCrossRod&&pDatumNode)
			{
				GEPOINT rodvec =pCurrAngle->End()-pCurrAngle->Start();
				GEPOINT rodvec2=pCrossRod->End()-pCrossRod->Start();
				GEPOINT worknorm=rodvec^rodvec2;
				normalize(rodvec);
				normalize(worknorm);
				if(fabs(pCurrAngle->vxWingNorm*worknorm)>fabs(pCurrAngle->vyWingNorm*worknorm))
				{
					ciResideWing='X';
					f3dPoint perppos=pCurrAngle->GetDatumPosWingX(pDatumNode);
					siLenPosToStart=(short)ftoi((perppos-pCurrAngle->Start())*rodvec+pCurrAngle->startOdd());
				}
				else
				{
					ciResideWing='Y';
					f3dPoint perppos=pCurrAngle->GetDatumPosWingY(pDatumNode);
					siLenPosToStart=(short)ftoi((perppos-pCurrAngle->Start())*rodvec+pCurrAngle->startOdd());
				}
			}
		}
	}
};
//����8��������˨�ľ��⼰ȱ�ף��˿׻򽻲�ף�����
void CheckMissingConnect(bool bCheckMissingHole,bool bExportSpecialAngleG)
{
	CLDSNode* pNode;
	CLDSLinePart* pRod;
	CLDSLineAngle* pAngle;
	CROSSNODE* pCrossNode;
	CHashListEx<CROSSNODE> hashCrossNodes;
	CHashList<bool>hashCrossRods;
	if(bCheckMissingHole)
	{
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->m_cPosCalType==4&&pNode->m_cPosCalViceType==0&&(pNode->arrRelationPole[0]>0x20&&pNode->arrRelationPole[1]>0x20))
			{
				pCrossNode=hashCrossNodes.Add(pNode->handle);
				pCrossNode->hRod=pNode->arrRelationPole[0];
				pCrossNode->hCrossRod=pNode->arrRelationPole[1];
				hashCrossRods.SetValue(pCrossNode->hRod,true);
				hashCrossRods.SetValue(pCrossNode->hCrossRod,true);
			}
		}
	}
	CXhChar100 headername="*********�Ǹ�������ʧ����λ�쳣��ʾ��Ϣ*********";
	if(bCheckMissingHole&&!bExportSpecialAngleG)
		headername.Copy("****************�Ǹ�������ʧ�쳣��ʾ��Ϣ****************");
	else if(!bCheckMissingHole&&bExportSpecialAngleG)
		headername.Copy("**************�Ǹ����ӿ�λ�ľ��쳣��ʾ��Ϣ**************");
	bool occurFirst=true;
	ARRAY_LIST<CLDSLineAngle*> angles(0,1024);
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if (pRod->m_bVirtualPart||pRod->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		angles.append((CLDSLineAngle*)pRod);
	}
	CHeapSort<CLDSLineAngle*>::HeapSort(angles.m_pData,angles.GetSize(),_LocalCompareAngleLabels);
	for(int j=0;j<angles.GetSize();j++)
	{
		pAngle=angles[j];
		double length=pAngle->GetLength();
		bool hasStartConnBolt=false,hasEndConnBolt=false;
		long hCrossRod=0;
		JGZJ xZhunJu=pAngle->GetZhunJu('X');
		JGZJ yZhunJu=pAngle->GetZhunJu('Y');
		char data[51]={0};
		CXhString idstr(data,51);
		idstr.Printf("�Ǹ�0x%X{%s},W=%.0f",pAngle->handle,pAngle->Label,pAngle->size_wide);
		idstr.ResizeLength(20);
		int idstrlen=idstr.Length;
		bool firstWarning=true;
		CXhSimpleList<CROSSNODEPOS>listCrossNodes;
		if(bCheckMissingHole)
		{
			for(pCrossNode=hashCrossNodes.GetFirst();pCrossNode;pCrossNode=hashCrossNodes.GetNext())
			{
				if(pCrossNode->hRod==pAngle->handle||pCrossNode->hCrossRod==pAngle->handle)
				{
					CROSSNODEPOS crosspos(pCrossNode,pAngle);
					if(crosspos.siLenPosToStart>0&&crosspos.siLenPosToStart<length)
						listCrossNodes.AttachObject(crosspos);	//���ڴ��ڴ�ϽǸֵ�Ե�ʣ����ڽ��㲻�ڽǸַ�Χ֮�ڵ���� wjh-2017.11.19
				}
			}
		}
		CROSSNODEPOS* pCrossPos;
		bool hasCrossNodeMissBolt=hashCrossNodes.GetNodeNum()>0;
		for(CLsRef* pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->pBolt;
			GEPOINT pos;
			if(!pAngle->GetBoltLocalPos(pBolt,pos,true))
				continue;
			if(pos.x<=50.4)
				hasStartConnBolt=true;
			if(length-pos.x<=50.4)
				hasEndConnBolt=true;
			if(hasCrossNodeMissBolt&&bCheckMissingHole)
			{	//�ø˼�ͨ��ʼĩ��������ʧ���
				bool passcheck=true;
				for(pCrossPos=listCrossNodes.EnumObjectFirst();hasCrossNodeMissBolt&&pCrossPos;pCrossPos=listCrossNodes.EnumObjectNext())
				{
					if(pCrossPos->bFindConnBolt)
						continue;
					if(pCrossPos->ciResideWing=='X'&&pos.y<0&&fabs(pCrossPos->siLenPosToStart-pos.x)<pAngle->size_wide)
						pCrossPos->bFindConnBolt=true;
					else if(pCrossPos->ciResideWing=='Y'&&pos.y>0&&fabs(pCrossPos->siLenPosToStart-pos.x)<pAngle->size_wide)
						pCrossPos->bFindConnBolt=true;
					else
						passcheck=false;
				}
				hasCrossNodeMissBolt=!passcheck;
			}
			if(bExportSpecialAngleG&&pos.y<0)
			{	//X֫��˨
				long* garr=&xZhunJu.g;
				bool validate=false;
				int gy=ftoi(pos.y);
				int perfectg=xZhunJu.g,perfectg_diff=5;
				for(int i=0;i<4;i++)
				{
					if(garr[i]!=0&&garr[i]==-gy)
					{
						validate=true;
						break;
					}
					else if(abs(garr[i]+gy)<perfectg_diff)
					{
						perfectg_diff=abs(garr[i]+gy);
						perfectg=garr[i];
					}
					//if(pBolt->get_d()==20&&-gy>pAngle->size_wide-40) ֫��-�߾����͵������ľ�
				}
				if(!validate)
				{
					if(occurFirst)
						logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,headername);
					occurFirst=false;
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%s��X֫M%d��˨0x%X�ľ��쳣Xg=%d,��%d",data,pBolt->get_d(),pBolt->handle,-gy,perfectg);
					FinalFirstWarningIdStr(&idstr,&firstWarning);
				}
			}
			else if(bExportSpecialAngleG&&pos.y>0)
			{	//Y֫��˨
				long* garr=&yZhunJu.g;
				bool validate=false;
				int gy=ftoi(pos.y);
				int perfectg=xZhunJu.g,perfectg_diff=5;
				for(int i=0;i<4;i++)
				{
					if(garr[i]!=0&&garr[i]==gy)
					{
						validate=true;
						break;
					}
					else if(abs(garr[i]+gy)<perfectg_diff)
					{
						perfectg_diff=abs(garr[i]+gy);
						perfectg=garr[i];
					}
				}
				if(!validate)
				{
					if(occurFirst)
						logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,headername);
					occurFirst=false;
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%s��Y֫M%d��˨0x%X�ľ��쳣Yg=%d,��%d",data,pBolt->get_d(),pBolt->handle,gy,perfectg);
					FinalFirstWarningIdStr(&idstr,&firstWarning);
				}
			}
		}
		if(!bCheckMissingHole)
			continue;	//�ݲ��������������ӿ���ʧ����
		for(pCrossPos=listCrossNodes.EnumObjectFirst();hasCrossNodeMissBolt&&pCrossPos;pCrossPos=listCrossNodes.EnumObjectNext())
		{
			if(pCrossPos->bFindConnBolt)
				continue;
			if(occurFirst)
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,headername);
			occurFirst=false;
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%s��0x%X����㴦ȱʧ��˨",data,pCrossPos->pCrossNode->hNode);
			FinalFirstWarningIdStr(&idstr,&firstWarning);
		}
		if(!hasStartConnBolt||!hasEndConnBolt)
		{
			if(occurFirst)
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,headername);
			occurFirst=false;
			if(!hasStartConnBolt)
			{
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%sʼ��������˨ȱʧ",data);
				FinalFirstWarningIdStr(&idstr,&firstWarning);
			}
			if(!hasEndConnBolt)
			{
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%s�ն�������˨ȱʧ",data);
				FinalFirstWarningIdStr(&idstr,&firstWarning);
			}
		}
	}
}

//11.Ԥ���߶ȱ���(���ؼ����ƽڵ�ĵ��߼�ʵ��Ԥ���߶�)���쳣��ʾ
void ExportArcLiftWarning()
{
	CMapList<bool> hahsArcLiftBySegIAndLiftName;;
	for (CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if((pNode->m_cPosCalType!=CLDSNode::COORD_INDEPENDENT&&
			pNode->m_cPosCalType!=CLDSNode::COORD_X_SPECIFIED&&
			pNode->m_cPosCalType!=CLDSNode::COORD_Y_SPECIFIED&&
			pNode->m_cPosCalType!=CLDSNode::COORD_Z_SPECIFIED&&
			pNode->m_cPosCalType <CLDSNode::PLANE_XY_SPECIFIED))
			continue;
		CLDSArcLift *pLift=pNode->BelongModel()->FromArcLiftHandle(pNode->ArcLiftHandle());
		if(pLift)
		{
			CXhChar16 sPosition=""; 
			f3dPoint pos=pNode->Position();
			if(fabs(pos.x)>fabs(pos.y)) //����
			{
				sPosition="��";
				if(pos.x>0)
					sPosition="��";
			}
			else	//ǰ��
			{
				sPosition="ǰ";
				if(pos.y<0)
					sPosition="��";
			}
			if(hahsArcLiftBySegIAndLiftName.GetValue(pNode->iSeg,pLift->handle))
				continue;
			hahsArcLiftBySegIAndLiftName.SetValue(pNode->iSeg,pLift->handle,true);
			logerr.Log("%s%s�Σ�Ԥ���߶�=%.1f",(char*)sPosition,(char*)pNode->iSeg.ToString(),pLift->m_fLiftHeight);
		}
	}
}
//�Ǹ���˨�߾���
CXhChar200 JGBOLTSPACE_CHKINFO::GetErrorMessage()
{
	CXhChar200 message;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(hJg,CLS_LINEANGLE);
	message.Printf("�Ǹ�%s{#%s}��W=%g ",(char*)CXhChar16("0X%X",hJg),(char*)pAngle->Label,pAngle->GetWidth());
	CXhChar16 realSpaceStr("%.1f",realEndSpace);
	SimplifiedNumString(realSpaceStr);
	realSpaceStr.ResizeLength(4);
	CXhChar16 lsStr("%s",bLsInStart?"ʼ����˨":"�ն���˨");
	if(ciCheckType==ROLLING_SPACE)
		message.Append(CXhChar100("��˨%s:[ʵ�����Ʊ߾�%s] %C [��׼���Ʊ߾�%d]",(char*)CXhChar16("0X%X",hBolt),(char*)realSpaceStr,realEndSpace>needEndSpace?'>':'<',needEndSpace));
	else if(ciCheckType==LS_END_SPACE)
		message.Append(CXhChar100("%s%s:[ʵ����˨�˾�%s] %C [��׼��˨�˾�%d]",(char*)lsStr,(char*)CXhChar16("0X%X",hBolt),(char*)realSpaceStr,realEndSpace>needEndSpace?'>':'<',needEndSpace));
	else if(ciCheckType==CUTANGLE_SPACE)
		message.Append(CXhChar100("%s%s:[ʵ���нǱ߾�%s] %C [��׼�нǱ߾�%d]",(char*)lsStr,(char*)CXhChar16("0X%X",hBolt),(char*)realSpaceStr,realEndSpace>needEndSpace?'>':'<',needEndSpace));
	return message;
}
void CheckJgBoltEndSpace(CXhSimpleList<JGBOLTSPACE_CHKINFO>* pListJgBoltSpaces)
{
	double epsilon=2;
	JGBOLTSPACE_CHKINFO  xCheckInfo;
	for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		//UCS_STRU ucs;
		//pAngle->getUCS(ucs);
		double fLen=pAngle->GetLength();// DISTANCE(pAngle->Start(),pAngle->End());
		for(CLsRef *pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace,pBolt->get_d());
			if( LsSpace.RollingEdgeSpace>0.5*pAngle->GetWidth())
				LsSpace.RollingEdgeSpace=ftoi(0.5*pAngle->GetWidth());
			//BOOL bInWingX=fabs(pAngle->vyWingNorm*pBolt->get_norm())<fabs(pAngle->vxWingNorm*pBolt->get_norm());
			//f3dPoint wing_norm=bInWingX?pAngle->vxWingNorm:pAngle->vyWingNorm;
			//f3dPoint pos;
			//Int3dlf(pos,pBolt->ucs.origin,pBolt->get_norm(),ucs.origin,wing_norm);
			//coord_trans(pos,ucs,FALSE);
			GEPOINT pos;
			pAngle->GetBoltLocalPos(pBolt,pos,true);
			//�˾���
			//if((pos.z<-pAngle->startOdd()+LsSpace.EndSpace-epsilon &&!pAngle->bHuoQuStart)||
			//	(pos.z>fLen+pAngle->endOdd()-LsSpace.EndSpace+epsilon &&!pAngle->bHuoQuEnd))	
			if( (pos.x<LsSpace.EndSpace-epsilon &&!pAngle->bHuoQuStart)||
				(pos.x>fLen-LsSpace.EndSpace+epsilon &&!pAngle->bHuoQuEnd))	
			{	//�˾಻����
				xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::LS_END_SPACE;
				xCheckInfo.hJg=pAngle->handle;
				xCheckInfo.hBolt=pBolt->handle;
				xCheckInfo.needEndSpace=LsSpace.EndSpace;
				if(pos.x<LsSpace.EndSpace-epsilon)
					xCheckInfo.realEndSpace=pos.x;
				if(fLen-pos.x<LsSpace.EndSpace-epsilon)
					xCheckInfo.realEndSpace=fLen-pos.x;
				if(pos.x<LsSpace.EndSpace-epsilon &&!pAngle->bHuoQuStart)
					xCheckInfo.bLsInStart=TRUE;
				else
					xCheckInfo.bLsInStart=FALSE;
				pListJgBoltSpaces->AttachNode(xCheckInfo);
			}
			//���Ʊ߾���
			double g=fabs(pos.y);
			double wing_wide=pos.y<0?pAngle->GetWidth():pAngle->GetWidthWingY();
			if(g<LsSpace.RollingEdgeSpace-epsilon||g>wing_wide-LsSpace.RollingEdgeSpace+epsilon)
			{	//���Ʊ߾಻����
				xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::ROLLING_SPACE;
				xCheckInfo.hJg=pAngle->handle;
				xCheckInfo.hBolt=pBolt->handle;
				xCheckInfo.needEndSpace=LsSpace.RollingEdgeSpace;
				xCheckInfo.realEndSpace=min(wing_wide-g,g);
				pListJgBoltSpaces->AttachNode(xCheckInfo);
			}
			//�нǱ߾���(��ʱ��˨pos����Ϊ�ֲ�����X��Ϊ�Ǹ�ʼ->�����췽��YֵΪ�ľ�(X֫Ϊ��) wjh-2017.11.24
			if(pos.y<0)
			{	//��˨λ��X֫��
				if(pAngle->cut_angle[0][0]>0 && pAngle->cut_angle[0][1]>0)
				{	//ʼ��X֫�н�
					f3dPoint ptS(0,-(wing_wide-pAngle->cut_angle[0][1]));
					f3dPoint ptE(pAngle->cut_angle[0][0],-wing_wide);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);//DistOf3dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=TRUE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_angle[2][0]>0 && pAngle->cut_angle[2][1]>0)
				{	//�ն�X֫�н�
					f3dPoint ptS(fLen-pAngle->cut_angle[2][0],-wing_wide);//wing_wide-pAngle->cut_angle[2][1],0,fLen+pAngle->endOdd());
					f3dPoint ptE(fLen,-(wing_wide-pAngle->cut_angle[2][1]));//,0,fLen+pAngle->endOdd()-pAngle->cut_angle[2][0]);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=FALSE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[0]==1)
				{	//ʼ��X֫��֫
					f3dPoint ptS(pAngle->cut_wing_para[0][0],0);
					f3dPoint ptE(pAngle->cut_wing_para[0][2],-wing_wide);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=TRUE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[0]==2&&pAngle->cut_wing_para[0][1]>epsilon)
				{	//ʼ��Y֫��֫(������֫)
					f3dPoint ptS(pAngle->cut_wing_para[0][0],0);
					f3dPoint ptE(0,-pAngle->cut_wing_para[0][1]);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=TRUE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[1]==1)
				{	//�ն�X֫��֫
					f3dPoint ptS(fLen-pAngle->cut_wing_para[1][2],-wing_wide);
					f3dPoint ptE(fLen-pAngle->cut_wing_para[1][0],0);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=FALSE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[1]==2&&pAngle->cut_wing_para[1][1]>epsilon)
				{	//�ն�Y֫��֫(������֫)
					f3dPoint ptS(fLen,-pAngle->cut_wing_para[1][1]);
					f3dPoint ptE(fLen-pAngle->cut_wing_para[1][0],0);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=FALSE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
			}
			else
			{
				if(pAngle->cut_angle[1][0]>0 && pAngle->cut_angle[1][1]>0)
				{	//ʼ��Y֫�н�
					f3dPoint ptS(pAngle->cut_angle[1][0],wing_wide);
					f3dPoint ptE(0,wing_wide-pAngle->cut_angle[1][1]);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=TRUE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_angle[3][0]>0 && pAngle->cut_angle[3][1]>0)
				{	//�ն�Y֫�н�
					f3dPoint ptS(fLen,wing_wide-pAngle->cut_angle[3][1]);
					f3dPoint ptE(fLen-pAngle->cut_angle[3][0],wing_wide);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=FALSE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[0]==1&&pAngle->cut_wing_para[0][1]>epsilon)
				{	//ʼ��X֫��֫(������֫)
					f3dPoint ptS(0,pAngle->cut_wing_para[0][1]);
					f3dPoint ptE(pAngle->cut_wing_para[0][0],0);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=TRUE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[0]==2)
				{	//ʼ��Y֫��֫
					f3dPoint ptS(pAngle->cut_wing_para[0][2],wing_wide);
					f3dPoint ptE(pAngle->cut_wing_para[0][0],0);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=TRUE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[1]==1&&pAngle->cut_wing_para[1][1]>epsilon)
				{	//�ն�X֫��֫(������֫)
					f3dPoint ptS(fLen-pAngle->cut_wing_para[1][0],0);
					f3dPoint ptE(fLen,pAngle->cut_wing_para[1][1]);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=FALSE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
				if(pAngle->cut_wing[1]==2)
				{	//�ն�Y֫��֫
					f3dPoint ptS(fLen-pAngle->cut_wing_para[1][0],0);
					f3dPoint ptE(fLen-pAngle->cut_wing_para[1][2],wing_wide);
					double fDist=DistOf2dPtLine(pos,ptS,ptE);
					if(fDist<LsSpace.CutAngleSpace-epsilon)
					{
						xCheckInfo.ciCheckType=JGBOLTSPACE_CHKINFO::CUTANGLE_SPACE;
						xCheckInfo.hJg=pAngle->handle;
						xCheckInfo.hBolt=pBolt->handle;
						xCheckInfo.needEndSpace=LsSpace.CutAngleSpace;
						xCheckInfo.realEndSpace=fDist;
						xCheckInfo.bLsInStart=FALSE;
						pListJgBoltSpaces->AttachNode(xCheckInfo);
					}
				}
			}
		}
	}
}
void ExportCheckJgBoltInfo(CXhSimpleList<JGBOLTSPACE_CHKINFO>* pListJgBoltSpaces)
{
	if(pListJgBoltSpaces->GetCount()>0)
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"*********�Ǹ���˨�߾ྯʾ��Ϣ*********");
	for(JGBOLTSPACE_CHKINFO* pBoltSpace=pListJgBoltSpaces->EnumObjectFirst();pBoltSpace;pBoltSpace=pListJgBoltSpaces->EnumObjectNext())
	{
		CXhChar200 sWarning=pBoltSpace->GetErrorMessage();
		if(sWarning.Length==0)
			continue;
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,(char*)sWarning);
	}
};
//У����Ŀ wjh-2017.1.16
/*
	1.����PART_DATAУ����Ŀ 
	2.������Ƴߴ�У����Ŀ
   V3.��������˨��׼�϶(OK)
	4.���⹤�տױ��漰�Գ���У��
	5.���߸˼����ӵ�Ԥ�������⽻�ӵ㱨��
   V6.���������϶��ƫ��׼�౨��
	7.���߿״�С�������ס��ӵؿ�(λ��)������ס����ƿױ����嵥���쳣���
   V8.Ԥ���߶ȱ���(���ؼ����ƽڵ�ĵ��߼�ʵ��Ԥ���߶�)���쳣��ʾ
   V9.���߰弰���߰屨�漰��Բ������(OK)
	10.��ߴ���
   V11.��ϽǸֱ�ż���϶�Ŵ�ֵ(OK)
   V12.�нǾ�ʾ��Ϣ(OK)
   V13.���ָ˼������仯����˨����δ�涯(OK)
   V14.�Ǹ��������嵥(�ҳ��ص�˼��嵥)
	15.���߰�ת�Ƕ���������������漰�ҿ׶���
   V16.�ְ�߾�(��ͨ�ְ�\���ְ�\���)�쳣��ʾ��Ϣ(OK)
	17.�Ȳ���ֵ
   *18.����㱨��
	19.����λ��(�˺��ƣ���λ�ƣ��������ơ���ʾ�ƣ���һ��λ�����Ⱥ�����ģ�2~3��˨��
   V20.�ԳƸ˼�����ͷ��һ�£�OK)
   V21.�Ǹ��ٿ�(������ٿף�(OK,��29������©���鲢��һ��)
	22.����ʩ����δ��
	23.����������ײ����
	24.����ѥ������
   V25.L63*5���ϽǸָ�ǿ���о�(OK)
   V26.�Ǹ�����������ʲ�һ�±���(OK)
	27.�Ǹ�����˾��⣨ֻ����ʼĩ��һ���������׼�⣩
   V28.��������©ȱʧ���(OK)
   V29.������©��⣨��ͷδ����������ƻ�ȱ�ٽ������˨��(OK)
*/
int CLDSView::RoutingWarningCheck()
{
	ROUTINGCHECK_FLAG flag;
	if(PRODUCT_FUNC::IsHasInternalTest())
	{
		flag.EnableAll();
		CLDSPart::INTERNAL_TEST_VERSION=true;
	}
	else if(CLDSPart::CustomerSerial==6)
	{
		flag.bExportSpecialAngleTeG=true;
		flag.bCheckMissingConnect=false;
	}
	CLogErrorLife life(&logerr);
	//3.��������˨��׼�϶
	if(flag.bExportExterHoleSpace)
		ExportExterHoleSpace();
	//*6.���������϶��ƫ��׼�౨��
	//ExportHuoQuSpaceAndOffsetG();
	//8.Ԥ���߶ȱ���(���ؼ����ƽڵ�ĵ��߼�ʵ��Ԥ���߶�)���쳣��ʾ
	if(flag.bExportArcLiftWarning)
		ExportArcLiftWarning();
	//9.���߰弰���߰屨�漰��Բ������
	if(flag.bExportAngleRibPlateAndGroovePlate)
		ExportAngleRibPlateAndGroovePlate();
	//10.��߼��
	//ExportRollEdgePart();
	//11.������ϽǸּ�϶����ֵ�������Ӱ��Ĳ���һֵ��
	if(flag.bGroupAngleSpace)
	{
		CXhSimpleList<GROUPANGLESPACE_CHKINFO> listGroupAngleChecks;
		CheckGroupAngleSpace(&listGroupAngleChecks);
		ExportCheckGroupAngleInfo(&listGroupAngleChecks);
	}
	//12.�нǾ�ʾ��Ϣ
	if(flag.bCutAngleInfo)
		CheckCutAngleInfoWarning();
	//13.���ָ˼������仯����˨����δ�涯(OK)
	if(flag.bCheckRodVariation)
		CheckRodVariation();
	//*14.�Ǹ��������嵥(�ҳ��ص�˼��嵥)
	//ExportAngleTeG();
	//16.��������͸ְ���˨�߾�
	if(flag.bCheckPlateEdgeSpace)
	{
		CXhSimpleList<PLATESPACE_CHKINFO> listPlateSpaces;
		CheckPlateEdgeSpace(&listPlateSpaces);
		ExportPlateEdgeInfo(&listPlateSpaces);
	}
	//17.�Ǹ�����˨�߾���
	if(flag.bCheckJgBoltEndSpace)
	{
		CXhSimpleList<JGBOLTSPACE_CHKINFO> listJgBoltSpaceChecks;
		CheckJgBoltEndSpace(&listJgBoltSpaceChecks);
		ExportCheckJgBoltInfo(&listJgBoltSpaceChecks);
	}
	//20.�ԳƸ˼�����ͷ��һ��
	if(flag.bCheckRodOddWarning)
		CheckRodOddWarning();
	//25.L63*5���ϽǸָ�ǿ���о�
	if(flag.bCheckSmallSizeHighMaterialAngle)
		CheckSmallSizeHighMaterialAngle(62);
	//26.�Ǹ�����������ʲ�һ�±���
	if(flag.bExportAngleAndPlateMatDiff)
		ExportAngleAndPlateMatDiff();
	//27.�Ǹ�����˾��⣨ֻ����ʼĩ��һ���������׼�⣩
	//28.��������©ȱʧ���
	if(flag.bCheckMissingLabelPart)
		CheckMissingLabelPart();
	//29.������©��⣨��ͷδ����������ƻ�ȱ�ٽ������˨��
	if(flag.bCheckMissingConnect||flag.bExportSpecialAngleTeG)
		CheckMissingConnect(flag.bCheckMissingConnect,flag.bExportSpecialAngleTeG);
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
	return 1;
}
void CLDSView::OnMechanicsCheck()
{
	MechanicsCheck();
}
#ifdef __PART_DESIGN_INC_
#include "MechanicsPlateCheckDlg.h"
double CallBackDialogUI(CLDSLineAngle* pAngle,double dfAxisNByBolts/*=0*/,DWORD *pdwCheckFlag/*=NULL*/)
{
	static CMechanicsPlateCheckDlg dlg;
	dlg.operangle.pOperAngle=pAngle;
	dlg.operangle.dfAxisNByBolts=dfAxisNByBolts;
	dlg.DoModal();
	if(pdwCheckFlag)
	{ 
		DWORD dwCheckFlag=0;
		if (dlg.m_bConnStrengthCheck)
			dwCheckFlag|=0x01;
		if (dlg.m_bBoltBlockShearCheck)
			dwCheckFlag|=0x02;
		if (dlg.m_bEdgeStableCheck)
			dwCheckFlag|=0x04;
		*pdwCheckFlag=dwCheckFlag;
	}
	return dlg.m_fRodAxisForce;
}
#endif
int CLDSView::MechanicsCheck()
{
#ifdef __PART_DESIGN_INC_
#ifdef __ANGLE_PART_INC_
	m_nPrevCommandID=ID_MECHANICS_CHECK;
#ifdef AFX_TARG_ENU_ENGLISH
    m_sPrevCommandName="Repeat the plates mechanics check";
#else
	m_sPrevCommandName="�ظ���˸ְ�ǿ��";
#endif
	CLogErrorLife loglife;
	CLDSPlate* pPlate;
	CLDSPlate* pool[128]={ NULL };
	ARRAY_LIST<CLDSPlate*> xarrPlates(pool,128,128);

	long *id_arr=NULL;
	long i,nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
	for (i=0;i<nn;i++)
	{
		if (pPlate=(CLDSPlate*)Ta.FromPartHandle(id_arr[i],CLS_PLATE))
		{
			xarrPlates.append(pPlate);
			if (nn>1)
				pPlate->StableCheck(&logerr,0xffff,NULL);
			else
				pPlate->StableCheck(&logerr,0xffff,CallBackDialogUI);
		}
	}
#endif
#endif
	return 0;
}
void CLDSView::OnOptimizePlate()
{
}
int CLDSView::OptimizePlate()
{
	return 0;
}
#endif

void Test123()
{
	CheckMissingConnect(true,true);
	//ExportHuoQuSpaceAndOffsetG();
	//ExportArcLiftWarning();
}