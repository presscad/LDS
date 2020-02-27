// ToolKitDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "ToolKitDlg.h"
#include "XhCharString.h"
#include "LmaDlg.h"
#include "AddVecOffsetItemDlg.h"
#include "PartUcsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OPER_PRE_ARCLIFT::Cal()
{
	double rot_ang=0;
	f3dLine axis(rot_center,rot_center);
	if(rot_axis=='X')
	{
		axis.startPt.x-=100;
		double deitay=fabs(lift_pos.y-rot_center.y);
		double tana=lift_height/deitay;
		rot_ang=atan(tana);
		result=pick;
		rotate_point_around_axis(result,rot_ang,axis);
	}
	else //if(rot_axis=='Y')
	{
		axis.startPt.y-=100;
		double deitax=fabs(lift_pos.x-rot_center.x);
		double tana=lift_height/deitax;
		rot_ang=atan(tana);
		result=pick;
		rotate_point_around_axis(result,rot_ang,axis);
	}
}
#ifndef __TSA_
BOOL OPER_BOARDCAL::Cal(char *error_info)
{
	CLDSLinePart *pGradLinePart=NULL,*pHoriLinePart=NULL;
	pGradLinePart=(CLDSLinePart*)console.FromPartHandle(hPoleGrad,CLS_LINEPART);
	pHoriLinePart=(CLDSLinePart*)console.FromPartHandle(hPoleHori,CLS_LINEPART);
	if(pGradLinePart==NULL||pHoriLinePart==NULL||
		pGradLinePart->pStart==NULL||pGradLinePart->pEnd==NULL||
		pHoriLinePart->pStart==NULL||pHoriLinePart->pEnd==NULL)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(error_info,"The input rod is error!");
#else 
			strcpy(error_info,"����˼�����!");
#endif
		return FALSE;
	}
	//1.����б���˼���ˮƽ�˼����췽��
	f3dPoint grad_vec = pGradLinePart->End()-pGradLinePart->Start();
	f3dPoint hori_vec = pHoriLinePart->End()-pHoriLinePart->Start();
	normalize(grad_vec);
	normalize(hori_vec);
	//1.1�ж�Ԥ��ֵ������
	CPtInLineCheck lineCheck(pHoriLinePart->pStart->Position(false),pHoriLinePart->pEnd->Position(false));
	double start_just=lineCheck.CheckPoint(pGradLinePart->pStart->Position(false));
	double end_just=lineCheck.CheckPoint(pGradLinePart->pEnd->Position(false));
	bool bGradTipIsEnd=true,bHoriTipIsEnd=true;
	if(start_just>=0&&start_just<=1.0)
	{
		bGradTipIsEnd=false;
		if( DISTANCE(pGradLinePart->pStart->Position(false),pHoriLinePart->Start())<
			DISTANCE(pGradLinePart->pStart->Position(false),pHoriLinePart->End()))
			bHoriTipIsEnd=false;	//��->ʼ
	}
	else if(end_just>=0&&end_just<=1.0)
	{
		if( DISTANCE(pGradLinePart->pEnd->Position(false),pHoriLinePart->Start())<
			DISTANCE(pGradLinePart->pEnd->Position(false),pHoriLinePart->End()))
			bHoriTipIsEnd=false;	//��->ʼ
	}
	bool bPositiveLift=true;
	if(!bHoriTipIsEnd)
		hori_vec*=-1.0;	//ˮƽ����չ���췽��
	if(bGradTipIsEnd)
		grad_vec*=-1.0;	//б�����������췽��
	if(hori_vec.z*grad_vec.z<0)
		bPositiveLift=false;
	//2.����б��ƫ������ˮƽ��ƫ����
	if(fabs(grad_vec.z)<EPS)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(error_info,"The diagonal rod inputs error");
#else 
			strcpy(error_info,"б���˼��������");
