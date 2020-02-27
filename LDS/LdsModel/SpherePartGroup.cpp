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
	m_pPartGroup->EmptyDesignItems();	//清空附加参数表
	//1.确定旋转主轴的基准钢管及旋转主轴的标准化矢量方向
	//1.1 确定旋转主轴基准钢管
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
		logerr.Log("未找到0X%X组焊球节点的基准钢管!",m_pPartGroup->handle);
#endif
		return FALSE;
	}
	//1.2 确定旋转主轴的标准化矢量方向
	datumcs_parainfo.desAxisY.norm_style=5;	//基准杆件上特征方向
	datumcs_parainfo.desAxisY.hVicePart=m_pDatumTube->handle;
	if(m_pDatumTube->pStart->handle==pSphere->hDatumNode)
	{
		dcs.axis_y=m_pDatumTube->Start()-m_pDatumTube->End();
		datumcs_parainfo.desAxisY.direction=1;//终至始
	}
	else if(m_pDatumTube->pEnd->handle==pSphere->hDatumNode)
	{
		dcs.axis_y=m_pDatumTube->End()-m_pDatumTube->Start();
		datumcs_parainfo.desAxisY.direction=0;//始至终
	}
	if(!dcs.axis_y.IsZero())
		normalize(dcs.axis_y);	//单位化杆件长度方向的延伸轴线方向
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The extend direction of 0X%X weld sphere group node's datum tube 0X%X is null!",m_pPartGroup->handle,m_pDatumTube->handle);
#else
		logerr.Log("0X%X组焊球节点的基准钢管0X%X延伸方向为空!",m_pPartGroup->handle,m_pDatumTube->handle);
#endif
		return FALSE;
	}
	//2.以旋转主轴为基准将其余构件按所在平面不同分类汇总
	ATOM_LIST<TUBE_WORKPLANE> workPlaneList;
	TUBE_WORKPLANE *pMainWorkPlane=InitWorkPlaneList(m_pDatumTube,NULL,partset,workPlaneList);
	if(workPlaneList.GetNodeNum()<0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("There is only one tube connecting with sphere node in 0X%X weld sphere group node!",m_pPartGroup->handle);
#else
		logerr.Log("0X%X组焊球节点的中仅有一根与球节点连接的钢管!",m_pPartGroup->handle);
#endif
		return FALSE;
	}
	//3.遍历WrokPlan添加视图
	//3.1添加主工作面视图，更新组焊件坐标系(组焊件坐标系与主工作面坐标系一致)
	CSectionView *pView=AddSectionViewByWorkPlane(pMainWorkPlane,partset,CXhChar16("%c",'A'));
	if(pView)
	{
		pView->m_iViewType=VIEW_MAIN;
		datumcs_parainfo=pView->des_cs.GetParamInfo();
		dcs=GECS(pView->ucs.origin,pView->ucs.axis_x,pView->ucs.axis_y,pView->ucs.axis_z);
	}
	//3.2添加普通工作面视图
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
//根据指定的主旋转构件及辅助杆件初始化工作面列表,返回主工作面
TUBE_WORKPLANE *CSpherePartGroup::InitWorkPlaneList(CLDSLineTube *pDatumTube,CLDSLineTube *pViceTube,
												 PARTSET &partset,ATOM_LIST<TUBE_WORKPLANE> &workPlaneList)
{
	TUBE_WORKPLANE *pMainWorkPlane=NULL;
	//1.将视图中的所有钢管，以主钢管为基准将所有钢管分到不同的工作面中
	//  指定辅助钢管时可直接确定主视图
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
			if(fabs(pWorkPlane->m_xWorkNormPlane*work_norm)>EPS_COS)	//已存在此工作面法线方向
				break;
		}
		if(pWorkPlane==NULL)	//新的工作面法线方向
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
		{	//指定辅助钢管，辅助钢管所在的视图即为主视图
			pMainWorkPlane=pWorkPlane;
			pWorkPlane->m_fViceVecCosa=cosa;
			pWorkPlane->m_pViceTube=pRayTube;
			pWorkPlane->m_xViceTubeVec=ray_vec;
		}
	}
	//2.遍历所有的工作面查找主视图,取辅助钢管与主钢管近似共线的钢管为主视图
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
	//3.工作面排序(为方便加工工作面需按到主视图基准面的旋转角度从小到大排序)

	return pMainWorkPlane;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CSpherePartGroup::AddSectionViewByWorkPlane">
