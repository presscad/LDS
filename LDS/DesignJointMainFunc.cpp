//<LOCALE_TRANSLATE Confirm by hxr/>

// DesignJoint.cpp: implementation of the CDesignJoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tower.h"
#include "LDS.h"
#include "dlg.h"
#include "Query.h"
#include "DesignJoint.h"
#include "GlobalFunc.h"
#include "EditJointJgZhunJuDlg.h" 
#include "JointDesParaDlg.h"
#include "JointBaseJg.h"
#include "env_def.h"
#include "XhCharString.h"
#include "DesignMultiLeg.h"	//CMappingBolts defined

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if !defined(__TSA_)&&!defined(__TSA_FILE_)

int deita = 20, S1 = 190;//S1Ϊ������,��ʵӦ�ɵ�ǰ�ڵ�Ľڵ��ȷ���Ժ�Ӧ����//////////////////////////////////////////////////////////////////////
//�������ͷ���
extern CJointDesParaDlg JointDesDlg;
extern CEditJointJgZhunJuDlg zhunjudlg;
// ���ý�ͷ�����ĵ��²��׼�Ǹ��ϵ���˨,ͬʱҲ����˨������Ӧ�İ�������ְ���
static f3dPoint ls_ptx,ls_pty,ls_ptx1,ls_ptx2,ls_pty1,ls_pty2;
static f3dPoint tm_pt;
static f3dLine lineTemp;
static BOOL bXOutLsLow;	//X֫�����˨�Ƿ��´����
static DWORD dwRayNo;
//��ʼ���Ǹ���˨�Ķ�λ����
void CDesignJoint::InitJgLsDesPosPara(CLDSBolt* pLs,CLDSLineAngle* pMainJg,char cWorkWing,double wing_off_dist,BOOL bSingRow/*=TRUE*/)
{
	if(pLs==NULL || pMainJg==NULL)
		return;
	JGZJ defaultZJ=pMainJg->GetZhunJu(cWorkWing);
	BYTE ciOffStyle=4;
	if(wing_off_dist==defaultZJ.g && bSingRow)
		ciOffStyle=0;
	else if(wing_off_dist==defaultZJ.g1 && !bSingRow)
		ciOffStyle=1;
	else if(wing_off_dist==defaultZJ.g2 && !bSingRow)
		ciOffStyle=2;
	else if(wing_off_dist==defaultZJ.g3 && !bSingRow)
		ciOffStyle=3;
	//��׼�����
	pLs->des_base_pos.datumPoint.datum_pos_style=1;
	pLs->des_base_pos.datumPoint.des_para.RODEND.hRod=pMainJg->handle;
	pLs->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
	pLs->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
	pLs->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
	pLs->des_base_pos.datumPoint.des_para.RODEND.direction=(BYTE)pMainJg->feature;
	pLs->des_base_pos.datumPoint.des_para.RODEND.offset_wing=(cWorkWing=='X')?0:1;
	//λ����Ʋ���
	pLs->des_base_pos.hPart=pMainJg->handle;
	pLs->des_base_pos.offset_wing=(cWorkWing=='X')?0:1;
	pLs->des_base_pos.wing_offset_dist=wing_off_dist;
	pLs->des_base_pos.m_biWingOffsetStyle=ciOffStyle;
	pLs->des_base_pos.direction=(BYTE)pMainJg->feature;
	//�������
	pLs->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���� wht 10-09-30
	pLs->des_work_norm.hVicePart=pMainJg->handle;
	pLs->des_work_norm.direction=0;
	pLs->des_work_norm.norm_wing=pLs->des_base_pos.offset_wing;
}
bool CDesignJoint::LayDownBaseJgLs()
{
	int j;
	CLDSBolt ls(console.GetActiveModel()),*pLs;
	UCS_STRU ucs = m_pJoint->ucs;
	//����ֱ�Ӹ�����˨ͨ��(δָ����������)�����½�ͷ������ƺ���˨��λ  wxc-2015.6.9
	//ls.AddL0Thick((long)GetLsL0(false),FALSE);
	ls.set_d(ls_group1.d);
	if(!ls.CalGuigeAuto())
	{
		CXhChar100 sError;
#ifdef AFX_TARG_ENU_ENGLISH
		sError.Printf("bolt specification in accordance with M%dX%.f not found in bolt specification library!",ls.get_d(),ls.get_L0());
#else
		sError.Printf("����˨������û���ҵ�����M%dX%.f����˨���!",ls.d,ls.L0);
#endif
		throw (char*)sError;
	}
	char cWorkWing='X';
	lineTemp.startPt=xDesJointPara.m_pMainJg1->Start();
	lineTemp.endPt  =xDesJointPara.m_pMainJg1->End();
	if(ls_group1.rows==1)//������˨
	{
		ls.dwRayNo = GetSingleWord(dwRayNo);
		dwRayNo++;
		for(j=0;j< ls_group1.wnConnBoltN;j++)
		{
			if(j==0)
			{
				if(m_nLsLHigh0RLow1==0)	//��˨X֫(��֫)�´����
				{
					ls_ptx = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,LsSpace1.EndSpace,jgzj1.g);
					ls_pty = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,(LsSpace1.EndSpace+LsSpace1.SingleRowSpace/2),jgzj1.g);
				}
				else	//��˨Y֫(��֫)�´����
				{
					ls_ptx = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,(LsSpace1.EndSpace+LsSpace1.SingleRowSpace/2),jgzj1.g);
					ls_pty = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,LsSpace1.EndSpace,jgzj1.g);
				}
			}
			else
			{
				ls_ptx+=LsSpace1.SingleRowSpace*ucs.axis_z;
				ls_pty+=LsSpace1.SingleRowSpace*ucs.axis_z;
			}
			ls.set_norm(xDesJointPara.norm_x_wing.vector);
			if(m_pJoint->joint_style==0)//���ڰ���
			{
				ls.ucs.origin=ls_ptx-ls.get_norm()*base_wing_thick1;
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",xDesJointPara.m_pMainJg1->handle);
			}
			else if(m_pJoint->joint_style==3)	//�ڰ��ְ������
			{
				ls.ucs.origin=ls_ptx-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pXPlate->handle);
			}
			else						//���ڰ���
			{
				ls.ucs.origin=ls_ptx-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pInWrapJg->handle);
			}
			pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
			m_pJoint->boltRefList.append(pLs->GetLsRef());
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��

			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'Y':'X';
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,jgzj1.g);
			tm_pt=pLs->ucs.origin;
			SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
			if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
			}
			else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
			}

			if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
			if(pXPadPlate&&base_wing_thick1<base_wing_thick2)
			{
				pXPadPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPadPlate->handle,TRUE);
			}
			if(m_pJoint->joint_style==0)		//�����
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==1)	//�������
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
			{
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				pXPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPlate->handle,TRUE);
			}
			else								//�ڰ��ְ������
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pXPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPlate->handle,TRUE);
			}
			//
			ls.set_norm(xDesJointPara.norm_y_wing.vector);
			if(m_pJoint->joint_style==0)//���ڰ���
			{
				ls.ucs.origin=ls_pty-ls.get_norm()*base_wing_thick1;
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
			}
			else if(m_pJoint->joint_style==3)	//�ڰ��ְ������
			{
				ls.ucs.origin=ls_pty-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
			}
			else						//���ڰ���
			{
				ls.ucs.origin=ls_pty-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
			}
			pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
			m_pJoint->boltRefList.append(pLs->GetLsRef());
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'X':'Y';
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,jgzj1.g);
			tm_pt=pLs->ucs.origin;
			SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
			if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
			}
			else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
			}
			if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
			if(pYPadPlate&&base_wing_thick1<base_wing_thick2)
			{
				pYPadPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPadPlate->handle,TRUE);
			}
			if(m_pJoint->joint_style==0)		//�����
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==1)	//�������
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
			{
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				pYPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPlate->handle,TRUE);
			}
			else
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pYPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPlate->handle,TRUE);
			}
		}
	}
	else	//˫����˨
	{
		ls.dwRayNo = GetSingleWord(dwRayNo);
		dwRayNo++;	//��˨���ߺ�
		for(j=0;j< ls_group1.wnConnBoltN;j+=2)
		{
			if(j==0)
			{
				if(ls_group1.wnConnBoltN %2==0)//ż������˨
				{	//���µ�X֫��Y֫����Խ�ͷ�������ϵucs��˵��
					if(m_nLsLHigh0RLow1==0)	//�ڲ���˨X֫�ϴ����
					{
						ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							(LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2),jgzj1.g1);
						ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g2);
						ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g1);
						ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							(LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2),jgzj1.g2);
						bXOutLsLow = FALSE;
					}
					else	//�ڲ���˨Y֫�ϴ����
					{
						ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g1);
						ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2,jgzj1.g2);
						ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2,jgzj1.g1);
						ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g2);
						bXOutLsLow = TRUE;
					}
				}
				else	//��������˨
				{
					if(m_nLsLHigh0RLow1==0)	//�ڲ���˨Y֫�ϴ����
					{
						ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g1);
						ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2,jgzj1.g2);
						ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2,jgzj1.g1);
						ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g2);
						bXOutLsLow = FALSE;
					}
					else	//�ڲ���˨X֫�ϴ����
					{
						ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2,jgzj1.g1);
						ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g2);
						ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							LsSpace1.EndSpace,jgzj1.g1);
						ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
							LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2,jgzj1.g2);
						bXOutLsLow = TRUE;
					}
				}
				ls.set_norm(xDesJointPara.norm_x_wing.vector);
				ls.dwRayNo = GetSingleWord(1);	//��˨���ߺ�
				if(m_pJoint->joint_style==0)//���ڰ���
				{
					ls.ucs.origin=ls_ptx1-ls.get_norm()*base_wing_thick1;
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
				}
				else if(m_pJoint->joint_style==3)	//�ڰ��ְ������
				{
					ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
				}
				else						//���ڰ���
				{
					ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
				}
				pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
				m_pJoint->boltRefList.append(pLs->GetLsRef());
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'Y':'X';
				InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,jgzj1.g1,FALSE);
				tm_pt=pLs->ucs.origin;
				SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
				if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
				if(pXPadPlate&&base_wing_thick1<base_wing_thick2)
				{
					pXPadPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPadPlate->handle,TRUE);
				}
				if(m_pJoint->joint_style==0)		//�����
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==1)	//�������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
				{
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					pXPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPlate->handle,TRUE);
				}
				else								//�ڰ����������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pXPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPlate->handle,TRUE);
				}
				if(m_pJoint->joint_style==0)//���ڰ���
				{
					ls.ucs.origin=ls_ptx2-ls.get_norm()*base_wing_thick1;
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
				}
				else if(m_pJoint->joint_style==3)
				{
					ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
				}
				else						//���ڰ���
				{
					ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
				}
				ls.dwRayNo = GetSingleWord(2);	//��˨���ߺ�
				pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				m_pJoint->boltRefList.append(pLs->GetLsRef());
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'Y':'X';
				InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,jgzj1.g2,FALSE);
				tm_pt=pLs->ucs.origin;
				SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
				if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
				if(pXPadPlate&&base_wing_thick1<base_wing_thick2)
				{
					pXPadPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPadPlate->handle,TRUE);
				}
				if(m_pJoint->joint_style==0)		//�����
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==1)	//�������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
				{
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					pXPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPlate->handle,TRUE);
				}
				else								//�ڰ����������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pXPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPlate->handle,TRUE);
				}
				ls.set_norm(xDesJointPara.norm_y_wing.vector);
				ls.dwRayNo = GetSingleWord(3);	//��˨���ߺ�
				if(m_pJoint->joint_style==0)//���ڰ���
				{
					ls.ucs.origin=ls_pty1-ls.get_norm()*base_wing_thick1;
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
				}
				else if(m_pJoint->joint_style==3)
				{
					ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
				}
				else						//���ڰ���
				{
					ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
				}
				pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
				m_pJoint->boltRefList.append(pLs->GetLsRef());
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'X':'Y';
				InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,jgzj1.g1,FALSE);
				tm_pt=pLs->ucs.origin;
				SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
				if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
				if(pYPadPlate&&base_wing_thick1<base_wing_thick2)
				{
					pYPadPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPadPlate->handle,TRUE);
				}
				if(m_pJoint->joint_style==0)		//�����
				{	
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==1)	//�������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
				{
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					pYPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPlate->handle,TRUE);
				}
				else								//�ڰ����������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pYPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPlate->handle,TRUE);
				}
				ls.dwRayNo = GetSingleWord(4);	//��˨���ߺ�
				if(m_pJoint->joint_style==0)//���ڰ���
				{
					ls.ucs.origin=ls_pty2-ls.get_norm()*base_wing_thick1;
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
				}
				else if(m_pJoint->joint_style==3)
				{
					ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
				}
				else						//���ڰ���
				{
					ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
				}
				pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				m_pJoint->boltRefList.append(pLs->GetLsRef());
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'X':'Y';
				InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,jgzj1.g2,FALSE);
				tm_pt=pLs->ucs.origin;
				SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
				if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
				if(pYPadPlate&&base_wing_thick1<base_wing_thick2)
				{
					pYPadPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPadPlate->handle,TRUE);
				}
				if(m_pJoint->joint_style==0)		//�����
				{	
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==1)	//�������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
				{
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					pYPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPlate->handle,TRUE);
				}
				else
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pYPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPlate->handle,TRUE);
				}
			}
			else
			{
				ls.set_norm(xDesJointPara.norm_x_wing.vector);
				if(bXOutLsLow)
				{
					Add_Pnt(ls_ptx2,ls_ptx2,LsSpace1.doubleRowSpace*ucs.axis_z);
					Add_Pnt(ls_pty1,ls_pty1,LsSpace1.doubleRowSpace*ucs.axis_z);
				}
				else
				{
					Add_Pnt(ls_ptx1,ls_ptx1,LsSpace1.doubleRowSpace*ucs.axis_z);
					Add_Pnt(ls_pty2,ls_pty2,LsSpace1.doubleRowSpace*ucs.axis_z);
				}
				if(m_pJoint->joint_style==0)//���ڰ���
				{
					if(bXOutLsLow)
					{
						ls.dwRayNo = GetSingleWord(2);	//��˨���ߺ�
						ls.ucs.origin=ls_ptx2-ls.get_norm()*base_wing_thick1;
						ls.des_base_pos.wing_offset_dist=jgzj1.g2;
					}
					else
					{
						ls.dwRayNo = GetSingleWord(1);	//��˨���ߺ�
						ls.ucs.origin=ls_ptx1-ls.get_norm()*base_wing_thick1;
						ls.des_base_pos.wing_offset_dist=jgzj1.g1;
					}
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
				}
				else if(m_pJoint->joint_style==3)
				{
					if(bXOutLsLow)
					{
						ls.dwRayNo = GetSingleWord(2);	//��˨���ߺ�
						ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
						ls.des_base_pos.wing_offset_dist=jgzj1.g2;
					}
					else
					{
						ls.dwRayNo = GetSingleWord(1);	//��˨���ߺ�
						ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
						ls.des_base_pos.wing_offset_dist=jgzj1.g1;
					}
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
				}
				else						//���ڰ���
				{
					if(bXOutLsLow)
					{
						ls.dwRayNo = GetSingleWord(2);	//��˨���ߺ�
						ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
						ls.des_base_pos.wing_offset_dist=jgzj1.g2;
					}
					else
					{
						ls.dwRayNo = GetSingleWord(1);	//��˨���ߺ�
						ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
						ls.des_base_pos.wing_offset_dist=jgzj1.g1;
					}
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
				}
				pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				m_pJoint->boltRefList.append(pLs->GetLsRef());
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'Y':'X';
				InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,ls.des_base_pos.wing_offset_dist,FALSE);
				tm_pt=pLs->ucs.origin;
				SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
				if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
				if(pXPadPlate&&base_wing_thick1<base_wing_thick2)
				{
					pXPadPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPadPlate->handle,TRUE);
				}
				if(m_pJoint->joint_style==0)		//�����
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==1)	//�������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
				{
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					pXPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPlate->handle,TRUE);
				}
				else
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pXPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pXPlate->handle,TRUE);
				}
				ls.set_norm(xDesJointPara.norm_y_wing.vector);
				if(m_pJoint->joint_style==0)//���ڰ������а��ְ�
				{
					if(bXOutLsLow)
					{
						ls.dwRayNo = GetSingleWord(3);	//��˨���ߺ�
						ls.ucs.origin=ls_pty1-ls.get_norm()*base_wing_thick1;
						ls.des_base_pos.wing_offset_dist = jgzj1.g1;
					}
					else
					{
						ls.dwRayNo = GetSingleWord(4);	//��˨���ߺ�
						ls.ucs.origin=ls_pty2-ls.get_norm()*base_wing_thick1;
						ls.des_base_pos.wing_offset_dist = jgzj1.g2;
					}
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
				}
				else if(m_pJoint->joint_style==3)
				{
					if(bXOutLsLow)
					{
						ls.dwRayNo = GetSingleWord(3);	//��˨���ߺ�
						ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
						ls.des_base_pos.wing_offset_dist = jgzj1.g1;
					}
					else
					{
						ls.dwRayNo = GetSingleWord(4);	//��˨���ߺ�
						ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
						ls.des_base_pos.wing_offset_dist = jgzj1.g2;
					}
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
				}
				else						//���ڰ���
				{
					if(bXOutLsLow)
					{
						ls.dwRayNo = GetSingleWord(3);	//��˨���ߺ�
						ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
						ls.des_base_pos.wing_offset_dist = jgzj1.g1;
					}
					else
					{
						ls.dwRayNo = GetSingleWord(4);	//��˨���ߺ�
						ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
						ls.des_base_pos.wing_offset_dist = jgzj1.g2;
					}
					_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
				}
				pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				m_pJoint->boltRefList.append(pLs->GetLsRef());
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'X':'Y';
				InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,ls.des_base_pos.wing_offset_dist,FALSE);
				tm_pt=pLs->ucs.origin;
				SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
				if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
				{
					if(pLs->des_base_pos.direction==0)
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
					else
						pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
				}
				if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
				if(pYPadPlate&&base_wing_thick1<base_wing_thick2)
				{
					pYPadPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPadPlate->handle,TRUE);
				}
				if(m_pJoint->joint_style==0)		//�����
				{	
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==1)	//�������
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				}
				else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
				{
					pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					pYPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPlate->handle,TRUE);
				}
				else
				{
					pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					pYPlate->AppendLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(pYPlate->handle,TRUE);
				}
				if(j+1<ls_group1.wnConnBoltN)
				{
					ls.set_norm(xDesJointPara.norm_x_wing.vector);
					if(bXOutLsLow)
					{
						Add_Pnt(ls_ptx1,ls_ptx1,LsSpace1.doubleRowSpace*ucs.axis_z);
						Add_Pnt(ls_pty2,ls_pty2,LsSpace1.doubleRowSpace*ucs.axis_z);
					}
					else
					{
						Add_Pnt(ls_ptx2,ls_ptx2,LsSpace1.doubleRowSpace*ucs.axis_z);
						Add_Pnt(ls_pty1,ls_pty1,LsSpace1.doubleRowSpace*ucs.axis_z);
					}
					if(m_pJoint->joint_style==0)//���ڰ���
					{
						if(bXOutLsLow)
						{
							ls.dwRayNo = GetSingleWord(1);	//��˨���ߺ�
							ls.ucs.origin=ls_ptx1-ls.get_norm()*base_wing_thick1;
							ls.des_base_pos.wing_offset_dist=jgzj1.g1;
						}
						else
						{
							ls.dwRayNo = GetSingleWord(2);	//��˨���ߺ�
							ls.ucs.origin=ls_ptx2-ls.get_norm()*base_wing_thick1;
							ls.des_base_pos.wing_offset_dist=jgzj1.g2;
						}
						_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
					}
					else if(m_pJoint->joint_style==3)
					{
						if(bXOutLsLow)
						{
							ls.dwRayNo = GetSingleWord(1);	//��˨���ߺ�
							ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g1;
						}
						else
						{
							ls.dwRayNo = GetSingleWord(2);	//��˨���ߺ�
							ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g2;
						}
						_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
					}
					else						//���ڰ���
					{
						if(bXOutLsLow)
						{
							ls.dwRayNo = GetSingleWord(1);	//��˨���ߺ�
							ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g1;
						}
						else
						{
							ls.dwRayNo = GetSingleWord(2);	//��˨���ߺ�
							ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g2;
						}
						_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
					}
					pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					m_pJoint->boltRefList.append(pLs->GetLsRef());
					pLs->CopyProperty(&ls);
					pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
					pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
					cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'Y':'X';
					InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,ls.des_base_pos.wing_offset_dist,FALSE);
					tm_pt=pLs->ucs.origin;
					SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
					if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
					{
						if(pLs->des_base_pos.direction==0)
							pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
						else
							pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
					}
					else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
					{
						if(pLs->des_base_pos.direction==0)
							pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
						else
							pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
					}
					if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
						xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
					else
						xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
					if(pXPadPlate&&base_wing_thick1<base_wing_thick2)
					{
						pXPadPlate->AppendLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pXPadPlate->handle,TRUE);
					}
					if(m_pJoint->joint_style==0)		//�����
					{	
						pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					}
					else if(m_pJoint->joint_style==1)	//�������
					{
						pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
						pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					}
					else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
					{
						pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pInWrapJg->handle,TRUE);
						pXPlate->AppendLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pXPlate->handle,TRUE);
					}
					else
					{
						pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
						pXPlate->AppendLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pXPlate->handle,TRUE);
					}
					ls.set_norm(xDesJointPara.norm_y_wing.vector);
					if(m_pJoint->joint_style==0)//���ڰ���
					{
						if(bXOutLsLow)
						{
							ls.dwRayNo = GetSingleWord(4);	//��˨���ߺ�
							ls.ucs.origin=ls_pty2-ls.get_norm()*base_wing_thick1;
							ls.des_base_pos.wing_offset_dist=jgzj1.g2;
						}
						else
						{
							ls.dwRayNo = GetSingleWord(3);	//��˨���ߺ�
							ls.ucs.origin=ls_pty1-ls.get_norm()*base_wing_thick1;
							ls.des_base_pos.wing_offset_dist=jgzj1.g1;
						}
						_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
					}
					else if(m_pJoint->joint_style==3)
					{
						if(bXOutLsLow)
						{
							ls.dwRayNo = GetSingleWord(4);	//��˨���ߺ�
							ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g2;
						}
						else
						{
							ls.dwRayNo = GetSingleWord(3);	//��˨���ߺ�
							ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g1;
						}
						_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
					}
					else						//���ڰ���
					{
						if(bXOutLsLow)
						{
							ls.dwRayNo = GetSingleWord(4);	//��˨���ߺ�
							ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g2;
						}
						else
						{
							ls.dwRayNo = GetSingleWord(3);	//��˨���ߺ�
							ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
							ls.des_base_pos.wing_offset_dist=jgzj1.g1;
						}
						_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
					}
					pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					m_pJoint->boltRefList.append(pLs->GetLsRef());
					pLs->CopyProperty(&ls);
					pLs->SetLayer(xDesJointPara.m_pMainJg1->layer());
					pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);	//������˨��ĺ����׼������ĺ�һ��
					cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg1->pStart)?'X':'Y';
					InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg1,cWorkWing,ls.des_base_pos.wing_offset_dist,FALSE);
					tm_pt=pLs->ucs.origin;
					SnapPerp(&tm_pt,xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End(),tm_pt);
					if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
					{
						if(pLs->des_base_pos.direction==0)
							pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
						else
							pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
					}
					else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
					{
						if(pLs->des_base_pos.direction==0)
							pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->Start(),tm_pt)+xDesJointPara.m_pMainJg1->startOdd();
						else
							pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg1->End(),tm_pt)+xDesJointPara.m_pMainJg1->endOdd();
					}
					if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
						xDesJointPara.m_pMainJg1->AppendStartLsRef(pLs->GetLsRef());
					else
						xDesJointPara.m_pMainJg1->AppendEndLsRef(pLs->GetLsRef());
					pLs->AddL0Thick(xDesJointPara.m_pMainJg1->handle,TRUE);
					if(pYPadPlate&&base_wing_thick1<base_wing_thick2)
					{
						pYPadPlate->AppendLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pYPadPlate->handle,TRUE);
					}
					if(m_pJoint->joint_style==0)		//�����
					{	
						pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
					}
					else if(m_pJoint->joint_style==1)	//�������
					{
						pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
						pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pInWrapJg->handle,TRUE);
					}
					else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
					{
						pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pInWrapJg->handle,TRUE);
						pYPlate->AppendLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pYPlate->handle,TRUE);
					}
					else
					{
						pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
						pYPlate->AppendLsRef(pLs->GetLsRef());
						pLs->AddL0Thick(pYPlate->handle,TRUE);
					}
				}
			}
		}
	}
	return true;
}

