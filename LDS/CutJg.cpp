//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "Tower.h"
#include "LDSView.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "Query.h"
#include "DesignJoint.h"
#include "ScrTools.h"
#include "InputAnValDlg.h"
#include "DefFaceDlg.h"
#include "MainFrm.h"
#include "CutJg.h"
#include "LogFile.h"
#include "SortFunc.h"
#include "OutputInfo.h"
#include "Customize.h"
#include "LabelProp.h"
#include "SnapTypeVerify.h"
#include "OpenSlotDlg.h"
#include "./StdPartSolid/Fitting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//<DEVELOP_PROCESS_MARK nameId="FUNC_SMARTCUT_IN_2ANGLE">	
#ifdef __COMMON_PART_INC_
//�жϽǸ��Ƿ���������ӹ�ϵ
BOOL CCutJg::IsMainAndRayRelation(CLDSLineAngle* pJg1,CLDSLineAngle* pJg2,BOOL* pCutStart/* =NULL */,BOOL* pMainFirst/* =NULL */)
{
	if(pJg1->pStart==NULL || pJg1->pEnd==NULL || pJg2->pStart==NULL || pJg2->pEnd==NULL)
		return FALSE;
	BOOL bCutStart=FALSE,bMainFirst=FALSE;
	if(pJg1->layer(1)=='Z'&& pJg2->layer(1)=='Z')	//����������֮�䲻���ж�
		return FALSE;
	if(pJg1->desStartPos.datum_jg_h==pJg2->handle||pJg1->desEndPos.datum_jg_h==pJg2->handle)
	{	//���߽Ǹ��������Ǹ�
		if(pJg1->desStartPos.datum_jg_h==pJg2->handle)
			bCutStart=TRUE;
		else
			bCutStart=FALSE;
		bMainFirst=FALSE;
	}
	else if(pJg2->desStartPos.datum_jg_h==pJg1->handle||pJg2->desEndPos.datum_jg_h==pJg1->handle)
	{	//���߽Ǹ��������Ǹ�
		if(pJg2->desStartPos.datum_jg_h==pJg1->handle)
			bCutStart=TRUE;
		else
			bCutStart=FALSE;
		bMainFirst=TRUE;
	}
	else if(pJg1->pStart->hFatherPart==pJg2->handle||pJg1->pEnd->hFatherPart==pJg2->handle)	
	{	//���߽Ǹ��������Ǹ�
		if(pJg1->pStart->hFatherPart==pJg2->handle)
			bCutStart=TRUE;
		else
			bCutStart=FALSE;
		bMainFirst=FALSE;
	}
	else if(pJg2->pStart->hFatherPart==pJg1->handle||pJg2->pEnd->hFatherPart==pJg1->handle)
	{	//���߽Ǹ��������Ǹ�
		if(pJg2->pStart->hFatherPart==pJg1->handle)
			bCutStart=TRUE;
		else
			bCutStart=FALSE;
		bMainFirst=TRUE;
	}
	else
		return FALSE;
	if(pCutStart)
		*pCutStart=bCutStart;
	if(pMainFirst)
		*pMainFirst=bMainFirst;
	return TRUE;
}
//ͨ�����Ǹ�֮��Ĵ�ӹ�ϵ��ʼ���Ǹ��и���Ϣ
BOOL CCutJg::InitJgAndJgCutInfo(ANGLE_CUT_INFO* pJgCutInfo,BYTE cFlag/* =0 */)
{
	if(pJgCutInfo==NULL || pJgCutInfo->hPart1<=0X20 || pJgCutInfo->hPart2<=0X20)
		return FALSE;
	//1���������Ǹֵ�ͬ�˵�Ľڵ���Ϣȷ����ϵ����
	CLDSLineAngle* pJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->hPart1,CLS_LINEANGLE);
	CLDSLineAngle* pJg2=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->hPart2,CLS_LINEANGLE);
	BOOL bCutStart=FALSE,bOthCutStart=FALSE,bMainFirst=FALSE;
	if(IsMainAndRayRelation(pJg1,pJg2,&bCutStart,&bMainFirst))
	{	//���߽Ǹ��������Ǹ�
		pJgCutInfo->cRelationType=ANGLE_CUT_INFO::RELATION_MAINJG_RAYJG;
		pJgCutInfo->relation.MAIN_RAY.bCutStart=bCutStart;
		if(bMainFirst)
		{
			pJgCutInfo->relation.MAIN_RAY.hMainJg=pJg1->handle;	
			pJgCutInfo->relation.MAIN_RAY.hRayJg=pJg2->handle;
		}
		else
		{
			pJgCutInfo->relation.MAIN_RAY.hMainJg=pJg2->handle;	
			pJgCutInfo->relation.MAIN_RAY.hRayJg=pJg1->handle;
		}
	}
	else if(pJg1->pStart->handle==pJg2->pStart->handle||pJg1->pStart->handle==pJg2->pEnd->handle)
	{	//���߽Ǹ������߽Ǹ�
		bCutStart=TRUE;
		if(pJg1->pStart->handle==pJg2->pStart->handle)
			bOthCutStart=TRUE;
		else
			bOthCutStart=FALSE;
		pJgCutInfo->cRelationType=ANGLE_CUT_INFO::RELATION_RAYJG_RAYJG;
		pJgCutInfo->relation.RAY_RAY.hMainJg=pJg1->pStart->hFatherPart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[0]=pJg1->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[0]=bCutStart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[1]=pJg2->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[1]=bOthCutStart;
	}
	else if(pJg1->pEnd->handle==pJg2->pStart->handle||pJg1->pEnd->handle==pJg2->pEnd->handle)
	{	//���߽Ǹ������߽Ǹ�
		bCutStart=FALSE;
		if(pJg1->pEnd->handle==pJg2->pStart->handle)
			bOthCutStart=TRUE;
		else
			bOthCutStart=FALSE;
		pJgCutInfo->cRelationType=ANGLE_CUT_INFO::RELATION_RAYJG_RAYJG;
		pJgCutInfo->relation.RAY_RAY.hMainJg=pJg1->pEnd->hFatherPart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[0]=pJg1->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[0]=bCutStart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[1]=pJg2->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[1]=bOthCutStart;
	}
	else if(pJg1->pStart->hFatherPart==pJg2->pStart->hFatherPart||pJg1->pStart->hFatherPart==pJg2->pEnd->hFatherPart)
	{	//û��ͬ�ڵ�������߽Ǹ�
		bCutStart=TRUE;
		if(pJg1->pStart->hFatherPart==pJg2->pStart->hFatherPart)
			bOthCutStart=TRUE;
		else
			bOthCutStart=FALSE;
		pJgCutInfo->cRelationType=ANGLE_CUT_INFO::RELATION_RAYJG_RAYJG;
		pJgCutInfo->relation.RAY_RAY.hMainJg=pJg1->pStart->hFatherPart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[0]=pJg1->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[0]=bCutStart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[1]=pJg2->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[1]=bOthCutStart;
	}
	else if(pJg1->pEnd->hFatherPart==pJg2->pStart->hFatherPart||pJg1->pEnd->hFatherPart==pJg2->pEnd->hFatherPart)
	{	//û��ͬ�ڵ�������߽Ǹ�
		bCutStart=FALSE;
		if(pJg1->pEnd->hFatherPart==pJg2->pStart->hFatherPart)
			bOthCutStart=TRUE;
		else
			bOthCutStart=FALSE;
		pJgCutInfo->cRelationType=ANGLE_CUT_INFO::RELATION_RAYJG_RAYJG;
		pJgCutInfo->relation.RAY_RAY.hMainJg=pJg1->pEnd->hFatherPart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[0]=pJg1->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[0]=bCutStart;
		pJgCutInfo->relation.RAY_RAY.hRayJg[1]=pJg2->handle;
		pJgCutInfo->relation.RAY_RAY.bCutStart[1]=bOthCutStart;
	}
	else	//���Ǹ�û��λ��ͬһ�ڵ�Ķ˵�
		return FALSE;
	//2��ȥ����ƥ�������
	BOOL bMatch=FALSE;
	if(cFlag==0)
		bMatch=TRUE;
	if(cFlag&0x01 && pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_RAYJG_RAYJG)
		bMatch=TRUE;
	if(cFlag&0x02 && pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_MAINJG_RAYJG)
		bMatch=TRUE;
	if(!bMatch)
		return FALSE;
	CLDSLineAngle* pMainJg=NULL,*pRayJg=NULL;
	//3���������Ǹֵ�λ�ô����Ϣ����ʼ���Ǹ��и���Ϣ
	if(pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_MAINJG_RAYJG)
	{
		pMainJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->relation.MAIN_RAY.hMainJg,CLS_LINEANGLE);
		pRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->relation.MAIN_RAY.hRayJg,CLS_LINEANGLE);
		if(!CLDSLineAngle::CoplanarConnectDistinguish(pMainJg,pRayJg,
			&pJgCutInfo->relation.MAIN_RAY.bInMainWingX,&pJgCutInfo->relation.MAIN_RAY.bInRayWingX,
			&pJgCutInfo->relation.MAIN_RAY.bInboard))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("no direct work leg between main angle0X%Xand ray angle0X%X,��skip the cutting angle detection!",
#else
			logerr.Log("���Ǹ�0X%X�����߽Ǹ�0X%X����ֱ������֫,�������нǼ�⣡",
#endif
				pJgCutInfo->relation.MAIN_RAY.hMainJg,pJgCutInfo->relation.MAIN_RAY.hRayJg);
			return FALSE; //���Ǹ�֫�������������ݴ���
		}
		if(!pJgCutInfo->relation.MAIN_RAY.bInboard)	//���߽Ǹִ������Ǹ���֫
			return FALSE;
		//���Ǹ��и����߽Ǹ�
		pJgCutInfo->cCutMode=ANGLE_CUT_INFO::CUT_RAYJG_BY_DATUMJG;
		pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hDatumJg=pJgCutInfo->relation.MAIN_RAY.hMainJg;
		pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg=pJgCutInfo->relation.MAIN_RAY.hRayJg;
		pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart=pJgCutInfo->relation.MAIN_RAY.bCutStart;
	}
	else if(pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_RAYJG_RAYJG)
	{
		pMainJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->relation.RAY_RAY.hMainJg,CLS_LINEANGLE);
		//f3dPoint ray_vec[2];
		for(int i=0;i<2;i++)
		{
			pRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->relation.RAY_RAY.hRayJg[i],CLS_LINEANGLE);
			if(!CLDSLineAngle::CoplanarConnectDistinguish(pMainJg,pRayJg,
				&pJgCutInfo->relation.RAY_RAY.bInMainWingX[i],&pJgCutInfo->relation.RAY_RAY.bInRayWingX[i],
				&pJgCutInfo->relation.RAY_RAY.bInboard[i]))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Ray Angle and main angle without connect work-leg,ignore the cut detection of ray angle0X%X��",
