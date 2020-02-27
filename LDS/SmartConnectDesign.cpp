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
//1.��ƽ���㵥��˨ 
BOOL DesignIntersNode(CLDSNode *pNode,BOOL bThrowError,CDianBanParaDlg *pDlg=NULL);
//2.��ƽ����˫��˨
BOOL DesignIntersNode2Ls(CLDSNode *pNode, BOOL bThrowError);
//��ƽ�����˨������(�ް�)
BOOL SmartDesignBoltOnlyConnect(CLDSNode *pNode,BOOL bThrowError,BOOL bCanUndo/*=TRUE*/)
{
	CLDSLineAngle* pXieJg=NULL,*pXieJg2=NULL;
	if (pNode->m_cPosCalType==CLDSNode::COORD_INTERSECTION)
	{
		pXieJg = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[0],CLS_LINEANGLE);
		pXieJg2 = (CLDSLineAngle*)console.FromPartHandle(pNode->arrRelationPole[1],CLS_LINEANGLE);
		if (pXieJg == NULL || pXieJg2 == NULL)
			return FALSE;
		//�����Ǹִ��Ľ����
		f3dPoint view_norm;
		GetWorkNorm(pXieJg,pXieJg2,&view_norm);
		BYTE ciWorkWing1 = 0,ciWorkWing2 = 1;
		BOOL bInsideJg1 = IsInsideJg(pXieJg,view_norm,&ciWorkWing1);
		BOOL bInsideJg2 = IsInsideJg(pXieJg2,view_norm,&ciWorkWing2);
		if (bInsideJg1 != bInsideJg2)
		{	//������Ǹ�һ��һ�⣬������˨����
			LOCALE_PROPITEM* pLocaleItem=gxLocalizer.GetLocaleItem("2IntersBolt-MinAnlgeWidth");
			UINT uiMinWidth=pLocaleItem!=NULL?pLocaleItem->liVal:10000;
			BOOL designed=false;
			if (pXieJg->GetWidth() >= uiMinWidth && pXieJg2->GetWidth() >= uiMinWidth)
				designed=DesignIntersNode2Ls(pNode,bThrowError);	//���˫��˨
			if (designed)
				return true;
			else
				return DesignIntersNode(pNode,bThrowError);	//��Ƶ���˨
		}
		else if (bInsideJg1 == bInsideJg2)
		{	//������Ǹ�ͬ���ͬ�⣬���д�ϴ���
			//TODO:��ʱ��֧�� wjh-2019.10.29
			return FALSE;
		}
	}
	//���´��뾭��֤ԭ�����������������������������Ǹֵ���˨��Ƶ�Ԫ�أ�����������ȫ����DesignBoltOnlyConnect�н��У�
	//�ʺ���������ʱ���ã���ʹ����ҲӦ���ǵ���DesignBoltOnlyConnect(...)@PartDesign.cpp ������ wjh-2019.10.29
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
			throw "ѡ��Ƿ���������";
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
			throw "û�д˽ڵ���޸�Ȩ�ޣ�";
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
				throw "ѡ��Ƿ���������";
#endif
			else
				return FALSE;
		}
		if(pRayJgArr[i]->pStart==NULL||pRayJgArr[i]->pEnd==NULL)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "the selected angle is short(starting or ending node is 0X0)��";
#else
				throw "��ѡ�Ǹ�Ϊ�̽Ǹ�(ʼ�˻��ն˽ڵ�Ϊ0X0)��";
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
				throw "û�д˽Ǹ�ʼ�˵��޸�Ȩ�ޣ�";
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
				throw "û�д˽Ǹ��ն˵��޸�Ȩ�ޣ�";
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
			throw "ѡ����һ�������Ľڵ�(û�и��ǸֵĽڵ�)";
