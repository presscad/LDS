//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "GUID.h"
#include "UI.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "database.h"
#include "DefinedView.h"
#include "image.h"
#include "lds_part.h"
#include "Query.h"
#include "DesignJoint.h"
#include "IntelligentConnectParaDlg.h"
#include "LibraryQuery.h"
#include "SortFunc.h"
#include "DianBanParaDlg.h"
#include "LogFile.h"
#include "SnapTypeVerify.h"
#include "UnifyModelLayer.h"
#ifdef __ANGLE_PART_INC_
#include "SingleRayEndBoltLj.h"
#include "CommonLjSmartDesign.h"
#include "GeneralNodeDesign.h"
#include "NewPartsListen.h"
#include "LdsLocale.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __ANGLE_PART_INC_
//1.设计交叉点单螺栓 
BOOL DesignIntersNode(CLDSNode *pNode,BOOL bThrowError,CDianBanParaDlg *pDlg=NULL);
//2.设计交叉点双螺栓
BOOL DesignIntersNode2Ls(CLDSNode *pNode, BOOL bThrowError);
//设计仅有螺栓的连接(无板)
BOOL SmartDesignBoltOnlyConnect(CLDSNode *pNode,BOOL bThrowError,BOOL bCanUndo/*=TRUE*/)
{
	CLDSLineAngle* pXieJg=NULL,*pXieJg2=NULL;
	if (pNode->m_cPosCalType==CLDSNode::COORD_INTERSECTION)
	{
		pXieJg = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[0],CLS_LINEANGLE);
		pXieJg2 = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[1],CLS_LINEANGLE);
		if (pXieJg == NULL || pXieJg2 == NULL)
			return FALSE;
		//处理单角钢处的交叉点
		f3dPoint view_norm;
		GetWorkNorm(pXieJg,pXieJg2,&view_norm);
		BYTE ciWorkWing1 = 0,ciWorkWing2 = 1;
		BOOL bInsideJg1 = IsInsideJg(pXieJg,view_norm,&ciWorkWing1);
		BOOL bInsideJg2 = IsInsideJg(pXieJg2,view_norm,&ciWorkWing2);
		if (bInsideJg1 != bInsideJg2)
		{	//两交叉角钢一里一外，进行螺栓连接
			LOCALE_PROPITEM* pLocaleItem=gxLocalizer.GetLocaleItem("2IntersBolt-MinAnlgeWidth");
			UINT uiMinWidth=pLocaleItem!=NULL?pLocaleItem->liVal:10000;
			BOOL designed=false;
			if (pXieJg->GetWidth() >= uiMinWidth && pXieJg2->GetWidth() >= uiMinWidth)
				designed=DesignIntersNode2Ls(pNode,bThrowError);	//设计双螺栓
			if (designed)
				return true;
			else
				return DesignIntersNode(pNode,bThrowError);	//设计单螺栓
		}
		else if (bInsideJg1 == bInsideJg2)
		{	//两交叉角钢同里或同外，进行打断处理
			//TODO:暂时不支持 wjh-2019.10.29
			return FALSE;
		}
	}
	//以下代码经查证原意是想在智能连接设计中整合融入角钢单螺栓设计的元素，但后续更新全部在DesignBoltOnlyConnect中进行，
	//故后续代码暂时无用，即使启用也应该是调用DesignBoltOnlyConnect(...)@PartDesign.cpp 来进行 wjh-2019.10.29
	CLDSLineAngle* pRayJgArr[2]={NULL};
	pRayJgArr[0]=pXieJg;
	pRayJgArr[1]=pXieJg2;
	JGZJ jgzj;
	CLDSBolt ls(&Ta),*pBolt;
	f3dPoint norm,direct,ls_pos;
	BOOL bInsideJg1,bInsideJg2;
	double len_offset1=0,len_offset2=0;
	int x_wing0_y_wing1;
	LSSPACE_STRU LsSpace;
	double wing_wide,wing_thick,base_wing_wide,base_wing_thick;
	if(pNode==NULL)
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "select illegal part error";
#else
			throw "选择非法构件错误";
#endif
		else
			return FALSE;
	}
	if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "no modify permission of the node";
#else
			throw "没有此节点的修改权限！";
#endif
		else
			return FALSE;
	}
	int i;
	for(i=0;i<2;i++)
	{
		if(pRayJgArr[i]==NULL)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "select illegal part error";
#else
				throw "选择非法构件错误";
#endif
			else
				return FALSE;
		}
		if(pRayJgArr[i]->pStart==NULL||pRayJgArr[i]->pEnd==NULL)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "the selected angle is short(starting or ending node is 0X0)！";
#else
				throw "所选角钢为短角钢(始端或终端节点为0X0)！";
#endif
			else
				return FALSE;
		}
		if(theApp.m_bCooperativeWork&&pNode==pRayJgArr[i]->pStart&&!theApp.IsHasModifyPerm(pRayJgArr[i]->dwStartPermission))
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "no modify permission of the angle starting!";
#else
				throw "没有此角钢始端的修改权限！";
#endif
			else 
				return FALSE;
		}
		else if(theApp.m_bCooperativeWork&&pNode==pRayJgArr[i]->pEnd&&!theApp.IsHasModifyPerm(pRayJgArr[i]->dwEndPermission))
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "no modify permission of the angle ending!";
#else
				throw "没有此角钢终端的修改权限！";
#endif
			else
				return FALSE;
		}
	}
	CLDSLineAngle *pFatherAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pNode->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pFatherAngle==NULL)
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "select a isolated node (the node without parent angle)";
#else
			throw "选择了一个孤立的节点(没有父角钢的节点)";
#endif
		else
			return FALSE;
	}
	CLDSGroupLineAngle *pGroupJg=NULL;	//节点父角钢
	if(pFatherAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//节点父杆件为组合角钢时自动选择合适的子角钢为单螺栓连接基准角钢 wht 10-11-23
		pGroupJg=(CLDSGroupLineAngle*)pFatherAngle;
		if(pGroupJg->group_style!=0)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "System only supports single bolt connection on child angle of diagonal group angles currently!";
#else
				throw "目前仅支持在对角组合角钢的子角钢上进行单螺栓连接!";
#endif
			else
				return FALSE;
		}
		f3dPoint ray_vec=pXieJg->Start()-pXieJg->End();
		if(pXieJg->pStart->handle==pNode->handle)
			ray_vec*=-1.0;
		normalize(ray_vec);
		CLDSLineAngle *pSonJg=NULL;
		GetWorkNorm(pGroupJg,pXieJg,&norm);	//工作面法线
		for(int i=0;i<4;i++)
		{
			if(pGroupJg->group_style<2&&i>=2)
				continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在 wht 11-08-05
			if(pGroupJg->son_jg_h[i]<0x20)
				continue;
			CLDSLineAngle *pJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
			if(pJg==NULL)
				continue;
			f3dPoint wing_vec;
			if(fabs(pJg->get_norm_x_wing()*norm)>fabs(pJg->get_norm_y_wing()*norm))
				wing_vec=pJg->GetWingVecX();
			else 
				wing_vec=pJg->GetWingVecY();
			if(wing_vec*ray_vec>0)
			{
				pSonJg=pJg;
				break;
			}
		}
		if(pSonJg)
			pFatherAngle=pSonJg;
		else 
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Basic point parent rod does not support the connection!";
#else
				throw "基点父杆件不支持该连接!";