#else 
				logerr.Log("���߽Ǹ�0X%X�����Ǹ�0X%X����ֱ������֫,�����������߽Ǹ�0X%X�нǼ�⣡",
#endif
					pJgCutInfo->relation.RAY_RAY.hRayJg[i],pJgCutInfo->relation.RAY_RAY.hMainJg,
					pJgCutInfo->relation.RAY_RAY.hRayJg[1-i]);
				return FALSE; //���Ǹ�֫�������������ݴ���
			}
			//if(pJgCutInfo->relation.RAY_RAY.bCutStart[0])
			//	ray_vec[i]=pRayJg->End()-pRayJg->Start();
			//else
			//	ray_vec[i]=pRayJg->Start()-pRayJg->End();
		}
		f3dPoint work_wing_norm1,work_wing_norm2,other_wing_norm1,other_wing_norm2;
		f3dPoint work_wing_vec1,work_wing_vec2,other_wing_vec1,other_wing_vec2;
		if(pJgCutInfo->relation.RAY_RAY.bInRayWingX[0])
		{
			work_wing_norm1=pJg1->get_norm_x_wing();
			work_wing_vec1=pJg1->GetWingVecX();
			other_wing_norm1=pJg1->get_norm_y_wing();
			other_wing_vec1=pJg1->GetWingVecY();
		}
		else
		{	
			work_wing_norm1=pJg1->get_norm_y_wing();
			work_wing_vec1=pJg1->GetWingVecY();
			other_wing_norm1=pJg1->get_norm_x_wing();
			other_wing_vec1=pJg1->GetWingVecX();
		}
		if(pJgCutInfo->relation.RAY_RAY.bInRayWingX[1])
		{	
			work_wing_norm2=pJg2->get_norm_x_wing();
			work_wing_vec2=pJg2->GetWingVecX();
			other_wing_norm2=pJg2->get_norm_y_wing();
			other_wing_vec2=pJg2->GetWingVecY();
		}
		else
		{	
			work_wing_norm2=pJg2->get_norm_y_wing();
			work_wing_vec2=pJg2->GetWingVecY();
			other_wing_norm2=pJg2->get_norm_x_wing();
			other_wing_vec2=pJg2->GetWingVecX();
		}
		double justify_rays=work_wing_norm1*work_wing_norm2;
		if(pJgCutInfo->relation.RAY_RAY.bInboard[0]&&pJgCutInfo->relation.RAY_RAY.bInboard[1]&&
			pJgCutInfo->relation.RAY_RAY.bInMainWingX[0]!=pJgCutInfo->relation.RAY_RAY.bInMainWingX[1])
		{	//�����߽Ǹֶ������Ǹ��ڲ࣬�������Ǹֲ�ͬ֫�ϣ������Ǹֵ���֫��ƽ������Ϊ�и����и����߽Ǹ�
			f3dPoint plane_vec=pMainJg->GetWingVecX()+pMainJg->GetWingVecY();
			normalize(plane_vec);
			f3dPoint ray_vec=pMainJg->End()-pMainJg->Start();
			normalize(ray_vec);
			f3dPoint plane_norm=plane_vec^ray_vec;
			normalize(plane_norm);
			pJgCutInfo->face_norm=plane_norm;
			pJgCutInfo->face_pick=pMainJg->Start();
			pJgCutInfo->cCutMode=ANGLE_CUT_INFO::CUT_TWORAYJG_BY_PLANE;
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg1=pJgCutInfo->relation.RAY_RAY.hRayJg[0];
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart1=pJgCutInfo->relation.RAY_RAY.bCutStart[0];
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg2=pJgCutInfo->relation.RAY_RAY.hRayJg[1];
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart2=pJgCutInfo->relation.RAY_RAY.bCutStart[1];
		}
		else if(fabs(justify_rays)<EPS_COS2)
		{	//�����߽Ǹ�Ϊ�ǹ���Ǹ�
			pJgCutInfo->face_norm.Set();//�����ʾ�Զ�����̽�н�;
			//pJgCutInfo->face_pick=pMainJg->Start();
			pJgCutInfo->cCutMode=ANGLE_CUT_INFO::CUT_TWORAYJG_BY_PLANE;
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg1=pJgCutInfo->relation.RAY_RAY.hRayJg[0];
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart1=pJgCutInfo->relation.RAY_RAY.bCutStart[0];
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg2=pJgCutInfo->relation.RAY_RAY.hRayJg[1];
			pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart2=pJgCutInfo->relation.RAY_RAY.bCutStart[1];
		}
		else if(pJgCutInfo->relation.RAY_RAY.bInboard[0]==pJgCutInfo->relation.RAY_RAY.bInboard[1]&&
			pJgCutInfo->relation.RAY_RAY.bInMainWingX[0]==pJgCutInfo->relation.RAY_RAY.bInMainWingX[1])	
		{	//�����߽Ǹ������Ǹֵ�ͬ�ࣨ�����ڲࣩ���������Ǹ�ͬ֫��(����)
			if(other_wing_norm1*other_wing_norm2<0)	//��ԣ�֫��֫��ײ����������ײ
			{	//�������߽Ǹֹ��湤��֫�����ߵ�ƽ������Ϊ�и����и����߽Ǹ�
				f3dPoint vec1,vec2,startPt1,endPt1,startPt2,endPt2,pt;
				vec1=pJg1->End()-pJg1->Start();
				normalize(vec1);
				startPt1=pJg1->Start()-vec1*pJg1->GetWidth();
				endPt1=pJg1->End()+vec1*pJg1->GetWidth();
				if(pJgCutInfo->relation.RAY_RAY.bCutStart[0]==FALSE)
					vec1*=-1;
				vec2=pJg2->End()-pJg2->Start();
				normalize(vec2);
				startPt2=pJg2->Start()-vec2*pJg2->GetWidth();
				endPt2=pJg2->End()+vec2*pJg2->GetWidth();
				if(pJgCutInfo->relation.RAY_RAY.bCutStart[1]==FALSE)
					vec2*=-1;
				project_point(startPt2,pJg1->Start(),work_wing_norm1);
				project_point(endPt2,pJg1->Start(),work_wing_norm1);
				int iRet=Int3dll(startPt1,endPt1,startPt2,endPt2,pt);
				if(Int3dll(startPt1,endPt1,startPt2,endPt2,pt)==1)
				{
					f3dPoint plane_vec=vec1+vec2;
					normalize(plane_vec);
					f3dPoint ray_vec=other_wing_vec1;
					normalize(ray_vec);
					f3dPoint plane_norm=plane_vec^ray_vec;
					normalize(plane_norm);
					pJgCutInfo->face_norm=plane_norm;
					pJgCutInfo->face_pick=pt;
					pJgCutInfo->cCutMode=ANGLE_CUT_INFO::CUT_TWORAYJG_BY_PLANE;
					pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg1=pJgCutInfo->relation.RAY_RAY.hRayJg[0];
					pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart1=pJgCutInfo->relation.RAY_RAY.bCutStart[0];
					pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg2=pJgCutInfo->relation.RAY_RAY.hRayJg[1];
					pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart2=pJgCutInfo->relation.RAY_RAY.bCutStart[1];
				}
				else
					return FALSE;
			}
			else //if(other_wing_norm1*other_wing_norm2>0)	//����ԣ�֫������ײ
			{	//ȷ����׼�Ǹ�
				pJgCutInfo->cCutMode=ANGLE_CUT_INFO::CUT_RAYJG_BY_DATUMJG;
				f3dPoint vec1,vec2,startPt,endPt,pt;
				vec1=pJg1->End()-pJg1->Start();
				normalize(vec1);
				f3dPoint datum_startPt,datum_endPt;
				datum_startPt=pJg1->Start()-vec1*pJg1->startOdd()-work_wing_vec1*g_sysPara.VertexDist;
				datum_endPt=pJg1->End()+vec1*pJg1->endOdd()-work_wing_vec1*g_sysPara.VertexDist;
				vec2=pJg2->End()-pJg2->Start();
				normalize(vec2);
				startPt=pJg2->Start()-vec2*pJg2->startOdd()+work_wing_vec2*pJg2->GetWidth();
				endPt=pJg2->End()+vec2*pJg2->endOdd()+work_wing_vec2*pJg2->GetWidth();
				project_point(startPt,pJg1->Start(),work_wing_norm1);
				project_point(endPt,pJg1->Start(),work_wing_norm1);
				if(Int3dll(startPt,endPt,datum_startPt,datum_endPt,pt)==1)
				{
					pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hDatumJg=pJgCutInfo->relation.RAY_RAY.hRayJg[0];
					pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg=pJgCutInfo->relation.RAY_RAY.hRayJg[1];
					pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart=pJgCutInfo->relation.RAY_RAY.bCutStart[1];
				}
				else
					return FALSE;
				//{
				//	pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hDatumJg=pJgCutInfo->relation.RAY_RAY.hRayJg[1];
				//	pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg=pJgCutInfo->relation.RAY_RAY.hRayJg[0];
				//	pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart=pJgCutInfo->relation.RAY_RAY.bCutStart[0];
				//}
			}
		}
		else	//�����иΧ
			return FALSE;
	}
	return TRUE;
}
//�����Ǹ�δ�����и�ǰ���н�����
static void CopyAngleCutData(ANGLE_CUT_INFO* pJgCutInfo,ATOM_LIST<ANGLE_CUTDATA>& cutDataList,BOOL bReverse)
{
	if(pJgCutInfo==NULL)
		return;
	if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_BOLT)
	{	//�Ǹ�����˨֮����н���Ϣ�ڼ��ʱ�������
		ANGLE_CUTDATA* pCutData=cutDataList.append();
		//ʼ����֫
		if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.cCutWing=='X')
			pCutData->cut_wing[0]=1;
		else if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.cCutWing=='Y')
			pCutData->cut_wing[0]=2;
		else
			pCutData->cut_wing[0]=0;
		if(pCutData->cut_wing[0]>0)
		{
			pCutData->cut_wing_para[0][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.wing.fCutBerLength;
			pCutData->cut_wing_para[0][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.wing.fCutWidth;
			pCutData->cut_wing_para[0][2]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.wing.fCutWingLength;
		}
		//�ն���֫
		if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.cCutWing=='X')
			pCutData->cut_wing[1]=1;
		else if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.cCutWing=='Y')
			pCutData->cut_wing[1]=2;
		else
			pCutData->cut_wing[1]=0;
		if(pCutData->cut_wing[1]>0)
		{
			pCutData->cut_wing_para[1][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.wing.fCutBerLength;
			pCutData->cut_wing_para[1][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.wing.fCutWidth;
			pCutData->cut_wing_para[1][2]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.wing.fCutWingLength;
		}
		//ʼ���н�
		pCutData->cut_angle[0][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[0].fLength;
		pCutData->cut_angle[0][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[0].fWidth;
		pCutData->cut_angle[1][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[1].fLength;
		pCutData->cut_angle[1][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[1].fWidth;
		//�ն��н�
		pCutData->cut_angle[2][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[0].fLength;
		pCutData->cut_angle[2][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[0].fWidth;
		pCutData->cut_angle[3][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[1].fLength;
		pCutData->cut_angle[3][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[1].fWidth;
		return;
	}
	//
	CLDSLineAngle* pRayJg1=NULL,*pRayJg2=NULL;
	if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_DATUMJG)
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg);
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_PLANE)
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_PLANE.hRayJg);
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_TWORAYJG_BY_45DU)
	{
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.hRayJg1);
		pRayJg2=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.hRayJg2);
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_TWORAYJG_BY_PLANE)
	{
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg1);
		pRayJg2=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg2);
	}
	if(pRayJg1==NULL)
		return ;
	if(bReverse==FALSE)
	{	//���Ǹ��н����ݿ�������ʱ���б���
		for(int i=0;i<2;i++)
		{
			CLDSLineAngle* pAngle=NULL;
			if(i==0)
				pAngle=pRayJg1;
			else
				pAngle=pRayJg2;
			if(pAngle==NULL)
				continue;
			//��֫����
			ANGLE_CUTDATA* pCutData=cutDataList.append();
			pCutData->m_bUserSpecStartCutAngle=pAngle->m_bUserSpecStartCutAngle;
			pCutData->m_bUserSpecEndCutAngle  =pAngle->m_bUserSpecEndCutAngle;
			pCutData->cut_wing[0]=pAngle->cut_wing[0];
			pCutData->cut_wing_para[0][0]=pAngle->cut_wing_para[0][0];
			pCutData->cut_wing_para[0][1]=pAngle->cut_wing_para[0][1];
			pCutData->cut_wing_para[0][2]=pAngle->cut_wing_para[0][2];
			pCutData->cut_wing[1]=pAngle->cut_wing[1];
			pCutData->cut_wing_para[1][0]=pAngle->cut_wing_para[1][0];
			pCutData->cut_wing_para[1][1]=pAngle->cut_wing_para[1][1];
			pCutData->cut_wing_para[1][2]=pAngle->cut_wing_para[1][2];
			//�н�����
			pCutData->cut_angle[0][0]=pAngle->cut_angle[0][0];
			pCutData->cut_angle[0][1]=pAngle->cut_angle[0][1];
			pCutData->cut_angle[1][0]=pAngle->cut_angle[1][0];
			pCutData->cut_angle[1][1]=pAngle->cut_angle[1][1];
			pCutData->cut_angle[2][0]=pAngle->cut_angle[2][0];
			pCutData->cut_angle[2][1]=pAngle->cut_angle[2][1];
			pCutData->cut_angle[3][0]=pAngle->cut_angle[3][0];
			pCutData->cut_angle[3][1]=pAngle->cut_angle[3][1];
			//��ʱ�����Ա��нǼ��ʱ�Զ������н�ֵ wjh-2016.6.16
			pAngle->m_bUserSpecStartCutAngle=FALSE;
			pAngle->m_bUserSpecEndCutAngle=FALSE;
		}
	}
	else
	{	//ͨ����ʱ�б��н����ݻ�ԭ�Ǹֵ��н�����
		for(int i=0;i<2;i++)
		{
			CLDSLineAngle* pAngle=NULL;
			if(i==0)
				pAngle=pRayJg1;
			else
				pAngle=pRayJg2;
			if(pAngle==NULL)
				continue;
			//��֫����
			ANGLE_CUTDATA* pCutData=cutDataList.GetByIndex(i);
			pAngle->m_bUserSpecStartCutAngle=pCutData->m_bUserSpecStartCutAngle;
			pAngle->m_bUserSpecEndCutAngle  =pCutData->m_bUserSpecEndCutAngle;
			pAngle->cut_wing[0]=pCutData->cut_wing[0];
			pAngle->cut_wing_para[0][0]=pCutData->cut_wing_para[0][0];
			pAngle->cut_wing_para[0][1]=pCutData->cut_wing_para[0][1];
			pAngle->cut_wing_para[0][2]=pCutData->cut_wing_para[0][2];
			pAngle->cut_wing[1]=pCutData->cut_wing[1];
			pAngle->cut_wing_para[1][0]=pCutData->cut_wing_para[1][0];
			pAngle->cut_wing_para[1][1]=pCutData->cut_wing_para[1][1];
			pAngle->cut_wing_para[1][2]=pCutData->cut_wing_para[1][2];
			//�н�����
			pAngle->cut_angle[0][0]=pCutData->cut_angle[0][0];
			pAngle->cut_angle[0][1]=pCutData->cut_angle[0][1];
			pAngle->cut_angle[1][0]=pCutData->cut_angle[1][0];
			pAngle->cut_angle[1][1]=pCutData->cut_angle[1][1];
			pAngle->cut_angle[2][0]=pCutData->cut_angle[2][0];
			pAngle->cut_angle[2][1]=pCutData->cut_angle[2][1];
			pAngle->cut_angle[3][0]=pCutData->cut_angle[3][0];
			pAngle->cut_angle[3][1]=pCutData->cut_angle[3][1];
		}
	}
}
static BOOL CompareAngleCutData(ANGLE_CUT_INFO* pJgCutInfo,ATOM_LIST<ANGLE_CUTDATA>& cutDataList)
{
	if(pJgCutInfo==NULL)
		return FALSE;
	CLDSLineAngle* pRayJg1=NULL,*pRayJg2=NULL;
	BOOL bCutStart=FALSE,bCutStart2=FALSE,bCollision=FALSE;
	if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_DATUMJG)
	{
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg);
		bCutStart=pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart;
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_PLANE)
	{	
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_PLANE.hRayJg);
		bCutStart=pJgCutInfo->cut_para.CUT_ONE_BY_PLANE.bCutStart;
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_TWORAYJG_BY_45DU)
	{
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.hRayJg1);
		bCutStart=pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart1;
		pRayJg2=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.hRayJg2);
		bCutStart2=pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart2;
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_TWORAYJG_BY_PLANE)
	{
		pRayJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg1);
		bCutStart=pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart1;
		pRayJg2=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg2);
		bCutStart2=pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.bCutStart2;
	}
	if(pRayJg1==NULL)
		return FALSE;
	BOOL bCutJg=FALSE;
	//if(pRayJg2==NULL)
	{
		ANGLE_CUTDATA* pCutData=cutDataList.GetByIndex(0);
		if(bCutStart)	//ʼ���и�
		{	
			if(pRayJg1->cut_wing_para[0][0]>pCutData->cut_wing_para[0][0] &&	
				pRayJg1->cut_wing_para[0][1]>pCutData->cut_wing_para[0][1]&&
				pRayJg1->cut_wing_para[0][2]>pCutData->cut_wing_para[0][2])		//ʼ�˱���֫
				bCutJg=TRUE;
			else if(pRayJg1->cut_angle[0][0]>pCutData->cut_angle[0][0] && 
				pRayJg1->cut_angle[0][1]>pCutData->cut_angle[0][1])				//ʼ��X֫�н�
				bCutJg=TRUE;
			else if(pRayJg1->cut_angle[1][0]>pCutData->cut_angle[1][0] &&
				pRayJg1->cut_angle[1][1]>pCutData->cut_angle[1][1])				//ʼ��Y֫�н�
				bCutJg=TRUE;
			else
				bCutJg=FALSE;
		}
		else	//�ն��и�
		{	
			if(pRayJg1->cut_wing_para[1][0]>pCutData->cut_wing_para[1][0] &&	
				pRayJg1->cut_wing_para[1][1]>pCutData->cut_wing_para[1][1]&&
				pRayJg1->cut_wing_para[1][2]>pCutData->cut_wing_para[1][2])		//�ն˱���֫
				bCutJg=TRUE;
			else if(pRayJg1->cut_angle[2][0]>pCutData->cut_angle[2][0] && 
				pRayJg1->cut_angle[2][1]>pCutData->cut_angle[2][1])				//�ն�X֫�н�
				bCutJg=TRUE;
			else if(pRayJg1->cut_angle[3][0]>pCutData->cut_angle[3][0] &&
				pRayJg1->cut_angle[3][1]>pCutData->cut_angle[3][1])				//�ն�Y֫�н�
				bCutJg=TRUE;
			else
				bCutJg=FALSE;
		}
	}
	if(pRayJg2 && !bCutJg)
	{
		ANGLE_CUTDATA* pCutData=cutDataList.GetByIndex(1);
		if(bCutStart2)	//ʼ���и�
		{	
			if(pRayJg2->cut_wing_para[0][0]>pCutData->cut_wing_para[0][0]&&
				pRayJg2->cut_wing_para[0][1]>pCutData->cut_wing_para[0][1]&&
				pRayJg2->cut_wing_para[0][2]>pCutData->cut_wing_para[0][2])		//ʼ�˱���֫
				bCutJg=TRUE;
			else if(pRayJg2->cut_angle[0][0]>pCutData->cut_angle[0][0]&&
				pRayJg2->cut_angle[0][1]>pCutData->cut_angle[0][1])				//ʼ��X֫�н�
				bCutJg=TRUE;
			else if(pRayJg2->cut_angle[1][0]>pCutData->cut_angle[1][0]&&
				pRayJg2->cut_angle[1][1]>pCutData->cut_angle[1][1])				//ʼ��Y֫�н�
				bCutJg=TRUE;
			else
				bCutJg=FALSE;
		}
		else	//�ն��и�
		{	
			if(pRayJg2->cut_wing_para[1][0]>pCutData->cut_wing_para[1][0]&&
				pRayJg2->cut_wing_para[1][1]>pCutData->cut_wing_para[1][1]&&
				pRayJg2->cut_wing_para[1][2]>pCutData->cut_wing_para[1][2])		//�ն˱���֫
				bCutJg=TRUE;
			else if(pRayJg2->cut_angle[2][0]>pCutData->cut_angle[2][0]&&
				pRayJg2->cut_angle[2][1]>pCutData->cut_angle[2][1])				//�ն�X֫�н�
				bCutJg=TRUE;
			else if(pRayJg2->cut_angle[3][0]>pCutData->cut_angle[3][0]&&
				pRayJg2->cut_angle[3][1]>pCutData->cut_angle[3][1])				//�ն�Y֫�н�
				bCutJg=TRUE;
			else
				bCutJg=FALSE;
		}
	}
	return bCutJg;
}
//�ǸֺͽǸ�֮����нǼ���������ײ���,�к�����ײ��ʼ���и���Ϣ
BOOL CCutJg::CollisionCheck(ANGLE_CUT_INFO* pJgCutInfo,BYTE cFlag/* =0 */)
{	
	if(pJgCutInfo==NULL||pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_ANGLE_BOLT)
		return FALSE;
	//1����ʼ���Ǹ��н���Ϣ
	CLDSPart* pPart1=Ta.FromPartHandle(pJgCutInfo->hPart1);
	CLDSPart* pPart2=Ta.FromPartHandle(pJgCutInfo->hPart2);
	if(!pPart1->IsAngle()||!pPart2->IsAngle())
		return FALSE;
	CLDSLineAngle* pJg1=(CLDSLineAngle*)pPart1;
	CLDSLineAngle* pJg2=(CLDSLineAngle*)pPart2;
	if(pJg1->m_hPartWeldParent==pJg2->handle||pJg2->m_hPartWeldParent==pJg1->handle)
		return FALSE;	//���˼�������һ�� ������ײ	
	if(!InitJgAndJgCutInfo(pJgCutInfo,cFlag))
		return FALSE;
	//2�����ڷǹ������߽Ǹֻ���ԵȽǸ����Ƚ���ʵ����ײ���
	//if( pJgCutInfo->cCutMode!=ANGLE_CUT_INFO::CUT_RAYJG_BY_DATUMJG)
	{
		if(pPart1->m_bDisplayHole||g_sysPara.bDisplayAllHole)
			pPart1->SetModified();
		if(pPart1->pSolidBody==NULL)
			pPart1->Create3dSolidModel(FALSE);
		if(pPart2->m_bDisplayHole||g_sysPara.bDisplayAllHole)
			pPart2->SetModified();
		if(pPart2->pSolidBody==NULL)
			pPart2->Create3dSolidModel(FALSE);
		if(!pPart1->pSolidBody->IsIntWith(pPart2->pSolidBody))
			return FALSE;
	}
	//3�������н�ǰ�ĽǸ��н���Ϣ���ԽǸֽ����нǴ���
	ATOM_LIST<ANGLE_CUTDATA> cutDataList;
	CopyAngleCutData(pJgCutInfo,cutDataList,FALSE);
	FinishCutAngle(pJgCutInfo,NULL,0,TRUE);
	//4���Ƚ��и�ǰ����и������ж��Ƿ���Ҫ�����и�����и�ĽǸֽ����ݻ�ԭ
	BOOL bCutJg=CompareAngleCutData(pJgCutInfo,cutDataList);
	CopyAngleCutData(pJgCutInfo,cutDataList,TRUE);	//��ԭ�Ǹ��и���Ϣ
	return bCutJg;
}
//�Ǹ�����˨֮����нǼ����Ҫ�����н����ݱȶ�
BOOL CCutJg::CutDataCheck(ANGLE_CUT_INFO* pJgCutInfo)
{
	if(pJgCutInfo==NULL||pJgCutInfo->cRelationType!=ANGLE_CUT_INFO::RELATION_ANGLE_BOLT)
		return FALSE;
	CLDSLineAngle* pJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->hPart1,CLS_LINEANGLE);
	if(pJg==NULL)
		return FALSE;
	//�н����ݱȶ�
	BOOL bCutJg=FALSE;
	ATOM_LIST<ANGLE_CUTDATA> cutDataList;
	CopyAngleCutData(pJgCutInfo,cutDataList,FALSE);
	if(pJgCutInfo->cut_para.CUT_BY_BOLT.bCutStart)	//ʼ���и�
	{	
		if(cutDataList[0].cut_wing[0]>0)
		{	//������֫
			if(pJg->cut_wing[0]==0||cutDataList[0].cut_wing_para[0][0]>pJg->cut_wing_para[0][0] ||	
				cutDataList[0].cut_wing_para[0][1]>pJg->cut_wing_para[0][1] ||
				cutDataList[0].cut_wing_para[0][2]>pJg->cut_wing_para[0][2])		//ʼ�˱���֫
				return TRUE;
		}
		else
		{	//�����нǣ����ȿ��ǽǸ�����֫�������
			if((pJg->cut_wing[0]==1&&pJg->cut_wing_para[0][2]>cutDataList[0].cut_angle[0][0])||
				(pJg->cut_wing[0]==2&&pJg->cut_wing_para[0][2]>cutDataList[0].cut_angle[1][0]))
				return FALSE;
			//�нǱȽ�
			if(cutDataList[0].cut_angle[0][0]>pJg->cut_angle[0][0] && 
				cutDataList[0].cut_angle[0][1]>pJg->cut_angle[0][1])		//ʼ��X֫�н�
				return TRUE;
			else if(cutDataList[0].cut_angle[1][0]>pJg->cut_angle[1][0] &&
				cutDataList[0].cut_angle[1][1]>pJg->cut_angle[1][1])		//ʼ��Y֫�н�
				return TRUE;
		}
	}
	else	//�ն��и�
	{	
		if(cutDataList[0].cut_wing[1]>0)
		{	//������֫
			if(pJg->cut_wing[1]==0||cutDataList[0].cut_wing_para[1][0]>pJg->cut_wing_para[1][0] ||	
			   cutDataList[0].cut_wing_para[1][1]>pJg->cut_wing_para[1][1] ||
			   cutDataList[0].cut_wing_para[1][2]>pJg->cut_wing_para[1][2])		//�ն˱���֫
				return TRUE;
		}
		else
		{	//�����нǣ����ȿ��ǽǸ�����֫�������
			if((pJg->cut_wing[1]==1&&pJg->cut_wing_para[1][2]>cutDataList[0].cut_angle[2][0])||
				(pJg->cut_wing[1]==2&&pJg->cut_wing_para[1][2]>cutDataList[0].cut_angle[3][0]))
				return FALSE;
			//�нǱȽ�
			else if(cutDataList[0].cut_angle[2][0]>pJg->cut_angle[2][0] && 
				cutDataList[0].cut_angle[2][1]>pJg->cut_angle[2][1])		//�ն�X֫�н�
				return TRUE;
			else if(cutDataList[0].cut_angle[3][0]>pJg->cut_angle[3][0] &&
				cutDataList[0].cut_angle[3][1]>pJg->cut_angle[3][1])		//�ն�Y֫�н�
				return TRUE;
		}
	}
	return FALSE;
}
/*
�ڽ��м������еĽǸ��н�ʱ��
	ͳһ���Ǹֵ�ʼ�ն��иʽ��ʱ�趨Ϊ�Զ��нǣ���֤��ײ�ĽǸ����н����ݣ����ں�����н����ݱȶԡ�
�ڽ���ʵ�ʹ����еĽǸ��н�ʱ��
	����иʽΪ���ݻ�׼�Ǹ��и���迼��ʼ�ն��Ƿ��û�ָ��,��������ʾ
	����иʽΪ45���нǻ�ͨ�����и���迼��ʼ�ն��Ƿ��û�ָ����ͳһ�����и�
*/
BOOL CCutJg::FinishCutAngle(ANGLE_CUT_INFO* pJgCutInfo,int (*InputCollideDist)(int dist),int nCutDist/* =0 */,BOOL bCheck/*=FALSE*/)
{
	if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_DATUMJG)
	{	//
		CLDSLineAngle* pDatumJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hDatumJg,CLS_LINEANGLE);
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg,CLS_LINEANGLE);
		if(CLDSPart::CustomerSerial==CUSTOMIZE::CLIENT_GUANGZHOU_ZENGLI)
		{
			if(pDatumJg->size_wide>=160&&nCutDist<10)
				nCutDist=10;
			else if(pRayJg->size_wide>=160&&nCutDist<10)
				nCutDist=10;
		}
		if(pRayJg->m_bUserSpecStartCutAngle&&pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The start cut-leg infomation of angle 0X%X  is setted by users,system ignore cut operation of start��",pRayJg->handle);
#else 
			logerr.Log("0X%X�Ǹ�ʼ�˵��нǹ�����Ϣ�趨Ϊ�û�ָ����ϵͳ�����˴˽Ǹ�ʼ�˵��нǲ�����",pRayJg->handle);
#endif
			return FALSE;
		}
		if(pRayJg->m_bUserSpecEndCutAngle&&!pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The end cut-leg infomation of angle 0X%X  is setted by users,system ignore cut operation of the end��",pRayJg->handle);
#else 
			logerr.Log("0X%X�Ǹ��ն˵��нǹ�����Ϣ�趨Ϊ�û�ָ����ϵͳ�����˴˽Ǹ��ն˵��нǲ�����",pRayJg->handle);
#endif
			return FALSE;
		}
		BOOL bCurrModified=pRayJg->IsModified();
		BOOL bCurrSolidModified=pRayJg->IsSolidModified();
		if(pRayJg->CalCutAngleInfo(pDatumJg,NULL,(pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart==1),InputCollideDist,nCutDist))
		{
			if(!bCheck)
			{
				pRayJg->SetModified();
				pRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole);
				g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
			}
			else
				pRayJg->SetModified(bCurrModified,bCurrSolidModified);
			return TRUE;
		}
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_PLANE)
	{
		CLDSLineAngle* pDatumJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_PLANE.hDatumJg,CLS_LINEANGLE);
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_PLANE.hRayJg,CLS_LINEANGLE);
		if(CLDSPart::CustomerSerial==CUSTOMIZE::CLIENT_GUANGZHOU_ZENGLI)
		{
			if(pDatumJg->size_wide>=160&&nCutDist<10)
				nCutDist=10;
			else if(pRayJg->size_wide>=160&&nCutDist<10)
				nCutDist=10;
		}
		BOOL bCurrModified=pRayJg->IsModified();
		BOOL bCurrSolidModified=pRayJg->IsSolidModified();
		if(pRayJg->CalCutAngleInfo(pJgCutInfo->face_pick,pJgCutInfo->face_norm,InputCollideDist,nCutDist))
		{
			if(!bCheck)
			{
				pRayJg->SetModified();
				pRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole);
				g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
			}
			else
				pRayJg->SetModified(bCurrModified,bCurrSolidModified);
			return TRUE;
		}
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_TWORAYJG_BY_PLANE)
	{
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg1,CLS_LINEANGLE);
		CLDSLineAngle* pOthRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_PLANE.hRayJg2,CLS_LINEANGLE);
		if(nCutDist==0)
			nCutDist=CLDSPart::library->GetVertexDist();
		if(CLDSPart::CustomerSerial==CUSTOMIZE::CLIENT_GUANGZHOU_ZENGLI&&nCutDist<10)
		{
			if(pRayJg->size_wide>=160||pOthRayJg->size_wide>=160)
				nCutDist=10;
		}
		if(InputCollideDist)
			nCutDist=InputCollideDist(nCutDist);
		nCutDist+=nCutDist%2;
		BOOL bRayCurrModified=pRayJg->IsModified();
		BOOL bRayCurrSolidModified=pRayJg->IsSolidModified();
		BOOL bOthRayCurrModified=pOthRayJg->IsModified();
		BOOL bOthRayCurrSolidModified=pOthRayJg->IsSolidModified();
		if(pRayJg->CalCutAngleInfo(pJgCutInfo->face_pick,pJgCutInfo->face_norm,NULL,nCutDist/2) && 
			pOthRayJg->CalCutAngleInfo(pJgCutInfo->face_pick,pJgCutInfo->face_norm,NULL,nCutDist/2))
		{
			if(!bCheck)
			{
				pRayJg->SetModified();
				pRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole);
				g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
				//
				pOthRayJg->SetModified();
				pOthRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole);
				g_pSolidDraw->NewSolidPart(pOthRayJg->GetSolidPartObject());
			}
			else
			{
				pRayJg->SetModified(bRayCurrModified,bRayCurrSolidModified);
				pRayJg->SetModified(bOthRayCurrModified,bOthRayCurrSolidModified);
			}
			return TRUE;
		}
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_TWORAYJG_BY_45DU)
	{
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.hRayJg1,CLS_LINEANGLE);
		CLDSLineAngle* pOthRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.hRayJg2,CLS_LINEANGLE);
		double R1=CLDSPart::library->FindAngleInnerR(pRayJg->size_wide);
		double R2=CLDSPart::library->FindAngleInnerR(pOthRayJg->size_wide);
		if(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart1&&pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX1)	//ʼ��X֫
			pRayJg->cut_angle[0][0]=pRayJg->cut_angle[0][1]=pRayJg->GetWidth()-pRayJg->GetThick()-R1;
		else if(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart1&&!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX1)//ʼ��Y֫
			pRayJg->cut_angle[1][0]=pRayJg->cut_angle[1][1]=pRayJg->GetWidth()-pRayJg->GetThick()-R1;
		else if(!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart1&&pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX1)//�ն�X֫
			pRayJg->cut_angle[2][0]=pRayJg->cut_angle[2][1]=pRayJg->GetWidth()-pRayJg->GetThick()-R1;
		else if(!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart1&&!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX1)//�ն�Y֫
			pRayJg->cut_angle[3][0]=pRayJg->cut_angle[3][1]=pRayJg->GetWidth()-pRayJg->GetThick()-R1;
		if(!bCheck)
		{
			pRayJg->SetModified();
			pRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
		}
		//
		if(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart2&&pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX2)	//ʼ��X֫
			pOthRayJg->cut_angle[0][0]=pOthRayJg->cut_angle[0][1]=pOthRayJg->GetWidth()-pOthRayJg->GetThick()-R2;
		else if(pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart2&&!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX2)//ʼ��Y֫
			pOthRayJg->cut_angle[1][0]=pOthRayJg->cut_angle[1][1]=pOthRayJg->GetWidth()-pOthRayJg->GetThick()-R2;
		else if(!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart2&&pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX2)//�ն�X֫
			pOthRayJg->cut_angle[2][0]=pOthRayJg->cut_angle[2][1]=pOthRayJg->GetWidth()-pOthRayJg->GetThick()-R2;
		else if(!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bCutStart2&&!pJgCutInfo->cut_para.CUT_TWO_BY_45DU.bInRayWingX2)//�ն�Y֫
			pOthRayJg->cut_angle[3][0]=pOthRayJg->cut_angle[3][1]=pOthRayJg->GetWidth()-pOthRayJg->GetThick()-R2;
		if(bCheck)
		{
			pOthRayJg->SetModified();
			pOthRayJg->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(pOthRayJg->GetSolidPartObject());
		}
		return TRUE;
	}
	else if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_BOLT)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->hPart1,CLS_LINEANGLE);
		if(pJgCutInfo->cut_para.CUT_BY_BOLT.bCutStart)
		{
			if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.cCutWing>0)
			{
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.cCutWing=='X')
					pAngle->cut_wing[0]=1;
				else
					pAngle->cut_wing[0]=2;
				pAngle->cut_wing_para[0][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.wing.fCutBerLength;
				pAngle->cut_wing_para[0][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.wing.fCutWidth;
				pAngle->cut_wing_para[0][2]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.wing.fCutWingLength;
			}
			else
			{
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[0].fLength>0)
					pAngle->cut_angle[0][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[0].fLength;
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[0].fWidth>0)
					pAngle->cut_angle[0][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[0].fWidth;
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[1].fLength>0)
					pAngle->cut_angle[1][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[1].fLength;
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[1].fWidth>0)
					pAngle->cut_angle[1][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.start.corner[1].fWidth;
			}
		}
		else
		{
			if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.cCutWing>0)
			{
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.cCutWing=='X')
					pAngle->cut_wing[1]=1;
				else if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.cCutWing=='Y')
					pAngle->cut_wing[1]=2;
				pAngle->cut_wing_para[1][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.wing.fCutBerLength;
				pAngle->cut_wing_para[1][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.wing.fCutWidth;
				pAngle->cut_wing_para[1][2]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.wing.fCutWingLength;
			}
			else
			{
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[0].fLength>0)
					pAngle->cut_angle[2][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[0].fLength;
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[0].fWidth>0)
					pAngle->cut_angle[2][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[0].fWidth;
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[1].fLength>0)
					pAngle->cut_angle[3][0]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[1].fLength;
				if(pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[1].fWidth>0)
					pAngle->cut_angle[3][1]=pJgCutInfo->cut_para.CUT_BY_BOLT.end.corner[1].fWidth;
			}
		}
		BOOL bCurrModified=pAngle->IsModified();
		BOOL bCurrSolidModified=pAngle->IsSolidModified();
		if(!bCheck)
		{
			pAngle->SetModified();
			pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
		}
		else
			pAngle->SetModified(bCurrModified,bCurrSolidModified);
		return TRUE;
	}
	return FALSE;
}
//���ڸ��ݻ�׼�Ǹֽ����и�����������ʾ�Ƿ����߽Ǹ�ʼ�˻��ն��ֶ�ָ���н���Ϣ
int CCutJg::FinishCheckCutAngle(ANGLE_CUT_INFO* pJgCutInfo)
{
	if(pJgCutInfo->cCutMode==ANGLE_CUT_INFO::CUT_RAYJG_BY_DATUMJG)
	{
		CLDSLineAngle* pDatumJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hDatumJg,CLS_LINEANGLE);
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg,CLS_LINEANGLE);
		if(pRayJg->m_bUserSpecStartCutAngle&&pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart)
		{
			pJgCutInfo->hPart1=pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg;
			return 2;	//�Ǹ�ʼ���ֶ�ָ���н���Ϣ����������Ƽ���
		}
		else if(pRayJg->m_bUserSpecEndCutAngle&&!pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.bCutStart)
		{	
			pJgCutInfo->hPart1=pJgCutInfo->cut_para.CUT_ONE_BY_DATUM.hRayJg;
			return 3;	//�Ǹ��ն��ֶ�ָ���н���Ϣ����������Ƽ���
		}
		else 
			return 1;
	}
	return 1;
}
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
int InputCutAngleCollideDist(int dist)
{
	CInputAnIntegerValDlg cut_dist_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	cut_dist_dlg.m_sValTitle = "Cut clearance";
#else 
	cut_dist_dlg.m_sValTitle = "�и��϶";
#endif
	cut_dist_dlg.m_nVal = dist;
	cut_dist_dlg.DoModal();
	return cut_dist_dlg.m_nVal;
}
BOOL CLDSView::FinishCalCutJg(CLDSDbObject *pObj1,CLDSDbObject *pObj2)
{
	BOOL result=TRUE;
	CUndoOperObject undo(&Ta);
	try{
		if(pObj1->GetClassTypeId()!=CLS_LINEANGLE||pObj2->GetClassTypeId()!=CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Choose the wrong type of tower parts!";
#else 
			throw "ѡ���˴������͵�������!";
#endif
		CLDSLineAngle* pRayJg = (CLDSLineAngle*)pObj1;
		CLDSLineAngle* pMainJg= (CLDSLineAngle*)pObj2;
		bool is_start_end;
		if(pMainJg->pStart==NULL||pMainJg->pEnd==NULL)
		{	//�̽Ǹ� wht 11-01-10
			f3dLine line(pMainJg->Start(),pMainJg->End());
			double start_dist=0,end_dist=0;
			f3dPoint perp;
			SnapPerp(&perp,line,pRayJg->Start(),&start_dist);
			SnapPerp(&perp,line,pRayJg->End(),&end_dist);
			if(fabs(start_dist)<fabs(end_dist))
				is_start_end=true;
			else 
				is_start_end=false;
		}
		else 
		{	
			f3dLine line(pMainJg->pStart->Position(false),pMainJg->pEnd->Position(false));
			if(	  pRayJg->pStart==pMainJg->pStart
				||pRayJg->pStart==pMainJg->pEnd
				||line.PtInLine(pRayJg->pStart->Position(false))!=0)
				is_start_end = true;
			else if(pRayJg->pEnd==pMainJg->pStart
				||pRayJg->pEnd==pMainJg->pEnd
				||line.PtInLine(pRayJg->pEnd->Position(false))!=0)
				is_start_end = false;
			else
				throw "�����߶˽ڵ㲻���и��׼�Ǹ�����,���޷��н���֫";
		}
		logerr.ClearContents();
		//��ΰ���ͬ��ŽǸ֣�ֻҪ�����нǲ�����Ӧ�ý���ͬ��ŶԳƸ˼������ԳƸ��� wjh-2019.3.30
		if(is_start_end)
		{
			pRayJg->CalCutAngleInfo((CLDSLineAngle*)pObj2,NULL,is_start_end,InputCutAngleCollideDist,ftoi(pRayJg->desStartOdd.m_fCollideDist));
			pRayJg->SyncMirProp("oddStartOnSameLabel",0,LABEL_PROP::ANGLE_CUT_START);
		}
		else
		{
			pRayJg->CalCutAngleInfo((CLDSLineAngle*)pObj2,NULL,is_start_end,InputCutAngleCollideDist,ftoi(pRayJg->desEndOdd.m_fCollideDist));
			pRayJg->SyncMirProp("oddEndOnSameLabel",0,LABEL_PROP::ANGLE_CUT_END);
		}
/*
		ANGLE_CUT_INFO jgCutInfo;
		jgCutInfo.hPart1=pObj1->handle;
		jgCutInfo.hPart2=pObj2->handle;
		CLDSLineAngle *pCutAngle=(CLDSLineAngle*)pObj1;
		logerr.ClearContents();
		if(!CCutJg::CollisionCheck(&jgCutInfo))
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The two angle without effective collision,so it needn't cut leg��";
#else 
			throw "�����Ǹ�û�з�����Ч��ײ������Ҫ�н���֫��";
#endif
		if(!CCutJg::FinishCutAngle(&jgCutInfo,InputCutAngleCollideDist)==1)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The ray angle doesn't need cut leg,the input information may be wrong��";
#else 
			throw "�����߽Ǹֲ���Ҫ�н���֫,��������Ϣ����";
#endif
*/
	}
	catch (char *sError)
	{
		AfxMessageBox(sError);
		result = FALSE;
	}
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
	return result;
}
#endif
//</DEVELOP_PROCESS_MARK>
void CLDSView::OnSpecCutFaceCutJg() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_SPEC_CUT_FACE_CUT_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeatedly specify cut-surface or cut leg";
#else 
	m_sPrevCommandName="�ظ�ָ���и����н���֫";
#endif
	f3dPoint face_pick,face_norm;
	static CDefFaceDlg cut_face_dlg;
	if(cut_face_dlg.DoModal()!=IDOK)
		return;
	static CInputAnIntegerValDlg cut_dist_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	cut_dist_dlg.m_sValTitle = "Cut clearance";
#else 
	cut_dist_dlg.m_sValTitle = "�и��϶";
#endif
	cut_dist_dlg.m_nVal = ftol(g_sysPara.VertexDist/2.0);
	if(cut_dist_dlg.DoModal()!=IDOK)
		return;
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	int nCutCount=0;
	int nCutDist=cut_dist_dlg.m_nVal;
	for(int i=0;i<n;i++)
	{
		CLDSDbObject* pObj = (CLDSDbObject*)console.FromHandle(id_arr[i],TRUE);
		if(pObj->GetClassTypeId()!=CLS_LINEANGLE)	//��û������ϽǸ����
			continue;
		CLDSLineAngle *pRayJg=(CLDSLineAngle*)pObj;
		face_pick.Set(cut_face_dlg.m_fBasePosX,cut_face_dlg.m_fBasePosY,cut_face_dlg.m_fBasePosZ);
		face_norm.Set(cut_face_dlg.m_fFaceNormX,cut_face_dlg.m_fFaceNormY,cut_face_dlg.m_fFaceNormZ);
		face_pick+=face_norm*cut_face_dlg.m_fBaseNormOffsetDist;
		if(pRayJg->CalCutAngleInfo(face_pick,face_norm,NULL,nCutDist))
		{
			pRayJg->SetModified();
			pRayJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
			nCutCount++;
		}
	}
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("%d angles have cut-leg��",nCutCount);
#else 
	ss.Format("%d���Ǹֱ��нǻ���֫��",nCutCount);
#endif
	MessageBox(ss);
#endif
}

