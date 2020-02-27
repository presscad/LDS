#include "stdafx.h"
#include "Tower.h"
#include "stdio.h"
#include "PartLib.h"
#include "CreateFace.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __ALFA_TEST_VERSION_

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void CLDSLineAngle::Test_CalPosition()
{
	if(!m_bEnableTeG)
	{
		JGZJ jgzj;
		if(getjgzj(jgzj,size.wide))
			xWingXZhunJu = xWingYZhunJu = jgzj;
	}
	cal_start_ber_pos();
	cal_end_ber_pos();
}
int CLDSLineAngle::Test_GetStartWorkWing()
{
	if(pOldDesStartPos==NULL)
	{
		logto.Log("CLDSLineAngle::Test_GetStartWorkWing�У�0X%X�Ǹ�ʼ��pOldDesStartPos=NULL",handle);
		return -1;
	}
	CSuperSmartPtr<CLDSLinePart>pDatumPart=GetStartDatumPart();
	if(pDatumPart.IsNULL()||!pDatumPart->IsAngle())
		return 2;	//�޻�׼����ʱ��Ϊ��˫֫����
	if(pOldDesStartPos->jgber_cal_style==1||pOldDesStartPos->jgber_cal_style==2)
		return 2;	//����ͶӰ��ƽ��ʱ��Ϊ��˫֫����
	if(pDatumPart->handle==handle)
		return 2;	//�ڵ㸸�˼�Ϊ��ǰ�Ǹֱ����޷��жϹ���ֻ֫�ܰ�˫֫���Ӵ���
	f3dPoint datum_norm;
	if(pOldDesStartPos->jgber_cal_style==0&&pOldDesStartPos->IsInDatumJgWingX())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_x_wing();
	else if(pOldDesStartPos->jgber_cal_style==0&&pOldDesStartPos->IsInDatumJgWingY())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_y_wing();
	else
	{
		f3dPoint datum_vec = pDatumPart->End()-pDatumPart->Start();
		f3dPoint line_vec = End()-Start();
		datum_norm = datum_vec^line_vec;
		normalize(datum_norm);
	}
	if(fabs(datum_norm*get_norm_x_wing())>fabs(datum_norm*get_norm_y_wing()))
		return 0;
	else
		return 1;
}
int CLDSLineAngle::Test_GetEndWorkWing()
{
	if(pOldDesEndPos==NULL)
	{
		logto.Log("CLDSLineAngle::Test_GetEndWorkWing�У�0X%X�Ǹ�ʼ��pOldDesEndPos=NULL",handle);
		return -1;
	}
	CSuperSmartPtr<CLDSLinePart>pDatumPart=GetEndDatumPart();
	if(pDatumPart.IsNULL()||!pDatumPart->IsAngle())
		return 2;	//�޻�׼����ʱ��Ϊ��˫֫����
	if(pOldDesEndPos->jgber_cal_style==1||pOldDesEndPos->jgber_cal_style==2)
		return 2;	//����ͶӰ��ƽ��ʱ��Ϊ��˫֫����
	f3dPoint datum_norm;
	if(pOldDesEndPos->jgber_cal_style==0&&pOldDesEndPos->IsInDatumJgWingX())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_x_wing();
	else if(pOldDesEndPos->jgber_cal_style==0&&pOldDesEndPos->IsInDatumJgWingY())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_y_wing();
	else
	{
		f3dPoint datum_vec = pDatumPart->End()-pDatumPart->Start();
		f3dPoint line_vec = End()-Start();
		datum_norm = datum_vec^line_vec;
		normalize(datum_norm);
	}
	if(fabs(datum_norm*get_norm_x_wing())>fabs(datum_norm*get_norm_y_wing()))
		return 0;
	else
		return 1;
}
double CLDSLineAngle::Test_GetStartExtraOdd(int wing_x0_y1_ber2)
{
	CSmartPtr<CLDSLineAngle> pDatumJg;
	if(pOldDesStartPos==NULL)
	{
		logto.Log("CLDSLineAngle::Test_GetStartExtraOdd�У�0X%X�Ǹ�ʼ��pOldDesStartPos=NULL",handle);
		return 0;
	}
	if(pStart==NULL)
		return 0;	//ʼ��ĩ����Ϣ��ȫ
	if(wing_x0_y1_ber2==0&&extraStartOdd.wing_x.style==0)
		return extraStartOdd.wing_x.odd;
	else if(wing_x0_y1_ber2==1&&extraStartOdd.wing_y.style==0)
		return extraStartOdd.wing_y.odd;
	else if(wing_x0_y1_ber2==2&&extraStartOdd.ber.style==0)
		return extraStartOdd.ber.odd;
	else if(wing_x0_y1_ber2==0&&extraStartOdd.wing_x.style==1)
	{
		//extraStartOdd.wing_x.extraStartPoint.UpdatePos(m_pTower);
		extraStartOdd.wing_x.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=Start();//extraStartOdd.wing_x.extraStartPoint.pos;
		f3dPoint end=extraStartOdd.wing_x.extraDatumPoint.Position();
		f3dPoint direct=End()-Start();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraStartOdd.wing_x.odd=odd_vec*direct;
		return extraStartOdd.wing_x.odd;
	}
	else if(wing_x0_y1_ber2==1&&extraStartOdd.wing_y.style==1)
	{
		//extraStartOdd.wing_y.extraStartPoint.UpdatePos(m_pTower);
		extraStartOdd.wing_y.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=Start();//extraStartOdd.wing_y.extraStartPoint.pos;
		f3dPoint end=extraStartOdd.wing_y.extraDatumPoint.Position();
		f3dPoint direct=End()-Start();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraStartOdd.wing_y.odd=odd_vec*direct;
		return extraStartOdd.wing_y.odd;
	}
	else if(wing_x0_y1_ber2==2&&extraStartOdd.ber.style==1)
	{
		//extraStartOdd.ber.extraStartPoint.UpdatePos(m_pTower);
		extraStartOdd.ber.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=Start();//extraStartOdd.ber.extraStartPoint.pos;
		f3dPoint end=extraStartOdd.ber.extraDatumPoint.Position();
		f3dPoint direct=End()-Start();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraStartOdd.ber.odd=odd_vec*direct;
		return extraStartOdd.ber.odd;
	}
	else if(wing_x0_y1_ber2==2)
	{
		int datum_wing=GetStartWorkWing();
		if(datum_wing==0)
			return -GetStartExtraOdd(0);
		else if(datum_wing==1)
			return -GetStartExtraOdd(1);
		else
		{	//�޷��жϵ�ǰ����֫ʱ��������ͷ����ֵ�ϴ���Ϊ��׼
			double odd_x=-GetStartExtraOdd(0);
			double odd_y=-GetStartExtraOdd(1);
			if(fabs(odd_x)>fabs(odd_y))
				return odd_x;
			else
				return odd_y;
		}
	}
	else
	{
		if(pOldDesStartPos->jgber_cal_style==1||pOldDesStartPos->jgber_cal_style==2)
			return 0;	//����ͶӰ/ƽ�ƶ����̶�����ͷΪ��
		else if(pOldDesStartPos->jgber_cal_style==3)
		{	//�Ǹ�ʼ�����ָ��������ʽ
			CLDSLinePart *pDatumLinePart=NULL;
			pDatumLinePart=GetStartDatumPart();
			if(pDatumLinePart&&(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pDatumJg=(CLDSLineAngle*)pDatumLinePart;
			if(pDatumJg.IsNULL()||pDatumJg.IsEqualPtr(this))
				return 0;	//�޻�׼�Ǹ�,���׼�Ǹ־��Ǳ���,���׼�˼����ǽǸ�
			
			int datum_start0_end1;
			if(pDatumJg->pStart==pStart)
				datum_start0_end1=0;	//ʼ������
			else if(pDatumJg->pEnd==pStart)
				datum_start0_end1=1;	//�ն�����
			else
				datum_start0_end1=2;	//�м�����
			int father_wing_x0_y1;
			if(wing_x0_y1_ber2==0)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_x_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_x_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_x_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			else if(wing_x0_y1_ber2==1)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_y_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_y_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_y_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			f3dLine axis_line,father_axis_line;
			f3dPoint inters,face_norm,face_pick;
			
			JGZJ jgzj,father_jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			getjgzj(father_jgzj,pDatumJg->GetWidth());
			if(pDatumJg->m_bEnableTeG)
			{
				if(father_wing_x0_y1==0)
					father_jgzj=pDatumJg->xWingXZhunJu;
				else
					father_jgzj=pDatumJg->xWingYZhunJu;
			}
			if(wing_x0_y1_ber2==0)
			{
				axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
				axis_line.endPt=End()+GetWingVecX()*jgzj.g;
			}
			else if(wing_x0_y1_ber2==1)
			{
				axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
				axis_line.endPt=End()+GetWingVecY()*jgzj.g;
			}
			if(father_wing_x0_y1==0)
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecX()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecX()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_x_wing();
			}
			else
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecY()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecY()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_y_wing();
			}
			normalize(face_norm);
			if(datum_start0_end1==0)
				face_pick=father_axis_line.startPt;
			else
				face_pick=father_axis_line.endPt;
			project_point(axis_line.startPt,face_pick,face_norm);
			project_point(axis_line.endPt,face_pick,face_norm);
			f3dPoint ray_vec,father_vec;
			ray_vec=axis_line.startPt-axis_line.endPt;
			father_vec=father_axis_line.startPt-father_axis_line.endPt;
			normalize(ray_vec);
			normalize(father_vec);
			if(fabs(ray_vec*father_vec)>EPS_COS2)
				return 0;	//���Ǹֻ���ƽ��
			int ret=Int3dpl(axis_line,father_axis_line,inters);
			if(ret!=1)
				return 0;
			else	//�󽻳ɹ�
			{
				double dd=DISTANCE(axis_line.startPt,inters);
				if(axis_line.PtInLine(inters)>0)	//����ֱ�߶���
					return dd;
				else
					return -dd;
			}
		}
		else	//����ƫ��
		{
			JGZJ jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			if(pOldDesStartPos->datum_pos_style==0&&pOldDesStartPos->datum_to_ber_style==1)
			{	//�˽ڵ��ڸ��Ǹ�ʼ�ˣ��Ҹ��Ǹ�ʼ�˴���ƽ�Ʒ�ʽ
				f3dPoint face_pick,face_norm,inters;
				f3dLine axis_line;
				if(wing_x0_y1_ber2==0)
				{
					axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
					axis_line.endPt=End()+GetWingVecX()*jgzj.g;
				}
				else if(wing_x0_y1_ber2==1)
				{
					axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
					axis_line.endPt=End()+GetWingVecY()*jgzj.g;
				}
				
				if(Int3dlf(inters,axis_line,pStart->Position(true),pOldDesStartPos->face_offset_norm)==1)
				{
					double dd;
					dd=DISTANCE(inters,axis_line.startPt);
					if(axis_line.PtInLine(inters)>0)	//�����߶��ڻ�˵���
						return dd;
					else
						return -dd;
				}
				else
					return 0;
			}
			else if(pOldDesStartPos->IsInHuoQuLine())	//���ڻ�����
			{
				CLDSLineAngle *pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesStartPos->huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg==NULL)
					pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg==NULL)
					return 0;
				f3dPoint work_norm=((pHuoQuJg->End()-pHuoQuJg->Start())^(End()-Start()));
				normalize(work_norm);
				if(wing_x0_y1_ber2==0&&fabs(work_norm*get_norm_x_wing())<EPS_COS2)
					return 0;
				else if(wing_x0_y1_ber2==1&&fabs(work_norm*get_norm_y_wing())<EPS_COS2)
					return 0;
				else
					return -DISTANCE(GetStartDatumPos(),GetStartDatumPosEx());
			}
			else
				return 0;
		}
	}
}
double CLDSLineAngle::Test_GetEndExtraOdd(int wing_x0_y1_ber2)
{
	if(pOldDesEndPos==NULL)
	{
		logto.Log("CLDSLineAngle::Test_GetEndExtraOdd�У�0X%X�Ǹ�ʼ��pOldDesEnd=NULL",handle);
		return 0;
	}
	int start0_end1=1;
	CSmartPtr<CLDSLineAngle> pDatumJg;
	if(pEnd==NULL)
		return 0;	//ʼ��ĩ����Ϣ��ȫ
	if(wing_x0_y1_ber2==0&&extraEndOdd.wing_x.style==0)
		return extraEndOdd.wing_x.odd;
	else if(wing_x0_y1_ber2==1&&extraEndOdd.wing_y.style==0)
		return extraEndOdd.wing_y.odd;
	else if(wing_x0_y1_ber2==2&&extraEndOdd.ber.style==0)
		return extraEndOdd.ber.odd;
	else if(wing_x0_y1_ber2==0&&extraEndOdd.wing_x.style==1)
	{
		//extraEndOdd.wing_x.extraStartPoint.UpdatePos(m_pTower);
		extraEndOdd.wing_x.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=End();//extraEndOdd.wing_x.extraStartPoint.pos;
		f3dPoint end=extraEndOdd.wing_x.extraDatumPoint.Position();
		f3dPoint direct=Start()-End();	//���߷���
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraEndOdd.wing_x.odd=odd_vec*direct;
		return extraEndOdd.wing_x.odd;
	}
	else if(wing_x0_y1_ber2==1&&extraEndOdd.wing_y.style==1)
	{
		//extraEndOdd.wing_y.extraStartPoint.UpdatePos(m_pTower);
		extraEndOdd.wing_y.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=End();//extraEndOdd.wing_y.extraStartPoint.pos;
		f3dPoint end=extraEndOdd.wing_y.extraDatumPoint.Position();
		f3dPoint direct=Start()-End();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraEndOdd.wing_y.odd=odd_vec*direct;
		return extraEndOdd.wing_y.odd;
	}
	else if(wing_x0_y1_ber2==2&&extraEndOdd.ber.style==1)
	{
		//extraEndOdd.ber.extraStartPoint.UpdatePos(m_pTower);
		extraEndOdd.ber.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=End();//extraEndOdd.ber.extraStartPoint.pos;
		f3dPoint end=extraEndOdd.ber.extraDatumPoint.Position();
		f3dPoint direct=Start()-End();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraEndOdd.ber.odd=odd_vec*direct;
		return extraEndOdd.ber.odd;
	}
	else if(wing_x0_y1_ber2==2)
	{
		int datum_wing=GetEndWorkWing();
		if(datum_wing==0)
			return -GetEndExtraOdd(0);
		else if(datum_wing==1)
			return -GetEndExtraOdd(1);
		else
		{	//�޷��жϵ�ǰ����֫ʱ��������ͷ����ֵ�ϴ���Ϊ��׼
			double odd_x=-GetEndExtraOdd(0);
			double odd_y=-GetEndExtraOdd(1);
			if(fabs(odd_x)>fabs(odd_y))
				return odd_x;
			else
				return odd_y;
		}
	}
	else
	{
		if(pOldDesEndPos->jgber_cal_style==1||pOldDesEndPos->jgber_cal_style==2)
			return 0;	//����ͶӰ/ƽ�ƶ����̶�����ͷΪ��
		else if(pOldDesEndPos->jgber_cal_style==3)
		{	//�Ǹ��ն����ָ��������ʽ
			CLDSLinePart *pDatumLinePart=NULL;
			pDatumLinePart=GetEndDatumPart();
			if(pDatumLinePart&&(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pDatumJg=(CLDSLineAngle*)pDatumLinePart;
			if(pDatumJg.IsNULL()||pDatumJg.IsEqualPtr(this))
				return 0;	//�޻�׼�Ǹ�,���׼�Ǹ־��Ǳ���,���׼�˼����ǽǸ�
			
			int datum_start0_end1;
			if(pDatumJg->pStart==pEnd)
				datum_start0_end1=0;	//ʼ������
			else if(pDatumJg->pEnd==pEnd)
				datum_start0_end1=1;	//�ն�����
			else
				datum_start0_end1=2;	//�м�����
			int father_wing_x0_y1;
			if(wing_x0_y1_ber2==0)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_x_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_x_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_x_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			else if(wing_x0_y1_ber2==1)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_y_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_y_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_y_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			f3dLine axis_line,father_axis_line;
			f3dPoint inters,face_norm,face_pick;
			
			JGZJ jgzj,father_jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			getjgzj(father_jgzj,pDatumJg->GetWidth());
			if(pDatumJg->m_bEnableTeG)
			{
				if(father_wing_x0_y1==0)
					father_jgzj=pDatumJg->xWingXZhunJu;
				else
					father_jgzj=pDatumJg->xWingYZhunJu;
			}
			if(wing_x0_y1_ber2==0)
			{
				axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
				axis_line.endPt=End()+GetWingVecX()*jgzj.g;
			}
			else if(wing_x0_y1_ber2==1)
			{
				axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
				axis_line.endPt=End()+GetWingVecY()*jgzj.g;
			}
			if(father_wing_x0_y1==0)
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecX()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecX()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_x_wing();
			}
			else
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecY()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecY()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_y_wing();
			}
			normalize(face_norm);
			if(datum_start0_end1==0)
				face_pick=father_axis_line.startPt;
			else
				face_pick=father_axis_line.endPt;
			project_point(axis_line.startPt,face_pick,face_norm);
			project_point(axis_line.endPt,face_pick,face_norm);
			f3dPoint ray_vec,father_vec;
			ray_vec=axis_line.startPt-axis_line.endPt;
			father_vec=father_axis_line.startPt-father_axis_line.endPt;
			normalize(ray_vec);
			normalize(father_vec);
			if(fabs(ray_vec*father_vec)>EPS_COS2)
				return 0;	//���Ǹֻ���ƽ��
			int ret=Int3dpl(axis_line,father_axis_line,inters);
			if(ret!=1)
				return 0;
			else	//�󽻳ɹ�
			{
				double dd=DISTANCE(axis_line.endPt,inters);
				if(axis_line.PtInLine(inters)>0)	//����ֱ�߶���
					return dd;
				else
					return -dd;
			}
		}
		else	//����ƫ��
		{
			JGZJ jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			if(pOldDesEndPos->datum_pos_style==0&&pOldDesEndPos->datum_to_ber_style==1)
			{	//�˽ڵ��ڸ��Ǹ��նˣ��Ҹ��Ǹ��ն˴���ƽ�Ʒ�ʽ
				f3dPoint inters;
				f3dLine axis_line;
				
				if(wing_x0_y1_ber2==0)
				{
					axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
					axis_line.endPt=End()+GetWingVecX()*jgzj.g;
				}
				else if(wing_x0_y1_ber2==1)
				{
					axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
					axis_line.endPt=End()+GetWingVecY()*jgzj.g;
				}
				
				if(Int3dlf(inters,axis_line,pEnd->Position(false),pOldDesEndPos->face_offset_norm)==1)
				{
					double dd;
					dd=DISTANCE(inters,axis_line.endPt);
					if(axis_line.PtInLine(inters)>0)	//�����߶��ڻ�˵���
						return dd;
					else
						return -dd;
				}
				else
					return 0;
			}
			else if(pOldDesEndPos->IsInHuoQuLine())	//���ڻ�����
			{
				CSmartPtr<CLDSLineAngle> pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesEndPos->huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg.IsNULL())
					pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg.IsNULL())
					return 0;
				f3dPoint work_norm=((pHuoQuJg->End()-pHuoQuJg->Start())^(End()-Start()));
				normalize(work_norm);
				if(wing_x0_y1_ber2==0&&fabs(work_norm*get_norm_x_wing())<EPS_COS2)
					return 0;
				else if(wing_x0_y1_ber2==1&&fabs(work_norm*get_norm_y_wing())<EPS_COS2)
					return 0;
				else
					return -DISTANCE(GetEndDatumPos(),GetEndDatumPosEx());
			}
			else
				return 0;
		}
	}
}
void CLDSLineAngle::Test_CopyProperty(CLDSDbObject* pObj)
{
    SetLayer(pObj->layer());						// ���Ǹ�����ͼ��.
	iSeg = pObj->iSeg;
	cfgword = pObj->cfgword;
	if(pObj->IsPart())
	{
		CSuperSmartPtr<CLDSPart> pPart=(CLDSPart*)pObj;
		SetPartNo(pPart->GetPartNo());
		m_bVirtualPart = pPart->m_bVirtualPart;
		cMaterial = pPart->cMaterial;
#ifdef __PART_DESIGN_INC_
		if(pPart->IsLinePart())
		{
			m_uStatMatNo=pPart.LinePartPointer()->m_uStatMatNo;
			m_bNeedSelSize=pPart.LinePartPointer()->m_bNeedSelSize;
			start_force_type=pPart.LinePartPointer()->start_force_type;
			end_force_type=pPart.LinePartPointer()->end_force_type;
			start_joint_type=pPart.LinePartPointer()->start_joint_type;
			end_joint_type=pPart.LinePartPointer()->end_joint_type;
			CalLenCoef.iTypeNo=pPart.LinePartPointer()->CalLenCoef.iTypeNo;
			if(pPart->IsAngle())
			{
				hole_out=pPart.LineAnglePointer()->hole_out;
				m_bAutoCalHoleOutCoef=pPart.LineAnglePointer()->m_bAutoCalHoleOutCoef;
			}
		}
#endif
	}
	is_data_modified = TRUE;
	if(pObj->GetClassTypeId()!=CLS_LINEANGLE&&pObj->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		return;
	CLDSLineAngle *pJg=(CLDSLineAngle*)pObj;
    size.wide = pJg->GetWidth();
	size.thick = pJg->GetThick();
	//start_oddment = pJg->start_oddment;	//ʵ��֤������ͷ�ԳƲ��ã�������
	//end_oddment = pJg->end_oddment;
	nominal_length = pJg->nominal_length;
	//m_bEnableTeG=pJg->m_bEnableTeG;
	//xWingXZhunJu=pJg->xWingXZhunJu;
	//xWingYZhunJu=pJg->xWingYZhunJu;
	if(GetClassTypeId()==CLS_LINEANGLE)
	{	//�Ǹ�
		group_father_jg_h = pJg->group_father_jg_h;
		connectStart.CopyBasicBoltInfo(pJg->connectStart);
		connectEnd.CopyBasicBoltInfo(pJg->connectEnd);
		if(!IsStartPosLocked())
		{
			pOldDesStartPos->jgber_cal_style=pJg->pOldDesStartPos->jgber_cal_style;
			pOldDesStartPos->ChangeToDatumType(pJg->pOldDesStartPos->GetDatumType());	//ͶӰƽ�Ʒ�ʽ WJH-2004.09.20
			pOldDesStartPos->datum_to_ber_style=pJg->pOldDesStartPos->datum_to_ber_style;
			pOldDesStartPos->bByOtherEndOffset=pJg->pOldDesStartPos->bByOtherEndOffset;
			pOldDesStartPos->face_offset_norm=pJg->pOldDesStartPos->face_offset_norm;
		}
		if(!IsEndPosLocked())
		{
			pOldDesEndPos->jgber_cal_style=pJg->pOldDesEndPos->jgber_cal_style;
			pOldDesEndPos->ChangeToDatumType(pJg->pOldDesEndPos->GetDatumType());	//ͶӰƽ�Ʒ�ʽ WJH-2004.09.20
			pOldDesEndPos->datum_to_ber_style=pJg->pOldDesEndPos->datum_to_ber_style;
			pOldDesEndPos->bByOtherEndOffset=pJg->pOldDesEndPos->bByOtherEndOffset;
			pOldDesEndPos->face_offset_norm=pJg->pOldDesEndPos->face_offset_norm;
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetClassTypeId()==CLS_GROUPLINEANGLE&&pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//��ϽǸ�
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pJg;
		CLDSGroupLineAngle *pCurGroupJg=(CLDSGroupLineAngle*)this;
		pCurGroupJg->jg_space=pGroupJg->jg_space;
		pCurGroupJg->group_style=pGroupJg->group_style;
		pCurGroupJg->m_bUpdateSonAngleOddment=pGroupJg->m_bUpdateSonAngleOddment;
	}
	else if(GetClassTypeId()==CLS_GROUPLINEANGLE&&pJg->GetClassTypeId()==CLS_LINEANGLE)
	{	//�������Ǹ���˨��Ϣ����ϽǸ�
		connectStart.CopyBasicBoltInfo(pJg->connectStart);
		connectEnd.CopyBasicBoltInfo(pJg->connectEnd);
	}
#endif
}
f3dPoint CLDSLineAngle::Test_GetStartDatumPos()
{
	if(pOldDesStartPos==NULL)
	{
		logto.Log("CLDSLineAngle::Test_GetStartDatumPos�У�0X%X�Ǹ�ʼ��pOldDesStartPos=NULL",handle);
		return Start();
	}
	f3dPoint datum_point;
	if(BelongModel()==NULL)
		return Start();
	if(pOldDesStartPos->jgber_cal_style==0)
	{
		CSuperSmartPtr<CLDSLinePart>pDatumRod;
		if(pOldDesStartPos->datum_jg_h>0x20)
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pOldDesStartPos->datum_jg_h,CLS_LINEPART);
		if(pDatumRod.IsNULL()&&pStart!=NULL)
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEPART);
		f3dPoint len_offset_vec;
		if(pDatumRod.IsHasPtr())
		{
			len_offset_vec=pDatumRod->End()-pDatumRod->Start();
			normalize(len_offset_vec);
			len_offset_vec*=pOldDesStartPos->len_offset_dist;
		}
		if(pOldDesStartPos->datum_pos_style==0&&pStart!=NULL)		//�ڵ㼴����
			return pStart->Position(false);
		else if(pOldDesStartPos->datum_pos_style==15)	//ֱ��ָ����������
			return pOldDesStartPos->datum_point.Position();
		if(pDatumRod.IsNULL())
			return f3dPoint(0,0,0);
		else if(pDatumRod->IsAngle())
		{
			CSmartPtr<CLDSLineAngle>pDatumJg=pDatumRod.LineAnglePointer();
			if(pOldDesStartPos->datum_pos_style==1||pOldDesStartPos->datum_pos_style==7)
				datum_point=pDatumJg->GetDatumPosBer(pStart)+len_offset_vec;
			else if(pOldDesStartPos->datum_pos_style==2||pOldDesStartPos->datum_pos_style==8||pOldDesStartPos->datum_pos_style==13)
				return pDatumJg->GetDatumPosWingX(pStart)+len_offset_vec;	//��X֫����ͶӰ
			else if(pOldDesStartPos->datum_pos_style==3||pOldDesStartPos->datum_pos_style==9||pOldDesStartPos->datum_pos_style==14)
				return pDatumJg->GetDatumPosWingY(pStart)+len_offset_vec;	//��Y֫����ͶӰ
			else if(pOldDesStartPos->datum_pos_style==4||pOldDesStartPos->datum_pos_style==10)//.offset_style==1)
			{	//��������ƽ��
				if(pOldDesStartPos->bByOtherEndOffset)
				{
					f3dPoint pt = GetEndDatumPos();
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&pt)+len_offset_vec;
				}
				else
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pStart,pOldDesStartPos->face_offset_norm)+len_offset_vec;
			}
			else if(pOldDesStartPos->datum_pos_style==5||pOldDesStartPos->datum_pos_style==11)		//X֫
			{	//����X֫����ƽ��
				if(pOldDesStartPos->bByOtherEndOffset)
				{
					f3dPoint pt = GetEndDatumPos();
					return pDatumJg->GetDatumPosWingXByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingXByFaceOffset(pStart,pOldDesStartPos->face_offset_norm)+len_offset_vec;
			}
			else if(pOldDesStartPos->datum_pos_style==6||pOldDesStartPos->datum_pos_style==12)
			{	//����Y֫����ƽ��
				if(pOldDesStartPos->bByOtherEndOffset)
				{
					f3dPoint pt = GetEndDatumPos();
					return pDatumJg->GetDatumPosWingYByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingYByFaceOffset(pStart,pOldDesStartPos->face_offset_norm)+len_offset_vec;
			}
			if((pOldDesStartPos->datum_pos_style==1||pOldDesStartPos->datum_pos_style==4)&&
				pDatumJg.IsHasPtr()&&pDatumJg->IsReservedHeJiao())
			{	//����ͶӰ���߻�ƽ������ʱ���б���֫��ȵ�����Ͻ���Ҫ���⴦��
				f3dPoint vec=End()-Start();
				f3dPoint work_norm=vec^len_offset_vec;
				normalize(work_norm);
				if(fabs(work_norm*pDatumJg->get_norm_x_wing())>fabs(work_norm*pDatumJg->get_norm_y_wing()))
					datum_point+=CalWingOffsetVec('X',0);
				else
					datum_point+=CalWingOffsetVec('Y',0);
			}
		}
		else if(pDatumRod->GetClassTypeId()==CLS_LINETUBE)
			SnapPerp(&datum_point,pDatumRod->Start(),pDatumRod->End(),pStart->Position(false));
		return datum_point;		//��������ͶӰ
	}
	else if(pOldDesStartPos->jgber_cal_style==1)	//����ֱ��ͶӰ
		return Start();
	else if(pOldDesStartPos->jgber_cal_style==2)	//����ֱ��ƽ��
	{
		Int3dlf(datum_point,f3dLine(Start(),End()),pStart->Position(true),pOldDesStartPos->face_offset_norm);
		return datum_point;
	}
	else// if(pOldDesStartPos->jgber_cal_style==3)	//��ʾָ������
		return Start();
}
f3dPoint CLDSLineAngle::Test_GetEndDatumPos()
{
	if(pOldDesEndPos==NULL)
	{
		logto.Log("CLDSLineAngle::Test_GetEndDatumPos�У�0X%X�Ǹ�ʼ��pOldDesEnd=NULL",handle);
		return End();
	}
	f3dPoint datum_point;
	if(BelongModel()==NULL)
		return End();
	if(pOldDesEndPos->jgber_cal_style==0)
	{
		CSuperSmartPtr<CLDSLinePart>pDatumRod;
		if(pOldDesEndPos->datum_jg_h>0x20)
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pOldDesEndPos->datum_jg_h,CLS_LINEPART);
		if(pDatumRod.IsNULL()&&pEnd!=NULL)
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEPART);
		f3dPoint len_offset_vec;
		if(pDatumRod.IsHasPtr())
		{
			len_offset_vec=pDatumRod->End()-pDatumRod->Start();
			normalize(len_offset_vec);
			len_offset_vec*=pOldDesEndPos->len_offset_dist;
		}
		if(pOldDesEndPos->datum_pos_style==0&&pEnd!=NULL)
			return pEnd->Position(false);	//�ڵ㼴����
		else if(pOldDesEndPos->datum_pos_style==15)	//ֱ��ָ����������
			return pOldDesEndPos->datum_point.Position();
		else if(pDatumRod.IsNULL())
			return f3dPoint(0,0,0);
		else if(pDatumRod->IsAngle())
		{
			CSmartPtr<CLDSLineAngle>pDatumJg=pDatumRod.LineAnglePointer();
			if(pOldDesEndPos->datum_pos_style==1||pOldDesEndPos->datum_pos_style==7)
				datum_point = pDatumJg->GetDatumPosBer(pEnd)+len_offset_vec;		//������ͶӰ
			else if(pOldDesEndPos->datum_pos_style==2||pOldDesEndPos->datum_pos_style==8||pOldDesEndPos->datum_pos_style==13)
				return pDatumJg->GetDatumPosWingX(pEnd)+len_offset_vec;	//����X֫����ͶӰ
			else if(pOldDesEndPos->datum_pos_style==3||pOldDesEndPos->datum_pos_style==9||pOldDesEndPos->datum_pos_style==14)
				return pDatumJg->GetDatumPosWingY(pEnd)+len_offset_vec;	//����Y֫����ͶӰ
			else if(pOldDesEndPos->datum_pos_style==4||pOldDesEndPos->datum_pos_style==10)
			{	//��������ƽ��
				if(pOldDesEndPos->bByOtherEndOffset)
				{
					f3dPoint pt = GetStartDatumPos();
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&pt)+len_offset_vec;
				}
				else
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm)+len_offset_vec;
			}
			else if(pOldDesEndPos->datum_pos_style==5||pOldDesEndPos->datum_pos_style==11)
			{	//����X֫����ƽ��
				if(pOldDesEndPos->bByOtherEndOffset)
				{
					f3dPoint pt = GetStartDatumPos();
					return pDatumJg->GetDatumPosWingXByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingXByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm)+len_offset_vec;
			}
			else if(pOldDesEndPos->datum_pos_style==6||pOldDesEndPos->datum_pos_style==12)
			{	//����Y֫����ƽ��
				if(pOldDesEndPos->bByOtherEndOffset==1)
				{
					f3dPoint pt = GetStartDatumPos();
					return pDatumJg->GetDatumPosWingYByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingYByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm)+len_offset_vec;
			}
			if((pOldDesEndPos->datum_pos_style==1||pOldDesEndPos->datum_pos_style==4)&&
				pDatumJg.IsHasPtr()&&pDatumJg->IsReservedHeJiao())
			{	//�б���֫��ȵ�����Ͻ�
				f3dPoint vec=End()-Start();
				f3dPoint work_norm=vec^len_offset_vec;
				normalize(work_norm);
				if(fabs(work_norm*pDatumJg->get_norm_x_wing())>fabs(work_norm*pDatumJg->get_norm_y_wing()))
					datum_point+=CalWingOffsetVec('X',0);
				else
					datum_point+=CalWingOffsetVec('Y',0);
			}
		}
		else if(pDatumRod->GetClassTypeId()==CLS_LINETUBE)
			SnapPerp(&datum_point,pDatumRod->Start(),pDatumRod->End(),pEnd->Position(false));
		return datum_point;
	}
	else if(pOldDesEndPos->jgber_cal_style==1)	//����ֱ��ͶӰ
		return End();
	else if(pOldDesEndPos->jgber_cal_style==2)	//����ֱ��ƽ��
	{
		Int3dlf(datum_point,f3dLine(Start(),End()),pEnd->Position(false),pOldDesEndPos->face_offset_norm);
		return datum_point;
	}
	else //if(pOldDesEndPos->jgber_cal_style==3)
	{	//��ʾָ�����꣬ȥ��ԭ������ʱҲֱ�ӷ��ص��趨����Ϊ�Ȼ�������ƶ���˨ʱ�����⣩ WJH-2004.11.19
		return End();
	}
}
f3dPoint CLDSLineAngle::Test_GetStartDatumPosEx()
{
	//try{
		if(pOldDesStartPos->jgber_cal_style==3||pOldDesStartPos->datum_pos_style<=6||pOldDesStartPos->datum_pos_style==15||pStart==NULL)
			return Test_GetStartDatumPos();
		else// if(pStart->hFatherPart!=handle)
		{	//��ӽǸ�
			f3dPoint start_ber,start_datum;
			start_ber=Start();
			f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
			//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint datum_vec,cur_vec,work_norm;;
			CSmartPtr<CLDSLineAngle>pDatumJg;
			if(pOldDesStartPos->datum_jg_h>0x20)
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesStartPos->datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
				throw "�Ǹ�ʼ�˶�λ��׼�Ǹּ�ʼ�˽ڵ㸸�˼���Ϊ0x0��";
			datum_vec = pDatumJg->pEnd->GetInverseNodePos()-pDatumJg->pStart->GetInverseNodePos();
			cur_vec = pEnd->GetInverseNodePos()-pStart->GetInverseNodePos();
			normalize(datum_vec);
			normalize(cur_vec);
			if(fabs(datum_vec*cur_vec)>=0.9)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
			{
				if(pOldDesStartPos->IsInDatumJgWingX())
					work_norm=pDatumJg->get_norm_x_wing();
				else if(pOldDesStartPos->IsInDatumJgWingY())
					work_norm=pDatumJg->get_norm_y_wing();
				else
					work_norm=datum_vec^cur_vec;
			}
			else
				work_norm = datum_vec^cur_vec;
			normalize(work_norm);
			if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
			{
				offset_vec_x = get_norm_x_wing();
				offset_vec_y = -GetWingVecX();
			}
			else
			{
				offset_vec_x = -GetWingVecY();
				offset_vec_y = get_norm_y_wing();
			}
		
			start_datum = start_ber-offset_vec_x*GetStartOffsetByWingX();
			start_datum = start_datum-offset_vec_y*GetStartOffsetByWingY();
			return start_datum;
		}
	/*}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
		return f3dPoint(0,0,0);
	}*/
}
f3dPoint CLDSLineAngle::Test_GetEndDatumPosEx()
{
	//try{
		if(pOldDesEndPos->jgber_cal_style==3||pOldDesEndPos->datum_pos_style<=6||pOldDesEndPos->datum_pos_style==15||pEnd==NULL)
			return Test_GetEndDatumPos();
		else// if(pEnd->hFatherPart!=handle)
		{	//��ӽǸ�
			f3dPoint end_ber,end_datum;
			end_ber=End();
			f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
			//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint datum_vec,cur_vec,work_norm;
			CSmartPtr<CLDSLineAngle>pDatumJg;
			if(pOldDesEndPos->datum_jg_h>0x20)
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesEndPos->datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
				throw "�Ǹ��ն˶�λ��׼�Ǹּ��ն˽ڵ㸸�˼���Ϊ0x0��";
			//��������ֱ�Ӹ�Ϊ�Ǹ���㣬��û�в���ֻ�Ǿ�����ǰ�������е���� WJH-2005.03.08
			datum_vec = pDatumJg->End()-pDatumJg->Start();
			cur_vec = End()-Start();
			//datum_vec = pDatumJg->pEnd->GetInverseNodePos()-pDatumJg->pStart->GetInverseNodePos();
			//cur_vec = pEnd->GetInverseNodePos()-pStart->GetInverseNodePos();
			normalize(datum_vec);
			normalize(cur_vec);
			if(fabs(datum_vec*cur_vec)>=0.9)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
			{
				if(pOldDesEndPos->IsInDatumJgWingX())
					work_norm=pDatumJg->get_norm_x_wing();
				else if(pOldDesEndPos->IsInDatumJgWingY())
					work_norm=pDatumJg->get_norm_y_wing();
				else
					work_norm=datum_vec^cur_vec;
			}
			else
				work_norm = datum_vec^cur_vec;
			normalize(work_norm);
			if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
			{
				offset_vec_x = get_norm_x_wing();
				offset_vec_y = -GetWingVecX();
			}
			else
			{
				offset_vec_x = -GetWingVecY();
				offset_vec_y = get_norm_y_wing();
			}
		
			end_datum = end_ber-offset_vec_x*GetEndOffsetByWingX();
			end_datum = end_datum-offset_vec_y*GetEndOffsetByWingY();
			return end_datum;
		}
	/*}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
		return f3dPoint(0,0,0);
	}*/
}