#endif
		else
			return FALSE;
	}
	CLDSGroupLineAngle *pGroupJg=NULL;	//�ڵ㸸�Ǹ�
	if(pFatherAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//�ڵ㸸�˼�Ϊ��ϽǸ�ʱ�Զ�ѡ����ʵ��ӽǸ�Ϊ����˨���ӻ�׼�Ǹ� wht 10-11-23
		pGroupJg=(CLDSGroupLineAngle*)pFatherAngle;
		if(pGroupJg->group_style!=0)
		{
			if(bThrowError)
#ifdef AFX_TARG_ENU_ENGLISH
				throw "System only supports single bolt connection on child angle of diagonal group angles currently!";
#else
				throw "Ŀǰ��֧���ڶԽ���ϽǸֵ��ӽǸ��Ͻ��е���˨����!";
#endif
			else
				return FALSE;
		}
		f3dPoint ray_vec=pXieJg->Start()-pXieJg->End();
		if(pXieJg->pStart->handle==pNode->handle)
			ray_vec*=-1.0;
		normalize(ray_vec);
		CLDSLineAngle *pSonJg=NULL;
		GetWorkNorm(pGroupJg,pXieJg,&norm);	//�����淨��
		for(int i=0;i<4;i++)
		{
			if(pGroupJg->group_style<2&&i>=2)
				continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
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
				throw "���㸸�˼���֧�ָ�����!";
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
	if(bCanUndo)	//֧�ֳ�������
		Ta.BeginUndoListen();	//��ʼ����
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
				if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
					pXieJg->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
			}
			else
			{
				pXieJg->desStartPos.wing_y_offset.gStyle=4;
				pXieJg->desStartPos.wing_y_offset.offsetDist=-base_wing_thick;
				if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherAngle->GetThick());
			ls.des_base_pos.norm_offset.AddThick(-pFatherAngle->handle,TRUE);
		}
		ls.set_d(pXieJg->connectStart.d);
		//���������˨ͨ���λ�� wht 10-11-04
		ls.AddL0Thick(pFatherAngle->handle,TRUE);
		ls.AddL0Thick(pXieJg->handle,TRUE);
		if(!ls.CalGuigeAuto())
		{
			char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"Fail to find bolt size according with M%dX%.f in bolt size library",pXieJg->connectStart.d,ls.get_L0());
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
		//pBolt->AddL0Thick(pFatherAngle->handle,TRUE);
		//pBolt->AddL0Thick(pXieJg->handle,TRUE);
		pBolt->iSeg=pXieJg->iSeg;
		pBolt->layer(2)=pXieJg->layer(2);	//������˨ͼ����
		pBolt->cfgword=pFatherAngle->cfgword;	//������˨��ĺ����׼������ĺ�һ��
		//��˨λ�����\�������������Ϣ���
		pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
		pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherAngle->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
		pBolt->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���߷���
		pBolt->des_work_norm.hVicePart = pFatherAngle->handle;
		if(pXieJg->desStartPos.IsInDatumJgWingX())	//���ڸ��Ǹ�X֫
		{
			pBolt->des_work_norm.norm_wing=0;
			pBolt->des_work_norm.direction=0;
			pBolt->des_base_pos.len_offset_dist=0;
			pBolt->des_base_pos.wing_offset_dist = 0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherAngle->GetNodeWingXG(pNode));
		}
		else									//���ڸ��Ǹ�Y֫
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
				if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
					pXieJg->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
			}
			else
			{
				pXieJg->desEndPos.wing_y_offset.gStyle=4;
				pXieJg->desEndPos.wing_y_offset.offsetDist=-base_wing_thick;
				if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
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
			Sub_Pnt(ls_pos,ls_pos,ls.get_norm()*pFatherAngle->GetThick());
			ls.des_base_pos.norm_offset.AddThick(-pFatherAngle->handle,TRUE);
		}
		ls.EmptyL0DesignPara();	//�����˨ͨ����Ʋ���
		ls.AddL0Thick(pFatherAngle->handle,TRUE);
		ls.AddL0Thick(pXieJg->handle,TRUE);
		ls.set_d(pXieJg->connectEnd.d);
		if(!ls.CalGuigeAuto())
		{
			char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"Fail to find bolt size according with M%dX%.f in bolt size library",pXieJg->connectEnd.d,ls.get_L0());
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
		pBolt->cfgword=pFatherAngle->cfgword;	//������˨��ĺ����׼������ĺ�һ��
		//�����˨ͨ����Ʋ���
		pBolt->AddL0Thick(pFatherAngle->handle,TRUE,TRUE);
		pBolt->AddL0Thick(pXieJg->handle,TRUE,TRUE);
		pBolt->iSeg=pXieJg->iSeg;
		pBolt->layer(2)=pXieJg->layer(2);	//������˨ͼ����
		//��˨λ�����\�������������Ϣ���
		pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
		pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pFatherAngle->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pXieJg->handle;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
		pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
		pBolt->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���߷���
		pBolt->des_work_norm.hVicePart = pFatherAngle->handle;
		if(pXieJg->desEndPos.IsInDatumJgWingX())	//���ڸ��Ǹ�X֫
		{
			pBolt->des_work_norm.norm_wing=0;
			pBolt->des_work_norm.direction=0;
			pBolt->des_base_pos.len_offset_dist=0;
			pBolt->des_base_pos.wing_offset_dist = 0;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pFatherAngle->GetNodeWingXG(pNode));
		}
		else									//���ڸ��Ǹ�Y֫
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
			throw "��ѡ���߲�,��ͨ����ǰ�ڵ�";
