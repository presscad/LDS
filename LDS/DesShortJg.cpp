//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "Dlg.h"
#include "UserDefOper.h"
#include "ScrTools.h"
#include "FootDesignDlg.h"
#include "Query.h"
#include "PromptDlg.h"
#include "ShortJgDefDlg.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"
#include "LogFile.h"
#include "DesignOuterAttachDlg.h"
#include "MirTaAtom.h"
#include "SnapTypeVerify.h"
#include "KeypointLifeObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
////////////////////////////////////////////////////////////////
//专用于OnHandShortJg() 
//返回四点中距离最近两点间距(前两点为一组,后两点为一组,组内不计算)离值,同时cJunction中返回接合状态(点号位置1)
static CShortJgDefDlg defshortjgdlg;
static CDesignOuterAttachDlg designOuterAttachDlg;
static double JunctionDiscern(const f3dPoint& pt1,const f3dPoint& pt2,const f3dPoint& pt3,const f3dPoint& pt4,char* cJunction)
{
	double dd1=DISTANCE(pt1,pt3);
	double dd2=DISTANCE(pt1,pt4);
	double dd3=DISTANCE(pt2,pt3);
	double dd4=DISTANCE(pt2,pt4);
	double min_dd=dd1;
	char junction=0x05;
	if(dd1<dd2&&dd1<dd3&&dd1<dd4)
	{
		junction=0x05;	//0x01+0x04
		min_dd=dd1;
	}
	else if(dd2<dd1&&dd2<dd3&&dd2<dd4)
	{
		junction=0x09;	//0x01+0x08
		min_dd=dd2;
	}
	else if(dd3<dd1&&dd3<dd2&&dd3<dd4)
	{
		junction=0x06;	//0x02+0x04
		min_dd=dd3;
	}
	else if(dd4<dd1&&dd4<dd2&&dd4<dd3)
	{
		junction=0x0a;	//0x02+0x08
		min_dd=dd4;
	}
	if(cJunction!=NULL)
		*cJunction=junction;
	return min_dd;
}
//识别判断pRod1与pRod2的连接端，有共同端节点时返回值为0,否则返回距离最近的端点距离
//bOnlySameNode=true：表示只考虑同端节点情况
double RodJunctionDiscern(CLDSLinePart* pRod1,CLDSLinePart* pRod2,char* cJunction,bool bOnlySameNode=false)
{
	char junction=0;
	if(pRod1->pStart!=NULL && pRod1->pStart==pRod2->pStart)
		junction=0x05;
	else if(pRod1->pStart!=NULL && pRod1->pStart==pRod2->pEnd)
		junction=0x09;
	else if(pRod1->pEnd!=NULL && pRod1->pEnd==pRod2->pStart)
		junction=0x06;
	else if(pRod1->pEnd!=NULL && pRod1->pEnd==pRod2->pEnd)
		junction=0x0a;
	else if(!bOnlySameNode)
		return JunctionDiscern(pRod1->Start(),pRod1->End(),pRod2->Start(),pRod2->End(),cJunction);
	else	//返回一大数
		return 1e8;
	if(cJunction!=NULL)
		*cJunction=junction;
	return 0;
}
//返回值, 0:两肢连接内包铁;1:X肢为连接肢;2:Y肢为连接肢
static int WorkWingDiscern(CLDSLineAngle* pAngle,BOLTSET& boltSet)
{
	char work_wing=0;
	f3dPoint wing_x_norm=pAngle->get_norm_x_wing();
	f3dPoint wing_y_norm=pAngle->get_norm_y_wing();
	for(CLDSBolt* pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		double ddx=fabs(wing_x_norm*pBolt->get_norm());
		double ddy=fabs(wing_y_norm*pBolt->get_norm());
		if( ddx>ddy && ddx>EPS_COS2)
			work_wing |= 0x01;	//有X肢上螺栓
		else if(ddy>EPS_COS2)
			work_wing |= 0x02;	//有Y肢上螺栓
	}
	if(work_wing==0x03)
		work_wing=0;
	return work_wing;
}
struct CONNECT_PLATE_INFO{
	CLDSPlate* pConnectPlate;
	char cWorkWing;
	CONNECT_PLATE_INFO(){
		pConnectPlate=NULL;
		cWorkWing=0;
	}
};
//查找添加短角钢时,以选中节点为基点,选中角钢为连接射线角钢的关联钢板及工作肢信息
static int SearchRelationPlates(CLDSNode* pNode, CLDSLineAngle* pRayAngle1,CLDSLineAngle* pRayAngle2,
								CONNECT_PLATE_INFO arrConnInfo[2])
{
	char work_wing=0;
	if(pRayAngle1==NULL || pRayAngle2==NULL)
		return NULL;
	arrConnInfo[0].pConnectPlate=arrConnInfo[1].pConnectPlate=NULL;
	arrConnInfo[0].cWorkWing=arrConnInfo[1].cWorkWing=0;
	BOOL pushed=Ta.Parts.push_stack();
	for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if(pPlate->designInfo.m_hBaseNode!=pNode->handle)
			continue;	
		CDesignLjPartPara *pLjPara=pPlate->designInfo.FromHandle(pRayAngle1->handle);
		if(pLjPara==NULL)
			continue;
		if(pLjPara->angle.cur_wing_x0_y1==0)
			work_wing = 0x01;
		else if(pLjPara->angle.cur_wing_x0_y1==1)
			work_wing = 0x02;
		pLjPara=pPlate->designInfo.FromHandle(pRayAngle2->handle);
		if(pLjPara==NULL)
			continue;
		if(pLjPara->angle.cur_wing_x0_y1==0)
			work_wing |= 0x04;
		else if(pLjPara->angle.cur_wing_x0_y1==1)
			work_wing |= 0x08;
		if(arrConnInfo[0].pConnectPlate==NULL)
		{
			arrConnInfo[0].pConnectPlate=pPlate;
			arrConnInfo[0].cWorkWing=work_wing;
		}
		else
		{
			arrConnInfo[1].pConnectPlate=pPlate;
			arrConnInfo[1].cWorkWing=work_wing;
			break;
		}
	}
	Ta.Parts.pop_stack(pushed);
	//变坡处的角钢端节点可能与连接板的设计节点不是同一个节点，在此重新查找一遍连接板  wxc-2017.1.10
	if(arrConnInfo[0].pConnectPlate==NULL && arrConnInfo[1].pConnectPlate==NULL)
	{
		BOOL pushed=Ta.Parts.push_stack();
		for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{	//只处理多面板的情况
			if(pPlate->GetFaceN()<2)
				continue;	//
			CDesignLjPartPara *pLjPara1=pPlate->designInfo.FromHandle(pRayAngle1->handle);
			CDesignLjPartPara *pLjPara2=pPlate->designInfo.FromHandle(pRayAngle2->handle);
			if(pLjPara1==NULL || pLjPara2==NULL)
				continue;
			if(pLjPara1->angle.cur_wing_x0_y1==0)
				work_wing = 0x01;
			else if(pLjPara1->angle.cur_wing_x0_y1==1)
				work_wing = 0x02;
			if(pLjPara2->angle.cur_wing_x0_y1==0)
				work_wing |= 0x04;
			else if(pLjPara2->angle.cur_wing_x0_y1==1)
				work_wing |= 0x08;
			arrConnInfo[0].pConnectPlate=pPlate;
			arrConnInfo[0].cWorkWing=work_wing;
			break;
		}
		Ta.Parts.pop_stack(pushed);
	}
	if(arrConnInfo[1].pConnectPlate!=NULL)
		return 2;
	else if(arrConnInfo[0].pConnectPlate!=NULL)
		return 1;
	else
		return 0;
}
//查找添加短角钢时,以选中节点为基点,选中角钢为连接射线角钢的关联钢板及工作肢信息
static int SearchRelationPlates(CLDSNode* pNode, CLDSLineAngle* pRayAngle,CONNECT_PLATE_INFO &connectInfo,const double* vxWorkNormal=NULL)
{
	char work_wing=0;
	if(pRayAngle==NULL)
		return NULL;
	CONNECT_PLATE_INFO connectInfoArr[2];
	BOOL pushed=Ta.Parts.push_stack();
	char ciAnchorWing=0;
	if (pRayAngle->pStart==pNode)
		ciAnchorWing=pRayAngle->ciStartAnchorWing;
	else if(pRayAngle->pEnd==pNode)
		ciAnchorWing=pRayAngle->ciEndAnchorWing;
	double dfCosaFactor=0;
	for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if (pPlate->designInfo.m_hBaseNode!=pNode->handle)
		{	//存在钢板基准节点非当前节点的情况，故此处不宜作为跳出条件 wjh-2019.12.30
			CLDSNode* pBaseNode=pNode->BelongModel()->FromNodeHandle(pPlate->designInfo.m_hBaseNode);
			if (pBaseNode==NULL||DISTANCE(pBaseNode->xOriginalPos,pNode->xOriginalPos)>200)
				continue;
		}
		CDesignLjPartPara *pLjPara=pPlate->designInfo.FromHandle(pRayAngle->handle);
		if(pLjPara==NULL)
			continue;
		if(pLjPara->angle.cur_wing_x0_y1==0)
			work_wing = 0x01;
		else if(pLjPara->angle.cur_wing_x0_y1==1)
			work_wing = 0x02;
		if (vxWorkNormal)
		{
			double cosa=fabs(GEPOINT(vxWorkNormal)*pPlate->ucs.axis_z);
			if (cosa>dfCosaFactor)
			{
				dfCosaFactor=cosa;
				connectInfoArr[0].pConnectPlate=pPlate;
				connectInfoArr[0].cWorkWing=work_wing;
				continue;
			}
		}
		else if (ciAnchorWing==pLjPara->angle.cur_wing_x0_y1+1)
		{
			connectInfoArr[0].pConnectPlate=pPlate;
			connectInfoArr[0].cWorkWing=work_wing;
			break;
		}
		else
		{
			if (connectInfoArr[0].pConnectPlate==NULL)
			{
				connectInfoArr[0].pConnectPlate=pPlate;
				connectInfoArr[0].cWorkWing=work_wing;
			}
			else if (connectInfoArr[1].pConnectPlate==NULL)
			{
				connectInfoArr[1].pConnectPlate=pPlate;
				connectInfoArr[1].cWorkWing=work_wing;
				break;
			}
		}
	}
	Ta.Parts.pop_stack(pushed);
	if(connectInfoArr[1].pConnectPlate)
	{	//在同一个节点位置的角钢上有两个板，通过视图法向进行取舍
		f3dPoint viewNorm,plateNorm1,plateNorm2;
		CDisplayView *pView=console.GetActiveView();
		if(pView)
			viewNorm=pView->ucs.axis_z;
		plateNorm1=connectInfoArr[0].pConnectPlate->ucs.axis_z;
		plateNorm2=connectInfoArr[1].pConnectPlate->ucs.axis_z;
		if(!viewNorm.IsZero()&&fabs(viewNorm*plateNorm2)>fabs(viewNorm*plateNorm1))
		{
			connectInfo.pConnectPlate=connectInfoArr[1].pConnectPlate;
			connectInfo.cWorkWing=connectInfoArr[1].cWorkWing;
		}
		else
		{
			connectInfo.pConnectPlate=connectInfoArr[0].pConnectPlate;
			connectInfo.cWorkWing=connectInfoArr[0].cWorkWing;
		}
		return 1;
	}
	else if(connectInfoArr[0].pConnectPlate)
	{
		connectInfo.pConnectPlate=connectInfoArr[0].pConnectPlate;
		connectInfo.cWorkWing=connectInfoArr[0].cWorkWing;
		return 1;
	}
	else
		return 0;
}
static f3dPoint CalMidIntersPos(CLDSNode *pSelNode,CLDSLineAngle *pFirstJg,CLDSLineAngle *pSecJg,double manu_space)
{
	f3dLine line_first,line_sec;
	f3dPoint inters,line_pick,angle_norm,face_norm,first_vec,sec_vec,bi_vec;
	BOOL arrLinkToStart1OrEnd0[2]={FALSE,FALSE};
	CLDSLinePartPtr rodArr[2]={pFirstJg,pSecJg};
	for(int i=0;i<2;i++)
	{
		if(rodArr[i]->pStart==pSelNode)
			arrLinkToStart1OrEnd0[i]=TRUE;
		else if(rodArr[i]->pEnd==pSelNode)
			arrLinkToStart1OrEnd0[i]=FALSE;
		else if(DISTANCE(pSelNode->Position(),rodArr[i]->Start())>DISTANCE(pSelNode->Position(),rodArr[i]->End()))
			arrLinkToStart1OrEnd0[i]=FALSE;
		else
			arrLinkToStart1OrEnd0[i]=TRUE;
	}
	if(arrLinkToStart1OrEnd0[0])
	{
		line_first.startPt = pFirstJg->Start();
		line_first.endPt   = pFirstJg->End();
		first_vec = line_first.endPt-line_first.startPt;
	}
	else 
	{
		line_first.startPt = pFirstJg->End();
		line_first.endPt   = pFirstJg->Start();
		first_vec = line_first.endPt-line_first.startPt;
	}
	
	if(arrLinkToStart1OrEnd0[1])
	{
		line_sec.startPt = pSecJg->Start();
		line_sec.endPt   = pSecJg->End();
		sec_vec = line_sec.endPt-line_sec.startPt;
	}
	else 
	{
		line_sec.startPt = pSecJg->End();
		line_sec.endPt   = pSecJg->Start();
		sec_vec = line_sec.endPt-line_sec.startPt;
	}
	normalize(first_vec);
	normalize(sec_vec);
	angle_norm = first_vec^sec_vec;
	normalize(angle_norm);
	if(angle_norm.mod()<EPS)
		face_norm = first_vec;
	else
	{
		bi_vec = first_vec+sec_vec;
		normalize(bi_vec);
		face_norm = bi_vec^angle_norm;
		normalize(face_norm);
	}
	//计算内楞位置(短角钢端点实际坐标)
	bi_vec = pFirstJg->get_norm_x_wing()+pFirstJg->get_norm_y_wing();
	normalize(bi_vec);
	double bi_angle = cal_angle_of_2vec(bi_vec,pFirstJg->get_norm_x_wing());
	double offset = (pFirstJg->GetThick()+manu_space)/cos(bi_angle);
	line_pick = line_first.startPt-bi_vec*offset;
	//计算角平分线所在面的拾取点 face_pick	wht 10-09-16
	project_point(line_sec.startPt,line_first.startPt,angle_norm);
	project_point(line_sec.endPt,line_first.startPt,angle_norm);
	f3dPoint face_pick;
	Int3dll(line_first,line_sec,face_pick);	//以前使用line_first.startPt做为面拾取点
	Int3dlf(inters,line_pick,first_vec,face_pick,face_norm);
	return inters;
}
static double CalShortAngleExtendLength(CLDSLineAngle* pRayAngle, BOLTSET& boltSet,const f3dPoint& basePos,const f3dPoint& lenStdVec)
{
	bool inited=false;
	double max_len_pos=0;
	long bolt_d=0;
	for(CLDSBolt* pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		//当用户用外贴材输入连接短角钢时，螺栓很有可能不在当前选中角钢内，此时为了统一UI，不应进行如下筛选判断 wjh-2016.6.15
		//if(pRayAngle->FindLsByHandle(pBolt->handle)==NULL)
		//	continue;
		double len_pos=lenStdVec*(pBolt->ucs.origin-basePos);
		if(!inited)
		{
			max_len_pos=len_pos;
			bolt_d=pBolt->get_d();
			inited=true;
		}
		else if(max_len_pos<len_pos)
		{
			max_len_pos=len_pos;
			bolt_d=pBolt->get_d();
		}
	}
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,bolt_d);
	return max_len_pos+LsSpace.EndSpace;;
}
static double CalShortAngleWingNormOffset(CLDSLineAngle* pRayAngle,CLDSPlate* pRelaPlate)
{
	if(pRelaPlate==NULL)
		return 0;
	else
		return pRelaPlate->Thick;
}
static void SetShortAngleWingNorm(CLDSLineAngle* pShortAngle,char cAttachWing,CLDSLineAngle* pDatumAngle,char cDatumWing)
{
	if(cAttachWing=='X')
	{
		pShortAngle->des_norm_x.bSpecific=TRUE;
		pShortAngle->des_norm_x.hViceJg=0;
		pShortAngle->des_norm_x.spec_norm.hVicePart=pDatumAngle->handle;
		pShortAngle->des_norm_x.spec_norm.norm_style=1;	//1:表示根据角钢肢法线定义矢量方向
		pShortAngle->des_norm_x.spec_norm.direction=1;	//1:表示由外向里为正方向
		pShortAngle->des_norm_x.spec_norm.norm_wing=cDatumWing - 'X';
		pShortAngle->des_norm_y.bSpecific=FALSE;
		pShortAngle->des_norm_y.bByOtherWing=TRUE;
		pShortAngle->cal_x_wing_norm();
		pShortAngle->cal_y_wing_norm();
	}
	else
	{
		pShortAngle->des_norm_y.bSpecific=TRUE;
		pShortAngle->des_norm_y.hViceJg=0;
		pShortAngle->des_norm_y.spec_norm.hVicePart=pDatumAngle->handle;
		pShortAngle->des_norm_y.spec_norm.norm_style=1;	//1:表示根据角钢肢法线定义矢量方向
		pShortAngle->des_norm_y.spec_norm.direction=1;	//1:表示由外向里为正方向
		pShortAngle->des_norm_y.spec_norm.norm_wing=cDatumWing - 'X';
		pShortAngle->des_norm_x.bSpecific=FALSE;
		pShortAngle->des_norm_x.bByOtherWing=TRUE;
		pShortAngle->cal_y_wing_norm();
		pShortAngle->cal_x_wing_norm();
	}
}
//短角钢位于基准角钢X肢时进行设计
static bool DesWingXShortJg(CLDSNode *pSelNode,CLDSLineAngle *pFirstJg,CLDSLineAngle *pSecJg,
							f3dPoint base_pos,f3dPoint base_vec,f3dPoint other_vec,SHORT_ANGLE_INFO* pDesignInfo,
							BOLTSET& boltSet,CLDSPlate* pRelaPlate)
{
	CLDSBolt* pBolt;
	CSmartPtr<CLDSLineAngle>pShortSegJg1,pShortSegJg2;
	f3dPoint start_pos,end_pos;
	double wing_wide,wing_thick;
	restore_JG_guige(pDesignInfo->sizeStr,wing_wide,wing_thick);

	double wing_norm_offset=CalShortAngleWingNormOffset(pFirstJg,pRelaPlate);
	double justify=base_vec*other_vec;
	bool bended=false;
	if(justify>=-0.9994)
		bended=true;	//制弯角大于2°
	else if(justify>=-EPS_COS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(AfxMessageBox("bending angle is too small,to design according to bending angle(Y)or design according to straight angle(N)?",MB_YESNO)==IDYES)
#else
		if(AfxMessageBox("制弯角过小，你确认按制弯角钢设计(Y)还是直角钢设计(N)?",MB_YESNO)==IDYES)
#endif
			bended=true;
	}
	SEGI current_segI=pFirstJg->iSeg;
	if(pSelNode!=NULL)
		current_segI=pSelNode->iSeg;
	BOOL arrLinkToStart1OrEnd0[2]={FALSE,FALSE};
	CLDSLinePartPtr rodArr[2]={pFirstJg,pSecJg};
	for(int i=0;i<2;i++)
	{
		if(rodArr[i]->pStart==pSelNode)
			arrLinkToStart1OrEnd0[i]=TRUE;
		else if(rodArr[i]->pEnd==pSelNode)
			arrLinkToStart1OrEnd0[i]=FALSE;
		else if(DISTANCE(pSelNode->Position(),rodArr[i]->Start())>DISTANCE(pSelNode->Position(),rodArr[i]->End()))
			arrLinkToStart1OrEnd0[i]=FALSE;
		else
			arrLinkToStart1OrEnd0[i]=TRUE;
	}
	//else if(justify<-EPS_COS)
	if(!bended)	//两衔接角钢大致共线,短角钢不需火曲
	{
		if(justify>-EPS_COS)	//以第一根（基准）角钢延伸方向修正另一连接方向 wjh-2013.10.12
			other_vec=-base_vec;
		double base_extend_len=CalShortAngleExtendLength(pFirstJg,boltSet,base_pos,base_vec);
		double vice_extend_len=CalShortAngleExtendLength(pSecJg,boltSet,base_pos,other_vec);
		start_pos = base_pos+base_vec*base_extend_len;
		end_pos	  = base_pos+other_vec*vice_extend_len;
		start_pos +=pFirstJg->get_norm_x_wing()*wing_norm_offset;
		end_pos +=pFirstJg->get_norm_x_wing()*wing_norm_offset;
		pShortSegJg1=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg1->iSeg=current_segI;
		pShortSegJg1->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg1->SetPartNo(pDesignInfo->sPartNo);
		pShortSegJg1->SetStart(start_pos);
		pShortSegJg1->SetEnd(end_pos);
		pShortSegJg1->set_wing_wide(wing_wide);
		pShortSegJg1->set_wing_thick(wing_thick);
		pShortSegJg1->desStartPos.jgber_cal_style=pShortSegJg1->desEndPos.jgber_cal_style=1;	//指定楞点定位
		pShortSegJg1->desStartPos.datumPoint.SetPosition(pShortSegJg1->Start());
		pShortSegJg1->desEndPos.datumPoint.SetPosition(pShortSegJg1->End());
		pShortSegJg1->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg1->desStartPos.wing_y_offset.SetOffset();
		pShortSegJg1->desEndPos.wing_x_offset.SetOffset();
		pShortSegJg1->desEndPos.wing_y_offset.SetOffset();
		if(arrLinkToStart1OrEnd0[0])	//始端点连接
			SetShortAngleWingNorm(pShortSegJg1,'X',pFirstJg,'X');//填写肢法线设计参数
		else
			SetShortAngleWingNorm(pShortSegJg1,'Y',pFirstJg,'X');//填写肢法线设计参数
		//添加短角钢上的螺栓
		UCS_STRU ucs;
		f3dPoint ls_pos;
		pFirstJg->getUCS(ucs);
		double length = DISTANCE(pFirstJg->Start(),pFirstJg->End());
		/*for(CLsRef *pLsRef=pFirstJg->GetFirstLsRef();pLsRef;pLsRef=pFirstJg->GetNextLsRef())
		{
			if(fabs((*pLsRef)->get_norm()*pFirstJg->get_norm_x_wing())
				<fabs((*pLsRef)->get_norm()*pFirstJg->get_norm_y_wing()))
				continue;	//Y肢上的螺栓
			if((*pLsRef)->get_norm()*pFirstJg->get_norm_x_wing()<0)//螺栓从外向里穿
				(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
			ls_pos = (*pLsRef)->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			if(pFirstJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else
			{
				if(ls_pos.z>length-base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
		}*/
		pSecJg->getUCS(ucs);
		length = DISTANCE(pSecJg->Start(),pSecJg->End());
		//for(pLsRef=pSecJg->GetFirstLsRef();pLsRef;pLsRef=pSecJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			if(fabs(pBolt->get_norm()*pFirstJg->get_norm_x_wing())
				<fabs(pBolt->get_norm()*pFirstJg->get_norm_y_wing()))
				continue;	//Y肢上的螺栓
			if(pBolt->get_norm()*pFirstJg->get_norm_x_wing()<0)//螺栓从外向里穿
				pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
			ls_pos = pBolt->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			/*if(pSecJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<vice_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
			else*/
			{
				//if(ls_pos.z>length-vice_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		//存在相关钢板,引入属于短角钢的螺栓 wht 11-01-15
		if(pRelaPlate)
		{
			for(CLsRef* pLsRef=pRelaPlate->GetFirstLsRef();pLsRef;pLsRef=pRelaPlate->GetNextLsRef())
			{
				if(pShortSegJg1->IsLsInWing(pLsRef->GetLsPtr()))
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
		}
		pShortSegJg1->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg1->GetSolidPartObject());
	}
	else
	{
		f3dPoint wing_norm[2];
		f3dPoint face_pick,face_norm,line_pick;
		wing_norm[0] = pFirstJg->get_norm_x_wing();
		if(arrLinkToStart1OrEnd0[0])
			line_pick=pFirstJg->Start();
		else
			line_pick=pFirstJg->End();
		char cSecondDatumWing='Y';
		if(wing_norm[0]*pSecJg->get_norm_y_wing()>wing_norm[0]*pSecJg->get_norm_x_wing())
			wing_norm[1]=pSecJg->get_norm_y_wing();
		else
		{
			wing_norm[1]=pSecJg->get_norm_x_wing();
			cSecondDatumWing='X';
		}
		if(arrLinkToStart1OrEnd0[1])
			face_pick=pSecJg->Start();
		else
			face_pick=pSecJg->End();
		face_pick+=wing_norm[1]*wing_norm_offset;
		line_pick+=wing_norm[0]*wing_norm_offset;
		f3dPoint inters=base_pos;
		bool datum_by_inters=true;	//中间为曲点是求交计算所得（需要指定坐标）
		short junction=1;	//默认基准角钢终端为火曲连接端
		double complement=0;
		if(base_pos==pFirstJg->Start())
			junction=0;	//基准角钢始端为火曲连接端

		GECS cs;
		cs.axis_y=wing_norm[0];
		cs.axis_z=base_vec;
		cs.axis_x=cs.axis_y^cs.axis_z;
		GEPOINT bend_vec=cs.TransVToCS(other_vec);
		int inters_style=0;
		//刘伟建议对于外贴材都应支持选择是否简化为直曲，此处去掉判断(bend_vec.x/bend_vec.y>5) wht 17-05-04
#ifdef AFX_TARG_ENU_ENGLISH
		if(fabs(bend_vec.y)>EPS&&fabs(bend_vec.x)>fabs(bend_vec.y)&&AfxMessageBox("bending gives priority to datum angle's X leg,can Y leg bending be ignored to simplify process to straight bending?",MB_YESNO)==IDYES)
#else
		if(fabs(bend_vec.y)>EPS&&fabs(bend_vec.x)>fabs(bend_vec.y)&&AfxMessageBox("制弯以基准角钢X肢为主，是否可以忽略Y肢制弯角，以简化工艺为直曲?",MB_YESNO)==IDYES)
#endif
		{	//主要是X肢制弯忽略Y肢制弯角度
			bend_vec.y=0;
			other_vec=cs.TransVFromCS(bend_vec);
			normalize(other_vec);
			project_point(face_pick,line_pick,cs.axis_y);
			if(Int3dpl(f3dLine(line_pick,line_pick+base_vec),f3dLine(face_pick,face_pick+other_vec),inters)>0)
				inters_style=1;	//单肢火曲
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("failure in measuring short angle's bending point,default value adopted temporarily!");
		}
		else if(fabs(bend_vec.x)>EPS&&fabs(bend_vec.y)>fabs(bend_vec.x)&&AfxMessageBox("bending gives priority to datum angle's Y leg,can X leg bending be ignored to simplify process to straight bending?",MB_YESNO)==IDYES)
#else
				logerr.Log("短角钢制弯点计算失败，暂取默认值!");
		}
		else if(fabs(bend_vec.x)>EPS&&fabs(bend_vec.y)>fabs(bend_vec.x)&&AfxMessageBox("制弯以基准角钢Y肢为主，是否可以忽略X肢制弯角，以简化工艺为直曲?",MB_YESNO)==IDYES)
#endif
		{	//主要是Y肢制弯忽略Y肢制弯角度
			bend_vec.x=0;
			other_vec=cs.TransVFromCS(bend_vec);
			normalize(other_vec);
			project_point(face_pick,line_pick,cs.axis_x);
			if(Int3dpl(f3dLine(line_pick,line_pick+base_vec),f3dLine(face_pick,face_pick+other_vec),inters)>0)
				inters_style=1;	//单肢火曲
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("failure in measuring short angle's bending point,default value adopted temporarily!");
#else
				logerr.Log("短角钢制弯点计算失败，暂取默认值!");
#endif
		}
		else if(justify>=-0.9994)	//制弯角度较大>2°直接采取交点为制弯点
		{
			if(Int3dlf(inters,line_pick,base_vec,face_pick,wing_norm[1])>0)
				inters_style=2;	//双肢火曲
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("failure in measuring short angle's bending point,default value adopted temporarily!");
#else
				logerr.Log("短角钢制弯点计算失败，暂取默认值!");
#endif
		}
		if(inters_style>0)
			base_pos=inters;	//有效交点
		else	//因火曲角度太小，干扰误差太大等因素带来结果的错误，故选择原基准顶点法向偏移板厚 wjh-2013.10.12
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstJg->handle);
			CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please input the extending compensation measure from datum angle connecting end to the angle to be connected<0>:","");
