//<LOCALE_TRANSLATE BY hxr /> 2015-04-23
#include "stdafx.h"
#include "LogFile.h"
#include ".\Tower.h"
#include ".\spherepartgroup.h"
#include "SortFunc.h"

//////////////////////////////////////////////////////////////////////////
//
#ifdef __COMPLEX_PART_INC_
CSpherePartGroup::CSpherePartGroup(CLDSPartGroup* pPartGroup)
{
	m_pPartGroup=pPartGroup;
	m_pDatumTube=m_pViceTube=NULL;
	m_pSphere=NULL;
	datumcs_parainfo.m_cDefStyle=CParaCS::CS_PARAM;
	datumcs_parainfo.m_eDatumPlane=CParaCS::DP_YZ;
}

CSpherePartGroup::~CSpherePartGroup(void)
{
}
//<DEVELOP_PROCESS_MARK nameId="CSpherePartGroup::AddSpherePartGroupViews">
BOOL CSpherePartGroup::AddSpherePartGroupViews(CLDSSphere *pSphere,PARTSET &partset)
{
	if(m_pPartGroup==NULL||pSphere==NULL||pSphere->handle!=m_pPartGroup->handle)
		return FALSE;
	m_pSphere=pSphere;

	double verify_vicetube=-1;
	m_pPartGroup->EmptyDesignItems();	//��ո��Ӳ�����
	//1.ȷ����ת����Ļ�׼�ֹܼ���ת����ı�׼��ʸ������
	//1.1 ȷ����ת�����׼�ֹ�
	CSuperSmartPtr<CLDSPart>pRod;
	for(pRod=partset.GetFirst();pRod.IsHasPtr();pRod=partset.GetNext())
	{
		if(pRod->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		else if(pRod.LinePartPointer()->pStart==NULL||pRod.LinePartPointer()->pEnd==NULL||
			(pRod.LinePartPointer()->pStart->handle!=pSphere->hDatumNode&&
			pRod.LinePartPointer()->pEnd->handle!=pSphere->hDatumNode))
			continue;
		if(m_pDatumTube==NULL)
			m_pDatumTube=pRod.LineTubePointer();
		else if(m_pDatumTube->GetDiameter()<pRod.LineTubePointer()->GetDiameter())
			m_pDatumTube=pRod.LineTubePointer();
		else if(m_pDatumTube->GetDiameter()==pRod.LineTubePointer()->GetDiameter()&&
			m_pDatumTube->GetLength()<pRod.LineTubePointer()->GetLength())
			m_pDatumTube=pRod.LineTubePointer();
	}
	if(m_pDatumTube==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Don't find the datum tube of 0X%X weld sphere group node!",m_pPartGroup->handle);
#else
		logerr.Log("δ�ҵ�0X%X�麸��ڵ�Ļ�׼�ֹ�!",m_pPartGroup->handle);
#endif
		return FALSE;
	}
	//1.2 ȷ����ת����ı�׼��ʸ������
	datumcs_parainfo.desAxisY.norm_style=5;	//��׼�˼�����������
	datumcs_parainfo.desAxisY.hVicePart=m_pDatumTube->handle;
	if(m_pDatumTube->pStart->handle==pSphere->hDatumNode)
	{
		dcs.axis_y=m_pDatumTube->Start()-m_pDatumTube->End();
		datumcs_parainfo.desAxisY.direction=1;//����ʼ
	}
	else if(m_pDatumTube->pEnd->handle==pSphere->hDatumNode)
	{
		dcs.axis_y=m_pDatumTube->End()-m_pDatumTube->Start();
		datumcs_parainfo.desAxisY.direction=0;//ʼ����
	}
	if(!dcs.axis_y.IsZero())
		normalize(dcs.axis_y);	//��λ���˼����ȷ�����������߷���
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The extend direction of 0X%X weld sphere group node's datum tube 0X%X is null!",m_pPartGroup->handle,m_pDatumTube->handle);
#else
		logerr.Log("0X%X�麸��ڵ�Ļ�׼�ֹ�0X%X���췽��Ϊ��!",m_pPartGroup->handle,m_pDatumTube->handle);
#endif
		return FALSE;
	}
	//2.����ת����Ϊ��׼�����๹��������ƽ�治ͬ�������
	ATOM_LIST<TUBE_WORKPLANE> workPlaneList;
	TUBE_WORKPLANE *pMainWorkPlane=InitWorkPlaneList(m_pDatumTube,NULL,partset,workPlaneList);
	if(workPlaneList.GetNodeNum()<0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("There is only one tube connecting with sphere node in 0X%X weld sphere group node!",m_pPartGroup->handle);
#else
		logerr.Log("0X%X�麸��ڵ���н���һ������ڵ����ӵĸֹ�!",m_pPartGroup->handle);
#endif
		return FALSE;
	}
	//3.����WrokPlan�����ͼ
	//3.1�������������ͼ�������麸������ϵ(�麸������ϵ��������������ϵһ��)
	CSectionView *pView=AddSectionViewByWorkPlane(pMainWorkPlane,partset,CXhChar16("%c",'A'));
	if(pView)
	{
		pView->m_iViewType=VIEW_MAIN;
		datumcs_parainfo=pView->des_cs.GetParamInfo();
		dcs=GECS(pView->ucs.origin,pView->ucs.axis_x,pView->ucs.axis_y,pView->ucs.axis_z);
	}
	//3.2�����ͨ��������ͼ
	int i=1;
	for(TUBE_WORKPLANE *pWorkPlane=workPlaneList.GetFirst();pWorkPlane;pWorkPlane=workPlaneList.GetNext())
	{
		if(pWorkPlane==pMainWorkPlane)
			continue;
		AddSectionViewByWorkPlane(pWorkPlane,partset,CXhChar16("%c",'A'+i));
		i++;
	}
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CSpherePartGroup::InitWorkPlaneList">
//����ָ��������ת�����������˼���ʼ���������б�,������������
TUBE_WORKPLANE *CSpherePartGroup::InitWorkPlaneList(CLDSLineTube *pDatumTube,CLDSLineTube *pViceTube,
												 PARTSET &partset,ATOM_LIST<TUBE_WORKPLANE> &workPlaneList)
{
	TUBE_WORKPLANE *pMainWorkPlane=NULL;
	//1.����ͼ�е����иֹܣ������ֹ�Ϊ��׼�����иֵֹܷ���ͬ�Ĺ�������
	//  ָ�������ֹ�ʱ��ֱ��ȷ������ͼ
	CSuperSmartPtr<CLDSPart>pRod;
	for(pRod=partset.GetFirst();pRod.IsHasPtr();pRod=partset.GetNext())
	{
		if(pRod->GetClassTypeId()!=CLS_LINETUBE||pRod.IsEqualPtr(pDatumTube))
			continue;
		else if(pRod.LinePartPointer()->pStart==NULL||pRod.LinePartPointer()->pEnd==NULL||
			(pRod.LinePartPointer()->pStart->handle!=m_pSphere->hDatumNode&&
			pRod.LinePartPointer()->pEnd->handle!=m_pSphere->hDatumNode))
			continue;
		CLDSLineTube *pRayTube=pRod.LineTubePointer();
		f3dPoint ray_vec;
		if(pRayTube->pStart->handle==m_pSphere->hDatumNode)
			ray_vec=pRayTube->End()-pRayTube->Start();
		else
			ray_vec=pRayTube->Start()-pRayTube->End();
		normalize(ray_vec);
		f3dPoint work_norm=ray_vec^dcs.axis_y;
		normalize(work_norm);
		double cosa=ray_vec*dcs.axis_y;
		TUBE_WORKPLANE *pWorkPlane=NULL;
		for(pWorkPlane=workPlaneList.GetFirst();pWorkPlane;pWorkPlane=workPlaneList.GetNext())
		{	
			if(fabs(pWorkPlane->m_xWorkNormPlane*work_norm)>EPS_COS)	//�Ѵ��ڴ˹����淨�߷���
				break;
		}
		if(pWorkPlane==NULL)	//�µĹ����淨�߷���
		{
			pWorkPlane=workPlaneList.append();
			pWorkPlane->m_pMainTube=pDatumTube;
			pWorkPlane->m_pViceTube=pRayTube;
			pWorkPlane->m_xViceTubeVec=ray_vec;
			pWorkPlane->m_xWorkNormPlane=work_norm;
			pWorkPlane->m_fViceVecCosa=cosa;
			pWorkPlane->m_hashTubePtrList.Add(pDatumTube->handle);
			if(pMainWorkPlane==NULL)
				pMainWorkPlane=pWorkPlane;
			else if(pWorkPlane->m_fViceVecCosa>pMainWorkPlane->m_fViceVecCosa)
				pMainWorkPlane=pWorkPlane;
		}
		else 
		{
			if(cosa>pWorkPlane->m_fViceVecCosa)
			{
				pWorkPlane->m_fViceVecCosa=cosa;
				pWorkPlane->m_pViceTube=pRayTube;
				pWorkPlane->m_xViceTubeVec=ray_vec;
			}
		}
		pWorkPlane->m_hashTubePtrList.Add(pRayTube->handle);
		if(pMainWorkPlane==NULL&&pViceTube==pRayTube)
		{	//ָ�������ֹܣ������ֹ����ڵ���ͼ��Ϊ����ͼ
			pMainWorkPlane=pWorkPlane;
			pWorkPlane->m_fViceVecCosa=cosa;
			pWorkPlane->m_pViceTube=pRayTube;
			pWorkPlane->m_xViceTubeVec=ray_vec;
		}
	}
	//2.�������еĹ������������ͼ,ȡ�����ֹ������ֹܽ��ƹ��ߵĸֹ�Ϊ����ͼ
	if(pMainWorkPlane==NULL)
	{	//
		for(TUBE_WORKPLANE *pWorkPlane=workPlaneList.GetFirst();pWorkPlane;pWorkPlane=workPlaneList.GetNext())
		{
			if(pMainWorkPlane==NULL)
				pMainWorkPlane=pWorkPlane;
			else if(pWorkPlane&&pWorkPlane->m_fViceVecCosa>pMainWorkPlane->m_fViceVecCosa)
				pMainWorkPlane=pWorkPlane;
		}
	}
	//3.����������(Ϊ����ӹ��������谴������ͼ��׼�����ת�Ƕȴ�С��������)

	return pMainWorkPlane;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CSpherePartGroup::AddSectionViewByWorkPlane">
CSectionView *CSpherePartGroup::AddSectionViewByWorkPlane(TUBE_WORKPLANE *pWorkPlane,PARTSET &partSet,CXhChar16 sViewName)
{
	//CSectionView *pSectionView=m_pPartGroup->SonViews.append();
	CSectionView *pSectionView=m_pPartGroup->SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
	pSectionView->m_iViewType=VIEW_FRONT;	
	//1.ȷ������ϵ
	CParaCS::PARAMETRIC_INFO csParaInfo=datumcs_parainfo;
	csParaInfo.desOrg.datum_pos_style=8;	//�Խڵ�Ϊ��λ��׼��
	csParaInfo.desOrg.des_para.hNode=m_pSphere->hDatumNode;
	
	if(pWorkPlane->m_pViceTube==NULL || pWorkPlane->m_fViceVecCosa>EPS_COS)	//�����븨�ܹ���
	{
		csParaInfo.desAxisX.norm_style=5;		//�˼��ϵ���������
		csParaInfo.desAxisX.hVicePart=pWorkPlane->m_pMainTube->handle;
		if(pWorkPlane->m_pMainTube->m_bHasWeldRoad)
			csParaInfo.desAxisX.direction=3;	//�Ժ������Ƹֹ���תָ���ǶȺ�ķ���
		else
			csParaInfo.desAxisX.direction=2;	//�Ի�׼���Ƹֹ���תָ���ǶȺ�ķ���
		csParaInfo.desAxisX.rot_ang=0;
		csParaInfo.m_eDatumPlane=CParaCS::DP_YX;
	}
	else
	{
		csParaInfo.desAxisZ.norm_style=2;	//�����淨��
		csParaInfo.desAxisZ.hVicePart=pWorkPlane->m_pViceTube->handle;
		csParaInfo.desAxisZ.hCrossPart=pWorkPlane->m_pMainTube->handle;
		csParaInfo.desAxisZ.nearVector=pWorkPlane->m_xViceTubeVec^dcs.axis_y;	//��֤֧�������ϲ�
		csParaInfo.m_eDatumPlane=CParaCS::DP_YZ;
	}

	pSectionView->des_cs.SetParamInfo(csParaInfo);
	if(!pSectionView->des_cs.UpdateACS(m_pPartGroup->BelongModel()))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Fail to calculate datum plane's draw UCS of 0X%X weld sphere group node!",m_pPartGroup->handle);
#else
		logerr.Log("0X%X�麸��ڵ�Ļ�׼ƽ���ͼ����ϵ����ʧ��!",m_pPartGroup->handle);
#endif
		return FALSE;
	}
	else 
		pSectionView->ucs=pSectionView->des_cs.GetACS();
	//2.��ӹ���
	CHashTable<CLDSPart*> hashProcessedPart;
	hashProcessedPart.CreateHashTable(1000);
	int i=1;
	strcpy(pSectionView->sName,sViewName);
	pSectionView->AddPart(m_pSphere->handle);
	hashProcessedPart.SetValueAt(m_pSphere->handle,m_pSphere);
	//������ܶ�Ӧ����ͼ
	AddTubeEndSectionview(pSectionView,m_pDatumTube,CXhChar16("%s%d",(char*)sViewName,i++));
	//2.1 �������иֹܼ��ֹܶ�ͷ����
	for(CLDSPart *pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsTube())
			continue;
		CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
		if(pLineTube->pStart==NULL||pLineTube->pEnd==NULL)
			continue;
		//2.1.1 ������ڵ�ǰ������ĸֹ�
		BOOL bRoughCoplaner=FALSE;	//���ƹ���
		BOOL bIncTube=(pWorkPlane->m_hashTubePtrList.GetValue(pPart->handle)!=NULL);
		if(!bIncTube)
		{	//����ȫ����ֹ�ʱ�������ֹ��Ƿ��뵱ǰ��������ƹ���
			f3dPoint ray_tube_vec=pLineTube->pStart->Position()-pLineTube->pEnd->Position();
			normalize(ray_tube_vec);
			//cos70��=0.34202014332566873304409961468226
			double cos70=0.34202014332566873304409961468226;
			if(fabs(ray_tube_vec*pWorkPlane->m_xWorkNormPlane)<cos70)
				bRoughCoplaner=TRUE;	//���߸ֹ��뵱ǰ�����淨�߼нǴ���70��ʱΪ���ƹ���
		}
		if(bIncTube||bRoughCoplaner)
		{	
			CSectionView::PART_ITEM *pItem=pSectionView->AddPart(pLineTube->handle);
			if(bIncTube)
				pItem->cFlag='Z';
			else 
				pItem->cFlag='S';	//���ƹ���,���ƹ���Ĺ��������麸ͼʱ��Ҫ�����߻���
			hashProcessedPart.SetValueAt(pLineTube->handle,pLineTube);
			//2.1.2Ϊ���������ĸֹ���Ӷ�����ͼ,��ȫ����ĸֹܲ���Ҫ��Ӷ�����ͼ
			if( bIncTube&&pLineTube!=m_pDatumTube&&
				(pLineTube->pStart->handle==m_pSphere->hDatumNode||pLineTube->pEnd->handle==m_pSphere->hDatumNode))
				AddTubeEndSectionview(pSectionView,pLineTube,CXhChar16("%s%d",(char*)sViewName,i++));
		}
		//2.1.3 ��Ӹֹܶ�ͷ����
		TUBEJOINT *pTubeJoint=NULL;
		if(pLineTube->pStart->handle==m_pSphere->hDatumNode)
			pTubeJoint=&pLineTube->m_tJointEnd;
		else if(pLineTube->pEnd->handle==m_pSphere->hDatumNode)
			pTubeJoint=&pLineTube->m_tJointStart;
		else
			continue;
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pSphere->BelongModel()->FromPartHandle(pTubeJoint->hLinkObj,CLS_PARAMPLATE);
		if(pParamPlate==NULL)
			continue;
		hashProcessedPart.SetValueAt(pParamPlate->handle,pParamPlate);
		if(bIncTube||bRoughCoplaner)
		{
			CSectionView::PART_ITEM *pItem=pSectionView->AddPart(pParamPlate->handle);
			if(bIncTube)
				pItem->cFlag='Z';
			else
				pItem->cFlag='S';	//���ƹ���,���ƹ���Ĺ��������麸ͼʱ��Ҫ�����߻���
		}
		for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			CLDSPart *pSonPart=m_pSphere->BelongModel()->FromPartHandle(pItem->h);
			if(pSonPart==NULL||!pSonPart->IsPlate())
				continue;
			hashProcessedPart.SetValueAt(pSonPart->handle,pSonPart);
			if(bIncTube)
			{	//��ȫ����ĸֹܲ���Ҫ��ӷ����߰�ȸ��ӹ���
				double cosa=fabs(pSonPart->ucs.axis_z*pSectionView->ucs.axis_z);
				if(cosa>EPS_COS||cosa<EPS)
					pSectionView->AddPart(pSonPart->handle);
			}
		}
	}
	//2.���α���������ֹ��޹صĸְ�
	for(CLDSPart *pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(hashProcessedPart.ContainsKey(pPart->handle)||!pPart->IsPlate())
			continue;
		double cosa=fabs(pPart->ucs.axis_z*pSectionView->ucs.axis_z);
		if(cosa<EPS_COS2&&cosa>EPS)
			continue;	//������뵱ǰ��ͼƽ�л�ֱ�ĸְ�
		CSectionView::PART_ITEM *pItem=pSectionView->AddPart(pPart->handle,pPart);
		if(cosa>EPS_COS)
			pItem->cFlag='Z';
		else 
			pItem->cFlag='S';	//���ƹ���,���ƹ���Ĺ��������麸ͼʱ��Ҫ�����߻���
	}
	return pSectionView;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CSpherePartGroup::AddTubeEndSectionview">
