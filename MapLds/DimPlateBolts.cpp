#include "stdafx.h"
#include "stdarx.h"
#include "DimPlateBolts.h"
#include "Query.h"
#include "SysPara.h"
#include "DrawUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef __TSA_FILE_

//////////////////////////////////////////////////////////////////////////
//局部函数
typedef IDimPlateBolts::BOLTRAY*	BOLTRAY_PTR;
static int CompareBoltRayLinePtr(const BOLTRAY_PTR &rayLine1, const BOLTRAY_PTR &rayLine2)
{
	IDimPlateBolts::BOLT *pBolt1 = rayLine1->ls_arr.GetFirst();
	IDimPlateBolts::BOLT *pBolt2 = rayLine2->ls_arr.GetFirst();
	double posY1 = pBolt1 ? pBolt1->location.y : 0;
	double posY2 = pBolt2 ? pBolt2->location.y : 0;
	if (posY1 > posY2)
		return -1;
	else if (posY1 < posY2)
		return 1;
	else
		return 0;
}
static int compare_func1(const RAYROD_AXIS &pRay1, const RAYROD_AXIS &pRay2)
{
	if (pRay1.fAngle > pRay2.fAngle)
		return 1;
	else if (pRay1.fAngle < pRay2.fAngle)
		return -1;
	else
		return 0;
	return 0;
}
static int compare_func2(const ANGULARDIM_STRU &pAngleDim1, const ANGULARDIM_STRU &pAngleDim2)
{
	if (pAngleDim1.angle > pAngleDim2.angle)
		return 1;
	else if (pAngleDim1.angle < pAngleDim2.angle)
		return -1;
	else
		return 0;
	return 0;
}
//////////////////////////////////////////////////////////////////////
//IDimPlateBolts
IDimPlateBolts::BOLTINFO_TAG* IDimPlateBolts::LoadTagInfo(IDimPlateBolts::BOLTRAY* pBoltRay)
{
	if(pBoltRay->pTag!=NULL)
		return (BOLTINFO_TAG*)pBoltRay->pTag;
	BOLTINFO_TAG* pTagInfo=AppendTagToList();
	pBoltRay->pTag=pTagInfo;
	pTagInfo->AttachObject(pBoltRay);
	return pTagInfo;
}
void IDimPlateBolts::BOLTRAY::AppendBoltLoca(BOLT& bolt)
{
	int n = ls_arr.GetSize();
	if (n < 2)
		ls_arr.append(bolt);
	else
	{
		f3dLine line;
		line.startPt = ls_arr[0].location;
		int j = 1;
		for (j = 1; j < n; j++)
		{
			line.endPt = ls_arr[j].location;
			int iRetCode = line.PtInLine(bolt.location, 0.5);
			if (iRetCode == -2)		//起点外侧
			{
				ls_arr.insert(bolt, j - 1);
				break;
			}
			else if (iRetCode == 2)	//位于两螺栓之间
			{
				ls_arr.insert(bolt, j);
				break;
			}
			line.startPt = line.endPt;
		}
		if (j == n && f3dLine(ls_arr[0].location,ls_arr[n - 1].location).PtInLine(bolt.location, EPS2) != 0)
			ls_arr.append(bolt);
	}
}
//////////////////////////////////////////////////////////////////////////
//CDimPlateBolts
CDimPlateBolts::CDimPlateBolts()
{
	m_pAdjustCs = NULL;
	m_pPlate = NULL;
	m_bDimRayLsSpace = TRUE;
	m_bDimLsRayAngle = FALSE;
	m_bDimSpaceToWeldEdge = FALSE;
	m_bDimSpaceToHuoQuLine = FALSE;
	m_bDimSpaceToEdgeLine = FALSE;
	m_bDimFeatureLsSpace = FALSE;
}