#endif
		return FALSE;
	}
	f3dPoint work_norm=grad_vec^hori_vec;
	normalize(work_norm);
	double hori_offset=0;	//ˮƽ�˼��Ҳ�ڼ�ƫ����
	f3dPoint grad_vec2d;	//б�˼���ƽ̨��չ����Ķ�άͶӰ(һ�������Ϊ0)
	f3dPoint hori_vec2d;	//ˮƽ�˼���ƽ̨��չ����Ķ�άͶӰ(һ�������Ϊ0)
	double space_cal_coef;	//����(������)��϶����תԤ����ĸ߶�(X/Y�����������)������ϵ��
	if(fabs(work_norm*f3dPoint(0,1,0))>fabs(work_norm*f3dPoint(1,0,0)))
	{
		if(fabs(hori_vec.x)<EPS)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The horizontal rod inputs error");
#else 
				strcpy(error_info,"ˮƽ�˼��������");
#endif
			return FALSE;
		}
		grad_vec2d.Set(grad_vec.x,0,grad_vec.z);
		hori_vec2d.Set(hori_vec.x,0,hori_vec.z);	//ˮƽ�˼���ƽ̨��չ����Ķ�άͶӰ(һ�������Ϊ0)
		normalize(grad_vec2d);
		normalize(hori_vec2d);
		//�����϶����תԤ�����X���������Ӧ��������
		space_cal_coef=fabs(hori_vec2d.x);
		hori_offset = hori_space*fabs(hori_vec2d.x/hori_vec.x);
	}
	else 
	{
		if(fabs(hori_vec.y)<EPS)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The horizontal rod inputs error");
#else 
				strcpy(error_info,"ˮƽ�˼��������");
#endif
			return FALSE;
		}
		grad_vec2d.Set(0,grad_vec.y,grad_vec.z);
		hori_vec2d.Set(0,hori_vec.y,hori_vec.z);	//ˮƽ�˼���ƽ̨��չ����Ķ�άͶӰ(һ�������Ϊ0)
		normalize(grad_vec2d);
		normalize(hori_vec2d);
		//�����϶����תԤ�����Y���������Ӧ��������
		space_cal_coef=fabs(hori_vec2d.y);
		hori_offset = hori_space*fabs(hori_vec2d.y/hori_vec.y);
	}
	double cos2dprj=grad_vec2d*hori_vec2d;
	double tana2dprj=sqrt(fabs(1-cos2dprj*cos2dprj))/cos2dprj;
	double left_hori_2dprjoffset=vert_high/tana2dprj;
	if(bPositiveLift)
		grad_offset = (vert_high*space_cal_coef-fabs(left_hori_2dprjoffset*hori_vec2d.z))/fabs(grad_vec.z);	//б��ƫ����
	else
		grad_offset = (vert_high*space_cal_coef+fabs(left_hori_2dprjoffset*hori_vec2d.z))/fabs(grad_vec.z);	//б��ƫ����
	//3.�������˼�����
	f3dLine base_zhun_line,grad_zhun_line;
	if(pGradLinePart->IsAngle()&&pHoriLinePart->IsAngle())
	{
		f3dPoint base_vec,other_vec;
		CLDSLineAngle *pHoriAngle=(CLDSLineAngle*)pHoriLinePart;	//ˮƽ�Ǹ�Ϊ��׼�Ǹ�
		CLDSLineAngle *pGradAngle=(CLDSLineAngle*)pGradLinePart;
		JGZJ base_jgzj,other_jgzj;
		int x_wing0_y_wing1=0;
		getjgzj(base_jgzj,pHoriAngle->GetWidth());
		CLDSLineAngle::IsInsideAngle(pHoriAngle,work_norm,&x_wing0_y_wing1);
		if(x_wing0_y_wing1==0)	//X֫
		{
			base_vec=pHoriAngle->GetWingVecX();
			if(pHoriAngle->m_bEnableTeG)
				base_jgzj=pHoriAngle->xWingXZhunJu;
		}
		else					//Y֫
		{
			base_vec=pHoriAngle->GetWingVecY();
			if(pHoriAngle->m_bEnableTeG)
				base_jgzj=pHoriAngle->xWingYZhunJu;
		}
		base_zhun_line.startPt=pHoriAngle->Start()+base_vec*base_jgzj.g;
		base_zhun_line.endPt=pHoriAngle->End()+base_vec*base_jgzj.g;
		
		getjgzj(other_jgzj,pGradAngle->GetWidth());
		CLDSLineAngle::IsInsideAngle(pGradAngle,work_norm,&x_wing0_y_wing1);
		if(x_wing0_y_wing1==0)	//X֫
		{
			other_vec=pGradAngle->GetWingVecX();
			if(pGradAngle->m_bEnableTeG)
				other_jgzj=pGradAngle->xWingXZhunJu;
		}
		else					//Y֫
		{
			other_vec=pGradAngle->GetWingVecY();
			if(pGradAngle->m_bEnableTeG)
				other_jgzj=pGradAngle->xWingYZhunJu;
		}
		grad_zhun_line.startPt=pGradAngle->Start()+other_vec*other_jgzj.g;
		grad_zhun_line.endPt=pGradAngle->End()+other_vec*other_jgzj.g;
		project_point(grad_zhun_line.startPt,base_zhun_line.startPt,work_norm);
		project_point(grad_zhun_line.endPt,base_zhun_line.startPt,work_norm);
	}
	else
	{
		base_zhun_line.startPt=pHoriLinePart->Start();
		base_zhun_line.endPt=pHoriLinePart->End();
		grad_zhun_line.startPt=pGradLinePart->Start();
		grad_zhun_line.endPt=pGradLinePart->End();
	}
	f3dPoint inters_pt;
	int nRetCode=Int3dll(base_zhun_line,grad_zhun_line,inters_pt);
	//4.�����˼�����Ϊ��׼�����grad_offset,hori_offset����corner_up
	if(nRetCode==-2||nRetCode==1||nRetCode==2)	//ֱ���⽻�㡢ֱ���ڽ��㡢ֱ�߶ζϵ�
	{	
		//if(DISTANCE(inters_pt,pGradLinePart->Start())>DISTANCE(inters_pt,pGradLinePart->End()))
		//	grad_vec*=-1.0;	//��->ʼ
		//if(DISTANCE(inters_pt,pHoriLinePart->Start())<DISTANCE(inters_pt,pHoriLinePart->End()))
		//	hori_vec*=-1.0;	//��->ʼ
		double cosa=grad_vec*hori_vec;
		left_hori_offset=grad_offset*fabs(cosa);
		if(cosa>0)	//ˮƽ�˼�������б���˼�����ͬ��
			corner_up = inters_pt+grad_vec*grad_offset-hori_vec*(left_hori_offset+hori_offset);
		else		//ˮƽ�˼�������б���˼�������
			corner_up = inters_pt+grad_vec*grad_offset+hori_vec*(left_hori_offset+hori_offset);
		corner_down=inters_pt+hori_vec*hori_offset;
		SnapPerp(&corner_down,pHoriLinePart->Start(),pHoriLinePart->End(),corner_down);
	}
	else 
	{
		if(error_info)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(nRetCode==-1)
				strcpy(error_info,"Line's endpoint is overlap,the definition is wrong");
			else if(nRetCode==0)
				strcpy(error_info,"Line is parallel,overlapping or different surface");
			else 
				strcpy(error_info,"Calculate error");
#else 
			if(nRetCode==-1)
				strcpy(error_info,"ֱ�߶˵��غϣ���������");
			else if(nRetCode==0)
				strcpy(error_info,"ֱ��ƽ�С��غϻ�����");
			else 
				strcpy(error_info,"�������쳣");
#endif
		}
		return FALSE;
	}
	return TRUE;
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CToolKitDlg dialog
static BOOL ModifyToolKitsProperty(CPropertyList* pPropList,CPropTreeItem* pItem,CString& valueStr)
{
	CToolKitDlg *pToolKitDlg=(CToolKitDlg*)pPropList->GetParent();
	if(pToolKitDlg==NULL)
		return FALSE;
	BOOL bContinue=FALSE;
	if(CToolKitDlg::GetPropID("OPER_VEC_LEN.start.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_len.start.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.start.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_len.start.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.start.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_len.start.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.end.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_len.end.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.end.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_len.end.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.end.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_len.end.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.result")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_len.result=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.start.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.start.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.start.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.start.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.start.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.start.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.end.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.end.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.end.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.end.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.end.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.end.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.result.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.result.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.result.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.result.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.result.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_p2p.result.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec1.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_ang.vec1.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec1.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_ang.vec1.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec1.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_ang.vec1.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec2.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_ang.vec2.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec2.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_ang.vec2.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec2.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_ang.vec2.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.rad_angle")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.vec_ang.rad_angle=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_OFFSET_ORIGIN.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.offset_origin.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_OFFSET_ORIGIN.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.offset_origin.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_OFFSET_ORIGIN.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.offset_origin.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.src.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.trans_cs.src.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.src.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.trans_cs.src.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.src.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.trans_cs.src.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.dest.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.trans_cs.dest.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.dest.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.trans_cs.dest.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.dest.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.trans_cs.dest.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[0].x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[0].x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[0].y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[0].y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[0].z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[0].z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[1].x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[1].x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[1].y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[1].y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[1].z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[1].z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[2].x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[2].x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[2].y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[2].y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[2].z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.p[2].z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.face_norm.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.face_norm.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.face_norm.z=atof(valueStr);
	//
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line1.startPt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line1.startPt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line1.startPt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line1.endPt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line1.endPt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line1.endPt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line2.startPt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line2.startPt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line2.startPt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line2.endPt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line2.endPt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.line2.endPt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.result.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.result.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_ll.result.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.line.startPt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.line.startPt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.line.startPt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.line.endPt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.line.endPt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.line.endPt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.face_pos.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.face_pos.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.face_pos.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.face_norm.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.face_norm.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.face_norm.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.result.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.result.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.inters_lf.result.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.pt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.pt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.pt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.line_start.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.line_start.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.line_start.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.line_end.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.line_end.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.line_end.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.result")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pl.result=atof(valueStr);
	//
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.pt.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.pt.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.pt.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.face_pos.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.face_pos.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.face_pos.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.face_norm.x=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.face_norm.y=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.face_norm.z=atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.result")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.dist_pf.result=atof(valueStr);

	//
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_axis")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.rot_axis = valueStr[0];
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_height")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.lift_height = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.rot_center.x = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.rot_center.y = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.rot_center.z = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.lift_pos.x = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.lift_pos.y = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.lift_pos.z = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.pick.x = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.pick.y = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.pick.z = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.x")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.result.x = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.y")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.result.y = atof(valueStr);
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.z")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.arc_lift.result.z = atof(valueStr);
	else 
		bContinue = TRUE;
	if(bContinue)
	{	
#ifndef __TSA_
		if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleGrad")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pToolKitDlg->m_xOperData.board.hPoleGrad));
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleHori")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pToolKitDlg->m_xOperData.board.hPoleHori));
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.vert_high")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.vert_high=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hori_space")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.hori_space=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.grad_offset")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.grad_offset=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.left_hori_offset")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.left_hori_offset=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.x")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.corner_up.x=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.y")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.corner_up.y=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.z")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.corner_up.z=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.x")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.corner_down.x=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.y")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.corner_down.y=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.z")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.board.corner_down.z=atof(valueStr);
		else 
