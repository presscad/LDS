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
//���㹲������߽Ǹ����׼�ǸֵĽ����(׼�߽����)
//ucsΪ��ǰ�������������ϵ
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
	if(pBaseNode[0]==pBaseNode[1])	//����׼�Ǹ�ͬһ��׼�ڵ�
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
	else if(pRayJg)	//����׼�Ǹָ����Լ��Ļ�׼�ڵ�
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

#ifdef ajkdlfdaadfadfadfa	//�ݲ�ʹ��						 
void CLDSView::OnShareJdbDesign() 
{	
	m_nPrevCommandID=ID_SHARE_JDB_DESIGN;
	m_sPrevCommandName="�ظ����ð�";
	CLDSPlate *pCurPlate=NULL;
	CLDSNode *pBaseNode[2]={NULL};
	CLDSLineAngle *pRayJg=NULL,*pBaseJg[2]={NULL};
	CDesignLjPartPara *pRayJgPara=NULL,*pLjJgParaArr[2]={NULL};
	int i;
	double quitdist[2]={0.0};
	double base_wing_wide[2],base_wing_thick[2],ray_wing_wide,ray_wing_thick;
	LSSPACE_STRU basespace[2],rayspace;
	f3dPoint vec;
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	if(!LockFunc())
		return;
	UINT nRetCode=1;	//���Ѳ���Ҫ��������״̬�� wjh-2017.9.18
	Ta.BeginUndoListen();
	try
	{
#ifdef DOG_CHECK
		if(nRetCode!=1)
			throw "�����ܹ�����,�������!";
#endif
		pCurPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pCurPlate->face_N = 1;
		pCurPlate->jdb_style = 6;	//���ð�
		f3dLine line3d;
		f3dPoint *point3d=NULL;
		//�л���������ʾģʽ
		g_pSolidSet->SetDisplayType(DISP_LINE);
		Invalidate(FALSE);
		for(i=0;i<2;i++)
		{
			if(i==0)
				g_pPromptMsg->SetMsg("��ѡ���ð�ĵ�һ����׼�Ǹ�");
			else
				g_pPromptMsg->SetMsg("��ѡ���ð�ĵڶ�����׼�Ǹ�");
			while(pBaseJg[i]==NULL)
			{
				if(g_pSolidSnap->SnapLine(line3d)>0)
				{
					pBaseJg[i] = (CLDSLineAngle*)console.FromPartHandle(line3d.ID,CLS_LINEANGLE);
					pLjJgParaArr[i]=pCurPlate->designInfo.partList.append();
					pLjJgParaArr[i]->m_bAdjustLjEndPos=FALSE;	//���ð�������׼�˼��Գ�ʱ������Ҫ�����ڷ�λ�� wht 10-02-26
					pLjJgParaArr[i]->hPart=pBaseJg[i]->handle;
				}
			}
			g_pSolidDraw->SetEntSnapStatus(line3d.ID);
			if(i==1)//������ʾ������,��Ȼ�û���֪���Ƿ�ѡ���˴˹�����ʾ״̬�ͱ������
				Sleep(500);	
		}
		
		for(i=0;i<2;i++)
		{
			if(i==0)
				g_pPromptMsg->SetMsg("��ѡ���ð�ĵ�һ����׼�ڵ�(ӦΪ��һ����׼�ǸֵĶ˽ڵ�");
			else
				g_pPromptMsg->SetMsg("��ѡ���ð�ĵڶ�����׼�ڵ�(ӦΪ�ڶ�����׼�ǸֵĶ˽ڵ�");
			//������ʾ��ǰ�Ļ�׼�Ǹ�
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pBaseJg[i]->handle);
			while(pBaseNode[i]==NULL)
			{
				if(g_pSolidSnap->SnapPoint (point3d,TRUE)>0)
				{
					pBaseNode[i] = console.FromNodeHandle(point3d->ID);
					if(i==0)	//��һ����׼�ڵ�Ϊ���Ӱ��׼�ڵ�
					{
						if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseNode[0]->dwPermission))
							throw "û�д˽ڵ���޸�Ȩ�ޣ�";
						pCurPlate->designInfo.m_hBaseNode = point3d->ID;
					}
				}
				else	//��ֹ���	
					throw "��;�˳�,���ʧ��!";
			}
			g_pSolidDraw->SetEntSnapStatus(point3d->ID);
			CSharePlankDesignDlg dlg;
			if( pBaseNode[i]==pBaseJg[i]->pStart)
			{	
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwStartPermission))
					throw "û�д˽Ǹ�ʼ�˵��޸�Ȩ�ޣ�";
				pBaseJg[i]->feature = 10;	//ʼ������
			}
			else if(pBaseNode[i]==pBaseJg[i]->pEnd)
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwEndPermission))
					throw "û�д˽Ǹ��ն˵��޸�Ȩ�ޣ�";
				pBaseJg[i]->feature = 11;	//�ն�����
			}
			else
				throw "��ѡ��׼�Ǹ����׼�ڵ㲻һ��,������Ч";
			if(i==0)
				g_pPromptMsg->SetMsg("�������һ���Ǹֵ������Ӱ��Ե�ĺ���");
			else
				g_pPromptMsg->SetMsg("������ڶ����Ǹֵ������Ӱ��Ե�ĺ���");
			if(pBaseJg[i]->feature==10)
			{
				dlg.m_sLsGuiGe.Format("%d",pBaseJg[i]->connectStart.d);
				GetLsSpace(basespace[i],pBaseJg[i]->connectStart.d);
				dlg.m_nLsN = pBaseJg[i]->connectStart.wnConnBoltN;
				dlg.m_nLsSpace = basespace[i].SingleRowSpace;
				//��ղ�����˨
				dlg.m_fOddment = basespace[i].EndSpace+basespace[i].SingleRowSpace/2;//�˾� 
				pLjJgParaArr[i]->end_space=basespace[i].EndSpace;
				pLjJgParaArr[i]->start0_end1=0;
			}
			else
			{
				dlg.m_sLsGuiGe.Format("%d",pBaseJg[i]->connectEnd.d);
				dlg.m_nLsN = pBaseJg[i]->connectEnd.wnConnBoltN;
				GetLsSpace(basespace[i],pBaseJg[i]->connectEnd.d);
				dlg.m_nLsSpace = basespace[i].SingleRowSpace;
				//��ղ�����˨
				dlg.m_fOddment = basespace[i].EndSpace+basespace[i].SingleRowSpace/2;//�˾�
				pLjJgParaArr[i]->end_space=basespace[i].EndSpace;
				pLjJgParaArr[i]->start0_end1=1;
			}
			if(dlg.DoModal()!=IDOK)
				throw "��Ƴ���";
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
			quitdist[i]=dlg.m_fOddment-basespace[i].EndSpace;	//����
			g_pSolidDraw->ReleaseSnapStatus();	//�����׼�Ǹֵĸ�����ʾ״̬
		}
		//-------���ð��������ϵ�Ľ���------------
		f3dPoint vec1,pt1,pt2;
		//��������׼�Ǹ��ϸ�ƽ��ʱ�ķ��߼������ WJH-2004/07/01
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
		//������ɵ������ܽϴ���Ϊ������ķ��߼���
		//�������ķ��߼����㷨��ͬ����������ز�һ��ʱ��
		//�ͻ���ɽϴ�����
		//if(m_eViewFlag==RANDOM_VIEW)
		//pCurPlate->ucs.axis_z = g_ucs.axis_z;
		//else
		{
			//��������׼�Ǹ��ϸ�ƽ��ʱ�ķ��߼������ WJH-2004/07/01
			pCurPlate->ucs.axis_z = CalFaceNorm(pBaseJg[0]->Start(),pBaseJg[0]->End(),pBaseJg[1]->Start(),pBaseJg[1]->End());//cross_prod(vec1,vec2);
			normalize(pCurPlate->ucs.axis_z);
			if(dot_prod(console.GetActiveView()->ucs.axis_z,pCurPlate->ucs.axis_z)<0)
				pCurPlate->ucs.axis_z = -1.0*pCurPlate->ucs.axis_z;
		}
		pCurPlate->cfgword=pBaseNode[0]->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pCurPlate->ucs.origin = CalSharePlankRayZhun(pBaseJg,pBaseNode,NULL,pCurPlate->ucs.axis_z);
		pCurPlate->designInfo.iProfileStyle0123=1;
		pCurPlate->designInfo.m_hBasePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.m_hBaseNode=pBaseNode[0]->handle;
		pCurPlate->designInfo.iFaceType=1;
		pCurPlate->designInfo.origin.datum_pos_style=3;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.norm_style=2;		//���Ǹֽ����߷���
		pCurPlate->designInfo.norm.hVicePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.norm.hCrossPart=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.nearVector=console.GetActiveView()->ucs.axis_z;//���Ʒ���
		//-------���ð��������ϵ�Ľ���------------
		
		g_pPromptMsg->SetMsg("��������߽Ǹֹ��ô����Ӱ�,��ѡ��,������յ�һ�����");
		int iBoltLayoutStyle=0;	//˫�Ų�����˨ʱ ��˨������ʽ 0.�����߲����� 1.Զ�����߲�����
		JGZJ ray_jg_zj;
		if(g_pSolidSnap->SnapLine(line3d)>0)
		{
			CJdbJgJointDlg ray_jg_dlg;
			pRayJg = (CLDSLineAngle*)console.FromPartHandle(line3d.ID,CLS_LINEANGLE);
			if(pRayJg==NULL)
				throw "ѡ���˴���������������ʧ��!";
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
				pRayJg->feature = 10;	//ʼ������
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
					throw "��Ƴ���";
				if(ray_jg_dlg.m_fOddment!=pRayJg->startOdd())
					pRayJg->desStartOdd.m_iOddCalStyle=2;
				sscanf(ray_jg_dlg.m_sLsGuiGe,"%d",&pRayJg->connectStart.d);
				rayspace = ray_jg_dlg.LsSpace;
				if(ray_jg_dlg.m_nLsRowsN==0)
					pRayJg->connectEnd.rows=1;	//������˨
				else 
					pRayJg->connectEnd.rows=2;	//˫����˨
				//˫����˨������ʽ 
				iBoltLayoutStyle=ray_jg_dlg.m_iLsLayOutStyle;
				ray_jg_zj=ray_jg_dlg.jgzj;	//�Ǹ�׼��
				pRayJg->connectStart.wnConnBoltN = ray_jg_dlg.m_nLsN;
				pRayJg->SetStartOdd(ray_jg_dlg.m_fOddment);
			}
			else if(pRayJg->pEnd==pBaseNode[0]||pRayJg->pEnd==pBaseNode[1])
			{
				pRayJg->feature = 11;	//�ն�����
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
					throw "��Ƴ���";
				if(ray_jg_dlg.m_fOddment!=pRayJg->endOdd())
					pRayJg->desEndOdd.m_iOddCalStyle=2;
				sscanf(ray_jg_dlg.m_sLsGuiGe,"%d",&pRayJg->connectEnd.d);
				rayspace = ray_jg_dlg.LsSpace;
				if(ray_jg_dlg.m_nLsRowsN==0)
					pRayJg->connectEnd.rows=1;	//������˨
				else 
					pRayJg->connectEnd.rows=2;	//˫����˨
				//˫����˨������ʽ 
				iBoltLayoutStyle=ray_jg_dlg.m_iLsLayOutStyle;
				ray_jg_zj=ray_jg_dlg.jgzj;	//�Ǹ�׼��
				pRayJg->connectEnd.wnConnBoltN = ray_jg_dlg.m_nLsN;
				pRayJg->SetEndOdd(ray_jg_dlg.m_fOddment);
			}
			else
				throw "��ѡ�������߽Ǹ����׼�ڵ㲻һ��,������Ч";
		}
		static CPlateBasicParaDlg share_dlg;
		share_dlg.m_bEnableWeld=FALSE; //�޺��ӱ�
		if(share_dlg.DoModal()!=IDOK)
			throw "��;�˳�,���ʧ��!";
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
				pLjJgParaArr[i]->angle.cur_wing_x0_y1=0;	//X֫Ϊ��ǰ����֫
			}
			else
			{
				norm = pBaseJg[i]->get_norm_y_wing();
				direct = -pBaseJg[i]->GetWingVecY();
				pLjJgParaArr[i]->angle.cur_wing_x0_y1=1;	//Y֫Ϊ��ǰ����֫
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
					pBolt->cfgword=pBaseJg[i]->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pBolt->AddL0Thick(pBaseJg[i]->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//�Ǹ���㶨λ
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pBaseJg[i]->handle;
					pBolt->des_base_pos.wing_offset_dist=jgzj.g;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pBaseJg[i]->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_y_wing()))
					{	//X֫�ϵ���˨
						//��X֫������˨λ��ƫ��
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y֫�ϵ���˨
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//��Y֫������˨λ��ƫ��
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=1;
					}
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=0;//ʼ-->��
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
					pBolt->cfgword=pBaseJg[i]->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pBolt->AddL0Thick(pBaseJg[i]->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//�Ǹ���㶨λ
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pBaseJg[i]->handle;
					pBolt->des_base_pos.wing_offset_dist=jgzj.g;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pBaseJg[i]->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pBaseJg[i]->get_norm_y_wing()))
					{	//X֫�ϵ���˨
						//��X֫������˨λ��ƫ��
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y֫�ϵ���˨
						pBolt->des_work_norm.hVicePart=pBaseJg[i]->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//��Y֫������˨λ��ƫ��
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=1;
					}
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=1;//��-->ʼ
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
			if(pRayJg->feature==10)	//ʼ������
			{
				pRayJgPara->start0_end1=0;
				//pRayJg->desStartPos.datum_pos_style=15;	//ֱ��ָ����������
				pRayJg->desStartPos.SetToDatumPointStyle();
				//���㶨λ��ʽ
				pRayJg->desStartPos.datumPoint.datum_pos_style=3; //�Ǹ����߽���
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
					pBolt->cfgword=pRayJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pBolt->AddL0Thick(pRayJg->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//�Ǹ���㶨λ
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pRayJg->handle;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pRayJg->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_y_wing()))
					{	//X֫�ϵ���˨
						//��X֫������˨λ��ƫ��
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y֫�ϵ���˨
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//��Y֫������˨λ��ƫ��
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=1;
					}
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=0;//ʼ-->��
					if(pRayJg->connectStart.rows==1)	//��������
					{
						pBolt->des_base_pos.wing_offset_dist=jgzj.g;	
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.SingleRowSpace;
					}
					else if(pRayJg->connectStart.rows==2)	//˫������
					{
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.doubleRowSpace;
						if(iBoltLayoutStyle==0)	//��������һ������
						{
							if(i%2==0)	//ż������˨
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g1;
							else		//��������˨
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
						}
						else	//Զ������һ������
						{
							if(i%2==0)	//ż������˨
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
							else		//��������˨
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
				//pRayJg->des_end_pos.datum_pos_style=15;;	//ֱ��ָ����������
				pRayJg->desEndPos.SetToDatumPointStyle();
				//���㶨λ��ʽ
				pRayJg->desEndPos.datumPoint.datum_pos_style=3; //�Ǹ����߽���
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
					pBolt->cfgword=pRayJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pBolt->AddL0Thick(pRayJg->handle,TRUE);
					pBolt->AddL0Thick(pCurPlate->handle,TRUE);
					pBolt->CalGuigeAuto();
					pBolt->set_norm(pCurPlate->ucs.axis_z);
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;	//�Ǹ���㶨λ
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=4;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=0;
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pRayJg->handle;
					sprintf(pBolt->des_base_pos.norm_offset.key_str,"-0X%X",pRayJg->handle);
					pBolt->des_work_norm.norm_style=1;
					if(fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_x_wing())>fabs(pCurPlate->ucs.axis_z*pRayJg->get_norm_y_wing()))
					{	//X֫�ϵ���˨
						//��X֫������˨λ��ƫ��
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=0;
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=0;
					}
					else
					{	//Y֫�ϵ���˨
						pBolt->des_work_norm.hVicePart=pRayJg->handle;
						pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=pBolt->des_base_pos.offset_wing=1;	//��Y֫������˨λ��ƫ��
						pBolt->des_work_norm.direction=0;		//����
						pBolt->des_work_norm.norm_wing=1;
					}
					
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=pBolt->des_base_pos.direction=1;//��-->ʼ
					if(pRayJg->connectEnd.rows==1)	//��������
					{
						pBolt->des_base_pos.wing_offset_dist=jgzj.g;	
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.SingleRowSpace;
					}
					else if(pRayJg->connectEnd.rows==2)	//˫������
					{
						pBolt->des_base_pos.len_offset_dist=rayspace.EndSpace+i*rayspace.doubleRowSpace;
						if(iBoltLayoutStyle==0)	//��������һ������
						{
							if(i%2==0)	//ż������˨
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g1;
							else		//��������˨
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
						}
						else	//Զ������һ������
						{
							if(i%2==0)	//ż������˨
								pBolt->des_base_pos.wing_offset_dist=ray_jg_zj.g2;
							else		//��������˨
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
			throw "��;�˳�!";
		pCurPlate->DesignPlate();
		pCurPlate->SetModified();
		pCurPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pCurPlate->GetSolidPartObject());
		g_pSolidDraw->ReleaseSnapStatus();
		OnOperOther();
		ReleaseFunc();	//�⿪������������״̬
		g_pPromptMsg->Destroy();
		static CMirMsgDlg mir_dlg;
		if(mir_dlg.DoModal()==IDOK)
			MirTaAtom(pCurPlate,mir_dlg.mirmsg);
		g_pSolidDraw->Draw();
		//�л���ʵ����ʾģʽ
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
		Ta.EndUndoListen();
	}
	catch(char *sError)
	{
		Ta.EndUndoListen();
		//�����쳣���ʧ��,ɾ��ϵͳ�������ɵĴ���ڵ��
		if(pCurPlate!=NULL)
			console.DeletePart(pCurPlate->handle);
		AfxMessageBox(sError);	//��ʾ�쳣������Ϣ
		g_pSolidDraw->ReleaseSnapStatus();
		ReleaseFunc();	//�⿪������������״̬
		OnOperOther();
		g_pPromptMsg->Destroy();
		return;
	}
	
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	ReleaseFunc();	//�⿪������������״̬
}
#endif

void CLDSView::OnShareJdbDesign() 
{
	m_nPrevCommandID=ID_SHARE_JDB_DESIGN;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Common Plate";
#else 
	m_sPrevCommandName="�ظ����ð�";
#endif
	Command("GongYongBan");
}
int CLDSView::ShareJdbDesign() 
{	
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	//-----vvvvvvv-ͨ���Ի�������ְ������Ϣ---------
	//�ڿ�ʼλ������ְ������Ϣ������ְ����޸�֮�����߽Ǹ�ƫ������Ҫ�����޸� wht 16-10-21
	static CPlateBasicParaDlg share_dlg;
	share_dlg.m_bEnableWeld=FALSE; //�޺��ӱ�
	if(share_dlg.DoModal()!=IDOK)
		return FALSE;
	//-----vvvvvvv-�����������׽���ð�����ĽǸֺͽڵ�---------
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verify;
	//ѡ���ð���Ҫ�Ļ�׼�Ǹֺͻ�׼�ڵ�
	CLDSLineAngle *pBaseJg[2]={NULL};
	CLDSNode *pBaseNode[2]={NULL};
	for(int i=0;i<2;i++)
	{
		//ѡ���ð�Ļ�׼�Ǹ�
		if(i==0)
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Please select common plate's first datum angle:");
		else
			cmdStr.Format("Please select common plate's second datum angle:");
#else 
			cmdStr.Format("��ѡ���ð�ĵ�һ����׼�Ǹ�:");
		else
			cmdStr.Format("��ѡ���ð�ĵڶ�����׼�Ǹ�:");
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
			continue;	//���Զ�ʶ����˼���ǰ��ʼĩ�� 
		//ѡ���׼�Ǹ��ϵĶ˽ڵ�
		if(i==0)
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Please select common plate's first datum node(it should be end point of first datum angle");
		else
			cmdStr.Format("Please select common plate's second datum node(it should be end point of second datum angle");
#else 
			cmdStr.Format("��ѡ���ð�ĵ�һ����׼�ǸֵĶ˽ڵ�:");
		else
			cmdStr.Format("��ѡ���ð�ĵڶ�����׼�ǸֵĶ˽ڵ�:");
#endif
		pCmdLine->FillCmdLine(cmdStr,"");
		verify.ClearSnapFlag();
		verify.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		{//�˴��ӻ������������ƽڵ�͹����ʾ����������
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
					AfxMessageBox("Without modify permission of the node��");
#else
					AfxMessageBox("û�д˽ڵ���޸�Ȩ�ޣ�");
#endif
					return FALSE;
				}
				else if(pBaseNode[i]==pBaseJg[i]->pStart||pBaseNode[i]==pBaseJg[i]->pEnd)
					break;
			}
		}}
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseNode[i]->handle));
		//�ж�ѡ�еĽڵ���߽Ǹ��Ƿ����޸�Ȩ��
		if( pBaseNode[i]==pBaseJg[i]->pStart)
		{	
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwStartPermission))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Without modify permission of angle's start��");
#else
				AfxMessageBox("û�д˽Ǹ�ʼ�˵��޸�Ȩ�ޣ�");
#endif
				return FALSE;
			}
			pBaseJg[i]->feature = 10;	//ʼ������
		}
		else if(pBaseNode[i]==pBaseJg[i]->pEnd)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[i]->dwEndPermission))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Without modify permission of angle's end��");
#else 
				AfxMessageBox("û�д˽Ǹ��ն˵��޸�Ȩ�ޣ�");
#endif
				return FALSE;
			}
			pBaseJg[i]->feature = 11;	//�ն�����
		}
	}
	//ѡ���ð�����߽Ǹ�
	CLDSLineAngle *pRayJg=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("If there is a ray angle Shared with the connection plate, please choose, otherwise click empty");