// �����ϲ��ͷ�Ǹ���˨��������˨ʱ���ϵ���˨
bool CDesignJoint::LayUpBaseJgSingleRowLs(CLDSBolt &ls)
{
	int j;
	CLDSBolt *pLs;
	char cWorkWing='X';
	UCS_STRU ucs = m_pJoint->ucs;
	ls.dwRayNo = GetSingleWord(dwRayNo);
	dwRayNo++;	//��˨���ߺ�
	for(j=0;j< ls_group2.wnConnBoltN;j++)
	{
		if(j==0)
		{
			if(m_nLsLHigh0RLow1==0)	//��˨X֫(��֫)�´����
			{
				ls_ptx = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
					base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace,jgzj2.g);
				ls_pty = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
					(base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace+LsSpace2.SingleRowSpace/2),jgzj2.g);
			}
			else	//��˨Y֫(��֫)�´����
			{
				ls_ptx = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
					(base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace+LsSpace2.SingleRowSpace/2),jgzj2.g);
				ls_pty = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
					base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace,jgzj2.g);
			}
			tm_pt=get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
				(base_jg1_oddment-S1-deita+g_sysPara.JgEndDist),0);
		}
		else
		{
			Add_Pnt(ls_ptx,ls_ptx,LsSpace2.SingleRowSpace*ucs.axis_z);
			Add_Pnt(ls_pty,ls_pty,LsSpace2.SingleRowSpace*ucs.axis_z);
		}
		ls.set_norm(xDesJointPara.norm_x_wing.vector);
		BOLT_LOCATION location(ls_ptx);
		int matched=false;
		double jx=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vxWingNorm);
		double jy=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vyWingNorm);
		if(jx>jy)
			matched=mappingX.Mapping(&location)>0;
		else
			matched=mappingY.Mapping(&location)>0;
		if(m_pJoint->joint_style==0)//����������а��ְ�
		{
			ls.ucs.origin=ls_ptx-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",xDesJointPara.m_pMainJg1->handle);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_ptx-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pXPlate->handle);
		}
		else						//�������
		{
			ls.ucs.origin=ls_ptx-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pInWrapJg->handle);
		}
		if(location.pRelaBolt!=NULL)
		{
			pLs=location.pRelaBolt;
			//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
			pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
		}
		else
		{
			pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
			if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g);
		}
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{	
			pXPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		location=BOLT_LOCATION(ls_pty);
		if(jx>jy)
			matched=mappingY.Mapping(&location)>0;
		else
			matched=mappingX.Mapping(&location)>0;
		ls.set_norm(xDesJointPara.norm_y_wing.vector);
		if(m_pJoint->joint_style==0)//���ڰ���
		{
			ls.ucs.origin=ls_pty-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_pty-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//���ڰ���
		{
			ls.ucs.origin=ls_pty-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		if(location.pRelaBolt!=NULL)
		{
			pLs=location.pRelaBolt;
			//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
			pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
		}
		else
		{
			pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
			if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g);
		}
		m_pJoint->boltRefList.append(pLs->GetLsRef());

		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		if(pYPadPlate&&base_wing_thick1>base_wing_thick2)		//�е��
		{
			pYPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
	}
	return true;
}
// �����ϲ��ͷ�Ǹ���˨��˫����˨ʱ���ϵĵ�һ����˨
bool CDesignJoint::LayUpBaseJgDoubleRowFirstLs(CLDSBolt & ls,UCS_STRU &ucs)
{
	char cWorkWing='X';
	CLDSBolt *pLs;
	if(ls_group1.rows!=2)
	{
		if(m_nLsLHigh0RLow1==0)	//��˨X֫(��֫)�´����
		{
			ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
				(base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace+LsSpace2.doubleRowSpace/2),jgzj2.g1);
			ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
				(base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace),jgzj2.g2);
			ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
				base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace,jgzj2.g1);
			ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
				base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace+LsSpace2.doubleRowSpace/2,jgzj2.g2);
		}
		else	//��˨Y֫(��֫)�´����
		{
			ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
				base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace,jgzj2.g1);
			ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ucs.origin,
				base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace+LsSpace2.doubleRowSpace/2,jgzj2.g2);
			ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
				(base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace+LsSpace2.doubleRowSpace/2),jgzj2.g1);
			ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ucs.origin,
				(base_jg1_oddment-S1-deita+g_sysPara.JgEndDist+LsSpace2.EndSpace),jgzj2.g2);
		}
	}
	else
	{
		if(m_nLsLayStyle==0)//����
		{
			//���۲���ʱ����׼�Ǹ���˨�����෴
			//if(ls_group1.wnConnBoltN%2>0)	//ż������˨
			//	bXOutLsLow = !bXOutLsLow;
			if(bXOutLsLow)	//������������˨
			{
				ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx1,
					(LsSpace1.EndSpace+LsSpace1.doubleRowSpace/2)+(LsSpace2.EndSpace+LsSpace2.doubleRowSpace/2)+g_sysPara.JgEndDist,
					jgzj2.g1-jgzj1.g1);
				ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx2,
					(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist),
					jgzj2.g2-jgzj1.g2);
				ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty1,
					(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist),
					jgzj2.g1-jgzj1.g1);
				ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty2,
					(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace1.doubleRowSpace/2+LsSpace2.doubleRowSpace/2),
					jgzj2.g2-jgzj1.g2);
			}
			else
			{
				ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx1,
					(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist),
					jgzj2.g1-jgzj1.g1);
				ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx2,
					(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace1.doubleRowSpace/2+LsSpace2.doubleRowSpace/2),
					jgzj2.g2-jgzj1.g2);
				ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty1,
					(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace1.doubleRowSpace/2+LsSpace2.doubleRowSpace/2),
					jgzj2.g1-jgzj1.g1);
				ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty2,
					(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist),
					jgzj2.g2-jgzj1.g2);
			}
		}
		else					//����--������
		{
			if(ls_group1.wnConnBoltN%2>=0)	//��������˨
				bXOutLsLow = !bXOutLsLow;
			if(bXOutLsLow)
			{
				ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx1,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace2.doubleRowSpace/2),
					jgzj2.g1-jgzj1.g1);
				ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx2,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace1.doubleRowSpace/2),
					jgzj2.g2-jgzj1.g2);
			}
			else
			{
				ls_ptx1 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx1,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace1.doubleRowSpace/2),
					jgzj2.g1-jgzj1.g1);
				ls_ptx2 = get_pt_by_vec(ucs.axis_z,ucs.axis_x,ls_ptx2,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace2.doubleRowSpace/2),
					jgzj2.g2-jgzj1.g2);
			}
			if(bXOutLsLow)
			{
				ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty1,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace1.doubleRowSpace/2),
					jgzj2.g1-jgzj1.g1);
				ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty2,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace2.doubleRowSpace/2),
					jgzj2.g2-jgzj1.g2);
			}
			else
			{
				ls_pty1 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty1,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace2.doubleRowSpace/2),
					jgzj2.g1-jgzj1.g1);
				ls_pty2 = get_pt_by_vec(ucs.axis_z,ucs.axis_y,ls_pty2,(LsSpace1.EndSpace+LsSpace2.EndSpace+g_sysPara.JgEndDist+LsSpace1.doubleRowSpace/2),
					jgzj2.g2-jgzj1.g2);
			}
		}
	}
	ls.set_norm(xDesJointPara.norm_x_wing.vector);
	BOLT_LOCATION location_x1(ls_ptx1),location_x2(ls_ptx2),location_y1(ls_pty1),location_y2(ls_pty2);
	int matched=false;
	double jx=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vxWingNorm);
	double jy=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vyWingNorm);
	if(jx>jy)
	{
		matched=mappingX.Mapping(&location_x1)>0;
		matched=mappingX.Mapping(&location_x2)>0;
		matched=mappingY.Mapping(&location_y1)>0;
		matched=mappingY.Mapping(&location_y2)>0;
	}
	else
	{
		matched=mappingX.Mapping(&location_y1)>0;
		matched=mappingX.Mapping(&location_y2)>0;
		matched=mappingY.Mapping(&location_x1)>0;
		matched=mappingY.Mapping(&location_x2)>0;
	}
	if(m_pJoint->joint_style==0)//���ڰ���
	{
		ls.ucs.origin=ls_ptx1-ls.get_norm()*base_wing_thick1;
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
	}
	else if(m_pJoint->joint_style==3)
	{
		ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
	}
	else						//���ڰ���
	{
		ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
	}
	ls.dwRayNo = GetSingleWord(5);	//��˨���ߺ�
	if(location_x1.pRelaBolt!=NULL)
	{
		pLs=location_x1.pRelaBolt;
		//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
		pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
	}
	else
	{
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g1,FALSE);
		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
	}
	m_pJoint->boltRefList.append(pLs->GetLsRef());

	tm_pt=pLs->ucs.origin;
	SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
	if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}
	else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}

	if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
	{	
		pXPadPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pXPadPlate->handle,TRUE);
	}
	if(m_pJoint->joint_style==0)		//�����
	{	
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==1)	//�������
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
	{
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		pXPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pXPlate->handle,TRUE);
	}
	else
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pXPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pXPlate->handle,TRUE);
	}

	ls.ucs.origin=ls_ptx2;
	if(m_pJoint->joint_style==0)//���ڰ���
	{
		ls.ucs.origin=ls_ptx2-ls.get_norm()*base_wing_thick1;
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
	}
	else if(m_pJoint->joint_style==3)
	{
		ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
	}
	else						//���ڰ���
	{
		ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
	}
	ls.dwRayNo = GetSingleWord(6);	//��˨���ߺ�
	if(location_x2.pRelaBolt!=NULL)
	{
		pLs=location_x2.pRelaBolt;
		//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
		pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
	}
	else
	{
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g2,FALSE);
		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
	}
	m_pJoint->boltRefList.append(pLs->GetLsRef());
	tm_pt=pLs->ucs.origin;
	SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
	if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}
	else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}
	
	if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
	{
		pXPadPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pXPadPlate->handle,TRUE);
	}
	if(m_pJoint->joint_style==0)		//�����
	{	
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==1)	//�������
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
	{
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		pXPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pXPlate->handle,TRUE);
	}
	else
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pXPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pXPlate->handle,TRUE);
	}

	ls.set_norm(xDesJointPara.norm_y_wing.vector);
	if(m_pJoint->joint_style==0)//���ڰ���
	{
		ls.ucs.origin=ls_pty1-ls.get_norm()*base_wing_thick1;
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
	}
	else if(m_pJoint->joint_style==3)
	{
		ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
	}
	else						//���ڰ���
	{
		ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
	}
	ls.dwRayNo = GetSingleWord(7);	//��˨���ߺ�
	if(location_y1.pRelaBolt!=NULL)
	{
		pLs=location_y1.pRelaBolt;
		//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
		pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
	}
	else
	{
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g1,FALSE);
		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
	}
	m_pJoint->boltRefList.append(pLs->GetLsRef());
	tm_pt=pLs->ucs.origin;
	SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
	if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}
	else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}
	
	if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
	{
		pYPadPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pYPadPlate->handle,TRUE);
	}
	if(m_pJoint->joint_style==0)		//�����
	{	
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==1)	//�������
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
	{
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		pYPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pYPlate->handle,TRUE);
	}
	else
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pYPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pYPlate->handle,TRUE);
	}

	if(m_pJoint->joint_style==0)//���ڰ���
	{
		ls.ucs.origin=ls_pty2-ls.get_norm()*base_wing_thick1;
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
	}
	else if(m_pJoint->joint_style==3)
	{
		ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
	}
	else						//���ڰ���
	{
		ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
		_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
	}
	ls.dwRayNo = GetSingleWord(8);	//��˨���ߺ�
	if(location_y2.pRelaBolt!=NULL)
	{
		pLs=location_y2.pRelaBolt;
		//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
		pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
	}
	else
	{
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g2,FALSE);
		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
	}
	m_pJoint->boltRefList.append(pLs->GetLsRef());
	tm_pt=pLs->ucs.origin;
	SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
	if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}
	else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
	{
		if(pLs->des_base_pos.direction==0)
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
		else
			pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
	}
	
	if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
	{
		pYPadPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pYPadPlate->handle,TRUE);
	}
	if(m_pJoint->joint_style==0)		//�����
	{	
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==1)	//�������
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
	}
	else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
	{
		pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		pYPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pYPlate->handle,TRUE);
	}
	else
	{
		pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		pYPlate->AppendLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(pYPlate->handle,TRUE);
	}
	return true;
}
// �����ϲ��ͷ�Ǹ���˨��˫����˨ʱ���ϳ���һ�������������˨
int CDesignJoint::LayUpBaseJgDoubleRowOtherLs(CLDSBolt & ls,int j,UCS_STRU &ucs)
{
	char cWorkWing='X';
	CLDSBolt *pLs;
	if(m_nLsLayStyle==0)//����
	{
		Add_Pnt(ls_ptx1,ls_ptx1,LsSpace2.doubleRowSpace*ucs.axis_z);
		Add_Pnt(ls_ptx2,ls_ptx2,LsSpace2.doubleRowSpace*ucs.axis_z);
		Add_Pnt(ls_pty1,ls_pty1,LsSpace2.doubleRowSpace*ucs.axis_z);
		Add_Pnt(ls_pty2,ls_pty2,LsSpace2.doubleRowSpace*ucs.axis_z);

		ls.set_norm(xDesJointPara.norm_x_wing.vector);
		BOLT_LOCATION location_x1(ls_ptx1),location_x2(ls_ptx2),location_y1(ls_pty1),location_y2(ls_pty2);
		int matched=false;
		double jx=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vxWingNorm);
		double jy=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vyWingNorm);
		if(jx>jy)
		{
			matched=mappingX.Mapping(&location_x1)>0;
			matched=mappingX.Mapping(&location_x2)>0;
			matched=mappingY.Mapping(&location_y1)>0;
			matched=mappingY.Mapping(&location_y2)>0;
		}
		else
		{
			matched=mappingX.Mapping(&location_y1)>0;
			matched=mappingX.Mapping(&location_y2)>0;
			matched=mappingY.Mapping(&location_x1)>0;
			matched=mappingY.Mapping(&location_x2)>0;
		}
		if(m_pJoint->joint_style==0)//���ڰ���
		{
			ls.ucs.origin=ls_ptx1-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//���ڰ���
		{
			ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(5);	//��˨���ߺ�
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g1,FALSE);
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}

		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pXPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)//���ڰ���
		{
			ls.ucs.origin=ls_ptx2-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//���ڰ���
		{
			ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(6);	//��˨���ߺ�
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g2,FALSE);
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		
		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pXPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}

		ls.set_norm(xDesJointPara.norm_y_wing.vector);
		if(m_pJoint->joint_style==0)//���ڰ���
		{
			ls.ucs.origin=ls_pty1-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//���ڰ���
		{
			ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(7);	//��˨���ߺ�
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g1,FALSE);
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		
		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pYPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)//���ڰ���
		{
			ls.ucs.origin=ls_pty2-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//���ڰ���
		{
			ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(8);	//��˨���ߺ�
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		pLs->CopyProperty(&ls);
		pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
		pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

		cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
		InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g2,FALSE);
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		
		if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
			xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
		else
			xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
		pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pYPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
	}
	else		//����
	{
		Add_Pnt(ls_ptx1,ls_ptx1,LsSpace2.doubleRowSpace*ucs.axis_z);
		Add_Pnt(ls_ptx2,ls_ptx2,LsSpace2.doubleRowSpace*ucs.axis_z);
		Add_Pnt(ls_pty1,ls_pty1,LsSpace2.doubleRowSpace*ucs.axis_z);
		Add_Pnt(ls_pty2,ls_pty2,LsSpace2.doubleRowSpace*ucs.axis_z);
		ls.set_norm(xDesJointPara.norm_x_wing.vector);
		BOLT_LOCATION location_x1(ls_ptx1),location_x2(ls_ptx2),location_y1(ls_pty1),location_y2(ls_pty2);
		int matched=false;
		double jx=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vxWingNorm);
		double jy=fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vyWingNorm);
		if(jx>jy)
		{
			matched=mappingX.Mapping(&location_x1)>0;
			matched=mappingX.Mapping(&location_x2)>0;
			matched=mappingY.Mapping(&location_y1)>0;
			matched=mappingY.Mapping(&location_y2)>0;
		}
		else
		{
			matched=mappingX.Mapping(&location_y1)>0;
			matched=mappingX.Mapping(&location_y2)>0;
			matched=mappingY.Mapping(&location_x1)>0;
			matched=mappingY.Mapping(&location_x2)>0;
		}
		if(m_pJoint->joint_style==0)//����������а��ְ�
		{
			ls.ucs.origin=ls_ptx1-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//�������
		{
			ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(5);	//��˨���ߺ�
		if(location_x1.pRelaBolt!=NULL)
		{
			pLs=location_x1.pRelaBolt;
			//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
			pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
		}
		else
		{
			pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g1,FALSE);
			if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		}
		m_pJoint->boltRefList.append(pLs->GetLsRef());

		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		
		if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pXPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)//����������а��ְ�
		{
			ls.ucs.origin=ls_ptx2-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//�������
		{
			ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(6);	//��˨���ߺ�
		if(location_x2.pRelaBolt!=NULL)
		{
			pLs=location_x2.pRelaBolt;
			//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
			pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
		}
		else
		{
			pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g2,FALSE);
			if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		}
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		
		if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pXPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pXPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pXPlate->handle,TRUE);
		}
		//------------------------------
		ls.set_norm(xDesJointPara.norm_y_wing.vector);
		if(m_pJoint->joint_style==0)//���ڰ���
		{
			ls.ucs.origin=ls_pty1-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//���ڰ���
		{
			ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(7);	//��˨���ߺ�
		if(location_y1.pRelaBolt!=NULL)
		{
			pLs=location_y1.pRelaBolt;
			//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
			pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
		}
		else
		{
			pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g1,FALSE);
			if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		}
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		
		if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pYPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)//���ڰ���
		{
			ls.ucs.origin=ls_pty2-ls.get_norm()*base_wing_thick1;
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
		}
		else if(m_pJoint->joint_style==3)
		{
			ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
		}
		else						//���ڰ���
		{
			ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
			_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
		}
		ls.dwRayNo = GetSingleWord(8);	//��˨���ߺ�
		if(location_y2.pRelaBolt!=NULL)
		{
			pLs=location_y2.pRelaBolt;
			//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
			pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
		}
		else
		{
			pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pLs->CopyProperty(&ls);
			pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
			pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

			cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
			InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g2,FALSE);
			if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
				xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
			else
				xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle,TRUE);
		}
		m_pJoint->boltRefList.append(pLs->GetLsRef());
		tm_pt=pLs->ucs.origin;
		SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
		if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
		{
			if(pLs->des_base_pos.direction==0)
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
			else
				pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
		}
		
		if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
		{
			pYPadPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPadPlate->handle,TRUE);
		}
		if(m_pJoint->joint_style==0)		//�����
		{	
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==1)	//�������
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
		}
		else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{
			pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
		else
		{
			pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			pYPlate->AppendLsRef(pLs->GetLsRef());
			pLs->AddL0Thick(pYPlate->handle,TRUE);
		}
	}
	//------------------------------
	if(j+3==ls_group2.wnConnBoltN)	//������������˨
	{
		if(bXOutLsLow)	//X֫�����˨�´����
		{
			Add_Pnt(ls_ptx2,ls_ptx2,LsSpace2.doubleRowSpace*ucs.axis_z);
			Add_Pnt(ls_pty1,ls_pty1,LsSpace2.doubleRowSpace*ucs.axis_z);
			ls.set_norm(xDesJointPara.norm_x_wing.vector);
			BOLT_LOCATION location_x(ls_ptx2), location_y(ls_pty1);
			int matched = false;
			double jx = fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vxWingNorm);
			double jy = fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vyWingNorm);
			if (jx > jy)
			{
				matched = mappingX.Mapping(&location_x) > 0;
				matched = mappingY.Mapping(&location_y) > 0;
			}
			else
			{
				matched = mappingX.Mapping(&location_y) > 0;
				matched = mappingY.Mapping(&location_x) > 0;
			}
			if(m_pJoint->joint_style==0)//���ڰ���
			{
				ls.ucs.origin=ls_ptx2-ls.get_norm()*base_wing_thick1;
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
			}
			else if(m_pJoint->joint_style==3)
			{
				ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
			}
			else						//���ڰ���
			{
				ls.ucs.origin=ls_ptx2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
			}
			ls.dwRayNo = GetSingleWord(6);	//��˨���ߺ�
			if(location_x.pRelaBolt!=NULL)
			{
				pLs = location_x.pRelaBolt;
				//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
				pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
			}
			else
			{
				pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
				InitJgLsDesPosPara(pLs, xDesJointPara.m_pMainJg2, cWorkWing, jgzj2.g2, FALSE);
				if (xDesJointPara.m_pMainJg2->pStart->handle == m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle, TRUE);
			}
			m_pJoint->boltRefList.append(pLs->GetLsRef());
			tm_pt=pLs->ucs.origin;
			SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
			if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			
			if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
			{
				pXPadPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPadPlate->handle,TRUE);
			}
			if(m_pJoint->joint_style==0)		//�����
			{	
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==1)	//�������
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
			{
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				pXPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPlate->handle,TRUE);
			}
			else
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pXPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPlate->handle,TRUE);
			}

			ls.set_norm(xDesJointPara.norm_y_wing.vector);
			if(m_pJoint->joint_style==0)//���ڰ���
			{
				ls.ucs.origin=ls_pty1-ls.get_norm()*base_wing_thick1;
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
			}
			else if(m_pJoint->joint_style==3)
			{
				ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
			}
			else						//���ڰ���
			{
				ls.ucs.origin=ls_pty1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
			}
			ls.dwRayNo = GetSingleWord(7);	//��˨���ߺ�
			if (location_y.pRelaBolt != NULL)
			{
				pLs = location_y.pRelaBolt;
				//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
				pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
			}
			else
			{
				pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
				InitJgLsDesPosPara(pLs, xDesJointPara.m_pMainJg2, cWorkWing, jgzj2.g1, FALSE);
				if (xDesJointPara.m_pMainJg2->pStart->handle == m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle, TRUE);
			}
			m_pJoint->boltRefList.append(pLs->GetLsRef());

			tm_pt=pLs->ucs.origin;
			SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
			if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			
			if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
			{
				pYPadPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPadPlate->handle,TRUE);
			}
			if(m_pJoint->joint_style==0)		//�����
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==1)	//�������
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
			{
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				pYPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPlate->handle,TRUE);
			}
			else
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pYPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPlate->handle,TRUE);
			}
		}
		else			//X֫�ڲ���˨�´����
		{
			Add_Pnt(ls_ptx1,ls_ptx1,LsSpace2.doubleRowSpace*ucs.axis_z);
			Add_Pnt(ls_pty2,ls_pty2,LsSpace2.doubleRowSpace*ucs.axis_z);
			ls.set_norm(xDesJointPara.norm_x_wing.vector);
			BOLT_LOCATION location_x(ls_ptx1), location_y(ls_pty2);
			int matched = false;
			double jx = fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vxWingNorm);
			double jy = fabs(ls.ucs.axis_z*xDesJointPara.m_pMainJg2->vyWingNorm);
			if (jx > jy)
			{
				matched = mappingX.Mapping(&location_x) > 0;
				matched = mappingY.Mapping(&location_y) > 0;
			}
			else
			{
				matched = mappingX.Mapping(&location_y) > 0;
				matched = mappingY.Mapping(&location_x) > 0;
			}
			if(m_pJoint->joint_style==0)//���ڰ���
			{
				ls.ucs.origin=ls_ptx1-ls.get_norm()*base_wing_thick1;
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
			}
			else if(m_pJoint->joint_style==3)
			{
				ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
			}
			else						//���ڰ���
			{
				ls.ucs.origin=ls_ptx1-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
			}
			ls.dwRayNo = GetSingleWord(5);	//��˨���ߺ�
			if (location_x.pRelaBolt != NULL)
			{
				pLs = location_x.pRelaBolt;
				//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
				pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
			}
			else
			{
				pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��
				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'X':'Y';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
				InitJgLsDesPosPara(pLs, xDesJointPara.m_pMainJg2, cWorkWing, jgzj2.g1, FALSE);
				if (xDesJointPara.m_pMainJg2->pStart->handle == m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle, TRUE);
			}
			m_pJoint->boltRefList.append(pLs->GetLsRef());

			tm_pt=pLs->ucs.origin;
			SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
			if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			
			if(pXPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
			{
				pXPadPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPadPlate->handle,TRUE);
			}
			if(m_pJoint->joint_style==0)		//�����
			{	
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==1)	//�������
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
			{
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				pXPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPlate->handle,TRUE);
			}
			else
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pXPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pXPlate->handle,TRUE);
			}

			ls.set_norm(xDesJointPara.norm_y_wing.vector);
			if(m_pJoint->joint_style==0)//���ڰ���
			{
				ls.ucs.origin=ls_pty2-ls.get_norm()*base_wing_thick1;
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-base_wing_thick1);
			}
			else if(m_pJoint->joint_style==3)
			{
				ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pXPlate->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pXPlate->handle);
			}
			else						//���ڰ���
			{
				ls.ucs.origin=ls_pty2-ls.get_norm()*(base_wing_thick1+pInWrapJg->GetThick());
				_snprintf(ls.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f,-0X%X",-base_wing_thick1,pInWrapJg->handle);
			}
			ls.dwRayNo = GetSingleWord(8);	//��˨���ߺ�
			if (location_y.pRelaBolt != NULL)
			{
				pLs = location_y.pRelaBolt;
				//������˼���ͷ��˨Ҫ�������˼���ͷ��˨���� wjh-2019.6.20
				pLs->cfgword.AddSpecWord(xDesJointPara.m_pMainJg1->cfgword);
			}
			else
			{
				pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
				pLs->CopyProperty(&ls);
				pLs->SetLayer(xDesJointPara.m_pMainJg2->layer());
				pLs->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg2);	//������˨��ĺ����׼������ĺ�һ��

				cWorkWing=(xDesJointPara.m_pCurNode==xDesJointPara.m_pMainJg2->pStart)?'Y':'X';	//�����Ǹֿ���ͷ��ͷ��β��β wjh-2019.8.3
				InitJgLsDesPosPara(pLs,xDesJointPara.m_pMainJg2,cWorkWing,jgzj2.g2,FALSE);
				if (xDesJointPara.m_pMainJg2->pStart->handle == m_pJoint->base_node_handle)
					xDesJointPara.m_pMainJg2->AppendStartLsRef(pLs->GetLsRef());
				else
					xDesJointPara.m_pMainJg2->AppendEndLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(xDesJointPara.m_pMainJg2->handle, TRUE);
			}
			m_pJoint->boltRefList.append(pLs->GetLsRef());
			tm_pt=pLs->ucs.origin;
			SnapPerp(&tm_pt,xDesJointPara.m_pMainJg2->Start(),xDesJointPara.m_pMainJg2->End(),tm_pt);
			if(lineTemp.PtInLine(tm_pt,EPS2)>0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			else	//if(lineTemp.PtInLine(tm_pt,EPS2)<0)	//��ֱ����
			{
				if(pLs->des_base_pos.direction==0)
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->Start(),tm_pt)+xDesJointPara.m_pMainJg2->startOdd();
				else
					pLs->des_base_pos.len_offset_dist=-DISTANCE(xDesJointPara.m_pMainJg2->End(),tm_pt)+xDesJointPara.m_pMainJg2->endOdd();
			}
			
			if(pYPadPlate&&base_wing_thick1>base_wing_thick2)	//�е��
			{
				pYPadPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPadPlate->handle,TRUE);
			}
			if(m_pJoint->joint_style==0)		//�����
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==1)	//�������
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
			}
			else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
			{
				pInWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pInWrapJg->handle,TRUE);
				pYPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPlate->handle,TRUE);
			}
			else
			{
				pOutWrapJg->AppendMidLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pOutWrapJg->handle,TRUE);
				pYPlate->AppendLsRef(pLs->GetLsRef());
				pLs->AddL0Thick(pYPlate->handle,TRUE);
			}
		}
		return 0;
	}
	return 1;
}
// �����ϲ��ͷ�Ǹ���˨��˫����˨ʱ���ϵ���˨
bool CDesignJoint::LayUpBaseJgDoubleRowLs(CLDSBolt &ls)
{
	int j;
	UCS_STRU ucs = m_pJoint->ucs;
	ls.dwRayNo = GetSingleWord(dwRayNo);
	dwRayNo++;	//��˨���ߺ�

	for(j=0;j< ls_group2.wnConnBoltN;j+=2)
	{
		if(j==0)
			LayUpBaseJgDoubleRowFirstLs(ls,ucs);
		else if(LayUpBaseJgDoubleRowOtherLs(ls,j,ucs)==0)
			break;
	}
	return true;
}
// ���ý�ͷ�������ϲ��׼�Ǹ��ϵ���˨,ͬʱҲ����˨������Ӧ�İ�������ְ���
bool CDesignJoint::LayUpBaseJgLs()
{
	CLDSBolt ls(console.GetActiveModel());
	UCS_STRU ucs = m_pJoint->ucs;
	//����ֱ�Ӹ�����˨ͨ��(δָ����������)�����½�ͷ������ƺ���˨��λ  wxc-2015.6.9
	//ls.AddL0Thick((long)GetLsL0(true),FALSE);
	ls.set_d(ls_group2.d);
	if(!ls.CalGuigeAuto())
	{
		char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
		_snprintf(sError,199,"bolt specification in accordance with M%dX%.f not found in bolt specification library",ls.get_d(),ls.get_L0());
#else
		_snprintf(sError,199,"����˨������û���ҵ�����M%dX%.f����˨���",ls.d,ls.L0);
#endif
		throw sError;
	}

	lineTemp.startPt = xDesJointPara.m_pMainJg2->Start();
	lineTemp.endPt = xDesJointPara.m_pMainJg2->End();
	if (m_pJoint->blInheritExistBoltsInCoupleAngle)
	{
		mappingX.ClearMapping();
		mappingY.ClearMapping();
		mappingX.InitExistBoltsInfo(xDesJointPara.m_pMainJg2,0, xDesJointPara.m_pCurNode);
		mappingY.InitExistBoltsInfo(xDesJointPara.m_pMainJg2,1, xDesJointPara.m_pCurNode);
		//matchcount = mapping.Mapping(existbolts.pPresetObjs, existbolts.Count, &listLocations, 5);
	}
	if(ls_group2.rows==1)
		LayUpBaseJgSingleRowLs(ls);
	else	//˫����˨
		LayUpBaseJgDoubleRowLs(ls);
	
	return true;
}
//��������ְ���
static void UpdateOutterPlateWidth(CLDSPlate *pPlate,int nPlateWidth,double fBerWidth,CLDSLineAngle *pMainAngle)
{	//1.����X������С��������˨
	CMaxDouble maxValue,maxValueAll;
	CMinDouble minValue,minValueAll;
	//TODO:��Ҫ����������Ϊ��һ��תΪ����������׼�� wjh-2016.8.4
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		GEPOINT ls_pos;//=(*pLsRef)->ucs.origin;
		pPlate->SpreadLsPos(pLsRef,ls_pos);
		maxValueAll.Update(ls_pos.x,pLsRef->GetLsPtr());
		minValueAll.Update(ls_pos.x,pLsRef->GetLsPtr());
		//TODO:�����ж��ǲ���������
		if(pMainAngle->FindLsByHandle((*pLsRef)->handle)==NULL)
			continue;	//�����Ǹ�1�ϵ���˨Ϊ�жϻ�׼
		maxValue.Update(ls_pos.x,pLsRef->GetLsPtr());
		minValue.Update(ls_pos.x,pLsRef->GetLsPtr());
	}
	if(!maxValue.IsInited())
		maxValue=maxValueAll;
	if(!minValue.IsInited())
		minValue=minValueAll;
	//2.���������Ϊ���߲༰֫������飬ÿ��������Y�����С��������
	GEPOINT wing_x_vec=pMainAngle->GetWingVecX();
	GEPOINT wing_y_vec=pMainAngle->GetWingVecY();
	GEPOINT wing_vec=wing_y_vec;
	if( fabs(pPlate->ucs.axis_x*wing_x_vec)>fabs(pPlate->ucs.axis_x*wing_y_vec))
		wing_vec=wing_x_vec;
	BOOL bAlongWingVec=pPlate->ucs.axis_x*wing_vec>0;
	typedef PROFILE_VERTEX* VERTEX_PTR;
	int iBer=0,iWing=0;
	VERTEX_PTR berVertexPtrArr[2],wingVertexPtrArr[2];	//Y�����С��������
	for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(pVertex->vertex.x>maxValue.number)
		{
			if(iBer==1&&berVertexPtrArr[0]->vertex.y>pVertex->vertex.y)
			{
				berVertexPtrArr[1]=berVertexPtrArr[0];
				berVertexPtrArr[0]=pVertex;
			}
			else if(iBer<2)
				berVertexPtrArr[iBer++]=pVertex;	
		}
		else
		{
			if(iWing==1&&wingVertexPtrArr[0]->vertex.y>pVertex->vertex.y)
			{
				wingVertexPtrArr[1]=wingVertexPtrArr[0];
				wingVertexPtrArr[0]=pVertex;
			}
			else if(iWing<2)
				wingVertexPtrArr[iWing++]=pVertex;
		}
	}
	if(bAlongWingVec)
	{	//X�����Сһ��ΪBer,�������������ݶԵ�
		PROFILE_VERTEX *pVertex1=berVertexPtrArr[0],*pVertex2=berVertexPtrArr[1];
		berVertexPtrArr[0]=wingVertexPtrArr[0];
		berVertexPtrArr[1]=wingVertexPtrArr[1];
		wingVertexPtrArr[0]=pVertex1;
		wingVertexPtrArr[1]=pVertex2;
	}
	//3.���������ȵ����ְ���
	double fRealWidth=fabs(wingVertexPtrArr[0]->vertex.x-berVertexPtrArr[0]->vertex.x);
	double ber_offset=0,wing_offset=0;
	if(fBerWidth<=0)
		ber_offset=wing_offset=max(0,0.5*(nPlateWidth-fRealWidth));
	else
	{
		double ber_bolt_x=bAlongWingVec?minValue.number:maxValue.number;
		ber_offset=max(0,fBerWidth-fabs(berVertexPtrArr[0]->vertex.x-ber_bolt_x));
		wing_offset=max(0,nPlateWidth-fRealWidth-ber_offset);
	}
	if(bAlongWingVec)
	{
		ber_offset=-ber_offset;
		wing_offset=-wing_offset;
	}
	if(berVertexPtrArr[0])
		berVertexPtrArr[0]->vertex.x+=ber_offset;
	if(berVertexPtrArr[1])
		berVertexPtrArr[1]->vertex.x+=ber_offset;
	if(wingVertexPtrArr[0])
		wingVertexPtrArr[0]->vertex.x-=wing_offset;
	if(wingVertexPtrArr[1])
		wingVertexPtrArr[1]->vertex.x-=wing_offset;
}
bool CDesignJoint::DesignJoint()
{
	pOutWrapJg=pInWrapJg=NULL;
	pYPlate=pXPlate=pXPadPlate=pYPadPlate=NULL;

	//���������Ǹ���˨��
	if(ls_group1.rows<1)
		ls_group1.rows = 1;
	else if(ls_group1.rows>2)
		ls_group1.rows=2;
	if(ls_group2.rows<1)
		ls_group2.rows = 1;
	else if(ls_group2.rows>2)
		ls_group2.rows=2;
	//GetLsSpace(LsSpace1,ls_group1.d);
	//GetLsSpace(LsSpace2,ls_group2.d);

	//��˨���Ͷ˾����Ϊ�Ǳ�׼���˾�
	int ls_group_len1=0, ls_group_len2=0;
	if(ls_group1.rows==1)
		ls_group_len1 = 2*LsSpace1.EndSpace+(ls_group1.wnConnBoltN-1)*LsSpace1.SingleRowSpace;
	else	//˫����˨
		ls_group_len1 = 2*LsSpace1.EndSpace +(ls_group1.wnConnBoltN-1)*LsSpace1.doubleRowSpace/2;
	if(ls_group2.rows==1)
		ls_group_len2 = 2*LsSpace2.EndSpace+(ls_group2.wnConnBoltN-1)*LsSpace2.SingleRowSpace;
	else	//˫����˨
		ls_group_len2 = 2*LsSpace2.EndSpace +(ls_group2.wnConnBoltN-1)*LsSpace2.doubleRowSpace/2;
	m_pJoint->length = g_sysPara.JgEndDist+ls_group_len1+ls_group_len2+
		ls_group1.rows%2*LsSpace1.SingleRowSpace/2+ls_group2.rows%2*LsSpace2.SingleRowSpace/2;
	//��д����ʼ�ն����λ�õ�����ƫ��������
	xDesJointPara.start.des_para.RODEND.len_offset_dist=ls_group1.rows%2*LsSpace1.SingleRowSpace/2+ls_group_len1;
	xDesJointPara.end.des_para.RODEND.len_offset_dist=ls_group2.rows%2*LsSpace2.SingleRowSpace/2+ls_group_len2;
	/*
	m_pJoint->length = g_sysPara.JgEndDist+
		ls_group1.rows%2*LsSpace1.SingleRowSpace/2+ls_group2.rows%2*LsSpace2.SingleRowSpace/2+
		GetLsGroupLen(ls_group1.d, ls_group1.wnConnBoltN, ls_group1.rows)+GetLsGroupLen(ls_group2.d, ls_group2.wnConnBoltN, ls_group2.rows);
	//��д����ʼ�ն����λ�õ�����ƫ��������
	xDesJointPara.start.des_para.RODEND.len_offset_dist=ls_group1.rows%2*LsSpace1.SingleRowSpace/2+GetLsGroupLen(ls_group1.d, ls_group1.wnConnBoltN, ls_group1.rows);
	xDesJointPara.end.des_para.RODEND.len_offset_dist=ls_group2.rows%2*LsSpace2.SingleRowSpace/2+GetLsGroupLen(ls_group2.d, ls_group2.wnConnBoltN, ls_group2.rows);
	*/

	if(!DesJointSubPart(pOutWrapJg,pInWrapJg,pYPlate,pXPlate,pXPadPlate,pYPadPlate))
		return FALSE;
	if(pInWrapJg)
	{
		pInWrapJg->SetPartNo(JointDesDlg.m_sPartNoA.GetBuffer(0));
		pInWrapJg->iSeg=SEGI(JointDesDlg.m_sASegI.GetBuffer());
		if(pInWrapJg->iSeg<=0)	//�κ�С�ڵ���0ʱĬ��ȡ��ͷ�κ�,�����������ʱ�κ�Ϊ0��Ҫ�ֶ����� wht 13-01-06
			pInWrapJg->iSeg=m_pJoint->iSeg;
	}
	if(pOutWrapJg)
	{
		pOutWrapJg->SetPartNo(JointDesDlg.m_sPartNoB.GetBuffer(0));
		pOutWrapJg->iSeg=SEGI(JointDesDlg.m_sBSegI.GetBuffer());
		if(pOutWrapJg->iSeg<=0)
			pOutWrapJg->iSeg=m_pJoint->iSeg;
	}
	if(pXPlate)
	{	//ʾ��ͼ��F���Ӧ��������ϵ�е�X֫���ְ�, �Ժ���ʱ�仹Ӧ��������Ƿ�Ҳ���� wjh-2015.2.2
		pXPlate->SetPartNo(JointDesDlg.m_sPartNoE);
		pXPlate->iSeg=SEGI(JointDesDlg.m_sESegI);
		if(pXPlate->iSeg<=0)
			pXPlate->iSeg=m_pJoint->iSeg;
	}
	if(pYPlate)
	{
		pYPlate->SetPartNo(JointDesDlg.m_sPartNoF);
		pYPlate->iSeg=SEGI(JointDesDlg.m_sFSegI);
		if(pYPlate->iSeg<=0)
			pYPlate->iSeg=m_pJoint->iSeg;
	}
	if(pXPadPlate)
	{
		pXPadPlate->SetPartNo(JointDesDlg.m_sPartNoG.GetBuffer(0));
		pYPadPlate->iSeg=SEGI(JointDesDlg.m_sGSegI.GetBuffer());
		if(pYPadPlate->iSeg<=0)
			pYPadPlate->iSeg=m_pJoint->iSeg;
	}
	if(pYPadPlate)
	{
		pYPadPlate->SetPartNo(JointDesDlg.m_sPartNoH.GetBuffer(0));
		pYPadPlate->iSeg=SEGI(JointDesDlg.m_sHSegI.GetBuffer());
		if(pYPadPlate->iSeg<=0)
			pYPadPlate->iSeg=m_pJoint->iSeg;
	}

	if(xDesJointPara.m_pMainJg1->pStart->handle==m_pJoint->base_node_handle)
	{
		xDesJointPara.m_pMainJg1->feature=0;	//ʼ�˽ڵ�Ϊ��ͷ��׼�ڵ�
		if(m_pJoint->blIntelliCalOddment)
		{
			xDesJointPara.m_pMainJg1->SetStartOdd(base_jg1_oddment);
			xDesJointPara.m_pMainJg1->desStartOdd.m_iOddCalStyle=2;
		}
	}
	else
	{
		xDesJointPara.m_pMainJg1->feature=1;	//�ն˽ڵ�Ϊ��ͷ��׼�ڵ�
		if (m_pJoint->blIntelliCalOddment)
		{
			xDesJointPara.m_pMainJg1->SetEndOdd(base_jg1_oddment);
			xDesJointPara.m_pMainJg1->desEndOdd.m_iOddCalStyle=2;
		}
	}

	if(xDesJointPara.m_pMainJg2->pStart->handle==m_pJoint->base_node_handle)
	{
		xDesJointPara.m_pMainJg2->feature=0;	//ʼ�˽ڵ�Ϊ��ͷ��׼�ڵ�
		if (m_pJoint->blIntelliCalOddment)
		{
			xDesJointPara.m_pMainJg2->SetStartOdd(-(base_jg1_oddment+g_sysPara.JgEndDist));
			xDesJointPara.m_pMainJg2->desStartOdd.m_iOddCalStyle=2;
		}
	}
	else
	{
		xDesJointPara.m_pMainJg2->feature=1;	//�ն˽ڵ�Ϊ��ͷ��׼�ڵ�
		if (m_pJoint->blIntelliCalOddment)
		{
			xDesJointPara.m_pMainJg2->SetEndOdd(-(base_jg1_oddment+g_sysPara.JgEndDist));
			xDesJointPara.m_pMainJg2->desEndOdd.m_iOddCalStyle=2;
		}
	}

	dwRayNo=1;
	//��ѯƥ���²�ģ���һ������׼�Ǹ��ϵ���˨���
	ls_group_len1 = 2 * LsSpace1.EndSpace + (ls_group1.wnConnBoltN - 1)*LsSpace1.doubleRowSpace / 2;
	ls_group_len2 = 2 * LsSpace2.EndSpace + (ls_group2.wnConnBoltN - 1)*LsSpace2.SingleRowSpace;
	LayDownBaseJgLs();
	//��ѯƥ���ϲ�ģ��ڶ�������׼�Ǹ��ϵ���˨���
	LayUpBaseJgLs();

	//������ְ��ְ�������γߴ�
	f3dPoint vertex[3];
	double offset_wide;
	//�ڰ�������ְ�(joint_style==2)ʱ�������߲��� wht 16-07-12
	if(pXPlate)
	{
		pXPlate->designInfo.origin.UpdatePos(pXPlate->BelongModel());
		pXPlate->ucs.origin=pXPlate->designInfo.origin.Position();
		pXPlate->CalStdProfile();
		if(m_pJoint->joint_style==2&&JointDesDlg.m_fPlankEBerWidth>0)
			UpdateOutterPlateWidth(pXPlate,JointDesDlg.m_nPlankWidthE,JointDesDlg.m_fPlankEBerWidth,xDesJointPara.m_pMainJg1);
		else
		{
			vertex[0]=pXPlate->vertex_list.GetFirst()->vertex;
			vertex[1]=pXPlate->vertex_list.GetNext()->vertex;
			vertex[2]=pXPlate->vertex_list.GetNext()->vertex;
			if(fabs(vertex[0].x-vertex[1].x)<eps)
			{
				offset_wide=max(0,0.5*(JointDesDlg.m_nPlankWidthE-fabs(vertex[2].x-vertex[1].x)));
				if(vertex[2].x<vertex[1].x)
					offset_wide=-offset_wide;
				pXPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
				pXPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
				pXPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
				pXPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			}
			else
			{
				offset_wide=max(0,0.5*(JointDesDlg.m_nPlankWidthE-fabs(vertex[1].x-vertex[0].x)));
				if(vertex[1].x<vertex[0].x)
					offset_wide=-offset_wide;
				pXPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
				pXPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
				pXPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
				pXPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
			}
		}
	}
	if(pYPlate)
	{
		pYPlate->designInfo.origin.UpdatePos(pYPlate->BelongModel());
		pYPlate->ucs.origin=pYPlate->designInfo.origin.Position();
		pYPlate->CalStdProfile();
		if(m_pJoint->joint_style==2&&JointDesDlg.m_fPlankFBerWidth>0)
			UpdateOutterPlateWidth(pYPlate,JointDesDlg.m_nPlankWidthF,JointDesDlg.m_fPlankFBerWidth,xDesJointPara.m_pMainJg1);
		else
		{
			vertex[0]=pYPlate->vertex_list.GetFirst()->vertex;
			vertex[1]=pYPlate->vertex_list.GetNext()->vertex;
			vertex[2]=pYPlate->vertex_list.GetNext()->vertex;
			if(fabs(vertex[0].x-vertex[1].x)<eps)
			{
				offset_wide=max(0,0.5*(JointDesDlg.m_nPlankWidthF-fabs(vertex[2].x-vertex[1].x)));
				if(vertex[2].x<vertex[1].x)
					offset_wide=-offset_wide;
				pYPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
				pYPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
				pYPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
				pYPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			}
			else
			{
				offset_wide=max(0,0.5*(JointDesDlg.m_nPlankWidthF-fabs(vertex[1].x-vertex[0].x)));
				if(vertex[1].x<vertex[0].x)
					offset_wide=-offset_wide;
				pYPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
				pYPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
				pYPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
				pYPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
			}
		}
	}
	if(JointDesDlg.m_bAdjustWrapPlateLen&&pXPlate&&pYPlate)
	{	//�Զ���������ְ峤��	wht 10-01-13
		double min_y=10000000,max_y=-10000000;
		PROFILE_VERTEX *pVertex=NULL;
		for(pVertex=pXPlate->vertex_list.GetFirst();pVertex;pVertex=pXPlate->vertex_list.GetNext())
		{	//���Ҹְ���СY�����Լ����Y����
			if(pVertex->vertex.y<min_y)
				min_y=pVertex->vertex.y;
			if(pVertex->vertex.y>max_y)
				max_y=pVertex->vertex.y;
		}
		for(pVertex=pYPlate->vertex_list.GetFirst();pVertex;pVertex=pYPlate->vertex_list.GetNext())
		{	//���Ҹְ���СY�����Լ����Y����
			coord_trans(pVertex->vertex,pYPlate->ucs,TRUE);
			coord_trans(pVertex->vertex,pXPlate->ucs,FALSE);
			if(pVertex->vertex.y<min_y)
				min_y=pVertex->vertex.y;
			if(pVertex->vertex.y>max_y)
				max_y=pVertex->vertex.y;
		}
		for(pVertex=pXPlate->vertex_list.GetFirst();pVertex;pVertex=pXPlate->vertex_list.GetNext())
		{	//�������ְ�ĳ���
			if(pVertex->vertex.y-min_y<max_y-pVertex->vertex.y)
				pVertex->vertex.y=min_y;
			else
				pVertex->vertex.y=max_y;
		}
		for(pVertex=pYPlate->vertex_list.GetFirst();pVertex;pVertex=pYPlate->vertex_list.GetNext())
		{	//�������ְ�ĳ���
			if(pVertex->vertex.y-min_y<max_y-pVertex->vertex.y)
				pVertex->vertex.y=min_y;
			else
				pVertex->vertex.y=max_y;
			coord_trans(pVertex->vertex,pXPlate->ucs,TRUE);
			coord_trans(pVertex->vertex,pYPlate->ucs,FALSE);
		}
	}
	if(pXPadPlate)
	{
		//��ʼ����˨���߾�
		CLsRef* pLsRef=pXPadPlate->GetFirstLsRef();
		LSSPACEEX space;
		IBoltLibrary::GetLsSpaceEx(pLsRef->GetLsPtr()->d,&space);
		int end_space=space.WasherEdgeSpace>0?space.WasherEdgeSpace:space.EndSpace;
		pXPadPlate->gasketEdgeSpace.berSpace=end_space;
		pXPadPlate->gasketEdgeSpace.wingSpace=end_space;
		pXPadPlate->gasketEdgeSpace.endSpaceS=end_space;
		pXPadPlate->gasketEdgeSpace.endSpaceE=end_space;
		//
		pXPadPlate->designInfo.origin.UpdatePos(pXPadPlate->BelongModel());
		pXPadPlate->ucs.origin=pXPadPlate->designInfo.origin.Position();
		pXPadPlate->CalStdProfile();
		//Ӧ�ñ�֤��˨λ�ڸְ��м�λ��   wht 10-01-13
		vertex[0]=pXPadPlate->vertex_list.GetFirst()->vertex;
		vertex[1]=pXPadPlate->vertex_list.GetNext()->vertex;
		vertex[2]=pXPadPlate->vertex_list.GetNext()->vertex;
		if(fabs(vertex[0].x-vertex[1].x)<eps)
		{
			offset_wide=0.5*(JointDesDlg.m_nPlankWidthG-fabs(vertex[2].x-vertex[1].x));
			if(vertex[2].x<vertex[1].x)
				offset_wide=-offset_wide;
			pXPadPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
			pXPadPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
			pXPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			pXPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
		}
		else
		{
			offset_wide=0.5*(JointDesDlg.m_nPlankWidthG-fabs(vertex[1].x-vertex[0].x));
			if(vertex[1].x<vertex[0].x)
				offset_wide=-offset_wide;
			pXPadPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
			pXPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			pXPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			pXPadPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
		}
	}
	if(pYPadPlate)
	{
		//��ʼ����˨���߾�
		CLsRef* pLsRef=pYPadPlate->GetFirstLsRef();
		LSSPACEEX space;
		IBoltLibrary::GetLsSpaceEx(pLsRef->GetLsPtr()->d,&space);
		int end_space=space.WasherEdgeSpace>0?space.WasherEdgeSpace:space.EndSpace;
		pYPadPlate->gasketEdgeSpace.berSpace=end_space;
		pYPadPlate->gasketEdgeSpace.wingSpace=end_space;
		pYPadPlate->gasketEdgeSpace.endSpaceS=end_space;
		pYPadPlate->gasketEdgeSpace.endSpaceE=end_space;
		//
		pYPadPlate->designInfo.origin.UpdatePos(pYPadPlate->BelongModel());
		pYPadPlate->ucs.origin=pYPadPlate->designInfo.origin.Position();
		pYPadPlate->CalStdProfile();
		//Ӧ�ñ�֤��˨λ�ڸְ��м�λ��   wht 10-01-13
		vertex[0]=pYPadPlate->vertex_list.GetFirst()->vertex;
		vertex[1]=pYPadPlate->vertex_list.GetNext()->vertex;
		vertex[2]=pYPadPlate->vertex_list.GetNext()->vertex;
		if(fabs(vertex[0].x-vertex[1].x)<eps)
		{
			offset_wide=0.5*(JointDesDlg.m_nPlankWidthH-fabs(vertex[2].x-vertex[1].x));
			if(vertex[2].x<vertex[1].x)
				offset_wide=-offset_wide;
			pYPadPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
			pYPadPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
			pYPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			pYPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
		}
		else
		{
			offset_wide=0.5*(JointDesDlg.m_nPlankWidthH-fabs(vertex[1].x-vertex[0].x));
			if(vertex[1].x<vertex[0].x)
				offset_wide=-offset_wide;
			pYPadPlate->vertex_list.GetFirst()->vertex.x-=offset_wide;
			pYPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			pYPadPlate->vertex_list.GetNext()->vertex.x+=offset_wide;
			pYPadPlate->vertex_list.GetNext()->vertex.x-=offset_wide;
		}
	}
	//�����ð��������������󣬸���������������ȡ��������Ӹ˼������Ϣ
	if(pXPlate)
		RetrieveWrapPlateLjRodPara(pXPlate);
	if(pYPlate)
		RetrieveWrapPlateLjRodPara(pYPlate);
	return TRUE;
}
#endif