#endif
		//ʸ��ƫ��
			if(CToolKitDlg::GetPropID("OPER_OFFSET_ORIGIN.x")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.offset_origin.x=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_OFFSET_ORIGIN.y")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.offset_origin.y=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_OFFSET_ORIGIN.z")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.offset_origin.z=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_OFFSET.result.x")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.result.x=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_OFFSET.result.y")==pItem->m_idProp)
			pToolKitDlg->m_xOperData.result.y=atof(valueStr);
		else if(CToolKitDlg::GetPropID("OPER_OFFSET.result.z")==pItem->m_idProp) 
			pToolKitDlg->m_xOperData.result.z=atof(valueStr);
		else if(pToolKitDlg->listOffset.GetNodeNum()>0)
		{
			TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pVecOffsetItem=NULL;
			for(pVecOffsetItem=pToolKitDlg->listOffset.GetFirst();pVecOffsetItem;pVecOffsetItem=pToolKitDlg->listOffset.GetNext())
			{
				if(pVecOffsetItem->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_VEC.x")==pItem->m_idProp)
				{
					pVecOffsetItem->vec.x=atof(valueStr);
					return TRUE;
				}
				else if(pVecOffsetItem->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_VEC.y")==pItem->m_idProp)
				{
					pVecOffsetItem->vec.y=atof(valueStr);
					return TRUE;
				}
				else if(pVecOffsetItem->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_VEC.z")==pItem->m_idProp)
				{
					pVecOffsetItem->vec.z=atof(valueStr);
					return TRUE;
				}	
				else if(pVecOffsetItem->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_DIST")==pItem->m_idProp)
				{
					pVecOffsetItem->offset=atof(valueStr);
					return TRUE;
				}
			}
		}
	}
	return TRUE;
}

static TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *GetVecOffsetById(CToolKitDlg *pDlg,int idProp)
{
	TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pVecOffset=NULL;
	if(pDlg)
	{
		for(pVecOffset=pDlg->listOffset.GetFirst();pVecOffset;pVecOffset=pDlg->listOffset.GetNext())
		{
			if(pVecOffset->iNo*VEC_OFFSET_ID_COEF==idProp)
				break;
		}
	}
	return pVecOffset;
}

static BOOL ButtonClickPointItem(CPropertyList *pPropList,f3dPoint &point,
								 char *sIdProp,char *sCaption)
{
	CXhChar100 sText;
	CReportVectorDlg pointDlg;
	pointDlg.m_sCaption = sCaption;
	pointDlg.m_fVectorX = point.x;
	pointDlg.m_fVectorY = point.y;
	pointDlg.m_fVectorZ = point.z;
	if(pointDlg.DoModal()==IDOK)
	{
		sText.Printf("%f",pointDlg.m_fVectorX);
		SimplifiedNumString(sText);
		CXhChar100 sSonProp("%s.x",sIdProp);
		pPropList->SetItemPropValue(CToolKitDlg::GetPropID(sSonProp),CString(sText));
		sText.Printf("%f",pointDlg.m_fVectorY);
		SimplifiedNumString(sText);
		sSonProp.Printf("%s.y",sIdProp);
		pPropList->SetItemPropValue(CToolKitDlg::GetPropID(sSonProp),CString(sText));
		sText.Printf("%f",pointDlg.m_fVectorZ);
		SimplifiedNumString(sText);
		sSonProp.Printf("%s.z",sIdProp);
		pPropList->SetItemPropValue(CToolKitDlg::GetPropID(sSonProp),CString(sText));
		point.Set(pointDlg.m_fVectorX,pointDlg.m_fVectorY,pointDlg.m_fVectorZ);
	}
	return TRUE;
}

static BOOL ToolKitsButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	if(pPropList==NULL || pItem==NULL)
		return FALSE;
	TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pVecOffset=NULL;
	CToolKitDlg *pToolKitDlg=(CToolKitDlg*)pPropList->GetParent();
	if(pToolKitDlg==NULL)
		return FALSE;
	CXhChar100 sText;
	if(CToolKitDlg::GetPropID("OPER_VEC_LEN.start")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_len.start,"OPER_VEC_LEN.start","Strat Coordinate");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_len.start,"OPER_VEC_LEN.start","�������");
#endif
		long idResult = CToolKitDlg::GetPropID("OPER_VEC_LEN.result");
		pToolKitDlg->m_xOperData.vec_len.CalLen();
		pPropList->SetItemPropValue(idResult,pToolKitDlg->GetPropValueStr(idResult));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.end")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_len.end,"OPER_VEC_LEN.end","End Coordinate");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_len.end,"OPER_VEC_LEN.end","�յ�����");
#endif
		long idResult = CToolKitDlg::GetPropID("OPER_VEC_LEN.result");
		pToolKitDlg->m_xOperData.vec_len.CalLen();
		pPropList->SetItemPropValue(idResult,pToolKitDlg->GetPropValueStr(idResult));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_LEN.result")==pItem->m_idProp)
	{
		pToolKitDlg->m_xOperData.vec_len.CalLen();
		pPropList->SetItemPropValue(pItem->m_idProp,pToolKitDlg->GetPropValueStr(pItem->m_idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.start")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_p2p.start,"OPER_VEC_P2P.start","Strat Coordinate");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_p2p.start,"OPER_VEC_P2P.start","�������");
#endif
		//����㵽��ʸ��
		pToolKitDlg->m_xOperData.vec_p2p.CalVec();
		long idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.end")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_p2p.end,"OPER_VEC_P2P.end","End Coordinate");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_p2p.end,"OPER_VEC_P2P.end","�յ�����");
#endif
		//����㵽��ʸ��
		pToolKitDlg->m_xOperData.vec_p2p.CalVec();
		long idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.result")==pItem->m_idProp)
	{	//���㲢����
		pToolKitDlg->m_xOperData.vec_p2p.CalVec();
		long idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		//
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_p2p.result,"OPER_VEC_P2P.result","Point To Point Vector");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_p2p.result,"OPER_VEC_P2P.result","�㵽��ʸ��");
#endif
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_P2P.CalResult")==pItem->m_idProp)
	{	//����㵽��ʸ��
		pToolKitDlg->m_xOperData.vec_p2p.CalVec();
		long idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_P2P.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec1")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_ang.vec1,"OPER_VEC_ANGLE.vec1","Vector 1");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_ang.vec1,"OPER_VEC_ANGLE.vec1","����1");
#endif
		pToolKitDlg->m_xOperData.vec_ang.CalAngle();
		long idProp = CToolKitDlg::GetPropID("OPER_VEC_ANGLE.rad_angle");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_ANGLE.deg_angle");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.vec2")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_ang.vec2,"OPER_VEC_ANGLE.vec2","Vector 2");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.vec_ang.vec2,"OPER_VEC_ANGLE.vec2","����2");
#endif
		pToolKitDlg->m_xOperData.vec_ang.CalAngle();
		long idProp = CToolKitDlg::GetPropID("OPER_VEC_ANGLE.rad_angle");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_VEC_ANGLE.deg_angle");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_VEC_ANGLE.deg_angle")==pItem->m_idProp)
	{
		pToolKitDlg->m_xOperData.vec_ang.CalAngle();
		pPropList->SetItemPropValue(pItem->m_idProp,pToolKitDlg->GetPropValueStr(pItem->m_idProp));
		int idProp=CToolKitDlg::GetPropID("OPER_VEC_ANGLE.rad_angle");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	//ʸ��ƫ��
	else if(CToolKitDlg::GetPropID("OPER_OFFSET.result")==pItem->m_idProp)
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.result,"OPER_OFFSET.result","Vector Offset Result");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.result,"OPER_OFFSET.result","ʸ��ƫ�ƽ��");
#endif
	else if(CToolKitDlg::GetPropID("OPER_OFFSET_ORIGIN")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.offset_origin,"OPER_OFFSET_ORIGIN","Vector Offset Start");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.offset_origin,"OPER_OFFSET_ORIGIN","ʸ��ƫ�����");