double CLDSLineAngle::Test_GetStartOffsetByWingX()
{
	if(!m_bEnableTeG)
		getjgzj(xWingYZhunJu,size.wide);
	if(pOldDesStartPos->offset_X_dist_style==0)
		return xWingYZhunJu.g;
	else if(pOldDesStartPos->offset_X_dist_style==1)
		return xWingYZhunJu.g1;
	else if(pOldDesStartPos->offset_X_dist_style==2)
		return xWingYZhunJu.g2;
	else if(pOldDesStartPos->offset_X_dist_style==3)
		return xWingYZhunJu.g3;
	else //if(pOldDesStartPos->offset_X_dist_style==4)
		return pOldDesStartPos->offset_X_dist;
}

double CLDSLineAngle::Test_GetStartOffsetByWingY()
{
	if(!m_bEnableTeG)
		getjgzj(xWingXZhunJu,size.wide);
	if(pOldDesStartPos->offset_Y_dist_style==0)
		return xWingXZhunJu.g;
	else if(pOldDesStartPos->offset_Y_dist_style==1)
		return xWingXZhunJu.g1;
	else if(pOldDesStartPos->offset_Y_dist_style==2)
		return xWingXZhunJu.g2;
	else if(pOldDesStartPos->offset_Y_dist_style==3)
		return xWingXZhunJu.g3;
	else //if(pOldDesStartPos->offset_Y_dist_style==4)
		return pOldDesStartPos->offset_Y_dist;
}
double CLDSLineAngle::Test_GetEndOffsetByWingX()
{
	if(!m_bEnableTeG)
		getjgzj(xWingYZhunJu,size.wide);
	if(pOldDesEndPos->offset_X_dist_style==0)
		return xWingYZhunJu.g;
	else if(pOldDesEndPos->offset_X_dist_style==1)
		return xWingYZhunJu.g1;
	else if(pOldDesEndPos->offset_X_dist_style==2)
		return xWingYZhunJu.g2;
	else if(pOldDesEndPos->offset_X_dist_style==3)
		return xWingYZhunJu.g3;
	else //if(pOldDesEndPos->offset_X_dist_style==4)
		return pOldDesEndPos->offset_X_dist;
}
double CLDSLineAngle::Test_GetEndOffsetByWingY()
{
	if(!m_bEnableTeG)
		getjgzj(xWingXZhunJu,size.wide);
	if(pOldDesEndPos->offset_Y_dist_style==0)
		return xWingXZhunJu.g;
	else if(pOldDesEndPos->offset_Y_dist_style==1)
		return xWingXZhunJu.g1;
	else if(pOldDesEndPos->offset_Y_dist_style==2)
		return xWingXZhunJu.g2;
	else if(pOldDesEndPos->offset_Y_dist_style==3)
		return xWingXZhunJu.g3;
	else //if(pOldDesEndPos->offset_Y_dist_style==4)
		return pOldDesEndPos->offset_Y_dist;
}
void CLDSLineAngle::Test_UpdateRelateRayJgPos()
{
	BelongModel()->PushPartStack();
	for(CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->EnumPartFirst(CLS_LINEANGLE);pLineAngle;
		pLineAngle=(CLDSLineAngle*)BelongModel()->EnumPartNext(CLS_LINEANGLE))
	{
		if( pLineAngle->pOldDesStartPos->datum_jg_h==handle ||
			(pLineAngle->pOldDesStartPos->datum_jg_h==0&&pLineAngle->pStart&&pLineAngle->pStart->hFatherPart==handle))
		{
			if(pLineAngle->pOldDesStartPos->datum_pos_style==2)
				pLineAngle->pOldDesStartPos->datum_pos_style=3;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==3)
				pLineAngle->pOldDesStartPos->datum_pos_style=2;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==5)
				pLineAngle->pOldDesStartPos->datum_pos_style=6;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==6)
				pLineAngle->pOldDesStartPos->datum_pos_style=5;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==8)
				pLineAngle->pOldDesStartPos->datum_pos_style=9;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==9)
				pLineAngle->pOldDesStartPos->datum_pos_style=8;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==11)
				pLineAngle->pOldDesStartPos->datum_pos_style=12;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==12)
				pLineAngle->pOldDesStartPos->datum_pos_style=11;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==13)
				pLineAngle->pOldDesStartPos->datum_pos_style=14;
			else if(pLineAngle->pOldDesStartPos->datum_pos_style==14)
				pLineAngle->pOldDesStartPos->datum_pos_style=13;
			pLineAngle->UnlockStart();
			pLineAngle->UnlockEnd();
			pLineAngle->CalPosition();
		}
		else if( pLineAngle->pOldDesEndPos->datum_jg_h==handle ||
			(pLineAngle->pOldDesEndPos->datum_jg_h==0&&pLineAngle->pEnd&&pLineAngle->pEnd->hFatherPart==handle))
		{
			if(pLineAngle->pOldDesEndPos->datum_pos_style==2)
				pLineAngle->pOldDesEndPos->datum_pos_style=3;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==3)
				pLineAngle->pOldDesEndPos->datum_pos_style=2;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==5)
				pLineAngle->pOldDesEndPos->datum_pos_style=6;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==6)
				pLineAngle->pOldDesEndPos->datum_pos_style=5;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==8)
				pLineAngle->pOldDesEndPos->datum_pos_style=9;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==9)
				pLineAngle->pOldDesEndPos->datum_pos_style=8;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==11)
				pLineAngle->pOldDesEndPos->datum_pos_style=12;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==12)
				pLineAngle->pOldDesEndPos->datum_pos_style=11;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==13)
				pLineAngle->pOldDesEndPos->datum_pos_style=14;
			else if(pLineAngle->pOldDesEndPos->datum_pos_style==14)
				pLineAngle->pOldDesEndPos->datum_pos_style=13;
			pLineAngle->UnlockStart();
			pLineAngle->UnlockEnd();
			pLineAngle->CalPosition();
		}
	}
	BelongModel()->PopPartStack();
}
void CLDSLineAngle::Test_UpdateConnectInfo(CLDSNode *pNode)
{
	if(pNode==NULL||pStart==NULL||pEnd==NULL)
		return;
	long hFatherPart;
	CConnectInfo *pConnectInfo;
	CJgBerPosPara *pDesPosPara;
	if(pNode->handle==pStart->handle)
	{	//ʼ������
		hFatherPart=pStart->hFatherPart;
		pConnectInfo=&connectStart;
		pDesPosPara=pOldDesStartPos;
	}
	else if(pNode->handle==pEnd->handle)
	{	//�ն�����
		hFatherPart=pEnd->hFatherPart;
		pConnectInfo=&connectEnd;
		pDesPosPara=pOldDesEndPos;
	}
	else 
		return;
	if(pDesPosPara->datum_jg_h>0)	//ָ����ӻ�׼�Ǹ�
		hFatherPart=pDesPosPara->datum_jg_h;
	
	if(hFatherPart==handle)	//�˽ڵ㸸�˼����ǵ�ǰ�˼���������Ϊ��Ҫ��˫֫����
		pConnectInfo->m_iConnectWing=0;
	else if(pDesPosPara->jgber_cal_style!=0 ||	//�Ǵ�Ӷ�λ
		pDesPosPara->IsInDatumJgBer()&&toupper(layer(1))=='Z')	//���Ĵ�����һ������
		pConnectInfo->m_iConnectWing=0;
	else
	{
		f3dPoint datum_norm;
		CSuperSmartPtr<CLDSLinePart> pDatumLinePart;
		pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hFatherPart);
		if(pDatumLinePart.IsNULL()||!pDatumLinePart->IsLinePart())
			return;
		if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if(pDesPosPara->IsInDatumJgWingX())
				datum_norm=pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
			else if(pDesPosPara->IsInDatumJgWingY())
				datum_norm=pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
		}
		else
		{
			f3dPoint datum_vec,line_vec;
			datum_vec=pDatumLinePart->End()-pDatumLinePart->Start();
			line_vec=End()-Start();
			datum_norm=datum_vec^line_vec;
			normalize(datum_norm);
		}
		if( fabs(datum_norm*get_norm_x_wing())>fabs(datum_norm*get_norm_y_wing()))
			pConnectInfo->m_iConnectWing=1;	//X֫����
		else
			pConnectInfo->m_iConnectWing=2;	//Y֫����
	}
}
#endif
#endif