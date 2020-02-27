//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "dlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "MirTaAtom.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "GroupJgStyleDlg.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////
static CLinkJGDlg link_jg_dlg;
/////////////////////////////////////////////////////////
/*共分为三种情况计算角钢的肢法线方向
	1.当前角钢两端点中有一个是派生节点,此时根据此节点计算
	  当前角钢的肢法线方向
	2.当前角钢的两端点均为基本节点,但有一端点的父角钢与当前角钢不相同
	3.当前角钢的两端点均为基本节点,且都还没有父节点
	****派生节点包括坐标无任何依赖的节点和Z坐标轴不变的节点****
*/
static BOOL InitLineAnglePosAndWingNorm(CLDSLineAngle* pCurJg,f3dPoint view_norm,BOOL bStartToBer/*=FALSE*/,BOOL bEndToBer/*=FALSE*/)
{
	CLDSPart *pPart;
	CString sErrorMsg;
	f3dPoint vec;

	try{
		//计算当前角钢的延伸方向，自始端指向终端为正
		Sub_Pnt(vec,pCurJg->pEnd->Position(true),pCurJg->pStart->Position(true));
		if(vec.mod()<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Current angle's start and end point overlap,data error!";
#else
			throw "当前角钢始终端点重合,数据错误!";
#endif
		normalize(vec);
		normalize(view_norm);
		if(fabs(vec*view_norm)>EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "View vector specified wrong!";
#else
			throw "视向指定错误!";
#endif
		switch(pCurJg->LayStyle)
		{
		case 1:	//X肢为当前肢
		case 3:	//X肢为当前肢
			if(pCurJg->LayStyle==1)	//外铁
				pCurJg->des_norm_x.near_norm =-view_norm;	//与肢法线接近的基准法线
			else					//里铁
				pCurJg->des_norm_x.near_norm = view_norm;	//与肢法线接近的基准法线
			pCurJg->des_norm_x.bByOtherWing = FALSE;
			pCurJg->des_norm_y.bByOtherWing = TRUE;		//Y肢法线以X肢法线为基准
			if(pCurJg->pStart->hFatherPart<0x20)
			{
				pCurJg->pStart->hFatherPart = pCurJg->handle;
				pCurJg->pStart->SetModified();
				if( pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desStartPos.spatialOperationStyle=0;	//指定基准点
					pCurJg->desStartPos.datumPoint.datum_pos_style=8;
					pCurJg->desStartPos.datumPoint.des_para.hNode=pCurJg->pStart->handle;
					pCurJg->desStartPos.wing_x_offset.gStyle=4;
					pCurJg->desStartPos.wing_x_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//头部角钢默认为两肢坐标控制
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desStartPos.wing_x_offset.gStyle = 4;	//角钢X肢紧贴父角钢
				pCurJg->desStartPos.wing_x_offset.offsetDist = 0;
				pCurJg->desStartPos.wing_y_offset.gStyle = 0;	//角钢Y肢偏移一个准距
				//pCurJg->desStartPos.wing_y_offset.offsetDist = 0;
			}
			if(pCurJg->pEnd->hFatherPart<0x20)
			{
				pCurJg->pEnd->hFatherPart = pCurJg->handle;
				pCurJg->pEnd->SetModified();
				if( pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desEndPos.spatialOperationStyle=0;	//指定基准点
					pCurJg->desEndPos.datumPoint.datum_pos_style=8;
					pCurJg->desEndPos.datumPoint.des_para.hNode=pCurJg->pEnd->handle;
					pCurJg->desEndPos.wing_x_offset.gStyle=4;
					pCurJg->desEndPos.wing_x_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//头部角钢默认为两肢坐标控制
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desEndPos.wing_x_offset.gStyle = 4;	//角钢X肢紧贴父角钢
				pCurJg->desEndPos.wing_x_offset.offsetDist = 0;
				pCurJg->desEndPos.wing_y_offset.gStyle = 0;	//角钢Y肢偏移一个准距
				//pCurJg->desEndPos.wing_y_offset.offsetDist = 0;
			}
			break;
		case 2:	//Y肢为当前肢
		case 4:	//Y肢为当前肢
		default:
			if(pCurJg->LayStyle==2)	//里铁
				pCurJg->des_norm_y.near_norm = view_norm;	//与肢法线接近的基准法线
			else					//外铁
				pCurJg->des_norm_y.near_norm =-view_norm;	//与肢法线接近的基准法线
			pCurJg->des_norm_y.bByOtherWing = FALSE;
			pCurJg->des_norm_x.bByOtherWing = TRUE;		//X肢法线以Y肢法线为基准
			if(pCurJg->pStart->hFatherPart<0x20)
			{
				pCurJg->pStart->hFatherPart = pCurJg->handle;
				pCurJg->pStart->SetModified();
				if( pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desStartPos.spatialOperationStyle=0;	//指定基准点
					pCurJg->desStartPos.datumPoint.datum_pos_style=8;
					pCurJg->desStartPos.datumPoint.des_para.hNode=pCurJg->pStart->handle;
					pCurJg->desStartPos.wing_y_offset.gStyle=4;
					pCurJg->desStartPos.wing_y_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//头部角钢默认为两肢坐标控制
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desStartPos.wing_x_offset.gStyle = 0;	//角钢X肢偏移一个准距
				//pCurJg->desStartPos.wing_x_offset.offsetDist = 0;
				pCurJg->desStartPos.wing_y_offset.gStyle = 4;	//角钢Y肢紧贴父角钢
				pCurJg->desStartPos.wing_y_offset.offsetDist = 0;
			}
			if(pCurJg->pEnd->hFatherPart<0x20)
			{
				pCurJg->pEnd->hFatherPart = pCurJg->handle;
				pCurJg->pEnd->SetModified();
				if( pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desEndPos.spatialOperationStyle=0;	//指定基准点
					pCurJg->desEndPos.datumPoint.datum_pos_style=8;
					pCurJg->desEndPos.datumPoint.des_para.hNode=pCurJg->pEnd->handle;
					pCurJg->desEndPos.wing_y_offset.gStyle=4;
					pCurJg->desEndPos.wing_y_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//头部角钢默认为两肢坐标控制
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desEndPos.wing_x_offset.gStyle = 0;	//角钢X肢偏移一个准距
				//pCurJg->desEndPos.wing_x_offset.offsetDist = 0;
				pCurJg->desEndPos.wing_y_offset.gStyle = 4;	//角钢Y肢紧贴父角钢
				pCurJg->desEndPos.wing_y_offset.offsetDist = 0;
			}
			break;
		}
		
		
		CSuperSmartPtr<CLDSLinePart> pStartFatherPart,pEndFatherPart;
		if(pCurJg->desStartPos.spatialOperationStyle==0&&pCurJg->desStartPos.datumPoint.datum_pos_style==3)
			pPart=console.FromPartHandle(pCurJg->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		else
			pPart=console.FromPartHandle(pCurJg->pStart->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pStartFatherPart = (CLDSLinePart*)pPart;
		if(pCurJg->desEndPos.spatialOperationStyle==0&&pCurJg->desEndPos.datumPoint.datum_pos_style==3)
			pPart=console.FromPartHandle(pCurJg->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		else
			pPart=console.FromPartHandle(pCurJg->pEnd->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pEndFatherPart = (CLDSLinePart*)pPart;
		short start_attach_type=pCurJg->pStart->m_cPosCalType;//当前角钢始端节点依附关系
		short end_attach_type=pCurJg->pEnd->m_cPosCalType;	//当前角钢终端节点依附关系
		/*  这部分代码由于TMA V5版本已对横隔面角点连材进行了处理(交于心线交点)，
			现在已无实际意义，且可能有负面意义 wjh-2016.5.19
			****0.0特殊情况主要处理当前角钢为横隔面交叉材的情况*****
		if(toupper(pCurJg->layer(2))=='G'&&pStartFatherPart.IsHasPtr())
		{
			f3dPoint cur_norm = CalFaceNorm(pCurJg->Start(),pCurJg->End(),pStartFatherPart->Start(),pStartFatherPart->End());
			if(cur_norm.mod()<EPS||fabs(cur_norm*view_norm)<EPS)
			{
				pStartFatherPart=NULL;
				pEndFatherPart=NULL;
				start_attach_type=end_attach_type=0; //按是终端节点为基准节点的情况进行处理
			}
		}
		*/
		CLDSLinePart* pWorkPlaneRod=NULL;
		//	****1.0 以下两种为当前角钢有一端节点为派生节点的情况****
		/*
		当前角钢端节点的父角钢已存在(节点为其父角钢的派生节点,角钢的
		派生节点一般位于角钢的中间不应位于角钢的端点,否则认为是数据错误，
		，此时抛出错误信息),一般来说,它们(派生节点)的父角钢不应与当前角
		钢共线,如果这样,则认为是数据错误，此时抛出错误信息。
		*/
		if(!pStartFatherPart.IsHasPtr())
		{
			char sError[MAX_TEMP_CHAR_200+1]="";
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sError,MAX_TEMP_CHAR_200,"parent rod of node 0X%X lost, failure in calculating leg's normal direction!",pCurJg->pStart->handle);
#else
			_snprintf(sError,MAX_TEMP_CHAR_200,"节点0X%X的的父杆件丢失,肢法线方向计算失败!",pCurJg->pStart->handle);
#endif
			throw sError;
		}
		else if(start_attach_type>3&&pStartFatherPart->handle!=pCurJg->handle)
		{	//始端点为派生节点
			if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
				pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
			else								//X肢法线以Y肢法线为计算基准
				pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
			pWorkPlaneRod=pStartFatherPart;
		}
		else if(!pEndFatherPart.IsHasPtr())
		{
			char sError[MAX_TEMP_CHAR_200+1]="";
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sError,MAX_TEMP_CHAR_200,"parent rod of node0X%X lost, failure in calculating leg's normal direction!",pCurJg->pEnd->handle);
#else
			_snprintf(sError,MAX_TEMP_CHAR_200,"节点0X%X的父杆件丢失,肢法线方向计算失败!",pCurJg->pEnd->handle);
#endif
			throw sError;
		}
		else if((start_attach_type<=3||pStartFatherPart.IsNULL())&&end_attach_type>3&&pEndFatherPart->handle!=pCurJg->handle)
		{	//终端点为派生节点
			if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
				pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
			else								//X肢法线以Y肢法线为计算基准
				pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
			pWorkPlaneRod=pEndFatherPart;
		}
		//	****2.0 以下为当前角钢的端节点均为基本节点的情况****
		else
		{
			if( pStartFatherPart.IsHasPtr()&&pStartFatherPart->handle!=pCurJg->handle
				&&pStartFatherPart->pStart!=pCurJg->pEnd	//自身为节点父杆件时不能作为肢法线参照杆件 wjh-2014.11.28
				&&pStartFatherPart->pEnd!=pCurJg->pEnd
				&&pStartFatherPart->cfgword.And(pCurJg->cfgword))//始端节点父角钢不在当前模型 wht 10-06-06
			{	//始端点父角钢与当前角钢不相同
				if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
					pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
				else								//X肢法线以Y肢法线为计算基准
					pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
				pWorkPlaneRod=pStartFatherPart;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->handle!=pCurJg->handle
				&&pEndFatherPart->pStart!=pCurJg->pStart	//自身为节点父杆件时不能作为肢法线参照杆件 wjh-2014.11.28
				&&pEndFatherPart->pEnd!=pCurJg->pStart
				&&pEndFatherPart->cfgword.And(pCurJg->cfgword))	//终端节点父角钢不在当前模型 wht 10-06-06
			{	//终端点父角钢与当前角钢不相同
				if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
					pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
				else								//X肢法线以Y肢法线为计算基准
					pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
				pWorkPlaneRod=pEndFatherPart;
			}
			//无法根据父角钢确定该角钢的肢法线方向时按塔身主材计算肢法矢
			//现改为按视图法线方向来确定  WJH--01.06.25
			else
			{
				if(pCurJg->des_norm_x.bByOtherWing)
				{	//X肢法线以Y肢法线为计算基准
					pCurJg->set_norm_y_wing(pCurJg->des_norm_y.near_norm,TRUE);
				}
				else
				{	//Y肢法线以X肢法线为计算基准
					pCurJg->set_norm_x_wing(pCurJg->des_norm_x.near_norm,TRUE);
				}
			}
		}
		//计算角钢两肢法线
		DESIGN_JGWING_NORM* pWorkWingNorm=!pCurJg->des_norm_x.bByOtherWing ? &pCurJg->des_norm_x : &pCurJg->des_norm_y;
		if(pWorkPlaneRod!=NULL&&!pWorkWingNorm->bByOtherWing&&pCurJg->pStart&&pCurJg->pEnd)
		{	//针对工作面法线与当前视图方向差距过大情况
			f3dPoint lenDependRodVec=pWorkPlaneRod->End()-pWorkPlaneRod->Start();
			f3dPoint lenRodVec=pCurJg->pEnd->Position(false)-pCurJg->pStart->Position(false);
			normalize(lenRodVec);
			normalize(lenDependRodVec);
			normalize(pWorkWingNorm->near_norm);
			f3dPoint vWorkNorm=lenDependRodVec^lenRodVec;
			normalize(vWorkNorm);
			if(fabs(vWorkNorm*pWorkWingNorm->near_norm)<0.707)
			{	//当前角钢与依赖杆件工作面法线与当前视图法线夹角大于45°
				pWorkWingNorm->near_norm=pWorkWingNorm->near_norm^lenRodVec;
				if(pCurJg->LayStyle==2||pCurJg->LayStyle==4)	//Y肢为当前工作肢
					pWorkWingNorm->near_norm*=-1.0;
				//当前工作肢X/Y调换
				DESIGN_JGWING_NORM temp=pCurJg->des_norm_x;
				pCurJg->des_norm_x=pCurJg->des_norm_y;
				pCurJg->des_norm_y=temp;
			}
		}
		pCurJg->cal_x_wing_norm();
		pCurJg->cal_y_wing_norm();
		if(!pCurJg->des_norm_x.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)	//单角钢
			{
				if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desStartPos.spatialOperationStyle = 2;
				else
					pCurJg->desStartPos.spatialOperationStyle = 3;
				//因横担处若默认设为平推，则侧面角钢与正面角钢处于两个角钢截面上导致错误，
				//实际此处应将主材设定正面肢为搭接定位基准肢，目前已通过简单定位解决此问题　wjh-2016.5.18
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//指定X坐标分量点
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//指定Y坐标分量点
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desStartPos.spatialOperationStyle+=3;	//投影改平推
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//指定X坐标分量点
				//	pCurJg->desStartPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//指定Y坐标分量点
				//	pCurJg->desStartPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desStartPos.face_offset_norm.Set(0,0,1);
			}
			else if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pCurJg==pStartFatherPart)
				{
					//pCurJg->desEndPos.datum_pos_style = 0;
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 18;
				}
				else if(pGroupJg->group_style==1)	//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desStartPos.spatialOperationStyle = 2;
					else
						pCurJg->desStartPos.spatialOperationStyle = 1;	//搭在Y肢上时实际应交于楞线
				}
				else
					pCurJg->desStartPos.spatialOperationStyle=1;	//交于楞线
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desStartPos.spatialOperationStyle = 1;	//节点向轴线投影
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode!");
#else
				logerr.Log("未处理方式!");
#endif
			if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desEndPos.spatialOperationStyle = 2;
				else
					pCurJg->desEndPos.spatialOperationStyle = 3;
				//因横担处若默认设为平推，则侧面角钢与正面角钢处于两个角钢截面上导致错误，
				//实际此处应将主材设定正面肢为搭接定位基准肢，目前已通过简单定位解决此问题　wjh-2016.5.18
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//指定X坐标分量点
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//指定Y坐标分量点
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desEndPos.spatialOperationStyle+=3;	//投影改平推
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//指定X坐标分量点
				//	pCurJg->desEndPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//指定Y坐标分量点
				//	pCurJg->desEndPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desEndPos.face_offset_norm.Set(0,0,1);
			}
			else if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pCurJg==pEndFatherPart)
				{
					//pCurJg->desEndPos.datum_pos_style = 0;
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 18;
				}
				else if(pGroupJg->group_style==1)	//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desEndPos.spatialOperationStyle = 2;
					else
						pCurJg->desEndPos.spatialOperationStyle = 1;
				}
				else
					pCurJg->desEndPos.spatialOperationStyle=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desEndPos.spatialOperationStyle=1;	//节点向轴线投影
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode!");
#else
				logerr.Log("未处理方式!");
#endif
		}
		else //if(pCurJg->des_norm_y.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desStartPos.spatialOperationStyle = 2;
				else
					pCurJg->desStartPos.spatialOperationStyle = 3;
				//因横担处若默认设为平推，则侧面角钢与正面角钢处于两个角钢截面上导致错误，
				//实际此处应将主材设定正面肢为搭接定位基准肢，目前已通过简单定位解决此问题　wjh-2016.5.18
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//指定X坐标分量点
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//指定Y坐标分量点
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desStartPos.spatialOperationStyle+=3;	//投影改平推
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//指定X坐标分量点
				//	pCurJg->desStartPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//指定Y坐标分量点
				//	pCurJg->desStartPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desStartPos.face_offset_norm.Set(0,0,1);
			}
			else if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pGroupJg->group_style==1)		//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desStartPos.spatialOperationStyle = 2;
					else
						pCurJg->desStartPos.spatialOperationStyle = 1;
				}
				else
					pCurJg->desStartPos.spatialOperationStyle=1;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desStartPos.spatialOperationStyle = 1;	//节点向轴线投影
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode!");
#else
				logerr.Log("未处理方式!");
#endif
			if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desEndPos.spatialOperationStyle = 2;
				else
					pCurJg->desEndPos.spatialOperationStyle = 3;
				//因横担处若默认设为平推，则侧面角钢与正面角钢处于两个角钢截面上导致错误，
				//实际此处应将主材设定正面肢为搭接定位基准肢，目前已通过简单定位解决此问题　wjh-2016.5.18
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//指定X坐标分量点
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//指定Y坐标分量点
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desEndPos.spatialOperationStyle+=3;	//投影改平推
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//指定X坐标分量点
				//	pCurJg->desEndPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//指定Y坐标分量点
				//	pCurJg->desEndPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//指定Z坐标分量点
				//	pCurJg->desEndPos.face_offset_norm.Set(0,0,1);
			}
			else if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pGroupJg->group_style==1)	//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desEndPos.spatialOperationStyle = 2;
					else
						pCurJg->desEndPos.spatialOperationStyle = 1;
				}
				else
					pCurJg->desEndPos.spatialOperationStyle=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desEndPos.spatialOperationStyle = 1;	//节点向轴线投影
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode");
#else
				logerr.Log("未处理方式");