#else 
	cmdStr.Format("��������߽Ǹֹ��ô����Ӱ�,��ѡ��,����ֱ�ӻس�:");
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
		{	//�س�ȷ�ϲ����������˼�
			pRayJg=NULL;
			break;
		}
		if(pRayJg && pRayJg->pStart && pRayJg->pEnd)
		{	//��ѡ�������˼�
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
		pCmdLine->FinishCmdLine("�����߽Ǹ�");
	//-----vvvvvvv-��ƹ��ð棬��乲�ð����Ʋ���---------
	try{
		CUndoOperObject undo(&Ta,true);
		CLDSPlate *pCurPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pCurPlate->face_N = 1;
		pCurPlate->jdb_style = 6;	//���ð�
		pCurPlate->Thick=share_dlg.m_nPlankThick;
		pCurPlate->cMaterial=CSteelMatLibrary::RecordAt(share_dlg.m_iMaterial).cBriefMark;
		pCurPlate->SetPartNo(share_dlg.m_sPartNo.GetBuffer());
		pCurPlate->iSeg=SEGI(share_dlg.m_sSegI.GetBuffer());
		pCurPlate->cfgword=pBaseJg[0]->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		//��������׼�Ǹ��ϸ�ƽ��ʱ�ķ��߼������ WJH-2004/07/01
		pCurPlate->ucs.axis_z = CalFaceNorm(pBaseJg[0]->Start(),pBaseJg[0]->End(),pBaseJg[1]->Start(),pBaseJg[1]->End());
		normalize(pCurPlate->ucs.axis_z);
		if(dot_prod(console.GetActiveView()->ucs.axis_z,pCurPlate->ucs.axis_z)<0)
			pCurPlate->ucs.axis_z = -1.0*pCurPlate->ucs.axis_z;
		pCurPlate->ucs.origin = CalSharePlankRayZhun(pBaseJg,pBaseNode,NULL,pCurPlate->ucs.axis_z);
		pCurPlate->designInfo.iProfileStyle0123=1;
		pCurPlate->designInfo.m_hBasePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.m_hBaseNode=pBaseNode[0]->handle;
		pCurPlate->designInfo.m_bEnableFlexibleDesign=TRUE;	//�����������
		pCurPlate->designInfo.iFaceType=1;
		pCurPlate->designInfo.origin.datum_pos_style=3;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1=pBaseJg[0]->handle;
		pCurPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.norm_style=2;		//���Ǹֽ����߷���
		pCurPlate->designInfo.norm.hVicePart=pBaseJg[0]->handle;
		pCurPlate->designInfo.norm.hCrossPart=pBaseJg[1]->handle;
		pCurPlate->designInfo.norm.nearVector=console.GetActiveView()->ucs.axis_z;//���Ʒ���
		pCurPlate->designInfo.origin.UpdatePos(console.GetActiveModel());
		pCurPlate->ucs.origin=pCurPlate->designInfo.origin.Position();
		//��ʼ�����ð�����Ӹ˼������Ϣ
		int x_wing0_y_wing1=0;
		CDesignLjPartPara *pLjJgParaArr[2]={NULL};
		for(int i=0;i<2;i++)
		{
			pLjJgParaArr[i]=pCurPlate->designInfo.partList.Add(pBaseJg[i]->handle);
			pLjJgParaArr[i]->m_bAdjustLjEndPos=FALSE;	//���ð�������׼�˼��Գ�ʱ������Ҫ�����ڷ�λ�� wht 10-02-26
			pLjJgParaArr[i]->hPart=pBaseJg[i]->handle;
			IsInsideJg(pBaseJg[i],pCurPlate->ucs.axis_z,&x_wing0_y_wing1);
			pLjJgParaArr[i]->angle.cur_wing_x0_y1=(BYTE)x_wing0_y_wing1;
			//�Ի�׼�Ǹֲ�����˨
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pBaseJg[i]->handle);
			CLayAngleBoltDlg dlg;
			ATOM_LIST<CDesignLsPara>ls_list;
			dlg.m_iOddCalStyle=pBaseJg[i]->desEndOdd.m_iOddCalStyle;
			if(pBaseJg[i]->feature==10)
			{	//���ð�Ǹ���˨һ��Ϊ����,����Ϊ2 wht 16-10-21
				pBaseJg[i]->connectStart.wnConnBoltN=2;
				pBaseJg[i]->connectStart.rows=1;
				dlg.m_fOddment = pBaseJg[i]->startOdd();
				dlg.m_iRayDirection=0;	//�գ�>ʼ
			}
			else
			{	
				pBaseJg[i]->connectEnd.wnConnBoltN=2;
				pBaseJg[i]->connectEnd.rows=1;
				dlg.m_fOddment = pBaseJg[i]->endOdd();
				dlg.m_iRayDirection=1;	//�գ�>ʼ
			}
			LSSPACE_STRU basespace;
			GetLsSpace(basespace,pBaseJg[i]->connectEnd.d);
			if(pBaseJg[i]->GetClassTypeId()==CLS_LINEANGLE)	
				dlg.m_pLinePart = pBaseJg[i];
			dlg.m_pNode = pBaseNode[i];
			dlg.m_pLsList=&ls_list;
			//���ð�ʱ�Ĳ���
			dlg.m_bIncPlateProfilePara = TRUE;
			dlg.m_bTwoEdgeProfile = TRUE;
			//��ղ�����˨
			dlg.m_nLsEndSpace = basespace.EndSpace+basespace.SingleRowSpace/2;//�˾�
			dlg.viewNorm = console.GetActiveView()->ucs.axis_z;
			if(dlg.DoModal()!=IDOK)
			{
				console.DeletePart(pCurPlate->handle);
				return FALSE;
			}
			//��ȡ�û�����
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
			//���������ʱ����Ϣ
			pLjJgParaArr[i]->angle.cbSpaceFlag=dlg.cbSpaceFlag;
			pLjJgParaArr[i]->end_space=dlg.m_fPlateEdgeSpace;
			pLjJgParaArr[i]->wing_space=dlg.m_fWingSpace;
			pLjJgParaArr[i]->ber_space=dlg.m_fBerSpace;
			pLjJgParaArr[i]->iFaceNo = 1;
			pLjJgParaArr[i]->angle.bTwoEdge = dlg.m_bTwoEdgeProfile;
			//�����û������ڽǸ��ϲ�����˨
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
				pBolt->cfgword=pBaseJg[i]->cfgword;	//������˨��ĺ����׼������ĺ�һ��
				//������˨ͨ���Լ�����ƫ����
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
				if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//����㶨λ
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
						{	//������˨λ�ڽ���Ǹ���
							pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
							pBolt->AddL0Thick(pCrossAngle->handle,TRUE);
						}
					}
				}
				//����˨��ӵ�����
				pCurPlate->AppendLsRef(pBolt->GetLsRef());
				double g=0.0;
				//����˨����Ǹ�
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
		{	//�������߽Ǹֵ������ð�����Ӹ˼���Ʋ���
			CDesignLjPartPara *pRayJgPara=pCurPlate->designInfo.partList.Add(pRayJg->handle);
			pRayJgPara->hPart=pRayJg->handle;
			BOOL bInsideJg=IsInsideJg(pRayJg,pCurPlate->ucs.axis_z,&x_wing0_y_wing1);
			pRayJgPara->angle.cur_wing_x0_y1=(BYTE)x_wing0_y_wing1;
			pCurPlate->ucs.origin = CalSharePlankRayZhun(pBaseJg,pBaseNode,pRayJg,pCurPlate->ucs.axis_z);
			//�������߽Ǹ���˨
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pRayJg->handle);
			ATOM_LIST<CDesignLsPara>ls_list;
			CLayAngleBoltDlg ray_jg_dlg;
			ray_jg_dlg.m_bIncPlateProfilePara = TRUE;
			if(pRayJg->pStart==pBaseNode[0]||pRayJg->pStart==pBaseNode[1])
			{
				pRayJg->feature = 10;	//ʼ������
				ray_jg_dlg.m_iRayDirection=0;	//ʼ��>��
				//����Ǹ�����ͷ
				if(pBaseJg[0])
					pRayJg->desStartOdd.m_hRefPart1=pBaseJg[0]->handle;
				if(pBaseJg[1])
					pRayJg->desStartOdd.m_hRefPart2=pBaseJg[1]->handle;
				pRayJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
				pRayJg->desStartOdd.m_iOddCalStyle=0;	//������ײ��������ͷ
				pRayJg->CalStartOddment();
				ray_jg_dlg.m_fOddment=pRayJg->startOdd();
			}
			else if(pRayJg->pEnd==pBaseNode[0]||pRayJg->pEnd==pBaseNode[1])
			{
				pRayJg->feature = 11;	//�ն�����
				ray_jg_dlg.m_iRayDirection=1;	//�գ�>ʼ
				//����Ǹ�����ͷ
				if(pBaseJg[0])
					pRayJg->desEndOdd.m_hRefPart1=pBaseJg[0]->handle;
				if(pBaseJg[1])
					pRayJg->desEndOdd.m_hRefPart2=pBaseJg[1]->handle;
				pRayJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
				pRayJg->desEndOdd.m_iOddCalStyle=0;	//������ײ��������ͷ
				pRayJg->CalEndOddment();
				ray_jg_dlg.m_fOddment=pRayJg->endOdd();
			}
			if(pRayJg->feature==10)	//ʼ������
			{
				pRayJgPara->start0_end1=0;
				//pRayJg->des_start_pos.datum_pos_style=15;	//ֱ��ָ����������
				pRayJg->desStartPos.SetToDatumPointStyle();
				//�򵥶�λʱ��Ϊ��������Զ��жϸı���㶨λ���ͣ���Ϊ�˹�ָ��ģʽ wjh-2016.3.24
				if(pRayJg->desStartPos.jgber_cal_style==2)
					pRayJg->desStartPos.bUdfDatumPointMode=TRUE;
				//���㶨λ��ʽ
				pRayJg->desStartPos.datumPoint.datum_pos_style=3; //�Ǹ����߽���
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
				//pRayJg->desEndPos.datum_pos_style=15;;	//ֱ��ָ����������
				pRayJg->desEndPos.SetToDatumPointStyle();
				//�򵥶�λʱ��Ϊ��������Զ��жϸı���㶨λ���ͣ���Ϊ�˹�ָ��ģʽ wjh-2016.3.24
				if(pRayJg->desEndPos.jgber_cal_style==2)
					pRayJg->desEndPos.bUdfDatumPointMode=TRUE;
				//���㶨λ��ʽ
				pRayJg->desEndPos.datumPoint.datum_pos_style=3; //�Ǹ����߽���
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
			//������Ӳ���
			ray_jg_dlg.m_bIncPlateProfilePara = TRUE;
			ray_jg_dlg.m_bTwoEdgeProfile = TRUE;
			if(ray_jg_dlg.DoModal()!=IDOK)
			{
				console.DeletePart(pCurPlate->handle);
				return FALSE;
			}
			//��ȡ�û�����
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
			//�����û������ڽǸ��ϲ�����˨
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
				pBolt->cfgword=pRayJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
				//������˨ͨ���Լ�����ƫ����
				pBolt->AddL0Thick(pRayJg->handle,TRUE);
				pBolt->AddL0Thick(pCurPlate->handle,TRUE);
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//����㶨λ
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
						{	//������˨λ�ڽ���Ǹ���
							pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
							pBolt->AddL0Thick(pCrossAngle->handle,TRUE);
						}
					}
				}
				//����˨��ӵ�����
				pCurPlate->AppendLsRef(pBolt->GetLsRef());
				//����˨��ӵ�����
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
			//�����Ͳ���
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
		ReleaseFunc();	//�⿪������������״̬
		static CMirMsgDlg mir_dlg;
		if(mir_dlg.DoModal()==IDOK)
			MirTaAtom(pCurPlate,mir_dlg.mirmsg);
		g_pSolidDraw->Draw();
		//�л���ʵ����ʾģʽ
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
	}
	catch(char *sError)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		ReleaseFunc();	//�⿪������������״̬
		OnOperOther();
		AfxMessageBox(sError);	//��ʾ�쳣������Ϣ
		return FALSE;
	}
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	ReleaseFunc();	//�⿪������������״̬
	return TRUE;
}

//��ƽǸֽ�ͷ
void CLDSView::OnCommonJgJoint() 
{
	m_nPrevCommandID=ID_COMMON_JG_JOINT;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Angle Joint";
#else 
	m_sPrevCommandName="�ظ��Ǹֽ�ͷ";
#endif
	Command("CommonJgJoint");
}
int CLDSView::CommonJgJoint()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	//ѡ����ƽڵ�
	g_pSolidDraw->ReleaseSnapStatus();
	CLDSNode* pSelNode=NULL;
	CString cmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("CommonJgJoint Please select the node of joint:");
#else 
	cmdStr.Format("CommonJgJoint ��ѡ��Ҫ��ƽ�ͷ�Ľڵ�:");
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
	//ѡ��������׼�Ǹ�
	CLDSLineAngle *pFirstJg=NULL,*pLastJg=NULL;
	CSnapTypeVerify verify(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_GROUPLINEANGLE));
	verify.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	for(int i=0;i<2;i++)
	{
		CLDSLineAngle* pSelAngle=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("CommonJgJoint please select the %d datum angles in the nodes:",i+1);
#else
		cmdStr.Format("CommonJgJoint ��ѡ���ͷ�ĵ�%d�����ӻ�׼�Ǹ�:",i+1);
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
	//��ƽǸֽ�ͷ���
	return FinishDesignJoint(pSelNode,pFirstJg,pLastJg);
}
//���ýŶ�
void CLDSView::OnLayoutFootnail() 
{
	m_nPrevCommandID=ID_LAYOUT_FOOTNAIL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to layout climbing bolt";
#else 
	m_sPrevCommandName="�ظ����ýŶ�";
#endif
	Command("LayoutFootnail");
}
#include "FootNailSpaceDlg.h"
int CLDSView::LayoutFootnail()
{
	CString cmdStr;
	int nail_type=0;//0.�۸ֽŶ��� 1.�Ǹ�
	bool bDownToUp;	//�Ŷ��������ϲ���
	PARTSET partset;
	THANDLE start_handle,cur_handle=0,last_end_handle=0;
	f3dPoint start,end,vec,nail_pos,wing_x_vec,wing_y_vec,ls_pos;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSLinePart *pSelLinePart, *pFirstLinePart, *pLastLinePart=NULL;
	//-----VVVVVVV-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	if (CLsLibrary::GetFootNailFamily()==0)
	{	//��ϵͳδ��Ŷ����⣬����ʾ�û��趨�Է��������뽫�Ŷ�����Ϊ��ͨ��˨ wjh-2019.9.21
		CSelNailSeriesDlg setnails;
		setnails.DoModal();
	}
	if (CLsLibrary::GetFootNailFamily()==0)
	{
		AfxMessageBox("��˨����δָ���Ŷ����ϵ�еĽŶ�����!");
		return false;
	}
	//�л���ʵ����ʾģʽ
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	static char cToward='U';
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Layout climbing bolt[from up to down(D)/from down to up(U)]<%C>:",cToward);
#else 
	cmdStr.Format("���ýŶ�[���϶���(D)/���¶���(U)]<%C>:",cToward);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		//������������Ӧ������̧�����Ϣ��ͬ���س���Ч����ͬ
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::LBUTTONUP_AS_RETURN))
			return 0;	//��;�˳�
		m_bStartOpenWndSelTest=FALSE;	//������ܻᵼ����������������ʱһ�´α�Ϊ������Ŀ���״̬��wjh-2015.9.8
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
			pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
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
	pCmdLine->FillCmdLine("������ѡ��Ҫ���ýŶ�������<�յ�����������ѡ��>:","");
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
			//������������Ӧ������̧�����Ϣ��ͬ���س���Ч����ͬ
			if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::LBUTTONUP_AS_RETURN))
				return 0;	//��;�˳�
			m_bStartOpenWndSelTest=FALSE;	//������ܻᵼ����������������ʱһ�´α�Ϊ������Ŀ���״̬��wjh-2015.9.8
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
					AfxMessageBox("û��ѡ����Ч�ĸ˼�!������ѡ��!");
#endif
					g_pSolidDraw->SetEntSnapStatus(id_arr[i],FALSE);
					continue;
				}
				if(pLastLinePart!=NULL&&pLastLinePart==pSelLinePart)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Repeat to select the same leg member,Please select again!");
#else 
					AfxMessageBox("�ظ�ѡ����ͬһ������,������ѡ��!");
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
						AfxMessageBox("The selected rod has no connection with previous rod��Please select again!");
#else 
						AfxMessageBox("��ѡ�˼�����һ�˼�û�����ӣ�������ѡ��!");