#endif
		//����ʸ��ƫ�ƽ��
		pToolKitDlg->CalVecOffsetResult();
		long idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_OFFSET.OFFSETLIST")==pItem->m_idProp)
	{
		CAddVecOffsetItemDlg dlg;
		dlg.m_bModifyItem=FALSE;
		if(dlg.DoModal()==IDOK)
		{
			TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pOffsetItem=NULL;
			pOffsetItem = pToolKitDlg->AddVecOffsetItem(f3dPoint(dlg.m_fVecX,dlg.m_fVecY,dlg.m_fVecZ),dlg.m_fOffsetDist);
			if(pItem)
			{
				long idInit = pOffsetItem->iNo*VEC_OFFSET_ID_COEF;
				CItemInfo *lpInfo=NULL;
				CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
				lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName.Format("Offset %d",pOffsetItem->iNo);
#else 
				lpInfo->m_strPropName.Format("ƫ����%d",pOffsetItem->iNo);
#endif
				lpInfo->m_controlType=PIT_BUTTON;
				pPropItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = idInit;
				pPropItem->m_bHideChildren=TRUE;
				if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
				pPropItem->m_dwPropGroup = pItem->m_dwPropGroup; 
				
				lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Coordinate";
#else 
				lpInfo->m_strPropName = "X����";
#endif
				lpInfo->m_controlType=PIT_EDIT;
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = idInit + CToolKitDlg::GetPropID("OPER_OFFSET_VEC.x");
				pSonPropItem->m_lpNodeInfo->m_strPropValue = pToolKitDlg->GetPropValueStr(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
				
				lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Y Coordinate";
#else 
				lpInfo->m_strPropName = "Y����";
#endif
				lpInfo->m_controlType=PIT_EDIT;
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,TRUE);
				pSonPropItem->m_idProp = idInit + CToolKitDlg::GetPropID("OPER_OFFSET_VEC.y");
				pSonPropItem->m_lpNodeInfo->m_strPropValue = pToolKitDlg->GetPropValueStr(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
				
				lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Z Coordinate";
#else 
				lpInfo->m_strPropName = "Z����";
#endif
				lpInfo->m_controlType=PIT_EDIT;
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,TRUE);
				pSonPropItem->m_idProp = idInit + CToolKitDlg::GetPropID("OPER_OFFSET_VEC.z");
				pSonPropItem->m_lpNodeInfo->m_strPropValue = pToolKitDlg->GetPropValueStr(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = pItem->m_dwPropGroup; 

				lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Offset Distance";
#else 
				lpInfo->m_strPropName = "ƫ�ƾ���";
#endif
				lpInfo->m_controlType=PIT_EDIT;
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,3,TRUE);
				pSonPropItem->m_idProp = idInit + CToolKitDlg::GetPropID("OPER_OFFSET_DIST");
				pSonPropItem->m_lpNodeInfo->m_strPropValue = pToolKitDlg->GetPropValueStr(pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = pItem->m_dwPropGroup; 
			}
			//����ʸ��ƫ�ƽ��
			pToolKitDlg->CalVecOffsetResult();
			long idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.x");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.y");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.z");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		}
	}
	else if(pVecOffset=GetVecOffsetById(pToolKitDlg,pItem->m_idProp))
	{
		CAddVecOffsetItemDlg dlg;
		dlg.m_bModifyItem=TRUE;
		dlg.m_fOffsetDist=pVecOffset->offset;
		dlg.m_fVecX = pVecOffset->vec.x;
		dlg.m_fVecY = pVecOffset->vec.y;
		dlg.m_fVecZ = pVecOffset->vec.z;
		if(dlg.DoModal()==IDOK)
		{
			pVecOffset->offset = dlg.m_fOffsetDist;
			pVecOffset->vec.Set(dlg.m_fVecX,dlg.m_fVecY,dlg.m_fVecZ);
			long idProp = pVecOffset->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_VEC.x");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp = pVecOffset->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_VEC.y");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp = pVecOffset->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_VEC.z");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp = pVecOffset->iNo*VEC_OFFSET_ID_COEF+CToolKitDlg::GetPropID("OPER_OFFSET_DIST");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		}
		if(dlg.m_bDelItem)
		{
			TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pTempItem=NULL;
			for(pTempItem=pToolKitDlg->listOffset.GetFirst();pTempItem;pTempItem=pToolKitDlg->listOffset.GetNext())
			{
				if(pTempItem->iNo==pVecOffset->iNo)
				{
					pPropList->DeleteItemByPropId(pVecOffset->iNo*VEC_OFFSET_ID_COEF);
					pToolKitDlg->listOffset.DeleteCursor(TRUE);
					//����ʸ��ƫ�ƽ��
					pToolKitDlg->CalVecOffsetResult();
					long idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.x");
					pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
					idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.y");
					pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
					idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.z");
					pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
					return FALSE;
				}
			}
		}
	}
	else if(CToolKitDlg::GetPropID("OPER_OFFSET.CalResult")==pItem->m_idProp)
	{	//����ʸ��ƫ�ƽ��
		pToolKitDlg->CalVecOffsetResult();
		long idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_OFFSET.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	//����ϵת��
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.cs")==pItem->m_idProp)
	{
		static CPartUcsDlg csDlg;
		csDlg.ucs=pToolKitDlg->m_xOperData.trans_cs.cs;
		if(csDlg.DoModal()==IDOK)
			pToolKitDlg->m_xOperData.trans_cs.cs=csDlg.ucs;
	}
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.wcs2ucs")==pItem->m_idProp
			||CToolKitDlg::GetPropID("OPER_CS_TRANS.ucs2wcs")==pItem->m_idProp)
	{
		pToolKitDlg->m_xOperData.trans_cs.CoordTrans(CToolKitDlg::GetPropID("OPER_CS_TRANS.ucs2wcs")==pItem->m_idProp);
		long idProp=CToolKitDlg::GetPropID("OPER_CS_TRANS.dest.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_CS_TRANS.dest.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_CS_TRANS.dest.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.dest")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.trans_cs.dest,"OPER_CS_TRANS.dest","Convert Result");
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.src")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.trans_cs.src,"OPER_CS_TRANS.src","Inpuy To Converting Coordinate");
#else 
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.dest")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.trans_cs.dest,"OPER_CS_TRANS.dest","ת�����");
	else if(CToolKitDlg::GetPropID("OPER_CS_TRANS.src")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.trans_cs.src,"OPER_CS_TRANS.src","�����ת������");
#endif
	//���㶨��
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.CalPlane")==pItem->m_idProp)
	{
		pToolKitDlg->m_xOperData.face3p.CalPlane();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.CopyPlane")==pItem->m_idProp)
		pToolKitDlg->m_xOperData.face3p.CopyPlane();
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[0]")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.p[0],"OPER_3D_FACE3P.p[0]","Datum Point 1");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.p[0],"OPER_3D_FACE3P.p[0]","��׼��1");
#endif
		//
		pToolKitDlg->m_xOperData.face3p.CalPlane();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[1]")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.p[1],"OPER_3D_FACE3P.p[1]","Datum Point 2");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.p[1],"OPER_3D_FACE3P.p[1]","��׼��2");
#endif
		//
		pToolKitDlg->m_xOperData.face3p.CalPlane();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.p[2]")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.p[2],"OPER_3D_FACE3P.p[2]","Datum Point 3");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.p[2],"OPER_3D_FACE3P.p[2]","��׼��3");
#endif
		//
		pToolKitDlg->m_xOperData.face3p.CalPlane();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_FACE3P.face_norm")==pItem->m_idProp)
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.face_norm,"OPER_3D_FACE3P.face_norm","Plane Normal");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.face3p.face_norm,"OPER_3D_FACE3P.face_norm","�淨��");
#endif
	//
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.CalInters")==pItem->m_idProp)
	{
		char sErrorInfo[100]="";
		if(pToolKitDlg->m_xOperData.inters_ll.CalIntersPt(sErrorInfo))
		{
			long idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.x");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.y");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.z");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		}
		else 
			AfxMessageBox(sErrorInfo);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line1.startPt,"OPER_3D_INT_LL.line1.start","Line Start");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line1.endPt,"OPER_3D_INT_LL.line1.end","Line End");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line2.startPt,"OPER_3D_INT_LL.line2.start","Line Start");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line2.endPt,"OPER_3D_INT_LL.line2.end","Line End");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.result,"OPER_3D_INT_LL.result","Two Line Intersection");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.line.startPt,"OPER_3D_INT_LF.line.start","Line Start");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.line.endPt,"OPER_3D_INT_LF.line.end","Line End");
#else 
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line1.startPt,"OPER_3D_INT_LL.line1.start","ֱ�����");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line1.endPt,"OPER_3D_INT_LL.line1.end","ֱ���յ�");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line2.startPt,"OPER_3D_INT_LL.line2.start","ֱ�����");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.line2.endPt,"OPER_3D_INT_LL.line2.end","ֱ���յ�");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_ll.result,"OPER_3D_INT_LL.result","��ֱ�߽���");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.line.startPt,"OPER_3D_INT_LF.line.start","ֱ�����");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.line.endPt,"OPER_3D_INT_LF.line.end","ֱ���յ�");
#endif
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.plane")==pItem->m_idProp)
	{	//ճ��ƽ��
		pToolKitDlg->m_xOperData.inters_lf.PastPlane();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.face_pos,"OPER_3D_INT_LF.face_pos","Plane Datum Point");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.face_norm,"OPER_3D_INT_LF.face_norm","Plane Normal");
#else 
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.face_pos,"OPER_3D_INT_LF.face_pos","�����");
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.face_norm,"OPER_3D_INT_LF.face_norm","�淨��");
#endif
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.CalInters")==pItem->m_idProp)
	{
		char sErrorInfo[100]="";
		if(pToolKitDlg->m_xOperData.inters_lf.CalIntersPt(sErrorInfo))
		{
			long idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.x");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.y");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.z");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		}
		else 
			AfxMessageBox(sErrorInfo);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.result,"OPER_3D_INT_LF.result","Line&Plane Intersection");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pl.pt,"OPER_3D_DIST_PL.pt","Datum Point");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pl.line_start,"OPER_3D_DIST_PL.line_start","Line Start");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pl.line_end,"OPER_3D_DIST_PL.line_end","Line End");
#else 
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.inters_lf.result,"OPER_3D_INT_LF.result","���潻��");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pl.pt,"OPER_3D_DIST_PL.pt","��׼��");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pl.line_start,"OPER_3D_DIST_PL.line_start","ֱ�����");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pl.line_end,"OPER_3D_DIST_PL.line_end","ֱ���յ�");
#endif
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.result")==pItem->m_idProp)
	{
		pToolKitDlg->m_xOperData.dist_pl.Cal();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PL.result");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	//
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.plane")==pItem->m_idProp)
	{	//ճ��ƽ��
		pToolKitDlg->m_xOperData.dist_pf.PastPlane();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pf.pt,"OPER_3D_DIST_PF.pt","Datum Point");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pf.face_pos,"OPER_3D_DIST_PF.face_pos","Plane Datum Point");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pf.face_norm,"OPER_3D_DIST_PF.face_norm","Plane Normal");
#else 
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pf.pt,"OPER_3D_DIST_PF.pt","��׼��");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pf.face_pos,"OPER_3D_DIST_PF.face_pos","�����");
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.dist_pf.face_norm,"OPER_3D_DIST_PF.face_norm","�淨��");
#endif
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.result")==pItem->m_idProp)
	{
		pToolKitDlg->m_xOperData.dist_pf.Cal();
		long idProp=CToolKitDlg::GetPropID("OPER_3D_DIST_PF.result");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	//Բ��Ԥ��
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.rot_center,"OPER_PRE_ARCLIFT.rot_center","Prelife Rotate Datum Point");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.rot_center,"OPER_PRE_ARCLIFT.rot_center","Ԥ����ת����");
#endif
		//����Ԥ����λ��
		pToolKitDlg->m_xOperData.arc_lift.Cal();
		long idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.lift_pos,"OPER_PRE_ARCLIFT.lift_pos","Start Of Datum Prelife Point");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.lift_pos,"OPER_PRE_ARCLIFT.lift_pos","��׼Ԥ�����ʼλ��");
#endif
		//����Ԥ����λ��
		pToolKitDlg->m_xOperData.arc_lift.Cal();
		long idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.pick,"OPER_PRE_ARCLIFT.pick","Start Of Prelife Point");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.pick,"OPER_PRE_ARCLIFT.pick","Ԥ�����ʼλ��");
#endif
		//����Ԥ����λ��
		pToolKitDlg->m_xOperData.arc_lift.Cal();
		long idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result")==pItem->m_idProp)
