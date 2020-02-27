//<LOCALE_TRANSLATE/>
#include "stdafx.h"
#include "stdarx.h"
#include "Tower.h"
#include "DrawTaMap.h"
#include "SysPara.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "DimPlankLs.h"
#include "DimJgZhun.h"
#include "AdsTools.h"
#include "RxTools.h"
#include "dbxrecrd.h"
#include "DragEntSet.h"
#include "DrawUnit.h"
#include "SortFunc.h"
#include "SelPartGroupDlg.h"
#include "DrawingVectorDlg.h"
#include "ComparePartNoString.h"
#include "SlotArm.h"
#include "LogFile.h"
#include "StackAcadVar.h"

#ifndef __TSA_FILE_
//norm用于特殊指定需要绘制螺栓的工作方向,为NULL时缺省为ucs.axis_z
//最后的thick表示螺栓沿工作法线的负方向偏移的距离（基准点距离工作平面
//的距离，对于普通连接板上的螺栓就是连接板厚度，如果还有一根外铁，则再
//加上外铁的肢厚
void DrawLsRefList(CLsRefList *pLsRefList,CDrawCoordTrans dcs,BOOL bDrawAllLs/*=FALSE*/,COORD3D *pNorm/*=NULL*/)
{
	f3dPoint work_norm;
	if(pNorm==NULL)
		work_norm = dcs.PrjNorm();
	else
		work_norm = *pNorm;
	if(pLsRefList!=NULL)	//螺栓表不能为空
	{
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef!=NULL;pLsRef=pLsRefList->GetNext())
		{
			if(pLsRef->GetLsPtr()==NULL)
				continue;
			CLsDrawUnit *pLsDraw=LSDRAW.AppendLsRef(pLsRef->GetLsPtr(),FALSE,work_norm,&dcs.m_xSpreadLine,dcs.m_fSpreadAngle);
			if(bDrawAllLs&&pLsDraw)
				pLsDraw->front0_side1=0;
		}
	}
}

void DimJgZhun(AcDbBlockTableRecord *pBlockTableRecord,CDrawCoordTrans dcs,
			   CDimJgZhun &dim_jg_zhun, const JGZJ &jgzj,int now_g,BOOL bSpecialZhun=FALSE)
{
	int i,n;
	char dimText[8];
	CZhunXianTextDim zhun_dim;
	AcGePoint3d acad_dimStart,acad_dimEnd,acad_dimPos;
	DimStyleTable::dimStyle3.InitDimStyle(sys.dim_map.fDimTextSize);	//标注字高
	n = dim_jg_zhun.GetZhunDimCount();
	BOOL bDimG = FALSE; //是否已经标注过特殊G
	for(i=0;i<n;i++)
	{
		dim_jg_zhun.GetZhunDimAt(i,&zhun_dim);
		zhun_dim.lineStart=dcs.TransToDraw(zhun_dim.lineStart);
		zhun_dim.lineEnd=dcs.TransToDraw(zhun_dim.lineEnd);
		f3dPoint vec=zhun_dim.lineEnd-zhun_dim.lineStart;
		normalize(vec);
		zhun_dim.lineStart-=2*vec;
		zhun_dim.lineEnd+=2*vec;
		//绘制螺栓心线
		if(fabs(zhun_dim.dist-now_g)>1)
		{
			GetCurDwg()->setClayer(LayerTable::BoltLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,zhun_dim.lineStart,zhun_dim.lineEnd);
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		}
		//标注螺栓心线
		if(bSpecialZhun||
			(fabs(zhun_dim.dist-jgzj.g)>1
			&&fabs(zhun_dim.dist-jgzj.g1)>1
			&&fabs(zhun_dim.dist-jgzj.g2)>1
			&&fabs(zhun_dim.dist-jgzj.g3)>1))	//构件图误差1mm
		{
			if(fabs(zhun_dim.dist-now_g)<1&&bDimG)
				continue;
			sprintf(dimText,"%.0f",zhun_dim.dist);
			f3dPoint vec;
			zhun_dim.dimStartPos=dcs.TransToDraw(zhun_dim.dimStartPos);
			zhun_dim.dimEndPos=dcs.TransToDraw(zhun_dim.dimEndPos);
			zhun_dim.dimPos=dcs.TransToDraw(zhun_dim.dimPos);
			Sub_Pnt(vec,zhun_dim.dimEndPos,zhun_dim.dimStartPos);
			normalize(vec);
			zhun_dim.dimPos -= vec*7.5;//150;
			zhun_dim.dimPos.x -= vec.y*5;//100;
			zhun_dim.dimPos.y += vec.x*5;//100;
			//dimStyle3专门用于标注文字可以在外部进行标注的情况 wht 11-07-12
			DimSize(pBlockTableRecord,zhun_dim.dimStartPos,zhun_dim.dimEndPos,
				zhun_dim.dimPos,dimText,DimStyleTable::dimStyle3.dimStyleId,2,sys.fonts.segment.fDimTextSize);
			if(fabs(zhun_dim.dist-now_g)<1)
				bDimG = TRUE;
		}
	}
}

//只处理绘制X肢或Y肢的情况，其余情况（截面图直接绘制不考虑消隐）
BOOL ProcessJgStru(CLDSLineAngle *pJg, UCS_STRU &ucs)
{
	BOOL bInsideJg;
	f3dPoint work_wing_norm;
	if(pJg==NULL)
		return FALSE;
	int x_wing0_y_wing1;
	normalize(ucs.axis_z);
	CViewPart *pViewPart=(CViewPart*)pJg->feature;
	if(pViewPart&&(pViewPart->cFlag=='X'||pViewPart->cFlag=='S'))
	{
		x_wing0_y_wing1=0;
		if(ucs.axis_z*pJg->get_norm_x_wing()>0)
			bInsideJg=TRUE;		//里铁
		else
			bInsideJg=FALSE;	//外铁
	}
	else if(pViewPart&&(pViewPart->cFlag=='Y'||pViewPart->cFlag=='Q'))
	{
		x_wing0_y_wing1=1;
		if(ucs.axis_z*pJg->get_norm_y_wing()>0)
			bInsideJg=TRUE;		//里铁
		else
			bInsideJg=FALSE;	//外铁
	}
	else //图层视图构件无关联的pViewPart
		bInsideJg=IsInsideJg(pJg,ucs.axis_z,&x_wing0_y_wing1);
	/*
	if(pJg->feature==1||pJg->feature==2)
	{
		x_wing0_y_wing1=pJg->feature-1;
		if(x_wing0_y_wing1==0)
		{
			if(ucs.axis_z*pJg->get_norm_x_wing()>0)
				bInsideJg=TRUE;		//里铁
			else
				bInsideJg=FALSE;	//外铁
		}
		else if(x_wing0_y_wing1==1)
		{
			if(ucs.axis_z*pJg->get_norm_y_wing()>0)
				bInsideJg=TRUE;		//里铁
			else
				bInsideJg=FALSE;	//外铁
		}
	}
	else if((pJg->feature<=0&&pJg->feature>-0x20)||(pJg->feature>2&&pJg->feature<32))
		bInsideJg=IsInsideJg(pJg,ucs.axis_z,&x_wing0_y_wing1);
	else// if(pJg->feature>=0X20||pJg->feature<=-0X20)
		return FALSE;
	*/
	if(bInsideJg)
		INSIDEJGSETDRAW.append(pJg);
	else
		OUTSIDEJGSETDRAW.append(pJg);
	return TRUE;
}
//只处理绘制X肢或Y肢的情况，其余情况（截面图直接绘制不考虑消隐）
PARTSET INSIDEPARAMPLATESET,OUTSIDEPARAMPLATESET;
BOOL ProcessParamPlateStru(CLDSParamPlate *pParamPlate, UCS_STRU &ucs)
{
	if(pParamPlate==NULL)
		return FALSE;
	normalize(ucs.axis_z);

	if(pParamPlate->m_iParamType==TYPE_ROLLEND&&(pParamPlate->ucs.axis_z*ucs.axis_z>EPS_COS2))
		INSIDEPARAMPLATESET.append(pParamPlate);
	else
		OUTSIDEPARAMPLATESET.append(pParamPlate);
	return TRUE;
}

//bOnlyDrawJg 仅绘制角钢轮廓边，不绘制螺栓以及角钢特殊准据 wht 10-11-28
void DrawJgStru(AcDbBlockTableRecord *pBlockTableRecord,CDrawPartInfo& drawPartInfo,CDrawCoordTrans dcs,
				BOOL bSetUpDrawing,BOOL bOnlyDrawJg=FALSE,AcDbObjectId *pOutEntId=NULL)
{
	CLDSLineAngle *pJg=(CLDSLineAngle*)drawPartInfo.pPart;
	BOOL bInsideJg;
	AcDbObjectId outPutEntId=0;	//关联构件句柄的实体ID wht 10-12-02
	double wing_wide,wing_thick;
	JGZJ jgzj;
	f3dPoint work_wing_norm;
	if(pJg==NULL)
		return;
	int x_wing0_y_wing1;
	AcDbObjectId layerId;
	//|￣￣￣￣￣￣￣￣￣￣|　first line: start_firs,end_firs
	//|_ _ _ _ _ _ _ _ _ _ |  second line:start_sec,end_sec
	//|____________________|  third line: start_thir,end_thir
	//axis_start ___ _ ____ _ ____axis_end
	f3dPoint point,start_firs,end_firs,start_sec,end_sec,
			start_thir,end_thir,axis_start,axis_end,wing_vec,len_vec,
			start_firs_derive,end_firs_derive,start_thir_derive,end_thir_derive;//由于切角切肢派生出的新顶点 wht 10-05-13
	f3dPoint startPt,centerPt; 
	f3dPoint endPt,centerPtEnd; //压扁所需位置点 xc 11-01-13
	//normalize(ucs.axis_z);	//不知改为CDrawCoordTrans后是否有用，暂时隐掉　wjh 2011.8.23
	getjgzj(jgzj,pJg->GetWidth());
	wing_wide = pJg->GetWidth();
	wing_thick= pJg->GetThick();
	double wing_wide_offset = 0;
	if(sys.stru_map.fMinWingWide>wing_wide)
	{
		wing_wide_offset = 0.5*(sys.stru_map.fMinWingWide-wing_wide)/dcs.m_fDrawScale;
#ifdef __SMART_DRAW_INC_
		CDrawingCommand *pCmd=(CDrawingCommand*)drawPartInfo.pDrawingCmd;
		if(pCmd!=NULL)
		{
			if(pCmd->m_paraPart.para.angle.extendStyle==1)	//向肢翼侧拓宽
				wing_wide_offset = 0;
			else if(pCmd->m_paraPart.para.angle.extendStyle==2)	//向楞线侧拓宽
				wing_wide_offset = (sys.stru_map.fMinWingWide-wing_wide)/dcs.m_fDrawScale;
			//else if(pCmd->m_paraPart.para.angle.extendStyle==0)	//向两侧拓宽保留原值
			//	wing_wide_offset = 0.5*(sys.stru_map.fMinWingWide-wing_wide)/dcs.m_fDrawScale;
		}
#endif
	}
	wing_wide  = __max(wing_wide,sys.stru_map.fMinWingWide);	//角钢肢宽夸张画法
	wing_thick = __max(wing_thick,sys.stru_map.fMinPartThick);	//角钢肢厚夸张画法

	pJg->getUCS(pJg->ucs);
	f3dPoint cur_jg_vec=pJg->End()-pJg->Start();
	normalize(cur_jg_vec);
	if(drawPartInfo.pViewPart&&(drawPartInfo.pViewPart->cFlag=='X'||drawPartInfo.pViewPart->cFlag=='S'))
		x_wing0_y_wing1=0;	//X肢为当前绘制肢
	else if(drawPartInfo.pViewPart&&(drawPartInfo.pViewPart->cFlag=='Y'||drawPartInfo.pViewPart->cFlag=='Q'))
		x_wing0_y_wing1=1;	//Y肢为当前绘制肢
	else if((drawPartInfo.pViewPart&&drawPartInfo.pViewPart->cFlag=='Z')||fabs(cur_jg_vec*dcs.PrjNorm())>0.70710)
		x_wing0_y_wing1=2;
	else
		bInsideJg=IsInsideJg(pJg,dcs.PrjNorm(),&x_wing0_y_wing1);
	if(pJg->m_bFootNail)
	{
		f3dPoint endPt=pJg->End();
		//project_point(endPt,pJg->Start(),ucs.axis_y); //不知为何要多此一举画蛇添足　wjh-2011.8.23
		f3dPoint cur_jg_vec=endPt-pJg->Start();
		normalize(cur_jg_vec);
		if(fabs(cur_jg_vec*dcs.PrjNorm())>0.70710)
			x_wing0_y_wing1=2;	//角钢脚钉截面
		else
			x_wing0_y_wing1=3;	//角钢脚钉正面
	}
	if(x_wing0_y_wing1==0)
	{
		if(dcs.PrjNorm()*pJg->get_norm_x_wing()>0)
			bInsideJg=TRUE;		//里铁
		else
			bInsideJg=FALSE;	//外铁
		if(pJg->m_bEnableTeG)
			jgzj = pJg->xWingXZhunJu;
		wing_vec = pJg->GetWingVecX();
		work_wing_norm = pJg->get_norm_x_wing();
	}
	else if(x_wing0_y_wing1==1)
	{
		if(dcs.PrjNorm()*pJg->get_norm_y_wing()>0)
			bInsideJg=TRUE;		//里铁
		else
			bInsideJg=FALSE;	//外铁
		if(pJg->m_bEnableTeG)
			jgzj = pJg->xWingYZhunJu;
		wing_vec = pJg->GetWingVecY();
		work_wing_norm = pJg->get_norm_y_wing();
	}
	else if(x_wing0_y_wing1==2)
	{	//绘制截面图
		if(!pJg->m_bEnableTeG)
		{
			pJg->xWingYZhunJu = jgzj;
			pJg->xWingXZhunJu = jgzj;
		}
		//线面求交时的基准点(隔面主材角钢截面图) wht 11-03-02
		f3dPoint face_pick;
		//if(pViewPart&&pViewPart->cFlag=='Z'&&pViewPart->pDisplayView)
		face_pick=dcs.PrjOrigin();//ucs.origin;//pViewPart->pDisplayView->ucs.origin;	
		//else 
		//	face_pick=pJg->ucs.origin;
		f3dPoint line_vec,x_out_vertex,x_in_vertex,y_out_vertex,y_in_vertex,ber_out_vertex,ber_in_vertex;
		//截取角钢截面图
		Int3dlf(ber_out_vertex,f3dLine(pJg->Start(),pJg->End()),face_pick,dcs.PrjNorm());
		x_out_vertex=ber_out_vertex+pJg->GetWingVecX()*wing_wide;
		y_out_vertex=ber_out_vertex+pJg->GetWingVecY()*wing_wide;
		x_in_vertex=x_out_vertex-pJg->get_norm_x_wing()*wing_thick;
		y_in_vertex=y_out_vertex-pJg->get_norm_y_wing()*wing_thick;
		line_vec=pJg->End()-pJg->Start();
		normalize(line_vec);
		Int3dlf(x_out_vertex,x_out_vertex,line_vec,face_pick,dcs.PrjNorm());
		Int3dlf(y_out_vertex,y_out_vertex,line_vec,face_pick,dcs.PrjNorm());
		Int3dlf(x_in_vertex,x_in_vertex,line_vec,face_pick,dcs.PrjNorm());
		Int3dlf(y_in_vertex,y_in_vertex,line_vec,face_pick,dcs.PrjNorm());
		//转换到绘图坐标系下
		ber_out_vertex=dcs.TransToDraw(ber_out_vertex);
		x_out_vertex=dcs.TransToDraw(x_out_vertex);
		y_out_vertex=dcs.TransToDraw(y_out_vertex);
		x_in_vertex=dcs.TransToDraw(x_in_vertex);
		y_in_vertex=dcs.TransToDraw(y_in_vertex);
		//空间直线求交确定角钢内楞点
		f3dLine x_in_line,y_in_line,x_axis_line,y_axis_line;
		x_in_line.startPt=x_in_vertex;
		y_in_line.startPt=y_in_vertex;
		x_in_line.endPt=x_in_vertex-x_out_vertex+ber_out_vertex;
		y_in_line.endPt=y_in_vertex-y_out_vertex+ber_out_vertex;
		Int3dll(x_in_line,y_in_line,x_in_line.endPt);
		y_in_line.endPt=x_in_line.endPt;
		//内侧线始终端均回缩角钢肢厚便于生成圆弧 wht 11-07-09
		f3dPoint x_line_vec=x_in_line.endPt-x_in_line.startPt;
		normalize(x_line_vec);
		x_in_line.startPt+=x_line_vec*wing_thick*(1.0/dcs.m_fDrawScale);
		x_in_line.endPt-=x_line_vec*wing_thick*(1.0/dcs.m_fDrawScale);
		f3dPoint y_line_vec=y_in_line.endPt-y_in_line.startPt;
		normalize(y_line_vec);
		y_in_line.startPt+=y_line_vec*wing_thick*(1.0/dcs.m_fDrawScale);
		y_in_line.endPt-=y_line_vec*wing_thick*(1.0/dcs.m_fDrawScale);
		//
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(pJg->m_bFootNail&&pJg->ucs.axis_z*dcs.PrjNorm()<0)
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);	//角钢脚钉
		
		if(!sys.stru_map.bUsePolyLineDrawEdge)
		{
			f3dPoint x_center=x_out_vertex+x_line_vec*wing_thick*(1.0/dcs.m_fDrawScale);		//X肢圆弧圆心
			f3dPoint y_center=y_out_vertex+y_line_vec*wing_thick*(1.0/dcs.m_fDrawScale);		//Y肢圆弧圆心
			f3dPoint ber_center=y_in_line.endPt-x_line_vec*wing_thick*(1.0/dcs.m_fDrawScale);	//内楞圆弧圆心
			outPutEntId=CreateAcadLine(pBlockTableRecord,ber_out_vertex,x_out_vertex,pJg->handle);
			CreateAcadLine(pBlockTableRecord,ber_out_vertex,y_out_vertex);
			//CreateAcadLine(pBlockTableRecord,x_in_vertex,x_out_vertex);
			//CreateAcadLine(pBlockTableRecord,y_in_vertex,y_out_vertex);
			CreateAcadLine(pBlockTableRecord,x_in_line.startPt,x_in_line.endPt);
			CreateAcadLine(pBlockTableRecord,y_in_line.startPt,y_in_line.endPt);
			//生成角钢内圆弧及轧制边圆弧 wht 11-07-09
			if(pJg->ucs.axis_z*dcs.PrjNorm()<0)
			{
				CreateAcadArcLine(pBlockTableRecord,ber_center,x_in_line.endPt,y_in_line.endPt);
				CreateAcadArcLine(pBlockTableRecord,x_center,x_in_line.startPt,x_out_vertex);
				CreateAcadArcLine(pBlockTableRecord,y_center,y_out_vertex,y_in_line.startPt);
			}
			else 
			{
				CreateAcadArcLine(pBlockTableRecord,ber_center,y_in_line.endPt,x_in_line.endPt);
				CreateAcadArcLine(pBlockTableRecord,x_center,x_out_vertex,x_in_line.startPt);
				CreateAcadArcLine(pBlockTableRecord,y_center,y_in_line.startPt,y_out_vertex);
			}
		}
		else
		{
			outPutEntId=CreateAcadPolyLine(pBlockTableRecord,ber_out_vertex,x_out_vertex,sys.stru_map.fPolyLineWidth,pJg->handle);
			CreateAcadPolyLine(pBlockTableRecord,ber_out_vertex,y_out_vertex,sys.stru_map.fPolyLineWidth);
			//CreateAcadPolyLine(pBlockTableRecord,x_in_vertex,x_out_vertex,sys.stru_map.fPolyLineWidth);
			//CreateAcadPolyLine(pBlockTableRecord,y_in_vertex,y_out_vertex,sys.stru_map.fPolyLineWidth);
			CreateAcadPolyLine(pBlockTableRecord,x_in_line.startPt,x_in_line.endPt,sys.stru_map.fPolyLineWidth);
			CreateAcadPolyLine(pBlockTableRecord,y_in_line.startPt,y_in_line.endPt,sys.stru_map.fPolyLineWidth);
			//生成角钢内圆弧及轧制边圆弧 wht 11-07-09
			double sector_angle=90*RADTODEG_COEF;
			if(pJg->ucs.axis_z*dcs.PrjNorm()>0)//ucs.axis_z>0)
				sector_angle*=-1;
			CreateAcadPolyArcLine(pBlockTableRecord,x_in_line.endPt,y_in_line.endPt,sector_angle,sys.stru_map.fPolyLineWidth);
			CreateAcadPolyArcLine(pBlockTableRecord,x_in_line.startPt,x_out_vertex,sector_angle,sys.stru_map.fPolyLineWidth);
			CreateAcadPolyArcLine(pBlockTableRecord,y_out_vertex,y_in_line.startPt,sector_angle,sys.stru_map.fPolyLineWidth);
		}
		x_axis_line.startPt=ber_out_vertex+(x_out_vertex-ber_out_vertex)*pJg->xWingXZhunJu.g/wing_wide;
		y_axis_line.startPt=ber_out_vertex+(y_out_vertex-ber_out_vertex)*pJg->xWingYZhunJu.g/wing_wide;
		x_axis_line.endPt=x_axis_line.startPt-(x_out_vertex-x_in_vertex)*2;
		y_axis_line.endPt=y_axis_line.startPt-(y_out_vertex-y_in_vertex)*2;
		x_axis_line.startPt=x_axis_line.startPt+(x_out_vertex-x_in_vertex)*2;
		y_axis_line.startPt=y_axis_line.startPt+(y_out_vertex-y_in_vertex)*2;
		//截面图不需要绘制角钢心线 wht 11-07-11
		/*if(!pJg->m_bFootNail)
		{	//角钢脚钉不绘制心线
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			outPutEntId=CreateAcadLine(pBlockTableRecord,x_axis_line.startPt,x_axis_line.endPt,pJg->handle);
			CreateAcadLine(pBlockTableRecord,y_axis_line.startPt,y_axis_line.endPt);
		}*/
		if(pOutEntId)
			*pOutEntId=outPutEntId;
		return;
	}
	else if(x_wing0_y_wing1==3)
	{	//绘制正面图
		int cur_wing_x0_y1=0;
		bInsideJg=IsInsideJg(pJg,dcs.PrjNorm(),&cur_wing_x0_y1);
		if(cur_wing_x0_y1==0)
		{
			if(dcs.PrjNorm()*pJg->get_norm_x_wing()>0)
				bInsideJg=TRUE;		//里铁
			else
				bInsideJg=FALSE;	//外铁
			if(pJg->m_bEnableTeG)
				jgzj = pJg->xWingXZhunJu;
			wing_vec = pJg->GetWingVecX();
			work_wing_norm = pJg->get_norm_x_wing();
		}
		else //if(cur_wing_x0_y1==1)
		{
			if(dcs.PrjNorm()*pJg->get_norm_y_wing()>0)
				bInsideJg=TRUE;		//里铁
			else
				bInsideJg=FALSE;	//外铁
			if(pJg->m_bEnableTeG)
				jgzj = pJg->xWingYZhunJu;
			wing_vec = pJg->GetWingVecY();
			work_wing_norm = pJg->get_norm_y_wing();
		}
		start_thir = pJg->Start();
		end_thir   = pJg->End();
		Sub_Pnt(len_vec,end_thir,start_thir);
		normalize(len_vec);//角钢长度方向(自始端指向末端)
		Add_Pnt(start_thir,pJg->Start(),-1.0*len_vec*pJg->startOdd());
		Add_Pnt(end_thir,pJg->End(),len_vec*pJg->endOdd());
		
		Add_Pnt(start_firs,start_thir,wing_vec*wing_wide);
		Add_Pnt(end_firs,end_thir,wing_vec*wing_wide);
		Add_Pnt(start_sec,start_thir,wing_vec*wing_thick);
		Add_Pnt(end_sec,end_thir,wing_vec*wing_thick);
		
		Add_Pnt(axis_start,pJg->Start(),wing_vec*jgzj.g);
		Add_Pnt(axis_end,pJg->End(),wing_vec*jgzj.g);
		
		//转换到绘图坐标系下
		start_firs=dcs.TransToDraw(start_firs);
		end_firs=dcs.TransToDraw(end_firs);
		start_sec=dcs.TransToDraw(start_sec);
		end_sec=dcs.TransToDraw(end_sec);
		start_thir=dcs.TransToDraw(start_thir);
		end_thir=dcs.TransToDraw(end_thir);
		axis_start=dcs.TransToDraw(axis_start);
		axis_end=dcs.TransToDraw(axis_end);
		wing_vec=dcs.TransVToDraw(wing_vec);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,start_firs-wing_wide_offset*wing_vec,end_firs-wing_wide_offset*wing_vec);
		CreateAcadLine(pBlockTableRecord,start_thir-wing_wide_offset*wing_vec,end_thir-wing_wide_offset*wing_vec);
		CreateAcadLine(pBlockTableRecord,start_firs-wing_wide_offset*wing_vec,start_thir-wing_wide_offset*wing_vec);
		CreateAcadLine(pBlockTableRecord,end_firs-wing_wide_offset*wing_vec,end_thir-wing_wide_offset*wing_vec);
		if(bInsideJg)	//里铁
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		else
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		outPutEntId=CreateAcadLine(pBlockTableRecord,start_sec-wing_wide_offset*wing_vec,end_sec-wing_wide_offset*wing_vec,pJg->handle);
		if(pOutEntId)
			*pOutEntId=outPutEntId;
		return;
	}

	start_thir = pJg->Start();
	end_thir   = pJg->End();
	Sub_Pnt(len_vec,end_thir,start_thir);
	normalize(len_vec);//角钢长度方向(自始端指向末端)
	Add_Pnt(start_thir,pJg->Start(),-1.0*len_vec*pJg->startOdd());
	Add_Pnt(end_thir,pJg->End(),len_vec*pJg->endOdd());
	Add_Pnt(start_firs,start_thir,wing_vec*wing_wide);
	Add_Pnt(end_firs,end_thir,wing_vec*wing_wide);
	Add_Pnt(start_sec,start_thir,wing_vec*wing_thick);
	Add_Pnt(end_sec,end_thir,wing_vec*wing_thick);
	start_firs_derive=start_firs;
	end_firs_derive=end_firs;
	start_thir_derive=start_thir;
	end_thir_derive=end_thir;
	if(pJg->start_push_flat_length>0)
	{
		start_sec+=len_vec*(pJg->start_push_flat_length+wing_wide-wing_thick);
		startPt = start_sec;
		centerPt=start_firs+(pJg->start_push_flat_length+wing_wide-wing_thick)*len_vec;
	}
	if(pJg->end_push_flat_length>0)
	{
		end_sec-=len_vec*(pJg->end_push_flat_length+wing_wide-wing_thick);
		endPt=end_sec;
		centerPtEnd=end_firs-(pJg->end_push_flat_length+wing_wide-wing_thick)*len_vec;
	}
	if(sys.stru_map.bDrawCutAngleAndWingInfo)
	{	//显示切肢/切角效果 wht 10-05-13
		if(x_wing0_y_wing1==0)	//X肢 
		{	//始端
			if(pJg->cut_wing[0]==1)	//始端X肢切肢
			{	
				Add_Pnt(start_thir_derive,start_thir_derive,len_vec*pJg->cut_wing_para[0][0]);
				Add_Pnt(start_firs,start_firs,len_vec*pJg->cut_wing_para[0][2]);
				start_firs_derive=start_firs;
			}
			else if(pJg->cut_angle[0][0]>0)	//始端X肢切角
			{
				Add_Pnt(start_firs,start_firs,len_vec*pJg->cut_angle[0][0]);
				Add_Pnt(start_firs_derive,start_firs_derive,-1.0*wing_vec*pJg->cut_angle[0][1]);
				if(pJg->cut_wing[0]==2)//始端Y肢切肢
				{
					double sec_oddment=0;
					if(pJg->cut_wing_para[0][1]>0)
						sec_oddment=(pJg->cut_wing_para[0][0]*(pJg->cut_wing_para[0][1]-wing_thick))/pJg->cut_wing_para[0][1];
					Add_Pnt(start_thir,start_thir,len_vec*pJg->cut_wing_para[0][0]);
					Add_Pnt(start_sec,start_sec,len_vec*sec_oddment);
					Add_Pnt(start_thir_derive,start_thir_derive,wing_vec*pJg->cut_wing_para[0][1]);
				}
			}
			//终端
			if(pJg->cut_wing[1]==1)	//终端X肢切肢
			{	
				Add_Pnt(end_thir_derive,end_thir_derive,len_vec*pJg->cut_wing_para[1][0]);
				Add_Pnt(end_firs,end_firs,-1.0*len_vec*pJg->cut_wing_para[1][2]);
				end_firs_derive=end_firs;
			}
			else if(pJg->cut_angle[2][0]>0)	//终端X肢切角
			{
				Add_Pnt(end_firs,end_firs,-1.0*len_vec*pJg->cut_angle[2][0]);
				Add_Pnt(end_firs_derive,end_firs_derive,-1.0*wing_vec*pJg->cut_angle[2][1]);
				if(pJg->cut_wing[1]==2)//终端Y肢切肢
				{
					double sec_oddment=0;
					if(pJg->cut_wing_para[1][1]>0)
						sec_oddment=(pJg->cut_wing_para[1][0]*(pJg->cut_wing_para[1][1]-wing_thick))/pJg->cut_wing_para[1][1];
					Add_Pnt(end_thir,end_thir,-1.0*len_vec*pJg->cut_wing_para[1][0]);
					Add_Pnt(end_sec,end_sec,-1.0*len_vec*sec_oddment);
					Add_Pnt(end_thir_derive,end_thir_derive,wing_vec*pJg->cut_wing_para[1][1]);
				}
			}
		}
		else	//Y肢
		{	//始端
			if(pJg->cut_wing[0]==2)	//始端Y肢切肢
			{	
				Add_Pnt(start_thir_derive,start_thir_derive,len_vec*pJg->cut_wing_para[0][0]);
				Add_Pnt(start_firs,start_firs,len_vec*pJg->cut_wing_para[0][2]);
				start_firs_derive=start_firs;
			}
			else if(pJg->cut_angle[1][0]>0)	//始端Y肢切角
			{
				Add_Pnt(start_firs,start_firs,len_vec*pJg->cut_angle[1][0]);
				Add_Pnt(start_firs_derive,start_firs_derive,-1.0*wing_vec*pJg->cut_angle[1][1]);
				if(pJg->cut_wing[0]==1)//始端X肢切肢
				{
					double sec_oddment=0;
					if(pJg->cut_wing_para[0][1]>0)
						sec_oddment=(pJg->cut_wing_para[0][0]*(pJg->cut_wing_para[0][1]-wing_thick))/pJg->cut_wing_para[0][1];
					Add_Pnt(start_thir,start_thir,len_vec*pJg->cut_wing_para[0][0]);
					Add_Pnt(start_sec,start_sec,len_vec*sec_oddment);
					Add_Pnt(start_thir_derive,start_thir_derive,wing_vec*pJg->cut_wing_para[0][1]);
				}
			}
			//终端
			if(pJg->cut_wing[1]==2)	//终端Y肢切肢
			{	
				Add_Pnt(end_thir_derive,end_thir_derive,len_vec*pJg->cut_wing_para[1][0]);
				Add_Pnt(end_firs,end_firs,-1.0*len_vec*pJg->cut_wing_para[1][2]);
				end_firs_derive=end_firs;
			}
			else if(pJg->cut_angle[3][0]>0)	//终端Y肢切角
			{
				Add_Pnt(end_firs,end_firs,-1.0*len_vec*pJg->cut_angle[3][0]);
				Add_Pnt(end_firs_derive,end_firs_derive,-1.0*wing_vec*pJg->cut_angle[3][1]);
				if(pJg->cut_wing[1]==1)//终端X肢切肢
				{
					double sec_oddment=0;
					if(pJg->cut_wing_para[1][1]>0)
						sec_oddment=(pJg->cut_wing_para[1][0]*(pJg->cut_wing_para[1][1]-wing_thick))/pJg->cut_wing_para[1][1];
					Add_Pnt(end_thir,end_thir,-1.0*len_vec*pJg->cut_wing_para[1][0]);
					Add_Pnt(end_sec,end_sec,-1.0*len_vec*sec_oddment);
					Add_Pnt(end_thir_derive,end_thir_derive,wing_vec*pJg->cut_wing_para[1][1]);
				}
			}
		}
	}
	Add_Pnt(axis_start,pJg->Start(),wing_vec*jgzj.g);
	Add_Pnt(axis_end,pJg->End(),wing_vec*jgzj.g);
	//axis_start=start_thir-len_vec*pJg->startOdd()+wing_vec*jgzj.g;
	//axis_end=end_thir+len_vec*pJg->endOdd()+wing_vec*jgzj.g;
	if(pJg->startOdd()>0)//准线两端都伸出30mm
		Sub_Pnt(axis_start,axis_start,len_vec*(sys.stru_map.fPoleAxisLineStartOutSpreadLen+pJg->startOdd()));
	else
		Sub_Pnt(axis_start,axis_start,len_vec*sys.stru_map.fPoleAxisLineStartOutSpreadLen);//准线两端都伸出30mm
	if(pJg->endOdd()>0)//准线两端都伸出30mm
		Add_Pnt(axis_end  ,axis_end  ,len_vec*(sys.stru_map.fPoleAxisLineEndOutSpreadLen+pJg->endOdd()));
	else
		Add_Pnt(axis_end  ,axis_end  ,len_vec*sys.stru_map.fPoleAxisLineEndOutSpreadLen);//准线两端都伸出30mm
	//旋转角钢轮廓点到指定平面 wht 10-12-10
	/*f3dLine spreadLine;
	double rotate_angle=0;
	if(pViewPart)
	{	//根据指定的展开面计算旋转轴及旋转角 wht 11-08-09
		//CDrawingAssistant *pAssist=pViewPart->GetRotateAxisAndAngle(&Ta,ucs.axis_z,spreadLine,rotate_angle);
		if(dcs.m_fSpreadAngle==0&&(pViewPart->cFlag=='S'||pViewPart->cFlag=='Q'))
		{	
			rotate_angle = cal_angle_of_2vec(ucs.axis_z,work_wing_norm);
			f3dPoint rotate_axis=cross_prod(ucs.axis_z,work_wing_norm);
			if(dot_prod(len_vec,rotate_axis)>0)
			{	//杆件延伸方向与旋转轴反向
				spreadLine.startPt=pJg->End()+wing_vec*pViewPart->nSpreadLineOffsetDist;
				spreadLine.endPt=pJg->Start()+wing_vec*pViewPart->nSpreadLineOffsetDist;
			}
			else
			{	//杆件延伸方向与旋转轴同向
				spreadLine.startPt=pJg->Start()+wing_vec*pViewPart->nSpreadLineOffsetDist;
				spreadLine.endPt=pJg->End()+wing_vec*pViewPart->nSpreadLineOffsetDist;
			}
		}
		if(fabs(rotate_angle)>0&&spreadLine.startPt!=spreadLine.endPt)
		{
			rotate_point_around_axis(start_firs,rotate_angle,spreadLine);
			rotate_point_around_axis(end_firs,rotate_angle,spreadLine);
			rotate_point_around_axis(start_firs_derive,rotate_angle,spreadLine);
			rotate_point_around_axis(end_firs_derive,rotate_angle,spreadLine);
			rotate_point_around_axis(start_sec,rotate_angle,spreadLine);
			rotate_point_around_axis(end_sec,rotate_angle,spreadLine);
			rotate_point_around_axis(start_thir,rotate_angle,spreadLine);
			rotate_point_around_axis(end_thir,rotate_angle,spreadLine);
			rotate_point_around_axis(start_thir_derive,rotate_angle,spreadLine);
			rotate_point_around_axis(end_thir_derive,rotate_angle,spreadLine);
			rotate_point_around_axis(axis_start,rotate_angle,spreadLine);
			rotate_point_around_axis(axis_end,rotate_angle,spreadLine);
			rotate_point_around_axis(startPt,rotate_angle,spreadLine);
			rotate_point_around_axis(centerPt,rotate_angle,spreadLine);
			rotate_point_around_axis(endPt,rotate_angle,spreadLine);
			rotate_point_around_axis(centerPtEnd,rotate_angle,spreadLine);
		}
	}*/
	//转换到绘图坐标系下
	start_firs=dcs.TransToDraw(start_firs);
	end_firs=dcs.TransToDraw(end_firs);
	start_firs_derive=dcs.TransToDraw(start_firs_derive);
	end_firs_derive=dcs.TransToDraw(end_firs_derive);
	start_sec=dcs.TransToDraw(start_sec);
	end_sec=dcs.TransToDraw(end_sec);
	start_thir=dcs.TransToDraw(start_thir);
	end_thir=dcs.TransToDraw(end_thir);
	start_thir_derive=dcs.TransToDraw(start_thir_derive);
	end_thir_derive=dcs.TransToDraw(end_thir_derive);
	axis_start=dcs.TransToDraw(axis_start);
	axis_end=dcs.TransToDraw(axis_end);
	startPt=dcs.TransToDraw(startPt);
	centerPt=dcs.TransToDraw(centerPt);
	endPt=dcs.TransToDraw(endPt);
	centerPtEnd=dcs.TransToDraw(centerPtEnd);
	wing_vec=dcs.TransVToDraw(wing_vec);
	f3dPoint vec = start_firs-end_firs;
	normalize(vec);
	f3dPoint worknorm = vec^wing_vec;
	//处理消隐
	fPtList vertexlist;
	vertexlist.append(start_thir-wing_wide_offset*wing_vec);
	vertexlist.append(end_thir-wing_wide_offset*wing_vec);
	vertexlist.append(end_firs-wing_wide_offset*wing_vec);
	vertexlist.append(start_firs-wing_wide_offset*wing_vec);
	ProcessHideLine(pBlockTableRecord,vertexlist,LayerTable::UnvisibleProfileLayer.layerName);
	if(bInsideJg)	//里铁
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
	else
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(pJg->start_push_flat_x1_y2>0)     //画压扁线
	{	//最小绘制肢宽大于当前角钢肢宽时为保证心线位置不变,需要偏移wing_wide_offset wht 11-07-19
		startPt=startPt-wing_vec*wing_wide_offset;	
		centerPt=centerPt-wing_vec*wing_wide_offset;
		CreateAcadArcLine(pBlockTableRecord,centerPt,startPt,Pi/2,worknorm);
	}
	if(pJg->end_push_flat_x1_y2>0)
	{
		endPt=endPt-wing_vec*wing_wide_offset;	
		centerPtEnd=centerPtEnd-wing_vec*wing_wide_offset;
		CreateAcadArcLine(pBlockTableRecord,centerPtEnd,endPt,Pi/2,-worknorm);
	}
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	
	if(!sys.stru_map.bUsePolyLineDrawEdge)
	{
		CreateAcadLine(pBlockTableRecord,start_firs-wing_vec*wing_wide_offset,end_firs-wing_vec*wing_wide_offset);
		CreateAcadLine(pBlockTableRecord,start_thir-wing_vec*wing_wide_offset,end_thir-wing_vec*wing_wide_offset);
		if(start_firs!=start_firs_derive)
			CreateAcadLine(pBlockTableRecord,start_firs-wing_vec*wing_wide_offset,start_firs_derive-wing_vec*wing_wide_offset);
		CreateAcadLine(pBlockTableRecord,start_firs_derive-wing_vec*wing_wide_offset,start_thir_derive-wing_vec*wing_wide_offset);
		if(start_thir!=start_thir_derive)
			CreateAcadLine(pBlockTableRecord,start_thir-wing_vec*wing_wide_offset,start_thir_derive-wing_vec*wing_wide_offset);
		if(sys.stru_map.bDrawCutAngleAndWingInfo)
		{	//当前肢切肢时绘制厚度线与楞线的边界线
			if((x_wing0_y_wing1==0&&pJg->cut_wing[0]==1)||(x_wing0_y_wing1==1&&pJg->cut_wing[0]==2))
				CreateAcadLine(pBlockTableRecord,start_sec-wing_vec*wing_wide_offset,start_thir-wing_vec*wing_wide_offset);
		}
		//
		if(end_firs!=end_firs_derive)
			CreateAcadLine(pBlockTableRecord,end_firs-wing_vec*wing_wide_offset,end_firs_derive-wing_vec*wing_wide_offset);
		CreateAcadLine(pBlockTableRecord,end_firs_derive-wing_vec*wing_wide_offset,end_thir_derive-wing_vec*wing_wide_offset);
		if(end_thir!=end_thir_derive)
			CreateAcadLine(pBlockTableRecord,end_thir-wing_vec*wing_wide_offset,end_thir_derive-wing_vec*wing_wide_offset);
		if(sys.stru_map.bDrawCutAngleAndWingInfo)
		{	//当前肢切肢时绘制厚度线与楞线的边界线
			if((x_wing0_y_wing1==0&&pJg->cut_wing[1]==1)||(x_wing0_y_wing1==1&&pJg->cut_wing[1]==2))
				CreateAcadLine(pBlockTableRecord,end_sec-wing_vec*wing_wide_offset,end_thir-wing_vec*wing_wide_offset);
		}
	}
	else
	{
		CreateAcadPolyLine(pBlockTableRecord,start_firs-wing_vec*wing_wide_offset,
						   end_firs-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		CreateAcadPolyLine(pBlockTableRecord,start_thir-wing_vec*wing_wide_offset,
						   end_thir-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		if(start_firs!=start_firs_derive)
			CreateAcadPolyLine(pBlockTableRecord,start_firs-wing_vec*wing_wide_offset,
							   start_firs_derive-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		CreateAcadPolyLine(pBlockTableRecord,start_firs_derive-wing_vec*wing_wide_offset,
						   start_thir_derive-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		if(start_thir!=start_thir_derive)
			CreateAcadPolyLine(pBlockTableRecord,start_thir-wing_vec*wing_wide_offset,
							   start_thir_derive-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		if(sys.stru_map.bDrawCutAngleAndWingInfo)
		{	//当前肢切肢时绘制厚度线与楞线的边界线
			if((x_wing0_y_wing1==0&&pJg->cut_wing[0]==1)||(x_wing0_y_wing1==1&&pJg->cut_wing[0]==2))
				CreateAcadPolyLine(pBlockTableRecord,start_sec-wing_vec*wing_wide_offset,
								   start_thir-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		}
		//
		if(end_firs!=end_firs_derive)
			CreateAcadPolyLine(pBlockTableRecord,end_firs-wing_vec*wing_wide_offset,
							   end_firs_derive-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		CreateAcadPolyLine(pBlockTableRecord,end_firs_derive-wing_vec*wing_wide_offset,
						   end_thir_derive-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		if(end_thir!=end_thir_derive)
			CreateAcadPolyLine(pBlockTableRecord,end_thir-wing_vec*wing_wide_offset,
							   end_thir_derive-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		if(sys.stru_map.bDrawCutAngleAndWingInfo)
		{	//当前肢切肢时绘制厚度线与楞线的边界线
			if((x_wing0_y_wing1==0&&pJg->cut_wing[1]==1)||(x_wing0_y_wing1==1&&pJg->cut_wing[1]==2))
				CreateAcadPolyLine(pBlockTableRecord,end_sec-wing_vec*wing_wide_offset,
								   end_thir-wing_vec*wing_wide_offset,sys.stru_map.fPolyLineWidth);
		}
	}
	//
	if(bInsideJg)	//里铁
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
	else
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	//设定角钢绘制方式字符
	int drawStyle=0;
	if(x_wing0_y_wing1==0&&bInsideJg)
		drawStyle='X';
	else if(x_wing0_y_wing1==0&&!bInsideJg)
		drawStyle='x';
	else if(x_wing0_y_wing1==1&&bInsideJg)
		drawStyle='Y';
	else if(x_wing0_y_wing1==1&&!bInsideJg)
		drawStyle='y';
	else
		drawStyle='Z';
	BOOL bDrawAxisLine=TRUE;
	if((pJg->pStart==NULL||pJg->pEnd==NULL)&&!pJg->IsShadowObject())	//包铁的特征线选择为角钢内楞
		bDrawAxisLine=FALSE;
#ifdef __SMART_DRAW_INC_
	else if(drawPartInfo.pDrawingCmd!=NULL&&!((CDrawingCommand*)drawPartInfo.pDrawingCmd)->m_paraPart.para.angle.bDrawAxisLine)
	{	//智能绘图时指定不绘心线
		bDrawAxisLine=FALSE;
	}
#endif
	if(!bDrawAxisLine)	//不绘制心线时，基准线为竖直肢内侧投影线
		outPutEntId=CreateAcadLine(pBlockTableRecord,start_sec-wing_wide_offset*wing_vec,end_sec-wing_wide_offset*wing_vec,pJg->handle,drawStyle);
	else
		CreateAcadLine(pBlockTableRecord,start_sec-wing_wide_offset*wing_vec,end_sec-wing_wide_offset*wing_vec);
	CLDSGroupLineAngle *pGroupJg=NULL;
	if(pJg->group_father_jg_h>0x20)
		pGroupJg=(CLDSGroupLineAngle*)Ta.FromPartHandle(pJg->group_father_jg_h);
	if(pGroupJg)
	{
		if(pGroupJg->group_style==0||pGroupJg->group_style==2)	//对角组合或十字组合
			GetCurDwg()->setClayer(LayerTable::SonJgAxisLineLayer.layerId);	//子角钢中心线图层 wht 11-07-21
		else if((pJg->handle==pGroupJg->son_jg_h[0]&&toupper(drawStyle)=='X')||
			(pJg->handle==pGroupJg->son_jg_h[1]&&toupper(drawStyle)=='Y'))
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);	//T型组合角钢侧面图中子角钢心线就是主心线，在标相似形时应显示
		else
			GetCurDwg()->setClayer(LayerTable::SonJgAxisLineLayer.layerId);	//子角钢中心线图层 wht 11-07-21
	}
	else 
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
	if(bDrawAxisLine)
	{
		outPutEntId=CreateAcadLine(pBlockTableRecord,axis_start,axis_end,pJg->handle,drawStyle);
		if(DRAGSET.m_pDrawCmdTagInfo&&DRAGSET.m_pDrawCmdTagInfo->IsDrawAnglePart())
		{
			DRAGSET.m_pDrawCmdTagInfo->AngleDrawTagInfo()->m_xStartDrawPos=axis_start;
			DRAGSET.m_pDrawCmdTagInfo->AngleDrawTagInfo()->m_xEndDrawPos=axis_end;
			DRAGSET.m_pDrawCmdTagInfo->AngleDrawTagInfo()->m_cDrawStyle=drawStyle;
		}
	}
	if(pOutEntId)	//关联构件句柄的CAD实体ID wht 10-12-02
		*pOutEntId=outPutEntId;
	if(!bOnlyDrawJg)
	{	//画角钢上的螺栓组
		CDimJgZhun dim_jg_zhun(pJg);	//分析计算角钢上的螺栓准距线
		DrawLsRefList(pJg->GetLsRefList(),dcs,FALSE,&work_wing_norm);
		if(sys.stru_map.bDimSpecialJgZhun)
		{
			for(CLsRef *pLsRef=pJg->GetFirstLsRef();pLsRef!=NULL;pLsRef=pJg->GetNextLsRef())
			{
				if((*pLsRef)->IsFootNail())
					continue;
				if(fabs(dot_prod((*pLsRef)->get_norm(),work_wing_norm))<0.707)
					continue;	//不是当前面螺栓不绘制
				dim_jg_zhun.DimLsZhun(pLsRef);	//分析计算上端螺栓准距线
			}
			dim_jg_zhun.EndDim();
			int now_g=jgzj.g;
			getjgzj(jgzj,pJg->GetWidth());
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			DimJgZhun(pBlockTableRecord,dcs,dim_jg_zhun,jgzj,now_g,pJg->m_bEnableTeG);
			dim_jg_zhun.Empty();
		}
	}
}

void DrawTubeStru(AcDbBlockTableRecord *pBlockTableRecord,CLDSLineTube *pTube, CDrawCoordTrans dcs,
				  BOOL bDimLineAngle=FALSE,BOOL bSetupDrawing=FALSE,AcDbObjectId *pOutEntId=NULL)
{
	AcDbObjectId layerId;
	AcDbObjectId outPutEntId=0;	//关联构件句柄的实体ID wht 10-12-02
	int h0v1;
	f3dPoint point,len_vec,norm_vec,start_weld,end_weld;
	len_vec=pTube->End()-pTube->Start();
	normalize(len_vec);//杆件长度方向(自始端指向末端)
	if(pTube->feature=='X')
		h0v1=0;
	else if(pTube->feature=='Z')
		h0v1=1;
	else
	{
		if(fabs(len_vec*dcs.PrjNorm())<0.707)
			h0v1=0;
		else
			h0v1=1;
	}
	// ____________________   up outter line: start_firs,end_firs
	//|┄┄┄┄┄┄┄┄┄┄|　up inner line:start_sec,end_sec
	//|_ _ _ _ _ _ _ _ _ _ |  
	//|                    |  axis line: start_thir,end_thir
	//|┄┄┄┄┄┄┄┄┄┄|  down inner line: start_four,end_four
	// ￣￣￣￣￣￣￣￣￣￣   down outter line: start_fif,end_fif
	if(h0v1==0)
	{	//平铺绘制
		if(bDimLineAngle&&fabs(len_vec*dcs.cs.axis_x)>EPS_COS)
			bDimLineAngle=FALSE;	//与绘图坐标系X轴方向一致不需标注角度

		f3dLine upOutterLine,upInnerLine,axisLine,downInnerLine,downOutterLine,frontAxisLine;
		double Tube_thick,radius=pTube->GetDiameter()/2.0;
		Tube_thick = __max(pTube->GetThick(),sys.stru_map.fMinPartThick);	//钢管壁厚夸张画法
		axisLine.startPt=pTube->Start()-len_vec*pTube->startOdd();
		axisLine.endPt  =pTube->End()+len_vec*pTube->endOdd();
		//norm_vec为当前绘制平面上垂直于杆件的偏移方向	
		//RotateVectorAroundVector(norm_vec,Pi/2,ucs.axis_z); //绕Z轴旋转仅能保证垂直于杆件方向但不能保证垂直于Z轴方向
		//应保证norm_vec同时垂直于杆件方向以及绘图坐标系Z轴方向	 wht 09-12-08
		norm_vec=len_vec^dcs.PrjNorm();
		normalize(norm_vec);
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
		if(pTube->startOdd()<0)
			axisLine.startPt+=len_vec*pTube->startOdd();
		axisLine.startPt-=len_vec*sys.stru_map.fPoleAxisLineStartOutSpreadLen;//准线两端都伸出30mm
		if(pTube->endOdd()<0)
			axisLine.endPt-=len_vec*pTube->endOdd();
		axisLine.endPt+=len_vec*sys.stru_map.fPoleAxisLineEndOutSpreadLen;//准线两端都伸出30mm
		//绘制钢管端部连接构造
		BOOL bStartTransect=FALSE,bEndTransect=FALSE;
		double start_transect_Tube_d=0,end_transect_Tube_d=0;
		if(pTube->m_tJointStart.type==0||	//平面切割
			(pTube->m_tJointStart.type==1&&pTube->m_tJointStart.hViceLinkObj<0))//对接相贯
		{	
			if(!pTube->m_tJointStart.cutPlaneNorm.IsZero())
			{	//切割面法线有效
				Int3dlf(upOutterLine.startPt,upOutterLine,pTube->m_tJointStart.cutPlanePos,pTube->m_tJointStart.cutPlaneNorm);
				Int3dlf(upInnerLine.startPt,upInnerLine,pTube->m_tJointStart.cutPlanePos,pTube->m_tJointStart.cutPlaneNorm);
				//Int3dlf(axisLine.startPt,axisLine,pTube->m_tJointStart.cutPlanePos,pTube->m_tJointStart.cutPlaneNorm);
				Int3dlf(downInnerLine.startPt,downInnerLine,pTube->m_tJointStart.cutPlanePos,pTube->m_tJointStart.cutPlaneNorm);
				Int3dlf(downOutterLine.startPt,downOutterLine,pTube->m_tJointStart.cutPlanePos,pTube->m_tJointStart.cutPlaneNorm);
			}
		}
		else if(pTube->m_tJointStart.hLinkObj>0&&pTube->m_tJointStart.type==1&&pTube->m_tJointStart.hViceLinkObj>=0)
		{	//绘制钢管相贯线
			CLDSLineTube *pLineTube = (CLDSLineTube*)Ta.FromPartHandle(pTube->m_tJointStart.hLinkObj,CLS_LINETUBE);
			if(pLineTube)
			{	//相贯面和钢管的第一个交点
				bStartTransect=TRUE;
				start_transect_Tube_d=pLineTube->GetDiameter();
				f3dPoint inters1, inters2, line_vec=upOutterLine.startPt-upOutterLine.endPt;
				int n=Int3dlc(&inters1,&inters2,upOutterLine.endPt,line_vec,
					pLineTube->Start(),pLineTube->End(),pLineTube->GetDiameter()/2,pLineTube->GetDiameter()/2);
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
				//相贯面和钢管的第二个交点
				n=Int3dlc(&inters1,&inters2,downOutterLine.endPt,line_vec,
					pLineTube->Start(),pLineTube->End(),pLineTube->GetDiameter()/2,pLineTube->GetDiameter()/2);
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
				//相贯面和钢管的第三个交点
				n=Int3dlc(&inters1,&inters2,frontAxisLine.endPt,line_vec,
					pLineTube->Start(),pLineTube->End(),pLineTube->GetDiameter()/2,pLineTube->GetDiameter()/2);
				if(n==1)
					frontAxisLine.startPt = inters1;
				if(n==2)
				{
					if(DISTANCE(inters1,frontAxisLine.endPt)<DISTANCE(inters2,frontAxisLine.endPt))
						frontAxisLine.startPt = inters1;
					else
						frontAxisLine.startPt = inters2;
				}
				//处理相贯对内侧厚度线造成的影响
				Int3dlf(upInnerLine.startPt,upInnerLine.startPt,line_vec,upOutterLine.startPt,line_vec);
				Int3dlf(downInnerLine.startPt,downInnerLine.startPt,line_vec,downOutterLine.startPt,line_vec);
			}
		}
		else if(pTube->m_tJointStart.hLinkObj>0&&pTube->m_tJointStart.type>1)
		{	//端板连接
			CLDSParamPlate *pParamPlate = (CLDSParamPlate*)Ta.FromPartHandle(pTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			if(pParamPlate&&(pParamPlate->m_iParamType==TYPE_UEND
				||pParamPlate->m_iParamType==TYPE_ROLLEND
				||pParamPlate->m_iParamType==TYPE_XEND))
			{
				double M;
				pParamPlate->GetDesignItemValue('M', &M);
				start_weld	= pParamPlate->ucs.origin + radius*pParamPlate->ucs.axis_y;
				end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
				start_weld=dcs.TransToDraw(start_weld);
				end_weld=dcs.TransToDraw(end_weld);
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					DrawWeldLineSymbol(pBlockTableRecord,start_weld,end_weld,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				else
					DrawWeldLineSymbol(pBlockTableRecord,end_weld,start_weld,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				start_weld	= pParamPlate->ucs.origin - radius*pParamPlate->ucs.axis_y;
				end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
				start_weld=dcs.TransToDraw(start_weld);
				end_weld=dcs.TransToDraw(end_weld);
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					DrawWeldLineSymbol(pBlockTableRecord,end_weld,start_weld,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				else
					DrawWeldLineSymbol(pBlockTableRecord,start_weld,end_weld,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
			}
		}
		if(pTube->m_tJointEnd.type==0||		//平面切割
			(pTube->m_tJointEnd.type==1&&pTube->m_tJointEnd.hViceLinkObj<0))//对接相贯
		{	
			if(!pTube->m_tJointEnd.cutPlaneNorm.IsZero())
			{	//切割面法线有效
				Int3dlf(upOutterLine.endPt,upOutterLine,pTube->m_tJointEnd.cutPlanePos,pTube->m_tJointEnd.cutPlaneNorm);
				Int3dlf(upInnerLine.endPt,upInnerLine,pTube->m_tJointEnd.cutPlanePos,pTube->m_tJointEnd.cutPlaneNorm);
				//Int3dlf(axisLine.endPt,axisLine,pTube->m_tJointEnd.cutPlanePos,pTube->m_tJointEnd.cutPlaneNorm);
				Int3dlf(downInnerLine.endPt,downInnerLine,pTube->m_tJointEnd.cutPlanePos,pTube->m_tJointEnd.cutPlaneNorm);
				Int3dlf(downOutterLine.endPt,downOutterLine,pTube->m_tJointEnd.cutPlanePos,pTube->m_tJointEnd.cutPlaneNorm);
			}
		}
		else if(pTube->m_tJointEnd.hLinkObj&&pTube->m_tJointEnd.type==1&&pTube->m_tJointEnd.hViceLinkObj>=0)	//终端存在相贯面
		{	//绘制钢管相贯线
			CLDSLineTube* pLineTube = (CLDSLineTube*)Ta.FromPartHandle(pTube->m_tJointEnd.hLinkObj,CLS_LINETUBE);
			if(pLineTube)
			{	//相贯面和钢管的第一个交点
				bEndTransect=TRUE;
				end_transect_Tube_d=pLineTube->GetDiameter();
				f3dPoint inters1, inters2, line_vec=upOutterLine.endPt-upOutterLine.startPt;
				int n=Int3dlc(&inters1,&inters2,upOutterLine.startPt,line_vec,
					pLineTube->Start(),pLineTube->End(),pLineTube->GetDiameter()/2,pLineTube->GetDiameter()/2);
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
				//相贯面和钢管的第二个交点
				n=Int3dlc(&inters1,&inters2,downOutterLine.startPt,line_vec,
					pLineTube->Start(),pLineTube->End(),pLineTube->GetDiameter()/2,pLineTube->GetDiameter()/2);
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
				//相贯面和钢管的第三个交点
				n=Int3dlc(&inters1,&inters2,frontAxisLine.startPt,line_vec,
					pLineTube->Start(),pLineTube->End(),pLineTube->GetDiameter()/2,pLineTube->GetDiameter()/2);
				if(n==1)
					frontAxisLine.endPt = inters1;
				if(n==2)
				{
					if(DISTANCE(inters1,frontAxisLine.startPt)<DISTANCE(inters2,frontAxisLine.startPt))
						frontAxisLine.endPt = inters1;
					else
						frontAxisLine.endPt = inters2;
				}	
				//处理相贯对内侧厚度线造成的影响
				Int3dlf(upInnerLine.endPt,upInnerLine.endPt,line_vec,upOutterLine.endPt,line_vec);
				Int3dlf(downInnerLine.endPt,downInnerLine.endPt,line_vec,downOutterLine.endPt,line_vec);
			}
		}
		else if(pTube->m_tJointEnd.hLinkObj>0&&pTube->m_tJointEnd.type>1)
		{	//端板连接
			CLDSParamPlate *pParamPlate = (CLDSParamPlate*)Ta.FromPartHandle(pTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
			if(pParamPlate&&(pParamPlate->m_iParamType==TYPE_UEND
				||pParamPlate->m_iParamType==TYPE_ROLLEND
				||pParamPlate->m_iParamType==TYPE_XEND))
			{
				double M;
				pParamPlate->GetDesignItemValue('M', &M);
				start_weld	= pParamPlate->ucs.origin + radius*pParamPlate->ucs.axis_y;
				end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
				start_weld=dcs.TransToDraw(start_weld);
				end_weld=dcs.TransToDraw(end_weld);
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					DrawWeldLineSymbol(pBlockTableRecord,start_weld,end_weld);
				else
					DrawWeldLineSymbol(pBlockTableRecord,end_weld,start_weld);
				start_weld	= pParamPlate->ucs.origin - radius*pParamPlate->ucs.axis_y;
				end_weld	= start_weld + M*pParamPlate->ucs.axis_x;
				start_weld=dcs.TransToDraw(start_weld);
				end_weld=dcs.TransToDraw(end_weld);
				if(pParamPlate->ucs.axis_z*dcs.PrjNorm()>0)
					DrawWeldLineSymbol(pBlockTableRecord,end_weld,start_weld,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				else
					DrawWeldLineSymbol(pBlockTableRecord,start_weld,end_weld,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
			}
		}
		//转换到绘图坐标系下
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
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		vector_trans(len_vec,dcs,FALSE);
		len_vec.z=0;
		normalize(len_vec);
		double hide_length=pTube->GetDiameter()/dcs.m_fDrawScale;
		if(bStartTransect)	//始端存在相贯面
		{
			//处理消隐
			fPtList vertexlist;
			vertexlist.append(upOutterLine.startPt+len_vec*hide_length);
			vertexlist.append(upOutterLine.startPt);
			vertexlist.append(frontAxisLine.startPt);
			vertexlist.append(downOutterLine.startPt);
			vertexlist.append(downOutterLine.startPt+len_vec*hide_length);
			ProcessHideLine(pBlockTableRecord,vertexlist,LayerTable::UnvisibleProfileLayer.layerName);
			if(start_transect_Tube_d==pTube->GetDiameter())
			{
				if(!sys.stru_map.bUsePolyLineDrawEdge)
				{
					CreateAcadLine(pBlockTableRecord,upOutterLine.startPt,frontAxisLine.startPt);
					CreateAcadLine(pBlockTableRecord,frontAxisLine.startPt,downOutterLine.startPt);
				}
				else
				{
					CreateAcadPolyLine(pBlockTableRecord,upOutterLine.startPt,frontAxisLine.startPt,sys.stru_map.fPolyLineWidth);
					CreateAcadPolyLine(pBlockTableRecord,frontAxisLine.startPt,downOutterLine.startPt,sys.stru_map.fPolyLineWidth);
				}
			}
			else
				CreateAcadSpline(pBlockTableRecord,upOutterLine.startPt,frontAxisLine.startPt,downOutterLine.startPt);
		}
		else
		{
			if(!sys.stru_map.bUsePolyLineDrawEdge)
				CreateAcadLine(pBlockTableRecord,upOutterLine.startPt,downOutterLine.startPt);
			else
				CreateAcadPolyLine(pBlockTableRecord,upOutterLine.startPt,downOutterLine.startPt,sys.stru_map.fPolyLineWidth);
		}
		if(bEndTransect)	//终端存在相贯面
		{
			//处理消隐
			fPtList vertexlist;
			vertexlist.append(downOutterLine.endPt-len_vec*hide_length);
			vertexlist.append(downOutterLine.endPt);
			vertexlist.append(frontAxisLine.endPt);
			vertexlist.append(upOutterLine.endPt);
			vertexlist.append(upOutterLine.endPt-len_vec*hide_length);
			ProcessHideLine(pBlockTableRecord,vertexlist,LayerTable::UnvisibleProfileLayer.layerName);
			if(end_transect_Tube_d==pTube->GetDiameter())
			{
				if(!sys.stru_map.bUsePolyLineDrawEdge)
				{
					CreateAcadLine(pBlockTableRecord,upOutterLine.endPt,frontAxisLine.endPt);
					CreateAcadLine(pBlockTableRecord,frontAxisLine.endPt,downOutterLine.endPt);
				}
				else
				{
					CreateAcadPolyLine(pBlockTableRecord,upOutterLine.endPt,frontAxisLine.endPt,sys.stru_map.fPolyLineWidth);
					CreateAcadPolyLine(pBlockTableRecord,frontAxisLine.endPt,downOutterLine.endPt,sys.stru_map.fPolyLineWidth);
				}
			}
			else
				CreateAcadSpline(pBlockTableRecord,downOutterLine.endPt,frontAxisLine.endPt,upOutterLine.endPt);
		}
		else
		{
			if(!sys.stru_map.bUsePolyLineDrawEdge)
				CreateAcadLine(pBlockTableRecord,upOutterLine.endPt,downOutterLine.endPt);
			else
				CreateAcadPolyLine(pBlockTableRecord,upOutterLine.endPt,downOutterLine.endPt,sys.stru_map.fPolyLineWidth);
		}
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		outPutEntId=CreateAcadLine(pBlockTableRecord,axisLine.startPt,axisLine.endPt,pTube->handle);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(!sys.stru_map.bUsePolyLineDrawEdge)
		{
			CreateAcadLine(pBlockTableRecord,upOutterLine.startPt,upOutterLine.endPt);
			CreateAcadLine(pBlockTableRecord,downOutterLine.startPt,downOutterLine.endPt);
		}
		else
		{
			CreateAcadPolyLine(pBlockTableRecord,upOutterLine.startPt,upOutterLine.endPt,sys.stru_map.fPolyLineWidth);
			CreateAcadPolyLine(pBlockTableRecord,downOutterLine.startPt,downOutterLine.endPt,sys.stru_map.fPolyLineWidth);
		}
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,upInnerLine.startPt,upInnerLine.endPt);
		CreateAcadLine(pBlockTableRecord,downInnerLine.startPt,downInnerLine.endPt);
		if(bDimLineAngle)
		{	//标注当前钢管(支管)与绘图坐标系X轴间的夹角
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			AcDbObjectId pointId,LineId;//定义标识符
			SETUPANGULARDIM_STRU angular_dim;
			f3dPoint vertexPoint,lineStart,lineEnd,arcPoint;
			//角顶点标记点
			Int3dll(axisLine,f3dLine(f3dPoint(0,0,0),f3dPoint(100,0,0)),vertexPoint);
			AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(vertexPoint.x,vertexPoint.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.vertexPointId=pointId;
			//角起始边标记点
			lineStart = 0.7*axisLine.startPt+0.3*axisLine.endPt;
			pPoint=new AcDbPoint(AcGePoint3d(lineStart.x,lineStart.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.lineStartPointId=pointId;
			//终止边标记点
			f3dPoint prj_line_vec=lineStart-vertexPoint;
			lineEnd.Set(vertexPoint.x+prj_line_vec.mod(),vertexPoint.y);
			pPoint=new AcDbPoint(AcGePoint3d(lineEnd.x,lineEnd.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.lineEndPointId=pointId;
			//标注位置圆弧标记点
			arcPoint=(lineStart+lineEnd)*0.5;
			pPoint=new AcDbPoint(AcGePoint3d(arcPoint.x,arcPoint.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();	
			angular_dim.arcPointId=pointId;
			DRAGSET.dimAngleList.append(angular_dim);
		}
	}
	else
	{
		Int3dlf(point,pTube->Start(),len_vec,dcs.PrjOrigin(),dcs.PrjNorm());
		point=dcs.TransToDraw(point);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadCircle(pBlockTableRecord,point,0.5*pTube->GetDiameter()/dcs.m_fDrawScale);
		outPutEntId=CreateAcadCircle(pBlockTableRecord,point,(0.5*pTube->GetDiameter()-pTube->GetThick())/dcs.m_fDrawScale,pTube->handle);
		//结构图绘制钢管截面图时不需要绘制焊道线方向及坐标系 wht 11-07-13
		/*
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		f3dPoint axis_start,axis_end,axis_x,axis_y;
		pTube->BuildUCS();
		axis_x=pTube->ucs.axis_x;
		if(pTube->m_bHasWeldRoad)
		{
			double weldRoadAngle=pTube->CalWeldRoadAngle();
			RotateVectorAroundVector(axis_x,weldRoadAngle,pTube->ucs.axis_z);
		}
		vector_trans(axis_x,ucs,FALSE);
		vector_trans(len_vec,ucs,FALSE);
		axis_x.Set(axis_x.x-len_vec.x*axis_x.z/len_vec.z,axis_x.y-len_vec.y*axis_x.z/len_vec.z);
		normalize(axis_x);
		axis_y.Set(-axis_x.y,axis_x.x);
		double extend_len=max(pTube->GetDiameter()/scale,25);
		axis_start=point-axis_x*extend_len;
		axis_end=point+axis_x*extend_len;
		CreateAcadLine(pBlockTableRecord,axis_start,axis_end);	//,pTube->handle);
		CreateAcadLine(pBlockTableRecord,axis_end,axis_end-axis_x*2+axis_y*0.75);	//,pTube->handle);
		CreateAcadLine(pBlockTableRecord,axis_end,axis_end-axis_x*2-axis_y*0.75);	//,pTube->handle);
		double text_angle=Cal2dLineAng(0,0,axis_x.x,axis_x.y);
		f3dPoint offset;
		if(text_angle>Pi/2&&text_angle<=1.5*Pi)
		{
			text_angle-=Pi;
			offset=-axis_x*2;
		}
		DimText(pBlockTableRecord,offset+axis_end,"焊道方向",TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,text_angle,AcDb::kTextLeft,AcDb::kTextBottom);
		axis_start=point-axis_y*extend_len;
		axis_end=point+axis_y*extend_len;
		CreateAcadLine(pBlockTableRecord,axis_start,axis_end);
		*/
	}
	if(pOutEntId)	//关联构件句柄的CAD实体ID wht 10-12-02
		*pOutEntId=outPutEntId;
}

/*void DimPlankLs(AcDbBlockTableRecord *pBlockTableRecord, 
				CLDSPlate *pPlank, UCS_STRU &ucs, char* layer, 
				double scale)
{
	int i,n;
	AcGePoint3d acad_start,acad_end,acad_dim;

	CDimPlankLs dim_ljb_ls;
	AcDbObjectId dimStyleId;
	//分析连接板上的螺栓标注信息
	dim_ljb_ls.Create(pPlank);
	dimStyleId = GetDimStyle(NULL,sys.dim_map.fDimTextSize);	//标注字高
	if(sys.stru_map.bDimSpecialLsEndSpace||sys.stru_map.bDimSpecialLsSpace)
	{
		ALIGNDIM_STRU align_dim;	//倾斜直线间距标注
		char sDimText[24];
		n = dim_ljb_ls.GetAlignDimCount();
		for(i=0;i<n;i++)
		{
			dim_ljb_ls.GetAlignDimAt(i,align_dim);
			sprintf(sDimText,"%.0f",align_dim.dist);
			coord_trans(align_dim.startPos,pPlank->ucs,TRUE);
			coord_trans(align_dim.dimPos,pPlank->ucs,TRUE);
			coord_trans(align_dim.endPos,pPlank->ucs,TRUE);
			coord_trans(align_dim.startPos,ucs,FALSE);
			coord_trans(align_dim.endPos,ucs,FALSE);
			coord_trans(align_dim.dimPos,ucs,FALSE);
			align_dim.startPos = align_dim.startPos*(1.0/scale);
			align_dim.endPos   = align_dim.endPos*(1.0/scale);
			align_dim.dimPos = align_dim.dimPos*(1.0/scale);
			align_dim.startPos.z = align_dim.endPos.z 
				= align_dim.dimPos.z = 0;
			align_dim.dimPos = (align_dim.startPos+align_dim.endPos)/2.0;
			Cpy_Pnt(acad_start,align_dim.startPos);
			Cpy_Pnt(acad_end,align_dim.endPos);
			Cpy_Pnt(acad_dim,align_dim.dimPos);
			//搜寻合适的标注位置
			f3dPoint dim,rgn_vert[4];
			Cpy_Pnt(dim,acad_dim);
			ads_name ss_name;
			rgn_vert[0].Set(dim.x-2.0,dim.y-2.0,dim.z);
			rgn_vert[1].Set(dim.x+2.0,dim.y-2.0,dim.z);
			rgn_vert[2].Set(dim.x+2.0,dim.y+2.0,dim.z);
			rgn_vert[3].Set(dim.x-2.0,dim.y+2.0,dim.z);
			SearchNoneIntersPos( &dim,rgn_vert,NULL,ss_name);
			acedSSFree(ss_name);
			Cpy_Pnt(acad_dim,dim);
			DimSize(pBlockTableRecord,acad_start,
				acad_end,acad_dim,sDimText,dimStyleId,2,layer);
		}
	}

	//标注螺栓射线
	if(sys.stru_map.bDimLsRayInfo)
	{
		n = dim_ljb_ls.GetTextDimCount();
		TXTDIM_STRU txt_dim;
		for(i=0;i<n;i++)
		{
			if(dim_ljb_ls.GetTextDimAt(i,txt_dim))
			{
				double txt_ang;//文本标注角度
				int quad_order[4]={1,2,3,4};	//搜索合适标位置时的象限优先顺序
				coord_trans(txt_dim.dimPos,pPlank->ucs,TRUE);
				coord_trans(txt_dim.dimPos,ucs,FALSE);
				if(txt_dim.dimVec.mod()>EPS)
				{
					vector_trans(txt_dim.dimVec,pPlank->ucs,TRUE);
					vector_trans(txt_dim.dimVec,ucs,FALSE);
					txt_ang = Cal2dLineAng(0,0,
						txt_dim.dimVec.x,txt_dim.dimVec.y);
				}
				else
					txt_ang = 0;
				//确定优先搜索象限顺序
				if(txt_ang>Pi/2&&txt_ang<3*Pi/2)
				{
					quad_order[0] = 3;
					quad_order[1] = 2;
					quad_order[2] = 4;
					quad_order[3] = 1;
					txt_ang -= Pi;
				}
				else
				{
					quad_order[0] = 4;
					quad_order[1] = 1;
					quad_order[2] = 3;
					quad_order[3] = 2;
				}
				txt_dim.dimPos.z = 0;
				txt_dim.dimPos = txt_dim.dimPos*(1.0/scale);
				Cpy_Pnt(acad_dim,txt_dim.dimPos);
				//搜寻合适的标注位置
				double sina=sin(txt_ang),cosa=cos(txt_ang);
				f3dPoint dim,rgn_vert[4];
				Cpy_Pnt(dim,acad_dim);
				ads_name ss_name;
				int l_s = strlen(txt_dim.dimText);
				double len = l_s*sys.dim_map.fLsGuigeTextSize*0.85;
				rgn_vert[0].Set(dim.x,dim.y,dim.z);
				rgn_vert[1].Set(dim.x+len*cosa,dim.y+len*sina,dim.z);
				rgn_vert[2].Set(dim.x+len*cosa-2*sina,dim.y+len*sina+2*cosa,dim.z);
				rgn_vert[3].Set(dim.x-2*sina,dim.y+2*cosa,dim.z);
				SearchNoneIntersPos(&dim,rgn_vert,NULL,ss_name,quad_order);
				acedSSFree(ss_name);
				Cpy_Pnt(acad_dim,dim);
				//利用短画线的颜色(红色)标注射线螺栓信息
				DimText(pBlockTableRecord,acad_dim,txt_dim.dimText,
					TextStyleTable::hzfs.textStyleId,sys.dim_map.fLsGuigeTextSize,txt_ang,layer,AcDb::kTextLeft,AcDb::kTextBase,sys.crIndex.crDashLine);
			}
		}
	}
}*/

//展开绘制双\三面板	wht 10-12-11
void SpreadDrawPlate(AcDbBlockTableRecord *pBlockTableRecord,CLDSPlate *pPlate,CDrawCoordTrans dcs,AcDbObjectId *pOutEntId=NULL)
{	
	if(pPlate->face_N<2)
		return;
	int i,n;
	double rotate_angle=0;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	f3dLine HuoQuLine[2],spreadLine;
	HuoQuLine[0]=pPlate->HuoQuLine[0];
	HuoQuLine[1]=pPlate->HuoQuLine[1];
	f3dPoint HuoQuPick[2];
	for(i=0;i<pPlate->face_N-1;i++)
	{
		HuoQuLine[i].startPt.z+=pPlate->GetNormOffset();
		HuoQuLine[i].endPt.z+=pPlate->GetNormOffset();
		SnapPerp(&HuoQuPick[i],HuoQuLine[i].startPt,HuoQuLine[i].endPt,f3dPoint(0,0,0));
	}
	for(i=0;i<pPlate->face_N-1;i++)
	{
		coord_trans(HuoQuLine[i].startPt,pPlate->ucs,TRUE);
		coord_trans(HuoQuLine[i].endPt,pPlate->ucs,TRUE);
		coord_trans(HuoQuPick[i],pPlate->ucs,TRUE);
	}
	PROFILE_VERTEX *pVertex=NULL,*pNewVertex=NULL;
	//展开钢板
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		pNewVertex=vertex_list.append(*pVertex);
		f3dPoint vertex=pVertex->vertex;
		coord_trans(vertex,pPlate->ucs,TRUE);
		pNewVertex->vertex=vertex;
	}
	//判断与当前绘制面与火曲面共面还是与基准面共面
	UCS_STRU drawUcs=dcs;
	f3dPoint justNorm=dcs.PrjNorm();
	double dd1=justNorm*pPlate->ucs.axis_z;
	double dd2=justNorm*pPlate->huoqufacenorm[0];
	double dd3=justNorm*pPlate->huoqufacenorm[1];
	if(fabs(dd2)>fabs(dd1)&&fabs(dd2)>fabs(dd3)&&pPlate->face_N>1)
	{	//以第一火曲面为绘制面基准
		f3dPoint rotate_axis=pPlate->ucs.axis_z^pPlate->huoqufacenorm[0];
		normalize(rotate_axis);
		f3dPoint huoquline_vec=HuoQuLine[0].endPt-HuoQuLine[0].startPt;
		normalize(huoquline_vec);
		if(rotate_axis*huoquline_vec<0)
		{
			spreadLine.startPt=HuoQuLine[0].endPt;
			spreadLine.endPt=HuoQuLine[0].startPt;
		}
		else
		{	
			spreadLine.startPt=HuoQuLine[0].startPt;
			spreadLine.endPt=HuoQuLine[0].endPt;
		}
		rotate_angle=cal_angle_of_2vec(pPlate->ucs.axis_z,pPlate->huoqufacenorm[0]);
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			rotate_point_around_axis(pVertex->vertex,rotate_angle,spreadLine);
	}
	else if(fabs(dd3)>fabs(dd1)&&fabs(dd3)>fabs(dd2)&&pPlate->face_N==3)
	{	//以第二火曲面为绘制面基准
		f3dPoint rotate_axis=pPlate->ucs.axis_z^pPlate->huoqufacenorm[1];
		normalize(rotate_axis);
		f3dPoint huoquline_vec=HuoQuLine[1].endPt-HuoQuLine[1].startPt;
		normalize(huoquline_vec);
		if(rotate_axis*huoquline_vec<0)
		{
			spreadLine.startPt=HuoQuLine[1].endPt;
			spreadLine.endPt=HuoQuLine[1].startPt;
		}
		else
		{	
			spreadLine.startPt=HuoQuLine[1].startPt;
			spreadLine.endPt=HuoQuLine[1].endPt;
		}
		rotate_angle=cal_angle_of_2vec(pPlate->ucs.axis_z,pPlate->huoqufacenorm[1]);
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			rotate_point_around_axis(pVertex->vertex,rotate_angle,spreadLine);
	}
	double sum_len=0;
	fPtList vertexlist;
	f3dPoint f_start,f_end,geom_center;
	n=vertex_list.GetNodeNum();
	//处理板消隐
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=vertex_list.GetByIndex(i);
		if(pStartVertex==NULL)
			continue;
		f_start = pStartVertex->vertex;
		f_start=dcs.TransToDraw(f_start);
		vertexlist.append(f_start);
	}
	if(vertexlist.GetNodeNum()>0)	
	{	//处理板消隐
		ProcessHideLine(pBlockTableRecord,vertexlist,LayerTable::UnvisibleProfileLayer.layerName);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	}
	//绘制钢板
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=vertex_list.GetByIndex(i);
		PROFILE_VERTEX *pEndVertex=vertex_list.GetByIndex((i+1)%n);
		if(pStartVertex==NULL||pEndVertex==NULL)
			break;
		f_start = pStartVertex->vertex;
		f_end   = pEndVertex->vertex;
		double len=DISTANCE(f_start,f_end);
		geom_center =geom_center+len*(f_start+f_end);
		sum_len+=DISTANCE(f_start,f_end);
		//水平绘制
		f_start=dcs.TransToDraw(f_start);
		f_end=dcs.TransToDraw(f_end);
		if(pStartVertex->type==0)
		{
			if(sys.stru_map.bUsePolyLineDrawEdge)
				CreateAcadPolyLine(pBlockTableRecord,f_start,f_end,sys.stru_map.fPolyLineWidth);
			else 
				CreateAcadLine(pBlockTableRecord,f_start,f_end);
			if(pStartVertex->m_bWeldEdge)
				DrawWeldLineSymbol(pBlockTableRecord,f_start,f_end,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
		}
		else if(pStartVertex->type<3)	//圆弧
		{
			f3dArcLine arcLine;
			f3dPoint worknorm(0,0,1);
			if(pPlate->ucs.axis_z*justNorm<0)	//板法线与当前绘图坐标系Z轴方向反向
				worknorm.z=-1;
			double fPara;
			if(pStartVertex->type==1)		//指定圆弧夹角
			{
				if(pStartVertex->sector_angle<0)
				{
					fPara=-pStartVertex->sector_angle;
					worknorm.z*=-1;
				}
				else
					fPara=pStartVertex->sector_angle;
				arcLine.CreateMethod2(f_start,f_end,worknorm,fPara);
			}
			else if(pStartVertex->type==2)
			{
				if(pStartVertex->radius<0)
				{
					fPara=-pStartVertex->radius;
					worknorm.z*=-1;
				}
				else
					fPara=pStartVertex->radius;
				f3dPoint center_pick=pStartVertex->center;
				//type==2时圆心拾取点为相对坐标需要先转换为绝对坐标再转换到绘图坐标系下 wht 11-04-28
				coord_trans(center_pick,pPlate->ucs,TRUE);
				center_pick=dcs.TransToDraw(center_pick);
				arcLine.CreateMethod3(f_start,f_end,worknorm,fPara/dcs.m_fDrawScale,center_pick);
			}
			
			if(arcLine.SectorAngle()<2*Pi)
			{
				if(sys.stru_map.bUsePolyLineDrawEdge)	//绘制多段线圆弧线
					CreateAcadPolyArcLine(pBlockTableRecord,arcLine,sys.stru_map.fPolyLineWidth);
				else
					CreateAcadArcLine(pBlockTableRecord,arcLine.Center(),arcLine.Start(),arcLine.SectorAngle(),arcLine.WorkNorm());
			}
			else if(i==0)
				CreateAcadCircle(pBlockTableRecord,arcLine.Center(),arcLine.Radius());//,pPlate->handle);
			else
				CreateAcadCircle(pBlockTableRecord,arcLine.Center(),arcLine.Radius());
		}
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	}
	//插入扩展数据点，以存储连接板信息
	if(sum_len<EPS)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("0x%X plate{%s} the perimeter of outline edge is zero, plate appearance is illegal!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
		logerr.Log("0x%X钢板{%s}轮廓边周长为0，板外形不合法！",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
		return;
	}
	geom_center/=2*sum_len;
	geom_center=dcs.TransToDraw(geom_center);	//计算板的外形几何中心点位置
	AcDbXrecord *pXrec;
	AcDbObjectId dictObjId, xrecObjId,pointId;
	AcDbDictionary *pDict;
	AcDbPoint *pPoint=new AcDbPoint(AcGePoint3d(geom_center.x,geom_center.y,geom_center.z));
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
	{
		pPoint->createExtensionDictionary();
		dictObjId = pPoint->extensionDictionary();
		if(pOutEntId)	//关联构件句柄的CAD实体ID wht 10-12-02
			*pOutEntId=pointId;
		pPoint->close();
	}
	acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
	pXrec = new AcDbXrecord;
#ifdef _ARX_2007
	pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
	pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
	pDict->close();
	
	struct resbuf *head;
	head = acutBuildList(AcDb::kDxfInt32, pPlate->handle,AcDb::kDxfInt32, CLS_PLATE,0);
	pXrec->setFromRbChain(*head);
	acutRelRb(head);
	pXrec->close();
	//绘制火曲线
	PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL;
	PROFILE_VERTEX* pBendLine2Start=NULL,*pBendLine2End=NULL;
	pPlate->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
	for(i=0;i<pPlate->face_N-1;i++)
	{	//计算在钢板上的火曲线
		f3dPoint f_start,f_end;
		if(i==0&&pBendLine1Start&&pBendLine1End)
		{
			f_start=pBendLine1Start->vertex;
			f_end  =pBendLine1End->vertex; 
		}
		if(i==1&&pBendLine2Start&&pBendLine2End)
		{
			f_start=pBendLine2Start->vertex;
			f_end  =pBendLine2End->vertex;
		}
		if(!f_start.IsZero()&&!f_end.IsZero())
		{	//绘制火曲线
			coord_trans(f_start,pPlate->ucs,TRUE);
			coord_trans(f_end,pPlate->ucs,TRUE);
			f_start=dcs.TransToDraw(f_start);
			f_end=dcs.TransToDraw(f_end);
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f_start,f_end);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		}
	}
	//展开绘制钢板时,绘制钢板螺栓信息 wht 14-04-15
	f3dPoint prjNorm=dcs.PrjNorm();
	DrawLsRefList(pPlate->GetLsRefList(),dcs,TRUE,&prjNorm);//显示所有螺栓
	CDimPlankLs dimls;
	dimls.Create(pPlate);
}

#ifdef __SMART_DRAW_INC_
typedef struct tagLSINFO{
	f3dPoint pos;
	char guige[20];
}LSINFO;
void SmartDimLsInfo(CLDSPart *pPart,UCS_STRU ucs,AcDbBlockTableRecord *pBlockTableRecord)
{
	if(pPart->GetClassTypeId()!=CLS_PLATE) 
		return;
	if(fabs(ucs.axis_z*pPart->ucs.axis_z)<=0.707)//只标准当前面的螺栓
		return;
	int rayNum = 0;
	int n = -1;
	CLsRefList *pLsRefList = NULL;
	ATOM_LIST<RAYNO_RECORD>rayNoList;	//射线号记录列表
	CLDSPlate *pPlate = (CLDSPlate *)pPart;
	rayNum = pPlate->designInfo.partList.GetNodeNum();
	if(rayNum<=0)
		return;
	pLsRefList = new CLsRefList[rayNum];
	RAYNO_RECORD *pRayNo=NULL;
	for (CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		if(pLsRef->GetLsPtr()->m_bVirtualPart)
			continue;
		int m = 0;
		for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
		{
			if(pLsRef->GetLsPtr()->des_base_pos.hPart==pRayNo->hPart)
			{
				pLsRefList[m].append(*pLsRef);
				break;
			}
			m++;
		}
		if(pRayNo==NULL)
		{
			pRayNo=rayNoList.append();
			pRayNo->dwRayNo=pLsRef->GetLsPtr()->dwRayNo;
			pRayNo->hPart=pLsRef->GetLsPtr()->des_base_pos.hPart;
			n++;
			pLsRefList[n].append(*pLsRef);		
		}
	}
	ATOM_LIST<LSINFO>lsinfolist;
	for (int i=0;i<=n;i++)
	{
		lsinfolist.Empty();//删除上次统计信息
		for(CLsRef *pLsRef=pLsRefList[i].GetFirst();pLsRef;pLsRef=pLsRefList[i].GetNext())
		{
			CLDSBolt *pBolt = pLsRef->GetLsPtr();
			if(pBolt)
			{
				LSINFO *pLsInfo=lsinfolist.append();
				sprintf(pLsInfo->guige,"M%dX%.0f",pBolt->get_d(),pBolt->get_L());
				f3dPoint center = pBolt->ucs.origin;
				coord_trans(center,ucs,FALSE);
				center = center*(1.0/sys.general_set.StruMapScale.fRealToDraw);
#ifdef __SMART_DRAW_INC_
				center+=ucs.origin;//调整偏移位置，智能绘图时执行以下代码 wht 11-02-23
#endif
				pLsInfo->pos.Set(center.x,center.y,center.z);
			}
		}
		double angle;
		LSINFO *pFirstLsInfo,*pSecLsInfo;
		if(lsinfolist.GetNodeNum()==1)
		{
			angle=0;
			pFirstLsInfo=lsinfolist.GetFirst();
		}
		else if(lsinfolist.GetNodeNum()>1)
		{
			pFirstLsInfo=lsinfolist.GetFirst();
			pSecLsInfo=lsinfolist.GetNext();
			LSINFO *pthirdLsInfo = lsinfolist.GetNext();
			if(pthirdLsInfo)
				pSecLsInfo = pthirdLsInfo;//调整标注方向和杆件方向一致
			f3dPoint vec = pSecLsInfo->pos-pFirstLsInfo->pos;
			angle=Cal2dLineAng(0,0,vec.x,vec.y);
			if(angle>Pi/2.0&&angle<=1.5*Pi)
				angle-=Pi;
		}
		if(lsinfolist.GetNodeNum()>0)
		{
			int m=0;
			for(LSINFO *pLsInfo=lsinfolist.GetFirst();pLsInfo;pLsInfo=lsinfolist.GetNext())
			{
				/*int bPush = lsinfolist.push_stack();
				if(stricmp(pLsInfo->guige,pFirstLsInfo->guige)==0)
					m++;
				else//如果lsinfolist中存有两种以上规格只会标注统计第一种规格，造成其他规格螺栓未统计。
				{
					char guige[20];
					sprintf(guige,"%s",pLsInfo->guige);
					f3dPoint text_pos = pLsInfo->pos;
					double angleTemp=0;
					LSINFO *pLsInfoTemp = lsinfolist.GetNext();
					int j = 1;
					while(pLsInfoTemp)
					{
						if(stricmp(pLsInfoTemp->guige,guige)==0)
						{
							j++;
							f3dPoint vec = pLsInfoTemp->pos-pLsInfo->pos;
							angleTemp=Cal2dLineAng(0,0,vec.x,vec.y);
							if(angleTemp>Pi/2.0&&angleTemp<=1.5*Pi)
								angleTemp-=Pi;
							lsinfolist.DeleteCursor();
						}
						pLsInfoTemp = lsinfolist.GetNext();
					}
					char dimText[40];
					sprintf(dimText,"%d%s",j,guige);
					DimText(pBlockTableRecord,text_pos,dimText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,
					angleTemp,AcDb::kTextCenter,AcDb::kTextBottom);
				}
				if(bPush)
					lsinfolist.pop_stack();*/
			}
			char dimText[40];
			sprintf(dimText,"%d%s",m,pFirstLsInfo->guige);
			f3dPoint text_pos=pFirstLsInfo->pos;
			DimText(pBlockTableRecord,text_pos,dimText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,
				angle,AcDb::kTextCenter,AcDb::kTextBottom);
		}
	}
	delete [] pLsRefList;
}
#endif

//获取角钢截面图外形轮廓点(先通过线面求交确定交点，无交点时使用投影点) wht 11-07-11
void GetPlateSectionProfile(CLDSPart *pPart,UCS_STRU dcs,double draw_scale,fPtList &vertexList,BOOL &bVisible)
{
	vertexList.Empty();
	if(pPart==NULL||!pPart->IsPlate())
		return;
	bVisible=TRUE;
	CLDSPlate *pPlate=NULL;
	CLDSParamPlate *pParamPlate=NULL;
	ATOM_LIST<PROFILE_VERTEX> *pVertexList=NULL;
	UCS_STRU plate_ucs;
	double plate_normoffset=0;
	int plate_face_N=1;
	f3dLine HuoquLine[2];
	f3dPoint huoqu_norm[2];
	//绘制钢板截面时启用最小绘制厚度 wht 12-03-24
	double plate_thick=__max(pPart->GetThick(),sys.stru_map.fMinPartThick);
	if(pPart->GetClassTypeId()==CLS_PLATE)
	{
		pPlate=(CLDSPlate*)pPart;
		pVertexList=&pPlate->vertex_list;
		plate_ucs=pPlate->ucs;
		plate_normoffset=pPlate->m_fNormOffset;
		plate_face_N=pPlate->face_N;
		HuoquLine[0]=pPlate->HuoQuLine[0];
		HuoquLine[1]=pPlate->HuoQuLine[1];
		huoqu_norm[0]=pPlate->huoqufacenorm[0];
		huoqu_norm[1]=pPlate->huoqufacenorm[1];
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pParamPlate=(CLDSParamPlate*)pPart;
		pVertexList=&pParamPlate->vertex_list;
		plate_ucs=pParamPlate->ucs;
		plate_normoffset=pParamPlate->GetNormOffset();
		if(pParamPlate->IsZhiWan())
		{
			plate_face_N=2;
			HuoquLine[0].startPt=pParamPlate->ucs.origin;
			HuoquLine[0].endPt+=(1000*pParamPlate->HuoquAxisVec());
			huoqu_norm[0]=pParamPlate->HuoquFaceNorm();
		}
		else 
		{
			plate_face_N=1;
			//某些参数化钢板设计是vertex.feature未赋值，可能导致截面图无法绘制 wht 16-07-05
			for(PROFILE_VERTEX *pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext())
				pVertex->vertex.feature=1;
		}
	}
	else 
		return;
	
	int vertex_num[3]={0},prj_vertex_num[3]={0};
	f3dPoint vertex_arr[3][30];		//分别用来记录基准面、第一火曲面及第二火曲面与当前绘制面求交得到的轮廓点
	f3dPoint prj_vertex_arr[3][30];	//分别用来记录隔面轮廓点在当前绘图面上的投影点仅取X方向坐标
	int i=0,j=0,n=pVertexList->GetNodeNum();
	//用于计算投影点
	UCS_STRU prj_ucs[3];	//投影坐标系
	for(i=0;i<plate_face_N;i++)
	{
		if(i==0)
		{
			prj_ucs[i].origin=plate_ucs.origin;
			prj_ucs[i].axis_y=plate_ucs.axis_z;
		}
		else 
		{
			prj_ucs[i].origin=HuoquLine[i-1].startPt;
			coord_trans(prj_ucs[i].origin,plate_ucs,TRUE);	//HuoquLine存储的是板内相对坐标系下的点坐标 wjh-2011.8.31
			prj_ucs[i].axis_y=huoqu_norm[i-1];
		}
		//绘图面与钢板当前面求交线
		f3dPoint inters_pt,inters_vec;
		if(Int3d2f(&inters_pt,&inters_vec,dcs.origin,dcs.axis_z,prj_ucs[i].origin,prj_ucs[i].axis_y)==1)
		{	//坐标系原点向交线投影
			f3dLine inters_line;
			inters_line.startPt=inters_pt+inters_vec*100000;
			inters_line.endPt=inters_pt-inters_vec*100000;
			SnapPerp(&prj_ucs[i].origin,inters_line,prj_ucs[i].origin);
		}
		prj_ucs[i].axis_z=dcs.axis_z;
		prj_ucs[i].axis_x=inters_vec;	//X轴方向为两面交线方向	//prj_ucs[i].axis_y^prj_ucs[i].axis_z;
		normalize(prj_ucs[i].axis_x);
		prj_ucs[i].axis_y=prj_ucs[i].axis_z^prj_ucs[i].axis_x;
		normalize(prj_ucs[i].axis_y);
		//X轴为两面交线Z轴为其中一面法线，则Z轴一定垂直与X轴,故不用进行以下运算
		//prj_ucs[i].axis_x=prj_ucs[i].axis_y^prj_ucs[i].axis_z;
		//normalize(prj_ucs[i].axis_x);
	}
	// 1.用钢板轮廓边与绘图面线面求交确定交点,同时计算投影点备用
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=pVertexList->GetByIndex(i);
		PROFILE_VERTEX *pEndVertex=pVertexList->GetByIndex((i+1)%n);
		f3dPoint f_start,f_end;
		if(pPlate)
		{	//普通钢板参数化板
			f_start = pPlate->GetRealVertex(pStartVertex->vertex);
			f_end   = pPlate->GetRealVertex(pEndVertex->vertex);
		}
		else if(pParamPlate)
		{	//参数化板
			f_start = pParamPlate->GetRealVertex(pStartVertex->vertex);
			f_end   = pParamPlate->GetRealVertex(pEndVertex->vertex);
		}
		//
		f3dPoint inters_pos;
		int cur_face=0;
		if(f_start.feature<10)
			cur_face=f_start.feature;
		else if(f_end.feature<10)
			cur_face=f_end.feature;
		// 1.1 当前边为直线时计算直线与绘图面交点，圆弧时计算弦与面的交点
		//转换到绝对坐标系下
		f3dLine line(f_start,f_end);
		coord_trans(line.startPt,plate_ucs,TRUE); 
		coord_trans(line.endPt,plate_ucs,TRUE);
		//线面求交pStartVertex->type==0&&
		if(Int3dlf(inters_pos,line,dcs.origin,dcs.axis_z)>0)	
		{	//判断交点是否为有效点(线段内交点)
			BOOL bHasIntersPos=FALSE;
			if(line.PtInLine(inters_pos)>0)
			{
				bHasIntersPos=TRUE;
				f3dPoint start_vec=line.startPt-inters_pos;
				f3dPoint end_vec=line.endPt-inters_pos;
				if((f_start.feature>10&&start_vec.mod()<EPS2)
					||(f_end.feature>10&&end_vec.mod()<EPS2))
					bHasIntersPos=FALSE;	//与火曲线端点重合的交点不在此处添加
			}
			if(bHasIntersPos)
			{
				if(cur_face>=1&&cur_face<=3)
				{
					vertex_arr[cur_face-1][vertex_num[cur_face-1]]=inters_pos;
					vertex_num[cur_face-1]++;
				}
				else if(f_start.feature>10&&f_end.feature>10)
				{	
					int min_face=0,max_face=0;
					min_face=min(f_start.feature/10,f_start.feature%10);
					max_face=max(f_start.feature/10,f_start.feature%10);
					vertex_arr[min_face][vertex_num[min_face]]=inters_pos;
					vertex_num[min_face]++;
					vertex_arr[max_face][vertex_num[max_face]]=inters_pos;
					vertex_num[max_face]++;
				}
			}
		}
		//计算投影点
		int face_no[2]={0};
		if(f_start.feature>10)
		{
			face_no[0]=f_start.feature/10;
			face_no[1]=f_start.feature%10;
		}
		else 
			face_no[0]=f_start.feature;
		for(j=0;j<2;j++)
		{
			if(face_no[j]<1)
				continue;
			int index=face_no[j]-1;
			f3dPoint cur_vertex=f_start;
			coord_trans(cur_vertex,plate_ucs,TRUE); 
			coord_trans(cur_vertex,prj_ucs[index],FALSE);	//转换到投影坐标系下
			if(cur_vertex.z<0)
				cur_vertex.pen.style=-1;
			else 
				cur_vertex.pen.style=1;
			cur_vertex.y=cur_vertex.z=0;	//y、z坐标归零
			prj_vertex_arr[index][prj_vertex_num[index]]=cur_vertex;
			prj_vertex_num[index]++;
		}
		if(pStartVertex->type>0)
		{	// 1.2 圆弧或椭圆弧
			int sign=1;
			UCS_STRU work_ucs=plate_ucs;
			f3dPoint work_norm=plate_ucs.axis_z;
			f3dPoint line_start=f_start,line_end=f_end;
			if(cur_face>1)
			{	
				
				// 1.2.1 构建工作坐标系 
				if(cur_face==2)
					work_norm=huoqu_norm[0];
				else if(cur_face==3)
					work_norm=huoqu_norm[1];
				work_ucs.axis_z=work_norm;
				work_ucs.axis_y=f_end-f_start;
				normalize(work_ucs.axis_y);
				vector_trans(work_ucs.axis_y,plate_ucs,TRUE);
				work_ucs.origin=f_start;
				coord_trans(work_ucs.origin,plate_ucs,TRUE);
				work_ucs.axis_x=work_ucs.axis_y^work_ucs.axis_z;
				normalize(work_ucs.axis_x);
				work_ucs.axis_y=work_ucs.axis_z^work_ucs.axis_x;
				normalize(work_ucs.axis_y);
				// 1.2.2 在工作坐标系下构件圆弧 
				coord_trans(line_start,plate_ucs,TRUE);
				coord_trans(line_end,plate_ucs,TRUE);
				coord_trans(line_start,work_ucs,FALSE);
				coord_trans(line_end,work_ucs,FALSE);
			}
			f3dArcLine arcLine;
			if(pStartVertex->type==1)
			{	//圆弧
				if(pStartVertex->sector_angle<0)
					sign=-1;
				arcLine.CreateMethod2(line_start,line_end,f3dPoint(0,0,sign),sign*pStartVertex->sector_angle);
			}
			else //if(pStartVertex->type==2||pStartVertex->type==3)	//指定圆弧R
			{	//此处椭圆弧近似按圆弧处理
				f3dPoint center_pick=pStartVertex->center;
				double radius=pStartVertex->radius;
				if(pStartVertex->type==3)
				{
					double cosa=work_norm*pStartVertex->column_norm;
					radius/=fabs(cosa);	//计算椭圆弧按圆弧近似处理时的半径
					coord_trans(center_pick,plate_ucs,FALSE);
				}
				if(cur_face>1)
				{
					coord_trans(center_pick,plate_ucs,TRUE);
					coord_trans(center_pick,work_ucs,FALSE);
				}
				arcLine.CreateMethod3(line_start,line_end,f3dPoint(0,0,sign),sign*radius,center_pick);
			}
			// 1.2.3 当前面与绘图基准面求交线,然后将交线转换到工作坐标系下进行直线与圆弧求交
			f3dPoint inters_pick,inters_vec;
			if(Int3d2f(&inters_pick,&inters_vec,dcs.origin,dcs.axis_z,work_ucs.origin,work_ucs.axis_z)==1)
			{	//两面交线与圆弧求交点
				f2dArc arc;
				arc.angle=arcLine.SectorAngle();
				if(arcLine.WorkNorm().z>0)
					arc.startPt = arcLine.Start();
				else
					arc.startPt = arcLine.End();
				arc.centre = arcLine.Center();
				//将交线转换到钢板坐标下与圆弧求交
				f3dLine inters_line;
				coord_trans(inters_pick,work_ucs,FALSE);	
				vector_trans(inters_vec,work_ucs,FALSE);
				inters_line.startPt=inters_pick-inters_vec*100000;
				inters_line.endPt=inters_pick+inters_vec*100000;
				inters_line.startPt.z=inters_line.endPt.z=0;
				//两面交线与圆弧求交点
				f3dPoint pos1,pos2;
				f2dLine line;
				line.startPt.Set(inters_line.startPt.x,inters_line.startPt.y);
				line.endPt.Set(inters_line.endPt.x,inters_line.endPt.y);
				int nRet=Int2dla(line,arc,pos1.x,pos1.y,pos2.x,pos2.y);
				if(nRet==1)
				{
					inters_pos=pos1;
					coord_trans(inters_pos,work_ucs,TRUE);
					if(cur_face>=1&&cur_face<=3)
					{
						vertex_arr[cur_face-1][vertex_num[cur_face-1]]=inters_pos;
						vertex_num[cur_face-1]++;
					}
					else if(f_start.feature>10&&f_end.feature>10)
					{	
						int min_face=0,max_face=0;
						min_face=min(f_start.feature/10,f_start.feature%10);
						max_face=max(f_start.feature/10,f_start.feature%10);
						vertex_arr[min_face][vertex_num[min_face]]=inters_pos;
						vertex_num[min_face]++;
						vertex_arr[max_face][vertex_num[max_face]]=inters_pos;
						vertex_num[max_face]++;
					}
				}
			}
		}
	}
	// 2.计算火曲线与当前绘制面的交点
	BOOL bHasHuoquLineInters=FALSE;	//是否存在火曲线交点
	f3dLine huoqu_line[2];
	for(i=0;i<2;i++)
	{
		if(plate_face_N<2)
			continue;
		if(i==1&&plate_face_N<3)
			continue;
		BOOL bFindStart=FALSE,bFindEnd=FALSE;
		huoqu_line[i].startPt=HuoquLine[i].startPt;
		huoqu_line[i].endPt  =HuoquLine[i].endPt;
		for(PROFILE_VERTEX *pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext())
		{
			if(pVertex->vertex.feature<10)
				continue;	//非火曲点 wht 11-02-22
			if(DistOf3dPtLine(pVertex->vertex,HuoquLine[i].startPt,HuoquLine[i].endPt)<EPS)
			{
				if(!bFindStart)
				{
					huoqu_line[i].startPt=pVertex->vertex;
					bFindStart=TRUE;
				}
				else
				{
					huoqu_line[i].endPt = pVertex->vertex;
					bFindEnd=TRUE;
					break;
				}
			}
		}
		if(pPlate&&pPlate->face_N==3&&DistOf3dPtLine(pPlate->top_point,HuoquLine[i].startPt,HuoquLine[i].endPt)<EPS)
		{
			if(!bFindStart)
			{
				huoqu_line[i].startPt=pPlate->top_point;
				bFindStart=TRUE;
			}
			else if(!bFindEnd)
			{
				huoqu_line[i].endPt=pPlate->top_point;
				bFindEnd=TRUE;
			}
		}
		if(!bFindStart||!bFindEnd)
			continue;	//未找到火曲线
		//计算火曲线与当前绘制面的交点
		coord_trans(huoqu_line[i].startPt,plate_ucs,TRUE); 
		coord_trans(huoqu_line[i].endPt,plate_ucs,TRUE);	
		if(vertex_num[0]==0&&vertex_num[1]==0&&vertex_num[2]==0)
		{	//基准面、第一火曲面及第二火曲面的交点数均为0，此时火曲线在当前绘图平面内
			if(plate_face_N==2)
			{	//双面板直接取火曲线为有效线段即可
				vertex_arr[0][0]=huoqu_line[i].startPt;
				vertex_arr[0][1]=huoqu_line[i].endPt;
				vertex_num[0]=2;
			}
			else if(plate_face_N==3)
			{	//三面板需要判断第一根火曲线还是第二根火曲线为有效线段
				f3dPoint huoqu_vec=huoqu_line[i].startPt-huoqu_line[i].endPt;
				normalize(huoqu_vec);
				if(fabs(huoqu_vec*dcs.axis_z)>EPS_COS2)
				{
					vertex_arr[0][0]=huoqu_line[i].startPt;
					vertex_arr[0][1]=huoqu_line[i].endPt;
					vertex_num[0]=2;
					break;
				}
			}
		}
		else 
		{	//线面求交
			f3dPoint inters_pos;
			if(Int3dlf(inters_pos,huoqu_line[i],dcs.origin,dcs.axis_z))	
			{	//判断交点是否为有效点(线段内交点)
				if(huoqu_line[i].PtInLine(inters_pos)>0)
				{
					int min_face=0,max_face=0;
					//取始终端feature中的较大值(避免使用top_point.feature) 
					int feature=max(huoqu_line[i].startPt.feature,huoqu_line[i].endPt.feature);	
					min_face=min(feature/10,feature%10);
					max_face=max(feature/10,feature%10);
					if(min_face>=1&&min_face<=3&&max_face>=1&&max_face<=3)
					{
						vertex_arr[min_face-1][vertex_num[min_face-1]]=inters_pos;
						vertex_num[min_face-1]++;
						vertex_arr[max_face-1][vertex_num[max_face-1]]=inters_pos;
						vertex_num[max_face-1]++;
					}
					bHasHuoquLineInters=TRUE;
				}
			}
		}
	}
	//钢板为三面板时将top_point投影至各面
	if(pPlate&&pPlate->face_N==3)
	{	
		for(i=0;i<3;i++)
		{	
			prj_vertex_arr[i][prj_vertex_num[i]]=pPlate->top_point;
			coord_trans(prj_vertex_arr[i][prj_vertex_num[i]],pPlate->ucs,TRUE);	//转换到绝对坐标下	
			coord_trans(prj_vertex_arr[i][prj_vertex_num[i]],prj_ucs[i],FALSE);	//转换到投影坐标系下
			prj_vertex_num[i]++;
		}
	}
	// 3.判断是否存在有效交点
	int visible_vertex_num=0;
	BOOL bHasValidIntersVertex=FALSE;
	for(i=0;i<3;i++)
	{
		if(vertex_num[i]>2||(vertex_num[i]==2
			&&DISTANCE(vertex_arr[i][0],vertex_arr[i][1])>1))
		{
			bHasValidIntersVertex=TRUE;
			break;
		}
		//判断是不是所有的投影点都在绘图面Z轴负方向一侧
		for(j=0;j<prj_vertex_num[i];j++)
		{
			if(prj_vertex_arr[i][j].pen.style==1)
				visible_vertex_num++;
		}
	}
	//存在有效交点时使用可见投影点修正X轴方向长度
	//不存在有效交点时使用投影点计算钢板截面轮廓点
	if(plate_face_N<2||!bHasValidIntersVertex)
	{	//有可用交线的火曲板可不考虑投影点 wht 11-07-26
		for(i=0;i<3;i++)
		{
			if(bHasValidIntersVertex&&!bHasHuoquLineInters&&vertex_num[i]<2)
				continue;	//有可用交线且无可用火曲线交点时仅考虑可用交线所在面的投影点 wht 11-07-26
			for(j=0;j<prj_vertex_num[i];j++)
			{
				if(bHasValidIntersVertex&&prj_vertex_arr[i][j].pen.style==-1)
					continue;	//存在有效交点时舍弃所有不可见投影点
				vertex_arr[i][vertex_num[i]]=prj_vertex_arr[i][j];
				//从投影坐标系转换到绝对坐标系下
				coord_trans(vertex_arr[i][vertex_num[i]],prj_ucs[i],TRUE);
				vertex_num[i]++;
			}
		}
	}
	if(visible_vertex_num<2&&!bHasValidIntersVertex)
	{
		if(pPlate&&Ta.FromPartHandle(pPlate->designInfo.m_hBasePart)&&pPlate->m_hPartWeldParent<0x20)
			bVisible=TRUE;	//普通钢板且存在基准构件且无焊接父构件时均按可见轮廓绘制 wht 11-07-31
		else
			bVisible=FALSE;	//可见投影点数小于2且无有效交点，此时钢板截面图为不可见图层
	}
	// 3.从各面交点中，遴选有效的截面线段
	f3dLine face_line[3];
	f3dLine valid_line_arr[2][3];	//记录有效线
	int valid_line_num=0;			//有效线的个数
	for(i=0;i<3;i++)
	{
		if(vertex_num[i]<2)
			continue;	//至少应求得两个交点
		for(int j=0;j<vertex_num[i];j++)
		{
			if(j==0)
				face_line[i].startPt=face_line[i].endPt=vertex_arr[i][j];
			else if(face_line[i].startPt.IsEqual(face_line[i].endPt,0.1))	//j==1有可能与j==0重复（交点正好位于钢板轮廓顶点上） wjh-2017.5.26
				face_line[i].endPt=vertex_arr[i][j];
			else 
			{
				int judge=face_line[i].PtInLine(vertex_arr[i][j]);
				if(judge==-1)		//在线段终端外侧
					face_line[i].endPt=vertex_arr[i][j];
				else if(judge==-2)	//在线段始端外侧
					face_line[i].startPt=vertex_arr[i][j];
			}
		}
		if(face_line[i].startPt!=face_line[i].endPt)
		{
			f3dPoint face_norm;
			if(i==0)
				face_norm=plate_ucs.axis_z;
			else if(i==1)
				face_norm=huoqu_norm[0];
			else 
				face_norm=huoqu_norm[1];
			//将当前面法线投影到绘图面
			f3dPoint old_face_norm=face_norm;
			project_vector(face_norm,dcs.axis_z);
			normalize(face_norm);
			double S=0;
			double cosa=fabs(old_face_norm*face_norm);	//计算板法线及板法线投影到绘图平面后法线的夹角余弦值,用来调整偏移值 wht 11-07-22
			if(pParamPlate&&pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE
				&&pParamPlate->GetDesignItemValue('S',&S)&&S<0)		//-1:设计上封板
			{	//上封板,大于真实板厚的部分应绘制在Z轴负方向侧 wht 12-03-24
				double real_plate_thick=pParamPlate->GetThick();
				double norm_offset=plate_normoffset;
				if(real_plate_thick<sys.stru_map.fMinPartThick)
					norm_offset-=(sys.stru_map.fMinPartThick-real_plate_thick);
				valid_line_arr[0][valid_line_num].startPt=face_line[i].startPt+face_norm*norm_offset/cosa;
				valid_line_arr[0][valid_line_num].endPt=face_line[i].endPt+face_norm*norm_offset/cosa;
				valid_line_arr[1][valid_line_num].startPt=face_line[i].startPt+face_norm*(norm_offset+plate_thick)/cosa;
				valid_line_arr[1][valid_line_num].endPt=face_line[i].endPt+face_norm*(norm_offset+plate_thick)/cosa;
			}
			else 
			{
				valid_line_arr[0][valid_line_num].startPt=face_line[i].startPt+face_norm*plate_normoffset/cosa;
				valid_line_arr[0][valid_line_num].endPt=face_line[i].endPt+face_norm*plate_normoffset/cosa;
				valid_line_arr[1][valid_line_num].startPt=face_line[i].startPt+face_norm*(plate_normoffset+plate_thick)/cosa;
				valid_line_arr[1][valid_line_num].endPt=face_line[i].endPt+face_norm*(plate_normoffset+plate_thick)/cosa;
			}
			valid_line_num++;
		}
	}
	// 4.调整可用直线的顺序(valid_line_num==3)
	if(pPlate&&pPlate->face_N==3&&valid_line_num==3)
	{	//基准面的线段应位于第一火曲面与第二火曲面之间
		f3dLine temp_line=valid_line_arr[0][0];
		valid_line_arr[0][0]=valid_line_arr[0][1];
		valid_line_arr[0][1]=temp_line;
		temp_line=valid_line_arr[1][0];
		valid_line_arr[1][0]=valid_line_arr[1][1];
		valid_line_arr[1][1]=temp_line;
	}
	// 5.计算截面轮廓点
	if(valid_line_num==1)
	{
		vertexList.append(valid_line_arr[0][0].startPt);
		vertexList.append(valid_line_arr[0][0].endPt);
		vertexList.append(valid_line_arr[1][0].endPt);
		vertexList.append(valid_line_arr[1][0].startPt);
	}
	else 
	{
		for(j=0;j<valid_line_num-1;j++)
		{
			f3dLine cur_line=valid_line_arr[0][j];
			f3dLine next_line=valid_line_arr[0][j+1];
			f3dPoint inters_pos;
			int nRetCode=Int3dll(cur_line,next_line,inters_pos);
			if(nRetCode==0||nRetCode==-1)
				continue;	//出现异常，正常情况下相邻两直线一定会有交点
			if(j==0)
			{
				f3dPoint vec_start=inters_pos-cur_line.startPt;
				f3dPoint vec_end=inters_pos-cur_line.endPt;
				if(vec_start.mod2()>vec_end.mod2())	//距离交点远的一侧为应插入的顶点,wjh-2011.8.17
					vertexList.append(cur_line.startPt);
				else
					vertexList.append(cur_line.endPt);
				/*nRetCode=cur_line.PtInLine(inters_pos);
				if(nRetCode==1)
				{
					if(cur_line.startPt==inters_pos)
						vertexList.append(cur_line.endPt);
					else 
						vertexList.append(cur_line.startPt);
				}
				else if(nRetCode==-1)	//交点在线段终端外侧
					vertexList.append(cur_line.startPt);
				else if(nRetCode==-2)	//交点在线段始端外侧
					vertexList.append(cur_line.endPt);*/
			}
			vertexList.append(inters_pos);
			if(j==valid_line_num-2)
			{
				f3dPoint vec_start=inters_pos-next_line.startPt;
				f3dPoint vec_end=inters_pos-next_line.endPt;
				if(vec_start.mod2()>vec_end.mod2())	//距离交点远的一侧为应插入的顶点,wjh-2011.8.17
					vertexList.append(next_line.startPt);
				else
					vertexList.append(next_line.endPt);
				/*nRetCode=next_line.PtInLine(inters_pos);
				if(nRetCode==1)
				{
					if(next_line.startPt==inters_pos)
						vertexList.append(next_line.endPt);
					else 
						vertexList.append(next_line.startPt);
				}
				else if(nRetCode==-1)	//交点在线段终端外侧
					vertexList.append(next_line.startPt);
				else if(nRetCode==-2)	//交点在线段始端外侧
					vertexList.append(next_line.endPt);*/
			}
		}
		for(j=valid_line_num-1;j>0;j--)
		{
			f3dLine cur_line=valid_line_arr[1][j];
			f3dLine next_line=valid_line_arr[1][j-1];
			f3dPoint inters_pos;
			int nRetCode=Int3dll(cur_line,next_line,inters_pos);
			if(nRetCode==0||nRetCode==-1)
				continue;	//出现异常，正常情况下相邻两直线一定会有交点
			if(j==valid_line_num-1)
			{
				f3dPoint vec_start=inters_pos-cur_line.startPt;
				f3dPoint vec_end=inters_pos-cur_line.endPt;
				if(vec_start.mod2()>vec_end.mod2())	//距离交点远的一侧为应插入的顶点,wjh-2011.8.17
					vertexList.append(cur_line.startPt);
				else
					vertexList.append(cur_line.endPt);
				/*nRetCode=cur_line.PtInLine(inters_pos);
				if(nRetCode==1)
				{
					if(cur_line.startPt==inters_pos)
						vertexList.append(cur_line.endPt);
					else 
						vertexList.append(cur_line.startPt);
				}
				else if(nRetCode==-1)	//交点在线段终端外侧
					vertexList.append(cur_line.startPt);
				else if(nRetCode==-2)	//交点在线段始端外侧
					vertexList.append(cur_line.endPt);*/
			}
			vertexList.append(inters_pos);
			if(j==1)
			{
				f3dPoint vec_start=inters_pos-next_line.startPt;
				f3dPoint vec_end=inters_pos-next_line.endPt;
				if(vec_start.mod2()>vec_end.mod2())	//距离交点远的一侧为应插入的顶点,wjh-2011.8.17
					vertexList.append(next_line.startPt);
				else
					vertexList.append(next_line.endPt);
				/*nRetCode=next_line.PtInLine(inters_pos);
				if(nRetCode==1)
				{
					if(next_line.startPt==inters_pos)
						vertexList.append(next_line.endPt);
					else 
						vertexList.append(next_line.startPt);
				}
				else if(nRetCode==-1)	//交点在线段终端外侧
					vertexList.append(next_line.startPt);
				else if(nRetCode==-2)	//交点在线段始端外侧
					vertexList.append(next_line.endPt);*/
			}
		}
	}
	// 6.将截面轮廓点转换到绘图坐标系下并按比例缩放
	f3dPoint *pPt=NULL;
	for(pPt=vertexList.GetFirst();pPt;pPt=vertexList.GetNext())
	{
		coord_trans(*pPt,dcs,FALSE);
		*pPt*=draw_scale;
	}
}

/*h0v1:0表示平铺着绘，1表示竖着绘，2或其它表示自动判断绘制方式,offset用于基准点在平板上平面时的情况
	函数中的定位标记点主要用于拖拽后标注相对尺寸或绝对尺寸时最终确定标注位置
*/
void DrawPlateStruDrawing(AcDbBlockTableRecord *pBlockTableRecord,CLDSPlate *pPlate,CDrawCoordTrans dcs,BOOL bSetupDrawing,AcDbObjectId *pOutEntId=NULL)
{
	int i,n,plate_thick=ftoi(pPlate->GetThick());
	f3dPoint f_start,f_end;
	f3dPoint point,start_firs,end_firs,start_sec,end_sec,
		start_thir,end_thir,axis_start,axis_end,ext_vec,len_vec;
	if (pPlate->IsShadowObject())
		pPlate->ShadowDesign();
	n = pPlate->vertex_list.GetNodeNum();
	if(n<3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("0x%X plate{%s} the amount of outline vertex is less than 3，plate appearance is invalid!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
		logerr.Log("0x%X钢板{%s}轮廓顶点数小于3，为无效板外形！",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
		return;
	}
	f3dPoint geom_center;	//板几何中心
	double sum_len=0;
	//画节点板外形
	f3dLine base_face_line,qu_face1_line,qu_face2_line;
	int h0v1=0;
	CViewPart *pViewPart=NULL;
	pViewPart=(CViewPart*)pPlate->feature;
	if(pViewPart&&pViewPart->cFlag=='X')		//平铺绘制
		h0v1=0;
	else if(pViewPart&&pViewPart->cFlag=='Z')	//竖直绘制
		h0v1=1;
	else if(pViewPart&&pViewPart->cFlag=='S'&&pPlate->face_N>1)	//展开绘制多面板
		h0v1=0;
	else	//未指定绘制方式，自动判断绘制方式
	{
		if(fabs(dcs.PrjNorm()*pPlate->ucs.axis_z)>0.707)
			h0v1=0;	//水平绘制
		else
			h0v1=1;	//竖直绘制
	}
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(h0v1==0&&pPlate->face_N>1)
	{	//水平绘制的多面板需要展开绘制 wht 11-08-09
		SpreadDrawPlate(pBlockTableRecord,pPlate,dcs,pOutEntId);
		return;
	}
	//装配定位基点
	CLDSNode *pDatumNode=Ta.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
	CLDSLineTube *pDatumTube=NULL;
	if(pPlate->m_hPartWeldParent>0x20)
		pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
	f3dPoint datum_line_vec;
	if(pDatumTube)
	{
		datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		vector_trans(datum_line_vec,pPlate->ucs,FALSE);
	}
	if(h0v1==0)
	{
		fPtList vertexlist;
		for(i=0;i<n;i++)
		{
			PROFILE_VERTEX *pStartVertex=pPlate->vertex_list.GetByIndex(i);
			f_start = pPlate->GetRealVertex(pStartVertex->vertex);
			coord_trans(f_start,pPlate->ucs,TRUE);
			f_start=dcs.TransToDraw(f_start);
			vertexlist.append(f_start);
		}
		if(vertexlist.GetNodeNum()>0)	//处理板消隐
		{
			ProcessHideLine(pBlockTableRecord,vertexlist,LayerTable::UnvisibleProfileLayer.layerName);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		}
		f3dLine datum_edge_line;
		for(i=0;i<n;i++)
		{
			PROFILE_VERTEX *pStartVertex=pPlate->vertex_list.GetByIndex(i);
			PROFILE_VERTEX *pEndVertex=pPlate->vertex_list.GetByIndex((i+1)%n);
			f_start = pPlate->GetRealVertex(pStartVertex->vertex);
			f_end   = pPlate->GetRealVertex(pEndVertex->vertex);
			double len=DISTANCE(f_start,f_end);
			geom_center =geom_center+len*(f_start+f_end);
			sum_len+=len;
			coord_trans(f_start,pPlate->ucs,TRUE);
			coord_trans(f_end,pPlate->ucs,TRUE);
			
			f_start=dcs.TransToDraw(f_start);
			f_end=dcs.TransToDraw(f_end);
			vertexlist.append(f_start);	//记录顶点链表用于消隐
			if(pStartVertex->type==0)
			{
				if(pStartVertex->m_bRollEdge&&pStartVertex->manu_space!=0)
				{	//绘制卷边截面图 wht 11-01-05
					f3dPoint roll_edge_vec=f_end-f_start,roll_edge_norm;
					normalize(roll_edge_vec);
					vector_trans(roll_edge_vec,dcs,TRUE);
					vector_trans(roll_edge_vec,pPlate->ucs,FALSE);
					roll_edge_norm.Set(roll_edge_vec.y,-roll_edge_vec.x);
					normalize(roll_edge_norm);
					//
					f3dLine huoqu_line;
					huoqu_line.startPt=f_start;
					huoqu_line.endPt=f_end;
					if(pStartVertex->roll_edge_offset_dist>0)
					{	//根据卷边外移量计算火曲线位置 wht 11-04-09
						f3dPoint prev_vec,next_vec,line_vec;
						PROFILE_VERTEX startVertex=*(pPlate->vertex_list.GetByIndex(i));
						PROFILE_VERTEX endVertex=*(pPlate->vertex_list.GetByIndex((i+1)%n));
						PROFILE_VERTEX prevVertex=*(pPlate->vertex_list.GetByIndex((i-1+n)%n));
						PROFILE_VERTEX nextVertex=*(pPlate->vertex_list.GetByIndex((i+2)%n));
						prevVertex.vertex=pPlate->GetRealVertex(prevVertex.vertex);
						nextVertex.vertex=pPlate->GetRealVertex(nextVertex.vertex);
						startVertex.vertex=pPlate->GetRealVertex(startVertex.vertex);
						endVertex.vertex=pPlate->GetRealVertex(endVertex.vertex);
						prev_vec=startVertex.vertex-prevVertex.vertex;
						next_vec=endVertex.vertex-nextVertex.vertex;
						line_vec=endVertex.vertex-startVertex.vertex;
						normalize(prev_vec);
						normalize(next_vec);
						normalize(line_vec);
						huoqu_line.startPt=startVertex.vertex+roll_edge_norm*pStartVertex->roll_edge_offset_dist;
						huoqu_line.endPt=endVertex.vertex+roll_edge_norm*pStartVertex->roll_edge_offset_dist;
						//
						f3dLine edge_line;
						if(fabs(line_vec*prev_vec)<EPS_COS)
						{	//前一边与当前边求交计算(卷边)边的始端坐标
							edge_line.startPt=startVertex.vertex;
							edge_line.endPt=startVertex.vertex+prev_vec*pStartVertex->roll_edge_offset_dist;
							Int3dll(huoqu_line,edge_line,huoqu_line.startPt);
						}
						if(fabs(line_vec*prev_vec)<EPS_COS)
						{	//后一边与当前边求交计算(卷边)边的终端坐标
							edge_line.startPt=endVertex.vertex;
							edge_line.endPt=endVertex.vertex+next_vec*pStartVertex->roll_edge_offset_dist;
							Int3dll(huoqu_line,edge_line,huoqu_line.endPt);
						}
						f3dPoint huoqu_line_vec;
						huoqu_line_vec=huoqu_line.endPt-huoqu_line.startPt;
						normalize(huoqu_line_vec);
						if(huoqu_line_vec*line_vec<EPS_COS)
						{	//前一边与后一边相交，直接沿roll_edge_norm向外偏移一个卷边外移量
							huoqu_line.startPt=startVertex.vertex+roll_edge_norm*pStartVertex->roll_edge_offset_dist;
							huoqu_line.endPt=endVertex.vertex+roll_edge_norm*pStartVertex->roll_edge_offset_dist;
						}
						coord_trans(huoqu_line.startPt,pPlate->ucs,TRUE);
						coord_trans(huoqu_line.endPt,pPlate->ucs,TRUE);
						huoqu_line.startPt=dcs.TransToDraw(huoqu_line.startPt);
						huoqu_line.endPt=dcs.TransToDraw(huoqu_line.endPt);
						//绘制因卷边外移量生成的前一边以及后一边的延伸线以及移动后的当前边
						if(!sys.stru_map.bUsePolyLineDrawEdge)
						{	//单线绘制卷边
							CreateAcadLine(pBlockTableRecord,f_start,huoqu_line.startPt);
							CreateAcadLine(pBlockTableRecord,f_end,huoqu_line.endPt);
							CreateAcadLine(pBlockTableRecord,huoqu_line.startPt,huoqu_line.endPt);
						}
						else
						{	//多段线绘制卷边
							CreateAcadPolyLine(pBlockTableRecord,f_start,huoqu_line.startPt,sys.stru_map.fPolyLineWidth);
							CreateAcadPolyLine(pBlockTableRecord,f_end,huoqu_line.endPt,sys.stru_map.fPolyLineWidth);
							CreateAcadPolyLine(pBlockTableRecord,huoqu_line.startPt,huoqu_line.endPt,sys.stru_map.fPolyLineWidth);
						}
					}
					else
					{
						if(!sys.stru_map.bUsePolyLineDrawEdge)
							CreateAcadLine(pBlockTableRecord,f_start,f_end);
						else
							CreateAcadPolyLine(pBlockTableRecord,f_start,f_end,sys.stru_map.fPolyLineWidth);
					}
					double dd=dcs.PrjNorm()*pPlate->ucs.axis_z;
					if(pStartVertex->manu_space*dd<0)
						GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
					vector_trans(roll_edge_norm,pPlate->ucs,TRUE);
					vector_trans(roll_edge_norm,dcs,FALSE);
					f3dLine roll_edge_line;
					roll_edge_line.startPt=huoqu_line.startPt+roll_edge_norm*plate_thick*(1.0/dcs.m_fDrawScale);
					roll_edge_line.endPt=huoqu_line.endPt+roll_edge_norm*plate_thick*(1.0/dcs.m_fDrawScale);
					if(!sys.stru_map.bUsePolyLineDrawEdge)
					{	//单线绘制卷边
						CreateAcadLine(pBlockTableRecord,roll_edge_line.startPt,roll_edge_line.endPt);
						CreateAcadLine(pBlockTableRecord,huoqu_line.endPt,roll_edge_line.endPt);
						CreateAcadLine(pBlockTableRecord,roll_edge_line.startPt,huoqu_line.startPt);
					}
					else
					{	//多段线绘制卷边
						CreateAcadPolyLine(pBlockTableRecord,roll_edge_line.startPt,roll_edge_line.endPt,sys.stru_map.fPolyLineWidth);
						CreateAcadPolyLine(pBlockTableRecord,huoqu_line.endPt,roll_edge_line.endPt,sys.stru_map.fPolyLineWidth);
						CreateAcadPolyLine(pBlockTableRecord,roll_edge_line.startPt,huoqu_line.startPt,sys.stru_map.fPolyLineWidth);
					}
				}
				else
				{
					if(!sys.stru_map.bUsePolyLineDrawEdge)
						CreateAcadLine(pBlockTableRecord,f_start,f_end);
					else
						CreateAcadPolyLine(pBlockTableRecord,f_start,f_end,sys.stru_map.fPolyLineWidth);
					if(pStartVertex->m_bWeldEdge)
						DrawWeldLineSymbol(pBlockTableRecord,f_start,f_end,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				}	
			}
			else if(pStartVertex->type<=3)	//圆弧
			{
				f3dArcLine arcLine;
				f3dPoint worknorm(0,0,1);
				if(pPlate->ucs.axis_z*dcs.PrjNorm()<0)	//板法线与当前绘图坐标系Z轴方向反向
					worknorm.z=-1;
				if(pStartVertex->type==1)		//指定圆弧夹角
				{
					if(pStartVertex->sector_angle<0)
						worknorm.z*=-1;
					arcLine.CreateMethod2(f_start,f_end,worknorm,fabs(pStartVertex->sector_angle));
				}
				else if(pStartVertex->type==2)
				{
					double radius=fabs(pStartVertex->radius)/dcs.m_fDrawScale;
					if(pStartVertex->radius<0)
						worknorm.z*=-1;
					f3dPoint center_pick=pStartVertex->center;
					//type==2时圆心拾取点为相对坐标需要先转换为绝对坐标再转换到绘图坐标系下 wht 11-04-28
					coord_trans(center_pick,pPlate->ucs,TRUE);
					center_pick=dcs.TransToDraw(center_pick);
					arcLine.CreateMethod3(f_start,f_end,worknorm,radius,center_pick);
				}
				else if(pStartVertex->type==3)
				{	//椭圆弧
					double radius=fabs(pStartVertex->radius)/dcs.m_fDrawScale;
					if(pStartVertex->radius<0)
						worknorm.z*=-1;
					f3dPoint columnnorm=pStartVertex->column_norm;
					f3dPoint center=pStartVertex->center;
					int iStartFace=pStartVertex->vertex.feature%10;
					int iEndFace=pEndVertex->vertex.feature%10;
					if((iStartFace==2||iStartFace==3)&&iStartFace==iEndFace)
					{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换 wht 11-05-31
						coord_trans(center,pPlate->ucs,FALSE);
						vector_trans(columnnorm,pPlate->ucs,FALSE);
						center=pPlate->GetSpreadVertex(center,iStartFace);
						columnnorm=pPlate->GetSpreadVertex(columnnorm,iStartFace,false);
						coord_trans(center,pPlate->ucs,TRUE);
						vector_trans(columnnorm,pPlate->ucs,TRUE);
					}
					vector_trans(columnnorm,dcs,FALSE);
					center=dcs.TransToDraw(center);
					arcLine.CreateEllipse(center,f_start,f_end,columnnorm,worknorm,radius);
				}
				
				if(arcLine.WorkNorm()==arcLine.ColumnNorm())
				{	//圆弧
					if(arcLine.SectorAngle()<2*Pi)
					{
						if(!sys.stru_map.bUsePolyLineDrawEdge)
							CreateAcadArcLine(pBlockTableRecord,arcLine.Center(),arcLine.Start(),arcLine.SectorAngle(),arcLine.WorkNorm());
						else	//绘制多段线圆弧线
							CreateAcadPolyArcLine(pBlockTableRecord,arcLine,sys.stru_map.fPolyLineWidth);
					}
					else
						CreateAcadCircle(pBlockTableRecord,arcLine.Center(),arcLine.Radius());
				}
				else 
				{	//椭圆弧
					if(!sys.stru_map.bUsePolyLineDrawEdge)
						CreateAcadEllipseLine(pBlockTableRecord,arcLine);
					else	//绘制多段线圆弧线
						CreateAcadPolyArcLine(pBlockTableRecord,arcLine,sys.stru_map.fPolyLineWidth);
				}
			}
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			
			f3dPoint line_vec=pEndVertex->vertex-pStartVertex->vertex;
			normalize(line_vec);
			if(bSetupDrawing&&pDatumTube&&pDatumNode&&pStartVertex->m_bWeldEdge
				&&datum_edge_line.startPt==datum_edge_line.endPt	//基准边未初始化表示第一次出现与钢管平行的轮廓边	
				&&fabs(line_vec*datum_line_vec)>EPS_COS)			//当前边与钢管平行
			{	//第一道与基准钢管平行的焊缝线为基准边
				f3dLine line;
				datum_edge_line.startPt=pStartVertex->vertex;
				datum_edge_line.endPt=pEndVertex->vertex;
				f3dPoint N_vec(-line_vec.y,line_vec.x);	//钢板定位基准边左侧偏移法向量
				
				//判断此连板在基准杆件的上侧还是下侧
				f3dPoint up_vec=N_vec;
				vector_trans(up_vec,pPlate->ucs,TRUE);
				vector_trans(up_vec,dcs,FALSE);
				
				SETUPDRAWINGDIM_STRU setup_dim;
				f3dPoint mark_pos=pDatumNode->Position(true);
				coord_trans(mark_pos,pPlate->ucs,FALSE);
				mark_pos.z=0;
				double start_mark_offset=0,end_mark_offset=0;
				SnapPerp(&mark_pos,datum_edge_line,mark_pos);
				if(line_vec*datum_line_vec>0)	//同向
				{
					//10mm是经验数据，主要是保证标识线要在基准边长度范围内
					end_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
					start_mark_offset=0.5*end_mark_offset;
					start_mark_offset=min(start_mark_offset,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
				}
				else
				{
					end_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
					start_mark_offset=0.5*end_mark_offset;
					start_mark_offset=min(start_mark_offset,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
				}
				line.startPt=mark_pos+N_vec*dcs.m_fDrawScale*1.5-datum_line_vec*start_mark_offset;
				line.endPt=mark_pos-N_vec*dcs.m_fDrawScale*1.5-datum_line_vec*start_mark_offset;
				coord_trans(line.startPt,pPlate->ucs,TRUE);
				coord_trans(line.endPt,pPlate->ucs,TRUE);
				line.startPt=dcs.TransToDraw(line.startPt);
				line.endPt=dcs.TransToDraw(line.endPt);
				//插入定位标记点
				AcDbObjectId pointId;
				AcDbPoint *pPoint=new AcDbPoint(AcGePoint3d(line.startPt.x,line.startPt.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				setup_dim.startPointId=pointId;
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);	//始端侧（左侧）装配方向定位线
				
				line.startPt=mark_pos+N_vec*dcs.m_fDrawScale*1.5;
				line.endPt=mark_pos-N_vec*dcs.m_fDrawScale*1.5;
				coord_trans(line.startPt,pPlate->ucs,TRUE);
				coord_trans(line.endPt,pPlate->ucs,TRUE);
				line.startPt=dcs.TransToDraw(line.startPt);
				line.endPt=dcs.TransToDraw(line.endPt);
				//插入定位标记点
				pPoint=new AcDbPoint(AcGePoint3d(line.startPt.x,line.startPt.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				setup_dim.endPointId=pointId;
				setup_dim.dist=start_mark_offset;
				if(up_vec.y>0)
					setup_dim.dimPosOffset=1;
				else
					setup_dim.dimPosOffset=-1;
				DRAGSET.dimSpaceList.append(setup_dim);
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);	//中间基准线
				
				line.startPt=mark_pos+N_vec*dcs.m_fDrawScale*1.5+datum_line_vec*end_mark_offset;
				line.endPt=mark_pos-N_vec*dcs.m_fDrawScale*1.5+datum_line_vec*end_mark_offset;
				coord_trans(line.startPt,pPlate->ucs,TRUE);
				coord_trans(line.endPt,pPlate->ucs,TRUE);
				line.startPt=dcs.TransToDraw(line.startPt);
				line.endPt=dcs.TransToDraw(line.endPt);
				//插入定位标记点
				pPoint=new AcDbPoint(AcGePoint3d(line.startPt.x,line.startPt.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				setup_dim.startPointId=setup_dim.endPointId;;
				setup_dim.endPointId=pointId;
				setup_dim.dist=end_mark_offset;
				if(up_vec.y>0)
					setup_dim.dimPosOffset=1;
				else
					setup_dim.dimPosOffset=-1;
				DRAGSET.dimSpaceList.append(setup_dim);
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);	//终端侧（右侧）装配方向定位线
				//装配定位标注
				SETUPPOSDIM_STRU pos_dim;
				mark_pos=pDatumNode->Position(true);
				coord_trans(mark_pos,dcs,FALSE);	//不需要进行比例缩放,故不用TransToDraw wjh-2011.8.13
				pos_dim.posDist=mark_pos.x;
				pos_dim.posPointId=setup_dim.startPointId;
				if(up_vec.y>0)
					pos_dim.dimPosOffset=1.0;
				else
					pos_dim.dimPosOffset=-1.0;
				DRAGSET.dimPosList.append(pos_dim);
			}
		}
		//计算几何中心位置用于插入扩展数据点，存储连接板信息
		if(sum_len<EPS)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0x%X plate{%s} the perimeter of outline edge is zero, plate appearance is illegal!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
			logerr.Log("0x%X钢板{%s}轮廓边周长为0，板外形不合法！",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
			return;
		}
		if(sum_len<EPS)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0x%X plate{%s} the perimeter of outline edge is zero, plate appearance is illegal!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
			logerr.Log("0x%X钢板{%s}轮廓边周长为0，板外形不合法！",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
			return;
		}
		geom_center/=2*sum_len;
		coord_trans(geom_center,pPlate->ucs,TRUE);
		geom_center=dcs.TransToDraw(geom_center);

		if(pPlate->m_fInnerRadius>0)
		{	
			if(fabs(pPlate->m_tInnerColumnNorm*pPlate->ucs.axis_z)<EPS_COS)	//圆弧
			{	//椭圆
				f3dArcLine arcLine;
				f3dPoint workNorm(0,0,1),columnNorm=pPlate->m_tInnerColumnNorm;
				if(pPlate->ucs.axis_z*dcs.PrjNorm()<0)	//板法线与当前绘图坐标系Z轴方向反向
					workNorm*=-1.0;
				vector_trans(columnNorm,pPlate->ucs,FALSE);
				f3dPoint minorAxis=columnNorm^workNorm;	
				normalize(minorAxis);	//椭圆短轴方向
				//顺时针旋转90度 wht 11-07-19
				f3dPoint majorAxis(minorAxis.y,-minorAxis.x,minorAxis.z);	
				normalize(majorAxis);	//椭圆长轴方向
				double radiusRatio=fabs(columnNorm*workNorm);
				double minorRadius=pPlate->m_fInnerRadius;				//椭圆短半轴长度
				double majorRadius=pPlate->m_fInnerRadius/radiusRatio;	//椭圆长半轴长度
				f3dPoint firstPt=pPlate->m_tInnerOrigin+majorAxis*majorRadius;
				f3dPoint secPt = pPlate->m_tInnerOrigin+minorAxis*minorRadius;
				f3dPoint thirdPt=pPlate->m_tInnerOrigin-majorAxis*majorRadius;
				f3dPoint fourPt = pPlate->m_tInnerOrigin-minorAxis*minorRadius;
				f3dPoint inner_origin=pPlate->m_tInnerOrigin;
				//转换到绝对坐标系下
				coord_trans(inner_origin,pPlate->ucs,TRUE);
				coord_trans(firstPt,pPlate->ucs,TRUE);
				coord_trans(secPt,pPlate->ucs,TRUE);
				coord_trans(thirdPt,pPlate->ucs,TRUE);
				coord_trans(fourPt,pPlate->ucs,TRUE);
				vector_trans(columnNorm,pPlate->ucs,TRUE);
				//转换到绘图坐标系下
				inner_origin=dcs.TransToDraw(inner_origin);
				firstPt=dcs.TransToDraw(firstPt);
				secPt=dcs.TransToDraw(secPt);
				thirdPt=dcs.TransToDraw(thirdPt);
				fourPt=dcs.TransToDraw(fourPt);
				vector_trans(columnNorm,dcs,FALSE);
				//调整比例
				minorRadius/=dcs.m_fDrawScale;
				arcLine.pen.style=PS_SOLID;
				arcLine.ID=0;
				arcLine.CreateEllipse(inner_origin,firstPt,secPt,columnNorm,workNorm,minorRadius);
				CreateAcadEllipseLine(pBlockTableRecord,arcLine);
				arcLine.CreateEllipse(inner_origin,secPt,thirdPt,columnNorm,workNorm,minorRadius);
				CreateAcadEllipseLine(pBlockTableRecord,arcLine);
				arcLine.CreateEllipse(inner_origin,thirdPt,fourPt,columnNorm,workNorm,minorRadius);
				CreateAcadEllipseLine(pBlockTableRecord,arcLine);
				arcLine.CreateEllipse(inner_origin,fourPt,firstPt,columnNorm,workNorm,minorRadius);
				CreateAcadEllipseLine(pBlockTableRecord,arcLine);
			}
			else
			{
				f3dPoint center=pPlate->m_tInnerOrigin;	//pPlate->ucs.origin;
				//coord_trans(center,ucs,FALSE);
				center/=dcs.m_fDrawScale;
				CreateAcadCircle(pBlockTableRecord,center,pPlate->m_fInnerRadius/dcs.m_fDrawScale);
			}
		}
	}
	else if(h0v1==1)
	{
		BOOL bVisible=TRUE;
		fPtList vertex_list;
		GetPlateSectionProfile(pPlate,dcs,1.0/dcs.m_fDrawScale,vertex_list,bVisible);
		int n=vertex_list.GetNodeNum();
		if(bVisible)
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else	
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		int nCount=0;
		geom_center.Set();
		for(i=0;i<n;i++)
		{
			if(sys.stru_map.bUsePolyLineDrawEdge)	//使用多段线绘制钢板截面图 wht 11-07-19
				CreateAcadPolyLine(pBlockTableRecord,vertex_list[i],vertex_list[(i+1+n)%n],sys.stru_map.fPolyLineWidth);
			else 
				CreateAcadLine(pBlockTableRecord,vertex_list[i],vertex_list[(i+1+n)%n]);
			if(i==0||i==1||(n>2&&(i==n-1||i==n-2)))//使用基准面的4个轮廓点计算扩展数据点位置
			{
				geom_center+=vertex_list[i];
				nCount++;
			}
		}
		if(nCount>0)
			geom_center/=nCount;
	}
	
	//插入扩展数据点，以存储连接板信息 wht 11-05-26
	AcDbObjectId entId=InsertExtensionRecord(pBlockTableRecord,geom_center,pPlate->handle,CLS_PLATE);
	if(pOutEntId)
		*pOutEntId=entId;
	if(h0v1==0)
	{	//绘制火曲线
		PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL;
		PROFILE_VERTEX* pBendLine2Start=NULL,*pBendLine2End=NULL;
		pPlate->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
		for(i=0;i<pPlate->face_N-1;i++)
		{	//计算在钢板上的火曲线
			f3dPoint f_start,f_end;
			if(i==0&&pBendLine1Start&&pBendLine1End)
			{
				f_start=pBendLine1Start->vertex;
				f_end  =pBendLine1End->vertex; 
			}
			if(i==1&&pBendLine2Start&&pBendLine2End)
			{
				f_start=pBendLine2Start->vertex;
				f_end  =pBendLine2End->vertex;
			}
			if(!f_start.IsZero()&&!f_end.IsZero())
			{	//绘制火曲线
				coord_trans(f_start,pPlate->ucs,TRUE);
				coord_trans(f_end,pPlate->ucs,TRUE);
				f_start=dcs.TransToDraw(f_start);
				f_end=dcs.TransToDraw(f_end);
				GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
				CreateAcadLine(pBlockTableRecord,f_start,f_end);
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			}
		}
	}
	//螺栓的法线变化范围比较大
	if(h0v1==0)
	{
		f3dPoint prjNorm=dcs.PrjNorm();
		DrawLsRefList(pPlate->GetLsRefList(),dcs,TRUE,&prjNorm);//显示所有螺栓
		CDimPlankLs dimls;
		dimls.Create(pPlate);
	}
	else if(h0v1==1&&bSetupDrawing&&pDatumTube)
	{
		f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		double dd=fabs(datum_line_vec*pPlate->ucs.axis_z);
		//横向放置的钢管如果有竖向绘制的板与其不垂直时需要标注板与基准钢管的角度
		UCS_STRU temCS=(UCS_STRU)dcs;
		if(fabs(datum_line_vec*temCS.axis_x)>EPS_COS&&dd>EPS&&dd<EPS_COS)
		{	//板与基准钢管不垂直不平行，需要标注角度
			SETUPANGULARDIM_STRU angular_dim;
			f3dPoint vertexPoint,lineStart,lineEnd,arcPoint;
			//角顶点标记点
			vertexPoint=pPlate->ucs.origin;
			vertexPoint=dcs.TransToDraw(vertexPoint);
			AcDbObjectId pointId;
			AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(vertexPoint.x,vertexPoint.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.vertexPointId=pointId;
			//角起始边标记点
			lineStart = pPlate->ucs.origin+datum_line_vec*400;
			lineStart=dcs.TransToDraw(lineStart);
			pPoint=new AcDbPoint(AcGePoint3d(lineStart.x,lineStart.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.lineStartPointId=pointId;
			//终止边标记点
			f3dPoint norm=datum_line_vec^pPlate->ucs.axis_z;
			f3dPoint prj_line_vec=norm^pPlate->ucs.axis_z;
			vector_trans(prj_line_vec,dcs,FALSE);
			prj_line_vec.z=0;
			normalize(prj_line_vec);
			if(prj_line_vec.x<0)
				prj_line_vec*=-1.0;
			lineEnd=pPlate->ucs.origin;
			coord_trans(lineEnd,dcs,FALSE);
			lineEnd+=prj_line_vec*400;
			lineEnd/=dcs.m_fDrawScale;
			lineEnd.z=0;
			pPoint=new AcDbPoint(AcGePoint3d(lineEnd.x,lineEnd.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.lineEndPointId=pointId;
			//标注位置圆弧标记点
			arcPoint=(lineStart+lineEnd)*0.5;
			pPoint=new AcDbPoint(AcGePoint3d(arcPoint.x,arcPoint.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.arcPointId=pointId;
			DRAGSET.dimAngleList.append(angular_dim);
		}
	}
#ifdef __SMART_DRAW_INC_
	//SmartDimLsInfo(pPlate,ucs,pBlockTableRecord);
#endif
}

#ifdef __GGG_
void DrawSlotArmStruDrawing(AcDbBlockTableRecord *pBlockTableRecord, CLDSSlotArm *pSlotArm, 
							UCS_STRU &ucs,double scale,BOOL bSetupDrawing,AcDbObjectId *pOutEntId=NULL)
{
	AcDbObjectId LineId=0;
	//PROFILE_VERTEX *pVertex,*pPreVertex=NULL;
	int i,hOv1=-1;
	if(fabs(ucs.axis_z*pSlotArm->ucs.axis_x)>EPS_COS2)
		hOv1=2;//端面绘制
	else if(fabs(ucs.axis_z*pSlotArm->ucs.axis_z)>0.707)
		hOv1=1;//侧向绘制
	else
		hOv1=0;//平铺绘制
	f3dPoint vertex_arr[20];
	//double fCoverBoardLen=pSlotArm->GetRailLeverLen()-pSlotArm->GetRootInsertLen();

	CXhChar100 sDimText;
	f3dPoint dim_pos,dim_start,dim_end;
	//DimStyleTable::dimStyle.InitDimStyle(sys.dim_map.fDimTextSize);
	AcDbObjectId dimStyleId=DimStyleTable::dimStyle.dimStyleId;
	if(hOv1==1)//侧向
	{
		vertex_arr[0] = pSlotArm->ucs.origin + 0.5*pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y;
		vertex_arr[1] = vertex_arr[0] + pSlotArm->GetRootInsertLen()*pSlotArm->ucs.axis_x;
		vertex_arr[2] = vertex_arr[1] + pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[3] = vertex_arr[0] + pSlotArm->GetRailLeverLen()*pSlotArm->ucs.axis_x + pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[4] = vertex_arr[0] + pSlotArm->GetRailLeverLen()*pSlotArm->ucs.axis_x;
		vertex_arr[9] = pSlotArm->ucs.origin - 0.5*pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y;
		vertex_arr[8] = vertex_arr[9] + pSlotArm->GetRootInsertLen()*pSlotArm->ucs.axis_x;
		vertex_arr[7] = vertex_arr[8] - pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[5] = vertex_arr[4] - pSlotArm->GetEndHigh()*pSlotArm->ucs.axis_y;
		vertex_arr[6] = vertex_arr[5] - pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y; 
		double f=pSlotArm->GetRootInsertLen()-pSlotArm->GetRibPlateLen();
		vertex_arr[10] = pSlotArm->ucs.origin + f*pSlotArm->ucs.axis_x + 0.5*pSlotArm->GetRibPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[11] = vertex_arr[10] - pSlotArm->GetRibPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[12] = vertex_arr[11] + pSlotArm->GetRibPlateLen()*pSlotArm->ucs.axis_x;
		vertex_arr[13] =  vertex_arr[10] + pSlotArm->GetRibPlateLen()*pSlotArm->ucs.axis_x;
		//转换到绘图坐标系下
		for(i=0;i<14;i++)
		{
			coord_trans(vertex_arr[i], ucs, FALSE);//转换到绘图坐标系下
			vertex_arr[i]/=scale;//调配绘图比例		
			vertex_arr[i].z=0;
		}
		//绘制
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1],pSlotArm->handle,2);
		for(int i=1;i<9;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[4]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[8],vertex_arr[5]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[9]);
		for(int i= 10;i<13;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[10],vertex_arr[13]);
		//标注
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		sDimText.Printf("%.f",pSlotArm->GetRailLeverLen());
		dim_pos=0.5*(vertex_arr[9]+vertex_arr[6]);
		dim_pos.y=vertex_arr[9].y-sys.dim_map.fDimTextSize*2*sDimText.Length();
		dim_start.Set(vertex_arr[9].x,vertex_arr[9].y);
		dim_end.Set(vertex_arr[6].x,vertex_arr[9].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetRootInsertLen());
		dim_pos=0.5*(vertex_arr[9]+vertex_arr[8]);
		dim_pos.y=vertex_arr[7].y-sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[9].x,vertex_arr[9].y);
		dim_end.Set(vertex_arr[7].x,vertex_arr[7].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetRootHigh());
		SimplifiedNumString(sDimText);
		dim_pos=0.5*(vertex_arr[9]+vertex_arr[0]);
		dim_pos.x=vertex_arr[9].x-sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[0].x,vertex_arr[0].y);
		dim_end.Set(vertex_arr[9].x,vertex_arr[9].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,Pi*0.5);
		sDimText.Printf("%.f",pSlotArm->GetEndHigh());
		dim_pos=0.5*(vertex_arr[4]+vertex_arr[5]);
		dim_pos.x=vertex_arr[4].x+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[4].x,vertex_arr[4].y);
		dim_end.Set(vertex_arr[5].x,vertex_arr[5].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,Pi*0.5);
		sDimText.Printf("%.f",pSlotArm->GetRibPlateLen());
		dim_pos=0.5*(vertex_arr[10]+vertex_arr[13]);
		dim_pos.y=vertex_arr[10].y+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[10].x,vertex_arr[10].y);
		dim_end.Set(vertex_arr[13].x,vertex_arr[13].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetRibPlateThick());
		dim_pos=0.5*(vertex_arr[12]+vertex_arr[13]);
		dim_pos.x=vertex_arr[12].x+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[12].x,vertex_arr[12].y);
		dim_end.Set(vertex_arr[13].x,vertex_arr[13].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0.5*Pi);
	}
	else if(hOv1==2)//端
	{
		vertex_arr[0] = pSlotArm->ucs.origin + 0.5*pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y - 0.5*pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[1] = vertex_arr[0] + pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[2] = vertex_arr[1] + pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[3] = vertex_arr[0] + pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[4] = pSlotArm->ucs.origin - 0.5*pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y -0.5*pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[5] = vertex_arr[4] + pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[6] = vertex_arr[5] - pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[7] = vertex_arr[4] - pSlotArm->GetCoverPlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[8] = pSlotArm->ucs.origin + 0.5*pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y - 0.5*pSlotArm->GetRootWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[9] = vertex_arr[8] + pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_z;
		vertex_arr[10] = vertex_arr[9] - pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y;
		vertex_arr[11] = vertex_arr[8] - pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y;
		vertex_arr[12] = vertex_arr[8] + pSlotArm->GetRootWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[13] = vertex_arr[12] - pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_z;
		vertex_arr[14] = vertex_arr[13] - pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y;
		vertex_arr[15] = vertex_arr[12] - pSlotArm->GetRootHigh()*pSlotArm->ucs.axis_y;
		double f=pSlotArm->GetRootWidth()-2*pSlotArm->GetSidePlateThick();
		vertex_arr[16] = pSlotArm->ucs.origin + 0.5*pSlotArm->GetRibPlateThick()*pSlotArm->ucs.axis_y - 0.5*f*pSlotArm->ucs.axis_z;
		vertex_arr[17] = vertex_arr[16] + f*pSlotArm->ucs.axis_z;
		vertex_arr[18] = vertex_arr[17] - pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_y;
		vertex_arr[19] = vertex_arr[16] - pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_y;
		//转换到绘图坐标系下
		for(i=0;i<20;i++)
		{
			coord_trans(vertex_arr[i], ucs, FALSE);//转换到绘图坐标系下
			vertex_arr[i]/=scale;//调配绘图比例		
			vertex_arr[i].z=0;
		}
		//绘制槽型横担端面图
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1],pSlotArm->handle);
		for(int i=1;i<3;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[3]);
		for(int i=4;i<7;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[7]);
		for(int i=8;i<11;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[8],vertex_arr[11]);
		for(int i=12;i<15;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[12],vertex_arr[15]);
		for(int i=16;i<19;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[16],vertex_arr[19]);
		//标注
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		sDimText.Printf("%.f",pSlotArm->GetCoverPlateWidth());
		dim_pos=0.5*(vertex_arr[7]+vertex_arr[6]);
		dim_pos.y=vertex_arr[7].y-2*sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[7].x,vertex_arr[7].y);
		dim_end.Set(vertex_arr[6].x,vertex_arr[6].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetCoverPlateThick());
		dim_pos=0.5*(vertex_arr[4]+vertex_arr[4]);
		dim_pos.x=vertex_arr[4].x+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[7].x,vertex_arr[7].y);
		dim_end.Set(vertex_arr[4].x,vertex_arr[4].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0.5*Pi);
		sDimText.Printf("%.f",pSlotArm->GetRootWidth());
		dim_pos=0.5*(vertex_arr[11]+vertex_arr[15]);
		dim_pos.y=vertex_arr[7].y-sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[11].x,vertex_arr[7].y);
		dim_end.Set(vertex_arr[15].x,vertex_arr[7].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetSidePlateThick());
		dim_pos=0.5*(vertex_arr[8]+vertex_arr[9]);
		dim_pos.y=vertex_arr[3].y+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[8].x,vertex_arr[3].y);
		dim_end.Set(vertex_arr[9].x,vertex_arr[3].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetSidePlateThick());
		dim_pos=0.5*(vertex_arr[13]+vertex_arr[12]);
		dim_pos.y=vertex_arr[3].y+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[13].x,vertex_arr[3].y);
		dim_end.Set(vertex_arr[12].x,vertex_arr[3].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetRibPlateThick());
		dim_pos=0.5*(vertex_arr[16]+vertex_arr[19]);
		dim_pos.x=vertex_arr[8].y-sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[8].x,vertex_arr[16].y);
		dim_end.Set(vertex_arr[8].x,vertex_arr[19].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0.5*Pi);
	}
	else if(hOv1==0)//平铺
	{
		vertex_arr[0] = pSlotArm->ucs.origin + 0.5*pSlotArm->GetRootWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[1] = vertex_arr[0] + pSlotArm->GetRootInsertLen()*pSlotArm->ucs.axis_x;
		vertex_arr[2] = vertex_arr[1] - pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_z;
		vertex_arr[3] = vertex_arr[0] - pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_z;
		vertex_arr[4] = vertex_arr[0] - pSlotArm->GetRootWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[5] = vertex_arr[4] + pSlotArm->GetRootInsertLen()*pSlotArm->ucs.axis_x;
		vertex_arr[6] = vertex_arr[5] + pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_z;
		vertex_arr[7] = vertex_arr[4] + pSlotArm->GetSidePlateThick()*pSlotArm->ucs.axis_z;
		vertex_arr[8] = pSlotArm->ucs.origin + pSlotArm->GetRootInsertLen()*pSlotArm->ucs.axis_x + 0.5*pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[9] = pSlotArm->ucs.origin + pSlotArm->GetRailLeverLen()*pSlotArm->ucs.axis_x + 0.5*pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[10] = vertex_arr[9] - pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		vertex_arr[11] = vertex_arr[8] - pSlotArm->GetCoverPlateWidth()*pSlotArm->ucs.axis_z;
		double f = pSlotArm->GetRootInsertLen()- pSlotArm->GetRibPlateLen();
		vertex_arr[12] = vertex_arr[3] + f*pSlotArm->ucs.axis_x;
		vertex_arr[13] = vertex_arr[7] + f*pSlotArm->ucs.axis_x;
		vertex_arr[14] = vertex_arr[0] + pSlotArm->GetRailLeverLen()*pSlotArm->ucs.axis_x;
		vertex_arr[15] = vertex_arr[3] + pSlotArm->GetRailLeverLen()*pSlotArm->ucs.axis_x;
		vertex_arr[16] = vertex_arr[7] + pSlotArm->GetRailLeverLen()*pSlotArm->ucs.axis_x;
		vertex_arr[17] = vertex_arr[4] + pSlotArm->GetRailLeverLen()*pSlotArm->ucs.axis_x;
		//转换到绘图坐标系下
		for(i=0;i<18;i++)
		{
			coord_trans(vertex_arr[i], ucs, FALSE);//转换到绘图坐标系下
			vertex_arr[i]/=scale;//调配绘图比例		
			vertex_arr[i].z=0;
		}
		//绘制槽型横担平铺图
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1],pSlotArm->handle);
		for(int i=1;i<3;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[3]);
		for(int i=4;i<7;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[7]);
		for(int i=8;i<11;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[8],vertex_arr[11]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[12],vertex_arr[2]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[6]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[12],vertex_arr[13]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[6],vertex_arr[13]);

		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[14],pSlotArm->handle);
		CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[15]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[6],vertex_arr[16]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[5],vertex_arr[17]);
		//标注
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		sDimText.Printf("%.f",pSlotArm->GetRootWidth());
		dim_pos=0.5*(vertex_arr[0]+vertex_arr[4]);
		dim_pos.x=vertex_arr[0].x-1.5*sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[0].x,vertex_arr[0].y);
		dim_end.Set(vertex_arr[4].x,vertex_arr[4].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0.5*Pi);
		sDimText.Printf("%.f",pSlotArm->GetSidePlateThick());
		dim_pos=0.5*(vertex_arr[7]+vertex_arr[4]);
		dim_pos.x=vertex_arr[7].x-sys.dim_map.fDimTextSize;
		dim_start.Set(vertex_arr[7].x,vertex_arr[7].y);
		dim_end.Set(vertex_arr[4].x,vertex_arr[4].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0.5*Pi);
		sDimText.Printf("%.f",pSlotArm->GetSidePlateThick());
		dim_pos=0.5*(vertex_arr[3]+vertex_arr[0]);
		dim_pos.x=vertex_arr[7].x-sys.dim_map.fDimTextSize;
		dim_start.Set(vertex_arr[3].x,vertex_arr[3].y);
		dim_end.Set(vertex_arr[0].x,vertex_arr[0].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0.5*Pi);
		sDimText.Printf("%.f",pSlotArm->GetCoverPlateWidth());
		dim_pos=0.5*(vertex_arr[9]+vertex_arr[10]);
		dim_pos.x=vertex_arr[9].x+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[9].x,vertex_arr[9].y);
		dim_end.Set(vertex_arr[10].x,vertex_arr[10].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0.5*Pi);
		sDimText.Printf("%.f",pSlotArm->GetRailLeverLen());
		dim_pos=0.5*(vertex_arr[4]+vertex_arr[10]);
		dim_pos.y=vertex_arr[4].y+sys.dim_map.fDimTextSize*sDimText.Length();
		dim_start.Set(vertex_arr[4].x,vertex_arr[4].y);
		dim_end.Set(vertex_arr[10].x,vertex_arr[10].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetRootInsertLen());
		dim_pos=0.5*(vertex_arr[4]+vertex_arr[5]);
		dim_pos.y=vertex_arr[4].y+sys.dim_map.fDimTextSize;
		dim_start.Set(vertex_arr[4].x,vertex_arr[4].y);
		dim_end.Set(vertex_arr[5].x,vertex_arr[5].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
		sDimText.Printf("%.f",pSlotArm->GetRibPlateLen());
		dim_pos=0.5*(vertex_arr[12]+vertex_arr[2]);
		dim_pos.y=vertex_arr[1].y-sys.dim_map.fDimTextSize;
		dim_start.Set(vertex_arr[12].x,vertex_arr[1].y);
		dim_end.Set(vertex_arr[2].x,vertex_arr[1].y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);

	}
}
#endif
void GetParaPlateSectionProfile(CLDSParamPlate *pPlank,UCS_STRU dcs,double draw_scale,fPtList &vertexList,BOOL &bVisible)
{
	vertexList.Empty();
	if(pPlank==NULL)
		return;
	bVisible=FALSE;
	double plate_thick=__max(pPlank->GetThick(),sys.stru_map.fMinPartThick);	//绘制钢板截面时启用最小绘制厚度
	//绘图面与钢板当前面求交线
	UCS_STRU prj_ucs;
	f3dPoint inters_pt,inters_vec;
	f3dLine inters_line;
	if(Int3d2f(&inters_pt,&inters_vec,dcs.origin,dcs.axis_z,pPlank->ucs.origin,pPlank->ucs.axis_z)==1)
	{	//坐标系原点向交线投影
		inters_line.startPt=inters_pt+inters_vec*100000;
		inters_line.endPt=inters_pt-inters_vec*100000;
		SnapPerp(&prj_ucs.origin,inters_line,pPlank->ucs.origin);
	}
	prj_ucs.axis_z=dcs.axis_z;
	prj_ucs.axis_x=inters_vec;	//X轴方向为两面交线方向
	normalize(prj_ucs.axis_x);
	prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
	normalize(prj_ucs.axis_y);
	//计算外形顶点到绘图面的投影点
	ATOM_LIST<f3dPoint> ptList;
	f3dPoint vertex;
	for(PROFILE_VERTEX* pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
	{
		vertex=pVertex->vertex;
		coord_trans(vertex,pPlank->ucs,TRUE);
		coord_trans(vertex,prj_ucs,FALSE);
		if(vertex.z>0)
			bVisible=TRUE;
		coord_trans(vertex,prj_ucs,TRUE);
		if(Int3dlf(inters_pt,vertex,dcs.axis_z,dcs.origin,dcs.axis_z)>0)
		{
			SnapPerp(&vertex,inters_line,inters_pt);
			ptList.append(vertex);
		}
	}
	// 提取有效交点
	f3dLine face_line;
	for(int i=0;i<ptList.GetNodeNum();i++)
	{
		if(i==0)
			face_line.startPt=ptList[i];
		else if(i==1)
			face_line.endPt=ptList[i];
		else 
		{
			int judge=face_line.PtInLine(ptList[i]);
			if(judge==-1)		//在线段终端外侧
				face_line.endPt=ptList[i];
			else if(judge==-2)	//在线段始端外侧
				face_line.startPt=ptList[i];
		}
	}
	if(face_line.startPt.IsEqual(face_line.endPt))
		return;
	vertex=face_line.startPt;
	vertexList.append(vertex);
	vertex+=pPlank->ucs.axis_z*plate_thick;
	vertexList.append(vertex);
	vertex=face_line.endPt+pPlank->ucs.axis_z*plate_thick;
	vertexList.append(vertex);
	vertex=face_line.endPt;
	vertexList.append(vertex);
	// 将截面轮廓点转换到绘图坐标系下并按比例缩放
	for(f3dPoint *pPt=vertexList.GetFirst();pPt;pPt=vertexList.GetNext())
	{
		coord_trans(*pPt,dcs,FALSE);
		*pPt*=draw_scale;
	}
}
//h0v1:0表示平铺着绘，1表示绘制侧面，2或其它表示自动判断绘制方式,offset用于基准点在平板上平面时的情况
void DrawParamPlateStruDrawing(AcDbBlockTableRecord *pBlockTableRecord, CLDSParamPlate *pPlate, 
							   UCS_STRU &ucs,double scale,BOOL bSetupDrawing,AcDbObjectId *pOutEntId=NULL)
{
	SETUPPOSDIM_STRU posDim;
	AcDbObjectId layerId, pointId;
	AcDbObjectId outPutEntId=0;	//关联构件句柄的实体ID wht 10-12-02
	f3dLine line;	//临时绘制线
	f3dPoint point,start_firs,end_firs,start_sec,end_sec,start_thir,end_thir,start_four,
		end_four,start_fif,end_fif,start_six,end_six,len_vec;
	f3dPoint start_seven,end_seven,start_eight,end_eight;//用来画卷边板的卷边
	double A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,L=0,M=0,N=0,S=0,R=0,W=0,H1=0,K=0,O=0;
	PROFILE_VERTEX *pVertex,*pPrevVertex=NULL;
	if(pPlate->vertex_list.GetNodeNum()==0)
		pPlate->DesignPlate();
	if(!pPlate->m_bStdPart&&pPlate->vertex_list.GetNodeNum()==0)
		return;	//数据有误没有外形顶点数据
	int i,h0v1=-1;
	if(pPlate->m_iParamType!=TYPE_ELBOWPLATE)
	{	//未指定绘制方式的非肘板参数化板，提前自动判断绘制方式
		if((pPlate->m_iParamType==TYPE_UEND||pPlate->m_iParamType==TYPE_ROLLEND)
			&&fabs(ucs.axis_z*pPlate->ucs.axis_x)>EPS_COS2)
			h0v1=2;		//槽型插板或者U型插板需要绘制端面图即竖直绘制
		else if(fabs(ucs.axis_z*pPlate->ucs.axis_z)>0.707)
			h0v1=0;	//平铺绘制
		else
			h0v1=1;	//侧向绘制
	}
	double plate_thick=pPlate->GetThick();
	if(!bSetupDrawing&&h0v1==1&&plate_thick<sys.stru_map.fMinPartThick)
		plate_thick=sys.stru_map.fMinPartThick;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
	if(pPlate->m_iParamType==TYPE_UEND||pPlate->m_iParamType==TYPE_ROLLEND||pPlate->m_iParamType==TYPE_XEND)
	{
		//U型板
		//5________________4 
		//|　		        \3
		//|					|
		//|-.—mid_line—.—|	mark_point_arr[0]:插板原点mark_point_arr[1]:最外侧螺栓孔
		//|                 |2	
		//0				   1/ 
		// ￣￣￣￣￣￣￣￣   
		f3dPoint vertex_arr[6],mark_point_arr[2];
		f3dLine up_roll_edge,down_roll_edge;
		pPlate->GetDesignItemValue('L', &L);
		pPlate->GetDesignItemValue('N', &N);
		pPlate->GetDesignItemValue('W', &W);
		pPlate->GetDesignItemValue('S', &S);
		pPlate->GetDesignItemValue('H', &H);
		if(h0v1==0)	//水平绘制
		{
			double fRealN=N;
			BYTE cRollType=pPlate->m_cRollProcessType;
			if(pPlate->m_iParamType==TYPE_XEND)
				fRealN=N=S=0;	//十字插板
			else if(pPlate->m_iParamType==TYPE_ROLLEND&&cRollType==1&&(S+W)>0)
			{	//最少刀数1 保证卷边外侧边长度为L-N wht 18-08-16
				double fInnerOffsetLen=(N-(S*N)/(S+W));
				fRealN=N-fInnerOffsetLen;
			}
			vertex_arr[0]= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_y;
			vertex_arr[1]= vertex_arr[0] + (L-fRealN)*pPlate->ucs.axis_x;
			vertex_arr[2]= vertex_arr[0] + L*pPlate->ucs.axis_x + S*pPlate->ucs.axis_y;
			vertex_arr[3]= vertex_arr[0] + L*pPlate->ucs.axis_x + (H-S)*pPlate->ucs.axis_y;
			vertex_arr[4]= vertex_arr[0] + (L-fRealN)*pPlate->ucs.axis_x + H*pPlate->ucs.axis_y;
			vertex_arr[5]= vertex_arr[0] + H*pPlate->ucs.axis_y;
			if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				up_roll_edge.startPt  = vertex_arr[5] - plate_thick*pPlate->ucs.axis_y;
				up_roll_edge.endPt    = vertex_arr[4] - plate_thick*pPlate->ucs.axis_y;
				down_roll_edge.startPt= vertex_arr[0] + plate_thick*pPlate->ucs.axis_y;
				down_roll_edge.endPt  = vertex_arr[1] + plate_thick*pPlate->ucs.axis_y;
			}
			//计算插板的三个定位标记点
			mark_point_arr[0]=pPlate->ucs.origin;
			mark_point_arr[1].x=0;
			for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				f3dPoint bolt_pos=pLsRef->GetLsPtr()->ucs.origin;
				Int3dlf(bolt_pos,bolt_pos,pLsRef->GetLsPtr()->ucs.axis_z,pPlate->ucs.origin,pPlate->ucs.axis_z);
				coord_trans(bolt_pos,pPlate->ucs,FALSE);
				if(bolt_pos.x>mark_point_arr[1].x)
					mark_point_arr[1].x=bolt_pos.x;
			}
			//转换到绘图坐标系下
			for(i=0;i<6;i++)
			{
				coord_trans(vertex_arr[i], ucs, FALSE);
				vertex_arr[i]/=scale;//调配绘图比例		
				vertex_arr[i].z=0;
			}
			if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				coord_trans(up_roll_edge.startPt, ucs, FALSE);
				coord_trans(up_roll_edge.endPt, ucs, FALSE);
				coord_trans(down_roll_edge.startPt, ucs, FALSE);
				coord_trans(down_roll_edge.endPt, ucs, FALSE);
				up_roll_edge.startPt/=scale;	//调配绘图比例
				up_roll_edge.endPt/=scale;		//调配绘图比例
				down_roll_edge.startPt/=scale;	//调配绘图比例
				down_roll_edge.endPt/=scale;	//调配绘图比例
				up_roll_edge.startPt.z=up_roll_edge.endPt.z=down_roll_edge.startPt.z=down_roll_edge.endPt.z=0; 
			}
			
			//处理消隐
			fPtList vertexlist;
			for(i=0;i<6;i++)
				vertexlist.append(vertex_arr[i]);
			ProcessHideLine(pBlockTableRecord,vertexlist,LayerTable::UnvisibleProfileLayer.layerName);
			//绘制U型板
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			if(!sys.stru_map.bUsePolyLineDrawEdge)
			{
				outPutEntId=CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1],pPlate->handle);
				CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[2]);
				CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[3]);
				CreateAcadLine(pBlockTableRecord,vertex_arr[3],vertex_arr[4]);
				CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[5]);
				CreateAcadLine(pBlockTableRecord,vertex_arr[5],vertex_arr[0]);
			}
			else
			{
				outPutEntId=CreateAcadPolyLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1],
											   sys.stru_map.fPolyLineWidth,pPlate->handle);
				CreateAcadPolyLine(pBlockTableRecord,vertex_arr[1],vertex_arr[2],sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,vertex_arr[2],vertex_arr[3],sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,vertex_arr[3],vertex_arr[4],sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,vertex_arr[4],vertex_arr[5],sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,vertex_arr[5],vertex_arr[0],sys.stru_map.fPolyLineWidth);
			}
			
			//插入U型或槽型插板的三个定位标记点
			if(pDatumTube)
			{
				coord_trans(mark_point_arr[1], pPlate->ucs, TRUE);	//转换到全塔坐标系下以便计算轴向位置
				for(i=0;i<2;i++)
				{
					posDim.bDisplayDimLine=TRUE;
					f3dPoint pos_vec=mark_point_arr[i]-pDatumTube->Start();
					f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
					normalize(datum_line_vec);
					posDim.posDist=pos_vec*datum_line_vec;
					posDim.dimPosOffset=1.0;
					coord_trans(mark_point_arr[i], ucs, FALSE);	//转换到绘图坐标系下
					mark_point_arr[i]/=scale;		//调配绘图比例	
					mark_point_arr[i].z=0;
					AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(mark_point_arr[i].x,mark_point_arr[i].y,0));
					if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
						pPoint->close();
					posDim.posPointId=pointId;
					DRAGSET.dimPosList.append(posDim);
				}
			}
			
			if(pPlate->m_iParamType==TYPE_ROLLEND)
			{	//绘制卷边板的卷边
				if(pPlate->ucs.axis_z*ucs.axis_z>0)
					GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
				else
					GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				CreateAcadLine(pBlockTableRecord,up_roll_edge.startPt,up_roll_edge.endPt);
				CreateAcadLine(pBlockTableRecord,down_roll_edge.startPt,down_roll_edge.endPt);
			}
		}
		else if(h0v1==1)	//绘制侧面
		{
			if(pPlate->m_iParamType==TYPE_UEND)
			{
				point		= pPlate->ucs.origin + (0.5*W+plate_thick)*pPlate->ucs.axis_x;
				start_firs	= point + 0.5*W*pPlate->ucs.axis_z;
				end_firs	= pPlate->ucs.origin + L*pPlate->ucs.axis_x + 0.5*W*pPlate->ucs.axis_z;	
				start_sec	= point - 0.5*W*pPlate->ucs.axis_z;
				end_sec		= pPlate->ucs.origin + L*pPlate->ucs.axis_x - 0.5*W*pPlate->ucs.axis_z;
				start_thir	= start_firs + plate_thick*pPlate->ucs.axis_z;
				end_thir	= end_firs + plate_thick*pPlate->ucs.axis_z;
				start_four	= start_sec - plate_thick*pPlate->ucs.axis_z;
				end_four	= end_sec - plate_thick*pPlate->ucs.axis_z;
				//转换到绘图坐标系下
				coord_trans(point, ucs, FALSE);
				coord_trans(start_firs, ucs, FALSE);
				coord_trans(end_firs, ucs, FALSE);
				coord_trans(start_sec, ucs, FALSE);
				coord_trans(end_sec, ucs, FALSE);
				coord_trans(start_thir, ucs, FALSE);
				coord_trans(end_thir, ucs, FALSE);
				coord_trans(start_four, ucs, FALSE);
				coord_trans(end_four, ucs, FALSE);
				point.z=start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=0;
				//调配绘图比例
				point = point*(1.0/scale);
				start_firs = start_firs*(1.0/scale);
				end_firs = end_firs*(1.0/scale);
				start_sec = start_sec*(1.0/scale);
				end_sec = end_sec*(1.0/scale);
				start_thir = start_thir*(1.0/scale);
				end_thir = end_thir*(1.0/scale);
				start_four = start_four*(1.0/scale);
				end_four = end_four*(1.0/scale);
				//绘制U型板侧面图
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_thir,end_thir);
				CreateAcadLine(pBlockTableRecord,start_four,end_four);
				CreateAcadLine(pBlockTableRecord,end_firs,end_thir);
				CreateAcadLine(pBlockTableRecord,end_sec,end_four);
				CreateAcadCircle(pBlockTableRecord,point,0.5*W);
				CreateAcadCircle(pBlockTableRecord,point,0.5*W+plate_thick);
			}
			else if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				start_firs	= pPlate->ucs.origin + 0.5*plate_thick*pPlate->ucs.axis_z;
				end_firs	= start_firs + L*pPlate->ucs.axis_x;	
				start_sec	= start_firs - (W+plate_thick)*pPlate->ucs.axis_z;
				end_sec		= start_sec + (L-N)*pPlate->ucs.axis_x;
				start_thir	= end_sec + W*pPlate->ucs.axis_z;
				end_thir	= start_thir + N*pPlate->ucs.axis_x;
				//转换到绘图坐标系下
				coord_trans(start_firs, ucs, FALSE);
				coord_trans(end_firs, ucs, FALSE);
				coord_trans(start_sec, ucs, FALSE);
				coord_trans(end_sec, ucs, FALSE);
				coord_trans(start_thir, ucs, FALSE);
				coord_trans(end_thir, ucs, FALSE);
				start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=0;
				//调配绘图比例
				start_firs = start_firs*(1.0/scale);
				end_firs = end_firs*(1.0/scale);
				start_sec = start_sec*(1.0/scale);
				end_sec = end_sec*(1.0/scale);
				start_thir = start_thir*(1.0/scale);
				end_thir = end_thir*(1.0/scale);
				//绘制卷边板侧面图
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_thir,end_thir);
				CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
				CreateAcadLine(pBlockTableRecord,end_sec,start_thir);
				CreateAcadLine(pBlockTableRecord,end_thir,end_firs);
			}
			else if(pPlate->m_iParamType==TYPE_XEND)
			{
				start_firs	= pPlate->ucs.origin - 0.5*plate_thick*pPlate->ucs.axis_z;
				end_firs	= start_firs + L*pPlate->ucs.axis_x;	
				start_sec	= start_firs + plate_thick*pPlate->ucs.axis_z;
				end_sec		= start_sec + L*pPlate->ucs.axis_x;
				//转换到绘图坐标系下
				coord_trans(start_firs, ucs, FALSE);
				coord_trans(end_firs, ucs, FALSE);
				coord_trans(start_sec, ucs, FALSE);
				coord_trans(end_sec, ucs, FALSE);
				start_firs.z=end_firs.z=start_sec.z=end_sec.z=0;
				//调配绘图比例
				start_firs = start_firs*(1.0/scale);
				end_firs = end_firs*(1.0/scale);
				start_sec = start_sec*(1.0/scale);
				end_sec = end_sec*(1.0/scale);
				//绘制卷边板侧面图
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
				CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
			}
		}
		else if(h0v1==2)
		{	//绘制端面
			if(pPlate->m_iParamType==TYPE_UEND)
			{
				point		= pPlate->ucs.origin + (0.5*W+plate_thick)*pPlate->ucs.axis_x;
				start_firs	= point + 0.5*W*pPlate->ucs.axis_z;
				end_firs	= pPlate->ucs.origin + L*pPlate->ucs.axis_x + 0.5*W*pPlate->ucs.axis_z;	
				start_sec	= point - 0.5*W*pPlate->ucs.axis_z;
				end_sec		= pPlate->ucs.origin + L*pPlate->ucs.axis_x - 0.5*W*pPlate->ucs.axis_z;
				start_thir	= start_firs + plate_thick*pPlate->ucs.axis_z;
				end_thir	= end_firs + plate_thick*pPlate->ucs.axis_z;
				start_four	= start_sec - plate_thick*pPlate->ucs.axis_z;
				end_four	= end_sec - plate_thick*pPlate->ucs.axis_z;
				//转换到绘图坐标系下
				coord_trans(point, ucs, FALSE);
				coord_trans(start_firs, ucs, FALSE);
				coord_trans(end_firs, ucs, FALSE);
				coord_trans(start_sec, ucs, FALSE);
				coord_trans(end_sec, ucs, FALSE);
				coord_trans(start_thir, ucs, FALSE);
				coord_trans(end_thir, ucs, FALSE);
				coord_trans(start_four, ucs, FALSE);
				coord_trans(end_four, ucs, FALSE);
				point.z=start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=0;
				//调配绘图比例
				point = point*(1.0/scale);
				start_firs = start_firs*(1.0/scale);
				end_firs = end_firs*(1.0/scale);
				start_sec = start_sec*(1.0/scale);
				end_sec = end_sec*(1.0/scale);
				start_thir = start_thir*(1.0/scale);
				end_thir = end_thir*(1.0/scale);
				start_four = start_four*(1.0/scale);
				end_four = end_four*(1.0/scale);
				//绘制U型板端面图
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_thir,end_thir);
				CreateAcadLine(pBlockTableRecord,start_four,end_four);
				CreateAcadLine(pBlockTableRecord,end_firs,end_thir);
				CreateAcadLine(pBlockTableRecord,end_sec,end_four);
				CreateAcadCircle(pBlockTableRecord,point,0.5*W);
				CreateAcadCircle(pBlockTableRecord,point,0.5*W+plate_thick);
			}
			else if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				start_firs	= pPlate->ucs.origin + 0.5*plate_thick*pPlate->ucs.axis_z + 0.5*H*pPlate->ucs.axis_y;
				end_firs	= start_firs -  H*pPlate->ucs.axis_y;
				start_sec	= pPlate->ucs.origin - 0.5*plate_thick*pPlate->ucs.axis_z + (0.5*H-plate_thick)*pPlate->ucs.axis_y;
				end_sec		= start_sec - (H-2*plate_thick)*pPlate->ucs.axis_y;
				start_thir	= start_firs - W*pPlate->ucs.axis_z;
				end_thir	= start_sec - (W-plate_thick)*pPlate->ucs.axis_z;
				start_four	= end_firs - W*pPlate->ucs.axis_z;
				end_four	= end_sec - (W-plate_thick)*pPlate->ucs.axis_z;
				//转换到绘图坐标系下
				coord_trans(start_firs, ucs, FALSE);
				coord_trans(end_firs, ucs, FALSE);
				coord_trans(start_sec, ucs, FALSE);
				coord_trans(end_sec, ucs, FALSE);
				coord_trans(start_thir, ucs, FALSE);
				coord_trans(end_thir, ucs, FALSE);
				coord_trans(start_four, ucs, FALSE);
				coord_trans(end_four, ucs, FALSE);
				start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=0;
				//调配绘图比例
				start_firs = start_firs*(1.0/scale);
				end_firs = end_firs*(1.0/scale);
				start_sec = start_sec*(1.0/scale);
				end_sec = end_sec*(1.0/scale);
				start_thir = start_thir*(1.0/scale);
				end_thir = end_thir*(1.0/scale);
				start_four = start_four*(1.0/scale);
				end_four = end_four*(1.0/scale);
				//绘制卷边板端面图
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_thir,end_thir);
				CreateAcadLine(pBlockTableRecord,start_four,end_four);
				CreateAcadLine(pBlockTableRecord,start_firs,start_thir);
				CreateAcadLine(pBlockTableRecord,end_firs,start_four);
				CreateAcadLine(pBlockTableRecord,start_sec,end_thir);
				CreateAcadLine(pBlockTableRecord,end_sec,end_four);
			}
			else if(pPlate->m_iParamType==TYPE_XEND)
			{
				start_firs	= pPlate->ucs.origin + 0.5*plate_thick*pPlate->ucs.axis_z + 0.5*H*pPlate->ucs.axis_y;
				end_firs	= start_firs -  H*pPlate->ucs.axis_y;
				start_sec	= pPlate->ucs.origin - 0.5*plate_thick*pPlate->ucs.axis_z + 0.5*H*pPlate->ucs.axis_y;
				end_sec		= start_sec - H*pPlate->ucs.axis_y;
				//转换到绘图坐标系下
				coord_trans(start_firs, ucs, FALSE);
				coord_trans(end_firs, ucs, FALSE);
				coord_trans(start_sec, ucs, FALSE);
				coord_trans(end_sec, ucs, FALSE);
				start_firs.z=end_firs.z=start_sec.z=end_sec.z=0;
				//调配绘图比例
				start_firs = start_firs*(1.0/scale);
				end_firs = end_firs*(1.0/scale);
				start_sec = start_sec*(1.0/scale);
				end_sec = end_sec*(1.0/scale);
				//绘制十字插板基板端面图
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
				CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
			}
		}
	}
	else if(pPlate->m_iParamType==TYPE_FL||
		pPlate->m_iParamType==TYPE_FLG||
		pPlate->m_iParamType==TYPE_FLR)
	{
		pPlate->GetDesignItemValue('R', &R);
		pPlate->GetDesignItemValue('W', &W);
		if(h0v1==1)	//绘制侧面图
		{	//法兰侧面图
			//应保证pre_vec同时垂直于法兰Z轴方向以及绘图坐标系Z轴方向	wht 09-12-08
			//RotateVectorAroundVector(pre_vec,-Pi*0.5,ucs.axis_z);//f3dPoint(0,1,0)); //绕Z轴旋转不能保证垂直于Z轴
			f3dPoint pre_vec=pPlate->ucs.axis_z^ucs.axis_z;
			normalize(pre_vec);
			start_firs	= pPlate->ucs.origin + 0.5*W*pre_vec;
			end_firs	= pPlate->ucs.origin - 0.5*W*pre_vec;
			start_sec	= start_firs + plate_thick*pPlate->ucs.axis_z;
			end_sec		= end_firs + plate_thick*pPlate->ucs.axis_z;
			//转换到绘图坐标系下
			coord_trans(start_firs, ucs, FALSE);
			coord_trans(end_firs, ucs, FALSE);
			coord_trans(start_sec, ucs, FALSE);
			coord_trans(end_sec, ucs, FALSE);
			start_firs.z=end_firs.z=start_sec.z=end_sec.z=0;
			//调整绘图比例
			start_firs /= scale;
			end_firs   /= scale;
			start_sec  /= scale;
			end_sec    /= scale;
			//绘制法兰截面图
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			if(!sys.stru_map.bUsePolyLineDrawEdge)
			{
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
				CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
			}
			else
			{
				outPutEntId=CreateAcadPolyLine(pBlockTableRecord,start_firs,end_firs,
											   sys.stru_map.fPolyLineWidth,pPlate->handle);
				CreateAcadPolyLine(pBlockTableRecord,start_sec,end_sec,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,start_firs,start_sec,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,end_firs,end_sec,sys.stru_map.fPolyLineWidth);
			}
			//添加法兰外壁定位标记点
			if(pDatumTube)
			{
				f3dPoint mark_point,datum_line_vec=pDatumTube->End()-pDatumTube->Start();
				normalize(datum_line_vec);
				mark_point=pPlate->ucs.origin;
				if( (pDatumTube->m_tJointStart.hLinkObj==pPlate->handle&&datum_line_vec*pPlate->ucs.axis_z<0)||
					(pDatumTube->m_tJointEnd.hLinkObj==pPlate->handle&&datum_line_vec*pPlate->ucs.axis_z>0))
					mark_point=pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->thick;
				posDim.posDist=(mark_point-pDatumTube->Start())*datum_line_vec;
				coord_trans(mark_point,ucs,FALSE);
				mark_point.z=0;
				posDim.bDisplayDimLine=TRUE;
				posDim.dimPosOffset=1;
				mark_point/=scale;
				AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(mark_point.x,mark_point.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				posDim.posPointId=pointId;
				DRAGSET.dimPosList.append(posDim);
			}
		}
		else if(h0v1==0)	//水平绘制
		{	
			point = pPlate->ucs.origin;
			//转换到绘图坐标系下
			coord_trans(point, ucs, FALSE);
			point.z = 0;
			//调整绘图比例
			point = point*(1.0/scale);
			//绘制法兰截面图
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadCircle(pBlockTableRecord, point,R/scale);
			outPutEntId=CreateAcadCircle(pBlockTableRecord, point,0.5*W/scale,pPlate->handle);
		}
	}
	else if(pPlate->m_iParamType==TYPE_FLP||pPlate->m_iParamType==TYPE_FLD)
	{	//标准构件 平焊法兰/对焊法兰
		//	1----------0----------1
		//	|		   			  |
		//	2____             ____2
		//		3\	   	     /3
		//		  |5   	   5|			
		//		  4---------4 
		CLDSStdPartModel stdPartModel;
		if(!pPlate->GetStdPartModel(&stdPartModel))
			return;
		B=stdPartModel.param.fl_param.B;
		C=stdPartModel.param.fl_param.C;
		D=stdPartModel.param.fl_param.D;
		H=stdPartModel.param.fl_param.H;
		H1=stdPartModel.param.fl_param.H1;
		if(pPlate->m_iParamType==TYPE_FLP)
			pPlate->GetDesignItemValue('O',&H1);
		N=stdPartModel.param.fl_param.N;
		R=stdPartModel.param.fl_param.R;
		S=stdPartModel.param.fl_param.S;
		if(pPlate->m_iParamType==TYPE_FLD)
			S+=pDatumTube->GetThick();	//对焊法兰S=S+钢管厚度 wht 11-07-08
		if(h0v1==1)	//绘制侧面图
		{	//法兰侧面图
			//应保证pre_vec同时垂直于法兰Z轴方向以及绘图坐标系Z轴方向	wht 09-12-08
			//RotateVectorAroundVector(pre_vec,-Pi*0.5,ucs.axis_z);//f3dPoint(0,1,0)); //绕Z轴旋转不能保证垂直于Z轴
			f3dPoint pre_vec=pPlate->ucs.axis_z^ucs.axis_z;
			normalize(pre_vec);
			start_firs	= pPlate->ucs.origin + 0.5*D*pre_vec;
			end_firs	= pPlate->ucs.origin - 0.5*D*pre_vec;
			start_sec	= start_firs + C*pPlate->ucs.axis_z;
			end_sec		= end_firs + C*pPlate->ucs.axis_z;
			start_thir	= start_sec - 0.5*(D-N)*pre_vec;
			end_thir	= end_sec + 0.5*(D-N)*pre_vec;
			start_four	= pPlate->ucs.origin + (0.5*B+S)*pre_vec + H*pPlate->ucs.axis_z;
			end_four	= pPlate->ucs.origin - (0.5*B+S)*pre_vec + H*pPlate->ucs.axis_z;
			double H2=H1;	//对焊法兰
			if(pPlate->m_iParamType==TYPE_FLP)
				H2=H-(C+R);	//平焊法兰
			start_fif	= start_four - H2*pPlate->ucs.axis_z;
			end_fif		= end_four - H2*pPlate->ucs.axis_z;
			//转换到绘图坐标系下
			coord_trans(start_firs, ucs, FALSE);
			coord_trans(end_firs, ucs, FALSE);
			coord_trans(start_sec, ucs, FALSE);
			coord_trans(end_sec, ucs, FALSE);
			coord_trans(start_thir, ucs, FALSE);
			coord_trans(end_thir, ucs, FALSE);
			coord_trans(start_four, ucs, FALSE);
			coord_trans(end_four, ucs, FALSE);
			coord_trans(start_fif, ucs, FALSE);
			coord_trans(end_fif, ucs, FALSE);
			start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=start_fif.z=end_fif.z=0;
			//调整绘图比例
			start_firs	/= scale;
			end_firs	/= scale;
			start_sec	/= scale;
			end_sec		/= scale;
			start_thir	/= scale;
			end_thir	/= scale;
			start_four	/= scale;
			end_four	/= scale;
			start_fif	/= scale;
			end_fif		/= scale;
			//绘制法兰截面图
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			if(!sys.stru_map.bUsePolyLineDrawEdge)
			{
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
				CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
				CreateAcadLine(pBlockTableRecord,start_sec,start_thir);
				CreateAcadLine(pBlockTableRecord,end_sec,end_thir);
				CreateAcadLine(pBlockTableRecord,start_thir,start_fif);
				CreateAcadLine(pBlockTableRecord,end_thir,end_fif);
				CreateAcadLine(pBlockTableRecord,start_fif,start_four);
				CreateAcadLine(pBlockTableRecord,end_fif,end_four);
				CreateAcadLine(pBlockTableRecord,start_four,end_four);
			}
			else
			{
				outPutEntId=CreateAcadPolyLine(pBlockTableRecord,start_firs,end_firs,
											   sys.stru_map.fPolyLineWidth,pPlate->handle);
				CreateAcadPolyLine(pBlockTableRecord,start_firs,start_sec,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,end_firs,end_sec,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,start_sec,start_thir,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,end_sec,end_thir,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,start_thir,start_fif,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,end_thir,end_fif,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,start_fif,start_four,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,end_fif,end_four,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,start_four,end_four,sys.stru_map.fPolyLineWidth);
			}
			//添加法兰外壁定位标记点
			if(pDatumTube)
			{
				f3dPoint mark_point,datum_line_vec=pDatumTube->End()-pDatumTube->Start();
				normalize(datum_line_vec);
				mark_point=pPlate->ucs.origin;
				if( (pDatumTube->m_tJointStart.hLinkObj==pPlate->handle&&datum_line_vec*pPlate->ucs.axis_z<0)||
					(pDatumTube->m_tJointEnd.hLinkObj==pPlate->handle&&datum_line_vec*pPlate->ucs.axis_z>0))
					mark_point=pPlate->ucs.origin+pPlate->ucs.axis_z*H;
				posDim.posDist=(mark_point-pDatumTube->Start())*datum_line_vec;
				coord_trans(mark_point,ucs,FALSE);
				mark_point.z=0;
				posDim.bDisplayDimLine=TRUE;
				posDim.dimPosOffset=1;
				mark_point/=scale;
				AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(mark_point.x,mark_point.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				posDim.posPointId=pointId;
				DRAGSET.dimPosList.append(posDim);
			}
		}
		else if(h0v1==0)	//水平绘制
		{	
			point = pPlate->ucs.origin;
			//转换到绘图坐标系下
			coord_trans(point, ucs, FALSE);
			point.z = 0;
			//调整绘图比例
			point = point*(1.0/scale);
			//绘制法兰截面图
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadCircle(pBlockTableRecord, point,0.5*B/scale);
			outPutEntId=CreateAcadCircle(pBlockTableRecord, point,0.5*D/scale,pPlate->handle);
		}
	}
	else if(pPlate->m_iParamType==TYPE_RIBPLATE)
	{	//绘制肋板结构图 wht 11-05-26
		UCS_STRU prj_ucs;
		f3dLine base_face_line;
		f3dPoint f_start,f_end;
		fPtList vertexlist;	//用于消隐计算的顶点链表
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		int n=pPlate->vertex_list.GetNodeNum();
		if(h0v1==0)
		{	//水平绘制时需要消隐
			fPtList vertexlist;
			for(i=0;i<n;i++)
			{
				f_start = pPlate->vertex_list [i].vertex;
				coord_trans(f_start,pPlate->ucs,TRUE);
				coord_trans(f_start,ucs,FALSE);
				f_start = f_start*(1.0/scale);
				f_start.z = 0;
				vertexlist.append(f_start);
			}
			if(vertexlist.GetNodeNum()>0)	//处理板消隐
			{
				ProcessHideLine(pBlockTableRecord,vertexlist,LayerTable::UnvisibleProfileLayer.layerName);
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			}
		}
		else if(h0v1==1)
		{	//计算板投影坐标系
			prj_ucs.origin=pPlate->ucs.origin+pPlate->m_fNormOffset*pPlate->ucs.axis_z;
			prj_ucs.axis_y=pPlate->ucs.axis_z;
			prj_ucs.axis_z=ucs.axis_z;
			prj_ucs.axis_x=prj_ucs.axis_y^prj_ucs.axis_z;
			prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
			normalize(prj_ucs.axis_x);
			normalize(prj_ucs.axis_y);
		}
		double sum_len=0;
		f3dPoint geom_center;
		for(i=0;i<n;i++)
		{
			PROFILE_VERTEX *pStartVertex=pPlate->vertex_list.GetByIndex(i);
			PROFILE_VERTEX *pEndVertex=pPlate->vertex_list.GetByIndex((i+1)%n);
			f_start = pStartVertex->vertex;
			f_end   = pEndVertex->vertex;
			double len=DISTANCE(f_start,f_end);
			geom_center =geom_center+len*(f_start+f_end);
			sum_len+=len;
			coord_trans(f_start,pPlate->ucs,TRUE);
			coord_trans(f_end,pPlate->ucs,TRUE);
			if(h0v1==0)
			{	//水平绘制
				coord_trans(f_start,ucs,FALSE);
				coord_trans(f_end,ucs,FALSE);
				f_start = f_start/scale;
				f_end   = f_end/scale;
				f_start.z = 0;
				f_end.z   = 0;
				
				if(pStartVertex->type==0)
				{
					if(!sys.stru_map.bUsePolyLineDrawEdge)
						CreateAcadLine(pBlockTableRecord,f_start,f_end);
					else
						CreateAcadPolyLine(pBlockTableRecord,f_start,f_end,sys.stru_map.fPolyLineWidth);
					if(pStartVertex->m_bWeldEdge)
						DrawWeldLineSymbol(pBlockTableRecord,f_start,f_end,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				}
				else if(pStartVertex->type<3)	//圆弧
				{
					f3dArcLine arcLine;
					f3dPoint worknorm(0,0,1);
					if(pPlate->ucs.axis_z*ucs.axis_z<0)	//板法线与当前绘图坐标系Z轴方向反向
						worknorm.z=-1;
					double fPara=0;
					if(pStartVertex->type==1)		//指定圆弧夹角
					{
						if(pStartVertex->sector_angle<0)
						{
							fPara=-pStartVertex->sector_angle;
							worknorm.z*=-1;
						}
						else
							fPara=pStartVertex->sector_angle;
						arcLine.CreateMethod2(f_start,f_end,worknorm,fPara);
					}
					else if(pStartVertex->type==2)
					{
						if(pStartVertex->radius<0)
						{
							fPara=-pStartVertex->radius;
							worknorm.z*=-1;
						}
						else
							fPara=pStartVertex->radius;
						f3dPoint center_pick=pStartVertex->center;
						//type==2时圆心拾取点为相对坐标需要先转换为绝对坐标再转换到绘图坐标系下 wht 11-04-28
						coord_trans(center_pick,pPlate->ucs,TRUE);
						coord_trans(center_pick,ucs,FALSE);	
						center_pick.z=0;
						center_pick/=scale;
						arcLine.CreateMethod3(f_start,f_end,worknorm,fPara/scale,center_pick);
					}
					if(arcLine.SectorAngle()<2*Pi)
					{
						if(!sys.stru_map.bUsePolyLineDrawEdge)
							CreateAcadArcLine(pBlockTableRecord,arcLine.Center(),arcLine.Start(),arcLine.SectorAngle(),arcLine.WorkNorm());
						else	//绘制多段线圆弧线
							CreateAcadPolyArcLine(pBlockTableRecord,arcLine,sys.stru_map.fPolyLineWidth);
					}
					else if(i==0)
						CreateAcadCircle(pBlockTableRecord,arcLine.Center(),arcLine.Radius());//,pPlate->handle);
					else
						CreateAcadCircle(pBlockTableRecord,arcLine.Center(),arcLine.Radius());
				}
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			}
			else
			{	//查找竖直绘制基准线
				f3dPoint start=f_start;
				f3dPoint end=f_end;
				coord_trans(start,prj_ucs,FALSE);
				start.y=start.z=0;
				if(base_face_line.startPt==base_face_line.endPt)	//缺省未赋值
				{
					base_face_line.startPt=start;
					coord_trans(end,prj_ucs,FALSE);
					end.y=end.z=0;
					base_face_line.endPt = end;
				}
				else
				{
					int judge=base_face_line.PtInLine(start);
					if(judge==-1)		//终点外侧
						base_face_line.endPt=start;
					else if(judge==-2)	//起点外侧
						base_face_line.startPt=start;
				}
			}
		}
		if(h0v1==1)
		{	//竖直绘制钢板
			f3dPoint base_vec,vec;
			base_vec=prj_ucs.axis_y;
			vector_trans(base_vec,ucs,FALSE);
			normalize(base_vec);
			//
			f3dLine offset_line;
			coord_trans(base_face_line.startPt,prj_ucs,TRUE);
			coord_trans(base_face_line.endPt,prj_ucs,TRUE);
			coord_trans(base_face_line.startPt,ucs,FALSE);
			coord_trans(base_face_line.endPt,ucs,FALSE);
			base_face_line.startPt = base_face_line.startPt*(1.0/scale);
			base_face_line.endPt   = base_face_line.endPt*(1.0/scale);
			base_face_line.startPt.z = 0;
			base_face_line.endPt.z   = 0;
			base_face_line.startPt=base_face_line.startPt-base_vec*0.5*plate_thick/scale;
			base_face_line.endPt=base_face_line.endPt-base_vec*0.5*plate_thick/scale;
			CreateAcadLine(pBlockTableRecord,base_face_line.startPt,base_face_line.endPt);
			//生成偏移矢量和等距偏移线
			offset_line.startPt=base_face_line.startPt+base_vec*plate_thick/scale;
			offset_line.endPt=base_face_line.endPt+base_vec*plate_thick/scale;
			CreateAcadLine(pBlockTableRecord,offset_line.startPt,offset_line.endPt);
			//绘制厚度线
			CreateAcadLine(pBlockTableRecord,offset_line.startPt,base_face_line.startPt);
			CreateAcadLine(pBlockTableRecord,offset_line.endPt,base_face_line.endPt);
		}
		//插入扩展数据点，以存储连接板信息
		if(sum_len<EPS)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0x%X plate{%s} the perimeter of outline edge is zero, plate appearance is illegal!",pPlate->handle,(char*)pPlate->GetPartNo());
#else
			logerr.Log("0x%X钢板{%s}轮廓边周长为0，板外形不合法！",pPlate->handle,(char*)pPlate->GetPartNo());
#endif
			return;
		}
		geom_center/=2*sum_len;
		coord_trans(geom_center,pPlate->ucs,TRUE);
		coord_trans(geom_center,ucs,FALSE);
		geom_center/=scale;	//计算板的外形几何中心点位置
		geom_center.z=0;
		//插入扩展记录 wht 11-05-26
		outPutEntId=InsertExtensionRecord(pBlockTableRecord,geom_center,pPlate->handle,CLS_PLATE);
		if(pOutEntId!=NULL)
			*pOutEntId=outPutEntId;
	}
	else if(pPlate->m_iParamType==TYPE_ELBOWPLATE)
	{
		if(h0v1<0||h0v1>1)
		{
			double dd=fabs(pPlate->ucs.axis_z*ucs.axis_z);
			if(dd<EPS_COS2&&dd>0.707)
				return;	//斜向布置的肘板在结构图中为了图面整洁，暂不显示
			pPlate->GetDesignItemValue('A', &A);
			pPlate->GetDesignItemValue('B', &B);
			pPlate->GetDesignItemValue('C', &C);
			pPlate->GetDesignItemValue('D', &D);
			pPlate->GetDesignItemValue('E', &E);
			pPlate->GetDesignItemValue('F', &F);
			pPlate->GetDesignItemValue('G', &G);
			pPlate->GetDesignItemValue('H', &H);
			long hPlate=0;
			pPlate->GetDesignItemValue('L', &hPlate);
			CLDSPart *pDatumPlate=Ta.FromPartHandle(hPlate,CLS_PLATE,CLS_PARAMPLATE);
			if(dd>EPS_COS2)
			{	//平铺放置
				for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					if(pPrevVertex==NULL)
					{
						pPrevVertex=pVertex;
						line.startPt=pPrevVertex->vertex;
						coord_trans(line.startPt,pPlate->ucs,TRUE);
						coord_trans(line.startPt,ucs,FALSE);
						line.startPt.z=0;
						line.startPt/=scale;
						continue;
					}
					else
					{
						line.endPt=pVertex->vertex;
						coord_trans(line.endPt,pPlate->ucs,TRUE);
						coord_trans(line.endPt,ucs,FALSE);
						line.endPt.z=0;
						line.endPt/=scale;
						CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
						line.startPt=line.endPt;
						pPrevVertex=pVertex;
					}
				}
				if(pPrevVertex)
				{	//最后一条直线边
					pVertex=pPlate->vertex_list.GetFirst();
					line.endPt=pVertex->vertex;
					coord_trans(line.endPt,pPlate->ucs,TRUE);
					coord_trans(line.endPt,ucs,FALSE);
					line.endPt.z=0;
					line.endPt/=scale;
					outPutEntId=CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt,pPlate->handle);
				}
			}
			//只有水平绘制肋板基板时绘制肋板截面图 wht 11-08-03
			else if(dd<0.707&&pDatumPlate&&fabs(pDatumPlate->ucs.axis_z*ucs.axis_z)>0.707)
			{	//垂直绘制肘板
				line.startPt=pPlate->ucs.origin;
				line.endPt=line.startPt+B*pPlate->ucs.axis_x;
				coord_trans(line.startPt,ucs,FALSE);
				coord_trans(line.endPt,ucs,FALSE);
				line.startPt/=scale;
				line.endPt/=scale;
				line.startPt.z=line.endPt.z=0;
				f3dPoint norm_vec=pPlate->ucs.axis_z;
				vector_trans(norm_vec,ucs,FALSE);
				norm_vec.z=0;
				normalize(norm_vec);
				double draw_thick=pPlate->thick/scale;
				line.startPt+=norm_vec*(0.5*draw_thick);
				line.endPt+=norm_vec*(0.5*draw_thick);
				outPutEntId=CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt,pPlate->handle);
				line.startPt-=norm_vec*draw_thick;
				line.endPt-=norm_vec*draw_thick;
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
				CreateAcadLine(pBlockTableRecord,line.startPt,line.startPt+norm_vec*draw_thick);
				CreateAcadLine(pBlockTableRecord,line.endPt,line.endPt+norm_vec*draw_thick);
			}
		}
	}
	else if(pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_CIRCOVERPLATE||
		pPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	//环向肋板和封板
		BOOL bDimHandle=FALSE;
		if(h0v1==0)
		{	//平铺放置
			pPrevVertex=NULL;
			int sign=1;
			if(pPlate->ucs.axis_z*ucs.axis_z>0)
				sign=1;
			else
				sign=-1;
			for(pVertex=pPlate->vertex_list.GetFirst();;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pPrevVertex==NULL)
				{
					pPrevVertex=pVertex;
					line.startPt=pVertex->vertex;
					coord_trans(line.startPt,pPlate->ucs,TRUE);
					coord_trans(line.startPt,ucs,FALSE);
					line.startPt.z=0;
					line.startPt/=scale;
					continue;
				}
				else
				{
					BOOL bFinalEdge=FALSE;
					f3dArcLine arcLine;
					if(pVertex==NULL)
					{
						pVertex=pPlate->vertex_list.GetFirst();
						bFinalEdge=TRUE;
					}
					line.endPt=pVertex->vertex;
					coord_trans(line.endPt,pPlate->ucs,TRUE);
					coord_trans(line.endPt,ucs,FALSE);
					line.endPt.z=0;
					line.endPt/=scale;
					if(pPrevVertex->type<=0)
					{
						if(!sys.stru_map.bUsePolyLineDrawEdge)
						{
							if(!bDimHandle)
								outPutEntId=CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt,pPlate->handle);
							else
								CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
						}
						else
						{
							if(!bDimHandle)
								outPutEntId=CreateAcadPolyLine(pBlockTableRecord,line.startPt,line.endPt,
															   sys.stru_map.fPolyLineWidth,pPlate->handle);
							else
								CreateAcadPolyLine(pBlockTableRecord,line.startPt,line.endPt,sys.stru_map.fPolyLineWidth);
						}
						bDimHandle=TRUE;
					}
					else
					{
						f3dPoint worknorm(0,0,sign);
						double fPara;
						if(pPrevVertex->type==1)	//指定圆弧夹角
						{
							if(pPrevVertex->sector_angle<0)
							{
								fPara=-pPrevVertex->sector_angle;
								worknorm*=-1;
							}
							else
								fPara=pPrevVertex->sector_angle;
							arcLine.CreateMethod2(line.startPt,line.endPt,worknorm,fPara);
						}
						else if(pPrevVertex->type==2)
						{	
							if(pPrevVertex->radius<0)
							{
								fPara=-pPrevVertex->radius;
								worknorm*=-1;
							}
							else
								fPara=pPrevVertex->radius;
							fPara/=scale;
							//拾取点转换到绘图相对坐标系下
							f3dPoint center=pPrevVertex->center;
							coord_trans(center,pPlate->ucs,TRUE);
							coord_trans(center,ucs,FALSE);
							center.z=0;
							center/=scale;
							arcLine.CreateMethod3(line.startPt,line.endPt,worknorm,fPara,center);
							//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
							//arcLine.CreateMethod2(line.startPt,line.endPt,worknorm,sec_angle);
						}
						else //if(pPrevVertex->type==3)
						{	//椭圆弧
							double radius=fabs(pPrevVertex->radius)/scale;
							if(pPrevVertex->radius<0)
								sign*=-1;
							f3dPoint columnnorm=pPrevVertex->column_norm;
							f3dPoint center=pPrevVertex->center;
							if(pPrevVertex->vertex.feature%10==2&&pVertex->vertex.feature%10==2)
							{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换 wht 11-05-31
								center=pPlate->SpreadTransV(center);
								columnnorm=pPlate->SpreadTransV(columnnorm,false);
							}
							vector_trans(columnnorm,ucs,FALSE);
							coord_trans(center,ucs,FALSE);
							center.z=0;
							center/=scale;
							arcLine.CreateEllipse(center,line.startPt,line.endPt,columnnorm,f3dPoint(0,0,sign),radius);
						}
						if(arcLine.ColumnNorm()==arcLine.WorkNorm())	//圆弧
						{
							if(!sys.stru_map.bUsePolyLineDrawEdge)
							{
								if(!bDimHandle)
									outPutEntId=CreateAcadArcLine(pBlockTableRecord,arcLine.Center(),line.startPt,
												arcLine.SectorAngle(),arcLine.WorkNorm(),pPlate->handle);
								else
									CreateAcadArcLine(pBlockTableRecord,arcLine.Center(),line.startPt,
													arcLine.SectorAngle(),arcLine.WorkNorm());
							}
							else
							{	//多段线绘制圆弧 wht 10-12-21
								if(!bDimHandle)
									outPutEntId=CreateAcadPolyArcLine(pBlockTableRecord,arcLine,
												sys.stru_map.fPolyLineWidth,pPlate->handle);
								else
									CreateAcadPolyArcLine(pBlockTableRecord,arcLine,sys.stru_map.fPolyLineWidth);
							}
							bDimHandle=TRUE;
						}
						else
							CreateAcadEllipseLine(pBlockTableRecord,arcLine);
					}
					if(pPrevVertex->m_bWeldEdge)
					{
						if(pPrevVertex->type==0)
						{
							if(sign>0)
								DrawWeldLineSymbol(pBlockTableRecord,line.startPt,line.endPt,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
							else
								DrawWeldLineSymbol(pBlockTableRecord,line.endPt,line.startPt,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
						}
						else
							DrawWeldLineSymbol(pBlockTableRecord,arcLine,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
						GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
					}
					line.startPt=line.endPt;
					pPrevVertex=pVertex;
					if(bFinalEdge)
						break;
				}
			}
		}
		else
		{	//竖直放置
			pPlate->GetDesignItemValue('D', &D);
			pPlate->GetDesignItemValue('W', &W);
			CLDSLineTube *pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pPlate->m_hPartWeldParent);
			BOOL bIsZhiWan=pPlate->IsZhiWan();
			if(bIsZhiWan&&pPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{	//制弯的跨越连板
				f3dPoint huoqu_norm=pPlate->HuoquFaceNorm();
				f3dPoint huoqu_axis_vec=pPlate->HuoquAxisVec();
				CIRCULAR_PLATE_VERTEXSET vertexSet=pPlate->GetCirPlateVertexSet();
				if(fabs(ucs.axis_z*huoqu_axis_vec)>0.707)
				{	//绘制火曲线方向的截面 三个关键点(火曲点,W,N) wht 11-05-31
					f3dPoint vertex_arr[6];
					vertex_arr[0]=vertexSet.weld_outer_w;
					vertex_arr[1]=vertexSet.weld_outer_huoqu;
					vertex_arr[2]=vertexSet.weld_outer_n;
					f3dPoint axis_z1=pPlate->ucs.axis_z,axis_z2=huoqu_norm;
					vector_trans(axis_z1,ucs,FALSE);
					vector_trans(axis_z2,ucs,FALSE);
					for(i=0;i<3;i++)
					{
						f3dPoint axis_z=axis_z1;
						f3dPoint vertex=vertex_arr[i];
						if(i==2)
						{
							vertex=pPlate->GetRealVertex(vertex);
							axis_z=axis_z2;
						}
						coord_trans(vertex,ucs,FALSE);
						vertex.z=0;
						vertex/=scale;
						if(i==1)
						{	//火曲点
							axis_z=axis_z1+axis_z2;
							normalize(axis_z);
							double cosa=axis_z1*axis_z;
							vertex_arr[i]=vertex+axis_z*plate_thick*cosa*0.5*1.0/scale;
							vertex_arr[i+3]=vertex-axis_z*plate_thick*cosa*0.5*1.0/scale;
						}
						else 
						{
							vertex_arr[i]=vertex+axis_z*plate_thick*0.5*1.0/scale;
							vertex_arr[i+3]=vertex-axis_z*plate_thick*0.5*1.0/scale;
						}
					}

					if(pPlate->ucs.axis_y*ucs.axis_z<0)
						GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
					else
						GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);

					outPutEntId=CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1],pPlate->handle);
					CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[2]);
					CreateAcadLine(pBlockTableRecord,vertex_arr[3],vertex_arr[4]);
					CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[5]);
					CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[3]);
					CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[5]);
				}
				else 
				{	//绘制端面(N或W，火曲线终点)
					f3dPoint axis_z=pPlate->ucs.axis_z;
					line.startPt=vertexSet.weld_outer_huoqu;
					if(ucs.axis_z*pPlate->ucs.axis_x>0)
						line.endPt=vertexSet.weld_outer_w;
					else 
					{
						line.endPt=vertexSet.weld_outer_n;
						axis_z=pPlate->HuoquFaceNorm();
						line.endPt=pPlate->GetRealVertex(line.endPt);
					}
					vector_trans(axis_z,ucs,FALSE);
					coord_trans(line.startPt,ucs,FALSE);
					coord_trans(line.endPt,ucs,FALSE);
					axis_z.z=line.startPt.z=line.endPt.z=0;
					line.startPt/=scale;
					line.endPt/=scale;
					//
					f3dLine offset_line;
					offset_line.startPt=line.startPt+axis_z*plate_thick*1.0/scale;
					offset_line.endPt=line.endPt+axis_z*plate_thick*1.0/scale;
					//
					outPutEntId=CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt,pPlate->handle);
					CreateAcadLine(pBlockTableRecord,offset_line.startPt,offset_line.endPt);
					CreateAcadLine(pBlockTableRecord,line.startPt,offset_line.startPt);
					CreateAcadLine(pBlockTableRecord,line.endPt,offset_line.endPt);
				}
			}
			else 
			{	//绘制钢板截面图
				BOOL bVisible=TRUE;
				fPtList vertex_list;
				GetPlateSectionProfile(pPlate,ucs,1.0/scale,vertex_list,bVisible);
				int n=vertex_list.GetNodeNum();
				if(bVisible)
					GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				else	
					GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
				f3dPoint line_norm;
				f3dLine up_line,down_line;
				for(i=0;i<n;i++)
				{
					line.startPt=vertex_list[i];
					line.endPt=vertex_list[(i+1)%n];
					if(sys.stru_map.bUsePolyLineDrawEdge)	//使用多段线绘制钢板截面图 wht 11-07-19
					{
						if(i==0)
							outPutEntId=CreateAcadPolyLine(pBlockTableRecord,line.startPt,line.endPt,sys.stru_map.fPolyLineWidth,pPlate->handle);
						else 
							CreateAcadPolyLine(pBlockTableRecord,line.startPt,line.endPt,sys.stru_map.fPolyLineWidth);
					}
					else 
					{
						if(i==0)
							outPutEntId=CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt,pPlate->handle);
						else 
							CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
					}
					if(i==0)
						up_line=line;
					else if(i==1)
						line_norm=line.endPt-line.startPt;
					else if(i==2)
						down_line=line;
				}
				if(pPlate->m_iParamType!=TYPE_CIRCOVERPLATE)
				{	//绘制肋板焊缝线 wht 11-07-22
					normalize(line_norm);
					f3dPoint line_vec=up_line.endPt-up_line.startPt;
					normalize(line_vec);
					line_vec.Set(-line_vec.y,line_vec.x);
					BOOL bClockwise=(line_vec*line_norm>0);	//判断顺时针旋转还是逆时针旋转90°
					DrawWeldLineSymbol(pBlockTableRecord,up_line.startPt,up_line.endPt,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
					DrawWeldLineSymbol(pBlockTableRecord,down_line.startPt,down_line.endPt,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				}
			}
			/*else if(pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{
				f3dPoint perp,vec,norm_vec,mark_line_vec=ucs.axis_z^pPlate->ucs.axis_z;
				double max_pos_len=0,min_pos_len=0;
				normalize(mark_line_vec);
				BOOL bFirstVertex=TRUE;
				for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					f3dPoint vertice=pVertex->vertex;
					coord_trans(vertice,pPlate->ucs,TRUE);
					SnapPerp(&perp,pPlate->ucs.origin,pPlate->ucs.origin+mark_line_vec*100,vertice);
					vec=perp-pPlate->ucs.origin;
					double verify_value=vec*mark_line_vec;
					if(bFirstVertex)
					{
						max_pos_len=min_pos_len=verify_value;
						bFirstVertex=FALSE;
					}
					else if(verify_value>max_pos_len)
						max_pos_len=verify_value;
					else if(verify_value<min_pos_len)
						min_pos_len=verify_value;
				}
				if(pDatumTube)
				{
					if(min_pos_len<0&&min_pos_len>-pDatumTube->GetDiameter()*0.5)
						min_pos_len=-pDatumTube->GetDiameter()*0.5;
					else if(min_pos_len>0&&min_pos_len<pDatumTube->GetDiameter()*0.5)
						min_pos_len=pDatumTube->GetDiameter()*0.5;
				}
				if(max_pos_len>min_pos_len)
				{
					norm_vec=pPlate->ucs.axis_z;
					vector_trans(norm_vec,ucs,FALSE);
					norm_vec.z=0;
					normalize(norm_vec);
					line.startPt=pPlate->ucs.origin+mark_line_vec*min_pos_len;
					line.endPt=pPlate->ucs.origin+mark_line_vec*max_pos_len;
					coord_trans(line.startPt,ucs,FALSE);
					coord_trans(line.endPt,ucs,FALSE);
					line.startPt.z=line.endPt.z=0;
					line.startPt/=scale;
					line.endPt/=scale;
					double draw_thick=pPlate->thick/scale;
					line.startPt+=norm_vec*(0.5*draw_thick);
					line.endPt+=norm_vec*(0.5*draw_thick);
					DrawWeldLineSymbol(pBlockTableRecord,line.endPt,line.startPt,sys.general_set.fWeldLineSymbolHigh);
					if(pPlate->m_iParamType==TYPE_CIRRIBPLATE)
						GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
					else
						GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
					outPutEntId=CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt,pPlate->handle);
					line.startPt-=norm_vec*(draw_thick);
					line.endPt-=norm_vec*(draw_thick);
					CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
					CreateAcadLine(pBlockTableRecord,line.startPt,line.startPt+norm_vec*(draw_thick));
					CreateAcadLine(pBlockTableRecord,line.endPt,line.endPt+norm_vec*(draw_thick));
					DrawWeldLineSymbol(pBlockTableRecord,line.startPt,line.endPt,sys.general_set.fWeldLineSymbolHigh);
					//DrawWeldLineSymbol(pBlockTableRecord,line.startPt,line.startPt+norm_vec*(draw_thick),sys.general_set.fWeldLineSymbolHigh);
					//DrawWeldLineSymbol(pBlockTableRecord,line.endPt,line.endPt+norm_vec*(draw_thick),sys.general_set.fWeldLineSymbolHigh);
				}
			}
			else if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			{
				pPlate->GetDesignItemValue('A', &A);	//环向旋转角度°
				pPlate->GetDesignItemValue('B', &B);	//扇形夹角°
				pPlate->GetDesignItemValue('S', &S);	//负值表示上封板，正值表示下封板
				double max_pos_len=0,min_pos_len=0;
				f3dPoint norm_vec=ucs.axis_z;
				vector_trans(norm_vec,pPlate->ucs,FALSE);
				double norm_angle_pos=Cal2dLineAng(0,0,norm_vec.x,norm_vec.y)*DEGTORAD_COEF;
				double display_angle_start=norm_angle_pos-90;
				double display_angle_end=norm_angle_pos+90;
				//在设计环向封板坐标系时已考虑了旋转角A
				double section_angle_start=-B/2.0;		//A-B/2.0;
				double section_angle_end=B/2.0;			//A+B/2.0;
				if(display_angle_start<0||section_angle_start<0)
				{	//所有角度都修正为正值以便比对确定显示角度区域
					display_angle_start+=360;
					display_angle_end+=360;
					section_angle_start+=360;
					section_angle_end+=360;
				}
				//计算显示区域范围
				if(norm_angle_pos<=180)
				{	//start-->max  end-->min	wht 09-07-28
					if(display_angle_start<section_angle_start)
						display_angle_start=section_angle_start;
					if(display_angle_end>section_angle_end)
						display_angle_end=section_angle_end;
				}
				else
				{	//start-->min  end-->max
					if(display_angle_start>section_angle_start)
						display_angle_start=section_angle_start;
					if(display_angle_end<section_angle_end)
						display_angle_end=section_angle_end;
				}
				f3dLine prj_line;
				if(display_angle_end>display_angle_start)
				{	//
					f3dLine prj_line2;
					double inner_radius=pDatumTube->GetDiameter()*0.5;
					double outter_radius=pDatumTube->GetDiameter()*0.5+W;
					prj_line.startPt.x=outter_radius*cos(display_angle_start*RADTODEG_COEF);
					prj_line.startPt.y=outter_radius*sin(display_angle_start*RADTODEG_COEF);
					prj_line.endPt.x=outter_radius*cos(display_angle_end*RADTODEG_COEF);
					prj_line.endPt.y=outter_radius*sin(display_angle_end*RADTODEG_COEF);
					prj_line2.startPt.x=inner_radius*cos(display_angle_start*RADTODEG_COEF);
					prj_line2.startPt.y=inner_radius*sin(display_angle_start*RADTODEG_COEF);
					prj_line2.endPt.x=inner_radius*cos(display_angle_end*RADTODEG_COEF);
					prj_line2.endPt.y=inner_radius*sin(display_angle_end*RADTODEG_COEF);
					coord_trans(prj_line.startPt,pPlate->ucs,TRUE);
					coord_trans(prj_line.endPt,pPlate->ucs,TRUE);
					coord_trans(prj_line.startPt,ucs,FALSE);
					coord_trans(prj_line.endPt,ucs,FALSE);
					coord_trans(prj_line2.startPt,pPlate->ucs,TRUE);
					coord_trans(prj_line2.endPt,pPlate->ucs,TRUE);
					coord_trans(prj_line2.startPt,ucs,FALSE);
					coord_trans(prj_line2.endPt,ucs,FALSE);
					prj_line.startPt.z=prj_line.endPt.z=prj_line2.startPt.z=prj_line2.endPt.z=0;
					norm_vec=pPlate->ucs.axis_z;
					vector_trans(norm_vec,ucs,FALSE);
					norm_vec.z=0;
					normalize(norm_vec);
					f3dPoint prj_vec=prj_line.endPt-prj_line.startPt;
					double len=prj_vec.mod();
					normalize(prj_vec);
					f3dPoint vec=prj_line2.startPt-prj_line.startPt;
					double dd_start=prj_vec*vec;
					vec=prj_line2.endPt-prj_line.startPt;
					double dd_end=prj_vec*vec;
					if(dd_start<0)
						prj_line.startPt=prj_line2.startPt;
					if(dd_end>len)
						prj_line.endPt=prj_line2.endPt;
					vec=prj_vec^norm_vec;
					if(vec.z<0)	//始终端反向
					{
						f3dPoint perp=prj_line.startPt;
						prj_line.startPt=prj_line.endPt;
						prj_line.endPt=perp;
					}
					//计算环形封板在绘图坐标系下的截面绘制法线
					prj_line.startPt/=scale;
					prj_line.endPt/=scale;
					double draw_thick=plate_thick/scale;
					prj_line.startPt+=norm_vec*(0.5*draw_thick);
					prj_line.endPt+=norm_vec*(0.5*draw_thick);
					GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
					if(!sys.stru_map.bUsePolyLineDrawEdge)
						outPutEntId=CreateAcadLine(pBlockTableRecord,prj_line.startPt,prj_line.endPt,pPlate->handle);
					else
						outPutEntId=CreateAcadPolyLine(pBlockTableRecord,prj_line.startPt,prj_line.endPt,sys.stru_map.fPolyLineWidth,pPlate->handle);
					if(S<0)
						DrawWeldLineSymbol(pBlockTableRecord,prj_line.endPt,prj_line.startPt,sys.general_set.fWeldLineSymbolHigh);
					prj_line.startPt-=norm_vec*(draw_thick);
					prj_line.endPt-=norm_vec*(draw_thick);
					GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
					if(!sys.stru_map.bUsePolyLineDrawEdge)
						CreateAcadLine(pBlockTableRecord,prj_line.startPt,prj_line.endPt);
					else
						CreateAcadPolyLine(pBlockTableRecord,prj_line.startPt,prj_line.endPt,sys.stru_map.fPolyLineWidth);
					if(S>0)
						DrawWeldLineSymbol(pBlockTableRecord,prj_line.startPt,prj_line.endPt,sys.general_set.fWeldLineSymbolHigh);
					GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
					if(!sys.stru_map.bUsePolyLineDrawEdge)
					{
						CreateAcadLine(pBlockTableRecord,prj_line.startPt,prj_line.startPt+norm_vec*(draw_thick));
						CreateAcadLine(pBlockTableRecord,prj_line.endPt,prj_line.endPt+norm_vec*(draw_thick));
					}
					else
					{
						CreateAcadPolyLine(pBlockTableRecord,prj_line.startPt,prj_line.startPt+norm_vec*(draw_thick),sys.stru_map.fPolyLineWidth);
						CreateAcadPolyLine(pBlockTableRecord,prj_line.endPt,prj_line.endPt+norm_vec*(draw_thick),sys.stru_map.fPolyLineWidth);
					}
				}
			}*/
		}
	}
	else if(pPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{	//遮水板
		long iDatumEdgeType=0,hDatumTube=0;
		pPlate->GetDesignItemValue('D', &D);	//内圆直径
		pPlate->GetDesignItemValue('W', &W);	//外圆直径
		pPlate->GetDesignItemValue('K', &K);	//流水槽宽度
		pPlate->GetDesignItemValue('A', &A);	//两流水槽之间夹角
		pPlate->GetDesignItemValue('H', &H);	//流水槽深度
		pPlate->GetDesignItemValue('O', &O);	//两流水槽夹角的平分线与基准边之间的夹角
		pPlate->GetDesignItemValue('L', &L);	//流水槽长度
		pPlate->GetDesignItemValue('E', &iDatumEdgeType);	//基准边标志 0.基准边 1.焊道线
		pPlate->GetDesignItemValue('C', &hDatumTube);	//基准钢管句柄
		CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(hDatumTube,CLS_LINETUBE);
		if(pDatumTube==NULL||D>0)	//遮水板内环直径不可能大于0
			return;
		pDatumTube->BuildUCS();
		double T=pPlate->thick;
		if(h0v1==1)
		{	//侧面
			//侧立剖面放置图绘制
			//应保证pre_vec同时垂直于遮水板Z轴方向以及绘图坐标系Z轴方向	wht 09-12-08
			//RotateVectorAroundVector(pre_vec,-Pi*0.5,ucs.axis_z);//f3dPoint(0,1,0)); //绕Z轴旋转不能保证垂直于Z轴
			f3dPoint pre_vec=pPlate->ucs.axis_z^ucs.axis_z;
			normalize(pre_vec);
			start_firs=pPlate->ucs.origin-0.5*W*pre_vec;
			end_firs=pPlate->ucs.origin+0.5*W*pre_vec;
			start_sec=start_firs+pPlate->ucs.axis_z*T;
			end_sec=end_firs+pPlate->ucs.axis_z*T;
			//转换到绘图坐标系下
			coord_trans(start_firs, ucs, FALSE);
			coord_trans(end_firs, ucs, FALSE);
			coord_trans(start_sec, ucs, FALSE);
			coord_trans(end_sec, ucs, FALSE);
			start_firs.z=end_firs.z=start_sec.z=end_sec.z=0;
			//调配绘图比例
			start_firs = start_firs*(1.0/scale);
			end_firs = end_firs*(1.0/scale);
			start_sec = start_sec*(1.0/scale);
			end_sec = end_sec*(1.0/scale);
			//绘制遮水板侧面图
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			if(!sys.stru_map.bUsePolyLineDrawEdge)
			{
				outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pPlate->handle);
				CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
				CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
				CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
			}
			else 
			{
				outPutEntId=CreateAcadPolyLine(pBlockTableRecord,start_firs,end_firs,sys.stru_map.fPolyLineWidth,pPlate->handle);
				CreateAcadPolyLine(pBlockTableRecord,start_sec,end_sec,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,start_firs,start_sec,sys.stru_map.fPolyLineWidth);
				CreateAcadPolyLine(pBlockTableRecord,end_firs,end_sec,sys.stru_map.fPolyLineWidth);
			}
		}
		else if(h0v1==0)
		{	//水平绘制
			point = pPlate->ucs.origin;
			//转换到绘图坐标系下
			coord_trans(point, ucs, FALSE);
			point.z = 0;
			//调整绘图比例
			point = point*(1.0/scale);
			//绘制遮水板平面图
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			if(D>0)
				CreateAcadCircle(pBlockTableRecord, point,D/scale);
			outPutEntId=CreateAcadCircle(pBlockTableRecord, point,0.5*W/scale,pPlate->handle);
			//
			BOOL bHasWaterSolt=TRUE;
			if(fabs(A)<EPS||fabs(H)<EPS||fabs(K)<EPS||fabs(L)<EPS)
				bHasWaterSolt=FALSE;	//两流水槽夹角、长度、深度或宽度为0时表示无流水槽 wht 11-07-01
			if(bHasWaterSolt)	//带流水槽
			{	
				A*=RADTODEG_COEF;	//转换为弧度
				double sina=sin(A*0.5);
				double cosa=cos(A*0.5);
				line.startPt=point;
				line.endPt.Set(point.x+0.5*W*cosa/scale,point.y+0.5*W*sina/scale);
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
				line.endPt.Set(point.x+0.5*W*cosa/scale,point.y-0.5*W*sina/scale);
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
				if(cosa!=0&&sina!=0)
				{	//绘制流水槽内侧边
					double a=0,b=0,c=0;
					c=(K/sina)/scale;
					a=c*cosa;
					b=0.5*W/scale-c;
					line.startPt.Set(point.x+c,point.y);
					line.endPt.Set(line.startPt.x+b*cosa,point.y+b*sina);
					CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
					line.startPt.Set(point.x+c,point.y);
					line.endPt.Set(line.startPt.x+b*cosa,point.y-b*sina);
					CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
				}
				//标注两流水槽之间的夹角
				f3dPoint dim_vertex,dim_start,dim_end,dim_pos;
				dim_vertex = point;
				dim_end.Set(point.x+0.5*(W+10)*cosa/scale,point.y+0.5*(W+10)*sina/scale);
				dim_start.Set(point.x+0.5*(W+10)*cosa/scale,point.y-0.5*(W+10)*sina/scale);
				dim_pos.Set(point.x+0.5*W/scale+sys.dim_map.fDimAngleTextSize*1.5,point.y);
				//
				DimStyleTable::dimAngleStyle.InitDimStyle(sys.dim_map.fDimAngleTextSize);
				GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,
					dim_vertex,dim_start,dim_end,dim_pos,"<>");
			}
		}
	}
	else if(pPlate->m_iParamType==TYPE_BASEPLATE)
	{
		if(h0v1==0)
		{	//水平绘制

		}
		else if(h0v1==1)
		{	//侧向绘制
			BOOL bVisible=TRUE;
			fPtList vertex_list;
			GetPlateSectionProfile(pPlate,ucs,1.0/scale,vertex_list,bVisible);
			int n=vertex_list.GetNodeNum();
			if(bVisible)
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			else	
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			for(i=0;i<n;i++)
			{
				if(sys.stru_map.bUsePolyLineDrawEdge)
					CreateAcadPolyLine(pBlockTableRecord,vertex_list[i],vertex_list[(i+1+n)%n],sys.stru_map.fPolyLineWidth);
				else 
					CreateAcadLine(pBlockTableRecord,vertex_list[i],vertex_list[(i+1+n)%n]);
			}
		}
	}
	else if(pPlate->m_iParamType==TYPE_SHOEPLATE)
	{
		if(h0v1==0)
		{	//水平绘制
			f3dPoint f_start,f_end;
			int n=pPlate->vertex_list.GetNodeNum();
			for(i=0;i<n;i++)
			{
				PROFILE_VERTEX *pStartVertex=pPlate->vertex_list.GetByIndex(i);
				PROFILE_VERTEX *pEndVertex=pPlate->vertex_list.GetByIndex((i+1+n)%n);
				f_start=pStartVertex->vertex;
				f_end=pEndVertex->vertex;
				coord_trans(f_start,pPlate->ucs,TRUE);
				coord_trans(f_start,ucs,FALSE);
				f_start/=scale;
				f_start.z=0;
				coord_trans(f_end,pPlate->ucs,TRUE);
				coord_trans(f_end,ucs,FALSE);
				f_end/=scale;
				f_end.z=0;
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				if(!sys.stru_map.bUsePolyLineDrawEdge)
					CreateAcadLine(pBlockTableRecord,f_start,f_end);
				else
					CreateAcadPolyLine(pBlockTableRecord,f_start,f_end,sys.stru_map.fPolyLineWidth);
				if(pStartVertex->m_bWeldEdge)
					DrawWeldLineSymbol(pBlockTableRecord,f_start,f_end,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
			}
		}
		else if(h0v1==1)
		{	//侧向绘制
			BOOL bVisible=TRUE;
			fPtList vertex_list;
			GetParaPlateSectionProfile(pPlate,ucs,1.0/scale,vertex_list,bVisible);
			int n=vertex_list.GetNodeNum();
			if(bVisible)
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			else	
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			for(i=0;i<n;i++)
			{
				if(sys.stru_map.bUsePolyLineDrawEdge)
					CreateAcadPolyLine(pBlockTableRecord,vertex_list[i],vertex_list[(i+1+n)%n],sys.stru_map.fPolyLineWidth);
				else 
					CreateAcadLine(pBlockTableRecord,vertex_list[i],vertex_list[(i+1+n)%n]);
			}
		}
	}
	if(h0v1==0)
	{	//参数化板绘制螺栓时不用显示所有螺栓，否则十字插板立板螺栓显示错误 wht 11-07-09
		CDrawCoordTrans dcs(ucs,scale);
		DrawLsRefList(pPlate->GetLsRefList(),dcs,FALSE,&ucs.axis_z);//显示所有螺栓
		CDimPlankLs dimls;
		dimls.Create(pPlate);
	}
	else if(h0v1==1&&bSetupDrawing&&pDatumTube)
	{
		f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		double dd=fabs(datum_line_vec*pPlate->ucs.axis_z);
		//横向放置的钢管如果有竖向绘制的板与其不垂直时需要标注板与基准钢管的角度
		if(fabs(datum_line_vec*ucs.axis_x)>EPS_COS&&dd>EPS&&dd<EPS_COS)
		{	//板与基准钢管不垂直不平行，需要标注角度
			SETUPANGULARDIM_STRU angular_dim;
			f3dPoint vertexPoint,lineStart,lineEnd,arcPoint;
			//角顶点标记点
			vertexPoint=pPlate->ucs.origin;
			coord_trans(vertexPoint,ucs,FALSE);
			vertexPoint.z=0;
			vertexPoint/=scale;
			AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(vertexPoint.x,vertexPoint.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.vertexPointId=pointId;
			//角起始边标记点
			lineStart = pPlate->ucs.origin+datum_line_vec*400;
			coord_trans(lineStart,ucs,FALSE);
			lineStart.z=0;
			lineStart/=scale;
			pPoint=new AcDbPoint(AcGePoint3d(lineStart.x,lineStart.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.lineStartPointId=pointId;
			//终止边标记点
			f3dPoint norm=datum_line_vec^pPlate->ucs.axis_z;
			f3dPoint prj_line_vec=norm^pPlate->ucs.axis_z;
			vector_trans(prj_line_vec,ucs,FALSE);
			prj_line_vec.z=0;
			normalize(prj_line_vec);
			if(prj_line_vec.x<0)
				prj_line_vec*=-1.0;
			lineEnd=pPlate->ucs.origin;
			coord_trans(lineEnd,ucs,FALSE);
			lineEnd+=prj_line_vec*400;
			lineEnd.z=0;
			lineEnd/=scale;
			pPoint=new AcDbPoint(AcGePoint3d(lineEnd.x,lineEnd.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
			{
				pPoint->close();
				angular_dim.lineEndPointId=pointId;
				//标注位置圆弧标记点
				arcPoint=(lineStart+lineEnd)*0.5;
			}
			pPoint=new AcDbPoint(AcGePoint3d(arcPoint.x,arcPoint.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			angular_dim.arcPointId=pointId;
			DRAGSET.dimAngleList.append(angular_dim);
		}
	}
	if(pOutEntId)	//关联构件句柄的CAD实体ID wht 10-12-02
		*pOutEntId=outPutEntId;
}

static f3dPoint create_joint_line(AcDbBlockTableRecord *pBlockTableRecord,
						 f3dPoint f_start,f3dPoint f_end,
						 UCS_STRU &ucs,double scale)
{
	coord_trans(f_start,ucs,FALSE);
	coord_trans(f_end,ucs,FALSE);
	f_start = f_start*(1.0/scale);
	f_end   = f_end*(1.0/scale);
	f_start.z=f_end.z=0;
	CreateAcadLine(pBlockTableRecord,f_start,f_end,NULL);
	return 0.5*(f_start+f_end);
}
void DrawWrapPlate(AcDbBlockTableRecord *pBlockTableRecord, 
				CLDSPlate *pWrapPlate, UCS_STRU &ucs,double scale)
{
	f3dPoint axis_x,axis_y,direct,f_start,f_end,f_tem_end;
	if(pWrapPlate==NULL)
		return;
	axis_x = pWrapPlate->ucs.axis_x;
	axis_y = pWrapPlate->ucs.axis_y;
	f3dPoint geom_center;
	if(fabs(dot_prod(ucs.axis_z,pWrapPlate->ucs.axis_z))>EPS_COS2)
	{		//正面的包钢板
		f_start = pWrapPlate->vertex_list.GetFirst()->vertex;
		f_end = pWrapPlate->vertex_list.GetNext()->vertex;
		coord_trans(f_start,pWrapPlate->ucs,TRUE);
		coord_trans(f_end,pWrapPlate->ucs,TRUE);
		f_tem_end = f_end;
		create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
		
		f_start = f_tem_end;
		f_end = pWrapPlate->vertex_list.GetNext()->vertex;
		coord_trans(f_end,pWrapPlate->ucs,TRUE);
		f_tem_end = f_end;
		create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
		
		f_start = f_tem_end;
		f_end = pWrapPlate->vertex_list.GetNext()->vertex;
		coord_trans(f_end,pWrapPlate->ucs,TRUE);
		f_tem_end = f_end;
		create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
		
		f_start = f_end;
		f_end = pWrapPlate->vertex_list.GetFirst()->vertex;
		coord_trans(f_end,pWrapPlate->ucs,TRUE);
		geom_center=create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
	}
	else		//侧面的包钢板
	{
		double length,tm_len1,tm_len2;
		//计算包钢板的长度
		tm_len1 = DISTANCE(pWrapPlate->vertex_list.GetFirst()->vertex,pWrapPlate->vertex_list.GetNext()->vertex);
		tm_len2 = DISTANCE(pWrapPlate->vertex_list.GetFirst()->vertex,pWrapPlate->vertex_list.GetTail()->vertex);
		length = __max(tm_len1,tm_len2);

		f_start = pWrapPlate->ucs.origin;
		Add_Pnt(f_end,pWrapPlate->ucs.origin,pWrapPlate->ucs.axis_z*pWrapPlate->GetThick());
		f_tem_end = f_end;
		create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
		
		f_start = f_tem_end;
		Add_Pnt(f_end,f_start,length*axis_y);
		f_tem_end = f_end;
		create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
		
		f_start = f_tem_end;
		Sub_Pnt(f_end,f_start,pWrapPlate->GetThick()*pWrapPlate->ucs.axis_z);
		f_tem_end = f_end;
		create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
		
		f_start = f_end;
		Sub_Pnt(f_end,f_start,length*pWrapPlate->ucs.axis_y);
		geom_center=create_joint_line(pBlockTableRecord,f_start,f_end,ucs,scale);
	}
	
	DrawLsRefList(pWrapPlate->GetLsRefList(),CDrawCoordTrans(ucs,scale));
	//插入扩展数据点，以存储连接板信息
	geom_center.z=0;
	AcDbXrecord *pXrec;
	AcDbObjectId dictObjId, xrecObjId,pointId;
	AcDbDictionary *pDict;
	AcDbPoint *pPoint=new AcDbPoint(AcGePoint3d(geom_center.x,geom_center.y,geom_center.z));
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
	{
		pPoint->createExtensionDictionary();
		dictObjId = pPoint->extensionDictionary();
		pPoint->close();
	}
	acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
	pXrec = new AcDbXrecord;
#ifdef _ARX_2007
	pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
	pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
	pDict->close();
	
	struct resbuf *head;
	head = acutBuildList(AcDb::kDxfInt32, pWrapPlate->handle,AcDb::kDxfInt32, CLS_PLATE,0);
	pXrec->setFromRbChain(*head);
	acutRelRb(head);
	pXrec->close();
}
void DrawJoint(AcDbBlockTableRecord *pBlockTableRecord, 
				CJoint *pJoint, UCS_STRU &ucs,double scale)
{
	if(pJoint==NULL)
		return;
	if(fabs(pJoint->ucs.axis_z*ucs.axis_z)>0.4)
		return;	//隔面上的接头不绘制
	CDrawCoordTrans dcs(ucs,scale);
	for(CLDSPart *pPart=pJoint->GetFirstPart();pPart;pPart=pJoint->GetNextPart())
	{
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			DrawJgStru(pBlockTableRecord,CDrawPartInfo(pPart),dcs,FALSE);
		else
		{
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			DrawPlateStruDrawing(pBlockTableRecord,(CLDSPlate*)pPart,dcs,FALSE);
		}
	}
}
void DrawFoot(AcDbBlockTableRecord *pBlockTableRecord, 
				CFoot *pFoot, UCS_STRU &ucs,double scale)
{
	CLDSPlate *pPlank;
	if(pFoot==NULL)
		return;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	int bFirstPlank=TRUE;
	UCS_STRU tm_ucs=ucs;
	if(fabs(tm_ucs.axis_x.y)<EPS2||fabs(tm_ucs.axis_x.x)<EPS2)	//正/侧面图
	{
		tm_ucs.axis_z.z=0;
		normalize(tm_ucs.axis_z);
		tm_ucs.axis_y=tm_ucs.axis_z^tm_ucs.axis_x;
		tm_ucs.axis_x=tm_ucs.axis_y^tm_ucs.axis_z;
		normalize(tm_ucs.axis_x);
		normalize(tm_ucs.axis_y);
	}
	for(pPlank=pFoot->GetFirstPlank();pPlank;pPlank=pFoot->GetNextPlank())
	{
		pPlank->feature=0;
		CDrawCoordTrans dcs(tm_ucs,scale);
		DrawPlateStruDrawing(pBlockTableRecord,pPlank,dcs,FALSE);
	}
}
void DrawFooStruView(AcDbBlockTableRecord *pBlockTableRecord, 
	PARTSET& partSet, UCS_STRU &ucs,double scale)
{
	if(partSet.GetNodeNum()<=0)
		return;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	UCS_STRU tm_ucs=ucs;
	if(fabs(tm_ucs.axis_x.y)<EPS2||fabs(tm_ucs.axis_x.x)<EPS2)	//正/侧面图
	{
		tm_ucs.axis_z.z=0;
		normalize(tm_ucs.axis_z);
		tm_ucs.axis_y=tm_ucs.axis_z^tm_ucs.axis_x;
		tm_ucs.axis_x=tm_ucs.axis_y^tm_ucs.axis_z;
		normalize(tm_ucs.axis_x);
		normalize(tm_ucs.axis_y);
	}
	for(CLDSPart* pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate* pPlank=(CLDSPlate*)pPart;
			pPlank->feature=0;
			CDrawCoordTrans dcs(tm_ucs,scale);
			DrawPlateStruDrawing(pBlockTableRecord,pPlank,dcs,FALSE);
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate* pPlank=(CLDSParamPlate*)pPart;
			DrawParamPlateStruDrawing(pBlockTableRecord,pPlank,tm_ucs,scale,FALSE);
		}
	}
}
//计算单线图中接头单线标记的位置
BOOL GetJointLine(f3dLine &joint_line,CLDSNode *pNode,UCS_STRU *pUCS)
{
	f3dPoint jg_vec,jg_direct,joint_base_pt;
	CJoint *pJoint=NULL;
	//CLDSNode::ATTACH_PART *pAttach=pNode->FindActivePartGroup();
	//pJoint = (CJoint*)Ta.FromPartHandle(pAttach->joint_handle,CLS_JOINT);
	if(pJoint==NULL)
		return FALSE;
	if(fabs(pJoint->ucs.axis_z*pUCS->axis_z)>0.4)
		return FALSE;	//隔面上的接头不绘制
	CLDSLineAngle *pBaseJg = (CLDSLineAngle*)Ta.FromPartHandle(pJoint->base_jg_handle_arr[0],CLS_LINEANGLE);
	if(pBaseJg==NULL)
		return FALSE;
	Sub_Pnt(jg_vec,pBaseJg->pEnd->Position(false),pBaseJg->pStart->Position(false));
	normalize(jg_vec);
	Add_Pnt(joint_line.startPt,pBaseJg->GetDatumPosBer(pNode),jg_vec*(sys.nJointLineLen/2.0));
	Sub_Pnt(joint_line.endPt ,pBaseJg->GetDatumPosBer(pNode),jg_vec*(sys.nJointLineLen/2.0));
	if(dot_prod(pUCS->axis_z,pBaseJg->get_norm_x_wing())>
		dot_prod(pUCS->axis_z,pBaseJg->get_norm_y_wing()))
		jg_direct = pBaseJg->get_norm_y_wing();
	else
		jg_direct = pBaseJg->get_norm_x_wing();
	Add_Pnt(joint_line.startPt,joint_line.startPt,jg_direct*sys.nJointLineOffset);
	Add_Pnt(joint_line.endPt,joint_line.endPt,jg_direct*sys.nJointLineOffset);
	coord_trans(joint_line.startPt,*pUCS,FALSE);
	coord_trans(joint_line.endPt,*pUCS,FALSE);
	joint_line.startPt.z=joint_line.endPt.z=0;
	return TRUE;
}

typedef CLDSPart* PARTPTR;
//塔构件类型排序,原则如下:
//主材角钢0;塔脚板2,其它构件1
static int GetTaAtomTypeGrade(CLDSPart *pPart)
{
	if(pPart->IsLinePart())
	{
		char layertag;
		int grade;
		layertag=pPart->layer(1);
		if(layertag=='Z')
			grade = 0;
		else if(layertag=='X')
			grade = 10;
		else if(layertag=='H')
			grade = 20;
		else
			grade = 30;
		if(pPart->m_hPartWeldParent>0)
			grade--;
		if(pPart->GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube *pTube=(CLDSLineTube*)pPart;
			if(pTube->m_tJointStart.type==1||pTube->m_tJointEnd.type==1)
				grade--;	//有圆柱面相贯线
		}
		return grade;
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE)
		return 40;
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		return 50;
	else if(pPart->GetClassTypeId()==CLS_FOOT)
		return 60;
	else
		return 70;
}
//优先级高的构件级别高,但特征值低(如主材为0)
//-1表示第一构件级别高 0表示相同 1表示第二构件级别高
static int CompareTaAtomType(const PARTPTR &pPart1,const PARTPTR &pPart2)
{
	int grade1,grade2;
	grade1 = GetTaAtomTypeGrade(pPart1);
	grade2 = GetTaAtomTypeGrade(pPart2);

	if(grade1<grade2)
		return 1;
	else if(grade1>grade2)
		return -1;
	else if(pPart1->IsLinePart()&&!pPart2->IsLinePart())
		return 1;
	else if(!pPart1->IsLinePart()&&pPart2->IsLinePart())
		return -1;
	else
		return 0;
}

void DrawLineSlotStru(AcDbBlockTableRecord *pBlockTableRecord,CLDSLineSlot *pLineSlot, UCS_STRU &ucs,
					  double scale,BOOL bSetupDrawing,AcDbObjectId *pOutEntId=NULL)
{
	if(pLineSlot==NULL)
		return;
	AcDbObjectId outPutEntId=0;	//关联构件句柄的实体ID wht 10-12-02
	pLineSlot->BuildUCS();
	f3dArcLine arc_line;
	f3dLine axis_line,line,nail_line1,nail_line2;	
	f3dCircle ls_cir;
	f3dPoint len_vec=pLineSlot->End()-pLineSlot->Start();
	normalize(len_vec);
	double foot_nail_len=16;	//绘制脚钉图符时的脚钉长度
	f3dPoint start_firs,end_firs,start_sec,end_sec,start_thir,end_thir,start_four,end_four;
	int h0v1s2=-1;	//平铺绘制-0 端面绘制-1 侧面绘制-2
	if(fabs(pLineSlot->ucs.axis_z*ucs.axis_z)>0.707)
		h0v1s2=2;	//端面
	else if(fabs(pLineSlot->ucs.axis_y*ucs.axis_z)>0.707)
		h0v1s2=0;	//平铺
	else 
		h0v1s2=1;	//侧面

	f3dPoint center,line_norm_vec;
	BOOL bFootNailPlate=FALSE;
	CLDSBolt *pFootNail=NULL;
	if(pLineSlot->GetLsCount()==1)
	{	//槽钢在此处为脚钉板
		pFootNail=pLineSlot->GetFirstLsRef()->GetLsPtr();
		if(pFootNail&&pFootNail->IsFootNail())
			bFootNailPlate=TRUE;	
	}
	if(h0v1s2==0)
	{	//平铺
		axis_line.startPt=pLineSlot->Start()-len_vec*pLineSlot->startOdd();
		axis_line.endPt=pLineSlot->End()+len_vec*pLineSlot->endOdd();
		start_firs=axis_line.startPt-pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		end_firs=axis_line.endPt-pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		start_sec=axis_line.startPt-pLineSlot->ucs.axis_x*(0.5*pLineSlot->GetHeight()-pLineSlot->GetThick());
		end_sec=axis_line.endPt-pLineSlot->ucs.axis_x*(0.5*pLineSlot->GetHeight()-pLineSlot->GetThick());
		start_thir=axis_line.startPt+pLineSlot->ucs.axis_x*(0.5*pLineSlot->GetHeight()-pLineSlot->GetThick());
		end_thir=axis_line.endPt+pLineSlot->ucs.axis_x*(0.5*pLineSlot->GetHeight()-pLineSlot->GetThick());
		start_four=axis_line.startPt+pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		end_four=axis_line.endPt+pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		//脚钉
		center=(axis_line.startPt+axis_line.endPt)*0.5;
		line_norm_vec.Set(-len_vec.y,len_vec.x);
		normalize(line_norm_vec);
		if(line_norm_vec*ucs.axis_y<0)
			line_norm_vec*=-1.0;
		nail_line1.startPt=center+line_norm_vec*foot_nail_len*0.5;
		nail_line1.endPt=center-line_norm_vec*(foot_nail_len*0.5+2);
		nail_line2.startPt=center+line_norm_vec*foot_nail_len*0.5+len_vec*(foot_nail_len*0.5-2);
		nail_line2.endPt=center+line_norm_vec*foot_nail_len*0.5-len_vec*(foot_nail_len*0.5-2);

		if(pLineSlot->startOdd()<0)
			axis_line.startPt+=len_vec*pLineSlot->startOdd();
		axis_line.startPt-=len_vec*sys.stru_map.fPoleAxisLineStartOutSpreadLen;//准线两端都伸出30mm
		if(pLineSlot->endOdd()<0)
			axis_line.endPt-=len_vec*pLineSlot->endOdd();
		axis_line.endPt+=len_vec*sys.stru_map.fPoleAxisLineEndOutSpreadLen;//准线两端都伸出30mm
		//转换到绘图坐标系下
		coord_trans(axis_line.startPt, ucs, FALSE);
		coord_trans(axis_line.endPt, ucs, FALSE);
		coord_trans(start_firs,ucs,FALSE);
		coord_trans(end_firs,ucs,FALSE);
		coord_trans(start_sec,ucs,FALSE);
		coord_trans(end_sec,ucs,FALSE);
		coord_trans(start_thir,ucs,FALSE);
		coord_trans(end_thir,ucs,FALSE);
		coord_trans(start_four,ucs,FALSE);
		coord_trans(end_four,ucs,FALSE);
		coord_trans(center,ucs,FALSE);
		coord_trans(nail_line1.startPt,ucs,FALSE);
		coord_trans(nail_line1.endPt,ucs,FALSE);
		coord_trans(nail_line2.startPt,ucs,FALSE);
		coord_trans(nail_line2.endPt,ucs,FALSE);
		nail_line1.startPt.z=nail_line1.endPt.z=nail_line2.startPt.z=nail_line2.endPt.z=
		axis_line.startPt.z=axis_line.endPt.z=start_firs.z=end_firs.z=center.z=
		start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=0;
		//调配绘图比例
		axis_line.startPt/=scale;
		axis_line.endPt/=scale;
		start_firs/=scale;
		end_firs/=scale;
		start_sec/=scale;
		end_sec/=scale;
		start_thir/=scale;
		end_thir/=scale;
		start_four/=scale;
		end_four/=scale;
		center/=scale;
		nail_line1.startPt/=scale;
		nail_line1.endPt/=scale;
		nail_line2.startPt/=scale;
		nail_line2.endPt/=scale;
		
		//绘制槽钢
		if(!bFootNailPlate)
		{
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,axis_line.startPt,axis_line.endPt);
		}
		//根据槽钢法线及绘图坐标系Z轴方向判断所在图层 wht 12-03-22
		if(pLineSlot->WorkPlaneNorm()*ucs.axis_z>0)
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);	//实线
		else 
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);	//虚线
		if(pFootNail)
		{	//绘制脚钉图符
			CreateAcadLine(pBlockTableRecord,nail_line1.startPt,nail_line1.endPt);
			CreateAcadLine(pBlockTableRecord,nail_line2.startPt,nail_line2.endPt);
			CreateAcadCircle(pBlockTableRecord,center,10/scale,pFootNail->handle);
		}
		outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pLineSlot->handle);
		CreateAcadLine(pBlockTableRecord,start_four,end_four);
		CreateAcadLine(pBlockTableRecord,start_firs,start_four);
		CreateAcadLine(pBlockTableRecord,end_firs,end_four);
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
		CreateAcadLine(pBlockTableRecord,start_thir,end_thir);
	}
	else if(h0v1s2==1)
	{	//侧面
		start_firs=pLineSlot->Start()-len_vec*pLineSlot->startOdd ();
		end_firs=pLineSlot->End()+len_vec*pLineSlot->endOdd();
		start_sec=start_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		end_sec=end_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		//脚钉
		center=(start_firs+end_firs)*0.5;
		line_norm_vec.Set(-len_vec.y,len_vec.x);
		normalize(line_norm_vec);
		if(line_norm_vec*pLineSlot->ucs.axis_y>0)
			line_norm_vec*=-1.0;
		nail_line1.startPt=center+line_norm_vec*foot_nail_len*1.5;
		nail_line1.endPt=center-line_norm_vec*5;
		nail_line2.startPt=nail_line1.startPt+len_vec*(foot_nail_len*0.5-2);
		nail_line2.endPt=nail_line1.startPt-len_vec*(foot_nail_len*0.5-2);
		//转换到绘图坐标系下
		coord_trans(start_firs,ucs,FALSE);
		coord_trans(end_firs,ucs,FALSE);
		coord_trans(start_sec,ucs,FALSE);
		coord_trans(end_sec,ucs,FALSE);
		coord_trans(nail_line1.startPt,ucs,FALSE);
		coord_trans(nail_line1.endPt,ucs,FALSE);
		coord_trans(nail_line2.startPt,ucs,FALSE);
		coord_trans(nail_line2.endPt,ucs,FALSE);
		nail_line1.startPt.z=nail_line1.endPt.z=
		nail_line2.startPt.z=nail_line2.endPt.z=
		start_firs.z=end_firs.z=start_sec.z=end_sec.z=0;
		//调整X方向的坐标
		start_firs/=scale;
		end_firs/=scale;
		start_sec/=scale;
		end_sec/=scale;
		nail_line1.startPt/=scale;
		nail_line1.endPt/=scale;
		nail_line2.startPt/=scale;
		nail_line2.endPt/=scale;
		if(bFootNailPlate&&pFootNail)
		{	//绘制脚钉图符
			GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
			CreateAcadLine(pBlockTableRecord,nail_line1.startPt,nail_line1.endPt);
			CreateAcadLine(pBlockTableRecord,nail_line2.startPt,nail_line2.endPt);
		}
		//绘制槽钢
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pLineSlot->handle);
		CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
		CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
		CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
	}
	else if(h0v1s2==2)
	{	//端面
		start_firs=pLineSlot->Start()-len_vec*pLineSlot->startOdd ()-pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		end_firs=pLineSlot->Start()-len_vec*pLineSlot->startOdd()+pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		start_sec=start_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		end_sec=end_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		start_thir=start_sec+pLineSlot->ucs.axis_x*pLineSlot->GetThick();
		end_thir=end_sec-pLineSlot->ucs.axis_x*pLineSlot->GetThick();
		start_four=start_thir-pLineSlot->ucs.axis_y*(pLineSlot->GetWidth()-pLineSlot->GetThick());
		end_four=end_thir-pLineSlot->ucs.axis_y*(pLineSlot->GetWidth()-pLineSlot->GetThick());
		//脚钉
		center=(start_firs+end_firs)*0.5;
		len_vec=start_firs-end_firs;
		normalize(len_vec);
		line_norm_vec.Set(-len_vec.y,len_vec.x);
		normalize(line_norm_vec);
		if(line_norm_vec*pLineSlot->ucs.axis_y>0)
			line_norm_vec*=-1.0;
		nail_line1.startPt=center+line_norm_vec*foot_nail_len*1.5;
		nail_line1.endPt=center-line_norm_vec*10;
		nail_line2.startPt=nail_line1.startPt+len_vec*(foot_nail_len*0.5-2);
		nail_line2.endPt=nail_line1.startPt-len_vec*(foot_nail_len*0.5-2);
		//转换到绘图坐标系下
		coord_trans(start_firs,ucs,FALSE);
		coord_trans(end_firs,ucs,FALSE);
		coord_trans(start_sec,ucs,FALSE);
		coord_trans(end_sec,ucs,FALSE);
		coord_trans(start_thir,ucs,FALSE);
		coord_trans(end_thir,ucs,FALSE);
		coord_trans(start_four,ucs,FALSE);
		coord_trans(end_four,ucs,FALSE);
		coord_trans(nail_line1.startPt,ucs,FALSE);
		coord_trans(nail_line1.endPt,ucs,FALSE);
		coord_trans(nail_line2.startPt,ucs,FALSE);
		coord_trans(nail_line2.endPt,ucs,FALSE);
		nail_line1.startPt.z=nail_line1.endPt.z=
		nail_line2.startPt.z=nail_line2.endPt.z=
		start_firs.z=end_firs.z=start_sec.z=end_sec.z=
		start_thir.z=end_thir.z=start_four.z=end_four.z=0;
		//调整X方向的坐标
		start_firs/=scale;
		end_firs/=scale;
		start_sec/=scale;
		end_sec/=scale;
		start_thir/=scale;
		end_thir/=scale;
		start_four/=scale;
		end_four/=scale;
		nail_line1.startPt/=scale;
		nail_line1.endPt/=scale;
		nail_line2.startPt/=scale;
		nail_line2.endPt/=scale;
		if(bFootNailPlate&&pFootNail)
		{	//绘制脚钉图符
			GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
			CreateAcadLine(pBlockTableRecord,nail_line1.startPt,nail_line1.endPt);
			CreateAcadLine(pBlockTableRecord,nail_line2.startPt,nail_line2.endPt);
		}
		//绘制槽钢
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		outPutEntId=CreateAcadLine(pBlockTableRecord,start_firs,end_firs,pLineSlot->handle);
		CreateAcadLine(pBlockTableRecord,start_four,end_four);
		CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
		CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
		CreateAcadLine(pBlockTableRecord,start_sec,start_thir);
		CreateAcadLine(pBlockTableRecord,end_sec,end_thir);
		CreateAcadLine(pBlockTableRecord,start_thir,start_four);
		CreateAcadLine(pBlockTableRecord,end_thir,end_four);
	}
	if(pOutEntId)	//关联构件句柄的CAD实体ID wht 10-12-02
		*pOutEntId=outPutEntId;
}

void DrawLineFlatStru(AcDbBlockTableRecord *pBlockTableRec,CLDSLineFlat *pLineFlat,UCS_STRU &ucs,
					  double scale,BOOL bSetUpDrawing,AcDbObjectId *pOutEntId=NULL)
{
	if(pLineFlat==NULL)
		return;
	AcDbObjectId outPutEntId=0;	
	pLineFlat->BuildUCS();
	f3dPoint len_vec=pLineFlat->End()-pLineFlat->Start();
	normalize(len_vec);
	int h0v1s2=-1;	//平铺绘制-0 端面绘制-1 侧面绘制-2
	if(fabs(pLineFlat->ucs.axis_z*ucs.axis_z)>0.707)
		h0v1s2=0;	//平铺
	else if(fabs(pLineFlat->ucs.axis_y*ucs.axis_z)>0.707)
		h0v1s2=0;	//侧面
	else 
		h0v1s2=1;	//端面
	f3dPoint start_firs,end_firs,start_sec,end_sec;
	f3dLine axis_line;
	axis_line.startPt=pLineFlat->Start()-len_vec*pLineFlat->startOdd();
	axis_line.endPt=pLineFlat->End()+len_vec*pLineFlat->endOdd();
	if(h0v1s2==0)
	{	//平铺
		start_firs=axis_line.startPt-pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
		end_firs=axis_line.endPt-pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
		start_sec=axis_line.startPt+pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
		end_sec=axis_line.endPt+pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
	}
	else if(h0v1s2==1)
	{	//侧面
		start_firs=axis_line.startPt;
		end_firs=axis_line.endPt;
		start_sec=start_firs+pLineFlat->ucs.axis_z*pLineFlat->GetThick();
		end_sec=end_firs+pLineFlat->ucs.axis_z*pLineFlat->GetThick();
	}
	else if(h0v1s2==2)
	{	//端面
		start_firs=axis_line.startPt-pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
		end_firs=axis_line.endPt-pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
		start_sec=axis_line.startPt+pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
		end_sec=axis_line.endPt+pLineFlat->ucs.axis_y*0.5*pLineFlat->GetWidth();
	}
	//转换到绘图坐标系下
	coord_trans(start_firs,ucs,FALSE);
	coord_trans(end_firs,ucs,FALSE);
	coord_trans(start_sec,ucs,FALSE);
	coord_trans(end_sec,ucs,FALSE);
	//调整绘图比例
	start_firs/=scale;
	end_firs/=scale;
	start_sec/=scale;
	end_sec/=scale;
	//绘制扁铁
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	outPutEntId=CreateAcadLine(pBlockTableRec,start_firs,end_firs,pLineFlat->handle);
	CreateAcadLine(pBlockTableRec,start_sec,end_sec);
	CreateAcadLine(pBlockTableRec,start_firs,start_sec);
	CreateAcadLine(pBlockTableRec,end_firs,end_sec);
	if(h0v1s2==0)
	{	//绘制螺栓
		CDrawCoordTrans dcs(ucs,scale);
		DrawLsRefList(pLineFlat->GetLsRefList(),dcs,TRUE,&ucs.axis_z);
	}
	if(pOutEntId)
		*pOutEntId=outPutEntId;
}
void DrawGroupLineAngle(AcDbBlockTableRecord *pBlockTableRecord,CLDSGroupLineAngle *pGroupJg,
						UCS_STRU &ucs,double scale,AcDbObjectId *pOutEntId=NULL)
{	//组合角钢仅需要绘制心线
	f3dPoint lineStart=pGroupJg->Start();
	f3dPoint lineEnd  =pGroupJg->End();
	f3dPoint vec=lineEnd-lineStart;
	normalize(vec);
	lineStart-=vec*50;
	lineEnd+=vec*50;
	GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
	coord_trans(lineStart, ucs, FALSE);
	coord_trans(lineEnd, ucs, FALSE);
	lineStart/=scale;
	lineEnd/=scale;
	lineStart.z=lineEnd.z=0;
	AcDbObjectId entId=CreateAcadLine(pBlockTableRecord,lineStart,lineEnd,pGroupJg->handle);
	if(pOutEntId)
		*pOutEntId=entId;
}

//绘制结构图时用来判断钢板绘制顺序 wht 11-07-12
void ProcessPlateStu(CLDSPlate *pPlate,UCS_STRU ucs,PARTSET& OutSidePlateSet,PARTSET& InSidePlateSet)
{
	if(pPlate==NULL)
		return;
	//大致判断钢板是平铺绘制还是截面绘制,平铺绘制为内侧绘制
	int h0v1=0;
	CViewPart *pViewPart=NULL;
	pViewPart=(CViewPart*)pPlate->feature;
	if(pViewPart&&pViewPart->cFlag=='X')		//平铺绘制
		h0v1=0;
	else if(pViewPart&&pViewPart->cFlag=='Z')	//竖直绘制
		h0v1=1;
	else if(pViewPart&&pViewPart->cFlag=='S'&&pPlate->face_N>1)	//展开绘制多面板
		h0v1=0;
	else	//未指定绘制方式，自动判断绘制方式
	{
		if(fabs(ucs.axis_z*pPlate->ucs.axis_z)>0.707)
			h0v1=0;	//水平绘制
		else
			h0v1=1;	//竖直绘制
	}
	if(h0v1==0)
		InSidePlateSet.append(pPlate);
	else
	{	//绘制钢板截面时Z坐标大于0的顶点数大于2时认为该钢板为可见轮廓
		int nVisibleNum=0;
		int i=0,n=pPlate->vertex_list.GetNodeNum();
		for(i=0;i<n;i++)
		{
			PROFILE_VERTEX *pStartVertex=&pPlate->vertex_list[i];
			PROFILE_VERTEX *pEndVertex=&pPlate->vertex_list[(i+1)%n];
			f3dPoint start_vertex=pStartVertex->vertex;
			coord_trans(start_vertex,pPlate->ucs,TRUE);
			f3dPoint vertex=start_vertex;
			coord_trans(vertex,ucs,FALSE);
			if(vertex.z>0)
				nVisibleNum++;
			if(pStartVertex->type>0)
			{
				double sector_angle=0;
				if(pStartVertex->type==1)
					sector_angle=fabs(pStartVertex->sector_angle);
				else 
				{
					f3dArcLine arcline;
					//椭圆也按圆来近似处理
					int sign=1;
					if(pStartVertex->radius<0)
						sign=-1;
					f3dPoint center_pick=pStartVertex->center;
					double radius=pStartVertex->radius;
					if(pStartVertex->type==3)	
					{	//椭圆弧圆心拾取点为绝对坐标需要转换到相对坐标系下 wht 11-07-02
						coord_trans(center_pick,pPlate->ucs,FALSE);	//相对坐标 wht 11-01-21
						double cosa=pStartVertex->column_norm*pPlate->ucs.axis_z;
						radius/=fabs(cosa);	//计算椭圆弧按圆弧近似处理时的半径
					}
					arcline.CreateMethod3(pStartVertex->vertex,pEndVertex->vertex,
						f3dPoint(0,0,sign),sign*radius,center_pick);
					sector_angle=arcline.SectorAngle();
				}
				//
				f3dPoint start_vec=start_vertex-pPlate->ucs.origin;
				f3dPoint end_vertex=pEndVertex->vertex;
				coord_trans(end_vertex,pPlate->ucs,TRUE);
				f3dPoint end_vec=end_vertex-pPlate->ucs.origin;
				f3dPoint mid_vec=start_vec+end_vec;
				if(sector_angle>Pi)
					mid_vec*=-1.0;
				if(mid_vec*ucs.axis_z>0)
				{	//为可见轮廓边,赋值3结束循环
					nVisibleNum=3;
					break;
				}
			}
			if(nVisibleNum>=2)
				break;
		}
		if(nVisibleNum>=2)
			OutSidePlateSet.append(pPlate);
		else 
			InSidePlateSet.append(pPlate);
	}
}

void DrawStruDrawing(PARTSET& partset,NODESET& nodeset,UCS_STRU ucs,
					 int nFaceFlag,double scale,BOOL bSetupDrawing)
{	
	CLogErrorLife loglife;	//记录错误日志
	AcDbObjectId LineId;//定义标识符
	AcGePoint3d start,end;
	InitDrawingEnv();
	CStackAcadVar stackCmdEcho("CMDECHO",0);
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord=NULL;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表
	LSDRAW.EmptyDrawList();
	LSSPACEDIM.Empty();
	INSIDEJGSETDRAW.Empty();
	OUTSIDEJGSETDRAW.Empty();
	INSIDEPARAMPLATESET.Empty();
	OUTSIDEPARAMPLATESET.Empty();
	//根据构件的类型进行排序
	try{
		int nPart = partset.GetNodeNum();
		PARTPTR *partPtrArr = new PARTPTR[nPart];
		int i=0;
		CLDSPart *pPart=NULL;
		for(pPart=partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
		{
			partPtrArr[i] = pPart;
			i++;
		}
		CHeapSort<PARTPTR>::HeapSort(partPtrArr,nPart,CompareTaAtomType);
		partset.Empty();
		for(i=0;i<nPart;i++)
			partset.append(partPtrArr[i]);
		delete []partPtrArr;

		PARTSET TubeSET,LINESLOTSET,OutSidePlateSet,InSidePlateSet;
		LINEPARTSET OutSideShortJgSet,InsideShortJgSet;
		CDrawCoordTrans dcs(ucs,scale);
		for(pPart=partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
		{
			if(pPart->m_bVirtualPart)	//成图时不绘制隐含构件
				continue;
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pJg=(CLDSLineAngle*)pPart;
				f3dPoint cur_jg_vec=pJg->End()-pJg->Start();
				normalize(cur_jg_vec);
				if((pJg->pStart==NULL||pJg->pEnd==NULL)
					&&(pJg->desStartPos.datumPoint.datum_pos_style==7
					||pJg->desEndPos.datumPoint.datum_pos_style==7))
				{	//短角钢始端/终端基准点定位方式为:7.沿角钢两肢法向偏移的包铁楞点 wht 11-03-02
					char norm_offset[50]="";
					if(pJg->desStartPos.datumPoint.datum_pos_style==7)
						strcpy(norm_offset,pJg->desStartPos.datumPoint.des_para.RODEND.norm_offset);
					else if(pJg->desEndPos.datumPoint.datum_pos_style==7)
						strcpy(norm_offset,pJg->desEndPos.datumPoint.des_para.RODEND.norm_offset);
					CDesignThickPara thick(norm_offset);
					double norm_offset_dist=thick.Thick(&Ta);
					if(norm_offset_dist>0)
						OutSideShortJgSet.append(pJg);
					else 
						InsideShortJgSet.append(pJg);
				}
				//支持绘制角钢截面 wht 10-06-17
				if(pJg->m_bFootNail||fabs(cur_jg_vec*ucs.axis_z)>0.70710)	//角钢脚钉或需要绘制角钢截面的角钢
					DrawJgStru(pBlockTableRecord,CDrawPartInfo(pJg),dcs,bSetupDrawing);
				else
				{
					if(!ProcessJgStru(pJg,ucs))
						DrawJgStru(pBlockTableRecord,CDrawPartInfo(pJg),dcs,bSetupDrawing);
				}
			}
			else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE) //绘制组合角钢
				DrawGroupLineAngle(pBlockTableRecord,(CLDSGroupLineAngle*)pPart,ucs,scale);
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				TubeSET.append(pPart);
			else if(pPart->GetClassTypeId()==CLS_PLATE)		//绘制普通钢板结构图
				ProcessPlateStu((CLDSPlate*)pPart,ucs,OutSidePlateSet,InSidePlateSet);
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)//绘制参数化板（如端板等）结构图
				ProcessParamPlateStru((CLDSParamPlate*)pPart,ucs);
			else if(pPart->GetClassTypeId()==CLS_BOLT)		//绘制螺栓
				LSDRAW.AppendLsRef((CLDSBolt*)pPart,TRUE,ucs.axis_z);
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)	//绘制槽钢(现在只有脚钉板用到槽钢)
				LINESLOTSET.append(pPart);
		}
		//里外铁角钢分开绘制用以进行一定程度上的消隐
		DrawAngleToMap(InsideShortJgSet,TRUE,pBlockTableRecord,dcs,bSetupDrawing);//绘制列铁短角钢
		DrawAngleToMap(INSIDEJGSETDRAW,TRUE,pBlockTableRecord,dcs,bSetupDrawing);	//绘制里铁角钢
		for(pPart=INSIDEPARAMPLATESET.GetFirst();pPart;pPart=INSIDEPARAMPLATESET.GetNext())
			DrawParamPlateStruDrawing(pBlockTableRecord,(CLDSParamPlate*)pPart,ucs,scale,bSetupDrawing);
		for(pPart=InSidePlateSet.GetFirst();pPart;pPart=InSidePlateSet.GetNext())
			DrawPlateStruDrawing(pBlockTableRecord,(CLDSPlate*)pPart,dcs,bSetupDrawing);
		for(pPart=TubeSET.GetFirst();pPart;pPart=TubeSET.GetNext())
			DrawTubeStru(pBlockTableRecord,(CLDSLineTube*)pPart,dcs,FALSE,bSetupDrawing);
		for(pPart=OutSidePlateSet.GetFirst();pPart;pPart=OutSidePlateSet.GetNext())
			DrawPlateStruDrawing(pBlockTableRecord,(CLDSPlate*)pPart,dcs,bSetupDrawing);
		for(pPart=OUTSIDEPARAMPLATESET.GetFirst();pPart;pPart=OUTSIDEPARAMPLATESET.GetNext())
			DrawParamPlateStruDrawing(pBlockTableRecord,(CLDSParamPlate*)pPart,ucs,scale,bSetupDrawing);
		for(pPart=LINESLOTSET.GetFirst();pPart;pPart=LINESLOTSET.GetNext())
			DrawLineSlotStru(pBlockTableRecord,(CLDSLineSlot*)pPart,ucs,scale,bSetupDrawing);
		DrawAngleToMap(OUTSIDEJGSETDRAW,FALSE,pBlockTableRecord,dcs,bSetupDrawing);//绘制外铁角钢
		DrawAngleToMap(OutSideShortJgSet,FALSE,pBlockTableRecord,dcs,bSetupDrawing);//绘制外铁短角钢
		/*CFoot *pFoot =Ta.Foot.FromHandle(pAttach->foot_handle);
		if(pFoot)
		DrawFoot(pBlockTableRecord,pFoot,ucs,scale);	//已改为单独绘制
		*/
		GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
		LSDRAW.DrawToMap(pBlockTableRecord,ucs,scale);
		DimLsSpaceToMap(pBlockTableRecord,ucs);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	if(pBlockTableRecord)
		pBlockTableRecord->close();//关闭块表记录
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
}
extern UCS_STRU seg_map_base_face_ucs;	//分段图基本面（第一面绘制相对坐标系）
struct CIRCULAR_DIMLINEANG{
	f3dLine line;
	double lineAngle;
	CIRCULAR_DIMLINEANG(){;}
	CIRCULAR_DIMLINEANG(f3dLine dimLine,double angle){line=dimLine;lineAngle=angle;}
};
void DrawLinePartGroupSetupDrawing(CLDSLinePart *pDatumLinePart,PARTSET& partset,NODESET& nodeset,
						   UCS_STRU ucs,double scale,BOOL bSetupDrawing)
{
	CLDSPart *pPart=NULL;
	SETUPPOSDIM_STRU posDim;
	f3dPoint posPoint;	//定位点
	AcDbObjectId pointId,LineId;//定义标识符
	if(pDatumLinePart==NULL)
		return;
	CSuperSmartPtr<CLDSLinePart>pDatumPart=pDatumLinePart;
	if(pDatumPart->IsLinePart())
	{	//如果定位基准构件是杆件，则记录钢管的长度延伸方向作为绘图坐标系的X轴方向
		DRAGSET.axis_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
		normalize(DRAGSET.axis_vec);
	}
	else
		DRAGSET.axis_vec.Set();
	//DrawStruDrawing(partset,nodeset,ucs,0,scale,TRUE);
	AcGePoint3d start,end;
	InitDrawingEnv();
	//获得当前图形的块表指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	LSDRAW.EmptyDrawList();
	LSSPACEDIM.Empty();
	INSIDEJGSETDRAW.Empty();
	OUTSIDEJGSETDRAW.Empty();
	//根据构件的类型进行排序
	int nPart = partset.GetNodeNum();
	PARTPTR *partPtrArr = new PARTPTR[nPart];
	int i=0;
	for(pPart=partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		partPtrArr[i] = pPart;
		i++;
	}
	CHeapSort<PARTPTR>::HeapSort(partPtrArr,nPart,CompareTaAtomType);
	partset.Empty();
	for(i=0;i<nPart;i++)
		partset.append(partPtrArr[i]);
	delete []partPtrArr;

	PARTSET PLATESET,PARAMPLATESET,TubeSET,LINESLOTSET,allplateset;
	CDrawCoordTrans dcs(ucs,scale);
	for(pPart=partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		if(pPart->m_bVirtualPart)	//成图时不绘制隐含构件
			continue;
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			if(!ProcessJgStru((CLDSLineAngle*)pPart,ucs))
				DrawJgStru(pBlockTableRecord,CDrawPartInfo(pPart),dcs,FALSE);
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
			TubeSET.append(pPart);
		else if(pPart->GetClassTypeId()==CLS_PLATE)		//绘制普通钢板结构图
		{
			PLATESET.append(pPart);
			allplateset.append(pPart);
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)//绘制参数化板（如端板等）结构图
		{
			PARAMPLATESET.append(pPart);
			allplateset.append(pPart);
		}
		else if(pPart->GetClassTypeId()==CLS_BOLT)		//绘制螺栓
			LSDRAW.AppendLsRef((CLDSBolt*)pPart,TRUE,ucs.axis_z);
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)	//绘制槽钢(现在只有脚钉板用到槽钢)
			LINESLOTSET.append(pPart);
	}
	ATOM_LIST<CIRCULAR_DIMLINEANG>middlelineset;	//各板的中间层线
	if(fabs(DRAGSET.axis_vec*ucs.axis_z)>EPS_COS2)
	{	//端面图分析基准钢管轴向钢板的环向角度标注
		if(pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pDatumPart.LineTubePointer()->m_bHasWeldRoad)
		{
			CIRCULAR_DIMLINEANG *pDimLine=NULL,*pPrevLine=NULL;
			double weldRoadAngle=pDatumPart.LineTubePointer()->CalWeldRoadAngle();
			f3dPoint weld_road_vec(cos(weldRoadAngle),sin(weldRoadAngle));
			vector_trans(weld_road_vec,pDatumPart->ucs,TRUE);	//由基准钢管相对坐标系转为全塔坐标系
			vector_trans(weld_road_vec,ucs,FALSE);	//由全塔坐标系转为绘图坐标系
			for(pPart=allplateset.GetFirst();pPart;pPart=allplateset.GetNext())
			{
				if(pPart->ucs.axis_z*ucs.axis_z>EPS2)
					continue;	//不是基准钢管轴向钢板
				f3dPoint markVertex,markOrigin,markVec;
				if(pPart->GetClassTypeId()==CLS_PLATE)
				{
					CLDSPlate *pPlate=(CLDSPlate*)pPart;
					markOrigin=pPlate->ucs.origin+(pPlate->m_fNormOffset+0.5*pPlate->GetThick())*pPlate->ucs.axis_z;
					for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
						markVertex+=pVertex->vertex;
					coord_trans(markVertex,pPlate->ucs,TRUE);
				}
				else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
					if(pParamPlate->vertex_list.GetNodeNum()==0)
						pParamPlate->DesignPlate();
					markOrigin=pParamPlate->ucs.origin;
					for(PROFILE_VERTEX *pVertex=pParamPlate->vertex_list.GetFirst();pVertex;pVertex=pParamPlate->vertex_list.GetNext())
						markVertex+=pVertex->vertex;
					coord_trans(markVertex,pParamPlate->ucs,TRUE);
				}
				markVec=markVertex-pDatumPart->Start();
				coord_trans(markOrigin,ucs,FALSE);
				markOrigin/=scale;
				vector_trans(markVec,ucs,FALSE);
				markOrigin.z=markVec.z=0;
				normalize(markVec);
				CIRCULAR_DIMLINEANG dimline;
				dimline.line.startPt=markOrigin;
				dimline.line.endPt=markOrigin+markVec*50;
				dimline.lineAngle=Cal2dLineAng(0,0,markVec.x,markVec.y)-weldRoadAngle;
				if(dimline.lineAngle<0)
					dimline.lineAngle+=2*Pi;
				for(pDimLine=middlelineset.GetFirst();pDimLine;pDimLine=middlelineset.GetNext())
				{
					if(pDimLine->lineAngle<dimline.lineAngle)
						continue;
					else
					{
						middlelineset.insert(dimline);
						break;
					}
				}
				if(pDimLine==NULL)
					middlelineset.append(dimline);
			}
			if(middlelineset.GetNodeNum()>0)
			{
				f3dLine weld_line;
				weld_line.startPt=pDatumPart.LineTubePointer()->Start();
				coord_trans(weld_line.startPt,ucs,FALSE);
				weld_line.startPt/=scale;
				weld_line.startPt.z=0;
				weld_line.endPt=weld_line.startPt+weld_road_vec*100;
				f3dLine prev_line,cur_line;
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				pPrevLine=middlelineset.GetTail();	//焊道线前一板连接方向
				//标注基准钢管焊道线与前一板连接方向间的夹角
				AcDbObjectId pointId,LineId;//定义标识符
				SETUPANGULARDIM_STRU angular_dim;
				f3dPoint vertexPoint,lineStart,lineEnd,arcPoint;
				prev_line=pPrevLine->line;
				cur_line=weld_line;
				//角顶点标记点
				Int3dll(prev_line,cur_line,vertexPoint);
				AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(vertexPoint.x,vertexPoint.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				angular_dim.vertexPointId=pointId;
				//角起始边标记点
				lineStart = prev_line.endPt;
				pPoint=new AcDbPoint(AcGePoint3d(lineStart.x,lineStart.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				angular_dim.lineStartPointId=pointId;
				//终止边标记点
				lineEnd=cur_line.endPt;
				pPoint=new AcDbPoint(AcGePoint3d(lineEnd.x,lineEnd.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				angular_dim.lineEndPointId=pointId;
				//标注位置圆弧标记点
				arcPoint=(lineStart+lineEnd)*0.5;
				pPoint=new AcDbPoint(AcGePoint3d(arcPoint.x,arcPoint.y,0));
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
					pPoint->close();
				angular_dim.arcPointId=pointId;
				DRAGSET.dimAngleList.append(angular_dim);
				pPrevLine=NULL;
				for(pDimLine=middlelineset.GetFirst();pDimLine;pDimLine=middlelineset.GetNext())
				{
					if(pPrevLine)
						prev_line=pPrevLine->line;
					else
						prev_line=weld_line;
					cur_line=pDimLine->line;
					//角顶点标记点
					Int3dll(prev_line,cur_line,vertexPoint);
					AcDbPoint* pPoint=new AcDbPoint(AcGePoint3d(vertexPoint.x,vertexPoint.y,0));
					if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
						pPoint->close();
					angular_dim.vertexPointId=pointId;
					//角起始边标记点
					lineStart = prev_line.endPt;
					pPoint=new AcDbPoint(AcGePoint3d(lineStart.x,lineStart.y,0));
					if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
						pPoint->close();
					angular_dim.lineStartPointId=pointId;
					//终止边标记点
					lineEnd=cur_line.endPt;
					pPoint=new AcDbPoint(AcGePoint3d(lineEnd.x,lineEnd.y,0));
					if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
						pPoint->close();
					angular_dim.lineEndPointId=pointId;
					//标注位置圆弧标记点
					arcPoint=(lineStart+lineEnd)*0.5;
					pPoint=new AcDbPoint(AcGePoint3d(arcPoint.x,arcPoint.y,0));
					if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
						pPoint->close();
					angular_dim.arcPointId=pointId;
					DRAGSET.dimAngleList.append(angular_dim);
					pPrevLine=pDimLine;
				}
			}
		}
	}
	//里外铁角钢分开绘制用以进行一定程度上的消隐	
	DrawAngleToMap(INSIDEJGSETDRAW,TRUE,pBlockTableRecord,dcs,bSetupDrawing);//绘制里铁角钢
	for(pPart=PLATESET.GetFirst();pPart;pPart=PLATESET.GetNext())
		DrawPlateStruDrawing(pBlockTableRecord,(CLDSPlate*)pPart,dcs,bSetupDrawing);
	for(pPart=PARAMPLATESET.GetFirst();pPart;pPart=PARAMPLATESET.GetNext())
		DrawParamPlateStruDrawing(pBlockTableRecord,(CLDSParamPlate*)pPart,ucs,scale,bSetupDrawing);
	for(pPart=TubeSET.GetFirst();pPart;pPart=TubeSET.GetNext())
	{
		if(bSetupDrawing&&pDatumPart->IsLinePart()&&pPart->m_hPartWeldParent==pDatumPart->handle)
		{	//焊接在定位基准构件上的杆件
			f3dLine datum_line(pDatumPart.LinePartPointer()->Start(),pDatumPart.LinePartPointer()->End());
			f3dLine ray_line(((CLDSLineTube*)pPart)->Start(),((CLDSLineTube*)pPart)->End());
			//空间计算射线杆件在定位基准杆件定位基准线上的交点位置及距离起始端的距离
			Int3dll(datum_line,ray_line,posPoint);
			posDim.posDist=DISTANCE(datum_line.startPt,posPoint);
			//计算射线方向
			f3dPoint dimVec;	//射线方向
			f3dPoint vec1=ray_line.startPt-posPoint;
			f3dPoint vec2=ray_line.endPt-posPoint;
			vector_trans(vec1,ucs,FALSE);
			vector_trans(vec2,ucs,FALSE);
			if(vec1.mod2()>vec2.mod2())
				dimVec=vec1;
			else
				dimVec=vec2;
			//转换到绘图坐标系下计算实际绘图中的交点（即定位标记点的绘图位置）
			coord_trans(datum_line.startPt,ucs,FALSE);
			coord_trans(datum_line.endPt,ucs,FALSE);
			coord_trans(ray_line.startPt,ucs,FALSE);
			coord_trans(ray_line.endPt,ucs,FALSE);
			datum_line.startPt.z=datum_line.endPt.z=ray_line.startPt.z=ray_line.endPt.z=0;
			datum_line.startPt/=scale;
			datum_line.endPt/=scale;
			ray_line.startPt/=scale;
			ray_line.endPt/=scale;
			Int3dll(datum_line,ray_line,posPoint);
			//插入定位标记点
			AcDbPoint *pPoint=new AcDbPoint(AcGePoint3d(posPoint.x,posPoint.y,0));
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))
				pPoint->close();
			posDim.posPointId=pointId;
			if(dimVec.y>0)
				posDim.dimPosOffset=1.0;
			else
				posDim.dimPosOffset=-1.0;
			DRAGSET.dimPosList.append(posDim);
		}
		CDrawCoordTrans dcs(ucs,scale);
		DrawTubeStru(pBlockTableRecord,(CLDSLineTube*)pPart,dcs,TRUE,bSetupDrawing);
	}
	for(pPart=LINESLOTSET.GetFirst();pPart;pPart=LINESLOTSET.GetNext())
		DrawLineSlotStru(pBlockTableRecord,(CLDSLineSlot*)pPart,dcs.cs,dcs.m_fDrawScale,bSetupDrawing);
	DrawAngleToMap(OUTSIDEJGSETDRAW,FALSE,pBlockTableRecord,dcs,bSetupDrawing);//绘制外铁角钢
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	LSDRAW.DrawToMap(pBlockTableRecord,ucs,scale);
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	pBlockTableRecord->close();
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
#endif 

typedef CLDSLinePart* POLEPTR;
static int ComparePoleOrderFunc(const POLEPTR& item1,const POLEPTR& item2)
{
	int nGrade1=0,nGrade2=0;
	char cFlag1=item1->Layer(1);
	char cFlag2=item2->Layer(1);
	if(toupper(cFlag1)=='Z')
		nGrade1=3;
	else if(toupper(cFlag1)=='X')
		nGrade1=2;
	else if(toupper(cFlag1)=='F')
		nGrade1=1;
	// 1.按杆件类型排序 Z>X>F>other
	if(toupper(cFlag2)=='Z')
		nGrade2=3;
	else if(toupper(cFlag1)=='X')
		nGrade2=2;
	else if(toupper(cFlag1)=='F')
		nGrade2=1;
	if(nGrade1>nGrade2)
		return -1;
	else if(nGrade1<nGrade2)
		return 1;
	else 
	{	// 2.按杆件长度排序
		double fPoleLen1=item1->GetLength();
		double fPoleLen2=item2->GetLength();
		if(fPoleLen1>fPoleLen2)
			return -1;
		else if(fPoleLen1<fPoleLen2)
			return 1;
		else 
		{	// 3.长度相同时按宽度(肢宽、直径)排序
			char sSpec1[50]="",sSpec2[50]="";
			item1->GetSpecification(sSpec1);
			item2->GetSpecification(sSpec2);
			return CompareMultiSectionString(sSpec1,sSpec2)*-1;
		}
	}
	return 0;
}

//计算绘图工作面坐标系
UCS_STRU CalDrawingUcs(PARTSET &partset,CDisplayView *pView,BOOL bSingleLineMap)
{
	UCS_STRU ucs;
	if(pView==NULL)
		return ucs;
	UCS_STRU near_ucs=pView->ucs;
	ucs=near_ucs;
	if(partset.GetNodeNum()<2)
		return ucs;
    if(bSingleLineMap&&pView->m_iViewType==1)
		return ucs;	// 1.单线图：定制视图不需要重新计算坐标系及坐标原点
	// 遴选出需要平铺绘制的杆件并进行排序
	CLDSPart *pPart=NULL;
	CArray<POLEPTR,POLEPTR&> poleArr;
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart->IsLinePart())
		{
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			f3dPoint len_vec=pLinePart->End()-pLinePart->Start();
			normalize(len_vec);
			if(fabs(ucs.axis_z*len_vec)>0.7071)
				continue;
			poleArr.Add(pLinePart);
		}	
	}
	CQuickSort<CLDSLinePart*>::QuickSort(poleArr.GetData(),poleArr.GetSize(),ComparePoleOrderFunc);
	// 计算绘图坐标系及坐标系原点
	int i=0,n=poleArr.GetSize();
	if(!bSingleLineMap&&pView->m_iViewType==1&&n>1)
	{	// 2.结构图：定制视图仅需要计算坐标系原点位置即可
		ucs.origin=poleArr[0]->Start();
		return ucs;
	}
	f3dPoint vec1,vec2;
	if(n>0)
	{
		CLDSLinePart *pLinePart=poleArr[0];
		if(bSingleLineMap)
		{	// 3.单线图：使用单线方向
			if(pLinePart->pEnd&&pLinePart->pStart)
				Sub_Pnt(vec1,pLinePart->pEnd->Position(true),pLinePart->pStart->Position(true));
			else
				Sub_Pnt(vec1,pLinePart->End(),pLinePart->Start());
		}
		else 
			Sub_Pnt(vec1,pLinePart->End(),pLinePart->Start());
		normalize(vec1);
		ucs.origin=pLinePart->Start();
	}
	i=1;
	while(i<n)
	{
		CLDSLinePart *pLinePart=poleArr[i];
		if(bSingleLineMap)
		{	// 3.单线图：使用单线方向
			if(pLinePart->pEnd&&pLinePart->pStart)
				Sub_Pnt(vec2,pLinePart->pEnd->Position(true),pLinePart->pStart->Position(true));
			else
				Sub_Pnt(vec2,pLinePart->End(),pLinePart->Start());
		}
		else 
			Sub_Pnt(vec2,pLinePart->End(),pLinePart->Start());
		normalize(vec2);
		ucs.axis_z = vec1^vec2;
		i++;
		double mod_len=ucs.axis_z.mod();
		if(mod_len<EPS2)	//方向过于接近误差较大，跳过该杆件
			continue;	//杆件方向相同
		ucs.axis_z/=mod_len;
		break;
		// 5.结构图：图层视图需要计算坐标系原点
		//if(!bSingleLineMap)
		//	ucs.origin=pLinePart->Start();
	}
	if(i==n)
	{
		ucs.axis_z = vec1^ucs.axis_x;
		normalize(ucs.axis_z);
	}
	// 单线\结构图：计算绘图坐标系
	if(ucs.axis_z*near_ucs.axis_z<0)	//Z轴方向应是向外的
		ucs.axis_z = -1.0*ucs.axis_z;
	normalize(ucs.axis_z);
	ucs.axis_y = ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	/*for(i=0;i<n;i++)
	{
		CLDSLinePart *pLinePart=poleArr[i];
		if(bSingleLineMap)
		{	// 3.单线图：使用单线方向
			if(pLinePart->pEnd&&pLinePart->pStart)
				Sub_Pnt(vec,pLinePart->pEnd->pos,pLinePart->pStart->pos);
			else
				Sub_Pnt(vec,pLinePart->End(),pLinePart->Start());
		}
		else 
			Sub_Pnt(vec,pLinePart->End(),pLinePart->Start());
		normalize(vec);
		double dd = fabs(vec*ucs.axis_x);
		//杆件不是沿X轴方向的杆件可以用叉积求Z轴
		if(dd<EPS_COS)
		{	// 4.单线\结构图：计算绘图坐标系
			ucs.axis_z = vec^ucs.axis_x;
			if(ucs.axis_z*near_ucs.axis_z<0)	//Z轴方向应是向外的
				ucs.axis_z = -1.0*ucs.axis_z;
			normalize(ucs.axis_z);
			ucs.axis_y = ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
			// 5.结构图：图层视图需要计算坐标系原点
			if(!bSingleLineMap)
				ucs.origin=pLinePart->Start();
			break;
		}	
	}*/
	/*
	CLDSLinePart *pLinePart=NULL;
	CLDSPart *pPart = partset.GetFirst();
	while(1)
	{
		if(pPart==NULL)
			break;
		else if(pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		else
		{
			pPart = partset.GetNext();
			continue;
		}
		if(pLinePart->pEnd&&pLinePart->pStart)
			Sub_Pnt(vec,pLinePart->pEnd->pos,pLinePart->pStart->pos);
		else
			Sub_Pnt(vec,pLinePart->End(),pLinePart->Start());
		normalize(vec);
		double dd = fabs(vec*ucs.axis_x);
		if(dd<EPS_COS)//杆件不是沿X轴方向的杆件可以用叉积求Z轴
		{
			ucs.axis_z = vec^ucs.axis_x;
			if(ucs.axis_z*near_ucs.axis_z<0)	//Z轴方向应是向外的
				ucs.axis_z = -1.0*ucs.axis_z;
			ucs.axis_y = ucs.axis_z^ucs.axis_x;
			break;
		}
		else	//此杆件与X轴平行
			pPart=partset.GetNext();//利用下一根不与X轴平行的杆件进行计算
	}*/
	return ucs;
}

#ifdef __SMART_DRAW_INC_
static void DrawAngleJointLine(ACADDRAW_CONTEXT context,
	CLDSLinePart *pRod,CDrawCoordTrans& dcs,CDrawingComponent* pDrawComponent,
	LINEPARTSET* pRayRods,AcDbObjectId *pStartJointId,AcDbObjectId *pEndJointId)
{
	if(pRod==NULL||pRod->pStart==NULL||pRod->pEnd==NULL||pRod->size_idClassType!=CLS_LINEANGLE)
		return;
	CTower* pTower=(CTower*)pRod->BelongModel();
	for(int i=0;i<2;i++)
	{
		f3dPoint wing_vec,pole_vec,line_start,line_end;
		long idAngleJoint=0;
		if(i==0)	//绘制始端接头
		{
			idAngleJoint=pRod->connectStart.idAngleJoint;
			Sub_Pnt(pole_vec,pRod->pEnd->Position(false),pRod->pStart->Position(false));
			normalize(pole_vec);
			Add_Pnt(line_start,pRod->pStart->Position(false),pole_vec*(sys.nJointLineLen/2.0));
			Sub_Pnt(line_end,pRod->pStart->Position(false),pole_vec*(sys.nJointLineLen/2.0));
		}
		else
		{
			idAngleJoint=pRod->connectEnd.idAngleJoint;
			Sub_Pnt(pole_vec,pRod->pStart->Position(false),pRod->pEnd->Position(false));
			normalize(pole_vec);
			Add_Pnt(line_start,pRod->pEnd->Position(false),pole_vec*(sys.nJointLineLen/2.0));
			Sub_Pnt(line_end,pRod->pEnd->Position(false),pole_vec*(sys.nJointLineLen/2.0));
		}
		ANGLE_JOINT* pJoint=pTower->hashAngleJoint.GetValue(idAngleJoint);
		if(pJoint==NULL)
			continue;
		else if(pDrawComponent&&((CLineView*)pDrawComponent)->IsItemDrawn(CLineView::TYPE_JOINT,idAngleJoint))
			continue;	//该接头在当前视图中已绘制过
		else if(pDrawComponent)	//如果执行Rerun重绘构件时，可能存在问题(接头不会重新绘制) wht 14-07-12
			((CLineView*)pDrawComponent)->MarkItemDrawnState(CLineView::TYPE_JOINT,idAngleJoint);
		if(pRod->GetClassTypeId()==CLS_LINEANGLE)
		{	//确定角钢当前工作肢
			UCS_STRU ucs=dcs.cs;
			CLDSLineAngle *pBaseJg=(CLDSLineAngle*)pRod;
			if(dot_prod(ucs.axis_z,pBaseJg->get_norm_x_wing())>
				dot_prod(ucs.axis_z,pBaseJg->get_norm_y_wing()))
				wing_vec = pBaseJg->get_norm_y_wing();
			else
				wing_vec = pBaseJg->get_norm_x_wing();
		}
		else if(pRayRods)
		{
			f3dPoint ray_vec;
			CLDSLinePart *pValidRayRod=NULL,*pRayRod=NULL;
			int nPush=pRayRods->push_stack();
			for(pRayRod=pRayRods->GetFirst();pRayRod;pRayRod=pRayRods->GetNext())
			{
				if(pRayRod->pStart==NULL||pRayRod->pEnd==NULL)
					continue;
				if(pRayRod->layer(1)=='Z'||pRayRod->layer(1)=='z')
					continue;
				if(pRayRod->pStart->handle==pRod->pStart->handle
					||pRayRod->pStart->handle==pRod->pEnd->handle)
				{
					if(pRayRod->iSeg.iSeg==pRod->iSeg.iSeg)
					{
						pRayRods->pop_stack(nPush);
						return;
					}
					pValidRayRod=pRayRod;
					ray_vec=pRayRod->pEnd->Position(false)-pRayRod->pStart->Position(false);
					break;
				}
				else if(pRayRod->pEnd->handle==pRod->pStart->handle
					||pRayRod->pEnd->handle==pRod->pEnd->handle)
				{
					if(pRayRod->iSeg.iSeg==pRod->iSeg.iSeg)
					{
						pRayRods->pop_stack(nPush);
						return;
					}
					pValidRayRod=pRayRod;
					ray_vec=pRayRod->pStart->Position(false)-pRayRod->pEnd->Position(false);
					break;
				}
			}
			pRayRods->pop_stack(nPush);
			if(pValidRayRod)
			{
				f3dPoint work_norm=pole_vec^ray_vec;
				wing_vec=work_norm^pole_vec;
				normalize(wing_vec);
				if(wing_vec*ray_vec>0)
					wing_vec*=-1.0;
			}
		}
		else
		{
			wing_vec=pole_vec^dcs.cs.axis_z;
			normalize(wing_vec);
		}
		Add_Pnt(line_start,line_start,wing_vec*sys.nJointLineOffset);
		Add_Pnt(line_end,line_end,wing_vec*sys.nJointLineOffset);
		line_start=dcs.TransToDraw(line_start);
		line_end=dcs.TransToDraw(line_end);
		line_start.z=line_end.z=0;
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		AcDbObjectId jointId=CreateAcadPolyLine(context,line_start,line_end,sys.stru_map.fPolyLineWidth,idAngleJoint);
		if(i==0&&pStartJointId!=NULL)
			*pStartJointId=jointId;
		else if(i==1&&pEndJointId!=NULL)
			*pEndJointId=jointId;
	}
}
#endif 

static void DrawRodJointLine(ACADDRAW_CONTEXT context,CLDSLinePart *pPole,CDrawCoordTrans& dcs,PARTSET &partset)
{
	if(pPole==NULL)
		return;
	if(pPole->pStart==NULL||pPole->pEnd==NULL)
		return;
#ifndef __TSA_FILE_
	f3dPoint wing_vec,pole_vec,line_start,line_end;
	Sub_Pnt(pole_vec,pPole->pEnd->Position(false),pPole->pStart->Position(false));
	normalize(pole_vec);
	Add_Pnt(line_start,pPole->pStart->Position(false),pole_vec*(sys.nJointLineLen/2.0));
	Sub_Pnt(line_end,pPole->pStart->Position(false),pole_vec*(sys.nJointLineLen/2.0));
	if(pPole->IsAngle())
	{
		UCS_STRU ucs=dcs.cs;
		CLDSLineAngle *pBaseJg=(CLDSLineAngle*)pPole;
		if(fabs(ucs.axis_z*pBaseJg->vxWingNorm)>fabs(ucs.axis_z*pBaseJg->vyWingNorm))
			wing_vec =-pBaseJg->GetWingVecX();
		else
			wing_vec =-pBaseJg->GetWingVecY();
		if( pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&(pBaseJg->Layer(0)=='S'||pBaseJg->Layer(0)=='L')&&
			pBaseJg->size_cSubClassType!='T'&&pBaseJg->Layer(1)=='Z')
		{	//塔身或腿组合角钢主材
			f3dPoint vVerifyVec=GEPOINT(pBaseJg->xPosStart.x,pBaseJg->xPosStart.y);
			if(wing_vec*vVerifyVec<0)
				wing_vec*=-1.0;
		}
	}
	else if(pPole->GetClassTypeId()==CLS_LINETUBE)
	{
		f3dPoint ray_vec;
		CLDSPart *pPart=NULL;
		CLDSLinePart *pLinePart=NULL,*pRayPole=NULL;
		int nPush=partset.push_stack();
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(!pPart->IsLinePart())
				continue;
			pLinePart=(CLDSLinePart*)pPart;
			if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
				continue;
			if(pLinePart->layer(1)=='Z'||pLinePart->layer(1)=='z')
				continue;
			if(pLinePart->pStart->handle==pPole->pStart->handle
				||pLinePart->pStart->handle==pPole->pEnd->handle)
			{
				if(pLinePart->iSeg.iSeg==pPole->iSeg.iSeg)
				{
					partset.pop_stack(nPush);
					return;
				}
				pRayPole=pLinePart;
				ray_vec=pLinePart->pEnd->Position(false)-pLinePart->pStart->Position(false);
				break;
			}
			else if(pLinePart->pEnd->handle==pPole->pStart->handle
				||pLinePart->pEnd->handle==pPole->pEnd->handle)
			{
				if(pLinePart->iSeg.iSeg==pPole->iSeg.iSeg)
				{
					partset.pop_stack(nPush);
					return;
				}
				pRayPole=pLinePart;
				ray_vec=pLinePart->pStart->Position(false)-pLinePart->pEnd->Position(false);
				break;
			}
		}
		partset.pop_stack(nPush);
		if(pRayPole)
		{
			f3dPoint work_norm=pole_vec^ray_vec;
			wing_vec=work_norm^pole_vec;
			normalize(wing_vec);
			if(wing_vec*ray_vec>0)
				wing_vec*=-1.0;
		}
	}
	Add_Pnt(line_start,line_start,wing_vec*sys.nJointLineOffset);
	Add_Pnt(line_end,line_end,wing_vec*sys.nJointLineOffset);
	line_start=dcs.TransToDraw(line_start);
	line_end=dcs.TransToDraw(line_end);
	line_start.z=line_end.z=0;
	GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
	CreateAcadLine(context,line_start,line_end);
#endif
}

#ifdef __SMART_DRAW_INC_
//绘制杆件单线图,返回杆件ID以及绘制节点的ID
BOOL DrawRodLine(CLDSLinePart *pLinePart,CDrawCoordTrans& dcs,BOOL bSumMap,BOOL bDimByAnalysisMode,
							AcDbObjectId *pPoleId,AcDbObjectId *pStartId,AcDbObjectId *pEndId,
							AcDbObjectId *pStartJointId,AcDbObjectId *pEndJointId,CDrawingCommand *pCmd/*=NULL*/)
{
	if(pLinePart->m_bVirtualPart)	//隐含杆件不统计
		return FALSE;
	if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
		return FALSE;
	f3dPoint org_start,org_end,point;
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	ACADDRAW_CONTEXT context(pBlockTableRecord);
	CStackAcadVar stackEcho("CMDECHO",0);
	//转至单线图层
#ifdef _ARX_2007
	acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::VisibleProfileLayer.layerName),RTSTR,L"",RTNONE);
#else
	acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::VisibleProfileLayer.layerName,RTSTR,"",RTNONE);
#endif
	if(!pCmd->m_paraLinePart.bAdjoinRod)
		context.currLayerId=LayerTable::VisibleProfileLayer.layerId;
	else
		context.currLayerId=LayerTable::UnvisibleProfileLayer.layerId;
	//绘制塔身以及腿部主材接头标志线 wht 10-12-22
	char cFirst=pLinePart->layer(0);
	char cSec=pLinePart->layer(1);
	PARTSET partset;
	if(pLinePart->size_idClassType==CLS_LINEANGLE&&bDimByAnalysisMode)
	{	//角钢构件目前可暂通过接头设计表进行绘制
		CTower* pTower=(CTower*)pLinePart->BelongModel();
		if( pTower->hashAngleJoint.GetValue(pLinePart->connectStart.idAngleJoint)!=NULL||
			pTower->hashAngleJoint.GetValue(pLinePart->connectEnd.idAngleJoint)!=NULL)
		{	//将来完善CDrawingComponent相关代码后，需要启用DrawAngleJointLine wjh-2014.7.1
			DrawAngleJointLine(pBlockTableRecord,pLinePart,dcs,pCmd->m_pDrawingCom,NULL,pStartJointId,pEndJointId);
			//DrawRodJointLine(pBlockTableRecord,pLinePart,dcs,partset);
		}
	}
	else if(sys.bDrawMainPoleJointLine&&towupper(cSec)=='Z'&&towupper(cFirst)!='T')	
	{	
		if(pCmd&&pCmd->m_pDrawingCom)
		{	//根据传入的pCmd构件partset集合用于绘制分段标识符 wht 11-07-13
			CLDSPart *pPart=NULL;
			for(pPart=pCmd->m_pDrawingCom->GetFirstPart();pPart;pPart=pCmd->m_pDrawingCom->GetNextPart())
				partset.append(pPart);
		}
		if(pLinePart->size_idClassType!=CLS_LINEANGLE||((CLDSLineAngle*)pLinePart)->group_father_jg_h<0x20)
			DrawRodJointLine(pBlockTableRecord,pLinePart,dcs,partset);
	}

	if(towupper(cSec)=='F')
		GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
	else
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	/*
#ifndef __TSA_FILE_
	if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle* pJg=(CLDSLineAngle*)pLinePart;
		if(!bSumMap)	//分段图
		{
#if defined(__TSA_FILE_)
			bDimByAnalysisMode=TRUE;		//节点
#endif
			if (bDimByAnalysisMode==FALSE)
			{
				JGZJ jgzj;
				getjgzj(jgzj,pJg->GetWidth());
				UCS_STRU ucs=dcs.cs;
				//获得用户坐标系下的角钢起始点
				if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&((CLDSGroupLineAngle*)pJg)->group_style!=1)
				{
					org_start=pJg->Start();
					org_end=pJg->End();
				}
				if(fabs(pJg->get_norm_x_wing()*ucs.axis_z)>fabs(pJg->get_norm_y_wing()*ucs.axis_z))
				{	//X肢为当前工作肢 此处暂不支持指定肢绘制的情况(即暂不考虑pJg->feature) wht 11-07-27
					if(pJg->m_bEnableTeG)
						jgzj=pJg->xWingXZhunJu;	
					if(pJg->pStart)
						org_start= pJg->GetDatumPosBer(pJg->pStart)+pJg->GetWingVecX()*jgzj.g;
					else
						org_start= pJg->Start()+pJg->GetWingVecX()*jgzj.g;
					if(pJg->pEnd)
						org_end  = pJg->GetDatumPosBer(pJg->pEnd)+pJg->GetWingVecX()*jgzj.g;
					else
						org_end= pJg->End()+pJg->GetWingVecX()*jgzj.g;
				}
				else if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&pJg->group_father_jg_h>0x20) //pJg->IsSubJgType(TA_JG_GROUP))
				{	//T型组合角钢的Y肢为当前工作肢
					org_start=pJg->Start();
					org_end=pJg->End();
				}
				else
				{	//Y肢为当前工作肢
					if(pJg->m_bEnableTeG)
						jgzj=pJg->xWingYZhunJu;	
					if(pJg->pStart)
						org_start= pJg->GetDatumPosBer(pJg->pStart)+pJg->GetWingVecY()*jgzj.g;
					else
						org_start= pJg->Start()+pJg->GetWingVecY()*jgzj.g;
					if(pJg->pEnd)
						org_end  = pJg->GetDatumPosBer(pJg->pEnd)+pJg->GetWingVecY()*jgzj.g;
					else
						org_end= pJg->End()+pJg->GetWingVecY()*jgzj.g;
				}
			}
			else
			{
				org_start=pJg->pStart->Position(false);
				org_end=pJg->pEnd->Position(false);
			}
		}
		else
		{
			org_start=pJg->pStart->xPreliftPos;
			org_end=pJg->pEnd->xPreliftPos;
		}
	}
	else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		org_start = pLinePart->Start();
		org_end = pLinePart->End();
	}	
	else
#endif
	{
		org_start=pLinePart->pStart->Position(false);
		org_end=pLinePart->pEnd->Position(false);
	}*/
	if(pLinePart->pStart)
		org_start=pLinePart->pStart->xPreliftPos;
	else
		org_start=pLinePart->Start();
	if(pLinePart->pEnd)
		org_end=pLinePart->pEnd->xPreliftPos;
	else
		org_end=pLinePart->End();

	//获得用户坐标系下的杆件起始点
	org_start=dcs.TransToDraw(org_start);
	//获得用户坐标系下的杆件终止点
	org_end=dcs.TransToDraw(org_end);
	//将三维图投影到X-Y平面上
	org_start.z=org_end.z=0;

	/*//绘制减腿
	char cThird=pLinePart->layer(2);
	char cPos=CalPoleQuad(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false));
	if ((pCmd->m_pDrawingCom->m_pDrawing->m_nSegI.iSeg==-1||pCmd->m_pDrawingCom->m_pDrawing->m_nSegI.iSeg==-2)//司令图、总图
		&&towupper(cFirst)=='L'&&towupper(cSec)=='X'&&cPos=='1')
	{
		CLDSModule *pModule=pCmd->m_pDrawingCom->m_pModule;
		CLDSLinePart *pMostLongLeg=(CLDSLinePart*)Ta.FromHandle(pModule->m_hMostLongLeg);
		f3dPoint dim_pos;
		double nSpace=0.5;		//下划线与标注之间的间隙
		if (pLinePart->Start().z>pLinePart->End().z)
			dim_pos=org_start;
		else
			dim_pos=org_end;
		dim_pos.Set(dim_pos.x,dim_pos.y+nSpace);
		CXhChar50 dim_text;
		if (pMostLongLeg->Start().z>pMostLongLeg->End().z)
			dim_text.Printf("-%f",pMostLongLeg->pStart->Position(false).z-pLinePart->pStart->Position(false).z);
		else
			dim_text.Printf("-%f",pMostLongLeg->pEnd->Position(false).z-pLinePart->pEnd->Position(false).z);
		SimplifiedNumString(dim_text);
		f3dLine line;
		double nLineLen=dim_text.Length()*sys.dim_map.fOtherTextSize*0.7+nSpace;
		line.startPt.Set(dim_pos.x,dim_pos.y-nSpace);
		if (cThird=='Q')
		{
			DimText(pBlockTableRecord,dim_pos,dim_text,TextStyleTable::hzfs.textStyleId,
				sys.dim_map.fOtherTextSize,0,AcDb::kTextLeft,AcDb::kTextBottom);
			line.endPt.Set(dim_pos.x+nLineLen,line.startPt.y);
		}
		else if(cThird=='Y')
		{
			DimText(pBlockTableRecord,dim_pos,dim_text,TextStyleTable::hzfs.textStyleId,
				sys.dim_map.fOtherTextSize,0,AcDb::kTextRight,AcDb::kTextBottom);
			line.endPt.Set(dim_pos.x-nLineLen,line.startPt.y);
		}
		CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	}*/
	AcDbObjectId LineId,pointId;//定义标识符
	//根据设定选择线型 wht 12-06-02
	BOOL bUsePolyLine=FALSE;
	if(pCmd->m_pDrawingCom->ParentDrawing()->m_nSegI.iSeg==-1)
		bUsePolyLine=CLineView::m_bUsePolyLineChiefMap;
	else 
		bUsePolyLine=CLineView::m_bUsePolyLineSumMap;
	if(bUsePolyLine)
		LineId=CreateAcadPolyLine(context,org_start,org_end,sys.stru_map.fPolyLineWidth,pLinePart->handle);
	else 
		LineId=CreateAcadLine(context,org_start,org_end,pLinePart->handle);
	if(pPoleId)
		*pPoleId=LineId;
	//
	if(pLinePart->pStart&&!pLinePart->pStart->is_visible)	//没绘制的节点
	{	//创建ACADPOINT对象
		pointId=InsertExtensionRecord(pBlockTableRecord,org_start,pLinePart->pStart->handle);
		if(pStartId)
			*pStartId=pointId;
		pLinePart->pStart->is_visible=TRUE;
	}
	if(pLinePart->pEnd&&!pLinePart->pEnd->is_visible)	//没绘制的节点
	{	//创建ACADPOINT对象
		pointId=InsertExtensionRecord(pBlockTableRecord,org_end,pLinePart->pEnd->handle);
		if(pEndId)
			*pEndId=pointId;
		pLinePart->pEnd->is_visible=TRUE;
	}
	pBlockTableRecord->close();//关闭块表记录
	return TRUE;
}
#endif
void DrawSingLine3DMap(PARTSET& partset,UCS_STRU ucs,double scale)
{
	AcDbObjectId LineId,pointId;//定义标识符
	AcGePoint3d start,end;
	AcDbXrecord *pXrec;
	AcDbObjectId dictObjId, xrecObjId;
	AcDbDictionary *pDict;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pBlockTableRecord==NULL)
		return;
	CStackAcadVar stackEcho("CMDECHO",0);
	//转至单线图层
#ifdef _ARX_2007
	acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::VisibleProfileLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);	//显示点
#else
	acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::VisibleProfileLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);	//显示点
#endif
	//绘制杆件单线，绘制前需先将原铁塔坐标系转换到用户坐标系ucs下
	f3dPoint rgn_vert[4];
	CLDSLinePart *pLinePart = NULL;
	rgn_vert[0].z=rgn_vert[1].z=rgn_vert[2].z=rgn_vert[3].z=0;
	CLDSPart *pPart=NULL;
	for(pPart=partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		if(pPart && pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		else
			continue;
		if(pLinePart->pStart)
			pLinePart->pStart->is_visible=TRUE;
		if(pLinePart->pEnd)
			pLinePart->pEnd->is_visible=TRUE;
	}
	BOOL bFirstSelect = TRUE;
	for(pPart = partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		f3dPoint org_start,org_end,point;
		if(pPart && pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		else
			continue;
		if(pLinePart->m_bVirtualPart)	//隐含杆件不统计
			continue;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		char cFirst=pLinePart->layer(0);
		char cSec=pLinePart->layer(1);
		if(towupper(cSec)=='F')
			GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
		else
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		//起点
		org_start=pLinePart->pStart->Position(false);
		point = org_start;
		coord_trans(point,ucs,FALSE);
		point = point*(1.0/scale);
		Cpy_Pnt(start,point);
		//终点
		org_end=pLinePart->pEnd->Position(false);
		point = org_end;
		coord_trans(point,ucs,FALSE);
		point = point*(1.0/scale);
		Cpy_Pnt(end,point);
		//创建ACADLINE对象
		AcDbLine *pLine=new AcDbLine(start,end);//创建LINE对象
		//设置角钢线颜色
		if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pLine))//将实体写入块表记录
		{
			pLine->createExtensionDictionary();
			pLine->close();//调用关闭对象的成员函数
		}
		dictObjId = pLine->extensionDictionary();
		acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
		pXrec = new AcDbXrecord;
#ifdef _ARX_2007
		pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
		pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
		pDict->close();
		struct resbuf *head;
		head = acutBuildList(AcDb::kDxfInt32, pLinePart->handle,0);
		pXrec->setFromRbChain(*head);
		acutRelRb(head);
		pXrec->close();
		if(pLinePart->pStart&&!pLinePart->pStart->is_visible)	//没绘制的节点
		{	//附近没有点
			//创建ACADPOINT对象
			AcDbPoint *pPoint=new AcDbPoint(start);//创建LINE对象
			//设置角钢线颜色
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))//将实体写入块表记录
			{
				pPoint->createExtensionDictionary();
				pPoint->close();//调用关闭对象的成员函数
			}
			dictObjId = pPoint->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32,pLinePart->pStart->handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
			pLinePart->pStart->is_visible=TRUE;
		}
		if(pLinePart->pEnd&&!pLinePart->pEnd->is_visible)	//没绘制的节点
		{	//附近没有点
			//创建ACADPOINT对象
			AcDbPoint *pPoint=new AcDbPoint(end);//创建LINE对象
			//设置角钢线颜色
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))//将实体写入块表记录
			{
				pPoint->createExtensionDictionary();
				pPoint->close();//调用关闭对象的成员函数
			}
			ads_name ent_name;
			acdbGetAdsName(ent_name,pointId);
			acdbEntUpd(ent_name);
			
			dictObjId = pPoint->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32,pLinePart->pEnd->handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
			pLinePart->pEnd->is_visible=TRUE;
		}
	}
	pBlockTableRecord->close();
}
void DrawSingleLineZhunMap(PARTSET& partset,NODESET& nodeset,UCS_STRU ucs,double scale,BOOL bSumMap/*=FALSE*/)
{
	AcDbObjectId LineId,pointId;//定义标识符
	AcGePoint3d start,end;

	InitDrawingEnv();
	AcDbXrecord *pXrec;
	AcDbObjectId dictObjId, xrecObjId;
	AcDbDictionary *pDict;
	CStackAcadVar stackEcho("CMDECHO",0);
	//转至单线图层
#ifdef _ARX_2007
	acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::VisibleProfileLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);	//显示点
#else
	acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::VisibleProfileLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);	//显示点
#endif
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表
	//
	BOOL bDimByAnalysisMode=FALSE;	//是否以应力简图形式标注
	//绘制杆件单线，绘制前需先将原铁塔坐标系转换到用户坐标系ucs下
	f3dPoint rgn_vert[4];
	CLDSLinePart *pLinePart = NULL;
	rgn_vert[0].z=rgn_vert[1].z=rgn_vert[2].z=rgn_vert[3].z=0;
	CLDSPart *pPart=NULL;
	for(pPart=partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		if(pPart && pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		else
			continue;
		if(pLinePart->pStart)
			pLinePart->pStart->is_visible=FALSE;
		if(pLinePart->pEnd)
			pLinePart->pEnd->is_visible=FALSE;
	}
	BOOL bFirstSelect = TRUE;
	for(pPart = partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		f3dPoint org_start,org_end,point;
		if(pPart && pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		else
			continue;
		if(pLinePart->m_bVirtualPart)	//隐含杆件不统计
			continue;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		//绘制塔身以及腿部主材接头标志线 wht 10-12-22
		char cFirst=pLinePart->layer(0);
		char cSec=pLinePart->layer(1);
		if(sys.bDrawMainPoleJointLine&&bSumMap&&towupper(cSec)=='Z'&&towupper(cFirst)!='T')	
			DrawRodJointLine(pBlockTableRecord,pLinePart,CDrawCoordTrans(ucs,scale),partset);
		if(towupper(cSec)=='F')
			GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
		else
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
#ifndef __TSA_FILE_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle* pJg=(CLDSLineAngle*)pLinePart;
			if(!bSumMap)	//分段图
			{
#if defined(__TSA_FILE_)
				bDimByAnalysisMode=TRUE;		//节点	
#elif defined(__LDS_FILE_)
				if (bFirstSelect)
				{
#ifdef _ARX_2007
					ACHAR result[20]={0};
#ifdef AFX_TARG_ENU_ENGLISH
					acedGetString(FALSE,L"Draw structure map model or [Mechanical sketch model(S)]:",result);
#else
					acedGetString(FALSE,L"绘制结构图模式或 [力学简图模式(S)]:",result);
#endif
					if(wcsicmp(result,L"S")==0)
#else
					char result[20]="";
#ifdef AFX_TARG_ENU_ENGLISH
					acedGetString(FALSE,"Draw structure map model or [Mechanical sketch model(S)]:",result);
#else
					acedGetString(FALSE,"绘制结构图模式或 [力学简图模式(S)]:",result);
#endif
					if(stricmp(result,"S")==0)
#endif
						bDimByAnalysisMode=TRUE;
					else
						bDimByAnalysisMode=FALSE;
					bFirstSelect = FALSE;
				}
				#else //defined(__LMA_FILE_)||defined(__TMA_FILE_)
					bDimByAnalysisMode=FALSE;
#endif
				if (bDimByAnalysisMode==FALSE)
				{
					JGZJ jgzj;
					getjgzj(jgzj,pJg->GetWidth());
					//获得用户坐标系下的角钢起始点
					if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&((CLDSGroupLineAngle*)pJg)->group_style!=1)
					{
						org_start=pJg->Start();
						org_end=pJg->End();
					}
					if(pJg->feature==1||(pJg->feature==0&&fabs(pJg->get_norm_x_wing()*ucs.axis_z)
						>fabs(pJg->get_norm_y_wing()*ucs.axis_z)))
					{	//X肢为当前工作肢
						if(pJg->m_bEnableTeG)
							jgzj=pJg->xWingXZhunJu;				
						if(pJg->pStart)
							org_start= pJg->GetDatumPosBer(pJg->pStart)+pJg->GetWingVecX()*jgzj.g;
						else
							org_start= pJg->Start()+pJg->GetWingVecX()*jgzj.g;
						if(pJg->pEnd)
							org_end  = pJg->GetDatumPosBer(pJg->pEnd)+pJg->GetWingVecX()*jgzj.g;
						else
							org_end= pJg->End()+pJg->GetWingVecX()*jgzj.g;
					}
					else if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&pJg->group_father_jg_h>0x20) //pJg->IsSubJgType(TA_JG_GROUP))
					{	//T型组合角钢的Y肢为当前工作肢
						org_start=pJg->Start();
						org_end=pJg->End();
					}
					else
					{	//Y肢为当前工作肢
						if(pJg->m_bEnableTeG)
							jgzj=pJg->xWingYZhunJu;	
						if(pJg->pStart)
							org_start= pJg->GetDatumPosBer(pJg->pStart)+pJg->GetWingVecY()*jgzj.g;
						else
							org_start= pJg->Start()+pJg->GetWingVecY()*jgzj.g;
						if(pJg->pEnd)
							org_end  = pJg->GetDatumPosBer(pJg->pEnd)+pJg->GetWingVecY()*jgzj.g;
						else
							org_end= pJg->End()+pJg->GetWingVecY()*jgzj.g;
					}
				}
				else
				{
					org_start=pJg->pStart->Position(false);
					org_end=pJg->pEnd->Position(false);
				}
			}
			else
			{
				org_start=pJg->pStart->Position(false);
				org_end=pJg->pEnd->Position(false);
			}
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			org_start = pLinePart->Start();
			org_end = pLinePart->End();
		}	
		else
#endif
		{
			org_start=pLinePart->pStart->Position(false);
			org_end=pLinePart->pEnd->Position(false);
		}
		//获得用户坐标系下的杆件起始点
		point = org_start;
		coord_trans(point,ucs,FALSE);
		point = point*(1.0/scale);
		Cpy_Pnt(start,point);
		//获得用户坐标系下的杆件终止点
		point = org_end;
		coord_trans(point,ucs,FALSE);
		point = point*(1.0/scale);
		Cpy_Pnt(end,point);
		//将三维图投影到X-Y平面上
		start.z = 0;
		end.z   = 0;
		//创建ACADLINE对象
		AcDbLine *pLine=new AcDbLine(start,end);//创建LINE对象
		//设置角钢线颜色
		if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pLine))//将实体写入块表记录
		{
			pLine->createExtensionDictionary();
			pLine->close();//调用关闭对象的成员函数
		}
		dictObjId = pLine->extensionDictionary();
		acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
		pXrec = new AcDbXrecord;
#ifdef _ARX_2007
		pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
		pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
		pDict->close();
		
		struct resbuf *head;
		head = acutBuildList(AcDb::kDxfInt32, pLinePart->handle,0);
		pXrec->setFromRbChain(*head);
		acutRelRb(head);
		pXrec->close();
		if(pLinePart->pStart&&!pLinePart->pStart->is_visible)	//没绘制的节点
		{	//附近没有点
			//创建ACADPOINT对象
			AcDbPoint *pPoint=new AcDbPoint(start);//创建LINE对象
			//设置角钢线颜色
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))//将实体写入块表记录
			//
			{
				pPoint->createExtensionDictionary();
				pPoint->close();//调用关闭对象的成员函数
			}
			dictObjId = pPoint->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32,pLinePart->pStart->handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
			pLinePart->pStart->is_visible=TRUE;
		}
		if(pLinePart->pEnd&&!pLinePart->pEnd->is_visible)	//没绘制的节点
		{	//附近没有点
			//创建ACADPOINT对象
			AcDbPoint *pPoint=new AcDbPoint(end);//创建LINE对象
			//设置角钢线颜色
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))//将实体写入块表记录
			{
				pPoint->createExtensionDictionary();
				pPoint->close();//调用关闭对象的成员函数
			}
			ads_name ent_name;
			acdbGetAdsName(ent_name,pointId);
			acdbEntUpd(ent_name);
			
			dictObjId = pPoint->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32,pLinePart->pEnd->handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
			pLinePart->pEnd->is_visible=TRUE;
		}
	}
	f3dLine joint_line;
	for(CLDSNode* pNode=nodeset.GetFirst();pNode!=NULL;pNode=nodeset.GetNext())
	{
		//绘制主材接头符号
		/*
		if(GetJointLine(joint_line,pNode,&ucs))
		{		//此节点如有接头,绘出接头标记
			joint_line.startPt = joint_line.startPt*(1.0/scale);
			joint_line.endPt   = joint_line.endPt*(1.0/scale);
			Cpy_Pnt(start,joint_line.startPt);
			Cpy_Pnt(end,joint_line.endPt);
			start.z = end.z = 0;
			AcDbLine *pLine=new AcDbLine(start,end);//创建LINE对象
			DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pLine);
			pLine->close();
		}
		*/
	}
	pBlockTableRecord->close();//关闭块表记录
#ifdef _ARX_2007
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"1",RTNONE);	//显示点
#else
	acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);	//显示点
#endif
}

#ifndef __TSA_FILE_
//计算构件绘制方式,传入定制视图用于展开绘制 wht 11-07-22
int CalPartDrawType(CLDSPart *pPart,UCS_STRU dcs,CDisplayView *pDefinedView,CDrawPartTagInfo *pDrawInfo/*=NULL*/)
{
	int h0v1e2=-1;	//0.平铺绘制 1.侧向绘制 2.绘制端面
	CViewPart *pViewPart=NULL;
	if(pDefinedView&&pDefinedView->m_iViewType==1)
		pViewPart=pDefinedView->FindViewPart(pPart->handle);
	if(pPart->IsAngle())
	{
		int x_wing0_y_wing1=-1;
		CLDSLineAngle *pJg=(CLDSLineAngle*)pPart;
		f3dPoint cur_jg_vec=pJg->End()-pJg->Start();
		normalize(cur_jg_vec);
		BOOL bInside=-1;
		if(pViewPart&&(pViewPart->cFlag=='X'||pViewPart->cFlag=='S'))
			x_wing0_y_wing1=0;	//X肢为当前绘制肢
		else if(pViewPart&&(pViewPart->cFlag=='Y'||pViewPart->cFlag=='Q'))
			x_wing0_y_wing1=1;	//Y肢为当前绘制肢
		else if((pViewPart&&pViewPart->cFlag=='Z')||fabs(cur_jg_vec*dcs.axis_z)>0.70710)
			x_wing0_y_wing1=2;
		else
			bInside=IsInsideJg(pJg,dcs.axis_z,&x_wing0_y_wing1);
		if(bInside==-1&&x_wing0_y_wing1==0)
		{
			if(pJg->get_norm_x_wing()*dcs.axis_z>0)
				bInside=TRUE;
			else
				bInside=FALSE;
		}
		else if(bInside==-1&&x_wing0_y_wing1==1)
		{
			if(pJg->get_norm_y_wing()*dcs.axis_z>0)
				bInside=TRUE;
			else
				bInside=FALSE;
		}
		if(pJg->m_bFootNail)
		{
			f3dPoint endPt=pJg->End();
			project_point(endPt,pJg->Start(),dcs.axis_y);
			f3dPoint cur_jg_vec=endPt-pJg->Start();
			normalize(cur_jg_vec);
			if(fabs(cur_jg_vec*dcs.axis_z)>0.70710)
				x_wing0_y_wing1=2;	//角钢脚钉截面
			else
				x_wing0_y_wing1=3;	//角钢脚钉正面
		}
		if(x_wing0_y_wing1==2)
			h0v1e2=1;
		else 
			h0v1e2=0;
		if(pDrawInfo)	//角钢当前绘制肢 wht 11-08-02
		{
			if(x_wing0_y_wing1==0&&bInside)
				pDrawInfo->m_cDrawStyle='X';
			else if(x_wing0_y_wing1==0&&!bInside)
				pDrawInfo->m_cDrawStyle='x';
			else if(x_wing0_y_wing1==1&&bInside)
				pDrawInfo->m_cDrawStyle='Y';
			else if(x_wing0_y_wing1==1&&!bInside)
				pDrawInfo->m_cDrawStyle='y';
			else
				pDrawInfo->m_cDrawStyle='Z';
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pPlate=(CLDSParamPlate*)pPart;
		if(pPlate->m_iParamType==TYPE_ELBOWPLATE)
		{
			double dd=fabs(pPlate->ucs.axis_z*dcs.axis_z);
			if(dd>EPS_COS2)
				h0v1e2=0;
			else if(dd<0.707)
				h0v1e2=1;
		}
		else
		{	//未指定绘制方式的非肘板参数化板，提前自动判断绘制方式
			if((pPlate->m_iParamType==TYPE_UEND||pPlate->m_iParamType==TYPE_ROLLEND)
				&&fabs(dcs.axis_z*pPlate->ucs.axis_x)>EPS_COS2)
				h0v1e2=2;		//槽型插板或者U型插板需要绘制端面图即竖直绘制
			else if(fabs(dcs.axis_z*pPlate->ucs.axis_z)>0.707)
				h0v1e2=0;	//平铺绘制
			else
				h0v1e2=1;	//侧向绘制
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		if(pViewPart&&pViewPart->cFlag=='X')		//平铺绘制
			h0v1e2=0;
		else if(pViewPart&&pViewPart->cFlag=='Z')	//竖直绘制
			h0v1e2=1;
		else if(pViewPart&&pViewPart->cFlag=='S'&&pPlate->face_N>1)	//展开绘制多面板
			h0v1e2=0;
		else	//未指定绘制方式，自动判断绘制方式
		{
			if(fabs(dcs.axis_z*pPlate->ucs.axis_z)>0.707)
				h0v1e2=0;	//水平绘制
			else
				h0v1e2=1;	//竖直绘制
		}
	}
	else if(pPart->IsTube())
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
		f3dPoint len_vec=pLineTube->End()-pLineTube->Start();
		normalize(len_vec);
		if(fabs(len_vec*dcs.axis_z)<0.707)
			h0v1e2=0;
		else
			h0v1e2=1;
	}
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
	{
		if(fabs(pPart->ucs.axis_z*dcs.axis_z)>0.707)
			h0v1e2=2;	//端面
		else if(fabs(pPart->ucs.axis_y*dcs.axis_z)>0.707)
			h0v1e2=0;	//平铺
		else 
			h0v1e2=1;	//侧面
	}
	else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
	{
		if(fabs(pPart->ucs.axis_x*dcs.axis_z)>0.707)
			h0v1e2=2;	//端面
		else if(fabs(pPart->ucs.axis_z*dcs.axis_z)>0.707)
			h0v1e2=0;	//平铺
		else
			h0v1e2=1;
	}
	else if(pPart->GetClassTypeId()==CLS_BOLT)
	{
		CLsListDraw boltDrawList;
		CLsDrawUnit *pDrawUnit=boltDrawList.AppendLsRef((CLDSBolt*)pPart,TRUE,dcs.axis_z);
		h0v1e2=pDrawUnit->front0_side1;
	}
	return h0v1e2;
}

AcDbObjectId DrawPart(AcDbBlockTableRecord *pBlockTableRec,CDrawPartInfo &drawPartInfo,CDrawCoordTrans dcs)	//绘制指定构件
{
	AcDbObjectId entId=0;
	if(drawPartInfo.pPart==NULL)
		return entId;
	if(drawPartInfo.pPart->GetClassTypeId()==CLS_LINEANGLE)
		DrawJgStru(pBlockTableRec,drawPartInfo,dcs,FALSE,TRUE,&entId);
	else if(drawPartInfo.pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		DrawGroupLineAngle(pBlockTableRec,(CLDSGroupLineAngle*)drawPartInfo.pPart,dcs.cs,dcs.m_fDrawScale,&entId);
	else if(drawPartInfo.pPart->GetClassTypeId()==CLS_LINETUBE)
		DrawTubeStru(pBlockTableRec,(CLDSLineTube*)drawPartInfo.pPart,dcs,FALSE,FALSE,&entId);
	else if(drawPartInfo.pPart->GetClassTypeId()==CLS_LINESLOT)
		DrawLineSlotStru(pBlockTableRec,(CLDSLineSlot*)drawPartInfo.pPart,dcs.cs,dcs.m_fDrawScale,FALSE,&entId);
	else if(drawPartInfo.pPart->GetClassTypeId()==CLS_LINEFLAT)
		DrawLineFlatStru(pBlockTableRec,(CLDSLineFlat*)drawPartInfo.pPart,dcs.cs,dcs.m_fDrawScale,FALSE,&entId);
	else if(drawPartInfo.pPart->GetClassTypeId()==CLS_PLATE)
		DrawPlateStruDrawing(pBlockTableRec,(CLDSPlate*)drawPartInfo.pPart,dcs,FALSE,&entId);
	else if(drawPartInfo.pPart->GetClassTypeId()==CLS_PARAMPLATE)
		DrawParamPlateStruDrawing(pBlockTableRec,(CLDSParamPlate*)drawPartInfo.pPart,dcs.cs,dcs.m_fDrawScale,FALSE,&entId);
	return entId;
}
#endif
