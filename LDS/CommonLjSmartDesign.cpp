// CommonLjSmartDesign.cpp: implementation of the CCommonLjSmartDesign class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "GUID.h"
#include "UI.h"
#include "env_def.h"
#include "database.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "LibraryQuery.h"
#include "CommonLjSmartDesign.h"
#include "SortFunc.h"
#include "BaseFoot.h"
#include "FootDesignTemplate.h"
#include "UnifyModelLayer.h"
#include "NewPartsListen.h"
#include "Deg.h"
#include "LdsLocale.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __ANGLE_PART_INC_
CCommonLjSmartDesign::CCommonLjSmartDesign()
{
	m_pPlate=NULL;
	m_iFirstEndLjAngle=m_iSecEndLjAngle=-1;
	m_ciRayEndFaceOffset=0;
	m_pFirstEndLjBolt=m_pSecondEndLjBolt=NULL;
	m_nBaseStartG3=m_nBaseEndG3=0;
	wing_intersline_style=0;
}

CCommonLjSmartDesign::~CCommonLjSmartDesign()
{

}
//from PartDesign.cpp
int CalEndLjFirstBoltOffsetDist(CLDSBolt *pFirstBolt,CLDSLineAngle *pDatumLineAngle,
								CLDSLineAngle *pCrossAngle,int nFirstLsJgSpace=0);//from PartDesign.cpp
//from NewPlate.cpp
//�����ͨ�ְ�
BOOL LayoutInsertPlateBoltAndAttachPlate(CLDSParamPlate *pParamPlate,char* sPartNoP,
										 char* sPartNoX,BOOL bUseLineAngle);
