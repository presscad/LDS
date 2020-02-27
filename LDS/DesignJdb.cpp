 //<LOCALE_TRANSLATE Confirm by hxr/>
// DesignJdb.cpp: implementation of the CDesignJdb class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LDS.h"
#include "DesignJdb.h"
#include "LjbDesignDlg.h"
#include "LayAngleBoltDlg.h"
#include "DesignLjParaDlg.h"
#include "Query.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "dlg.h"
#include "PromptDlg.h"
#include "ScrTools.h"
#include "GSMH.h"
#include "NetDogAPI.h"
#include "f_alg_fun.h"
#include "f_ent.h"
#include "LDSView.h"
#include "PlateBasicParaDlg.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"
#include "DesignTubeEndLjDlg.h"
#include "LicFuncDef.h"
//#include "Lic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//根据规程2.6.4
int GetDeita23ByLsDiameter(int diameter)
{
	if(diameter<=12)
		return 15;
	else if(diameter<=16)//M20
		return 20;
	else if(diameter<=20)
		return 25;
	else	//>=M24
		return 30;
}
CDesignJdb::CDesignJdb()
{
	m_pPlank=NULL;
	m_nViewFlag = FRONT_VIEW;
	for(int i=0;i<2;i++)
	{
		design_huoqu[i].bOffsetWingX = TRUE;
		design_huoqu[i].offset_dist = 5;
		design_huoqu[i].pBaseJg = NULL;
	}
		//连接板设计参数
	xDesLjbPara.cMaterial = CSteelMatLibrary::Q235BriefMark();//'S';
	xDesLjbPara.bHuoQuPosByCtrlPt = FALSE;
	xDesLjbPara.iRayIntersStyle = 0;
	xDesLjbPara.nHuoQuDist[0] = xDesLjbPara.nHuoQuDist[1] = g_sysPara.HuoQuDist;
	xDesLjbPara.nOffsetZhun[0] = xDesLjbPara.nOffsetZhun[1] = 0;
	strcpy(xDesLjbPara.sPartNo,"");
	xDesLjbPara.plank_thick = 6;
	xDesLjbPara.fNormOffset=0;
	xDesLjbPara.fInsertPlateEdgeSpace=g_sysPara.fInsertPlateEdgeSpace;
	m_bAdjustWeldLen=FALSE;	//默认不调整同一节点上所有连接板的焊缝长度一致
	m_bRayEndFaceOffset=FALSE;
	m_bTerminateByUser=FALSE;
	m_cPlateType=TYPE_COMMON_PLATE;
}

//装配定位杆件为钢管时,连板与定位杆件间需要焊接,调整同一节点上的焊接连接板焊缝线长度一致
BOOL CDesignJdb::AdjustPlateWeldLenOnSameNode(CLDSLinePart *pBaseLinePart,CLDSNode *pBaseNode)
{
	if(pBaseLinePart==NULL||pBaseNode==NULL||!pBaseLinePart->IsTube())
		return FALSE;
	CLDSModule* pModule=Ta.GetActiveModule();
	PARTSET plateset;	
	CLDSPart *pPart;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		if(((CLDSPlate*)pPart)->vertex_list.GetNodeNum()<3)
			continue;	//非法钢板,不能用来调整焊缝长度 wht 12-09-29
		if(((CLDSPlate*)pPart)->designInfo.m_hBaseNode==pBaseNode->handle &&
			((CLDSPlate*)pPart)->designInfo.m_hBasePart==pBaseLinePart->handle)
			plateset.append(pPart);
	}
	if(plateset.GetNodeNum()>1)
	{
		f3dLine line(pBaseLinePart->Start(),pBaseLinePart->End());
		f3dPoint origin,perp,len_vec=line.endPt-line.startPt;
		Standarized(len_vec);
		SnapPerp(&origin,line.startPt,line.endPt,pBaseNode->Position(true));
		double up_len_dist,down_len_dist;
		BOOL bFirst=TRUE;
		for(pPart=plateset.GetFirst();pPart;pPart=plateset.GetNext())
		{
			f3dPoint vertice;
			CLDSPlate *pTmPlate=(CLDSPlate*)pPart;
			for(PROFILE_VERTEX *pVertex=pTmPlate->vertex_list.GetFirst();pVertex;pVertex=pTmPlate->vertex_list.GetNext())
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pTmPlate->ucs,TRUE);
				SnapPerp(&perp,line.startPt,line.endPt,vertice);
				f3dPoint vec=perp-origin;
				double dd=vec*len_vec;
				if(bFirst)
				{
					up_len_dist=down_len_dist=dd;
					bFirst=FALSE;
				}
				else if(dd>down_len_dist)
					down_len_dist=dd;
				else if(dd<up_len_dist)
					up_len_dist=dd;
			}
		}
		for(pPart=plateset.GetFirst();pPart;pPart=plateset.GetNext())
		{
			CLDSPlate *pTmPlate=(CLDSPlate*)pPart;
			pTmPlate->designInfo.iWeldLineStyle=1;
			//if(pTmPlate->ucs.axis_y*len_vec>0)
			//{
			pTmPlate->designInfo.weld_start_high=up_len_dist;
			pTmPlate->designInfo.weld_end_high=down_len_dist;
			//}
			/*else
			{
			pTmPlate->designInfo.weld_start_high=-up_len_dist;
			pTmPlate->designInfo.weld_end_high=-down_len_dist;
			}*/
			pTmPlate->DesignPlate();
			if(pModule&&pModule->IsSonPart(pTmPlate))
			{	//防止塔腿八字铁处各接腿的连接板临时干扰显示 wjh-2014.12.31
				pTmPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pTmPlate->GetSolidPartObject());
			}
		}
	}
	else
	{
		CLDSPlate *pPlate=(CLDSPlate*)plateset.GetFirst();
		if(pPlate&&pModule&&pModule->IsSonPart(pPlate))
		{
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
	}
	return TRUE;
}

//根据组合角钢中的一根子角钢及当前工作面法线，查找对应的背靠背角钢 wht 16-07-12
static CLDSLineAngle* FindSonJgBackToBackJg(CLDSLineAngle *pLineAngle,GEPOINT plate_norm,int *wing_x0_y1=NULL)
{
	if(pLineAngle->group_father_jg_h<0x20)
		return NULL;
	CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)console.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
	if(pGroupAngle==NULL)
		return NULL;
	CLDSLineAngle *pBackAngleInSrcAngle=NULL;
	int work_wing_x0_y1=fabs(pLineAngle->vxWingNorm*plate_norm)>fabs(pLineAngle->vyWingNorm*plate_norm)?0:1;
	GEPOINT work_norm=(work_wing_x0_y1==0)?pLineAngle->vxWingNorm:pLineAngle->vyWingNorm;
	GEPOINT work_wing_vec=(work_wing_x0_y1==0)?pLineAngle->GetWingVecX():pLineAngle->GetWingVecY();
	for(int i=0;i<4;i++)
	{
		if(pGroupAngle->son_jg_h[i]==pLineAngle->handle)
			continue;
		CLDSLineAngle *pOtherAngle=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
		if(pOtherAngle==NULL||pOtherAngle->m_bVirtualPart)
			continue;
		if((pOtherAngle->vxWingNorm*work_norm<-EPS_COS2&&pOtherAngle->GetWingVecX()*work_wing_vec>EPS_COS2)||
		   (pOtherAngle->vyWingNorm*work_norm<-EPS_COS2&&pOtherAngle->GetWingVecY()*work_wing_vec>EPS_COS2))
		{	//肢法线方向相反，肢延伸方向相同
			pBackAngleInSrcAngle=pOtherAngle;
			break;
		}
	}
	if(wing_x0_y1!=NULL)
		*wing_x0_y1=work_wing_x0_y1;
	return pBackAngleInSrcAngle;
}
//From NewPlate.cpp
//精度过高时直梁位置因预拱造成的异面会误设为折叠板 wht 16-10-12
const double FOLD_PLATE_COS_EPS=0.99984769515639123915701155881391;	//cos1度
BOOL CreateFoldPlateByMergePlates(CLDSPlate *pPlate1,CLDSPlate *pPlate2);
BOOL CDesignJdb::DesignCommonPlank(CLDSPlate* pPlank)
{
	if(pPlank==NULL)
		return FALSE;
	m_pPlank = pPlank;
	m_pBaseNode=console.FromNodeHandle(m_pPlank->designInfo.m_hBaseNode);
	m_pPlank->designInfo.partList.Empty();
	m_pPlank->designInfo.additional_vertex.Empty();
	if(!InputJdbLinePart())
		return FALSE;
	//处理基准杆件不是基准节点父杆件的情况 wht 08-01-31
	if(partset.FromHandle(m_pBaseNode->hFatherPart)==NULL&&m_pBasePart->IsLinePart())
	{
		for(CLDSLinePart *pLinePart=partset.GetFirst(); pLinePart; pLinePart=partset.GetNext())
		{
			f3dLine ent_line;
			ent_line.startPt = (pLinePart->pStart) ? pLinePart->pStart->xOriginalPos : pLinePart->xPosStart;
			ent_line.endPt = (pLinePart->pEnd) ? pLinePart->pEnd->xOriginalPos : pLinePart->xPosEnd;
			if(ent_line.PtInLine(m_pBaseNode->Position(false))>=1)	//0:不在直线上 1:在线段端点上 2:在线段内
			{
				m_pBasePart=pLinePart;	
				break;
			}
		}
	}
	if(m_pBasePart.IsNULL())
		return FALSE;
	pPlank->CopyModuleInstanceInfo(m_pBasePart);
	pPlank->designInfo.m_hBasePart=m_pBasePart->handle;
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	for(pLinePart=partset.GetFirst();!pLinePart.IsNULL();pLinePart=partset.GetNext())
		sum_partset.append(pLinePart.LinePartPointer());
	for(pLinePart=face2_jgset.GetFirst();!pLinePart.IsNULL();pLinePart=face2_jgset.GetNext())
		sum_partset.append(pLinePart.LineAnglePointer());
	//处理钢管
	for (pLinePart=face2_tubeset.GetFirst();!pLinePart.IsNULL();pLinePart=face2_tubeset.GetNext())
		sum_partset.append(pLinePart.LinePartPointer());
	for(pLinePart=face3_jgset.GetFirst();!pLinePart.IsNULL();pLinePart=face3_jgset.GetNext())
		sum_partset.append(pLinePart.LineAnglePointer());

	//判断是否为钢管折叠板
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
	if( VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION)&&
		m_pBasePart->IsTube()&&m_pPlank->face_N==1&&sum_partset.GetNodeNum()>0)
	{
		CLDSLineTube *pTube=m_pBasePart.LineTubePointer(),*pOtherTube=NULL;
		TUBEJOINT *pCurJoint=NULL;
		if(pTube->pStart==m_pBaseNode)
			pCurJoint=&pTube->m_tJointStart;
		else if(pTube->pEnd==m_pBaseNode)
			pCurJoint=&pTube->m_tJointEnd;
		if(pCurJoint&&pCurJoint->type==1)
			pOtherTube=(CLDSLineTube*)m_pBasePart->BelongModel()->FromPartHandle(pCurJoint->hLinkObj,CLS_LINETUBE);
		GEPOINT tube_len_vec[2]={pTube->ucs.axis_z,pTube->ucs.axis_z};
		ROD_PTR baseRodArr[2]={pTube,pOtherTube};
		for(int i=0;i<2;i++)
		{
			if(baseRodArr[i]==NULL)
				continue;
			tube_len_vec[i]=baseRodArr[i]->ucs.axis_z;
			if(baseRodArr[i]->pEnd==m_pBaseNode||DISTANCE(baseRodArr[i]->Start(),m_pBaseNode->Position())>DISTANCE(baseRodArr[i]->End(),m_pBaseNode->Position()))
				tube_len_vec[i]=-baseRodArr[i]->ucs.axis_z;
		}
		//将角钢按工作肢所在平面分组
		CLDSLinePart *pRayPart = partset.GetFirst();
		if(pRayPart==pTube||pRayPart==pOtherTube)
			pRayPart=partset.GetNext();
		if(pRayPart==NULL)
			return FALSE;
		f3dPoint work_norm=(pRayPart->End()-pRayPart->Start())^(pTube->End()-pTube->Start());
		normalize(work_norm);
		CXhPtrSet<CLDSLineAngle> rodSetArr[3];
		f3dPoint face_norm[3],spreadVec[3];
		double normOffset[3][2]={};
		long viceNormRodArr[3]={0,0,0};
		CMinDouble minRayVecZ;	//最接近垂直的射线杆件集合
		if(pTube&&pOtherTube)
		{	//按角钢肢法线定位基准构件分组
			for(CLDSLinePart *pRod=sum_partset.GetFirst();pRod;pRod=sum_partset.GetNext())
			{
				if(pRod==m_pBasePart||pRod==pOtherTube)
					continue;
				if(!pRod->IsAngle())
					logerr.Log("0x%X杆件不是角钢, 不支持此种结构的节点板设计",pRod->handle);
				CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
				long hViceJg=pAngle->des_norm_y.hViceJg;
				if(fabs(pAngle->vxWingNorm*work_norm)>fabs(pAngle->vyWingNorm*work_norm))
					hViceJg=pAngle->des_norm_x.hViceJg;
				if(hViceJg==pTube->handle)
					rodSetArr[0].append(pAngle);
				else if(hViceJg==pOtherTube->handle)
					rodSetArr[1].append(pAngle);
				else
					logerr.Log("0x%X角钢的当前工作肢法线的工作面参照杆件不是当前节点板的基准钢管",pAngle->handle);
			}
		}
		else
		{
			for(CLDSLinePart *pRod=sum_partset.GetFirst();pRod;pRod=sum_partset.GetNext())
			{
				if(pRod==m_pBasePart||pRod==pOtherTube)
					continue;
				CLDSLineAngle *pAngle=pRod->IsAngle()?(CLDSLineAngle*)pRod:NULL;
				if(pAngle==NULL)
					continue;
				int start0_end1=LjPosInPart(pRod);
				f3dPoint start=(pRod->pStart!=NULL)?pRod->pStart->Position():pRod->Start();
				f3dPoint end=(pRod->pEnd!=NULL)?pRod->pEnd->Position():pRod->End();
				f3dPoint ray_vec=(start0_end1==0)?(end-start):(start-end);
				normalize(ray_vec);
				vector_trans(ray_vec,pTube->ucs,FALSE);
				double fRayVecZ=ray_vec.z;
				ray_vec.z=0;
				normalize(ray_vec);
				int iSetIndex=0;
				for(int i=0;i<3;i++)
				{
					if(rodSetArr[i].GetNodeNum()<=0)
					{
						rodSetArr[i].append(pAngle);
						spreadVec[i]=ray_vec;
						iSetIndex=i;
						minRayVecZ.Update(fabs(fRayVecZ),(void*)i);
						break;
					}
					else if(fabs(spreadVec[i]*ray_vec)>FOLD_PLATE_COS_EPS)
					{
						rodSetArr[i].append(pAngle);
						break;
					}
				}
				if((iSetIndex==0||iSetIndex==1||iSetIndex==2)&&pAngle&&face_norm[iSetIndex].IsZero())
				{
					if(fabs(pAngle->vxWingNorm*work_norm)>fabs(pAngle->vyWingNorm*work_norm))
					{
						if(pAngle->desStartPos.wing_x_offset.gStyle==4)
						{
							normOffset[iSetIndex][0]=pAngle->desStartPos.wing_x_offset.offsetDist;
							normOffset[iSetIndex][1]=pAngle->desEndPos.wing_x_offset.offsetDist;
						}
						face_norm[iSetIndex]=pAngle->vxWingNorm;
						if(!pAngle->des_norm_x.bByOtherWing)
							viceNormRodArr[iSetIndex]=pAngle->des_norm_x.hViceJg;
					}
					else
					{
						if(pAngle->desEndPos.wing_x_offset.gStyle==4)
						{
							normOffset[iSetIndex][0]=pAngle->desStartPos.wing_y_offset.offsetDist;
							normOffset[iSetIndex][1]=pAngle->desEndPos.wing_y_offset.offsetDist;
						}
						face_norm[iSetIndex]=pAngle->vyWingNorm;
						if(!pAngle->des_norm_y.bByOtherWing)
							viceNormRodArr[iSetIndex]=pAngle->des_norm_y.hViceJg;
					}
				}
			}
		}
		//计算钢板延伸方向判断是否需要生成折叠板
		m_cPlateType=TYPE_COMMON_PLATE;
		if(rodSetArr[0].GetNodeNum()>0&&rodSetArr[1].GetNodeNum()>0)
		{
			if(pOtherTube==NULL&&viceNormRodArr[1]==pTube->handle&&viceNormRodArr[0]==pTube->handle)
			{	//基准构件为钢管且射线角钢肢法线参照杆件为当前钢管时支持生成折叠板
				if(rodSetArr[2].GetNodeNum()>0)
				{	//存在三组构件
					int iSet=(int)minRayVecZ.m_pRelaObj;
					if(viceNormRodArr[2]==pTube->handle) 
					{	//三根杆件分布从属三个不同面时，将竖直杆件归到其他面中
						if(iSet==0)
						{	
							if(fabs(face_norm[0]*face_norm[1])>fabs(face_norm[0]*face_norm[2]))
							{	
								rodSetArr[0].append(rodSetArr[1]);
								rodSetArr[1].Empty();
								rodSetArr[1].append(rodSetArr[2]);
							}
							else
								rodSetArr[0].append(rodSetArr[2]);
						}
						else if(iSet==1)
						{
							if(fabs(face_norm[1]*face_norm[0])>fabs(face_norm[1]*face_norm[2]))
							{
								rodSetArr[0].append(rodSetArr[1]);
								rodSetArr[1].Empty();
								rodSetArr[1].append(rodSetArr[2]);
							}
							else
								rodSetArr[1].append(rodSetArr[2]);
						}
						else if(iSet==2)
						{
							if(fabs(face_norm[2]*face_norm[0])>fabs(face_norm[2]*face_norm[1]))
								rodSetArr[0].append(rodSetArr[2]);
							else
								rodSetArr[1].append(rodSetArr[2]);
						}
						m_cPlateType=TYPE_BTM_FOLD_PLATE1;	//TYPE_BTM_FOLD_PLATE2
					}
				}
				else if(spreadVec[0]*spreadVec[1]<EPS_COS)
					m_cPlateType=TYPE_BTM_FOLD_PLATE1;
			}
			else if(pOtherTube==NULL&&viceNormRodArr[0]!=0&&
				viceNormRodArr[0]==viceNormRodArr[1]&&viceNormRodArr[0]==viceNormRodArr[2]&&
				(rodSetArr[0].GetNodeNum()+rodSetArr[1].GetNodeNum()+rodSetArr[2].GetNodeNum())==3)
			{	//顶部折叠板
				m_cPlateType=TYPE_TOP_FOLD_PLATE;
				const double MIN_NORM_OFFSET = -10;
				if( fabs(normOffset[0][0]-normOffset[0][1])<EPS&&normOffset[0][0]<MIN_NORM_OFFSET&&
					normOffset[0][0]<normOffset[1][0]&&normOffset[0][0]<normOffset[2][0])
				{	//0为基准合并集合2到集合1
					rodSetArr[1].append(rodSetArr[2]);
				}
				else if(fabs(normOffset[1][0]-normOffset[1][1])<EPS&&normOffset[1][0]<MIN_NORM_OFFSET&&
						normOffset[1][0]<normOffset[0][0]&&normOffset[1][0]<normOffset[2][0])
				{	//1为基准合并集合2到集合0
					rodSetArr[0].append(rodSetArr[2]);
				}
				else if(fabs(normOffset[2][0]-normOffset[2][1])<EPS&&normOffset[2][0]<MIN_NORM_OFFSET&&
						normOffset[2][0]<normOffset[0][0]&&normOffset[2][0]<normOffset[1][0])
				{	//2为基准合并集合1到集合0,拷贝集合2到集合1
					rodSetArr[0].append(rodSetArr[1]);
					rodSetArr[1].Empty();
					rodSetArr[1].append(rodSetArr[2]);
				}
				else 
					m_cPlateType=TYPE_COMMON_PLATE;
			}
			else if(pOtherTube)
				m_cPlateType=TYPE_BTM_FOLD_PLATE1;
		}
		if( m_cPlateType==TYPE_TOP_FOLD_PLATE||
			m_cPlateType==TYPE_BTM_FOLD_PLATE1||
			m_cPlateType==TYPE_BTM_FOLD_PLATE2)
		{
			typedef CLDSPlate* PLATE_PTR;
			PLATE_PTR plateArr[2]={m_pPlank,NULL};
			plateArr[1]=(CLDSPlate*)m_pPlank->BelongModel()->AppendPart(CLS_PLATE);
			for(int i=0;i<2;i++)
			{
				m_pPlank=plateArr[i];
				if(baseRodArr[i]!=NULL)
					m_pBasePart=baseRodArr[i];
				if(i==1)
				{
					m_pPlank->CopyProperty(plateArr[0]);
					m_pPlank->CopyModuleInstanceInfo(m_pBasePart);
					m_pPlank->designInfo.m_hBasePart=m_pBasePart->handle;
				}
				partset.Empty();
				sum_partset.Empty();
				for(CLDSLinePart *pRod=rodSetArr[i].GetFirst();pRod;pRod=rodSetArr[i].GetNext())
				{
					partset.append(pRod);
					sum_partset.append(pRod);
				}
				if(DesignCommonPlank_Internal(i==0)==FALSE)
					return FALSE;
			}
			m_pPlank=plateArr[0];	//合并之前设置基准钢板为第一块钢板
			return CreateFoldPlateByMergePlates(plateArr[0],plateArr[1]);
		}
	}
#endif
	BOOL bRetCode=DesignCommonPlank_Internal(TRUE);
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
	if( VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION)&&
		bRetCode&&m_pBasePart->IsTube()&&m_cPlateType==TYPE_TOP_SPLAY_PLATE)
	{	//设计变电构件八字折叠板
		m_pPlank->face_N=3;
		m_pPlank->designInfo.iFaceType=7;
		m_pPlank->jdb_style=1;
		m_pPlank->designInfo.m_bEnableFlexibleDesign=TRUE;
		m_pPlank->designInfo.iProfileStyle0123=1;
		m_pPlank->DesignPlate();
	}