#endif
		}
		if(bStartToBer&&pCurJg!=pStartFatherPart)
			pCurJg->desStartPos.spatialOperationStyle=1;
		if(bEndToBer&&pCurJg!=pEndFatherPart)
			pCurJg->desEndPos.spatialOperationStyle=1;
		if(bStartToBer)
			pCurJg->desStartPos.jgber_cal_style=0;
		if(bEndToBer)
			pCurJg->desEndPos.jgber_cal_style=0;
		//通过简单定位智能识别角钢的楞点位置参数　wjh-2017.12.14
		BYTE ciStartBerCalStyle =pCurJg->desStartPos.jgber_cal_style;
		BYTE ciEndBerCalStyle	=pCurJg->desEndPos.jgber_cal_style;
		if(pCurJg->desStartPos.spatialOperationStyle==15)
			pCurJg->desStartPos.jgber_cal_style=2;
		if(pCurJg->desEndPos.spatialOperationStyle==15)
			pCurJg->desEndPos.jgber_cal_style=2;
		pCurJg->CalPosition();
		pCurJg->desStartPos.jgber_cal_style=ciStartBerCalStyle;
		pCurJg->desEndPos.jgber_cal_style  =ciEndBerCalStyle;
	}
	catch(char *sError)
	{
		CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("error in calculating angle leg normal, %s",sError);
#else
		ss.Format("计算角钢肢法线错误，%s",sError);
#endif
		AfxMessageBox(ss);
	}
	return TRUE;
}

