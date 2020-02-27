#include "stdafx.h"
#include "Tower.h"
#include "TubeDrawing.h"
#include "partgroupdrawing.h"
#include "SortFunc.h"
#include "DrawUnit.h"
#include "SysPara.h"
#include "GlobalFunc.h"
#include "LogFile.h"

CTubeDrawing::TUBE_DRAWING_STYLE::TUBE_DRAWING_STYLE(){
	bDrawExterCircle=bDimSectSlotAngle=bDrawWeldline=true;
	bMandatoryDimLabel=false;
	hSectSlotPlate=ciDrawModeH0V1=0;
}
CTubeDrawing::CTubeDrawing()
{
	style.bDrawExterCircle=true;
}
GEPOINT RotatePlaneNormalToDrawing(const double* vPlaneNorm,GECS cs);//defined in PartGroup.cpp
void CTubeDrawing::CreateFlatLayingDrawing(CLDSLineTube *pLineTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext)
{	//ƽ�̻��Ƹֹ�
	f3dLine line;	//��ʱ������
	f3dArcLine arcline;
	CTextFrame data_point;//���ݵ�
	//��ʼ�����߿��ı�
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	f3dPoint point,len_vec,norm_vec,start_weld,end_weld;
	len_vec=pLineTube->End()-pLineTube->Start();
	normalize(len_vec);//�˼����ȷ���(��ʼ��ָ��ĩ��)
	// ____________________   up outter line: start_firs,end_firs
	//|��������������������|��up inner line:start_sec,end_sec
	//|_ _ _ _ _ _ _ _ _ _ |  
	//|                    |  axis line: start_thir,end_thir
	//|��������������������|  down inner line: start_four,end_four
	// ��������������������   down outter line: start_fif,end_fif
	//��ʱֻ�ڸֹ���ʾ����ͼʱ���������չ��  wht 09-12-17
	BOOL bHasDimAngle=FALSE; //�ñ�����ʾ�Ƿ��Ѿ���ע��֧�����麸������֮��ļн�
	f3dLine upOutterLine,upInnerLine,axisLine,downInnerLine,downOutterLine,frontAxisLine;
	double Tube_thick=pLineTube->GetThick(),radius=pLineTube->GetDiameter()/2.0;
	double fMinDrawModelThick=pContext->TransToModelLength(sys.part_group_map.fMinDrawThick);
	Tube_thick = __max(pLineTube->GetThick(),fMinDrawModelThick);	//�ֹܱں���Ż���
	norm_vec=len_vec;
	RotateVectorAroundVector(norm_vec,Pi/2,dcs.PrjNorm());	//��ǰ����ƽ���ϴ�ֱ�ڸ˼���ƫ�Ʒ���
	axisLine.startPt=pLineTube->Start()-len_vec*pLineTube->startOdd();
	axisLine.endPt  =pLineTube->End()+len_vec*pLineTube->endOdd();
	frontAxisLine.startPt=axisLine.startPt+dcs.PrjNorm()*radius;
	frontAxisLine.endPt=axisLine.endPt+dcs.PrjNorm()*radius;
	upOutterLine.startPt=axisLine.startPt+norm_vec*radius;
	upOutterLine.endPt  =axisLine.endPt+norm_vec*radius;
	upInnerLine.startPt =upOutterLine.startPt-norm_vec*Tube_thick;
	upInnerLine.endPt   =upOutterLine.endPt-norm_vec*Tube_thick;
	downOutterLine.startPt =axisLine.startPt-norm_vec*radius;
	downOutterLine.endPt   =axisLine.endPt-norm_vec*radius;
	downInnerLine.startPt=downOutterLine.startPt+norm_vec*Tube_thick;
	downInnerLine.endPt  =downOutterLine.endPt+norm_vec*Tube_thick;
	axisLine.startPt-=len_vec*30;//׼�����˶����30mm
	axisLine.endPt+=len_vec*30;//׼�����˶����30mm
	//���Ƹֹܶ˲����ӹ���
	BOOL bStartTransect=FALSE,bEndTransect=FALSE,bStartPlate=FALSE,bEndPlate=FALSE;
	double start_transect_Tube_d=0,end_transect_Tube_d=0;
	if(pLineTube->m_tJointStart.type==0||(pLineTube->m_tJointStart.type==1&&pLineTube->m_tJointStart.hViceLinkObj<0))
	{	//ƽ���и��߻�Խ����
		if(!pLineTube->m_tJointStart.cutPlaneNorm.IsZero())
		{	//�и��淨����Ч
			GEPOINT vPrjCutPlaneNorm=pLineTube->m_tJointStart.cutPlaneNorm;
			GEPOINT xCutPlaneOrg=pLineTube->m_tJointStart.cutPlanePos;
			if(fabs(pLineTube->m_tJointStart.cutPlaneNorm*dcs.PrjNorm())>EPS)
			{
				Int3dlf(xCutPlaneOrg,f3dLine(pLineTube->Start(),pLineTube->End()),xCutPlaneOrg,vPrjCutPlaneNorm);
				vPrjCutPlaneNorm=RotatePlaneNormalToDrawing(pLineTube->m_tJointStart.cutPlaneNorm,dcs.cs);//dcs.cs.axis_x,dcs.cs.axis_y,dcs.cs.axis_z);
			}
			Int3dlf(upOutterLine.startPt,upOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(upInnerLine.startPt,upInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			//Int3dlf(axisLine.startPt,axisLine,pLineTube->m_tJointStart.cutPlanePos,vPrjCutPlaneNorm);
			Int3dlf(downInnerLine.startPt,downInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(downOutterLine.startPt,downOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
		/** ��ƽ���и�ֹ��������溸�ӵװ�Ĵ��ڣ������ӵװ����ʱ����нǶȱ�ע���ʴ˴��Ƕȱ�ע�������� wjh-2015.10.13
			//��ע�ֹ����췽����(�и����뵱ǰ������)����֮��ļн� wht 10-08-10
			f3dPoint vertex,start_vec,end_vec;
			Int3dlf(vertex,axisLine,pLineTube->m_tJointStart.cutPlanePos,pLineTube->m_tJointStart.cutPlaneNorm);
			start_vec=len_vec;
			end_vec=pLineTube->m_tJointStart.cutPlaneNorm^dcs.PrjNorm();
			normalize(end_vec);
			if(start_vec*end_vec<0)
				end_vec*=-1.0;
			pContext->AppendAngularDim(vertex,start_vec,end_vec,0.3*pLineTube->GetDiameter());
			bHasDimAngle=TRUE;
		*/
		}
	}
	else if(pLineTube->m_tJointStart.hLinkObj>0&&pLineTube->m_tJointStart.type==1&&pLineTube->m_tJointStart.hViceLinkObj>=0)
	{	//���Ƹֹ������(�˴�������Խ����)
		CLDSLineTube *pTempLineTube=NULL;
		pTempLineTube = (CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_LINETUBE);
		if(pTempLineTube)
		{	//�����͸ֹܵĵ�һ������
			bStartTransect=TRUE;
			start_transect_Tube_d=pTempLineTube->GetDiameter();
			f3dPoint inters1, inters2, line_vec=upOutterLine.startPt-upOutterLine.endPt;
			int n=Int3dlc(&inters1,&inters2,upOutterLine.endPt,line_vec,
				pTempLineTube->Start(),pTempLineTube->End(),pTempLineTube->GetDiameter()/2,pTempLineTube->GetDiameter()/2);
			if(n==1)
				upOutterLine.startPt = inters1;
			if(n==2)
			{
				if(DISTANCE(inters1,upOutterLine.endPt)<DISTANCE(inters2,upOutterLine.endPt))
					upOutterLine.startPt = inters1;
				else
					upOutterLine.startPt = inters2;
			}	
			n = 0;
			//�����͸ֹܵĵڶ�������
			n=Int3dlc(&inters1,&inters2,downOutterLine.endPt,line_vec,
				pTempLineTube->Start(),pTempLineTube->End(),pTempLineTube->GetDiameter()/2,pTempLineTube->GetDiameter()/2);
			if(n==1)
				downOutterLine.startPt = inters1;
			if(n==2)
			{
				if(DISTANCE(inters1,downOutterLine.endPt)<DISTANCE(inters2,downOutterLine.endPt))
					downOutterLine.startPt = inters1;
				else
					downOutterLine.startPt = inters2;
			}	
			n = 0;
			//�����͸ֹܵĵ���������
			n=Int3dlc(&inters1,&inters2,frontAxisLine.endPt,line_vec,
				pTempLineTube->Start(),pTempLineTube->End(),pTempLineTube->GetDiameter()/2,pTempLineTube->GetDiameter()/2);
			if(n==1)
				frontAxisLine.startPt = inters1;
			if(n==2)
			{
				if(DISTANCE(inters1,frontAxisLine.endPt)<DISTANCE(inters2,frontAxisLine.endPt))
					frontAxisLine.startPt = inters1;
				else
					frontAxisLine.startPt = inters2;
			}
			//���������ڲ�������ɵ�Ӱ��
			Int3dlf(upInnerLine.startPt,upInnerLine.startPt,line_vec,upOutterLine.startPt,line_vec);
			Int3dlf(downInnerLine.startPt,downInnerLine.startPt,line_vec,downOutterLine.startPt,line_vec);
			//�жϵ�ǰ���ֹ��Ƿ���Ҫ��ע����
			f3dPoint axis_temp=pTempLineTube->Start()-pTempLineTube->End();
			normalize(axis_temp);
			f3dPoint work_norm=axis_temp^len_vec;
			normalize(work_norm);
			if(fabs(work_norm*dcs.PrjNorm())>EPS_COS2)
			{	//��ע���ֹ������ֹܽ��㵽��һ��(������Ƥ��ֹܶ˻����������˨)�ľ��� wht 10-08-08
				f3dPoint end_pos=pLineTube->End()+len_vec*pLineTube->endOdd();
				CLDSParamPlate *pEndParamPlate=NULL;
				pEndParamPlate=(CLDSParamPlate*)pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pEndParamPlate&&(pEndParamPlate->m_iParamType==TYPE_FL||
					pEndParamPlate->m_iParamType==TYPE_FLG||pEndParamPlate->m_iParamType==TYPE_FLR))
				{
					double H=0;
					pEndParamPlate->GetDesignItemValue('H',&H);
					end_pos+=len_vec*H;
				}
				CSizeTextDim space_dim;
				space_dim.dimStartPos=frontAxisLine.startPt;
				space_dim.dimEndPos=end_pos;
				//space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
				space_dim.dimStartPos=dcs.TransToDraw(space_dim.dimStartPos);
				space_dim.dimEndPos=dcs.TransToDraw(space_dim.dimEndPos);
				space_dim.dimStartPos.z=space_dim.dimEndPos.z=0;
				space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
				space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
				space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
				space_dim.iOrder = 0;	//װ�����߳ߴ�
				space_dim.nHits=1;
				pContext->AppendSizeTextDim(space_dim);
				//spaceDimList.append(space_dim);
			}
		}
	}
	else if(pLineTube->m_tJointStart.hLinkObj>0&&pLineTube->m_tJointStart.type>1)
	{	//�˰�����
		CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		if(pParamPlate&&(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND))
		{
			double M=0,L=0;
			pParamPlate->GetDesignItemValue('M', &M);
			pParamPlate->GetDesignItemValue('L', &L);
			//�����ֹ����߳���
			axisLine.startPt-=len_vec*(L-M);
			start_weld	= pParamPlate->ucs.origin + radius*pParamPlate->ucs.axis_y;
			end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
			start_weld=dcs.TransToDraw(start_weld);
			end_weld=dcs.TransToDraw(end_weld);
			start_weld.z=end_weld.z;
			if(style.bDrawWeldline&&fabs(pParamPlate->ucs.axis_z*dcs.PrjNorm())>EPS_COS2)
			{	//���ƶ˰�����ʱ����Ҫ���ƺ�����
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					pContext->DrawWeldLineSymbol(start_weld,end_weld);
				else 
					pContext->DrawWeldLineSymbol(end_weld,start_weld);	
			}
			start_weld	= pParamPlate->ucs.origin - radius*pParamPlate->ucs.axis_y;
			end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
			start_weld=dcs.TransToDraw(start_weld);
			end_weld=dcs.TransToDraw(end_weld);
			start_weld.z=end_weld.z;
			if(style.bDrawWeldline&&fabs(pParamPlate->ucs.axis_z*dcs.PrjNorm())>EPS_COS2)
			{	//���ƶ˰�����ʱ����Ҫ���ƺ�����
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					pContext->DrawWeldLineSymbol(end_weld,start_weld);
				else 
					pContext->DrawWeldLineSymbol(start_weld,end_weld);
			}
		}
		else if(pParamPlate)//�����ֹ����߳���
		{
			axisLine.startPt-=len_vec*(pParamPlate->GetThick());
			if(pParamPlate->IsFL())
				bStartPlate=TRUE;
		}
	}
	if(pLineTube->m_tJointEnd.type==0||(pLineTube->m_tJointEnd.type==1&&pLineTube->m_tJointEnd.hViceLinkObj<0))
	{	//ƽ���и��߻�Խ����
		if(!pLineTube->m_tJointEnd.cutPlaneNorm.IsZero())
		{	//�и��淨����Ч
			GEPOINT vPrjCutPlaneNorm=pLineTube->m_tJointEnd.cutPlaneNorm;
			GEPOINT xCutPlaneOrg=pLineTube->m_tJointEnd.cutPlanePos;
			if(fabs(pLineTube->m_tJointEnd.cutPlaneNorm*dcs.PrjNorm())>EPS)
			{
				Int3dlf(xCutPlaneOrg,f3dLine(pLineTube->Start(),pLineTube->End()),xCutPlaneOrg,vPrjCutPlaneNorm);
				vPrjCutPlaneNorm=RotatePlaneNormalToDrawing(pLineTube->m_tJointEnd.cutPlaneNorm,dcs.cs);//dcs.cs.axis_x,dcs.cs.axis_y,dcs.cs.axis_z);
			}
			Int3dlf(upOutterLine.endPt,upOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(upInnerLine.endPt,upInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			//Int3dlf(axisLine.endPt,axisLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(downInnerLine.endPt,downInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(downOutterLine.endPt,downOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
		/** ��ƽ���и�ֹ��������溸�ӵװ�Ĵ��ڣ������ӵװ����ʱ����нǶȱ�ע���ʴ˴��Ƕȱ�ע�������� wjh-2015.10.13
			//��ע�ֹ����췽����(�и����뵱ǰ������)����֮��ļн� wht 10-08-10
			f3dPoint vertex,start_vec,end_vec;
			Int3dlf(vertex,axisLine,pLineTube->m_tJointEnd.cutPlanePos,pLineTube->m_tJointEnd.cutPlaneNorm);
			start_vec=len_vec*-1.0;
			end_vec=pLineTube->m_tJointEnd.cutPlaneNorm^dcs.PrjNorm();
			normalize(end_vec);
			if(start_vec*end_vec<0)
				end_vec*=-1.0;
			pContext->AppendAngularDim(vertex,start_vec,end_vec,0.3*pLineTube->GetDiameter());
			bHasDimAngle=TRUE;
		*/
		}
	}
	else if(pLineTube->m_tJointEnd.hLinkObj>0&&pLineTube->m_tJointEnd.type==1&&pLineTube->m_tJointEnd.hViceLinkObj>=0)	//�ն˴��������
	{	//���Ƹֹ������(�˴�������Խ����)
		CLDSLineTube* pTempLineTube = NULL;
		pTempLineTube=(CLDSLineTube*)pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_LINETUBE);
		if(pTempLineTube)
		{	//�����͸ֹܵĵ�һ������
			bEndTransect=TRUE;
			end_transect_Tube_d=pTempLineTube->GetDiameter();
			f3dPoint inters1, inters2, line_vec=upOutterLine.endPt-upOutterLine.startPt;
			int n=Int3dlc(&inters1,&inters2,upOutterLine.startPt,line_vec,
				pTempLineTube->Start(),pTempLineTube->End(),pTempLineTube->GetDiameter()/2,pTempLineTube->GetDiameter()/2);
			if(n==1)
				upOutterLine.endPt = inters1;
			if(n==2)
			{
				if(DISTANCE(inters1,upOutterLine.startPt)<DISTANCE(inters2,upOutterLine.startPt))
					upOutterLine.endPt = inters1;
				else
					upOutterLine.endPt = inters2;
			}	
			n = 0;
			//�����͸ֹܵĵڶ�������
			n=Int3dlc(&inters1,&inters2,downOutterLine.startPt,line_vec,
				pTempLineTube->Start(),pTempLineTube->End(),pTempLineTube->GetDiameter()/2,pTempLineTube->GetDiameter()/2);
			if(n==1)
				downOutterLine.endPt = inters1;
			if(n==2)
			{
				if(DISTANCE(inters1,downOutterLine.startPt)<DISTANCE(inters2,downOutterLine.startPt))
					downOutterLine.endPt = inters1;
				else
					downOutterLine.endPt = inters2;
			}	
			n = 0;
			//�����͸ֹܵĵ���������
			n=Int3dlc(&inters1,&inters2,frontAxisLine.startPt,line_vec,
				pTempLineTube->Start(),pTempLineTube->End(),pTempLineTube->GetDiameter()/2,pTempLineTube->GetDiameter()/2);
			if(n==1)
				frontAxisLine.endPt = inters1;
			if(n==2)
			{
				if(DISTANCE(inters1,frontAxisLine.startPt)<DISTANCE(inters2,frontAxisLine.startPt))
					frontAxisLine.endPt = inters1;
				else
					frontAxisLine.endPt = inters2;
			}	
			//���������ڲ�������ɵ�Ӱ��
			Int3dlf(upInnerLine.endPt,upInnerLine.endPt,line_vec,upOutterLine.endPt,line_vec);
			Int3dlf(downInnerLine.endPt,downInnerLine.endPt,line_vec,downOutterLine.endPt,line_vec);
			//�жϵ�ǰ���ֹ��Ƿ���Ҫ��ע����
			f3dPoint axis_temp=pTempLineTube->Start()-pTempLineTube->End();
			normalize(axis_temp);
			f3dPoint work_norm=axis_temp^len_vec;
			normalize(work_norm);
			if(fabs(work_norm*dcs.PrjNorm())>EPS_COS2)
			{	//��ע���ֹ������ֹܽ��㵽��һ��(������Ƥ��ֹܶ˻����������˨)�ľ��� wht 10-08-08
				f3dPoint start_pos=pLineTube->Start()-len_vec*pLineTube->startOdd();
				CLDSParamPlate *pStartParamPlate=NULL;
				pStartParamPlate=(CLDSParamPlate*)pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				if(pStartParamPlate&&(pStartParamPlate->m_iParamType==TYPE_FL||
					pStartParamPlate->m_iParamType==TYPE_FLG||pStartParamPlate->m_iParamType==TYPE_FLR))
				{
					double H=0;
					pStartParamPlate->GetDesignItemValue('H',&H);
					start_pos-=len_vec*H;
				}
				CSizeTextDim space_dim;
				space_dim.dimStartPos=frontAxisLine.endPt;
				space_dim.dimEndPos=start_pos;
				//space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
				space_dim.dimStartPos=dcs.TransToDraw(space_dim.dimStartPos);
				space_dim.dimEndPos=dcs.TransToDraw(space_dim.dimEndPos);
				space_dim.dimStartPos.z=space_dim.dimEndPos.z=0;
				space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
				space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
				space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
				space_dim.iOrder = 0;	//װ�����߳ߴ�
				space_dim.nHits=1;
				pContext->AppendSizeTextDim(space_dim);
				//spaceDimList.append(space_dim);
			}
		}
	}
	else if(pLineTube->m_tJointEnd.hLinkObj>0&&pLineTube->m_tJointEnd.type>1)
	{	//�˰�����
		CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pLineTube->BelongModel()->FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pParamPlate&&(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND))
		{
			double M=0,L=0;
			pParamPlate->GetDesignItemValue('M', &M);
			pParamPlate->GetDesignItemValue('L', &L);
			//�����ֹ����߳���
			axisLine.endPt+=len_vec*(L-M);
			start_weld	= pParamPlate->ucs.origin + radius*pParamPlate->ucs.axis_y;
			end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
			start_weld=dcs.TransToDraw(start_weld);
			end_weld=dcs.TransToDraw(end_weld);
			start_weld.z=end_weld.z;
			if(style.bDrawWeldline&&fabs(pParamPlate->ucs.axis_z*dcs.PrjNorm())>EPS_COS2)
			{	//���ƶ˰�����ʱ����Ҫ���ƺ�����
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					pContext->DrawWeldLineSymbol(start_weld,end_weld);
				else
					pContext->DrawWeldLineSymbol(end_weld,start_weld);
			}
			start_weld	= pParamPlate->ucs.origin - radius*pParamPlate->ucs.axis_y;
			end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
			start_weld=dcs.TransToDraw(start_weld);
			end_weld=dcs.TransToDraw(end_weld);
			start_weld.z=end_weld.z;
			
			if(style.bDrawWeldline&&fabs(pParamPlate->ucs.axis_z*dcs.PrjNorm())>EPS_COS2)
			{	//���ƶ˰�����ʱ����Ҫ���ƺ�����
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					pContext->DrawWeldLineSymbol(end_weld,start_weld);
				else
					pContext->DrawWeldLineSymbol(start_weld,end_weld);
			}
		}
		else if(pParamPlate) //�����ֹ����߳���
		{
			axisLine.endPt+=len_vec*(pParamPlate->GetThick());
			if(pParamPlate->IsFL())
				bEndPlate=TRUE;
		}
	}
	//�����ĸֹ��и���
	if(!pLineTube->cutPlaneNorm.IsZero())
	{	//�и��淨����Ч
		GEPOINT vPrjCutPlaneNorm=pLineTube->cutPlaneNorm;
		GEPOINT xCutPlaneOrg=pLineTube->cutPlaneOrigin;
		if(fabs(pLineTube->cutPlaneNorm*dcs.PrjNorm())>EPS)
		{
			Int3dlf(xCutPlaneOrg,f3dLine(pLineTube->Start(),pLineTube->End()),xCutPlaneOrg,vPrjCutPlaneNorm);
			vPrjCutPlaneNorm=RotatePlaneNormalToDrawing(pLineTube->cutPlaneNorm,dcs.cs);//dcs.cs.axis_x,dcs.cs.axis_y,dcs.cs.axis_z);
		}
		if(len_vec*pLineTube->cutPlaneNorm<0)
		{
			Int3dlf(upOutterLine.startPt,upOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(upInnerLine.startPt,upInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			//Int3dlf(axisLine.startPt,axisLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(downInnerLine.startPt,downInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(downOutterLine.startPt,downOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
		}
		else
		{
			Int3dlf(upOutterLine.endPt,upOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(upInnerLine.endPt,upInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			//Int3dlf(axisLine.endPt,axisLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(downInnerLine.endPt,downInnerLine,xCutPlaneOrg,vPrjCutPlaneNorm);
			Int3dlf(downOutterLine.endPt,downOutterLine,xCutPlaneOrg,vPrjCutPlaneNorm);
		}
	}
	//ת������ͼ����ϵ��
	upOutterLine.startPt=dcs.TransToDraw(upOutterLine.startPt);
	upOutterLine.endPt=dcs.TransToDraw(upOutterLine.endPt);
	upInnerLine.startPt=dcs.TransToDraw(upInnerLine.startPt);
	upInnerLine.endPt=dcs.TransToDraw(upInnerLine.endPt);
	axisLine.startPt=dcs.TransToDraw(axisLine.startPt);
	axisLine.endPt=dcs.TransToDraw(axisLine.endPt);
	frontAxisLine.startPt=dcs.TransToDraw(frontAxisLine.startPt);
	frontAxisLine.endPt=dcs.TransToDraw(frontAxisLine.endPt);
	downInnerLine.startPt=dcs.TransToDraw(downInnerLine.startPt);
	downInnerLine.endPt=dcs.TransToDraw(downInnerLine.endPt);
	downOutterLine.startPt=dcs.TransToDraw(downOutterLine.startPt);
	downOutterLine.endPt=dcs.TransToDraw(downOutterLine.endPt);
	upOutterLine.startPt.z=upOutterLine.endPt.z=downOutterLine.startPt.z=downOutterLine.endPt.z=
		axisLine.startPt.z=axisLine.endPt.z=frontAxisLine.startPt.z=frontAxisLine.endPt.z=
		upInnerLine.startPt.z=upInnerLine.endPt.z=downInnerLine.startPt.z=downInnerLine.endPt.z=0;
	//���˼����췽��ת������ͼ������
	vector_trans(len_vec,dcs,FALSE);
	len_vec.z=0;
	normalize(len_vec);
	double hide_length=pLineTube->GetDiameter();
	if(bStartTransect)	//ʼ�˴��������
	{
		if(start_transect_Tube_d==pLineTube->GetDiameter())
		{
			line.startPt=upOutterLine.startPt;
			line.endPt=frontAxisLine.startPt;
			line.pen.style=PS_SOLID;	//ͼ��0
			line.ID=0;	
			pContext->AppendLine(line);
			//edgeList.append(LineToArc(line));
			line.startPt=frontAxisLine.startPt;
			line.endPt=downOutterLine.startPt;
			pContext->AppendLine(line);
			//edgeList.append(LineToArc(line));
		}
		else	//����Spline��������
		{
			arcline.SetCenter(frontAxisLine.startPt);
			arcline.SetStart(upOutterLine.startPt);
			arcline.SetEnd(downOutterLine.startPt);
			line.pen.style=PS_USERSTYLE;	//Spline��������
			line.ID=0;
			pContext->AppendLine(arcline);
			//edgeList.append(arcline);
		}
		//CreateAcadSpline(pBlockTableRecord,upOutterLine.startPt,frontAxisLine.startPt,downOutterLine.startPt);
	}
	else
	{
		line.startPt=upOutterLine.startPt;
		line.endPt=downOutterLine.startPt;
		if(bStartPlate)
			line.pen.style=PS_DASH;	
		else
			line.pen.style=PS_SOLID;	//ͼ��0
		line.ID=0;	
		pContext->AppendLine(line);
		//edgeList.append(LineToArc(line));
	}
	if(bEndTransect)	//�ն˴��������
	{
		if(end_transect_Tube_d==pLineTube->GetDiameter())
		{
			line.startPt=upOutterLine.endPt;
			line.endPt=frontAxisLine.endPt;
			line.pen.style=PS_SOLID;	//ͼ��0
			line.ID=0;	
			pContext->AppendLine(line);
			//edgeList.append(LineToArc(line));
			line.startPt=frontAxisLine.endPt;
			line.endPt=downOutterLine.endPt;
			pContext->AppendLine(line);
			//edgeList.append(LineToArc(line));
		}
		else	//����Spline��������
		{
			arcline.SetCenter(frontAxisLine.endPt);
			arcline.SetStart(downOutterLine.endPt);
			arcline.SetEnd(upOutterLine.endPt);
			line.pen.style=PS_USERSTYLE;	//Splin��������
			line.ID=0;
			pContext->AppendLine(arcline);
			//edgeList.append(arcline);
		}
		//CreateAcadSpline(pBlockTableRecord,downOutterLine.endPt,frontAxisLine.endPt,upOutterLine.endPt);
	}
	else
	{
		line.startPt=upOutterLine.endPt;
		line.endPt=downOutterLine.endPt;
		if(bEndPlate)
			line.pen.style=PS_DASH;
		else
			line.pen.style=PS_SOLID;	//ͼ��0
		line.ID=0;	
		pContext->AppendLine(line);
		//edgeList.append(LineToArc(line));
	}
	line.startPt=axisLine.startPt;
	line.endPt=axisLine.endPt;
	line.pen.style=PS_DASHDOTDOT;	//ͼ��4
	line.ID=0;	//pLineTube->handle;	
	pContext->AppendLine(line);
	//edgeList.append(LineToArc(line));
	
	line.startPt=upOutterLine.startPt;
	line.endPt=upOutterLine.endPt;
	line.pen.style=PS_SOLID;	//ͼ��0
	line.ID=0;	
	pContext->AppendLine(line);
	//edgeList.append(LineToArc(line));
	line.startPt=downOutterLine.startPt;
	line.endPt=downOutterLine.endPt;
	line.pen.style=PS_SOLID;	//ͼ��0
	line.ID=0;	
	pContext->AppendLine(line);
	//edgeList.append(LineToArc(line));
	line.startPt=upInnerLine.startPt;
	line.endPt=upInnerLine.endPt;
	line.pen.style=PS_DASH;		//ͼ��1
	line.ID=0;	
	pContext->AppendLine(line);
	//edgeList.append(LineToArc(line));
	line.startPt=downInnerLine.startPt;
	line.endPt=downInnerLine.endPt;
	line.pen.style=PS_DASH;		//ͼ��1
	line.ID=0;	
	pContext->AppendLine(line);
	//edgeList.append(LineToArc(line));
	//
	if(pContext->IsSectionDrawing())
	{
		CSectionDrawingEntsManager *pSectionDrawingContext=(CSectionDrawingEntsManager*)pContext;
		pSectionDrawingContext->tube_edge_line[0]=upOutterLine;
		pSectionDrawingContext->tube_edge_line[1]=downOutterLine;
	}
	//��ǰ�ֹ��麸����׼�ֹ�
	len_vec=pLineTube->End()-pLineTube->Start();
	normalize(len_vec);
	//���߸ֹ�������ʾʱ����Ҫ��ע���׼�ֹܼн� wht 10-04-19
	double cosa=len_vec*dcs.PrjNorm(); 
	if(pContext->IsBranchRod()&&fabs(cosa)<EPS2&&!bHasDimAngle)
	{	//��ע��ǰ�ֹ�(֧��)���ͼ����ϵX���ļн�
		f3dPoint vertex,start_vec,end_vec;
		Int3dll(axisLine,f3dLine(f3dPoint(0,0,0),f3dPoint(100,0,0)),vertex);//����
		coord_trans(vertex,dcs,TRUE); //�˴���Ҫ�ĽǶȱ�ע����Ϊ��������	
		start_vec=dcs.cs.axis_x;	//����ʼ�߷���
		end_vec=pLineTube->End()-pLineTube->Start();
		if(pContext->WeldRootPart()&&pContext->WeldRootPart()->GetClassTypeId()==CLS_LINETUBE)
		{	//�麸����׼����Ϊ�ֹ�,�ж�֧���ն˻���ʼ�������ڻ�׼�ֹ���
			CLDSLineTube *pDatumLineTube=(CLDSLineTube*)pContext->WeldRootPart();
			if(pDatumLineTube->pStart&&pDatumLineTube->pEnd)
			{
				f3dLine datum_line;
				datum_line.startPt=pDatumLineTube->pStart->Position(false);
				datum_line.endPt=pDatumLineTube->pEnd->Position(false);
				//�ֹ��ն������ڻ�׼�ֹ���
				if(datum_line.PtInLine(pLineTube->pEnd->Position(false))!=0)
					end_vec*=-1.0;
			}
		}
		normalize(end_vec);		//��ֹ�߷���
		if(start_vec*end_vec<0)
			start_vec*=-1.0;
		pContext->AppendAngularDim(vertex,start_vec,end_vec,200);
	}
	//���������Ϣ��չ��
	data_point.cls_id=pLineTube->GetClassTypeId();
	data_point.hPart=pLineTube->handle;
	data_point.BuildFrame(GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrame));
	data_point.dataPos=0.5*(axisLine.startPt+axisLine.endPt);
	int str_len=(int)strlen(pLineTube->GetPartNo());
	data_point.dimPos=data_point.dataPos+dcs.cs.axis_y*str_len*0.5;
	pContext->AppendDataPoint(data_point);
	//
	for(CLsRef *pLsRef=pLineTube->GetFirstLsRef();pLsRef;pLsRef=pLineTube->GetNextLsRef())
	{
		if(fabs(pLsRef->GetLsPtr()->get_norm()*dcs.PrjNorm())>EPS_COS2)
		{
			f3dCircle ls_cir;
			ls_cir.centre=(*pLsRef)->ucs.origin;
			ls_cir.centre=dcs.TransToDraw(ls_cir.centre);
			ls_cir.centre.z=0;	//z�������
			ls_cir.radius=(*pLsRef)->get_d()*0.5;
			ls_cir.feature=ftoi((*pLsRef)->hole_d_increment*10);
			pContext->AppendCircle(ls_cir);
			//cirList.append(ls_cir);
		}
		else if(pLsRef->GetLsPtr()->IsFootNail())
		{
			double D;	// ��ñֱ��.
			double H;	// ��˨ͷ�߶�.
			double H2;	// ��ñ�߶�.
			f3dCircle ls_cir;
			ls_cir.centre=(*pLsRef)->ucs.origin;
			FindLsD_H((*pLsRef)->get_d(),&D,&H,&H2);
			line.startPt=ls_cir.centre+(*pLsRef)->get_L()*(*pLsRef)->get_norm();
			line.endPt=ls_cir.centre-H*(*pLsRef)->get_norm();
			line.startPt=dcs.TransToDraw(line.startPt);
			line.endPt=dcs.TransToDraw(line.endPt);
			line.startPt.z=line.endPt.z=0;
			line.pen.style=PS_SOLID;
			line.ID=0;
			pContext->AppendLine(line);
			//edgeList.append(LineToArc(line));
		}
	}

	/* ����Ϊ�ֹܴ�ֱ������ƴ��룬�ѱ�CreateUprightLayingDrawing()����ȡ��������ȷ�����ú�ɾ�� wjh-2016.8.11
	else if(h0_v1==1)
	{
		Int3dlf(point,pLineTube->Start(),len_vec,dcs.PrjOrigin(),dcs.PrjNorm());
		f3dPoint start_firs,start_sec;
		pLineTube->BuildUCS();	//�׹ܵȸֹ��п���δ��ʼ������ϵ
		start_firs=point+pLineTube->ucs.axis_x*0.5*pLineTube->GetDiameter();
		start_sec=point+pLineTube->ucs.axis_x*(0.5*pLineTube->GetDiameter()-pLineTube->GetThick());
		//ת������ͼ����ϵ��
		point=dcs.TransToDraw(point);
		start_firs=dcs.TransToDraw(start_firs);
		start_sec=dcs.TransToDraw(start_sec);
		//���Ƹֹܶ���
		arcline.CreateMethod1(point,start_firs,f3dPoint(0,0,1),2*Pi);
		//if(len_vec*cs.axis_z<0) //��ˮ��ֹܽ���Ӧ�������߻���
		//	arcline.pen.style=PS_DASH;	//PS_DASH ��ʾΪͼ��1
		//else
		arcline.pen.style=PS_SOLID;	//PS_SOLID ��ʾΪͼ��0
		arcline.ID=pLineTube->handle;
		pContext->AppendLine(arcline);
		//edgeList.append(arcline);
		arcline.CreateMethod1(point,start_sec,f3dPoint(0,0,1),2*Pi);
		//if(len_vec*cs.axis_z<0)
		//	arcline.pen.style=PS_DASH;	//PS_DASH ��ʾΪͼ��1
		//else
		arcline.pen.style=PS_SOLID;	
		pContext->AppendLine(arcline);
		//edgeList.append(arcline);
		if(pLineTube->handle==pContext->WeldRootPart()->handle)
		{	//�������� ����
			f3dPoint axis_start,axis_end,axis_x,axis_y;
			pLineTube->BuildUCS();
			//ת������ͼ����ϵ��
			vector_trans(len_vec,dcs,FALSE);
			f3dPoint datum_line_vec=pLineTube->ucs.axis_x;
			if(pLineTube->m_bHasWeldRoad)
			{	//�ֹ��к�����ʱ�ڴ˴����ƻ�׼�߷���
				double weldRoadAngle=pLineTube->CalWeldRoadAngle();
				RotateVectorAroundVector(datum_line_vec,weldRoadAngle,pLineTube->ucs.axis_z);
			}
			axis_x=datum_line_vec;
			vector_trans(axis_x,dcs,FALSE);
			axis_x.Set(axis_x.x-len_vec.x*axis_x.z/len_vec.z,axis_x.y-len_vec.y*axis_x.z/len_vec.z);
			normalize(axis_x);
			//������ת�ǶȻ�׼�ᣨ�����߷���
			double extend_len=max(pLineTube->GetDiameter(),25);
			line.startPt=point-axis_x*extend_len;
			line.endPt=point+axis_x*extend_len;
			line.ID=0;
			line.pen.style=PS_DASHDOTDOT;
			pContext->AppendLine(line);
			//���ƻ�׼�����Ӧ�Ĵ�ֱ��
			axis_y.Set(-axis_x.y,axis_x.x);
			line.startPt=point-axis_y*extend_len;
			line.endPt=point+axis_y*extend_len;
			line.ID=0;
			line.pen.style=PS_DASHDOTDOT;
			pContext->AppendLine(line);
			//���ƻ�׼��ı��
			IDbPolyline* pPolyline=(IDbPolyline*)pContext->AppendDbEntity(IDbEntity::DbPolyline);
			double radius=pLineTube->GetWidth()*0.5;
			f3dPoint weldSymbolStart=point+axis_x*(radius-pLineTube->GetThick());
			f3dPoint weldSymbolEnd=point+axis_x*radius;
			pPolyline->AddVertexAt(0,weldSymbolStart,pLineTube->GetThick()*0.2,pLineTube->GetThick()*1.6);
			pPolyline->AddVertexAt(1,weldSymbolEnd);
			if(pLineTube->m_bHasWeldRoad)
			{	//�ֹ��к�����ʱ�ڴ˴����ƻ�׼�߷���
				CTextOnlyDrawing textDraing;
				textDraing.angle=CalRotateAngle(f3dPoint(0,0,1),f3dPoint(1,0,0),axis_x);
				textDraing.dimPos=line.endPt;
				double angle=ReviseTextAngleToHeadUpward(textDraing.angle,true);
				if(angle!=textDraing.angle)
				{
					textDraing.angle=angle;
					textDraing.dimPos+=axis_x*sys.dim_map.fDimTextSize*3;
				}
#ifdef AFX_TARG_ENU_ENGLISH
				textDraing.dimText="weld road line";
#else
				textDraing.dimText="������";
#endif
				textDraing.iOrder=0;
				textDraing.bNeedConvertDistToText=false;
				textDraing.bInXWing=false;
				pContext->AppendTextDim(textDraing);
				//pureTextDimList.append(textDraing);
			}
		}
	}*/
}
typedef double(*GETSLOTLENGTHERROR)(DWORD iPlateType);
typedef double(*GETSLOTWIDTHERROR)(DWORD iPlateType);
bool CTubeDrawing::CreateSectSlotDrawing(CLDSLineTube *pTube,CLDSGeneralPlate* pInsertPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext)		//���ƺ����ڲ۵ĸֹܽ���ͼ
{
	//����ֹܽ���ͼ����ԭ��
	GEPOINT draw_center;
	Int3dlf(draw_center,f3dLine(pTube->Start(),pTube->End()),dcs.cs.origin,dcs.cs.axis_z);
	coord_trans(draw_center,dcs.cs,FALSE,TRUE);
	double draw_tube_d=pTube->GetDiameter();// /pContext->GetUser2ScrDrawScale();
	double draw_tube_t=pTube->GetThick();// /pContext->GetUser2ScrDrawScale();
	double half_d=draw_tube_d*0.5;
	//���Ƹֹܶ���ͼ����ע�ֹ�ֱ��
	CLDSPlate* pCurrPlate=NULL;
	CLDSParamPlate* pCurrParamPlate=NULL;
	if(pInsertPlate->GetClassTypeId()==CLS_PARAMPLATE)
		pCurrParamPlate=(CLDSParamPlate*)pInsertPlate;
	else
		pCurrPlate=(CLDSPlate*)pInsertPlate;
	double half_drawray_len=1.5*draw_tube_d;
	//���㵱ǰ����ְ��ڻ�ͼ����ϵ�µĽ��淨��
	GEPOINT vEndPlaneExtVec,vEndPlaneNorm=pInsertPlate->ucs.axis_z;
	vector_trans(vEndPlaneNorm,dcs.cs,FALSE,TRUE);
	vEndPlaneNorm.z=0;
	Standarized(vEndPlaneNorm);
	vEndPlaneExtVec.Set( vEndPlaneNorm.y,-vEndPlaneNorm.x);

	//��ȡ��ͬ���Ŀ�������
	char file_name[MAX_PATH];
	GetAppPath(file_name);
	char* finalSlash0=SearchChar(file_name,'/',true);
	char* finalSlash=SearchChar(file_name,'\\',true);
	if(finalSlash==NULL || finalSlash0>finalSlash)
		finalSlash=finalSlash0;
	if(finalSlash==NULL)
		strcat(file_name,"/");
	strcat(file_name,"SlotProcess.dll");
	HMODULE hInstance=LoadLibrary(file_name);
	if(hInstance != NULL)
	{	//���ݲ�ͬ���ҵĶ��ƶ�̬���ȡ��ͬ���Ϳ��۵��ݲ�ֵ����ԣ���� wjh-2016.8.15
		int iPlateType=pCurrParamPlate->m_iParamType;
		GETSLOTLENGTHERROR GetSlotLenError=(GETSLOTLENGTHERROR)GetProcAddress(hInstance,"GetSlotLengthError");
		if(GetSlotLenError!=NULL)
			sys.part_map.tube.fSlotLenProcessError=GetSlotLenError(iPlateType);
		GETSLOTWIDTHERROR GetSlotWidthError=(GETSLOTWIDTHERROR) GetProcAddress(hInstance,"GetSlotWidthError");
		if(GetSlotWidthError!=NULL)
			sys.part_map.tube.fSlotWidthProcessError=GetSlotWidthError(iPlateType);
		FreeLibrary(hInstance);
	}
	double draw_slot_width=0,draw_slot_len=0;
	if(pCurrParamPlate&&(pCurrParamPlate->m_iParamType==TYPE_ROLLEND||pCurrParamPlate->m_iParamType==TYPE_UEND))
	{	//���Ͳ���U�Ͳ��
		pCurrParamPlate->GetDesignItemValue('M',&draw_slot_len);
		pCurrParamPlate->GetDesignItemValue('W',&draw_slot_width);
	}
	else if(pCurrParamPlate&&pCurrParamPlate->m_iParamType==TYPE_XEND)
		pCurrParamPlate->GetDesignItemValue('M',&draw_slot_len);	//ʮ�ֲ��
	else	//�ݻ���֧��
		return false;
	if(pCurrParamPlate&&pCurrParamPlate->m_iParamType==TYPE_UEND)
		draw_slot_width+=pInsertPlate->GetThick()*2+sys.part_map.tube.fSlotWidthProcessError;
	else
		draw_slot_width =pInsertPlate->GetThick()+sys.part_map.tube.fSlotWidthProcessError;
	//draw_slot_width/=pContext->GetUser2ScrDrawScale();
	//draw_slot_len  /=pContext->GetUser2ScrDrawScale();
	f3dLine line;
	f3dArcLine arcline;
	f3dPoint vertex_arr[16];
	if(pCurrParamPlate&&(pCurrParamPlate->m_iParamType==TYPE_ROLLEND||pCurrParamPlate->m_iParamType==TYPE_UEND))
	{	//���Ͳ���U�Ͳ�壨���ѻ��ƻ���ǻ�׼�ߣ��������壬δ��ȷ�����ú�ȥ��) wjh-2016.10.09
		//line.pen.style=PS_DASHDOT;	//��ӦLayerTable::AxisLineLayer.layerId
		//line.startPt.Set(draw_center.x,draw_center.y+half_drawray_len);
		//line.endPt.Set(draw_center.x,draw_center.y-half_drawray_len);
		//pContext->AppendLine(line);	//CreateAcadLine(pBlockTableRecord,start_pt,end_pt);
	
		//����half_d�򿪲ۿ�ȵ��µ�΢ϸСƫ�� wjh-2016.8.15
		half_d=SQRT(draw_tube_d*draw_tube_d*0.25-0.25*draw_slot_width*draw_slot_width);
		vertex_arr[0].Set(+half_d,+0.5*draw_slot_width);
		vertex_arr[1].Set(-half_d,+0.5*draw_slot_width);
		vertex_arr[2].Set(-half_d,-0.5*draw_slot_width);
		vertex_arr[3].Set(+half_d,-0.5*draw_slot_width);
		vertex_arr[4].Set(+half_d-draw_tube_t,+0.5*draw_slot_width);
		vertex_arr[5].Set(-half_d+draw_tube_t,+0.5*draw_slot_width);
		vertex_arr[6].Set(-half_d+draw_tube_t,-0.5*draw_slot_width);
		vertex_arr[7].Set(+half_d-draw_tube_t,-0.5*draw_slot_width);
		for(int k=0;k<8;k++)
		{
			if(!vEndPlaneExtVec.IsEqual(1,0,0))
				RotateVectorAroundVector(vertex_arr[k],vEndPlaneExtVec.y,vEndPlaneExtVec.x,GEPOINT(0,0,1));
			vertex_arr[k]+=draw_center;
		}
		arcline.pen.style=line.pen.style=PS_SOLID;	//LayerTable::VisibleProfileLayer.layerId
		for(int i=0;i<4;i++)
		{
			line.startPt=vertex_arr[i];
			line.endPt=vertex_arr[i+4];
			pContext->AppendLine(line);	//CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+4]);
			//����Բ��Բ�ġ���ʼ���Լ���ֹ�����Բ����
			double drawR=i<2?draw_tube_d*0.5:draw_tube_d*0.5-draw_tube_t;
			arcline.CreateMethod3(vertex_arr[2*i],vertex_arr[2*i+1],f3dPoint(0,0,1),drawR,draw_center);
			pContext->AppendLine(arcline);
			//CreateAcadArcLine(pBlockTableRecord,draw_center,vertex_arr[2*i],vertex_arr[2*i+1]);
		}
		/*if(style.bDimSectSlotAngle)
		{
			//��ע���ۿ�� ���Ǹֹܿ��ۿ�ȼӹ����ֵ wht 10-08-18
			double fSoltWidth=pCurrParamPlate->thick+sys.part_map.tube.fSlotWidthProcessError;
			if(pCurrParamPlate->m_iParamType==TYPE_UEND)
				fSoltWidth=pCurrParamPlate->thick*2+W+sys.part_map.tube.fSlotWidthProcessError;	//U�Ͳ�忪�ڲۿ�� 2*T+W
			sDimText.Printf("%.f",fSoltWidth);
			if(j==0)
			{
				dim_pos=0.5*(vertex_arr[0]+vertex_arr[3]);
				dim_pos.y+=sys.dim_map.fDimTextSize*sDimText.Length();
				dim_pos.x+=sys.dim_map.fDimTextSize*1.5;
				dim_start.Set(vertex_arr[0].x+sys.dim_map.fDimTextSize,vertex_arr[0].y);
				dim_end.Set(vertex_arr[3].x+sys.dim_map.fDimTextSize,vertex_arr[3].y);
				//if(pDrawRect==NULL)
				DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,-0.5*Pi);
			}
			else
			{
				dim_pos=0.5*(vertex_arr[1]+vertex_arr[2]);
				dim_pos.y+=sys.dim_map.fDimTextSize*sDimText.Length();
				if(pDrawRect==NULL)
					dim_pos.x-=sys.dim_map.fDimTextSize*1.5;
				else 
					dim_pos.x-=1;
				DimAngleSize(pBlockTableRecord,vertex_arr[1],vertex_arr[2],dim_pos,sDimText,dimStyleId,-ACOS(vEndPlaneNorm.x)-0.5*Pi);
			}
		}*/
	}
	else if(pCurrParamPlate&&pCurrParamPlate->m_iParamType==TYPE_XEND)
	{	//ʮ�ֲ��
		double half_slot_w=draw_slot_width*0.5;
		//����half_d�򿪲ۿ�ȵ��µ�΢ϸСƫ�� wjh-2016.8.15
		half_d=SQRT(draw_tube_d*draw_tube_d*0.25-half_slot_w*half_slot_w);
		//�������ߣ����ѻ��ƻ���ǻ�׼�ߣ��������壬δ��ȷ�����ú�ȥ��) wjh-2016.10.09
		//line.startPt.Set(draw_center.x,draw_center.y+half_drawray_len);
		//line.endPt.Set(draw_center.x,draw_center.y-half_drawray_len);
		//line.pen.style=PS_DASHDOT;	//LayerTable::AxisLineLayer.layerId
		//pContext->AppendLine(line);	//CreateAcadLine(pBlockTableRecord,start_pt,end_pt);

		//Ӧ֧�����岻��ֱ�ڻ����ʮ�ֲ��Ŀ��ڲۻ��� wht 10-08-17
		CLDSPlate *pUpPlate=NULL,*pDownPlate=NULL;
		pCurrParamPlate->DesignPlate();
		pCurrParamPlate->GetVertPlateInXEndPlate(&pUpPlate,&pDownPlate);
		double cos_up=1.0,cos_down=1.0;
		f3dPoint up_vec(0,1,0),down_vec(0,-1,0);
		const double TEMP_EPS_COS = 0.99999847691328769880290124792571;//=cos(0.1��)
		if(pUpPlate&&pDownPlate)
		{
			cos_up  =fabs(pUpPlate->ucs.axis_z  *pCurrParamPlate->ucs.axis_y);
			cos_down=fabs(pDownPlate->ucs.axis_z*pCurrParamPlate->ucs.axis_y);
			up_vec=pUpPlate->ucs.axis_y;
			down_vec=pDownPlate->ucs.axis_y;
			vector_trans(up_vec,dcs.cs,FALSE,TRUE);
			up_vec.Set(up_vec.x,up_vec.y);
			vector_trans(down_vec,dcs.cs,FALSE,TRUE);
			down_vec.Set(down_vec.x,down_vec.y);
			GECS drawcs;
			drawcs.axis_x=vEndPlaneExtVec;
			drawcs.axis_y=vEndPlaneNorm;
			drawcs.axis_z.Set(0,0,1);
			up_vec=drawcs.TransVToCS(up_vec);
			down_vec=drawcs.TransVToCS(down_vec);
			Standarized(up_vec);
			Standarized(down_vec);
		}
		if(fabs(cos_up)>TEMP_EPS_COS&&fabs(cos_down)>TEMP_EPS_COS)
		{
			vertex_arr[0].Set(+half_d,+half_slot_w);
			vertex_arr[1].Set(+half_slot_w,+half_d);
			vertex_arr[2].Set(-half_slot_w,+half_d);
			vertex_arr[3].Set(-half_d,+half_slot_w);
			vertex_arr[4].Set(-half_d,-half_slot_w);
			vertex_arr[5].Set(-half_slot_w,-half_d);
			vertex_arr[6].Set(+half_slot_w,-half_d);
			vertex_arr[7].Set(+half_d,-half_slot_w);
			vertex_arr[8].Set(+half_d-draw_tube_t,+half_slot_w);
			vertex_arr[9].Set(+half_slot_w,+half_d-draw_tube_t);
			vertex_arr[10].Set(-half_slot_w,+half_d-draw_tube_t);
			vertex_arr[11].Set(-half_d+draw_tube_t,+half_slot_w);
			vertex_arr[12].Set(-half_d+draw_tube_t,-half_slot_w);
			vertex_arr[13].Set(-half_slot_w,-half_d+draw_tube_t);
			vertex_arr[14].Set(+half_slot_w,-half_d+draw_tube_t);
			vertex_arr[15].Set(+half_d-draw_tube_t,-half_slot_w);
		}
		else
		{	//���Ʒ�90�㿪�ڲ�
			vertex_arr[0].Set(+half_d,+half_slot_w);
			f3dPoint norm_vec(-up_vec.y,up_vec.x),temp_pt=draw_center+up_vec*half_d;
			vertex_arr[1]=temp_pt-norm_vec*half_slot_w;
			vertex_arr[2]=temp_pt+norm_vec*half_slot_w;
			vertex_arr[3].Set(-half_d,+half_slot_w);
			vertex_arr[4].Set(-half_d,-half_slot_w);
			norm_vec.Set(-down_vec.y,down_vec.x);
			temp_pt=draw_center+down_vec*half_d;
			vertex_arr[5]=temp_pt-norm_vec*half_slot_w;
			vertex_arr[6]=temp_pt+norm_vec*half_slot_w;
			vertex_arr[7].Set(+half_d,-half_slot_w);
			vertex_arr[8].Set(+half_d-draw_tube_t,+half_slot_w);
			norm_vec.Set(-up_vec.y,up_vec.x);
			temp_pt=draw_center+up_vec*(half_d-draw_tube_t);
			vertex_arr[9]=temp_pt-norm_vec*half_slot_w;
			vertex_arr[10]=temp_pt+norm_vec*half_slot_w;
			vertex_arr[11].Set(-half_d+draw_tube_t,+half_slot_w);
			vertex_arr[12].Set(-half_d+draw_tube_t,-half_slot_w);
			norm_vec.Set(-down_vec.y,down_vec.x);
			temp_pt=draw_center+down_vec*(half_d-draw_tube_t);
			vertex_arr[13]=temp_pt-norm_vec*half_slot_w;
			vertex_arr[14]=temp_pt+norm_vec*half_slot_w;
			vertex_arr[15].Set(+half_d-draw_tube_t,-half_slot_w);
		}
		line.pen.style=arcline.pen.style=PS_SOLID;	//LayerTable::VisibleProfileLayer.layerId
		for(int k=0;k<16;k++)
		{
			if(!vEndPlaneExtVec.IsEqual(1,0,0))
				RotateVectorAroundVector(vertex_arr[k],vEndPlaneExtVec.y,vEndPlaneExtVec.x,GEPOINT(0,0,-1));
			vertex_arr[k]+=draw_center;
		}
		for(int i=0;i<8;i++)
		{
			line.startPt=vertex_arr[i];
			line.endPt=vertex_arr[i+8];
			pContext->AppendLine(line);	//CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+4]);
			//����Բ��Բ�ġ���ʼ���Լ���ֹ�����Բ����
			double drawR=i<4?draw_tube_d*0.5:draw_tube_d*0.5-draw_tube_t;
			arcline.CreateMethod3(vertex_arr[2*i],vertex_arr[2*i+1],f3dPoint(0,0,1),drawR,draw_center);
			pContext->AppendLine(arcline);
			//CreateAcadArcLine(pBlockTableRecord,draw_center,vertex_arr[2*i],vertex_arr[2*i+1]);
		}
		if(fabs(cos_up)<=TEMP_EPS_COS||fabs(cos_down)<=TEMP_EPS_COS)
		{			
			/*if(style.bDimSectSlotAngle)
			{
			//��ע�Ƕ�
			if(fabs(cos_up)<EPS_COS)
			{
				GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
				CreateAcadLine(pBlockTableRecord,draw_center,draw_center+up_vec*draw_tube_d);
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				start_pt=draw_center+up_vec*half_d;
				end_pt.Set(draw_center.x,draw_center.y+half_d);
				f3dPoint mid_vec=(start_pt-draw_center)+(end_pt-draw_center);
				normalize(mid_vec);
				dim_pos=draw_center+mid_vec*half_d*1.2;
				DimAngle(pBlockTableRecord,dimStyleId,draw_center,start_pt,end_pt,dim_pos);
			}
			if(fabs(cos_down)<EPS_COS)
			{
				GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
				CreateAcadLine(pBlockTableRecord,draw_center,draw_center+down_vec*draw_tube_d);
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				start_pt=draw_center+down_vec*half_d;
				end_pt.Set(draw_center.x,draw_center.y-half_d);
				f3dPoint mid_vec=(start_pt-draw_center)+(end_pt-draw_center);
				normalize(mid_vec);
				dim_pos=draw_center+mid_vec*(sys.dim_map.fDimAngleTextSize+half_d*1.2);
				DimAngle(pBlockTableRecord,dimStyleId,draw_center,start_pt,end_pt,dim_pos);
			}
			}*/
		}
		/*if(style.bDimSectSlotAngle)
		{
		//��ע���ۿ�� ���Ǹֹܿ��ۿ�ȼӹ����ֵ wht 10-08-18
		double fSoltWidth=pCurrParamPlate->thick+sys.part_map.tube.fSlotWidthProcessError;	
		sDimText.Printf("%.f",fSoltWidth);
		if(j==0)
		{
			dim_pos=0.5*(vertex_arr[0]+vertex_arr[7]);
			dim_pos.x+=sys.dim_map.fDimTextSize*1.5;
			dim_pos.y+=sys.dim_map.fDimTextSize*sDimText.Length();
			dim_start.Set(vertex_arr[0].x+sys.dim_map.fDimTextSize,vertex_arr[0].y);
			dim_end.Set(vertex_arr[7].x+sys.dim_map.fDimTextSize,vertex_arr[7].y);
			//if(pDrawRect==NULL)
			DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,-0.5*Pi);
		}
		else
		{
			dim_pos=0.5*(vertex_arr[3]+vertex_arr[4]);
			dim_pos.y+=sys.dim_map.fDimTextSize*sDimText.Length();
			if(pDrawRect==NULL)
				dim_pos.x-=sys.dim_map.fDimTextSize*1.5;
			else 
				dim_pos.x-=1;
			DimAngleSize(pBlockTableRecord,vertex_arr[3],vertex_arr[4],dim_pos,sDimText,dimStyleId,-ACOS(vEndPlaneNorm.x)-0.5*Pi);
		}
		//��ע���ۿ�� ���Ǹֹܿ��ۿ�ȼӹ����ֵ wht 10-08-18
		sDimText.Printf("%.f",fSoltWidth);
		dim_pos=0.5*(vertex_arr[5]+vertex_arr[6]);
		dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
		dim_pos.x=vertex_arr[6].x+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[5].x,vertex_arr[5].y-sys.dim_map.fDimTextSize);
		dim_end.Set(vertex_arr[6].x,vertex_arr[6].y-sys.dim_map.fDimTextSize);
		//if(pDrawRect==NULL)
		double dim_angle=0;
		if(fabs(cos_down)<EPS_COS)
		{	//��ע����б�Ƕ���ֱ���ڲۿ�� wht 10-08-18
			dim_start=vertex_arr[5]+down_vec*sys.dim_map.fDimTextSize*2;
			dim_end=vertex_arr[6]+down_vec*sys.dim_map.fDimTextSize*2;
			dim_pos=draw_center+down_vec*(half_d+sys.dim_map.fDimTextSize*2.2);
			dim_angle=Cal2dLineAng(f2dPoint(0,0),f2dPoint(-down_vec.y,down_vec.x));
		}
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,dim_angle);
		}*/
	}
	return true;
}
void CTubeDrawing::CreateUprightLayingDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext)
{
	UCS_STRU cs=dcs.cs;
	pTube->BuildUCS();
	double justify=fabs(pTube->ucs.axis_z*cs.axis_z);
	double radius=pTube->GetDiameter()*0.5;
	double drawThick=pTube->GetThick();	//ʵ�ʻ��Ƶĸֹܱں�
	if(justify<EPS2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("There is no intersection between the tube 0X%X axis and the current view,can't draw the cross-section of the tube.",pTube->handle);//,pDrawingView->m_pSectionView->sName
#else
		logerr.Log("0X%X�ֹ������뵱ǰ��ͼ�޽��㣬�޷����Ƹֹܽ���ͼ",pTube->handle);//,pDrawingView->m_pSectionView->sName
#endif
		return;	//δ�ҵ����㣬������
	}
	if(radius*(1-justify)<pContext->TransToModelLength(0.25))
		justify=1.0;	//(��)Բ��������ʱ������1mm���ɰ���׼Բ�򻯻���
	if(pContext->TransToModelLength(0.5)>pTube->GetThick())		//�ֹܱں�Ӧ��֤���Ƽ�϶��С��1mm;
		drawThick=pContext->TransToModelLength(0.5);
	f3dPoint center;
	if(Int3dlf(center,pTube->Start(),pTube->ucs.axis_z,cs.origin,cs.axis_z)<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("0X%X There is no intersection between the tube 0X%X axis and the current view,can't draw the cross-section of the tube.",pTube->handle);//,pDrawingView->m_pSectionView->sName
#else
		logerr.Log("0X%X�ֹ������뵱ǰ��ͼ�޽��㣬�޷����Ƹֹܽ���ͼ",pTube->handle);//,pDrawingView->m_pSectionView->sName
#endif
		return;	//δ�ҵ����㣬������
	}
	coord_trans(center,cs,FALSE,TRUE);	//Բ�ĵ�ת������ͼ����ϵ��
	PEN_STRU penStyle;
	penStyle.width=0;
	penStyle.crColor=0;
	penStyle.style=PS_SOLID;
	CSuperSmartPtr<CLDSPart> pInsertPlate=style.hSectSlotPlate>0?pTube->BelongModel()->FromPartHandle(style.hSectSlotPlate):NULL;
	if(pInsertPlate.IsHasPtr()&&!pInsertPlate->IsPlate())
		pInsertPlate=NULL;
	if(justify==1)	//��׼Բ����
	{
		if(pInsertPlate.IsNULL()||!CreateSectSlotDrawing(pTube,pInsertPlate.GeneralPlatePointer(),dcs,pContext))
		{
			IDbArcline* pArcline=(IDbArcline*)pContext->AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ��ڱڽ���Բ
			pArcline->CreateMethod1(center,center+f3dPoint(radius-drawThick),f3dPoint(0,0,1),Pi+Pi);
			pArcline->SetPen(penStyle);
			if(style.bDrawExterCircle)//(TagInfo()==NULL || TagInfo()->TubeSect()->bDrawExterCircle)
			{
				pArcline=(IDbArcline*)pContext->AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ���ڽ���Բ
				pArcline->CreateMethod1(center,center+f3dPoint(radius),f3dPoint(0,0,1),Pi+Pi);
				pArcline->SetPen(penStyle);
			}
		}
	}
	else
	{	//��б��̫�󣬱��밴��ʵ����Բ�������
		IDbArcline* pArcline=(IDbArcline*)pContext->AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ���ڽ�����Բ
		GEPOINT vxColAxisVec=pTube->ucs.axis_z;
		vector_trans(vxColAxisVec,cs,FALSE,TRUE);	//��ʼ��ת������ͼ����ϵ��
		GEPOINT vxStartEdgeVec=cs.axis_z^pTube->ucs.axis_z;
		normalize(vxStartEdgeVec);
		vector_trans(vxStartEdgeVec,cs,FALSE,TRUE);	//��ʼ��ת������ͼ����ϵ��
		GEPOINT vxEllipseStart=center+vxStartEdgeVec*radius;
		pArcline->CreateEllipse(center,vxEllipseStart,vxEllipseStart,vxColAxisVec,GEPOINT(0,0,1),radius);
		pArcline->SetPen(penStyle);
		pArcline=(IDbArcline*)pContext->AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ��ڱڽ�����Բ
		vxEllipseStart=center+vxStartEdgeVec*(radius-drawThick);
		pArcline->CreateEllipse(center,vxEllipseStart,vxEllipseStart,vxColAxisVec,GEPOINT(0,0,1),(radius-drawThick));
		pArcline->SetPen(penStyle);
	}
	//ת������ͼ����ϵ��
	f3dPoint len_vec=pTube->ucs.axis_z;
	vector_trans(len_vec,cs,FALSE,TRUE);				//��ͼ����ϵ�µĸֹ����߷���
	{
		//���Ƹֹܻ�׼�߻���Ǳ�ע��׼��(����׼�߹����ĵĿ�ֱ��������)
		GEPOINT datum_edgevec=pTube->ucs.axis_x;	//�ֹ�ʵ�ʼӹ�ʱ�ĽǶȻ�׼��(һ��Ϊ�����ߣ����޺��������Ըֹܻ�׼��Ϊ��׼)
		GEPOINT datum_edgevec_outer=datum_edgevec*(radius-pTube->size_thick);
		//����<����->��׼��>�ظֹ��������ͼƽ���ͶӰʸ��
		Int3dlf(datum_edgevec_outer,datum_edgevec_outer,pTube->ucs.axis_z,GEPOINT(0,0,0),cs.axis_z);
		vector_trans(datum_edgevec_outer,cs,FALSE,TRUE);	//��ͼ����ϵ�µĸֹ�<����->��ں���>������������ʸ��
		GEPOINT origin=center;
		GEPOINT arrowHead=origin+datum_edgevec_outer;
		GEPOINT arrowBtm=origin-datum_edgevec_outer;
		GEPOINT arrowStdVec=datum_edgevec_outer;
		normalize(arrowStdVec);
		arrowHead.z=arrowBtm.z=0;
		CSectionDrawingEntsManager* pDrawingContext=(CSectionDrawingEntsManager*)pContext;
		bool blBushTube = pTube->m_cFuncType == CLDSLineTube::FUNC_WIREBUSH || (pTube->pStart == NULL && pTube->pEnd == NULL);
		if(pDrawingContext&& !blBushTube)
		{	//�������׹�������
			pDrawingContext->AppendTubeBaseLine(1,pTube,arrowStdVec,arrowBtm,arrowHead);
			if(toupper(pTube->Layer(1))=='Z'&&toupper(pTube->Layer(0))!='T')
			{	//��ͷ�����ĸֹܶ���ͼ�������ָ�����ĵ�ʾ��ͼ
				GEPOINT intersPt;
				Int3dlf(intersPt,GEPOINT(0,0,0),GEPOINT(0,0,1),cs.origin,cs.axis_z);
				GEPOINT vec=intersPt-GEPOINT(cs.origin);
				vector_trans(vec,cs,FALSE,TRUE);
				vec.z=0;
				normalize(vec);
				GEPOINT xHead=vec*radius;
				GEPOINT xBtm(0,0,0);//=vec*(-radius);
				pDrawingContext->AppendTubeTaCoreVecLine(vec,xBtm,xHead);
			}
		}
	}
	f3dPoint datum_axis_x=pTube->ucs.axis_x;	//�ֹ�ʵ�ʼӹ�ʱ�ĽǶȻ�׼��(һ��Ϊ�����ߣ����޺��������Ըֹܻ�׼��Ϊ��׼)
	if(pTube->m_bHasWeldRoad)
	{	//�ֹ��к�����ʱ�Ժ�����Ϊ��ת�ǶȻ�׼��
		double weldRoadAngle=pTube->CalWeldRoadAngle();
		RotateVectorAroundVector(datum_axis_x,weldRoadAngle,pTube->ucs.axis_z);
	}
	f3dPoint weld_line_vec_outer=datum_axis_x*radius;
	f3dPoint weld_line_vec_inner=datum_axis_x*(radius-drawThick);
	vector_trans(weld_line_vec_outer,cs,FALSE,TRUE);	//��ͼ����ϵ�µĸֹ�<����->��ں���>������������ʸ��
	vector_trans(weld_line_vec_inner,cs,FALSE,TRUE);	//��ͼ����ϵ�µĸֹ�<����->�ڱں���>������������ʸ��
	//����<����->����>�ظֹ��������ͼƽ���ͶӰʸ��
	weld_line_vec_outer.Set(weld_line_vec_outer.x-len_vec.x*weld_line_vec_outer.z/len_vec.z,weld_line_vec_outer.y-len_vec.y*weld_line_vec_outer.z/len_vec.z);
	weld_line_vec_inner.Set(weld_line_vec_inner.x-len_vec.x*weld_line_vec_inner.z/len_vec.z,weld_line_vec_inner.y-len_vec.y*weld_line_vec_inner.z/len_vec.z);
	if(fabs(justify-1.0)<EPS)
	{	//�ܱ�Ϊ(����)Բ,��Բ����
		GEPOINT radialVec=weld_line_vec_outer.normalized();
		weld_line_vec_outer=radialVec*radius;
		weld_line_vec_inner=radialVec*(radius-drawThick);
	}

	double axis_len=weld_line_vec_outer.mod();
	f3dPoint datum_line_vec=weld_line_vec_outer/axis_len;		//��ȡ<����->����>�ĵ�λ����������
	double extend_len=pContext->TransToModelLength(5);	//��׼������X��Yʵ�ʻ��Ƴ���Ӧ��Բ������5mm
	//���ƺ��������Ǳ�Ƿ���
	if(pTube->m_bHasWeldRoad)	//Ϊ�˷���ʶ��ֹܻ���ǶȻ�׼���޺���Ҳ���ƻ�׼�߱��
	{	//�ֹ��к�����ʱͨ�������߻��ƺ�����
		IDbPolyline* pPolyline=(IDbPolyline*)pContext->AppendDbEntity(IDbEntity::DbPolyline);
		penStyle.style=PS_SOLID;
		pPolyline->SetPen(penStyle);
		f3dPoint weldSymbolStart=center+weld_line_vec_inner;
		f3dPoint weldSymbolEnd=center+weld_line_vec_outer;
		pPolyline->AddVertexAt(0,weldSymbolStart,drawThick*0.2,drawThick*1.6);
		pPolyline->AddVertexAt(1,weldSymbolEnd,0,0);
	}
	//���������Ϣ��չ��
	CTextFrame dataPoint;
	//��ʼ�����߿��ı�
	dataPoint.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	dataPoint.cls_id=pTube->GetClassTypeId();
	dataPoint.hPart=pTube->handle;
	dataPoint.BuildFrame(GetPartNoIncMat(pTube->GetPartNo(),pTube->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrame));
	dataPoint.dataPos=center+GEPOINT(-0.5*(radius-drawThick/2),0.866*(radius-drawThick/2));
	dataPoint.dimPos=dataPoint.dataPos+GEPOINT(-1,0,0);
	pContext->AppendDataPoint(dataPoint);
}

//void DrawTubeStruDrawing1(CLDSLineTube *pLineTube,CDrawCoordTrans dcs,IDbEntsContext* pContext,int h0v1=-1)
BOOL CTubeDrawing::CreateTubeDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId/*=NULL*/)
{
	GEPOINT len_vec=pTube->End()-pTube->Start();
	normalize(len_vec);//�˼����ȷ���(��ʼ��ָ��ĩ��)
	if(fabs(len_vec*dcs.PrjNorm())<0.707)
	{
		style.ciDrawModeH0V1=0;
		CreateFlatLayingDrawing(pTube,dcs,pContext);
	}
	else
	{
		style.ciDrawModeH0V1=1;
		CreateUprightLayingDrawing(pTube,dcs,pContext);
	}
	//֧�ܶ�����ͼ�е�֧�ܾ�ˮƽ����
	//if(m_pSectionView->m_iViewType==VIEW_BRANCHTUBE&&pLineTube->handle!=m_pRootPart->handle)
	//	h0v1=0;
	return FALSE;
}