#endif
						g_pSolidDraw->SetEntSnapStatus(pSelLinePart->handle,FALSE);
						continue;
					}
					else if(bDownToUp)	//�Ŷ��������ϲ���
					{
						if(pSelLinePart->pStart->Position(false).z>pSelLinePart->pEnd->Position(false).z)
						{		//��ѡ�ĵ�һ���˼�
							start_handle = pSelLinePart->pStart->handle;
							cur_handle = pSelLinePart->pEnd->handle;
						}
						else	//��ʼ���,��ֹ���
						{
							start_handle = pSelLinePart->pEnd->handle;
							cur_handle = pSelLinePart->pStart->handle;
						}
					}
					else //if(!bDownToUp) //�Ŷ��������²���
					{
						if(pSelLinePart->pStart->Position(false).z>pSelLinePart->pEnd->Position(false).z)
						{		//��ѡ�ĵ�һ���˼�
							start_handle = pSelLinePart->pEnd->handle;
							cur_handle = pSelLinePart->pStart->handle;
						}
						else	//��ʼ���,��ֹ���
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
					if(AfxMessageBox("��ȷ��Ҫ��ѡ�еĶ̽Ǹ��ϲ�����˨��?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#endif
					{
						partset.append(pSelLinePart);
						break;	//�ڶ̽Ǹ��ϲ�����˨ʱֻ�ܵ�����ĳһ���̽Ǹ��ϲ�����˨ 
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
				if(AfxMessageBox("��ȷ���Ѿ�ѡ����Ҫ������Ƶ����и˼���?",MB_YESNO|MB_ICONQUESTION)==IDYES)
#endif
					break;
				else
					continue;
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();

		CFootnailSpaceDlg naildlg;
		CLDSBolt ls(console.GetActiveModel()),*pLs;
		BOOL X0_OR_Y1_WING=FALSE;//FALSE:X֫ TRUE:Y֫
		double dist=0,used_dist=0,odd_dist=0,prev_odd=0,overlap_length=0;
		double extra_dist=0;	//��һ���˼������Ŀհ׳���
		//�жϸ˼�����
		pFirstLinePart = (CLDSLinePart*)partset.GetFirst();
		if(!pFirstLinePart)
			return 0;	//��;�˳�
		if(pFirstLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			JGZJ jgzj;
			double fPreRodRemainingLen=0;	//ǰһ�˼���ʣ�೤��,��˼����õ�һ����˨ʱ�Ŷ������Ҫ��ȥǰһ�˼�ʣ�೤�� wht 15-08-13
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
					//����нŶ��Ļ���������ʼ�Ŷ�
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Please select start climbing nails and press return key, none nails type 'N' key:","");
#else 
					pCmdLine->FillCmdLine("��ѡ����ʼ�Ŷ��س�,����ʼ�Ŷ�[N]:","");
#endif
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
					g_pSolidDraw->ReleaseSnapStatus();
					if(pStartBolt)
					{
						g_pSolidDraw->SetEntSnapStatus(pStartBolt->handle);
						//���Ŷ�λ�÷Ŵ���ʾ
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
					//����ʰȡ����ʼ�Ŷ��ڽǸ������ϵ�ͶӰ
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
						vec*=-1.0;	//��ת����->ʼ
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
						vec*=-1.0;	//��ת����->ʼ
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
				{	//�ڶ̽Ǹ��ϲ��ýŶ�ʱ���ն�==>ʼ�˲���	wht 09-12-03
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
				ls.m_hFamily= CLsLibrary::GetFootNailFamily();//�Ŷ�
				used_dist = 0;
				//Ԥ�е�ǰ�˼�ʣ�೤���Ƿ��㹻������һ���Ŷ� wht 15-08-13
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
						if(pCurJg->m_bFootNail)	//�ǸֽŶ�
						{
							naildlg.m_nNailSpace=30;
							naildlg.m_iLayWing=1;	//Y֫
						}
						if(naildlg.DoModal()!=IDOK)
							goto end;
						used_dist+=(naildlg.m_nNailSpace-naildlg.m_fPreRodRemainingLen);
						X0_OR_Y1_WING = naildlg.m_iLayWing;
					}
					//else	//�ϴ���ʣ��
					//	used_dist = odd_dist;
					used_dist+=overlap_length;
					overlap_length=0.0;	//�����ص����ֳߴ�һ�κ�����ص��ߴ�
					//���ýǸ��ϵĽŶ�ʱodd_dist��������
					//�����Ի�����ȷ�������жϵ�ǰ�Ǹ��Ƿ��㹻������һ�ŽŶ����ᵼ�±��β��ýŶ�ʧЧ
					//Ӧ�ڿ�ʼ���õ�ǰ�Ǹֵ�һ�ŽŶ��Ͳ�����ÿһ�ŽŶ���Ԥ�е�ǰ�Ǹ�ʣ�೤���Ƿ��㹻������һ�ŽŶ� wht 15-08-13
					/*if(used_dist>dist+extra_dist)
					{
						odd_dist = used_dist-dist-extra_dist;
						break;
					}*/
					//if(fabs(odd_dist)<EPS)		//�ϴ����û��ʣ��ֵ
						Add_Pnt(ls_pos,start,vec*ftol(used_dist));
					//else				//�ϴ����ʱ��ʣ��ֵ
					//{
					//	Add_Pnt(ls_pos,start,vec*ftol(odd_dist));
					//	odd_dist = 0.0;
					//}
					restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
					if(!X0_OR_Y1_WING)	//X֫�ϵĽŶ�
					{
						Add_Pnt(ls_pos,ls_pos,wing_x_vec*naildlg.m_nGDist);
						ls.set_norm(pCurJg->get_norm_x_wing());
						
						if(naildlg.m_bShiftWing)
							X0_OR_Y1_WING = !X0_OR_Y1_WING;	//XY֫������
					}
					else			//Y֫�ϵĽŶ�
					{
						Add_Pnt(ls_pos,ls_pos,wing_y_vec*naildlg.m_nGDist);
						ls.set_norm(pCurJg->get_norm_y_wing());
						
						if(naildlg.m_bShiftWing)
							X0_OR_Y1_WING = !X0_OR_Y1_WING;	//XY֫������
					}
					//��˨�����Ϣ���
					ls_pos=ls_pos-ls.get_norm()*pCurJg->GetThick();
					//
					sprintf(ls.des_base_pos.norm_offset.key_str,"-0X%X",pCurJg->handle);
					ls.EmptyL0DesignPara();	//�����˨ͨ�����
					ls.AddL0Thick(pCurJg->handle,TRUE,TRUE);
					//ls.L0 = pCurJg->GetThick();
					//ls.des_base_pos.bDatumPointInUcs=TRUE;
					ls.des_base_pos.datumPoint.datum_pos_style=1;	//�Ǹ��϶����Ϊ��׼
					ls.des_base_pos.datumPoint.des_para.RODEND.hRod=pCurJg->handle;
					ls.des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					ls.des_base_pos.hPart = pCurJg->handle;
					f3dPoint perp;
					SnapPerp(&perp,start,end,ls_pos);
					//TODO��������ܻ���������,����򵥵Ľ�ͷ��϶��һ��10,һ��-50,��overlap_length=-10,��ֵΪ10 wjh-2016.11.9
					double offset_dist = DISTANCE(perp,start)-extra_dist;
					//if(pCurJg==(CLDSLineAngle*)pFirstLinePart&&!nail_start.IsZero())
					//	offset_dist = DISTANCE(perp,nail_start);//����һ���Ǹ��нŶ���ʱ�����⴦��
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
					//�Ƿ�������ƹ�����˨����Ͻ�
					CLDSBolt *pNearBolt=naildlg.m_pMergeBolt;//NULL;
					//for(CLsRef* pLsRef=pCurJg->GetFirstLsRef();pLsRef!=NULL;pLsRef=pCurJg->GetNextLsRef())
					//{//EPS_COS2���ϵ������̫��������ԭ�Ȼ������ټ�ȥ0.03
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
						pLs->cfgword=pCurJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
						pLs->iSeg=pCurJg->iSeg;
						pLs->des_base_pos=ls.des_base_pos;
						pLs->des_work_norm=ls.des_work_norm;
						pLs->ucs.origin=ls_pos;
						//����˨���뵽�Ǹ� wht 11-01-14
						pCurJg->AppendMidLsRef(pLs->GetLsRef());
						CLDSGroupLineAngle *pGroupJg=NULL;
						pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(pCurJg->group_father_jg_h,CLS_GROUPLINEANGLE);
						if(pGroupJg)
						{	//��ǰ�Ǹ�Ϊ��ϽǸ��ӽǸ�							
							for(int i=0;i<4;i++)
							{
								if(pGroupJg->son_jg_h[i]==pCurJg->handle)
									continue;
								CLDSLineAngle *pSonAngle=NULL;
								if(pGroupJg->son_jg_h[i]>0x20)
									pSonAngle=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
								if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
									continue;	//�����ڶ�Ӧ���ӽǸֻ��ӽǸ�Ϊ���⹹�� wht 11-07-25
								if(pSonAngle->IsLsInWing(pLs))//��˨���ӽǸ��Ͽ�������
								{
									pSonAngle->AppendMidLsRef(pLs->GetLsRef());
									pLs->AddL0Thick(pSonAngle->handle,TRUE);	//�ӽǸ�
									//�Ŷ����������ӽǸ�ʱ����Ҫ�ԽŶ���Ƶ�壬��֤������ wxc-2017.7.14
									BOLTSET boltset;
									boltset.append(pLs);
									int thick=ftoi(pGroupJg->jg_space-g_sysPara.m_nGroupJgMachiningGap);
									CLDSPlate* pBoltPad=DesignBoltPad(pCurJg,NULL,boltset,thick,g_sysPara.m_nGroupJgMachiningGap*0.5);
									if(pBoltPad)
									{
										if(sBoltPadPartNo.GetLength()<=0)
										{
											CInputAnStringValDlg inputStrDlg;
											inputStrDlg.m_sItemTitle="����Ŷ������:";
											if(inputStrDlg.DoModal()==IDOK)
												sBoltPadPartNo.Copy(inputStrDlg.m_sItemValue);
										}
										pBoltPad->SetPartNo(sBoltPadPartNo);
										pLs->AddL0Thick(ftoi(g_sysPara.m_nGroupJgMachiningGap));//��ϽǸּӹ���϶
									}
									else
										pLs->AddL0Thick(ftoi(pGroupJg->jg_space));	//�Ǹּ�϶
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
						//����ԭ��˨����Ч����
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
					//Ԥ�е�ǰ�˼�ʣ�೤���Ƿ��㹻������һ���Ŷ� wht 15-08-13
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
			long hFirstLineSlot=NULL; //��׼�Ŷ���
			naildlg.m_iLineType = 1;	//�˼�����Ϊ�ֹ�
			//����ָ���ֹ��ϵĽŶ�
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
			//���øֹܽŶ�
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
					//����нŶ��Ļ���������ʼ�Ŷ�
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("Please select start climbing nails and press return key, none nails type 'N' key:","");
#else 
					pCmdLine->FillCmdLine("��ѡ����ʼ�Ŷ��س�,����ʼ�Ŷ�[N]:","");
#endif
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
					g_pSolidDraw->ReleaseSnapStatus();
					if(pStartNail)
					{
						g_pSolidDraw->SetEntSnapStatus(pStartNail->handle);
						//���Ŷ�λ�÷Ŵ���ʾ
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
						//��ʱpStartNail��һ���ǽŶ����۸�ԭ���Ѳ������ĵ� wjh-2015.9.1
						start=pSlot->desDatumPoint.Position();
					}
					else if(pStartNail->GetClassTypeId()==CLS_LINEANGLE)
					{
						CLDSLineAngle* pAngle=(CLDSLineAngle*)pStartNail;
						hFirstLineSlot=pAngle->handle;
						start=pAngle->Start();
					}
					//TODO:Ӧ���ж���������ת��λ�Ƕ�
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
						vec*=-1.0;	//��ת����->ʼ
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
				ls.m_hFamily= CLsLibrary::GetFootNailFamily();//�Ŷ�
				used_dist = 0;
				naildlg.m_pRod=pCurTube;
				while(1)
				{
					naildlg.m_iLayWing=X0_OR_Y1_WING;
					naildlg.m_bHasWeldRoad=pCurTube->m_bHasWeldRoad;//��׼�ֹ��Ƿ���ں�����
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
					else	//�ϴ���ʣ��
						used_dist = odd_dist;
					if(fabs(naildlg.m_fCurNailAngle-naildlg.m_fNailAngle-naildlg.m_fNailAngle2)<1)
						X0_OR_Y1_WING=1;
					else
						X0_OR_Y1_WING=0;
					if(naildlg.m_iNailType==0)	//���ò۸����͵ĽŶ���
					{	//��һ������Ŷ����Ӱ��ʱ�򵯳����öԻ��� �Ժ��ٵ���
						restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
						pLineSlot = (CLDSLineSlot*)console.AppendPart(CLS_LINESLOT);
						pLineSlot->m_hPartWeldParent = pCurTube->handle; 
						CLDSLineSlot *pStandardFootNailPlate = (CLDSLineSlot*)console.FromPartHandle(hFirstLineSlot,CLS_LINESLOT);
						if(pStandardFootNailPlate==NULL)
						{
							pLineSlot->CopyProperty(&naildlg.xNailSlot);
							//TODO:pLineSlot->feature��ֵ�����벻���淶 wjh-2015.9.1
							pLineSlot->feature=naildlg.xNailSlot.feature;
							hFirstLineSlot=pLineSlot->handle;
						}
						else
						{
							pLineSlot->CopyProperty(pStandardFootNailPlate);
							pLineSlot->feature=pStandardFootNailPlate->feature;	//�Ŷ��ĳ���
						}
						//��������pLineSlot->CopyProperty(pStandardFootNailPlate)֮�󣬷������Իᱻ��� wjh-2015.9.1
						pLineSlot->CopyModuleInstanceInfo(pCurTube);
						f3dPoint workNorm=pCurTube->ucs.axis_x;
						double rotangle=naildlg.m_fCurNailAngle*RADTODEG_COEF;
						if(naildlg.m_iDatumLine==1)	//�Ժ�������ʼ��ת
							rotangle+=pCurTube->CalWeldRoadAngle();
						RotateVectorAroundVector(workNorm,rotangle,axis_z);
						pLineSlot->m_iNormCalStyle=0;	//ֱ��ָ���۸ֹ����淨�� 
						pLineSlot->SetWorkPlaneNorm(workNorm);
						pLineSlot->ucs.axis_y=-workNorm;
						pLineSlot->ucs.axis_z = -pCurTube->ucs.axis_z;
						normalize(pLineSlot->ucs.axis_z);
						//RotateVectorAroundVector(pLineSlot->ucs.axis_y,cur_angle,axis_z);//pCurTube->ucs.axis_z);
						pLineSlot->ucs.axis_x=pLineSlot->ucs.axis_y^pLineSlot->ucs.axis_z;
						normalize(pLineSlot->ucs.axis_x);
						if(naildlg.m_bDualSide)
						{	//˫�಼�Ŷ�
							pLineSlot2 = (CLDSLineSlot*)console.AppendPart(CLS_LINESLOT);
							pLineSlot2->m_hPartWeldParent = pCurTube->handle; 
							pLineSlot2->CopyProperty(pLineSlot);
							pLineSlot2->feature=pLineSlot->feature;
							//��������pLineSlot->CopyProperty(pStandardFootNailPlate)֮�󣬷������Իᱻ��� wjh-2015.9.1
							pLineSlot2->CopyModuleInstanceInfo(pCurTube);
							f3dPoint workNorm=pCurTube->ucs.axis_x;
							double rotangle=naildlg.m_fCurNailAngle2*RADTODEG_COEF;
							if(naildlg.m_iDatumLine==1)	//�Ժ�������ʼ��ת
								rotangle+=pCurTube->CalWeldRoadAngle();
							RotateVectorAroundVector(workNorm,rotangle,axis_z);
							pLineSlot2->m_iNormCalStyle=0;	//ֱ��ָ���۸ֹ����淨�� 
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
					else 	//���ýǸ�
					{
						double wing_wide=0,wing_thick=0;
						pLineAngle=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
						pLineAngle->m_bFootNail=TRUE;//��ʾ�ýǸ�Ϊ�Ŷ�
						pLineAngle->CopyModuleInstanceInfo(pCurTube);
						pLineAngle->ucs.axis_z=pCurTube->ucs.axis_x;	
						double rot_angle=pCurTube->CalWeldRoadAngle();
						if(naildlg.m_iDatumLine==1) //�Ի�׼��Ϊ��׼�����Ժ�����Ϊ��׼ 
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
							//TODO:pLineSlot->feature��ֵ�����벻���淶 wjh-2015.9.1
							angle_dlg.m_fParamL=pLineAngle->feature=naildlg.xNailAngle.feature;//angle_dlg.xNailAngle.feature;
							hFirstLineSlot=pLineAngle->handle;
						}
						else
						{
							pLineAngle->CopyProperty(pStandardFootNailAngle);
							angle_dlg.m_fParamL=pLineAngle->feature=pStandardFootNailAngle->feature;	//�Ŷ��ĳ���
						}
						/*angle_dlg.m_pLineTube=pCurTube;
						angle_dlg.part_type=1;	//��׹�Ǹ�
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
					//if(bDownToUp)	//�Ŷ��������ϲ���
					//{
					//	if(pCurTube->pStart->Position(false).z>pCurTube->pEnd->Position(false).z)
					//		pCurTube->ucs.origin = pCurTube->Start()-line_vec*pCurTube->startOdd();
					//	else	//��ʼ���,��ֹ���
					//		pCurTube->ucs.origin = pCurTube->End()+line_vec*pCurTube->endOdd();
					//}
					//else //if(!bDownToUp) //�Ŷ��������²���
					//{
					//	if(pCurTube->pStart->Position(false).z>pCurTube->pEnd->Position(false).z)	
					//		pCurTube->ucs.origin = pCurTube->End()+line_vec*pCurTube->endOdd();
					//	else	//��ʼ���,��ֹ���
					//		pCurTube->ucs.origin = pCurTube->Start()-line_vec*pCurTube->startOdd();
					//}
					double cur_angle=naildlg.m_fCurNailAngle*RADTODEG_COEF;
					used_dist+=overlap_length;
					overlap_length=0.0;	//�����ص����ֳߴ�һ�κ�����ص��ߴ�
					if(used_dist>dist)
					{
						odd_dist = used_dist-dist;
						long handle=0;
						if(naildlg.m_iNailType==0)	//�۸�
							handle=pLineSlot->handle;
						else //�Ǹ�
							handle=pLineAngle->handle;
						console.DispPartSet.DeleteNode(handle);
						console.DeletePart(handle);
						break;
					}
					if(naildlg.m_iNailType==0)	//�۸�
					{
						//����Ŷ�������ϵԭ��λ��
						double H = pLineSlot->GetHeight();	//�۸ֵױ߿��
						double W = pLineSlot->GetWidth();	//�۸�֫��
						double T = pLineSlot->GetThick();
						double dist = sqrt(SQR(0.5*pCurTube->GetDiameter())-SQR(0.5*H-T));
						pLineSlot->ucs.origin = start-pLineSlot->ucs.axis_y*(dist+W)-pLineSlot->ucs.axis_z*(pLineSlot->feature*0.5);
						if(pLineSlot2)
							pLineSlot2->ucs.origin = start-pLineSlot2->ucs.axis_y*(dist+W)-pLineSlot2->ucs.axis_z*(pLineSlot2->feature*0.5);
					}
					else	//�Ǹ�
					{
						RotateVectorAroundVector(pLineAngle->ucs.axis_z,cur_angle,axis_z);
						pLineAngle->ucs.axis_y=pLineAngle->ucs.axis_z^pLineAngle->ucs.axis_x;
						normalize(pLineAngle->ucs.axis_y);
						pLineAngle->ucs.axis_z=pLineAngle->ucs.axis_x^pLineAngle->ucs.axis_y;
						normalize(pLineAngle->ucs.axis_z);
						pLineAngle->ucs.origin = start;
					}
					if(naildlg.m_bShiftWing)		//�Ŷ����沼��
						X0_OR_Y1_WING = !X0_OR_Y1_WING;
					if(fabs(odd_dist)<EPS)	//�ϴ����û��ʣ��ֵ
					{
						if(naildlg.m_iNailType==0)	//�۸�
						{
							Add_Pnt(pLineSlot->ucs.origin, pLineSlot->ucs.origin, vec*ftol(used_dist));
							if(pLineSlot2)
								Add_Pnt(pLineSlot2->ucs.origin, pLineSlot2->ucs.origin, vec*ftol(used_dist));
						}
						else	//�Ǹ�
							Add_Pnt(pLineAngle->ucs.origin, pLineAngle->ucs.origin, vec*ftol(used_dist));;
					}
					else	//�ϴ����ʱ��ʣ��ֵ
					{
						if(naildlg.m_iNailType==0)	//�۸�
						{
							Add_Pnt(pLineSlot->ucs.origin, pLineSlot->ucs.origin,vec*ftol(odd_dist));
							if(pLineSlot2)
								Add_Pnt(pLineSlot2->ucs.origin, pLineSlot2->ucs.origin,vec*ftol(odd_dist));
						}
						else	//�Ǹ�
							Add_Pnt(pLineAngle->ucs.origin, pLineAngle->ucs.origin,vec*ftol(odd_dist));;
						odd_dist = 0.0;
					}
					if(naildlg.m_iNailType==0)	//�۸�
					{	//�Ŷ��������Ϣ
						f3dPoint datumStartPos,datumLenVec,pt=start+vec*ftol(used_dist);//-pLineSlot->ucs.axis_z*pLineSlot->feature*0.5;
						pt+=pLineSlot->WorkPlaneNorm()*pCurTube->GetDiameter()*0.5;
						pLineSlot->desDatumPoint.SetPosition(pt);
						//�趨�Ŷ��۸ֵ����Ĳ�������λ��Ϣ
						pLineSlot->hAttachTube=pCurTube->handle;
						pLineSlot->desDatumPoint.datum_pos_style=11;//���涨λ��
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.hLineTube=pCurTube->handle;
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag|=0x02;
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.bSpecRo=FALSE;
						if((bDownToUp &&pCurTube->pStart->Position('Z')>pCurTube->pEnd->Position('Z')) ||	//�Ŷ��������ϲ���
							(!bDownToUp&&pCurTube->pStart->Position('Z')<pCurTube->pEnd->Position('Z')))	//�Ŷ��������²���
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
						if(naildlg.m_bHorizonSlotNail)	//ˮƽ���ýŶ��۸ֵ���
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
						else if(naildlg.m_iDatumLine==0)		//�ֹܻ�׼��Ϊ��ת��ʼ��
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=2;
						else if(naildlg.m_iDatumLine==1)//�ֹܺ�����Ϊ��ת��ʼ��
							pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=3;
						pLineSlot->desDatumPoint.des_para.COLUMN_INTERS.ray_angle=naildlg.m_fCurNailAngle;
						pLineSlot->fLen=pLineSlot->feature;
						pLineSlot->CalTubeSlotPos();
						//��˨
						//restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
						Add_Pnt(ls_pos,pLineSlot->Start()+pLineSlot->ucs.axis_z*pLineSlot->feature*0.5,pLineSlot->ucs.axis_y*pLineSlot->GetThick());
						ls.set_norm(-pLineSlot->ucs.axis_y);
						//��˨λ�������Ϣ���
						pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls,false);
						pLs->iSeg=pLineSlot->iSeg;
						pLs->layer(0)=pCurTube->Layer(0);
						pLs->cfgword=pLineSlot->cfgword;	//������˨��ĺ����׼������ĺ�һ��
						pLs->AddL0Thick(pLineSlot->handle,TRUE);
						pLs->des_base_pos.hPart = pLineSlot->handle;
						pLs->des_base_pos.cLocationStyle=TRUE; 
						pLs->des_base_pos.wing_offset_dist=0;
						pLs->des_base_pos.len_offset_dist=pLineSlot->feature*0.5;
						pLs->des_base_pos.norm_offset.AddThick(-pLineSlot->handle,TRUE);
						pLs->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
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
							//�趨�Ŷ��۸ֵ����Ĳ�������λ��Ϣ
							pLineSlot2->hAttachTube=pCurTube->handle;
							pLineSlot2->desDatumPoint.datum_pos_style=11;//���涨λ��
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.hLineTube=pCurTube->handle;
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag|=0x02;
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.bSpecRo=FALSE;
							if((bDownToUp &&pCurTube->pStart->Position('Z')>pCurTube->pEnd->Position('Z')) ||	//�Ŷ��������ϲ���
								(!bDownToUp&&pCurTube->pStart->Position('Z')<pCurTube->pEnd->Position('Z')))	//�Ŷ��������²���
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
							if(naildlg.m_bHorizonSlotNail)	//ˮƽ���ýŶ��۸ֵ���
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
							else if(naildlg.m_iDatumLine==0)		//�ֹܻ�׼��Ϊ��ת��ʼ��
								pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=2;
							else if(naildlg.m_iDatumLine==1)//�ֹܺ�����Ϊ��ת��ʼ��
								pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.ray_vec_style=3;
							pLineSlot2->desDatumPoint.des_para.COLUMN_INTERS.ray_angle=naildlg.m_fCurNailAngle2;
							pLineSlot2->fLen=pLineSlot2->feature;
							pLineSlot2->CalTubeSlotPos();
							//��˨
							//restore_Ls_guige(naildlg.m_sNailGuiGe,ls);
							Add_Pnt(ls_pos,pLineSlot2->Start()+pLineSlot2->ucs.axis_z*pLineSlot2->feature*0.5,pLineSlot2->ucs.axis_y*pLineSlot2->GetThick());
							ls.set_norm(-pLineSlot2->ucs.axis_y);
							//��˨λ�������Ϣ���
							pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
							pLs->CopyProperty(&ls,false);
							pLs->iSeg=pLineSlot->iSeg;
							pLs->layer(0)=pCurTube->Layer(0);
							pLs->cfgword=pLineSlot->cfgword;	//������˨��ĺ����׼������ĺ�һ��
							pLs->AddL0Thick(pLineSlot2->handle,TRUE);
							pLs->des_base_pos.hPart = pLineSlot2->handle;
							pLs->des_base_pos.cLocationStyle=TRUE; 
							pLs->des_base_pos.wing_offset_dist=0;
							pLs->des_base_pos.len_offset_dist=pLineSlot2->feature*0.5;
							pLs->des_base_pos.norm_offset.AddThick(-pLineSlot2->handle,TRUE);
							pLs->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
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
					else //�Ǹ� 
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
						//�������ն�λ�ú������÷��߷���
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
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
	return 0;	
}
//���ý�̤��
int  CLDSView::LayoutPedalPlate()
{
	CCmdLockObject cmdlock(this);
	if (!cmdlock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	g_pSolidDraw->ReleaseSnapStatus();
	//ȷ����̤�岼�÷�ʽ
	CString cmdStr,valueStr;
	static char cToward = 'U';
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Layout pedal plate[from up to down(D)/from down to up(U)]<%C>:", cToward);
#else 
	cmdStr.Format("���ý�̤��[���϶���(D)/���¶���(U)]<%C>:", cToward);
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
			pCmdLine->FinishCmdLine("�ؼ�����������,������¼��!");
			pCmdLine->FillCmdLine(cmdStr, "");
			continue;
		}
		break;
	}
	BOOL bDownToUp = (cToward == 'U') ? TRUE : FALSE;
	//ѡ������ý�̤������ĽǸ�
	DWORD dwhObj = 0, dwExportFlag = 0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_LINEANGLE));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE, SNAP_LINE);
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select leg member which will layout pedal plate<Click empty to over>:", "");
#else 
	pCmdLine->FillCmdLine("������ѡ��Ҫ���ý�̤�������<�յ�����������ѡ��>:", "");
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
				AfxMessageBox("û��ѡ����Ч�ĸ˼�!������ѡ��!");
#endif
				continue;
			}
			if (pLastJg != NULL && pLastJg == pSelJg)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Repeat to select the same leg member,Please select again!");
#else 
				AfxMessageBox("�ظ�ѡ����ͬһ������,������ѡ��!");
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
				AfxMessageBox("The selected rod has no connection with previous rod��Please select again!");
#else 
				AfxMessageBox("��ѡ�˼�����һ�˼�û�����ӣ�������ѡ��!");
#endif
				continue;
			}
			else if (bDownToUp)	//�������ϲ���
			{
				if (pSelJg->pStart->Position(false).z > pSelJg->pEnd->Position(false).z)
				{
					hStart = pSelJg->pStart->handle;
					hCur = pSelJg->pEnd->handle;
				}
				else	//��ʼ���,��ֹ���
				{
					hStart = pSelJg->pEnd->handle;
					hCur = pSelJg->pStart->handle;
				}
			}
			else //if(!bDownToUp) //�������²���
			{
				if (pSelJg->pStart->Position(false).z > pSelJg->pEnd->Position(false).z)
				{
					hStart = pSelJg->pEnd->handle;
					hCur = pSelJg->pStart->handle;
				}
				else	//��ʼ���,��ֹ���
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
		AfxMessageBox("û��ѡ�д����õĽǸ֣����ʧ��!");
		pCmdLine->CancelCmdLine();
		return 0;
	}
	//ѡ����ʼ��̤��
	CLDSLineAngle* pFirstJg = (CLDSLineAngle*)partset.GetFirst();
	CLDSParamPlate* pStartPedalPlate = NULL;
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PARAMPLATE));
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select start climbing nails and press return key, none nails type 'N' key:", "");
#else 
	pCmdLine->FillCmdLine("��ѡ����ʼ��̤��,û�а��س���:", "");
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
	//���ý�̤��
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
		double fJointGap= (fabs(fPreOdd) > EPS) ? (fPreOdd + fCurLinkOdd) : 0;	//��ͷ��϶
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
		{	//Ԥ�е�ǰ�˼�ʣ�೤���Ƿ��㹻������һ����̤��
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
			//Ԥ�е�ǰ�˼�ʣ�೤���Ƿ��㹻������һ����̤��
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
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:", "");
#else 
	pCmdLine->FillCmdLine("����:", "");
#endif
	return 0;
}
//��Ƶ׽Ű�
void CLDSView::OnFootPlank() {
	DesignFoot(); 
}
int CLDSView::DesignOldFoot()		//��ƾ�ʽ����
{
#ifdef __SMART_DRAW_INC_
	DesignFoot();
#else
	m_nPrevCommandID=ID_FOOT_PLANK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat base plates";
#else 
	m_sPrevCommandName="�ظ��׽Ű�";
#endif
	g_pSolidSet->SetOperType(OPER_OSNAP);
	m_curTask = TASK_DESIGN_FOOT_PLANK;
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
	//�л���������ʾ״̬
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please select base plates' nodes and leg member!");
#else 
	g_pPromptMsg->SetMsg("��ѡ��Ҫ��Ƶ׽Ű�Ľڵ㼰���ĽǸ�!");
#endif
#endif
	return 0;
}
//���ţ���
int CLDSView::DesignHoof()			//���ţ���
{
	OnDesHoofPlank();
	return TRUE;
}
void CLDSView::OnDesHoofPlank() 
{	//��ϽǸֽ�ͷ����ţ�������
	m_nPrevCommandID=ID_DES_HOOF_PLANK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat design hoof plates";
#else 
	m_sPrevCommandName="�ظ�ţ���";
#endif
	g_pSolidSet->SetOperType(OPER_OSNAP);
	m_curTask = TASK_DESIGN_HOOF_PLANK;
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
	//�л���������ʾ״̬
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please select base plates' nodes and leg member!");
#else 
	g_pPromptMsg->SetMsg("��ѡ��Ҫ��Ƶ׽Ű�Ľڵ㼰���ĽǸ�!");
#endif
}
//�и�Ǹ�
void CLDSView::OnCutJg() 
{
	m_nPrevCommandID=ID_CUT_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to specify datum angle's leg cut";
#else 
	m_sPrevCommandName="�ظ�ָ����׼�Ǹ��н���֫";
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
			g_pPromptMsg->SetMsg("Select one datum angle to cut ray angle��");
#else 
			g_pPromptMsg->SetMsg("ѡ��һ����׼�Ǹ������и����߽Ǹ֣�");
#endif
		}
		return;
	}
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please select a cutted ray angle!");
#else 
	g_pPromptMsg->SetMsg("��ѡ���и�����߽Ǹ�!");
#endif
}
//��ƽڵ��
void CLDSView::OnDesignJdb() 
{
	m_nPrevCommandID=ID_DESIGN_JDB;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to design node plate";
#else 
	m_sPrevCommandName="�ظ��ڵ��";
#endif
	Command("DesignNodePlate");
}

CMirMsgDlg mirmsgDlg;	//���Ӱ�Գ���Ϣ
int CLDSView::DesignNodePlate()	//ͨ�������е���FinishDesignJdb
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
	BOOL bHasMirJdb = FALSE;		//�Ƿ�Գ����ɶԽǲ�λ�����Ӱ�
 	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pSelNode==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select nodes which to be designed!","");
#else 
		pCmdLine->FillCmdLine("DesignNodePlate ѡ��ڵ��������ڵ�:","");
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
	pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pSelNode->handle));

	BOOL bTerminateByUser=FALSE;
	CLogErrorLife loglife;
	CUndoOperObject undo(&Ta,true);
	try{
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pSelNode->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Without modify permission of the node��";
#else 
			throw "û�д˽ڵ���޸�Ȩ�ޣ�";
#endif
		g_pSolidDraw->SetEntSnapStatus(pSelNode->handle,TRUE);	//������ʾ�ڵ���׼��
		//ѡ��װ�䶨λ�˼�
		f3dLine line_3d;
		CString cmdStr;
		SmartPartPtr pBasePart;
		CLDSLinePart *pBaseLinePart=NULL,*pNodeFatherPart=NULL;
		//g_pSolidDraw->ReleaseSnapStatus();
		//static int iPlateFaceType;	//�����
		int iPlateFaceType=1;	//�����(��ΰ˵�ͻ���ӳ�ڵ�����ͻ���ȥ�����书�ܸ�ʵ�� wjh-2015.6.24)
		iPlateFaceType=max(1,iPlateFaceType);
		iPlateFaceType=min(4,iPlateFaceType);
		BOOL bHasSpecifyType=FALSE;	//�Ƿ����ƶ����ڵ������
		pNodeFatherPart=(CLDSLinePart*)console.FromPartHandle(pSelNode->hFatherPart,CLS_LINEPART);
		if(pSelNode->m_cPosCalType==4&&pNodeFatherPart&&pNodeFatherPart->GetClassTypeId()==CLS_LINEANGLE)
		{	//ѡ��ڵ������ 1.����� 2.˫��� 3.����� 4.�ڲ������
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select node-plate's type<1.planar plate|2.2-plane plate|3.3-plane plate|4.inner cross plate>[4]:","");
#else 
			pCmdLine->FillCmdLine("DesignNodePlate ��ָ���ڵ������<1.�����|2.˫���|3.�����|4.�ڲ������>[4]:","");
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
					pCmdLine->FillCmdLine("�Ƿ����룬������ָ���ڵ������<1.�����|2.˫���|3.�����|4.�ڲ������>[4]:","");
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
			pCmdLine->FillCmdLine("DesignNodePlate ��ѡ��ڵ���װ�䶨λ�˼�:","");
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
		//δָ���ڵ������
		if(pBaseLinePart==NULL)	//�Ի��θ˼�Ϊ��׼��ֻ���������
			iPlateFaceType=1;
		else if(!bHasSpecifyType)
		{	//���ݻ�׼�˼�����Ԥ�нڵ������
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
						iPlateFaceType=2;	//��׼�ֹ�Ϊ�Խ���ᣬ�˴�Ӧ������˫���
				}
			}
			else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pBaseLinePart)->group_father_jg_h>0x20)
			{	//�漰��ϽǸ�ʱ����������ϽǸ�Ϊװ�䶨λ��׼�˼� wjh-2016.11.04
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)Ta.Parts.FromHandle(((CLDSLineAngle*)pBaseLinePart)->group_father_jg_h,CLS_GROUPLINEANGLE);
				if(pGroupAngle)
					pBasePart=pBaseLinePart=pGroupAngle;
			}
			//ѡ��ڵ������ 1.����� 2.˫��� 3.����� 4.�ڲ������
			CString sPrompt="";
#ifdef AFX_TARG_ENU_ENGLISH
			sPrompt.Format("Please select node-plate's type<1.planar plate|2.2-plane plate|3.3-plane plate|4.inner cross plate>[%d]:",iPlateFaceType);
#else 
			sPrompt.Format("DesignNodePlate ��ָ���ڵ������<1.�����|2.˫���|3.�����|4.�ڲ������>[%d]:",iPlateFaceType);
#endif
			pCmdLine->FillCmdLine(sPrompt,"");
			while(1)
			{
				//����ǰ��ʹ��SnapObjectʱ���ܻ���������̧�����Ϣ��������ֻ����LBUTTONDOWN_AS_RETURN��wjh-2016.10.25
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
					pCmdLine->FillCmdLine("�Ƿ����룬������ָ���ڵ������<1.�����|2.˫���|3.�����|4.�ڲ������>[4]:","");
#endif
					continue;
				}
				else
					break;
			}
		}
		//
		g_pSolidSet->SetDisplayType(DISP_SOLID);//�л���ʵ����ʾģʽ
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->Draw();
		pCurPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pCurPlate->cfgword=pSelNode->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pCurPlate->designInfo.m_hBaseNode = pSelNode->handle;
		if(pBasePart.IsHasPtr())
			pCurPlate->designInfo.m_hBasePart = pBasePart->handle;
		designJdb.SetViewFlag(m_eViewFlag);
		CStackVariant<char> stackprop0(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_BY_SUMM_THICK);
		//��һ�д����Ѹ���ϵͳ�������������� wjh-2019.8.25
		//CStackVariant<char> stackprop0i(&CLDSBolt::BOLTL0_PREFER_MODE,CLDSBolt::L0CAL_INC_GAP_THICK);
		if(iPlateFaceType<4)
		{	//��ͨ���Ӱ�
			pCurPlate->face_N = iPlateFaceType;
			pCurPlate->jdb_style = 0;
			if(!designJdb.DesignCommonPlank(pCurPlate))
			{
				bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Design failed";
#else 
				throw "���ʧ��";
#endif
			}
		}
		else //if(iPlateFaceType==4)//��Ƶ��ǽ����(��Ҫ���⴦��)
		{
			pCurPlate->jdb_style = 1;
			pCurPlate->face_N = 1;
			if(!designJdb.DesignCrossPlank(pCurPlate))
			{
				bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Design failed";
#else 
				throw "���ʧ��";
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
		//�����쳣���ʧ��,ɾ��ϵͳ�������ɵĴ���ڵ��
		if(pCurPlate)
		{
			console.DispPartSet.DeleteNode(pCurPlate->handle);
			console.DeletePart(pCurPlate->handle);
		}
		if(!bTerminateByUser)
			AfxMessageBox(s);	//��ʾ�쳣������Ϣ
		g_pSolidDraw->ReleaseSnapStatus();
		return FALSE;
	}
	
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
#endif
	FinishDesignJdb(NULL);	//����ִ�нڵ���������(��ΰ˵���ͨ���)��wjh-2016.12.7
	return TRUE;
}

//�۵������
void CLDSView::OnFoldPlank() 
{
	m_nPrevCommandID=ID_FOLD_PLANK;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to design folded plate";
#else 
	m_sPrevCommandName="�ظ��۵���";
#endif
	CLDSPlate *pCurPlate=NULL;
	CDesignJdb designJdb;
	CLDSNode *pBaseNode[2]={NULL};
	CLDSLineAngle *pBaseJg[2]={NULL},*pMirJg=NULL;
	int i;
	//CLDSNode::ATTACH_PART *pAttach=NULL;
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	if(!LockFunc())
		return;
	UINT nRetCode=1;	//���Ѳ���Ҫ��������״̬�� wjh-2017.9.18
	BOOL bTerminateByUser=FALSE;
	Ta.BeginUndoListen();
	try
	{
		if(m_eViewFlag == PERSPECTIVE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Can't design folded plate in perspective view,please convert to another view";
#else 
			throw "������͸��ͼ�½����۵������,��ת��������ͼ�����۵������";
#endif
		if(m_eViewFlag==RANDOM_VIEW)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Can't design folded plate in spread view,please convert to another view";
#else 
			throw "������չ��ͼ�½����۵������,��ת��������ͼ�����۵������";
#endif
#ifdef DOG_CHECK
		if(nRetCode!=1)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Hardware lock's detection is wrong,program is error!";
#else 
			throw "�����ܹ�����,�������!";
#endif
#endif
		///////////////��׽����/////////////////////////
		f3dLine line;
		f3dPoint *point;
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select first angle in turn!");
#else 
		g_pPromptMsg->SetMsg("������ѡ���һ���Ǹ�!");
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
			throw "��;�˳�";
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select first angle's design nodes int turn!");
#else 
		g_pPromptMsg->SetMsg("������ѡ���һ���Ǹֵ���ƽڵ�!");
#endif
		while(g_pSolidSnap->SnapPoint(point)>0)
		{
			pBaseNode[0]=console.FromNodeHandle(point->ID);
			if(theApp.m_bCooperativeWork&&pBaseNode[0]&&!theApp.IsHasModifyPerm(pBaseNode[0]->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of the node��";
#else 
				throw "û�д˽ڵ���޸�Ȩ�ޣ�";
#endif
			if(pBaseNode[0]&&pBaseJg[0]->pStart==pBaseNode[0])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[0]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's start��";
#else 
					throw "û�д˽Ǹ�ʼ�˵��޸�Ȩ�ޣ�";
#endif
				break;
			}
			else if(pBaseNode[0]&&pBaseJg[0]->pEnd==pBaseNode[0])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[0]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's end��";
#else 
					throw "û�д˽Ǹ��ն˵��޸�Ȩ�ޣ�";
#endif
				break;
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The selected angle don't fit!");
#else 
				AfxMessageBox("ѡ��Ǹֲ�����!");
#endif
		}
		if(pBaseNode[0])
			g_pSolidDraw->SetEntSnapStatus(pBaseNode[0]->handle);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Quit midway";
#else 
			throw "��;�˳�";
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select second angle's another design node in turn!");
#else 
		g_pPromptMsg->SetMsg("������ѡ��ڶ����Ǹ���һ��ƽڵ�!");
#endif
		while(g_pSolidSnap->SnapPoint(point)>0)
		{
			pBaseNode[1]=console.FromNodeHandle(point->ID);
			if(pBaseNode[1])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseNode[1]->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of the node��";
#else 
					throw "û�д˽ڵ���޸�Ȩ�ޣ�";
#endif
				g_pSolidDraw->SetEntSnapStatus(pBaseNode[1]->handle);
				break;
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The selected node don't fit!");
			throw "Quit midway";
#else 
				AfxMessageBox("ѡ��ڵ㲻����!");
			throw "��;�˳�";
#endif
		}
		
#ifdef AFX_TARG_ENU_ENGLISH
		g_pPromptMsg->SetMsg("Please select second angle in turn!");
#else 
		g_pPromptMsg->SetMsg("������ѡ��ڶ����Ǹ�!");
#endif
		while(g_pSolidSnap->SnapLine(line)>0)
		{
			pBaseJg[1]=(CLDSLineAngle*)console.FromPartHandle(line.ID,CLS_LINEANGLE);
			if(pBaseJg[1]&&pBaseJg[1]->pStart==pBaseNode[1])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[1]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's start��";
#else 
					throw "û�д˽Ǹ�ʼ�˵��޸�Ȩ�ޣ�";
#endif
				g_pSolidDraw->SetEntSnapStatus(pBaseJg[1]->handle);
				break;
			}
			else if(pBaseJg[1]&&pBaseJg[1]->pEnd==pBaseNode[1])
			{
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pBaseJg[1]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Without modify permission of angle's end��";
#else 
					throw "û�д˽Ǹ��ն˵��޸�Ȩ�ޣ�";
#endif
				g_pSolidDraw->SetEntSnapStatus(pBaseJg[1]->handle);
				break;
			}
			else
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected angle don't fit,Quit midway!";
#else 
				throw "ѡ��Ǹֲ�����,��;�˳�!";
#endif
		}
		/*g_pPromptMsg->SetMsg("������ѡ��������Ǹ�!");
		while(g_pSolidSnap->SnapLine(line))
		{
		pBaseJg[2]=Ta.LineAngles.FromHandle(line->ID);
		if(pBaseJg[2]&&(
		pBaseJg[2]->pStart==pBaseNode[1]||pBaseJg[2]->pEnd==pBaseNode[1]))
		break;
		else
		AfxMessageBox("ѡ��Ǹֲ�����!");
		}
		if(pBaseJg[2])
		g_pSolidDraw->SetEntSnapStatus(pBaseJg[2]->handle);
		else
		throw "��;�˳�";*/
		for(i=0;i<2;i++)
		{
			if(pBaseJg[i]==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The selected datum angle is error,quit midway��";
#else 
				throw "��׼�Ǹ�ѡ��������ʧ����;�˳���";
#endif
			if(m_eViewFlag==FRONT_VIEW||m_eViewFlag==BACK_VIEW)
				pMirJg=(CLDSLineAngle*)pBaseJg[i]->GetMirPart(MIRMSG(1),pBaseNode[i]->Position());	//X��Գ�
			else
				pMirJg=(CLDSLineAngle*)pBaseJg[i]->GetMirPart(MIRMSG(2),pBaseNode[i]->Position());	//Y��Գ�
			if(pMirJg==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Lack of symmetry angle,quit midway��";
#else 
				throw "ȱ�ٶԳƽǸ֣����ʧ����;�˳���";
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
		pCurPlate->cfgword=pBaseNode[0]->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pCurPlate->designInfo.m_hBaseNode = pBaseNode[0]->handle;
		pCurPlate->jdb_style=1;	////֮ǰ�۵��帳ֵΪ3�Ǵ�Ļ���ֹܼа����������������Ϊ1��������IsFoldPlate()���� wjh-2016.1.09
		pCurPlate->face_N = 3;
		designJdb.SetViewFlag(m_eViewFlag);
		pCurPlate->ucs.axis_x=pBaseNode[1]->Position(true)-pBaseNode[0]->Position(true);//���������ϵX��
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
		else//���ʧ��,ɾ��ϵͳ�������ɵĴ���ڵ��
		{
			bTerminateByUser=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Design failed";
#else 
			throw "���ʧ��";
#endif
		}
		
		g_pSolidDraw->ReleaseSnapStatus();
		Ta.EndUndoListen();
	}
	catch(char *s)
	{
		//�����쳣���ʧ��,ɾ��ϵͳ�������ɵĴ���ڵ��
		if(pCurPlate)
			console.DeletePart(pCurPlate->handle);
		if(!bTerminateByUser)
			AfxMessageBox(s);	//��ʾ�쳣������Ϣ
		g_pSolidDraw->ReleaseSnapStatus();
		ReleaseFunc();	//�⿪������������״̬
		Ta.EndUndoListen();
		return;
	}
	
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	ReleaseFunc();	//�⿪������������״̬
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
			throw "Without modify permission of the node��";
#else 
			throw "û�д˽ڵ���޸�Ȩ�ޣ�";
#endif
		if(pNodeAtom->GetClassTypeId()!=CLS_NODE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Part type is error,the selected first part should be node!";
#else 
			throw "�������ʹ���,����ѡ�ĵ�һ������Ӧ���ǽڵ�!";
#endif
		pSelNode = (CLDSNode*)pNodeAtom;
		//CLDSNode::ATTACH_PART *pAttach = FindJointPosIndex(pSelNode);
		
		if(pJgAtom1->GetClassTypeId()!=CLS_LINEANGLE&&pJgAtom1->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Part type is error,the selected second part should be angle!";
#else 
			throw "�������ʹ���,����ѡ�ĵڶ�������Ӧ���ǽǸ�!";
#endif
		pMainJg[1] = (CLDSLineAngle*)pJgAtom1;
		if(pJgAtom2->GetClassTypeId()!=CLS_LINEANGLE&&pJgAtom2->GetClassTypeId()!=CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Part type is error,the selected third part should be angle!";
#else 
			throw "�������ʹ���,����ѡ�ĵ���������Ӧ���ǽǸ�!";
#endif
		pMainJg[0] = (CLDSLineAngle*)pJgAtom2;
		
		if(pMainJg[0]->pStart->handle==pSelNode->handle)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[0]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's start��";
#else 
				throw "û�д˽Ǹ�ʼ�˵��޸�Ȩ�ޣ�";
#endif
			pMainJg[0]->feature = 0;//����뵱ǰ�ڵ�������
		}
		else
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[0]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's end��";
#else 
				throw "û�д˽Ǹ��ն˵��޸�Ȩ�ޣ�";
#endif
			pMainJg[0]->feature = 1;//�յ��뵱ǰ�ڵ�������
		}
		if(pMainJg[1]->pStart->handle==pSelNode->handle)
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[1]->dwStartPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's start��";
#else 
				throw "û�д˽Ǹ�ʼ�˵��޸�Ȩ�ޣ�";
#endif
			pMainJg[1]->feature = 0;//����뵱ǰ�ڵ�������
		}
		else
		{
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pMainJg[1]->dwEndPermission))
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of angle's end��";
#else 
				throw "û�д˽Ǹ��ն˵��޸�Ȩ�ޣ�";
#endif
			pMainJg[1]->feature = 1;//�յ��뵱ǰ�ڵ�������
		}
		if(pMainJg[0]->GetClassTypeId()!=pMainJg[1]->GetClassTypeId())
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Two datum part's material are different,it isn't fit design joint here!";
#else 
			throw "����׼�����Ͳ�ͬ�����ʺ��ڴ˴���ƽ�ͷ!";
#endif
		else if(pMainJg[0]->GetClassTypeId()==CLS_GROUPLINEANGLE
			&&((CLDSGroupLineAngle*)pMainJg[0])->group_style!=((CLDSGroupLineAngle*)pMainJg[1])->group_style)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Two datum part's material are different,it isn't fit design joint here!";
#else 
			throw "����׼�����Ͳ�ͬ�����ʺ��ڴ˴���ƽ�ͷ!";
#endif
		CLDSLineAngle *pTemJg;
		if(  pMainJg[1]->GetThick()>pMainJg[0]->GetThick() ||
			(pMainJg[1]->GetThick()==pMainJg[0]->GetThick()&&
			pMainJg[1]->GetWidth()>pMainJg[0]->GetWidth()))
		{	//��һ����׼�Ǹ�Ӧ��Ϊ֫��Ϻ��֫��Ͽ���Ǹ�
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
		MessageBox("��ͷ��Ƴɹ�!");
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
				throw "Not have modify permission of the combined angle��";
#else 
				throw "û�д���ϽǸֵĻ����޸�Ȩ�ޣ�";
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
				throw "Not have modify permission of the combined angle��";
#else 
			if(pSecObj==NULL||pThirObj==NULL)
				throw "ѡ�񹹼�����!��岼��ʧ��!";
			else if(pSecObj->GetClassTypeId()!=CLS_GROUPLINEANGLE||pThirObj->GetClassTypeId()!=CLS_NODE)
				throw "ѡ�񹹼����Ͳ���!��岼��ʧ��!";
			else if(pSecObj->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				throw "ѡ��Ǹֲ�����ϽǸֲ��貼�����!";
			pGroupJg=(CLDSGroupLineAngle*)pSecObj;
			if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pGroupJg->dwPermission))
				throw "û�д���ϽǸֵĻ����޸�Ȩ�ޣ�";
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
			throw "��;�˳�!��岼��ʧ��!";
		}
		if(fill_plank_dlg.m_nPlankNum<=0)
			throw "ָ�������Ϊ��,�������!";
#endif
		int nSecType = 0;
		if(fill_plank_dlg.m_bDifferentType)
			nSecType = !fill_plank_dlg.m_iLsLayOutStyle;
		else
			nSecType = fill_plank_dlg.m_iLsLayOutStyle;
		if(fill_plank_dlg.m_nThick<=0)
			fill_plank_dlg.m_nThick=ftoi(pGroupJg->jg_space);
		//��ϽǸֽǸּ�϶��һ�룬������˨ʱʹ�ã���֤��˨�ڽǸ�׼���� wht 11-07-01
		double fHalfGroupJgSpace=0.5*pGroupJg->jg_space;	
		CLDSLineAngle *pBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->m_hDatumSonAngle,CLS_LINEANGLE);
		if(pBaseJg==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The combined angle lacks of datum angle!Layout filler plate failure!";
#else 
			throw "��ϽǸ�ȱ�ٻ�׼�Ǹ�!��岼��ʧ��!";
#endif
		
		CLDSBolt ls(console.GetActiveModel());	//��˨
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
			pPlank->cfgword=pBaseJg->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
			pPlank->Thick=fill_plank_dlg.m_nThick;
			pPlank->SetPartNo(fill_plank_dlg.m_sPartNo.GetBuffer());
			pPlank->iSeg=SEGI(fill_plank_dlg.m_sSegI.GetBuffer());
			pPlank->cMaterial=CSteelMatLibrary::RecordAt(fill_plank_dlg.m_iPlateMaterial).cBriefMark;
			pPlank->designInfo.m_bEnableFlexibleDesign=TRUE; //�����������
			pPlank->ucs.axis_y.Set();	//����������������ʱ������Y������ϵ,���յ��������������ϽǸֲ�һ�� wjh-2016.4.13
			pPlank->designInfo.iProfileStyle0123=2;			 //��������
			//���øְ����Ի�����
			pPlank->designInfo.m_hBasePart=pGroupJg->handle;
			pPlank->designInfo.norm.norm_style=1;	//�Ǹ�֫���߷���
			pPlank->designInfo.norm.hVicePart=pBaseJg->handle;
			pPlank->designInfo.norm.direction=0;
			//���λ�ò��� wht 11-01-23
			pPlank->m_fNormOffset=-(0.5*fill_plank_dlg.m_nThick);
			pPlank->designInfo.origin.datum_pos_style=10;	//���ڵ��ı����ȷֵ�
			pPlank->designInfo.origin.des_para.SCALE_NODE.hLinePart=pGroupJg->handle;
			pPlank->designInfo.origin.des_para.SCALE_NODE.hStartNode=pStart->handle;
			pPlank->designInfo.origin.des_para.SCALE_NODE.hEndNode=pEnd->handle;
			pPlank->designInfo.origin.des_para.SCALE_NODE.start_offset_dist=fill_plank_dlg.m_fStartOffset;
			pPlank->designInfo.origin.des_para.SCALE_NODE.end_offset_dist=fill_plank_dlg.m_fEndOffset;
			pPlank->designInfo.origin.des_para.SCALE_NODE.offset_scale=unit_scale*(i+1);
			pPlank->designInfo.origin.des_para.SCALE_NODE.offset_dist=0;
			//��˨������Ʋ���
			ls.iSeg=SEGI(fill_plank_dlg.m_sSegI.GetBuffer());	//ָ����˨�κ� wht 11-07-01
			ls.des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
			ls.des_work_norm.hVicePart=pPlank->handle;
			ls.des_work_norm.nearVector.Set(0,0,1);
			if(pGroupJg->group_style==0)	//�Խ������
			{
				CLDSLineAngle *pOtherJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				JGZJ jgzj;
				if(fill_plank_dlg.m_iWing==0)
				{	//��ʼ����X֫��
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=0;
					else
						pPlank->designInfo.norm.norm_wing=1;
				}
				else
				{	//��ʼ����Y֫��
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=1;
					else
						pPlank->designInfo.norm.norm_wing=0;
				}
				pPlank->DesignSetupUcs();	//����ְ�װ������ϵ
				if(fabs(pPlank->ucs.axis_x*pBaseJg->GetWingVecX())>EPS_COS2)
				{
					vert_vec=pBaseJg->GetWingVecX();	//��˨��X֫��
					jgzj=jgzj_x;
				}
				else
				{
					vert_vec=pBaseJg->GetWingVecY();	//��˨��Y֫��
					jgzj=jgzj_y;
				}
				ls.EmptyL0DesignPara();	//�����˨��Ʋ���
				ls.AddL0Thick(pPlank->handle,TRUE);
				ls.AddL0Thick(pBaseJg->handle,TRUE);
				if(!ls.CalGuigeAuto())
				{
					char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
					sprintf(ss,"�Ҳ������ʵ���˨���M%dX%.0f.",ls.get_d(),ls.L0);
#endif
					throw ss;
				}
				for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
				{
					//��׼�Ǹ��������˨
					CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//��������
						ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)
							   +vert_vec*(fHalfGroupJgSpace+jgzj.g);
					else	//˫������
					{
						if(fill_plank_dlg.m_iLsLayOutStyle==0)	//��������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*(fHalfGroupJgSpace+jgzj.g1);
							else		//��������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*(fHalfGroupJgSpace+jgzj.g2);
						}
						else	//Զ������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*(fHalfGroupJgSpace+jgzj.g2);
							else		//��������˨
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
					//��˨λ�ò��� 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�����ϵ����λ��
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//��λ��˨
					pLs->correct_worknorm();
					pLs->correct_pos();
					pLs->CalGuigeAuto();
					pLs->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
					
					//�ǻ�׼�Ǹ��������˨
					if(pOtherJg==NULL)
						continue;
					//��һ���Ǹ������ϵĻ�׼��
					SnapPerp(&ber_pick,pOtherJg->Start(),pOtherJg->End(),pPlank->ucs.origin);
					pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pOtherJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//��������
						ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
					else	//˫������
					{
						if(nSecType==0)	//��������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
							else		//��������˨
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
						}
						else	//Զ������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							else		//��������˨
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
						}
					}
					pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank->GetThick()*0.5));
					ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
					pLs->ucs.origin=ls_pos;
					pOtherJg->AppendMidLsRef(pLs->GetLsRef());
					pPlank->AppendLsRef(pLs->GetLsRef());
					//��˨λ�ò��� 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�����ϵ����λ��
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//��λ��˨
					pLs->correct_worknorm();
					pLs->correct_pos();
					pLs->CalGuigeAuto();
					pLs->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
				}
				pPlank->CalStdProfile();	//����ְ�����
				pPlank->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlank->GetSolidPartObject());

				if(fill_plank_dlg.m_iType==1)
				{
					CLDSPlate *pPlank2=NULL;
					if(fill_plank_dlg.m_iType==1)
					{
						pPlank2=(CLDSPlate*)console.AppendPart(CLS_PLATE);
						pPlank2->CopyProperty(pPlank);
						//���øְ����Ի�����
						pPlank2->designInfo.m_hBasePart=pGroupJg->handle;
						pPlank2->designInfo.norm.norm_style=1;	//�Ǹ�֫���߷���
						pPlank2->designInfo.norm.hVicePart=pBaseJg->handle;
						pPlank2->designInfo.norm.direction=0;
						//�ְ�λ�ò��� 
						pPlank2->designInfo=pPlank->designInfo;
					}
					if(fill_plank_dlg.m_iWing==1)
					{	//��ʼ����X֫��
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=0;
						else
							pPlank2->designInfo.norm.norm_wing=1;
					}
					else
					{	//��ʼ����Y֫��
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=1;
						else
							pPlank2->designInfo.norm.norm_wing=0;
					}
					pPlank2->DesignSetupUcs();	//����ְ�װ������ϵ
					if(fabs(pPlank2->ucs.axis_y*pBaseJg->GetWingVecX())>EPS_COS2)
					{
						vert_vec=pBaseJg->GetWingVecX();	//��˨��X֫��
						jgzj=jgzj_x;
					}
					else
					{
						vert_vec=pBaseJg->GetWingVecY();	//��˨��Y֫��
						jgzj=jgzj_y;
					}
					//��˨������Ʋ���
					ls.des_work_norm.hVicePart=pPlank2->handle;
					ls.EmptyL0DesignPara();	//�����˨��Ʋ���
					ls.AddL0Thick(pPlank2->handle,TRUE);
					ls.AddL0Thick(pBaseJg->handle,TRUE);
					if(!ls.CalGuigeAuto())
					{
						char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
						sprintf(ss,"�Ҳ������ʵ���˨���M%dX%.0f.",ls.get_d(),ls.L0);
#endif
						throw ss;
					}
					for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
					{
						//��׼�Ǹ��������˨
						CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pBaseJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
						pLs->des_work_norm=ls.des_work_norm;
						pLs->set_norm(pPlank2->ucs.axis_z);
						if(fill_plank_dlg.m_iLsRows==0)	//��������
							ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj.g;
						else	//˫������
						{
							if(fill_plank_dlg.m_iLsLayOutStyle==0)	//��������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
								else		//��������˨
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
							}
							else	//Զ������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
								else		//��������˨
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
						//��˨λ�ò��� 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�˼��ϵ����λ��
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						//��λ��˨
						pLs->correct_worknorm();
						pLs->correct_pos();
						pLs->CalGuigeAuto();
						pLs->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
						
						//�ǻ�׼�Ǹ��������˨
						if(pOtherJg==NULL)
							continue;
						pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pOtherJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
						pLs->des_work_norm=ls.des_work_norm;
						pLs->set_norm(pPlank2->ucs.axis_z);
						if(fill_plank_dlg.m_iLsRows==0)	//��������
							ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
						else	//˫������
						{
							if(nSecType==0)	//��������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
								else		//��������˨
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							}
							else	//Զ������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
								else		//��������˨
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
							}
						}
						pLs->des_base_pos.norm_offset.AddThick(-ftol(pPlank2->GetThick()*0.5));
						ls_pos=ls_pos+pLs->get_norm()*pLs->des_base_pos.norm_offset.Thick(pLs->BelongModel());
						pLs->ucs.origin=ls_pos;
						pOtherJg->AppendMidLsRef(pLs->GetLsRef());
						pPlank2->AppendLsRef(pLs->GetLsRef());
						//��˨λ�ò��� 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�˼��ϵ����λ��
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						//��λ��˨
						pLs->correct_worknorm();
						pLs->correct_pos();
						pLs->CalGuigeAuto();
						pLs->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pLs->GetSolidPartObject());
					}
					pPlank2->CalStdProfile();	//����ְ�����
					//
					CLDSPlate *pNewPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
					long hNew=pNewPlate->handle;	//����ְ���
					pPlank->CloneTo(*pNewPlate);	//��¡�ְ� wht 11-01-07
					pNewPlate->handle=hNew;			//���¸ְ���
					pNewPlate->relativeObjs.Empty();//��չ��������б�
					pNewPlate->EmptyLsRef();		//�����˨����
					//������˨����
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
			else if(pGroupJg->group_style==1)//T�������
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
					throw "�Ҳ�����ϽǸֵ���һ�ǻ�׼�ӽǸ�!";
#endif
				
				vert_vec=pBaseJg->GetWingVecX();	//��ʼ����Y֫��
				pPlank->designInfo.norm.norm_wing=0;
				pPlank->DesignSetupUcs();	//����ְ�װ������ϵ
				ls.EmptyL0DesignPara();	//�����˨��Ʋ���
				ls.AddL0Thick(pPlank->handle,TRUE);
				ls.AddL0Thick(pBaseJg->handle,TRUE);
				ls.AddL0Thick(pOtherJg->handle,TRUE);
				if(!ls.CalGuigeAuto())
				{
					char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
					sprintf(ss,"�Ҳ������ʵ���˨���M%dX%.0f.",ls.get_d(),ls.L0);
#endif
					throw ss;
				}
				for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
				{
					//��׼�Ǹ��������˨
					CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//��������
						ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj_x.g;
					else	//˫������
					{
						if(fill_plank_dlg.m_iLsLayOutStyle==0)	//��������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj_x.g1;
							else		//��������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj_x.g2;
						}
						else	//Զ������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj_x.g2;
							else		//��������˨
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
					//��˨λ�ò��� 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�˼��ϵ����λ��
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//��λ��˨
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
			else if(pGroupJg->group_style==2)//ʮ�������
			{
				CLDSLineAngle *pOtherJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				CLDSLineAngle *pOtherJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
				CLDSLineAngle *pOtherJg3=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
				JGZJ jgzj;
				if(fill_plank_dlg.m_iWing==0)
				{	//��ʼ����X֫��
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=0;
					else
						pPlank->designInfo.norm.norm_wing=1;
				}
				else
				{	//��ʼ����Y֫��
					if(!fill_plank_dlg.m_bSwapWing||i%2==0)
						pPlank->designInfo.norm.norm_wing=1;
					else
						pPlank->designInfo.norm.norm_wing=0;
				}
				pPlank->DesignSetupUcs();	//��Ƹְ�װ������ϵ
				if(fabs(pPlank->ucs.axis_x*pBaseJg->GetWingVecX())>EPS_COS2)
				{
					vert_vec=pBaseJg->GetWingVecX();	//��˨��X֫��
					jgzj=jgzj_x;
				}
				else
				{
					vert_vec=pBaseJg->GetWingVecY();	//��˨��Y֫��
					jgzj=jgzj_y;
				}
				ls.EmptyL0DesignPara();	//�����˨��Ʋ���
				ls.AddL0Thick(pPlank->handle,TRUE);
				ls.AddL0Thick(pBaseJg->handle,TRUE);
				ls.AddL0Thick(ftol(pBaseJg->GetThick()),FALSE);
				if(!ls.CalGuigeAuto())
				{
					char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
					sprintf(ss,"�Ҳ������ʵ���˨���M%dX%.0f.",ls.get_d(),ls.L0);
#endif
					throw ss;
				}
				for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
				{
					//��׼�Ǹ��������˨
					CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//��������
						ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj.g;
					else	//˫������
					{
						if(fill_plank_dlg.m_iLsLayOutStyle==0)	//��������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
							else		//��������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
						}
						else	//Զ������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=pPlank->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
							else		//��������˨
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
					//��˨λ�ò��� 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�˼��ϵ����λ��
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					
					//�ǻ�׼�Ǹ��������˨
					if(pOtherJg2==NULL)
						continue;
					//��һ���Ǹ������ϵĻ�׼��
					SnapPerp(&ber_pick,pOtherJg2->Start(),pOtherJg2->End(),pPlank->ucs.origin);
					pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pLs->CopyProperty(&ls);
					pLs->cfgword=pBaseJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
					pLs->des_work_norm=ls.des_work_norm;
					pLs->set_norm(pPlank->ucs.axis_z);
					if(fill_plank_dlg.m_iLsRows==0)	//��������
						ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
					else	//˫������
					{
						if(nSecType==0)	//��������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
							else		//��������˨
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
						}
						else	//Զ������һ������
						{
							if(j%2==0)	//ż������˨
								ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
								+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							else		//��������˨
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
					//��˨λ�ò��� 
					pLs->des_base_pos.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�˼��ϵ����λ��
					pLs->des_base_pos.datumPoint.des_para.hPart=pPlank->handle;
					pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
					coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank->ucs,FALSE);
					pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
					pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
					//��λ��˨
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
					//���øְ����Ի�����
					pPlank2->designInfo.m_hBasePart=pGroupJg->handle;
					pPlank2->designInfo.norm.norm_style=1;	//�Ǹ�֫���߷���
					pPlank2->designInfo.norm.hVicePart=pBaseJg->handle;
					pPlank2->designInfo.norm.direction=0;
					//�ְ�λ�ò��� 
					pPlank2->designInfo=pPlank->designInfo;
					if(fill_plank_dlg.m_iWing==1)
					{	//��ʼ����X֫��
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=0;
						else
							pPlank2->designInfo.norm.norm_wing=1;
					}
					else
					{	//��ʼ����Y֫��
						if(!fill_plank_dlg.m_bSwapWing||i%2==0)
							pPlank2->designInfo.norm.norm_wing=1;
						else
							pPlank2->designInfo.norm.norm_wing=0;
					}
					pPlank2->DesignSetupUcs();	//�ְ�װ������ϵ
					if(fabs(pPlank2->ucs.axis_y*pBaseJg->GetWingVecX())>EPS_COS2)
					{
						vert_vec=pBaseJg->GetWingVecX();	//��˨��X֫��
						jgzj=jgzj_x;
					}
					else
					{
						vert_vec=pBaseJg->GetWingVecY();	//��˨��Y֫��
						jgzj=jgzj_y;
					}
					ls.des_work_norm.hVicePart=pPlank2->handle;
					ls.EmptyL0DesignPara();	//�����˨��Ʋ���
					ls.AddL0Thick(pPlank2->handle,TRUE);
					ls.AddL0Thick(pBaseJg->handle,TRUE);
					ls.AddL0Thick(ftol(pBaseJg->GetThick()),FALSE);
					if(!ls.CalGuigeAuto())
					{
						char ss[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(ss,"Can't find fit bolt's G M%dX%.0f.",ls.get_d(),ls.L0);
#else 
						sprintf(ss,"�Ҳ������ʵ���˨���M%dX%.0f.",ls.get_d(),ls.L0);
#endif
						throw ss;
					}
					for(j=0;j<fill_plank_dlg.m_nLsNum;j++)
					{
						//��׼�Ǹ��������˨
						CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pBaseJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
						pLs->des_work_norm=ls.des_work_norm;
						pLs->set_norm(pPlank2->ucs.axis_z);
						if(fill_plank_dlg.m_iLsRows==0)	//��������
							ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)+vert_vec*jgzj.g;
						else	//˫������
						{
							if(fill_plank_dlg.m_iLsLayOutStyle==0)	//��������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g1;
								else		//��������˨
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
							}
							else	//Զ������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=pPlank2->ucs.origin+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)+vert_vec*jgzj.g2;
								else		//��������˨
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
						//��˨λ�ò��� 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�˼��ϵ����λ��
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						
						//�ǻ�׼�Ǹ��������˨
						if(pOtherJg2==NULL)
							continue;
						pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						pLs->CopyProperty(&ls);
						pLs->cfgword=pBaseJg->cfgword;	//������˨��ĺ����׼������ĺ�һ��
						pLs->des_work_norm=ls.des_work_norm;
						if(fill_plank_dlg.m_iLsRows==0)	//��������
							ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace+LsSpace.SingleRowSpace*j)-vert_vec*jgzj.g;
						else	//˫������
						{
							if(nSecType==0)	//��������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g2;
								else		//��������˨
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
							}
							else	//Զ������һ������
							{
								if(j%2==0)	//ż������˨
									ls_pos=ber_pick+vec*(-plank_len*0.5+LsSpace.EndSpace
									+LsSpace.doubleRowSpace*j*0.5)-vert_vec*jgzj.g1;
								else		//��������˨
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
						//��˨λ�ò��� 
						pLs->des_base_pos.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos_style=9;	//�˼��ϵ����λ��
						pLs->des_base_pos.datumPoint.des_para.hPart=pPlank2->handle;
						pLs->des_base_pos.datumPoint.datum_pos=ls_pos;
						coord_trans(pLs->des_base_pos.datumPoint.datum_pos,pPlank2->ucs,FALSE);
						pLs->des_base_pos.datumPoint.datum_pos.z=0;	//Z�������
						pLs->des_base_pos.datumPoint.SetPosition(ls_pos);
						//��λ��˨
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
					long hNew=pNewPlate->handle;	//����ְ���
					pPlank->CloneTo(*pNewPlate);	//��¡�ְ� wht 11-01-07
					pNewPlate->handle=hNew;			//���¸ְ���
					pNewPlate->relativeObjs.Empty();//��չ��������б�
					pNewPlate->EmptyLsRef();		//�����˨����
					//������˨����
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
				throw "�����˴���ĽǸ������ʽ!��岼��ʧ��!";
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

//-----------VVV---���ýǸ���˨--------VVV--------
//���ýǸ���˨ һ�����õ���˨����  �������õ��Ǹ���˨
//һ�����õ��Ǹ���˨
void CLDSView::OnLayJgEndLs() 
{
	m_nPrevCommandID=ID_LAY_JG_END_LS;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to layout angle's bolts";
#else 
	m_sPrevCommandName="�ظ����ýǸ���˨";
#endif
#ifndef __LDS_
	Command("LayoutAngleBolts");
#else
	Command("AngleBolts");	//��ʱLayoutAngleBolts������Ƿ����ȫȡ�� wjh-2019.8.6
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
	//��׽��������˨�ĸ˼�����
	CLDSLinePart* pCurRod=NULL;
	HIBERID hiberid;
	f3dLine briefline;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("AngleBolt Please select angle bolt line:","");
#else 
	pCmdLine->FillCmdLine("AngleBolt ��ѡ����Ҫ������˨�ĸ˼�����:","");
#endif
	CSnapTypeVerify verifier(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbLine));
	verifier.SetEntsSelectLevel(2);	//����ѡ�нǸ�֫����
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
		pCmdLine->FillCmdLine("AngleBolt û��ѡ�к��ʵĸ˼����ߣ�������ѡ����Ҫ������˨�ĸ˼�����:","");
#endif
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X",pCurRod->handle));
	//ѡ��ڵ㲼�ý��涨λ��˨��ѡ����һ���˼�֫���߲��ý������˨
	CLDSNode* pBaseNode=NULL;
	CLDSLinePart* pOtherRod=NULL;
	if(pCurRod->GetClassTypeId()==CLS_LINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("AngleBolt Please select connected node which angle's bolts attached��or select other angle bolt line:","");
#else
		pCmdLine->FillCmdLine("AngleBolt ��ѡ��˼��ϲ�����˨�Ķ�λ��׼(�ڵ㡢�ཻ�Ǹ�)���򰴼�<�س�|�ո�>����:","");
#endif
		
		verifier.ClearSnapFlag();
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE)|GetSingleWord(SELECTINDEX_LINEANGLE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("AngleBolt Please select connected node which angle's bolts attached��or select other angle bolt line:","");
#else
		pCmdLine->FillCmdLine("AngleBolt ��ѡ��˼��ϲ�����˨�Ķ�λ����ڵ㣬�򰴼�<�س�|�ո�>����:","");
#endif

		verifier.ClearSnapFlag();
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
	}
	{//����������ƽڵ���ʾ����������
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
				continue;	//�����ظ�ѡ��ͬһ���Ǹ�
			if(obj.hRelaObj==0 && obj.ciTriggerType == SELOBJ::TRIGGER_KEYRETURN)
				break;	//���س���ո�����
			if((pBaseNode=console.FromNodeHandle(obj.hRelaObj))==NULL)
				pOtherRod=(CLDSLinePart*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
			if(pBaseNode)
			{ 
				if(briefline.PtInLine(pBaseNode->Position(false))!=0)
					break;
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("AngleBolt Nonselect valid angle. Please select angle bolt line:","");
#else 
				pCmdLine->FillCmdLine("AngleBolt û��ѡ�к��ʵĽڵ㣬������ѡ��ڵ㣬���ཻ�ĽǸ�:","");
#endif
			}
			else if(pOtherRod)
			{
				if(pOtherRod->pStart&&pOtherRod->pEnd)
					break;
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("AngleBolt Nonselect valid angle. Please select angle bolt line:","");
#else 
				pCmdLine->FillCmdLine("AngleBolt û��ѡ����Ч�Ľ���Ǹ֣�������ѡ��ڵ㣬���ཻ�ĽǸ�:","");
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
			pCmdLine->FinishCmdLine("<�س�>");
	}
	//������˨
	CUndoOperObject undo(&Ta,true);
	if(pCurRod)
	{
		BYTE ciSelWorkWing=0;	//��ǰѡ�еĽǸ�����֫��'X','Y'
		if(pCurRod->IsAngle())
		{
			if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==0)
				ciSelWorkWing=0;	//��ǰѡ�е���X֫����
			else if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==1)
				ciSelWorkWing=1;	//��ǰѡ�е���Y֫����
		}
		else if(pCurRod->GetClassTypeId()==CLS_LINESLOT)
		{
			if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==0)
				ciSelWorkWing=0;	//����
			else if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==1)
				ciSelWorkWing=1;	//X+֫
			else if(hiberid.HiberUpId(1)==0&&hiberid.HiberUpId(2)==2)
				ciSelWorkWing=2;	//X-֫
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
	pCmdLine->FillCmdLine("����:","");
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
	pCmdLine->FillCmdLine("LayoutAngleBolts ��ѡ����Ҫ������˨�ĽǸ�(�۸ֻ���):","");
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
					//��������˨�˳�����ʱ���л���ʵ����ʾģʽ
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
				pCmdLine->FillCmdLine("û��ѡ�к��ʵĹ�����������ѡ����Ҫ������˨�ĽǸ�(�۸ֻ���):","");
	#endif
			}
			g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
			pCmdLine->FinishCmdLine(CXhChar16("0X%X",pLinePart->handle));
		}
		pCmdLine->FinishCmdLine();
		if(!(pLinePart->IsAngle()&&pLinePart->pStart==NULL&&pLinePart->pEnd==NULL))
		{	//���ö̽Ǹ���˨ʱ������Ҫѡ��ڵ�
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
				pCmdLine->FillCmdLine("��ѡ��Ǹ��ϲ�����˨�����������ӽڵ�:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				pCmdLine->FillCmdLine("��ѡ��۸��ϲ�����˨�����������ӽڵ�:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
				pCmdLine->FillCmdLine("��ѡ�����ϲ�����˨�����������ӽڵ�:","");
			else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
				pCmdLine->FillCmdLine("��ѡ��ֹ��ϲ�����˨�����������ӽڵ�:","");
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
					//��������˨�˳�����ʱ���л���ʵ����ʾģʽ
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
		{	//������ϽǸ���˨
			g_pSolidSet->SetDisplayType(DISP_SOLID);
			g_pSolidDraw->Draw();
			CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
			for(i=0;i<4;i++)
			{
				CLDSLineAngle *pCommBaseJg=NULL,*pBackToBackJgX=NULL,*pBackToBackJgY=NULL;
				if(pGroupAngle->group_style==2)	
				{	//����ʮ����ϽǸ���˨ʱ�����貼��1�ź�3���ӽǸֵ���˨  wht 09-09-07
					//ϵͳ���Զ���1�ŽǸ��ϵ���˨���뵽2,4�ŽǸ��ϣ���3�ŽǸ��ϵ���˨���뵽2,4�ŽǸ��� 
					if(i==1||i==3)
						continue;
					if(i==0)
					{	//1�ŽǸ�X֫��˨���뵽2�ŽǸ֣�Y֫�ϵ���˨���뵽4�ŽǸ�	wht 09-10-11
						if(pGroupAngle->son_jg_h[1]>=0x20)
							pBackToBackJgX=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[1],CLS_LINEANGLE);
						if(pGroupAngle->son_jg_h[3]>0x20)
							pBackToBackJgY=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[3],CLS_LINEANGLE);
					}
					else if(i==2)
					{	//3�ŽǸ�Y֫��˨���뵽2�ŽǸ֣�X֫�ϵ���˨���뵽4�ŽǸ�	wht 09-10-11
						if(pGroupAngle->son_jg_h[3]>=0x20)
							pBackToBackJgX=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[3],CLS_LINEANGLE);
						if(pGroupAngle->son_jg_h[1]>=0x20)
							pBackToBackJgY=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[1],CLS_LINEANGLE);
					}
				}
				if(pGroupAngle&&pGroupAngle->son_jg_h[i]>=0x20)
					pCommBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
				if(pCommBaseJg==NULL||pCommBaseJg->m_bVirtualPart)
					continue;	//�����ڶ�Ӧ���ӽǸֻ��ӽǸ�Ϊ���⹹�� wht 11-07-25
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pCommBaseJg->handle);
				LayoutSingleAngleBolts(pCommBaseJg,pBaseNode,pBackToBackJgX,pBackToBackJgY);
			}
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)	//���õ��Ǹ�
			LayoutSingleAngleBolts((CLDSLineAngle*)pLinePart,pBaseNode);
		else if (pLinePart->GetClassTypeId()==CLS_LINEFLAT||pLinePart->GetClassTypeId()==CLS_LINESLOT)//�۸֡������˨
			LayoutSlotOrFlatBolts(pLinePart,pBaseNode);
		else if (pLinePart->GetClassTypeId()==CLS_LINETUBE)
			LayoutTubeBolts((CLDSLineTube*)pLinePart,pBaseNode);//�ֹ�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->Draw();	
		m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
		Ta.EndUndoListen();
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Command:","");
#else 
		pCmdLine->FillCmdLine("����:","");
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
		laybolt_dlg.m_iDatumPointStyle = 3;	//���߽���
		laybolt_dlg.m_hCrossAngle.Format("0X%X", pCrossAngle->handle);
	}
	if (laybolt_dlg.DoModal() != IDOK)
		return;
	//�����û������ڽǸ��ϲ�����˨
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->iSeg = pCurRod->iSeg;
		pBolt->CopyModuleInstanceInfo(pCurRod);	//������˨��ĺ����׼������ĺ�һ��
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
		{	//��ӹ��߿ף�����ʾ��˨ʵ�壬��ʾ�ױ�  wxc-2019.7.24
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
		{	//���ƽǸ�����˨��
			pCurRod->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pCurRod->GetSolidPartObject());
		}
		//����㶨λ
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
			{	//������˨λ�ڽ���Ǹ���
				pCrossAngle->AppendMidLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(pCrossAngle->handle,TRUE);
			}
			if (pBolt->m_bVirtualPart && pHoleWall)
			{	//���ƽ���Ǹ�����˨�׼��ױ�ʵ��
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
	//�����û������ڽǸ��ϲ�����˨
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSLineAngle *pBackToBackJg=NULL;
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->m_bVirtualPart=pLsPara->m_bVirtualPart;
		pBolt->iSeg = pLineTube->iSeg;
		pBolt->cfgword=pLineTube->cfgword;	//������˨��ĺ����׼������ĺ�һ��
		pBolt->SetGrade(pLsPara->grade);
		pBolt->AddL0Thick(pLineTube->handle,TRUE);
		if(pBackToBackJg)
		{	//������˨ͨ���Լ�����ƫ����
			pBolt->AddL0Thick(pBackToBackJg->handle,TRUE);
			if(pBolt->des_work_norm.direction==1)	//����
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
		laybolt_dlg.m_iRayDirection=1;	//�գ�>ʼ
	}
	else if(pBaseNode==pLinePart->pStart)
	{
		laybolt_dlg.m_iOddCalStyle=pLinePart->desStartOdd.m_iOddCalStyle;
		laybolt_dlg.m_fOddment=pLinePart->startOdd();
		laybolt_dlg.m_iRayDirection=0;	//ʼ��>��
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
	//�����û������ڽǸ��ϲ�����˨
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->iSeg = pLinePart->iSeg;
		pBolt->cfgword=pLinePart->cfgword;	//������˨��ĺ����׼������ĺ�һ��
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
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//����㶨λ
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
				{	//������˨λ�ڽ���Ǹ���
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
		laybolt_dlg.m_iRayDirection=1;	//�գ�>ʼ
	}
	else if((pBaseNode&&pBaseNode==pLineAngle->pStart)||
			(pLineAngle->pStart==NULL&&pLineAngle->pEnd==NULL))
	{
		laybolt_dlg.m_iOddCalStyle=pLineAngle->desStartOdd.m_iOddCalStyle;
		laybolt_dlg.m_fOddment=pLineAngle->startOdd();
		laybolt_dlg.m_iRayDirection=0;	//ʼ��>��
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
	//�����û������ڽǸ��ϲ�����˨
	for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
	{
		CLDSLineAngle *pBackToBackJg=NULL;
		if(pLsPara->des_work_norm.norm_wing==0)		//X֫
			pBackToBackJg=pBackToBackJgX;
		else if(pLsPara->des_work_norm.norm_wing==1)//Y֫
			pBackToBackJg=pBackToBackJgY;
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		pBolt->set_d(pLsPara->d);
		pBolt->iSeg = pLineAngle->iSeg;
		pBolt->CopyModuleInstanceInfo(pLineAngle);	//������˨��ĺ����׼������ĺ�һ��
		pBolt->SetGrade(pLsPara->grade);
		pBolt->AddL0Thick(pLineAngle->handle,TRUE);
		if(pBackToBackJg)
		{	//������˨ͨ���Լ�����ƫ����
			pBolt->AddL0Thick(pBackToBackJg->handle,TRUE);
			if(pBolt->des_work_norm.direction==1)	//����
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
			if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE&&pBackToBackJg)	//����˨���뵽���Ա��ĽǸ��� wht 09-09-07
				pBackToBackJg->AppendStartLsRef(pBolt->GetLsRef());
		}
		else if(pBaseNode==pLineAngle->pEnd)
		{
			pLineAngle->AppendEndLsRef(pBolt->GetLsRef());
			if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE&&pBackToBackJg)	//����˨���뵽���Ա��ĽǸ���
				pBackToBackJg->AppendEndLsRef(pBolt->GetLsRef());
		}
		else
		{
			pLineAngle->AppendMidLsRef(pBolt->GetLsRef());
			if(pBackToBackJg)	//����˨���뵽���Ա��ĽǸ���
				pBackToBackJg->AppendMidLsRef(pBolt->GetLsRef());
		}
		if(pBolt->des_base_pos.datumPoint.datum_pos_style==3)	//����㶨λ
		{
			CLDSLinePart *pLinePart=(CLDSLineAngle*)console.FromPartHandle(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
			if(pLinePart)
			{
				f3dPoint wing_vec,wing_norm,datum_pos;
				if(pLinePart->IsAngle())
				{	//�Ǹ�
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
				{	//����
					wing_norm=pLinePart->ucs.axis_z;
					wing_vec=pLinePart->ucs.axis_y;
					datum_pos=pLinePart->Start()-wing_vec*pLinePart->size_wide*0.5;
				}
				else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
				{	//�۸�
					wing_norm=pLinePart->ucs.axis_y;
					wing_vec=pLinePart->ucs.axis_x;
					datum_pos=pLinePart->Start()-wing_vec*pLinePart->size_wide*0.5;
				}
				f3dPoint pos;
				Int3dlf(pos,pBolt->ucs.origin,pBolt->ucs.axis_z,pLinePart->Start(),wing_norm);
				f3dPoint bolt_vec=pos-datum_pos;
				double dd=bolt_vec*wing_vec;
				if(dd>0&&dd<pLinePart->GetWidth())
				{	//������˨λ�ڽ���Ǹ���
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
//��������˨���Ӱ�����1.����㵥��˨ 2.���Ǹֶ���˨���� 3.�ް嵥��˨����
void CLDSView::OnOneBoltDesign()
{	//����˨����
	m_nPrevCommandID=ID_ONEBOLT_DESIGN;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to connect single bolts";
#else 
	m_sPrevCommandName="�ظ�����˨����";
#endif
	Command("1Bolt");
}

void CLDSView::OnSpecNodeOneBoltDesign()
{	//�������ɵ���˨����
	m_nPrevCommandID=ID_SPEC_NODE_ONEBOLT_DESIGN;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat to specify single-bolt connection";
#else 
	m_sPrevCommandName="�ظ�ָ���ڵ㵥��˨����";
#endif
	Command("Spec1Bolt");
}
//��Ƶ��
static BOOL DesignDianBan(CLDSBolt *pLs,BOOL bThrowError,CLDSNode* pNode,
						 double dist,int nType,CDianBanParaDlg *pDlg=NULL)
{
	CDianBanParaDlg dianbandlg;
	CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(pNode->arrRelationPole[1],CLS_LINEPART);
	CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(pNode->arrRelationPole[0],CLS_LINEPART);
	if(dianbandlg.m_nSpaceDist==0||dianbandlg.m_nThick==0)
	{
		dianbandlg.m_sLsGuiGe.Format("%d",pLs->get_d());
		dianbandlg.m_nSpaceDist = (int)(dist+0.5);//Բ�� pLs->DianQuan.thick;
		dianbandlg.m_iPartType=nType;
		dianbandlg.m_nThick=(nType==1)?pLs->DianQuan.thick:dianbandlg.m_nSpaceDist;
		if(dianbandlg.m_nThick>0)
			dianbandlg.m_nNum=dianbandlg.m_nSpaceDist/dianbandlg.m_nThick;
	}
	//�Գ�������˨���ʱ���ǰ���϶ֵ����ͬ����Ҫ������ƶԻ��� wht 11-01-19
	if((pDlg&&pDlg->m_bMirCreate&&dianbandlg.m_nSpaceDist==pDlg->m_nSpaceDist)	
		||!bThrowError||dianbandlg.DoModal()==IDOK)
	{	//
		//���Ʊ���Ĳ�������ǰ�Ի��� wht 11-01-10
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
				pPadPlank->cfgword=pLinePart1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
				pPadPlank->designInfo.m_hBasePart=pLinePart1->handle;
				if(i==iCycle-1&&nSurplus>0&&dianbandlg.m_bCreateSurplusPlate)
					pPadPlank->Thick=nSurplus;
				else
					pPadPlank->Thick=dianbandlg.m_nThick;
				pPadPlank->jdb_style=4;
				if(pDatumAngle)
				{
					pPadPlank->designInfo.m_hBasePart=pDatumAngle->handle;	//��֤�ְ��׼������ԭ�㼰���߻�׼����һ�� wht 15-06-25
					pPadPlank->designInfo.origin.datum_pos_style=2;
					pPadPlank->designInfo.origin.des_para.RODNODE.hRod=pDatumAngle->handle;
					pPadPlank->designInfo.origin.des_para.RODNODE.wing_offset_style=0;
					pPadPlank->designInfo.origin.des_para.RODNODE.hNode=pNode->handle;
					if(fabs(ddx)>fabs(ddy)&&fabs(ddx)>EPS_COS2)	//X֫Ϊ����֫
						pPadPlank->designInfo.origin.des_para.RODNODE.offset_wing=0;
					else if(fabs(ddy)>fabs(ddx)&&fabs(ddy)>EPS_COS2)
						pPadPlank->designInfo.origin.des_para.RODNODE.offset_wing=1;
					pPadPlank->designInfo.origin.UpdatePos(pPadPlank->BelongModel());
					pPadPlank->ucs.origin=pPadPlank->designInfo.origin.Position();
					//������˨��巨�߲���
					pPadPlank->designInfo.norm.norm_style=1;
					pPadPlank->designInfo.norm.hVicePart=pDatumAngle->handle;
					pPadPlank->designInfo.norm.norm_wing=pPadPlank->designInfo.origin.des_para.RODNODE.offset_wing;
					pPadPlank->designInfo.norm.direction=0;
				}
				else
				{
					pPadPlank->ucs.origin=pLs->ucs.origin+pLs->get_norm()*(pLs->DianQuan.offset+i*dianbandlg.m_nThick);
					//������˨���ԭ�㼰���߲���
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
				pLs->DianQuan.AutoMatchThick=false;	//����ָ����Ȧ��ȣ��Ժ����������
			if(pDatumAngle)
				pLs->DianQuan.offset=pDatumAngle->GetThick();
			else
				pLs->DianQuan.offset=0;
		}
	}
	else
	{	//��������,�������һ�������,�п��ܵ�����˨ͨ�񳬳� wjh-2015.4.8
		pLs->DianQuan.N = 0;
		pLs->DianQuan.offset = 0;
		pLs->DianQuan.thick = 0;
	}
	//������Ի������ò��� wht 11-01-10
	if(pDlg&&(!pDlg->m_bMirCreate||(pDlg->m_bMirCreate&&dianbandlg.m_nSpaceDist!=pDlg->m_nSpaceDist)))
		pDlg->CopyProperty(dianbandlg);
	return TRUE;
}
extern BOOL UnifyAngleBoltParamG(DESIGN_LS_POS &designLsPos);
//1.��ƽ���㵥��˨ 
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
				throw "ѡ��Ƿ�����";
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
				throw "ѡ��ڵ㲻�ǽ���ڵ�";
#endif
			else
				return FALSE;
		}
		if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Without modify permission of the node��";
#else 
				throw "û�д˽ڵ���޸�Ȩ�ޣ�";
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
				throw "ѡ��ڵ�ȱ����������";
#endif
			else
				return FALSE;
		}
		//���ý�����Ƿ��Ѿ����ڽ������˨
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
				if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==3)	//���Ǹֽ���㶨λ
				{
					long hDatum1=(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
					long hDatum2=(*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
					if( (hDatum1==pLinePart1->handle&&hDatum2==pLinePart2->handle)||
						(hDatum2==pLinePart1->handle&&hDatum1==pLinePart2->handle))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "Intersection bolt 0x%X already exists and does not allow the repeat design!",(*pLsRef)->handle);
#else
						logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"�Ѵ���0x%X�������˨�������ظ����!",(*pLsRef)->handle);
#endif
						return FALSE;
					}
				}
			}
		}
		//1.�趨��˨������Ʋ���
		double offsetDist;
		if(pLinePart1->IsAngle())
		{	//��˨��׼����Ϊ�Ǹ�
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
			//��˨����λ�ڽǸֵ�ĳ֫������
			IsInsideJg(pAngle1,ls_norm,&x_wing0_y_wing1);
			if(x_wing0_y_wing1==0)
				ls_norm=pAngle1->get_norm_x_wing();
			else 
				ls_norm=pAngle1->get_norm_y_wing();
		}
		else if(pLinePart1->GetClassTypeId()==CLS_LINEFLAT)
		{	// ��˨��׼����Ϊ��������˨���߾��Ǳ����ķ���
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
		//2.ȷ����˨���(ֱ��)
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
			if(pJg->pStart==pNode)	//���ݵ��˵�����˨ֱ����С
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
				{	//��������㴦���ӵĽǸ�λ��	δ���...
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
			sprintf(sError,"����˨������û���ҵ�����M%dX%.f����˨���",ls.get_d(),ls.L0);
#endif
			if(bThrowError)
				throw sError;
			else
				return FALSE;
		}
		//3.��˨λ��(����ƫ����)�����λ��ȷ��
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
				throw "��������������";
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
			ls.DianQuan.offset=pAngle1->GetThick();	//��Ȧλ��
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
			ls.DianQuan.offset=pLineFlat1->GetThick();	//��Ȧλ��
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
		pLs->cfgword=pLinePart1->cfgword;	//������˨��ĺ����׼������ĺ�һ��
		dist=ftoi(dist);	//Ϊ����ǰ��һ�£�ͳһ������Բ��Ϊ�������жϵ�Ȧ wjh-2019.9.2
		int nType=pLs->CalPadPara(dist); //0.��� 1.��Ȧ
		int nMaxWasherThick=CLDSPart::library->GetBoltPadThick(pLs->d,1);
		double dfMinPromptSpaceByDianBan=nMaxWasherThick*2+1.2;
		if (nType==DIANQUAN_STRU::DIANBAN||dist>=dfMinPromptSpaceByDianBan||
			(g_sysPara.b1BoltDesignPrompt&&dist>0))	//��ֹ���϶����10)ʱ������ʾ�������Ƴ�3����Ȧ wjh-2019.7.24
			DesignDianBan(pLs,bThrowError,pNode,dist,DIANQUAN_STRU::DIANBAN,pDlg);	//�������˵>8һ��������϶࣬�ʻ���Ĭ��Ϊ������ wjh-2019.9.2
		pLs->DianQuan.offset=pLinePart1->GetThick();
		if(dist>0)
		{	//����Ȧʱ����Ҫ���dist wht 11-07-08
			if(!(pLs->DianQuan.N>0&&pLs->DianQuan.thick>0))
				pLs->AddL0Thick(ftoi(dist),FALSE);
		}
		//��˨λ�������Ϣ���
		pLs->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
		pLs->des_base_pos.hPart=pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pLinePart1->handle;
		pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pLinePart2->handle;
		if(pAngle1)
		{
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=jgzj1.g;
		}
		if(pAngle2)
		{
			pLs->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4; //�Զ���
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
			logerr.LevelLog(1, "ѡ��ڵ㲻�ǽ���ڵ�");
		return FALSE;
	}
	if (theApp.m_bCooperativeWork && !theApp.IsHasModifyPerm(pNode->dwPermission))
	{
		if (bThrowError)
			logerr.LevelLog(1, "û�д˽ڵ�(0X%X)���޸�Ȩ�ޣ�",pNode->handle);
		return FALSE;
	}
	CLDSLineAngle* pCrossJg1 = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[0], CLS_LINEANGLE);
	CLDSLineAngle* pCrossJg2 = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[1], CLS_LINEANGLE);
	if (pCrossJg1 == NULL || pCrossJg2 == NULL)
	{
		if (bThrowError)
			logerr.LevelLog(1, "ѡ��ڵ�(0X%X)ȱ������������", pNode->handle);
		return FALSE;
	}
	//���ý�����Ƿ��Ѿ����ڽ������˨
	for (int i = 0; i < 2; i++)
	{
		CLDSLineAngle* pJg = (i == 0) ? pCrossJg1 : pCrossJg2;
		if (pJg == NULL)
			continue;
		for (CLsRef *pLsRef = pJg->GetFirstLsRef(); pLsRef; pLsRef = pJg->GetNextLsRef())
		{
			if ((*pLsRef)->des_base_pos.datumPoint.datum_pos_style == 3)	
			{	//���Ǹֽ���㶨λ
				long hDatum1 = (*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
				long hDatum2 = (*pLsRef)->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
				if ((hDatum1 == pCrossJg1->handle&&hDatum2 == pCrossJg2->handle) ||
					(hDatum2 == pCrossJg1->handle&&hDatum1 == pCrossJg2->handle))
				{
					if(bThrowError)
						logerr.LevelLog(3, "�Ѵ���0x%X�������˨�������ظ����!", (*pLsRef)->handle);
					return FALSE;
				}
			}
		}
	}
	//���ݽ���˼�������ȷ����˨���(ֱ��)
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
	//�趨��˨λ��(����ƫ����)
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
			logerr.LevelLog(1, "����ڵ�������Ǹ�(0X%X)�����㲼��˫��˨!", pCrossJg1->handle);
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
			logerr.LevelLog(1, "����ڵ�������Ǹ�(0X%X)�����㲼��˫��˨!", pCrossJg2->handle);
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
			logerr.LevelLog(1, "��������������");
		return FALSE;
	}
	//�����˨
	BOLTSET boltSet;
	for (int i = 0; i < 2; i++)
	{
		CLDSBolt *pLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->SetLayer(pNode->layer());
		pLs->iSeg = pNode->iSeg;
		pLs->cfgword = pCrossJg1->cfgword;	//������˨��ĺ����׼������ĺ�һ��
		pLs->set_d(ls_d);
		pLs->AddL0Thick(pCrossJg1->handle, TRUE);
		pLs->AddL0Thick(pCrossJg2->handle, TRUE);
		//��˨λ�������Ϣ���
		pLs->des_base_pos.hPart = pCrossJg1->handle;
		pLs->des_base_pos.datumPoint.datum_pos_style = 3;	//���߽���
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
	//������Ӱ�
	if (jgset.GetNodeNum() <= 0 && ftoi(dist) > 6)
	{	//�޵���ʱ��Ƶ��
		CLDSView* pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
		pView->DesignBoltPad(pCrossJg1, NULL, boltSet, ftoi(dist), 0);
	}
	else if (jgset.GetNodeNum() > 0 && ftoi(dist) > 6)
	{	//�е���ʱ����ڲ������
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
		//����ְ�����ϵ
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
		//������Ӹ˼���Ʋ���
		CDesignLjPartPara* pLinePartPara = NULL;
		for (int i = 0; i < 2; i++)
		{
			CLDSLineAngle* pJg = (i == 0) ? pCrossJg1 : pCrossJg2;
			BYTE ciWorkWing = (i == 0) ? ciWorkWing1 : ciWorkWing2;
			pLinePartPara = pPlate->designInfo.partList.Add(pJg->handle);	//����Ǹ�1
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
			//��������ͷ
			CLDSLineAngle *pJg = (CLDSLineAngle*)pRod;
			BYTE ciS0_E1 = (pRod->pStart == pNode) ? 0 : 1;
			DESIGNODDMENT* pOddDes = (pRod->pStart == pNode) ? &pJg->desStartOdd : &pJg->desEndOdd;
			pOddDes->m_fCollideDist = g_sysPara.VertexDist;
			pOddDes->m_iOddCalStyle = 0;
			pOddDes->m_hRefPart1 = pCrossJg1->handle;
			pOddDes->m_hRefPart2 = pCrossJg2->handle;
			pJg->CalStartOddment();
			pJg->CalEndOddment();
			//������˨
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
			//��˨λ�������Ϣ���
			pLs->des_base_pos.datumPoint.datum_pos_style = 1;	//�Ǹ����
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
			//����ְ����Ӹ˼�������
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
		//��ʼ���ְ���˨��Ϣ
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
		//��Ƹְ�
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
	if((n1!=1||n2!=1)&&AfxMessageBox("���߲Ķ�ͷ��˨����Ϊ1,�Ƿ����?",MB_YESNO)!=IDYES)
#endif
		return FALSE;
	else
		return TRUE;
}
//2.��Ƶ���˨������
//��ƽ�����˨������(�ް�)
BOOL DesignBoltOnlyConnect(CLDSNode *pNode,CLDSLinePart *pXieRod,CLDSLinePart *pXieRod2,CLDSLinePart *pXieRod3,
								   BOOL bThrowError,BOOL bCanUndo/*=TRUE*/)
{
	//����㵥��˨�������
	CLDSLineAngle *pXieJg=NULL,*pXieJg2=NULL;
	CLDSLineFlat *pXieFlat=NULL,*pXieFlat2=NULL;
	CLDSLineSlot* pXieSlot=NULL,*pXieSlot2=NULL;
	if(pXieRod&&pXieRod->IsAngle())
		pXieJg=(CLDSLineAngle*)pXieRod;
	else if(pXieRod&&pXieRod->GetClassTypeId()==CLS_LINEFLAT)
		pXieFlat=(CLDSLineFlat*)pXieRod;
	else if(pXieRod&&pXieRod->GetClassTypeId()==CLS_LINESLOT)
		pXieSlot=(CLDSLineSlot*)pXieRod;
	else	//�ݲ�֧�ָ������͵Ĺ�������˨�������
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
	{	//�������߽Ǹ���������������Ǹֽ���ͬһ��ʱҪ��֤ǰ�����Ǹ�Ϊ����ͬһ����������Ǹ� wjh-2017.3.2
		typedef CMultiOffsetPos* CMultiOffsetPosPtr;
		CMultiOffsetPosPtr desPosArr[3]={	arrRayAngles[0]->pStart->handle==pNode->handle?&arrRayAngles[0]->desStartPos:&arrRayAngles[0]->desEndPos,
											arrRayAngles[1]->pStart->handle==pNode->handle?&arrRayAngles[1]->desStartPos:&arrRayAngles[1]->desEndPos,
											arrRayAngles[2]->pStart->handle==pNode->handle?&arrRayAngles[2]->desStartPos:&arrRayAngles[2]->desEndPos};
		if(fabs(desPosArr[1]->len_offset_dist-desPosArr[2]->len_offset_dist)<EPS)
		{	//�ڶ��������������ͬһ��
			CLDSLineAngle* pTempAngle=arrRayAngles[0];
			memmove(arrRayAngles,&arrRayAngles[1],sizeof(CLDSLineAngle*)*2);
			arrRayAngles[2]=pTempAngle;
		}
		else if(fabs(desPosArr[0]->len_offset_dist-desPosArr[2]->len_offset_dist)<EPS)
		{	//��һ�������������ͬһ��
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
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�˼��ѽ��ж�ͷ��˨������ƣ���������˨���!",pXieRod->handle);
		return false;
	}
	if(  pXieRod2&&(
		(pXieRod2->pStart==pNode&&pXieRod2->GetLocalLsCount(1,100)>0)||
		(pXieRod2->pEnd==pNode&&pXieRod2->GetLocalLsCount(2,100)>0)))
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�˼��ѽ��ж�ͷ��˨������ƣ���������˨���!",pXieRod2->handle);
		return false;
	}
	if(  pXieRod3&&(
		(pXieRod3->pStart==pNode&&pXieRod2->GetLocalLsCount(1,100)>0)||
		(pXieRod3->pEnd==pNode&&pXieRod2->GetLocalLsCount(2,100)>0)))
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�˼��ѽ��ж�ͷ��˨������ƣ���������˨���!",pXieRod3->handle);
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
			throw "ѡ��Ƿ���������";
#endif
		else
			return FALSE;
	}
	if(pXieRod->pStart==NULL||pXieRod->pEnd==NULL)
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The selected part is short angle(start or end node is 0X0)��";
#else 
			throw "��ѡ�˼�Ϊ�̽Ǹ�(ʼ�˻��ն˽ڵ�Ϊ0X0)��";
#endif
		else
			return FALSE;
	}
	if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Without modify permission of the node��";
#else 
			throw "û�д˽ڵ���޸�Ȩ�ޣ�";
#endif
		else
			return FALSE;
	}
	if(theApp.m_bCooperativeWork&&pNode==pXieRod->pStart&&!theApp.IsHasModifyPerm(pXieRod->dwStartPermission))
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Don't have modify permission of rod's start��";
#else 
			throw "û�д˸˼�ʼ�˵��޸�Ȩ�ޣ�";
#endif
		else 
			return FALSE;
	}
	else if(theApp.m_bCooperativeWork&&pNode==pXieRod->pEnd&&!theApp.IsHasModifyPerm(pXieRod->dwEndPermission))
	{
		if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Don't have modify permission of rod's end��";
#else 
			throw "û�д˸˼��ն˵��޸�Ȩ�ޣ�";
#endif
		else
			return FALSE;
	}
	CLDSLineAngle* pXieAngle=NULL;
	if(pXieRod&&pXieRod->GetClassTypeId()==CLS_LINEANGLE)
		pXieAngle=(CLDSLineAngle*)pXieRod;
	//���Ӷ��ֶ�ָ���˽ڵ�������׼�Ǹֵ�����¶���˨���� wjh-2015.1.25
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
			logerr.Log("0X%X and its end node dependent rod0X%X doesn't live in any model��bolt-only design failed!",pXieRod->handle,pFatherRod->handle);
#else
			logerr.Log("0X%X������ƶ˽ڵ㸸�˼�0X%X���������κ�һ������ģ���У�����˨���ʧ��!",pXieRod->handle,pFatherRod->handle);
#endif
			return FALSE;
		}
		for(pLsRef=pXieRod->GetFirstLsRef();pLsRef;pLsRef=pXieRod->GetNextLsRef())
		{
			if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style==3)	//���Ǹֽ���㶨λ
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
					logerr.Log("�Ѵ���0x%X�˵㵥��˨�������ظ����!",(*pLsRef)->handle);
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
			throw "ѡ����һ�������Ľڵ�(û�л�׼�ǸֵĽڵ�)";
