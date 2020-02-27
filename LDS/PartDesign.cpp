//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "lds_part.h"
#include "lds_co_part.h"
#include "DesignJdb.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "Query.h"
#include "dlg.h"
#include "LmaDlg.h"
#include "JdbDlg.h"
#include "DesignJoint.h"
#include "ScrTools.h"
#include "PlateBasicParaDlg.h"
#include "LayAngleBoltDlg.h"
#include "LayTubeBoltDlg.h"
#include "LayFillPlankDlg.h"
#include "InputAnValDlg.h"
#include "DesignLjParaDlg.h"
#include "DianBanParaDlg.h"
#include "DesignFootNailPlateDlg.h"
#include "f_ent.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"
#include "MirTaAtom.h"
#include "DesignOneBoltDlg.h"
#include "SnapTypeVerify.h"
#include "KeypointLifeObj.h"
#include "SnapTypeVerify.h"
#include "AngleBoltsDlg.h"
#include "env_def.h"
#include "UI.h"
#include "SelNailSeriesDlg.h"
#include "DesPedalPlateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if defined(__TSA_)||defined(__TSA_FILE_)
void CLDSView::OnShareJdbDesign(){;}
void CLDSView::OnCommonJgJoint(){;}
void CLDSView::OnLayoutFootnail(){;}
void CLDSView::OnFootPlank(){;}
void CLDSView::OnDesHoofPlank(){;} 
void CLDSView::OnCutJg(){;}
void CLDSView::OnDesignJdb(){;} 
void CLDSView::OnFoldPlank(){;}
void CLDSView::OnLayJgEndLs(){;}
void CLDSView::OnOneBoltDesign(){;}
void CLDSView::OnSpecNodeOneBoltDesign(){;}
void CLDSView::OnSingleXieNodeDesign(){;}
void CLDSView::OnXieIntersPtDesign(){;}
void CLDSView::OnAllSingleXieNodeDesign(){;} 
#else
//计算共享板射线角钢与基准角钢的交汇点(准线交汇点)
//ucs为当前共享板的相对坐标系
static f3dPoint CalSharePlankRayZhun(CLDSLineAngle *pBaseJg[2], CLDSNode *pBaseNode[2],
									 CLDSLineAngle *pRayJg, f3dPoint work_norm)
{
	f3dPoint zhun,wing_vec1,wing_vec2;
	f3dLine line1,line2;
	JGZJ jgzj1,jgzj2;
	getjgzj(jgzj1,pBaseJg[0]->GetWidth());
	getjgzj(jgzj2,pBaseJg[1]->GetWidth());
	int x_wing0_y_wing1;
	IsInsideJg(pBaseJg[0],work_norm,&x_wing0_y_wing1);
	if(pBaseJg[0]->m_bEnableTeG)
	{
		if(x_wing0_y_wing1==0)
			jgzj1 = pBaseJg[0]->xWingXZhunJu;
		else
			jgzj1 = pBaseJg[0]->xWingYZhunJu;
	}
	IsInsideJg(pBaseJg[1],work_norm,&x_wing0_y_wing1);
	if(pBaseJg[1]->m_bEnableTeG)
	{
		if(x_wing0_y_wing1==0)
			jgzj2 = pBaseJg[1]->xWingXZhunJu;
		else
			jgzj2 = pBaseJg[1]->xWingYZhunJu;
	}
	if(pBaseNode[0]==pBaseNode[1])	//两基准角钢同一基准节点
	{
		if(fabs(pBaseJg[0]->get_norm_x_wing()*work_norm) >
			fabs(pBaseJg[0]->get_norm_y_wing()*work_norm))
			wing_vec1=pBaseJg[0]->GetWingVecX();
		else
			wing_vec1=pBaseJg[0]->GetWingVecY();
		if(fabs(pBaseJg[1]->get_norm_x_wing()*work_norm) >
			fabs(pBaseJg[1]->get_norm_y_wing()*work_norm))
			wing_vec2=pBaseJg[1]->GetWingVecX();
		else
			wing_vec2=pBaseJg[1]->GetWingVecY();
		
		line1.startPt=pBaseJg[0]->Start()+wing_vec1*jgzj1.g;
		line1.endPt=pBaseJg[0]->End()+wing_vec1*jgzj1.g;
		line2.startPt=pBaseJg[1]->Start()+wing_vec2*jgzj2.g;
		line2.endPt=pBaseJg[1]->End()+wing_vec2*jgzj2.g;
		UCS_STRU ucs;
		ucs.axis_z = work_norm;
		ucs.axis_x = line1.endPt-line1.startPt;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		if(pBaseJg[0]->pStart==pBaseNode[0])
			ucs.origin = line1.startPt;
		else
			ucs.origin = line1.endPt;
		coord_trans(line1.startPt,ucs,FALSE);
		coord_trans(line1.endPt,ucs,FALSE);
		coord_trans(line2.startPt,ucs,FALSE);
		coord_trans(line2.endPt,ucs,FALSE);
		line1.startPt.z = line1.endPt.z = line2.startPt.z = line2.endPt.z = 0;
		Int3dpl(line1,line2,zhun);
		coord_trans(zhun,ucs,TRUE);
	}
	else if(pRayJg)	//两基准角钢各有自己的基准节点
	{
		if(pRayJg->pStart==pBaseNode[0]||pRayJg->pEnd==pBaseNode[0])
		{
			if(fabs(dot_prod(pBaseJg[0]->get_norm_x_wing(),work_norm)) >
				fabs(dot_prod(pBaseJg[0]->get_norm_y_wing(),work_norm)))
				wing_vec1=pBaseJg[0]->GetWingVecX();
			else
				wing_vec1=pBaseJg[0]->GetWingVecY();
			if(pBaseJg[0]->pStart==pBaseNode[0])
				zhun=pBaseJg[0]->Start()+wing_vec1*jgzj1.g;
			else
				zhun=pBaseJg[0]->End()+wing_vec1*jgzj1.g;
		}
		else	//(pRayJg->pStart==pBaseNode[1]||pRayJg->pEnd==pBaseNode[1])
		{
			if(fabs(dot_prod(pBaseJg[1]->get_norm_x_wing(),work_norm)) >
				fabs(dot_prod(pBaseJg[1]->get_norm_y_wing(),work_norm)))
				wing_vec2=pBaseJg[1]->GetWingVecX();
			else
				wing_vec2=pBaseJg[1]->GetWingVecY();
			if(pBaseJg[1]->pStart==pBaseNode[1])
				zhun=pBaseJg[1]->Start()+wing_vec2*jgzj2.g;
			else
				zhun=pBaseJg[1]->End()+wing_vec2*jgzj2.g;
		}
	}
	else
	{
		if(fabs(dot_prod(pBaseJg[0]->get_norm_x_wing(),work_norm)) >
			fabs(dot_prod(pBaseJg[0]->get_norm_y_wing(),work_norm)))
			wing_vec1=pBaseJg[0]->GetWingVecX();
		else
			wing_vec1=pBaseJg[0]->GetWingVecY();
		if(pBaseJg[0]->pStart==pBaseNode[0])
			Sub_Pnt(zhun,pBaseJg[0]->Start(),wing_vec1*jgzj1.g);
		else
			Sub_Pnt(zhun,pBaseJg[0]->End(),wing_vec1*jgzj1.g);
	}
	return zhun;
}

#ifdef ajkdlfdaadfadfadfa	//暂不使用						 
void CLDSView::OnShareJdbDesign() 
{	
	m_nPrevCommandID=ID_SHARE_JDB_DESIGN;
	m_sPrevCommandName="重复共用板";
	CLDSPlate *pCurPlate=NULL;
	CLDSNode *pBaseNode[2]={NULL};
	CLDSLineAngle *pRayJg=NULL,*pBaseJg[2]={NULL};
	CDesignLjPartPara *pRayJgPara=NULL,*pLjJgParaArr[2]={NULL};
	int i;
	double quitdist[2]={0.0};
	double base_wing_wide[2],base_wing_thick[2],ray_wing_wide,ray_wing_thick;
	LSSPACE_STRU basespace[2],rayspace;
	f3dPoint vec;
	//-----vvvvvvv-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
	if(!LockFunc())
		return;
	UINT nRetCode=1;	//现已不需要检测加密锁状态了 wjh-2017.9.18
	Ta.BeginUndoListen();
	try
	{
#ifdef DOG_CHECK
		if(nRetCode!=1)
			throw "检测加密狗出错,程序出错!";
#endif
		pCurPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pCurPlate->face_N = 1;
		pCurPlate->jdb_style = 6;	//共用板
		f3dLine line3d;
		f3dPoint *point3d=NULL;
		//切换到单线显示模式
		g_pSolidSet->SetDisplayType(DISP_LINE);
		Invalidate(FALSE);
		for(i=0;i<2;i++)
		{
			if(i==0)
				g_pPromptMsg->SetMsg("请选择共用板的第一根基准角钢");
			else
				g_pPromptMsg->SetMsg("请选择共用板的第二根基准角钢");
			while(pBaseJg[i]==NULL)
			{
				if(g_pSolidSnap->SnapLine(line3d)>0)
				{
					pBaseJg[i] = (CLDSLineAngle*)console.FromPartHandle(line3d.ID,CLS_LINEANGLE);
					pLjJgParaArr[i]=pCurPlate->designInfo.partList.append();
					pLjJgParaArr[i]->m_bAdjustLjEndPos=FALSE;	//共用板两根基准杆件对称时都不需要调整摆放位置 wht 10-02-26
					pLjJgParaArr[i]->hPart=pBaseJg[i]->handle;
				}
			}
			g_pSolidDraw->SetEntSnapStatus(line3d.ID);
			if(i==1)//高亮显示半秒钟,不然用户不知道是否选中了此构件显示状态就被冲掉了
				Sleep(500);	
		}
		
		for(i=0;i<2;i++)
		{
			if(i==0)
				g_pPromptMsg->SetMsg("请选择共用板的第一个基准节点(应为第一根基准角钢的端节点");
			else
				g_pPromptMsg->SetMsg("请选择共用板的第二个基准节点(应为第二根基准角钢的端节点");
			//高亮显示当前的基准角钢
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pBaseJg[i]->handle);
			while(pBaseNode[i]==NULL)
			{
				if(g_pSolidSnap->SnapPoint (point3d,TRUE)>0)
				{
					pBaseNode[i] = console.FromNodeHandle(point3d->ID);
					if(i==0)	//第一个基准节点为连接板基准节点
					{
						if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseNode[0]->dwPermission))
							throw "没有此节点的修改权限！";
						pCurPlate->designInfo.m_hBaseNode = point3d->ID;
					}
				}
				else	//中止设计	
					throw "中途退出,设计失败!";
			}
			g_pSolidDraw->SetEntSnapStatus(point3d->ID);
			CSharePlankDesignDlg dlg;
			if( pBaseNode[i]==pBaseJg[i]->pStart)
			{	
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwStartPermission))
					throw "没有此角钢始端的修改权限！";
				pBaseJg[i]->feature = 10;	//始端连接
			}
			else if(pBaseNode[i]==pBaseJg[i]->pEnd)
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwEndPermission))
					throw "没有此角钢终端的修改权限！";
				pBaseJg[i]->feature = 11;	//终端连接
			}
			else
				throw "所选基准角钢与基准节点不一致,操作无效";
			if(i==0)
				g_pPromptMsg->SetMsg("请输入第一根角钢的上连接板边缘的后撤量");
			else
				g_pPromptMsg->SetMsg("请输入第二根角钢的上连接板边缘的后撤量");
			if(pBaseJg[i]->feature==10)
			{
				dlg.m_sLsGuiGe.Format("%d",pBaseJg[i]->connectStart.d);
				GetLsSpace(basespace[i],pBaseJg[i]->connectStart.d);
				dlg.m_nLsN = pBaseJg[i]->connectStart.wnConnBoltN;
				dlg.m_nLsSpace = basespace[i].SingleRowSpace;
				//插空布置螺栓
				dlg.m_fOddment = basespace[i].EndSpace+basespace[i].SingleRowSpace/2;//端距 
				pLjJgParaArr[i]->end_space=basespace[i].EndSpace;
				pLjJgParaArr[i]->start0_end1=0;
			}
			else
			{
				dlg.m_sLsGuiGe.Format("%d",pBaseJg[i]->connectEnd.d);
				dlg.m_nLsN = pBaseJg[i]->connectEnd.wnConnBoltN;
				GetLsSpace(basespace[i],pBaseJg[i]->connectEnd.d);
				dlg.m_nLsSpace = basespace[i].SingleRowSpace;
				//插空布置螺栓
				dlg.m_fOddment = basespace[i].EndSpace+basespace[i].SingleRowSpace/2;//端距
				pLjJgParaArr[i]->end_space=basespace[i].EndSpace;
				pLjJgParaArr[i]->start0_end1=1;
			}
			if(dlg.DoModal()!=IDOK)
				throw "设计撤消";
			if(pBaseJg[i]->feature==10)
			{
				sscanf(dlg.m_sLsGuiGe,"%d",&pBaseJg[i]->connectStart.d);
				pBaseJg[i]->connectStart.wnConnBoltN = dlg.m_nLsN;
			}
			else
			{
				sscanf(dlg.m_sLsGuiGe,"%d",&pBaseJg[i]->connectEnd.d);
				pBaseJg[i]->connectEnd.wnConnBoltN = dlg.m_nLsN;
			}
			basespace[i].SingleRowSpace = dlg.m_nLsSpace;
			quitdist[i]=dlg.m_fOddment-basespace[i].EndSpace;	//后撤量
			g_pSolidDraw->ReleaseSnapStatus();	//清除基准角钢的高亮显示状态
		}
		//-------共用板相对坐标系的建立------------
		f3dPoint vec1,pt1,pt2;
		//更正两基准角钢严格平行时的法线计算错误 WJH-2004/07/01
		if(pBaseJg[0]->feature==10)
			vec1 = pBaseJg[0]->GetDatumPosBer(pBaseJg[0]->pEnd)-pBaseJg[0]->GetDatumPosBer(pBaseJg[0]->pStart);
		else
			vec1 = pBaseJg[0]->GetDatumPosBer(pBaseJg[0]->pStart)-pBaseJg[0]->GetDatumPosBer(pBaseJg[0]->pEnd);
		normalize(vec1);
		/*f3dPoint vec2;
		if(pBaseJg[1]->feature==10)
		vec2 = pBaseJg[1]->GetDatumPosBer(pBaseJg[1]->pEnd)-pBaseJg[1]->GetDatumPosBer(pBaseJg[1]->pStart);
		else
		vec2 = pBaseJg[1]->GetDatumPosBer(pBaseJg[1]->pStart)-pBaseJg[1]->GetDatumPosBer(pBaseJg[1]->pEnd);
		normalize(vec2);*/
		//这样造成的误差可能较大，因为任意面的法线计算
		//可能与板的法线计算算法不同两都如果严重不一致时，
		//就会造成较大的误差
		//if(m_eViewFlag==RANDOM_VIEW)
		//pCurPlate->ucs.axis_z = g_ucs.axis_z;
		//else
		{
			//更正两基准角钢严格平行时的法线计算错误 WJH-2004/07/01
			pCurPlate->ucs.axis_z = CalFaceNorm(pBaseJg[0]->Start(),pBaseJg[0]->End(),pBaseJg[1]->Start(),pBaseJg[1]->End());//cross_prod(vec1,vec2);
			normalize(pCurPlate->ucs.axis_z);
			if(dot_prod(console.GetActiveView()->ucs.axis_z,pCurPlate->ucs.axis_z)<0)
				pCurPlate->ucs.axis_z = -1.0*pCurPlate->ucs.axis_z;
		}
		pCurPlate->cfgword=pBaseNode[0]->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pCurPlate->ucs.origin = CalSharePlankRayZhun(pBaseJg,pBaseNode,NULL,pCurPlate->ucs.axis_z);
		pCurPlate->designInfo.iProfileStyle0123=1;
		pCurPlate->designInfo.m_hBasePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.m_hBaseNode=pBaseNode[0]->handle;
		pCurPlate->designInfo.iFaceType=1;
		pCurPlate->designInfo.origin.datum_pos_style=3;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.norm_style=2;		//两角钢交叉线法线
		pCurPlate->designInfo.norm.hVicePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.norm.hCrossPart=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.nearVector=console.GetActiveView()->ucs.axis_z;//近似法线
		//-------共用板相对坐标系的建立------------
		
		g_pPromptMsg->SetMsg("如果有射线角钢共用此连接板,请选择,若无则空点一下鼠标");
		int iBoltLayoutStyle=0;	//双排布置螺栓时 螺栓布置形式 0.靠楞线侧优先 1.远离楞线侧优先
		JGZJ ray_jg_zj;
		if(g_pSolidSnap->SnapLine(line3d)>0)
		{
			CJdbJgJointDlg ray_jg_dlg;
			pRayJg = (CLDSLineAngle*)console.FromPartHandle(line3d.ID,CLS_LINEANGLE);
			if(pRayJg==NULL)
				throw "选择了错误的塔构件，设计失败!";
			pRayJgPara=pCurPlate->designInfo.partList.append();
			pRayJgPara->hPart=pRayJg->handle;
			ray_jg_dlg.m_sRayJgGuiGe.Format("%.0fX%.0f",pRayJg->GetWidth(),pRayJg->GetThick());
			int x_wing0_y_wing1;
			IsInsideJg(pRayJg,pCurPlate->ucs.axis_z,&x_wing0_y_wing1);
			pRayJgPara->angle.cur_wing_x0_y1=(short)x_wing0_y_wing1;
			if(pRayJg->m_bEnableTeG)
			{
				if(x_wing0_y_wing1==0)
					ray_jg_dlg.jgzj = pRayJg->xWingXZhunJu;
				else
					ray_jg_dlg.jgzj = pRayJg->xWingYZhunJu;
			}
			else
				getjgzj(ray_jg_dlg.jgzj,pRayJg->GetWidth());
			g_pSolidDraw->SetEntSnapStatus(pRayJg->handle);
			pCurPlate->ucs.origin = CalSharePlankRayZhun(pBaseJg,pBaseNode,pRayJg,pCurPlate->ucs.axis_z);
			if(pRayJg->pStart==pBaseNode[0]||pRayJg->pStart==pBaseNode[1])
			{
				pRayJg->feature = 10;	//始端连接
				GetLsSpace(rayspace,pRayJg->connectStart.d);
				pRayJgPara->start0_end1=0;
				pRayJgPara->end_space=rayspace.EndSpace;
				ray_jg_dlg.m_nLsN = 2;
				ray_jg_dlg.m_nLsSpace = rayspace.SingleRowSpace;
				ray_jg_dlg.m_sLsGuiGe.Format("%d",pRayJg->connectStart.d);
				if(fabs(pBaseJg[0]->get_norm_x_wing()*pCurPlate->ucs.axis_z)>
					fabs(pBaseJg[0]->get_norm_y_wing()*pCurPlate->ucs.axis_z)&&pRayJg->pStart==pBaseNode[0])
				{
					while(IsPartsCollide(pBaseJg[0],pRayJg))
					{
						pRayJg->SetStartOdd(pRayJg->startOdd()-5);
						if(fabs(pRayJg->startOdd())>500)
							break;
					}
				}
				else if(pRayJg->pStart==pBaseNode[0])
				{
					while(IsPartsCollide(pBaseJg[0],pRayJg))
					{
						pRayJg->SetStartOdd(pRayJg->startOdd()-5);
						if(fabs(pRayJg->startOdd())>500)
							break;
					}
				}
				if(fabs(pBaseJg[1]->get_norm_x_wing()*pCurPlate->ucs.axis_z)>
					fabs(pBaseJg[1]->get_norm_y_wing()*pCurPlate->ucs.axis_z)
					&&pRayJg->pStart==pBaseNode[1])
				{
					while(IsPartsCollide(pBaseJg[1],pRayJg))
					{
						pRayJg->SetStartOdd(pRayJg->startOdd()-5);
						if(fabs(pRayJg->startOdd())>500)
							break;
					}
				}
				else if(pRayJg->pStart==pBaseNode[1])
				{
					while(IsPartsCollide(pBaseJg[1],pRayJg))
					{
						pRayJg->SetStartOdd(pRayJg->startOdd()-5);
						if(fabs(pRayJg->startOdd())>500)
							break;
					}
				}
				ray_jg_dlg.m_fOddment = pRayJg->startOdd();
				ray_jg_dlg.m_nLsN = pRayJg->connectStart.wnConnBoltN;
				if(ray_jg_dlg.DoModal()!=IDOK)
					throw "设计撤消";
				if(ray_jg_dlg.m_fOddment!=pRayJg->startOdd())
					pRayJg->desStartOdd.m_iOddCalStyle=2;
				sscanf(ray_jg_dlg.m_sLsGuiGe,"%d",&pRayJg->connectStart.d);
				rayspace = ray_jg_dlg.LsSpace;
				if(ray_jg_dlg.m_nLsRowsN==0)
					pRayJg->connectEnd.rows=1;	//单排螺栓
				else 
					pRayJg->connectEnd.rows=2;	//双排螺栓
				//双排螺栓布置形式 
				iBoltLayoutStyle=ray_jg_dlg.m_iLsLayOutStyle;
				ray_jg_zj=ray_jg_dlg.jgzj;	//角钢准距
				pRayJg->connectStart.wnConnBoltN = ray_jg_dlg.m_nLsN;
				pRayJg->SetStartOdd(ray_jg_dlg.m_fOddment);
			}
			else if(pRayJg->pEnd==pBaseNode[0]||pRayJg->pEnd==pBaseNode[1])
			{
				pRayJg->feature = 11;	//终端连接
				GetLsSpace(rayspace,pRayJg->connectEnd.d);
				pRayJgPara->start0_end1=1;
				pRayJgPara->end_space=rayspace.EndSpace;
				ray_jg_dlg.m_nLsN = 2;
				ray_jg_dlg.m_nLsSpace = rayspace.SingleRowSpace;
				ray_jg_dlg.m_sLsGuiGe.Format("%d",pRayJg->connectEnd.d);
				if(fabs(pBaseJg[0]->get_norm_x_wing()*pCurPlate->ucs.axis_z)>
					fabs(pBaseJg[0]->get_norm_y_wing()*pCurPlate->ucs.axis_z))
				{
					if(pRayJg->pEnd==pBaseNode[0])
					{
						while(IsPartsCollide(pBaseJg[0],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
					else if(pRayJg->pEnd==pBaseNode[0])
					{
						while(IsPartsCollide(pBaseJg[0],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
				}
				else
				{
					if(pRayJg->pEnd==pBaseNode[0])
					{
						while(IsPartsCollide(pBaseJg[0],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
					else if(pRayJg->pEnd==pBaseNode[0])
					{
						while(IsPartsCollide(pBaseJg[0],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
				}
				if(fabs(pBaseJg[1]->get_norm_x_wing()*pCurPlate->ucs.axis_z)>
					fabs(pBaseJg[1]->get_norm_y_wing()*pCurPlate->ucs.axis_z))
				{
					if(pRayJg->pEnd==pBaseNode[1])
					{
						while(IsPartsCollide(pBaseJg[1],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
					else if(pRayJg->pEnd!=pBaseNode[1])
					{
						while(IsPartsCollide(pBaseJg[1],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
				}
				else
				{
					if(pRayJg->pEnd==pBaseNode[1])
					{
						while(IsPartsCollide(pBaseJg[1],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
					else if(pRayJg->pEnd!=pBaseNode[1])
					{
						while(IsPartsCollide(pBaseJg[1],pRayJg))
						{
							pRayJg->SetEndOdd(pRayJg->endOdd()-5);
							if(fabs(pRayJg->endOdd())>500)
								break;
						}
					}
				}
				ray_jg_dlg.m_fOddment = pRayJg->endOdd();
				ray_jg_dlg.m_nLsN = pRayJg->connectEnd.wnConnBoltN;
				if(ray_jg_dlg.DoModal()!=IDOK)
					throw "设计撤消";
				if(ray_jg_dlg.m_fOddment!=pRayJg->endOdd())
					pRayJg->desEndOdd.m_iOddCalStyle=2;
				sscanf(ray_jg_dlg.m_sLsGuiGe,"%d",&pRayJg->connectEnd.d);
				rayspace = ray_jg_dlg.LsSpace;
				if(ray_jg_dlg.m_nLsRowsN==0)
					pRayJg->connectEnd.rows=1;	//单排螺栓
				else 
					pRayJg->connectEnd.rows=2;	//双排螺栓
				//双排螺栓布置形式 
				iBoltLayoutStyle=ray_jg_dlg.m_iLsLayOutStyle;
				ray_jg_zj=ray_jg_dlg.jgzj;	//角钢准距
				pRayJg->connectEnd.wnConnBoltN = ray_jg_dlg.m_nLsN;
				pRayJg->SetEndOdd(ray_jg_dlg.m_fOddment);
			}
			else
				throw "所选共用射线角钢与基准节点不一致,操作无效";
		}
		static CPlateBasicParaDlg share_dlg;
		share_dlg.m_bEnableWeld=FALSE; //无焊接边
		if(share_dlg.DoModal()!=IDOK)
			throw "中途退出,设计失败!";
		else
		{
			pCurPlate->designInfo.iProfileStyle0123=share_dlg.m_iProfileType+1;
			pCurPlate->Thick=share_dlg.m_nPlankThick;
			pCurPlate->cMaterial=steelmat_tbl[share_dlg.m_iMaterial].cBriefMark;
			sprintf(pCurPlate->sPartNo,"%s",share_dlg.m_sPartNo);
			pCurPlate->iSeg=SEGI(share_dlg.m_sSegI.GetBuffer());
		}
		CLDSBolt *pBolt=NULL;
		for(i=0;i<2;i++)
		{
			f3dPoint vec,direct,norm;
			pLjJgParaArr[i]->iFaceNo=1;
			pLjJgParaArr[i]->angle.bTwoEdge=TRUE;
			
			//double offset = basespace[i].EndSpace;
			if(fabs(dot_prod(pBaseJg[i]->get_norm_x_wing(),pCurPlate->ucs.axis_z))>0.7071)
			{
				norm = pBaseJg[i]->get_norm_x_wing();
				direct= -pBaseJg[i]->GetWingVecX();
				pLjJgParaArr[i]->angle.cur_wing_x0_y1=0;	//X肢为当前连接肢
			}
			else
			{
				norm = pBaseJg[i]->get_norm_y_wing();
				direct = -pBaseJg[i]->GetWingVecY();
				pLjJgParaArr[i]->angle.cur_wing_x0_y1=1;	//Y肢为当前连接肢
			}
			JGZJ jgzj;
			int x_wing0_y_wing1;
			getjgzj(jgzj,pBaseJg[i]->GetWidth());
			IsInsideJg(pBaseJg[i],pCurPlate->ucs.axis_z,&x_wing0_y_wing1);
			if(pBaseJg[i]->m_bEnableTeG)
			{
				if(x_wing0_y_wing1==0)
					jgzj = pBaseJg[i]->xWingXZhunJu;
				else
					jgzj = pBaseJg[i]->xWingYZhunJu;
			}
			base_wing_wide[i] = pBaseJg[i]->GetWidth();
			base_wing_thick[i]= pBaseJg[i]->GetThick();
			if(pBaseJg[i]->feature==10)
			{
				pLjJgParaArr[i]->wing_space=base_wing_wide[i]-jgzj.g;
				pLjJgParaArr[i]->ber_space=jgzj.g;
				Sub_Pnt(vec,pBaseJg[i]->End(),pBaseJg[i]->Start());
				normalize(vec);
				//offset = quitdist[i]+basespace[i].EndSpace;
				for(int j=0;j<pBaseJg[i]->connectStart.wnConnBoltN;j++)
				{
					pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pBolt->iSeg=pCurPlate->iSeg;
					pBolt->set_d(pBaseJg[i]->connectStart.d);
					pBolt->cfgword=pBaseJg[i]->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pBolt->AddL0Thick(pBaseJg[i]->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//角钢楞点定位
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pBaseJg[i]->handle;
					pBolt->des_base_pos.wing_offset_dist=jgzj.g;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pBaseJg[i]->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_y_wing()))
					{	//X肢上的螺栓
						//沿X肢进行螺栓位置偏移
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y肢上的螺栓
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//沿Y肢进行螺栓位置偏移
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=1;
					}
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=0;//始-->终
					pBolt->des_base_pos.len_offset_dist=quitdist[i]+basespace[i].EndSpace+j*basespace[i].SingleRowSpace;
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
					CLsRef *pLsRef=pCurPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
					pLsRef->dwRayNo=GetSingleWord(i*2);
					pBaseJg[i]->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
				}
				//offset = offset-basespace[i].SingleRowSpace+basespace[i].EndSpace;
				/*
				if(dot_prod(norm,pCurPlate->ucs.axis_z)<0)
				{
					pBaseJg[i]->SetStart(pBaseJg[i]->Start()+
						pCurPlate->GetThick()*pCurPlate->ucs.axis_z);
				}*/
			}
			else
			{
				pLjJgParaArr[i]->wing_space=base_wing_wide[i]-jgzj.g;
				pLjJgParaArr[i]->ber_space=jgzj.g;
				Sub_Pnt(vec,pBaseJg[i]->Start(),pBaseJg[i]->End());
				normalize(vec);
				//offset = quitdist[i]+basespace[i].EndSpace;
				for(int j=0;j<pBaseJg[i]->connectEnd.wnConnBoltN;j++)
				{
					pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pBolt->iSeg=pCurPlate->iSeg;
					pBolt->set_d(pBaseJg[i]->connectEnd.d);
					pBolt->cfgword=pBaseJg[i]->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pBolt->AddL0Thick(pBaseJg[i]->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//角钢楞点定位
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pBaseJg[i]->handle;
					pBolt->des_base_pos.wing_offset_dist=jgzj.g;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pBaseJg[i]->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_y_wing()))
					{	//X肢上的螺栓
						//沿X肢进行螺栓位置偏移
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y肢上的螺栓
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//沿Y肢进行螺栓位置偏移
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=1;
					}
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=1;//终-->始
					pBolt->des_base_pos.len_offset_dist=quitdist[i]+basespace[i].EndSpace+j*basespace[i].SingleRowSpace;
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
					CLsRef *pLsRef=pCurPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
					pLsRef->dwRayNo=GetSingleWord(i*2);
					pBaseJg[i]->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
				}
				/*
				if(dot_prod(norm,pCurPlate->ucs.axis_z)<0)
				{
					pBaseJg[i]->SetEnd(pBaseJg[i]->End()+
						pCurPlate->GetThick()*pCurPlate->ucs.axis_z);
				}*/
			}
			pBaseJg[i]->ClearFlag();
			pBaseJg[i]->CalPosition();
			pBaseJg[i]->SetModified();
			pBaseJg[i]->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBaseJg[i]->GetSolidPartObject());
		}
		if(pRayJg!=NULL)
		{
			JGZJ jgzj;
			int x_wing0_y_wing1;
			pRayJgPara->angle.bTwoEdge=FALSE;
			pRayJgPara->iFaceNo=1;
			BOOL bInsideJg=IsInsideJg(pRayJg,pCurPlate->ucs.axis_z,&x_wing0_y_wing1);
			pRayJgPara->angle.cur_wing_x0_y1=x_wing0_y_wing1;
			ray_wing_wide = pRayJg->GetWidth();
			ray_wing_thick = pRayJg->GetThick();
			getjgzj(jgzj,pRayJg->GetWidth());
			
			if(pRayJg->m_bEnableTeG)
			{
				if(x_wing0_y_wing1==0)
					jgzj = pRayJg->xWingXZhunJu;
				else
					jgzj = pRayJg->xWingYZhunJu;
			}
			pRayJgPara->ber_space=jgzj.g;
			pRayJgPara->wing_space=ray_wing_wide-jgzj.g;
			if(pRayJg->feature==10)	//始端连接
			{
				pRayJgPara->start0_end1=0;
				//pRayJg->desStartPos.datum_pos_style=15;	//直接指定基点坐标
				pRayJg->desStartPos.SetToDatumPointStyle();
				//基点定位方式
				pRayJg->desStartPos.datumPoint.datum_pos_style=3; //角钢心线交点
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
				if(x_wing0_y_wing1==0)
				{
					pRayJg->desStartPos.wing_x_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desStartPos.wing_x_offset.offsetDist=0;
					else
						pRayJg->desStartPos.wing_x_offset.offsetDist=-pCurPlate->GetThick();
					pRayJg->desStartPos.wing_y_offset.gStyle=0;
				}
				else
				{
					pRayJg->desStartPos.wing_x_offset.gStyle=0;
					pRayJg->desStartPos.wing_y_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desStartPos.wing_y_offset.offsetDist=0;
					else
						pRayJg->desStartPos.wing_y_offset.offsetDist=-pCurPlate->GetThick();
				}
				Sub_Pnt(vec,pRayJg->End(),pRayJg->Start());
				
				for(i=0;i<pRayJg->connectStart.wnConnBoltN;i++)
				{
					pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pBolt->iSeg=pCurPlate->iSeg;
					pBolt->set_d(pRayJg->connectStart.d);
					pBolt->cfgword=pRayJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pBolt->AddL0Thick(pRayJg->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//角钢楞点定位
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pRayJg->handle;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pRayJg->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_y_wing()))
					{	//X肢上的螺栓
						//沿X肢进行螺栓位置偏移
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y肢上的螺栓
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//沿Y肢进行螺栓位置偏移
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=1;
					}
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=0;//始-->终
					if(pRayJg->connectStart.rows==1)	//单排排列
					{
						pBolt->des_base_pos.wing_offset_dist=jgzj.g;	
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.SingleRowSpace;
					}
					else if(pRayJg->connectStart.rows==2)	//双排排列
					{
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.doubleRowSpace;
						if(iBoltLayoutStyle==0)	//靠近楞线一侧优先
						{
							if(i%2==0)	//偶数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g1;
							else		//奇数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
						}
						else	//远离楞线一侧优先
						{
							if(i%2==0)	//偶数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
							else		//奇数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g1;
						}
					}
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pBolt->SetModified();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
					CLsRef *pLsRef=pCurPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
					pLsRef->dwRayNo=GetSingleWord(i*2);
					pRayJg->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
				}
				if(!bInsideJg)
					pRayJg->SetStart(pRayJg->Start()+pCurPlate->GetThick()*pCurPlate->ucs.axis_z);
			}
			else
			{
				pRayJgPara->start0_end1=1;
				//pRayJg->des_end_pos.datum_pos_style=15;;	//直接指定基点坐标
				pRayJg->desEndPos.SetToDatumPointStyle();
				//基点定位方式
				pRayJg->desEndPos.datumPoint.datum_pos_style=3; //角钢心线交点
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
				if(x_wing0_y_wing1==0)
				{
					pRayJg->desEndPos.wing_x_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desEndPos.wing_x_offset.offsetDist=0;
					else
						pRayJg->desEndPos.wing_x_offset.offsetDist=-pCurPlate->GetThick();
					pRayJg->desEndPos.wing_y_offset.gStyle=0;
				}
				else
				{
					pRayJg->desEndPos.wing_x_offset.gStyle=0;
					pRayJg->desEndPos.wing_y_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desEndPos.wing_y_offset.offsetDist=0;
					else
						pRayJg->desEndPos.wing_y_offset.offsetDist=-pCurPlate->GetThick();
				}
				
				Sub_Pnt(vec,pRayJg->Start(),pRayJg->End());
				for(i=0;i<pRayJg->connectEnd.wnConnBoltN;i++)
				{
					pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pBolt->iSeg=pCurPlate->iSeg;
					pBolt->set_d(pRayJg->connectEnd.d);
					pBolt->cfgword=pRayJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pBolt->AddL0Thick(pRayJg->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//角钢楞点定位
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pRayJg->handle;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pRayJg->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_y_wing()))
					{	//X肢上的螺栓
						//沿X肢进行螺栓位置偏移
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y肢上的螺栓
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//沿Y肢进行螺栓位置偏移
						pBolt->des_work_norm.direction=0;		//朝外
						pBolt->des_work_norm.norm_wing=1;
					}
					
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=1;//终-->始
					if(pRayJg->connectEnd.rows==1)	//单排排列
					{
						pBolt->des_base_pos.wing_offset_dist=jgzj.g;	
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.SingleRowSpace;
					}
					else if(pRayJg->connectEnd.rows==2)	//双排排列
					{
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.doubleRowSpace;
						if(iBoltLayoutStyle==0)	//靠近楞线一侧优先
						{
							if(i%2==0)	//偶数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g1;
							else		//奇数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
						}
						else	//远离楞线一侧优先
						{
							if(i%2==0)	//偶数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
							else		//奇数个螺栓
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g1;
						}
					}
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pBolt->SetModified();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
					CLsRef *pLsRef=pCurPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
					pLsRef->dwRayNo=GetSingleWord(i*2);
					pRayJg->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
				}
				if(!bInsideJg)
					pRayJg->SetEnd(pRayJg->End()+pCurPlate->GetThick()*pCurPlate->ucs.axis_z);
			}
			pRayJg->ClearFlag();
			pRayJg->CalPosition();
			pRayJg->SetModified();
			pRayJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
		}
		pCurPlate->designInfo.m_hBasePart = pBaseJg[0]->handle;
		CDesignLjParaDlg lj_dlg;
		lj_dlg.m_pLjPara=&pCurPlate->designInfo;
		pCurPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
		if(lj_dlg.DoModal()!=IDOK)
			throw "中途退出!";
		pCurPlate->DesignPlate();
		pCurPlate->SetModified();
		pCurPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCurPlate->GetSolidPartObject());
		g_pSolidDraw->ReleaseSnapStatus();
		OnOperOther();
		ReleaseFunc();	//解开函数运行锁定状态
		g_pPromptMsg->Destroy();
		static CMirMsgDlg mir_dlg;
		if(mir_dlg.DoModal()==IDOK)
			MirTaAtom(pCurPlate,mir_dlg.mirmsg);
		g_pSolidDraw->Draw();
		//切换到实体显示模式
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
		Ta.EndUndoListen();
	}
	catch(char *sError)
	{
		Ta.EndUndoListen();
		//发生异常设计失败,删除系统中已生成的错误节点板
		if(pCurPlate!=NULL)
			console.DeletePart(pCurPlate->handle);
		AfxMessageBox(sError);	//提示异常错误信息
		g_pSolidDraw->ReleaseSnapStatus();
		ReleaseFunc();	//解开函数运行锁定状态
		OnOperOther();
		g_pPromptMsg->Destroy();
		return;
	}
	
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	ReleaseFunc();	//解开函数运行锁定状态
}
#endif

void CLDSView::OnShareJdbDesign() 
{
	m_nPrevCommandID=ID_SHARE_JDB_DESIGN;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Common Plate";
#else 
	m_sPrevCommandName="重复共用板";
#endif
	Command("GongYongBan");
}
int CLDSView::ShareJdbDesign() 
{	
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	//-----vvvvvvv-通过对话框，输入钢板基本信息---------
	//在开始位置输入钢板基本信息，否则钢板厚度修改之后射线角钢偏移量需要二次修改 wht 16-10-21
	static CPlateBasicParaDlg share_dlg;
	share_dlg.m_bEnableWeld=FALSE; //无焊接边
	if(share_dlg.DoModal()!=IDOK)
		return FALSE;
	//-----vvvvvvv-界面操作，捕捉共用板所需的角钢和节点---------
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verify;
	//选择共用板需要的基准角钢和基准节点
	CLDSLineAngle *pBaseJg[2]={NULL};
	CLDSNode *pBaseNode[2]={NULL};
	for(int i=0;i<2;i++)
	{
		//选择共用板的基准角钢
		if(i==0)
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Please select common plate's first datum angle:");
		else
			cmdStr.Format("Please select common plate's second datum angle:");
#else 
			cmdStr.Format("请选择共用板的第一根基准角钢:");
		else
			cmdStr.Format("请选择共用板的第二根基准角钢:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		verify.ClearSnapFlag();
		verify.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
		verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
		while(pBaseJg[i]==NULL)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if((pBaseJg[i]=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE))==NULL)
			{
				pCmdLine->FillCmdLine(cmdStr,"");
				continue;
			}
			double scale=GetPickPosScaleOnRodS2E(pBaseJg[i]);
			if(scale<=0.3)
				pBaseNode[i]=pBaseJg[i]->pStart;
			else if(scale>-0.7)
				pBaseNode[i]=pBaseJg[i]->pEnd;
		}
		g_pSolidDraw->SetEntSnapStatus(pBaseJg[i]->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseJg[i]->handle));
		if(pBaseNode[i]!=NULL)
			continue;	//已自动识别出杆件当前的始末端 
		//选择基准角钢上的端节点
		if(i==0)
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Please select common plate's first datum node(it should be end point of first datum angle");
		else
			cmdStr.Format("Please select common plate's second datum node(it should be end point of second datum angle");
#else 
			cmdStr.Format("请选择共用板的第一根基准角钢的端节点:");
		else
			cmdStr.Format("请选择共用板的第二根基准角钢的端节点:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		verify.ClearSnapFlag();
		verify.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		{//此处加花括号用来控制节点凸出显示的生命周期
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
			pBaseNode[i]=console.FromNodeHandle(obj.hRelaObj);
			if(pBaseNode[i])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseNode[i]->dwPermission))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Without modify permission of the node！");
#else
					AfxMessageBox("没有此节点的修改权限！");
#endif
					return FALSE;
				}
				else if(pBaseNode[i]==pBaseJg[i]->pStart||pBaseNode[i]==pBaseJg[i]->pEnd)
					break;
			}
		}}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseNode[i]->handle));
		//判断选中的节点或者角钢是否有修改权限
		if( pBaseNode[i]==pBaseJg[i]->pStart)
		{	
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwStartPermission))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Without modify permission of angle's start！");
#else
				AfxMessageBox("没有此角钢始端的修改权限！");
#endif
				return FALSE;
			}
			pBaseJg[i]->feature = 10;	//始端连接
		}
		else if(pBaseNode[i]==pBaseJg[i]->pEnd)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwEndPermission))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Without modify permission of angle's end！");
#else 
				AfxMessageBox("没有此角钢终端的修改权限！");
#endif
				return FALSE;
			}
			pBaseJg[i]->feature = 11;	//终端连接
		}
	}
	//选择共用板的射线角钢
	CLDSLineAngle *pRayJg=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("If there is a ray angle Shared with the connection plate, please choose, otherwise click empty");
#else 
	cmdStr.Format("如果有射线角钢共用此连接板,请选择,若无直接回车:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	verify.ClearSnapFlag();
	verify.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pRayJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
		if(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
		{	//回车确认不需人依赖杆件
			pRayJg=NULL;
			break;
		}
		if(pRayJg && pRayJg->pStart && pRayJg->pEnd)
		{	//已选中依赖杆件
			if(pRayJg->pStart==pBaseNode[0]||pRayJg->pStart==pBaseNode[1]||
				pRayJg->pEnd==pBaseNode[0]||pRayJg->pEnd==pBaseNode[1])
				break;
		}
	}
	if(pRayJg)
	{
		g_pSolidDraw->SetEntSnapStatus(pRayJg->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pRayJg->handle));
	}
	else
		pCmdLine->FinishCmdLine("无射线角钢");
	//-----vvvvvvv-设计共用版，填充共用板的设计参数---------
	try{
		CUndoOperObject undo(&Ta,true);
		CLDSPlate *pCurPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pCurPlate->face_N = 1;
		pCurPlate->jdb_style = 6;	//共用板
		pCurPlate->Thick=share_dlg.m_nPlankThick;
		pCurPlate->cMaterial=CSteelMatLibrary::RecordAt(share_dlg.m_iMaterial).cBriefMark;
		pCurPlate->SetPartNo(share_dlg.m_sPartNo.GetBuffer());
		pCurPlate->iSeg=SEGI(share_dlg.m_sSegI.GetBuffer());
		pCurPlate->cfgword=pBaseJg[0]->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		//更正两基准角钢严格平行时的法线计算错误 WJH-2004/07/01
		pCurPlate->ucs.axis_z = CalFaceNorm(pBaseJg[0]->Start(),pBaseJg[0]->End(),pBaseJg[1]->Start(),pBaseJg[1]->End());
		normalize(pCurPlate->ucs.axis_z);
		if(dot_prod(console.GetActiveView()->ucs.axis_z,pCurPlate->ucs.axis_z)<0)
			pCurPlate->ucs.axis_z = -1.0*pCurPlate->ucs.axis_z;
		pCurPlate->ucs.origin = CalSharePlankRayZhun(pBaseJg,pBaseNode,NULL,pCurPlate->ucs.axis_z);
		pCurPlate->designInfo.iProfileStyle0123=1;
		pCurPlate->designInfo.m_hBasePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.m_hBaseNode=pBaseNode[0]->handle;
		pCurPlate->designInfo.m_bEnableFlexibleDesign=TRUE;	//启用柔性设计
		pCurPlate->designInfo.iFaceType=1;
		pCurPlate->designInfo.origin.datum_pos_style=3;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.norm_style=2;		//两角钢交叉线法线
		pCurPlate->designInfo.norm.hVicePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.norm.hCrossPart=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.nearVector=console.GetActiveView()->ucs.axis_z;//近似法线
		pCurPlate->designInfo.origin.UpdatePos(console.GetActiveModel());
		pCurPlate->ucs.origin=pCurPlate->designInfo.origin.Position();
		//初始化共用板的连接杆件设计信息
		int x_wing0_y_wing1=0;
		CDesignLjPartPara *pLjJgParaArr[2]={NULL};
		for(int i=0;i<2;i++)
		{
			pLjJgParaArr[i]=pCurPlate->designInfo.partList.Add(pBaseJg[i]->handle);
			pLjJgParaArr[i]->m_bAdjustLjEndPos=FALSE;	//共用板两根基准杆件对称时都不需要调整摆放位置 wht 10-02-26
			pLjJgParaArr[i]->hPart=pBaseJg[i]->handle;
			IsInsideJg(pBaseJg[i],pCurPlate->ucs.axis_z,&x_wing0_y_wing1);
			pLjJgParaArr[i]->angle.cur_wing_x0_y1=(BYTE)x_wing0_y_wing1;
			//对基准角钢布置螺栓
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pBaseJg[i]->handle);
			CLayAngleBoltDlg dlg;
			ATOM_LIST<CDesignLsPara>ls_list;
			dlg.m_iOddCalStyle=pBaseJg[i]->desEndOdd.m_iOddCalStyle;
			if(pBaseJg[i]->feature==10)
			{	//共用板角钢螺栓一般为单排,数量为2 wht 16-10-21
				pBaseJg[i]->connectStart.wnConnBoltN=2;
				pBaseJg[i]->connectStart.rows=1;
				dlg.m_fOddment = pBaseJg[i]->startOdd();
				dlg.m_iRayDirection=0;	//终－>始
			}
			else
			{	
				pBaseJg[i]->connectEnd.wnConnBoltN=2;
				pBaseJg[i]->connectEnd.rows=1;
				dlg.m_fOddment = pBaseJg[i]->endOdd();
				dlg.m_iRayDirection=1;	//终－>始
			}
			LSSPACE_STRU basespace;
			GetLsSpace(basespace,pBaseJg[i]->connectEnd.d);
			if(pBaseJg[i]->GetClassTypeId()==CLS_LINEANGLE)	
				dlg.m_pLinePart = pBaseJg[i];
			dlg.m_pNode = pBaseNode[i];
			dlg.m_pLsList=&ls_list;
			//布置板时的参数
			dlg.m_bIncPlateProfilePara = TRUE;
			dlg.m_bTwoEdgeProfile = TRUE;
			//插空布置螺栓
			dlg.m_nLsEndSpace = basespace.EndSpace+basespace.SingleRowSpace/2;//端距
			dlg.viewNorm = console.GetActiveView()->ucs.axis_z;
			if(dlg.DoModal()!=IDOK)
			{
				console.DeletePart(pCurPlate->handle);
				return FALSE;
			}
			//获取用户输入
			if(pBaseJg[i]->feature==10)
			{
				pBaseJg[i]->desStartOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
				pBaseJg[i]->SetStartOdd(dlg.m_fOddment);
				if(dlg.m_iOffsetWing==0)
					pBaseJg[i]->desStartPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
				else
					pBaseJg[i]->desStartPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
				pBaseJg[i]->SetModified();
				pBaseJg[i]->CalPosition();
				pBaseJg[i]->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pBaseJg[i]->GetSolidPartObject());
			}
			else
			{
				pBaseJg[i]->desEndOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
				pBaseJg[i]->SetEndOdd(dlg.m_fOddment);
				if(dlg.m_iOffsetWing==0)	
					pBaseJg[i]->desEndPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
				else
					pBaseJg[i]->desEndPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
				pBaseJg[i]->SetModified();
				pBaseJg[i]->CalPosition();
				pBaseJg[i]->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pBaseJg[i]->GetSolidPartObject());
			}
			//定义板外型时的信息
			pLjJgParaArr[i]->angle.cbSpaceFlag=dlg.cbSpaceFlag;
			pLjJgParaArr[i]->end_space=dlg.m_fPlateEdgeSpace;
			pLjJgParaArr[i]->wing_space=dlg.m_fWingSpace;
			pLjJgParaArr[i]->ber_space=dlg.m_fBerSpace;
			pLjJgParaArr[i]->iFaceNo = 1;
			pLjJgParaArr[i]->angle.bTwoEdge = dlg.m_bTwoEdgeProfile;
			//根据用户输入在角钢上布置螺栓
			int iInitRayNo=1;
			ATOM_LIST<RAYNO_RECORD>rayNoList;
			for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
			{
				CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				pBolt->des_base_pos=pLsPara->des_base_pos;
				pBolt->des_work_norm=pLsPara->des_work_norm;
				pBolt->set_d(pLsPara->d);
				pBolt->iSeg = pBaseJg[i]->iSeg;
				pBolt->SetGrade(pLsPara->grade);
				pBolt->cfgword=pBaseJg[i]->cfgword;	//调整螺栓配材号与基准构件配材号一致
				//更新螺栓通厚以及法向偏移量
				pBolt->AddL0Thick(pBaseJg[i]->handle,TRUE);
				pBolt->AddL0Thick(pCurPlate->handle,TRUE);
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				if(pBaseNode[i]==pBaseJg[i]->pStart)
					pBaseJg[i]->AppendStartLsRef(pBolt->GetLsRef());
				else if(pBaseNode[i]==pBaseJg[i]->pEnd)
					pBaseJg[i]->AppendEndLsRef(pBolt->GetLsRef());
				else
					pBaseJg[i]->AppendMidLsRef(pBolt->GetLsRef());
				if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//交叉点定位
				{
					CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
					if(pCrossAngle)
					{
						f3dPoint pos,bolt_vec,wing_vec;
						if(fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_x_wing())>
							fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_y_wing()))
						{
							wing_vec=pCrossAngle->GetWingVecX();
							Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,
								pCrossAngle->Start(),pCrossAngle->get_norm_x_wing());
						}
						else
						{
							wing_vec=pCrossAngle->GetWingVecY();
							Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,
								pCrossAngle->Start(),pCrossAngle->get_norm_y_wing());
						}
						bolt_vec=pos-pCrossAngle->Start();
						double dd=bolt_vec*wing_vec;
						if(dd>0&&dd<pCrossAngle->GetWidth())
						{	//交叉螺栓位于交叉角钢内
							pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
							pBolt->AddL0Thick(pCrossAngle->handle,TRUE);
						}
					}
				}
				//将螺栓添加到板上
				pCurPlate->AppendLsRef(pBolt->GetLsRef());
				double g=0.0;
				//将螺栓引入角钢
				if(pBaseJg[i]->GetClassTypeId()==CLS_LINEANGLE)
					g=pBaseJg[i]->GetLsG(pBolt);
				else 
				{
					f3dPoint ls_pos=pBolt->ucs.origin;
					coord_trans(ls_pos,pBaseJg[i]->ucs,FALSE);
					g=ls_pos.y;
				}
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
					pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
					pRayNo->hPart=pBolt->des_base_pos.hPart;
					pRayNo->yCoord=g;
					pBolt->dwRayNo=pRayNo->dwRayNo;
					iInitRayNo++;
				}
			}
			if(pBaseNode[i]==pBaseJg[i]->pStart&&pBaseJg[i]->desStartOdd.m_iOddCalStyle==1)
				pBaseJg[i]->CalStartOddment();
			else if(pBaseNode[i]==pBaseJg[i]->pEnd&&pBaseJg[i]->desEndOdd.m_iOddCalStyle==1)
				pBaseJg[i]->CalEndOddment();
			pBaseJg[i]->SetModified();
		}
		if(pRayJg)
		{	//根据射线角钢调整共用板的连接杆件设计参数
			CDesignLjPartPara *pRayJgPara=pCurPlate->designInfo.partList.Add(pRayJg->handle);
			pRayJgPara->hPart=pRayJg->handle;
			BOOL bInsideJg=IsInsideJg(pRayJg,pCurPlate->ucs.axis_z,&x_wing0_y_wing1);
			pRayJgPara->angle.cur_wing_x0_y1=(BYTE)x_wing0_y_wing1;
			pCurPlate->ucs.origin = CalSharePlankRayZhun(pBaseJg,pBaseNode,pRayJg,pCurPlate->ucs.axis_z);
			//布置射线角钢螺栓
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pRayJg->handle);
			ATOM_LIST<CDesignLsPara>ls_list;
			CLayAngleBoltDlg ray_jg_dlg;
			ray_jg_dlg.m_bIncPlateProfilePara = TRUE;
			if(pRayJg->pStart==pBaseNode[0]||pRayJg->pStart==pBaseNode[1])
			{
				pRayJg->feature = 10;	//始端连接
				ray_jg_dlg.m_iRayDirection=0;	//始－>终
				//计算角钢正负头
				if(pBaseJg[0])
					pRayJg->desStartOdd.m_hRefPart1=pBaseJg[0]->handle;
				if(pBaseJg[1])
					pRayJg->desStartOdd.m_hRefPart2=pBaseJg[1]->handle;
				pRayJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
				pRayJg->desStartOdd.m_iOddCalStyle=0;	//根据碰撞计算正负头
				pRayJg->CalStartOddment();
				ray_jg_dlg.m_fOddment=pRayJg->startOdd();
			}
			else if(pRayJg->pEnd==pBaseNode[0]||pRayJg->pEnd==pBaseNode[1])
			{
				pRayJg->feature = 11;	//终端连接
				ray_jg_dlg.m_iRayDirection=1;	//终－>始
				//计算角钢正负头
				if(pBaseJg[0])
					pRayJg->desEndOdd.m_hRefPart1=pBaseJg[0]->handle;
				if(pBaseJg[1])
					pRayJg->desEndOdd.m_hRefPart2=pBaseJg[1]->handle;
				pRayJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
				pRayJg->desEndOdd.m_iOddCalStyle=0;	//根据碰撞计算正负头
				pRayJg->CalEndOddment();
				ray_jg_dlg.m_fOddment=pRayJg->endOdd();
			}
			if(pRayJg->feature==10)	//始端连接
			{
				pRayJgPara->start0_end1=0;
				//pRayJg->des_start_pos.datum_pos_style=15;	//直接指定基点坐标
				pRayJg->desStartPos.SetToDatumPointStyle();
				//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定模式 wjh-2016.3.24
				if(pRayJg->desStartPos.jgber_cal_style==2)
					pRayJg->desStartPos.bUdfDatumPointMode=TRUE;
				//基点定位方式
				pRayJg->desStartPos.datumPoint.datum_pos_style=3; //角钢心线交点
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				pRayJg->desStartPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
				if(x_wing0_y_wing1==0)
				{
					pRayJg->desStartPos.wing_x_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desStartPos.wing_x_offset.offsetDist=0;
					else
						pRayJg->desStartPos.wing_x_offset.offsetDist=-pCurPlate->GetThick();
					pRayJg->desStartPos.wing_y_offset.gStyle=0;
				}
				else
				{
					pRayJg->desStartPos.wing_x_offset.gStyle=0;
					pRayJg->desStartPos.wing_y_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desStartPos.wing_y_offset.offsetDist=0;
					else
						pRayJg->desStartPos.wing_y_offset.offsetDist=-pCurPlate->GetThick();
				}
			}
			else
			{
				pRayJgPara->start0_end1=1;
				//pRayJg->desEndPos.datum_pos_style=15;;	//直接指定基点坐标
				pRayJg->desEndPos.SetToDatumPointStyle();
				//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定模式 wjh-2016.3.24
				if(pRayJg->desEndPos.jgber_cal_style==2)
					pRayJg->desEndPos.bUdfDatumPointMode=TRUE;
				//基点定位方式
				pRayJg->desEndPos.datumPoint.datum_pos_style=3; //角钢心线交点
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				pRayJg->desEndPos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
				if(x_wing0_y_wing1==0)
				{
					pRayJg->desEndPos.wing_x_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desEndPos.wing_x_offset.offsetDist=0;
					else
						pRayJg->desEndPos.wing_x_offset.offsetDist=-pCurPlate->GetThick();
					pRayJg->desEndPos.wing_y_offset.gStyle=0;
				}
				else
				{
					pRayJg->desEndPos.wing_x_offset.gStyle=0;
					pRayJg->desEndPos.wing_y_offset.gStyle=4;
					if(bInsideJg)
						pRayJg->desEndPos.wing_y_offset.offsetDist=0;
					else
						pRayJg->desEndPos.wing_y_offset.offsetDist=-pCurPlate->GetThick();
				}
			}
			ray_jg_dlg.m_pLinePart = pRayJg;
			ray_jg_dlg.m_pNode = pBaseNode[1];
			ray_jg_dlg.m_pLsList=&ls_list;
			ray_jg_dlg.viewNorm = console.GetActiveView()->ucs.axis_z;
			//板的连接参数
			ray_jg_dlg.m_bIncPlateProfilePara = TRUE;
			ray_jg_dlg.m_bTwoEdgeProfile = TRUE;
			if(ray_jg_dlg.DoModal()!=IDOK)
			{
				console.DeletePart(pCurPlate->handle);
				return FALSE;
			}
			//获取用户输入
			if(pRayJg->feature==10)
			{
				pRayJg->desStartOdd.m_iOddCalStyle=ray_jg_dlg.m_iOddCalStyle;
				pRayJg->SetStartOdd(ray_jg_dlg.m_fOddment);
				if(ray_jg_dlg.m_iOddCalStyle==1)
					pRayJg->CalStartOddment();
				if(ray_jg_dlg.m_iOffsetWing==0)
					pRayJg->desStartPos.wing_x_offset.offsetDist=ray_jg_dlg.m_fAngleEndNormOffset;
				else
					pRayJg->desStartPos.wing_y_offset.offsetDist=ray_jg_dlg.m_fAngleEndNormOffset;
				pRayJg->SetModified();
				pRayJg->CalPosition();
				pRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
			}
			else
			{
				pRayJg->desEndOdd.m_iOddCalStyle=ray_jg_dlg.m_iOddCalStyle;
				pRayJg->SetEndOdd(ray_jg_dlg.m_fOddment);
				if(ray_jg_dlg.m_iOddCalStyle==1)
					pRayJg->CalEndOddment();
				if(ray_jg_dlg.m_iOffsetWing==0)	
					pRayJg->desEndPos.wing_x_offset.offsetDist=ray_jg_dlg.m_fAngleEndNormOffset;
				else
					pRayJg->desEndPos.wing_y_offset.offsetDist=ray_jg_dlg.m_fAngleEndNormOffset;
				pRayJg->SetModified();
				pRayJg->CalPosition();
				pRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
			}
			//根据用户输入在角钢上布置螺栓
			int iInitRayNo=1;
			ATOM_LIST<RAYNO_RECORD>rayNoList;
			for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
			{
				CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				pBolt->des_base_pos=pLsPara->des_base_pos;
				pBolt->des_work_norm=pLsPara->des_work_norm;
				pBolt->set_d(pLsPara->d);
				pBolt->iSeg = pRayJg->iSeg;
				pBolt->SetGrade(pLsPara->grade);
				pBolt->cfgword=pRayJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
				//更新螺栓通厚以及法向偏移量
				pBolt->AddL0Thick(pRayJg->handle,TRUE);
				pBolt->AddL0Thick(pCurPlate->handle,TRUE);
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//交叉点定位
				{
					CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
					if(pCrossAngle)
					{
						f3dPoint pos,bolt_vec,wing_vec;
						if(fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_x_wing())>
							fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_y_wing()))
						{
							wing_vec=pCrossAngle->GetWingVecX();
							Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,
								pCrossAngle->Start(),pCrossAngle->get_norm_x_wing());
						}
						else
						{
							wing_vec=pCrossAngle->GetWingVecY();
							Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,
								pCrossAngle->Start(),pCrossAngle->get_norm_y_wing());
						}
						bolt_vec=pos-pCrossAngle->Start();
						double dd=bolt_vec*wing_vec;
						if(dd>0&&dd<pCrossAngle->GetWidth())
						{	//交叉螺栓位于交叉角钢内
							pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
							pBolt->AddL0Thick(pCrossAngle->handle,TRUE);
						}
					}
				}
				//将螺栓添加到板上
				pCurPlate->AppendLsRef(pBolt->GetLsRef());
				//将螺栓添加到板上
				if(pRayJg->feature == 10)
					pRayJg->AppendStartLsRef(pBolt->GetLsRef());
				else 
					pRayJg->AppendEndLsRef(pBolt->GetLsRef());
				double g=0.0;
				if(pRayJg->GetClassTypeId()==CLS_LINEANGLE)
					g=pRayJg->GetLsG(pBolt);
				else 
				{
					f3dPoint ls_pos=pBolt->ucs.origin;
					coord_trans(ls_pos,pRayJg->ucs,FALSE);
					g=ls_pos.y;
				}
				for(RAYNO_RECORD *pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
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
					pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
					pRayNo->hPart=pBolt->des_base_pos.hPart;
					pRayNo->yCoord=g;
					pBolt->dwRayNo=pRayNo->dwRayNo;
					iInitRayNo++;
				}
			}
			//板外型参数
			pRayJgPara->angle.cbSpaceFlag=ray_jg_dlg.cbSpaceFlag;
			pRayJgPara->end_space=ray_jg_dlg.m_fPlateEdgeSpace;
			pRayJgPara->wing_space=ray_jg_dlg.m_fWingSpace;
			pRayJgPara->ber_space=ray_jg_dlg.m_fBerSpace;
			pRayJgPara->iFaceNo=1;
			pRayJgPara->angle.bTwoEdge = ray_jg_dlg.m_bTwoEdgeProfile;
			
			if(pRayJg->feature==10&&pRayJg->desStartOdd.m_iOddCalStyle==1)
				pRayJg->CalStartOddment();
			else if(pRayJg->feature==11&&pRayJg->desEndOdd.m_iOddCalStyle==1)
				pRayJg->CalEndOddment();
			pRayJg->SetModified();
		}
		//
		pCurPlate->DesignPlate();
		pCurPlate->SetModified();
		pCurPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCurPlate->GetSolidPartObject());
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pCurPlate->handle);
		OnOperOther();
		ReleaseFunc();	//解开函数运行锁定状态
		static CMirMsgDlg mir_dlg;
		if(mir_dlg.DoModal()==IDOK)
			MirTaAtom(pCurPlate,mir_dlg.mirmsg);
		g_pSolidDraw->Draw();
		//切换到实体显示模式
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
	}
	catch(char *sError)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		ReleaseFunc();	//解开函数运行锁定状态
		OnOperOther();
		AfxMessageBox(sError);	//提示异常错误信息
		return FALSE;
	}
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	ReleaseFunc();	//解开函数运行锁定状态
	return TRUE;
}

//设计角钢接头
void CLDSView::OnCommonJgJoint() 
{
	m_nPrevCommandID=ID_COMMON_JG_JOINT;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Angle Joint";
#else 
	m_sPrevCommandName="重复角钢接头";
#endif
	Command("CommonJgJoint");
}
int CLDSView::CommonJgJoint()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	//选择设计节点
	g_pSolidDraw->ReleaseSnapStatus();
	CLDSNode* pSelNode=NULL;
	CString cmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CommonJgJoint Please select the node of joint:");
#else 
	cmdStr.Format("CommonJgJoint 请选择要设计接头的节点:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	DWORD dwhObj=0,dwExportFlag=0;
	/*CSnapTypeVerify verify(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	if(pSelNode==NULL)
	{
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pSelNode=Ta.Node.FromHandle(obj.hRelaObj);
			if(pSelNode)
				break;
			pCmdLine->FillCmdLine(cmdStr,"");
		}
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pSelNode->handle));
	g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);*/
	//选择两根基准角钢
	CLDSLineAngle *pFirstJg=NULL,*pLastJg=NULL;
	CSnapTypeVerify verify(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_GROUPLINEANGLE));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	for(int i=0;i<2;i++)
	{
		CLDSLineAngle* pSelAngle=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CommonJgJoint please select the %d datum angles in the nodes:",i+1);
#else
		cmdStr.Format("CommonJgJoint 请选择接头的第%d根连接基准角钢:",i+1);
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		while(1)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify))<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pSelAngle=(CLDSLineAngle*)Ta.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pSelAngle==NULL)
				continue;
			if(pSelNode==NULL)
			{
				double scaleOfS2E=this->GetPickPosScaleOnRodS2E(pSelAngle);
				if(scaleOfS2E<0.3&&pSelAngle->pStart!=NULL)
					pSelNode=pSelAngle->pStart;
				else if(scaleOfS2E>0.7&&pSelAngle->pEnd!=NULL)
					pSelNode=pSelAngle->pEnd;
			}
			if(pSelAngle!=NULL)
				break;
			//pCmdLine->FillCmdLine(cmdStr,"");
		}
		g_pSolidDraw->SetEntSnapStatus(pSelAngle->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pSelAngle->handle));
		if(pFirstJg==NULL)
			pFirstJg=pSelAngle;
		else if(pLastJg==NULL)
			pLastJg=pSelAngle;
	}
	if(pSelNode==NULL)
	{
		verify.ClearSnapFlag();
		verify.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verify)<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pSelNode=Ta.Node.FromHandle(obj.hRelaObj);
			if(pSelNode)
				break;
			pCmdLine->FillCmdLine(cmdStr,"");
		}
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pSelNode->handle));
	g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);
	//设计角钢接头设计
	return FinishDesignJoint(pSelNode,pFirstJg,pLastJg);
}
//布置脚钉
void CLDSView::OnLayoutFootnail() 
{
	m_nPrevCommandID=ID_LAYOUT_FOOTNAIL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to layout climbing bolt";
#else 
	m_sPrevCommandName="重复布置脚钉";
#endif
	Command("LayoutFootnail");
}
#include "FootNailSpaceDlg.h"
int CLDSView::LayoutFootnail()
{
	CString cmdStr;
	int nail_type=0;//0.槽钢脚钉板 1.角钢
	bool bDownToUp;	//脚钉自下向上布置
	PARTSET partset;
	THANDLE start_handle,cur_handle=0,last_end_handle=0;
	f3dPoint start,end,vec,nail_pos,wing_x_vec,wing_y_vec,ls_pos;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSLinePart *pSelLinePart, *pFirstLinePart, *pLastLinePart=NULL;
	//-----VVVVVVV-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	if (CLsLibrary::GetFootNailFamily()==0)
	{	//如系统未设脚钉规格库，则提示用户设定以防后续代码将脚钉布置为普通螺栓 wjh-2019.9.21
		CSelNailSeriesDlg setnails;
		setnails.DoModal();
	}
	if (CLsLibrary::GetFootNailFamily()==0)
	{
		AfxMessageBox("螺栓库中未指定脚钉规格系列的脚钉属性!");
		return false;
	}
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	static char cToward='U';
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Layout climbing bolt[from up to down(D)/from down to up(U)]<%C>:",cToward);
#else 
	cmdStr.Format("布置脚钉[自上而下(D)/自下而上(U)]<%C>:",cToward);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		//设置命令行响应鼠标左键抬起的消息，同按回车键效果相同
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::LBUTTONUP_AS_RETURN))
			return 0;	//中途退出
		m_bStartOpenWndSelTest=FALSE;	//否则可能会导致命令结束后点击鼠标时一下次变为不合理的开窗状态　wjh-2015.9.8
		if(cmdStr.GetLength()>0&&toupper(cmdStr[0])=='U')
			cToward='U';
		else if(cmdStr.GetLength()>0&&toupper(cmdStr[0])=='D')
			cToward='D';
		if(cToward=='U')
			bDownToUp = TRUE;
		else if(cToward=='D')
			bDownToUp = FALSE;
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Need selected key-word","");
#else 
			pCmdLine->FillCmdLine("需要选项关键字","");
#endif
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		break;
	}
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select leg member which will layout climbing bolts<Click empty to over>:","");
#else 
	pCmdLine->FillCmdLine("请依次选择要布置脚钉的主材<空点鼠标左键结束选择>:","");
#endif
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINETUBE));
	int i=0;
	CUndoOperObject undo(&Ta,true);
	try
	{
		while(1)
		{	
			long *id_arr=NULL;
			int nRetCode=g_pSolidSnap->GetLastSelectEnts(id_arr);
			//设置命令行响应鼠标左键抬起的消息，同按回车键效果相同
			if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::LBUTTONUP_AS_RETURN))
				return 0;	//中途退出
			m_bStartOpenWndSelTest=FALSE;	//否则可能会导致命令结束后点击鼠标时一下次变为不合理的开窗状态　wjh-2015.9.8
			if(g_pSolidSnap->GetLastSelectEnts(id_arr)>i)
			{
				CLDSPart *pPart = console.FromPartHandle(id_arr[i]);
				pSelLinePart=NULL;
				if(pPart&&pPart->IsLinePart())
					pSelLinePart=(CLDSLinePart*)pPart;
				if(pSelLinePart==NULL)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Don't select effective rod!Please select again!");
#else 
					AfxMessageBox("没有选择有效的杆件!请重新选择!");
#endif
					g_pSolidDraw->SetEntSnapStatus(id_arr[i],FALSE);
					continue;
				}
				if(pLastLinePart!=NULL&&pLastLinePart==pSelLinePart)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Repeat to select the same leg member,Please select again!");
#else 
					AfxMessageBox("重复选择了同一根主材,请重新选择!");
#endif
					g_pSolidDraw->SetEntSnapStatus(pSelLinePart->handle,FALSE);
					continue;
				}
				if(pSelLinePart->pStart&&pSelLinePart->pEnd)
				{
					if(pSelLinePart->pStart->handle==last_end_handle)
						cur_handle = pSelLinePart->pEnd->handle;
					else if(pSelLinePart->pEnd->handle==last_end_handle)
						cur_handle = pSelLinePart->pStart->handle;
					else if(last_end_handle!=0)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The selected rod has no connection with previous rod，Please select again!");
#else 
						AfxMessageBox("所选杆件与上一杆件没有连接，请重新选择!");
#endif
						g_pSolidDraw->SetEntSnapStatus(pSelLinePart->handle,FALSE);
						continue;
					}
					else if(bDownToUp)	//脚钉自下向上布置
					{
						if(pSelLinePart->pStart->Position(false).z>pSelLinePart->pEnd->Position(false).z)
						{		//初选的第一根杆件
							start_handle = pSelLinePart->pStart->handle;
							cur_handle = pSelLinePart->pEnd->handle;
						}
						else	//起始点高,终止点低
						{
							start_handle = pSelLinePart->pEnd->handle;
							cur_handle = pSelLinePart->pStart->handle;
						}
					}
					else //if(!bDownToUp) //脚钉自上向下布置
					{
						if(pSelLinePart->pStart->Position(false).z>pSelLinePart->pEnd->Position(false).z)
						{		//初选的第一根杆件
							start_handle = pSelLinePart->pEnd->handle;
							cur_handle = pSelLinePart->pStart->handle;
						}
						else	//起始点高,终止点低
						{
							start_handle = pSelLinePart->pStart->handle;
							cur_handle = pSelLinePart->pEnd->handle;
						}
					}
					last_end_handle = cur_handle;
					partset.append(pSelLinePart);
					pLastLinePart = pSelLinePart;
				}
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(AfxMessageBox("Are you sure you want to layout bolts on the selected short angle?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#else 
					if(AfxMessageBox("你确定要在选中的短角钢上布置螺栓吗?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#endif
					{
						partset.append(pSelLinePart);
						break;	//在短角钢上布置螺栓时只能单独在某一根短角钢上布置螺栓 
					}
					else
						continue;
				}
				g_pSolidDraw->SetEntSnapStatus(pSelLinePart->handle,TRUE);
				i++;
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(AfxMessageBox("Are you sure you have select all rods?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#else 
				if(AfxMessageBox("你确定已经选完了要进行设计的所有杆件吗?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#endif
					break;
				else
					continue;
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();

		CFootnailSpaceDlg naildlg;
		CLDSBolt ls(console.GetActiveModel()),*pLs;
		BOOL X0_OR_Y1_WING=FALSE;//FALSE:X肢 TRUE:Y肢
		double dist=0,used_dist=0,odd_dist=0,prev_odd=0,overlap_length=0;
		double extra_dist=0;	//上一根杆件遗留的空白长度
		//判断杆件类型
		pFirstLinePart = (CLDSLinePart*)partset.GetFirst();
		if(!pFirstLinePart)
			return 0;	//中途退出
		if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			JGZJ jgzj;
			double fPreRodRemainingLen=0;	//前一杆件的剩余长度,跨杆件布置第一个螺栓时脚钉间距需要减去前一杆件剩余长度 wht 15-08-13
			double wing_wide,wing_thick;naildlg.m_iLineType = 0;
			for(CLDSLineAngle *pCurJg=(CLDSLineAngle*)partset.GetFirst();pCurJg!=NULL;pCurJg=(CLDSLineAngle*)partset.GetNext())
			{
				UCS_STRU ucs;
				pCurJg->getUCS(ucs);
				g_pSolidDraw->SetEntSnapStatus(pCurJg->handle,TRUE);
				//
				CXhChar16 sBoltPadPartNo;
				CLDSBolt* pStartBolt=NULL;
				if(pCurJg==(CLDSLineAngle*)pFirstLinePart)
				{
					for(CLsRef *pRef=pCurJg->GetFirstLsRef();pRef!=NULL;pRef=pCurJg->GetNextLsRef())
					{
						if(pRef->GetLsPtr()->m_dwFlag.IsHasFlag(CLDSBolt::FOOT_NAIL))
						{
							if(pStartBolt==NULL)
								pStartBolt=pRef->GetLsPtr();
							if(bDownToUp)
							{
								if((*pRef)->ucs.origin.z<pStartBolt->ucs.origin.z)
									pStartBolt=pRef->GetLsPtr();
							}
							else if((*pRef)->ucs.origin.z>pStartBolt->ucs.origin.z)
								pStartBolt=pRef->GetLsPtr();
						}
					}
					//如果有脚钉的话，设置起始脚钉
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Please select start climbing nails and press return key, none nails type 'N' key:","");
#else 
					pCmdLine->FillCmdLine("请选择起始脚钉回车,无起始脚钉[N]:","");
#endif
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
					g_pSolidDraw->ReleaseSnapStatus();
					if(pStartBolt)
					{
						g_pSolidDraw->SetEntSnapStatus(pStartBolt->handle);
						//将脚钉位置放大显示
						SCOPE_STRU scope;
						scope.VerifyVertex(pStartBolt->ucs.origin);
						scope.fMinX-=150;
						scope.fMaxX+=150;
						scope.fMinY-=150;
						scope.fMaxY+=150;
						scope.fMinZ-=150;
						scope.fMaxZ+=150;
						g_pSolidOper->FocusTo(scope,0.2);
					}
					if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"N"))
					{	
						pCmdLine->CancelCmdLine();	
						return 0;
					}
					long *id_arr=NULL;
					int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
					if(cmdStr.CompareNoCase("N")==0||n!=1)
						pStartBolt=NULL;
					else if(n==1)
					{
						CLDSBolt *pSelectedBolt=(CLDSBolt*)console.FromPartHandle(id_arr[0],CLS_BOLT);
						if(pSelectedBolt)
							pStartBolt=pSelectedBolt;
					}
				}
				else
					extra_dist=0;
				//
				long old_start_handle=start_handle;
				if(pStartBolt&&pCurJg==pFirstLinePart)
				{
					vec=pCurJg->End()-pCurJg->Start();
					normalize(vec);
					//计算拾取的起始脚钉在角钢楞线上的投影
					int wingx0_y1=0;
					pCurJg->GetBoltIntersPos(pStartBolt,start,&wingx0_y1);
					X0_OR_Y1_WING=!wingx0_y1;
					SnapPerp(&start,pCurJg->Start(),pCurJg->End(),start);
					//start=pCurJg->Start()+((start-pCurJg->Start())*vec)*vec;
					if(bDownToUp&&vec.z<=0)
					{
						start_handle = pCurJg->pEnd->handle;
						end=pCurJg->End()+vec*pCurJg->endOdd();
						extra_dist=(pCurJg->Start()-vec*pCurJg->startOdd()-start)*vec;
						if(fabs(prev_odd)>EPS)
							overlap_length=prev_odd+pCurJg->startOdd();
						else
							overlap_length=0.0;
						prev_odd=pCurJg->endOdd();
					}
					else if(bDownToUp&&vec.z>0)
					{
						vec*=-1.0;	//调转至终->始
						start_handle = pCurJg->pStart->handle;
						end=pCurJg->Start()+vec*pCurJg->startOdd();
						extra_dist=(pCurJg->End()-vec*pCurJg->endOdd()-start)*vec;
						if(fabs(prev_odd)>EPS)
							overlap_length=prev_odd+pCurJg->endOdd();
						else
							overlap_length=0.0;
						prev_odd=pCurJg->startOdd();
					}
					else if(!bDownToUp&&vec.z>=0)
					{
						start_handle = pCurJg->pEnd->handle;
						end=pCurJg->End()+vec*pCurJg->endOdd();
						extra_dist=(pCurJg->Start()-vec*pCurJg->startOdd()-start)*vec;
						if(fabs(prev_odd)>EPS)
							overlap_length=prev_odd+pCurJg->startOdd();
						else
							overlap_length=0.0;
						prev_odd=pCurJg->endOdd();
					}
					else if(!bDownToUp&&vec.z<0)
					{
						vec*=-1.0;	//调转至终->始
						start_handle = pCurJg->pStart->handle;
						end=pCurJg->Start()+vec*pCurJg->startOdd();
						extra_dist=(pCurJg->End()-vec*pCurJg->endOdd()-start)*vec;
						if(fabs(prev_odd)>EPS)
							overlap_length=prev_odd+pCurJg->endOdd();
						else
							overlap_length=0.0;
						prev_odd=pCurJg->startOdd();
					}
					else
						logerr.Log("error");
				}
				else if((pCurJg->pEnd&&pCurJg->pEnd->handle==start_handle)||pCurJg->pEnd==NULL)
				{	//在短角钢上布置脚钉时由终端==>始端布置	wht 09-12-03
					start = pCurJg->End();
					end   = pCurJg->Start();
					vec=end-start;
					normalize(vec);
					if(pCurJg->pStart)
						start_handle = pCurJg->pStart->handle;
					Sub_Pnt(start,start,vec*pCurJg->endOdd());
					Add_Pnt(end,end,vec*pCurJg->startOdd());
					if(fabs(prev_odd)>EPS)
						overlap_length=prev_odd+pCurJg->endOdd();
					else
						overlap_length=0.0;
					prev_odd=pCurJg->startOdd();
				}
				else if((pCurJg->pStart&&pCurJg->pStart->handle==start_handle)||pCurJg->pStart==NULL)
				{
					start = pCurJg->Start();
					end   = pCurJg->End();
					vec=end-start;
					normalize(vec);
					if(pCurJg->pEnd)
						start_handle = pCurJg->pEnd->handle;
					Sub_Pnt(start,start,vec*pCurJg->startOdd());
					Add_Pnt(end,end,vec*pCurJg->endOdd());
					if(fabs(prev_odd)>EPS)
						overlap_length=prev_odd+pCurJg->startOdd();
					else
						overlap_length=0.0;
					prev_odd=pCurJg->endOdd();
				}
				dist = pCurJg->GetLength();
				wing_x_vec = pCurJg->GetWingVecX();
				wing_y_vec = pCurJg->GetWingVecY();
				wing_wide = pCurJg->GetWidth();
				wing_thick = pCurJg->GetThick();
				getjgzj(jgzj,pCurJg->GetWidth());
				if(!pCurJg->m_bEnableTeG)
					pCurJg->xWingXZhunJu = pCurJg->xWingYZhunJu = jgzj;
				ls.m_hFamily= CLsLibrary::GetFootNailFamily();//脚钉
				used_dist = 0;
				//预判当前杆件剩余长度是否足够布置下一个脚钉 wht 15-08-13
				if(g_sysPara.FootNailDist>dist+extra_dist)
				{
					fPreRodRemainingLen=dist+extra_dist;
					continue;
				}
				while(1)
				{
					naildlg.InitState(pCurJg->handle,dist+extra_dist-used_dist,dist,used_dist);
					naildlg.m_iLayWing = X0_OR_Y1_WING;
					naildlg.m_pRod=pCurJg;
					naildlg.m_xDatumStart=start;
					naildlg.m_xDatumLenVec=vec;
					if(fPreRodRemainingLen>0)
					{
						naildlg.m_fPreRodRemainingLen=fPreRodRemainingLen;
						fPreRodRemainingLen=0;
					}
					else
						naildlg.m_fPreRodRemainingLen=0;
					//if(odd_dist==0)
					{
						naildlg.wing_wide=ftol(pCurJg->GetWidth());
						naildlg.xWingXZhunJu=pCurJg->xWingXZhunJu;
						naildlg.xWingYZhunJu=pCurJg->xWingYZhunJu;
						naildlg.m_bEnableTeG=pCurJg->m_bEnableTeG;
						if(pCurJg->m_bFootNail)	//角钢脚钉
						{
							naildlg.m_nNailSpace=30;
							naildlg.m_iLayWing=1;	//Y肢
						}
						if(naildlg.DoModal()!=IDOK)
							goto end;
						used_dist+=(naildlg.m_nNailSpace-naildlg.m_fPreRodRemainingLen);
						X0_OR_Y1_WING = naildlg.m_iLayWing;
					}
					//else	//上次有剩余
					//	used_dist = odd_dist;
					used_dist+=overlap_length;
					overlap_length=0.0;	//考虑重叠部分尺寸一次后清掉重叠尺寸
					//布置角钢上的脚钉时odd_dist变量弃用
					//弹出对话框点击确定后再判断当前角钢是否足够布置下一颗脚钉，会导致本次布置脚钉失效
					//应在开始布置当前角钢第一颗脚钉和布置完每一颗脚钉后预判当前角钢剩余长度是否足够布置下一颗脚钉 wht 15-08-13
					/*if(used_dist>dist+extra_dist)
					{
						odd_dist = used_dist-dist-extra_dist;
						break;
					}*/
					//if(fabs(odd_dist)<EPS)		//上次设计没有剩余值
						Add_Pnt(ls_pos,start,vec*ftol(used_dist));
					//else				//上次设计时有剩余值
					//{
					//	Add_Pnt(ls_pos,start,vec*ftol(odd_dist));
					//	odd_dist = 0.0;
					//}
					restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
					if(!X0_OR_Y1_WING)	//X肢上的脚钉
					{
						Add_Pnt(ls_pos,ls_pos,wing_x_vec*naildlg.m_nGDist);
						ls.set_norm(pCurJg->get_norm_x_wing());
						
						if(naildlg.m_bShiftWing)
							X0_OR_Y1_WING = !X0_OR_Y1_WING;	//XY肢互相间隔
					}
					else			//Y肢上的脚钉
					{
						Add_Pnt(ls_pos,ls_pos,wing_y_vec*naildlg.m_nGDist);
						ls.set_norm(pCurJg->get_norm_y_wing());
						
						if(naildlg.m_bShiftWing)
							X0_OR_Y1_WING = !X0_OR_Y1_WING;	//XY肢互相间隔
					}
					//螺栓设计信息填充
					ls_pos=ls_pos-ls.get_norm()*pCurJg->GetThick();
					//
					sprintf(ls.des_base_pos.norm_offset.key_str,"-0X%X",pCurJg->handle);
					ls.EmptyL0DesignPara();	//清空螺栓通厚参数
					ls.AddL0Thick(pCurJg->handle,TRUE,TRUE);
					//ls.L0 = pCurJg->GetThick();
					//ls.des_base_pos.bDatumPointInUcs=TRUE;
					ls.des_base_pos.datumPoint.datum_pos_style=1;	//角钢上端楞点为基准
					ls.des_base_pos.datumPoint.des_para.RODEND.hRod=pCurJg->handle;
					ls.des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					ls.des_base_pos.hPart = pCurJg->handle;
					f3dPoint perp;
					SnapPerp(&perp,start,end,ls_pos);
					//TODO：这里可能还存在问题,在最简单的接头间隙处一端10,一端-50,当overlap_length=-10,此值为10 wjh-2016.11.9
					double offset_dist = DISTANCE(perp,start)-extra_dist;
					//if(pCurJg==(CLDSLineAngle*)pFirstLinePart&&!nail_start.IsZero())
					//	offset_dist = DISTANCE(perp,nail_start);//当第一根角钢有脚钉的时候特殊处理
					//if(nflag==0&&!nail_start.IsZero())
					//{
					//	dist -=offset_dist;
					//	nflag =1;
					//}
					if(pCurJg->pStart&&pCurJg->pStart->handle==old_start_handle)
						ls.des_base_pos.direction=0;
					else
						ls.des_base_pos.direction=1;
					ls.des_base_pos.len_offset_dist=ftoi(offset_dist);
					ls.des_base_pos.wing_offset_dist=naildlg.m_nGDist;
					ls.des_base_pos.datumPoint.des_para.RODEND.direction=ls.des_base_pos.direction;
					ls.des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					ls.des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					if(naildlg.m_bShiftWing)
					{
						ls.des_base_pos.datumPoint.des_para.RODEND.offset_wing=!X0_OR_Y1_WING;
						ls.des_base_pos.offset_wing = !X0_OR_Y1_WING;
						ls.des_work_norm.norm_wing=!X0_OR_Y1_WING;
					}
					else
					{
						ls.des_base_pos.datumPoint.des_para.RODEND.offset_wing=X0_OR_Y1_WING;
						ls.des_base_pos.offset_wing = X0_OR_Y1_WING;
						ls.des_work_norm.norm_wing=X0_OR_Y1_WING;
					}
					if(ls.des_work_norm.norm_wing==0)
					{
						if(ls.get_norm()*pCurJg->get_norm_x_wing()>0)
							ls.des_work_norm.direction=0;
						else
							ls.des_work_norm.direction=1;
					}
					else
					{
						if(ls.get_norm()*pCurJg->get_norm_y_wing()>0)
							ls.des_work_norm.direction=0;
						else
							ls.des_work_norm.direction=1;
					}
					ls.des_work_norm.norm_style=1;
					ls.des_work_norm.hVicePart = pCurJg->handle;
					ls.SetBelongModel(console.GetActiveModel());
					ls.correct_worknorm();
					ls.correct_pos();
					ls_pos=ls.ucs.origin;
					//是否与已设计过的螺栓距离较近
					CLDSBolt *pNearBolt=naildlg.m_pMergeBolt;//NULL;
					//for(CLsRef* pLsRef=pCurJg->GetFirstLsRef();pLsRef!=NULL;pLsRef=pCurJg->GetNextLsRef())
					//{//EPS_COS2误差系数还是太大所以在原先基础上再减去0.03
					//	if(fabs(ls.get_norm()*(*pLsRef)->get_norm())<EPS_COS2-0.03)
					//		continue;
					//	f3dPoint basePt = (*pLsRef)->ucs.origin;
					//	project_point(basePt,ls_pos,ls.get_norm());
					//	if(DISTANCE(ls_pos,basePt)<g_sysPara.fNailRepScope)
					//	{
					//		pNearBolt = pLsRef->GetLsPtr();
					//		break;
					//	}
					//}
					if(pNearBolt==NULL)
					{
						pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls,false);
						pLs->layer(0)=pCurJg->Layer(0);
						pLs->cfgword=pCurJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pLs->iSeg=pCurJg->iSeg;
						pLs->des_base_pos=ls.des_base_pos;
						pLs->des_work_norm=ls.des_work_norm;
						pLs->ucs.origin=ls_pos;
						//将螺栓引入到角钢 wht 11-01-14
						pCurJg->AppendMidLsRef(pLs->GetLsRef());
						CLDSGroupLineAngle *pGroupJg=NULL;
						pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(pCurJg->group_father_jg_h,CLS_GROUPLINEANGLE);
						if(pGroupJg)
						{	//当前角钢为组合角钢子角钢							
							for(int i=0;i<4;i++)
							{
								if(pGroupJg->son_jg_h[i]==pCurJg->handle)
									continue;
								CLDSLineAngle *pSonAngle=NULL;
								if(pGroupJg->son_jg_h[i]>0x20)
									pSonAngle=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
								if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
									continue;	//不存在对应的子角钢或子角钢为虚拟构件 wht 11-07-25
								if(pSonAngle->IsLsInWing(pLs))//螺栓在子角钢上可以引入
								{
									pSonAngle->AppendMidLsRef(pLs->GetLsRef());
									pLs->AddL0Thick(pSonAngle->handle,TRUE);	//子角钢
									//脚钉穿过两根子角钢时，需要对脚钉设计垫板，保证其受力 wxc-2017.7.14
									BOLTSET boltset;
									boltset.append(pLs);
									int thick=ftoi(pGroupJg->jg_space-g_sysPara.m_nGroupJgMachiningGap);
									CLDSPlate* pBoltPad=DesignBoltPad(pCurJg,NULL,boltset,thick,g_sysPara.m_nGroupJgMachiningGap*0.5);
									if(pBoltPad)
									{
										if(sBoltPadPartNo.GetLength()<=0)
										{
											CInputAnStringValDlg inputStrDlg;
											inputStrDlg.m_sItemTitle="输入脚钉垫板编号:";
											if(inputStrDlg.DoModal()==IDOK)
												sBoltPadPartNo.Copy(inputStrDlg.m_sItemValue);
										}
										pBoltPad->SetPartNo(sBoltPadPartNo);
										pLs->AddL0Thick(ftoi(g_sysPara.m_nGroupJgMachiningGap));//组合角钢加工间隙
									}
									else
										pLs->AddL0Thick(ftoi(pGroupJg->jg_space));	//角钢间隙
									break;
								}
							}
						}
						pLs->CalGuigeAuto();
						pLs->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
						g_pSolidDraw->Draw();
					}
					else
					{
						//保存原螺栓的有效长度
						pNearBolt->set_oldL(ftoi(pNearBolt->L));
						if(pNearBolt->get_d()==16)
							pNearBolt->set_L(180);
						else if(pNearBolt->get_d()==20)
							pNearBolt->set_L(200);
						else if(pNearBolt->get_d()==24)
							pNearBolt->set_L(240);
						else
							pNearBolt->set_L(ls.get_L());
						pNearBolt->m_hFamily = CLsLibrary::GetFootNailFamily();
						pNearBolt->CalGuigeAuto();
						pNearBolt->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pNearBolt->GetSolidPartObject());
						g_pSolidDraw->Draw();
					}
					//预判当前杆件剩余长度是否足够布置下一个脚钉 wht 15-08-13
					double pre_used_dist=used_dist+g_sysPara.FootNailDist;
					if(pre_used_dist>dist+extra_dist)
					{
						fPreRodRemainingLen=(dist+extra_dist)-used_dist;
						break;
					}
					Ta.EndUndoListen();
				}
				g_pSolidDraw->SetEntSnapStatus(pCurJg->handle,FALSE);
			}
		}
		else if(pFirstLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			BOOL bFirst=TRUE;
			static CDesignFootNailPlateDlg angle_dlg;
			CLDSLineAngle *pLineAngle=NULL;
			CLDSLineSlot *pLineSlot = NULL,*pLineSlot2 = NULL;
			long hFirstLineSlot=NULL; //标准脚钉板
			naildlg.m_iLineType = 1;	//杆件类型为钢管
			//查找指定钢管上的脚钉
			CLDSPart* pFootNail=NULL;
			ATOM_LIST<CLDSLinePart*> nailRodList;
			for(CLDSLinePart* pRod=console.EnumRodFirst();pRod;pRod=console.EnumRodNext())
			{
				if(pRod->GetClassTypeId()==CLS_LINESLOT)
				{
					CLDSLineSlot* pSlot=(CLDSLineSlot*)pRod;
					if(pSlot->pStart!=NULL || pSlot->pEnd!=NULL)
						continue;
					if(pSlot->GetLsRefList()->GetNodeNum()!=1)
						continue;
					if(pSlot->m_hPartWeldParent!=pFirstLinePart->handle &&
						pSlot->hAttachTube!=pFirstLinePart->handle)
						continue;
					nailRodList.append(pSlot);
				}
				else if(pRod->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
					if(!pAngle->m_bFootNail)
						continue;
					if(pAngle->m_hPartWeldParent!=pFirstLinePart->handle)
						continue;
					nailRodList.append(pRod);
				}
				else
					continue;
			}
			//布置钢管脚钉
			for(CLDSLineTube *pCurTube=(CLDSLineTube*)partset.GetFirst();pCurTube!=NULL;pCurTube=(CLDSLineTube*)partset.GetNext())
			{
				pCurTube->BuildUCS();
				g_pSolidDraw->SetEntSnapStatus(pCurTube->handle,TRUE);
				CLDSPart* pStartNail=NULL;
				if(pCurTube==(CLDSLineTube*)pFirstLinePart)
				{
					for(CLDSLinePart** ppRod=nailRodList.GetFirst();ppRod;ppRod=nailRodList.GetNext())
					{
						if(pStartNail==NULL)
							pStartNail=(*ppRod);
						if(bDownToUp)
						{
							if((*ppRod)->ucs.origin.z<pStartNail->ucs.origin.z)
								pStartNail=(*ppRod);
						}
						else if((*ppRod)->ucs.origin.z>pStartNail->ucs.origin.z)
							pStartNail=(*ppRod);
					}
					//如果有脚钉的话，设置起始脚钉
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Please select start climbing nails and press return key, none nails type 'N' key:","");
#else 
					pCmdLine->FillCmdLine("请选择起始脚钉回车,无起始脚钉[N]:","");
#endif
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
					g_pSolidDraw->ReleaseSnapStatus();
					if(pStartNail)
					{
						g_pSolidDraw->SetEntSnapStatus(pStartNail->handle);
						//将脚钉位置放大显示
						SCOPE_STRU scope;
						scope.VerifyVertex(pStartNail->ucs.origin);
						scope.fMinX-=150;
						scope.fMaxX+=150;
						scope.fMinY-=150;
						scope.fMaxY+=150;
						scope.fMinZ-=150;
						scope.fMaxZ+=150;
						g_pSolidOper->FocusTo(scope,0.2);
					}
					if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"N"))
					{	
						pCmdLine->CancelCmdLine();	
						return 0;
					}
					long *id_arr=NULL;
					int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
					if(cmdStr.CompareNoCase("N")==0||n!=1)
						pStartNail=NULL;
					else if(n==1)
					{
						CLDSPart *pSelectedPart=(CLDSBolt*)console.FromPartHandle(id_arr[0]);
						if(pSelectedPart)
							pStartNail=pSelectedPart;
					}
				}
				else
					extra_dist=0;
				long old_start_handle=start_handle;
				pCurTube->BuildUCS();
				f3dPoint axis_z=pCurTube->End()-pCurTube->Start();
				if(pStartNail&&pCurTube==pFirstLinePart)
				{
					f3dPoint nail_norm;
					nail_norm=pStartNail->ucs.axis_z;
					start=pStartNail->ucs.origin;
					if(pStartNail->GetClassTypeId()==CLS_LINESLOT)
					{
						CLDSLineSlot* pSlot=(CLDSLineSlot*)pStartNail;
						pSlot->BuildUCS();
						nail_norm=pSlot->WorkPlaneNorm();
						hFirstLineSlot=pSlot->handle;
						//此时pStartNail不一定是脚钉，槽钢原点已不是中心点 wjh-2015.9.1
						start=pSlot->desDatumPoint.Position();
					}
					else if(pStartNail->GetClassTypeId()==CLS_LINEANGLE)
					{
						CLDSLineAngle* pAngle=(CLDSLineAngle*)pStartNail;
						hFirstLineSlot=pAngle->handle;
						start=pAngle->Start();
					}
					//TODO:应进行定量计算旋转定位角度
					if(fabs(nail_norm*pCurTube->ucs.axis_x)>EPS_COS2)
						X0_OR_Y1_WING=TRUE;
					else
						X0_OR_Y1_WING=FALSE;
					vec=pCurTube->End()-pCurTube->Start();
					normalize(vec);
					SnapPerp(&start,pCurTube->Start(),pCurTube->End(),start);
					if((bDownToUp&&vec.z<=0) || (!bDownToUp&&vec.z>=0))
					{
						start_handle = pCurTube->pEnd->handle;
						end=pCurTube->End()+vec*pCurTube->endOdd();
						extra_dist=(pCurTube->Start()-vec*pCurTube->startOdd()-start)*vec;
						if(fabs(prev_odd)>EPS)
							overlap_length=prev_odd+pCurTube->startOdd();
						else
							overlap_length=0.0;
						prev_odd=pCurTube->endOdd();
					}
					else //if((bDownToUp&&vec.z>0) || (!bDownToUp&&vec.z<0))
					{
						vec*=-1.0;	//调转至终->始
						start_handle = pCurTube->pStart->handle;
						end=pCurTube->Start()+vec*pCurTube->startOdd();
						extra_dist=(pCurTube->End()-vec*pCurTube->endOdd()-start)*vec;
						if(fabs(prev_odd)>EPS)
							overlap_length=prev_odd+pCurTube->endOdd();
						else
							overlap_length=0.0;
						prev_odd=pCurTube->startOdd();
					}
				}
				else if(pCurTube->pEnd->handle==start_handle)
				{
					start = pCurTube->End();
					end   = pCurTube->Start();
					start_handle = pCurTube->pStart->handle;
					Sub_Pnt(vec,end,start);
					normalize(vec);
					Sub_Pnt(start,start,vec*pCurTube->endOdd());
					Add_Pnt(end,end,vec*pCurTube->startOdd());
					if(fabs(prev_odd)>EPS)
						overlap_length=prev_odd+pCurTube->endOdd();
					else
						overlap_length=0.0;
					prev_odd=pCurTube->startOdd();
				}
				else
				{
					start = pCurTube->Start();
					end   = pCurTube->End();
					start_handle = pCurTube->pEnd->handle;
					Sub_Pnt(vec,end,start);
					normalize(vec);
					Sub_Pnt(start,start,vec*pCurTube->startOdd());
					Add_Pnt(end,end,vec*pCurTube->endOdd());
					if(fabs(prev_odd)>EPS)
						overlap_length=prev_odd+pCurTube->startOdd();
					else
						overlap_length=0.0;
					prev_odd=pCurTube->endOdd();
				}
				dist = pCurTube->GetLength();
				ls.m_hFamily= CLsLibrary::GetFootNailFamily();//脚钉
				used_dist = 0;
				naildlg.m_pRod=pCurTube;
				while(1)
				{
					naildlg.m_iLayWing=X0_OR_Y1_WING;
					naildlg.m_bHasWeldRoad=pCurTube->m_bHasWeldRoad;//基准钢管是否存在焊道线
					naildlg.m_xDatumStart=start;
					naildlg.m_xDatumLenVec=vec;
					naildlg.hFirstLineSlot=hFirstLineSlot;
					naildlg.InitState(pCurTube->handle,dist+extra_dist-used_dist,dist,used_dist,naildlg.m_fNailAngle,naildlg.m_fNailAngle2);
					if(odd_dist==0)
					{
						if(naildlg.DoModal()!=IDOK)
							goto end;
						used_dist+=naildlg.m_nNailSpace;
					}
					else	//上次有剩余
						used_dist = odd_dist;
					if(fabs(naildlg.m_fCurNailAngle-naildlg.m_fNailAngle-naildlg.m_fNailAngle2)<1)
						X0_OR_Y1_WING=1;
					else
						X0_OR_Y1_WING=0;
					if(naildlg.m_iNailType==0)	//布置槽钢类型的脚钉板
					{	//第一次添件脚钉连接板的时候弹出设置对话框 以后不再弹出
						restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
						pLineSlot = (CLDSLineSlot*)console.AppendPart(CLS_LINESLOT);
						pLineSlot->m_hPartWeldParent = pCurTube->handle; 
						CLDSLineSlot *pStandardFootNailPlate = (CLDSLineSlot*)console.FromPartHandle(hFirstLineSlot,CLS_LINESLOT);
						if(pStandardFootNailPlate==NULL)
						{
							pLineSlot->CopyProperty(&naildlg.xNailSlot);
							//TODO:pLineSlot->feature赋值，代码不够规范 wjh-2015.9.1
							pLineSlot->feature=naildlg.xNailSlot.feature;
							hFirstLineSlot=pLineSlot->handle;
						}
						else
						{
							pLineSlot->CopyProperty(pStandardFootNailPlate);
							pLineSlot->feature=pStandardFootNailPlate->feature;	//脚钉的长度
						}
						//必须置于pLineSlot->CopyProperty(pStandardFootNailPlate)之后，否则属性会被冲掉 wjh-2015.9.1
						pLineSlot->CopyModuleInstanceInfo(pCurTube);
						f3dPoint workNorm=pCurTube->ucs.axis_x;
						double rotangle=naildlg.m_fCurNailAngle*RADTODEG_COEF;
						if(naildlg.m_iDatumLine==1)	//自焊缝线起始旋转
							rotangle+=pCurTube->CalWeldRoadAngle();
						RotateVectorAroundVector(workNorm,rotangle,axis_z);
						pLineSlot->m_iNormCalStyle=0;	//直接指定槽钢工作面法线 
						pLineSlot->SetWorkPlaneNorm(workNorm);
						pLineSlot->ucs.axis_y=-workNorm;
						pLineSlot->ucs.axis_z = -pCurTube->ucs.axis_z;
						normalize(pLineSlot->ucs.axis_z);
						//RotateVectorAroundVector(pLineSlot->ucs.axis_y,cur_angle,axis_z);//pCurTube->ucs.axis_z);
						pLineSlot->ucs.axis_x=pLineSlot->ucs.axis_y^pLineSlot->ucs.axis_z;
						normalize(pLineSlot->ucs.axis_x);
						if(naildlg.m_bDualSide)
						{	//双侧布脚钉
							pLineSlot2 = (CLDSLineSlot*)console.AppendPart(CLS_LINESLOT);
							pLineSlot2->m_hPartWeldParent = pCurTube->handle; 
							pLineSlot2->CopyProperty(pLineSlot);
							pLineSlot2->feature=pLineSlot->feature;
							//必须置于pLineSlot->CopyProperty(pStandardFootNailPlate)之后，否则属性会被冲掉 wjh-2015.9.1
							pLineSlot2->CopyModuleInstanceInfo(pCurTube);
							f3dPoint workNorm=pCurTube->ucs.axis_x;
							double rotangle=naildlg.m_fCurNailAngle2*RADTODEG_COEF;
							if(naildlg.m_iDatumLine==1)	//自焊缝线起始旋转
								rotangle+=pCurTube->CalWeldRoadAngle();
							RotateVectorAroundVector(workNorm,rotangle,axis_z);
							pLineSlot2->m_iNormCalStyle=0;	//直接指定槽钢工作面法线 
							pLineSlot2->SetWorkPlaneNorm(workNorm);
							pLineSlot2->ucs.axis_y=-workNorm;
							pLineSlot2->ucs.axis_z = -pCurTube->ucs.axis_z;
							normalize(pLineSlot2->ucs.axis_z);
							pLineSlot2->ucs.axis_x=pLineSlot2->ucs.axis_y^pLineSlot2->ucs.axis_z;
							normalize(pLineSlot2->ucs.axis_x);
						}
						else
							pLineSlot2 = NULL;
					}
					else 	//布置角钢
					{
						double wing_wide=0,wing_thick=0;
						pLineAngle=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
						pLineAngle->m_bFootNail=TRUE;//表示该角钢为脚钉
						pLineAngle->CopyModuleInstanceInfo(pCurTube);
						pLineAngle->ucs.axis_z=pCurTube->ucs.axis_x;	
						double rot_angle=pCurTube->CalWeldRoadAngle();
						if(naildlg.m_iDatumLine==1) //以基准边为基准还是以焊道线为基准 
							RotateVectorAroundVector(pLineAngle->ucs.axis_z,rot_angle,axis_z);
						pLineAngle->ucs.axis_x=axis_z;
						normalize(pLineAngle->ucs.axis_x);
						pLineAngle->ucs.axis_y=pLineAngle->ucs.axis_z^pLineAngle->ucs.axis_x;
						normalize(pLineAngle->ucs.axis_y);
						pLineAngle->ucs.axis_z=pLineAngle->ucs.axis_x^pLineAngle->ucs.axis_y;
						normalize(pLineAngle->ucs.axis_z);
						pLineAngle->m_hPartWeldParent=pCurTube->handle; 

						CLDSLineAngle *pStandardFootNailAngle = (CLDSLineAngle*)console.FromPartHandle(hFirstLineSlot,CLS_LINEANGLE);
						if(pStandardFootNailAngle==NULL)
						{
							pLineAngle->CopyProperty(&naildlg.xNailAngle);
							//TODO:pLineSlot->feature赋值，代码不够规范 wjh-2015.9.1
							angle_dlg.m_fParamL=pLineAngle->feature=naildlg.xNailAngle.feature;//angle_dlg.xNailAngle.feature;
							hFirstLineSlot=pLineAngle->handle;
						}
						else
						{
							pLineAngle->CopyProperty(pStandardFootNailAngle);
							angle_dlg.m_fParamL=pLineAngle->feature=pStandardFootNailAngle->feature;	//脚钉的长度
						}
						/*angle_dlg.m_pLineTube=pCurTube;
						angle_dlg.part_type=1;	//防坠角钢
						if(bFirst)
						{
							if(angle_dlg.DoModal()!=IDOK)
								return 0;
							bFirst=FALSE;
						}*/
						pLineAngle->cMaterial = CSteelMatLibrary::RecordAt(angle_dlg.m_iMaterial).cBriefMark;
						pLineAngle->iSeg = SEGI(angle_dlg.m_sSegNo.GetBuffer());
						pLineAngle->SetPartNo(angle_dlg.m_sPartNo.GetBuffer());
						restore_JG_guige(angle_dlg.m_sGuiGe.GetBuffer(),wing_wide,wing_thick);
						pLineAngle->set_wing_thick(wing_thick);
						pLineAngle->set_wing_wide(wing_wide);
					}
					f3dPoint line_vec=pCurTube->End()-pCurTube->Start();
					normalize(line_vec);
					//if(bDownToUp)	//脚钉自下向上布置
					//{
					//	if(pCurTube->pStart->Position(false).z>pCurTube->pEnd->Position(false).z)
					//		pCurTube->ucs.origin = pCurTube->Start()-line_vec*pCurTube->startOdd();
					//	else	//起始点高,终止点低
					//		pCurTube->ucs.origin = pCurTube->End()+line_vec*pCurTube->endOdd();
					//}
					//else //if(!bDownToUp) //脚钉自上向下布置
					//{
					//	if(pCurTube->pStart->Position(false).z>pCurTube->pEnd->Position(false).z)	
					//		pCurTube->ucs.origin = pCurTube->End()+line_vec*pCurTube->endOdd();
					//	else	//起始点高,终止点低
					//		pCurTube->ucs.origin = pCurTube->Start()-line_vec*pCurTube->startOdd();
					//}
					double cur_angle=naildlg.m_fCurNailAngle*RADTODEG_COEF;
					used_dist+=overlap_length;
					overlap_length=0.0;	//考虑重叠部分尺寸一次后清掉重叠尺寸
					if(used_dist>dist)
					{
						odd_dist = used_dist-dist;
						long handle=0;
						if(naildlg.m_iNailType==0)	//槽钢
							handle=pLineSlot->handle;
						else //角钢
							handle=pLineAngle->handle;
						console.DispPartSet.DeleteNode(handle);
						console.DeletePart(handle);
						break;
					}
					if(naildlg.m_iNailType==0)	//槽钢
					{
						//计算脚钉板坐标系原点位置
						double H = pLineSlot->GetHeight();	//槽钢底边宽度
						double W = pLineSlot->GetWidth();	//槽钢肢宽
						double T = pLineSlot->GetThick();
						double dist = sqrt(SQR(0.5*pCurTube->GetDiameter())-SQR(0.5*H-T));
						pLineSlot->ucs.origin = start-pLineSlot->ucs.axis_y*(dist+W)-pLineSlot->ucs.axis_z*(pLineSlot->feature*0.5);
						if(pLineSlot2)
							pLineSlot2->ucs.origin = start-pLineSlot2->ucs.axis_y*(dist+W)-pLineSlot2->ucs.axis_z*(pLineSlot2->feature*0.5);
					}
					else	//角钢
					{
						RotateVectorAroundVector(pLineAngle->ucs.axis_z,cur_angle,axis_z);
						pLineAngle->ucs.axis_y=pLineAngle->ucs.axis_z^pLineAngle->ucs.axis_x;
						normalize(pLineAngle->ucs.axis_y);
						pLineAngle->ucs.axis_z=pLineAngle->ucs.axis_x^pLineAngle->ucs.axis_y;
						normalize(pLineAngle->ucs.axis_z);
						pLineAngle->ucs.origin = start;
					}
					if(naildlg.m_bShiftWing)		//脚钉交替布置
						X0_OR_Y1_WING = !X0_OR_Y1_WING;
					if(fabs(odd_dist)<EPS)	//上次设计没有剩余值
					{
						if(naildlg.m_iNailType==0)	//槽钢
						{
							Add_Pnt(pLineSlot->ucs.origin, pLineSlot->ucs.origin, vec*ftol(used_dist));
							if(pLineSlot2)
								Add_Pnt(pLineSlot2->ucs.origin, pLineSlot2->ucs.origin, vec*ftol(used_dist));
						}
						else	//角钢
							Add_Pnt(pLineAngle->ucs.origin, pLineAngle->ucs.origin, vec*ftol(used_dist));;
					}
					else	//上次设计时有剩余值
					{
						if(naildlg.m_iNailType==0)	//槽钢
						{
							Add_Pnt(pLineSlot->ucs.origin, pLineSlot->ucs.origin,vec*ftol(odd_dist));
							if(pLineSlot2)
								Add_Pnt(pLineSlot2->ucs.origin, pLineSlot2->ucs.origin,vec*ftol(odd_dist));
						}
						else	//角钢
							Add_Pnt(pLineAngle->ucs.origin, pLineAngle->ucs.origin,vec*ftol(odd_dist));;
						odd_dist = 0.0;
					}
					if(naildlg.m_iNailType==0)	//槽钢
					{	//脚钉板基本信息
						f3dPoint datumStartPos,datumLenVec,pt=start+vec*ftol(used_dist);//-pLineSlot->ucs.axis_z*pLineSlot->feature*0.5;
						pt+=pLineSlot->WorkPlaneNorm()*pCurTube->GetDiameter()*0.5;
						pLineSlot->desDatumPoint.SetPosition(pt);
						//设定脚钉槽钢底座的参数化定位信息
						pLineSlot->hAttachTube=pCurTube->handle;
						pLineSlot->desDatumPoint.datum_pos_style=11;//柱面定位点
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.hLineTube=pCurTube->handle;
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag|=0x02;
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.bSpecRo=FALSE;
						if((bDownToUp &&pCurTube->pStart->Position('Z')>pCurTube->pEnd->Position('Z')) ||	//脚钉自下向上布置
							(!bDownToUp&&pCurTube->pStart->Position('Z')<pCurTube->pEnd->Position('Z')))	//脚钉自上向下布置
						{
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=pCurTube->pStart->handle;
							datumStartPos=pCurTube->GetDatumPos(pCurTube->pStart);
							datumLenVec=pCurTube->End()-pCurTube->Start();
						}
						else
						{
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-pCurTube->pEnd->handle;
							datumStartPos=pCurTube->GetDatumPos(pCurTube->pEnd);
							datumLenVec=pCurTube->Start()-pCurTube->End();
						}
						normalize(datumLenVec);
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset=(pt-datumStartPos)*datumLenVec;
						if(naildlg.m_bHorizonSlotNail)	//水平布置脚钉槽钢底座
						{
							f3dPoint verify_vec=pt-datumStartPos;
							f3dPoint verify_vec2=(verify_vec^datumLenVec)^datumLenVec;
							normalize(verify_vec2);
							if(verify_vec*verify_vec2>0)
								pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec= verify_vec2;
							else
								pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=-verify_vec2;
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=4;
							pLineSlot->m_iNormCalStyle=2;
						}
						else if(naildlg.m_iDatumLine==0)		//钢管基准边为旋转起始边
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=2;
						else if(naildlg.m_iDatumLine==1)//钢管焊道边为旋转起始边
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=3;
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_angle=naildlg.m_fCurNailAngle;
						pLineSlot->fLen=pLineSlot->feature;
						pLineSlot->CalTubeSlotPos();
						//螺栓
						//restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
						Add_Pnt(ls_pos,pLineSlot->Start()+pLineSlot->ucs.axis_z*pLineSlot->feature*0.5,pLineSlot->ucs.axis_y*pLineSlot->GetThick());
						ls.set_norm(-pLineSlot->ucs.axis_y);
						//螺栓位置设计信息填充
						pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls,false);
						pLs->iSeg=pLineSlot->iSeg;
						pLs->layer(0)=pCurTube->Layer(0);
						pLs->cfgword=pLineSlot->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pLs->AddL0Thick(pLineSlot->handle,TRUE);
						pLs->des_base_pos.hPart = pLineSlot->handle;
						pLs->des_base_pos.cLocationStyle=TRUE; 
						pLs->des_base_pos.wing_offset_dist=0;
						pLs->des_base_pos.len_offset_dist=pLineSlot->feature*0.5;
						pLs->des_base_pos.norm_offset.AddThick(-pLineSlot->handle,TRUE);
						pLs->des_work_norm.norm_style=4;	//基准构件上的相对方向
						pLs->des_work_norm.nearVector.Set(0,-1,0);
						pLs->des_work_norm.hVicePart=pLineSlot->handle;
						pLs->ucs.origin=ls_pos;
						pLs->CalGuigeAuto();
						pLs->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());	
						if(pLineSlot->FindLsByHandle(pLs->handle)==NULL)
							pLineSlot->AppendMidLsRef(pLs->GetLsRef());
						pLineSlot->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pLineSlot->GetSolidPartObject());
						g_pSolidDraw->Draw();
						if(pLineSlot2)
						{
							//f3dPoint datumStartPos,datumLenVec;
							pt=start+vec*ftol(used_dist);//-pLineSlot->ucs.axis_z*pLineSlot->feature*0.5;
							pt+=pLineSlot2->WorkPlaneNorm()*pCurTube->GetDiameter()*0.5;
							pLineSlot2->desDatumPoint.SetPosition(pt);
							//设定脚钉槽钢底座的参数化定位信息
							pLineSlot2->hAttachTube=pCurTube->handle;
							pLineSlot2->desDatumPoint.datum_pos_style=11;//柱面定位点
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.hLineTube=pCurTube->handle;
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag|=0x02;
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.bSpecRo=FALSE;
							if((bDownToUp &&pCurTube->pStart->Position('Z')>pCurTube->pEnd->Position('Z')) ||	//脚钉自下向上布置
								(!bDownToUp&&pCurTube->pStart->Position('Z')<pCurTube->pEnd->Position('Z')))	//脚钉自上向下布置
							{
								pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=pCurTube->pStart->handle;
								datumStartPos=pCurTube->GetDatumPos(pCurTube->pStart);
								datumLenVec=pCurTube->End()-pCurTube->Start();
							}
							else
							{
								pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-pCurTube->pEnd->handle;
								datumStartPos=pCurTube->GetDatumPos(pCurTube->pEnd);
								datumLenVec=pCurTube->Start()-pCurTube->End();
							}
							normalize(datumLenVec);
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset=(pt-datumStartPos)*datumLenVec;
							if(naildlg.m_bHorizonSlotNail)	//水平布置脚钉槽钢底座
							{
								f3dPoint verify_vec=pt-datumStartPos;
								f3dPoint verify_vec2=(verify_vec^datumLenVec)^datumLenVec;
								normalize(verify_vec2);
								if(verify_vec*verify_vec2>0)
									pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec= verify_vec2;
								else
									pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.RayVector.ray_vec=-verify_vec2;
								pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=4;
								pLineSlot2->m_iNormCalStyle=2;
							}
							else if(naildlg.m_iDatumLine==0)		//钢管基准边为旋转起始边
								pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=2;
							else if(naildlg.m_iDatumLine==1)//钢管焊道边为旋转起始边
								pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=3;
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.ray_angle=naildlg.m_fCurNailAngle2;
							pLineSlot2->fLen=pLineSlot2->feature;
							pLineSlot2->CalTubeSlotPos();
							//螺栓
							//restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
							Add_Pnt(ls_pos,pLineSlot2->Start()+pLineSlot2->ucs.axis_z*pLineSlot2->feature*0.5,pLineSlot2->ucs.axis_y*pLineSlot2->GetThick());
							ls.set_norm(-pLineSlot2->ucs.axis_y);
							//螺栓位置设计信息填充
							pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
							pLs->CopyProperty(&ls,false);
							pLs->iSeg=pLineSlot->iSeg;
							pLs->layer(0)=pCurTube->Layer(0);
							pLs->cfgword=pLineSlot->cfgword;	//调整螺栓配材号与基准构件配材号一致
							pLs->AddL0Thick(pLineSlot2->handle,TRUE);
							pLs->des_base_pos.hPart = pLineSlot2->handle;
							pLs->des_base_pos.cLocationStyle=TRUE; 
							pLs->des_base_pos.wing_offset_dist=0;
							pLs->des_base_pos.len_offset_dist=pLineSlot2->feature*0.5;
							pLs->des_base_pos.norm_offset.AddThick(-pLineSlot2->handle,TRUE);
							pLs->des_work_norm.norm_style=4;	//基准构件上的相对方向
							pLs->des_work_norm.nearVector.Set(0,-1,0);
							pLs->des_work_norm.hVicePart=pLineSlot2->handle;
							pLs->ucs.origin=ls_pos;
							pLs->CalGuigeAuto();
							pLs->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());	
							if(pLineSlot2->FindLsByHandle(pLs->handle)==NULL)
								pLineSlot2->AppendMidLsRef(pLs->GetLsRef());
							pLineSlot2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
							g_pSolidDraw->NewSolidPart(pLineSlot2->GetSolidPartObject());
							g_pSolidDraw->Draw();
						}
					}
					else //角钢 
					{
						pLineAngle->SetStartOdd(-pCurTube->GetDiameter()*0.5);
						GEPOINT xAngleStart=pLineAngle->ucs.origin;
						GEPOINT xAngleEnd  =pLineAngle->ucs.origin+pLineAngle->ucs.axis_z*(angle_dlg.m_fParamL+pCurTube->GetDiameter()*0.5);
						pLineAngle->SetStart(xAngleStart);
						pLineAngle->SetEnd(xAngleEnd);
						pLineAngle->desStartPos.datumPoint.SetPosition(pLineAngle->Start());
						pLineAngle->desEndPos.datumPoint.SetPosition(pLineAngle->End());
						pLineAngle->des_norm_y.bSpecific=TRUE;
						pLineAngle->des_norm_y.spec_norm.vector=-pLineAngle->ucs.axis_x;
						pLineAngle->des_norm_x.bSpecific=FALSE;
						pLineAngle->des_norm_x.bByOtherWing=TRUE;
						//设置设终端位置后再设置法线方向
						pLineAngle->set_norm_x_wing(-pLineAngle->ucs.axis_y);
						pLineAngle->set_norm_y_wing(-pLineAngle->ucs.axis_x);
						pLineAngle->cal_x_wing_norm();
						pLineAngle->cal_y_wing_norm();
						xAngleStart+=(pLineAngle->vxWingNorm+pLineAngle->vyWingNorm)*(pLineAngle->size_wide*0.5);
						xAngleEnd  +=(pLineAngle->vxWingNorm+pLineAngle->vyWingNorm)*(pLineAngle->size_wide*0.5);
						pLineAngle->SetStart(xAngleStart);
						pLineAngle->SetEnd(xAngleEnd);
						pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
						g_pSolidDraw->Draw();
					}
					Ta.EndUndoListen();
				}
				g_pSolidDraw->SetEntSnapStatus(pCurTube->handle,FALSE);
			}
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
end:
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;	
}
//布置脚踏板
int  CLDSView::LayoutPedalPlate()
{
	CCmdLockObject cmdlock(this);
	if (!cmdlock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
	//确定脚踏板布置方式
	CString cmdStr,valueStr;
	static char cToward = 'U';
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Layout pedal plate[from up to down(D)/from down to up(U)]<%C>:", cToward);
#else 
	cmdStr.Format("布置脚踏板[自上而下(D)/自下而上(U)]<%C>:", cToward);
#endif
	pCmdLine->FillCmdLine(cmdStr, "");
	while (1)
	{
		if (!pCmdLine->GetStrFromCmdLine(valueStr, CCmdLineDlg::KEYWORD_AS_RETURN, "D|U"))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}	
		if (valueStr.CompareNoCase("d") == 0|| valueStr.CompareNoCase("D") == 0)
			cToward = toupper(valueStr[0]);
		else if (valueStr.CompareNoCase("u") == 0 || valueStr.CompareNoCase("U") == 0)
			cToward = toupper(valueStr[0]);
		else if (valueStr.GetLength() > 0)
		{
			pCmdLine->FinishCmdLine("关键字输入有误,请重新录入!");
			pCmdLine->FillCmdLine(cmdStr, "");
			continue;
		}
		break;
	}
	BOOL bDownToUp = (cToward == 'U') ? TRUE : FALSE;
	//选择待布置脚踏板的主材角钢
	DWORD dwhObj = 0, dwExportFlag = 0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_LINEANGLE));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE, SNAP_LINE);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select leg member which will layout pedal plate<Click empty to over>:", "");
#else 
	pCmdLine->FillCmdLine("请依次选择要布置脚踏板的主材<空点鼠标左键结束选择>:", "");
#endif
	PARTSET partset;
	THANDLE hStart = 0, hCur = 0, hEnd = 0;
	CLDSLineAngle* pLastJg = NULL;
	while (1)
	{
		if (g_pSolidSnap->SnapObject(&dwhObj, &dwExportFlag, &verifier) < 0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj, dwExportFlag);
		dwhObj = obj.hRelaObj;
		if (dwhObj == 0 && obj.ciTriggerType == SELOBJ::TRIGGER_KEYRETURN)
			break;
		else if (dwhObj > 0)
		{
			CLDSLineAngle* pSelJg = (CLDSLineAngle*)console.FromPartHandle(dwhObj, CLS_LINEANGLE);
			if (pSelJg == NULL || pSelJg->pStart == NULL || pSelJg->pEnd == NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Don't select effective rod!Please select again!");
#else 
				AfxMessageBox("没有选择有效的杆件!请重新选择!");
#endif
				continue;
			}
			if (pLastJg != NULL && pLastJg == pSelJg)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Repeat to select the same leg member,Please select again!");
#else 
				AfxMessageBox("重复选择了同一根主材,请重新选择!");
#endif
				continue;
			}
			if (pSelJg->pStart->handle == hEnd)
				hCur = pSelJg->pEnd->handle;
			else if (pSelJg->pEnd->handle == hEnd)
				hCur = pSelJg->pStart->handle;
			else if (hEnd != 0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The selected rod has no connection with previous rod，Please select again!");
#else 
				AfxMessageBox("所选杆件与上一杆件没有连接，请重新选择!");
#endif
				continue;
			}
			else if (bDownToUp)	//自下向上布置
			{
				if (pSelJg->pStart->Position(false).z > pSelJg->pEnd->Position(false).z)
				{
					hStart = pSelJg->pStart->handle;
					hCur = pSelJg->pEnd->handle;
				}
				else	//起始点高,终止点低
				{
					hStart = pSelJg->pEnd->handle;
					hCur = pSelJg->pStart->handle;
				}
			}
			else //if(!bDownToUp) //自上向下布置
			{
				if (pSelJg->pStart->Position(false).z > pSelJg->pEnd->Position(false).z)
				{
					hStart = pSelJg->pEnd->handle;
					hCur = pSelJg->pStart->handle;
				}
				else	//起始点高,终止点低
				{
					hStart = pSelJg->pStart->handle;
					hCur = pSelJg->pEnd->handle;
				}
			}
			hEnd = hCur;
			pLastJg = pSelJg;
			partset.append(pSelJg);
			g_pSolidDraw->SetEntSnapStatus(pSelJg->handle, TRUE);
		}
	}
	if (partset.GetNodeNum() <= 0)
	{
		AfxMessageBox("没有选中待布置的角钢，设计失败!");
		pCmdLine->CancelCmdLine();
		return 0;
	}
	//选择起始脚踏板
	CLDSLineAngle* pFirstJg = (CLDSLineAngle*)partset.GetFirst();
	CLDSParamPlate* pStartPedalPlate = NULL;
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PARAMPLATE));
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select start climbing nails and press return key, none nails type 'N' key:", "");
#else 
	pCmdLine->FillCmdLine("请选择起始脚踏板,没有按回车键:", "");
#endif
	while (1)
	{
		if (g_pSolidSnap->SnapObject(&dwhObj, &dwExportFlag, &verifier) < 0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj, dwExportFlag);
		dwhObj = obj.hRelaObj;
		if (dwhObj == 0 && obj.ciTriggerType == SELOBJ::TRIGGER_KEYRETURN)
			break;
		pStartPedalPlate = (CLDSParamPlate*)console.FromPartHandle(dwhObj, CLS_PARAMPLATE);
		if (pStartPedalPlate && pStartPedalPlate->m_iParamType == TYPE_PEDALPLATE)
		{
			long hBaseAngle = 0;
			pStartPedalPlate->GetDesignItemValue(KEY2C("A"), &hBaseAngle);
			if (pFirstJg->handle == hBaseAngle)
				break;
		}
	}
	if (pStartPedalPlate)
	{
		g_pSolidDraw->SetEntSnapStatus(pStartPedalPlate->handle);
		pCmdLine->FinishCmdLine(CXhChar16("0x%X", pStartPedalPlate->handle));
		SCOPE_STRU scope;
		scope.VerifyVertex(pStartPedalPlate->ucs.origin);
		scope.fMinX -= 150;
		scope.fMaxX += 150;
		scope.fMinY -= 150;
		scope.fMaxY += 150;
		scope.fMinZ -= 150;
		scope.fMaxZ += 150;
		g_pSolidOper->FocusTo(scope, 0.2);
	}
	else
		pCmdLine->FinishCmdLine();
	//布置脚踏板
	CUndoOperObject undo(&Ta, true);
	f3dPoint view_norm= console.GetActiveView()->ucs.axis_z;
	double ddx = view_norm * pFirstJg->vxWingNorm;
	double ddy = view_norm * pFirstJg->vyWingNorm;
	BYTE ciWorkWing = (fabs(ddx) > fabs(ddy)) ? 0 : 1;
	double fPreOdd = 0, fPreRodRemainingLen = 0;
	for (CLDSLineAngle *pCurJg = (CLDSLineAngle*)partset.GetFirst(); pCurJg; pCurJg = (CLDSLineAngle*)partset.GetNext())
	{
		g_pSolidDraw->SetEntSnapStatus(pCurJg->handle, TRUE);
		long hCurLineNode = hStart;
		f3dPoint line_vec = (pCurJg->End() - pCurJg->Start()).normalized();
		f3dPoint line_ptS = pCurJg->Start() - line_vec * pCurJg->startOdd();
		f3dPoint line_ptE = pCurJg->End() + line_vec * pCurJg->endOdd();
		BYTE ciLinkS0E1 = (pCurJg->pStart->handle == hCurLineNode) ? 0 : 1;
		hStart = (ciLinkS0E1 == 0) ? pCurJg->pEnd->handle : pCurJg->pStart->handle;
		double fCurLinkOdd = (ciLinkS0E1 == 0) ? pCurJg->startOdd() : pCurJg->endOdd();
		double fJointGap= (fabs(fPreOdd) > EPS) ? (fPreOdd + fCurLinkOdd) : 0;	//接头间隙
		fPreOdd = (ciLinkS0E1 == 0) ? pCurJg->endOdd() : pCurJg->startOdd();
		double fSumLen = pCurJg->GetLength(), fUsedLen = 0, fExtraLen = 0;
		if (pStartPedalPlate && pCurJg == pFirstJg)
		{
			double ddx = pStartPedalPlate->ucs.axis_z*pCurJg->vxWingNorm;
			double ddy = pStartPedalPlate->ucs.axis_z*pCurJg->vyWingNorm;
			ciWorkWing = (fabs(ddx) > fabs(ddy)) ? 0 : 1;
			f3dPoint ptS = (ciLinkS0E1 == 0) ? line_ptS : line_ptE;
			f3dPoint ptE = (ciLinkS0E1 == 0) ? line_ptE : line_ptS;
			f3dPoint vec = (ptE - ptS).normalized();
			f3dPoint start = pStartPedalPlate->ucs.origin;
			project_point(start, pCurJg->xPosStart, pStartPedalPlate->ucs.axis_z);
			SnapPerp(&start, pCurJg->Start(), pCurJg->End(), start);
			fExtraLen = (start - ptS)*vec;
		}
		if (g_sysPara.FootNailDist > fSumLen - fExtraLen)
		{	//预判当前杆件剩余长度是否足够布置下一个脚踏板
			fPreRodRemainingLen = fSumLen - fExtraLen;
			continue;
		}
		while (1)
		{
			CDesPedalPlateDlg dlg;
			dlg.m_bNewPlate = TRUE;
			dlg.m_pPedalPlate = NULL;
			dlg.m_pDatumAngle = pCurJg;
			dlg.layout_para.m_ciWorkWing = ciWorkWing;
			dlg.layout_para.m_ciLineS0E1 = ciLinkS0E1;
			dlg.layout_para.m_fSumLen = fSumLen;
			dlg.layout_para.m_fExtraLen = fExtraLen;
			dlg.layout_para.m_fCanLen = fSumLen - fExtraLen - fUsedLen;
			dlg.layout_para.m_fUseLen = fUsedLen;
			dlg.layout_para.m_fPreRemainDist = fPreRodRemainingLen;
			dlg.layout_para.m_fJointGap = fJointGap;
			if (dlg.DoModal() != IDOK)
				return 0;
			fUsedLen = dlg.CalOffDist();
			fPreRodRemainingLen = fJointGap = fExtraLen = 0;
			CLDSParamPlate* pPedalPlate = dlg.CreatePedalPlate();
			pPedalPlate->DesignPlate();
			pPedalPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pPedalPlate->GetSolidPartObject());
			g_pSolidDraw->Draw();
			//预判当前杆件剩余长度是否足够布置下一个脚踏板
			double pre_used_dist = fUsedLen + g_sysPara.FootNailDist;
			if (pre_used_dist > fSumLen + fExtraLen)
			{
				fPreRodRemainingLen = fSumLen + fExtraLen - fUsedLen;
				break;
			}
		}
		g_pSolidDraw->SetEntSnapStatus(pCurJg->handle, FALSE);
	}
	//
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:", "");
#else 
	pCmdLine->FillCmdLine("命令:", "");
#endif
	return 0;
}
//设计底脚板
void CLDSView::OnFootPlank() {
	DesignFoot(); 
}
int CLDSView::DesignOldFoot()		//设计旧式塔脚
{
#ifdef __SMART_DRAW_INC_
	DesignFoot();
#else
	m_nPrevCommandID=ID_FOOT_PLANK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat base plates";
#else 
	m_sPrevCommandName="重复底脚板";
#endif
	g_pSolidSet->SetOperType(OPER_OSNAP);
	m_curTask = TASK_DESIGN_FOOT_PLANK;
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
	//切换到单线显示状态
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please select base plates' nodes and leg member!");
#else 
	g_pPromptMsg->SetMsg("请选择要设计底脚板的节点及主材角钢!");
#endif
#endif
	return 0;
}
//设计牛蹄板
int CLDSView::DesignHoof()			//设计牛蹄板
{
	OnDesHoofPlank();
	return TRUE;
}
void CLDSView::OnDesHoofPlank() 
{	//组合角钢接头处的牛蹄板连接
	m_nPrevCommandID=ID_DES_HOOF_PLANK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat design hoof plates";
#else 
	m_sPrevCommandName="重复牛蹄板";
#endif
	g_pSolidSet->SetOperType(OPER_OSNAP);
	m_curTask = TASK_DESIGN_HOOF_PLANK;
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
	//切换到单线显示状态
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please select base plates' nodes and leg member!");
#else 
	g_pPromptMsg->SetMsg("请选择要设计底脚板的节点及主材角钢!");
#endif
}
//切割角钢
void CLDSView::OnCutJg() 
{
	m_nPrevCommandID=ID_CUT_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to specify datum angle's leg cut";
#else 
	m_sPrevCommandName="重复指定基准角钢切角切肢";
#endif
	m_curTask = TASK_CAL_CUT_JG;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
	long *id_arr=NULL;
	int selects=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(selects>0)
	{
		for(int i=0;i<selects;i++)
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)console.FromPartHandle(id_arr[i],CLS_LINEANGLE);
			if(pAngle!=NULL)
				m_setSnapObjects.append(pAngle);
			if(m_setSnapObjects.GetNodeNum()==2)
				break;
		}
		if(m_setSnapObjects.GetNodeNum()==2)
			FinishCalCutJg(m_setSnapObjects[0],m_setSnapObjects[1]);
		else if(m_setSnapObjects.GetNodeNum()==1)
		{
			m_nObjectSnapedCounts=1;
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("Select one datum angle to cut ray angle！");
#else 
			g_pPromptMsg->SetMsg("选择一根基准角钢用以切割射线角钢！");
#endif
		}
		return;
	}
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please select a cutted ray angle!");
#else 
	g_pPromptMsg->SetMsg("请选择被切割的射线角钢!");
#endif
}
//设计节点板
void CLDSView::OnDesignJdb() 
{
	m_nPrevCommandID=ID_DESIGN_JDB;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to design node plate";
#else 
	m_sPrevCommandName="重复节点板";
#endif
	Command("DesignNodePlate");
}

CMirMsgDlg mirmsgDlg;	//连接板对称信息
int CLDSView::DesignNodePlate()	//通过命令行调用FinishDesignJdb
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	return FinishDesignJdb(NULL);
}
BOOL CLDSView::FinishDesignJdb(CLDSNode *pSelNode) 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSPlate *pCurPlate = NULL,*pMirPlank=NULL,*pSideMirPlank=NULL;
	CDesignJdb designJdb;
	BOOL bHasMirJdb = FALSE;		//是否对称生成对角部位的连接板
 	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pSelNode==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select nodes which to be designed!","");
#else 
		pCmdLine->FillCmdLine("DesignNodePlate 选择节点板的依赖节点:","");
#endif
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			pSelNode= console.FromNodeHandle(dwhObj);
			if(pSelNode&&pSelNode->IsNode())
				break;
		}
	}
	g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);
	pCmdLine->FinishCmdLine(CXhChar16("节点0x%X",pSelNode->handle));

	BOOL bTerminateByUser=FALSE;
	CLogErrorLife loglife;
	CUndoOperObject undo(&Ta,true);
	try{
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSelNode->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Without modify permission of the node！";
#else 
			throw "没有此节点的修改权限！";
#endif
		g_pSolidDraw->SetEntSnapStatus(pSelNode->handle,TRUE);	//高亮显示节点板基准点
		//选择装配定位杆件
		f3dLine line_3d;
		CString cmdStr;
		SmartPartPtr pBasePart;
		CLDSLinePart *pBaseLinePart=NULL,*pNodeFatherPart=NULL;
		//g_pSolidDraw->ReleaseSnapStatus();
		//static int iPlateFaceType;	//单面板
		int iPlateFaceType=1;	//单面板(刘伟说客户反映节点板类型还是去掉记忆功能更实用 wjh-2015.6.24)
		iPlateFaceType=max(1,iPlateFaceType);
		iPlateFaceType=min(4,iPlateFaceType);
		BOOL bHasSpecifyType=FALSE;	//是否已制定过节点板类型
		pNodeFatherPart=(CLDSLinePart*)console.FromPartHandle(pSelNode->hFatherPart,CLS_LINEPART);
		if(pSelNode->m_cPosCalType==4&&pNodeFatherPart&&pNodeFatherPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//选择节点板类型 1.单面板 2.双面板 3.三面板 4.内部交叉板
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select node-plate's type<1.planar plate|2.2-plane plate|3.3-plane plate|4.inner cross plate>[4]:","");
#else 
			pCmdLine->FillCmdLine("DesignNodePlate 请指定节点板类型<1.单面板|2.双面板|3.三面板|4.内部交叉板>[4]:","");
#endif
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN|CCmdLineDlg::LBUTTONDOWN_AS_RETURN,"1|2|3|4"))
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				if(cmdStr.GetLength()==0)
					iPlateFaceType=4;
				else 
					iPlateFaceType = atoi(cmdStr);
				if(iPlateFaceType<1||iPlateFaceType>4)
				{
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Illegal input,please repeat to select node-plate's type<1.planar plate|2.2-plane plate|3.3-plane plate|4.inner cross plate>[4]:","");
#else 
					pCmdLine->FillCmdLine("非法输入，请重新指定节点板类型<1.单面板|2.双面板|3.三面板|4.内部交叉板>[4]:","");
#endif
					continue;
				}
				else
					break;
			}
			bHasSpecifyType=TRUE;
			pBasePart=pNodeFatherPart;
			pBaseLinePart=pNodeFatherPart;
		}
		
		if(iPlateFaceType!=4)
		{
			CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_LINE|SNAP_ARC);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART|SELECT_ARCPART);
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select node-plate's assemble locate rod:","");
#else 
			pCmdLine->FillCmdLine("DesignNodePlate 请选择节点板的装配定位杆件:","");
#endif
			int ret=0;
			DWORD dwhObj=0,dwExportFlag=0;
			while(1)
			{
				if((ret=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;
				if(dwhObj==0&&obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN)
				{
					long* id_arr=NULL;
					if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
						dwhObj=id_arr[0];
				}
				if(dwhObj>0)
					pBasePart=console.FromPartHandle(dwhObj);
				if(pBasePart.IsNULL())
					continue;
				if(pBasePart->IsLinePart())
					pBaseLinePart=pBasePart.pRod;
				break;
			}
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBasePart->handle));
		}
		if(pBaseLinePart)
			g_pSolidDraw->SetEntSnapStatus(pBaseLinePart->handle);
		//未指定节点板类型
		if(pBaseLinePart==NULL)	//以弧形杆件为基准，只能做单面板
			iPlateFaceType=1;
		else if(!bHasSpecifyType)
		{	//根据基准杆件类型预判节点板类型
			if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
			{
				TUBEJOINT *pCurJoint=NULL;
				if(pBaseLinePart->pStart==pSelNode)
					pCurJoint=&((CLDSLineTube*)pBaseLinePart)->m_tJointStart;
				else if(pBaseLinePart->pEnd==pSelNode)
					pCurJoint=&((CLDSLineTube*)pBaseLinePart)->m_tJointEnd;
				if(pCurJoint&&pCurJoint->type==1&&pCurJoint->hLinkObj>0&&pCurJoint->hViceLinkObj<0)
				{
					CLDSLineTube *pCoupleTube=(CLDSLineTube*)console.FromPartHandle(pCurJoint->hLinkObj,CLS_LINETUBE);
					if(pCoupleTube&&!(fabs(pBaseLinePart->ucs.axis_y*pCoupleTube->ucs.axis_y)>EPS_COS))
						iPlateFaceType=2;	//基准钢管为对接相贯，此处应该生成双面板
				}
			}
			else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pBaseLinePart)->group_father_jg_h>0x20)
			{	//涉及组合角钢时，优先以组合角钢为装配定位基准杆件 wjh-2016.11.04
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)Ta.Parts.FromHandle(((CLDSLineAngle*)pBaseLinePart)->group_father_jg_h,CLS_GROUPLINEANGLE);
				if(pGroupAngle)
					pBasePart=pBaseLinePart=pGroupAngle;
			}
			//选择节点板类型 1.单面板 2.双面板 3.三面板 4.内部交叉板
			CString sPrompt="";
#ifdef AFX_TARG_ENU_ENGLISH
			sPrompt.Format("Please select node-plate's type<1.planar plate|2.2-plane plate|3.3-plane plate|4.inner cross plate>[%d]:",iPlateFaceType);
#else 
			sPrompt.Format("DesignNodePlate 请指定节点板类型<1.单面板|2.双面板|3.三面板|4.内部交叉板>[%d]:",iPlateFaceType);
#endif
			pCmdLine->FillCmdLine(sPrompt,"");
			while(1)
			{
				//由于前面使用SnapObject时可能会有鼠标左键抬起的消息残留，故只能用LBUTTONDOWN_AS_RETURN　wjh-2016.10.25
				if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN|CCmdLineDlg::LBUTTONDOWN_AS_RETURN,"1|2|3|4"))
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				if(cmdStr.GetLength()!=0)
					iPlateFaceType = atoi(cmdStr);
				else
					pCmdLine->FinishCmdLine(CXhChar16("%d",iPlateFaceType));
				if(iPlateFaceType<1||iPlateFaceType>4)
				{
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Illegal input,please repeat to select node-plate's type<1.planar plate|2.2-plane plate|3.3-plane plate|4.inner cross plate>[4]:","");
#else 
					pCmdLine->FillCmdLine("非法输入，请重新指定节点板类型<1.单面板|2.双面板|3.三面板|4.内部交叉板>[4]:","");
#endif
					continue;
				}
				else
					break;
			}
		}
		//
		g_pSolidSet->SetDisplayType(DISP_SOLID);//切换到实体显示模式
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->Draw();
		pCurPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pCurPlate->cfgword=pSelNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pCurPlate->designInfo.m_hBaseNode = pSelNode->handle;
		if(pBasePart.IsHasPtr())
			pCurPlate->designInfo.m_hBasePart = pBasePart->handle;
		designJdb.SetViewFlag(m_eViewFlag);
		CStackVariant<char> stackprop0(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_BY_SUMM_THICK);
		//下一行代码已改由系统配置属性来控制 wjh-2019.8.25
		//CStackVariant<char> stackprop0i(&CLDSBolt::BOLTL0_PREFER_MODE,CLDSBolt::L0CAL_INC_GAP_THICK);
		if(iPlateFaceType<4)
		{	//普通连接板
			pCurPlate->face_N = iPlateFaceType;
			pCurPlate->jdb_style = 0;
			if(!designJdb.DesignCommonPlank(pCurPlate))
			{
				bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Design failed";
#else 
				throw "设计失败";
#endif
			}
		}
		else //if(iPlateFaceType==4)//设计的是交叉板(需要特殊处理)
		{
			pCurPlate->jdb_style = 1;
			pCurPlate->face_N = 1;
			if(!designJdb.DesignCrossPlank(pCurPlate))
			{
				bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Design failed";
#else 
				throw "设计失败";
#endif
			}
		}
		if(designJdb.m_pPlank)
		{
			designJdb.m_pPlank->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(designJdb.m_pPlank->GetSolidPartObject());
			mirmsgDlg.m_bUseMirrorSymmetry=TRUE;
			if(mirmsgDlg.DoModal()==IDOK)
				MirTaAtom(designJdb.m_pPlank,mirmsgDlg.mirmsg);
			if(designJdb.m_pPlank->GetThick()<=0)
			{
				console.DispPartSet.DeleteNode(designJdb.m_pPlank->handle);
				console.DeletePart(designJdb.m_pPlank->handle);
			}
		}
		g_pSolidDraw->Draw();
	}
	catch(char *s)
	{
		//发生异常设计失败,删除系统中已生成的错误节点板
		if(pCurPlate)
		{
			console.DispPartSet.DeleteNode(pCurPlate->handle);
			console.DeletePart(pCurPlate->handle);
		}
		if(!bTerminateByUser)
			AfxMessageBox(s);	//提示异常错误信息
		g_pSolidDraw->ReleaseSnapStatus();
		return FALSE;
	}
	
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
#endif
	FinishDesignJdb(NULL);	//继续执行节点板设计命令(刘伟说汇金通提出)　wjh-2016.12.7
	return TRUE;
}

//折叠板设计
void CLDSView::OnFoldPlank() 
{
	m_nPrevCommandID=ID_FOLD_PLANK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to design folded plate";
#else 
	m_sPrevCommandName="重复折叠板";
#endif
	CLDSPlate *pCurPlate=NULL;
	CDesignJdb designJdb;
	CLDSNode *pBaseNode[2]={NULL};
	CLDSLineAngle *pBaseJg[2]={NULL},*pMirJg=NULL;
	int i;
	//CLDSNode::ATTACH_PART *pAttach=NULL;
	//-----vvvvvvv-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
	if(!LockFunc())
		return;
	UINT nRetCode=1;	//现已不需要检测加密锁状态了 wjh-2017.9.18
	BOOL bTerminateByUser=FALSE;
	Ta.BeginUndoListen();
	try
	{
		if(m_eViewFlag == PERSPECTIVE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Can't design folded plate in perspective view,please convert to another view";
#else 
			throw "不能在透视图下进行折叠板设计,请转到其它视图进行折叠板设计";
#endif
		if(m_eViewFlag==RANDOM_VIEW)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Can't design folded plate in spread view,please convert to another view";
#else 
			throw "不能在展开图下进行折叠板设计,请转到其它视图进行折叠板设计";
#endif
#ifdef DOG_CHECK
		if(nRetCode!=1)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Hardware lock's detection is wrong,program is error!";
#else 
			throw "检测加密狗出错,程序出错!";
#endif
#endif
		///////////////捕捉区域/////////////////////////
		f3dLine line;
		f3dPoint *point;
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select first angle in turn!");
#else 
		g_pPromptMsg->SetMsg("请依次选择第一根角钢!");
#endif
		while(g_pSolidSnap->SnapLine(line)>0)
		{
			pBaseJg[0]=(CLDSLineAngle*)console.FromPartHandle(line.ID,CLS_LINEANGLE);
			if(pBaseJg[0])
				break;
		}
		if(pBaseJg[0])
			g_pSolidDraw->SetEntSnapStatus(pBaseJg[0]->handle);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Quit midway";
#else 
			throw "中途退出";
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select first angle's design nodes int turn!");
#else 
		g_pPromptMsg->SetMsg("请依次选择第一根角钢的设计节点!");
#endif
		while(g_pSolidSnap->SnapPoint(point)>0)
		{
			pBaseNode[0]=console.FromNodeHandle(point->ID);
			if(theApp.m_bCooperativeWork&&pBaseNode[0]&&!theApp.IsHasModifyPerm(pBaseNode[0]->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of the node！";
#else 
				throw "没有此节点的修改权限！";
#endif
			if(pBaseNode[0]&&pBaseJg[0]->pStart==pBaseNode[0])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[0]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's start！";
#else 
					throw "没有此角钢始端的修改权限！";
#endif
				break;
			}
			else if(pBaseNode[0]&&pBaseJg[0]->pEnd==pBaseNode[0])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[0]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's end！";
#else 
					throw "没有此角钢终端的修改权限！";
#endif
				break;
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The selected angle don't fit!");
#else 
				AfxMessageBox("选择角钢不合适!");
#endif
		}
		if(pBaseNode[0])
			g_pSolidDraw->SetEntSnapStatus(pBaseNode[0]->handle);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Quit midway";
#else 
			throw "中途退出";
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select second angle's another design node in turn!");
#else 
		g_pPromptMsg->SetMsg("请依次选择第二根角钢另一设计节点!");
#endif
		while(g_pSolidSnap->SnapPoint(point)>0)
		{
			pBaseNode[1]=console.FromNodeHandle(point->ID);
			if(pBaseNode[1])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseNode[1]->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of the node！";
#else 
					throw "没有此节点的修改权限！";
#endif
				g_pSolidDraw->SetEntSnapStatus(pBaseNode[1]->handle);
				break;
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The selected node don't fit!");
			throw "Quit midway";
#else 
				AfxMessageBox("选择节点不合适!");
			throw "中途退出";
#endif
		}
		
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select second angle in turn!");
#else 
		g_pPromptMsg->SetMsg("请依次选择第二根角钢!");
#endif
		while(g_pSolidSnap->SnapLine(line)>0)
		{
			pBaseJg[1]=(CLDSLineAngle*)console.FromPartHandle(line.ID,CLS_LINEANGLE);
			if(pBaseJg[1]&&pBaseJg[1]->pStart==pBaseNode[1])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[1]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's start！";
#else 
					throw "没有此角钢始端的修改权限！";
#endif
				g_pSolidDraw->SetEntSnapStatus(pBaseJg[1]->handle);
				break;
			}
			else if(pBaseJg[1]&&pBaseJg[1]->pEnd==pBaseNode[1])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[1]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's end！";
#else 
					throw "没有此角钢终端的修改权限！";
#endif
				g_pSolidDraw->SetEntSnapStatus(pBaseJg[1]->handle);
				break;
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected angle don't fit,Quit midway!";
#else 
				throw "选择角钢不合适,中途退出!";
#endif
		}
		/*g_pPromptMsg->SetMsg("请依次选择第三根角钢!");
		while(g_pSolidSnap->SnapLine(line))
		{
		pBaseJg[2]=Ta.LineAngles.FromHandle(line->ID);
		if(pBaseJg[2]&&(
		pBaseJg[2]->pStart==pBaseNode[1]||pBaseJg[2]->pEnd==pBaseNode[1]))
		break;
		else
		AfxMessageBox("选择角钢不合适!");
		}
		if(pBaseJg[2])
		g_pSolidDraw->SetEntSnapStatus(pBaseJg[2]->handle);
		else
		throw "中途退出";*/
		for(i=0;i<2;i++)
		{
			if(pBaseJg[i]==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected datum angle is error,quit midway！";
#else 
				throw "基准角钢选择错误，设计失败中途退出！";
#endif
			if(m_eViewFlag==FRONT_VIEW||m_eViewFlag==BACK_VIEW)
				pMirJg=(CLDSLineAngle*)pBaseJg[i]->GetMirPart(MIRMSG(1),pBaseNode[i]->Position());	//X轴对称
			else
				pMirJg=(CLDSLineAngle*)pBaseJg[i]->GetMirPart(MIRMSG(2),pBaseNode[i]->Position());	//Y轴对称
			if(pMirJg==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Lack of symmetry angle,quit midway！";
#else 
				throw "缺少对称角钢，设计失败中途退出！";
#endif
			if(i==0)
			{
				designJdb.face2_jgset.append(pBaseJg[i]);
				designJdb.face2_jgset.append(pMirJg);
			}
			else
			{
				designJdb.face3_jgset.append(pBaseJg[i]);
				designJdb.face3_jgset.append(pMirJg);
			}
		}
		pCurPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pCurPlate->cfgword=pBaseNode[0]->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pCurPlate->designInfo.m_hBaseNode = pBaseNode[0]->handle;
		pCurPlate->jdb_style=1;	////之前折叠板赋值为3是错的会与钢管夹板混淆，现重新修正为1，并增加IsFoldPlate()函数 wjh-2016.1.09
		pCurPlate->face_N = 3;
		designJdb.SetViewFlag(m_eViewFlag);
		pCurPlate->ucs.axis_x=pBaseNode[1]->Position(true)-pBaseNode[0]->Position(true);//定义板坐标系X轴
		pCurPlate->ucs.origin=pBaseNode[0]->Position(true);
		
		if(designJdb.DesignFoldPlank(pCurPlate))
		{
			int iViewIndex=2;
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pBaseNode[0]->handle);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pBaseNode[1]->handle);
			g_pSolidDraw->ReleaseSnapStatus();
		}
		else//设计失败,删除系统中已生成的错误节点板
		{
			bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Design failed";
#else 
			throw "设计失败";
#endif
		}
		
		g_pSolidDraw->ReleaseSnapStatus();
		Ta.EndUndoListen();
	}
	catch(char *s)
	{
		//发生异常设计失败,删除系统中已生成的错误节点板
		if(pCurPlate)
			console.DeletePart(pCurPlate->handle);
		if(!bTerminateByUser)
			AfxMessageBox(s);	//提示异常错误信息
		g_pSolidDraw->ReleaseSnapStatus();
		ReleaseFunc();	//解开函数运行锁定状态
		Ta.EndUndoListen();
		return;
	}
	
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	ReleaseFunc();	//解开函数运行锁定状态
}

BOOL CLDSView::FinishDesignJoint(CLDSDbObject *pNodeAtom,CLDSDbObject *pJgAtom1,CLDSDbObject *pJgAtom2)
{
	BOOL bFinish=FALSE;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSNode *pSelNode;
	CLDSLineAngle *pMainJg[2];
	CDesignJoint designJoint;
	if(pNodeAtom==NULL||pJgAtom1==NULL||pJgAtom2==NULL)
		return FALSE;
	CUndoOperObject undo(&Ta);
	try
	{
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNodeAtom->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Without modify permission of the node！";
#else 
			throw "没有此节点的修改权限！";
#endif
		if(pNodeAtom->GetClassTypeId()!=CLS_NODE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Part type is error,the selected first part should be node!";
#else 
			throw "构件类型错误,你所选的第一个构件应该是节点!";
#endif
		pSelNode = (CLDSNode*)pNodeAtom;
		//CLDSNode::ATTACH_PART *pAttach = FindJointPosIndex(pSelNode);
		
		if(pJgAtom1->GetClassTypeId()!=CLS_LINEANGLE&&pJgAtom1->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Part type is error,the selected second part should be angle!";
#else 
			throw "构件类型错误,你所选的第二个构件应该是角钢!";
#endif
		pMainJg[1] = (CLDSLineAngle*)pJgAtom1;
		if(pJgAtom2->GetClassTypeId()!=CLS_LINEANGLE&&pJgAtom2->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Part type is error,the selected third part should be angle!";
#else 
			throw "构件类型错误,你所选的第三个构件应该是角钢!";
#endif
		pMainJg[0] = (CLDSLineAngle*)pJgAtom2;
		
		if(pMainJg[0]->pStart->handle==pSelNode->handle)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[0]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's start！";
#else 
				throw "没有此角钢始端的修改权限！";
#endif
			pMainJg[0]->feature = 0;//起点与当前节点相连接
		}
		else
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[0]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's end！";
#else 
				throw "没有此角钢终端的修改权限！";
#endif
			pMainJg[0]->feature = 1;//终点与当前节点相连接
		}
		if(pMainJg[1]->pStart->handle==pSelNode->handle)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[1]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's start！";
#else 
				throw "没有此角钢始端的修改权限！";
#endif
			pMainJg[1]->feature = 0;//起点与当前节点相连接
		}
		else
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[1]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's end！";
#else 
				throw "没有此角钢终端的修改权限！";
#endif
			pMainJg[1]->feature = 1;//终点与当前节点相连接
		}
		if(pMainJg[0]->GetClassTypeId()!=pMainJg[1]->GetClassTypeId())
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Two datum part's material are different,it isn't fit design joint here!";
#else 
			throw "两基准材类型不同，不适合在此处设计接头!";
#endif
		else if(pMainJg[0]->GetClassTypeId()==CLS_GROUPLINEANGLE
			&&((CLDSGroupLineAngle*)pMainJg[0])->group_style!=((CLDSGroupLineAngle*)pMainJg[1])->group_style)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Two datum part's material are different,it isn't fit design joint here!";
#else 
			throw "两基准材类型不同，不适合在此处设计接头!";
#endif
		CLDSLineAngle *pTemJg;
		if(  pMainJg[1]->GetThick()>pMainJg[0]->GetThick() ||
			(pMainJg[1]->GetThick()==pMainJg[0]->GetThick()&&
			pMainJg[1]->GetWidth()>pMainJg[0]->GetWidth()))
		{	//第一根基准角钢应该为肢厚较厚或肢宽较宽的那根
			pTemJg = pMainJg[0];
			pMainJg[0] = pMainJg[1];
			pMainJg[1] = pTemJg;
		}
		CJoint joint;
		joint.cfgword=Ta.GetDefaultCfgPartNo();
		joint.base_jg_handle_arr[0] = pMainJg[0]->handle;
		joint.base_jg_handle_arr[1] = pMainJg[1]->handle;
		joint.base_node_handle = pSelNode->handle;
		joint.SetBelongModel(console.GetActiveModel());
		joint.SetLayer(pMainJg[0]->layer());	//(theApp.env.layer);
		joint.iSeg=pSelNode->iSeg;
		bFinish=designJoint.CreateJoint(&joint);
		for(CLDSPart* pSubPart=joint.GetFirstPart();pSubPart;pSubPart=joint.GetNextPart())
		{
			pSubPart->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pSubPart->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	m_pDoc->SetModifiedFlag();
	if(bFinish)
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Design joint is success!");
#else 
		MessageBox("接头设计成功!");
#endif
#endif
	return bFinish;
}

BOOL CLDSView::FinishAddFillPlank(CLDSDbObject *pFirObj,
								  CLDSDbObject *pSecObj/*=NULL*/,
								  CLDSDbObject *pThirObj/*=NULL*/)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSGroupLineAngle *pGroupJg=NULL;
	CLDSNode *pStart=NULL,*pEnd=NULL;
	static CLayFillPlankDlg fill_plank_dlg;
	int i,j;
	BOOL bTerminateByUser=FALSE;
	Ta.BeginUndoListen();
	try
	{
		f3dPoint start,end,vec,vert_vec;
		if(pFirObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			pGroupJg=(CLDSGroupLineAngle*)pFirObj;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pGroupJg->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Not have modify permission of the combined angle！";
#else 
				throw "没有此组合角钢的基本修改权限！";
#endif
			pStart=pGroupJg->pStart;
			pEnd=pGroupJg->pEnd;
			vec=pGroupJg->End()-pGroupJg->Start();
			normalize(vec);
		}
		else if(pFirObj->GetClassTypeId()==CLS_NODE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pSecObj==NULL||pThirObj==NULL)
				throw "The number of selected parts is not enough!Layout filler plate failure!";
			else if(pSecObj->GetClassTypeId()!=CLS_GROUPLINEANGLE||pThirObj->GetClassTypeId()!=CLS_NODE)
				throw "The selected part's type is error!Layout filler plate failure!";
			else if(pSecObj->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				throw "The selected angle isn't combined angle that no need to layout filler plate!";
			pGroupJg=(CLDSGroupLineAngle*)pSecObj;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pGroupJg->dwPermission))
				throw "Not have modify permission of the combined angle！";
#else 
			if(pSecObj==NULL||pThirObj==NULL)
				throw "选择构件不足!填板布置失败!";
			else if(pSecObj->GetClassTypeId()!=CLS_GROUPLINEANGLE||pThirObj->GetClassTypeId()!=CLS_NODE)
				throw "选择构件类型不对!填板布置失败!";
			else if(pSecObj->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				throw "选择角钢不是组合角钢不需布置填板!";
			pGroupJg=(CLDSGroupLineAngle*)pSecObj;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pGroupJg->dwPermission))
				throw "没有此组合角钢的基本修改权限！";
#endif
			pStart=(CLDSNode*)pFirObj;
			pEnd=(CLDSNode*)pThirObj;
			start=pGroupJg->GetDatumPosBer(pStart);
			end=pGroupJg->GetDatumPosBer(pEnd);
			vec=end-start;
			normalize(vec);
		}
		fill_plank_dlg.m_pGroupAngle=pGroupJg;
#ifdef AFX_TARG_ENU_ENGLISH
		if(fill_plank_dlg.DoModal()!=IDOK)
		{
			bTerminateByUser=TRUE;
			throw "Quit midway!Layout filler plate failure!";
		}
		if(fill_plank_dlg.m_nPlankNum<=0)
			throw "The specified filler plate's number is 0,no need to add!";
#else 
		if(fill_plank_dlg.DoModal()!=IDOK)
		{
			bTerminateByUser=TRUE;
			throw "中途退出!填板布置失败!";
		}
		if(fill_plank_dlg.m_nPlankNum<=0)
			throw "指定填板数为零,不需添加!";
#endif
		int nSecType = 0;
		if(fill_plank_dlg.m_bDifferentType)
			nSecType = !fill_plank_dlg.m_iLsLayOutStyle;
		else
			nSecType = fill_plank_dlg.m_iLsLayOutStyle;
		if(fill_plank_dlg.m_nThick<=0)
			fill_plank_dlg.m_nThick=ftoi(pGroupJg->jg_space);
		//组合角钢角钢间隙的一半，生成螺栓时使用，保证螺栓在角钢准线上 wht 11-07-01
		double fHalfGroupJgSpace=0.5*pGroupJg->jg_space;	
		CLDSLineAngle *pBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->m_hDatumSonAngle,CLS_LINEANGLE);
		if(pBaseJg==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The combined angle lacks of datum angle!Layout filler plate failure!";
#else 
			throw "组合角钢缺少基准角钢!填板布置失败!";
#endif
		
		CLDSBolt ls(console.GetActiveModel());	//螺栓
		int ls_d=atoi(fill_plank_dlg.m_sLsGuiGe);
		ls.set_d(ls_d);
		JGZJ jgzj_x,jgzj_y;
		getjgzj(jgzj_x,pGroupJg->GetWidth());
		jgzj_y=jgzj_x;
		if(pGroupJg->m_bEnableTeG)
		{
			jgzj_x=pGroupJg->xWingXZhunJu;
			jgzj_y=pGroupJg->xWingYZhunJu;
		}
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,ls_d);
		int plank_len=GetLsGroupLen(ls.get_d(),fill_plank_dlg.m_nLsNum,fill_plank_dlg.m_iLsRows+1);
		double unit_scale=1.0/(fill_plank_dlg.m_nPlankNum+1);
		for(i=0;i<fill_plank_dlg.m_nPlankNum;i++)
		{
			f3dPoint ber_pick,ls_pos;
			CLDSPlate *pPlank=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pPlank->cfgword=pBaseJg->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
			pPlank->Thick=fill_plank_dlg.m_nThick;
			pPlank->SetPartNo(fill_plank_dlg.m_sPartNo.GetBuffer());
			pPlank->iSeg=SEGI(fill_plank_dlg.m_sSegI.GetBuffer());
			pPlank->cMaterial=CSteelMatLibrary::RecordAt(fill_plank_dlg.m_iPlateMaterial).cBriefMark;
			pPlank->designInfo.m_bEnableFlexibleDesign=TRUE; //启用柔性设计
			pPlank->ucs.axis_y.Set();	//不清零后续重新设计时不重置Y轴坐标系,最终导致填板走向与组合角钢不一致 wjh-2016.4.13
			pPlank->designInfo.iProfileStyle0123=2;			 //包络外形
			//设置钢板柔性化参数
			pPlank->designInfo.m_hBasePart=pGroupJg->handle;
			pPlank->designInfo.norm.norm_style=1;	//角钢肢法线方向
			pPlank->designInfo.norm.hVicePart=pBaseJg->handle;
			pPlank->designInfo.norm.direction=0;
			//填板位置参数 wht 11-01-23
			pPlank->m_fNormOffset=-(0.5*fill_plank_dlg.m_nThick);
			pPlank->designInfo.origin.datum_pos_style=10;	//两节点间的比例等分点
			pPlank->designInfo.origin.des_para.SCALE_NODE.hLinePart=pGroupJg->handle;
			pPlank->designInfo.origin.des_para.SCALE_NODE.hStartNode=pStart->handle;
			pPlank->designInfo.origin.des_para.SCALE_NODE.hEndNode=pEnd->handle;
			pPlank->designInfo.origin.des_para.SCALE_NODE.start_offset_dist=fill_plank_dlg.m_fStartOffset;
			pPlank->designInfo.origin.des_para.SCALE_NODE.end_offset_dist=fill_plank_dlg.m_fEndOffset;
			pPlank->designInfo.origin.des_para.SCALE_NODE.offset_scale=unit_scale*(i+1);
			pPlank->designInfo.origin.des_para.SCALE_NODE.offset_dist=0;
			//螺栓法线设计参数
			ls.iSeg=SEGI(fill_plank_dlg.m_sSegI.GetBuffer());	//指定螺栓段号 wht 11-07-01
			ls.des_work_norm.norm_style=4;	//基准构件上的相对法线
			ls.des_work_norm.hVicePart=pPlank->handle;
			ls.des_work_norm.nearVector.Set(0,0,1);
			if(pGroupJg->group_style==0)	//对角型组合
			{
				CLDSLineAngle *pOtherJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				JGZJ jgzj;
				if(fill_plank_dlg.m_iWing==0)
				{	//初始板在X肢上
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=0;
					else
						pPlank->designInfo.norm.norm_wing=1;
				}
				else
				{	//初始板在Y肢上
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=1;
					else
						pPlank->designInfo.norm.norm_wing=0;
				}
				pPlank->DesignSetupUcs();	//计算钢板装配坐标系
				if(fabs(pPlank->ucs.axis_x*pBaseJg->GetWingVecX())>EPS_COS2)
				{
					vert_vec=pBaseJg->GetWingVecX();	//螺栓在X肢上
					jgzj=jgzj_x;
				}
				else
				{
					vert_vec=pBaseJg->GetWingVecY();	//螺栓在Y肢上
					jgzj=jgzj_y;
				}
				ls.EmptyL0DesignPara();	//清空螺栓设计参数
				ls.AddL0Thick(pPlank->handle,TRUE);
				ls.AddL0Thick(pBaseJg->handle,TRUE);
				if(!ls.CalGuigeAuto())
				{
					char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
					sprintf(ss,"找不到合适的螺栓规格M%dX%.0f.",ls.get_d(),ls.L0);
#endif
					throw ss;
				}
				for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
				{
					//基准角钢上填板螺栓
					CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//单排排列
						ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)
							   +vert_vec*(fHalfGroupJgSpace+jgzj.g);
					else	//双排排列
					{
						if(fill_plank_dlg.m_iLsLayOutStyle==0)	//靠近楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*(fHalfGroupJgSpace+jgzj.g1);
							else		//奇数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*(fHalfGroupJgSpace+jgzj.g2);
						}
						else	//远离楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*(fHalfGroupJgSpace+jgzj.g2);
							else		//奇数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*(fHalfGroupJgSpace+jgzj.g1);
						}
					}
					pLs->des_base_pos.norm_offset.AddThick(-pBaseJg->handle,TRUE);
					pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank->GetThick()*0.5));
					ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
					pLs->ucs.origin=ls_pos;
					pBaseJg->AppendMidLsRef(pLs->GetLsRef());
					pPlank->AppendLsRef(pLs->GetLsRef());
					//螺栓位置参数 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//构件上的相对位置
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//到位螺栓
					pLs->correct_worknorm();
					pLs->correct_pos();
					pLs->CalGuigeAuto();
					pLs->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
					
					//非基准角钢上填板螺栓
					if(pOtherJg==NULL)
						continue;
					//另一单角钢楞线上的基准点
					SnapPerp(&ber_pick,pOtherJg->Start(),pOtherJg->End(),pPlank->ucs.origin);
					pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pOtherJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//单排排列
						ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
					else	//双排排列
					{
						if(nSecType==0)	//靠近楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
							else		//奇数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
						}
						else	//远离楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							else		//奇数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
						}
					}
					pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank->GetThick()*0.5));
					ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
					pLs->ucs.origin=ls_pos;
					pOtherJg->AppendMidLsRef(pLs->GetLsRef());
					pPlank->AppendLsRef(pLs->GetLsRef());
					//螺栓位置参数 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//构件上的相对位置
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//到位螺栓
					pLs->correct_worknorm();
					pLs->correct_pos();
					pLs->CalGuigeAuto();
					pLs->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
				}
				pPlank->CalStdProfile();	//计算钢板外形
				pPlank->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlank->GetSolidPartObject());

				if(fill_plank_dlg.m_iType==1)
				{
					CLDSPlate *pPlank2=NULL;
					if(fill_plank_dlg.m_iType==1)
					{
						pPlank2=(CLDSPlate*)console.AppendPart(CLS_PLATE);
						pPlank2->CopyProperty(pPlank);
						//设置钢板柔性化参数
						pPlank2->designInfo.m_hBasePart=pGroupJg->handle;
						pPlank2->designInfo.norm.norm_style=1;	//角钢肢法线方向
						pPlank2->designInfo.norm.hVicePart=pBaseJg->handle;
						pPlank2->designInfo.norm.direction=0;
						//钢板位置参数 
						pPlank2->designInfo=pPlank->designInfo;
					}
					if(fill_plank_dlg.m_iWing==1)
					{	//初始板在X肢上
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=0;
						else
							pPlank2->designInfo.norm.norm_wing=1;
					}
					else
					{	//初始板在Y肢上
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=1;
						else
							pPlank2->designInfo.norm.norm_wing=0;
					}
					pPlank2->DesignSetupUcs();	//计算钢板装配坐标系
					if(fabs(pPlank2->ucs.axis_y*pBaseJg->GetWingVecX())>EPS_COS2)
					{
						vert_vec=pBaseJg->GetWingVecX();	//螺栓在X肢上
						jgzj=jgzj_x;
					}
					else
					{
						vert_vec=pBaseJg->GetWingVecY();	//螺栓在Y肢上
						jgzj=jgzj_y;
					}
					//螺栓法线设计参数
					ls.des_work_norm.hVicePart=pPlank2->handle;
					ls.EmptyL0DesignPara();	//清空螺栓设计参数
					ls.AddL0Thick(pPlank2->handle,TRUE);
					ls.AddL0Thick(pBaseJg->handle,TRUE);
					if(!ls.CalGuigeAuto())
					{
						char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
						sprintf(ss,"找不到合适的螺栓规格M%dX%.0f.",ls.get_d(),ls.L0);
#endif
						throw ss;
					}
					for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
					{
						//基准角钢上填板螺栓
						CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pLs->des_work_norm=ls.des_work_norm;
						pLs->set_norm(pPlank2->ucs.axis_z);
						if(fill_plank_dlg.m_iLsRows==0)	//单排排列
							ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj.g;
						else	//双排排列
						{
							if(fill_plank_dlg.m_iLsLayOutStyle==0)	//靠近楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
								else		//奇数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
							}
							else	//远离楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
								else		//奇数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
							}
						}
						pLs->des_base_pos.norm_offset.AddThick(-pBaseJg->handle,TRUE);
						pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank2->GetThick()*0.5));
						ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
						pLs->ucs.origin=ls_pos;
						pBaseJg->AppendMidLsRef(pLs->GetLsRef());
						pPlank2->AppendLsRef(pLs->GetLsRef());
						//螺栓位置参数 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//杆件上的相对位置
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						//到位螺栓
						pLs->correct_worknorm();
						pLs->correct_pos();
						pLs->CalGuigeAuto();
						pLs->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
						
						//非基准角钢上填板螺栓
						if(pOtherJg==NULL)
							continue;
						pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pOtherJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pLs->des_work_norm=ls.des_work_norm;
						pLs->set_norm(pPlank2->ucs.axis_z);
						if(fill_plank_dlg.m_iLsRows==0)	//单排排列
							ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
						else	//双排排列
						{
							if(nSecType==0)	//靠近楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
								else		//奇数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							}
							else	//远离楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
								else		//奇数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
							}
						}
						pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank2->GetThick()*0.5));
						ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
						pLs->ucs.origin=ls_pos;
						pOtherJg->AppendMidLsRef(pLs->GetLsRef());
						pPlank2->AppendLsRef(pLs->GetLsRef());
						//螺栓位置参数 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//杆件上的相对位置
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						//到位螺栓
						pLs->correct_worknorm();
						pLs->correct_pos();
						pLs->CalGuigeAuto();
						pLs->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
					}
					pPlank2->CalStdProfile();	//计算钢板外形
					//
					CLDSPlate *pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
					long hNew=pNewPlate->handle;	//保存钢板句柄
					pPlank->CloneTo(*pNewPlate);	//克隆钢板 wht 11-01-07
					pNewPlate->handle=hNew;			//更新钢板句柄
					pNewPlate->relativeObjs.Empty();//清空关联构件列表
					pNewPlate->EmptyLsRef();		//清空螺栓引用
					//复制螺栓引用
					for(CLsRef *pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
						pNewPlate->AppendLsRef(*pLsRef);
					pNewPlate->SetModified();
					pNewPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pNewPlate->GetSolidPartObject());
					f3dPoint face_pos,face_norm;
					face_norm=pPlank2->ucs.axis_z;
					face_pos=pPlank2->ucs.origin;
					CutPlateByPlane(pPlank,face_pos,face_norm,fill_plank_dlg.m_fDistance);
					pPlank->m_hPartWeldParent = pPlank2->handle;
					face_pos+=face_norm*pNewPlate->GetThick();
					face_norm*=-1.0;
					CutPlateByPlane(pNewPlate,face_pos,face_norm,fill_plank_dlg.m_fDistance);
					pNewPlate->m_hPartWeldParent = pPlank2->handle;
				}
			}
			else if(pGroupJg->group_style==1)//T字型组合
			{
				CLDSLineAngle *pOtherJg=NULL;
				if(pGroupJg->m_hDatumSonAngle==pGroupJg->son_jg_h[0])
					pOtherJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				else
					pOtherJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
				if(pOtherJg==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Can't find combined angle's another not datum child angle !";
#else 
					throw "找不到组合角钢的另一非基准子角钢!";
#endif
				
				vert_vec=pBaseJg->GetWingVecX();	//初始板在Y肢上
				pPlank->designInfo.norm.norm_wing=0;
				pPlank->DesignSetupUcs();	//计算钢板装配坐标系
				ls.EmptyL0DesignPara();	//清空螺栓设计参数
				ls.AddL0Thick(pPlank->handle,TRUE);
				ls.AddL0Thick(pBaseJg->handle,TRUE);
				ls.AddL0Thick(pOtherJg->handle,TRUE);
				if(!ls.CalGuigeAuto())
				{
					char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
					sprintf(ss,"找不到合适的螺栓规格M%dX%.0f.",ls.get_d(),ls.L0);
#endif
					throw ss;
				}
				for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
				{
					//基准角钢上填板螺栓
					CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//单排排列
						ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj_x.g;
					else	//双排排列
					{
						if(fill_plank_dlg.m_iLsLayOutStyle==0)	//靠近楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj_x.g1;
							else		//奇数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj_x.g2;
						}
						else	//远离楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj_x.g2;
							else		//奇数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj_x.g1;
						}
					}
					pLs->des_base_pos.norm_offset.AddThick(-pBaseJg->handle,TRUE);
					pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank->GetThick()*0.5));
					ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
					pLs->ucs.origin=ls_pos;
					pBaseJg->AppendMidLsRef(pLs->GetLsRef());
					if(pOtherJg)
						pOtherJg->AppendMidLsRef(pLs->GetLsRef());
					pPlank->AppendLsRef(pLs->GetLsRef());
					//螺栓位置参数 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//杆件上的相对位置
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//到位螺栓
					pLs->correct_worknorm();
					pLs->correct_pos();
					pLs->CalGuigeAuto();
					pLs->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
				}
				pPlank->CalStdProfile();
				pPlank->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlank->GetSolidPartObject());
			}
			else if(pGroupJg->group_style==2)//十字型组合
			{
				CLDSLineAngle *pOtherJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				CLDSLineAngle *pOtherJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
				CLDSLineAngle *pOtherJg3=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
				JGZJ jgzj;
				if(fill_plank_dlg.m_iWing==0)
				{	//初始板在X肢上
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=0;
					else
						pPlank->designInfo.norm.norm_wing=1;
				}
				else
				{	//初始板在Y肢上
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=1;
					else
						pPlank->designInfo.norm.norm_wing=0;
				}
				pPlank->DesignSetupUcs();	//设计钢板装配坐标系
				if(fabs(pPlank->ucs.axis_x*pBaseJg->GetWingVecX())>EPS_COS2)
				{
					vert_vec=pBaseJg->GetWingVecX();	//螺栓在X肢上
					jgzj=jgzj_x;
				}
				else
				{
					vert_vec=pBaseJg->GetWingVecY();	//螺栓在Y肢上
					jgzj=jgzj_y;
				}
				ls.EmptyL0DesignPara();	//清空螺栓设计参数
				ls.AddL0Thick(pPlank->handle,TRUE);
				ls.AddL0Thick(pBaseJg->handle,TRUE);
				ls.AddL0Thick(ftol(pBaseJg->GetThick()),FALSE);
				if(!ls.CalGuigeAuto())
				{
					char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
					sprintf(ss,"找不到合适的螺栓规格M%dX%.0f.",ls.get_d(),ls.L0);
#endif
					throw ss;
				}
				for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
				{
					//基准角钢上填板螺栓
					CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//单排排列
						ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj.g;
					else	//双排排列
					{
						if(fill_plank_dlg.m_iLsLayOutStyle==0)	//靠近楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
							else		//奇数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
						}
						else	//远离楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
							else		//奇数个螺栓
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
						}
					}
					pLs->des_base_pos.norm_offset.AddThick(-pBaseJg->handle,TRUE);
					pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank->GetThick()*0.5));
					ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
					pLs->ucs.origin=ls_pos;
					pBaseJg->AppendMidLsRef(pLs->GetLsRef());
					if(pOtherJg1&&pOtherJg1->IsLsInWing(pLs))
						pOtherJg1->AppendMidLsRef(pLs->GetLsRef());
					if(pOtherJg3&&pOtherJg3->IsLsInWing(pLs))
						pOtherJg3->AppendMidLsRef(pLs->GetLsRef());
					pPlank->AppendLsRef(pLs->GetLsRef());
					//螺栓位置参数 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//杆件上的相对位置
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					
					//非基准角钢上填板螺栓
					if(pOtherJg2==NULL)
						continue;
					//另一单角钢楞线上的基准点
					SnapPerp(&ber_pick,pOtherJg2->Start(),pOtherJg2->End(),pPlank->ucs.origin);
					pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//单排排列
						ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
					else	//双排排列
					{
						if(nSecType==0)	//靠近楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
							else		//奇数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
						}
						else	//远离楞线一侧优先
						{
							if(j%2==0)	//偶数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							else		//奇数个螺栓
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
						}
					}
					sprintf(pLs->des_base_pos.norm_offset.key_str,"-0X%X",pBaseJg->handle);
					pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank->GetThick()*0.5));
					ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
					pLs->ucs.origin=ls_pos;
					pOtherJg2->AppendMidLsRef(pLs->GetLsRef());
					if(pOtherJg1&&pOtherJg1->IsLsInWing(pLs))
						pOtherJg1->AppendMidLsRef(pLs->GetLsRef());
					if(pOtherJg3&&pOtherJg3->IsLsInWing(pLs))
						pOtherJg3->AppendMidLsRef(pLs->GetLsRef());
					pPlank->AppendLsRef(pLs->GetLsRef());
					//螺栓位置参数 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//杆件上的相对位置
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//到位螺栓
					pLs->correct_worknorm();
					pLs->correct_pos();
					pLs->CalGuigeAuto();
					pLs->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
				}
				pPlank->CalStdProfile();
				pPlank->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlank->GetSolidPartObject());

				if(fill_plank_dlg.m_iType==1)
				{
					CLDSPlate *pPlank2=(CLDSPlate*)console.AppendPart(CLS_PLATE);
					pPlank2->CopyProperty(pPlank);
					//设置钢板柔性化参数
					pPlank2->designInfo.m_hBasePart=pGroupJg->handle;
					pPlank2->designInfo.norm.norm_style=1;	//角钢肢法线方向
					pPlank2->designInfo.norm.hVicePart=pBaseJg->handle;
					pPlank2->designInfo.norm.direction=0;
					//钢板位置参数 
					pPlank2->designInfo=pPlank->designInfo;
					if(fill_plank_dlg.m_iWing==1)
					{	//初始板在X肢上
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=0;
						else
							pPlank2->designInfo.norm.norm_wing=1;
					}
					else
					{	//初始板在Y肢上
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=1;
						else
							pPlank2->designInfo.norm.norm_wing=0;
					}
					pPlank2->DesignSetupUcs();	//钢板装配坐标系
					if(fabs(pPlank2->ucs.axis_y*pBaseJg->GetWingVecX())>EPS_COS2)
					{
						vert_vec=pBaseJg->GetWingVecX();	//螺栓在X肢上
						jgzj=jgzj_x;
					}
					else
					{
						vert_vec=pBaseJg->GetWingVecY();	//螺栓在Y肢上
						jgzj=jgzj_y;
					}
					ls.des_work_norm.hVicePart=pPlank2->handle;
					ls.EmptyL0DesignPara();	//清空螺栓设计参数
					ls.AddL0Thick(pPlank2->handle,TRUE);
					ls.AddL0Thick(pBaseJg->handle,TRUE);
					ls.AddL0Thick(ftol(pBaseJg->GetThick()),FALSE);
					if(!ls.CalGuigeAuto())
					{
						char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
						sprintf(ss,"找不到合适的螺栓规格M%dX%.0f.",ls.get_d(),ls.L0);
#endif
						throw ss;
					}
					for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
					{
						//基准角钢上填板螺栓
						CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pLs->des_work_norm=ls.des_work_norm;
						pLs->set_norm(pPlank2->ucs.axis_z);
						if(fill_plank_dlg.m_iLsRows==0)	//单排排列
							ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj.g;
						else	//双排排列
						{
							if(fill_plank_dlg.m_iLsLayOutStyle==0)	//靠近楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
								else		//奇数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
							}
							else	//远离楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
								else		//奇数个螺栓
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
							}
						}
						pLs->des_base_pos.norm_offset.AddThick(-pBaseJg->handle,TRUE);
						pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank2->GetThick()*0.5));
						ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
						pLs->ucs.origin=ls_pos;
						pBaseJg->AppendMidLsRef(pLs->GetLsRef());
						if(pOtherJg1&&pOtherJg1->IsLsInWing(pLs))
							pOtherJg1->AppendMidLsRef(pLs->GetLsRef());
						if(pOtherJg3&&pOtherJg3->IsLsInWing(pLs))
							pOtherJg3->AppendMidLsRef(pLs->GetLsRef());
						pPlank2->AppendLsRef(pLs->GetLsRef());
						//螺栓位置参数 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//杆件上的相对位置
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						
						//非基准角钢上填板螺栓
						if(pOtherJg2==NULL)
							continue;
						pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pLs->des_work_norm=ls.des_work_norm;
						if(fill_plank_dlg.m_iLsRows==0)	//单排排列
							ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
						else	//双排排列
						{
							if(nSecType==0)	//靠近楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
								else		//奇数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							}
							else	//远离楞线一侧优先
							{
								if(j%2==0)	//偶数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
								else		//奇数个螺栓
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
							}
						}
						sprintf(pLs->des_base_pos.norm_offset.key_str,"-0X%X",pBaseJg->handle);
						pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank2->GetThick()*0.5));
						ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
						pLs->ucs.origin=ls_pos;
						pOtherJg2->AppendMidLsRef(pLs->GetLsRef());
						if(pOtherJg1&&pOtherJg1->IsLsInWing(pLs))
							pOtherJg1->AppendMidLsRef(pLs->GetLsRef());
						if(pOtherJg3&&pOtherJg3->IsLsInWing(pLs))
							pOtherJg3->AppendMidLsRef(pLs->GetLsRef());
						pPlank2->AppendLsRef(pLs->GetLsRef());
						//螺栓位置参数 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//杆件上的相对位置
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z坐标归零
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						//到位螺栓
						pLs->correct_worknorm();
						pLs->correct_pos();
						pLs->CalGuigeAuto();
						pLs->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
					}
					pPlank2->CalStdProfile();
					pPlank2->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pPlank->GetSolidPartObject());

					CLDSPlate *pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
					long hNew=pNewPlate->handle;	//保存钢板句柄
					pPlank->CloneTo(*pNewPlate);	//克隆钢板 wht 11-01-07
					pNewPlate->handle=hNew;			//更新钢板句柄
					pNewPlate->relativeObjs.Empty();//清空关联构件列表
					pNewPlate->EmptyLsRef();		//清空螺栓引用
					//复制螺栓引用
					for(CLsRef *pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
						pNewPlate->AppendLsRef(*pLsRef);
					pNewPlate->SetModified();
					pNewPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pNewPlate->GetSolidPartObject());
					f3dPoint face_pos,face_norm;
					face_norm=pPlank2->ucs.axis_z;
					face_pos=pPlank2->ucs.origin;
					CutPlateByPlane(pPlank,face_pos,face_norm,fill_plank_dlg.m_fDistance);
					pPlank->m_hPartWeldParent = pPlank2->handle;
					face_pos+=face_norm*pNewPlate->GetThick();
					face_norm*=-1.0;
					CutPlateByPlane(pNewPlate,face_pos,face_norm,fill_plank_dlg.m_fDistance);
					pNewPlate->m_hPartWeldParent = pPlank2->handle;
				}
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The style of combine angle is error!Layout filler plate failure!";
#else 
				throw "出现了错误的角钢组合型式!填板布置失败!";
#endif
			pPlank->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pPlank->GetSolidPartObject());
		}
	}
	catch(char* sError)
	{
		Ta.EndUndoListen();
		if(!bTerminateByUser)
			AfxMessageBox(sError);
		g_pSolidDraw->ReleaseSnapStatus();
		return FALSE;
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
#endif
	return TRUE;
}

//-----------VVV---布置角钢螺栓--------VVV--------
//布置角钢螺栓 一、布置单螺栓连接  二、布置单角钢螺栓
//一、布置单角钢螺栓
void CLDSView::OnLayJgEndLs() 
{
	m_nPrevCommandID=ID_LAY_JG_END_LS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to layout angle's bolts";
#else 
	m_sPrevCommandName="重复布置角钢螺栓";
#endif
#ifndef __LDS_
	Command("LayoutAngleBolts");
#else
	Command("AngleBolts");	//临时LayoutAngleBolts命令，看是否可完全取代 wjh-2019.8.6
#endif
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView::AngleBolts">
void _LocalLayoutSingleRodBolts(CLDSLinePart* pCurRod, BYTE ciCurWorkWing, RODSECTION sect, CLDSLineAngle* pCrossAngle=NULL);
int CLDSView::AngleBolts()
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	IDrawing* pDrawing=g_pSolidSet->GetSolidBuddyDrawing();
	if(pDrawing==NULL)
		return FALSE;
	g_pSolidDraw->ReleaseSnapStatus();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	DWORD dwhObj=0,dwExportFlag=0;
	//捕捉待布置螺栓的杆件心线
	CLDSLinePart* pCurRod=NULL;
	HIBERID hiberid;
	f3dLine briefline;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("AngleBolt Please select angle bolt line:","");
#else 
	pCmdLine->FillCmdLine("AngleBolt 请选择需要布置螺栓的杆件心线:","");
#endif
	CSnapTypeVerify verifier(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbLine));
	verifier.SetEntsSelectLevel(2);	//允许选中角钢肢心线
	RODSECTION section;
	section.ciSectType=-1;
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag,pDrawing);
		pCurRod=(CLDSLinePart*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE,CLS_LINEFLAT,CLS_LINESLOT);
		if(pCurRod)
		{
			if(pCurRod->pStart&&pCurRod->pEnd)
			{
				briefline.startPt=pCurRod->pStart->Position(false);
				briefline.endPt  =pCurRod->pEnd->Position(false);
			}
			IDbEntity* pAxisLine=obj.idEnt>0?pDrawing->GetDbEntity(obj.idEnt):NULL;
			if(pAxisLine&&pAxisLine->GetDbEntType()==IDbEntity::DbLine)
			{
				hiberid=pAxisLine->GetHiberId();
				POINT point;
				GetCursorPos(&point);
				ScreenToClient(&point);
				IDbLine *pLine=(IDbLine*)pAxisLine;
				GECS ocs;
				pDrawing->GetOCS(ocs);
				f3dLine axisline(ocs.TransPToCS(pLine->StartPosition()), ocs.TransPToCS(pLine->EndPosition()));
				GEPOINT lineStdVec=axisline.endPt-axisline.startPt;
				GEPOINT xScrStart=g_pSolidOper->TransPToScr(axisline.startPt);
				GEPOINT xScrEnd=g_pSolidOper->TransPToScr(axisline.endPt);
				GEPOINT lineStdVec2(xScrEnd.x-xScrStart.x,xScrEnd.y-xScrStart.y);
				double scaleOfS2E=0,length=0,lengthProj=lineStdVec2.mod();
				if(lengthProj>EPS)
				{
					lineStdVec2 /= lengthProj;
					scaleOfS2E=GEPOINT(point.x-xScrStart.x,point.y-xScrStart.y)*lineStdVec2;
					scaleOfS2E /= lengthProj;
				}
				if((length=lineStdVec.mod())>EPS)
				{
					lineStdVec/=length;
					double sectlen=max(length*0.25,100);
					if(sectlen>length/2)
						sectlen=length/2;
					else if(pCurRod->pStart==NULL||pCurRod->pEnd==NULL)
						sectlen=length/2;
					double lendist=scaleOfS2E*length;
					if(lendist<sectlen)
						section.ciSectType=1;
					else if(lendist>length-sectlen)
						section.ciSectType=2;
				}
			}
			break;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("AngleBolt Nonselect valid angle. Please select angle bolt line:","");
#else 
		pCmdLine->FillCmdLine("AngleBolt 没有选中合适的杆件心线，请重新选择需要布置螺栓的杆件心线:","");
#endif
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pCurRod->handle));
	//选择节点布置截面定位螺栓或选择另一根杆件肢心线布置交叉点螺栓
	CLDSNode* pBaseNode=NULL;
	CLDSLinePart* pOtherRod=NULL;
	if(pCurRod->GetClassTypeId()==CLS_LINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("AngleBolt Please select connected node which angle's bolts attached，or select other angle bolt line:","");
#else
		pCmdLine->FillCmdLine("AngleBolt 请选择杆件上布置螺栓的定位基准(节点、相交角钢)，或按键<回车|空格>跳出:","");
#endif
		
		verifier.ClearSnapFlag();
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE)|GetSingleWord(SELECTINDEX_LINEANGLE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("AngleBolt Please select connected node which angle's bolts attached，or select other angle bolt line:","");
#else
		pCmdLine->FillCmdLine("AngleBolt 请选择杆件上布置螺栓的定位截面节点，或按键<回车|空格>跳出:","");
#endif

		verifier.ClearSnapFlag();
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
	}
	{//此作用域控制节点显示的生命周期
		CDisplayNodeAtFrontLife showPoint;
		showPoint.DisplayNodeAtFront();
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if(pCurRod->handle==obj.hRelaObj)
				continue;	//不能重复选择同一根角钢
			if(obj.hRelaObj==0 && obj.ciTriggerType == SELOBJ::TRIGGER_KEYRETURN)
				break;	//按回车或空格跳出
			if((pBaseNode=console.FromNodeHandle(obj.hRelaObj))==NULL)
				pOtherRod=(CLDSLinePart*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
			if(pBaseNode)
			{ 
				if(briefline.PtInLine(pBaseNode->Position(false))!=0)
					break;
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("AngleBolt Nonselect valid angle. Please select angle bolt line:","");
#else 
				pCmdLine->FillCmdLine("AngleBolt 没有选中合适的节点，请重新选择节点，或相交的角钢:","");
#endif
			}
			else if(pOtherRod)
			{
				if(pOtherRod->pStart&&pOtherRod->pEnd)
					break;
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("AngleBolt Nonselect valid angle. Please select angle bolt line:","");
#else 
				pCmdLine->FillCmdLine("AngleBolt 没有选中有效的交叉角钢，请重新选择节点，或相交的角钢:","");
#endif			
			}
		}
		if(pBaseNode)
		{
			g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
			pCmdLine->FinishCmdLine(CXhChar16("0X%X",pBaseNode->handle));
		}
		else if(pOtherRod)
		{
			g_pSolidDraw->SetEntSnapStatus(pOtherRod->handle);
			pCmdLine->FinishCmdLine(CXhChar16("0X%X",pOtherRod->handle));
		}
		else
			pCmdLine->FinishCmdLine("<回车>");
	}
	//布置螺栓
	CUndoOperObject undo(&Ta,true);
	if(pCurRod)
	{
		BYTE ciSelWorkWing=0;	//当前选中的角钢心线肢，'X','Y'
		if(pCurRod->IsAngle())
		{
			if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==0)
				ciSelWorkWing=0;	//当前选中的是X肢心线
			else if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==1)
				ciSelWorkWing=1;	//当前选中的是Y肢心线
		}
		else if(pCurRod->GetClassTypeId()==CLS_LINESLOT)
		{
			if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==0)
				ciSelWorkWing=0;	//槽面
			else if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==1)
				ciSelWorkWing=1;	//X+肢
			else if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==2)
				ciSelWorkWing=2;	//X-肢
		}
		else if(pCurRod->GetClassTypeId()==CLS_LINEFLAT)
			ciSelWorkWing=0;
		CLDSLineAngle* pOtherAngle=NULL;
		if(pOtherRod && pOtherRod->IsAngle())
			pOtherAngle=(CLDSLineAngle*)pOtherRod;
		if(pBaseNode!=NULL||section.ciSectType<0)
		{
			section.pSectNode=pBaseNode;
			section.ciSectType=0;
		}
		_LocalLayoutSingleRodBolts((CLDSLineAngle*)pCurRod,ciSelWorkWing,section,pOtherAngle);
	}
	//
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();	
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 0;
}
//</DEVELOP_PROCESS_MARK>
int CLDSView::LayoutAngleBolts()
{
	CLDSLinePart *pLinePart=NULL;
	CLDSNode *pBaseNode=NULL;
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("LayoutAngleBolts Please select angle(slot angle or flat angle)which will be layout bolts:","");
#else 
	pCmdLine->FillCmdLine("LayoutAngleBolts 请选择需要布置螺栓的角钢(槽钢或扁钢):","");
#endif
	CCmdLockObject cmdlock(this);
	long *id_arr,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n==1)
		pLinePart=(CLDSLinePart*)console.FromPartHandle(id_arr[0],CLS_LINEPART);
	
	if(pLinePart==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		Invalidate(FALSE);
	}
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE)|
		GetSingleWord(SELECTINDEX_LINESLOT)|GetSingleWord(SELECTINDEX_LINEFLAT));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
	f3dLine line;
	try{
		if(pLinePart==NULL)
		{
			while(1)
			{
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					//布置完螺栓退出命令时，切换到实体显示模式
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					Invalidate(FALSE);
					return 0;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;		
				if(dwhObj>0x20)
				{
					pLinePart=(CLDSLinePart*)console.FromPartHandle(dwhObj,CLS_LINEPART);
					if(pLinePart)
					{
						if(pLinePart->pStart&&pLinePart->pEnd)
						{
							line.startPt=pLinePart->pStart->Position(false);
							line.endPt  =pLinePart->pEnd->Position(false);
						}
						break;
					}
				}
	#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("Please select angle(slot angle or flat angle)which will be layout bolts:","");
	#else 
				pCmdLine->FillCmdLine("没有选中合适的构件，请重新选择需要布置螺栓的角钢(槽钢或扁钢):","");
	#endif
			}
			g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
			pCmdLine->FinishCmdLine(CXhChar16("0X%X",pLinePart->handle));
		}
		pCmdLine->FinishCmdLine();
		if(!(pLinePart->IsAngle()&&pLinePart->pStart==NULL&&pLinePart->pEnd==NULL))
		{	//布置短角钢螺栓时，不需要选择节点
	#ifdef AFX_TARG_ENU_ENGLISH
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				pCmdLine->FillCmdLine("Please select connected node which angle's bolts attached:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				pCmdLine->FillCmdLine("Please select connected node which slot angle's bolts attached:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
				pCmdLine->FillCmdLine("Please select connected node which flat angle's bolts attached:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
				pCmdLine->FillCmdLine("Please select connected node which tube's bolts attached:","");
	#else 
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				pCmdLine->FillCmdLine("请选择角钢上布置螺栓所归属的连接节点:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				pCmdLine->FillCmdLine("请选择槽钢上布置螺栓所归属的连接节点:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
				pCmdLine->FillCmdLine("请选择扁钢上布置螺栓所归属的连接节点:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
				pCmdLine->FillCmdLine("请选择钢管上布置螺栓所归属的连接节点:","");
	#endif
			CDisplayNodeAtFrontLife showPoint;
			showPoint.DisplayNodeAtFront();
			verifier.ClearSnapFlag();
			verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
			verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
			while(1)
			{
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					//布置完螺栓退出命令时，切换到实体显示模式
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					Invalidate(FALSE);
					return 0;
				}	
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;		
				if(dwhObj>0x20)
				{
					pBaseNode=console.FromNodeHandle(dwhObj);
					if(pBaseNode&&line.PtInLine(pBaseNode->Position(false))!=0)
						break;
				}
			}
			g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
			pCmdLine->FinishCmdLine(CXhChar16("0X%X",pBaseNode->handle));
			showPoint.HideNodeAtFront();
		}
		int i=0;
		if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//布置组合角钢螺栓
			g_pSolidSet->SetDisplayType(DISP_SOLID);
			g_pSolidDraw->Draw();
			CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
			for(i=0;i<4;i++)
			{
				CLDSLineAngle *pCommBaseJg=NULL,*pBackToBackJgX=NULL,*pBackToBackJgY=NULL;
				if(pGroupAngle->group_style==2)	
				{	//布置十字组合角钢螺栓时，仅需布置1号和3号子角钢的螺栓  wht 09-09-07
					//系统会自动将1号角钢上的螺栓引入到2,4号角钢上，将3号角钢上的螺栓引入到2,4号角钢上 
					if(i==1||i==3)
						continue;
					if(i==0)
					{	//1号角钢X肢螺栓引入到2号角钢，Y肢上的螺栓引入到4号角钢	wht 09-10-11
						if(pGroupAngle->son_jg_h[1]>=0x20)
							pBackToBackJgX=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[1],CLS_LINEANGLE);
						if(pGroupAngle->son_jg_h[3]>0x20)
							pBackToBackJgY=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[3],CLS_LINEANGLE);
					}
					else if(i==2)
					{	//3号角钢Y肢螺栓引入到2号角钢，X肢上的螺栓引入到4号角钢	wht 09-10-11
						if(pGroupAngle->son_jg_h[3]>=0x20)
							pBackToBackJgX=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[3],CLS_LINEANGLE);
						if(pGroupAngle->son_jg_h[1]>=0x20)
							pBackToBackJgY=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[1],CLS_LINEANGLE);
					}
				}
				if(pGroupAngle&&pGroupAngle->son_jg_h[i]>=0x20)
					pCommBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
				if(pCommBaseJg==NULL||pCommBaseJg->m_bVirtualPart)
					continue;	//不存在对应的子角钢或子角钢为虚拟构件 wht 11-07-25
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pCommBaseJg->handle);
				LayoutSingleAngleBolts(pCommBaseJg,pBaseNode,pBackToBackJgX,pBackToBackJgY);
			}
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)	//布置单角钢
			LayoutSingleAngleBolts((CLDSLineAngle*)pLinePart,pBaseNode);
		else if (pLinePart->GetClassTypeId()==CLS_LINEFLAT||pLinePart->GetClassTypeId()==CLS_LINESLOT)//槽钢、扁钢螺栓
			LayoutSlotOrFlatBolts(pLinePart,pBaseNode);
		else if (pLinePart->GetClassTypeId()==CLS_LINETUBE)
			LayoutTubeBolts((CLDSLineTube*)pLinePart,pBaseNode);//钢管
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->Draw();	
		m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
		Ta.EndUndoListen();
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Command:","");
#else 
		pCmdLine->FillCmdLine("命令:","");
#endif
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	return 0;
}

void _LocalLayoutSingleRodBolts(CLDSLinePart* pCurRod,BYTE ciCurWorkWing,RODSECTION sect,CLDSLineAngle* pCrossAngle/*=NULL*/)
{
	if(pCurRod==NULL)
		return ;
	int iInitRayNo=1;
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	ATOM_LIST<CDesignLsPara>ls_list;
	CLayAngleBoltDlg laybolt_dlg;
	laybolt_dlg.m_pLinePart = pCurRod;
	laybolt_dlg.m_ciSectType = sect.ciSectType;
	laybolt_dlg.m_pNode = sect.pSectNode;
	laybolt_dlg.m_pLsList = &ls_list;
	laybolt_dlg.m_iOffsetWing = ciCurWorkWing;
	//laybolt_dlg.m_iBoltNorm = ciCurWorkWing;
	laybolt_dlg.viewNorm = console.GetActiveView()->ucs.axis_z;
	laybolt_dlg.m_bIncPlateProfilePara = FALSE;
	laybolt_dlg.m_bTwoEdgeProfile = FALSE;
	if (pCrossAngle)
	{
		laybolt_dlg.m_iDatumPointStyle = 3;	//心线交点
		laybolt_dlg.m_hCrossAngle.Format("0X%X", pCrossAngle->handle);
	}
	if (laybolt_dlg.DoModal() != IDOK)
		return;
	//根据用户输入在角钢上布置螺栓
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->iSeg = pCurRod->iSeg;
		pBolt->CopyModuleInstanceInfo(pCurRod);	//调整螺栓配材号与基准构件配材号一致
		pBolt->SetGrade(pLsPara->grade);
		pBolt->AddL0Thick(pCurRod->handle,TRUE);
		pBolt->m_cFuncType=laybolt_dlg.m_iHoleFuncType;
		pBolt->m_bVirtualPart=(pBolt->m_cFuncType>=2);
		if (pBolt->d == 12 || pBolt->d == 16 || pBolt->d == 20 || pBolt->d == 24)
			pBolt->hole_d_increment = 1.5;
		else
			pBolt->hole_d_increment = 0;
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		HOLE_WALL* pHoleWall = NULL;
		if (pBolt->m_bVirtualPart)
		{	//添加挂线孔，不显示螺栓实体，显示孔壁  wxc-2019.7.24
			if((pHoleWall = console.MakeHoleWall(pBolt, pCurRod))==NULL)
			{
				pHoleWall = console.hashHoles.Add(DUALKEY(pBolt->handle, pCurRod->handle));
				pHoleWall->pBolt = pBolt;
				for (int j = 0; j < 4; j++)
				{
					if (pHoleWall->items[j].hRelaPart == pCurRod->handle)
						break;
					else if (pHoleWall->items[j].hRelaPart == 0)
					{
						pHoleWall->items[j].hRelaPart = pCurRod->handle;
						break;
					}
				}
				pHoleWall->is_visible = TRUE;
				pHoleWall->Create3dSolidModel(g_pSolidOper->GetScaleUserToScreen(), g_pSolidOper->GetScaleUserToScreen(), g_sysPara.display.nSmoothness);
			}
			pBolt->is_visible = FALSE;
			g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
		}
		else
		{
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		//
		if(sect.ciSectType==0||(sect.pSectNode&&sect.pSectNode==pCurRod->pStart))
			pCurRod->AppendStartLsRef(pBolt->GetLsRef());
		else if(sect.ciSectType==1||(sect.pSectNode&&sect.pSectNode==pCurRod->pEnd))
			pCurRod->AppendEndLsRef(pBolt->GetLsRef());
		else
			pCurRod->AppendMidLsRef(pBolt->GetLsRef());
		if (pBolt->m_bVirtualPart)
		{	//绘制角钢上螺栓孔
			pCurRod->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pCurRod->GetSolidPartObject());
		}
		//交叉点定位
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	
		{
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			f3dPoint wing_vec,wing_norm,datum_pos;
			if(fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_x_wing())>
				fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_y_wing()))
			{
				wing_vec=pCrossAngle->GetWingVecX();
				wing_norm=pCrossAngle->get_norm_x_wing();
			}
			else
			{
				wing_vec=pCrossAngle->GetWingVecY();
				wing_norm=pCrossAngle->get_norm_y_wing();
			}
			datum_pos=pCrossAngle->Start();
			f3dPoint pos;
			Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,pCrossAngle->Start(),wing_norm);
			f3dPoint bolt_vec=pos-datum_pos;
			double dd=bolt_vec*wing_vec;
			if(dd>0&&dd<pCrossAngle->GetWidth())
			{	//交叉螺栓位于交叉角钢内
				pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(pCrossAngle->handle,TRUE);
			}
			if (pBolt->m_bVirtualPart && pHoleWall)
			{	//绘制交叉角钢上螺栓孔及孔壁实体
				for (int j = 0; j < 4; j++)
				{
					if (pHoleWall->items[j].hRelaPart == pCrossAngle->handle)
						break;
					else if (pHoleWall->items[j].hRelaPart == 0)
					{
						pHoleWall->items[j].hRelaPart = pCrossAngle->handle;
						break;
					}
				}
				pHoleWall->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pHoleWall->GetSolidPartObject());
				pCrossAngle->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pCrossAngle->GetSolidPartObject());
			}
		}
		//
		if(pCurRod->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle* pCurAngle=(CLDSLineAngle*)pCurRod;
			double g=0.0;
			g=pCurAngle->GetLsG(pBolt);
			for(RAYNO_RECORD *pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
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
				pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
				pRayNo->hPart=pBolt->des_base_pos.hPart;
				pRayNo->yCoord=g;
				pBolt->dwRayNo=pRayNo->dwRayNo;
				iInitRayNo++;
			}
		}
	}
	pCurRod->SetModified();
}
void CLDSView::LayoutSingleRodBolts(CLDSLinePart* pCurRod, BYTE ciCurWorkWing, CLDSNode* pBaseNode, CLDSLineAngle* pCrossAngle/*=NULL*/)
{
	_LocalLayoutSingleRodBolts(pCurRod,ciCurWorkWing,RODSECTION(pBaseNode),pCrossAngle);
}
void CLDSView::LayoutTubeBolts(CLDSLineTube* pLineTube,CLDSNode* pBaseNode)
{
#ifdef __COMPLEX_PART_INC_
	if(pBaseNode==NULL||pLineTube==NULL)
		return;
	CLayTubeBoltDlg laybolt_dlg;
	ATOM_LIST<CDesignLsPara>ls_list;
	CDesignLsPara ls_stru;
	int iInitRayNo=1;
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	laybolt_dlg.m_pTube = pLineTube;
	laybolt_dlg.m_pNode = pBaseNode;
	laybolt_dlg.m_pLsList=&ls_list;
	laybolt_dlg.viewNorm = console.GetActiveView()->ucs.axis_z;
	if(laybolt_dlg.DoModal()!=IDOK)
		return;
	Ta.BeginUndoListen();
	pLineTube->SetModified();
	pLineTube->CalPosition();
	pLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pLineTube->GetSolidPartObject());
	//根据用户输入在角钢上布置螺栓
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSLineAngle *pBackToBackJg=NULL;
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->m_bVirtualPart=pLsPara->m_bVirtualPart;
		pBolt->iSeg = pLineTube->iSeg;
		pBolt->cfgword=pLineTube->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pBolt->SetGrade(pLsPara->grade);
		pBolt->AddL0Thick(pLineTube->handle,TRUE);
		if(pBackToBackJg)
		{	//更新螺栓通厚以及法向偏移量
			pBolt->AddL0Thick(pBackToBackJg->handle,TRUE);
			if(pBolt->des_work_norm.direction==1)	//朝内
				pBolt->des_base_pos.norm_offset.AddThick(-pBackToBackJg->handle,TRUE,TRUE);
		}
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		if(pBaseNode==pLineTube->pStart)
			pLineTube->AppendStartLsRef(pBolt->GetLsRef());
		else if(pBaseNode==pLineTube->pEnd)
			pLineTube->AppendEndLsRef(pBolt->GetLsRef());
		else
			pLineTube->AppendMidLsRef(pBolt->GetLsRef());
		double g=0.0;
		for(RAYNO_RECORD *pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
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
			pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
			pRayNo->hPart=pBolt->des_base_pos.hPart;
			pRayNo->yCoord=g;
			pBolt->dwRayNo=pRayNo->dwRayNo;
			iInitRayNo++;
		}
	}
	if(pBaseNode==pLineTube->pStart&&pLineTube->desStartOdd.m_iOddCalStyle==1)
		pLineTube->CalStartOddment();
	else if(pBaseNode==pLineTube->pEnd&&pLineTube->desEndOdd.m_iOddCalStyle==1)
		pLineTube->CalEndOddment();
	pLineTube->SetModified();
#endif
}
void CLDSView::LayoutSlotOrFlatBolts(CLDSLinePart *pLinePart,CLDSNode *pBaseNode)
{
	if(pBaseNode==NULL||pLinePart==NULL||(pLinePart->GetClassTypeId()!=CLS_LINEFLAT&&pLinePart->GetClassTypeId()!=CLS_LINESLOT))
		return;
	CLayAngleBoltDlg laybolt_dlg;
	ATOM_LIST<CDesignLsPara>ls_list;
	CDesignLsPara ls_stru;
	int iInitRayNo=1;
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	
	if(pBaseNode==pLinePart->pEnd)
	{
		laybolt_dlg.m_iOddCalStyle=pLinePart->desEndOdd.m_iOddCalStyle;
		laybolt_dlg.m_fOddment=pLinePart->endOdd();
		laybolt_dlg.m_iRayDirection=1;	//终－>始
	}
	else if(pBaseNode==pLinePart->pStart)
	{
		laybolt_dlg.m_iOddCalStyle=pLinePart->desStartOdd.m_iOddCalStyle;
		laybolt_dlg.m_fOddment=pLinePart->startOdd();
		laybolt_dlg.m_iRayDirection=0;	//始－>终
	}
	laybolt_dlg.m_pLinePart = pLinePart;
	laybolt_dlg.m_pNode = pBaseNode;
	laybolt_dlg.m_pLsList=&ls_list;
	laybolt_dlg.viewNorm = console.GetActiveView()->ucs.axis_z;
	if(laybolt_dlg.DoModal()!=IDOK)
		return;
	Ta.BeginUndoListen();
	if(pBaseNode==pLinePart->pEnd)
	{
		pLinePart->desEndOdd.m_iOddCalStyle=laybolt_dlg.m_iOddCalStyle;
		pLinePart->SetEndOdd(laybolt_dlg.m_fOddment);
		/*if(laybolt_dlg.m_iOffsetWing==0)	
			pLinePart->desEndPos.wing_x_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;
		else
			pLinePart->desEndPos.wing_y_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;*/
		pLinePart->SetModified();
		pLinePart->CalPosition();
		pLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pLinePart->GetSolidPartObject());
	}
	else if(pBaseNode==pLinePart->pStart)
	{
		pLinePart->desStartOdd.m_iOddCalStyle=laybolt_dlg.m_iOddCalStyle;
		pLinePart->SetStartOdd(laybolt_dlg.m_fOddment);
		/*if(laybolt_dlg.m_iOffsetWing==0)
			pLinePart->desStartPos.wing_x_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;
		else
			pLinePart->desStartPos.wing_y_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;*/
		pLinePart->SetModified();
		pLinePart->CalPosition();
		pLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pLinePart->GetSolidPartObject());
	}
	//根据用户输入在角钢上布置螺栓
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->iSeg = pLinePart->iSeg;
		pBolt->cfgword=pLinePart->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pBolt->SetGrade(pLsPara->grade);
		pBolt->AddL0Thick(pLinePart->handle,TRUE);
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		if(pBaseNode==pLinePart->pStart)
			pLinePart->AppendStartLsRef(pBolt->GetLsRef());
		else if(pBaseNode==pLinePart->pEnd)
			pLinePart->AppendEndLsRef(pBolt->GetLsRef());
		else
			pLinePart->AppendMidLsRef(pBolt->GetLsRef());
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//交叉点定位
		{
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			if(pCrossAngle)
			{
				f3dPoint pos,bolt_vec,wing_vec;
				if(fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_x_wing())>
					fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_y_wing()))
				{
					wing_vec=pCrossAngle->GetWingVecX();
					Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,
						pCrossAngle->Start(),pCrossAngle->get_norm_x_wing());
				}
				else
				{
					wing_vec=pCrossAngle->GetWingVecY();
					Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,
						pCrossAngle->Start(),pCrossAngle->get_norm_y_wing());
				}
				bolt_vec=pos-pCrossAngle->Start();
				double dd=bolt_vec*wing_vec;
				if(dd>0&&dd<pCrossAngle->GetWidth())
				{	//交叉螺栓位于交叉角钢内
					pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pCrossAngle->handle,TRUE);
				}
			}
		}
		/*double g=0.0;
		g=pLinePart->GetLsG(pBolt);
		for(RAYNO_RECORD *pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
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
			pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
			pRayNo->hPart=pBolt->des_base_pos.hPart;
			pRayNo->yCoord=g;
			pBolt->dwRayNo=pRayNo->dwRayNo;
			iInitRayNo++;
		}*/
	}
	if(pBaseNode==pLinePart->pStart&&pLinePart->desStartOdd.m_iOddCalStyle==1)
		pLinePart->CalStartOddment();
	else if(pBaseNode==pLinePart->pEnd&&pLinePart->desEndOdd.m_iOddCalStyle==1)
		pLinePart->CalEndOddment();
	pLinePart->SetModified();
}

void CLDSView::LayoutSingleAngleBolts(CLDSLineAngle *pLineAngle, CLDSNode *pBaseNode, 
									  CLDSLineAngle *pBackToBackJgX/*=NULL*/,CLDSLineAngle *pBackToBackJgY/*=NULL*/)
{
	CLayAngleBoltDlg laybolt_dlg;
	ATOM_LIST<CDesignLsPara>ls_list;
	CDesignLsPara ls_stru;
	int iInitRayNo=1;
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	if(pLineAngle==NULL||(pLineAngle->pStart&&pLineAngle->pEnd&&pBaseNode==NULL))
		return;
	if(pBaseNode&&pBaseNode==pLineAngle->pEnd)
	{
		laybolt_dlg.m_iOddCalStyle=pLineAngle->desEndOdd.m_iOddCalStyle;
		laybolt_dlg.m_fOddment=pLineAngle->endOdd();
		laybolt_dlg.m_iRayDirection=1;	//终－>始
	}
	else if((pBaseNode&&pBaseNode==pLineAngle->pStart)||
			(pLineAngle->pStart==NULL&&pLineAngle->pEnd==NULL))
	{
		laybolt_dlg.m_iOddCalStyle=pLineAngle->desStartOdd.m_iOddCalStyle;
		laybolt_dlg.m_fOddment=pLineAngle->startOdd();
		laybolt_dlg.m_iRayDirection=0;	//始－>终
	}
	laybolt_dlg.m_pLinePart = pLineAngle;
	laybolt_dlg.m_pNode = pBaseNode;
	laybolt_dlg.m_pLsList=&ls_list;
	laybolt_dlg.viewNorm = console.GetActiveView()->ucs.axis_z;
	if(laybolt_dlg.DoModal()!=IDOK)
		return;
	Ta.BeginUndoListen();
	if(pBaseNode&&pBaseNode==pLineAngle->pEnd)
	{
		pLineAngle->desEndOdd.m_iOddCalStyle=laybolt_dlg.m_iOddCalStyle;
		pLineAngle->SetEndOdd(laybolt_dlg.m_fOddment);
		if(laybolt_dlg.m_iOffsetWing==0)	
			pLineAngle->desEndPos.wing_x_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;
		else
			pLineAngle->desEndPos.wing_y_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;
		pLineAngle->SetModified();
		pLineAngle->CalPosition();
		pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
	}
	else if((pBaseNode&&pBaseNode==pLineAngle->pStart)||
			(pLineAngle->pStart==NULL&&pLineAngle->pEnd==NULL))
	{
		pLineAngle->desStartOdd.m_iOddCalStyle=laybolt_dlg.m_iOddCalStyle;
		pLineAngle->SetStartOdd(laybolt_dlg.m_fOddment);
		if(laybolt_dlg.m_iOffsetWing==0)
			pLineAngle->desStartPos.wing_x_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;
		else
			pLineAngle->desStartPos.wing_y_offset.offsetDist=laybolt_dlg.m_fAngleEndNormOffset;
		pLineAngle->SetModified();
		pLineAngle->CalPosition();
		pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
	}
	//根据用户输入在角钢上布置螺栓
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSLineAngle *pBackToBackJg=NULL;
		if(pLsPara->des_work_norm.norm_wing==0)		//X肢
			pBackToBackJg=pBackToBackJgX;
		else if(pLsPara->des_work_norm.norm_wing==1)//Y肢
			pBackToBackJg=pBackToBackJgY;
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->iSeg = pLineAngle->iSeg;
		pBolt->CopyModuleInstanceInfo(pLineAngle);	//调整螺栓配材号与基准构件配材号一致
		pBolt->SetGrade(pLsPara->grade);
		pBolt->AddL0Thick(pLineAngle->handle,TRUE);
		if(pBackToBackJg)
		{	//更新螺栓通厚以及法向偏移量
			pBolt->AddL0Thick(pBackToBackJg->handle,TRUE);
			if(pBolt->des_work_norm.direction==1)	//朝内
				pBolt->des_base_pos.norm_offset.AddThick(-pBackToBackJg->handle,TRUE,TRUE);
		}
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		if(pBaseNode==pLineAngle->pStart)
		{
			pLineAngle->AppendStartLsRef(pBolt->GetLsRef());
			if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE&&pBackToBackJg)	//将螺栓引入到背对背的角钢上 wht 09-09-07
				pBackToBackJg->AppendStartLsRef(pBolt->GetLsRef());
		}
		else if(pBaseNode==pLineAngle->pEnd)
		{
			pLineAngle->AppendEndLsRef(pBolt->GetLsRef());
			if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE&&pBackToBackJg)	//将螺栓引入到背对背的角钢上
				pBackToBackJg->AppendEndLsRef(pBolt->GetLsRef());
		}
		else
		{
			pLineAngle->AppendMidLsRef(pBolt->GetLsRef());
			if(pBackToBackJg)	//将螺栓引入到背对背的角钢上
				pBackToBackJg->AppendMidLsRef(pBolt->GetLsRef());
		}
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//交叉点定位
		{
			CLDSLinePart *pLinePart=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
			if(pLinePart)
			{
				f3dPoint wing_vec,wing_norm,datum_pos;
				if(pLinePart->IsAngle())
				{	//角钢
					CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)pLinePart;
					if(fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_x_wing())>
						fabs(pBolt->ucs.axis_z*pCrossAngle->get_norm_y_wing()))
					{
						wing_vec=pCrossAngle->GetWingVecX();
						wing_norm=pCrossAngle->get_norm_x_wing();
					}
					else
					{
						wing_vec=pCrossAngle->GetWingVecY();
						wing_norm=pCrossAngle->get_norm_y_wing();
					}
					datum_pos=pLinePart->Start();
				}
				else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
				{	//扁铁
					wing_norm=pLinePart->ucs.axis_z;
					wing_vec=pLinePart->ucs.axis_y;
					datum_pos=pLinePart->Start()-wing_vec*pLinePart->size_wide*0.5;
				}
				else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				{	//槽钢
					wing_norm=pLinePart->ucs.axis_y;
					wing_vec=pLinePart->ucs.axis_x;
					datum_pos=pLinePart->Start()-wing_vec*pLinePart->size_wide*0.5;
				}
				f3dPoint pos;
				Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,pLinePart->Start(),wing_norm);
				f3dPoint bolt_vec=pos-datum_pos;
				double dd=bolt_vec*wing_vec;
				if(dd>0&&dd<pLinePart->GetWidth())
				{	//交叉螺栓位于交叉角钢内
					pLinePart->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pLinePart->handle,TRUE);
				}
			}
		}
		double g=0.0;
		g=pLineAngle->GetLsG(pBolt);
		for(RAYNO_RECORD *pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
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
			pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
			pRayNo->hPart=pBolt->des_base_pos.hPart;
			pRayNo->yCoord=g;
			pBolt->dwRayNo=pRayNo->dwRayNo;
			iInitRayNo++;
		}
	}
	if(pBaseNode==pLineAngle->pStart&&pLineAngle->desStartOdd.m_iOddCalStyle==1)
		pLineAngle->CalStartOddment();
	else if(pBaseNode==pLineAngle->pEnd&&pLineAngle->desEndOdd.m_iOddCalStyle==1)
		pLineAngle->CalEndOddment();
	pLineAngle->SetModified();
}
//二、单螺栓连接包括：1.交叉点单螺栓 2.单角钢端螺栓连接 3.无板单螺栓连接
void CLDSView::OnOneBoltDesign()
{	//单螺栓连接
	m_nPrevCommandID=ID_ONEBOLT_DESIGN;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to connect single bolts";
#else 
	m_sPrevCommandName="重复单螺栓连接";
#endif
	Command("1Bolt");
}

void CLDSView::OnSpecNodeOneBoltDesign()
{	//批量生成单螺栓连接
	m_nPrevCommandID=ID_SPEC_NODE_ONEBOLT_DESIGN;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to specify single-bolt connection";
#else 
	m_sPrevCommandName="重复指定节点单螺栓连接";
#endif
	Command("Spec1Bolt");
}
//设计垫板
static BOOL DesignDianBan(CLDSBolt *pLs,BOOL bThrowError,CLDSNode* pNode,
						 double dist,int nType,CDianBanParaDlg *pDlg=NULL)
{
	CDianBanParaDlg dianbandlg;
	CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pNode->arrRelationPole[1],CLS_LINEPART);
	CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pNode->arrRelationPole[0],CLS_LINEPART);
	if(dianbandlg.m_nSpaceDist==0||dianbandlg.m_nThick==0)
	{
		dianbandlg.m_sLsGuiGe.Format("%d",pLs->get_d());
		dianbandlg.m_nSpaceDist = (int)(dist+0.5);//圆整 pLs->DianQuan.thick;
		dianbandlg.m_iPartType=nType;
		dianbandlg.m_nThick=(nType==1)?pLs->DianQuan.thick:dianbandlg.m_nSpaceDist;
		if(dianbandlg.m_nThick>0)
			dianbandlg.m_nNum=dianbandlg.m_nSpaceDist/dianbandlg.m_nThick;
	}
	//对称生成螺栓垫板时如果前后间隙值不相同则需要弹出设计对话框 wht 11-01-19
	if((pDlg&&pDlg->m_bMirCreate&&dianbandlg.m_nSpaceDist==pDlg->m_nSpaceDist)	
		||!bThrowError||dianbandlg.DoModal()==IDOK)
	{	//
		//复制保存的参数到当前对话框 wht 11-01-10
		if(pDlg&&pDlg->m_bMirCreate&&dianbandlg.m_nSpaceDist==pDlg->m_nSpaceDist) 
			dianbandlg.CopyProperty(*pDlg);
		double ddx,ddy;
		CLDSLineAngle* pDatumAngle=NULL;
		if(pLinePart1->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pLinePart1;
			ddx=pAngle->get_norm_x_wing()*pLs->get_norm();
			ddy=pAngle->get_norm_y_wing()*pLs->get_norm();
			if( (fabs(ddx)>fabs(ddy)&&ddx>EPS_COS2)||
				(fabs(ddy)>fabs(ddx)&&ddy>EPS_COS2))
				pDatumAngle=pAngle;
		}
		if(pDatumAngle==NULL&&pLinePart2->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pLinePart2;
			ddx=pAngle->get_norm_x_wing()*pLs->get_norm();
			ddy=pAngle->get_norm_y_wing()*pLs->get_norm();
			if( (fabs(ddx)>fabs(ddy)&&ddx>EPS_COS2)||
				(fabs(ddy)>fabs(ddx)&&ddy>EPS_COS2))
				pDatumAngle=pAngle;
		}
		if(!dianbandlg.m_bAutoJudgeLs)
			pLs->set_d(atol(dianbandlg.m_sLsGuiGe));
		if(dianbandlg.m_iPartType == 0)
		{
			int iCycle = 0;
			long nSurplus=(long)(pLs->DianQuan.thick-dianbandlg.m_nNum*dianbandlg.m_nThick);
			if(dianbandlg.m_bCreateSurplusPlate&&nSurplus>0)
				iCycle = dianbandlg.m_nNum+1;
			else
				iCycle = dianbandlg.m_nNum;
			for(int i=0;i<iCycle;i++)
			{
				if(dianbandlg.m_nThick<=0)
					continue;
				CLDSPlate *pPadPlank=(CLDSPlate*)console.AppendPart(CLS_PLATE);
				pPadPlank->cfgword=pLinePart1->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
				pPadPlank->designInfo.m_hBasePart=pLinePart1->handle;
				if(i==iCycle-1&&nSurplus>0&&dianbandlg.m_bCreateSurplusPlate)
					pPadPlank->Thick=nSurplus;
				else
					pPadPlank->Thick=dianbandlg.m_nThick;
				pPadPlank->jdb_style=4;
				if(pDatumAngle)
				{
					pPadPlank->designInfo.m_hBasePart=pDatumAngle->handle;	//保证钢板基准构件与原点及法线基准构件一致 wht 15-06-25
					pPadPlank->designInfo.origin.datum_pos_style=2;
					pPadPlank->designInfo.origin.des_para.RODNODE.hRod=pDatumAngle->handle;
					pPadPlank->designInfo.origin.des_para.RODNODE.wing_offset_style=0;
					pPadPlank->designInfo.origin.des_para.RODNODE.hNode=pNode->handle;
					if(fabs(ddx)>fabs(ddy)&&fabs(ddx)>EPS_COS2)	//X肢为工作肢
						pPadPlank->designInfo.origin.des_para.RODNODE.offset_wing=0;
					else if(fabs(ddy)>fabs(ddx)&&fabs(ddy)>EPS_COS2)
						pPadPlank->designInfo.origin.des_para.RODNODE.offset_wing=1;
					pPadPlank->designInfo.origin.UpdatePos(pPadPlank->BelongModel());
					pPadPlank->ucs.origin=pPadPlank->designInfo.origin.Position();
					//设置螺栓垫板法线参数
					pPadPlank->designInfo.norm.norm_style=1;
					pPadPlank->designInfo.norm.hVicePart=pDatumAngle->handle;
					pPadPlank->designInfo.norm.norm_wing=pPadPlank->designInfo.origin.des_para.RODNODE.offset_wing;
					pPadPlank->designInfo.norm.direction=0;
				}
				else
				{
					pPadPlank->ucs.origin=pLs->ucs.origin+pLs->get_norm()*(pLs->DianQuan.offset+i*dianbandlg.m_nThick);
					//设置螺栓垫板原点及法线参数
					pPadPlank->designInfo.origin.datum_pos_style=0;
					pPadPlank->designInfo.origin.datum_pos=pPadPlank->ucs.origin;
					pPadPlank->designInfo.norm.norm_style=0;
					pPadPlank->designInfo.norm.vector=pLs->get_norm();
				}
				pPadPlank->ucs.axis_z=pLs->get_norm();
				pPadPlank->ucs.axis_y=pLinePart2->End()-pLinePart2->Start();
				pPadPlank->cDatumAxis='Y';
				normalize(pPadPlank->ucs.axis_y);
				pPadPlank->ucs.axis_x=pPadPlank->ucs.axis_y^pPadPlank->ucs.axis_z;
				pPadPlank->ucs.axis_y=pPadPlank->ucs.axis_z^pPadPlank->ucs.axis_x;
				pPadPlank->AppendLsRef(pLs->GetLsRef());
				pPadPlank->SetLayer(pLs->layer());
				pPadPlank->cfgword=pLs->cfgword;
				pPadPlank->iSeg=pLs->iSeg;
				if(dianbandlg.m_bAutoJudgePad)
					pPadPlank->CalStdProfile();
				else
				{
					PROFILE_VERTEX *pVertex = pPadPlank->vertex_list.append();
					pVertex->vertex.Set(dianbandlg.m_fPadLen*0.5,dianbandlg.m_fPadWidth*0.5,0);
					pVertex->vertex.feature=1;
					pVertex = pPadPlank->vertex_list.append();
					pVertex->vertex.Set(-dianbandlg.m_fPadLen*0.5,dianbandlg.m_fPadWidth*0.5,0);
					pVertex->vertex.feature=1;
					pVertex = pPadPlank->vertex_list.append();
					pVertex->vertex.Set(-dianbandlg.m_fPadLen*0.5,-dianbandlg.m_fPadWidth*0.5,0);
					pVertex->vertex.feature=1;
					pVertex = pPadPlank->vertex_list.append();
					pVertex->vertex.Set(dianbandlg.m_fPadLen*0.5,-dianbandlg.m_fPadWidth*0.5,0);
					pVertex->vertex.feature=1;
				}
				if(i==iCycle-1&&dianbandlg.m_bCreateSurplusPlate)
					pPadPlank->SetPartNo(dianbandlg.m_sPartNoSurplus.GetBuffer());
				else
					pPadPlank->SetPartNo(dianbandlg.m_sPartNo.GetBuffer());
				if(UI::blEnableIntermediateUpdateUI)
				{
					pPadPlank->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pPadPlank->GetSolidPartObject());
				}
			}
			pLs->DianQuan.N = 0;
			pLs->DianQuan.offset = 0;
			pLs->DianQuan.thick = 0;
		}
		else
		{
			pLs->DianQuan.N = dianbandlg.m_nNum;
			pLs->DianQuan.thick = dianbandlg.m_nThick;
			if(pLs->DianQuan.thick!=CLDSPart::library->GetBoltPadThick(pLs->get_d()))
				pLs->DianQuan.AutoMatchThick=false;	//特殊指定垫圈厚度，以后不再重新设计
			if(pDatumAngle)
				pLs->DianQuan.offset=pDatumAngle->GetThick();
			else
				pLs->DianQuan.offset=0;
		}
	}
	else
	{	//必须清零,否则会是一堆随机数,有可能导致螺栓通厚超长 wjh-2015.4.8
		pLs->DianQuan.N = 0;
		pLs->DianQuan.offset = 0;
		pLs->DianQuan.thick = 0;
	}
	//保存垫板对话框设置参数 wht 11-01-10
	if(pDlg&&(!pDlg->m_bMirCreate||(pDlg->m_bMirCreate&&dianbandlg.m_nSpaceDist!=pDlg->m_nSpaceDist)))
		pDlg->CopyProperty(dianbandlg);
	return TRUE;
}
extern BOOL UnifyAngleBoltParamG(DESIGN_LS_POS &designLsPos);
//1.设计交叉点单螺栓 
BOOL DesignIntersNode(CLDSNode *pNode,BOOL bThrowError,CDianBanParaDlg *pDlg=NULL)
{
	CLDSLinePart  *pLinePart1=NULL,*pLinePart2=NULL;
	CLDSLineAngle *pAngle1=NULL,   *pAngle2=NULL;
	CLDSLineFlat  *pLineFlat1=NULL,*pLineFlat2=NULL;
	JGZJ jgzj1,jgzj2;
	f3dPoint wing_vec1,wing_vec2,ls_pos,ls_norm;
	double dist;
	CLDSBolt ls(console.GetActiveModel());
	CUndoOperObject undo(&Ta, true);
	try
	{
		if(pNode==NULL)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected parts are illegal";
#else 
				throw "选择非法构件";
#endif
			else
				return FALSE;
		}
		if(pNode->m_cPosCalType!=4)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected node isn't cross node";
#else 
				throw "选择节点不是交叉节点";
#endif
			else
				return FALSE;
		}
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of the node！";
#else 
				throw "没有此节点的修改权限！";
#endif
			else
				return FALSE;
		}
		LINEPARTSET jgset;
		for(CLDSLinePart *pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
		{
			if(pLinePart->pStart==pNode||pLinePart->pEnd==pNode)
				jgset.append(pLinePart);
		}
		pLinePart1=(CLDSLinePart*)console.FromPartHandle(pNode->arrRelationPole[0],CLS_LINEPART);
		pLinePart2=(CLDSLinePart*)console.FromPartHandle(pNode->arrRelationPole[1],CLS_LINEPART);
		if(pLinePart1==NULL || pLinePart2==NULL)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected node lack of attached part";
#else 
				throw "选择节点缺少依附构件";
#endif
			else
				return FALSE;
		}
		//检查该交叉点是否已经存在交叉点螺栓
		int i=0;
		CLsRef *pLsRef=NULL;
		int x_wing0_y_wing1;
		for(i=0;i<2;i++)
		{
			CLDSLinePart *pLineRod=NULL;
			if(i==0)
				pLineRod=pLinePart1;
			else if(i==1)
				pLineRod=pLinePart2;
			if(pLineRod==NULL)
				continue;
			for(pLsRef=pLineRod->GetFirstLsRef();pLsRef;pLsRef=pLineRod->GetNextLsRef())
			{
				if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==3)	//两角钢交叉点定位
				{
					long hDatum1=(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
					long hDatum2=(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
					if( (hDatum1==pLinePart1->handle&&hDatum2==pLinePart2->handle)||
						(hDatum2==pLinePart1->handle&&hDatum1==pLinePart2->handle))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "Intersection bolt 0x%X already exists and does not allow the repeat design!",(*pLsRef)->handle);
#else
						logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"已存在0x%X交叉点螺栓，不能重复设计!",(*pLsRef)->handle);
#endif
						return FALSE;
					}
				}
			}
		}
		//1.设定螺栓法线设计参数
		double offsetDist;
		if(pLinePart1->IsAngle())
		{	//螺栓基准构件为角钢
			pAngle1=(CLDSLineAngle*)pLinePart1;
			offsetDist=pAngle1->GetThick();
			if(pLinePart2->IsAngle())
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)pLinePart2;
				GetWorkNorm(pAngle1,pAngle,&ls_norm);
			}
			else if(pLinePart2->GetClassTypeId()==CLS_LINEFLAT)
			{
				CLDSLineFlat *pLineFlat=(CLDSLineFlat*)pLinePart2;
				ls_norm=pLineFlat->WorkPlaneNorm();
			}
			//螺栓法线位于角钢的某肢法线上
			IsInsideJg(pAngle1,ls_norm,&x_wing0_y_wing1);
			if(x_wing0_y_wing1==0)
				ls_norm=pAngle1->get_norm_x_wing();
			else 
				ls_norm=pAngle1->get_norm_y_wing();
		}
		else if(pLinePart1->GetClassTypeId()==CLS_LINEFLAT)
		{	// 螺栓基准构件为扁铁，螺栓法线就是扁铁的法线
			pLineFlat1=(CLDSLineFlat*)pLinePart1;
			offsetDist=0;
			ls_norm = pLineFlat1->WorkPlaneNorm();
		}
		normalize(ls_norm);
		ls.set_norm(ls_norm);
		if(pAngle1)
		{
			ls.des_work_norm.norm_style=1;
			ls.des_work_norm.hVicePart=pAngle1->handle;
			ls.des_work_norm.norm_wing=x_wing0_y_wing1;
			ls.des_work_norm.direction=0;
		}
		else if(pLineFlat1)
		{
			ls.des_work_norm.norm_style=3;
			ls.des_work_norm.hVicePart=pLineFlat1->handle;
			ls.des_work_norm.nearVector=ls_norm;
			ls.des_work_norm.direction=0; 
		}
		//2.确定螺栓规格(直径)
		int ls_d=pLinePart1->connectStart.d;
		for(CLDSLinePart *pLinePart=(CLDSLinePart*)jgset.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)jgset.GetNext())
		{
			if(pLinePart->pStart==pNode)
				ls_d = __min(ls_d,pLinePart->connectStart.d);
			else if(pLinePart->pEnd==pNode)
				ls_d = __min(ls_d,pLinePart->connectEnd.d);
			if(!pLinePart->IsAngle())
				continue;
			CLDSLineAngle *pJg=(CLDSLineAngle*)pLinePart;
			if(pJg->pStart==pNode)	//根据吊杆调整螺栓直径大小
			{
				if(fabs(ls_norm*pJg->get_norm_x_wing())>fabs(ls_norm*pJg->get_norm_y_wing()))
				{
					pJg->desStartPos.wing_x_offset.gStyle=4;
					pJg->desStartPos.wing_x_offset.offsetDist=-offsetDist;
					pJg->desStartPos.wing_y_offset.gStyle=0;
				}
				else
				{
					pJg->desStartPos.wing_y_offset.gStyle=4;
					pJg->desStartPos.wing_y_offset.offsetDist=-offsetDist;
					pJg->desStartPos.wing_x_offset.gStyle=0;
				}
			}
			else if(pJg->pEnd==pNode)
			{
				if(fabs(ls_norm*pJg->get_norm_x_wing())>fabs(ls_norm*pJg->get_norm_y_wing()))
				{	//调整交叉点处连接的角钢位置	未完成...
					pJg->desEndPos.wing_x_offset.gStyle=4;
					pJg->desEndPos.wing_x_offset.offsetDist=-offsetDist;
					pJg->desEndPos.wing_y_offset.gStyle=0;
				}
				else
				{
					pJg->desEndPos.wing_y_offset.gStyle=4;
					pJg->desEndPos.wing_y_offset.offsetDist=-offsetDist;
					pJg->desEndPos.wing_x_offset.gStyle=0;
				}
			}
		}
		ls.set_d(ls_d);
		if(!ls.CalGuigeAuto())
		{
			char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"Can't find bolt specification M%dX%.f in specification library",ls.get_d(),ls.get_L0());
#else 
			sprintf(sError,"在螺栓规格库中没有找到符合M%dX%.f的螺栓规格",ls.get_d(),ls.L0);
#endif
			if(bThrowError)
				throw sError;
			else
				return FALSE;
		}
		//3.螺栓位置(法线偏移量)、垫板位置确定
		f3dLine L1,L2;
		if(pAngle1)
		{	
			getjgzj(jgzj1,pAngle1->GetWidth());
			if(pAngle1->m_bEnableTeG)
			{
				if(x_wing0_y_wing1==0)
					jgzj1=pAngle1->xWingXZhunJu;
				else
					jgzj1=pAngle1->xWingYZhunJu;
			}
			if(fabs(pAngle1->get_norm_x_wing()*ls.get_norm())>fabs(pAngle1->get_norm_y_wing()*ls.get_norm()))
				wing_vec1=pAngle1->GetWingVecX();
			else
				wing_vec1=pAngle1->GetWingVecY();
			L1.startPt= pAngle1->Start()+wing_vec1*jgzj1.g;
			L1.endPt  = pAngle1->End()+wing_vec1*jgzj1.g;
		}
		else if(pLineFlat1)
		{	
			L1.startPt=pLineFlat1->Start();
			L1.endPt=pLineFlat1->End();
		}
		if(pLinePart2->IsAngle())
		{
			pAngle2=(CLDSLineAngle*)pLinePart2;
			IsInsideJg(pAngle2,ls_norm,&x_wing0_y_wing1);
			getjgzj(jgzj2,pAngle2->GetWidth());
			if(pAngle2->m_bEnableTeG)
			{
				if(x_wing0_y_wing1==0)
					jgzj2=pAngle2->xWingXZhunJu;
				else
					jgzj2=pAngle2->xWingYZhunJu;
			}
			if(fabs(pAngle2->get_norm_x_wing()*ls.get_norm())>fabs(pAngle2->get_norm_y_wing()*ls.get_norm()))
				wing_vec2=pAngle2->GetWingVecX();
			else
				wing_vec2=pAngle2->GetWingVecY();
			L2.startPt=pAngle2->Start()+wing_vec2*jgzj2.g;
			L2.endPt  =pAngle2->End()+wing_vec2*jgzj2.g;
		}
		else if(pLinePart2->GetClassTypeId()==CLS_LINEFLAT)
		{
			pLineFlat2=(CLDSLineFlat*)pLinePart2;
			L2.startPt=pLineFlat2->Start();
			L2.endPt=pLineFlat2->End();
		}
		dist = DistOf3dll(L1,L2);
		project_point(L1.startPt,pLinePart1->Start(),ls.get_norm());
		project_point(L1.endPt,  pLinePart1->Start(),ls.get_norm());
		project_point(L2.startPt,pLinePart1->Start(),ls.get_norm());
		project_point(L2.endPt,  pLinePart1->Start(),ls.get_norm());
		if(Int3dll(L1,L2,ls_pos)<1)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The calculation of intersection coord is error";
#else 
				throw "交点坐标计算错误";
#endif
			else
				return FALSE;
		}
		if(pAngle1)
		{
			SnapPerp(&ls_pos,pAngle1->Start(),pAngle1->End(),ls_pos,NULL);
			ls_pos = ls_pos+jgzj1.g*wing_vec1;
			ls_pos = ls_pos-ls.get_norm()*pAngle1->GetThick();
			ls.des_base_pos.norm_offset.AddThick(-pAngle1->handle,TRUE);
			ls.DianQuan.offset=pAngle1->GetThick();	//垫圈位置
			if(pLineFlat2&&pLineFlat2->WorkPlaneNorm()*ls_norm<0)
				dist-=pLineFlat2->GetThick();
		}
		else if(pLineFlat1)
		{
			SnapPerp(&ls_pos,pLineFlat1->Start(),pLineFlat1->End(),ls_pos,NULL);
			f3dPoint vec=pLinePart2->Start() - ls_pos;
			if(vec*ls_norm<0)
			{	
				ls_norm*=-1;
				ls.set_norm(ls_norm);
				ls.des_work_norm.direction=1;
				ls_pos=ls_pos-ls_norm*pLineFlat1->GetThick();
				ls.des_base_pos.norm_offset.AddThick(-pLinePart1->handle,TRUE);
			}
			ls.DianQuan.offset=pLineFlat1->GetThick();	//垫圈位置
			if(pAngle2&&IsNormSameToPlank(pAngle2,ls.get_norm())==0)
				dist-=pLineFlat1->GetThick();
			else if(pLineFlat2)
			{
				if(pLineFlat1->WorkPlaneNorm()*pLineFlat2->WorkPlaneNorm()>0&&ls_norm*pLineFlat1->WorkPlaneNorm()>0)
					dist-=pLineFlat1->GetThick();
				else if(pLineFlat1->WorkPlaneNorm()*pLineFlat2->WorkPlaneNorm()>0&&ls_norm*pLineFlat1->WorkPlaneNorm()<0)
					dist-=pLineFlat2->GetThick();
				else 
					dist=dist-pLineFlat1->GetThick()-pLineFlat2->GetThick();
			}
		}
		ls.ucs.origin=ls_pos;
		ls.EmptyL0DesignPara();
		ls.AddL0Thick(pLinePart1->handle,TRUE);
		ls.AddL0Thick(pLinePart2->handle,TRUE);
		//
		CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->CopyProperty(&ls);
		pLs->SetLayer(pNode->layer());
		pLs->iSeg=pNode->iSeg;
		pLs->cfgword=pLinePart1->cfgword;	//调整螺栓配材号与基准构件配材号一致
		dist=ftoi(dist);	//为减少前后不一致，统一将距离圆整为整数再判断垫圈 wjh-2019.9.2
		int nType=pLs->CalPadPara(dist); //0.垫板 1.垫圈
		int nMaxWasherThick=CLDSPart::library->GetBoltPadThick(pLs->d,1);
		double dfMinPromptSpaceByDianBan=nMaxWasherThick*2+1.2;
		if (nType==DIANQUAN_STRU::DIANBAN||dist>=dfMinPromptSpaceByDianBan||
			(g_sysPara.b1BoltDesignPrompt&&dist>0))	//防止大间隙（如10)时在无提示情况下设计成3个垫圈 wjh-2019.7.24
			DesignDianBan(pLs,bThrowError,pNode,dist,DIANQUAN_STRU::DIANBAN,pDlg);	//保鸡段炜说>8一般垫板情况较多，故还是默认为垫板更好 wjh-2019.9.2
		pLs->DianQuan.offset=pLinePart1->GetThick();
		if(dist>0)
		{	//带垫圈时不需要添加dist wht 11-07-08
			if(!(pLs->DianQuan.N>0&&pLs->DianQuan.thick>0))
				pLs->AddL0Thick(ftoi(dist),FALSE);
		}
		//螺栓位置设计信息填充
		pLs->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
		pLs->des_base_pos.hPart=pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pLinePart1->handle;
		pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pLinePart2->handle;
		if(pAngle1)
		{
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=jgzj1.g;
		}
		if(pAngle2)
		{
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4; //自定义
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=jgzj2.g;
		}
		pLs->des_base_pos.norm_offset=ls.des_base_pos.norm_offset;
		pLs->des_work_norm=ls.des_work_norm;
		UnifyAngleBoltParamG(pLs->des_base_pos);
		pLs->correct_worknorm();
		pLs->correct_pos();
		pLs->CalGuigeAuto();
		if(UI::blEnableIntermediateUpdateUI)
		{
			pLs->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
		}
		pLinePart1->AppendMidLsRef(pLs->GetLsRef());
		pLinePart2->AppendMidLsRef(pLs->GetLsRef());
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,ls.get_d());
		for(CLDSLinePart* pLinePart=(CLDSLinePart*)jgset.GetFirst();pLinePart;pLinePart=(CLDSLinePart*)jgset.GetNext())
		{
			if(!pLinePart->IsAngle())
				continue;
			CLDSLineAngle *pJg=(CLDSLineAngle*)pLinePart;
			if(pJg->pStart==pNode)
			{
				pJg->SetStartOdd(LsSpace.EndSpace);
				pJg->AppendStartLsRef(pLs->GetLsRef());
			}
			else 
			{
				pJg->SetEndOdd(LsSpace.EndSpace);
				pJg->AppendEndLsRef(pLs->GetLsRef());
			}
			pJg->SetModified();
		}
	}
	catch (CString sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	catch (char *sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}
BOOL DesignIntersNode2Ls(CLDSNode *pNode, BOOL bThrowError)
{
	CLogErrorLife logErrLife;
	CUndoOperObject undo(&Ta, true);
	if (pNode == NULL || pNode->m_cPosCalType != 4)
	{
		if (bThrowError)
			logerr.LevelLog(1, "选择节点不是交叉节点");
		return FALSE;
	}
	if (theApp.m_bCooperativeWork && !theApp.IsHasModifyPerm(pNode->dwPermission))
	{
		if (bThrowError)
			logerr.LevelLog(1, "没有此节点(0X%X)的修改权限！",pNode->handle);
		return FALSE;
	}
	CLDSLineAngle* pCrossJg1 = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[0], CLS_LINEANGLE);
	CLDSLineAngle* pCrossJg2 = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[1], CLS_LINEANGLE);
	if (pCrossJg1 == NULL || pCrossJg2 == NULL)
	{
		if (bThrowError)
			logerr.LevelLog(1, "选择节点(0X%X)缺少依附构件！", pNode->handle);
		return FALSE;
	}
	//检查该交叉点是否已经存在交叉点螺栓
	for (int i = 0; i < 2; i++)
	{
		CLDSLineAngle* pJg = (i == 0) ? pCrossJg1 : pCrossJg2;
		if (pJg == NULL)
			continue;
		for (CLsRef *pLsRef = pJg->GetFirstLsRef(); pLsRef; pLsRef = pJg->GetNextLsRef())
		{
			if ((*pLsRef)->des_base_pos.datumPoint.datum_pos_style == 3)	
			{	//两角钢交叉点定位
				long hDatum1 = (*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
				long hDatum2 = (*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
				if ((hDatum1 == pCrossJg1->handle&&hDatum2 == pCrossJg2->handle) ||
					(hDatum2 == pCrossJg1->handle&&hDatum1 == pCrossJg2->handle))
				{
					if(bThrowError)
						logerr.LevelLog(3, "已存在0x%X交叉点螺栓，不能重复设计!", (*pLsRef)->handle);
					return FALSE;
				}
			}
		}
	}
	//根据交叉杆件及吊杆确定螺栓规格(直径)
	LINEPARTSET jgset;
	int ls_d = pCrossJg1->connectStart.d;
	for (CLDSLinePart *pLinePart = Ta.EnumRodFirst(); pLinePart; pLinePart = Ta.EnumRodNext())
	{
		if (pLinePart->pStart == pNode || pLinePart->pEnd == pNode)
		{
			if (pLinePart->pStart == pNode)
				ls_d = __min(ls_d, pLinePart->connectStart.d);
			else if (pLinePart->pEnd == pNode)
				ls_d = __min(ls_d, pLinePart->connectEnd.d);
			jgset.append(pLinePart);
		}
	}
	//设定螺栓位置(法线偏移量)
	f3dLine jg1_axis_g1, jg1_axis_g2, jg2_axis_g1, jg2_axis_g2;
	BYTE ciWorkWing1 = 0, ciWorkWing2 = 0;
	JGZJ jgzj1,jgzj2;
	f3dPoint work_norm;
	GetWorkNorm(pCrossJg1, pCrossJg2, &work_norm);
	IsInsideJg(pCrossJg1, work_norm, &ciWorkWing1);
	getjgzj(jgzj1, pCrossJg1->GetWidth());
	if (pCrossJg1->m_bEnableTeG)
		jgzj1 = (ciWorkWing1 == 0) ? pCrossJg1->xWingXZhunJu:pCrossJg1->xWingYZhunJu;
	if (jgzj1.g1 == 0 || jgzj1.g2 == 0)
	{
		if (bThrowError)
			logerr.LevelLog(1, "交叉节点的依赖角钢(0X%X)不满足布置双螺栓!", pCrossJg1->handle);
		return FALSE;
	}
	f3dPoint wing_vec1 = (ciWorkWing1 == 0) ? pCrossJg1->GetWingVecX() : pCrossJg1->GetWingVecY();
	jg1_axis_g1.startPt = pCrossJg1->Start() + wing_vec1 * jgzj1.g1;
	jg1_axis_g1.endPt = pCrossJg1->End() + wing_vec1 * jgzj1.g1;
	jg1_axis_g2.startPt = pCrossJg1->Start() + wing_vec1 * jgzj1.g2;
	jg1_axis_g2.endPt = pCrossJg1->End() + wing_vec1 * jgzj1.g2;
	//
	getjgzj(jgzj2, pCrossJg2->GetWidth());
	IsInsideJg(pCrossJg2, work_norm, &ciWorkWing2);
	if (pCrossJg2->m_bEnableTeG)
		jgzj2 = (ciWorkWing2 == 0) ? pCrossJg2->xWingXZhunJu : pCrossJg2->xWingYZhunJu;
	if (jgzj2.g1 == 0 || jgzj2.g2 == 0)
	{
		if (bThrowError)
			logerr.LevelLog(1, "交叉节点的依赖角钢(0X%X)不满足布置双螺栓!", pCrossJg2->handle);
		return FALSE;
	}
	f3dPoint wing_vec2 = (ciWorkWing2 == 0) ? pCrossJg2->GetWingVecX() : pCrossJg2->GetWingVecY();
	jg2_axis_g1.startPt = pCrossJg2->Start() + wing_vec2 * jgzj2.g1;
	jg2_axis_g1.endPt = pCrossJg2->End() + wing_vec2 * jgzj2.g1;
	jg2_axis_g2.startPt = pCrossJg2->Start() + wing_vec2 * jgzj2.g2;
	jg2_axis_g2.endPt = pCrossJg2->End() + wing_vec2 * jgzj2.g2;
	double dist = DistOf3dll(jg1_axis_g1, jg2_axis_g1);	
	f3dPoint pickPt = pCrossJg1->Start(), intersPt1, intersPt2;
	project_point(jg1_axis_g1.startPt, pickPt, work_norm);
	project_point(jg1_axis_g1.endPt, pickPt, work_norm);
	project_point(jg1_axis_g2.startPt, pickPt, work_norm);
	project_point(jg1_axis_g2.endPt, pickPt, work_norm);
	project_point(jg2_axis_g1.startPt, pickPt, work_norm);
	project_point(jg2_axis_g1.endPt, pickPt, work_norm);
	project_point(jg2_axis_g2.startPt, pickPt, work_norm);
	project_point(jg2_axis_g2.endPt, pickPt, work_norm);
	if (Int3dll(jg1_axis_g1, jg2_axis_g2, intersPt1) < 1 || Int3dll(jg1_axis_g2, jg2_axis_g1,intersPt2) < 1)
	{
		if (bThrowError)
			logerr.LevelLog(1, "交点坐标计算错误");
		return FALSE;
	}
	//添加螺栓
	BOLTSET boltSet;
	for (int i = 0; i < 2; i++)
	{
		CLDSBolt *pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->SetLayer(pNode->layer());
		pLs->iSeg = pNode->iSeg;
		pLs->cfgword = pCrossJg1->cfgword;	//调整螺栓配材号与基准构件配材号一致
		pLs->set_d(ls_d);
		pLs->AddL0Thick(pCrossJg1->handle, TRUE);
		pLs->AddL0Thick(pCrossJg2->handle, TRUE);
		//螺栓位置设计信息填充
		pLs->des_base_pos.hPart = pCrossJg1->handle;
		pLs->des_base_pos.datumPoint.datum_pos_style = 3;	//心线交点
		pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pCrossJg1->handle;
		pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pCrossJg2->handle;
		if (i == 0)
		{
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 = 4;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj1.g1;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2 = 4;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj2.g2;
		}
		else
		{
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 = 4;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj1.g2;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2 = 4;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj2.g1;
		}
		pLs->des_base_pos.norm_offset.AddThick(-pCrossJg1->handle, TRUE);
		pLs->des_work_norm.norm_style = 1;
		pLs->des_work_norm.hVicePart = pCrossJg1->handle;
		pLs->des_work_norm.norm_wing = ciWorkWing1;
		pLs->des_work_norm.direction = 0;
		UnifyAngleBoltParamG(pLs->des_base_pos);
		pLs->correct_worknorm();
		pLs->correct_pos();
		pLs->CalGuigeAuto();
		if (UI::blEnableIntermediateUpdateUI)
		{
			pLs->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
		}
		//
		pCrossJg1->AppendMidLsRef(pLs->GetLsRef());
		pCrossJg2->AppendMidLsRef(pLs->GetLsRef());
		boltSet.append(pLs);
	}
	//添加连接板
	if (jgset.GetNodeNum() <= 0 && ftoi(dist) > 6)
	{	//无吊杆时设计垫板
		CLDSView* pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
		pView->DesignBoltPad(pCrossJg1, NULL, boltSet, ftoi(dist), 0);
	}
	else if (jgset.GetNodeNum() > 0 && ftoi(dist) > 6)
	{	//有吊杆时设计内部交叉板
		CLDSPlate* pPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pPlate->cfgword = pNode->cfgword;
		pPlate->jdb_style = 0;
		pPlate->face_N = 1;
		pPlate->Thick = ftol(dist);
		pPlate->designInfo.iProfileStyle0123 = 1;
		pPlate->designInfo.iFaceType = 1;
		pPlate->designInfo.m_bEnableFlexibleDesign = TRUE;
		pPlate->designInfo.m_hBaseNode = pNode->handle;
		pPlate->designInfo.m_hBasePart = pCrossJg1->handle;
		pPlate->designInfo.norm.norm_style = 1;
		pPlate->designInfo.norm.hVicePart = pCrossJg1->handle;
		pPlate->designInfo.norm.norm_wing = ciWorkWing1;
		pPlate->designInfo.norm.direction = 0;
		pPlate->designInfo.origin.datum_pos_style = 3;
		pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1 = pCrossJg1->handle;
		pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2 = pCrossJg2->handle;
		//计算钢板坐标系
		pNode->GetIntersZhun(&pPlate->ucs.origin);
		pPlate->ucs.axis_z = (ciWorkWing1 == 0) ? pCrossJg1->vxWingNorm : pCrossJg1->vyWingNorm;
		f3dPoint perp;
		SnapPerp(&perp, pCrossJg1->Start(), pCrossJg1->End(), pPlate->ucs.origin, NULL);
		project_point(pPlate->ucs.origin, perp, pPlate->ucs.axis_z);
		pPlate->ucs.axis_y = (pCrossJg1->End() - pCrossJg1->Start()).normalized();
		pPlate->ucs.axis_x = pPlate->ucs.axis_y^pPlate->ucs.axis_z;
		pPlate->ucs.axis_y = pPlate->ucs.axis_z^pPlate->ucs.axis_x;
		normalize(pPlate->ucs.axis_x);
		normalize(pPlate->ucs.axis_y);
		normalize(pPlate->ucs.axis_z);
		//添加连接杆件设计参数
		CDesignLjPartPara* pLinePartPara = NULL;
		for (int i = 0; i < 2; i++)
		{
			CLDSLineAngle* pJg = (i == 0) ? pCrossJg1 : pCrossJg2;
			BYTE ciWorkWing = (i == 0) ? ciWorkWing1 : ciWorkWing2;
			pLinePartPara = pPlate->designInfo.partList.Add(pJg->handle);	//交叉角钢1
			pLinePartPara->hPart = pJg->handle;
			pLinePartPara->m_nClassTypeId = CLS_LINEANGLE;
			pLinePartPara->iFaceNo = 1;
			pLinePartPara->start0_end1 = 2;
			pLinePartPara->angle.bEndLjJg = FALSE;
			pLinePartPara->angle.cur_wing_x0_y1 = ciWorkWing;
			pLinePartPara->angle.bTwoEdge = TRUE;
			pLinePartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			pLinePartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			pLinePartPara->angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
		}
		for (CLDSLinePart *pRod = jgset.GetFirst(); pRod; pRod = jgset.GetNext())
		{
			if (!pRod->IsAngle())
				continue;
			//计算正负头
			CLDSLineAngle *pJg = (CLDSLineAngle*)pRod;
			BYTE ciS0_E1 = (pRod->pStart == pNode) ? 0 : 1;
			DESIGNODDMENT* pOddDes = (pRod->pStart == pNode) ? &pJg->desStartOdd : &pJg->desEndOdd;
			pOddDes->m_fCollideDist = g_sysPara.VertexDist;
			pOddDes->m_iOddCalStyle = 0;
			pOddDes->m_hRefPart1 = pCrossJg1->handle;
			pOddDes->m_hRefPart2 = pCrossJg2->handle;
			pJg->CalStartOddment();
			pJg->CalEndOddment();
			//布置螺栓
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace, ls_d);
			BYTE ciWorkWing = 0;
			IsInsideJg(pJg, work_norm, &ciWorkWing);
			CLDSBolt *pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->SetLayer(pNode->layer());
			pLs->iSeg = pNode->iSeg;
			pLs->cfgword = pCrossJg1->cfgword;
			pLs->set_d(ls_d);
			pLs->des_base_pos.hPart = pJg->handle;
			pLs->des_base_pos.offset_wing = ciWorkWing;
			pLs->des_base_pos.m_biWingOffsetStyle = 0;
			pLs->des_base_pos.len_offset_dist = LsSpace.EndSpace;
			pLs->des_base_pos.direction = ciS0_E1;
			//螺栓位置设计信息填充
			pLs->des_base_pos.datumPoint.datum_pos_style = 1;	//角钢楞点
			pLs->des_base_pos.datumPoint.des_para.RODEND.hRod = pJg->handle;
			pLs->des_base_pos.datumPoint.des_para.RODEND.direction = ciS0_E1;
			pLs->des_base_pos.datumPoint.des_para.RODEND.offset_wing = ciWorkWing;
			pLs->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style = 4;
			pLs->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist = 0;
			pLs->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect = true;
			pLs->des_work_norm.norm_style = 1;
			pLs->des_work_norm.hVicePart = pJg->handle;
			pLs->des_work_norm.norm_wing = ciWorkWing;
			pLs->des_work_norm.direction = 0;
			pLs->AddL0Thick(pJg->handle, TRUE);
			if (ciS0_E1 == 0)
				pJg->AppendStartLsRef(pLs->GetLsRef());
			else
				pJg->AppendEndLsRef(pLs->GetLsRef());
			boltSet.append(pLs);
			//加入钢板连接杆件参数中
			pLinePartPara = pPlate->designInfo.partList.Add(pJg->handle);
			pLinePartPara->hPart = pJg->handle;
			pLinePartPara->m_nClassTypeId = CLS_LINEANGLE;
			pLinePartPara->iFaceNo = 1;
			pLinePartPara->start0_end1 = ciS0_E1;
			pLinePartPara->angle.bEndLjJg = FALSE;
			pLinePartPara->angle.cur_wing_x0_y1 = ciWorkWing;
			pLinePartPara->angle.bTwoEdge = FALSE;
			pLinePartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			pLinePartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			pLinePartPara->angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
		}
		//初始化钢板螺栓信息
		for (CLDSBolt* pBolt = boltSet.GetFirst(); pBolt; pBolt=boltSet.GetNext())
		{
			pBolt->AddL0Thick(pPlate->handle);
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			if (UI::blEnableIntermediateUpdateUI)
			{
				pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
			//
			pPlate->AppendLsRef(pBolt->GetLsRef());
		}
		//设计钢板
		pPlate->DesignPlate();
		if (UI::blEnableIntermediateUpdateUI)
		{
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		}
	}
	return TRUE;
}
BOOL ValidateOneBoltConnect(CLDSNode *pNode,CLDSLinePart *pLineRod1,CLDSLinePart *pLineRod2)
{
	int n1=1,n2=1;
	if(pLineRod1)
	{
		if(pLineRod1->pStart==pNode)
			n1=pLineRod1->connectStart.wnConnBoltN;
		else if(pLineRod1->pEnd==pNode)
			n1=pLineRod1->connectEnd.wnConnBoltN;
	}
	else
		return FALSE;
	if(pLineRod2)
	{
		if(pLineRod2->pStart==pNode)
			n2=pLineRod2->connectStart.wnConnBoltN;
		else if(pLineRod2->pEnd==pNode)
			n2=pLineRod2->connectEnd.wnConnBoltN;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if((n1!=1||n2!=1)&&AfxMessageBox("The number of ray part's end bolts is not 1,if continue?",MB_YESNO)!=IDYES)
#else 
	if((n1!=1||n2!=1)&&AfxMessageBox("射线材端头螺栓数不为1,是否继续?",MB_YESNO)!=IDYES)
#endif
		return FALSE;
	else
		return TRUE;
}
//2.设计单螺栓端连接
//设计仅有螺栓的连接(无板)
BOOL DesignBoltOnlyConnect(CLDSNode *pNode,CLDSLinePart *pXieRod,CLDSLinePart *pXieRod2,CLDSLinePart *pXieRod3,
								   BOOL bThrowError,BOOL bCanUndo/*=TRUE*/)
{
	//交叉点单螺栓连接设计
	CLDSLineAngle *pXieJg=NULL,*pXieJg2=NULL;
	CLDSLineFlat *pXieFlat=NULL,*pXieFlat2=NULL;
	CLDSLineSlot* pXieSlot=NULL,*pXieSlot2=NULL;
	if(pXieRod&&pXieRod->IsAngle())
		pXieJg=(CLDSLineAngle*)pXieRod;
	else if(pXieRod&&pXieRod->GetClassTypeId()==CLS_LINEFLAT)
		pXieFlat=(CLDSLineFlat*)pXieRod;
	else if(pXieRod&&pXieRod->GetClassTypeId()==CLS_LINESLOT)
		pXieSlot=(CLDSLineSlot*)pXieRod;
	else	//暂不支持该种类型的构件单螺栓连接设计
		return FALSE;
	if(pXieRod==NULL&&pXieRod2==NULL)
		return DesignIntersNode(pNode,bThrowError);

	CLDSLineAngle* arrRayAngles[3]={NULL};
	int index=0;
	if(pXieRod&&pXieRod->IsAngle())
		arrRayAngles[index++]=(CLDSLineAngle*)pXieRod;
	if(pXieRod2&&pXieRod2->IsAngle())
		arrRayAngles[index++]=(CLDSLineAngle*)pXieRod2;
	if(pXieRod3&&pXieRod3->IsAngle())
		arrRayAngles[index++]=(CLDSLineAngle*)pXieRod3;
	if(arrRayAngles[2]!=NULL)
	{	//三根射线角钢如果存在里外铁角钢交于同一点时要保证前两根角钢为交于同一点的里外铁角钢 wjh-2017.3.2
		typedef CMultiOffsetPos* CMultiOffsetPosPtr;
		CMultiOffsetPosPtr desPosArr[3]={	arrRayAngles[0]->pStart->handle==pNode->handle?&arrRayAngles[0]->desStartPos:&arrRayAngles[0]->desEndPos,
											arrRayAngles[1]->pStart->handle==pNode->handle?&arrRayAngles[1]->desStartPos:&arrRayAngles[1]->desEndPos,
											arrRayAngles[2]->pStart->handle==pNode->handle?&arrRayAngles[2]->desStartPos:&arrRayAngles[2]->desEndPos};
		if(fabs(desPosArr[1]->len_offset_dist-desPosArr[2]->len_offset_dist)<EPS)
		{	//第二根与第三根交于同一点
			CLDSLineAngle* pTempAngle=arrRayAngles[0];
			memmove(arrRayAngles,&arrRayAngles[1],sizeof(CLDSLineAngle*)*2);
			arrRayAngles[2]=pTempAngle;
		}
		else if(fabs(desPosArr[0]->len_offset_dist-desPosArr[2]->len_offset_dist)<EPS)
		{	//第一根与第三根交于同一点
			CLDSLineAngle* pTempAngle=arrRayAngles[1];
			arrRayAngles[1]=arrRayAngles[2];
			arrRayAngles[2]=pTempAngle;
		}
		pXieRod =arrRayAngles[0];
		pXieRod2=arrRayAngles[1];
		pXieRod3=arrRayAngles[2];
	}
	if(  pXieRod&&(
		(pXieRod->pStart==pNode&&pXieRod->GetLocalLsCount(1,100)>0)||
		(pXieRod->pEnd==pNode&&pXieRod->GetLocalLsCount(2,100)>0)))
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X杆件已进行端头螺栓连接设计，跳过单螺栓设计!",pXieRod->handle);
		return false;
	}
	if(  pXieRod2&&(
		(pXieRod2->pStart==pNode&&pXieRod2->GetLocalLsCount(1,100)>0)||
		(pXieRod2->pEnd==pNode&&pXieRod2->GetLocalLsCount(2,100)>0)))
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X杆件已进行端头螺栓连接设计，跳过单螺栓设计!",pXieRod2->handle);
		return false;
	}
	if(  pXieRod3&&(
		(pXieRod3->pStart==pNode&&pXieRod2->GetLocalLsCount(1,100)>0)||
		(pXieRod3->pEnd==pNode&&pXieRod2->GetLocalLsCount(2,100)>0)))
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X杆件已进行端头螺栓连接设计，跳过单螺栓设计!",pXieRod3->handle);
		return false;
	}

	SmartPartPtr pFatherRod;
	JGZJ jgzj;
	CLDSBolt ls(console.GetActiveModel()),*pBolt;
	f3dPoint norm,direct,ls_pos;
	BOOL bInsideJg1,bInsideJg2;
	double len_offset1=0,len_offset2=0;
	int x_wing0_y_wing1;
	LSSPACE_STRU LsSpace;
	if(pNode==NULL||pXieRod==NULL)
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The select part is illegal";
#else 
			throw "选择非法构件错误";
#endif
		else
			return FALSE;
	}
	if(pXieRod->pStart==NULL||pXieRod->pEnd==NULL)
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The selected part is short angle(start or end node is 0X0)！";
#else 
			throw "所选杆件为短角钢(始端或终端节点为0X0)！";
#endif
		else
			return FALSE;
	}
	if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Without modify permission of the node！";
#else 
			throw "没有此节点的修改权限！";
#endif
		else
			return FALSE;
	}
	if(theApp.m_bCooperativeWork&&pNode==pXieRod->pStart&&!theApp.IsHasModifyPerm(pXieRod->dwStartPermission))
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Don't have modify permission of rod's start！";
#else 
			throw "没有此杆件始端的修改权限！";
#endif
		else 
			return FALSE;
	}
	else if(theApp.m_bCooperativeWork&&pNode==pXieRod->pEnd&&!theApp.IsHasModifyPerm(pXieRod->dwEndPermission))
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Don't have modify permission of rod's end！";
#else 
			throw "没有此杆件终端的修改权限！";
#endif
		else
			return FALSE;
	}
	CLDSLineAngle* pXieAngle=NULL;
	if(pXieRod&&pXieRod->GetClassTypeId()==CLS_LINEANGLE)
		pXieAngle=(CLDSLineAngle*)pXieRod;
	//增加对手动指定端节点依附基准角钢的情况下端螺栓布置 wjh-2015.1.25
	if(pXieAngle&&pXieAngle->pStart==pNode&&pXieAngle->desStartPos.datum_jg_h>0x20)
		pFatherRod = console.FromPartHandle(pXieAngle->desStartPos.datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	else if(pXieAngle&&pXieAngle->pEnd==pNode&&pXieAngle->desEndPos.datum_jg_h>0x20)
		pFatherRod = console.FromPartHandle(pXieAngle->desEndPos.datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	else
		pFatherRod = console.FromPartHandle(pNode->hFatherPart);
	CLsRef* pLsRef;
	if(pFatherRod.IsHasPtr()&&pXieRod)
	{
		if(!pXieRod->cfgword.And(pFatherRod->cfgword))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0X%X and its end node dependent rod0X%X doesn't live in any model，bolt-only design failed!",pXieRod->handle,pFatherRod->handle);
#else
			logerr.Log("0X%X与其设计端节点父杆件0X%X不共存于任何一个呼高模型中，单螺栓设计失败!",pXieRod->handle,pFatherRod->handle);
#endif
			return FALSE;
		}
		for(pLsRef=pXieRod->GetFirstLsRef();pLsRef;pLsRef=pXieRod->GetNextLsRef())
		{
			if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==3)	//两角钢交叉点定位
			{
				long hDatum1=(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
				long hDatum2=(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
				long hRod2=pXieRod2!=NULL?pXieRod2->handle:0;
				if( (hDatum1==pXieRod->handle&&hDatum2==pFatherRod->handle)||
					(hDatum2==pXieRod->handle&&hDatum1==pFatherRod->handle)||
					(hDatum1==hRod2&&hDatum2==pFatherRod->handle)||(hDatum2==hRod2&&hDatum1==pFatherRod->handle))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					logerr.Log("End bolt 0x%X already exists and does not allow the repeat design!",(*pLsRef)->handle);
#else
					logerr.Log("已存在0x%X端点单螺栓，不能重复设计!",(*pLsRef)->handle);
#endif
					return FALSE;
				}
			}
		}
	}
	if(pFatherRod.IsNULL())
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The selected node is isolated(not datum angle's node)";
#else 
			throw "选择了一个孤立的节点(没有基准角钢的节点)";
#endif
		else
			return FALSE;
	}
	CLDSGroupLineAngle *pGroupJg=NULL;	//节点父角钢
	if(pFatherRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//节点父杆件为组合角钢时自动选择合适的子角钢为单螺栓连接基准角钢 wht 10-11-23
		pGroupJg=pFatherRod.pGroupAngle;
		if(pGroupJg->group_style!=0)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Currently only support single-bolt-connection on combined angle's child angle!";
#else 
				throw "目前仅支持在对角组合角钢的子角钢上进行单螺栓连接!";
#endif
			else
				return FALSE;
		}
		f3dPoint ray_vec=pXieRod->Start()-pXieRod->End();
		if(pXieRod->pStart->handle==pNode->handle)
			ray_vec*=-1.0;
		normalize(ray_vec);
		CLDSLineAngle *pSonJg=NULL;
		if(pXieJg)
			GetWorkNorm(pGroupJg,pXieJg,&norm);	//工作面法线
		else
			norm=(pXieFlat!=NULL ? pXieFlat->WorkPlaneNorm() : pXieSlot->WorkPlaneNorm());

		for(int i=0;i<4;i++)
		{
			if(pGroupJg->group_style<2&&i>=2)
				continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在 wht 11-08-05
			if(pGroupJg->son_jg_h[i]<0x20)
				continue;
			CLDSLineAngle *pJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
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
			pFatherRod=pSonJg;
		else 
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Datum parent rod doesn't support the connection!";
#else 
				throw "基点父杆件不支持该连接!";
#endif
			else
				return FALSE;
		}
	}
	g_pSolidDraw->SetEntSnapStatus(pNode->handle);
	g_pSolidDraw->SetEntSnapStatus(pXieRod->handle);
	if(bCanUndo)	//支持撤销操作
		Ta.BeginUndoListen();	//开始监听
	if(pXieRod->IsAngle())
	{
		pXieJg=(CLDSLineAngle*)pXieRod;
		GetWorkNorm(pFatherRod.pAngle,pXieJg,&norm);
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
		if(pXieJg->pStart == pNode)
		{
			len_offset1=pXieJg->desStartPos.len_offset_dist;
			ls_pos=pXieJg->GetStartDatumPos();
			GetLsSpace(LsSpace,pXieJg->connectStart.d);
			pXieJg->SetStartOdd(LsSpace.EndSpace);
			pXieJg->desStartOdd.m_iOddCalStyle=2;
			if(pXieJg->desStartPos.jgber_cal_style==2)	//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定模式 wjh-2016.3.24
				pXieJg->desStartPos.bUdfDatumPointMode=TRUE;
			if(bInsideJg1)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg->desStartPos.wing_x_offset.gStyle=4;
					pXieJg->desStartPos.wing_x_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg->desStartPos.wing_y_offset.gStyle=4;
					pXieJg->desStartPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg->desStartPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				pXieJg->ClearFlag();
				pXieJg->CalPosition();
				Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*
					(pFatherRod->GetThick()+pXieJg->GetThick()));
				sprintf(ls.des_base_pos.norm_offset.key_str,"-0X%X,-0X%X",pFatherRod->handle,pXieJg->handle);
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
				Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherRod->GetThick());
				ls.des_base_pos.norm_offset.AddThick(-pFatherRod->handle,TRUE);
			}
			ls.set_d(pXieJg->connectStart.d);
			//上移添加螺栓通厚的位置 wht 10-11-04
			ls.AddL0Thick(pFatherRod->handle,TRUE);
			ls.AddL0Thick(pXieJg->handle,TRUE);
			if(!ls.CalGuigeAuto())
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find bolt specification M%dX%.f in specification library",pXieJg->connectStart.d,ls.get_L0());
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
			pBolt->CopyModuleInstanceInfo(pNode);
			pBolt->iSeg=pXieJg->iSeg;
			pBolt->layer(2)=pXieJg->layer(2);	//调整螺栓图层名
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieJg->UnifiedCfgword();	//调整螺栓配材号与基准构件配材号一致
			//螺栓位置设计\工作法线设计信息填充
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
			if(pXieJg->desStartPos.IsInDatumJgWingX())	//搭于父角钢X肢
			{
				pBolt->des_work_norm.norm_wing=0;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pNode->hFatherPart==pFatherRod->handle)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=(short)pNode->xFatherAngleZhunJu.offset_X_dist_style;
				else
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingXG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			else									//搭于父角钢Y肢
			{
				pBolt->des_work_norm.norm_wing=1;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.offset_wing=1;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pNode->hFatherPart==pFatherRod->handle)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=(short)pNode->xFatherAngleZhunJu.offset_Y_dist_style;
				else
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingYG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			if(pFatherRod->IsArcPart())
			{
				pBolt->des_work_norm.norm_style=1;
				pBolt->des_work_norm.hVicePart=pXieJg->handle;
				pBolt->des_work_norm.norm_wing=x_wing0_y_wing1;
			}
			else //if(pFatherRod->IsAngle())
			{
				pBolt->des_work_norm.norm_style=1;	//指定角钢肢法线方向
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
			}
			//pBolt->des_base_pos.hNode = pNode->handle;
			pBolt->des_base_pos.norm_offset=ls.des_base_pos.norm_offset;

			if(!pFatherRod->IsLinePart())
				pFatherRod->AppendLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pStart==pNode)
				pFatherRod.pRod->AppendStartLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pEnd==pNode)
				pFatherRod.pRod->AppendEndLsRef(pBolt->GetLsRef());
			else
				pFatherRod.pRod->AppendMidLsRef(pBolt->GetLsRef());
			pXieJg->AppendStartLsRef(pBolt->GetLsRef());
			if(pFatherRod->IsAngle())
				pXieJg->CalCutAngleInfo(pFatherRod.pAngle,NULL,true,NULL);
			pXieJg->SetModified();
		}
		else if(pXieJg->pEnd == pNode)
		{
			len_offset1=pXieJg->desStartPos.len_offset_dist;
			GetLsSpace(LsSpace,pXieJg->connectEnd.d);
			pXieJg->SetEndOdd(LsSpace.EndSpace);
			pXieJg->desEndOdd.m_iOddCalStyle=2;
			if(pXieJg->desEndPos.jgber_cal_style==2)	//简单定位时，为避免后续自动判断改变基点定位类型，改为人工指定模式 wjh-2016.3.24
				pXieJg->desEndPos.bUdfDatumPointMode=TRUE;
			ls_pos=pXieJg->GetEndDatumPos();
			if(bInsideJg1)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg->desEndPos.wing_x_offset.gStyle=4;
					pXieJg->desEndPos.wing_x_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg->desEndPos.wing_y_offset.gStyle=4;
					pXieJg->desEndPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg->desEndPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				pXieJg->ClearFlag();
				pXieJg->CalPosition();
				Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*
					(pFatherRod->GetThick()+pXieJg->GetThick()));
				sprintf(ls.des_base_pos.norm_offset.key_str,"-0X%X,-0X%X",pFatherRod->handle,pXieJg->handle);
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
				Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherRod->GetThick());
				ls.des_base_pos.norm_offset.AddThick(-pFatherRod->handle,TRUE);
			}
			//添加螺栓通厚设计参数
			ls.EmptyL0DesignPara();	//清空螺栓通厚设计参数
			ls.AddL0Thick(pFatherRod->handle,TRUE);
			ls.AddL0Thick(pXieJg->handle,TRUE);
			ls.set_d(pXieJg->connectEnd.d);
			if(!ls.CalGuigeAuto())
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find bolt specification M%dX%.f in specification library",pXieJg->connectEnd.d,ls.get_L0());
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
			pBolt->CopyModuleInstanceInfo(pNode);
			pBolt->iSeg=pXieJg->iSeg;
			pBolt->layer(2)=pXieJg->layer(2);	//调整螺栓图层名
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieJg->UnifiedCfgword();	//调整螺栓配材号与基准构件配材号一致
			//螺栓位置设计\工作法线设计信息填充
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
			if(pXieJg->desEndPos.IsInDatumJgWingX())	//搭于父角钢X肢
			{
				pBolt->des_work_norm.norm_wing=0;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pNode->hFatherPart==pFatherRod->handle)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=(short)pNode->xFatherAngleZhunJu.offset_X_dist_style;
				else
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingXG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			else									//搭于父角钢Y肢
			{
				pBolt->des_work_norm.norm_wing=1;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.offset_wing=1;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pNode->hFatherPart==pFatherRod->handle)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=(short)pNode->xFatherAngleZhunJu.offset_Y_dist_style;
				else
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingYG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			if(pFatherRod->IsArcPart())
			{
				pBolt->des_work_norm.norm_style=1;
				pBolt->des_work_norm.hVicePart=pXieJg->handle;
				pBolt->des_work_norm.norm_wing=x_wing0_y_wing1;
			}
			else //if(pFatherRod->IsAngle())
			{
				pBolt->des_work_norm.norm_style=1;	//指定角钢肢法线方向
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
			}
			//pBolt->des_base_pos.hNode = pNode->handle;
			pBolt->des_base_pos.norm_offset=ls.des_base_pos.norm_offset;

			if(!pFatherRod->IsLinePart())
				pFatherRod->AppendLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pStart==pNode)
				pFatherRod.pRod->AppendStartLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pEnd==pNode)
				pFatherRod.pRod->AppendEndLsRef(pBolt->GetLsRef());
			else
				pFatherRod.pRod->AppendMidLsRef(pBolt->GetLsRef());
			pXieJg->AppendEndLsRef(pBolt->GetLsRef());
			if(pFatherRod->IsAngle())
				pXieJg->CalCutAngleInfo(pFatherRod.pAngle,NULL,false,NULL);
			pXieJg->SetModified();
		}
		else
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected ray part doesn't through current node";
#else 
				throw "所选射线材,不通过当前节点";
#endif
			else
				return FALSE;
		}
		pXieJg->SetModified();
		pXieJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pXieJg->GetSolidPartObject());
	}
	else if(pXieRod->GetClassTypeId()==CLS_LINESLOT||pXieRod->GetClassTypeId()==CLS_LINEFLAT)
	{
		norm=(pXieFlat!=NULL ? pXieFlat->WorkPlaneNorm() : pXieSlot->WorkPlaneNorm());
		ls.set_norm(norm);
		if(pXieRod->pStart==pNode)
		{
			GetLsSpace(LsSpace,pXieRod->connectStart.d);
			pXieRod->SetStartOdd(LsSpace.EndSpace);
			pXieRod->desStartOdd.m_iOddCalStyle=2;
			pXieRod->ClearFlag();
			pXieRod->CalPosition();
			ls_pos=pXieRod->Start();
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherRod->GetThick());
			sprintf(ls.des_base_pos.norm_offset.key_str,"-0X%X",pFatherRod->handle);
			ls.set_d(pXieRod->connectStart.d);
			ls.AddL0Thick(pFatherRod->handle,TRUE);
			ls.AddL0Thick(pXieRod->handle,TRUE);
			ls.ucs.origin=ls_pos;
			if(!ls.CalGuigeAuto())
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find bolt specification M%dX%.f in specification library",pXieRod->connectStart.d,ls.get_L0());
#else 
				sprintf(sError,"在螺栓规格库中没有找到符合M%dX%.f的螺栓规格",pXieRod->connectStart.d,ls.L0);
#endif
				if(bThrowError)
					throw sError;
				else
					return FALSE;
			}
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->CopyProperty(&ls);
			pBolt->iSeg=pXieRod->iSeg;
			//pBolt->layer(2)=pXieRod->layer(2);	//调整螺栓图层名
			pBolt->CopyModuleInstanceInfo(pNode);
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieRod->UnifiedCfgword();	//调整螺栓配材号与基准构件配材号一致
			//螺栓位置设计\工作法线设计信息填充
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			//指定构件是扁铁时，不需要肢方向偏移类型和偏移量
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieRod->handle;
			CTubeEndPosPara* pCurrPos=(pXieSlot!=NULL ? &pXieSlot->desStartPos : &pXieFlat->desStartPos);
			if(pCurrPos->IsInDatumJgWingX())	//搭于父角钢X肢
			{
				pBolt->des_work_norm.norm_wing=0;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingXG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			else									//搭于父角钢Y肢
			{
				pBolt->des_work_norm.norm_wing=1;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.offset_wing=1;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingYG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			if(pFatherRod->IsArcPart()||pFatherRod.IsEqualPtr(pXieRod))
			{
				pBolt->des_work_norm.norm_style=5;
				pBolt->des_work_norm.hVicePart = pXieRod->handle;
				if(pXieFlat)		//扁铁法线+
					pBolt->des_work_norm.direction=2;
				else if(pXieSlot)	//槽钢侧向法线+
					pBolt->des_work_norm.direction=4;
			}
			else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
			{
				pBolt->des_work_norm.norm_style=5;	//指定角钢肢法线方向
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
				if(pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
					pBolt->des_work_norm.direction=2;	//扁铁法线+
				else 
					pBolt->des_work_norm.direction=4;	//槽钢侧向法线+
			}
			else //if(pFatherRod->IsAngle())
			{
				pBolt->des_work_norm.norm_style=1;	//指定角钢肢法线方向
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
			}
			if(!pFatherRod->IsLinePart())
				pFatherRod->AppendLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pStart==pNode)
				pFatherRod.pRod->AppendStartLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pEnd==pNode)
				pFatherRod.pRod->AppendEndLsRef(pBolt->GetLsRef());
			else
				pFatherRod.pRod->AppendMidLsRef(pBolt->GetLsRef());
			pXieRod->AppendStartLsRef(pBolt->GetLsRef());
			pXieRod->SetModified();
		}
		else if(pXieRod->pEnd==pNode)
		{
			GetLsSpace(LsSpace,pXieRod->connectEnd.d);
			pXieRod->SetEndOdd(LsSpace.EndSpace);
			pXieRod->desEndOdd.m_iOddCalStyle=2;
			ls_pos=pXieRod->End();
			pXieRod->ClearFlag();
			pXieRod->CalPosition();
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherRod->GetThick());
			ls.des_base_pos.norm_offset.AddThick(-pFatherRod->handle,TRUE);
			ls.EmptyL0DesignPara();	//清空螺栓通厚设计参数
			ls.AddL0Thick(pFatherRod->handle,TRUE);
			ls.AddL0Thick(pXieRod->handle,TRUE);
			ls.ucs.origin=ls_pos;
			ls.set_d(pXieRod->connectEnd.d);
			if(!ls.CalGuigeAuto())
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find bolt specification M%dX%.f in specification library",pXieRod->connectEnd.d,ls.get_L0());
#else 
				sprintf(sError,"在螺栓规格库中没有找到符合M%dX%.f的螺栓规格",pXieRod->connectEnd.d,ls.L0);
#endif
				if(bThrowError)
					throw sError;
				else
					return FALSE;
			}
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->CopyProperty(&ls);
			pBolt->iSeg=pXieRod->iSeg;
			//pBolt->layer(2)=pXieRod->layer(2);	//调整螺栓图层名
			pBolt->CopyModuleInstanceInfo(pNode);
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieRod->UnifiedCfgword();	//调整螺栓配材号与基准构件配材号一致
			//螺栓位置设计\工作法线设计信息填充
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//心线交点
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			//指定构件是扁铁时，不需要肢方向偏移类型和偏移量
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieRod->handle;
			CTubeEndPosPara* pCurrPos=(pXieSlot!=NULL ? &pXieSlot->desEndPos : &pXieFlat->desEndPos);
			if(pCurrPos->IsInDatumJgWingX())	//搭于父角钢X肢
			{
				pBolt->des_work_norm.norm_wing=0;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingXG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			else									//搭于父角钢Y肢
			{
				pBolt->des_work_norm.norm_wing=1;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.offset_wing=1;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pFatherRod->IsAngle())
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod.pAngle->GetNodeWingYG(pNode));
				else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_ARCSLOT)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetHeight()/2);
				else//
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherRod->GetWidth()/2);
			}
			if(pFatherRod->IsArcPart()||pFatherRod.IsEqualPtr(pXieRod))
			{
				pBolt->des_work_norm.norm_style=5;
				pBolt->des_work_norm.hVicePart = pXieRod->handle;
				if(pXieFlat)		//扁铁法线+
					pBolt->des_work_norm.direction=2;
				else if(pXieSlot)	//槽钢侧向法线+
					pBolt->des_work_norm.direction=4;
			}
			else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
			{
				pBolt->des_work_norm.norm_style=5;	//指定角钢肢法线方向
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
				if(pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
					pBolt->des_work_norm.direction=2;	//扁铁法线+
				else 
					pBolt->des_work_norm.direction=4;	//槽钢侧向法线+
			}
			else //if(pFatherRod->IsAngle())
			{
				pBolt->des_work_norm.norm_style=1;	//指定角钢肢法线方向
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
			}
			if(!pFatherRod->IsLinePart())
				pFatherRod->AppendLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pStart==pNode)
				pFatherRod.pRod->AppendStartLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pEnd==pNode)
				pFatherRod.pRod->AppendEndLsRef(pBolt->GetLsRef());
			else
				pFatherRod.pRod->AppendMidLsRef(pBolt->GetLsRef());
			pXieRod->AppendEndLsRef(pBolt->GetLsRef());
			pXieRod->SetModified();
		}
		else
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected ray part doesn't through current node";
#else 
				throw "所选射线材,不通过当前节点";
#endif
			else
				return FALSE;
		}
		pXieRod->SetModified();
		pXieRod->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pXieRod->GetSolidPartObject());
	}
	for(index=1;index<3;index++)
	{	//引入螺栓 调整角钢正负头以及角钢摆放位置
		if(arrRayAngles[index])
			pXieRod2=arrRayAngles[index];
		else if(index==2)
			break;
		if(pXieRod2==NULL)
			continue;
		if(pXieRod2->IsAngle())
			pXieJg2=(CLDSLineAngle*)pXieRod2;
		else if(pXieRod2->GetClassTypeId()==CLS_LINEFLAT)
			pXieFlat2=(CLDSLineFlat*)pXieRod2;
		else if(pXieRod2->GetClassTypeId()==CLS_LINESLOT)
			pXieSlot2=(CLDSLineSlot*)pXieRod2;
		if(pXieFlat || pXieFlat2 ||pXieSlot2)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Curent only support angle's single-bolt-connection of three rod.!";
#else 
				throw "三根杆件的单螺栓连接,目前只支持角钢的设计!";
#endif
			else 
				return FALSE;
		}
		if(norm.IsZero())
			GetWorkNorm(pFatherRod.pAngle,pXieJg2,&norm);
		bInsideJg2 = IsInsideJg(pXieJg2,norm,&x_wing0_y_wing1);
		CConnectInfo* pCurrConnBoltInfo=NULL;
		if(pXieRod2->pStart==pNode)
			pCurrConnBoltInfo=&pXieRod2->connectStart;
		else if(pXieRod2->pEnd==pNode)
			pCurrConnBoltInfo=&pXieRod2->connectEnd;
		if(pXieJg2->pStart == pNode)
		{
			len_offset2=pXieJg2->desStartPos.len_offset_dist;
			GetLsSpace(LsSpace,pXieJg2->connectStart.d);
			pXieJg2->SetStartOdd(LsSpace.EndSpace);
			pXieJg2->desStartOdd.m_iOddCalStyle=2;
			if(bInsideJg2)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg2->desStartPos.wing_x_offset.gStyle=4;
					pXieJg2->desStartPos.wing_x_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg2->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desStartPos.wing_y_offset.gStyle=4;
					pXieJg2->desStartPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
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
			if(pFatherRod->IsAngle())
				pXieJg2->CalCutAngleInfo(pFatherRod.pAngle,NULL,true,NULL);
		}
		else if(pXieJg2->pEnd == pNode)
		{
			len_offset2=pXieJg2->desEndPos.len_offset_dist;
			GetLsSpace(LsSpace,pXieJg2->connectEnd.d);
			pXieJg2->SetEndOdd(LsSpace.EndSpace);
			pXieJg2->desEndOdd.m_iOddCalStyle=2;
			if(bInsideJg2)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg2->desEndPos.wing_x_offset.gStyle=4;
					pXieJg2->desEndPos.wing_x_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //节点父角钢为组合角钢 调整斜材偏移量时考虑组合角钢间隙值 wht 10-11-23
						pXieJg2->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desEndPos.wing_y_offset.gStyle=4;
					pXieJg2->desEndPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
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
			if(pFatherRod->IsAngle())
				pXieJg2->CalCutAngleInfo(pFatherRod.pAngle,NULL,false,NULL);
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
			pBolt2->d=pCurrConnBoltInfo->d;
			pBolt2->iSeg=pXieJg2->iSeg;
			//pBolt2->layer(2)=pXieJg2->layer(2);	//调整螺栓图层名
			pBolt2->CopyModuleInstanceInfo(pNode);
			pBolt2->cfgword=pFatherRod->UnifiedCfgword();	//调整螺栓配材号与基准构件配材号一致
			pBolt2->cfgword&=pXieJg2->UnifiedCfgword();
			pBolt2->des_base_pos=pBolt->des_base_pos;
			pBolt2->des_work_norm=pBolt->des_work_norm;
			pBolt2->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg2->handle;
			pBolt2->EmptyL0DesignPara();
			pBolt2->AddL0Thick(pFatherRod->handle,TRUE,TRUE);
			pBolt2->AddL0Thick(pXieJg2->handle,TRUE,TRUE);
			pBolt2->CalGuigeAuto();
			pBolt2->correct_worknorm();
			pBolt2->correct_pos();
			pBolt2->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt2->GetSolidPartObject());
			if(pXieJg2->pStart==pNode)
				pXieJg2->AppendStartLsRef(pBolt2->GetLsRef());
			else if(pXieJg2->pEnd==pNode)
				pXieJg2->AppendEndLsRef(pBolt2->GetLsRef());
			if(!pFatherRod->IsLinePart())
				pFatherRod->AppendLsRef(pBolt->GetLsRef());
			else if(pFatherRod.pRod->pStart==pNode)
				pFatherRod.pRod->AppendStartLsRef(pBolt2->GetLsRef());
			else if(pFatherRod.pRod->pEnd==pNode)
				pFatherRod.pRod->AppendEndLsRef(pBolt2->GetLsRef());
			else
				pFatherRod.pRod->AppendMidLsRef(pBolt2->GetLsRef());
		}
		pXieRod2->SetModified();
		pXieRod2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pXieRod2->GetSolidPartObject());
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
BOOL DesignBoltOnlyConnect(CLDSNode *pNode,CLDSLinePart *pXieRod,CLDSLinePart *pXieRod2,
								   BOOL bThrowError,BOOL bCanUndo/*=TRUE*/)
{
	return DesignBoltOnlyConnect(pNode,pXieRod,pXieRod2,NULL,bThrowError,bCanUndo);
}

//设计单螺栓连接
int CLDSView::DesignOneBolt()
{
	CLDSNode *pNode=NULL;
	CLDSLinePart *pLinePart=NULL,*pSecLinePart=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	int retcode=0;
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT);
	try
	{
		CLogErrorLife loglife;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("1Bolt Please select node of single-bolt-connection which will be designed:","");
#else 
		pCmdLine->FillCmdLine("1Bolt 请选择要设计单螺栓连接的节点:","");
#endif
		//切换到单线显示状态
		CDisplayNodeAtFrontLife showPoint;
		showPoint.DisplayNodeAtFront();
		while(1)
		{
			if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			else if(retcode>0)
			{
				SELOBJ obj(dwhObj,dwExportFlag);
				if(pNode=console.FromNodeHandle(obj.hRelaObj))
				{
					g_pSolidDraw->SetEntSnapStatus(obj.hRelaObj);
					break;
				}
			}
		}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pNode->handle));
		BOOL bRet=FALSE;
		CDianBanParaDlg dlg;
		if(pNode->m_cPosCalType==4)
			bRet=DesignIntersNode(pNode,TRUE,&dlg);//设计交叉单螺栓
		else //if(pNode->m_cPosCalType!=4)
		{	//非交叉节点，	
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select one ray rod of single-bolt-connection which will be designed:","");
#else 
			pCmdLine->FillCmdLine("选择当前要设计单螺栓连接的任一根射线杆件:","");
#endif
			verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE|SNAP_ARC);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,SELECT_LINEPART);
			while(1)
			{
				f3dLine line;
				if((retcode=g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				{
					pCmdLine->CancelCmdLine();
					return 0;
				}
				if(retcode==0)
					continue;	//未选中
				SELOBJ obj(dwhObj,dwExportFlag);
				dwhObj=obj.hRelaObj;
				if(pLinePart=(CLDSLinePart*)console.FromPartHandle(dwhObj,CLS_LINEPART))
				{
					g_pSolidDraw->SetEntSnapStatus(dwhObj);
					break;
				}
			}
			pCmdLine->FinishCmdLine(CXhChar16("0x%X",pLinePart->handle));

			SmartPartPtr pFatherPart=console.FromPartHandle(pNode->hFatherPart);
			if(pFatherPart.IsNULL())
			{
				AfxMessageBox("当前连接节点缺少依附连接杆件");
				return FALSE;
			}
			//基准角钢与基准节点的父角钢可确定一个平面
			PARTSET partset;
			GEPOINT father_line_vec,temp_vec,work_norm;
			//CLDSLineAngle *pFatherAngle=pFatherPart->IsAngle()?pFatherPart.LineAnglePointer():NULL;
			if(pLinePart&&pLinePart->handle!=pNode->hFatherPart&&pFatherPart->IsLinePart()&&
				pLinePart->pStart&&pLinePart->pEnd&&pFatherPart.pRod->pStart&&pFatherPart.pRod->pEnd)	
			{	//基准节点的父角钢非基准角钢时，才可以执行下面的代码	//wht 09-08-07
				father_line_vec=pFatherPart.pRod->pEnd->Position(false)-pFatherPart.pRod->pStart->Position(false);
			}
			else if(pLinePart&&pLinePart->handle!=pNode->hFatherPart&&pFatherPart->IsArcPart())
			{
				father_line_vec=pFatherPart.pArcRod->GetArcTangentVec(pNode->Position());
			}
			if(!father_line_vec.IsZero())
			{
				normalize(father_line_vec);
				temp_vec=pLinePart->pEnd->Position(false)-pLinePart->pStart->Position(false);
				normalize(temp_vec);
				work_norm=temp_vec^father_line_vec;
				normalize(work_norm);
				for(CLDSLinePart *pLineRod=Ta.Parts.GetFirstLinePart();pLineRod;pLineRod=Ta.Parts.GetNextLinePart())
				{
					if(pLineRod==pLinePart||pLineRod==pFatherPart)
						continue;	//跳过基准角钢
					if(pLineRod->pStart!=pNode&&pLineRod->pEnd!=pNode)
						continue;
					if(pLineRod->pStart==NULL||pLineRod->pEnd==NULL)
						continue;
					temp_vec=pLineRod->pEnd->Position(false)-pLineRod->pStart->Position(false);
					normalize(temp_vec);
					if(fabs(temp_vec*work_norm)<EPS2&&fabs(temp_vec*father_line_vec)<EPS_COS)
						partset.append(pLineRod);	//需防止射线杆件与基准杆件平行(原为下面注释代码，5.14日改错） wjh-2015.8.29
					//f3dPoint norm=temp_vec^father_line_vec;
					//normalize(norm);
					//if(fabs(work_norm*norm)>EPS_COS)
					//	partset.append(pLineRod);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			if(partset.GetNodeNum()>=1&&AfxMessageBox("Whether to design no plate's single-bolt-connection?",MB_YESNO)==IDYES)
#else 
			if(partset.GetNodeNum()>=1&&AfxMessageBox("是否进行无板单螺栓连接?",MB_YESNO)==IDYES)
#endif
				pSecLinePart=(CLDSLinePart*)partset.GetFirst();
			CLDSLinePart* pThirdRod=partset.GetNodeNum()<2?NULL:(CLDSLinePart*)partset.GetNext();
			if(ValidateOneBoltConnect(pNode,pLinePart,pSecLinePart))
				bRet=DesignBoltOnlyConnect(pNode,pLinePart,pSecLinePart,pThirdRod,TRUE,TRUE);
			else
				bRet=FALSE;
		}		
		BOOL bIgnoreLsNum=FALSE;
		if(pLinePart&&bRet)
			bIgnoreLsNum=TRUE; //生成第一个螺栓时选择了忽略螺栓个数必须为1的限制,对称时也采取相应设置
		//对称生成单螺栓连接
		if(bRet)
		{
			if(pNode->m_cPosCalType==4)
			{	//检查对称可用性
				dlg.m_bMirCreate=TRUE;	//对称生成交叉点螺栓 wht 11-01-11
			}
			g_pSolidDraw->Draw();
			static CMirMsgDlg mir_dlg;
			//根据节点对称方式初始化螺栓对称方式 wht 16-10-09
			//根据宝鸡反馈单螺栓设计经常需要旋转对称,而如果按海涛所改(利波提的建议),就失去了旋转对称,不习惯 wjh-2016.12-17
			if (mir_dlg.mirmsg.axis_flag==0)
			{	//综合考虑如果第一次执行，应该根据当前节点的关联对称类型进行对称信息的初始化 wjh-2019.9.2
				for (RELATIVE_OBJECT *pObj=pNode->relativeObjs.GetFirst();pObj;pObj=pNode->relativeObjs.GetNext())
					mir_dlg.mirmsg.axis_flag|=pObj->mirInfo.axis_flag;
			}
			if(mir_dlg.DoModal()==IDOK)
			{
				MIRMSG mirmsg=mir_dlg.mirmsg;
				CLDSNode *src_mirnode[4]={NULL};
				CLDSLinePart *src_mirpart[4]={NULL};
				CLDSLinePart *src_mirsecpart[4]={NULL};
				src_mirnode[0]=pNode;
				src_mirpart[0]=pLinePart;
				src_mirsecpart[0]=pSecLinePart;
				if(mir_dlg.mirmsg.axis_flag&1)
				{	//X轴对称
					src_mirnode[1]=pNode->GetMirNode(mir_dlg.mirmsg.GetSubMirItem(1));
					if(pLinePart)
						src_mirpart[1]=(CLDSLinePart*)pLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					if(pSecLinePart)
						src_mirsecpart[1]=(CLDSLinePart*)pSecLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					if(pNode->m_cPosCalType==4)	//交叉节点
						DesignIntersNode(src_mirnode[1],TRUE,&dlg);
					//区分单角钢端连接对称和无板单螺栓对称
					else if((pSecLinePart&&src_mirsecpart[1])||(pSecLinePart==NULL&&src_mirsecpart[1]==NULL))
					{
						if(bIgnoreLsNum||ValidateOneBoltConnect(src_mirnode[1],src_mirpart[1],src_mirsecpart[1]))
							DesignBoltOnlyConnect(src_mirnode[1],src_mirpart[1],src_mirsecpart[1],TRUE,TRUE);
					}
				}
				if(mir_dlg.mirmsg.axis_flag&2)
				{	//Y轴对称
					src_mirnode[2]=pNode->GetMirNode(mir_dlg.mirmsg.GetSubMirItem(2));
					if(pLinePart)
						src_mirpart[2]=(CLDSLinePart*)pLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					if(pSecLinePart)
						src_mirsecpart[2]=(CLDSLinePart*)pSecLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					if(pNode->m_cPosCalType==4)	//交叉节点
						DesignIntersNode(src_mirnode[2],TRUE,&dlg);
					//区分单角钢端连接对称和无板单螺栓对称
					else if((pSecLinePart&&src_mirsecpart[2])||(pSecLinePart==NULL&&src_mirsecpart[2]==NULL))
					{
						if(bIgnoreLsNum||ValidateOneBoltConnect(src_mirnode[2],src_mirpart[2],src_mirsecpart[2]))
							DesignBoltOnlyConnect(src_mirnode[2],src_mirpart[2],src_mirsecpart[2],TRUE,TRUE);
					}
				}
				if(mir_dlg.mirmsg.axis_flag&4)
				{	//Z轴对称
					src_mirnode[3]=pNode->GetMirNode(mir_dlg.mirmsg.GetSubMirItem(3));
					if(pLinePart)
						src_mirpart[3]=(CLDSLinePart*)pLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					if(pSecLinePart)
						src_mirsecpart[3]=(CLDSLinePart*)pSecLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					if(pNode->m_cPosCalType==4)	//交叉节点
						DesignIntersNode(src_mirnode[3],TRUE,&dlg);
					//区分单角钢端连接对称和无板单螺栓对称
					else if((pSecLinePart&&src_mirsecpart[3])||(pSecLinePart==NULL&&src_mirsecpart[3]==NULL))
					{
						if(bIgnoreLsNum||ValidateOneBoltConnect(src_mirnode[3],src_mirpart[3],src_mirsecpart[3]))
							DesignBoltOnlyConnect(src_mirnode[3],src_mirpart[3],src_mirsecpart[3],TRUE,TRUE);
					}
				}
				if(mirmsg.axis_flag&8)
				{	//旋转对称
					mirmsg.axis_flag=8;
					for(int i=0;i<4;i++)
					{
						if(src_mirnode[i]==NULL)
							continue;
						CLDSNode *pMirNode=NULL;
						CLDSLinePart *pMirLinePart=NULL, *pMirSecLinePart=NULL;
						for(int j=0;j<mirmsg.array_num;j++)
						{
							if(pNode->m_cPosCalType!=4&&src_mirpart[i]==NULL)
								continue;	//非交叉点螺栓
							if(j==0)
							{
								pMirNode=src_mirnode[i]->GetMirNode(mirmsg);
								if(src_mirpart[i])
									pMirLinePart=(CLDSLinePart*)src_mirpart[i]->GetMirPart(mirmsg);
								if(src_mirsecpart[i])
									pMirSecLinePart=(CLDSLinePart*)src_mirsecpart[i]->GetMirPart(mirmsg);
							}
							else
							{
								pMirNode=pMirNode->GetMirNode(mirmsg);
								if(pMirLinePart)
									pMirLinePart=(CLDSLinePart*)pMirLinePart->GetMirPart(mirmsg);
								if(pMirSecLinePart)
									pMirSecLinePart=(CLDSLinePart*)pMirSecLinePart->GetMirPart(mirmsg);
							}
							if(pNode->m_cPosCalType==4&&pMirNode)	//交叉点螺栓
								DesignIntersNode(pMirNode,TRUE,&dlg);
							//区分单角钢端连接对称和无板单螺栓对称
							else if(pMirNode&&pMirLinePart&&((pSecLinePart&&pMirSecLinePart)||(pSecLinePart==NULL||pMirSecLinePart==NULL)))
							{
								if(bIgnoreLsNum||ValidateOneBoltConnect(pMirNode,pMirLinePart,pMirSecLinePart))
									DesignBoltOnlyConnect(pMirNode,pMirLinePart,pMirSecLinePart,TRUE,TRUE);
							}
							else
								break;
						}
					}
				}
			}
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Command:","");
#else 
		pCmdLine->FillCmdLine("命令:","");
#endif
		return 0;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("命令:","");
#endif
	DesignOneBolt();	
	return 0;
}

//批量设计单螺栓连接
int CLDSView::DesignSpecNodeOneBolt()
{
	CLDSNode *pNode;
	CLDSLineAngle *pAngle1=NULL,*pAngle2=NULL,*pAngle3=NULL;
	long *id_arr=NULL;
	long n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	static CDesignOneBoltDlg one_bolt_dlg;
	if(one_bolt_dlg.DoModal()!=IDOK)
		return 0;
	CProcBarDlg *pProcDlg = new CProcBarDlg(this);
	pProcDlg->Create();
#ifdef AFX_TARG_ENU_ENGLISH
	pProcDlg->SetWindowText("Design process");
#else 
	pProcDlg->SetWindowText("设计进度");
#endif
	
	BOOL bIgnoreError=FALSE;
	if(!one_bolt_dlg.m_bCheckUpLsNum)
		bIgnoreError=TRUE;
	//Ta.BeginUndoListen();	//批量设计时暂时不支持撤销操作 需保存的操作可能过多导致内存不足 wht 09-10-10
	CUndoOperObject undo(&Ta,true);	//换成成组undo应该可以避免undo存储信息过多的问题
	for(int i=0;i<n;i++)
	{
		pNode=console.FromNodeHandle(id_arr[i]);
		pProcDlg->Refresh((i*100)/n);
		if(pNode==NULL)
			continue;
		try
		{
			if(pNode->m_cPosCalType==4&&one_bolt_dlg.m_bInterNodeBolt)	//交叉节点
				DesignIntersNode(pNode,FALSE,FALSE);
			else
			{
				PARTSET jgset;		//连接在基准节点上的角钢集合
				PARTSET partset[2];	//partset[0]搭接在X肢上的角钢集合，partset[1]搭接在Y肢上的角钢集合
				Ta.Node.push_stack();
				for(CLDSPart *pPart=Ta.Parts.GetFirst(CLS_LINEANGLE);pPart;pPart=Ta.Parts.GetNext(CLS_LINEANGLE))
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					if(pLinePart->pStart==pNode||pLinePart->pEnd==pNode)
						jgset.append(pLinePart);
				}
				Ta.Node.pop_stack();
				//将连接在基准节点上的角钢按搭接肢分类
				CLDSLineAngle *pLineAngle=NULL,*pFatherLineAngle=NULL;
				pFatherLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNode->hFatherPart,CLS_LINEANGLE);
				if(pFatherLineAngle)
				{
					f3dPoint norm_x,norm_y,father_line_vec;
					norm_x=pFatherLineAngle->get_norm_x_wing();
					norm_y=pFatherLineAngle->get_norm_y_wing();
					father_line_vec=pFatherLineAngle->End()-pFatherLineAngle->Start();
					normalize(father_line_vec);
					for(pLineAngle=(CLDSLineAngle*)jgset.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)jgset.GetNext())
					{
						if(pLineAngle==pFatherLineAngle)
							continue;
						f3dPoint line_vec=pLineAngle->End()-pLineAngle->Start();
						normalize(line_vec);
						f3dPoint norm=line_vec^father_line_vec;
						normalize(norm);
						//cos(30°)=0.866
						if(fabs(norm*norm_x)>0.866)			//搭接在X肢上的射线角钢
							partset[0].append(pLineAngle);
						else if(fabs(norm*norm_y)>0.866)	//搭接在Y肢上的射线角钢
							partset[1].append(pLineAngle);
					}
				}	
				int i=0;
				//Ta.BeginUndoListen();	//支持逐步撤销
				for(i=0;i<2;i++)
				{
					if(partset[i].GetNodeNum()==1&&one_bolt_dlg.m_bSingleAngleEndBolt)
					{
						pAngle1 = (CLDSLineAngle*)partset[i][0];
						Ta.Node.push_stack();
						if(pAngle1->pStart->handle==pNode->handle)
						{	//始端螺栓设计个数为1，且未布置螺栓
							if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
									bIgnoreError=TRUE;
							}
							if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)	
								DesignBoltOnlyConnect(pNode,pAngle1,NULL,FALSE,FALSE);
						}
						else if(pAngle1->pEnd->handle==pNode->handle)	
						{	//终端螺栓设计个数为1，且未布置螺栓
							if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
									bIgnoreError=TRUE;
							}
							if((bIgnoreError||!bIgnoreError&&pAngle1->connectEnd.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(2,100)==0)
								DesignBoltOnlyConnect(pNode,pAngle1,NULL,FALSE,FALSE);
						}
						Ta.Node.pop_stack();
					}
					else if(partset[i].GetNodeNum()==2&&one_bolt_dlg.m_bSingleBoltNoPlate)
					{	
						pAngle1 = (CLDSLineAngle*)partset[i][0];
						pAngle2 = (CLDSLineAngle*)partset[i][1];
						Ta.Node.push_stack();
						//保证两根连接材在同一平面上
						if(pAngle1->layer(2)!=pAngle2->layer(2))
							continue;	
						if(pAngle1->pStart->handle==pNode->handle)
						{	//始端螺栓设计个数为1，且未布置螺栓
							if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
									bIgnoreError=TRUE;
							}
							if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)
								DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
						}
						else if(pAngle1->pEnd->handle==pNode->handle)
						{	//终端螺栓设计个数为1，且未布置螺栓
							if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
									bIgnoreError=TRUE;
							}
							if((bIgnoreError||!bIgnoreError&&pAngle1->connectEnd.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(2,100)==0)
								DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
						}
						/*
						if(pAngle2->pStart->handle==pNode->handle)
						{	//始端螺栓设计个数为1，且未布置螺栓
						if(pAngle2->connectStart.wnConnBoltN==1&&pAngle2->GetLocalLsCount(1)==0)
						DesignBoltOnlyConnect(pNode,pAngle2,FALSE,FALSE);
						}
						else if(pAngle2->pEnd->handle==pNode->handle)
						{	//终端螺栓设计个数为1，且未布置螺栓
						if(pAngle2->connectEnd.wnConnBoltN==1&&pAngle2->GetLocalLsCount(2)==0)
						DesignBoltOnlyConnect(pNode,pAngle2,FALSE,FALSE);
					}*/
						Ta.Node.pop_stack();
					}
					else if(partset[i].GetNodeNum()==3&&one_bolt_dlg.m_bSingleBoltNoPlate)
					{
						pAngle1 = (CLDSLineAngle*)partset[i][0];
						pAngle2 = (CLDSLineAngle*)partset[i][1];
						pAngle3 = (CLDSLineAngle*)partset[i][2];
						//f3dPoint norm1,norm2,norm3;
						//int x_wing0_y_wing1,x_wing0_y_wing2,x_wing0_y_wing3;
						//GetWorkNorm(pFatherLineAngle,pAngle1,&norm1);
						//GetWorkNorm(pFatherLineAngle,pAngle2,&norm2);
						//GetWorkNorm(pFatherLineAngle,pAngle3,&norm3);
						if(!(pAngle1->layer(2)==pAngle2->layer(2)&&pAngle2->layer(2)==pAngle3->layer(2)))
							continue;
						DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,pAngle3,FALSE,FALSE);
						/*
						if(!(IsInsideJg(pAngle1,norm1,&x_wing0_y_wing1)==IsInsideJg(pAngle2,norm2,&x_wing0_y_wing2)&&
							IsInsideJg(pAngle2,norm2,&x_wing0_y_wing2)==IsInsideJg(pAngle3,norm3,&x_wing0_y_wing3)))
							continue;
						typedef CMultiOffsetPos* CMultiOffsetPosPtr;
						CMultiOffsetPosPtr desPosArr[3]={pAngle1->pStart->handle==pNode->handle?&pAngle1->desStartPos:&pAngle1->desEndPos,
														 pAngle2->pStart->handle==pNode->handle?&pAngle2->desStartPos:&pAngle2->desEndPos,
														 pAngle3->pStart->handle==pNode->handle?&pAngle3->desStartPos:&pAngle3->desEndPos};
						if( (desPosArr[0]->len_offset_dist==0&&desPosArr[1]->len_offset_dist==0)||
							(desPosArr[1]->len_offset_dist==0&&desPosArr[2]->len_offset_dist==0)||
							(desPosArr[0]->len_offset_dist==0&&desPosArr[2]->len_offset_dist==0))
							continue;
						Ta.Node.push_stack();
						for(int j=0;j<=2;++j)
						{
							CLDSLineAngle *pAngle=(CLDSLineAngle*)partset[i][j];
							if(pAngle->pStart->handle==pNode->handle)
							{	//始端螺栓设计个数为1，且未布置螺栓
								if(pAngle->connectStart.wnConnBoltN!=1&&!bIgnoreError)
								{
	#ifdef AFX_TARG_ENU_ENGLISH
									if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
	#else 
									if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
	#endif
										bIgnoreError=TRUE;
								}
								if((bIgnoreError||!bIgnoreError&&pAngle->connectStart.wnConnBoltN==1)&&pAngle->GetLocalLsCount(1)==0)	
									DesignBoltOnlyConnect(pNode,pAngle,NULL,FALSE,FALSE);
							}
							else if(pAngle->pEnd->handle==pNode->handle)	
							{	//终端螺栓设计个数为1，且未布置螺栓
								if(pAngle->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
								{
	#ifdef AFX_TARG_ENU_ENGLISH
									if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
	#else 
									if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
	#endif
										bIgnoreError=TRUE;
								}
								if((bIgnoreError||!bIgnoreError&&pAngle->connectEnd.wnConnBoltN==1)&&pAngle->GetLocalLsCount(2)==0)
									DesignBoltOnlyConnect(pNode,pAngle,NULL,FALSE,FALSE);
							}
						}
						Ta.Node.pop_stack();
						*/
					}
				}
				//Ta.EndUndoListen();	//支持逐步撤销
			}
		}
		catch(char *sError)
		{
			AfxMessageBox(sError);
			g_pSolidDraw->SetEntSnapStatus(pNode->handle,FALSE);
			//Ta.EndUndoListen();
		}
	}
	//Ta.EndUndoListen();
	pProcDlg->DestroyWindow();
	delete pProcDlg;
	return 0;
}

//根据外孔间隙调整螺栓纵向偏移量 
//使用该函数之前应保证螺栓的位置以及法线参数已赋值	wht 10-07-22
int CalEndLjFirstBoltOffsetDist(CLDSBolt *pFirstBolt,CLDSLineAngle *pDatumLineAngle,CLDSLineAngle *pCrossAngle,int nFirstLsJgSpace=0)
{
	if(pFirstBolt==NULL||pDatumLineAngle==NULL)
		return 0;
	if(nFirstLsJgSpace==0)
	{	//根据螺栓直径得到外孔间隙
		if(pFirstBolt->get_d()<=12)
			nFirstLsJgSpace = 15;
		else if(pFirstBolt->get_d()<=16)
			nFirstLsJgSpace = 20;
		else if(pFirstBolt->get_d()<=20)
			nFirstLsJgSpace = 25;
		else	
			nFirstLsJgSpace = 30;
	}
	pFirstBolt->correct_worknorm();	//
	pFirstBolt->des_base_pos.datumPoint.UpdatePos(pFirstBolt->BelongModel());
	f3dPoint line_vec,wing_vec,cross_wing_vec,cross_line_vec,cross_wing_norm;
	cross_line_vec= pCrossAngle->End()-pCrossAngle->Start();
	normalize(cross_line_vec);
	int datum_wing_x0_y1=0,cross_wing_x0_y1=1;
	if(fabs(pFirstBolt->get_norm()*pDatumLineAngle->get_norm_x_wing())>
		fabs(pFirstBolt->get_norm()*pDatumLineAngle->get_norm_y_wing()))
	{
		datum_wing_x0_y1=0;
		wing_vec=pDatumLineAngle->GetWingVecX();
	}
	else
	{
		datum_wing_x0_y1=1;
		wing_vec=pDatumLineAngle->GetWingVecY();
	}
	if((fabs(pFirstBolt->get_norm()*pCrossAngle->get_norm_x_wing())>
		fabs(pFirstBolt->get_norm()*pCrossAngle->get_norm_y_wing())))
	{
		cross_wing_x0_y1=0;
		cross_wing_norm=pCrossAngle->get_norm_x_wing();
		cross_wing_vec=pCrossAngle->GetWingVecX();
	}
	else
	{
		cross_wing_x0_y1=1;
		cross_wing_norm=pCrossAngle->get_norm_y_wing();
		cross_wing_vec=pCrossAngle->GetWingVecY();
	}
	if(pFirstBolt->des_base_pos.direction==1)
		line_vec=pDatumLineAngle->Start()-pDatumLineAngle->End();
	else
		line_vec=pDatumLineAngle->End()-pDatumLineAngle->Start();
	normalize(wing_vec);
	normalize(line_vec);
	f3dPoint pos=pFirstBolt->des_base_pos.datumPoint.Position()+
		line_vec*pFirstBolt->des_base_pos.len_offset_dist+
		wing_vec*pFirstBolt->des_base_pos.wing_offset_dist;
	f3dPoint verfiy_vec=pos-pFirstBolt->des_base_pos.datumPoint.Position();
	normalize(verfiy_vec);
	if(verfiy_vec*line_vec<0)
		line_vec*=-1.0;
	UCS_STRU ucs;
	ucs.origin=pCrossAngle->Start();
	ucs.axis_x=cross_line_vec;
	ucs.axis_y=cross_wing_vec;
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	normalize(ucs.axis_z);
	coord_trans(pos,ucs,FALSE);
	double cosa=line_vec*cross_wing_vec;
	double dd=0;
	if(cosa>0)	//螺栓位于肢翼侧
		dd=pos.y-pCrossAngle->GetWidth();
	else		//螺栓位于楞线侧
		dd=-pos.y;
	double len_offset=0;
	if(dd<nFirstLsJgSpace)	//目前间隙小于外孔间隙
	{
		double add_offset=(nFirstLsJgSpace-dd)/fabs(cosa);
		double round_offset=fto_halfi(add_offset/10)*10;	//圆整后的增加值
		if(pFirstBolt->des_base_pos.len_offset_dist>0)
		{	
			if(round_offset<add_offset)
				len_offset=pFirstBolt->des_base_pos.len_offset_dist+ftoi(round_offset)+5;
			else
				len_offset=pFirstBolt->des_base_pos.len_offset_dist+ftoi(round_offset);
		}
		else
		{	
			if(round_offset<add_offset)
				len_offset=pFirstBolt->des_base_pos.len_offset_dist-ftoi(round_offset)-5;
			else
				len_offset=pFirstBolt->des_base_pos.len_offset_dist-ftoi(round_offset);
		}
	}
	else
		len_offset=pFirstBolt->des_base_pos.len_offset_dist;
	return abs((int)len_offset); //返回第一个外孔螺栓与角钢心线交点之间的距离
}

/* 该项功能代码目前已完全被复制螺栓及偏移螺栓命令取代,待使用过程中进一步确认后删除　wjh-2015.3.8
//偏移生成螺栓
void CLDSView::OnDefOffsetLs()
{
	m_nPrevCommandID=ID_DEF_OFFSET_LS;
	m_sPrevCommandName="重复定义偏移螺栓";
	Command("OffsetBolt");
}
int CLDSView::DefOffsetLs() 
{
	CXhChar100 sText;//[100]="";
	int len_offset=0;
	CString cmdStr;
	//选中的射线角钢
	CLDSLineAngle *pSelectLineAngle=NULL;
	//临时使用的变量
	CLsRef *pLsRef=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSBolt *pDatumBolt=NULL;
	CLDSBolt *pNewBolt=NULL;
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_BOLT),CLS_BOLT);
	scr_part_para.disp_type=DISP_SOLID;
	scr_part_para.cmdStr="DefOffsetLs 请选择偏移基准螺栓:";
	scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择基准螺栓:";
	if(!GetPartsFromScr(scr_part_para))
		return 0;
	CLDSDbObject *pObj=scr_part_para.pResultObjsArr[0];
	if(pObj&&pObj->GetClassTypeId()==CLS_BOLT)
		pDatumBolt=(CLDSBolt*)pObj;
	if(pDatumBolt==NULL)
		return 0;
	//选中螺栓所在的角钢
	g_pSolidDraw->SetEntSnapStatus(pDatumBolt->des_base_pos.hPart,TRUE);
	g_pSolidDraw->Draw();
	CLDSPlate *pDatumPlate=NULL;
	CLDSLineAngle *pDatumLineAngle = (CLDSLineAngle*)console.FromPartHandle(pDatumBolt->des_base_pos.hPart,CLS_LINEANGLE);
	for(pDatumPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pDatumPlate;pDatumPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if(pDatumPlate->FindLsByHandle(pDatumBolt->handle))
			break;
	}
	//选中螺栓所在的基准钢板
	if(pDatumPlate)
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle,TRUE);
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,pDatumBolt->get_d());
	//生成偏移螺栓
	Ta.BeginUndoListen();
	pNewBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT,TRUE);
	pDatumBolt->CloneTo(*pNewBolt);
	//基准螺栓定位方式为杆件节点定位 若选择射线角钢则需要生成心线交点螺栓
	if((pDatumBolt->des_base_pos.datumPoint.datum_pos_style==2||pDatumBolt->des_base_pos.datumPoint.datum_pos_style==3)
		&&pDatumBolt->des_base_pos.len_offset_dist==0&&pDatumBolt->des_base_pos.wing_offset_dist==0)
	{	//选择了在基准位置的螺栓，可以生成心线交点螺栓
		Sleep(500);//高亮显示半秒钟,不然用户不知道是否选中了此构件显示状态就被冲掉了
		pCmdLine->FillCmdLine("请选择需要添加螺栓的射线角钢<可不选,默认在基准角钢上布置螺栓>:","");
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_PLATE));
		while(1)
		{
			if(pCmdLine->GetStrFromCmdLine(cmdStr))
			{
				long *id_arr;
				if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
				{
					CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
					if(pPart)
					{
						if(pPart->GetClassTypeId()==CLS_LINEANGLE)
						{	//所选射线杆件不在基准钢板连接构件列表中
							if(pDatumPlate)
							{
								if(pDatumPlate->designInfo.FromHandle(pPart->handle))
									pSelectLineAngle=(CLDSLineAngle*)pPart;
								else
								{
									pCmdLine->FillCmdLine("所选射线角钢未连接在基准钢板上，请重新选择<可不选,默认在基准角钢上布置螺栓>:","");
									g_pSolidDraw->ReleaseSnapStatus();
									continue;
								}
							}
							else
							{
								pSelectLineAngle=(CLDSLineAngle*)pPart;
								if(!pSelectLineAngle->FindLsByHandle(pDatumBolt->handle))
								{
									pCmdLine->FillCmdLine("基准螺栓不在所选射线角钢上，请重新选择<可不选,默认在基准角钢上布置螺栓>:","");
									g_pSolidDraw->ReleaseSnapStatus();
									continue;
								}
							}
						}	
						else
						{
							pCmdLine->FillCmdLine("没有选中合适的射线角钢，请重新选择<可不选,默认在基准角钢上布置螺栓>:","");
							g_pSolidDraw->ReleaseSnapStatus();
							continue;
						}
						break;
					}
				}
				//未选择射线角钢,默认在基准角钢上布置螺栓
				if(pSelectLineAngle==NULL)	
					break;
			}
			else
			{
				g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
				Ta.EndUndoListen();
				return 0;
			}
		}
		if(pSelectLineAngle==NULL)
		{	//选中螺栓所在的角钢
			g_pSolidDraw->SetEntSnapStatus(pDatumBolt->des_base_pos.hPart,TRUE);
			//选中螺栓所在的基准钢板
			if(pDatumPlate)
				g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle,TRUE);
		}
		else if(pSelectLineAngle&&pDatumLineAngle&&pDatumPlate)
		{	//选择了射线角钢可生成心线交点螺栓
			g_pSolidDraw->Draw();
			pCmdLine->FillCmdLine(cmdStr,"");
			pNewBolt->des_base_pos.datumPoint.datum_pos_style=3;//角钢心线交点
			pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pDatumLineAngle->handle;
			pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pSelectLineAngle->handle;
			pNewBolt->des_base_pos.hPart=pSelectLineAngle->handle;
			//判断射线杆件为始端连接还是终端连接
			f3dLine datum_line;
			datum_line.startPt=pDatumLineAngle->pStart->Position(true);
			datum_line.endPt=pDatumLineAngle->pEnd->Position(true);
			int start0_end1=0;	//始端连接 始端==>终端
			if(datum_line.PtInLine(pSelectLineAngle->pEnd->Position(true))>0)
				start0_end1=1;	//终端连接 终端==>始端
			pNewBolt->des_base_pos.direction=start0_end1;
			
			int nOffsetStyle=0;
			//g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pSelectLineAngle->handle,TRUE);
			cmdStr.Format("请输入射线角钢肢向偏移距离[0为g,1为g1,2为g2,3为g3,或直接输入距离]<%d>",0);
			pCmdLine->FillCmdLine(cmdStr,"");
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				{
					Ta.EndUndoListen();
					return 0;
				}
				if(cmdStr.GetLength()>0)
				{
					sText.Printf("%s",cmdStr);
					//sprintf(sText,"%s",cmdStr);
					nOffsetStyle = atoi(sText);
				}
				break;
			}
			if(nOffsetStyle>3)
			{
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;//自定义偏移距离
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=nOffsetStyle;
			}
			else
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=nOffsetStyle;
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pDatumLineAngle->handle,TRUE);
			cmdStr.Format("请输入基准角钢肢向偏移距离[0为g,1为g1,2为g2,3为g3,或直接输入距离]<%d>:",0);
			pCmdLine->FillCmdLine(cmdStr,"");
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				{
					Ta.EndUndoListen();
					return 0;
				}
				if(cmdStr.GetLength()>0)
				{
					sText.Printf("%s",cmdStr);
					//sprintf(sText,"%s",cmdStr);
					nOffsetStyle = atoi(sText);
				}
				break;
			}
			if(nOffsetStyle>3)
			{
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;//自定义偏移距离
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=nOffsetStyle;
			}
			else
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=nOffsetStyle;
			BOOL bEndLj=TRUE;
			cmdStr.Format("该射线角钢是否[Y/N]需要端连接<Y>:");
			pCmdLine->FillCmdLine(cmdStr,"");
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				{
					Ta.EndUndoListen();
					return 0;
				}
				if(cmdStr.GetLength()>0)
				{
					if(cmdStr.CompareNoCase("N")==0)
						bEndLj=FALSE;
					else if(cmdStr.CompareNoCase("Y")==0)
						bEndLj=TRUE;
					else
					{
						pCmdLine->FinishCmdLine();
						cmdStr.Format("输入错误请重新选择,该射线角钢是否[Y/N]需要端连接<Y>:");
						continue;
					}
				}
				break;
			}
			if(bEndLj&&pDatumPlate)
			{	//所选射线杆件需要端连接
				CDesignLjPartPara *pLjPartPara=pDatumPlate->designInfo.FromHandle(pSelectLineAngle->handle);
				if(pLjPartPara)
				{
					if(pLjPartPara->start0_end1==0)
					{
						pSelectLineAngle->desStartOdd.m_iOddCalStyle=1;//按螺栓计算正负头
						pSelectLineAngle->CalStartOddment();
						pSelectLineAngle->AppendStartLsRef(pDatumBolt->GetLsRef());
					}
					else if(pLjPartPara->start0_end1==1)
					{
						pSelectLineAngle->desEndOdd.m_iOddCalStyle=1;	//按螺栓计算正负头
						pSelectLineAngle->CalEndOddment();
						pSelectLineAngle->AppendEndLsRef(pDatumBolt->GetLsRef());
					}
					else
						pSelectLineAngle->AppendMidLsRef(pDatumBolt->GetLsRef());
					f3dPoint ray_wing_norm,datum_wing_norm;
					if(pDatumBolt->des_base_pos.offset_wing==0)
						datum_wing_norm=pDatumLineAngle->get_norm_x_wing();
					else
						datum_wing_norm=pDatumLineAngle->get_norm_y_wing();
					if(pLjPartPara->angle.cur_wing_x0_y1==0)
						ray_wing_norm=pSelectLineAngle->get_norm_x_wing();
					else
						ray_wing_norm=pSelectLineAngle->get_norm_y_wing();
					if(ray_wing_norm*datum_wing_norm>0)	//里铁
					{	//需要调整端连接射线角钢偏移距离 添加螺栓垫板 调整螺栓法向偏移量 计算切角信息
						pDatumBolt->des_base_pos.norm_offset.AddThick(-pSelectLineAngle->handle,TRUE,TRUE);
						pNewBolt->des_base_pos.norm_offset.AddThick(-pSelectLineAngle->handle,TRUE,TRUE);
						CMultiOffsetPos *pDesPosPara=NULL;
						if(pLjPartPara->start0_end1==0)
							pDesPosPara=&pSelectLineAngle->desStartPos;
						else if(pLjPartPara->start0_end1==1)
							pDesPosPara=&pSelectLineAngle->desEndPos;
						if(pDesPosPara&&pDatumLineAngle)
						{	//调整端连接射线角钢偏移方向
							if(pLjPartPara->angle.cur_wing_x0_y1==0)
							{	//当前连接肢为X肢
								pDesPosPara->wing_x_offset.gStyle=4;
								pDesPosPara->wing_x_offset.offsetDist=-pDatumLineAngle->GetThick();
							}
							else
							{	//当前连接肢为Y肢
								pDesPosPara->wing_y_offset.gStyle=4;
								pDesPosPara->wing_y_offset.offsetDist=-pDatumLineAngle->GetThick();
							}
						}
					}
					else //外铁
						pNewBolt->des_base_pos.norm_offset.EmptyThick();
					pDatumBolt->AddL0Thick(pSelectLineAngle->handle,TRUE,TRUE);
					pDatumBolt->CalGuigeAuto();
					pDatumBolt->correct_worknorm();
					pDatumBolt->correct_pos();
					pDatumBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pDatumBolt->GetSolidPartObject());
					pLjPartPara->angle.bEndLjJg=TRUE;
					pSelectLineAngle->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pSelectLineAngle->GetSolidPartObject());
					//存在端连接射线杆件时应调整基准杆件上的螺栓基点定位方式为角钢心线交点
					for(pLsRef=pDatumLineAngle->GetFirstLsRef();pLsRef;pLsRef=pDatumLineAngle->GetNextLsRef())
					{
						if(!pDatumPlate->FindLsByHandle((*pLsRef)->handle))
							continue;
						(*pLsRef)->des_base_pos.datumPoint.datum_pos_style=3;//角钢心线交点
						(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pDatumLineAngle->handle;
						(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pSelectLineAngle->handle;
						(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1
							=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
						(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1
							=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
						(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2
							=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
						(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2
							=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
						(*pLsRef)->correct_worknorm();
						(*pLsRef)->correct_pos();
						(*pLsRef)->CalGuigeAuto();
						(*pLsRef)->SetModified();
						(*pLsRef)->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
					}
					len_offset=(int)CalEndLjFirstBoltOffsetDist(pNewBolt,pSelectLineAngle,pDatumLineAngle);
					//根据默认的最小外孔间隙调整主材以及端连接射线角钢螺栓位置
					int adjust_len_offset=0;
					if(pDatumPlate)
					{	
						BOLTSET boltSet[2];	//boltSet[0]存放偏移值为正的螺栓 boltSet[1]存放偏移值为负的螺栓
						CLDSBolt *pBolt=NULL;
						for(pLsRef=pDatumLineAngle->GetFirstLsRef();pLsRef;pLsRef=pDatumLineAngle->GetNextLsRef())
						{
							if(!pDatumPlate->FindLsByHandle((*pLsRef)->handle))	
								continue;	//不处理不在当前钢板上的螺栓
							if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style!=3)
								continue;	//螺栓基点定位方式非角钢心线交点的螺栓不用调整位置
							//将端连接射线角钢上定位方式为角钢心线交点的螺栓按螺栓纵向偏移量从小到大排序
							if((*pLsRef)->des_base_pos.len_offset_dist>0)
							{
								for(pBolt=boltSet[0].GetFirst();pBolt;pBolt=boltSet[0].GetNext())
								{
									if((*pLsRef)->des_base_pos.len_offset_dist<pBolt->des_base_pos.len_offset_dist)
									{
										boltSet[0].insert(pLsRef->GetLsPtr());
										break;
									}
								}
								if(pBolt==NULL)
									boltSet[0].append(pLsRef->GetLsPtr());
							}
							else if((*pLsRef)->des_base_pos.len_offset_dist<0)
							{
								for(pBolt=boltSet[1].GetFirst();pBolt;pBolt=boltSet[1].GetNext())
								{
									if((*pLsRef)->des_base_pos.len_offset_dist>pBolt->des_base_pos.len_offset_dist)
									{
										boltSet[1].insert(pLsRef->GetLsPtr());
										break;
									}
								}
								if(pBolt==NULL)
									boltSet[1].append(pLsRef->GetLsPtr());
							}
						}
						int i=0,nFlag=1;
						for(i=0;i<2;i++)
						{
							if(i==1)//boltSet[0]存放偏移值为正的螺栓 boltSet[1]存放偏移值为负的螺栓
								nFlag=-1;
							CLDSBolt *pFirstBolt=boltSet[i].GetFirst();
							if(pFirstBolt==NULL)
								continue;
							adjust_len_offset=(int)CalEndLjFirstBoltOffsetDist(pFirstBolt,pDatumLineAngle,pSelectLineAngle);
							if(adjust_len_offset>abs((int)pFirstBolt->des_base_pos.len_offset_dist))
							{
								int tmp_len_offset=abs((int)adjust_len_offset)-abs((int)pFirstBolt->des_base_pos.len_offset_dist);
								for(pBolt=boltSet[i].GetFirst();pBolt;pBolt=boltSet[i].GetNext())
								{
									pBolt->des_base_pos.len_offset_dist+=tmp_len_offset*nFlag;
									pBolt->CalGuigeAuto();
									pBolt->correct_worknorm();
									pBolt->correct_pos();
									pBolt->Create3dSolidModel();
									g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
								}
							}
						}
					}
				}
			}
		}
	}
	cmdStr.Format("请输入螺栓的横向偏移值<%d>:",0);
	pCmdLine->FillCmdLine(cmdStr,"");
	int nBoltSpace=0;
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			Ta.EndUndoListen();
			return 0;
		}
		if(cmdStr.GetLength()>0)
		{
			sText.Printf("%s",cmdStr);
			//sprintf(sText,"%s",cmdStr);
			nBoltSpace = atoi(sText);
		}
		break;
	}
	pNewBolt->des_base_pos.wing_offset_dist+=nBoltSpace;
	if(len_offset!=0&&len_offset>LsSpace.SingleRowSpace)
	{
		cmdStr.Format("请输入螺栓的纵向偏移值<%d>:",len_offset);
		nBoltSpace=len_offset;
	}
	else
	{
		if(pDatumBolt->des_base_pos.len_offset_dist>0)
		{
			cmdStr.Format("请输入螺栓的纵向偏移值<%d>:",LsSpace.SingleRowSpace);
			nBoltSpace=LsSpace.SingleRowSpace;
		}
		else
		{
			cmdStr.Format("请输入螺栓的纵向偏移值<%d>:",-LsSpace.SingleRowSpace);
			nBoltSpace=-LsSpace.SingleRowSpace;
		}
	}
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			Ta.EndUndoListen();
			return 0;
		}
		if(cmdStr.GetLength()>0)
		{
			sText.Printf("%s",cmdStr);
			//sprintf(sText,"%s",cmdStr);
			nBoltSpace = atoi(sText);
		}
		break;
	}
	//端连接射线角钢上螺栓使用从基准点开始偏移最小偏移距离,其余螺栓从所选螺栓位置开始偏移
	if(pSelectLineAngle&&pNewBolt->des_base_pos.datumPoint.datum_pos_style==3)
		pNewBolt->des_base_pos.len_offset_dist=nBoltSpace;
	else
		pNewBolt->des_base_pos.len_offset_dist+=nBoltSpace;
	if(pSelectLineAngle&&pNewBolt->des_base_pos.hPart==pDatumLineAngle->handle)
	{	//更新螺栓定位方式 wht 10-09-18
		pNewBolt->des_base_pos.hPart=pSelectLineAngle->handle;
		pNewBolt->des_base_pos.datumPoint.datum_pos_style=3;	//角钢心线交点
		pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pSelectLineAngle->handle;
		pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pDatumLineAngle->handle;
		pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1
			=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
		pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1
			=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
		pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2
			=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
		pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2
			=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
		pNewBolt->des_base_pos.len_offset_dist=nBoltSpace;
	}
	pNewBolt->CalGuigeAuto();
	pNewBolt->correct_worknorm();
	pNewBolt->correct_pos();
	pNewBolt->SetModified();
	pNewBolt->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pNewBolt->GetSolidPartObject());
	//将螺栓引入到基准角钢
	if(pSelectLineAngle)
	{	
		if(pDatumPlate)
		{	//其他的螺栓也要修改定位方式
			double nPrevLsSpace=pNewBolt->des_base_pos.len_offset_dist;
			LSSPACE_STRU LsSpace;
			for(pLsRef=pSelectLineAngle->GetFirstLsRef();pLsRef;pLsRef=pSelectLineAngle->GetNextLsRef())
			{
				if(!pDatumPlate->FindLsByHandle((*pLsRef)->handle))
					continue;
				if((*pLsRef)->handle == pDatumBolt->handle)
					continue;
				(*pLsRef)->des_base_pos.datumPoint.datum_pos_style=3;//角钢心线交点
				(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pSelectLineAngle->handle;
				(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pDatumLineAngle->handle;
				(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1
					=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
				(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1
					=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2;
				(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2
					=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1;
				(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2
					=pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1;
				GetLsSpace(LsSpace,(*pLsRef)->get_d());
				nPrevLsSpace+=LsSpace.SingleRowSpace;
				(*pLsRef)->des_base_pos.len_offset_dist=nPrevLsSpace;
				(*pLsRef)->correct_worknorm();
				(*pLsRef)->correct_pos();
				(*pLsRef)->CalGuigeAuto();
				(*pLsRef)->SetModified();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
			//角钢心线交点定位
			if(pNewBolt->des_base_pos.direction==0)
				pSelectLineAngle->AppendStartLsRef(pNewBolt->GetLsRef());
			else if(pNewBolt->des_base_pos.direction==1)
				pSelectLineAngle->AppendEndLsRef(pNewBolt->GetLsRef());
			else 
				pSelectLineAngle->AppendMidLsRef(pNewBolt->GetLsRef());
		}
		else
			pSelectLineAngle->AppendMidLsRef(pNewBolt->GetLsRef());
	}
	else if(pDatumLineAngle)
	{
		if(pDatumBolt->des_base_pos.datumPoint.datum_pos_style==1)
		{	//角钢楞点定位
			if(pNewBolt->des_base_pos.direction==0)
				pDatumLineAngle->AppendStartLsRef(pNewBolt->GetLsRef());
			else if(pNewBolt->des_base_pos.direction==1)
				pDatumLineAngle->AppendEndLsRef(pNewBolt->GetLsRef());
		} 
		else //杆件上的节点定位或角钢心线交点定位
			pDatumLineAngle->AppendMidLsRef(pNewBolt->GetLsRef());
	}
	//将螺栓引入到基准螺栓所在钢板并重新设计钢板外形
	if(pDatumPlate)
	{
		pDatumPlate->AppendLsRef(pNewBolt->GetLsRef());
		pDatumPlate->DesignPlate();
		pDatumPlate->SetModified();
		pDatumPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	Ta.EndUndoListen();
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("命令","");
	DefOffsetLs();
	return 0;
}
*/
//--------------VVV---OldCommand----VVV-----------------
void CLDSView::OnSingleXieNodeDesign() 
{
	Command("1BoltEndAngle");
}

//设计所选中的交叉点螺栓
void CLDSView::OnXieIntersPtDesign() 
{
	int i,N;
	long n, *id_arr=NULL;
	CLDSNode *pNode;
	NODESET nodeset;
	CString cmdStr="";
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select cross node of designed bolts:","");
#else 
	pCmdLine->FillCmdLine("选择要设计螺栓的交叉节点:","");
#endif
	g_pSolidSnap->SetSnapType(SNAP_POINT);
	if(pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		n=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for(i=0;i<n;i++)
		{
			pNode=console.FromNodeHandle(id_arr[i]);
			if(pNode)
				nodeset.append(pNode);
		}
	}
	else
	{
		pCmdLine->CancelCmdLine();
		return;
	}
	//防止用户误操作
#ifdef AFX_TARG_ENU_ENGLISH
	if(n>0&&MessageBox( "Are you sure to design selected cross bolts？",NULL,MB_YESNO)!=IDYES)
#else 
	if(n>0&&MessageBox( "真的要设计所选中的交叉点螺栓吗？",NULL,MB_YESNO)!=IDYES)
#endif
		return;
	
	CProcBarDlg *pProcDlg = new CProcBarDlg(this);
	pProcDlg->Create();	//创建进度条
#ifdef AFX_TARG_ENU_ENGLISH
	pProcDlg->SetWindowText("Design process");
#else 
	pProcDlg->SetWindowText("设计进度");
#endif
	BeginWaitCursor();
	N=nodeset.GetNodeNum();
	for(pNode=nodeset.GetFirst(),i=0;pNode;pNode=nodeset.GetNext(),i++)
	{
		g_pSolidDraw->SetEntSnapStatus(pNode->handle);
		DesignIntersNode(pNode,FALSE,FALSE);
		g_pSolidDraw->SetEntSnapStatus(pNode->handle,FALSE);
		pProcDlg->Refresh((i*100+1)/N);	//更新进度条
	}
	pProcDlg->DestroyWindow();	//销毁进度条
	EndWaitCursor();
	
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("The design of cross bolts is completed!");
#else 
	MessageBox("选中的交叉点螺栓设计完毕!");
#endif
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
}
//设计单螺栓连接
int CLDSView::DesignOneBoltEndAngle()
{
	CLDSNode *pNode;
	CLDSLineAngle *pAngle;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	try
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("1BoltEndAngle Please select single-bolt-connection's angle:","");
#else 
		pCmdLine->FillCmdLine("1BoltEndAngle 选择当前要设计单螺栓端连接的角钢:","");
#endif
		//切换到单线显示状态
		g_pSolidSet->SetDisplayType(DISP_LINE);
		Invalidate(FALSE);
		while(1)
		{
			f3dLine line;
			int ret = g_pSolidSnap->SnapLine(line);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			else if(ret>0)
			{
				pAngle=(CLDSLineAngle*)console.FromPartHandle(line.ID,CLS_LINEANGLE);
				if(pAngle)
				{
					g_pSolidDraw->SetEntSnapStatus(line.ID);
					break;
				}
			}
		}
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select end nodes of angle:","");
#else 
		pCmdLine->FillCmdLine("请选择角钢当前设计端的端节点:","");
#endif
		while(1)
		{
			f3dPoint* point;
			int ret = g_pSolidSnap->SnapPoint(point);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			else if(ret>0)
			{
				pNode=console.FromNodeHandle(point->ID);
				if(pNode)
				{
					g_pSolidDraw->SetEntSnapStatus(point->ID);
					break;
				}
			}
		}
		pCmdLine->FinishCmdLine();
		
		PARTSET partset;
		//基准角钢与基准节点的父角钢可确定一个平面
		CLDSLineAngle *pFatherAngle=NULL;
		if(pNode==pAngle->pStart&&pAngle->desStartPos.datum_jg_h>0x20)
			pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->desStartPos.datum_jg_h,CLS_LINEANGLE);
		else if(pNode==pAngle->pEnd&&pAngle->desEndPos.datum_jg_h>0x20)
			pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->desEndPos.datum_jg_h,CLS_LINEANGLE);
		else
			pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pNode->hFatherPart,CLS_LINEANGLE);
		if(pAngle->handle!=pNode->hFatherPart&&pFatherAngle&&
			pAngle->pStart&&pAngle->pEnd&&pFatherAngle->pStart&&pFatherAngle->pEnd)	
		{	//基准节点的父角钢非基准角钢时，才可以执行下面的代码	//wht 09-08-07
			f3dPoint father_line_vec,temp_vec,work_norm;
			father_line_vec=pFatherAngle->pEnd->Position(true)-pFatherAngle->pStart->Position(true);
			normalize(father_line_vec);
			temp_vec=pAngle->pEnd->Position(true)-pAngle->pStart->Position(true);
			normalize(temp_vec);
			work_norm=temp_vec^father_line_vec;
			normalize(work_norm);
			for(CLDSLineAngle *pLineAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);
			pLineAngle;pLineAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
			{
				if(pLineAngle==pAngle)
					continue;	//跳过基准角钢
				if(pLineAngle->pStart!=pNode&&pLineAngle->pEnd!=pNode)
					continue;
				if(pLineAngle->pStart==NULL||pLineAngle->pEnd==NULL)
					continue;
				temp_vec=pLineAngle->pEnd->Position(true)-pLineAngle->pStart->Position(true);
				normalize(temp_vec);
				f3dPoint norm=temp_vec^father_line_vec;
				normalize(norm);
				if(fabs(work_norm*norm)>EPS_COS)
					partset.append(pLineAngle);
			}
		}
		CLDSLineAngle *pSecLineAngle=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		if(partset.GetNodeNum()==1&&AfxMessageBox("Whether to design no plate's single-bolt-connection?",MB_YESNO)==IDYES)
#else 
		if(partset.GetNodeNum()==1&&AfxMessageBox("是否进行无板单螺栓连接?",MB_YESNO)==IDYES)
#endif
			pSecLineAngle=(CLDSLineAngle*)partset.GetFirst();
		if(ValidateOneBoltConnect(pNode,pAngle,pSecLineAngle))
			DesignBoltOnlyConnect(pNode,pAngle,pSecLineAngle,TRUE,TRUE);
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Command:","");
#else 
		pCmdLine->FillCmdLine("命令:","");
#endif
		return 0;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("命令:","");
#endif
	Command("1BoltEndAngle");
	return 0;
}
//设计所有的塔材端点单螺栓连接
void CLDSView::OnAllSingleXieNodeDesign() 
{
	CLDSNode *pNode;
	CLDSLineAngle *pAngle1=NULL,*pAngle2=NULL;
	long *id_arr=NULL;
	long n=g_pSolidSnap->GetLastSelectEnts(id_arr);
#ifdef AFX_TARG_ENU_ENGLISH
	if(MessageBox("Are you sure to design selected single-bolt-connection?",NULL,MB_YESNO)!=IDYES)
#else 
	if(MessageBox("你真的要设计所选中角钢的单螺栓端连接吗?",NULL,MB_YESNO)!=IDYES)
#endif
		return;
	CProcBarDlg *pProcDlg = new CProcBarDlg(this);
	pProcDlg->Create();
#ifdef AFX_TARG_ENU_ENGLISH
	pProcDlg->SetWindowText("Design process");
#else 
	pProcDlg->SetWindowText("设计进度");
#endif
	
	BOOL bIgnoreError=FALSE;
	for(int i=0;i<n;i++)
	{
		pNode=console.FromNodeHandle(id_arr[i]);
		pProcDlg->Refresh((i*100)/n);
		if(pNode==NULL)
			continue;
		try{
			PARTSET jgset;		//连接在基准节点上的角钢集合
			PARTSET partset[2];	//partset[0]搭接在X肢上的角钢集合，partset[1]搭接在Y肢上的角钢集合
			Ta.Node.push_stack();
			for(CLDSPart *pPart=Ta.Parts.GetFirst(CLS_LINEANGLE);pPart;pPart=Ta.Parts.GetNext(CLS_LINEANGLE))
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				if(pLinePart->pStart==pNode||pLinePart->pEnd==pNode)
					jgset.append(pLinePart);
			}
			Ta.Node.pop_stack();
			//将连接在基准节点上的角钢按搭接肢分类
			CLDSLineAngle *pLineAngle=NULL,*pFatherLineAngle=NULL;
			pFatherLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNode->hFatherPart,CLS_LINEANGLE);
			if(pFatherLineAngle)
			{
				f3dPoint norm_x,norm_y,father_line_vec;
				norm_x=pFatherLineAngle->get_norm_x_wing();
				norm_y=pFatherLineAngle->get_norm_y_wing();
				father_line_vec=pFatherLineAngle->End()-pFatherLineAngle->Start();
				normalize(father_line_vec);
				for(pLineAngle=(CLDSLineAngle*)jgset.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)jgset.GetNext())
				{
					if(pLineAngle==pFatherLineAngle)
						continue;
					f3dPoint line_vec=pLineAngle->End()-pLineAngle->Start();
					normalize(line_vec);
					f3dPoint norm=line_vec^father_line_vec;
					normalize(norm);
					//cos(30°)=0.866
					double a=norm*norm_x;
					double b=norm*norm_y;
					double c=norm_x*norm_y;
					if(fabs(norm*norm_x)>0.866)			//搭接在X肢上的射线角钢
						partset[0].append(pLineAngle);
					else if(fabs(norm*norm_y)>0.866)	//搭接在Y肢上的射线角钢
						partset[1].append(pLineAngle);
				}
			}	
			int i=0;
			for(i=0;i<2;i++)
			{
				if(partset[i].GetNodeNum()==1)
				{
					pAngle1 = (CLDSLineAngle*)partset[i][0];
					Ta.Node.push_stack();
					if(pAngle1->pStart->handle==pNode->handle)
					{	//始端螺栓设计个数为1，且未布置螺栓
						if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
								bIgnoreError=TRUE;
						}
						if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)	
							DesignBoltOnlyConnect(pNode,pAngle1,NULL,FALSE,FALSE);
					}
					else if(pAngle1->pEnd->handle==pNode->handle)	
					{	//终端螺栓设计个数为1，且未布置螺栓
						if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
								bIgnoreError=TRUE;
						}
						if((bIgnoreError||!bIgnoreError&&pAngle1->connectEnd.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(2,100)==0)
							DesignBoltOnlyConnect(pNode,pAngle1,NULL,FALSE,FALSE);
					}
					Ta.Node.pop_stack();
				}
				else if(partset[i].GetNodeNum()==2)
				{
					pAngle1 = (CLDSLineAngle*)partset[i][0];
					pAngle2 = (CLDSLineAngle*)partset[i][1];
					Ta.Node.push_stack();
					//保证两根连接材在同一平面上
					if(pAngle1->layer(2)!=pAngle2->layer(2))
						continue;	
					if(pAngle1->pStart->handle==pNode->handle)
					{	//始端螺栓设计个数为1，且未布置螺栓
						if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
								bIgnoreError=TRUE;
						}
						if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)
							DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
					}
					else if(pAngle1->pEnd->handle==pNode->handle)
					{	//终端螺栓设计个数为1，且未布置螺栓
						if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("是否忽略所有射线角钢端螺栓数不为1的错误?",MB_YESNO)==IDYES)
#endif
								bIgnoreError=TRUE;
						}
						if((bIgnoreError||!bIgnoreError&&pAngle1->connectEnd.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(2,100)==0)
							DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
					}
					/*
					if(pAngle2->pStart->handle==pNode->handle)
					{	//始端螺栓设计个数为1，且未布置螺栓
					if(pAngle2->connectStart.wnConnBoltN==1&&pAngle2->GetLocalLsCount(1)==0)
					DesignBoltOnlyConnect(pNode,pAngle2,FALSE,FALSE);
					}
					else if(pAngle2->pEnd->handle==pNode->handle)
					{	//终端螺栓设计个数为1，且未布置螺栓
					if(pAngle2->connectEnd.wnConnBoltN==1&&pAngle2->GetLocalLsCount(2)==0)
					DesignBoltOnlyConnect(pNode,pAngle2,FALSE,FALSE);
					}*/
					Ta.Node.pop_stack();
				}
			}
		}
		catch(char *sError)
		{
			AfxMessageBox(sError);
			g_pSolidDraw->SetEntSnapStatus(pNode->handle,FALSE);
		}
	}
	pProcDlg->DestroyWindow();
	delete pProcDlg;
}
/* 胡笑蓉还未完成，暂时不启用　wjh-2015.4.12
void CLDSView::OnMirCreateWBConnect()
{
	//m_nPrevCommandID=ID_MIR_CREATE_WB_CONNECT;
	//m_sPrevCommandName="重复对称生成无板连接";
	//Command("MirCreateWBConnect");
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_POINT);
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();

	CString cmdStr="";
	char sText[MAX_TEMP_CHAR_100+1];
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//-----vvvvvvv-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
	if(!LockFunc())
		return;
	CUndoOperObject undo(&Ta,true);
	try{
		//1.切换到单线图状态便于选择节点
		g_pSolidSet->SetDisplayType(DISP_LINE);
		g_pSolidDraw->Draw();
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FillCmdLine("请选择要对称的无板连接的定位基准节点:","");
		CLDSNode *pBaseNode=NULL;
		while(1)
		{
			f3dPoint* point;
			int ret = g_pSolidSnap->SnapPoint(point);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return;// 0;
			}
			else if(ret>0)
			{
				pBaseNode=console.FromNodeHandle(point->ID);
				if(pBaseNode)
				{
					g_pSolidDraw->SetEntSnapStatus(point->ID);
					break;
				}
			}
		}
		Invalidate(FALSE);
		pCmdLine->FinishCmdLine();

		//虚拟板
		CLDSPlate * pVirtualPlate;
		pVirtualPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pVirtualPlate->cfgword=pBaseNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pVirtualPlate->designInfo.m_hBaseNode = pBaseNode->handle;

		//基准杆件
		CSuperSmartPtr<CLDSLinePart> pBasePart;
		if(pBaseNode->m_cPosCalType!=4)
		{	//非交叉节点
			pBasePart=(CLDSLinePart*)console.FromPartHandle(pBaseNode->hFatherPart,CLS_LINEPART);
			if(pBasePart.IsHasPtr())
				pVirtualPlate->designInfo.m_hBasePart = pBasePart->handle;
			else
				throw "未找到合法的节点父杆件!";
		}
		if(pBasePart)
			pVirtualPlate->designInfo.m_hBasePart = pBasePart->handle;
		//无板连接均为单面板
		pVirtualPlate->face_N = 1;
		//designJdb.SetViewFlag(m_eViewFlag);
		if(pBaseNode->m_cPosCalType!=4)
		{	//普通连接板
			pVirtualPlate->jdb_style = 0;
			//if(!designJdb.DesignCommonPlank(pCurPlate))
				//throw "设计失败";
		}
		else //if(iPlateFaceType==4)//设计的是交叉板(需要特殊处理)
		{
			pVirtualPlate->jdb_style = 1;
			//if(!designJdb.DesignCrossPlate(pCurPlate))
				//throw "设计失败";
		}

		//2.框选连接角钢
		CSuperSmartPtr<CLDSPart>pPart;
		CLDSLinePart* pSelLinePart;
		LINEPARTSET partset;
		CHashTable<CLDSLinePart*>partsetTable;
		partsetTable.CreateHashTable(100);
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		pCmdLine->FillCmdLine("请选择该节点板基本面所连接的所有杆件:","");
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_LINEFLAT));
		((CMainFrame*)AfxGetMainWnd())->GetLDSView()->OpenWndSel();
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			//根据句柄字符串添加构件
			long *id_arr1=NULL,arr1_len=0;;
			if(cmdStr.GetLength()>0)
			{
				id_arr1=new long[cmdStr.GetLength()];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%s",cmdStr);
				for(char* key=strtok(sText,",");key;key=strtok(NULL,","))
				{
					long h;
					sscanf(key,"%X",&h);
					pPart=console.FromPartHandle(h);
					id_arr1[arr1_len]=h;
					arr1_len++;
				}
			}
			long *id_arr;
			int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
			for(int i=0;i<arr1_len+n;i++)
			{
				if(i<arr1_len)
					pPart=console.FromPartHandle(id_arr1[i]);
				else
					pPart=console.FromPartHandle(id_arr[i-arr1_len]);
				if(pPart.IsHasPtr()&&pPart->IsLinePart()&&
					pPart.LinePartPointer()->pStart!=NULL&&pPart.LinePartPointer()->pEnd!=NULL)
				{
					long hGroupFatherAngle=0;
					if(pPart.LinePartPointer()->IsAngle())
						hGroupFatherAngle=pPart.LineAnglePointer()->group_father_jg_h;
					if(!partsetTable.GetValueAt(pPart->handle,pSelLinePart)&&(hGroupFatherAngle==0||!partsetTable.GetValueAt(hGroupFatherAngle,pSelLinePart)))
					{		
						partsetTable.SetValueAt(pPart->handle,pPart.LinePartPointer());
						partset.append(pPart.LinePartPointer());
					}
				}
			}
			if(id_arr1)
				delete []id_arr1;
			g_pSolidDraw->ReleaseSnapStatus();
		}
		else
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		if(partset.GetNodeNum()<2)
		{
			g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle, FALSE);
			throw "至少应有两根杆件才算是连接设计";
		}
		CDesignLjPartPara desLjPartPara;
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			desLjPartPara.hPart=pPart->handle;
			desLjPartPara.iFaceNo = 1;
			desLjPartPara.m_nClassTypeId = pPart->GetClassTypeId();
			if(pPart.LinePartPointer()->pStart->handle==pVirtualPlate->designInfo.m_hBaseNode)
				desLjPartPara.start0_end1=0;
			else if(pPart.LinePartPointer()->pEnd->handle==pVirtualPlate->designInfo.m_hBaseNode)
				desLjPartPara.start0_end1=1;
			else
				desLjPartPara.start0_end1=2;
			if(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle* pLineAngle=pPart.LineAnglePointer();
				double justify_x=pLineAngle->get_norm_x_wing()*pVirtualPlate->ucs.axis_z;
				double justify_y=pLineAngle->get_norm_y_wing()*pVirtualPlate->ucs.axis_z;
				if(fabs(justify_x)>fabs(justify_y))//&&justify_x>EPS_COS2)
					desLjPartPara.angle.cur_wing_x0_y1=0;//将来应考虑引入杆件在制弯面情况　wjh-2015.1.05
				else //if(fabs(justify_x)<fabs(justify_y))//&&justify_y>EPS_COS2)
					desLjPartPara.angle.cur_wing_x0_y1=1;
				double max_d=0;
				for(CLsRef* pLsRef=pVirtualPlate->GetFirstLsRef();pLsRef;pLsRef=pVirtualPlate->GetNextLsRef())
				{
					CLDSBolt* pBolt=pLsRef->GetLsPtr();
					if(pLineAngle->FindLsByHandle(pBolt->handle)&&pBolt->get_d()>max_d)
						max_d=pBolt->get_d();
				}
				LSSPACE_STRU LsSpace;
				if(!GetLsSpace(LsSpace,ftoi(max_d)))
				{	//防止未引入螺栓先加入射线杆件的情况
					if(pLineAngle->GetWidth()<63)
						LsSpace.EndSpace=25;
					else if(pLineAngle->GetWidth()<=110)
						LsSpace.EndSpace=30;
					else
						LsSpace.EndSpace=40;
				}
				desLjPartPara.ber_space=desLjPartPara.wing_space=desLjPartPara.end_space=LsSpace.EndSpace;
				desLjPartPara.angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
			}
			pVirtualPlate->designInfo.partList.append(desLjPartPara);
		}
		pVirtualPlate->DesignPlate();
		CMirMsgDlg dlg;
		if(dlg.DoModal()==IDOK)
			MirTaAtom(pVirtualPlate,dlg.mirmsg);
		if(pVirtualPlate->GetThick()<=0)
		{
			console.DispPartSet.DeleteNode(pVirtualPlate->handle);
			console.DeletePart(pVirtualPlate->handle);
		}
		pCmdLine->FillCmdLine("命令:","");
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	g_pSolidDraw->ReleaseSnapStatus();
	ReleaseFunc();
}
*/
#endif