#endif
		else
			return FALSE;
	}
	pXieJg->SetModified();
	pXieJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pXieJg->GetSolidPartObject());

	if(pXieJg2)
	{	//������˨ �����Ǹ�����ͷ�Լ��Ǹְڷ�λ��
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
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg2->desStartPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desStartPos.wing_y_offset.gStyle=4;
					pXieJg2->desStartPos.wing_y_offset.offsetDist=-base_wing_thick;
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
					if(pGroupJg) //�ڵ㸸�Ǹ�Ϊ��ϽǸ� ����б��ƫ����ʱ������ϽǸּ�϶ֵ wht 10-11-23
						pXieJg2->desEndPos.wing_x_offset.offsetDist-=(pGroupJg->jg_space*0.5);
				}
				else
				{
					pXieJg2->desEndPos.wing_y_offset.gStyle=4;
					pXieJg2->desEndPos.wing_y_offset.offsetDist=-base_wing_thick;
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
			pXieJg2->CalCutAngleInfo(pFatherAngle,NULL,false,NULL);
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
			pBolt->cfgword=pFatherAngle->cfgword;	//������˨��ĺ����׼������ĺ�һ��
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
	theApp.GetLDSDoc()->SetModifiedFlag(); // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	if(bCanUndo)	//֧�ֳ�������
		Ta.EndUndoListen();	//��������
	return TRUE;
}