//根据当前杆件始/终端节点父杆件类型判断始/终端是否需要平推
static BOOL IsNeedHoriOffset(CLDSNode *pNode)
{
	CLDSLinePart *pLinePart = (CLDSLinePart*)console.FromPartHandle(pNode->hFatherPart,CLS_LINEPART);
	if(pLinePart == NULL)
		return FALSE;
	if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE&&pLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		return FALSE;
	if(toupper(pLinePart->layer(0))=='S'&&toupper((pLinePart->layer(1))=='Z'))
		return TRUE;
	return FALSE;
}

bool InitHorizonCrossAnglePosition(CLDSLineAngle* pAngle)
{
	if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
		return false;
	if(fabs(pAngle->pStart->Position('Z')-pAngle->pEnd->Position('Z'))>EPS)
		return false;
	if(pAngle->pStart->Layer(0)!='L'&&pAngle->pStart->Layer(0)!='S')
		return false;	//起点不始塔身节点
	if(pAngle->pEnd->Layer(0)!='L'&&pAngle->pEnd->Layer(0)!='S')
		return false;	//终点不始塔身节点
	if(pAngle->Layer(0)!='L'&&pAngle->Layer(0)!='S')
		return false;	//终点不始塔身节点
	char cQuadStart=pAngle->pStart->layer(2)-'0';
	char cQuadEnd=pAngle->pEnd->layer(2)-'0';
	GEPOINT lenVec=pAngle->pEnd->Position(false)-pAngle->pStart->Position(false);
	normalize(lenVec);
	if(fabs(lenVec.z)>EPS2||fabs(lenVec.x)<=0.25||fabs(lenVec.y)<=0.25||
		((cQuadStart<0||cQuadStart>4)&&(cQuadEnd<0||cQuadEnd>4)))
		return false;	//不是对角交叉斜材
	CLDSLineAngle* startLinkAnglePtrArr[2];
	CLDSLineAngle* endLinkAnglePtrArr[2];
	startLinkAnglePtrArr[0]=startLinkAnglePtrArr[1]=endLinkAnglePtrArr[0]=endLinkAnglePtrArr[1]=NULL;
	BOOL pushed=Ta.Parts.push_stack();
	for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pLineAngle;
		pLineAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pLineAngle->pStart==NULL||pLineAngle->pEnd==NULL||pLineAngle==pAngle)
			continue;
		if(pLineAngle->Layer(0)!='L'&&pLineAngle->Layer(0)!='S')
			continue;
		if(pLineAngle->pStart==pAngle->pStart||pLineAngle->pEnd==pAngle->pStart)
		{
			if(fabs(pLineAngle->pStart->Position('Z')-pLineAngle->pEnd->Position('Z'))<EPS)
			{
				if(fabs(pLineAngle->pStart->Position('X')-pLineAngle->pEnd->Position('X'))<EPS)
					startLinkAnglePtrArr[1]=pLineAngle;
				else if(fabs(pLineAngle->pStart->Position('Y')-pLineAngle->pEnd->Position('Y'))<EPS)
					startLinkAnglePtrArr[0]=pLineAngle;
			}
		}
		if(pLineAngle->pStart==pAngle->pEnd||pLineAngle->pEnd==pAngle->pEnd)
		{
			if(fabs(pLineAngle->pStart->Position('Z')-pLineAngle->pEnd->Position('Z'))<EPS)
			{
				if(fabs(pLineAngle->pStart->Position('X')-pLineAngle->pEnd->Position('X'))<EPS)
					endLinkAnglePtrArr[1]=pLineAngle;
				else if(fabs(pLineAngle->pStart->Position('Y')-pLineAngle->pEnd->Position('Y'))<EPS)
					endLinkAnglePtrArr[0]=pLineAngle;
			}
		}
		if( startLinkAnglePtrArr[0]!=NULL&&startLinkAnglePtrArr[1]!=NULL&&
			endLinkAnglePtrArr[0]!=NULL&&endLinkAnglePtrArr[1]!=NULL)
			break;
	}
	Ta.Parts.pop_stack(pushed);
	if( startLinkAnglePtrArr[0]!=NULL&&startLinkAnglePtrArr[1]!=NULL)
	{
		GEPOINT vHoriLenVec0=startLinkAnglePtrArr[0]->pEnd->Position(false)-startLinkAnglePtrArr[0]->pStart->Position(false);
		GEPOINT vHoriLenVec1=startLinkAnglePtrArr[1]->pEnd->Position(false)-startLinkAnglePtrArr[1]->pStart->Position(false);
		if(startLinkAnglePtrArr[0]->pEnd==pAngle->pStart)
			vHoriLenVec0*=-1;
		if(startLinkAnglePtrArr[1]->pEnd==pAngle->pStart)
			vHoriLenVec1*=-1;
		if(lenVec*vHoriLenVec0>EPS&&lenVec*vHoriLenVec1>EPS)
		{
			pAngle->desStartPos.spatialOperationStyle=0;
			pAngle->desStartPos.datumPoint.datum_pos_style=3;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1=startLinkAnglePtrArr[0]->handle;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum2=startLinkAnglePtrArr[1]->handle;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.near_vector[0]=0;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.near_vector[1]=0;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.near_vector[2]=-1.0;
			strcpy(pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.norm_offset,"");
		}
		else
			return false;
	}
	if(endLinkAnglePtrArr[0]!=NULL&&endLinkAnglePtrArr[1]!=NULL)
	{
		GEPOINT vHoriLenVec0=endLinkAnglePtrArr[0]->pEnd->Position(false)-endLinkAnglePtrArr[0]->pStart->Position(false);
		GEPOINT vHoriLenVec1=endLinkAnglePtrArr[1]->pEnd->Position(false)-endLinkAnglePtrArr[1]->pStart->Position(false);
		if(endLinkAnglePtrArr[0]->pEnd==pAngle->pEnd)
			vHoriLenVec0*=-1;
		if(endLinkAnglePtrArr[1]->pEnd==pAngle->pEnd)
			vHoriLenVec1*=-1;
		if(lenVec*vHoriLenVec0<-EPS&&lenVec*vHoriLenVec1<-EPS)
		{
			pAngle->desEndPos.spatialOperationStyle=0;
			pAngle->desEndPos.datumPoint.datum_pos_style=3;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1=endLinkAnglePtrArr[0]->handle;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum2=endLinkAnglePtrArr[1]->handle;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.near_vector[0]=0;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.near_vector[1]=0;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.near_vector[2]=-1.0;
			strcpy(pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.norm_offset,"");
		}
		else
			return false;
	}
	return true;
}
//在连接塔身斜时用户最好不要在透视图下进行，
//此时系统视向缺省为3（从前面看）
CLDSLineAngle* CLDSView::FinishLinkAngle(CLDSNode *pStartNode, CLDSNode *pEndNode)
{
	CLDSLineAngle* pNowJg=NULL;
	f3dPoint view_norm,side_view_norm;
	double wing_wide,wing_thick;
	link_jg_dlg.m_nViewFlag = m_eViewFlag;
	//link_jg_dlg.m_nJgMaterial=0; //保留上一次输入的材质 wht 11-04-25
	link_jg_dlg.m_pStartNode=pStartNode;
	link_jg_dlg.m_pEndNode=pEndNode;
	//设置默认段号
	//因默认段号应与构件编号一致，故不再根据始末端节点进行段号赋值 wjh-2015.4.15
	//if(pStartNode->iSeg.iSeg>0)
	//	link_jg_dlg.m_sSegNo = pStartNode->iSeg.ToString();
	//else if(pEndNode->iSeg.iSeg>0)
	//	link_jg_dlg.m_sSegNo = pEndNode->iSeg.ToString();
	//else 
	//	link_jg_dlg.m_sSegNo.Empty();
	//是否需要平推判断不准确暂时不自动判断  wht 10-01-14
	//link_jg_dlg.m_bStartHoriOffset=IsNeedHoriOffset(pStartNode);	//始端平推
	//link_jg_dlg.m_bEndHoriOffset=IsNeedHoriOffset(pEndNode);		//终端平推
	f3dLine line(pStartNode->Position(),pEndNode->Position());
	line.pen.style=PS_SOLID;
	line.pen.width=2;
	line.pen.crColor=RGB(255,0,0);
	line.ID=0x01;
	g_pSolidDraw->NewLine(line);	//临时显示角钢单线，方便判断杆件始终端方向 wht 16-10-30
	int nRetCode=link_jg_dlg.DoModal();
	g_pSolidDraw->DelEnt(0x01);		//删除角钢单线
	if(nRetCode!=IDOK)
		return NULL;

	Ta.BeginUndoListen();
	try
	{
		if(link_jg_dlg.m_bGroupJg)
		{
			pNowJg=(CLDSGroupLineAngle*)console.AppendPart(CLS_GROUPLINEANGLE);
			((CLDSGroupLineAngle*)pNowJg)->jg_space=link_jg_dlg.m_fFillThick;
			//link_jg_dlg.group_style里的概念已发生变化，现拆分成wSectStyle及ciMasterIndex两个属性　wjh-2017.1.10
			((CLDSGroupLineAngle*)pNowJg)->wSectStyle=link_jg_dlg.wSectStyle;
			((CLDSGroupLineAngle*)pNowJg)->ciMasterIndex=link_jg_dlg.ciMasterIndex;
			((CLDSGroupLineAngle*)pNowJg)->AppendSonJg();
			CLDSGroupLineAngle *pGroupJg = (CLDSGroupLineAngle*)pNowJg;
			for(int i=0;i<4;i++)
			{
				CLDSLineAngle *pComJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
				if(pComJg)
					pComJg->SetPartNo(link_jg_dlg.sPartNoArr[0][i]);
			}
		}
		else
			pNowJg = (CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		if(link_jg_dlg.m_sOriginPartNo.GetLength()>MAX_CHAR_GUIGE_16)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "part's number input is too long, input failure";
#else
			throw "输入的构件编号太长,输入失败";
#endif
		pNowJg->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
		pNowJg->SetPartNo(link_jg_dlg.m_sOriginPartNo.GetBuffer(0));
		pNowJg->cMaterial = CSteelMatLibrary::RecordAt(link_jg_dlg.m_nJgMaterial).cBriefMark;
		pNowJg->pStart = pStartNode;
		pNowJg->pEnd   = pEndNode;
		//根据始终端节点预拱项属性设置角钢预拱项		wxc-17.1.6 
		if(pStartNode->ArcLiftHandle()>0X20 && pStartNode->ArcLiftHandle()==pEndNode->ArcLiftHandle())
			pNowJg->SetArcLift(pStartNode->ArcLiftHandle());
		//-------Wjh 未完成 --------------
		switch(link_jg_dlg.m_iAngleLocation)
		{
		case 0:
			pNowJg->layer(0) = 'T';
			break;
		case 1:
			pNowJg->layer(0) = 'S';
			break;
		case 2:
			pNowJg->layer(0) = 'L';
		default:
			break;
		}
		switch(link_jg_dlg.m_iAngleGradeType)
		{
		case 0:
			pNowJg->layer(1) = 'Z';
			break;
		case 1:
			pNowJg->layer(1) = 'X';
			break;
		case 2:
			pNowJg->layer(1) = 'H';
			break;
		case 3:
			pNowJg->layer(1) = 'F';
			break;
		default:
			break;
		}
		
		pNowJg->layer(2) = theApp.env.layer[2];
		//设置塔腿杆件配材号 wht 13-04-01
		if(pNowJg->Layer(0)=='L')
		{
			if(pStartNode->Layer(0)=='L')
				pNowJg->cfgword&=pStartNode->UnifiedCfgword();
			if(pEndNode->Layer(0)=='L')
				pNowJg->cfgword&=pEndNode->UnifiedCfgword();
			if(toupper(pStartNode->Layer(0))!='L'&&toupper(pEndNode->Layer(0))!='L')
			{	//始终端节点均为塔身节点时根据激活呼高及激活接腿设置配材号 wht 16-06-18
				CLDSModule *pActiveModule=Ta.GetActiveModule();
				char cPos=CalPoleQuad(pStartNode->Position(),pEndNode->Position());
				if(pActiveModule)
				{
					if(cPos>='1'&&cPos<='4')
						pNowJg->cfgword=CFGLEG((BYTE)pActiveModule->GetBodyNo(),(BYTE)pActiveModule->m_arrActiveQuadLegNo[cPos-'1']).ToCfgWord();
					else
						pNowJg->cfgword.SetBodyLegs(pActiveModule->GetBodyNo(),pActiveModule->m_dwLegCfgWord.GetBodyLegs(1));
				}
			}
		}
		//-------空间布置状态 Wjh 未完成 --------------
		pNowJg->LayStyle = link_jg_dlg.m_iJgStyle+1;
		pNowJg->MirLayStyle=link_jg_dlg.m_iMirLayStyle+1;
		restore_JG_guige(link_jg_dlg.m_sJgGuiGe.GetBuffer(),wing_wide,wing_thick);
		pNowJg->set_wing_wide(wing_wide);
		pNowJg->set_wing_thick(wing_thick);
		//------vvvvvvvvv-------螺栓规格--------------------
		strcpy(pNowJg->connectStart.grade,link_jg_dlg.m_sBoltGrade);
		strcpy(pNowJg->connectEnd.grade,link_jg_dlg.m_sBoltGrade);
		if(link_jg_dlg.m_bConnect1Wing)
			pNowJg->connectStart.m_iConnectWing=pNowJg->connectEnd.m_iConnectWing=1;//单肢连接
		else
			pNowJg->connectStart.m_iConnectWing=pNowJg->connectEnd.m_iConnectWing=0;//双肢连接
		pNowJg->connectStart.d = atoi(link_jg_dlg.m_sStartLsGuiGe);
		pNowJg->connectEnd.d   = atoi(link_jg_dlg.m_sEndLsGuiGe);
		pNowJg->connectStart.rows = 1;//缺省单排螺栓
		pNowJg->connectStart.wnConnBoltN = link_jg_dlg.m_nStartLsNum;
		pNowJg->connectEnd.d = atoi(link_jg_dlg.m_sEndLsGuiGe);
		pNowJg->connectEnd.rows = 1;//缺省单排螺栓
		pNowJg->connectEnd.wnConnBoltN = link_jg_dlg.m_nEndLsNum;
		//------^^^^^^^^^-------螺栓规格--------------------
		pNowJg->SetStart(pNowJg->pStart->ActivePosition());
		pNowJg->SetEnd(pNowJg->pEnd->ActivePosition());
		InitHorizonCrossAnglePosition(pNowJg);
		InitLineAnglePosAndWingNorm(pNowJg, console.GetActiveView()->ucs.axis_z,link_jg_dlg.m_bStartToBer,link_jg_dlg.m_bEndToBer);//计算角钢各法线矢量
		InitHorizonCrossAnglePosition(pNowJg);
		if(link_jg_dlg.m_bStartHoriOffset)	//平推
		{
			pNowJg->desStartPos.ChangeToDatumType(2);
			CLDSLineAngle *pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pNowJg->pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pFatherAngle&&pFatherAngle->desStartPos.IsFaceOffset())
				pNowJg->desStartPos.face_offset_norm=pFatherAngle->desStartPos.face_offset_norm;
			else if(pFatherAngle&&pFatherAngle->desEndPos.IsFaceOffset())
				pNowJg->desStartPos.face_offset_norm=pFatherAngle->desEndPos.face_offset_norm;
		}
		if(link_jg_dlg.m_bEndHoriOffset)	//平推
		{
			pNowJg->desEndPos.ChangeToDatumType(2);
			CLDSLineAngle *pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pNowJg->pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pFatherAngle&&pFatherAngle->desStartPos.IsFaceOffset())
				pNowJg->desEndPos.face_offset_norm=pFatherAngle->desStartPos.face_offset_norm;
			else if(pFatherAngle&&pFatherAngle->desEndPos.IsFaceOffset())
				pNowJg->desEndPos.face_offset_norm=pFatherAngle->desEndPos.face_offset_norm;
		}
		//特殊准距
		JGZJ jgzj;
		getjgzj(jgzj,pNowJg->GetWidth());
		pNowJg->xWingXZhunJu=pNowJg->xWingYZhunJu=jgzj;		//更新角钢X、Y肢准据,角钢宽度发生变化但g未发生变化时无法及时更新g1,g2 wht 16-10-30
		if(jgzj.g!=link_jg_dlg.m_nWingG)
		{
			pNowJg->m_bEnableTeG=TRUE;
			pNowJg->xWingXZhunJu.g = link_jg_dlg.m_nWingG;
			pNowJg->xWingYZhunJu.g = link_jg_dlg.m_nWingG;
		}
		pNowJg->ClearFlag();
		pNowJg->CalPosition();
		f3dPoint len_vec=pNowJg->pEnd->Position()-pNowJg->pStart->Position();
		normalize(len_vec);
		if((pNowJg->layer(0)=='L'||pNowJg->layer(0)=='S')&&pNowJg->layer(1)=='H'&&
		   (fabs(len_vec*f3dPoint(1,0,0))>EPS_COS||fabs(len_vec*f3dPoint(0,1,0))>EPS_COS))	//进一步保证角钢为水平横材 wht 16-10-17
		{	//自动设定塔身横材的水平肢法线为水平面
			//工作肢法线不为(0,0,1)或(0,0,-1)时才能自动设置水平肢法线(否则横隔面内部横材肢法线计算错误) wht 16-10-30
			if(pNowJg->des_norm_x.bByOtherWing&&fabs(pNowJg->vyWingNorm.z)<EPS_COS2)
				pNowJg->set_norm_x_wing(f3dPoint(0,0,pNowJg->vxWingNorm.z>0?1:-1));
			else if(pNowJg->des_norm_y.bByOtherWing&&fabs(pNowJg->vxWingNorm.z)<EPS_COS2)
				pNowJg->set_norm_y_wing(f3dPoint(0,0,pNowJg->vyWingNorm.z>0?1:-1));
		}
		NewLinePart(pNowJg);
		g_pSolidDraw->SetEntSnapStatus(pNowJg->handle);
		
		if(link_jg_dlg.m_bGroupJg)
		{
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgBasicInfo();
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgPos();
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgWingNorm();
		}
		MIRMSG mirmsg=link_jg_dlg.mirmsg;
		if(!MirTaAtom(pNowJg, mirmsg))
		{
			Ta.EndUndoListen();
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the kind of symmetry in the working condition not supported, symmetry failure");
#else
			AfxMessageBox("此工况不能这样的对称，对称失败");
#endif
			return FALSE;
		}
		CSuperSmartPtr<CLDSPart>pMirPart;
		MIRMSG rotmirmsg = mirmsg;
		rotmirmsg.axis_flag=8;
		if(mirmsg.axis_flag&8)	//旋转对称
		{
			pMirPart=pNowJg->GetMirPart(rotmirmsg);
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_jg_dlg.m_sOriginPartNo.GetBuffer(0));
			}
		}
		if(mirmsg.axis_flag&1)	//X轴对称
		{
			pMirPart=pNowJg->GetMirPart(MIRMSG(1));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_jg_dlg.m_sMirXOriginPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_jg_dlg.m_sMirXOriginPartNo.GetBuffer(0));
			
				}
			}
		}
		if(mirmsg.axis_flag&2)	//Y轴对称
		{
			pMirPart=pNowJg->GetMirPart(MIRMSG(2));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_jg_dlg.m_sMirYOriginPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_jg_dlg.m_sMirYOriginPartNo.GetBuffer(0));
				}
			}
		}
		if(mirmsg.axis_flag&4)	//Z轴对称
		{
			pMirPart=pNowJg->GetMirPart(MIRMSG(3));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());	
				pMirPart->SetPartNo(link_jg_dlg.m_sMirDiagOriginPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_jg_dlg.m_sMirDiagOriginPartNo.GetBuffer(0));
				}
			}
		}
	}
	catch(char* sErrorMsg)
	{
		Ta.EndUndoListen();
		if(pNowJg)
			console.DeletePart(pNowJg->handle);
		AfxMessageBox(sErrorMsg);
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	Invalidate(FALSE);
	return pNowJg;
}
//对于单角钢转换为最新摆放方式的对角组合角钢，需添加控制条件
static BOOL IsUseNewGroupStyle(int n,long* id_arr)
{
	if(n<4)
		return FALSE;
	CHashList<long> partHashList;
	for(int i=0;i<n;i++)
	{	
		CLDSPart* pPart = console.FromPartHandle(id_arr[i]);
		if(pPart==NULL)
			return FALSE;
		if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
			return FALSE;
		if(pPart->Layer(0)=='T' || pPart->Layer(1)!='Z')
			return FALSE;
		if(pPart->Layer(2)<'1' || pPart->Layer(2)>'4')	
			return FALSE;
		if(pPart->relativeObjs.GetNodeNum()!=3)
			return FALSE;
		partHashList.SetValue(id_arr[i],id_arr[i]);
	}
	for(int i=0;i<n;i++)
	{
		CLDSPart* pPart = console.FromPartHandle(id_arr[i]);
		for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
		{
			long* phObj=partHashList.GetValue(pMirObj->hObj);
			if(phObj==NULL)
				return FALSE;
		}
	}
	return TRUE;
}
void CLDSView::OnConvertToGroupJg() 
{
	m_nPrevCommandID=ID_CONVERT_TO_GROUP_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat converting into combined angle";
#else
	m_sPrevCommandName="重复转换为组合角钢";
#endif 
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	PARTSET partSet;
	for(int i=0;i<n;i++)
	{
		CLDSLineAngle* pLineAngle = (CLDSLineAngle*)console.FromPartHandle(id_arr[i],CLS_GROUPLINEANGLE,CLS_LINEANGLE);
		if(pLineAngle&&pLineAngle->group_father_jg_h<=0)
			partSet.append(pLineAngle);
	}
	if(partSet.GetNodeNum()<=0)
	{
		AfxMessageBox("未选中待转换角钢！");
		return;
	}
	//宝鸡塔厂反馈说还是原来的对话框更直观方便　wjh-2016.12.17
	CGroupJgStyleDlg group_style_dlg;
	if(group_style_dlg.DoModal()!=IDOK)
		return;
	SmartPartPtr pSmartAngle;
	//判断是否选中角钢原本就是组合角钢，且截面类型未发生变化
	bool hasmodified=false;
	for(pSmartAngle=partSet.GetFirst();pSmartAngle.IsHasPtr()&&!hasmodified;pSmartAngle=partSet.GetNext())
	{
		if(pSmartAngle->GetClassTypeId()==CLS_LINEANGLE)
			hasmodified=true;
		else if(pSmartAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if( pSmartAngle.pGroupAngle->wSectStyle!=group_style_dlg.wSectStyle||
				pSmartAngle.pGroupAngle->ciMasterIndex!=group_style_dlg.ciMasterIndex||
				pSmartAngle.pGroupAngle->jg_space!=group_style_dlg.m_fFillThick)
				hasmodified=true;
		}
	}
	if(!hasmodified)
	{
		AfxMessageBox("选中组合角钢截面类型未发生任何变化!");
	}
	//int group_style=0,layout_style=0,nSpace=12;
	int group_style=group_style_dlg.wSectStyle;
	int layout_style=group_style_dlg.ciMasterIndex;
	int nSpace=ftoi(group_style_dlg.m_fFillThick);
	//
	CUndoOperObject undo(&Ta);
	int nHits=0;
	for(pSmartAngle=partSet.GetFirst();pSmartAngle.IsHasPtr();pSmartAngle=partSet.GetNext())
	{
		CLDSGroupLineAngle *pGroupJg=NULL;
		CSuperSmartPtr<CLDSPart> pPart=pSmartAngle;
		BYTE iWorkWing=0;
		CMultiOffsetPos desStartPos,desEndPos;
		//ciShiftAngleLineAxisToBerX(Y)=true时，搭接于其上的子角钢均应由原来的交于指定心线改为交于虚拟组合角钢的楞线
		char ciShiftAngleLineAxis_BerX=(pPart->GetClassTypeId()==CLS_LINEANGLE);
		char ciShiftAngleLineAxis_BerY=(pPart->GetClassTypeId()==CLS_LINEANGLE);
		if(group_style_dlg.wSectStyle==1)	//T型组合角钢
		{
			if(pSmartAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&pSmartAngle.pGroupAngle->wSectStyle==1)
			{	//原本即为T型组合角钢
				if(pSmartAngle.pGroupAngle->ciMasterIndex==0)
				{
					ciShiftAngleLineAxis_BerX=1;
					ciShiftAngleLineAxis_BerY=2;
				}
				else
				{
					ciShiftAngleLineAxis_BerX=2;
					ciShiftAngleLineAxis_BerY=1;
				}
			}
			else if(group_style_dlg.ciMasterIndex==0)
				ciShiftAngleLineAxis_BerX=false;
			else if(group_style_dlg.ciMasterIndex==1)
				ciShiftAngleLineAxis_BerY=false;
		}
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			//由于以下两原因角钢类型转换时必须分配新句柄 wjh-2012.9.17
			//1.协同时需要重新分配组合角钢的构件句柄否则会在协同中心出现同句柄不同构件类型的错误
			//2.Undo操作中需要通过句柄标识记录历史，如果用同一句柄将会混淆单角钢和组合角钢
			pGroupJg=(CLDSGroupLineAngle*)console.AppendPart(CLS_GROUPLINEANGLE);
			Ta.ReplaceReferenceHandle(CHandleReplacer(pPart->handle,pGroupJg->handle));
			pGroupJg->CopyProperty(pPart);
			pGroupJg->SetStartOdd(pPart.LineAnglePointer()->startOdd());
			pGroupJg->SetEndOdd(pPart.LineAnglePointer()->endOdd());
			pGroupJg->pStart=pPart.LineAnglePointer()->pStart;
			pGroupJg->pEnd=pPart.LineAnglePointer()->pEnd;
			pGroupJg->SetStart(pPart.LineAnglePointer()->pStart->Position(true));
			pGroupJg->SetEnd(pPart.LineAnglePointer()->pEnd->Position(true));
			pGroupJg->set_norm_x_wing(pPart.LineAnglePointer()->get_norm_x_wing(),FALSE);
			pGroupJg->set_norm_y_wing(pPart.LineAnglePointer()->get_norm_y_wing(),FALSE);
			pGroupJg->des_norm_x=pPart.LineAnglePointer()->des_norm_x;
			pGroupJg->des_norm_y=pPart.LineAnglePointer()->des_norm_y;
			desStartPos=pPart.LineAnglePointer()->desStartPos;
			desEndPos  =pPart.LineAnglePointer()->desEndPos;
			iWorkWing  =pPart.LineAnglePointer()->GetStartWorkWing();
			console.DispPartSet.DeleteNode(pPart.LineAnglePointer()->handle);
			console.DeletePart(pPart.LineAnglePointer()->handle);
			console.DispPartSet.append(pGroupJg);
			g_pSolidDraw->DelEnt(pPart->handle);	//从显示引擎中移除原单角钢
		}
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pGroupJg=pPart.pGroupAngle;
		else
			continue;
		pGroupJg->jg_space=nSpace;
		pGroupJg->group_style=group_style;
		if(pGroupJg->group_style==0)
		{
			pGroupJg->size_cSubClassType='D';
			if(layout_style==1)
			{	//通过设定肢法线设计信息，调整对角组合角钢的肢法线方向（不建议直接指定肢法线） wxc-2016.11.1
				f3dPoint x_wing_norm,y_wing_norm,wing_norm;
				x_wing_norm=pGroupJg->get_norm_x_wing();
				y_wing_norm=pGroupJg->get_norm_y_wing();
				DESIGN_JGWING_NORM tem_des_norm=pGroupJg->des_norm_y; 
				pGroupJg->des_norm_y=pGroupJg->des_norm_x;
				pGroupJg->des_norm_x=tem_des_norm;
				if(pGroupJg->Layer(2)=='2' || pGroupJg->Layer(2)=='3')
				{	//直接指定肢法线改变spec_norm.vect值，不能再进行反转操作
					//pGroupJg->set_norm_x_wing(y_wing_norm);
					//pGroupJg->set_norm_y_wing(-x_wing_norm);
					pGroupJg->des_norm_y.near_norm*=-1.0;
					pGroupJg->des_norm_y.spec_norm.ReverseDirection();
				}
				else
				{	//直接指定肢法线改变spec_norm.vect值，不能再进行反转操作
					//pGroupJg->set_norm_x_wing(-y_wing_norm);
					//pGroupJg->set_norm_y_wing( x_wing_norm);
					pGroupJg->des_norm_x.near_norm*=-1.0;
					pGroupJg->des_norm_x.spec_norm.ReverseDirection();
				}
			}
		}
		else if(pGroupJg->group_style==1)
		{
			pGroupJg->size_cSubClassType='T';
			if(layout_style==0)	//X肢外贴
				pGroupJg->ciMasterIndex=0;
			else				//Y肢外贴
				pGroupJg->ciMasterIndex=1;
		}
		else if(pGroupJg->group_style==2)
			pGroupJg->size_cSubClassType='X';
		pGroupJg->AppendSonJg();
		if(pGroupJg->group_style==1)
		{	//T型组合角钢应继承原单角钢的位置搭接关系 wjh-2015.6.29
			pGroupJg->desStartPos=desStartPos;
			pGroupJg->desEndPos=desEndPos;
		}
		//将楞点定位方式设置为参数化定位，简单定位智能判断定位信息不便于外部参数设定
		pGroupJg->desStartPos.jgber_cal_style=0;
		pGroupJg->desEndPos.jgber_cal_style=0;
		if(pGroupJg->group_style!=1||(pGroupJg->group_style==1 && pGroupJg->ciMasterIndex!=iWorkWing))
		{	//非T型组合角钢或T型组合角钢外贴肢非工作肢时，需清空肢法线偏移信息   wxc-2016.10.21
			pGroupJg->desStartPos.wing_x_offset.gStyle=pGroupJg->desStartPos.wing_y_offset.gStyle=4;
			pGroupJg->desStartPos.wing_x_offset.offsetDist=pGroupJg->desStartPos.wing_y_offset.offsetDist=0;
			pGroupJg->desEndPos.wing_x_offset.gStyle=pGroupJg->desEndPos.wing_y_offset.gStyle=4;
			pGroupJg->desEndPos.wing_x_offset.offsetDist=pGroupJg->desEndPos.wing_y_offset.offsetDist=0;
		}
		pGroupJg->CalPosition();
		//更新子构件定位信息
		pGroupJg->UpdateSonJgBasicInfo();
		pGroupJg->UpdateSonJgOddment();
		pGroupJg->UpdateSonJgWingNorm();
		pGroupJg->UpdateSonJgPos();
		if(pGroupJg->pSolidBody)
			pGroupJg->pSolidBody->Empty();
		//更新显示引擎中组合角钢及子角钢的图形显示信息
		g_pSolidDraw->NewSolidPart(pGroupJg->GetSolidPartObject());
		for (int j = 0; j < 4; j++)
		{
			CLDSLineAngle* pSonAngle = NULL;
			if(pGroupJg->son_jg_h[j]>0)
				pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupJg->son_jg_h[j], CLS_LINEANGLE);
			if (pSonAngle)
			{
				pSonAngle->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
			}
		}
		pGroupJg->SetModified();
		//更新搭接于其上的其余射线角钢
		if(ciShiftAngleLineAxis_BerX+ciShiftAngleLineAxis_BerY>0)
		{
			SCOPE_STRU scope;
			scope.VerifyVertex(pGroupJg->pStart->xOriginalPos);
			scope.VerifyVertex(pGroupJg->pEnd->xOriginalPos);
			CLDSNode* pNode;
			CHashSet<CLDSNode*> hashResideNodes;
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(!pGroupJg->ModuleTogetherWith(pNode))
					continue;
				if(!scope.IsIncludePoint(pNode->xOriginalPos))
					continue;
				if(pNode->hFatherPart==pGroupJg->handle)
					hashResideNodes.SetValue(pNode->handle,pNode);
			}
			SmartRodPtr pSmartRayAngle;
			for(pSmartRayAngle=Ta.EnumRodFirst();pSmartRayAngle.IsHasPtr();pSmartRayAngle=Ta.EnumRodNext())
			{
				if(!pSmartRayAngle->IsAngle())
					continue;
				long hStartDatumAngle=pSmartRayAngle.pAngle->desStartPos.datum_jg_h;
				long hEndDatumAngle=pSmartRayAngle.pAngle->desEndPos.datum_jg_h;
				if(hStartDatumAngle==0&&pSmartRayAngle->pStart)
					hStartDatumAngle=pSmartRayAngle->pStart->hFatherPart;
				if(hEndDatumAngle==0&&pSmartRayAngle->pEnd)
					hEndDatumAngle=pSmartRayAngle->pEnd->hFatherPart;
				BYTE ciOldStartSpatialStyle=pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle;
				BYTE ciOldEndSpatialStyle=pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle;
				if( hStartDatumAngle==pGroupJg->handle)
				{
					if(ciShiftAngleLineAxis_BerX==1&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgWingX()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle-=1;	//X肢心线变为楞线
					else if(ciShiftAngleLineAxis_BerY==1&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgWingY()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle-=2;	//Y肢心线变为楞线
					else if(ciShiftAngleLineAxis_BerX==2&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle+=1;	//楞线变为X肢心线
					else if(ciShiftAngleLineAxis_BerY==2&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle+=2;	//楞线变为Y肢心线
				}
				if(hEndDatumAngle==pGroupJg->handle)
				{
					if(ciShiftAngleLineAxis_BerX==1&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgWingX()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle-=1;	//X肢心线变为楞线
					else if(ciShiftAngleLineAxis_BerY==1&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgWingY()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle-=2;	//Y肢心线变为楞线
					else if(ciShiftAngleLineAxis_BerX==2&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle+=1;	//楞线变为X肢心线
					else if(ciShiftAngleLineAxis_BerY==2&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle+=2;	//楞线变为Y肢心线
				}
				if( ciOldStartSpatialStyle!=pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle||
					ciOldEndSpatialStyle!=pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle)
					pSmartRayAngle->CalPosition(true);
			}
		}
		nHits++;
	}
	Invalidate();
	UpdatePropertyPage();	//必须刷新左侧属性栏，否则转换为钢管后selectObjs中的对象已被清除
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("succeeded in converting %d angles into combined angle!",nHits);
#else
	ss.Format("已成功转换%d根角钢为组合角钢!",nHits);
#endif
	MessageBox(ss);
}

void CLDSView::OnConvertToSingleJg() 
{
	m_nPrevCommandID=ID_CONVERT_TO_SINGLE_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat converting into single angle";
#else
	m_sPrevCommandName="重复转换为单角钢";
#endif
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	int nHits=0,nTubes=0;
	CUndoOperObject undo(&Ta);
	for(int i=0;i<n;i++)
	{
		CLDSLineAngle *pJg=NULL;
		CLDSLineTube *pTube=NULL;
		CLDSGroupLineAngle* pGroupJg = (CLDSGroupLineAngle*)console.FromPartHandle(id_arr[i],CLS_GROUPLINEANGLE);
		if(pGroupJg)
		{
			//由于以下两原因角钢类型转换时必须分配新句柄 wjh-2012.9.17
			//1.协同时需要重新分配组合角钢的构件句柄否则会在协同中心出现同句柄不同构件类型的错误
			//2.Undo操作中需要通过句柄标识记录历史，如果用同一句柄将会混淆单角钢和组合角钢
			pJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			Ta.ReplaceReferenceHandle(CHandleReplacer(pGroupJg->handle,pJg->handle));
			pJg->CopyProperty((CLDSLineAngle*)pGroupJg);
			pJg->pStart=pGroupJg->pStart;
			pJg->pEnd=pGroupJg->pEnd;
			pJg->set_norm_x_wing(pGroupJg->get_norm_x_wing());
			pJg->set_norm_y_wing(pGroupJg->get_norm_y_wing());
			pJg->des_norm_x=pGroupJg->des_norm_x;
			pJg->des_norm_y=pGroupJg->des_norm_y;
			pJg->desStartPos=pGroupJg->desStartPos;
			pJg->desEndPos=pGroupJg->desEndPos;
			pJg->SetStart(pGroupJg->Start());
			pJg->SetEnd(pGroupJg->End());
			pJg->cal_x_wing_norm();
			pJg->cal_y_wing_norm();
			nHits++;
			for(int j=0;j<4;j++)
			{
				console.DispPartSet.DeleteNode(pGroupJg->son_jg_h[j]);
				console.DeletePart(pGroupJg->son_jg_h[j]);
			}
			//单机作业也需要重新分配角钢句柄 wht 09-10-13
			//if(theApp.m_bCooperativeWork)	
			{	//协同时需要重新分配角钢句柄
				g_pSolidDraw->DelEnt(pGroupJg->handle);
				console.DispPartSet.DeleteNode(pGroupJg->handle);
				console.DeletePart(pGroupJg->handle);
				console.DispPartSet.append(pJg);
				g_pSolidDraw->NewSolidPart(pJg->GetSolidPartObject());
			}
		}
		else if((pTube=(CLDSLineTube*)console.FromPartHandle(id_arr[i],CLS_LINETUBE))!=NULL)
		{
			if(ConvertToAngle(pTube)!=NULL)
				nTubes++;
		}
	}
	UpdatePropertyPage();	//必须刷新左侧属性栏，否则转换为钢管后selectObjs中的对象已被清除
	CString ss;
	if(nTubes==0)
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("succeeded in converting %d combined angles into single angle!",nHits);
	else if(nHits==0)
		ss.Format("succeeded in converting %d tube into single angle!",nTubes);
	else
		ss.Format("succeeded in converting %d combined angles into single angle!",nHits,nTubes);
#else
		ss.Format("已成功转换%d根组合角钢为单角钢!",nHits);
	else if(nHits==0)
		ss.Format("已成功转换%d根钢管为单角钢!",nTubes);
	else
		ss.Format("已成功转换%d根组合角钢及%d根钢管为单角钢!",nHits,nTubes);
#endif
	MessageBox(ss);
}

#else
void CLDSView::OnConvertToGroupJg(){;}
void CLDSView::OnConvertToSingleJg(){;}
#endif
void CLDSView::OnConvertToTube() 
{
#ifdef __COMPLEX_PART_INC_
	m_nPrevCommandID=ID_CONVERT_TO_TUBE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat converting into tube";
#else
	m_sPrevCommandName="重复转换为钢管";
#endif
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	int nHits=0;
	CUndoOperObject undo(&Ta,false);
	for(int i=0;i<n;i++)
	{
		CLDSLineTube *pTube=NULL;
		CSuperSmartPtr<CLDSPart> pPart = console.FromPartHandle(id_arr[i]);
		if(pPart.IsNULL()||!pPart->IsLinePart())
			continue;
		if(pPart.LinePartPointer()->pStart==NULL||pPart.LinePartPointer()->pEnd==NULL)
			continue;
		if(pPart->GetClassTypeId()!=CLS_LINETUBE)
		{
			//由于以下两原因角钢类型转换时必须分配新句柄 wjh-2012.9.17
			//1.协同时需要重新分配组合角钢的构件句柄否则会在协同中心出现同句柄不同构件类型的错误
			//2.Undo操作中需要通过句柄标识记录历史，如果用同一句柄将会混淆单角钢和组合角钢
			pTube=(CLDSLineTube*)console.AppendPart(CLS_LINETUBE);
			Ta.ReplaceReferenceHandle(CHandleReplacer(pPart->handle,pTube->handle));
			pTube->CopyProperty(pPart);
			pTube->pStart=pPart.LinePartPointer()->pStart;
			pTube->pEnd=pPart.LinePartPointer()->pEnd;
			pTube->SetStartOdd(pPart.LinePartPointer()->startOdd());
			pTube->SetEndOdd(pPart.LinePartPointer()->endOdd());
			pTube->SetStart(pTube->pStart->Position(true));
			pTube->SetEnd(pTube->pEnd->Position(true));
			if(pPart.LinePartPointer()->pStart->hFatherPart==pPart->handle)
				pTube->desStartPos.endPosType=0;
			else
				pTube->desStartPos.endPosType=1;
			if(pPart.LinePartPointer()->pEnd->hFatherPart==pPart->handle)
				pTube->desEndPos.endPosType=0;
			else
				pTube->desEndPos.endPosType=1;
			console.DispPartSet.DeleteNode(pPart->handle);
			console.DeletePart(pPart->handle);
			pTube->CalPosition();
			pTube->SetModified();
			nHits++;
		}
	}
	Invalidate();
	UpdatePropertyPage();	//必须刷新左侧属性栏，否则转换为钢管后selectObjs中的对象已被清除
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("succeeded in converting %d rods into tube!",nHits);
#else
	ss.Format("已成功转换%d根杆件为钢管!",nHits);
#endif
	MessageBox(ss);
#endif
}
bool CorrectSingleAngleLayout(CLDSLineAngle* pJg,bool updatePartSolid=true);	//defined in LDSView.cpp
CLDSLineAngle* CLDSView::ConvertToAngle(CLDSLineTube *pTube) 
{
#ifdef __COMPLEX_PART_INC_
	if(pTube==NULL||pTube->pStart==NULL||pTube->pEnd==NULL)
		return NULL;
	//由于以下两原因角钢类型转换时必须分配新句柄 wjh-2012.9.17
	//1.协同时需要重新分配组合角钢的构件句柄否则会在协同中心出现同句柄不同构件类型的错误
	//2.Undo操作中需要通过句柄标识记录历史，如果用同一句柄将会混淆单角钢和组合角钢
	CLDSLineAngle* pAngle=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
	Ta.ReplaceReferenceHandle(CHandleReplacer(pTube->handle,pAngle->handle));
	pAngle->CopyProperty(pTube);
	pAngle->pStart=pTube->pStart;
	pAngle->pEnd=pTube->pEnd;
	pAngle->SetStartOdd(pTube->startOdd());
	pAngle->SetEndOdd(pTube->endOdd());
	pAngle->SetStart(pAngle->pStart->Position(true));
	pAngle->SetEnd(pAngle->pEnd->Position(true));
	//
	pAngle->desStartPos.spatialOperationStyle=pAngle->desEndPos.spatialOperationStyle=18;
	CorrectSingleAngleLayout(pAngle);
	console.DispPartSet.DeleteNode(pTube->handle);
	console.DeletePart(pTube->handle);
	pAngle->CalPosition();
	UpdatePropertyPage();	//必须刷新左侧属性栏，否则转换为钢管后selectObjs中的对象已被清除
	return pAngle;
#else
	return NULL;
#endif
}