#endif
			else
				return FALSE;
		}
	}
	g_pSolidDraw->SetEntSnapStatus(pNode->handle);
	g_pSolidDraw->SetEntSnapStatus(pXieJg->handle);
	wing_wide = pXieJg->GetWidth();
	wing_thick =pXieJg->GetThick();
	base_wing_wide = pFatherAngle->GetWidth();
	base_wing_thick= pFatherAngle->GetThick();
	GetWorkNorm(pFatherAngle,pXieJg,&norm);
	bInsideJg1 = IsInsideJg(pXieJg,norm,&x_wing0_y_wing1);
	if(x_wing0_y_wing1==0)
		direct = pXieJg->get_norm_y_wing();
	else
		direct = pXieJg->get_norm_x_wing();
	
	
	ls.set_norm(norm);
	getjgzj(jgzj,pXieJg->GetWidth());
	if(pXieJg->m_bEnableTeG)
	{
		if(x_wing0_y_wing1==0)
			jgzj = pXieJg->xWingXZhunJu;
		else
			jgzj = pXieJg->xWingYZhunJu;
	}
	if(bCanUndo)	//支持撤销操作
		Ta.BeginUndoListen();	//开始监听
	if(pXieJg->pStart == pNode)
	{
		len_offset1=pXieJg->desStartPos.len_offset_dist;
		ls_pos=pXieJg->GetStartDatumPos();
		GetLsSpace(LsSpace,pXieJg->connectStart.d);
		pXieJg->SetStartOdd(LsSpace.EndSpace);
		pXieJg->desStartOdd.m_iOddCalStyle=2;
		if(bInsideJg1)
		{
			if(x_wing0_y_wing1==0)
			{
				pXieJg->desStartPos.wing_x_offset.gStyle=4;
				pXieJg->desStartPos.wing_x_offset.offsetDist=-base_wing_thick;
				if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
					pXieJg->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
			}
			else
			{
				pXieJg->desStartPos.wing_y_offset.gStyle=4;
				pXieJg->desStartPos.wing_y_offset.offsetDist=-base_wing_thick;
				if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
					pXieJg->desStartPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
			}
			pXieJg->ClearFlag();
			pXieJg->CalPosition();
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*
				(pFatherAngle->GetThick()+pXieJg->GetThick()));
			sprintf(ls.des_base_pos.norm_offset.key_str,"-0X%X,-0X%X",pFatherAngle->handle,pXieJg->handle);
		}
		else
		{
			if(pGroupJg)
			{	//节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
				if(x_wing0_y_wing1==0)
				{
					pXieJg->desStartPos.wing_x_offset.gStyle=4;
					pXieJg->desStartPos.wing_x_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				else
				{
					pXieJg->desStartPos.wing_y_offset.gStyle=4;
					pXieJg->desStartPos.wing_y_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				pXieJg->ClearFlag();
				pXieJg->CalPosition();
			}
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherAngle->GetThick());
			ls.des_base_pos.norm_offset.AddThick(-pFatherAngle->handle,TRUE);
		}
		ls.set_d(pXieJg->connectStart.d);
		//上移添加螺栓通厚的位置 wht 10-11-04
		ls.AddL0Thick(pFatherAngle->handle,TRUE);
		ls.AddL0Thick(pXieJg->handle,TRUE);
		if(!ls.CalGuigeAuto())
		{
			char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"Fail to find bolt size according with M%dX%.f in bolt size library",pXieJg->connectStart.d,ls.get_L0());
#else
			sprintf(sError,"在螺栓规格库中没有找到符合M%dX%.f的螺栓规格",pXieJg->connectStart.d,ls.L0);
#endif
			if(bThrowError)
				throw sError;
			else
				return FALSE;
		}
		ls.ucs.origin=ls_pos;
		ls.set_d(pXieJg->connectStart.d);
		pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->CopyProperty(&ls);
		//pBolt->AddL0Thick(pFatherAngle->handle,TRUE);
		//pBolt->AddL0Thick(pXieJg->handle,TRUE);
		pBolt->iSeg=pXieJg->iSeg;
		pBolt->layer(2)=pXieJg->layer(2);	//调整螺栓图层名
		pBolt->cfgword=pFatherAngle->cfgword;	//调整螺栓配材号与基准构件配材号一致
		//螺栓位置设计\工作法线设计信息填充
		pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
		pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherAngle->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
		pBolt->des_work_norm.norm_style=1;	//指定角钢肢法线方向
		pBolt->des_work_norm.hVicePart = pFatherAngle->handle;
		if(pXieJg->desStartPos.IsInDatumJgWingX())	//搭于父角钢X肢
		{
			pBolt->des_work_norm.norm_wing=0;
			pBolt->des_work_norm.direction=0;
			pBolt->des_base_pos.len_offset_dist=0;
			pBolt->des_base_pos.wing_offset_dist = 0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherAngle->GetNodeWingXG(pNode));
		}
		else									//搭于父角钢Y肢
		{
			pBolt->des_work_norm.norm_wing=1;
			pBolt->des_work_norm.direction=0;
			pBolt->des_base_pos.offset_wing=1;
			pBolt->des_base_pos.len_offset_dist=0;
			pBolt->des_base_pos.wing_offset_dist = 0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherAngle->GetNodeWingYG(pNode));
		}
		//pBolt->des_base_pos.hNode = pNode->handle;
		pBolt->des_base_pos.norm_offset=ls.des_base_pos.norm_offset;
		
		if(pFatherAngle->pStart==pNode)
			pFatherAngle->AppendStartLsRef(pBolt->GetLsRef());
		else if(pFatherAngle->pEnd==pNode)
			pFatherAngle->AppendEndLsRef(pBolt->GetLsRef());
		else
			pFatherAngle->AppendMidLsRef(pBolt->GetLsRef());
		pXieJg->AppendStartLsRef(pBolt->GetLsRef());
		pXieJg->CalCutAngleInfo(pFatherAngle,NULL,true,NULL);
		pXieJg->SetModified();
	}
	else if(pXieJg->pEnd == pNode)
	{
		len_offset1=pXieJg->desEndPos.len_offset_dist;
		GetLsSpace(LsSpace,pXieJg->connectEnd.d);
		pXieJg->SetEndOdd(LsSpace.EndSpace);
		pXieJg->desEndOdd.m_iOddCalStyle=2;
		ls_pos=pXieJg->GetEndDatumPos();
		if(bInsideJg1)
		{
			if(x_wing0_y_wing1==0)
			{
				pXieJg->desEndPos.wing_x_offset.gStyle=4;
				pXieJg->desEndPos.wing_x_offset.offsetDist=-base_wing_thick;
				if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
					pXieJg->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
			}
			else
			{
				pXieJg->desEndPos.wing_y_offset.gStyle=4;
				pXieJg->desEndPos.wing_y_offset.offsetDist=-base_wing_thick;
				if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
					pXieJg->desEndPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
			}
			pXieJg->ClearFlag();
			pXieJg->CalPosition();
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*
				(pFatherAngle->GetThick()+pXieJg->GetThick()));
			sprintf(ls.des_base_pos.norm_offset.key_str,"-0X%X,-0X%X",-pFatherAngle->handle,pXieJg->handle);
		}
		else
		{
			if(pGroupJg)
			{	//节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
				if(x_wing0_y_wing1==0)
				{
					pXieJg->desEndPos.wing_x_offset.gStyle=4;
					pXieJg->desEndPos.wing_x_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				else
				{
					pXieJg->desEndPos.wing_y_offset.gStyle=4;
					pXieJg->desEndPos.wing_y_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				pXieJg->ClearFlag();
				pXieJg->CalPosition();
			}
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherAngle->GetThick());
			ls.des_base_pos.norm_offset.AddThick(-pFatherAngle->handle,TRUE);
		}
		ls.EmptyL0DesignPara();	//清空螺栓通厚设计参数
		ls.AddL0Thick(pFatherAngle->handle,TRUE);
		ls.AddL0Thick(pXieJg->handle,TRUE);
		ls.set_d(pXieJg->connectEnd.d);
		if(!ls.CalGuigeAuto())
		{
			char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"Fail to find bolt size according with M%dX%.f in bolt size library",pXieJg->connectEnd.d,ls.get_L0());
#else
			sprintf(sError,"在螺栓规格库中没有找到符合M%dX%.f的螺栓规格",pXieJg->connectEnd.d,ls.L0);
#endif
			if(bThrowError)
				throw sError;
			else
				return FALSE;
		}
		
		ls.ucs.origin=ls_pos;
		ls.set_d(pXieJg->connectEnd.d);
		pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->CopyProperty(&ls);
		pBolt->cfgword=pFatherAngle->cfgword;	//调整螺栓配材号与基准构件配材号一致
		//添加螺栓通厚设计参数
		pBolt->AddL0Thick(pFatherAngle->handle,TRUE,TRUE);
		pBolt->AddL0Thick(pXieJg->handle,TRUE,TRUE);
		pBolt->iSeg=pXieJg->iSeg;
		pBolt->layer(2)=pXieJg->layer(2);	//调整螺栓图层名
		//螺栓位置设计\工作法线设计信息填充
		pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
		pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherAngle->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
		pBolt->des_work_norm.norm_style=1;	//指定角钢肢法线方向
		pBolt->des_work_norm.hVicePart = pFatherAngle->handle;
		if(pXieJg->desEndPos.IsInDatumJgWingX())	//搭于父角钢X肢
		{
			pBolt->des_work_norm.norm_wing=0;
			pBolt->des_work_norm.direction=0;
			pBolt->des_base_pos.len_offset_dist=0;
			pBolt->des_base_pos.wing_offset_dist = 0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherAngle->GetNodeWingXG(pNode));
		}
		else									//搭于父角钢Y肢
		{
			pBolt->des_work_norm.norm_wing=1;
			pBolt->des_work_norm.direction=0;
			pBolt->des_base_pos.offset_wing=1;
			pBolt->des_base_pos.len_offset_dist=0;
			pBolt->des_base_pos.wing_offset_dist = 0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherAngle->GetNodeWingYG(pNode));
		}
		//pBolt->des_base_pos.hNode = pNode->handle;
		pBolt->des_base_pos.norm_offset=ls.des_base_pos.norm_offset;
		
		if(pFatherAngle->pStart==pNode)
			pFatherAngle->AppendStartLsRef(pBolt->GetLsRef());
		else if(pFatherAngle->pEnd==pNode)
			pFatherAngle->AppendEndLsRef(pBolt->GetLsRef());
		else
			pFatherAngle->AppendMidLsRef(pBolt->GetLsRef());
		pXieJg->AppendEndLsRef(pBolt->GetLsRef());
		pXieJg->CalCutAngleInfo(pFatherAngle,NULL,false,NULL);
		pXieJg->SetModified();
	}
	else
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "the selected ray material does not pass through current node";
#else
			throw "所选射线材,不通过当前节点";
#endif
		else
			return FALSE;
	}
	pXieJg->SetModified();
	pXieJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pXieJg->GetSolidPartObject());

	if(pXieJg2)
	{	//引入螺栓 调整角钢正负头以及角钢摆放位置
		GetWorkNorm(pFatherAngle,pXieJg2,&norm);
		bInsideJg2 = IsInsideJg(pXieJg2,norm,&x_wing0_y_wing1);
		if(pXieJg2->pStart == pNode)
		{
			len_offset2=pXieJg->desStartPos.len_offset_dist;
			GetLsSpace(LsSpace,pXieJg2->connectStart.d);
			pXieJg2->SetStartOdd(LsSpace.EndSpace);
			pXieJg2->desStartOdd.m_iOddCalStyle=2;
			if(bInsideJg2)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg2->desStartPos.wing_x_offset.gStyle=4;
					pXieJg2->desStartPos.wing_x_offset.offsetDist=-base_wing_thick;
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg2->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desStartPos.wing_y_offset.gStyle=4;
					pXieJg2->desStartPos.wing_y_offset.offsetDist=-base_wing_thick;
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg2->desStartPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				pXieJg2->ClearFlag();
				pXieJg2->CalPosition();
			}
			else if(pGroupJg)
			{	//节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
				if(x_wing0_y_wing1==0)
				{
					pXieJg2->desStartPos.wing_x_offset.gStyle=4;
					pXieJg2->desStartPos.wing_x_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				else
				{
					pXieJg2->desStartPos.wing_y_offset.gStyle=4;
					pXieJg2->desStartPos.wing_y_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				pXieJg2->ClearFlag();
				pXieJg2->CalPosition();
			}
			pXieJg2->CalCutAngleInfo(pFatherAngle,NULL,true,NULL);
		}
		else if(pXieJg2->pEnd == pNode)
		{
			len_offset2=pXieJg->desEndPos.len_offset_dist;
			GetLsSpace(LsSpace,pXieJg2->connectEnd.d);
			pXieJg2->SetEndOdd(LsSpace.EndSpace);
			pXieJg2->desEndOdd.m_iOddCalStyle=2;
			if(bInsideJg2)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg2->desEndPos.wing_x_offset.gStyle=4;
					pXieJg2->desEndPos.wing_x_offset.offsetDist=-base_wing_thick;
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg2->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desEndPos.wing_y_offset.gStyle=4;
					pXieJg2->desEndPos.wing_y_offset.offsetDist=-base_wing_thick;
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg2->desEndPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				pXieJg2->ClearFlag();
				pXieJg2->CalPosition();
			}
			else if(pGroupJg)
			{	//节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
				if(x_wing0_y_wing1==0)
				{
					pXieJg2->desEndPos.wing_x_offset.gStyle=4;
					pXieJg2->desEndPos.wing_x_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				else
				{
					pXieJg2->desEndPos.wing_y_offset.gStyle=4;
					pXieJg2->desEndPos.wing_y_offset.offsetDist=pGroupJg->jg_space*0.5;
				}
				pXieJg2->ClearFlag();
				pXieJg2->CalPosition();
			}
			pXieJg2->CalCutAngleInfo(pFatherAngle,NULL,false,NULL);
		}
		if(bInsideJg1!=bInsideJg2&&fabs(len_offset1-len_offset2)<EPS)
		{	//两角钢一里一外，而且共用一颗螺栓情况
			//更新螺栓通厚
			pBolt->AddL0Thick(pXieJg2->handle,TRUE,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->SetModified();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			if(pXieJg2->pStart==pNode)
				pXieJg2->AppendStartLsRef(pBolt->GetLsRef());
			else if(pXieJg2->pEnd==pNode)
				pXieJg2->AppendEndLsRef(pBolt->GetLsRef());
		}
		else
		{	//两角钢无共用螺栓情况
			CLDSBolt *pBolt2=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt2->CopyProperty(pBolt);
			pBolt->cfgword=pFatherAngle->cfgword;	//调整螺栓配材号与基准构件配材号一致
			pBolt2->des_base_pos=pBolt->des_base_pos;
			pBolt2->des_work_norm=pBolt->des_work_norm;
			pBolt2->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg2->handle;
			pBolt2->EmptyL0DesignPara();
			pBolt2->AddL0Thick(pFatherAngle->handle,TRUE,TRUE);
			pBolt2->AddL0Thick(pXieJg->handle,TRUE,TRUE);
			pBolt2->CalGuigeAuto();
			pBolt2->correct_worknorm();
			pBolt2->correct_pos();
			pBolt2->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt2->GetSolidPartObject());
			if(pXieJg2->pStart==pNode)
				pXieJg2->AppendStartLsRef(pBolt2->GetLsRef());
			else if(pXieJg2->pEnd==pNode)
				pXieJg2->AppendEndLsRef(pBolt2->GetLsRef());
		}

		pXieJg2->SetModified();
		pXieJg2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pXieJg2->GetSolidPartObject());
	}
	if(pBolt)
	{
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	theApp.GetLDSDoc()->SetModifiedFlag(); // 修改数据后应调用此函数置修改标志.
	if(bCanUndo)	//支持撤销操作
		Ta.EndUndoListen();	//结束监听
	return TRUE;
}

//设计共用板
static void DesignSharePlate(CConnectPointInfo *pConnectPoint)
{
	if(pConnectPoint==NULL)
		return;	
}