#else
			pCmdLine->FillCmdLine("请输入自基准(当前选中)角钢连接端点向待连接角钢的延伸补偿量<0>:","");
#endif
			CString cmdStr;
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return false;
			if(cmdStr.GetLength()>0)
				complement=atof(cmdStr);
			base_pos+=wing_norm[0]*wing_norm_offset-complement*base_vec;
			datum_by_inters=false;	//中间火曲点为延基准角钢延伸方向偏移所得
		}
		double base_extend_len=CalShortAngleExtendLength(pFirstJg,boltSet,base_pos,base_vec);
		double vice_extend_len=CalShortAngleExtendLength(pSecJg,boltSet,base_pos,other_vec);
		start_pos = base_pos+base_vec*base_extend_len;
		end_pos	  = base_pos+other_vec*vice_extend_len;
		long huoqu_handle=Ta.GetNewHandle();
		pShortSegJg1=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg1->huoqu_handle=huoqu_handle;
		pShortSegJg1->SetStart(start_pos);
		pShortSegJg1->SetEnd(base_pos);
		pShortSegJg1->set_wing_wide(wing_wide);
		pShortSegJg1->set_wing_thick(wing_thick);
		pShortSegJg2=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg2->huoqu_handle=huoqu_handle;
		pShortSegJg2->SetStart(base_pos);
		pShortSegJg2->SetEnd(end_pos);
		pShortSegJg2->set_wing_wide(wing_wide);
		pShortSegJg2->set_wing_thick(wing_thick);
		pShortSegJg1->pEnd    = pSelNode;
		pShortSegJg2->pStart  = pSelNode;
		pShortSegJg1->desStartPos.datumPoint.SetPosition(pShortSegJg1->Start());
		pShortSegJg1->desEndPos.datumPoint.SetPosition(pShortSegJg1->End());
		pShortSegJg2->desStartPos.datumPoint.SetPosition(pShortSegJg2->Start());
		pShortSegJg2->desEndPos.datumPoint.SetPosition(pShortSegJg2->End());
		pShortSegJg1->iSeg=current_segI;
		pShortSegJg2->iSeg=current_segI;
		pShortSegJg1->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg2->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg1->SetPartNo(pDesignInfo->sPartNo);
		pShortSegJg2->SetPartNo(pDesignInfo->sPartNo);
		CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)console.AppendPart(CLS_POLYJOINT);
		pPolyJoint->cMaterial=pShortSegJg1->cMaterial;
		pPolyJoint->iSeg=pShortSegJg1->iSeg;
		pPolyJoint->SetPartNo(pShortSegJg1->GetPartNo());
		pPolyJoint->pPrevLinePart=pShortSegJg1;
		pPolyJoint->pNextLinePart=pShortSegJg2;
		//外短角钢，不用铲背 WJH-- 2004/09/21
		//pShortSegJg1->is_cut_back=pShortSegJg2->is_cut_back=2;
		
		if(arrLinkToStart1OrEnd0[0])	//始端点连接
		{
			SetShortAngleWingNorm(pShortSegJg1,'X',pFirstJg,'X');			//填写肢法线设计参数
			SetShortAngleWingNorm(pShortSegJg2,'X',pSecJg,cSecondDatumWing);//填写肢法线设计参数
			pShortSegJg1->huoqu_base_wing_x0_y1_both2=0;
			pShortSegJg2->huoqu_base_wing_x0_y1_both2=0;
		}
		else
		{
			SetShortAngleWingNorm(pShortSegJg1,'Y',pFirstJg,'X');			//填写肢法线设计参数
			SetShortAngleWingNorm(pShortSegJg2,'Y',pSecJg,cSecondDatumWing);//填写肢法线设计参数
			pShortSegJg1->huoqu_base_wing_x0_y1_both2=1;
			pShortSegJg2->huoqu_base_wing_x0_y1_both2=1;
		}

		pShortSegJg1->desStartPos.jgber_cal_style=1;
		pShortSegJg1->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg1->desStartPos.wing_y_offset.SetOffset();
		pShortSegJg1->LockStartPos();
		pShortSegJg1->LockEndPos();
		pShortSegJg1->bHuoQuEnd=TRUE;
		if(datum_by_inters)
			pShortSegJg1->desEndPos.jgber_cal_style=1;
		else
		{
			pShortSegJg1->desEndPos.jgber_cal_style=0;
			pShortSegJg1->desEndPos.spatialOperationStyle=0;	//指定基准点
			pShortSegJg1->desEndPos.datumPoint.datum_pos_style=1;	//角钢两端楞点为基准
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.hRod=pFirstJg->handle;
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.direction=junction;
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.offset_wing=0;	//Y肢
			sprintf(pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.norm_offset,"%s",(char*)CXhChar16(wing_norm_offset));
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.SetWingOffset();
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.len_offset_dist=-complement;
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.bIncOddEffect=false;
			pShortSegJg1->desEndPos.wing_x_offset.SetOffset();
			pShortSegJg1->desEndPos.wing_y_offset.SetOffset();
		}
		pShortSegJg2->LockStartPos();
		pShortSegJg2->LockEndPos();
		pShortSegJg2->bHuoQuStart=TRUE;
		//设定第二段子角钢的始端顶点搭在第一段子角钢末端顶点上
		pShortSegJg2->desStartPos.jgber_cal_style=0;
		pShortSegJg2->desStartPos.spatialOperationStyle=0;
		pShortSegJg2->desStartPos.datumPoint.datum_pos_style=1;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.hRod=pShortSegJg1->handle;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.direction=1;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.len_offset_dist=
			pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.wing_offset_style=4;
		strcpy(pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.norm_offset,"");
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.bIncOddEffect=FALSE;
		pShortSegJg2->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg2->desStartPos.wing_y_offset.SetOffset();

		pShortSegJg2->desEndPos.jgber_cal_style=1;
		pShortSegJg2->desEndPos.wing_x_offset.SetOffset();
		pShortSegJg2->desEndPos.wing_y_offset.SetOffset();
		//添加短角钢上的螺栓
		UCS_STRU ucs;
		f3dPoint ls_pos;
		pFirstJg->getUCS(ucs);
		double length = DISTANCE(pFirstJg->Start(),pFirstJg->End());
		//for(CLsRef *pLsRef=pFirstJg->GetFirstLsRef();pLsRef;pLsRef=pFirstJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			ls_pos = pBolt->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			/*if(pFirstJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<base_extend_len&&fabs(wing_norm[0]*(*pLsRef)->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_x_wing()>0)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else*/
			if(pFirstJg->FindLsByHandle(pBolt->handle))
			{
				//if(ls_pos.z>length-base_extend_len&&fabs(wing_norm[0]*pBolt->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					if(pBolt->get_norm()*pShortSegJg1->get_norm_x_wing()>0)//螺栓相对短角钢从里向外穿
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		pSecJg->getUCS(ucs);
		length = DISTANCE(pSecJg->Start(),pSecJg->End());
		//for(pLsRef=pSecJg->GetFirstLsRef();pLsRef;pLsRef=pSecJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			ls_pos = pBolt->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			/*if(pSecJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<vice_extend_len&&fabs(wing_norm[1]*pBolt->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg2->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg2->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					if(pBolt->get_norm()*pShortSegJg2->get_norm_x_wing()>0)//螺栓相对短角钢从里向外穿
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg2->handle,TRUE);
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
			else*/
			if(pSecJg->FindLsByHandle(pBolt->handle))
			{
				//if(ls_pos.z>length-vice_extend_len&&fabs(wing_norm[1]*pBolt->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg2->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg2->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					if(pBolt->get_norm()*pShortSegJg2->get_norm_x_wing()>0)//螺栓相对短角钢从里向外穿
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg2->handle,TRUE);
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		//存在相关钢板,引入属于短角钢的螺栓 wht 11-01-15
		if(pRelaPlate)
		{
			for(CLsRef* pLsRef=pRelaPlate->GetFirstLsRef();pLsRef;pLsRef=pRelaPlate->GetNextLsRef())
			{
				if(pShortSegJg1->IsLsInWing(pLsRef->GetLsPtr()))
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
				if(pShortSegJg2->IsLsInWing(pLsRef->GetLsPtr()))
				{
					pShortSegJg2->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg2->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
		}
		pPolyJoint->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPolyJoint->GetSolidPartObject());
		pShortSegJg1->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg1->GetSolidPartObject());
		pShortSegJg2->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg2->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	return true;
}
//短角钢位于基准角钢Y肢时进行设计
static bool DesWingYShortJg(CLDSNode *pSelNode,CLDSLineAngle *pFirstJg,CLDSLineAngle *pSecJg,
							f3dPoint base_pos, f3dPoint base_vec,f3dPoint other_vec,SHORT_ANGLE_INFO* pDesignInfo,
							BOLTSET& boltSet,CLDSPlate* pRelaPlate)
{
	CLDSBolt* pBolt;
	CSmartPtr<CLDSLineAngle>pShortSegJg1,pShortSegJg2;
	f3dPoint start_pos,end_pos;
	double wing_wide,wing_thick;
	restore_JG_guige(pDesignInfo->sizeStr,wing_wide,wing_thick);

	double wing_norm_offset=CalShortAngleWingNormOffset(pFirstJg,pRelaPlate);
	double justify=base_vec*other_vec;
	bool bended=false;
	if(justify>=-0.9994)
		bended=true;	//制弯角大于2°
	else if(justify>=-EPS_COS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(AfxMessageBox("bending angle is too small,to design according to bending angle(Y)or design according to straight angle(N)?",MB_YESNO)==IDYES)
#else
		if(AfxMessageBox("制弯角过小，你确认按制弯角钢设计(Y)还是直角钢设计(N)?",MB_YESNO)==IDYES)
#endif
			bended=true;
	}
	SEGI current_segI=pFirstJg->iSeg;
	if(pSelNode!=NULL)
		current_segI=pSelNode->iSeg;
	BOOL arrLinkToStart1OrEnd0[2]={FALSE,FALSE};
	CLDSLinePartPtr rodArr[2]={pFirstJg,pSecJg};
	for(int i=0;i<2;i++)
	{
		if(rodArr[i]->pStart==pSelNode)
			arrLinkToStart1OrEnd0[i]=TRUE;
		else if(rodArr[i]->pEnd==pSelNode)
			arrLinkToStart1OrEnd0[i]=FALSE;
		else if(DISTANCE(pSelNode->Position(),rodArr[i]->Start())>DISTANCE(pSelNode->Position(),rodArr[i]->End()))
			arrLinkToStart1OrEnd0[i]=FALSE;
		else
			arrLinkToStart1OrEnd0[i]=TRUE;
	}
	//else if(justify<-EPS_COS)
	//if(fabs(base_vec*other_vec)>EPS_COS)	//两衔接角钢大致共线,短角钢不需火曲
	if(!bended)	//两衔接角钢大致共线,短角钢不需火曲
	{
		if(justify>-EPS_COS)	//以第一根（基准）角钢延伸方向修正另一连接方向 wjh-2013.10.12
			other_vec=-base_vec;
		double base_extend_len=CalShortAngleExtendLength(pFirstJg,boltSet,base_pos,base_vec);
		double vice_extend_len=CalShortAngleExtendLength(pSecJg,boltSet,base_pos,other_vec);
		start_pos = base_pos+base_vec*base_extend_len;
		end_pos	  = base_pos+other_vec*vice_extend_len;
		pShortSegJg1=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg1->iSeg=current_segI;
		pShortSegJg1->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg1->SetPartNo(pDesignInfo->sPartNo);
		start_pos+=pFirstJg->get_norm_y_wing()*wing_norm_offset;
		end_pos+=pFirstJg->get_norm_y_wing()*wing_norm_offset;
		pShortSegJg1->SetStart(start_pos);
		pShortSegJg1->SetEnd(end_pos);
		pShortSegJg1->set_wing_wide(wing_wide);
		pShortSegJg1->set_wing_thick(wing_thick);
		pShortSegJg1->desStartPos.jgber_cal_style=pShortSegJg1->desEndPos.jgber_cal_style=1;	//指定楞点定位
		pShortSegJg1->desStartPos.datumPoint.SetPosition(pShortSegJg1->Start());
		pShortSegJg1->desEndPos.datumPoint.SetPosition(pShortSegJg1->End());
		pShortSegJg1->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg1->desStartPos.wing_y_offset.SetOffset();
		pShortSegJg1->desEndPos.wing_x_offset.SetOffset();
		pShortSegJg1->desEndPos.wing_y_offset.SetOffset();
		if(arrLinkToStart1OrEnd0[0])	//始端点连接
			SetShortAngleWingNorm(pShortSegJg1,'Y',pFirstJg,'Y');//填写肢法线设计参数
		else
			SetShortAngleWingNorm(pShortSegJg1,'X',pFirstJg,'Y');//填写肢法线设计参数
		//添加短角钢上的螺栓
		UCS_STRU ucs;
		f3dPoint ls_pos;
		pFirstJg->getUCS(ucs);
		double length = DISTANCE(pFirstJg->Start(),pFirstJg->End());
		/*for(CLsRef *pLsRef=pFirstJg->GetFirstLsRef();pLsRef;pLsRef=pFirstJg->GetNextLsRef())
		{
			if(fabs((*pLsRef)->get_norm()*pFirstJg->get_norm_x_wing())
						>fabs((*pLsRef)->get_norm()*pFirstJg->get_norm_y_wing()))
						continue;	//X肢上的螺栓
			if((*pLsRef)->get_norm()*pFirstJg->get_norm_y_wing()<0)//螺栓从外向里穿
				(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
			ls_pos = (*pLsRef)->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			if(pFirstJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else
			{
				if(ls_pos.z>length-base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
		}*/
		pSecJg->getUCS(ucs);
		length = DISTANCE(pSecJg->Start(),pSecJg->End());
		//for(pLsRef=pSecJg->GetFirstLsRef();pLsRef;pLsRef=pSecJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			if(fabs(pBolt->get_norm()*pFirstJg->get_norm_x_wing())>fabs(pBolt->get_norm()*pFirstJg->get_norm_y_wing()))
				continue;	//X肢上的螺栓
			if(pBolt->get_norm()*pFirstJg->get_norm_y_wing()<0)//螺栓从外向里穿
				pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
			ls_pos = pBolt->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			/*if(pSecJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<vice_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else*/
			{
				//if(ls_pos.z>length-vice_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		//存在相关钢板,引入属于短角钢的螺栓 wht 11-01-15
		if(pRelaPlate)
		{
			for(CLsRef* pLsRef=pRelaPlate->GetFirstLsRef();pLsRef;pLsRef=pRelaPlate->GetNextLsRef())
			{
				if(pShortSegJg1->IsLsInWing(pLsRef->GetLsPtr()))
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
		}
		pShortSegJg1->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg1->GetSolidPartObject());
	}
	else
	{
		f3dPoint wing_norm[2];
		f3dPoint face_pick,face_norm,line_pick;
		wing_norm[0] = pFirstJg->get_norm_y_wing();
		if(arrLinkToStart1OrEnd0[0])
			line_pick=pFirstJg->Start();
		else
			line_pick=pFirstJg->End();
		char cSecondDatumWing='Y';
		if(wing_norm[0]*pSecJg->get_norm_y_wing()>wing_norm[0]*pSecJg->get_norm_x_wing())
			wing_norm[1]=pSecJg->get_norm_y_wing();
		else
		{
			wing_norm[1]=pSecJg->get_norm_x_wing();
			cSecondDatumWing='X';
		}
		if(arrLinkToStart1OrEnd0[1])
			face_pick=pSecJg->Start();
		else
			face_pick=pSecJg->End();
		face_pick+=wing_norm[1]*wing_norm_offset;
		line_pick+=wing_norm[0]*wing_norm_offset;
		f3dPoint inters=base_pos;
		bool datum_by_inters=true;	//中间为曲点是求交计算所得（需要指定坐标）
		short junction=1;	//默认基准角钢终端为火曲连接端
		double complement=0;
		if(base_pos==pFirstJg->Start())
			junction=0;	//基准角钢始端为火曲连接端
		GECS cs;
		cs.axis_y=wing_norm[0];
		cs.axis_z=base_vec;
		cs.axis_x=cs.axis_y^cs.axis_z;
		GEPOINT bend_vec=cs.TransVToCS(other_vec);
		int inters_style=0;
		//刘伟建议对于外贴材都应支持选择是否简化为直曲，此处去掉判断(bend_vec.x/bend_vec.y>5) wht 17-05-04
#ifdef AFX_TARG_ENU_ENGLISH
		if(fabs(bend_vec.y)>EPS&&fabs(bend_vec.x)>fabs(bend_vec.y)&&AfxMessageBox("bending gives priority to datum angle's X leg,can Y leg bending be ignored to simplify process to straight bending?",MB_YESNO)==IDYES)
#else
		if(fabs(bend_vec.y)>EPS&&fabs(bend_vec.x)>fabs(bend_vec.y)&&AfxMessageBox("制弯以角钢角钢X肢为主，是否可以忽略Y肢制弯角，以简化工艺为直曲?",MB_YESNO)==IDYES)
#endif
		{	//主要是X肢制弯忽略Y肢制弯角度
			bend_vec.y=0;
			other_vec=cs.TransVFromCS(bend_vec);
			normalize(other_vec);
			project_point(face_pick,line_pick,cs.axis_y);
			if(Int3dpl(f3dLine(line_pick,line_pick+base_vec),f3dLine(face_pick,face_pick+other_vec),inters)>0)
				inters_style=1;	//单肢火曲
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("failure in measuring short angle's bending point,default value adopted temporarily!");
		}
		else if(fabs(bend_vec.x)>EPS&&fabs(bend_vec.y)>fabs(bend_vec.x)&&AfxMessageBox("bending gives priority to datum angle's Y leg,can X leg bending be ignored to simplify process to straight bending?",MB_YESNO)==IDYES)
#else
				logerr.Log("短角钢制弯点计算失败，暂取默认值!");
		}
		else if(fabs(bend_vec.x)>EPS&&fabs(bend_vec.y)>fabs(bend_vec.x)&&AfxMessageBox("制弯以基准角钢Y肢为主，是否可以忽略X肢制弯角，以简化工艺为直曲?",MB_YESNO)==IDYES)
#endif
		{	//主要是Y肢制弯忽略Y肢制弯角度
			bend_vec.x=0;
			other_vec=cs.TransVFromCS(bend_vec);
			normalize(other_vec);
			project_point(face_pick,line_pick,cs.axis_x);
			if(Int3dpl(f3dLine(line_pick,line_pick+base_vec),f3dLine(face_pick,face_pick+other_vec),inters)>0)
				inters_style=1;	//单肢火曲
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("failure in measuring short angle's bending point,default value adopted temporarily!");
#else
				logerr.Log("短角钢制弯点计算失败，暂取默认值!");
#endif
		}
		else if(justify>=-0.9994)	//制弯角度较大>2°直接采取交点为制弯点
		{
			if(Int3dlf(inters,line_pick,base_vec,face_pick,wing_norm[1])>0)
				inters_style=2;	//双肢火曲
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("failure in measuring short angle's bending point,default value adopted temporarily!");
#else
				logerr.Log("短角钢制弯点计算失败，暂取默认值!");
#endif
		}
		if(inters_style>0)
			base_pos=inters;
		else	//因火曲角度太小，干扰误差太大等因素带来结果的错误，故选择原基准顶点法向偏移板厚 wjh-2013.10.12
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pFirstJg->handle);
			CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please input the extending compensation measure from datum angle connecting end to the angle to be connected<0>:","");
#else
			pCmdLine->FillCmdLine("请输入自基准(当前选中)角钢连接端点向待连接角钢的延伸补偿量<0>:","");
#endif
			CString cmdStr;
			if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				return false;
			if(cmdStr.GetLength()>0)
				complement=atof(cmdStr);
			base_pos+=wing_norm[0]*wing_norm_offset-complement*base_vec;
			datum_by_inters=false;	//中间火曲点为延基准角钢延伸方向偏移所得
		}

		double base_extend_len=CalShortAngleExtendLength(pFirstJg,boltSet,base_pos,base_vec);
		double vice_extend_len=CalShortAngleExtendLength(pSecJg,boltSet,base_pos,other_vec);
		start_pos = base_pos+base_vec*base_extend_len;
		end_pos	  = base_pos+other_vec*vice_extend_len;

		long huoqu_handle=Ta.GetNewHandle();
		pShortSegJg1=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg1->huoqu_handle=huoqu_handle;
		pShortSegJg1->SetStart(start_pos);
		pShortSegJg1->SetEnd(base_pos);
		pShortSegJg1->set_wing_wide(wing_wide);
		pShortSegJg1->set_wing_thick(wing_thick);
		pShortSegJg2=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg2->huoqu_handle=huoqu_handle;
		pShortSegJg2->SetStart(base_pos);
		pShortSegJg2->SetEnd(end_pos);
		pShortSegJg2->set_wing_wide(wing_wide);
		pShortSegJg2->set_wing_thick(wing_thick);
		pShortSegJg1->pEnd   = pSelNode;
		pShortSegJg2->pStart = pSelNode;
		pShortSegJg1->desStartPos.datumPoint.SetPosition(pShortSegJg1->Start());
		pShortSegJg1->desEndPos.datumPoint.SetPosition(pShortSegJg1->End());
		pShortSegJg2->desStartPos.datumPoint.SetPosition(pShortSegJg2->Start());
		pShortSegJg2->desEndPos.datumPoint.SetPosition(pShortSegJg2->End());
		pShortSegJg2->iSeg=current_segI;
		pShortSegJg1->iSeg=pShortSegJg2->iSeg;
		pShortSegJg1->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg2->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg1->SetPartNo(pDesignInfo->sPartNo);
		pShortSegJg2->SetPartNo(pDesignInfo->sPartNo);
		CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)console.AppendPart(CLS_POLYJOINT);
		pPolyJoint->cMaterial=pShortSegJg1->cMaterial;
		pPolyJoint->iSeg=pShortSegJg1->iSeg;
		pPolyJoint->SetPartNo(pShortSegJg1->GetPartNo());
		pPolyJoint->pPrevLinePart=pShortSegJg1;
		pPolyJoint->pNextLinePart=pShortSegJg2;
		//外短角钢，不用铲背 WJH-- 2004/09/21
		//pShortSegJg1->is_cut_back=pShortSegJg2->is_cut_back=2;

		if(arrLinkToStart1OrEnd0[0])	//始端点连接
		{
			SetShortAngleWingNorm(pShortSegJg1,'Y',pFirstJg,'Y');			//填写肢法线设计参数
			SetShortAngleWingNorm(pShortSegJg2,'Y',pSecJg,cSecondDatumWing);//填写肢法线设计参数
			pShortSegJg1->huoqu_base_wing_x0_y1_both2=1;
			pShortSegJg2->huoqu_base_wing_x0_y1_both2=1;
		}
		else
		{
			SetShortAngleWingNorm(pShortSegJg1,'X',pFirstJg,'Y');			//填写肢法线设计参数
			SetShortAngleWingNorm(pShortSegJg2,'X',pSecJg,cSecondDatumWing);//填写肢法线设计参数
			pShortSegJg1->huoqu_base_wing_x0_y1_both2=0;
			pShortSegJg2->huoqu_base_wing_x0_y1_both2=0;
		}

		pShortSegJg1->desStartPos.jgber_cal_style=1;
		pShortSegJg1->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg1->desStartPos.wing_y_offset.SetOffset();
		pShortSegJg1->LockStartPos();
		pShortSegJg1->LockEndPos();
		pShortSegJg1->bHuoQuEnd=TRUE;
		if(datum_by_inters)
			pShortSegJg1->desEndPos.jgber_cal_style=1;
		else
		{
			pShortSegJg1->desEndPos.jgber_cal_style=0;
			pShortSegJg1->desEndPos.spatialOperationStyle=0;	//指定基准点
			pShortSegJg1->desEndPos.datumPoint.datum_pos_style=1;	//角钢两端楞点为基准
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.hRod=pFirstJg->handle;
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.direction=junction;
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.offset_wing=1;	//Y肢
			sprintf(pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.norm_offset,"%s",(char*)CXhChar16(wing_norm_offset));
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.SetWingOffset();
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.len_offset_dist=-complement;
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.bIncOddEffect=false;
			pShortSegJg1->desEndPos.wing_x_offset.SetOffset();
			pShortSegJg1->desEndPos.wing_y_offset.SetOffset();
		}
		pShortSegJg2->LockStartPos();
		pShortSegJg2->LockEndPos();
		pShortSegJg2->bHuoQuStart=TRUE;
		//设定第二段子角钢的始端顶点搭在第一段子角钢末端顶点上
		pShortSegJg2->desStartPos.jgber_cal_style=0;
		pShortSegJg2->desStartPos.spatialOperationStyle=0;
		pShortSegJg2->desStartPos.datumPoint.datum_pos_style=1;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.hRod=pShortSegJg1->handle;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.direction=1;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.len_offset_dist=
			pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.wing_offset_style=4;
		strcpy(pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.norm_offset,"");
		pShortSegJg2->desStartPos.datumPoint.des_para.RODEND.bIncOddEffect=FALSE;
		pShortSegJg2->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg2->desStartPos.wing_y_offset.SetOffset();

		pShortSegJg2->desEndPos.jgber_cal_style=1;
		pShortSegJg2->desEndPos.wing_x_offset.SetOffset();
		pShortSegJg2->desEndPos.wing_y_offset.SetOffset();
		//添加短角钢上的螺栓
		UCS_STRU ucs;
		f3dPoint ls_pos;
		pFirstJg->getUCS(ucs);
		double length = DISTANCE(pFirstJg->Start(),pFirstJg->End());
		//for(CLsRef *pLsRef=pFirstJg->GetFirstLsRef();pLsRef;pLsRef=pFirstJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			ls_pos = pBolt->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			/*if(pFirstJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<base_extend_len&&fabs(wing_norm[0]*(*pLsRef)->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_y_wing()>0)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else*/
			if(pFirstJg->FindLsByHandle(pBolt->handle))
			{
				//if(ls_pos.z>length-base_extend_len&&fabs(wing_norm[0]*(*pLsRef)->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					if(pBolt->get_norm()*pShortSegJg1->get_norm_y_wing()>0)//螺栓相对短角钢从里向外穿
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		pSecJg->getUCS(ucs);
		length = DISTANCE(pSecJg->Start(),pSecJg->End());
		//for(pLsRef=pSecJg->GetFirstLsRef();pLsRef;pLsRef=pSecJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			ls_pos = pBolt->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			/*if(pSecJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<vice_extend_len&&fabs(wing_norm[1]*(*pLsRef)->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg2->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg2->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					if((*pLsRef)->get_norm()*pShortSegJg2->get_norm_y_wing()>0)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg2->handle,TRUE);
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else*/
			if(pSecJg->FindLsByHandle(pBolt->handle))
			{
				//if(ls_pos.z>length-vice_extend_len&&fabs(wing_norm[1]*pBolt->get_norm())>EPS_COS2)	//穿过短角钢
				{
					pShortSegJg2->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg2->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					if(pBolt->get_norm()*pShortSegJg2->get_norm_y_wing()>0)//螺栓相对短角钢从里向外穿
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg2->handle,TRUE);
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
			//存在相关钢板,引入属于短角钢的螺栓 wht 11-01-15
			if(pRelaPlate)
			{	
				for(CLsRef* pLsRef=pRelaPlate->GetFirstLsRef();pLsRef;pLsRef=pRelaPlate->GetNextLsRef())
				{
					if(pShortSegJg1->IsLsInWing(pLsRef->GetLsPtr()))
					{
						pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE); //过滤相同句柄 wht 11-05-03
						(*pLsRef)->CalGuigeAuto();
						(*pLsRef)->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
					}
					if(pShortSegJg2->IsLsInWing(pLsRef->GetLsPtr()))
					{
						pShortSegJg2->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pShortSegJg2->handle,TRUE,TRUE);	//过滤相同句柄 wht 11-05-03
						(*pLsRef)->CalGuigeAuto();
						(*pLsRef)->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
					}
				}
			}
		}
		pPolyJoint->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPolyJoint->GetSolidPartObject());
		pShortSegJg1->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg1->GetSolidPartObject());
		pShortSegJg2->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg2->GetSolidPartObject());
	}
	return true;
}
bool DesLocateBerPosShortJg(CLDSLineAngle* &pShortSegJg,OUTER_ATTACH_ANGLE_INFO* pDesignInfo);
//外贴短角钢位于基准角钢Y肢时进行设计
static bool DesOuterAttachShortJg(CLDSLineAngle* &pShortSegJg,OUTER_ATTACH_ANGLE_INFO* pDesignInfo)
{
	if(pDesignInfo->iWorkWingOfDepend==0)	//短角钢贴于基准角钢两肢
		return DesLocateBerPosShortJg(pShortSegJg,pDesignInfo);
	if( pDesignInfo==NULL||pDesignInfo->pBoltSet==NULL||
		pDesignInfo->pDatumJg==NULL||pDesignInfo->pDatumNode==NULL)
		return false;
	CLDSNode *pBaseNode=pDesignInfo->pDatumNode;
	CLDSLineAngle *pDatumJg=pDesignInfo->pDatumJg,*pDatumJg1=pDesignInfo->pDatumJg1;
	CLDSPlate *pRelaPlate=pDesignInfo->pRelaPlate;
	BOLTSET boltSet;
	if(pDesignInfo->pBoltSet)
	{
		for(CLDSBolt *pBolt=pDesignInfo->pBoltSet->GetFirst();pBolt;pBolt=pDesignInfo->pBoltSet->GetNext())
			boltSet.append(pBolt);
	}

	CLDSBolt* pBolt;
	f3dPoint base_vec = pDatumJg->End()-pDatumJg->Start();
	Standarized(base_vec);
	double wing_wide,wing_thick;
	restore_JG_guige(pDesignInfo->spec,wing_wide,wing_thick);
	double start_extend_len = 200,end_extend_len = 200;
	//由螺栓包络矩形预估外贴材规格
	f3dPoint datumAngleBerPos;
	//计算角钢基准角钢上下
	BOOL bDatumAngle=FALSE;     //基准角钢 在上面
	BOOL bDatumPos=FALSE;     //基准角钢 在左侧
	//
	if(pDatumJg1)
	{	//根据交叉角钢计算外贴材长度
		f3dPoint datumAngleWordVec,datumJG1WordVec;
		f3dPoint datumAngleVecX=pDatumJg->get_norm_x_wing();
		f3dPoint datumAngleVecY=pDatumJg->get_norm_y_wing();
		if(abs(datumAngleVecX*pDatumJg1->get_norm_x_wing())>EPS_COS2)
		{
			datumAngleWordVec = pDatumJg->GetWingVecY();
			datumJG1WordVec = pDatumJg1->GetWingVecY();
		}
		else if(abs(datumAngleVecX*pDatumJg1->get_norm_y_wing())>EPS_COS2)
		{
			datumAngleWordVec = pDatumJg->GetWingVecY();
			datumJG1WordVec = pDatumJg1->GetWingVecX();
		}
		else if(abs(datumAngleVecY*pDatumJg1->get_norm_x_wing())>EPS_COS2)
		{
			datumAngleWordVec = pDatumJg->GetWingVecY();
			datumJG1WordVec = pDatumJg1->GetWingVecY();
		}
		else if(abs(datumAngleVecY*pDatumJg1->get_norm_y_wing())>EPS_COS2)
		{
			datumAngleWordVec = pDatumJg->GetWingVecY();
			datumJG1WordVec = pDatumJg1->GetWingVecX();
		}
		//
		Int3dDifferPlaneLL( pDatumJg->Start(), pDatumJg->End(),pDatumJg1->Start(),pDatumJg1->End(),datumAngleBerPos);
		//计算角钢1楞线上的点
		double fDist=0;
		f3dPoint dtaumJG1Berpos =datumAngleBerPos;
		SnapPerp(&dtaumJG1Berpos,pDatumJg1->Start(),pDatumJg1->End(),dtaumJG1Berpos,&fDist);
		//最小长度是二个肢宽
		double fSumDist=pDatumJg->GetWidthWingY()+pDatumJg1->GetWidthWingY();
		f3dPoint vec= datumAngleBerPos-dtaumJG1Berpos;
		normalize(vec);
		if(fDist>=fSumDist)
		{
			start_extend_len=0;
			end_extend_len=fDist;
		}
		else if(fDist<pDatumJg1->GetWidthWingY()) //T字形
		{
			start_extend_len=pDatumJg->GetWidthWingY();
			end_extend_len=fDist;
		}
		else
		{
			f3dPoint dtaumJGWingPos=dtaumJG1Berpos+datumJG1WordVec*pDatumJg1->GetWidthWingY();
			//
			if(DISTANCE(dtaumJG1Berpos,datumAngleBerPos)>=DISTANCE(dtaumJGWingPos,datumAngleBerPos))
				end_extend_len = DISTANCE(dtaumJG1Berpos,datumAngleBerPos);
			else
				end_extend_len = DISTANCE(dtaumJGWingPos,datumAngleBerPos);
			//
			if(end_extend_len>=fSumDist)
				start_extend_len=0;
			else
				start_extend_len=pDatumJg->GetWidthWingY();
		}
		//计算基准角钢1钢位于基准点左右
		f3dPoint rodVec=pDatumJg->End()-pDatumJg->Start();
		vec=pBaseNode->Position()-datumAngleBerPos;
		normalize(vec);
		normalize(rodVec);
		if(vec*rodVec>EPS_COS2)
			bDatumPos=FALSE;
		else
			bDatumPos=TRUE;
	}
	else if (boltSet.GetFirst())
	{
		UCS_STRU tmp_ucs;
		if (pDesignInfo->nRotateAngle <= 0)
		{
			tmp_ucs.axis_x=pDatumJg->ucs.axis_z;
			if (pDesignInfo->iWorkWingOfDepend == 1)
				tmp_ucs.axis_y=pDatumJg->GetWingVecX();
			else //if(pDesignInfo->iWorkWingOfDepend == 2)
				tmp_ucs.axis_y=pDatumJg->GetWingVecY();
		}
		else
		{
			tmp_ucs.axis_y=pDatumJg->ucs.axis_z;
			if (pDesignInfo->iWorkWingOfDepend == 1)
				tmp_ucs.axis_x=pDatumJg->GetWingVecX();
			else //if(pDesignInfo->iWorkWingOfDepend == 2)
				tmp_ucs.axis_x=pDatumJg->GetWingVecY();
		}
		tmp_ucs.axis_z=tmp_ucs.axis_x^tmp_ucs.axis_y;
		tmp_ucs.axis_y=tmp_ucs.axis_z^tmp_ucs.axis_x;
		//tmp_ucs.origin=boltSet.GetFirst()->ucs.origin;
		tmp_ucs.origin = pDatumJg->GetDatumPosBer(pBaseNode);
		SCOPE_STRU scope;
		int end_space;	//螺栓端距
		for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			f3dPoint vertice,ls_pos;
			pDatumJg->GetBoltIntersPos(pBolt,ls_pos);
			coord_trans(ls_pos,tmp_ucs,FALSE);
			if(pBolt->get_d()==12)
				end_space=20;
			else if(pBolt->get_d()==16)
				end_space=25;
			else if(pBolt->get_d()==20)
				end_space=30;
			else if(pBolt->get_d()==24)
				end_space=40;
			else
				end_space=30;
			vertice.Set(ls_pos.x-end_space,ls_pos.y+end_space);
			scope.VerifyVertex(vertice);	//左上角
			vertice.Set(ls_pos.x+end_space,ls_pos.y+end_space);
			scope.VerifyVertex(vertice);	//右上角
			vertice.Set(ls_pos.x+end_space,ls_pos.y-end_space);
			scope.VerifyVertex(vertice);	//右下角
			vertice.Set(ls_pos.x-end_space,ls_pos.y-end_space);
			scope.VerifyVertex(vertice);	//左下角
		}
		double shortjg_len=max(wing_wide,scope.fMaxX-scope.fMinX);
		//同一排心有可能因计算误差带来角钢肢宽不够情况（如50以下角钢），故不在自动修订角钢肢宽　wjh-2016.4.9
		/*if (wing_wide < scope.fMaxY-scope.fMinY)
		{
			for(int i=0;i<jgguige_N;i++)
			{
				if(jgguige_table[i].wing_wide>scope.fMaxY-scope.fMinY)
				{
					wing_wide = jgguige_table[i].wing_wide;
					wing_thick = jgguige_table[i].wing_thick;
					break;
				}
			}
		}*/
		double min_extend_len=-CalShortAngleExtendLength(pDatumJg,boltSet,tmp_ucs.origin,-base_vec);
		double max_extend_len=CalShortAngleExtendLength(pDatumJg,boltSet,tmp_ucs.origin,base_vec);
		double center_extend_len = (min_extend_len+max_extend_len)/2;
		start_extend_len=center_extend_len-shortjg_len/2;
		end_extend_len=center_extend_len+shortjg_len/2;
	}
	if(pShortSegJg==NULL)
		pShortSegJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
	pShortSegJg->iSeg=pDesignInfo->iSeg;
	if (pShortSegJg->iSeg.ToString().Equal(""))
		pShortSegJg->iSeg = pDatumJg->iSeg;
	pShortSegJg->cMaterial=pDesignInfo->cMaterial;
	pShortSegJg->SetPartNo(pDesignInfo->sPartNo);
	pShortSegJg->set_wing_wide(wing_wide);
	pShortSegJg->set_wing_thick(wing_thick);
	pShortSegJg->desStartPos.jgber_cal_style=pShortSegJg->desEndPos.jgber_cal_style=0;	//参数化定位

	pShortSegJg->desStartPos.spatialOperationStyle = 0;//指定基准点
	pShortSegJg->desStartPos.datumPoint.datum_pos_style = 2;//杆件上节点为基准
	pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.hRod  = pDatumJg->handle;
	pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.hNode = pBaseNode->handle;
	pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.direction = 0;//始端向终端偏移
	pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.len_offset_dist = start_extend_len;
	pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.offset_wing = pDesignInfo->iWorkWingOfDepend-1;
	pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.wing_offset_style = 4;

	if (pDesignInfo->iWorkWingOfDepend == pDesignInfo->iWorkWingOfSelf)//顺贴
		pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.wing_offset_dist = wing_wide;
	else//倒贴
		pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.wing_offset_dist = 0;
	if (pDatumJg1&&pDatumJg)
	{
		if(bDatumAngle)
		{
			pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.len_offset_dist=start_extend_len;
			pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.wing_offset_dist=DISTANCE(datumAngleBerPos,pBaseNode->Position());	
		}
		else
		{
			pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.len_offset_dist=0/*DISTANCE(datumAngleBerPos,pBaseNode->Position())*/;
			if(bDatumPos)
				pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.wing_offset_dist=end_extend_len;
			else
				pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.wing_offset_dist=start_extend_len;
		}
	}
	else if (pDesignInfo->nRotateAngle > 0)//垂向外贴
	{
		pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.len_offset_dist=0;
		pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.wing_offset_dist =0;//-= (wing_wide-pDatumJg->GetWidth())/2;
	}
	//double wing_norm_offset=CalShortAngleWingNormOffset(pDatumJg,pRelaPlate);
	if (pRelaPlate)
		_snprintf(pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.norm_offset,49,"0x%X",pRelaPlate->handle);
	else
		pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.norm_offset[0]=0;
	pShortSegJg->desStartPos.datumPoint.UpdatePos(pDatumJg->BelongModel());

	pShortSegJg->desEndPos.spatialOperationStyle = 0;//指定基准点
	pShortSegJg->desEndPos.datumPoint.datum_pos_style = 2;//杆件上节点为基准
	pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.hRod  = pDatumJg->handle;
	pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.hNode = pBaseNode->handle;
	pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.direction = 0;//始端向终端偏移
	pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.len_offset_dist = end_extend_len;
	pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.offset_wing = pDesignInfo->iWorkWingOfDepend-1;
	pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.wing_offset_style = 4;
	if (pDesignInfo->iWorkWingOfDepend == pDesignInfo->iWorkWingOfSelf)//顺贴
		pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.wing_offset_dist = wing_wide;
	else//倒贴
		pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.wing_offset_dist = 0;
	if (pDatumJg1&&pDatumJg)
	{	
		if(bDatumAngle)
		{
			pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.wing_offset_dist=DISTANCE(datumAngleBerPos,pBaseNode->Position());
			pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.len_offset_dist=end_extend_len;
		}
		else
		{
			if(bDatumPos)
				pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.wing_offset_dist=start_extend_len;
			else
				pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.wing_offset_dist=end_extend_len;
			pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.len_offset_dist=0/*DISTANCE(datumAngleBerPos,pBaseNode->Position())*/;
		}
	}
	else if (pDesignInfo->nRotateAngle > 0)//垂向外贴
	{
		pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.len_offset_dist = 0;
		pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.wing_offset_dist=wing_wide;// -= (wing_wide-pDatumJg->GetWidth())/2;
	}
	memcpy(pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.norm_offset,
		pShortSegJg->desStartPos.datumPoint.des_para.RODNODE.norm_offset,51);
	//_snprintf(pShortSegJg->desEndPos.datumPoint.des_para.RODNODE.norm_offset,49,"%f",wing_norm_offset);
	pShortSegJg->desEndPos.datumPoint.UpdatePos(pDatumJg->BelongModel());
	//初始化外贴材的始终端的基点->楞点定位方式
	pShortSegJg->desStartPos.wing_x_offset.SetOffset();
	pShortSegJg->desStartPos.wing_y_offset.SetOffset();
	pShortSegJg->desEndPos.wing_x_offset.SetOffset();
	pShortSegJg->desEndPos.wing_y_offset.SetOffset();
	if (pDesignInfo->nRotateAngle>0)
	{//旋转
		//pShortSegJg->desStartPos.jgber_cal_style=pShortSegJg->desEndPos.jgber_cal_style=1;	//指定楞点定位
		pShortSegJg->ClearFlag();
		pShortSegJg->CalPosition();
		//肢法线设计
		if(pDesignInfo->iWorkWingOfSelf==1)//X肢
		{
			pShortSegJg->des_norm_x.bSpecific = pShortSegJg->des_norm_y.bSpecific = TRUE;
			pShortSegJg->des_norm_y.bSpecific=FALSE;
			pShortSegJg->des_norm_y.bByOtherWing=TRUE;
		}
		else //if(pDesignInfo->iWorkWingOfSelf==2)//Y肢
		{
			pShortSegJg->des_norm_x.bSpecific = pShortSegJg->des_norm_y.bSpecific = TRUE;
			pShortSegJg->des_norm_x.bSpecific=FALSE;
			pShortSegJg->des_norm_x.bByOtherWing=TRUE;
		}
	}

	pShortSegJg->SetStart(pShortSegJg->desStartPos.datumPoint.Position());
	pShortSegJg->SetEnd(pShortSegJg->desEndPos.datumPoint.Position());

	if (pDesignInfo->iWorkWingOfDepend == 1)
	{//短角钢位于基准角钢X肢
		if(pDesignInfo->iWorkWingOfSelf == 1)
			SetShortAngleWingNorm(pShortSegJg,'X',pDatumJg,'X');//填写肢法线设计参数
		else //if(pDesignInfo->iWorkWingOfSelf == 2)
			SetShortAngleWingNorm(pShortSegJg,'Y',pDatumJg,'X');//填写肢法线设计参数
	}
	else //if(pDesignInfo->iWorkWingOfDepend == 2)
	{//短角钢位于基准角钢Y肢
		if(pDesignInfo->iWorkWingOfSelf == 1)
			SetShortAngleWingNorm(pShortSegJg,'X',pDatumJg,'Y');//填写肢法线设计参数
		else //if(pDesignInfo->iWorkWingOfSelf == 2)
			SetShortAngleWingNorm(pShortSegJg,'Y',pDatumJg,'Y');//填写肢法线设计参数
	}
	pShortSegJg->cal_x_wing_norm();
	pShortSegJg->cal_y_wing_norm();
	//添加短角钢上的螺栓
	UCS_STRU ucs;
	f3dPoint ls_pos;
	pDatumJg->getUCS(ucs);
	double length = DISTANCE(pDatumJg->Start(),pDatumJg->End());
	for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		if (pDesignInfo->iWorkWingOfDepend ==1)
		{
			if(fabs(pBolt->get_norm()*pDatumJg->get_norm_x_wing())
				<fabs(pBolt->get_norm()*pDatumJg->get_norm_y_wing()))
				continue;	//Y肢上的螺栓
			if(pBolt->get_norm()*pDatumJg->get_norm_x_wing()<0)//螺栓从外向里穿
				pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg->handle,TRUE);
		}
		else //if(pDesignInfo->iWorkWingOfDepend == 2)
		{
			if(fabs(pBolt->get_norm()*pDatumJg->get_norm_x_wing())
				>fabs(pBolt->get_norm()*pDatumJg->get_norm_y_wing()))
				continue;	//X肢上的螺栓
			if(pBolt->get_norm()*pDatumJg->get_norm_y_wing()<0)//螺栓从外向里穿
				pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg->handle,TRUE);
		}
		ls_pos = pBolt->ucs.origin;
		coord_trans(ls_pos,ucs,FALSE);
		//if(ls_pos.z>length-vice_extend_len)	//穿过短角钢
		pShortSegJg->AppendMidLsRef(pBolt->GetLsRef());
		pBolt->AddL0Thick(pShortSegJg->handle,TRUE,TRUE);
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	if (pDesignInfo->nRotateAngle > 0)//垂向外贴
	{	//自动根据引用螺栓计算垂向外贴材的横向偏移量　wjh-2016.5.31
		GEPOINT inters,startPoint=pShortSegJg->Start();
		GEPOINT vWingVecX=pShortSegJg->GetWingVecX();
		GEPOINT vWingVecY=pShortSegJg->GetWingVecY();
		int wingx0_y1=0,nMaxD=0;
		CMinDouble minCoordY;
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			if(!pShortSegJg->GetBoltIntersPos(pBolt,inters,&wingx0_y1))
				continue;
			GEPOINT bolt_vec=inters-startPoint;
			if(wingx0_y1==0)
				minCoordY.Update(bolt_vec*vWingVecX);
			else
				minCoordY.Update(bolt_vec*vWingVecY);
			nMaxD=max(nMaxD,pBolt->get_d());
		}
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,nMaxD);
		if(wingx0_y1==0)
		{
			pShortSegJg->desStartPos.wing_y_offset.offsetDist=pShortSegJg->desEndPos.wing_y_offset.offsetDist=-minCoordY.number+LsSpace.EndSpace+10;
			pShortSegJg->SetStart(pShortSegJg->Start()-f3dPoint(vWingVecX)*pShortSegJg->desStartPos.wing_y_offset.offsetDist);
			pShortSegJg->SetEnd(pShortSegJg->End()-f3dPoint(vWingVecX)*pShortSegJg->desEndPos.wing_y_offset.offsetDist);
		}
		else
		{
			pShortSegJg->desStartPos.wing_x_offset.offsetDist=pShortSegJg->desEndPos.wing_x_offset.offsetDist=-minCoordY.number+LsSpace.EndSpace+10;
			pShortSegJg->SetStart(pShortSegJg->Start()-f3dPoint(vWingVecY)*pShortSegJg->desStartPos.wing_x_offset.offsetDist);
			pShortSegJg->SetEnd(pShortSegJg->End()-f3dPoint(vWingVecY)*pShortSegJg->desEndPos.wing_x_offset.offsetDist);
		}
	}
	//存在相关钢板,引入属于短角钢的螺栓 wht 11-01-15
	if(pRelaPlate)
	{
		for(CLsRef* pLsRef=pRelaPlate->GetFirstLsRef();pLsRef;pLsRef=pRelaPlate->GetNextLsRef())
		{
			if(pShortSegJg->IsLsInWing(pLsRef->GetLsPtr()))
			{
				pShortSegJg->AppendMidLsRef((*pLsRef)->GetLsRef());
				(*pLsRef)->AddL0Thick(pShortSegJg->handle,TRUE,TRUE);
				(*pLsRef)->CalGuigeAuto();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
		}
	}
	pShortSegJg->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pShortSegJg->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return true;
}
//外贴短角钢位于基准角钢外楞时进行设计
static bool DesLocateBerPosShortJg(CLDSLineAngle* &pShortSegJg,OUTER_ATTACH_ANGLE_INFO* pDesignInfo)
{
	if( pDesignInfo==NULL||pDesignInfo->pBoltSet==NULL||
		pDesignInfo->pDatumJg==NULL||pDesignInfo->pDatumNode==NULL)
		return false;
	CLDSLineAngle *pDatumJg=pDesignInfo->pDatumJg;
	CLDSNode *pSelNode=pDesignInfo->pDatumNode;
	BOLTSET boltSet;
	if(pDesignInfo->pBoltSet)
	{
		for(CLDSBolt *pBolt=pDesignInfo->pBoltSet->GetFirst();pBolt;pBolt=pDesignInfo->pBoltSet->GetNext())
			boltSet.append(pBolt);
	}
	//根据螺栓计算内包短角钢的规格
	BOOL bDesStart=TRUE;
	f3dPoint base_pos,base_vec;
	if(pDatumJg->pEnd==pSelNode)
	{
		bDesStart=FALSE;
		base_vec=pDatumJg->Start()-pDatumJg->End();
		base_pos=pDatumJg->End();
	}
	else
	{
		bDesStart=TRUE;
		base_vec=pDatumJg->End()-pDatumJg->Start();
		base_pos=pDatumJg->Start();
	}
	Standarized(base_vec);
	double wing_wide,wing_thick;
	restore_JG_guige(pDesignInfo->spec,wing_wide,wing_thick);
	double start_extend_len=0,end_extend_len=100;
	start_extend_len=-CalShortAngleExtendLength(pDatumJg,boltSet,base_pos,-base_vec);
	end_extend_len=CalShortAngleExtendLength(pDatumJg,boltSet,base_pos,base_vec);
	char start_offset_str[20],end_offset_str[20];
	if(pDesignInfo->bLocateOutBer)
	{	//外包
		_snprintf(start_offset_str,19,"%.0f",wing_thick);
		_snprintf(end_offset_str,19,"%.0f",wing_thick);
	}
	else
	{	//内包
		_snprintf(start_offset_str,19,"-0X%X,-1",pDatumJg->handle);
		_snprintf(end_offset_str,19,"-0X%X,-1",pDatumJg->handle);
	}
	//
	if(pShortSegJg==NULL)
		pShortSegJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
	pShortSegJg->iSeg=pDatumJg->iSeg;
	pShortSegJg->cMaterial=pDesignInfo->cMaterial;
	pShortSegJg->SetPartNo(pDesignInfo->sPartNo);
	pShortSegJg->set_wing_wide(wing_wide);
	pShortSegJg->set_wing_thick(wing_thick);
	if(pDesignInfo->bLocateOutBer)
	{	//外包短角钢进行清根
		pShortSegJg->m_bCutRoot=TRUE;
	}
	else
	{	//内包短角钢进行铲背
		pShortSegJg->m_bCutBer=TRUE;
		pShortSegJg->m_nCutBerR=(WORD)FindJg_r(pDatumJg->GetWidth())+2;
	}
	//始端定位信息
	pShortSegJg->desStartPos.SetToDatumPointStyle();
	pShortSegJg->desEndPos.SetToDatumPointStyle();
	pShortSegJg->desStartPos.datumPoint.datum_pos_style=7;
	pShortSegJg->desEndPos.datumPoint.datum_pos_style=7;
	pShortSegJg->desStartPos.datumPoint.des_para.RODEND.hRod=pDatumJg->handle;
	pShortSegJg->desEndPos.datumPoint.des_para.RODEND.hRod=pDatumJg->handle;
	if(bDesStart)
	{
		pShortSegJg->desStartPos.datumPoint.des_para.RODEND.direction=0;
		start_extend_len+=pDatumJg->startOdd();	
		pShortSegJg->desEndPos.datumPoint.des_para.RODEND.direction=0;
		end_extend_len+=pDatumJg->startOdd();
	}
	else
	{
		pShortSegJg->desStartPos.datumPoint.des_para.RODEND.direction=1;
		start_extend_len+=pDatumJg->endOdd();
		pShortSegJg->desEndPos.datumPoint.des_para.RODEND.direction=1;
		end_extend_len+=pDatumJg->endOdd();
	}
	pShortSegJg->desStartPos.datumPoint.des_para.RODEND.len_offset_dist=start_extend_len;
	pShortSegJg->desEndPos.datumPoint.des_para.RODEND.len_offset_dist=end_extend_len;
	strcpy(pShortSegJg->desStartPos.datumPoint.des_para.RODEND.norm_offset,start_offset_str);
	strcpy(pShortSegJg->desEndPos.datumPoint.des_para.RODEND.norm_offset,end_offset_str);
	pShortSegJg->desStartPos.wing_x_offset.SetOffset();
	pShortSegJg->desStartPos.wing_y_offset.SetOffset();
	pShortSegJg->desEndPos.wing_x_offset.SetOffset();
	pShortSegJg->desEndPos.wing_y_offset.SetOffset();
	pShortSegJg->ClearFlag();
	pShortSegJg->CalPosition();
	//法线信息
	pShortSegJg->des_norm_x.bSpecific=TRUE;
	pShortSegJg->des_norm_x.hViceJg=pDatumJg->handle;
	pShortSegJg->des_norm_x.spec_norm.hVicePart=pDatumJg->handle;
	pShortSegJg->des_norm_x.spec_norm.norm_style=1;
	pShortSegJg->des_norm_x.spec_norm.direction=0;
	if(bDesStart)
		pShortSegJg->des_norm_x.spec_norm.norm_wing=0;	//X肢
	else
		pShortSegJg->des_norm_x.spec_norm.norm_wing=1;
	//
	pShortSegJg->des_norm_y.bSpecific=TRUE;
	pShortSegJg->des_norm_y.hViceJg=pDatumJg->handle;
	pShortSegJg->des_norm_y.spec_norm.hVicePart=pDatumJg->handle;
	pShortSegJg->des_norm_y.spec_norm.norm_style=1;
	pShortSegJg->des_norm_y.spec_norm.direction=0;
	if(bDesStart)
		pShortSegJg->des_norm_y.spec_norm.norm_wing=1;	//Y肢
	else
		pShortSegJg->des_norm_y.spec_norm.norm_wing=0;
	pShortSegJg->cal_x_wing_norm();
	pShortSegJg->cal_y_wing_norm();
	//添加短角钢上的螺栓
	for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		pShortSegJg->AppendMidLsRef(pBolt->GetLsRef());
		pBolt->AddL0Thick(pShortSegJg->handle,TRUE,TRUE);
		//计算螺栓法向偏移值
		PARTSET part_set;
		for(THICK_ITEM item=pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pBolt->GetNextL0ThickItem())
		{
			if(!item.IsHandle())
				continue;
			CLDSPart* pPart=console.FromPartHandle(item.val);
			part_set.append(pPart);
		}
		pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pBolt,part_set);
		//
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	pShortSegJg->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pShortSegJg->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return true;
}
//短角钢位于基准角钢内楞（内包）时进行设计
static bool DesInternalBerPosShortJg(CLDSNode *pSelNode,CLDSLineAngle *pFirstJg,CLDSLineAngle *pSecJg,
									 f3dPoint base_pos,f3dPoint base_vec,f3dPoint other_vec,SHORT_ANGLE_INFO* pDesignInfo,
									 BOLTSET& boltSet)
{
	//计算内楞位置(短角钢端点实际坐标)
	CSmartPtr<CLDSLineAngle>pShortSegJg1,pShortSegJg2;
	CSmartPtr<CLDSPlate>pXPadPlate,pYPadPlate;
	f3dPoint start_pos,end_pos;
	double wing_wide,wing_thick;
	restore_JG_guige(pDesignInfo->sizeStr,wing_wide,wing_thick);
	double linear_justify=base_vec*other_vec;

	if(fabs(linear_justify)<EPS_COS)	//两衔接角钢不共线时需要计算内角钢火曲制弯顶(楞)点
		base_pos = CalMidIntersPos(pSelNode,pFirstJg,pSecJg,pDesignInfo->manu_space);
	double base_extend_len=CalShortAngleExtendLength(pFirstJg,boltSet,base_pos,base_vec);
	double vice_extend_len=CalShortAngleExtendLength(pSecJg,boltSet,base_pos,other_vec);

	start_pos = base_pos+base_vec*base_extend_len;
	end_pos	  = base_pos+other_vec*vice_extend_len;
	f3dPoint bi_vec = pSecJg->get_norm_x_wing()+pSecJg->get_norm_y_wing();
	normalize(bi_vec);
	double bi_angle = cal_angle_of_2vec(bi_vec,pSecJg->get_norm_x_wing());
	char start_offset_str[20],end_offset_str[20];
	_snprintf(start_offset_str,19,"-0X%X,-%.0f",pFirstJg->handle,pDesignInfo->manu_space);
	_snprintf(end_offset_str,19,"-0X%X,-%.0f",pFirstJg->handle,pDesignInfo->manu_space);
	double offset = pFirstJg->GetThick()/cos(bi_angle);
	/*start_pos = start_pos-bi_vec*offset;
	bi_vec = pFirstJg->get_norm_x_wing()+pFirstJg->get_norm_y_wing();
	normalize(bi_vec);
	bi_angle = cal_angle_of_2vec(bi_vec,pFirstJg->get_norm_x_wing());
	offset = pFirstJg->GetThick()/cos(bi_angle);
	end_pos = end_pos-bi_vec*offset;*/
	SEGI current_segI=pFirstJg->iSeg;
	if(pSelNode!=NULL)
		current_segI=pSelNode->iSeg;

	BOOL arrLinkToStart1OrEnd0[2]={FALSE,FALSE};
	CLDSLinePartPtr rodArr[2]={pFirstJg,pSecJg};
	for(int i=0;i<2;i++)
	{
		if(rodArr[i]->pStart==pSelNode)
			arrLinkToStart1OrEnd0[i]=TRUE;
		else if(rodArr[i]->pEnd==pSelNode)
			arrLinkToStart1OrEnd0[i]=FALSE;
		else if(DISTANCE(pSelNode->Position(),rodArr[i]->Start())>DISTANCE(pSelNode->Position(),rodArr[i]->End()))
			arrLinkToStart1OrEnd0[i]=FALSE;
		else
			arrLinkToStart1OrEnd0[i]=TRUE;
	}

	if(fabs(linear_justify)>EPS_COS)	//两衔接角钢大致共线,短角钢不需火曲
	{
		pShortSegJg1=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg1->iSeg=current_segI;
		pShortSegJg1->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg1->SetPartNo(pDesignInfo->sPartNo);
		pShortSegJg1->SetStart(start_pos);
		pShortSegJg1->SetEnd(end_pos);
		pShortSegJg1->set_wing_wide(wing_wide);
		pShortSegJg1->set_wing_thick(wing_thick);
		pShortSegJg1->m_bCutBer=TRUE;
		pShortSegJg1->m_nCutBerR=(WORD)FindJg_r(pFirstJg->GetWidth())+2;
		pShortSegJg1->desStartPos.SetToDatumPointStyle();
		pShortSegJg1->desEndPos.SetToDatumPointStyle();
		pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.hRod = pFirstJg->handle;
		pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.hRod = pSecJg->handle;
		if(arrLinkToStart1OrEnd0[0])
		{
			base_extend_len+=pFirstJg->startOdd();
			pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.direction = 0;
		}
		else
		{
			base_extend_len+=pFirstJg->endOdd();
			pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.direction = 1;
		}
		if(arrLinkToStart1OrEnd0[1])
		{
			vice_extend_len+=pSecJg->startOdd();
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.direction = 0;
		}
		else
		{
			vice_extend_len+=pSecJg->endOdd();
			pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.direction = 1;
		}
		pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.len_offset_dist  = base_extend_len;
		pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.len_offset_dist = vice_extend_len;
		pShortSegJg1->desStartPos.datumPoint.datum_pos_style = 7;
		pShortSegJg1->desEndPos.datumPoint.datum_pos_style = 7;
		pShortSegJg1->desStartPos.datumPoint.SetPosition(pShortSegJg1->Start());
		pShortSegJg1->desEndPos.datumPoint.SetPosition(pShortSegJg1->End());
		strcpy(pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.norm_offset,start_offset_str);
		strcpy(pShortSegJg1->desEndPos.datumPoint.des_para.RODEND.norm_offset,end_offset_str);
		pShortSegJg1->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg1->desStartPos.wing_y_offset.SetOffset();
		pShortSegJg1->desEndPos.wing_x_offset.SetOffset();
		pShortSegJg1->desEndPos.wing_y_offset.SetOffset();
		if(arrLinkToStart1OrEnd0[0])	//始端点连接
		{
			pShortSegJg1->set_norm_x_wing(pFirstJg->get_norm_y_wing());
			pShortSegJg1->set_norm_y_wing(pFirstJg->get_norm_x_wing());
		}
		else
		{
			pShortSegJg1->set_norm_x_wing(pFirstJg->get_norm_x_wing());
			pShortSegJg1->set_norm_y_wing(pFirstJg->get_norm_y_wing());
		}
		pShortSegJg1->des_norm_x.spec_norm.vector=pShortSegJg1->get_norm_x_wing();
		pShortSegJg1->des_norm_y.spec_norm.vector=pShortSegJg1->get_norm_y_wing();
		pShortSegJg1->ClearFlag();
		pShortSegJg1->CalPosition();
		//添加短角钢上的螺栓
		UCS_STRU ucs;
		f3dPoint ls_pos;
		pFirstJg->getUCS(ucs);
		double length = DISTANCE(pFirstJg->Start(),pFirstJg->End());
		/*for(CLsRef *pLsRef=pFirstJg->GetFirstLsRef();pLsRef;pLsRef=pFirstJg->GetNextLsRef())
		{
			ls_pos = (*pLsRef)->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			if(pFirstJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					f3dPoint pos = (*pLsRef)->ucs.origin;
					(*pLsRef)->ucs.origin=pos-(*pLsRef)->get_norm()*pShortSegJg1->GetThick();
					if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_x_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					else if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_y_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					(*pLsRef)->correct_pos();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else
			{
				if(ls_pos.z>length-base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					f3dPoint pos = (*pLsRef)->ucs.origin;
					(*pLsRef)->ucs.origin=pos-(*pLsRef)->get_norm()*pShortSegJg1->GetThick();
					if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_x_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					else if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_y_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					(*pLsRef)->correct_pos();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
		}*/
		pSecJg->getUCS(ucs);
		length = DISTANCE(pSecJg->Start(),pSecJg->End());
		double add_thick;	//穿过短角钢的螺栓增加的通过厚度
		add_thick = pShortSegJg1->GetThick()+pFirstJg->GetThick()-pSecJg->GetThick();
		if(fabs(pFirstJg->GetThick()-pSecJg->GetThick())>=2)//&&pAttach->joint_handle<0x20)
		{	//需要加垫板
			//备份原有角钢选择顺序
			CLDSLineAngle* pBackupFirstAngle=pFirstJg;
			CLDSLineAngle* pBackupSecAngle=pSecJg;
			if(pFirstJg->GetThick()<pSecJg->GetThick())
			{
				pSecJg=pBackupFirstAngle;
				pFirstJg=pBackupSecAngle;
			}
			CJoint joint;
			f3dPoint pos,line_vec;
			int thick = (int)(pFirstJg->GetThick()-pSecJg->GetThick());
			double jg_r=FindJg_r(pSecJg->GetWidth());//角钢内圆半径
			//pJoint=(CJoint*)console.AppendPart(CLS_JOINT);
			joint.base_node_handle = pSelNode->handle;
			pXPadPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pYPadPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pYPadPlate->iSeg=current_segI;
			pXPadPlate->iSeg=pYPadPlate->iSeg;
			pXPadPlate->SetPartNo(pDesignInfo->xPadPartNo);
			pYPadPlate->SetPartNo(pDesignInfo->yPadPartNo);
			pXPadPlate->jdb_style=5;
			pYPadPlate->jdb_style=5;
			pXPadPlate->designInfo.m_hBaseNode = pSelNode->handle;
			pYPadPlate->designInfo.m_hBaseNode = pSelNode->handle;
			joint.AppendWrapPlank(pXPadPlate->handle,'G');
			joint.AppendWrapPlank(pYPadPlate->handle,'H');
			pXPadPlate->Thick=thick;
			pYPadPlate->Thick=thick;
			pXPadPlate->cfgword=pSelNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
			pYPadPlate->cfgword=pSelNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
			pXPadPlate->ucs.axis_z = -pSecJg->get_norm_x_wing();
			pXPadPlate->ucs.axis_x = pSecJg->GetWingVecX();
			pXPadPlate->ucs.axis_y = pXPadPlate->ucs.axis_z^pXPadPlate->ucs.axis_x;
			pXPadPlate->ucs.origin = base_pos+thick*pSecJg->get_norm_x_wing();
			pYPadPlate->ucs.axis_z = -pSecJg->get_norm_y_wing();
			pYPadPlate->ucs.axis_x = pSecJg->GetWingVecY();
			pYPadPlate->ucs.axis_y = pYPadPlate->ucs.axis_z^pYPadPlate->ucs.axis_x;
			pYPadPlate->ucs.origin = base_pos+thick*pSecJg->get_norm_y_wing();
			if(arrLinkToStart1OrEnd0[1])
			{
				line_vec=pSecJg->End()-pSecJg->Start();
				normalize(line_vec);
				//定义X肢垫板顶点
				pos = pXPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pXPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos-line_vec*vice_extend_len;
				pXPadPlate->vertex_list.append(pos);
				//定义Y肢垫板顶点
				pos = pYPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pYPadPlate->vertex_list.append(pos);
				pos = pos-(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
			}
			else
			{
				line_vec=pSecJg->Start()-pSecJg->End();
				normalize(line_vec);
				//定义X肢垫板顶点
				pos = pXPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pXPadPlate->vertex_list.append(pos);
				pos = pos-(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				//定义Y肢垫板顶点
				pos = pYPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pYPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos-line_vec*vice_extend_len;
				pYPadPlate->vertex_list.append(pos);
			}
			for(int i=0;i<4;i++)
			{
				coord_trans(pXPadPlate->vertex_list[i].vertex,pXPadPlate->ucs,FALSE);
				coord_trans(pYPadPlate->vertex_list[i].vertex,pYPadPlate->ucs,FALSE);
				pXPadPlate->vertex_list[i].vertex.feature = 1;
				pYPadPlate->vertex_list[i].vertex.feature = 1;
			}
			//复原原有的角钢选择顺序 wjh-2017.1.10
			pSecJg=pBackupSecAngle;
			pFirstJg=pBackupFirstAngle;
		}
		//for(pLsRef=pSecJg->GetFirstLsRef();pLsRef;pLsRef=pSecJg->GetNextLsRef())
		for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			ls_pos = pBolt->ucs.origin;
			coord_trans(ls_pos,ucs,FALSE);
			//if(pSecJg->pStart==pSelNode)	//始端连接
			{
				//if(ls_pos.z<vice_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick((long)add_thick,FALSE);
					pBolt->CalGuigeAuto();
					f3dPoint pos = pBolt->ucs.origin;
					pBolt->ucs.origin=pos-pBolt->get_norm()*add_thick;
					if(pSecJg->FindLsByHandle(pBolt->handle))
					{
					if(pXPadPlate&&
						fabs(pBolt->get_norm()*pSecJg->get_norm_x_wing())>
						fabs(pBolt->get_norm()*pSecJg->get_norm_y_wing()))
					{	//X肢螺栓
						pXPadPlate->AppendLsRef(pBolt->GetLsRef());
					}
					else if(pYPadPlate)
						pYPadPlate->AppendLsRef(pBolt->GetLsRef());
					}
					if(pBolt->get_norm()*pShortSegJg1->get_norm_x_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						pBolt->des_base_pos.norm_offset.AddThick(-ftol(add_thick));
					else if(pBolt->get_norm()*pShortSegJg1->get_norm_y_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						pBolt->des_base_pos.norm_offset.AddThick(-ftol(add_thick));
					pBolt->correct_pos();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
			/*else
			{
				if(ls_pos.z>length-vice_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick((long)add_thick,FALSE);
					//pBolt->designL0.AddThick(pShortSegJg1->handle,TRUE);
					pBolt->CalGuigeAuto();
					f3dPoint pos = pBolt->ucs.origin;
					if(pXPadPlate&&
						fabs(pBolt->get_norm()*pSecJg->get_norm_x_wing())>
						fabs(pBolt->get_norm()*pSecJg->get_norm_y_wing()))
					{	//X肢螺栓
						pXPadPlate->AppendLsRef(pBolt->GetLsRef());
					}
					else if(pYPadPlate)
						pYPadPlate->AppendLsRef(pBolt->GetLsRef());
					if(pBolt->get_norm()*pShortSegJg1->get_norm_x_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
					{
						pBolt->ucs.origin=pos-pBolt->get_norm()*add_thick;
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					}
					else if(pBolt->get_norm()*pShortSegJg1->get_norm_y_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
					{
						pBolt->ucs.origin=pos-pBolt->get_norm()*add_thick;
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					}
					pBolt->correct_pos();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}*/
		}
		pShortSegJg1->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg1->GetSolidPartObject());
	}
	else
	{
		long huoqu_handle = Ta.GetNewHandle();
		pShortSegJg1=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg1->iSeg=current_segI;
		pShortSegJg1->SetStart(start_pos);
		pShortSegJg1->SetEnd(base_pos);
		pShortSegJg1->set_wing_wide(wing_wide);
		pShortSegJg1->set_wing_thick(wing_thick);
		pShortSegJg2=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		pShortSegJg2->iSeg=current_segI;
		pShortSegJg2->SetStart(base_pos);
		pShortSegJg2->SetEnd(end_pos);
		pShortSegJg2->set_wing_wide(wing_wide);
		pShortSegJg2->set_wing_thick(wing_thick);
		pShortSegJg1->pEnd	 = pSelNode;
		pShortSegJg2->pStart = pSelNode;
		pShortSegJg1->m_bCutBer=TRUE;//
		pShortSegJg1->m_nCutBerR=(WORD)FindJg_r(pFirstJg->GetWidth())+2;
		pShortSegJg2->m_bCutBer=TRUE;
		pShortSegJg2->m_nCutBerR=(WORD)FindJg_r(pSecJg->GetWidth())+2;
		pShortSegJg1->huoqu_handle=huoqu_handle;
		pShortSegJg2->huoqu_handle=huoqu_handle;
		pShortSegJg1->huoqu_base_wing_x0_y1_both2=2;
		pShortSegJg2->huoqu_base_wing_x0_y1_both2=2;
		pShortSegJg1->bHuoQuEnd=TRUE;
		pShortSegJg2->bHuoQuStart=TRUE;

		pShortSegJg1->desStartPos.SetToDatumPointStyle();
		pShortSegJg1->desStartPos.wing_x_offset.SetOffset();
		pShortSegJg1->desStartPos.wing_y_offset.SetOffset();
		//pShortSegJg1->desStartPos.datum_pos_style=15;
		pShortSegJg1->desEndPos.jgber_cal_style = 1;
		pShortSegJg1->desStartPos.datumPoint.datum_pos_style = 7;
		pShortSegJg1->desStartPos.datumPoint.SetPosition(pShortSegJg1->Start());
		strcpy(pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.norm_offset,start_offset_str);

		pShortSegJg2->desStartPos.jgber_cal_style = 1;
		pShortSegJg2->desEndPos.SetToDatumPointStyle();
		pShortSegJg2->desEndPos.wing_x_offset.SetOffset();
		pShortSegJg2->desEndPos.wing_y_offset.SetOffset();
		//pShortSegJg2->desEndPos.datum_pos_style=15;
		pShortSegJg2->desEndPos.datumPoint.datum_pos_style = 7;
		pShortSegJg2->desEndPos.datumPoint.SetPosition(pShortSegJg2->End());
		double base_extend_len_prj=base_extend_len;
		double vice_extend_len_prj=vice_extend_len;
		if(arrLinkToStart1OrEnd0[0])
		{	//包铁楞点类型的基准点默认是计算杆件正负头的，所以此处应考虑正负头影响 wjh-2013.11.11
			base_extend_len_prj-=base_vec*(pFirstJg->Start()-base_pos)-pFirstJg->startOdd();
			pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.direction = 0;
		}
		else
		{
			base_extend_len_prj-=base_vec*(pFirstJg->End()-base_pos)-pFirstJg->endOdd();
			pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.direction = 1;
		}
		if(arrLinkToStart1OrEnd0[1])
		{
			vice_extend_len_prj-=other_vec*(pSecJg->Start()-base_pos)-pSecJg->startOdd();
			pShortSegJg2->desEndPos.datumPoint.des_para.RODEND.direction = 0;
		}
		else
		{
			vice_extend_len_prj-=other_vec*(pSecJg->End()-base_pos)-pSecJg->endOdd();
			pShortSegJg2->desEndPos.datumPoint.des_para.RODEND.direction = 1;
		}
		strcpy(pShortSegJg2->desEndPos.datumPoint.des_para.RODEND.norm_offset,end_offset_str);

		pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.hRod = pFirstJg->handle;
		pShortSegJg2->desEndPos.datumPoint.des_para.RODEND.hRod = pSecJg->handle;
		pShortSegJg1->desStartPos.datumPoint.des_para.RODEND.len_offset_dist  = base_extend_len_prj;
		pShortSegJg2->desEndPos.datumPoint.des_para.RODEND.len_offset_dist = vice_extend_len_prj;
		pShortSegJg1->iSeg=current_segI;
		pShortSegJg2->iSeg=current_segI;
		pShortSegJg2->cMaterial=pDesignInfo->cMaterial;
		pShortSegJg1->cMaterial=pShortSegJg2->cMaterial;
		pShortSegJg1->SetPartNo(pDesignInfo->sPartNo);
		pShortSegJg2->SetPartNo(pDesignInfo->sPartNo);
		CLDSPolyJoint *pPolyJoint=(CLDSPolyJoint*)console.AppendPart(CLS_POLYJOINT);
		pPolyJoint->cMaterial=pShortSegJg1->cMaterial;
		pPolyJoint->iSeg=current_segI;
		pPolyJoint->SetPartNo(pShortSegJg1->GetPartNo());
		pPolyJoint->pPrevLinePart=pShortSegJg1;
		pPolyJoint->pNextLinePart=pShortSegJg2;
		int add_thick = (int)(pShortSegJg1->GetThick()+pFirstJg->GetThick()-pSecJg->GetThick());
		if(fabs(pFirstJg->GetThick()-pSecJg->GetThick())>=2)
		{	//需要加垫板
			//备份原有角钢选择顺序
			CLDSLineAngle* pBackupFirstAngle=pFirstJg;
			CLDSLineAngle* pBackupSecAngle=pSecJg;
			if(pFirstJg->GetThick()<pSecJg->GetThick())
			{
				pSecJg=pBackupFirstAngle;
				pFirstJg=pBackupSecAngle;
			}

			CJoint joint;
			f3dPoint pos,line_vec;
			int thick = (int)(pFirstJg->GetThick()-pSecJg->GetThick());
			double jg_r=FindJg_r(pSecJg->GetWidth());//角钢内圆半径
			//pJoint=(CJoint*)console.AppendPart(CLS_JOINT);
			joint.base_node_handle = pSelNode->handle;
			pXPadPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pYPadPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pXPadPlate->iSeg=current_segI;
			pYPadPlate->iSeg=current_segI;//pSelNode->iSeg;
			pXPadPlate->SetPartNo(pDesignInfo->xPadPartNo);
			pYPadPlate->SetPartNo(pDesignInfo->yPadPartNo);
			pXPadPlate->jdb_style=5;
			pYPadPlate->jdb_style=5;
			pXPadPlate->designInfo.m_hBaseNode = pSelNode->handle;
			pYPadPlate->designInfo.m_hBaseNode = pSelNode->handle;
			joint.AppendWrapPlank(pXPadPlate->handle,'G');
			joint.AppendWrapPlank(pYPadPlate->handle,'H');
			pXPadPlate->Thick=thick;
			pYPadPlate->Thick=thick;
			pXPadPlate->cfgword=pSelNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
			pYPadPlate->cfgword=pSelNode->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
			pXPadPlate->ucs.axis_z = -pSecJg->get_norm_x_wing();
			pXPadPlate->ucs.axis_x = pSecJg->GetWingVecX();
			pXPadPlate->ucs.axis_y = pXPadPlate->ucs.axis_z^pXPadPlate->ucs.axis_x;
			pXPadPlate->ucs.origin = base_pos+thick*pSecJg->get_norm_x_wing();
			pYPadPlate->ucs.axis_z = -pSecJg->get_norm_y_wing();
			pYPadPlate->ucs.axis_x = pSecJg->GetWingVecY();
			pYPadPlate->ucs.axis_y = pYPadPlate->ucs.axis_z^pYPadPlate->ucs.axis_x;
			pYPadPlate->ucs.origin = base_pos+thick*pSecJg->get_norm_y_wing();
			if(arrLinkToStart1OrEnd0[1])
			{
				line_vec=pSecJg->End()-pSecJg->Start();
				normalize(line_vec);
				//定义X肢垫板顶点
				pos = pXPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pXPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos-line_vec*vice_extend_len;
				pXPadPlate->vertex_list.append(pos);
				//定义Y肢垫板顶点
				pos = pYPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pYPadPlate->vertex_list.append(pos);
				pos = pos-(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
			}
			else
			{
				line_vec=pSecJg->Start()-pSecJg->End();
				normalize(line_vec);
				//定义X肢垫板顶点
				pos = pXPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pXPadPlate->vertex_list.append(pos);
				pos = pos-(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecX();
				pXPadPlate->vertex_list.append(pos);
				//定义Y肢垫板顶点
				pos = pYPadPlate->ucs.origin+jg_r*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos+line_vec*vice_extend_len;
				pYPadPlate->vertex_list.append(pos);
				pos = pos+(pSecJg->GetWidth()-pSecJg->GetThick()-jg_r)*pSecJg->GetWingVecY();
				pYPadPlate->vertex_list.append(pos);
				pos = pos-line_vec*vice_extend_len;
				pYPadPlate->vertex_list.append(pos);
			}
			for(int i=0;i<4;i++)
			{
				coord_trans(pXPadPlate->vertex_list[i].vertex,pXPadPlate->ucs,FALSE);
				coord_trans(pYPadPlate->vertex_list[i].vertex,pYPadPlate->ucs,FALSE);
				pXPadPlate->vertex_list[i].vertex.feature = 1;
				pYPadPlate->vertex_list[i].vertex.feature = 1;
			}
			//复原原有的角钢选择顺序 wjh-2017.1.10
			pSecJg=pBackupSecAngle;
			pFirstJg=pBackupFirstAngle;
		}
		if(arrLinkToStart1OrEnd0[0])	//始端点连接
		{
			pShortSegJg1->set_norm_x_wing(pFirstJg->get_norm_y_wing());
			pShortSegJg1->set_norm_y_wing(pFirstJg->get_norm_x_wing());
		}
		else
		{
			pShortSegJg1->set_norm_x_wing(pFirstJg->get_norm_x_wing());
			pShortSegJg1->set_norm_y_wing(pFirstJg->get_norm_y_wing());
		}
		if(arrLinkToStart1OrEnd0[1])	//始端点连接
		{
			pShortSegJg2->set_norm_x_wing(pSecJg->get_norm_x_wing());
			pShortSegJg2->set_norm_y_wing(pSecJg->get_norm_y_wing());
		}
		else
		{
			pShortSegJg2->set_norm_x_wing(pSecJg->get_norm_y_wing());
			pShortSegJg2->set_norm_y_wing(pSecJg->get_norm_x_wing());
		}
		pShortSegJg1->des_norm_x.spec_norm.vector=pShortSegJg1->get_norm_x_wing();
		pShortSegJg1->des_norm_y.spec_norm.vector=pShortSegJg1->get_norm_y_wing();
		pShortSegJg2->des_norm_x.spec_norm.vector=pShortSegJg2->get_norm_x_wing();
		pShortSegJg2->des_norm_y.spec_norm.vector=pShortSegJg2->get_norm_y_wing();
		
		pShortSegJg1->ClearFlag();
		pShortSegJg2->ClearFlag();
		pShortSegJg1->CalPosition();
		pShortSegJg2->CalPosition();
		pShortSegJg1->LockStartPos();
		pShortSegJg1->LockEndPos();
		pShortSegJg1->bHuoQuEnd=TRUE;
		pShortSegJg2->LockStartPos();
		pShortSegJg2->LockEndPos();
		pShortSegJg2->bHuoQuStart=TRUE;
		/*pShortSegJg1->desStartPos.jgber_cal_style=1;
		pShortSegJg1->desEndPos.jgber_cal_style=1;
		pShortSegJg2->desStartPos.jgber_cal_style=1;
		pShortSegJg2->desEndPos.jgber_cal_style=1;*/
		//添加短角钢上的螺栓
		UCS_STRU ucs;
		pFirstJg->getUCS(ucs);
		CLDSBolt* pBolt;
		double length = DISTANCE(pFirstJg->Start(),pFirstJg->End());
		//for(CLsRef *pLsRef=pFirstJg->GetFirstLsRef();pLsRef;pLsRef=pFirstJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			/*if(pFirstJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					(*pLsRef)->CalGuigeAuto();
					f3dPoint pos = (*pLsRef)->ucs.origin;
					(*pLsRef)->ucs.origin=pos-(*pLsRef)->get_norm()*pShortSegJg1->GetThick();
					if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_x_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					else if((*pLsRef)->get_norm()*pShortSegJg1->get_norm_y_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					(*pLsRef)->correct_pos();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else*/
			if(pFirstJg->FindLsByHandle(pBolt->handle))
			{
				//if(ls_pos.z>length-base_extend_len)	//穿过短角钢
				{
					pShortSegJg1->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(pShortSegJg1->handle,TRUE,TRUE);
					pBolt->CalGuigeAuto();
					f3dPoint pos = pBolt->ucs.origin;
					if(pBolt->get_norm()*pShortSegJg1->get_norm_x_wing()>EPS_COS2 ||
						pBolt->get_norm()*pShortSegJg1->get_norm_y_wing()>EPS_COS2)		//螺栓相对短角钢从里向外穿
					{
						pBolt->ucs.origin=pos-pBolt->get_norm()*pShortSegJg1->GetThick();
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg1->handle,TRUE);
					}
					pBolt->correct_pos();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		pSecJg->getUCS(ucs);
		length = DISTANCE(pSecJg->Start(),pSecJg->End());
		//for(pLsRef=pSecJg->GetFirstLsRef();pLsRef;pLsRef=pSecJg->GetNextLsRef())
		for(pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
		{
			/*if(pSecJg->pStart==pSelNode)	//始端连接
			{
				if(ls_pos.z<vice_extend_len)	//穿过短角钢
				{
					pShortSegJg2->AppendMidLsRef((*pLsRef)->GetLsRef());
					(*pLsRef)->AddL0Thick((long)add_thick,FALSE);
					(*pLsRef)->CalGuigeAuto();
					f3dPoint pos = (*pLsRef)->ucs.origin;
					if(pXPadPlate&&
						fabs((*pLsRef)->get_norm()*pXPadPlate->ucs.axis_z)>
						fabs((*pLsRef)->get_norm()*pYPadPlate->ucs.axis_z))
					{	//X肢螺栓
						pXPadPlate->AppendLsRef((*pLsRef)->GetLsRef());
					}
					else if(pYPadPlate)
						pYPadPlate->AppendLsRef((*pLsRef)->GetLsRef());
					if((*pLsRef)->get_norm()*pShortSegJg2->get_norm_x_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
					{
						(*pLsRef)->ucs.origin=pos-(*pLsRef)->get_norm()*add_thick;
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg2->handle,TRUE);
					}
					else if((*pLsRef)->get_norm()*pShortSegJg2->get_norm_y_wing()>EPS_COS2)//螺栓相对短角钢从里向外穿
					{
						(*pLsRef)->ucs.origin=pos-(*pLsRef)->get_norm()*add_thick;
						(*pLsRef)->des_base_pos.norm_offset.AddThick(-pShortSegJg2->handle,TRUE);
					}
					(*pLsRef)->correct_pos();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
			}
			else*/
			if(pSecJg->FindLsByHandle(pBolt->handle))
			{
				//if(ls_pos.z>length-vice_extend_len)	//穿过短角钢
				{
					pShortSegJg2->AppendMidLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick((long)add_thick,FALSE);
					pBolt->CalGuigeAuto();
					f3dPoint pos = pBolt->ucs.origin;
					if(pXPadPlate&&
						fabs(pBolt->get_norm()*pXPadPlate->ucs.axis_z)>
						fabs(pBolt->get_norm()*pYPadPlate->ucs.axis_z))
					{	//X肢螺栓
						pXPadPlate->AppendLsRef(pBolt->GetLsRef());
					}
					else if(pYPadPlate)
						pYPadPlate->AppendLsRef(pBolt->GetLsRef());
					//计算螺栓位置及法线方向
					if(pBolt->get_norm()*pShortSegJg2->get_norm_x_wing()>EPS_COS2 ||
						pBolt->get_norm()*pShortSegJg2->get_norm_y_wing()>EPS_COS2)		//螺栓相对短角钢从里向外穿
					{
						pBolt->ucs.origin=pos-pBolt->get_norm()*add_thick;
						pBolt->des_base_pos.norm_offset.AddThick(-pShortSegJg2->handle,TRUE);
						if(pBolt->get_norm()*pShortSegJg2->get_norm_x_wing()>EPS_COS2)	//X肢螺栓
						{
							if(pXPadPlate)
								pBolt->des_base_pos.norm_offset.AddThick(-pXPadPlate->handle,TRUE);
						}
						else if(pYPadPlate)
							pBolt->des_base_pos.norm_offset.AddThick(-pYPadPlate->handle,TRUE);
					}
					pBolt->correct_pos();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		pPolyJoint->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPolyJoint->GetSolidPartObject());
		pShortSegJg1->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg1->GetSolidPartObject());
		pShortSegJg2->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pShortSegJg2->GetSolidPartObject());
	}
	if(pXPadPlate)
	{
		pXPadPlate->CalStdProfile();
		pXPadPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pXPadPlate->GetSolidPartObject());
	}
	if(pYPadPlate)
	{
		pYPadPlate->CalStdProfile();
		pYPadPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pYPadPlate->GetSolidPartObject());
	}
	return true;
}
void CLDSView::OnHandShortJg()
{
	m_nPrevCommandID=ID_HAND_SHORT_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat customizing short angle";
#else
	m_sPrevCommandName="重复定制短角钢";
#endif
	//-----vvvvvvv-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
	CCmdLockObject cmdlock(this,false);
	if(!cmdlock.LockSuccessed())
		return;
	DesignShortAngle();
}
BOOL CLDSView::DesignShortAngle()
{
	CString cmdStr="";
	CSmartPtr<CLDSNode>pSelNode;
	CSmartPtr<CLDSLineAngle>pFirstJg,pSecJg;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CUndoOperObject undo(&Ta,true);
	try{
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
		f3dPoint *point=NULL;
		f3dLine line;
		f3dPoint base_pos,base_vec,other_vec;
		/*g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FillCmdLine("请选择需要添加短角钢的节点:","");
		while(1)
		{
			int ret = g_pSolidSnap->SnapPoint(point);
			if(ret<0)
				return;
			else if(ret>0)
			{
				pSelNode=console.FromNodeHandle(point->ID);
				if(pSelNode)
					break;
			}
		}
		g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);*/
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select connecting angle:","");
#else
		pCmdLine->FillCmdLine("请选择待连接角钢:","");
#endif
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prLine);
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);	
			if(obj.hRelaObj>0x20)
			{
				pFirstJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pFirstJg.IsNULL())
				{
					pCmdLine->FinishCmdLine();
					//pCmdLine->FillCmdLine("没有选中合适的角钢，请重新选择以此节点为端点的基准待连接角钢:","");
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("suitable angle not selected,please select connecting angle again:","");
#else
					pCmdLine->FillCmdLine("没有选中合适的角钢，请重新选择待连接角钢:","");
#endif
				}
				else 
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(pFirstJg->handle);
					break;
				}
			}
		}	
		pCmdLine->FinishCmdLine(CXhChar16("0X%X",pFirstJg->handle));
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select another connecting angle or positioning datum nodes for the outside stuck material:","");
#else
		pCmdLine->FillCmdLine("请选择另一根连接角钢或外贴材依赖节点:","");
#endif
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		CLDSNode* pNode;
		while(1)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if(obj.hRelaObj>0x20)
			{
				pSecJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				pNode=console.FromNodeHandle(obj.hRelaObj);
				if(pSecJg.IsHasPtr())
				{
					g_pSolidDraw->SetEntSnapStatus(pFirstJg->handle);
					g_pSolidDraw->SetEntSnapStatus(pSecJg->handle);
					pNode=NULL;
					break;
				}
				else if(pNode)
				{
					g_pSolidDraw->SetEntSnapStatus(pFirstJg->handle);
					g_pSolidDraw->SetEntSnapStatus(pNode->handle);
					pSecJg=NULL;
					break;
				}
				else
				{
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("suitable angle not selected,please select connecting angle again:","");
#else
					pCmdLine->FillCmdLine("没有选中合适的连接角钢或外贴钢板依赖节点,请重新选择:","");
#endif
					continue;
				}
			}
		}
		if(pNode)
			pCmdLine->FinishCmdLine(CXhChar16("节点0X%X",pNode->handle));
		else
			pCmdLine->FinishCmdLine(CXhChar16("角钢0X%X",pSecJg->handle));
		if(pFirstJg.IsHasPtr()&&pSecJg.IsNULL())
		{	//只选择一根角钢时，按外贴材处理
			FinishOuterAttachAngle(pFirstJg,pNode);
			return DesignShortAngle();
		}
		char junction=0;
		const int JUNCTION_CLEARANCE=50;
		if(pFirstJg.IsHasPtr()&&pSecJg.IsHasPtr())
		{
			/* 不允许自动调整基准角钢顺序(遵循用户输入顺序)，否则会导致变坡处外贴材无法生成  wxc 17-1-10
			if(pSecJg->GetThick()>pFirstJg->GetThick())
			{	//保证第一根角钢为厚度较大的角钢，否则无法正确添加垫板	wht 16-06-15
				CLDSLineAngle *pAngle=pSecJg;
				pSecJg=pFirstJg;
				pFirstJg=pAngle;
			}*/
			if(RodJunctionDiscern(pFirstJg,pSecJg,&junction)>=JUNCTION_CLEARANCE)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox(CXhChar50("the nearest ending distance between the two chosen angles to be connected is over %d mm,please check!",JUNCTION_CLEARANCE));
#else
				AfxMessageBox(CXhChar50("选中的两根待连接角钢最近端点距离大于%dmm,请核实!",JUNCTION_CLEARANCE));
#endif
			if(junction&0x01)
			{
				pSelNode=pFirstJg->pStart;
				base_vec = pFirstJg->End()-pFirstJg->Start();
				base_pos = pFirstJg->Start();
			}
			else if(junction&0x02)
			{
				pSelNode=pFirstJg->pEnd;
				base_vec = pFirstJg->Start()-pFirstJg->End();
				base_pos = pFirstJg->End();
			}
			if(junction&0x04)
				other_vec = pSecJg->End()-pSecJg->Start();
			else if(junction&0x08)
				other_vec = pSecJg->Start()-pSecJg->End();
			//g_pSolidDraw->ReleaseSnapStatus();
		}
		else
			base_vec = pFirstJg->End()-pFirstJg->Start();
		//g_pSolidDraw->SetEntSnapStatus(pFirstJg->handle);
		pCmdLine->FinishCmdLine();
		Standarized(base_vec);
		Standarized(other_vec);
		if(base_vec*other_vec>0)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The bending angle between the two angles to be connected is too big, short angle cannot be added!";

		//select connecting bolts
		pCmdLine->FillCmdLine("select the bolts which short angles bring in, or input handle directly(use','to seperate),press enter to confirm:","");
#else
			throw "两衔接角钢间弯曲角度太大不适宜添加短角钢!";
		//选择关联螺栓
		pCmdLine->FillCmdLine("选择短角钢要引入的螺栓,或直接输入句柄(可用','分隔),按回车键确认:","");
#endif
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
		CONNECT_PLATE_INFO arrConnInfo[2];
		arrConnInfo[0].pConnectPlate=arrConnInfo[1].pConnectPlate=NULL;
		if(pSelNode&&pFirstJg.IsHasPtr()&&pSecJg.IsHasPtr())
			SearchRelationPlates(pSelNode,pFirstJg,pSecJg,arrConnInfo);
		else if(pSelNode)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Current short angle's dependant node search failed!";
#else
			throw "当前定制短角钢依赖节点搜索失败!";
#endif
		ARRAY_LIST<long> boltid_arr;
		boltid_arr.SetSize(0,10);
		for(int iPlate=0;iPlate<2;iPlate++)
		{
			if(arrConnInfo[iPlate].pConnectPlate==NULL)
				continue;
			for(CLsRef* pLsRef=arrConnInfo[iPlate].pConnectPlate->GetFirstLsRef();pLsRef;pLsRef=arrConnInfo[iPlate].pConnectPlate->GetNextLsRef())
			{
				if(pFirstJg->FindLsByHandle((*pLsRef)->handle))
					boltid_arr.append((*pLsRef)->handle);
				if(pSecJg->FindLsByHandle((*pLsRef)->handle))
					boltid_arr.append((*pLsRef)->handle);
			}
		}
		if(boltid_arr.GetSize()>0)
			g_pSolidDraw->SetEntSnapStatus(boltid_arr.m_pData,boltid_arr.GetSize());
		BOLTSET boltSetSel;
		while(1)
		{
			if(pCmdLine->GetStrFromCmdLine(cmdStr))
			{
				long *id_arr;
				int nSelBolts=g_pSolidSnap->GetLastSelectEnts(id_arr);
				for(int iSel=0;iSel<nSelBolts;iSel++)
				{
					CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[iSel],CLS_BOLT);
					if(pBolt)
						boltSetSel.append(pBolt);
				}
				pCmdLine->FinishCmdLine();
				break;
			}
			else
				return FALSE;
		}
		if(boltSetSel.GetNodeNum()==0)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No bolts selected,set the length of short angle as default value!");
#else
			AfxMessageBox("未选中任何螺栓,短角钢长度设定为默认值!");
#endif

		defshortjgdlg.info.cMaterial=pFirstJg->cMaterial;
		defshortjgdlg.m_sJgGuiGe.Format("%.0fX%.0f",pFirstJg->GetWidth(),pFirstJg->GetThick());
		defshortjgdlg.m_nThickDiff=(int)fabs(pFirstJg->GetThick()-pSecJg->GetThick());
		defshortjgdlg.m_iJunctPosStyle=WorkWingDiscern(pFirstJg,boltSetSel);
		if(defshortjgdlg.DoModal()!=IDOK)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "design cancelled halfway!";
#else
			throw "设计中途取消!";
#endif
		if(defshortjgdlg.m_iJunctPosStyle==1)		//短角钢位于基准角钢X肢
		{
			if(arrConnInfo[0].pConnectPlate&&arrConnInfo[0].cWorkWing&0x01)
				DesWingXShortJg(pSelNode,pFirstJg,pSecJg,base_pos,base_vec,other_vec,&defshortjgdlg.info,boltSetSel,arrConnInfo[0].pConnectPlate);
			else if(arrConnInfo[1].pConnectPlate&&arrConnInfo[1].cWorkWing&0x01)
				DesWingXShortJg(pSelNode,pFirstJg,pSecJg,base_pos,base_vec,other_vec,&defshortjgdlg.info,boltSetSel,arrConnInfo[1].pConnectPlate);
			else
				DesWingXShortJg(pSelNode,pFirstJg,pSecJg,base_pos,base_vec,other_vec,&defshortjgdlg.info,boltSetSel,NULL);
		}
		else if(defshortjgdlg.m_iJunctPosStyle==2)	//短角钢位于基准角钢Y肢
		{
			if(arrConnInfo[0].pConnectPlate&&arrConnInfo[0].cWorkWing&0x02)
				DesWingYShortJg(pSelNode,pFirstJg,pSecJg,base_pos,base_vec,other_vec,&defshortjgdlg.info,boltSetSel,arrConnInfo[0].pConnectPlate);
			else if(arrConnInfo[1].pConnectPlate&&arrConnInfo[1].cWorkWing&0x02)
				DesWingYShortJg(pSelNode,pFirstJg,pSecJg,base_pos,base_vec,other_vec,&defshortjgdlg.info,boltSetSel,arrConnInfo[1].pConnectPlate);
			else
				DesWingYShortJg(pSelNode,pFirstJg,pSecJg,base_pos,base_vec,other_vec,&defshortjgdlg.info,boltSetSel,NULL);
		}
		else						//短角钢位于基准角钢里面(内包)
			DesInternalBerPosShortJg(pSelNode,pFirstJg,pSecJg,base_pos,base_vec,other_vec,&defshortjgdlg.info,boltSetSel);
		m_pDoc->SetModifiedFlag();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	g_pSolidDraw->ReleaseSnapStatus();
	return DesignShortAngle();
}
void CLDSView::OnOuterAttachAngle()
{
	//-----vvvvvvv-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
	CCmdLockObject cmdlock(this,false);
	if(!cmdlock.LockSuccessed())
		return;
	FinishOuterAttachAngle();
}
BOOL CLDSView::FinishOuterAttachAngle(CLDSLineAngle* pAngle/*=NULL*/,CLDSNode* pNode/*=NULL*/)
{
	CString cmdStr="";
	CSmartPtr<CLDSLineAngle>pDatumJg,pDatumJg1;
	if(pAngle==NULL)
	{
		long *id_arr=NULL;
		g_pSolidSnap->GetLastSelectEnts(id_arr);
		pDatumJg=(CLDSLineAngle*)console.FromPartHandle(id_arr[0],CLS_LINEANGLE);
		pDatumJg1=(CLDSLineAngle*)console.FromPartHandle(id_arr[1],CLS_LINEANGLE);
	}
	else
		pDatumJg=pAngle;
	if(pDatumJg.IsNULL())
		return FALSE;
	//
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CUndoOperObject undo(&Ta,true);
	try{
		g_pSolidDraw->ReleaseSnapStatus();
		CLDSNode* pBaseNode=NULL;
		if(pNode==NULL)
		{
	#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select positioning datum nodes for the outside stuck material:","");
	#else
			pCmdLine->FillCmdLine("请选择外贴材的定位基准节点:","");
	#endif
			DWORD dwhObj=0,dwExportFlag=0;
			CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
			verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
			CDisplayNodeAtFrontLife showNode;
			showNode.DisplayNodeAtFront();
			while(1)
			{
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				pBaseNode=console.FromNodeHandle(obj.hRelaObj);
				if(pBaseNode&&pBaseNode->GetClassTypeId()==CLS_NODE)
					break;
				else
				{
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("positioning datum nodes for the outside stuck material not selected,please select:","");
#else
					pCmdLine->FillCmdLine("没有选择合适的外贴材定位基准节点,请重新选择:","");
#endif
					continue;
				}
			}
			pCmdLine->FinishCmdLine(CXhChar16("0X%X",pBaseNode->handle));
			g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
		}
		else
			pBaseNode=pNode;
		//2.选择关联螺栓
		g_pSolidSet->SetDisplayType(DISP_SOLID);
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("select the bolts which short angles bring in, or input handle directly(use','to seperate),press enter to confirm:","");
#else
		pCmdLine->FillCmdLine("选择短角钢要引入的螺栓,或直接输入句柄(可用','分隔),按回车键确认:","");
#endif
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
		((CMainFrame*)AfxGetMainWnd())->GetLDSView()->OpenWndSel();
		CONNECT_PLATE_INFO arrConnInfo;
		arrConnInfo.pConnectPlate=NULL;
		if(pBaseNode&&pDatumJg.IsHasPtr())
			SearchRelationPlates(pBaseNode,pDatumJg,arrConnInfo);
		ARRAY_LIST<long> boltid_arr;
		boltid_arr.SetSize(0,10);
		if(arrConnInfo.pConnectPlate!=NULL)
		{
			for(CLsRef* pLsRef=arrConnInfo.pConnectPlate->GetFirstLsRef();pLsRef;pLsRef=arrConnInfo.pConnectPlate->GetNextLsRef())
			{
				if(pDatumJg->FindLsByHandle((*pLsRef)->handle))
					boltid_arr.append((*pLsRef)->handle);
			}
		}
		if(boltid_arr.GetSize()>0)
			g_pSolidDraw->SetEntSnapStatus(boltid_arr.m_pData,boltid_arr.GetSize());
		BOLTSET boltSetSel;
		while(1)
		{
			if(pCmdLine->GetStrFromCmdLine(cmdStr))
			{
				long *id_arr;
				int nSelBolts=g_pSolidSnap->GetLastSelectEnts(id_arr);
				for(int iSel=0;iSel<nSelBolts;iSel++)
				{
					CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[iSel],CLS_BOLT);
					if(pBolt)
						boltSetSel.append(pBolt);
				}
				pCmdLine->FinishCmdLine();
				break;
			}
			else
				return FALSE;
		}
		designOuterAttachDlg.info.cMaterial=pDatumJg->cMaterial;
		designOuterAttachDlg.info.iSeg=pDatumJg->iSeg;
		designOuterAttachDlg.m_sSpec.Format("%gX%g",pDatumJg->GetWidth(),pDatumJg->GetThick());
		if(boltSetSel.GetNodeNum()==0)
			return FALSE;
		else
		{
			designOuterAttachDlg.m_nWorkWingOfDepend=WorkWingDiscern(pDatumJg,boltSetSel);
			if(pDatumJg&&pDatumJg1)
				designOuterAttachDlg.m_nOutterAttachStyle=5;	//基准角钢交叉处外贴材
			else
				designOuterAttachDlg.m_nOutterAttachStyle=0;	//顺帖|外包
			CLDSBolt* pBolt=boltSetSel.GetFirst();
			if(pBaseNode&&pDatumJg.IsHasPtr())
				SearchRelationPlates(pBaseNode,pDatumJg,arrConnInfo,pBolt->ucs.axis_z);
		}
		designOuterAttachDlg.info.pDatumNode=pBaseNode;
		designOuterAttachDlg.info.pDatumJg=pDatumJg;
		designOuterAttachDlg.info.pDatumJg1=pDatumJg1;
		designOuterAttachDlg.info.pBoltSet=&boltSetSel;
		designOuterAttachDlg.info.pRelaPlate=arrConnInfo.pConnectPlate;
		designOuterAttachDlg.DesOuterAttachShortJg=DesOuterAttachShortJg;
		//预先生成短角钢
		designOuterAttachDlg.m_pShortSegJg=NULL;
		designOuterAttachDlg.UpdateOuterAttachAngleInfo(FALSE);
		DesOuterAttachShortJg(designOuterAttachDlg.m_pShortSegJg,&designOuterAttachDlg.info);
		if(designOuterAttachDlg.DoModal()!=IDOK)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			if(designOuterAttachDlg.m_pShortSegJg)
			{
				console.DispPartSet.DeleteNode(designOuterAttachDlg.m_pShortSegJg->handle);
				g_pSolidDraw->DelEnt(designOuterAttachDlg.m_pShortSegJg->handle);
				console.DeletePart(designOuterAttachDlg.m_pShortSegJg->handle);
			}
			return FALSE;
		}
		DesOuterAttachShortJg(designOuterAttachDlg.m_pShortSegJg,&designOuterAttachDlg.info);
		m_pDoc->SetModifiedFlag();
		//对称生成
		static CMirMsgDlg mir_dlg;
		if(designOuterAttachDlg.m_pShortSegJg && mir_dlg.DoModal()==IDOK)
			MirTaAtom(designOuterAttachDlg.m_pShortSegJg,mir_dlg.mirmsg);
	}
	catch(char *sError)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		AfxMessageBox(sError);
		return FALSE;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	return TRUE;
}
#else
void CLDSView::OnHandShortJg(){;}
void CLDSView::OnOuterAttachAngle(){;}
BOOL CLDSView::FinishOuterAttachAngle(CLDSLineAngle* pAngle/*=NULL*/,CLDSNode* pNode/*=NULL*/){;}
#endif