CSectionView *CSpherePartGroup::AddSectionViewByWorkPlane(TUBE_WORKPLANE *pWorkPlane,PARTSET &partSet,CXhChar16 sViewName)
{
	//CSectionView *pSectionView=m_pPartGroup->SonViews.append();
	CSectionView *pSectionView=m_pPartGroup->SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
	pSectionView->m_iViewType=VIEW_FRONT;	
	//1.确定坐标系
	CParaCS::PARAMETRIC_INFO csParaInfo=datumcs_parainfo;
	csParaInfo.desOrg.datum_pos_style=8;	//以节点为定位基准点
	csParaInfo.desOrg.des_para.hNode=m_pSphere->hDatumNode;
	
	if(pWorkPlane->m_pViceTube==NULL || pWorkPlane->m_fViceVecCosa>EPS_COS)	//主管与辅管共线
	{
		csParaInfo.desAxisX.norm_style=5;		//杆件上的特征方向
		csParaInfo.desAxisX.hVicePart=pWorkPlane->m_pMainTube->handle;
		if(pWorkPlane->m_pMainTube->m_bHasWeldRoad)
			csParaInfo.desAxisX.direction=3;	//自焊道线绕钢管旋转指定角度后的方向
		else
			csParaInfo.desAxisX.direction=2;	//自基准边绕钢管旋转指定角度后的方向
		csParaInfo.desAxisX.rot_ang=0;
		csParaInfo.m_eDatumPlane=CParaCS::DP_YX;
	}
	else
	{
		csParaInfo.desAxisZ.norm_style=2;	//交叉面法线
		csParaInfo.desAxisZ.hVicePart=pWorkPlane->m_pViceTube->handle;
		csParaInfo.desAxisZ.hCrossPart=pWorkPlane->m_pMainTube->handle;
		csParaInfo.desAxisZ.nearVector=pWorkPlane->m_xViceTubeVec^dcs.axis_y;	//保证支管在右上侧
		csParaInfo.m_eDatumPlane=CParaCS::DP_YZ;
	}

	pSectionView->des_cs.SetParamInfo(csParaInfo);
	if(!pSectionView->des_cs.UpdateACS(m_pPartGroup->BelongModel()))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Fail to calculate datum plane's draw UCS of 0X%X weld sphere group node!",m_pPartGroup->handle);
#else
		logerr.Log("0X%X组焊球节点的基准平面绘图坐标系计算失败!",m_pPartGroup->handle);
#endif
		return FALSE;
	}
	else 
		pSectionView->ucs=pSectionView->des_cs.GetACS();
	//2.添加构件
	CHashTable<CLDSPart*> hashProcessedPart;
	hashProcessedPart.CreateHashTable(1000);
	int i=1;
	strcpy(pSectionView->sName,sViewName);
	pSectionView->AddPart(m_pSphere->handle);
	hashProcessedPart.SetValueAt(m_pSphere->handle,m_pSphere);
	//添加主管对应端面图
	AddTubeEndSectionview(pSectionView,m_pDatumTube,CXhChar16("%s%d",(char*)sViewName,i++));
	//2.1 遍历所有钢管及钢管端头构件
	for(CLDSPart *pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsTube())
			continue;
		CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
		if(pLineTube->pStart==NULL||pLineTube->pEnd==NULL)
			continue;
		//2.1.1 添加属于当前工作面的钢管
		BOOL bRoughCoplaner=FALSE;	//近似共面
		BOOL bIncTube=(pWorkPlane->m_hashTubePtrList.GetValue(pPart->handle)!=NULL);
		if(!bIncTube)
		{	//非完全共面钢管时，需计算钢管是否与当前工作面近似共面
			f3dPoint ray_tube_vec=pLineTube->pStart->Position()-pLineTube->pEnd->Position();
			normalize(ray_tube_vec);
			//cos70°=0.34202014332566873304409961468226
			double cos70=0.34202014332566873304409961468226;
			if(fabs(ray_tube_vec*pWorkPlane->m_xWorkNormPlane)<cos70)
				bRoughCoplaner=TRUE;	//射线钢管与当前工作面法线夹角大于70°时为近似共面
		}
		if(bIncTube||bRoughCoplaner)
		{	
			CSectionView::PART_ITEM *pItem=pSectionView->AddPart(pLineTube->handle);
			if(bIncTube)
				pItem->cFlag='Z';
			else 
				pItem->cFlag='S';	//近似共面,近似共面的构件生成组焊图时需要用虚线绘制
			hashProcessedPart.SetValueAt(pLineTube->handle,pLineTube);
			//2.1.2为与球相连的钢管添加端面视图,完全共面的钢管才需要添加端面视图
			if( bIncTube&&pLineTube!=m_pDatumTube&&
				(pLineTube->pStart->handle==m_pSphere->hDatumNode||pLineTube->pEnd->handle==m_pSphere->hDatumNode))
				AddTubeEndSectionview(pSectionView,pLineTube,CXhChar16("%s%d",(char*)sViewName,i++));
		}
		//2.1.3 添加钢管端头构件
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
				pItem->cFlag='S';	//近似共面,近似共面的构件生成组焊图时需要用虚线绘制
		}
		for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			CLDSPart *pSonPart=m_pSphere->BelongModel()->FromPartHandle(pItem->h);
			if(pSonPart==NULL||!pSonPart->IsPlate())
				continue;
			hashProcessedPart.SetValueAt(pSonPart->handle,pSonPart);
			if(bIncTube)
			{	//完全共面的钢管才需要添加法兰肋板等附加构件
				double cosa=fabs(pSonPart->ucs.axis_z*pSectionView->ucs.axis_z);
				if(cosa>EPS_COS||cosa<EPS)
					pSectionView->AddPart(pSonPart->handle);
			}
		}
	}
	//2.二次遍历处理与钢管无关的钢板
	for(CLDSPart *pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(hashProcessedPart.ContainsKey(pPart->handle)||!pPart->IsPlate())
			continue;
		double cosa=fabs(pPart->ucs.axis_z*pSectionView->ucs.axis_z);
		if(cosa<EPS_COS2&&cosa>EPS)
			continue;	//仅添加与当前视图平行或垂直的钢板
		CSectionView::PART_ITEM *pItem=pSectionView->AddPart(pPart->handle,pPart);
		if(cosa>EPS_COS)
			pItem->cFlag='Z';
		else 
			pItem->cFlag='S';	//近似共面,近似共面的构件生成组焊图时需要用虚线绘制
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
	//1.添加视图
	//CSectionView *pSectionView=m_pPartGroup->SonViews.append();
	CSectionView *pSectionView=m_pPartGroup->SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
	pSectionView->m_iViewType=VIEW_FL;		//法兰视图
	strcpy(pSectionView->sName,sViewName);
	//2.确定坐标系
	CParaCS::PARAMETRIC_INFO csParaInfo;
	csParaInfo.m_cDefStyle=0;
	csParaInfo.m_eDatumPlane=CParaCS::DP_ZY;
	//2.1 坐标系原点
	csParaInfo.desOrg.datum_pos_style=1;		//杆件端节点
	csParaInfo.desOrg.des_para.RODEND.hRod=pLineTube->handle;
	if(pLineTube->pStart->handle==m_pSphere->hDatumNode)
		csParaInfo.desOrg.des_para.RODEND.direction=1;	//终端
	else //if(pLineTube->pEnd->handle==m_pSphere->hDatumNode)
		csParaInfo.desOrg.des_para.RODEND.direction=0;	//始端
	//2.2 Z轴
	csParaInfo.desAxisZ.norm_style=5;			//杆件上的特征方向
	csParaInfo.desAxisZ.hVicePart=pLineTube->handle;
	if(pLineTube->pStart->handle==m_pSphere->hDatumNode)
		csParaInfo.desAxisZ.direction=1;		//终->始
	else //if(pLineTube->pEnd->handle==m_pSphere->hDatumNode)
		csParaInfo.desAxisZ.direction=0;		//始->终
	//2.3 Y轴
	CParaCS::PARAMETRIC_INFO mainViewCsParaInfo=pMainView->des_cs.GetParamInfo();
	if(mainViewCsParaInfo.m_cDefStyle==0&&
	   (mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_XZ||
		mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_YZ||
		mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_ZX||
		mainViewCsParaInfo.m_eDatumPlane==CParaCS::DP_ZY))
	{
		csParaInfo.desAxisY=pMainView->des_cs.GetParamInfo().desAxisZ;
		if(csParaInfo.desAxisY.norm_style==2)	//交叉面方向
			csParaInfo.desAxisY.nearVector*=-1;	//保证端面视图X轴方向与主视图X轴方向在同一侧
	}
	else 
	{
		csParaInfo.desAxisY.norm_style=4;		//基准构件上的相对方向
		csParaInfo.desAxisY.hVicePart=m_pDatumTube->handle;
		csParaInfo.desAxisY.nearVector=pMainView->ucs.axis_z*-1;
		vector_trans(csParaInfo.desAxisY.nearVector,m_pDatumTube->ucs,FALSE);
	}

	pSectionView->des_cs.SetParamInfo(csParaInfo);
	pSectionView->des_cs.UpdateACS(m_pSphere->BelongModel());
	pSectionView->ucs=pSectionView->des_cs.GetACS();
	//2.添加视图构件
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