static BOOL DesignConnectL_1LB1(CConnectPointInfo *pConnectPoint)
{
	CSingleRayEndBoltLj design;
	return design.DesignConnectPoint(pConnectPoint);
}
static BOOL DesignConnectL_1LB2(CConnectPointInfo *pConnectPoint)
{
	CSingleRayEndBoltLj design;
	return design.DesignConnectPoint(pConnectPoint);
}
static BOOL DesignConnectL_2LB1(CConnectPointInfo *pConnectPoint)
{
	CSingleRayEndBoltLj design;
	return design.DesignConnectPoint(pConnectPoint);
}
static BOOL DesignConnectL_2LB4(CConnectPointInfo *pConnectPoint)
{	//与L_2LB1除射线螺栓数为2以外，其余无区别，故可复用
	return DesignConnectL_2LB1(pConnectPoint);
}
static BOOL DesignDoubleBoltEndLj(CConnectPointInfo *pConnectPoint)
{
	CLDSNode *pDatumNode=Ta.Node.FromHandle(pConnectPoint->m_hBaseNode);
	CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pConnectPoint->m_hBasePart,CLS_GROUPLINEANGLE,CLS_LINEANGLE);
	if(pDatumLineAngle==NULL&&pDatumNode==NULL)
		return FALSE;
	int nLinePart=pConnectPoint->linkPartSet.GetNodeNum();
	if(nLinePart>2)
		return FALSE;
	//
	for(int i=0;i<nLinePart;i++)
	{
		CSuperSmartPtr<CLDSLinePart> pRayJg;
		if(i==0)
			pRayJg=pConnectPoint->linkPartSet.GetFirst();
		else if(i==1)
			pRayJg=pConnectPoint->linkPartSet.GetNext();
		if(pRayJg.IsNULL()||!pRayJg->IsAngle())
			return FALSE;
		if(pRayJg->pStart==NULL||pRayJg->pEnd==NULL)
			return FALSE;
		int nBoltNum=0,nBoltD=16;
		if(pRayJg->pStart->handle==pConnectPoint->m_hBaseNode)
		{
			nBoltNum=pRayJg->connectStart.wnConnBoltN;
			nBoltD=pRayJg->connectStart.d;
		}
		else if(pRayJg->pEnd->handle==pConnectPoint->m_hBaseNode)
		{
			nBoltNum=pRayJg->connectEnd.wnConnBoltN;
			nBoltD=pRayJg->connectEnd.d;
		}
		else 
			return FALSE;
		if(nBoltNum>2)
			return FALSE;	//此处最多支持两个螺栓
		for(int j=0;j<nBoltNum;j++)
		{
			JGZJ jgzj;
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->AddL0Thick(pDatumLineAngle->handle,TRUE);
			pBolt->AddL0Thick(pRayJg->handle,TRUE);
			pBolt->iSeg=pRayJg->iSeg;
			pBolt->layer(2)=pRayJg->layer(2);	//调整螺栓图层名
			pBolt->set_d(nBoltD);
			pBolt->cfgword=pDatumLineAngle->cfgword;	//调整螺栓配材号与基准构件配材号一致
			//螺栓位置设计\工作法线设计信息填充
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pDatumLineAngle->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pRayJg->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			//pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
			pBolt->des_work_norm.norm_style=1;	//指定角钢肢法线方向
			pBolt->des_work_norm.hVicePart = pDatumLineAngle->handle;
			getjgzj(jgzj,pDatumLineAngle->GetWidth());
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace,nBoltD);
			if(pConnectPoint->m_iConnectWing==0)	//搭于父角钢X肢
			{
				pBolt->des_work_norm.norm_wing=0;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pDatumLineAngle->m_bEnableTeG)
					jgzj=pDatumLineAngle->xWingXZhunJu;
				if(nBoltNum==1)	//单螺栓
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj.g;
				else if(j==0)	//双螺栓
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj.g1;
				else if(j==1)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 =(long)pDatumLineAngle->GetWidth()-LsSpace.EndSpace;//jgzj.g2;
				//pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pDatumLineAngle->GetNodeWingXG(pDatumNode));
			}
			else				//搭于父角钢Y肢
			{
				pBolt->des_work_norm.norm_wing=1;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.offset_wing=1;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pDatumLineAngle->m_bEnableTeG)
					jgzj=pDatumLineAngle->xWingYZhunJu;
				if(nBoltNum==1)	//单螺栓
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj.g;
				else if(j==0)	//双螺栓
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj.g1;
				else if(j==1)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = (long)pDatumLineAngle->GetWidth()-LsSpace.EndSpace;//jgzj.g2;
				//pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pDatumLineAngle->GetNodeWingYG(pDatumNode));
			}
			pBolt->des_base_pos.norm_offset.AddThick(-(long)(pDatumLineAngle->GetThick()));
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			//
			if(pDatumLineAngle->pStart->handle==pConnectPoint->m_hBaseNode)
				pDatumLineAngle->AppendStartLsRef(pBolt->GetLsRef());
			else if(pDatumLineAngle->pEnd->handle==pConnectPoint->m_hBaseNode)
				pDatumLineAngle->AppendEndLsRef(pBolt->GetLsRef());
			else
				pDatumLineAngle->AppendMidLsRef(pBolt->GetLsRef());
			if(pRayJg->pStart->handle==pConnectPoint->m_hBaseNode)
			{
				pRayJg->AppendStartLsRef(pBolt->GetLsRef());
				pRayJg->desStartOdd.m_iOddCalStyle=1;//根据螺栓位置计算正负头
				pRayJg->CalStartOddment();
			}
			else 
			{
				pRayJg->AppendEndLsRef(pBolt->GetLsRef());
				pRayJg->desEndOdd.m_iOddCalStyle=1;	//根据螺栓位置计算正负头
				pRayJg->CalEndOddment();
			}
		}
		pRayJg.LineAnglePointer()->CalCutAngleInfo(pDatumLineAngle,NULL,true,NULL);
		pRayJg->SetModified();
		pRayJg->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
	}
	return TRUE;
}
static BOOL NewStandardPart(CLDSLineTube *pDatumTube,CLDSStdPartModel *pStdPartModel,
							BOOL bStartEnd,CLDSLineTube *pRelatedTube,CLDSStdPartModel *pCoupleStdPartModel)
{
	if(pStdPartModel==NULL)
		return FALSE;
	CLDSParamPlate *pFlPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	pFlPlate->m_bStdPart=TRUE;	//标准构件
	pFlPlate->iSeg=pDatumTube->iSeg;
	pFlPlate->m_uStatMatCoef=1;
	pFlPlate->SetPartNo(pStdPartModel->GetPartNo(),true);
	pFlPlate->m_hPartWeldParent=pDatumTube->handle;
	pFlPlate->cfgword=pDatumTube->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
	pFlPlate->SetLayer(pDatumTube->layer()); //调整法兰的图层名
	pFlPlate->m_iParamType=pStdPartModel->m_iStdPartType;	
	//设置工作面相对于钢管基准面的旋转角度 wht 10-01-21
	//用字母'X'表示旋转角度，D与标准法兰参数重名 
#ifdef AFX_TARG_ENU_ENGLISH
	pFlPlate->SetDesignItemValue('X',0,"relative angle of datum X axis rotating around tube X axis");
	pFlPlate->SetDesignItemValue('J',1,"ring layout bolt:1 on behalf of by the reference of weld road line，0 on behalf of by the reference of datum edge.");
	if(pFlPlate->m_iParamType==TYPE_FLP)	
		pFlPlate->SetDesignItemValue('O',pStdPartModel->param.fl_param.H1,"the distance between tube end and flange bottom");
#else
	pFlPlate->SetDesignItemValue('X',0,"基准X轴相对钢管X轴旋转角度");
	pFlPlate->SetDesignItemValue('J',1,"环向布置螺栓时:1表示以焊道线为基准，0表示以基准边为基准。");
	if(pFlPlate->m_iParamType==TYPE_FLP)	
		pFlPlate->SetDesignItemValue('O',pStdPartModel->param.fl_param.H1,"钢管端与法兰底面的距离");
#endif
	pFlPlate->cMaterial=pStdPartModel->param.fl_param.cMaterial;	//法兰材质
	if(bStartEnd)
		pDatumTube->m_tJointStart.hLinkObj=pFlPlate->handle;
	else 
		pDatumTube->m_tJointEnd.hLinkObj=pFlPlate->handle;
	pFlPlate->DesignStdPartUcs();
	pFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	pStdPartModel->CreateBoltInPlate(pFlPlate);//布置法兰上的螺栓
	//生成对接法兰
	double fOddment=0;
	CLDSNode *pNode=NULL;
	if(bStartEnd)
	{
		pNode=pDatumTube->pStart;
		fOddment=pDatumTube->startOdd();
	}
	else
	{
		pNode=pDatumTube->pEnd;
		fOddment=pDatumTube->endOdd();
	}
	if(pCoupleStdPartModel)
	{	//设计对接法兰
		if(pRelatedTube==NULL)
			return FALSE;
		double oddment;
		if(pFlPlate->m_iParamType==TYPE_FLP)		//平焊法兰
			oddment=-pStdPartModel->param.fl_param.H1-pCoupleStdPartModel->param.fl_param.H1-fOddment;
		else //对焊法兰
			oddment=-pCoupleStdPartModel->param.fl_param.H-pStdPartModel->param.fl_param.H-fOddment;
		CLDSParamPlate *pCoupleFlPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pCoupleFlPlate->CopyProperty(pFlPlate);
		pCoupleFlPlate->m_hPartWeldParent = pRelatedTube->handle;
		pCoupleFlPlate->layer(2)=pRelatedTube->layer(2); //调整法兰的图层名
		pCoupleFlPlate->cMaterial=pCoupleStdPartModel->param.fl_param.cMaterial;	//标准法兰材质
		pCoupleFlPlate->SetPartNo(pCoupleStdPartModel->param.fl_param.codeName);
		if(pRelatedTube->pStart==pNode)
		{
			pRelatedTube->SetStartOdd(oddment);
			pRelatedTube->m_tJointStart.type=2;
			pRelatedTube->m_tJointStart.hLinkObj=pCoupleFlPlate->handle;
		}
		else
		{
			pRelatedTube->SetEndOdd(oddment);
			pRelatedTube->m_tJointEnd.type=2;
			pRelatedTube->m_tJointEnd.hLinkObj=pCoupleFlPlate->handle;
		}
		//修改螺栓通厚重新计算规格
		for(CLsRef *pLsRef=pFlPlate->GetFirstLsRef();pLsRef;pLsRef=pFlPlate->GetNextLsRef())
		{
			pCoupleFlPlate->AppendLsRef(*pLsRef);
			pLsRef->GetLsPtr()->AddL0Thick(pCoupleFlPlate->handle,TRUE);
			pLsRef->GetLsPtr()->CalGuigeAuto();
			(*pLsRef)->SetModified();
			(*pLsRef)->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		}
		pCoupleFlPlate->DesignStdPartUcs();
		pRelatedTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		pCoupleFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCoupleFlPlate->GetSolidPartObject());
		g_pSolidDraw->NewSolidPart(pRelatedTube->GetSolidPartObject());	
	}
	if(bStartEnd)
	{
		pDatumTube->m_tJointStart.type=2;
		pDatumTube->m_tJointStart.hLinkObj=pFlPlate->handle;
	}
	else
	{
		pDatumTube->m_tJointEnd.type=2;
		pDatumTube->m_tJointEnd.hLinkObj=pFlPlate->handle;
	}
	pDatumTube->CalPosition();	//计算钢管位置
	pFlPlate->DesignStdPartUcs();
	pFlPlate->DesignPlate();
	pFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pFlPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	//绘制标准构件上的螺栓
	for(CLsRef *pLsRef=pFlPlate->GetFirstLsRef();pLsRef;pLsRef=pFlPlate->GetNextLsRef())
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	pFlPlate->ResetSonBoltRayNo();		//布置插板上螺栓射线号
	pFlPlate->GetLsRefList()->ResetBoltRefRayNo(pFlPlate);	//布置插板上螺栓孔射线号
	return TRUE;
}
//设计对接法兰
static BOOL DesignTubeFlJoint(CLDSNode *pBaseNode,CLDSLineTube *pDatumTube,CLDSLineTube *pCoupleTube)
{
	if(pBaseNode==NULL||pDatumTube==NULL||pCoupleTube==NULL)
		return FALSE;
	CLDSPart *pPart=NULL;
	if(pCoupleTube->pStart==pBaseNode&&pCoupleTube->m_tJointStart.hLinkObj>0X20)
		pPart = Ta.Parts.FromHandle(pCoupleTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	else if(pCoupleTube->pEnd==pBaseNode&&pCoupleTube->m_tJointEnd.hLinkObj>0X20)
		pPart = Ta.Parts.FromHandle(pCoupleTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if(pPart)
		return FALSE;
	if(pDatumTube->pStart==pBaseNode&&pDatumTube->m_tJointStart.hLinkObj>0X20)
		pPart = Ta.Parts.FromHandle(pDatumTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	else if(pDatumTube->pEnd==pBaseNode&&pDatumTube->m_tJointEnd.hLinkObj>0X20)
		pPart = Ta.Parts.FromHandle(pDatumTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	if(pPart)
		return FALSE;
	int i=0;
	char cMat=pDatumTube->cMaterial;
	double couple_tube_d=pCoupleTube->GetDiameter();
	double tube_d=pDatumTube->GetDiameter(),tube_t=pDatumTube->GetThick();
	CLDSStdPartModel stdPartModel,coupleStdPartModel;
	if(fabs(tube_d-couple_tube_d)<EPS)
	{	//设计等径对接法兰
		//根据预设优先级查找法兰标准
		for(i=0;i<g_sysPara.m_arrFlType.GetSize();i++)
		{
			PARAM_PLATE_TYPE iFlType;
			if(g_sysPara.m_arrFlType[i].CompareNoCase("FLD")==0)
				iFlType=TYPE_FLD;
			else if(g_sysPara.m_arrFlType[i].CompareNoCase("FLP")==0)
				iFlType=TYPE_FLP;
			else if(g_sysPara.m_arrFlType[i].CompareNoCase("FLG")==0)
				iFlType=TYPE_FLG;
			else if(g_sysPara.m_arrFlType[i].CompareNoCase("FLR")==0)
				iFlType=TYPE_FLR;
			else
				return FALSE;
			if(globalLibrary.GetStdPartModel(tube_d,tube_t,cMat,'A',iFlType,&stdPartModel))
			{
				stdPartModel.m_fRelateTubeThick=pDatumTube->GetThick();
				return NewStandardPart(pDatumTube,&stdPartModel,pDatumTube->pStart==pBaseNode,pCoupleTube,&stdPartModel);
			}
		}
	}
	else 
	{	//成对设计不等径法兰
		for(i=0;i<g_sysPara.m_arrFlType.GetSize();i++)
		{
			char cFlType='D';
			PARAM_PLATE_TYPE iFlType;
			if(g_sysPara.m_arrFlType[i].CompareNoCase("FLD")==0)
			{
				cFlType='D';
				iFlType=TYPE_FLD;
			}
			else if(g_sysPara.m_arrFlType[i].CompareNoCase("FLP")==0)
			{
				cFlType='P';
				iFlType=TYPE_FLP;
			}
			else if(g_sysPara.m_arrFlType[i].CompareNoCase("FLG")==0)
			{
				cFlType='G';
				iFlType=TYPE_FLG;
			}
			else if(g_sysPara.m_arrFlType[i].CompareNoCase("FLR")==0)
			{
				cFlType='R';
				iFlType=TYPE_FLR;
			}
			else
				return FALSE;
			FL_COUPLE couple;
			if(!QueryFlCouple(tube_d,couple_tube_d,cFlType,&couple))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("fail to find the flange tube diameter match of main tube 0X%X at 0x%X node %.0f/%.0f",pDatumTube->handle,
					pBaseNode->handle,pDatumTube->GetDiameter(),pCoupleTube->GetDiameter());
#else
				logerr.Log("未找到0x%X节点处主管0X%X上的法兰钢管直径配对%.0f/%.0f",pBaseNode->handle,
					pDatumTube->handle,pDatumTube->GetDiameter(),pCoupleTube->GetDiameter());
#endif
				continue;
			}
			bool hasStdFl[2]={false};
			if(globalLibrary.GetStdPartModel(couple.mainFlName,iFlType,&stdPartModel,pDatumTube->GetThick()))
			{
				stdPartModel.m_fRelateTubeThick=pCoupleTube->GetThick();
				hasStdFl[0]=true;
			}
			if(globalLibrary.GetStdPartModel(couple.coupleFlName,iFlType,&coupleStdPartModel,pCoupleTube->GetThick()))
			{
				coupleStdPartModel.m_fRelateTubeThick=pDatumTube->GetThick();
				hasStdFl[1]=true;
			}
			if(hasStdFl[0]&&hasStdFl[1])
				return NewStandardPart(pDatumTube,&stdPartModel,pDatumTube->pStart==pBaseNode,pCoupleTube,&coupleStdPartModel);
		}
	}
	return FALSE;
}
#include "Deg.h"
#include "./StdComTempl/IStdComTempl.h"
#include "./StdComTempl/StdComModel.h"
// 以下代码已移植到StdComModel.cpp中 wjh-2019.1.29
#ifdef __DEPRECATED_CODE_
struct MATCHROD_INFO{
	bool matched;
	char ciConnTipS0E1Els2;
	char ciPlaneNo;
public:	//属性
	short siRayDegAngle;
	DEG get_xRayDegAngle(){return DEG(siRayDegAngle);}
	__declspec(property(get=get_xRayDegAngle)) DEG xRayDegAngle;
	char get_ciRodType()		//'A':Across，杆件跨越基准节点;'R':Radiation杆件从基准节点射线发出
	{
		if(ciConnTipS0E1Els2==0||ciConnTipS0E1Els2==1)
			return 'R';
		else
			return 'A';
	}
	__declspec(property(get=get_ciRodType)) char ciRodType;
public:
	CLDSLinePart* pRod;
	MATCHROD_INFO(CLDSLinePart* _pRod=NULL){
		matched=false;
		pRod=_pRod;
		ciPlaneNo=0;
		ciConnTipS0E1Els2=0;
	}
};
struct MATCHCONNPOINT_INFO{
public:	//连接点基本识别信息
	GEPOINT lenBaseRodStdVec;
	PRESET_ARRAY8<MATCHROD_INFO> connrods;
public:	//连接点匹配信息
	long hBaseNode;
	CLDSLinePart* pBaseRod;
	char ciMirrorMatchMode;	//是否为镜像对称
	PRESET_ARRAY8<long> xarrMatchRodHid;	//预留8根杆件，遇到第一根widRod=0杆件结束 
public:	//属性
	long set_hBaseRod(long hRod){
		xarrMatchRodHid.Set(0,hRod);
		return hRod;
	}
	long get_hBaseRod(){return xarrMatchRodHid[0];}
	__declspec(property(put=set_hBaseRod,get=get_hBaseRod)) long hBaseRod;
	long set_hRayRodOnAxisY(long hRod){
		xarrMatchRodHid.Set(1,hRod);
		return hRod;
	}
	long get_hRayRodOnAxisY(){return xarrMatchRodHid[1];}
	__declspec(property(put=set_hRayRodOnAxisY,get=get_hRayRodOnAxisY)) long hRayRodOnAxisY;
	MATCHCONNPOINT_INFO(){
		ciMirrorMatchMode=0;
		hBaseNode=0;
		pBaseRod=NULL;
		xarrMatchRodHid.ZeroPresetMemory();
	}
};
static bool MapStdConnPointInfo(CStdComTemplConnPoint* pStdConnPoint,MATCHCONNPOINT_INFO* pMatchInfo)
{
	CLDSNode* pBaseNode=Ta.FromNodeHandle(pMatchInfo->hBaseNode);
	if((pMatchInfo->pBaseRod=Ta.FromRodHandle(pMatchInfo->hBaseRod))==NULL)
		return false;
	CLDSLinePart* pRayRod=Ta.FromRodHandle(pMatchInfo->hRayRodOnAxisY);
	if(pRayRod==NULL)
		return false;
	pStdConnPoint->ClearTmplParams();
	pStdConnPoint->m_ciMirrorMatchMode=pMatchInfo->ciMirrorMatchMode;
	pStdConnPoint->SetMappingNode(1,pMatchInfo->hBaseNode);
	pStdConnPoint->listParaRods.Add(pStdConnPoint->widBaseRod)->pRelaRod=pMatchInfo->pBaseRod;
	pStdConnPoint->listParaRods.Add(2)->pRelaRod=pRayRod;
	pStdConnPoint->hashMatchRodHid.SetValue(pStdConnPoint->widBaseRod,pMatchInfo->pBaseRod->handle);
	pStdConnPoint->hashMatchRodHid.SetValue(2,pRayRod->handle);
	pStdConnPoint->SetBelongModel(&Ta);
	//if(!pStdConnPoint->IntelliRecogRayRods())
	//	return false;
	for(UINT i=0;i<pMatchInfo->xarrMatchRodHid.Count;i++)
	{
		pStdConnPoint->rodarr[i].hMapRod=pMatchInfo->xarrMatchRodHid[i];
		pStdConnPoint->hashMatchRodHid.SetValue(i+1,pMatchInfo->xarrMatchRodHid[i]);
	}
	return true;
}
static bool RetrieveBasicMatchInfo(CConnectPointInfo *pConnectPoint,MATCHCONNPOINT_INFO* pMatchInfo)
{
	pMatchInfo->hBaseRod=pConnectPoint->m_hBasePart;
	pMatchInfo->connrods.Clear();
	pMatchInfo->connrods.ZeroPresetMemory();
	CLDSLinePart* pBaseRod,*pLinkRod;
	pBaseRod=pMatchInfo->pBaseRod=Ta.FromRodHandle(pConnectPoint->m_hBasePart);
	if(pBaseRod==NULL||pBaseRod->pStart==NULL||pBaseRod->pEnd==NULL)
		return false;
	pMatchInfo->lenBaseRodStdVec=pBaseRod->pEnd->xOriginalPos-pBaseRod->pStart->xOriginalPos;
	normalize(pMatchInfo->lenBaseRodStdVec);
	GECS cs;
	cs.axis_z=pConnectPoint->datumplane.vLine;
	normalize(cs.axis_z);
	double prjlen=pMatchInfo->lenBaseRodStdVec*cs.axis_z;
	if(prjlen>EPS)
		pMatchInfo->lenBaseRodStdVec-=prjlen*cs.axis_z;
	cs.axis_x=pMatchInfo->lenBaseRodStdVec;
	normalize(cs.axis_x);
	cs.axis_y=cs.axis_z^cs.axis_x;

	LINEPARTSET rodset;
	if(pConnectPoint->linkPartSet.FromHandle(pBaseRod->handle)==NULL)
	{
		rodset.append(pBaseRod);
		pBaseRod->feature=1;
	}
	for(pLinkRod=pConnectPoint->linkPartSet.GetFirst();pLinkRod;pLinkRod=pConnectPoint->linkPartSet.GetNext())
	{
		rodset.append(pLinkRod);
		pLinkRod->feature=1;
	}
	for(pLinkRod=pConnectPoint->bendPlaneRodSet.GetFirst();pLinkRod;pLinkRod=pConnectPoint->bendPlaneRodSet.GetNext())
	{
		rodset.append(pLinkRod);
		pLinkRod->feature=2;
	}
	for(pLinkRod=rodset.GetFirst();pLinkRod;pLinkRod=rodset.GetNext())
	{
		MATCHROD_INFO rodinfo(pLinkRod);
		GEPOINT lenStdVec=pLinkRod->pEnd->xOriginalPos-pLinkRod->pStart->xOriginalPos;
		lenStdVec=cs.TransVToCS(lenStdVec);
		rodinfo.siRayDegAngle=ftoi(DEG(lenStdVec));
		rodinfo.ciPlaneNo=(BYTE)pLinkRod->feature;
		if(pLinkRod->pStart->handle==pConnectPoint->m_hBaseNode)
			rodinfo.ciConnTipS0E1Els2=0;
		else if(pLinkRod->pEnd->handle==pConnectPoint->m_hBaseNode)
		{
			rodinfo.siRayDegAngle=(rodinfo.siRayDegAngle+180)%360;
			rodinfo.ciConnTipS0E1Els2=1;
		}
		else//pRod->pStart!=pBaseNode&&pRod->pEnd!=pBaseNode
		{	//贯穿杆件
			if(rodinfo.siRayDegAngle>180)
				rodinfo.siRayDegAngle-=180;
			rodinfo.ciConnTipS0E1Els2=2;
		}
		pMatchInfo->connrods.Append(rodinfo);
	}
	return true;
}
static bool MatchWithStdConnPoint(CConnectPointInfo *pConnectPoint,
		CStdComTemplConnPoint* pStdConnPoint,MATCHCONNPOINT_INFO* pMatchInfo)
{
	char ciViewPlaneNormal=pStdConnPoint->xView.ciViewPlaneNormal;
	if(ciViewPlaneNormal=='H')
	{	//表示视向为水平方向，'X','x','Y'及'y'都有可能，需根据连接点线判断
		if(fabs(pConnectPoint->datumplane.vLine.x)>fabs(pConnectPoint->datumplane.vLine.y))
			ciViewPlaneNormal=pConnectPoint->datumplane.vLine.x>0?'X':'x';
		else
			ciViewPlaneNormal=pConnectPoint->datumplane.vLine.y>0?'Y':'y';
	}
	GECS cs;	//视觉投影平面坐标系（Z轴指向人的眼睛反方向）
	if(ciViewPlaneNormal=='X')
		cs=GECS(GEPOINT(0,0,0),GEPOINT(0,-1,0),GEPOINT(0,0,-1),GEPOINT( 1,0,0));
	else if(ciViewPlaneNormal=='x')
		cs=GECS(GEPOINT(0,0,0),GEPOINT(0, 1,0),GEPOINT(0,0,-1),GEPOINT(-1,0,0));
	else if(ciViewPlaneNormal=='Y')
		cs=GECS(GEPOINT(0,0,0),GEPOINT( 1,0,0),GEPOINT(0,0,-1),GEPOINT(0, 1,0));
	else if(ciViewPlaneNormal=='y')
		cs=GECS(GEPOINT(0,0,0),GEPOINT(-1,0,0),GEPOINT(0,0,-1),GEPOINT(0,-1,0));
	else if(ciViewPlaneNormal=='Z')
		cs=GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0, 1,0),GEPOINT(0,0, 1));
	else if(ciViewPlaneNormal=='y')
		cs=GECS(GEPOINT(0,0,0),GEPOINT(1,0,0),GEPOINT(0,-1,0),GEPOINT(0,0,-1));
	else
		return false;
	if(fabs(pConnectPoint->datumplane.vLine*cs.axis_z)<0.7)
		return false;	//与三坐标轴的均分角射线与坐标轴夹角余弦为0.57735,平面45°余弦=0.707
	CLDSLinePart* pBaseRod=Ta.FromRodHandle(pConnectPoint->m_hBasePart);
	if(pMatchInfo==NULL||pBaseRod==NULL||pBaseRod->pStart==NULL||pBaseRod->pEnd==NULL)
	{
		logerr.Log("error");
		return false;
	}
	pMatchInfo->hBaseNode=pConnectPoint->m_hBaseNode;
	pMatchInfo->hBaseRod=pConnectPoint->m_hBasePart;

	cs.axis_z=pConnectPoint->datumplane.vLine;
	if( pStdConnPoint->xView.ciViewPlaneNormal=='X'||pStdConnPoint->xView.ciViewPlaneNormal=='x'||
		pStdConnPoint->xView.ciViewPlaneNormal=='Y'||pStdConnPoint->xView.ciViewPlaneNormal=='y')
	{
		cs.axis_x=cs.axis_y^cs.axis_z;
		normalize(cs.axis_x);
		normalize(cs.axis_z);
	}
	else if(pStdConnPoint->xView.ciViewPlaneNormal=='Z'||pStdConnPoint->xView.ciViewPlaneNormal=='z')
	{
		cs.axis_y=cs.axis_z^cs.axis_x;
		normalize(cs.axis_y);
		normalize(cs.axis_z);
	}
	GEPOINT lenStdVec=pMatchInfo->lenBaseRodStdVec;
	normalize(lenStdVec);
	lenStdVec=cs.TransVToCS(lenStdVec);
	short siDegAng=ftoi(DEG(lenStdVec));
	if(pBaseRod->pEnd->handle==pConnectPoint->m_hBaseNode)
		siDegAng=(siDegAng+180)%360;
	else if(pBaseRod->pStart->handle!=pConnectPoint->m_hBaseNode)
	{	//中间贯穿连接,取小于180度那一侧夹角为基准夹角
		siDegAng=min(siDegAng,(siDegAng+180)%360);
	}
	short siMirTurnDegAng=siDegAng<=180?180-siDegAng:360-siDegAng;
	DEG xDegAngle(siDegAng),xMirAngle(siMirTurnDegAng);
	if( !xDegAngle.IsInSector(pStdConnPoint->xView.siMinBaseRodAngle,pStdConnPoint->xView.siMaxBaseRodAngle,true)&&
		!xMirAngle.IsInSector(pStdConnPoint->xView.siMinBaseRodAngle,pStdConnPoint->xView.siMaxBaseRodAngle,true))
		return false;	//正向或左右镜象对称均无法匹配基准杆件
	bool bTooManyRods=false,bNeedNecessaryRod=false;
	UINT i,j,mirturn;
	bool meetconditions=true;
	MATCHROD_INFO* pMatchRod;
	SCHEMA_ROD* pSchemaRod;
	for(mirturn=0;mirturn<4;mirturn++)
	{	//mirturn分别代表了连接点四度对称方位的不同形态
		meetconditions=true;
		for(j=0;j<pMatchInfo->connrods.Count;j++)
			pMatchInfo->connrods.At(j).matched=false;
		for(i=0;meetconditions&&i<pStdConnPoint->rodarr.Count;i++)
		{
			pSchemaRod=pStdConnPoint->rodarr.GetAt(i);
			pSchemaRod->hMapRod=0;
			for(j=0;j<pMatchInfo->connrods.Count;j++)
			{
				pMatchRod=pMatchInfo->connrods.GetAt(j);
				if(pMatchRod->matched)
					continue;	//已匹配过
				DEG xRayDegAngle(pMatchRod->siRayDegAngle);
				if(mirturn==1)	//左右对称匹配
					xRayDegAngle=180+pMatchRod->siRayDegAngle;
				else if(mirturn==2)
					xRayDegAngle=360-pMatchRod->siRayDegAngle;
				else if(mirturn==3)
					xRayDegAngle=180+360-pMatchRod->siRayDegAngle;
				if(!xRayDegAngle.IsInSector(pSchemaRod->siMinRayAngle,pSchemaRod->siMaxRayAngle,pSchemaRod->dualSideEdge))
					continue;	//射线夹角不匹配
				if(pSchemaRod->ciRodType=='R'&&pMatchRod->ciRodType=='A')
					continue;	//端连接类型不一致(SchemaRod为'A'而MatchRod为'R'时考虑到存在正头情况，可认为一致)
				char szRodSectType[4]={0};
				szRodSectType[0]=pMatchRod->pRod->size_cSubClassType;
				if(!pMatchRod->pRod->IsTube())
					memcpy(szRodSectType,"φ",2);
				else if(!pMatchRod->pRod->IsAngle())
					szRodSectType[0]=0;

				if(pSchemaRod->xFlag.bFilterSectType)
				{	//杆件截面类型判断
					if((pSchemaRod->szRodSectType[0]=='L'||pSchemaRod->szRodSectType[0]=='T'||
						pSchemaRod->szRodSectType[0]=='D'||pSchemaRod->szRodSectType[0]=='X')&&
						pMatchRod->pRod->size_cSubClassType!=pSchemaRod->szRodSectType[0])
						continue;
				}
				pMatchRod->matched=true;
				pSchemaRod->hMapRod=pMatchRod->pRod->handle;
				break;
			}
			if(pSchemaRod->ciNecessity==1&&pSchemaRod->hMapRod==0)
				meetconditions=false;	//缺少必要的杆件
		}
		for(j=0;meetconditions&&j<pMatchInfo->connrods.Count;j++)
			meetconditions=meetconditions&&pMatchInfo->connrods.At(j).matched;
		if(meetconditions)
			break;
	}
	if(meetconditions)
	{
		pMatchInfo->ciMirrorMatchMode=mirturn;
		pMatchInfo->hRayRodOnAxisY=pStdConnPoint->rodarr[1].hMapRod;
		for(i=2;i<pStdConnPoint->rodarr.Count;i++)
		{
			pSchemaRod=pStdConnPoint->rodarr.GetAt(i);
			pMatchInfo->xarrMatchRodHid.Set(i,pSchemaRod->hMapRod);
		}
		MapStdConnPointInfo(pStdConnPoint,pMatchInfo);
	}
	return meetconditions;
}
#include "ConnPointTemplPlateDlg.h"
bool DesignStdConnPoint(CStdComTemplConnPoint* pStdConnPoint,bool bDisplayMultiTmplUI=false)
{
	long idStdNodePlate=0;
	CStdComTempl* pStdNodePlate=NULL;
	CStdComModel modeler(&Ta);
	CStdComTemplConnPoint::CMatchStdPlate* pMatchStdPlate=pStdConnPoint->hashMaybeTmplPlates.GetFirst();
	//if(pStdConnPoint->hashMaybeTmplPlates.GetNodeNum()>1&&bDisplayMultiTmplUI)
	{
		pStdNodePlate=globalStdComTmplLibrary.LoadStdComTempl(pMatchStdPlate->idStdNodePlate);
		modeler.VerifyMatchRods(pStdNodePlate,NULL);
		/*TODO:界面选择idStdNodePlate=pStdConnPoint->hashMaybeTmplPlates.GetFirst();
		CConnPointTemplPlateDlg templPlate;
		templPlate.InitImageList(pStdConnPoint->m_idStdComTmpl);
		if(!templPlate.DoModal())
			return false;
		pMatchStdPlate=pStdConnPoint->hashMaybeTmplPlates.GetValue(templPlate.GetTemplPlateId());
		*/
	}
	if(pStdNodePlate==NULL)
		return false;
	pStdNodePlate->SetBelongModel(&Ta);
	pStdConnPoint->UpdateMatchStdPlateTmpl(pStdNodePlate);
	//pMatchConnPoint->cbMirCreateFlag;
	modeler.BuildModel(pStdNodePlate);
	//for(CLDSPart* pNewPart=modeler.m_xMapPartSet.GetFirst();pNewPart;pNewPart=modeler.m_xMapPartSet.GetNext())
	//	m_xMapPartSet.append(pNewPart);
	//pMatchConnPoint->cbMirCreateFlag
	return true;
}
#endif
long CIntelliModelDesigner::DesignConnectPointStructure(CConnectPointInfo *pConnectPoint,CIntelliModelDesigner* pIntelliDesigner/*=NULL*/)
{
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	CLDSNode *pBaseNode=Ta.Node.FromHandle(pConnectPoint->m_hBaseNode);
	CSuperSmartPtr<CLDSLinePart>pDatumRod=(CLDSLinePart*)Ta.Parts.FromHandle(pConnectPoint->m_hBasePart);
	BOOL bFinishDesign=FALSE;
#ifdef __SMART_DRAW_INC_
	if(pConnectPoint->designType.bFlJoint)			//设计对接法兰
	{
		pLinePart=Ta.FromRodHandle(pConnectPoint->m_hJointPart);
		if(pLinePart.blValid)
			DesignTubeFlJoint(pBaseNode,pDatumRod.LineTubePointer(),pLinePart.LineTubePointer());
		return 0;
	}
	else if(pConnectPoint->designType.bAngleJoint)	//满足设计角钢接头的条件
	{
		CJoint joint;
		CDesignJoint designJoint;
		joint.cfgword=Ta.GetDefaultCfgPartNo();
		pLinePart=Ta.FromRodHandle(pConnectPoint->m_hJointPart);
		if(!pLinePart.blValid)
			return 0;
		f3dPoint lenVec=pDatumRod->End()-pDatumRod->Start();
		if(pDatumRod->pEnd==pBaseNode)
			lenVec=pDatumRod->Start()-pDatumRod->End();
		if(lenVec.z<0&&pDatumRod->layer(0)!='T'&&pDatumRod->layer(1)=='Z')
		{
			joint.base_jg_handle_arr[0] = pLinePart->handle;
			joint.base_jg_handle_arr[1] = pDatumRod->handle;
		}
		else
		{
			joint.base_jg_handle_arr[0] = pDatumRod->handle;
			joint.base_jg_handle_arr[1] = pLinePart->handle;
		}
		if (pIntelliDesigner)
		{
			if (pIntelliDesigner->hashJoints.GetValue(DUALKEY(pDatumRod->handle, pLinePart->handle, true)))
				return 0;	//此接头已设计
			pIntelliDesigner->hashJoints.Add(DUALKEY(pDatumRod->handle, pLinePart->handle, true));
		}
		joint.base_node_handle = pBaseNode->handle;
		joint.SetBelongModel(&Ta);
		joint.SetLayer(pDatumRod->layer());
		joint.iSeg=pBaseNode->iSeg;
		designJoint.CreateJoint(&joint,TRUE,TRUE);
		if (UI::blEnableIntermediateUpdateUI)
		{
			pDatumRod->Create3dSolidModel();
			pLinePart->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pDatumRod->GetSolidPartObject());
			g_pSolidDraw->NewSolidPart(pLinePart->GetSolidPartObject());
			for(CLDSPart* pPart=joint.GetFirstPart();pPart;pPart=joint.GetNextPart())
			{
				pPart->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
				g_pSolidDraw->Draw();
			}
		}
		return 0;
	}
#endif
#ifdef __COMPLEX_PART_INC_
	if(pDatumRod->GetClassTypeId()==CLS_LINETUBE)
	{
		CCommonLjSmartDesign design;
		return design.DesignConnectPoint(pConnectPoint,0,TRUE);
	}
#endif
#ifdef __ANGLE_PART_INC_
	if (pConnectPoint->designType.bIntersBolt)		//设计交叉点螺栓
	{
		SmartDesignBoltOnlyConnect(pBaseNode,FALSE,FALSE);
		return 0;
	}
	if(pDatumRod->IsAngle())
	{
		char cWorkWing=0;
		if(pConnectPoint->m_iConnectWing==0)
			cWorkWing='X';
		else if(pConnectPoint->m_iConnectWing==1)
			cWorkWing='Y';
		PLATE plate(pConnectPoint->m_hBaseNode,pConnectPoint->m_hBasePart,cWorkWing),*pDesignPlate=NULL;
		pDesignPlate=pIntelliDesigner?pIntelliDesigner->hashPlates.GetValue(plate.StrKey()):NULL;
		if(pDesignPlate)
			return pDesignPlate->hPlate;
		bool bEnablePromptUI=pIntelliDesigner?pIntelliDesigner->m_bEnablePromptUI:true;
		CStackVariant<bool> stackprop(&CStdComModel::m_bEnablePromptUI,bEnablePromptUI);
		//UINT uidTmpl=0x02090001;//STDTEMPL_CONNPOINT_XH_DM1;//0;
		CLDSPartPtr xarrNewPartsPool[100];
		CNewPartsListen listen(&Ta.Parts,xarrNewPartsPool,100);
		int status = CStdComModel::IntelliDesignStdComNodePlate(pConnectPoint, NULL, 0, 0, false);
		if (status > 0)	//根据标准节点构造的模板设计失败（并非用户主动中断），此时依据传统规则继续生成节点板
		{	//注册新生成的连接板
			listen.EndListen();
			SmartPartPtr pNewPart;
			for (int i=0;i<listen.xarrNewParts.Count;i++)
			{
				pNewPart = listen.xarrNewParts[i];
				if (!pNewPart->IsPlate())
					continue;
				long hBasePart = pNewPart.pBasicPlate->m_hBaseRod;
				long hBaseNode = pNewPart.pBasicPlate->m_hBaseNode;
				CLDSLineAngle* pBaseAngle = (CLDSLineAngle*)Ta.Parts.FromHandle(hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if (pBaseAngle == NULL|| hBaseNode==0)
					continue;
				double jx = pNewPart->ucs.axis_z*pBaseAngle->vxWingNorm;
				double jy = pNewPart->ucs.axis_z*pBaseAngle->vyWingNorm;
				char cWorkWingX0Y1 = fabs(jx) > fabs(jy) ? 'X' : 'Y';
				PLATE* pRegPlate=pIntelliDesigner->RegisterPlate(hBaseNode, hBasePart, cWorkWingX0Y1);
				pRegPlate->hPlate = pNewPart->handle;
			}
			return true;
		}
		else if (status == -1)
			return false;
		//传统模式智能连接中的节点板不处理共线截断情况，故应剔除对接的接头杆件 wjh-2019.11.1
		for (pLinePart=pConnectPoint->linkPartSet.GetFirst();pLinePart.blValid;pLinePart=pConnectPoint->linkPartSet.GetNext())
		{
			if (pLinePart->handle==pConnectPoint->m_hJointPart)
				pConnectPoint->linkPartSet.DeleteCursor();
		}
		for (pLinePart=pConnectPoint->bendPlaneRodSet.GetFirst();pLinePart.blValid;pLinePart=pConnectPoint->bendPlaneRodSet.GetNext())
		{
			if (pLinePart->handle==pConnectPoint->m_hJointPart)
				pConnectPoint->bendPlaneRodSet.DeleteCursor();
		}
		for (pLinePart=pConnectPoint->bendPlaneRodSet2.GetFirst();pLinePart.blValid;pLinePart=pConnectPoint->bendPlaneRodSet2.GetNext())
		{
			if (pLinePart->handle==pConnectPoint->m_hJointPart)
				pConnectPoint->bendPlaneRodSet2.DeleteCursor();
		}
		if(pConnectPoint->m_hBendlineAngle==0&&pConnectPoint->bendPlaneRodSet.GetNodeNum()>0)
			pConnectPoint->m_hBendlineAngle=pConnectPoint->m_hBasePart;

		if(pDatumRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CGroupAngleLjDesign design;
			return design.DesignConnectPoint(pConnectPoint,TRUE);
		}
		else if(pConnectPoint->linkPartSet.GetNodeNum()==1)
		{
			//根据优先级设计连接
			for(int i=0;i<g_sysPara.m_arrL1RayPole.GetSize();i++)
			{
				CCommonLjSmartDesign design;
				if( g_sysPara.m_arrL1RayPole[i].CompareNoCase("L_1LB1")==0&&
					pConnectPoint->designType.bL_1LB1&&(plate.hPlate=DesignConnectL_1LB1(pConnectPoint))>0)
					break;
				else if(g_sysPara.m_arrL1RayPole[i].CompareNoCase("L_1LB2")==0&&
					pConnectPoint->designType.bL_1LB2&&(plate.hPlate=DesignConnectL_1LB2(pConnectPoint))>0)
					break;
				else if(g_sysPara.m_arrL1RayPole[i].CompareNoCase("L_1LE1")==0&&
					pConnectPoint->designType.bL_1LE1&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,1,TRUE))>0)
					break;
				else if(g_sysPara.m_arrL1RayPole[i].CompareNoCase("L_1LE2")==0&&
					pConnectPoint->designType.bL_1LE2&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,1,TRUE))>0)
					break;
				else if(g_sysPara.m_arrL1RayPole[i].CompareNoCase("L_1LO1")==0&&//负头钢板连接
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,0,TRUE))>0)
					break;
			}
		}
		else if(pConnectPoint->linkPartSet.GetNodeNum()==2)
		{	//双射线杆件
			//根据优先级设计连接
			CLDSLinePart* pRayRod1=pConnectPoint->linkPartSet.GetFirst();
			CLDSLinePart* pRayRod2=pConnectPoint->linkPartSet.GetNext();
			bool mirTypePlate=false;
			if(pRayRod1->MirSimilarGradeWith(pRayRod2,MIRMSG(1))<=1||pRayRod1->MirSimilarGradeWith(pRayRod2,MIRMSG(2))<=1)
				mirTypePlate=true;//射线对称时不应选择不对称的结构类型(如接腿正侧面中间八字铁连板
			for(int i=0;i<g_sysPara.m_arrL2RayPole.GetSize();i++)
			{
				CCommonLjSmartDesign design;
				if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LB1")==0&&pConnectPoint->bendplane.vSolid.IsZero()&&
					pConnectPoint->designType.bL_2LB1&&(plate.hPlate=DesignConnectL_2LB1(pConnectPoint))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LB2")==0&&pConnectPoint->bendplane.vSolid.IsZero()&&
					pConnectPoint->designType.bL_2LB2&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,FALSE))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LB3")==0&&pConnectPoint->bendplane.vSolid.IsZero()&&
					pConnectPoint->designType.bL_2LB3&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,FALSE))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LB4")==0&&pConnectPoint->bendplane.vSolid.IsZero()&&
					pConnectPoint->designType.bL_2LB4&&(plate.hPlate=DesignConnectL_2LB4(pConnectPoint))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LE1")==0&&
					pConnectPoint->designType.bL_2LE1&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,TRUE,FALSE))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LE2")==0&&
					pConnectPoint->designType.bL_2LE2&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,TRUE))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LC1")==0&&!mirTypePlate&&
					pConnectPoint->designType.bL_2LC1&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,1,TRUE))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LC2")==0&&!mirTypePlate&&
					pConnectPoint->designType.bL_2LC2&&(plate.hPlate=design.DesignConnectPoint(pConnectPoint,1,TRUE))>0)
					break;
				else if(g_sysPara.m_arrL2RayPole[i].CompareNoCase("L_2LO1")==0&&
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,0,TRUE))>0)
					break;
			}
		}
		//三射线杆件或多射线杆件
		else if(pConnectPoint->linkPartSet.GetNodeNum()==3)
		{	//根据优先级设计连接
			CLDSNode* pBaseNode=Ta.Node.FromHandle(pConnectPoint->m_hBaseNode);
			pDatumRod=Ta.FromRodHandle(pConnectPoint->m_hBasePart);
			if( pBaseNode==NULL||pDatumRod.IsNULL()||pDatumRod->pStart==NULL||pDatumRod->pEnd==NULL)
				return 0;
			CFGWORD cfgword=pBaseNode->cfgword&pDatumRod->cfgword;
			if(cfgword.IsNull())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("There is not sharing configure word of datum node(0X%X) and datum material(0X%X), possibly node parent rod is not in current module!",pBaseNode->handle,pDatumRod->handle);
#else
				logerr.Log("基准节点(0X%X)与基准材(0X%X)无共用配材号,可能节点父杆件不在当前呼高内!",pBaseNode->handle,pDatumRod->handle);
#endif
				return 0;
			}
			for(CLDSLinePart* pRod=pConnectPoint->linkPartSet.GetFirst();pRod;pRod=pConnectPoint->linkPartSet.GetNext())
			{
				cfgword&=pRod->cfgword;
				if(cfgword.IsNull())
				{
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("There is not sharing configure word of ray material(0X%X)and datum node(0X%X),datum material(0X%X),possibly node parent rod is not in current module!",pRod->handle,pBaseNode->handle,pDatumRod->handle);
#else
					logerr.Log("射线材(0X%X)与基准节点(0X%X)、基准材(0X%X)无共用配材号,可能节点父杆件不在当前呼高内!",pRod->handle,pBaseNode->handle,pDatumRod->handle);
#endif
					return 0;
				}
			}
			for(int i=0;i<g_sysPara.m_arrL3RayPole.GetSize();i++)
			{
				CCommonLjSmartDesign design;
				CSingleRayEndBoltLj design2;
				if(g_sysPara.m_arrL3RayPole[i].CompareNoCase("L_3LB1")==0&&pConnectPoint->bendplane.vSolid.IsZero()&&
					(plate.hPlate=design2.DesignConnectPoint(pConnectPoint))>0)
					break;
				else if(g_sysPara.m_arrL3RayPole[i].CompareNoCase("L_3LO1")==0&&
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,0,TRUE))>0)
					break;
				else if(g_sysPara.m_arrL3RayPole[i].CompareNoCase("L_3LC1")==0&&
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,TRUE))>0)		//优先中间射线材端连接
					break;
				else if(g_sysPara.m_arrL3RayPole[i].CompareNoCase("L_3LC2")==0&&
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,TRUE,1,FALSE))>0)//优先两边射线材端连接
					break;
				else if(g_sysPara.m_arrL3RayPole[i].CompareNoCase("L_3LC3")==0&&
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,1,TRUE))>0)
					break;
			}
		}
		else  if(pConnectPoint->linkPartSet.GetNodeNum()>3)
		{
			CCommonLjSmartDesign design;
			plate.hPlate=design.DesignConnectPoint(pConnectPoint,0,TRUE);
		}
		if (pIntelliDesigner&&plate.hPlate > 0 && pDatumRod->IsAngle())
			pIntelliDesigner->hashPlates.SetValue(plate.StrKey(), plate);
		return plate.hPlate;
	}