#endif
	return bRetCode;
}
BOOL CDesignJdb::DesignCommonPlank_Internal(BOOL bDisplayDlg)
{
	try{
		if(m_pPlank->face_N==3)
			Create3FaceRefAxis();
		else
			CreateRefAxis();
		CSuperSmartPtr<CLDSLinePart> pLinePart;
		//重新设定角钢连接面号
		for(pLinePart=partset.GetFirst();pLinePart.IsHasPtr();pLinePart=partset.GetNext())
			pLinePart->feature=1;
		//处理钢管
		for (pLinePart=face2_tubeset.GetFirst();pLinePart.IsHasPtr();pLinePart=face2_tubeset.GetNext())
			pLinePart->feature=2;
		for(pLinePart=face2_jgset.GetFirst();pLinePart.IsHasPtr();pLinePart=face2_jgset.GetNext())
			pLinePart->feature=2;
		for(pLinePart=face3_jgset.GetFirst();pLinePart.IsHasPtr();pLinePart=face3_jgset.GetNext())
			pLinePart->feature=3;
		if(m_pBasePart->GetClassTypeId()!=CLS_LINETUBE&&IsOneLsDesignValid())
		{
			if(MessageBox(AfxGetMainWnd()->GetSafeHwnd(),
#ifdef AFX_TARG_ENU_ENGLISH
				"You can design singular bolt design without plate, whether to carry out single bolt design without plate?","LDS",MB_YESNO)==IDYES)
#else
				"你可进行无板的单螺栓设计,是否进行无板单螺栓设计?","LDS",MB_YESNO)==IDYES)
#endif
			{
				return DesignOneBoltJdb();
			}
		}
		if(bDisplayDlg)
 		{
			xDesLjbPara.plank_thick=CalJdbThick();
			DesLjbPara();
		}
		if(m_pBasePart->GetClassTypeId()==CLS_LINEANGLE&&m_pPlank->face_N<2)
			m_pPlank->designInfo.m_bWeldToDatumJg=xDesLjbPara.bWeldToDatumJg!=FALSE;	//单面板是否焊接到基准角钢上
		m_pPlank->designInfo.iProfileStyle0123=xDesLjbPara.iProfileCalType01+1;
		m_pPlank->designInfo.iFaceType=(short)m_pPlank->face_N;
		m_pPlank->designInfo.m_hBasePart=m_pBasePart->handle;
		long hSecBasePart=0;	//第二根基准杆件句柄
		//下面一行为新加,处理板在角钢内皮时的情况 WJH-2003.12.18
		if(m_pPlank->designInfo.origin.datum_pos_style==1)	//角钢两端楞点为基准
			_snprintf(m_pPlank->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",xDesLjbPara.fNormOffset);
		else if(m_pPlank->designInfo.origin.datum_pos_style==2)	//角钢节点为基准
			_snprintf(m_pPlank->designInfo.origin.des_para.RODNODE.norm_offset,MAX_TEMP_CHAR_50,"%.0f",xDesLjbPara.fNormOffset);
		if((m_pBasePart->GetClassTypeId()==CLS_LINEANGLE||m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)&&m_pPlank->face_N>1)
			DesignHuoQu();
		else if(m_pBasePart->GetClassTypeId()==CLS_LINETUBE)	//m_pPlank->face_N=1;
		{
			TUBEJOINT *pCurJoint=&m_pBasePart.LineTubePointer()->m_tJointStart;
			if(m_pBaseRod->pEnd->handle==m_pBaseNode->handle)
				pCurJoint=&m_pBasePart.LineTubePointer()->m_tJointEnd;
			CLDSLineTube *pOtherBase=NULL;
			if(pCurJoint->type==1&&pCurJoint->hViceLinkObj<0&&pCurJoint->hLinkObj>0)
			{
				hSecBasePart=pCurJoint->hLinkObj;
				pOtherBase = (CLDSLineTube *)face2_tubeset.FromHandle(hSecBasePart);
			}
			if(pOtherBase&&2==m_pPlank->face_N)
			{	//设计火曲线
				m_pPlank->designInfo.huoqufacenorm[0].norm_style=2;
				m_pPlank->designInfo.huoqufacenorm[0].hVicePart=hSecBasePart;
				if (face2_jgset.GetNodeNum()>0)
					m_pPlank->designInfo.huoqufacenorm[0].hCrossPart=face2_jgset.GetFirst()->handle;
				else if (face2_tubeset.GetNodeNum()>0 && face2_tubeset.GetFirst()->handle != pOtherBase->handle)
					m_pPlank->designInfo.huoqufacenorm[0].hCrossPart=face2_tubeset.GetFirst()->handle;
				else if (face2_tubeset.GetNodeNum()>1)
					m_pPlank->designInfo.huoqufacenorm[0].hCrossPart=face2_tubeset.GetNext()->handle;		
				f3dPoint huoqulineStart;
				f3dPoint huoqulineVec;
				m_pPlank->designInfo.huoqufacenorm[0].UpdateVector(console.GetActiveModel());
				m_pPlank->huoqufacenorm[0]=m_pPlank->designInfo.huoqufacenorm[0].vector;
				Int3d2f(&huoqulineStart,&huoqulineVec,pOtherBase->Start(),m_pPlank->huoqufacenorm[0],
					m_pBasePart.LineTubePointer()->Start(),m_pPlank->ucs.axis_z);
				m_pPlank->designInfo.huoquline_start[0].datum_pos_style = 0;
				m_pPlank->designInfo.huoquline_start[0].SetPosition( huoqulineStart-huoqulineVec*1000);
				m_pPlank->designInfo.huoquline_end[0].datum_pos_style = 0;
				m_pPlank->designInfo.huoquline_end[0].SetPosition(huoqulineStart+huoqulineVec*1000);
			}
		}
		SortLinePartSet();
		if(m_pBasePart->GetClassTypeId()!=CLS_LINETUBE&&m_pPlank->face_N>=2)
		{
			m_pPlank->designInfo.huoqufacenorm[0].norm_style=2;
			m_pPlank->designInfo.huoqufacenorm[0].hVicePart=design_huoqu[0].pBaseJg->handle;
			if(face2_jgset.GetNodeNum()>0)
				m_pPlank->designInfo.huoqufacenorm[0].hCrossPart=face2_jgset.GetFirst()->handle;
			else if(face2_tubeset.GetNodeNum()>0)
				m_pPlank->designInfo.huoqufacenorm[0].hCrossPart=face2_tubeset.GetFirst()->handle;
			else
#ifdef AFX_TARG_ENU_ENGLISH
				throw "No ray rod on the second plane,two-plane plate can't be generated!";
#else
				throw "第二面无射线杆件，不能生成双面板！";
#endif
			if(m_pPlank->face_N==3)
			{
				m_pPlank->designInfo.huoqufacenorm[1].norm_style=2;
				m_pPlank->designInfo.huoqufacenorm[1].hVicePart=design_huoqu[1].pBaseJg->handle;
				m_pPlank->designInfo.huoqufacenorm[1].hCrossPart=face3_jgset.GetFirst()->handle;
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pBaseNode->handle);
		if(m_pBasePart->GetClassTypeId()==CLS_LINETUBE)
		{	
			if (m_pPlank->face_N == 1)
				DesignLineTube(1,m_pBasePart.LineTubePointer(),NULL);
		}
		else if(m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)m_pBasePart.LineAnglePointer();
			//pPlank->designInfo.m_hBasePart=pGroupAngle->son_jg_h[0];
			//现已通过综合考虑,不再将组合角钢类型的基准角钢强制转为其基准子角钢,以免在后期PresetRayJgEndPos时出错 wjh-2016.11.05
			//m_pBasePart=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->m_hDatumSonAngle,CLS_LINEANGLE);
			DISPLAY_TYPE disp_type;
			g_pSolidSet->GetDisplayType(&disp_type);
			g_pSolidSet->SetDisplayType(DISP_SOLID);
			g_pSolidDraw->Draw();
			CHashSet<CLDSLineAngle*> hashBackAngleByHandle;
			for(int i=0;i<4;i++)
			{
				CLDSLineAngle *pCommBaseJg=NULL;
				if(pGroupAngle->son_jg_h[i]>=0x20)
					pCommBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
				if(pCommBaseJg==NULL||pCommBaseJg->m_bVirtualPart)
					continue;	//不存在对应的子角钢或子角钢为虚拟构件 wht 11-07-25
				if(hashBackAngleByHandle.GetValue(pCommBaseJg->handle))
					continue;
				CLDSLineAngle *pBackToBackJg=FindSonJgBackToBackJg(pCommBaseJg,m_pPlank->ucs.axis_z);
				if(pBackToBackJg)
					hashBackAngleByHandle.SetValue(pBackToBackJg->handle,pBackToBackJg);
				DesignLineAngle(1,pCommBaseJg,NULL,TRUE,pBackToBackJg);
			}
			g_pSolidSet->SetDisplayType(disp_type);
			g_pSolidDraw->Draw();
		}
		else if(m_pBasePart->IsAngle())
		{
			if(xDesLjbPara.bWeldToDatumJg)
			{	//节点板焊接到基准角钢时基准角钢不需要布置螺栓 将基准构件添加到构件列表中 wht 10-06-07
				CDesignLjPartPara* pLinePartPara=m_pPlank->designInfo.partList.Add(m_pBasePart->handle);
				//pLinePartPara->hPart=m_pBasePart.LineAnglePointer()->handle;
				pLinePartPara->m_nClassTypeId=CLS_LINEANGLE;
				pLinePartPara->iFaceNo=1;
				pLinePartPara->angle.bEndLjJg=(end_lj_jgset.FromHandle(pLinePartPara->hPart)!=NULL);
				IsInsideJg(m_pBasePart.LineAnglePointer(),&pLinePartPara->angle.cur_wing_x0_y1);
			}
			else
				DesignLineAngle(1,m_pBasePart.LineAnglePointer(),NULL,TRUE);
		}
		m_pBasePart->SetModified();
		CLDSLinePart *pNextPart=NULL;
		CHashSet<CLDSLineAngle*> backToBackJgSet;	//仅为判断当前组合角钢子角钢是否已作为背铁处理过 wjh-2016.8.4
		for(pLinePart=sum_partset.GetFirst();pLinePart.IsHasPtr();pLinePart=sum_partset.GetNext())
		{
			pLinePart->SetModified();
			if(m_pBasePart.IsEqualPtr(pLinePart))	//基准角钢已设计
				continue;
			else if(pLinePart->IsAngle()&&pLinePart.pAngle->group_father_jg_h==m_pBasePart->handle)
				continue;
			if(pLinePart->handle==hSecBasePart)
			{	//第二根基准杆件与基准杆件对接相贯无需设计,将其加入到钢板连接杆件列表中
				CDesignLjPartPara *pLinePartPara=m_pPlank->designInfo.partList.Add(pLinePart->handle);
				pLinePartPara->m_nClassTypeId=pLinePart->GetClassTypeId();
				//pLinePartPara->hPart=pLinePart->handle;
				pLinePartPara->iFaceNo=(int)pLinePart->feature;	//所在面号
				pLinePartPara->start0_end1=LjPosInPart(pLinePart);
				continue;	 
			}
			else if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE&&pLinePart.GroupAnglePointer()->m_hDatumSonAngle==m_pBasePart->handle)
				continue;
			sum_partset.push_stack();
			pNextPart = sum_partset.GetNext();
			if(pNextPart==NULL)
			{
				pNextPart=sum_partset.GetFirst();	//最末尾角钢的下一角钢就是首角钢
				if(m_pBasePart.IsEqualPtr(pNextPart))
					pNextPart=sum_partset.GetNext();
				if(pNextPart==pLinePart)
					pNextPart=NULL;
			}
			sum_partset.pop_stack();
			if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				DISPLAY_TYPE disp_type;
				g_pSolidSet->GetDisplayType(&disp_type);
				g_pSolidSet->SetDisplayType(DISP_SOLID);
				g_pSolidDraw->Draw();
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pCommRayJg=NULL,*pBackToBackJg=NULL;
					/*if(((CLDSGroupLineAngle*)pLinePart.LineAnglePointer())->group_style==2)	
					{	//布置十字组合角钢螺栓时，仅需布置1号和3号子角钢的螺栓  wht 09-09-07
						//系统会自动将1号角钢上的螺栓引入到2/4号角钢上，将3号角钢上的螺栓引入到2/4号角钢上
						if(i==1||i==3)
							continue;
						if(i==0&&pLinePart.LineAnglePointer()->son_jg_h[1]>=0x20)
							pBackToBackJg=(CTMALineAngle*)console.FromPartHandle(pLinePart.LineAnglePointer()->son_jg_h[1],CLS_LINEANGLE);
						else if(i==2&&pLinePart.LineAnglePointer()->son_jg_h[3]>=0x20)
							pBackToBackJg=(CTMALineAngle*)console.FromPartHandle(pLinePart.LineAnglePointer()->son_jg_h[3],CLS_LINEANGLE);
					}*/
					pCommRayJg=(CLDSLineAngle*)console.FromPartHandle(pLinePart.LineAnglePointer()->son_jg_h[i],CLS_LINEANGLE);
					if(pCommRayJg&&!pCommRayJg->m_bVirtualPart)
					{	//存在对应的子角钢且子角钢不为虚拟构件 wht 11-07-25
						if(pCommRayJg->pStart==m_pBaseNode)
						{
							pCommRayJg->desStartOdd.m_hRefPart1=m_pPlank->designInfo.m_hBasePart;
							if(pNextPart)
								pCommRayJg->desStartOdd.m_hRefPart2=pNextPart->handle;
							pCommRayJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
							if(pCommRayJg->GetLocalLsCount(1,150)==0)
								pCommRayJg->desStartOdd.m_iOddCalStyle=0;	//根据碰撞计算正负头
							pCommRayJg->CalStartOddment();
							/*if(pBackToBackJg)
							{
								pBackToBackJg->desStartOdd.m_hRefPart1=m_pPlank->designInfo.m_hBasePart;
								if(pNextPart)
									pBackToBackJg->desStartOdd.m_hRefPart2=pNextPart->handle;
								pBackToBackJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
							}*/
						}
						else if(pCommRayJg->pEnd==m_pBaseNode)
						{
							pCommRayJg->desEndOdd.m_hRefPart1=m_pPlank->designInfo.m_hBasePart;
							if(pNextPart)
								pCommRayJg->desEndOdd.m_hRefPart2=pNextPart->handle;
							pCommRayJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
							if(pCommRayJg->GetLocalLsCount(2,150)==0)
								pCommRayJg->desEndOdd.m_iOddCalStyle=0;	//根据碰撞计算正负头
							pCommRayJg->CalEndOddment();
							/*if(pBackToBackJg)
							{
								pBackToBackJg->desEndOdd.m_hRefPart1=m_pPlank->designInfo.m_hBasePart;
								if(pNextPart)
									pBackToBackJg->desEndOdd.m_hRefPart2=pNextPart->handle;
								pBackToBackJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
							}*/
						}
						else 
						{	//判断端节点非基准节点的杆件始/终端连接 wht 10-01-04
							f3dLine datum_line;
							datum_line.startPt=m_pBasePart.LinePartPointer()->pStart->Position(false);
							datum_line.endPt=m_pBasePart.LinePartPointer()->pEnd->Position(false);
							if(datum_line.PtInLine(pCommRayJg->pStart->Position(false))>0)
							{
								pCommRayJg->desStartOdd.m_hRefPart1=m_pPlank->designInfo.m_hBasePart;
								if(pNextPart)
									pCommRayJg->desEndOdd.m_hRefPart2=pNextPart->handle;
								pCommRayJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
								if(pCommRayJg->GetLocalLsCount(1,150)==0)
									pCommRayJg->desStartOdd.m_iOddCalStyle=0;	//根据碰撞计算正负头
								pCommRayJg->CalStartOddment();
							}
							else if(datum_line.PtInLine(pCommRayJg->pEnd->Position(false))>0)
							{
								pCommRayJg->desEndOdd.m_hRefPart1=m_pPlank->designInfo.m_hBasePart;
								if(pNextPart)
									pCommRayJg->desEndOdd.m_hRefPart2=pNextPart->handle;
								pCommRayJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
								if(pCommRayJg->GetLocalLsCount(2,150)==0)
									pCommRayJg->desEndOdd.m_iOddCalStyle=0;	//根据碰撞计算正负头
								pCommRayJg->CalEndOddment();
							}
						}
						DesignLineAngle((int)pLinePart->feature,pCommRayJg,pNextPart,FALSE,pBackToBackJg);
					}
				}
				g_pSolidSet->SetDisplayType(disp_type);
				g_pSolidDraw->Draw();
			}
			else// if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				long hRefPart2=0;
				//判断端节点非基准节点的杆件始/终端连接 wht 10-01-04
				f3dLine datum_line(m_pBaseRod->xPosStart, m_pBaseRod->xPosEnd);
				if(m_pBaseRod->pStart!=NULL&&m_pBaseRod->pEnd!=NULL)
				{
					datum_line.startPt=m_pBaseRod->pStart->Position(false);
					datum_line.endPt=m_pBaseRod->pEnd->Position(false);
					if(pNextPart&&pNextPart->handle!=m_pBasePart->handle)
					{
						if(datum_line.PtInLine(pNextPart->pStart->Position(false))==0
							&&datum_line.PtInLine(pNextPart->pEnd->Position(false))==0)
							hRefPart2=0; //第二根杆件始终端均不在基准杆件上,不做为碰撞依据 wht 10-05-25
						else
							hRefPart2=pNextPart->handle;
					}
					else if(!m_pPrevPart.IsNULL())
					{
						if(datum_line.PtInLine(m_pPrevPart.LinePartPointer()->pStart->Position(false))==0
							&&datum_line.PtInLine(m_pPrevPart.LinePartPointer()->pEnd->Position(false))==0)
							hRefPart2=0; //第二根杆件始终端均不在基准杆件上,不做为碰撞依据 wht 10-05-25
						else
							hRefPart2=m_pPrevPart->handle;
					}
				}
				//当前杆件是否为端连接杆件，端连接杆件不需要自动计算正负头,
				//但需要碰撞参照杆件，用来自动计算切角切肢	wht 12-05-10
				BOOL bEndLjRod = (end_lj_jgset.FromHandle(pLinePart->handle)!=NULL);
				BOOL bTubeEndLjExist=(pLinePart->GetClassTypeId()==CLS_LINETUBE);
				BOOL iLinkToStart0End1=-1;
				if(pLinePart->pStart==m_pBaseNode)
					iLinkToStart0End1=0;
				else if(pLinePart->pEnd==m_pBaseNode)
					iLinkToStart0End1=1;
				else if(datum_line.PtInLine(pLinePart->Start(false))!=0)
					iLinkToStart0End1=0;	//存在连接到的偏移节点已超出datum_line范围的情况，故判断!=0 wjh-2015.1.05
				else if(datum_line.PtInLine(pLinePart->End(false))!=0)
					iLinkToStart0End1=1;
				DESIGNODDMENT *pCurrDesOdd=NULL;
				if(iLinkToStart0End1==0)
				{
					pCurrDesOdd=&pLinePart->desStartOdd;
					if( bTubeEndLjExist&&(pLinePart.LineTubePointer()->m_tJointStart.hLinkObj<=0||
						Ta.Parts.FromHandle(pLinePart.LineTubePointer()->m_tJointStart.hLinkObj)==NULL))
						bTubeEndLjExist=FALSE;
				}
				else if(iLinkToStart0End1==1)
				{
					pCurrDesOdd=&pLinePart->desEndOdd;
					if( bTubeEndLjExist&&(pLinePart.LineTubePointer()->m_tJointEnd.hLinkObj<=0||
						Ta.Parts.FromHandle(pLinePart.LineTubePointer()->m_tJointEnd.hLinkObj)==NULL))
						bTubeEndLjExist=FALSE;
				}
				if(!bTubeEndLjExist&&!bEndLjRod&&pCurrDesOdd)
				{
					long hRefPart1=m_pPlank->designInfo.m_hBasePart;
					if(m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
					{	//正负头参照杆件为组合角钢时，自动识别靠近的子角钢 wxc-2018.6.22
						f3dPoint norm=m_pPlank->ucs.axis_z;
						f3dPoint line_vec=(pLinePart->End()-pLinePart->Start()).normalized();
						if(iLinkToStart0End1==1)
							line_vec*=-1;
						CLDSGroupLineAngle *pGroupAngle=m_pBasePart.GroupAnglePointer();
						for(int i=0;i<4;i++)
						{
							CLDSLineAngle *pSonJg=NULL;
							if(pGroupAngle->son_jg_h[i]>=0x20)
								pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
							if(pSonJg==NULL||pSonJg->m_bVirtualPart)
								continue;
							f3dPoint workWingVec=(fabs(norm*pSonJg->vxWingNorm)>fabs(norm*pSonJg->vyWingNorm))?pSonJg->GetWingVecX():pSonJg->GetWingVecY();
							if(workWingVec*line_vec>0)
							{
								hRefPart1=pSonJg->handle;
								break;
							}
						}
					}
					pCurrDesOdd->m_hRefPart1=hRefPart1;
					pCurrDesOdd->m_hRefPart2=hRefPart2;
					pCurrDesOdd->m_fCollideDist=g_sysPara.VertexDist;
					if(pLinePart->IsAngle()&&(
						(iLinkToStart0End1==0&&pLinePart->GetLocalLsCount(1,150)==0)||
						(iLinkToStart0End1==1&&pLinePart->GetLocalLsCount(2,150)==0)))
						pCurrDesOdd->m_iOddCalStyle=0;	//根据碰撞计算正负头
					if(pLinePart->IsAngle()&&iLinkToStart0End1==0)
						pLinePart->CalStartOddment();//设计钢管时不用在此处计算正负头，否则会很慢 wht10-11-10
					else if(pLinePart->IsAngle()&&iLinkToStart0End1==1)
						pLinePart->CalEndOddment();//设计钢管时不用在此处计算正负头，否则会很慢 wht10-11-10
				}

				if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
				{
					BOLTSET legacyBoltSet;
					if(m_pPlank!=NULL&&end_lj_jgset.FromHandle(pLinePart->handle)){
						for(CLsRef *pLsRef=m_pPlank->GetFirstLsRef();pLsRef;pLsRef=m_pPlank->GetNextLsRef())
						{
							if(pLinePart.LineAnglePointer()->IsLsInWing(pLsRef->GetLsPtr()))
								legacyBoltSet.append(pLsRef->GetLsPtr());
						}
					}
					//当前角钢为组合角钢子角钢时，查找当前工作肢对应的背靠背角钢，只需引入螺栓并计算正负头，不用布置螺栓 wht 16-07-12
					CLDSLineAngle *pBackToBackJg=FindSonJgBackToBackJg(pLinePart.LineAnglePointer(),m_pPlank->ucs.axis_z);
					if(pBackToBackJg&&sum_partset.FromHandle(pBackToBackJg->handle)!=NULL)
						backToBackJgSet.SetValue(pBackToBackJg->handle,pBackToBackJg);
					else
						pBackToBackJg=NULL;
					if(backToBackJgSet.GetValue(pLinePart->handle)==NULL)
					{
						DesignLineAngle((int)pLinePart->feature,pLinePart.LineAnglePointer(),pNextPart,FALSE,pBackToBackJg,&legacyBoltSet);
						if(pBackToBackJg)
						{
							if(iLinkToStart0End1==0)
							{
								pBackToBackJg->desStartOdd.m_iOddCalStyle=pLinePart->desStartOdd.m_iOddCalStyle;
								pBackToBackJg->SetStartOdd(pLinePart->startOdd());
								pBackToBackJg->CalStartOddment();
							}
							else if(iLinkToStart0End1==1)
							{
								pBackToBackJg->desEndOdd.m_iOddCalStyle=pLinePart->desEndOdd.m_iOddCalStyle;
								pBackToBackJg->SetEndOdd(pLinePart->endOdd());
								pBackToBackJg->CalEndOddment();
							}
						}
					}
				}
				else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
				{
					if(!DesignLineTube((int)pLinePart->feature,pLinePart.LineTubePointer(),pNextPart))
						return FALSE;	//用户选择中止设计过程 wht 11-08-29
				}
				else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT||pLinePart->GetClassTypeId()==CLS_LINESLOT)
				{
					if(!DesignLineFlatOrSlot((int)pLinePart->feature,pLinePart))
						return FALSE;
				}
			}
			pLinePart->CalPosition();
			if(pLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				pLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pLinePart->GetSolidPartObject());
		}
		m_pPlank->designInfo.m_bEnableFlexibleDesign=TRUE;	//启用连板柔性设计
		/*CDesignLjParaDlg lj_dlg;
		lj_dlg.m_pLjPara=&m_pPlank->designInfo;
		if(lj_dlg.DoModal()!=IDOK)
			throw "中途退出!";*/
		if(m_pBasePart->GetClassTypeId()==CLS_LINETUBE)
		{	//基准构件为钢管时应在设计钢板前设定焊接父构件，否则不能正确标出对活点
			m_pPlank->m_bWeldPart=TRUE;
			m_pPlank->m_hPartWeldParent=m_pBasePart->handle;
		}
 		m_pPlank->DesignPlate();
		//m_pPlank->designInfo.iProfileStyle0123=3;	//自定义外形 暂认为默认为定制外形更合理,否则负头更改时可能不能彻底更新外形 wjh-2013.12.20
		PARTSET plateset;
		//装配定位杆件为钢管时,连板与定位杆件间需要焊接,同一节点上的焊接连接板根据用户输入判断是否需要调整焊缝线长度一致
		if(m_bAdjustWeldLen&&m_pBasePart->IsTube())
			AdjustPlateWeldLenOnSameNode(m_pBaseRod,m_pBaseNode);
		/*for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{	//重新设计并生成螺栓
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->SetModified();
			pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();*/
	}
	catch(char *sError)
	{
		if(!m_bTerminateByUser)
			AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}

void CDesignJdb::DesLjbPara()
{
	if(m_pBasePart->GetClassTypeId()==CLS_LINETUBE)
	{
		static CPlateBasicParaDlg basicParaDlg;
		if(basicParaDlg.m_nPlankThick<=0)
			basicParaDlg.m_nPlankThick=xDesLjbPara.plank_thick;
		if(strlen(basicParaDlg.m_sPartNo)==0)
			basicParaDlg.m_sPartNo=xDesLjbPara.sPartNo;
		if(basicParaDlg.m_iMaterial<=0)
			basicParaDlg.m_iMaterial=QuerySteelMatIndex(xDesLjbPara.cMaterial);
		if(strlen(basicParaDlg.m_sSegI)==0)
			basicParaDlg.m_sSegI=m_pBaseNode->iSeg.ToString();
		if(basicParaDlg.m_fEdgeSpace<=0)
			basicParaDlg.m_fEdgeSpace=xDesLjbPara.fInsertPlateEdgeSpace;
		if(basicParaDlg.DoModal()==IDOK)
		{
			xDesLjbPara.plank_thick = basicParaDlg.m_nPlankThick;
			m_pPlank->cMaterial = xDesLjbPara.cMaterial = CSteelMatLibrary::RecordAt(basicParaDlg.m_iMaterial).cBriefMark;
			_snprintf(xDesLjbPara.sPartNo,MAX_CHAR_GUIGE_16,basicParaDlg.m_sPartNo);
			m_pPlank->SetPartNo(xDesLjbPara.sPartNo);
			m_pPlank->iSeg=SEGI(basicParaDlg.m_sSegI.GetBuffer());
			m_pPlank->Thick=xDesLjbPara.plank_thick;
			xDesLjbPara.iProfileCalType01=basicParaDlg.m_iProfileType;
			m_pPlank->designInfo.weld_width = basicParaDlg.m_nWeldWidth;
			m_pPlank->m_fNormOffset=-0.5*xDesLjbPara.plank_thick;
			m_bAdjustWeldLen=basicParaDlg.m_bAdjustWeldLen;	//是否调整同一节点上的连接板焊缝长度一致
			xDesLjbPara.fInsertPlateEdgeSpace=basicParaDlg.m_fEdgeSpace;
		}
		else
		{
			m_bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
			throw "design cancelled halfway!";
#else
			throw "设计中途取消!";
#endif
		}
	}
	else
	{
		static CLjbDesignDlg dlg;
		if(design_huoqu[0].pBaseJg.IsEqualPtr(m_pBasePart))
			dlg.m_bEnableHuoQuPos = FALSE;
		else
			dlg.m_bEnableHuoQuPos = TRUE;
		dlg.m_nPlankThick = xDesLjbPara.plank_thick;
		dlg.m_nHuoQuDist  = xDesLjbPara.nHuoQuDist[0];
		dlg.m_nHuoQuDist2 = xDesLjbPara.nHuoQuDist[1];
		if(m_pBasePart->IsAngle())
			dlg.m_pBaseLineAngle=m_pBasePart.LineAnglePointer();
		dlg.m_fNormOffset=xDesLjbPara.fNormOffset;
		CLDSGroupLineAngle* pGroupAngle=m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE?m_pBasePart.pGroupAngle:NULL;
		if(pGroupAngle)
		{
			BYTE x_wing0_y_wing1=0;
			IsInsideJg(pGroupAngle,&x_wing0_y_wing1);
			dlg.base_x_wing0_y_wing1=x_wing0_y_wing1;
			dlg.reverseDirection=(m_pPlank->designInfo.norm.direction==1);	//钢板法向与所在肢法向相反，调整法向偏移量
			if(pGroupAngle->size_cSubClassType!='T'||pGroupAngle->ciMasterIndex==x_wing0_y_wing1)
			{
				dlg.m_fNormOffset=-xDesLjbPara.plank_thick/2.0;
				if(m_pPlank->designInfo.norm.direction==1)	//钢板法向与所在肢法向相反，调整法向偏移量
					dlg.m_fNormOffset*=-1;
			}
		}
		dlg.m_nNormDirection=m_pPlank->designInfo.norm.direction;
		dlg.m_nFaceN = m_pPlank->face_N;
		dlg.jdb_style = m_pPlank->jdb_style;
		dlg.m_nVertexDist = g_sysPara.VertexDist;
		dlg.m_fNormX=m_pPlank->ucs.axis_z.x;
		dlg.m_fNormY=m_pPlank->ucs.axis_z.y;
		dlg.m_fNormZ=m_pPlank->ucs.axis_z.z;
		if(dlg.m_sSegI.GetLength()<=0)
			dlg.m_sSegI=m_pBaseNode->iSeg.ToString();
		if(dlg.DoModal()==IDOK)
		{
			if(m_pBasePart->GetClassTypeId()==CLS_LINEANGLE&&m_pPlank->face_N==1)
				xDesLjbPara.bWeldToDatumJg = dlg.m_bWeldLj;	//节点板是否焊接到基准角钢
			else
				xDesLjbPara.bWeldToDatumJg=FALSE;
			xDesLjbPara.bAtAngleOuter=dlg.m_bIntersAtAngleOuter;
			xDesLjbPara.bHuoQuPosByCtrlPt = dlg.m_bHuoQuPosByCtrlPt;
			xDesLjbPara.plank_thick = dlg.m_nPlankThick;
			m_pPlank->Thick=xDesLjbPara.plank_thick;
			xDesLjbPara.nHuoQuDist[0] = dlg.m_nHuoQuDist ;
			xDesLjbPara.nHuoQuDist[1] = dlg.m_nHuoQuDist2;
			xDesLjbPara.nOffsetZhun[0]= dlg.m_nOffsetZhun;
			xDesLjbPara.nOffsetZhun[1]= dlg.m_nOffsetZhun2;
			xDesLjbPara.iProfileCalType01=dlg.m_iProfileType-1;	//自2015.9.21起增加了标准外形(盛达用)
			xDesLjbPara.fNormOffset=dlg.m_fNormOffset;
			m_pPlank->cMaterial = xDesLjbPara.cMaterial = CSteelMatLibrary::RecordAt(dlg.m_iPlankMaterial).cBriefMark;
			_snprintf(xDesLjbPara.sPartNo,MAX_CHAR_GUIGE_16,dlg.m_sPartNo);
			m_pPlank->SetPartNo(xDesLjbPara.sPartNo);
			xDesLjbPara.iRayIntersStyle=dlg.m_iRayJointStyle;
			xDesLjbPara.nVertexDist = dlg.m_nVertexDist;
			m_pPlank->iSeg=SEGI(dlg.m_sSegI.GetBuffer());
			m_pPlank->ucs.axis_z.x=dlg.m_fNormX;
			m_pPlank->ucs.axis_z.y=dlg.m_fNormY;
			m_pPlank->ucs.axis_z.z=dlg.m_fNormZ;
			Standarized(m_pPlank->ucs.axis_z);
			m_pPlank->ucs.axis_x=m_pPlank->ucs.axis_y^m_pPlank->ucs.axis_z;
			m_pPlank->ucs.axis_y=m_pPlank->ucs.axis_z^m_pPlank->ucs.axis_x;
			Standarized(m_pPlank->ucs.axis_x);
			Standarized(m_pPlank->ucs.axis_y);
		}
		else
		{
			m_bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
			throw "design cancelled halfway!";
#else
			throw "设计中途取消!";
#endif
		}
		for(int i=0;i<2;i++)	//提取火曲线设置数据
		{
			short x_wing0_y_wing1;
			if(!design_huoqu[i].pBaseJg.IsNULL())
			{
				IsInsideJg(design_huoqu[i].pBaseJg,&x_wing0_y_wing1);
				if(x_wing0_y_wing1==0)
					design_huoqu[i].bOffsetWingX=TRUE;
				else
					design_huoqu[i].bOffsetWingX=FALSE;
				design_huoqu[i].offset_dist = xDesLjbPara.nHuoQuDist[i];
			}
		}
	}
}
   //  1.计算连接板相对坐标系