//<DEVELOP_PROCESS_MARK nameId="FUNC_ONEKEY_CUTANGLE">
#include "CutAngleCheckTypeDlg.h"
CCutJg cutJg;
//ʶ���ж�pRod1��pRod2�����Ӷˣ��й�ͬ�˽ڵ�ʱ����ֵΪ0,���򷵻ؾ�������Ķ˵����
double RodJunctionDiscern(CLDSLinePart* pRod1,CLDSLinePart* pRod2,char* cJunction,bool bOnlySameNode=false);

void GetShortestVerticalLineLength(double &fShortLength,f3dPoint prj_pt,f3dPoint face_pos,f3dPoint face_norm)
{
	f3dPoint pt=prj_pt;
	project_point(prj_pt,face_pos,face_norm);
	double dis = DISTANCE(prj_pt,pt);
	if(dis==0)
		return;
	if(dis < fShortLength)
		fShortLength = dis;
}

void GetFace(f3dPoint proj_pt,CLDSLineAngle *pAngle,UCS_STRU ucsArr[2])
{
	CXhPtrSet<f3dPoint>ptList;
	if(pAngle->IsEndCutAngleOrWing())
	{
		if(pAngle->cut_wing==0)
		{
			//X֫�н�
			if(pAngle->cut_angle[2][0]>0)
			{
				f3dPoint start = ucsArr[0].origin ;
				get_pt_by_vec(ucsArr[0].axis_x,ucsArr[0].axis_z,start,pAngle->GetLength(),pAngle->GetWidth()-pAngle->cut_angle[2][1]);
				ptList.append(&start);
				start = start + ucsArr[0].axis_y*pAngle->GetThick();
				ptList.append(&start);
				start = ucsArr[0].origin;
				get_pt_by_vec(ucsArr[0].axis_z,ucsArr[0].axis_x,start,pAngle->GetWidth(),pAngle->GetLength()-pAngle->cut_angle[2][0]);
				ptList.append(&start);
				start = start + ucsArr[0].axis_y*pAngle->GetThick();
				ptList.append(&start);
			}
			if(pAngle->cut_angle[0][0]>0)
			{
				f3dPoint start = ucsArr[0].origin ;
				start = start + ucsArr[0].axis_z* (pAngle->GetWidth() - pAngle->cut_angle[0][1]);
				ptList.append(&start);
				start = start + ucsArr[0].axis_y*pAngle->GetThick();
				ptList.append(&start);
				start = ucsArr[0].origin;
				get_pt_by_vec(ucsArr[0].axis_x,ucsArr[0].axis_z,start,pAngle->cut_angle[0][0],pAngle->GetWidth());
				ptList.append(&start);
				start = start + ucsArr[0].axis_y*pAngle->GetThick();
				ptList.append(&start);
			}
		}
		//TODO: δ��ɡ�wjh-2015.4.14
		/*else if(pAngle->cut_wing==1)
		{

		}*/
	}
}