#ifdef AFX_TARG_ENU_ENGLISH
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.result,"OPER_PRE_ARCLIFT.result","Position After Prelife");
#else 
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.arc_lift.result,"OPER_PRE_ARCLIFT.result","Ԥ����λ��");
#endif
	else if(CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.CalResult")==pItem->m_idProp)
	{	//����Ԥ����λ��
		pToolKitDlg->m_xOperData.arc_lift.Cal();
		long idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.x");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.y");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		idProp=CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.z");
		pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
	}
	//ƽ̨����
#ifndef __TSA_
	else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleGrad")==pItem->m_idProp)
		pToolKitDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//б���˼�
	else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleHori")==pItem->m_idProp)
		pToolKitDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//ˮƽ�˼�
	else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.CalResult")==pItem->m_idProp)
	{
		char error_info[200]="";
		if(pToolKitDlg->m_xOperData.board.Cal(error_info))
		{
			long idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.grad_offset");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.left_hori_offset");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.x");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.y");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.z");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.x");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.y");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
			idProp=CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.z");
			pPropList->SetItemPropValue(idProp,pToolKitDlg->GetPropValueStr(idProp));
		}
		else 
			AfxMessageBox(error_info);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.board.corner_up,"OPER_PRE_BOARDCAL.corner_up","Up Right Corner Position Of Datum Point");
	else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.board.corner_down,"OPER_PRE_BOARDCAL.corner_down","Down Right Corner Position Of Ridge Vertex");
#else 
	else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.board.corner_up,"OPER_PRE_BOARDCAL.corner_up","���Ͻǻ���λ��");
	else if(CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down")==pItem->m_idProp)
		ButtonClickPointItem(pPropList,pToolKitDlg->m_xOperData.board.corner_down,"OPER_PRE_BOARDCAL.corner_down","���½����λ��");
#endif
#endif
	return TRUE;
}
static BOOL ToolKitsItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	return CToolKitDlg::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
}

static BOOL FireSelChange(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CToolKitDlg *pToolKitDlg=(CToolKitDlg*)pPropList->GetParent();
	if(pToolKitDlg==NULL)
		return FALSE;
	pToolKitDlg->UpdateSketchMap(pItem);
	return TRUE;
}

CToolKitDlg::CToolKitDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CToolKitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolKitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iMinAvailable=1;
}


void CToolKitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolKitDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_TAB_GROUP, m_tabCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolKitDlg, CDialog)
	//{{AFX_MSG_MAP(CToolKitDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CToolKitDlg::m_iCurGroup;				//��¼��������ǰ��ʾ���ID
CStringKeyHashTable<long> CToolKitDlg::propHashtable;	//�����ַ�����ϣ��
CHashTable<DWORD> CToolKitDlg::propStatusHashtable;		//����״̬��ϣ��
void CToolKitDlg::InitToolkitsPropHashtable()
{
	int id=1;
	CToolKitDlg::propHashtable.CreateHashTable(500);
	CToolKitDlg::propStatusHashtable.CreateHashTable(50);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.start",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.start.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.start.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.start.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.end",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.end.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.end.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.end.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_LEN.result",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.start",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.start.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.start.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.start.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.end",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.end.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.end.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.end.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.CalResult",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.result",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.result.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.result.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_P2P.result.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec1",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec1.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec1.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec1.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec2",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec2.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec2.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.vec2.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.rad_angle",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_VEC_ANGLE.deg_angle",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_ORIGIN",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_ORIGIN.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_ORIGIN.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_ORIGIN.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET.OFFSETLIST",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_VEC",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_VEC.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_VEC.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_VEC.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET_DIST",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET.CalResult",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET.result",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET.result.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET.result.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_OFFSET.result.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.cs",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.src",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.src.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.src.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.src.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.wcs2ucs",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.ucs2wcs",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.dest",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.dest.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.dest.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_CS_TRANS.dest.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.CalPlane",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.CopyPlane",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[0]",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[0].x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[0].y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[0].z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[1]",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[1].x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[1].y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[1].z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[2]",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[2].x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[2].y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.p[2].z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.face_norm",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.face_norm.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.face_norm.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_FACE3P.face_norm.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.CalInters",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.start",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.start.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.start.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.start.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.end",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.end.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.end.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line1.end.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.start",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.start.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.start.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.start.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.end",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.end.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.end.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.line2.end.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.result",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.result.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.result.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LL.result.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.start",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.start.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.start.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.start.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.end",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.end.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.end.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.line.end.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.plane",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_pos",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_pos.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_pos.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_pos.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_norm",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_norm.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_norm.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.face_norm.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.CalInters",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.result",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.result.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.result.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_INT_LF.result.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.pt",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.pt.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.pt.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.pt.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_start",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_start.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_start.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_start.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_end",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_end.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_end.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.line_end.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PL.result",id++);
	
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.plane",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.pt",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.pt.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.pt.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.pt.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_pos",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_pos.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_pos.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_pos.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_norm",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_norm.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_norm.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.face_norm.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_3D_DIST_PF.result",id++);
	//
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.rot_axis",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.lift_height",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.rot_center",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.rot_center.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.rot_center.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.rot_center.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.lift_pos",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.lift_pos.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.lift_pos.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.lift_pos.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.pick",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.pick.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.pick.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.pick.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.CalResult",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.result",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.result.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.result.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_ARCLIFT.result.z",id++);
	//
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.InPutParam",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.hPoleGrad",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.hPoleHori",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.vert_high",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.hori_space",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.CalResult",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.grad_offset",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.left_hori_offset",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_up",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_up.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_up.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_up.z",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_down",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_down.x",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_down.y",id++);
	CToolKitDlg::propHashtable.SetValueAt("OPER_PRE_BOARDCAL.corner_down.z",id++);
}
long CToolKitDlg::GetPropID(char* propStr)		//�õ�����ID
{
	long id;
	if(CToolKitDlg::propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
int CToolKitDlg::GetPropStatus(long id)	//�õ�����״̬
{
	DWORD dwStatus;
	if(CToolKitDlg::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}
BOOL CToolKitDlg::SetPropStatus(long id,BOOL bHide)	//��������״̬
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CToolKitDlg::propStatusHashtable.SetValueAt(id,dwStatus);
}
CString CToolKitDlg::GetPropValueStr(long id)	//��������ID�õ�����ֵ
{
	char sText[100]="";
	CXhChar100 valueStr;
	bool bContinueJustify=false;
	if(GetPropID("OPER_VEC_LEN.start.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_len.start.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_LEN.start.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_len.start.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_LEN.start.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_len.start.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_LEN.end.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_len.end.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_LEN.end.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_len.end.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_LEN.end.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_len.end.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_LEN.result")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_len.result);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.start.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.start.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.start.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.start.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.start.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.start.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.end.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.end.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.end.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.end.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.end.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.end.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.result.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.result.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.result.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.result.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_P2P.result.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_p2p.result.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.vec1.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.vec1.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.vec1.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.vec1.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.vec1.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.vec1.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.vec2.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.vec2.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.vec2.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.vec2.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.vec2.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.vec2.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.rad_angle")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.rad_angle);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_VEC_ANGLE.deg_angle")==id)
	{
		valueStr.Printf("%f",m_xOperData.vec_ang.rad_angle*DEGTORAD_COEF);
		SimplifiedNumString(valueStr);
	}
	//����ϵת��
	else if(GetPropID("OPER_CS_TRANS.src.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.trans_cs.src.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_CS_TRANS.src.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.trans_cs.src.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_CS_TRANS.src.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.trans_cs.src.z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_CS_TRANS.dest.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.trans_cs.dest.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_CS_TRANS.dest.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.trans_cs.dest.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_CS_TRANS.dest.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.trans_cs.dest.z);
		SimplifiedNumString(valueStr);
	}
	//���㶨��
	else if(GetPropID("OPER_3D_FACE3P.p[0].x")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[0].x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[0].y")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[0].y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[0].z")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[0].z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[1].x")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[1].x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[1].y")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[1].y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[1].z")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[1].z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[2].x")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[2].x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[2].y")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[2].y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.p[2].z")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.p[2].z);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.face_norm.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.face_norm.x);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.face_norm.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.face_norm.y);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("OPER_3D_FACE3P.face_norm.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.face3p.face_norm.z);
		SimplifiedNumString(valueStr);
	}
	//
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line1.startPt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line1.startPt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.start.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line1.startPt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line1.endPt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line1.endPt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line1.end.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line1.endPt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line2.startPt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line2.startPt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.start.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line2.startPt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line2.endPt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line2.endPt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.line2.end.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.line2.endPt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.result.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.result.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LL.result.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_ll.result.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.line.startPt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.line.startPt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.start.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.line.startPt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.line.endPt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.line.endPt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.line.end.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.line.endPt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.face_pos.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.face_pos.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_pos.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.face_pos.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.face_norm.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.face_norm.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.face_norm.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.face_norm.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.result.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.result.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_INT_LF.result.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.inters_lf.result.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.pt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.pt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.pt.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.pt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.line_start.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.line_start.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_start.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.line_start.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.line_end.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.line_end.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.line_end.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.line_end.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PL.result")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pl.result);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.pt.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.pt.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.pt.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.pt.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.face_pos.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.face_pos.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_pos.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.face_pos.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.x")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.face_norm.x);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.y")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.face_norm.y);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.face_norm.z")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.face_norm.z);
		SimplifiedNumString(valueStr);
	}
	else if(CToolKitDlg::GetPropID("OPER_3D_DIST_PF.result")==id)
	{
		valueStr.Printf("%f",m_xOperData.dist_pf.result);
		SimplifiedNumString(valueStr);
	}
	else 
		bContinueJustify = true;
	if(bContinueJustify)
	{
		//Բ��Ԥ��
		if(GetPropID("OPER_PRE_ARCLIFT.rot_axis")==id)
			valueStr.Printf("%c",m_xOperData.arc_lift.rot_axis);
		else if(GetPropID("OPER_PRE_ARCLIFT.lift_height")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.lift_height);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.rot_center.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.rot_center.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.rot_center.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.rot_center.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.rot_center.z")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.rot_center.z);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.lift_pos.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.lift_pos.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.lift_pos.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.lift_pos.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.lift_pos.z")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.lift_pos.z);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.pick.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.pick.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.pick.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.pick.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.pick.z")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.pick.z);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.result.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.result.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.result.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.result.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_ARCLIFT.result.z")==id)
		{
			valueStr.Printf("%f",m_xOperData.arc_lift.result.z);
			SimplifiedNumString(valueStr);
		}
		//ƽ̨����
