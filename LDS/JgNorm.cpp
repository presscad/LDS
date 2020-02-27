//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "dlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CLDSView::OnCal1jgNorm() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_CAL_1JG_NORM;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat counting angle's leg direction";
#else 
	m_sPrevCommandName="重复计算角钢肢方向";
#endif
	m_curTask = TASK_CAL_1JG_NORM;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_LINE);
	//切换到单线显示状态
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please choose to compute leg direction of angle firstly,then select another reference angle!");
#else 
	g_pPromptMsg->SetMsg("请首先选择要计算肢方向的角钢,然后再选择一根共工作面参照角钢!");
#endif
#endif
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
static CConfirmJgStyleDlg confirm_jgstyle_dlg;
BOOL CalLineAngleWingNorm(CLDSLineAngle* pCurJg,CLDSLinePart *pByLinePart,f3dPoint view_norm,BOOL bUpdateOtherWing,BOOL bSpecStyle/*=TRUE*/,BOOL bPromptError/*=TRUE*/)
{
	f3dPoint vec,aid_vec,norm,direct,view_vec,vertical_view;
	//以下代码为以前的角钢肢法线计算代码，一直不稳定，现换为一函数CalFaceNorm，代码暂留 WJH-2004/07/01
	/*Sub_Pnt(vec,pCurJg->End(),pCurJg->Start());
	if(vec.mod()<EPS)
		throw "当前角钢始终端点重合,数据错误!";

	//Sub_Pnt(aid_vec,pByJg->pEnd->pos,pByJg->pStart->pos);
	//为了处理酒杯塔内内曲臂将上行改为下面语句 WJH--01.07.28
	Sub_Pnt(aid_vec,pByJg->End(),pByJg->Start());
	if(aid_vec.mod()<EPS)
		throw CString("基准角钢始终端节点重合,数据错误!");
	//叉积求出角钢工作面上肢的的法矢
	normalize(vec);
	normalize(aid_vec);
	norm = cross_prod(aid_vec,vec);
	normalize(view_norm);
	normalize(norm);
	if(norm.mod()<EPS)
	{
		aid_vec = pByJg->End()-pCurJg->End();
		normalize(aid_vec);
		norm = cross_prod(aid_vec,vec);
		normalize(view_norm);
		normalize(norm);
		if(norm.mod()<EPS)
			throw CString("当前角钢与基准角钢重合,数据错误!");
	}*/
	try{
		norm=CalFaceNorm(pCurJg->Start(),pCurJg->End(),pByLinePart->Start(),pByLinePart->End());
		if(norm.mod()<EPS)
		{
			if(bPromptError)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The current angle and datum angle are coincidence,data is error!");
#else 
				AfxMessageBox("当前角钢与基准角钢重合,数据错误!");
#endif
			else
				return FALSE;
		}
		
		if(bSpecStyle)
		{
			if(confirm_jgstyle_dlg.DoModal()!=IDOK)
				return FALSE;
		}
		else
			confirm_jgstyle_dlg.m_nJgStyle=pCurJg->LayStyle-1;
		pCurJg->ClearFlag();
		//计算非工作面肢的法矢
		switch(confirm_jgstyle_dlg.m_nJgStyle+1)
		{
		case 1:
		case 3:
			pCurJg->des_norm_x.hViceJg = pByLinePart->handle;
			if(confirm_jgstyle_dlg.m_nJgStyle+1==1)
			{	//外铁
				pCurJg->des_norm_x.near_norm = -view_norm;	//与肢法线接近的基准法线
				if(norm*view_norm<0)
					pCurJg->set_norm_x_wing(norm,pCurJg->des_norm_x.bSpecific);
				else
					pCurJg->set_norm_x_wing(-norm,pCurJg->des_norm_x.bSpecific);
			}
			else
			{	//里铁
				pCurJg->des_norm_x.near_norm = view_norm;	//与肢法线接近的基准法线
				if(norm*view_norm>0)
					pCurJg->set_norm_x_wing(norm,pCurJg->des_norm_x.bSpecific);
				else
					pCurJg->set_norm_x_wing(-norm,pCurJg->des_norm_x.bSpecific);
			}
			pCurJg->des_norm_x.bSpecific = FALSE;
			pCurJg->des_norm_x.bByOtherWing = FALSE;
			if(bUpdateOtherWing)	//另一肢与当前工作肢一致处理
			{
				pCurJg->des_norm_y.bByOtherWing = TRUE;
				pCurJg->des_norm_y.bSpecific = FALSE;
			}
			else
			{	//计算开合角时非工作肢法线方向不能随另一肢变化 wht 09-08-28
				pCurJg->des_norm_y.bByOtherWing=FALSE;
				pCurJg->des_norm_y.bSpecific=TRUE;
			}
			if(pCurJg->des_norm_y.bByOtherWing)		//Y肢法线以X肢法线为基准
				pCurJg->cal_y_wing_norm();
			break;
		case 2:
		case 4:
		default:
			pCurJg->des_norm_y.hViceJg = pByLinePart->handle;
			if(confirm_jgstyle_dlg.m_nJgStyle+1==4)
			{	//外铁
				pCurJg->des_norm_y.near_norm = -view_norm;	//与肢法线接近的基准法线
				if(norm*view_norm<0)
					pCurJg->set_norm_y_wing(norm);
				else
					pCurJg->set_norm_y_wing(-norm);
			}
			else
			{	//里铁
				pCurJg->des_norm_y.near_norm = view_norm;	//与肢法线接近的基准法线
				if(norm*view_norm>0)
					pCurJg->set_norm_y_wing(norm);
				else
					pCurJg->set_norm_y_wing(-norm);
			}
			if(norm*view_norm>0)
				pCurJg->set_norm_y_wing(norm);
			else
				pCurJg->set_norm_y_wing(-norm);
			pCurJg->des_norm_y.bSpecific = FALSE;
			pCurJg->des_norm_y.bByOtherWing = FALSE;
			if(bUpdateOtherWing)	//另一肢与当前工作肢一致处理
			{
				pCurJg->des_norm_x.bByOtherWing = TRUE;
				pCurJg->des_norm_x.bSpecific = FALSE;
			}
			else
			{	//计算开合角时非工作肢法线方向不能随另一肢变化	wht 09-08-28
				pCurJg->des_norm_x.bSpecific=TRUE;
				pCurJg->des_norm_x.bByOtherWing=FALSE;
			}
			if(pCurJg->des_norm_x.bByOtherWing)		//X肢法线以Y肢法线为基准
				pCurJg->cal_x_wing_norm();
			break;
		}
		pCurJg->LayStyle=confirm_jgstyle_dlg.m_nJgStyle+1;
	}
	catch(char* sError)
	{
		if(bPromptError)
			AfxMessageBox(sError);
		else
			return FALSE;
	}
	return TRUE;
}