bool CDesignJdb::CreateArcBasePlateAcs(CLDSLinePart* pRayRod)
{
	if(pRayRod==NULL || m_pBaseRod->pDelegatePart==NULL||!m_pBaseRod->pDelegatePart->IsArcPart())
		return false;
	SmartArcPtr pBaseArcRod=(CLDSArcPart*)m_pBaseRod->pDelegatePart;
	m_pPlank->ucs.axis_x=-pBaseArcRod->worknorm;
	f3dPoint rayvec=m_pBaseNode->Position()-f3dPoint(pBaseArcRod->center);
	project_vector(rayvec,pBaseArcRod->worknorm);
	Standarized(rayvec);
	m_pPlank->ucs.axis_z=rayvec;
	m_pPlank->designInfo.norm.norm_style=0;
	m_pPlank->designInfo.norm.vector=m_pPlank->ucs.axis_z;
	m_pPlank->ucs.origin=m_pBaseNode->Position()+pBaseArcRod->offset_h*rayvec;
	m_pPlank->ucs.axis_y=m_pPlank->ucs.axis_z^m_pPlank->ucs.axis_x;
	return true;
}
void CDesignJdb::CreateRefAxis()
{
	CSuperSmartPtr<CLDSLinePart> pRayPart = partset.GetFirst();
	if(m_pBasePart.IsEqualPtr(pRayPart)&&partset.GetNodeNum()>1)
		pRayPart=partset.GetNext();
	else if(m_pBaseRod==pRayPart)
		pRayPart=NULL;
	if(pRayPart.IsHasPtr()&&CreateArcBasePlateAcs(pRayPart))
		return;	//以环形杆件为基准杆件的连接板
	//要避免组合角钢子角钢形式的射线角钢与基准角钢（组合角钢的另一根子角钢）间参照计算角钢工作面法线　wjh-2016.11.2
	GEPOINT lenStdVec=m_pBasePart.pRod->End()-m_pBasePart.pRod->Start();
	normalize(lenStdVec);
	while(m_pBasePart->IsLinePart()&&pRayPart.IsHasPtr())
	{
		if(!pRayPart->IsLinePart())
		{
			pRayPart=partset.GetNext();
			continue;
		}
		GEPOINT lenStdVec2=pRayPart.pRod->End()-pRayPart.pRod->Start();
		normalize(lenStdVec2);
		if(fabs(lenStdVec*lenStdVec2)>EPS_COS2)
			pRayPart=partset.GetNext();
		else
			break;
	}
	if(m_pBasePart->GetClassTypeId()==CLS_LINETUBE)
	{
		m_pPlank->designInfo.origin.datum_pos_style=2;	//杆件上节点为基准
		m_pPlank->designInfo.origin.des_para.RODNODE.hNode=m_pBaseNode->handle;
		m_pPlank->designInfo.origin.des_para.RODNODE.hRod =m_pBasePart->handle;
		if(pRayPart.IsNULL())
		{
			f3dLine line(m_pBaseRod->Start(),m_pBaseRod->End());
			m_pPlank->ucs.axis_z=m_pBasePart.LineTubePointer()->WorkPlaneNorm();
			if(m_pPlank->ucs.axis_z*console.GetActiveView()->ucs.axis_z<0)
				m_pPlank->ucs.axis_z*=-1.0;
			m_pPlank->ucs.axis_y=line.startPt-line.endPt;
			m_pPlank->ucs.axis_x=m_pPlank->ucs.axis_y^m_pPlank->ucs.axis_z;
			m_pPlank->ucs.axis_y=m_pPlank->ucs.axis_z^m_pPlank->ucs.axis_x;
			Standarized(m_pPlank->ucs.axis_x);
			Standarized(m_pPlank->ucs.axis_y);
			SnapPerp(&m_pPlank->ucs.origin,line,m_pBaseNode->xArcliftPos,NULL);
			m_pPlank->designInfo.norm.norm_style=0;	//人工指定
			m_pPlank->designInfo.norm.vector=m_pPlank->ucs.axis_z;
		}
		else
		{
			f3dLine line(m_pBaseRod->Start(),m_pBaseRod->End());
			f3dPoint rayVec;
			bool bDisableSplayPlateDesign=false;
			if(!m_pBaseNode.IsEqualPtr(pRayPart->pStart)&&!m_pBaseNode.IsEqualPtr(pRayPart->pEnd))
				bDisableSplayPlateDesign=true;	//八字板要求必须是射线材连接到同一个节点上
			if(m_pBaseNode.IsEqualPtr(pRayPart->pStart))
				rayVec=pRayPart->End()-pRayPart->Start();
			else
				rayVec=pRayPart->Start()-pRayPart->End();
			m_pPlank->ucs.axis_y=line.startPt-line.endPt;
			m_pPlank->ucs.axis_z=rayVec^m_pPlank->ucs.axis_y;
			normalize(m_pPlank->ucs.axis_z);
			//
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
			CLDSLinePart *pNextRayRod=NULL;
			double ddx=0,ddy=0;
			if( VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION)&&
				m_pBasePart->IsTube()&&m_pPlank->face_N==1&&partset.GetNodeNum()>=2&&
				(m_cPlateType==TYPE_COMMON_PLATE||	 //设计普通板时判断是否为顶部八字板
				 m_cPlateType==TYPE_TOP_FOLD_PLATE)) //设计听不折叠板存在两个射线杆件时法线以射线杆件交叉为基准 wht 16-10-20
			{
				f3dPoint axis_z;
				for(CLDSLinePart *pRod=partset.GetFirst();pRod;pRod=partset.GetNext())
				{
					if(pRod!=m_pBasePart&&pRod!=pRayPart)
					{
						pNextRayRod=pRod;
						break;
					}
				}
				if(pNextRayRod)
				{
					//八字板要求必须是射线材连接到同一个节点上(以便排除管塔小跳线架干扰) wjh-2018.8.15
					if(!m_pBaseNode.IsEqualPtr(pNextRayRod->pStart)&&!m_pBaseNode.IsEqualPtr(pNextRayRod->pEnd))
						bDisableSplayPlateDesign|=true;
					if(bDisableSplayPlateDesign)
						pNextRayRod=NULL;
					else
					{
						f3dPoint nextRayVec;
						if(m_pBaseNode.IsEqualPtr(pNextRayRod->pStart))
							nextRayVec=pNextRayRod->End()-pNextRayRod->Start();
						else
							nextRayVec=pNextRayRod->Start()-pNextRayRod->End();
						axis_z=rayVec^nextRayVec;
						normalize(axis_z);
						//精度过高时直梁位置因预拱造成的异面会误设为折叠板 wht 16-10-12
						if(fabs(axis_z*m_pPlank->ucs.axis_z)>FOLD_PLATE_COS_EPS&&m_cPlateType==TYPE_COMMON_PLATE)
							pNextRayRod=NULL;
						if(m_cPlateType==TYPE_COMMON_PLATE&&pNextRayRod)
							m_cPlateType=TYPE_TOP_SPLAY_PLATE;	//设置顶部八字板
					}
				}
				if(pNextRayRod)
					m_pPlank->ucs.axis_z=axis_z;
			}
			else if( VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION)&&
				m_pPlank->face_N==1&&partset.GetNodeNum()==1&&partset.GetFirst()->IsAngle()&&
				m_cPlateType==TYPE_TOP_FOLD_PLATE)
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)partset.GetFirst();
				ddx=pAngle->vxWingNorm*m_pPlank->ucs.axis_z;
				ddy=pAngle->vyWingNorm*m_pPlank->ucs.axis_z;
				if(fabs(ddx)>fabs(ddy))
					m_pPlank->ucs.axis_z=pAngle->vxWingNorm;
				else
					m_pPlank->ucs.axis_z=pAngle->vyWingNorm;
			}
#endif
			if(m_pPlank->ucs.axis_z*console.GetActiveView()->ucs.axis_z<0)
				m_pPlank->ucs.axis_z*=-1.0;
			m_pPlank->ucs.axis_x=m_pPlank->ucs.axis_y^m_pPlank->ucs.axis_z;
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
			if(pNextRayRod)
				m_pPlank->ucs.axis_y=m_pPlank->ucs.axis_z^m_pPlank->ucs.axis_x;
#endif
			if(m_pPlank->ucs.axis_x*rayVec<0)
			{
				m_pPlank->ucs.axis_x*=-1.0;
				m_pPlank->ucs.axis_y*=-1.0;
			}
			Standarized(m_pPlank->ucs.axis_x);
			Standarized(m_pPlank->ucs.axis_y);
			Standarized(m_pPlank->ucs.axis_z);
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
			if( VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION)&&
				m_pPlank->face_N==1&&partset.GetNodeNum()==1&&partset.GetFirst()->IsAngle()&&
				m_cPlateType==TYPE_TOP_FOLD_PLATE)
			{	//
				CLDSLineAngle *pAngle=(CLDSLineAngle*)partset.GetFirst();
				m_pPlank->designInfo.origin.datum_pos_style=1;
				m_pPlank->designInfo.origin.des_para.RODEND.hRod=pAngle->handle;
				if(m_pBaseNode.IsEqualPtr(pRayPart->pStart))
					m_pPlank->designInfo.origin.des_para.RODEND.direction=0;
				else
					m_pPlank->designInfo.origin.des_para.RODEND.direction=1;
				m_pPlank->designInfo.origin.UpdatePos(&Ta);
				m_pPlank->ucs.origin=m_pPlank->designInfo.origin.Position();
				//
				m_pPlank->designInfo.norm.norm_style=1;
				m_pPlank->designInfo.norm.hVicePart=pAngle->handle;
				if(fabs(ddx)>fabs(ddy))
				{
					m_pPlank->designInfo.norm.norm_wing=0;
					m_pPlank->designInfo.norm.direction=(ddx>0?0:1);
				}
				else
				{
					m_pPlank->designInfo.norm.norm_wing=1;
					m_pPlank->designInfo.norm.direction=(ddy>0?0:1);
				}
			}
			else
			{
#endif
			SnapPerp(&m_pPlank->ucs.origin,line,m_pBaseNode->xArcliftPos,NULL);
			m_pPlank->designInfo.norm.norm_style=2;
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
			if(pNextRayRod)
				m_pPlank->designInfo.norm.hVicePart=pNextRayRod->handle;
			else
#endif
				m_pPlank->designInfo.norm.hVicePart=m_pPlank->designInfo.m_hBasePart;
			m_pPlank->designInfo.norm.hCrossPart=pRayPart->handle;
			m_pPlank->designInfo.norm.vector=m_pPlank->ucs.axis_z;
			m_pPlank->designInfo.norm.nearVector=console.GetActiveView()->ucs.axis_z;
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
			}
#endif
		}
	}
	else
	{
		BOOL bNeedUserJudge=FALSE;
		BOOL bInBaseJgWingX=FALSE;
		//确定基准角钢上的当前连接基准肢
		if(!pRayPart.IsNULL()&&pRayPart->GetClassTypeId()==CLS_LINEANGLE)	//第一面有射线角钢
		{
			int start0_end1=LjPosInPart(pRayPart);
			if(start0_end1==0&&pRayPart->pStart&&pRayPart->pStart->hFatherPart==m_pBaseRod->handle)
			{	//始端连接
				if(pRayPart.LineAnglePointer()->desStartPos.IsInDatumJgWingX())
					bInBaseJgWingX = TRUE;
				else if(pRayPart.LineAnglePointer()->desStartPos.IsInDatumJgWingY())
					bInBaseJgWingX = FALSE;
				else
					bNeedUserJudge = TRUE;
			}
			else if(start0_end1==1&&pRayPart->pEnd&&pRayPart->pEnd->hFatherPart==m_pBaseRod->handle)
			{	//终端连接
				if(pRayPart.LineAnglePointer()->desEndPos.IsInDatumJgWingX())
					bInBaseJgWingX = TRUE;
				else if(pRayPart.LineAnglePointer()->desEndPos.IsInDatumJgWingY())
					bInBaseJgWingX = FALSE;
				else
					bNeedUserJudge = TRUE;
			}
			else
				bNeedUserJudge = TRUE;
		}
		else if(!pRayPart.IsNULL()&&pRayPart->GetClassTypeId()==CLS_LINETUBE)	//第一面有射线角钢
		{
			int start0_end1=LjPosInPart(pRayPart);
			if(start0_end1==0)
			{
				if(pRayPart.LineTubePointer()->desStartPos.IsInDatumJgWingX())	//始端连接
					bInBaseJgWingX = TRUE;
				else if(pRayPart.LineTubePointer()->desStartPos.IsInDatumJgWingY())
					bInBaseJgWingX = FALSE;
				else
					bNeedUserJudge = TRUE;;
			}
			else if(start0_end1==1)		//终端连接
			{
				if(pRayPart.LineTubePointer()->desEndPos.IsInDatumJgWingX())
					bInBaseJgWingX = TRUE;
				else if(pRayPart.LineTubePointer()->desEndPos.IsInDatumJgWingY())
					bInBaseJgWingX = FALSE;
				else
					bNeedUserJudge = TRUE;;
			}
			else
				bNeedUserJudge = TRUE;
		}
		else if(!pRayPart.IsNULL()&&
			(pRayPart->GetClassTypeId()==CLS_LINEFLAT||pRayPart->GetClassTypeId()==CLS_LINESLOT))
		{
			int start0_end1=LjPosInPart(pRayPart);
			if(start0_end1==0)
			{
				if(pRayPart->DesStartPos()->IsInDatumJgWingX())	//始端连接
					bInBaseJgWingX = TRUE;
				else if(pRayPart->DesStartPos()->IsInDatumJgWingY())
					bInBaseJgWingX = FALSE;
				else
					bNeedUserJudge = TRUE;
			}
			else if(start0_end1==1)		//终端连接
			{
				if(pRayPart->DesEndPos()->IsInDatumJgWingX())
					bInBaseJgWingX = TRUE;
				else if(pRayPart->DesEndPos()->IsInDatumJgWingY())
					bInBaseJgWingX = FALSE;
				else
					bNeedUserJudge = TRUE;
			}
			else
				bNeedUserJudge = TRUE;
		}
		else
			bNeedUserJudge=TRUE;
		if(bNeedUserJudge&&pRayPart.IsHasPtr())
		{	//根据基准角钢与射线角钢自动判断基准角钢工作肢 wht 16-07-13
			GEPOINT work_norm=CalFaceNorm(m_pBaseRod->Start(),m_pBaseRod->End(),pRayPart->Start(),pRayPart->End());
			//GEPOINT work_norm=(m_pBaseRod->End()-m_pBaseRod->Start())^(pRayPart->End()-pRayPart->Start());
			normalize(work_norm);
			if( fabs(m_pBasePart.LineAnglePointer()->vxWingNorm*work_norm)>
				fabs(m_pBasePart.LineAnglePointer()->vyWingNorm*work_norm))
				bInBaseJgWingX=TRUE;
			else
				bInBaseJgWingX=FALSE;
		}
		else if(bNeedUserJudge)
		{
	#ifdef AFX_TARG_ENU_ENGLISH
			if(MessageBox(NULL," to choose basic angle's X leg(YES),or Y leg(NO)as current connecting working leg?","confirm working leg",MB_YESNO)==IDYES)
	#else
			if(MessageBox(NULL,"以基准角钢X肢(YES),还是以Y肢(NO)为当前连接基准肢?","指定基准肢",MB_YESNO)==IDYES)
	#endif
				bInBaseJgWingX = TRUE;
			else
				bInBaseJgWingX = FALSE;
		}
		//初始化钢板的定位参数
		f3dPoint plank_norm;
		CSuperSmartPtr<CLDSPart> pLineAngle=m_pBasePart;
		if(bInBaseJgWingX)
		{
			m_pPlank->designInfo.origin.datum_pos_style=2;	//角钢上节点为基准
			m_pPlank->designInfo.origin.des_para.RODNODE.hRod =m_pPlank->designInfo.m_hBasePart;
			m_pPlank->designInfo.origin.des_para.RODNODE.hNode=m_pPlank->designInfo.m_hBaseNode;
			m_pPlank->designInfo.origin.des_para.RODNODE.offset_wing=0;
			m_pPlank->designInfo.origin.des_para.RODNODE.wing_offset_style=0;	//标准准距
			m_pPlank->designInfo.norm.norm_style=1;
			m_pPlank->designInfo.norm.hVicePart=m_pPlank->designInfo.m_hBasePart;
			m_pPlank->designInfo.norm.norm_wing=0;
			m_pPlank->designInfo.norm.direction=0;	//朝外
			plank_norm=pLineAngle.LineAnglePointer()->get_norm_x_wing();
		}
		else
		{
			m_pPlank->designInfo.origin.datum_pos_style=2;	//角钢上节点为基准
			m_pPlank->designInfo.origin.des_para.RODNODE.hRod =m_pPlank->designInfo.m_hBasePart;
			m_pPlank->designInfo.origin.des_para.RODNODE.hNode=m_pPlank->designInfo.m_hBaseNode;
			m_pPlank->designInfo.origin.des_para.RODNODE.offset_wing=1;
			m_pPlank->designInfo.origin.des_para.RODNODE.wing_offset_style=0;	//标准准距
			m_pPlank->designInfo.norm.norm_style=1;
			m_pPlank->designInfo.norm.hVicePart=m_pPlank->designInfo.m_hBasePart;
			m_pPlank->designInfo.norm.norm_wing=1;
			m_pPlank->designInfo.norm.direction=0;	//朝外
			plank_norm=pLineAngle.LineAnglePointer()->get_norm_y_wing();
		}
		if(m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//对于组合角钢，需调整钢板法线朝向，保证与所在视图一致  wxc-2016.4.14
			f3dPoint axis_x,axis_y;
			AssureNormDirect(plank_norm);
			/*
			if(m_pBasePart->layer(2)=='1'||m_pBasePart->layer(2)=='3')
				axis_x.Set(1,0,0);
			else if(m_pBasePart->layer(2)=='2'||m_pBasePart->layer(2)=='4')
				axis_x.Set(-1,0,0);
			if(m_pBasePart->layer(2)=='1'||m_pBasePart->layer(2)=='2')
				axis_y.Set(0,1,0);
			else if(m_pBasePart->layer(2)=='3'||m_pBasePart->layer(2)=='4')
				axis_y.Set(0,-1,0);
			if(!axis_x.IsZero()&&!axis_y.IsZero())
			{
				double dd1=axis_x*plank_norm;
				double dd2=axis_y*plank_norm;
				if((fabs(dd1)>fabs(dd2)&&dd1<0) || (fabs(dd1)<fabs(dd2)&&dd2<0))
					m_pPlank->designInfo.norm.ReverseDirection();//direction=1;	//朝里(肢法向与视图法向相反)
			}
			*/
		}
		if(m_pBaseRod->pEnd==m_pBaseNode)
			m_pPlank->ucs.axis_y=m_pBaseRod->Start()-m_pBaseRod->End();
		else
			m_pPlank->ucs.axis_y=m_pBaseRod->End()-m_pBaseRod->Start();
		m_pPlank->designInfo.origin.UpdatePos(m_pPlank->BelongModel());
		m_pPlank->designInfo.norm.UpdateVector(m_pPlank->BelongModel());
		m_pPlank->ucs.axis_z=m_pPlank->designInfo.norm.vector;
		m_pPlank->ucs.axis_x=m_pPlank->ucs.axis_y^m_pPlank->ucs.axis_z;
		m_pPlank->ucs.axis_y=m_pPlank->ucs.axis_z^m_pPlank->ucs.axis_x;
		m_pPlank->ucs.origin=m_pPlank->designInfo.origin.Position();
		Standarized(m_pPlank->ucs.axis_x);
		Standarized(m_pPlank->ucs.axis_y);
		Standarized(m_pPlank->ucs.axis_z);
		f3dPoint huqu_vec,ray_vec[2];
		if(face2_jgset.GetNodeNum()>0)
			ray_vec[0]=face2_jgset.GetFirst()->End()-face2_jgset.GetFirst()->Start();
		if(face3_jgset.GetNodeNum()>0)
			ray_vec[1]=face3_jgset.GetFirst()->End()-face3_jgset.GetFirst()->Start();
		for(int i=0;i<2;i++)
		{
			if(design_huoqu[i].pBaseJg.IsHasPtr())
			{
				huqu_vec=design_huoqu[i].pBaseJg->End()-design_huoqu[i].pBaseJg->Start();
				m_pPlank->huoqufacenorm[i]=huqu_vec^ray_vec[i];
				if(m_pPlank->huoqufacenorm[i]*m_pPlank->ucs.axis_z<0)
					m_pPlank->huoqufacenorm[i]=-m_pPlank->huoqufacenorm[i].normalized();	//反向，一般默认连板火曲角度不超过90度
				else
					m_pPlank->huoqufacenorm[i]=m_pPlank->huoqufacenorm[i].normalized();
			}
		}
	}
}

BOOL CDesignJdb::IsInsideJg(CLDSLineAngle *pJg,BYTE *x_wing0_y_wing1)
{
	short siWingStyle=0;
	BOOL bInside=IsInsideJg(pJg,&siWingStyle);
	if(x_wing0_y_wing1)
		*x_wing0_y_wing1=(BYTE)siWingStyle;
	return bInside;
}
BOOL CDesignJdb::IsInsideJg(CLDSLineAngle *pJg,short *x_wing0_y_wing1)
{
	double ddx,ddy;
	int iFaceI = GetJgInFaceI(pJg);
	f3dPoint view_norm;
	if(iFaceI==1)
		view_norm = m_pPlank->ucs.axis_z;
	else if(iFaceI==2)
		view_norm = m_pPlank->huoqufacenorm[0];
	else if(iFaceI==3)
		view_norm = m_pPlank->huoqufacenorm[1];
	else
		view_norm = m_pPlank->ucs.axis_z;
	ddx = pJg->vxWingNorm*view_norm;
	ddy = pJg->vyWingNorm*view_norm;
	if(fabs(ddx)>fabs(ddy))
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 0;
		if(ddx>0)	//里铁
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 1;
		if(ddy>0)	//里铁
			return TRUE;
		else
			return FALSE;
	}
}
//0表示始端连接，1表示终端连接，2表示中间连接
int CDesignJdb::LjPosInPart(CLDSLinePart *pLinePart)
{
	if(m_pBaseRod.IsHasPtr())
		return LjPosInPart(m_pBaseRod,m_pBaseNode,pLinePart);
	else
		return LjPosInPart(NULL,m_pBaseNode,pLinePart);
}
int CDesignJdb::LjPosInPart(CLDSLinePart *pDatumLinePart,CLDSNode *pDatumNode,CLDSLinePart *pLinePart)
{
	if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pDatumLinePart==pLinePart)
		return 2;
	if(pLinePart->pStart==pDatumNode)
		return 0;
	else if(pLinePart->pEnd==pDatumNode)
		return 1;
	else	//中间连接
	{
		//基准角钢为组合且当前角钢为基准角钢子角钢
		//或当前角钢与基准角钢为同一个组合角钢中的子角钢 wht 10-01-25
		CLDSGroupLineAngle *pDatumGroupLineAngle=NULL;
		if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pDatumGroupLineAngle=(CLDSGroupLineAngle*)pDatumLinePart;
		else if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE&&
			((CLDSLineAngle*)pDatumLinePart)->group_father_jg_h>0x20)
			pDatumGroupLineAngle=(CLDSGroupLineAngle*)console.FromPartHandle(((CLDSLineAngle*)pDatumLinePart)->group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pDatumGroupLineAngle&&pLinePart->GetClassTypeId()==CLS_LINEANGLE&&
			((CLDSLineAngle*)pLinePart)->group_father_jg_h==pDatumGroupLineAngle->handle)
			return 2;
		//基准节点为交叉点时不自动判断杆件始终端连接	wht 10-02-02
		if(pDatumNode&&pDatumNode->m_cPosCalType==4)
			return 2;	
		if(pDatumLinePart&&pDatumLinePart!=pLinePart)
		{	//判断端节点非基准节点的杆件始/终端连接 wht 10-01-04
			f3dLine datum_line;
			datum_line.startPt=pDatumLinePart->pStart?pDatumLinePart->pStart->Position(false):pDatumLinePart->Start();
			datum_line.endPt=pDatumLinePart->pEnd?pDatumLinePart->pEnd->Position(false):pDatumLinePart->End();
			if(datum_line.PtInLine(pLinePart->pStart?pLinePart->pStart->Position(false):pLinePart->Start()>0))
				return 0;
			else if(datum_line.PtInLine(pLinePart->pEnd?pLinePart->pEnd->Position(false):pLinePart->End()>0))
				return 1;
		}
		return 2;
	}
}
/*void CDesignJdb::CreateRayJg2Vertex(BOOL bBaseJg)
{
	f3dLine ber_line;
	f3dPoint jg_vec,wing_vec,pt1,pt2;
	int x_wing0_y_wing1;
	int iFaceI = GetJgInFaceI(xDesJgPara.pCurJg);
	int ls_style = LjPosInPart(xDesJgPara.pCurJg);

	ber_line.startPt = xDesJgPara.pCurJg->Start();
	ber_line.endPt = xDesJgPara.pCurJg->End();

	IsInsideJg(xDesJgPara.pCurJg,&x_wing0_y_wing1);
	if(x_wing0_y_wing1==0)
		wing_vec=xDesJgPara.pCurJg->GetWingVecX();
	else
		wing_vec=xDesJgPara.pCurJg->GetWingVecY();
	CConnectInfo *pLsGroup;
	LSSPACE_STRU LsSpace;
	int ber_space,wing_space;
	GetLsSpace(LsSpace,xDesJgPara.ls_d);
	if(bBaseJg)
	{
		if(ls_style==0)	//始端连接
		{
			jg_vec = ber_line.endPt-ber_line.startPt;
			//SnapPerp(&pt1,ber_line,m_pBaseRod->connectStart.head_ls_pos);
			pLsGroup = &xDesJgPara.pCurJg->connectStart;
		}
		else if(ls_style==1)	//中间连接
		{
			jg_vec = ber_line.endPt-ber_line.startPt;
			//SnapPerp(&pt1,ber_line,m_pBaseRod->connectMid.head_ls_pos);
			pLsGroup = &xDesJgPara.pCurJg->connectMid;
		}
		else			//末端连接
		{
			jg_vec = ber_line.startPt-ber_line.endPt;
			//SnapPerp(&pt1,ber_line,m_pBaseRod->connectEnd.head_ls_pos);
			pLsGroup = &xDesJgPara.pCurJg->connectEnd;
		}
		ber_space=pLsGroup->ber_ls_line.feature;
		wing_space=pLsGroup->wing_ls_line.feature;
		if(ber_space<=0)
			ber_space=LsSpace.EndSpace;
		if(wing_space<=0)
			wing_space=LsSpace.EndSpace;
		normalize(jg_vec);
		if(m_pPlank->face_N!=3)
		{
			pt1 = pLsGroup->ber_ls_line.startPt-jg_vec*LsSpace.PlateEdgeSpace-wing_vec*ber_space;
			pt2 = pLsGroup->wing_ls_line.startPt-jg_vec*LsSpace.PlateEdgeSpace+wing_vec*wing_space;
			coord_trans(pt1,m_pPlank->ucs,FALSE);
			coord_trans(pt2,m_pPlank->ucs,FALSE);
			pt1 = MutiFacePntToDatumFace(pt1,iFaceI);
			pt2 = MutiFacePntToDatumFace(pt2,iFaceI);
			pt1.z = pt2.z = 0;
			
			pt1.ID = pt2.ID = xDesJgPara.pCurJg->handle;
			pt1.feature = pt2.feature = GetJgInFaceI(xDesJgPara.pCurJg);
			//暂时将以下三行注释掉，这三行可能用于生成部分非凸多边形连接板 WJH-2005.05.27
			//if(iFaceI>=2&&xDesJgPara.pCurJg!=m_pHuoQuJg[iFaceI-2]&&ls_style!=1)
			//	pt1.is_visible = FALSE;
			//else
			//	pt1.is_visible = TRUE;
			//pt2.is_visible = TRUE;;
			m_pPlank->vertex_list.append(pt1);
			m_pPlank->vertex_list.append(pt2);
		}
		//SnapPerp(&pt1,ber_line,ls_pos);
		pt1 = pLsGroup->ber_ls_line.endPt+jg_vec*LsSpace.PlateEdgeSpace-wing_vec*ber_space;
		pt2 = pLsGroup->wing_ls_line.endPt+jg_vec*LsSpace.PlateEdgeSpace+wing_vec*wing_space;
		coord_trans(pt1,m_pPlank->ucs,FALSE);
		coord_trans(pt2,m_pPlank->ucs,FALSE);
		pt1 = MutiFacePntToDatumFace(pt1,iFaceI);
		pt2 = MutiFacePntToDatumFace(pt2,iFaceI);
		pt1.z = pt2.z = 0;

		pt1.ID = pt2.ID = xDesJgPara.pCurJg->handle;
		pt1.feature = pt2.feature = GetJgInFaceI(xDesJgPara.pCurJg);
		m_pPlank->vertex_list.append(pt1);
		m_pPlank->vertex_list.append(pt2);
		return;
	}
	else if(ls_style==0)
	{
		jg_vec = ber_line.endPt-ber_line.startPt;
		pLsGroup = &xDesJgPara.pCurJg->connectStart;
	}
	else
	{
		jg_vec = ber_line.startPt-ber_line.endPt;
		pLsGroup = &xDesJgPara.pCurJg->connectEnd;
	}
	ber_space=pLsGroup->ber_ls_line.feature;
	wing_space=pLsGroup->wing_ls_line.feature;
	if(ber_space<=0)
		ber_space=LsSpace.EndSpace;
	if(wing_space<=0)
		wing_space=LsSpace.EndSpace;
	normalize(jg_vec);
	pt1 = pLsGroup->ber_ls_line.endPt+jg_vec*LsSpace.PlateEdgeSpace-wing_vec*ber_space;
	pt2 = pLsGroup->wing_ls_line.endPt+jg_vec*LsSpace.PlateEdgeSpace+wing_vec*wing_space;
	coord_trans(pt1,m_pPlank->ucs,FALSE);
	coord_trans(pt2,m_pPlank->ucs,FALSE);
	pt1 = MutiFacePntToDatumFace(pt1,iFaceI);
	pt2 = MutiFacePntToDatumFace(pt2,iFaceI);
	pt1.z = pt2.z = 0;

	pt1.ID = pt2.ID = xDesJgPara.pCurJg->handle;
	pt1.feature = pt2.feature = GetJgInFaceI(xDesJgPara.pCurJg);
	//pt1.is_visible = pt2.is_visible = TRUE;;
	m_pPlank->vertex_list.append(pt1);
	m_pPlank->vertex_list.append(pt2);
}*/