//��ƹ��ð�
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
{	//��L_2LB1��������˨��Ϊ2���⣬���������𣬹ʿɸ���
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
			return FALSE;	//�˴����֧��������˨
		for(int j=0;j<nBoltNum;j++)
		{
			JGZJ jgzj;
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->AddL0Thick(pDatumLineAngle->handle,TRUE);
			pBolt->AddL0Thick(pRayJg->handle,TRUE);
			pBolt->iSeg=pRayJg->iSeg;
			pBolt->layer(2)=pRayJg->layer(2);	//������˨ͼ����
			pBolt->set_d(nBoltD);
			pBolt->cfgword=pDatumLineAngle->cfgword;	//������˨��ĺ����׼������ĺ�һ��
			//��˨λ�����\�������������Ϣ���
			pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
			pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pDatumLineAngle->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pRayJg->handle;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
			pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			//pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist2 = jgzj.g;
			pBolt->des_work_norm.norm_style=1;	//ָ���Ǹ�֫���߷���
			pBolt->des_work_norm.hVicePart = pDatumLineAngle->handle;
			getjgzj(jgzj,pDatumLineAngle->GetWidth());
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace,nBoltD);
			if(pConnectPoint->m_iConnectWing==0)	//���ڸ��Ǹ�X֫
			{
				pBolt->des_work_norm.norm_wing=0;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pDatumLineAngle->m_bEnableTeG)
					jgzj=pDatumLineAngle->xWingXZhunJu;
				if(nBoltNum==1)	//����˨
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj.g;
				else if(j==0)	//˫��˨
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj.g1;
				else if(j==1)
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 =(long)pDatumLineAngle->GetWidth()-LsSpace.EndSpace;//jgzj.g2;
				//pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = ftoi(pDatumLineAngle->GetNodeWingXG(pDatumNode));
			}
			else				//���ڸ��Ǹ�Y֫
			{
				pBolt->des_work_norm.norm_wing=1;
				pBolt->des_work_norm.direction=0;
				pBolt->des_base_pos.offset_wing=1;
				pBolt->des_base_pos.len_offset_dist=0;
				pBolt->des_base_pos.wing_offset_dist = 0;
				if(pDatumLineAngle->m_bEnableTeG)
					jgzj=pDatumLineAngle->xWingYZhunJu;
				if(nBoltNum==1)	//����˨
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 = jgzj.g;
				else if(j==0)	//˫��˨
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
				pRayJg->desStartOdd.m_iOddCalStyle=1;//������˨λ�ü�������ͷ
				pRayJg->CalStartOddment();
			}
			else 
			{
				pRayJg->AppendEndLsRef(pBolt->GetLsRef());
				pRayJg->desEndOdd.m_iOddCalStyle=1;	//������˨λ�ü�������ͷ
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
	pFlPlate->m_bStdPart=TRUE;	//��׼����
	pFlPlate->iSeg=pDatumTube->iSeg;
	pFlPlate->m_uStatMatCoef=1;
	pFlPlate->SetPartNo(pStdPartModel->GetPartNo(),true);
	pFlPlate->m_hPartWeldParent=pDatumTube->handle;
	pFlPlate->cfgword=pDatumTube->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pFlPlate->SetLayer(pDatumTube->layer()); //����������ͼ����
	pFlPlate->m_iParamType=pStdPartModel->m_iStdPartType;	
	//���ù���������ڸֹܻ�׼�����ת�Ƕ� wht 10-01-21
	//����ĸ'X'��ʾ��ת�Ƕȣ�D���׼������������ 
#ifdef AFX_TARG_ENU_ENGLISH
	pFlPlate->SetDesignItemValue('X',0,"relative angle of datum X axis rotating around tube X axis");
	pFlPlate->SetDesignItemValue('J',1,"ring layout bolt:1 on behalf of by the reference of weld road line��0 on behalf of by the reference of datum edge.");
	if(pFlPlate->m_iParamType==TYPE_FLP)	
		pFlPlate->SetDesignItemValue('O',pStdPartModel->param.fl_param.H1,"the distance between tube end and flange bottom");
#else
	pFlPlate->SetDesignItemValue('X',0,"��׼X����Ըֹ�X����ת�Ƕ�");
	pFlPlate->SetDesignItemValue('J',1,"��������˨ʱ:1��ʾ�Ժ�����Ϊ��׼��0��ʾ�Ի�׼��Ϊ��׼��");
	if(pFlPlate->m_iParamType==TYPE_FLP)	
		pFlPlate->SetDesignItemValue('O',pStdPartModel->param.fl_param.H1,"�ֹܶ��뷨������ľ���");
#endif
	pFlPlate->cMaterial=pStdPartModel->param.fl_param.cMaterial;	//��������
	if(bStartEnd)
		pDatumTube->m_tJointStart.hLinkObj=pFlPlate->handle;
	else 
		pDatumTube->m_tJointEnd.hLinkObj=pFlPlate->handle;
	pFlPlate->DesignStdPartUcs();
	pFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	pStdPartModel->CreateBoltInPlate(pFlPlate);//���÷����ϵ���˨
	//���ɶԽӷ���
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
	{	//��ƶԽӷ���
		if(pRelatedTube==NULL)
			return FALSE;
		double oddment;
		if(pFlPlate->m_iParamType==TYPE_FLP)		//ƽ������
			oddment=-pStdPartModel->param.fl_param.H1-pCoupleStdPartModel->param.fl_param.H1-fOddment;
		else //�Ժ�����
			oddment=-pCoupleStdPartModel->param.fl_param.H-pStdPartModel->param.fl_param.H-fOddment;
		CLDSParamPlate *pCoupleFlPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pCoupleFlPlate->CopyProperty(pFlPlate);
		pCoupleFlPlate->m_hPartWeldParent = pRelatedTube->handle;
		pCoupleFlPlate->layer(2)=pRelatedTube->layer(2); //����������ͼ����
		pCoupleFlPlate->cMaterial=pCoupleStdPartModel->param.fl_param.cMaterial;	//��׼��������
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
		//�޸���˨ͨ�����¼�����
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
	pDatumTube->CalPosition();	//����ֹ�λ��
	pFlPlate->DesignStdPartUcs();
	pFlPlate->DesignPlate();
	pFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pFlPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	//���Ʊ�׼�����ϵ���˨
	for(CLsRef *pLsRef=pFlPlate->GetFirstLsRef();pLsRef;pLsRef=pFlPlate->GetNextLsRef())
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	pFlPlate->ResetSonBoltRayNo();		//���ò������˨���ߺ�
	pFlPlate->GetLsRefList()->ResetBoltRefRayNo(pFlPlate);	//���ò������˨�����ߺ�
	return TRUE;
}
//��ƶԽӷ���
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
	{	//��ƵȾ��Խӷ���
		//����Ԥ�����ȼ����ҷ�����׼
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
	{	//�ɶ���Ʋ��Ⱦ�����
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
				logerr.Log("δ�ҵ�0x%X�ڵ㴦����0X%X�ϵķ����ֹ�ֱ�����%.0f/%.0f",pBaseNode->handle,
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
// ���´�������ֲ��StdComModel.cpp�� wjh-2019.1.29
#ifdef __DEPRECATED_CODE_
struct MATCHROD_INFO{
	bool matched;
	char ciConnTipS0E1Els2;
	char ciPlaneNo;
public:	//����
	short siRayDegAngle;
	DEG get_xRayDegAngle(){return DEG(siRayDegAngle);}
	__declspec(property(get=get_xRayDegAngle)) DEG xRayDegAngle;
	char get_ciRodType()		//'A':Across���˼���Խ��׼�ڵ�;'R':Radiation�˼��ӻ�׼�ڵ����߷���
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
public:	//���ӵ����ʶ����Ϣ
	GEPOINT lenBaseRodStdVec;
	PRESET_ARRAY8<MATCHROD_INFO> connrods;
public:	//���ӵ�ƥ����Ϣ
	long hBaseNode;
	CLDSLinePart* pBaseRod;
	char ciMirrorMatchMode;	//�Ƿ�Ϊ����Գ�
	PRESET_ARRAY8<long> xarrMatchRodHid;	//Ԥ��8���˼���������һ��widRod=0�˼����� 
public:	//����
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
		{	//�ᴩ�˼�
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
	{	//��ʾ����Ϊˮƽ����'X','x','Y'��'y'���п��ܣ���������ӵ����ж�
		if(fabs(pConnectPoint->datumplane.vLine.x)>fabs(pConnectPoint->datumplane.vLine.y))
			ciViewPlaneNormal=pConnectPoint->datumplane.vLine.x>0?'X':'x';
		else
			ciViewPlaneNormal=pConnectPoint->datumplane.vLine.y>0?'Y':'y';
	}
	GECS cs;	//�Ӿ�ͶӰƽ������ϵ��Z��ָ���˵��۾�������
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
		return false;	//����������ľ��ֽ�������������н�����Ϊ0.57735,ƽ��45������=0.707
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
	{	//�м�ᴩ����,ȡС��180����һ��н�Ϊ��׼�н�
		siDegAng=min(siDegAng,(siDegAng+180)%360);
	}
	short siMirTurnDegAng=siDegAng<=180?180-siDegAng:360-siDegAng;
	DEG xDegAngle(siDegAng),xMirAngle(siMirTurnDegAng);
	if( !xDegAngle.IsInSector(pStdConnPoint->xView.siMinBaseRodAngle,pStdConnPoint->xView.siMaxBaseRodAngle,true)&&
		!xMirAngle.IsInSector(pStdConnPoint->xView.siMinBaseRodAngle,pStdConnPoint->xView.siMaxBaseRodAngle,true))
		return false;	//��������Ҿ���Գƾ��޷�ƥ���׼�˼�
	bool bTooManyRods=false,bNeedNecessaryRod=false;
	UINT i,j,mirturn;
	bool meetconditions=true;
	MATCHROD_INFO* pMatchRod;
	SCHEMA_ROD* pSchemaRod;
	for(mirturn=0;mirturn<4;mirturn++)
	{	//mirturn�ֱ���������ӵ��ĶȶԳƷ�λ�Ĳ�ͬ��̬
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
					continue;	//��ƥ���
				DEG xRayDegAngle(pMatchRod->siRayDegAngle);
				if(mirturn==1)	//���ҶԳ�ƥ��
					xRayDegAngle=180+pMatchRod->siRayDegAngle;
				else if(mirturn==2)
					xRayDegAngle=360-pMatchRod->siRayDegAngle;
				else if(mirturn==3)
					xRayDegAngle=180+360-pMatchRod->siRayDegAngle;
				if(!xRayDegAngle.IsInSector(pSchemaRod->siMinRayAngle,pSchemaRod->siMaxRayAngle,pSchemaRod->dualSideEdge))
					continue;	//���߼нǲ�ƥ��
				if(pSchemaRod->ciRodType=='R'&&pMatchRod->ciRodType=='A')
					continue;	//���������Ͳ�һ��(SchemaRodΪ'A'��MatchRodΪ'R'ʱ���ǵ�������ͷ���������Ϊһ��)
				char szRodSectType[4]={0};
				szRodSectType[0]=pMatchRod->pRod->size_cSubClassType;
				if(!pMatchRod->pRod->IsTube())
					memcpy(szRodSectType,"��",2);
				else if(!pMatchRod->pRod->IsAngle())
					szRodSectType[0]=0;

				if(pSchemaRod->xFlag.bFilterSectType)
				{	//�˼����������ж�
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
				meetconditions=false;	//ȱ�ٱ�Ҫ�ĸ˼�
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
		/*TODO:����ѡ��idStdNodePlate=pStdConnPoint->hashMaybeTmplPlates.GetFirst();
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
	if(pConnectPoint->designType.bFlJoint)			//��ƶԽӷ���
	{
		pLinePart=Ta.FromRodHandle(pConnectPoint->m_hJointPart);
		if(pLinePart.blValid)
			DesignTubeFlJoint(pBaseNode,pDatumRod.LineTubePointer(),pLinePart.LineTubePointer());
		return 0;
	}
	else if(pConnectPoint->designType.bAngleJoint)	//������ƽǸֽ�ͷ������
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
				return 0;	//�˽�ͷ�����
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
	if (pConnectPoint->designType.bIntersBolt)		//��ƽ������˨
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
		if (status > 0)	//���ݱ�׼�ڵ㹹���ģ�����ʧ�ܣ������û������жϣ�����ʱ���ݴ�ͳ����������ɽڵ��
		{	//ע�������ɵ����Ӱ�
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
		//��ͳģʽ���������еĽڵ�岻�����߽ض��������Ӧ�޳��ԽӵĽ�ͷ�˼� wjh-2019.11.1
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
			//�������ȼ��������
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
				else if(g_sysPara.m_arrL1RayPole[i].CompareNoCase("L_1LO1")==0&&//��ͷ�ְ�����
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,0,TRUE))>0)
					break;
			}
		}
		else if(pConnectPoint->linkPartSet.GetNodeNum()==2)
		{	//˫���߸˼�
			//�������ȼ��������
			CLDSLinePart* pRayRod1=pConnectPoint->linkPartSet.GetFirst();
			CLDSLinePart* pRayRod2=pConnectPoint->linkPartSet.GetNext();
			bool mirTypePlate=false;
			if(pRayRod1->MirSimilarGradeWith(pRayRod2,MIRMSG(1))<=1||pRayRod1->MirSimilarGradeWith(pRayRod2,MIRMSG(2))<=1)
				mirTypePlate=true;//���߶Գ�ʱ��Ӧѡ�񲻶ԳƵĽṹ����(������������м����������
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
		//�����߸˼�������߸˼�
		else if(pConnectPoint->linkPartSet.GetNodeNum()==3)
		{	//�������ȼ��������
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
				logerr.Log("��׼�ڵ�(0X%X)���׼��(0X%X)�޹�����ĺ�,���ܽڵ㸸�˼����ڵ�ǰ������!",pBaseNode->handle,pDatumRod->handle);
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
					logerr.Log("���߲�(0X%X)���׼�ڵ�(0X%X)����׼��(0X%X)�޹�����ĺ�,���ܽڵ㸸�˼����ڵ�ǰ������!",pRod->handle,pBaseNode->handle,pDatumRod->handle);
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
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,TRUE))>0)		//�����м����߲Ķ�����
					break;
				else if(g_sysPara.m_arrL3RayPole[i].CompareNoCase("L_3LC2")==0&&
					(plate.hPlate=design.DesignConnectPoint(pConnectPoint,2,TRUE,1,FALSE))>0)//�����������߲Ķ�����
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
	//2.��Ƶ���˨����
	//3.��ƹ��ð�������η���
	//4.�����ͨ���Ӱ�
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
	NODESET nodeSet;	//�ڵ㼯��
	long *id_arr=NULL;
	int i,serial,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CUndoOperObject undo(&Ta,true);
	if(n<=0)
	{	//ѡ�񹹼�
		OpenWndSel();	//����ѡ��
		GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECTINDEX_NODE),CLS_NODE,0,2);
		scr_part_para.disp_type=DISP_LINE;
#ifdef AFX_TARG_ENU_ENGLISH
		scr_part_para.cmdStr="please select the node of intelligent connection design:";
		scr_part_para.cmdErrorFeedbackStr="no proper selected part,please select the node of intelligent connection design again:";
#else
		scr_part_para.cmdStr="��ѡ��Ҫ��������������ƵĽڵ�:";
		scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�����������ѡ��Ҫ��������������ƵĽڵ�:";
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
		char cWorkWing;		//�ְ嶨λ��׼�ǸֵĹ���֫������֫��
		long hAttachNode;	//�ְ��������Ľڵ�
		long hLocationAngle;//�ְ嶨λ��׼�Ǹ�
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
	//�������Ĭ����˨��Ϊ����ȱʧ�ĵ���϶ wjh-2019.9.3
	CStackVariant<char> stackprop0i(&CLDSBolt::BOLTL0_PREFER_MODE,CLDSBolt::L0CAL_INC_GAP_THICK);
	CStackVariant<bool> stackprop1(&CStdComModel::m_bEnableMirCreate,false);
	CLDSPartPtr xarrNewPartsPool[500];
	CNewPartsListen listen(&Ta.Parts,xarrNewPartsPool,500);
	CDisplayView *pCurView=console.GetActiveView();
	for(pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext(),serial++)
	{	//����ʶ��ǰ���ӵ���Ϣ
		CONNECT_NODEINFO xConnNodeInfo;
		pNode->RetrieveConnectNodeInfo(&xConnNodeInfo,g_sysPara.intelligentConnect.m_bDesignBendConnect);
		if(xConnNodeInfo.ciTransitConnType==1)
		{	//���´����
			CStackVariant<bool> stackprop(&CStdComModel::m_bEnablePromptUI, intelli.m_bEnablePromptUI);
			if(xConnNodeInfo.transit.pDownRod->GetClassTypeId()==CLS_LINEANGLE)
				CCommonLjSmartDesign::DesignSlopeTransition(&xConnNodeInfo.transit);
			else if(xConnNodeInfo.transit.pDownRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				CCommonLjSmartDesign::DesignSlopOrFootPoint(&xConnNodeInfo.transit);
			continue;
		}
		else if(xConnNodeInfo.ciTransitConnType==2)
		{	//���Ű����
			CCommonLjSmartDesign::DesignSlopOrFootPoint(&xConnNodeInfo.transit);
			continue;
		}
		//�����ͨ����
		CConnectPointInfo xConnPointInfo;
		const BYTE xarrConstBytes[8]={ 0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80 };
		for(i=0;i<(int)xConnNodeInfo.xCommNode.planes.Count;i++)
		{
			CONNECT_PLANE* pConnPlane=&xConnNodeInfo.xCommNode.planes[i];
			if (!pConnPlane->blValid||(xarrConstBytes[i]&xConnNodeInfo.cbDisableFlag)>0)
				continue;	//��ǰ��������Ч�����Ѿ���֮ǰ�������Ƶ�����������ǰ������� wjh-2019.10.6
			if(!pConnPlane->designType.bIntersBolt&&g_sysPara.intelligentConnect.m_bDesignCommonConnect)
			{
#ifdef __SMART_DRAW_INC_
				if(g_sysPara.intelligentConnect.m_bOnlyDesignCurView&&
					fabs(pCurView->ucs.axis_z*pConnPlane->datumplane.vSolid)<0.9)//EPS_COS2)
#elif defined(__COMMON_PART_INC_)			//���������֧�ֵ�ǰ�������������
				if(fabs(pCurView->ucs.axis_z*pConnPlane->datumplane.vSolid)<0.9)//EPS_COS2)
#endif
				{
					//pConnectList->DeleteCursor();
					continue;	//�����ɵ�ǰ��ְ�
				}
				if(xConnNodeInfo.GetConnectPointInfo(i,&xConnPointInfo))
					intelli.DesignConnectPointStructure(&xConnPointInfo,&intelli); //��Ƹ����ӵ�����
			}
		}
		//��ƽ������˨
		for(i=0;i<(int)xConnNodeInfo.xCommNode.planes.Count;i++)
		{
			CONNECT_PLANE* pConnPlane=&xConnNodeInfo.xCommNode.planes[i];
			if (!pConnPlane->blValid||!pConnPlane->designType.bIntersBolt)
				continue;
			if(xConnNodeInfo.GetConnectPointInfo(i,&xConnPointInfo))
				intelli.DesignConnectPointStructure(&xConnPointInfo,&intelli); //��Ƹ����ӵ㽻�����˨����
		}
		//��ƽǸֽ�ͷ���ֹܷ����ȣ����ڵ�Ȧ��������϶��ԭ��������ƣ�
#ifdef __SMART_DRAW_INC_
		if ( xConnNodeInfo.xCommNode.hJointRod>0&&(
			(g_sysPara.intelligentConnect.m_bDesignAngleJoint&&!xConnNodeInfo.xCommNode.blFlJoint)||
			(g_sysPara.intelligentConnect.m_bDesignFL&&xConnNodeInfo.xCommNode.blFlJoint)))
		{
			if (xConnNodeInfo.GetConnectPointInfo(4,&xConnPointInfo))
				intelli.DesignConnectPointStructure(&xConnPointInfo,&intelli); //��Ƹ����ӵ�Ǹֽ�ͷ
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
	stackprop0.PopStack();	//�ָ�������˨���Լ��㣬�Ա㼯��������˨ͨ�� wjh-2019.9.3
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
	OnOperOther();	//ȡ����������״̬
	g_pSolidDraw->ReleaseSnapStatus();
	theApp.m_ePrevEnvType=theApp.m_eEnvType;
	theApp.m_eEnvType=SOLID_ENV;
	LdsWorkEnvShift(SOLID_ENV);	//ת����ʵ�彨ģ��������ʾ����Ƶ�����
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
	{	//ѡ��Ҫƫ�ƶ˽ڵ�����߸˼�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSnap->SetSelectPartsType(SELECT_LINEPART);
		OpenWndSel();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("OffsetEndNode Please select all ray-rods :","");
#else 
		pCmdLine->FillCmdLine("OffsetEndNode ��ѡ�����ƫ�ƶ˽ڵ�����߸˼�:","");
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
	//ѡ��Ҫƫ�ƵĽڵ�
	CLDSNode* pBaseNode=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("BranchNodes Please select the datum node :","");
#else 
	pCmdLine->FillCmdLine("BranchNodes ��ѡ��Ҫ���в�ֵĸ˼��˽ڵ�:","");
#endif
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verify(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verify.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	//{ԭUNDO�������ִ˺�������Ϊ�Ӻ������������û������ wjh-2017.12.21
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
					AfxMessageBox(CXhChar50("ѡ�и˼�0x%X�Ķ˽ڵ㲻�ǵ�ǰѡ�еĽڵ�",pLinePart->handle));
					break;
				}
			}
			if(pLinePart==NULL)
				break;
		}
	}
	//}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBaseNode->handle));
	//����ƫ����
	CLDSLinePart* pBaseRod=(CLDSLinePart*)console.FromPartHandle(pBaseNode->hFatherPart,CLS_LINEPART);
	if(pBaseRod==NULL)
	{
		AfxMessageBox("ѡ��Ļ�׼�ڵ�û�и��˼�!");
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
	if(!xCmdMiniPanel.m_bSpecOffset)	//δ����ָ��ֵʱ����Ĭ��Ϊ��ǰƫ��ֵ
		xCmdMiniPanel.m_nSpecOffset=40;//fOffsetDist;
	else	//����ָ��ֵʱ����Ĭ��Ϊ��һ��ָ��ƫ��ֵ
		fOffsetDist=abs((int)xCmdMiniPanel.m_nSpecOffset);
	char ciMirBranchNodeMode=0;
	{
		btc::SKETCH_PLANE sketch(datum_pos,base_vec,ucs.axis_x,300);
		CWorkPlaneSketchLifeObject workPlaneSketchLife;
		workPlaneSketchLife.WorkPlaneSketch(sketch,sign*base_vec,"ƫ�Ʒ���",100);
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine((char*)CXhChar100("Please Input offset distance<%.f>:",fOffsetDist),"");
#else 
		pCmdLine->FillCmdLine((char*)CXhChar100("��ѡ��ƫ�ƻ�׼�˼���������ƫ��ֵ<%.f>:",fOffsetDist),"");
		//pCmdLine->FillCmdLine((char*)CXhChar100("BranchNodes ������ƫ�ƾ�<%.f>:",fOffsetDist),"");
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
					workPlaneSketchLife.WorkPlaneSketch(sketch2,sign*base_vec,"ƫ�Ʒ���");
					g_pSolidDraw->Draw();
				}
				continue;	//������׼ƫ�Ʒ������ȻӦȷ��ƫ�ƾ���
			}
		}
		fOffsetDist*=sign;
		if(ciMirBranchNodeMode==0)
			ciMirBranchNodeMode=xCmdMiniPanel.m_bMirAll?'Y':'N';
	}
	xCmdMiniPanel.m_nSpecOffset=ftoi(abs(fOffsetDist));
	f3dPoint offset_pos=datum_pos+base_vec*fOffsetDist;
	//����ƫ�Ƶ㣬���������߸˼��˽ڵ�
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
	//�������߽ǸֵĶ˽ڵ���Ϣ
	for(pRayRod=linePartSet.GetFirst();pRayRod;pRayRod=linePartSet.GetNext())
	{
		//���µ�ǰ�����߽ǸֵĶ˽ڵ���Ϣ
		if(pRayRod->pStart==pBaseNode)
			pRayRod->pStart=pNewNode;
		else
			pRayRod->pEnd  =pNewNode;
		pRayRod->CalPosition(true);
		pRayRod->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pRayRod->GetSolidPartObject());
	}

	//���������˼�Ϊ�����߽ǸֵĽ���ڵ������Ϣ
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
			{	//����ڵ�
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
			{	//��ǰ���߸˼��ϵ������ڵ�
				bNeedMove=TRUE;
				for(int i=0;i<4;i++)
				{
					if(pNode->arrRelationNode[i]==pCurrNode->handle)
						pNode->arrRelationNode[i]=pBranchNode->handle;
				}
			}
		}
		if(bNeedMove)
		{	//���½ڵ�����
			pNode->CalPosition(true);
			g_pSolidDraw->ModifyDbPoint(pNode->handle,pNode->xActivePos);
		}
	}
	console.PopNodeStack(pushed);

	g_pSolidDraw->Draw();
	g_pSolidDraw->ReleaseSnapStatus();
	return TRUE;
}
//���ܺᵣ����
#include "CrossArmDesignDlg.h"
static long GetGlobalNewHandle()
{
	return Ta.GetNewHandle();
}
/* �ù����������°��CParaTemplate�� wjh-2014.12.18
void CLDSView::OnDesignCrossArm()
{
	//1���趨ģ��ģ��
	CLogErrorLife logerrLife;
	CParaBlockModel model;
	CParaBlockModelTemplate para_template;
	para_template.m_iTempType=CParaBlockModelTemplate::CROSSARM_TEMPLATE;
	model.GetNewHandleHookFunc=GetGlobalNewHandle;
	model.m_pTemplate=&para_template;
	//2����ƶԻ�����ʾ���޸�ģ�����
	CCrossArmDesignDlg crossArmDlg;
	crossArmDlg.m_pModel=&model;
	if(crossArmDlg.DoModal()!=IDOK)
		return;
	//3������ѡ��ᵣģ������ؽڵ�
	CLDSNode* keyNodeArr[4]={NULL};
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString sCmdStr;
	sCmdStr.Format("��ѡ��ᵣģ��ؼ��ڵ�(��%d��)��",crossArmDlg.m_nKeyNodeNum);
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
			sCmdStr.Format("û��ѡ�к��ʵĽڵ㣬������ѡ��ؼ��ڵ�(��%d��):",crossArmDlg.m_nKeyNodeNum);
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
			sCmdStr.Format("û��ѡ�к��ʵĽڵ㣬������ѡ��ؼ��ڵ�(��%d��):",crossArmDlg.m_nKeyNodeNum);
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
	//���ѡ���������ؽڵ㣬ͨ���Գƹ�ϵ�ҵ��������ؽڵ�
	if(keyNodeArr[2]==NULL || keyNodeArr[3]==NULL)
	{
		CLDSNode* pMirNode1=keyNodeArr[0]->GetMirNode(MIRMSG(crossArmDlg.m_iKeyNodeMir));
		CLDSNode* pMirNode2=keyNodeArr[1]->GetMirNode(MIRMSG(crossArmDlg.m_iKeyNodeMir));
		if(pMirNode1==NULL || pMirNode2==NULL)
		{
			logerr.Log("û���ҵ��Գ����ؽڵ�!");
			return;
		}
		keyNodeArr[2]=pMirNode1;
		keyNodeArr[3]=pMirNode2;
	}

	//4�������ᵣģ��
	for(int i=0;i<4;i++)
	{
		model.keyNodeSet.append(keyNodeArr[i]);
		CLDSPart* pPart=console.FromPartHandle(keyNodeArr[i]->hFatherPart);
		model.keyPartSet.append(pPart);
	}
	model.RebuildModel();
	//5�����ɺᵣ�Ľڵ㡢�˼�
	Ta.BeginUndoListen();
	CLDSNode* pTemNode=NULL;
	for(CLDSNode* pNode=model.Node.GetFirst();pNode;pNode=model.Node.GetNext())
	{	
		if(Ta.FromNodeHandle(pNode->handle))	//�������ؽڵ�
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
		if(Ta.FromNodeHandle(pLinePart->handle))	//�������ظ˼�
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