#endif
	//2.设计单螺栓连接
	//3.设计共用板或者异形法兰
	//4.设计普通连接板
	return 0;
}
#endif

void CLDSView::OnIntelligentConnectDesign() 
{
#ifdef __ANGLE_PART_INC_
	CLogErrorLife life;
	CLDSNode *pNode=NULL;
	CSuperSmartPtr<CLDSPart> pPart;
	CSuperSmartPtr<CLDSLinePart>pLinePart;
	NODESET nodeSet;	//节点集合
	long *id_arr=NULL;
	int i,serial,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CUndoOperObject undo(&Ta,true);
	if(n<=0)
	{	//选择构件
		OpenWndSel();	//开窗选择
		GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_NODE),CLS_NODE,0,2);
		scr_part_para.disp_type=DISP_LINE;
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="please select the node of intelligent connection design:";
		scr_part_para.cmdErrorFeedbackStr="no proper selected part,please select the node of intelligent connection design again:";
#else
		scr_part_para.cmdStr="请选择要进行智能连接设计的节点:";
		scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择要进行智能连接设计的节点:";
#endif
		if(!GetPartsFromScr(scr_part_para))
			return; 
		for(i=0;i<scr_part_para.nResultObjs;i++)
		{
			CLDSDbObject *pObj=scr_part_para.pResultObjsArr[i];
			if(pObj&&pObj->IsNode())
				nodeSet.append((CLDSNode*)pObj);
		}
	}
	else
	{	
		for(i=0;i<n;i++)
		{
			CLDSNode *pNode=Ta.Node.FromHandle(id_arr[i]);
			if(pNode)
				nodeSet.append(pNode);
		}
	}
	i=serial=0;
	int percent=0;
	Ta.DisplayProcess(0,NULL);
	/*struct PLATE{
		char cWorkWing;		//钢板定位基准角钢的工作肢（连接肢）
		long hAttachNode;	//钢板所附属的节点
		long hLocationAngle;//钢板定位基准角钢
		PLATE(long hNode,long hDatumAngle,char cAngleWorkWing)
		{
			hAttachNode=hNode;
			hLocationAngle=hDatumAngle;
			cWorkWing=cAngleWorkWing;
		}
		CXhChar50 StrKey(){return CXhChar50("%X-%X-%C",hLocationAngle,hAttachNode,cWorkWing);}
	};
	CHashStrList<PLATE> hashPlates;*/
	CIntelliModelDesigner intelli;
	intelli.m_bEnablePromptUI=nodeSet.GetNodeNum()<=gxModelGuide.m_nMaxNodeCountOfManualDesign;
	CStackVariant<char> stackprop0(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_BY_SUMM_THICK);
	//智能设计默认螺栓均为计入缺失的垫板间隙 wjh-2019.9.3
	CStackVariant<char> stackprop0i(&CLDSBolt::BOLTL0_PREFER_MODE,CLDSBolt::L0CAL_INC_GAP_THICK);
	CStackVariant<bool> stackprop1(&CStdComModel::m_bEnableMirCreate,false);
	CLDSPartPtr xarrNewPartsPool[500];
	CNewPartsListen listen(&Ta.Parts,xarrNewPartsPool,500);
	CDisplayView *pCurView=console.GetActiveView();
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext(),serial++)
	{	//智能识别当前连接点信息
		CONNECT_NODEINFO xConnNodeInfo;
		pNode->RetrieveConnectNodeInfo(&xConnNodeInfo,g_sysPara.intelligentConnect.m_bDesignBendConnect);
		if(xConnNodeInfo.ciTransitConnType==1)
		{	//变坡处设计
			CStackVariant<bool> stackprop(&CStdComModel::m_bEnablePromptUI, intelli.m_bEnablePromptUI);
			if(xConnNodeInfo.transit.pDownRod->GetClassTypeId()==CLS_LINEANGLE)
				CCommonLjSmartDesign::DesignSlopeTransition(&xConnNodeInfo.transit);
			else if(xConnNodeInfo.transit.pDownRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				CCommonLjSmartDesign::DesignSlopOrFootPoint(&xConnNodeInfo.transit);
			continue;
		}
		else if(xConnNodeInfo.ciTransitConnType==2)
		{	//塔脚板设计
			CCommonLjSmartDesign::DesignSlopOrFootPoint(&xConnNodeInfo.transit);
			continue;
		}
		//设计普通连接
		CConnectPointInfo xConnPointInfo;
		const BYTE xarrConstBytes[8]={ 0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80 };
		for(i=0;i<(int)xConnNodeInfo.xCommNode.planes.Count;i++)
		{
			CONNECT_PLANE* pConnPlane=&xConnNodeInfo.xCommNode.planes[i];
			if (!pConnPlane->blValid||(xarrConstBytes[i]&xConnNodeInfo.cbDisableFlag)>0)
				continue;	//当前连接面无效，或已经被之前代码抑制掉（可能已提前处理过） wjh-2019.10.6
			if(!pConnPlane->designType.bIntersBolt&&g_sysPara.intelligentConnect.m_bDesignCommonConnect)
			{
#ifdef __SMART_DRAW_INC_
				if(g_sysPara.intelligentConnect.m_bOnlyDesignCurView&&
					fabs(pCurView->ucs.axis_z*pConnPlane->datumplane.vSolid)<0.9)//EPS_COS2)
#elif defined(__COMMON_PART_INC_)			//放样软件仅支持当前面智能连接设计
				if(fabs(pCurView->ucs.axis_z*pConnPlane->datumplane.vSolid)<0.9)//EPS_COS2)
#endif
				{
					//pConnectList->DeleteCursor();
					continue;	//仅生成当前面钢板
				}
				if(xConnNodeInfo.GetConnectPointInfo(i,&xConnPointInfo))
					intelli.DesignConnectPointStructure(&xConnPointInfo,&intelli); //设计该连接点连接
			}
		}
		//设计交叉点螺栓
		for(i=0;i<(int)xConnNodeInfo.xCommNode.planes.Count;i++)
		{
			CONNECT_PLANE* pConnPlane=&xConnNodeInfo.xCommNode.planes[i];
			if (!pConnPlane->blValid||!pConnPlane->designType.bIntersBolt)
				continue;
			if(xConnNodeInfo.GetConnectPointInfo(i,&xConnPointInfo))
				intelli.DesignConnectPointStructure(&xConnPointInfo,&intelli); //设计该连接点交叉点螺栓连接
		}
		//设计角钢接头及钢管法兰等（由于垫圈、构件间隙等原因必须后设计）
#ifdef __SMART_DRAW_INC_
		if ( xConnNodeInfo.xCommNode.hJointRod>0&&(
			(g_sysPara.intelligentConnect.m_bDesignAngleJoint&&!xConnNodeInfo.xCommNode.blFlJoint)||
			(g_sysPara.intelligentConnect.m_bDesignFL&&xConnNodeInfo.xCommNode.blFlJoint)))
		{
			if (xConnNodeInfo.GetConnectPointInfo(4,&xConnPointInfo))
				intelli.DesignConnectPointStructure(&xConnPointInfo,&intelli); //设计该连接点角钢接头
		}
#endif
		percent=100*serial/nodeSet.GetNodeNum();
		Ta.DisplayProcess(percent,NULL);
	}
	Ta.DisplayProcess(100,NULL);
	listen.EndListen(CLDSBolt::BOLTL0_PREFER_MODE==CLDSBolt::L0CAL_INC_GAP_THICK);
	CLDSBoltPtr xarrBoltsPool[500]={ NULL };
	ARRAY_LIST<CLDSBoltPtr> xarrNewBolts(xarrBoltsPool,500,100);
	for (i=0;i<listen.xarrNewParts.Count;i++)
	{
		if (listen.xarrNewParts[i]->IsBolt())
			xarrNewBolts.append((CLDSBolt*)listen.xarrNewParts[i]);
	}
	stackprop0.PopStack();	//恢复根据螺栓属性计算，以便集中修正螺栓通厚 wjh-2019.9.3
	CLDSBolt::CorrectBoltZnL0Para(xarrNewBolts.m_pData,xarrNewBolts.Count);
	CLDSModule* pModule=Ta.GetActiveModule();
	for (i=0;i<xarrNewBolts.Count;i++)
	{
		xarrNewBolts[i]->CalGuigeAuto();
		if(IsNeedReverseBoltDirection(xarrNewBolts[i]))
			ReverseBoltDirection(xarrNewBolts[i],pModule);
		if (UI::blEnableIntermediateUpdateUI)
		{
			xarrNewBolts[i]->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(xarrNewBolts[i]->GetSolidPartObject());
		}
	}
	OnOperOther();	//取消开窗操作状态
	g_pSolidDraw->ReleaseSnapStatus();
	theApp.m_ePrevEnvType=theApp.m_eEnvType;
	theApp.m_eEnvType=SOLID_ENV;
	LdsWorkEnvShift(SOLID_ENV);	//转换到实体建模环境下显示新设计的连接
	g_pSolidDraw->Draw();
#endif
}
#include "KeypointLifeObj.h"
#include "EmbedMirInfoPanel.h"
struct TWIN_NODE{
	CLDSNode* pOrgNode,*pBranchNode;
	CLDSLinePart* pRayRod;
	TWIN_NODE(CLDSNode* _pOrgNode=NULL,CLDSNode* _pBranchNode=NULL,CLDSLinePart* _pRayRod=NULL){
		pOrgNode=_pOrgNode;
		pBranchNode=_pBranchNode;
		pRayRod=_pRayRod;
	}
};
void SetRelativeObj(CLDSDbObject *pObj);
CLDSNode* CreateSingleMirNode(CLDSNode *pSrcNode, MIRMSG msg,BOOL bIncAttachInfo=TRUE,char *sErrorInfo=NULL);
int CLDSView::OffsetRayRodEndNode(LINEPARTSET& linePartSet)
{
	CString cmdStr;
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CEmbedMirInfoPanel xCmdMiniPanel;
	/*long *id_arr;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int i=0;i<n;i++)
	{
		CLDSPart* pPart=console.FromPartHandle(id_arr[i]);
		if(pPart==NULL || !pPart->IsLinePart())
			continue;
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart && pLinePart->pEnd)
			linePartSet.append(pLinePart);
	}
	if(linePartSet.GetNodeNum()<=0)
	{	//选择要偏移端节点的射线杆件
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSnap->SetSelectPartsType(SELECT_LINEPART);
		OpenWndSel();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("OffsetEndNode Please select all ray-rods :","");
#else 
		pCmdLine->FillCmdLine("OffsetEndNode 请选择进行偏移端节点的射线杆件:","");
#endif
		while(1)
		{
			if(pCmdLine->GetStrFromCmdLine(cmdStr)==FALSE)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			linePartSet.Empty();
			long *id_arr;
			int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<n;i++)
			{
				CLDSPart* pPart=console.FromPartHandle(id_arr[i]);
				if(pPart==NULL || !pPart->IsLinePart())
					continue;
				CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
				if(pLinePart->pStart && pLinePart->pEnd)
					linePartSet.append(pLinePart);
			}
			if(linePartSet.GetNodeNum()>0)
				break;
		}
	}*/
	//选择要偏移的节点
	CLDSNode* pBaseNode=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("BranchNodes Please select the datum node :","");
#else 
	pCmdLine->FillCmdLine("BranchNodes 请选择要进行拆分的杆件端节点:","");
#endif
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verify(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verify.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	//{原UNDO作用域，现此函数已作为子函数，作用域就没意义了 wjh-2017.12.21
	CDisplayNodeAtFrontLife displayNode;
	displayNode.DisplayNodeAtFront();
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pBaseNode=console.FromNodeHandle(obj.hRelaObj);
		if(pBaseNode)
		{
			CLDSLinePart* pLinePart=NULL;
			for(pLinePart=linePartSet.GetFirst();pLinePart;pLinePart=linePartSet.GetNext())
			{
				if(pLinePart->pStart!=pBaseNode && pLinePart->pEnd!=pBaseNode)
				{
					AfxMessageBox(CXhChar50("选中杆件0x%X的端节点不是当前选中的节点",pLinePart->handle));
					break;
				}
			}
			if(pLinePart==NULL)
				break;
		}
	}
	//}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseNode->handle));
	//输入偏移量
	CLDSLinePart* pBaseRod=(CLDSLinePart*)console.FromPartHandle(pBaseNode->hFatherPart,CLS_LINEPART);
	if(pBaseRod==NULL)
	{
		AfxMessageBox("选择的基准节点没有父杆件!");
		return FALSE;
	}
	UCS_STRU ucs=pBaseRod->BuildUCS();
	f3dPoint datum_pos=pBaseNode->ActivePosition();
	GEPOINT vRayVec,base_vec=pBaseRod->End()-pBaseRod->Start();
	normalize(base_vec); 
	int sign=1;
	CLDSLinePart* pCurrPickRayRod=NULL;
	if(linePartSet.GetNodeNum()==1)
	{
		pCurrPickRayRod=linePartSet.GetFirst();
		if(pCurrPickRayRod->pStart!=NULL&&pCurrPickRayRod->pEnd!=NULL)
		{
			vRayVec=pCurrPickRayRod->pEnd->xPreliftPos-pCurrPickRayRod->pStart->xPreliftPos;
			if(pCurrPickRayRod->pEnd==pBaseNode)
				vRayVec*=-1;
			if(base_vec*vRayVec<0)
				sign=-1;
		}
	}
	double fOffsetDist=40;
	if(!xCmdMiniPanel.m_bSpecOffset)	//未启用指定值时，总默认为当前偏移值
		xCmdMiniPanel.m_nSpecOffset=40;//fOffsetDist;
	else	//启用指定值时，总默认为上一次指定偏移值
		fOffsetDist=abs((int)xCmdMiniPanel.m_nSpecOffset);
	char ciMirBranchNodeMode=0;
	{
		btc::SKETCH_PLANE sketch(datum_pos,base_vec,ucs.axis_x,300);
		CWorkPlaneSketchLifeObject workPlaneSketchLife;
		workPlaneSketchLife.WorkPlaneSketch(sketch,sign*base_vec,"偏移方向",100);
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine((char*)CXhChar100("Please Input offset distance<%.f>:",fOffsetDist),"");
#else 
		pCmdLine->FillCmdLine((char*)CXhChar100("请选择偏移基准杆件，或输入偏移值<%.f>:",fOffsetDist),"");
		//pCmdLine->FillCmdLine((char*)CXhChar100("BranchNodes 请输入偏移距<%.f>:",fOffsetDist),"");
#endif
		CEmbedMirInfoPanel xCmdMiniPanel;
		if(ciMirBranchNodeMode==0)
			pCmdLine->AttachMiniCmdOptionPanel(&xCmdMiniPanel,100);
		CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_LINE);
		verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_LINEPART);
		DWORD dwhObj=0,dwExportFlag=0;
		PEEKCMDLINE cmd_detect("",pCmdLine);
		while(1)
		{
			//if(pCmdLine->GetStrFromCmdLine(cmdStr)==FALSE)
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier,PEEKCMDLINE::MatchKeyword,&cmd_detect)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
			{
				pCmdLine->PeekStrFromCmdLine(cmdStr);
				if(cmdStr.GetLength()>0)
					fOffsetDist=atof(cmdStr);
				if(fabs(fOffsetDist)>0)
					break;
			}
			else if(obj.ciTriggerType==SELOBJ::TRIGGER_LBUTTON)
			{
				CLDSLinePart *pRod=console.FromRodHandle(obj.hRelaObj);
				if(pRod==NULL)
					continue;
				pBaseRod=pRod;
				base_vec=pBaseRod->End()-pBaseRod->Start();
				normalize(base_vec); 
				if(pCurrPickRayRod!=NULL)
				{
					sign=(base_vec*vRayVec<0)?-1:1;
					workPlaneSketchLife.Clear();
					btc::SKETCH_PLANE sketch2(datum_pos,base_vec,ucs.axis_x,300);
					workPlaneSketchLife.WorkPlaneSketch(sketch2,sign*base_vec,"偏移方向");
					g_pSolidDraw->Draw();
				}
				continue;	//调整基准偏移方向后，依然应确认偏移距离
			}
		}
		fOffsetDist*=sign;
		if(ciMirBranchNodeMode==0)
			ciMirBranchNodeMode=xCmdMiniPanel.m_bMirAll?'Y':'N';
	}
	xCmdMiniPanel.m_nSpecOffset=ftoi(abs(fOffsetDist));
	f3dPoint offset_pos=datum_pos+base_vec*fOffsetDist;
	//生成偏移点，并更新射线杆件端节点
	CUndoOperObject undo(&Ta,true);
	CLDSNode* pNewNode = Ta.CheckOverlappedNode(offset_pos,CFGINFO(pBaseRod->cfgword,pBaseRod->IsLegObj()));
	if(pNewNode==NULL)
	{
		pNewNode = console.AppendNode();
		pNewNode->m_cPosCalType = 7;
		pNewNode->arrRelationNode[0]=pBaseNode->handle;
		pNewNode->arrRelationNode[1]=pBaseRod->pStart->handle;
		pNewNode->arrRelationNode[2]=pBaseRod->pEnd->handle;
		pNewNode->arrRelationPole[0]=pBaseRod->handle;
		pNewNode->attach_offset=fOffsetDist;
		pNewNode->SetPosition(offset_pos);
		pNewNode->hFatherPart=pBaseRod->handle;
		pNewNode->cfgword=pBaseRod->cfgword;
		pNewNode->iSeg=pBaseRod->iSeg;
		pNewNode->SetLayer(pBaseRod->layer());
		pNewNode->layer(1)='P';
		NewNode(pNewNode);
	}
	CLDSLinePart* pRayRod;
	PRESET_ARRAY4<TWIN_NODE> relaBranchNodes;
	if(ciMirBranchNodeMode=='Y')
	{
		for(pRayRod=linePartSet.GetFirst();pRayRod;pRayRod=linePartSet.GetNext())
		{
			if(pRayRod->pStart!=pBaseNode&&pRayRod->pEnd!=pBaseNode)
				continue;
			relaBranchNodes.Append(TWIN_NODE(pBaseNode,pNewNode,pRayRod));
			for(RELATIVE_OBJECT* pRelaObj=pBaseNode->relativeObjs.GetFirst();pRelaObj;pRelaObj=pBaseNode->relativeObjs.GetNext())
			{
				CLDSNode* pMirNode=Ta.Node.FromHandle(pRelaObj->hObj);
				if(pMirNode==NULL)
					continue;
				CLDSLinePart* pMirRod=pRayRod->GetMirRod(pRelaObj->mirInfo);
				if(pMirRod==NULL)
					continue;
				CLDSNode* pMirOffsetNode=CreateSingleMirNode(pNewNode,pRelaObj->mirInfo,TRUE);
				relaBranchNodes.Append(TWIN_NODE(pMirNode,pMirOffsetNode,pMirRod));
				if(pMirRod->pStart==pMirNode)
					pMirRod->pStart=pMirOffsetNode;
				else if(pMirRod->pEnd==pMirNode)
					pMirRod->pEnd=pMirOffsetNode;
				pMirRod->CalPosition(true);
				pMirRod->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pMirRod->GetSolidPartObject());
			}
		}
		SetRelativeObj(pNewNode);
	}
	//更新射线角钢的端节点信息
	for(pRayRod=linePartSet.GetFirst();pRayRod;pRayRod=linePartSet.GetNext())
	{
		//更新当前主射线角钢的端节点信息
		if(pRayRod->pStart==pBaseNode)
			pRayRod->pStart=pNewNode;
		else
			pRayRod->pEnd  =pNewNode;
		pRayRod->CalPosition(true);
		pRayRod->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pRayRod->GetSolidPartObject());
	}

	//更新依赖杆件为该射线角钢的交叉节点设计信息
	BOOL pushed=console.PushNodeStack();
	CNodeMoveOperObject xNodeMoveObj;
	for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
	{	
		if(pNode->m_cPosCalType==0)
			continue;
		BOOL bNeedMove=FALSE;
		for(UINT k=0;!bNeedMove&&k<relaBranchNodes.Count;k++)
		{
			CLDSNode* pCurrNode   =relaBranchNodes.At(k).pOrgNode;
			CLDSNode* pBranchNode =relaBranchNodes.At(k).pBranchNode;
			CLDSLinePart* pCurrRod=relaBranchNodes.At(k).pRayRod;
			if(pNode==pCurrNode || pNode==pBranchNode)
				continue;
			//CNodeMoveOperObject xNodeMoveObj;
			if(pNode->m_cPosCalType==4)
			{	//交叉节点
				if(pNode->arrRelationPole[0]!=pCurrRod->handle&&pNode->arrRelationPole[1]!=pCurrRod->handle)
					continue;
				for(int i=0;i<4;i++)
				{
					if(pNode->arrRelationNode[i]==pCurrNode->handle)
					{
						pNode->arrRelationNode[i]=pBranchNode->handle;
						bNeedMove=TRUE;
						break;
					}
				}
			}
			else if(pNode->hFatherPart==pCurrRod->handle)
			{	//当前射线杆件上的其他节点
				bNeedMove=TRUE;
				for(int i=0;i<4;i++)
				{
					if(pNode->arrRelationNode[i]==pCurrNode->handle)
						pNode->arrRelationNode[i]=pBranchNode->handle;
				}
			}
		}
		if(bNeedMove)
		{	//更新节点坐标
			pNode->CalPosition(true);
			g_pSolidDraw->ModifyDbPoint(pNode->handle,pNode->xActivePos);
		}
	}
	console.PopNodeStack(pushed);

	g_pSolidDraw->Draw();
	g_pSolidDraw->ReleaseSnapStatus();
	return TRUE;
}
//智能横担连接
#include "CrossArmDesignDlg.h"
static long GetGlobalNewHandle()
{
	return Ta.GetNewHandle();
}
/* 该功能已融入新版的CParaTemplate中 wjh-2014.12.18
void CLDSView::OnDesignCrossArm()
{
	//1、设定模版模型
	CLogErrorLife logerrLife;
	CParaBlockModel model;
	CParaBlockModelTemplate para_template;
	para_template.m_iTempType=CParaBlockModelTemplate::CROSSARM_TEMPLATE;
	model.GetNewHandleHookFunc=GetGlobalNewHandle;
	model.m_pTemplate=&para_template;
	//2、设计对话框，显示及修改模版参数
	CCrossArmDesignDlg crossArmDlg;
	crossArmDlg.m_pModel=&model;
	if(crossArmDlg.DoModal()!=IDOK)
		return;
	//3、开窗选择横担模版的主控节点
	CLDSNode* keyNodeArr[4]={NULL};
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString sCmdStr;
	sCmdStr.Format("请选择横担模板关键节点(共%d个)！",crossArmDlg.m_nKeyNodeNum);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidSnap->SetSnapType(SNAP_POINT);
	pCmdLine->FillCmdLine(sCmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(sCmdStr))
			return;
		long *id_arr;
		int nSelNodes=g_pSolidSnap->GetLastSelectEnts(id_arr);
		BOOL bFindNode=TRUE;
		if(nSelNodes!=crossArmDlg.m_nKeyNodeNum)
		{
			pCmdLine->FinishCmdLine();
			sCmdStr.Format("没有选中合适的节点，请重新选择关键节点(共%d个):",crossArmDlg.m_nKeyNodeNum);
			pCmdLine->FillCmdLine(sCmdStr,"");
			g_pSolidDraw->ReleaseSnapStatus();
			continue;
		}
		for(int iSel=0;iSel<nSelNodes;iSel++)
		{
			keyNodeArr[iSel]=console.FromNodeHandle(id_arr[iSel]);
			if(keyNodeArr[iSel]==NULL)
			{
				bFindNode=FALSE;
				break;
			}
		}
		if(bFindNode==FALSE)
		{
			pCmdLine->FinishCmdLine();
			sCmdStr.Format("没有选中合适的节点，请重新选择关键节点(共%d个):",crossArmDlg.m_nKeyNodeNum);
			pCmdLine->FillCmdLine(sCmdStr,"");
			g_pSolidDraw->ReleaseSnapStatus();
			continue;
		}
		else
		{
			pCmdLine->FinishCmdLine();
			g_pSolidDraw->ReleaseSnapStatus();
			break;
		}
	}
	//如果选择两个主控节点，通过对称关系找到其他主控节点
	if(keyNodeArr[2]==NULL || keyNodeArr[3]==NULL)
	{
		CLDSNode* pMirNode1=keyNodeArr[0]->GetMirNode(MIRMSG(crossArmDlg.m_iKeyNodeMir));
		CLDSNode* pMirNode2=keyNodeArr[1]->GetMirNode(MIRMSG(crossArmDlg.m_iKeyNodeMir));
		if(pMirNode1==NULL || pMirNode2==NULL)
		{
			logerr.Log("没有找到对称主控节点!");
			return;
		}
		keyNodeArr[2]=pMirNode1;
		keyNodeArr[3]=pMirNode2;
	}

	//4、建立横担模型
	for(int i=0;i<4;i++)
	{
		model.keyNodeSet.append(keyNodeArr[i]);
		CLDSPart* pPart=console.FromPartHandle(keyNodeArr[i]->hFatherPart);
		model.keyPartSet.append(pPart);
	}
	model.RebuildModel();
	//5、生成横担的节点、杆件
	Ta.BeginUndoListen();
	CLDSNode* pTemNode=NULL;
	for(CLDSNode* pNode=model.Node.GetFirst();pNode;pNode=model.Node.GetNext())
	{	
		if(Ta.FromNodeHandle(pNode->handle))	//过滤主控节点
			continue;
		pTemNode=Ta.Node.append();
		pTemNode->handle=pNode->handle;
		pTemNode->SetPosition(pNode->Position());
		pTemNode->CopyProperty(pNode);
		pTemNode->cfgword=Ta.GetDefaultCfgPartNo();
		pTemNode->hFatherPart=pNode->hFatherPart;
		if(pNode->m_cPosCalType>0&&pNode->m_cPosCalType<=5)
		{
			pTemNode->arrRelationNode[0]=pNode->arrRelationNode[0];
			pTemNode->arrRelationNode[1]=pNode->arrRelationNode[1];
			pTemNode->arrRelationPole[0]=pNode->arrRelationPole[0];
		}
		if(pNode->m_cPosCalType==4)
		{
			pTemNode->arrRelationNode[2]=pNode->arrRelationNode[2];
			pTemNode->arrRelationNode[3]=pNode->arrRelationNode[3];	
			pTemNode->arrRelationPole[0]=pNode->arrRelationPole[0];
			pTemNode->arrRelationPole[1]=pNode->arrRelationPole[1];
		}
		NewNode(pTemNode);
	}
	Ta.Node.RebuildHashtable();
	CLDSLinePart* pTemLinePart=NULL;
	CBuffer clonebuf;
	for(CLDSLinePart* pLinePart=(CLDSLinePart*)model.Parts.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)model.Parts.GetNext())
	{
		if(Ta.FromNodeHandle(pLinePart->handle))	//过滤主控杆件
			continue;
		pTemLinePart=(CLDSLinePart*)Ta.AppendPart(pLinePart->GetClassTypeId());
		clonebuf.SeekToBegin();
		pLinePart->ToBuffer(clonebuf);
		clonebuf.SeekToBegin();
		pTemLinePart->FromBuffer(clonebuf);
		pTemLinePart->handle=pLinePart->handle;
		pTemLinePart->cfgword=Ta.GetDefaultCfgPartNo();
		NewLinePart(pTemLinePart);
		g_pSolidDraw->SetEntSnapStatus(pTemLinePart->handle);
	}
	Ta.Parts.RebuildHashtable();
	Ta.EndUndoListen();
}
*/