double GetShortestLength(f3dPoint proj_pt,CLDSLineAngle *pAngle,UCS_STRU ucs)
{
	//ע�⣺����û�п��ǲ��ȱ߽Ǹ�
	double shortLength = 10000;
	GetShortestVerticalLineLength(shortLength,proj_pt,ucs.origin,ucs.axis_z);							    //��
	GetShortestVerticalLineLength(shortLength,proj_pt,ucs.origin+ucs.axis_z*pAngle->GetThick() ,ucs.axis_z);//��
	GetShortestVerticalLineLength(shortLength,proj_pt,ucs.origin,ucs.axis_y);							    //��
	GetShortestVerticalLineLength(shortLength,proj_pt,ucs.origin+ucs.axis_y*pAngle->GetWidth() ,ucs.axis_y);//ǰ
	GetShortestVerticalLineLength(shortLength,proj_pt,ucs.origin,ucs.axis_x);								//��
	GetShortestVerticalLineLength(shortLength,proj_pt,ucs.origin+ucs.origin*pAngle->GetLength(),ucs.axis_x);//��									//��
	return shortLength;
}
void ANGLECUT_ADVICE::Init(CLDSLineAngle* pAngle)
{
	if(pAngle==NULL)
	{
		Clear();
		return;
	}
	if(pAngle->cut_wing[0]==1||pAngle->cut_wing[0]==2)
		start.cCutWing='X'+(char)pAngle->cut_wing[0]-1;
	else
		start.cCutWing=0;
	if(pAngle->cut_wing[1]==1||pAngle->cut_wing[1]==2)
		end.cCutWing='X'+(char)pAngle->cut_wing[1]-1;
	else
		end.cCutWing=0;
	start.wing.fCutBerLength=pAngle->cut_wing_para[0][0];
	start.wing.fCutWidth=pAngle->cut_wing_para[0][1];
	start.wing.fCutWingLength=pAngle->cut_wing_para[1][2];
	end.wing.fCutBerLength=pAngle->cut_wing_para[1][0];
	end.wing.fCutWidth=pAngle->cut_wing_para[1][1];
	end.wing.fCutWingLength=pAngle->cut_wing_para[1][2];

	start.corner[0].fLength=pAngle->cut_angle[0][0];
	start.corner[0].fWidth=pAngle->cut_angle[0][1];
	start.corner[1].fLength=pAngle->cut_angle[1][0];
	start.corner[1].fWidth=pAngle->cut_angle[1][1];
	end.corner[0].fLength=pAngle->cut_angle[2][0];
	end.corner[0].fWidth=pAngle->cut_angle[2][1];
	end.corner[1].fLength=pAngle->cut_angle[3][0];
	end.corner[1].fWidth=pAngle->cut_angle[3][1];
}
void ANGLECUT_ADVICE::MergeCutting(const ANGLECUT_ADVICE& advice,CLDSLineAngle* pAngle)
{
	long hAngle=pAngle?pAngle->handle:0;
	if(bCutStart)
	{
		for(int i=0;i<2;i++)
		{
			if(start.corner[i].fLength<advice.start.corner[i].fLength)
				start.corner[i].fLength=advice.start.corner[i].fLength;
			if(start.corner[i].fWidth<advice.start.corner[i].fWidth)
				start.corner[i].fWidth=advice.start.corner[i].fWidth;
			if(start.corner[i].fLength==0&&start.corner[i].fWidth>0)
				start.corner[i].fLength=start.corner[i].fWidth;
			if(start.corner[i].fLength>0&&start.corner[i].fWidth==0)
				start.corner[i].fWidth=start.corner[i].fLength;
		}
		if(advice.start.cCutWing>0)
		{
			if(start.cCutWing>0 && start.cCutWing!=advice.start.cCutWing)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Angle(0x%X)'s start cut wing data encounter a problem, please manually check",hAngle);
#else
				logerr.Log("0x%X�Ǹ�ʼ����֫�����ж��������⣬���ֶ���ʵ",hAngle);
#endif
			else
			{
				start.cCutWing=advice.start.cCutWing;
				if(start.wing.fCutWidth<advice.start.wing.fCutWidth)
					start.wing.fCutWidth=advice.start.wing.fCutWidth;
				if(start.wing.fCutBerLength<advice.start.wing.fCutBerLength)
					start.wing.fCutBerLength=advice.start.wing.fCutBerLength;
				if(start.wing.fCutWingLength<advice.start.wing.fCutWingLength)
					start.wing.fCutWingLength=advice.start.wing.fCutWingLength;
				if(start.cCutWing=='X'&&start.wing.fCutWingLength<start.corner[0].fLength)
					start.wing.fCutBerLength=start.wing.fCutWingLength=start.corner[0].fLength;
				else if(start.cCutWing=='Y'&&start.wing.fCutWingLength<start.corner[1].fLength)
					start.wing.fCutBerLength=start.wing.fCutWingLength=start.corner[1].fLength;
			}
		}
	}
	else
	{
		for(int i=0;i<2;i++)
		{
			if(end.corner[i].fLength<advice.end.corner[i].fLength)
				end.corner[i].fLength=advice.end.corner[i].fLength;
			if(end.corner[i].fWidth<advice.end.corner[i].fWidth)
				end.corner[i].fWidth=advice.end.corner[i].fWidth;
			if(end.corner[i].fLength==0&&end.corner[i].fWidth>0)
				end.corner[i].fLength=end.corner[i].fWidth;
			if(end.corner[i].fLength>0&&end.corner[i].fWidth==0)
				end.corner[i].fWidth=end.corner[i].fLength;
		}
		if(advice.end.cCutWing>0)
		{
			if(end.cCutWing>0 && end.cCutWing!=advice.end.cCutWing)
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Angle(0x%X)'s end cut wing data encounter a problem, please manually check",hAngle);
#else
				logerr.Log("0x%X�Ǹ��ն���֫�����ж��������⣬���ֶ���ʵ",hAngle);
#endif
			else
			{
				end.cCutWing=advice.end.cCutWing;
				if(end.wing.fCutWidth<advice.end.wing.fCutWidth)
					end.wing.fCutWidth=advice.end.wing.fCutWidth;
				if(end.wing.fCutBerLength<advice.end.wing.fCutBerLength)
					end.wing.fCutBerLength=advice.end.wing.fCutBerLength;
				if(end.wing.fCutWingLength<advice.end.wing.fCutWingLength)
					end.wing.fCutWingLength=advice.end.wing.fCutWingLength;
				if(end.cCutWing=='X'&&end.wing.fCutWingLength<end.corner[0].fLength)
					end.wing.fCutBerLength=end.wing.fCutWingLength=end.corner[0].fLength;
				else if(end.cCutWing=='Y'&&end.wing.fCutWingLength<end.corner[1].fLength)
					end.wing.fCutBerLength=end.wing.fCutWingLength=end.corner[1].fLength;
			}
		}
	}
}
/* ��ȡ��Ч���н�����
	A.����Ǹ�ͬһ֫���н�����֫�����
	B.���ƽǸ��н�����|��֫����
*/
void ANGLECUT_ADVICE::RetriveValidCutData()
{
	if(bCutStart)
	{
		if(start.corner[0].fWidth>0 && start.corner[0].fLength<=0)
			start.corner[0].fLength=start.corner[0].fWidth;
		else if(start.corner[0].fWidth<=0&&start.corner[0].fLength>0)
			start.corner[0].fWidth=start.corner[0].fLength;
		if(start.corner[1].fWidth>0 && start.corner[1].fLength<=0)
			start.corner[1].fLength=start.corner[1].fWidth;
		else if(start.corner[1].fWidth<=0&&start.corner[1].fLength>0)
			start.corner[1].fWidth=start.corner[1].fLength;
		if(start.cCutWing=='X')
		{
			if(start.corner[0].fLength>start.wing.fCutWingLength)
				start.wing.fCutWingLength=start.corner[0].fLength;
			start.corner[0].fLength=start.corner[0].fWidth=0;
		}
		else if(start.cCutWing=='Y')
		{
			if(start.corner[1].fLength>start.wing.fCutWingLength)
				start.wing.fCutWingLength=start.corner[1].fLength;
			start.corner[1].fLength=start.corner[1].fWidth=0;
		}
		if(start.cCutWing>0)
		{
			if(start.wing.fCutWingLength<=0)
				start.wing.fCutWingLength=start.wing.fCutWidth;
			if(start.wing.fCutBerLength<=0)
				start.wing.fCutBerLength=start.wing.fCutWidth;
		}
	}
	else
	{
		if(end.corner[0].fWidth>0 && end.corner[0].fLength<=0)
			end.corner[0].fLength=end.corner[0].fWidth;
		else if(end.corner[0].fWidth<=0&&end.corner[0].fLength>0)
			end.corner[0].fWidth=end.corner[0].fLength;
		if(end.corner[1].fWidth>0 && end.corner[1].fLength<=0)
			end.corner[1].fLength=end.corner[1].fWidth;
		else if(end.corner[1].fWidth<=0&&end.corner[1].fLength>0)
			end.corner[1].fWidth=end.corner[1].fLength;
		if(end.cCutWing=='X')
		{
			if(end.corner[0].fLength>end.wing.fCutWingLength)
				end.wing.fCutWingLength=end.corner[0].fLength;
			end.corner[0].fLength=end.corner[0].fWidth=0;
		}
		else if(end.cCutWing=='Y')
		{
			if(end.corner[1].fLength>end.wing.fCutWingLength)
				end.wing.fCutWingLength=end.corner[1].fLength;
			end.corner[1].fLength=end.corner[1].fWidth=0;
		}
		if(end.cCutWing>0)
		{
			if(end.wing.fCutWingLength<=0)
				end.wing.fCutWingLength=end.wing.fCutWidth;
			if(end.wing.fCutBerLength<=0)
				end.wing.fCutBerLength=end.wing.fCutWidth;
		}
	}
}
static bool IsOverlapped(double start1,double end1,double start2,double end2)
{
	if(end1<start2||start1>end2)
		return false;
	else
		return true;
}
static void CalPerfectCutParamWL(double r,double deita,double tipL,double* W,double* L)
{
	double W_arr[3],w_arr[3];	//��ǰ�����и��ȣ�Բ��λ�ü�ʵ��λ�ã�
	double L_arr[3],l_arr[3];	//��ǰ�н���һ�ߣ����ߣ�Բ��λ�ü�ʵ��λ�ã�
	double sina_arr[3]={0.5,0.707,0.866};
	double cosa_arr[3]={0.866,0.707,0.5};
	double perfect_area=0;
	int i,perfect;
	for(i=0;i<3;i++)
	{
		W_arr[i]=tipL+r/cosa_arr[i];
		L_arr[i]=W_arr[i]/sina_arr[i];
		w_arr[i]=W_arr[i]-deita*sina_arr[i]/cosa_arr[i];
		l_arr[i]=L_arr[i]-deita;
		double area=w_arr[i]*l_arr[i];
		if(i==0 || area<perfect_area)
		{
			perfect_area=area;
			perfect=i;
		}
	}
	if(W) *W=w_arr[i];
	if(L) *L=l_arr[i];
}
bool IsBoltAndAngleCollision(CLDSLineAngle *pAngle, CLDSBolt *pBolt,
	const BOLTSOLID_PARA& solidparam,const GECS& boxCSX,const GECS& boxCSY,
	ANGLECUT_ADVICE* pAdvice=NULL,double minClearance=5)
{
	double inner_r = FindJg_r(pAngle->GetWidth());
	double clearance=inner_r*pAngle->GetCutAngleCoefR();
	//1.�����˨Ϊ����Բ����
	BOLTSOLID_COLUMN bodyCol(solidparam.D/2+minClearance);	//����˨ͷ��������ñ����
	BOLTSOLID_COLUMN headCol(pBolt->get_d()/2+minClearance);	//����˨ñ��������˨�˶���
	bodyCol.vStdAxis=headCol.vStdAxis=pBolt->ucs.axis_z;
	bodyCol.start=pBolt->ucs.origin-pBolt->ucs.axis_z*(solidparam.H+minClearance);
	bodyCol.end=pBolt->ucs.origin+pBolt->ucs.axis_z*(pBolt->get_L0()+solidparam.H2);
	if(pBolt->IsTwoCap())
		bodyCol.end+=pBolt->ucs.axis_z*solidparam.H2;
	bodyCol.height=DISTANCE(bodyCol.start,bodyCol.end);
	headCol.start=bodyCol.end;
	headCol.end=pBolt->ucs.origin+pBolt->ucs.axis_z*(pBolt->get_L()+minClearance);
	headCol.height=DISTANCE(headCol.start,headCol.end);
	//2.�ж�Բ�����Ƿ���Ǹ���֫��ײ
	double xL=pAngle->GetLength();
	double yW=pAngle->GetWidth();
	double zT=pAngle->GetThick();
	GEPOINT inters,vertexArr[8];
	GECS colcs(bodyCol.start,pBolt->ucs.axis_x,pBolt->ucs.axis_y,pBolt->ucs.axis_z);
	bool onceInters=false;		//������˨�Ƿ��������ʼ�Ǹ�״̬�й���ײ�����������
	bool processInters=false;	//ÿ����ײ�нǼ���ѭ������ײ����״̬
	int i,j,k,iter=0;
	//do{
		processInters=false;	
		for(i=0;i<2;i++)
		{	//X\Y֫
			GECS boxcs = (i==0) ? boxCSX : boxCSY;
			GEPOINT xAxis=colcs.TransVToCS(boxcs.axis_x);
			GEPOINT yAxis=colcs.TransVToCS(boxcs.axis_y);
			GEPOINT zAxis=colcs.TransVToCS(boxcs.axis_z);
			double fLongRadius=solidparam.D/2;
			if(fabs(zAxis.z)>EPS)
				fLongRadius/=fabs(zAxis.z);
			for (j=0;j<2;j++)
			{	//��˨ͷ����˨��
				double dist=0,scaleInLineS2E=0;
				//double r= (j==0) ? solidparam.D/2 : solidparam.d/2 ;
				BOLTSOLID_COLUMN col = (j==0) ? bodyCol : headCol;
				double r=col.radius;
				colcs.origin=col.start;
				vertexArr[0]=colcs.TransPToCS(boxcs.origin);
				vertexArr[1]=vertexArr[0]+xAxis*xL;
				vertexArr[2]=vertexArr[1]+yAxis*yW;
				vertexArr[3]=vertexArr[0]+yAxis*yW;
				double lenArr[4]={xL,yW,xL,yW};
				for(k=0;k<4;k++)
					vertexArr[k+4]=vertexArr[k]+zAxis*zT;
				//�������Ͷ�����Ե�(�ĸ�)��������˨����Բ���潻��
				//vertexArr��ʱ�����Ϊ��˨����Բ���ֲ�����ϵ������ֵ
				bool hasValidInters=false;
				double inters_scale_arr[8]={2,-2,2,-2,2,-2,2,-2};
				GEPOINT inters_arr[8];
				for(int bi=0;bi<=4;bi+=4)
				{
					for(k=0;k<4;k++)
					{
						GEPOINT start=vertexArr[bi+k];
						GEPOINT end=vertexArr[bi+(k+1)%4];
						if(start.z<0&&end.z<0||start.z>col.height&&end.z>col.height)
							continue;
						GEPOINT lineStdVec=(end-start)/lenArr[k];
						//normalize(lineStdVec);
						dist=fabs(DistOf2dPtLine(0,0,start.x,start.y,end.x,end.y,&scaleInLineS2E));
						if(dist>r+EPS)	//����Բ����֮�⣬�������н���
							continue;
						else if(dist>r-EPS&&(scaleInLineS2E<-EPS||scaleInLineS2E>1+EPS))
							continue;	//Ψһ��������������߶���Ч��֮��
						inters=start+(end-start)*scaleInLineS2E;
						double L2Start=lenArr[k]*scaleInLineS2E;
						double half_prj_arc = SQRT(r*r-dist*dist);
						double cosa=SQRT(lineStdVec.x*lineStdVec.x+lineStdVec.y*lineStdVec.y);
						double offset0 = cosa>EPS?half_prj_arc/cosa:0;//cosa=fabs(lineStdVec.z)
						half_prj_arc=max(half_prj_arc,(r-dist*0.5)/0.866);	//�Ŵ��Ա���������ɱ�����
						half_prj_arc=max(half_prj_arc,r*1.414-dist);	//�Ŵ��Ա���������ɱ�����
						half_prj_arc=max(half_prj_arc,(r-dist*0.866)/0.5);	//�Ŵ��Ա���������ɱ�����
						double offset = cosa>EPS?half_prj_arc/cosa:0;//cosa=fabs(lineStdVec.z)
						//�ж��Ƿ��н��㲻��ʹ�Ŵ���и�㣬��Ӧʹ��ԭʼ���㣬������������нǽ�Сʱ��������� wjh-2016.6.17
						if(L2Start+offset0<=-EPS||L2Start-offset0>=lenArr[k]+EPS)
							continue;
						if(L2Start-offset>=-EPS)
						{
							inters_arr[k*2]=start+lineStdVec*(L2Start-offset);
							hasValidInters|=inters_arr[k*2].z>0&&inters_arr[k*2].z<col.height;
							if(hasValidInters&&pAdvice==NULL)
								break;
							else if(pAdvice&&inters_arr[k*2].z>0&&inters_arr[k*2].z<col.height)
							{
								//double W,L;
								//CalPerfectCutParamWL(col.radius,dist,lenArr[k]-L2Start,&W,&L);
								//���㵱ǰ�ཻ���ϵ��и��
								double scale=(L2Start-offset)/lenArr[k];
								if(scale<inters_scale_arr[k*2])
									inters_scale_arr[k*2]=scale;
							}
						}
						else if(L2Start+offset>0&&L2Start-offset<0)
						{
							hasValidInters=true;
							if(hasValidInters&&pAdvice==NULL)
								break;
							else if(pAdvice)
								inters_scale_arr[k*2]=0;
						}
						if(L2Start+offset<=lenArr[k]+EPS)
						{
							inters_arr[k*2+1]=start+lineStdVec*(L2Start+offset);
							hasValidInters|=inters_arr[k*2+1].z>0&&inters_arr[k*2+1].z<col.height;
							if(hasValidInters&&pAdvice==NULL)
								break;
							else if(pAdvice&&inters_arr[k*2+1].z>0&&inters_arr[k*2+1].z<col.height)
							{
								double scale=(L2Start+offset)/lenArr[k];
								if(scale>inters_scale_arr[k*2+1])
									inters_scale_arr[k*2+1]=scale;
							}
						}
						else if(L2Start-offset<lenArr[k]&&L2Start+offset>lenArr[k])
						{
							hasValidInters=true;
							if(hasValidInters&&pAdvice==NULL)
								break;
							else if(pAdvice)
								inters_scale_arr[k*2+1]=1.0;
						}
					}
					if(hasValidInters&&pAdvice==NULL)
						break;
				}
				if(hasValidInters)
				{	//�ҵ���Ч����
					if(pAdvice==NULL)
						return true;
					ANGLECUT_ADVICE advice;
					advice.Init(pAngle);
					//TODO:1.����inters_scale_arr��inters_arr����ǡ�����н�ֵ
					//D--------<----------C
					//|					  |
					//|___________________|
					//A-------->----------B
					//A->Bʼ->�����߱�
					if(inters_scale_arr[0+1]>0&&inters_scale_arr[0+1]<0.5)
					{
						advice.start.cCutWing='X'+i;
						advice.start.wing.fCutBerLength=inters_scale_arr[1]*xL;
					}
					else if(inters_scale_arr[0]>0.5&&inters_scale_arr[0]<1)
					{
						advice.start.cCutWing='X'+i;
						advice.end.wing.fCutBerLength=xL-inters_scale_arr[0]*xL;
					}
					//B->C�ն���Ե��
					if( inters_scale_arr[2]>-EPS&&inters_scale_arr[2]<clearance/yW||
						inters_scale_arr[2]<-EPS&&inters_scale_arr[3]>EPS)
					{
						if(advice.end.corner[i].fWidth>0||inters_scale_arr[3]>0.6)
						{	//��֫�������κμ�������Ŀǰֻ����ʱ����˨ֱ��Ϊ��֫��Ⱦ���ֵ wjh-2016.6.17
							advice.end.cCutWing='X'+i;
							advice.end.wing.fCutWingLength=advice.end.wing.fCutBerLength=solidparam.d;
							advice.end.wing.fCutWidth=pAngle->GetThick()+inner_r;
						}
						else
						{
							advice.end.cCutWing='X'-i+1;
							if(inters_scale_arr[3]<1)
								advice.end.wing.fCutWidth=inters_scale_arr[3]*yW;
							else if(inters_scale_arr[2]>-EPS)
								advice.end.wing.fCutWidth=inters_scale_arr[2]*yW;
						}
					}
					else if(inters_scale_arr[2]>0&&inters_scale_arr[2]<1)
						advice.end.corner[i].fWidth=(1-inters_scale_arr[2])*yW;
					//C-D
					if(inters_scale_arr[5]>0&&inters_scale_arr[5]<0.5)
						advice.end.corner[i].fLength=inters_scale_arr[5]*xL;
					else if(inters_scale_arr[4]<1&&inters_scale_arr[4]>0.5)
						advice.start.corner[i].fLength=xL-inters_scale_arr[4]*xL;
					//D-A
					double cutRCoef=1-inner_r/yW;
					if(inters_scale_arr[7]>0&&inters_scale_arr[7]*yW<yW-inner_r)
						advice.start.corner[i].fWidth=inters_scale_arr[7]*yW;
					else if(inters_scale_arr[7]>cutRCoef&&inters_scale_arr[6]<0.4)
					{
						advice.start.cCutWing='X'+i;
						advice.start.wing.fCutWingLength=advice.start.wing.fCutBerLength=solidparam.d;
						advice.start.wing.fCutWidth=pAngle->GetThick()+inner_r;
					}
					else if(inters_scale_arr[7]>cutRCoef&&inters_scale_arr[6]<1.0&&inters_scale_arr[6]>0.4)
					{
						advice.start.cCutWing='X'-i+1;
						advice.start.wing.fCutWingLength=advice.start.wing.fCutBerLength=solidparam.d;
						advice.start.wing.fCutWidth=yW-inters_scale_arr[6]*yW;
					}
					pAdvice->MergeCutting(advice,pAngle);
					//���ͬʱ����֫������ײ���������֫�������һ֫��ײֻ�н�
				}
				else
				{	//֫���������
					for(k=0;k<4;k++)
					{
						GEPOINT start=vertexArr[k];
						GEPOINT end=vertexArr[k+4];
						if(start.z<0&&end.z<0||start.z>col.height&&end.z>col.height)
							continue;
						GEPOINT lineStdVec=(end-start)/zT;
						normalize(lineStdVec);
						dist=fabs(DistOf2dPtLine(0,0,start.x,start.y,end.x,end.y,&scaleInLineS2E));
						if(dist>r+EPS)	//����Բ����֮�⣬�������н���
							continue;
						else if(dist>r-EPS&&(scaleInLineS2E<-EPS||scaleInLineS2E>1+EPS))
							continue;	//Ψһ��������������߶���Ч��֮��
						inters=start+(end-start)*scaleInLineS2E;
						double L2Start=zT*scaleInLineS2E;
						double half_prj_arc = SQRT(r*r-dist*dist);
						double cosa=SQRT(lineStdVec.x*lineStdVec.x+lineStdVec.y*lineStdVec.y);
						double offset = cosa>EPS?half_prj_arc/cosa:0;//cosa=fabs(lineStdVec.z)
						hasValidInters=IsOverlapped(-EPS,zT+EPS,L2Start-offset,L2Start+offset);
						if(hasValidInters)
							break;
					}
					if(hasValidInters)
					{	//�ҵ���Ч����
						if(pAdvice==NULL)
							return true;
						ANGLECUT_ADVICE advice;
						advice.Clear();
						if(k==0)
						{
							if(i==0)
								advice.start.cCutWing='Y';
							else
								advice.start.cCutWing='X';
							advice.start.wing.fCutWidth=pAngle->GetThick();
						}
						else if(k==1)
						{	
							if(i==0)
								advice.end.cCutWing='Y';
							else
								advice.end.cCutWing='X';
							advice.end.wing.fCutWidth=pAngle->GetThick();
						}
						else if(k==2)
							advice.end.corner[i].fLength=advice.end.corner[i].fWidth=pAngle->GetThick();
						else if(k==3)
							advice.start.corner[i].fLength=advice.end.corner[i].fWidth=pAngle->GetThick();
						pAdvice->MergeCutting(advice,pAngle);
					}
				}
				processInters|=hasValidInters;
			}
			if(!processInters)
			{	//Բ���κ�һֱ�߱߾�����Ч���㣬��ֱ�����������������󽻼���
				//X-Yƽ��(z=0)
				GEPOINT intersZ[2],intersX[2],intersY[2];
				bool hasValidInters=false,validIntersZ[2]={false},validIntersX[2]={false},validIntersY[2]={false};
				BOLTSOLID_COLUMN col = bodyCol;
				GEPOINT colstart=boxcs.TransPToCS(bodyCol.start);
				GEPOINT colAxisVec=boxcs.TransVToCS(col.vStdAxis);
				if(Int3dlf(intersZ[0],colstart,colAxisVec,GEPOINT(0,0,0),GEPOINT(0,0,1))==1)
				{
					double zPosOffset=(intersZ[0]-colstart)*colAxisVec;
					bool isInBolt=zPosOffset>0&&zPosOffset<(bodyCol.height+headCol.height);
					validIntersZ[0]=isInBolt&&intersZ[0].x>0&&intersZ[0].x<xL&&intersZ[0].y>0&&intersZ[0].y<yW;
					hasValidInters|=validIntersZ[0];
				}
				if(pAdvice==NULL&&hasValidInters)
					return true;
				if(Int3dlf(intersZ[1],colstart,colAxisVec,GEPOINT(0,0,zT),GEPOINT(0,0,1))==1)
				{
					double zPosOffset=(intersZ[1]-colstart)*colAxisVec;
					bool isInBolt=zPosOffset>0&&zPosOffset<(bodyCol.height+headCol.height);
					validIntersZ[1]=isInBolt&&intersZ[1].x>0&&intersZ[1].x<xL&&intersZ[1].y>0&&intersZ[1].y<yW;
					hasValidInters|=validIntersZ[1];
				}
				if(pAdvice==NULL&&hasValidInters)
					return true;
				if(Int3dlf(intersX[0],colstart,colAxisVec,GEPOINT(0,0,0),GEPOINT(1,0,0))==1)
				{
					double zPosOffset=(intersX[0]-colstart)*colAxisVec;
					bool isInBolt=zPosOffset>0&&zPosOffset<(bodyCol.height+headCol.height);
					validIntersX[0]=isInBolt&&intersX[0].z>0&&intersX[0].z<zT&&intersX[0].y>0&&intersX[0].y<yW;
					hasValidInters|=validIntersX[0];
				}
				if(pAdvice==NULL&&hasValidInters)
					return true;
				if(Int3dlf(intersX[1],colstart,colAxisVec,GEPOINT(xL,0,0),GEPOINT(1,0,0))==1)
				{
					double zPosOffset=(intersX[1]-colstart)*colAxisVec;
					bool isInBolt=zPosOffset>0&&zPosOffset<(bodyCol.height+headCol.height);
					validIntersX[1]=isInBolt&&intersX[1].z>0&&intersX[1].z<zT&&intersX[1].y>0&&intersX[1].y<yW;
					hasValidInters|=validIntersX[1];
				}
				if(pAdvice==NULL&&hasValidInters)
					return true;
				if(Int3dlf(intersY[0],colstart,colAxisVec,GEPOINT(0,0,0),GEPOINT(0,1,0))==1)
				{
					double zPosOffset=(intersY[0]-colstart)*colAxisVec;
					bool isInBolt=zPosOffset>0&&zPosOffset<(bodyCol.height+headCol.height);
					validIntersY[0]=isInBolt&&intersY[0].x>0&&intersY[0].x<xL&&intersY[0].z>0&&intersY[0].z<zT;
					hasValidInters|=validIntersY[0];
				}
				if(pAdvice==NULL&&hasValidInters)
					return true;
				if(Int3dlf(intersY[1],colstart,colAxisVec,GEPOINT(0,yW,0),GEPOINT(0,1,0))==1)
				{
					double zPosOffset=(intersY[1]-colstart)*colAxisVec;
					bool isInBolt=zPosOffset>0&&zPosOffset<(bodyCol.height+headCol.height);
					validIntersY[1]=isInBolt&&intersY[1].x>0&&intersY[1].x<xL&&intersY[1].z>0&&intersY[1].z<zT;
					hasValidInters|=validIntersY[1];
				}
				if(pAdvice==NULL&&hasValidInters)
					return true;
				ANGLECUT_ADVICE advice;
				for(int index=0;index<2;index++)
				{
					if(validIntersZ[index])
					{
						advice.Clear();
						boxcs.TransPToCS(intersZ[index]);
						if(intersZ[index].y>0&&intersZ[index].y<pAngle->GetWidth())
						{
							if(intersZ[index].y>clearance)	//�н�
								advice.start.corner[i].fWidth=advice.start.corner[i].fLength=intersZ[index].x+yW-intersZ[index].y+fLongRadius*1.5;
							else	//��֫
							{
								advice.start.cCutWing='X'+1-i;
								advice.start.wing.fCutWidth=advice.start.wing.fCutBerLength=advice.start.wing.fCutWingLength=intersZ[index].y+fLongRadius*1.5;
							}
						}
						else
							continue;
						pAdvice->MergeCutting(advice,pAngle);
					}
					//if(validIntersY[index])
					//{
					//	boxcs.TransPToCS(intersZ[index]);
					//	if(intersZ[index].x<pAngle->GetWidth()*2)
					//	{
					//		if(intersY[index].y>clearance)	//�н�
					//			advice.start.corner[i].fWidth=advice.start.corner[i].fLength=intersZ[index].x+yW-intersZ[index].y+fLongRadius;
					//		else	//��֫
					//		{
					//			advice.start.cCutWing='X'+1-i;
					//			advice.start.wing.fCutWidth=advice.start.wing.fCutBerLength=advice.start.wing.fCutWingLength=intersZ[index].y+fLongRadius;
					//		}
					//	}
					//}
				}
				//TODO:3.intersZ[0]
				//���ͬʱ����֫������ײ���������֫�������һ֫��ײֻ�н�
				processInters|=hasValidInters;
			}
		}
		onceInters|=processInters;
		if(processInters)
			iter++;
	//}while(processInters&&iter<3);
	return onceInters;
}
ARRAY_LIST<BOLTSOLID_PARA> BOLTSOLID_PARA::BOLT_LIBRARY;
BOLTSOLID_PARA::BOLTSOLID_PARA(short boltd/*=0*/,double _D/*=0*/,double _H/*=0*/,double _H2/*=0*/)
{
	d =boltd;
	D =_D;
	H =_H;
	H2=_H2;
}
bool BOLTSOLID_PARA::QuerySolidParams(short boltd,BOLTSOLID_PARA* pBoltSolidParams)
{
	if(boltd<=0)
		return false;
	BOLTSOLID_PARA solidparam(boltd);
	int count=BOLT_LIBRARY.GetSize();
	for(int i=0;i<count;i++)
	{
		if(BOLT_LIBRARY[i].d==boltd)
		{
			if(pBoltSolidParams)
				*pBoltSolidParams=BOLT_LIBRARY[i];
			return true;
		}
		else if(BOLT_LIBRARY[i].d<boltd)
			continue;
		FindLsD_H(boltd,&solidparam.D,&solidparam.H,&solidparam.H2);
		if(pBoltSolidParams)
			*pBoltSolidParams=solidparam;
		if(BOLT_LIBRARY[i].d<=0)
			BOLT_LIBRARY[i]=solidparam;
		if(BOLT_LIBRARY[i].d>boltd)
		{
			BOLT_LIBRARY.SetSize(count+1,10);
			memmove(&BOLT_LIBRARY[i+1],&BOLT_LIBRARY[i],(count-i)*sizeof(BOLTSOLID_PARA));
			BOLT_LIBRARY[i]=solidparam;
		}
		return true;
	}
	BOLT_LIBRARY.SetSize(count+1,10);
	FindLsD_H(boltd,&solidparam.D,&solidparam.H,&solidparam.H2);
	BOLT_LIBRARY[count]=solidparam;
	if(pBoltSolidParams)
		*pBoltSolidParams=solidparam;
	return true;
}
//cFlag&0x01��ͬ�˵�ԵȽǸ��нǣ�cFlag&0x02��������ӽǸ��нǣ�cFlag&0x04���Ǹ�����˨�н�void InitOneKeyCutJgInfo(ATOM_LIST<ANGLE_CUT_INFO>& cutJgInfoSet,BYTE cFlag)
void InitOneKeyCutJgInfo(ATOM_LIST<ANGLE_CUT_INFO>& angleCutsList,BYTE cFlag,double minClearance=5,bool bIncBoltInterfere=true)
{
	angleCutsList.Empty();
	if(cFlag&0x01 || cFlag&0x02)
	{	//�ж����Ǹ��Ƿ��й�ͬ�˽ڵ㣨���У�
		for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
		{
			if(pAngle->m_bVirtualPart || pAngle->pStart==NULL || pAngle->pEnd==NULL)
				continue;
			CLdsPartListStack stack_p(&Ta.Parts);
			for(CLDSLineAngle* pOthAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE);pOthAngle;pOthAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
			{
				if(pOthAngle->m_bVirtualPart || pOthAngle->pStart==NULL || pOthAngle->pEnd==NULL)
					continue;
				if(!pAngle->ModuleTogetherWith(pOthAngle))
					continue;	//����ͬһģ����
				char junction=0;
				double threshold=pAngle->GetWidth()+pOthAngle->GetWidth()+g_sysPara.VertexDist;
				bool valid_angles=false;
				if(RodJunctionDiscern(pAngle,pOthAngle,&junction)<threshold)	
					valid_angles=true;	//�й�ͬ�˽ڵ�(�ԵȽǸֺ������Ǹ�)
				else if(cFlag&0x02 && CCutJg::IsMainAndRayRelation(pAngle,pOthAngle))	//�ж��Ƿ�Ϊ�����Ǹ�
					valid_angles=true;
				if(valid_angles)
				{	//��Ӽ�¼
					ANGLE_CUT_INFO jgCutInfo;
					jgCutInfo.hPart1=pAngle->handle;
					jgCutInfo.hPart2=pOthAngle->handle;
					jgCutInfo.iSeg=pAngle->iSeg;
					angleCutsList.append(jgCutInfo);
				}
			}
		}
	}
	if(cFlag&0x04)
	{	//�Ǹ�����˨
		double clearance=minClearance;
		if(!bIncBoltInterfere)
			clearance=0;
		CLDSBolt* pBolt;
		for(pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
			pBolt->GetUCS();	//��ʼ����˨����ϵ�Է�������ϵ��������ײ����ʧ��
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(0,"Initializing cut-leg information automatically!");
#else 
		DisplayProcess(0,"���ڳ�ʼ���Ǹ�����˨���н���Ϣ!");
#endif
		int i=0,n=0;
		for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
		{
			if(pAngle->m_bVirtualPart || pAngle->pStart==NULL || pAngle->pEnd==NULL)
				continue;
			n++;
		}
		for(CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pAngle;pAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
		{
			if(pAngle->m_bVirtualPart || pAngle->pStart==NULL || pAngle->pEnd==NULL)
				continue;
			i++;
#ifdef AFX_TARG_ENU_ENGLISH
			DisplayProcess((100*i)/n,"Initializing cut-leg information automatically!");
#else 
			DisplayProcess((100*i)/n,"���ڳ�ʼ���Ǹ�����˨���н���Ϣ!");
#endif
			f3dPoint lenStdVec=pAngle->End()-pAngle->Start();
			normalize(lenStdVec);
			if(CLDSPart::CustomerSerial==CUSTOMIZE::CLIENT_GUANGZHOU_ZENGLI)
			{
				if(pAngle->size_wide>=160&&clearance<10)
					clearance=10;
				else
					clearance=minClearance;
			}

			CLdsPartListStack stack_p(&Ta.Parts);
			
			GECS anglecs,anglecs2;
			GEPOINT vWingVecX=pAngle->GetWingVecX();
			GEPOINT vWingVecY=pAngle->GetWingVecY();
			anglecs2.origin=anglecs.origin=pAngle->Start()-lenStdVec*pAngle->startOdd();
			anglecs2.axis_x=anglecs.axis_x=lenStdVec;
			anglecs.axis_y=vWingVecX;
			anglecs.axis_z=-pAngle->vxWingNorm;
			anglecs2.axis_y=vWingVecY;
			anglecs2.axis_z=-pAngle->vyWingNorm;
			SCOPE_STRU scope_angle;
			//����Z���귶Χ
			scope_angle.fMinX=0;
			scope_angle.fMaxX=pAngle->GetLength();
			//����X���귶Χ
			GEPOINT vWingTipY1=vWingVecY*pAngle->GetWidth();
			GEPOINT vWingTipY2=vWingTipY1-pAngle->vyWingNorm*pAngle->GetThick();
			scope_angle.fMinZ=min(scope_angle.fMinX,vWingTipY1*anglecs.axis_z);
			scope_angle.fMaxZ=pAngle->GetWidth();
			//����Y���귶Χ
			scope_angle.fMinY=0;
			scope_angle.fMaxY=pAngle->GetWidth();
			scope_angle.fMaxY=max(scope_angle.fMaxY,vWingTipY1*anglecs.axis_y);
			scope_angle.fMaxY=max(scope_angle.fMaxY,vWingTipY2*anglecs.axis_y);
			//������������϶
			scope_angle.fMinX-=clearance;
			scope_angle.fMinY-=clearance;
			scope_angle.fMinZ-=clearance;
			scope_angle.fMaxX+=clearance;
			scope_angle.fMaxY+=clearance;
			scope_angle.fMaxZ+=clearance;

			for(CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
			{
				if(pBolt->m_bVirtualPart || pAngle->FindLsByHandle(pBolt->handle)!=NULL)
					continue;	//������˨�׻򣬵�ǰ�Ǹ��ϵĴ�����˨
				if(!pAngle->ModuleTogetherWith(pBolt))
					continue;
				BOLTSOLID_PARA solidparam;
				BOLTSOLID_PARA::QuerySolidParams((short)pBolt->get_d(),&solidparam);
				GEPOINT origin=anglecs.TransPToCS(pBolt->ucs.origin);
				GEPOINT worknorm=anglecs.TransVToCS(pBolt->ucs.axis_z);
				SCOPE_STRU scope_bolt;
				scope_bolt.VerifyVertex(origin-worknorm*solidparam.H);
				scope_bolt.VerifyVertex(origin+worknorm*pBolt->get_L());
				double R=solidparam.D/2;
				scope_bolt.fMinX-=R;
				scope_bolt.fMinY-=R;
				scope_bolt.fMinZ-=R;
				scope_bolt.fMaxX+=R;
				scope_bolt.fMaxY+=R;
				scope_bolt.fMaxZ+=R;
				if(!scope_angle.IsIntersWith(scope_bolt))
					continue;	//����������ʵ��ռ����û�н���
				//���нǸ�����˨��ȷ��ײ�ж� wjh-2016.5.11
				ANGLE_CUT_INFO anglecut;
				anglecut.cut_para.CUT_BY_BOLT.Clear();
				if(DISTANCE(pAngle->Start(),pBolt->ucs.origin)<DISTANCE(pAngle->End(),pBolt->ucs.origin))
					anglecut.cut_para.CUT_BY_BOLT.bCutStart=TRUE;
				else
					anglecut.cut_para.CUT_BY_BOLT.bCutStart=FALSE;
				if(IsBoltAndAngleCollision(pAngle,pBolt,solidparam,anglecs,anglecs2,&anglecut.cut_para.CUT_BY_BOLT,clearance))
				{
					anglecut.hPart1=pAngle->handle;
					anglecut.hPart2=pBolt->handle;
					anglecut.iSeg=pAngle->iSeg;
					anglecut.cRelationType=ANGLE_CUT_INFO::RELATION_ANGLE_BOLT;
					anglecut.cCutMode=ANGLE_CUT_INFO::CUT_RAYJG_BY_BOLT;
					anglecut.cut_para.CUT_BY_BOLT.RetriveValidCutData();
					angleCutsList.append(anglecut);
				}
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(100,"Initializing cut-leg information automatically!");
#else 
		DisplayProcess(100,"���ڳ�ʼ���Ǹ��Զ��н���Ϣ!");
#endif
	}
	//
	int i=0,n=angleCutsList.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"Initializing cut-leg information automatically!");
#else 
	DisplayProcess(0,"���ڹ��˽Ǹ��Զ��н���Ϣ!");
#endif
	for(ANGLE_CUT_INFO* pJgCutInfo=angleCutsList.GetFirst();pJgCutInfo;pJgCutInfo=angleCutsList.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess((100*i)/n,"Initializing cut-leg information automatically!");
#else 
		DisplayProcess((100*i)/n,"���ڹ��˽Ǹ��Զ��н���Ϣ!");
#endif
		if(pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_ANGLE_BOLT)
		{	//�Ǹ�����˨֮����нǼ����Ҫ�����н����ݱȶ�
			if(!CCutJg::CutDataCheck(pJgCutInfo))
				angleCutsList.DeleteCursor();
		}
		else
		{	//�Ǹ���Ǹ�֮����нǼ����Ҫ������Ч��ײ���
			if(!CCutJg::CollisionCheck(pJgCutInfo,cFlag))
				angleCutsList.DeleteCursor();
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"Initializing cut-leg information automatically!");
#else 
	DisplayProcess(100,"���ڹ��˽Ǹ��Զ��н���Ϣ!");
#endif
	angleCutsList.Clean();
}
//��ʼ���нǼ�����ײ������
f3dPoint InitCollisionPt(ANGLE_CUT_INFO* pJgCutInfo)
{
	f3dPoint collisionPt;
	if(pJgCutInfo==NULL)
		return collisionPt;
	if(pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_MAINJG_RAYJG)
	{
		CLDSLineAngle* pJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->relation.MAIN_RAY.hMainJg);
		CLDSLineAngle* pJg2=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->relation.MAIN_RAY.hRayJg);
		if(pJg2->desStartPos.datum_jg_h==pJg1->handle)
			collisionPt=pJg2->pStart->Position();
		else if(pJg2->desEndPos.datum_jg_h==pJg1->handle)
			collisionPt=pJg2->pStart->Position();
		else if(pJg2->pStart->hFatherPart==pJg1->handle)
			collisionPt=pJg2->pStart->Position();
		else if(pJg2->pEnd->hFatherPart==pJg1->handle)
			collisionPt=pJg2->pEnd->Position();
	}
	else if(pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_RAYJG_RAYJG)
	{
		CLDSLineAngle* pJg1=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->hPart1);
		CLDSLineAngle* pJg2=(CLDSLineAngle*)Ta.FromPartHandle(pJgCutInfo->hPart2);
		if(pJg1->pStart->handle==pJg2->pStart->handle ||
			pJg1->pStart->handle==pJg2->pEnd->handle)
			collisionPt=pJg1->pStart->Position();
		else if(pJg1->pEnd->handle==pJg2->pStart->handle ||
			pJg1->pEnd->handle==pJg2->pEnd->handle)
			collisionPt=pJg1->pEnd->Position();
		else if(pJg1->pStart->hFatherPart==pJg2->pStart->hFatherPart||
			pJg1->pStart->hFatherPart==pJg2->pEnd->hFatherPart)
			collisionPt=pJg1->pStart->Position();
		else if(pJg1->pEnd->hFatherPart==pJg2->pStart->hFatherPart||
			pJg1->pEnd->hFatherPart==pJg2->pEnd->hFatherPart)
			collisionPt=pJg1->pEnd->Position();
	}
	else if(pJgCutInfo->cRelationType==ANGLE_CUT_INFO::RELATION_ANGLE_BOLT)
	{
		CLDSBolt* pBolt=(CLDSBolt*)Ta.FromPartHandle(pJgCutInfo->hPart2);
		if(pBolt)
			collisionPt=pBolt->ucs.origin;
	}
	return collisionPt;
}
static int compare_func1(const ANGLE_CUT_INFO &pObj1,const ANGLE_CUT_INFO &pObj2)
{
	return compare_int(pObj1.iSeg.iSeg,pObj2.iSeg.iSeg);
}
//һ���нǼ��
void CLDSView::OnOneKeyCutAngleCheck()
{
	//ȷ��һ���нǼ������	0x01��ͬ�˵����߽Ǹ��нǣ�0x02��������ӽǸ��нǣ�0x04���Ǹ�����˨�н�
	cutJg.Empty();
	BYTE cFlag=0;
	static CCutAngleCheckTypeDlg cutJgCheckTypeDlg;
	if(cutJgCheckTypeDlg.DoModal()!=IDOK)
		return;
	if(cutJgCheckTypeDlg.m_bCheckRayAndRay)
		cFlag|=0x01;
	if(cutJgCheckTypeDlg.m_bCheckMainAndRay)
		cFlag|=0x02;
	if(cutJgCheckTypeDlg.m_bCheckAngleAndBolt)
		cFlag|=0x04;
	if(cFlag==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Don't choose the one-key check type of corner cut");
#else 
		AfxMessageBox("û��ѡ��һ���нǼ������");
#endif
		return;
	}
	//��ʼ����Ҫ�����нǵĽǸ���Ϣ
	CLogErrorLife loglife;
	ATOM_LIST<ANGLE_CUT_INFO> angleCutsList;
	InitOneKeyCutJgInfo(angleCutsList,cFlag,g_sysPara.VertexDist,cutJgCheckTypeDlg.m_iBoltDetectType!=0);
	if(angleCutsList.GetNodeNum()<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("There is no angle that need to be cutted!");
#else
		AfxMessageBox("û����Ҫ�����нǵĽǸ�!");
#endif
		return;
	}
	//���и���Ϣ���նκŽ�������
	ARRAY_LIST<ANGLE_CUT_INFO> angleCutArr;
	angleCutArr.SetSize(0,angleCutsList.GetNodeNum());
	for(ANGLE_CUT_INFO* pJgCutInfo=angleCutsList.GetFirst();pJgCutInfo;pJgCutInfo=angleCutsList.GetNext())
		angleCutArr.append(*pJgCutInfo);
	CQuickSort<ANGLE_CUT_INFO>::QuickSort(angleCutArr.m_pData,angleCutArr.GetSize(),compare_func1);
	//�ԽǸֽ����нǼ��
	long iKey=0;
	cutJg.hashAngleCuts.Empty();
	CTaskOutputList infoList;
	for(ANGLE_CUT_INFO* pJgCutInfo=angleCutArr.GetFirst();pJgCutInfo;pJgCutInfo=angleCutArr.GetNext())
	{
		//��pJgCutInfo���浽��ϣ����cugJg.cugJgHashList�У�����������������Զ��нǵ�ʵ�֣���֤����ͬ��
		iKey++;
		cutJg.hashAngleCuts.SetValue(iKey,*pJgCutInfo);
		CLDSPart* pPart1=Ta.Parts.FromHandle(pJgCutInfo->hPart1);
		CLDSPart* pPart2=Ta.Parts.FromHandle(pJgCutInfo->hPart2);
		CXhChar16 sPartType1,sPartType2,sPartNo1,sPartNo2;
		if(pPart1&&pPart1->GetClassTypeId()==CLS_LINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sPartType1.Copy("Angle");
#else
			sPartType1.Copy("�Ǹ�");
#endif
			sPartNo1=((CLDSLineAngle*)pPart1)->GetPartNo();
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
			if(sPartNo1.Length()<1)
#else
			if(sPartNo1.Length<1)
#endif
				sPartNo1.Printf("0X%X",pPart1->handle);
		}
		else if(pPart1&&pPart1->GetClassTypeId()==CLS_BOLT)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sPartType1.Copy("Bolt");
#else
			sPartType1.Copy("��˨");
#endif
			sPartNo1.Printf("0X%X",pPart1->handle);
		}
		if(pPart2&&pPart2->GetClassTypeId()==CLS_LINEANGLE)
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			sPartType2.Copy("Angle");
#else
			sPartType2.Copy("�Ǹ�");
#endif
			sPartNo2=((CLDSLineAngle*)pPart2)->GetPartNo();
#ifndef __ENABLE_XHSTRING_LENGTH_PROPERTY_
			if(sPartNo2.Length()<1)
#else
			if(sPartNo2.Length<1)
#endif
				sPartNo2.Printf("0X%X",pPart2->handle);
		}
		else if(pPart2&&pPart2->GetClassTypeId()==CLS_BOLT)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sPartType2.Copy("Bolt");
#else
			sPartType2.Copy("��˨");
#endif
			sPartNo2.Printf("0X%X",pPart2->handle);
		}
		//
		COutputInfo* pInfo=infoList.append();
		pInfo->m_hRelaObj1=pJgCutInfo->hPart1;
		pInfo->m_hRelaObj2=pJgCutInfo->hPart2;
		pInfo->m_uType=INFO_ONEKEY_CUTANGLE_CHECK;
		pInfo->collisionPt=InitCollisionPt(pJgCutInfo);
		int iRet=CCutJg::FinishCheckCutAngle(pJgCutInfo);
#ifdef AFX_TARG_ENU_ENGLISH
		if(iRet==2)
			pInfo->m_sDescription.Format("%d��,We need to manually specify  the start-cut information of angle %s",pJgCutInfo->iSeg.iSeg,(char*)sPartNo1);
		else if(iRet==3) 
			pInfo->m_sDescription.Format("%d��,We need to manually specify  the end-cut information of angle %s",pJgCutInfo->iSeg.iSeg,(char*)sPartNo1);
		else if(iRet==4)
			pInfo->m_sDescription.Format("%d��,We need to manually specify  the start-cut information of angle %s",pJgCutInfo->iSeg.iSeg,(char*)sPartNo2);
		else if(iRet==5)
			pInfo->m_sDescription.Format("%d��,We need to manually specify the end-cut information of angle %s",pJgCutInfo->iSeg.iSeg,(char*)sPartNo2);
		else
			pInfo->m_sDescription.Format("%d��,%s%s and %s%s can be cutted leg",pJgCutInfo->iSeg.iSeg,(char*)sPartNo1,(char*)sPartType1,(char*)sPartNo2,(char*)sPartType2);
#else 
		if(iRet==2)
			pInfo->m_sDescription.Format("%d��,%s�Ǹ�ʼ���ֶ�ָ���н���Ϣ,�����Զ��н�",pJgCutInfo->iSeg.iSeg,(char*)sPartNo1);
		else if(iRet==3) 
			pInfo->m_sDescription.Format("%d��,%s�Ǹ��ն��ֶ�ָ���н���Ϣ,�����Զ��н�",pJgCutInfo->iSeg.iSeg,(char*)sPartNo1);
		else if(iRet==4)
			pInfo->m_sDescription.Format("%d��,%s�Ǹ�ʼ���ֶ�ָ���н���Ϣ,�����Զ��н�",pJgCutInfo->iSeg.iSeg,(char*)sPartNo2);
		else if(iRet==5)
			pInfo->m_sDescription.Format("%d��,%s�Ǹ��ն��ֶ�ָ���н���Ϣ,�����Զ��н�",pJgCutInfo->iSeg.iSeg,(char*)sPartNo2);
		else
			pInfo->m_sDescription.Format("%d��,%s%s��%s%s�����Զ��н�!",pJgCutInfo->iSeg.iSeg,(char*)sPartNo1,(char*)sPartType1,(char*)sPartNo2,(char*)sPartType2);
#endif
	}
	//���������Ϣ����������б���
	CMainFrame* pMainFrm=(CMainFrame*)theApp.GetMainWnd();
	COutputInfoDlg *pTaskListDlg = (COutputInfoDlg*)pMainFrm->GetOutputInfoPage();
	if(infoList.GetNodeNum()>0)
		pMainFrm->ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
	pTaskListDlg->ClearOutputInfoList();
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"The current have %d task",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"The current have 0 task");
#else 
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"��ǰ��%d������",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"��ǰ��0������");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	((CMainFrame*)AfxGetMainWnd())->ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
}