//对塔身斜材计算缺省值，对于其它情况请求用户输入
int CalJgPosStyle(CLDSNode *pStart,CLDSNode *pEnd,const char *layer,int lstyle)
{
	int iJgStyle=lstyle;
	//塔身斜材一般为撇外捺里
	if(layer[0] == 'S' &&layer[1]=='X')
	{
		switch(layer[2])
		{
		case 'Q':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//角钢自上至下
				if(pEnd->Position(false).x<pStart->Position(false).x)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//角钢自下至上
				if(pEnd->Position(false).x>pStart->Position(false).x)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			break;
		case 'H':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//角钢自上至下
				if(pEnd->Position(true).x>pStart->Position(false).x)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//角钢自下至上
				if(pEnd->Position(false).x<pStart->Position(false).x)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			break;
		case 'Z':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//角钢自上至下
				if(pEnd->Position(false).y<pStart->Position(false).y)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//角钢自下至上
				if(pEnd->Position(false).y>pStart->Position(false).y)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			break;
		case 'Y':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//角钢自上至下
				if(pEnd->Position(false).y>pStart->Position(false).y)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//角钢自下至上
				if(pEnd->Position(false).y<pStart->Position(false).y)
					iJgStyle=0;	//撇角钢
				else
					iJgStyle=2;	//捺角钢
			}
			break;
		case 'T':
			break;
		default:
			break;
		}
	}
	return iJgStyle;
}