CSectionView *CSpherePartGroup::AddTubeEndSectionview(CSectionView *pMainView,CLDSLineTube *pLineTube,CXhChar16 sViewName)
{
	if(pLineTube==NULL||pLineTube->pStart==NULL||pLineTube->pEnd==NULL||
		(pLineTube->pStart->handle!=m_pSphere->hDatumNode&&pLineTube->pEnd->handle!=m_pSphere->hDatumNode))
		return NULL;
	TUBEJOINT *pTubeJoint=NULL;
	if(pLineTube->pStart->handle==m_pSphere->hDatumNode)
		pTubeJoint=&pLineTube->m_tJointEnd;
	else //if(pLineTube->pEnd->handle==m_pSphere->hDatumNode)
		pTubeJoint=&pLineTube->m_tJointStart;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pSphere->BelongModel()->FromPartHandle(pTubeJoint->hLinkObj,CLS_PARAMPLATE);
	if(pParamPlate==NULL)
		return NULL;
	//1.�����ͼ
	//CSectionView *pSectionView=m_pPartGroup->SonViews.append();
	CSectionView *pSectionView=m_pPartGroup->SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
	pSectionView->m_iViewType=VIEW_FL;		//������ͼ
	strcpy(pSectionView->sName,sViewName);
	//2.ȷ������ϵ
	CParaCS::PARAMETRIC_INFO csParaInfo;
	csParaInfo.m_cDefStyle=0;
	csParaInfo.m_eDatumPlane=CParaCS::DP_ZY;
	//2.1 ����ϵԭ��
	csParaInfo.desOrg.datum_pos_style=1;		//�˼��˽ڵ�
	csParaInfo.desOrg.des_para.RODEND.hRod=pLineTube->handle;
	if(pLineTube->pStart->handle==m_pSphere->hDatumNode)
		csParaInfo.desOrg.des_para.RODEND.direction=1;	//�ն�
	else //if(pLineTube->pEnd->handle==m_pSphere->hDatumNode)
		csParaInfo.desOrg.des_para.RODEND.direction=0;	//ʼ��
	//2.2 Z��
	csParaInfo.desAxisZ.norm_style=5;			//�˼��ϵ���������
	csParaInfo.desAxisZ.hVicePart=pLineTube->handle;
	if(pLineTube->pStart->handle==m_pSphere->hDatumNode)
		csParaInfo.desAxisZ.direction=1;		//��->ʼ
	else //if(pLineTube->pEnd->handle==m_pSphere->hDatumNode)
		csParaInfo.desAxisZ.direction=0;		//ʼ->��
	//2.3 Y��
	CParaCS::PARAMETRIC_INFO mainViewCsParaInfo=pMainView->des_cs.GetParamInfo();
	if(mainViewCsParaInfo.m_cDefStyle==0&&
	   (mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_XZ||
		mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_YZ||
		mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_ZX||
		mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_ZY))
	{
		csParaInfo.desAxisY=pMainView->des_cs.GetParamInfo().desAxisZ;
		if(csParaInfo.desAxisY.norm_style==2)	//�����淽��
			csParaInfo.desAxisY.nearVector*=-1;	//��֤������ͼX�᷽��������ͼX�᷽����ͬһ��
	}
	else 
	{
		csParaInfo.desAxisY.norm_style=4;		//��׼�����ϵ���Է���
		csParaInfo.desAxisY.hVicePart=m_pDatumTube->handle;
		csParaInfo.desAxisY.nearVector=pMainView->ucs.axis_z*-1;
		vector_trans(csParaInfo.desAxisY.nearVector,m_pDatumTube->ucs,FALSE);
	}

	pSectionView->des_cs.SetParamInfo(csParaInfo);
	pSectionView->des_cs.UpdateACS(m_pSphere->BelongModel());
	pSectionView->ucs=pSectionView->des_cs.GetACS();
	//2.�����ͼ����
	pSectionView->AddPart(pLineTube->handle);
	pSectionView->AddPart(pParamPlate->handle);
	for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
	{
		CLDSPart *pSonPart=m_pSphere->BelongModel()->FromPartHandle(pItem->h);
		if(pSonPart==NULL||!pSonPart->IsPlate())
			continue;
		double cosa=fabs(pSonPart->ucs.axis_z*pSectionView->ucs.axis_z);
		if(cosa>EPS_COS2||cosa<EPS2)
			pSectionView->AddPart(pSonPart->handle);
	}
	return pSectionView;
}
//</DEVELOP_PROCESS_MARK>
#endif