static int SteelMatPriority(char cMat)
{
	if(cMat==CSteelMatLibrary::Q235BriefMark())//'S')
		return 5;
	if(cMat==CSteelMatLibrary::Q345BriefMark())//'H')
		return 4;
	if(cMat==CSteelMatLibrary::Q390BriefMark())//'G')
		return 3;
	if(cMat==CSteelMatLibrary::Q420BriefMark())//'T')
		return 2;
	if(cMat==CSteelMatLibrary::Q460BriefMark())//'U')
		return 1;
	return 5;
}
//������˨�����Լ���˨����������˨
static void AutoLayoutAngleBolts(CLDSLineAngle *pCurLineAngle,CConnectPointInfo *pConnectPoint,CLDSPlate *pDatumPlate,
								 CConnectInfo *pLsInfo=NULL,BOOL bEndLj = FALSE,CLDSLineAngle *pCrossLineAngle = NULL,BOOL bAllEndLj=FALSE)
{
	if(pCurLineAngle==NULL||pConnectPoint==NULL||pDatumPlate==NULL)
		return;	
	if(pCurLineAngle->pStart==NULL||pCurLineAngle->pEnd==NULL)
		return;
	CLDSLinePart *pDatumPart = (CLDSLinePart *)Ta.Parts.FromHandle(pConnectPoint->m_hBasePart);
	JGZJ jgzj;
	getjgzj(jgzj,pCurLineAngle->GetWidth());
	CMultiOffsetPos *pDesPosPara=NULL;
	int i=0,j=0,d=16,bolt_num=2,bolt_rows=1,iter_num=0;
	if(pCurLineAngle->handle!=pConnectPoint->m_hBasePart&&pCurLineAngle->pStart->handle==pConnectPoint->m_hBaseNode)
	{	//���߽Ǹ�ʼ����˨��Ϣ
		bolt_rows=pCurLineAngle->connectStart.rows;
		iter_num=bolt_num=pCurLineAngle->connectStart.wnConnBoltN;
		d=pCurLineAngle->connectStart.d;
		pDesPosPara=&pCurLineAngle->desStartPos;
	}	
	else if(pCurLineAngle->handle!=pConnectPoint->m_hBasePart&&pCurLineAngle->pEnd->handle==pConnectPoint->m_hBaseNode)
	{	//���߽Ǹ��ն���˨��Ϣ
		bolt_rows=pCurLineAngle->connectEnd.rows;
		iter_num=bolt_num=pCurLineAngle->connectEnd.wnConnBoltN;
		d=pCurLineAngle->connectEnd.d;
		pDesPosPara=&pCurLineAngle->desEndPos;
	}
	else if(pLsInfo)
	{	//��׼�Ǹ���˨��Ϣ
		d=pLsInfo->d;
		bolt_num=pLsInfo->wnConnBoltN;
		bolt_rows=pLsInfo->rows;
	}
	//˫�Ŷ�����Ĭ��Ϊ����Ҹ�
	if (bolt_rows==2&&bEndLj)
		g_sysPara.intelligentConnect.m_iBoltLayoutStyle = 1;
	//������˨λ���Լ��Ǹ�ƫ����
	double ddx=pDatumPlate->designInfo.norm.vector*pCurLineAngle->get_norm_x_wing();
	double ddy=pDatumPlate->designInfo.norm.vector*pCurLineAngle->get_norm_y_wing();
	int offset_X_dist_style=0,offset_Y_dist_style=4;
	double offset_X_dist=0,offset_Y_dist=0;
	if( fabs(ddx) > fabs(ddy))
	{
		offset_X_dist_style=4;
		offset_Y_dist_style=0;
		if (pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			offset_X_dist=-0.5*pDatumPlate->GetThick();
		else
		{
			if(ddx>0)
			{
				if(bEndLj&&pCrossLineAngle)
					offset_X_dist=-pCrossLineAngle->GetThick();
				else 
					offset_X_dist=0;
			}
			else
				offset_X_dist=-pDatumPlate->GetThick();
		}
	}
	else
	{
		offset_X_dist_style=0;
		offset_Y_dist_style=4;
		if(pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			offset_Y_dist=-0.5*pCrossLineAngle->GetThick();
		else
		{
			if(ddy>0)
			{
				if(bEndLj&&pCrossLineAngle)
					offset_Y_dist=-pCrossLineAngle->GetThick();
				else 
					offset_Y_dist=0;
			}
			else
				offset_Y_dist=-pDatumPlate->GetThick();
		}
	}
	if(pDesPosPara)
	{	//�������߽Ǹ�ƫ����
		pDesPosPara->wing_x_offset.gStyle=offset_X_dist_style;
		pDesPosPara->wing_y_offset.gStyle=offset_Y_dist_style;
		pDesPosPara->wing_x_offset.offsetDist=offset_X_dist;
		pDesPosPara->wing_y_offset.offsetDist=offset_Y_dist;
	}
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,d);
	double len_offset=LsSpace.EndSpace,wing_offset=0;
	double offsetLast[3] = {0};
	for(i=0;i<bolt_rows;i++)
	{
		iter_num=bolt_num;
		//˫�Ų�����˨ʱ�����˨��Ϊ����һ�㱣֤��������˨�׸���˨Ϊ�˾�
		//�ʵ�����˨���÷�ʽʱ��֤��������˨λ�ڸߵ�һ�༴�� wht 10-01-15
		if(bolt_rows==2)
		{
			if(bolt_num%2==0)
				iter_num=bolt_num/2+(bolt_num%2*i);
			//��(֫���)����(���߲�)�Ͳ���˫����˨
			else if(g_sysPara.intelligentConnect.m_iBoltLayoutStyle==0)
			{
				if(i==1)//��(֫���)
					iter_num=bolt_num/2+bolt_num%2;
				else	//��(���߲�)
					iter_num=bolt_num/2;
			}
			//��(֫���)����(���߲�)�߲���˫����˨
			else if(g_sysPara.intelligentConnect.m_iBoltLayoutStyle==1)
			{
				if(i==1)//��(֫���)
					iter_num=bolt_num/2;
				else	//��(���߲�)
					iter_num=bolt_num/2+bolt_num%2;
			}
		}
		if(bolt_rows==1)
		{	//������˨
			if(pDesPosPara)	//���߽Ǹ�
				len_offset=LsSpace.EndSpace;
			else			//��׼�Ǹ�
				len_offset=-(iter_num-1)*LsSpace.SingleRowSpace/2;
			wing_offset=jgzj.g-jgzj.g;
		}
		else
		{	//˫����˨
			if(pDesPosPara)	//���߽Ǹ�
			{
				if(bolt_num%2==0)
				{
					//��(֫���)����(���߲�)�Ͳ���˫����˨
					if(g_sysPara.intelligentConnect.m_iBoltLayoutStyle==0)
						len_offset=LsSpace.EndSpace+i*LsSpace.doubleRowSpace/2;
					//��(֫���)����(���߲�)�߲���˫����˨
					else
					{
						if(i==0)//��(֫���)
							len_offset=LsSpace.EndSpace+LsSpace.doubleRowSpace/2;
						else	//��(���߲�)
							len_offset=LsSpace.EndSpace;
					}
				}
				else
				{
					if(iter_num%2==0)	//ż����
						len_offset=LsSpace.EndSpace+LsSpace.doubleRowSpace/2;
					else				//������
						len_offset=LsSpace.EndSpace;
				}
			}
			else			//��׼�Ǹ�
			{
				if(bolt_num%2==0)	//��˨����Ϊż��
				{
					//��(֫���)����(���߲�)�Ͳ���˫����˨
					if(g_sysPara.intelligentConnect.m_iBoltLayoutStyle==0)
						len_offset=-(iter_num-1)*LsSpace.doubleRowSpace/2-i*LsSpace.doubleRowSpace/2;
					//��(֫���)����(���߲�)�߲���˫����˨
					else
					{
						if(i==0)//��(֫���)
							len_offset=-(iter_num-1)*LsSpace.doubleRowSpace/2-LsSpace.doubleRowSpace/2;
						else	//��(���߲�)
							len_offset=-(iter_num-1)*LsSpace.doubleRowSpace/2;
					}
				}
				else				//��˨����Ϊ����
				{
					//if(iter_num%2==0)	//ż����
					//	len_offset=-(iter_num-1)*LsSpace.doubleRowSpace/2-i*LsSpace.doubleRowSpace/2;
					//else				//������
						len_offset=-(iter_num-1)*LsSpace.doubleRowSpace/2;
				}
			}
			if(i==0)//���߲���˨(��)
				wing_offset=jgzj.g1-jgzj.g;
			else	//֫�����˨(��)
				wing_offset=jgzj.g2-jgzj.g;
		}
		for(j=0;j<iter_num;j++)
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->AddL0Thick(pCurLineAngle->handle,TRUE);
			pBolt->AddL0Thick(pDatumPlate->handle,TRUE);
			pBolt->set_d(d);
			pBolt->SetLayer(pCurLineAngle->layer());
			pBolt->cfgword=pCurLineAngle->cfgword;
			pBolt->iSeg = pDatumPart->iSeg;
			pDatumPlate->AppendLsRef(pBolt->GetLsRef());
			if(pDesPosPara)
			{	//���߽Ǹ��ϵ���˨λ���Լ�������Ʋ���
				pBolt->des_work_norm.norm_style=1;	//�Ǹ�֫����
				pBolt->des_work_norm.hVicePart=pCurLineAngle->handle;
				if(fabs(ddx) > fabs(ddy))
				{
					if(ddx>0)
						pBolt->des_work_norm.direction=0;
					else
						pBolt->des_work_norm.direction=1;
					pBolt->des_work_norm.norm_wing=0;
				}
				else
				{
					if(ddy>0)
						pBolt->des_work_norm.direction=0;
					else
						pBolt->des_work_norm.direction=1;
					pBolt->des_work_norm.norm_wing=1;
				}
				//
				if(pCurLineAngle->pStart->handle==pConnectPoint->m_hBaseNode)
				{
					pCurLineAngle->AppendStartLsRef(pBolt->GetLsRef());
					pBolt->des_base_pos.direction=0;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=0;
					pCurLineAngle->desStartOdd.m_iOddCalStyle = 0;	//������ײ����Ǹ�����ͷ
				}
				else if(pCurLineAngle->pEnd->handle==pConnectPoint->m_hBaseNode)
				{
					pCurLineAngle->AppendEndLsRef(pBolt->GetLsRef());
					pBolt->des_base_pos.direction=1;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.direction=1;
					pCurLineAngle->desEndOdd.m_iOddCalStyle = 0;	//������ײ����Ǹ�����ͷ
					}
				//�����߽Ǹ�Ϊ���������߽Ǹ�,�����������˨��ĵ�һ����˨��λ��
				if(bEndLj&&pCrossLineAngle)
				{	//��˨���㶨λ��ʽΪ�Ǹ����߽���
					pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pCurLineAngle->handle;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pCrossLineAngle->handle;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
					
					//������˨��׼�϶
					if(j==0)
					{
						CLDSLineAngle *pDatumAngle = (CLDSLineAngle *)pDatumPart;
						len_offset=CalEndLjFirstBoltOffsetDist(pBolt,pCurLineAngle,pDatumAngle);
						//��Ϊ�е��Ӧ������ƫ��ֵ��ֹ�����������ײ
						len_offset += 8;
						j++; //�˴����ò������׼�Ǹֹ��õ���˨����׼�Ǹ����Ѳ��ù���
					}
				}
				else
				{	//�Ǹ���㶨λ
					pBolt->des_base_pos.datumPoint.datum_pos_style=1;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.hRod=pCurLineAngle->handle;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_dist=jgzj.g;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.wing_offset_style=0;
				}
				pBolt->des_base_pos.len_offset_dist=len_offset;
				pBolt->des_base_pos.wing_offset_dist=wing_offset;
				pBolt->des_base_pos.hPart=pCurLineAngle->handle;
				if(fabs(ddx) > fabs(ddy))
				{
					if(ddx>0)
						pBolt->des_base_pos.norm_offset.AddThick(-pCurLineAngle->handle,TRUE);
					else
						pBolt->des_base_pos.norm_offset.AddThick(-pDatumPlate->handle,TRUE);
					pBolt->des_base_pos.offset_wing=0;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=0;
				}
				else
				{
					if(ddy>0)
						pBolt->des_base_pos.norm_offset.AddThick(-pCurLineAngle->handle,TRUE);
					else
						pBolt->des_base_pos.norm_offset.AddThick(-pDatumPlate->handle,TRUE);
					pBolt->des_base_pos.offset_wing=1;
					pBolt->des_base_pos.datumPoint.des_para.RODEND.offset_wing=1;
				}
				pBolt->des_base_pos.len_offset_dist=len_offset;
			}
			else
			{	//��׼�Ǹ���˨��Ʋ���
				pBolt->des_work_norm=pDatumPlate->designInfo.norm;
				pDatumPlate->AppendLsRef(pBolt->GetLsRef());
				pCurLineAngle->AppendMidLsRef(pBolt->GetLsRef());
				//
				if(bEndLj&&pCrossLineAngle)
				{	//�����ӵ�����˨��λ��ʽΪ���߽���
					pBolt->des_base_pos.datumPoint.datum_pos_style=3;	//���߽���
					pBolt->des_base_pos.hPart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1 = pCurLineAngle->handle;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pCrossLineAngle->handle;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=0;
					pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
					pBolt->des_base_pos.wing_offset_dist=wing_offset;
					int offsetTemp = 0;//����CalEndLjFirstBoltOffsetDist������˨ƫ����ʱ��ʾƫ�Ʒ���
					if (j==0)
						offsetTemp = -1;
					else
						offsetTemp = 1;
					pBolt->des_base_pos.len_offset_dist=offsetTemp;
					//��׼�˼���˨����Ӧ�÷ֱ������߽Ǹ���ȡƫ������ȡ�������ֵ
					int nFirstLsJgSpace=0;
					for(CLDSLinePart* pLinePart = pConnectPoint->linkPartSet.GetFirst();pLinePart;pLinePart=pConnectPoint->linkPartSet.GetNext())
					{
						CLDSLineAngle *pLineAngle = NULL;
						if(pLinePart->IsAngle())
							pLineAngle =(CLDSLineAngle*)pLinePart;
						int tempLsOffset = CalEndLjFirstBoltOffsetDist(pBolt,pCurLineAngle,pLineAngle);
						if (tempLsOffset>nFirstLsJgSpace)
							nFirstLsJgSpace = tempLsOffset;
						if (!bAllEndLj)
							break;
					}
					if(j==0)
						nFirstLsJgSpace += offsetTemp;
					else
						nFirstLsJgSpace -= offsetTemp;
					//����������˨��������ʱ������˨ӦΪ��׼��
					if (i==0)
					{
						if(j==0)
						{
							if (bolt_rows==2)
							{
								len_offset = -nFirstLsJgSpace;
								offsetLast[1] = len_offset;
							}
							else
								len_offset-=(nFirstLsJgSpace-LsSpace.SingleRowSpace);	
						}
						else if(j==1)	//������˨
							len_offset=0;
						else if(j==2)	//�����²���˨
						{
							if(bolt_rows==2)
							{
								len_offset = nFirstLsJgSpace;
								offsetLast[2] = len_offset;
							}
							else
								len_offset+=(nFirstLsJgSpace-LsSpace.SingleRowSpace);
						}
						else if(j%2==1)
						{
							len_offset = offsetLast[1]-LsSpace.doubleRowSpace;
							offsetLast[1] = len_offset;
						}
						else if(j%2==0)
							len_offset = offsetLast[2]+LsSpace.doubleRowSpace*((j-1)/2);
					}
					else if(i==1)
					{
						if (j==0)
						{
							len_offset=-nFirstLsJgSpace;
							offsetLast[0] = nFirstLsJgSpace;
							offsetLast[1] = len_offset;
						}
						else if(j==1)
						{
							len_offset = nFirstLsJgSpace;
							offsetLast[2] = len_offset;
						}
						else if (j%2==0)
						{
							len_offset = offsetLast[1]-LsSpace.doubleRowSpace;
							offsetLast[1] = len_offset;
						}
						else
						{
							len_offset = offsetLast[2]+LsSpace.doubleRowSpace;
							offsetLast[2] = len_offset;
						}
					}
				}
				else
					pBolt->des_base_pos.datumPoint=pDatumPlate->designInfo.origin;
				pBolt->des_base_pos.len_offset_dist=len_offset;
				pBolt->des_base_pos.offset_wing=(BYTE)pDatumPlate->designInfo.norm.norm_wing;
				pBolt->des_base_pos.hPart=pCurLineAngle->handle;
				pBolt->des_base_pos.norm_offset.AddThick(-pCurLineAngle->handle,TRUE);
			}
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();	
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			if(bolt_rows==2)
				len_offset+=LsSpace.doubleRowSpace;
			else
				len_offset+=LsSpace.SingleRowSpace;
		}
	}
	//���߽Ǹ�Ϊ������ʱ��������˨���õ����߽Ǹ��� 
	if(pDesPosPara&&bEndLj&&pCrossLineAngle)
	{
		for (CLsRef* pLsRef = pDatumPlate->GetFirstLsRef();pLsRef;pLsRef = pDatumPlate->GetNextLsRef())
		{
			f3dPoint pos,bolt_vec,wing_vec;
			CLDSBolt *pBoltTemp = pLsRef->GetLsPtr();
			CLDSLineAngle *pDatumAngle = (CLDSLineAngle *)pDatumPart;
			if(pDatumAngle->FindLsByHandle(pBoltTemp->handle))
				pBoltTemp->layer(2) = pCurLineAngle->layer(2);//������˨ͼ�������߸˼�һ��
			if(fabs(pBoltTemp->ucs.axis_z*pCurLineAngle->get_norm_x_wing())>
				fabs(pBoltTemp->ucs.axis_z*pCurLineAngle->get_norm_y_wing()))
			{
				wing_vec=pCurLineAngle->GetWingVecX();
				Int3dlf(pos,pBoltTemp->ucs.origin,pBoltTemp->ucs.axis_z,
					pCurLineAngle->Start(),pCurLineAngle->get_norm_x_wing());
			}
			else
			{
				wing_vec=pCurLineAngle->GetWingVecY();
				Int3dlf(pos,pBoltTemp->ucs.origin,pBoltTemp->ucs.axis_z,
					pCurLineAngle->Start(),pCurLineAngle->get_norm_y_wing());
			}
			bolt_vec=pos-pCurLineAngle->Start();
			double dd=bolt_vec*wing_vec;
			if(dd>0&&dd<pCurLineAngle->GetWidth())
			{	
				BYTE x_wing0_y_wing1;
				pCurLineAngle->AppendMidLsRef(*pLsRef);
				if(pDatumAngle->FindLsByHandle(pBoltTemp->handle))
				{
					pBoltTemp->AddL0Thick(pCurLineAngle->handle,TRUE);
					if(IsInsideJg(pCurLineAngle,pConnectPoint->datumplane.vSolid,&x_wing0_y_wing1))
						pBoltTemp->des_base_pos.norm_offset.AddThick(-pCurLineAngle->handle,TRUE);
				}
				else if(IsInsideJg(pCurLineAngle,pConnectPoint->datumplane.vSolid,&x_wing0_y_wing1))
				{
					pBoltTemp->AddL0Thick(pDatumAngle->handle,TRUE);
					pBoltTemp->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2 = pDatumAngle->handle;
				}
				pBoltTemp->CalGuigeAuto();
				pBoltTemp->correct_worknorm();
				pBoltTemp->correct_pos();
				pBoltTemp->SetModified();
				pBoltTemp->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBoltTemp->GetSolidPartObject());
			}
		}
		//�������߽Ǹ�����ͷ���㷽ʽΪ������˨����Ǹ�����ͷ
		if(pCurLineAngle->pStart->handle==pConnectPoint->m_hBaseNode)
		{
			pCurLineAngle->desStartOdd.m_iOddCalStyle=1;	//������˨λ�ü���Ǹ�����ͷ
			pCurLineAngle->CalStartOddment();
		}
		else
		{
			pCurLineAngle->desEndOdd.m_iOddCalStyle=1;		//������˨λ�ü���Ǹ�����ͷ
			pCurLineAngle->CalEndOddment();
		}
		pCurLineAngle->ClearFlag();
		pCurLineAngle->CalPosition();
		pCurLineAngle->SetModified();
		pCurLineAngle->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pCurLineAngle->GetSolidPartObject());
	}
	//���������ʽ������Ӧ�����ӵ��
	if(pDesPosPara&&bEndLj&&pCrossLineAngle)
	{
		BYTE x_wing0_y_wing1;
		if (IsInsideJg(pCurLineAngle,pConnectPoint->datumplane.vSolid,&x_wing0_y_wing1)
			&&pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSPlate *pBoltPad= (CLDSPlate*)console.AppendPart(CLS_PLATE);
			for (CLsRef *pLsRefTemp = pDatumPlate->GetFirstLsRef();pLsRefTemp;pLsRefTemp=pDatumPlate->GetNextLsRef())
			{
				CLDSLineAngle *pDatumAngle = (CLDSLineAngle *)pDatumPart;
				CLDSBolt *pBolt = pLsRefTemp->GetLsPtr();	
				if (pDatumAngle->FindLsByHandle(pBolt->handle))
					continue;
				CLsRef *pLsRef=pBoltPad->AppendLsRef(pBolt->GetLsRef());
			}
			CLDSBolt *pBolt=pBoltPad->GetFirstLsRef()->GetLsPtr();
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->SetLayer(pDatumPart->layer());
			pBoltPad->SetLayer(pBolt->layer());
			pBoltPad->cfgword=pBolt->cfgword=pDatumPart->cfgword;
			//����巨�������Ϣ
			pBoltPad->ucs.axis_z = pConnectPoint->datumplane.vSolid;
			pBoltPad->designInfo.norm.norm_style=1;
			pBoltPad->designInfo.norm.norm_wing=x_wing0_y_wing1;
			pBoltPad->designInfo.norm.direction=0;	//����
			pBoltPad->designInfo.norm.hVicePart=pCurLineAngle->handle;
			
			CLDSLineAngle *pDatumAngle = (CLDSLineAngle *)pDatumPart;
			pBoltPad->Thick=ftoi(pDatumAngle->GetThick());
			pBoltPad->cMaterial=pDatumAngle->cMaterial;
			pBoltPad->SetPartNo(CXhChar16("%s",pDatumAngle->GetPartNo()));
			pBoltPad->iSeg=pDatumAngle->iSeg;
			pBoltPad->m_fNormOffset=0;
			pBoltPad->jdb_style=4;	//��˨���
			//����������ϵ
			f3dPoint inter_pos;
			f3dLine line,datum_line;
			datum_line.startPt=pCurLineAngle->Start();
			datum_line.endPt=pCurLineAngle->End();
			project_point(datum_line.startPt,pCurLineAngle->Start(),pBoltPad->ucs.axis_z);
			project_point(datum_line.endPt,pCurLineAngle->Start(),pBoltPad->ucs.axis_z);
			line.startPt=pDatumAngle->Start();
			line.endPt=pDatumAngle->End();
			project_point(line.startPt,pCurLineAngle->Start(),pBoltPad->ucs.axis_z);
			project_point(line.endPt,pCurLineAngle->Start(),pBoltPad->ucs.axis_z);
			Int3dll(line,datum_line,inter_pos);
			pBoltPad->ucs.axis_x=pCurLineAngle->End()-pCurLineAngle->Start();
			normalize(pBoltPad->ucs.axis_x);
			SnapPerp(&pBoltPad->ucs.origin,pCurLineAngle->ucs.origin,pCurLineAngle->ucs.origin+pBoltPad->ucs.axis_x*1000,pBolt->ucs.origin);
			f3dPoint vec=inter_pos-pBoltPad->ucs.origin;
			if(pBoltPad->GetLsCount()==1)//����һ����˨��ʱ��Ҫ��֤����˨����X�ᴹֱ�и��׼�Ǹ�
			{
				f3dPoint axis_y=pDatumAngle->End()-pDatumAngle->Start();
				pBoltPad->ucs.axis_x=pBoltPad->ucs.axis_z^axis_y;
				normalize(pBoltPad->ucs.axis_x);
			}
			//ѡ�����и���Ļ�׼�Ǹ֣������X�᷽��ָ���и��׼�Ǹ�
			if(pBoltPad->ucs.axis_x*vec<0)
				pBoltPad->ucs.axis_x*=-1.0;
			//�洢��׼�Ǹֺ��и��׼�Ǹ��������ʱ��
			pBoltPad->designInfo.m_hBasePart=pCurLineAngle->handle;
			CDesignLjPartPara *pDesPara=pBoltPad->designInfo.partList.Add(pDatumAngle->handle);
			pDesPara->m_nClassTypeId=CLS_LINEANGLE;
			
			pBoltPad->ucs.axis_y=pBoltPad->ucs.axis_z^pBoltPad->ucs.axis_x;
			normalize(pBoltPad->ucs.axis_y);
			pBoltPad->CalStdProfile();
			if(pDatumAngle&&pBoltPad->GetLsCount()>1)	//�����Զ���˨�������и��
			{	//���и��׼�Ǹ���Ӧ���и���
				f3dPoint face_pos,face_norm;
				face_pos=pDatumAngle->ucs.origin;
				if(fabs(pDatumAngle->get_norm_x_wing()*pBoltPad->ucs.axis_z)>fabs(pDatumAngle->get_norm_y_wing()*pBoltPad->ucs.axis_z))
					face_norm=pDatumAngle->get_norm_y_wing();	//1.Y֫����Ϊ��ʼ�и��淨��
				else if(fabs(pDatumAngle->get_norm_y_wing()*pBoltPad->ucs.axis_z)>fabs(pDatumAngle->get_norm_x_wing()*pBoltPad->ucs.axis_z))
					face_norm=pDatumAngle->get_norm_x_wing();	//2.X֫����Ϊ��ʼ�и��淨��
				//1.Y֫��������X�᷽��ͬ,��Y֫������и���,����ͬ����X֫������и���
				//2.X֫��������X�᷽��ͬ,��X֫������и���,����ͬ����Y֫������и���
				if(face_norm*pBoltPad->ucs.axis_x<0)	
					face_norm*=-1.0;
				else 
					face_pos-=face_norm*pDatumAngle->GetWidth();
				face_pos-=face_norm*g_sysPara.VertexDist;
				CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
				if(!pView->CutPlateByPlane(pBoltPad,face_pos,face_norm))
					return;
			}
			for(CLsRef *pLsRef=pBoltPad->GetFirstLsRef();pLsRef;pLsRef=pBoltPad->GetNextLsRef())
			{
				pLsRef->GetLsPtr()->CalGuigeAuto();
				pLsRef->GetLsPtr()->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				g_pSolidDraw->NewSolidPart(pLsRef->GetLsPtr()->GetSolidPartObject());
			}
			pBoltPad->SetModified();
			pBoltPad->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pBoltPad->GetSolidPartObject());
		}
	}
}
static int BoltSpace(int d,char cType='A')
{	//������˨ֱ���õ���׼�϶
	int nSpace = 0;
	if(cType=='A')
	{
		if(d<=12)
			nSpace = g_sysPara.intelligentConnect.m_n12LsOutterSpace;
		else if(d<=16)
			nSpace = g_sysPara.intelligentConnect.m_n16LsOutterSpace;
		else if(d<=20)
			nSpace = g_sysPara.intelligentConnect.m_n20LsOutterSpace;
		else	
			nSpace = g_sysPara.intelligentConnect.m_n24LsOutterSpace;
	}
	else if(cType=='a')
	{
		if(d<=12)
			nSpace = g_sysPara.intelligentConnect.m_n12LsOutterSpaceHasWasher;
		else if(d<=16)
			nSpace = g_sysPara.intelligentConnect.m_n16LsOutterSpaceHasWasher;
		else if(d<=20)
			nSpace = g_sysPara.intelligentConnect.m_n20LsOutterSpaceHasWasher;
		else	
			nSpace = g_sysPara.intelligentConnect.m_n24LsOutterSpace;
	}
	else if(cType=='B')	//֫��߾�
	{
		if(d<=12)
			nSpace = g_sysPara.intelligentConnect.m_n12LsAngleEdgeSpace;
		else if(d<=16)
			nSpace = g_sysPara.intelligentConnect.m_n16LsAngleEdgeSpace;
		else if(d<=20)
			nSpace = g_sysPara.intelligentConnect.m_n20LsAngleEdgeSpace;
		else	
			nSpace = g_sysPara.intelligentConnect.m_n24LsAngleEdgeSpace;
	}
	else if(cType=='C')	//�����˨��׼��C
	{
		if(d<=12)
			nSpace = g_sysPara.intelligentConnect.m_n12LsOutterStaggerSpace;
		else if(d<=16)								   
			nSpace = g_sysPara.intelligentConnect.m_n16LsOutterStaggerSpace;
		else if(d<=20)								   
			nSpace = g_sysPara.intelligentConnect.m_n20LsOutterStaggerSpace;
		else										   
			nSpace = g_sysPara.intelligentConnect.m_n24LsOutterStaggerSpace;
	}
	else //if(cType=='S')	//������˨��׼��S
	{
		if(d<=12)
			nSpace = g_sysPara.intelligentConnect.m_n12LsShareStaggerSpace;
		else if(d<=16)
			nSpace = g_sysPara.intelligentConnect.m_n16LsShareStaggerSpace;
		else if(d<=20)
			nSpace = g_sysPara.intelligentConnect.m_n20LsShareStaggerSpace;
		else	
			nSpace = g_sysPara.intelligentConnect.m_n24LsShareStaggerSpace;
	}
	return nSpace;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "BraceConnectPoint.h"
bool CCommonLjSmartDesign::DesignBraceConnParams(short* psiStyleG,double* pdfOffsetDistG)	//��Ƹ������ӵ���ز���
{
	BRACE_CONNECT_POINT fxTmpBraceConnState;	//�˱���δ�������Ӧ����CCommonLjSmartDesign����Ա���� wjh-2019.11.9
	fxTmpBraceConnState.niBaseAngleWidth=(short)m_pBaseRod->GetWidth();
	fxTmpBraceConnState.cnBraceRayAngles = 0;
	BRACE_CONNECT_POINT::RAYANGLE xarrBraceAngles[4];	//Ŀǰ����֧�������߽Ǹ�
	fxTmpBraceConnState.xarrRayAngles = xarrBraceAngles;
	//������߸˼�
	for (CLDSLineAngle* pRayAngle = (CLDSLineAngle*)m_pConnectPoint->linkPartSet.GetFirst(); pRayAngle != NULL;
		pRayAngle = (CLDSLineAngle*)m_pConnectPoint->linkPartSet.GetNext(), fxTmpBraceConnState.cnBraceRayAngles++)
	{
		if (!pRayAngle->IsAuxPole())
		{	//����ȫ�����߸�������
			fxTmpBraceConnState.cnBraceRayAngles = 0;
			break;
		}
		f3dPoint rayVecPos = pRayAngle->Start() - pRayAngle->End();
		if (pRayAngle->pStart == m_pBaseNode)
			rayVecPos *= -1;
		if (fxTmpBraceConnState.cnBraceRayAngles>=4)
		{
			if (m_pBaseNode)
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�ڵ㴦���ӵĸ�����Ŀ����(>4)���ṹ�д���",this->m_pBaseNode->handle);
			continue;
		}
		BRACE_CONNECT_POINT::RAYANGLE* pCurrRayAngleInfo = &xarrBraceAngles[fxTmpBraceConnState.cnBraceRayAngles];
		BYTE ciRayAngleWorkWing = 0;
		GEPOINT vRayVec = ucs.TransVToCS(rayVecPos);
		pCurrRayAngleInfo->siRayAngle = (short)ftoi(DEG(vRayVec));
		pCurrRayAngleInfo->blInsideAngle = IsInsideJg(pRayAngle, base_wing_norm, &ciRayAngleWorkWing) != FALSE;
		if (pRayAngle->pStart == this->m_pBaseNode)
		{
			pCurrRayAngleInfo->ciBoltD = (BYTE)pRayAngle->connectStart.d;
			pCurrRayAngleInfo->ciBoltN = (BYTE)pRayAngle->connectStart.wnConnBoltN;
		}
		else
		{
			pCurrRayAngleInfo->ciBoltD = (BYTE)pRayAngle->connectEnd.d;
			pCurrRayAngleInfo->ciBoltN = (BYTE)pRayAngle->connectEnd.wnConnBoltN;
		}
	}
	
	if (fxTmpBraceConnState.cnBraceRayAngles > 0 &&fxTmpBraceConnState.DesignConnectPoint() &&
		fxTmpBraceConnState.xarrRayAngles[0].ciBaseStyleG>=0)
	{
		if(fxTmpBraceConnState.xarrRayAngles[0].ciBaseStyleG<5)
				*psiStyleG= fxTmpBraceConnState.xarrRayAngles[0].ciBaseStyleG;
		if(fxTmpBraceConnState.xarrRayAngles[0].ciBaseStyleG==4)
			*pdfOffsetDistG = fxTmpBraceConnState.xarrRayAngles[0].siOffsetG;
		if(fxTmpBraceConnState.xarrRayAngles[0].ciBaseStyleG>=5)
		{
			*psiStyleG=4;
			*pdfOffsetDistG=fxTmpBraceConnState.niBaseAngleWidth+fxTmpBraceConnState.xarrRayAngles[0].siOffsetG;
		}
		return true;
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////
void CCommonLjSmartDesign::LayoutBaseAngleBolts()
{
	if(m_pPlate==NULL)	
		return;	//�ް�ʱ���ж�������˨������ǰ���ã�����Ҫ���⴦��
	int i=0,j=0;
	int bolt_num=xBaseAngleBolts.wnBoltN;
	int bolt_rows=1;
	if(bolt_num>=5)
		bolt_rows=2;
	else
		bolt_rows=1;
	int nEndLjBoltNum=m_pPlate->GetLsCount();
	bolt_num-=nEndLjBoltNum;	//�������Ѳ���˨Ϊ��������˨��Ӧ�ӻ�׼����˨���п۳�
	if(m_jgzjBase.g1<=0||m_jgzjBase.g2<=0)
		bolt_rows=1;	//��������˨������һ��˵����׼���ϵĶ�������˨Ϊ���Ų���
	//else if(nEndLjBoltNum>1)
	//	bolt_rows=2;	//��������˨������һ��˵����׼���ϵĶ�������˨Ϊ˫�Ų���
	if (nEndLjBoltNum == 1)
	{
		bolt_rows = 1;
		this->m_nBaseStartG3 = this->m_nBaseEndG3 = m_jgzjBase.g;
	}
	
	CLDSBolt *pBolt;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace, xBaseAngleBolts.wiBoltD);
	CLDSVector des_bolt_norm;
	des_bolt_norm.norm_style=1;
	des_bolt_norm.hVicePart=m_pBaseRod->handle;
	des_bolt_norm.norm_wing=m_iBaseAngleWorkWing;
	des_bolt_norm.direction=0;
	des_bolt_norm.UpdateVector(&Ta);
	int up_endlj,down_endlj;
	if(m_iFirstEndLjAngle>=0&&m_iSecEndLjAngle>=0)
	{
		if(fabs(m_arrRayRod[m_iFirstEndLjAngle].alfa)>fabs(m_arrRayRod[m_iSecEndLjAngle].alfa))
		{
			up_endlj=m_iFirstEndLjAngle;
			down_endlj=m_iSecEndLjAngle;
		}
		else
		{
			up_endlj=m_iSecEndLjAngle;
			down_endlj=m_iFirstEndLjAngle;
		}
	}
	else
		up_endlj=down_endlj=m_iFirstEndLjAngle;
	CLDSPoint up_point,down_point;	//�ػ�׼����ʼ�˲�ƫ��Ϊ�ϣ��ն˲�ƫ��Ϊ��
	f3dPoint face_offset_norm;		//��׼�˼�ʼ��ĩ�˵�ƽ���淨��
	if(m_pBaseRod.LineAnglePointer()->desStartPos.IsFaceOffset())
		face_offset_norm=m_pBaseRod.LineAnglePointer()->desStartPos.face_offset_norm;
	else if(m_pBaseRod.LineAnglePointer()->desEndPos.IsFaceOffset())
		face_offset_norm=m_pBaseRod.LineAnglePointer()->desEndPos.face_offset_norm;
	//���û�׼�Ǹ���˨ʱ��3.�ǹ����νӶνضϵ����Ӵ�Ҳ��Ϊ������µ㴦�Դ�����׼���׿���˨�޷����ڻ��� wjh-2019.9.4
	//2.�νӶ�ĩ�������������׼������˨Ҳ��Ҫ����������ͬ���ǣ��׿���˨���������������ĵĴ�ӻ��㴦 wjh-2019.9.4
	bool blAsTransitNodePlate=m_pConnectPoint->m_bTransitConnect||
		(m_pConnectPoint->m_ciBaseRodConnState==2)||(m_pConnectPoint->m_ciBaseRodConnState==3);
	CStackVariant<bool> stackprop(&m_pConnectPoint->m_bTransitConnect,blAsTransitNodePlate);
	if(bolt_rows==1)
	{	//������˨����
		double up_offset=0,down_offset=0;
		if(nEndLjBoltNum==1)	//����˨������
		{
			f3dPoint len_vec=m_pBaseRod->End()-m_pBaseRod->Start();
			normalize(len_vec);
			up_point.datum_pos_style=3;
			up_point.des_para.AXIS_INTERS.hDatum1=m_pBaseRod->handle;
			up_point.des_para.AXIS_INTERS.wing_offset_style1=0;
			up_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[up_endlj].Angle()->handle;
			up_point.des_para.AXIS_INTERS.wing_offset_style2=4;
			if(m_arrRayRod[up_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[up_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			down_point=up_point;
			down_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[down_endlj].Angle()->handle;
			if(m_arrRayRod[down_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[down_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			if(m_pFirstEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1==0)
			{	//��������˨λ�ڱ�׼��ʱ��������׼����˨���Բ������
				up_point.UpdatePos(m_pBaseRod->BelongModel());
				down_point.UpdatePos(m_pBaseRod->BelongModel());
				f3dPoint up_vec=up_point.Position()-m_pFirstEndLjBolt->des_base_pos.datumPoint.Position();
				f3dPoint down_vec=down_point.Position()-m_pFirstEndLjBolt->des_base_pos.datumPoint.Position();
				up_offset=-up_vec*len_vec;
				down_offset=down_vec*len_vec;
				up_point=m_pFirstEndLjBolt->des_base_pos.datumPoint;
				strcpy(up_point.des_para.AXIS_INTERS.norm_offset,"");
				down_point=up_point;
				up_offset=ceil(up_offset);
				down_offset=ceil(down_offset);
				int sign_up=1,sign_down=1;
				if(up_offset<0)
					sign_up=-1;
				if(down_offset<0)
					down_offset=-1;
				up_offset*=sign_up;
				down_offset*=sign_down;
				//��������׼�϶A�Ļ�������5Ϊ�ݶȽ����ֵ����Բ��
				if(ftoi(up_offset)%5>0)
					up_offset=up_offset+5-ftoi(up_offset)%5;
				if(ftoi(down_offset)%5>0)
					down_offset=down_offset+5-ftoi(down_offset)%5;
				up_offset*=sign_up;
				down_offset*=sign_down;
			}
			m_pConnectPoint->m_bTransitConnect = false;	//�ж���˨����ʱӦ���Ե�����˨���õ�ѡ�����������׳�ͻ
		}
		else
		{
			if(m_pConnectPoint->m_bTransitConnect&&m_pConnectPoint->m_ciBaseRodConnState!=2)
			{	//�ų�m_pConnectPoint->m_ciBaseRodConnState==2��Ҫ��Ϊ�˱���������ͨ��CStackVariant�趨�ļٱ������ wjh-2019.9.4
				up_point.datum_pos_style=1;
				up_point.des_para.RODEND.hRod=m_pBaseRod->handle;
				up_point.des_para.RODEND.offset_wing=m_iBaseAngleWorkWing;
				if(m_pBaseRod->pStart==m_pBaseNode)
					up_point.des_para.RODEND.direction=0;
				else
					up_point.des_para.RODEND.direction=1;
				up_point.des_para.RODEND.wing_offset_style=0;
				up_point.des_para.RODEND.bIncOddEffect=TRUE;
				down_point=up_point;
			}
			else
			{
				up_point.datum_pos_style=2;
				up_point.des_para.RODNODE.hNode=m_pBaseNode->handle;
				up_point.des_para.RODNODE.hRod=m_pBaseRod->handle;
				up_point.des_para.RODNODE.offset_wing=m_iBaseAngleWorkWing;
				up_point.des_para.RODNODE.direction=0;
				up_point.des_para.RODNODE.wing_offset_style=0;
				up_point.des_para.RODNODE.bFaceOffset=m_ciRayEndFaceOffset!=0;
				//��������
				DesignBraceConnParams(&up_point.des_para.RODNODE.wing_offset_style,&up_point.des_para.RODNODE.wing_offset_dist);
				if(m_ciRayEndFaceOffset=='X')
					up_point.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
				else if(m_ciRayEndFaceOffset=='Y')
					up_point.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
				else if(m_ciRayEndFaceOffset=='Z')
					up_point.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
				down_point=up_point;
			}
			if (m_pConnectPoint->m_bTransitConnect)
			{
				if (m_pConnectPoint->m_ciBaseRodConnState == 2)	//2.�νӶ�ĩ������
					up_offset = 0;
				else
					up_offset = LsSpace.EndSpace;	//��׼��ż����˨Ĭ�Ͽ�ǰ
				down_offset = up_offset + LsSpace.SingleRowSpace;
			}
			else
			{
				if (bolt_num % 2 == 0)		//��׼��ż����˨�����޶�������˨
					down_offset = up_offset = LsSpace.SingleRowSpace*0.5;
				else if (bolt_num % 2 == 1)	//��׼��������˨�����޶�������˨
					down_offset = LsSpace.SingleRowSpace;
			}
		}
		for(i=0;i<bolt_num;i++)
		{
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->SetLayer(m_pBaseRod->layer());
			pBolt->cfgword=m_pBaseRod->cfgword;
			boltset.append(pBolt);
			pBolt->des_work_norm=des_bolt_norm;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetGrade(xBaseAngleBolts.grade);
			pBolt->AddL0Thick(m_pBaseRod->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->des_base_pos.hPart=m_pBaseRod->handle;
			pBolt->des_base_pos.norm_offset.AddThick(-m_pBaseRod->handle,TRUE);
			pBolt->des_base_pos.offset_wing=m_iBaseAngleWorkWing;
			if( (i%2==0&&nEndLjBoltNum==0)||nEndLjBoltNum==1&&up_endlj>=0&&(
				(i%2==0&&m_arrRayRod[up_endlj].endLjBoltNum<=m_arrRayRod[down_endlj].endLjBoltNum)||	//ż����˨����ʼ�˲�
				(i%2==1&&m_arrRayRod[up_endlj].endLjBoltNum>m_arrRayRod[down_endlj].endLjBoltNum)))
			{
				pBolt->des_base_pos.datumPoint=up_point;
				pBolt->des_base_pos.len_offset_dist=up_offset;
				if(m_pConnectPoint->m_bTransitConnect)
				{	//��׼��ͬʱҲ�����߲�
					if (m_pBaseRod->pStart == m_pBaseNode)
						pBolt->des_base_pos.direction = 0;	//ʼ->��Ϊ���߷���
					else
						pBolt->des_base_pos.direction = 1;	//��->ʼΪ���߷���
					//pBolt->des_base_pos.direction=(BYTE)up_point.des_para.RODEND.direction;	//������ʼƫ��
					up_offset+=2*LsSpace.SingleRowSpace;
				}
				else
				{
					pBolt->des_base_pos.direction=1;	//������ʼƫ��
					up_offset+=LsSpace.SingleRowSpace;
				}
			}
			else		//������˨�����ն˲�
			{
				pBolt->des_base_pos.datumPoint=down_point;
				pBolt->des_base_pos.len_offset_dist=down_offset;
				if(m_pConnectPoint->m_bTransitConnect)
				{
					if (m_pBaseRod->pStart == m_pBaseNode)
						pBolt->des_base_pos.direction = 0;	//ʼ->��Ϊ���߷���
					else
						pBolt->des_base_pos.direction = 1;	//��->ʼΪ���߷���
					//pBolt->des_base_pos.direction=(BYTE)down_point.des_para.RODEND.direction;	//������ʼƫ��
					down_offset+=2*LsSpace.SingleRowSpace;
				}
				else
				{
					pBolt->des_base_pos.direction=0;	//��ʼ����ƫ��
					down_offset+=LsSpace.SingleRowSpace;
				}
			}
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			if(m_pBaseRod->pStart==m_pBaseNode)
				m_pBaseRod->AppendStartLsRef(pBolt->GetLsRef());
			else if(m_pBaseRod->pEnd==m_pBaseNode)
				m_pBaseRod->AppendEndLsRef(pBolt->GetLsRef());
			else
				m_pBaseRod->AppendMidLsRef(pBolt->GetLsRef());
			m_pPlate->AppendLsRef(pBolt->GetLsRef());
		}
	}
	else
	{	//˫�Ŷ�����Ĭ���Ȳ����߲���˨
		double bolt_alfa_tan,bolt_alfa_cos_up,bolt_alfa_cos_down;
		if(m_nBaseStartG3>0)
			bolt_alfa_tan=(m_nBaseStartG3-m_jgzjBase.g1)/BoltSpace(xBaseAngleBolts.wiBoltD,'C');//(LsSpace.doubleRowSpace*0.5);
		else
			bolt_alfa_tan=(m_jgzjBase.g2-m_jgzjBase.g1)/BoltSpace(xBaseAngleBolts.wiBoltD,'C');//(LsSpace.doubleRowSpace*0.5);
		bolt_alfa_cos_up=1/sqrt(1+bolt_alfa_tan*bolt_alfa_tan);	//ʼ��������ֵ
		if(m_nBaseEndG3>0)
			bolt_alfa_tan=(m_nBaseEndG3-m_jgzjBase.g1)/BoltSpace(xBaseAngleBolts.wiBoltD,'C');//(LsSpace.doubleRowSpace*0.5);
		else
			bolt_alfa_tan=(m_jgzjBase.g2-m_jgzjBase.g1)/BoltSpace(xBaseAngleBolts.wiBoltD,'C');//(LsSpace.doubleRowSpace*0.5);
		bolt_alfa_cos_down=1/sqrt(1+bolt_alfa_tan*bolt_alfa_tan);//�ն�������ֵ

		BYTE cUpBoltLine=1,cDownBoltLine=2;	//���¶���˨�����ڻ�׼����������,1:g1��2:g2
		double up_wing_offset_g1,down_wing_offset_g1;
		double up_offset=0,down_offset=0;
		double up_g2_offset=0,down_g2_offset=0;	//���ڵڶ��Ŷ�������˨����׼�࣬Ϊ�����϶C�����׼�ಹ��ֵ
		//���ڵڶ��Ŷ�������˨����׼�࣬Ϊ�����϶C���������ƫ�Ʋ���ֵ
		double up_offset_complement=0,down_offset_complement=0;
		if(nEndLjBoltNum>0)	//����˨������	nEndLjBoltNum>1
		{
			up_point.datum_pos_style=3;
			up_point.des_para.AXIS_INTERS.hDatum1=m_pBaseRod->handle;
			double dd=m_arrRayRod[up_endlj].rayVec*ucs.axis_x;
			if(dd<=0)	//���߲ĳ�ʼ�˷�����
			{
				if(fabs(dd)>bolt_alfa_cos_up&&m_arrRayRod[up_endlj].endLjBoltNum==2)
					up_point.des_para.AXIS_INTERS.wing_offset_style1=2;	//������б�ȳ�����˨��׳̶ȣ�g2��������˨
				else
					up_point.des_para.AXIS_INTERS.wing_offset_style1=1;	//wing_intersline_style;//1;
			}
			else		//���߲ĳ��ն˷����䣬g2��������˨
				up_point.des_para.AXIS_INTERS.wing_offset_style1=2;
			up_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[up_endlj].Angle()->handle;
			up_point.des_para.AXIS_INTERS.wing_offset_style2=4;
			if(m_arrRayRod[up_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[up_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			down_point=up_point;
			dd=m_arrRayRod[down_endlj].rayVec*ucs.axis_x;
			if(dd<=0)	//���߲ĳ�ʼ�˷�����
				down_point.des_para.AXIS_INTERS.wing_offset_style1=2;
			else		//���߲ĳ��ն˷����䣬g2��������˨
			{
				if(fabs(dd)>bolt_alfa_cos_down&&m_arrRayRod[down_endlj].endLjBoltNum==2)	//������б�ȳ�����˨��׳̶ȣ�g2��������˨
					down_point.des_para.AXIS_INTERS.wing_offset_style1=2;
				else
					down_point.des_para.AXIS_INTERS.wing_offset_style1=1;	//wing_intersline_style;//1;
			}
			down_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[down_endlj].Angle()->handle;
			if(m_arrRayRod[down_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[down_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');

			if(up_point.des_para.AXIS_INTERS.wing_offset_style1==1)
				up_wing_offset_g1=0;
			else	//g2����
				up_wing_offset_g1=m_jgzjBase.g1-m_jgzjBase.g2;
			if(down_point.des_para.AXIS_INTERS.wing_offset_style1==1)
				down_wing_offset_g1=0;
			else	//g2����
				down_wing_offset_g1=m_jgzjBase.g1-m_jgzjBase.g2;
			cUpBoltLine=(BYTE)up_point.des_para.AXIS_INTERS.wing_offset_style1;
			cDownBoltLine=(BYTE)down_point.des_para.AXIS_INTERS.wing_offset_style1;
			if(cUpBoltLine==2&&m_nBaseStartG3>0)
			{
				up_point.des_para.AXIS_INTERS.wing_offset_style1=4;
				up_point.des_para.AXIS_INTERS.wing_offset_dist1=m_nBaseStartG3;
				up_g2_offset=m_nBaseStartG3-m_jgzjBase.g2;
			}
			else if(cUpBoltLine==1&&m_nBaseStartG3>0)
			{
				up_offset_complement=fabs((m_nBaseStartG3-m_jgzjBase.g2)*m_arrRayRod[up_endlj].cosa/m_arrRayRod[up_endlj].sina);
				up_offset+=up_offset_complement;
			}
			if(cDownBoltLine==2&&m_nBaseEndG3>0)
			{
				down_point.des_para.AXIS_INTERS.wing_offset_style1=4;
				down_point.des_para.AXIS_INTERS.wing_offset_dist1=m_nBaseEndG3;
				down_g2_offset=m_nBaseEndG3-m_jgzjBase.g2;
			}
			else if(cDownBoltLine==1&&m_nBaseEndG3>0)
			{
				down_offset_complement=fabs((m_nBaseEndG3-m_jgzjBase.g2)*m_arrRayRod[down_endlj].cosa/m_arrRayRod[down_endlj].sina);
				down_offset += down_offset_complement;
			}

		}
		else
		{
			if(m_pConnectPoint->m_bTransitConnect&&m_pConnectPoint->m_ciBaseRodConnState!=2)
			{	//�ų�m_pConnectPoint->m_ciBaseRodConnState==2��Ҫ��Ϊ�˱���������ͨ��CStackVariant�趨�ļٱ������ wjh-2019.9.4
				up_point.datum_pos_style=1;
				up_point.des_para.RODEND.hRod=m_pBaseRod->handle;
				up_point.des_para.RODEND.offset_wing=m_iBaseAngleWorkWing;
				if(m_pBaseRod->pStart==m_pBaseNode)
					up_point.des_para.RODEND.direction=0;
				else
					up_point.des_para.RODEND.direction=1;
				up_point.des_para.RODEND.wing_offset_style=0;
				up_point.des_para.RODEND.bIncOddEffect=TRUE;
				down_point=up_point;
				up_wing_offset_g1=down_wing_offset_g1=m_jgzjBase.g1-m_jgzjBase.g;

				cUpBoltLine=1;
				cDownBoltLine=2;
				up_offset=LsSpace.EndSpace;	//��׼��ż����˨Ĭ�Ͽ�ǰ
				down_offset=LsSpace.EndSpace+LsSpace.doubleRowSpace*0.5;
			}
			else
			{
				up_point.datum_pos_style=2;
				up_point.des_para.RODNODE.hNode=m_pBaseNode->handle;
				up_point.des_para.RODNODE.hRod=m_pBaseRod->handle;
				up_point.des_para.RODNODE.offset_wing=m_iBaseAngleWorkWing;
				up_point.des_para.RODNODE.direction=0;
				up_point.des_para.RODNODE.wing_offset_style=0;
				up_point.des_para.RODNODE.bFaceOffset=m_ciRayEndFaceOffset!=0;

				//������������
				DesignBraceConnParams(&up_point.des_para.RODNODE.wing_offset_style,&up_point.des_para.RODNODE.wing_offset_dist);
				if(m_ciRayEndFaceOffset=='X')
					up_point.des_para.RODNODE.vPlaneNormal.Set(1,0,0);
				else if(m_ciRayEndFaceOffset=='Y')
					up_point.des_para.RODNODE.vPlaneNormal.Set(0,1,0);
				else if(m_ciRayEndFaceOffset=='Z')
					up_point.des_para.RODNODE.vPlaneNormal.Set(0,0,1);
				down_point=up_point;
				up_wing_offset_g1=down_wing_offset_g1=m_jgzjBase.g1-m_jgzjBase.g;
				cUpBoltLine=1;
				cDownBoltLine=2;
				if (m_pConnectPoint->m_bTransitConnect)
				{	//�ػ�׼�����߷����沼��˫����˨ wjh-2019.11.9
					if (m_pConnectPoint->m_ciBaseRodConnState == 2)	//2.�νӶ�ĩ������
						up_offset = 0;
					else
						up_offset = LsSpace.EndSpace;	//��׼��ż����˨Ĭ�Ͽ�ǰ
					down_offset=up_offset+LsSpace.doubleRowSpace*0.5;
				}
				else
				{	//�Ի�׼��˫��ԳƲ���
					if (bolt_num%2==0)		//��׼��ż����˨�����޶�������˨
						down_offset=up_offset=LsSpace.doubleRowSpace*0.25;
					else if (bolt_num%2==1)	//��׼��������˨�����޶�������˨
					{
						up_offset=0;
						down_offset=LsSpace.doubleRowSpace*0.5;
					}
				}
			}
		}
		for(i=0;i<bolt_num;i++)
		{
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->SetLayer(m_pBaseRod->layer());
			pBolt->cfgword=m_pBaseRod->cfgword;
			boltset.append(pBolt);
			pBolt->des_work_norm=des_bolt_norm;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetGrade(xBaseAngleBolts.grade);
			pBolt->AddL0Thick(m_pBaseRod->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->des_base_pos.hPart=m_pBaseRod->handle;
			pBolt->des_base_pos.norm_offset.AddThick(-m_pBaseRod->handle,TRUE);
			pBolt->des_base_pos.offset_wing=m_iBaseAngleWorkWing;
			if((i%2==0&&nEndLjBoltNum==0)||
			   (nEndLjBoltNum==1&&up_endlj>=0&&(
				(i%2==0&&m_arrRayRod[up_endlj].endLjBoltNum<=m_arrRayRod[down_endlj].endLjBoltNum)||
				(i%2==1&&m_arrRayRod[up_endlj].endLjBoltNum>m_arrRayRod[down_endlj].endLjBoltNum)))||
			   (nEndLjBoltNum==2&&i%2==0))
			{	//ż����˨����ʼ�˲�(���޶�������˨��ʼ���������˨�������ղ��������)��������˨����ʼ�˲�(ʼ���������˨�������ղ����)
				pBolt->des_base_pos.datumPoint=up_point;
				if(m_pConnectPoint->m_bTransitConnect&&m_pBaseRod->pStart==m_pBaseNode)
					pBolt->des_base_pos.direction=0;	//���µ�ʼ�������߷���(��ʼ����)ƫ��
				else
					pBolt->des_base_pos.direction=1;	//������ʼƫ��
				if(cUpBoltLine==1)	//g1
					pBolt->des_base_pos.wing_offset_dist=up_wing_offset_g1-up_g2_offset;
				else
					pBolt->des_base_pos.wing_offset_dist=up_wing_offset_g1+m_jgzjBase.g2-m_jgzjBase.g1-up_g2_offset;
				pBolt->des_base_pos.len_offset_dist=up_offset;
				if(m_pConnectPoint->m_bTransitConnect)
					up_offset+=LsSpace.doubleRowSpace;
				else
				{
					cUpBoltLine=(cUpBoltLine+1)%2;
					if( up_point.datum_pos_style==3&&up_point.des_para.AXIS_INTERS.wing_offset_style1>=2&&
						i<=1&&fabs(m_arrRayRod[up_endlj].alfa)>90)//&&m_arrRayRod[up_endlj].alfa<270)
						up_offset-=LsSpace.doubleRowSpace*0.5;
					else
						up_offset+=LsSpace.doubleRowSpace*0.5;//+up_offset_complement;
				}
			}
			else
			{	//������˨�����ն˲�
				pBolt->des_base_pos.datumPoint=down_point;
				if(m_pConnectPoint->m_bTransitConnect&&m_pBaseRod->pEnd==m_pBaseNode)
					pBolt->des_base_pos.direction=1;	//���µ�ʼ�������߷���(������ʼ)ƫ��
				else
					pBolt->des_base_pos.direction=0;	//��ʼ����ƫ��
				if(cDownBoltLine==1)	//g1
					pBolt->des_base_pos.wing_offset_dist=down_wing_offset_g1-down_g2_offset;
				else
					pBolt->des_base_pos.wing_offset_dist=down_wing_offset_g1+m_jgzjBase.g2-m_jgzjBase.g1-down_g2_offset;
				pBolt->des_base_pos.len_offset_dist=down_offset;
				if(m_pConnectPoint->m_bTransitConnect)
					down_offset+=LsSpace.doubleRowSpace;
				else
				{
					cDownBoltLine=(cDownBoltLine+1)%2;
					if( down_point.datum_pos_style==3&&down_point.des_para.AXIS_INTERS.wing_offset_style1>=2&&
						i<=1&&fabs(m_arrRayRod[down_endlj].alfa)<90)//||m_arrRayRod[down_endlj].alfa>270))
						down_offset-=LsSpace.doubleRowSpace*0.5;
					else
						down_offset+=LsSpace.doubleRowSpace*0.5;//+down_offset_complement;
				}
			}
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			CLsRef* pFinalLsRef=NULL;
			if(m_pBaseRod->pStart==m_pBaseNode)
				pFinalLsRef=m_pBaseRod->AppendStartLsRef(pBolt->GetLsRef(),TRUE);
			else if(m_pBaseRod->pEnd==m_pBaseNode)
				pFinalLsRef=m_pBaseRod->AppendEndLsRef(pBolt->GetLsRef(),TRUE);
			else
				pFinalLsRef=m_pBaseRod->AppendMidLsRef(pBolt->GetLsRef(),TRUE);
			if(pFinalLsRef)
				m_pPlate->AppendLsRef(*pFinalLsRef,FALSE);
			else
				m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
		}
	}
}
void CCommonLjSmartDesign::LayoutAngleBolts(int index)	//index<0��ʾ��ƻ�׼�ģ�����������߸˼�������
{
	BOOL bEndLj = FALSE;
	CLDSLineAngle *pCurLineAngle=m_arrRayRod[index].Angle();
	CLDSLineAngle *pCrossLineAngle=NULL;
	CLDSPlate *pDatumPlate=m_pPlate;
	CConnectInfo *pLsInfo=m_arrRayRod[index].pConn;
	if(pCurLineAngle==NULL)
		return;	
	if(index==m_iFirstEndLjAngle&&m_iSecEndLjAngle>=0&&m_iSecEndLjAngle!=m_iFirstEndLjAngle)
		pCrossLineAngle=m_arrRayRod[m_iSecEndLjAngle].Angle();
	else if(index==m_iSecEndLjAngle&&m_iFirstEndLjAngle!=m_iSecEndLjAngle)
		pCrossLineAngle=m_arrRayRod[m_iFirstEndLjAngle].Angle();
	if(index==m_iFirstEndLjAngle||index==m_iSecEndLjAngle)
		bEndLj=TRUE;
	int i=0,j=0;
	int nEndBolts=0;
	for(CLDSPart *pPart=boltset.GetFirst();pPart;pPart=boltset.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_BOLT&&m_arrRayRod[index].Angle()->FindLsByHandle(pPart->handle))
			nEndBolts++;	//�۳���������˨
	}
	int bolt_num = m_arrRayRod[index].pConn->wnConnBoltN-nEndBolts;
	int bolt_rows=bolt_num>=5?2:1;
	if (nEndBolts>0)	//���ݵ�ǰʵ������������ʱ������������˨����3�žͿ�˫�Ų��� wjh-2019.6.9
		bolt_rows = bolt_num>=3 ? 2 : 1;
	if(m_arrRayRod[index].jgzj.g1==0||m_arrRayRod[index].jgzj.g2==0)
		bolt_rows=1;
	CLDSVector des_bolt_norm;
	des_bolt_norm.norm_style=1;
	des_bolt_norm.hVicePart=m_arrRayRod[index].Rod()->handle;
	des_bolt_norm.norm_wing=m_arrRayRod[index].work_wing;
	if(m_arrRayRod[index].wing_norm*ucs.axis_z>0)
		des_bolt_norm.direction=0;
	else
		des_bolt_norm.direction=1;
	des_bolt_norm.UpdateVector(&Ta);
	CLDSPoint datumPoint;
	CLDSPlate *pBoltPad= NULL;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,m_arrRayRod[index].pConn->d);
	double len_offset=LsSpace.EndSpace;
	long liDatumG=m_arrRayRod[index].jgzj.g;
	short siGStyle=bolt_rows==2?1:0;
	if(bolt_num!=m_arrRayRod[index].pConn->wnConnBoltN)
	{
		//bolt_rows=1;	//�ж�������˨����ʱ���϶��ǵ�����˨
		if (bolt_rows > 2)
		{	//Ŀǰ������������˨����ʱ�����ʣ��������˨�࣬��Ȼ��˫�Ų��� wjh-2019.6.9
			liDatumG = siGStyle == 0 ? m_arrRayRod[index].jgzj.g : m_arrRayRod[index].jgzj.g1;
			if ((m_arrRayRod[index].wing_vec*ucs.axis_x < 0 && m_arrRayRod[index].alfa <= 90) ||
				(m_arrRayRod[index].wing_vec*ucs.axis_x > 0 && m_arrRayRod[index].alfa > 90))
			{
				siGStyle = 2;
				liDatumG = m_arrRayRod[index].jgzj.g2;
			}
		}
		datumPoint.datum_pos_style=3;
		datumPoint.des_para.AXIS_INTERS.hDatum1=m_arrRayRod[index].Rod()->handle;
		datumPoint.des_para.AXIS_INTERS.hDatum2=m_pBaseRod->handle;
		datumPoint.des_para.AXIS_INTERS.wing_offset_style1=siGStyle;
		datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
		//��׼�϶
		int nFirstLsJgSpace =  BoltSpace(m_arrRayRod[index].pConn->d,'A');
		if (m_arrRayRod[index].bInsideAngle&&bolt_num>0)//�е��
			nFirstLsJgSpace = BoltSpace(m_arrRayRod[index].pConn->d,'a');
		if(base_wing_vec*m_arrRayRod[index].rayVec>0)	//�����ڻ�׼��֫���
		{
			datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=
			nFirstLsJgSpace+ftoi(m_pBaseRod->GetWidth());
		}
		else
			datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=-nFirstLsJgSpace;
		//len_offset=0;	//�ж�������˨����һ����˨�����߶�λ��������ƫ����
		if(m_pFirstEndLjBolt&&
			m_pFirstEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2==datumPoint.des_para.AXIS_INTERS.wing_offset_style1)
		{
			datumPoint.UpdatePos(m_arrRayRod[index].Rod()->BelongModel());
			f3dPoint len_vec=datumPoint.Position()-m_pFirstEndLjBolt->des_base_pos.datumPoint.Position();
			len_offset=len_vec*m_arrRayRod[index].rayVec;
			double sign=1;
			if(len_offset<0)
				sign=-1;
			len_offset=ceil(fabs(len_offset));
			if(ftoi(len_offset)%5>0)
				len_offset=len_offset+5-ftoi(len_offset)%5;
			len_offset*=sign;
			datumPoint=m_pFirstEndLjBolt->des_base_pos.datumPoint;
			strcpy(datumPoint.des_para.AXIS_INTERS.norm_offset,"");
		}
		if(m_arrRayRod[index].bInsideAngle&&bolt_num>0)	//�ڶ����ӽǸ���Ҫ���
		{//�����߽Ǹ���ڵ���϶С��8�������߽Ǹ�����1�Ų����׼�Ǹֹ��õ���˨ʱ��
		//����˨�����Զ����ɵ�Ȧ��
			if (!(bolt_num==1&&m_pBaseRod->GetThick()<8))
			{
				pBoltPad= (CLDSPlate*)console.AppendPart(CLS_PLATE);
				padSet.append(pBoltPad);
				pBoltPad->iSeg=m_pPlate->iSeg;
				pBoltPad->SetLayer(m_pPlate->layer());
				pBoltPad->cfgword=cfgword;
			}
		}
	}
	else
	{
		datumPoint.datum_pos_style=1;
		if(m_arrRayRod[index].Rod()->pStart==m_pBaseNode)
			datumPoint.des_para.RODEND.direction=0;	//ʼ�����ӣ���˨��ʼ���ն�λ����
		else
			datumPoint.des_para.RODEND.direction=1;	//�ն����ӣ���˨������ʼ��λ����
		datumPoint.des_para.RODEND.hRod=m_arrRayRod[index].Rod()->handle;
		datumPoint.des_para.RODEND.offset_wing=m_arrRayRod[index].work_wing;
		datumPoint.des_para.RODEND.len_offset_dist=0;
		datumPoint.des_para.RODEND.wing_offset_style=0;
		datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
	}
	for(i=0;i<bolt_num;i++)
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		boltset.append(pBolt);
		pBolt->iSeg=m_pBaseNode->iSeg;
		pBolt->SetLayer(m_arrRayRod[index].Rod()->layer());
		pBolt->cfgword=m_arrRayRod[index].Rod()->cfgword;
		pBolt->set_d(m_arrRayRod[index].pConn->d);
		pBolt->SetGrade(m_arrRayRod[index].pConn->grade);
		pBolt->AddL0Thick(m_arrRayRod[index].Rod()->handle,TRUE);
		if(m_pPlate)
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
		if(m_arrRayRod[index].bInsideAngle&&bolt_num!=m_arrRayRod[index].pConn->wnConnBoltN)
		{
			pBolt->AddL0Thick(m_pBaseRod->handle,TRUE);	//�ж�������˨��������Ҫ��������֫��
			double thick = m_pBaseRod->GetThick();
			if (bolt_num==1&&thick<8)
			{
				pBolt->DianQuan.N = 1;
				pBolt->DianQuan.thick = (short)thick;
				pBolt->DianQuan.offset = thick;
			}
		}
		if(m_arrRayRod[index].bInsideAngle)	//����
			pBolt->des_base_pos.norm_offset.AddThick(-m_arrRayRod[index].Rod()->handle,TRUE);
		else if(m_pPlate)
			pBolt->des_base_pos.norm_offset.AddThick(-m_pPlate->handle,TRUE);
		pBolt->des_work_norm=des_bolt_norm;
		pBolt->des_base_pos.datumPoint=datumPoint;
		pBolt->des_base_pos.hPart=m_arrRayRod[index].Rod()->handle;
		if(m_arrRayRod[index].Rod()->pStart==m_pBaseNode)	//ʼ������
			pBolt->des_base_pos.direction=0;
		else
			pBolt->des_base_pos.direction=1;
		pBolt->des_base_pos.offset_wing=m_arrRayRod[index].work_wing;
		pBolt->des_base_pos.len_offset_dist=len_offset;
		if(bolt_rows==1||m_arrRayRod[index].jgzj.g1==0||m_arrRayRod[index].jgzj.g2==0)	//������˨����
		{	//ָ�����Ż�֧��˫��
			pBolt->des_base_pos.wing_offset_dist=0;
			len_offset+=LsSpace.SingleRowSpace;
		}
		else				//˫����˨����
		{
			int complement=(siGStyle==2)?1:0;	//���ڴ�������˨������g2����ʱ�Ĳ����� wjh-2019.6.9
			if((complement+i)%2==0)	//ż����˨���������߲�
				pBolt->des_base_pos.wing_offset_dist=m_arrRayRod[index].jgzj.g1-liDatumG;
			else		//������˨������֫���
				pBolt->des_base_pos.wing_offset_dist=m_arrRayRod[index].jgzj.g2-liDatumG;
			len_offset+=LsSpace.doubleRowSpace*0.5;
		}
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		if(m_arrRayRod[index].Angle()->pStart==m_pBaseNode)
			m_arrRayRod[index].Angle()->AppendStartLsRef(pBolt->GetLsRef());
		else
			m_arrRayRod[index].Angle()->AppendEndLsRef(pBolt->GetLsRef());
		if(m_pPlate)
			m_pPlate->AppendLsRef(pBolt->GetLsRef());
		if(pBoltPad)
			pBoltPad->AppendLsRef(pBolt->GetLsRef());
	}
	if(pBoltPad)
	{
		pBoltPad->Thick=ftoi(m_pBaseRod->GetThick());
		pBoltPad->cMaterial='S';
		pBoltPad->iSeg=m_pBaseRod->iSeg;
		pBoltPad->m_fNormOffset=0;
		pBoltPad->jdb_style=4;	//��˨���
		//����巨�������Ϣ
		pBoltPad->ucs.axis_z = m_pConnectPoint->datumplane.vSolid;
		pBoltPad->designInfo.iFaceType=5;	//��˨���
		pBoltPad->designInfo.norm.norm_style=1;
		pBoltPad->designInfo.norm.norm_wing=m_arrRayRod[index].work_wing;
		pBoltPad->designInfo.norm.direction=0;	//����
		pBoltPad->designInfo.norm.hVicePart=m_arrRayRod[index].Rod()->handle;
		pBoltPad->cDatumAxis='X';	//��׼����Ϊ�Ǹ�ʱ�����ȼ���X����
		//�洢��׼�Ǹֺ��и��׼�Ǹ��������ʱ��
		pBoltPad->designInfo.m_hBasePart=m_arrRayRod[index].Rod()->handle;
		CDesignLjPartPara *pLjPara=pBoltPad->designInfo.partList.Add(m_pBaseRod->handle);
		pLjPara->hPart=m_pBaseRod->handle;
		pLjPara->m_nClassTypeId=CLS_LINEANGLE;
		pLjPara->m_bAdjustLjEndPos=FALSE;
		pBoltPad->DesignSetupUcs();
		pBoltPad->CalStdProfile();
		if(pBoltPad->GetLsCount()>1)	//�����Զ���˨�������и��
		{	//���и��׼�Ǹ���Ӧ���и���
			CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
			if(!pView->CutBoltPadByAngle(pBoltPad,BaseAngle()))
				return;
		}
		if(UI::blEnableIntermediateUpdateUI)
		{
			pBoltPad->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pBoltPad->GetSolidPartObject());
		}
	}
}
CLDSLineAngle* CCommonLjSmartDesign::BaseAngle()
{
	if( m_pBaseRod.IsHasPtr()&&(
		m_pBaseRod->GetClassTypeId()==CLS_LINEANGLE||m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE))
		return m_pBaseRod.LineAnglePointer();
	else
		return NULL;
}
CLDSGroupLineAngle* CCommonLjSmartDesign::BaseGroupAngle()
{
	if( m_pBaseRod.IsHasPtr()&&m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return m_pBaseRod.GroupAnglePointer();
	else
		return NULL;
}
CLDSLineTube* CCommonLjSmartDesign::BaseTube()
{
	if( m_pBaseRod.IsHasPtr()&&m_pBaseRod->GetClassTypeId()==CLS_LINETUBE)
		return m_pBaseRod.LineTubePointer();
	else
		return NULL;
}
void CCommonLjSmartDesign::SetPlateBasicInfo()
{
	int thick=5;	//����˵���߽Ǹ�֫��С��4ʱ�����ȡ5mm wjh-2013.5.7
	char cMat='S';
	int i=0;
	CLDSLinePart *pLinePart;
	CDesignLjPartPara *pLjPartPara=NULL;
	CHashSet<CLDSLinePart*> hashRayRods;
	m_pConnectPoint->BuildRodSet(&hashRayRods);
	for(pLinePart=hashRayRods.GetFirst();pLinePart;pLinePart=hashRayRods.GetNext())
	{
		if(SteelMatPriority(pLinePart->cMaterial)<SteelMatPriority(cMat))
			cMat=pLinePart->cMaterial;
	}
	if(m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		thick=ftoi(BaseGroupAngle()->jg_space);	//��ϽǸ������Ӱ���϶�����ϽǸּ�϶һ��
	else
	{
		for(pLinePart=hashRayRods.GetFirst();pLinePart;pLinePart=hashRayRods.GetNext(),i++)
		{
			
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pLinePart->GetThick()>thick)
				thick=(int)pLinePart->GetThick();
			else if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
				if(pGroupAngle->jg_space!=1)
				{	//��T��˫ƴ��ϽǸ�
					if(pGroupAngle->jg_space>thick)
						thick=(int)pGroupAngle->jg_space;
				}
				else if(i<m_arrRayRod.GetSize()&&m_arrRayRod[i].work_wing==0&&pGroupAngle->jg_space>thick)
					thick=(int)pGroupAngle->jg_space;	//T��˫ƴ��ϽǸ�
				else if(i<m_arrRayRod.GetSize()&&m_arrRayRod[i].work_wing==1&&pGroupAngle->GetThick()>thick)
					thick=(int)pGroupAngle->GetThick();	//T��˫ƴ��ϽǸ�
			}
		}
		if(thick==5)
			thick+=1;
		else if(thick>6&&thick%2>0)	//���ȡż��
			thick+=3;
		else
			thick+=2;	//���Ӧ�����߲������ֵ��һ���ȼ�
	}
	m_pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	m_pPlate->Thick=thick;
	m_pPlate->cMaterial=cMat;
	m_pPlate->iSeg=m_pBaseNode->iSeg;
	//m_pPlate->layer(0)=m_arrRayRod[0].Rod()->layer(0);	//��ʼ���ְ�ͼ�������һ�����߸˼�һ��
	//m_pPlate->layer(2)=m_arrRayRod[0].Rod()->layer(2);	//��ʼ���ְ�ͼ�������һ�����߸˼�һ��
	m_pPlate->SetLayer(m_pBaseRod->layer());
	m_pPlate->cfgword=m_pBaseRod->cfgword;
	m_pPlate->designInfo.m_hBaseNode=m_pBaseNode->handle;
	m_pPlate->designInfo.m_hBasePart=m_pBaseRod->handle;
	m_pPlate->designInfo.iFaceType=1;
	m_pPlate->designInfo.iProfileStyle0123=g_sysPara.intelligentConnect.m_iPlateProfileStyle;
	m_pPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
	m_pPlate->ucs=ucs;
	if(m_pBaseRod->GetClassTypeId()==CLS_LINEANGLE)
	{
		m_pPlate->designInfo.norm.norm_style=1;
		m_pPlate->designInfo.norm.hVicePart=m_pBaseRod->handle;
		m_pPlate->designInfo.norm.direction=0;
		m_pPlate->designInfo.norm.norm_wing=m_iBaseAngleWorkWing;
		m_pPlate->designInfo.norm.UpdateVector(&Ta);
		m_pPlate->designInfo.origin.datum_pos_style=2;
		m_pPlate->designInfo.origin.des_para.RODNODE.hRod=m_pBaseRod->handle;
		m_pPlate->designInfo.origin.des_para.RODNODE.hNode=m_pBaseNode->handle;
		m_pPlate->designInfo.origin.des_para.RODNODE.offset_wing=m_iBaseAngleWorkWing;
		double g;
		if(m_iBaseAngleWorkWing==0)
			g=BaseAngle()->GetNodeWingXG(m_pBaseNode);
		else
			g=BaseAngle()->GetNodeWingYG(m_pBaseNode);
		if(g==m_jgzjBase.g)
			m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=0;
		else if(g==m_jgzjBase.g1)
			m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=1;
		else if(g==m_jgzjBase.g2)
			m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=2;
		else if(g==m_jgzjBase.g3)
			m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=3;
		else
		{
			m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=4;
			m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_dist=g;
		}
		//��������˼����ӱ��л�׼�ĵ������Ϣ
		pLjPartPara=m_pPlate->designInfo.partList.Add(m_pBaseRod->handle);
		pLjPartPara->iFaceNo=1;
		pLjPartPara->angle.bTwoEdge=1;
		pLjPartPara->m_nClassTypeId=CLS_LINEANGLE;
		pLjPartPara->angle.cur_wing_x0_y1=(BYTE)m_iBaseAngleWorkWing;
		if (m_pBaseRod->pStart == m_pBaseNode)
			pLjPartPara->start0_end1 = 0;
		else if(m_pBaseRod->pEnd == m_pBaseNode)
			pLjPartPara->start0_end1 = 1;
		else
			pLjPartPara->start0_end1 = 2;
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,xBaseAngleBolts.wiBoltD);
		//pLjPartPara->ber_space=m_jgzjBase.g-m_pBaseRod->GetThick();
		//pLjPartPara->wing_space=m_pBaseRod->GetWidth()-m_jgzjBase.g;
		pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
		pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
		pLjPartPara->end_space=LsSpace.EndSpace;
	}
	else if(m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		m_pPlate->designInfo.norm.norm_style=1;
		m_pPlate->designInfo.norm.hVicePart=m_pBaseRod->handle;
		m_pPlate->designInfo.norm.direction=0;
		m_pPlate->designInfo.norm.norm_wing=m_iBaseAngleWorkWing;
		m_pPlate->designInfo.norm.UpdateVector(&Ta);
		m_pPlate->designInfo.origin.datum_pos_style=2;
		m_pPlate->designInfo.origin.des_para.RODNODE.hRod=m_pBaseRod->handle;
		m_pPlate->designInfo.origin.des_para.RODNODE.hNode=m_pBaseNode->handle;
		m_pPlate->designInfo.origin.des_para.RODNODE.offset_wing=m_iBaseAngleWorkWing;
		_snprintf(m_pPlate->designInfo.origin.des_para.RODNODE.norm_offset,50,
			"-%.0f",BaseGroupAngle()->jg_space*0.5);
		m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=4;
		m_pPlate->designInfo.origin.des_para.RODNODE.wing_offset_dist=0;
		//��������˼����ӱ��л�׼�ĵ������Ϣ
		for(i=0;i<4;i++)
		{
			if(BaseGroupAngle()->son_jg_h[i]<=0x20)
				continue;	//�޴��ӽǸ�
			pLjPartPara=m_pPlate->designInfo.partList.Add(BaseGroupAngle()->son_jg_h[i]);
			//pLjPartPara->hPart=BaseGroupAngle()->son_jg_h[i];
			pLjPartPara->iFaceNo=1;
			pLjPartPara->angle.bTwoEdge=1;
			pLjPartPara->m_nClassTypeId=CLS_LINEANGLE;
			if (m_pBaseRod->pStart == m_pBaseNode)
				pLjPartPara->start0_end1 = 0;
			else if (m_pBaseRod->pEnd == m_pBaseNode)
				pLjPartPara->start0_end1 = 1;
			else
				pLjPartPara->start0_end1 = 2;
			if (BaseGroupAngle()->group_style == 2)
			{	//ʮ�����
				if (i == 0)
					pLjPartPara->angle.cur_wing_x0_y1 = m_iBaseAngleWorkWing;
				else if (i == 1)
				{
					if (BaseGroupAngle()->group_style == 0)
						pLjPartPara->angle.cur_wing_x0_y1 = m_iBaseAngleWorkWing;
					else
						pLjPartPara->angle.cur_wing_x0_y1 = 1 - m_iBaseAngleWorkWing;
				}
				else if (i == 2)	//BaseGroupAngle()->group_style==2	ʮ�ֽ���
					pLjPartPara->angle.cur_wing_x0_y1 = m_iBaseAngleWorkWing;
				else //if(i==3)
					pLjPartPara->angle.cur_wing_x0_y1 = 1 - m_iBaseAngleWorkWing;
			}
			else
			{
				if (BaseGroupAngle()->son_jg_h[i] == BaseGroupAngle()->m_hDatumSonAngle)
					pLjPartPara->angle.cur_wing_x0_y1 = m_iBaseAngleWorkWing;
				else
					pLjPartPara->angle.cur_wing_x0_y1 = 1 - m_iBaseAngleWorkWing;
			}
			LSSPACE_STRU LsSpace;
			GetLsSpace(LsSpace,xBaseAngleBolts.wiBoltD);
			pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
			pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
			//pLjPartPara->ber_space=m_jgzjBase.g-m_pBaseRod->GetThick();
			pLjPartPara->end_space=LsSpace.EndSpace;
			//pLjPartPara->wing_space=m_pBaseRod->GetWidth()-m_jgzjBase.g;
		}
	}
	else if(m_pBaseRod->GetClassTypeId()==CLS_LINETUBE)
	{
		m_pPlate->designInfo.norm.norm_style=2;	//�˼������淨��
		m_pPlate->designInfo.norm.hVicePart=m_pBaseRod->handle;
		m_pPlate->designInfo.norm.hCrossPart=m_pConnectPoint->linkPartSet.GetFirst()->handle;
		m_pPlate->designInfo.norm.nearVector = m_pConnectPoint->datumplane.vSolid;
		m_pPlate->designInfo.norm.UpdateVector(&Ta);
		m_pPlate->designInfo.origin.datum_pos_style=2;
		m_pPlate->designInfo.origin.des_para.RODNODE.hNode=m_pConnectPoint->m_hBaseNode;
		m_pPlate->designInfo.origin.des_para.RODNODE.hRod=m_pConnectPoint->m_hBasePart;
		pLjPartPara=m_pPlate->designInfo.partList.Add(m_pBaseRod->handle);
		pLjPartPara->iFaceNo=1;
		if (m_pBaseRod->pStart == m_pBaseNode)
			pLjPartPara->start0_end1 = 0;
		else if (m_pBaseRod->pEnd == m_pBaseNode)
			pLjPartPara->start0_end1 = 1;
		else
			pLjPartPara->start0_end1 = 2;
		pLjPartPara->m_nClassTypeId=m_pBaseRod->GetClassTypeId();
	}
}
bool RAYROD_INFO::get_blViewAsSingularAngle() const
{
	if (rod.pRod != NULL && rod.pRod->IsAngle())
	{
		if (rod.pRod->GetClassTypeId() == CLS_LINEANGLE || _blViewAsSingularAngle)
			return true;
	}
	return false;
}
bool RAYROD_INFO::set_blViewAsSingularAngle(bool blValue)
{
	if (rod.pRod != NULL && rod.pRod->IsAngle())
		_blViewAsSingularAngle = blValue;
	else
		_blViewAsSingularAngle = false;
	return _blViewAsSingularAngle;
}
int RAYROD_INFO::CompareRayRod(const RAYROD_INFO& rodInfo1,const RAYROD_INFO& rodInfo2)
{
	if(rodInfo1.alfa>rodInfo2.alfa)
		return 1;
	else if(rodInfo1.alfa==rodInfo2.alfa)
		return 0;
	else
		return -1;
}
//�������ǰ��׼������
BOOL CCommonLjSmartDesign::Prepare(int endljNum,BOOL bHasPlate,BOOL canMultiRow,BOOL priorMiddleEndLj/*=TRUE*/)
{
	m_pBaseNode=Ta.Node.FromHandle(m_pConnectPoint->m_hBaseNode);
	CSuperSmartPtr<CLDSPart>pDatumPart=Ta.Parts.FromHandle(m_pConnectPoint->m_hBasePart);
	if( m_pBaseNode==NULL||pDatumPart.IsNULL()||//pDatumPart->GetClassTypeId()==CLS_GROUPLINEANGLE||
		pDatumPart.LinePartPointer()->pStart==NULL||pDatumPart.LinePartPointer()->pEnd==NULL)
		return FALSE;
	m_pBaseRod=pDatumPart.LinePartPointer();
	ucs.axis_x=m_pBaseRod->End()-m_pBaseRod->Start();
	normalize(ucs.axis_x);
	m_arrRayRod.SetSize(0,m_pConnectPoint->linkPartSet.GetNodeNum());
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	//ȷ��������Ƶ���ĺ�
	cfgword=m_pBaseNode->UnifiedCfgword()&m_pBaseRod->UnifiedCfgword();
	if(cfgword.IsNull())
	{
		logerr.Log("��׼�ڵ�(0X%X)���׼��(0X%X)�޹�����ĺ�,���ܽڵ㸸�˼����ڵ�ǰ������!",m_pBaseNode->handle,m_pBaseRod->handle);
		return FALSE;
	}
	int i=0;
	m_ciRayEndFaceOffset=FALSE;
	CHashSet<CLDSLinePart*>hashRayRods;
	m_pConnectPoint->BuildRodSet(&hashRayRods);
	for(pLinePart=hashRayRods.GetFirst();pLinePart.IsHasPtr();pLinePart=hashRayRods.GetNext(),i++)
	{
		if(!pLinePart->IsAngle()&&pLinePart->GetClassTypeId()!=CLS_LINETUBE)
		{
			char clsname[51];
			pLinePart->GetClassType(clsname);
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"ϵͳ��ʱ���������<%s>�������߸˼�(0x%X)���������Ӱ����",clsname,pLinePart->handle);
			return FALSE;
		}
		m_arrRayRod.append();
		if (m_pConnectPoint->bendPlaneRodSet.FromHandle(pLinePart->handle) != NULL)
			m_arrRayRod[i].ciPlaneNo = 2;
		else
			m_arrRayRod[i].ciPlaneNo = 1;
		m_arrRayRod[i].rod.pRod=pLinePart;
		if(pLinePart->pStart==m_pBaseNode)
		{
			GEPOINT vPlaneNormal=m_arrRayRod[i].Angle()->desStartPos.face_offset_norm;
			if (!m_arrRayRod[i].Angle()->desStartPos.IsFaceOffset())
				m_ciRayEndFaceOffset = 0;
			else if(fabs(vPlaneNormal.x)>fabs(vPlaneNormal.y)&&fabs(vPlaneNormal.x)>fabs(vPlaneNormal.z))
				m_ciRayEndFaceOffset='X';
			else if(fabs(vPlaneNormal.y)>fabs(vPlaneNormal.x)&&fabs(vPlaneNormal.y)>fabs(vPlaneNormal.z))
				m_ciRayEndFaceOffset='Y';
			else if(fabs(vPlaneNormal.z)>fabs(vPlaneNormal.y)&&fabs(vPlaneNormal.z)>fabs(vPlaneNormal.x))
				m_ciRayEndFaceOffset='Z';
			m_arrRayRod[i].rayVec=pLinePart->End()-pLinePart->Start();
		}
		else
		{
			GEPOINT vPlaneNormal=m_arrRayRod[i].Angle()->desEndPos.face_offset_norm;
			if (!m_arrRayRod[i].Angle()->desEndPos.IsFaceOffset())
				m_ciRayEndFaceOffset = 0;
			else if(fabs(vPlaneNormal.x)>fabs(vPlaneNormal.y)&&fabs(vPlaneNormal.x)>fabs(vPlaneNormal.z))
				m_ciRayEndFaceOffset='X';
			else if(fabs(vPlaneNormal.y)>fabs(vPlaneNormal.x)&&fabs(vPlaneNormal.y)>fabs(vPlaneNormal.z))
				m_ciRayEndFaceOffset='Y';
			else if(fabs(vPlaneNormal.z)>fabs(vPlaneNormal.y)&&fabs(vPlaneNormal.z)>fabs(vPlaneNormal.x))
				m_ciRayEndFaceOffset='Z';
			m_arrRayRod[i].rayVec=pLinePart->Start()-pLinePart->End();
		}
		normalize(m_arrRayRod[i].rayVec);
		if(m_arrRayRod[i].rayVec*ucs.axis_x>EPS_COS2)
			return FALSE;	//�ӽ����Ѿ�����
		if(ucs.axis_z.IsZero() && i<m_pConnectPoint->linkPartSet.GetNodeNum())
		{
			ucs.axis_z=m_arrRayRod[i].rayVec^ucs.axis_x;
			normalize(ucs.axis_z);
			if(m_pBaseRod->IsAngle())
			{
				double ddx=ucs.axis_z*BaseAngle()->get_norm_x_wing();
				double ddy=ucs.axis_z*BaseAngle()->get_norm_y_wing();
				if(fabs(ddx)>fabs(ddy))
				{
					m_iBaseAngleWorkWing=0;
					if(ddx<0)
						ucs.axis_z*=-1.0;
					base_wing_vec=BaseAngle()->GetWingVecX();
					base_wing_norm=BaseAngle()->get_norm_x_wing();
					ucs.origin=BaseAngle()->GetDatumPosWingX(m_pBaseNode);
				}
				else
				{
					m_iBaseAngleWorkWing=1;
					if(ddy<0)
						ucs.axis_z*=-1.0;
					base_wing_vec=BaseAngle()->GetWingVecY();
					base_wing_norm=BaseAngle()->get_norm_y_wing();
					ucs.origin=BaseAngle()->GetDatumPosWingY(m_pBaseNode);
				}
			}
			else //if(m_pBaseTube)
			{
				if(ucs.axis_z*m_pConnectPoint->datumplane.vSolid<0)
					ucs.axis_z*=-1.0;
				ucs.origin=BaseTube()->GetDatumPos(m_pBaseNode);
			}
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
		}
		cfgword&=pLinePart->UnifiedCfgword();
		if(cfgword.IsNull())
		{
			logerr.Log("���߲�(0X%X)���׼�ڵ�(0X%X)����׼��(0X%X)�޹�����ĺ�,���ܽڵ㸸�˼����ڵ�ǰ������!",pLinePart->handle,m_pBaseNode->handle,m_pBaseRod->handle);
			return FALSE;
		}
	}
	for(i=0;i<m_arrRayRod.GetSize();i++)
	{
		f3dPoint rayVec=m_arrRayRod[i].rayVec;
		vector_trans(rayVec,ucs,FALSE);
		double alfa=Cal2dLineAng(0,0,rayVec.x,rayVec.y);
		m_arrRayRod[i].sina=sin(alfa);
		m_arrRayRod[i].cosa=cos(alfa);
		m_arrRayRod[i].alfa=alfa*DEGTORAD_COEF;	//ת��Ϊ�Ƕ���
	}
	int nDatumPlaneRayRodNum = m_arrRayRod.GetSize();
	CQuickSort<RAYROD_INFO>::QuickSort(m_arrRayRod.m_pData,nDatumPlaneRayRodNum,RAYROD_INFO::CompareRayRod);
	for(i=0;i<m_arrRayRod.GetSize();i++)
	{
		if(m_arrRayRod[i].alfa>180)
			m_arrRayRod[i].alfa-=360;
	}
	if(m_pBaseRod->IsAngle())
	{
		if(m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
			ucs.origin=BaseGroupAngle()->GetDatumPosBer(m_pBaseNode);
		if(!BaseAngle()->m_bEnableTeG)
			getjgzj(m_jgzjBase,m_pBaseRod->GetWidth());
		else if(m_iBaseAngleWorkWing==0)	//��׼��X֫����
			m_jgzjBase=BaseAngle()->xWingXZhunJu;
		else
			m_jgzjBase=BaseAngle()->xWingYZhunJu;
	}
	//for(i=0;i<m_pConnectPoint->linkPartSet.GetNodeNum();i++)
	for(i=0;i<m_arrRayRod.GetSize();i++)
	{
		if(m_arrRayRod[i].Rod()->pStart==m_pBaseNode)
		{
			m_arrRayRod[i].pConn=&m_arrRayRod[i].Rod()->connectStart;
			if(m_arrRayRod[i].Rod()->GetClassTypeId()==CLS_LINETUBE)
				m_arrRayRod[i].pos.pTubePos=&m_arrRayRod[i].Tube()->desStartPos;
			else
				m_arrRayRod[i].pos.pAnglePos=&m_arrRayRod[i].Angle()->desStartPos;
		}
		else //if(m_arrRayRod[i].Rod()->pEnd==pNode)
		{
			m_arrRayRod[i].pConn=&m_arrRayRod[i].Rod()->connectEnd;
			if(m_arrRayRod[i].Rod()->GetClassTypeId()==CLS_LINETUBE)
				m_arrRayRod[i].pos.pTubePos=&m_arrRayRod[i].Tube()->desEndPos;
			else
				m_arrRayRod[i].pos.pAnglePos=&m_arrRayRod[i].Angle()->desEndPos;
		}
		GetLsSpace(m_arrRayRod[i].LsSpace,m_arrRayRod[i].pConn->d);
	}
	f3dPoint merge_vec;	//�������߷���ĺ������������ж��м��
	LSSPACE_STRU LsSpace;
	CONNBOLTS basebolts(0, 0, (int)m_pBaseRod->Thick);
	BYTE cnCanEndLjAngles = 0;
	for(i=0;i<m_arrRayRod.GetSize();i++)
	{
		//�������߲ĵ�ǰ����֫����������Ϣ
		if(m_arrRayRod[i].Rod()->IsAngle())
			m_arrRayRod[i].bInsideAngle=IsInsideJg(m_arrRayRod[i].Angle(),ucs.axis_z,&m_arrRayRod[i].work_wing);
		if(!m_arrRayRod[i].Angle()->m_bEnableTeG)
			getjgzj(m_arrRayRod[i].jgzj,m_arrRayRod[i].pAngle->GetWidth());
		else
		{
			if(m_arrRayRod[i].work_wing==0)
				m_arrRayRod[i].jgzj=m_arrRayRod[i].pAngle->xWingXZhunJu;
			else
				m_arrRayRod[i].jgzj=m_arrRayRod[i].pAngle->xWingYZhunJu;
		}
		if(m_arrRayRod[i].work_wing==0)
		{
			m_arrRayRod[i].wing_norm=m_arrRayRod[i].pAngle->vxWingNorm;
			m_arrRayRod[i].wing_vec =m_arrRayRod[i].pAngle->GetWingVecX();
		}
		else
		{
			m_arrRayRod[i].wing_norm=m_arrRayRod[i].pAngle->vyWingNorm;
			m_arrRayRod[i].wing_vec =m_arrRayRod[i].pAngle->GetWingVecY();
		}
		if(m_arrRayRod[i].ciPlaneNo>1)//i>=m_pConnectPoint->linkPartSet.GetNodeNum())
			continue;	//ֻ�����׼�������������׼����˨ֱ������ wjh-2019.11.6
		merge_vec+=m_arrRayRod[i].rayVec;
		if(!m_arrRayRod[i].pRod->IsAngle())
			continue;
		if (m_pConnectPoint->m_ciBaseRodConnState==2||m_pConnectPoint->m_ciBaseRodConnState==3)
			m_arrRayRod[i].endLjBoltNum=0;	//�ǹ����νӶνضϵ����Ӵ������޷�����˨�����Ҳ��������ڶ���˨ wjh-2019.9.4
		else if (fabs(m_arrRayRod[i].rayVec.z)<EPS&&m_arrRayRod[i].pRod->chLayer1st!='T'&&m_arrRayRod[i].pRod->chLayer2nd=='H')
			m_arrRayRod[i].endLjBoltNum=0;	//�������ǲ�������ڶ�������˨�ģ�����ᵼ�º����ߴ緢���仯 wjh-2019.9.4
		else if (m_arrRayRod[i].pRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
			m_arrRayRod[i].endLjBoltNum = 0;	//��ϽǸֲ�֧�����׼����˨ֱ��
		else if(m_arrRayRod[i].bInsideAngle&&m_arrRayRod[i].rayVec*base_wing_vec<0)
			m_arrRayRod[i].endLjBoltNum=0;	//��׼�����߲������޷�������
		else if(!canMultiRow||m_jgzjBase.g1<=0||m_jgzjBase.g2<=0||m_arrRayRod[i].pConn->wnConnBoltN==1)
			m_arrRayRod[i].endLjBoltNum=1;
		else
		{	//����֧��˫���ģ���������˨��Ϊ2���
			double cosa=fabs(ucs.axis_x *m_arrRayRod[i].rayVec);
			double sina=sqrt(1-cosa*cosa);
			GetLsSpace(LsSpace,m_arrRayRod[i].pConn->d);
			double base_g=m_jgzjBase.g;
			if(m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				base_g=m_jgzjBase.g1;
			double space=m_jgzjBase.g2-base_g;
			double space_B=BoltSpace(m_arrRayRod[i].pConn->d,'B');
			double space_S=BoltSpace(m_arrRayRod[i].pConn->d,'S');
			double spaceMax=m_pBaseRod->GetWidth()-space_B-base_g;//m_jgzjBase.g1;
			double distL=space/sina;
			double distMaxL=spaceMax/sina;
			double distS=distL*fabs(cosa);
			double distMaxS=distMaxL*fabs(cosa);
			//һ�㲻�ã��ڴ���ʱ��ʾָ��������׼�ࣨ֫����˨�߾ࣩ
			bool directStart=true;
			if(m_arrRayRod[i].rayVec*ucs.axis_x>0)
				directStart=false;
			if(directStart)
				m_nBaseStartG3=0;
			else
				m_nBaseEndG3=0;
			if((distL<LsSpace.SingleRowSpace&&distMaxL<LsSpace.SingleRowSpace) ||
			   (distS<space_S&&distMaxS<space_S))
			{	//����������˨��಻�㲻���Խ����ް��˫��˨���������
				if(m_pConnectPoint->m_nMaxRayLsNum>2&&m_jgzjBase.g1>0&&m_jgzjBase.g2>0)
					m_arrRayRod[i].endLjBoltNum=0;	//����һ����˫���ģ���ʱ���Ƕ�����һ����˫��˨�����ӣ������ܶ��������
				else
					m_arrRayRod[i].endLjBoltNum=1;	//ֻ�ܽ��е���˨���������
			}
			else if(distL>=LsSpace.SingleRowSpace&&distS>=space_S)
				m_arrRayRod[i].endLjBoltNum=2;		//�ɽ���˫��˨���������
			else
			{	//���������ĵ���֤��˨�߾�
				if(directStart)
					m_nBaseStartG3=ftoi(m_pBaseRod->GetWidth()-space_B);
				else
					m_nBaseEndG3=ftoi(m_pBaseRod->GetWidth()-space_B);
				m_arrRayRod[i].endLjBoltNum=2;		//�ɽ���˫��˨���������
			}
		}
		if(m_arrRayRod[i].endLjBoltNum > 0)
			cnCanEndLjAngles +=1;
		CONNBOLTS raybolts(m_arrRayRod[i].pConn->d, (int)ceil(m_arrRayRod[i].pConn->wnConnBoltN*1.5), (WORD)m_pBaseRod->Thick, m_arrRayRod[i].pConn->grade);
		if (basebolts.wiBoltD == 0)
			basebolts = raybolts;
		else if(raybolts.dfCtrlAf > basebolts.dfCtrlAf)
			basebolts = raybolts;
		if(m_arrRayRod[i].pConn->wnConnBoltN>2&&!bHasPlate)
			return FALSE;	//��������˨������2ʱ���޷�ֻͨ��������ʵ�֣������ǰ�����
		else if(endljNum==0&&!bHasPlate)
			return FALSE;	//����������Ϊ��ʱ�޷������ް�����
		else if(m_arrRayRod[i].pConn->wnConnBoltN>m_arrRayRod[i].endLjBoltNum&&!bHasPlate)
			return FALSE;	//�ް�����ʱ���������߲ı��붼�Ƕ�����
	}
	if (cnCanEndLjAngles<endljNum)
		return false;
	xBaseAngleBolts = basebolts;
	if (this->m_pBaseRod->pStart == this->m_pBaseNode)
	{
		CONNBOLTS bolts(m_pBaseRod->connectStart.d, m_pBaseRod->connectStart.wnConnBoltN, (int)m_pBaseRod->Thick, m_pBaseRod->connectStart.grade);
		if (bolts.dfCtrlAf < basebolts.dfCtrlAf)
			bolts = basebolts;
		xBaseAngleBolts = bolts;
	}
	else if(this->m_pBaseRod->pEnd == this->m_pBaseNode)
	{
		CONNBOLTS bolts(m_pBaseRod->connectEnd.d, m_pBaseRod->connectEnd.wnConnBoltN, (int)m_pBaseRod->Thick, m_pBaseRod->connectEnd.grade);
		if (bolts.dfCtrlAf < basebolts.dfCtrlAf)
			bolts = basebolts;
		xBaseAngleBolts = bolts;
	}
	if(endljNum>2)
		endljNum=2;	//��������Ǹֶ�����
	if(endljNum==2)	//˫���߲Ĺ�����˨�����ӣ���ʱ������һ�ԽǸ�����������ͬ
	{
		for(i=1;i<m_arrRayRod.Count;i++)
		{
			if(m_arrRayRod[i-1].bInsideAngle!=m_arrRayRod[i].bInsideAngle)
				break;
		}
		if(i==m_arrRayRod.Count)	//����������ȫ����ͬ
			return FALSE;
	}
	if(endljNum>0)
	{	//��ѡ���ʵĶ����ӽǸ֣���ѡ˳���м��>���߲ģ�����>��������˨��>��˨�٣�����ʱֻ��ѡ����ͬ�Ĳ�
		if(normalize(merge_vec)==0)
			return FALSE;	//���߲ĶԳ�ʱ��֧�ֶ�����
		if(endljNum>3)
			return FALSE;	//���߲Ĺ���ʱ��֧�ֶ�����
		m_iFirstEndLjAngle=-1;
		double ddmax=-1,ddmin=1;
		//��ѡ��һ�����߲�
		for(i=0;i<m_arrRayRod.Count;i++)
		{
			if (m_arrRayRod[i].ciPlaneNo>1||m_arrRayRod[i].endLjBoltNum==0)
				continue;
			double dd=fabs(merge_vec*m_arrRayRod[i].rayVec);
			if(m_iFirstEndLjAngle<0)
			{
				m_iFirstEndLjAngle=i;
				ddmin=ddmax=dd;
			}
			if(priorMiddleEndLj)
			{	//�����м�Ķ�����
			if(dd>ddmax+EPS)
			{
				m_iFirstEndLjAngle=i;
				ddmax=dd;
			}
			else if(fabs(dd-ddmax)<EPS)
			{	//�������������߲�
				if(m_arrRayRod[i].bInsideAngle<m_arrRayRod[m_iFirstEndLjAngle].bInsideAngle)
					m_iFirstEndLjAngle=i;	//�������߲�ʱ��������
				else if(m_arrRayRod[i].bInsideAngle==m_arrRayRod[m_iFirstEndLjAngle].bInsideAngle)
				{
					if(m_arrRayRod[i].pConn->wnConnBoltN>m_arrRayRod[m_iFirstEndLjAngle].pConn->wnConnBoltN)
						m_iFirstEndLjAngle=i;
				}
			}
			}
			else
			{	//�������߲Ķ�����
				if(dd<ddmin-EPS)
				{
					m_iFirstEndLjAngle=i;
					ddmin=dd;
				}
				else if(fabs(dd-ddmin)<EPS)
				{	//�������������߲�
					if(m_arrRayRod[i].bInsideAngle<m_arrRayRod[m_iFirstEndLjAngle].bInsideAngle)
						m_iFirstEndLjAngle=i;	//�������߲�ʱ��������
					else if(m_arrRayRod[i].bInsideAngle==m_arrRayRod[m_iFirstEndLjAngle].bInsideAngle)
					{
						if(m_arrRayRod[i].pConn->wnConnBoltN>m_arrRayRod[m_iFirstEndLjAngle].pConn->wnConnBoltN)
							m_iFirstEndLjAngle=i;
					}
				}
			}
		}
		//��ѡ�ڶ������߲�
		if(endljNum==2)
		{
			ddmax=-1;
			ddmin=1;
			m_iSecEndLjAngle=-1;
			for(i=0;i<m_arrRayRod.Count;i++)
			{
				if (m_arrRayRod[i].ciPlaneNo>1||m_arrRayRod[i].endLjBoltNum==0)
					continue;
				if(i==m_iFirstEndLjAngle)
					continue;	//��һ�������ӽǸ�
				else if(m_arrRayRod[i].bInsideAngle==m_arrRayRod[m_iFirstEndLjAngle].bInsideAngle)
					continue;	//�ڶ��������ӽǸֱ������һ������ͬ
				double dd=fabs(m_arrRayRod[m_iFirstEndLjAngle].rayVec*m_arrRayRod[i].rayVec);
				if(m_iSecEndLjAngle<0)
				{
					m_iSecEndLjAngle = i;
					ddmin=ddmax=dd;
				}
				if(priorMiddleEndLj)
				{	//�����м�Ķ�����
					if(dd>ddmax+EPS)
					{	//���һ���Ƕ�С�ĸ��ʺ϶�����
						m_iSecEndLjAngle=i;
						ddmax=dd;
					}
					else if(fabs(dd-ddmax)<EPS)
					{	//�������������߲�
						if(m_arrRayRod[i].pConn->wnConnBoltN>m_arrRayRod[m_iSecEndLjAngle].pConn->wnConnBoltN)
							m_iSecEndLjAngle=i;
					}
				}
				else
				{
					if(dd<ddmin-EPS)
					{	//���һ���Ƕ�С�ĸ��ʺ϶�����
						m_iSecEndLjAngle=i;
						ddmin=dd;
					}
					else if(fabs(dd-ddmin)<EPS)
					{	//�������������߲�
						if(m_arrRayRod[i].pConn->wnConnBoltN>m_arrRayRod[m_iSecEndLjAngle].pConn->wnConnBoltN)
							m_iSecEndLjAngle=i;
					}
				}
			}
		}
	}
	if(bHasPlate)
		SetPlateBasicInfo();
	return TRUE;
}
BOOL CCommonLjSmartDesign::DesignBendPlane(CConnectPointInfo *pConnectPoint)
{
	m_pPlate->designInfo.huoqufacenorm[0].norm_style=2;
	if((m_pPlate->designInfo.huoqufacenorm[0].hVicePart=pConnectPoint->m_hBendlineAngle)<0x20)
		return FALSE;
	CLDSLineAngle* pBendlineAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pConnectPoint->m_hBendlineAngle,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pBendlineAngle==NULL&&!pConnectPoint->m_bTransitConnect)
		return FALSE;
	double length,raylength=0;
	GEPOINT rayvec;
	CLDSLinePart* pRod,*pMaxLongRod=NULL;
	CLDSLinePart* pCoupleMainRod = NULL;	//���´����������������ĽǸ�
	double huoquoffset=0;	//����֫��������(�㣭>֫��Ϊ��)
	char bendwing_x0_y1;
	for(pRod=pConnectPoint->bendPlaneRodSet.GetFirst();pRod;pRod=pConnectPoint->bendPlaneRodSet.GetNext())
	{
		length=pRod->GetLength();
		if(pRod->pStart->handle==pConnectPoint->m_hBaseNode)
			rayvec=pRod->End()-pRod->Start();
		else if(pRod->pEnd->handle==pConnectPoint->m_hBaseNode)
			rayvec=pRod->Start()-pRod->End();
		if (pRod->chLayer2nd == 'Z')
			pCoupleMainRod = pRod;
		if(length>raylength)
		{
			raylength=length;
			pMaxLongRod=pRod;
		}
		if(huoquoffset==0&&!pConnectPoint->m_bTransitConnect)
		{
			bendwing_x0_y1=1;
			GEPOINT vertwing_norm=pBendlineAngle->get_norm_x_wing();
			if( fabs(pBendlineAngle->get_norm_x_wing()*m_pPlate->ucs.axis_z)>
				fabs(pBendlineAngle->get_norm_y_wing()*m_pPlate->ucs.axis_z))
			{
				bendwing_x0_y1=0;
				vertwing_norm=pBendlineAngle->get_norm_y_wing();
			}
			if(rayvec*vertwing_norm>0)	//�����������߲�
				huoquoffset=-g_sysPara.HuoQuDist;
			else
				huoquoffset=g_sysPara.HuoQuDist+pBendlineAngle->GetWidth();
		}
	}
	if(pBendlineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&!pConnectPoint->m_bTransitConnect)
	{
		if(huoquoffset>0)
			huoquoffset=((CLDSGroupLineAngle*)pBendlineAngle)->jg_space*0.5+pBendlineAngle->GetWidth()+g_sysPara.HuoQuDist;
		else
			huoquoffset=-((CLDSGroupLineAngle*)pBendlineAngle)->jg_space*0.5-pBendlineAngle->GetWidth()-g_sysPara.HuoQuDist;
	}
	if(pMaxLongRod==NULL)
		return FALSE;
	m_pPlate->designInfo.huoqufacenorm[0].hCrossPart=pMaxLongRod->handle;
	m_pPlate->designInfo.huoqufacenorm[0].nearVector=m_pPlate->ucs.axis_z;
	if(m_pPlate->designInfo.huoqufacenorm[0].UpdateVector(&Ta))
		m_pPlate->huoqufacenorm[0]=m_pPlate->designInfo.huoqufacenorm[0].vector;
	else
		return FALSE;
	//�趨������λ��
	if(pConnectPoint->m_bTransitConnect)
	{
		m_pPlate->designInfo.huoquline_start[0].datum_pos_style=m_pPlate->designInfo.huoquline_end[0].datum_pos_style=2;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.hRod=pConnectPoint->m_hBasePart;
		f3dPoint mirpick=m_pBaseNode->Position(false);
		if(fabs(pConnectPoint->datumplane.vLine.x)<fabs(pConnectPoint->datumplane.vLine.y))	//�����
			MirPartHandle(&Ta,&m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.hRod,pConnectPoint->m_hBasePart,MIRMSG(2),&mirpick);
		else	//�����
			MirPartHandle(&Ta,&m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.hRod,pConnectPoint->m_hBasePart,MIRMSG(1),&mirpick);
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.hNode=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.hNode=m_pBaseNode->handle;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.direction=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.direction=0;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.wing_offset_style=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.wing_offset_style=4;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.wing_offset_dist=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.wing_offset_dist=0;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.bFaceOffset=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.bFaceOffset=TRUE;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.vPlaneNormal.Set(0,0,1);
		m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.vPlaneNormal.Set(0,0,1);
	}
	else
	{
		m_pPlate->designInfo.huoquline_start[0].datum_pos_style=m_pPlate->designInfo.huoquline_end[0].datum_pos_style=2;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.hRod=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.hRod=pBendlineAngle->handle;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.hNode=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.hNode=pConnectPoint->m_hBaseNode;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.wing_offset_dist=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.wing_offset_dist=huoquoffset;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.offset_wing=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.offset_wing=bendwing_x0_y1;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.wing_offset_style=m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.wing_offset_style=4;
		m_pPlate->designInfo.huoquline_start[0].des_para.RODNODE.len_offset_dist=-200;
		m_pPlate->designInfo.huoquline_end[0].des_para.RODNODE.len_offset_dist= 200;
	}
	m_pPlate->designInfo.huoquline_start[0].UpdatePos(&Ta);
	m_pPlate->designInfo.huoquline_end[0].UpdatePos(&Ta);
	m_pPlate->HuoQuLine[0].startPt=m_pPlate->designInfo.huoquline_start[0].Position();
	m_pPlate->HuoQuLine[0].endPt=m_pPlate->designInfo.huoquline_end[0].Position();
	coord_trans(m_pPlate->HuoQuLine[0].startPt,m_pPlate->ucs,FALSE);
	coord_trans(m_pPlate->HuoQuLine[0].endPt,m_pPlate->ucs,FALSE);
	m_pPlate->HuoQuLine[0].startPt.z=m_pPlate->HuoQuLine[0].endPt.z=0;
	m_pPlate->jdb_style=0;
	m_pPlate->face_N=m_pPlate->designInfo.iFaceType=2;

	for(pRod=pConnectPoint->bendPlaneRodSet.GetFirst();pRod;pRod=pConnectPoint->bendPlaneRodSet.GetNext())
	{
		if(!pRod->IsAngle())
			continue;
		if (pCoupleMainRod == pRod)
			continue;
		int work_wing=0;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
		BOOL bInsideAngle=IsInsideJg(pAngle,m_pPlate->huoqufacenorm[0],&work_wing);
		if(pAngle->pStart->handle==m_pPlate->designInfo.m_hBaseNode)
			pAngle->desStartPos.ChangeToDatumType(3);	//תΪ����
		else //if(pAngle->pEnd->handle==m_pPlate->designInfo.m_hBaseNode)
			pAngle->desEndPos.ChangeToDatumType(3);
		pAngle->CalPosition(true);
		for(CLDSBolt* pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		{
			if(pAngle->FindLsByHandle(pBolt->handle))
			{
				pBolt->correct_worknorm();
				pBolt->correct_pos();
			}
		}
	}
	return TRUE;
}
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
#endif
//��ȡiRayRodOrder���߽Ǹֵ����ฺͷ������ո˼� wjh-2019.12.13
BOOL CCommonLjSmartDesign::GetOddCalNearRods(int iRayRodOrder,long* phRefRod1/*=NULL*/,long* phRefRod2/*=NULL*/)
{
	if (iRayRodOrder<0||iRayRodOrder>=m_arrRayRod.Count||!m_pBaseRod.blValid)
		return false;
	//��ʱ��תǰһ���߸˼�
	long hPrevRefRod=0,hNextRefRod=0;
	SmartRodPtr pRayRod=m_arrRayRod[iRayRodOrder].Rod();
	int count=m_arrRayRod.Count;
	int iPrevIndex=(iRayRodOrder+count-1)%count;
	if ((m_arrRayRod[iRayRodOrder].alfa>0&&m_arrRayRod[iPrevIndex].alfa>0)||
		(m_arrRayRod[iRayRodOrder].alfa<0&&m_arrRayRod[iPrevIndex].alfa<0))
	{
		if(m_arrRayRod[iRayRodOrder].pRod->size_wide<=m_arrRayRod[iPrevIndex].pRod->size_wide)
		{
			DEG curr(m_arrRayRod[iRayRodOrder].alfa);
			DEG prev(m_arrRayRod[iPrevIndex].alfa);
			if(curr<prev||(curr>180&&prev<180))
				hPrevRefRod=m_pBaseRod->handle;
			else if(curr>prev)	//���߼нǹ淶Ϊ0~360�Ⱥ�ǰһ���߼н�һ��Ҫ�ȵ�ǰ���߼н�С�������Ի�׼�˼�Ϊǰһ�н�
				hPrevRefRod=m_arrRayRod[iPrevIndex].pRod->handle;
		}
	}
	else	//��ǰһ�Ǹּн���ţ���ʾ�ֱ�λ�ڻ�׼������
		hPrevRefRod=this->m_pBaseRod->handle;
	int iNextIndex=(iRayRodOrder+1)%count;
	if ((m_arrRayRod[iRayRodOrder].alfa>0&&m_arrRayRod[iNextIndex].alfa>0)||
		(m_arrRayRod[iRayRodOrder].alfa<0&&m_arrRayRod[iNextIndex].alfa<0))
	{
		if(m_arrRayRod[iRayRodOrder].pRod->size_wide<=m_arrRayRod[iNextIndex].pRod->size_wide)
			hNextRefRod=m_arrRayRod[iNextIndex].pRod->handle;
	}
	else	//����һ�Ǹּн���ţ���ʾ�ֱ�λ�ڻ�׼������
		hNextRefRod=this->m_pBaseRod->handle;
	if (hPrevRefRod==0&&hNextRefRod!=m_pBaseRod->handle)
		hPrevRefRod=m_pBaseRod->handle;
	if(hNextRefRod==0&&hPrevRefRod!=m_pBaseRod->handle)
		hNextRefRod=m_pBaseRod->handle;
	if (hPrevRefRod==hNextRefRod&&hPrevRefRod!=m_pBaseRod->handle)
		hPrevRefRod=m_pBaseRod->handle;
	if (phRefRod1!=NULL)
		*phRefRod1=hPrevRefRod;
	if (phRefRod2!=NULL)
		*phRefRod2=hNextRefRod;
	return true;
}
long CCommonLjSmartDesign::DesignConnectPoint(CConnectPointInfo *pConnectPoint,int endljNum,BOOL bHasPlate,BOOL canMultiRow/*=TRUE*/,BOOL priorMiddleEndLj/*=TRUE*/,bool bCreate3dSolid/*=true*/)
{
	CLDSBolt* pBolt;
	m_pConnectPoint=pConnectPoint;
	m_arrRayRod.Empty();
	m_arrG1Cover.Empty();
	m_arrG2Cover.Empty();
	boltset.Empty();
	endlj_boltset.Empty();
	padSet.Empty();
	ucs.axis_z.Set();
	bCreate3dSolid&=UI::blEnableIntermediateUpdateUI;
	m_pFirstEndLjBolt=m_pSecondEndLjBolt=NULL;
	try{
		if(!Prepare(endljNum,bHasPlate,canMultiRow,priorMiddleEndLj))
			return FALSE;
		//�жϵ����Ǹ��ϵ�˫��˨�Ƿ�ɺϷ����õ���׼���ϵ�˫��������
		int i=0;
		LINEPARTSET linkPartSet;
		CSuperSmartPtr<CLDSLinePart> pLinePart;
		//���ù��ö�������˨
		CLDSLineAngle *pFirstEndLjAngle=NULL;
		CLDSLineAngle *pSecondEndLjAngle=NULL;
		CLDSLineAngle* pTransitPrimaryAngle=NULL;	//���°���һ���׼��
		if(pConnectPoint->m_bTransitConnect)
		{
			CLDSLinePart* pPrimaryRod=pConnectPoint->bendPlaneRodSet.GetFirst();
			if(pPrimaryRod->IsAngle())
				pTransitPrimaryAngle=(CLDSLineAngle*)pPrimaryRod;
		}
		if(endljNum>0)
		{
			int endboltNum=0;
			if(m_iFirstEndLjAngle>=0)
			{
				pFirstEndLjAngle=m_arrRayRod[m_iFirstEndLjAngle].Angle();
				if(m_arrRayRod[m_iFirstEndLjAngle].endLjBoltNum>endboltNum)
					endboltNum=m_arrRayRod[m_iFirstEndLjAngle].endLjBoltNum;
			}
			if(endljNum==2&&m_iSecEndLjAngle>=0&&m_iSecEndLjAngle!=m_iFirstEndLjAngle)
			{
				pSecondEndLjAngle=m_arrRayRod[m_iSecEndLjAngle].Angle();
				if(m_arrRayRod[m_iSecEndLjAngle].endLjBoltNum>endboltNum)
					endboltNum=m_arrRayRod[m_iSecEndLjAngle].endLjBoltNum;
			}
			wing_intersline_style=0;
			if(endboltNum==1)	//ֻ��һ�Ŷ�������˨����ʱ�����ӽǸֽ��ڱ�׼����
			{
				if(m_iBaseAngleWorkWing==0)
					m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style=0;
				else
					m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style=0;
			}
			else if(endboltNum==2&&m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{	//�����Ŷ�������˨ʱ�������ӽǸֽ��ڵ�1��������
				if(m_iBaseAngleWorkWing==0)
					wing_intersline_style=m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style=1;
				else
					wing_intersline_style=m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style=1;
			}
			CLDSBolt *pShareLjBolt=m_pFirstEndLjBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			boltset.append(pShareLjBolt);
			pShareLjBolt->iSeg=m_pBaseNode->iSeg;
			pShareLjBolt->SetLayer(m_pBaseRod->layer());
			pShareLjBolt->cfgword=cfgword;
			pShareLjBolt->iSeg=m_pBaseNode->iSeg;
			pShareLjBolt->set_d(m_arrRayRod[m_iFirstEndLjAngle].pConn->d);
			pShareLjBolt->SetGrade(m_arrRayRod[m_iFirstEndLjAngle].pConn->grade);
			pShareLjBolt->AddL0Thick(m_pBaseRod->handle,TRUE);
			if(pFirstEndLjAngle)
				pShareLjBolt->AddL0Thick(pFirstEndLjAngle->handle,TRUE);
			if(pSecondEndLjAngle)
				pShareLjBolt->AddL0Thick(pSecondEndLjAngle->handle,TRUE);
			if(m_pPlate)
				pShareLjBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pShareLjBolt->CalGuigeAuto();
			pShareLjBolt->des_work_norm.norm_style=1;
			pShareLjBolt->des_work_norm.hVicePart=m_pBaseRod->handle;
			pShareLjBolt->des_work_norm.direction=0;
			pShareLjBolt->des_work_norm.norm_wing=m_iBaseAngleWorkWing;
			pShareLjBolt->des_base_pos.hPart=m_pBaseRod->handle;
			pShareLjBolt->des_base_pos.offset_wing=m_iBaseAngleWorkWing;
			pShareLjBolt->des_base_pos.datumPoint.datum_pos_style=3;
			pShareLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1=m_pBaseRod->handle;
			pShareLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=pFirstEndLjAngle->handle;
			pShareLjBolt->des_base_pos.norm_offset.AddThick(-m_pBaseRod->handle,TRUE);
			if(m_arrRayRod[m_iFirstEndLjAngle].bInsideAngle)
				pShareLjBolt->des_base_pos.norm_offset.AddThick(-pFirstEndLjAngle->handle,TRUE);
			else if(m_iSecEndLjAngle>0&&m_arrRayRod[m_iSecEndLjAngle].bInsideAngle)
				//��Ҫ�ж�m_iSecEndLjAngle>0��ֹ������˨������ʱm_iSecEndLjAngleֵΪ-1
				pShareLjBolt->des_base_pos.norm_offset.AddThick(-pSecondEndLjAngle->handle,TRUE);
			if(m_iFirstEndLjAngle>=0&&m_arrRayRod[m_iFirstEndLjAngle].bInsideAngle)
				pFirstEndLjAngle->CalCutAngleInfo(BaseAngle(),NULL,pFirstEndLjAngle->pStart==m_pBaseNode,NULL);
			if(m_iSecEndLjAngle>=0&&m_arrRayRod[m_iSecEndLjAngle].bInsideAngle)
				pSecondEndLjAngle->CalCutAngleInfo(BaseAngle(),NULL,pSecondEndLjAngle->pStart==m_pBaseNode,NULL);
			
			//������������
			short siStyleG=0;
			double dfOffsetDistG=0;
			if (m_pBaseRod->chLayer1st!='T'&&m_pBaseRod->chLayer2nd=='Z'&&DesignBraceConnParams(&siStyleG,&dfOffsetDistG))
			{	//��������Ժ������֤�����������Ľ���������(��)������� wjh-2019.12.13
				if (m_iBaseAngleWorkWing == 0)
				{
					m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style=siStyleG;
					m_pBaseNode->xFatherAngleZhunJu.offset_X_dist=dfOffsetDistG;
				}
				else
				{
					m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style=siStyleG;
					m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist=dfOffsetDistG;
				}
			}
			if (m_iBaseAngleWorkWing == 0)
			{
				pShareLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 =
					(short)m_pBaseNode->xFatherAngleZhunJu.offset_X_dist_style;
				pShareLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 =
					(short)m_pBaseNode->xFatherAngleZhunJu.offset_X_dist;
			}
			else
			{
				pShareLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1 =
					(short)m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist_style;
				pShareLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1 =
					(short)m_pBaseNode->xFatherAngleZhunJu.offset_Y_dist;
			}
			pShareLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2=0;
			if(m_pBaseRod->pStart==m_pBaseNode)
				m_pBaseRod->AppendStartLsRef(pShareLjBolt->GetLsRef());
			else if(m_pBaseRod->pEnd==m_pBaseNode)
				m_pBaseRod->AppendEndLsRef(pShareLjBolt->GetLsRef());
			else
				m_pBaseRod->AppendMidLsRef(pShareLjBolt->GetLsRef());
			if(pFirstEndLjAngle->pStart==m_pBaseNode)
				pFirstEndLjAngle->AppendStartLsRef(pShareLjBolt->GetLsRef());
			else
				pFirstEndLjAngle->AppendEndLsRef(pShareLjBolt->GetLsRef());
			if(pSecondEndLjAngle&&pSecondEndLjAngle->pStart==m_pBaseNode)
			{
				pSecondEndLjAngle->AppendStartLsRef(pShareLjBolt->GetLsRef());
				if(pSecondEndLjAngle->connectStart.d>pShareLjBolt->get_d())
					pShareLjBolt->set_d(pSecondEndLjAngle->connectStart.d);
			}
			else if(pSecondEndLjAngle&&pSecondEndLjAngle->pEnd==m_pBaseNode)
			{
				pSecondEndLjAngle->AppendEndLsRef(pShareLjBolt->GetLsRef());
				if(pSecondEndLjAngle->connectEnd.d>pShareLjBolt->get_d())
					pShareLjBolt->set_d(pSecondEndLjAngle->connectEnd.d);
			}
			if(m_pPlate)
				m_pPlate->AppendLsRef(pShareLjBolt->GetLsRef());
			//����ר�ö�������˨
			for(i=0;i<endljNum;i++)
			{
				int iEndLj=0;
				if(i==0&&pFirstEndLjAngle)
					iEndLj=m_iFirstEndLjAngle;
				else if(i==1&&pSecondEndLjAngle)
					iEndLj=m_iSecEndLjAngle;
				else
					break;
				if(m_arrRayRod[iEndLj].endLjBoltNum==2)	//�ڶ��Ŷ�������˨���ڻ�׼�ĵڶ�������
				{
					CLDSBolt *pSecondEndLjBolt=m_pSecondEndLjBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					boltset.append(pSecondEndLjBolt);
					pSecondEndLjBolt->iSeg=m_pBaseNode->iSeg;
					pSecondEndLjBolt->CopyProperty(pShareLjBolt);
					pSecondEndLjBolt->SetLayer(m_pBaseRod->layer());
					pSecondEndLjBolt->cfgword=m_pBaseRod->cfgword;
					pSecondEndLjBolt->EmptyL0DesignPara();
					pSecondEndLjBolt->AddL0Thick(m_pBaseRod->handle,TRUE);
					pSecondEndLjBolt->AddL0Thick(m_arrRayRod[iEndLj].Rod()->handle,TRUE);
					if(m_pPlate)
						pSecondEndLjBolt->AddL0Thick(m_pPlate->handle,TRUE);
					if(m_pBaseRod->pStart==m_pBaseNode)
						m_pBaseRod->AppendStartLsRef(pSecondEndLjBolt->GetLsRef(),FALSE);
					else if(m_pBaseRod->pEnd==m_pBaseNode)
						m_pBaseRod->AppendEndLsRef(pSecondEndLjBolt->GetLsRef(),FALSE);
					else
						m_pBaseRod->AppendMidLsRef(pSecondEndLjBolt->GetLsRef(),FALSE);
					if(m_arrRayRod[iEndLj].Angle()->pStart==m_pBaseNode)
						m_arrRayRod[iEndLj].Angle()->AppendStartLsRef(pSecondEndLjBolt->GetLsRef(),FALSE);
					else //if(m_arrRayAngle[iEndLj]->pEnd==m_pBaseNode)
						m_arrRayRod[iEndLj].Angle()->AppendEndLsRef(pSecondEndLjBolt->GetLsRef(),FALSE);
					if(m_pPlate)
						m_pPlate->AppendLsRef(pSecondEndLjBolt->GetLsRef(),FALSE);
					pSecondEndLjBolt->des_work_norm=pShareLjBolt->des_work_norm;
					pSecondEndLjBolt->des_base_pos=pShareLjBolt->des_base_pos;
					pSecondEndLjBolt->des_base_pos.hPart=m_pBaseRod->handle;
					pSecondEndLjBolt->des_base_pos.offset_wing=m_iBaseAngleWorkWing;
					pSecondEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[iEndLj].Rod()->handle;
					bool directStart=true;
					if(m_arrRayRod[iEndLj].rayVec*ucs.axis_x>0)
						directStart=false;
					if(directStart&&m_nBaseStartG3>0)
					{
						pSecondEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
						pSecondEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=m_nBaseStartG3;
					}
					else if(!directStart&&m_nBaseEndG3>0)
					{
						pSecondEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=4;
						pSecondEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_dist1=m_nBaseEndG3;
					}
					else
						pSecondEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style1=2;
					pSecondEndLjBolt->des_base_pos.norm_offset.EmptyThick();
					pSecondEndLjBolt->des_base_pos.norm_offset.AddThick(-m_pBaseRod->handle,TRUE);
					if(m_arrRayRod[iEndLj].bInsideAngle)
						pSecondEndLjBolt->des_base_pos.norm_offset.AddThick(-m_arrRayRod[iEndLj].Rod()->handle,TRUE);
					pSecondEndLjBolt->CalGuigeAuto();
				}
			}
		}
		//�������߲Ķ�ͷλ��
		for (i = 0; i < m_arrRayRod.GetSize(); i++)
		{
			int iEndLj = 0;
			if (pConnectPoint->m_bTransitConnect&&m_arrRayRod[i].ciPlaneNo == 2 && m_arrRayRod[i].pRod->chLayer2nd == 'Z')
				continue;	//���°���һ���׼��
			CLDSGroupLineAngle* pGroupAngle = NULL;
			m_arrRayRod[i].blViewAsSingularAngle = m_arrRayRod[i].pRod->IsAngle();
			if (m_arrRayRod[i].pRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
			{
				pGroupAngle = (CLDSGroupLineAngle*)m_arrRayRod[i].pRod;
				if (pGroupAngle->size_cSubClassType != 'T' || pGroupAngle->ciMasterIndex == m_arrRayRod[i].work_wing)
					m_arrRayRod[i].blViewAsSingularAngle = false;	//����֫���ǵ�ǰ��������֫����ʱ��Ӧ��ͬ�������Ǹ�һ������ 2jh-2019.11.26
			}
			if (m_pBaseRod->GetClassTypeId() == CLS_LINETUBE)
			{
				if (m_arrRayRod[i].Rod()->IsAngle())
				{
					if (m_arrRayRod[i].work_wing == 0)	//X֫����
						m_arrRayRod[i].AnglePos()->wing_x_offset.offsetDist = -0.5*m_pPlate->GetThick();
					else if (m_arrRayRod[i].work_wing == 1)	//Y֫����
						m_arrRayRod[i].AnglePos()->wing_y_offset.offsetDist = -0.5*m_pPlate->GetThick();
				}
				else if (m_arrRayRod[i].Rod()->GetClassTypeId() == CLS_LINETUBE)
					m_arrRayRod[i].TubePos()->fNormOffset = -0.5*m_pPlate->GetThick();
			}
			else if (pGroupAngle && !m_arrRayRod[i].blViewAsSingularAngle)
			{	//�ܶ��������ϽǸ��м�ļ�϶���÷������Ӱ壬��˲���Ҫ�����ع���֫����ƫ�� wjh-2019.11.26
				if (m_arrRayRod[i].work_wing == 0)	//X֫����
					m_arrRayRod[i].AnglePos()->wing_x_offset.SetOffset(0, 4);
				else
					m_arrRayRod[i].AnglePos()->wing_y_offset.SetOffset(0, 4);
			}
			else if (m_arrRayRod[i].bInsideAngle)	//����
			{
				if (m_arrRayRod[i].work_wing == 0)	//X֫����
				{
					m_arrRayRod[i].AnglePos()->wing_x_offset.gStyle = 4;
					if (i == m_iFirstEndLjAngle || i == m_iSecEndLjAngle)	//�����ӽǸ�
						m_arrRayRod[i].AnglePos()->wing_x_offset.offsetDist = -m_pBaseRod->GetThick();
					else
						m_arrRayRod[i].AnglePos()->wing_x_offset.offsetDist = 0;
					m_arrRayRod[i].AnglePos()->wing_y_offset.gStyle = 0;
				}
				else
				{
					m_arrRayRod[i].AnglePos()->wing_x_offset.gStyle = 0;
					m_arrRayRod[i].AnglePos()->wing_y_offset.gStyle = 4;
					if (i == m_iFirstEndLjAngle || i == m_iSecEndLjAngle)	//�����ӽǸ�
						m_arrRayRod[i].AnglePos()->wing_y_offset.offsetDist = -m_pBaseRod->GetThick();
					else
						m_arrRayRod[i].AnglePos()->wing_y_offset.offsetDist = 0;
				}
			}
			else	//����
			{
				if (m_arrRayRod[i].work_wing == 0)	//X֫����
				{
					m_arrRayRod[i].AnglePos()->wing_x_offset.gStyle = 4;
					if (m_pPlate)
						m_arrRayRod[i].AnglePos()->wing_x_offset.offsetDist = -m_pPlate->GetThick();
					else
						m_arrRayRod[i].AnglePos()->wing_x_offset.offsetDist = 0;
					m_arrRayRod[i].AnglePos()->wing_y_offset.gStyle = 0;
				}
				else
				{
					m_arrRayRod[i].AnglePos()->wing_x_offset.gStyle = 0;
					m_arrRayRod[i].AnglePos()->wing_y_offset.gStyle = 4;
					if (m_pPlate)
						m_arrRayRod[i].AnglePos()->wing_y_offset.offsetDist = -m_pPlate->GetThick();
					else
						m_arrRayRod[i].AnglePos()->wing_y_offset.offsetDist = 0;
				}
			}
			m_arrRayRod[i].Rod()->ClearFlag();
			m_arrRayRod[i].Rod()->CalPosition();
		}
		//�����ڵ��������߽Ǹֺ��������˨λ�ã���Ϊ��˨λ�������������׼���󽻵��������
		for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		{
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			if(bCreate3dSolid)
			{
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
		}
		//���û�׼����������˨
		double base_oddment=0;
		DESIGNODDMENT desOdd;
		CLDSPlate *pPlate=m_pPlate;
		if(m_pBaseRod->IsAngle())
			LayoutBaseAngleBolts();
		//��׼�˼�ʼ�ն��������Ӱ�ʱӦ�ȸ�������ͷ�Ա�֤���߲���ײ���,���������ĺ��ٻָ�
		if(m_pBaseRod->pStart==m_pBaseNode)
		{
			base_oddment=m_pBaseRod->startOdd();
			desOdd=m_pBaseRod->desStartOdd;
			if(pConnectPoint->m_bTransitConnect)
			{
				desOdd.m_iOddCalStyle=2;
				m_pBaseRod->SetStartOdd(-5);
			}
			else //if(desOdd.m_iOddCalStyle==0)
				m_pBaseRod->SetStartOdd(500);
		}
		else if(m_pBaseRod->pEnd==m_pBaseNode)
		{
			base_oddment=m_pBaseRod->endOdd();
			desOdd=m_pBaseRod->desEndOdd;
			if(pConnectPoint->m_bTransitConnect)
			{
				desOdd.m_iOddCalStyle=2;
				m_pBaseRod->SetEndOdd(-5);
			}
			else //if(desOdd.m_iOddCalStyle==0)
				m_pBaseRod->SetEndOdd(500);
		}
		//�������߲�������˨
		CDesignLjPartPara *pLjPartPara=NULL;
		CLDSLineAngle *pEndLjRayJg=NULL;//���������߽Ǹ�
		double len_offset=0,wing_offset=0;
		for(i=0;i<m_arrRayRod.GetSize();i++)
		{	
#ifdef __TIMER_COUNT_
			DWORD dwStartTick=timer.Relay();
#endif
			pLinePart=m_arrRayRod[i].Rod();
			if(pLinePart->IsAngle())
			{
				if(pPlate)	//TODO:��Ҫ��ʵ֮ǰδ�����Ӹ˼�����Ƿ�BUG wjh-2017.3.24
					pLjPartPara=pPlate->designInfo.partList.Add(pLinePart->handle);
					//pLjPartPara=pPlate->designInfo.partList.append();
				if(pLinePart->pStart->handle==pConnectPoint->m_hBaseNode)
				{
					if(pPlate)
						pLjPartPara->start0_end1=0;
					if(pTransitPrimaryAngle==pLinePart)
					{
						pTransitPrimaryAngle->desStartOdd.m_iOddCalStyle=2;
						pTransitPrimaryAngle->SetStartOdd(-5);
					}
					else if(i==m_iFirstEndLjAngle||i==m_iSecEndLjAngle)
						pLinePart->desStartOdd.m_iOddCalStyle=1;	//���������������˨��������ͷ
					else
					{
						pLinePart->desStartOdd.m_iOddCalStyle = 0;
						pLinePart->desStartOdd.m_fCollideDist = g_sysPara.VertexDist;
						GetOddCalNearRods(i,&pLinePart->desStartOdd.m_hRefPart1,&pLinePart->desStartOdd.m_hRefPart2);
						/*pLinePart->desStartOdd.m_hRefPart1=m_pBaseRod->handle;
						if(m_arrRayRod.GetSize()>1)
						{
							if(i==0)
								pLinePart->desStartOdd.m_hRefPart2=m_arrRayRod[i+1].Rod()->handle;
							else
							{
								if(i+1>=m_arrRayRod.GetSize())
									pLinePart->desStartOdd.m_hRefPart2=m_arrRayRod[i-1].Rod()->handle;
								//else if(m_arrRayRod[i].alfa-m_arrRayRod[i-1].alfa<m_arrRayRod[i+1].alfa-m_arrRayRod[i].alfa)
								//	pLinePart->desStartOdd.m_hRefPart2=m_arrRayRod[i-1].Rod()->handle;
							}
						}*/
						pLinePart->CalStartOddment();
					}
				}
				else
				{
					if(pPlate)
						pLjPartPara->start0_end1=1;
					if(pTransitPrimaryAngle==pLinePart)
					{
						pTransitPrimaryAngle->desEndOdd.m_iOddCalStyle=2;
						pTransitPrimaryAngle->SetEndOdd(-5);
					}
					else if(i==m_iFirstEndLjAngle||i==m_iSecEndLjAngle)
						pLinePart->desEndOdd.m_iOddCalStyle=1;
					else
					{
						pLinePart->desEndOdd.m_iOddCalStyle = 0;
						pLinePart->desEndOdd.m_fCollideDist = g_sysPara.VertexDist;
						GetOddCalNearRods(i,&pLinePart->desEndOdd.m_hRefPart1,&pLinePart->desEndOdd.m_hRefPart2);
						/*pLinePart->desEndOdd.m_hRefPart1=m_pBaseRod->handle;
						if(i==0&&m_arrRayRod.GetSize()>1)
							pLinePart->desEndOdd.m_hRefPart2=m_arrRayRod[i+1].Rod()->handle;
						else if(i>0)
							pLinePart->desEndOdd.m_hRefPart2=m_arrRayRod[i-1].Rod()->handle;*/
						pLinePart->CalEndOddment();
					}
				}
#ifdef __TIMER_COUNT_
				timer.Relay(100,dwStartTick);
#endif
				LSSPACE_STRU LsSpace;
				GetLsSpace(LsSpace,m_arrRayRod[i].pConn->d);
				if(pLjPartPara)
				{
					pLjPartPara->angle.cur_wing_x0_y1=m_arrRayRod[i].work_wing;
					pLjPartPara->hPart=pLinePart->handle;
					pLjPartPara->end_space=LsSpace.EndSpace;
					pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
					pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
					//pLjPartPara->ber_space=m_arrRayRod[i].jgzj.g;
					//pLjPartPara->wing_space=pLinePart->GetWidth()-m_arrRayRod[i].jgzj.g;
					if(pConnectPoint->bendPlaneRodSet.FromHandle(pLinePart->handle))
						pLjPartPara->iFaceNo=2;
					else
						pLjPartPara->iFaceNo=1;
					pLjPartPara->m_nClassTypeId=pLinePart->GetClassTypeId();
					if(endljNum>0&&(i==m_iFirstEndLjAngle||i==m_iSecEndLjAngle))
						pLjPartPara->angle.bEndLjJg = TRUE;//�Գ�ʱ��Ҫ����Ϣ
				}
				//�������߽Ǹ���˨
				LayoutAngleBolts(i);
				if(pLinePart->pStart==m_pBaseNode&&pLinePart->desStartOdd.m_iOddCalStyle==1)
					pLinePart->CalStartOddment();	//ʼ��Ϊ��˨ȷ������ͷ
				else if(pLinePart->pEnd==m_pBaseNode&&pLinePart->desEndOdd.m_iOddCalStyle==1)
					pLinePart->CalEndOddment();
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//�������߸ֹܲ��,���ݸֹܹ��ӹ����в��Ҹֹܲ��
				int start0_end1=0;
				TUBEJOINT *pCurJoint=NULL;
				CTubeEndPosPara *pCurDesPara=NULL;
				if(pLinePart->pStart->handle==pConnectPoint->m_hBaseNode)
				{
					start0_end1=0;
					pCurDesPara=&pLinePart.LineTubePointer()->desStartPos;
					pCurJoint=&pLinePart.LineTubePointer()->m_tJointStart;
				}
				else
				{
					start0_end1=1;
					pCurDesPara=&pLinePart.LineTubePointer()->desEndPos;
					pCurJoint=&pLinePart.LineTubePointer()->m_tJointEnd;
				}
				CLDSParamPlate *pParamPlate=NULL;
				pParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pCurJoint->hLinkObj,CLS_PARAMPLATE);
				CLDSStdPartModel stdPartModel;
				//����Ԥ�������ȼ����Ҷ�Ӧ��׼�����Ƿ�����Ӧ�ı�׼����
				int i=0,j=0,iTubeEndLjStyle=0;
				BOOL bFindStdModel=FALSE;
				double tube_t=pLinePart->GetThick();
				double tube_d=pLinePart.LineTubePointer()->GetDiameter();
				PARAM_PLATE_TYPE iFirstType=TYPE_ROLLEND;
				for(i=0;i<g_sysPara.m_arrInsertPlateType.GetSize();i++)
				{
					PARAM_PLATE_TYPE iPlateType;
					if(g_sysPara.m_arrInsertPlateType[i].CompareNoCase("C_END")==0)
					{
						iTubeEndLjStyle=4;
						iPlateType=TYPE_ROLLEND;
					}
					else if(g_sysPara.m_arrInsertPlateType[i].CompareNoCase("U_END")==0)
					{
						iTubeEndLjStyle=5;
						iPlateType=TYPE_UEND;
					}
					else if(g_sysPara.m_arrInsertPlateType[i].CompareNoCase("X_END")==0)
					{
						iTubeEndLjStyle=6;
						iPlateType=TYPE_XEND;
					}
					else 
						continue;
					if (i==0)
						iFirstType = iPlateType;
					for(j=9;j>=0;j--)
					{
						if(pParamPlate==NULL&&globalLibrary.GetStdPartModel(
							tube_d,tube_t,pLinePart->cMaterial,j,iPlateType,&stdPartModel))
						{
							bFindStdModel=TRUE;
							break;
						}
					}
					if(bFindStdModel)
						break;
				}
				int nLsNum=0,nLsD=0,nLsRow;
				if(bFindStdModel)
				{	//���ɱ�׼����
					pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
					pParamPlate->m_bStdPart=TRUE;	//��׼����
					pParamPlate->m_uStatMatCoef=1;
					pParamPlate->iSeg=pLinePart->iSeg;
					pParamPlate->m_hPartWeldParent = pLinePart->handle;
					pParamPlate->SetLayer(pLinePart->layer()); //������������ͼ����
					pParamPlate->cfgword=pLinePart->cfgword;
					pParamPlate->cMaterial = pLinePart->cMaterial;
					pParamPlate->SetPartNo(stdPartModel.param.insert_plate_param.codeName,true);
					pParamPlate->m_iParamType=stdPartModel.m_iStdPartType;
					//���ù���������ڸֹܻ�׼�����ת�Ƕ� 
					//����ĸ'X'��ʾ��ת�Ƕȣ�D���ע������������ wht 10-01-21
					pParamPlate->SetDesignItemValue('X',180,"��������Ըֹܻ�׼����ת�Ƕ�");
				}
				else
				{
					//������ͨ��
					pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
					pParamPlate->m_bStdPart=FALSE;
					pParamPlate->m_uStatMatCoef=1;
					pParamPlate->iSeg=pLinePart->iSeg;
					pParamPlate->m_hPartWeldParent = pLinePart->handle;
					pParamPlate->SetLayer(pLinePart->layer()); //������������ͼ����
					pParamPlate->cfgword=pLinePart->cfgword;
					pParamPlate->cMaterial = pLinePart->cMaterial;
					pParamPlate->SetPartNo(CXhChar16("X%.0f%.0fH1",tube_d,tube_t));
					sprintf(stdPartModel.param.insert_plate_param.codeName,"X%.0f%.0fH1",tube_d,tube_t);
					stdPartModel.m_iStdPartType = iFirstType;
					stdPartModel.param.insert_plate_param.iBoltRect = 1;
					if(!start0_end1)
					{
						nLsNum = pLinePart.LineTubePointer()->connectStart.wnConnBoltN;
						nLsD = pLinePart.LineTubePointer()->connectStart.d;
						nLsRow = pLinePart.LineTubePointer()->connectStart.rows;
						stdPartModel.param.insert_plate_param.ls_param[0].iRow = nLsRow;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltNum = nLsNum;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltD = pLinePart.LineTubePointer()->connectStart.d;
					}
					else
					{
						nLsNum = pLinePart.LineTubePointer()->connectEnd.wnConnBoltN;
						nLsD = pLinePart.LineTubePointer()->connectEnd.d;
						nLsRow = pLinePart.LineTubePointer()->connectEnd.rows;
						stdPartModel.param.insert_plate_param.ls_param[0].iRow = pLinePart.LineTubePointer()->connectEnd.rows;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltNum = pLinePart.LineTubePointer()->connectEnd.wnConnBoltN;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltD = pLinePart.LineTubePointer()->connectEnd.d;
					}
					//��˨
					LSSPACE_STRU LsSpace;
					GetLsSpace(LsSpace,nLsD);
					pParamPlate->m_iParamType=iFirstType;
					pParamPlate->thick=8;
					//���ù���������ڸֹܻ�׼�����ת�Ƕ� 
					//����ĸ'X'��ʾ��ת�Ƕȣ�D���ע������������ wht 10-01-21
					pParamPlate->SetDesignItemValue('X',180,"��������Ըֹܻ�׼����ת�Ƕ�");
					double A=0,N=0,M=0,R=0,L=0,H=0,S=0,W=0;
					double fVertSpace = 0;
					if (nLsRow>1)
						fVertSpace=(tube_d-nLsD)/(nLsRow-1);
					double fHorzSpace = nLsD+30;
					int nColunm = (nLsNum+nLsRow-1)/nLsRow;
					A=10;
					R=15;
					L=((nColunm-1)*fHorzSpace+2*LsSpace.PlateEdgeSpace)*2;
					M=L/2-10;
					W=40;
					if(iFirstType==TYPE_UEND)
						W=pPlate->GetThick();
					H=tube_d+60;
					if (tube_d<190)
						S=N=30;
					else
						S=N=40;
					if(iFirstType==TYPE_XEND)
					{
						N=0.5*(H-60);
						S=L;
						W = 8;
						sprintf(stdPartModel.param.insert_plate_param.weldPlatePartNo,"XL%.0f%.0fH1",tube_d,tube_t);
						sprintf(stdPartModel.param.insert_plate_param.wrapPlatePartNo,"XL%.0f%.0fH1",tube_d,tube_t);
					}
					pParamPlate->SetDesignItemValue('A',A);
					pParamPlate->SetDesignItemValue('N',N);
					pParamPlate->SetDesignItemValue('M',M);
					pParamPlate->SetDesignItemValue('R',R);
					pParamPlate->SetDesignItemValue('L',L);
					pParamPlate->SetDesignItemValue('H',H);
					pParamPlate->SetDesignItemValue('S',S);
					pParamPlate->SetDesignItemValue('W',W);
					pParamPlate->SetDesignItemValue('T',pParamPlate->thick);
					
					stdPartModel.param.insert_plate_param.ls_param[0].fHorzSpace = nLsD+30;
					stdPartModel.param.insert_plate_param.ls_param[0].fVertSpace = fVertSpace;
					stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosX = M+LsSpace.PlateEdgeSpace+10;
					stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY = -tube_d*0.5+nLsD*0.5;
					if(nLsRow==1&&pParamPlate->m_iParamType!=TYPE_XEND)
						stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY = 0;
				}
				if(pParamPlate)
				{
					//���ݲ������ȵ����ְ���
					if(pPlate->GetThick()<pParamPlate->GetThick())
						pPlate->Thick=ftoi(pParamPlate->GetThick());
					//�����߸˼���ӵ��ְ����Ӹ˼��б���
					pLjPartPara=pPlate->designInfo.partList.Add(pLinePart->handle);
					pLjPartPara->iFaceNo=1;
					pLjPartPara->m_nClassTypeId=pLinePart->GetClassTypeId();
					pLjPartPara->start0_end1=start0_end1;
					//�Զ������ֹܻ���λ��λ��
					if(pParamPlate->m_iParamType==TYPE_ROLLEND)
					{
						double fWorkPlaneRotAngle=0;	//����ƽ����ת�Ƕ�
						double offset_dist=-0.5*(pParamPlate->GetThick()+pPlate->GetThick());
						double X=0;	//��X��ʾ��ת�Ƕ� D���׼������ز���ͬ�� ��ʱ��˴��� wht 10-01-21
						if(pParamPlate->m_bStdPart)
							pParamPlate->GetDesignItemValue('X',&fWorkPlaneRotAngle);
						else
							pParamPlate->GetDesignItemValue('D',&fWorkPlaneRotAngle);
						if(fabs(fabs(fWorkPlaneRotAngle)-180)<EPS)	//����߷����෴���巨�߷����෴
							offset_dist*=-1.0;	
						if(g_sysPara.nAutoAdjustTubePos==2)	//�ֹ�ƫ��
							pCurDesPara->fNormOffset+=offset_dist;
						else if(g_sysPara.nAutoAdjustTubePos==1) //���ƫ��
							pParamPlate->m_fNormOffset-=offset_dist;
					}
					pLinePart.LineTubePointer()->ClearFlag();
					pLinePart.LineTubePointer()->CalPosition();	//����ֹ�λ��
					pLinePart.LineTubePointer()->CalWorkPlaneNorm();
					if(pParamPlate->m_bStdPart)
					{
						pCurJoint->type=iTubeEndLjStyle;	//���Ͳ��
						pCurJoint->hLinkObj=pParamPlate->handle;
						pParamPlate->DesignStdPartUcs();
					}
					if(!pParamPlate->m_bStdPart)//�Ǳ�׼���
					{
						if(start0_end1==0)
						{
							pLinePart.LineTubePointer()->m_tJointStart.hLinkObj=pParamPlate->handle;
							pLinePart.LineTubePointer()->m_tJointStart.type = iFirstType;
							pLinePart.LineTubePointer()->DesignStartJoint();
						}
						else if(start0_end1==1)
						{
							pLinePart.LineTubePointer()->DesignEndJointUCS(pParamPlate);
						}
					}
					//�ֹ���ײ��϶Ӧ���Ǻ���߶�(Ĭ��Ϊ1.2t)
					if(pLinePart->pStart->handle==pConnectPoint->m_hBaseNode)
					{
						pLinePart.LineTubePointer()->desStartOdd.m_hRefPart1=m_pBaseRod->handle;
						pLinePart.LineTubePointer()->desStartOdd.m_fCollideDist = g_sysPara.VertexDist+ftoi(1.2*pPlate->Thick);
						pLinePart.LineTubePointer()->CalStartOddment();
					}
					else
					{
						pLinePart.LineTubePointer()->desEndOdd.m_hRefPart1=m_pBaseRod->handle;	
						pLinePart.LineTubePointer()->desEndOdd.m_fCollideDist = g_sysPara.VertexDist+ftoi(1.2*pPlate->Thick);
						pLinePart.LineTubePointer()->CalEndOddment();
					}
					
					pParamPlate->DesignPlate();
					if(bCreate3dSolid)
					{
						pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
						g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
					}
					
					if(pParamPlate->m_iParamType==TYPE_XEND)
					{
						if(pParamPlate->m_bStdPart)
							LayoutInsertPlateBoltAndAttachPlate(pParamPlate,stdPartModel.param.insert_plate_param.weldPlatePartNo,
							stdPartModel.param.insert_plate_param.wrapPlatePartNo,FALSE);
						else
						{	
							//����ʮ�ֲ��
							double A,N,M,R,L,H,S,W;
							pParamPlate->GetDesignItemValue('A',&A);
							pParamPlate->GetDesignItemValue('N',&N);
							pParamPlate->GetDesignItemValue('M',&M);
							pParamPlate->GetDesignItemValue('R',&R);
							pParamPlate->GetDesignItemValue('L',&L);
							pParamPlate->GetDesignItemValue('H',&H);
							pParamPlate->GetDesignItemValue('S',&S);
							pParamPlate->GetDesignItemValue('W',&W);
							double x=A+M;
							double y=R;
							double z=0.5*pParamPlate->GetThick();
							//
							stdPartModel.param.insert_plate_param.L = L;
							stdPartModel.param.insert_plate_param.iBoltRect = 2;
							stdPartModel.param.insert_plate_param.ls_param[0].iBoltNum = nLsNum;
							stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosX = -A-10;
							stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY -= 2*R;
							LSSPACE_STRU LsSpace;
							GetLsSpace(LsSpace,nLsD);
							double disTowPlate = 10;
							S = ((nLsNum-1)*stdPartModel.param.insert_plate_param.ls_param[0].fHorzSpace+LsSpace.EndSpace*2)*2+disTowPlate;
							N = 0.5*(H-2*R);
							stdPartModel.param.insert_plate_param.ls_param[1].iBoltNum = nLsNum;
							stdPartModel.param.insert_plate_param.ls_param[1].fBoltPosX = stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosX
								+stdPartModel.param.insert_plate_param.ls_param[0].fHorzSpace*(nLsNum-1)
								+LsSpace.EndSpace*2+disTowPlate;
							stdPartModel.param.insert_plate_param.ls_param[1].fBoltPosY = stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY;
							stdPartModel.param.insert_plate_param.ls_param[1].fHorzSpace = nLsD+30;
							stdPartModel.param.insert_plate_param.ls_param[1].fVertSpace = stdPartModel.param.insert_plate_param.ls_param[0].fVertSpace;
							stdPartModel.CreateBoltInPlate(pParamPlate);
							
							CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z����������  Z����������
							CString sPartNo;		
							int i;
							for(i=0;i<2;i++)
							{
								CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
								pVertPlate->iSeg=pParamPlate->iSeg;
								pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
								pVertPlate->cMaterial = pParamPlate->cMaterial;
								pVertPlate->SetLayer(pParamPlate->layer()); //���������ͼ����
								pVertPlate->cfgword=pParamPlate->cfgword;
								pVertPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
								pVertPlate->Thick=ftoi(pParamPlate->GetThick());
								pVertPlate->SetPartNo(stdPartModel.param.insert_plate_param.weldPlatePartNo);
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
										if(pLsRef->start1_end2_mid0==41)//������˨��Z������������
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
										if(pLsRef->start1_end2_mid0==31)//������˨��Z������������
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
								if(bCreate3dSolid)
								{
									pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
									g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
								}
								//��������ӵ����ӹ����б���
								pParamPlate->AppendAttachPart(pVertPlate);
							}
							for(i=0;i<8;i++)
							{
								CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
								pPlate->iSeg=pParamPlate->iSeg;
								pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
								pPlate->cMaterial = pParamPlate->cMaterial;
								pPlate->SetLayer(pParamPlate->layer()); //���������ͼ����
								pPlate->cfgword=pParamPlate->cfgword;
								//����Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
								//pPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
								pPlate->Thick=ftoi(W);
								pPlate->SetPartNo(stdPartModel.param.insert_plate_param.weldPlatePartNo);
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
										if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
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
										if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
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
										if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
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
										if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
										}
									}
								}
								for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
								{	
									(*pLsRef)->SetModified();
									(*pLsRef)->correct_worknorm();
									(*pLsRef)->correct_pos();
									(*pLsRef)->CalGuigeAuto();
									if(bCreate3dSolid)
										(*pLsRef)->Create3dSolidModel();
								}
								pPlate->DesignSetupUcs();
								pPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
								pPlate->vertex_list.append(PROFILE_VERTEX(S,0,0));
								pPlate->vertex_list.append(PROFILE_VERTEX(S,N,0));
								pPlate->vertex_list.append(PROFILE_VERTEX(0,N,0));
								if(bCreate3dSolid)
								{
									pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
									g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
								}
								//��������ӵ����ӹ����б���
								pParamPlate->AppendAttachPart(pPlate);
							}
						}
					}
					else 
						stdPartModel.CreateBoltInPlate(pParamPlate);
					//������˨λ�ò���ʾ��˨
					double L=0;
					pParamPlate->GetDesignItemValue('L',&L);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{
						CLDSBolt *pBolt=pLsRef->GetLsPtr();
						if(fabs(pBolt->get_norm()*pParamPlate->ucs.axis_z)<EPS_COS2)
							continue;
						if(pParamPlate->m_iParamType==TYPE_XEND)
						{
							f3dPoint ls_pos=pBolt->ucs.origin;
							coord_trans(ls_pos,pParamPlate->ucs,FALSE);
							if(ls_pos.x<L)
								continue;
						}
						pPlate->AppendLsRef(*pLsRef);
						if(pBolt->des_work_norm.direction==1)	//����(��巨�߷����෴)
							pBolt->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
						pBolt->AddL0Thick(pPlate->handle,TRUE);
						pBolt->cfgword=pLinePart->cfgword;
						pBolt->correct_worknorm();
						pBolt->correct_pos();
						pBolt->CalGuigeAuto();
						if(bCreate3dSolid)
						{
							pBolt->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
						}
					}
					pParamPlate->ResetSonBoltRayNo();		//���ò������˨���ߺ�
					pParamPlate->GetLsRefList()->ResetBoltRefRayNo(pParamPlate);	//���ò������˨�����ߺ�
				}
			}
			//m_arrRayRod[iEndLj].Angle()->Create3dSolidModel();
			//g_pSolidDraw->NewSolidPart(m_arrRayRod[iEndLj].Angle()->GetSolidPartObject());
			if(bCreate3dSolid)
			{
				pLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pLinePart->GetSolidPartObject());
			}
		}
		if(m_pBaseRod->GetClassTypeId()==CLS_LINETUBE)
		{	//��׼����Ϊ�ֹ�
			pPlate->m_bWeldPart=TRUE;
			pPlate->m_hPartWeldParent=m_pBaseRod->handle;
			pPlate->designInfo.iProfileStyle0123=1;
			pPlate->m_fNormOffset = -0.5*pPlate->GetThick();
			pPlate->ucs.axis_x.Set(0,0,0);
			pPlate->DesignSetupUcs();
		}
		if(m_pPlate)
		{	//�а�ʱDesignPlate�л������˨λ��
			DesignBendPlane(pConnectPoint);
#ifdef __TIMER_COUNT_
			DWORD dwStartTick=timer.Relay();
#endif
			m_pPlate->DesignPlate();
#ifdef __TIMER_COUNT_
			timer.Relay(101,dwStartTick);
#endif
			if(bCreate3dSolid)
			{
				m_pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(m_pPlate->GetSolidPartObject());
			}
			//m_pPlate->designInfo.iProfileStyle0123=3;	//�Զ�������
		}
		else
		{	//�ް�ʱ��Ҫ�ڴ˸�����˨λ��
			for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
			{
				pBolt->correct_worknorm();
				pBolt->correct_pos();
			}
		}
		for(CLDSPlate *pBoltPad=(CLDSPlate*)padSet.GetFirst();pBoltPad;pBoltPad=(CLDSPlate*)padSet.GetNext())
		{
			pBoltPad->DesignSetupUcs();
			if(bCreate3dSolid)
			{
				pBoltPad->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBoltPad->GetSolidPartObject());
			}
		}
		for(pBolt=boltset.GetFirst();bCreate3dSolid&&pBolt;pBolt=boltset.GetNext())
		{
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		//�ָ���׼�˼�ʼ�ն�����ͷ��Ϣ,������ں���������������˨λ�û�δ���£���������ͷ�������
		if(m_pBaseRod->pStart==m_pBaseNode)
		{
			if(!pConnectPoint->m_bTransitConnect)
			{
				m_pBaseRod->desStartOdd.m_iOddCalStyle=1;
				m_pBaseRod->CalStartOddment();
			}
			if(bCreate3dSolid)
			{
				m_pBaseRod->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(m_pBaseRod->GetSolidPartObject());
			}
			//m_pBaseRod->SetStartOdd(base_oddment);
			//m_pBaseRod->desStartOdd=desOdd;
		}
		else if(m_pBaseRod->pEnd==m_pBaseNode)
		{
			if(!pConnectPoint->m_bTransitConnect)
			{
				m_pBaseRod->desEndOdd.m_iOddCalStyle=1;
				m_pBaseRod->CalEndOddment();
			}
			if(bCreate3dSolid)
			{
				m_pBaseRod->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(m_pBaseRod->GetSolidPartObject());
			}
			//m_pBaseRod->desEndOdd=desOdd;
			//m_pBaseRod->SetEndOdd(base_oddment);
		}
		if(bCreate3dSolid)
			g_pSolidDraw->Draw();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	if(m_pPlate)
		return m_pPlate->handle;
	else
		return TRUE;	//�ް�����
}
#include ".\StdComTempl\StdComModel.h"
#include ".\StdComTempl\NodeStdComTempl.h"
bool CCommonLjSmartDesign::DesignSlopeTransition(SLOPE_TRANSITION* pTransit,CIntelliModelDesigner* pIntelliDesigner/*=NULL*/)
{
#ifdef __SMART_DRAW_INC_
	if( !pTransit->pUpRod->IsAngle()||!pTransit->pDownRod->IsAngle()||
		(pTransit->pHoriRodX!=NULL&&!pTransit->pHoriRodX->IsAngle())||
		(pTransit->pHoriRodY!=NULL&&!pTransit->pHoriRodY->IsAngle()))
		return false;
	long hBaseAngleX=0,hBaseAngleY=0;
	if(pTransit->pHoriRodX)
		hBaseAngleX=pTransit->pHoriRodX->pStart==pTransit->pNode?((CLDSLineAngle*)pTransit->pHoriRodX)->desStartPos.datum_jg_h:((CLDSLineAngle*)pTransit->pHoriRodX)->desEndPos.datum_jg_h;
	if(pTransit->pHoriRodY)
		hBaseAngleY=pTransit->pHoriRodY->pStart==pTransit->pNode?((CLDSLineAngle*)pTransit->pHoriRodY)->desStartPos.datum_jg_h:((CLDSLineAngle*)pTransit->pHoriRodY)->desEndPos.datum_jg_h;
	if (hBaseAngleX == 0)	//���ڱ��µ��²����������ڵ���ĺŲ�ͬ��ʱ�ڵ㸸�˼��ܿ���Ϊ�ϲ�������� wjh-2019.5.26
		hBaseAngleX = pTransit->pDownRod->handle;// pTransit->pNode->hFatherPart;
	if (hBaseAngleY == 0)
		hBaseAngleY = pTransit->pDownRod->handle;// pTransit->pNode->hFatherPart;
	CLDSLineAngle* pAngle=NULL,*pDatumAngle=NULL,*pPrimaryAngle=NULL;
	//�������(X����)���Ӱ�
	CConnectPointInfo connectx,connecty;
	pDatumAngle=(CLDSLineAngle*)Ta.FromPartHandle(hBaseAngleX,CLS_LINEANGLE);
	GEPOINT datum_lenvec=pDatumAngle->pEnd->Position(false)-pDatumAngle->pStart->Position(false);
	if(pDatumAngle->pEnd==pTransit->pNode)
		datum_lenvec*=-1.0;
	if(pTransit->pHoriRodX==NULL)
	{
		logerr.Log("δ�ڱ��¿��ҵ��������Ӻ��");
		return false;	//TODO:error
	}
	GEPOINT horiz_lenvec=pTransit->pHoriRodX->pEnd->Position(false)-pTransit->pHoriRodX->pStart->Position(false);
	connectx.datumplane.vLine=datum_lenvec^horiz_lenvec;
	normalize(connectx.datumplane.vLine);
	if(fabs(connectx.datumplane.vLine*pDatumAngle->get_norm_x_wing())>fabs(connectx.datumplane.vLine*pDatumAngle->get_norm_y_wing()))
		connectx.datumplane.vSolid=pDatumAngle->get_norm_x_wing();
	else
		connectx.datumplane.vSolid=pDatumAngle->get_norm_y_wing();
	if(connectx.datumplane.vLine*connectx.datumplane.vSolid<0)
		connectx.datumplane.vLine*=-1.0;
	pPrimaryAngle=(CLDSLineAngle*)(datum_lenvec.z>0 ?pTransit->pUpRod:pTransit->pDownRod);
	GEPOINT primary_lenvec=pPrimaryAngle->pEnd->Position(false)-pPrimaryAngle->pStart->Position(false);
	connectx.bendplane.vLine=primary_lenvec^horiz_lenvec;
	normalize(connectx.bendplane.vLine);
	if(fabs(connectx.bendplane.vLine*pPrimaryAngle->get_norm_x_wing())>fabs(connectx.bendplane.vLine*pPrimaryAngle->get_norm_y_wing()))
	{
		connectx.m_iConnectWing=0;
		connectx.bendplane.vSolid=pDatumAngle->get_norm_x_wing();
	}
	else
	{
		connectx.m_iConnectWing=1;
		connectx.bendplane.vSolid=pDatumAngle->get_norm_y_wing();
	}
	if(connectx.bendplane.vLine*connectx.bendplane.vSolid<0)
		connectx.bendplane.vLine*=-1.0;
	connectx.m_hBaseNode=pTransit->pNode->handle;
	connectx.m_hBasePart=pDatumAngle->handle;
	connectx.cfgword=pDatumAngle->cfgword;
	//connectx.linkPartSet.append(pDatumAngle);
	if(pTransit->pHoriRodX)
		connectx.linkPartSet.append(pTransit->pHoriRodX);
	connectx.bendPlaneRodSet.append(pPrimaryAngle);
	connectx.m_bTransitConnect=connecty.m_bTransitConnect=true;
	CCommonLjSmartDesign design;
	if(pTransit->pHoriRodX)
	{
		pAngle=(CLDSLineAngle*)pTransit->pHoriRodX;
		connectx.m_hBendlineAngle=pTransit->pHoriRodX->handle;
		JGZJ jgzj=pAngle->GetZhunJu('X');
		if(fabs(pAngle->get_norm_x_wing()*connectx.datumplane.vSolid)<fabs(pAngle->get_norm_y_wing()*connectx.datumplane.vSolid))
			jgzj=pAngle->GetZhunJu('Y');
		pAngle->desStartPos.len_offset_dist=pAngle->desEndPos.len_offset_dist=jgzj.g+g_sysPara.HuoQuDist;
		pAngle->desStartPos.len_offset_type=pAngle->desEndPos.len_offset_type=1;
		pAngle->CalPosition(true);
	}
	//��Ʋ���(Y����)���Ӱ�
	if(pDatumAngle->handle!=hBaseAngleY)
	{
		pDatumAngle=(CLDSLineAngle*)Ta.FromPartHandle(hBaseAngleY,CLS_LINEANGLE);
		datum_lenvec=pDatumAngle->pEnd->Position(false)-pDatumAngle->pStart->Position(false);
		if(pDatumAngle->pEnd==pTransit->pNode)
			datum_lenvec*=-1.0;
	}
	horiz_lenvec=pTransit->pHoriRodY->pEnd->Position(false)-pTransit->pHoriRodY->pStart->Position(false);
	connecty.datumplane.vLine=datum_lenvec^horiz_lenvec;
	normalize(connecty.datumplane.vLine);
	if(fabs(connecty.datumplane.vLine*pDatumAngle->get_norm_x_wing())>fabs(connecty.datumplane.vLine*pDatumAngle->get_norm_y_wing()))
		connecty.datumplane.vSolid=pDatumAngle->get_norm_x_wing();
	else
		connecty.datumplane.vSolid=pDatumAngle->get_norm_y_wing();
	if(connecty.datumplane.vLine*connecty.datumplane.vSolid<0)
		connecty.datumplane.vLine*=-1.0;
	connecty.bendplane.vLine=primary_lenvec^horiz_lenvec;
	normalize(connecty.bendplane.vLine);
	if(fabs(connecty.bendplane.vLine*pPrimaryAngle->get_norm_x_wing())>fabs(connecty.bendplane.vLine*pPrimaryAngle->get_norm_y_wing()))
	{
		connecty.m_iConnectWing=0;
		connecty.bendplane.vSolid=pDatumAngle->get_norm_x_wing();
	}
	else
	{
		connecty.m_iConnectWing=1;
		connecty.bendplane.vSolid=pDatumAngle->get_norm_y_wing();
	}
	if(connecty.bendplane.vLine*connecty.bendplane.vSolid<0)
		connecty.bendplane.vLine*=-1.0;
	connecty.m_hBaseNode=pTransit->pNode->handle;
	connecty.m_hBasePart=pDatumAngle->handle;
	connecty.cfgword=pDatumAngle->cfgword;
	if(pTransit->pHoriRodY)
		connecty.linkPartSet.append(pTransit->pHoriRodY);
	connecty.bendPlaneRodSet.append(pPrimaryAngle);
	if(pTransit->pHoriRodY)
	{
		pAngle=(CLDSLineAngle*)pTransit->pHoriRodY;
		connecty.m_hBendlineAngle=pTransit->pHoriRodY->handle;
		JGZJ jgzj=pAngle->GetZhunJu('X');
		if(fabs(pAngle->get_norm_x_wing()*connecty.datumplane.vSolid)<fabs(pAngle->get_norm_y_wing()*connecty.datumplane.vSolid))
			jgzj=pAngle->GetZhunJu('Y');
		pAngle->desStartPos.len_offset_dist=pAngle->desEndPos.len_offset_dist=jgzj.g+g_sysPara.HuoQuDist;
		pAngle->desStartPos.len_offset_type=pAngle->desEndPos.len_offset_type=1;
		pAngle->CalPosition(true);
	}
	const double LOCAL_COPLANE_EPS=0.01;
	//��ӻ����漰����������߸˼�
	for(pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pAngle->pStart!=pTransit->pNode&&pAngle->pEnd!=pTransit->pNode)
			continue;
		if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
			continue;
		if(pAngle==pDatumAngle||pAngle==pPrimaryAngle||pAngle==pTransit->pHoriRodX||pAngle==pTransit->pHoriRodY)
			continue;
		if (!pDatumAngle->ModuleTogetherWith(pAngle))
			continue;
		GEPOINT lenvec=pAngle->pEnd->Position(false)-pAngle->pStart->Position(false);
		normalize(lenvec);
		if(fabs(lenvec*connectx.datumplane.vLine)<LOCAL_COPLANE_EPS)
			connectx.linkPartSet.append(pAngle);
		else if(fabs(lenvec*connectx.bendplane.vLine)<LOCAL_COPLANE_EPS)
			connectx.bendPlaneRodSet.append(pAngle);
		else if(fabs(lenvec*connecty.datumplane.vLine)<LOCAL_COPLANE_EPS)
			connecty.linkPartSet.append(pAngle);
		else if(fabs(lenvec*connecty.bendplane.vLine)<LOCAL_COPLANE_EPS)
			connecty.bendPlaneRodSet.append(pAngle);
	}
	CDisplayView *pCurView=console.GetActiveView();
	CCommonLjSmartDesign designx,designy;
	UINT idSlopeTransitTmpl=STDTEMPL_CONNPOINT_BP1;
	if(!g_sysPara.intelligentConnect.m_bOnlyDesignCurView||fabs(pCurView->ucs.axis_z*connectx.datumplane.vSolid)>=EPS_COS2)
	{
		CXhChar50 key = CUnifyModelLayer::PLATE::BuildKey(connectx.m_hBaseNode,connectx.m_hBasePart, 'X');
		CUnifyModelLayer::PLATE* pRegister = pIntelliDesigner!=NULL?pIntelliDesigner->hashPlates.GetValue(key):NULL;
		if (pRegister==NULL||pRegister->hPlate <= 0)
		{
			CNewPartsListen listen(&Ta.Parts);
			int status = CStdComModel::IntelliDesignStdComNodePlate(&connectx, &idSlopeTransitTmpl, 0);
			if (status == 0)	//���ݱ�׼�ڵ㹹���ģ�����ʧ�ܣ������û������жϣ�����ʱ���ݴ�ͳ����������ɽڵ��
				designx.DesignConnectPoint(&connectx, 0, TRUE);
			listen.EndListen();
			for (int i = 0; pIntelliDesigner != NULL && i < listen.xarrNewParts.Count; i++)
			{
				SmartPartPtr pNewPart = listen.xarrNewParts[i];
				if (pNewPart->GetClassTypeId() != CLS_PLATE)
					continue;
				if (pNewPart.pPlate->m_hBaseNode == connectx.m_hBaseNode&&pNewPart.pPlate->m_hBaseRod == connectx.m_hBasePart)
				{
					pRegister = pIntelliDesigner->hashPlates.Add(key);
					pRegister->hPlate = pNewPart->handle;
				}
			}
		}
	}
	if(!g_sysPara.intelligentConnect.m_bOnlyDesignCurView||fabs(pCurView->ucs.axis_z*connecty.datumplane.vSolid)>=EPS_COS2)
	{
		CXhChar50 key = CUnifyModelLayer::PLATE::BuildKey(connecty.m_hBaseNode,connecty.m_hBasePart, 'Y');
		CUnifyModelLayer::PLATE* pRegister = pIntelliDesigner!=NULL?pIntelliDesigner->hashPlates.GetValue(key):NULL;
		if (pRegister==NULL||pRegister->hPlate <= 0)
		{
			CLDSPartPtr xarrNewPartsPool[100];
			CNewPartsListen listen(&Ta.Parts, xarrNewPartsPool, 100);
			int status = CStdComModel::IntelliDesignStdComNodePlate(&connecty, &idSlopeTransitTmpl, 0);
			if (status == 0)	//���ݱ�׼�ڵ㹹���ģ�����ʧ�ܣ������û������жϣ�����ʱ���ݴ�ͳ����������ɽڵ��
				designy.DesignConnectPoint(&connecty, 0, TRUE);
			listen.EndListen();
			for (int i = 0; pIntelliDesigner != NULL && i < listen.xarrNewParts.Count; i++)
			{
				SmartPartPtr pNewPart = listen.xarrNewParts[i];
				if (pNewPart->GetClassTypeId() != CLS_PLATE)
					continue;
				if (pNewPart.pPlate->m_hBaseNode == connecty.m_hBaseNode&&pNewPart.pPlate->m_hBaseRod == connecty.m_hBasePart)
				{
					pRegister = pIntelliDesigner->hashPlates.Add(key);
					pRegister->hPlate = pNewPart->handle;
				}
			}
		}
	}
#endif
	return true;
}
bool CCommonLjSmartDesign::DesignSlopOrFootPoint(SLOPE_TRANSITION* pTransit)
{
#ifdef __SMART_DRAW_INC_
	//���ݽڵ��������ѡ�����ģ��
	CString temp_file(theApp.execute_path);
	char cSectType=pTransit->pUpRod!=NULL?pTransit->pUpRod->size_cSubClassType:'L';
	if (pTransit->pUpRod->GetClassTypeId()==CLS_LINEANGLE)
		cSectType='L';	//��������һЩ����ģ�͵����CLDSLineAngle.size.cSubClassType=0�������Ҫ�ڴ˽��з����� wjh-2019.8.12
	if(pTransit->pDownRod)
		temp_file+="hoof.ft";
	else if(cSectType=='L')
		temp_file+="foot_L.ft";
	else if(cSectType=='D')
		temp_file+="foot_DL.ft";	//˫���ķ�����
	else if(cSectType=='X')
		temp_file+="foot_XL_C.ft";	//������Բ������
	CFootTempl foot_templ;
	if (pTransit->pUpRod==NULL||!pTransit->pUpRod->IsAngle())
		return false;
	if (!foot_templ.FromXmlTemplFile(temp_file))
		return false;
	CalFootDesignParams((CLDSLineAngle*)pTransit->pUpRod,NULL,&foot_templ);
	UCS_STRU ucs;
	foot_templ.m_hBaseNode=pTransit->pNode->handle;	//��λ�ڵ�
	foot_templ.primaryAngle.pAngle=(CLDSLineAngle*)pTransit->pUpRod;	//��׼�Ǹ�
	foot_templ.primaryAngle.UpdateBoltInfo(pTransit->pNode);
	if(foot_templ.primaryAngle.bolt.num>=5)
		foot_templ.primaryAngle.bolt.rows=2;
	foot_templ.UpdateBoltEndSpace(foot_templ.primaryAngle);
	JGZJ jgzj=foot_templ.primaryAngle.pAngle->GetZhunJu(0);
	foot_templ.primaryAngle.g =(short)jgzj.g;
	foot_templ.primaryAngle.g1=(short)jgzj.g1;
	foot_templ.primaryAngle.g2=(short)jgzj.g2;
	if(pTransit->pDownRod)		
	{
		foot_templ.primaryAngleDown.pAngle=(CLDSLineAngle*)pTransit->pDownRod;
		foot_templ.primaryAngleDown.UpdateBoltInfo(pTransit->pNode);
		if(foot_templ.primaryAngleDown.bolt.num>=5)
			foot_templ.primaryAngleDown.bolt.rows=2;
		foot_templ.UpdateBoltEndSpace(foot_templ.primaryAngleDown);
	}
	foot_templ.GetUCS(ucs);
	foot_templ.origin.SetPosition(ucs.origin);
	foot_templ.UpdateLinkAngleOddment(foot_templ.primaryAngle);
	foot_templ.RetrieveLinkAngles();
	foot_templ.RetrieveShoePlates();
	for(CFootTempl::SHOEPLATE* pShoePlate=foot_templ.listShoePlates.GetFirst();pShoePlate;pShoePlate=foot_templ.listShoePlates.GetNext())
	{
		pShoePlate->bAutoLayBolt=TRUE;
		foot_templ.RetrieveLinkAngles(pShoePlate);
	}
	if(!foot_templ.DesignFoot())
		return false;
	if (UI::blEnableIntermediateUpdateUI)
	{
		if (foot_templ.base.pPlate)
		{
			foot_templ.base.pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength(), g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(foot_templ.base.pPlate->GetSolidPartObject());
		}
		for (CFootTempl::SHOEPLATE* pShoePlate = foot_templ.listShoePlates.GetFirst(); pShoePlate; pShoePlate = foot_templ.listShoePlates.GetNext())
		{
			if (pShoePlate->pPlate)
			{
				pShoePlate->pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength(), g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pShoePlate->pPlate->GetSolidPartObject());
				for (CLsRef* pLsRef = pShoePlate->pPlate->GetFirstLsRef(); pLsRef; pLsRef = pShoePlate->pPlate->GetNextLsRef())
				{
					CLDSBolt* pBolt = pLsRef->GetLsPtr();
					pBolt->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		for (CFootTempl::RIBPLATE* pRibPlate = foot_templ.listRibPlates.GetFirst(); pRibPlate; pRibPlate = foot_templ.listRibPlates.GetNext())
		{
			if (pRibPlate->pPlate)
			{
				pRibPlate->pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength(), g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pRibPlate->pPlate->GetSolidPartObject());
			}
		}
		g_pSolidDraw->Draw();
	}
#endif
	return true;
}
//CGroupAngleLjDesign��Ա����ʵ��
int CGroupAngleLjDesign::FILLPLATE_BOLT2_WIDEMAX=80;
int CGroupAngleLjDesign::FILLPLATE_BOLT3_WIDEMAX=125;
int CGroupAngleLjDesign::FILLPLATE_BOLT4_WIDEMAX=160;
int CGroupAngleLjDesign::FILLPLATE_BOLT5_WIDEMAX=180;
int CGroupAngleLjDesign::FILLPLATE_BOLT6_WIDEMAX=200;
int compare_coverregion(const COVER_REGION& cover1,const COVER_REGION& cover2)
{
	if(cover1.lenOffsetStart>cover2.lenOffsetStart)
		return 1;
	else if(cover1.lenOffsetStart<cover2.lenOffsetStart)
		return -1;
	else
		return 0;
}
CGroupAngleLjDesign::CGroupAngleLjDesign()
{
	m_pDatumSonAngle=m_pCoupleSonAngle=m_pCoupleSonAngle2=m_pCoupleSonAngle4=NULL;
}
CGroupAngleLjDesign::~CGroupAngleLjDesign()
{
}
int CGroupAngleLjDesign::CalFillPlateBoltNum(int wide)
{
	if(wide<=FILLPLATE_BOLT2_WIDEMAX)
		return 2;
	else if(wide<=FILLPLATE_BOLT3_WIDEMAX)
		return 3;
	else if(wide<=FILLPLATE_BOLT4_WIDEMAX)
		return 4;
	else if(wide<=FILLPLATE_BOLT5_WIDEMAX)
		return 5;
	else //if(wide<=FILLPLATE_BOLT6_WIDEMAX)
		return 6;
}
BOOL CGroupAngleLjDesign::Prepare(BOOL bEndLjPrefer/*=TRUE*/)	//�������ǰ��׼������
{
	return CCommonLjSmartDesign::Prepare(0,TRUE,TRUE);
}
void CGroupAngleLjDesign::LayoutBaseAngleBolts()
{
	int i=0,j=0;
	//���������ڻ�׼�ĵ�֫�ϵ���С��˨��
	int bolt_num=(int)ceil(m_pConnectPoint->m_nMaxRayLsNum*1.5);	//��֫��˨��
	int num_temp=0;
	for(i=0;i<m_arrRayRod.GetSize();i++)
	{
		if((m_arrRayRod[i].alfa<=0&&m_iBaseAngleWorkWing==0)||(m_arrRayRod[i].alfa>=0&&m_iBaseAngleWorkWing==1))
			num_temp+=m_arrRayRod[i].pConn->wnConnBoltN;
	}
	if(bolt_num<num_temp)	//���������ڵ������߲���˨����
		bolt_num=num_temp;
	int fillplate_boltnum=CalFillPlateBoltNum(ftoi(m_pBaseRod->GetWidth()));
	if(bolt_num<fillplate_boltnum)
		bolt_num=fillplate_boltnum;
	/*if(m_pBaseRod->pStart==m_pBaseNode)
		bolt_num=max(bolt_num,m_pBaseRod->connectStart.N);
	else if(m_pBaseRod->pEnd==m_pBaseNode)
		bolt_num=max(bolt_num,m_pBaseRod->connectEnd.N);*/
	int bolt_rows=1;
	if(bolt_num>=5)
		bolt_rows=2;
	else
		bolt_rows=1;

	if(m_jgzjBase.g2<=0||bolt_num<4)
		bolt_rows=1;
	else
		bolt_rows=2;
	
	CLDSBolt *pBolt;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,xBaseAngleBolts.wiBoltD);
	CLDSVector des_bolt_norm;
	des_bolt_norm.norm_style=1;
	des_bolt_norm.hVicePart=m_pBaseRod->handle;
	des_bolt_norm.norm_wing=m_iBaseAngleWorkWing;
	des_bolt_norm.direction=0;
	des_bolt_norm.UpdateVector(&Ta);
	double len_offset=0;
	//��׼��ϽǸ��е�����ӽǸ�����˨��λ���㣨��׼����g��)
	CLDSPoint mid_point;	//�ػ�׼����ʼ�˲�ƫ��Ϊ�ϣ��ն˲�ƫ��Ϊ��
	mid_point.datum_pos_style=2;
	mid_point.des_para.RODNODE.hRod=m_pCoupleSonAngle->handle;
	mid_point.des_para.RODNODE.hNode=m_pBaseNode->handle;
	mid_point.des_para.RODNODE.wing_offset_style=0;
	if(BaseGroupAngle()->group_style==1)	//T����ϣ���Բ�֫���׼�ӽǸ�X/Y����
		mid_point.des_para.RODNODE.offset_wing=1-m_iBaseAngleWorkWing;
	else
		mid_point.des_para.RODNODE.offset_wing=m_iBaseAngleWorkWing;
	CLDSVector desBoltNorm;
	desBoltNorm.hVicePart=m_pCoupleSonAngle->handle;
	desBoltNorm.norm_style=1;
	desBoltNorm.norm_wing=mid_point.des_para.RODNODE.offset_wing;
	DESIGN_LS_POS designBoltPos;
	designBoltPos.direction=0;
	designBoltPos.datumPoint=mid_point;
	designBoltPos.hPart=m_pCoupleSonAngle->handle;
	designBoltPos.offset_wing=(BYTE)desBoltNorm.norm_wing;
	designBoltPos.norm_offset.AddThick(-m_pCoupleSonAngle->handle,TRUE);
	designBoltPos.wing_offset_dist=designBoltPos.len_offset_dist=0;
	designBoltPos.offset_angle=0;

	if(bolt_rows==1)
	{	//����
		if(bolt_num%2==0)	//ż����˨
			len_offset=LsSpace.SingleRowSpace*0.5;
		else
			len_offset=0;
		for(i=0;i<bolt_num;i++)
		{
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->SetLayer(m_pCoupleSonAngle->layer());
			pBolt->cfgword=m_pCoupleSonAngle->cfgword;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->des_work_norm=desBoltNorm;
			pBolt->des_base_pos=designBoltPos;
			if(i%2==0)
				pBolt->des_base_pos.len_offset_dist=-len_offset;
			else
				pBolt->des_base_pos.len_offset_dist=len_offset;
			len_offset+=LsSpace.SingleRowSpace;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->AddL0Thick(m_pCoupleSonAngle->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			if(m_pCoupleSonAngle->pStart==m_pBaseNode)
				m_pCoupleSonAngle->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
			else if(m_pCoupleSonAngle->pStart==m_pBaseNode)
				m_pCoupleSonAngle->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
			else
				m_pCoupleSonAngle->AppendMidLsRef(pBolt->GetLsRef(),FALSE);
			m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
			boltset.append(pBolt);
		}
	}
	else //if(bolt_rows==2)
	{
		int g2_bolt_num=bolt_num/2;
		int g1_bolt_num=g2_bolt_num+bolt_num%2;
		double up_len_offset=0,down_len_offset=0,up_len_offset_g2=0,down_len_offset_g2=0;
		if(g1_bolt_num%2==0&&g2_bolt_num%2==0)
		{	//���������϶���ż������˨
			up_len_offset=-LsSpace.doubleRowSpace*0.25;
			down_len_offset=up_len_offset+LsSpace.doubleRowSpace;
			up_len_offset_g2=up_len_offset+0.5*LsSpace.doubleRowSpace;
			down_len_offset_g2=down_len_offset+0.5*LsSpace.doubleRowSpace;
		}
		else if(g1_bolt_num%2==1&&g2_bolt_num%2==0)
		{
			up_len_offset=0;
			down_len_offset=LsSpace.doubleRowSpace;
			up_len_offset_g2  =-0.5*LsSpace.doubleRowSpace;
			down_len_offset_g2= 0.5*LsSpace.doubleRowSpace;
		}
		else if(g1_bolt_num%2==0&&g2_bolt_num%2==1)
		{
			up_len_offset  =-0.5*LsSpace.doubleRowSpace;
			down_len_offset= 0.5*LsSpace.doubleRowSpace;
			up_len_offset_g2=0;
			down_len_offset_g2=LsSpace.doubleRowSpace;
		}
		else
		{	//�������߶�����������˨
			up_len_offset=-LsSpace.doubleRowSpace*0.25;
			down_len_offset=up_len_offset+LsSpace.doubleRowSpace;
			up_len_offset_g2=up_len_offset+0.5*LsSpace.doubleRowSpace;
			down_len_offset_g2=down_len_offset+0.5*LsSpace.doubleRowSpace;
		}
		for(i=0;i<g1_bolt_num;i++)
		{
			//g1��������˨
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetLayer(m_pCoupleSonAngle->layer());
			pBolt->cfgword=m_pCoupleSonAngle->cfgword;
			pBolt->des_work_norm=desBoltNorm;
			pBolt->des_base_pos=designBoltPos;
			if(i%2==0)
			{
				pBolt->des_base_pos.len_offset_dist=up_len_offset;
				up_len_offset-=LsSpace.doubleRowSpace;
			}
			else
			{
				pBolt->des_base_pos.len_offset_dist=down_len_offset;
				down_len_offset+=LsSpace.doubleRowSpace;
			}
			pBolt->des_base_pos.wing_offset_dist=m_jgzjBase.g1-m_jgzjBase.g;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->AddL0Thick(m_pCoupleSonAngle->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			if(m_pCoupleSonAngle->pStart==m_pBaseNode)
				m_pCoupleSonAngle->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
			else if(m_pCoupleSonAngle->pStart==m_pBaseNode)
				m_pCoupleSonAngle->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
			else
				m_pCoupleSonAngle->AppendMidLsRef(pBolt->GetLsRef(),FALSE);
			pBolt->CalGuigeAuto();
			m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
			boltset.append(pBolt);
			if(i>=g2_bolt_num)
				continue;
			//g2��������˨
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetLayer(m_pCoupleSonAngle->layer());
			pBolt->cfgword=m_pCoupleSonAngle->cfgword;
			pBolt->des_work_norm=desBoltNorm;
			pBolt->des_base_pos=designBoltPos;
			if(i%2==0)
			{
				pBolt->des_base_pos.len_offset_dist=up_len_offset_g2;
				up_len_offset_g2-=LsSpace.doubleRowSpace;
			}
			else
			{
				pBolt->des_base_pos.len_offset_dist=down_len_offset_g2;
				down_len_offset_g2+=LsSpace.doubleRowSpace;
			}
			pBolt->des_base_pos.wing_offset_dist=m_jgzjBase.g2-m_jgzjBase.g;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->AddL0Thick(m_pCoupleSonAngle->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			if(m_pCoupleSonAngle->pStart==m_pBaseNode)
				m_pCoupleSonAngle->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
			else if(m_pCoupleSonAngle->pStart==m_pBaseNode)
				m_pCoupleSonAngle->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
			else
				m_pCoupleSonAngle->AppendMidLsRef(pBolt->GetLsRef(),FALSE);
			m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
			boltset.append(pBolt);
		}
	}
	//��׼��ϽǸ��еĻ�׼�ӽǸ�����˨��λ����
	desBoltNorm.hVicePart=m_pDatumSonAngle->handle;
	desBoltNorm.norm_style=1;
	desBoltNorm.norm_wing=m_iBaseAngleWorkWing;
	designBoltPos.datumPoint.datum_pos_style=2;
	designBoltPos.datumPoint.des_para.hNode=m_pBaseNode->handle;
	designBoltPos.datumPoint.des_para.RODNODE.hRod=m_pDatumSonAngle->handle;
	designBoltPos.datumPoint.des_para.RODNODE.offset_wing=(short)m_iBaseAngleWorkWing;
	designBoltPos.hPart=m_pDatumSonAngle->handle;
	designBoltPos.offset_wing=(BYTE)desBoltNorm.norm_wing;
	designBoltPos.norm_offset.EmptyThick();
	designBoltPos.norm_offset.AddThick(-m_pDatumSonAngle->handle,TRUE);
	CBubbleSort<COVER_REGION>::BubSort(m_arrG1Cover.m_pData,m_arrG1Cover.GetSize(),compare_coverregion);
	CBubbleSort<COVER_REGION>::BubSort(m_arrG2Cover.m_pData,m_arrG2Cover.GetSize(),compare_coverregion);
	//���û�׼���ϵĲ�ն���˨
	for(i=1;i<m_arrG1Cover.GetSize();i++)
	{
		if(m_arrG1Cover[i].lenOffsetStart>m_arrG1Cover[i-1].lenOffsetEnd)	//�ɲ�ײ�һ����˨
		{
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetLayer(m_pDatumSonAngle->layer());
			pBolt->cfgword=m_pDatumSonAngle->cfgword;
			pBolt->des_work_norm=desBoltNorm;
			pBolt->des_base_pos=designBoltPos;
			if(bolt_rows==1)
				pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=0;	//g
			else
				pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=1;	//g1
			pBolt->des_base_pos.len_offset_dist=(m_arrG1Cover[i].lenOffsetStart+m_arrG1Cover[i-1].lenOffsetEnd)/2;
			pBolt->des_base_pos.wing_offset_dist=0;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->AddL0Thick(m_pDatumSonAngle->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			if(m_pDatumSonAngle->pStart==m_pBaseNode)
				m_pDatumSonAngle->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
			else if(m_pDatumSonAngle->pStart==m_pBaseNode)
				m_pDatumSonAngle->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
			else
				m_pDatumSonAngle->AppendMidLsRef(pBolt->GetLsRef(),FALSE);
			m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
			boltset.append(pBolt);
			bolt_num--;
		}
	}
	for(i=1;i<m_arrG2Cover.GetSize();i++)
	{
		if(m_arrG2Cover[i].lenOffsetStart>m_arrG2Cover[i-1].lenOffsetEnd)	//�ɲ�ײ�һ����˨
		{
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetLayer(m_pDatumSonAngle->layer());
			pBolt->cfgword=m_pDatumSonAngle->cfgword;
			pBolt->des_work_norm=desBoltNorm;
			pBolt->des_base_pos=designBoltPos;
			pBolt->des_base_pos.datumPoint.des_para.RODNODE.wing_offset_style=2;	//g1
			pBolt->des_base_pos.len_offset_dist=(m_arrG2Cover[i].lenOffsetStart+m_arrG2Cover[i-1].lenOffsetEnd)/2;
			pBolt->des_base_pos.wing_offset_dist=0;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->AddL0Thick(m_pDatumSonAngle->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			if(m_pDatumSonAngle->pStart==m_pBaseNode)
				m_pDatumSonAngle->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
			else if(m_pDatumSonAngle->pStart==m_pBaseNode)
				m_pDatumSonAngle->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
			else
				m_pDatumSonAngle->AppendMidLsRef(pBolt->GetLsRef(),FALSE);
			m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
			boltset.append(pBolt);
			bolt_num--;
		}
	}
	int up_endlj=-1,down_endlj=-1;
	for(i=0;i<m_arrRayRod.GetSize();i++)
	{
		if(m_arrRayRod[i].endLjBoltNum<=0)
			continue;
		else if(up_endlj==-1)	//��һ�����������߲ĽǸ�
			up_endlj=down_endlj=i;
		else
		{
			if(fabs(m_arrRayRod[i].alfa)>fabs(m_arrRayRod[up_endlj].alfa))
				up_endlj=i;
			if(fabs(m_arrRayRod[i].alfa)<fabs(m_arrRayRod[down_endlj].alfa))
				down_endlj=i;
		}
	}
	//���û�׼��ϽǸ��еĻ�׼�ӽǸ���������˨
	CLDSPoint up_point,down_point;	//�ػ�׼����ʼ�˲�ƫ��Ϊ�ϣ��ն˲�ƫ��Ϊ��
	int nEndLjBoltNum=endlj_boltset.GetNodeNum();
	if(bolt_rows==1)
	{	//������˨����
		if(up_endlj>=0)	//������˨������
		{
			up_point.datum_pos_style=3;
			up_point.des_para.AXIS_INTERS.hDatum1=m_pDatumSonAngle->handle;
			up_point.des_para.AXIS_INTERS.wing_offset_style1=0;
			up_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[up_endlj].Angle()->handle;
			up_point.des_para.AXIS_INTERS.wing_offset_style2=4;
			if(m_arrRayRod[up_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[up_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			down_point=up_point;
			down_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[down_endlj].Angle()->handle;
			if(m_arrRayRod[down_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[down_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');
		}
		else
		{
			up_point.datum_pos_style=2;
			up_point.des_para.RODNODE.hNode=m_pBaseNode->handle;
			up_point.des_para.RODNODE.hRod=m_pDatumSonAngle->handle;
			up_point.des_para.RODNODE.offset_wing=m_iBaseAngleWorkWing;
			up_point.des_para.RODNODE.direction=0;
			up_point.des_para.RODNODE.wing_offset_style=0;
			//up_point.des_para.RODNODE.bFaceOffset=TRUE;
			down_point=up_point;
		}
		double up_offset=0,down_offset=0;
		if(bolt_num%2==0&&up_endlj<0)		//��׼��ż����˨�����޶�������˨
			down_offset=up_offset=LsSpace.SingleRowSpace*0.5;
		else if(bolt_num%2==1&&up_endlj<0)	//��׼��������˨�����޶�������˨
			down_offset=LsSpace.SingleRowSpace;
		for(i=nEndLjBoltNum;i<bolt_num;i++)
		{
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->SetLayer(m_pBaseNode->layer());
			pBolt->cfgword=cfgword;
			boltset.append(pBolt);
			pBolt->des_work_norm=des_bolt_norm;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetGrade(xBaseAngleBolts.grade);
			pBolt->AddL0Thick(m_pDatumSonAngle->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->des_base_pos.hPart=m_pDatumSonAngle->handle;
			pBolt->des_base_pos.norm_offset.AddThick(-m_pDatumSonAngle->handle,TRUE);
			pBolt->des_base_pos.offset_wing=m_iBaseAngleWorkWing;
			if( (i%2==0&&up_endlj<0)||up_endlj>=0&&(
				(i%2==0&&m_arrRayRod[up_endlj].endLjBoltNum<=m_arrRayRod[down_endlj].endLjBoltNum)||	//ż����˨����ʼ�˲�
				(i%2==1&&m_arrRayRod[up_endlj].endLjBoltNum>m_arrRayRod[down_endlj].endLjBoltNum)))
			{
				pBolt->des_base_pos.datumPoint=up_point;
				pBolt->des_base_pos.direction=1;	//������ʼƫ��
				pBolt->des_base_pos.len_offset_dist=up_offset;
				up_offset+=LsSpace.SingleRowSpace;
			}
			else		//������˨�����ն˲�
			{
				pBolt->des_base_pos.datumPoint=down_point;
				pBolt->des_base_pos.direction=0;	//��ʼ����ƫ��
				pBolt->des_base_pos.len_offset_dist=down_offset;
				down_offset+=LsSpace.SingleRowSpace;
			}
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			if(m_pDatumSonAngle->pStart==m_pBaseNode)
				m_pDatumSonAngle->AppendStartLsRef(pBolt->GetLsRef());
			else if(m_pDatumSonAngle->pEnd==m_pBaseNode)
				m_pDatumSonAngle->AppendEndLsRef(pBolt->GetLsRef());
			else
				m_pDatumSonAngle->AppendMidLsRef(pBolt->GetLsRef());
			m_pPlate->AppendLsRef(pBolt->GetLsRef());
		}
	}
	else
	{	//˫�Ŷ�����Ĭ���Ȳ����߲���˨
		double bolt_alfa_tan=(m_jgzjBase.g2-m_jgzjBase.g1)/(LsSpace.doubleRowSpace*0.5);
		double bolt_alfa_cos=1/sqrt(1+bolt_alfa_tan*bolt_alfa_tan);
		BYTE cUpBoltLine=1,cDownBoltLine=2;	//���¶���˨�����ڻ�׼����������,1:g1��2:g2
		double up_wing_offset_g1,down_wing_offset_g1;
		double up_offset=0,down_offset=0;
		if(up_endlj>=0)	//����˨������
		{
			up_point.datum_pos_style=3;
			up_point.des_para.AXIS_INTERS.hDatum1=m_pDatumSonAngle->handle;
			double dd=m_arrRayRod[up_endlj].rayVec*ucs.axis_x;
			if(dd<=0)	//���߲ĳ�ʼ�˷�����
			{
				if(fabs(dd)>bolt_alfa_cos&&m_arrRayRod[up_endlj].endLjBoltNum==2)
				{
					up_point.des_para.AXIS_INTERS.wing_offset_style1=2;	//������б�ȳ�����˨��׳̶ȣ�g2��������˨
					up_offset=-0.5*LsSpace.doubleRowSpace;
				}
				else
					up_point.des_para.AXIS_INTERS.wing_offset_style1=1;
			}
			else		//���߲ĳ��ն˷����䣬g2��������˨
			{
				up_point.des_para.AXIS_INTERS.wing_offset_style1=2;
				cUpBoltLine=2;
			}
			up_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[up_endlj].Angle()->handle;
			up_point.des_para.AXIS_INTERS.wing_offset_style2=4;
			if(m_arrRayRod[up_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				up_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[up_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			down_point=up_point;
			dd=m_arrRayRod[down_endlj].rayVec*ucs.axis_x;
			if(dd<=0)	//���߲ĳ�ʼ�˷�����
			{
				down_point.des_para.AXIS_INTERS.wing_offset_style1=2;
				cDownBoltLine=2;
			}
			else		//���߲ĳ��ն˷����䣬g2��������˨
			{
				if(fabs(dd)>bolt_alfa_cos&&m_arrRayRod[down_endlj].endLjBoltNum==2)	//������б�ȳ�����˨��׳̶ȣ�g2��������˨
				{
					down_point.des_para.AXIS_INTERS.wing_offset_style1=2;
					cDownBoltLine=1;
					down_offset=-0.5*LsSpace.doubleRowSpace;
				}
				else
					down_point.des_para.AXIS_INTERS.wing_offset_style1=cDownBoltLine=1;
			}
			down_point.des_para.AXIS_INTERS.hDatum2=m_arrRayRod[down_endlj].Angle()->handle;
			if(m_arrRayRod[down_endlj].wing_vec*ucs.axis_x>0)	//���߳�ʼ�˲�
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 = ftoi(m_arrRayRod[down_endlj].Angle()->GetWidth())+BoltSpace(xBaseAngleBolts.wiBoltD,'A');
			else
				down_point.des_para.AXIS_INTERS.wing_offset_dist2 =-BoltSpace(xBaseAngleBolts.wiBoltD,'A');

			if(up_point.des_para.AXIS_INTERS.wing_offset_style1==1)
				up_wing_offset_g1=0;
			else	//g2����
				up_wing_offset_g1=m_jgzjBase.g1-m_jgzjBase.g2;
			if(down_point.des_para.AXIS_INTERS.wing_offset_style1==1)
				down_wing_offset_g1=0;
			else	//g2����
				down_wing_offset_g1=m_jgzjBase.g1-m_jgzjBase.g2;
		}
		else
		{
			up_point.datum_pos_style=2;
			up_point.des_para.RODNODE.hNode=m_pBaseNode->handle;
			up_point.des_para.RODNODE.hRod=m_pDatumSonAngle->handle;
			up_point.des_para.RODNODE.offset_wing=m_iBaseAngleWorkWing;
			up_point.des_para.RODNODE.direction=0;
			up_point.des_para.RODNODE.wing_offset_style=0;
			//up_point.des_para.RODNODE.bFaceOffset=TRUE;
			down_point=up_point;
			up_wing_offset_g1=down_wing_offset_g1=m_jgzjBase.g1-m_jgzjBase.g;
			if(bolt_num%2==0)		//��׼��ż����˨�����޶�������˨
			{
				cUpBoltLine=1;
				cDownBoltLine=2;
				down_offset=up_offset=LsSpace.doubleRowSpace*0.25;
			}
			else if(bolt_num%2==1)	//��׼��������˨�����޶�������˨
			{
				cUpBoltLine=1;
				cDownBoltLine=2;
				up_offset=0;
				down_offset=LsSpace.doubleRowSpace*0.5;
			}
		}
		for(i=nEndLjBoltNum;i<bolt_num;i++)
		{
			pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg=m_pBaseNode->iSeg;
			pBolt->SetLayer(m_pDatumSonAngle->layer());
			pBolt->cfgword=cfgword;
			boltset.append(pBolt);
			pBolt->des_work_norm=des_bolt_norm;
			pBolt->set_d(xBaseAngleBolts.wiBoltD);
			pBolt->SetGrade(xBaseAngleBolts.grade);
			pBolt->AddL0Thick(m_pDatumSonAngle->handle,TRUE);
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->des_base_pos.hPart=m_pDatumSonAngle->handle;
			pBolt->des_base_pos.norm_offset.AddThick(-m_pDatumSonAngle->handle,TRUE);
			pBolt->des_base_pos.offset_wing=m_iBaseAngleWorkWing;
			if( (i%2==0&&up_endlj<0)||up_endlj>=0&&(
				(i%2==0&&m_arrRayRod[up_endlj].endLjBoltNum<=m_arrRayRod[down_endlj].endLjBoltNum)||
				(i%2==1&&m_arrRayRod[up_endlj].endLjBoltNum>m_arrRayRod[down_endlj].endLjBoltNum)))
			{	//ż����˨����ʼ�˲�(���޶�������˨��ʼ���������˨�������ղ��������)��������˨����ʼ�˲�(ʼ���������˨�������ղ����)
				pBolt->des_base_pos.datumPoint=up_point;
				pBolt->des_base_pos.direction=1;	//������ʼƫ��
				if(cUpBoltLine==1)	//g1
					pBolt->des_base_pos.wing_offset_dist=up_wing_offset_g1;
				else
					pBolt->des_base_pos.wing_offset_dist=up_wing_offset_g1+m_jgzjBase.g2-m_jgzjBase.g1;
				cUpBoltLine=(cUpBoltLine+1)%2;
				pBolt->des_base_pos.len_offset_dist=up_offset;
				if( up_point.datum_pos_style==3&&up_point.des_para.AXIS_INTERS.wing_offset_style1==2&&
					i<=1&&up_endlj>=0&&fabs(m_arrRayRod[up_endlj].alfa)>90)//&&m_arrRayRod[up_endlj].alfa<270)
					up_offset-=LsSpace.doubleRowSpace*0.5;
				else
					up_offset+=LsSpace.doubleRowSpace*0.5;
			}
			else
			{	//������˨�����ն˲�
				pBolt->des_base_pos.datumPoint=down_point;
				pBolt->des_base_pos.direction=0;	//��ʼ����ƫ��
				if(cDownBoltLine==1)	//g1
					pBolt->des_base_pos.wing_offset_dist=down_wing_offset_g1;
				else
					pBolt->des_base_pos.wing_offset_dist=down_wing_offset_g1+m_jgzjBase.g2-m_jgzjBase.g1;
				cDownBoltLine=(cDownBoltLine+1)%2;
				pBolt->des_base_pos.len_offset_dist=down_offset;
				if( down_point.datum_pos_style==3&&down_point.des_para.AXIS_INTERS.wing_offset_style1==2&&
					i<=1&&down_endlj>=0&&fabs(m_arrRayRod[down_endlj].alfa)<90)//||m_arrRayRod[down_endlj].alfa>270))
					down_offset-=LsSpace.doubleRowSpace*0.5;
				else
					down_offset+=LsSpace.doubleRowSpace*0.5;
			}
			if(m_pDatumSonAngle->pStart==m_pBaseNode)
				m_pDatumSonAngle->AppendStartLsRef(pBolt->GetLsRef(),FALSE);
			else if(m_pDatumSonAngle->pEnd==m_pBaseNode)
				m_pDatumSonAngle->AppendEndLsRef(pBolt->GetLsRef(),FALSE);
			else
				m_pDatumSonAngle->AppendMidLsRef(pBolt->GetLsRef(),FALSE);
			m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
		}
	}
}
void CGroupAngleLjDesign::LayoutAngleBolts(int index,CLDSBolt *pDatumEndLjBolt/*=NULL*/)
{
	BOOL bEndLj = FALSE;
	CLDSLineAngle *pCurLineAngle=m_arrRayRod[index].Angle();
	CLDSLineAngle *pCrossLineAngle=NULL;
	CLDSPlate *pDatumPlate=m_pPlate;
	CConnectInfo *pLsInfo=m_arrRayRod[index].pConn;
	if(pCurLineAngle==NULL)
		return;	
	int iRow=0,iFirstBoltRow=0;
	if(pDatumEndLjBolt)
		iRow=pDatumEndLjBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2;
	if(m_iBaseAngleWorkWing==0&&m_arrRayRod[index].alfa<=0)
		pCrossLineAngle=m_pDatumSonAngle;
	else if(m_iBaseAngleWorkWing==0&&m_arrRayRod[index].alfa>0)
		pCrossLineAngle=m_pCoupleSonAngle;
	else if(m_iBaseAngleWorkWing==1&&m_arrRayRod[index].alfa<0)
		pCrossLineAngle=m_pCoupleSonAngle;
	else //if(m_iBaseAngleWorkWing==1&&m_arrRayRod[index].alfa>=0)
		pCrossLineAngle=m_pDatumSonAngle;
	int i=0,j=0;
	int bolt_num=m_arrRayRod[index].pConn->wnConnBoltN-m_arrRayRod[index].endLjBoltNum;
	if(m_arrRayRod[index].jgzj.g1<=0||m_arrRayRod[index].jgzj.g2<=0)
		iRow=0;
	else if(bolt_num>=4&&pDatumEndLjBolt==NULL)
		iRow=2;		//�޶����ӵ����߲�˫����˨����
	else if(iRow>0)	//�ж�����ʱ�任˫������
		iRow=3-iRow;
	CLDSVector des_bolt_norm;
	des_bolt_norm.norm_style=1;
	des_bolt_norm.hVicePart=m_arrRayRod[index].Rod()->handle;
	des_bolt_norm.norm_wing=m_arrRayRod[index].work_wing;
	if(m_arrRayRod[index].wing_norm*ucs.axis_z>0)
		des_bolt_norm.direction=0;
	else
		des_bolt_norm.direction=1;
	des_bolt_norm.UpdateVector(&Ta);
	CLDSPoint datumPoint;
	CLDSPlate *pBoltPad= NULL;
	iFirstBoltRow=iRow;
	if(bolt_num!=m_arrRayRod[index].pConn->wnConnBoltN)
	{
		datumPoint.datum_pos_style=3;
		datumPoint.des_para.AXIS_INTERS.hDatum1=m_arrRayRod[index].Rod()->handle;
		datumPoint.des_para.AXIS_INTERS.hDatum2=pCrossLineAngle->handle;
		datumPoint.des_para.AXIS_INTERS.wing_offset_style1=iFirstBoltRow;
		datumPoint.des_para.AXIS_INTERS.wing_offset_style2=4;
		//��׼�϶
		int nFirstLsJgSpace =  BoltSpace(m_arrRayRod[index].pConn->d,'A');
		if (m_arrRayRod[index].bInsideAngle&&bolt_num>0)//�е��
			nFirstLsJgSpace = BoltSpace(m_arrRayRod[index].pConn->d,'a');
		if(base_wing_vec*m_arrRayRod[index].rayVec>0)	//�����ڻ�׼��֫���
		{
			datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=
			nFirstLsJgSpace+ftoi(m_pBaseRod->GetWidth());
		}
		else
			datumPoint.des_para.AXIS_INTERS.wing_offset_dist2=-nFirstLsJgSpace;
		if(m_arrRayRod[index].bInsideAngle&&bolt_num>0)	//�ڶ����ӽǸ���Ҫ���
		{//�����߽Ǹ���ڵ���϶С��8�������߽Ǹ�����1�Ų����׼�Ǹֹ��õ���˨ʱ��
		//����˨�����Զ����ɵ�Ȧ��
			if (!(bolt_num==1&&pCrossLineAngle->GetThick()<8))
			{
				pBoltPad= (CLDSPlate*)console.AppendPart(CLS_PLATE);
				padSet.append(pBoltPad);
				pBoltPad->iSeg=m_pPlate->iSeg;
				pBoltPad->SetLayer(m_pPlate->layer());
				pBoltPad->cfgword=cfgword;
			}
		}
	}
	else
	{
		datumPoint.datum_pos_style=1;
		if(m_arrRayRod[index].Rod()->pStart==m_pBaseNode)
			datumPoint.des_para.RODEND.direction=0;	//ʼ�����ӣ���˨��ʼ���ն�λ����
		else
			datumPoint.des_para.RODEND.direction=1;	//�ն����ӣ���˨������ʼ��λ����
		datumPoint.des_para.RODEND.hRod=m_arrRayRod[index].Rod()->handle;
		datumPoint.des_para.RODEND.offset_wing=m_arrRayRod[index].work_wing;
		datumPoint.des_para.RODEND.len_offset_dist=0;
		datumPoint.des_para.RODEND.wing_offset_style=iFirstBoltRow;
		datumPoint.des_para.RODEND.bIncOddEffect=TRUE;
	}
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,m_arrRayRod[index].pConn->d);
	double len_offset=LsSpace.EndSpace;
	if(bolt_num!=m_arrRayRod[index].pConn->wnConnBoltN)
		len_offset=0;	//�ж�������˨����һ����˨�����߶�λ��������ƫ����
	for(i=0;i<bolt_num;i++)
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->iSeg=m_pBaseNode->iSeg;
		boltset.append(pBolt);
		pBolt->iSeg=m_pBaseNode->iSeg;
		pBolt->SetLayer(m_arrRayRod[index].Rod()->layer());
		pBolt->cfgword=cfgword;
		pBolt->set_d(m_arrRayRod[index].pConn->d);
		pBolt->SetGrade(m_arrRayRod[index].pConn->grade);
		pBolt->AddL0Thick(m_arrRayRod[index].Rod()->handle,TRUE);
		if(m_pPlate)
			pBolt->AddL0Thick(m_pPlate->handle,TRUE);
		if(m_arrRayRod[index].bInsideAngle&&bolt_num!=m_arrRayRod[index].pConn->wnConnBoltN)
		{
			pBolt->AddL0Thick(pCrossLineAngle->handle,TRUE);	//�ж�������˨��������Ҫ��������֫��
			double thick = pCrossLineAngle->GetThick();
			if (bolt_num==1&&thick<8)
			{
				pBolt->DianQuan.N = 1;
				pBolt->DianQuan.thick = (short)thick;
				pBolt->DianQuan.offset = thick;
			}
		}
		if(m_arrRayRod[index].bInsideAngle)	//����
			pBolt->des_base_pos.norm_offset.AddThick(-m_arrRayRod[index].Rod()->handle,TRUE);
		else if(m_pPlate)
			pBolt->des_base_pos.norm_offset.AddThick(-m_pPlate->handle,TRUE);
		pBolt->des_work_norm=des_bolt_norm;
		pBolt->des_base_pos.datumPoint=datumPoint;
		pBolt->des_base_pos.hPart=m_arrRayRod[index].Rod()->handle;
		if(m_arrRayRod[index].Rod()->pStart==m_pBaseNode)	//ʼ������
			pBolt->des_base_pos.direction=0;
		else
			pBolt->des_base_pos.direction=1;
		pBolt->des_base_pos.offset_wing=m_arrRayRod[index].work_wing;
		pBolt->des_base_pos.len_offset_dist=len_offset;
		if(iRow==0)	//������˨����
		{
			pBolt->des_base_pos.wing_offset_dist=0;
			len_offset+=LsSpace.SingleRowSpace;
		}
		else				//˫����˨����
		{
			if(iFirstBoltRow==0)
			{
				if(iRow==1)	//���������߲�
					pBolt->des_base_pos.wing_offset_dist=m_arrRayRod[index].jgzj.g1-m_arrRayRod[index].jgzj.g;
				else		//������֫���
					pBolt->des_base_pos.wing_offset_dist=m_arrRayRod[index].jgzj.g2-m_arrRayRod[index].jgzj.g;
			}
			else if(iFirstBoltRow==1)
			{
				if(iRow==1)	//���������߲�
					pBolt->des_base_pos.wing_offset_dist=0;
				else		//������֫���
					pBolt->des_base_pos.wing_offset_dist=m_arrRayRod[index].jgzj.g2-m_arrRayRod[index].jgzj.g1;
			}
			else// if(iFirstBoltRow==2)
			{
				if(iRow==1)	//���������߲�
					pBolt->des_base_pos.wing_offset_dist=m_arrRayRod[index].jgzj.g1-m_arrRayRod[index].jgzj.g2;
				else		//������֫���
					pBolt->des_base_pos.wing_offset_dist=0;
			}
			len_offset+=LsSpace.doubleRowSpace*0.5;
			iRow=3-iRow;
		}
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		if(m_arrRayRod[index].Angle()->pStart==m_pBaseNode)
			m_arrRayRod[index].Angle()->AppendStartLsRef(pBolt->GetLsRef());
		else
			m_arrRayRod[index].Angle()->AppendEndLsRef(pBolt->GetLsRef());
		if(m_pPlate)
			m_pPlate->AppendLsRef(pBolt->GetLsRef());
		if(pBoltPad)
			pBoltPad->AppendLsRef(pBolt->GetLsRef());
	}
	if(pBoltPad)
	{
		pBoltPad->Thick=ftoi(m_pBaseRod->GetThick());
		pBoltPad->cMaterial='S';
		pBoltPad->iSeg=m_pBaseRod->iSeg;
		pBoltPad->m_fNormOffset=0;
		pBoltPad->jdb_style=4;	//��˨���
		//����巨�������Ϣ
		pBoltPad->ucs.axis_z = m_pConnectPoint->datumplane.vSolid;
		pBoltPad->designInfo.iFaceType=5;	//��˨���
		pBoltPad->designInfo.m_hBaseNode=m_pBaseNode->handle;
		pBoltPad->designInfo.norm.norm_style=1;
		pBoltPad->designInfo.norm.norm_wing=m_arrRayRod[index].work_wing;
		pBoltPad->designInfo.norm.direction=0;	//����
		pBoltPad->designInfo.norm.hVicePart=m_arrRayRod[index].Rod()->handle;
		pBoltPad->cDatumAxis='X';	//��׼����Ϊ�Ǹ�ʱ�����ȼ���X����
		//�洢��׼�Ǹֺ��и��׼�Ǹ��������ʱ��
		pBoltPad->designInfo.m_hBasePart=m_arrRayRod[index].Rod()->handle;
		CDesignLjPartPara *pDesPara=pBoltPad->designInfo.partList.Add(pCrossLineAngle->handle);
		pDesPara->iFaceNo=1;
		pDesPara->angle.cur_wing_x0_y1=m_iBaseAngleWorkWing;
		pDesPara->m_nClassTypeId=CLS_LINEANGLE;
		pBoltPad->DesignSetupUcs();
		pBoltPad->CalStdProfile();
		if(pBoltPad->GetLsCount()>1)	//�����Զ���˨�������и��
		{	//���и��׼�Ǹ���Ӧ���и���
			f3dPoint face_pos,face_norm;
			face_pos=pCrossLineAngle->ucs.origin;
			if(m_iBaseAngleWorkWing==0)
				face_norm=BaseAngle()->get_norm_y_wing();	//1.Y֫����Ϊ��ʼ�и��淨��
			else
				face_norm=BaseAngle()->get_norm_x_wing();	//2.X֫����Ϊ��ʼ�и��淨��
			if(face_norm*m_arrRayRod[index].rayVec>0)	
				face_norm*=-1.0;
			else 
				face_pos-=face_norm*pCrossLineAngle->GetWidth();
			face_pos-=face_norm*g_sysPara.VertexDist;
			CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
			if(!pView->CutPlateByPlane(pBoltPad,face_pos,face_norm))
				return;
		}
		pBoltPad->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pBoltPad->GetSolidPartObject());
	}
}

int compare_endlj_rayrod(const ENDLJ_ANGLEPTR& ray1,const ENDLJ_ANGLEPTR& ray2)
{
	if(!ray1.pRod->bInsideAngle&&ray2.pRod->bInsideAngle)
		return -1;
	else if(ray1.pRod->bInsideAngle&&!ray2.pRod->bInsideAngle)
		return 1;
	else
	{
		double da1=fabs(fabs(ray1.pRod->alfa)-90);
		double da2=fabs(fabs(ray2.pRod->alfa)-90);
		if(da1>10&&da2>10&&da1>da2)
			return -1;	//���׼�Ĵ��߼нǴ���10��ʱ����н�����С�н�
		else if(da1>10&&da2>10&&da1<da2)
			return 1;
		else if(da1<da2)
			return -1;
		else if(da1>da2)
			return 1;
		else
			return 0;
	}
}
BOOL IsBoltTooNearToAngle(f3dPoint boltPos,int bolt_d,CLDSLineAngle *pAngle,int start0_end1,int wing_x0_y1)
{
	UCS_STRU ucs;
	if(wing_x0_y1==0)
		ucs.axis_y=pAngle->GetWingVecX();
	else
		ucs.axis_y=pAngle->GetWingVecY();
	double odd;
	if(start0_end1==0)
	{
		ucs.origin=pAngle->Start();
		ucs.axis_x=pAngle->End()-pAngle->Start();
		odd=pAngle->startOdd();
	}
	else
	{
		ucs.origin=pAngle->End();
		ucs.axis_x=pAngle->Start()-pAngle->End();
		odd=pAngle->endOdd();
	}
	normalize(ucs.axis_x);
	ucs.origin-=odd*ucs.axis_x;
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	project_point(boltPos,ucs.origin,ucs.axis_z);
	coord_trans(boltPos,ucs,FALSE);
	int space=BoltSpace(bolt_d,'A');
	if(boltPos.x<-space||boltPos.y<-space||boltPos.y>pAngle->GetWidth()+space)
		return FALSE;	//����Ǹ��㹻Զ
	else if(boltPos.x<0&&boltPos.y>-space&&boltPos.y<0)
	{
		double dd2=boltPos.x*boltPos.x+boltPos.y*boltPos.y;
		if(dd2>space*space)
			return FALSE;
	}
	else if(boltPos.x<0&&boltPos.y>pAngle->GetWidth()&&boltPos.y<pAngle->GetWidth()+space)
	{
		double dd2=boltPos.x*boltPos.x+(boltPos.y-pAngle->GetWidth())*(boltPos.y-pAngle->GetWidth());
		if(dd2>space*space)
			return FALSE;
	}
	return TRUE;	//����Ǹֹ��������˽��ж���˨����
}
//axis_g_n=1,ֻ����g���߸�������axis_g_n=2,ֻ����g1,g2���߸�������axis_g_n=3,����g,g1,g2���߸�������
void CGroupAngleLjDesign::CalRayCoverRegion(int iRayOrder,COVER_REGION cover_arr[],int axis_g_n/*=3*/)
{
	CLDSLineAngle *pDatumSonAngle=NULL;
	if((m_iBaseAngleWorkWing==0&&m_arrRayRod[iRayOrder].alfa<=0)	||//��׼�Ǹ�֫���
	   (m_iBaseAngleWorkWing==1&&m_arrRayRod[iRayOrder].alfa>=0))
		pDatumSonAngle=m_pDatumSonAngle;
	else
		pDatumSonAngle=m_pCoupleSonAngle;
	UCS_STRU cs;
	f3dPoint wing_vec;
	cs.origin=pDatumSonAngle->GetDatumPosBer(m_pBaseNode);
	if(pDatumSonAngle==m_pDatumSonAngle)
	{
		if(m_iBaseAngleWorkWing==0)
			wing_vec=pDatumSonAngle->GetWingVecX();
		else
			wing_vec=pDatumSonAngle->GetWingVecY();
	}
	else
	{
		if((m_iBaseAngleWorkWing==0&&BaseGroupAngle()->group_style!=1)||
		   (m_iBaseAngleWorkWing==1&&BaseGroupAngle()->group_style==1))	//T����ϻ�׼��Y֫�൱������ӽǸ�X֫
			wing_vec=pDatumSonAngle->GetWingVecX();
		else
			wing_vec=pDatumSonAngle->GetWingVecY();
	}
	cs.axis_x=ucs.axis_x;
	cs.axis_y=wing_vec;
	cs.axis_z=cs.axis_x^cs.axis_y;
	double space=BoltSpace(xBaseAngleBolts.wiBoltD,'A');
	f3dLine up_line,down_line,end_line;
	f2dLine up_line2d,down_line2d,end_line2d;
	if(m_arrRayRod[iRayOrder].Rod()->pStart==m_pBaseNode)
	{	//ʼ������
		up_line.startPt=m_arrRayRod[iRayOrder].Rod()->Start();
		up_line.endPt=m_arrRayRod[iRayOrder].Rod()->End();
		up_line.startPt-=m_arrRayRod[iRayOrder].rayVec*m_arrRayRod[iRayOrder].Rod()->startOdd();
		end_line.startPt=up_line.startPt;
		down_line=up_line;
		up_line.startPt-=m_arrRayRod[iRayOrder].wing_vec*space;
		up_line.endPt  -=m_arrRayRod[iRayOrder].wing_vec*space;
		down_line.startPt+=m_arrRayRod[iRayOrder].wing_vec*(m_arrRayRod[iRayOrder].Rod()->GetWidth()+space);
		down_line.endPt  +=m_arrRayRod[iRayOrder].wing_vec*(m_arrRayRod[iRayOrder].Rod()->GetWidth()+space);
		end_line.startPt-=m_arrRayRod[iRayOrder].rayVec*space;
		end_line.endPt   =end_line.startPt+m_arrRayRod[iRayOrder].wing_vec*m_arrRayRod[iRayOrder].Rod()->GetWidth();
	}
	else
	{	//�ն�����
		up_line.startPt=m_arrRayRod[iRayOrder].Rod()->End();
		up_line.endPt=m_arrRayRod[iRayOrder].Rod()->Start();
		up_line.startPt-=m_arrRayRod[iRayOrder].rayVec*m_arrRayRod[iRayOrder].Rod()->endOdd();
		end_line.startPt=up_line.startPt;
		down_line=up_line;
		up_line.startPt-=m_arrRayRod[iRayOrder].wing_vec*space;
		up_line.endPt  -=m_arrRayRod[iRayOrder].wing_vec*space;
		down_line.startPt+=m_arrRayRod[iRayOrder].wing_vec*(m_arrRayRod[iRayOrder].Rod()->GetWidth()+space);
		down_line.endPt  +=m_arrRayRod[iRayOrder].wing_vec*(m_arrRayRod[iRayOrder].Rod()->GetWidth()+space);
		end_line.startPt-=m_arrRayRod[iRayOrder].rayVec*space;
		end_line.endPt   =end_line.startPt+m_arrRayRod[iRayOrder].wing_vec*m_arrRayRod[iRayOrder].Rod()->GetWidth();
	}
	//����g���߸�������
	f3dPoint rayVec=m_arrRayRod[iRayOrder].rayVec;
	f3dPoint wingVec=m_arrRayRod[iRayOrder].wing_vec;
	vector_trans(rayVec,cs,FALSE);
	vector_trans(wingVec,cs,FALSE);
	rayVec.z=0;
	normalize(rayVec);
	double ctana=rayVec.x/rayVec.y;
	int i;
	for(i=0;i<axis_g_n;i++)
	{
		cover_arr[i].iRayOrder=-1;	//��Ϊ�޸���
		if(i==0&&(axis_g_n==1||axis_g_n==3))
			cs.origin=ucs.origin+wing_vec*m_jgzjBase.g;
		else if(i==0&&axis_g_n==2||i==1&&axis_g_n==3)
			cs.origin=ucs.origin+wing_vec*m_jgzjBase.g1;
		else if(i==1&&axis_g_n==2||i==2&&axis_g_n==3)
			cs.origin=ucs.origin+wing_vec*m_jgzjBase.g2;
		f3dLine ray_line=up_line;
		coord_trans(ray_line.startPt,cs,FALSE);
		coord_trans(ray_line.endPt,cs,FALSE);
		up_line2d.startPt=ray_line.startPt;
		up_line2d.endPt=ray_line.endPt;
		ray_line=down_line;
		coord_trans(ray_line.startPt,cs,FALSE);
		coord_trans(ray_line.endPt,cs,FALSE);
		down_line2d.startPt=ray_line.startPt;
		down_line2d.endPt=ray_line.endPt;
		ray_line=end_line;
		coord_trans(ray_line.startPt,cs,FALSE);
		coord_trans(ray_line.endPt,cs,FALSE);
		end_line2d.startPt=ray_line.startPt;
		end_line2d.endPt=ray_line.endPt;
		f2dArc arc;
		f2dLine axis_line2d(f2dPoint(-10000,0),f2dPoint(10000,0));
		double P1X,P1Y,P2X,P2Y;
		//����up_line����
		if(up_line2d.startPt.y*up_line2d.endPt.y<0)	//�н���
		{
			cover_arr[i].lenOffsetStart=up_line2d.startPt.x-up_line2d.startPt.y*ctana;
			cover_arr[i].lenOffsetEnd=cover_arr[i].lenOffsetStart;
			cover_arr[i].iRayOrder=iRayOrder;
		}
		//����down_line����
		if(down_line2d.startPt.y*down_line2d.endPt.y<0)	//�н���
		{
			double lenOffset=down_line2d.startPt.x-down_line2d.startPt.y*ctana;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetStart>lenOffset)
				cover_arr[i].lenOffsetStart=lenOffset;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetEnd<lenOffset)
				cover_arr[i].lenOffsetEnd=lenOffset;
			cover_arr[i].iRayOrder=iRayOrder;
		}
		//����end_line����
		if(end_line2d.startPt.y*end_line2d.endPt.y<0)	//�н���
		{
			double tana=(end_line2d.endPt.x-end_line2d.startPt.x)/(end_line2d.endPt.y-end_line2d.startPt.y);
			double lenOffset=end_line2d.startPt.x-end_line2d.startPt.y*tana;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetStart>lenOffset)
				cover_arr[i].lenOffsetStart=lenOffset;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetEnd<lenOffset)
				cover_arr[i].lenOffsetEnd=lenOffset;
			cover_arr[i].iRayOrder=iRayOrder;
		}
		//�϶�Բ�ǻ�
		arc.centre.Set(end_line2d.startPt.x+rayVec.x*space,end_line2d.startPt.y+rayVec.y*space);
		arc.angle=0.5*Pi;
		if(rayVec.x*wingVec.y-rayVec.y*wingVec.x>0)
			arc.startPt=end_line2d.startPt;
		else
			arc.startPt=up_line2d.startPt;
		//file.NewArc(arc.centre.x,arc.centre.y,space,
		int inters_num=Int2dla(axis_line2d,arc,P1X,P1Y,P2X,P2Y);
		if(inters_num>0)
		{
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetStart>P1X)
				cover_arr[i].lenOffsetStart=P1X;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetEnd<P1X)
				cover_arr[i].lenOffsetEnd=P1X;
			cover_arr[i].iRayOrder=iRayOrder;
		}
		if(inters_num>1)
		{
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetStart>P2X)
				cover_arr[i].lenOffsetStart=P2X;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetEnd<P2X)
				cover_arr[i].lenOffsetEnd=P2X;
			cover_arr[i].iRayOrder=iRayOrder;
		}
		//�¶�Բ�ǻ�
		arc.centre.Set(end_line2d.endPt.x+rayVec.x*space,end_line2d.endPt.y+rayVec.y*space);
		arc.angle=0.5*Pi;
		if(rayVec.x*wingVec.y-rayVec.y*wingVec.x>0)
			arc.startPt=down_line2d.startPt;
		else
			arc.startPt=end_line2d.endPt;
		inters_num=Int2dla(axis_line2d,arc,P1X,P1Y,P2X,P2Y);
		if(inters_num>0)
		{
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetStart>P1X)
				cover_arr[i].lenOffsetStart=P1X;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetEnd<P1X)
				cover_arr[i].lenOffsetEnd=P1X;
			cover_arr[i].iRayOrder=iRayOrder;
		}
		if(inters_num>1)
		{
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetStart>P2X)
				cover_arr[i].lenOffsetStart=P2X;
			if(cover_arr[i].iRayOrder<0||cover_arr[i].lenOffsetEnd<P2X)
				cover_arr[i].lenOffsetEnd=P2X;
			cover_arr[i].iRayOrder=iRayOrder;
		}
	}
	for(i=0;i<axis_g_n;i++)
		cover_arr[i].bDatumSonAngle=pDatumSonAngle==m_pDatumSonAngle;
}
long CGroupAngleLjDesign::DesignConnectPoint(CConnectPointInfo *pConnectPoint,BOOL bEndLjPrefer/*=TRUE*/)
{
	CLDSBolt* pBolt;
	m_pConnectPoint=pConnectPoint;
	m_arrRayRod.Empty();
	m_arrG1Cover.Empty();
	m_arrG2Cover.Empty();
	boltset.Empty();
	padSet.Empty();
	endlj_boltset.Empty();
	try{
		if(!Prepare(bEndLjPrefer))
			return FALSE;
		if(BaseGroupAngle()->group_style==2)
		{
			m_pDatumSonAngle = (CLDSLineAngle*)Ta.Parts.FromHandle(BaseGroupAngle()->son_jg_h[0], CLS_LINEANGLE);
			m_pCoupleSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(BaseGroupAngle()->son_jg_h[2],CLS_LINEANGLE);
			m_pCoupleSonAngle2=(CLDSLineAngle*)Ta.Parts.FromHandle(BaseGroupAngle()->son_jg_h[1],CLS_LINEANGLE);
			m_pCoupleSonAngle4=(CLDSLineAngle*)Ta.Parts.FromHandle(BaseGroupAngle()->son_jg_h[3],CLS_LINEANGLE);
		}
		else
		{
			BYTE ciMasterIndex = BaseGroupAngle()->ciMasterIndex;
			m_pDatumSonAngle = (CLDSLineAngle*)Ta.Parts.FromHandle(BaseGroupAngle()->son_jg_h[ciMasterIndex], CLS_LINEANGLE);
			m_pCoupleSonAngle = (CLDSLineAngle*)Ta.Parts.FromHandle(BaseGroupAngle()->son_jg_h[1-ciMasterIndex], CLS_LINEANGLE);
		}
		if(m_pDatumSonAngle==NULL||m_pCoupleSonAngle==NULL)
			return FALSE;

		double base_oddment=0;
		DESIGNODDMENT desOdd;
		//��׼�˼�ʼ�ն��������Ӱ�ʱӦ�ȸ�������ͷ�Ա�֤���߲���ײ���,���������ĺ��ٻָ�
		if(m_pBaseRod->pStart==m_pBaseNode)
		{
			base_oddment=m_pBaseRod->startOdd();
			desOdd=m_pBaseRod->desStartOdd;
			if(desOdd.m_iOddCalStyle==0)
				m_pBaseRod->SetStartOdd(500);
		}
		else if(m_pBaseRod->pEnd==m_pBaseNode)
		{
			base_oddment=m_pBaseRod->endOdd();
			desOdd=m_pBaseRod->desEndOdd;
			if(desOdd.m_iOddCalStyle==0)
				m_pBaseRod->SetEndOdd(500);
		}
		//�������߲�������˨
		int i=0;
		LINEPARTSET linkPartSet;
		CSuperSmartPtr<CLDSLinePart> pLinePart;
		CDesignLjPartPara *pLjPartPara=NULL;
		CLDSLineAngle *pEndLjRayJg=NULL;//���������߽Ǹ�
		double len_offset=0,wing_offset=0;
		CLDSPoint datumPointArr[2];
		m_arrRayRodPtr.SetSize(m_arrRayRod.GetSize());
		for(i=0;i<m_arrRayRod.GetSize();i++)
		{
			m_arrRayRodPtr[i].iAngleOrder=i;
			m_arrRayRodPtr[i].pRod=&m_arrRayRod[i];
		}
		CQuickSort<ENDLJ_ANGLEPTR>::QuickSort(m_arrRayRodPtr.m_pData,m_arrRayRod.GetSize(),compare_endlj_rayrod);
		bool blDisableLayDirectConnBolt=false;
		if(m_pBaseRod->GetClassTypeId()==CLS_GROUPLINEANGLE&&m_pBaseRod.pGroupAngle->wSectStyle==1)
			blDisableLayDirectConnBolt=(m_iBaseAngleWorkWing==m_pBaseRod.pGroupAngle->ciMasterIndex);
		for(i=0;i<m_arrRayRod.GetSize();i++)
		{
			int order=m_arrRayRodPtr[i].iAngleOrder;
			pLinePart=m_arrRayRod[order].Rod();
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//TODO:��Ҫ��ʵ֮ǰδ�����Ӹ˼�����Ƿ�BUG wjh-2017.3.24
				pLjPartPara=m_pPlate->designInfo.partList.Add(pLinePart->handle);
				//pLjPartPara=m_pPlate->designInfo.partList.append();
				if(pLinePart->pStart->handle==pConnectPoint->m_hBaseNode)
					pLjPartPara->start0_end1=0;
				else
					pLjPartPara->start0_end1=1;
				if(blDisableLayDirectConnBolt)
					m_arrRayRod[order].endLjBoltNum=0;
				else
				{
					datumPointArr[0].datum_pos_style=3;
					if(!m_arrRayRod[order].bOnSideBer)
					{	//��׼�Ǹ�֫������߽Ǹ�
						datumPointArr[0].des_para.AXIS_INTERS.hDatum1=m_pDatumSonAngle->handle;
						if(m_arrRayRod[order].bInsideAngle)	//����������ƫ�ƻ�׼��֫��+���߲�֫��
							sprintf(datumPointArr[0].des_para.AXIS_INTERS.norm_offset,"-0X%X,-0X%X",m_pDatumSonAngle->handle,m_arrRayRod[order].Angle()->handle);
						else	//����������ƫ�ƻ�׼��֫��
							sprintf(datumPointArr[0].des_para.AXIS_INTERS.norm_offset,"-0X%X",m_pDatumSonAngle->handle);
					}
					else
					{	//��׼�����߲࣬��Ի�׼��֫���(T����Ͻ��ڻ�׼��X֫����)
						datumPointArr[0].des_para.AXIS_INTERS.hDatum1=m_pCoupleSonAngle->handle;
						if(m_arrRayRod[order].bInsideAngle)	//����������ƫ�ƻ�׼��֫��+���߲�֫��
							sprintf(datumPointArr[0].des_para.AXIS_INTERS.norm_offset,"-0X%X",m_pCoupleSonAngle->handle);
						else	//����������ƫ�ƻ�׼��֫��
							sprintf(datumPointArr[0].des_para.AXIS_INTERS.norm_offset,"-0X%X,-0X%X",m_pCoupleSonAngle->handle,m_arrRayRod[order].Angle()->handle);
					}
					datumPointArr[0].des_para.AXIS_INTERS.hDatum2=m_arrRayRod[order].Angle()->handle;
					if(m_jgzjBase.g2<=0)
					{	//��׼�Ĳ�֧�ֵڶ����ģ����ж�������˨�������ڻ�׼�ı�׼��g��
						if(m_arrRayRod[order].pConn->wnConnBoltN==1)	//����˨�������������߲ı�׼��
						{
							m_arrRayRod[order].endLjBoltNum=1;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style1=0;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style2=0;
						}
						else if(m_arrRayRod[order].pConn->wnConnBoltN<=3||m_arrRayRod[order].jgzj.g2<=0)
						{	//��˨������4�������߲Ĳ�֧��˫����
							m_arrRayRod[order].endLjBoltNum=1;
							datumPointArr[0].datum_pos_style=3;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style1=0;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style2=0;
						}
						else
						{
							double dd=(m_arrRayRod[order].jgzj.g2-m_arrRayRod[order].jgzj.g1)/fabs(m_arrRayRod[order].sina);
							if(dd<m_arrRayRod[order].LsSpace.SingleRowSpace)
								m_arrRayRod[order].endLjBoltNum=1;
							else
								m_arrRayRod[order].endLjBoltNum=2;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style1=0;
							datumPointArr[1]=datumPointArr[0];
							if(m_arrRayRod[order].pConn->wnConnBoltN>3)	//���߲Ŀ϶�֧��˫���Ĳſ���ִ�е���
							{
								datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style2=1;
								datumPointArr[1].des_para.AXIS_INTERS.wing_offset_style2=2;
							}
						}
					}
					else
					{	//��׼��֧�ֵڶ�����,��ʱ��������˨���Ȳ��ڻ�׼�ĵڶ�����g2��
						if(m_arrRayRod[order].pConn->wnConnBoltN==1)	//����˨�������������߲ı�׼��
						{
							m_arrRayRod[order].endLjBoltNum=1;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style1=2;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style2=0;
						}
						else if(m_arrRayRod[order].pConn->wnConnBoltN<=3||m_arrRayRod[order].jgzj.g2<=0)
						{	//��˨������4�������߲Ĳ�֧��˫����
							m_arrRayRod[order].endLjBoltNum=1;
							double dd=(m_jgzjBase.g2-m_jgzjBase.g1)/fabs(m_arrRayRod[order].sina);
							if(dd>m_arrRayRod[order].LsSpace.SingleRowSpace)
								m_arrRayRod[order].endLjBoltNum=2;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style2=0;
							datumPointArr[1]=datumPointArr[0];
							if(m_arrRayRod[order].endLjBoltNum==1)
								datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style1=2;
							else
							{
								datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style1=1;
								//�����󣬴�ʱ����˫��������˨ͬΪ���߽���ģʽ����Ϊ�׾ඨλ
								if(dd>m_arrRayRod[order].LsSpace.SingleRowSpace+40)
									datumPointArr[1].datum_pos_style=0;	//��ʱ�������߽��㷽ʽ��ʵ�ʶ�λʱ��Ϊ���߽���+des_base_pos.len_offset
								else
									datumPointArr[1].des_para.AXIS_INTERS.wing_offset_style1=2;
							}
						}
						else
						{	//���߲�&��׼��֧��˫����
							m_arrRayRod[order].endLjBoltNum=1;
							if((m_jgzjBase.g2-m_jgzjBase.g1)/fabs(m_arrRayRod[order].sina)>
								m_arrRayRod[order].LsSpace.SingleRowSpace)	//����ǰ�������Ƚ���������˨���ڻ�׼��g2����
								m_arrRayRod[order].endLjBoltNum=2;
							else if((m_arrRayRod[order].jgzj.g2-m_arrRayRod[order].jgzj.g1)/fabs(m_arrRayRod[order].sina)>
								m_arrRayRod[order].LsSpace.SingleRowSpace&&m_arrRayRod[order].pConn->wnConnBoltN==4)
								m_arrRayRod[order].endLjBoltNum=2;	//��˨������ʱ�ɲ������ߵ��Ų���
							if((m_arrRayRod[order].jgzj.g2-m_arrRayRod[order].jgzj.g1)/fabs(m_arrRayRod[order].sina)>
								m_arrRayRod[order].LsSpace.SingleRowSpace)
								m_arrRayRod[order].endLjBoltNum=2;
							f3dPoint verify_ray_vec=-m_arrRayRod[order].rayVec;
							f3dPoint verify_axis_y=ucs.axis_y;
							if(verify_ray_vec*ucs.axis_y<0)
								verify_axis_y=-ucs.axis_y;
							else
								verify_axis_y= ucs.axis_y;
							datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style1=2;
							datumPointArr[1]=datumPointArr[0];
							if(verify_axis_y*m_arrRayRod[order].wing_vec>=0)
							{
								datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style2=1;
								datumPointArr[1].des_para.AXIS_INTERS.wing_offset_style2=2;
							}
							else
							{
								datumPointArr[0].des_para.AXIS_INTERS.wing_offset_style2=2;
								datumPointArr[1].des_para.AXIS_INTERS.wing_offset_style2=1;
							}
						}
					}
				}
				int j,subEndLjBoltNum=0;
				CLDSBolt *pDatumEndLjBolt=NULL;	//��ʱ�洢��ǰ���߲���ĩһ����������˨
				for(j=0;j<m_arrRayRod[order].endLjBoltNum;j++)
				{
					//�жϵ�ǰ��������˨�Ƿ����ǰ���Ѳ���˨�����߲ĸ���
					f3dPoint boltPos;
					if(j==1&&datumPointArr[j].datum_pos_style==0)
					{
						//boltPos=datumPointArr[0].pos+=m_arrRayRod[order].rayVec*m_arrRayRod[order].LsSpace.SingleRowSpace;
						boltPos+=m_arrRayRod[order].rayVec*m_arrRayRod[order].LsSpace.SingleRowSpace;
						datumPointArr[0].SetPosition(boltPos);
					}
					else
					{
						datumPointArr[j].UpdatePos(&Ta);
						boltPos=datumPointArr[j].Position();
					}
					if(subEndLjBoltNum>0||j==0)
					{	//subEndLjBoltNum==0ʱ��˵����һ����˨�����棬�ǵڶ�����˨�϶������棬�ʲ��ؼ��
						//��⵱ǰ���ö�������˨�Ƿ����Ѳ���˨���߲��и���
						int k,start0_end1=0;
						for(k=0;k<i;k++)
						{
							int pre_order=m_arrRayRodPtr[k].iAngleOrder;
							start0_end1=0;
							if(m_arrRayRod[pre_order].Angle()->pEnd==m_pBaseNode)
								start0_end1=1;
							if(IsBoltTooNearToAngle(boltPos,m_arrRayRod[pre_order].pConn->d,m_arrRayRod[pre_order].Angle(),
								start0_end1,m_arrRayRod[pre_order].work_wing))
								break;
						}
						if(k<i)	//����ײ������ڲ��ܲ��˶�������˨
						{
							subEndLjBoltNum++;
							continue;
						}
						//����Ѳ����߲��϶�������˨�Ƿ��뵱ǰ���߲��и��棬�и���Ӧ���ٶ�������˨��������ͷ
						f3dPoint oddVec,perp;
						SnapPerp(&perp,m_arrRayRod[order].Rod()->Start(),m_arrRayRod[order].Rod()->End(),boltPos);
						start0_end1=0;
						if(m_arrRayRod[order].Rod()->pStart==m_pBaseNode)
						{
							oddVec=perp-m_arrRayRod[order].Rod()->Start();
							double odd=-oddVec*m_arrRayRod[order].rayVec+m_arrRayRod[order].LsSpace.EndSpace;
							m_arrRayRod[order].Rod()->SetStartOdd(odd);
						}
						else
						{
							start0_end1=1;
							oddVec=perp-m_arrRayRod[order].Rod()->End();
							double odd=-oddVec*m_arrRayRod[order].rayVec+m_arrRayRod[order].LsSpace.EndSpace;
							m_arrRayRod[order].Rod()->SetEndOdd(odd);
						}
						CLDSBolt *pPreBolt=NULL;
						for(pPreBolt=endlj_boltset.GetFirst();pPreBolt;pPreBolt=endlj_boltset.GetNext())
						{
							if(IsBoltTooNearToAngle(pPreBolt->ucs.origin,pPreBolt->get_d(),m_arrRayRod[order].Angle(),
								start0_end1,m_arrRayRod[order].work_wing))
								break;
						}
						if(pPreBolt!=NULL)	//����ײ������ڲ��ܲ��˶�������˨
						{
							subEndLjBoltNum++;
							continue;
						}
					}
					//else����ײ����
					pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
					pBolt->iSeg=m_pBaseNode->iSeg;
					pBolt->SetLayer(m_pBaseNode->layer());
					pBolt->cfgword=cfgword;
					endlj_boltset.append(pBolt);
					boltset.append(pBolt);
					pBolt->set_d(m_arrRayRod[order].pConn->d);
					pBolt->SetGrade(m_arrRayRod[order].pConn->grade);
					if(j==1&&datumPointArr[j].datum_pos_style==0)
					{
						//�������߽������ⲻ֧��norm_offset�����ٸ���Ͳ������ɶȿ��ǣ������轫����ƫ�ƣ��Ƶ�des_base_pos��
						pBolt->des_base_pos.norm_offset.key_str.Copy(datumPointArr[0].des_para.AXIS_INTERS.norm_offset);
						strcpy(datumPointArr[0].des_para.AXIS_INTERS.norm_offset,"");
						pBolt->des_base_pos.datumPoint=datumPointArr[0];
						pBolt->des_base_pos.len_offset_dist=m_arrRayRod[order].LsSpace.SingleRowSpace;
					}
					else
					{
						//�������߽������ⲻ֧��norm_offset�����ٸ���Ͳ������ɶȿ��ǣ������轫����ƫ�ƣ��Ƶ�des_base_pos��
						pBolt->des_base_pos.norm_offset.key_str.Copy(datumPointArr[j].des_para.AXIS_INTERS.norm_offset);
						strcpy(datumPointArr[j].des_para.AXIS_INTERS.norm_offset,"");
						pBolt->des_base_pos.datumPoint=datumPointArr[j];
					}
					pBolt->des_base_pos.hPart=m_arrRayRod[order].Angle()->handle;
					pBolt->des_base_pos.direction=(BYTE)pLjPartPara->start0_end1;
					pBolt->des_base_pos.offset_wing=m_arrRayRod[order].work_wing;
					pBolt->des_work_norm.hVicePart=pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
					pBolt->des_work_norm.direction=0;
					pBolt->des_work_norm.norm_style=1;
					if(pBolt->des_work_norm.hVicePart==m_pCoupleSonAngle->handle&&BaseGroupAngle()->group_style==1)
						pBolt->des_work_norm.norm_wing=(short)(1-m_iBaseAngleWorkWing);	//T����ϽǸ��е�����ӽǸ�Ϊ��׼
					else
						pBolt->des_work_norm.norm_wing=(short)m_iBaseAngleWorkWing;
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					m_pPlate->AppendLsRef(pBolt->GetLsRef(),FALSE);
					if(pLjPartPara->start0_end1==0)
						m_arrRayRod[order].Angle()->AppendStartLsRef(pBolt->GetLsRef());
					else
						m_arrRayRod[order].Angle()->AppendEndLsRef(pBolt->GetLsRef());
					pBolt->AddL0Thick(m_arrRayRod[order].Angle()->handle,TRUE);
					pBolt->AddL0Thick(m_pPlate->handle,TRUE);
					pBolt->AddL0Thick(pBolt->des_work_norm.hVicePart,TRUE);
					if(pBolt->des_work_norm.hVicePart==m_pDatumSonAngle->handle)
					{
						if(m_pBaseRod->pStart==m_pBaseNode)
							m_pDatumSonAngle->AppendStartLsRef(pBolt->GetLsRef());
						else
							m_pDatumSonAngle->AppendEndLsRef(pBolt->GetLsRef());
					}
					else
					{
						if(m_pBaseRod->pStart==m_pBaseNode)
							m_pCoupleSonAngle->AppendStartLsRef(pBolt->GetLsRef());
						else
							m_pCoupleSonAngle->AppendEndLsRef(pBolt->GetLsRef());
					}
					pBolt->CalGuigeAuto();
					if(	pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2==0)
						pBolt->dwRayNo=1;
					else if(pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.wing_offset_style2==1)
						pBolt->dwRayNo=2;
					else
						pBolt->dwRayNo=3;
					pDatumEndLjBolt=pBolt;
				}
				m_arrRayRod[order].endLjBoltNum-=subEndLjBoltNum;
				//�趨��ǰ���ߵ�����ͷ�������
				if(pLinePart->pStart->handle==pConnectPoint->m_hBaseNode)
				{
					if(m_arrRayRod[order].endLjBoltNum==0)
					{	//�޶�����
						m_arrRayRod[order].Angle()->desStartOdd.m_iOddCalStyle=0;
						if(!m_arrRayRod[order].bOnSideBer)
							pLinePart->desStartOdd.m_hRefPart1=m_pDatumSonAngle->handle;
						else
							pLinePart->desStartOdd.m_hRefPart1=m_pCoupleSonAngle->handle;
						if(m_arrRayRod.GetSize()>1)
						{
							if(i==0)
								pLinePart->desStartOdd.m_hRefPart2=m_arrRayRod[i+1].Rod()->handle;
							else
							{
								if(i+1>=m_arrRayRod.GetSize())
									pLinePart->desStartOdd.m_hRefPart2=m_arrRayRod[i-1].Rod()->handle;
								//else if(m_arrRayRod[order].alfa-m_arrRayRod[i-1].alfa<m_arrRayRod[i+1].alfa-m_arrRayRod[order].alfa)
								//	pLinePart->desStartOdd.m_hRefPart2=m_arrRayRod[i-1].Rod()->handle;
							}
						}
						pLinePart->desStartOdd.m_fCollideDist = g_sysPara.VertexDist;
						pLinePart->CalStartOddment();
					}
					else
						m_arrRayRod[order].Angle()->desStartOdd.m_iOddCalStyle=1;
				}
				else
				{
					if(m_arrRayRod[order].endLjBoltNum==0)
					{
						m_arrRayRod[order].Angle()->desEndOdd.m_iOddCalStyle=0;
						if(!m_arrRayRod[order].bOnSideBer)
							pLinePart->desEndOdd.m_hRefPart1=m_pDatumSonAngle->handle;
						else
							pLinePart->desEndOdd.m_hRefPart1=m_pCoupleSonAngle->handle;
						pLinePart->desEndOdd.m_fCollideDist = g_sysPara.VertexDist;
						if(i==0&&m_arrRayRod.GetSize()>1)
							pLinePart->desEndOdd.m_hRefPart2=m_arrRayRod[i+1].Rod()->handle;
						else if(i>0)
							pLinePart->desEndOdd.m_hRefPart2=m_arrRayRod[i-1].Rod()->handle;
						pLinePart->CalEndOddment();
					}
					else
						m_arrRayRod[order].Angle()->desEndOdd.m_iOddCalStyle=1;
				}
				//�趨��ǰ���߲ĵĶ����λ�ò���
				if(m_arrRayRod[order].endLjBoltNum>0&&
					(m_arrRayRod[order].bInsideAngle||m_pBaseRod.GroupAnglePointer()->group_style==2))
				{	//�ж�������Ϊ��������ϽǸ�Ϊʮ�����ʱ
					if(m_arrRayRod[order].work_wing==0)
						m_arrRayRod[order].AnglePos()->wing_x_offset.offsetDist=-0.5*m_pPlate->GetThick()-m_pBaseRod->GetThick();
					else
						m_arrRayRod[order].AnglePos()->wing_y_offset.offsetDist=-0.5*m_pPlate->GetThick()-m_pBaseRod->GetThick();
				}
				else
				{	//�޶�����
					if(m_arrRayRod[order].work_wing==0)
						m_arrRayRod[order].AnglePos()->wing_x_offset.offsetDist=-0.5*m_pPlate->GetThick();
					else
						m_arrRayRod[order].AnglePos()->wing_y_offset.offsetDist=-0.5*m_pPlate->GetThick();
				}
				m_arrRayRod[order].Rod()->CalPosition();
				if(pLjPartPara)
				{
					pLjPartPara->angle.cur_wing_x0_y1=m_arrRayRod[order].work_wing;
					pLjPartPara->hPart=pLinePart->handle;
					pLjPartPara->end_space=m_arrRayRod[order].LsSpace.EndSpace;
					pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
					pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
					//pLjPartPara->ber_space=m_arrRayRod[order].jgzj.g;
					//pLjPartPara->wing_space=pLinePart->GetWidth()-m_arrRayRod[order].jgzj.g;
					pLjPartPara->iFaceNo=1;
					pLjPartPara->m_nClassTypeId=pLinePart->GetClassTypeId();
					if(bEndLjPrefer&&(i==m_iFirstEndLjAngle||i==m_iSecEndLjAngle))
						pLjPartPara->angle.bEndLjJg = TRUE;//�Գ�ʱ��Ҫ����Ϣ
				}
				//�������߽Ǹ���˨
				LayoutAngleBolts(order,pDatumEndLjBolt);
				if(pLinePart->pStart==m_pBaseNode&&pLinePart->desStartOdd.m_iOddCalStyle==1)
					pLinePart->CalStartOddment();	//ʼ��Ϊ��˨ȷ������ͷ
				else if(pLinePart->pEnd==m_pBaseNode&&pLinePart->desEndOdd.m_iOddCalStyle==1)
					pLinePart->CalEndOddment();
				COVER_REGION cover_arr[2];
				int axis_g_n=1;
				if(m_jgzjBase.g2>0)
					axis_g_n=2;
				CalRayCoverRegion(order,cover_arr,axis_g_n);
				if(cover_arr[0].iRayOrder>=0)
					m_arrG1Cover.append(cover_arr[0]);
				if(cover_arr[1].iRayOrder>=0)
					m_arrG2Cover.append(cover_arr[1]);
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//�������߸ֹܲ��,���ݸֹܹ��ӹ����в��Ҹֹܲ��
				int start0_end1=0;
				TUBEJOINT *pCurJoint=NULL;
				CTubeEndPosPara *pCurDesPara=NULL;
				if(pLinePart->pStart->handle==pConnectPoint->m_hBaseNode)
				{
					start0_end1=0;
					pCurDesPara=&pLinePart.LineTubePointer()->desStartPos;
					pCurJoint=&pLinePart.LineTubePointer()->m_tJointStart;
				}
				else
				{
					start0_end1=1;
					pCurDesPara=&pLinePart.LineTubePointer()->desEndPos;
					pCurJoint=&pLinePart.LineTubePointer()->m_tJointEnd;
				}
				CLDSParamPlate *pParamPlate=NULL;
				pParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pCurJoint->hLinkObj,CLS_PARAMPLATE);
				CLDSStdPartModel stdPartModel;
				//����Ԥ�������ȼ����Ҷ�Ӧ��׼�����Ƿ�����Ӧ�ı�׼����
				int i=0,j=0,iTubeEndLjStyle=0;
				BOOL bFindStdModel=FALSE;
				double tube_t=pLinePart->GetThick();
				double tube_d=pLinePart.LineTubePointer()->GetDiameter();
				PARAM_PLATE_TYPE iFirstType;
				for(i=0;i<g_sysPara.m_arrInsertPlateType.GetSize();i++)
				{
					PARAM_PLATE_TYPE iPlateType;
					if(g_sysPara.m_arrInsertPlateType[i].CompareNoCase("C")==0)
					{
						iTubeEndLjStyle=4;
						iPlateType=TYPE_ROLLEND;
					}
					else if(g_sysPara.m_arrInsertPlateType[i].CompareNoCase("U")==0)
					{
						iTubeEndLjStyle=5;
						iPlateType=TYPE_UEND;
					}
					else if(g_sysPara.m_arrInsertPlateType[i].CompareNoCase("X")==0)
					{
						iTubeEndLjStyle=6;
						iPlateType=TYPE_XEND;
					}
					else 
						continue;
					if (i==0)
						iFirstType = iPlateType;
					for(j=0;j<10;j++)
					{
						if(pParamPlate==NULL&&globalLibrary.GetStdPartModel(
							tube_d,tube_t,pLinePart->cMaterial,j,iPlateType,&stdPartModel))
						{
							bFindStdModel=TRUE;
							break;
						}
					}
					if(bFindStdModel)
						break;
				}
				int nLsNum=0,nLsD=0,nLsRow;
				if(bFindStdModel)
				{	//���ɱ�׼����
					pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
					pParamPlate->m_bStdPart=TRUE;	//��׼����
					pParamPlate->m_uStatMatCoef=1;
					pParamPlate->iSeg=pLinePart->iSeg;
					pParamPlate->m_hPartWeldParent = pLinePart->handle;
					pParamPlate->SetLayer(pLinePart->layer()); //������������ͼ����
					pParamPlate->cfgword=pLinePart->cfgword;
					pParamPlate->cMaterial = pLinePart->cMaterial;
					pParamPlate->SetPartNo(stdPartModel.param.insert_plate_param.codeName);
					pParamPlate->m_iParamType=stdPartModel.m_iStdPartType;
					//���ù���������ڸֹܻ�׼�����ת�Ƕ� 
					//����ĸ'X'��ʾ��ת�Ƕȣ�D���ע������������ wht 10-01-21
					pParamPlate->SetDesignItemValue('X',180,"��������Ըֹܻ�׼����ת�Ƕ�");
				}
				else
				{
					//������ͨ��
					pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
					pParamPlate->m_bStdPart=FALSE;
					pParamPlate->m_uStatMatCoef=1;
					pParamPlate->iSeg=pLinePart->iSeg;
					pParamPlate->m_hPartWeldParent = pLinePart->handle;
					pParamPlate->SetLayer(pLinePart->layer()); //������������ͼ����
					pParamPlate->cfgword=pLinePart->cfgword;
					pParamPlate->cMaterial = pLinePart->cMaterial;
					pParamPlate->SetPartNo(CXhChar16("X%.0f%.0fH1",tube_d,tube_t));
					sprintf(stdPartModel.param.insert_plate_param.codeName,"X%.0f%.0fH1",tube_d,tube_t);
					stdPartModel.m_iStdPartType = iFirstType;
					stdPartModel.param.insert_plate_param.iBoltRect = 1;
					if(!start0_end1)
					{
						nLsNum = pLinePart.LineTubePointer()->connectStart.wnConnBoltN;
						nLsD = pLinePart.LineTubePointer()->connectStart.d;
						nLsRow = pLinePart.LineTubePointer()->connectStart.rows;
						stdPartModel.param.insert_plate_param.ls_param[0].iRow = nLsRow;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltNum = nLsNum;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltD = pLinePart.LineTubePointer()->connectStart.d;
					}
					else
					{
						nLsNum = pLinePart.LineTubePointer()->connectEnd.wnConnBoltN;
						nLsD = pLinePart.LineTubePointer()->connectEnd.d;
						nLsRow = pLinePart.LineTubePointer()->connectEnd.rows;
						stdPartModel.param.insert_plate_param.ls_param[0].iRow = pLinePart.LineTubePointer()->connectEnd.rows;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltNum = pLinePart.LineTubePointer()->connectEnd.wnConnBoltN;
						stdPartModel.param.insert_plate_param.ls_param[0].iBoltD = pLinePart.LineTubePointer()->connectEnd.d;
					}
					//��˨
					LSSPACE_STRU LsSpace;
					GetLsSpace(LsSpace,nLsD);
					pParamPlate->m_iParamType=iFirstType;
					pParamPlate->thick=8;
					//���ù���������ڸֹܻ�׼�����ת�Ƕ� 
					//����ĸ'X'��ʾ��ת�Ƕȣ�D���ע������������ wht 10-01-21
					pParamPlate->SetDesignItemValue('X',180,"��������Ըֹܻ�׼����ת�Ƕ�");
					double A=0,N=0,M=0,R=0,L=0,H=0,S=0,W=0;
					double fVertSpace = 0;
					if (nLsRow>1)
						fVertSpace=(tube_d-nLsD)/(nLsRow-1);
					double fHorzSpace = nLsD+30;
					int nColunm = (nLsNum+nLsRow-1)/nLsRow;
					A=10;
					R=15;
					L=((nColunm-1)*fHorzSpace+2*LsSpace.PlateEdgeSpace)*2;
					M=L/2-10;
					W=40;
					if(iFirstType==TYPE_UEND)
						W=m_pPlate->GetThick();
					H=tube_d+60;
					if (tube_d<190)
						S=N=30;
					else
						S=N=40;
					if(iFirstType==TYPE_XEND)
					{
						N=0.5*(H-60);
						S=L;
						W = 8;
						sprintf(stdPartModel.param.insert_plate_param.weldPlatePartNo,"XL%.0f%.0fH1",tube_d,tube_t);
						sprintf(stdPartModel.param.insert_plate_param.wrapPlatePartNo,"XL%.0f%.0fH1",tube_d,tube_t);
					}
					pParamPlate->SetDesignItemValue('A',A);
					pParamPlate->SetDesignItemValue('N',N);
					pParamPlate->SetDesignItemValue('M',M);
					pParamPlate->SetDesignItemValue('R',R);
					pParamPlate->SetDesignItemValue('L',L);
					pParamPlate->SetDesignItemValue('H',H);
					pParamPlate->SetDesignItemValue('S',S);
					pParamPlate->SetDesignItemValue('W',W);
					pParamPlate->SetDesignItemValue('T',pParamPlate->thick);
					
					stdPartModel.param.insert_plate_param.ls_param[0].fHorzSpace = nLsD+30;
					stdPartModel.param.insert_plate_param.ls_param[0].fVertSpace = fVertSpace;
					stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosX = M+LsSpace.PlateEdgeSpace+10;
					stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY = -tube_d*0.5+nLsD*0.5;
					if(nLsRow==1&&pParamPlate->m_iParamType!=TYPE_XEND)
						stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY = 0;
				}
				if(pParamPlate)
				{
					//���ݲ������ȵ����ְ���
					if(m_pPlate->GetThick()<pParamPlate->GetThick())
						m_pPlate->Thick=ftoi(pParamPlate->GetThick());
					//�����߸˼���ӵ��ְ����Ӹ˼��б���
					pLjPartPara=m_pPlate->designInfo.partList.Add(pLinePart->handle);
					pLjPartPara->iFaceNo=1;
					pLjPartPara->m_nClassTypeId=pLinePart->GetClassTypeId();
					pLjPartPara->start0_end1=start0_end1;
					//�Զ������ֹܻ���λ��λ��
					if(pParamPlate->m_iParamType==TYPE_ROLLEND)
					{
						double fWorkPlaneRotAngle=0;	//����ƽ����ת�Ƕ�
						double offset_dist=-0.5*(pParamPlate->GetThick()+m_pPlate->GetThick());
						double X=0;	//��X��ʾ��ת�Ƕ� D���׼������ز���ͬ�� ��ʱ��˴��� wht 10-01-21
						if(pParamPlate->m_bStdPart)
							pParamPlate->GetDesignItemValue('X',&fWorkPlaneRotAngle);
						else
							pParamPlate->GetDesignItemValue('D',&fWorkPlaneRotAngle);
						if(fabs(fabs(fWorkPlaneRotAngle)-180)<EPS)	//����߷����෴���巨�߷����෴
							offset_dist*=-1.0;	
						if(g_sysPara.nAutoAdjustTubePos==2)	//�ֹ�ƫ��
							pCurDesPara->fNormOffset+=offset_dist;
						else if(g_sysPara.nAutoAdjustTubePos==1) //���ƫ��
							pParamPlate->m_fNormOffset-=offset_dist;
					}
					pLinePart.LineTubePointer()->ClearFlag();
					pLinePart.LineTubePointer()->CalPosition();	//����ֹ�λ��
					pLinePart.LineTubePointer()->CalWorkPlaneNorm();
					if(pParamPlate->m_bStdPart)
					{
						pCurJoint->type=iTubeEndLjStyle;	//���Ͳ��
						pCurJoint->hLinkObj=pParamPlate->handle;
						pParamPlate->DesignStdPartUcs();
					}
					if(!pParamPlate->m_bStdPart)//�Ǳ�׼���
					{
						if(start0_end1==0)
						{
							pLinePart.LineTubePointer()->m_tJointStart.hLinkObj=pParamPlate->handle;
							pLinePart.LineTubePointer()->m_tJointStart.type = iFirstType;
							pLinePart.LineTubePointer()->DesignStartJoint();
						}
						else if(start0_end1==1)
						{
							pLinePart.LineTubePointer()->DesignEndJointUCS(pParamPlate);
						}
					}
					if(pLinePart->pStart->handle==pConnectPoint->m_hBaseNode)
					{
						pLinePart.LineTubePointer()->desStartOdd.m_hRefPart1=m_pBaseRod->handle;
						//��ײ��϶��ֵ
						pLinePart.LineTubePointer()->desStartOdd.m_fCollideDist = g_sysPara.VertexDist;
						pLinePart.LineTubePointer()->CalStartOddment();
					}
					else
					{
						pLinePart.LineTubePointer()->desEndOdd.m_hRefPart1=m_pBaseRod->handle;	
						pLinePart.LineTubePointer()->desEndOdd.m_fCollideDist = g_sysPara.VertexDist;
						pLinePart.LineTubePointer()->CalEndOddment();
					}
					
					pParamPlate->DesignPlate();
					pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
					g_pSolidDraw->NewSolidPart(pParamPlate->GetSolidPartObject());
					
					if(pParamPlate->m_iParamType==TYPE_XEND)
					{
						if(pParamPlate->m_bStdPart)
							LayoutInsertPlateBoltAndAttachPlate(pParamPlate,stdPartModel.param.insert_plate_param.weldPlatePartNo,
							stdPartModel.param.insert_plate_param.wrapPlatePartNo,FALSE);
						else
						{	
							//����ʮ�ֲ��
							double A,N,M,R,L,H,S,W;
							pParamPlate->GetDesignItemValue('A',&A);
							pParamPlate->GetDesignItemValue('N',&N);
							pParamPlate->GetDesignItemValue('M',&M);
							pParamPlate->GetDesignItemValue('R',&R);
							pParamPlate->GetDesignItemValue('L',&L);
							pParamPlate->GetDesignItemValue('H',&H);
							pParamPlate->GetDesignItemValue('S',&S);
							pParamPlate->GetDesignItemValue('W',&W);
							double x=A+M;
							double y=R;
							double z=0.5*pParamPlate->GetThick();
							//
							stdPartModel.param.insert_plate_param.L = L;
							stdPartModel.param.insert_plate_param.iBoltRect = 2;
							stdPartModel.param.insert_plate_param.ls_param[0].iBoltNum = nLsNum;
							stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosX = -A-10;
							stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY -= 2*R;
							LSSPACE_STRU LsSpace;
							GetLsSpace(LsSpace,nLsD);
							double disTowPlate = 10;
							S = ((nLsNum-1)*stdPartModel.param.insert_plate_param.ls_param[0].fHorzSpace+LsSpace.EndSpace*2)*2+disTowPlate;
							N = 0.5*(H-2*R);
							stdPartModel.param.insert_plate_param.ls_param[1].iBoltNum = nLsNum;
							stdPartModel.param.insert_plate_param.ls_param[1].fBoltPosX = stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosX
								+stdPartModel.param.insert_plate_param.ls_param[0].fHorzSpace*(nLsNum-1)
								+LsSpace.EndSpace*2+disTowPlate;
							stdPartModel.param.insert_plate_param.ls_param[1].fBoltPosY = stdPartModel.param.insert_plate_param.ls_param[0].fBoltPosY;
							stdPartModel.param.insert_plate_param.ls_param[1].fHorzSpace = nLsD+30;
							stdPartModel.param.insert_plate_param.ls_param[1].fVertSpace = stdPartModel.param.insert_plate_param.ls_param[0].fVertSpace;
							stdPartModel.CreateBoltInPlate(pParamPlate);
							
							CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z����������  Z����������
							CString sPartNo;		
							int i;
							for(i=0;i<2;i++)
							{
								CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
								pVertPlate->iSeg=pParamPlate->iSeg;
								pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
								pVertPlate->cMaterial = pParamPlate->cMaterial;
								pVertPlate->SetLayer(pParamPlate->layer()); //���������ͼ����
								pVertPlate->cfgword=pParamPlate->cfgword;
								pVertPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
								pVertPlate->Thick=ftoi(pParamPlate->GetThick());
								pVertPlate->SetPartNo(stdPartModel.param.insert_plate_param.weldPlatePartNo);
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
										if(pLsRef->start1_end2_mid0==41)//������˨��Z������������
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
										if(pLsRef->start1_end2_mid0==31)//������˨��Z������������
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
							for(i=0;i<8;i++)
							{
								CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
								pPlate->iSeg=pParamPlate->iSeg;
								pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
								pPlate->cMaterial = pParamPlate->cMaterial;
								pPlate->SetLayer(pParamPlate->layer()); //���������ͼ����
								pPlate->cfgword=pParamPlate->cfgword;
								//����Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
								//pPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
								pPlate->Thick=ftoi(W);
								pPlate->SetPartNo(stdPartModel.param.insert_plate_param.weldPlatePartNo);
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
										if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
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
										if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
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
										if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
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
										if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
											(*pLsRef)->des_base_pos.norm_offset.AddThick(-pPlate->handle,TRUE);
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
										if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
										{
											pPlate->AppendLsRef(*pLsRef);
											(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
										}
									}
								}
								for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
								{	
									(*pLsRef)->SetModified();
									(*pLsRef)->correct_worknorm();
									(*pLsRef)->correct_pos();
									(*pLsRef)->CalGuigeAuto();
									(*pLsRef)->Create3dSolidModel();
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
					else 
						stdPartModel.CreateBoltInPlate(pParamPlate);
					//������˨λ�ò���ʾ��˨
					double L=0;
					pParamPlate->GetDesignItemValue('L',&L);
					for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
					{
						CLDSBolt *pBolt=pLsRef->GetLsPtr();
						if(fabs(pBolt->get_norm()*pParamPlate->ucs.axis_z)<EPS_COS2)
							continue;
						if(pParamPlate->m_iParamType==TYPE_XEND)
						{
							f3dPoint ls_pos=pBolt->ucs.origin;
							coord_trans(ls_pos,pParamPlate->ucs,FALSE);
							if(ls_pos.x<L)
								continue;
						}
						m_pPlate->AppendLsRef(*pLsRef);
						if(pBolt->des_work_norm.direction==1)	//����(��巨�߷����෴)
							pBolt->des_base_pos.norm_offset.AddThick(-m_pPlate->handle,TRUE);
						pBolt->AddL0Thick(m_pPlate->handle,TRUE);
						pBolt->cfgword=pLinePart->cfgword;
						pBolt->correct_worknorm();
						pBolt->correct_pos();
						pBolt->CalGuigeAuto();
						pBolt->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
					}
					pParamPlate->ResetSonBoltRayNo();		//���ò������˨���ߺ�
					pParamPlate->GetLsRefList()->ResetBoltRefRayNo(pParamPlate);	//���ò������˨�����ߺ�
				}
			}
			m_arrRayRod[order].Angle()->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(m_arrRayRod[order].Angle()->GetSolidPartObject());
			pLinePart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pLinePart->GetSolidPartObject());
		}
		//�����ڵ��������߽Ǹֺ��������˨λ�ã���Ϊ��˨λ�������������׼���󽻵��������
		for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		{
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		for(CLDSPlate *pBoltPad=(CLDSPlate*)padSet.GetFirst();pBoltPad;pBoltPad=(CLDSPlate*)padSet.GetNext())
		{
			pBoltPad->DesignSetupUcs();
			pBoltPad->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBoltPad->GetSolidPartObject());
		}
		//���û�׼����������˨
		LayoutBaseAngleBolts();
		//�ָ���׼�˼�ʼ�ն�����ͷ��Ϣ
		if(m_pBaseRod->pStart==m_pBaseNode)
		{
			m_pBaseRod->desStartOdd.m_iOddCalStyle=1;
			m_pBaseRod->CalStartOddment();
			m_pBaseRod->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(m_pBaseRod->GetSolidPartObject());
			//m_pBaseRod->SetStartOdd(base_oddment);
			//m_pBaseRod->desStartOdd=desOdd;
		}
		else if(m_pBaseRod->pEnd==m_pBaseNode)
		{
			m_pBaseRod->desEndOdd.m_iOddCalStyle=1;
			m_pBaseRod->CalEndOddment();
			m_pBaseRod->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(m_pBaseRod->GetSolidPartObject());
			//m_pBaseRod->desEndOdd=desOdd;
			//m_pBaseRod->SetEndOdd(base_oddment);
		}
		if(m_pBaseRod->GetClassTypeId()==CLS_LINETUBE)
		{	//��׼����Ϊ�ֹ�
			m_pPlate->m_bWeldPart=TRUE;
			m_pPlate->m_hPartWeldParent=m_pBaseRod->handle;
			m_pPlate->designInfo.iProfileStyle0123=1;
			m_pPlate->m_fNormOffset = -0.5*m_pPlate->GetThick();
			m_pPlate->ucs.axis_x.Set(0,0,0);
			m_pPlate->DesignSetupUcs();
		}
		DesignBendPlane(pConnectPoint);
		m_pPlate->DesignPlate();
		m_pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(m_pPlate->GetSolidPartObject());
		for(pBolt=boltset.GetFirst();pBolt;pBolt=boltset.GetNext())
		{
			if(m_pCoupleSonAngle2&&m_pCoupleSonAngle2->IsLsInWing(pBolt))
			{
				m_pCoupleSonAngle2->AppendMidLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(m_pCoupleSonAngle2->handle,TRUE);
			}
			if(m_pCoupleSonAngle4&&m_pCoupleSonAngle4->IsLsInWing(pBolt))
			{
				m_pCoupleSonAngle4->AppendMidLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(m_pCoupleSonAngle4->handle,TRUE);
			}
			//��֪Ϊʲô��ǰ����ִ������ʱ����ϵδ����,��ʱ����һ�� wjh-2020.1.15
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	if(m_pPlate)
		return m_pPlate->handle;
	else
		return TRUE;	//�ް�����
}
#endif