//�Ǹֿ���
int CLDSView::OpenCutAngle()
{
	CCmdLockObject cmdlock(this);
	if (!cmdlock.LockSuccessed())
		return FALSE;
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	g_pSolidDraw->Draw();
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//��׽�Ǹ�
	CLDSLineAngle* pCurJg = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("please select OpenCut angle:", "");
#else
	pCmdLine->FillCmdLine("��ѡ������۵ĽǸ�:", "");
#endif
	DWORD dwhObj = 0, dwExportFlag = 0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_LINEANGLE));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE, AtomType::prLine);
	while (1)
	{
		if (g_pSolidSnap->SnapObject(&dwhObj, &dwExportFlag, &verifier) < 0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj, dwExportFlag);
		pCurJg = (CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj, CLS_LINEANGLE);
		if(pCurJg)
			break;
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("suitable angle not selected,please select opencut angle again:", "");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĽǸ֣�������ѡ������۽Ǹ�:", "");
#endif
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X", pCurJg->handle));
	g_pSolidDraw->SetEntSnapStatus(pCurJg->handle);
	//��׽���
	CLDSFitting* pFitting = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("please select fitting:", "");
#else
	pCmdLine->FillCmdLine("��ѡ�������Ľ��:", "");
#endif
	verifier.ClearSnapFlag();
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE, GetSingleWord(SELECTINDEX_FITTINGS));
	while (1)
	{
		if (g_pSolidSnap->SnapObject(&dwhObj, &dwExportFlag, &verifier) < 0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj, dwExportFlag);
		pFitting = (CLDSFitting*)console.FromPartHandle(obj.hRelaObj, CLS_FITTINGS);
		if (pFitting)
			break;
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("suitable angle not selected,please select fitting again:", "");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĽ�ߣ�������ѡ������:", "");
#endif
	}
	pCmdLine->FinishCmdLine(CXhChar16("0X%X", pFitting->handle));
	g_pSolidDraw->SetEntSnapStatus(pFitting->handle);
	FinishJgOpenCut(pCurJg, pFitting);
	//
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:", "");
#else
	pCmdLine->FillCmdLine("����:", "");