//设计钢管端连接
BOOL CDesignJdb::DesignLineTube(int iFaceNo,CLDSLineTube* pTube,CLDSLinePart* pNextPart/*=NULL*/)
{
	CDesignLjPartPara *pLinePartPara=m_pPlank->designInfo.partList.Add(pTube->handle);
	pLinePartPara->m_nClassTypeId=CLS_LINETUBE;
	pLinePartPara->hPart=pTube->handle;
	pLinePartPara->iFaceNo=iFaceNo;
	pLinePartPara->start0_end1=LjPosInPart(pTube);
	pLinePartPara->ber_space = xDesLjbPara.fInsertPlateEdgeSpace;
	//钢管连接基准钢管不作任何设计
	if(m_pBasePart.IsEqualPtr(pTube))
		return TRUE;
	g_pSolidDraw->SetEntSnapStatus(pTube->handle);
	CLDSParamPlate *pParamPlate=NULL;
	CDesignTubeEndLjDlg tubeendljdlg;
	//设置工作面法线
	tubeendljdlg.SetWorkNorm(m_pPlank->ucs.axis_z);
	tubeendljdlg.m_iTubeEndLjStyle=4; //默认选中槽型插板
	tubeendljdlg.FillPlatePara.nNodePlateThick=(int)m_pPlank->GetThick(); //节点板厚度 wht 11-05-11
	if(pLinePartPara->start0_end1==0)
	{	//始端连接
		tubeendljdlg.m_bStartEnd=TRUE;
		pTube->desStartOdd.m_hRefPart1=m_pBasePart->handle;
	}
	else if(pLinePartPara->start0_end1==1)
	{	//终端连接
		tubeendljdlg.m_bStartEnd=FALSE;
		pTube->desEndOdd.m_hRefPart1=m_pBasePart->handle;
	}
	tubeendljdlg.AttachPlatePara.m_pWeldParentPlate=m_pPlank;	//十字插板附加肋板焊接父构件 wht 10-08-02
	tubeendljdlg.NodePlatePara.m_nThick=ftoi(m_pPlank->GetThick());
	tubeendljdlg.m_pBaseNode=m_pBaseNode;
	tubeendljdlg.m_pCurLineTube=pTube;
	int nRetCode=tubeendljdlg.DoModal();
	pParamPlate=tubeendljdlg.GetParamPlate();
	
	if(pParamPlate)
	{
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{
			CSuperSmartPtr<CLDSBolt> pBolt=pLsRef->GetLsPtr();
			if(pParamPlate->m_iParamType==TYPE_XEND)
			{	//跳过十字插板上不合理螺栓
				double L=0;
				f3dPoint ls_pos=pBolt->ucs.origin;
				coord_trans(ls_pos,pParamPlate->ucs,FALSE);
				pParamPlate->GetDesignItemValue('L',&L);
				if(fabs(pBolt->get_norm()*pParamPlate->ucs.axis_z)<EPS_COS2)
					continue;
				if(ls_pos.x<L)
					continue;	
			}
			pBolt->AddL0Thick(m_pPlank->handle,TRUE,TRUE);
			pBolt->CalGuigeAuto();
			m_pPlank->AppendLsRef(pBolt->GetLsRef());
			pBolt->correct_pos();
			pBolt->SetModified();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	g_pSolidDraw->SetEntSnapStatus(pTube->handle,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	if(nRetCode!=IDOK&&pParamPlate==NULL&&AfxMessageBox("whether to terminate node plate design order?",MB_YESNO)==IDYES)
#else
	if(nRetCode!=IDOK&&pParamPlate==NULL&&AfxMessageBox("是否需要中止节点板设计命令?",MB_YESNO)==IDYES)
#endif
		return FALSE;	//点击取消按钮且pParamPlate为NULL时询问用户是否需要中止当前命令 wht 11-08-29
	return TRUE;
}
//扁铁端头连接设计
BOOL CDesignJdb::DesignLineFlatOrSlot(int iFaceNo,CLDSLinePart* pFlatOrSlot,BOLTSET *pLegacyBoltSet/*=NULL*/,BOOL bBasePart/*=FALSE*/)
{
	SmartRodPtr pFlat=pFlatOrSlot;	//出现少改代码考虑，暂延用原有pFlat名字代表扁铁或槽钢
	CDesignLjPartPara* pLinePartPara=m_pPlank->designInfo.partList.Add(pFlat->handle);
	pLinePartPara->hPart=pFlat->handle;
	pLinePartPara->m_nClassTypeId=pFlat->GetClassTypeId();//CLS_LINEFLAT;
	pLinePartPara->iFaceNo=iFaceNo;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pFlat->handle);
	double fWorkEndNormOffset;
	if(!bBasePart)	
		fWorkEndNormOffset=PresetRayFlatEndPos(pFlat);	//预设置射线扁铁位置
	else
		fWorkEndNormOffset=0;
	//设计当前连接扁铁的当前连接端
	CLayAngleBoltDlg dlg;
	dlg.m_pNode=m_pBaseNode;
	dlg.m_hPlate=m_pPlank->handle;
	dlg.m_pLinePart=pFlat;
	dlg.m_xiPreferSegI=m_pPlank->iSeg;
	dlg.viewNorm=m_pPlank->ucs.axis_z;
	dlg.m_pLegacyLsList=pLegacyBoltSet;
	if(pFlat->pStart==m_pBaseNode)
	{
		pLinePartPara->start0_end1=0;
		dlg.m_fOddment=pFlat->startOdd();
	}
	else if(pFlat->pEnd==m_pBaseNode)
	{
		pLinePartPara->start0_end1=1;
		dlg.m_fOddment=pFlat->endOdd();
	}
	GEPOINT workPlaneNorm=pFlatOrSlot->WorkPlaneNorm();
	if(workPlaneNorm*m_pPlank->ucs.axis_z<0)
	{
		dlg.m_tBoltNormOffset.AddThick(-m_pPlank->handle,TRUE);
		dlg.m_tBoltNormOffset.AddThick(-pFlat->handle,TRUE);
	}
	dlg.m_fAngleEndNormOffset=fWorkEndNormOffset;
	ATOM_LIST<CDesignLsPara>boltList;
	dlg.m_pLsList=&boltList;
	dlg.m_bIncPlateProfilePara=TRUE;
	if(dlg.DoModal()==IDOK)
	{	
		pLinePartPara->ber_space=dlg.m_fBerSpace;
		pLinePartPara->wing_space=dlg.m_fWingSpace;
		pLinePartPara->end_space=dlg.m_fPlateEdgeSpace;
		pLinePartPara->angle.bTwoEdge=dlg.m_bTwoEdgeProfile;
		if(pLinePartPara->start0_end1==0)
		{
			pFlat->desStartOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pFlat->SetStartOdd(dlg.m_fOddment);
			if(dlg.m_iOddCalStyle==1)
				pFlat->CalStartOddment();
		}
		else if(pLinePartPara->start0_end1==1)
		{
			pFlat->desEndOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pFlat->SetEndOdd(dlg.m_fOddment);
			if(dlg.m_iOddCalStyle==1)
				pFlat->CalEndOddment();
		}
		ATOM_LIST<RAYNO_RECORD>rayNoList;
		CObjNoGroup NoGroup(32);
		f3dLine ber_line;
		ber_line.startPt=pFlat->Start();
		ber_line.endPt=pFlat->End();
		for(CDesignLsPara *pBoltPara=boltList.GetFirst();pBoltPara;pBoltPara=boltList.GetNext())
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg = pBoltPara->iSeg;
			if(pBoltPara->des_work_norm.hVicePart==m_pBasePart->handle&&m_pBasePart.pRod->pDelegatePart&&m_pBasePart.pRod->pDelegatePart->IsArcPart())
			{	//以环向构件定位/向的螺栓，应改为以钢板为基准
				pBoltPara->des_work_norm.norm_style=3;
				pBoltPara->des_work_norm.direction=0;
				pBoltPara->des_work_norm.hVicePart=m_pPlank->handle;
			}
			pBoltPara->DesignLs(pBolt);
			pBolt->CopyModuleInstanceInfo(pFlat);
			//pBolt->cfgword=pFlat->cfgword;	//调整螺栓配材号与基准构件配材号一致
			//pBolt->SetLayer(pFlat->layer());
			pBolt->AddL0Thick(m_pPlank->handle,TRUE);
			pBolt->AddL0Thick(pFlat->handle,TRUE);
			if(pLinePartPara->start0_end1==0)
			{
				pFlat->connectStart.d=atoi(dlg.m_sBoltSize);
				pFlat->connectStart.wnConnBoltN=dlg.m_nLsNum;
				pFlat->connectStart.rows=dlg.m_iBoltRows+1;
				pFlat->AppendStartLsRef(pBolt->GetLsRef());
			}
			else if(pLinePartPara->start0_end1==1)
			{
				pFlat->connectEnd.d=atoi(dlg.m_sBoltSize);
				pFlat->connectEnd.wnConnBoltN=dlg.m_nLsNum;
				pFlat->connectEnd.rows=dlg.m_iBoltRows+1;
				pFlat->AppendEndLsRef(pBolt->GetLsRef());
			}
			m_pPlank->AppendLsRef(pBolt->GetLsRef());
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			//螺栓所在射线号的y坐标
			f3dPoint ls_vec,ls_pos=pBolt->ucs.origin;
			double ber_dist=0;
			project_point(ls_pos,pFlat->Start(),workPlaneNorm);
			SnapPerp(&ls_vec,ber_line,ls_pos,&ber_dist);
			if((ls_vec-ls_pos)*pFlat->ucs.axis_y<0)
				ber_dist*=-1;
			RAYNO_RECORD *pRayNo;
			for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
			{
				if(ftoi(pRayNo->yCoord)==ftoi(ber_dist))
				{
					pBolt->dwRayNo=pRayNo->dwRayNo;
					break;
				}
			}
			if(pRayNo==NULL)
			{
				pRayNo=rayNoList.append();
				int iRayNo=NoGroup.EnumIdleNo();
				NoGroup.SetNoState(iRayNo);
				pRayNo->dwRayNo=GetSingleWord(iRayNo);
				pRayNo->hPart=pBolt->des_base_pos.hPart;
				pRayNo->yCoord=ber_dist;
				pBolt->dwRayNo=pRayNo->dwRayNo;
			}
		}
	}
	else
	{
		m_bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
		throw "design cancelled halfway";
#else
		throw "设计中途取消";
#endif
	}
	g_pSolidDraw->SetEntSnapStatus(pFlat->handle,FALSE);
	m_pPrevPart = (CLDSLinePart*)pFlat;
	return TRUE;
}
//pBackToBackJg 背对背的角钢 主要在布置十字型组合角钢螺栓时使用 wht 09-09-07
void CDesignJdb::DesignLineAngle(int iFaceNo,CLDSLineAngle* pJg,CLDSLinePart* pNextPart,BOOL bBasePart,
								 CLDSLineAngle *pBackToBackJg/*=NULL*/,BOLTSET *pLegacyBoltSet/*=NULL*/)
{	
 	CDesignLjPartPara* pLinePartPara=m_pPlank->designInfo.partList.Add(pJg->handle);
	pLinePartPara->hPart=pJg->handle;
	pLinePartPara->m_nClassTypeId=CLS_LINEANGLE;
	pLinePartPara->iFaceNo=iFaceNo;
	pLinePartPara->angle.bEndLjJg=(end_lj_jgset.FromHandle(pLinePartPara->hPart)!=NULL);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pJg->handle);
	double fWorkEndNormOffset;
	if(!bBasePart)	//预设置（归位）射线角钢位置
	{
#if defined(__COMPLEX_PART_INC_) && defined(__LMA_FILE_)
		if( VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SUBSTATION)&&
			m_pPlank->face_N==1&&partset.GetNodeNum()==1&&partset.GetFirst()->IsAngle()&&
			m_cPlateType==TYPE_TOP_FOLD_PLATE)
		{	//顶部折叠板单射线构件单面板定位基准构件为角钢楞点,已预设始终端法向偏移量,此处不应再次调整 wht 16-10-20
			CLDSLineAngle *pAngle=(CLDSLineAngle*)partset.GetFirst();
			if(fabs(pAngle->vxWingNorm*m_pPlank->ucs.axis_z)>fabs(pAngle->vyWingNorm*m_pPlank->ucs.axis_z))
				fWorkEndNormOffset=pAngle->desStartPos.wing_x_offset.offsetDist;
			else
				fWorkEndNormOffset=pAngle->desStartPos.wing_y_offset.offsetDist;
		}
		else
#endif
		fWorkEndNormOffset=PresetRayJgEndPos(pJg);
	}
	else
		fWorkEndNormOffset=0;
	//设计当前连接角钢的当前连接端
	CLayAngleBoltDlg dlg;	//不能设为static 类型，否则会有很多属性错误继承 wjh-2015.3.7
	dlg.m_pNode=m_pBaseNode;
	dlg.m_hPlate=m_pPlank->handle;
	dlg.m_pLinePart=pJg;
	dlg.m_xiPreferSegI=m_pPlank->iSeg;	//使用钢板段号初始化节点板段号 wht 11-01-07
	if(pLinePartPara->iFaceNo==1)
		dlg.viewNorm=m_pPlank->ucs.axis_z;
	else if(pLinePartPara->iFaceNo==2)
		dlg.viewNorm=m_pPlank->huoqufacenorm[0];
	else //if(pLinePartPara->iFaceNo==3)
		dlg.viewNorm=m_pPlank->huoqufacenorm[1];
	CalCurJgPreSetLsN(pJg,bBasePart);
	if(bBasePart)	//基准角钢
	{
		dlg.m_bRayEndFaceOffset=m_bRayEndFaceOffset;
		dlg.m_bMirLayLs = TRUE;
	}
	else		//射线角钢
	{
		dlg.m_bMirLayLs = FALSE;
		dlg.m_bRayEndFaceOffset=FALSE;
	}
	BOOL bInsideJg = IsInsideJg(pJg,&pLinePartPara->angle.cur_wing_x0_y1);
	if(bInsideJg)	//里铁
		dlg.m_tBoltNormOffset.AddThick(-pJg->handle,TRUE);
	else
		dlg.m_tBoltNormOffset.AddThick(-m_pPlank->handle,TRUE);
	if(bBasePart)
	{
		if(m_pPlank->face_N!=3)
			dlg.m_bTwoEdgeProfile=TRUE;
		else if(design_huoqu[0].pBaseJg==design_huoqu[1].pBaseJg)
			dlg.m_bTwoEdgeProfile=TRUE;
		else	//三面板基准角钢不需要两端轮廓
			dlg.m_bTwoEdgeProfile=FALSE;
	}
	else
		dlg.m_bTwoEdgeProfile=FALSE;
	dlg.m_iBoltNorm=pLinePartPara->angle.cur_wing_x0_y1;
	if(pJg->pStart==m_pBaseNode)
	{
		pLinePartPara->start0_end1=0;
		//刘伟说放样时布置角钢螺栓对话框中即然确认了正负头，就不应该后台再更改，故默认值就应设为指定 wjh-2019.7.25
		dlg.m_iOddCalStyle=pJg->desStartOdd.m_iOddCalStyle==1?1:2;	//pJg->desStartOdd.m_iOddCalStyle;
		dlg.m_fOddment=pJg->startOdd();
		dlg.m_sExtraOdd.Format("%.0f",pJg->GetStartExtraOdd(dlg.m_iBoltNorm));
	}
	else if(pJg->pEnd==m_pBaseNode)
	{
		pLinePartPara->start0_end1=1;
		dlg.m_iOddCalStyle=pJg->desEndOdd.m_iOddCalStyle==1?1:2;	//pJg->desEndOdd.m_iOddCalStyle;
		dlg.m_fOddment=pJg->endOdd();
		dlg.m_sExtraOdd.Format("%.0f",pJg->GetEndExtraOdd(dlg.m_iBoltNorm));
	}
	else
	{
		//基准角钢为组合角钢且当前角钢为基准角钢子角钢
		//或当前角钢与基准角钢为同一个组合角钢中的子角钢 wht 10-01-25
		CLDSGroupLineAngle *pDatumGroupLineAngle=NULL;
		if(m_pBasePart.IsHasPtr()&&m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pDatumGroupLineAngle=(CLDSGroupLineAngle*)m_pBasePart.LineAnglePointer();
		else if(m_pBasePart.IsHasPtr()&&m_pBasePart->GetClassTypeId()==CLS_LINEANGLE&&
			m_pBasePart.LineAnglePointer()->group_father_jg_h>0x20)
			pDatumGroupLineAngle=(CLDSGroupLineAngle*)console.FromPartHandle(m_pBasePart.LineAnglePointer()->group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pDatumGroupLineAngle&&pJg->group_father_jg_h==pDatumGroupLineAngle->handle)
			pLinePartPara->start0_end1=2;
		//基准节点为交叉点时不自动判断杆件始终端连接	wht 10-02-02
		else if(m_pBaseNode->m_cPosCalType==4)
			pLinePartPara->start0_end1=2;	
		else if(m_pBasePart.IsHasPtr()&&!m_pBasePart.IsEqualPtr(pJg))
		{	//判断端节点非基准节点的杆件始/终端连接
			CLDSLinePart* pBaseRod = m_pBasePart.LinePartPointer();
			f3dLine datum_line;
			datum_line.startPt = (pBaseRod->pStart) ? pBaseRod->pStart->xOriginalPos : pBaseRod->xPosStart;
			datum_line.endPt = (pBaseRod->pEnd) ? pBaseRod->pEnd->xOriginalPos : pBaseRod->xPosEnd;
			if(datum_line.PtInLine(pJg->Start(false))>0)
				pLinePartPara->start0_end1=0;
			else if(datum_line.PtInLine(pJg->End(false))>0)
				pLinePartPara->start0_end1=1;	
			else //应询问用户始终端连接
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(AfxMessageBox("Is the rod starting current design?(Y/N)?",MB_YESNO)==IDYES)
#else
				if(AfxMessageBox("杆件的始端为当前设计端吗(Y/N)?",MB_YESNO)==IDYES)
#endif
					pLinePartPara->start0_end1=0;
				else
					pLinePartPara->start0_end1=1;
				dlg.m_bTwoEdgeProfile=TRUE; //不搭接在基准角钢上的射线角钢一般也需要两端轮廓 wht 10-05-25
			}
			if(pLinePartPara->start0_end1==0)
			{
				dlg.m_pNode=pJg->pStart;
				dlg.m_fOddment=pJg->startOdd();
				dlg.m_sExtraOdd.Format("%.0f",pJg->GetStartExtraOdd(dlg.m_iBoltNorm));
			}
			else if(pLinePartPara->start0_end1==1)
			{
				dlg.m_pNode=pJg->pEnd;
				dlg.m_fOddment=pJg->endOdd();
				dlg.m_sExtraOdd.Format("%.0f",pJg->GetEndExtraOdd(dlg.m_iBoltNorm));
			}
		}	
		else
			pLinePartPara->start0_end1=2;
	}
	GEPOINT vCurrRayVec=pJg->End()-pJg->Start();
	if(pLinePartPara->start0_end1==0)
	{
		dlg.m_nLsNum=pJg->BoltsNumOn1Wing(TRUE);//connectStart.N;
		dlg.m_iBoltRows=pJg->connectStart.rows-1;
		dlg.m_sBoltSize.Format("%d",pJg->connectStart.d);
	}
	else if(pLinePartPara->start0_end1==1)
	{
		dlg.m_nLsNum=pJg->BoltsNumOn1Wing(FALSE);//connectEnd.N;
		dlg.m_iBoltRows=pJg->connectEnd.rows-1;
		dlg.m_sBoltSize.Format("%d",pJg->connectEnd.d);
		vCurrRayVec=pJg->Start()-pJg->End();
	}
	else
	{
		dlg.m_nLsNum=pJg->connectMid.wnConnBoltN;
		dlg.m_iBoltRows=pJg->connectMid.rows-1;
		dlg.m_sBoltSize.Format("%d",pJg->connectMid.d);
		if(pNextPart)
		{
			dlg.m_iDatumPointStyle=3;	//角钢心线交点定位 wht 11-01-18
			dlg.m_hCrossAngle.Format("0X%X",pNextPart->handle);	//中间连接时往往都有交叉线螺栓存在
		}
	}
	dlg.m_fAngleEndNormOffset=fWorkEndNormOffset;
	/*if(end_lj_jgset.FromHandle(pJg->handle)||m_pPlank->jdb_style==1)
		dlg.m_bEnable2RowsLs = FALSE;	//端连接角钢或交叉板不允许双排
	else
		dlg.m_bEnable2RowsLs = TRUE;*/
	ATOM_LIST<CDesignLsPara>boltList;
	dlg.m_pLsList=&boltList;
	dlg.m_bIncPlateProfilePara=TRUE;
	long hGroupCorssAngle=0;
	if(end_lj_jgset.FromHandle(pJg->handle))
	{	//端连接射线角钢 需要使用心线交点，默认基准杆件为交叉杆件 wht 10-01-14
		dlg.m_bEndLjJg = TRUE;		//端连接角钢
		dlg.m_iDatumPointStyle=3;	//角钢心线交点定位
		if(dlg.m_iOddCalStyle==0)
			dlg.m_iOddCalStyle=1;	//端连接角钢默认正负头计算方式为根据螺栓计算，除非是用户指定 wjh-2015.1.06
		if(m_pBasePart.IsHasPtr()&&m_pBasePart->GetClassTypeId()==CLS_LINEANGLE)
			dlg.m_hCrossAngle.Format("0X%X",m_pBasePart->handle);
		else if(m_pBasePart.IsHasPtr()&&m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//基准角钢为组合角钢时,自动判断哪一根子角钢为端连接交叉参照角钢
			BYTE base_x_wing0_y_wing1=0;
			IsInsideJg(m_pBasePart.pAngle,&base_x_wing0_y_wing1);
			bool samedirect=false;
			if(base_x_wing0_y_wing1==0&&m_pBasePart.pAngle->GetWingVecX()*vCurrRayVec>0)
				samedirect=true;
			else if(base_x_wing0_y_wing1==1&&m_pBasePart.pAngle->GetWingVecY()*vCurrRayVec>0)
				samedirect=true;
			hGroupCorssAngle=m_pBasePart.pGroupAngle->m_hDatumSonAngle;;
			if(!samedirect&&m_pBasePart.pGroupAngle->size_cSubClassType=='D')
				hGroupCorssAngle=m_pBasePart.pGroupAngle->son_jg_h[1-m_pBasePart.pGroupAngle->ciMasterIndex];
			else if(!samedirect&&m_pBasePart.pGroupAngle->size_cSubClassType=='X'&&base_x_wing0_y_wing1==0)
				hGroupCorssAngle=m_pBasePart.pGroupAngle->son_jg_h[3];
			else if(!samedirect&&m_pBasePart.pGroupAngle->size_cSubClassType=='X'&&base_x_wing0_y_wing1==1)
				hGroupCorssAngle=m_pBasePart.pGroupAngle->son_jg_h[1];
			else if(!samedirect&&m_pBasePart.pGroupAngle->size_cSubClassType=='T'&&base_x_wing0_y_wing1!=m_pBasePart.pGroupAngle->ciMasterIndex)
				hGroupCorssAngle=m_pBasePart.pGroupAngle->son_jg_h[1-m_pBasePart.pGroupAngle->ciMasterIndex];
			dlg.m_hCrossAngle.Format("0X%X",hGroupCorssAngle);
		}
	}
	CLDSLineAngle *pEndLjRayJg=end_lj_jgset.GetFirst();
	if((m_pBasePart.IsEqualPtr(pJg)||pJg->group_father_jg_h==m_pBasePart->handle)&&pEndLjRayJg)
	{	//当前设计角钢为基准角钢时才需要端连接 wht 11-01-17
		GEPOINT rayVec;
		CMultiOffsetPos *pDesPosPara=NULL;
		int start0_end1_else2=LjPosInPart(pEndLjRayJg);
		if(start0_end1_else2==0)
		{
			pDesPosPara=&pEndLjRayJg->desStartPos;
			rayVec=pEndLjRayJg->End()-pEndLjRayJg->Start();
		}
		else if(start0_end1_else2==1)
		{
			pDesPosPara=&pEndLjRayJg->desEndPos;
			rayVec=pEndLjRayJg->Start()-pEndLjRayJg->End();
		}
		//if(pDesPosPara&&(pDesPosPara->spatialOperationStyle==17		//点线平推
		//	||(pDesPosPara->jgber_cal_style==0&&pDesPosPara->GetDatumType()==2)	//分两步偏移基点定位方式为平推
		//	||(pDesPosPara->datum_pos_style==15&&pDesPosPara->datum_to_ber_style==1)))//基点到楞点定位方式为平推
		GEPOINT vWorkWingVec=pLinePartPara->angle.cur_wing_x0_y1==0?pJg->GetWingVecX():pJg->GetWingVecY();
		if(pDesPosPara&&pDesPosPara->IsFaceOffset()&&((rayVec*vWorkWingVec)>0||m_pBasePart->GetClassTypeId()==CLS_LINEANGLE))
		{	//端连接射线杆件为平推时基准角钢默认螺栓定位方式为心线交点，交叉角钢默认为端连接角钢
			dlg.m_bEndLjDatumJg=TRUE;//端连接基准角钢 wht 10-01-14
			dlg.m_pEndLjRayJg=pEndLjRayJg;
			dlg.m_iDatumPointStyle=3;	//角钢心线交点定位
			if(pEndLjRayJg->GetClassTypeId()==CLS_LINEANGLE)
				dlg.m_hCrossAngle.Format("0X%X",pEndLjRayJg->handle);
		}
	}
	dlg.m_pLegacyLsList=pLegacyBoltSet;
	dlg.m_fHuoquWingOffsetDist=0;
	for(int i=0;i<2;i++)
	{
		if(!design_huoqu[i].pBaseJg.IsEqualPtr(pJg))
			continue;
		//当前角钢为火曲线基准角钢
		double wing_offset_dist=m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_dist;
		if(wing_offset_dist>0&&wing_offset_dist<pJg->GetWidth())
			dlg.m_fHuoquWingOffsetDist=wing_offset_dist;
	}
	if(dlg.DoModal()==IDOK)
	{
		if(!bBasePart)//设计射线角钢当前连接端位置
			DesRayJgEndPos(pJg, dlg.m_fAngleEndNormOffset);
		pLinePartPara->angle.cbSpaceFlag=dlg.cbSpaceFlag;
		pLinePartPara->ber_space=dlg.m_fBerSpace;
		pLinePartPara->wing_space=dlg.m_fWingSpace;
		pLinePartPara->end_space=dlg.m_fPlateEdgeSpace;
		pLinePartPara->angle.bTwoEdge=dlg.m_bTwoEdgeProfile;
		if(pLinePartPara->start0_end1==0)
		{
			pJg->desStartOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pJg->SetStartOdd(dlg.m_fOddment);
		}
		else if(pLinePartPara->start0_end1==1)
		{
			pJg->desEndOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pJg->SetEndOdd(dlg.m_fOddment);
		}
		//int iInitRayNo=1;
		ATOM_LIST<RAYNO_RECORD>rayNoList;
		CObjNoGroup NoGroup(32);
		BOLTSET boltSet;
		for(CDesignLsPara *pBoltPara=boltList.GetFirst();pBoltPara;pBoltPara=boltList.GetNext())
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg = pBoltPara->iSeg;
			if(pBoltPara->des_work_norm.hVicePart==m_pBasePart->handle&&m_pBasePart.pRod->pDelegatePart&&m_pBasePart.pRod->pDelegatePart->IsArcPart())
			{	//以环向构件定位/向的螺栓，应改为以钢板为基准
				pBoltPara->des_work_norm.norm_style=3;
				pBoltPara->des_work_norm.direction=0;
				pBoltPara->des_work_norm.hVicePart=m_pPlank->handle;
			}
			pBoltPara->DesignLs(pBolt);
			pBolt->CopyModuleInstanceInfo(pJg);
			PARTSET partset;
			partset.append(m_pPlank);
			partset.append(pJg);
			if(pBackToBackJg)
				partset.append(pBackToBackJg);
			//
			m_pPlank->AppendLsRef(pBolt->GetLsRef());
			if(pLinePartPara->start0_end1==0)
			{
				pJg->connectStart.d=atoi(dlg.m_sBoltSize);
				//pJg->connectStart.N=dlg.m_nLsNum;
				pJg->SetBoltsNumOn1Wing(dlg.m_nLsNum,TRUE);
				pJg->connectStart.rows=dlg.m_iBoltRows+1;
				pJg->AppendStartLsRef(pBolt->GetLsRef());
				if(pBackToBackJg)	//将螺栓引入到背对背的角钢上去 
					pBackToBackJg->AppendStartLsRef(pBolt->GetLsRef());
			}
			else if(pLinePartPara->start0_end1==1)
			{
				pJg->connectEnd.d=atoi(dlg.m_sBoltSize);
				//pJg->connectEnd.N=dlg.m_nLsNum;
				pJg->SetBoltsNumOn1Wing(dlg.m_nLsNum,FALSE);
				pJg->connectEnd.rows=dlg.m_iBoltRows+1;
				pJg->AppendEndLsRef(pBolt->GetLsRef());
				if(pBackToBackJg)	//将螺栓引入到背对背的角钢上去 
					pBackToBackJg->AppendEndLsRef(pBolt->GetLsRef());
			}
			else
			{
				pJg->AppendMidLsRef(pBolt->GetLsRef());
				if(pBackToBackJg)	//将螺栓引入到背对背的角钢上去 
					pBackToBackJg->AppendMidLsRef(pBolt->GetLsRef());
			}
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==3||m_pBasePart.IsEqualPtr(pJg))//交叉点定位或者角钢为基准角钢
			{
				CLDSLineAngle *pCrossAngle=NULL;
				JGSET jgset;
				for(pCrossAngle=end_lj_jgset.GetFirst();pCrossAngle;pCrossAngle=end_lj_jgset.GetNext())
					jgset.append(pCrossAngle);
				if(pBolt->des_base_pos.datumPoint.datum_pos_style==3&&end_lj_jgset.FromHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2)==NULL)
				{	//交叉点定位
					pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
					if(pCrossAngle)
						jgset.append(pCrossAngle);
				}
				for(pCrossAngle=jgset.GetFirst();pCrossAngle;pCrossAngle=jgset.GetNext())
				{
					if(pCrossAngle==pJg)
						continue;
					f3dPoint pos,bolt_vec,wing_vec,work_wing_norm,bolt_norm=pBolt->ucs.axis_z;
					char cWorkWing=(fabs(bolt_norm*pCrossAngle->vxWingNorm)>fabs(bolt_norm*pCrossAngle->vyWingNorm))?'X':'Y';
					wing_vec=(cWorkWing=='X')?pCrossAngle->GetWingVecX():pCrossAngle->GetWingVecY();
					work_wing_norm=(cWorkWing=='X')?pCrossAngle->vxWingNorm:pCrossAngle->vyWingNorm;
					Int3dlf(pos,pBolt->ucs.origin,bolt_norm,pCrossAngle->Start(),work_wing_norm);
					bolt_vec=pos-pCrossAngle->Start();
					double dd=bolt_vec*wing_vec;
					if(dd>0&&dd<pCrossAngle->GetWidth())
					{	//交叉螺栓位于交叉角钢内
						pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
						partset.append(pCrossAngle);
					}
				}
			}
			//统一更新通厚和法向偏移量(端连接时没考虑射线角钢)
			pBolt->EmptyL0DesignPara();
			for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				pBolt->AddL0Thick(pPart->handle,TRUE);
			pBolt->des_base_pos.norm_offset.EmptyThick();
			pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,partset);
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			double g=pJg->GetLsG(pBolt);
			RAYNO_RECORD *pRayNo;
			for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
			{
				if(ftoi(pRayNo->yCoord)==ftoi(g))
				{
					pBolt->dwRayNo=pRayNo->dwRayNo;
					break;
				}
			}
			if(pRayNo==NULL)
			{
				pRayNo=rayNoList.append();
				int iRayNo=NoGroup.EnumIdleNo();
				NoGroup.SetNoState(iRayNo);
				pRayNo->dwRayNo=GetSingleWord(iRayNo);
				pRayNo->hPart=pBolt->des_base_pos.hPart;
				pRayNo->yCoord=g;
				pBolt->dwRayNo=pRayNo->dwRayNo;
			}
			//
			boltSet.append(pBolt);
		}
		//根据螺栓计算角钢正负头应在角钢引入螺栓之后再进行计算 wht 16-10-31
		if(pLinePartPara->start0_end1==0&&pJg->desStartOdd.m_iOddCalStyle==1)
		{
			pJg->CalStartOddment();
			pJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pJg->GetSolidPartObject());
		}
		else if(pLinePartPara->start0_end1==1&&pJg->desEndOdd.m_iOddCalStyle==1)
		{
			pJg->CalEndOddment();
			pJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pJg->GetSolidPartObject());
		}
		if(dlg.m_bEndLjJg&&bInsideJg&&boltSet.GetNodeNum()>0&&m_pBasePart->IsAngle()&&m_pPlank->GetThick()>0)	
		{	//端连接里铁射线角钢，需要添加螺栓垫板 wht 12-05-28
			//当射线角钢设计端的螺栓同时属于基准构件时，此时不需要螺栓垫板 wxc-15-6-12
			//当射线角钢设计端的钢板交于基准角钢的内侧，此时不需要螺栓垫板 wxc-17-3-3
			CLDSLineAngle* pCutJg=(CLDSLineAngle*)m_pBasePart.LineAnglePointer();
			if(hGroupCorssAngle>0X20)	//基准角钢为组合角钢时，需要确定进行切割的子角钢
				pCutJg=(CLDSLineAngle*)console.FromPartHandle(hGroupCorssAngle,CLS_LINEANGLE);
			for(CLDSBolt* pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
			{	//清除在切割角钢上的螺栓(螺栓不可能共存于垫板和切割角钢上)
				if(pCutJg&&pCutJg->FindLsByHandle(pBolt->handle))
					boltSet.DeleteCursor();
			}
			boltSet.Clean();
			if(boltSet.GetNodeNum()>0&&xDesLjbPara.bAtAngleOuter)
			{	//根据间隙值判断是否添加垫板还是垫圈	wxc-16.8.4
				CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
				CLDSBolt* pFistBolt=boltSet.GetFirst();
				int nType=pFistBolt->CalPadPara(m_pBasePart->GetThick());
				if( nType==DIANQUAN_STRU::DIANQUAN &&boltSet.Count==1&&
					AfxMessageBox("间隙处是添加螺栓垫圈[Y],还是添加螺栓垫板[N]?", MB_YESNO)==IDYES)	
				{	//设计垫圈
					PARTSET partset;
					int nNum = pFistBolt->DianQuan.N, nThick = pFistBolt->DianQuan.thick;
					for (CLDSBolt* pBolt = boltSet.GetFirst(); pBolt; pBolt = boltSet.GetNext())
					{
						pView->GetBoltActiveRelaPartSet(pBolt, partset, NULL);
						pBolt->DianQuan.N = nNum;
						pBolt->DianQuan.thick = nThick;
						pBolt->CalGasketOffset(partset);
					}
				}
				else //if(nType==DIANQUAN_STRU::DIANBAN||boltSet.Count>1)		//设计垫板
				{	//设计垫板，需清空螺栓垫圈信息(据宝鸡段炜反馈两颗螺栓一定是垫板设计 wjh-2019.9.2)
					pFistBolt->DianQuan.thick=pFistBolt->DianQuan.N=0;
					pView->DesignBoltPad(pJg,pCutJg,boltSet);
				}
			}
		}
		if(!bBasePart)	//定位基准杆件在此处不计算切角切肢信息
		{	//此处应根据句柄计算两根切割基准杆件，不能只计算第一根，否则m_hRefPart2将被设置为0
			//导致生成节点板时计算出的正负头与，布置角钢螺栓时自动计算的正负头不一致 wht 13-04-27
			CLDSLineAngle *pCutAngle1=NULL,*pCutAngle2=NULL;
			if(pLinePartPara->start0_end1==0)
			{
				pCutAngle1=(CLDSLineAngle*)console.FromPartHandle(pJg->desStartOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				pCutAngle2=(CLDSLineAngle*)console.FromPartHandle(pJg->desStartOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pCutAngle1||pCutAngle2)
					pJg->CalCutAngleInfo(pCutAngle1,pCutAngle2,true,NULL,ftoi(pJg->desStartOdd.m_fCollideDist));
			}
			else
			{
				pCutAngle1=(CLDSLineAngle*)console.FromPartHandle(pJg->desEndOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				pCutAngle2=(CLDSLineAngle*)console.FromPartHandle(pJg->desEndOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pCutAngle1||pCutAngle2)
					pJg->CalCutAngleInfo(pCutAngle1,pCutAngle2,false,NULL,ftoi(pJg->desEndOdd.m_fCollideDist));
			}
		}
	}
	else
	{
		m_bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
		throw "design cancelled halfway";
#else
		throw "设计中途取消";
#endif
	}
	if(pBackToBackJg)
	{	
		//将背对背的子角钢添加到板射线杆件列表中
		CDesignLjPartPara* pBackToBackPara=m_pPlank->designInfo.partList.Add(pBackToBackJg->handle);
		pBackToBackPara->hPart=pBackToBackJg->handle;
		pBackToBackPara->m_nClassTypeId=CLS_LINEANGLE;
		pBackToBackPara->iFaceNo=iFaceNo;
		pBackToBackPara->angle.bEndLjJg=(end_lj_jgset.FromHandle(pBackToBackPara->hPart)!=NULL);
		if(pLinePartPara->angle.cur_wing_x0_y1==0)
			pBackToBackPara->angle.cur_wing_x0_y1=1;
		else if(pLinePartPara->angle.cur_wing_x0_y1==1)
			pBackToBackPara->angle.cur_wing_x0_y1=0;
		pBackToBackPara->start0_end1=pLinePartPara->start0_end1;
	}
	g_pSolidDraw->SetEntSnapStatus(pJg->handle,FALSE);
	if(!bBasePart)
		m_pPrevPart = (CLDSLinePart*)pJg;
}

void CDesignJdb::DesignHuoQu()
{
	short i,x_wing0_y_wing1;
	//因火曲线在肢翼侧时不会考虑自动肢宽,故存在此附加偏移,故fHuoquDist,fHuoquSpace不一定相同 wjh-2016.11.06
	double fHuoquDist[2]={0}, fHuoquSpace[2]={0};
	for(i=0;i<2;i++)
	{
		f3dPoint wing_vec;
		if(design_huoqu[i].pBaseJg.IsNULL())
			break;
		f3dPoint huoqu_vec;
		f3dLine huoqu_line;
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)Ta.Parts.FromHandle(design_huoqu[i].pBaseJg->group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pGroupAngle)
			IsInsideJg(pGroupAngle,&x_wing0_y_wing1);
		else
			IsInsideJg(design_huoqu[i].pBaseJg,&x_wing0_y_wing1);
		if(pGroupAngle&&pGroupAngle->size_cSubClassType=='T'&&pGroupAngle->ciMasterIndex==x_wing0_y_wing1)
		{
			if(design_huoqu[i].pBaseJg->handle!=pGroupAngle->m_hDatumSonAngle)
				design_huoqu[i].pBaseJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupAngle->m_hDatumSonAngle,CLS_LINEANGLE);	//只有这一种情况没必要把组合角钢子角钢换为组合角钢
		}
		else if(pGroupAngle)	//其余情况选中任一根组合角钢的子角钢，都等效替换为选中组合角钢
			design_huoqu[i].pBaseJg=pGroupAngle;
		IsInsideJg(design_huoqu[i].pBaseJg,&x_wing0_y_wing1);
		if(x_wing0_y_wing1==0)
			wing_vec = design_huoqu[i].pBaseJg->GetWingVecX();
		else
			wing_vec = design_huoqu[i].pBaseJg->GetWingVecY();
		double wing_wide = design_huoqu[i].pBaseJg->GetWidth();
		
		fHuoquDist[i]=fHuoquSpace[i]=xDesLjbPara.nHuoQuDist[i];
		if(design_huoqu[i].pBaseJg->IsAngle())
		{
			if(IsBerHuoQuJg(i))
				fHuoquDist[i]=-xDesLjbPara.nHuoQuDist[i];
			else
				fHuoquDist[i]+=wing_wide;
		}
		if(design_huoqu[i].pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupJg=(CLDSGroupLineAngle*)Ta.FromPartHandle(design_huoqu[i].pBaseJg->handle);
			if(pGroupJg->size_cSubClassType=='T'&&pGroupJg->ciMasterIndex!=x_wing0_y_wing1)
			{	//T型组合角钢,且连接至1号角钢Y肢(即2号角钢X肢)
				if(IsBerHuoQuJg(i))
				{
					fHuoquDist[i] -=(pGroupJg->jg_space*0.5+wing_wide);
					fHuoquSpace[i]+=(pGroupJg->jg_space*0.5+wing_wide);
				}
				else
				{
					fHuoquDist[i] +=pGroupJg->jg_space*0.5;
					fHuoquSpace[i]+=pGroupJg->jg_space*0.5;
				}
			}
			else if(pGroupJg->size_cSubClassType!='T')
			{
				if(IsBerHuoQuJg(i))
				{
					fHuoquDist[i] -=(pGroupJg->jg_space*0.5+wing_wide);
					fHuoquSpace[i]+=(pGroupJg->jg_space*0.5+wing_wide);
				}
				else
				{
					fHuoquDist[i] += pGroupJg->jg_space*0.5;
					fHuoquSpace[i]+= pGroupJg->jg_space*0.5;
				}
			}
		}
		if(xDesLjbPara.bHuoQuPosByCtrlPt&&design_huoqu[i].pBaseJg!=m_pBasePart)
		{
			huoqu_line.startPt = design_huoqu[i].pBaseJg->pStart->Position(true);
			huoqu_line.endPt = design_huoqu[i].pBaseJg->pEnd->Position(true);
			huoqu_vec = huoqu_line.endPt-huoqu_line.startPt;
			if(LjPosInPart(design_huoqu[i].pBaseJg)==0)	//始端连接
			{
				huoqu_line.startPt = m_pBasePart.LineAnglePointer()->GetDatumPosBer(m_pBaseNode);
				//the last line code is not good for 酒杯井口	--wjh,2003.04.16
				//huoqu_line.startPt = design_huoqu[i].pBaseJg->GetStartDatumPos();
				huoqu_line.endPt =huoqu_vec+huoqu_line.startPt;
			}
			else
			{
				huoqu_line.endPt = m_pBasePart.LineAnglePointer()->GetDatumPosBer(m_pBaseNode);
				//the last line code is not good for 酒杯井口	--wjh,2003.04.16
				//huoqu_line.endPt = design_huoqu[i].pBaseJg->GetEndDatumPos();
				huoqu_line.startPt =huoqu_line.endPt-huoqu_vec;
			}
			f3dPoint start,end;
			start = huoqu_line.startPt+wing_vec*fHuoquDist[i];
			end = huoqu_line.endPt+wing_vec*fHuoquDist[i];
			Standarized(huoqu_vec);
			design_huoqu[i].pBaseJg->desStartPos.jgber_cal_style=1;
			design_huoqu[i].pBaseJg->desEndPos.jgber_cal_style=1;
			if(LjPosInPart(design_huoqu[i].pBaseJg)==0)
			{
				JGZJ jgzj;
				getjgzj(jgzj,m_pBasePart.LineAnglePointer()->GetWidth());
				start=start+huoqu_vec*jgzj.g;
				design_huoqu[i].pBaseJg->UnlockStartPos();
				design_huoqu[i].pBaseJg->SetStart(start);
				design_huoqu[i].pBaseJg->SetEnd(end);
				double dd=design_huoqu[i].pBaseJg->GetStartExtraOdd(x_wing0_y_wing1);
				f3dPoint vec=end-start;
				Standarized(vec);
				start+=dd*vec;
				design_huoqu[i].pBaseJg->SetStart(start);	//使该角钢一直延伸到主材心线上为止
			}
			else
			{
				JGZJ jgzj;
				getjgzj(jgzj,m_pBasePart.LineAnglePointer()->GetWidth());
				end=end-huoqu_vec*jgzj.g;
				design_huoqu[i].pBaseJg->UnlockEndPos();
				design_huoqu[i].pBaseJg->SetStart(start);
				design_huoqu[i].pBaseJg->SetEnd(end);
				double dd=design_huoqu[i].pBaseJg->GetEndExtraOdd(x_wing0_y_wing1);
				f3dPoint vec=start-end;
				Standarized(vec);
				end+=dd*vec;
				design_huoqu[i].pBaseJg->SetEnd(end);	//使该角钢一直延伸到主材心线上为止
			}
			m_pPlank->designInfo.huoquline_start[i].datum_pos_style=0;
			m_pPlank->designInfo.huoquline_start[i].SetPosition(huoqu_line.startPt);
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.hRod =design_huoqu[i].pBaseJg->handle;
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.hNode=m_pBaseNode->handle;
			m_pPlank->designInfo.huoquline_end[i].datum_pos_style=0;
			m_pPlank->designInfo.huoquline_end[i].SetPosition(huoqu_line.endPt);
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.hRod =design_huoqu[i].pBaseJg->handle;
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.hNode=m_pBaseNode->handle;
			
			//if(LjPosInPart(design_huoqu[i].pBaseJg)==0)	//始端连接
				design_huoqu[i].pBaseJg->LockStartPos();	//锁定起始点坐标
			//else
				design_huoqu[i].pBaseJg->LockEndPos();	//锁定终止点坐标
		}
		else
		{
			huoqu_line.startPt = design_huoqu[i].pBaseJg->Start();
			huoqu_line.endPt = design_huoqu[i].pBaseJg->End();
			m_pPlank->designInfo.huoquline_start[i].datum_pos_style=2;
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.hRod =design_huoqu[i].pBaseJg->handle;
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.hNode=m_pBaseNode->handle;
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.offset_wing=x_wing0_y_wing1;
			m_pPlank->designInfo.huoquline_end[i].datum_pos_style=2;
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.hRod =design_huoqu[i].pBaseJg->handle;
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.hNode=m_pBaseNode->handle;
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.offset_wing=x_wing0_y_wing1;
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_style=4;
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.wing_offset_style=4;
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.wing_offset_dist=fHuoquDist[i];
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.wing_offset_dist=fHuoquDist[i];
			m_pPlank->designInfo.huoquline_start[i].des_para.RODNODE.len_offset_dist=200;
			m_pPlank->designInfo.huoquline_end[i].des_para.RODNODE.len_offset_dist=-200;
			huoqu_line.startPt += wing_vec*fHuoquDist[i];
			huoqu_line.endPt += wing_vec*fHuoquDist[i];
		}
		coord_trans(huoqu_line.startPt,m_pPlank->ucs,FALSE);
		coord_trans(huoqu_line.endPt,m_pPlank->ucs,FALSE);
		huoqu_line.startPt.z=huoqu_line.endPt.z=0;
		m_pPlank->HuoQuLine[i].startPt = huoqu_line.startPt;
		m_pPlank->HuoQuLine[i].endPt = huoqu_line.endPt;
	}
	//只有双面板才有必要计算火曲线与基准角钢准线间的交汇点
	if(m_pPlank->face_N==2)
	{
		double px,py;
		f2dLine L1,L2;
		f3dPoint pt1,pt2,wing_norm,wing_vec;
		JGZJ jgzj;
		L1.startPt.x = m_pPlank->HuoQuLine[0].startPt.x;
		L1.startPt.y = m_pPlank->HuoQuLine[0].startPt.y;
		L1.endPt.x = m_pPlank->HuoQuLine[0].endPt.x;
		L1.endPt.y = m_pPlank->HuoQuLine[0].endPt.y;
		IsInsideJg(m_pBasePart.LineAnglePointer(),&x_wing0_y_wing1);

		if(x_wing0_y_wing1==0)
		{
			wing_vec = m_pBasePart.LineAnglePointer()->GetWingVecX();
			wing_norm= m_pBasePart.LineAnglePointer()->get_norm_x_wing();
		}
		else
		{
			wing_vec = m_pBasePart.LineAnglePointer()->GetWingVecY();
			wing_norm= m_pBasePart.LineAnglePointer()->get_norm_y_wing();
		}
		getjgzj(jgzj,m_pBasePart.LineAnglePointer()->GetWidth());
		if(m_pBasePart.LineAnglePointer()->m_bEnableTeG)
		{
			if(x_wing0_y_wing1==0)
				jgzj = m_pBasePart.LineAnglePointer()->xWingXZhunJu;
			else
				jgzj = m_pBasePart.LineAnglePointer()->xWingYZhunJu;
		}
		Add_Pnt(pt1,m_pBaseRod->Start(),wing_vec*jgzj.g);
		Add_Pnt(pt2,m_pBaseRod->End(),wing_vec*jgzj.g);
		coord_trans(pt1,m_pPlank->ucs,FALSE);
		coord_trans(pt2,m_pPlank->ucs,FALSE);
		L2.startPt.x = pt1.x;
		L2.startPt.y = pt1.y;
		L2.endPt.x   = pt2.x;
		L2.endPt.y   = pt2.y;
		//节点板基准角钢既为火曲线基准角钢
		if(design_huoqu[0].pBaseJg==m_pBasePart)
		{
			HuoQuJoint[0]=m_pBasePart.LineAnglePointer()->GetDatumPosBer(m_pBaseNode)+wing_vec*fHuoquDist[0];
			//此时两面火曲线交点重合
			HuoQuJoint[1] = HuoQuJoint[0];
		}
		else
		{
			int dd = Int2dll(L1,L2,px,py);
			if(dd==-1)//直线定义有误
#ifdef AFX_TARG_ENU_ENGLISH
				throw "bend line or angle line defined wrongly, data wrong";
			else if(dd==0)
				throw "bend line or angle line defined wrongly, data wrong";
#else
				throw "火曲线或角钢准线定义有误,数据错误";
			else if(dd==0)
				throw "火曲线与基准角钢准线平行,数据错误";
#endif
			else
			{
				HuoQuJoint[0].x = px;
				HuoQuJoint[0].y = py;
				HuoQuJoint[0].z = 0;
				coord_trans(HuoQuJoint[0],m_pPlank->ucs,TRUE);
			}
			CLDSLineAngle *pJg,*pBerBaseJg = NULL;
			for(pJg=face2_jgset.GetFirst();pJg!=NULL;pJg=face2_jgset.GetNext())
			{
				int start0_end1 = LjPosInPart(pJg);
				if(start0_end1==0&&pJg->desStartPos.IsInDatumJgBer())	//楞线对齐
				{
					pBerBaseJg = pJg;
					break;
				}
				else if(start0_end1==1&&pJg->desEndPos.IsInDatumJgBer())	//楞线对齐
				{
					pBerBaseJg = pJg;
					break;
				}
			}
			if(pBerBaseJg)
			{
				f3dPoint axis_pt;
				getjgzj(jgzj,pBerBaseJg->GetWidth());
				IsInsideJg(pBerBaseJg,&x_wing0_y_wing1);
				if(pBerBaseJg->m_bEnableTeG)
				{
					if(x_wing0_y_wing1==0)
						jgzj = pBerBaseJg->xWingXZhunJu;
					else
						jgzj = pBerBaseJg->xWingYZhunJu;
				}
				//
				if(x_wing0_y_wing1==0)
				{
					wing_norm = pBerBaseJg->get_norm_y_wing();
					wing_vec  = pBerBaseJg->GetWingVecX();
				}
				else
				{
					wing_norm= pBerBaseJg->get_norm_x_wing();
					wing_vec = pBerBaseJg->GetWingVecY();
				}
				Add_Pnt(pt1,pBerBaseJg->Start(),wing_vec*jgzj.g);
				Add_Pnt(pt2,pBerBaseJg->End(),wing_vec*jgzj.g);
				coord_trans(pt1,m_pPlank->ucs,FALSE);
				coord_trans(pt2,m_pPlank->ucs,FALSE);
				pt1 = MutiFacePntToDatumFace(pt1,2);
				pt2 = MutiFacePntToDatumFace(pt2,2);
				
				L2.startPt.x = pt1.x;
				L2.startPt.y = pt1.y;
				L2.endPt.x   = pt2.x;
				L2.endPt.y   = pt2.y;
				f3dLine huoqu_line = m_pPlank->HuoQuLine[0];
				L1.startPt.Set(huoqu_line.startPt.x,huoqu_line.startPt.y);
				L1.endPt.Set(huoqu_line.endPt.x,huoqu_line.endPt.y);
				dd = Int2dll(L1,L2,px,py);
				if(dd==-1)//直线定义有误
#ifdef AFX_TARG_ENU_ENGLISH
					throw "bend line or angle line defined wrongly, data wrong";
				else if(dd==0)
					throw "bend line or angle line defined wrongly, data wrong";
#else
					throw "火曲线或角钢准线定义有误,数据错误";
				else if(dd==0)
					throw "火曲线与基准角钢准线平行,数据错误";
#endif
				else
				{
					HuoQuJoint[1].x = px;
					HuoQuJoint[1].y = py;
					HuoQuJoint[1].z = 0;
					coord_trans(HuoQuJoint[1],m_pPlank->ucs,TRUE);
				}
			}
			else
				HuoQuJoint[1] = HuoQuJoint[0];
		}
	}