BOOL CLDSView::FinishCal1JgNorm(CLDSDbObject *pAtom1,
						CLDSDbObject *pAtom2)
{
	CLDSLineAngle *pNowJg=NULL;
	CLDSLinePart *pByLinePart=NULL;
	if(pAtom1->GetClassTypeId()!=CLS_LINEANGLE&&pAtom1->GetClassTypeId()!=CLS_GROUPLINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Choose the wrong part!");
#else 
		AfxMessageBox("选择了错误的塔构件!");
#endif
		return FALSE;
	}
	if(pAtom2&&pAtom2->IsPart())
	{	//确保pAtom2为杆件
		CLDSPart *pPart=(CLDSPart*)pAtom2;
		if(!pPart->IsLinePart())
			return FALSE;
	}
	else
		return FALSE;
	f3dPoint view_norm;
	pNowJg = (CLDSLineAngle*)pAtom1;
	pByLinePart = (CLDSLinePart*)pAtom2;
	//------VVVVVV--------------角钢肢重新摆放-----------
	Ta.BeginUndoListen();
	try{
		BOOL bKaiHeAngle;
#ifdef AFX_TARG_ENU_ENGLISH
		if(MessageBox("Whether to need to process open and merge angle?","",MB_YESNO|MB_DEFBUTTON2)==IDYES)
#else 
		if(MessageBox("是否需要开合角处理?","",MB_YESNO|MB_DEFBUTTON2)==IDYES)
#endif
			bKaiHeAngle=FALSE;
		else
			bKaiHeAngle=TRUE;
		CalLineAngleWingNorm(pNowJg,pByLinePart,console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
		
		if(pNowJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgWingNorm();
		pNowJg->SetModified();
		//切换到实体显示状态
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
		if(!bKaiHeAngle)
		{	//计算对称角钢的开合角
			static CMirMsgDlg mir_dlg;
			if(mir_dlg.DoModal()==IDOK)
			{
				CLDSLineAngle *cur_jg[4]={NULL};
				CLDSLinePart *line_part[4]={NULL};
				cur_jg[0]=pNowJg;
				line_part[0]=pByLinePart;
				if(mir_dlg.mirmsg.axis_flag&1)
				{
					cur_jg[1]=(CLDSLineAngle*)pNowJg->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					line_part[1]=(CLDSLinePart*)pByLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					if(cur_jg[1]&&line_part[1])
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(cur_jg[1]->handle,TRUE);
						g_pSolidDraw->SetEntSnapStatus(line_part[1]->handle,TRUE);
						CalLineAngleWingNorm(cur_jg[1],line_part[1],console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
						if(cur_jg[1]->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)cur_jg[1])->UpdateSonJgWingNorm();
						cur_jg[1]->SetModified();
					}
				}
				if(mir_dlg.mirmsg.axis_flag&2)
				{
					cur_jg[2]=(CLDSLineAngle*)pNowJg->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					line_part[2]=(CLDSLinePart*)pByLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					if(cur_jg[2]&&line_part[2])
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(cur_jg[2]->handle,TRUE);
						g_pSolidDraw->SetEntSnapStatus(line_part[2]->handle,TRUE);
						CalLineAngleWingNorm(cur_jg[2],line_part[2],console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
						if(cur_jg[2]->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)cur_jg[2])->UpdateSonJgWingNorm();
						cur_jg[2]->SetModified();
					}
				}
				if(mir_dlg.mirmsg.axis_flag&4)
				{
					cur_jg[3]=(CLDSLineAngle*)pNowJg->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					line_part[3]=(CLDSLinePart*)pByLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					if(cur_jg[3]&&line_part[3])
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(cur_jg[3]->handle,TRUE);
						g_pSolidDraw->SetEntSnapStatus(line_part[3]->handle,TRUE);
						CalLineAngleWingNorm(cur_jg[3],line_part[3],console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
						if(cur_jg[3]->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)cur_jg[3])->UpdateSonJgWingNorm();
						cur_jg[3]->SetModified();
					}
				}
				if(mir_dlg.mirmsg.axis_flag&8)
				{
					mir_dlg.mirmsg.axis_flag=8;
					for(int i=0;i<4;i++)
					{
						if(cur_jg[i]&&line_part[i])
						{
							CLDSLineAngle *pMirAngle=NULL;
							CLDSLinePart *pMirLinePart=NULL;
							for(int j=0;j<mir_dlg.mirmsg.array_num;j++)
							{
								if(j==0)
								{
									pMirAngle=(CLDSLineAngle*)cur_jg[i]->GetMirPart(mir_dlg.mirmsg);
									pMirLinePart=(CLDSLinePart*)line_part[i]->GetMirPart(mir_dlg.mirmsg);
								}
								else if(pMirAngle&&pMirLinePart)
								{
									pMirAngle=(CLDSLineAngle*)pMirAngle->GetMirPart(mir_dlg.mirmsg);
									pMirLinePart=(CLDSLinePart*)pMirLinePart->GetMirPart(mir_dlg.mirmsg);
								}
								if(pMirAngle&&pMirLinePart)
								{
									g_pSolidDraw->ReleaseSnapStatus();
									g_pSolidDraw->SetEntSnapStatus(pMirAngle->handle,TRUE);
									g_pSolidDraw->SetEntSnapStatus(pMirLinePart->handle,TRUE);
									CalLineAngleWingNorm(pMirAngle,pMirLinePart,console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
									if(pMirAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
										((CLDSGroupLineAngle*)pMirAngle)->UpdateSonJgWingNorm();
									pMirAngle->SetModified();
								}
								else
									break;
							}
						}
					}
				}
			}
		}
		Ta.EndUndoListen();
	}
	catch(char* sError)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sError);
		return FALSE;
	}
	
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("Normal direction calculated successfully");
#else 
	MessageBox("法线方向计算成功");
#endif
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	return TRUE;
}
#endif