#endif
		else
			return FALSE;
	}
	CLDSGroupLineAngle *pGroupJg=NULL;	//�ڵ㸸�Ǹ�
	if(pFatherRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//�ڵ㸸�˼�Ϊ��ϽǸ�ʱ�Զ�ѡ����ʵ��ӽǸ�Ϊ����˨���ӻ�׼�Ǹ� wht 10-11-23
		pGroupJg=pFatherRod.pGroupAngle;
		if(pGroupJg->group_style!=0)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Currently only support single-bolt-connection on combined angle's child angle!";
#else 
				throw "Ŀǰ��֧���ڶԽ���ϽǸֵ��ӽǸ��Ͻ��е���˨����!";
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
			GetWorkNorm(pGroupJg,pXieJg,&norm);	//�����淨��
		else
			norm=(pXieFlat!=NULL ? pXieFlat->WorkPlaneNorm() : pXieSlot->WorkPlaneNorm());

		for(int i=0;i<4;i++)
		{
			if(pGroupJg->group_style<2&&i>=2)
				continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
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
				throw "���㸸�˼���֧�ָ�����!";
#endif
			else
				return FALSE;
		}
	}
	g_pSolidDraw->SetEntSnapStatus(pNode->handle);
	g_pSolidDraw->SetEntSnapStatus(pXieRod->handle);
	if(bCanUndo)	//֧�ֳ�������
		Ta.BeginUndoListen();	//��ʼ����
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
			if(pXieJg->desStartPos.jgber_cal_style==2)	//�򵥶�λʱ��Ϊ��������Զ��жϸı���㶨λ���ͣ���Ϊ�˹�ָ��ģʽ wjh-2016.3.24
				pXieJg->desStartPos.bUdfDatumPointMode=TRUE;
			if(bInsideJg1)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg->desStartPos.wing_x_offset.gStyle=4;
					pXieJg->desStartPos.wing_x_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg->desStartPos.wing_y_offset.gStyle=4;
					pXieJg->desStartPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
				{	//�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
			//���������˨ͨ���λ�� wht 10-11-04
			ls.AddL0Thick(pFatherRod->handle,TRUE);
			ls.AddL0Thick(pXieJg->handle,TRUE);
			if(!ls.CalGuigeAuto())
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find bolt specification M%dX%.f in specification library",pXieJg->connectStart.d,ls.get_L0());
#else 
				sprintf(sError,"����˨������û���ҵ�����M%dX%.f����˨���",pXieJg->connectStart.d,ls.L0);
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
			pBolt->layer(2)=pXieJg->layer(2);	//������˨ͼ����
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieJg->UnifiedCfgword();	//������˨��ĺ����׼������ĺ�һ��
			//��˨λ�����\�������������Ϣ���
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
			if(pXieJg->desStartPos.IsInDatumJgWingX())	//���ڸ��Ǹ�X֫
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
			else									//���ڸ��Ǹ�Y֫
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
				pBolt->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���߷���
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
			if(pXieJg->desEndPos.jgber_cal_style==2)	//�򵥶�λʱ��Ϊ��������Զ��жϸı���㶨λ���ͣ���Ϊ�˹�ָ��ģʽ wjh-2016.3.24
				pXieJg->desEndPos.bUdfDatumPointMode=TRUE;
			ls_pos=pXieJg->GetEndDatumPos();
			if(bInsideJg1)
			{
				if(x_wing0_y_wing1==0)
				{
					pXieJg->desEndPos.wing_x_offset.gStyle=4;
					pXieJg->desEndPos.wing_x_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg->desEndPos.wing_y_offset.gStyle=4;
					pXieJg->desEndPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
				{	//�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
			//�����˨ͨ����Ʋ���
			ls.EmptyL0DesignPara();	//�����˨ͨ����Ʋ���
			ls.AddL0Thick(pFatherRod->handle,TRUE);
			ls.AddL0Thick(pXieJg->handle,TRUE);
			ls.set_d(pXieJg->connectEnd.d);
			if(!ls.CalGuigeAuto())
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find bolt specification M%dX%.f in specification library",pXieJg->connectEnd.d,ls.get_L0());
#else 
				sprintf(sError,"����˨������û���ҵ�����M%dX%.f����˨���",pXieJg->connectEnd.d,ls.L0);
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
			pBolt->layer(2)=pXieJg->layer(2);	//������˨ͼ����
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieJg->UnifiedCfgword();	//������˨��ĺ����׼������ĺ�һ��
			//��˨λ�����\�������������Ϣ���
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
			if(pXieJg->desEndPos.IsInDatumJgWingX())	//���ڸ��Ǹ�X֫
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
			else									//���ڸ��Ǹ�Y֫
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
				pBolt->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���߷���
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
				throw "��ѡ���߲�,��ͨ����ǰ�ڵ�";
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
				sprintf(sError,"����˨������û���ҵ�����M%dX%.f����˨���",pXieRod->connectStart.d,ls.L0);
#endif
				if(bThrowError)
					throw sError;
				else
					return FALSE;
			}
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->CopyProperty(&ls);
			pBolt->iSeg=pXieRod->iSeg;
			//pBolt->layer(2)=pXieRod->layer(2);	//������˨ͼ����
			pBolt->CopyModuleInstanceInfo(pNode);
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieRod->UnifiedCfgword();	//������˨��ĺ����׼������ĺ�һ��
			//��˨λ�����\�������������Ϣ���
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			//ָ�������Ǳ���ʱ������Ҫ֫����ƫ�����ͺ�ƫ����
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieRod->handle;
			CTubeEndPosPara* pCurrPos=(pXieSlot!=NULL ? &pXieSlot->desStartPos : &pXieFlat->desStartPos);
			if(pCurrPos->IsInDatumJgWingX())	//���ڸ��Ǹ�X֫
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
			else									//���ڸ��Ǹ�Y֫
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
				if(pXieFlat)		//��������+
					pBolt->des_work_norm.direction=2;
				else if(pXieSlot)	//�۸ֲ�����+
					pBolt->des_work_norm.direction=4;
			}
			else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
			{
				pBolt->des_work_norm.norm_style=5;	//ָ���Ǹ�֫���߷���
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
				if(pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
					pBolt->des_work_norm.direction=2;	//��������+
				else 
					pBolt->des_work_norm.direction=4;	//�۸ֲ�����+
			}
			else //if(pFatherRod->IsAngle())
			{
				pBolt->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���߷���
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
			ls.EmptyL0DesignPara();	//�����˨ͨ����Ʋ���
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
				sprintf(sError,"����˨������û���ҵ�����M%dX%.f����˨���",pXieRod->connectEnd.d,ls.L0);
#endif
				if(bThrowError)
					throw sError;
				else
					return FALSE;
			}
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->CopyProperty(&ls);
			pBolt->iSeg=pXieRod->iSeg;
			//pBolt->layer(2)=pXieRod->layer(2);	//������˨ͼ����
			pBolt->CopyModuleInstanceInfo(pNode);
			pBolt->cfgword=pFatherRod->UnifiedCfgword()&pXieRod->UnifiedCfgword();	//������˨��ĺ����׼������ĺ�һ��
			//��˨λ�����\�������������Ϣ���
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherRod->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			//ָ�������Ǳ���ʱ������Ҫ֫����ƫ�����ͺ�ƫ����
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieRod->handle;
			CTubeEndPosPara* pCurrPos=(pXieSlot!=NULL ? &pXieSlot->desEndPos : &pXieFlat->desEndPos);
			if(pCurrPos->IsInDatumJgWingX())	//���ڸ��Ǹ�X֫
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
			else									//���ڸ��Ǹ�Y֫
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
				if(pXieFlat)		//��������+
					pBolt->des_work_norm.direction=2;
				else if(pXieSlot)	//�۸ֲ�����+
					pBolt->des_work_norm.direction=4;
			}
			else if(pFatherRod->GetClassTypeId()==CLS_LINESLOT||pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
			{
				pBolt->des_work_norm.norm_style=5;	//ָ���Ǹ�֫���߷���
				pBolt->des_work_norm.hVicePart = pFatherRod->handle;
				if(pFatherRod->GetClassTypeId()==CLS_LINEFLAT)
					pBolt->des_work_norm.direction=2;	//��������+
				else 
					pBolt->des_work_norm.direction=4;	//�۸ֲ�����+
			}
			else //if(pFatherRod->IsAngle())
			{
				pBolt->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���߷���
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
				throw "��ѡ���߲�,��ͨ����ǰ�ڵ�";
#endif
			else
				return FALSE;
		}
		pXieRod->SetModified();
		pXieRod->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pXieRod->GetSolidPartObject());
	}
	for(index=1;index<3;index++)
	{	//������˨ �����Ǹ�����ͷ�Լ��Ǹְڷ�λ��
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
				throw "�����˼��ĵ���˨����,Ŀǰֻ֧�ֽǸֵ����!";
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
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg2->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desStartPos.wing_y_offset.gStyle=4;
					pXieJg2->desStartPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg2->desStartPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				pXieJg2->ClearFlag();
				pXieJg2->CalPosition();
			}
			else if(pGroupJg)
			{	//�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg2->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desEndPos.wing_y_offset.gStyle=4;
					pXieJg2->desEndPos.wing_y_offset.offsetDist=-pFatherRod->GetThick();
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg2->desEndPos.wing_y_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				pXieJg2->ClearFlag();
				pXieJg2->CalPosition();
			}
			else if(pGroupJg)
			{	//�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
		{	//���Ǹ�һ��һ�⣬���ҹ���һ����˨���
			//������˨ͨ��
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
		{	//���Ǹ��޹�����˨���
			CLDSBolt *pBolt2=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt2->CopyProperty(pBolt);
			pBolt2->d=pCurrConnBoltInfo->d;
			pBolt2->iSeg=pXieJg2->iSeg;
			//pBolt2->layer(2)=pXieJg2->layer(2);	//������˨ͼ����
			pBolt2->CopyModuleInstanceInfo(pNode);
			pBolt2->cfgword=pFatherRod->UnifiedCfgword();	//������˨��ĺ����׼������ĺ�һ��
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
	theApp.GetLDSDoc()->SetModifiedFlag(); // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	if(bCanUndo)	//֧�ֳ�������
		Ta.EndUndoListen();	//��������
	return TRUE;
}
BOOL DesignBoltOnlyConnect(CLDSNode *pNode,CLDSLinePart *pXieRod,CLDSLinePart *pXieRod2,
								   BOOL bThrowError,BOOL bCanUndo/*=TRUE*/)
{
	return DesignBoltOnlyConnect(pNode,pXieRod,pXieRod2,NULL,bThrowError,bCanUndo);
}

//��Ƶ���˨����
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
		pCmdLine->FillCmdLine("1Bolt ��ѡ��Ҫ��Ƶ���˨���ӵĽڵ�:","");
#endif
		//�л���������ʾ״̬
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
			bRet=DesignIntersNode(pNode,TRUE,&dlg);//��ƽ��浥��˨
		else //if(pNode->m_cPosCalType!=4)
		{	//�ǽ���ڵ㣬	
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please select one ray rod of single-bolt-connection which will be designed:","");
#else 
			pCmdLine->FillCmdLine("ѡ��ǰҪ��Ƶ���˨���ӵ���һ�����߸˼�:","");
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
					continue;	//δѡ��
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
				AfxMessageBox("��ǰ���ӽڵ�ȱ���������Ӹ˼�");
				return FALSE;
			}
			//��׼�Ǹ����׼�ڵ�ĸ��Ǹֿ�ȷ��һ��ƽ��
			PARTSET partset;
			GEPOINT father_line_vec,temp_vec,work_norm;
			//CLDSLineAngle *pFatherAngle=pFatherPart->IsAngle()?pFatherPart.LineAnglePointer():NULL;
			if(pLinePart&&pLinePart->handle!=pNode->hFatherPart&&pFatherPart->IsLinePart()&&
				pLinePart->pStart&&pLinePart->pEnd&&pFatherPart.pRod->pStart&&pFatherPart.pRod->pEnd)	
			{	//��׼�ڵ�ĸ��Ǹַǻ�׼�Ǹ�ʱ���ſ���ִ������Ĵ���	//wht 09-08-07
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
						continue;	//������׼�Ǹ�
					if(pLineRod->pStart!=pNode&&pLineRod->pEnd!=pNode)
						continue;
					if(pLineRod->pStart==NULL||pLineRod->pEnd==NULL)
						continue;
					temp_vec=pLineRod->pEnd->Position(false)-pLineRod->pStart->Position(false);
					normalize(temp_vec);
					if(fabs(temp_vec*work_norm)<EPS2&&fabs(temp_vec*father_line_vec)<EPS_COS)
						partset.append(pLineRod);	//���ֹ���߸˼����׼�˼�ƽ��(ԭΪ����ע�ʹ��룬5.14�ոĴ� wjh-2015.8.29
					//f3dPoint norm=temp_vec^father_line_vec;
					//normalize(norm);
					//if(fabs(work_norm*norm)>EPS_COS)
					//	partset.append(pLineRod);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			if(partset.GetNodeNum()>=1&&AfxMessageBox("Whether to design no plate's single-bolt-connection?",MB_YESNO)==IDYES)
#else 
			if(partset.GetNodeNum()>=1&&AfxMessageBox("�Ƿ�����ް嵥��˨����?",MB_YESNO)==IDYES)
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
			bIgnoreLsNum=TRUE; //���ɵ�һ����˨ʱѡ���˺�����˨��������Ϊ1������,�Գ�ʱҲ��ȡ��Ӧ����
		//�Գ����ɵ���˨����
		if(bRet)
		{
			if(pNode->m_cPosCalType==4)
			{	//���Գƿ�����
				dlg.m_bMirCreate=TRUE;	//�Գ����ɽ������˨ wht 11-01-11
			}
			g_pSolidDraw->Draw();
			static CMirMsgDlg mir_dlg;
			//���ݽڵ�ԳƷ�ʽ��ʼ����˨�ԳƷ�ʽ wht 16-10-09
			//���ݱ�����������˨��ƾ�����Ҫ��ת�Գ�,���������������(������Ľ���),��ʧȥ����ת�Գ�,��ϰ�� wjh-2016.12-17
			if (mir_dlg.mirmsg.axis_flag==0)
			{	//�ۺϿ��������һ��ִ�У�Ӧ�ø��ݵ�ǰ�ڵ�Ĺ����Գ����ͽ��жԳ���Ϣ�ĳ�ʼ�� wjh-2019.9.2
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
				{	//X��Գ�
					src_mirnode[1]=pNode->GetMirNode(mir_dlg.mirmsg.GetSubMirItem(1));
					if(pLinePart)
						src_mirpart[1]=(CLDSLinePart*)pLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					if(pSecLinePart)
						src_mirsecpart[1]=(CLDSLinePart*)pSecLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					if(pNode->m_cPosCalType==4)	//����ڵ�
						DesignIntersNode(src_mirnode[1],TRUE,&dlg);
					//���ֵ��Ǹֶ����ӶԳƺ��ް嵥��˨�Գ�
					else if((pSecLinePart&&src_mirsecpart[1])||(pSecLinePart==NULL&&src_mirsecpart[1]==NULL))
					{
						if(bIgnoreLsNum||ValidateOneBoltConnect(src_mirnode[1],src_mirpart[1],src_mirsecpart[1]))
							DesignBoltOnlyConnect(src_mirnode[1],src_mirpart[1],src_mirsecpart[1],TRUE,TRUE);
					}
				}
				if(mir_dlg.mirmsg.axis_flag&2)
				{	//Y��Գ�
					src_mirnode[2]=pNode->GetMirNode(mir_dlg.mirmsg.GetSubMirItem(2));
					if(pLinePart)
						src_mirpart[2]=(CLDSLinePart*)pLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					if(pSecLinePart)
						src_mirsecpart[2]=(CLDSLinePart*)pSecLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					if(pNode->m_cPosCalType==4)	//����ڵ�
						DesignIntersNode(src_mirnode[2],TRUE,&dlg);
					//���ֵ��Ǹֶ����ӶԳƺ��ް嵥��˨�Գ�
					else if((pSecLinePart&&src_mirsecpart[2])||(pSecLinePart==NULL&&src_mirsecpart[2]==NULL))
					{
						if(bIgnoreLsNum||ValidateOneBoltConnect(src_mirnode[2],src_mirpart[2],src_mirsecpart[2]))
							DesignBoltOnlyConnect(src_mirnode[2],src_mirpart[2],src_mirsecpart[2],TRUE,TRUE);
					}
				}
				if(mir_dlg.mirmsg.axis_flag&4)
				{	//Z��Գ�
					src_mirnode[3]=pNode->GetMirNode(mir_dlg.mirmsg.GetSubMirItem(3));
					if(pLinePart)
						src_mirpart[3]=(CLDSLinePart*)pLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					if(pSecLinePart)
						src_mirsecpart[3]=(CLDSLinePart*)pSecLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					if(pNode->m_cPosCalType==4)	//����ڵ�
						DesignIntersNode(src_mirnode[3],TRUE,&dlg);
					//���ֵ��Ǹֶ����ӶԳƺ��ް嵥��˨�Գ�
					else if((pSecLinePart&&src_mirsecpart[3])||(pSecLinePart==NULL&&src_mirsecpart[3]==NULL))
					{
						if(bIgnoreLsNum||ValidateOneBoltConnect(src_mirnode[3],src_mirpart[3],src_mirsecpart[3]))
							DesignBoltOnlyConnect(src_mirnode[3],src_mirpart[3],src_mirsecpart[3],TRUE,TRUE);
					}
				}
				if(mirmsg.axis_flag&8)
				{	//��ת�Գ�
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
								continue;	//�ǽ������˨
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
							if(pNode->m_cPosCalType==4&&pMirNode)	//�������˨
								DesignIntersNode(pMirNode,TRUE,&dlg);
							//���ֵ��Ǹֶ����ӶԳƺ��ް嵥��˨�Գ�
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
		pCmdLine->FillCmdLine("����:","");
#endif
		return 0;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
	DesignOneBolt();	
	return 0;
}

//������Ƶ���˨����
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
	pProcDlg->SetWindowText("��ƽ���");
#endif
	
	BOOL bIgnoreError=FALSE;
	if(!one_bolt_dlg.m_bCheckUpLsNum)
		bIgnoreError=TRUE;
	//Ta.BeginUndoListen();	//�������ʱ��ʱ��֧�ֳ������� �豣��Ĳ������ܹ��ർ���ڴ治�� wht 09-10-10
	CUndoOperObject undo(&Ta,true);	//���ɳ���undoӦ�ÿ��Ա���undo�洢��Ϣ���������
	for(int i=0;i<n;i++)
	{
		pNode=console.FromNodeHandle(id_arr[i]);
		pProcDlg->Refresh((i*100)/n);
		if(pNode==NULL)
			continue;
		try
		{
			if(pNode->m_cPosCalType==4&&one_bolt_dlg.m_bInterNodeBolt)	//����ڵ�
				DesignIntersNode(pNode,FALSE,FALSE);
			else
			{
				PARTSET jgset;		//�����ڻ�׼�ڵ��ϵĽǸּ���
				PARTSET partset[2];	//partset[0]�����X֫�ϵĽǸּ��ϣ�partset[1]�����Y֫�ϵĽǸּ���
				Ta.Node.push_stack();
				for(CLDSPart *pPart=Ta.Parts.GetFirst(CLS_LINEANGLE);pPart;pPart=Ta.Parts.GetNext(CLS_LINEANGLE))
				{
					CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
					if(pLinePart->pStart==pNode||pLinePart->pEnd==pNode)
						jgset.append(pLinePart);
				}
				Ta.Node.pop_stack();
				//�������ڻ�׼�ڵ��ϵĽǸְ����֫����
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
						//cos(30��)=0.866
						if(fabs(norm*norm_x)>0.866)			//�����X֫�ϵ����߽Ǹ�
							partset[0].append(pLineAngle);
						else if(fabs(norm*norm_y)>0.866)	//�����Y֫�ϵ����߽Ǹ�
							partset[1].append(pLineAngle);
					}
				}	
				int i=0;
				//Ta.BeginUndoListen();	//֧���𲽳���
				for(i=0;i<2;i++)
				{
					if(partset[i].GetNodeNum()==1&&one_bolt_dlg.m_bSingleAngleEndBolt)
					{
						pAngle1 = (CLDSLineAngle*)partset[i][0];
						Ta.Node.push_stack();
						if(pAngle1->pStart->handle==pNode->handle)
						{	//ʼ����˨��Ƹ���Ϊ1����δ������˨
							if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
#endif
									bIgnoreError=TRUE;
							}
							if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)	
								DesignBoltOnlyConnect(pNode,pAngle1,NULL,FALSE,FALSE);
						}
						else if(pAngle1->pEnd->handle==pNode->handle)	
						{	//�ն���˨��Ƹ���Ϊ1����δ������˨
							if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
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
						//��֤�������Ӳ���ͬһƽ����
						if(pAngle1->layer(2)!=pAngle2->layer(2))
							continue;	
						if(pAngle1->pStart->handle==pNode->handle)
						{	//ʼ����˨��Ƹ���Ϊ1����δ������˨
							if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
#endif
									bIgnoreError=TRUE;
							}
							if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)
								DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
						}
						else if(pAngle1->pEnd->handle==pNode->handle)
						{	//�ն���˨��Ƹ���Ϊ1����δ������˨
							if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
							{
#ifdef AFX_TARG_ENU_ENGLISH
								if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
								if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
#endif
									bIgnoreError=TRUE;
							}
							if((bIgnoreError||!bIgnoreError&&pAngle1->connectEnd.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(2,100)==0)
								DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
						}
						/*
						if(pAngle2->pStart->handle==pNode->handle)
						{	//ʼ����˨��Ƹ���Ϊ1����δ������˨
						if(pAngle2->connectStart.wnConnBoltN==1&&pAngle2->GetLocalLsCount(1)==0)
						DesignBoltOnlyConnect(pNode,pAngle2,FALSE,FALSE);
						}
						else if(pAngle2->pEnd->handle==pNode->handle)
						{	//�ն���˨��Ƹ���Ϊ1����δ������˨
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
							{	//ʼ����˨��Ƹ���Ϊ1����δ������˨
								if(pAngle->connectStart.wnConnBoltN!=1&&!bIgnoreError)
								{
	#ifdef AFX_TARG_ENU_ENGLISH
									if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
	#else 
									if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
	#endif
										bIgnoreError=TRUE;
								}
								if((bIgnoreError||!bIgnoreError&&pAngle->connectStart.wnConnBoltN==1)&&pAngle->GetLocalLsCount(1)==0)	
									DesignBoltOnlyConnect(pNode,pAngle,NULL,FALSE,FALSE);
							}
							else if(pAngle->pEnd->handle==pNode->handle)	
							{	//�ն���˨��Ƹ���Ϊ1����δ������˨
								if(pAngle->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
								{
	#ifdef AFX_TARG_ENU_ENGLISH
									if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
	#else 
									if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
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
				//Ta.EndUndoListen();	//֧���𲽳���
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

//������׼�϶������˨����ƫ���� 
//ʹ�øú���֮ǰӦ��֤��˨��λ���Լ����߲����Ѹ�ֵ	wht 10-07-22
int CalEndLjFirstBoltOffsetDist(CLDSBolt *pFirstBolt,CLDSLineAngle *pDatumLineAngle,CLDSLineAngle *pCrossAngle,int nFirstLsJgSpace=0)
{
	if(pFirstBolt==NULL||pDatumLineAngle==NULL)
		return 0;
	if(nFirstLsJgSpace==0)
	{	//������˨ֱ���õ���׼�϶
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
	if(cosa>0)	//��˨λ��֫���
		dd=pos.y-pCrossAngle->GetWidth();
	else		//��˨λ�����߲�
		dd=-pos.y;
	double len_offset=0;
	if(dd<nFirstLsJgSpace)	//Ŀǰ��϶С����׼�϶
	{
		double add_offset=(nFirstLsJgSpace-dd)/fabs(cosa);
		double round_offset=fto_halfi(add_offset/10)*10;	//Բ���������ֵ
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
	return abs((int)len_offset); //���ص�һ�������˨��Ǹ����߽���֮��ľ���
}

/* ����ܴ���Ŀǰ����ȫ��������˨��ƫ����˨����ȡ��,��ʹ�ù����н�һ��ȷ�Ϻ�ɾ����wjh-2015.3.8
//ƫ��������˨
void CLDSView::OnDefOffsetLs()
{
	m_nPrevCommandID=ID_DEF_OFFSET_LS;
	m_sPrevCommandName="�ظ�����ƫ����˨";
	Command("OffsetBolt");
}
int CLDSView::DefOffsetLs() 
{
	CXhChar100 sText;//[100]="";
	int len_offset=0;
	CString cmdStr;
	//ѡ�е����߽Ǹ�
	CLDSLineAngle *pSelectLineAngle=NULL;
	//��ʱʹ�õı���
	CLsRef *pLsRef=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSBolt *pDatumBolt=NULL;
	CLDSBolt *pNewBolt=NULL;
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_BOLT),CLS_BOLT);
	scr_part_para.disp_type=DISP_SOLID;
	scr_part_para.cmdStr="DefOffsetLs ��ѡ��ƫ�ƻ�׼��˨:";
	scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�����������ѡ���׼��˨:";
	if(!GetPartsFromScr(scr_part_para))
		return 0;
	CLDSDbObject *pObj=scr_part_para.pResultObjsArr[0];
	if(pObj&&pObj->GetClassTypeId()==CLS_BOLT)
		pDatumBolt=(CLDSBolt*)pObj;
	if(pDatumBolt==NULL)
		return 0;
	//ѡ����˨���ڵĽǸ�
	g_pSolidDraw->SetEntSnapStatus(pDatumBolt->des_base_pos.hPart,TRUE);
	g_pSolidDraw->Draw();
	CLDSPlate *pDatumPlate=NULL;
	CLDSLineAngle *pDatumLineAngle = (CLDSLineAngle*)console.FromPartHandle(pDatumBolt->des_base_pos.hPart,CLS_LINEANGLE);
	for(pDatumPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pDatumPlate;pDatumPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if(pDatumPlate->FindLsByHandle(pDatumBolt->handle))
			break;
	}
	//ѡ����˨���ڵĻ�׼�ְ�
	if(pDatumPlate)
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle,TRUE);
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,pDatumBolt->get_d());
	//����ƫ����˨
	Ta.BeginUndoListen();
	pNewBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT,TRUE);
	pDatumBolt->CloneTo(*pNewBolt);
	//��׼��˨��λ��ʽΪ�˼��ڵ㶨λ ��ѡ�����߽Ǹ�����Ҫ�������߽�����˨
	if((pDatumBolt->des_base_pos.datumPoint.datum_pos_style==2||pDatumBolt->des_base_pos.datumPoint.datum_pos_style==3)
		&&pDatumBolt->des_base_pos.len_offset_dist==0&&pDatumBolt->des_base_pos.wing_offset_dist==0)
	{	//ѡ�����ڻ�׼λ�õ���˨�������������߽�����˨
		Sleep(500);//������ʾ������,��Ȼ�û���֪���Ƿ�ѡ���˴˹�����ʾ״̬�ͱ������
		pCmdLine->FillCmdLine("��ѡ����Ҫ�����˨�����߽Ǹ�<�ɲ�ѡ,Ĭ���ڻ�׼�Ǹ��ϲ�����˨>:","");
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
						{	//��ѡ���߸˼����ڻ�׼�ְ����ӹ����б���
							if(pDatumPlate)
							{
								if(pDatumPlate->designInfo.FromHandle(pPart->handle))
									pSelectLineAngle=(CLDSLineAngle*)pPart;
								else
								{
									pCmdLine->FillCmdLine("��ѡ���߽Ǹ�δ�����ڻ�׼�ְ��ϣ�������ѡ��<�ɲ�ѡ,Ĭ���ڻ�׼�Ǹ��ϲ�����˨>:","");
									g_pSolidDraw->ReleaseSnapStatus();
									continue;
								}
							}
							else
							{
								pSelectLineAngle=(CLDSLineAngle*)pPart;
								if(!pSelectLineAngle->FindLsByHandle(pDatumBolt->handle))
								{
									pCmdLine->FillCmdLine("��׼��˨������ѡ���߽Ǹ��ϣ�������ѡ��<�ɲ�ѡ,Ĭ���ڻ�׼�Ǹ��ϲ�����˨>:","");
									g_pSolidDraw->ReleaseSnapStatus();
									continue;
								}
							}
						}	
						else
						{
							pCmdLine->FillCmdLine("û��ѡ�к��ʵ����߽Ǹ֣�������ѡ��<�ɲ�ѡ,Ĭ���ڻ�׼�Ǹ��ϲ�����˨>:","");
							g_pSolidDraw->ReleaseSnapStatus();
							continue;
						}
						break;
					}
				}
				//δѡ�����߽Ǹ�,Ĭ���ڻ�׼�Ǹ��ϲ�����˨
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
		{	//ѡ����˨���ڵĽǸ�
			g_pSolidDraw->SetEntSnapStatus(pDatumBolt->des_base_pos.hPart,TRUE);
			//ѡ����˨���ڵĻ�׼�ְ�
			if(pDatumPlate)
				g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle,TRUE);
		}
		else if(pSelectLineAngle&&pDatumLineAngle&&pDatumPlate)
		{	//ѡ�������߽Ǹֿ��������߽�����˨
			g_pSolidDraw->Draw();
			pCmdLine->FillCmdLine(cmdStr,"");
			pNewBolt->des_base_pos.datumPoint.datum_pos_style=3;//�Ǹ����߽���
			pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=pDatumLineAngle->handle;
			pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pSelectLineAngle->handle;
			pNewBolt->des_base_pos.hPart=pSelectLineAngle->handle;
			//�ж����߸˼�Ϊʼ�����ӻ����ն�����
			f3dLine datum_line;
			datum_line.startPt=pDatumLineAngle->pStart->Position(true);
			datum_line.endPt=pDatumLineAngle->pEnd->Position(true);
			int start0_end1=0;	//ʼ������ ʼ��==>�ն�
			if(datum_line.PtInLine(pSelectLineAngle->pEnd->Position(true))>0)
				start0_end1=1;	//�ն����� �ն�==>ʼ��
			pNewBolt->des_base_pos.direction=start0_end1;
			
			int nOffsetStyle=0;
			//g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pSelectLineAngle->handle,TRUE);
			cmdStr.Format("���������߽Ǹ�֫��ƫ�ƾ���[0Ϊg,1Ϊg1,2Ϊg2,3Ϊg3,��ֱ���������]<%d>",0);
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
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;//�Զ���ƫ�ƾ���
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=nOffsetStyle;
			}
			else
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=nOffsetStyle;
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pDatumLineAngle->handle,TRUE);
			cmdStr.Format("�������׼�Ǹ�֫��ƫ�ƾ���[0Ϊg,1Ϊg1,2Ϊg2,3Ϊg3,��ֱ���������]<%d>:",0);
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
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;//�Զ���ƫ�ƾ���
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=nOffsetStyle;
			}
			else
				pNewBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=nOffsetStyle;
			BOOL bEndLj=TRUE;
			cmdStr.Format("�����߽Ǹ��Ƿ�[Y/N]��Ҫ������<Y>:");
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
						cmdStr.Format("�������������ѡ��,�����߽Ǹ��Ƿ�[Y/N]��Ҫ������<Y>:");
						continue;
					}
				}
				break;
			}
			if(bEndLj&&pDatumPlate)
			{	//��ѡ���߸˼���Ҫ������
				CDesignLjPartPara *pLjPartPara=pDatumPlate->designInfo.FromHandle(pSelectLineAngle->handle);
				if(pLjPartPara)
				{
					if(pLjPartPara->start0_end1==0)
					{
						pSelectLineAngle->desStartOdd.m_iOddCalStyle=1;//����˨��������ͷ
						pSelectLineAngle->CalStartOddment();
						pSelectLineAngle->AppendStartLsRef(pDatumBolt->GetLsRef());
					}
					else if(pLjPartPara->start0_end1==1)
					{
						pSelectLineAngle->desEndOdd.m_iOddCalStyle=1;	//����˨��������ͷ
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
					if(ray_wing_norm*datum_wing_norm>0)	//����
					{	//��Ҫ�������������߽Ǹ�ƫ�ƾ��� �����˨��� ������˨����ƫ���� �����н���Ϣ
						pDatumBolt->des_base_pos.norm_offset.AddThick(-pSelectLineAngle->handle,TRUE,TRUE);
						pNewBolt->des_base_pos.norm_offset.AddThick(-pSelectLineAngle->handle,TRUE,TRUE);
						CMultiOffsetPos *pDesPosPara=NULL;
						if(pLjPartPara->start0_end1==0)
							pDesPosPara=&pSelectLineAngle->desStartPos;
						else if(pLjPartPara->start0_end1==1)
							pDesPosPara=&pSelectLineAngle->desEndPos;
						if(pDesPosPara&&pDatumLineAngle)
						{	//�������������߽Ǹ�ƫ�Ʒ���
							if(pLjPartPara->angle.cur_wing_x0_y1==0)
							{	//��ǰ����֫ΪX֫
								pDesPosPara->wing_x_offset.gStyle=4;
								pDesPosPara->wing_x_offset.offsetDist=-pDatumLineAngle->GetThick();
							}
							else
							{	//��ǰ����֫ΪY֫
								pDesPosPara->wing_y_offset.gStyle=4;
								pDesPosPara->wing_y_offset.offsetDist=-pDatumLineAngle->GetThick();
							}
						}
					}
					else //����
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
					//���ڶ��������߸˼�ʱӦ������׼�˼��ϵ���˨���㶨λ��ʽΪ�Ǹ����߽���
					for(pLsRef=pDatumLineAngle->GetFirstLsRef();pLsRef;pLsRef=pDatumLineAngle->GetNextLsRef())
					{
						if(!pDatumPlate->FindLsByHandle((*pLsRef)->handle))
							continue;
						(*pLsRef)->des_base_pos.datumPoint.datum_pos_style=3;//�Ǹ����߽���
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
					//����Ĭ�ϵ���С��׼�϶���������Լ����������߽Ǹ���˨λ��
					int adjust_len_offset=0;
					if(pDatumPlate)
					{	
						BOLTSET boltSet[2];	//boltSet[0]���ƫ��ֵΪ������˨ boltSet[1]���ƫ��ֵΪ������˨
						CLDSBolt *pBolt=NULL;
						for(pLsRef=pDatumLineAngle->GetFirstLsRef();pLsRef;pLsRef=pDatumLineAngle->GetNextLsRef())
						{
							if(!pDatumPlate->FindLsByHandle((*pLsRef)->handle))	
								continue;	//�������ڵ�ǰ�ְ��ϵ���˨
							if((*pLsRef)->des_base_pos.datumPoint.datum_pos_style!=3)
								continue;	//��˨���㶨λ��ʽ�ǽǸ����߽������˨���õ���λ��
							//�����������߽Ǹ��϶�λ��ʽΪ�Ǹ����߽������˨����˨����ƫ������С��������
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
							if(i==1)//boltSet[0]���ƫ��ֵΪ������˨ boltSet[1]���ƫ��ֵΪ������˨
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
	cmdStr.Format("��������˨�ĺ���ƫ��ֵ<%d>:",0);
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
		cmdStr.Format("��������˨������ƫ��ֵ<%d>:",len_offset);
		nBoltSpace=len_offset;
	}
	else
	{
		if(pDatumBolt->des_base_pos.len_offset_dist>0)
		{
			cmdStr.Format("��������˨������ƫ��ֵ<%d>:",LsSpace.SingleRowSpace);
			nBoltSpace=LsSpace.SingleRowSpace;
		}
		else
		{
			cmdStr.Format("��������˨������ƫ��ֵ<%d>:",-LsSpace.SingleRowSpace);
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
	//���������߽Ǹ�����˨ʹ�ôӻ�׼�㿪ʼƫ����Сƫ�ƾ���,������˨����ѡ��˨λ�ÿ�ʼƫ��
	if(pSelectLineAngle&&pNewBolt->des_base_pos.datumPoint.datum_pos_style==3)
		pNewBolt->des_base_pos.len_offset_dist=nBoltSpace;
	else
		pNewBolt->des_base_pos.len_offset_dist+=nBoltSpace;
	if(pSelectLineAngle&&pNewBolt->des_base_pos.hPart==pDatumLineAngle->handle)
	{	//������˨��λ��ʽ wht 10-09-18
		pNewBolt->des_base_pos.hPart=pSelectLineAngle->handle;
		pNewBolt->des_base_pos.datumPoint.datum_pos_style=3;	//�Ǹ����߽���
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
	//����˨���뵽��׼�Ǹ�
	if(pSelectLineAngle)
	{	
		if(pDatumPlate)
		{	//��������˨ҲҪ�޸Ķ�λ��ʽ
			double nPrevLsSpace=pNewBolt->des_base_pos.len_offset_dist;
			LSSPACE_STRU LsSpace;
			for(pLsRef=pSelectLineAngle->GetFirstLsRef();pLsRef;pLsRef=pSelectLineAngle->GetNextLsRef())
			{
				if(!pDatumPlate->FindLsByHandle((*pLsRef)->handle))
					continue;
				if((*pLsRef)->handle == pDatumBolt->handle)
					continue;
				(*pLsRef)->des_base_pos.datumPoint.datum_pos_style=3;//�Ǹ����߽���
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
			//�Ǹ����߽��㶨λ
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
		{	//�Ǹ���㶨λ
			if(pNewBolt->des_base_pos.direction==0)
				pDatumLineAngle->AppendStartLsRef(pNewBolt->GetLsRef());
			else if(pNewBolt->des_base_pos.direction==1)
				pDatumLineAngle->AppendEndLsRef(pNewBolt->GetLsRef());
		} 
		else //�˼��ϵĽڵ㶨λ��Ǹ����߽��㶨λ
			pDatumLineAngle->AppendMidLsRef(pNewBolt->GetLsRef());
	}
	//����˨���뵽��׼��˨���ڸְ岢������Ƹְ�����
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
	pCmdLine->FillCmdLine("����","");
	DefOffsetLs();
	return 0;
}
*/
//--------------VVV---OldCommand----VVV-----------------
void CLDSView::OnSingleXieNodeDesign() 
{
	Command("1BoltEndAngle");
}

//�����ѡ�еĽ������˨
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
	pCmdLine->FillCmdLine("ѡ��Ҫ�����˨�Ľ���ڵ�:","");
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
	//��ֹ�û������
#ifdef AFX_TARG_ENU_ENGLISH
	if(n>0&&MessageBox( "Are you sure to design selected cross bolts��",NULL,MB_YESNO)!=IDYES)
#else 
	if(n>0&&MessageBox( "���Ҫ�����ѡ�еĽ������˨��",NULL,MB_YESNO)!=IDYES)
#endif
		return;
	
	CProcBarDlg *pProcDlg = new CProcBarDlg(this);
	pProcDlg->Create();	//����������
#ifdef AFX_TARG_ENU_ENGLISH
	pProcDlg->SetWindowText("Design process");
#else 
	pProcDlg->SetWindowText("��ƽ���");
#endif
	BeginWaitCursor();
	N=nodeset.GetNodeNum();
	for(pNode=nodeset.GetFirst(),i=0;pNode;pNode=nodeset.GetNext(),i++)
	{
		g_pSolidDraw->SetEntSnapStatus(pNode->handle);
		DesignIntersNode(pNode,FALSE,FALSE);
		g_pSolidDraw->SetEntSnapStatus(pNode->handle,FALSE);
		pProcDlg->Refresh((i*100+1)/N);	//���½�����
	}
	pProcDlg->DestroyWindow();	//���ٽ�����
	EndWaitCursor();
	
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("The design of cross bolts is completed!");
#else 
	MessageBox("ѡ�еĽ������˨������!");
#endif
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
}
//��Ƶ���˨����
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
		pCmdLine->FillCmdLine("1BoltEndAngle ѡ��ǰҪ��Ƶ���˨�����ӵĽǸ�:","");
#endif
		//�л���������ʾ״̬
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
		pCmdLine->FillCmdLine("��ѡ��Ǹֵ�ǰ��ƶ˵Ķ˽ڵ�:","");
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
		//��׼�Ǹ����׼�ڵ�ĸ��Ǹֿ�ȷ��һ��ƽ��
		CLDSLineAngle *pFatherAngle=NULL;
		if(pNode==pAngle->pStart&&pAngle->desStartPos.datum_jg_h>0x20)
			pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->desStartPos.datum_jg_h,CLS_LINEANGLE);
		else if(pNode==pAngle->pEnd&&pAngle->desEndPos.datum_jg_h>0x20)
			pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pAngle->desEndPos.datum_jg_h,CLS_LINEANGLE);
		else
			pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pNode->hFatherPart,CLS_LINEANGLE);
		if(pAngle->handle!=pNode->hFatherPart&&pFatherAngle&&
			pAngle->pStart&&pAngle->pEnd&&pFatherAngle->pStart&&pFatherAngle->pEnd)	
		{	//��׼�ڵ�ĸ��Ǹַǻ�׼�Ǹ�ʱ���ſ���ִ������Ĵ���	//wht 09-08-07
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
					continue;	//������׼�Ǹ�
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
		if(partset.GetNodeNum()==1&&AfxMessageBox("�Ƿ�����ް嵥��˨����?",MB_YESNO)==IDYES)
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
		pCmdLine->FillCmdLine("����:","");
#endif
		return 0;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("����:","");
#endif
	Command("1BoltEndAngle");
	return 0;
}
//������е����Ķ˵㵥��˨����
void CLDSView::OnAllSingleXieNodeDesign() 
{
	CLDSNode *pNode;
	CLDSLineAngle *pAngle1=NULL,*pAngle2=NULL;
	long *id_arr=NULL;
	long n=g_pSolidSnap->GetLastSelectEnts(id_arr);
#ifdef AFX_TARG_ENU_ENGLISH
	if(MessageBox("Are you sure to design selected single-bolt-connection?",NULL,MB_YESNO)!=IDYES)
#else 
	if(MessageBox("�����Ҫ�����ѡ�нǸֵĵ���˨��������?",NULL,MB_YESNO)!=IDYES)
#endif
		return;
	CProcBarDlg *pProcDlg = new CProcBarDlg(this);
	pProcDlg->Create();
#ifdef AFX_TARG_ENU_ENGLISH
	pProcDlg->SetWindowText("Design process");
#else 
	pProcDlg->SetWindowText("��ƽ���");
#endif
	
	BOOL bIgnoreError=FALSE;
	for(int i=0;i<n;i++)
	{
		pNode=console.FromNodeHandle(id_arr[i]);
		pProcDlg->Refresh((i*100)/n);
		if(pNode==NULL)
			continue;
		try{
			PARTSET jgset;		//�����ڻ�׼�ڵ��ϵĽǸּ���
			PARTSET partset[2];	//partset[0]�����X֫�ϵĽǸּ��ϣ�partset[1]�����Y֫�ϵĽǸּ���
			Ta.Node.push_stack();
			for(CLDSPart *pPart=Ta.Parts.GetFirst(CLS_LINEANGLE);pPart;pPart=Ta.Parts.GetNext(CLS_LINEANGLE))
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				if(pLinePart->pStart==pNode||pLinePart->pEnd==pNode)
					jgset.append(pLinePart);
			}
			Ta.Node.pop_stack();
			//�������ڻ�׼�ڵ��ϵĽǸְ����֫����
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
					//cos(30��)=0.866
					double a=norm*norm_x;
					double b=norm*norm_y;
					double c=norm_x*norm_y;
					if(fabs(norm*norm_x)>0.866)			//�����X֫�ϵ����߽Ǹ�
						partset[0].append(pLineAngle);
					else if(fabs(norm*norm_y)>0.866)	//�����Y֫�ϵ����߽Ǹ�
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
					{	//ʼ����˨��Ƹ���Ϊ1����δ������˨
						if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
#endif
								bIgnoreError=TRUE;
						}
						if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)	
							DesignBoltOnlyConnect(pNode,pAngle1,NULL,FALSE,FALSE);
					}
					else if(pAngle1->pEnd->handle==pNode->handle)	
					{	//�ն���˨��Ƹ���Ϊ1����δ������˨
						if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
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
					//��֤�������Ӳ���ͬһƽ����
					if(pAngle1->layer(2)!=pAngle2->layer(2))
						continue;	
					if(pAngle1->pStart->handle==pNode->handle)
					{	//ʼ����˨��Ƹ���Ϊ1����δ������˨
						if(pAngle1->connectStart.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
#endif
								bIgnoreError=TRUE;
						}
						if((bIgnoreError||!bIgnoreError&&pAngle1->connectStart.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(1,100)==0)
							DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
					}
					else if(pAngle1->pEnd->handle==pNode->handle)
					{	//�ն���˨��Ƹ���Ϊ1����δ������˨
						if(pAngle1->connectEnd.wnConnBoltN!=1&&!bIgnoreError)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							if(AfxMessageBox("Whether to ignore the error that all ray angle's number isnt't 1?",MB_YESNO)==IDYES)
#else 
							if(AfxMessageBox("�Ƿ�����������߽Ǹֶ���˨����Ϊ1�Ĵ���?",MB_YESNO)==IDYES)
#endif
								bIgnoreError=TRUE;
						}
						if((bIgnoreError||!bIgnoreError&&pAngle1->connectEnd.wnConnBoltN==1)&&pAngle1->GetLocalLsCount(2,100)==0)
							DesignBoltOnlyConnect(pNode,pAngle1,pAngle2,FALSE,FALSE);
					}
					/*
					if(pAngle2->pStart->handle==pNode->handle)
					{	//ʼ����˨��Ƹ���Ϊ1����δ������˨
					if(pAngle2->connectStart.wnConnBoltN==1&&pAngle2->GetLocalLsCount(1)==0)
					DesignBoltOnlyConnect(pNode,pAngle2,FALSE,FALSE);
					}
					else if(pAngle2->pEnd->handle==pNode->handle)
					{	//�ն���˨��Ƹ���Ϊ1����δ������˨
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
/* ��Ц�ػ�δ��ɣ���ʱ�����á�wjh-2015.4.12
void CLDSView::OnMirCreateWBConnect()
{
	//m_nPrevCommandID=ID_MIR_CREATE_WB_CONNECT;
	//m_sPrevCommandName="�ظ��Գ������ް�����";
	//Command("MirCreateWBConnect");
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_POINT);
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();

	CString cmdStr="";
	char sText[MAX_TEMP_CHAR_100+1];
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	if(!LockFunc())
		return;
	CUndoOperObject undo(&Ta,true);
	try{
		//1.�л�������ͼ״̬����ѡ��ڵ�
		g_pSolidSet->SetDisplayType(DISP_LINE);
		g_pSolidDraw->Draw();
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FillCmdLine("��ѡ��Ҫ�ԳƵ��ް����ӵĶ�λ��׼�ڵ�:","");
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

		//�����
		CLDSPlate * pVirtualPlate;
		pVirtualPlate = (CLDSPlate*)console.AppendPart(CLS_PLATE);
		pVirtualPlate->cfgword=pBaseNode->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pVirtualPlate->designInfo.m_hBaseNode = pBaseNode->handle;

		//��׼�˼�
		CSuperSmartPtr<CLDSLinePart> pBasePart;
		if(pBaseNode->m_cPosCalType!=4)
		{	//�ǽ���ڵ�
			pBasePart=(CLDSLinePart*)console.FromPartHandle(pBaseNode->hFatherPart,CLS_LINEPART);
			if(pBasePart.IsHasPtr())
				pVirtualPlate->designInfo.m_hBasePart = pBasePart->handle;
			else
				throw "δ�ҵ��Ϸ��Ľڵ㸸�˼�!";
		}
		if(pBasePart)
			pVirtualPlate->designInfo.m_hBasePart = pBasePart->handle;
		//�ް����Ӿ�Ϊ�����
		pVirtualPlate->face_N = 1;
		//designJdb.SetViewFlag(m_eViewFlag);
		if(pBaseNode->m_cPosCalType!=4)
		{	//��ͨ���Ӱ�
			pVirtualPlate->jdb_style = 0;
			//if(!designJdb.DesignCommonPlank(pCurPlate))
				//throw "���ʧ��";
		}
		else //if(iPlateFaceType==4)//��Ƶ��ǽ����(��Ҫ���⴦��)
		{
			pVirtualPlate->jdb_style = 1;
			//if(!designJdb.DesignCrossPlate(pCurPlate))
				//throw "���ʧ��";
		}

		//2.��ѡ���ӽǸ�
		CSuperSmartPtr<CLDSPart>pPart;
		CLDSLinePart* pSelLinePart;
		LINEPARTSET partset;
		CHashTable<CLDSLinePart*>partsetTable;
		partsetTable.CreateHashTable(100);
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		pCmdLine->FillCmdLine("��ѡ��ýڵ������������ӵ����и˼�:","");
		g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_LINEANGLE)|GetSingleWord(SELECTINDEX_LINETUBE)|GetSingleWord(SELECTINDEX_LINEFLAT));
		((CMainFrame*)AfxGetMainWnd())->GetLDSView()->OpenWndSel();
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			//���ݾ���ַ�����ӹ���
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
			throw "����Ӧ�������˼��������������";
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
					desLjPartPara.angle.cur_wing_x0_y1=0;//����Ӧ��������˼��������������wjh-2015.1.05
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
				{	//��ֹδ������˨�ȼ������߸˼������
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
		pCmdLine->FillCmdLine("����:","");
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