//设计各面角钢的初步位置
	CLDSLineAngle *pJg;
	//设计火曲基准角钢的初步位置
	for(i=0;i<2;i++)
	{
		if(design_huoqu[i].pBaseJg.IsHasPtr())
		{
			if(!m_pBasePart.IsEqualPtr(design_huoqu[i].pBaseJg)&&xDesLjbPara.bHuoQuPosByCtrlPt)
			{	//此火曲基准角钢不是基准角钢且火曲线根据控制点定位
				IsInsideJg(design_huoqu[i].pBaseJg,&x_wing0_y_wing1);
				if(x_wing0_y_wing1==0)
				{
					design_huoqu[i].pBaseJg->desStartPos.wing_x_offset.gStyle = 4;
					design_huoqu[i].pBaseJg->desEndPos.wing_x_offset.gStyle   = 4;
					design_huoqu[i].pBaseJg->desStartPos.wing_x_offset.offsetDist=fHuoquDist[i];
					design_huoqu[i].pBaseJg->desEndPos.wing_x_offset.offsetDist	=fHuoquDist[i];
				}
				else
				{
					design_huoqu[i].pBaseJg->desStartPos.wing_y_offset.gStyle = 4;
					design_huoqu[i].pBaseJg->desEndPos.wing_y_offset.gStyle   = 4;
					design_huoqu[i].pBaseJg->desStartPos.wing_y_offset.offsetDist= fHuoquDist[i];
					design_huoqu[i].pBaseJg->desEndPos.wing_y_offset.offsetDist	= fHuoquDist[i];
				}
			}
			design_huoqu[i].pBaseJg->ClearFlag();
			design_huoqu[i].pBaseJg->UnlockStart();
			design_huoqu[i].pBaseJg->UnlockEnd();
			design_huoqu[i].pBaseJg->CalPosition();
		}
	}
	if(m_pPlank->face_N==2&&xDesLjbPara.iRayIntersStyle==1)
	{	//双面板射线交于火曲线上的情况需要重新设置第一面射线角钢的位置
		for(CLDSLinePart *pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
		{
			if(pLinePart&&(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pJg=(CLDSLineAngle*)pLinePart;
			else
				continue;
			if(pJg==design_huoqu[0].pBaseJg||m_pBasePart.IsEqualPtr(pJg))
				continue;
			pJg->ClearFlag();
			if(LjPosInPart(pJg)==0)
			{
				//pJg->des_start_pos.datum_pos_style = 15;
				pJg->desStartPos.SetToDatumPointStyle();
				pJg->desStartPos.datumPoint.SetPosition(HuoQuJoint[0]);
				pJg->CalPosition();
			}
			else
			{
				//pJg->des_end_pos.datum_pos_style = 15;
				pJg->desEndPos.SetToDatumPointStyle();
				pJg->desEndPos.datumPoint.SetPosition(HuoQuJoint[0]);
				pJg->CalPosition();
			}
		}
	}
	if(design_huoqu[0].pBaseJg.IsHasPtr())
	{
		for(CLDSLineTube *pLineTube=face2_tubeset.GetFirst();pLineTube;pLineTube=face2_tubeset.GetNext())
		{
			pLineTube->ClearFlag();
			int start0_end1_else2=LjPosInPart(pLineTube);
			JGZJ jgzj;
			getjgzj(jgzj,design_huoqu[0].pBaseJg->GetWidth());
			if(start0_end1_else2==0)		//始端连接
			{
				if(m_pPlank->face_N==2&&pLineTube->desStartPos.IsInDatumJgBer())
					continue;	//酒杯塔井口处
				if(design_huoqu[0].pBaseJg->m_bEnableTeG)
				{
					if(pLineTube->desStartPos.datum_pos_style==1||pLineTube->desStartPos.datum_pos_style==4)
						jgzj=design_huoqu[0].pBaseJg->xWingXZhunJu;
					else if(pLineTube->desStartPos.datum_pos_style==2||pLineTube->desStartPos.datum_pos_style==5)
						jgzj=design_huoqu[0].pBaseJg->xWingYZhunJu;
				}
				pLineTube->desStartPos.fEccentricDist=design_huoqu[0].pBaseJg->GetWidth()-jgzj.g+fHuoquDist[0];
			}
			else if(start0_end1_else2==1)	//终端连接
			{
				if(m_pPlank->face_N==2&&pLineTube->desEndPos.IsInDatumJgBer()&&xDesLjbPara.bHuoQuPosByCtrlPt)
					continue;	//火曲线由控制点定位
				if(design_huoqu[0].pBaseJg->m_bEnableTeG)
				{
					if(pLineTube->desEndPos.datum_pos_style==1||pLineTube->desEndPos.datum_pos_style==4)
						jgzj=design_huoqu[0].pBaseJg->xWingXZhunJu;
					else if(pLineTube->desEndPos.datum_pos_style==2||pLineTube->desEndPos.datum_pos_style==5)
						jgzj=design_huoqu[0].pBaseJg->xWingYZhunJu;
				}
				pLineTube->desEndPos.fEccentricDist=design_huoqu[0].pBaseJg->GetWidth()-jgzj.g+fHuoquDist[0];
			}
			pLineTube->CalPosition();
		}
		for(pJg=face2_jgset.GetFirst();pJg;pJg=face2_jgset.GetNext())
		{
			pJg->ClearFlag();
			int start0_end1_else2=LjPosInPart(pJg);
			if(start0_end1_else2==0)		//始端连接
			{
				//酒杯塔井口处的搭接基准角钢一般不是组合角钢(如果基准角钢是组合角钢且角钢基点在基准角钢楞线上,此构造为横担处连接） wxc-2016.11.17
				if(m_pPlank->face_N==2&&pJg->desStartPos.IsInDatumJgBer()&&(design_huoqu[0].pBaseJg->GetClassTypeId()!=CLS_GROUPLINEANGLE
					&&design_huoqu[0].pBaseJg->group_father_jg_h<0x20))
					continue;	//酒杯塔井口处
				if(xDesLjbPara.iRayIntersStyle==1)//射线交于火曲线
				{
					//pJg->desStartPos.datum_pos_style=15;
					pJg->desStartPos.SetToDatumPointStyle();
					pJg->desStartPos.datumPoint.SetPosition(HuoQuJoint[1]);
					if(pJg->desStartPos.jgber_cal_style==2)	//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定模式 wjh-2016.3.24
						pJg->desStartPos.bUdfDatumPointMode=TRUE;
				}
				else
				{	//这里可能还有问题 WJH-2005.03.03
					if(xDesLjbPara.iRayIntersStyle==0)//射线交于基准角钢预定准线上
					{
						if(pJg->desStartPos.spatialOperationStyle==1)
							pJg->desStartPos.spatialOperationStyle= 7;
						else if(pJg->desStartPos.spatialOperationStyle==4)
							pJg->desStartPos.spatialOperationStyle= 10;
						else if(pJg->desStartPos.spatialOperationStyle==2)
							pJg->desStartPos.spatialOperationStyle=8;
						else if(pJg->desStartPos.spatialOperationStyle==5)
							pJg->desStartPos.spatialOperationStyle=11;
						else if(pJg->desStartPos.spatialOperationStyle==3)
							pJg->desStartPos.spatialOperationStyle=9;
						else if(pJg->desStartPos.spatialOperationStyle==6)
							pJg->desStartPos.spatialOperationStyle=12;
						//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于火曲线模式 wjh-2016.3.24
						if(pJg->desStartPos.jgber_cal_style==2&&pJg->desStartPos.IsInHuoQuLine())
							pJg->desStartPos.ciDestLineMode=CMultiOffsetPos::TO_BENDLINE;
					}
					else if(xDesLjbPara.iRayIntersStyle==2)//射线交于基准角钢偏移准线上
					{
						if(pJg->desStartPos.IsInDatumJgWingX())
							pJg->desStartPos.spatialOperationStyle = 13;
						else if(pJg->desStartPos.IsInDatumJgWingY())
							pJg->desStartPos.spatialOperationStyle = 14;
						//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于火曲线模式 wjh-2016.3.24
						if(pJg->desStartPos.jgber_cal_style==2&&pJg->desStartPos.IsInOffsetZhunLine())
							pJg->desStartPos.ciDestLineMode=CMultiOffsetPos::TO_OFFSETZHUN;
					}
					pJg->desStartPos.datum_offset_dist=xDesLjbPara.fNormOffset;
					pJg->desStartPos.huoqu_dist = fHuoquSpace[0];
					pJg->desStartPos.offset_zhun = xDesLjbPara.nOffsetZhun[0];
				}
				pJg->desStartPos.huoqu_jg_h = design_huoqu[0].pBaseJg->handle;
				pJg->ClearFlag();
				//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定搭接肢模式 wjh-2016.3.24
				if(pJg->desStartPos.jgber_cal_style==2)
					pJg->desStartPos.bUdfLappedDatumWing=TRUE;
				pJg->CalPosition();
			}
			else if(start0_end1_else2==1)	//终端连接
			{
				//酒杯塔井口处的搭接基准角钢一般不是组合角钢(如果基准角钢是组合角钢且角钢基点在基准角钢楞线上,此构造为横担处连接） wxc-2016.11.17
				if(m_pPlank->face_N==2&&pJg->desEndPos.IsInDatumJgBer()&&design_huoqu[0].pBaseJg->GetClassTypeId()!=CLS_GROUPLINEANGLE)
					continue;	//火曲线由控制点定位
				if(xDesLjbPara.iRayIntersStyle==1)//射线交于火曲线
				{
					//pJg->des_end_pos.datum_pos_style=15;	//直接指定基点坐标
					pJg->desEndPos.SetToDatumPointStyle();
					pJg->desEndPos.datumPoint.SetPosition(HuoQuJoint[1]);
					if(pJg->desEndPos.jgber_cal_style==2)	//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定模式 wjh-2016.3.24
						pJg->desEndPos.bUdfDatumPointMode=TRUE;
				}
				else
				{
					if(xDesLjbPara.iRayIntersStyle==0)//射线交于基准角钢预定准线上
					{
						if(pJg->desEndPos.spatialOperationStyle==1)
							pJg->desEndPos.spatialOperationStyle= 7;
						else if(pJg->desEndPos.spatialOperationStyle==4)
							pJg->desEndPos.spatialOperationStyle= 10;
						else if(pJg->desEndPos.spatialOperationStyle==2)
							pJg->desEndPos.spatialOperationStyle=8;
						else if(pJg->desEndPos.spatialOperationStyle==5)
							pJg->desEndPos.spatialOperationStyle=11;
						else if(pJg->desEndPos.spatialOperationStyle==3)
							pJg->desEndPos.spatialOperationStyle=9;
						else if(pJg->desEndPos.spatialOperationStyle==6)
							pJg->desEndPos.spatialOperationStyle=12;
						//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于火曲线模式 wjh-2016.3.24
						if(pJg->desEndPos.jgber_cal_style==2&&pJg->desEndPos.IsInHuoQuLine())
							pJg->desEndPos.ciDestLineMode=CMultiOffsetPos::TO_BENDLINE;
					}
					else if(xDesLjbPara.iRayIntersStyle==2)//射线交于基准角钢偏移准线上
					{
						if(pJg->desEndPos.IsInDatumJgWingX())
							pJg->desEndPos.spatialOperationStyle = 13;
						else if(pJg->desEndPos.IsInDatumJgWingY())
							pJg->desEndPos.spatialOperationStyle = 14;
						//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于火曲线模式 wjh-2016.3.24
						if(pJg->desEndPos.jgber_cal_style==2&&pJg->desEndPos.IsInOffsetZhunLine())
							pJg->desEndPos.ciDestLineMode=CMultiOffsetPos::TO_OFFSETZHUN;
					}
					pJg->desEndPos.datum_offset_dist=xDesLjbPara.fNormOffset;
					pJg->desEndPos.huoqu_dist = fHuoquSpace[0];
					pJg->desEndPos.offset_zhun = xDesLjbPara.nOffsetZhun[0];
				}
				pJg->desEndPos.huoqu_jg_h = design_huoqu[0].pBaseJg->handle;
				pJg->ClearFlag();
				//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定搭接肢模式 wjh-2016.3.24
				if(pJg->desEndPos.jgber_cal_style==2)
					pJg->desEndPos.bUdfLappedDatumWing=TRUE;
				pJg->CalPosition();
			}
		}
	}
	for(pJg=face3_jgset.GetFirst();pJg;pJg=face3_jgset.GetNext())
	{	//三面板只允许射线角钢只允许交于基准角钢预定准线上
		pJg->ClearFlag();
		int start0_end1_else2=LjPosInPart(pJg);
		if(start0_end1_else2==0)		//始端连接
		{
			if(pJg->desStartPos.spatialOperationStyle==1)
				pJg->desStartPos.spatialOperationStyle= 7;
			else if(pJg->desStartPos.spatialOperationStyle==4)
				pJg->desStartPos.spatialOperationStyle= 10;
			else if(pJg->desStartPos.spatialOperationStyle==2)
				pJg->desStartPos.spatialOperationStyle=8;
			else if(pJg->desStartPos.spatialOperationStyle==5)
				pJg->desStartPos.spatialOperationStyle=11;
			else if(pJg->desStartPos.spatialOperationStyle==3)
				pJg->desStartPos.spatialOperationStyle=9;
			else if(pJg->desStartPos.spatialOperationStyle==6)
				pJg->desStartPos.spatialOperationStyle=12;
			pJg->desStartPos.huoqu_dist = fHuoquSpace[1];
			pJg->desStartPos.offset_zhun = 0;
			if(design_huoqu[1].pBaseJg.IsHasPtr())
				pJg->desStartPos.huoqu_jg_h = design_huoqu[1].pBaseJg->handle;
			pJg->ClearFlag();
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于火曲线模式 wjh-2016.3.24
			if(pJg->desStartPos.jgber_cal_style==2&&pJg->desStartPos.IsInHuoQuLine())
				pJg->desStartPos.ciDestLineMode=CMultiOffsetPos::TO_BENDLINE;
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定搭接肢模式 wjh-2016.3.24
			if(pJg->desStartPos.jgber_cal_style==2)
				pJg->desStartPos.bUdfLappedDatumWing=TRUE;
			pJg->CalPosition();
		}
		else if(start0_end1_else2==1)	//终端连接
		{
			if(pJg->desEndPos.spatialOperationStyle==1)
				pJg->desEndPos.spatialOperationStyle= 7;
			else if(pJg->desEndPos.spatialOperationStyle==4)
				pJg->desEndPos.spatialOperationStyle= 10;
			else if(pJg->desEndPos.spatialOperationStyle==2)
				pJg->desEndPos.spatialOperationStyle=8;
			else if(pJg->desEndPos.spatialOperationStyle==5)
				pJg->desEndPos.spatialOperationStyle=11;
			else if(pJg->desEndPos.spatialOperationStyle==3)
				pJg->desEndPos.spatialOperationStyle=9;
			else if(pJg->desEndPos.spatialOperationStyle==6)
				pJg->desEndPos.spatialOperationStyle=12;
			pJg->desEndPos.huoqu_dist = fHuoquSpace[1];
			pJg->desEndPos.offset_zhun = 0;
			if(design_huoqu[1].pBaseJg.IsHasPtr())
				pJg->desEndPos.huoqu_jg_h = design_huoqu[1].pBaseJg->handle;
			pJg->ClearFlag();
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于火曲线模式 wjh-2016.3.24
			if(pJg->desEndPos.jgber_cal_style==2&&pJg->desEndPos.IsInHuoQuLine())
				pJg->desEndPos.ciDestLineMode=CMultiOffsetPos::TO_BENDLINE;
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定搭接肢模式 wjh-2016.3.24
			if(pJg->desEndPos.jgber_cal_style==2)
				pJg->desEndPos.bUdfLappedDatumWing=TRUE;
			pJg->CalPosition();
		}
	}
}

void CDesignJdb::CalCurJgPreSetLsN(CLDSLineAngle *pCurAngle,BOOL bBasePart)
{
	int start0_end1_else2=LjPosInPart(pCurAngle);
	if(start0_end1_else2!=2&&m_pPlank->jdb_style!=7)		//不是中间连接不需计算
		return;
	else if(!bBasePart)//pCurAngle!=m_pBasePart)
		return;
	if(start0_end1_else2==2)
	{
		int ls_n = 0;
		int ls_d = 0;
		for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
		{
			CLDSLineAngle *pJg=NULL;
			if(pLinePart&&(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pJg=(CLDSLineAngle*)pLinePart;
			else
				continue;
			CConnectInfo ls_arr;
			if(pJg==NULL||pJg==pCurAngle)
				continue;
			int start0_end1=LjPosInPart(pJg);
			if(start0_end1==0)	//始端连接
				ls_arr = pJg->connectStart;
			else if(start0_end1==1)
				ls_arr = pJg->connectEnd;
			else
				ls_arr = pJg->connectMid;
			
			ls_d = max(ls_d,ls_arr.d);
			ls_n = max(ls_n,ls_arr.wnConnBoltN);
		}
		//李占岭说基准角钢上的螺栓数是射线角钢上最多螺栓数的1.5倍的圆整结果WJH--00.09.18
		//但我认为对于单向布孔的基准角钢上的螺栓数系数应为0.8左右  WJH--00.09.20
		//if(xDesJgPara.bMirLayLs)//对称布孔
		ls_n = (int)(ls_n*1.5+0.5);
		/*else				//单向布孔
		ls_n = max(ls_n, (int)(ls_n*0.8+0.5));*/
		pCurAngle->connectMid.d = ls_d;
		pCurAngle->connectMid.wnConnBoltN = ls_n;
		if(pCurAngle->connectMid.d==0)	//螺栓直径为0时默认使用角钢始端螺栓设计参数 wht 11-01-18
		{
			pCurAngle->connectMid.d=pCurAngle->connectStart.d;
			pCurAngle->connectMid.wnConnBoltN=1; //默认螺栓数位1
			pCurAngle->connectMid.rows=1;
		}
	}
}

void CDesignJdb::SortLinePartSet()
{
	int i,j,n;
	BOOL sorted;	//冒泡排序时的有序标识
	double temangle;
	CLDSLinePart* pTemPart;
	n = sum_partset.GetNodeNum();
	f3dLine line3d;
	f2dLine line2d;
	double *angle = new double[n];
	for(i=0;i<n;i++)
	{
		if(LjPosInPart(sum_partset[i])==0)
			line3d.endPt = sum_partset[i]->End();
		else
			line3d.endPt = sum_partset[i]->Start();
		coord_trans(line3d.endPt,m_pPlank->ucs,FALSE);
		line2d.endPt.Set(line3d.endPt.x,line3d.endPt.y);
		angle[i] = Cal2dLineAng(line2d);
	}
	//使角钢集按逆时针排序排序
    for(i=0;i<n-1;i++)   //冒泡排序
    {
        for(j=0;j<n-i-1;j++)
        {
			sorted = TRUE;
            if(angle[j]>angle[j+1])
            {
                temangle = angle[j];
                angle[j]   = angle[j+1];
                angle[j+1] = temangle;

				pTemPart = sum_partset[j];
				sum_partset[j] = sum_partset[j+1];
				sum_partset[j+1] = pTemPart;
				sorted = FALSE;
            }
        }
    }
	delete []angle;
}

long CDesignJdb::CalJdbThick()
{
	CMaxDouble max_thick(0),second_thick(0);
	CSuperSmartPtr<CLDSLinePart>pLinePart;
	//if(m_pBasePart->GetClassTypeId()==CLS_LINETUBE)
	if(!m_pBasePart->IsAngle())
		return 10;
	BOOL bGroupJg=FALSE;
	for(pLinePart=partset.GetFirst();pLinePart.IsHasPtr();pLinePart=partset.GetNext())
	{
		if(pLinePart==m_pBasePart)
			continue;	//不以基准角钢的肢厚为基准
		if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		if(max_thick.number<pLinePart.LineAnglePointer()->GetThick())
		{
			second_thick=max_thick;
			max_thick.Update(pLinePart.LineAnglePointer()->GetThick(),pLinePart.LinePartPointer());
		}
		else if(max_thick.number==pLinePart.LineAnglePointer()->GetThick())
			second_thick=pLinePart.LineAnglePointer()->GetThick();
		else if(second_thick.number<pLinePart.LineAnglePointer()->GetThick())
			second_thick.Update(pLinePart.LineAnglePointer()->GetThick(),pLinePart.LineAnglePointer());
	}
	for(pLinePart=face2_jgset.GetFirst();pLinePart.IsHasPtr();pLinePart=face2_jgset.GetNext())
	{
		if(max_thick.number<pLinePart.LineAnglePointer()->GetThick())
		{
			second_thick=max_thick;
			max_thick.Update(pLinePart.LineAnglePointer()->GetThick(),pLinePart.LineAnglePointer());
		}
		else if(max_thick.number==pLinePart.LineAnglePointer()->GetThick())
			second_thick=pLinePart.LineAnglePointer()->GetThick();
		else if(second_thick.number<pLinePart.LineAnglePointer()->GetThick())
			second_thick.Update(pLinePart.LineAnglePointer()->GetThick(),pLinePart.LineAnglePointer());
	}
	for(pLinePart=face3_jgset.GetFirst();pLinePart.IsHasPtr();pLinePart=face3_jgset.GetNext())
	{
		if(max_thick.number<pLinePart.LineAnglePointer()->GetThick())
		{
			second_thick=max_thick;
			max_thick.Update(pLinePart.LineAnglePointer()->GetThick(),pLinePart.LineAnglePointer());
		}
		else if(max_thick.number==pLinePart.LineAnglePointer()->GetThick())
			second_thick=pLinePart.LineAnglePointer()->GetThick();
		else if(second_thick.number<pLinePart.LineAnglePointer()->GetThick())
			second_thick.Update(pLinePart.LineAnglePointer()->GetThick(),pLinePart.LineAnglePointer());
	}
	if(m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		bGroupJg=TRUE;
		second_thick.number=((CLDSGroupLineAngle*)m_pBasePart.LineAnglePointer())->jg_space-g_sysPara.m_nGroupJgMachiningGap;
		second_thick.m_pRelaObj=m_pBasePart.LinePartPointer();
	}
	else if(m_pBasePart.LineAnglePointer()->group_father_jg_h>0x20)//IsSubJgType(TA_JG_GROUPSON))
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(m_pBasePart.LineAnglePointer()->group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pGroupJg)
		{
			bGroupJg=TRUE;
			second_thick.number=pGroupJg->jg_space;
			second_thick.m_pRelaObj=pGroupJg;
		}
	}
	/*	wxc-2016.4.5
	对于组合角钢中的钢板板厚不可以超过角钢组合间隙
	对于普通角钢，板厚应比射线材中最厚值大一个等级且取偶数
	*/
	if(second_thick.number==0)	//单射线连接板
		second_thick=max_thick;
	if(bGroupJg==FALSE)
	{
		second_thick.number+=2+ftoi(second_thick.number)%2;
		second_thick=max(second_thick.number,6);	//最小板厚必须保证6mm
	}
	if(second_thick.m_pRelaObj)
		m_pPlank->cMaterial=((CLDSLinePart*)second_thick.m_pRelaObj)->cMaterial;
	return ftol(second_thick.number);
}

/*static void JgToJgState(JG_STATE_STRU *pJgState,CLDSLineAngle *pJg)
{
	int i,j;
	pJgState->start_pos = pJg->desStartPos;
	pJgState->end_pos = pJg->desEndPos;
	pJgState->start_ber = pJg->Start();
	pJgState->end_ber   = pJg->End();
	pJgState->start_oddment		= pJg->startOdd();
	pJgState->end_oddment		= pJg->endOdd();
	pJgState->handle			= pJg->handle;
	for(i=0;i<2;i++)
	{
		for(j=0;j<3;j++)
			pJgState->cut_wing_para[i][j] = pJg->cut_wing_para[i][j];
	}
	pJgState->cut_wing[0] = pJg->cut_wing[0];
	pJgState->cut_wing[1] = pJg->cut_wing[1];
	for(i=0;i<4;i++)
	{
		for(j=0;j<2;j++)
			pJgState->cut_angle[i][j]	= pJg->cut_angle[i][j];
	}
}

static BOOL JgStateToJg(CLDSLineAngle *pJg,JG_STATE_STRU *pJgState)
{
	int i,j;
	if(pJg==NULL||pJgState==NULL)
		return FALSE;
	pJg->desStartPos = pJgState->start_pos;
	pJg->desEndPos = pJgState->end_pos;
	pJg->SetStart(pJgState->start_ber);
	pJg->SetEnd(pJgState->end_ber);
	pJg->SetStartOdd(pJgState->start_oddment);
	pJg->SetEndOdd(pJgState->end_oddment);
	if(pJg->handle!=pJgState->handle)
		return FALSE;	//句柄不一致,场景恢复错误
	for(i=0;i<2;i++)
	{
		for(j=0;j<3;j++)
			pJg->cut_wing_para[i][j] = pJgState->cut_wing_para[i][j];
	}
	pJg->cut_wing[0] = pJgState->cut_wing[0];
	pJg->cut_wing[1] = pJgState->cut_wing[1];
	for(i=0;i<4;i++)
	{
		for(j=0;j<2;j++)
			pJg->cut_angle[i][j]	= pJgState->cut_angle[i][j];
	}
	return TRUE;
}*/

int CDesignJdb::GetJgInFaceI(CLDSLineAngle *pJg)
{
	if(m_pBasePart.IsEqualPtr(pJg))
		return 1;
	else if(partset.FromHandle(pJg->handle)||partset.FromHandle(pJg->group_father_jg_h))
		return 1;
	else if(face2_jgset.FromHandle(pJg->handle)||face2_jgset.FromHandle(pJg->group_father_jg_h))
		return 2;
	else if(face3_jgset.FromHandle(pJg->handle)||face3_jgset.FromHandle(pJg->group_father_jg_h))
		return 3;
	else if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE)  
	{	//通过子角钢得到父角钢所在面号
		for(int i=0;i<4;i++)
		{
			long hSonJg=pJg->son_jg_h[i];
			if(partset.FromHandle(hSonJg))
				return 1;
			else if(face2_jgset.FromHandle(hSonJg))
				return 2;
			else if(face3_jgset.FromHandle(hSonJg))
				return 3;
		}
		return 1;
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT," can not judge the junction surface of angle which is not in connecting angle group!");
#else
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"无法判断不在连接角钢集中的角钢所在连接面!");
#endif
		return 1;
	}
}