#endif
	return 1;
}
BOOL CLDSView::FinishJgOpenCut(CLDSLineAngle* pLineAngle, CLDSFitting* pFitting)
{
	if (pLineAngle == NULL || pFitting == NULL)
		return FALSE;
	CLogErrorLife logErrLife;
	if (pLineAngle->xarrOpenCuts.Count >= 4)
	{
		logerr.Log("Ŀǰ���֧��4������!");
		return FALSE;
	}
	//���ݽ��λ���жϽǸִ����۵�֫
	double fCutW = 0, fCutH = pLineAngle->GetWidth()*0.5;
	BYTE ciOpencutWing = 'X';
	if (pFitting->m_ciFittingType!= CLDSFitting::TYPE_UR)
	{
		long hBolt = pFitting->holes[0].hBolt;
		long hRod = pFitting->holes[0].hResidePart;
		CLDSBolt* pBolt = (CLDSBolt*)Ta.FromPartHandle(hBolt, CLS_BOLT);
		CLDSLineAngle* pResideJg = (CLDSLineAngle*)Ta.FromPartHandle(hRod, CLS_LINEANGLE);
		if (pBolt == NULL || pResideJg == NULL)
		{
			logerr.Log("0X%X����Ҳ���������˨�ͽǸ�!",pFitting->handle);
			return FALSE;
		}
		int iWorkWIng = 0;
		IsInsideJg(pResideJg, pBolt->ucs.axis_z, &iWorkWIng);
		f3dPoint work_vec = (iWorkWIng == 0) ? pResideJg->vyWingNorm : pResideJg->vxWingNorm;
		IsInsideJg(pLineAngle, work_vec, &iWorkWIng);
		ciOpencutWing = (iWorkWIng == 0) ? 'X' : 'Y';
		//
		if (pFitting->m_ciFittingType == CLDSFitting::TYPE_EB)
		{
			FITTING_EB* pEB = QueryFitting<FITTING_EB>(pFitting->sizespec);
			if (pEB == NULL)
			{
				logerr.Log("�������в������ͺ�%s��EB",pFitting->sizespec);
				return FALSE;
			}
			fCutW = pEB->dfBoardL + pResideJg->GetThick() * 2;
		}
		else
		{
			FITTING_GD* pGD = QueryFitting<FITTING_GD>(pFitting->sizespec);
			if (pGD == NULL)
			{
				logerr.Log("�������в������ͺ�%s��GD", pFitting->sizespec);
				return FALSE;
			}
			fCutW = pGD->dfBoardL + pResideJg->GetThick() * 2;
		}
	}
	else
		return FALSE;	//TODO:���޴˽��
	//�������ڽǸ������ϵ�λ��
	GEPOINT ptS, ptE, pick_org, intersPt,wing_norm,wing_vec;
	pick_org = pFitting->ucs.origin;
	ptS = pLineAngle->Start();
	ptE = pLineAngle->End();
	wing_norm = (ciOpencutWing == 'X') ? pLineAngle->vxWingNorm : pLineAngle->vyWingNorm;
	wing_vec = (ciOpencutWing == 'X') ? pLineAngle->GetWingVecX() : pLineAngle->GetWingVecY();
	if (Int3dlf(intersPt, pick_org, wing_norm, ptS, wing_norm) != 1)
		return FALSE;
	if (Int3dll(ptS, ptE, intersPt + wing_vec * 1000, intersPt - wing_vec * 1000, intersPt) < 0)
		return FALSE;
	//��ʼ����������
	COpenSlotDlg dlg;
	dlg.m_iCutWing = (ciOpencutWing == 'X') ? 0 : 1;
	dlg.m_fOffLen= DISTANCE(ptS, intersPt) - fCutW * 0.5;
	dlg.m_fSlotH = fCutH;
	dlg.m_fSlotW = fCutW;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	CLDSLineAngle::OPENING_CUT xOpenCut;
	xOpenCut.dfLateralBtmXorY = pLineAngle->size_wide - dlg.m_fSlotH;
	if (dlg.m_iCutWing == 0)
		xOpenCut.dfLateralBtmXorY *= -1;
	xOpenCut.dfLenPosFrom = dlg.m_fOffLen;
	xOpenCut.dfLenPosTo = dlg.m_fOffLen + dlg.m_fSlotW;
	xOpenCut.dfTipDeltaS = dlg.m_fDeltaS;
	xOpenCut.dfTipDeltaE = dlg.m_fDeltaE;
	pLineAngle->xarrOpenCuts.Append(xOpenCut);
	//
	pLineAngle->SetModified();
	pLineAngle->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return TRUE;
}
#endif
//</DEVELOP_PROCESS_MARK>