#ifndef __TSA_
		else if(GetPropID("OPER_PRE_BOARDCAL.hPoleGrad")==id)
			valueStr.Printf("0X%X",m_xOperData.board.hPoleGrad);
		else if(GetPropID("OPER_PRE_BOARDCAL.hPoleHori")==id)
			valueStr.Printf("0X%X",m_xOperData.board.hPoleHori);
		else if(GetPropID("OPER_PRE_BOARDCAL.vert_high")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.vert_high);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.hori_space")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.hori_space);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.grad_offset")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.grad_offset);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.left_hori_offset")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.left_hori_offset);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.corner_up.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.corner_up.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.corner_up.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.corner_up.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.corner_up.z")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.corner_up.z);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.corner_down.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.corner_down.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.corner_down.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.corner_down.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_PRE_BOARDCAL.corner_down.z")==id)
		{
			valueStr.Printf("%f",m_xOperData.board.corner_down.z);
			SimplifiedNumString(valueStr);
		}
#endif
		//ʸ��ƫ��
		else if(GetPropID("OPER_OFFSET_ORIGIN.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.offset_origin.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_OFFSET_ORIGIN.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.offset_origin.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_OFFSET_ORIGIN.z")==id)
		{
			valueStr.Printf("%f",m_xOperData.offset_origin.z);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_OFFSET.result.x")==id)
		{
			valueStr.Printf("%f",m_xOperData.result.x);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_OFFSET.result.y")==id)
		{
			valueStr.Printf("%f",m_xOperData.result.y);
			SimplifiedNumString(valueStr);
		}
		else if(GetPropID("OPER_OFFSET.result.z")==id) 
		{
			valueStr.Printf("%f",m_xOperData.result.z);
			SimplifiedNumString(valueStr);
		}
		else if(listOffset.GetNodeNum()>0)
		{
			TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pItem=NULL;
			for(pItem=listOffset.GetFirst();pItem;pItem=listOffset.GetNext())
			{
				if(pItem->iNo*VEC_OFFSET_ID_COEF+GetPropID("OPER_OFFSET_VEC.x")==id)
				{
					valueStr.Printf("%f",pItem->vec.x);
					SimplifiedNumString(valueStr);
					return (char*)valueStr;
				}
				else if(pItem->iNo*VEC_OFFSET_ID_COEF+GetPropID("OPER_OFFSET_VEC.y")==id)
				{
					valueStr.Printf("%f",pItem->vec.y);
					SimplifiedNumString(valueStr);
					return (char*)valueStr;
				}
				else if(pItem->iNo*VEC_OFFSET_ID_COEF+GetPropID("OPER_OFFSET_VEC.z")==id)
				{
					valueStr.Printf("%f",pItem->vec.z);
					SimplifiedNumString(valueStr);
					return (char*)valueStr;
				}	
				else if(pItem->iNo*VEC_OFFSET_ID_COEF+GetPropID("OPER_OFFSET_DIST")==id)
				{
					valueStr.Printf("%f",pItem->offset);
					SimplifiedNumString(valueStr);
					return (char*)valueStr;
				}
			}
		}
		else
			return "";
	}
	return (char*)valueStr;
}
/////////////////////////////////////////////////////////////////////////////
// CToolKitDlg message handlers

BOOL CToolKitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	memset(&m_xOperData,0,sizeof(m_xOperData));
	m_xOperData.arc_lift.rot_axis='Y';	//Ĭ����ת��ΪY��

	m_tabCtrl.DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	m_tabCtrl.InsertItem(0,"Vector Calculation");	//���������,�㵽��ʸ��,ʸ���н�,ʸ��ƫ��
	m_tabCtrl.InsertItem(1,"Coordinate Convertion");	//ֱ������ϵת��,������ת��
	m_tabCtrl.InsertItem(2,"Space Calculation");	//���㶨��,������,������,���������
	m_tabCtrl.InsertItem(3,"Prelife Calculation");	//Բ��Ԥ��,ƽ̨����
#else 
	m_tabCtrl.InsertItem(0,"ʸ������");	//���������,�㵽��ʸ��,ʸ���н�,ʸ��ƫ��
	m_tabCtrl.InsertItem(1,"����ת��");	//ֱ������ϵת��,������ת��
	m_tabCtrl.InsertItem(2,"�ռ����");	//���㶨��,������,������,���������
	m_tabCtrl.InsertItem(3,"Ԥ������");	//Բ��Ԥ��,ƽ̨����
#endif

	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	InitToolkitsPropHashtable();
	DisplayItemList();
	RefreshTabCtrl(0);
	ResetDlgSize(0);
	
	return TRUE;
}

void CToolKitDlg::InsertPointPropItem(CPropTreeItem *pParentItem,char *sIdProp,
									  char *sPropName,BOOL bHideChild/*=FALSE*/)
{
	char prop_str[100]="";
	CItemInfo *lpInfo = NULL;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_strPropName = sPropName;
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pParentItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID(sIdProp);
	pPropItem->m_bHideChildren=bHideChild;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup; 
	//X����
	sprintf(prop_str,"%s.x",sIdProp);
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID(prop_str);
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup; 
	//Y����
	sprintf(prop_str,"%s.y",sIdProp);
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID(prop_str);
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup; 
	//Z����
	sprintf(prop_str,"%s.z",sIdProp);
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID(prop_str);
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup; 
}

void CToolKitDlg::DisplayItemList()
{
	CPropTreeItem* pRootItem=m_propList.GetRootItem();
	CPropTreeItem* pGroupItem, *pPropItem,*pSonPropItem,*pGradSonPropItem;
	const int GROUP_VECCAL=1,GROUP_CSTRANS=2,GROUP_3DCAL=3,GROUP_PREVLIFT=4;
	m_propList.SetModifyValueFunc(ModifyToolKitsProperty);
	m_propList.SetButtonClickFunc(ToolKitsButtonClick);
	m_propList.SetModifyStatusFunc(ToolKitsItemStatus);
	m_propList.SetPropHelpPromptFunc(FireSelChange);
	m_propList.CleanTree();
	//һ���������
	CItemInfo* lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Two Point Distance";
#else 
	lpInfo->m_strPropName = "�������";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_VEC_LEN");
	pGroupItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	//���λ��
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_VEC_LEN.start","Start Position");
	//�յ�λ��
	InsertPointPropItem(pGroupItem,"OPER_VEC_LEN.end","End Position");
#else 
	InsertPointPropItem(pGroupItem,"OPER_VEC_LEN.start","���λ��");
	//�յ�λ��
	InsertPointPropItem(pGroupItem,"OPER_VEC_LEN.end","�յ�λ��");
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "2 Point Distance Calculation Result";
#else 
	lpInfo->m_strPropName = "������������";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_sButtonName = "����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_VEC_LEN.result");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	//����ʸ���н�
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Vector Angle";
#else 
	lpInfo->m_strPropName = "ʸ���н�";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_VEC_ANGLE");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	//����1
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_VEC_ANGLE.vec1","Vector 1");
	//����2
	InsertPointPropItem(pGroupItem,"OPER_VEC_ANGLE.vec2","Vector 2");
#else 
	InsertPointPropItem(pGroupItem,"OPER_VEC_ANGLE.vec1","����1");
	//����2
	InsertPointPropItem(pGroupItem,"OPER_VEC_ANGLE.vec2","����2");
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Two Vector Angle(��)";
	lpInfo->m_sButtonName = "Calculation";
#else 
	lpInfo->m_strPropName = "�������н�(��)";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMONEDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_VEC_ANGLE.deg_angle");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	pPropItem->m_bHideChildren=FALSE;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Two Vector Angle(Arc Angle)";
