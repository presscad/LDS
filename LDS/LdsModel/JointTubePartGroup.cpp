//<LOCALE_TRANSLATE BY hxr /> 2015-04-17
#include "stdafx.h"
#include "Tower.h"
#include "lds_part.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
IMPLEMENT_GET(CJointTubesView,m_iViewType){return VIEW_JOINTTUBE;}
CJointTubesView::CJointTubesView(int id) : CSectionView(id)
{
	_idViewType=CLS_JOINT_TUBES_VIEW;
	_iLegacyViewType=VIEW_JOINTTUBE;
	m_hRootTube=m_hJointTube=0;
	m_ciJointsViewType=0;
}
bool CJointTubesView::UpdateJointViewType()
{	//�Զ�����m_ciJointsViewType������ֵ wjh-2018.3.6
	if(m_pPartGroup==NULL)
		return false;
	UpdateUCS(m_pPartGroup->BelongModel(),m_pPartGroup);
	CLDSLineTube* pRootRod =(CLDSLineTube*)m_pPartGroup->BelongModel()->FromPartHandle(m_hRootTube,CLS_LINETUBE);
	CLDSLineTube* pJointRod=(CLDSLineTube*)m_pPartGroup->BelongModel()->FromPartHandle(m_hJointTube,CLS_LINETUBE);
	if(pRootRod&&pJointRod)
	{
		GEPOINT lenRootVec =pRootRod->End() - pRootRod->Start();
		if(pRootRod->pEnd==pJointRod->pStart||pRootRod->pEnd==pJointRod->pEnd)
			lenRootVec*=-1;	//�ն˶Խ�
		GEPOINT lenJointVec=pJointRod->End()-pJointRod->Start();
		if(pJointRod->pEnd==pRootRod->pStart||pJointRod->pEnd==pRootRod->pEnd)
			lenJointVec*=-1;	//�ն˶Խ�
		normalize(lenRootVec);
		normalize(lenJointVec);
		double jointRadAngle=ACOS(-(lenRootVec*lenJointVec));
		GEPOINT lenPrjRootVec=lenRootVec;
		GEPOINT lenPrjJointVec=lenJointVec;
		vector_trans(lenPrjRootVec,ucs,FALSE,TRUE);
		vector_trans(lenPrjJointVec,ucs,FALSE,TRUE);
		lenPrjRootVec.z=lenPrjJointVec.z=0;
		normalize(lenPrjRootVec);
		normalize(lenPrjJointVec);
		double jointPrjRadAngle=ACOS(-(lenPrjRootVec*lenPrjJointVec));
		if(jointPrjRadAngle<=0.6*jointRadAngle)
			m_ciJointsViewType=1;	//ͶӰ��Խӽ�С��ʵ�ʶԽӽ�һ�룬��ƽ��ͼֱ�ߴ���
		else if(fabs(jointPrjRadAngle-jointRadAngle)<0.000001)
			m_ciJointsViewType=0;	//չʾ�ԽӸֹܼнǵĸ���ͼ
		else
			m_ciJointsViewType=2;	//�������ĶԽ���������ͼ
		return true;
	}
	else
		return false;
}
void CJointTubesView::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(m_ciJointsViewType);
	buffer.WriteInteger(m_hRootTube);
	buffer.WriteInteger(m_hJointTube);
}
void CJointTubesView::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	CSectionView::FromBuffer(buffer,version,doc_type);
	//ʵ�ʴ�LMA V2.0.7��ʼ����CJointTubesView wjh-2016.11.11
	buffer.ReadByte(&m_ciJointsViewType);
	buffer.ReadInteger(&m_hRootTube);
	buffer.ReadInteger(&m_hJointTube);
}
//��ȡ�ֹܶ�����ͼ
extern CLDSPart* GetTubeJointPart(CLDSLineTube* pLineTube,BOOL bIntersect);
static CSectionView *AddTubeEndSectionview(CLDSPartGroup* pPartGroup,CSectionView *pMainView,CLDSLineTube *pLineTube,const char* strViewName16,int &view_index)
{
	CLDSPart* pPart=GetTubeJointPart(pLineTube,FALSE);
	if(pPart==NULL || pPart->GetClassTypeId()!=CLS_PARAMPLATE)
		return NULL;
	CLDSParamPlate* pParaPlate=(CLDSParamPlate*)pPart;
	if(!pParaPlate->IsFL())
		return NULL;
	pMainView->AddPart(pParaPlate->handle,pParaPlate);
	BOOL bStartIntersect=FALSE;
	if(pLineTube->m_tJointStart.type==1)
		bStartIntersect=TRUE;
	else if(pLineTube->m_tJointEnd.type==1)
		bStartIntersect=FALSE;
	CSlaveSectionView *pSectionView = (CSlaveSectionView*)pPartGroup->SonViews.Add(0,CSectionView::CLS_SLAVE_VIEW);
	pSectionView->m_bLocked=TRUE;
	pSectionView->m_iViewType=VIEW_FL;		//������ͼ
	pSectionView->index=view_index;
	StrCopy(pSectionView->sName,strViewName16,51);
	//��ʼ����ͼ����ϵ
	CParaCS::PARAMETRIC_INFO csParaInfo;
	csParaInfo.m_cDefStyle=0;
	csParaInfo.desOrg.datum_pos_style=9;
	csParaInfo.desOrg.des_para.hPart=pParaPlate->handle;
	csParaInfo.m_eDatumPlane=CParaCS::DP_ZX;
	if(pMainView&&pMainView->des_cs.IsDesAxisZ())
		csParaInfo.desAxisX=pMainView->des_cs.GetParamInfo().desAxisZ;
	else
	{
		csParaInfo.desAxisX.norm_style=4;
		csParaInfo.desAxisX.hVicePart=pLineTube->handle;
		csParaInfo.desAxisX.nearVector.Set(1,0,0);
	}
	csParaInfo.desAxisZ.norm_style=3;
	csParaInfo.desAxisZ.hVicePart=pParaPlate->handle;
	csParaInfo.desAxisZ.direction=0;
	pSectionView->des_cs.SetParamInfo(csParaInfo);
	pSectionView->UpdateUCS(pLineTube->BelongModel(),pPartGroup);
	//��ʼ����ͼ����
	pSectionView->AddPart(pLineTube->handle);
	pSectionView->AddPart(pParaPlate->handle);
	for(ATTACHPART_ITEM *pItem=pParaPlate->GetFirstAttachPart();pItem;pItem=pParaPlate->GetNextAttachPart())
	{
		CLDSPart *pSonPart=pLineTube->BelongModel()->FromPartHandle(pItem->h);
		if(pSonPart==NULL||!pSonPart->IsPlate())
			continue;
		double cosa=fabs(pSonPart->ucs.axis_z*pSectionView->ucs.axis_z);
		if(cosa>EPS_COS2||cosa<EPS2)
			pSectionView->AddPart(pSonPart->handle);
	}
	pSectionView->InitSlaveSectionView(pMainView,pLineTube->BelongModel());
	return pSectionView;
}
//��ȡ�Խ����ֹ�������ͼ	wxc--2014.6.27
extern void AdjustViewNorm(f3dPoint &axis_z,CLDSPart *pDatumPart);
BOOL CLDSPartGroup::AddJointTubeViews(CLDSLineTube *pRootTube,PARTSET &partset)
{
	if(pRootTube==NULL||pRootTube->handle!=handle)
		return FALSE;
	CLDSNode* pJointNode=NULL;
	BOOL bJoinStart=FALSE;
	if(pRootTube->m_tJointStart.type==1&&pRootTube->m_tJointStart.hViceLinkObj<0)
	{
		pJointNode=pRootTube->pStart;
		bJoinStart=TRUE;
	}
	else if(pRootTube->m_tJointEnd.type==1&&pRootTube->m_tJointEnd.hViceLinkObj<0)
	{
		pJointNode=pRootTube->pEnd;
		bJoinStart=FALSE;
	}
	else
		return FALSE;
	CLDSLineTube* pJointTube=(CLDSLineTube*)GetTubeJointPart(pRootTube,TRUE);
	if(pJointTube==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Didn't find joint tube of 0X%X jointing and intersecting group weld part!",handle);
#else
		logerr.Log("δ�ҵ�0X%X�Խ�����麸���ĶԽӸֹ�!",handle);
#endif
		return FALSE;
	}
	//1.��ӶԽ��������ͼ(��Ҫ���������ֹ��Լ�����),��Ҫ�����Խ����ֹ�֮��ĽǶ��Լ���Ƥ����
	SonViews.Empty();		//���������ͼ
	int view_index=1,fl_view_index=1;
	CSectionView* pSectionView=SonViews.Add(0,CSectionView::CLS_JOINT_TUBES_VIEW);
	pSectionView->index=0;//view_index++;
	CJointTubesView* pJointView=(CJointTubesView*)pSectionView;
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(pSectionView->sName,"Joint main view");
#else
	sprintf(pSectionView->sName,"�Խ�����ͼ");
#endif
	GEPOINT lenVec1=pRootTube->End()-pRootTube->Start();
	GEPOINT lenVec2=pJointTube->End()-pJointTube->Start();
	normalize(lenVec1);
	normalize(lenVec2);
	if( fabs(lenVec2.x)>EPS_COS2&&fabs(lenVec2.x)>fabs(lenVec1.x)||
		fabs(lenVec2.y)>EPS_COS2&&fabs(lenVec2.y)>fabs(lenVec1.y)||
		fabs(lenVec2.z)>EPS_COS2&&fabs(lenVec2.z)>fabs(lenVec1.z)||
		pJointTube->GetLength()>8*pJointTube->GetLength())
	{
		CLDSLineTube* pTube=pRootTube;
		pRootTube=pJointTube;
		pJointTube=pTube;
	}
	pJointView->m_hRootTube=pRootTube->handle;
	pJointView->m_hJointTube=pJointTube->handle;
	//��ʼ����ͼ����
	CParaCS::PARAMETRIC_INFO param;
	param.m_cDefStyle=0;
	param.desOrg.datum_pos_style=1;
	param.desOrg.des_para.RODEND.hRod=pRootTube->handle;
	param.desOrg.des_para.RODEND.bIncOddEffect=TRUE;
	if(bJoinStart)
		param.desOrg.des_para.RODEND.direction=1;
	else
		param.desOrg.des_para.RODEND.direction=0;
	param.m_eDatumPlane=CParaCS::DP_XY;
	param.desAxisX.norm_style=5;
	param.desAxisX.hVicePart=pRootTube->handle;
	if(bJoinStart)
		param.desAxisX.direction=1;
	else
		param.desAxisX.direction=0;
	param.desAxisY.norm_style=5;
	param.desAxisY.hVicePart=pJointTube->handle;
	if(pJointTube->m_tJointEnd.hLinkObj==pRootTube->handle)
		param.desAxisY.direction=1;	
	else
		param.desAxisY.direction=0;

	pSectionView->des_cs.SetParamInfo(param);
	pSectionView->UpdateUCS(m_pModel,this);
	if(fabs(pSectionView->ucs.axis_x.z)<0.1&&fabs(pSectionView->ucs.axis_y.z)>0.9)
	{	//��ֹ��繹�ܶ�������ͼ�ڳ��� wjh-2016.11.22
		param.desAxisY.UpdateVector(m_pModel);
		if(param.desAxisY.vector.z>0)
			param.desAxisY.ReverseDirection();
		pSectionView->des_cs.SetParamInfo(param);
		pSectionView->UpdateUCS(m_pModel,this);
	}
	//��ʼ��������ͼ����
	pSectionView->AddPart(pRootTube->handle,pRootTube);
	pSectionView->AddPart(pJointTube->handle,pJointTube);
#ifdef AFX_TARG_ENU_ENGLISH
	AddTubeEndSectionview(this,pSectionView,pRootTube,CXhChar16("Flange View%d",fl_view_index++),view_index);
	AddTubeEndSectionview(this,pSectionView,pJointTube,CXhChar16("Flange View%d",fl_view_index++),view_index);
#else
	AddTubeEndSectionview(this,pSectionView,pRootTube ,CXhChar16("������ͼ%d",fl_view_index++),view_index);
	AddTubeEndSectionview(this,pSectionView,pJointTube,CXhChar16("������ͼ%d",fl_view_index++),view_index);
#endif
	CLDSPart* pPart=NULL;
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		pSectionView->AddPart(pPart->handle,pPart);
	//2��ͳ����ԽӸֹ�����֧�ܼ���
	TUBESET lineTubeSet;
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		CLDSLineTube* pLinkTube=(CLDSLineTube*)pPart;
		if(pLinkTube==pRootTube||pLinkTube==pJointTube)
			continue;
		if((pLinkTube->m_tJointStart.type==1&&pLinkTube->m_tJointStart.hLinkObj>=0)||
			(pLinkTube->m_tJointEnd.type==1&&pLinkTube->m_tJointEnd.hLinkObj>=0))
			lineTubeSet.append(pLinkTube);
	}
	//3�������֧��ʱ�����֧����ԽӸֹܵ�װ����ͼ,��Ҫ����֧����ԽӸֹܵĽǶ�
	if(lineTubeSet.GetNodeNum()>0)
	{
		//ȷ����ת�����׼��ʸ��
		f3dPoint axis_x;
		param.desAxisX.norm_style=5;	//��׼�˼�����������
		param.desAxisX.hVicePart=pRootTube->handle;
		if(bJoinStart)
		{
			param.desAxisX.direction=1;
			axis_x=pRootTube->Start()-pRootTube->End();
		}
		else
		{	
			param.desAxisX.direction=0;
			axis_x=pRootTube->End()-pRootTube->Start();
		}
		normalize(axis_x);
		int index=1;
		for(CLDSLineTube* pRayTube=lineTubeSet.GetFirst();pRayTube;pRayTube=lineTubeSet.GetNext())
		{
			pSectionView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
			pSectionView->index=view_index++;
			pSectionView->m_iViewType=VIEW_FRONT;
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(pSectionView->sName,"Branch tube assemble view%d",index++);
#else
			sprintf(pSectionView->sName,"֧��װ��ͼ%d",index++);
#endif
			//��ʼ����ͼ����ϵ
			f3dPoint ray_vec,work_norm;
			ray_vec=pRayTube->Start()-pRayTube->End();
			normalize(ray_vec);
			work_norm=axis_x^ray_vec;
			normalize(work_norm);
			param.desAxisZ.norm_style=2;	//�����淨��
			param.desAxisZ.hVicePart=pRayTube->handle;
			param.desAxisZ.hCrossPart=pRootTube->handle;
			param.desAxisZ.nearVector=work_norm;
			param.m_eDatumPlane=CParaCS::DP_XZ;
			pSectionView->des_cs.SetParamInfo(param);
			pSectionView->UpdateUCS(m_pModel,this);
			//��ʼ����ͼ�˼�
			pSectionView->AddPart(pRootTube->handle,pRootTube);
			CLDSPart* pPart=GetTubeJointPart(pRootTube,FALSE);
			if(pPart)
				pSectionView->AddPart(pPart->handle);
			pSectionView->AddPart(pJointTube->handle,pJointTube);
			pPart=GetTubeJointPart(pJointTube,FALSE);
			if(pPart)
				pSectionView->AddPart(pPart->handle,pPart);
			pSectionView->AddPart(pRayTube->handle,pRayTube);
#ifdef AFX_TARG_ENU_ENGLISH
			AddTubeEndSectionview(this,pSectionView,pRayTube,CXhChar16("Flange View%d",fl_view_index++),view_index);
#else
			AddTubeEndSectionview(this,pSectionView,pRayTube,CXhChar16("������ͼ%d",fl_view_index++),view_index);
#endif
			//��ӹ���ֹܼ���������
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(!pPart->IsTube() || pPart->handle==pJointTube->handle || pPart->handle==pRayTube->handle)
					continue;
				CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
				if(pLineTube->pStart==NULL||pLineTube->pEnd==NULL)
					continue;
				//A ������ڵ�ǰ������ĸֹ�(���ֹܶ���ͼ)
				BOOL bIncTube=FALSE,bRoughCoplaner=FALSE;
				char cFlag='Z';
				f3dPoint ray_tube_vec=pLineTube->pStart->Position()-pLineTube->pEnd->Position();
				normalize(ray_tube_vec);
				//���߸ֹ��뵱ǰ�����淨�߼нǴ���70��ʱΪ���ƹ���
				double cos70=0.34202014332566873304409961468226;
				if(fabs(ray_tube_vec*work_norm)<EPS2)			//����
					bIncTube=TRUE;
				else if(fabs(ray_tube_vec*work_norm)<cos70)		//���ƹ���
					bRoughCoplaner=TRUE;	
				else											//����
					continue;
				if(bRoughCoplaner)		//���ƹ���,���ƹ���Ĺ��������麸ͼʱ��Ҫ�����߻���
					cFlag='S';	
				pSectionView->AddPart(pLineTube->handle,pLineTube,cFlag);
				//B ��Ӹֹܶ�ͷ����
				TUBEJOINT *pTubeJoint=NULL;
				if(pLineTube->m_tJointStart.type==1)			//ʼ�����
					pTubeJoint=&pLineTube->m_tJointEnd;
				else if(pLineTube->m_tJointEnd.type==1)		//�ն����
					pTubeJoint=&pLineTube->m_tJointStart;
				else
					continue;
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pModel->FromPartHandle(pTubeJoint->hLinkObj,CLS_PARAMPLATE);
				if(pParamPlate==NULL)
					continue;
				pSectionView->AddPart(pParamPlate->handle,pParamPlate,cFlag);
				for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
				{	//��ȫ����ĸֹܲ���Ҫ��ӷ����߰�ȸ��ӹ���
					CLDSPart *pSonPart=m_pModel->FromPartHandle(pItem->h);
					if(pSonPart==NULL||!pSonPart->IsPlate() || bRoughCoplaner)
						continue;
					double cosa=fabs(pSonPart->ucs.axis_z*pSectionView->ucs.axis_z);
					if(cosa>EPS_COS||cosa<EPS)
						pSectionView->AddPart(pSonPart->handle,pSonPart,cFlag);
				}
			}
			//���α���������ֹ��޹صĸְ�
			for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(pPart->GetClassTypeId()!=CLS_PLATE)
					continue;
				double cosa=fabs(pPart->ucs.axis_z*pSectionView->ucs.axis_z);
				if(cosa<EPS_COS2&&cosa>EPS2)
					continue;	//������뵱ǰ��ͼƽ�л�ֱ�ĸְ�
				pSectionView->AddPart(pPart->handle,pPart);
			}
		}
	}
	else	//4�������֧��ʱ���������ͼ,������Ҫ����λ��
	{
		long hOldRoot=handle;
		handle=pRootTube->handle;
		BOOL pushed =SonViews.push_stack();
		AddCommonRodPartGroupViews(pRootTube,partset);
		SonViews.pop_stack(pushed);

		CSectionView *pFrontView,*pSlaveView;
		CJointTubesView* pJointView2;
		CHashSet<CJointTubesView*>hashJointViews;
		hashJointViews.SetValue(pJointView->Id,pJointView);
		for(pFrontView=SonViews.GetNext();pFrontView;pFrontView=SonViews.GetNext())
		{
			if(pFrontView->m_iViewType!=VIEW_MAIN&&pFrontView->m_iViewType!=VIEW_FRONT)
				continue;
			if(fabs(pFrontView->ucs.axis_z*pJointView->ucs.axis_z)>0.866)	//0.866=cos(30��)
				continue;
			BOOL pushed2=SonViews.push_stack();
			pJointView2=(CJointTubesView*)SonViews.Add(0,CSectionView::CLS_JOINT_TUBES_VIEW);
			SonViews.pop_stack(pushed2);
			StrCopy(pJointView2->sName,pFrontView->sName,51);
			pJointView2->index=pFrontView->index;
			pJointView2->des_cs=pFrontView->des_cs;
			pJointView2->ucs=pFrontView->ucs;
			pJointView2->m_hRootTube=pJointView->m_hRootTube;
			pJointView2->m_hJointTube=pJointView->m_hJointTube;
			//pJointView2->m_ciJointsViewType=1;
			pJointView2->AddPart(pJointTube->handle,pJointTube);
			for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				pJointView2->AddPart(pPart->handle,pPart);
			hashJointViews.SetValue(pJointView2->Id,pJointView2);
		}

		handle=pJointTube->handle;
		AddCommonRodPartGroupViews(pJointTube,partset);
		handle=hOldRoot;

		CXhPtrSet<CSectionView> listFrontViews;	//��������ͼ(VIEW_MAIN��VIEW_FRONT������ͼ)
		for(pFrontView=SonViews.GetFirst();pFrontView;pFrontView=SonViews.GetNext())
		{
			if(pFrontView->m_iViewType==VIEW_NODEPLATE)
			{
				CSectionView::PART_ITEM* pItem=pFrontView->SonParts.GetFirst();
				while(pItem)
				{
					CLDSPart* pItemPart=pRootTube->BelongModel()->FromPartHandle(pItem->hSonPart);
					if(pItemPart&&pItemPart->IsPlate()&&((CLDSGeneralPlate*)pItemPart)->GetBaseNode()==pJointNode)
					{
						for(pItemPart=partset.GetFirst();pItemPart;pItemPart=partset.GetNext())
						{
							if(pItemPart->IsPlate()&&((CLDSGeneralPlate*)pItemPart)->GetBaseNode()==pJointNode)
								pFrontView->AddPart(pItemPart->handle,pItemPart);
						}
						break;
					}
					pItem=pFrontView->SonParts.GetNext();
				}
			}
			if(pFrontView==pJointView)
				continue;	//�����Խ�����ͼ
			if(pFrontView->m_iViewType==VIEW_MAIN||pFrontView->m_iViewType==VIEW_FRONT)
			{
				pFrontView->AddPart(pRootTube->handle,pRootTube);	//�������һЩ����ͼȱ�ٶԽӸֹܾ��
				pFrontView->AddPart(pJointTube->handle,pJointTube);
				listFrontViews.append(pFrontView);
			}
		}
		for(pFrontView=listFrontViews.GetFirst();pFrontView;pFrontView=listFrontViews.GetNext())
		{
			for(pJointView2=hashJointViews.GetFirst();pJointView2;pJointView2=hashJointViews.GetNext())
			{
				if(fabs(pFrontView->ucs.axis_z*pJointView2->ucs.axis_z)>0.866)	//0.9999985=cos(0.1��)
				{
					for(pSlaveView=SonViews.GetFirst();pSlaveView;pSlaveView=SonViews.GetNext())
					{
						if(pSlaveView->IsSlaveView()&&((CSlaveSectionView*)pSlaveView)->m_idHostView==pFrontView->GetId())
						{
							((CSlaveSectionView*)pSlaveView)->m_idHostView=pJointView->GetId();
							coord_trans(((CSlaveSectionView*)pSlaveView)->location,pFrontView->ucs,TRUE,TRUE);	//������ͼ��ͼ����ϵ�µ�����ͼ����λ������
							coord_trans(((CSlaveSectionView*)pSlaveView)->location,pJointView2->ucs,FALSE,TRUE);
							vector_trans(((CSlaveSectionView*)pSlaveView)->direction,pFrontView->ucs,TRUE,TRUE);	//������ͼ��ͼ����ϵ�µ�����ͼ�����߷���
							vector_trans(((CSlaveSectionView*)pSlaveView)->direction,pJointView2->ucs,FALSE,TRUE);
						}
					}
					listFrontViews.DeleteCursor(FALSE);
					SonViews.DeleteNode(pFrontView->GetId());
					break;
				}
			}
			if(pJointView2)
				continue;	//��ǰ��ͼ�Ѻϲ������жԽ���ͼ��
			BOOL pushed=listFrontViews.push_stack();
			for(CSectionView* pNextFrontView=listFrontViews.GetNext();pNextFrontView;pNextFrontView=listFrontViews.GetNext())
			{
				if(fabs(pFrontView->ucs.axis_z*pNextFrontView->ucs.axis_z)<0.9999985)	//0.9999985=cos(0.1��)
					continue;
				for(pSlaveView=SonViews.GetFirst();pSlaveView;pSlaveView=SonViews.GetNext())
				{
					if(pSlaveView->IsSlaveView()&&((CSlaveSectionView*)pSlaveView)->m_idHostView==pNextFrontView->GetId())
					{
						((CSlaveSectionView*)pSlaveView)->m_idHostView=pFrontView->GetId();
						coord_trans(((CSlaveSectionView*)pSlaveView)->location,pNextFrontView->ucs,TRUE,TRUE);	//������ͼ��ͼ����ϵ�µ�����ͼ����λ������
						coord_trans(((CSlaveSectionView*)pSlaveView)->location,pFrontView->ucs,FALSE,TRUE);
						vector_trans(((CSlaveSectionView*)pSlaveView)->direction,pNextFrontView->ucs,TRUE,TRUE);	//������ͼ��ͼ����ϵ�µ�����ͼ�����߷���
						vector_trans(((CSlaveSectionView*)pSlaveView)->direction,pFrontView->ucs,FALSE,TRUE);
					}
				}
				listFrontViews.DeleteCursor(FALSE);
				SonViews.DeleteNode(pNextFrontView->GetId());
				continue;
			}
			listFrontViews.pop_stack(pushed);
		}
	}
	//5����������ͼ
	//TODO:��ʱ���������ʱΪʲôҪר�ż����������ͼ
	/*pSectionView=SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
	pSectionView->index=view_index++;
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(pSectionView->sName,"Intersect section view");
#else
	strcpy(pSectionView->sName,"��������ͼ");
#endif
	pSectionView->m_iViewType=VIEW_JOINSECTION;
	//��ʼ����ͼ����ϵ
	param.m_eDatumPlane=CParaCS::DP_ZX;
	param.desAxisZ.norm_style=5;
	param.desAxisZ.hVicePart=pRootTube->handle;
	if(bJoinStart)
		param.desAxisZ.direction=4;	
	else
		param.desAxisZ.direction=5;
	param.desAxisX.norm_style=4;
	param.desAxisX.hVicePart=pRootTube->handle;
	param.desAxisX.nearVector.Set(1,0,0);
	pSectionView->des_cs.SetParamInfo(param);
	pSectionView->UpdateUCS(m_pModel,this);
	//��ʼ����ͼ����
	pSectionView->AddPart(pRootTube->handle,pRootTube);
	for(CLDSLineTube* pRayTube=lineTubeSet.GetFirst();pRayTube;pRayTube=lineTubeSet.GetNext())
		pSectionView->AddPart(pRayTube->handle,pRayTube);
	for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart==NULL ||pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		double cosa=fabs(pPart->ucs.axis_z*pSectionView->ucs.axis_z);
		if(cosa>EPS_COS2||cosa<EPS2)
			pSectionView->AddPart(pPart->handle,pPart);	
	}*/
	return TRUE;
}


