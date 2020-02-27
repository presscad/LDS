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
#include "UI.h"
#include "DesignMultiLeg.h"	//CMappingBolts defined
#include "LdsPortalLocale.h"
#include "IntelliJointConnect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

extern int deita,S1;//S1Ϊ������,��ʵӦ�ɵ�ǰ�ڵ�Ľڵ��ȷ���Ժ�Ӧ����//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDesignJoint::CDesignJoint()
{
	ls_group1.wnConnBoltN = ls_group2.wnConnBoltN = 0;
	ls_group1.rows = ls_group2.rows = 1;
	xDesJointPara.norm_x_wing.norm_style=1;
	xDesJointPara.norm_y_wing.norm_style=1;
}

CDesignJoint::~CDesignJoint()
{

}
/*�Ǹֽ�ͷ���������ϵ��������:
	Z�᷽��Ϊ�ӵ�һ����׼�Ǹ�ָ��ڶ�����׼�Ǹ�
	X����Y�᷽��Ӧ�ص�һ����׼�Ǹֵ�֫���췽����Z�᷽��һ��
	ԭ�㶨��Ϊ���Ǹֿ�����һ����׼�Ǹֵ���ʼ��(λ�ڵ�һ��
	��׼�Ǹ�������)
	S1Ϊ���ڵ�ǰ�ڵ��ϵ����Ӱ��ƫ�Ƶľ���,deitaΪ��ͷ�����Ӱ�
	��ļ�϶
	******������ϵ����������X����Y�������׼�Ǹֵ���֫******
*/
void CDesignJoint::CreateRefAxisNorm(CJoint *pJoint)
{
	f3dPoint axis_z;
	axis_z = xDesJointPara.m_pMainJg1->End()-xDesJointPara.m_pMainJg1->Start();
	normalize(axis_z);
	xDesJointPara.norm_x_wing.hVicePart=xDesJointPara.m_pMainJg1->handle;
	xDesJointPara.norm_y_wing.hVicePart=xDesJointPara.m_pMainJg1->handle;
	xDesJointPara.start.datum_pos_style=xDesJointPara.end.datum_pos_style=7;
	xDesJointPara.start.des_para.RODEND.bIncOddEffect=TRUE;
	xDesJointPara.start.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
	xDesJointPara.end.des_para.RODEND.hRod=xDesJointPara.m_pMainJg2->handle;
	xDesJointPara.end.des_para.RODEND.bIncOddEffect=TRUE;
	if(xDesJointPara.m_pMainJg1->pStart==xDesJointPara.m_pCurNode)
	{	//��һ����׼�Ǹ���ʼ��Ϊ��ƶ˵�
		pJoint->ucs.axis_z = -1.0*axis_z;
		pJoint->ucs.axis_x = xDesJointPara.m_pMainJg1->GetWingVecY();
		pJoint->ucs.axis_y = xDesJointPara.m_pMainJg1->GetWingVecX();
		pJoint->ucs.origin = xDesJointPara.m_pMainJg1->Start()+m_pJoint->ucs.axis_z*(S1+deita);
		xDesJointPara.start.des_para.RODEND.direction=0;
		xDesJointPara.norm_x_wing.vector = xDesJointPara.m_pMainJg1->get_norm_y_wing();
		xDesJointPara.norm_y_wing.vector = xDesJointPara.m_pMainJg1->get_norm_x_wing();
		xDesJointPara.norm_x_wing.norm_wing=1;
		xDesJointPara.norm_y_wing.norm_wing=0;
	}
	else
	{	//��һ����׼�Ǹ���ֹ��Ϊ��ƶ˵�
		pJoint->ucs.axis_z = axis_z;
		pJoint->ucs.axis_x = xDesJointPara.m_pMainJg1->GetWingVecX();
		pJoint->ucs.axis_y = xDesJointPara.m_pMainJg1->GetWingVecY();
		double len = DISTANCE(xDesJointPara.m_pMainJg1->Start(),xDesJointPara.m_pMainJg1->End());
		xDesJointPara.start.des_para.RODEND.direction=1;
		pJoint->ucs.origin=xDesJointPara.m_pMainJg1->End()+m_pJoint->ucs.axis_z*(S1+deita);
		xDesJointPara.norm_x_wing.vector = xDesJointPara.m_pMainJg1->get_norm_x_wing();
		xDesJointPara.norm_y_wing.vector = xDesJointPara.m_pMainJg1->get_norm_y_wing();
		xDesJointPara.norm_x_wing.norm_wing=0;
		xDesJointPara.norm_y_wing.norm_wing=1;
	}
	if(xDesJointPara.m_pMainJg2->pStart==xDesJointPara.m_pCurNode)
		xDesJointPara.end.des_para.RODEND.direction=0;
	else
		xDesJointPara.end.des_para.RODEND.direction=1;
}
extern void AddMirObjToSrcObjRelaObjList(CLDSDbObject *pSrcObj,CLDSDbObject *pMirObj,MIRMSG msg);
extern void SetRelativeObj(CLDSDbObject *pObj);
void CDesignJoint::MirJoint(CJoint *pJoint,int iMirMsg)
{
	CJoint mirJoint;
	CLDSNode *pBaseNode,*pMirBaseNode;
	CLDSLineAngle *pMainJg1,*pMainJg2,*pMirMainJg1,*pMirMainJg2;
	pMainJg1 = (CLDSLineAngle*)console.FromPartHandle(pJoint->base_jg_handle_arr[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	pMainJg2 = (CLDSLineAngle*)console.FromPartHandle(pJoint->base_jg_handle_arr[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	pBaseNode = console.FromNodeHandle(pJoint->base_node_handle);
	if(pMainJg1==NULL||pMainJg2==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "basic angle of parent joint lost, joint symmetric design failure!";
#else
		throw "����ͷ�Ļ�׼�Ǹֶ�ʧ,��ͷ�Գ����ʧ��!";
#endif
	if(iMirMsg>0&&iMirMsg<=3)
	{
		pMirBaseNode = pBaseNode->GetMirNode(iMirMsg);
		if(pMirBaseNode==NULL||pMirBaseNode==pBaseNode)
			return;	//��׼�ڵ���ͬ���ͷ����Ʋ������ظ���ƽǸֽ�ͷ
		mirJoint.cfgword=Ta.GetDefaultCfgPartNo();
		mirJoint.SetBelongModel(pJoint->BelongModel());
		pMirMainJg1 = (CLDSLineAngle*)pMainJg1->GetMirPart(iMirMsg);
		pMirMainJg2 = (CLDSLineAngle*)pMainJg2->GetMirPart(iMirMsg);
		if(pMirMainJg1==NULL||pMirMainJg2==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "basic angle of parent joint not symmetric, joint symmetric design failure!";
#else
			throw "����ͷ�Ļ�׼�Ǹֲ��Գ�,��ͷ�Գ����ʧ��!";
#endif
		pMirMainJg1->feature = pMainJg1->feature;
		pMirMainJg2->feature = pMainJg2->feature;
		mirJoint.base_jg_handle_arr[0] = pMirMainJg1->handle;
		mirJoint.base_jg_handle_arr[1] = pMirMainJg2->handle;
		if(pMirBaseNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "basic nodes of parent joint not symmetric,joint symmetric design failure!";
#else
			throw "����ͷ�Ļ�׼�ڵ㲻�Գ�,��ͷ�Գ����ʧ��!";
#endif
		mirJoint.base_node_handle = pMirBaseNode->handle;
		char sNewLayer[16]="SPQ";
		CalMirAtomLayer(pJoint->layer(),sNewLayer,MIRMSG(iMirMsg));
		mirJoint.SetLayer(sNewLayer);
		mirJoint.iSeg=pJoint->iSeg;
		mirJoint.blInheritExistBoltsInMainAngle = pJoint->blInheritExistBoltsInMainAngle;
		mirJoint.blInheritExistBoltsInCoupleAngle = pJoint->blInheritExistBoltsInCoupleAngle;
		mirJoint.blIntelliCalOddment = false;// pJoint->blIntelliCalOddment;
		if (!mirJoint.blIntelliCalOddment)
		{
			if (pMainJg1->pStart == pBaseNode)
			{
				pMirMainJg1->desStartOdd.m_iOddCalStyle = 2;
				pMirMainJg1->SetStartOdd(pMainJg1->dfStartOdd);
			}
			else
			{
				pMirMainJg1->desEndOdd.m_iOddCalStyle = 2;
				pMirMainJg1->SetEndOdd(pMainJg1->dfEndOdd);
			}
			if (pMainJg2->pStart == pBaseNode)
			{
				pMirMainJg2->desStartOdd.m_iOddCalStyle = 2;
				pMirMainJg2->SetStartOdd(pMainJg2->dfStartOdd);
			}
			else
			{
				pMirMainJg2->desEndOdd.m_iOddCalStyle = 2;
				pMirMainJg2->SetEndOdd(pMainJg2->dfEndOdd);
			}
		}
		CreateJoint(&mirJoint,FALSE);
		//���ԳƵİ�����ӵ�ԭ�����Ĺ�������,�����¹�����ϵ
		PART_ITEM *pOrgItem=NULL,*pMirItem=NULL;
		CLDSLineAngle* pWrapJg=NULL,*pMirJg=NULL;
		for(int i=0;i<4;i++)
		{
			char cID='A'+i;
			pOrgItem=pJoint->FromPartId(cID);
			pMirItem=mirJoint.FromPartId(cID);
			if(pOrgItem==NULL || pMirItem==NULL)
				continue;
			pWrapJg=(CLDSLineAngle*)console.FromPartHandle(pOrgItem->h,CLS_LINEANGLE);
			pMirJg=(CLDSLineAngle*)console.FromPartHandle(pMirItem->h,CLS_LINEANGLE);
			if(pWrapJg && pMirJg)
			{
				AddMirObjToSrcObjRelaObjList(pWrapJg,pMirJg,MIRMSG(iMirMsg));
				SetRelativeObj(pWrapJg);
			}
		}
		//���ԳƵİ�����ӵ�ԭ����Ĺ�������,�����¹�����ϵ
		CLDSPlate *pWrapPlank=NULL,*pMirPlank=NULL;
		for(int i=0;i<4;i++)
		{
			char cID='E'+i;
			pOrgItem=pJoint->FromPartId(cID);
			pMirItem=mirJoint.FromPartId(cID);
			if(pOrgItem==NULL || pMirItem==NULL)
				continue;
			pWrapPlank=(CLDSPlate*)console.FromPartHandle(pOrgItem->h,CLS_PLATE);
			pMirPlank=(CLDSPlate*)console.FromPartHandle(pMirItem->h,CLS_PLATE);
			if(pWrapPlank && pMirPlank)
			{
				AddMirObjToSrcObjRelaObjList(pWrapPlank,pMirPlank,MIRMSG(iMirMsg));
				SetRelativeObj(pWrapPlank);
			}
		}
	}
	else
	{
		MirJoint(pJoint,1);	//Y��Գ�
		MirJoint(pJoint,2);	//X��Գ�
		MirJoint(pJoint,3);	//Z��Գ�
	}
}

//�������Ĺ���Լ���˨���Զ��жϽǸֽ�ͷ�����Լ�����,�жϹ�������:
//1.��ͷΪ��������ʱ,��������Ǹ�,����ǸֵĿ��Ӧ�ȱ����ӽǸ�֫���һ��
//  (��ϸ����80����ʱ,����Ǹ�֫���ٴ�һ��),�����ӽǸֹ��ͬʱ,Ӧȡ��С�Ĺ��
//2.��ͷΪ˫������ʱ,�����ڰ��Ǹ�������,�ڰ��Ǹֺ��������������˲�С�ڱ����ӽǸ������1.3��
//3.L140���Ϲ��ĽǸ��˲���˫������
//joint_type 0.����� 1.������� 2.�ڰ�������ְ� 3.�ڰ��ְ������ 4.�Խ���Ͻ�ͷA 
//			 5.�Խ���Ͻ�ͷB 6.�Խ���Ͻ�ͷC 7.�Խ���Ͻ�ͷD 8.ʮ����Ͻ�ͷA 9.ʮ����Ͻ�ͷB
BOOL CalJgJointPara(CLDSLineAngle *pMainJg1,CLDSLineAngle *pMainJg2,
					JG_JOINT_PARA *pJointPara,int &joint_type)
{
	if(pMainJg1==NULL||pMainJg2==NULL||pJointPara==NULL)
		return FALSE;
	memset(pJointPara,0,sizeof(JG_JOINT_PARA));
	CLDSLineAngle *pBaseJg=pMainJg1;
	if(pMainJg1->GetWidth()>pMainJg2->GetWidth())
		pBaseJg=pMainJg2;	//ȡ֫���խ�ĽǸ�Ϊ��׼�Ǹ�
	pJointPara->fBaseJgWidth=pBaseJg->GetWidth();
	pJointPara->fBaseJgThick=pBaseJg->GetThick();
	CConnectInfo connectInfo1,connectInfo2;
	if(pMainJg1->pStart==pMainJg2->pStart||pMainJg1->pStart==pMainJg2->pEnd)
	{
		connectInfo1=pMainJg1->connectStart;
		if(pMainJg1->pStart==pMainJg2->pStart)
			connectInfo2=pMainJg2->connectStart;
		else 
			connectInfo2=pMainJg2->connectEnd;
	}
	else if(pMainJg1->pEnd==pMainJg2->pStart||pMainJg1->pEnd==pMainJg2->pEnd)
	{
		connectInfo1=pMainJg1->connectEnd;
		if(pMainJg1->pEnd==pMainJg2->pStart)
			connectInfo2=pMainJg2->connectStart;
		else 
			connectInfo2=pMainJg2->connectEnd;
	}
	else //���Ǹ��޹��ýڵ�
		return FALSE;
	if(joint_type<0)
	{	//�Զ��жϽǸֽ�ͷ����
		if(connectInfo1.rows==1&&connectInfo2.rows==1
			&&pMainJg1->GetWidth()<140&&pMainJg2->GetWidth()<140)
			joint_type=0;	//�����
		else 
			joint_type=2;	//�ڰ�������ְ�
	}
	//���ݽǸֽ�ͷ���Ͳ��ҽ�ͷ����
	JG_JOINT_PARA *pTempJointPara=FindJgJointPara(pJointPara->fBaseJgWidth,pJointPara->fBaseJgThick);
	if(pTempJointPara==NULL)
		return FALSE;
	if(joint_type==0)
	{	//�����
		pJointPara->fOuterJgWidth=pTempJointPara->fOuterJgWidth;
		pJointPara->fOuterJgThick=pTempJointPara->fOuterJgThick;
		JG_PARA_TYPE *pJgParaType=FindJgType(pBaseJg->GetWidth(),pBaseJg->GetThick());
		if(pJgParaType)
		{
			double fLamda=(pBaseJg->GetLength()/10)/pJgParaType->Rx;//(ʹ��ƽ�����ת�뾶)����õ��ĳ�ϸ��
			if(fLamda<80)	//��ϸ����80����ʱ,����Ǹ�֫���ٴ�һ��
				pJointPara->fOuterJgThick+=2;
		}
	}
	else if(joint_type==1)
	{	//�������
		pJointPara->fOuterJgWidth=pTempJointPara->fOuterJgWidth;
		pJointPara->fOuterJgThick=pTempJointPara->fOuterJgThick;
		pJointPara->fInnerJgWidth=pTempJointPara->fInnerJgWidth;
		pJointPara->fInnerJgThick=pTempJointPara->fInnerJgThick;
	}
	else if(joint_type==2)
	{	//�ڰ�������ְ�
		pJointPara->fOuterPlateWidth=pTempJointPara->fOuterPlateWidth;
		pJointPara->fOuterPlateThick=pTempJointPara->fOuterPlateThick;
		pJointPara->fInnerJgWidth=pTempJointPara->fInnerJgWidth;
		pJointPara->fInnerJgThick=pTempJointPara->fInnerJgThick;
	}
	else if(joint_type==3)
	{	//�ڰ��ְ������
		pJointPara->fOuterPlateWidth=pTempJointPara->fOuterPlateWidth;
		pJointPara->fOuterPlateThick=pTempJointPara->fOuterPlateThick;
		pJointPara->fOuterJgWidth=pTempJointPara->fOuterJgWidth;
		pJointPara->fOuterJgThick=pTempJointPara->fOuterJgThick;
	}
	else 
	{	//�ڰ�������ְ�
		pJointPara->fOuterPlateWidth=pTempJointPara->fOuterPlateWidth;
		pJointPara->fOuterPlateThick=pTempJointPara->fOuterPlateThick;
		pJointPara->fInnerJgWidth=pTempJointPara->fInnerJgWidth;
		pJointPara->fInnerJgThick=pTempJointPara->fInnerJgThick;
	}
	return TRUE;
}

CJointDesParaDlg JointDesDlg;
CEditJointJgZhunJuDlg zhunjudlg;
WORD CalJointConnBoltN(double dfMinWrapLength,LSSPACE_STRU& xBoltSpace,int niMinBoltN,bool blCanDoubleRow=true)
{	//������ͷ��ͷ���Ȳ�ӦС�ڱ����ӹ���֫���2��(��L��2B)
	//˫����ͷ��ͷ���Ȳ�ӦС�ڱ����ӹ���֫���1.5��(��L��1.5B)
	long niWrapLength=0;
	BYTE rows=(niMinBoltN>=5&&blCanDoubleRow)?2:1;
	if (!blCanDoubleRow)
	{	//ֻ�����Ų���
		niWrapLength=xBoltSpace.EndSpace*2+xBoltSpace.SingleRowSpace*(niMinBoltN-1);
		if (xBoltSpace.SingleRowSpace<=0)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"��˨�����ϢΪ0����ͷ���ʧ��");
			return niMinBoltN;
		}
		while (niWrapLength<dfMinWrapLength&&xBoltSpace.SingleRowSpace>0)
		{
			niMinBoltN++;
			niWrapLength+=xBoltSpace.SingleRowSpace;
		}
		return niMinBoltN;
	}
	long liHalfDoubleRowSpace=xBoltSpace.doubleRowSpace/2;
	if(rows==1)
		niWrapLength=xBoltSpace.EndSpace*2+xBoltSpace.SingleRowSpace*(niMinBoltN-1);
	else
		niWrapLength=xBoltSpace.EndSpace*2+liHalfDoubleRowSpace*(niMinBoltN-1);
	while (niWrapLength<dfMinWrapLength)
	{
		niMinBoltN++;
		if (niMinBoltN>=5)
			rows=2;
		if(rows==1)
			niWrapLength=xBoltSpace.EndSpace*2+xBoltSpace.SingleRowSpace*(niMinBoltN-1);
		else
			niWrapLength=xBoltSpace.EndSpace*2+liHalfDoubleRowSpace*(niMinBoltN-1);
		if((rows==1&&xBoltSpace.SingleRowSpace<=0)||(rows==2&&liHalfDoubleRowSpace<=0))
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"��˨�����ϢΪ0����ͷ���ʧ��");
			return niMinBoltN;
		}
	}
	return niMinBoltN;
}
//ԭbNoMirΪ��������ʵ��Ӧ��blEnableMirCreate�Ÿ��ܱ��ԭʼ���뺬�� wjh-2019.6.20
BOOL CDesignJoint::CreateJoint(CJoint *pJoint,BOOL blEnableMirCreate/*=TRUE*/,BOOL bIntelligent/*=FALSE*/)
{
	m_pJoint = pJoint;
	try
	{
		//int base_ls_d;
		//LSSPACE_STRU base_ls_space;
		xDesJointPara.m_pMainJg1 = (CLDSLineAngle*)console.FromPartHandle(m_pJoint->base_jg_handle_arr[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		xDesJointPara.m_pMainJg2 = (CLDSLineAngle*)console.FromPartHandle(m_pJoint->base_jg_handle_arr[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		xDesJointPara.m_pMainJg1->SetModified();
		xDesJointPara.m_pMainJg2->SetModified();
		xDesJointPara.m_pCurNode = console.FromNodeHandle(m_pJoint->base_node_handle);
		base_wing_wide1 = xDesJointPara.m_pMainJg1->GetWidth();
		base_wing_thick1 = xDesJointPara.m_pMainJg1->GetThick();
		base_wing_wide2 = xDesJointPara.m_pMainJg2->GetWidth();
		base_wing_thick2 = xDesJointPara.m_pMainJg2->GetThick();
		//���а�,���ȱʡ���
		plank_width = __min(base_wing_wide1-base_wing_thick1,
			base_wing_wide2-base_wing_thick2);
		//��׼����Խϱ����Ϊ��׼;���տ��Ǵ˾������壬������޵�壬��ȥ�� WJH-2004.12.15
		//base_wing_thick1 = __min(base_wing_thick1,base_wing_thick2);

		if(UI::blEnableIntermediateUpdateUI)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pJoint->base_node_handle);
		}
		if(blEnableMirCreate)
		{	//���ǶԳƽ�ͷ���(��ԭʼ��ͷ���)
			if(bIntelligent)
			{	//�������
				pJoint->joint_style = g_sysPara.intelligentConnect.m_iSingleAngleJointStyle;
				if(xDesJointPara.m_pMainJg1->pStart->handle==pJoint->base_node_handle)
				{
					ls_group1=xDesJointPara.m_pMainJg1->connectStart;
#ifdef __SMART_DRAW_INC_
					if (pJoint->joint_style > 0)	//˫����˨
							ls_group1.wnConnBoltN = xDesJointPara.m_pMainJg1->connectStart.wnDualShearConnBoltN;// DesJointPara.m_pMainJg1->BoltsNumOn1Wing(TRUE);
#endif
				}
				else
				{
					ls_group1=xDesJointPara.m_pMainJg1->connectEnd;
#ifdef __SMART_DRAW_INC_
					if (pJoint->joint_style > 0)	//˫����˨
						ls_group1.wnConnBoltN = xDesJointPara.m_pMainJg1->connectEnd.wnDualShearConnBoltN;//BoltsNumOn1Wing(FALSE);
#endif
				}
				//��ƽǸֽ�ͷʱʼ�ն���˨����Ϊ˫֫��˨���� wht 10-07-22
				if(toupper(xDesJointPara.m_pMainJg1->layer(1))=='Z')
					ls_group1.wnConnBoltN=max(ls_group1.wnConnBoltN,3);
				else if(toupper(xDesJointPara.m_pMainJg1->layer(1))=='X')
					ls_group1.wnConnBoltN=max(ls_group1.wnConnBoltN,2);
				if (!GetLsSpace(LsSpace1, ls_group1.d))
					logerr.Log("��˨����ѯʧ�ܣ����ʵ0x%X�˼��Ķ�ͷ������˨ֱ��M%d�Ƿ����", xDesJointPara.m_pMainJg1->handle, ls_group1.d);
				if(xDesJointPara.m_pMainJg2->pStart->handle==pJoint->base_node_handle)
				{
					ls_group2=xDesJointPara.m_pMainJg2->connectStart;
#ifdef __SMART_DRAW_INC_
					if (pJoint->joint_style > 0)	//˫����˨
						ls_group2.wnConnBoltN=xDesJointPara.m_pMainJg2->connectStart.wnDualShearConnBoltN;//BoltsNumOn1Wing(TRUE);
#endif
				}
				else
				{
					ls_group2=xDesJointPara.m_pMainJg2->connectEnd;
#ifdef __SMART_DRAW_INC_
					if (pJoint->joint_style > 0)	//˫����˨
						ls_group2.wnConnBoltN = xDesJointPara.m_pMainJg2->connectEnd.wnDualShearConnBoltN;// BoltsNumOn1Wing(FALSE);
#endif
				}
				if(toupper(xDesJointPara.m_pMainJg2->layer(1))=='Z')
					ls_group2.wnConnBoltN=max(ls_group2.wnConnBoltN,3);
				else if(toupper(xDesJointPara.m_pMainJg2->layer(1))=='X')
					ls_group2.wnConnBoltN=max(ls_group2.wnConnBoltN,2);
				if (!GetLsSpace(LsSpace2, ls_group2.d))
					logerr.Log("��˨����ѯʧ�ܣ����ʵ0x%X�˼��Ķ�ͷ������˨ֱ��M%d�Ƿ����", xDesJointPara.m_pMainJg2->handle, ls_group2.d);
				//��ʼ��׼�� wht 10-01-18
				getjgzj(jgzj1,base_wing_wide1);
				getjgzj(jgzj2,base_wing_wide2);
				INTELLI_JOINT_CONNECT intelli;
				LOCALE_PROPITEM* pUdfLocaleItem=gxPortalLocalizer.GetLocaleItem("UdfIntelliAngleJoint");
				if(pUdfLocaleItem!=NULL&&pUdfLocaleItem->bVal)
				{
					intelli.blPostAngle=xDesJointPara.m_pMainJg1->IsPostRod()!=0;
					intelli.pairangles[0].ciAngleThick=(BYTE)ftoi(xDesJointPara.m_pMainJg1->size_thick);
					intelli.pairangles[0].wiAngleWidth=(WORD)ftoi(xDesJointPara.m_pMainJg1->size_wide);
					intelli.pairangles[0].cnMinBoltN=(BYTE)ls_group1.wnConnBoltN;
					intelli.pairangles[0].cnRows=ls_group1.wnConnBoltN >= 5?2:1;
					intelli.pairangles[0].ciBoltD=(BYTE)ls_group1.d;
					intelli.pairangles[1].ciAngleThick=(BYTE)ftoi(xDesJointPara.m_pMainJg2->size_thick);
					intelli.pairangles[1].wiAngleWidth=(WORD)ftoi(xDesJointPara.m_pMainJg2->size_wide);
					intelli.pairangles[1].cnMinBoltN=(BYTE)ls_group2.wnConnBoltN;
					intelli.pairangles[1].cnRows=ls_group2.wnConnBoltN >= 5?2:1;
					intelli.pairangles[1].ciBoltD=(BYTE)ls_group2.d;
					if (intelli.DesignJointConnect())
					{
						ls_group1.wnConnBoltN=intelli.pairangles[0].cnMinBoltN;
						ls_group2.wnConnBoltN=intelli.pairangles[1].cnMinBoltN;
						ls_group1.d=intelli.pairangles[0].ciBoltD;
						ls_group2.d=intelli.pairangles[1].ciBoltD;
						if (intelli.blRowsSpecified)
						{
							ls_group1.rows=intelli.pairangles[0].cnRows;
							ls_group2.rows=intelli.pairangles[1].cnRows;
						}
					}
				}
				//���ݻ�ͼ�涨��ͷ����Ӧ��ѭһ�����򣬽���Ӧ���ƽ�ͷ��˨������
				//������ͷ��ͷ���Ȳ�ӦС�ڱ����ӹ���֫���2��(��L��2B)
				//˫����ͷ��ͷ���Ȳ�ӦС�ڱ����ӹ���֫���1.5��(��L��1.5B)
				double dfWrapLenFactor=g_sysPara.intelligentConnect.m_iSingleAngleJointStyle==0?2.0:1.5;
				ls_group1.wnConnBoltN=CalJointConnBoltN(xDesJointPara.m_pMainJg1->size_wide*dfWrapLenFactor,LsSpace1,ls_group1.wnConnBoltN,jgzj1.g2>0);
				ls_group2.wnConnBoltN=CalJointConnBoltN(xDesJointPara.m_pMainJg2->size_wide*dfWrapLenFactor,LsSpace2,ls_group2.wnConnBoltN,jgzj2.g2>0);
				if (!intelli.blRowsSpecified)
				{
					if (ls_group1.wnConnBoltN >= 5 && jgzj1.g1 > 0 && jgzj1.g2 > 0)
						ls_group1.rows = 2;
					if (ls_group2.wnConnBoltN >= 5 && jgzj2.g1 > 0 && jgzj2.g2 > 0)
						ls_group2.rows = 2;
				}
			}
			else
			{	//������˨���������Լ����
				CJointBaseJg base_jg_info_dlg;
				LSSPACE_STRU LsSpace;
				if(xDesJointPara.m_pMainJg2->feature==0)
				{
					base_jg_info_dlg.m_nLsN = xDesJointPara.m_pMainJg2->wnStartConnBoltN;
					base_jg_info_dlg.m_nLsRows = xDesJointPara.m_pMainJg2->connectStart.rows-1;
					base_jg_info_dlg.m_sLsGuiGe.Format("%d",xDesJointPara.m_pMainJg2->connectStart.d);
					base_jg_info_dlg.m_sJgGuiGe.Format("L%.0fX%.0f",base_wing_wide2,base_wing_thick2);
					GetLsSpace(LsSpace,xDesJointPara.m_pMainJg2->connectStart.d);
					base_jg_info_dlg.m_nLsEndSpace=LsSpace.EndSpace;
					if(base_jg_info_dlg.m_nLsRows==0)
						base_jg_info_dlg.m_nLsSpace=LsSpace.SingleRowSpace;
					else
						base_jg_info_dlg.m_nLsSpace=LsSpace.doubleRowSpace;
				}
				else
				{
					base_jg_info_dlg.m_nLsN = xDesJointPara.m_pMainJg2->wnEndConnBoltN;
					base_jg_info_dlg.m_nLsRows = xDesJointPara.m_pMainJg2->connectEnd.rows-1;
					base_jg_info_dlg.m_sLsGuiGe.Format("%d",xDesJointPara.m_pMainJg2->connectEnd.d);
					base_jg_info_dlg.m_sJgGuiGe.Format("L%.0fX%.0f",base_wing_wide2,base_wing_thick2);
					GetLsSpace(LsSpace,xDesJointPara.m_pMainJg2->connectEnd.d);
					base_jg_info_dlg.m_nLsEndSpace=LsSpace.EndSpace;
					if(base_jg_info_dlg.m_nLsRows==0)
						base_jg_info_dlg.m_nLsSpace=LsSpace.SingleRowSpace;
					else
						base_jg_info_dlg.m_nLsSpace=LsSpace.doubleRowSpace;
				}
				if(UI::blEnableIntermediateUpdateUI)
					g_pSolidDraw->SetEntSnapStatus(xDesJointPara.m_pMainJg2->handle);
				if(base_jg_info_dlg.DoModal()==IDOK)
				{
					ls_group2.wnConnBoltN = base_jg_info_dlg.m_nLsN;
					ls_group2.uiLegacyN&= 0x0000ffff;	//
					ls_group2.rows = base_jg_info_dlg.m_nLsRows+1;
					if(xDesJointPara.m_pMainJg2->feature==0)
					{
						xDesJointPara.m_pMainJg2->SetBoltsNumOn1Wing(base_jg_info_dlg.m_nLsN,TRUE);
						xDesJointPara.m_pMainJg2->connectStart.rows = base_jg_info_dlg.m_nLsRows+1;
						sscanf(base_jg_info_dlg.m_sLsGuiGe,"%d",&xDesJointPara.m_pMainJg2->connectStart.d);
						ls_group2.d=xDesJointPara.m_pMainJg2->connectStart.d;
					}
					else
					{
						xDesJointPara.m_pMainJg2->SetBoltsNumOn1Wing(base_jg_info_dlg.m_nLsN,FALSE);
						xDesJointPara.m_pMainJg2->connectEnd.rows = base_jg_info_dlg.m_nLsRows+1;
						sscanf(base_jg_info_dlg.m_sLsGuiGe,"%d",&xDesJointPara.m_pMainJg2->connectEnd.d);
						ls_group2.d=xDesJointPara.m_pMainJg2->connectEnd.d;
					}
					//�õ���˨����
					LsSpace2.EndSpace=base_jg_info_dlg.m_nLsEndSpace;
					if(base_jg_info_dlg.m_nLsRows==0)
						LsSpace2.SingleRowSpace=base_jg_info_dlg.m_nLsSpace;
					else
						LsSpace2.doubleRowSpace=base_jg_info_dlg.m_nLsSpace;
				}
				else
					return FALSE;
				if(UI::blEnableIntermediateUpdateUI)
					g_pSolidDraw->SetEntSnapStatus(xDesJointPara.m_pMainJg2->handle,FALSE);
				
				if(xDesJointPara.m_pMainJg1->feature==0)
				{
					base_jg_info_dlg.m_nLsN = xDesJointPara.m_pMainJg1->wnStartConnBoltN;
					base_jg_info_dlg.m_nLsRows = xDesJointPara.m_pMainJg1->connectStart.rows-1;
					base_jg_info_dlg.m_sLsGuiGe.Format("%d",xDesJointPara.m_pMainJg1->connectStart.d);
					base_jg_info_dlg.m_sJgGuiGe.Format("L%.0fX%.0f",base_wing_wide1,base_wing_thick1);
					//base_ls_d = xDesJointPara.m_pMainJg1->connectStart.d;
					GetLsSpace(LsSpace,xDesJointPara.m_pMainJg1->connectStart.d);
					base_jg_info_dlg.m_nLsEndSpace=LsSpace.EndSpace;
					if(base_jg_info_dlg.m_nLsRows==0)
						base_jg_info_dlg.m_nLsSpace=LsSpace.SingleRowSpace;
					else
						base_jg_info_dlg.m_nLsSpace=LsSpace.doubleRowSpace;
				}
				else
				{
					base_jg_info_dlg.m_nLsN = xDesJointPara.m_pMainJg1->wnEndConnBoltN;
					base_jg_info_dlg.m_nLsRows = xDesJointPara.m_pMainJg1->connectEnd.rows-1;
					base_jg_info_dlg.m_sLsGuiGe.Format("%d",xDesJointPara.m_pMainJg1->connectEnd.d);
					base_jg_info_dlg.m_sJgGuiGe.Format("L%.0fX%.0f",base_wing_wide1,base_wing_thick1);
					//base_ls_d = xDesJointPara.m_pMainJg1->connectEnd.d;
					GetLsSpace(LsSpace,xDesJointPara.m_pMainJg1->connectEnd.d);
					base_jg_info_dlg.m_nLsEndSpace=LsSpace.EndSpace;
					if(base_jg_info_dlg.m_nLsRows==0)
						base_jg_info_dlg.m_nLsSpace=LsSpace.SingleRowSpace;
					else
						base_jg_info_dlg.m_nLsSpace=LsSpace.doubleRowSpace;
				}
				if(UI::blEnableIntermediateUpdateUI)
					g_pSolidDraw->SetEntSnapStatus(xDesJointPara.m_pMainJg1->handle);
				if(base_jg_info_dlg.DoModal()==IDOK)
				{
					ls_group1.wnConnBoltN = base_jg_info_dlg.m_nLsN;
					ls_group1.uiLegacyN &= 0x0000ffff;	//
					ls_group1.rows = base_jg_info_dlg.m_nLsRows+1;
					if(xDesJointPara.m_pMainJg1->feature==0)
					{
						xDesJointPara.m_pMainJg1->SetBoltsNumOn1Wing(base_jg_info_dlg.m_nLsN,TRUE);
						xDesJointPara.m_pMainJg1->connectStart.rows = base_jg_info_dlg.m_nLsRows+1;
						sscanf(base_jg_info_dlg.m_sLsGuiGe,"%d",&xDesJointPara.m_pMainJg1->connectStart.d);
						//GetLsSpace(base_ls_space,xDesJointPara.m_pMainJg1->connectStart.d);
						//base_ls_d = xDesJointPara.m_pMainJg1->connectStart.d;
						ls_group1.d = xDesJointPara.m_pMainJg1->connectStart.d;
					}
					else
					{
						xDesJointPara.m_pMainJg1->SetBoltsNumOn1Wing(base_jg_info_dlg.m_nLsN,FALSE);
						xDesJointPara.m_pMainJg1->connectEnd.rows = base_jg_info_dlg.m_nLsRows+1;
						sscanf(base_jg_info_dlg.m_sLsGuiGe,"%d",&xDesJointPara.m_pMainJg1->connectEnd.d);
						//GetLsSpace(base_ls_space,xDesJointPara.m_pMainJg1->connectEnd.d);
						//base_ls_d = xDesJointPara.m_pMainJg1->connectEnd.d;
						ls_group1.d = xDesJointPara.m_pMainJg1->connectEnd.d;
					}
					//�õ���˨����
					LsSpace1.EndSpace=base_jg_info_dlg.m_nLsEndSpace;
					if(base_jg_info_dlg.m_nLsRows==0)
						LsSpace1.SingleRowSpace=base_jg_info_dlg.m_nLsSpace;
					else
						LsSpace1.doubleRowSpace=base_jg_info_dlg.m_nLsSpace;
				}
				else
					return FALSE;
				if(UI::blEnableIntermediateUpdateUI)
					g_pSolidDraw->SetEntSnapStatus(xDesJointPara.m_pMainJg1->handle,FALSE);
				//�����ýǸ�׼�ݶԻ�����������Ǹֲ���֮ǰ���������׼�ݼ������߲��� wht 16-08-05
				getjgzj(zhunjudlg.jgzj1,base_wing_wide1);
				getjgzj(zhunjudlg.jgzj2,base_wing_wide2);
				JGZJ jgzjX,jgzjY;
				jgzjX=xDesJointPara.m_pMainJg1->GetZhunJu('X');
				jgzjY=xDesJointPara.m_pMainJg1->GetZhunJu('Y');
				if(jgzjX.g==jgzjY.g&&jgzjX.g1==jgzjY.g1&&jgzjX.g2==jgzjY.g2&&jgzjX.g3==jgzjY.g3)
					zhunjudlg.jgzj1=jgzjX;	//��ֹ�Ǹ�˫֫���趨������׼���,��ͷ׼�����м�������˨׼�಻һ�� wjh-2017.11.23
				jgzjX=xDesJointPara.m_pMainJg2->GetZhunJu('X');
				jgzjY=xDesJointPara.m_pMainJg2->GetZhunJu('Y');
				if(jgzjX.g==jgzjY.g&&jgzjX.g1==jgzjY.g1&&jgzjX.g2==jgzjY.g2&&jgzjX.g3==jgzjY.g3)
					zhunjudlg.jgzj2=jgzjX;

				zhunjudlg.m_sJg1GuiGe.Format("%.0fX%.0f",base_wing_wide1,base_wing_thick1);
				zhunjudlg.m_sJg2GuiGe.Format("%.0fX%.0f",base_wing_wide2,base_wing_thick2);
				if(zhunjudlg.DoModal()==IDOK)
				{	//��ʼ���Ǹ�׼��
					jgzj1  = zhunjudlg.jgzj1;
					jgzj2  = zhunjudlg.jgzj2;
				}
				else
					return false;
			}
			CLDSNode *pNode = console.FromNodeHandle(m_pJoint->base_node_handle);
			f3dPoint startPos,endPos,startPos2,endPos2;
			BOOL bRetcode=FALSE;
			//��˨���Ͷ˾����Ϊ�Ǳ�׼���˾�
			int ls_group_len = 0;
			if (ls_group1.rows == 1)
				ls_group_len = 2 * LsSpace1.EndSpace + (ls_group1.wnConnBoltN - 1)*LsSpace1.SingleRowSpace;
			else	//˫����˨
				ls_group_len = 2 * LsSpace1.EndSpace + (ls_group1.wnConnBoltN - 1)*LsSpace1.doubleRowSpace / 2;
			int wrap_ls_len = (ls_group1.rows % 2)*LsSpace1.SingleRowSpace / 2 + ls_group_len;
			if (pJoint->blIntelliCalOddment)
			{
				if (pNode->handle == xDesJointPara.m_pMainJg1->pStart->handle)
				{
					double oddment = xDesJointPara.m_pMainJg1->startOdd();
					startPos.Set();
					endPos.Set();
					xDesJointPara.m_pMainJg1->SetStartOdd(1000);	//Ԥ��һ�ܴ�����ͷ�Ա�����������Ӱ�ı߽罻��
					UCS_STRU angle_ucs;
					xDesJointPara.m_pMainJg1->getUCS(angle_ucs);
					int kaihestyle = xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1;
					xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1 = 0;
					if (CalKaiHeStartEndPos(xDesJointPara.m_pMainJg1, pNode, startPos, endPos, angle_ucs, bIntelligent))
						bRetcode = TRUE;
					xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1 = 1;
					if (CalKaiHeStartEndPos(xDesJointPara.m_pMainJg1, pNode, startPos2, endPos2, angle_ucs, bIntelligent))
					{
						if (startPos2.z < startPos.z)
							startPos = startPos2;
						bRetcode = TRUE;
					}
					xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1 = kaihestyle;
					xDesJointPara.m_pMainJg1->SetStartOdd(oddment); //��ԭ����ͷ
				}
				else //if(pNode==xDesJointPara.m_pMainJg1->pEnd)
				{
					double oddment = xDesJointPara.m_pMainJg1->endOdd();
					startPos.z = endPos.z = DISTANCE(xDesJointPara.m_pMainJg1->Start(), xDesJointPara.m_pMainJg1->End());
					xDesJointPara.m_pMainJg1->SetEndOdd(1000);	//Ԥ��һ�ܴ�����ͷ�Ա�����������Ӱ�ı߽罻��
					UCS_STRU angle_ucs;
					xDesJointPara.m_pMainJg1->getUCS(angle_ucs);
					int kaihestyle = xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1;
					xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1 = 0;
					if (CalKaiHeStartEndPos(xDesJointPara.m_pMainJg1, pNode, startPos, endPos, angle_ucs, bIntelligent))
						bRetcode = TRUE;
					xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1 = 1;
					if (CalKaiHeStartEndPos(xDesJointPara.m_pMainJg1, pNode, startPos2, endPos2, angle_ucs, bIntelligent))
					{
						if (endPos2.z > endPos.z)
							endPos = endPos2;
						bRetcode = TRUE;
					}
					xDesJointPara.m_pMainJg1->kaihe_base_wing_x0_y1 = kaihestyle;
					xDesJointPara.m_pMainJg1->SetEndOdd(oddment); //��ԭ����ͷ
				}
				if (bRetcode)	//����ɹ�
				{
					UCS_STRU ucs;
					xDesJointPara.m_pMainJg1->getUCS(ucs);
					if (pNode == xDesJointPara.m_pMainJg1->pStart)
						JointDesDlg.m_nUpOffset = (int)-startPos.z + deita;
					else
					{
						double len = DISTANCE(xDesJointPara.m_pMainJg1->Start(), xDesJointPara.m_pMainJg1->End());
						JointDesDlg.m_nUpOffset = (int)(endPos.z - len) + deita;
					}
				}
				else
					JointDesDlg.m_nUpOffset = S1 + deita;
				JointDesDlg.m_nUpOffset += wrap_ls_len;	//��ƫ��ֵת��Ϊ����ͷ
				long remainder = 50 - JointDesDlg.m_nUpOffset % 50;
				if (remainder < 50)
					JointDesDlg.m_nUpOffset += remainder;
			}
			else if (pNode->handle == xDesJointPara.m_pMainJg1->pStart->handle)
				JointDesDlg.m_nUpOffset = ftoi(xDesJointPara.m_pMainJg1->dfStartOdd);
			else if (pNode->handle == xDesJointPara.m_pMainJg1->pEnd->handle)
				JointDesDlg.m_nUpOffset = ftoi(xDesJointPara.m_pMainJg1->dfEndOdd);
			JointDesDlg.m_nPlankWidthE=ftol(plank_width);
			JointDesDlg.m_nPlankWidthF=ftol(plank_width);
			JointDesDlg.m_nPlankWidthG=ftol(plank_width)-abs(ftol(base_wing_thick2-base_wing_thick1));
			JointDesDlg.m_nPlankWidthH=ftol(plank_width)-abs(ftol(base_wing_thick2-base_wing_thick1));
			if(fabs(base_wing_thick1-base_wing_thick2)>=2)
			{
				JointDesDlg.m_nPlankThickG=JointDesDlg.m_nPlankThickH=ftol(fabs(base_wing_thick1-base_wing_thick2));
				JointDesDlg.m_bHavePadPlank=TRUE;
			}
			else
			{
				JointDesDlg.m_nPlankThickG=JointDesDlg.m_nPlankThickH==0;
				JointDesDlg.m_bHavePadPlank=FALSE;
			}
			if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(bIntelligent)
					JointDesDlg.m_iJointStyle=g_sysPara.intelligentConnect.m_iSingleAngleJointStyle;	//���Ǹ�
				else
					JointDesDlg.m_iJointStyle=0;
				JointDesDlg.m_iBaseJgStyle=0;	//���Ǹ�
			}
			else
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)xDesJointPara.m_pMainJg1;
				if(pGroupJg->group_style==0)
				{
					JointDesDlg.m_iBaseJgStyle=1;//�Խ���ϽǸ�
					JointDesDlg.m_iJointStyle=4;
				}
				else if(pGroupJg->group_style==2)
				{
					JointDesDlg.m_iBaseJgStyle=2;//ʮ����ϽǸ�
					JointDesDlg.m_iJointStyle=8;
				}
				else
				{
					if (UI::blDisableIntermediateMessageBox)
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("program does not support T type combined angle joints!");
					else
						AfxMessageBox("program does not support T type combined angle joints!");
#else
						logerr.Log("�����ݲ�֧��T����ϽǸֽ�ͷ0x%X-0x%X��",xDesJointPara.m_pMainJg1->handle,xDesJointPara.m_pMainJg2->handle);
					else
						AfxMessageBox(CXhChar200("�����ݲ�֧��T����ϽǸֽ�ͷ0x%X-0x%X��",xDesJointPara.m_pMainJg1->handle,xDesJointPara.m_pMainJg2->handle));
#endif
					return FALSE;
				}
			}
			JointDesDlg.m_sJgAGuige.Format("%.0fX%.0f",xDesJointPara.m_pMainJg1->GetWidth(),xDesJointPara.m_pMainJg1->GetThick());
			JointDesDlg.m_sJgBGuige=JointDesDlg.m_sJgCGuige=JointDesDlg.m_sJgDGuige=JointDesDlg.m_sJgAGuige;
			if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_GROUPLINEANGLE)
				JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=(int)(((CLDSGroupLineAngle*)xDesJointPara.m_pMainJg1)->jg_space-g_sysPara.m_nGroupJgMachiningGap);
			else
				JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=(int)xDesJointPara.m_pMainJg1->GetThick();
			JointDesDlg.base_jgzj1=jgzj1;
			JointDesDlg.base_jgzj2=jgzj2;
			JointDesDlg.LsSpace1=LsSpace1;
			JointDesDlg.LsSpace2=LsSpace2;
			if(bIntelligent)
			{	//������ƽǸֽ�ͷ
				int joint_type=-1;
				JG_JOINT_PARA joint_para;
				JointDesDlg.m_iMirMsg=0;	//������ƽǸֽ�ͷʱ�����öԳ�
				if(JointDesDlg.m_nUpOffset%50>0)	//��ƫ������50Բ��
					JointDesDlg.m_nUpOffset=(JointDesDlg.m_nUpOffset/50+1)*50;
				if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_LINEANGLE)
					JointDesDlg.m_iJointStyle=g_sysPara.intelligentConnect.m_iSingleAngleJointStyle;
				else if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{
					CLDSGroupLineAngle *pTempGroupAngle=(CLDSGroupLineAngle*)xDesJointPara.m_pMainJg1;
					if(pTempGroupAngle->group_style==0)		//�Խ�
						JointDesDlg.m_iJointStyle=g_sysPara.intelligentConnect.m_iTwoAngleJointStyle+4;
					else if(pTempGroupAngle->group_style==2)	//ʮ��
						JointDesDlg.m_iJointStyle=g_sysPara.intelligentConnect.m_iFourAngleJointStyle+8;
					else	//�ݲ�֧��T����ϽǸ�
						return FALSE;
					joint_type=JointDesDlg.m_iJointStyle;
				}
				joint_type=JointDesDlg.m_iJointStyle;
				if(CalJgJointPara(xDesJointPara.m_pMainJg1,xDesJointPara.m_pMainJg2,&joint_para,joint_type))
				{	//�Զ�����Ǹֽ�ͷ���� wht 11-01-25
					if(joint_type<4)	//���Ǹֽ�ͷ����
						JointDesDlg.m_iJointStyle=joint_type;
					JointDesDlg.m_bAdjustWrapPlateLen=FALSE;
					if(JointDesDlg.m_iJointStyle==0)//�����
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
					else if(JointDesDlg.m_iJointStyle==1)
					{	//�������
						JointDesDlg.m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
					}
					else if(JointDesDlg.m_iJointStyle==2)
					{	//�ڰ�������ְ�
						JointDesDlg.m_bAdjustWrapPlateLen=TRUE;	//�����������ʱӦ�Զ���������ְ� wht 13-04-03
						JointDesDlg.m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_nPlankWidthE=JointDesDlg.m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
						JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
						JointDesDlg.m_fPlankEBerWidth=JointDesDlg.m_fPlankFBerWidth=0;		//0��ʾ�Զ��������߲��� wht 16-08-05
					}
					else if(JointDesDlg.m_iJointStyle==3)
					{	//�ڰ��ְ������
						JointDesDlg.m_nPlankWidthE=JointDesDlg.m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
						JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
					}
					else 
					{	//�ڰ�������ְ�
						JointDesDlg.m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_nPlankWidthE=JointDesDlg.m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
						JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
						if(JointDesDlg.m_iJointStyle==6||JointDesDlg.m_iJointStyle==7)
						{
							JointDesDlg.m_sJgCGuige.Format("%.0fX%.0f",xDesJointPara.m_pMainJg1->GetWidth(),xDesJointPara.m_pMainJg2->GetThick());
							JointDesDlg.m_sJgDGuige=JointDesDlg.m_sJgCGuige;
						}
					}
				}
			}
			else
			{
				int joint_type=-1;
				JG_JOINT_PARA joint_para;
				if(CalJgJointPara(xDesJointPara.m_pMainJg1,xDesJointPara.m_pMainJg2,&joint_para,joint_type))
				{	//�Զ�����Ǹֽ�ͷ���� wht 11-01-25
					if(joint_type<4)	//���Ǹֽ�ͷ����
						JointDesDlg.m_iJointStyle=joint_type;
					if(JointDesDlg.m_iJointStyle==0)//�����
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
					else if(JointDesDlg.m_iJointStyle==1)
					{	//�������
						JointDesDlg.m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
					}
					else if(JointDesDlg.m_iJointStyle==2)
					{	//�ڰ�������ְ�
						JointDesDlg.m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_nPlankWidthE=JointDesDlg.m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
						JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
					}
					else if(JointDesDlg.m_iJointStyle==3)
					{	//�ڰ��ְ������
						JointDesDlg.m_nPlankWidthE=JointDesDlg.m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
						JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fOuterJgWidth,joint_para.fOuterJgThick);
					}
					else 
					{	//�ڰ�������ְ�
						JointDesDlg.m_sJgAGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_sJgBGuige.Format("%.0fX%.0f",joint_para.fInnerJgWidth,joint_para.fInnerJgThick);
						JointDesDlg.m_nPlankWidthE=JointDesDlg.m_nPlankWidthF=ftol(joint_para.fOuterPlateWidth);
						JointDesDlg.m_nPlankThickE=JointDesDlg.m_nPlankThickF=ftol(joint_para.fOuterPlateThick);
						if(JointDesDlg.m_iJointStyle==6||JointDesDlg.m_iJointStyle==7)
						{
							JointDesDlg.m_sJgCGuige.Format("%.0fX%.0f",xDesJointPara.m_pMainJg1->GetWidth(),xDesJointPara.m_pMainJg2->GetThick());
							JointDesDlg.m_sJgDGuige=JointDesDlg.m_sJgCGuige;
						}
					}
				}
				JointDesDlg.m_pMainJg1=xDesJointPara.m_pMainJg1;
				JointDesDlg.m_pMainJg2=xDesJointPara.m_pMainJg2;
				if(JointDesDlg.DoModal()!=IDOK)
					return FALSE;
			}
			base_jg1_oddment = JointDesDlg.m_nUpOffset;
			JointDesDlg.m_nUpOffset -= wrap_ls_len;	//������ͷת��Ϊƫ��ֵ
		}

		m_nLsLHigh0RLow1 = JointDesDlg.m_iLsOffsetStyle;
		S1 = JointDesDlg.m_nUpOffset-deita;
		// ��ͷ��ʽ0.����� 1.������� 2.�ڰ�������ְ� 3.������ڰ��ְ�4.�ڰ���X֫�ְ�ض�
		// 5.�ڰ���Y֫�ְ�ض�6.�İ���X֫�ְ�ض�7.�İ���Y֫�ְ�ض�8.ʮ�ֽ�ͷX֫�ְ�ض�9.ʮ�ֽ�ͷY֫�ְ�ض�
		m_pJoint->joint_style = JointDesDlg.m_iJointStyle;
		m_nLsLayStyle = JointDesDlg.m_iLsLayStyle;
		CLDSGroupLineAngle *pGroupMainJg1=NULL,*pGroupMainJg2=NULL;
		if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_GROUPLINEANGLE&&xDesJointPara.m_pMainJg2->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//��Ͻ�ͷ
			pGroupMainJg1=(CLDSGroupLineAngle*)xDesJointPara.m_pMainJg1;
			pGroupMainJg2=(CLDSGroupLineAngle*)xDesJointPara.m_pMainJg2;
			if(pGroupMainJg1->pStart->handle==m_pJoint->base_node_handle)
				pGroupMainJg1->SetStartOdd(base_jg1_oddment);
			else
				pGroupMainJg1->SetEndOdd(base_jg1_oddment);
			
			if(pGroupMainJg2->pStart->handle==m_pJoint->base_node_handle)
				pGroupMainJg2->SetStartOdd(-(base_jg1_oddment+g_sysPara.JgEndDist));
			else
				pGroupMainJg2->SetEndOdd(-(base_jg1_oddment+g_sysPara.JgEndDist));

			xDesJointPara.m_pMainJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg1->son_jg_h[0],CLS_LINEANGLE);
			xDesJointPara.m_pMainJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg2->son_jg_h[0],CLS_LINEANGLE);
			xDesJointPara.m_fJgSpace1=pGroupMainJg1->jg_space;
			xDesJointPara.m_fJgSpace2=pGroupMainJg2->jg_space;
			m_pJoint->base_node_handle=m_pJoint->base_node_handle;
			m_pJoint->joint_style=2;	//��ϽǸֽ�ͷ���������ڰ��������ݰ����Ǹ��ڰ�������ְ��ͷ���
			CreateRefAxisNorm(m_pJoint);
			//--------VVVVVVVV-----��ͷ�����------------------
			if(!DesignJoint())	//�������ʱ����Ҫ��ʾ�޸�׼��Ի��� wht 09-12-29
				return FALSE;
			if(pGroupMainJg2->group_style==0)	//�ԽǶԳ�
				xDesJointPara.m_pMainJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg1->son_jg_h[1],CLS_LINEANGLE);
			else if(pGroupMainJg2->group_style==2)	//ʮ�ֶԳ�
				xDesJointPara.m_pMainJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg1->son_jg_h[2],CLS_LINEANGLE);
			else	//�ݲ�֧��
				return FALSE;
			if(pGroupMainJg2->group_style==0)	//�ԽǶԳ�
				xDesJointPara.m_pMainJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg2->son_jg_h[1],CLS_LINEANGLE);
			else if(pGroupMainJg2->group_style==2)	//ʮ�ֶԳ�
				xDesJointPara.m_pMainJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg2->son_jg_h[2],CLS_LINEANGLE);
			else	//�ݲ�֧��
				return FALSE;

			CreateRefAxisNorm(m_pJoint);
			//--------VVVVVVVV-----��ͷ�����------------------
			//�ȴ���������Ӧ�ĵ��Ǹֽ�ͷ
			if(!DesignJoint())
				return FALSE;
			m_pJoint->base_jg_handle_arr[0]=pGroupMainJg1->handle;
			m_pJoint->base_jg_handle_arr[1]=pGroupMainJg2->handle;

			CLDSPlate *pPlankE=NULL,*pPlankF=NULL;
			CLDSLineAngle *pJgB=NULL,*pJgC=NULL,*pJgD=NULL;
			if(JointDesDlg.m_iJointStyle==6)	//||JointDesDlg.m_iJointStyle==4
			{	//X֫�ض��ڰ���&X֫�ض��������
				pJgC=m_pJoint->AppendWrapJg('C');
				pJgD=m_pJoint->AppendWrapJg('D');
				pJgC->ucs.axis_x =  m_pJoint->ucs.axis_y;
				pJgC->ucs.axis_y = -m_pJoint->ucs.axis_x;
				pJgC->ucs.axis_z =  m_pJoint->ucs.axis_z;
				pJgC->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_y_wing.vector,xDesJointPara.norm_x_wing.vector,JointDesDlg.m_nPlankThickF,0); 
				pJgD->ucs.axis_x = -m_pJoint->ucs.axis_y;
				pJgD->ucs.axis_y =  m_pJoint->ucs.axis_x;
				pJgD->ucs.axis_z =  m_pJoint->ucs.axis_z;
				pJgD->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,JointDesDlg.m_nPlankThickE,0); 

				pJgC->desStartPos.origin_pos_style=pJgC->desEndPos.origin_pos_style=1;
				pJgC->desStartPos.spatialOperationStyle=pJgC->desEndPos.spatialOperationStyle=0;
				pJgC->desStartPos.datumPoint=xDesJointPara.start;
				pJgC->desStartPos.datumPoint.datum_pos_style=1;
				pJgC->desStartPos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
				pJgC->desStartPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
				pJgC->desStartPos.datumPoint.des_para.RODEND.offset_wing=xDesJointPara.norm_x_wing.norm_wing;
				pJgC->desStartPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgC->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				_snprintf(pJgC->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgC->desStartPos.wing_x_offset.SetOffset();
				pJgC->desStartPos.wing_y_offset.SetOffset();
				pJgC->desEndPos.datumPoint=xDesJointPara.end;
				pJgC->desEndPos.datumPoint.datum_pos_style=1;
				pJgC->desEndPos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
				pJgC->desEndPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg2->handle;
				pJgC->desEndPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgC->desEndPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				if(xDesJointPara.m_pMainJg2->pStart==xDesJointPara.m_pCurNode)//ʼ������
					pJgC->desEndPos.datumPoint.des_para.RODEND.offset_wing=0;
				else	//�ն�����
					pJgC->desEndPos.datumPoint.des_para.RODEND.offset_wing=1;
				_snprintf(pJgC->desEndPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgC->desEndPos.wing_x_offset.SetOffset();
				pJgC->desEndPos.wing_y_offset.SetOffset();
				//pJgD->desStartPos.datum_pos_style=pJgD->desEndPos.datum_pos_style=15;
				pJgD->desStartPos.origin_pos_style=pJgD->desEndPos.origin_pos_style=1;
				pJgD->desStartPos.spatialOperationStyle=pJgD->desEndPos.spatialOperationStyle=0;
				pJgD->desStartPos.datumPoint=xDesJointPara.start;
				pJgD->desStartPos.datumPoint.datum_pos_style=1;
				pJgD->desStartPos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
				pJgD->desStartPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
				pJgD->desStartPos.datumPoint.des_para.RODEND.offset_wing=xDesJointPara.norm_y_wing.norm_wing;
				pJgD->desStartPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgD->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				_snprintf(pJgD->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgD->desStartPos.wing_x_offset.SetOffset();
				pJgD->desStartPos.wing_y_offset.SetOffset();
				pJgD->desEndPos.datumPoint=xDesJointPara.end;
				pJgD->desEndPos.datumPoint.datum_pos_style=1;
				pJgD->desEndPos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
				pJgD->desEndPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg2->handle;
				pJgD->desEndPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgD->desEndPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				if(xDesJointPara.m_pMainJg2->pStart==xDesJointPara.m_pCurNode)//ʼ������
					pJgD->desEndPos.datumPoint.des_para.RODEND.offset_wing=1;
				else
					pJgD->desEndPos.datumPoint.des_para.RODEND.offset_wing=0;
				_snprintf(pJgD->desEndPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgD->desEndPos.wing_x_offset.SetOffset();
				pJgD->desEndPos.wing_y_offset.SetOffset();

				pJgC->CalPosition();
				pJgD->CalPosition();
				
				pJgC->des_norm_x.bSpecific=pJgC->des_norm_y.bSpecific=TRUE;
				pJgD->des_norm_x.bSpecific=pJgD->des_norm_y.bSpecific=TRUE;
				pJgC->des_norm_x.spec_norm=xDesJointPara.norm_y_wing;
				pJgC->des_norm_x.spec_norm.direction=0;
				pJgC->des_norm_y.spec_norm=xDesJointPara.norm_x_wing;
				pJgC->des_norm_y.spec_norm.direction=1;
				pJgD->des_norm_x.spec_norm=xDesJointPara.norm_y_wing;
				pJgD->des_norm_x.spec_norm.direction=1;
				pJgD->des_norm_y.spec_norm=xDesJointPara.norm_x_wing;
				pJgD->des_norm_y.spec_norm.direction=0;
				pJgC->cal_x_wing_norm();
				pJgC->cal_y_wing_norm();
				pJgD->cal_x_wing_norm();
				pJgD->cal_y_wing_norm();
				//pJgC->SetStart(pJgC->ucs.origin);
				//pJgD->SetStart(pJgD->ucs.origin);
				//pJgC->SetEnd(pJgC->Start()+pJgC->ucs.axis_z*m_pJoint->length);
				//pJgD->SetEnd(pJgD->Start()+pJgD->ucs.axis_z*m_pJoint->length);
				double wing_wide,wing_thick;
				restore_JG_guige(JointDesDlg.m_sJgCGuige.GetBuffer(),wing_wide,wing_thick);
				pJgC->set_wing_wide(wing_wide);
				pJgC->set_wing_thick(wing_thick);
				restore_JG_guige(JointDesDlg.m_sJgDGuige.GetBuffer(),wing_wide,wing_thick);
				pJgD->set_wing_wide(wing_wide);
				pJgD->set_wing_thick(wing_thick);
				pJgC->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgCMat).cBriefMark;
				pJgD->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgDMat).cBriefMark;
				pJgC->SetPartNo(JointDesDlg.m_sPartNoC.GetBuffer(0));
				pJgD->SetPartNo(JointDesDlg.m_sPartNoD.GetBuffer(0));
				pJgC->iSeg=SEGI(JointDesDlg.m_sCSegI.GetBuffer());//C�Ǹֶκ�
				pJgD->iSeg=SEGI(JointDesDlg.m_sDSegI.GetBuffer());//D�Ǹֶκ�
				PART_ITEM *pIA=m_pJoint->FromPartId('A');
				PART_ITEM *pIB=m_pJoint->FromPartId('B');
				CLDSLineAngle *pJgA=(CLDSLineAngle*)console.FromPartHandle(pIA->h,CLS_LINEANGLE);
				CLDSLineAngle *pJgB=(CLDSLineAngle*)console.FromPartHandle(pIB->h,CLS_LINEANGLE);
				pJgB->SetPartNo(JointDesDlg.m_sPartNoB.GetBuffer(0));
				CLsRef *pLsRef;
				for(pLsRef=pJgA->GetFirstLsRef();pLsRef;pLsRef=pJgA->GetNextLsRef())
				{
					int x_wing0_y_wing1;
					pJgA->IsLsInWing(pLsRef->GetLsPtr(),&x_wing0_y_wing1);
					if(x_wing0_y_wing1==0)	//X֫����˨
					{
						pJgD->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgD->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
					else if(x_wing0_y_wing1==1)	//Y֫����˨
					{
						pJgC->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgC->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
				}
				for(pLsRef=pJgB->GetFirstLsRef();pLsRef;pLsRef=pJgB->GetNextLsRef())
				{
					int x_wing0_y_wing1;
					pJgB->IsLsInWing(pLsRef->GetLsPtr(),&x_wing0_y_wing1);
					if(x_wing0_y_wing1==0)	//X֫����˨
					{
						pJgC->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgC->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
					else if(x_wing0_y_wing1==1)	//Y֫����˨
					{
						pJgD->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgD->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
				}
			}
			else if(JointDesDlg.m_iJointStyle==7)	//JointDesDlg.m_iJointStyle==5
			{	//Y֫�ض��ڰ���&Y֫�ض��������
				pJgC=m_pJoint->AppendWrapJg('C');
				pJgD=m_pJoint->AppendWrapJg('D');
				pJgC->ucs.axis_x =  m_pJoint->ucs.axis_y;
				pJgC->ucs.axis_y = -m_pJoint->ucs.axis_x;
				pJgC->ucs.axis_z =  m_pJoint->ucs.axis_z;
				pJgC->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_y_wing.vector,xDesJointPara.norm_x_wing.vector,JointDesDlg.m_nPlankThickF,0); 
				pJgD->ucs.axis_x = -m_pJoint->ucs.axis_y;
				pJgD->ucs.axis_y =  m_pJoint->ucs.axis_x;
				pJgD->ucs.axis_z =  m_pJoint->ucs.axis_z;
				pJgD->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,JointDesDlg.m_nPlankThickE,0); 

				//pJgC->desStartPos.datum_pos_style=pJgC->desEndPos.datum_pos_style=15;
				pJgC->desStartPos.origin_pos_style=pJgC->desEndPos.origin_pos_style=1;
				pJgC->desStartPos.spatialOperationStyle=pJgC->desEndPos.spatialOperationStyle=0;
				pJgC->desStartPos.datumPoint=xDesJointPara.start;
				pJgC->desStartPos.datumPoint.datum_pos_style=1;
				pJgC->desStartPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
				pJgC->desStartPos.datumPoint.des_para.RODEND.offset_wing=xDesJointPara.norm_x_wing.norm_wing;
				pJgC->desStartPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgC->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				_snprintf(pJgC->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgC->desStartPos.wing_x_offset.SetOffset();
				pJgC->desStartPos.wing_y_offset.SetOffset();
				pJgC->desEndPos.datumPoint=xDesJointPara.end;
				pJgC->desEndPos.datumPoint.datum_pos_style=1;
				pJgC->desEndPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg2->handle;
				pJgC->desEndPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgC->desEndPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				if(xDesJointPara.m_pMainJg2->pStart==xDesJointPara.m_pCurNode)//ʼ������
					pJgC->desEndPos.datumPoint.des_para.RODEND.offset_wing=0;
				else	//�ն�����
					pJgC->desEndPos.datumPoint.des_para.RODEND.offset_wing=1;
				_snprintf(pJgC->desEndPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgD->desEndPos.wing_x_offset.SetOffset();
				pJgD->desEndPos.wing_y_offset.SetOffset();

				//pJgD->desStartPos.datum_pos_style=pJgD->desEndPos.datum_pos_style=15;
				pJgD->desStartPos.origin_pos_style=pJgD->desEndPos.origin_pos_style=1;
				pJgD->desStartPos.spatialOperationStyle=pJgD->desEndPos.spatialOperationStyle=0;
				pJgD->desStartPos.datumPoint=xDesJointPara.start;
				pJgD->desStartPos.datumPoint.datum_pos_style=1;
				pJgD->desStartPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
				pJgD->desStartPos.datumPoint.des_para.RODEND.offset_wing=xDesJointPara.norm_y_wing.norm_wing;
				pJgD->desStartPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgD->desStartPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				_snprintf(pJgD->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgD->desStartPos.wing_x_offset.SetOffset();
				pJgD->desStartPos.wing_y_offset.SetOffset();
				pJgD->desEndPos.datumPoint=xDesJointPara.end;
				pJgD->desEndPos.datumPoint.datum_pos_style=1;
				pJgD->desEndPos.datumPoint.des_para.RODEND.hRod=xDesJointPara.m_pMainJg2->handle;
				pJgD->desEndPos.datumPoint.des_para.RODEND.wing_offset_style=4;
				pJgD->desEndPos.datumPoint.des_para.RODEND.wing_offset_dist=0;
				if(xDesJointPara.m_pMainJg2->pStart==xDesJointPara.m_pCurNode)//ʼ������
					pJgD->desEndPos.datumPoint.des_para.RODEND.offset_wing=1;
				else
					pJgD->desEndPos.datumPoint.des_para.RODEND.offset_wing=0;
				_snprintf(pJgD->desEndPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"%.0f",pGroupMainJg1->jg_space);
				pJgD->desEndPos.wing_x_offset.SetOffset();
				pJgD->desEndPos.wing_y_offset.SetOffset();

				pJgC->CalPosition();
				pJgD->CalPosition();

				pJgC->des_norm_x.bSpecific=pJgC->des_norm_y.bSpecific=TRUE;
				pJgD->des_norm_x.bSpecific=pJgD->des_norm_y.bSpecific=TRUE;
				pJgC->des_norm_x.spec_norm=xDesJointPara.norm_y_wing;
				pJgC->des_norm_x.spec_norm.direction=0;
				pJgC->des_norm_y.spec_norm=xDesJointPara.norm_x_wing;
				pJgC->des_norm_y.spec_norm.direction=1;
				pJgD->des_norm_x.spec_norm=xDesJointPara.norm_y_wing;
				pJgD->des_norm_x.spec_norm.direction=1;
				pJgD->des_norm_y.spec_norm=xDesJointPara.norm_x_wing;
				pJgD->des_norm_y.spec_norm.direction=0;
				pJgC->cal_x_wing_norm();
				pJgC->cal_y_wing_norm();
				pJgD->cal_x_wing_norm();
				pJgD->cal_y_wing_norm();
				//pJgC->SetStart(pJgC->ucs.origin);
				//pJgD->SetStart(pJgD->ucs.origin);
				//pJgC->SetEnd(pJgC->Start()+pJgC->ucs.axis_z*m_pJoint->length);
				//pJgD->SetEnd(pJgD->Start()+pJgD->ucs.axis_z*m_pJoint->length);
				double wing_wide,wing_thick;
				restore_JG_guige(JointDesDlg.m_sJgCGuige.GetBuffer(),wing_wide,wing_thick);
				pJgC->set_wing_wide(wing_wide);
				pJgC->set_wing_thick(wing_thick);
				restore_JG_guige(JointDesDlg.m_sJgDGuige.GetBuffer(),wing_wide,wing_thick);
				pJgD->set_wing_wide(wing_wide);
				pJgD->set_wing_thick(wing_thick);
				pJgC->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgCMat).cBriefMark;
				pJgD->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgDMat).cBriefMark;
				pJgC->SetPartNo(JointDesDlg.m_sPartNoC.GetBuffer(0));
				pJgD->SetPartNo(JointDesDlg.m_sPartNoD.GetBuffer(0));
				pJgC->iSeg=SEGI(JointDesDlg.m_sCSegI.GetBuffer());//C�Ǹֶκ�
				pJgD->iSeg=SEGI(JointDesDlg.m_sDSegI.GetBuffer());//D�Ǹֶκ�
				PART_ITEM *pIA=m_pJoint->FromPartId('A');
				PART_ITEM *pIB=m_pJoint->FromPartId('B');
				CLDSLineAngle *pJgA=(CLDSLineAngle*)console.FromPartHandle(pIA->h,CLS_LINEANGLE);
				CLDSLineAngle *pJgB=(CLDSLineAngle*)console.FromPartHandle(pIB->h,CLS_LINEANGLE);
				pJgB->SetPartNo(JointDesDlg.m_sPartNoB.GetBuffer(0));
				CLsRef *pLsRef;
				for(pLsRef=pJgA->GetFirstLsRef();pLsRef;pLsRef=pJgA->GetNextLsRef())
				{
					int x_wing0_y_wing1;
					pJgA->IsLsInWing(pLsRef->GetLsPtr(),&x_wing0_y_wing1);
					if(x_wing0_y_wing1==0)	//X֫����˨
					{
						(*pLsRef)->add_L(pJgD->GetThick());
						pJgD->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgD->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
					else if(x_wing0_y_wing1==1)	//Y֫����˨
					{
						(*pLsRef)->add_L(pJgC->GetThick());
						pJgC->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgC->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
				}
				for(pLsRef=pJgB->GetFirstLsRef();pLsRef;pLsRef=pJgB->GetNextLsRef())
				{
					int x_wing0_y_wing1;
					pJgB->IsLsInWing(pLsRef->GetLsPtr(),&x_wing0_y_wing1);
					if(x_wing0_y_wing1==0)	//X֫����˨
					{
						pJgC->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgC->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
					else if(x_wing0_y_wing1==1)	//Y֫����˨
					{
						pJgD->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgD->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
				}
			}
			else if(JointDesDlg.m_iJointStyle==8||JointDesDlg.m_iJointStyle==9)
			{	//ʮ�����A or B-->X or Y֫�ְ�ض�
				CLDSLineAngle *pSonMainJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg1->son_jg_h[0],CLS_LINEANGLE);
				CLDSLineAngle *pSonMainJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg1->son_jg_h[2],CLS_LINEANGLE);
				CLDSLineAngle *pSonJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg1->son_jg_h[1],CLS_LINEANGLE);	//��ӦC	MainJg1
				CLDSLineAngle *pSonJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg1->son_jg_h[3],CLS_LINEANGLE);	//��ӦD	MainJg1
				CLDSLineAngle *pSonJg3=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg2->son_jg_h[1],CLS_LINEANGLE);	//��ӦC	MainJg2
				CLDSLineAngle *pSonJg4=(CLDSLineAngle*)console.FromPartHandle(pGroupMainJg2->son_jg_h[3],CLS_LINEANGLE);
				double deita_thick=base_wing_thick1-base_wing_thick2;
				pJgC=m_pJoint->AppendWrapJg('C');
				pJgD=m_pJoint->AppendWrapJg('D');
				pJgC->m_bCutBer=TRUE;
				pJgD->m_bCutBer=TRUE;	//ʮ����ϽǸֽ�ͷ�ڰ�����Ҫ���� wjh-2017.1.12
				pJgC->ucs.axis_x =  m_pJoint->ucs.axis_y;
				pJgC->ucs.axis_y = -m_pJoint->ucs.axis_x;
				pJgC->ucs.axis_z =  m_pJoint->ucs.axis_z;
				if(deita_thick>0)
					pJgC->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_y_wing.vector,xDesJointPara.norm_x_wing.vector,JointDesDlg.m_nPlankThickF+base_wing_thick1,-base_wing_thick1); 
				else
					pJgC->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_y_wing.vector,xDesJointPara.norm_x_wing.vector,JointDesDlg.m_nPlankThickF+base_wing_thick2,-base_wing_thick2); 
				pJgD->ucs.axis_x = -m_pJoint->ucs.axis_y;
				pJgD->ucs.axis_y =  m_pJoint->ucs.axis_x;
				pJgD->ucs.axis_z =  m_pJoint->ucs.axis_z;
				if(deita_thick>0)
					pJgD->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,JointDesDlg.m_nPlankThickE+base_wing_thick1,-base_wing_thick1); 
				else
					pJgD->ucs.origin =  OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,JointDesDlg.m_nPlankThickE+base_wing_thick2,-base_wing_thick2); 
				pJgC->SetStart(pJgC->ucs.origin);
				pJgD->SetStart(pJgD->ucs.origin);
				pJgC->SetEnd(pJgC->Start()+pJgC->ucs.axis_z*m_pJoint->length);
				pJgD->SetEnd(pJgD->Start()+pJgD->ucs.axis_z*m_pJoint->length);
				pJgC->set_norm_x_wing(-xDesJointPara.norm_y_wing.vector);
				pJgC->set_norm_y_wing(xDesJointPara.norm_x_wing.vector);
				pJgD->set_norm_x_wing(xDesJointPara.norm_y_wing.vector);
				pJgD->set_norm_y_wing(-xDesJointPara.norm_x_wing.vector);
				double wing_wide,wing_thick;
				restore_JG_guige(JointDesDlg.m_sJgCGuige.GetBuffer(),wing_wide,wing_thick);
				pJgC->set_wing_wide(wing_wide);
				pJgC->set_wing_thick(wing_thick);
				restore_JG_guige(JointDesDlg.m_sJgDGuige.GetBuffer(),wing_wide,wing_thick);
				pJgD->set_wing_wide(wing_wide);
				pJgD->set_wing_thick(wing_thick);
				pJgC->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgCMat).cBriefMark;
				pJgD->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgDMat).cBriefMark;
				pJgC->SetPartNo(JointDesDlg.m_sPartNoC.GetBuffer(0));
				pJgD->SetPartNo(JointDesDlg.m_sPartNoD.GetBuffer(0));
				pJgC->iSeg=SEGI(JointDesDlg.m_sCSegI.GetBuffer());//C�Ǹֶκ�
				pJgD->iSeg=SEGI(JointDesDlg.m_sDSegI.GetBuffer());//D�Ǹֶκ�
				//�趨C�Ǹֶ�λ����
				xDesJointPara.m_pMainJg1=pSonJg1;
				xDesJointPara.m_pMainJg2=pSonJg3;
				CreateRefAxisNorm(m_pJoint);
				CreateWrapJg(pJgC,TRUE);
				pJgC->SetModified();
				pJgC->ClearFlag();
				pJgC->CalPosition();
				//�趨D�Ǹֶ�λ����
				xDesJointPara.m_pMainJg1=pSonJg2;
				xDesJointPara.m_pMainJg2=pSonJg4;
				CreateRefAxisNorm(m_pJoint);
				CreateWrapJg(pJgD,TRUE);
				pJgD->SetModified();
				pJgD->ClearFlag();
				pJgD->CalPosition();
				CLDSPlate* pPadG[2]={NULL,NULL};
				CLDSPlate* pPadH[2]={NULL,NULL};
				if(fabs(deita_thick)>=2)
				{
					for(int j=0;j<2;j++)
					{
						CLDSLineAngle *pCurJg;
						if(j==0)
							pCurJg=pJgC;
						else
							pCurJg=pJgD;
						pPadG[j]=m_pJoint->AppendWrapPlank('G');
						pPadH[j]=m_pJoint->AppendWrapPlank('H');
						pPadG[j]->wiType=CLDSPlate::WASHER_PLATE;
						pPadH[j]->wiType=CLDSPlate::WASHER_PLATE;
						pPadG[j]->Thick=ftoi(fabs(deita_thick));
						pPadH[j]->Thick=ftoi(fabs(deita_thick));
						pPadG[j]->cfgword=pCurJg->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
						pPadH[j]->cfgword=pCurJg->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
						pPadG[j]->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankGMat).cBriefMark;
						pPadH[j]->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankHMat).cBriefMark;
						pPadG[j]->SetPartNo(JointDesDlg.m_sPartNoG.GetBuffer(0));
						pPadH[j]->SetPartNo(JointDesDlg.m_sPartNoH.GetBuffer(0));
						pPadG[j]->face_N=1;
						pPadH[j]->face_N= 1;
						pPadG[j]->ucs.axis_z=pCurJg->get_norm_y_wing();
						pPadG[j]->ucs.axis_x=pCurJg->GetWingVecY();
						pPadG[j]->ucs.axis_y=pPadG[j]->ucs.axis_z^pPadG[j]->ucs.axis_x;
						pPadG[j]->ucs.origin=pCurJg->End();
						pPadH[j]->ucs.axis_z=pCurJg->get_norm_x_wing();
						pPadH[j]->ucs.axis_x=-pCurJg->GetWingVecX();
						pPadH[j]->ucs.axis_y=pPadG[j]->ucs.axis_z^pPadG[j]->ucs.axis_x;
						pPadH[j]->ucs.origin=pCurJg->End();
					}
				}
				PART_ITEM *pIA=m_pJoint->FromPartId('A');
				PART_ITEM *pIB=m_pJoint->FromPartId('B');
				CLDSLineAngle *pJgA=(CLDSLineAngle*)console.FromPartHandle(pIA->h,CLS_LINEANGLE);
				CLDSLineAngle *pJgB=(CLDSLineAngle*)console.FromPartHandle(pIB->h,CLS_LINEANGLE);
				pJgB->SetPartNo(JointDesDlg.m_sPartNoB.GetBuffer(0));
				CLsRef *pLsRef;
				for(pLsRef=pJgA->GetFirstLsRef();pLsRef;pLsRef=pJgA->GetNextLsRef())
				{
					int x_wing0_y_wing1=-1;
					pJgA->IsLsInWing(pLsRef->GetLsPtr(),&x_wing0_y_wing1);
					BOOL bInMainJg1=FALSE;	//�����Ǹ�1�ϵ���˨	wht 09-12-10
					if(pSonMainJg1->FindLsByHandle((*pLsRef)->handle)||pSonMainJg2->FindLsByHandle((*pLsRef)->handle))
						bInMainJg1=TRUE;
					if(x_wing0_y_wing1==0)	//X֫����˨
					{
						pJgD->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgD->handle,TRUE);
						if(pPadG[1]&&((deita_thick>0&&!bInMainJg1)||(deita_thick<0&&bInMainJg1)))
						{
							pPadG[1]->AppendLsRef((*pLsRef)->GetLsRef());
							(*pLsRef)->AddL0Thick(pPadG[1]->handle,TRUE);
						}
						(*pLsRef)->CalGuigeAuto();
					}
					else if(x_wing0_y_wing1==1)	//Y֫����˨
					{
						pJgC->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgC->handle,TRUE);
						if(pPadH[0]&&((deita_thick>0&&!bInMainJg1)||(deita_thick<0&&bInMainJg1)))
						{
							pPadH[0]->AppendLsRef((*pLsRef)->GetLsRef());
							(*pLsRef)->AddL0Thick(pPadH[0]->handle,TRUE);
						}
						(*pLsRef)->CalGuigeAuto();
					}
				}
				for(pLsRef=pJgB->GetFirstLsRef();pLsRef;pLsRef=pJgB->GetNextLsRef())
				{
					int x_wing0_y_wing1=-1;
					pJgB->IsLsInWing(pLsRef->GetLsPtr(),&x_wing0_y_wing1);
					BOOL bInMainJg1=FALSE;	//�����Ǹ�1�ϵ���˨  wht 09-12-10
					if(pSonMainJg1->FindLsByHandle((*pLsRef)->handle)||pSonMainJg2->FindLsByHandle((*pLsRef)->handle))
						bInMainJg1=TRUE;
					if(x_wing0_y_wing1==0)	//X֫����˨
					{
						pJgC->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgC->handle,TRUE);
						if(pPadG[0]&&((deita_thick>0&&!bInMainJg1)||(deita_thick<0&&bInMainJg1)))
						{
							pPadG[0]->AppendLsRef((*pLsRef)->GetLsRef());
							(*pLsRef)->AddL0Thick(pPadG[0]->handle,TRUE);
						}
						(*pLsRef)->CalGuigeAuto();
					}
					else if(x_wing0_y_wing1==1)	//Y֫����˨
					{
						pJgD->AppendMidLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pJgD->handle,TRUE);
						if(pPadH[1]&&((deita_thick>0&&!bInMainJg1)||(deita_thick<0&&bInMainJg1)))
						{
							pPadH[1]->AppendLsRef((*pLsRef)->GetLsRef());
							(*pLsRef)->AddL0Thick(pPadH[1]->handle,TRUE);
						}
						(*pLsRef)->CalGuigeAuto();
					}
				}
				for(pLsRef=pJgC->GetFirstLsRef();pLsRef;pLsRef=pJgC->GetNextLsRef())
				{
					if(pSonMainJg1->FindLsByHandle((*pLsRef)->handle)||pSonMainJg2->FindLsByHandle((*pLsRef)->handle))
					{
						if(pSonJg1->pStart->handle==m_pJoint->handle)
							pSonJg1->AppendStartLsRef((*pLsRef)->GetLsRef());
						else
							pSonJg1->AppendEndLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pSonJg1->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
					else if(!pSonMainJg1->FindLsByHandle((*pLsRef)->handle)&&!pSonMainJg2->FindLsByHandle((*pLsRef)->handle))
					{
						if(pSonJg3->pStart->handle==m_pJoint->handle)
							pSonJg3->AppendStartLsRef((*pLsRef)->GetLsRef());
						else
							pSonJg3->AppendEndLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pSonJg3->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
				}
				for(pLsRef=pJgD->GetFirstLsRef();pLsRef;pLsRef=pJgD->GetNextLsRef())
				{
					if(pSonMainJg1->FindLsByHandle((*pLsRef)->handle)||pSonMainJg2->FindLsByHandle((*pLsRef)->handle))
					{
						if(pSonJg2->pStart->handle==m_pJoint->handle)
							pSonJg2->AppendStartLsRef((*pLsRef)->GetLsRef());
						else
							pSonJg2->AppendEndLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pSonJg2->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
					if(!pSonMainJg1->FindLsByHandle((*pLsRef)->handle)&&!pSonMainJg2->FindLsByHandle((*pLsRef)->handle))
					{
						if(pSonJg4->pStart->handle==m_pJoint->handle)
							pSonJg4->AppendStartLsRef((*pLsRef)->GetLsRef());
						else
							pSonJg4->AppendEndLsRef((*pLsRef)->GetLsRef());
						(*pLsRef)->AddL0Thick(pSonJg4->handle,TRUE);
						(*pLsRef)->CalGuigeAuto();
					}
				}
				if(pPadG[0])
				{
					pPadG[0]->CalStdProfile();
					pPadG[1]->CalStdProfile();
					pPadH[0]->CalStdProfile();
					pPadH[1]->CalStdProfile();
					for(int i=0;i<4;i++)
					{
						CLDSPlate *pCurPlate=NULL;
						if(i==0)
							pCurPlate=pPadG[0];
						else if(i==1)
							pCurPlate=pPadG[1];
						else if(i==2)
							pCurPlate=pPadH[0];
						else if(i==3)
							pCurPlate=pPadH[1];
						int nPlankWidth=JointDesDlg.m_nPlankWidthG;
						if(i>1)
							nPlankWidth=JointDesDlg.m_nPlankWidthH;
						//��������ĵ���ȵ����������
						double first_x=pCurPlate->vertex_list.GetFirst()->vertex.x;
						double tail_x=pCurPlate->vertex_list.GetTail()->vertex.x;
						double offsetdist=0.5*(nPlankWidth-fabs(tail_x-first_x));
						int sign=1;
						if(first_x<tail_x)
						{
							//if(first_x<0)
							//	sign=-1;
							pCurPlate->vertex_list.GetFirst()->vertex.x-=offsetdist;
							pCurPlate->vertex_list.GetNext()->vertex.x -=offsetdist;
							pCurPlate->vertex_list.GetNext()->vertex.x +=offsetdist;
							pCurPlate->vertex_list.GetNext()->vertex.x +=offsetdist;
						}
						else 
						{
							//if(tail_x<0)
							//	sign=-1;
							pCurPlate->vertex_list.GetFirst()->vertex.x+=offsetdist;
							pCurPlate->vertex_list.GetNext()->vertex.x +=offsetdist;
							pCurPlate->vertex_list.GetNext()->vertex.x -=offsetdist;
							pCurPlate->vertex_list.GetNext()->vertex.x -=offsetdist;
						}
					}
				}

			}
				
			m_pJoint->joint_style=JointDesDlg.m_iJointStyle;
			//�ڴ˲����Զ��������η����˹�����İ��Ƚ�ʧЧ wjh-2008.12.28
			/*for(PART_ITEM *pI=m_pJoint->GetFirstPartItem();pI;pI=m_pJoint->GetNextPartItem())
			{
				CLDSPlate *pPlank=(CLDSPlate*)console.FromPartHandle(pI->h,CLS_PLATE);
				if(pPlank)
					pPlank->CalStdProfile();
			}*/
		}
		else if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_LINEANGLE&&xDesJointPara.m_pMainJg2->GetClassTypeId()==CLS_LINEANGLE)
		{
			CreateRefAxisNorm(m_pJoint);
			//--------VVVVVVVV-----��ͷ�����------------------
			if(!DesignJoint())	//�������ʱ����Ҫ��ʾ�޸�׼��Ի��� wht 09-12-29
				return FALSE;
		}
		for(CLsRef *pLsRef=m_pJoint->boltRefList.GetFirst();pLsRef;pLsRef=m_pJoint->boltRefList.GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			pBolt->iSeg=m_pJoint->iSeg;
			pBolt->CalGuigeAuto();
			if(UI::blEnableIntermediateUpdateUI)
			{
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
		if((JointDesDlg.m_iJointStyle==4||JointDesDlg.m_iJointStyle==5) && JointDesDlg.m_bWeldingVertPlate)
		{
			CLDSPlate* pBasePlate=NULL,*pVertPlateArr[2]={NULL,NULL};
			char cBaseSymbol='E',cVertSymbol='F';
			if(JointDesDlg.m_iJointStyle==5)
			{
				cBaseSymbol='F';
				cVertSymbol='E';
			}
			for(PART_ITEM *pItem=m_pJoint->GetFirstPartItem();pItem;pItem=m_pJoint->GetNextPartItem())
			{
				if(pItem->cID==cBaseSymbol)
					pBasePlate=(CLDSPlate*)Ta.Parts.FromHandle(pItem->h,CLS_PLATE);
				else if(pItem->cID==cVertSymbol&&pVertPlateArr[0]==NULL)
					pVertPlateArr[0]=(CLDSPlate*)Ta.Parts.FromHandle(pItem->h,CLS_PLATE);
				else if(pItem->cID==cVertSymbol&&pVertPlateArr[1]==NULL)
					pVertPlateArr[1]=(CLDSPlate*)Ta.Parts.FromHandle(pItem->h,CLS_PLATE);
				if(pBasePlate&&pVertPlateArr[0]&&pVertPlateArr[1])
					break;
			}
			for(int index=0;index<2&&pBasePlate&&pVertPlateArr[index];index++)
			{
				CLDSPlate* pVertPlate=pVertPlateArr[index];
				pVertPlate->m_hPartWeldParent=pBasePlate->handle;
				int i=0,lowi0=0,lowi1=0;
				GEPOINT vertex_arr[4],geom,lowest_vertex[2];
				for(PROFILE_VERTEX* pVertex=pVertPlate->vertex_list.GetFirst();pVertex&&i<4;pVertex=pVertPlate->vertex_list.GetNext(),i++)
				{
					geom+=pVertex->vertex;
					vertex_arr[i]=pVertex->vertex;
					coord_trans(vertex_arr[i],pVertPlate->ucs,TRUE);
					coord_trans(vertex_arr[i],pBasePlate->ucs,FALSE);
					if(i==0)
					{
						lowest_vertex[0]=vertex_arr[0];
						lowi0=0;
					}
					else if(fabs(vertex_arr[i].z)<fabs(lowest_vertex[0].z))
					{
						lowi1=lowi0;
						lowi0=i;
						lowest_vertex[1]=lowest_vertex[0];
						lowest_vertex[0]=vertex_arr[i];
					}
					else if(i==1 || fabs(vertex_arr[i].z)<fabs(lowest_vertex[1].z))
					{
						lowi1=i;
						lowest_vertex[1]=vertex_arr[i];
					}
				}
				//���岻��Ҫ������ͨ��,��ͨ���������߲�����Ǹ������ wht 17-05-18
				for(i=0;i<2;i++)
				{
					PROFILE_VERTEX* pBtmVertex0=pVertPlate->vertex_list.GetByIndex(lowi0);
					PROFILE_VERTEX* pBtmVertex1=pVertPlate->vertex_list.GetByIndex(lowi1);
					if(lowi0<lowi1 ||(lowi0==3&&lowi1==0))
					{
						pBtmVertex0->m_bWeldEdge=true;
						pBtmVertex0->manu_space=1;
					}
					else
					{
						pBtmVertex1->m_bWeldEdge=true;
						pBtmVertex1->manu_space=1;
					}
				}
			}
		}
		if(blEnableMirCreate&&JointDesDlg.m_iMirMsg>0&&JointDesDlg.m_iMirMsg<=4)
			MirJoint(m_pJoint,JointDesDlg.m_iMirMsg);
		if(UI::blEnableIntermediateUpdateUI)
		{
			g_pSolidDraw->SetEntSnapStatus(xDesJointPara.m_pMainJg1->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(xDesJointPara.m_pMainJg2->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(xDesJointPara.m_pCurNode->handle,TRUE);
		}
		return TRUE;
	}
	catch(char *sErrorMsg)
	{
		AfxMessageBox(sErrorMsg);
		return FALSE;
	}
}

BOOL CDesignJoint::DesJointSubPart(CLDSLineAngle *&pOutWrapJg,CLDSLineAngle *&pInWrapJg,
					CLDSPlate *&pYPlate,CLDSPlate *&pXPlate,
					CLDSPlate *&pXPadPlate,CLDSPlate *&pYPadPlate)
{
	double wing_wide_out=0,wing_wide_in=0,
		wing_thick_out=0,wing_thick_in=0,norm_offset=0;
	pOutWrapJg=pInWrapJg=NULL;
	pYPlate=pXPlate=pXPadPlate=pYPadPlate=NULL;

	//�����ݿ������ɽ�ͷ���ӹ���
	if(m_pJoint->joint_style==0)	//�����
	{
		pOutWrapJg = m_pJoint->AppendWrapJg('B');
		pOutWrapJg->m_bCutRoot = TRUE;	//�������
	}
	else if(m_pJoint->joint_style==1)	//�������
	{
		//��Ӱ��Ǹ�
		pOutWrapJg = m_pJoint->AppendWrapJg('B');
		pInWrapJg  = m_pJoint->AppendWrapJg('A');
		pOutWrapJg->m_bCutRoot = TRUE;	//�������
		pInWrapJg->m_bCutBer=TRUE;
		pInWrapJg->m_nCutBerR=(WORD)FindJg_r(base_wing_wide1)+2;	//����Բ���뾶
		//��϶����2����ʹ�õ��
		if(fabs(base_wing_thick1-base_wing_thick2)-2>-EPS)
		{
			pXPadPlate = m_pJoint->AppendWrapPlank('H');
			pYPadPlate = m_pJoint->AppendWrapPlank('G');
			pXPadPlate->wiType=CLDSPlate::WASHER_PLATE;//jdb_style=5;ʵ��jdb_styleӦ��Ϊ4,ԭ����5���������� wjh-2017.1.21
			pYPadPlate->wiType=CLDSPlate::WASHER_PLATE;//jdb_style=5;
			pXPadPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
			pYPadPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
			pXPadPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
			pXPadPlate->designInfo.iProfileStyle0123=pYPadPlate->designInfo.iProfileStyle0123=3;
			pYPadPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
			pXPadPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
			pYPadPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		}
	}
	else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
	{
		//����ڽǸ�
		if(JointDesDlg.m_iJointStyle>3&&m_pJoint->FromPartId('A'))	//��ϽǸֽ�ͷ
			pInWrapJg=m_pJoint->AppendWrapJg('B');	//��һ�ڰ���
		else
			pInWrapJg  = m_pJoint->AppendWrapJg('A');
		pInWrapJg->m_bCutBer=TRUE;//
		pInWrapJg->m_nCutBerR=(WORD)FindJg_r(base_wing_wide1)+2;	//����Բ���뾶
		//��Ӱ��ְ�
		if((JointDesDlg.m_iJointStyle==4||JointDesDlg.m_iJointStyle==6||JointDesDlg.m_iJointStyle==8)&&m_pJoint->FromPartId('F'))
		{
			PART_ITEM *pI=m_pJoint->FromPartId('E');
			pXPlate=(CLDSPlate*)console.FromPartHandle(pI->h,CLS_PLATE);
		}
		else if((JointDesDlg.m_iJointStyle==5||JointDesDlg.m_iJointStyle==7||JointDesDlg.m_iJointStyle==9)&&m_pJoint->FromPartId('E'))
		{
			PART_ITEM *pI=m_pJoint->FromPartId('F');
			pYPlate=(CLDSPlate*)console.FromPartHandle(pI->h,CLS_PLATE);
		}
		if(pXPlate==NULL)
			pXPlate = m_pJoint->AppendWrapPlank('E');
		if(pYPlate==NULL)
			pYPlate = m_pJoint->AppendWrapPlank('F');
		pXPlate->wiType=CLDSPlate::WRAPPER_PLATE;
		pYPlate->wiType=CLDSPlate::WRAPPER_PLATE;
		pXPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
		pYPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
		pXPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
		pYPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
		pXPlate->designInfo.iProfileStyle0123=pYPlate->designInfo.iProfileStyle0123=3;
		pXPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pYPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		if(fabs(base_wing_thick1-base_wing_thick2)-2>-EPS)
		{	//��϶����2����ʹ�õ��
			pXPadPlate = m_pJoint->AppendWrapPlank('H');
			pYPadPlate = m_pJoint->AppendWrapPlank('G');
			pXPadPlate->wiType=CLDSPlate::WASHER_PLATE;//jdb_style=5;ʵ��jdb_styleӦ��Ϊ4,ԭ����5���������� wjh-2017.1.21
			pYPadPlate->wiType=CLDSPlate::WASHER_PLATE;//jdb_style=5;
			pXPadPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
			pYPadPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
			pXPadPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
			pYPadPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
			pXPadPlate->designInfo.iProfileStyle0123=pYPadPlate->designInfo.iProfileStyle0123=3;
			pXPadPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
			pYPadPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		}
	}
	else// if(m_pJoint->joint_style==3)	//������ڰ��ְ�
	{
		//�����Ǹ�
		pOutWrapJg  = m_pJoint->AppendWrapJg('B');
		pOutWrapJg->m_bCutRoot=TRUE;		//�������
		//��Ӱ��ְ�
		pXPlate = m_pJoint->AppendWrapPlank('F');
		pYPlate = m_pJoint->AppendWrapPlank('E');
		pXPlate->wiType=CLDSPlate::WRAPPER_PLATE;
		pYPlate->wiType=CLDSPlate::WRAPPER_PLATE;
		pXPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
		pYPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
		pXPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
		pYPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
		pXPlate->designInfo.iProfileStyle0123=pYPlate->designInfo.iProfileStyle0123=3;
		pXPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pYPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		if(fabs(base_wing_thick1-base_wing_thick2)-2>-EPS)
		{	//��϶����2����ʹ�õ��
			pXPadPlate = m_pJoint->AppendWrapPlank('H');
			pYPadPlate = m_pJoint->AppendWrapPlank('G');
			pXPadPlate->wiType=CLDSPlate::WASHER_PLATE;//jdb_style=5;ʵ��jdb_styleӦ��Ϊ4,ԭ����5���������� wjh-2017.1.21
			pYPadPlate->wiType=CLDSPlate::WASHER_PLATE;//jdb_style=5;
			pXPadPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
			pYPadPlate->designInfo.m_hBaseNode = xDesJointPara.m_pCurNode->handle;
			pXPadPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
			pYPadPlate->designInfo.m_hBasePart=xDesJointPara.m_pMainJg1->handle;
			pXPadPlate->designInfo.iProfileStyle0123=pYPadPlate->designInfo.iProfileStyle0123=3;
			pXPadPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
			pYPadPlate->cfgword=xDesJointPara.m_pMainJg1->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		}
	}
	/*else if(m_pJoint->joint_style==4)	//��Ͻ�ͷ�ڰ���X֫�ְ�ض�
	{
	}
	else if(m_pJoint->joint_style==5)	//��Ͻ�ͷ�ڰ���Y֫�ְ�ض�
	{
	}
	else if(m_pJoint->joint_style==6)	//��Ͻ�ͷ�������X֫�ְ�ض�
	{
	}
	else if(m_pJoint->joint_style==7)	//��Ͻ�ͷ�������Y֫�ְ�ض�
	{
	}
	else if(m_pJoint->joint_style==8)	//ʮ�ֽ�ͷ�ڰ���X֫�ְ�ض�
	{
	}
	else if(m_pJoint->joint_style==9)	//ʮ�ֽ�ͷ�ڰ���Y֫�ְ�ض�
	{
	}*/
	//�������ݿ��������ɽ�ͷ�ĸ��ӹ����ļ��β���
	if(m_pJoint->joint_style==0)		//�����
	{
		pOutWrapJg->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgBMat).cBriefMark;
		restore_JG_guige(JointDesDlg.m_sJgBGuige.GetBuffer(),wing_wide_out,wing_thick_out);
		pOutWrapJg->set_wing_wide(wing_wide_out);
		pOutWrapJg->set_wing_thick(wing_thick_out);

		//ls.L0 = base_wing_thick1+wing_thick_out;
		pOutWrapJg->ucs = m_pJoint->ucs;
		pOutWrapJg->ucs.origin = OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,wing_thick_out+JointDesDlg.m_fJgBNormOffsetX,wing_thick_out+JointDesDlg.m_fJgBNormOffsetY);
	}
	else if(m_pJoint->joint_style==1)	//�������
	{
		pOutWrapJg->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgBMat).cBriefMark;
		pInWrapJg->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgBMat).cBriefMark;
		//��϶����2����ʹ�õ��
		if(pXPadPlate)
		{
			pXPadPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankGMat).cBriefMark;
			pYPadPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankHMat).cBriefMark;
		}
		restore_JG_guige(JointDesDlg.m_sJgBGuige.GetBuffer(),wing_wide_out,wing_thick_out);
		pOutWrapJg->set_wing_wide(wing_wide_out);
		pOutWrapJg->set_wing_thick(wing_thick_out);
		restore_JG_guige(JointDesDlg.m_sJgAGuige.GetBuffer(),wing_wide_in,wing_thick_in);
		pInWrapJg->set_wing_wide(wing_wide_in);
		pInWrapJg->set_wing_thick(wing_thick_in);

		//ls.L0 = base_wing_thick1+wing_thick_out+wing_thick_in;

		pOutWrapJg->ucs = pInWrapJg->ucs = m_pJoint->ucs;
		pOutWrapJg->ucs.origin = OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,wing_thick_out+JointDesDlg.m_fJgBNormOffsetX,wing_thick_out+JointDesDlg.m_fJgBNormOffsetY);
		//if(base_wing_thick1>base_wing_thick2)
			pInWrapJg->ucs.origin = OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,-base_wing_thick1-JointDesDlg.m_fJgANormOffsetX,-base_wing_thick1-JointDesDlg.m_fJgANormOffsetY);
		//else
		//	pInWrapJg->ucs.origin = OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,-base_wing_thick2+JointDesDlg.m_fJgANormOffsetX,-base_wing_thick2+JointDesDlg.m_fJgANormOffsetY);
		//��϶���ڵ���2����ʹ�õ��
		if(pXPadPlate)
		{
			pXPadPlate->Thick=ftoi(fabs(base_wing_thick1-base_wing_thick2));
			pXPadPlate->Thick=ftoi(pXPadPlate->GetThick()-ftoi(pXPadPlate->GetThick())%2);
			pYPadPlate->Thick=ftoi(pXPadPlate->GetThick());
			//����
			pXPadPlate->feature = (int)(base_wing_wide2-
						base_wing_thick2-FindJg_r(base_wing_wide2));
			pYPadPlate->feature = pXPadPlate->feature;

			//if(base_wing_thick1>base_wing_thick2)
			{	//�ϱ��º�
				//X֫�ϵĵ��
				pXPadPlate->ucs.origin = m_pJoint->ucs.origin
					+(g_sysPara.JgEndDist+base_jg1_oddment-S1-deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide2-pXPadPlate->feature/2)*m_pJoint->ucs.axis_x
					-(base_wing_thick2+pXPadPlate->GetThick())*xDesJointPara.norm_x_wing.vector;
				//Y֫�ϵĵ��
				pYPadPlate->ucs.origin = m_pJoint->ucs.origin
					+(g_sysPara.JgEndDist+base_jg1_oddment-S1-deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide2-pYPadPlate->feature/2)*m_pJoint->ucs.axis_y
					-(base_wing_thick2+pYPadPlate->GetThick())*xDesJointPara.norm_y_wing.vector;
			}
		}
	}
	else if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
	{
		pInWrapJg->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgAMat).cBriefMark;
		//��϶����2����ʹ�õ��
		pXPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankEMat).cBriefMark;
		pYPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankFMat).cBriefMark;
		if(pXPadPlate)//��϶����2����ʹ�õ��
			pXPadPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankGMat).cBriefMark;
		if(pYPadPlate)//��϶����2����ʹ�õ��
			pYPadPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankHMat).cBriefMark;
		restore_JG_guige(JointDesDlg.m_sJgAGuige.GetBuffer(),wing_wide_in,wing_thick_in);
		pInWrapJg->set_wing_wide(wing_wide_in);
		pInWrapJg->set_wing_thick(wing_thick_in);
		pXPlate->Thick=JointDesDlg.m_nPlankThickE;
		pYPlate->Thick=JointDesDlg.m_nPlankThickF;
		pYPlate->feature = pXPlate->feature = JointDesDlg.m_nPlankThickE;
		//ls.L0 = base_wing_thick1+wing_thick_in+JointDesDlg.m_nPlankThick;

		pInWrapJg->ucs = m_pJoint->ucs;
		if(base_wing_thick1>base_wing_thick2)
			pInWrapJg->ucs.origin = OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,-base_wing_thick1-JointDesDlg.m_fJgANormOffsetX,-base_wing_thick1-JointDesDlg.m_fJgANormOffsetY);
		else
			pInWrapJg->ucs.origin = OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,-base_wing_thick2-JointDesDlg.m_fJgANormOffsetX,-base_wing_thick2-JointDesDlg.m_fJgANormOffsetY);
		//Y֫�ϵİ�
		if(!pYPlate->designInfo.m_bEnableFlexibleDesign)
			Add_Pnt(pYPlate->ucs.origin,m_pJoint->ucs.origin,
					m_pJoint->ucs.axis_y*(base_wing_wide1+base_wing_thick1)*0.5);
		//X֫�ϵİ�
		if(!pXPlate->designInfo.m_bEnableFlexibleDesign)
			Add_Pnt(pXPlate->ucs.origin,m_pJoint->ucs.origin,
					m_pJoint->ucs.axis_x*(base_wing_wide1+base_wing_thick1)*0.5);
		//��϶���ڵ���2����ʹ�õ��
		if(pXPadPlate)
		{
			//����
			pXPadPlate->Thick=ftoi(fabs(base_wing_thick1-base_wing_thick2));
			pXPadPlate->Thick=ftoi(pXPadPlate->GetThick())-ftoi(pXPadPlate->GetThick())%2;
			pYPadPlate->Thick=ftoi(pXPadPlate->GetThick());
			//����
			pXPadPlate->feature = (int)(base_wing_wide2-
						base_wing_thick2-FindJg_r(base_wing_wide2));
			pXPadPlate->feature = __min(pXPadPlate->feature,pXPlate->feature);
			pYPadPlate->feature = pXPadPlate->feature;
			if(base_wing_thick1>base_wing_thick2)
			{	//�ϱ��º�
				//X֫�ϵĵ��
				pXPadPlate->ucs.origin = m_pJoint->ucs.origin
					+(g_sysPara.JgEndDist+base_jg1_oddment-S1-deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide2-pXPadPlate->feature/2)*m_pJoint->ucs.axis_x
					-(base_wing_thick2+pXPadPlate->GetThick())*xDesJointPara.norm_x_wing.vector;
				//Y֫�ϵĵ��
				pYPadPlate->ucs.origin = m_pJoint->ucs.origin
					+(g_sysPara.JgEndDist+base_jg1_oddment-S1-deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide2-pXPadPlate->feature/2)*m_pJoint->ucs.axis_y
					-(base_wing_thick2+pYPadPlate->GetThick())*xDesJointPara.norm_y_wing.vector;
			}
			/*else
			{	//�Ϻ��±�
				//X֫�ϵĵ��
				pXPadPlate->ucs.origin = m_pJoint->ucs.origin
					-(S1+deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide1-pXPadPlate->feature/2)*m_pJoint->ucs.axis_x
					-(base_wing_thick1+pXPadPlate->GetThick())*xDesJointPara.norm_x_wing.vector;
				//Y֫�ϵĵ��
				pYPadPlate->ucs.origin = m_pJoint->ucs.origin
					-(S1+deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide1-pXPadPlate->feature/2)*m_pJoint->ucs.axis_y
					-(base_wing_thick1+pYPadPlate->GetThick())*xDesJointPara.norm_y_wing.vector;
			}*/
		}
	}
	else	//�ڰ��ְ������
	{
		pOutWrapJg->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iJgBMat).cBriefMark;
		pXPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankEMat).cBriefMark;
		pYPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankFMat).cBriefMark;
		if(pXPadPlate)//��϶����2����ʹ�õ��
			pXPadPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankGMat).cBriefMark;
		if(pYPadPlate)//��϶����2����ʹ�õ��
			pYPadPlate->cMaterial=CSteelMatLibrary::RecordAt(JointDesDlg.m_iPlankHMat).cBriefMark;
		restore_JG_guige(JointDesDlg.m_sJgBGuige.GetBuffer(),wing_wide_out,wing_thick_out);
		pOutWrapJg->set_wing_wide(wing_wide_out);
		pOutWrapJg->set_wing_thick(wing_thick_out);
		pXPlate->Thick=JointDesDlg.m_nPlankThickE;
		pYPlate->Thick=JointDesDlg.m_nPlankThickF;
		pYPlate->feature = pXPlate->feature = JointDesDlg.m_nPlankThickE;
		//ls.L0 = base_wing_thick1+wing_thick_in+JointDesDlg.m_nPlankThick;

		pOutWrapJg->ucs = m_pJoint->ucs;
		pOutWrapJg->ucs.origin = OffsetBy2Vec(m_pJoint->ucs.origin,xDesJointPara.norm_x_wing.vector,xDesJointPara.norm_y_wing.vector,wing_thick_out+JointDesDlg.m_fJgBNormOffsetX,wing_thick_out+JointDesDlg.m_fJgBNormOffsetY);
		if(base_wing_thick1>base_wing_thick2)
		{	//�ϱ��º�
			//Y֫�ϵİ�
			pYPlate->ucs.origin = get_pt_by_vec(m_pJoint->ucs.axis_y,m_pJoint->ucs.axis_x,m_pJoint->ucs.origin,
				(base_wing_wide1+base_wing_thick1)*0.5,base_wing_thick1+JointDesDlg.m_nPlankThickF);
			//X֫�ϵİ�
			pXPlate->ucs.origin = get_pt_by_vec(m_pJoint->ucs.axis_x,m_pJoint->ucs.axis_y,m_pJoint->ucs.origin,
				(base_wing_wide1+base_wing_thick1)*0.5,base_wing_thick1+JointDesDlg.m_nPlankThickE);
		}
		/*else
		{	//�Ϻ��±�
			//Y֫�ϵİ�
			pYPlate->ucs.origin = get_pt_by_vec(m_pJoint->ucs.axis_y,m_pJoint->ucs.axis_x,m_pJoint->ucs.origin,
				(base_wing_wide1+base_wing_thick1)*0.5,base_wing_thick2+JointDesDlg.m_nPlankThickF);
			//X֫�ϵİ�
			pXPlate->ucs.origin = get_pt_by_vec(m_pJoint->ucs.axis_x,m_pJoint->ucs.axis_y,m_pJoint->ucs.origin,
				(base_wing_wide1+base_wing_thick1)*0.5,base_wing_thick2+JointDesDlg.m_nPlankThickE);
		}*/
		//��϶���ڵ���2����ʹ�õ��
		if(pXPadPlate)
		{
			//����
			pXPadPlate->Thick=ftoi(fabs(base_wing_thick1-base_wing_thick2));
			pXPadPlate->Thick=ftoi(pXPadPlate->GetThick())-ftoi(pXPadPlate->GetThick())%2;
			pYPadPlate->Thick=ftoi(pXPadPlate->GetThick());
			//����
			pXPadPlate->feature = (int)(base_wing_wide2-
						base_wing_thick2-FindJg_r(base_wing_wide2));
			pXPadPlate->feature = __min(pXPadPlate->feature,pXPlate->feature);
			pYPadPlate->feature = pXPadPlate->feature;
			if(base_wing_thick1>base_wing_thick2)
			{	//�ϱ��º�
				//X֫�ϵĵ��
				pXPadPlate->ucs.origin = m_pJoint->ucs.origin
					+(g_sysPara.JgEndDist+base_jg1_oddment-S1-deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide2-pXPadPlate->feature/2)*m_pJoint->ucs.axis_x
					-(base_wing_thick2+pXPadPlate->GetThick())*xDesJointPara.norm_x_wing.vector;
				//Y֫�ϵĵ��
				pYPadPlate->ucs.origin = m_pJoint->ucs.origin
					+(g_sysPara.JgEndDist+base_jg1_oddment-S1-deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide2-pXPadPlate->feature/2)*m_pJoint->ucs.axis_y
					-(base_wing_thick2+pYPadPlate->GetThick())*xDesJointPara.norm_y_wing.vector;
			}
			/*else
			{	//�Ϻ��±�
				//X֫�ϵĵ��
				pXPadPlate->ucs.origin = m_pJoint->ucs.origin
					-(S1+deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide1-pXPadPlate->feature/2)*m_pJoint->ucs.axis_x
					-(base_wing_thick1+pXPadPlate->GetThick())*xDesJointPara.norm_x_wing.vector;
				//Y֫�ϵĵ��
				pYPadPlate->ucs.origin = m_pJoint->ucs.origin
					-(S1+deita)*m_pJoint->ucs.axis_z
					+(base_wing_wide1-pXPadPlate->feature/2)*m_pJoint->ucs.axis_y
					-(base_wing_thick1+pYPadPlate->GetThick())*xDesJointPara.norm_y_wing.vector;
			}*/
		}
	}
	if(pXPlate&&pYPlate)
	{
		if(!pXPlate->designInfo.m_bEnableFlexibleDesign)
		{
			pXPlate->designInfo.origin=xDesJointPara.start;
			pXPlate->designInfo.origin.datum_pos_style=1;
			pXPlate->designInfo.origin.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
			pXPlate->designInfo.origin.des_para.RODEND.bIncOddEffect=TRUE;
			pXPlate->designInfo.origin.des_para.RODEND.wing_offset_style=4;
			pXPlate->designInfo.origin.des_para.RODEND.wing_offset_dist=base_wing_wide2-pXPlate->feature/2;
			if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_LINEANGLE&&xDesJointPara.m_pMainJg1->group_father_jg_h>0x20)
			{	//������ϽǸ������Ӱ���С����ϼ�϶����£����Ĳ�����ϽǸ�������� wjh-2015.5.14
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)xDesJointPara.m_pMainJg1->BelongModel()->FromPartHandle(xDesJointPara.m_pMainJg1->group_father_jg_h,CLS_GROUPLINEANGLE);
				if(pGroupAngle&&(pGroupAngle->jg_space-pXPlate->GetThick())>=1)
					strcpy(pXPlate->designInfo.origin.des_para.RODEND.norm_offset,CXhChar16("%.0f",(pGroupAngle->jg_space-pXPlate->GetThick())/2));
			}
			pXPlate->designInfo.norm.hVicePart=xDesJointPara.m_pMainJg1->handle;
			pXPlate->designInfo.norm.norm_style=1;
			pXPlate->designInfo.norm.direction=0;
		}
		if(!pYPlate->designInfo.m_bEnableFlexibleDesign)
		{
			pYPlate->designInfo.origin=xDesJointPara.start;
			pYPlate->designInfo.origin.datum_pos_style=1;
			pYPlate->designInfo.origin.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
			pYPlate->designInfo.origin.des_para.RODEND.bIncOddEffect=TRUE;
			pYPlate->designInfo.origin.des_para.RODEND.wing_offset_style=4;
			pYPlate->designInfo.origin.des_para.RODEND.wing_offset_dist=base_wing_wide2-pYPlate->feature/2;
			if(xDesJointPara.m_pMainJg1->GetClassTypeId()==CLS_LINEANGLE&&xDesJointPara.m_pMainJg1->group_father_jg_h>0x20)
			{	//������ϽǸ������Ӱ���С����ϼ�϶����£����Ĳ�����ϽǸ�������� wjh-2015.5.14
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)xDesJointPara.m_pMainJg1->BelongModel()->FromPartHandle(xDesJointPara.m_pMainJg1->group_father_jg_h,CLS_GROUPLINEANGLE);
				if(pGroupAngle&&(pGroupAngle->jg_space-pYPlate->GetThick())>=1)
					strcpy(pYPlate->designInfo.origin.des_para.RODEND.norm_offset,CXhChar16("%.0f",(pGroupAngle->jg_space-pYPlate->GetThick())/2));
			}
			pYPlate->designInfo.norm.hVicePart=xDesJointPara.m_pMainJg1->handle;
			pYPlate->designInfo.norm.norm_style=1;
			pYPlate->designInfo.norm.direction=0;
		}
		if(xDesJointPara.m_pMainJg1->pStart==xDesJointPara.m_pCurNode)	//��ʼ���շ���
		{
			if(!pXPlate->designInfo.m_bEnableFlexibleDesign)
			{
				pXPlate->designInfo.origin.des_para.RODEND.direction=0;
				pXPlate->designInfo.origin.des_para.RODEND.offset_wing=1;
				pXPlate->designInfo.norm.norm_wing=1;
			}
			if(!pYPlate->designInfo.m_bEnableFlexibleDesign)
			{
				pYPlate->designInfo.origin.des_para.RODEND.direction=0;
				pYPlate->designInfo.origin.des_para.RODEND.offset_wing=0;
				pYPlate->designInfo.norm.norm_wing=0;
			}
		}
		else								//������ʼ����
		{
			if(!pXPlate->designInfo.m_bEnableFlexibleDesign)
			{
				pXPlate->designInfo.origin.des_para.RODEND.direction=1;
				pXPlate->designInfo.origin.des_para.RODEND.offset_wing=0;
				pXPlate->designInfo.norm.norm_wing=0;
			}
			if(!pYPlate->designInfo.m_bEnableFlexibleDesign)
			{
				pYPlate->designInfo.origin.des_para.RODEND.direction=1;
				pYPlate->designInfo.origin.des_para.RODEND.offset_wing=1;
				pYPlate->designInfo.norm.norm_wing=1;
			}
		}

		if(m_pJoint->joint_style==2)	//�ڰ�������ְ�
		{	//��֪������Ϊʲô������ò��ִ��룬������Ϊ���࣬�һ��������趨�Ľ�� wjh-2015.5.14
			//if(!pXPlate->designInfo.m_bEnableFlexibleDesign)
			//	_snprintf(pXPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"");
			//if(!pYPlate->designInfo.m_bEnableFlexibleDesign)
			//	_snprintf(pYPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"");
		}
		else	//�ڰ��ְ������
		{
			if(!pXPlate->designInfo.m_bEnableFlexibleDesign)
				_snprintf(pXPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pXPlate->handle);
			if(!pYPlate->designInfo.m_bEnableFlexibleDesign)
				_snprintf(pYPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pYPlate->handle);
		}
	}
	if(pXPadPlate&&pYPadPlate)
	{
		if(base_wing_thick1>base_wing_thick2)
		{
			pXPadPlate->designInfo.m_bEnableFlexibleDesign=pYPadPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
			pXPadPlate->designInfo.origin.datum_pos_style=1;
			pXPadPlate->designInfo.origin.des_para.RODEND.hRod=xDesJointPara.m_pMainJg2->handle;
			pXPadPlate->designInfo.origin.des_para.RODEND.len_offset_dist=0;
			pXPadPlate->designInfo.origin.des_para.RODEND.bIncOddEffect=TRUE;
			pXPadPlate->designInfo.origin.des_para.RODEND.wing_offset_style=4;
			pXPadPlate->designInfo.origin.des_para.RODEND.wing_offset_dist=base_wing_wide2;//-pXPadPlate->feature/2;  //wht 09-12-10
			_snprintf(pXPadPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X",xDesJointPara.m_pMainJg1->handle);
			pYPadPlate->designInfo.origin.datum_pos_style=1;
			pYPadPlate->designInfo.origin.des_para.RODEND.hRod=xDesJointPara.m_pMainJg2->handle;
			pYPadPlate->designInfo.origin.des_para.RODEND.len_offset_dist=0;
			pYPadPlate->designInfo.origin.des_para.RODEND.bIncOddEffect=TRUE;
			pYPadPlate->designInfo.origin.des_para.RODEND.wing_offset_style=4;
			pYPadPlate->designInfo.origin.des_para.RODEND.wing_offset_dist=base_wing_wide2;//-pYPadPlate->feature/2;
			_snprintf(pYPadPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X",xDesJointPara.m_pMainJg1->handle);

			pXPadPlate->designInfo.norm.hVicePart=xDesJointPara.m_pMainJg2->handle;
			pYPadPlate->designInfo.norm.hVicePart=xDesJointPara.m_pMainJg2->handle;
			pXPadPlate->designInfo.norm.norm_style=1;
			pXPadPlate->designInfo.norm.direction=0;
			pYPadPlate->designInfo.norm.norm_style=1;
			pYPadPlate->designInfo.norm.direction=0;
			if(xDesJointPara.m_pMainJg2->pStart==xDesJointPara.m_pCurNode)	//��ʼ���շ���
			{
				pXPadPlate->designInfo.origin.des_para.RODEND.direction=0;
				pXPadPlate->designInfo.origin.des_para.RODEND.offset_wing=0;
				pXPadPlate->designInfo.norm.norm_wing=0;
				pYPadPlate->designInfo.origin.des_para.RODEND.direction=0;
				pYPadPlate->designInfo.origin.des_para.RODEND.offset_wing=1;
				pYPadPlate->designInfo.norm.norm_wing=1;
			}
			else								//������ʼ����
			{
				pXPadPlate->designInfo.origin.des_para.RODEND.direction=1;
				pXPadPlate->designInfo.origin.des_para.RODEND.offset_wing=1;
				pXPadPlate->designInfo.norm.norm_wing=1;
				pYPadPlate->designInfo.origin.des_para.RODEND.direction=1;
				pYPadPlate->designInfo.origin.des_para.RODEND.offset_wing=0;
				pYPadPlate->designInfo.norm.norm_wing=0;
			}
		}
		/*else if(base_wing_thick1<base_wing_thick2)
		{
			pXPadPlate->designInfo.origin.datum_pos_style=1;	//�Ǹ��������Ϊ��׼
			pXPadPlate->designInfo.origin.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
			pXPadPlate->designInfo.origin.des_para.RODEND.wing_offset_style=4;
			pXPadPlate->designInfo.origin.des_para.RODEND.wing_offset_dist=base_wing_wide1-pXPadPlate->feature/2;
			_snprintf(pXPadPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pXPadPlate->handle);
			pXPadPlate->designInfo.norm.hVicePart=xDesJointPara.m_pMainJg1->handle;
			pXPadPlate->designInfo.norm.norm_style=1;
			pXPadPlate->designInfo.norm.direction=0;
			if(xDesJointPara.m_pMainJg1->pStart==xDesJointPara.m_pCurNode)	//��ʼ���շ���
			{
				pXPadPlate->designInfo.origin.des_para.RODEND.direction=0;
				pXPadPlate->designInfo.origin.des_para.RODEND.offset_wing=1;
				pXPadPlate->designInfo.norm.norm_wing=1;
			}
			else								//������ʼ����
			{
				pXPadPlate->designInfo.origin.des_para.RODEND.direction=1;
				pXPadPlate->designInfo.origin.des_para.RODEND.offset_wing=0;
				pXPadPlate->designInfo.norm.norm_wing=0;
			}
			pYPadPlate->designInfo.origin.datum_pos_style=1;	//�Ǹ��������Ϊ��׼
			pYPadPlate->designInfo.origin.des_para.RODEND.hRod=xDesJointPara.m_pMainJg1->handle;
			pYPadPlate->designInfo.origin.des_para.RODEND.wing_offset_style=4;
			pYPadPlate->designInfo.origin.des_para.RODEND.wing_offset_dist=base_wing_wide1-pXPadPlate->feature/2;
			_snprintf(pYPadPlate->designInfo.origin.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X,-0X%X",xDesJointPara.m_pMainJg1->handle,pYPadPlate->handle);
			pYPadPlate->designInfo.norm.hVicePart=xDesJointPara.m_pMainJg1->handle;
			pYPadPlate->designInfo.norm.norm_style=1;
			pYPadPlate->designInfo.norm.direction=0;
			if(xDesJointPara.m_pMainJg2->pStart==xDesJointPara.m_pCurNode)	//��ʼ���շ���
			{
				pYPadPlate->designInfo.origin.des_para.RODEND.direction=0;
				pYPadPlate->designInfo.origin.des_para.RODEND.offset_wing=0;
				pYPadPlate->designInfo.norm.norm_wing=0;
			}
			else								//������ʼ����
			{
				pYPadPlate->designInfo.origin.des_para.RODEND.direction=1;
				pYPadPlate->designInfo.origin.des_para.RODEND.offset_wing=1;
				pXPadPlate->designInfo.norm.norm_wing=1;
			}
		}*/
		pXPadPlate->designInfo.origin.UpdatePos(pXPadPlate->BelongModel());
		pXPadPlate->ucs.origin=pXPadPlate->designInfo.origin.Position();
		pYPadPlate->designInfo.origin.UpdatePos(pXPadPlate->BelongModel());
		pYPadPlate->ucs.origin=pYPadPlate->designInfo.origin.Position();
	}
	CreateWrapJg(pOutWrapJg,FALSE);
	CreateWrapJg(pInWrapJg,TRUE);
	if(pXPlate&&!pXPlate->designInfo.m_bEnableFlexibleDesign)
	{
		CreateWrapPlate(pXPlate,0,FALSE);
		pXPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
	}
	if(pYPlate&&!pYPlate->designInfo.m_bEnableFlexibleDesign)
	{
		CreateWrapPlate(pYPlate,1,FALSE);
		pYPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
	}
	CreateWrapPlate(pXPadPlate,0,TRUE);
	CreateWrapPlate(pYPadPlate,1,TRUE);
	return TRUE;
}

void CDesignJoint::CreateWrapJg(CLDSLineAngle *pWrapJg,BOOL bInWrapJg)
{
	if(pWrapJg==NULL)
		return;
	pWrapJg->CopyModuleInstanceInfo(xDesJointPara.m_pMainJg1);
	//λ�ò����趨
	pWrapJg->SetStart(pWrapJg->ucs.origin);
	pWrapJg->desStartPos.SetToDatumPointStyle();
	pWrapJg->desStartPos.wing_x_offset.SetOffset();
	pWrapJg->desStartPos.wing_y_offset.SetOffset();
	pWrapJg->desStartPos.datumPoint.datum_pos_style=7;
	pWrapJg->desStartPos.datumPoint=xDesJointPara.start;
	pWrapJg->desStartPos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
	pWrapJg->SetEnd(pWrapJg->ucs.origin+m_pJoint->ucs.axis_z*m_pJoint->length);
	//pWrapJg->des_end_pos.datum_pos_style=15;
	pWrapJg->desEndPos.SetToDatumPointStyle();
	pWrapJg->desEndPos.wing_x_offset.SetOffset();
	pWrapJg->desEndPos.wing_y_offset.SetOffset();
	pWrapJg->desEndPos.datumPoint.datum_pos_style=7;
	pWrapJg->desEndPos.datumPoint=xDesJointPara.end;
	pWrapJg->desEndPos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
	double offset=0;
	if(bInWrapJg)
	{
		long hThickJg = xDesJointPara.m_pMainJg1->handle;
		if(xDesJointPara.m_pMainJg1->GetThick()<xDesJointPara.m_pMainJg2->GetThick())
			hThickJg = xDesJointPara.m_pMainJg2->handle;
		offset = __max(JointDesDlg.m_fJgANormOffsetX,JointDesDlg.m_fJgANormOffsetY);
		if(offset>0)
			_snprintf(pWrapJg->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X,-%.0f",hThickJg,offset);
		else
			_snprintf(pWrapJg->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"-0X%X",hThickJg);
	}
	else
	{
		offset = __max(JointDesDlg.m_fJgBNormOffsetX,JointDesDlg.m_fJgBNormOffsetY);
		if(offset>0)
			_snprintf(pWrapJg->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"0X%X,%.0f",pWrapJg->handle,offset);
		else
			_snprintf(pWrapJg->desStartPos.datumPoint.des_para.RODEND.norm_offset,MAX_TEMP_CHAR_50,"0X%X",pWrapJg->handle);
	}
	strcpy(pWrapJg->desEndPos.datumPoint.des_para.RODEND.norm_offset,pWrapJg->desStartPos.datumPoint.des_para.RODEND.norm_offset);
	//��ϽǸֽ�ͷ��ƣ��������Ǹֵ���ϼ�϶ֵ��ͬʱ���ڰ������㷨��ƫ�����迼����ϼ�϶��ֵ wxc-2017.4.13
	if(bInWrapJg&&fabs(xDesJointPara.m_fJgSpace1-xDesJointPara.m_fJgSpace2)>1)
	{	
		double space_offset=(xDesJointPara.m_fJgSpace1-xDesJointPara.m_fJgSpace2)*0.5;
		//���ڰ���ʼ�˻��㷨��ƫ�������в���
		if(space_offset>0&&pWrapJg->desStartPos.datumPoint.des_para.RODEND.hRod==xDesJointPara.m_pMainJg2->handle)
			strcat(pWrapJg->desStartPos.datumPoint.des_para.RODEND.norm_offset,CXhChar16(",-%.0f",(fabs(space_offset))));
		else if(space_offset<0&&pWrapJg->desStartPos.datumPoint.des_para.RODEND.hRod==xDesJointPara.m_pMainJg1->handle)
			strcat(pWrapJg->desStartPos.datumPoint.des_para.RODEND.norm_offset,CXhChar16(",-%.0f",(fabs(space_offset))));
		//���ڰ����ն˻��㷨��ƫ�������в���
		if(space_offset>0&&pWrapJg->desEndPos.datumPoint.des_para.RODEND.hRod==xDesJointPara.m_pMainJg2->handle)
			strcat(pWrapJg->desEndPos.datumPoint.des_para.RODEND.norm_offset,CXhChar16(",-%.0f",(fabs(space_offset))));
		else if(space_offset<0&&pWrapJg->desEndPos.datumPoint.des_para.RODEND.hRod==xDesJointPara.m_pMainJg1->handle)
			strcat(pWrapJg->desEndPos.datumPoint.des_para.RODEND.norm_offset,CXhChar16(",-%.0f",(fabs(space_offset))));
	}
	//���߲����趨
	pWrapJg->des_norm_x.bSpecific=TRUE;
	pWrapJg->set_norm_x_wing(xDesJointPara.norm_x_wing.vector,TRUE);
	pWrapJg->des_norm_x.spec_norm=xDesJointPara.norm_x_wing;
	pWrapJg->des_norm_y.bSpecific=TRUE;
	pWrapJg->set_norm_y_wing(xDesJointPara.norm_y_wing.vector,TRUE);
	pWrapJg->des_norm_y.spec_norm=xDesJointPara.norm_y_wing;
	//����ͷ�����趨(������˨��������ͷ)
	pWrapJg->desStartOdd.m_iOddCalStyle=1;
	pWrapJg->desEndOdd.m_iOddCalStyle=1;
}

void CDesignJoint::CreateWrapPlate(CLDSPlate *pWrapPlate,int x_wing0_y_wing1,BOOL bPadPlate)
{
	if(pWrapPlate==NULL)
		return;
	//�������ְ�Y�᷽��(�����ͷ����) wjh-2015.2.2
	pWrapPlate->ucs.axis_y = xDesJointPara.m_pMainJg1->Start()-xDesJointPara.m_pMainJg1->End();//m_pJoint->ucs.axis_z;
	normalize(pWrapPlate->ucs.axis_y);
	if(x_wing0_y_wing1==0)
		pWrapPlate->ucs.axis_z = xDesJointPara.norm_x_wing.vector;
	else
		pWrapPlate->ucs.axis_z = xDesJointPara.norm_y_wing.vector;
	pWrapPlate->ucs.axis_x = pWrapPlate->ucs.axis_y^pWrapPlate->ucs.axis_z;
	normalize(pWrapPlate->ucs.axis_x);
	pWrapPlate->ucs.axis_z = pWrapPlate->ucs.axis_x^pWrapPlate->ucs.axis_y;
}
void CDesignJoint::RetrieveVertexDesPara(CLDSPlate* pWrapPlate,CLDSLineAngle* pMainJg,EDGE_SPACE& edge_space)
{
	if(pWrapPlate==NULL || pMainJg==NULL)
		return;
	SCOPE_STRU scope;
	GEPOINT interspoint;
	for(CLsRef* pLsRef=pMainJg->GetFirstLsRef();pLsRef;pLsRef=pMainJg->GetNextLsRef())
	{
		if(pWrapPlate->FindLsByHandle(pLsRef->pBolt->handle)==NULL)
			continue;
		pWrapPlate->GetBoltIntersPos(pLsRef->pBolt,interspoint);
		coord_trans(interspoint,pWrapPlate->ucs,FALSE,TRUE);
		scope.VerifyVertex(interspoint);
	}
	//����ֲ�������˨��Χ
	CMinDouble minX,minY;
	CMaxDouble maxX,maxY;
	for(PROFILE_VERTEX* pVertex=pWrapPlate->vertex_list.GetFirst();pVertex;pVertex=pWrapPlate->vertex_list.GetNext())
	{
		minX.Update(pVertex->vertex.x);
		maxX.Update(pVertex->vertex.x);
		minY.Update(pVertex->vertex.y);
		maxY.Update(pVertex->vertex.y);
	}
	//���ְ�����ϵ��λ��ʽĬ�ϻ�׼��ΪY��(cDatumAxis='Y')
	UCS_STRU ucs=pWrapPlate->ucs;
	char ciWorkWing=fabs(pMainJg->vxWingNorm*ucs.axis_z)>fabs(pMainJg->vyWingNorm*ucs.axis_z)?'X':'Y';
	GEPOINT vWingVec=ciWorkWing=='X'?pMainJg->GetWingVecX():pMainJg->GetWingVecY();
	GEPOINT vLenVec=(xDesJointPara.m_pCurNode==pMainJg->pStart)?(pMainJg->End()-pMainJg->Start()):(pMainJg->Start()-pMainJg->End());
	int signLenY =vLenVec *ucs.axis_y<0?-1:1;
	int signWingX=vWingVec*ucs.axis_x<0?-1:1;
	if(signLenY>0)
		edge_space.endSpace=ftoi(maxY.number-scope.fMaxY);
	else
		edge_space.endSpace=ftoi(scope.fMinY-minY.number);
	if(signWingX>0)
	{
		edge_space.wingSpace=ftoi(maxX.number-scope.fMaxX);
		edge_space.berSpace =ftoi(scope.fMinX-minX.number);
	}
	else
	{
		edge_space.wingSpace=ftoi(scope.fMinX-minX.number);
		edge_space.berSpace =ftoi(maxX.number-scope.fMaxX);
	}
	//������ȡ����������ļ�������
	int dependPosY=signLenY>0?ftoi(maxY.number):ftoi(minY.number);
	for(PROFILE_VERTEX* pVertex=pWrapPlate->vertex_list.GetFirst();pVertex;pVertex=pWrapPlate->vertex_list.GetNext())
	{
		if(ftoi(pVertex->vertex.y)==dependPosY)
		{	//����pMainJg�ϵĶ���
			pVertex->m_hRod=pMainJg->handle;
			if((ftoi(pVertex->vertex.x)==ftoi(minX.number)&&signWingX>0)||(ftoi(pVertex->vertex.x)==ftoi(maxX.number)&&signWingX<0))
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
			else if((ftoi(pVertex->vertex.x)==ftoi(minX.number)&&signWingX<0)||(ftoi(pVertex->vertex.x)==ftoi(maxX.number)&&signWingX>0))
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
			else
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_NONE;
		}
	}
}
void CDesignJoint::RetrieveWrapPlateLjRodPara(CLDSPlate* pWrapPlate)
{
	if(pWrapPlate==NULL || pWrapPlate->wiType!=CLDSPlate::WRAPPER_PLATE)
		return;
	CLDSLineAngle* pMainJg=NULL;
	for(int i=0;i<2;i++)
	{
		if(i==0)
			pMainJg=xDesJointPara.m_pMainJg1;
		else
			pMainJg=xDesJointPara.m_pMainJg2;
		if(pMainJg==NULL)
			continue;
		BOOL bLjStart=(xDesJointPara.m_pCurNode==pMainJg->pStart)?TRUE:FALSE;
		EDGE_SPACE edge_space;
		RetrieveVertexDesPara(pWrapPlate,pMainJg,edge_space);
		//���������Ӹ˼���Ϣ
		CDesignLjPartPara* pLjPara=pWrapPlate->designInfo.partList.Add(pMainJg->handle);
		pLjPara->iFaceNo=1;
		pLjPara->hPart=pMainJg->handle;
		pLjPara->m_nClassTypeId=CLS_LINEANGLE;
		pLjPara->start0_end1=bLjStart?0:1;
		pLjPara->angle.bTwoEdge=FALSE;
		pLjPara->angle.cur_wing_x0_y1=fabs(pMainJg->vxWingNorm*pWrapPlate->ucs.axis_z)>fabs(pMainJg->vyWingNorm*pWrapPlate->ucs.axis_z)?0:1;
		pLjPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
		pLjPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
		pLjPara->angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
		pLjPara->ber_space=edge_space.berSpace;
		pLjPara->wing_space=edge_space.wingSpace;
		pLjPara->end_space=edge_space.endSpace;
	}
}
#endif