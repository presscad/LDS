#include "stdafx.h"
#include "..\LDS.h"
#include "..\env_def.h"
#include "..\Query.h"
#include "..\GlobalFunc.h"
#include "..\XInsertPlateIntersTubeDlg.h"
//#include "DesignJdb.h"
//#include "MirTaAtom.h"
#include "Tower.h"
#include "..\XhCharString.h"
#include "SortFunc.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __ALFA_TEST_VERSION_

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//���øֹܲ���ϵ���˨�͸��Ӱ�
extern CXInsertPlateIntersTubeDlg inters_tube_dlg;
BOOL Test_LayoutInsertPlateBoltAndAttachPlate(CLDSParamPlate *pParamPlate,char* sPartNoP,
										 char* sPartNoX,BOOL bUseLineAngle,char *sJgGuige)
{
	//if(nBoltNum%2)	//��֤��˨����Ϊż��
	//	nBoltNum+=1;
	double A=0,N=0,M=0,R=0,L=0,H=0,S=0,W=0;
	if(pParamPlate->m_bStdPart)
	{	
		CLDSStdPartModel stdPartModel;
		if(!pParamPlate->GetStdPartModel(&stdPartModel))
			return FALSE;
		//���ɱ�׼�����ϵ���˨
		stdPartModel.CreateBoltInPlate(pParamPlate);
		//���Ʊ�׼�����ϵ���˨
		for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
		A=stdPartModel.param.insert_plate_param.A;
		N=stdPartModel.param.insert_plate_param.N;
		M=stdPartModel.param.insert_plate_param.M;
		R=stdPartModel.param.insert_plate_param.R;
		L=stdPartModel.param.insert_plate_param.L;
		H=stdPartModel.param.insert_plate_param.H;
		S=stdPartModel.param.insert_plate_param.S;
		W=stdPartModel.param.insert_plate_param.W;
	}
	else
	{
		pParamPlate->GetDesignItemValue('A',&A);
		pParamPlate->GetDesignItemValue('N',&N);
		pParamPlate->GetDesignItemValue('M',&M);
		pParamPlate->GetDesignItemValue('R',&R);
		pParamPlate->GetDesignItemValue('L',&L);
		pParamPlate->GetDesignItemValue('H',&H);
		pParamPlate->GetDesignItemValue('S',&S);
		pParamPlate->GetDesignItemValue('W',&W);
		//���ò���ϵ���˨
		logto.Log("error");
		inters_tube_dlg.CreateBolt(pParamPlate);
	}
	
	double x=A+M;
	double y=R;
	double z=0.5*pParamPlate->GetThick();
	CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z����������  Z����������
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{	//����ʮ�ְ��ϵĸ��Ӱ岢�Ҽ��뵽���ӹ����б���
		CString sPartNo;
		//ʮ�ֲ����Ҫ��������
		//ʮ�ֲ�����巨����ƻ�׼����Ϊ����			//wht 10-03-06
		//X-Yƽ���ϵİ��弰��˨������ƻ�׼����Ϊ����
		//X-Zƽ���ϵİ��弰��˨������ƻ�׼����Ϊ��Ӧ������,���巨���޸ĺ���弰��˨�����ܼ�ʱ����
		for(int i=0;i<2;i++)
		{
			CLDSPlate *pVertPlate=(CLDSPlate*)Ta.Parts.append(CLS_PLATE);
			pVertPlate->iSeg=pParamPlate->iSeg;
			pVertPlate->cfgword=pParamPlate->cfgword;
			pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pVertPlate->cMaterial = pParamPlate->cMaterial;
			pVertPlate->layer(2)=pParamPlate->layer(2); //���������ͼ����
			pVertPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
			pVertPlate->set_thick(ftoi(pParamPlate->GetThick()));
			pVertPlate->SetPartNo(sPartNoP);
			pVertPlate->jdb_style=5;	//��׼���������λ�ò���ĸ��Ӱ�
			//.................................................
			//�˴������⾡����ȥʹ��jdb_style �Ժ��޸� 
			//.................................................
			pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
			pVertPlate->designInfo.norm.norm_style=4;	//��׼�����ϵ���Է���
			pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			pVertPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
			pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
			pVertPlate->m_fNormOffset=-z;
			if(i==0)
			{	//Z�����������
				pUpVertPlate=pVertPlate;	//��¼Z����������
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,z);
				pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
				//������Ӧ����˨��������
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==31)//������˨��Z������������
					{
						pVertPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//Z�����������
				pDownVertPlate=pVertPlate;	//��¼Z����������
				pVertPlate->designInfo.origin.datum_pos.Set(0,0,-z);
				pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
				//������Ӧ����˨��������
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{
					if(pLsRef->start1_end2_mid0==41)//������˨��Z������������
					{
						pVertPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
					}
				}
			}
			pVertPlate->DesignSetupUcs();
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0.5*H-z,0));
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0.5*H-z,0));
			pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
			//��������ӵ����ӹ����б���
			pParamPlate->AppendAttachPart(pVertPlate);
		}
		if(bUseLineAngle)
		{	//ʹ�ýǸִ������
			double fAngleY=0.5*pParamPlate->GetThick();	//�̽Ǹֶ�λY����
			double wing_wide=N+R-0.5*pParamPlate->GetThick();
			double wing_thick=W;
			if(sJgGuige)	//����ָ���Ǹֹ�����ɽǸ� wht 11-06-01
				restore_JG_guige(sJgGuige,wing_wide,wing_thick);
			for(i=0;i<4;i++)
			{	//�����ĸ��̽Ǹִ���˿����
				CLDSLineAngle *pShortJg=(CLDSLineAngle*)Ta.Parts.append(CLS_LINEANGLE);
				pShortJg->iSeg=pParamPlate->iSeg;
				pShortJg->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pShortJg->cMaterial = pParamPlate->cMaterial;
				pShortJg->layer(2)=pParamPlate->layer(2); //���������ͼ����
				//�Ǹ�Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
				//pShortJg->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
				pShortJg->set_wing_thick(wing_thick);	//֫��
				pShortJg->set_wing_wide(wing_wide);		//֫��
				sprintf(pShortJg->GetPartNo(),"%s",sPartNoX);
				//λ�ò���
				pShortJg->pOldDesStartPos->datum_pos_style=15;	//ֱ��ָ������
				pShortJg->pOldDesStartPos->datum_point.datum_pos_style=9;	//ָ�������ϵ��������
				pShortJg->pOldDesStartPos->datum_point.des_para.hPart=pParamPlate->handle;
				pShortJg->pOldDesEndPos->datum_pos_style=15;	//ֱ��ָ������
				pShortJg->pOldDesEndPos->datum_point.datum_pos_style=9;	//ָ�������ϵ��������
				pShortJg->pOldDesEndPos->datum_point.des_para.hPart=pParamPlate->handle;
				//���߲���
				pShortJg->des_norm_x.spec_norm.norm_style=4;//ָ�������ϵ���Է���
				pShortJg->des_norm_x.bByOtherWing=FALSE;
				pShortJg->des_norm_x.bSpecific=TRUE;
				pShortJg->des_norm_y.spec_norm.norm_style=4;//ָ�������ϵ���Է���
				pShortJg->des_norm_y.bByOtherWing=FALSE;
				pShortJg->des_norm_y.bSpecific=TRUE;
				if(i==0)
				{	//X-Yƽ��: Y��Z��
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,-fAngleY,z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,-fAngleY,z);
					//X֫���߷�����Z����������� Y֫���߷��������
					pShortJg->des_norm_x.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,-1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Yƽ��: Y��Z��
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,fAngleY,z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,fAngleY,z);
					//Y֫���߷�����Z����������� X֫���߷��������
					pShortJg->des_norm_y.spec_norm.hVicePart=pUpVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
							||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Yƽ��: Y��Z��
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,-fAngleY,-z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,-fAngleY,-z);
					//X֫���߷�����Z����������� Y֫���߷��������
					pShortJg->des_norm_y.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
							||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				} 
				else if(i==3)
				{	//X-Yƽ��: Y��Z��
					pShortJg->pOldDesStartPos->datum_point.datum_pos.Set(x,fAngleY,-z);
					pShortJg->pOldDesEndPos->datum_point.datum_pos.Set(x+S,fAngleY,-z);
					//Y֫���߷�����Z����������� X֫���߷��������
					pShortJg->des_norm_x.spec_norm.hVicePart=pDownVertPlate->handle;
					pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
					pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
					pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
					//������˨
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
							||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pShortJg->AppendMidLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
						}
					}
				}
				pShortJg->ClearFlag();
				pShortJg->CalPosition();
				pShortJg->cal_x_wing_norm();
				pShortJg->cal_y_wing_norm();
				pShortJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pShortJg->GetSolidPartObject());
				//��������ӵ����ӹ����б���
				pParamPlate->AppendAttachPart(pShortJg);
			}
		}
		else
		{	//����Ϊ��ͨ�ְ�
			for(i=0;i<8;i++)
			{
				CLDSPlate *pPlate=(CLDSPlate*)Ta.Parts.append(CLS_PLATE);
				pPlate->iSeg=pParamPlate->iSeg;
				pPlate->cfgword=pParamPlate->cfgword;
				pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
				pPlate->cMaterial = pParamPlate->cMaterial;
				pPlate->layer(2)=pParamPlate->layer(2); //���������ͼ����
				//����Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
				//pPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
				pPlate->set_thick(ftoi(W));
				strcpy(pPlate->GetPartNo(),sPartNoX);
				pPlate->jdb_style=5;	//��׼���������λ�ò���ĸ��Ӱ�
				pPlate->designInfo.m_hBasePart=pParamPlate->handle;
				pPlate->designInfo.norm.norm_style=4;	//��׼�����ϵ���Է���
				pPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
				if(i==0)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==1)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,-1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,-y,-z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==2)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,z);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==3)
				{	//X-Yƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
					pPlate->designInfo.origin.datum_pos.Set(x,y,-z);
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Yƽ��:Y������һ�����˨
						if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==4)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//�ϲ�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//�ϲ�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=������
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==5)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//�ϲ�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//�ϲ�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=������
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==6)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//�²�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//�²�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=������
					pPlate->m_fNormOffset=-W;
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				else if(i==7)
				{	//X-Zƽ��: Y��Z��
					pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//�²�����
					pPlate->designInfo.norm.nearVector.Set(0,0,1);
					pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//�²�����
					pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=������
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{	//X-Zƽ��:Z������һ�����˨
						if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
						{
							pPlate->AppendLsRef(*pLsRef);
							(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
						}
					}
				}
				pPlate->DesignSetupUcs();
				pPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(S,0,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(S,N,0));
				pPlate->vertex_list.append(PROFILE_VERTEX(0,N,0));
				pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
				//��������ӵ����ӹ����б���
				pParamPlate->AppendAttachPart(pPlate);
			}
		}
	}
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{	
		if(pParamPlate->m_iParamType==TYPE_XEND) 
		{
			//����˨��������/����������˨����ƫ����
			(*pLsRef)->des_base_pos.norm_offset.AddThick(-(long)W);	
			//����ʮ�ֲ����˨������Ʋ����Լ�λ����Ʋ���	wht 10-03-06
			if(pUpVertPlate&&pDownVertPlate)
			{	//X-Zƽ��:Z������һ�����˨
				if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pUpVertPlate->handle;
					//����λ�ò���
					(*pLsRef)->des_base_pos.hPart=pUpVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=������
					(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
				}
				//X-Zƽ��:Z������һ�����˨
				else if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
				{
					(*pLsRef)->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
					(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
					(*pLsRef)->des_work_norm.hVicePart=pDownVertPlate->handle;
					//����λ�ò���
					(*pLsRef)->des_base_pos.hPart=pDownVertPlate->handle;
					double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
					double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
					(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=������
					(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
				}
			}
		}
		(*pLsRef)->correct_pos();
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
	return TRUE;
}

#endif
#endif