f3dPoint CDesignJdb::MutiFacePntToDatumFace(f3dPoint point,int face_i,BOOL bFromMuti/*=TRUE*/)
{
	double tem_angle;
	f3dPoint axis_vec,vice_vec;
	f3dLine axis;
	if(face_i<2)
		return point;

	//cal_angle_of_2vec的返回在0~π之间的角度
	tem_angle = cal_angle_of_2vec(m_pPlank->ucs.axis_z,m_pPlank->huoqufacenorm[face_i-2]);
	//两面法线叉积为其交线的矢量方向
	axis_vec = m_pPlank->huoqufacenorm[face_i-2]^m_pPlank->ucs.axis_z;
	axis = m_pPlank->HuoQuLine[face_i-2];

	if(!bFromMuti)	//反向旋转使多面板角钢恢复原来位置
		tem_angle = -tem_angle;
	coord_trans(axis.startPt,m_pPlank->ucs,TRUE);
	coord_trans(axis.endPt,m_pPlank->ucs,TRUE);
	Sub_Pnt(vice_vec,axis.endPt, axis.startPt);
	if(dot_prod(vice_vec,axis_vec)<0)
		tem_angle = -tem_angle;
	coord_trans(axis.startPt,m_pPlank->ucs,FALSE);
	coord_trans(axis.endPt,m_pPlank->ucs,FALSE);
	rotate_point_around_axis(point,tem_angle,axis);

	return point;
}