#else 
	lpInfo->m_strPropName = "�������н�(����)";
#endif
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_VEC_ANGLE.rad_angle");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	pSonPropItem->SetReadOnly();
	//�����㵽��ʸ��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Point To Point Vector";
#else 
	lpInfo->m_strPropName = "�㵽��ʸ��";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_VEC_P2P");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	//���λ��
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_VEC_P2P.start","Start Position");
	//�յ�λ��
	InsertPointPropItem(pGroupItem,"OPER_VEC_P2P.end","End Position");
#else 
	InsertPointPropItem(pGroupItem,"OPER_VEC_P2P.start","���λ��");
	//�յ�λ��
	InsertPointPropItem(pGroupItem,"OPER_VEC_P2P.end","�յ�λ��");
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Result";
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "������";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_VEC_P2P.CalResult");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	//
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pPropItem,"OPER_VEC_P2P.result","Point To Point Vector");	
#else 
	InsertPointPropItem(pPropItem,"OPER_VEC_P2P.result","�㵽��ʸ��");	
#endif
	//�ġ�ʸ��ƫ��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Vector Offset";
#else 
	lpInfo->m_strPropName = "ʸ��ƫ��";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_OFFSET");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	//���λ��
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_OFFSET_ORIGIN","Vector Offset Start");
#else 
	InsertPointPropItem(pGroupItem,"OPER_OFFSET_ORIGIN","ʸ��ƫ����ʼ��");
#endif
	//���ƫ����
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Offset List";
	lpInfo->m_sButtonName = "Add";
#else 
	lpInfo->m_strPropName = "ƫ�����б�";
	lpInfo->m_sButtonName = "���";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_OFFSET.OFFSETLIST");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL);
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Result";
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "������";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_OFFSET.CalResult");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_VECCAL); 
	//
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pPropItem,"OPER_OFFSET.result","Vector Offset Result");
#else 
	InsertPointPropItem(pPropItem,"OPER_OFFSET.result","ʸ��ƫ�ƽ��");
#endif
	//�塢����ת��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "CSYS Convertion";
#else 
	lpInfo->m_strPropName = "����ϵת��";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_CS_TRANS");
	pGroupItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_CSTRANS); 
	//����ϵ
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "CSYS";
	lpInfo->m_sButtonName = "Edit";
#else 
	lpInfo->m_strPropName = "����ϵ";
	lpInfo->m_sButtonName = "�༭";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_CS_TRANS.cs");
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_CSTRANS);
	//��ת������
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_CS_TRANS.src","To Convert Coord");
#else 
	InsertPointPropItem(pGroupItem,"OPER_CS_TRANS.src","��ת������");
#endif
	//����->���
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Absolute->Relative";
	lpInfo->m_sButtonName = "Absolute->Relative";
#else 
	lpInfo->m_strPropName = "����->���";
	lpInfo->m_sButtonName = "����->���";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_CS_TRANS.wcs2ucs");
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_CSTRANS);
	//���->����
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Relative->Absolute";
	lpInfo->m_sButtonName = "Relative->Absolute";
#else 
	lpInfo->m_strPropName = "���->����";
	lpInfo->m_sButtonName = "���->����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_CS_TRANS.ucs2wcs");
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_CSTRANS);
	//ת�����
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_CS_TRANS.dest","Convert Result");
#else 
	InsertPointPropItem(pGroupItem,"OPER_CS_TRANS.dest","ת�����");
#endif
	//�������㹲��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "3 Point Plane";
#else 
	lpInfo->m_strPropName = "����ƽ��";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_FACE3P");
	pGroupItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_3D_FACE3P.p[0]","Datum Point 1");
	InsertPointPropItem(pGroupItem,"OPER_3D_FACE3P.p[1]","Datum Point 2",TRUE);
	InsertPointPropItem(pGroupItem,"OPER_3D_FACE3P.p[2]","Datum Point 3",TRUE);
#else 
	InsertPointPropItem(pGroupItem,"OPER_3D_FACE3P.p[0]","��׼��1");
	InsertPointPropItem(pGroupItem,"OPER_3D_FACE3P.p[1]","��׼��2",TRUE);
	InsertPointPropItem(pGroupItem,"OPER_3D_FACE3P.p[2]","��׼��3",TRUE);
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Result";
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "������";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_FACE3P.CalPlane");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//����ƽ��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Copy Plane";
	lpInfo->m_sButtonName = "Copy Plane";
#else 
	lpInfo->m_strPropName = "����ƽ��";
	lpInfo->m_sButtonName = "����ƽ��";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_FACE3P.CopyPlane");
	pSonPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//ƽ�淨��
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pPropItem,"OPER_3D_FACE3P.face_norm","Plane Normal");
#else 
	InsertPointPropItem(pPropItem,"OPER_3D_FACE3P.face_norm","�淨��");
#endif
	//�ߡ����߽���
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Line&Line Intersection";
#else 
	lpInfo->m_strPropName = "���߽���";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_INT_LL");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//���
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line1.start","Line 1 Start",TRUE);
	//�յ�
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line1.end","Line 1 End",TRUE);
	//ֱ��2���
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line2.start","Line 2 Start",TRUE);
	//ֱ��2�յ�
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line2.end","Line 2 End",TRUE);
#else 
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line1.start","ֱ��1���",TRUE);
	//�յ�
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line1.end","ֱ��1�յ�",TRUE);
	//ֱ��2���
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line2.start","ֱ��2���",TRUE);
	//ֱ��2�յ�
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LL.line2.end","ֱ��2�յ�",TRUE);
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Intersection";
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "���㽻��";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_INT_LL.CalInters");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//����
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LL.result","Two Line Intersection");
	//�ˡ����潻��
	lpInfo = new CItemInfo();
	lpInfo->m_strPropName = "Line&Plane Intersection";
#else 
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LL.result","��ֱ�߽���");
	//�ˡ����潻��
	lpInfo = new CItemInfo();
	lpInfo->m_strPropName = "���潻��";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_INT_LF");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//ֱ�����
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LF.line.start","Line Start",TRUE);
	//ֱ���յ�
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LF.line.end","Line End",TRUE);
#else 
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LF.line.start","ֱ�����",TRUE);
	//ֱ���յ�
	InsertPointPropItem(pGroupItem,"OPER_3D_INT_LF.line.end","ֱ���յ�",TRUE);
#endif
	//ƽ��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Plane";
	lpInfo->m_sButtonName ="Paste Plane";
#else 
	lpInfo->m_strPropName = "ƽ��";
	lpInfo->m_sButtonName ="ճ��ƽ��";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_INT_LF.plane");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//�����
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LF.face_pos","Plane Datum Point");
	//�淨��
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LF.face_norm","Plane Normal");
#else 
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LF.face_pos","����");
	//�淨��
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LF.face_norm","����");
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Intersection";
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "���㽻��";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_INT_LF.CalInters");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//����
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LF.result","Line&Plane Intersection");
#else 
	InsertPointPropItem(pPropItem,"OPER_3D_INT_LF.result","���潻��");
#endif
	//�š����߾���
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Point&Line Distance";
#else 
	lpInfo->m_strPropName = "���߾���";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_DIST_PL");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PL.pt","Datum Point");
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PL.line_start","Line Start");
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PL.line_end","Line End");
#else 
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PL.pt","��׼��");
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PL.line_start","ֱ�����");
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PL.line_end","ֱ���յ�");
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Distance";
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "�������";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_DIST_PL.result");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//ʮ���������
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Point&Plane Distance";
#else 
	lpInfo->m_strPropName = "�������";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_DIST_PF");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PF.pt","Datum Point");
#else 
	InsertPointPropItem(pGroupItem,"OPER_3D_DIST_PF.pt","��׼��");
#endif
	//ƽ��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Plane";
	lpInfo->m_sButtonName ="Paste Plane";
#else 
	lpInfo->m_strPropName = "ƽ��";
	lpInfo->m_sButtonName ="ճ��ƽ��";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_DIST_PF.plane");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//
#ifdef AFX_TARG_ENU_ENGLISH
	InsertPointPropItem(pPropItem,"OPER_3D_DIST_PF.face_pos","Datum Point");
	InsertPointPropItem(pPropItem,"OPER_3D_DIST_PF.face_norm","Normal");
#else 
	InsertPointPropItem(pPropItem,"OPER_3D_DIST_PF.face_pos","����");
	InsertPointPropItem(pPropItem,"OPER_3D_DIST_PF.face_norm","����");
#endif
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Distance";
#else 
	lpInfo->m_strPropName = "�������";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_sButtonName = "����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_3D_DIST_PF.result");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_3DCAL); 
	//ʮһ��Բ��Ԥ��
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Arc Prelift";
#else 
	lpInfo->m_strPropName = "Բ��Ԥ��";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT");
	pGroupItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Rotate Axis";
#else 
	lpInfo->m_strPropName = "��ת��";
