#include "stdafx.h"
#include "..\LDS.h"
#include "..\LDSDoc.h"
#include "..\LDSView.h"
#include "..\dlg.h"
#include "..\env_def.h"
#include "..\GlobalFunc.h"
#include "..\PromptDlg.h"
#include "..\MirTaAtom.h"
#include "..\GroupJgStyleDlg.h"
#include "Tower.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __ALFA_TEST_VERSION_

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL Test_InitLineAnglePosAndWingNorm(CLDSLineAngle* pCurJg,f3dPoint view_norm,BOOL bStartToBer/*=FALSE*/,BOOL bEndToBer/*=FALSE*/)
{
	CLDSPart *pPart;
	CString sErrorMsg;
	f3dPoint vec;
	if(pCurJg->pOldDesStartPos==NULL||pCurJg->pOldDesEndPos==NULL)
	{
		logto.Log("error");
		return FALSE;
	}
	try{
		//计算当前角钢的延伸方向，自始端指向终端为正
		Sub_Pnt(vec,pCurJg->pEnd->Position(false),pCurJg->pStart->Position(true));
		if(vec.mod()<EPS)
			throw "当前角钢始终端点重合,数据错误!";
		normalize(vec);
		normalize(view_norm);
		if(fabs(vec*view_norm)>EPS_COS)
			throw "视向指定错误!";
		switch(pCurJg->lstyle)
		{
		case 1:
		case 3:
			if(pCurJg->lstyle==1)	//外铁
				pCurJg->des_norm_x.near_norm =-view_norm;	//与肢法线接近的基准法线
			else					//里铁
				pCurJg->des_norm_x.near_norm = view_norm;	//与肢法线接近的基准法线
			pCurJg->des_norm_x.bByOtherWing = FALSE;
			pCurJg->des_norm_y.bByOtherWing = TRUE;		//Y肢法线以X肢法线为基准
			if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesStartPos->offset_X_dist_style = 4;	//角钢X肢紧贴父角钢
				pCurJg->pOldDesStartPos->offset_X_dist = 0;
				pCurJg->pOldDesStartPos->offset_Y_dist_style = 0;	//角钢Y肢偏移一个准距
				//pCurJg->pOldDesStartPos->offset_Y_dist = 0;
			}
			if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesEndPos->offset_X_dist_style = 4;	//角钢X肢紧贴父角钢
				pCurJg->pOldDesEndPos->offset_X_dist = 0;
				pCurJg->pOldDesEndPos->offset_Y_dist_style = 0;	//角钢Y肢偏移一个准距
				//pCurJg->pOldDesEndPos->offset_Y_dist = 0;
			}
			break;
		case 2:
		case 4:
		default:
			if(pCurJg->lstyle==2)	//里铁
				pCurJg->des_norm_y.near_norm = view_norm;	//与肢法线接近的基准法线
			else					//外铁
				pCurJg->des_norm_y.near_norm =-view_norm;	//与肢法线接近的基准法线
			pCurJg->des_norm_y.bByOtherWing = FALSE;
			pCurJg->des_norm_x.bByOtherWing = TRUE;		//X肢法线以Y肢法线为基准
			if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesStartPos->offset_X_dist_style = 0;	//角钢X肢偏移一个准距
				//pCurJg->pOldDesStartPos->offset_X_dist = 0;
				pCurJg->pOldDesStartPos->offset_Y_dist_style = 4;	//角钢Y肢紧贴父角钢
				pCurJg->pOldDesStartPos->offset_Y_dist = 0;
			}
			if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesEndPos->offset_X_dist_style = 0;	//角钢X肢偏移一个准距
				//pCurJg->pOldDesEndPos->offset_X_dist = 0;
				pCurJg->pOldDesEndPos->offset_Y_dist_style = 4;	//角钢Y肢紧贴父角钢
				pCurJg->pOldDesEndPos->offset_Y_dist = 0;
			}
			break;
		}
		
		
		CSuperSmartPtr<CLDSLinePart> pStartFatherPart,pEndFatherPart;
		pPart = Ta.Parts.FromHandle(pCurJg->pStart->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pStartFatherPart = (CLDSLinePart*)pPart;
		pPart = Ta.Parts.FromHandle(pCurJg->pEnd->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pEndFatherPart = (CLDSLinePart*)pPart;
		short start_attach_type=pCurJg->pStart->m_cPosCalType;//当前角钢始端节点依附关系
		short end_attach_type=pCurJg->pEnd->m_cPosCalType;	//当前角钢终端节点依附关系
		//	****0.0特殊情况主要处理当前角钢为横隔面交叉材的情况*****
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
		//	****1.0 以下两种为当前角钢有一端节点为派生节点的情况****
		/*
		当前角钢端节点的父角钢已存在(节点为其父角钢的派生节点,角钢的
		派生节点一般位于角钢的中间不应位于角钢的端点,否则认为是数据错误，
		，此时抛出错误信息),一般来说,它们(派生节点)的父角钢不应与当前角
		钢共线,如果这样,则认为是数据错误，此时抛出错误信息。
		*/
		if(start_attach_type>3)
		{	//始端点为派生节点
			if(!pStartFatherPart.IsHasPtr())
			{
				char sError[MAX_TEMP_CHAR_200+1]="";
				_snprintf(sError,MAX_TEMP_CHAR_200,"节点0X%X的的父杆件丢失,肢法线方向计算失败!",pCurJg->pStart->handle);
				throw sError;
			}
			if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
				pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
			else								//X肢法线以Y肢法线为计算基准
				pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
		}
		else if(end_attach_type>3)
		{	//终端点为派生节点
			if(!pEndFatherPart.IsHasPtr())
			{
				char sError[MAX_TEMP_CHAR_200+1]="";
				_snprintf(sError,MAX_TEMP_CHAR_200,"节点0X%X的父杆件丢失,肢法线方向计算失败!",pCurJg->pEnd->handle);
				throw sError;
			}
			if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
				pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
			else								//X肢法线以Y肢法线为计算基准
				pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
		}
		//	****2.0 以下为当前角钢的端节点均为基本节点的情况****
		else
		{
			if( pStartFatherPart.IsHasPtr()
				&&pStartFatherPart->pStart!=pCurJg->pEnd
				&&pStartFatherPart->pEnd!=pCurJg->pEnd
				&&pStartFatherPart->cfgword.And(pCurJg->cfgword))//始端节点父角钢不在当前模型 wht 10-06-06
			{	//始端点父角钢与当前角钢不相同
				if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
					pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
				else								//X肢法线以Y肢法线为计算基准
					pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
			}
			else if(pEndFatherPart.IsHasPtr()
				&&pEndFatherPart->pStart!=pCurJg->pStart
				&&pEndFatherPart->pEnd!=pCurJg->pStart
				&&pEndFatherPart->cfgword.And(pCurJg->cfgword))	//终端节点父角钢不在当前模型 wht 10-06-06
			{	//终端点父角钢与当前角钢不相同
				if(pCurJg->des_norm_y.bByOtherWing)	//Y肢法线以X肢法线为计算基准
					pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
				else								//X肢法线以Y肢法线为计算基准
					pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
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
		pCurJg->cal_x_wing_norm();
		pCurJg->cal_y_wing_norm();
		if(!pCurJg->des_norm_x.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)	//单角钢
			{
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesStartPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesStartPos->datum_pos_style = 3;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)	//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesStartPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesStartPos->datum_pos_style = 1;	//搭在Y肢上时实际应交于楞线
				}
				else
					pCurJg->pOldDesStartPos->datum_pos_style=1;	//交于楞线
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesStartPos->datum_pos_style = 1;	//节点向轴线投影
			else
				pCurJg->pOldDesStartPos->datum_pos_style=0;	//节点即基点
			if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesEndPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesEndPos->datum_pos_style = 3;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)	//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesEndPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesEndPos->datum_pos_style = 1;
				}
				else
					pCurJg->pOldDesEndPos->datum_pos_style=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesEndPos->datum_pos_style=1;	//节点向轴线投影
			else
				pCurJg->pOldDesEndPos->datum_pos_style=0;	//节点即基点
		}
		else //if(pCurJg->des_norm_y.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesStartPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesStartPos->datum_pos_style = 3;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)		//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesStartPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesStartPos->datum_pos_style = 1;
				}
				else
					pCurJg->pOldDesStartPos->datum_pos_style=1;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesStartPos->datum_pos_style = 1;	//节点向轴线投影
			else
				pCurJg->pOldDesStartPos->datum_pos_style=0;
			if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesEndPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesEndPos->datum_pos_style = 3;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)	//T型组合角钢
				{
					if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesEndPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesEndPos->datum_pos_style = 1;
				}
				else
					pCurJg->pOldDesEndPos->datum_pos_style=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesEndPos->datum_pos_style = 1;	//节点向轴线投影
			else
				pCurJg->pOldDesEndPos->datum_pos_style=0;
		}
		if(bStartToBer&&pCurJg!=pStartFatherPart)
			pCurJg->pOldDesStartPos->datum_pos_style=1;
		if(bEndToBer&&pCurJg!=pEndFatherPart)
			pCurJg->pOldDesEndPos->datum_pos_style=1;
		pCurJg->CalPosition();
	}
	catch(char *sError)
	{
		CString ss;
		ss.Format("计算角钢肢法线错误，%s",sError);
		AfxMessageBox(ss);
	}
	return TRUE;
}

#endif
#endif