void CDesignJdb::QuitJgStartLsGroup(CLDSLineAngle *pJg, int quit_dist)
{
	CLsRef *pLsRef;
	f3dPoint vec,pt1,pt2,start,end,origin;
	int iFaceI = GetJgInFaceI(pJg);
	vec = pJg->End()-pJg->Start();

	if(iFaceI==1)	//第一面上的角钢
		project_point(vec,origin,m_pPlank->ucs.axis_z);
	else	//火曲面上的角钢
		project_point(vec,origin,m_pPlank->huoqufacenorm[iFaceI-2]);
	Standarized(vec);
	
	for(pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
	{
		if(m_pPlank->FindLsByHandle((*pLsRef)->handle))
		{
			f3dPoint oldpos = (*pLsRef)->ucs.origin;
			oldpos += vec*quit_dist;
			(*pLsRef)->ucs.origin=oldpos;
			(*pLsRef)->SetModified();
		}
	}
	vector_trans(vec,m_pPlank->ucs,FALSE);
	for(PROFILE_VERTEX *pVertex=m_pPlank->vertex_list.GetFirst();pVertex;pVertex=m_pPlank->vertex_list.GetNext())
	{
		if(pVertex->vertex.ID==pJg->handle)
			pVertex->vertex += vec*quit_dist;
	}
	vector_trans(vec,m_pPlank->ucs,TRUE);
	//pJg->connectStart.ber_ls_line.startPt+=vec*quit_dist;
	//pJg->connectStart.ber_ls_line.endPt+=vec*quit_dist;
	//pJg->connectStart.wing_ls_line.startPt+=vec*quit_dist;
	//pJg->connectStart.wing_ls_line.endPt+=vec*quit_dist;
	//pJg->connectStart.tail_ls_pos.z = 0;
}

void CDesignJdb::QuitJgEndLsGroup(CLDSLineAngle *pJg, int quit_dist)
{
	CLsRef *pLsRef;
	f3dPoint vec,pt1,pt2,start,end,origin;
	int iFaceI = GetJgInFaceI(pJg);
	vec = pJg->Start()-pJg->End();
	if(iFaceI==1)	//第一面上的角钢
		project_point(vec,origin,m_pPlank->ucs.axis_z);
	else	//火曲面上的角钢
		project_point(vec,origin,m_pPlank->huoqufacenorm[iFaceI-2]);
	Standarized(vec);

	for(pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
	{
		if(m_pPlank->FindLsByHandle((*pLsRef)->handle))
		{
			f3dPoint oldpos = (*pLsRef)->ucs.origin;
			oldpos += vec*quit_dist;
			(*pLsRef)->ucs.origin=oldpos;
			(*pLsRef)->SetModified();
		}
	}
	vector_trans(vec,m_pPlank->ucs,FALSE);
	for(PROFILE_VERTEX *pVertex=m_pPlank->vertex_list.GetFirst();pVertex;pVertex=m_pPlank->vertex_list.GetNext())
	{
		if(pVertex->vertex.ID==pJg->handle)
			pVertex->vertex += vec*quit_dist;
	}
	vector_trans(vec,m_pPlank->ucs,TRUE);
	//pJg->connectEnd.ber_ls_line.startPt+=vec*quit_dist;
	//pJg->connectEnd.ber_ls_line.endPt+=vec*quit_dist;
	//pJg->connectEnd.wing_ls_line.startPt+=vec*quit_dist;
	//pJg->connectEnd.wing_ls_line.endPt+=vec*quit_dist;
}

void CDesignJdb::DesRayJgEndPos(CLDSLineAngle *pCurAngle, double fNormOffset)
{
	if(pCurAngle==m_pBasePart||pCurAngle->handle==m_pBaseNode->hFatherPart)
		return;	//基准角钢不需要再次移动,后一条件新加未测试WJH-04/04/22
	//处理射线角钢为组合子角钢的情况，根据1号子角钢调整组合父角钢的定位信息
	CLDSLineAngle* pGroupFatherJg=NULL;
	if(pCurAngle->group_father_jg_h>0X20)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(pCurAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pGroupJg&&pGroupJg->m_hDatumSonAngle==pCurAngle->handle)
			pGroupFatherJg=pGroupJg;
	}
	//
	BOOL bInsideJg;
	short x_wing0_y_wing1;
	bInsideJg = IsInsideJg(pCurAngle,&x_wing0_y_wing1);
	pCurAngle->ClearFlag();
	CDesignLjPartPara *pLinePartPara=m_pPlank->designInfo.partList.GetTail();
	if(LjPosInPart(pCurAngle)==0)	//始端连接
	{
		if(x_wing0_y_wing1==0)
		{
			pCurAngle->desStartPos.wing_x_offset.gStyle=4;
			pCurAngle->desStartPos.wing_x_offset.offsetDist = fNormOffset;
			if(pGroupFatherJg)
			{
				pGroupFatherJg->desStartPos.wing_x_offset.gStyle=4;
				pGroupFatherJg->desStartPos.wing_x_offset.offsetDist=fNormOffset;
			}
		}
		else
		{
			pCurAngle->desStartPos.wing_y_offset.gStyle=4;
			pCurAngle->desStartPos.wing_y_offset.offsetDist = fNormOffset;
			if(pGroupFatherJg)
			{
				pGroupFatherJg->desStartPos.wing_y_offset.gStyle=4;
				pGroupFatherJg->desStartPos.wing_y_offset.offsetDist=fNormOffset;
			}
		}
		if(pCurAngle->desStartPos.jgber_cal_style==2)
		{
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于基准心线模式 wjh-2016.3.24
			if(pCurAngle->desStartPos.spatialOperationStyle>=1&&pCurAngle->desStartPos.spatialOperationStyle<=6)
				pCurAngle->desStartPos.ciDestLineMode=CMultiOffsetPos::TO_AXISLINE;
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定搭接肢模式 wjh-2016.3.24
			pCurAngle->desStartPos.bUdfLappedDatumWing=TRUE;
		}
		pCurAngle->CalPosition();
	}
	else	//终端连接
	{
		if(x_wing0_y_wing1==0)
		{
			pCurAngle->desEndPos.wing_x_offset.gStyle=4;
			pCurAngle->desEndPos.wing_x_offset.offsetDist = fNormOffset;
			if(pGroupFatherJg)
			{
				pGroupFatherJg->desEndPos.wing_x_offset.gStyle=4;
				pGroupFatherJg->desEndPos.wing_x_offset.offsetDist=fNormOffset;
			}
		}
		else
		{
			pCurAngle->desEndPos.wing_y_offset.gStyle=4;
			pCurAngle->desEndPos.wing_y_offset.offsetDist = fNormOffset;
			if(pGroupFatherJg)
			{
				pGroupFatherJg->desEndPos.wing_y_offset.gStyle=4;
				pGroupFatherJg->desEndPos.wing_y_offset.offsetDist=fNormOffset;
			}
		}
		if(pCurAngle->desEndPos.jgber_cal_style==2)
		{
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定交于基准心线模式 wjh-2016.3.24
			if(pCurAngle->desEndPos.spatialOperationStyle>=1&&pCurAngle->desEndPos.spatialOperationStyle<=6)
				pCurAngle->desEndPos.ciDestLineMode=CMultiOffsetPos::TO_AXISLINE;
			//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定搭接肢模式 wjh-2016.3.24
			pCurAngle->desEndPos.bUdfLappedDatumWing=TRUE;
		}
		pCurAngle->CalPosition();
	}
	//sprintf(pLinePartPara->datum_norm_offset.key_str,"%.0f",xDesLjbPara.fNormOffset);
	//感觉下面一行中的fabs(...)有问题，应该直接用，不知道当时出于什么考虑没有直接用，暂改回来看效果
	// wjh 2007/03/07
	//sprintf(pLinePartPara->norm_offset.key_str,"-%.0f",fabs(xDesJgPara.jg_norm_offset));
	//sprintf(pLinePartPara->norm_offset.key_str,"%.0f",xDesJgPara.jg_norm_offset);
}
//将角钢初始位置归位
double CDesignJdb::PresetRayJgEndPos(CLDSLineAngle *pCurAngle)
{
	if(m_pBasePart.IsNULL()||pCurAngle==m_pBasePart||pCurAngle->handle==m_pBaseNode->hFatherPart)
		return 0;	//基准角钢不需要再次移动
	double fNormOffset=0;
	short x_wing0_y_wing1,base_x_wing0_y_wing1;
	if(m_pBasePart->IsAngle())
		IsInsideJg(m_pBasePart.pAngle,&base_x_wing0_y_wing1);
	BOOL bInsideJg = IsInsideJg(pCurAngle,&x_wing0_y_wing1);
	pCurAngle->ClearFlag();
	bool bGroupLineAngle=false;	//记录基准杆件是否为组合角钢
	if(m_pBasePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//组合角钢
		CLDSGroupLineAngle* pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(pCurAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
		f3dPoint base_vec=(m_pBasePart.pAngle->End()-m_pBasePart.pAngle->Start()).normalized();
		f3dPoint line_vec=(pCurAngle->End()-pCurAngle->Start()).normalized();
		if(pGroupJg && fabs(base_vec*line_vec)>EPS_COS2)
			return 0;	//同一坡度内，处于接头的两组合角钢不进行肢法向偏移
		bGroupLineAngle=true;	//基准杆件为组合角钢
		if(m_pBasePart.pAngle->size_cSubClassType=='T')
		{
			if(m_pBasePart.pGroupAngle->ciMasterIndex!=base_x_wing0_y_wing1)
				bGroupLineAngle=false;
		}
		if(bGroupLineAngle)
		{
			if(bInsideJg&&end_lj_jgset.FromHandle(pCurAngle->handle))
				fNormOffset=-m_pBasePart.LineAnglePointer()->GetThick()-xDesLjbPara.plank_thick/2.0;
			else
				fNormOffset=-xDesLjbPara.plank_thick/2.0;
		}
		else
		{
			if(bInsideJg&&end_lj_jgset.FromHandle(pCurAngle->handle))
				fNormOffset=-m_pBasePart.LineAnglePointer()->GetThick();
			else if(!bInsideJg)
				fNormOffset=-xDesLjbPara.plank_thick;
		}
	}
	if(!bGroupLineAngle&&m_pBasePart->GetClassTypeId()==CLS_LINEANGLE)
	{	//单角钢(或视同单角钢处理)
		if(bInsideJg&&end_lj_jgset.FromHandle(pCurAngle->handle))//端连接
		{
			fNormOffset=-m_pBasePart.LineAnglePointer()->GetThick();
			if(!xDesLjbPara.bAtAngleOuter)	//节点板交于基准角钢里皮
				fNormOffset-=xDesLjbPara.plank_thick;
		}
		else if(bInsideJg)	//里铁
		{
			if(xDesLjbPara.bWeldToDatumJg)	//节点板焊接到基准角钢上 wht 10-06-07
				fNormOffset=-xDesLjbPara.plank_thick;
			else if(xDesLjbPara.bAtAngleOuter)	//节点板交于基准角钢外皮
				fNormOffset=0;
			else								//节点板交于基准角钢里皮
				fNormOffset=-(m_pBasePart.LineAnglePointer()->GetThick()+xDesLjbPara.plank_thick);
		}
		else			//外铁
		{
			if(xDesLjbPara.bWeldToDatumJg)	//节点板焊接到基准角钢上 wht 10-06-07
				fNormOffset=0;
			else if(xDesLjbPara.bAtAngleOuter)	//节点板交于基准角钢外皮
				fNormOffset=-xDesLjbPara.plank_thick;
			else								//节点板交于基准角钢里皮	
				fNormOffset=m_pBasePart.LineAnglePointer()->GetThick();
		}
	}
	else if(m_pBasePart->IsTube())
		fNormOffset=-0.5*m_pPlank->GetThick();	//基准构件为钢管时射线角钢均偏移半个板厚  wht 09-09-22
	int start0_end1_else2=LjPosInPart(pCurAngle);
	if(start0_end1_else2==0)	//始端连接
	{
		//基准杆件为单角钢且射线杆件IsInDatumJgBer() return 0; wht 10-03-18
		if(m_pBasePart->GetClassTypeId()!=CLS_LINETUBE&&(m_pBasePart->GetClassTypeId()!=CLS_GROUPLINEANGLE&&
			(m_pBasePart->GetClassTypeId()==CLS_LINEANGLE&&m_pBasePart.pAngle->group_father_jg_h<0x20))&&
			pCurAngle->desStartPos.IsInDatumJgBer())
			return 0;
		double extra_norm_offset=0;
		if(pCurAngle->desStartPos.IsInHuoQuLine()||pCurAngle->desStartPos.IsInOffsetZhunLine())
		{
			GEPOINT vDatumWorkWing;
			CLDSLineAngle* pBendBaseAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pCurAngle->desStartPos.huoqu_jg_h>0?pCurAngle->desStartPos.huoqu_jg_h:pCurAngle->pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pBendBaseAngle)
			{
				short bend_x_wing0_y_wing1=base_x_wing0_y_wing1;
				if(!m_pBasePart.IsEqualPtr(pBendBaseAngle))
					IsInsideJg(pBendBaseAngle,&bend_x_wing0_y_wing1);
				vDatumWorkWing=bend_x_wing0_y_wing1==0?pBendBaseAngle->vxWingNorm:pBendBaseAngle->vyWingNorm;
			}
			else if(m_pBasePart->IsAngle())
				vDatumWorkWing=base_x_wing0_y_wing1==0?m_pBasePart.pAngle->vxWingNorm:m_pBasePart.pAngle->vyWingNorm;
			extra_norm_offset=pCurAngle->desStartPos.datum_offset_dist;
			if( (x_wing0_y_wing1==0&&(vDatumWorkWing*pCurAngle->vxWingNorm)<0)||
				(x_wing0_y_wing1==1&&(vDatumWorkWing*pCurAngle->vyWingNorm)<0))
				extra_norm_offset*=-1;
		}
		if(x_wing0_y_wing1==0)
		{
			pCurAngle->desStartPos.wing_x_offset.gStyle=4;
			pCurAngle->desStartPos.wing_x_offset.offsetDist = fNormOffset-extra_norm_offset;
		}
		else
		{
			pCurAngle->desStartPos.wing_y_offset.gStyle=4;
			pCurAngle->desStartPos.wing_y_offset.offsetDist = fNormOffset-extra_norm_offset;
		}
		pCurAngle->UnlockStart();
		pCurAngle->CalPosition();
	}
	else if(start0_end1_else2==1)	//终端连接
	{
		//基准杆件为单角钢且射线杆件IsInDatumJgBer() return 0; wht 10-03-18
		if(m_pBasePart->GetClassTypeId()!=CLS_LINETUBE&&(m_pBasePart->GetClassTypeId()!=CLS_GROUPLINEANGLE&&
			(m_pBasePart->GetClassTypeId()==CLS_LINEANGLE&&m_pBasePart.pAngle->group_father_jg_h<0x20))&&
			pCurAngle->desEndPos.IsInDatumJgBer())
			return 0;	
		double extra_norm_offset=0;
		if(pCurAngle->desEndPos.IsInHuoQuLine()||pCurAngle->desEndPos.IsInOffsetZhunLine())
		{
			GEPOINT vDatumWorkWing;
			CLDSLineAngle* pBendBaseAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pCurAngle->desEndPos.huoqu_jg_h>0?pCurAngle->desEndPos.huoqu_jg_h:pCurAngle->pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(bGroupLineAngle&&pBendBaseAngle)
			{
				short bend_x_wing0_y_wing1=base_x_wing0_y_wing1;
				if(!m_pBasePart.IsEqualPtr(pBendBaseAngle))
					IsInsideJg(pBendBaseAngle,&bend_x_wing0_y_wing1);
				vDatumWorkWing=bend_x_wing0_y_wing1==0?pBendBaseAngle->vxWingNorm:pBendBaseAngle->vyWingNorm;
			}
			else if(m_pBasePart->IsAngle())
				vDatumWorkWing=base_x_wing0_y_wing1==0?m_pBasePart.pAngle->vxWingNorm:m_pBasePart.pAngle->vyWingNorm;
			extra_norm_offset=pCurAngle->desEndPos.datum_offset_dist;
			if( (x_wing0_y_wing1==0&&(vDatumWorkWing*pCurAngle->vxWingNorm)<0)||
				(x_wing0_y_wing1==1&&(vDatumWorkWing*pCurAngle->vyWingNorm)<0))
				extra_norm_offset*=-1;
		}
		if(x_wing0_y_wing1==0)
		{
			pCurAngle->desEndPos.wing_x_offset.gStyle=4;
			pCurAngle->desEndPos.wing_x_offset.offsetDist = fNormOffset-extra_norm_offset;
		}
		else
		{
			pCurAngle->desEndPos.wing_y_offset.gStyle=4;
			pCurAngle->desEndPos.wing_y_offset.offsetDist = fNormOffset-extra_norm_offset;
		}
		pCurAngle->CalPosition();
	}
	return fNormOffset;
}
double CDesignJdb::PresetRayFlatEndPos(CLDSLinePart* pCurFlatOrSlot)
{
	if(m_pBasePart.IsNULL()||pCurFlatOrSlot==m_pBasePart)
		return 0;
	SmartRodPtr pFlatOrSlot=pCurFlatOrSlot;
	double fNormOffset=-pCurFlatOrSlot->GetThick();
	int start0_end1_else2=LjPosInPart(pCurFlatOrSlot);
	if(start0_end1_else2==0)	//始端连接
		((CTubeEndPosPara*)pFlatOrSlot->DesStartPos())->fNormOffset=fNormOffset;
	else if(start0_end1_else2==1)	//终端连接
		((CTubeEndPosPara*)pFlatOrSlot->DesEndPos())->fNormOffset=fNormOffset;
	pFlatOrSlot->CalPosition();
	return fNormOffset;
}

//判断火曲线是否位于火曲基准角钢楞线一侧
BOOL CDesignJdb::IsBerHuoQuJg(int i)
{
	if(design_huoqu[i].pBaseJg.IsNULL())
#ifdef AFX_TARG_ENU_ENGLISH
		throw "the bending basic angle does not exist, data input wrong!";
#else
		throw "没有这个火曲基准角钢,数据输入错误!";
#endif
	CLDSLinePart *pRayPole=NULL;
	if(i==0)
	{
		if(face2_jgset.GetNodeNum()>0)
			pRayPole=face2_jgset.GetFirst();
		else if(face2_tubeset.GetNodeNum()>0)
			pRayPole=face2_tubeset.GetFirst();
	}
	else
		pRayPole=face3_jgset.GetFirst();
	if(pRayPole==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "lack of ray angle of bending plane, data input wrong!";
#else
		throw "缺少火曲面上的射线角钢,数据输入错误!";
#endif
	f3dPoint ray_vec;
	if(LjPosInPart(pRayPole)==0)
		ray_vec = pRayPole->End()-pRayPole->Start();
	else
		ray_vec = pRayPole->Start()-pRayPole->End();
	short x_wing0_y_wing1;
	f3dPoint wing_norm;
	IsInsideJg(design_huoqu[i].pBaseJg,&x_wing0_y_wing1);
	if(x_wing0_y_wing1==0)
		wing_norm = design_huoqu[i].pBaseJg->get_norm_y_wing();
	else
		wing_norm = design_huoqu[i].pBaseJg->get_norm_x_wing();
	if(wing_norm*ray_vec>0)
		return TRUE;	//火曲线位于火曲基准角钢楞线一侧
	else
		return FALSE;	//火曲线位于火曲基准角钢肢一侧
}
//计算端连接角钢两侧的首个基准角钢螺栓位置
void CDesignJdb::FirstMirLsPos(int ls_d, int initBoltSpace,double *head_offset_p, double *tail_offset_p)
{
	short x_wing0_y_wing1;
	int deita23;
	double ray_wing_dist;
	f3dLine ray_line;
	f3dPoint base_pos,base_extend_vec,ray_wing_vec;

	deita23 = GetDeita23ByLsDiameter(ls_d);
	IsInsideJg(m_pBasePart.LineAnglePointer(),&x_wing0_y_wing1);
	if(x_wing0_y_wing1==0)
		base_pos = m_pBasePart.LineAnglePointer()->GetDatumPosWingX(m_pBaseNode);
	else
		base_pos = m_pBasePart.LineAnglePointer()->GetDatumPosWingY(m_pBaseNode);
	int start0_end1_else2=LjPosInPart(m_pBaseRod);
	if(start0_end1_else2==0)	//始端连接
		base_extend_vec = m_pBaseRod->End()-m_pBaseRod->Start();
	else if(start0_end1_else2==1)	//终端连接
		base_extend_vec = m_pBaseRod->End()-m_pBaseRod->Start();
	else
		base_extend_vec = m_pBaseRod->Start()-m_pBaseRod->End();

	Standarized(base_extend_vec);
	double head_offset = initBoltSpace;
	double tail_offset =-initBoltSpace;
	for(CLDSLineAngle *pJg=end_lj_jgset.GetFirst();pJg;pJg=end_lj_jgset.GetNext())
	{
		IsInsideJg(pJg,&x_wing0_y_wing1);
		CLDSLineAngle *pStartFatherJg=(CLDSLineAngle*)console.FromPartHandle(pJg->pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CLDSLineAngle *pEndFatherJg = (CLDSLineAngle*)console.FromPartHandle(pJg->pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		char sError[200]="";
		if(pStartFatherJg==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sError,199,"0X%X node parent angle0X%X lost,design failure!",pJg->pStart->handle,pJg->pStart->hFatherPart);
#else
			_snprintf(sError,199,"0X%X节点父角钢0X%X丢失，设计失败！",pJg->pStart->handle,pJg->pStart->hFatherPart);
#endif
			throw sError;
		}
		if(pEndFatherJg==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sError,199,"0X%X node parent angle 0X%X lost,design failure!",pJg->pEnd->handle,pJg->pEnd->hFatherPart);
#else
			_snprintf(sError,199,"0X%X节点父角钢0X%X丢失，设计失败！",pJg->pEnd->handle,pJg->pEnd->hFatherPart);
#endif
			throw sError;
		}
		if(pJg->desStartPos.IsInDatumJgBer())
			ray_line.startPt = pStartFatherJg->GetDatumPosBer(pJg->pStart);
		else if(pJg->desStartPos.IsInDatumJgWingX())
			ray_line.startPt = pStartFatherJg->GetDatumPosWingX(pJg->pStart);
		else
			ray_line.startPt = pStartFatherJg->GetDatumPosWingY(pJg->pStart);
		if(pJg->desEndPos.IsInDatumJgBer())
			ray_line.endPt = pEndFatherJg->GetDatumPosBer(pJg->pEnd);
		else if(pJg->desEndPos.IsInDatumJgWingX())
			ray_line.endPt = pEndFatherJg->GetDatumPosWingX(pJg->pEnd);
		else
			ray_line.endPt = pEndFatherJg->GetDatumPosWingY(pJg->pEnd);
		JGZJ jgzj;
		getjgzj(jgzj,pJg->GetWidth());
		if(pJg->m_bEnableTeG)
		{
			if(x_wing0_y_wing1==0)
				jgzj = pJg->xWingXZhunJu;
			else
				jgzj = pJg->xWingYZhunJu;
		}
		if(x_wing0_y_wing1==1)
		{
			ray_wing_vec = pJg->GetWingVecX();
			if(LjPosInPart(pJg)==0)	//始端连接
			{
				if(pJg->desStartPos.wing_x_offset.gStyle==0)
					ray_wing_dist = jgzj.g;
				else if(pJg->desStartPos.wing_x_offset.gStyle==1)
					ray_wing_dist = jgzj.g1;
				else if(pJg->desStartPos.wing_x_offset.gStyle==2)
					ray_wing_dist = jgzj.g2;
				else if(pJg->desStartPos.wing_x_offset.gStyle==3)
					ray_wing_dist = jgzj.g3;
				else //if(pJg->desStartPos.wing_x_offset.gStyle==4)
					ray_wing_dist = pJg->desStartPos.wing_x_offset.offsetDist;
			}
			else
			{
				if(pJg->desEndPos.wing_x_offset.gStyle==0)
					ray_wing_dist = jgzj.g;
				else if(pJg->desEndPos.wing_x_offset.gStyle==1)
					ray_wing_dist = jgzj.g1;
				else if(pJg->desEndPos.wing_x_offset.gStyle==2)
					ray_wing_dist = jgzj.g2;
				else if(pJg->desEndPos.wing_x_offset.gStyle==3)
					ray_wing_dist = jgzj.g3;
				else //if(pJg->desEndPos.wing_x_offset.gStyle==4)
					ray_wing_dist = pJg->desEndPos.wing_x_offset.offsetDist;
			}
		}
		else
		{
			ray_wing_vec = pJg->GetWingVecX();
			if(LjPosInPart(pJg)==0)	//始端连接
			{
				if(pJg->desStartPos.wing_y_offset.gStyle==0)
					ray_wing_dist = jgzj.g;
				else if(pJg->desStartPos.wing_y_offset.gStyle==1)
					ray_wing_dist = jgzj.g1;
				else if(pJg->desStartPos.wing_y_offset.gStyle==2)
					ray_wing_dist = jgzj.g2;
				else if(pJg->desStartPos.wing_y_offset.gStyle==3)
					ray_wing_dist = jgzj.g3;
				else //if(pJg->desStartPos.wing_y_offset.gStyle==4)
					ray_wing_dist = pJg->desStartPos.wing_y_offset.offsetDist;
			}
			else
			{
				if(pJg->desEndPos.wing_y_offset.gStyle==0)
					ray_wing_dist = jgzj.g;
				else if(pJg->desEndPos.wing_y_offset.gStyle==1)
					ray_wing_dist = jgzj.g1;
				else if(pJg->desEndPos.wing_y_offset.gStyle==2)
					ray_wing_dist = jgzj.g2;
				else if(pJg->desEndPos.wing_y_offset.gStyle==3)
					ray_wing_dist = jgzj.g3;
				else //if(pJg->desEndPos.wing_y_offset.gStyle==4)
					ray_wing_dist = pJg->desEndPos.wing_y_offset.offsetDist;
			}
		}
		double head_dist,tail_dist;
		if(ray_wing_vec*base_extend_vec>0)
		{
			head_dist = pJg->GetWidth()-ray_wing_dist+deita23;
			tail_dist = ray_wing_dist+deita23;
		}
		else
		{
			head_dist = ray_wing_dist+deita23;
			tail_dist = pJg->GetWidth()-ray_wing_dist+deita23;
		}
		f3dPoint head_ls_pos = base_pos+base_extend_vec*head_offset;
		while(DistOf3dPtLine(head_ls_pos,ray_line.startPt,ray_line.endPt)<head_dist)
		{
			head_offset+=5;
			head_ls_pos = base_pos+base_extend_vec*head_offset;
		}
		f3dPoint tail_ls_pos = base_pos+base_extend_vec*tail_offset;
		while(DistOf3dPtLine(tail_ls_pos,ray_line.startPt,ray_line.endPt)<tail_dist)
		{
			tail_offset-=5;
			tail_ls_pos = base_pos+base_extend_vec*tail_offset;
		}
	}
	if(head_offset_p)
		*head_offset_p = head_offset;
	if(tail_offset_p)
		*tail_offset_p = tail_offset;
}

BOOL CDesignJdb::DesignOneBoltJdb()
{
	BOOL bRayJgInside[2];
	short x_wing0_y_wing1[2];
	double base_wing_wide, base_wing_thick,wing_wide[2],wing_thick[2];
	CLDSLineAngle *ray_jg[2]={NULL};
	f3dPoint ls_norm;
	//条件1:两射线角钢节点面上的肢法线方向相反
	if(partset[0]&&partset[0]->GetClassTypeId()==CLS_LINEANGLE&&partset[0]!=m_pBasePart)
		ray_jg[0]=(CLDSLineAngle*)partset[0];
	if(partset[1]&&partset[1]->GetClassTypeId()==CLS_LINEANGLE&&partset[1]!=m_pBasePart)
	{
		if(ray_jg[0]==NULL)
			ray_jg[0]=(CLDSLineAngle*)partset[1];
		else
			ray_jg[1]=(CLDSLineAngle*)partset[1];
	}
	if(partset[2]&&partset[2]->GetClassTypeId()==CLS_LINEANGLE&&partset[2]!=m_pBasePart)
	{
		if(ray_jg[0]==NULL)
			return FALSE;
		else if(ray_jg[1])
			return FALSE;
		else
			ray_jg[1]=(CLDSLineAngle*)partset[2];
	}
	if(ray_jg[0]==NULL||ray_jg[1]==NULL)
		return FALSE;
	base_wing_wide = m_pBasePart.LineAnglePointer()->GetWidth();
	base_wing_thick= m_pBasePart.LineAnglePointer()->GetThick();
	wing_wide[0] = ray_jg[0]->GetWidth();
	wing_thick[0]= ray_jg[0]->GetThick();
	wing_wide[1] = ray_jg[1]->GetWidth();
	wing_thick[1]= ray_jg[1]->GetThick();
	bRayJgInside[0] = IsInsideJg(ray_jg[0],&x_wing0_y_wing1[0]);
	bRayJgInside[1] = IsInsideJg(ray_jg[1],&x_wing0_y_wing1[1]);
	f3dPoint ls_pos;
	CMultiOffsetPos des_jg_pos;
	if(ray_jg[0]->pStart==m_pBaseNode)
	{
		if(ray_jg[0]->desStartPos.IsInDatumJgWingX())
			ls_pos = m_pBasePart.LineAnglePointer()->GetDatumPosWingX(m_pBaseNode);
		else if(ray_jg[0]->desStartPos.IsInDatumJgWingY())
			ls_pos = m_pBasePart.LineAnglePointer()->GetDatumPosWingY(m_pBaseNode);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "the position of ray angle connecting end is wrong, singular bolt design cannot be done";
#else
			throw "射线角钢连接端位置不正确，不能进行单螺栓设计";
#endif
		des_jg_pos = ray_jg[0]->desStartPos;
	}
	else
	{
		if(ray_jg[0]->desEndPos.IsInDatumJgWingX())
			ls_pos = m_pBasePart.LineAnglePointer()->GetDatumPosWingX(m_pBaseNode);
		else if(ray_jg[0]->desEndPos.IsInDatumJgWingY())
			ls_pos = m_pBasePart.LineAnglePointer()->GetDatumPosWingY(m_pBaseNode);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "the position of ray angle connecting end is wrong, singular bolt design cannot be done";
#else
			throw "射线角钢连接端位置不正确，不能进行单螺栓设计";
#endif
		des_jg_pos = ray_jg[0]->desEndPos;
	}
	
	CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
	pBolt->iSeg=m_pBasePart->iSeg;
	//根据角钢肢宽自动选择螺栓规格(指直径)
	if(ray_jg[0]->pStart==m_pBaseNode)
		pBolt->set_d(ray_jg[0]->connectStart.d);
	else //if(ray_jg[0]->pEnd==m_pBaseNode)
		pBolt->set_d(ray_jg[0]->connectEnd.d);
	if(ray_jg[1]->pStart==m_pBaseNode)
		pBolt->set_d(min(pBolt->get_d(),ray_jg[1]->connectStart.d));
	else //if(ray_jg[1]->pEnd==m_pBaseNode)
		pBolt->set_d(min(pBolt->get_d(),ray_jg[1]->connectEnd.d));
	pBolt->set_d(min(pBolt->get_d(),SelLsByWingWide(__min(wing_wide[0],wing_wide[1]))));
	if(pBolt->get_d()<20)
		pBolt->SetGrade("4.8");
	else if(pBolt->get_d()<24)
		pBolt->SetGrade("6.8");
	else// if(
		pBolt->SetGrade("8.8");
	//螺栓位置设计信息填充
	pBolt->des_base_pos.datumPoint.datum_pos_style=2;
	if(des_jg_pos.IsFaceOffset())	//平推IsFaceOffset比GetDatumType()==2多考虑点线平推等因素
	{
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=TRUE;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.vPlaneNormal=des_jg_pos.face_offset_norm;
	}
	else
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.bFaceOffset=FALSE;
	
	pBolt->CopyModuleInstanceInfo(m_pBasePart);
	//pBolt->cfgword=m_pBasePart->cfgword;	//调整螺栓配材号与基准构件配材号一致
	//pBolt->SetLayer(m_pBasePart->layer());
	pBolt->des_base_pos.hPart=m_pBasePart->handle;
	pBolt->des_base_pos.datumPoint.des_para.RODNODE.hRod  = m_pBasePart->handle;
	pBolt->des_base_pos.datumPoint.des_para.RODNODE.hNode = m_pBaseNode->handle;
	pBolt->des_base_pos.len_offset_dist=0;
	pBolt->des_base_pos.wing_offset_dist=0;
	pBolt->des_base_pos.direction=0;
	JGZJ jgzj;
	if(m_pBasePart.LineAnglePointer()->m_bEnableTeG)
	{
		if(des_jg_pos.IsInDatumJgWingX())
			jgzj = m_pBasePart.LineAnglePointer()->xWingXZhunJu;
		else
			jgzj = m_pBasePart.LineAnglePointer()->xWingYZhunJu;
	}
	else
	{
		getjgzj(jgzj,m_pBasePart.LineAnglePointer()->GetWidth());
		m_pBasePart.LineAnglePointer()->xWingXZhunJu = m_pBasePart.LineAnglePointer()->xWingYZhunJu = jgzj;
	}
	pBolt->des_base_pos.datumPoint.des_para.RODNODE.direction=0;
	pBolt->des_base_pos.datumPoint.des_para.RODNODE.len_offset_dist=0;
	if(des_jg_pos.IsInDatumJgWingX())
	{
		jgzj = m_pBasePart.LineAnglePointer()->xWingXZhunJu;
		if(m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style==0)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g;
		else if(m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style==1)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g1;
		else if(m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style==2)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g2;
		else if(m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style==3)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g3;
		else //if(m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style==4)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = (int)m_pBaseNode->xFatherAngleZhunJu.offset_X_dist;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing = pBolt->des_base_pos.offset_wing = 0;
	}
	else
	{
		jgzj = m_pBasePart.LineAnglePointer()->xWingYZhunJu;
		if(m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style==0)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g;
		else if(m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style==1)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g1;
		else if(m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style==2)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g2;
		else if(m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style==3)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = jgzj.g3;
		else //if(m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style==4)
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_dist = (int)m_pBaseNode->xFatherAngleZhunJu.offset_X_dist;
		pBolt->des_base_pos.datumPoint.des_para.RODNODE.offset_wing = pBolt->des_base_pos.offset_wing = 1;
	}

	pBolt->des_work_norm.norm_style=1;
	pBolt->des_work_norm.hVicePart = m_pBasePart->handle;
	if(des_jg_pos.IsInDatumJgWingX())
		pBolt->des_work_norm.norm_wing = 0;
	else
		pBolt->des_work_norm.norm_wing = 1;
	if(x_wing0_y_wing1==0)
	{
		if(m_pPlank->ucs.axis_z*m_pBasePart.LineAnglePointer()->get_norm_x_wing()>0)
			pBolt->des_work_norm.direction=0;
		else
			pBolt->des_work_norm.direction=1;
	}
	else
	{
		if(m_pPlank->ucs.axis_z*m_pBasePart.LineAnglePointer()->get_norm_y_wing()>0)
			pBolt->des_work_norm.direction=0;
		else
			pBolt->des_work_norm.direction=1;
	}
	if(x_wing0_y_wing1==0)
	{
		if(m_pPlank->ucs.axis_z*m_pBasePart.LineAnglePointer()->get_norm_x_wing()>0)
			pBolt->des_work_norm.direction=0;
		else
			pBolt->des_work_norm.direction=1;
	}
	else
	{
		if(m_pPlank->ucs.axis_z*m_pBasePart.LineAnglePointer()->get_norm_y_wing()>0)
			pBolt->des_work_norm.direction=0;
		else
			pBolt->des_work_norm.direction=1;
	}
	if(pBolt->des_work_norm.direction==0)
	{	//朝外
		if(bRayJgInside[0])
			_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",m_pBasePart->handle,ray_jg[0]->handle);
		else
			_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",m_pBasePart->handle,ray_jg[1]->handle);
	}
	else //if(pBolt->des_work_norm.direction==1)
	{	//朝内
		if(bRayJgInside[0])
			_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",ray_jg[0]->handle);
		else
			_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",ray_jg[1]->handle);
	}
	pBolt->AddL0Thick(m_pBasePart->handle,TRUE,TRUE);
	pBolt->AddL0Thick(ray_jg[0]->handle,TRUE,TRUE);
	pBolt->AddL0Thick(ray_jg[1]->handle,TRUE,TRUE);
	pBolt->correct_worknorm();
	pBolt->CalGuigeAuto();
	pBolt->correct_pos();
	pBolt->Create3dSolidModel();
	m_pPlank->AppendLsRef(pBolt->GetLsRef());
	g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	m_pBasePart->AppendLsRef(pBolt->GetLsRef());
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,pBolt->get_d());
	CDesignLjPartPara *pLjPart=m_pPlank->designInfo.partList.Add(m_pBasePart->handle);
	pLjPart->iFaceNo=1;
	pLjPart->hPart=m_pBasePart->handle;
	pLjPart->start0_end1=2;
	//pLjPart->angle.cur_wing_x0_y1
	for(int kk=0;kk<2;kk++)//设计两根射线角钢
	{
		pLjPart=m_pPlank->designInfo.partList.Add(ray_jg[kk]->handle);
		pLjPart->iFaceNo=1;
		pLjPart->hPart=ray_jg[kk]->handle;
		pLjPart->angle.cur_wing_x0_y1=(BYTE)x_wing0_y_wing1[kk];
		pLjPart->angle.bEndLjJg=TRUE;
		if(ray_jg[kk]->pStart==m_pBaseNode)
		{
			pLjPart->start0_end1=0;
			ray_jg[kk]->AppendStartLsRef(pBolt->GetLsRef());
			if(bRayJgInside[kk])
			{
				if(x_wing0_y_wing1[kk]==0)
				{
					ray_jg[kk]->desStartPos.wing_x_offset.gStyle=4;
					ray_jg[kk]->desStartPos.wing_x_offset.offsetDist=-m_pBasePart.LineAnglePointer()->GetThick();
				}
				else
				{
					ray_jg[kk]->desStartPos.wing_y_offset.gStyle=4;
					ray_jg[kk]->desStartPos.wing_y_offset.offsetDist=-m_pBasePart.LineAnglePointer()->GetThick();
				}
			}
			else
			{
				if(x_wing0_y_wing1[kk]==0)
				{
					ray_jg[kk]->desStartPos.wing_x_offset.gStyle=4;
					ray_jg[kk]->desStartPos.wing_x_offset.offsetDist=0;
				}
				else
				{
					ray_jg[kk]->desStartPos.wing_y_offset.gStyle=4;
					ray_jg[kk]->desStartPos.wing_y_offset.offsetDist=0;
				}
			}
			ray_jg[kk]->SetStartOdd(LsSpace.EndSpace);
			ray_jg[kk]->desStartOdd.m_iOddCalStyle=2;
		}
		else
		{
			pLjPart->start0_end1=1;
			ray_jg[kk]->AppendEndLsRef(pBolt->GetLsRef());
			if(bRayJgInside[kk])
			{
				if(x_wing0_y_wing1[kk]==0)
				{
					ray_jg[kk]->desEndPos.wing_x_offset.gStyle=4;
					ray_jg[kk]->desEndPos.wing_x_offset.offsetDist=-m_pBasePart.LineAnglePointer()->GetThick();
				}
				else
				{
					ray_jg[kk]->desEndPos.wing_y_offset.gStyle=4;
					ray_jg[kk]->desEndPos.wing_y_offset.offsetDist=-m_pBasePart.LineAnglePointer()->GetThick();
				}
			}
			else
			{
				if(x_wing0_y_wing1[kk]==0)
				{
					ray_jg[kk]->desEndPos.wing_x_offset.gStyle=4;
					ray_jg[kk]->desEndPos.wing_x_offset.offsetDist=0;
				}
				else
				{
					ray_jg[kk]->desEndPos.wing_y_offset.gStyle=4;
					ray_jg[kk]->desEndPos.wing_y_offset.offsetDist=0;
				}
			}
			ray_jg[kk]->SetEndOdd(LsSpace.EndSpace);
			ray_jg[kk]->desEndOdd.m_iOddCalStyle=2;
		}
		ray_jg[kk]->SetModified();
		ray_jg[kk]->ClearFlag();
		ray_jg[kk]->CalPosition();
		if(bRayJgInside[kk])
			ray_jg[kk]->CalCutAngleInfo(m_pBasePart.LineAnglePointer(),NULL,(ray_jg[kk]->pStart==m_pBaseNode),false);
		double scale_user2scr=g_pSolidOper->GetScaleUserToScreen();
		double sample_len=g_pSolidSet->GetArcSamplingLength();
		ray_jg[kk]->Create3dSolidModel(g_sysPara.bDisplayAllHole,scale_user2scr,sample_len);
		g_pSolidDraw->NewSolidPart(ray_jg[kk]->GetSolidPartObject());
	}
	g_pSolidDraw->ReleaseSnapStatus();
	m_pPlank->Thick=0;
	//console.DispPartSet.DeleteNode(m_pPlank->handle);//在显示集合中删除虚拟板
	//console.DeletePart(m_pPlank->handle);	//单螺栓连接不需要连接板
	//m_pPlank=NULL;
	return TRUE;
}

void CDesignJdb::Create3FaceRefAxis()
{
    //--------VVVVVVVVVV----计算角钢相对坐标系的三个坐标轴方向  Wjh--00.05.15-------
	f3dPoint huoqu_vec[2];
	CTmaPtrList<CLDSLineAngle,CLDSLineAngle*> tem_jgset[3];
	CLDSLineAngle*   pBaseHuoQuJg[2] = {NULL,NULL};
	UCS_STRU ucs;
	if(design_huoqu[0].pBaseJg==NULL||design_huoqu[1].pBaseJg==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "lack of bending angle";
#else
		throw "缺少火曲角钢";
#endif
	if(design_huoqu[0].pBaseJg->pStart == m_pBaseNode)
		Sub_Pnt(huoqu_vec[0],design_huoqu[0].pBaseJg->GetEndDatumPos(),design_huoqu[0].pBaseJg->GetStartDatumPos());
	else//(pBaseHuoQuJg[0]->pStart == pBaseNode)
		Sub_Pnt(huoqu_vec[0],design_huoqu[0].pBaseJg->GetStartDatumPos(),design_huoqu[0].pBaseJg->GetEndDatumPos());

	if(design_huoqu[1].pBaseJg->pStart == m_pBaseNode)
		Sub_Pnt(huoqu_vec[1],design_huoqu[1].pBaseJg->GetEndDatumPos(),design_huoqu[1].pBaseJg->GetStartDatumPos());
	else//(pBaseHuoQuJg[1]->pStart == pBaseNode)
		Sub_Pnt(huoqu_vec[1],design_huoqu[1].pBaseJg->GetStartDatumPos(),design_huoqu[1].pBaseJg->GetEndDatumPos());
	ucs.axis_z = huoqu_vec[0]^huoqu_vec[1];
	AssureNormDirect(ucs.axis_z);
	ucs.axis_y = huoqu_vec[1];
	Standarized(ucs.axis_y);
	Standarized(ucs.axis_z);
	ucs.axis_x = ucs.axis_y^ucs.axis_z;
	//基准面为两根火曲线间连接有射线角钢的区域(与焊接区(不连角钢)相对应
		//第二根火曲线的端点在第一根火曲线的左侧
	/*判断算法不严谨有错误的时候（尤其是变坡口三面板，上段为竖直段基面时） wjh-2013.2.2
	f3dPoint origin,ray_vec1,ray_vec2,ray_vec3;
	vector_trans(huoqu_vec[0],ucs,FALSE);
	vector_trans(huoqu_vec[1],ucs,FALSE);
	if(DistOf2dPtLine(huoqu_vec[1],origin,huoqu_vec[0])>0)
		;//第一根火曲线为基准面起始火曲线
	else
	{	//第二根火曲线为基准面起始火曲线
		
		HUOQU_LINE_STRU tem_huoqu = design_huoqu[0];
		design_huoqu[0] = design_huoqu[1];
		design_huoqu[1] = tem_huoqu;
		design_huoqu[0].pBaseJg = design_huoqu[0].pBaseJg;
		design_huoqu[1].pBaseJg = design_huoqu[1].pBaseJg;
		f3dPoint tem_vec = huoqu_vec[0];
		huoqu_vec[0] = huoqu_vec[1];
		huoqu_vec[1] = tem_vec;
	}
		
	//决定基准面以及两火曲面(逆时针方向)射线角钢集
	int i,jgset_no[3]={-1,-1,-1};
	double qu_ang1,qu_ang2,ang1,ang2,ang3;
	if(partset[0]&&(partset[0]->GetClassTypeId()==CLS_LINEANGLE||partset[0]->GetClassTypeId()==CLS_GROUPLINEANGLE))
	{
		if(partset[0]->pStart==m_pBaseNode)
			Sub_Pnt(ray_vec1,((CLDSLineAngle*)partset[0])->GetEndDatumPos(),((CLDSLineAngle*)partset[0])->GetStartDatumPos());
		else
			Sub_Pnt(ray_vec1,((CLDSLineAngle*)partset[0])->GetStartDatumPos(),((CLDSLineAngle*)partset[0])->GetEndDatumPos());
	}
	vector_trans(ray_vec1,ucs,FALSE);
	if(face2_jgset[0]->pStart==m_pBaseNode)
		Sub_Pnt(ray_vec2,face2_jgset[0]->GetEndDatumPos(),face2_jgset[0]->GetStartDatumPos());
	else
		Sub_Pnt(ray_vec2,face2_jgset[0]->GetStartDatumPos(),face2_jgset[0]->GetEndDatumPos());
	vector_trans(ray_vec2,ucs,FALSE);
	if(face3_jgset[0]->pStart==m_pBaseNode)
		Sub_Pnt(ray_vec3,face3_jgset[0]->GetEndDatumPos(),face3_jgset[0]->GetStartDatumPos());
	else
		Sub_Pnt(ray_vec3,face3_jgset[0]->GetStartDatumPos(),face3_jgset[0]->GetEndDatumPos());
	vector_trans(ray_vec3,ucs,FALSE);
	qu_ang1 = Cal2dLineAng(origin.x,origin.y,huoqu_vec[0].x,huoqu_vec[0].y);
	qu_ang2 = Cal2dLineAng(origin.x,origin.y,huoqu_vec[1].x,huoqu_vec[1].y);
	ang1 = Cal2dLineAng(origin.x,origin.y,ray_vec1.x,ray_vec1.y);
	ang2 = Cal2dLineAng(origin.x,origin.y,ray_vec2.x,ray_vec2.y);
	ang3 = Cal2dLineAng(origin.x,origin.y,ray_vec3.x,ray_vec3.y);
	//以第一火曲线为角度基准
	ang1 -= qu_ang1;
	ang2 -= qu_ang1;
	ang3 -= qu_ang1;
	qu_ang2 -= qu_ang1;
	if(ang1<0)
		ang1+=2*Pi;
	if(ang2<0)
		ang2+=2*Pi;
	if(ang3<0)
		ang3+=2*Pi;
	if(qu_ang2<0)
		qu_ang2+=2*Pi;
	if(ang1<=qu_ang2)	//第二条火曲线右侧为基准面(第一面)
		jgset_no[0] = 0;
	else if(ang2<=qu_ang2)
		jgset_no[1] = 0;
	else//(ang3<=qu_ang2)
		jgset_no[2] = 0;
	//第一火曲线右侧为第一火曲面(第二射线角钢集)
	if(jgset_no[0]==0)
	{
		if(ang2<ang3)
		{
			jgset_no[1] = 2;	//第二火曲面(第三射线角钢集)
			jgset_no[2] = 1;	//第一火曲面(第二射线角钢集)
		}
		else if(ang2>ang3)
		{
			jgset_no[1] = 1;	//第一火曲面(第二射线角钢集)
			jgset_no[2] = 2;	//第二火曲面(第三射线角钢集)
		}
		else
			throw "数据错误,无法划分火曲面角钢集";
	}
	else if(jgset_no[1]==0)
	{
		if(ang1<ang3)
		{
			jgset_no[0] = 2;
			jgset_no[2] = 1;
		}
		else if(ang1>ang3)
		{
			jgset_no[0] = 1;
			jgset_no[2] = 2;
		}
		else
			throw "数据错误,无法划分火曲面角钢集";
	}
	else //if(jgset_no[2]==0)
	{
		if(ang1<ang2)
		{
			jgset_no[0] = 2;
			jgset_no[1] = 1;
		}
		else if(ang2>ang3)
		{
			jgset_no[0] = 1;
			jgset_no[1] = 2;
		}
		else
			throw "数据错误,无法划分火曲面角钢集";
	}
	if(jgset_no[0]!=0)
	{
		for(i=0;i<partset.GetNodeNum();i++)
		{
			if(partset[i]&&(partset[i]->GetClassTypeId()==CLS_LINEANGLE||partset[i]->GetClassTypeId()==CLS_GROUPLINEANGLE))
				tem_jgset[jgset_no[0]].append((CLDSLineAngle*)partset[i]);
		}
	}
	if(jgset_no[1]!=1)
	{
		for(i=0;i<face2_jgset.GetNodeNum();i++)
			tem_jgset[jgset_no[1]].append(face2_jgset[i]);
	}
	if(jgset_no[2]!=2)
	{
		for(i=0;i<face3_jgset.GetNodeNum();i++)
			tem_jgset[jgset_no[2]].append(face3_jgset[i]);
	}
	if(jgset_no[0]!=0)
	{
		partset.Empty();
		for(i=0;i<tem_jgset[0].GetNodeNum();i++)
		{
			tem_jgset[0][i]->feature = tem_jgset[0][i]->feature%10+10;
			partset.append(tem_jgset[0][i]);
		}
	}
	if(jgset_no[1]!=1)
	{
		face2_jgset.Empty();
		for(i=0;i<tem_jgset[1].GetNodeNum();i++)
		{
			tem_jgset[1][i]->feature = tem_jgset[1][i]->feature%10+20;
			face2_jgset.append(tem_jgset[1][i]);
		}
	}
	if(jgset_no[2]!=2)
	{
		face3_jgset.Empty();
		for(i=0;i<tem_jgset[2].GetNodeNum();i++)
		{
			tem_jgset[2][i]->feature = tem_jgset[2][i]->feature%10+30;
			face3_jgset.append(tem_jgset[2][i]);
		}
	}*/
	CreateRefAxis();
}

bool CDesignJdb::AssureNormDirect(double* vNormCoords)
{
	f3dPoint norm(vNormCoords);
	Standarized(norm);
	bool reversed=true;
	if(m_nViewFlag==FRONT_VIEW&&norm.y<0)	//前面视图
		norm = -1.0*norm;
	else if(m_nViewFlag==BACK_VIEW&&norm.y>0)		//后面视图
		norm = -1.0*norm;
	else if(m_nViewFlag==RIGHT_VIEW&&norm.x<0)	//右面视图
		norm = -1.0*norm;
	else if(m_nViewFlag==LEFT_VIEW&&norm.x>0)	//左面视图
		norm = -1.0*norm;
	else //if(m_nViewFlag==RANDOM_VIEW)
	{//在任意面上设计连接板,此时连接板面法线方向应与任意面的当前视向一致
		if(dot_prod(console.GetActiveView()->ucs.axis_z,norm)<0)	//与视向反向
			norm = -1.0*norm;
		else
			reversed=false;
	}
	vNormCoords[0]=norm.x;
	vNormCoords[1]=norm.y;
	vNormCoords[2]=norm.z;
	if(reversed)
		m_pPlank->designInfo.norm.ReverseDirection();
	return reversed;
}
#endif