#endif
	lpInfo->m_controlType=PIT_COMBO;
	lpInfo->m_cmbType=CDT_LIST;
	lpInfo->m_cmbItems="X|Y";
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_axis");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Prelift Height";
#else 
	lpInfo->m_strPropName = "Ԥ���߶�";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_height");
	pPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Prelife Rotate Datum Point";
#else 
	lpInfo->m_strPropName = "Ԥ����ת����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center");
	pPropItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.rot_center.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Start Of Datum Prelife Point";
#else 
	lpInfo->m_strPropName = "��׼Ԥ�����ʼλ��";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos");
	pPropItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.lift_pos.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Start Of Prelife Point";
#else 
	lpInfo->m_strPropName = "Ԥ�����ʼλ��";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick.x");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick.y");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.pick.z");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Result";
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "������";
	lpInfo->m_sButtonName = "����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.CalResult");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 

	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Position After Prelife";
#else 
	lpInfo->m_strPropName = "Ԥ����λ��";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result");
	pSonPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.x");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.y");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_ARCLIFT.result.z");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
#ifndef __TSA_
	//ʮ�����ᵣƽ̨����
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Arm Platform Calculate";
#else 
	lpInfo->m_strPropName = "�ᵣƽ̨����";
#endif
	pGroupItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pGroupItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL");
	pGroupItem->m_bHideChildren=TRUE;
	if(CToolKitDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pGroupItem->m_idProp);
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
		//�������
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Input Parameter";
#else 
	lpInfo->m_strPropName = "�������";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.InPutParam");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Diagonal Rod";
#else 
	lpInfo->m_strPropName = "б���˼�";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMONEDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleGrad");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Horizontal Rod";
#else 
	lpInfo->m_strPropName = "ˮƽ�˼�";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMONEDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleHori");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Vertical Height";
#else 
	lpInfo->m_strPropName = "��ֱ�߶�";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.vert_high");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Horizontal Space";
#else 
	lpInfo->m_strPropName = "�����϶";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hori_space");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
		//������
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Calculate Result";
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_sButtonName = "Calculate";
#else 
	lpInfo->m_strPropName = "������";
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_sButtonName = "����";
#endif
	pPropItem = m_propList.InsertItem(pGroupItem,lpInfo,-1);
	pPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.CalResult");
	pPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pPropItem->m_idProp)!=-1)
		pPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pPropItem->m_idProp);
	pPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Diagonal Rod Offset";
#else 
	lpInfo->m_strPropName = "б����ƫ����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.grad_offset");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Horizontal Rod Offset";
#else 
	lpInfo->m_strPropName = "ˮƽ��ƫ����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.left_hori_offset");
	pSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Up Right Corner Position Of Datum Point";
	lpInfo->m_strPropHelp = "The locate datum point of up horizontal rod on the right center line";
#else 
	lpInfo->m_strPropName = "���Ͻǻ���λ��";
	lpInfo->m_strPropHelp = "�Ϻ�˽Ǹֹ���֫�Ҳ����߶�λ����";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up");
	pSonPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.x");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.y");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_up.z");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	//
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Down Right Corner Position Of Ridge Vertex";
	lpInfo->m_strPropHelp = "The locate datum point of down main angle on the right side";
#else 
	lpInfo->m_strPropName = "���½����λ��";
	lpInfo->m_strPropHelp = "ƽ̨�²����Ǹ��Ҳඨλ���";
#endif
	lpInfo->m_controlType=PIT_BUTTON;
	pSonPropItem = m_propList.InsertItem(pPropItem,lpInfo,-1);
	pSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down");
	pSonPropItem->m_bHideChildren=FALSE;
	if(CToolKitDlg::GetPropStatus(pSonPropItem->m_idProp)!=-1)
		pSonPropItem->m_bHideChildren=CToolKitDlg::GetPropStatus(pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Coordinate";
#else 
	lpInfo->m_strPropName = "X����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.x");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Coordinate";
#else 
	lpInfo->m_strPropName = "Y����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.y");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
	
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Coordinate";
#else 
	lpInfo->m_strPropName = "Z����";
#endif
	lpInfo->m_controlType=PIT_EDIT;
	pGradSonPropItem = m_propList.InsertItem(pSonPropItem,lpInfo,-1);
	pGradSonPropItem->m_idProp = CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.corner_down.z");
	pGradSonPropItem->m_lpNodeInfo->m_strPropValue = GetPropValueStr(pGradSonPropItem->m_idProp);
	pGradSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_PREVLIFT); 
#endif
	m_propList.Redraw();
}
void CToolKitDlg::RefreshTabCtrl(int iCurSel)
{
	if(m_tabCtrl.GetItemCount()>0)
	{
		m_tabCtrl.SetCurSel(iCurSel);
		m_propList.m_iPropGroup=iCurSel;
	}
	else //����Ҫ������ʾ
		m_propList.m_iPropGroup=0;

	m_propList.Redraw();
}

void CToolKitDlg::ResetDlgSize(int iCurSel)
{	//�������ڴ�С
	CRect rc;
	GetWindowRect(rc);
	int offset_dist=15;
	if(iCurSel==3)
		rc.bottom = rc.top+588;
	else 
		rc.bottom = rc.top+588-148;
	MoveWindow(rc);	
}

void CToolKitDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel = m_tabCtrl.GetCurSel();
	RefreshTabCtrl(iCurSel);
	ResetDlgSize(iCurSel);
	*pResult = 0;
}

BOOL CToolKitDlg::Create()
{
	return CDialog::Create(CToolKitDlg::IDD);
}

void CToolKitDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;	//ɾ���Լ���ռ��Դ
}

int CToolKitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Center the window.
	CenterWindow();	
	return 0;
}

void CToolKitDlg::OnOK() 
{
	//��Ļ�������а��س����رնԻ���Ĳ���
}

void CToolKitDlg::OnClose() 
{
	CDialog::OnOK();
}

void CToolKitDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
#ifndef __TSA_
	if(GetEventPropId()==CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleGrad"))
	{	//б���˼�
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
			m_xOperData.board.hPoleGrad=pLinePart->handle;
	}
	else if(GetEventPropId()==CToolKitDlg::GetPropID("OPER_PRE_BOARDCAL.hPoleHori"))
	{	//ˮƽ�˼�
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_hPickObj,CLS_LINEPART);
		if(pLinePart)
			m_xOperData.board.hPoleHori=pLinePart->handle;	
	}
	//���²��������ݲ��ָ�ѡ����
	CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
	m_propList.SetFocus();
	if(pItem)
	{
		m_propList.SetItemPropValue(m_idEventProp,GetPropValueStr(m_idEventProp));	//������ʾ����
		m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����
	}
#endif
}

TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *CToolKitDlg::AddVecOffsetItem(f3dPoint vec,double offse_dist)
{
	TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pOffsetItem = listOffset.append();
	pOffsetItem->iNo = m_iMinAvailable;
	pOffsetItem->vec = vec;
	pOffsetItem->offset = offse_dist;
	m_iMinAvailable++;
	return pOffsetItem;
}

f3dPoint CToolKitDlg::CalVecOffsetResult()
{
	m_xOperData.result=m_xOperData.offset_origin;
	TOOLKIT_OPER_DATA::OPER_VEC_OFFSET *pItem=NULL;
	for(pItem=listOffset.GetFirst();pItem;pItem=listOffset.GetNext())
	{
		f3dPoint vec=pItem->vec;
		normalize(vec);
		m_xOperData.result+=vec*pItem->offset;
	}
	return m_xOperData.result;
}

void CToolKitDlg::UpdateSketchMap(CPropTreeItem *pItem)
{
	const int GROUP_VECCAL=1,GROUP_CSTRANS=2,GROUP_3DCAL=3,GROUP_PREVLIFT=4;
	if(pItem==NULL||pItem->m_dwPropGroup!=GetSingleWord(GROUP_PREVLIFT))
		return;
	m_xCurPlateBmp.Detach();
	if(GetPropID("OPER_PRE_ARCLIFT")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.rot_axis")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.lift_height")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.rot_center")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.rot_center.x")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.rot_center.y")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.rot_center.z")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.lift_pos")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.lift_pos.x")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.lift_pos.y")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.lift_pos.z")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.pick")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.pick.x")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.pick.y")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.pick.z")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.CalResult")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.result")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.result.x")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.result.y")==pItem->m_idProp
		||GetPropID("OPER_PRE_ARCLIFT.result.z")==pItem->m_idProp)
	{
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_ARC_LIFT);
	}
	else if(GetPropID("OPER_PRE_BOARDCAL")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.InPutParam")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.hPoleGrad")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.hPoleHori")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.vert_high")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.hori_space")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.CalResult")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.grad_offset")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.left_hori_offset")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_up")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_up.x")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_up.y")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_up.z")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_down")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_down.x")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_down.y")==pItem->m_idProp
		||GetPropID("OPER_PRE_BOARDCAL.corner_down.z")==pItem->m_idProp)
	{
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_BOARD_CAL);
	}
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_SKETCH_MAP);
	pPic->SetBitmap(m_xCurPlateBmp);
}