CDimPlateBolts::~CDimPlateBolts()
{
	//listTagInfo.Empty();
}
//加标螺栓端距(主要就是将一个与射线垂直的垂足)
void CDimPlateBolts::DimLsEndSpace()
{
	/*LSRAY_STRU ls_ray;
	f3dLine ray_line,edge_line,line;
	int i,j,k,nDimCount;
	if(pPlank==NULL)
		return;
	nDimCount = ls_ray_arr.GetSize();
	for(i=0;i<nDimCount;i++)
	{
		int n = ls_ray_arr[i].ls_arr.GetNodeNum();
		if(n<2)	//现只有一个螺栓
			continue;
		else
		{
			f3dPoint perp;
			int nn = pPlank->vertex_list.GetNodeNum();
			ray_line.startPt = ls_ray_arr[i].ls_arr[0];
			ray_line.endPt = ls_ray_arr[i].ls_arr[1];
			for(k=0;k<nn;k++)
			{
				edge_line.startPt = pPlank->vertex_list[k].vertex;
				edge_line.endPt = pPlank->vertex_list[(k+1)%nn].vertex;
				if(!IsPerpToLine(ray_line,edge_line,perp,0.01))
					continue;	//此边界线不与螺栓射线垂直，略过
				perp.feature = -1;	//表示边界交点不是螺栓点
				line.startPt = ls_ray_arr[i].ls_arr[0];
				for(j=1;j<n;j++)
				{
					line.endPt = ls_ray_arr[i].ls_arr[j];
					int iRetCode = line.PtInLine(perp,EPS2);
					if(iRetCode==-2)		//起点外侧
					{
						if(DISTANCE(line.startPt,perp)>80)
							break;
						else
						{
							ls_ray_arr[i].ls_arr.insert(perp,j-1);
							break;
						}
					}
					else if(iRetCode==2)	//位于两螺栓之间
					{
						//ls_ray_arr[i].ls_arr.insert(perp,j);
						break;
					}
					//line.startPt = line.endPt;
				}
				if(j==n&&DISTANCE(line.endPt,perp)<80)//&&line.PtInLine(ls_centre,EPS2)==-1)
					ls_ray_arr[i].ls_arr.append(perp);
			}
		}
	}*/
}
//分析螺栓射线
bool CDimPlateBolts::AnalyzeBoltsRayline(const double* location, int iRayNo, CLsRef *pLsRef, const double* resideRayVec/* = NULL*/)
{
	CXhChar16 sizespec("M%dX%.0f", pLsRef->pBolt->d, pLsRef->pBolt->L);
	BOLT bolt(location);
	if (m_pAdjustCs)
	{
		CLDSBolt* pBolt = pLsRef->GetLsPtr();
		m_pPlate->GetBoltIntersPos(pBolt, bolt.adjustPt);
		coord_trans(bolt.adjustPt, *m_pAdjustCs, FALSE, TRUE);
		bolt.adjustPt.z = 0;
	}
	return AnalyzeBoltsRayline(bolt, iRayNo, sizespec, resideRayVec);
}
bool CDimPlateBolts::AnalyzeBoltsRayline(BOLT& bolt ,int iRayNo,const char *guige, const double* resideRayVec)
{
	f3dLine line;
	BOLTRAY* pBoltRay=hashBoltRays.GetValue(iRayNo);
	if (pBoltRay)
	{
		pBoltRay->AppendBoltLoca(bolt);
		LoadTagInfo(pBoltRay)->N++;	//螺栓个数加1
	}
	else	//暂时还没有此射线上的螺栓，添加当前射线
	{
		pBoltRay=hashBoltRays.Add(iRayNo);
		if (resideRayVec != NULL)
			pBoltRay->rayResideVec = GEPOINT(resideRayVec);
		if (guige != NULL)
		{
			long d, L;
			restore_Ls_guige(guige, NULL, &d, &L);
			LoadTagInfo(pBoltRay)->d = d;
			LoadTagInfo(pBoltRay)->L = L;
			LoadTagInfo(pBoltRay)->N = 1;
		}
		pBoltRay->ls_arr.append(bolt);
	}
	return true;
}
void CDimPlateBolts::DimBoltsRayline()
{	//调整钢板坐标系时，需要参数pPart。用来调整螺栓线的位置
	/*GECS dcs;
	UCS_STRU new_ucs;
	bool bAdjustPlateUcs=GetReplacedDCS(&dcs);
	if(bAdjustPlateUcs)
		new_ucs=dcs;
	else
		new_ucs=pPlatePart->ucs;*/
	RAYROD_AXIS* pRayRodAxis=NULL;
	CLDSPlate *pPlate=NULL;
	BOOL bDrawOuterLsRay=FALSE;	//添加临时变量控制是否绘制位于板外的螺栓射线 wht 15-08-12
	CHashList<BOLTRAY_PARA>hashBelongRodAxies;	//键值为射线号，数据值为归属的射线杆件的句柄
	CHashListEx<RAYROD_AXIS> hashRayRodAxes;
	if(m_pPlate&&m_pPlate->GetClassTypeId()==CLS_PLATE&& m_pPlate->GetLsCount()>1)
	{	//单孔垫板不需标注任何心线
		pPlate=(CLDSPlate*)m_pPlate;
		CSuperSmartPtr<CLDSLinePart> pRayRod;
		CXhSimpleList<CDesignLjPartPara> listTempLjParts;
		CLDSLineAngle* pRayAngle=NULL;
		CDesignLjPartPara* pLjPart=NULL;
		hashRayRodAxes.Empty();
		for(pLjPart=pPlate->GetFirstLjPara();pLjPart;pLjPart=pPlate->GetNextLjPara())
		{
			if(pLjPart->hPart==pPlate->designInfo.m_hBasePart)
				continue;	//只有射线杆件需要处理，基准杆件及中间穿过连接杆件不需处理
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pPlate->BelongModel()->FromPartHandle(pLjPart->hPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pAngle&&pAngle->GetClassTypeId()==CLS_LINEANGLE&&pAngle->group_father_jg_h==pPlate->designInfo.m_hBasePart)
				continue;	//当前为组合角钢的子角钢，钢板基准角钢为组合父角钢
			else if(pAngle&&pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&(
				((CLDSGroupLineAngle*)pAngle)->son_jg_h[0]==pPlate->designInfo.m_hBasePart ||(((CLDSGroupLineAngle*)pAngle)->son_jg_h[1]==pPlate->designInfo.m_hBasePart||
				((CLDSGroupLineAngle*)pAngle)->son_jg_h[2]==pPlate->designInfo.m_hBasePart ||(((CLDSGroupLineAngle*)pAngle)->son_jg_h[3]==pPlate->designInfo.m_hBasePart))))
				continue;	//当前为组合角钢，钢板基准角钢为其子角钢之一
			pRayRodAxis=hashRayRodAxes.Add(pLjPart->hPart);
			pRayRodAxis->pLjPart=pLjPart;
		}
		//当缺少CLDSPart::designInfo信息时，从螺栓定位基准杆件汇总射线角钢
		for(CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			pRayRodAxis=hashRayRodAxes.GetValue(pLsRef->GetLsPtr()->des_base_pos.hPart);
			CDesignLjPartPara ljpart;
			if(pRayRodAxis==NULL)
			{
				if(pLsRef->GetLsPtr()->des_base_pos.hPart==pPlate->designInfo.m_hBasePart)
					continue;	//只有射线杆件需要处理，基准杆件不需处理
				pRayRod=pPlate->BelongModel()->FromRodHandle(pLsRef->GetLsPtr()->des_base_pos.hPart);
				if(pRayRod.IsNULL())
					continue;
				ljpart.hPart=pRayRod->handle;
				if(pRayRod->GetClassTypeId()!=CLS_LINEANGLE)
					continue;	//暂只处理角钢 wjh-2015.2.8
				double ddx=pPlate->ucs.axis_z*pRayRod.LineAnglePointer()->get_norm_x_wing();
				double ddy=pPlate->ucs.axis_z*pRayRod.LineAnglePointer()->get_norm_y_wing();
				if(pRayRod->pStart&&pRayRod->pStart->handle==pPlate->designInfo.m_hBaseNode)
					ljpart.start0_end1=0;
				else if(pRayRod->pEnd&&pRayRod->pEnd->handle==pPlate->designInfo.m_hBaseNode)
					ljpart.start0_end1=1;
				else
					ljpart.start0_end1=2;
				if(fabs(ddx)>fabs(ddy))
					ljpart.angle.cur_wing_x0_y1=0;
				else
					ljpart.angle.cur_wing_x0_y1=1;
				LIST_NODE<CDesignLjPartPara>* pListNode=listTempLjParts.AttachNode(ljpart);
				pRayRodAxis=hashRayRodAxes.Add(pRayRod->handle);
				pRayRodAxis->pLjPart=&pListNode->data;
			}
		}
		for(pRayRodAxis=hashRayRodAxes.GetFirst();pRayRodAxis;pRayRodAxis=hashRayRodAxes.GetNext())
		{
			pRayRod=pPlate->BelongModel()->FromRodHandle(pRayRodAxis->hRod);
			if(pRayRod.IsNULL())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X plate's linking rod 0X%X is already not exist!",pPlate->handle,pRayRodAxis->hRod);
#else
				logerr.Log("0X%X钢板的连接杆件0X%X已不存在!",pPlate->handle,pRayRodAxis->hRod);
#endif
				continue;
			}
			else if(!pRayRod->IsAngle())
				continue;
			pRayAngle=pRayRod.LineAnglePointer();
			f3dPoint wing_vec;
			double wing_vec_offset=0;
			JGZJ jgzj;
			if(pRayRodAxis->pLjPart->angle.cur_wing_x0_y1==0)
			{
				wing_vec=pRayAngle->GetWingVecX();
				jgzj=pRayAngle->GetZhunJu('X');
				if(pRayAngle->desStartPos.jgber_cal_style==1)
					wing_vec_offset=jgzj.g;
				else if(pRayAngle->desStartPos.wing_y_offset.gStyle==0)
					wing_vec_offset=jgzj.g;
				else if(pRayAngle->desStartPos.wing_y_offset.gStyle==1)
					wing_vec_offset=jgzj.g1;
				else if(pRayAngle->desStartPos.wing_y_offset.gStyle==2)
					wing_vec_offset=jgzj.g2;
				else if(pRayAngle->desStartPos.wing_y_offset.gStyle==3)
					wing_vec_offset=jgzj.g3;
				else
					wing_vec_offset=pRayAngle->desStartPos.wing_y_offset.offsetDist;
			}
			else
			{
				wing_vec=pRayAngle->GetWingVecY();
				jgzj=pRayAngle->GetZhunJu('Y');
				if(pRayAngle->desStartPos.jgber_cal_style==1)
					wing_vec_offset=jgzj.g;
				else if(pRayAngle->desStartPos.wing_x_offset.gStyle==0)
					wing_vec_offset=jgzj.g;
				else if(pRayAngle->desStartPos.wing_x_offset.gStyle==1)
					wing_vec_offset=jgzj.g1;
				else if(pRayAngle->desStartPos.wing_x_offset.gStyle==2)
					wing_vec_offset=jgzj.g2;
				else if(pRayAngle->desStartPos.wing_x_offset.gStyle==3)
					wing_vec_offset=jgzj.g3;
				else
					wing_vec_offset=pRayAngle->desStartPos.wing_x_offset.offsetDist;
			}
			pRayRodAxis->wing_vec=wing_vec;
			pRayRodAxis->ber_space=wing_vec_offset;
			pRayRodAxis->wing_space=pRayAngle->GetWidth()-pRayRodAxis->ber_space;
			int start0_end1=pRayRodAxis->pLjPart->start0_end1;	//为防止原始数据中连接端信息有错误进而导致心线延伸有误,需要进一步进行核实 wjh-2015.2.12
			if(pRayAngle->pStart&&pRayAngle->pStart->handle==pPlate->designInfo.m_hBaseNode)
				start0_end1=0;
			else if(pRayAngle->pEnd&&pRayAngle->pEnd->handle==pPlate->designInfo.m_hBaseNode)
				start0_end1=1;
			else 
			{	//判断端节点非基准节点的杆件始/终端连接 wht 16-03-28
				CLDSLinePart *pBaseRod=(CLDSLineFlat*)pPlate->BelongModel()->FromPartHandle(pPlate->designInfo.m_hBasePart,CLS_LINEPART);
				if(pBaseRod&&pBaseRod->pStart&&pBaseRod->pEnd)
				{
					f3dLine datum_line(pBaseRod->pStart->Position(false),pBaseRod->pEnd->Position(false));
					if(datum_line.PtInLine(pRayAngle->pStart->Position(false))!=0)
						start0_end1=0;
					else if(datum_line.PtInLine(pRayAngle->pEnd->Position(false))!=0)
						start0_end1=1;
				}
			}
			if(start0_end1==0)
			{	//始端连接
				pRayRodAxis->org=pRayAngle->Start()+wing_vec*wing_vec_offset;
				pRayRodAxis->axis_vec=pRayAngle->End()-pRayAngle->Start();
			}
			else
			{	//终端连接
				pRayRodAxis->org=pRayAngle->End()+wing_vec*wing_vec_offset;
				pRayRodAxis->axis_vec=pRayAngle->Start()-pRayAngle->End();
			}
			normalize(pRayRodAxis->axis_vec);
			//统计nSubAxisCount并计算rayend的工作放到AnalizeLsRay中
		}
	}
	ROTATEDIM_STRU rotate_dim;
	ALIGNDIM_STRU align_dim;
	f3dLine line,datum_line;
	BOLTRAY *pLsRay;
	BOOL bFindValidDatumBoltLine=FALSE;
	for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
	{
		//pLsRay = &ls_ray_arr[i];
		BOLT* pFirstBolt =pLsRay->ls_arr.GetFirst();
		BOLT* pSecondBolt=pLsRay->ls_arr.GetNext();
		if(pFirstBolt==NULL)//.GetNodeNum()==0)
			continue;
		else if(pSecondBolt!=NULL)//pLsRay->ls_arr.GetNext()!=NULL)//.GetNodeNum()>1)
		{
			datum_line.startPt=pFirstBolt->location;	//lpLsRay->ls_arr[0];
			datum_line.endPt  =pSecondBolt->location;	//pLsRay->ls_arr[1];
		}
		else //if(pLsRay->ls_arr.GetNodeNum()==1)
		{
			datum_line.startPt.Set();
			datum_line.endPt=pFirstBolt->location;//pLsRay->ls_arr[0];
		}
		if(fabs(datum_line.startPt.x)+fabs(datum_line.endPt.x)<0.1)
		{
			bFindValidDatumBoltLine=TRUE;
			break;
		}
	}
	if(m_pPlate&&m_pPlate->GetClassTypeId()==CLS_PLATE&& m_pPlate->GetLsCount()>1)
	{	//单孔垫板不需标注任何心线
		//将全部坐标转换到最终执行的新绘图坐标系new_ucs下
		for(pRayRodAxis=hashRayRodAxes.GetFirst();pRayRodAxis;pRayRodAxis=hashRayRodAxes.GetNext())
		{	//识别每根射线杆件涵盖的螺栓线
			CLDSLinePart* pRod=pPlate->BelongModel()->FromRodHandle(pRayRodAxis->hRod);
			DWORD rayword=0;
			bool bFirstBolt=true;
			if(pRayRodAxis->pLjPart->iFaceNo<=1)
			{	//单面板
				coord_trans(pRayRodAxis->org,dcs.cs,FALSE,TRUE);
				vector_trans(pRayRodAxis->axis_vec, dcs.cs,FALSE,TRUE);
				vector_trans(pRayRodAxis->wing_vec, dcs.cs,FALSE,TRUE);
			}
			else if(pRayRodAxis->pLjPart->iFaceNo>1)
			{
				coord_trans(pRayRodAxis->org,pPlate->ucs,FALSE,TRUE);
				vector_trans(pRayRodAxis->axis_vec,pPlate->ucs,FALSE,TRUE);
				vector_trans(pRayRodAxis->wing_vec,pPlate->ucs,FALSE,TRUE);
				pRayRodAxis->org=pPlate->GetSpreadVertex(pRayRodAxis->org,pRayRodAxis->pLjPart->iFaceNo,true);
				pRayRodAxis->axis_vec=pPlate->GetSpreadVertex(pRayRodAxis->axis_vec,pRayRodAxis->pLjPart->iFaceNo,false);
				pRayRodAxis->wing_vec=pPlate->GetSpreadVertex(pRayRodAxis->wing_vec,pRayRodAxis->pLjPart->iFaceNo,false);
				//
				coord_trans(pRayRodAxis->org,pPlate->ucs,TRUE,TRUE);
				vector_trans(pRayRodAxis->axis_vec,pPlate->ucs,TRUE,TRUE);
				vector_trans(pRayRodAxis->wing_vec,pPlate->ucs,TRUE,TRUE);
				coord_trans(pRayRodAxis->org, dcs.cs,FALSE,TRUE);
				vector_trans(pRayRodAxis->axis_vec, dcs.cs,FALSE,TRUE);
				vector_trans(pRayRodAxis->wing_vec, dcs.cs,FALSE,TRUE);
			}
			pRayRodAxis->org.z=pRayRodAxis->axis_vec.z=pRayRodAxis->wing_vec.z=0;	//z坐标归零
			GECS rectCS;
			rectCS.origin=pRayRodAxis->org;
			rectCS.axis_x=pRayRodAxis->axis_vec;
			rectCS.axis_y=-pRayRodAxis->wing_vec;
			rectCS.axis_x.z=rectCS.axis_y.z=0;
			normalize(rectCS.axis_x);
			rectCS.axis_z=rectCS.axis_x^rectCS.axis_y;
			if(rectCS.axis_z.z>0)
				rectCS.axis_y=f3dPoint(0,0,1)^rectCS.axis_x;
			else
				rectCS.axis_y=f3dPoint(0,0,-1)^rectCS.axis_x;
			//LSRAY_STRU *pLsRay;
			//for(i=0;i<ls_ray_arr.GetSize();i++)
			for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
			{
				//pLsRay=&ls_ray_arr[i];
				//不绘制单排螺栓丁字尺寸时,max_y和min_y不需要设置默认值为0 wht 15-08-12
				CMaxDouble max_y;
				CMinDouble min_x,min_y;
				bool bOuterBolt=false,samelenVec=true;
				GEPOINT rayvec=pLsRay->rayResideVec;
				rayvec=rectCS.TransVToCS(rayvec);
				BOLT* pFirstBolt =pLsRay->ls_arr.GetFirst();
				BOLT* pSecondBolt=pLsRay->ls_arr.GetNext();
				if(pSecondBolt!=NULL)//pLsRay->ls_arr.GetNext()!=NULL)//pLsRay->ls_arr.GetNodeNum()>1)
				{
					rayvec=pSecondBolt->location-pFirstBolt->location;//pLsRay->ls_arr[1]-pLsRay->ls_arr[0];
					normalize(rayvec);
					rayvec=rectCS.TransVToCS(rayvec);
				}
				//for(f3dPoint *pLocate=pLsRay->ls_arr.GetFirst();pLocate;pLocate=pLsRay->ls_arr.GetNext())
				for(BOLT* pBolt=pLsRay->ls_arr.GetFirst();pBolt;pBolt=pLsRay->ls_arr.GetNext())
				{
					GEPOINT location=rectCS.TransPToCS(pBolt->location);
					if(location.x<=-EPS2)
						bOuterBolt=true;
					else if(location.y>=pRayRodAxis->ber_space)
						bOuterBolt=true;
					else if(location.y<=-pRayRodAxis->wing_space)
						bOuterBolt=true;
					if(fabs(rayvec.y)>EPS2)	//该射线方向与当前射线杆件方向不一致，即使全部螺栓在当前杆件内也不可视为涵盖射线
						bOuterBolt=true;
					if(bOuterBolt)
						break;
					max_y.Update(location.y,pBolt);
					min_y.Update(location.y,pBolt);
					if(min_x.IsInited()&&location.x<min_x)
						samelenVec=false;	//pLsRay->ls_arr由远至近，与螺栓射线方向相反
					min_x.Update(location.x);
				}
				if(!bOuterBolt)
				{
					pRayRodAxis->ber_dist=max_y;
					pRayRodAxis->wing_dist=min_y;
					if(pRayRodAxis->ray_dist>min_x)
						pRayRodAxis->ray_dist=min_x;
					double coord_y=0;
					if(min_y.m_pRelaObj)
						coord_y=min_y;
					else if(max_y.m_pRelaObj)
						coord_y=max_y;
					BOLTRAY_PARA* pRayPara=hashBelongRodAxies.SetValue(pLsRay->iRayNo,BOLTRAY_PARA(pRayRodAxis->hRod,min_x,coord_y));
					pRayPara->reverseLenvec=!samelenVec;
				}
			}
		}
		//补齐单孔射线
		for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
		{
			//pLsRay = &ls_ray_arr[i];
			f3dPoint line_vec;
			BOLTRAY_PARA* pRayBelongPara=hashBelongRodAxies.GetValue(pLsRay->iRayNo);
			if(pRayBelongPara)
				pRayRodAxis=hashRayRodAxes.GetValue(pRayBelongPara->hRod);
			else
				pRayRodAxis=NULL;
			if(pRayRodAxis==NULL)
				continue;	//可能该射线属基准角钢螺栓心线
			BOLT* pFirstBolt =pLsRay->ls_arr.GetFirst();
			BOLT* pSecondBolt=pLsRay->ls_arr.GetNext();
			if(pFirstBolt!=NULL && pSecondBolt==NULL)//pLsRay->ls_arr.GetNodeNum()==1)
			{	//补齐单孔射线
				if(pRayRodAxis->pLjPart->start0_end1<2&&fabs(pRayBelongPara->coord_y)<1)
				{
					if(bDrawOuterLsRay||PtInPlateRgn(pRayRodAxis->org))
					{
						for(BOLT* pBolt=pLsRay->ls_arr.GetFirst();pBolt;pBolt=pLsRay->ls_arr.GetNext())
						{	//筛选掉过重叠在一起的螺栓点
							if(DISTANCE(pBolt->location,pRayRodAxis->org)<EPS)
								break;
						}
						if(pBolt==NULL)
							pLsRay->ls_arr.append(BOLT(pRayRodAxis->org))->siReserve=-1;	//feature=-1表示该点不需要延伸绘制
					}
				}
				//对于单孔射线ray_dist距离为螺栓孔到org的距离，以下代码计算得到的点与insertpos重合，以下代码注释不用 wht 15-08-12
				/*else
				{
					f3dPoint insertpos=pRayRodAxis->org+pRayRodAxis->axis_vec*pRayRodAxis->ray_dist;
					if(SnapPerp(&insertpos,pLsRay->ls_arr[0],pLsRay->ls_arr[0]+pRayRodAxis->axis_vec,insertpos))
						pLsRay->ls_arr.append(insertpos)->feature=-1;	//feature=-1表示该点不需要延伸绘制
				}*/
			}
			else if(pRayBelongPara->coord_x>pRayRodAxis->ray_dist+1)	//容许误差预定值为1mm
			{	//补齐多孔射线与同属（归属同一射线杆件）射线螺栓孔间的错孔点
				f3dPoint ray_pick1=pFirstBolt->location;//pLsRay->ls_arr[0];
				f3dPoint ray_pick2=pSecondBolt->location;//pLsRay->ls_arr[1];
				f3dPoint insertpos=pRayRodAxis->org+pRayRodAxis->axis_vec*pRayRodAxis->ray_dist;
				SnapPerp(&insertpos,ray_pick1,ray_pick2,insertpos);
				if(pRayBelongPara->reverseLenvec)
					pLsRay->ls_arr.append(BOLT(insertpos))->siReserve=-1;	//feature=-1表示该点不需要延伸绘制
				else
					pLsRay->ls_arr.insert(BOLT(insertpos),0)->siReserve=-1;	//feature=-1表示该点不需要延伸绘制
			}
			else if(pRayBelongPara->coord_x>1&&fabs(pRayBelongPara->coord_y)<1&&pRayRodAxis->pLjPart->start0_end1<2)
			{	//单排射线未交于钢板定位原点螺栓的情况，补齐预定位点
				if(!bDrawOuterLsRay&&!PtInPlateRgn(pRayRodAxis->org))
					continue;	//pRayRodAxis->org不在钢板内
				if(pRayBelongPara->reverseLenvec)
					pLsRay->ls_arr.append(BOLT(pRayRodAxis->org))->siReserve=-1;	//feature=-1表示该点不需要延伸绘制
				else
					pLsRay->ls_arr.insert(BOLT(pRayRodAxis->org),0)->siReserve=-1;	//feature=-1表示该点不需要延伸绘制
			}
		}
		for(pRayRodAxis=hashRayRodAxes.GetFirst();pRayRodAxis;pRayRodAxis=hashRayRodAxes.GetNext())
		{	
			if(pRayRodAxis->ray_dist>1&&fabs(pRayRodAxis->ber_dist-pRayRodAxis->wing_dist)>1
				&&(bDrawOuterLsRay||PtInPlateRgn(pRayRodAxis->org)))	//pRayRodAxis->org在钢板内
			{
				//LSRAY_STRU lsray;
				//lsray.iRayNo=0;
				if(pRayRodAxis->pLjPart->start0_end1<2)
				{
					//ls_ray_arr.Add(lsray);
					//i = ls_ray_arr.GetUpperBound();
					pLsRay=hashBoltRays.Add(0);	//自动分配一空闲射线号
					pLsRay->ls_arr.append(BOLT(pRayRodAxis->org))->siReserve=-1;	//feature=-1表示该点不需要延伸绘制
					pLsRay->ls_arr.append(BOLT(pRayRodAxis->org+pRayRodAxis->ray_dist*pRayRodAxis->axis_vec))->siReserve=-1;
				}
				//ls_ray_arr.Add(lsray);
				//i = ls_ray_arr.GetUpperBound();
				pLsRay=hashBoltRays.Add(0);	//自动分配一空闲射线号
				GEPOINT ray_pos=pRayRodAxis->org+pRayRodAxis->ray_dist*pRayRodAxis->axis_vec;
				GEPOINT ber_pos=ray_pos-pRayRodAxis->wing_vec*pRayRodAxis->ber_dist;
				GEPOINT wing_pos=ray_pos-pRayRodAxis->wing_vec*pRayRodAxis->wing_dist;
				pLsRay->ls_arr.append(BOLT(ber_pos))->siReserve=-1;
				if( fabs(pRayRodAxis->ber_dist)>=1&&fabs(pRayRodAxis->wing_dist)>1&&fabs(pRayRodAxis->ber_dist-pRayRodAxis->wing_dist)>1
					&&pRayRodAxis->pLjPart->start0_end1<2)
					pLsRay->ls_arr.append(BOLT(ray_pos))->siReserve=-1;
				pLsRay->ls_arr.insert(BOLT(wing_pos))->siReserve=-1;
			}
		}
	}
	if(!bFindValidDatumBoltLine)
	{	//没有找到合适的螺栓线作为定位基准线
		datum_line.startPt.Set(0,-80);
		datum_line.endPt.Set(0,80);
	}
	ANGULARDIM_STRU angular_dim;
	for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
	{
		f3dPoint vec;
		//pLsRay = &ls_ray_arr[i];
		BOLT* pFirstBolt =pLsRay->ls_arr.GetFirst();
		BOLT* pSecondBolt=pLsRay->ls_arr.GetNext();
		if(pFirstBolt==NULL)//pLsRay->ls_arr.GetNodeNum()==0)
			continue;
		else if(pSecondBolt!=NULL)//pLsRay->ls_arr.GetNodeNum()>1)
		{
			line.startPt=pFirstBolt->location;	//pLsRay->ls_arr[0];
			line.endPt  =pSecondBolt->location;	//pLsRay->ls_arr[1];
		}
		else
		{
			line.startPt.Set();
			line.endPt=pFirstBolt->location;//pLsRay->ls_arr[0];
		}
		pLsRay->rayOrg=line.startPt;
		vec=line.endPt-line.startPt;
		rotate_dim.angle = Cal2dLineAng(0,0,vec.x,vec.y);
		rotate_dim.endPos = pFirstBolt->location;//pLsRay->ls_arr[0];
		if(sys.part_map.plate.bDimPlateRayLs)
		{	//是否标注板螺栓射线距离
			for(BOLT* pBolt=pSecondBolt;pBolt;pBolt=pLsRay->ls_arr.GetNext())
			{
				rotate_dim.startPos = rotate_dim.endPos;
				rotate_dim.endPos = pBolt->location;//pLsRay->ls_arr[j];
				rotate_dim.dist = DISTANCE(rotate_dim.startPos,rotate_dim.endPos);	//输出时控制精度 wht 15-07-13
				Sub_Pnt(rotate_dim.dimPos,rotate_dim.endPos,
					rotate_dim.startPos);
				Add_Pnt(rotate_dim.dimPos,rotate_dim.startPos,
					rotate_dim.dimPos*0.5);
				m_xRotateDimList.AttachObject(rotate_dim);
			}
		}
		vec.z=line.startPt.z=line.endPt.z=0;
		normalize(vec);
		//螺栓射线角度的标注
		f3dPoint datum_line_vec=datum_line.endPt-datum_line.startPt;
		f3dPoint line_vec = line.endPt-line.startPt;
		normalize(datum_line_vec);
		normalize(line_vec);
		if(fabs(datum_line_vec*line_vec)<EPS_COS2&&sys.part_map.plate.bDimPlateBoltAngle)
		{	//标注板螺栓角度
			int ret=Int2dll(f2dLine(datum_line.startPt,datum_line.endPt),f2dLine(line.startPt,line.endPt),angular_dim.vertex.x,angular_dim.vertex.y);
			if(ret!=0&&ret!=-1)	
			{
				if(vec.y>0)
					angular_dim.startPos.Set(angular_dim.vertex.x,angular_dim.vertex.y+30);
				else
					angular_dim.startPos.Set(angular_dim.vertex.x,angular_dim.vertex.y-30);
				angular_dim.endPos=angular_dim.vertex+vec*30;
				angular_dim.dimPos=0.5*(angular_dim.startPos+angular_dim.endPos);
				m_xAngularDimList.AttachObject(angular_dim);
				rotate_dim.angle = Cal2dLineAng(0,0,vec.x,vec.y);
				rotate_dim.startPos=angular_dim.vertex;
				rotate_dim.endPos = pFirstBolt->location;//pLsRay->ls_arr[0];
				rotate_dim.dist = DISTANCE(rotate_dim.startPos,rotate_dim.endPos);//输出时控制精度 wht 15-07-13
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				m_xRotateDimList.AttachObject(rotate_dim);
				pLsRay->rayOrg=angular_dim.vertex;
			}
		}
	}
	//标注与火曲线平行的螺栓射线间距   wxc-2015.9.9
	if(pPlate==NULL || pPlate->GetFaceN()<=1)
		return;
	f3dLine huoqu_line;
	f3dPoint huoqu_vec,line_vec,huoqu_start,huoqu_end,tem_pt;
	for(int m=1;m<pPlate->GetFaceN();m++)
	{
		BOOL bFindStart=FALSE;
		huoqu_line=pPlate->GetHuoquLine(m-1);
		huoqu_start=huoqu_line.startPt;
		huoqu_end=huoqu_line.endPt;
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(huoqu_line.PtInLine(pVertex->vertex)!=0
				&&pVertex->vertex.feature>10)
			{
				if(!bFindStart)
				{
					huoqu_start=pVertex->vertex;
					bFindStart=TRUE;
				}
				else
				{
					huoqu_end=pVertex->vertex;
					break;
				}
			}
		}
		if(sys.part_map.bExportDeformedProfile)
		{
			huoqu_start=pPlate->GetDeformedVertex(huoqu_start);
			huoqu_end=pPlate->GetDeformedVertex(huoqu_end);
		}
		coord_trans(huoqu_start,pPlate->ucs,TRUE,TRUE);
		coord_trans(huoqu_start, dcs.cs,FALSE,TRUE);
		coord_trans(huoqu_end,pPlate->ucs,TRUE,TRUE);
		coord_trans(huoqu_end, dcs.cs,FALSE,TRUE);
		huoqu_vec=huoqu_end-huoqu_start;
		normalize(huoqu_vec);
		double fMinDist=1000;
		//int index=-1;
		//for(i=0;i<ls_ray_arr.GetSize();i++)
		BOLTRAY* pNearestRay=NULL;
		for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
		{
			//pLsRay = &ls_ray_arr[i];
			BOLT* pFirstBolt =pLsRay->ls_arr.GetFirst();
			BOLT* pSecondBolt=pLsRay->ls_arr.GetNext();
			if(pSecondBolt==NULL)//pLsRay->ls_arr.GetNodeNum()<2)
				continue;
			line.startPt=pFirstBolt->location;//pLsRay->ls_arr[0];
			line.endPt =pSecondBolt->location;//pLsRay->ls_arr[1];
			line_vec=line.endPt-line.startPt;
			normalize(line_vec);
			if(fabs(huoqu_vec*line_vec)<EPS_COS2)
				continue;
			double dist=DistOf2dPtLine(line.startPt,huoqu_start,huoqu_end);
			if(fMinDist>fabs(dist))
			{
				fMinDist=fabs(dist);
				//index=i;
				pNearestRay=pLsRay;
			}
		}
		if(pNearestRay)//index>=0)
		{
			//pLsRay = &ls_ray_arr[index];
			//int n=pLsRay->ls_arr.GetNodeNum();
			//rotate_dim.startPos=tem_pt=0.5*(pLsRay->ls_arr[0]+pLsRay->ls_arr[n-1]);
			BOLT* pFirstBolt=pNearestRay->ls_arr.GetFirst();
			BOLT* pTailBolt=pNearestRay->ls_arr.GetTail();
			rotate_dim.startPos=tem_pt=0.5*(pFirstBolt->location+pTailBolt->location);
			SnapPerp(&tem_pt,huoqu_start,huoqu_end,tem_pt);
			rotate_dim.endPos=tem_pt;
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dist = fMinDist;
			rotate_dim.angle = Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			m_xRotateDimList.AttachObject(rotate_dim);
		}
	}
}
BOOL CDimPlateBolts::DimInsertBoltsRaylineSpace(CLDSParamPlate *pPlate)
{
	if (!pPlate->IsInsertPlate() || hashBoltRays.GetNodeNum() <= 1)
		return FALSE;	//插板上有多个螺栓时，标注螺栓间距
	ARRAY_LIST<BOLTRAY*> rayPtrArr;
	for(BOLTRAY *pRay=hashBoltRays.GetFirst();pRay;pRay=hashBoltRays.GetNext())
		rayPtrArr.append(pRay);
	CHeapSort<BOLTRAY*>::HeapSort(rayPtrArr.m_pData,rayPtrArr.Size(),CompareBoltRayLinePtr);
	double L=0;
	pPlate->GetDesignItemValue('L',&L);
	ROTATEDIM_STRU rotate_dim,*pPrevDim=NULL;
	for(DWORD i=0;i<rayPtrArr.Size()-1;i++)
	{
		BOLTRAY *pCurRay=(*rayPtrArr.GetByIndex(i));
		BOLTRAY *pNextRay=(*rayPtrArr.GetByIndex(i+1));
		BOLT *pStartBolt=pCurRay->ls_arr.GetTail();
		BOLT *pEndBolt=pNextRay->ls_arr.GetTail();
		if(pStartBolt&&pEndBolt)
		{
			rotate_dim.startPos=pStartBolt->location;
			rotate_dim.endPos=pEndBolt->location;
			rotate_dim.dist=fabs(rotate_dim.startPos.y-rotate_dim.endPos.y);
			rotate_dim.angle=0.5*Pi;
			if(rayPtrArr.Size()==2)
			{	//插板上只有两行螺栓时,标注内容按DX2进行标注
				pPrevDim=m_xRotateDimList.AttachObject(rotate_dim);
				pPrevDim->dist*=0.5;
				pPrevDim->nHits=2;
			}
			else
			{	//插板上有多行螺栓时,合并相邻尺寸一样的标注
				if(pPrevDim&&fabs(pPrevDim->dist-rotate_dim.dist)<EPS2)
				{
					pPrevDim->endPos=pEndBolt->location;
					pPrevDim->nHits++;
				}
				else
				{
					pPrevDim=m_xRotateDimList.AttachObject(rotate_dim);
					pPrevDim->nHits=1;
				}
			}
			pPrevDim->dimPos=(pPrevDim->startPos+pPrevDim->endPos)*0.5;
			pPrevDim->dimPos.x=L;
		}
	}
	return TRUE;
}
BOOL CDimPlateBolts::Create(CLDSParamPlate *pPlate, UCS_STRU* pCs /*= NULL*/)
{
	if (pPlate == NULL)
		return FALSE;
	m_pPlate = pPlate;
	if (pCs)
		dcs.SetCS(*pCs);
	else
		dcs.SetCS(pPlate->ucs);
	ALIGNDIM_STRU align_dim;
	char sGuiGe[MAX_CHAR_GUIGE_16+1];
	f3dPoint basePt;
	double L=0;
	pPlate->GetDesignItemValue('L',&L);
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		if(pPlate->m_iParamType==TYPE_XEND)
		{	//X插板
			f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
			coord_trans(ls_pos,pPlate->ucs,FALSE);
			if(ls_pos.x>L||fabs(pLsRef->GetLsPtr()->get_norm()*pPlate->ucs.axis_z)<EPS_COS2)
				continue;	//不绘制不在十字插板基板长度范围内的螺栓
		}
		sprintf(sGuiGe,"%sM%dX%.f",(*pLsRef)->Grade(),(*pLsRef)->get_d(),(*pLsRef)->get_L());
		Int3dlf(basePt,(*pLsRef)->ucs.origin,(*pLsRef)->ucs.axis_z,pPlate->ucs.origin,pPlate->ucs.axis_z);
		coord_trans(basePt,pPlate->ucs,FALSE);
		basePt.feature = (*pLsRef)->get_d();
		basePt.ID=(*pLsRef)->handle;
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
				AnalyzeBoltsRayline(BOLT(basePt),i,sGuiGe);
		}
	}
	if(sys.stru_map.bDimSpecialLsSpace)
		DimBoltsRayline();
	DimInsertBoltsRaylineSpace(pPlate);
	return TRUE;
}
BOOL CDimPlateBolts::Create(CLDSPlate *pPlate, UCS_STRU* pCs /*= NULL*/)
{
	if (pPlate == NULL)
		return FALSE;
	m_pPlate = pPlate;
	if (pCs)
		dcs.SetCS(*pCs);
	else
		dcs.SetCS(pPlate->ucs);
	ALIGNDIM_STRU align_dim;
	CXhChar16 sGuiGe;
	f3dPoint basePt;
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		sGuiGe.Printf("%sM%dX%.f",(*pLsRef)->Grade(),(*pLsRef)->get_d(),(*pLsRef)->get_L());
		basePt = (*pLsRef)->ucs.origin;
		pPlate->SpreadLsPos(pLsRef,basePt);
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
				AnalyzeBoltsRayline(BOLT(basePt),i,sGuiGe);
		}
	}
	if(sys.stru_map.bDimSpecialLsSpace)
	{
		//判断螺栓射线上的螺栓是否为双排螺栓 wht 11-06-30
		CHashTable<int> rayNoTable;			//定位构件为key 射线号为内容
		CHashTable<int> doubleLsRayNoTable;	//key及内容均为射线号,用来记录双排螺栓射线号
		rayNoTable.CreateHashTable(50);
		doubleLsRayNoTable.CreateHashTable(32);
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			long hPart=pLsRef->GetLsPtr()->des_base_pos.hPart;
			int iRayNo=0;
			for(int i=1;i<=32;i++)
			{
				if(pLsRef->dwRayNo&GetSingleWord(i))
				{
					if(iRayNo==0)
						iRayNo=i;
					else if(iRayNo>0)
					{	//该螺栓引用有多个射线号
						iRayNo=0;
						break;
					}
				}
			}
			if(iRayNo==0)
				continue;	//不考虑有多个射线号的螺栓引用(一般都由端连接造成)
			int iFirstRayNo=0;
			if(rayNoTable.GetValueAt(hPart,iFirstRayNo))
			{
				if(iFirstRayNo>0&&iFirstRayNo!=iRayNo)
				{	//同一杆件上存在存在多条螺栓射线，该杆件上的螺栓为双排螺栓
					int nTemp=0;
					if(!doubleLsRayNoTable.GetValueAt(iRayNo,nTemp))
						doubleLsRayNoTable.SetValueAt(iRayNo,iRayNo);
					if(!doubleLsRayNoTable.GetValueAt(iFirstRayNo,nTemp))
						doubleLsRayNoTable.SetValueAt(iFirstRayNo,iFirstRayNo);
				}
			}
			else //未设置
				rayNoTable.SetValueAt(hPart,iRayNo);
		}
		for(BOLTRAY* pBoltRay=hashBoltRays.GetFirst();pBoltRay;pBoltRay=hashBoltRays.GetNext())
		{
			int nTemp=0;
			if(doubleLsRayNoTable.GetValueAt(pBoltRay->iRayNo,nTemp))
				pBoltRay->bIsDoubleLsRay=TRUE;
		}
		DimBoltsRayline();
	}
	return TRUE;
}
void CDimPlateBolts::InitPlateRgn(f3dPoint* lp3dPoints, int nCount)
{
	m_xEdgeRegion.CreatePolygonRgn(lp3dPoints, nCount);
}
//判断坐标点是否在钢板内
bool CDimPlateBolts::PtInPlateRgn(const double* poscoord)
{
	if (m_xEdgeRegion.GetAxisZ().IsZero())
		return true;
	int nRetCode = m_xEdgeRegion.PtInRgn(poscoord);
	return nRetCode == 1;
}
//
IDimPlateBolts::BOLTRAY* CDimPlateBolts::FindLsRay(f3dPoint* lsPos1, f3dPoint* lsPos2)
{
	for(BOLTRAY* pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
	{
		BOOL bFind1 = FALSE, bFind2 = FALSE;
		for (BOLT* pBoltLoca=pLsRay->ls_arr.GetFirst(); pBoltLoca; pBoltLoca = pLsRay->ls_arr.GetNext())
		{
			if (pBoltLoca->location.IsEqual(*lsPos1, EPS2))
				bFind1 = TRUE;
			if (lsPos2 == NULL || pBoltLoca->location.IsEqual(*lsPos2, EPS2))
				bFind2 = TRUE;
			if (bFind1&&bFind2)
				return pLsRay;
		}
	}
	return NULL;
}
//修正螺栓射线
void CDimPlateBolts::CorrectLsRays()
{
	//删除无效的螺栓射线
	BOLTRAY* pLsRay = NULL;
	ATOM_LIST<BOLTRAY> xLsRayList;
	for (BOLTRAY* pSrcLsRay = hashBoltRays.GetFirst(); pSrcLsRay; pSrcLsRay = hashBoltRays.GetNext())
	{
		pLsRay = xLsRayList.append();
		pLsRay->CloneMember(pSrcLsRay);
	}
	for (pLsRay = xLsRayList.GetFirst(); pLsRay; pLsRay = xLsRayList.GetNext())
	{
		if (pLsRay->ls_arr.GetSize() == 0)
		{
			pLsRay->featrue = 1;
			continue;
		}
		BOLT *pHeadBolt = pLsRay->ls_arr.GetFirst();
		if (pLsRay->ls_arr.GetSize() > 1)
		{	//判断多个螺栓的是否有效
			for(BOLT* pBoltLoca =pLsRay->ls_arr.GetNext(); pBoltLoca; pBoltLoca =pLsRay->ls_arr.GetNext())
			{
				if (pHeadBolt->location.IsEqual(pBoltLoca->location, EPS2))
				{
					pLsRay->featrue = 1;
					break;
				}
			}
		}
		else
		{	//判断单个螺栓的是否有效
			int nPush = xLsRayList.push_stack(); 
			for (BOLTRAY* pLsRay2 = xLsRayList.GetFirst(); pLsRay2; pLsRay2 = xLsRayList.GetNext())
			{
				if (pLsRay == pLsRay2 || pLsRay2->featrue == 1)
					continue;
				BOLT* pBoltLoca = NULL;
				for (pBoltLoca = pLsRay2->ls_arr.GetFirst(); pBoltLoca; pBoltLoca = pLsRay2->ls_arr.GetNext())
				{
					if (pHeadBolt->location.IsEqual(pBoltLoca->location, EPS2))
						break;
				}
				if (pBoltLoca)
				{
					pLsRay->featrue = 1;
					break;
				}
			}
			xLsRayList.pop_stack(nPush);
		}
		//判断有两个点的螺栓射线是否有效(多排螺栓临时添加的辅助射线不做处理)
		if (pLsRay->ls_arr.GetSize() == 2 && pLsRay->featrue != 1 && pLsRay->iRayNo!=0)
		{
			GEPOINT ray_vec = (pLsRay->ls_arr[1].location - pLsRay->ls_arr[0].location);
			normalize(ray_vec);
			if (fabs(ray_vec*pLsRay->rayResideVec)<0.999)
				pLsRay->featrue = 1;
		}
	}
	for (pLsRay = xLsRayList.GetFirst(); pLsRay; pLsRay = xLsRayList.GetNext())
	{
		if (pLsRay->featrue == 1)
			xLsRayList.DeleteCursor();
	}
	xLsRayList.Clean();
	//重新填充螺栓射线信息
	hashBoltRays.Empty();
	for (pLsRay = xLsRayList.GetFirst(); pLsRay; pLsRay = xLsRayList.GetNext())
	{
		BOOL bCopyLsArr = FALSE;
		if (pLsRay->ls_arr.GetSize() <= 1)
			bCopyLsArr = TRUE;
		else
		{
			GEPOINT vec = pLsRay->ls_arr[1].location - pLsRay->ls_arr[0].location;
			normalize(vec);
			if (vec*pLsRay->rayResideVec > 0)
				bCopyLsArr = TRUE;
		}
		BOLTRAY* pRay= hashBoltRays.Add(pLsRay->iRayNo);
		pRay->CloneMember(pLsRay, bCopyLsArr);
		if (bCopyLsArr == FALSE)
		{	//需反转螺栓坐标列表
			for (BOLT* pBoltLoca = pLsRay->ls_arr.GetTail(); pBoltLoca; pBoltLoca = pLsRay->ls_arr.GetPrev())
				pRay->ls_arr.append(*pBoltLoca);
		}
	}
}
//完善螺栓射线信息
void CDimPlateBolts::CompleteLsRayInfo()
{
	//查找钢板的依赖杆件及定位节点
	CSuperSmartPtr<CLDSPart> pSuperPlate = m_pPlate;
	CLDSLineAngle* pDatumAngle = NULL, *pAngle = NULL;
	CLDSNode* pDatumNode = NULL;
	if (m_pPlate->GetClassTypeId() == CLS_PLATE)
	{
		pDatumNode = m_pPlate->BelongModel()->FromNodeHandle(pSuperPlate.PlatePointer()->designInfo.m_hBaseNode);
		pDatumAngle = (CLDSLineAngle*)m_pPlate->BelongModel()->FromPartHandle(pSuperPlate.PlatePointer()->designInfo.m_hBasePart, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
		CDesignLjPartPara* pDesLjPara = NULL;
		if (pDatumAngle && pDatumAngle->GetClassTypeId() == CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupJg = (CLDSGroupLineAngle*)pDatumAngle;
			for (int i = 0; i < 4; i++)
			{
				pDesLjPara = pSuperPlate.PlatePointer()->GetLjRodPara(pGroupJg->son_jg_h[i]);
				if (pDesLjPara)
					break;
			}
			if (pDesLjPara)
				pDatumAngle = (CLDSLineAngle*)m_pPlate->BelongModel()->FromPartHandle(pDesLjPara->hPart);
			else
				pDatumAngle = (CLDSLineAngle*)m_pPlate->BelongModel()->FromPartHandle(pGroupJg->m_hDatumSonAngle);
		}
		else if(pDatumAngle!=NULL && pSuperPlate.pPlate->GetLjRodPara(pDatumAngle->handle) == NULL)
		{	//基准杆件一定是连接杆件之一，否则表示不合法，此时查找与基准杆件共线的连接杆件
			CPtInLineCheck check(pDatumAngle->xPosStart, pDatumAngle->xPosEnd);
			for (pDesLjPara = pSuperPlate.pPlate->designInfo.partList.GetFirst(); pDesLjPara; pDesLjPara = pSuperPlate.pPlate->designInfo.partList.GetNext())
			{
				if((pAngle= (CLDSLineAngle*)m_pPlate->BelongModel()->FromPartHandle(pDesLjPara->hPart))==NULL)
					continue;
				bool blStartInLine = false, blEndInLine = false;
				check.CheckPointEx(pAngle->xPosStart, &blStartInLine, 1);
				check.CheckPointEx(pAngle->xPosStart, &blEndInLine, 1);
				if (blStartInLine&&blEndInLine)
				{	//钢板连接杆件中找到了更合适的基准杆件，这部分主要是修正过往钢板基准杆件数据错误的。 wjh-2019.5.19
					pDatumAngle = pAngle;
					break;
				}
			}
		}
	}
	else //if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if (pSuperPlate.ParamPlatePointer()->m_iParamType == TYPE_SHOEPLATE)
		{	//靴板
			pDatumNode = pSuperPlate.pParamPlate->GetBaseNode();
			long hDatumJg = 0;
			pSuperPlate.ParamPlatePointer()->GetDesignItemValue('D', &hDatumJg);
			pDatumAngle = (CLDSLineAngle*)m_pPlate->BelongModel()->FromPartHandle(hDatumJg, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
		}
		else if (pSuperPlate.ParamPlatePointer()->m_iParamType == TYPE_TUBERIBPLATE)
		{	//分析跨越连板螺栓射线

		}
	}
	if (pDatumAngle == NULL || pDatumNode == NULL)
		return;	//暂只处理角钢
	BOOL bDrawOuterLsRay = FALSE;	//添加临时变量控制是否绘制位于板外的螺栓射线 wht 15-08-12
	BOLTRAY *pLsRay = NULL;
	RAYROD_AXIS* pRayRodAxis = NULL;
	CHashList<BOLTRAY_PARA>hashBelongRodAxies;	//键值为射线号，数据值为归属的射线杆件的句柄
	CHashListEx<RAYROD_AXIS> hashRayRodAxes;
	CLDSLineAngle* pRayAngle = NULL;
	hashRayRodAxes.Empty();
	if (m_pPlate->GetClassTypeId() == CLS_PLATE)
	{	//单孔垫板不需标注任何心线
		CLDSPlate *pPlate = (CLDSPlate*)m_pPlate;
		for (CDesignLjPartPara* pLjPart = pPlate->GetFirstLjPara(); pLjPart; pLjPart = pPlate->GetNextLjPara())
		{
			CLDSLineAngle* pAngle = (CLDSLineAngle*)pPlate->BelongModel()->FromPartHandle(pLjPart->hPart, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
			if (pAngle == NULL)
				continue;	//暂只处理角钢
			pRayRodAxis = hashRayRodAxes.Add(pLjPart->hPart);
			pRayRodAxis->iFaceNo = pLjPart->iFaceNo;
			pRayRodAxis->ciWorkWingX0Y1 = pLjPart->angle.cur_wing_x0_y1;
			//为防止原始数据中连接端信息有错误进而导致心线延伸有误,需要进一步进行核实 wjh-2015.2.12
			if (pAngle->pStart&&pAngle->pStart->handle == pPlate->designInfo.m_hBaseNode)
				pRayRodAxis->start_end = 0;
			else if (pAngle->pEnd&&pAngle->pEnd->handle == pPlate->designInfo.m_hBaseNode)
				pRayRodAxis->start_end = 1;
			else
				pRayRodAxis->start_end = 2;
			double ddx = pPlate->ucs.axis_z*pAngle->vxWingNorm;
			double ddy = pPlate->ucs.axis_z*pAngle->vyWingNorm;
			if (fabs(ddx) > fabs(ddy))
				pRayRodAxis->ciWorkWingX0Y1 = 0;
			else
				pRayRodAxis->ciWorkWingX0Y1 = 1;
			//判断当前杆件是否为基准角钢
			if (pLjPart->hPart == pDatumAngle->handle)
				pRayRodAxis->bDatumRod = true;
		}
	}
	//当缺少CLDSPart::designInfo信息时，从螺栓定位基准杆件汇总射线角钢
	for (CLsRef* pLsRef = m_pPlate->GetFirstLsRef(); pLsRef; pLsRef = m_pPlate->GetNextLsRef())
	{
		long hResideRod = pLsRef->GetLsPtr()->des_base_pos.hPart;
		if ((pRayRodAxis = hashRayRodAxes.GetValue(hResideRod)) != NULL)
			continue;
		if ((pRayAngle = (CLDSLineAngle*)m_pPlate->BelongModel()->FromPartHandle(hResideRod,CLS_LINEANGLE, CLS_GROUPLINEANGLE)) == NULL)
			continue;	//暂只处理角钢 wjh-2015.2.8
		pRayRodAxis = hashRayRodAxes.Add(pRayAngle->handle);
		if (pRayAngle->pStart&&pRayAngle->pStart->handle == pDatumNode->handle)
			pRayRodAxis->start_end = 0;
		else if (pRayAngle->pEnd&&pRayAngle->pEnd->handle == pDatumNode->handle)
			pRayRodAxis->start_end = 1;
		else
			pRayRodAxis->start_end = 2;
		double ddx = pLsRef->GetLsPtr()->ucs.axis_z*pRayAngle->vxWingNorm;
		double ddy = pLsRef->GetLsPtr()->ucs.axis_z*pRayAngle->vyWingNorm;
		if (fabs(ddx) > fabs(ddy))
			pRayRodAxis->ciWorkWingX0Y1 = 0;
		else
			pRayRodAxis->ciWorkWingX0Y1 = 1;
		//判断当前杆件是否为基准角钢
		if (pRayRodAxis->hRod == pDatumAngle->handle)
			pRayRodAxis->bDatumRod = true;
	}
	if(hashRayRodAxes.GetNodeNum()>0)
	{
		for (pRayRodAxis = hashRayRodAxes.GetFirst(); pRayRodAxis; pRayRodAxis = hashRayRodAxes.GetNext())
		{
			pRayAngle = (CLDSLineAngle*)m_pPlate->BelongModel()->FromRodHandle(pRayRodAxis->hRod);
			if (pRayAngle == NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X plate's linking rod 0X%X is already not exist!", m_pPlate->handle, pRayRodAxis->hRod);
#else
				logerr.Log("0X%X钢板的连接杆件0X%X已不存在!", m_pPlate->handle, pRayRodAxis->hRod);
#endif
				continue;
			}
			JGZJ jgzj;
			if (pRayRodAxis->ciWorkWingX0Y1 == 0)
			{
				pRayRodAxis->wing_vec = pRayAngle->GetWingVecX();
				jgzj = pRayAngle->GetZhunJu('X');
			}
			else
			{
				pRayRodAxis->wing_vec = pRayAngle->GetWingVecY();
				jgzj = pRayAngle->GetZhunJu('Y');
			}
			pRayRodAxis->ber_space = jgzj.g;
			pRayRodAxis->wing_space = pRayAngle->GetWidth() - pRayRodAxis->ber_space;
			if (pRayRodAxis->start_end == 0)
			{	//始端连接
				pRayRodAxis->dfOrgTipExtend = max(0, pRayAngle->startOdd());
				pRayRodAxis->org = pRayAngle->xPosStart + pRayRodAxis->wing_vec*jgzj.g;
				pRayRodAxis->axis_vec = pRayAngle->xPosEnd - pRayAngle->xPosStart;
			}
			else
			{	//终端连接
				pRayRodAxis->dfOrgTipExtend = max(0, pRayAngle->endOdd());
				pRayRodAxis->org = pRayAngle->xPosEnd + pRayRodAxis->wing_vec*jgzj.g;
				pRayRodAxis->axis_vec = pRayAngle->xPosStart - pRayAngle->xPosEnd;
			}
			normalize(pRayRodAxis->axis_vec);
			//将全部坐标转换到最终执行的新绘图坐标系new_ucs下
			if (pRayRodAxis->iFaceNo <= 1)
			{	//单面板
				pRayRodAxis->org = dcs.TransToDraw(pRayRodAxis->org);
				pRayRodAxis->axis_vec = dcs.TransVToDraw(pRayRodAxis->axis_vec);
				pRayRodAxis->wing_vec = dcs.TransVToDraw(pRayRodAxis->wing_vec);
			}
			else if (pRayRodAxis->iFaceNo > 1)
			{
				coord_trans(pRayRodAxis->org, m_pPlate->ucs, FALSE, TRUE);
				vector_trans(pRayRodAxis->axis_vec, m_pPlate->ucs, FALSE, TRUE);
				vector_trans(pRayRodAxis->wing_vec, m_pPlate->ucs, FALSE, TRUE);
				pRayRodAxis->org = m_pPlate->GetSpreadVertex(pRayRodAxis->org, pRayRodAxis->iFaceNo, true);
				pRayRodAxis->axis_vec = m_pPlate->GetSpreadVertex(pRayRodAxis->axis_vec, pRayRodAxis->iFaceNo, false);
				pRayRodAxis->wing_vec = m_pPlate->GetSpreadVertex(pRayRodAxis->wing_vec, pRayRodAxis->iFaceNo, false);
				//
				coord_trans(pRayRodAxis->org, m_pPlate->ucs, TRUE, TRUE);
				vector_trans(pRayRodAxis->axis_vec, m_pPlate->ucs, TRUE, TRUE);
				vector_trans(pRayRodAxis->wing_vec, m_pPlate->ucs, TRUE, TRUE);
				pRayRodAxis->org = dcs.TransToDraw(pRayRodAxis->org);
				pRayRodAxis->axis_vec = dcs.TransVToDraw(pRayRodAxis->axis_vec);
				pRayRodAxis->wing_vec = dcs.TransVToDraw(pRayRodAxis->wing_vec);
			}
			pRayRodAxis->org.z = pRayRodAxis->axis_vec.z = pRayRodAxis->wing_vec.z = 0;	//z坐标归零
		}
		//识别每根射线杆件涵盖的螺栓线
		for (pRayRodAxis = hashRayRodAxes.GetFirst(); pRayRodAxis; pRayRodAxis = hashRayRodAxes.GetNext())
		{
			if (pRayRodAxis->bDatumRod)
				continue;	//只有射线杆件需要处理，基准杆件此处不处理
			GECS rectCS;
			rectCS.origin = pRayRodAxis->org;
			rectCS.axis_x = pRayRodAxis->axis_vec;
			rectCS.axis_y = -pRayRodAxis->wing_vec;
			rectCS.axis_x.z = rectCS.axis_y.z = 0;
			normalize(rectCS.axis_x);
			rectCS.axis_z = rectCS.axis_x^rectCS.axis_y;
			if (rectCS.axis_z.z > 0)
				rectCS.axis_y = f3dPoint(0, 0, 1) ^ rectCS.axis_x;
			else
				rectCS.axis_y = f3dPoint(0, 0, -1) ^ rectCS.axis_x;
			for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
			{
				//不绘制单排螺栓丁字尺寸时,max_y和min_y不需要设置默认值为0 wht 15-08-12
				CMaxDouble max_y;
				CMinDouble min_x, min_y;
				bool bOuterBolt = false, samelenVec = true;
				GEPOINT rayvec = pLsRay->rayResideVec;
				rayvec = rectCS.TransVToCS(rayvec);
				if (pLsRay->ls_arr.GetSize() > 1)
				{
					rayvec = pLsRay->ls_arr[1].location - pLsRay->ls_arr[0].location;
					normalize(rayvec);
					rayvec = rectCS.TransVToCS(rayvec);
				}
				for (BOLT *pLocate = pLsRay->ls_arr.GetFirst(); pLocate; pLocate = pLsRay->ls_arr.GetNext())
				{
					GEPOINT location = rectCS.TransPToCS(pLocate->location);
					if (location.x <= -pRayRodAxis->dfOrgTipExtend - EPS2)
						bOuterBolt = true;	//pRayRodAxis->dfOrgTipExtend考虑正负头影响
					else if (location.y >= pRayRodAxis->ber_space)
						bOuterBolt = true;	//楞线外侧螺栓
					else if (location.y <= -pRayRodAxis->wing_space)
						bOuterBolt = true;	//肢翼外侧螺栓
					if (fabs(rayvec.y) > EPS2)	//该射线方向与当前射线杆件方向不一致，即使全部螺栓在当前杆件内也不可视为涵盖射线
						bOuterBolt = true;
					if (bOuterBolt)
						break;
					max_y.Update(location.y, pLocate);
					min_y.Update(location.y, pLocate);
					if (min_x.IsInited() && location.x < min_x)
						samelenVec = false;	//pLsRay->ls_arr由远至近，与螺栓射线方向相反
					min_x.Update(location.x);
				}
				if (!bOuterBolt)
				{
					if (pRayRodAxis->ber_dist < max_y)
						pRayRodAxis->ber_dist = max_y;
					if (pRayRodAxis->wing_dist > min_y)
						pRayRodAxis->wing_dist = min_y;
					if (pRayRodAxis->ray_dist > min_x)
						pRayRodAxis->ray_dist = min_x;
					double coord_y = 0;
					if (min_y.m_pRelaObj)
						coord_y = min_y;
					else if (max_y.m_pRelaObj)
						coord_y = max_y;
					BOLTRAY_PARA* pRayPara = hashBelongRodAxies.SetValue(pLsRay->iRayNo, BOLTRAY_PARA(pRayRodAxis->hRod, min_x, coord_y));
					pRayPara->reverseLenvec = !samelenVec;
				}
			}
		}
		//补齐单孔射线
		for (pLsRay = hashBoltRays.GetFirst(); pLsRay; pLsRay = hashBoltRays.GetNext())
		{
			f3dPoint line_vec;
			BOLTRAY_PARA* pRayBelongPara = hashBelongRodAxies.GetValue(pLsRay->iRayNo);
			if (pRayBelongPara)
				pRayRodAxis = hashRayRodAxes.GetValue(pRayBelongPara->hRod);
			else
				pRayRodAxis = NULL;
			if (pRayRodAxis == NULL)
				continue;	//可能该射线属基准角钢螺栓心线
			pLsRay->rayOrg = pRayRodAxis->org;			//用以查找与基准角钢的基点
			pLsRay->rayResideVec = pRayRodAxis->axis_vec;	//用以调整ls_arr顺序
			if (pLsRay->ls_arr.GetSize() == 1)
			{	//补齐单孔射线
				if (pRayRodAxis->start_end < 2 && fabs(pRayBelongPara->coord_y) < 1 && !pLsRay->ls_arr[0].location.IsEqual(pRayRodAxis->org, 1))
				{
					if (bDrawOuterLsRay || PtInPlateRgn(pRayRodAxis->org))
						pLsRay->ls_arr.insert(BOLT(pRayRodAxis->org), 0)->siReserve = -1;	//feature=-1表示该点不需要延伸绘制
				}
			}
			else if (pRayBelongPara->coord_x > pRayRodAxis->ray_dist + 1)	//容许误差预定值为1mm
			{	//补齐多孔射线与同属（归属同一射线杆件）射线螺栓孔间的错孔点
				f3dPoint ray_pick1 = pLsRay->ls_arr[0].location;
				f3dPoint ray_pick2 = pLsRay->ls_arr[1].location;
				f3dPoint insertpos = pRayRodAxis->org + pRayRodAxis->axis_vec*pRayRodAxis->ray_dist;
				SnapPerp(&insertpos, ray_pick1, ray_pick2, insertpos);
				if (pRayBelongPara->reverseLenvec)
					pLsRay->ls_arr.append(BOLT(insertpos))->siReserve = -1;	//feature=-1表示该点不需要延伸绘制
				else
					pLsRay->ls_arr.insert(BOLT(insertpos), 0)->siReserve = -1;	//feature=-1表示该点不需要延伸绘制
			}
			else if (pRayBelongPara->coord_x > 1 && fabs(pRayBelongPara->coord_y) < 1 && pRayRodAxis->start_end < 2)
			{	//单排射线未交于钢板定位原点螺栓的情况，补齐预定位点
				if (!bDrawOuterLsRay && !PtInPlateRgn(pRayRodAxis->org))
					continue;	//pRayRodAxis->org不在钢板内
				if (pRayBelongPara->reverseLenvec)
					pLsRay->ls_arr.append(BOLT(pRayRodAxis->org))->siReserve = -1;	//feature=-1表示该点不需要延伸绘制
				else
					pLsRay->ls_arr.insert(BOLT(pRayRodAxis->org), 0)->siReserve = -1;	//feature=-1表示该点不需要延伸绘制
			}
		}
		hashBoltRays.SetMinmalId(100);	//32以内是钢板上射线杆件形成的实际螺栓射线，之后是辅助射线 wjh-2019.5.18
		for (pRayRodAxis = hashRayRodAxes.GetFirst(); pRayRodAxis; pRayRodAxis = hashRayRodAxes.GetNext())
		{
			if (pRayRodAxis->ray_dist > 1 && fabs(pRayRodAxis->ber_dist - pRayRodAxis->wing_dist) > 1
				&& (bDrawOuterLsRay || PtInPlateRgn(pRayRodAxis->org)))	//pRayRodAxis->org在钢板内
			{
				if (pRayRodAxis->start_end < 2)
				{
					BOLTRAY* pRay= hashBoltRays.Add(0);
					pRay->iRayNo = 0;
					pRay->rayOrg = pRayRodAxis->org;
					pRay->rayResideVec = pRayRodAxis->axis_vec;
					pRay->ls_arr.append(BOLT(pRayRodAxis->org))->siReserve = -1;	//feature=-1表示该点不需要延伸绘制
					pRay->ls_arr.append(BOLT(pRayRodAxis->org + pRayRodAxis->ray_dist*pRayRodAxis->axis_vec))->siReserve = -1;
				}
				GEPOINT ray_pos = pRayRodAxis->org + pRayRodAxis->ray_dist*pRayRodAxis->axis_vec;
				GEPOINT ber_pos = ray_pos - pRayRodAxis->wing_vec*pRayRodAxis->ber_dist;
				GEPOINT wing_pos = ray_pos - pRayRodAxis->wing_vec*pRayRodAxis->wing_dist;
				BOLTRAY* pRay= hashBoltRays.Add(0);
				pRay->iRayNo = 0;
				pRay->rayOrg = ber_pos;
				pRay->ls_arr.append(BOLT(ber_pos))->siReserve = -1;
				if (fabs(pRayRodAxis->ber_dist) >= 1 && fabs(pRayRodAxis->wing_dist) > 1 && fabs(pRayRodAxis->ber_dist - pRayRodAxis->wing_dist) > 1
					&& pRayRodAxis->start_end < 2)
					pRay->ls_arr.append(BOLT(ray_pos))->siReserve = -1;
				pRay->ls_arr.append(BOLT(wing_pos))->siReserve = -1;
				pRay->rayResideVec = pRay->ls_arr[1].location-pRay->ls_arr[0].location;
				normalize(pRay->rayResideVec);
			}
		}
	}
	//查找基准螺栓射线,初始化基准射线信息
	RAYROD_AXIS* pDatumRodAxis = hashRayRodAxes.GetValue(pDatumAngle->handle);
	if (pDatumRodAxis)
	{
		f3dPoint origin, wing_vec, axis_vec;
		origin = pDatumAngle->Start();
		axis_vec = pDatumAngle->End() - pDatumAngle->Start();
		normalize(axis_vec);
		if (pDatumRodAxis->ciWorkWingX0Y1 == 0)
			wing_vec = pDatumAngle->GetWingVecX();
		else
			wing_vec = pDatumAngle->GetWingVecY();
		GECS rectCS;
		rectCS.origin = origin;
		rectCS.axis_x = axis_vec;
		rectCS.axis_y = wing_vec;
		rectCS.axis_z = rectCS.axis_x^rectCS.axis_y;
		for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
		{
			if (fabs(pLsRay->rayResideVec*pDatumRodAxis->axis_vec) < 0.999)
				continue;	//延伸方向不一致
			BOOL bOuterBolt = FALSE;
			for (BOLT* pBoltLoca = pLsRay->ls_arr.GetFirst(); pBoltLoca; pBoltLoca = pLsRay->ls_arr.GetNext())
			{
				GEPOINT location = pBoltLoca->location;
				coord_trans(location, dcs.cs, TRUE);
				coord_trans(location, rectCS, FALSE, TRUE);
				if (location.y <= -EPS2)
					bOuterBolt = TRUE;
				else if (location.y >= pDatumAngle->GetWidth())
					bOuterBolt = TRUE;
				if (bOuterBolt)
					break;
			}
			if (bOuterBolt)
				continue;
			pLsRay->bDatumLsRay = TRUE;
			pLsRay->rayOrg = pDatumRodAxis->org;
			pLsRay->rayResideVec = pDatumRodAxis->axis_vec;
			CLDSGroupLineAngle* pGroupJg = (CLDSGroupLineAngle*)m_pPlate->BelongModel()->FromPartHandle(pDatumAngle->group_father_jg_h);
			if (pGroupJg)
			{	//组合角钢重新计算射线原点
				if (pDatumRodAxis->start_end == 0)
					pLsRay->rayOrg = pGroupJg->Start();
				else
					pLsRay->rayOrg = pGroupJg->End();
				coord_trans(pLsRay->rayOrg, dcs.cs, FALSE, TRUE);
				pLsRay->rayOrg.z = 0;
			}
		}
	}
	CorrectLsRays();	//对螺栓射线信息进行修正
}
BOOL CDimPlateBolts::FindLsRayOrgAndDatumVec(f3dPoint &origin, f3dPoint &datum_vec)
{
	BOLTRAY* pDatumLsRay = NULL,*pLsRay;
	for (pLsRay = hashBoltRays.GetFirst(); pLsRay; pLsRay = hashBoltRays.GetNext())
	{
		if (!pLsRay->bDatumLsRay || pLsRay->ls_arr.GetSize() < 2)
			continue;
		pDatumLsRay = pLsRay;
		break;
	}
	if (pDatumLsRay == NULL)
		return FALSE;
	//查找螺栓射线的基准方向
	if (datum_vec.IsZero())
		datum_vec = pDatumLsRay->rayResideVec;
	f3dPoint ptS = pDatumLsRay->rayOrg;
	f3dPoint ptE = ptS + datum_vec * 100;
	f3dLine datum_line(ptS, ptE);
	SnapPerp(&ptS, datum_line, pDatumLsRay->ls_arr[0].location);
	SnapPerp(&ptE, datum_line, pDatumLsRay->ls_arr[pDatumLsRay->ls_arr.GetSize()-1].location);
	datum_line.startPt = ptS;
	datum_line.endPt = ptE;
	//查找螺栓射线的定位基点
	CXhSimpleList<INTERS_POINT> listIntersPoints;
	for (pLsRay = hashBoltRays.GetFirst(); pLsRay; pLsRay = hashBoltRays.GetNext())
	{
		if (pLsRay->bDatumLsRay)
			continue;
		INTERS_POINT* pItem = NULL;
		for (pItem = listIntersPoints.MoveFirst(); pItem; pItem = listIntersPoints.MoveNext())
		{
			if (pItem->xIntersPoint.IsEqual(pLsRay->rayOrg, EPS2))
			{
				pItem->nNum++;
				break;
			}
		}
		if (pItem == NULL)
		{
			pItem = listIntersPoints.Append();
			pItem->xIntersPoint = pLsRay->rayOrg;
			pItem->nNum = 1;
		}
	}
	int nMax = 0;
	for (INTERS_POINT* pItem = listIntersPoints.MoveFirst(); pItem; pItem = listIntersPoints.MoveNext())
	{
		if (nMax < pItem->nNum&&datum_line.PtInLine(pItem->xIntersPoint, 2) != 0)
		{
			nMax = pItem->nNum;
			origin = pItem->xIntersPoint;
		}
	}
	if (nMax == 0 && hashBoltRays.Count >= 2)
	{	//多根螺栓射线，取第一根与基准射线直接求交计算原点
		for (pLsRay = hashBoltRays.GetFirst(); pLsRay; pLsRay = hashBoltRays.GetNext())
		{
			if (pLsRay->bDatumLsRay)
				continue;
			if (fabs(pLsRay->rayResideVec*datum_vec) < 0.9)
			{
				int iRet = Int3dll(datum_line, f3dLine(pLsRay->rayOrg, pLsRay->rayOrg + pLsRay->rayResideVec), origin);
				if (iRet != 0 && iRet != -1 && datum_line.PtInLine(origin) > 0)
					return TRUE;
			}
		}
	}
	if (nMax == 0)
		return FALSE;
	return TRUE;
}
double WrapLine2dcc(f2dCircle cir1, f2dCircle cir2, f2dLine &wrap_tan_line);
void CDimPlateBolts::DimWrapLsSpace()
{
	f3dPoint ls_pos;
	ATOM_LIST<f2dCircle> ls_cir_list;
	double L = 0;
	if (m_pPlate->GetClassTypeId() == CLS_PARAMPLATE && ((CLDSParamPlate*)m_pPlate)->m_iParamType == TYPE_XEND)
		m_pPlate->GetDesignItemValue('L', &L);
	for (CLsRef *pLsRef = m_pPlate->GetFirstLsRef(); pLsRef; pLsRef = m_pPlate->GetNextLsRef())
	{
		CLDSBolt* pBolt = pLsRef->GetLsPtr();
		int iLsFace = m_pPlate->GetNormFaceI(pBolt->get_norm());
		m_pPlate->SpreadLsPos(pBolt, ls_pos);
		ls_pos.feature = iLsFace + 1;
		if (sys.part_map.bExportDeformedProfile)
			ls_pos = m_pPlate->GetDeformedVertex(ls_pos);
		if (L > 0 && ls_pos.x > L)
		{	//不在十字插板基板上 wht 18-08-09
			continue;
		}
		//
		f2dCircle ls_cir(pBolt->get_d()*0.5, ls_pos.x, ls_pos.y);
		ls_cir.feature = iLsFace + 1;
		ls_cir.pen.style = pBolt->FilterFuncType();
		ls_cir_list.append(ls_cir);
	}
	//查找外轮廓特征螺栓
	//通过最简便的方式找出其中一颗外轮廓特征螺栓,作为外轮廓首螺栓
	BOOL bHasTwoLs = FALSE, bOneAxisLineLs = TRUE;	//所有螺栓都在一条心线上的情况
	int start_space, end_space;
	f3dLine axis_line;
	f2dCircle *pStartLsCir = NULL, *pEndLsCir = NULL;
	for (f2dCircle *pLsCir = ls_cir_list.GetFirst(); pLsCir; pLsCir = ls_cir_list.GetNext())
	{
		if (pStartLsCir == NULL)
		{
			pStartLsCir = pLsCir;
			axis_line.startPt.Set(pLsCir->centre.x, pLsCir->centre.y);
			axis_line.startPt.feature = pLsCir->feature;
			axis_line.endPt = axis_line.startPt;	//添加用于处理单螺栓情况板外形的创建
			axis_line.endPt.feature = pLsCir->feature;
			end_space = start_space = ftoi(pLsCir->radius);
		}
		else
		{
			if (bHasTwoLs)
			{
				if (bOneAxisLineLs)
				{
					f3dPoint centre;
					centre.Set(pLsCir->centre.x, pLsCir->centre.y);
					int ret = axis_line.PtInLine(centre);	//螺栓不在螺栓射线上
					if (ret == -2)			//起点外侧
					{
						axis_line.startPt = centre;
						axis_line.startPt.feature = pLsCir->feature;
						start_space = ftoi(pLsCir->radius);
					}
					else if (ret == -1)	//终点外侧
					{
						axis_line.endPt = centre;
						axis_line.endPt.feature = pLsCir->feature;
						end_space = ftoi(pLsCir->radius);
					}
					if (ret == 0)
						bOneAxisLineLs = FALSE;
				}
			}
			else
			{
				bHasTwoLs = TRUE;
				axis_line.endPt.Set(pLsCir->centre.x, pLsCir->centre.y);
				axis_line.endPt.feature = pLsCir->feature;
				end_space = ftoi(pLsCir->radius);
			}
			if (pStartLsCir->centre.y > pLsCir->centre.y)
				pStartLsCir = pLsCir;		//查获最低处的螺栓
		}
	}
	if (bOneAxisLineLs)
		return;		//所有螺栓都在同一直线上
	ATOM_LIST<f2dCircle> out_ls_list;
	out_ls_list.append(*pStartLsCir);
	f2dCircle *pFirstStartLsCir = pStartLsCir;
	int nLoops = 0;
	f2dLine tan_line;
	for (;;)
	{
		if (nLoops == ls_cir_list.GetNodeNum())
			break;
		nLoops++;
		for (pEndLsCir = ls_cir_list.GetFirst(); pEndLsCir; pEndLsCir = ls_cir_list.GetNext())
		{
			if (pEndLsCir == pStartLsCir)
				continue;
			BOOL bOutLs = TRUE;
			ls_cir_list.push_stack();
			//由点在线左侧算法改为直接求包络公切线夹角算法,这样
			//可以避免圆直径不同的影响 WJH-2004.09.22
			WrapLine2dcc(*pStartLsCir, *pEndLsCir, tan_line);
			for (pLsCir = ls_cir_list.GetFirst(); pLsCir; pLsCir = ls_cir_list.GetNext())
			{
				if (pLsCir == pStartLsCir || pLsCir == pEndLsCir)
					continue;
				//由点在线左侧算法改为直接求包络公切线夹角算法,这样可以避免圆直径不同的影响 WJH-2004.09.22
				double dd = DistOf2dPtLine(pLsCir->centre, tan_line.startPt, tan_line.endPt);
				if (dd < pLsCir->radius - eps)	//终端螺栓不是边缘螺栓
				{
					bOutLs = FALSE;
					break;
				}
				else if (dd > pLsCir->radius - eps && dd < pLsCir->radius + eps)
				{	//清除掉共线螺栓
					f2dPoint perp;
					SnapPerp(&perp, tan_line, pLsCir->centre);
					f3dPoint line_vec(tan_line.endPt.x - tan_line.startPt.x, tan_line.endPt.y - tan_line.startPt.y);
					f3dPoint vec1(pLsCir->centre.x - tan_line.startPt.x, pLsCir->centre.y - tan_line.startPt.y);
					f3dPoint vec2(pLsCir->centre.x - tan_line.endPt.x, pLsCir->centre.y - tan_line.endPt.y);
					if (line_vec*vec1 > 0 && line_vec*vec2 > 0)
					{	//pLsCir为pEndLsCir的下一个共线螺栓,所以pEndLsCir不为外轮廓特征螺栓
						bOutLs = FALSE;
						break;
					}
				}
			}
			ls_cir_list.pop_stack();
			if (bOutLs)		//终端螺栓为边缘螺栓
				break;
		}
		if (pEndLsCir == pFirstStartLsCir)	//末螺栓为第一颗外轮廓螺栓时终止(证明已找了一周)
			break;
		else if (pEndLsCir)				//找到下一颗外轮廓螺栓
		{
			out_ls_list.append(*pEndLsCir);
			pStartLsCir = pEndLsCir;
		}
		else							//当前螺栓不是下一颗外轮廓螺栓
			continue;
	}
	//标注轮廓特征螺栓之间的螺栓距离
	ROTATEDIM_STRU rotate_dim;
	if(m_bDimFeatureLsSpace)
	{	
		f3dPoint f_start, f_end;
		int nNum = out_ls_list.GetNodeNum();
		for (int i = 0; i < nNum; i++)
		{
			f_start.Set(out_ls_list[i].centre.x, out_ls_list[i].centre.y, 0);
			f_end.Set(out_ls_list[(i + 1) % nNum].centre.x, out_ls_list[(i + 1) % nNum].centre.y, 0);
			coord_trans(f_start, m_pPlate->ucs, TRUE);
			coord_trans(f_start, dcs.cs, FALSE);
			coord_trans(f_end, m_pPlate->ucs, TRUE);
			coord_trans(f_end, dcs.cs, FALSE);
			if (FindLsRay(&f_start, &f_end) != NULL)
				continue;	//两特征螺栓在同一个螺栓射线上，不再进行标注
			//
			rotate_dim.startPos = f_start;
			rotate_dim.endPos = f_end;
			rotate_dim.dimPos = 0.5*(f_start + f_end);
			rotate_dim.dist = DISTANCE(rotate_dim.startPos, rotate_dim.endPos);
			rotate_dim.angle = Cal2dLineAng(0, 0, rotate_dim.endPos.x - rotate_dim.startPos.x, rotate_dim.endPos.y - rotate_dim.startPos.y);
			rotate_dim.bDisplayDimLine = TRUE;
			m_xRotateDimList.AttachObject(rotate_dim);
		}
	}
	//标注轮廓特征螺栓到轮廓边距离
	if (m_bDimSpaceToEdgeLine)
	{	
		//获取绘图坐标下的轮廓点
		int nVertex = m_pPlate->vertex_list.GetNodeNum();
		ATOM_LIST<f3dPoint> vertexArr;
		for (int i = 0; i < nVertex; i++)
		{
			f3dPoint vertex = m_pPlate->vertex_list[i].vertex;
			coord_trans(vertex, m_pPlate->ucs, TRUE);
			coord_trans(vertex, dcs.cs, FALSE);
			if (sys.part_map.bExportDeformedProfile)
				vertex = m_pPlate->GetDeformedVertex(vertex, dcs.cs);
			vertex.z = 0;
			vertex.pen.style = m_pPlate->vertex_list[i].type;
			if (m_pPlate->vertex_list[i].m_bRollEdge)
				vertex.pen.style = -1;
			vertexArr.append(vertex);
		}
		//标注特征螺栓到轮廓边距离
		BOLTRAY* pLsRay = NULL;
		for (int i = 0; i < out_ls_list.GetNodeNum(); i++)
		{
			f3dPoint startPt, endPt, vertex, intersPt;
			startPt.Set(out_ls_list[i].centre.x, out_ls_list[i].centre.y, 0);
			coord_trans(startPt, m_pPlate->ucs, TRUE);
			coord_trans(startPt, dcs.cs, FALSE);
			startPt.z = 0;
			int iFaceNo = out_ls_list[i].feature;
			if (out_ls_list[i].pen.style > 1)
			{	//特殊功用螺栓孔
				double fDist = 0, fMinDist = 500; //特殊功用孔边距最大值暂取500
				for (int j = 0; j < nVertex; j++)
				{
					f3dPoint f_start = vertexArr[j];
					f3dPoint f_end = vertexArr[(j + 1) % nVertex];
					if (f_start.pen.style == -1 || (iFaceNo != f_start.feature&&iFaceNo != f_end.feature))
						continue;
					f3dLine edge_line(f_start, f_end);
					SnapPerp(&intersPt, edge_line, startPt, &fDist);
					if (edge_line.PtInLine(intersPt) != 2)
						continue;
					if (f_start.pen.style > 0 && fDist < fMinDist)
					{	//垂足在圆弧处,通过圆弧半径标注已处理
						vertex.Set();
						fMinDist = fDist;
					}
					else if (f_start.pen.style == 0 && fDist < fMinDist)
					{	//垂足在线段端内,取该垂点进行标注
						vertex = intersPt;
						fMinDist = fDist;
					}
				}
			}
			else if ((pLsRay = FindLsRay(&startPt, NULL)))
			{	//螺栓射线的螺栓孔
				double fDist = 0, fMinDist = 100; //连接螺栓边距最大值暂取100
				f3dPoint ray_vec = pLsRay->rayResideVec;
				if (ray_vec.IsZero() && pLsRay->ls_arr.GetSize() > 1)
				{
					ray_vec = (pLsRay->ls_arr[1].location - pLsRay->ls_arr[0].location);
					normalize(ray_vec);
				}
				if (startPt.IsEqual(pLsRay->ls_arr[0].location, EPS2))
					ray_vec *= -1;
				endPt = startPt + ray_vec * 300;
				for (int j = 0; j < nVertex; j++)
				{
					if (vertexArr[j].pen.style != 0)
						continue;
					if (Int3dll(vertexArr[j], vertexArr[(j + 1) % nVertex], startPt, endPt, intersPt) != 1)
						continue;
					fDist = DISTANCE(startPt, intersPt);
					if (fDist < fMinDist)
					{
						fMinDist = fDist;
						vertex = intersPt;
					}
				}
			}
			if (!vertex.IsZero())
			{
				rotate_dim.startPos = startPt;
				rotate_dim.endPos = vertex;
				rotate_dim.dimPos = 0.5*(rotate_dim.startPos + rotate_dim.endPos);
				rotate_dim.dist = DISTANCE(rotate_dim.startPos, rotate_dim.endPos);
				rotate_dim.angle = Cal2dLineAng(0, 0, rotate_dim.endPos.x - rotate_dim.startPos.x, rotate_dim.endPos.y - rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine = TRUE;
				m_xRotateDimList.AttachObject(rotate_dim);
			}
		}
		//处理基准螺栓射线到平行轮廓边的距离
		f3dPoint datum_vec;
		ATOM_LIST<f3dPoint> datumLsRayPtArr;
		for(pLsRay=hashBoltRays.GetFirst();pLsRay;pLsRay=hashBoltRays.GetNext())
		{
			if (!pLsRay->bDatumLsRay)
				continue;
			if (datum_vec.IsZero())
				datum_vec = pLsRay->rayResideVec;
			datumLsRayPtArr.append(pLsRay->ls_arr[0].location);
		}
		for (int i = 0; i < nVertex; i++)
		{
			f3dLine edge_line(vertexArr[i], vertexArr[(i + 1) % nVertex]);
			f3dPoint line_vec = (edge_line.endPt - edge_line.startPt).normalized();
			if (vertexArr[i].pen.style == 0 && fabs(line_vec*datum_vec) > 0.9999)
			{	//与基准螺栓射线平行的轮廓边
				double fMinDist = 100, fDist = 0;
				f3dPoint vertex, intersPt;
				int iSel = -1;
				for (int j = 0; j < datumLsRayPtArr.GetNodeNum(); j++)
				{
					SnapPerp(&intersPt, edge_line, datumLsRayPtArr[j], &fDist);
					if (edge_line.PtInLine(intersPt) != 2)
						continue;
					if (fDist < fMinDist)
					{
						iSel = j;
						fMinDist = fDist;
						vertex = intersPt;
					}
				}
				if (!vertex.IsZero())
				{
					rotate_dim.startPos = datumLsRayPtArr[iSel];
					rotate_dim.endPos = vertex;
					rotate_dim.dimPos = 0.5*(rotate_dim.startPos + rotate_dim.endPos);
					rotate_dim.dist = DISTANCE(rotate_dim.startPos, rotate_dim.endPos);
					rotate_dim.angle = Cal2dLineAng(0, 0, rotate_dim.endPos.x - rotate_dim.startPos.x, rotate_dim.endPos.y - rotate_dim.startPos.y);
					rotate_dim.bDisplayDimLine = TRUE;
					m_xRotateDimList.AttachObject(rotate_dim);
				}
				break;
			}
		}
	}
}
BOOL CDimPlateBolts::CreateEx(CLDSGeneralPlate* pPlate, CDrawCoordTrans cs)
{
	if (pPlate == NULL || pPlate->GetLsCount() <= 1)
		return FALSE;
	m_pPlate = pPlate;
	dcs = cs;
	//如果绘图坐标系与钢板坐标系有夹角，需要通过钢板坐标系来校准螺栓间距
	m_pAdjustCs = NULL;
	if (fabs(dcs.PrjNorm()*pPlate->ucs.axis_z)< 0.999)
		m_pAdjustCs = &pPlate->ucs;
	//提取螺栓信息，分析螺栓射线
	f3dCircle ls_cir;
	for (CLsRef *pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
	{
		CLDSBolt* pBolt = pLsRef->GetLsPtr();
		pPlate->GetBoltIntersPos(pBolt, ls_cir.centre);
		ls_cir.centre = dcs.TransToDraw(ls_cir.centre);
		ls_cir.radius = pBolt->get_d()*0.5;
		ls_cir.feature = (int)(pBolt->hole_d_increment * 10);
		ls_cir.pen.style = pBolt->FilterFuncType();		//记录螺栓孔的功用类型
		ls_cir.centre.pen.style = pLsRef->cFlag;		//记录螺栓孔的工艺类型
		if ((int)pLsRef->waistLen > 0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width = (int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm = pBolt->ucs.axis_x;				//使用螺栓孔法线方向保存腰圆方向
			ls_cir.norm = dcs.TransVToDraw(ls_cir.norm);	//转换到绘图坐标系下
		}
		m_xLsCirArr.append(ls_cir);
		//标注坐标时也有可能要分析螺栓射线
		for (int i = 1; i <= 32; i++)
		{
			if (pLsRef->dwRayNo&GetSingleWord(i))
				AnalyzeBoltsRayline(ls_cir.centre, i, pLsRef);
		}
	}
	//根据钢板的连接设计信息完善螺栓射线信息
	CompleteLsRayInfo();
	//标注螺栓射线上的螺栓间距
	ROTATEDIM_STRU rotate_dim;
	if (m_bDimRayLsSpace)
	{	//之前该功能选漏启用了,今日加上 wjh-2019.12.13
		for (BOLTRAY *pLsRay = hashBoltRays.GetFirst(); pLsRay; pLsRay = hashBoltRays.GetNext())
		{
			if (pLsRay->ls_arr.GetSize() < 2)
				continue;
			for (int j = 0; j < pLsRay->ls_arr.GetSize() - 1; j++)
			{
				rotate_dim.startPos = pLsRay->ls_arr[j].location;
				rotate_dim.endPos = pLsRay->ls_arr[j + 1].location;
				rotate_dim.angle = Cal2dLineAng(0,0,rotate_dim.endPos.x - rotate_dim.startPos.x,rotate_dim.endPos.y - rotate_dim.startPos.y);
				rotate_dim.dist = DISTANCE(rotate_dim.startPos,rotate_dim.endPos);	//输出时控制精度 wht 15-07-13
				if (m_pAdjustCs)
				{	//根据校准坐标系下的螺栓点坐标，计算螺栓间距
					f3dPoint ptS = pLsRay->ls_arr[j].adjustPt;
					f3dPoint ptE = pLsRay->ls_arr[j + 1].adjustPt;
					rotate_dim.dist = DISTANCE(ptS,ptE);
				}
				Sub_Pnt(rotate_dim.dimPos,rotate_dim.endPos,rotate_dim.startPos);
				Add_Pnt(rotate_dim.dimPos,rotate_dim.startPos,rotate_dim.dimPos*0.5);
				m_xRotateDimList.AttachObject(rotate_dim);
			}
		}
	}
	//螺栓射线角度的标注
	f3dPoint orgin, datum_vec;
	BOOL bValid = FindLsRayOrgAndDatumVec(orgin, datum_vec);
	if (m_bDimLsRayAngle && bValid)
	{
		ARRAY_LIST<RAYROD_AXIS> rayAxisArr;
		rayAxisArr.SetSize(0, hashBoltRays.Count);
		f3dPoint datum_ls_pos, intersPt;
		int nLeft = 0, nRight = 0;
		for (BOLTRAY *pLsRay = hashBoltRays.GetFirst(); pLsRay; pLsRay = hashBoltRays.GetNext())
		{
			f3dLine ray_line(pLsRay->rayOrg, pLsRay->rayOrg + pLsRay->rayResideVec * 100);
			if (ray_line.PtInLine(orgin, 2) == 0)
				continue;	//标注原点不在此射线轴
			RAYROD_AXIS* pRayAxis = NULL;
			for (pRayAxis = rayAxisArr.GetFirst(); pRayAxis; pRayAxis = rayAxisArr.GetNext())
			{
				if (pLsRay->rayResideVec*pRayAxis->axis_vec > 0.9999)
					break;	//已存在此射线轴
			}
			if (pRayAxis == NULL)
			{
				pRayAxis = rayAxisArr.append();
				pRayAxis->axis_vec = pLsRay->rayResideVec;
				pRayAxis->fAngle = CalRotateAngle(f3dPoint(0, 0, 1), datum_vec, pRayAxis->axis_vec);
				pRayAxis->fRadius = DISTANCE(orgin, pLsRay->ls_arr[pLsRay->ls_arr.GetSize()-1].location)*0.8;
				if (pLsRay->bDatumLsRay)
				{	//基准射线需要添加反向射线
					pRayAxis = rayAxisArr.append();
					pRayAxis->axis_vec = datum_vec * (-1);
					pRayAxis->fAngle = CalRotateAngle(f3dPoint(0, 0, 1), datum_vec, pRayAxis->axis_vec);
					pRayAxis->fRadius = DISTANCE(orgin, pLsRay->ls_arr[0].location)*0.8;
					if (datum_ls_pos.IsZero())
						SnapPerp(&datum_ls_pos, ray_line, pLsRay->ls_arr[0].location);
				}
				else
				{	//记录其他射线的分布
					if (fabs(pRayAxis->fAngle) < 0.01)
						continue;
					else if (pRayAxis->fAngle > 0.01)
						nLeft++;
					else if (pRayAxis->fAngle < -0.01)
						nRight++;
				}
			}
		}
		CQuickSort<RAYROD_AXIS>::QuickSort(rayAxisArr.m_pData, rayAxisArr.GetSize(), compare_func1);
		//计算标注角度并对角度进行排序
		int nn = rayAxisArr.GetSize();
		ANGULARDIM_STRU angular_dim;
		ARRAY_LIST<ANGULARDIM_STRU> dimAngleArr;
		dimAngleArr.SetSize(0, nn);
		for (int i = 0; i < nn; i++)
		{
			angular_dim.vertex = orgin;
			angular_dim.startPos = rayAxisArr[i].axis_vec;
			angular_dim.endPos = rayAxisArr[(i + 1) % nn].axis_vec;
			angular_dim.fRadius = min(rayAxisArr[i].fRadius, rayAxisArr[(i + 1) % nn].fRadius);
			angular_dim.angle = CalRotateAngle(f3dPoint(0, 0, 1), angular_dim.startPos, angular_dim.endPos);
			if (angular_dim.angle < 0)
				angular_dim.angle += 2 * Pi;
			dimAngleArr.append(angular_dim);
		}
		CQuickSort<ANGULARDIM_STRU>::QuickSort(dimAngleArr.m_pData, dimAngleArr.GetSize(), compare_func2);
		//添加角度标注
		double fMinR = 40;
		double fRadius = fMinR;
		f3dLine edge_line;
		if (!PtInPlateRgn(orgin))
		{	//标注原点在板轮廓外,考虑原点到最近轮廓边的距离
			int nVertex = pPlate->vertex_list.GetNodeNum();
			for (int j = 0; j < nVertex; j++)
			{
				if (pPlate->vertex_list[j].type != 0 || pPlate->vertex_list[j].m_bRollEdge)
					continue;
				f3dPoint f_start = pPlate->vertex_list[j].vertex;
				f3dPoint f_end = pPlate->vertex_list[(j + 1) % nVertex].vertex;
				coord_trans(f_start, pPlate->ucs, TRUE);
				coord_trans(f_end, pPlate->ucs, TRUE);
				coord_trans(f_start, dcs.cs, FALSE);
				coord_trans(f_end, dcs.cs, FALSE);
				if (sys.part_map.bExportDeformedProfile)
				{
					f_start = pPlate->GetDeformedVertex(f_start, dcs.cs);
					f_end = pPlate->GetDeformedVertex(f_end, dcs.cs);
				}
				if (Int3dll(f_start, f_end, orgin, datum_ls_pos, intersPt) == 1)
				{
					fRadius = fMinR + DISTANCE(orgin, intersPt);
					edge_line.startPt = f_start;
					edge_line.endPt = f_end;
					break;
				}
			}
		}
		int nSize = (nLeft > 0 && nRight > 0) ? dimAngleArr.GetSize() - 1 : dimAngleArr.GetSize() - 2;
		for (int i = 0; i < nSize; i++)
		{
			double fR = max(dimAngleArr[i].fRadius, fRadius);
			f3dPoint dimStart = orgin + dimAngleArr[i].startPos*fR;
			f3dPoint dimEnd = orgin + dimAngleArr[i].endPos*fR;
			if (fRadius > fMinR)
			{	//角度标注的辅助线
				Int3dll(edge_line, f3dLine(orgin, dimStart), intersPt);
				m_xAssLsRayArr.append(f3dLine(intersPt, dimStart));
				Int3dll(edge_line, f3dLine(orgin, dimEnd), intersPt);
				m_xAssLsRayArr.append(f3dLine(intersPt, dimEnd));
			}
			//
			angular_dim.vertex = orgin;
			angular_dim.startPos = dimStart;
			angular_dim.endPos = dimEnd;
			angular_dim.dimPos = 0.5*(dimStart + dimEnd);
			m_xAngularDimList.AttachObject(angular_dim);
		}
	}
	//标注距离焊接边最近的螺栓孔到焊接边的最小垂直距离
	if (m_bDimSpaceToWeldEdge)
	{	
		PROFILE_VERTEX* pPrevVertex = pPlate->vertex_list.GetTail();
		for (PROFILE_VERTEX* pVertex = pPlate->vertex_list.GetFirst(); pVertex; pVertex = pPlate->vertex_list.GetNext())
		{
			if (!pPrevVertex->m_bWeldEdge)
			{
				pPrevVertex = pVertex;
				continue;
			}
			CLsRef* pLsRef;
			if (pPrevVertex->type == 0)	//直焊缝边
			{
				GEPOINT pos;
				CMinDouble mindist;
				for (pLsRef = pPlate->GetFirstLsRef(); pLsRef; pLsRef = pPlate->GetNextLsRef())
				{
					if (!pPlate->SpreadLsPos(pLsRef->GetLsPtr(), pos))
						continue;
					double scaleInLine = 0;
					double distance = DistOf2dPtLine(pos.x, pos.y, pPrevVertex->vertex.x, pPrevVertex->vertex.y,
						pVertex->vertex.x, pVertex->vertex.y, &scaleInLine);
					if (scaleInLine > -EPS && scaleInLine < 1 + EPS)
						mindist.Update(distance, pLsRef->GetLsPtr(), EPS2);
				}
				if (!mindist.IsInited())
				{
					pPrevVertex = pVertex;
					continue;	//可能本来就没有螺栓孔
				}
				CLDSBolt* pNearestBolt = (CLDSBolt*)mindist.m_pRelaObj;
				pPlate->SpreadLsPos(pNearestBolt, pos);
				f3dPoint perp;
				double nearest = mindist.number;
				SnapPerp(&perp, pPrevVertex->vertex, pVertex->vertex, pos, &nearest);
				coord_trans(pos, pPlate->ucs, TRUE, TRUE);
				coord_trans(perp, pPlate->ucs, TRUE, TRUE);
				coord_trans(pos, dcs.cs, FALSE, TRUE);
				coord_trans(perp, dcs.cs, FALSE, TRUE);
				rotate_dim.startPos = pos;
				rotate_dim.endPos = perp;
				rotate_dim.dimPos = 0.5*(rotate_dim.startPos + rotate_dim.endPos);
				rotate_dim.dist = nearest;
				rotate_dim.angle = Cal2dLineAng(0, 0, rotate_dim.endPos.x - rotate_dim.startPos.x, rotate_dim.endPos.y - rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine = TRUE;
				m_xRotateDimList.AttachObject(rotate_dim);
				pPrevVertex = pVertex;
			}
			else
				pPrevVertex = pVertex;
		}
	}
	//标注与火曲线平行的螺栓射线间距
	if (m_bDimSpaceToHuoQuLine && pPlate->GetFaceN() > 1)
	{
		PROFILE_VERTEX* pBendLine1Start = NULL, *pBendLine1End = NULL;
		PROFILE_VERTEX* pBendLine2Start = NULL, *pBendLine2End = NULL;
		pPlate->GetBendlineVertexies(&pBendLine1Start, &pBendLine1End, &pBendLine2Start, &pBendLine2End);
		for (int m = 1; m < pPlate->GetFaceN(); m++)
		{
			f3dPoint huoqu_start, huoqu_end;
			if (m == 1 && pBendLine1Start&&pBendLine1End)
			{
				huoqu_start = pBendLine1Start->vertex;
				huoqu_end = pBendLine1End->vertex;
			}
			if (m == 1 && pBendLine2Start&&pBendLine2End)
			{
				huoqu_start = pBendLine2Start->vertex;
				huoqu_end = pBendLine2End->vertex;
			}
			if (huoqu_start.IsZero() && huoqu_end.IsZero())
				continue;
			if (sys.part_map.bExportDeformedProfile)
			{
				huoqu_start = pPlate->GetDeformedVertex(huoqu_start);
				huoqu_end = pPlate->GetDeformedVertex(huoqu_end);
			}
			coord_trans(huoqu_start, pPlate->ucs, TRUE, TRUE);
			coord_trans(huoqu_start, dcs.cs, FALSE, TRUE);
			coord_trans(huoqu_end, pPlate->ucs, TRUE, TRUE);
			coord_trans(huoqu_end, dcs.cs, FALSE, TRUE);
			f3dPoint huoqu_vec = huoqu_end - huoqu_start;
			normalize(huoqu_vec);
			//螺栓射线到火曲线的距离标注
			BOLTRAY* pFinalRay = NULL;
			double fMinDist = 1000;
			for (BOLTRAY *pLsRay = hashBoltRays.GetFirst(); pLsRay; pLsRay = hashBoltRays.GetNext())
			{
				if (fabs(huoqu_vec*pLsRay->rayResideVec) < EPS_COS2)
					continue;
				double dist = DistOf2dPtLine(pLsRay->ls_arr[0].location, huoqu_start, huoqu_end);
				if (fMinDist > fabs(dist))
				{
					fMinDist = fabs(dist);
					pFinalRay = pLsRay;
				}
			}
			if (pFinalRay)
			{
				int n = pFinalRay->ls_arr.GetSize();
				rotate_dim.startPos = 0.5*(pFinalRay->ls_arr[0].location + pFinalRay->ls_arr[n-1].location);
				SnapPerp(&rotate_dim.endPos, huoqu_start, huoqu_end, rotate_dim.startPos);
				rotate_dim.dimPos = 0.5*(rotate_dim.startPos + rotate_dim.endPos);
				rotate_dim.dist = fMinDist;
				rotate_dim.angle = Cal2dLineAng(0, 0, rotate_dim.endPos.x - rotate_dim.startPos.x, rotate_dim.endPos.y - rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine = TRUE;
				m_xRotateDimList.AttachObject(rotate_dim);
			}
			//挂线孔到火曲线的距离标注
			f3dLine line;
			ATOM_LIST<BOLTRAY> xGuaXianLsRay;
			for (int i = 0; i < m_xLsCirArr.GetSize(); i++)
			{
				if (m_xLsCirArr[i].pen.style <= 1)
					continue;	//连接螺栓或脚钉不处理	
				f3dPoint intersPt, ls_centre = m_xLsCirArr[i].centre;
				SnapPerp(&intersPt, huoqu_start, huoqu_end, ls_centre);
				double fDist = DISTANCE(ls_centre, intersPt);
				BOLTRAY* pLsRay = NULL;
				for (pLsRay = xGuaXianLsRay.GetFirst(); pLsRay; pLsRay = xGuaXianLsRay.GetNext())
				{
					if (pLsRay->iRayNo == ftoi(fDist))
						break;
				}
				if (pLsRay)
					pLsRay->AppendBoltLoca(BOLT(ls_centre));
				else
				{
					pLsRay = xGuaXianLsRay.append();
					pLsRay->rayOrg = ls_centre;
					pLsRay->ls_arr.append(BOLT(ls_centre));
					pLsRay->featrue = ftoi(fDist);
				}
			}
			for (BOLTRAY* pLsRay = xGuaXianLsRay.GetFirst(); pLsRay; pLsRay = xGuaXianLsRay.GetNext())
			{
				rotate_dim.startPos = pLsRay->ls_arr[0].location;
				if (pLsRay->ls_arr.GetSize() > 1)
				{	//多个挂线孔在一排时，绘制辅助线
					line.startPt = pLsRay->ls_arr[0].location;
					line.endPt = pLsRay->ls_arr[pLsRay->ls_arr.GetSize()-1].location;
					m_xAssLsRayArr.append(line);
					//调整标注原点
					f3dPoint vec = pLsRay->ls_arr[1].location - pLsRay->ls_arr[0].location;
					double fLen = vec.mod();
					vec.normalized();
					rotate_dim.startPos += vec*0.3*fLen;
				}
				SnapPerp(&rotate_dim.endPos, huoqu_start, huoqu_end, rotate_dim.startPos);
				rotate_dim.dimPos = 0.5*(rotate_dim.startPos + rotate_dim.endPos);
				rotate_dim.dist = DISTANCE(rotate_dim.startPos, rotate_dim.endPos);
				rotate_dim.angle = Cal2dLineAng(0, 0, rotate_dim.endPos.x - rotate_dim.startPos.x, rotate_dim.endPos.y - rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine = TRUE;
				m_xRotateDimList.AttachObject(rotate_dim);
			}
		}
	}
	//标注轮廓特征螺栓
	if (m_bDimSpaceToEdgeLine || m_bDimFeatureLsSpace)
		DimWrapLsSpace();
	return TRUE;
}
#endif