//<LOCALE_TRANSLATE/>
//构件图
#include "stdafx.h"
#include "stdarx.h"
#include "MenuFunc.h"
#include "Tower.h"
#include "SysPara.h"
#include "Query.h"
#include "JgDrawing.h"
#include "PlankDrawing.h"
#include "GlobalFunc.h"
#include "InputPartNoDlg.h"
#include "PaperPatternDrawing.h"
#include "TblDef.h"
#include "FormatTable.h"
#include "adstools.h"
#include "RxTools.h"
#include "ScrTools.h"
#include "NcPlank.h"
#include "DragEntSet.h"
#include "PlankPartParaExportDlg.h"
#include "ClassicJgPart.h"
#include "PartSetCreatorDlg.h"
#include "PartDrawingArraySpaceDlg.h"
#include "DrawTaMap.h"
#include "SpecialProcessStatCardDlg.h"
#include "TubeSpreadCurveExportDlg.h"
#include "StatStdPartGroupDlg.h"
#include "SetupAidedPlateDlg.h"
#include "MenuFunc.h"
#include "ComparePartNoString.h"
#include "StdPartSetDlg.h"
#include "ArrangeAndDrawPlateSetDlg.h"
#include "SetPartMapFilePathDlg.h"
#include "SortFunc.h"
#include "DistFillet.h"
#include "PolyJgBendDrawDlg.h"
#include "CutAngleBriefMap.h"
#include "InputAnValDlg.h"
#include "SearchBlankPosition.h"
#include ".\LdsDrwLocale.h"

#ifndef __TSA_FILE_
//#ifndef ZHEJIANGSHENGDA
//	#define ZHEJIANGSHENGDA
//#endif
//----------------------------------------------------------------------------------------
//This function from DrawTaMap.cpp
//static double PART_MAP_SCALE=1.0;	//实际构件图绘制比例
//double MAP_Y_AUG_COEF=1.0;
BOOL DrawJgMap(CRodDrawing *pJgDraw,f3dPoint origin,BOOL bDimStyle2=FALSE,AcDbBlockTableRecord *pExterBlockTableRecord=NULL);
static CPartSetCreatorDlg partSetDlg;
static CArrangeAndDrawPlateSetDlg setDlg;
//本文件中的公用函数
void NewTubeBreakFaceLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,f3dPoint end,double t=2.0)
{
	//均放在同一水平面内绘制
	start.z=end.z=0;

	f2dArc arc_start_left,arc_start_right;
	f3dPoint center,midPoint=0.5*(start+end);
	f3dPoint direction=midPoint-start;
	f3dPoint std_direction=direction.normalized();
	f3dPoint offset_vec(-direction.y,direction.x);
	normalize(offset_vec);
	double cosa=cos(Pi/6);
	double radius=direction.mod();
	//绘制始端左侧外壁圆弧
	center=start+0.5*direction-offset_vec*radius*cosa;
	arc_start_left.startPt.Set(midPoint.x,midPoint.y);
	arc_start_left.centre.Set(center.x,center.y);
	arc_start_left.angle=60*RADTODEG_COEF;
	CreateAcadArcLine(pBlockTableRecord,center,midPoint,60*RADTODEG_COEF);
	//绘制始端左侧内壁圆弧
	center=start+std_direction*t*0.5+0.5*direction-offset_vec*(radius-t)*cosa;
	arc_start_left.startPt.Set(midPoint.x,midPoint.y);
	arc_start_left.centre.Set(center.x,center.y);
	arc_start_left.angle=60*RADTODEG_COEF;
	CreateAcadArcLine(pBlockTableRecord,center,midPoint,60*RADTODEG_COEF);
	//绘制终端左侧圆弧,隐去暂不绘
	//center+=direction;
	//CreateAcadArcLine(pBlockTableRecord,center,end,60*RADTODEG_COEF);

	//绘制始端右侧圆弧
	center=start+0.5*direction+offset_vec*radius*cosa;
	arc_start_right.startPt.Set(start.x,start.y);
	arc_start_right.centre.Set(center.x,center.y);
	arc_start_right.angle=60*RADTODEG_COEF;
	CreateAcadArcLine(pBlockTableRecord,center,start,60*RADTODEG_COEF);
	//绘制终端右侧圆弧
	center+=direction;
	CreateAcadArcLine(pBlockTableRecord,center,midPoint,60*RADTODEG_COEF);
	//绘制始端右侧内壁圆弧
	center=start+std_direction*t*0.5+0.5*direction+offset_vec*(radius-t)*cosa;
	start+=std_direction*t;
	arc_start_right.startPt=start;
	arc_start_right.centre.Set(center.x,center.y);
	arc_start_right.angle=60*RADTODEG_COEF;
	CreateAcadArcLine(pBlockTableRecord,center,start,60*RADTODEG_COEF);
	//绘制断面斜影线
	/*double space=2;
	f3dPoint line_pick,line_vec,inters;
	f2dLine line;
	normalize(direction);
	line_vec=direction;
	RotateVectorAroundVector(line_vec,-Pi/4,f3dPoint(0,0,1));
	for(int i=1;i<(int)(radius/space);i++)
	{
		line_pick=start+direction*(i*space);
		line.startPt.Set(line_pick.x-line_vec.x*radius,line_pick.y-line_vec.y*radius);
		line.endPt.Set(line_pick.x+line_vec.x*radius,line_pick.y+line_vec.y*radius);
		Int2dla(line,arc_start_left,line.startPt.x,line.startPt.y,inters.x,inters.y);
		Int2dla(line,arc_start_right,line.endPt.x,line.endPt.y,inters.x,inters.y);
		CreateAcadLine(pBlockTableRecord,f3dPoint(line.startPt.x,line.startPt.y),f3dPoint(line.endPt.x,line.endPt.y));
	}*/
}
AcDbObjectId NewBoltCircle(AcDbBlockTableRecord *pBlockTableRecord,f3dCircle Cir,BOOL bFootNail=FALSE)
{
	AcDbObjectId circleId;
	AcGePoint3d acad_center(Cir.centre.x,Cir.centre.y,Cir.centre.z);//定义圆心
	AcGeVector3d normal(0.0,0.0,1.0);//定义法线矢量
	AcDbCircle *pCircle = new AcDbCircle(acad_center,normal,Cir.radius);
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,circleId,pCircle))
		pCircle->close();
	double extend=Cir.radius*0.5;
	CreateAcadLine(pBlockTableRecord,Cir.centre-f3dPoint(extend,0,0),Cir.centre+f3dPoint(extend,0,0));
	if(bFootNail)	//脚钉图符 wht 11-05-09
		CreateAcadLine(pBlockTableRecord,Cir.centre,Cir.centre+f3dPoint(0,extend,0));
	else			//普通螺栓图符
		CreateAcadLine(pBlockTableRecord,Cir.centre-f3dPoint(0,extend,0),Cir.centre+f3dPoint(0,extend,0));
	return circleId;
}

//生成腰圆孔 wht 10-02-25
BOOL NewWaistHole(AcDbBlockTableRecord *pBlockTableRecord,f3dCircle Cir,f3dPoint waist_vec,double waist_len)
{
	if(waist_len<=0||waist_vec.IsZero())
		return FALSE;
	double half_len=0.5*waist_len+Cir.radius;
	double radius=Cir.radius;
	normalize(waist_vec);
	if(waist_vec.x<0)	//保证腰圆孔方向与X轴方向相同
		waist_vec*=-1.0;
	f3dPoint vec(-waist_vec.y,waist_vec.x);	//vec垂直于腰圆方向
	f3dPoint topLeft=Cir.centre-waist_vec*half_len+vec*radius;
	CreateAcadRect(pBlockTableRecord,topLeft,2*half_len,2*radius,TRUE,NULL,waist_vec);
	double extend=Cir.radius*0.5;
	CreateAcadLine(pBlockTableRecord,Cir.centre-vec*extend,Cir.centre+vec*extend);
	CreateAcadLine(pBlockTableRecord,Cir.centre-waist_vec*extend,Cir.centre+waist_vec*extend);
	return TRUE;
}

//钢管简图列表
void DrawBriefTubeTable(TUBESET &tubeSet)
{
	int i=0,j=0;
	CLDSLineTube *pTempTube=NULL;
	int nPartNum=0,nPageNum=0,nSumPage=tubeSet.GetNodeNum()/18;
	if(tubeSet.GetNodeNum()%18>0)
		nSumPage+=1;
	f2dPoint tbl_pos;
	TUBESET tubePageSet;
	for(pTempTube=tubeSet.GetFirst();pTempTube;pTempTube=tubeSet.GetNext())
	{
		tubePageSet.append(pTempTube);
		nPartNum++;
		if(nPartNum%18==0||nPartNum==tubeSet.GetNodeNum())
		{	//分页显示构件
			nPageNum++;
			CFormatTbl xTubeBriefTbl;
			xTubeBriefTbl.Create(tubePageSet.GetNodeNum()+1,6);
			for(i=0;i<xTubeBriefTbl.GetRowsCount();i++)
			{
				if(i==0)
					xTubeBriefTbl.SetRowHigh(i,10);
				else 
					xTubeBriefTbl.SetRowHigh(i,16);
				for(j=0;j<xTubeBriefTbl.GetColumnsCount();j++)
					xTubeBriefTbl.GetGridAt(i,j)->feature = TEXTALIGN_MIDDLE_CENTER;
			}
			xTubeBriefTbl.SetColumnWide(0,25);
			xTubeBriefTbl.SetColumnWide(1,20);
			xTubeBriefTbl.SetColumnWide(2,10);
			xTubeBriefTbl.SetColumnWide(3,70);
			xTubeBriefTbl.SetColumnWide(4,25);
			xTubeBriefTbl.SetColumnWide(5,25);
#ifdef AFX_TARG_ENU_ENGLISH
			xTubeBriefTbl.SetGrid(0,0,"Part No.");
			xTubeBriefTbl.SetGrid(0,1,"Spec");
			xTubeBriefTbl.SetGrid(0,2,"Number Of Parts");
			xTubeBriefTbl.SetGrid(0,3,"Tube sketch");
			xTubeBriefTbl.SetGrid(0,4,"Slotting info");
			xTubeBriefTbl.SetGrid(0,5,"Notes");
#else
			xTubeBriefTbl.SetGrid(0,0,"零件编号");
			xTubeBriefTbl.SetGrid(0,1,"规  格");
			xTubeBriefTbl.SetGrid(0,2,"件数");
			xTubeBriefTbl.SetGrid(0,3,"钢管简图");
			xTubeBriefTbl.SetGrid(0,4,"开槽信息");
			xTubeBriefTbl.SetGrid(0,5,"备  注");
#endif
			double tbl_width=25+20+10+70+25+25;
			for(i=0;i<tubePageSet.GetNodeNum();i++)
			{	//不知道为什么不能显示此处的进度信息 wht 10-08-17
#ifdef AFX_TARG_ENU_ENGLISH
				PutMessage(NULL,"Totally %d pages,analyzing %d tube information on page %d ...",nSumPage,i+1,nPageNum);
#else
				PutMessage(NULL,"共%d页,正在分析第%d页第%d个钢管信息...",nSumPage,nPageNum,i+1);
#endif
				CLDSLineTube *pLineTube=tubePageSet[i];
				xTubeBriefTbl.SetGrid(i+1,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
				CXhChar50 sTemp;
				sTemp.Printf("Φ%.0fX%.0f",pLineTube->GetWidth(),pLineTube->GetThick());
				xTubeBriefTbl.SetGrid(i+1,1,sTemp);
				sTemp.Printf("%d",pLineTube->feature);
				xTubeBriefTbl.SetGrid(i+1,2,sTemp);
				sTemp.Empty();
				f3dPoint len_vec=pLineTube->End()-pLineTube->Start();
				normalize(len_vec);
				CXhChar50 sStartAngle,sEndAngle;
				for(j=0;j<2;j++)
				{
					TUBEJOINT *pTubeJoint=&pLineTube->m_tJointStart;
					if(j==1)
						pTubeJoint=&pLineTube->m_tJointEnd;
					CLDSParamPlate *pParamPlate=NULL;
					if((pTubeJoint->type==0||(pTubeJoint->type==1&&pTubeJoint->hViceLinkObj<0))
						&&!pTubeJoint->cutPlaneNorm.IsZero())
					{	//平面切割或对接相贯
						double cosa=fabs(pTubeJoint->cutPlaneNorm*len_vec);
						double fAngle=ACOS(cosa)*DEGTORAD_COEF;
						if(fAngle>90)
							fAngle=180-fAngle;
						fAngle=90-fAngle;
						if(j==0)
#ifdef AFX_TARG_ENU_ENGLISH
							sTemp.Printf("starting:%.1f°",fAngle);
						else
						{
							CXhChar50 sAngle("ending:%.1f°",fAngle);
#else
							sTemp.Printf("始端:%.1f°",fAngle);
						else
						{
							CXhChar50 sAngle("终端:%.1f°",fAngle);
#endif
							if(sTemp.GetLength()>0)
								sTemp.Append(",\t");	
							sTemp.Append(sAngle);
						}
					}
					else if(pTubeJoint->type>2&&pTubeJoint->type<6&&pTubeJoint->hLinkObj>0x20)
					{	
						pParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pTubeJoint->hLinkObj,CLS_PARAMPLATE);		
						if(pParamPlate==NULL)
							continue;
						double M=0;
						char cPartType;
						pParamPlate->GetDesignItemValue('M',&M);
						if(pParamPlate->m_iParamType==TYPE_ROLLEND)
							cPartType='C';
						else if(pParamPlate->m_iParamType==TYPE_UEND)
							cPartType='U';
						else if(pParamPlate->m_iParamType==TYPE_XEND)
						{
							cPartType='X';
							if(pParamPlate->vertex_list.GetNodeNum()<3)
								pParamPlate->DesignPlate();
							CLDSPlate *pUpPlate=NULL,*pDownPlate=NULL;
							pParamPlate->GetVertPlateInXEndPlate(&pUpPlate,&pDownPlate);
							if(pUpPlate&&pDownPlate)
							{
								double cos_up=fabs(pUpPlate->ucs.axis_z*pParamPlate->ucs.axis_y);
								double cos_down=fabs(pDownPlate->ucs.axis_z*pParamPlate->ucs.axis_y);
								if(fabs(cos_up-cos_down)<EPS&&fabs(cos_down)<EPS_COS)
								{	//非90度，且上下两立板倾斜角度相同
									double fAngle=ACOS(cos_down)*DEGTORAD_COEF;
									if(j==0)
										sStartAngle.Printf("S:%.1f°",fAngle);
									else 
										sEndAngle.Printf("E:%.1f°",fAngle);
								}
								else if(fabs(cos_up)<EPS_COS||fabs(cos_down)<EPS_COS)
								{	//上下两侧立板有一块为非90度
									double fUpAngle=ACOS(cos_up)*DEGTORAD_COEF;
									double fDownAngle=ACOS(cos_down)*DEGTORAD_COEF;
									if(j==0)
									{
										if(fabs(cos_up)<EPS_COS&&fabs(cos_down)<EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
											sStartAngle.Printf("S:Upside %.1f,downside %.1f",fUpAngle,fDownAngle);
										else if(fabs(cos_up)<EPS_COS)
											sStartAngle.Printf("S:upside %.1f°",fUpAngle);
										else //if(fabs(cos_down)<EPS_COS)
											sStartAngle.Printf("S:downside %.1f°",fDownAngle);
#else
											sStartAngle.Printf("S:上侧%.1f,下侧%.1f",fUpAngle,fDownAngle);
										else if(fabs(cos_up)<EPS_COS)
											sStartAngle.Printf("S:上侧%.1f°",fUpAngle);
										else //if(fabs(cos_down)<EPS_COS)
											sStartAngle.Printf("S:下侧%.1f°",fDownAngle);
#endif
									}
									else 
									{
										if(fabs(cos_up)<EPS_COS&&fabs(cos_down)<EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
											sEndAngle.Printf("E:Upside %.1f,Downside %.1f",fUpAngle,fDownAngle);
										else if(fabs(cos_up)<EPS_COS)
											sEndAngle.Printf("E:Upside %.1f°",fUpAngle);
										else //if(fabs(cos_down)<EPS_COS)
											sEndAngle.Printf("E:Downside %.1f°",fDownAngle);
#else
											sEndAngle.Printf("E:上侧%.1f,下侧%.1f",fUpAngle,fDownAngle);
										else if(fabs(cos_up)<EPS_COS)
											sEndAngle.Printf("E:上侧%.1f°",fUpAngle);
										else //if(fabs(cos_down)<EPS_COS)
											sEndAngle.Printf("E:下侧%.1f°",fDownAngle);
#endif
									}
								}
							}
						}
						else
							continue;
						if(j==0)
							sTemp.Printf("S%c%0.fX%d",cPartType,M,pParamPlate->thick);
						else
						{
							CXhChar50 sEndSlot("E%c%0.fX%d",cPartType,M,pParamPlate->thick);
							if(sTemp.GetLength()>0)
								sTemp.Append(",\t");
							sTemp.Append(sEndSlot);
						}
					}
				}
				xTubeBriefTbl.SetGrid(i+1,4,sTemp);
				//备注信息主要显示十字插板肋板角度，平面切割角度
				sTemp.Empty();
				if(sStartAngle.GetLength()>0)
					sTemp.Append(sStartAngle);
				if(sEndAngle.GetLength()>0)
				{
					sTemp.Append(",\t");
					sTemp.Append(sEndAngle);
				}
				xTubeBriefTbl.SetGrid(i+1,5,sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(NULL,"analyzing parts information and drawing table....");
#else
			PutMessage(NULL,"正在分析构件信息并绘制表格....");
#endif
			//
			AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
			if(nPageNum>1)	//计算表格插入位置
				tbl_pos.x+=(tbl_width+sys.general_set.nColumnWidth);
			xTubeBriefTbl.SetBasePos(tbl_pos);
			DrawTable(pBlockTableRecord,&xTubeBriefTbl,FALSE);
			pBlockTableRecord->close();
			//
			for(i=0;i<tubePageSet.GetNodeNum();i++)
			{
				CLDSLineTube *pLineTube=tubePageSet[i];
				f2dRect draw_rect;
				xTubeBriefTbl.GetGridRect(i+1,3,draw_rect);
#ifdef AFX_TARG_ENU_ENGLISH
				PutMessage(NULL,"Totally %d page, drawing diagram of tube %d on page %d ...",nSumPage,i+1,nPageNum);
#else
				PutMessage(NULL,"共%d页,正在绘制第%d页第%d个钢管的简图...",nSumPage,nPageNum,i+1);
#endif
				if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
				{
					f3dPoint origin;
					origin.x = draw_rect.topLeft.x;
					origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
					DrawTubeTypeMap(pLineTube,&draw_rect,FALSE,-1,1,NULL,1,1,FALSE);
				}
			}
			PutMessage(NULL,NULL);
			tubePageSet.Empty();	//清空链表
		}
	}
}

CTubeSpreadCurveExportDlg tubeCurveDlg;
void PartDrawingOneByOne()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CLDSPart* pPart;
	CProcessPart *pProcessPart;
	InitDrawingEnv();
	partSetDlg.m_bBatchPlot = FALSE;
    CAcModuleResourceOverride resOverride;
	if(partSetDlg.DoModal()!=IDOK)
		return;
	//根据用户选择出图模式和火曲变形考虑方案实时判断是否需要进行变形处理 wxc-2017.3.28
	sys.part_map.bExportDeformedProfile=sys.IsDeformed(partSetDlg.m_bPatternDrawing);
	CLogErrorLife logobj;
	try{
		int nHits;
		if(partSetDlg.partList.GetNodeNum()!=1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("only one parts can be selected by the command");
#else
			AfxMessageBox("该命令只能选中一个构件");
#endif
			return;
		}
		CPartSetCreatorDlg::STDPART_ITEM *pCurSelItem=partSetDlg.partList.GetFirst();
		nHits = pCurSelItem->num;
		pPart=pCurSelItem->GetPart();
		pProcessPart=pCurSelItem->GetProcessPart();
		DRAGSET.ClearEntSet();
		BOOL para[10]={FALSE},tube_para[4]={TRUE};
		para[9]=TRUE;
		BOOL bDisplayedTubeCurve=FALSE;
		CXhChar200 export_info;
		ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> *pTblRowList=NULL;
		int idClassType=-1;
		if(pPart!=NULL)
			idClassType=pPart->GetClassTypeId();
		switch(idClassType)
		{
		case -1:
			DrawJgTypeMap(pProcessPart,NULL,NULL,-1,nHits,0,"temp",1,partSetDlg.m_nTaNum);
			break;
		case CLS_LINESLOT:
		case CLS_LINEFLAT:
			DrawSlotOrFlatTypeMap(pPart,NULL,-1,nHits,"temp",1,partSetDlg.m_nTaNum);
			break;
		case CLS_LINEANGLE:
			if(pPart!=NULL)
			{
				DrawJgTypeMap(&CProcessAngle(pPart),NULL,NULL,-1,nHits,0,"temp",1,partSetDlg.m_nTaNum);
				break;
			}
		case CLS_POLYPART:
			DrawHuoQuJg((CLDSPolyPart*)pPart,NULL,-1,nHits,"temp",1,partSetDlg.m_nTaNum);
			break;
		case CLS_PLATE:
		case CLS_PARAMPLATE:
			if(!sys.general_set.bProcessCardTbl||partSetDlg.m_bPatternDrawing)
			{
				if(sys.part_map.plate.bUseDimStyleConfigFile==FALSE||sys.part_map.plate.bShowPlateCfgInfoDlg)
				{
					CPlankPartParaExportDlg plank_para_dlg;
					plank_para_dlg.DoModal();
				}
				para[0]=CPlankPartParaExportDlg::m_bMarkVertex;
				para[1]=CPlankPartParaExportDlg::m_bMartWeld;
				strcpy(export_info,CPlankPartParaExportDlg::m_sExportPartInfoKeyStr);
				pTblRowList=&CPlankPartParaExportDlg::m_xTblRowList;
			}
			DrawPlankTypePartMap(pPart,export_info,para,-1,nHits,sys.general_set.bProcessCardTbl,
				partSetDlg.m_bPatternDrawing,1,partSetDlg.m_nTaNum,NULL,TRUE,pTblRowList);
			break;
		case CLS_LINETUBE:
			if(partSetDlg.m_bIncTransectTube)
			{	//绘制相贯钢管零件图(相贯线示意图)或大样图(相贯线展开图) wht 10-08-04
				if(partSetDlg.m_bPatternDrawing)
				{
					if(!bDisplayedTubeCurve)
					{
						if(tubeCurveDlg.DoModal()!=IDOK)
							return;
						bDisplayedTubeCurve=TRUE;
						tube_para[0]=tubeCurveDlg.m_bInnerSpreadCurve;			//内
						tube_para[1]=tubeCurveDlg.m_bOutterSpreadCurve;			//外
						tube_para[2]=tubeCurveDlg.m_bInnerOutterComposeCurve;	//内外合成线
						tube_para[3]=tubeCurveDlg.m_bProcessSpreadCurve;		//加工线
					}	
					DrawTubeSpreadMap2((CLDSLineTube*)pPart,tube_para,-1,nHits,NULL,1,partSetDlg.m_nTaNum);
				}
				else 
					DrawTubeTypeMap((CLDSLineTube*)pPart,NULL,TRUE,-1,nHits,NULL,1,partSetDlg.m_nTaNum);
			}
			else if(partSetDlg.m_bIncTube&&!partSetDlg.m_bCreateTubeBrief)	//普通钢管
				DrawTubeTypeMap((CLDSLineTube*)pPart,NULL,FALSE,-1,nHits,NULL,1,partSetDlg.m_nTaNum);
			break;
		case CLS_ARCANGLE:
		case CLS_ARCFLAT:
		case CLS_ARCSLOT:
			DrawArcPartTypeMap((CLDSArcPart*)pPart,NULL,-1,nHits,"temp",1,partSetDlg.m_nTaNum);
			break;
		default:
#ifdef AFX_TARG_ENU_ENGLISH
			throw " unreasonable part type found, part map scale of the part can not be drawn";
#else
			throw "发现了不合理的构件类型,此构件不能绘制构件图";
#endif
		}
		ads_point base;
		base[X] = 0;
		base[Y] = 0;
		base[Z] = 0;
#ifdef _ARX_2007
		ads_command(RTSTR,L"ZOOM",RTSTR,L"e",RTNONE);
#else
		ads_command(RTSTR,"ZOOM",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		DragEntSet(base,"Please select the insertion point of the part map scale");
#else
		DragEntSet(base,"请点取构件图的插入点");
#endif
		//钢管开槽列表
		if(pPart && pPart->GetClassTypeId()==CLS_LINETUBE)
		{
			TUBESET tubeSet;
			pPart->feature=nHits;
			tubeSet.append((CLDSLineTube*)pPart);
			if(partSetDlg.m_bCreateTubeBrief&&tubeSet.GetNodeNum()>0)
				DrawBriefTubeTable(tubeSet);
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}
static void SaveDrawingToFile(const char* sFilePath,BOOL bDeleteErase=TRUE)
{
	AcDbDatabase *pCurDatabase=GetCurDwg();
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"a",RTNONE);
	pCurDatabase->saveAs((ACHAR*)_bstr_t(sFilePath));
	//删除所有图元
	if (bDeleteErase)
		acedCommand(RTSTR,L"erase",RTSTR,L"all",RTSTR,L"",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"a",RTNONE);
	pCurDatabase->saveAs(sFilePath);
	//删除所有图元
	if (bDeleteErase)
		acedCommand(RTSTR,"erase",RTSTR,"all",RTSTR,"",RTNONE);
#endif
}
void BatchPartDrawingAndSaveFile()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    // When resource from this ARX app is needed, just
    // instantiate a local CAcModuleResourceOverride
    CAcModuleResourceOverride resOverride;
	InitDrawingEnv();
	int i=0;
	//partSetDlg.m_bBatchPlot = TRUE;
	//if(partSetDlg.DoModal()!=IDOK)
	//	return;
	CLogErrorLife logobj;
	//设置文件路径已及绘图区域尺寸
	CSetPartMapFilePathDlg filePathDlg;
	filePathDlg.m_sPartDrawingPath=sys.sFilePath;
	i=filePathDlg.m_sPartDrawingPath.ReverseFind('\\');	//反向查找'\\'
	filePathDlg.m_sPartDrawingPath=filePathDlg.m_sPartDrawingPath.Left(i); //移除文件名称
	filePathDlg.m_sPartDrawingPath+="\\";
	filePathDlg.m_sPartDrawingPath+=Ta.m_sPrjName;
	if(filePathDlg.DoModal()!=IDOK)
		return;
	CXhChar500 sFilePath("%s",filePathDlg.m_sPartDrawingPath);
	MakeDirectory(sFilePath);
	f2dRect draw_rect,*pDrawRect=NULL;
	if(!sys.general_set.bProcessCardTbl)
	{
		draw_rect.topLeft.Set(0,filePathDlg.m_fDrawRectWidth);
		draw_rect.bottomRight.Set(filePathDlg.m_fDrawRectLen,0);
		pDrawRect=&draw_rect;
	}
	//
	BOOL bPatternDrawing=FALSE;	//大样图
	DRAGSET.ClearEntSet();
	BOOL bDisplayed=FALSE,para[10]={FALSE},tube_para[4]={TRUE};
	BOOL bDisplayedTubeCurve=FALSE;
	para[9]=TRUE;
	CXhChar200 export_info;
	ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> *pTblRowList=NULL;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	int nPageNum=1;
	nPageNum=partSetDlg.partList.GetNodeNum();
	TUBESET tubeSet;
	JGSET cutAngleJgSet;
	//
	i=0;
	for(CPartSetCreatorDlg::STDPART_ITEM *pItem=partSetDlg.partList.GetFirst();pItem;
	pItem=partSetDlg.partList.GetNext())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(NULL,"generating %d part!",i+1);
#else
		PutMessage(NULL,"正在生成第%d个构件!",i+1);
#endif
		CLDSPart* pPart=pItem->GetPart();
		if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_POLYPART)
			continue;	//目前仅支持绘制角钢及获取角钢 wht 11-06-02
		if(pDrawRect)
		{	//绘制矩形框
			AcDbBlockTableRecord *pBlockTableRec=GetBlockTableRecord();
			CreateAcadRect(pBlockTableRec,f3dPoint(pDrawRect->topLeft.x,pDrawRect->topLeft.y),
				pDrawRect->Width(),pDrawRect->Height());
			pBlockTableRec->close();
		}
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
			if(pLineAngle->IsCutAngleOrWing())	//统计切角角钢集合
				cutAngleJgSet.append(pLineAngle);
			DrawJgTypeMap(&CProcessAngle(pLineAngle),NULL,pDrawRect,-1,pItem->num,1,NULL,
				nPageNum,partSetDlg.m_nTaNum,filePathDlg.m_bDimPartInfo);
		}
		else if(pPart->GetClassTypeId()==CLS_POLYPART)
		{
			DrawHuoQuJg((CLDSPolyPart*)pPart,pDrawRect,-1,pItem->num,NULL,
						nPageNum,partSetDlg.m_nTaNum,filePathDlg.m_bDimPartInfo);
		}
		else if(pPart->GetClassTypeId()==CLS_LINESLOT||pPart->GetClassTypeId()==CLS_LINEFLAT)
		{
			DrawSlotOrFlatTypeMap(pPart,pDrawRect,-1,pItem->num,NULL,
							nPageNum,partSetDlg.m_nTaNum,filePathDlg.m_bDimPartInfo);
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE)
		{
			if(partSetDlg.m_bIncTransectTube)
			{	//绘制相贯钢管零件图(相贯线示意图)或大样图(相贯线展开图) wht 10-08-04
				if(bPatternDrawing)
				{
					if(!bDisplayedTubeCurve)
					{
						if(tubeCurveDlg.DoModal()!=IDOK)
							continue;
						bDisplayedTubeCurve=TRUE;
						tube_para[0]=tubeCurveDlg.m_bInnerSpreadCurve;
						tube_para[1]=tubeCurveDlg.m_bOutterSpreadCurve;
						tube_para[2]=tubeCurveDlg.m_bInnerOutterComposeCurve;
						tube_para[3]=tubeCurveDlg.m_bProcessSpreadCurve;		//加工线
					}
					DrawTubeSpreadMap2((CLDSLineTube*)pPart,tube_para,-1,pItem->num,NULL,
									nPageNum,partSetDlg.m_nTaNum,filePathDlg.m_bDimPartInfo);
				}
				else
					DrawTubeTypeMap((CLDSLineTube*)pPart,pDrawRect,TRUE,-1,pItem->num,NULL,
									nPageNum,partSetDlg.m_nTaNum,filePathDlg.m_bDimPartInfo);
			}
			else if(partSetDlg.m_bIncTube)	//普通钢管
			{
				pPart->feature=pItem->num;
				tubeSet.append((CLDSLineTube*)pPart);
				DrawTubeTypeMap((CLDSLineTube*)pPart,pDrawRect,FALSE,-1,pItem->num,NULL,
								nPageNum,partSetDlg.m_nTaNum,filePathDlg.m_bDimPartInfo);
			}
		}
		else
		{
			if((!sys.general_set.bProcessCardTbl&&!bDisplayed)||partSetDlg.m_bPatternDrawing)
			{
				if(sys.part_map.plate.bUseDimStyleConfigFile==FALSE||sys.part_map.plate.bShowPlateCfgInfoDlg)
				{
					CPlankPartParaExportDlg plank_para_dlg;
					plank_para_dlg.DoModal();
				}
				strcpy(export_info,CPlankPartParaExportDlg::m_sExportPartInfoKeyStr);
				pTblRowList=&CPlankPartParaExportDlg::m_xTblRowList;
				bDisplayed=TRUE;
			}
			if(pPart->GetClassTypeId()==CLS_PLATE || pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				DrawPlankTypePartMap(pPart,export_info,para,-1,pItem->num,sys.general_set.bProcessCardTbl,
									bPatternDrawing,nPageNum,partSetDlg.m_nTaNum,NULL,filePathDlg.m_bDimPartInfo,pTblRowList);
			}
		}
		i++;
		//
		CXhChar500 sFullPath("%s\\%s.dwg",(char*)sFilePath,(char*)pPart->GetPartNo());
		SaveDrawingToFile(sFullPath);
	}
	if(partSetDlg.m_bIncAngle&&partSetDlg.m_bCreateJgCutAngleBriefMap
		&&cutAngleJgSet.GetNodeNum()>0)
	{	//输出角钢切肢切角简图 wht 11-07-04
		CCutAngleBriefMap::Draw(cutAngleJgSet,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		CXhChar500 sFullPath("%s\\angle cut and leg cut sketch.dwg",(char*)sFilePath);
#else
		CXhChar500 sFullPath("%s\\切肢切角简图.dwg",(char*)sFilePath);
#endif
		SaveDrawingToFile(sFullPath);
	}
	//打开当前文件夹
	CString sCmdLinePara="";
	sCmdLinePara.Format("explorer.exe %s",(char*)sFilePath);
	WinExec(sCmdLinePara,SW_SHOW);
	PutMessage(NULL,NULL);
}

void DrawPlateMap(CPlankDrawing *pPlateDraw,AcDbBlockTableRecord *pBlockTableRecord,f3dPoint origin,
	CXhChar200 export_info,int nTaNum=1,double zoomCoef=1.0);
void ArrangeAndDrawPlate(PARTSET &partList,CXhChar200 export_info,int nPaperL=0,int nPaperW=1000,int minDistance=0,
						int nTaNum=1,BOOL PatternDrawing=TRUE,BYTE ciArrangeMode=0);
typedef  CPartSetCreatorDlg::STDPART_ITEM*  STDPART_ITEM_PTR;
static int ComparePartItem(const  STDPART_ITEM_PTR& item1,const STDPART_ITEM_PTR& item2) //比较件号和段号
{
	SEGI iSeg1=(const_cast<STDPART_ITEM_PTR>(item1))->GetSegI(),
		iSeg2=(const_cast<STDPART_ITEM_PTR>(item2))->GetSegI();
	if(iSeg1.iSeg>iSeg2.iSeg)
		return 1;
	else if(iSeg1.iSeg<iSeg2.iSeg)
		return -1;
	else
		return ComparePartNoString((const_cast<CPartSetCreatorDlg::STDPART_ITEM*>(item1))->GetPartNo(),
									(const_cast<CPartSetCreatorDlg::STDPART_ITEM*>(item2))->GetPartNo());
}

static int _LocalCompareSegI(const SEGREC& seg1,const SEGREC& seg2)
{
	if(seg1.iSeg.Digital()<seg2.iSeg.Digital())
		return -1;
	else if(seg1.iSeg.Digital()>seg2.iSeg.Digital())
		return 1;
	else
		return seg1.iSeg.iSeg-seg2.iSeg.iSeg;
}
void BatchPartDrawing()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    //
	CAcModuleResourceOverride resOverride;
	CLogErrorLife logobj;
	InitDrawingEnv();
	int i=0,index=0,iPage=0,iPart=0;
	partSetDlg.m_bBatchPlot = TRUE;
	if(partSetDlg.DoModal()!=IDOK)
		return;
	//根据用户选择出图模式和火曲变形考虑方案实时判断是否需要进行变形处理 wxc-2017.3.28
	sys.part_map.bExportDeformedProfile=sys.IsDeformed(partSetDlg.m_bPatternDrawing);
	if(partSetDlg.m_bAutoCreateFile&&partSetDlg.m_bIncAngle)
	{	//目前仅支持将角钢逐个生成到单个Dwg文件中 wht 11-06-02
		BatchPartDrawingAndSaveFile();
		return;
	}
	BOOL bOnlyDrawPlate=TRUE;
	PARTSET parts;
	int nPageNum=partSetDlg.partList.GetNodeNum();
	ARRAY_LIST<CPartSetCreatorDlg::STDPART_ITEM*> partList(0,nPageNum);
	for(CPartSetCreatorDlg::STDPART_ITEM *pItem=partSetDlg.partList.GetFirst();pItem;pItem=partSetDlg.partList.GetNext())
	{
		partList.append(pItem);
		CLDSPart* pPart=pItem->GetPart();
		if(pPart==NULL)
			continue;
		if(bOnlyDrawPlate&&!pPart->IsPlate())
			bOnlyDrawPlate=FALSE;
		else if(pPart->IsPlate())
		{
			pPart->feature = pItem->num;
			parts.append(pPart);
		}
	}
	CPlankPartParaExportDlg plank_para_dlg;
	BOOL bArrangePlate=FALSE;
	if(partSetDlg.m_bAutoArrangePlate)	
		bArrangePlate=TRUE;	//自动排版
	else if(partSetDlg.m_bPatternDrawing)	
		bArrangePlate=TRUE;	//钢板大样图
	else if(!sys.general_set.bProcessCardTbl&&sys.general_set.PartMapScale.fRealToDraw==1)
		bArrangePlate=TRUE;	//1:1构件图
	if(bOnlyDrawPlate&&parts.GetNodeNum()>1&&bArrangePlate)
	{	//处理钢板自动排版或大样图布局
		if(sys.part_map.plate.bUseDimStyleConfigFile==FALSE||sys.part_map.plate.bShowPlateCfgInfoDlg)
			plank_para_dlg.DoModal();
		CXhChar200 export_info;
		strcpy(export_info,CPlankPartParaExportDlg::m_sExportPartInfoKeyStr);
		//
		setDlg.m_bAutoArrange=partSetDlg.m_bAutoArrangePlate;
		if(setDlg.DoModal()!=IDOK)
			return;
		if(partSetDlg.m_bAutoArrangePlate)
			ArrangeAndDrawPlate(parts,export_info,setDlg.m_nPaperL,setDlg.m_nPaperW,setDlg.m_nMinDistance,partSetDlg.m_nTaNum,partSetDlg.m_bPatternDrawing,0);
		else if(partSetDlg.m_bGroupBySeg)
			ArrangeAndDrawPlate(parts,export_info,setDlg.m_nPaperL,setDlg.m_nPaperW,setDlg.m_nMinDistance,partSetDlg.m_nTaNum,partSetDlg.m_bPatternDrawing,1);
		else 
			ArrangeAndDrawPlate(parts,export_info,setDlg.m_nPaperL,setDlg.m_nPaperW,setDlg.m_nMinDistance,partSetDlg.m_nTaNum,partSetDlg.m_bPatternDrawing,2);
	}
	else
	{
		if(partSetDlg.partList.GetNodeNum()>1)
		{
			CPartDrawingArraySpaceDlg spaceDlg;
			spaceDlg.m_nRowHigh=sys.general_set.nRowHigh;
			spaceDlg.m_nColumnWidth=sys.general_set.nColumnWidth;
			if(spaceDlg.DoModal()!=IDOK)
				return;
			sys.general_set.nRowHigh=spaceDlg.m_nRowHigh;
			sys.general_set.nColumnWidth=spaceDlg.m_nColumnWidth;
		}
		DRAGSET.ClearEntSet();
		BOOL bDisplayed=FALSE,para[10]={FALSE},tube_para[4]={TRUE};
		BOOL bDisplayedTubeCurve=FALSE;
		para[9]=TRUE;
		CXhChar200 export_info;
		ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> *pTblRowList=NULL;
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		TUBESET tubeSet;
		JGSET cutAngleJgSet;
		CXhChar200 sFilePath;
		if(!partSetDlg.m_bGroupBySeg)
		{
			for(index=0,i=0;index<partList.GetSize();index++)
			{
				CPartSetCreatorDlg::STDPART_ITEM *pItem = partList[index];//(*ppItem);
				CString prompt;
#ifdef AFX_TARG_ENU_ENGLISH
				prompt="generating ";
				prompt+=CXhChar16("%d part!",i);
#else
				prompt="正在生成";
				prompt+=CXhChar16("%d个构件",i);
#endif
				PutMessage(NULL,prompt);
				if(sys.part_map.angle.bOneCardMultiPart&&partSetDlg.m_bIncAngle&&!partSetDlg.m_bIncPlate&&!partSetDlg.m_bIncTransectTube)
				{	//一卡多件
					nPageNum=partList.GetSize()/2+partList.GetSize()%2;	//总页数
					CLDSPart* pPart=pItem->GetPart();
					if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_POLYPART)
					{	
						CPartSetCreatorDlg::STDPART_ITEM *pNextItem=NULL;
						int tempI=index+1;
						do{
							if(tempI<partList.GetSize())
								pNextItem=partList[tempI];
							else
								break;
							tempI++;
						}while(pNextItem&&pNextItem->GetPart()->GetClassTypeId()!=CLS_LINEANGLE
							&&pNextItem->GetPart()->GetClassTypeId()!=CLS_POLYPART);
						if(pNextItem)
						{
							index=tempI-1;
							DrawJgTypeMap(&CProcessAngle(pPart),&CProcessAngle(pNextItem->GetPart()),NULL,i,pItem->num,pNextItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
						}
						else
							DrawJgTypeMap(&CProcessAngle(pPart),NULL,NULL,i,pItem->num,1,NULL,nPageNum,partSetDlg.m_nTaNum);
					}
					i++;
				}
				else
				{
					CLDSPart* pPart=pItem->GetPart();
					if(pPart&&pPart->IsArcPart())
						DrawArcPartTypeMap((CLDSArcPart*)pPart,NULL,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
					else if(pPart&&(pPart->GetClassTypeId()==CLS_LINESLOT||pPart->GetClassTypeId()==CLS_LINEFLAT))
						DrawSlotOrFlatTypeMap(pPart,NULL,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
					else if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
						DrawJgTypeMap(&CProcessAngle(pPart),NULL,NULL,i,pItem->num,1,NULL,nPageNum,partSetDlg.m_nTaNum);
					else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
						DrawHuoQuJg((CLDSPolyPart*)pPart,NULL,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
					else if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
					{
						if(partSetDlg.m_bIncTransectTube)
						{	//绘制相贯钢管零件图(相贯线示意图)或大样图(相贯线展开图) wht 10-08-04
							if(partSetDlg.m_bPatternDrawing)
							{
								if(!bDisplayedTubeCurve)
								{
									if(tubeCurveDlg.DoModal()!=IDOK)
										continue;
									bDisplayedTubeCurve=TRUE;
									tube_para[0]=tubeCurveDlg.m_bInnerSpreadCurve;
									tube_para[1]=tubeCurveDlg.m_bOutterSpreadCurve;
									tube_para[2]=tubeCurveDlg.m_bInnerOutterComposeCurve;
									tube_para[3]=tubeCurveDlg.m_bProcessSpreadCurve;
								}
								DrawTubeSpreadMap2((CLDSLineTube*)pPart,tube_para,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
							}
							else
								DrawTubeTypeMap((CLDSLineTube*)pPart,NULL,TRUE,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
						}
						else //if(partSetDlg.m_bIncTube)	//普通钢管
						{
							pPart->feature=pItem->num;
							tubeSet.append((CLDSLineTube*)pPart);
							if(!partSetDlg.m_bCreateTubeBrief)
								DrawTubeTypeMap((CLDSLineTube*)pPart,NULL,FALSE,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
						}
					}
					else if(pPart&&(pPart->GetClassTypeId()==CLS_PLATE || pPart->GetClassTypeId()==CLS_PARAMPLATE))
					{
						if((!sys.general_set.bProcessCardTbl||partSetDlg.m_bPatternDrawing)&&!bDisplayed)
						{
							if(sys.part_map.plate.bUseDimStyleConfigFile==FALSE||sys.part_map.plate.bShowPlateCfgInfoDlg)
								plank_para_dlg.DoModal();
							para[0]=CPlankPartParaExportDlg::m_bMarkVertex;
							para[1]=CPlankPartParaExportDlg::m_bMartWeld;
							strcpy(export_info,CPlankPartParaExportDlg::m_sExportPartInfoKeyStr);
							pTblRowList=&CPlankPartParaExportDlg::m_xTblRowList;
							bDisplayed=TRUE;
						}
						DrawPlankTypePartMap(pPart,export_info,para,i,pItem->num,sys.general_set.bProcessCardTbl,
							partSetDlg.m_bPatternDrawing,nPageNum,partSetDlg.m_nTaNum,NULL,TRUE,pTblRowList);
					}
					else
					{
						CProcessPart* pProcessPart=pItem->GetProcessPart();
						if(pProcessPart)
							DrawJgTypeMap(pProcessPart,NULL,NULL,i,pItem->num,1,NULL,nPageNum,partSetDlg.m_nTaNum);
					}
					i++;
				}
				//统计切角切肢角钢集合
				CLDSPart* pPart=pItem->GetPart();
				if(partSetDlg.m_bCreateJgCutAngleBriefMap&&
					pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
					if(pLineAngle->IsCutAngleOrWing())
						cutAngleJgSet.append(pLineAngle);
				}
			}
		}
		else //if(partSetDlg.m_bGroupBySeg)
		{	//需要分段分组输出构件图
			//设置出图保存路径
			if(partSetDlg.m_bOneSegOneFile)
			{
				CSetPartMapFilePathDlg filePathDlg;
				filePathDlg.m_sPartDrawingPath=sys.sFilePath;
				int n=filePathDlg.m_sPartDrawingPath.ReverseFind('\\');	//反向查找'\\'
				filePathDlg.m_sPartDrawingPath=filePathDlg.m_sPartDrawingPath.Left(n); //移除文件名称
				filePathDlg.m_sPartDrawingPath+="\\";
				filePathDlg.m_sPartDrawingPath+=Ta.m_sPrjName;
				if(filePathDlg.DoModal()!=IDOK)
					return;
				sFilePath.Append(filePathDlg.m_sPartDrawingPath);
				MakeDirectory(sFilePath);
			}	
			CPartSetCreatorDlg::STDPART_ITEM **ppFistItem=partList.GetFirst();
			if(ppFistItem==NULL)
				return ;  // 没处理
			CPartSetCreatorDlg::STDPART_ITEM *pTailItem=partList.GetSize()>0?(*partList.GetTail()):NULL;
			SEGI curSeg=(*ppFistItem)->GetSegI();
			const int DRAWING_SPACE=3;   //跳过几个图纸空间
			//计算每段页数
			int *piPartCount;
	 		CHashList<int> hashPartCountBySegI;
			for(i=0;i<partList.GetSize();i++)
			{
				CPartSetCreatorDlg::STDPART_ITEM *pStdItem = partList[i];//(*ppItem);
				piPartCount=hashPartCountBySegI.GetValue((DWORD)pStdItem->GetSegI());
				if(piPartCount==NULL)
					hashPartCountBySegI.SetValue((DWORD)pStdItem->GetSegI(),1);
				else
					hashPartCountBySegI.SetValue((DWORD)pStdItem->GetSegI(),*piPartCount+1);
			}
			DYN_ARRAY<SEGREC> segarr(hashPartCountBySegI.GetNodeNum());
			for(piPartCount=hashPartCountBySegI.GetFirst(),i=0;piPartCount;piPartCount=hashPartCountBySegI.GetNext(),i++)
			{
				segarr[i].iSeg=hashPartCountBySegI.GetCursorKey();
				segarr[i].nPageCount=*piPartCount;
			}
			CQuickSort<SEGREC>::QuickSort(segarr,segarr.Size(),_LocalCompareSegI);
			i=0;	//清零当前构件在整体构件中的序号
			for(UINT is=0;is<segarr.Size();is++)
			{
				SEGREC xCurrSegInfo=segarr[is];
				if(partSetDlg.m_bOneSegOneFile)
					i=0;
				else if(i>0)
				{	//多段分组输出到一张DWG图中时需要换行补齐空白占位工艺卡位
					if(i%sys.general_set.nPartRowsN>0)	//补齐占位
						i+=(sys.general_set.nPartRowsN-i%sys.general_set.nPartRowsN);
					i+=sys.general_set.nPartRowsN;	//换行(列)
				}
				//index:当前构件在partList中的索引; i用于计算当前构件在图纸中位置的序号iOrder(含占位空白工艺卡)
				//iPart:当前构件在当前组(段)中的索引号;iPage:当前构件在当前组(段)中的页码索引号,以上均以0为索引基数
				for(index=0,iPart=0,iPage=0;index<partList.GetSize();index++)
				{
					CPartSetCreatorDlg::STDPART_ITEM *pItem = partList[index];//(*ppItem);
					if(pItem->GetSegI()!=xCurrSegInfo.iSeg)
						continue;
					CString prompt;
#ifdef AFX_TARG_ENU_ENGLISH
					prompt="generating ";
					prompt+=CXhChar16("%s segment's ",(char*)xCurrSegInfo.iSeg.ToString());
					prompt+=CXhChar16("%d part!",iPart);
#else
					prompt="正在生成";
					prompt+=CXhChar16("%s段",(char*)xCurrSegInfo.iSeg.ToString());
					prompt+=CXhChar16("%d个构件",iPart);
#endif
					PutMessage(NULL,prompt);
					if(sys.part_map.angle.bOneCardMultiPart&&partSetDlg.m_bIncAngle&&!partSetDlg.m_bIncPlate&&!partSetDlg.m_bIncTransectTube)
					{	//一卡多件
						nPageNum=xCurrSegInfo.nPageCount/2+xCurrSegInfo.nPageCount%2;	//总页数
						CLDSPart* pPart=pItem->GetPart();
						if(pItem->m_cPartType==CProcessPart::TYPE_LINEANGLE)
						{	
							CPartSetCreatorDlg::STDPART_ITEM *pNextItem=NULL;
							int tempI=index+1;
							do{
								if(tempI<partList.GetSize())
									pNextItem=partList[tempI];
								else
									break;
								tempI++;
							}while(pNextItem&&pNextItem->m_cPartType!=CProcessPart::TYPE_LINEANGLE);
							CProcessAngle procAngle(pItem->GetPart()),nextProcAngle;
							CProcessAngle* pProcessAngle=(CProcessAngle*)pItem->GetProcessPart();
							if(pProcessAngle==NULL)
								pProcessAngle=&procAngle;
							if(pNextItem)
							{
								CProcessAngle nextProcAngle(pNextItem->GetPart());
								CProcessAngle* pNextProcessAngle=(CProcessAngle*)pNextItem->GetProcessPart();
								if(pNextProcessAngle==NULL)
									pNextProcessAngle=&nextProcAngle;
								index=tempI-1;
								DrawJgTypeMap(pProcessAngle,pNextProcessAngle,NULL,i,pItem->num,pNextItem->num,NULL,nPageNum,partSetDlg.m_nTaNum,TRUE,iPage);
								iPart++;
							}
							else
								DrawJgTypeMap(pProcessAngle,NULL,NULL,i,pItem->num,1,NULL,nPageNum,partSetDlg.m_nTaNum,TRUE,iPage);
						}
						i++;
						iPart++;
						iPage++;
					}
					else
					{
						CLDSPart* pPart=pItem->GetPart();
						if(pPart&&pPart->IsArcPart())
							DrawArcPartTypeMap((CLDSArcPart*)pPart,NULL,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
						else if(pPart&&(pPart->GetClassTypeId()==CLS_LINESLOT||pPart->GetClassTypeId()==CLS_LINEFLAT))
							DrawSlotOrFlatTypeMap(pPart,NULL,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum,TRUE,iPage);
						else if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
							DrawJgTypeMap(&CProcessAngle(pPart),NULL,NULL,i,pItem->num,1,NULL,nPageNum,partSetDlg.m_nTaNum,TRUE,iPage);
						else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
							DrawHuoQuJg((CLDSPolyPart*)pPart,NULL,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum,TRUE,iPage);
						else if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
						{
							if(partSetDlg.m_bIncTransectTube)
							{	//绘制相贯钢管零件图(相贯线示意图)或大样图(相贯线展开图) wht 10-08-04
								if(partSetDlg.m_bPatternDrawing)
								{
									if(!bDisplayedTubeCurve)
									{
										if(tubeCurveDlg.DoModal()!=IDOK)
											continue;
										bDisplayedTubeCurve=TRUE;
										tube_para[0]=tubeCurveDlg.m_bInnerSpreadCurve;
										tube_para[1]=tubeCurveDlg.m_bOutterSpreadCurve;
										tube_para[2]=tubeCurveDlg.m_bInnerOutterComposeCurve;
										tube_para[3]=tubeCurveDlg.m_bProcessSpreadCurve;
									}
									DrawTubeSpreadMap2((CLDSLineTube*)pPart,tube_para,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
								}
								else
									DrawTubeTypeMap((CLDSLineTube*)pPart,NULL,TRUE,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
							}
							else //if(partSetDlg.m_bIncTube)	//普通钢管
							{
								pPart->feature=pItem->num;
								tubeSet.append((CLDSLineTube*)pPart);
								if(!partSetDlg.m_bCreateTubeBrief)
									DrawTubeTypeMap((CLDSLineTube*)pPart,NULL,FALSE,i,pItem->num,NULL,nPageNum,partSetDlg.m_nTaNum);
							}
						}
						else if(pPart&&(pPart->GetClassTypeId()==CLS_PLATE || pPart->GetClassTypeId()==CLS_PARAMPLATE))
						{
							if((!sys.general_set.bProcessCardTbl||partSetDlg.m_bPatternDrawing)&&!bDisplayed)
							{
								if(sys.part_map.plate.bUseDimStyleConfigFile==FALSE||sys.part_map.plate.bShowPlateCfgInfoDlg)
									plank_para_dlg.DoModal();
								para[0]=CPlankPartParaExportDlg::m_bMarkVertex;
								para[1]=CPlankPartParaExportDlg::m_bMartWeld;
								strcpy(export_info,CPlankPartParaExportDlg::m_sExportPartInfoKeyStr);
								pTblRowList=&CPlankPartParaExportDlg::m_xTblRowList;
								bDisplayed=TRUE;
							}
							DrawPlankTypePartMap(pPart,export_info,para,i,pItem->num,sys.general_set.bProcessCardTbl,
								partSetDlg.m_bPatternDrawing,nPageNum,partSetDlg.m_nTaNum,NULL,TRUE,pTblRowList);
						}
						else
						{
							CProcessPart* pProcessPart=pItem->GetProcessPart();
							if(pProcessPart)
							{
								if(hashPartCountBySegI.GetValue(DWORD(pItem->GetSegI()))!=NULL)
								{
									nPageNum=*hashPartCountBySegI.GetValue(DWORD(pItem->GetSegI()));
									iPage=0;
								}
								DrawJgTypeMap(pProcessPart,NULL,NULL,i,pItem->num,1,NULL,nPageNum,partSetDlg.m_nTaNum,TRUE,iPage);
							}
						}
						i++;
						iPart++;
						iPage++;
					}
					//统计切角切肢角钢集合
					CLDSPart* pPart=pItem->GetPart();
					if(partSetDlg.m_bCreateJgCutAngleBriefMap&&
						pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
					{
						CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
						if(pLineAngle->IsCutAngleOrWing())
							cutAngleJgSet.append(pLineAngle);
					}
					if(partSetDlg.m_bOneSegOneFile&&pItem==pTailItem)
					{
						CXhChar500 sFullPath("%s\\%s.dwg",(char*)sFilePath,(char*)pPart->iSeg.ToString());
						SaveDrawingToFile(sFullPath);
					}
				}
			}
		}
		//钢管开槽列表
		if(partSetDlg.m_bCreateTubeBrief&&tubeSet.GetNodeNum()>0)
			DrawBriefTubeTable(tubeSet);
		//角钢切角简图 wht 11-07-04
		if(partSetDlg.m_bIncAngle&&partSetDlg.m_bCreateJgCutAngleBriefMap&&cutAngleJgSet.GetNodeNum()>0)
			CCutAngleBriefMap::Draw(cutAngleJgSet,TRUE);
		if(partSetDlg.m_bOneSegOneFile)
		{	//打开文件位置
			CString sCmdLinePara="";
			sCmdLinePara.Format("explorer.exe %s",(char*)sFilePath);
			WinExec(sCmdLinePara,SW_SHOW);
		}
		else
		{	//进行拖拽
#ifdef _ARX_2007
			ads_command(RTSTR,L"ZOOM",RTSTR,L"e",RTNONE);
#else
			ads_command(RTSTR,"ZOOM",RTSTR,"e",RTNONE);
#endif
			PutMessage(NULL,NULL);
			ads_point base;
			base[X] = 0;
			base[Y] = 0;
			base[Z] = 0;
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(base,"Please select the insertion point of the part map scale");
#else
			DragEntSet(base,"请点取构件图的插入点");
#endif
		}
	}
}


f2dRect InsertSpecialProcessStatCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,SPECIAL_PROCESS_STAT_CARD_INFO *pProcessInfo);
struct COLUMN_INFO{
	int width;
	CXhChar100 title;
	COLUMN_INFO(){strcpy(title,"");width=20;}
	COLUMN_INFO(char *sTitle,int column_width)
	{
		strcpy(title,sTitle);
		width=column_width;
	}
};
typedef double(*GETSLOTLENGTHERROR)(DWORD iPlateType);
typedef double(*GETSLOTWIDTHERROR)(DWORD iPlateType);
extern CXhChar200 g_sLicFile;
void SpecialProcessStatCard()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CSpecialProcessStatCardDlg processStatCardDlg;
	if(processStatCardDlg.DoModal()!=IDOK)
		return;
	int i=0,j=0,nListNum=0;
	ATOM_LIST<CSpecialProcessStatCardDlg::STDPART_ITEM> part_list[10];
	CHashTable<int>partsetTbl;
	partsetTbl.CreateHashTable(100);
	for(CSpecialProcessStatCardDlg::STDPART_ITEM *pItem=processStatCardDlg.partList.GetFirst();pItem;
	pItem=processStatCardDlg.partList.GetNext())
	{	//按所需示意图类型不同分类
		if(partsetTbl.GetValueAt(pItem->nSketchType,j))
			part_list[j].append(*pItem);
		else 
		{
			part_list[nListNum].append(*pItem);
			partsetTbl.SetValueAt(pItem->nSketchType,nListNum);
			nListNum++;
		}
	}
	if(nListNum<1)
		return; //无记录
	//获取不同插板的开槽数据
	GETSLOTLENGTHERROR GetSlotLenError=NULL;
	GETSLOTWIDTHERROR GetSlotWidthError=NULL;
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
	{
		GetSlotLenError=(GETSLOTLENGTHERROR)GetProcAddress(hInstance,"GetSlotLengthError");
		GetSlotWidthError=(GETSLOTWIDTHERROR) GetProcAddress(hInstance,"GetSlotWidthError");
	}
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表

	PARTSET partset;
	f2dPoint insert_pos;
	for(j=0;j<nListNum;j++)
	{
		CFormatTbl xPartTbl;
		xPartTbl.SetLocateStyle(TOP_LEFT);
		double tbl_width=0;
		SPECIAL_PROCESS_STAT_CARD_INFO processInfo;
		int iPart=0,nPartNum=part_list[j].GetNodeNum();
		processInfo.m_nPageIndex=1;
		processInfo.m_nPageNum=nPartNum/15;
		if(nPartNum%15>0)
			processInfo.m_nPageNum+=1;
		if(part_list[j].GetNodeNum()<=0)
			continue;
		int nColNum=0;
		int nSketchType=part_list[j].GetFirst()->nSketchType;
		if(nSketchType==SKETCH_TUBETRANS)
		{	//相贯钢管加工图
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Penetration tube processing figure");
#else
			processInfo.sProcessName.Copy("相贯管加工图");
#endif
			processInfo.sSketchMap.Copy("TubeTransCut");
			tbl_width=180;
			nColNum=9;
		}
		else if(nSketchType==SKETCH_RSLOT)
		{	//一型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("open slot processing figure");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
#endif
			processInfo.sSketchMap.Copy("Slot-");
			tbl_width=180;
			nColNum=10;
		}
		else if(nSketchType==SKETCH_USLOT)
		{	//U型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
#endif
			processInfo.sSketchMap.Copy("SlotU");
			tbl_width=200;
			nColNum=11;
		}
		else if(nSketchType==SKETCH_TSLOT)
		{	//T型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
#endif
			processInfo.sSketchMap.Copy("SlotT");
			tbl_width=200;
			nColNum=11;
		}
		else if(nSketchType==SKETCH_XSLOT)
		{	//十型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
			processInfo.sSketchMap.Copy("Slot Cross");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
			processInfo.sSketchMap.Copy("Slot十");
#endif
			tbl_width=200;
			nColNum=11;
		}
		else if(nSketchType==SKETCH_RTSLOT)
		{	//一/T型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
#endif
			processInfo.sSketchMap.Copy("Slot-T");
			tbl_width=260;
			nColNum=14;
		}
		else if(nSketchType==SKETCH_RUSLOT)
		{	//一/U型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
#endif
			processInfo.sSketchMap.Copy("Slot-U");
			tbl_width=260;
			nColNum=14;
		}
		else if(nSketchType==SKETCH_RXSLOT)
		{	//一/十型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
			processInfo.sSketchMap.Copy("Slot-Cross");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
			processInfo.sSketchMap.Copy("Slot-十");
#endif
			tbl_width=260;
			nColNum=14;
		}
		else if(nSketchType==SKETCH_TUSLOT)
		{	//T/U型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
#endif
			processInfo.sSketchMap.Copy("SlotTU");
			tbl_width=280;
			nColNum=15;
		}
		else if(nSketchType==SKETCH_TXSLOT)
		{	//T/十型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
			processInfo.sSketchMap.Copy("Slot T Cross");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
			processInfo.sSketchMap.Copy("SlotT十");
#endif
			tbl_width=280;
			nColNum=15;
		}
		else if(nSketchType==SKETCH_UXSLOT)
		{	//U/十型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
			processInfo.sProcessName.Copy("Open slot processing figure");
			processInfo.sSketchMap.Copy("Slot U Cross");
#else
			processInfo.sProcessName.Copy("开口槽加工图");
			processInfo.sSketchMap.Copy("SlotU十");
#endif
			tbl_width=280;
			nColNum=15;
		}
		CSpecialProcessStatCardDlg::STDPART_ITEM *pItem=NULL;
		for(pItem=part_list[j].GetFirst();pItem;pItem=part_list[j].GetNext())
		{
			pItem->pPart->feature=pItem->num;
			partset.append(pItem->pPart);
			if(partset.GetNodeNum()==15||iPart==nPartNum-1)
			{
				f2dRect draw_rect=InsertSpecialProcessStatCardTbl(pBlockTableRecord,insert_pos,&processInfo);
				double scale=draw_rect.Width()/tbl_width;
				f2dPoint tbl_pos=draw_rect.topLeft;
				insert_pos.x+=sys.general_set.nColumnWidth;
				processInfo.m_nPageIndex++;
				xPartTbl.Destroy();
				int a=partset.GetNodeNum();
				if(nSketchType==SKETCH_TUSLOT||nSketchType==SKETCH_TXSLOT||nSketchType==SKETCH_UXSLOT||
					nSketchType==SKETCH_RTSLOT||nSketchType==SKETCH_RUSLOT||nSketchType==SKETCH_RXSLOT)
					xPartTbl.Create(partset.GetNodeNum()+2,nColNum);
				else 
					xPartTbl.Create(partset.GetNodeNum()+1,nColNum);
				//设置表头内容
				i=0;
				xPartTbl.SetRowHigh(0,8);
				xPartTbl.SetRowHigh(1,8);
				//表格中的文字缺省是右下角对齐即feature = 0
				//feature个位数表示，文字的对齐方式0:右中央 1表示正中央 2表示右下角
				//十位表示文字书写方向0:从左到右1:从上到下
				if(nSketchType==SKETCH_TUBETRANS)
				{	//相贯钢管加工图
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,0,"Branch No.");
					xPartTbl.SetColumnWide(0,20*scale);
					xPartTbl.SetGrid(0,1,"Material");
					xPartTbl.SetColumnWide(1,20*scale);
					xPartTbl.SetGrid(0,2,"Spec");
					xPartTbl.SetColumnWide(2,20*scale);
					xPartTbl.SetGrid(0,3,"length L");
					xPartTbl.SetColumnWide(3,20*scale);
					xPartTbl.SetGrid(0,4,"main tube spec");
					xPartTbl.SetColumnWide(4,20*scale);
					xPartTbl.SetGrid(0,5,"angle A°");
					xPartTbl.SetColumnWide(5,20*scale);
					xPartTbl.SetGrid(0,6,"Number");
					xPartTbl.SetColumnWide(6,20*scale);
					xPartTbl.SetGrid(0,7,"total number");
					xPartTbl.SetColumnWide(7,20*scale);
					xPartTbl.SetGrid(0,8,"notes");
#else
					xPartTbl.SetGrid(0,0,"支管编号");
					xPartTbl.SetColumnWide(0,20*scale);
					xPartTbl.SetGrid(0,1,"材质");
					xPartTbl.SetColumnWide(1,20*scale);
					xPartTbl.SetGrid(0,2,"规格");
					xPartTbl.SetColumnWide(2,20*scale);
					xPartTbl.SetGrid(0,3,"长度L");
					xPartTbl.SetColumnWide(3,20*scale);
					xPartTbl.SetGrid(0,4,"主管规格");
					xPartTbl.SetColumnWide(4,20*scale);
					xPartTbl.SetGrid(0,5,"角度A°");
					xPartTbl.SetColumnWide(5,20*scale);
					xPartTbl.SetGrid(0,6,"单基数");
					xPartTbl.SetColumnWide(6,20*scale);
					xPartTbl.SetGrid(0,7,"总数");
					xPartTbl.SetColumnWide(7,20*scale);
					xPartTbl.SetGrid(0,8,"备注");
#endif
					xPartTbl.SetColumnWide(8,20*scale);
					i=1;
				}
				else if(nSketchType==SKETCH_RSLOT)
				{	//一型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,0,"Tube No.");
					xPartTbl.SetColumnWide(0,20*scale);
					xPartTbl.SetGrid(0,1,"Material");
					xPartTbl.SetColumnWide(1,15*scale);
					xPartTbl.SetGrid(0,2,"Spec");
					xPartTbl.SetColumnWide(2,15*scale);
					xPartTbl.SetGrid(0,3,"board slot type");
					xPartTbl.SetColumnWide(3,20*scale);
					xPartTbl.SetGrid(0,4,"Board slot length L");
					xPartTbl.SetColumnWide(4,20*scale);
					xPartTbl.SetGrid(0,5,"Board slot width A");
					xPartTbl.SetColumnWide(5,20*scale);
					xPartTbl.SetGrid(0,6,"length of part");
					xPartTbl.SetColumnWide(6,20*scale);
					xPartTbl.SetGrid(0,7,"Number");
					xPartTbl.SetColumnWide(7,15*scale);
					xPartTbl.SetGrid(0,8,"total number");
					xPartTbl.SetColumnWide(8,15*scale);
					xPartTbl.SetGrid(0,9,"notes");
#else
					xPartTbl.SetGrid(0,0,"管编号");
					xPartTbl.SetColumnWide(0,20*scale);
					xPartTbl.SetGrid(0,1,"材质");
					xPartTbl.SetColumnWide(1,15*scale);
					xPartTbl.SetGrid(0,2,"规格");
					xPartTbl.SetColumnWide(2,15*scale);
					xPartTbl.SetGrid(0,3,"插板槽型式");
					xPartTbl.SetColumnWide(3,20*scale);
					xPartTbl.SetGrid(0,4,"插板槽长度L");
					xPartTbl.SetColumnWide(4,20*scale);
					xPartTbl.SetGrid(0,5,"插板槽宽度A");
					xPartTbl.SetColumnWide(5,20*scale);
					xPartTbl.SetGrid(0,6,"杆件长度");
					xPartTbl.SetColumnWide(6,20*scale);
					xPartTbl.SetGrid(0,7,"单基数");
					xPartTbl.SetColumnWide(7,15*scale);
					xPartTbl.SetGrid(0,8,"总数");
					xPartTbl.SetColumnWide(8,15*scale);
					xPartTbl.SetGrid(0,9,"备注");
#endif
					xPartTbl.SetColumnWide(9,20*scale);
					i=1;
				}
				else if(nSketchType==SKETCH_USLOT||nSketchType==SKETCH_TSLOT||nSketchType==SKETCH_XSLOT)
				{	//U型开口槽
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,0,"Tube No.");
					xPartTbl.SetColumnWide(0,20*scale);
					xPartTbl.SetGrid(0,1,"Material");
					xPartTbl.SetColumnWide(1,15*scale);
					xPartTbl.SetGrid(0,2,"Spec");
					xPartTbl.SetColumnWide(2,15*scale);
					xPartTbl.SetGrid(0,3,"Board slot type");
					xPartTbl.SetColumnWide(3,20*scale);
					xPartTbl.SetGrid(0,4,"Board slot length L");
					xPartTbl.SetColumnWide(4,20*scale);
					xPartTbl.SetGrid(0,5,"Board slot width A");
					xPartTbl.SetColumnWide(5,20*scale);
					if(nSketchType==SKETCH_USLOT)
						xPartTbl.SetGrid(0,6,"Board slot arc R");
					else 
						xPartTbl.SetGrid(0,6,"Board slot width B");
					xPartTbl.SetColumnWide(6,20*scale);
					xPartTbl.SetGrid(0,7,"part length");
					xPartTbl.SetColumnWide(7,20*scale);
					xPartTbl.SetGrid(0,8,"Number");
					xPartTbl.SetColumnWide(8,15*scale);
					xPartTbl.SetGrid(0,9,"total number");
					xPartTbl.SetColumnWide(9,15*scale);
					xPartTbl.SetGrid(0,10,"remark");
#else
					xPartTbl.SetGrid(0,0,"管编号");
					xPartTbl.SetColumnWide(0,20*scale);
					xPartTbl.SetGrid(0,1,"材质");
					xPartTbl.SetColumnWide(1,15*scale);
					xPartTbl.SetGrid(0,2,"规格");
					xPartTbl.SetColumnWide(2,15*scale);
					xPartTbl.SetGrid(0,3,"插板槽型式");
					xPartTbl.SetColumnWide(3,20*scale);
					xPartTbl.SetGrid(0,4,"插板槽长度L");
					xPartTbl.SetColumnWide(4,20*scale);
					xPartTbl.SetGrid(0,5,"插板槽宽度A");
					xPartTbl.SetColumnWide(5,20*scale);
					if(nSketchType==SKETCH_USLOT)
						xPartTbl.SetGrid(0,6,"插板槽圆弧R");
					else 
						xPartTbl.SetGrid(0,6,"插板槽宽度B");
					xPartTbl.SetColumnWide(6,20*scale);
					xPartTbl.SetGrid(0,7,"杆件长度");
					xPartTbl.SetColumnWide(7,20*scale);
					xPartTbl.SetGrid(0,8,"单基数");
					xPartTbl.SetColumnWide(8,15*scale);
					xPartTbl.SetGrid(0,9,"总数");
					xPartTbl.SetColumnWide(9,15*scale);
					xPartTbl.SetGrid(0,10,"备注");
#endif
					xPartTbl.SetColumnWide(10,20*scale);
					i=1;
				}
				else if(nSketchType==SKETCH_RTSLOT||nSketchType==SKETCH_RUSLOT||nSketchType==SKETCH_RXSLOT)
				{	//一/T型开口槽  一/U型开口槽  一/X型开口槽
					for(i=0;i<14;i++)
						xPartTbl.SetColumnWide(i,20*scale);
					xPartTbl.GetGridAt(1,0)->m_bVGridVirtual = TRUE;//坚直合并单元格
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,0,"tube No.");
					xPartTbl.GetGridAt(1,1)->m_bVGridVirtual = TRUE;//An upright merge cells
					xPartTbl.SetGrid(0,1,"Material");
					xPartTbl.GetGridAt(1,2)->m_bVGridVirtual = TRUE;//An upright merge cells
					xPartTbl.SetGrid(0,2,"Spec");
#else
					xPartTbl.SetGrid(0,0,"管编号");
					xPartTbl.GetGridAt(1,1)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,1,"材质");
					xPartTbl.GetGridAt(1,2)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,2,"规格");
#endif
					xPartTbl.GetGridAt(0,4)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,5)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,3)->feature=1;				//居中
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,3,"left side");
					xPartTbl.SetGrid(1,3,"slot type");
					xPartTbl.SetGrid(1,4,"slot length L");
					xPartTbl.SetGrid(1,5,"slot width A");
#else
					xPartTbl.SetGrid(0,3,"左端");
					xPartTbl.SetGrid(1,3,"槽型式");
					xPartTbl.SetGrid(1,4,"槽长L");
					xPartTbl.SetGrid(1,5,"槽宽A");
#endif
					xPartTbl.GetGridAt(0,7)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,8)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,9)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,6)->feature=1;				//居中
#ifdef AFX_TARG_ENU_ENGLISH
   					xPartTbl.SetGrid(0,6,"right side");
					xPartTbl.SetGrid(1,6,"slot type");
					xPartTbl.SetGrid(1,7,"slot length L");
					xPartTbl.SetGrid(1,8,"slot width A");
					if(nSketchType==SKETCH_RUSLOT)
						xPartTbl.SetGrid(1,9,"slot arc R");
					else
						xPartTbl.SetGrid(1,9,"slot width B");
					xPartTbl.GetGridAt(1,10)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,10,"part length");
					xPartTbl.GetGridAt(1,11)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,11,"Number");
					xPartTbl.GetGridAt(1,12)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,12,"total number");
					xPartTbl.GetGridAt(1,13)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,13,"notes");
#else
					xPartTbl.SetGrid(0,6,"右端");
					xPartTbl.SetGrid(1,6,"槽型式");
					xPartTbl.SetGrid(1,7,"槽长L");
					xPartTbl.SetGrid(1,8,"槽宽A");
					if(nSketchType==SKETCH_RUSLOT)
						xPartTbl.SetGrid(1,9,"槽圆弧R");
					else
						xPartTbl.SetGrid(1,9,"槽宽B");
					xPartTbl.GetGridAt(1,10)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,10,"杆件长度");
					xPartTbl.GetGridAt(1,11)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,11,"单基数");
					xPartTbl.GetGridAt(1,12)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,12,"总数");
					xPartTbl.GetGridAt(1,13)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,13,"备注");
#endif
					i=2;
				}
				else if(nSketchType==SKETCH_TUSLOT||nSketchType==SKETCH_TXSLOT||nSketchType==SKETCH_UXSLOT)
				{	//T/U型开口槽  T/X型开口槽  U/X型开口槽
					for(i=0;i<15;i++)
						xPartTbl.SetColumnWide(i,20*scale);
					xPartTbl.GetGridAt(1,0)->m_bVGridVirtual = TRUE;//坚直合并单元格
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,0,"tube No.");
					xPartTbl.GetGridAt(1,1)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,1,"Material");
					xPartTbl.GetGridAt(1,2)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,2,"Spec");
#else
					xPartTbl.SetGrid(0,0,"管编号");
					xPartTbl.GetGridAt(1,1)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,1,"材质");
					xPartTbl.GetGridAt(1,2)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,2,"规格");
#endif
					xPartTbl.GetGridAt(0,4)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,5)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,6)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,3)->feature=1;				//居中
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,3,"left side");
					xPartTbl.SetGrid(1,3,"slot type");
					xPartTbl.SetGrid(1,4,"slot length L");
					xPartTbl.SetGrid(1,5,"slot width A");
					if(nSketchType==SKETCH_TUSLOT||nSketchType==SKETCH_UXSLOT)
						xPartTbl.SetGrid(1,6,"slot arc R");
					else
						xPartTbl.SetGrid(1,6,"slot width B");
#else
					xPartTbl.SetGrid(0,3,"左端");
					xPartTbl.SetGrid(1,3,"槽型式");
					xPartTbl.SetGrid(1,4,"槽长L");
					xPartTbl.SetGrid(1,5,"槽宽A");
					if(nSketchType==SKETCH_TUSLOT||nSketchType==SKETCH_UXSLOT)
						xPartTbl.SetGrid(1,6,"槽圆弧R");
					else
						xPartTbl.SetGrid(1,6,"槽宽B");
#endif
					xPartTbl.GetGridAt(0,8)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,9)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,10)->m_bHGridVirtual = TRUE;//水平合并单元格
					xPartTbl.GetGridAt(0,7)->feature=1;				//居中
#ifdef AFX_TARG_ENU_ENGLISH
					xPartTbl.SetGrid(0,7,"right side");
					xPartTbl.SetGrid(1,7,"slot type");
					xPartTbl.SetGrid(1,8,"slot length L");
					xPartTbl.SetGrid(1,9,"slot width A");
					xPartTbl.SetGrid(1,10,"slot length B");
					xPartTbl.GetGridAt(1,11)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,11,"Length of rod");
					xPartTbl.GetGridAt(1,12)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,12,"Number");
					xPartTbl.GetGridAt(1,13)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,13,"Total number");
					xPartTbl.GetGridAt(1,14)->m_bVGridVirtual = TRUE;//upright merge cells
					xPartTbl.SetGrid(0,14,"notes");
#else
					xPartTbl.SetGrid(0,7,"右端");
					xPartTbl.SetGrid(1,7,"槽型式");
					xPartTbl.SetGrid(1,8,"槽长L");
					xPartTbl.SetGrid(1,9,"槽宽A");
					xPartTbl.SetGrid(1,10,"槽宽B");
					xPartTbl.GetGridAt(1,11)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,11,"杆件长度");
					xPartTbl.GetGridAt(1,12)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,12,"单基数");
					xPartTbl.GetGridAt(1,13)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,13,"总数");
					xPartTbl.GetGridAt(1,14)->m_bVGridVirtual = TRUE;//坚直合并单元格
					xPartTbl.SetGrid(0,14,"备注");
#endif
					i=2;
				}
				for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				{
					if(pPart->GetClassTypeId()!=CLS_LINETUBE)
						continue;
					CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
					CXhChar100 spec,tm_str;
					pPart->GetSpecification(spec,FALSE);
					//获取开槽加工间隙裕量
					CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
					if(pLineTube->m_tJointStart.type>=3&&pLineTube->m_tJointStart.type<=5)
						pStartParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
					if(pLineTube->m_tJointEnd.type>=3&&pLineTube->m_tJointEnd.type<=5)
						pEndParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
					double fStartSlotLenProcessError=sys.part_map.tube.fSlotLenProcessError;
					double fStartSlotWidthProcessError=sys.part_map.tube.fSlotWidthProcessError;
					double fEndSlotLenProcessError=sys.part_map.tube.fSlotLenProcessError;
					double fEndSlotWidthProcessError=sys.part_map.tube.fSlotWidthProcessError;
					if(pStartParamPlate&&GetSlotLenError!=NULL)		//槽长加工间隙裕量
						fStartSlotLenProcessError=GetSlotLenError(pStartParamPlate->m_iParamType);
					if(pStartParamPlate&&GetSlotWidthError!=NULL)	//槽宽加工间隙裕量
						fStartSlotWidthProcessError=GetSlotWidthError(pStartParamPlate->m_iParamType);
					if(pEndParamPlate&&GetSlotLenError!=NULL)		//槽长加工间隙裕量
						fEndSlotLenProcessError=GetSlotLenError(pEndParamPlate->m_iParamType);
					if(pEndParamPlate&&GetSlotWidthError!=NULL)		//槽宽加工间隙裕量
						fEndSlotWidthProcessError=GetSlotWidthError(pEndParamPlate->m_iParamType);

					if(nSketchType==SKETCH_TUBETRANS)
					{
						xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
						QuerySteelMatMark(pPart->cMaterial,tm_str);	//材质
						xPartTbl.SetGrid(i,1,tm_str);
						xPartTbl.SetGrid(i,2,spec);
						tm_str.Printf("%.0f",pLineTube->GetLength());
						xPartTbl.SetGrid(i,3,tm_str);
						CLDSLineTube *pDatumTube=NULL;
						f3dPoint line_vec,datum_line_vec;
						if(pLineTube->m_tJointStart.type==1)	//圆柱相贯
						{
							pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_LINETUBE);
							line_vec=pLineTube->End()-pLineTube->Start();
						}
						else if(pLineTube->m_tJointEnd.type==1)
						{
							pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_LINETUBE);
							line_vec=pLineTube->Start()-pLineTube->End();
						}
						if(pDatumTube)
						{
							//主管规格
							pDatumTube->GetSpecification(spec,FALSE);
							xPartTbl.SetGrid(i,4,spec);
							//角度A°
							datum_line_vec=pDatumTube->End()-pDatumTube->Start();
							double alfa=cal_angle_of_2vec(line_vec,datum_line_vec);
							if(alfa>Pi/2)
								alfa=Pi-alfa;
							alfa*=DEGTORAD_COEF;
							tm_str.Printf("%.1f°",alfa);
							xPartTbl.SetGrid(i,5,tm_str);
						}
						tm_str.Printf("%d",pLineTube->feature);			//单基数
						xPartTbl.SetGrid(i,6,tm_str);
						tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
						xPartTbl.SetGrid(i,7,tm_str);
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,8,"curve cutting");
#else
						xPartTbl.SetGrid(i,8,"割相贯线");
#endif
					}
					else if(nSketchType==SKETCH_RSLOT)
					{	//一型开口槽
						xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
						QuerySteelMatMark(pPart->cMaterial,tm_str);	//材质
						xPartTbl.SetGrid(i,1,tm_str);
						xPartTbl.SetGrid(i,2,spec);
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,3,"one shape slot");
#else
						xPartTbl.SetGrid(i,3,"—型槽");
#endif
						CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
						double M;
						if(pLineTube->m_tJointStart.type==4)
							pStartParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
						if(pLineTube->m_tJointEnd.type==4)
							pEndParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
						if(pStartParamPlate)
						{
							pStartParamPlate->GetDesignItemValue('M',&M);
							tm_str.Printf("%.0f",M+fStartSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							tm_str.Printf("%.0f",pStartParamPlate->thick+fStartSlotWidthProcessError);
							xPartTbl.SetGrid(i,5,tm_str);
						}
						else if(pEndParamPlate)
						{
							pEndParamPlate->GetDesignItemValue('M',&M);
							//槽长
							tm_str.Printf("%.0f",M+fEndSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							//槽宽
							tm_str.Printf("%d",pEndParamPlate->thick+fEndSlotWidthProcessError);
							xPartTbl.SetGrid(i,5,tm_str);
						}
						else
						{
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,4,"unknown");
							xPartTbl.SetGrid(i,5,"unknown");
#else
							xPartTbl.SetGrid(i,4,"未知");
							xPartTbl.SetGrid(i,5,"未知");
#endif
						}
						tm_str.Printf("%.0f",pLineTube->GetLength());	//杆件长度
						xPartTbl.SetGrid(i,6,tm_str);
						tm_str.Printf("%d",pLineTube->feature);			//单基数
						xPartTbl.SetGrid(i,7,tm_str);
						tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
						xPartTbl.SetGrid(i,8,tm_str);
						if(pStartParamPlate&&pEndParamPlate)
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,9,"Cutting slot on both ends");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,9,"Cut slot on one end");
#else
							xPartTbl.SetGrid(i,9,"两端割槽");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,9,"一端割槽");
#endif
					}
					else if(nSketchType==SKETCH_USLOT)
					{	//U型开口槽
						xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
						QuerySteelMatMark(pPart->cMaterial,tm_str);	//材质
						xPartTbl.SetGrid(i,1,tm_str);
						xPartTbl.SetGrid(i,2,spec);
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,3,"U shape slot");
#else
						xPartTbl.SetGrid(i,3,"U型槽");
#endif
						CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
						if(pLineTube->m_tJointStart.type==3)
							pStartParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
						if(pLineTube->m_tJointEnd.type==3)
							pEndParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
						double M,W;
						if(pStartParamPlate)
						{
							pStartParamPlate->GetDesignItemValue('M',&M);
							pStartParamPlate->GetDesignItemValue('W',&W);
							//槽长
							tm_str.Printf("%.0f",M+fStartSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							//槽宽
							tm_str.Printf("%.1f",pStartParamPlate->thick*2+W+fStartSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,5,tm_str);
							//开槽圆弧
							tm_str.Printf("%.1f",pStartParamPlate->thick+0.5*(W+fEndSlotWidthProcessError));
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,6,tm_str);
						}
						else if(pEndParamPlate)
						{
							pEndParamPlate->GetDesignItemValue('M',&M);
							pEndParamPlate->GetDesignItemValue('W',&W);
							//槽长
							tm_str.Printf("%.0f",M+fEndSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							//槽宽
							tm_str.Printf("%.1f",pEndParamPlate->thick*2+W+fEndSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,5,tm_str);
							//开槽圆弧
							tm_str.Printf("%.1f",pEndParamPlate->thick+0.5*(W+fEndSlotWidthProcessError));
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,6,tm_str);
						}
						else
						{
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,4,"unknown");
							xPartTbl.SetGrid(i,5,"unknown");
							xPartTbl.SetGrid(i,6,"unknown");
#else
							xPartTbl.SetGrid(i,4,"未知");
							xPartTbl.SetGrid(i,5,"未知");
							xPartTbl.SetGrid(i,6,"未知");
#endif
						}
						tm_str.Printf("%.0f",pLineTube->GetLength());	//杆件长度
						xPartTbl.SetGrid(i,7,tm_str);
						tm_str.Printf("%d",pLineTube->feature);			//单基数
						xPartTbl.SetGrid(i,8,tm_str);
						tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
						xPartTbl.SetGrid(i,9,tm_str);
						if(pStartParamPlate&&pEndParamPlate)
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,10,"Cut slot on both ends");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,10,"Cut slot on one end");
#else
							xPartTbl.SetGrid(i,10,"两端割槽");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,10,"一端割槽");
#endif
					}
					else if(nSketchType==SKETCH_TSLOT)
					{	//T型开口槽
						xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
						QuerySteelMatMark(pPart->cMaterial,tm_str);	//材质
						xPartTbl.SetGrid(i,1,tm_str);
						xPartTbl.SetGrid(i,2,spec);
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,3,"T slot");
#else
						xPartTbl.SetGrid(i,3,"T型槽");
#endif
						CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
						if(pLineTube->m_tJointStart.type==4)
							pStartParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
						if(pLineTube->m_tJointEnd.type==4)
							pEndParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
						double M,W;
						if(pStartParamPlate)
						{
							pStartParamPlate->GetDesignItemValue('M',&M);
							pStartParamPlate->GetDesignItemValue('W',&W);
							//槽长
							tm_str.Printf("%.0f",M+fStartSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							//槽宽A
							tm_str.Printf("%.1f",pStartParamPlate->thick*2+W+fStartSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,5,tm_str);
							//槽宽B
							//tm_str.Printf("%.1f",pStartParamPlate->thick*4+W*2);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,6,tm_str);
						}
						else if(pEndParamPlate)
						{
							pEndParamPlate->GetDesignItemValue('M',&M);
							pEndParamPlate->GetDesignItemValue('W',&W);
							//槽长
							tm_str.Printf("%.0f",M+fEndSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							//槽宽A
							tm_str.Printf("%.1f",pEndParamPlate->thick*2+W+fEndSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,5,tm_str);
							//槽宽B
							//tm_str.Printf("%.1f",pEndParamPlate->thick*4+W*2);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,6,tm_str);
						}
						else
						{
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,4,"unknown");
							xPartTbl.SetGrid(i,5,"unknown");
							xPartTbl.SetGrid(i,6,"unknown");
#else
							xPartTbl.SetGrid(i,4,"未知");
							xPartTbl.SetGrid(i,5,"未知");
							xPartTbl.SetGrid(i,6,"未知");
#endif
						}
						tm_str.Printf("%.0f",pLineTube->GetLength());	//杆件长度
						xPartTbl.SetGrid(i,7,tm_str);
						tm_str.Printf("%d",pLineTube->feature);			//单基数
						xPartTbl.SetGrid(i,8,tm_str);
						tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
						xPartTbl.SetGrid(i,9,tm_str);
						if(pStartParamPlate&&pEndParamPlate)
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,10,"Cut slot on both ends");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,10,"Cut slot on one end");
#else
							xPartTbl.SetGrid(i,10,"两端割槽");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,10,"一端割槽");
#endif
					}
					else if(nSketchType==SKETCH_XSLOT)
					{	//十型开口槽
						xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
						QuerySteelMatMark(pPart->cMaterial,tm_str);	//材质
						xPartTbl.SetGrid(i,1,tm_str);
						xPartTbl.SetGrid(i,2,spec);
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,3,"cross shape slot");
#else
						xPartTbl.SetGrid(i,3,"十型槽");
#endif
						CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
						if(pLineTube->m_tJointStart.type==5)
							pStartParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
						if(pLineTube->m_tJointEnd.type==5)
							pEndParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
						double M,W;
						if(pStartParamPlate)
						{
							pStartParamPlate->GetDesignItemValue('M',&M);
							pStartParamPlate->GetDesignItemValue('W',&W);
							//槽长
							tm_str.Printf("%.0f",M+fStartSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							//槽宽A
							tm_str.Printf("%.1f",pStartParamPlate->GetThick()+fStartSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,5,tm_str);
							//槽宽B
							tm_str.Printf("%.1f",pStartParamPlate->GetThick()+fStartSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,6,tm_str);
						}
						else if(pEndParamPlate)
						{
							pEndParamPlate->GetDesignItemValue('M',&M);
							pEndParamPlate->GetDesignItemValue('W',&W);
							//槽长
							tm_str.Printf("%.0f",M+fEndSlotLenProcessError);
							xPartTbl.SetGrid(i,4,tm_str);
							//槽宽A
							tm_str.Printf("%.1f",pEndParamPlate->GetThick()+fEndSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,5,tm_str);
							//槽宽B
							tm_str.Printf("%.1f",pEndParamPlate->GetThick()+fEndSlotWidthProcessError);
							SimplifiedNumString(tm_str);
							xPartTbl.SetGrid(i,6,tm_str);
						}
						else
						{
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,4,"unknown");
							xPartTbl.SetGrid(i,5,"unknown");
							xPartTbl.SetGrid(i,6,"unknown");
#else
							xPartTbl.SetGrid(i,4,"未知");
							xPartTbl.SetGrid(i,5,"未知");
							xPartTbl.SetGrid(i,6,"未知");
#endif
						}
						tm_str.Printf("%.0f",pLineTube->GetLength());	//杆件长度
						xPartTbl.SetGrid(i,7,tm_str);
						tm_str.Printf("%d",pLineTube->feature);			//单基数
						xPartTbl.SetGrid(i,8,tm_str);
						tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
						xPartTbl.SetGrid(i,9,tm_str);
						if(pStartParamPlate&&pEndParamPlate)
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,10,"Cut slot on both ends");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,10,"cut slot on one end");
#else
							xPartTbl.SetGrid(i,10,"两端割槽");
						else if(pStartParamPlate||pEndParamPlate)
							xPartTbl.SetGrid(i,10,"一端割槽");
#endif
					}
					else if(nSketchType==SKETCH_RUSLOT||nSketchType==SKETCH_RXSLOT||nSketchType==SKETCH_RTSLOT)
					{	//一/T型开口槽  一/U型开口槽  一/X型开口槽
						CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
						pStartParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
						pEndParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
						if(pStartParamPlate==NULL||pEndParamPlate==NULL)
							continue;
						if(pStartParamPlate->m_iParamType!=TYPE_ROLLEND&&pEndParamPlate->m_iParamType!=TYPE_ROLLEND)
							continue;
						if(pStartParamPlate->m_iParamType!=TYPE_ROLLEND)
						{	//确保pStartParamPlate中为槽型插板
							CLDSParamPlate *pParamPlate=pStartParamPlate;
							pStartParamPlate=pEndParamPlate;
							pEndParamPlate=pParamPlate;
						}
						if(pStartParamPlate->m_iParamType!=TYPE_ROLLEND&&(pEndParamPlate->m_iParamType!=TYPE_UEND||
							pEndParamPlate->m_iParamType!=TYPE_XEND))
							continue;
						double M=0,W=0;
						xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
						QuerySteelMatMark(pPart->cMaterial,tm_str);	//材质
						xPartTbl.SetGrid(i,1,tm_str);
						xPartTbl.SetGrid(i,2,spec);
						//左端
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,3,"one shape slot");
#else
						xPartTbl.SetGrid(i,3,"一型槽");
#endif
						pStartParamPlate->GetDesignItemValue('M',&M);
						pStartParamPlate->GetDesignItemValue('W',&W);
						tm_str.Printf("%.0f",M+fStartSlotLenProcessError);
						xPartTbl.SetGrid(i,4,tm_str);
						tm_str.Printf("%.1f",pStartParamPlate->GetThick()+fStartSlotWidthProcessError);
						SimplifiedNumString(tm_str);
						xPartTbl.SetGrid(i,5,tm_str);
						//右端
						if(pEndParamPlate->m_iParamType==TYPE_UEND)
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,6,"U shape slot");
						else if(pEndParamPlate->m_iParamType==TYPE_XEND)
							xPartTbl.SetGrid(i,6,"cross shape slot");
#else
							xPartTbl.SetGrid(i,6,"U型槽");
						else if(pEndParamPlate->m_iParamType==TYPE_XEND)
							xPartTbl.SetGrid(i,6,"十字型槽");
#endif
						pEndParamPlate->GetDesignItemValue('M',&M);
						pEndParamPlate->GetDesignItemValue('W',&W);
						tm_str.Printf("%.0f",M+fEndSlotLenProcessError);
						xPartTbl.SetGrid(i,7,tm_str);
						//槽宽
						if(pEndParamPlate->m_iParamType==TYPE_UEND)
							tm_str.Printf("%.1f",pEndParamPlate->GetThick()*2+W+fEndSlotWidthProcessError);
						else
							tm_str.Printf("%.1f",pEndParamPlate->GetThick()+fEndSlotWidthProcessError);
						SimplifiedNumString(tm_str);
						xPartTbl.SetGrid(i,8,tm_str);
						//槽弧R/槽宽B
						if(pEndParamPlate->m_iParamType==TYPE_UEND)
							tm_str.Printf("%.1f",pEndParamPlate->GetThick()+0.5*(W+fEndSlotWidthProcessError));
						else
							tm_str.Printf("%.1f",pEndParamPlate->GetThick()+fEndSlotWidthProcessError);
						SimplifiedNumString(tm_str);
						xPartTbl.SetGrid(i,9,tm_str);
						
						tm_str.Printf("%.0f",pLineTube->GetLength());	//杆件长度
						xPartTbl.SetGrid(i,10,tm_str);
						tm_str.Printf("%d",pLineTube->feature);			//单基数
						xPartTbl.SetGrid(i,11,tm_str);
						tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
						xPartTbl.SetGrid(i,12,tm_str);
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,13,"Cut slot on both ends");
#else
						xPartTbl.SetGrid(i,13,"两端割槽");
#endif
					}
					else if(nSketchType==SKETCH_TXSLOT||nSketchType==SKETCH_TUSLOT||nSketchType==SKETCH_UXSLOT)
					{	//T/X型开口槽  T/U型开口槽  U/X型开口槽
						CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
						pStartParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
						pEndParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
						if(pStartParamPlate==NULL||pEndParamPlate==NULL)
							continue;
						//当前未考虑T型槽
						if(pStartParamPlate->m_iParamType!=TYPE_UEND&&pEndParamPlate->m_iParamType!=TYPE_UEND)
							continue;
						if(pStartParamPlate->m_iParamType!=TYPE_UEND)
						{	//确保pStartParamPlate中为U型插板
							CLDSParamPlate *pParamPlate=pStartParamPlate;
							pStartParamPlate=pEndParamPlate;
							pEndParamPlate=pParamPlate;
						}
						if(pStartParamPlate->m_iParamType!=TYPE_UEND&&pEndParamPlate->m_iParamType!=TYPE_XEND)
							continue;	//U/X
						double M=0,W=0;
						xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
						QuerySteelMatMark(pPart->cMaterial,tm_str);	//材质
						xPartTbl.SetGrid(i,1,tm_str);
						xPartTbl.SetGrid(i,2,spec);
						//左端
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,3,"U slot");
#else
						xPartTbl.SetGrid(i,3,"U型槽");
#endif
						pStartParamPlate->GetDesignItemValue('M',&M);
						pStartParamPlate->GetDesignItemValue('W',&W);
						tm_str.Printf("%.0f",M+fStartSlotLenProcessError);
						xPartTbl.SetGrid(i,4,tm_str);	
						tm_str.Printf("%.1f",W+2*pStartParamPlate->GetThick()+fStartSlotWidthProcessError);
						SimplifiedNumString(tm_str);
						xPartTbl.SetGrid(i,5,tm_str);	
						tm_str.Printf("%.1f",0.5*(W+fStartSlotWidthProcessError)+pStartParamPlate->GetThick());
						SimplifiedNumString(tm_str);
						xPartTbl.SetGrid(i,6,tm_str);
						//右端
						if(pEndParamPlate->m_iParamType==TYPE_XEND)
#ifdef AFX_TARG_ENU_ENGLISH
							xPartTbl.SetGrid(i,7,"cross slot");
#else
							xPartTbl.SetGrid(i,7,"十字型槽");
#endif
						pEndParamPlate->GetDesignItemValue('M',&M);
						pEndParamPlate->GetDesignItemValue('W',&W);
						tm_str.Printf("%.0f",M+fEndSlotLenProcessError);
						xPartTbl.SetGrid(i,8,tm_str);
						tm_str.Printf("%.1f",pEndParamPlate->GetThick()+fEndSlotWidthProcessError);
						SimplifiedNumString(tm_str);
						xPartTbl.SetGrid(i,9,tm_str);
						tm_str.Printf("%.1f",pEndParamPlate->GetThick()+fEndSlotWidthProcessError);
						SimplifiedNumString(tm_str);
						xPartTbl.SetGrid(i,10,tm_str);
						
						tm_str.Printf("%.0f",pLineTube->GetLength());	//杆件长度
						xPartTbl.SetGrid(i,11,tm_str);
						tm_str.Printf("%d",pLineTube->feature);			//单基数
						xPartTbl.SetGrid(i,12,tm_str);
						tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
						xPartTbl.SetGrid(i,13,tm_str);
#ifdef AFX_TARG_ENU_ENGLISH
						xPartTbl.SetGrid(i,14,"cut slot on both side");
#else
						xPartTbl.SetGrid(i,14,"两端割槽");
#endif
					}
					i++;
				}
				partset.Empty();
				xPartTbl.SetBasePos(tbl_pos);
				DrawTable(pBlockTableRecord,&xPartTbl,FALSE,1);
			}
			iPart++;
		}
	}
	if(hInstance)
		FreeLibrary(hInstance);
	pBlockTableRecord->close();
}

AcDbObjectId NewSpline(AcDbBlockTableRecord *pBlockTableRecord,
					   fPtList& vertex_list,int order=4,double fitTolerance=0.0)
{
	AcDbObjectId outputId;
	AcGePoint3dArray fitPoints;
	for(f3dPoint *pVert=vertex_list.GetFirst();pVert;pVert=vertex_list.GetNext())
		fitPoints.append(AcGePoint3d(pVert->x,pVert->y,pVert->z));
	AcDbSpline *pSpline=new AcDbSpline(fitPoints,4,0);//创建SPline样条曲线对象
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,outputId,pSpline))
		pSpline->close();//调用关闭对象的成员函数
	return outputId;
}
//para[0]:输出外壁相贯展开线;para[1]:输出内壁相贯展开线;para[2]:输出内外壁合成的内外壁相贯切割线
void DrawTubeSpreadMap(CLDSLineTube *pTube,BOOL *para,int iOrder,int nHits,
					   char *layer,int nPageNum/*=1*/,int nTaNum/*=1*/,BOOL bDimPartInfo/*=TRUE*/)
{
	//获得当前图形的块表指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	//计算原点位置
	f3dPoint origin;	//原点坐标为(0,0,0)
	if(iOrder>0)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}

	UINT i,n,uSlices;
	double length=pTube->GetLength();
	//计算弧长方向的抽样间隔长度
	if(pTube->m_uSlices>0)
		uSlices=pTube->m_uSlices;
	else
	{
		double fSamplingLength=2.5;
		n = ftoi(Pi*pTube->GetDiameter()/fSamplingLength);
		uSlices=max(n,6);
		uSlices=min(n,288);
	}
	//计算弧长周向各样点的切割长度
	double uncut_drawlen=100;	//绘制两端相贯线图时,未被切割部分的绘制长度,为程序指定无特殊含义
	double symbol_width=5;		//破断符号大小
	double breakSpace=15;		//杆件一般较长,在中间需要打断出图时就会生成两条破断线,此值为两破断线间隙值
	double piece_angle=2*Pi/uSlices;
	double* start_cutlen_arr = new double[uSlices];
	double* end_cutlen_arr = new double[uSlices];
	f3dPoint *start_vert_arr=new f3dPoint[uSlices+1];
	f3dPoint *end_vert_arr=new f3dPoint[uSlices+1];
	f3dPoint line_pick_start,line_pick_start2,line_pick,line_vec,inters1,inters2;
	double outter_radius=pTube->GetDiameter()*0.5;
	double inner_radius=outter_radius-pTube->GetThick();
	pTube->BuildUCS();
	f3dPoint axis_origin=pTube->ucs.origin;
	f3dPoint axis_end=pTube->ucs.origin+pTube->ucs.axis_z*pTube->GetLength();
	double start_left_y=0,end_left_y=0,start_right_in_y=0,start_right_out_y=0,
		end_right_in_y=0,end_right_out_y=0;
	double start_max_cutlen=0,end_max_cutlen=0,start_min_cutlen=0,end_min_cutlen=0,sum_cutlen=0;
	double startedge_cutlen[4]={0};	//[0]:始端外侧;[1]:终端外侧;[2]:始端内侧;[3]:终端内侧
	BOOL bInit=FALSE;
	double weldRoadAngle=0;
	if(pTube->m_bHasWeldRoad)
		weldRoadAngle=pTube->CalWeldRoadAngle();
	for(int in0_out1=0;in0_out1<3;in0_out1++)
	{
		if(!para[in0_out1])
			continue;
		double radius=0;
		if(in0_out1==1)
		{	//绘制外壁相贯线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			radius=outter_radius;
		}
		else if(in0_out1==0)
		{	//绘制内壁相贯线
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			radius=inner_radius;
		}
		else 
		{	//绘制内外壁合成的切割线图
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
			radius=outter_radius;
		}
		memset(start_cutlen_arr,0,sizeof(double)*uSlices);
		memset(end_cutlen_arr,0,sizeof(double)*uSlices);
		BOOL bHasStartCutPlane=FALSE;
		line_vec=pTube->Start()-pTube->End();
		normalize(line_vec);
		if(pTube->cutPlaneNorm*line_vec>0)
			bHasStartCutPlane=TRUE;
		if((pTube->m_tJointStart.type==0||(pTube->m_tJointStart.type==1&&pTube->m_tJointStart.hViceLinkObj<0))&&
			!pTube->m_tJointStart.cutPlaneNorm.IsZero()) //平面切割或对接相贯
		{
			line_pick_start=pTube->ucs.origin+pTube->ucs.axis_x*radius;
			for(i=0;i<uSlices;i++)
			{
				line_pick=line_pick_start;
				if(i>0)
					rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
				if(Int3dlf(inters1,line_pick,line_vec,pTube->m_tJointStart.cutPlanePos,pTube->m_tJointStart.cutPlaneNorm)>0)
				{
					f3dPoint vec=inters1-pTube->Start();
					double dd=vec*line_vec;
					if(dd<pTube->startOdd())
						start_cutlen_arr[i]=pTube->startOdd()-dd;	//第一个交点有效
					else
						start_cutlen_arr[i]=0;	//无有效交点
				}
				else
					start_cutlen_arr[i]=0;	//无有效交点
				if(bHasStartCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
				{
					f3dPoint vec=inters1-pTube->Start();
					double dd=vec*line_vec;
					if(start_cutlen_arr[i]<pTube->startOdd()-dd)
						start_cutlen_arr[i]=pTube->startOdd()-dd;	//第一个交点有效
				}
			}
			if(in0_out1==2)	//输出合成线
			{	//需要检查内壁的切割情况
				line_pick_start=pTube->ucs.origin+pTube->ucs.axis_x*inner_radius;
				for(i=0;i<uSlices;i++)
				{
					line_pick=line_pick_start;
					if(i>0)
						rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
					if(Int3dlf(inters1,line_pick,line_vec,pTube->m_tJointStart.cutPlanePos,pTube->m_tJointStart.cutPlaneNorm)>0)
					{
						f3dPoint vec=inters1-pTube->Start();
						double dd=vec*line_vec;
						if(start_cutlen_arr[i]<pTube->startOdd()-dd)
							start_cutlen_arr[i]=pTube->startOdd()-dd;	//第一个交点有效
					}
					if(bHasStartCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
					{
						f3dPoint vec=inters1-pTube->Start();
						double dd=vec*line_vec;
						if(start_cutlen_arr[i]<pTube->startOdd()-dd)
							start_cutlen_arr[i]=pTube->startOdd()-dd;	//第一个交点有效
					}
				}
			}
		}
		else if(pTube->m_tJointStart.hLinkObj>0&&pTube->m_tJointStart.type==1&&pTube->m_tJointStart.hViceLinkObj>=0)	
		{	//钢管相贯(对接相贯在平面切割处处理)
			CLDSLineTube *pTransTube=(CLDSLineTube*)Ta.FromPartHandle(pTube->m_tJointStart.hLinkObj,CLS_LINETUBE);
			double trans_radius=pTransTube->GetDiameter()*0.5;
			line_vec=pTube->Start()-pTube->End();
			normalize(line_vec);
			line_pick_start=pTube->ucs.origin+pTube->ucs.axis_x*radius;
			for(i=0;i<uSlices;i++)
			{
				line_pick=line_pick_start;
				if(i>0)
					rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
				if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius)>0)
				{
					f3dPoint vec1=inters1-pTube->Start();
					f3dPoint vec2=inters2-pTube->Start();
					double dd1=vec1*line_vec;
					double dd2=vec2*line_vec;
					if(dd1<dd2&&dd1<pTube->startOdd())
						start_cutlen_arr[i]=pTube->startOdd()-dd1;	//第一个交点有效
					else if(dd2<dd1&&dd2<pTube->startOdd())
						start_cutlen_arr[i]=pTube->startOdd()-dd2;	//第二个交点有效
					else
						start_cutlen_arr[i]=0;	//无有效交点
				}
				else
					start_cutlen_arr[i]=0;	//无有效交点
				if(bHasStartCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
				{
					f3dPoint vec=inters1-pTube->Start();
					double dd=vec*line_vec;
					if(start_cutlen_arr[i]<pTube->startOdd()-dd)
						start_cutlen_arr[i]=pTube->startOdd()-dd;	//第一个交点有效
				}
			}
			if(in0_out1==2)	//输出合成线
			{	//需要检查内壁的切割情况
				line_pick_start=pTube->ucs.origin+pTube->ucs.axis_x*inner_radius;
				for(i=0;i<uSlices;i++)
				{
					line_pick=line_pick_start;
					if(i>0)
						rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
					if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius)>0)
					{
						f3dPoint vec1=inters1-pTube->Start();
						f3dPoint vec2=inters2-pTube->Start();
						double dd1=vec1*line_vec;
						double dd2=vec2*line_vec;
						if(dd1<dd2&&dd1<pTube->startOdd())
						{
							if(start_cutlen_arr[i]<pTube->startOdd()-dd1)
								start_cutlen_arr[i]=pTube->startOdd()-dd1;	//第一个交点有效
						}
						else if(dd2<dd1&&dd2<pTube->startOdd())
						{
							if(start_cutlen_arr[i]<pTube->startOdd()-dd2)
								start_cutlen_arr[i]=pTube->startOdd()-dd2;	//第二个交点有效
						}
					}
					if(bHasStartCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
					{
						f3dPoint vec=inters1-pTube->Start();
						double dd=vec*line_vec;
						if(start_cutlen_arr[i]<pTube->startOdd()-dd)
							start_cutlen_arr[i]=pTube->startOdd()-dd;	//第一个交点有效
					}
				}
			}
		}
		line_vec=pTube->End()-pTube->Start();
		normalize(line_vec);
		BOOL bHasEndCutPlane=FALSE;
		if(pTube->cutPlaneNorm*line_vec>0)
			bHasEndCutPlane=TRUE;
		if((pTube->m_tJointEnd.type==0||(pTube->m_tJointEnd.type==1&&pTube->m_tJointEnd.hViceLinkObj<0))&&
			!pTube->m_tJointEnd.cutPlaneNorm.IsZero())//平面切割或对接相贯
		{
			line_pick_start=pTube->End()+pTube->ucs.axis_x*radius;
			for(i=0;i<uSlices;i++)
			{
				line_pick=line_pick_start;
				if(i>0)
					rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
				if(Int3dlf(inters1,line_pick,line_vec,pTube->m_tJointEnd.cutPlanePos,pTube->m_tJointEnd.cutPlaneNorm)>0)
				{
					f3dPoint vec=inters1-pTube->End();
					double dd=vec*line_vec;
					if(dd<pTube->endOdd())
						end_cutlen_arr[i]=pTube->endOdd()-dd;	//第一个交点有效
					else
						end_cutlen_arr[i]=0;					//无有效交点
				}
				else
					end_cutlen_arr[i]=0;
				if(bHasEndCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
				{
					f3dPoint vec=inters1-pTube->End();
					double dd=vec*line_vec;
					if(end_cutlen_arr[i]<pTube->endOdd()-dd)
						end_cutlen_arr[i]=pTube->endOdd()-dd;	//第一个交点有效
				}
			}
			if(in0_out1==2)	//输出合成线
			{	//需要检查内壁的切割情况
				line_pick_start=pTube->End()+pTube->ucs.axis_x*inner_radius;
				for(i=0;i<uSlices;i++)
				{
					line_pick=line_pick_start;
					if(i>0)
						rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
					if(Int3dlf(inters1,line_pick,line_vec,pTube->m_tJointEnd.cutPlanePos,pTube->m_tJointEnd.cutPlaneNorm)>0)
					{
						f3dPoint vec=inters1-pTube->End();
						double dd=vec*line_vec;
						if(end_cutlen_arr[i]<pTube->endOdd()-dd)
							end_cutlen_arr[i]=pTube->endOdd()-dd;	//第一个交点有效
					}
					if(bHasEndCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
					{
						f3dPoint vec=inters1-pTube->End();
						double dd=vec*line_vec;
						if(end_cutlen_arr[i]<pTube->endOdd()-dd)
							end_cutlen_arr[i]=pTube->endOdd()-dd;	//第一个交点有效
					}
				}
			}
		}
		else if(pTube->m_tJointEnd.hLinkObj>0&&pTube->m_tJointEnd.type==1&&pTube->m_tJointEnd.hViceLinkObj>=0)	
		{	//钢管相贯(对接相贯在平面切割处处理)
			CLDSLineTube *pTransTube=(CLDSLineTube*)Ta.FromPartHandle(pTube->m_tJointEnd.hLinkObj,CLS_LINETUBE);
			double trans_radius=pTransTube->GetDiameter()*0.5;
			line_vec=pTube->End()-pTube->Start();
			normalize(line_vec);
			line_pick_start=pTube->End()+pTube->ucs.axis_x*radius;
			for(i=0;i<uSlices;i++)
			{
				line_pick=line_pick_start;
				if(i>0)
					rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
				if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius)>0)
				{
					f3dPoint vec1=inters1-pTube->End();
					f3dPoint vec2=inters2-pTube->End();
					double dd1=vec1*line_vec;
					double dd2=vec2*line_vec;
					if(dd1<dd2&&dd1<pTube->endOdd())
						end_cutlen_arr[i]=pTube->endOdd()-dd1;	//第一个交点有效
					else if(dd2<dd1&&dd2<pTube->endOdd())
						end_cutlen_arr[i]=pTube->endOdd()-dd2;	//第二个交点有效
					else
						end_cutlen_arr[i]=0;					//无有效交点
				}
				else
					end_cutlen_arr[i]=0;
				if(bHasEndCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
				{
					f3dPoint vec=inters1-pTube->End();
					double dd=vec*line_vec;
					if(end_cutlen_arr[i]<pTube->endOdd()-dd)
						end_cutlen_arr[i]=pTube->endOdd()-dd;	//第一个交点有效
				}
			}
			if(in0_out1==2)	//输出合成线
			{	//需要检查内壁的切割情况
				line_pick_start=pTube->End()+pTube->ucs.axis_x*inner_radius;
				for(i=0;i<uSlices;i++)
				{
					line_pick=line_pick_start;
					if(i>0)
						rotate_point_around_axis(line_pick,piece_angle*i,axis_origin,axis_end);
					if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius)>0)
					{
						f3dPoint vec1=inters1-pTube->End();
						f3dPoint vec2=inters2-pTube->End();
						double dd1=vec1*line_vec;
						double dd2=vec2*line_vec;
						if(dd1<dd2&&dd1<pTube->endOdd())
						{
							if(end_cutlen_arr[i]<pTube->endOdd()-dd1)
								end_cutlen_arr[i]=pTube->endOdd()-dd1;	//第一个交点有效
						}
						else if(dd2<dd1&&dd2<pTube->endOdd())
						{
							if(end_cutlen_arr[i]<pTube->endOdd()-dd2)
								end_cutlen_arr[i]=pTube->endOdd()-dd2;	//第二个交点有效
						}
					}
					if(bHasEndCutPlane&&Int3dlf(inters1,line_pick,line_vec,pTube->cutPlaneOrigin,pTube->cutPlaneNorm)>0)
					{
						f3dPoint vec=inters1-pTube->End();
						double dd=vec*line_vec;
						if(end_cutlen_arr[i]<pTube->endOdd()-dd)
							end_cutlen_arr[i]=pTube->endOdd()-dd;	//第一个交点有效
					}
				}
			}
		}
		if(in0_out1==1)
		{
			startedge_cutlen[0]=start_cutlen_arr[0];
			startedge_cutlen[1]=end_cutlen_arr[0];
		}
		else
		{
			startedge_cutlen[2]=start_cutlen_arr[0];
			startedge_cutlen[3]=end_cutlen_arr[0];
		}
		if(!bInit)
		{	//赋初始切割长度值
			bInit=TRUE;
			start_max_cutlen=start_min_cutlen=start_cutlen_arr[0];
			end_max_cutlen=end_min_cutlen=end_cutlen_arr[0];
			for(i=1;i<uSlices;i++)
			{
				if(start_cutlen_arr[i]<start_min_cutlen)
					start_min_cutlen=start_cutlen_arr[i];
				if(start_cutlen_arr[i]>start_max_cutlen)
					start_max_cutlen=start_cutlen_arr[i];
				if(end_cutlen_arr[i]<end_min_cutlen)
					end_min_cutlen=end_cutlen_arr[i];
				if(end_cutlen_arr[i]>end_max_cutlen)
					end_max_cutlen=end_cutlen_arr[i];
			}
			/*  暂时考虑到钢管一端被两根钢管相贯时需要以正头设计位置为定位基准
				不启用这两项用于减少下料高度的属性
			*/
			//start_min_cutlen=end_min_cutlen=0;
			sum_cutlen=start_max_cutlen-start_min_cutlen+end_max_cutlen-end_min_cutlen;
		}
		for(i=0;i<uSlices;i++)
		{
			start_vert_arr[i].x=origin.x+i*piece_angle*radius;
			start_vert_arr[i].y=origin.y-start_cutlen_arr[i];//+start_min_cutlen;
			end_vert_arr[i].x=origin.x+i*piece_angle*radius;
			end_vert_arr[i].y=origin.y-start_max_cutlen-(symbol_width+uncut_drawlen)*2-breakSpace-end_max_cutlen+end_cutlen_arr[i];
		}
		start_vert_arr[uSlices].Set(2*Pi*radius,start_vert_arr[0].y);
		end_vert_arr[uSlices].Set(2*Pi*radius,end_vert_arr[0].y);
		
		AcGePoint3dArray startFitPoints,endFitPoints;
		BOOL bEnableStartSpline=FALSE,bEnableEndSpline=FALSE;
		AcDbObjectId outputId;
		f3dPoint lineUpStart,lineUpEnd,lineDownStart,lineDownEnd;
		fPtList startvertex_list,endvertex_list;
		for(i=0;i<uSlices+1;i++)
		{
			if(pTube->m_tJointStart.type<2)
			{
				if(i==0)
				{
					if(start_cutlen_arr[0]>0||start_cutlen_arr[1]>0)
					{
						bEnableStartSpline=TRUE;
						startvertex_list.append(start_vert_arr[0]);
					}
					else
					{
						bEnableStartSpline=FALSE;
						lineUpStart=lineUpEnd=start_vert_arr[0];
					}
				}
				else
				{
					if(bEnableStartSpline)	//当前状态为样条曲线加入特征顶点
					{
						startvertex_list.append(start_vert_arr[i]);
						if(start_cutlen_arr[i%uSlices]<EPS2||i==uSlices)	//由样条曲线结束,相贯边终止或将变为直线,此时需要绘制样条曲线
						{
							NewSpline(pBlockTableRecord,startvertex_list);
							startvertex_list.Empty();
							if(start_cutlen_arr[(i+1)%uSlices]>=EPS2)
							{
								startvertex_list.append(start_vert_arr[i]);
								bEnableStartSpline=TRUE;
							}
							else
							{
								bEnableStartSpline=FALSE;
								lineUpStart=lineUpEnd=start_vert_arr[i];
							}
						}
						//else	//延续样条曲线
					}
					else	//当前状态为直线边
					{
						lineUpEnd=start_vert_arr[i];
						if(start_cutlen_arr[i%uSlices]>=EPS2||i==uSlices)
						{		//直线边结束,相贯边变为样条曲线或将终止,此时需要绘制直线
							bEnableStartSpline=TRUE;
							startvertex_list.append(start_vert_arr[i]);
							if(lineUpStart!=lineUpEnd)
								CreateAcadLine(pBlockTableRecord,lineUpStart,lineUpEnd);
						}
						//else	//延续直线边
					}
				}
			}
			if(pTube->m_tJointEnd.type<2)
			{
				//终端轮廓边
				if(i==0)
				{
					if(end_cutlen_arr[0]>0||end_cutlen_arr[1]>0)
					{
						bEnableEndSpline=TRUE;
						endvertex_list.append(end_vert_arr[0]);
					}
					else
					{
						bEnableEndSpline=FALSE;
						lineDownStart=lineDownEnd=end_vert_arr[0];
					}
				}
				else
				{
					if(bEnableEndSpline)	//当前状态为样条曲线加入特征顶点
					{
						endvertex_list.append(end_vert_arr[i]);
						if(end_cutlen_arr[i%uSlices]<EPS2||i==uSlices)	//由样条曲线结束,相贯边终止或将变为直线,此时需要绘制样条曲线
						{
							NewSpline(pBlockTableRecord,endvertex_list);
							endvertex_list.Empty();
							if(end_cutlen_arr[(i+1)%uSlices]>=EPS2)
							{
								bEnableEndSpline=TRUE;
								endvertex_list.append(end_vert_arr[i]);
							}
							else
							{
								bEnableEndSpline=FALSE;
								lineDownStart=lineDownEnd=end_vert_arr[i];
							}
						}
						//else	//延续样条曲线
					}
					else	//当前状态为直线边
					{
						lineDownEnd=end_vert_arr[i];
						if(end_cutlen_arr[i%uSlices]>=EPS2||i==uSlices)
						{		//直线边结束,相贯边变为样条曲线或将终止,此时需要绘制直线
							bEnableEndSpline=TRUE;
							if(lineDownStart!=lineDownEnd)
								CreateAcadLine(pBlockTableRecord,lineDownStart,lineDownEnd);
						}
						//else	//延续直线边
					}
				}
			}
		}
		//计算左边线和右边线参数
		if(start_cutlen_arr[0]>start_left_y)
			start_left_y=start_cutlen_arr[0];
		if(end_cutlen_arr[0]>end_left_y)
			end_left_y=end_cutlen_arr[0];
		if(in0_out1==0)
		{	//内壁右边线
			start_right_in_y=start_cutlen_arr[0];
			end_right_in_y=end_cutlen_arr[0];
		}
		else
		{	//外壁或内外壁复合切割右边线
			start_right_out_y=start_cutlen_arr[0];
			end_right_out_y=end_cutlen_arr[0];
		}
	}
	start_left_y=origin.y;
	end_left_y=origin.y-start_max_cutlen-(symbol_width+uncut_drawlen)*2-breakSpace-end_max_cutlen;
	//绘制钢管展开图
	double start_out_y=origin.y-startedge_cutlen[0];
	double start_in_y=origin.y-startedge_cutlen[2];
	double start_y=origin.y-start_max_cutlen-uncut_drawlen-5;
	double end_out_y=start_y-breakSpace-symbol_width-uncut_drawlen-end_max_cutlen+startedge_cutlen[1];
	double end_in_y=start_y-breakSpace-symbol_width-uncut_drawlen-end_max_cutlen+startedge_cutlen[3];
	double end_y=start_y-breakSpace;
	double end_min_y=start_y-breakSpace-symbol_width-uncut_drawlen-end_max_cutlen;
	f3dPoint midpoint1(origin.x+outter_radius*0.5*Pi,origin.y);
	f3dPoint midpoint2(origin.x+outter_radius*1.5*Pi,origin.y);
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(pTube->m_tJointStart.type<2)
	{
		if(weldRoadAngle>0&&(para[0]||para[2]))
		{
			f3dLine roadLine(f3dPoint(origin.x+weldRoadAngle*outter_radius,origin.y),
				f3dPoint(origin.x+weldRoadAngle*outter_radius,start_y));
			CreateAcadLine(pBlockTableRecord,roadLine.startPt,roadLine.endPt);
#ifdef AFX_TARG_ENU_ENGLISH
			DimText(pBlockTableRecord,f3dPoint(roadLine.endPt.x,roadLine.endPt.y+40),"welding line",TextStyleTable::hzfs.textStyleId,5,-0.5*Pi,AcDb::kTextCenter,AcDb::kTextBottom);
#else
			DimText(pBlockTableRecord,f3dPoint(roadLine.endPt.x,roadLine.endPt.y+40),"焊道线",TextStyleTable::hzfs.textStyleId,5,-0.5*Pi,AcDb::kTextCenter,AcDb::kTextBottom);
#endif
		}
		//始端左侧边线
		CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,origin.y),f3dPoint(origin.x,start_y));
		//始端右侧内壁边线
		if(para[1])
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+inner_radius*2.0*Pi,start_in_y),
				f3dPoint(origin.x+inner_radius*2.0*Pi,start_y));
		GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		//始端顶部基准边线
		CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,origin.y),f3dPoint(origin.x+outter_radius*2.0*Pi,origin.y));
		//始端右侧外壁边线
		if(para[0]||para[2])
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+outter_radius*2.0*Pi,origin.y),
				f3dPoint(origin.x+outter_radius*2.0*Pi,start_y));
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		//始端中间线
		midpoint1.y=midpoint2.y=start_y;
		CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,start_y),f3dPoint(midpoint1.x-5,start_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x-5,start_y),f3dPoint(midpoint1.x,start_y+5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x,start_y+5),f3dPoint(midpoint1.x,start_y-5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x,start_y-5),f3dPoint(midpoint1.x+5,start_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x+5,start_y),f3dPoint(midpoint2.x-5,start_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x-5,start_y),f3dPoint(midpoint2.x,start_y+5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x,start_y+5),f3dPoint(midpoint2.x,start_y-5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x,start_y-5),f3dPoint(midpoint2.x+5,start_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x+5,start_y),f3dPoint(origin.x+outter_radius*2*Pi,start_y));
	}
	//中间线
	GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
	CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,start_y-0.5*breakSpace),f3dPoint(origin.x+outter_radius*2*Pi,start_y-0.5*breakSpace));
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(pTube->m_tJointEnd.type<2)
	{
		//终端中间线
		midpoint1.y=midpoint2.y=end_y;
		CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,end_y),f3dPoint(midpoint1.x-5,end_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x-5,end_y),f3dPoint(midpoint1.x,end_y+5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x,end_y+5),f3dPoint(midpoint1.x,end_y-5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x,end_y-5),f3dPoint(midpoint1.x+5,end_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint1.x+5,end_y),f3dPoint(midpoint2.x-5,end_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x-5,end_y),f3dPoint(midpoint2.x,end_y+5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x,end_y+5),f3dPoint(midpoint2.x,end_y-5));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x,end_y-5),f3dPoint(midpoint2.x+5,end_y));
		CreateAcadLine(pBlockTableRecord,f3dPoint(midpoint2.x+5,end_y),f3dPoint(origin.x+outter_radius*2*Pi,end_y));
		//终端左侧边线
		CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,end_min_y),f3dPoint(origin.x,end_y));
		//终端右侧内壁边线
		if(para[1])
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+inner_radius*2.0*Pi,end_in_y),
				f3dPoint(origin.x+inner_radius*2.0*Pi,end_y));
		GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		//终端右侧外壁边线
		if(para[0]||para[2])
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+outter_radius*2.0*Pi,end_min_y),
				f3dPoint(origin.x+outter_radius*2.0*Pi,end_y));
		if(weldRoadAngle>0&&(para[0]||para[2]))
		{
			f3dLine roadLine(f3dPoint(origin.x+weldRoadAngle*outter_radius,end_min_y),
				f3dPoint(origin.x+weldRoadAngle*outter_radius,end_y));
			CreateAcadLine(pBlockTableRecord,roadLine.startPt,roadLine.endPt);
#ifdef AFX_TARG_ENU_ENGLISH
			DimText(pBlockTableRecord,f3dPoint(roadLine.endPt.x,roadLine.endPt.y-30),"welding line",TextStyleTable::hzfs.textStyleId,5,-0.5*Pi,AcDb::kTextCenter,AcDb::kTextBottom);
#else
			DimText(pBlockTableRecord,f3dPoint(roadLine.endPt.x,roadLine.endPt.y-30),"焊道线",TextStyleTable::hzfs.textStyleId,5,-0.5*Pi,AcDb::kTextCenter,AcDb::kTextBottom);
#endif
		}
		//终端底部基准边线
		CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,end_min_y),f3dPoint(origin.x+outter_radius*2.0*Pi,end_min_y));
	}
	//标注钢管基本信息
	if(bDimPartInfo)
	{
		CXhChar100 sNotes, steelmark,guige;
		if(pTube->cMaterial!='S')
			QuerySteelMatMark(pTube->cMaterial,steelmark);
		sNotes.Printf("%s%%%%c%f",(char*)steelmark,pTube->GetDiameter());
		SimplifiedNumString(sNotes);
		guige.Printf("%sX%f",(char*)sNotes,pTube->GetThick());
		SimplifiedNumString(guige);
		length=DISTANCE(pTube->Start(),pTube->End())+pTube->GetStartActualOdd()+pTube->GetEndActualOdd();
		f3dPoint base;
#ifdef AFX_TARG_ENU_ENGLISH
		sNotes.Printf("tower type-%s part number. %s* %d  %s length>=%.1f mm",(char*)Ta.m_sTaType,(char*)GetPartNoIncMat(pTube->GetPartNo(),pTube->cMaterial),nHits,(char*)guige,length);
#else
		sNotes.Printf("塔型-%s 件号. %s* %d  %s 长度>=%.1f mm",(char*)Ta.m_sTaType,(char*)GetPartNoIncMat(pTube->GetPartNo(),pTube->cMaterial),nHits,(char*)guige,length);
#endif
		origin.x +=sys.dim_map.PartNo.fPartNoTextSize;
		origin.y = end_y-sys.dim_map.PartNo.fPartNoTextSize;
		base=origin;
		DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
	}
	pBlockTableRecord->close();
	delete []start_vert_arr;
	delete []end_vert_arr;
	delete []start_cutlen_arr;
	delete []end_cutlen_arr;
	//标注构件明细
	/*{
		char sNotes[200]="",sTemp[50]="";
		if(para[0])
			sNotes.Printf("%s ",Ta.m_sPrjName);
		if(para[1])
		{
			sTemp," %s",Ta.m_sTaAlias);
			sNotes,sTemp);
		}
		if(para[2])
		{
			sTemp," %s",Ta.m_sTaType);
			sNotes.Append(sTemp);
		}
		if(para[3])
		{
			sTemp," %s*%d ",pTube->GetPartNo(),nHits);
			sNotes.Append(sTemp);
		}
		if(para[4])
		{
			char steelmark[50];
			QuerySteelMatMark(pTube->cMaterial,steelmark);
			sTemp," %s ",steelmark);
			sNotes.Append(sTemp);
		}
		if(para[5])
		{
			sTemp,"-%d ",pTube->t);
			sNotes.Append(sTemp);
		}
		if(pPlate->m_iParamType==TYPE_FL)
		{
			L=H=(R+W)*2;
		}
		if(para[6])
		{
			sTemp.Printf("%.0f*%.0f(mm) ",L,H);
			sNotes.Append(sTemp);
		}
		if(para[7])
		{
			sTemp,"单重:%.2f(kg)",L*H*pPlate->thick*7.85e-6);
			sNotes.Append(sTemp);
		}
		if(para[8])
		{
			sTemp,"总重:%.2f(kg)",nHits*L*H*pPlate->thick*7.85e-6);
			sNotes.Append(sTemp);
		}
		TXTDIM_STRU txt_dim;
		DimText(pBlockTableRecord,ucs.origin+offset_ucs.origin,sNotes,TextStyleTable::hzfs.textStyleId,
			sys.dim_map.fPartNoTextSize*zoomCoef*4,0,AcDb::kTextCenter,AcDb::kTextBottom);
	}*/
}
/*分开绘制钢管相贯线
 *para[0]:输出外壁相贯展开线;
 *para[1]:输出内壁相贯展开线;
 *para[2]:输出内外壁合成的内外壁相贯切割线
 *para[3]:输出粗略加工相贯展开线
*/
double CalCutLenByCutPlane(f3dPoint base_pt,f3dPoint line_pick,f3dPoint line_vec,f3dPoint plane_pick,f3dPoint plane_norm)
{
	double fCutLen=0;
	f3dPoint intersPt;
	if(Int3dlf(intersPt,line_pick,line_vec,plane_pick,plane_norm)>0)
	{
		f3dPoint vec=intersPt-base_pt;
		fCutLen=vec*line_vec;
	}
	return fCutLen;
}
double CalCutLenByCylinder(f3dPoint base_pt,f3dPoint line_pick,f3dPoint line_vec,f3dPoint ptS,f3dPoint ptE,double R)
{
	double fCutLen=0;
	f3dPoint inters1,inters2;
	if(Int3dlc(&inters1,&inters2,line_pick,line_vec,ptS,ptE,R,R)>0)
	{
		f3dPoint vec1=inters1-base_pt;
		f3dPoint vec2=inters2-base_pt;
		double dd1=vec1*line_vec;
		double dd2=vec2*line_vec;
		if(dd1>dd2)
			fCutLen=dd1;	//第一个交点有效
		else if(dd2>dd1)
			fCutLen=dd2;	//第二个交点有效
		else
			fCutLen=0;	//无有效交点
	}
	return fCutLen;
}
void CalTubeSpreadCutLens(CLDSLineTube *pTube,BYTE biType,BOOL bCalStart,double* pCutLenArr,int nSlices)
{
	if(pTube==NULL)
		return;
	//初始化设计参数
	double fOuterR=pTube->GetDiameter()*0.5;
	double fInnerR=fOuterR-pTube->GetThick();
	double radius=(biType==0)?fInnerR:fOuterR;
	double piece_angle=2*Pi/nSlices;
	f3dPoint axis_ptS=pTube->ucs.origin;
	f3dPoint axis_ptE=pTube->ucs.origin+pTube->ucs.axis_z*pTube->GetLength();
	f3dPoint base_pt=bCalStart?pTube->Start():pTube->End();
	f3dPoint base_vec=pTube->ucs.axis_x;
	if(pTube->m_bHasWeldRoad)	//有焊道线，以焊道线为起始边
		RotateVectorAroundVector(base_vec,pTube->CalWeldRoadAngle(),pTube->ucs.axis_z);
	//
	f3dPoint line_pick,line_vec=(pTube->End()-pTube->Start()).normalized();
	if(!bCalStart)
		line_vec*=-1;
	BOOL bHasCutPlane=(pTube->cutPlaneNorm*line_vec>0)?TRUE:FALSE;
	int weld_space=(biType==3)?2:0;	//粗略加工时，考虑焊接间隙
	f3dPoint cutPickPt=pTube->cutPlaneOrigin+pTube->cutPlaneNorm*weld_space;
	TUBEJOINT *pCurJoint=bCalStart?(&pTube->m_tJointStart):(&pTube->m_tJointEnd);
	if((pCurJoint->type==0||(pCurJoint->type==1&&pCurJoint->hViceLinkObj<0))&&!pCurJoint->cutPlaneNorm.IsZero())
	{	 //平面切割或对接相贯
		f3dPoint cutPlanePos=pCurJoint->cutPlanePos+pCurJoint->cutPlaneNorm*weld_space;
		for(int i=0;i<nSlices;i++)
		{
			line_pick=base_pt+base_vec*radius;			
			rotate_point_around_axis(line_pick,piece_angle*i,axis_ptS,axis_ptE);
			pCutLenArr[i]=CalCutLenByCutPlane(base_pt,line_pick,line_vec,cutPlanePos,pCurJoint->cutPlaneNorm);
			if(bHasCutPlane)
			{
				double fCutLen=CalCutLenByCutPlane(base_pt,line_pick,line_vec,cutPickPt,pTube->cutPlaneNorm);
				if(pCutLenArr[i]<fCutLen)
					pCutLenArr[i]=fCutLen;
			}
		}
		if(biType==2||biType==3)
		{	//输出合成线|粗略加工线,需要检查内壁的切割情况
			for(int i=0;i<nSlices;i++)
			{
				line_pick=base_pt+base_vec*fInnerR;
				rotate_point_around_axis(line_pick,piece_angle*i,axis_ptS,axis_ptE);
				double fCutLen=CalCutLenByCutPlane(base_pt,line_pick,line_vec,cutPlanePos,pCurJoint->cutPlaneNorm);
				if(pCutLenArr[i]<fCutLen)
					pCutLenArr[i]=fCutLen;
				if(bHasCutPlane)
				{
					fCutLen=CalCutLenByCutPlane(base_pt,line_pick,line_vec,cutPickPt,pTube->cutPlaneNorm);
					if(pCutLenArr[i]<fCutLen)
						pCutLenArr[i]=fCutLen;
				}
			}
		}
	}
	else if(pCurJoint->hLinkObj>0&&pCurJoint->type==1&&pCurJoint->hViceLinkObj>=0)	
	{	//钢管相贯(对接相贯在平面切割处处理)
		CLDSLineTube *pTransTube=(CLDSLineTube*)Ta.FromPartHandle(pCurJoint->hLinkObj,CLS_LINETUBE);
		CLDSLineTube *pTransTube2=(CLDSLineTube*)Ta.FromPartHandle(pCurJoint->hViceLinkObj,CLS_LINETUBE);
		double trans_radius=pTransTube->GetDiameter()*0.5;
		double trans_radius2=pTransTube2?pTransTube2->GetDiameter()*0.5:0;
		double fCutLen=0;
		for(int i=0;i<nSlices;i++)
		{
			line_pick=base_pt+base_vec*radius;
			rotate_point_around_axis(line_pick,piece_angle*i,axis_ptS,axis_ptE);
			pCutLenArr[i]=CalCutLenByCylinder(base_pt,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius);
			if(pTransTube2)
			{
				fCutLen=CalCutLenByCylinder(base_pt,line_pick,line_vec,pTransTube2->Start(),pTransTube2->End(),trans_radius2);
				if(pCutLenArr[i]<fCutLen)
					pCutLenArr[i]=fCutLen;
			}
			if(bHasCutPlane)
			{
				fCutLen=CalCutLenByCutPlane(base_pt,line_pick,line_vec,cutPickPt,pTube->cutPlaneNorm);
				if(pCutLenArr[i]<fCutLen)
					pCutLenArr[i]=fCutLen;
			}
		}
		if(biType==2||biType==3)
		{	//输出合成线|粗略加工线,需要检查内壁的切割情况
			for(i=0;i<nSlices;i++)
			{
				line_pick=base_pt+base_vec*fInnerR;
				rotate_point_around_axis(line_pick,piece_angle*i,axis_ptS,axis_ptE);
				fCutLen=CalCutLenByCylinder(base_pt,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius+weld_space);
				if(pCutLenArr[i]<fCutLen)
					pCutLenArr[i]=fCutLen;
				if(pTransTube2)
				{
					fCutLen=CalCutLenByCylinder(base_pt,line_pick,line_vec,pTransTube2->Start(),pTransTube2->End(),trans_radius2+weld_space);
					if(pCutLenArr[i]<fCutLen)
						pCutLenArr[i]=fCutLen;
				}
				if(bHasCutPlane)
				{
					fCutLen=CalCutLenByCutPlane(base_pt,line_pick,line_vec,cutPickPt,pTube->cutPlaneNorm);
					if(pCutLenArr[i]<fCutLen)
						pCutLenArr[i]=fCutLen;
				}
			}
		}
	}
}
void DrawTubeSpreadMap2(CLDSLineTube *pTube,BOOL *para,int iOrder,int nHits,
	char *layer,int nPageNum/*=1*/,int nTaNum/*=1*/,BOOL bDimPartInfo/*=TRUE*/)
{
	if(gxDrwLocalizer.ciCustomerSerial==9)	//重庆江津定制绘制相贯展开图
		return CustomDrawTubeSpreadMap(pTube,para,iOrder,nHits,layer,nPageNum,nTaNum,bDimPartInfo);
	//
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pBlockTableRecord==NULL)
		return;
	//计算原点位置
	f3dPoint origin;
	if(iOrder>0)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}
	//初始化绘制参数
	pTube->BuildUCS();
	double fOuterR=pTube->GetDiameter()*0.5;
	double fInnerR=fOuterR-pTube->GetThick();
	double fWeldAngle=pTube->m_bHasWeldRoad?pTube->CalWeldRoadAngle():0;
	double fLen=2*Pi*fOuterR;
	double fKeepDrawLen=100;	//绘制两端相贯线图时,未被切割部分的绘制长度,为程序指定无特殊含义
	double fBreakSpace=20;		//杆件一般较长,在中间需要打断出图时就会生成两条破断线,此值为两破断线间隙值
	int uSlices=pTube->m_uSlices;
	if(uSlices<=0)
	{
		double fSamplingLength=2.5;
		int n = ftoi(Pi*pTube->GetDiameter()/fSamplingLength);
		uSlices=max(n,6);
		uSlices=min(n,288);
	}
	double piece_angle=2*Pi/uSlices;
	double* pCutLenS=new double[uSlices];
	double* pCutLenE=new double[uSlices];
	//根据设置，绘制相应的相贯展开面
	for(int index=0;index<4;index++)
	{
		if(!para[index])
			continue;
		double radius=(index==0)?fInnerR:fOuterR;
		//计算始|终端弧长周向各样点的切割长度
		memset(pCutLenS,0,sizeof(double)*uSlices);
		memset(pCutLenE,0,sizeof(double)*uSlices);
		CalTubeSpreadCutLens(pTube,index,TRUE,pCutLenS,uSlices);
		CalTubeSpreadCutLens(pTube,index,FALSE,pCutLenE,uSlices);
		//统计最大及最小割长度值
		double fMaxCutLenS=pCutLenS[0],fMaxCutLenE=pCutLenE[0];
		double fMinCutLenS=pCutLenS[0],fMinCutLenE=pCutLenE[0];
		for(int i=1;i<uSlices;i++)
		{
			if(pCutLenS[i]>fMaxCutLenS)
				fMaxCutLenS=pCutLenS[i];
			if(pCutLenS[i]<fMinCutLenS)
				fMinCutLenS=pCutLenS[i];
			if(pCutLenE[i]>fMaxCutLenE)
				fMaxCutLenE=pCutLenE[i];
			if(pCutLenE[i]<fMinCutLenE)
				fMinCutLenE=pCutLenE[i];
		}
		double fMaxOffDist=fMaxCutLenS+fKeepDrawLen+fBreakSpace+fMaxCutLenE;
		if(fMaxCutLenS>0)
			fMaxOffDist+=fKeepDrawLen;
		//计算始|终端弧长周向各样点坐标
		fPtList ptListS,ptListE;
		for(int i=0;i<=uSlices;i++)
		{
			f3dPoint pt;
			//始端取样点
			if(i<uSlices)
				pt.Set(origin.x+i*piece_angle*radius,origin.y-pCutLenS[i]);
			else
				pt.Set(origin.x+2*Pi*radius,origin.y-pCutLenS[0]);
			ptListS.append(pt);
			//终端取样点
			if(i<uSlices)
				pt.Set(origin.x+i*piece_angle*radius,origin.y-fMaxOffDist+pCutLenE[i]);
			else
				pt.Set(origin.x+2*Pi*radius,origin.y-fMaxOffDist+pCutLenE[0]);
			ptListE.append(pt);
		}
		//绘制相贯展开线
		if(index==0)		//内壁相贯线
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else if(index==1)	//外壁相贯线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		else if(index==2)	//内外壁合成切割线 
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		else				//粗略加工线
			GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
		BOOL bDrawSpline=FALSE;
		f3dPoint line_ptS,line_ptE;
		fPtList pt_list;
		//始端相贯展开线
		if(fMaxCutLenS>0)
		{
			pt_list.Empty();
			for(int i=0;i<uSlices+1;i++)
			{
				if(i==0)
				{
					bDrawSpline=(pCutLenS[0]>0||pCutLenS[1]>0)?TRUE:FALSE;
					if(bDrawSpline)
						pt_list.append(ptListS[0]);
					else
						line_ptS=line_ptE=ptListS[0];
				}
				else if(bDrawSpline)
				{	//当前状态为样条曲线加入特征顶点
					pt_list.append(ptListS[i]);
					if(pCutLenS[i%uSlices]<EPS2||i==uSlices)
					{	//由样条曲线结束,相贯边终止或将变为直线,此时需要绘制样条曲线
						NewSpline(pBlockTableRecord,pt_list);
						//
						pt_list.Empty();
						if(pCutLenS[(i+1)%uSlices]>=EPS2)
						{
							bDrawSpline=TRUE;
							pt_list.append(ptListS[i]);
						}
						else
						{
							bDrawSpline=FALSE;
							line_ptS=line_ptE=ptListS[i];
						}
					}
				}
				else	
				{	//当前状态为直线边
					line_ptE=ptListS[i];
					if(pCutLenS[i%uSlices]>=EPS2||i==uSlices)
					{	//直线边结束,相贯边变为样条曲线或将终止,此时需要绘制直线
						bDrawSpline=TRUE;
						pt_list.append(ptListS[i]);
						if(line_ptS!=line_ptE)
							CreateAcadLine(pBlockTableRecord,line_ptS,line_ptE);
					}
				}
			}
		}
		//终端相贯展开线
		if(fMaxCutLenE>0)
		{
			pt_list.Empty();
			for(int i=0;i<uSlices+1;i++)
			{	//终端轮廓边
				if(i==0)
				{
					bDrawSpline=(pCutLenE[0]>0||pCutLenE[1]>0)?TRUE:FALSE;
					if(bDrawSpline)
						pt_list.append(ptListE[0]);
					else
						line_ptS=line_ptE=ptListE[0];
				}
				else if(bDrawSpline)
				{	//当前状态为样条曲线加入特征顶点
					pt_list.append(ptListE[i]);
					if(pCutLenE[i%uSlices]<EPS2||i==uSlices)
					{	//由样条曲线结束,相贯边终止或将变为直线,此时需要绘制样条曲线
						NewSpline(pBlockTableRecord,pt_list);
						//
						pt_list.Empty();
						if(pCutLenE[(i+1)%uSlices]>=EPS2)
						{
							bDrawSpline=TRUE;
							pt_list.append(ptListE[i]);
						}
						else
						{
							bDrawSpline=FALSE;
							line_ptS=line_ptE=ptListE[i];
						}
					}
				}
				else	
				{	//当前状态为直线边
					line_ptE=ptListE[i];
					if(pCutLenE[i%uSlices]>=EPS2||i==uSlices)
					{		//直线边结束,相贯边变为样条曲线或将终止,此时需要绘制直线
						bDrawSpline=TRUE;
						if(line_ptS!=line_ptE)
							CreateAcadLine(pBlockTableRecord,line_ptS,line_ptE);
					}
				}
			}
		}
		//绘制钢管展开图
		double max_y=origin.y,min_y=origin.y-fMaxOffDist;
		double midPosX1=origin.x+radius*0.5*Pi,midPosX2=origin.x+radius*1.5*Pi;
		if(fMaxCutLenS>0)
		{	
			double ptS_y_m=max_y-pCutLenS[0],ptS_y=max_y-fMaxCutLenS-fKeepDrawLen;
			//始端顶部基准边线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,max_y),f3dPoint(origin.x+fOuterR*2.0*Pi,max_y));
			//始端左侧|右侧边线
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,ptS_y_m),f3dPoint(origin.x,ptS_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*2.0*Pi,ptS_y_m),f3dPoint(origin.x+radius*2.0*Pi,ptS_y));
			//始端中间线
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,ptS_y),f3dPoint(midPosX1-5,ptS_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1-5,ptS_y),f3dPoint(midPosX1,ptS_y+5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1,ptS_y+5),f3dPoint(midPosX1,ptS_y-5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1,ptS_y-5),f3dPoint(midPosX1+5,ptS_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1+5,ptS_y),f3dPoint(midPosX2-5,ptS_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2-5,ptS_y),f3dPoint(midPosX2,ptS_y+5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2,ptS_y+5),f3dPoint(midPosX2,ptS_y-5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2,ptS_y-5),f3dPoint(midPosX2+5,ptS_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2+5,ptS_y),f3dPoint(origin.x+radius*2*Pi,ptS_y));
		}
		//中间线
		double ptM_y=max_y-0.5*fBreakSpace;
		if(fMaxCutLenS>0)
			ptM_y-=(fMaxCutLenS+fKeepDrawLen);
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,ptM_y),f3dPoint(origin.x+fOuterR*2*Pi,ptM_y));
		if(fMaxCutLenE>0)
		{
			double ptE_y_m=min_y+pCutLenE[0],ptE_y=min_y+fMaxCutLenE+fKeepDrawLen;
			//终端底部基准边线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,min_y),f3dPoint(origin.x+fOuterR*2.0*Pi,min_y));
			//终端左侧|右侧边线
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,ptE_y_m),f3dPoint(origin.x,ptE_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*2.0*Pi,ptE_y_m),f3dPoint(origin.x+radius*2.0*Pi,ptE_y));
			//终端中间线
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,ptE_y),f3dPoint(midPosX1-5,ptE_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1-5,ptE_y),f3dPoint(midPosX1,ptE_y+5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1,ptE_y+5),f3dPoint(midPosX1,ptE_y-5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1,ptE_y-5),f3dPoint(midPosX1+5,ptE_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX1+5,ptE_y),f3dPoint(midPosX2-5,ptE_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2-5,ptE_y),f3dPoint(midPosX2,ptE_y+5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2,ptE_y+5),f3dPoint(midPosX2,ptE_y-5));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2,ptE_y-5),f3dPoint(midPosX2+5,ptE_y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(midPosX2+5,ptE_y),f3dPoint(origin.x+radius*2*Pi,ptE_y));
		}
		//标注钢管基本信息
		if(bDimPartInfo)
		{
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			f3dPoint base;
			CXhChar100 sNotes, steelmark,guige;
			if(pTube->cMaterial!='S')
				QuerySteelMatMark(pTube->cMaterial,steelmark);
			sNotes.Printf("%s%%%%c%f",(char*)steelmark,pTube->GetDiameter());
			SimplifiedNumString(sNotes);
			guige.Printf("%sX%f",(char*)sNotes,pTube->GetThick());
			SimplifiedNumString(guige);
			double length=pTube->GetLength();
#ifdef AFX_TARG_ENU_ENGLISH
			sNotes.Printf("tower type-%s part number. %s* %d  %s length>=%.1f mm",(char*)Ta.m_sTaType,(char*)GetPartNoIncMat(pTube->GetPartNo(),pTube->cMaterial),nHits,(char*)guige,length);
#else
			sNotes.Printf("塔型-%s 件号. %s* %d  %s 长度>=%.1f mm",(char*)Ta.m_sTaType,(char*)GetPartNoIncMat(pTube->GetPartNo(),pTube->cMaterial),nHits,(char*)guige,length);
#endif
			double fTestH=sys.dim_map.PartNo.fPartNoTextSize;
			double fTestL=TestDrawTextLength(sNotes,fTestH,TextStyleTable::hzfs.textStyleId);
			base.Set(origin.x+radius*Pi-0.5*fTestL,ptM_y+0.5*fTestH);
			DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
			//
			if(index==0)
				sNotes.Copy("内壁相贯线展开图");
			else if(index==1)
				sNotes.Copy("外壁相贯线展开图");
			else if(index==2)
				sNotes.Copy("内外壁合成切割线展开图");
			else
				sNotes.Copy("粗略加工相贯线展开图");
			fTestL=TestDrawTextLength(sNotes,fTestH,TextStyleTable::hzfs.textStyleId);
			base.Set(origin.x+radius*Pi-0.5*fTestL,ptM_y-2*fTestH);
			DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
		}
		//绘制坐标进行水平偏移
		origin.x+=(fLen+20);
	}
	pBlockTableRecord->close();
	//释放内存空间
	delete []pCutLenS;
	delete []pCutLenE;
}
/*重庆江津用户定制绘制相贯线： wxc-2018.4.17
 *两端都相贯的钢管，按照钢管实际长度绘制相贯线
 *只有一端相贯的钢管，截取整数长度绘制相贯线，并标明另一端的长度
*/
//根据最大切割长度计算保留长度
int CalKeepDist(double fMaxCutLen,int tube_len)
{ 
	int nKeepDist=0;
	int nValue=tube_len-(tube_len/100*100);
	if(fMaxCutLen<100)
		nKeepDist=500+nValue;
	else
	{
		double ff=fMaxCutLen+500;
		int v1=ftoi(ff)/100;
		nKeepDist=v1*100+nValue;
	}
	return nKeepDist;
}
void CustomDrawTubeSpreadMap(CLDSLineTube *pTube,BOOL *para,int iOrder,int nHits,
	char *layer,int nPageNum/*=1*/,int nTaNum/*=1*/,BOOL bDimPartInfo/*=TRUE*/)
{
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pBlockTableRecord==NULL)
		return;
	//计算原点位置
	f3dPoint origin;
	if(iOrder>0)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}
	//初始化绘制参数
	pTube->BuildUCS();
	double fOuterR=pTube->GetDiameter()*0.5;
	double fInnerR=fOuterR-pTube->GetThick();
	double fWeldAngle=pTube->m_bHasWeldRoad?pTube->CalWeldRoadAngle():0;
	double fLen=2*Pi*fOuterR;
	long tube_len=ftoi(DISTANCE(pTube->Start(),pTube->End()));
	int uSlices=pTube->m_uSlices;
	if(uSlices<=0)
	{
		double fSamplingLength=2.5;
		int n = ftoi(Pi*pTube->GetDiameter()/fSamplingLength);
		uSlices=max(n,6);
		uSlices=min(n,288);
	}
	int nMidIndex=uSlices/2;
	double piece_angle=2*Pi/uSlices;
	double* pCutLenS=new double[uSlices];
	double* pCutLenE=new double[uSlices];
	int ciDrawType=-1;	//0.始端相贯|1.终端相贯|2.两端相贯
	if(pTube->m_tJointStart.type<2)
		ciDrawType=0;
	if(pTube->m_tJointEnd.type<2)
		ciDrawType=(ciDrawType<0)?1:2;
	//根据设置，绘制相应的相贯展开面
	for(int index=0;index<4;index++)
	{
		if(!para[index])
			continue;
		double radius=(index==0)?fInnerR:fOuterR;
		//计算始|终端弧长周向各样点的切割长度
		memset(pCutLenS,0,sizeof(double)*uSlices);
		memset(pCutLenE,0,sizeof(double)*uSlices);
		CalTubeSpreadCutLens(pTube,index,TRUE,pCutLenS,uSlices);
		CalTubeSpreadCutLens(pTube,index,FALSE,pCutLenE,uSlices);
		//统计切割最大值及最大偏移长度
		double fMaxCutLenS=pCutLenS[0],fMaxCutLenE=pCutLenE[0];
		double fMinCutLenS=pCutLenS[0],fMinCutLenE=pCutLenE[0];
		for(int i=1;i<uSlices;i++)
		{
			if(pCutLenS[i]>fMaxCutLenS)
				fMaxCutLenS=pCutLenS[i];
			if(pCutLenS[i]<fMinCutLenS)
				fMinCutLenS=pCutLenS[i];
			if(pCutLenE[i]>fMaxCutLenE)
				fMaxCutLenE=pCutLenE[i];
			if(pCutLenE[i]<fMinCutLenE)
				fMinCutLenE=pCutLenE[i];
		}
		int nMaxOffDist=0;
		if(ciDrawType==2)
			nMaxOffDist=tube_len;
		else if(ciDrawType==0)
			nMaxOffDist=CalKeepDist(fMaxCutLenS,tube_len);
		else if(ciDrawType==1)
			nMaxOffDist=CalKeepDist(fMaxCutLenE,tube_len);
		//计算始|终端弧长周向各样点坐标
		fPtList ptListS,ptListE;
		for(int i=0;i<=uSlices;i++)
		{
			f3dPoint pt;
			//始端取样点
			if(i<uSlices)
				pt.Set(origin.x+i*piece_angle*radius,origin.y-pCutLenS[i]);
			else
				pt.Set(origin.x+2*Pi*radius,origin.y-pCutLenS[0]);
			ptListS.append(pt);
			//终端取样点
			if(i<uSlices)
				pt.Set(origin.x+i*piece_angle*radius,origin.y-nMaxOffDist+pCutLenE[i]);
			else
				pt.Set(origin.x+2*Pi*radius,origin.y-nMaxOffDist+pCutLenE[0]);
			ptListE.append(pt);
		}
		//绘制相贯展开线
		if(index==0)		//内壁相贯线
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else if(index==1)	//外壁相贯线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		else if(index==2)	//内外壁合成切割线 
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		else				//粗略加工线
			GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
		BOOL bDrawSpline=FALSE;
		f3dPoint line_ptS,line_ptE;
		fPtList pt_list;
		//始端相贯展开线
		if(ciDrawType!=1)
		{
			pt_list.Empty();
			for(int i=0;i<uSlices+1;i++)
			{
				if(i==0)
				{
					bDrawSpline=(pCutLenS[0]>0||pCutLenS[1]>0)?TRUE:FALSE;
					if(bDrawSpline)
						pt_list.append(ptListS[0]);
					else
						line_ptS=line_ptE=ptListS[0];
				}
				else if(bDrawSpline)
				{	//当前状态为样条曲线加入特征顶点
					pt_list.append(ptListS[i]);
					if(pCutLenS[i%uSlices]<EPS2||i==uSlices)
					{	//由样条曲线结束,相贯边终止或将变为直线,此时需要绘制样条曲线
						NewSpline(pBlockTableRecord,pt_list);
						//
						pt_list.Empty();
						if(pCutLenS[(i+1)%uSlices]>=EPS2)
						{
							bDrawSpline=TRUE;
							pt_list.append(ptListS[i]);
						}
						else
						{
							bDrawSpline=FALSE;
							line_ptS=line_ptE=ptListS[i];
						}
					}
				}
				else	
				{	//当前状态为直线边
					line_ptE=ptListS[i];
					if(pCutLenS[i%uSlices]>=EPS2||i==uSlices)
					{	//直线边结束,相贯边变为样条曲线或将终止,此时需要绘制直线
						bDrawSpline=TRUE;
						pt_list.append(ptListS[i]);
						if(line_ptS!=line_ptE)
							CreateAcadLine(pBlockTableRecord,line_ptS,line_ptE);
					}
				}
			}
		}
		//终端相贯展开线
		if(ciDrawType!=0)
		{
			pt_list.Empty();
			for(int i=0;i<uSlices+1;i++)
			{	//终端轮廓边
				if(i==0)
				{
					bDrawSpline=(pCutLenE[0]>0||pCutLenE[1]>0)?TRUE:FALSE;
					if(bDrawSpline)
						pt_list.append(ptListE[0]);
					else
						line_ptS=line_ptE=ptListE[0];
				}
				else if(bDrawSpline)
				{	//当前状态为样条曲线加入特征顶点
					pt_list.append(ptListE[i]);
					if(pCutLenE[i%uSlices]<EPS2||i==uSlices)
					{	//由样条曲线结束,相贯边终止或将变为直线,此时需要绘制样条曲线
						NewSpline(pBlockTableRecord,pt_list);
						//
						pt_list.Empty();
						if(pCutLenE[(i+1)%uSlices]>=EPS2)
						{
							bDrawSpline=TRUE;
							pt_list.append(ptListE[i]);
						}
						else
						{
							bDrawSpline=FALSE;
							line_ptS=line_ptE=ptListE[i];
						}
					}
				}
				else	
				{	//当前状态为直线边
					line_ptE=ptListE[i];
					if(pCutLenE[i%uSlices]>=EPS2||i==uSlices)
					{		//直线边结束,相贯边变为样条曲线或将终止,此时需要绘制直线
						bDrawSpline=TRUE;
						if(line_ptS!=line_ptE)
							CreateAcadLine(pBlockTableRecord,line_ptS,line_ptE);
					}
				}
			}
		}
		//绘制钢管轮廓边
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		f3dPoint weld_lien_ptS,weld_line_ptE;
		if(ciDrawType==2)
		{
			weld_lien_ptS.Set(origin.x,origin.y-pCutLenS[0]);
			weld_line_ptE.Set(origin.x,origin.y-nMaxOffDist+pCutLenE[0]);
			CreateAcadLine(pBlockTableRecord,weld_lien_ptS,weld_line_ptE);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*2.0*Pi,origin.y-pCutLenS[0]),f3dPoint(origin.x+radius*2.0*Pi,origin.y-nMaxOffDist+pCutLenE[0]));
			//CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,origin.y-fMinCutLenS),f3dPoint(origin.x+radius*2.0*Pi,origin.y-fMinCutLenS));
			//CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,origin.y-nMaxOffDist+fMinCutLenE),f3dPoint(origin.x+radius*2.0*Pi,origin.y-nMaxOffDist+fMinCutLenE));
			//中间线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*Pi,origin.y-pCutLenS[nMidIndex]),f3dPoint(origin.x+radius*Pi,origin.y-nMaxOffDist+pCutLenE[nMidIndex]));
		}
		else if(ciDrawType==0)
		{
			weld_lien_ptS.Set(origin.x,origin.y-pCutLenS[0]);
			weld_line_ptE.Set(origin.x,origin.y-nMaxOffDist);
			CreateAcadLine(pBlockTableRecord,weld_lien_ptS,weld_line_ptE);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*2.0*Pi,origin.y-pCutLenS[0]),f3dPoint(origin.x+radius*2.0*Pi,origin.y-nMaxOffDist));
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,origin.y-nMaxOffDist),f3dPoint(origin.x+radius*2.0*Pi,origin.y-nMaxOffDist));
			//标注到终端的长度
			f3dPoint base(origin.x+radius*0.5*Pi,origin.y-nMaxOffDist+sys.dim_map.PartNo.fPartNoTextSize);
			DimText(pBlockTableRecord,base,CXhChar100("-%d",tube_len-nMaxOffDist),TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
			//中间线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*Pi,origin.y-pCutLenS[nMidIndex]),f3dPoint(origin.x+radius*Pi,origin.y-nMaxOffDist));
		}
		else //if(ciDrawType==1)
		{
			weld_lien_ptS.Set(origin.x,origin.y);
			weld_line_ptE.Set(origin.x,origin.y-nMaxOffDist+pCutLenE[0]);
			CreateAcadLine(pBlockTableRecord,weld_lien_ptS,weld_line_ptE);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*2.0*Pi,origin.y),f3dPoint(origin.x+radius*2.0*Pi,origin.y-nMaxOffDist+pCutLenE[0]));
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x,origin.y),f3dPoint(origin.x+radius*2.0*Pi,origin.y));
			//标注到始端的长度
			f3dPoint base(origin.x+radius*0.5*Pi,origin.y-2*sys.dim_map.PartNo.fPartNoTextSize);
			DimText(pBlockTableRecord,base,CXhChar100("-%d",tube_len-nMaxOffDist),TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
			//中间线
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,f3dPoint(origin.x+radius*Pi,origin.y),f3dPoint(origin.x+radius*Pi,origin.y-nMaxOffDist+pCutLenE[nMidIndex]));
		}
		//标注钢管基本信息
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(bDimPartInfo)
		{
			f3dPoint base;
			//标注基本信息
			CXhChar100 sNotes, steelmark,guige;
			if(pTube->cMaterial!='S')
				QuerySteelMatMark(pTube->cMaterial,steelmark);
			sNotes.Printf("%s%%%%c%f",(char*)steelmark,pTube->GetDiameter());
			SimplifiedNumString(sNotes);
			guige.Printf("%sX%f",(char*)sNotes,pTube->GetThick());
			SimplifiedNumString(guige);
			double length=pTube->GetLength();
#ifdef AFX_TARG_ENU_ENGLISH
			sNotes.Printf("tower type-%s part number. %s* %d  %s length>=%.1f mm",(char*)Ta.m_sTaType,(char*)GetPartNoIncMat(pTube->GetPartNo(),pTube->cMaterial),nHits,(char*)guige,length);
#else
			sNotes.Printf("塔型-%s 件号. %s* %d  %s 长度>=%.1f mm",(char*)Ta.m_sTaType,(char*)GetPartNoIncMat(pTube->GetPartNo(),pTube->cMaterial),nHits,(char*)guige,length);
#endif
			double fMidY=0.5*(weld_lien_ptS.y+weld_line_ptE.y);
			double fTestH=sys.dim_map.PartNo.fPartNoTextSize;
			double fTestL=TestDrawTextLength(sNotes,fTestH,TextStyleTable::hzfs.textStyleId);
			base.Set(origin.x+radius*0.5*Pi-0.5*fTestL,fMidY+0.5*fTestH);
			DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
			//标记焊缝线
			if(pTube->m_bHasWeldRoad)
			{
				f3dPoint mid_pt=0.5*(weld_lien_ptS+weld_line_ptE);
				CreateAcadLine(pBlockTableRecord,f3dPoint(mid_pt.x+2,mid_pt.y),f3dPoint(mid_pt.x+22,mid_pt.y));
				CreateAcadLine(pBlockTableRecord,f3dPoint(mid_pt.x+2,mid_pt.y),f3dPoint(mid_pt.x+10,mid_pt.y+5));
				base.Set(mid_pt.x+12,mid_pt.y);
				DimText(pBlockTableRecord,base,"焊缝线",TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
			}
			//标注展开图类型
			if(index==0)
				sNotes.Copy("内壁相贯线展开图");
			else if(index==1)
				sNotes.Copy("外壁相贯线展开图");
			else if(index==2)
				sNotes.Copy("内外壁合成切割线展开图");
			else
				sNotes.Copy("粗略加工相贯线展开图");
			fTestL=TestDrawTextLength(sNotes,fTestH,TextStyleTable::hzfs.textStyleId);
			base.Set(origin.x+radius*Pi-0.5*fTestL,origin.y-nMaxOffDist-2*fTestH);
			DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
		}
		//绘制坐标进行水平偏移
		origin.x+=(fLen+20);
	}
	pBlockTableRecord->close();
	//释放内存空间
	delete []pCutLenS;
	delete []pCutLenE;
}
//典型角钢类构件图，主要包括X/Y肢单面两孔角钢，X/Y肢单面三孔角钢，若存在角钢始端端互换可用情况，则归到X肢
void PartDrawingClassicAngle()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	int i,j,nHits=0;
	f2dRect tbl_rect;
	ATOM_LIST<C2HoleJgPart> x2HoleJgList;
	ATOM_LIST<C3HoleJgPart> x3HoleJgList;
 	ATOM_LIST<C5HoleJgPart> x5HoleJgList;
	// When resource from this ARX app is needed, just
    // instantiate a local CAcModuleResourceOverride
    CAcModuleResourceOverride resOverride;
	/*char part_map_file_jg[MAX_PATH];
	part_map_file_jg.Printf("%s_%d_part_jg.dwg",sys.sFilePath,g_nCurMapSegNo);
	NewDwgFile(part_map_file_jg);*/
	CInputAnStringValDlg inputDlg;
	inputDlg.m_sItemTitle="段号范围：";
	inputDlg.m_sTip="*表示输出所有段";
	inputDlg.m_sItemValue="*";
	if(inputDlg.DoModal()!=IDOK)
		return;
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(inputDlg.m_sItemValue,segTable);

	InitDrawingEnv();
	JGSET cutanglejgset;
	typedef CLDSPart* CJgPtr;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(CLDSLineAngle *pCommJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pCommJg;pCommJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(strlen(pCommJg->GetPartNo())==0)
			continue;
		else if(pCommJg->bHuoQuStart||pCommJg->bHuoQuEnd)//火曲角钢中的分段角钢
			continue;
		else if(pCommJg->m_bVirtualPart)
			continue;
		if(segTable.GetNodeNum()>0&&segTable.GetValue(pCommJg->iSeg.iSeg)==NULL)
			continue;
		//else if(pCommJg->iSeg.iSeg==Ta.m_iCurSeg.iSeg)
		{
			CLsRef *pFirstLsRef = pCommJg->GetFirstLsRef();
			CLsRef *pSecondLsRef= pCommJg->GetNextLsRef();
			CLsRef *pThirdLsRef = pCommJg->GetNextLsRef();
			CLsRef *pFourthLsRef= pCommJg->GetNextLsRef();
			CLsRef *pFifthLsRef = pCommJg->GetNextLsRef();
			int lswing1,lswing2,lswing3,lswing4,lswing5;
			double g1,g2,g3,g4,g5;
			f3dPoint perp_pos[5],start_xyz,end_xyz,vec;
			double len_pos[5],length;
			
			start_xyz = pCommJg->Start();
			end_xyz = pCommJg->End();
			vec = end_xyz-start_xyz;
			normalize(vec);
			start_xyz -= vec*pCommJg->startOdd();
			end_xyz   += vec*pCommJg->endOdd();
			length = DISTANCE(start_xyz,end_xyz);
			if(pFirstLsRef)
			{
				if(fabs((*pFirstLsRef)->get_norm()*pCommJg->get_norm_x_wing())>fabs((*pFirstLsRef)->get_norm()*pCommJg->get_norm_y_wing()))
					lswing1=0;
				else
					lswing1=1;
				//pCommJg->IsLsInWing(pFirstLsRef->GetLsPtr(),&lswing1);	//此函数会进行深度检测，而此处不需要进行深度检测
				g1 = pCommJg->GetLsG(pFirstLsRef);
				SnapPerp(&perp_pos[0],start_xyz,end_xyz,(*pFirstLsRef)->ucs.origin);
				f3dPoint len_vec = perp_pos[0]-start_xyz;
				if(len_vec*vec>0)
					len_pos[0] = len_vec.mod();
				else
					len_pos[0] =-len_vec.mod();
			}
			if(pSecondLsRef)
			{
				if(fabs((*pSecondLsRef)->get_norm()*pCommJg->get_norm_x_wing())>fabs((*pSecondLsRef)->get_norm()*pCommJg->get_norm_y_wing()))
					lswing2=0;
				else
					lswing2=1;
				//pCommJg->IsLsInWing(pSecondLsRef->GetLsPtr(),&lswing2);
				g2 = pCommJg->GetLsG(pSecondLsRef);
				SnapPerp(&perp_pos[1],start_xyz,end_xyz,(*pSecondLsRef)->ucs.origin);
				f3dPoint len_vec = perp_pos[1]-start_xyz;
				if(len_vec*vec>0)
					len_pos[1] = len_vec.mod();
				else
					len_pos[1] =-len_vec.mod();
			}
			if(pThirdLsRef)
			{
				pCommJg->IsLsInWing(pThirdLsRef->GetLsPtr(),&lswing3);
				g3 = pCommJg->GetLsG(pThirdLsRef);
				SnapPerp(&perp_pos[2],start_xyz,end_xyz,(*pThirdLsRef)->ucs.origin);
				f3dPoint len_vec = perp_pos[2]-start_xyz;
				if(len_vec*vec>0)
					len_pos[2] = len_vec.mod();
				else
					len_pos[2] =-len_vec.mod();
			}
			if(pFourthLsRef)
			{
				pCommJg->IsLsInWing(pFourthLsRef->GetLsPtr(),&lswing4);
				g4 = pCommJg->GetLsG(pFourthLsRef);
				SnapPerp(&perp_pos[3],start_xyz,end_xyz,(*pFourthLsRef)->ucs.origin);
				f3dPoint len_vec = perp_pos[3]-start_xyz;
				if(len_vec*vec>0)
					len_pos[3] = len_vec.mod();
				else
					len_pos[3] =-len_vec.mod();
			}
			if(pFifthLsRef)
			{
				pCommJg->IsLsInWing(pFifthLsRef->GetLsPtr(),&lswing5);
				g5 = pCommJg->GetLsG(pFifthLsRef);
				SnapPerp(&perp_pos[4],start_xyz,end_xyz,(*pFifthLsRef)->ucs.origin);
				f3dPoint len_vec = perp_pos[4]-start_xyz;
				if(len_vec*vec>0)
					len_pos[4] = len_vec.mod();
				else
					len_pos[4] =-len_vec.mod();
			}
			//统计切角信息 wht 18-11-05
			CXhChar50 xWingCut,yWingCut,startCutInfo,endCutInfo;
			CCutAngleBriefMap::GetAngleCutAngleInfo(pCommJg,xWingCut,yWingCut,true,sys.part_map.angle.iCutAngleDimType);
			if(xWingCut.GetLength()>0&&yWingCut.GetLength()>0)
			{
				if(strstr(yWingCut,"CO")>0)
					startCutInfo.Printf("%s,%s",(char*)yWingCut,(char*)xWingCut);
				else
					startCutInfo.Printf("%s,%s",(char*)xWingCut,(char*)yWingCut);
			}
			else if(xWingCut.GetLength()>0)
				startCutInfo=xWingCut;
			else if(yWingCut.GetLength()>0)
				startCutInfo=yWingCut;
			CCutAngleBriefMap::GetAngleCutAngleInfo(pCommJg,xWingCut,yWingCut,false,sys.part_map.angle.iCutAngleDimType);
			if(xWingCut.GetLength()>0&&yWingCut.GetLength()>0)
			{
				if(strstr(yWingCut,"CO")>0)
					endCutInfo.Printf("%s,%s",(char*)yWingCut,(char*)xWingCut);
				else
					endCutInfo.Printf("%s,%s",(char*)xWingCut,(char*)yWingCut);
			}
			else if(xWingCut.GetLength()>0)
				endCutInfo=xWingCut;
			else if(yWingCut.GetLength()>0)
				endCutInfo=yWingCut;
			CXhChar50 sStartPushInfo,sEndPushInfo,sMiddlePushInfo;
			if(pCommJg->start_push_flat_length>0&&(pCommJg->start_push_flat_x1_y2==1||pCommJg->start_push_flat_x1_y2==2))
				sStartPushInfo.Printf("压扁%dmm",(int)pCommJg->start_push_flat_length);
			if(pCommJg->end_push_flat_length>0&&(pCommJg->end_push_flat_x1_y2==1||pCommJg->end_push_flat_x1_y2==2))
				sEndPushInfo.Printf("压扁%dmm",(int)pCommJg->end_push_flat_length);
			if(pCommJg->mid_push_flat_length>0&&(pCommJg->mid_push_flat_x1_y2==1||pCommJg->mid_push_flat_x1_y2==2))
				sEndPushInfo.Printf("中间压扁%dmm",(int)pCommJg->mid_push_flat_length);
			//
			CXhChar50 sNotes;
			JGWING eBaseWing=X_WING;
			pCommJg->GetProcessStr(sNotes,50);
			sNotes.Replace("切角","");
			sNotes.Replace("切肢","");
			sNotes.Replace("压扁","");
			if(lswing1==0)	//位于X肢
			{
				eBaseWing = X_WING;
				if(startCutInfo.GetLength()>0)
				{
					sNotes.Append("左:");
					sNotes.Append(startCutInfo);
				}
				if(endCutInfo.GetLength()>0)
				{
					if(sNotes.GetLength()>0)
						sNotes.Append("\n");
					sNotes.Append("右:");
					sNotes.Append(endCutInfo);
				}
				if(sStartPushInfo.GetLength()>0||sEndPushInfo.GetLength()>0||sMiddlePushInfo.GetLength()>0)
				{
					if(sNotes.GetLength()>0)
						sNotes.Append("\n");
					CXhChar200 sPushInfo;
					if(sStartPushInfo.GetLength()>0)
					{
						sPushInfo.Append("左侧");
						sPushInfo.Append(sStartPushInfo);
					}
					if(sMiddlePushInfo.GetLength()>0)
					{
						if(sPushInfo.GetLength()>0)
							sPushInfo.Append(",");
						sPushInfo.Append(sMiddlePushInfo);
					}
					if(sEndPushInfo.GetLength()>0)
					{
						if(sPushInfo.GetLength()>0)
							sPushInfo.Append(",");
						sPushInfo.Append("右侧");
						sPushInfo.Append(sEndPushInfo);
					}
					sNotes.Append(sPushInfo);
				}
			}
			else				//位于Y肢
			{
				eBaseWing = Y_WING;
				if(endCutInfo.GetLength()>0)
				{
					sNotes.Append("左:");
					sNotes.Append(endCutInfo);
				}
				if(startCutInfo.GetLength()>0)
				{
					if(sNotes.GetLength()>0)
						sNotes.Append("\n");
					sNotes.Append("右:");
					sNotes.Append(startCutInfo);
				}
				if(sStartPushInfo.GetLength()>0||sEndPushInfo.GetLength()>0||sMiddlePushInfo.GetLength()>0)
				{
					if(sNotes.GetLength()>0)
						sNotes.Append("\n");
					CXhChar200 sPushInfo;
					if(sEndPushInfo.GetLength()>0)
					{
						sPushInfo.Append("左侧");
						sPushInfo.Append(sEndPushInfo);
					}
					if(sMiddlePushInfo.GetLength()>0)
					{
						if(sPushInfo.GetLength()>0)
							sPushInfo.Append(",");
						sPushInfo.Append(sMiddlePushInfo);
					}
					if(sStartPushInfo.GetLength()>0)
					{
						if(sPushInfo.GetLength()>0)
							sPushInfo.Append(",");
						sPushInfo.Append("右侧");
						sPushInfo.Append(sStartPushInfo);
					}
					sNotes.Append(sPushInfo);
				}
			}
			if(pCommJg->GetLsCount()==2)
			{	//单面两孔同心典型角钢件
				if(lswing1!=lswing2||ftoi(g1)!=ftoi(g2))
					continue;
				C2HoleJgPart *pPart = NULL;
				if(x2HoleJgList.GetNodeNum()==0)
				{
					if(pCommJg->IsCutAngleOrWing())
						cutanglejgset.append(pCommJg);
					pPart = x2HoleJgList.append();
					pPart->num = pCommJg->m_uStatMatCoef;
					pPart->cMaterial = pCommJg->cMaterial;
					strcpy(pPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial));
					pPart->m_nG = ftoi(g1);
					if(len_pos[0]<len_pos[1])
					{
						pPart->m_nStartSpace = ftoi(len_pos[0]);
						pPart->m_nLength= ftoi(pCommJg->GetLength());
						pPart->m_nEndSpace   = ftoi(length-len_pos[1]);
						pPart->m_fD1 = (*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
						pPart->m_fD2 = (*pSecondLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
					}
					else
					{
						pPart->m_nStartSpace = ftoi(len_pos[1]);
						pPart->m_nLength= ftoi(pCommJg->GetLength());
						pPart->m_nEndSpace   = ftoi(length-len_pos[0]);
						pPart->m_fD1 = (*pSecondLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
						pPart->m_fD2 = (*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
					}
					pPart->spec.Printf("L%.0fX%.0f",pCommJg->GetWidth(),pCommJg->GetThick());
					//特殊工艺处理
					pPart->sNotes=sNotes;
					pPart->m_eBaseWing = eBaseWing;
				}
				else 
				{
					for(C2HoleJgPart *pLegacyPart=x2HoleJgList.GetFirst();pLegacyPart;pLegacyPart=x2HoleJgList.GetNext())
					{
						if(stricmp(pLegacyPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial))==0)
						{
							pPart = pLegacyPart;
							pPart->num += pCommJg->m_uStatMatCoef;
							break;
						}
					}
					if(pPart==NULL)
					{
						if(pCommJg->IsCutAngleOrWing())
							cutanglejgset.append(pCommJg);
						pPart = x2HoleJgList.append();
						pPart->num = pCommJg->m_uStatMatCoef;
						pPart->cMaterial = pCommJg->cMaterial;
						strcpy(pPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial));
						pPart->m_nG = ftoi(g1);
						if(len_pos[0]<len_pos[1])
						{
							pPart->m_nStartSpace = ftoi(len_pos[0]);
							pPart->m_nLength= ftoi(pCommJg->GetLength());
							pPart->m_nEndSpace   = ftoi(length-len_pos[1]);
							pPart->m_fD1 = (*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
							pPart->m_fD2 = (*pSecondLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
						}
						else
						{
							pPart->m_nStartSpace = ftoi(len_pos[1]);
							pPart->m_nLength= ftoi(pCommJg->GetLength());
							pPart->m_nEndSpace   = ftoi(length-len_pos[0]);
							pPart->m_fD1 = (*pSecondLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
							pPart->m_fD2 = (*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
						}
						pPart->spec.Printf("L%.0fX%.0f",pCommJg->GetWidth(),pCommJg->GetThick());
						pPart->m_eBaseWing = eBaseWing;
						pPart->sNotes=sNotes;
					}
				}
			}
			else if(pCommJg->GetLsCount()==3)
			{	//单面三孔同心典型角钢件
				if(lswing1!=lswing2||lswing1!=lswing3||ftoi(g1)!=ftoi(g2)||ftoi(g1)!=ftoi(g3))
					continue;
				C3HoleJgPart *pPart = NULL;
				if(x3HoleJgList.GetNodeNum()==0)
				{
					if(pCommJg->IsCutAngleOrWing())
						cutanglejgset.append(pCommJg);
					pPart = x3HoleJgList.append();
					pPart->num = pCommJg->m_uStatMatCoef;
					pPart->cMaterial = pCommJg->cMaterial;
					strcpy(pPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial));
					pPart->m_nG = ftoi(g1);
					double len1=len_pos[0],len2=len_pos[1],len3=len_pos[2];
					double d1=(*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
					double d2=(*pSecondLsRef)->get_d()+(*pSecondLsRef)->hole_d_increment;
					double d3=(*pThirdLsRef)->get_d()+(*pThirdLsRef)->hole_d_increment;
					if(len1<=len2&&len2<=len3)	//1,2,3排序
					{
						pPart->m_fD1=d1;
						pPart->m_fD2=d2;
						pPart->m_fD3=d3;
					}
					else if(len1<=len3&&len3<=len2)	//1,3,2排序
					{
						len_pos[1]=len3;
						len_pos[2]=len2;
						pPart->m_fD1=d1;
						pPart->m_fD2=d3;
						pPart->m_fD3=d2;
					}
					else if(len2<=len1&&len1<=len3)	//2,1,3排序
					{
						len_pos[0]=len2;
						len_pos[1]=len1;
						len_pos[2]=len3;
						pPart->m_fD1=d2;
						pPart->m_fD2=d1;
						pPart->m_fD3=d3;
					}
					else if(len2<=len3&&len3<=len1)	//2,3,1排序
					{
						len_pos[0]=len2;
						len_pos[1]=len3;
						len_pos[2]=len1;
						pPart->m_fD1=d2;
						pPart->m_fD2=d3;
						pPart->m_fD3=d1;
					}
					else if(len3<=len1&&len1<=len2)	//3,1,2排序
					{
						len_pos[0]=len3;
						len_pos[1]=len1;
						len_pos[2]=len2;
						pPart->m_fD1=d3;
						pPart->m_fD2=d1;
						pPart->m_fD3=d2;
					}
					else//if(len3<=len2&&len2<=len1)//3,2,1排序
					{
						len_pos[0]=len3;
						len_pos[1]=len2;
						len_pos[2]=len1;
						pPart->m_fD1=d3;
						pPart->m_fD2=d2;
						pPart->m_fD3=d1;
					}
					//对孔位进行圆整
					CDistFillet fillet;
					fillet.coordArr.SetSize(4);
					for(int i=0;i<3;i++)
						fillet.coordArr[i]=len_pos[i];
					fillet.coordArr[3]=length;
					fillet.Fillet(120,FILLET_INTEGER);
					for(int i=0;i<3;i++)
						len_pos[i]=fillet.coordArr[i];
					length=fillet.coordArr[3];
					//
					pPart->m_nStartSpace		= ftoi(len_pos[0]);
					pPart->m_nMiddleLsLeftSpace	= ftoi(len_pos[1]);
					pPart->m_nMiddleLsRightSpace	= ftoi(length-len_pos[1]);
					pPart->m_nEndSpace			= ftoi(length-len_pos[2]);
					pPart->spec.Printf("L%.0fX%.0f",pCommJg->GetWidth(),pCommJg->GetThick());
					pPart->m_eBaseWing = eBaseWing;
					pPart->sNotes=sNotes;
				}
				else 
				{
					for(C3HoleJgPart *pLegacyPart=x3HoleJgList.GetFirst();pLegacyPart;pLegacyPart=x3HoleJgList.GetNext())
					{
						if(stricmp(pLegacyPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial))==0)
						{
							pPart = pLegacyPart;
							pPart->num += pCommJg->m_uStatMatCoef;
							break;
						}
					}
					if(pPart==NULL)
					{
						if(pCommJg->IsCutAngleOrWing())
							cutanglejgset.append(pCommJg);
						pPart = x3HoleJgList.append();
						pPart->num = pCommJg->m_uStatMatCoef;
						pPart->cMaterial = pCommJg->cMaterial;
						strcpy(pPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial));
						pPart->m_nG = ftoi(g1);
						double len1=len_pos[0],len2=len_pos[1],len3=len_pos[2];
						double d1=(*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
						double d2=(*pSecondLsRef)->get_d()+(*pSecondLsRef)->hole_d_increment;
						double d3=(*pThirdLsRef)->get_d()+(*pThirdLsRef)->hole_d_increment;
						if(len1<=len2&&len2<=len3)	//1,2,3排序
						{
							pPart->m_fD1=d1;
							pPart->m_fD2=d2;
							pPart->m_fD3=d3;
						}
						else if(len1<=len3&&len3<=len2)	//1,3,2排序
						{
							len_pos[1]=len3;
							len_pos[2]=len2;
							pPart->m_fD1=d1;
							pPart->m_fD2=d3;
							pPart->m_fD3=d2;
						}
						else if(len2<=len1&&len1<=len3)	//2,1,3排序
						{
							len_pos[0]=len2;
							len_pos[1]=len1;
							len_pos[2]=len3;
							pPart->m_fD1=d2;
							pPart->m_fD2=d1;
							pPart->m_fD3=d3;
						}
						else if(len2<=len3&&len3<=len1)	//2,3,1排序
						{
							len_pos[0]=len2;
							len_pos[1]=len3;
							len_pos[2]=len1;
							pPart->m_fD1=d2;
							pPart->m_fD2=d3;
							pPart->m_fD3=d1;
						}
						else if(len3<=len1&&len1<=len2)	//3,1,2排序
						{
							len_pos[0]=len3;
							len_pos[1]=len1;
							len_pos[2]=len2;
							pPart->m_fD1=d3;
							pPart->m_fD2=d1;
							pPart->m_fD3=d2;
						}
						else//if(len3<=len2&&len2<=len1)//3,2,1排序
						{
							len_pos[0]=len3;
							len_pos[1]=len2;
							len_pos[2]=len1;
							pPart->m_fD1=d3;
							pPart->m_fD2=d2;
							pPart->m_fD3=d1;
						}
						//对孔位进行圆整
						CDistFillet fillet;
						fillet.coordArr.SetSize(4);
						for(int i=0;i<3;i++)
							fillet.coordArr[i]=len_pos[i];
						fillet.coordArr[3]=length;
						fillet.Fillet(120,FILLET_INTEGER);
						for(int i=0;i<3;i++)
							len_pos[i]=fillet.coordArr[i];
						length=fillet.coordArr[3];
						//
						pPart->m_nStartSpace		= ftoi(len_pos[0]);
						pPart->m_nMiddleLsLeftSpace	= ftoi(len_pos[1]);
						pPart->m_nMiddleLsRightSpace= ftoi(length-len_pos[1]);
						pPart->m_nEndSpace			= ftoi(length-len_pos[2]);
						pPart->spec.Printf("L%.0fX%.0f",pCommJg->GetWidth(),pCommJg->GetThick());
						pPart->m_eBaseWing = eBaseWing;
						pPart->sNotes=sNotes;
					}
				}
			}
			else if(pCommJg->GetLsCount()==5)
			{	//单面五孔同心典型角钢件
				if(!(lswing1==lswing2&&lswing1==lswing3&&lswing1==lswing4&&lswing1==lswing5&&
					ftoi(g1)==ftoi(g2)&&ftoi(g1)==ftoi(g3)&&ftoi(g1)==ftoi(g4)&&ftoi(g1)==ftoi(g5)))
					continue;
				C5HoleJgPart *pPart = NULL;
				if(x5HoleJgList.GetNodeNum()==0)
				{
					double d[5];
					d[0]=(*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
					d[1]=(*pSecondLsRef)->get_d()+(*pSecondLsRef)->hole_d_increment;
					d[2]=(*pThirdLsRef)->get_d()+(*pThirdLsRef)->hole_d_increment;
					d[3]=(*pFourthLsRef)->get_d()+(*pFourthLsRef)->hole_d_increment;
					d[4]=(*pFifthLsRef)->get_d()+(*pFifthLsRef)->hole_d_increment;
					for(i=0;i<5;i++)   //冒泡排序
					{
						BOOL bSorted = TRUE;
						for(j=0;j<4-i;j++)
						{
							if(len_pos[j]>len_pos[j+1])
							{
								double tm_len=len_pos[j];
								len_pos[j]=len_pos[j+1];
								len_pos[j+1] = tm_len;
								double tm_d=d[j];
								d[j]=d[j+1];
								d[j+1]=tm_d;
								bSorted = FALSE;
							}
						}
						if(bSorted)
							break;
					}
					if(ftoi(len_pos[1]-len_pos[0])>999||ftoi(len_pos[4]-len_pos[3])>999)
						continue;	//螺栓间距太大显示不下，而且也不太符合人常规习惯，暂时过滤掉这种情况
					if(pCommJg->IsCutAngleOrWing())
						cutanglejgset.append(pCommJg);
					pPart = x5HoleJgList.append();
					pPart->num = pCommJg->m_uStatMatCoef;
					pPart->cMaterial = pCommJg->cMaterial;
					//对孔位进行圆整
					CDistFillet fillet;
					fillet.coordArr.SetSize(6);
					for(int i=0;i<5;i++)
						fillet.coordArr[i]=len_pos[i];
					fillet.coordArr[5]=length;
					fillet.Fillet(120,FILLET_INTEGER);
					for(int i=0;i<5;i++)
						len_pos[i]=fillet.coordArr[i];
					length=fillet.coordArr[5];
					strcpy(pPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial));
					pPart->m_nG = ftoi(g1);
					pPart->m_fD1=d[0];
					pPart->m_fD2=d[1];
					pPart->m_fD3=d[2];
					pPart->m_fD4=d[3];
					pPart->m_fD5=d[4];
					pPart->m_nStartSpace		= ftoi(len_pos[0]);
					pPart->m_nLeftLsSpace		= ftoi(len_pos[1]-len_pos[0]);
					pPart->m_nMiddleLsLeftSpace	= ftoi(len_pos[2]);
					pPart->m_nMiddleLsRightSpace= ftoi(length-len_pos[2]);
					pPart->m_nRightLsSpace		= ftoi(len_pos[4]-len_pos[3]);
					pPart->m_nEndSpace			= ftoi(length-len_pos[4]);
					pPart->spec.Printf("L%.0fX%.0f",pCommJg->GetWidth(),pCommJg->GetThick());
					pPart->m_eBaseWing = eBaseWing;
					pPart->sNotes=sNotes;
				}
				else 
				{
					for(C5HoleJgPart *pLegacyPart=x5HoleJgList.GetFirst();pLegacyPart;pLegacyPart=x5HoleJgList.GetNext())
					{
						if(stricmp(pLegacyPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial))==0)
						{
							pPart = pLegacyPart;
							pPart->num += pCommJg->m_uStatMatCoef;
							break;
						}
					}
					if(pPart==NULL)
					{
						double d[5];
						d[0]=(*pFirstLsRef)->get_d()+(*pFirstLsRef)->hole_d_increment;
						d[1]=(*pSecondLsRef)->get_d()+(*pSecondLsRef)->hole_d_increment;
						d[2]=(*pThirdLsRef)->get_d()+(*pThirdLsRef)->hole_d_increment;
						d[3]=(*pFourthLsRef)->get_d()+(*pFourthLsRef)->hole_d_increment;
						d[4]=(*pFifthLsRef)->get_d()+(*pFifthLsRef)->hole_d_increment;
						for(i=0;i<5;i++)   //冒泡排序
						{
							BOOL bSorted = TRUE;
							for(j=0;j<4-i;j++)
							{
								if(len_pos[j]>len_pos[j+1])
								{
									double tm_len=len_pos[j];
									len_pos[j]=len_pos[j+1];
									len_pos[j+1] = tm_len;
									double tm_d=d[j];
									d[j]=d[j+1];
									d[j+1]=tm_d;
									bSorted = FALSE;
								}
							}
							if(bSorted)
								break;
						}
						if(ftoi(len_pos[1]-len_pos[0])>999||ftoi(len_pos[4]-len_pos[3])>999)
							continue;	//螺栓间距太大显示不下，而且也不太符合人常规习惯，暂时过滤掉这种情况
						if(pCommJg->IsCutAngleOrWing())
							cutanglejgset.append(pCommJg);
						pPart = x5HoleJgList.append();
						pPart->num = pCommJg->m_uStatMatCoef;
						pPart->cMaterial = pCommJg->cMaterial;
						//对孔位进行圆整
						CDistFillet fillet;
						fillet.coordArr.SetSize(6);
						for(int i=0;i<5;i++)
							fillet.coordArr[i]=len_pos[i];
						fillet.coordArr[5]=length;
						fillet.Fillet(120,FILLET_INTEGER);
						for(int i=0;i<5;i++)
							len_pos[i]=fillet.coordArr[i];
						length=fillet.coordArr[5];
						strcpy(pPart->sPartNo,GetPartNoIncMat(pCommJg->GetPartNo(),pCommJg->cMaterial));
						pPart->m_nG = ftoi(g1);
						pPart->m_fD1=d[0];
						pPart->m_fD2=d[1];
						pPart->m_fD3=d[2];
						pPart->m_fD4=d[3];
						pPart->m_fD5=d[4];
						pPart->m_nStartSpace		= ftoi(len_pos[0]);
						pPart->m_nLeftLsSpace		= ftoi(len_pos[1]-len_pos[0]);
						pPart->m_nMiddleLsLeftSpace	= ftoi(len_pos[2]);
						pPart->m_nMiddleLsRightSpace= ftoi(length-len_pos[2]);
						pPart->m_nRightLsSpace		= ftoi(len_pos[4]-len_pos[3]);
						pPart->m_nEndSpace			= ftoi(length-len_pos[4]);
						pPart->spec.Printf("L%.0fX%.0f",pCommJg->GetWidth(),pCommJg->GetThick());
						pPart->m_eBaseWing = eBaseWing;
						pPart->sNotes=sNotes;
					}
				}
			}
		}
	}

	for(i=0;i<x2HoleJgList.GetNodeNum()-1;i++)   //冒泡排序
	{
		BOOL bSorted = TRUE;
		for(j=0;j<x2HoleJgList.GetNodeNum()-i-1;j++)
		{
			//if(stricmp(x2HoleJgList[j].sPartNo,x2HoleJgList[j+1].sPartNo)>0)
			if(ComparePartNoString(x2HoleJgList[j].sPartNo,x2HoleJgList[j+1].sPartNo)>0)
			{
				C2HoleJgPart tm_part=x2HoleJgList[j];
				x2HoleJgList[j] = x2HoleJgList[j+1];
				x2HoleJgList[j+1]=tm_part;
				bSorted = FALSE;
			}
		}
		if(bSorted)
			break;
	}
	for(i=0;i<x3HoleJgList.GetNodeNum()-1;i++)   //冒泡排序
	{
		BOOL bSorted = TRUE;
		for(j=0;j<x3HoleJgList.GetNodeNum()-i-1;j++)
		{
			//if(stricmp(x3HoleJgList[j].sPartNo,x3HoleJgList[j+1].sPartNo)>0)
			if(ComparePartNoString(x3HoleJgList[j].sPartNo,x3HoleJgList[j+1].sPartNo)>0)
			{
				C3HoleJgPart tm_part=x3HoleJgList[j];
				x3HoleJgList[j] = x3HoleJgList[j+1];
				x3HoleJgList[j+1]=tm_part;
				bSorted = FALSE;
			}
		}
		if(bSorted)
			break;
	}
	for(i=0;i<x5HoleJgList.GetNodeNum()-1;i++)   //冒泡排序
	{
		BOOL bSorted = TRUE;
		for(j=0;j<x5HoleJgList.GetNodeNum()-i-1;j++)
		{
			//if(stricmp(x5HoleJgList[j].sPartNo,x5HoleJgList[j+1].sPartNo)>0)
			if(ComparePartNoString(x5HoleJgList[j].sPartNo,x5HoleJgList[j+1].sPartNo)>0)
			{
				C5HoleJgPart tm_part=x5HoleJgList[j];
				x5HoleJgList[j] = x5HoleJgList[j+1];
				x5HoleJgList[j+1]=tm_part;
				bSorted = FALSE;
			}
		}
		if(bSorted)
			break;
	}
	CFormatTbl xClassic2HoleTbl,xClassic3HoleTbl,xClassic5HoleTbl,xHeaderTbl,xCutAngleTbl,frameTbl;
	frameTbl.Create(1,1);
	frameTbl.SetRowHigh(0,300);
	frameTbl.SetColumnWide(0,210);
	xClassic2HoleTbl.Create(33,14);
	xClassic3HoleTbl.Create(33,14);
	xClassic5HoleTbl.Create(33,18);
	xHeaderTbl.Create(3,7);
	if(cutanglejgset.GetNodeNum()>0)
		xCutAngleTbl.Create(cutanglejgset.GetNodeNum()*2+1,6);
	xHeaderTbl.SetRowHigh(0,7);
	xHeaderTbl.SetRowHigh(1,7);
	xHeaderTbl.SetRowHigh(2,7);
	for(i=0;i<xClassic2HoleTbl.GetRowsCount();i++)
	{
		if(i==0)
			xClassic2HoleTbl.SetRowHigh(i,40);
		else
			xClassic2HoleTbl.SetRowHigh(i,7);
		for(j=0;j<xClassic2HoleTbl.GetColumnsCount();j++)	//首行为标题行
		{
			if(i==0&&j>0)
				xClassic2HoleTbl.GetGridAt(i,j)->m_bHGridVirtual=TRUE;
			else if(j==8||j==9)
				xClassic2HoleTbl.GetGridAt(i,j)->m_bHGridVirtual=TRUE;
			if(i==1)
			{
				xClassic2HoleTbl.GetGridAt(i,j)->textsize = sys.dim_map.fCommonTextSize;
				xClassic2HoleTbl.GetGridAt(i,j)->feature=TEXTALIGN_MIDDLE_CENTER;
			}
			else
				xClassic2HoleTbl.GetGridAt(i,j)->textsize = sys.dim_map.fNumericTextSize;
			if(i>1&&j==xClassic2HoleTbl.GetColumnsCount()-1)	//特殊工艺列字体设置为 2 wht 18-11-05
			{
				xClassic2HoleTbl.GetGridAt(i,j)->textsize = 2;
				xClassic2HoleTbl.GetGridAt(i,j)->feature=TEXTALIGN_LEFT_CENTER;
			}
		}
	}
	for(i=0;i<xClassic3HoleTbl.GetRowsCount();i++)
	{
		if(i==0)
			xClassic3HoleTbl.SetRowHigh(i,40);
		else
			xClassic3HoleTbl.SetRowHigh(i,7);
		for(j=0;j<xClassic3HoleTbl.GetColumnsCount();j++)	//首行为标题行
		{
			if(i==0&&j>0)
				xClassic3HoleTbl.GetGridAt(i,j)->m_bHGridVirtual=TRUE;
			if(i==1)
			{
				xClassic3HoleTbl.GetGridAt(i,j)->textsize = sys.dim_map.fCommonTextSize;
				xClassic3HoleTbl.GetGridAt(i,j)->feature=TEXTALIGN_MIDDLE_CENTER;
			}
			else
				xClassic3HoleTbl.GetGridAt(i,j)->textsize = sys.dim_map.fNumericTextSize;
			if(i>1&&j==xClassic3HoleTbl.GetColumnsCount()-1)	//特殊工艺列字体设置为 2 wht 18-11-05
			{
				xClassic3HoleTbl.GetGridAt(i,j)->textsize = 2;
				xClassic3HoleTbl.GetGridAt(i,j)->feature=TEXTALIGN_LEFT_CENTER;
			}
		}
	}
	for(i=0;i<xClassic5HoleTbl.GetRowsCount();i++)
	{
		if(i==0)
			xClassic5HoleTbl.SetRowHigh(i,40);
		else
			xClassic5HoleTbl.SetRowHigh(i,7);
		for(j=0;j<xClassic5HoleTbl.GetColumnsCount();j++)	//首行为标题行
		{
			if(i==0&&j>0)
				xClassic5HoleTbl.GetGridAt(i,j)->m_bHGridVirtual=TRUE;
			if(i==1)
			{
				xClassic5HoleTbl.GetGridAt(i,j)->textsize = sys.dim_map.fCommonTextSize;
				xClassic5HoleTbl.GetGridAt(i,j)->feature=TEXTALIGN_MIDDLE_CENTER;
			}
			else
				xClassic5HoleTbl.GetGridAt(i,j)->textsize = sys.dim_map.fNumericTextSize;
			if(i>1&&j==xClassic5HoleTbl.GetColumnsCount()-1)	//特殊工艺列字体设置为 2 wht 18-11-05
			{
				xClassic5HoleTbl.GetGridAt(i,j)->textsize = 2;
				xClassic5HoleTbl.GetGridAt(i,j)->feature=TEXTALIGN_LEFT_CENTER;
			}
		}
	}
	//标题栏
	xHeaderTbl.SetColumnWide(0,13);
	xHeaderTbl.SetColumnWide(1,19);
	xHeaderTbl.SetColumnWide(2,19);
	xHeaderTbl.SetColumnWide(3,58);
	xHeaderTbl.SetColumnWide(4,22);
	xHeaderTbl.SetColumnWide(5,26);
	xHeaderTbl.SetColumnWide(6,22);
#ifdef AFX_TARG_ENU_ENGLISH
	xHeaderTbl.SetGrid(0,0,"check");
	xHeaderTbl.SetGrid(0,1,Ta.m_sAuditor);
	xHeaderTbl.SetGrid(0,2,"Project Name");
	xHeaderTbl.SetGrid(0,3,Ta.m_sPrjName);
	xHeaderTbl.SetGrid(0,4,"Tower Type");
	xHeaderTbl.SetGrid(0,5,Ta.m_sTaType);
	xHeaderTbl.SetGrid(0,6,"picture number");
	xHeaderTbl.SetGrid(0,7,Ta.m_sTaStampNo);
	xHeaderTbl.SetGrid(0,8,"No.   page");
	xHeaderTbl.SetGrid(1,0,"review");
	xHeaderTbl.SetGrid(1,1,Ta.m_sCritic);
	xHeaderTbl.SetGrid(2,0,"draw picture");
	xHeaderTbl.SetGrid(2,1,Ta.m_sOperator);
	xHeaderTbl.SetGrid(1,2,"date");
#else
	xHeaderTbl.SetGrid(0,0,"审核");
	xHeaderTbl.SetGrid(0,1,Ta.m_sAuditor);
	xHeaderTbl.SetGrid(0,2,"塔型");
	xHeaderTbl.SetGrid(0,3,Ta.m_sTaType);
	xHeaderTbl.SetGrid(0,4,"产品代号");
	xHeaderTbl.SetGrid(0,5,Ta.m_sTaAlias);
	xHeaderTbl.SetGrid(0,6,"第    页");
	xHeaderTbl.SetGrid(1,0,"复核");
	xHeaderTbl.SetGrid(1,1,Ta.m_sCritic);
	xHeaderTbl.SetGrid(2,0,"制图");
	xHeaderTbl.SetGrid(2,1,Ta.m_sOperator);
	xHeaderTbl.SetGrid(1,2,"日  期");
#endif
	xHeaderTbl.SetGrid(1,3,sys.m_sCompanyName);
	for(i=0;i<xHeaderTbl.GetRowsCount();i++)
	for(j=0;j<xHeaderTbl.GetColumnsCount();j++)
	{
		xHeaderTbl.GetGridAt(i,j)->textsize = sys.dim_map.fCommonTextSize;
		xHeaderTbl.GetGridAt(i,j)->feature = TEXTALIGN_MIDDLE_CENTER;
	}
	xHeaderTbl.GetGridAt(1,3)->textsize = sys.dim_map.fHeaderTextSize;
	for(j=4;j<xHeaderTbl.GetColumnsCount();j++)
	{
		xHeaderTbl.GetGridAt(1,j)->m_bHGridVirtual=TRUE;
		xHeaderTbl.GetGridAt(2,j)->m_bHGridVirtual=TRUE;
		xHeaderTbl.GetGridAt(2,j)->m_bVGridVirtual=TRUE;
		if(j==4)
			xHeaderTbl.GetGridAt(2,3)->m_bVGridVirtual=TRUE;
	}

	xClassic2HoleTbl.SetColumnWide(0,18);	//部件编号
	xClassic3HoleTbl.SetColumnWide(0,18);	//部件编号
	xClassic5HoleTbl.SetColumnWide(0,11.5);	//部件编号
#ifdef AFX_TARG_ENU_ENGLISH
	xClassic2HoleTbl.SetGrid(1,0,"Block No.");
	xClassic3HoleTbl.SetGrid(1,0,"Block No.");
	xClassic5HoleTbl.SetGrid(1,0,"No.");
#else
	xClassic2HoleTbl.SetGrid(1,0,"部件编号");
	xClassic3HoleTbl.SetGrid(1,0,"部件编号");
	xClassic5HoleTbl.SetGrid(1,0,"编号");
#endif
	xClassic2HoleTbl.SetColumnWide(1,21);	//规格
	xClassic3HoleTbl.SetColumnWide(1,21);	//规格
	xClassic5HoleTbl.SetColumnWide(1,17);	//规格
#ifdef AFX_TARG_ENU_ENGLISH
	xClassic2HoleTbl.SetGrid(1,1,"Spec");
	xClassic3HoleTbl.SetGrid(1,1,"Spec");
	xClassic5HoleTbl.SetGrid(1,1,"Spec");
#else
	xClassic2HoleTbl.SetGrid(1,1,"规格");
	xClassic3HoleTbl.SetGrid(1,1,"规格");
	xClassic5HoleTbl.SetGrid(1,1,"规格");
#endif
	xClassic2HoleTbl.SetColumnWide(2,11);	//材质
	xClassic3HoleTbl.SetColumnWide(2,11);	//材质
	xClassic5HoleTbl.SetColumnWide(2,11);	//材质
#ifdef AFX_TARG_ENU_ENGLISH
	xClassic2HoleTbl.SetGrid(1,2,"Material");
	xClassic3HoleTbl.SetGrid(1,2,"Material");
	xClassic5HoleTbl.SetGrid(1,2,"Material");
#else
	xClassic2HoleTbl.SetGrid(1,2,"材质");
	xClassic3HoleTbl.SetGrid(1,2,"材质");
	xClassic5HoleTbl.SetGrid(1,2,"材质");
#endif
	xClassic2HoleTbl.SetColumnWide(3,8.5);	//数量
	xClassic3HoleTbl.SetColumnWide(3,8.5);	//数量
	xClassic5HoleTbl.SetColumnWide(3,8.5);	//数量
#ifdef AFX_TARG_ENU_ENGLISH
	xClassic2HoleTbl.SetGrid(1,3,"quantity");
	xClassic3HoleTbl.SetGrid(1,3,"quantity");
	xClassic5HoleTbl.SetGrid(1,3,"quantity");
#else
	xClassic2HoleTbl.SetGrid(1,3,"数量");
	xClassic3HoleTbl.SetGrid(1,3,"数量");
	xClassic5HoleTbl.SetGrid(1,3,"数量");
#endif
	xClassic2HoleTbl.SetColumnWide(4,8.5);	//B(心距)
	xClassic3HoleTbl.SetColumnWide(4,8.5);	//B(心距)
	xClassic5HoleTbl.SetColumnWide(4,6);	//B(心距)
	xClassic2HoleTbl.SetGrid(1,4,"B");
	xClassic3HoleTbl.SetGrid(1,4,"B");
	xClassic5HoleTbl.SetGrid(1,4,"B");
	xClassic2HoleTbl.SetColumnWide(5,8.5);	//始端距
	xClassic3HoleTbl.SetColumnWide(5,8.5);	//始端距
	xClassic5HoleTbl.SetColumnWide(5,6);	//始端距
	xClassic2HoleTbl.SetGrid(1,5,"M");
	xClassic3HoleTbl.SetGrid(1,5,"M");
	xClassic5HoleTbl.SetGrid(1,5,"M");
	xClassic2HoleTbl.SetColumnWide(6,8.5);	//终端距
	xClassic3HoleTbl.SetColumnWide(6,8.5);	//终端距
	xClassic5HoleTbl.SetColumnWide(6,6);	//终端距
	xClassic2HoleTbl.SetGrid(1,6,"N");
	xClassic3HoleTbl.SetGrid(1,6,"N");
	xClassic5HoleTbl.SetGrid(1,6,"N");
	xClassic2HoleTbl.SetColumnWide(7,13.2);	//左螺栓间距
	xClassic3HoleTbl.SetColumnWide(7,13.2);	//左螺栓端距
	xClassic5HoleTbl.SetColumnWide(7,8);	//左侧螺栓间距
	xClassic2HoleTbl.SetGrid(1,7,"L");
	xClassic3HoleTbl.SetGrid(1,7,"L1");
	xClassic5HoleTbl.SetGrid(1,7,"A1");
	xClassic2HoleTbl.SetColumnWide(8,13.2);	//右螺栓间距
	xClassic3HoleTbl.SetColumnWide(8,13.2);	//右螺栓间距
	xClassic5HoleTbl.SetColumnWide(8,8);	//右侧螺栓间距
	xClassic2HoleTbl.GetGridAt(1,8)->m_bHGridVirtual=TRUE;
	xClassic3HoleTbl.SetGrid(1,8,"L2");
	xClassic5HoleTbl.SetGrid(1,8,"A2");
	xClassic2HoleTbl.SetColumnWide(9,13.1);	//杆件总长
	xClassic3HoleTbl.SetColumnWide(9,13.1);	//杆件总长
	xClassic5HoleTbl.SetColumnWide(9,9.4);	//左螺栓端距
	xClassic2HoleTbl.GetGridAt(1,9)->m_bHGridVirtual=TRUE;
	xClassic3HoleTbl.SetGrid(1,9,"L");
	xClassic5HoleTbl.SetGrid(1,9,"L1");
	xClassic2HoleTbl.SetColumnWide(10,8.5);	//左螺栓孔径
	xClassic3HoleTbl.SetColumnWide(10,8.5);	//左螺栓孔径
	xClassic5HoleTbl.SetColumnWide(10,9.4);	//右螺栓端距
	xClassic2HoleTbl.SetGrid(1,10,"D1");
	xClassic3HoleTbl.SetGrid(1,10,"D1");
	xClassic5HoleTbl.SetGrid(1,10,"L2");
	xClassic2HoleTbl.SetColumnWide(11,8.5);	//中间螺栓孔径
	xClassic3HoleTbl.SetColumnWide(11,8.5);	//中间螺栓孔径
	xClassic5HoleTbl.SetColumnWide(11,10.7);//杆件总长
	xClassic2HoleTbl.SetGrid(1,11,"");
	xClassic3HoleTbl.SetGrid(1,11,"D2");
	xClassic5HoleTbl.SetGrid(1,11,"L");
	xClassic2HoleTbl.SetColumnWide(12,8.5);	//右螺栓孔径
	xClassic3HoleTbl.SetColumnWide(12,8.5);	//右螺栓孔径
	xClassic5HoleTbl.SetColumnWide(12,7.5);	//1号螺栓孔径
	xClassic5HoleTbl.SetColumnWide(13,7.5);	//2号螺栓孔径
	xClassic5HoleTbl.SetColumnWide(14,7.5);	//3号螺栓孔径
	xClassic5HoleTbl.SetColumnWide(15,7.5);	//4号螺栓孔径
	xClassic5HoleTbl.SetColumnWide(16,7.5);	//5号螺栓孔径
	xClassic2HoleTbl.SetGrid(1,12,"D3");
	xClassic3HoleTbl.SetGrid(1,12,"D3");
	xClassic5HoleTbl.SetGrid(1,12,"D1");
	xClassic5HoleTbl.SetGrid(1,13,"D2");
	xClassic5HoleTbl.SetGrid(1,14,"D3");
	xClassic5HoleTbl.SetGrid(1,15,"D4");
	xClassic5HoleTbl.SetGrid(1,16,"D5");
	xClassic2HoleTbl.SetColumnWide(13,30);	//特殊工艺备注
	xClassic3HoleTbl.SetColumnWide(13,30);	//特殊工艺备注
	xClassic5HoleTbl.SetColumnWide(17,30);	//特殊工艺备注
#ifdef AFX_TARG_ENU_ENGLISH
	xClassic2HoleTbl.SetGrid(1,13,"special process");
	xClassic3HoleTbl.SetGrid(1,13,"special process");
	xClassic5HoleTbl.SetGrid(1,17,"special process");
#else
	xClassic2HoleTbl.SetGrid(1,13,"特殊工艺");
	xClassic3HoleTbl.SetGrid(1,13,"特殊工艺");
	xClassic5HoleTbl.SetGrid(1,17,"特殊工艺");
#endif

	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	//单面双孔同心典型构件
	int iSum=0;
	i=0;
	C2HoleJgPart *p2HolePart;
	int nLocatePos=-1;	//定位方式
	const int TABLE_SPACE=10;
	f2dPoint inputBasePos;	//手动选择的基准位置 wht 18-11-08
	for(p2HolePart=x2HoleJgList.GetFirst();p2HolePart;p2HolePart=x2HoleJgList.GetNext())
	{
		double D1=p2HolePart->m_fD1;
		double D2=p2HolePart->m_fD2;
		int M =p2HolePart->m_nStartSpace;
		int N =p2HolePart->m_nEndSpace;
		if(p2HolePart->m_eBaseWing==Y_WING)
		{
			D1=p2HolePart->m_fD2;
			D2=p2HolePart->m_fD1;
			M =p2HolePart->m_nEndSpace;
			N =p2HolePart->m_nStartSpace;
		}
		xClassic2HoleTbl.SetGrid(i+2,0,p2HolePart->sPartNo);
		xClassic2HoleTbl.SetGrid(i+2,1,p2HolePart->spec);
		CXhChar16 steelmark;
		QuerySteelMatMark(p2HolePart->cMaterial,steelmark);
		xClassic2HoleTbl.SetGrid(i+2,2,steelmark);
		CXhChar50 ss;
		ss.Printf("%d",p2HolePart->num);
		xClassic2HoleTbl.SetGrid(i+2,3,ss);	//数量
		ss.Printf("%d",p2HolePart->m_nG);
		xClassic2HoleTbl.SetGrid(i+2,4,ss);	//心距
		ss.Printf("%d",M);	//始端距
		xClassic2HoleTbl.SetGrid(i+2,5,ss);			//始端距
		ss.Printf("%d",N);	//终端距
		xClassic2HoleTbl.SetGrid(i+2,6,ss);			//终端距
		ss.Printf("%d",p2HolePart->m_nLength);		//杆件总长
		xClassic2HoleTbl.SetGrid(i+2,7,ss);			//螺栓间距
		if(p2HolePart->m_eBaseWing==X_WING)
		{
			ss.Printf("%.1f",D1);		//左螺栓孔径
			xClassic2HoleTbl.SetGrid(i+2,10,ss);			//左螺栓孔径
			ss.Printf("%.1f",D2);			//右螺栓孔径
			xClassic2HoleTbl.SetGrid(i+2,12,ss);		//右螺栓孔径
		}
		else
		{
			ss.Printf("%.1f",p2HolePart->m_fD2);		//左螺栓孔径
			xClassic2HoleTbl.SetGrid(i+2,10,ss);			//左螺栓孔径
			ss.Printf("%.1f",p2HolePart->m_fD1);			//右螺栓孔径
			xClassic2HoleTbl.SetGrid(i+2,12,ss);		//右螺栓孔径
		}
		xClassic2HoleTbl.SetGrid(i+2,13,p2HolePart->sNotes);
		i++;
		iSum++;
		if(i==31)	//一页已满，应重新起一新页
		{
			if(nLocatePos!=-1)
			{
				f2dPoint tablePos;
				frameTbl.GetBasePos(tablePos);
				tablePos.y-=(frameTbl.GetTableHeight()+TABLE_SPACE);
				frameTbl.SetBasePos(tablePos);
			}
			BOOL bPromptPos=(nLocatePos==-1);
			DrawTable(pBlockTableRecord,&frameTbl,bPromptPos,1,nLocatePos);
			if(bPromptPos)
				frameTbl.GetBasePos(inputBasePos);
			nLocatePos=frameTbl.GetLocateStyle();
			frameTbl.GetTblRect(&tbl_rect);
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
			xClassic2HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
			xClassic2HoleTbl.GetGridAt(0,0)->data.SetGridString("2HoleInWingX");
			xClassic2HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
			xClassic2HoleTbl.SetLocateStyle(TOP_LEFT);
			DrawTable(pBlockTableRecord,&xClassic2HoleTbl,FALSE);
			xClassic2HoleTbl.GetTblRect(&tbl_rect);
			xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
			xHeaderTbl.SetLocateStyle(TOP_RIGHT);
			DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
			xClassic2HoleTbl.CleanGrid(2,i+2);		//一页已满以后，需要清空指定数据区域，否则会出现重复数据
			i=0;	//重新计数
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	ads_prompt(L"\n The following output is the table for typical parts whose double holes are on leg X and concentric!");
#else
	ads_prompt("\n The following output is the table for typical parts whose double holes are on leg X and concentric!");
#endif
#else
#ifdef _ARX_2007
	ads_prompt(L"\n下面输出的是双孔位于X肢且同心距的典型构件表!");
#else
	ads_prompt("\n下面输出的是双孔位于X肢且同心距的典型构件表!");
#endif
#endif
	if(i>0)
	{
		if(nLocatePos!=-1)
		{
			f2dPoint tablePos;
			frameTbl.GetBasePos(tablePos);
			tablePos.y-=(frameTbl.GetTableHeight()+TABLE_SPACE);
			frameTbl.SetBasePos(tablePos);
		}
		BOOL bPromptPos=(nLocatePos==-1);
		DrawTable(pBlockTableRecord,&frameTbl,bPromptPos,1,nLocatePos);
		if(bPromptPos)
			frameTbl.GetBasePos(inputBasePos);
		nLocatePos=frameTbl.GetLocateStyle();
		frameTbl.GetTblRect(&tbl_rect);
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
		xClassic2HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
		xClassic2HoleTbl.GetGridAt(0,0)->data.SetGridString("2HoleInWingX");
		xClassic2HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
		xClassic2HoleTbl.SetLocateStyle(TOP_LEFT);
		DrawTable(pBlockTableRecord,&xClassic2HoleTbl,FALSE);
		xClassic2HoleTbl.GetTblRect(&tbl_rect);
		xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
		xHeaderTbl.SetLocateStyle(TOP_RIGHT);
		DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
	}
	/*if(x2HoleJgList.GetNodeNum()!=iSum)	//存在有不可调换的Y肢螺栓典型件
	{
		for(i=2;i<xClassic2HoleTbl.GetRowsCount();i++)
		for(j=0;j<xClassic2HoleTbl.GetColumnsCount();j++)
			xClassic2HoleTbl.SetGrid(i,j,"");
		i=0;
		for(p2HolePart=x2HoleJgList.GetFirst();p2HolePart;p2HolePart=x2HoleJgList.GetNext())
		{
			if(p2HolePart->m_eBaseWing==Y_WING&&(p2HolePart->m_fD1!=p2HolePart->m_fD2||p2HolePart->m_nStartSpace!=p2HolePart->m_nEndSpace))
			{
				xClassic2HoleTbl.SetGrid(i+2,0,p2HolePart->sPartNo);
				xClassic2HoleTbl.SetGrid(i+2,1,p2HolePart->spec);
				CXhChar16 steelmark;
				QuerySteelMatMark(p2HolePart->cMaterial,steelmark);
				xClassic2HoleTbl.SetGrid(i+2,2,steelmark);
				CXhChar16 ss;
				ss.Printf("%d",p2HolePart->num);
				xClassic2HoleTbl.SetGrid(i+2,3,ss);	//数量
				ss.Printf("%d",p2HolePart->m_nG);
				xClassic2HoleTbl.SetGrid(i+2,4,ss);	//心距
				ss.Printf("%d",p2HolePart->m_nStartSpace);	//始端距
				xClassic2HoleTbl.SetGrid(i+2,5,ss);			//始端距
				ss.Printf("%d",p2HolePart->m_nEndSpace);	//终端距
				xClassic2HoleTbl.SetGrid(i+2,6,ss);			//终端距
				ss.Printf("%d",p2HolePart->m_nLength);		//杆件总长
				xClassic2HoleTbl.SetGrid(i+2,7,ss);			//螺栓间距
				ss.Printf("%.1f",p2HolePart->m_fD1);		//左螺栓孔径
				xClassic2HoleTbl.SetGrid(i+2,10,ss);			//左螺栓孔径
				ss.Printf("%.1f",p2HolePart->m_fD2);			//右螺栓孔径
				xClassic2HoleTbl.SetGrid(i+2,12,ss);		//右螺栓孔径
				xClassic2HoleTbl.SetGrid(i+2,13,p2HolePart->sNotes);
				i++;
				if(i==31)	//一页已满，应重新起一新页
				{
					DrawTable(pBlockTableRecord,&frameTbl);
					frameTbl.GetTblRect(&tbl_rect);
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
					xClassic2HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
					xClassic2HoleTbl.GetGridAt(0,0)->data.SetGridString("2HoleInWingY");
					xClassic2HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
					xClassic2HoleTbl.SetLocateStyle(TOP_LEFT);
					DrawTable(pBlockTableRecord,&xClassic2HoleTbl,FALSE);
					xClassic2HoleTbl.GetTblRect(&tbl_rect);
					xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
					xHeaderTbl.SetLocateStyle(TOP_RIGHT);
					DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
					i=0;	//重新计数
				}
			}
		}
		// 提示用户信息
#ifdef _ARX_2007
		ads_prompt(L"\n下面输出的是双孔位于Y肢且同心距的典型构件表!");
#else
		ads_prompt("\n下面输出的是双孔位于Y肢且同心距的典型构件表!");
#endif
		if(i>0)
		{
			DrawTable(pBlockTableRecord,&frameTbl);
			frameTbl.GetTblRect(&tbl_rect);
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
			xClassic2HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
			xClassic2HoleTbl.GetGridAt(0,0)->data.SetGridString("2HoleInWingY");
			xClassic2HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
			xClassic2HoleTbl.SetLocateStyle(TOP_LEFT);
			DrawTable(pBlockTableRecord,&xClassic2HoleTbl,FALSE);
			xClassic2HoleTbl.GetTblRect(&tbl_rect);
			xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
			xHeaderTbl.SetLocateStyle(TOP_RIGHT);
			DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
		}
	}*/

	//单面三孔同心典型构件
	i=0;
	iSum=0;
	C3HoleJgPart *p3HolePart;
	int iPage=0;
	for(p3HolePart=x3HoleJgList.GetFirst();p3HolePart;p3HolePart=x3HoleJgList.GetNext())
	{
		double D1=p3HolePart->m_fD1;
		double D2=p3HolePart->m_fD2;
		double D3=p3HolePart->m_fD3;
		int L1=p3HolePart->m_nMiddleLsLeftSpace;
		int L2=p3HolePart->m_nMiddleLsRightSpace;
		int M =p3HolePart->m_nStartSpace;
		int N =p3HolePart->m_nEndSpace;
		if(p3HolePart->m_eBaseWing==Y_WING)
		{
			D1=p3HolePart->m_fD3;
			D3=p3HolePart->m_fD1;
			L1=p3HolePart->m_nMiddleLsRightSpace;
			L2=p3HolePart->m_nMiddleLsLeftSpace;
			M =p3HolePart->m_nEndSpace;
			N =p3HolePart->m_nStartSpace;
		}
		xClassic3HoleTbl.SetGrid(i+2,0,p3HolePart->sPartNo);
		xClassic3HoleTbl.SetGrid(i+2,1,p3HolePart->spec);
		CXhChar16 steelmark;
		QuerySteelMatMark(p3HolePart->cMaterial,steelmark);
		xClassic3HoleTbl.SetGrid(i+2,2,steelmark);
		CXhChar50 ss;
		ss.Printf("%d",p3HolePart->num);
		xClassic3HoleTbl.SetGrid(i+2,3,ss);	//数量
		ss.Printf("%d",p3HolePart->m_nG);
		xClassic3HoleTbl.SetGrid(i+2,4,ss);	//心距
		ss.Printf("%d",M);	//始端距
		xClassic3HoleTbl.SetGrid(i+2,5,ss);				//始端距
		ss.Printf("%d",N);	//终端距
		xClassic3HoleTbl.SetGrid(i+2,6,ss);				//终端距
		ss.Printf("%d",L1);	//中间螺栓左端距
		xClassic3HoleTbl.SetGrid(i+2,7,ss);				//中间螺栓左端距
		ss.Printf("%d",L2);	//中间螺栓右端距
		xClassic3HoleTbl.SetGrid(i+2,8,ss);				//中间螺栓右端距
		ss.Printf("%d",L1+L2);//螺栓间距
		xClassic3HoleTbl.SetGrid(i+2,9,ss);				//杆件总长
		if(p3HolePart->m_eBaseWing==X_WING)
		{
			ss.Printf("%.1f",D1);		//左螺栓孔径
			xClassic3HoleTbl.SetGrid(i+2,10,ss);			//左螺栓孔径
			ss.Printf("%.1f",D2);		//右螺栓孔径
			xClassic3HoleTbl.SetGrid(i+2,11,ss);			//右螺栓孔径
			ss.Printf("%.1f",D3);		//左螺栓孔径
			xClassic3HoleTbl.SetGrid(i+2,12,ss);			//左螺栓孔径
		}
		else
		{
			ss.Printf("%.1f",p3HolePart->m_fD3);		//左螺栓孔径
			xClassic3HoleTbl.SetGrid(i+2,10,ss);				//左螺栓孔径
			ss.Printf("%.1f",p3HolePart->m_fD2);		//左螺栓孔径
			xClassic3HoleTbl.SetGrid(i+2,11,ss);				//左螺栓孔径
			ss.Printf("%.1f",p3HolePart->m_fD1);			//右螺栓孔径
			xClassic3HoleTbl.SetGrid(i+2,12,ss);			//右螺栓孔径
		}
		xClassic3HoleTbl.SetGrid(i+2,13,p3HolePart->sNotes);
		i++;
		iSum++;
		if(i==31)	//此页已满，另起新页
		{
			if(nLocatePos!=-1)
			{	//自动计算表格初始位置
				f2dPoint tablePos=inputBasePos;
				if(iPage>0)
				{
					frameTbl.GetBasePos(tablePos);
					tablePos.y-=(frameTbl.GetTableHeight()+TABLE_SPACE);
				}
				else
					tablePos.x+=(frameTbl.GetTableWide()+TABLE_SPACE);
				frameTbl.SetBasePos(tablePos);
			}
			BOOL bPromptPos=(nLocatePos==-1);
			DrawTable(pBlockTableRecord,&frameTbl,bPromptPos,1,nLocatePos);
			if(bPromptPos)
				frameTbl.GetBasePos(inputBasePos);
			nLocatePos=frameTbl.GetLocateStyle();
			frameTbl.GetTblRect(&tbl_rect);
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
			xClassic3HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
			xClassic3HoleTbl.GetGridAt(0,0)->data.SetGridString("3HoleInWingX");
			xClassic3HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
			xClassic3HoleTbl.SetLocateStyle(TOP_LEFT);
			DrawTable(pBlockTableRecord,&xClassic3HoleTbl,FALSE);
			xClassic3HoleTbl.GetTblRect(&tbl_rect);
			xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
			xHeaderTbl.SetLocateStyle(TOP_RIGHT);
			DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
			xClassic3HoleTbl.CleanGrid(2,i+2);		//一页已满以后，需要清空指定数据区域，否则会出现重复数据
			iPage++;
			i=0;	//重新计数
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	ads_prompt(L"\n The following output is the table for typical parts whose three holes are on leg X and concentric!");
#else
	ads_prompt("\n The following output is the table for typical parts whose three holes are on leg X and concentric!");
#endif
#else
#ifdef _ARX_2007
	ads_prompt(L"\n下面输出的是叁孔位于X肢且同心距的典型构件表!");
#else
	ads_prompt("\n下面输出的是叁孔位于X肢且同心距的典型构件表!");
#endif
#endif
	if(i>0)
	{
		if(nLocatePos!=-1)
		{
			f2dPoint tablePos=inputBasePos;
			if(iPage>0)
			{
				frameTbl.GetBasePos(tablePos);
				tablePos.y-=(frameTbl.GetTableHeight()+TABLE_SPACE);
			}
			else
				tablePos.x+=(frameTbl.GetTableWide()+TABLE_SPACE);
			frameTbl.SetBasePos(tablePos);
		}
		BOOL bPromptPos=(nLocatePos==-1);
		DrawTable(pBlockTableRecord,&frameTbl,bPromptPos,1,nLocatePos);
		if(bPromptPos)
			frameTbl.GetBasePos(inputBasePos);
		nLocatePos=frameTbl.GetLocateStyle();
		frameTbl.GetTblRect(&tbl_rect);
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
		xClassic3HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
		xClassic3HoleTbl.GetGridAt(0,0)->data.SetGridString("3HoleInWingX");
		xClassic3HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
		xClassic3HoleTbl.SetLocateStyle(TOP_LEFT);
		DrawTable(pBlockTableRecord,&xClassic3HoleTbl,FALSE);
		xClassic3HoleTbl.GetTblRect(&tbl_rect);
		xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
		xHeaderTbl.SetLocateStyle(TOP_RIGHT);
		DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
	}
	if(x3HoleJgList.GetNodeNum()>0&&x2HoleJgList.GetNodeNum()>0)
		inputBasePos.x+=(frameTbl.GetTableWide()+TABLE_SPACE);	//基准点向右偏移一列 wht 18-11-08
	/*if(x3HoleJgList.GetNodeNum()!=iSum)	//存在有不可调换的Y肢螺栓典型件
	{
		for(i=2;i<xClassic3HoleTbl.GetRowsCount();i++)
		for(j=0;j<xClassic3HoleTbl.GetColumnsCount();j++)
			xClassic3HoleTbl.SetGrid(i,j,"");
		i=0;
		for(p3HolePart=x3HoleJgList.GetFirst();p3HolePart;p3HolePart=x3HoleJgList.GetNext())
		{
			if(p3HolePart->m_eBaseWing==Y_WING&&
				(p3HolePart->m_fD1!=p3HolePart->m_fD3||p3HolePart->m_nStartSpace!=p3HolePart->m_nEndSpace||p3HolePart->m_nMiddleLsLeftSpace!=p3HolePart->m_nMiddleLsRightSpace))
			{
				xClassic3HoleTbl.SetGrid(i+2,0,p3HolePart->sPartNo);
				xClassic3HoleTbl.SetGrid(i+2,1,p3HolePart->spec);
				CXhChar16 steelmark;
				QuerySteelMatMark(p3HolePart->cMaterial,steelmark);
				xClassic3HoleTbl.SetGrid(i+2,2,steelmark);
				CXhChar50 ss;
				ss.Printf("%d",p3HolePart->num);
				xClassic3HoleTbl.SetGrid(i+2,3,ss);	//数量
				ss.Printf("%d",p3HolePart->m_nG);
				xClassic3HoleTbl.SetGrid(i+2,4,ss);	//心距
				ss.Printf("%d",p3HolePart->m_nStartSpace);	//始端距
				xClassic3HoleTbl.SetGrid(i+2,5,ss);			//始端距
				ss.Printf("%d",p3HolePart->m_nEndSpace);	//终端距
				xClassic3HoleTbl.SetGrid(i+2,6,ss);			//终端距
				ss.Printf("%d",p3HolePart->m_nMiddleLsLeftSpace);	//中间螺栓左端距
				xClassic3HoleTbl.SetGrid(i+2,7,ss);					//中间螺栓左端距
				ss.Printf("%d",p3HolePart->m_nMiddleLsRightSpace);	//中间螺栓右端距
				xClassic3HoleTbl.SetGrid(i+2,8,ss);					//中间螺栓右端距
				ss.Printf("%d",p3HolePart->m_nMiddleLsLeftSpace+p3HolePart->m_nMiddleLsRightSpace);//杆件总长
				xClassic3HoleTbl.SetGrid(i+2,9,ss);			//杆件总长
				ss.Printf("%.1f",p3HolePart->m_fD1);	//左螺栓孔径
				xClassic3HoleTbl.SetGrid(i+2,10,ss);		//左螺栓孔径
				ss.Printf("%.1f",p3HolePart->m_fD2);	//中间螺栓孔径
				xClassic3HoleTbl.SetGrid(i+2,11,ss);		//中间螺栓孔径
				ss.Printf("%.1f",p3HolePart->m_fD3);	//右螺栓孔径
				xClassic3HoleTbl.SetGrid(i+2,12,ss);		//右螺栓孔径
				xClassic3HoleTbl.SetGrid(i+2,13,p3HolePart->sNotes);
				i++;
				if(i==31)	//此页已满，另起新页
				{
					DrawTable(pBlockTableRecord,&frameTbl);
					frameTbl.GetTblRect(&tbl_rect);
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
					xClassic3HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
					xClassic3HoleTbl.GetGridAt(0,0)->data.SetGridString("3HoleInWingY");
					xClassic3HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
					xClassic3HoleTbl.SetLocateStyle(TOP_LEFT);
					DrawTable(pBlockTableRecord,&xClassic3HoleTbl,FALSE);
					xClassic3HoleTbl.GetTblRect(&tbl_rect);
					xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
					xHeaderTbl.SetLocateStyle(TOP_RIGHT);
					DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
					i=0;	//重新计数
				}
			}
		}
		// 提示用户信息
#ifdef _ARX_2007
		ads_prompt(L"\n下面输出的是双孔位于Y肢且同心距的典型构件表!");
#else
		ads_prompt("\n下面输出的是双孔位于Y肢且同心距的典型构件表!");
#endif
		if(i>0)
		{
			DrawTable(pBlockTableRecord,&frameTbl);
			frameTbl.GetTblRect(&tbl_rect);
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
			xClassic3HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
			xClassic3HoleTbl.GetGridAt(0,0)->data.SetGridString("3HoleInWingY");
			xClassic3HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
			xClassic3HoleTbl.SetLocateStyle(TOP_LEFT);
			DrawTable(pBlockTableRecord,&xClassic3HoleTbl,FALSE);
			xClassic3HoleTbl.GetTblRect(&tbl_rect);
			xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
			xHeaderTbl.SetLocateStyle(TOP_RIGHT);
			DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
		}
	}*/

	//单面五孔同心典型构件
	i=0;
	iSum=0;
	iPage=0;
	C5HoleJgPart *p5HolePart;
	for(p5HolePart=x5HoleJgList.GetFirst();p5HolePart;p5HolePart=x5HoleJgList.GetNext())
	{
		double D1=p5HolePart->m_fD1;
		double D2=p5HolePart->m_fD2;
		double D3=p5HolePart->m_fD3;
		double D4=p5HolePart->m_fD4;
		double D5=p5HolePart->m_fD5;
		int A1=p5HolePart->m_nLeftLsSpace;
		int A2=p5HolePart->m_nRightLsSpace;
		int L1=p5HolePart->m_nMiddleLsLeftSpace;
		int L2=p5HolePart->m_nMiddleLsRightSpace;
		int M =p5HolePart->m_nStartSpace;
		int N =p5HolePart->m_nEndSpace;
		if(p5HolePart->m_eBaseWing==Y_WING)
		{
			D1=p5HolePart->m_fD5;
			D2=p5HolePart->m_fD4;
			D4=p5HolePart->m_fD2;
			D5=p5HolePart->m_fD1;
			A1=p5HolePart->m_nRightLsSpace;
			A2=p5HolePart->m_nLeftLsSpace;
			L1=p5HolePart->m_nMiddleLsRightSpace;
			L2=p5HolePart->m_nMiddleLsLeftSpace;
			M =p5HolePart->m_nEndSpace;
			N =p5HolePart->m_nStartSpace;
		}
		xClassic5HoleTbl.SetGrid(i+2,0,p5HolePart->sPartNo);
		xClassic5HoleTbl.SetGrid(i+2,1,p5HolePart->spec);
		CXhChar16 steelmark;
		QuerySteelMatMark(p5HolePart->cMaterial,steelmark);
		xClassic5HoleTbl.SetGrid(i+2,2,steelmark);
		CXhChar50 ss;
		ss.Printf("%d",p5HolePart->num);
		xClassic5HoleTbl.SetGrid(i+2,3,ss);	//数量
		ss.Printf("%d",p5HolePart->m_nG);
		xClassic5HoleTbl.SetGrid(i+2,4,ss);	//心距
		ss.Printf("%d",M);	//始端距
		xClassic5HoleTbl.SetGrid(i+2,5,ss);			//始端距
		ss.Printf("%d",N);	//终端距
		xClassic5HoleTbl.SetGrid(i+2,6,ss);			//终端距
		ss.Printf("%d",A1);	//左侧螺栓间距
		xClassic5HoleTbl.SetGrid(i+2,7,ss);				//左侧螺栓间距
		ss.Printf("%d",A2);	//左侧螺栓间距
		xClassic5HoleTbl.SetGrid(i+2,8,ss);				//左侧螺栓间距
		ss.Printf("%d",L1);	//中间螺栓左端距
		xClassic5HoleTbl.SetGrid(i+2,9,ss);					//中间螺栓左端距
		ss.Printf("%d",L2);	//中间螺栓右端距
		xClassic5HoleTbl.SetGrid(i+2,10,ss);				//中间螺栓右端距
		ss.Printf("%d",L1+L2);//螺栓间距
		xClassic5HoleTbl.SetGrid(i+2,11,ss);					//杆件总长
		//if(p5HolePart->m_eBaseWing==X_WING)
		{
			ss.Printf("%.1f",D1);		//1号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,12,ss);		//1号螺栓孔径
			if(D2==D1)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",D2);	//2号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,13,ss);		//2号螺栓孔径
			if(D3==D2)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",D3);	//3号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,14,ss);		//3号螺栓孔径
			if(D4==D3)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",D4);	//4号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,15,ss);		//4号螺栓孔径
			if(D5==D4)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",D5);	//5号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,16,ss);		//5号螺栓孔径
		}
		/*else
		{
			ss.Printf("%.1f",p5HolePart->m_fD5);		//1号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,12,ss);		//1号螺栓孔径
			if(p5HolePart->m_fD4==p5HolePart->m_fD5)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",p5HolePart->m_fD4);	//2号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,13,ss);		//2号螺栓孔径
			if(p5HolePart->m_fD3==p5HolePart->m_fD4)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",p5HolePart->m_fD3);	//3号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,14,ss);		//3号螺栓孔径
			if(p5HolePart->m_fD2==p5HolePart->m_fD3)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",p5HolePart->m_fD2);	//4号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,15,ss);		//4号螺栓孔径
			if(p5HolePart->m_fD1==p5HolePart->m_fD2)
				strcpy(ss,"..");
			else
				ss.Printf("%.1f",p5HolePart->m_fD1);	//5号螺栓孔径
			xClassic5HoleTbl.SetGrid(i+2,16,ss);		//5号螺栓孔径
		}*/
		xClassic5HoleTbl.SetGrid(i+2,17,p5HolePart->sNotes);
		i++;
		iSum++;
		if(i==31)	//此页已满，另起新页
		{
			if(nLocatePos!=-1)
			{	//自动计算表格初始位置
				f2dPoint tablePos=inputBasePos;
				if(iPage>0)
				{
					frameTbl.GetBasePos(tablePos);
					tablePos.y-=(frameTbl.GetTableHeight()+TABLE_SPACE);
				}
				else
					tablePos.x+=(frameTbl.GetTableWide()+TABLE_SPACE);
				frameTbl.SetBasePos(tablePos);
			}
			BOOL bPromptPos=(nLocatePos==-1);
			DrawTable(pBlockTableRecord,&frameTbl,bPromptPos,1,nLocatePos);
			if(bPromptPos)
				frameTbl.GetBasePos(inputBasePos);
			nLocatePos=frameTbl.GetLocateStyle();
			frameTbl.GetTblRect(&tbl_rect);
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
			xClassic5HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
			xClassic5HoleTbl.GetGridAt(0,0)->data.SetGridString("5HoleInWingX");
			xClassic5HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
			xClassic5HoleTbl.SetLocateStyle(TOP_LEFT);
			DrawTable(pBlockTableRecord,&xClassic5HoleTbl,FALSE);
			xClassic5HoleTbl.GetTblRect(&tbl_rect);
			xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
			xHeaderTbl.SetLocateStyle(TOP_RIGHT);
			DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
			xClassic5HoleTbl.CleanGrid(2,i+2);		//一页已满以后，需要清空指定数据区域，否则会出现重复数据
			iPage++;
			i=0;	//重新计数
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	ads_prompt(L"\n The following output is the table for typical parts whose three holes are on leg X and concentric!");
#else
	ads_prompt("\n The following output is the table for typical parts whose three holes are on leg X and concentric!");
#endif
#else
#ifdef _ARX_2007
	ads_prompt(L"\n下面输出的是叁孔位于X肢且同心距的典型构件表!");
#else
	ads_prompt("\n下面输出的是叁孔位于X肢且同心距的典型构件表!");
#endif
#endif
	if(i>0)
	{
		if(nLocatePos!=-1)
		{
			f2dPoint tablePos=inputBasePos;
			if(iPage>0)
			{
				frameTbl.GetBasePos(tablePos);
				tablePos.y-=(frameTbl.GetTableHeight()+TABLE_SPACE);
			}
			else
				tablePos.x+=(frameTbl.GetTableWide()+TABLE_SPACE);
			frameTbl.SetBasePos(tablePos);
		}
		BOOL bPromptPos=(nLocatePos==-1);
		DrawTable(pBlockTableRecord,&frameTbl,bPromptPos,1,nLocatePos);
		if(bPromptPos)
			frameTbl.GetBasePos(inputBasePos);
		nLocatePos=frameTbl.GetLocateStyle();
		frameTbl.GetTblRect(&tbl_rect);
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
		CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
		xClassic5HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
		xClassic5HoleTbl.GetGridAt(0,0)->data.SetGridString("5HoleInWingX");
		xClassic5HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
		xClassic5HoleTbl.SetLocateStyle(TOP_LEFT);
		DrawTable(pBlockTableRecord,&xClassic5HoleTbl,FALSE);
		xClassic5HoleTbl.GetTblRect(&tbl_rect);
		xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
		xHeaderTbl.SetLocateStyle(TOP_RIGHT);
		DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
	}
	/*if(x5HoleJgList.GetNodeNum()!=iSum)	//存在有不可调换的Y肢螺栓典型件
	{
		for(i=2;i<xClassic5HoleTbl.GetRowsCount();i++)
		for(j=0;j<xClassic5HoleTbl.GetColumnsCount();j++)
			xClassic5HoleTbl.SetGrid(i,j,"");
		i=0;
		for(p5HolePart=x5HoleJgList.GetFirst();p5HolePart;p5HolePart=x5HoleJgList.GetNext())
		{
			if(p5HolePart->m_eBaseWing==Y_WING&&
				(p5HolePart->m_fD1!=p5HolePart->m_fD5||p5HolePart->m_fD2!=p5HolePart->m_fD4||p5HolePart->m_nStartSpace!=p5HolePart->m_nEndSpace||p5HolePart->m_nMiddleLsLeftSpace!=p5HolePart->m_nMiddleLsRightSpace))
			{
				xClassic5HoleTbl.SetGrid(i+2,0,p5HolePart->sPartNo);
				xClassic5HoleTbl.SetGrid(i+2,1,p5HolePart->spec);
				CXhChar16 steelmark;
				QuerySteelMatMark(p5HolePart->cMaterial,steelmark);
				xClassic5HoleTbl.SetGrid(i+2,2,steelmark);
				CXhChar50 ss;
				ss.Printf("%d",p5HolePart->num);
				xClassic5HoleTbl.SetGrid(i+2,3,ss);	//数量
				ss.Printf("%d",p5HolePart->m_nG);
				xClassic5HoleTbl.SetGrid(i+2,4,ss);	//心距
				ss.Printf("%d",p5HolePart->m_nStartSpace);	//始端距
				xClassic5HoleTbl.SetGrid(i+2,5,ss);			//始端距
				ss.Printf("%d",p5HolePart->m_nEndSpace);	//终端距
				xClassic5HoleTbl.SetGrid(i+2,6,ss);			//终端距
				ss.Printf("%d",p5HolePart->m_nLeftLsSpace);	//左侧螺栓间距
				xClassic5HoleTbl.SetGrid(i+2,7,ss);				//左侧螺栓间距
				ss.Printf("%d",p5HolePart->m_nRightLsSpace);	//右侧螺栓间距
				xClassic5HoleTbl.SetGrid(i+2,8,ss);				//右侧螺栓间距
				ss.Printf("%d",p5HolePart->m_nMiddleLsLeftSpace);	//中间螺栓左端距
				xClassic5HoleTbl.SetGrid(i+2,9,ss);					//中间螺栓左端距
				ss.Printf("%d",p5HolePart->m_nMiddleLsRightSpace);	//中间螺栓右端距
				xClassic5HoleTbl.SetGrid(i+2,10,ss);				//中间螺栓右端距
				ss.Printf("%d",p5HolePart->m_nMiddleLsLeftSpace+p5HolePart->m_nMiddleLsRightSpace);//杆件总长
				xClassic5HoleTbl.SetGrid(i+2,11,ss);			//杆件总长
				ss.Printf("%.1f",p5HolePart->m_fD1);	//1号螺栓孔径
				xClassic5HoleTbl.SetGrid(i+2,12,ss);	//1号螺栓孔径
				ss.Printf("%.1f",p5HolePart->m_fD2);	//2号螺栓孔径
				xClassic5HoleTbl.SetGrid(i+2,13,ss);	//2号螺栓孔径
				ss.Printf("%.1f",p5HolePart->m_fD3);	//3号螺栓孔径
				xClassic5HoleTbl.SetGrid(i+2,14,ss);	//3号螺栓孔径
				ss.Printf("%.1f",p5HolePart->m_fD4);	//4号螺栓孔径
				xClassic5HoleTbl.SetGrid(i+2,15,ss);	//4号螺栓孔径
				ss.Printf("%.1f",p5HolePart->m_fD5);	//5号螺栓孔径
				xClassic5HoleTbl.SetGrid(i+2,16,ss);	//5号螺栓孔径
				xClassic5HoleTbl.SetGrid(i+2,17,p5HolePart->sNotes);
				i++;
				if(i==31)	//此页已满，另起新页
				{
					DrawTable(pBlockTableRecord,&frameTbl);
					frameTbl.GetTblRect(&tbl_rect);
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
					CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
					xClassic5HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
					xClassic5HoleTbl.GetGridAt(0,0)->data.SetGridString("5HoleInWingY");
					xClassic5HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
					xClassic5HoleTbl.SetLocateStyle(TOP_LEFT);
					DrawTable(pBlockTableRecord,&xClassic5HoleTbl,FALSE);
					xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
					xHeaderTbl.SetLocateStyle(TOP_RIGHT);
					DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
					i=0;	//重新计数
				}
			}
		}
		// 提示用户信息
#ifdef _ARX_2007
		ads_prompt(L"\n下面输出的是双孔位于Y肢且同心距的典型构件表!");
#else
		ads_prompt("\n下面输出的是双孔位于Y肢且同心距的典型构件表!");
#endif
		if(i>0)
		{
			DrawTable(pBlockTableRecord,&frameTbl);
			frameTbl.GetTblRect(&tbl_rect);
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-111),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-111));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-104),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-118));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+5.5,tbl_rect.topLeft.y-188),f3dPoint(tbl_rect.topLeft.x+19.5,tbl_rect.topLeft.y-188));
			CreateAcadLine(pBlockTableRecord,f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-181),f3dPoint(tbl_rect.topLeft.x+12.5,tbl_rect.topLeft.y-195));
			xClassic5HoleTbl.GetGridAt(0,0)->grid_mode = PIC_BLOCK_GRID;
			xClassic5HoleTbl.GetGridAt(0,0)->data.SetGridString("5HoleInWingY");
			xClassic5HoleTbl.SetBasePos(f2dPoint(tbl_rect.topLeft.x+25,tbl_rect.topLeft.y-5));
			xClassic5HoleTbl.SetLocateStyle(TOP_LEFT);
			DrawTable(pBlockTableRecord,&xClassic5HoleTbl,FALSE);
			xClassic5HoleTbl.GetTblRect(&tbl_rect);
			xHeaderTbl.SetBasePos(tbl_rect.bottomRight);
			xHeaderTbl.SetLocateStyle(TOP_RIGHT);
			DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
		}
	}*/
	if((x2HoleJgList.GetNodeNum()>0||x3HoleJgList.GetNodeNum()>0)&&x5HoleJgList.GetNodeNum()>0)
		inputBasePos.x+=(frameTbl.GetTableWide()+TABLE_SPACE);	//基准点向右偏移一列 wht 18-11-08
	if(cutanglejgset.GetNodeNum()>0)
	{
		for(i=0;i<cutanglejgset.GetNodeNum()-1;i++)   //冒泡排序
		{
			BOOL bSorted = TRUE;
			for(j=0;j<cutanglejgset.GetNodeNum()-i-1;j++)
			{
				//if(stricmp(cutanglejgset[j]->GetPartNo(),cutanglejgset[j+1]->GetPartNo())>0)
				if(ComparePartNoString(cutanglejgset[j]->GetPartNo(),cutanglejgset[j+1]->GetPartNo())>0)
				{
					CLDSLineAngle* pJg=cutanglejgset[j];
					cutanglejgset[j] = cutanglejgset[j+1];
					cutanglejgset[j+1]=pJg;
					bSorted = FALSE;
				}
			}
			if(bSorted)
				break;
		}
		for(i=0;i<xCutAngleTbl.GetRowsCount();i++)
		{
			xCutAngleTbl.SetRowHigh(i,10);
			for(j=0;j<xCutAngleTbl.GetColumnsCount();j++)
				xCutAngleTbl.GetGridAt(i,j)->feature = TEXTALIGN_MIDDLE_CENTER;
			if(i>0 && i%2==0)
			{
				xCutAngleTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
				xCutAngleTbl.GetGridAt(i,1)->m_bVGridVirtual = TRUE;
				xCutAngleTbl.GetGridAt(i,2)->m_bVGridVirtual = TRUE;
				xCutAngleTbl.GetGridAt(i,5)->m_bVGridVirtual = TRUE;
			}
		}
		xCutAngleTbl.GetGridAt(0,4)->m_bHGridVirtual = TRUE;
		xCutAngleTbl.SetColumnWide(0,25);
		xCutAngleTbl.SetColumnWide(1,20);
		xCutAngleTbl.SetColumnWide(2,55);
		xCutAngleTbl.SetColumnWide(3,25);
		xCutAngleTbl.SetColumnWide(4,25);
		xCutAngleTbl.SetColumnWide(5,20);
#ifdef AFX_TARG_ENU_ENGLISH
		xCutAngleTbl.SetGrid(0,0,"Part No.");
		xCutAngleTbl.SetGrid(0,1,"spec");
		xCutAngleTbl.SetGrid(0,2,"cut angle shape");
		xCutAngleTbl.SetGrid(0,3,"Cut angle para");
		xCutAngleTbl.SetGrid(0,5,"notes");
#else
		xCutAngleTbl.SetGrid(0,0,"零件编号");
		xCutAngleTbl.SetGrid(0,1,"规  格");
		xCutAngleTbl.SetGrid(0,2,"切角形状");
		xCutAngleTbl.SetGrid(0,3,"切角切肢参数");
		xCutAngleTbl.SetGrid(0,5,"备  注");
#endif
		for(i=0;i<cutanglejgset.GetNodeNum();i++)
		{
			CLDSLineAngle *pJg=cutanglejgset[i];
			xCutAngleTbl.SetGrid(i*2+1,0,GetPartNoIncMat(pJg->GetPartNo(),pJg->cMaterial));
			CXhChar50 spec;
			spec.Printf("L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
			xCutAngleTbl.SetGrid(i*2+1,1,spec);
#ifdef AFX_TARG_ENU_ENGLISH
			spec.Printf("centre distance %.0f",pJg->GetLsG(pJg->GetFirstLsRef()));
#else
			spec.Printf("心距%.0f",pJg->GetLsG(pJg->GetFirstLsRef()));
#endif
			xCutAngleTbl.SetGrid(i*2+1,5,spec);
			CXhChar50 xWingCut,yWingCut;
			CCutAngleBriefMap::GetAngleCutAngleInfo(pJg,xWingCut,yWingCut,true,sys.part_map.angle.iCutAngleDimType);
			xCutAngleTbl.SetGrid(i*2+2,3,xWingCut);
			xCutAngleTbl.SetGrid(i*2+1,3,yWingCut);
			CCutAngleBriefMap::GetAngleCutAngleInfo(pJg,xWingCut,yWingCut,false,sys.part_map.angle.iCutAngleDimType);
			xCutAngleTbl.SetGrid(i*2+2,4,xWingCut);
			xCutAngleTbl.SetGrid(i*2+1,4,yWingCut);
		}
		if (nLocatePos != -1)
		{	//自动计算表格初始位置
			f2dPoint tablePos = inputBasePos;
			if(nLocatePos == 0|| nLocatePos == 2)
				tablePos.x += (frameTbl.GetTableWide() + TABLE_SPACE);
			else
				tablePos.x += (xCutAngleTbl.GetTableWide() + TABLE_SPACE);
			xCutAngleTbl.SetBasePos(tablePos);
			if (nLocatePos == 0)	//左上角
				xCutAngleTbl.SetLocateStyle(TOP_LEFT);
			else if (nLocatePos == 1)//右上角
				xCutAngleTbl.SetLocateStyle(TOP_RIGHT);
			else if (nLocatePos == 2)//左下角
				xCutAngleTbl.SetLocateStyle(BOTTOM_LEFT);
			else if (nLocatePos == 3)//右下角
				xCutAngleTbl.SetLocateStyle(BOTTOM_RIGHT);
		}
		DrawTable(pBlockTableRecord,&xCutAngleTbl,nLocatePos==-1,1,nLocatePos);
	}
	pBlockTableRecord->close();
	if(cutanglejgset.GetNodeNum()>0)
	{
		for(i=0;i<cutanglejgset.GetNodeNum();i++)
		{
			CLDSLineAngle *pJg=cutanglejgset[i];
			CLineAngleDrawing xBriefJgDraw;
			f2dRect draw_rect;
			xCutAngleTbl.GetGridRect(i*2+1,2,draw_rect);
			xBriefJgDraw.dimStyleFlag.Clear();
			xBriefJgDraw.dimStyleFlag.SetFlagDword(CLineAngleDrawing::BriefDrawing);
			if(sys.part_map.angle.bDimCutAngle)	//标注切角信息
				xBriefJgDraw.dimStyleFlag.SetFlagDword(CLineAngleDrawing::CutAngleDimByText);
			xBriefJgDraw.CreateLineAngleDrawing(pJg,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
			{
				f3dPoint origin;
				origin.x = draw_rect.topLeft.x;
				origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
				DrawJgMap(&xBriefJgDraw,origin,TRUE);
			}
		}
	}
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
}

//以下两个生成工艺卡图表的函数来自于ProcessCard.cpp
/*f2dRect CreateJgProcessCardTbl(CFlexTbl *pFlexTbl,f2dPoint insert_pos,double wide=225,double high=165,
						  BOOL bPrompt = FALSE,TBL_LOCATE_STYLE locate_style=BOTTOM_LEFT);
f2dRect CreatePlankProcessCardTbl(CFlexTbl *pFlexTbl,f2dPoint insert_pos,double wide=200,double high=126,
							 BOOL bPrompt=FALSE,TBL_LOCATE_STYLE locate_style=BOTTOM_LEFT);*/
CXhChar16 GetMarkSymbol();
f2dRect GetProcessCardDrawingRect(int iProcessCardType=0);
f2dRect InsertPartProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,PART_JG_CARD_INFO *pTubeInfo,LDSCLASS_TYPEID cls_id,BOOL bPrompt = FALSE);
f2dRect InsertJgProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,PART_JG_CARD_INFO *pJgInfo,BOOL bPrompt = FALSE);
f2dRect InsertPlankProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,PART_PLANK_CARD_INFO *pPlankInfo,AcDbObjectId &id,BOOL bPrompt = FALSE);
//进行材料汇总，并生成材料汇总表//from DrawTable.cpp
//----------------------------------------------------------------------------------------
static void trans_pos(f3dPoint &pt,f3dPoint origin)
{
	pt+=origin;
}

static void trans_pos(f3dLine &line,f3dPoint origin)
{
	line.startPt+=origin;
	line.endPt+=origin;
}

static void trans_pos(f3dArcLine &line,f3dPoint origin)
{
	f3dPoint startPt=line.Start();
	f3dPoint endPt=line.End();
	f3dPoint center=line.Center();
	double radius=line.Radius();
	startPt+=origin;
	endPt+=origin;
	center+=origin;
	if(line.SectorAngle()==0)
		line.CreateMethod2(startPt,endPt,line.WorkNorm(),0);
	else if(line.ColumnNorm()==line.WorkNorm())	//圆
		line.CreateMethod1(center,startPt,line.WorkNorm(),line.SectorAngle());
	else //if	//椭圆
		line.CreateEllipse(center,startPt,endPt,line.ColumnNorm(),line.WorkNorm(),radius);
}
//line_arr为输出参数,其余为输入参数
int DrawHuoQuLine(CLineAngleDrawing *pMainDraw,CLDSLineAngle *pPrevSeg, CLDSLineAngle *pCurSeg,CLDSPolyPart *pHuoQuJg,int Start0End1Mid2,f3dPoint origin,char *layer="temp")
{
	UCS_STRU ucs;
	double wing_wide,wing_thick,length_x,length_y;
	f3dLine line,line_arr[2];
	f3dPoint wing_x_vec,wing_y_vec,result_vec,acad_vec;
	ANGULARDIM_STRU angular_dim;
	CArray<ANGULARDIM_STRU,ANGULARDIM_STRU&>angular_dim_arr;
	if(pCurSeg==NULL||pPrevSeg==NULL)
		return 0;

	wing_wide = pHuoQuJg->GetWidth();
	wing_thick=pHuoQuJg->GetThick();
	//计算前一火曲角钢段的相对坐标系
	pPrevSeg->getUCS(ucs);

	result_vec = pCurSeg->End()-pCurSeg->Start();
	/*if( pPrevSeg->pStart==pSeg->pStart&&DISTANCE(pPrevSeg->Start(),pSeg->Start())<2.0)
	{
		wing_x_vec = pCurSeg->get_norm_x_wing()^pPrevSeg->get_norm_x_wing();
		wing_y_vec = pPrevSeg->get_norm_y_wing()^pCurSeg->get_norm_y_wing();
	}
	else if(pPrevSeg->pEnd==pSeg->pStart&&DISTANCE(pPrevSeg->End(),pSeg->Start())<2.0)
	{
	}
	else if()
	{
	}
	else 
	{
	}*/
	wing_x_vec = pCurSeg->get_norm_x_wing()^pPrevSeg->get_norm_x_wing();
	wing_y_vec = pPrevSeg->get_norm_y_wing()^pCurSeg->get_norm_y_wing();
	double angle_x,angle_y;
	angle_x = asin(wing_x_vec.mod());
	angle_y = asin(wing_y_vec.mod());
	normalize(result_vec);
	normalize(wing_x_vec);
	normalize(wing_y_vec);
	vector_trans(result_vec,ucs,FALSE);
	vector_trans(wing_x_vec,ucs,FALSE);
	vector_trans(wing_y_vec,ucs,FALSE);
	if(wing_x_vec.x<0)
	{
		wing_x_vec = -wing_x_vec;
		angle_x = Pi-angle_x;
	}
	else
		angle_x = Pi+angle_x;
	if(wing_y_vec.y<0)
	{
		wing_y_vec = -wing_y_vec;
		angle_y = Pi-angle_y;
	}
	else
		angle_y = Pi+angle_y;
	if(fabs(wing_x_vec.x)>eps)
		length_x = wing_x_vec.mod()*wing_wide/fabs(wing_x_vec.x);
	else
		length_x = 0.0;
	if(fabs(wing_y_vec.y)>eps)
		length_y = wing_y_vec.mod()*wing_wide/fabs(wing_y_vec.y);
	else
		length_y = 0.0;
	length_x = min(length_x,2*wing_wide);
	length_y = min(length_y,2*wing_wide);

	acad_vec.Set(wing_x_vec.z,-wing_x_vec.x,-wing_x_vec.y);
	line_arr[0].endPt = length_x*acad_vec;
	acad_vec.Set(wing_y_vec.z,wing_y_vec.y,-wing_y_vec.x);
	line_arr[1].endPt = length_y*acad_vec;

	int i;
	for(i=0;i<2;i++)
	{
		if(line_arr[i].endPt.mod2()<eps)
			continue;	//只是一点不是线
		angular_dim.vertex = line_arr[i].startPt;
		angular_dim.startPos.Set(-wing_wide*0.3,0,0);
		angular_dim.endPos = line_arr[i].endPt*0.3;
		angular_dim.endPos.z = 0;
		angular_dim.dimPos = (angular_dim.startPos+angular_dim.endPos)*0.5;
		angular_dim_arr.Add(angular_dim);
	}
	//绘制火曲线
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表
	for(i=0;i<2;i++)
	{
		if(line_arr[i].endPt.mod2()>=eps)
		{
			line_arr[i].feature = 2;//表示火曲线型
			trans_pos(line_arr[i],origin);
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line_arr[i].startPt,
				line_arr[i].endPt);
		}
	}
	
	//标注火曲参数
	//计算火曲变形处长度缩短值
	f3dPoint vec_start,vec_end,vec_mid;
	vec_start = pPrevSeg->Start()-pPrevSeg->End();
	vec_end   = pCurSeg->End()-pCurSeg->Start();
	normalize(vec_start);
	normalize(vec_end);
	vec_mid = (vec_start+vec_end)/2.0;
	double angle = cal_angle_of_2vec(vec_start,vec_end)/2.0;
	double sub_len = (tan(Pi/2.0-angle)*pPrevSeg->huoqu_r-(Pi/2-angle)*pPrevSeg->huoqu_r)*2;
	CXhChar50 sAngleText;
	//sAngleText,"火曲方向(%.2f,%.2f,%.2f),变形缩短值%.1f",result_vec.x,result_vec.y,result_vec.z,sub_len);
#ifdef AFX_TARG_ENU_ENGLISH
	sAngleText.Printf("bending line(%.1f,%.1f), angle between two planes=%.1f,Deformation reduced value %.1f",wing_wide*wing_y_vec.z/wing_y_vec.y,
#else
	sAngleText.Printf("火曲线(%.1f,%.1f),两面夹角=%.1f,变形缩短值%.1f",wing_wide*wing_y_vec.z/wing_y_vec.y,
#endif
		wing_wide,angle_y*DEGTORAD_COEF,sub_len);
	f3dPoint base;
	base=origin;
	base.x+=5;
	base.y+=3;
	DimText(pBlockTableRecord,base,sAngleText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fOtherTextSize,0);//sys.dim_map.fDimTextSize,0);

#ifdef AFX_TARG_ENU_ENGLISH
	sAngleText.Printf("bending line(%.1f,%.1f),angle between two planes=%.1f",wing_wide*wing_x_vec.z/wing_x_vec.x,wing_wide,angle_x*DEGTORAD_COEF);
#else
	sAngleText.Printf("火曲线(%.1f,%.1f),两面夹角=%.1f",wing_wide*wing_x_vec.z/wing_x_vec.x,wing_wide,angle_x*DEGTORAD_COEF);
#endif
	base=origin;
	base.x+=5;
	base.y =base.y-3-sys.fonts.angle.fOtherTextSize;//sys.dim_map.fDimTextSize;
	DimText(pBlockTableRecord,base,sAngleText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fOtherTextSize,0);//sys.dim_map.fDimTextSize,0);
	pBlockTableRecord->close();
	return 1;
}
//绘制分段火曲角钢
BOOL DrawHuoQuJg(CLDSPolyPart *pHuoQuJg,f2dRect *pDrawRect,int iOrder,
				 int nHits,char *layer,int nPageNum/*=1*/,
				 int nTaNum/*=1*/,BOOL bDimPartInfo/*=TRUE*/,int iPage/*=-1*/)
{
	CPolyAngleDrawing xJgDraw;
	f3dPoint origin,old_origin,start_origin,mid_origin,end_origin;	//分段火曲角钢中每段角钢的绘图原点

	f3dPoint base;
	if(iOrder>0)
	{	//计算火曲角钢构件图（原点）插入位置
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}
	old_origin = origin;
	Cpy_Pnt(base,origin);
	f2dRect draw_rect=GetProcessCardDrawingRect();
	if(!sys.general_set.bProcessCardTbl&&pDrawRect)
	{
		draw_rect=*pDrawRect;	//未启用工艺卡图框且外界指定绘图区域 wht 11-06-02
		if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
		{
			origin.x = draw_rect.topLeft.x;
			origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
		}
	}
	xJgDraw.CreatePolyAngleDrawing(pHuoQuJg,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);

//------------------------------------------------------------------------------------
	//标注此构件的注释行
	double length,weight;
	CXhChar100 sNotes;
	CXhChar50 sPartNo,guige;
	guige.Printf("L%.0fX%.0f",pHuoQuJg->GetWidth(),pHuoQuJg->GetThick());
	length = pHuoQuJg->GetLength();
	//添加以下代码主要是为了使工艺卡上的角钢长度与中间大样图上的角钢总长标注小数点后一位也一致
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		length = ftoi(length);
	else if(sys.general_set.iDimPrecision==1)//精确到0.5mm
		length = ftoi(length*2)/2.0;
	else if(sys.general_set.iDimPrecision==2)//精确到0.1mm
		length = ftol(length*10)/10.0;				
	strcpy(sPartNo,GetPartNoIncMat(pHuoQuJg->segset.GetFirst()->GetPartNo(),pHuoQuJg->segset.GetFirst()->cMaterial));
	weight = pHuoQuJg->GetWeight();//length*GetJgUnitLenWei(pHuoQuJg->GetWidth(),pHuoQuJg->GetThick())/1000;
//------------------------------------------------------------------------------------
	if(sys.general_set.bProcessCardTbl)
	{
		CFlexTbl xJgProcessCard;
		f2dRect actual_rect,tm_actual_rect;
		f2dRect draw_rect;
		PART_JG_CARD_INFO JgInfo;
		memset(&JgInfo,'\0',sizeof(PART_JG_CARD_INFO));
		JgInfo.m_nPageNum=nPageNum;		//总页数
		if(iPage==-1&&iOrder<0)
			JgInfo.m_nPageIndex=1;	//第几页
		else if (iPage!=-1)
			JgInfo.m_nPageIndex=iPage+1;	//第几页
		else
			JgInfo.m_nPageIndex=iOrder+1;	//第几页
		JgInfo.m_nTaNum = nTaNum;
		CLDSLineAngle *pFirstJg=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();
		JgInfo.sPartNo.Copy(GetPartNoIncMat(pFirstJg->GetPartNo(),pFirstJg->cMaterial));
		if(fabs(length-(int)length)>EPS)
			JgInfo.sLength.Printf("%.1f",length);
		else
			JgInfo.sLength.Printf("%.0f",length);
		for(CLDSLineAngle *pJg=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pJg;pJg=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
		{
			if(pJg->m_bCutRoot)	//刨根处理
			{
				JgInfo.sCutRoot.Copy(GetMarkSymbol());
				JgInfo.sCutBer.Empty();
				//暂时仅支持标注短角钢刨根长度 wht 10-11-17
				if(pJg->pStart==NULL||pJg->pEnd==NULL)
					JgInfo.sCutRootLen.Printf("%.0f",length);
				else 
					JgInfo.sCutRootLen.Empty();
				JgInfo.sCutBerLen.Empty();
			}
			else if(pJg->m_bCutBer)	//铲背处理
			{
				if(pJg->m_nCutBerR<=0||pJg->m_nCutBerR>pJg->GetWidth())
				{	//重新计算角钢的铲背半径
					CLDSLineAngle* pDatumJg=(CLDSLineAngle*)Ta.FromPartHandle(pJg->desStartPos.datumPoint.des_para.RODEND.hRod);
					if(pDatumJg==NULL)
						pDatumJg=(CLDSLineAngle*)Ta.FromPartHandle(pJg->desEndPos.datumPoint.des_para.RODEND.hRod);
					if(pDatumJg)
						pJg->m_nCutBerR=(WORD)FindJg_r(pDatumJg->GetWidth())+2;
					else
						pJg->m_nCutBerR=0;
				}
				JgInfo.m_nCutBerR=(pJg->m_nCutBerR>JgInfo.m_nCutBerR)?pJg->m_nCutBerR:JgInfo.m_nCutBerR;
				JgInfo.sCutRoot.Empty();
				JgInfo.sCutBer.Copy(GetMarkSymbol());
				//暂时仅支持标注短角钢铲背长度 wht 10-11-17
				if(pJg->pStart==NULL||pJg->pEnd==NULL)
					JgInfo.sCutBerLen.Printf("%.0f",length);
				else 
					JgInfo.sCutBerLen.Empty();
				JgInfo.sCutRootLen.Empty();
			}
			else
			{
				JgInfo.sCutRoot.Empty();
				JgInfo.sCutBer.Empty();
				JgInfo.sCutRootLen.Empty();
				JgInfo.sCutBerLen.Empty();
			}
			if(pJg->m_bWeldPart)
				JgInfo.sWeld.Copy(GetMarkSymbol());
			else
				JgInfo.sWeld.Empty();
		}
		
		JgInfo.sPartNum.Printf("%d",nHits);
		JgInfo.sSumPartNum.Printf("%d",nHits*JgInfo.m_nTaNum);
		
		JgInfo.sPartWeight.Printf("%.1f",weight);
		JgInfo.sSumPartWeight.Printf("%.1f",weight*nHits*JgInfo.m_nTaNum);
		//使用【开合角阈值】控制是否需要标注开合角信息默认为2°wht 11-05-06
		double wing_angle=pFirstJg->GetDecWingAngle();
		int kaihe_base_wing=pFirstJg->GetKaiHeBaseWing();
		if(kaihe_base_wing==0||kaihe_base_wing==1)
		{
			if(wing_angle-90>sys.part_map.angle.fKaiHeJiaoThreshold)
				JgInfo.sKaiJiao.Printf("+%.1f",wing_angle-90);
			else if(90-wing_angle>sys.part_map.angle.fKaiHeJiaoThreshold)
				JgInfo.sHeJiao.Printf("-%.1f",90-wing_angle);
		}
		//
		JgInfo.sWingAngle.Printf("%.1f°",wing_angle);
		int nLsN=0,nSumLsN=0;
		CLDSLineAngle *pSonAngle;
		for(pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSonAngle;
			pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
				nLsN+=pSonAngle->GetLsCount(12);
		JgInfo.sM12HoleNum.Printf("%d",nLsN);
		nSumLsN+=nLsN;

		nLsN=0;
		for(pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSonAngle;
			pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
				nLsN+=pSonAngle->GetLsCount(16);
		JgInfo.sM16HoleNum.Printf("%d",nLsN);
		nSumLsN+=nLsN;

		nLsN=0;
		for(pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSonAngle;
			pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
				nLsN+=pSonAngle->GetLsCount(18);
		JgInfo.sM18HoleNum.Printf("%d",nLsN);
		nSumLsN+=nLsN;
		
		nLsN=0;
		for(pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSonAngle;
			pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
				nLsN+=pSonAngle->GetLsCount(20);
		JgInfo.sM20HoleNum.Printf("%d",nLsN);
		nSumLsN+=nLsN;

		nLsN=0;
		for(pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSonAngle;
			pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
				nLsN+=pSonAngle->GetLsCount(22);
		JgInfo.sM22HoleNum.Printf("%d",nLsN);
		nSumLsN+=nLsN;

		nLsN=0;
		for(pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSonAngle;
			pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
				nLsN+=pSonAngle->GetLsCount(24);
		JgInfo.sM24HoleNum.Printf("%d",nLsN);
		nSumLsN+=nLsN;
		JgInfo.sSumHoleNum.Printf("%d",nSumLsN);
		//统计钻孔数
		nLsN=0;
		for(pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();pSonAngle;
			pSonAngle=(CLDSLineAngle*)pHuoQuJg->segset.GetNext())
		{
			for(CLsRef *pLsRef=pSonAngle->GetFirstLsRef();pLsRef;pLsRef=pSonAngle->GetNextLsRef())
			{
				if(pLsRef->cFlag==CLsRef::DRILL_HOLE)
					nLsN++;
			}
		}
		if(nLsN>0)
			JgInfo.sZuanKongNum.Printf("%d",nLsN);
		int nChongKongNum=nSumLsN-nLsN;
		if(nChongKongNum>0)
			JgInfo.sChongKongNum.Printf("%d",nChongKongNum);
		//
		if(pHuoQuJg->segset.GetNodeNum()>1)
			JgInfo.sHuoQuFst.Copy(GetMarkSymbol());
		else
			JgInfo.sHuoQuFst.Empty();
		if(pHuoQuJg->segset.GetNodeNum()>2)
			JgInfo.sHuoQuSec.Copy(GetMarkSymbol());
		else
			JgInfo.sHuoQuSec.Empty();
		
		
		int n=guige.GetLength();
		CXhChar16 steelmark;
		QuerySteelMatMark(pFirstJg->cMaterial,steelmark);
		JgInfo.sDesMaterial.Copy(steelmark);
		JgInfo.sDesGuiGe.Copy(guige);
		//
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		draw_rect = InsertJgProcessCardTbl(pBlockTableRecord,origin,&JgInfo);
		origin.x = draw_rect.topLeft.x;
		origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
		if(sys.part_map.angle.bDimCutAngleMap)
		{	//插入制弯角钢火曲图示
			double fOffY=12,fTextY=5,fTextSize=2;//sys.fonts.angle.fPartNoTextSize;
			for(int i=0;i<xJgDraw.GetHuoQuLineCount();i++)
			{
				HUOQU_LINE xHuoquLine=xJgDraw.GetHuoQuLine(i);
				f3dLine line(xHuoquLine.startPt,xHuoquLine.endPt);
				trans_pos(line,origin);
				f3dPoint base_pos,dim_pos;
				if(xHuoquLine.huoqu_type==0)
				{	//X肢内曲
					base_pos.Set(line.startPt.x,draw_rect.bottomRight.y+fOffY,0);
					InsertBlock(pBlockTableRecord,base_pos,"HuoQuXI",1,0);
					dim_pos.Set(base_pos.x,base_pos.y+fTextY);
					CXhChar50 sDimText("%.1f",180-xHuoquLine.huoqu_angle);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,fTextSize,0,AcDb::kTextCenter);
				}
				else if(xHuoquLine.huoqu_type==1)
				{	//X肢外曲
					base_pos.Set(line.startPt.x,draw_rect.bottomRight.y+fOffY,0);
					InsertBlock(pBlockTableRecord,base_pos,"HuoQuXO",1,0);
					dim_pos.Set(base_pos.x,base_pos.y-(fTextY+fTextSize));
					CXhChar50 sDimText("%.1f",180-xHuoquLine.huoqu_angle);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,fTextSize,0,AcDb::kTextCenter);
				}
				else if(xHuoquLine.huoqu_type==2)
				{	//Y肢内曲
					base_pos.Set(line.startPt.x,draw_rect.topLeft.y-fOffY);
					InsertBlock(pBlockTableRecord,base_pos,"HuoQuYI",1,0);
					dim_pos.Set(base_pos.x,base_pos.y-(fTextY+fTextSize));
					CXhChar50 sDimText("%.1f",180-xHuoquLine.huoqu_angle);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,fTextSize,0,AcDb::kTextCenter);
				}
				else if(xHuoquLine.huoqu_type==3)
				{	//Y肢外曲
					base_pos.Set(line.startPt.x,draw_rect.topLeft.y-fOffY);
					InsertBlock(pBlockTableRecord,base_pos,"HuoQuYO",1,0);
					dim_pos.Set(base_pos.x,base_pos.y+fTextY);
					CXhChar50 sDimText("%.1f",180-xHuoquLine.huoqu_angle);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,fTextSize,0,AcDb::kTextCenter);
				}
			}
		}
		//
		CLDSLineAngle* pSonJg=(CLDSLineAngle*)pHuoQuJg->segset.GetFirst();
		CLDSLineAngle* pSecJg=(CLDSLineAngle*)pHuoQuJg->segset.GetNext();
		if(pSonJg->m_bCutBer||pSonJg->m_bCutRoot)
		{
			f3dPoint dim_pos(draw_rect.topLeft.x+draw_rect.Width()/2,draw_rect.bottomRight.y+20,0);
			CXhChar50 sDimText;
			if(pSonJg->m_bCutRoot)
				strcpy(sDimText,"全长清根");
			else if(pSonJg->m_bCutBer)
			{
				double fAngle1=0,fAngle2=0;
				if(pSonJg->huoqu_base_wing_x0_y1_both2==2)
				{
					if(xJgDraw.GetHuoQuLineCount()>0)
						fAngle1=xJgDraw.GetHuoQuLine(0).huoqu_angle;
					if(xJgDraw.GetHuoQuLineCount()>1)
						fAngle2=xJgDraw.GetHuoQuLine(1).huoqu_angle;
				}
				if(fAngle1>0&&fabs(fAngle1-fAngle2)<1e-6)
				{
					f3dPoint vecS=(pSonJg->Start()-pSonJg->End()).normalized();
					f3dPoint vecE=(pSecJg->End()-pSecJg->Start()).normalized();
					double vec_angle=ACOS(vecS,vecE)*DEGTORAD_COEF;
					sprintf(sDimText,"向背火曲角度(%.1f),全长铲背",vec_angle);
				}
				else
					strcpy(sDimText,"全长铲背");
			}
			DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fPartNoTextSize,0,AcDb::kTextCenter);
		}
		pBlockTableRecord->close();
	}
//------------------------------------------------------------------------------------
	DrawJgMap(&xJgDraw,origin);
	if(!sys.general_set.bProcessCardTbl&&bDimPartInfo)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sNotes.Printf("Tower type -%s part number. %s* %d  %s length=%.0f mm Need bending",(char*)Ta.m_sTaType,(char*)sPartNo,nHits,(char*)guige,length);
#else
		sNotes.Printf("塔型-%s 件号. %s* %d  %s 长度=%.0f mm 需要制弯",(char*)Ta.m_sTaType,(char*)sPartNo,nHits,(char*)guige,length);
#endif
		base.y -= (pHuoQuJg->GetWidth()+100)/sys.general_set.PartMapScale.fRealToDraw+sys.dim_map.PartNo.fPartNoTextSize*6;
		if(pDrawRect)
		{	//根据指定的矩形框,计算备注标注位置 wht 11-06-02
			base.x = sNotes.GetLength()*sys.dim_map.PartNo.fPartNoTextSize*0.5;
			base.y = sys.dim_map.PartNo.fPartNoTextSize*5;
		}
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
		pBlockTableRecord->close();
	}
	return TRUE;
}
//origin为绘图原点,主要是为了处理火曲角钢而添加的
//BOOL DrawJgMap(CLineAngleDrawing *pJgDraw,f3dPoint origin,BOOL bDimStyle2/*=FALSE*/)
/*{
	int i,n;
	f3dLine line;
	f3dCircle Cir;
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表
	n = pJgDraw->GetLineCount();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(i=0;i<n;i++)
	{
		pJgDraw->GetLineAt(line,i);
		trans_pos(line,origin);
		CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	}
	for(i=0;i<5;i++)
	{
		pJgDraw->GetZEdge(line,i);
		trans_pos(line,origin);
		if(line.feature==0)
		{
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
		}
		else
		{
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		}
	}
	n = pJgDraw->GetXLsCount();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	CXhChar16 sM12("M12"),sM16("M16"),sM18("M18"),sM20("M20"),sM22("M22"),sM24("M24");
	if(sys.part_map.angle.bJgUseSimpleLsMap)
	{
		sM12.Copy("M12_Jg");
		sM16.Copy("M16_Jg");
		sM18.Copy("M18_Jg");
		sM20.Copy("M20_Jg");
		sM22.Copy("M22_Jg");
		sM24.Copy("M24_Jg");
	}
	for(i=0;i<n;i++)
	{
		pJgDraw->GetXLsCircle(Cir,i);
		trans_pos(Cir.centre,origin);
		//NewCircle(pBlockTableRecord,Cir);
		if(fabs(2*Cir.radius-12)<EPS)	//按3mm直径螺栓绘制
			InsertBlock(pBlockTableRecord,Cir.centre,sM12,1.2,0);
		else if(fabs(2*Cir.radius-16)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM16,1.2,0);
		else if(fabs(2*Cir.radius-20)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM20,1.2,0);
		else if(fabs(2*Cir.radius-22)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM22,1.2,0);
		else if(fabs(2*Cir.radius-24)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM24,1.2,0);
		else
		{
			CXhChar16 ls_str;
			ls_str.Printf("M%d",ftol(2*Cir.radius));
			InsertBlock(pBlockTableRecord,Cir.centre,ls_str,1.2,0);
		}
	}
	n = pJgDraw->GetYLsCount();
	for(i=0;i<n;i++)
	{
		pJgDraw->GetYLsCircle(Cir,i);
		trans_pos(Cir.centre,origin);
		//NewCircle(pBlockTableRecord,Cir);
		if(fabs(2*Cir.radius-12)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM12,1.2,0);
		else if(fabs(2*Cir.radius-16)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM16,1.2,0);
		else if(fabs(2*Cir.radius-20)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM20,1.2,0);
		else if(fabs(2*Cir.radius-22)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM22,1.2,0);
		else if(fabs(2*Cir.radius-24)<EPS)
			InsertBlock(pBlockTableRecord,Cir.centre,sM24,1.2,0);
		else
		{
			CXhChar16 ls_str;
			ls_str.Printf("M%d",ftol(2*Cir.radius));
			InsertBlock(pBlockTableRecord,Cir.centre,ls_str,1.2,0);
		}
	}
	CSizeTextDim jg_dim;
	n = pJgDraw->GetDimCount();
	CXhChar200 sDimText;
	AcDbObjectId dimStyleId;
	if(bDimStyle2)
		dimStyleId = DimStyleTable::dimStyle2.dimStyleId;
	else
		dimStyleId = DimStyleTable::dimStyle.dimStyleId;
	GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
	//标注螺栓相对孔距尺寸
	//if(sys.part_map.bDimLsSpace)
	{
		for(i=0;i<n;i++)
		{
			pJgDraw->GetDimAt(i,&jg_dim);
			if(jg_dim.dist==0)
				continue;
			jg_dim.dimPos+=jg_dim.origin;
			jg_dim.dimStartPos+=jg_dim.origin;
			jg_dim.dimEndPos+=jg_dim.origin;
			trans_pos(jg_dim.dimPos,origin);
			trans_pos(jg_dim.dimStartPos,origin);
			trans_pos(jg_dim.dimEndPos,origin);
			if(sys.part_map.angle.bDimLsSpace&&jg_dim.nHits>0 || sys.part_map.angle.bDimCutAngle&&jg_dim.nHits<=0)
			{
				DimAngleSize(pBlockTableRecord,
					jg_dim.dimStartPos,jg_dim.dimEndPos,
					jg_dim.dimPos,jg_dim.DimText(),dimStyleId,jg_dim.angle);
			}
		}
	}

	CZhunXianTextDim zhun_dim;
	n = pJgDraw->GetZhunDimCount();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(i=0;i<n;i++)
	{
		pJgDraw->GetZhunDimAt(i,&zhun_dim);
		double txt_offset=zhun_dim.dimPos.x-zhun_dim.dimStartPos.x;
		zhun_dim.dimStartPos+=zhun_dim.origin;
		zhun_dim.dimEndPos+=zhun_dim.origin;
		zhun_dim.dimPos+=zhun_dim.origin;
		trans_pos(zhun_dim.dimPos,origin);
		trans_pos(zhun_dim.dimStartPos,origin);
		trans_pos(zhun_dim.dimEndPos,origin);
		zhun_dim.dimPos.x=zhun_dim.dimStartPos.x+txt_offset;
		sDimText.Printf("%.0f",zhun_dim.dist);
		if(zhun_dim.dimStartPos!=zhun_dim.dimEndPos)
		{
			DimAngleSize(pBlockTableRecord,
				zhun_dim.dimStartPos,zhun_dim.dimEndPos,
				zhun_dim.dimPos,sDimText,dimStyleId,zhun_dim.angle);
		}
		if(pJgDraw->dimStyleFlag.IsHasFlag(CLineAngleDrawing::DimAxisGTextAtStart))
		{
			zhun_dim.lineEnd.x+=15/sys.general_set.PartMapScale;
		}
		else
		{
			zhun_dim.lineStart.x-=15/sys.general_set.PartMapScale;
			zhun_dim.lineEnd.x+=15/sys.general_set.PartMapScale;
		}
		trans_pos(zhun_dim.lineStart,origin);
		trans_pos(zhun_dim.lineEnd,origin);
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord,zhun_dim.lineStart,zhun_dim.lineEnd);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	}
	CTextOnlyDrawing pure_txt;
	if(sys.part_map.angle.bDimLsPos)
	{
		f3dPoint base_pos;	//距离累积值标注插入点
		n = pJgDraw->GetPureTxtCount();
		SetCurLayer("DIMTEXT");
		for(i=0;i<n;i++)
		{
			if(pJgDraw->GetPureTxtDimAt(i,&pure_txt))
			{
				pure_txt.dimPos+=pure_txt.origin;
				trans_pos(pure_txt.dimPos,origin);
				if(sys.part_map.angle.iDimPrecision==0)	//精确到1mm
					sDimText.Copy(pure_txt.GetPrecisionDimText("0"));
				else if(sys.part_map.angle.iDimPrecision==1)//精确到0.5mm
					sDimText.Copy(pure_txt.GetPrecisionDimText("0.5"));
				else	//精确到0.1mm
					sDimText.Copy(pure_txt.GetPrecisionDimText("0.0"));
				if(pure_txt.bInXWing)
					DimText(pBlockTableRecord,pure_txt.dimPos,sDimText,TextStyleTable::hzfs.textStyleId,pure_txt.textHeight,Pi/2.0,AcDb::kTextRight);
				else
					DimText(pBlockTableRecord,pure_txt.dimPos,sDimText,TextStyleTable::hzfs.textStyleId,pure_txt.textHeight,Pi/2.0,AcDb::kTextLeft);
			}
		}
	}

	if(sys.part_map.angle.bDimKaiHe)
	{
		GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		CKaiHeAngleDim kaihe_dim;
		n = pJgDraw->GetKaiHeDimCount();
		double dist=0;
		for(i=0;i<n;i++)
		{
			pJgDraw->GetKaiHeDimAt(i,&kaihe_dim);
			kaihe_dim.dimPos+=kaihe_dim.origin;
			kaihe_dim.dimStartPos+=kaihe_dim.origin;
			kaihe_dim.dimEndPos+=kaihe_dim.origin;
			//kaihe_dim.start_ls_pos+=kaihe_dim.origin;
			//kaihe_dim.end_ls_pos+=kaihe_dim.origin;
			trans_pos(kaihe_dim.dimPos,origin);
			trans_pos(kaihe_dim.dimStartPos,origin);
			trans_pos(kaihe_dim.dimEndPos,origin);
			if(kaihe_dim.bInWingX)
			{
				if(kaihe_dim.start0_end1_else2==2)	//不是两端开合角
				{
					BOOL bFindStartLs=FALSE;
					double dist[3]={0,0,0};
					f3dPoint mid_pos[2];
					for(int j=0;j<pJgDraw->GetPureTxtCount();j++)
					{
						if(pJgDraw->GetPureTxtDimAt(j,&pure_txt))
						{
							if(pure_txt.bInXWing)
							{
								if(!bFindStartLs)
								{
									if(pure_txt.dist>=kaihe_dim.real_start.x)
									{
										pure_txt.dimPos+=pure_txt.origin;
										trans_pos(pure_txt.dimPos,origin);
										mid_pos[0]=pure_txt.dimPos;
										dist[0]=pure_txt.dist-kaihe_dim.real_start.x;
										bFindStartLs=TRUE;
										continue;
									}
								}
								else if(bFindStartLs)
								{
									if(pure_txt.dist>=kaihe_dim.real_end.x)
									{
										pJgDraw->GetPureTxtDimAt(j-1,&pure_txt);
										pure_txt.dimPos+=pure_txt.origin;
										trans_pos(pure_txt.dimPos,origin);
										mid_pos[1]=pure_txt.dimPos;
										dist[1]=pure_txt.dist-dist[0]-kaihe_dim.real_start.x;
										dist[2]=kaihe_dim.real_end.x-pure_txt.dist;
										break;
									}
								}
							}
						}
					}
					f3dPoint dimPos;
					if(dist[0]>1)
					{
						sDimText.Printf("%.0f",dist[0]);
						dimPos.x=kaihe_dim.dimStartPos.x-1;
						mid_pos[0].y=mid_pos[1].y=kaihe_dim.dimStartPos.y;
						dimPos.y=kaihe_dim.dimPos.y+kaihe_dim.dimStyle.textHeight*1.5;
						DimSize(pBlockTableRecord,
							kaihe_dim.dimStartPos,mid_pos[0],dimPos,sDimText,dimStyleId,0);
					}
					if(dist[1]>1)
					{
						sDimText.Printf("%.0f",dist[1]);
						dimPos.x=(mid_pos[0].x+mid_pos[1].x)/2;
						DimSize(pBlockTableRecord,
							mid_pos[0],mid_pos[1],dimPos,sDimText,dimStyleId,0);
					}
					if(dist[2]>1)
					{
						sDimText.Printf("%.0f",dist[2]);
						dimPos.x=mid_pos[1].x+1;
						DimSize(pBlockTableRecord,
							mid_pos[1],kaihe_dim.dimEndPos,dimPos,sDimText,dimStyleId,0);
					}
				}
			}
			else
			{
				if(kaihe_dim.start0_end1_else2==2)	//不是两端开合角
				{
					BOOL bFindStartLs=FALSE;
					double dist[3]={0,0,0};
					f3dPoint mid_pos[2];
					for(int j=0;j<pJgDraw->GetPureTxtCount();j++)
					{
						if(pJgDraw->GetPureTxtDimAt(j,&pure_txt))
						{
							if(!pure_txt.bInXWing)
							{
								if(!bFindStartLs)
								{
									if(pure_txt.dist>=kaihe_dim.real_start.x)
									{
										pure_txt.dimPos+=pure_txt.origin;
										trans_pos(pure_txt.dimPos,origin);
										mid_pos[0]=pure_txt.dimPos;
										dist[0]=pure_txt.dist-kaihe_dim.real_start.x;
										bFindStartLs=TRUE;
										continue;
									}
								}
								else if(bFindStartLs)
								{
									if(pure_txt.dist>=kaihe_dim.real_end.x)
									{
										pJgDraw->GetPureTxtDimAt(j-1,&pure_txt);
										pure_txt.dimPos+=pure_txt.origin;
										trans_pos(pure_txt.dimPos,origin);
										mid_pos[1]=pure_txt.dimPos;
										dist[1]=pure_txt.dist-dist[0]-kaihe_dim.real_start.x;
										dist[2]=kaihe_dim.real_end.x-pure_txt.dist;
										break;
									}
								}
							}
						}
					}
					f3dPoint dimPos;
					if(dist[0]>1)
					{
						sDimText.Printf("%.0f",dist[0]);
						dimPos.x=kaihe_dim.dimStartPos.x-1;
						mid_pos[0].y=mid_pos[1].y=kaihe_dim.dimStartPos.y;
						dimPos.y=kaihe_dim.dimPos.y-3*kaihe_dim.dimStyle.textHeight;
						DimSize(pBlockTableRecord,
							kaihe_dim.dimStartPos,mid_pos[0],dimPos,sDimText,dimStyleId,0);
					}
					if(dist[1]>1)
					{
						sDimText.Printf("%.0f",dist[1]);
						dimPos.x=(mid_pos[0].x+mid_pos[1].x)/2;
						DimSize(pBlockTableRecord,
							mid_pos[0],mid_pos[1],dimPos,sDimText,dimStyleId,0);
					}
					if(dist[2]>1)
					{
						sDimText.Printf("%.0f",dist[2]);
						dimPos.x=mid_pos[1].x+1;
						DimSize(pBlockTableRecord,
							mid_pos[1],kaihe_dim.dimEndPos,dimPos,sDimText,dimStyleId,0);
					}
				}
			}
			kaihe_dim.dimPos.x=(kaihe_dim.dimStartPos.x+kaihe_dim.dimEndPos.x)/2.0;
			DimText(pBlockTableRecord,kaihe_dim.dimPos-f3dPoint(0,kaihe_dim.dimStyle.textHeight*0.25),kaihe_dim.dimText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
			//if(kaihe_dim.start0_end1_else2!=2)	//两端开合角
			{
				dist = kaihe_dim.dimEndPos.x-kaihe_dim.dimStartPos.x;
				if(dist>EPS2)
				{
					sDimText.Printf("%.0f",kaihe_dim.dist);
					kaihe_dim.dimPos.x=(kaihe_dim.dimStartPos.x+kaihe_dim.dimEndPos.x)/2;
					//kaihe_dim.dimPos.y=kaihe_dim.dimStartPos.y+1;//+sys.dim_map.fDimTextSize+1;
					DimSize(pBlockTableRecord,
						kaihe_dim.dimStartPos,kaihe_dim.dimEndPos,
						kaihe_dim.dimPos,sDimText,dimStyleId,0);
				}
			}
		}
	}
	pBlockTableRecord->close();
	return TRUE;
}*/
//origin为绘图原点,主要是为了处理火曲角钢而添加的
BOOL DrawJgMap(CRodDrawing *pJgDraw,f3dPoint origin,BOOL bDimStyle2/*=FALSE*/,AcDbBlockTableRecord *pExterBlockTableRecord/*=NULL*/)
{
	int i,n;
	f3dLine line;
	f3dCircle Cir;
	AcDbBlockTableRecord *pBlockTableRecord;
	if(pExterBlockTableRecord==NULL)
	{	//获得当前图形的块表指针
		AcDbBlockTable *pBlockTable;
		GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
		//以写方式打开模型空间，获得块表记录指针
		pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
		pBlockTable->close();//关闭块表
	}
	else 
		pBlockTableRecord=pExterBlockTableRecord;
	if(pJgDraw->IsPolyAngleDrawing())
	{	//火曲角钢制弯线
		n = ((CPolyAngleDrawing*)pJgDraw)->GetHuoQuLineCount();
		for(i=0;i<n;i++)
		{
			((CPolyAngleDrawing*)pJgDraw)->GetHuoQuLine(line,i);
			trans_pos(line,origin);
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
		}
		//制弯角钢火曲信息标注
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		n=((CPolyAngleDrawing*)pJgDraw)->GetHuoQuTextCount();
		for(i=0;i<n;i++)
		{
			CTextOnlyDrawing* pTextDim=((CPolyAngleDrawing*)pJgDraw)->GetHuoQuTxtDimAt(i);
			if(pTextDim==NULL)
				continue;
			pTextDim->dimPos+=origin;
			DimText(pBlockTableRecord,pTextDim->dimPos,pTextDim->GetPrecisionDimText(),TextStyleTable::hzfs.textStyleId,pTextDim->textHeight,pTextDim->angle,AcDb::kTextLeft);
		}
	}
	double PRECISION=1.0;
	if(sys.general_set.iDimPrecision==0)	//精确到1.0mm
		PRECISION=1;
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
		PRECISION=0.5;
	else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
		PRECISION=0.1;
	n = pJgDraw->GetLineCount();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for (i = 0; i < n; i++)
	{
		pJgDraw->GetLineAt(line,i);
		trans_pos(line,origin);
		CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	}
	for(i=0;i<pJgDraw->GetZEdgeCount();i++)
	{
		pJgDraw->GetZEdge(line,i);
		trans_pos(line,origin);
		if(line.feature==0)
		{
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
		}
		else
		{
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		}
	}
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for (i = 0; i < pJgDraw->GetFilletArcCount(); i++)
	{
		FILLET_ARC filletArc;
		pJgDraw->GetFilletArcAt(i, filletArc);
		trans_pos(filletArc.m_ptS, origin);
		trans_pos(filletArc.m_ptE, origin);
		f3dArcLine arcline;
		if (filletArc.ExtractArc(arcline, line))
		{
			CreateAcadArcLine(pBlockTableRecord, arcline.Center(), arcline.Start(), arcline.SectorAngle(), arcline.WorkNorm(), arcline.feature);
			CreateAcadLine(pBlockTableRecord, line.startPt, line.endPt);
			CXhChar16 sText("R%.0f", filletArc.m_fFilletR);
			f3dPoint chord_pos = arcline.PositionInAngle(Pi/4),center= arcline.Center();
			f3dPoint dim_pos = chord_pos+(center-chord_pos).normalized();
			DimRadial(pBlockTableRecord, dim_pos, chord_pos,1,sText, DimStyleTable::dimRadiusStyle.dimStyleId);
		}
		else
			CreateAcadLine(pBlockTableRecord, filletArc.m_ptS, filletArc.m_ptE);
	}
	n = pJgDraw->GetXLsCount();
	CXhChar16 sM12("M12"),sM16("M16"),sM18("M18"),sM20("M20"),sM22("M22"),sM24("M24");
	CXhChar16 sM12_N("M12_Nail"),sM16_N("M16_Nail"),sM18_N("M18_Nail"),sM20_N("M20_Nail"),sM22_N("M22_Nail"),sM24_N("M24_Nail");
	if(sys.part_map.angle.bJgUseSimpleLsMap)
	{
		sM12.Copy("M12_Jg");
		sM16.Copy("M16_Jg");
		sM18.Copy("M18_Jg");
		sM20.Copy("M20_Jg");
		sM22.Copy("M22_Jg");
		sM24.Copy("M24_Jg");
	}
	//绘制角钢简图时应适当调整螺栓绘制比例 wht 11-07-15
	double bolt_draw_scale=1.0;//1.2;有可能导致孔符叠压，感觉也没太大必要绘那大图符 wjh-2014.5.6
	if(pJgDraw->dimStyleFlag.IsHasFlag(CAngleDrawing::BriefDrawing))
		bolt_draw_scale=0.3;
	//绘制螺栓时使用螺栓图层 wht 11-06-01
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	for(i=0;i<n;i++)
	{
		pJgDraw->GetXLsCircle(Cir,i);
		if(Cir.radius<EPS)
			continue;	//非螺栓孔特征点,如楞线上的制弯点
		Cir.centre+=origin;
		//得到螺栓孔腰圆长度 画笔默认宽度为1 wht 10-02-25
		int waist_len=Cir.pen.width;
		if(waist_len>1)	
		{	//按2mm直径绘制腰圆孔 根据比例计算腰圆长度
			double waistLen=waist_len*(1.0/Cir.radius);
			Cir.radius=1;
			f3dPoint waist_vec=Cir.norm;	//腰圆方向
			if(NewWaistHole(pBlockTableRecord,Cir,waist_vec,waistLen))
			{	//标注螺栓的腰圆长度  wxc-2017.3.7
				CXhChar16 sText("%d",waist_len);
				if(waist_vec.x<0)	//保证腰圆孔方向与X轴方向相同
					waist_vec*=-1.0;
				double half_len=0.5*waistLen;
				f3dPoint dimStart=Cir.centre-waist_vec*half_len;
				f3dPoint dimEnd=Cir.centre+waist_vec*half_len;
				f3dPoint dimPos=(dimStart+dimEnd)*0.5;
				double angle=Cal2dLineAng(0,0,waist_vec.x,waist_vec.y);
				DimAngleSize(pBlockTableRecord,dimStart,dimEnd,dimPos,sText,DimStyleTable::dimStyle.dimStyleId,angle,sys.fonts.angle.fDimTextSize);
				continue;
			}
		}
		BOOL bRetCode=FALSE;
		if(Cir.pen.style==1)
		{	//脚钉图符特殊处理(使用脚钉图符) wxc 15-2-13
			if(fabs(2*Cir.radius-12)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-16)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-20)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-22)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-24)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24_N,bolt_draw_scale,0);
			else
			{
				CXhChar16 ls_str;
				ls_str.Printf("M%d",ftol(2*Cir.radius));
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,bolt_draw_scale,0);
			}
		}
		else if(Cir.pen.style>0)	
		{	//特殊孔图符应按孔径插入图符
			double hole_d=2*Cir.radius+Cir.feature*0.1;
			if(Cir.feature==15)
			{	//标准图符
				if(fabs(2*Cir.radius-12)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-16)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-18)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM18,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-20)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-22)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-24)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,bolt_draw_scale,0);
			}
			else
			{
				CXhChar16 ls_str;
				if(hole_d==(int)hole_d)	//MHXX_X.dwg图符是为了与普通螺栓图符MXX_XX.dwg相区别 wjh-2015.2.13
					ls_str.Printf("MH%d",ftol(hole_d));
				else
					ls_str.Printf("MH%d_%d",(int)hole_d,ftoi(hole_d*10)%10);
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,bolt_draw_scale,0);
				if(!bRetCode)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
			}
		}
		else
		{	//连接螺栓
			if(fabs(2*Cir.radius-12)<EPS)	//按3mm螺栓直径绘制
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-16)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-20)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-22)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-24)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,bolt_draw_scale,0);
			else
			{
				CXhChar16 ls_str;
				ls_str.Printf("M%d",ftol(2*Cir.radius));
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,bolt_draw_scale,0);
			}
		}
		if(!bRetCode&&fabs(Cir.radius)>EPS2)
		{	//绘制螺栓
			Cir.radius=1;
			NewBoltCircle(pBlockTableRecord,Cir,Cir.pen.style==1); //自绘制螺栓孔是同样支持绘制脚钉图符 wht 11-05-09
		}
	}
	n = pJgDraw->GetYLsCount();
	for(i=0;i<n;i++)
	{
		pJgDraw->GetYLsCircle(Cir,i);
		if(Cir.radius<EPS)
			continue;	//非螺栓孔特征点,如楞线上的制弯点
		Cir.centre+=origin;
		//得到螺栓孔腰圆长度 画笔默认宽度为1 wht 10-02-25
		int waist_len=Cir.pen.width;
		if(waist_len>1)	
		{	//按2mm直径绘制腰圆孔 根据比例计算腰圆长度
			double waistLen=waist_len*(1.0/Cir.radius);
			Cir.radius=1;
			f3dPoint waist_vec=Cir.norm;	//腰圆方向
			if(NewWaistHole(pBlockTableRecord,Cir,waist_vec,waistLen))
			{	//标注螺栓的腰圆长度  wxc-2017.3.7
				CXhChar16 sText("%d",waist_len);
				if(waist_vec.x<0)	//保证腰圆孔方向与X轴方向相同
					waist_vec*=-1.0;
				double half_len=0.5*waistLen;
				f3dPoint dimStart=Cir.centre-waist_vec*half_len;
				f3dPoint dimEnd=Cir.centre+waist_vec*half_len;
				f3dPoint dimPos=(dimStart+dimEnd)*0.5;
				double angle=Cal2dLineAng(0,0,waist_vec.x,waist_vec.y);
				DimAngleSize(pBlockTableRecord,dimStart,dimEnd,dimPos,sText,DimStyleTable::dimStyle.dimStyleId,angle,sys.fonts.angle.fDimTextSize);
				continue;
			}
		}
		BOOL bRetCode=FALSE;
		if(Cir.pen.style==1)
		{	//脚钉图符特殊处理(使用脚钉图符) wxc 15-2-13
			if(fabs(2*Cir.radius-12)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-16)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-20)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-22)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22_N,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-24)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24_N,bolt_draw_scale,0);
			else
			{
				CXhChar16 ls_str;
				ls_str.Printf("M%d",ftol(2*Cir.radius));
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,bolt_draw_scale,0);
			}
		}
		else if(Cir.pen.style>0)	
		{	//特殊孔图符应按孔径插入图符
			double hole_d=2*Cir.radius+Cir.feature*0.1;
			if(Cir.feature==15)
			{	//标准图符
				if(fabs(2*Cir.radius-12)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-16)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-18)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM18,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-20)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-22)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,bolt_draw_scale,0);
				else if(fabs(2*Cir.radius-24)<EPS)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,bolt_draw_scale,0);
			}
			else
			{
				CXhChar16 ls_str;
				if(hole_d==(int)hole_d)	//MHXX_X.dwg图符是为了与普通螺栓图符MXX_XX.dwg相区别 wjh-2015.2.13
					ls_str.Printf("MH%d",ftol(hole_d));
				else
					ls_str.Printf("MH%d_%d",(int)hole_d,ftoi(hole_d*10)%10);
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,bolt_draw_scale,0);
				if(!bRetCode)
					bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
			}
		}
		else
		{
			if(fabs(2*Cir.radius-12)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-16)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-20)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-22)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,bolt_draw_scale,0);
			else if(fabs(2*Cir.radius-24)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,bolt_draw_scale,0);
			else
			{
				CXhChar16 ls_str;
				ls_str.Printf("M%d",ftol(2*Cir.radius));
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,bolt_draw_scale,0);
			}
		}
		if(!bRetCode&&fabs(Cir.radius)>EPS2)
		{	//绘制螺栓
			Cir.radius=1;
			NewBoltCircle(pBlockTableRecord,Cir);
		}
	}
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	//标注螺栓相对距离
	CSizeTextDim jg_dim;
	n = pJgDraw->GetDimCount();
	CXhChar200 sDimText;
	AcDbObjectId dimStyleId,lsSpaceDimStyleId;
	if(bDimStyle2)
		dimStyleId = DimStyleTable::dimStyle2.dimStyleId;
	else
		dimStyleId = DimStyleTable::dimStyle.dimStyleId;
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	if(sys.part_map.angle.iLsSpaceDimStyle!=3)
	{
		CXhChar50 dimText;		
		for(i=0;i<n;i++)
		{
			pJgDraw->GetDimAt(i,&jg_dim);
			if(jg_dim.dist==0)
				continue;
			if(jg_dim.iTextDimSylte==1)	//角钢X肢螺栓的间距标注，文字需在标注线下侧 wxc-2016.8.5
				lsSpaceDimStyleId=DimStyleTable::dimStyle4.dimStyleId;
			else
				lsSpaceDimStyleId=dimStyleId;
			jg_dim.dimPos+=jg_dim.origin;
			jg_dim.dimStartPos+=jg_dim.origin;
			jg_dim.dimEndPos+=jg_dim.origin;
			jg_dim.dimPos+=origin;
			jg_dim.dimStartPos+=origin;
			jg_dim.dimEndPos+=origin;
			//jg_dim.nHits==0表示切角，jg_dim.nHits==-1表示切肢，>0表示螺栓间距
			if(sys.part_map.angle.iLsSpaceDimStyle==4&&jg_dim.nHits>0)	//不标注内容
				dimText.Copy(" ");
			else
				dimText.Copy(jg_dim.DimText());
			if((sys.part_map.angle.iLsSpaceDimStyle>=0&&jg_dim.nHits>0)
				||(sys.part_map.angle.bDimCutAngle&&jg_dim.nHits<=0))
			{
				DimAngleSize(pBlockTableRecord,
					jg_dim.dimStartPos,jg_dim.dimEndPos,
					jg_dim.dimPos,dimText,lsSpaceDimStyleId,jg_dim.angle,sys.fonts.angle.fDimTextSize);
			}
		}
	}
	double maxZhunPosY=-100000,minZhunPosY=100000;
	CZhunXianTextDim zhun_dim;
	n = pJgDraw->GetZhunDimCount();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(i=0;i<n;i++)
	{
		pJgDraw->GetZhunDimAt(i,&zhun_dim);
		zhun_dim.dimStartPos+=zhun_dim.origin;
		zhun_dim.dimEndPos+=zhun_dim.origin;
		zhun_dim.dimPos+=zhun_dim.origin;
		zhun_dim.dimPos+=origin;
		zhun_dim.dimStartPos+=origin;
		zhun_dim.dimEndPos+=origin;
		sDimText.Printf("%.0f",zhun_dim.dist);
		if(zhun_dim.dimStartPos!=zhun_dim.dimEndPos)
		{
			DimAngleSize(pBlockTableRecord,
				zhun_dim.dimStartPos,zhun_dim.dimEndPos,
				zhun_dim.dimPos,sDimText,dimStyleId,zhun_dim.angle,sys.fonts.angle.fDimTextSize);
		}
		if(zhun_dim.lineStart.y>maxZhunPosY)
			maxZhunPosY=zhun_dim.lineStart.y;
		if(zhun_dim.lineStart.y<minZhunPosY)
			minZhunPosY=zhun_dim.lineStart.y;

		zhun_dim.lineStart+=origin;
		zhun_dim.lineEnd+=origin;
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord,zhun_dim.lineStart,zhun_dim.lineEnd);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	}
	CTextOnlyDrawing pure_txt;
	n = pJgDraw->GetPureTxtCount();
	//绝对尺寸或者竖直标注的相对尺寸或其他文字标注
	for(i=0;i<n;i++)
	{
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(pJgDraw->GetPureTxtDimAt(i,&pure_txt))
		{
			pure_txt.dimPos+=pure_txt.origin;
			pure_txt.dimPos+=origin;
			if(!sys.part_map.angle.bDimLsAbsoluteDist&&abs(pure_txt.iOrder)==3)
				continue;	//不标注绝对尺寸
			if(sys.part_map.angle.iLsSpaceDimStyle==3&&(abs(pure_txt.iOrder)==1||abs(pure_txt.iOrder)==2))
				continue;	//不标注竖直相对尺寸(1.第一排螺栓相对尺寸 2.第二排螺栓相对尺寸) wht 11-05-07
			if(pure_txt.iOrder==0)
				DimText(pBlockTableRecord,pure_txt.dimPos,pure_txt.GetPrecisionDimText(),TextStyleTable::hzfs.textStyleId,pure_txt.textHeight,pure_txt.angle,AcDb::kTextLeft);
			else if(pure_txt.iOrder==4)
			{	//特殊螺栓标记说明
				GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
				DimText(pBlockTableRecord,pure_txt.dimPos,pure_txt.dimText,TextStyleTable::hzfs.textStyleId,pure_txt.textHeight,pure_txt.angle,AcDb::kTextLeft);
			}
			else if(pure_txt.bInXWing)
				DimText(pBlockTableRecord,pure_txt.dimPos,pure_txt.GetPrecisionDimText(),TextStyleTable::hzfs.textStyleId,pure_txt.textHeight,Pi/2.0,AcDb::kTextRight);
			else
				DimText(pBlockTableRecord,pure_txt.dimPos,pure_txt.GetPrecisionDimText(),TextStyleTable::hzfs.textStyleId,pure_txt.textHeight,Pi/2.0,AcDb::kTextLeft);
		}
	}
	//标注角钢肋板位置
	CSizeTextDim rib_dim;
	n = pJgDraw->GetRibPlateDimCount();
	GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
	if(sys.part_map.angle.bDimRibPlateSetUpPos)
	{
		for(i=0;i<n;i++)
		{
			pJgDraw->GetRibPlateDimAt(i,&rib_dim);
			f3dPoint dimPos=rib_dim.dimPos+rib_dim.origin+origin;
			f3dPoint dimStart=rib_dim.dimStartPos+rib_dim.origin+origin;
			f3dPoint dimEnd=rib_dim.dimEndPos+rib_dim.origin+origin;
			if(rib_dim.iTextDimSylte==1)	//角钢X肢螺栓的间距标注，文字需在标注线外侧 wxc-2016.8.5
				lsSpaceDimStyleId=DimStyleTable::dimStyle4.dimStyleId;
			else
				lsSpaceDimStyleId=dimStyleId;
			DimAngleSize(pBlockTableRecord,dimStart,dimEnd,dimPos,rib_dim.DimText(),lsSpaceDimStyleId,rib_dim.angle,sys.fonts.angle.fDimTextSize);
		}
	}
	//标注肋板编号
	CTextFrame data_point;
	n = pJgDraw->GetPartNoCount();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(sys.part_map.angle.bDimRibPlatePartNo)
	{	//标注角钢肋板编号
		double fTextSize=sys.fonts.angle.fPartNoTextSize;//sys.dim_map.PartNo.fPartNoTextSize;
		for(i=0;i<n;i++)
		{
			if(!pJgDraw->GetPartNoDimAt(i,&data_point))
				continue;
			//根据数据点位置标注构件编号
			if(strlen(data_point.Text())<=0)
				continue;
			double fHalfWidth=0.5*data_point.Width();
			double fHalfHeight=0.5*data_point.Height();
			f3dPoint data_pos=data_point.dataPos+origin;
			f3dPoint dim_pos=data_point.dimPos+origin;
			f3dPoint refEndPos=data_point.GetRefLineEndPos()+origin;
			//绘制引出线
			CreateAcadLine(pBlockTableRecord,data_pos,refEndPos);
			//标注肋板件号+厚度材质
			f3dPoint text_pos(dim_pos),refVec=dim_pos-data_pos;
			normalize(refVec);
			DimText(pBlockTableRecord,text_pos,data_point.Text(),TextStyleTable::hzfs.textStyleId,
				fTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
			text_pos+=refVec*(fHalfWidth+0.5*fTextSize);
			DimText(pBlockTableRecord,text_pos,data_point.Text2(),TextStyleTable::hzfs.textStyleId,
				fTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
			//生成文本框的框线
			if(data_point.Style()==0)
				CreateAcadCircle(pBlockTableRecord,dim_pos,fHalfWidth);
			else 
			{	
				f3dPoint topLeft(dim_pos.x-fHalfWidth,dim_pos.y+fHalfHeight);
				if(data_point.Style()==1)	//腰圆矩形框
					CreateAcadRect(pBlockTableRecord,topLeft,data_point.Width(),data_point.Height(),TRUE);
				else	//直角矩形框
					CreateAcadRect(pBlockTableRecord,topLeft,data_point.Width(),data_point.Height());
			}
		}
	}
	CKaiHeAngleDim kaihe_dim;
	n = pJgDraw->GetKaiHeDimCount();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	double dist=0;
	for(i=0;i<n;i++)
	{
		pJgDraw->GetKaiHeDimAt(i,&kaihe_dim);
		kaihe_dim.dimPos+=kaihe_dim.origin+origin;
		kaihe_dim.dimStartPos+=kaihe_dim.origin+origin;
		kaihe_dim.dimEndPos+=kaihe_dim.origin+origin;
		kaihe_dim.dimStyle.style=DimStyleTable::dimStyle;
		kaihe_dim.dimStyle.textHeight=sys.fonts.angle.fDimTextSize;//sys.dim_map.fDimTextSize;
		if(sys.part_map.angle.bDimKaiheSegLen)
		{	//标注开合角区域分段长度 wht 12-03-13
			if(kaihe_dim.bInWingX)
			{
				kaihe_dim.dimStartPos.y-=kaihe_dim.dimStyle.style.Gap;
				kaihe_dim.dimEndPos.y-=kaihe_dim.dimStyle.style.Gap;
				if(!sys.part_map.angle.bDimKaiheSumLen&&kaihe_dim.start0_end1_else2<2)
				{	//不标注开合角总长时，需要在将始末端的开合范围以分段形式标注出来
					sDimText.Printf("%.0f",kaihe_dim.dist);
					kaihe_dim.dimPos.x=(kaihe_dim.dimStartPos.x+kaihe_dim.dimEndPos.x)/2;
					f3dPoint dimPos=kaihe_dim.dimPos;
					dimPos.y+=(kaihe_dim.dimStyle.textHeight+kaihe_dim.dimStyle.style.Gap*2);
					//kaihe_dim.dimPos.y=kaihe_dim.dimStartPos.y+1;//+sys.dim_map.fDimTextSize+1;
					DimSize(pBlockTableRecord,kaihe_dim.dimStartPos,kaihe_dim.dimEndPos,dimPos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
				}
				else if(kaihe_dim.start0_end1_else2==2)	//不是两端开合角
				{
					BOOL bFindStartLs=FALSE;
					double dist[3]={0,0,0};
					f3dPoint mid_pos[2];
					int prevPureTextOfWingX=0;
					for(int j=0;j<pJgDraw->GetPureTxtCount();j++)
					{
						if(pJgDraw->GetPureTxtDimAt(j,&pure_txt))
						{
							if(pure_txt.iOrder!=3&&pure_txt.iOrder!=-3)
								continue;	//不是孔位标注 原用iOrder==2表示，现在2代表第二层螺栓相对尺寸，3代表绝对尺寸 wht 11-05-07
							if(pure_txt.bInXWing)
							{
								if(!bFindStartLs)
								{
									if(pure_txt.dist-kaihe_dim.real_start.x>=-PRECISION)
									{
										pure_txt.dimPos+=pure_txt.origin+origin;
										mid_pos[0]=pure_txt.dimPos;
										dist[0]=pure_txt.dist-kaihe_dim.real_start.x;
										bFindStartLs=TRUE;
										prevPureTextOfWingX=j;
										continue;
									}
								}
								else if(bFindStartLs)
								{
									if(pure_txt.dist-kaihe_dim.real_end.x>=-PRECISION)
									{
										pJgDraw->GetPureTxtDimAt(prevPureTextOfWingX,&pure_txt);
										pure_txt.dimPos+=pure_txt.origin+origin;
										mid_pos[1]=pure_txt.dimPos;
										dist[1]=pure_txt.dist-dist[0]-kaihe_dim.real_start.x;
										dist[2]=kaihe_dim.real_end.x-pure_txt.dist;
										break;
									}
								}
								prevPureTextOfWingX=j;
							}
						}
					}
					f3dPoint dimPos;
					kaihe_dim.dist=ftoi(dist[0])+ftoi(dist[1])+ftoi(dist[2]);
					if(dist[0]>0)
					{
						sDimText.Printf("%.0f",dist[0]);
						dimPos.x=kaihe_dim.dimStartPos.x-1;
						mid_pos[0].y=mid_pos[1].y=kaihe_dim.dimStartPos.y;
						dimPos.y=kaihe_dim.dimPos.y+kaihe_dim.dimStyle.textHeight+kaihe_dim.dimStyle.style.Gap*2;
						double dimtextlen=0.7*kaihe_dim.dimStyle.textHeight*sDimText.GetLength();
						if(kaihe_dim.dimStartPos.x+dimtextlen>mid_pos[0].x)
							kaihe_dim.dimStartPos.x=mid_pos[0].x-dimtextlen;
						DimSize(pBlockTableRecord,
							kaihe_dim.dimStartPos,mid_pos[0],dimPos,sDimText,DimStyleTable::dimStyle.dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
					if(dist[1]>EPS)
					{
						sDimText.Printf("%.0f",dist[1]);
						dimPos.x=(mid_pos[0].x+mid_pos[1].x)/2;
						DimSize(pBlockTableRecord,
							mid_pos[0],mid_pos[1],dimPos,sDimText,DimStyleTable::dimStyle.dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
					if(dist[2]>0)
					{
						sDimText.Printf("%.0f",dist[2]);
						dimPos.x=mid_pos[1].x+1;
						double dimtextlen=0.7*kaihe_dim.dimStyle.textHeight*sDimText.GetLength();
						if(kaihe_dim.dimEndPos.x<mid_pos[1].x+dimtextlen)
							kaihe_dim.dimEndPos.x=mid_pos[1].x+dimtextlen;
						DimSize(pBlockTableRecord,
							mid_pos[1],kaihe_dim.dimEndPos,dimPos,sDimText,DimStyleTable::dimStyle.dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
				}
			}
			else
			{
				kaihe_dim.dimStartPos.y+=kaihe_dim.dimStyle.style.Gap;
				kaihe_dim.dimEndPos.y+=kaihe_dim.dimStyle.style.Gap;
				if(!sys.part_map.angle.bDimKaiheSumLen&&kaihe_dim.start0_end1_else2<2)
				{	//不标注开合角总长时，需要在将始末端的开合范围以分段形式标注出来
					sDimText.Printf("%.0f",kaihe_dim.dist);
					kaihe_dim.dimPos.x=(kaihe_dim.dimStartPos.x+kaihe_dim.dimEndPos.x)/2;
					f3dPoint dimPos=kaihe_dim.dimPos;
					dimPos.y-=2*kaihe_dim.dimStyle.textHeight+3*kaihe_dim.dimStyle.style.Gap;
					//kaihe_dim.dimPos.y=kaihe_dim.dimStartPos.y+1;//+sys.dim_map.fDimTextSize+1;
					DimSize(pBlockTableRecord,kaihe_dim.dimStartPos,kaihe_dim.dimEndPos,dimPos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
				}
				if(kaihe_dim.start0_end1_else2==2)	//不是两端开合角
				{
					BOOL bFindStartLs=FALSE;
					double dist[3]={0,0,0};
					f3dPoint mid_pos[2];
					int prevPureTextOfWingY=0;
					for(int j=0;j<pJgDraw->GetPureTxtCount();j++)
					{
						if(pJgDraw->GetPureTxtDimAt(j,&pure_txt))
						{
							if(pure_txt.iOrder!=3&&pure_txt.iOrder!=-3)
								continue;	//不是孔位标注 原用iOrder==2表示，现在2代表第二层螺栓相对尺寸，3代表绝对尺寸 wht 11-05-07
							if(!pure_txt.bInXWing)
							{
								if(!bFindStartLs)
								{
									if(pure_txt.dist-kaihe_dim.real_start.x>=-PRECISION)
									{
										pure_txt.dimPos+=pure_txt.origin+origin;
										mid_pos[0]=pure_txt.dimPos;
										dist[0]=pure_txt.dist-kaihe_dim.real_start.x;
										bFindStartLs=TRUE;
										prevPureTextOfWingY=j;
										continue;
									}
								}
								else if(bFindStartLs)
								{
									if(pure_txt.dist-kaihe_dim.real_end.x>=-PRECISION)
									{
										pJgDraw->GetPureTxtDimAt(prevPureTextOfWingY,&pure_txt);
										pure_txt.dimPos+=pure_txt.origin+origin;
										mid_pos[1]=pure_txt.dimPos;
										dist[1]=pure_txt.dist-dist[0]-kaihe_dim.real_start.x;
										dist[2]=kaihe_dim.real_end.x-pure_txt.dist;
										break;
									}
								}
								prevPureTextOfWingY=j;
							}
						}
					}
					f3dPoint dimPos;
					kaihe_dim.dist=ftoi(dist[0])+ftoi(dist[1])+ftoi(dist[2]);
					if(dist[0]>0)
					{
						sDimText.Printf("%.0f",dist[0]);
						dimPos.x=kaihe_dim.dimStartPos.x-1;
						mid_pos[0].y=mid_pos[1].y=kaihe_dim.dimStartPos.y;
						dimPos.y=kaihe_dim.dimPos.y-2*kaihe_dim.dimStyle.textHeight-3*kaihe_dim.dimStyle.style.Gap;
						//if(sys.part_map.angle.bDimKaiheAngle)	
						//	dimPos.y-=kaihe_dim.dimStyle.textHeight;
						double dimtextlen=0.7*kaihe_dim.dimStyle.textHeight*sDimText.GetLength();
						if(kaihe_dim.dimStartPos.x+dimtextlen>mid_pos[0].x)
							kaihe_dim.dimStartPos.x=mid_pos[0].x-dimtextlen;
						DimSize(pBlockTableRecord,
							kaihe_dim.dimStartPos,mid_pos[0],dimPos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
					if(dist[1]>EPS)
					{
						sDimText.Printf("%.0f",dist[1]);
						dimPos.x=(mid_pos[0].x+mid_pos[1].x)/2;
						DimSize(pBlockTableRecord,
							mid_pos[0],mid_pos[1],dimPos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
					if(dist[2]>0)
					{
						sDimText.Printf("%.0f",dist[2]);
						dimPos.x=mid_pos[1].x+1;
						double dimtextlen=0.7*kaihe_dim.dimStyle.textHeight*sDimText.GetLength();
						if(kaihe_dim.dimEndPos.x<mid_pos[1].x+dimtextlen)
							kaihe_dim.dimEndPos.x=mid_pos[1].x+dimtextlen;
						DimSize(pBlockTableRecord,
							mid_pos[1],kaihe_dim.dimEndPos,dimPos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
				}
			}
		}
		kaihe_dim.dimPos.x=(kaihe_dim.dimStartPos.x+kaihe_dim.dimEndPos.x)/2.0;
		if(sys.part_map.angle.bDimKaiheAngle)
		{	//标注开合角度数 wht 12-03-13
			DimText(pBlockTableRecord,kaihe_dim.dimPos-f3dPoint(0,kaihe_dim.dimStyle.style.Gap),kaihe_dim.dimText,
				TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
		}
		dist = kaihe_dim.dimEndPos.x-kaihe_dim.dimStartPos.x;
		if(sys.part_map.angle.bDimKaiheSumLen&&dist>EPS2)
		{	//标注开合角区域总长度 wht 12-03-13
			if(strstr(kaihe_dim.dimText,"+"))
				sDimText.Printf("开角%.0f",kaihe_dim.dist);
			else if(strstr(kaihe_dim.dimText,"-"))
				sDimText.Printf("合角%.0f",kaihe_dim.dist);
			else
				sDimText.Printf("%.0f",kaihe_dim.dist);
			kaihe_dim.dimPos.x=(kaihe_dim.dimStartPos.x+kaihe_dim.dimEndPos.x)/2;
			//kaihe_dim.dimPos.y=kaihe_dim.dimStartPos.y+1;//+sys.dim_map.fDimTextSize+1;
			DimSize(pBlockTableRecord,
				kaihe_dim.dimStartPos,kaihe_dim.dimEndPos,
				kaihe_dim.dimPos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		}
		if(sys.part_map.angle.bDimKaiheScopeMap)
		{	//标注开合角区域标识符 wht 12-03-13
			double dimPosY=origin.y;
			if(kaihe_dim.bInWingX)
				dimPosY += minZhunPosY;
			else 
				dimPosY += maxZhunPosY;
			f3dPoint dim_pos=kaihe_dim.dimStartPos;
			dim_pos.y=dimPosY;
			InsertBlock(pBlockTableRecord,dim_pos,"dollar",1,0);
			dim_pos=kaihe_dim.dimEndPos;
			dim_pos.y=dimPosY;
			InsertBlock(pBlockTableRecord,dim_pos,"dollar",1,0);	
		}
	}
	if(pExterBlockTableRecord==NULL)
		pBlockTableRecord->close();
	return TRUE;
}
static CXhChar200 GetNoteByAngle(CProcessPart *pJgTypePart )
{
	CXhChar200 notes1,ls_str;
	CXhChar100 notes;
	notes.Copy("备注");
	if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		((CLDSLineAngle*)pJgTypePart->pPart)->GetLsRefList()->GetLsString(ls_str);
		((CLDSLineAngle*)pJgTypePart->pPart)->GetProcessStr(notes,99);
		if(strlen(pJgTypePart->pPart->sNotes)>0)
		{
			notes.Append(",");
			notes.Append(pJgTypePart->pPart->sNotes);
		}
	}
	else if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pJgTypePart->pPart;
		CLsRefList lsRefList;
		CLDSLinePart *pLinePart=NULL;
		for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
		{
			if(strlen(pLinePart->sNotes)>0)
			{
				notes.Append(",");
				notes.Append(pLinePart->sNotes);
			}
			CLsRef *pLsRef=NULL;
			for(pLsRef=pLinePart->GetFirstLsRef();pLsRef;pLsRef=pLinePart->GetNextLsRef())
				lsRefList.append(*pLsRef);
		}
		lsRefList.GetLsString(ls_str);
	}
	else if(pJgTypePart->pPart==NULL)
	{
		if(pJgTypePart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
		{
			((CProcessAngle*)pJgTypePart)->GetLsString(ls_str);
			((CProcessAngle*)pJgTypePart)->GetProcessStr(notes,99);
			if(strlen(((CProcessAngle*)pJgTypePart)->GetNotes())>0)
			{
				notes.Append(",");
				notes.Append(pJgTypePart->GetNotes());
			}
		}
	}
	notes1.Append(notes);
	return notes1;
}

 static void DrawMapAccordingToJGType(CProcessPart*pJgTypePart, f2dRect draw_rect)
{
	f3dPoint origin;
	if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
	{
		origin.x = draw_rect.topLeft.x;
		origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
		if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CLDSLineAngle*)pJgTypePart->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
		else if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CPolyAngleDrawing xPolyJgDraw;
			xPolyJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);	
			xPolyJgDraw.CreatePolyAngleDrawing((CLDSPolyPart*)pJgTypePart->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);	
			DrawJgMap(&xPolyJgDraw,origin);
		}
		if (pJgTypePart->pPart==NULL)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CProcessAngle*)pJgTypePart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
	}
}

int DrawJgTypeMapInOnePaper(ATOM_LIST<CProcessPart *>& partList,BOOL bVertical,int nPageNum,int iOrder)
{
	//获得当前图形的块表指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	f3dPoint origin;	//原点坐标为(0,0,0)
	//1.绘制页码
	CFormatTbl xHeaderTbl;
	int nColumnWideArr[3]={7,7,7};
	int nRowHighArr[8]={13,18,20,31,11,32,20,20};
	int nRow=bVertical?3:8;
	int nColumn=bVertical?8:3;
	xHeaderTbl.Create(nRow,nColumn);
	int *pTableColumnWideArr=bVertical?nRowHighArr:nColumnWideArr;
	int *pTableRowHighArr=bVertical?nColumnWideArr:nRowHighArr;
	for(int i=0;i<nColumn;i++)
		xHeaderTbl.SetColumnWide(i,pTableColumnWideArr[i]);
	for(int j=0;j<nRow;j++)
		xHeaderTbl.SetRowHigh(j,pTableRowHighArr[j]);
	CXhChar100 page_str,page_str2;
	CTime t = CTime::GetCurrentTime();
	CXhChar100 time_str;
	time_str.Printf("%s",t.Format("%y %m %d"));
	if(bVertical) //竖着使用纸张 
	{
		xHeaderTbl.GetGridAt(2,4)->m_bVGridVirtual = TRUE;
		for(int i=5;i<8;i++)
		{
			xHeaderTbl.GetGridAt(2,i)->m_bVGridVirtual = TRUE;
			xHeaderTbl.GetGridAt(2,i)->m_bHGridVirtual = TRUE;
			xHeaderTbl.GetGridAt(1,i)->m_bHGridVirtual = TRUE;
		}
		xHeaderTbl.SetLocateStyle(BOTTOM_LEFT);
		xHeaderTbl.SetBasePos(f2dPoint(origin.x,origin.y));
		//设置表格内容
		for(i=0;i<8;i++)
		{
			for(int j=0;j<3;j++)
			{
				CGrid *pGrid=xHeaderTbl.GetGridAt(j,i);
				pGrid->feature = TEXTALIGN_MIDDLE_CENTER;
				pGrid->textsize = sys.dim_map.fCommonTextSize;
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		page_str.Printf("Total %d pages",nPageNum);
		page_str2.Printf("No. %d page",iOrder+1);
		CXhChar100 strArr[50] ={"Operator",Ta.m_sCritic,"Project Name",Ta.m_sPrjName,"Tower Type",Ta.m_sTaType,page_str,page_str2,
								"Critic",Ta.m_sOperator,"Drawing number",Ta.m_sTaStampNo,"Auditor",Ta.m_sAuditor,"Date",time_str};
#else
		page_str.Printf("共 %d 页",nPageNum);
		page_str2.Printf("第 %d 页",iOrder+1);
		CXhChar100 strArr[50] ={"审核",Ta.m_sCritic,"工程名称",Ta.m_sPrjName,"塔型",Ta.m_sTaType,page_str,page_str2,
			                     "评审",Ta.m_sOperator,"图  号",Ta.m_sTaStampNo,"制图",Ta.m_sAuditor,"日  期",time_str};
#endif
		int nDataUnm=0;
		for (int i=0;i<nRow;i++)
		{
			for (int j=0;j<nColumn;j++)
			{
				xHeaderTbl.SetGrid(i,j,(char*)strArr[nDataUnm]);
				nDataUnm++;
			    if((i==1||i==2)&&j==3) //跳过没有单元格
					break;
			}
		}
	}
	else
	{
		xHeaderTbl.GetGridAt(4,1)->m_bHGridVirtual = TRUE;
		for(int i=5;i<8;i++)
		{
			xHeaderTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
			xHeaderTbl.GetGridAt(i,1)->m_bHGridVirtual = TRUE;
			xHeaderTbl.GetGridAt(i,1)->m_bVGridVirtual = TRUE;
		}
		xHeaderTbl.SetLocateStyle(BOTTOM_LEFT);
		xHeaderTbl.SetBasePos(f2dPoint(origin.x,origin.y));
		//设置表格内容
		for(i=0;i<8;i++)
		{
			for(int j=0;j<3;j++)
			{
				CGrid *pGrid=xHeaderTbl.GetGridAt(i,j);
				pGrid->feature = TEXTALIGN_MIDDLE_CENTER;
				pGrid->text_angle = -Pi/2;
				pGrid->textsize = sys.dim_map.fCommonTextSize;
			}
		}
#ifdef AFX_TARG_ENU_ENGLISH
		page_str.Printf("Total %d pages",nPageNum);
		page_str2.Printf("No. %d page",iOrder+1);
		CXhChar100 strArr[50] ={"Operator",Ta.m_sCritic,"Project Name",Ta.m_sPrjName,"Tower Type",Ta.m_sTaType,page_str,page_str2,
			"Critic",Ta.m_sOperator,"Drawing number",Ta.m_sTaStampNo,"Auditor",Ta.m_sAuditor,"Date",time_str};
#else
		page_str.Printf("共 %d 页",nPageNum);
		page_str2.Printf("第 %d 页",iOrder+1);
		CXhChar100 strArr[50] ={"审核",Ta.m_sCritic,"工程名称",Ta.m_sPrjName,"塔型",Ta.m_sTaType,page_str,page_str2,
			"评审",Ta.m_sOperator,"图  号",Ta.m_sTaStampNo,"制图",Ta.m_sAuditor,"日  期",time_str};
#endif
		int nDataUnm=0;
		for (int i=nColumn-1;i>=0;i--)
		{
			for (int j=0;j<nRow;j++)
			{
				if((j==4||j==5||j==6||j==7)&&(i==0||i==1)) 
					break;
				xHeaderTbl.SetGrid(j,i,(char*)strArr[nDataUnm]);
				nDataUnm++;
			}
		}
	}
	DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
	//2. 绘制构件图
	int	nPartCoun = partList.GetNodeNum();//210*297
	int nA4PaperWidth = 165,nA4PaperHeigith=250; //A4 纸张的标准大小 css 2016-4-7
	CFormatTbl xJGcard;
	//设置每个构件所站的高度，其中包括{一行字，一行构件图}
	nRow=nPartCoun*2, nColumn =4;
	xJGcard.Create(nRow,nColumn);
	double fWordHeight=8,fPartMapHeight=0,fMapHeight=bVertical?((nA4PaperHeigith-21)*1.0)/nPartCoun:(nA4PaperWidth*1.0)/nPartCoun;
	fPartMapHeight=fMapHeight-fWordHeight;
	for(int i=1;i<=nPartCoun;i++)
	{
		xJGcard.SetRowHigh((i-1)*2,fWordHeight);
		xJGcard.SetRowHigh(i*2-1,fPartMapHeight);
	}
	double fVerticalTableColumnArr[4]={20,40,40,65};//表竖着
	double fEndwaysTableColumnArr[4]={20,44.5,34.5,120};//表横着
	double *fTableColumn=bVertical?fVerticalTableColumnArr:fEndwaysTableColumnArr;
	for(int i=0;i<nColumn;i++)
		xJGcard.SetColumnWide(i,fTableColumn[i]);
	//设置字体大小 居中方式
	for (int i=0;i<nRow;i++)
	{
		for (int j=0;j<nColumn;j++)
		{
			CGrid *pGrid=xHeaderTbl.GetGridAt(i,j);
			pGrid->feature = TEXTALIGN_MIDDLE_CENTER;
			pGrid->textsize = sys.dim_map.fCommonTextSize;
		}
	}
	// 设置虚拟单元格
	for (int i =1;i<nRow;i++)
	{
		for (int j=1;j<nColumn;j++)
		{
			if (i%2>0)
				xJGcard.GetGridAt(i,j)->m_bHGridVirtual = TRUE;
		}
	}
	// 向单元格中写数据
	nRow =0; 
	CXhChar100 guige;
	for (CProcessPart **ppPart=partList.GetFirst();ppPart;ppPart=partList.GetNext())
	{
		for (int j=0;j<nColumn;j++)
			xJGcard.GetGridAt(nRow*2,j)->feature =TEXTALIGN_LEFT_CENTER;
		if((*ppPart)==NULL)
			continue;
		xJGcard.SetGrid(nRow*2,0,GetPartNoIncMat((*ppPart)->GetPartNo(),(*ppPart)->cMaterial));
		guige.Printf("L%.0fX%.0f",(*ppPart)->GetWidth(),(*ppPart)->GetThick());
		CXhChar16 steelmark;
		QuerySteelMatMark((*ppPart)->cMaterial,steelmark);
		guige.Printf("%sL%.0fX%.0fX%d %d件",(char*)steelmark,(*ppPart)->GetWidth(),(*ppPart)->GetThick(),(*ppPart)->GetLength(),nRow);
		xJGcard.SetGrid(nRow*2,1,guige);
		xJGcard.SetGrid(nRow*2,3,GetNoteByAngle(*ppPart));
		nRow++;
	}
	f2dPoint pos;
	xHeaderTbl.GetRowLineEnd(0,pos);
	bVertical?xJGcard.SetLocateStyle(BOTTOM_RIGHT):xJGcard.SetLocateStyle(TOP_LEFT);
	xJGcard.SetBasePos(pos);
	DrawTable(pBlockTableRecord,&xJGcard,FALSE);
	pBlockTableRecord->close();
	
	// 向表中画构件图
	nRow=1;
	for (CProcessPart **ppPart=partList.GetFirst();ppPart;ppPart=partList.GetNext())
	{
		if((*ppPart)==NULL)
			continue;
		f2dRect draw_rect;
		xJGcard.GetGridRect(nRow,0,draw_rect);
		DrawMapAccordingToJGType(*ppPart,draw_rect);
		nRow+=2;
	}
	return partList.GetNodeNum();
}
int DrawMultiPartInOnePaper(CProcessPart *pJgTypePart,CProcessPart *pJgTypePart2,int nHits,int nHits2,f3dPoint origin)
{
	int i=0,j=0;
	CXhChar100 sPartNo,guige,guige2,ls_str,ls_str2;
	CXhChar16 steelmark;
	//刘伟说宝鸡一卡两件中,Q235材质也需要正常标识 wjh-2017.11.1
	//if(pJgTypePart->cMaterial!=CSteelMatLibrary::Q235BriefMark())
	QuerySteelMatMark(pJgTypePart->cMaterial,steelmark);
#ifdef AFX_TARG_ENU_ENGLISH
	guige.Printf("%sL%.0fX%.0fX%d %d piece",(char*)steelmark,pJgTypePart->GetWidth(),pJgTypePart->GetThick(),pJgTypePart->GetLength(),nHits);
#else
	guige.Printf("%sL%.0fX%.0fX%d %d件",(char*)steelmark,pJgTypePart->GetWidth(),pJgTypePart->GetThick(),pJgTypePart->GetLength(),nHits);
#endif
	if(pJgTypePart2)
	{
		//if(pJgTypePart2->cMaterial!=CSteelMatLibrary::Q235BriefMark())
		QuerySteelMatMark(pJgTypePart2->cMaterial,steelmark);
		//else
		//	steelmark.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
		guige2.Printf("%sL%.0fX%.0fX%d %d piece",(char*)steelmark,pJgTypePart2->GetWidth(),pJgTypePart2->GetThick(),pJgTypePart2->GetLength(),nHits2);
#else
		guige2.Printf("%sL%.0fX%.0fX%d %d件",(char*)steelmark,pJgTypePart2->GetWidth(),pJgTypePart2->GetThick(),pJgTypePart2->GetLength(),nHits2);
#endif
	}
	CFlexTbl xJgProcessCard;
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表

	CFormatTbl xHeaderTbl,xTwoJgCard;
	xHeaderTbl.Create(8,3);
	xTwoJgCard.Create(4,4);
	xHeaderTbl.SetColumnWide(0,7);
	xHeaderTbl.SetColumnWide(1,7);
	xHeaderTbl.SetColumnWide(2,7);
	xHeaderTbl.SetRowHigh(0,13);
	xHeaderTbl.SetRowHigh(1,18);
	xHeaderTbl.SetRowHigh(2,20);
	xHeaderTbl.SetRowHigh(3,31);
	xHeaderTbl.SetRowHigh(4,11);
	xHeaderTbl.SetRowHigh(5,32);
	xHeaderTbl.SetRowHigh(6,20);
	xHeaderTbl.SetRowHigh(7,20);
	xHeaderTbl.GetGridAt(4,1)->m_bHGridVirtual = TRUE;
	for(i=5;i<8;i++)
	{
		xHeaderTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
		xHeaderTbl.GetGridAt(i,1)->m_bHGridVirtual = TRUE;
		xHeaderTbl.GetGridAt(i,1)->m_bVGridVirtual = TRUE;
	}
	xHeaderTbl.SetLocateStyle(BOTTOM_LEFT);
	xHeaderTbl.SetBasePos(f2dPoint(origin.x,origin.y));
	//设置表格内容
	for(i=0;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			CGrid *pGrid=xHeaderTbl.GetGridAt(i,j);
			pGrid->feature = TEXTALIGN_MIDDLE_CENTER;
			pGrid->text_angle = -Pi/2;
			pGrid->textsize = sys.fonts.angle.fCommonTextSize;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	xHeaderTbl.SetGrid(0,0,"Operator");
	xHeaderTbl.SetGrid(1,0,Ta.m_sOperator);
	xHeaderTbl.SetGrid(0,1,"Critic");
	xHeaderTbl.SetGrid(1,1,Ta.m_sCritic);
	xHeaderTbl.SetGrid(0,2,"Auditor");
	xHeaderTbl.SetGrid(1,2,Ta.m_sAuditor);
	xHeaderTbl.SetGrid(2,0,"Date");
	xHeaderTbl.SetGrid(2,1,"Drawing number");
	xHeaderTbl.SetGrid(2,2,"Project Name");
	xHeaderTbl.SetGrid(4,2,"Tower Type");
	CXhChar100 page_str;
	//根据刘伟要求一卡多件工艺卡中不需要页码 wxc-2017.9.12
	//page_str.Printf("Total %d pages",nPageNum);
	page_str.Copy("Total      pages");
	xHeaderTbl.SetGrid(6,2,page_str);
	//if (iPage!=-1)
	//	page_str.Printf("No. %d page",iPage+1);
	//else
	//	page_str.Printf("No. %d page",iOrder+1);
	page_str.Copy("No.      pages");
#else
	xHeaderTbl.SetGrid(0,0,"制图");
	xHeaderTbl.SetGrid(1,0,Ta.m_sOperator);
	xHeaderTbl.SetGrid(0,1,"评审");
	xHeaderTbl.SetGrid(1,1,Ta.m_sCritic);
	xHeaderTbl.SetGrid(0,2,"审核");
	xHeaderTbl.SetGrid(1,2,Ta.m_sAuditor);
	xHeaderTbl.SetGrid(2,0,"日  期");
	xHeaderTbl.SetGrid(2,1,"图  号");
	xHeaderTbl.SetGrid(2,2,"工程名称");
	xHeaderTbl.SetGrid(4,2,"塔型");
	CXhChar100 page_str;
	//根据刘伟要求一卡多件工艺卡中不需要页码 wxc-2017.9.12
	//page_str.Printf("共 %d 页",nPageNum);
	page_str.Copy("共      页");
	xHeaderTbl.SetGrid(6,2,page_str);
	//if(iPage!=-1)
	//	page_str.Printf("第 %d 页",iPage+1);
	//else
	//	page_str.Printf("第 %d 页",iOrder+1);
	page_str.Copy("第      页");
#endif
	xHeaderTbl.SetGrid(7,2,page_str);
	xHeaderTbl.SetGrid(3,2,Ta.m_sPrjName);
	xHeaderTbl.SetGrid(5,2,Ta.m_sTaType);
	CTime t = CTime::GetCurrentTime();
	CXhChar100 time_str;
	time_str.Printf("%s",t.Format("%y %m %d"));
	xHeaderTbl.SetGrid(3,0,time_str);
	xHeaderTbl.SetGrid(3,1,Ta.m_sTaStampNo);
	xHeaderTbl.SetGrid(4,0,sys.m_sCompanyName);
	xHeaderTbl.GetGridAt(4,0)->textsize = sys.dim_map.fHeaderTextSize;
	DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
	if(sys.general_set.bHasFrame)
		InsertBlock(pBlockTableRecord,origin,"frame",1,0);
	xTwoJgCard.SetColumnWide(0,19);
	xTwoJgCard.SetColumnWide(1,60);
	xTwoJgCard.SetColumnWide(2,60);
	xTwoJgCard.SetColumnWide(3,90);
	xTwoJgCard.SetRowHigh(0,5);
	xTwoJgCard.SetRowHigh(1,77.5);
	xTwoJgCard.SetRowHigh(2,5);
	xTwoJgCard.SetRowHigh(3,77.5);
	f2dPoint pos;
	xHeaderTbl.GetRowLineEnd(0,pos);
	xTwoJgCard.SetLocateStyle(TOP_LEFT);
	xTwoJgCard.SetBasePos(pos);
	for(int j=1;j<4;j++)
	{
		xTwoJgCard.GetGridAt(1,j)->m_bHGridVirtual = TRUE;
		xTwoJgCard.GetGridAt(3,j)->m_bHGridVirtual = TRUE;
	}

	xTwoJgCard.SetGrid(0,0,GetPartNoIncMat(pJgTypePart->GetPartNo(),pJgTypePart->cMaterial));
	if(pJgTypePart2)
		xTwoJgCard.SetGrid(2,0,GetPartNoIncMat(pJgTypePart2->GetPartNo(),pJgTypePart2->cMaterial));
	xTwoJgCard.GetGridAt(0,0)->feature = TEXTALIGN_MIDDLE_CENTER;	//刘伟反馈宝鸡要求，一卡多件件号居中显示 wht 18-11-01
	xTwoJgCard.GetGridAt(0,1)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(0,2)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(0,3)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(2,0)->feature = TEXTALIGN_MIDDLE_CENTER;
	xTwoJgCard.GetGridAt(2,1)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(2,2)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(2,3)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.SetGrid(0,1,guige);
	xTwoJgCard.SetGrid(2,1,guige2);
	CXhChar200 notes1,notes2;
	CXhChar100 notes;
#ifdef AFX_TARG_ENU_ENGLISH
	notes1.Copy("notes：");
	notes2.Copy("notes：");
#else
	notes1.Copy("备注：");
	notes2.Copy("备注：");
#endif
	if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		((CLDSLineAngle*)pJgTypePart->pPart)->GetLsRefList()->GetLsString(ls_str);
		((CLDSLineAngle*)pJgTypePart->pPart)->GetProcessStr(notes,100,false,true);
		if(strlen(pJgTypePart->pPart->sNotes)>0)
		{
			notes.Append(",");
			notes.Append(pJgTypePart->pPart->sNotes);
		}
	}
	else if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pJgTypePart->pPart;
		CLsRefList lsRefList;
		CLDSLinePart *pLinePart=NULL;
		for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
		{
			if(strlen(pLinePart->sNotes)>0)
			{
				notes.Append(",");
				notes.Append(pLinePart->sNotes);
			}
			CLsRef *pLsRef=NULL;
			for(pLsRef=pLinePart->GetFirstLsRef();pLsRef;pLsRef=pLinePart->GetNextLsRef())
				lsRefList.append(*pLsRef);
		}
		lsRefList.GetLsString(ls_str);
	}
	else if(pJgTypePart->pPart==NULL)
	{
		if(pJgTypePart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
		{
			((CProcessAngle*)pJgTypePart)->GetLsString(ls_str);
			((CProcessAngle*)pJgTypePart)->GetProcessStr(notes,100,false,true);
			if(strlen(((CProcessAngle*)pJgTypePart)->GetNotes())>0)
			{
				notes.Append(",");
				notes.Append(pJgTypePart->GetNotes());
			}
		}
	}
	notes1.Append(notes);
	notes.Empty();	//清空上一个构件的备注内容，防止与下一个构件备注重叠
	if(pJgTypePart2)
	{	
		if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			((CLDSLineAngle*)pJgTypePart2->pPart)->GetLsRefList()->GetLsString(ls_str2);
			((CLDSLineAngle*)pJgTypePart2->pPart)->GetProcessStr(notes,100,false,true);
		}
		else if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pJgTypePart2->pPart;
			CLsRefList lsRefList;
			CLDSLinePart *pLinePart=NULL;
			for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
			{
				if(strlen(pLinePart->sNotes)>0)
				{
					notes.Append(",");
					notes.Append(pLinePart->sNotes);
				}
				CLsRef *pLsRef=NULL;
				for(pLsRef=pLinePart->GetFirstLsRef();pLsRef;pLsRef=pLinePart->GetNextLsRef())
					lsRefList.append(*pLsRef);
			}
			lsRefList.GetLsString(ls_str2);
		}
		else if (pJgTypePart2==NULL)
		{
			if (pJgTypePart2->m_cPartType==CProcessPart::TYPE_LINEANGLE)
			{
				((CProcessAngle*)pJgTypePart2)->GetLsString(ls_str);
				((CProcessAngle*)pJgTypePart2)->GetProcessStr(notes,100,false,true);
				if(strlen(((CProcessAngle*)pJgTypePart2)->GetNotes())>0)
				{
					notes.Append(",");
					notes.Append(pJgTypePart2->GetNotes());
				}
			}
		}
		notes2.Append(notes);
	}
	xTwoJgCard.SetGrid(0,2,ls_str);
	xTwoJgCard.SetGrid(2,2,ls_str2);
	//一卡多件原本是专为宝鸡塔厂绘制,现据刘伟说根据他们的要求,切角切肢时,一律显示切角 wjh-2017.10.31
	if(strstr(notes1,"切角")!=NULL)
		notes1.Replace("切肢","");
	else
		notes1.Replace("切肢","切角");
	if(strstr(notes2,"切角")!=NULL)
		notes2.Replace("切肢","");
	else
		notes2.Replace("切肢","切角");
	xTwoJgCard.SetGrid(0,3,notes1);
	xTwoJgCard.SetGrid(2,3,notes2);
	for(int i=0;i<4;i++)
	{
		xTwoJgCard.GetGridAt(0,i)->textsize=sys.fonts.angle.fNumericTextSize;//dim_map.fCommonTextSize;
		xTwoJgCard.GetGridAt(2,i)->textsize=sys.fonts.angle.fNumericTextSize;//dim_map.fCommonTextSize;
	}
	DrawTable(pBlockTableRecord,&xTwoJgCard,FALSE);
	pBlockTableRecord->close();

	f2dRect draw_rect,draw_rect2;
	xTwoJgCard.GetGridRect(1,0,draw_rect);
	xTwoJgCard.GetGridRect(3,0,draw_rect2);
	if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
	{
		origin.x = draw_rect.topLeft.x;
		origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
		if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CLDSLineAngle*)pJgTypePart->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
		else if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CPolyAngleDrawing xPolyJgDraw;
			xPolyJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);	
			xPolyJgDraw.CreatePolyAngleDrawing((CLDSPolyPart*)pJgTypePart->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);	
			DrawJgMap(&xPolyJgDraw,origin);
		}
		if (pJgTypePart->pPart==NULL)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CProcessAngle*)pJgTypePart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
	}
	if(pJgTypePart2&&draw_rect2.Width()>EPS2&&draw_rect2.Height()>EPS2)
	{
		origin.x = draw_rect2.topLeft.x;
		origin.y = (draw_rect2.topLeft.y+draw_rect2.bottomRight.y)/2;
		if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLineAngleDrawing xJgDraw2;
			xJgDraw2.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw2.CreateLineAngleDrawing((CLDSLineAngle*)pJgTypePart2->pPart,draw_rect2.Width(),draw_rect2.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw2,origin);
		}
		else if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CPolyAngleDrawing xPolyJgDraw2;
			xPolyJgDraw2.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xPolyJgDraw2.CreatePolyAngleDrawing((CLDSPolyPart*)pJgTypePart2->pPart,draw_rect2.Width(),draw_rect2.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);	
			DrawJgMap(&xPolyJgDraw2,origin);
		}
		if (pJgTypePart2->pPart==NULL)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CProcessAngle*)pJgTypePart2,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
	}
	return 2;	//一次处理了两个角钢构件
}

int DrawMultiPartInOnePaper2(CProcessPart *pJgTypePart,CProcessPart *pJgTypePart2,int nHits,int nHits2,f3dPoint origin)
{
	int i=0,j=0;
	CXhChar100 sPartNo,guige,guige2,ls_str,ls_str2;
	CXhChar16 steelmark;
	//刘伟说宝鸡一卡两件中,Q235材质也需要正常标识 wjh-2017.11.1
	//if(pJgTypePart->cMaterial!=CSteelMatLibrary::Q235BriefMark())
	QuerySteelMatMark(pJgTypePart->cMaterial,steelmark);
#ifdef AFX_TARG_ENU_ENGLISH
	guige.Printf("%sL%.0fX%.0fX%d %d piece",(char*)steelmark,pJgTypePart->GetWidth(),pJgTypePart->GetThick(),pJgTypePart->GetLength(),nHits);
#else
	guige.Printf("%sL%.0fX%.0fX%d %d件",(char*)steelmark,pJgTypePart->GetWidth(),pJgTypePart->GetThick(),pJgTypePart->GetLength(),nHits);
#endif
	if(pJgTypePart2)
	{
		//if(pJgTypePart2->cMaterial!=CSteelMatLibrary::Q235BriefMark())
		QuerySteelMatMark(pJgTypePart2->cMaterial,steelmark);
		//else
		//	steelmark.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
		guige2.Printf("%sL%.0fX%.0fX%d %d piece",(char*)steelmark,pJgTypePart2->GetWidth(),pJgTypePart2->GetThick(),pJgTypePart2->GetLength(),nHits2);
#else
		guige2.Printf("%sL%.0fX%.0fX%d %d件",(char*)steelmark,pJgTypePart2->GetWidth(),pJgTypePart2->GetThick(),pJgTypePart2->GetLength(),nHits2);
#endif
	}
	CFlexTbl xJgProcessCard;
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表

	CFormatTbl xHeaderTbl,xTwoJgCard;
	xHeaderTbl.Create(8,3);
	xTwoJgCard.Create(4,4);
	xHeaderTbl.SetColumnWide(0,7);
	xHeaderTbl.SetColumnWide(1,7);
	xHeaderTbl.SetColumnWide(2,7);
	xHeaderTbl.SetRowHigh(0,13);
	xHeaderTbl.SetRowHigh(1,18);
	xHeaderTbl.SetRowHigh(2,20);
	xHeaderTbl.SetRowHigh(3,31);
	xHeaderTbl.SetRowHigh(4,11);
	xHeaderTbl.SetRowHigh(5,32);
	xHeaderTbl.SetRowHigh(6,20);
	xHeaderTbl.SetRowHigh(7,20);
	xHeaderTbl.GetGridAt(4,2)->m_bVGridVirtual = TRUE;
	xHeaderTbl.GetGridAt(5,2)->m_bVGridVirtual = TRUE;
	xHeaderTbl.GetGridAt(4,1)->m_bHGridVirtual = TRUE;
	for(i=5;i<8;i++)
	{
		xHeaderTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
		xHeaderTbl.GetGridAt(i,1)->m_bHGridVirtual = TRUE;
		xHeaderTbl.GetGridAt(i,1)->m_bVGridVirtual = TRUE;
	}
	xHeaderTbl.SetLocateStyle(BOTTOM_LEFT);
	xHeaderTbl.SetBasePos(f2dPoint(origin.x,origin.y));
	//设置表格内容
	for(i=0;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			CGrid *pGrid=xHeaderTbl.GetGridAt(i,j);
			pGrid->feature = TEXTALIGN_MIDDLE_CENTER;
			pGrid->text_angle = -Pi/2;
			pGrid->textsize = sys.fonts.angle.fCommonTextSize;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	xHeaderTbl.SetGrid(0,0,"Operator");
	xHeaderTbl.SetGrid(1,0,Ta.m_sOperator);
	xHeaderTbl.SetGrid(0,1,"Critic");
	xHeaderTbl.SetGrid(1,1,Ta.m_sCritic);
	xHeaderTbl.SetGrid(0,2,"Auditor");
	xHeaderTbl.SetGrid(1,2,Ta.m_sAuditor);
	xHeaderTbl.SetGrid(2,0,"Date");
	xHeaderTbl.SetGrid(2,1,"Drawing number");
	xHeaderTbl.SetGrid(2,2,"Project Name");
	xHeaderTbl.SetGrid(4,2,"Tower Type");
	CXhChar100 page_str;
	//根据刘伟要求一卡多件工艺卡中不需要页码 wxc-2017.9.12
	//page_str.Printf("Total %d pages",nPageNum);
	page_str.Copy("Total      pages");
	xHeaderTbl.SetGrid(6,2,page_str);
	//if (iPage!=-1)
	//	page_str.Printf("No. %d page",iPage+1);
	//else
	//	page_str.Printf("No. %d page",iOrder+1);
	page_str.Copy("No.      pages");
#else
	xHeaderTbl.SetGrid(0,0,"制图");
	xHeaderTbl.SetGrid(1,0,Ta.m_sOperator);
	xHeaderTbl.SetGrid(0,1,"复核");
	xHeaderTbl.SetGrid(1,1,Ta.m_sCritic);
	xHeaderTbl.SetGrid(0,2,"审核");
	xHeaderTbl.SetGrid(1,2,Ta.m_sAuditor);
	xHeaderTbl.SetGrid(2,0,"日  期");
	xHeaderTbl.SetGrid(2,1,"产品代号");
	xHeaderTbl.SetGrid(2,2,"塔型");
	CXhChar100 page_str;
	//根据刘伟要求一卡多件工艺卡中不需要页码 wxc-2017.9.12
	//page_str.Printf("共 %d 页",nPageNum);
	page_str.Copy("共      页");
	xHeaderTbl.SetGrid(6,2,page_str);
	//if(iPage!=-1)
	//	page_str.Printf("第 %d 页",iPage+1);
	//else
	//	page_str.Printf("第 %d 页",iOrder+1);
	page_str.Copy("第      页");
#endif
	xHeaderTbl.SetGrid(7,2,page_str);
	xHeaderTbl.SetGrid(3,2,Ta.m_sTaType);
	CTime t = CTime::GetCurrentTime();
	CXhChar100 time_str;
	time_str.Printf("%s",t.Format("%y %m %d"));
	xHeaderTbl.SetGrid(3,0,time_str);
	xHeaderTbl.SetGrid(3,1,Ta.m_sTaAlias);
	xHeaderTbl.SetGrid(4,0,sys.m_sCompanyName);
	xHeaderTbl.GetGridAt(4,0)->textsize = sys.dim_map.fHeaderTextSize;
	DrawTable(pBlockTableRecord,&xHeaderTbl,FALSE);
	if(sys.general_set.bHasFrame)
		InsertBlock(pBlockTableRecord,origin,"frame",1,0);
	xTwoJgCard.SetColumnWide(0,19);
	xTwoJgCard.SetColumnWide(1,60);
	xTwoJgCard.SetColumnWide(2,60);
	xTwoJgCard.SetColumnWide(3,90);
	xTwoJgCard.SetRowHigh(0,5);
	xTwoJgCard.SetRowHigh(1,77.5);
	xTwoJgCard.SetRowHigh(2,5);
	xTwoJgCard.SetRowHigh(3,77.5);
	f2dPoint pos;
	xHeaderTbl.GetRowLineEnd(0,pos);
	xTwoJgCard.SetLocateStyle(TOP_LEFT);
	xTwoJgCard.SetBasePos(pos);
	for(int j=1;j<4;j++)
	{
		xTwoJgCard.GetGridAt(1,j)->m_bHGridVirtual = TRUE;
		xTwoJgCard.GetGridAt(3,j)->m_bHGridVirtual = TRUE;
	}

	xTwoJgCard.SetGrid(0,0,GetPartNoIncMat(pJgTypePart->GetPartNo(),pJgTypePart->cMaterial));
	if(pJgTypePart2)
		xTwoJgCard.SetGrid(2,0,GetPartNoIncMat(pJgTypePart2->GetPartNo(),pJgTypePart2->cMaterial));
	xTwoJgCard.GetGridAt(0,0)->feature = TEXTALIGN_MIDDLE_CENTER;	//刘伟反馈宝鸡要求，一卡多件件号居中显示 wht 18-11-01
	xTwoJgCard.GetGridAt(0,1)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(0,2)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(0,3)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(2,0)->feature = TEXTALIGN_MIDDLE_CENTER;
	xTwoJgCard.GetGridAt(2,1)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(2,2)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.GetGridAt(2,3)->feature = TEXTALIGN_LEFT_CENTER;
	xTwoJgCard.SetGrid(0,1,guige);
	xTwoJgCard.SetGrid(2,1,guige2);
	CXhChar200 notes1,notes2;
	CXhChar100 notes;
#ifdef AFX_TARG_ENU_ENGLISH
	notes1.Copy("notes：");
	notes2.Copy("notes：");
#else
	notes1.Copy("备注：");
	notes2.Copy("备注：");
#endif
	if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		((CLDSLineAngle*)pJgTypePart->pPart)->GetLsRefList()->GetLsString(ls_str);
		((CLDSLineAngle*)pJgTypePart->pPart)->GetProcessStr(notes,100,false,true);
		if(strlen(pJgTypePart->pPart->sNotes)>0)
		{
			notes.Append(",");
			notes.Append(pJgTypePart->pPart->sNotes);
		}
	}
	else if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pJgTypePart->pPart;
		CLsRefList lsRefList;
		CLDSLinePart *pLinePart=NULL;
		for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
		{
			if(strlen(pLinePart->sNotes)>0)
			{
				notes.Append(",");
				notes.Append(pLinePart->sNotes);
			}
			CLsRef *pLsRef=NULL;
			for(pLsRef=pLinePart->GetFirstLsRef();pLsRef;pLsRef=pLinePart->GetNextLsRef())
				lsRefList.append(*pLsRef);
		}
		lsRefList.GetLsString(ls_str);
	}
	else if(pJgTypePart->pPart==NULL)
	{
		if(pJgTypePart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
		{
			((CProcessAngle*)pJgTypePart)->GetLsString(ls_str);
			((CProcessAngle*)pJgTypePart)->GetProcessStr(notes,100,false,true);
			if(strlen(((CProcessAngle*)pJgTypePart)->GetNotes())>0)
			{
				notes.Append(",");
				notes.Append(pJgTypePart->GetNotes());
			}
		}
	}
	notes1.Append(notes);
	notes.Empty();	//清空上一个构件的备注内容，防止与下一个构件备注重叠
	if(pJgTypePart2)
	{	
		if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			((CLDSLineAngle*)pJgTypePart2->pPart)->GetLsRefList()->GetLsString(ls_str2);
			((CLDSLineAngle*)pJgTypePart2->pPart)->GetProcessStr(notes,100,false,true);
		}
		else if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pJgTypePart2->pPart;
			CLsRefList lsRefList;
			CLDSLinePart *pLinePart=NULL;
			for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
			{
				if(strlen(pLinePart->sNotes)>0)
				{
					notes.Append(",");
					notes.Append(pLinePart->sNotes);
				}
				CLsRef *pLsRef=NULL;
				for(pLsRef=pLinePart->GetFirstLsRef();pLsRef;pLsRef=pLinePart->GetNextLsRef())
					lsRefList.append(*pLsRef);
			}
			lsRefList.GetLsString(ls_str2);
		}
		else if (pJgTypePart2==NULL)
		{
			if (pJgTypePart2->m_cPartType==CProcessPart::TYPE_LINEANGLE)
			{
				((CProcessAngle*)pJgTypePart2)->GetLsString(ls_str);
				((CProcessAngle*)pJgTypePart2)->GetProcessStr(notes,100,false,true);
				if(strlen(((CProcessAngle*)pJgTypePart2)->GetNotes())>0)
				{
					notes.Append(",");
					notes.Append(pJgTypePart2->GetNotes());
				}
			}
		}
		notes2.Append(notes);
	}
	xTwoJgCard.SetGrid(0,2,ls_str);
	xTwoJgCard.SetGrid(2,2,ls_str2);
	//一卡多件原本是专为宝鸡塔厂绘制,现据刘伟说根据他们的要求,切角切肢时,一律显示切角 wjh-2017.10.31
	if(strstr(notes1,"切角")!=NULL)
		notes1.Replace("切肢","");
	else
		notes1.Replace("切肢","切角");
	if(strstr(notes2,"切角")!=NULL)
		notes2.Replace("切肢","");
	else
		notes2.Replace("切肢","切角");
	xTwoJgCard.SetGrid(0,3,notes1);
	xTwoJgCard.SetGrid(2,3,notes2);
	for(int i=0;i<4;i++)
	{
		xTwoJgCard.GetGridAt(0,i)->textsize=sys.fonts.angle.fNumericTextSize;//dim_map.fCommonTextSize;
		xTwoJgCard.GetGridAt(2,i)->textsize=sys.fonts.angle.fNumericTextSize;//dim_map.fCommonTextSize;
	}
	DrawTable(pBlockTableRecord,&xTwoJgCard,FALSE);
	pBlockTableRecord->close();

	f2dRect draw_rect,draw_rect2;
	xTwoJgCard.GetGridRect(1,0,draw_rect);
	xTwoJgCard.GetGridRect(3,0,draw_rect2);
	if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
	{
		origin.x = draw_rect.topLeft.x;
		origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
		if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CLDSLineAngle*)pJgTypePart->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
		else if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CPolyAngleDrawing xPolyJgDraw;
			xPolyJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);	
			xPolyJgDraw.CreatePolyAngleDrawing((CLDSPolyPart*)pJgTypePart->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);	
			DrawJgMap(&xPolyJgDraw,origin);
		}
		if (pJgTypePart->pPart==NULL)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CProcessAngle*)pJgTypePart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
	}
	if(pJgTypePart2&&draw_rect2.Width()>EPS2&&draw_rect2.Height()>EPS2)
	{
		origin.x = draw_rect2.topLeft.x;
		origin.y = (draw_rect2.topLeft.y+draw_rect2.bottomRight.y)/2;
		if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLineAngleDrawing xJgDraw2;
			xJgDraw2.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw2.CreateLineAngleDrawing((CLDSLineAngle*)pJgTypePart2->pPart,draw_rect2.Width(),draw_rect2.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw2,origin);
		}
		else if(pJgTypePart2->pPart&&pJgTypePart2->pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CPolyAngleDrawing xPolyJgDraw2;
			xPolyJgDraw2.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xPolyJgDraw2.CreatePolyAngleDrawing((CLDSPolyPart*)pJgTypePart2->pPart,draw_rect2.Width(),draw_rect2.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);	
			DrawJgMap(&xPolyJgDraw2,origin);
		}
		if (pJgTypePart2->pPart==NULL)
		{
			CLineAngleDrawing xJgDraw;
			xJgDraw.dimStyleFlag.SetFlagDword(CAngleDrawing::OneCardMultiPart);
			xJgDraw.CreateLineAngleDrawing((CProcessAngle*)pJgTypePart2,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
			DrawJgMap(&xJgDraw,origin);
		}
	}
	return 2;	//一次处理了两个角钢构件
}
int DrawJgTypeMap(CProcessPart *pJgTypePart,CProcessPart *pJgTypePart2,f2dRect *pDrawRect,
				   int iOrder,int nHits,int nHits2,char *layer,int nPageNum/*=1*/,
				   int nTaNum/*=1*/,BOOL bDimPartInfo/*=TRUE*/,int iPage/*=-1*/)
{
	if(pJgTypePart==NULL)
		return 0;
	f3dPoint origin;	//原点坐标为(0,0,0)
	if(iOrder>0)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}
	
	CLineAngleDrawing xJgDraw;
	//生成此构件的需要注释的内容
	double length,weight;
	int nLsN=0;
	CXhChar100 sPartNo,guige,guige2,ls_str,ls_str2;
	if(pJgTypePart->IsAngle()&&((CProcessAngle*)pJgTypePart)->IsUnequalAngle())
		guige.Printf("L%.0fX%.0fX%.0f",pJgTypePart->GetWidth(),pJgTypePart->GetHeight(),pJgTypePart->GetThick());
	else
		guige.Printf("L%.0fX%.0f",pJgTypePart->GetWidth(),pJgTypePart->GetThick());
	if(pJgTypePart->pPart)//TODO:出错了吗？
		length=pJgTypePart->pPart->GetLength();
	else
		length=pJgTypePart->GetLength();
	//添加以下代码主要是为了使工艺卡上的角钢长度与中间大样图上的角钢总长标注小数点后一位也一致
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		length = ftoi(length);
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
		length = ftoi(length*2)/2.0;
	else if(sys.general_set.iDimPrecision==2)//精确到0.1mm
		length = ftol(length*10)/10.0;				
	weight = pJgTypePart->GetWeight();//length*GetJgUnitLenWei(pJgTypePart->GetWidth(),pJgTypePart->GetThick())/1000;
	strcpy(sPartNo,GetPartNoIncMat(pJgTypePart->GetPartNo(),pJgTypePart->cMaterial));
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(sys.part_map.angle.bOneCardMultiPart)	//省纸模式简化角钢工艺卡
	{
		return DrawMultiPartInOnePaper2(pJgTypePart,pJgTypePart2,nHits,nHits2,origin);
	}
	else if(sys.general_set.bProcessCardTbl)
	{
		CFlexTbl xJgProcessCard;
		//获得当前图形的块表指针
		AcDbBlockTable *pBlockTable;
		GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
		pBlockTable->close();//关闭块表

		CProcessAngle *pJg=NULL;
		if(pJgTypePart->m_cPartType!=CProcessPart::TYPE_LINEANGLE)
			return 0;
		pJg=(CProcessAngle*)pJgTypePart;
		f2dRect draw_rect=GetProcessCardDrawingRect();
		if(pJg->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
			xJgDraw.CreateLineAngleDrawing((CLDSLineAngle*)pJg->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
		else if(pJg->pPart==NULL)
			xJgDraw.CreateLineAngleDrawing(pJg,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
		PART_JG_CARD_INFO JgInfo;
		memset(&JgInfo,'\0',sizeof(PART_JG_CARD_INFO));
		JgInfo.m_nPageNum=nPageNum;		//总页数
		if(iOrder<0&&iPage==-1)
			JgInfo.m_nPageIndex=1;		//第几页
		else if(iPage!=-1)
			JgInfo.m_nPageIndex=iPage+1;//第几页
		else
			JgInfo.m_nPageIndex=iOrder+1;//第几页
		if(nTaNum>0)
			JgInfo.m_nTaNum = nTaNum;
		JgInfo.sSeg=pJg->iSeg.ToString();
		JgInfo.sPartNo.Copy(GetPartNoIncMat(pJg->GetPartNo(),pJg->cMaterial));
		if(pJg->pPart&&pJg->pPart->GetClassTypeId()==CLS_LINEANGLE)
			JgInfo.m_nCutBerR = ((CLDSLineAngle*)pJg->pPart)->m_nCutBerR;
		if(length-(int)length>EPS)
			JgInfo.sLength.Printf("%.1f",length);
		else
			JgInfo.sLength.Printf("%.0f",length);
		if(pJg->m_bCutRoot)	//刨根处理
		{
			JgInfo.sCutRoot.Copy(GetMarkSymbol());
			JgInfo.sCutBer.Empty();
			//暂时仅支持标注短角钢刨根长度 wht 10-11-17
			if(pJg->pPart&&pJg->pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
			if(((CLDSLineAngle*)pJg->pPart)->pStart==NULL||((CLDSLineAngle*)pJg->pPart)->pEnd==NULL)
				JgInfo.sCutRootLen.Printf("%.0f",length);
			else 
				JgInfo.sCutRootLen.Empty();
			}
			JgInfo.sCutBerLen.Empty();
		}
		else if(pJg->m_bCutBer)	//铲背处理
		{
			JgInfo.sCutRoot.Empty();
			JgInfo.sCutBer.Copy(GetMarkSymbol());
			//暂时仅支持标注短角钢铲背长度 wht 10-11-17
			if(pJg->pPart&&pJg->pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
			if(((CLDSLineAngle*)pJg->pPart)->pStart==NULL||((CLDSLineAngle*)pJg->pPart)->pEnd==NULL)
				JgInfo.sCutBerLen.Printf("%.0f",length);
			else 
				JgInfo.sCutBerLen.Empty();
			}
			JgInfo.sCutRootLen.Empty();
		}
		else
		{
			JgInfo.sCutRoot.Empty();
			JgInfo.sCutBer.Empty();
			JgInfo.sCutRootLen.Empty();
			JgInfo.sCutBerLen.Empty();
		}
		if(pJg->m_bWeld)
			JgInfo.sWeld.Copy(GetMarkSymbol());
		else
			JgInfo.sWeld.Empty();
		
		JgInfo.sPartNum.Printf("%d",nHits);
		JgInfo.sSumPartNum.Printf("%d",nHits*JgInfo.m_nTaNum);
		
		JgInfo.sPartWeight.Printf("%.1f",weight);
		JgInfo.sSumPartWeight.Printf("%.1f",weight*nHits*JgInfo.m_nTaNum);
		//选择不在绘图区域标注开合角时也应该在工艺卡中标注开合角度 wht 10-11-02
		/*if(xJgDraw.GetKaiHeDimCount()>0)
		{
			if(pJg->GetDecWingAngle()>90)
				JgInfo.sKaiJiao.Printf("+%.1f°",pJg->GetDecWingAngle()-90);
			else
				JgInfo.sHeJiao.Printf("-%.1f°",90-pJg->GetDecWingAngle());
		}*/
		//根据97制图规定进行的修改,夹角偏差大于2需要开合角
		//使用【开合角阈值】控制是否需要标注开合角信息默认为2°wht 11-05-06
		double wing_angle=pJg->GetDecWingAngle();
		if(pJg->kaihe_base_wing_x0_y1==0||pJg->kaihe_base_wing_x0_y1==1)
		{
			if(wing_angle-90>sys.part_map.angle.fKaiHeJiaoThreshold)
				JgInfo.sKaiJiao.Printf("+%.1f",wing_angle-90);
			else if(90-wing_angle>sys.part_map.angle.fKaiHeJiaoThreshold)
				JgInfo.sHeJiao.Printf("-%.1f",90-wing_angle);
		}
		//
		JgInfo.sWingAngle.Printf("%.1f°",wing_angle);
		JgInfo.sNotes.Printf("%s",pJg->GetNotes());
		
		nLsN=pJg->GetLsCount(12);
		JgInfo.sM12HoleNum.Printf("%d",nLsN);
		nLsN=pJg->GetLsCount(16);
		JgInfo.sM16HoleNum.Printf("%d",nLsN);
		nLsN=pJg->GetLsCount(18);
		JgInfo.sM18HoleNum.Printf("%d",nLsN);
		nLsN=pJg->GetLsCount(20);
		JgInfo.sM20HoleNum.Printf("%d",nLsN);
		nLsN=pJg->GetLsCount(22);
		JgInfo.sM22HoleNum.Printf("%d",nLsN);
		nLsN=pJg->GetLsCount(24);
		JgInfo.sM24HoleNum.Printf("%d",nLsN);
		nLsN=pJg->GetLsCount();
		JgInfo.sSumHoleNum.Printf("%d",nLsN);
		//统计钻孔数
		if(gxDrwLocalizer.ciCustomerSerial==5)	
		{	//青岛东方定制,根据特定条件判断角钢加工工艺
			if((pJg->cMaterial==CSteelMatLibrary::Q235BriefMark()&&pJg->m_fThick<=16)||
				(pJg->cMaterial==CSteelMatLibrary::Q345BriefMark()&&pJg->m_fThick<=14)||
				(pJg->cMaterial==CSteelMatLibrary::Q420BriefMark()&&pJg->m_fThick<=12))
			{
				if(pJg->m_fWidth<100)
					JgInfo.m_bPunchShear=TRUE;
				else
					JgInfo.m_bOneShot=TRUE;
			}
			else if(pJg->m_fWidth>=100)
				JgInfo.m_bGasCutting=TRUE;
			//判断螺栓孔加工工艺
			if(JgInfo.m_bPunchShear)	//冲剪
				JgInfo.sChongKongNum.Copy("1");
			if(JgInfo.m_bGasCutting)	//气割
				JgInfo.sZuanKongNum.Copy("1");
		}
		else
		{
			nLsN=pJg->GetDrillHoleCount();
			if(nLsN)
				JgInfo.sZuanKongNum.Printf("%d",nLsN);
			int nChongKongNum=pJg->GetLsCount()-nLsN;
			if(nChongKongNum>0)
				JgInfo.sChongKongNum.Printf("%d",nChongKongNum);
		}
		int n=strlen(guige);
		CXhChar16 steelmark;
		if(pJg->pPart&&pJg->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
		if(((CLDSLineAngle*)pJg->pPart)->m_bReplPart)	//代用构件
		{
			QuerySteelMatMark(pJg->cMaterial,steelmark);
			JgInfo.sReplMaterial.Copy(steelmark);
			JgInfo.sReplGuiGe.Copy(guige);
			QuerySteelMatMark(((CLDSLineAngle*)pJg->pPart)->org_material,steelmark);
			JgInfo.sDesMaterial.Copy(steelmark);
			JgInfo.sDesGuiGe.Printf("L%.0fX%.0f",((CLDSLineAngle*)pJg->pPart)->org_wing_wide,((CLDSLineAngle*)pJg->pPart)->org_wing_thick);
		}
		else
		{
			QuerySteelMatMark(pJg->cMaterial,steelmark);
			JgInfo.sDesMaterial.Copy(steelmark);
			JgInfo.sDesGuiGe.Copy(guige);
		}
		}
#ifdef ZHEJIANGSHENGDA//下面以前是浙江盛达的标法，觉得不太合适，现在改为重庆顺泰的标法，感觉更规范一些
		if((pJg->cut_angle[0][0]>0&&pJg->cut_angle[0][1]>0)||pJg->cut_wing[0]>0)
		{
			if(pJg->cut_wing[0]!=2)
				JgInfo.sCutAngSX.Copy(GetMarkSymbol());
			if(pJg->cut_wing[0]==1)		//始端X肢有切肢
				JgInfo.sCutAngParaSX.Printf("%.0f=%.0f",pJg->cut_wing_para[0][0],pJg->cut_wing_para[0][2]);
			else if(pJg->cut_wing[0]==2)	//始端Y肢有切肢
				JgInfo.sCutAngParaSX.Printf("%.0f<%.0f",pJg->cut_wing_para[0][1],pJg->cut_wing_para[0][0]);
			else						//始端X肢有切角
				JgInfo.sCutAngParaSX.Printf("%.0fX%.0f",pJg->cut_angle[0][1],pJg->cut_angle[0][0]);
			
		}
		if((pJg->cut_angle[1][0]>0&&pJg->cut_angle[1][1]>0)||pJg->cut_wing[0]>0)
		{
			if(pJg->cut_wing[0]!=1)
				JgInfo.sCutAngSY.Copy(GetMarkSymbol());
			if(pJg->cut_wing[0]==1)		//始端X肢有切肢
				JgInfo.sCutAngParaSY.Printf("%.0f<%.0f",pJg->cut_wing_para[0][1],pJg->cut_wing_para[0][0]);
			else if(pJg->cut_wing[0]==2)	//始端Y肢有切肢
				JgInfo.sCutAngParaSY.Printf("%.0f=%.0f",pJg->cut_wing_para[0][0],pJg->cut_wing_para[0][2]);
			else						//始端Y肢有切角
				JgInfo.sCutAngParaSY.Printf("%.0fX%.0f",pJg->cut_angle[1][1],pJg->cut_angle[1][0]);
		}
		if((pJg->cut_angle[2][0]>0&&pJg->cut_angle[2][1]>0)||pJg->cut_wing[1]>0)
		{
			if(pJg->cut_wing[1]!=2)
				JgInfo.sCutAngEX.Copy(GetMarkSymbol());
			if(pJg->cut_wing[1]==1)		//终端X肢有切肢
				JgInfo.sCutAngParaEX.Printf("%.0f=%.0f",pJg->cut_wing_para[1][0],pJg->cut_wing_para[1][2]);
			else if(pJg->cut_wing[1]==2)	//终端Y肢有切肢
				JgInfo.sCutAngParaEX.Printf("%.0f<%.0f",pJg->cut_wing_para[1][1],pJg->cut_wing_para[1][0]);
			else						//终端X肢有切角
				JgInfo.sCutAngParaEX.Printf("%.0fX%.0f",pJg->cut_angle[2][1],pJg->cut_angle[2][0]);
		}
		if((pJg->cut_angle[3][0]>0&&pJg->cut_angle[3][1]>0)||pJg->cut_wing[1]>0)
		{
			if(pJg->cut_wing[1]!=1)
				JgInfo.sCutAngEY.Copy(GetMarkSymbol());
			if(pJg->cut_wing[1]==1)		//终端X肢有切肢
				JgInfo.sCutAngParaEY.Printf("%.0f<%.0f",pJg->cut_wing_para[1][1],pJg->cut_wing_para[1][0]);
			else if(pJg->cut_wing[1]==2)	//终端Y肢有切肢
				JgInfo.sCutAngParaEY.Printf("%.0f=%.0f",pJg->cut_wing_para[1][0],pJg->cut_wing_para[1][2]);
			else						//终端Y肢有切角
				JgInfo.sCutAngParaEY.Printf("%.0fX%.0f",pJg->cut_angle[3][1],pJg->cut_angle[3][0]);
		}
#else
		if(sys.part_map.angle.iCutAngleDimType==1)
		{	//切角标注样式二 B:端头尺寸 L:肢边尺寸 C:筋边尺寸 
			//BXL 切角  CXL 切肢 BXC 切筋  切大角=切筋+切肢
			if((pJg->cut_angle[0][0]>0&&pJg->cut_angle[0][1]>0)||pJg->cut_wing[0]>0)
			{
				if(pJg->cut_wing[0]!=2)
					JgInfo.sCutAngSX.Copy(GetMarkSymbol());
				if(pJg->cut_wing[0]==1)		//始端X肢有切肢
				{
					JgInfo.sCutAngParaSX.Printf("%.0f=%.0f",pJg->cut_wing_para[0][0],pJg->cut_wing_para[0][2]);
					if(pJg->cut_angle[0][0]>pJg->cut_wing_para[0][2])
						JgInfo.sCutAngParaSX.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[0][1],pJg->cut_angle[0][0]));
				}
				else if(pJg->cut_wing[0]==2)	//始端Y肢有切肢
				{
					JgInfo.sCutAngParaSX.Printf("%.0f<%.0f",pJg->cut_wing_para[0][1],pJg->cut_wing_para[0][0]);
					if(pJg->cut_angle[0][0]>0&&pJg->cut_angle[0][1]>0)
						JgInfo.sCutAngParaSX.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[0][1],pJg->cut_angle[0][0]));
				}
				else						//始端X肢有切角
					JgInfo.sCutAngParaSX.Printf("%.0fX%.0f",pJg->cut_angle[0][1],pJg->cut_angle[0][0]);
				
			}
			if((pJg->cut_angle[1][0]>0&&pJg->cut_angle[1][1]>0)||pJg->cut_wing[0]>0)
			{
				if(pJg->cut_wing[0]!=1)
					JgInfo.sCutAngSY.Copy(GetMarkSymbol());
				if(pJg->cut_wing[0]==1)		//始端X肢有切肢
				{
					JgInfo.sCutAngParaSY.Printf("%.0f<%.0f",pJg->cut_wing_para[0][1],pJg->cut_wing_para[0][0]);
					if(pJg->cut_angle[1][0]>0&&pJg->cut_angle[1][1]>0)
						JgInfo.sCutAngParaSY.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[1][1],pJg->cut_angle[1][0]));
				}
				else if(pJg->cut_wing[0]==2)	//始端Y肢有切肢
				{
					JgInfo.sCutAngParaSY.Printf("%.0f=%.0f",pJg->cut_wing_para[0][0],pJg->cut_wing_para[0][2]);
					if(pJg->cut_angle[1][0]>pJg->cut_wing_para[0][2])
						JgInfo.sCutAngParaSY.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[1][1],pJg->cut_angle[1][0]));
				}
				else						//始端Y肢有切角
					JgInfo.sCutAngParaSY.Printf("%.0fX%.0f",pJg->cut_angle[1][1],pJg->cut_angle[1][0]);
			}
			if((pJg->cut_angle[2][0]>0&&pJg->cut_angle[2][1]>0)||pJg->cut_wing[1]>0)
			{
				if(pJg->cut_wing[1]!=2)
					JgInfo.sCutAngEX.Copy(GetMarkSymbol());
				if(pJg->cut_wing[1]==1)		//终端X肢有切肢
				{
					JgInfo.sCutAngParaEX.Printf("%.0f=%.0f",pJg->cut_wing_para[1][0],pJg->cut_wing_para[1][2]);
					if(pJg->cut_angle[2][0]>pJg->cut_wing_para[1][2])
						JgInfo.sCutAngParaEX.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[2][1],pJg->cut_angle[2][0]));
				}
				else if(pJg->cut_wing[1]==2)	//终端Y肢有切肢
				{
					JgInfo.sCutAngParaEX.Printf("%.0f<%.0f",pJg->cut_wing_para[1][1],pJg->cut_wing_para[1][0]);
					if(pJg->cut_angle[2][0]>0&&pJg->cut_angle[2][1]>0)
						JgInfo.sCutAngParaEX.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[2][1],pJg->cut_angle[2][0]));
				}
				else						//终端X肢有切角
					JgInfo.sCutAngParaEX.Printf("%.0fX%.0f",pJg->cut_angle[2][1],pJg->cut_angle[2][0]);
			}
			if((pJg->cut_angle[3][0]>0&&pJg->cut_angle[3][1]>0)||pJg->cut_wing[1]>0)
			{
				if(pJg->cut_wing[1]!=1)
					JgInfo.sCutAngEY.Copy(GetMarkSymbol());
				if(pJg->cut_wing[1]==1)		//终端X肢有切肢
				{
					JgInfo.sCutAngParaEY.Printf("%.0f<%.0f",pJg->cut_wing_para[1][1],pJg->cut_wing_para[1][0]);
					if(pJg->cut_angle[3][0]>0&&pJg->cut_angle[3][1]>0)
						JgInfo.sCutAngParaEY.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[3][1],pJg->cut_angle[3][0]));
				}
				else if(pJg->cut_wing[1]==2)	//终端Y肢有切肢
				{
					JgInfo.sCutAngParaEY.Printf("%.0f=%.0f",pJg->cut_wing_para[1][0],pJg->cut_wing_para[1][2]);
					if(pJg->cut_angle[3][0]>pJg->cut_wing_para[1][2])
						JgInfo.sCutAngParaEY.Append(CXhChar50(",%.0fX%.0f",pJg->cut_angle[3][1],pJg->cut_angle[3][0]));
				}
				else						//终端Y肢有切角
					JgInfo.sCutAngParaEY.Printf("%.0fX%.0f",pJg->cut_angle[3][1],pJg->cut_angle[3][0]);
			}	
		}
		else
		{	//切角标注样式一
			int alfa=0;
			CXhChar16 alfa_str,cut_para_str;
			if(pJg->cut_wing[0]>0)
			{
				if(pJg->cut_wing[0]==1)		//始端X肢有切肢
				{
					JgInfo.sCutAngSX.Copy(GetMarkSymbol());
					alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[0][2]-pJg->cut_wing_para[0][0])/pJg->GetWidth()));
					if(pJg->cut_wing_para[0][1]==0)	//X肢切整肢
						JgInfo.sCutAngParaSX.Printf("CF%.0f",pJg->cut_wing_para[0][0]);
					else	//背切肢
						JgInfo.sCutAngParaSX.Printf("BC%.0fX%.0f",pJg->cut_wing_para[0][0],pJg->cut_wing_para[0][1]);
					if(alfa!=0)
					{
						if(gxDrwLocalizer.ciCustomerSerial!=4)		//X肢切肢用角度表示
							alfa_str.Printf("+CO%d%%%%D",alfa);
						else										//X肢切肢用切割数据表示(重庆顺泰定制)	
							alfa_str.Printf("X%.0f",pJg->cut_wing_para[0][2]);
						JgInfo.sCutAngParaSX.Append(alfa_str);
					}
					//重庆顺泰要求将X肢(无孔肢，切整肢除外)切肢信息保存到Y肢(有孔肢)中
					if(gxDrwLocalizer.ciCustomerSerial==4&&JgInfo.sCutAngParaSX[0]!='C')
					{
						JgInfo.sCutAngParaSY.Copy(JgInfo.sCutAngParaSX);
						JgInfo.sCutAngParaSX.Empty();
					}
				}
				else if(pJg->cut_wing[0]==2)	//始端Y肢有切肢
				{
					JgInfo.sCutAngSY.Copy(GetMarkSymbol());
					alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[0][2]-pJg->cut_wing_para[0][0])/pJg->GetWidth()));
					if(pJg->cut_wing_para[0][1]==0)	//X肢切整肢
						JgInfo.sCutAngParaSY.Printf("CF%.0f",pJg->cut_wing_para[0][0]);
					else	//背切肢
						JgInfo.sCutAngParaSY.Printf("BC%.0fX%.0f",pJg->cut_wing_para[0][0],pJg->cut_wing_para[0][1]);
					if(alfa!=0)
					{
						if(gxDrwLocalizer.ciCustomerSerial!=4)		//Y肢切肢用角度表示
							alfa_str.Printf("+CO%d%%%%D",alfa);
						else										//Y肢切肢用切割数据表示
							alfa_str.Printf("X%.0f",pJg->cut_wing_para[0][2]);
						JgInfo.sCutAngParaSY.Append(alfa_str);		
					}
					//重庆顺泰要求将Y肢(无孔肢，切整肢除外)切肢信息保存到X肢(有孔肢)中
					if(gxDrwLocalizer.ciCustomerSerial==4&&JgInfo.sCutAngParaSY[0]!='C')
					{
						JgInfo.sCutAngParaSX.Copy(JgInfo.sCutAngParaSY);
						JgInfo.sCutAngParaSY.Empty();
					}
				}
			}
			if(pJg->cut_angle[0][0]>0&&pJg->cut_angle[0][1]>0&&pJg->cut_wing[0]!=1)	
			{	//始端X肢有切角
				JgInfo.sCutAngSX.Copy(GetMarkSymbol());
				if(pJg->cut_angle[0][0]==pJg->cut_angle[0][1]&&pJg->cut_angle[0][1]==(pJg->GetWidth()-pJg->GetThick()))
					cut_para_str.Printf("CO45%%%%D",alfa);
				else
					cut_para_str.Printf("CT%.0fX%.0f",pJg->cut_angle[0][0],pJg->cut_angle[0][1]);
				JgInfo.sCutAngParaSX.Append(cut_para_str,'+');
			}
			if(pJg->cut_angle[1][0]>0&&pJg->cut_angle[1][1]>0&&pJg->cut_wing[0]!=2)
			{	//始端Y肢有切角
				JgInfo.sCutAngSY.Copy(GetMarkSymbol());
				if(pJg->cut_angle[1][0]==pJg->cut_angle[1][1]&&pJg->cut_angle[1][1]==(pJg->GetWidth()-pJg->GetThick()))
					cut_para_str.Printf("CO45%%%%D");
				else
					cut_para_str.Printf("CT%.0fX%.0f",pJg->cut_angle[1][0],pJg->cut_angle[1][1]);
				JgInfo.sCutAngParaSY.Append(cut_para_str,'+');
			}
			if(pJg->cut_wing[1]>0)
			{
				if(pJg->cut_wing[1]==1)		//终端X肢有切肢
				{
					JgInfo.sCutAngEX.Copy(GetMarkSymbol());
					alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[1][2]-pJg->cut_wing_para[1][0])/pJg->GetWidth()));
					if(pJg->cut_wing_para[1][1]==0)	//X肢切整肢
						JgInfo.sCutAngParaEX.Printf("CF%.0f",pJg->cut_wing_para[1][0]);
					else	//背切肢
						JgInfo.sCutAngParaEX.Printf("BC%.0fX%.0f",pJg->cut_wing_para[1][0],pJg->cut_wing_para[1][1]);
					if(alfa!=0)
					{
						if(gxDrwLocalizer.ciCustomerSerial!=4)		//X肢切肢用角度表示
							alfa_str.Printf("+CO%d%%%%D",alfa);
						else										//X肢切肢用切割数据表示
							alfa_str.Printf("X%.0f",pJg->cut_wing_para[1][2]);
						JgInfo.sCutAngParaEX.Append(alfa_str);

					}
					//重庆顺泰要求将X肢(无孔肢，切整肢除外)切肢信息保存到Y肢(有孔肢)中
					if(gxDrwLocalizer.ciCustomerSerial==4&&JgInfo.sCutAngParaEX[0]!='C')
					{
						JgInfo.sCutAngParaEY.Copy(JgInfo.sCutAngParaEX);
						JgInfo.sCutAngParaEX.Empty();
					}
				}
				else if(pJg->cut_wing[1]==2)	//终端Y肢有切肢
				{
					JgInfo.sCutAngEY.Copy(GetMarkSymbol());
					alfa = ftoi(DEGTORAD_COEF*atan((pJg->cut_wing_para[1][2]-pJg->cut_wing_para[1][0])/pJg->GetWidth()));
					if(pJg->cut_wing_para[1][1]==0)	//X肢切整肢
						JgInfo.sCutAngParaEY.Printf("CF%.0f",pJg->cut_wing_para[1][0]);
					else	//背切肢
						JgInfo.sCutAngParaEY.Printf("BC%.0fX%.0f",pJg->cut_wing_para[1][0],pJg->cut_wing_para[1][1]);
					if(alfa!=0)
					{
						if(gxDrwLocalizer.ciCustomerSerial!=4)		//Y肢切肢用角度表示
							alfa_str.Printf("+CO%d%%%%D",alfa);
						else										//Y肢切肢用切割数据表示
							alfa_str.Printf("X%.0f",pJg->cut_wing_para[1][2]);
						JgInfo.sCutAngParaEY.Append(alfa_str);
					}
					//重庆顺泰要求将Y肢(无孔肢，切整肢除外)切肢信息保存到X肢(有孔肢)中
					if(gxDrwLocalizer.ciCustomerSerial==4&&JgInfo.sCutAngParaEY[0]!='C')
					{
						JgInfo.sCutAngParaEX.Copy(JgInfo.sCutAngParaEY);
						JgInfo.sCutAngParaEY.Empty();
					}
				}
			}
			if(pJg->cut_angle[2][0]>0&&pJg->cut_angle[2][1]>0&&pJg->cut_wing[1]!=1)		
			{	//终端X肢有切角
				JgInfo.sCutAngEX.Copy(GetMarkSymbol());
				if(pJg->cut_angle[2][0]==pJg->cut_angle[2][1]&&pJg->cut_angle[2][1]==(pJg->GetWidth()-pJg->GetThick()))
					cut_para_str.Printf("CO45%%%%D");
				else
					cut_para_str.Printf("CT%.0fX%.0f",pJg->cut_angle[2][0],pJg->cut_angle[2][1]);
				JgInfo.sCutAngParaEX.Append(cut_para_str,'+');
			}
			if(pJg->cut_angle[3][0]>0&&pJg->cut_angle[3][1]>0&&pJg->cut_wing[1]!=2)
			{	//终端Y肢有切角
				JgInfo.sCutAngEY.Copy(GetMarkSymbol());
				if(pJg->cut_angle[3][0]==pJg->cut_angle[3][1]&&pJg->cut_angle[3][1]==(pJg->GetWidth()-pJg->GetThick()))
					cut_para_str.Printf("CO45%%%%D");
				else
					cut_para_str.Printf("CT%.0fX%.0f",pJg->cut_angle[3][0],pJg->cut_angle[3][1]);
				JgInfo.sCutAngParaEY.Append(cut_para_str,'+');
			}
		}
#endif
		
		if(pJg->pPart&&pJg->pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
		if(((CLDSLineAngle*)pJg->pPart)->start_push_flat_x1_y2>0)
		{
			if(((CLDSLineAngle*)pJg->pPart)->start_push_flat_x1_y2==1)
			{	//始端X肢压扁
				JgInfo.sPushFlatParaSX.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
#ifdef AFX_TARG_ENU_ENGLISH
				JgInfo.sPushFlat.Printf("starting-X/%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
#else
				JgInfo.sPushFlat.Printf("始-X/%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
#endif
			}
			else
			{	//始端Y肢压扁
				JgInfo.sPushFlatParaSY.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
#ifdef AFX_TARG_ENU_ENGLISH
				JgInfo.sPushFlat.Printf("starting-Y/%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
#else
				JgInfo.sPushFlat.Printf("始-Y/%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
#endif
			}
		}
		
		if(((CLDSLineAngle*)pJg->pPart)->end_push_flat_x1_y2>0)
		{
			if(((CLDSLineAngle*)pJg->pPart)->end_push_flat_x1_y2==1)
			{	//终端X肢压扁
				JgInfo.sPushFlatParaEX.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
#ifdef AFX_TARG_ENU_ENGLISH
				JgInfo.sPushFlat.Printf("ending-X/%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
#else
				JgInfo.sPushFlat.Printf("终-X/%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
#endif
			}
			else
			{	//终端Y肢压扁
				JgInfo.sPushFlatParaEY.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
#ifdef AFX_TARG_ENU_ENGLISH
				JgInfo.sPushFlat.Printf("ending-Y/%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
#else
				JgInfo.sPushFlat.Printf("终-Y/%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
#endif
			}
		}
		if(((CLDSLineAngle*)pJg->pPart)->mid_push_flat_x1_y2>0)
		{
			if(((CLDSLineAngle*)pJg->pPart)->mid_push_flat_x1_y2==1)
			{	//中间X肢压扁
				JgInfo.sPushFlatParaMX.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
#ifdef AFX_TARG_ENU_ENGLISH
				JgInfo.sPushFlat.Printf("middle-X/%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
#else
				JgInfo.sPushFlat.Printf("中-X/%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
#endif
			}
			else
			{	//中间Y肢压扁
				JgInfo.sPushFlatParaMY.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
#ifdef AFX_TARG_ENU_ENGLISH
				JgInfo.sPushFlat.Printf("middle-Y/%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
#else
				JgInfo.sPushFlat.Printf("中-Y/%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
#endif
			}
		}
		}
		else if(pJg->pPart==NULL)
		{
			if(pJg->start_push_pos==0&&pJg->end_push_pos>0)
			{
				if(pJg->wing_push_X1_Y2==1)
				{	//始端X肢压扁
					JgInfo.sPushFlatParaSX.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#ifdef AFX_TARG_ENU_ENGLISH
					JgInfo.sPushFlat.Printf("starting-X/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#else
					JgInfo.sPushFlat.Printf("始-X/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#endif
				}
				else if(pJg->wing_push_X1_Y2==2)
				{	//始端Y肢压扁
					JgInfo.sPushFlatParaSY.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#ifdef AFX_TARG_ENU_ENGLISH
					JgInfo.sPushFlat.Printf("starting-Y/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#else
					JgInfo.sPushFlat.Printf("始-Y/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#endif
				}
			}
			
			if(pJg->end_push_pos>=pJg->GetLength())
			{
				if(pJg->wing_push_X1_Y2==1)
				{	//终端X肢压扁
					JgInfo.sPushFlatParaEX.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#ifdef AFX_TARG_ENU_ENGLISH
					JgInfo.sPushFlat.Printf("ending-X/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#else
					JgInfo.sPushFlat.Printf("终-X/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#endif
				}
				else if(pJg->wing_push_X1_Y2==2)
				{	//终端Y肢压扁
					JgInfo.sPushFlatParaEY.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#ifdef AFX_TARG_ENU_ENGLISH
					JgInfo.sPushFlat.Printf("ending-Y/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#else
					JgInfo.sPushFlat.Printf("终-Y/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#endif
				}
			}
			if(pJg->start_push_pos>0 && pJg->end_push_pos>pJg->start_push_pos && pJg->end_push_pos<pJg->GetLength())
			{	//
				if(pJg->wing_push_X1_Y2==1)
				{	//中间X肢压扁
					JgInfo.sPushFlatParaMX.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#ifdef AFX_TARG_ENU_ENGLISH
					JgInfo.sPushFlat.Printf("middle-X/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#else
					JgInfo.sPushFlat.Printf("中-X/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#endif
				}
				else if(pJg->wing_push_X1_Y2==2)
				{	//中间Y肢压扁
					JgInfo.sPushFlatParaMY.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#ifdef AFX_TARG_ENU_ENGLISH
					JgInfo.sPushFlat.Printf("middle-Y/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#else
					JgInfo.sPushFlat.Printf("中-Y/%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
#endif
				}
			}
		}
		draw_rect = InsertJgProcessCardTbl(pBlockTableRecord,origin,&JgInfo);
		if(sys.general_set.bHasFrame)
			InsertBlock(pBlockTableRecord,origin,"frame",1,0);
		if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
		{
			CXhChar50 sDimText;
			f3dPoint base_pos,dim_pos;
			double fTextSize=sys.fonts.angle.fDimTextSize;
			double fLeverMargin=fTextSize+3,fVertMargin=fTextSize+3;
			double fMaxVerMargin=(strstr(sys.general_set.m_sJgCard,"二"))?fVertMargin+5:fVertMargin;//角钢工艺卡二右上角多出一个数据区
			//初始化工艺卡区域特征位置
			f3dPoint leftTopPt,leftBtmPt,rightTopPt,rightBtmPt,midTopPT,midBtmPt;
			leftTopPt.Set(draw_rect.topLeft.x,draw_rect.topLeft.y,0);
			leftBtmPt.Set(draw_rect.topLeft.x,draw_rect.bottomRight.y);
			rightTopPt.Set(draw_rect.bottomRight.x,draw_rect.topLeft.y,0);
			rightBtmPt.Set(draw_rect.bottomRight.x,draw_rect.bottomRight.y,0);
			midTopPT=(leftTopPt+rightTopPt)*0.5;
			midBtmPt=(leftBtmPt+rightBtmPt)*0.5;
			origin.x = draw_rect.topLeft.x;
			origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
			//-----------VVVVVVV-------王海涛添加的切角图示
			if(sys.part_map.angle.bDimCutAngleMap)
			{
				double fBlockL=18,fBlockW=10;	//插入块的长和高
				//起始端切肢/切角标注
				double fDimOffX=5;	//从块原点(左下)到数据标注的水平偏移距
				if(pJg->cut_wing[0]>0)
				{
					if(pJg->cut_wing[0]==1)	//X肢为切肢主肢
					{	//表示切角/切肢的图标 长=18,宽=10	(左下角)
						base_pos.Set(leftBtmPt.x+fLeverMargin,leftBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"CutWingSX",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);		//块上侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[0][0]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);	//块下侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[0][2]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						if(pJg->cut_wing_para[0][1]>0)	//Y肢切角
						{	//(左上角)
							base_pos.Set(leftTopPt.x+fLeverMargin,leftTopPt.y-fVertMargin-fBlockW,0);
							InsertBlock(pBlockTableRecord,base_pos,"CutAngleSY1",1,0);
							dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);//块下侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[0][0]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
							dim_pos.Set(base_pos.x-1,base_pos.y+fBlockW/2);			 //块左侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[0][1]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
						}
					}
					else	//Y肢为切肢主肢
					{	//(左上角)
						base_pos.Set(leftTopPt.x+fLeverMargin,leftTopPt.y-fVertMargin-fBlockW,0);
						InsertBlock(pBlockTableRecord,base_pos,"CutWingSY",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);	//块下侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[0][0]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);		//块上侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[0][2]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						if(pJg->cut_wing_para[0][1]>0)	//X肢切角
						{	//(左下角)
							base_pos.Set(leftBtmPt.x+fLeverMargin,leftBtmPt.y+fVertMargin,0);
							InsertBlock(pBlockTableRecord,base_pos,"CutAngleSX1",1,0);
							dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);	//块上侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[0][0]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
							dim_pos.Set(base_pos.x-1,base_pos.y+fBlockW/2);			//块左侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[0][1]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
						}
					} 
				}
				if(pJg->cut_angle[0][0]>0)	//起始端X肢切角
				{	//(左下角)
					base_pos.Set(leftBtmPt.x+fLeverMargin,leftBtmPt.y+fVertMargin,0);
					InsertBlock(pBlockTableRecord,base_pos,"CutAngleSX",1,0);
					dim_pos.Set(base_pos.x-1,base_pos.y+fBlockW/2);//块左侧
					sDimText.Printf("%.0f",pJg->cut_angle[0][1]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
					dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);//块下侧
					sDimText.Printf("%.0f",pJg->cut_angle[0][0]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
				}
				if(pJg->cut_angle[1][0]>0)	//起始端Y肢切角
				{	//(左上角)
					base_pos.Set(leftTopPt.x+fLeverMargin,draw_rect.topLeft.y-fVertMargin-fBlockW,0);
					InsertBlock(pBlockTableRecord,base_pos,"CutAngleSY",1,0);
					dim_pos.Set(base_pos.x-1,base_pos.y+fBlockW/2);//块左侧
					sDimText.Printf("%.0f",pJg->cut_angle[1][1]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
					dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);//块上侧
					sDimText.Printf("%.0f",pJg->cut_angle[1][0]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
				}
				//终止端切肢/切角标注
				fDimOffX=13;	//从块原点(左下)到数据标注的水平偏移距
				if(pJg->cut_wing[1]>0)
				{
					if(pJg->cut_wing[1]==1)	//X肢为切肢主肢
					{	//(右下角)
						base_pos.Set(rightBtmPt.x-fLeverMargin-fBlockL,rightBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"CutWingEX",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);//块上侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[1][0]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);//块下侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[1][2]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						if(pJg->cut_wing_para[1][1]>0)	//Y肢切角
						{	//(右上角)
							base_pos.Set(rightTopPt.x-fLeverMargin-fBlockL,rightTopPt.y-fBlockW-fMaxVerMargin,0);
							InsertBlock(pBlockTableRecord,base_pos,"CutAngleEY1",1,0);
							dim_pos.Set(base_pos.x+fBlockL+fTextSize+1,base_pos.y+fBlockW/2);//块右侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[1][1]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
							dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);//块下侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[1][0]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						}
					}
					else							//Y肢为切肢主肢
					{	//(右上角)
						base_pos.Set(rightTopPt.x-fLeverMargin-fBlockL,rightTopPt.y-fBlockW-fMaxVerMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"CutWingEY",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);//块下侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[1][0]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);//块上侧
						sDimText.Printf("%.0f",pJg->cut_wing_para[1][2]);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						if(pJg->cut_wing_para[1][1]>0)	//X肢切角
						{	//(右下角)
							base_pos.Set(rightBtmPt.x-fLeverMargin-fBlockL,rightBtmPt.y+fVertMargin,0);
							InsertBlock(pBlockTableRecord,base_pos,"CutAngleEX1",1,0);
							dim_pos.Set(base_pos.x+fBlockL+fTextSize+1,base_pos.y+fBlockW/2);//块右侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[1][1]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
							dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);//块上侧
							sDimText.Printf("%.0f",pJg->cut_wing_para[1][0]);
							DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						}
					}
				}
				if(pJg->cut_angle[2][0]>0)	//终止端X肢切角
				{	//(右下角)
					base_pos.Set(rightBtmPt.x-fLeverMargin-fBlockL,rightBtmPt.y+fVertMargin,0);
					InsertBlock(pBlockTableRecord,base_pos,"CutAngleEX",1,0);
					dim_pos.Set(base_pos.x+fBlockL+fTextSize+1,base_pos.y+fBlockW/2);//块右侧
					sDimText.Printf("%.0f",pJg->cut_angle[2][1]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
					dim_pos.Set(base_pos.x+fDimOffX,base_pos.y-fTextSize-1);//块下侧
					sDimText.Printf("%.0f",pJg->cut_angle[2][0]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
				}
				if(pJg->cut_angle[3][0]>0)	//终止端Y肢切角
				{	//(右上角)
					base_pos.Set(rightTopPt.x-fLeverMargin-fBlockL,rightTopPt.y-fBlockW-fMaxVerMargin,0);
					InsertBlock(pBlockTableRecord,base_pos,"CutAngleEY",1,0);
					dim_pos.Set(base_pos.x+fBlockL+fTextSize+1,base_pos.y+fBlockW/2);//块右侧
					sDimText.Printf("%.0f",pJg->cut_angle[3][1]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,Pi*0.5,AcDb::kTextCenter);
					dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW+1);//块上侧
					sDimText.Printf("%.0f",pJg->cut_angle[3][0]);
					DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
				}
			}
			//----------^^^^^^^^^---------

			//----------^^^^^^^^^---------压扁图示
			if(sys.part_map.angle.bDimPushFlatMap)
			{	 
				double fBlockL=17,fBlockW=8;		//始终端压扁块 长=17,宽=8
				double fDimOffX=2.5,fDimOffY=2;		//始终端压扁 压扁长度标注的X偏移值，Y偏移值 
				double fBlockMidL=32,fBlockMidW=9;	//中间压扁块 长=32,宽=9
				double fMidXDimX1=15,fMidXDimY1=11,fMidXDimX2=11,fMidXDimY2=4;	//X肢中间压扁长度标注和向上偏移标注的X偏移值，Y偏移值
				double fMidYDimX1=15,fMidYDimY1=2,fMidYDimX2=11,fMidYDimY2=5;	//Y肢中间压扁长度标注和向上偏移标注的X偏移值，Y偏移值
				if(pJg->pPart&&pJg->pPart->GetClassTypeId()==CLS_LINEANGLE)
				{
					if(((CLDSLineAngle*)pJg->pPart)->start_push_flat_x1_y2==1)
					{	//始端X肢压扁(左下角)
						base_pos.Set(leftBtmPt.x+fLeverMargin,leftBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatSX",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW-fDimOffY-fTextSize-1);//块下侧
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					else if(((CLDSLineAngle*)pJg->pPart)->start_push_flat_x1_y2==2)
					{	//始端Y肢压扁(左上角)
						base_pos.Set(leftTopPt.x+fLeverMargin,leftTopPt.y-fVertMargin-fBlockW,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatSY",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fDimOffY+1);//块上侧
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->start_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					if(((CLDSLineAngle*)pJg->pPart)->end_push_flat_x1_y2==1)
					{	//终端X肢压扁(右下角)
						base_pos.Set(rightBtmPt.x-fBlockL-fLeverMargin,rightBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatEX",1,0);
						dim_pos.Set(base_pos.x+fBlockL-fDimOffX,base_pos.y+fBlockW-fDimOffY-fTextSize-1);//块下侧
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					else if(((CLDSLineAngle*)pJg->pPart)->end_push_flat_x1_y2==2)
					{	//终端Y肢压扁(右上角)
						base_pos.Set(rightTopPt.x-fBlockL-fLeverMargin,rightTopPt.y-fMaxVerMargin-fBlockW,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatEY",1,0);
						dim_pos.Set(base_pos.x+fBlockL-fDimOffX,base_pos.y+fDimOffY+1);//块上侧
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->end_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					if(((CLDSLineAngle*)pJg->pPart)->mid_push_flat_x1_y2==1)
					{	//中间X肢压扁
						base_pos.Set(midBtmPt.x-fBlockMidL/2,midBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"MidPushFlatX",1,0);
						dim_pos.Set(base_pos.x+fMidXDimX1,base_pos.y+fMidXDimY1+1);//块上侧（上偏移距离）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fMidXDimX2,base_pos.y+fMidXDimY2-fTextSize-1);//块下侧（压扁长度）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_start_offset);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					else if(((CLDSLineAngle*)pJg->pPart)->mid_push_flat_x1_y2==2)
					{   //中间Y肢压扁
						base_pos.Set(midTopPT.x-fBlockMidL/2,midTopPT.y-fVertMargin-fBlockMidW,0);
						InsertBlock(pBlockTableRecord,base_pos,"MidPushFlatY",1,0);
						dim_pos.Set(base_pos.x+fMidYDimX1,base_pos.y-fMidYDimY1-fTextSize-1);//块下侧（压扁长度）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fMidYDimX2,base_pos.y+fMidYDimY2+1);//块上侧（上偏移距离）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_start_offset);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
				}
				else if(pJg->pPart==NULL)
				{
					if(pJg->wing_push_X1_Y2==1&&pJg->start_push_pos==0&&pJg->end_push_pos>0)
					{	//始端X肢压扁
						base_pos.Set(leftBtmPt.x+fLeverMargin,leftBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatSX",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fBlockW-fDimOffY-fTextSize-1);//块下侧
						sDimText.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					else if(pJg->wing_push_X1_Y2==2&&pJg->start_push_pos==0&&pJg->end_push_pos>0)
					{	//始端Y肢压扁
						base_pos.Set(leftTopPt.x+fLeverMargin,leftTopPt.y-fVertMargin-fBlockW,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatSY",1,0);
						dim_pos.Set(base_pos.x+fDimOffX,base_pos.y+fDimOffY+1);//块上侧
						sDimText.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					if(pJg->wing_push_X1_Y2==1&&pJg->start_push_pos>=0&&pJg->end_push_pos>=pJg->GetLength())
					{	//终端X肢压扁
						base_pos.Set(rightBtmPt.x-fBlockL-fLeverMargin,rightBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatEX",1,0);
						dim_pos.Set(base_pos.x+fBlockL-fDimOffX,base_pos.y+fBlockW-fDimOffY-fTextSize-1);//块下侧
						sDimText.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					else if(pJg->wing_push_X1_Y2==2&&pJg->start_push_pos>=0&&pJg->end_push_pos>=pJg->GetLength())
					{	//终端Y肢压扁
						base_pos.Set(rightTopPt.x-fBlockL-fLeverMargin,rightTopPt.y-fMaxVerMargin-fBlockW,0);
						InsertBlock(pBlockTableRecord,base_pos,"PushFlatEY",1,0);
						dim_pos.Set(base_pos.x+fBlockL-fDimOffX,base_pos.y+fDimOffY+1);//块上侧
						sDimText.Printf("%.0f",(float)(pJg->end_push_pos-pJg->start_push_pos));
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					if(((CLDSLineAngle*)pJg->pPart)->mid_push_flat_x1_y2==1)
					{	//中间X肢压扁
						base_pos.Set(midBtmPt.x-fBlockMidL/2,midBtmPt.y+fVertMargin,0);
						InsertBlock(pBlockTableRecord,base_pos,"MidPushFlatX",1,0);
						dim_pos.Set(base_pos.x+fMidXDimX2,base_pos.y+fMidXDimY2+1);//块上侧（上偏移距离）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fMidXDimX1,base_pos.y+fMidXDimY1-fTextSize-1);//块下侧（压扁长度）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_start_offset);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
					else if(((CLDSLineAngle*)pJg->pPart)->mid_push_flat_x1_y2==2)
					{	//中间Y肢压扁
						base_pos.Set(midTopPT.x-fBlockMidL/2,midTopPT.y-fVertMargin-fBlockMidW,0);
						InsertBlock(pBlockTableRecord,base_pos,"MidPushFlatY",1,0);
						dim_pos.Set(base_pos.x+fMidYDimX1,base_pos.y-fMidYDimY1-fTextSize);//块下侧（压扁长度）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_length);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
						dim_pos.Set(base_pos.x+fMidYDimX2,base_pos.y+fMidYDimY2+1);//块上侧（上偏移距离）
						sDimText.Printf("%.0f",((CLDSLineAngle*)pJg->pPart)->mid_push_flat_start_offset);
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fDimTextSize,0,AcDb::kTextCenter);
					}
				}
			}
			//----------^^^^^^^^^---------
			//对于定制短角钢,如果有清根或铲背工艺，需在工艺卡中添加特殊文字标注
			if(pJg->pPart&&pJg->pPart->IsAngle())
			{
				CLDSLineAngle* pAngle=(CLDSLineAngle*)pJg->pPart;
				if(pAngle->pStart==NULL || pAngle->pEnd==NULL)
				{
					f3dPoint dim_pos;
					CXhChar50 sDimText;
					if(pAngle->m_bCutRoot)
					{	//刨根
						dim_pos.Set(draw_rect.topLeft.x+draw_rect.Width()/2,draw_rect.bottomRight.y+20,0);
						strcpy(sDimText,"全长清根");
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fPartNoTextSize,0,AcDb::kTextCenter);
					}
					else if(pAngle->m_bCutBer)
					{	//铲背
						dim_pos.Set(draw_rect.topLeft.x+draw_rect.Width()/2,draw_rect.bottomRight.y+20,0);
						strcpy(sDimText,"全长铲背");
						DimText(pBlockTableRecord,dim_pos,sDimText,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fPartNoTextSize,0,AcDb::kTextCenter);
					}
				}
			}
		}
		pBlockTableRecord->close();
	}
	else
	{
		f2dRect draw_rect=GetProcessCardDrawingRect();
		if(!sys.general_set.bProcessCardTbl&&pDrawRect)
		{
			draw_rect=*pDrawRect;	//未启用工艺卡图框且外界指定绘图区域 wht 11-06-02
			if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
			{
				origin.x = draw_rect.topLeft.x;
				origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
			}
		}
		if(pJgTypePart->pPart&&pJgTypePart->pPart->GetClassTypeId()==CLS_LINEANGLE)
			xJgDraw.CreateLineAngleDrawing((CLDSLineAngle*)pJgTypePart->pPart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
		else if (pJgTypePart->pPart==NULL&&pJgTypePart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
			xJgDraw.CreateLineAngleDrawing((CProcessAngle*)pJgTypePart,draw_rect.Width(),draw_rect.Height(),sys.fonts.angle.fDimTextSize);//sys.dim_map.fDimTextSize);
	}

	if(!DrawJgMap(&xJgDraw,origin))
		return 1;	//虽然输出失败，但也算是处理过了，所以返回1

	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(!sys.general_set.bProcessCardTbl&&bDimPartInfo)
	{
		CXhChar100 sNotes;
		f3dPoint base;
		if(length-(int)length>EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			sNotes.Printf("tower type-%s part number. %s*%d piece*%d fundation  %s length=%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("Tower type-%s part number. %s*%d piece*%d foundation  %s length =%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基  %s 长度=%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基  %s 长度=%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#endif
		
		origin.x += sys.general_set.nColumnWidth/10;
		origin.y -=250.0/sys.general_set.PartMapScale.fRealToDraw;
		if(pDrawRect)
		{	//根据指定的矩形框,计算备注标注位置 wht 11-06-02
			origin.x = sNotes.GetLength()*sys.fonts.angle.fPartNoTextSize*0.5;
			origin.y = sys.fonts.angle.fPartNoTextSize*5;
		}
		base=origin;
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.fonts.angle.fPartNoTextSize,0);
		pBlockTableRecord->close();
	}
	return 1;
}

extern char g_sTaskNo[101];
extern char g_sTolerance[101];
static CXhChar100 GetPropValueByName(PART_PLANK_CARD_INFO &plateInfo,const char* sPropName)
{
	CXhChar100 sTemp;
#ifdef AFX_TARG_ENU_ENGLISH
	if(stricmp(sPropName,"Project Name")==0)
#else
	if(stricmp(sPropName,"工程名称")==0)
#endif
		sTemp.Printf("%s",Ta.m_sPrjName);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Tower Type Code")==0)
#else
	else if(stricmp(sPropName,"塔型代号")==0)
#endif
		sTemp.Printf("%s",Ta.m_sTaAlias);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Material")==0)
#else
	else if(stricmp(sPropName,"材质")==0)
#endif
		sTemp.Copy(plateInfo.sDesMaterial);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Part No.")==0)
#else
	else if(stricmp(sPropName,"件号")==0)
#endif
		sTemp.Printf("%s",(char*)plateInfo.sPartNo);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Number Of Parts")==0)
		sTemp.Printf("%s pieces",(char*)plateInfo.sPartNum);
	else if(stricmp(sPropName,"Thickness")==0)
#else
	else if(stricmp(sPropName,"件数")==0)
		sTemp.Printf("%s",(char*)plateInfo.sPartNum);
	else if(stricmp(sPropName,"厚度")==0)
#endif
		sTemp.Printf("-%s",(char*)plateInfo.sThick);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Area")==0)
		sTemp.Printf("area:%s(mm2)",(char*)plateInfo.sArea);
	else if(stricmp(sPropName,"Piece Weight")==0)
		sTemp.Printf("piece weight:%s(kg)",(char*)plateInfo.sPartWeight);
	else if(stricmp(sPropName,"Total Weight")==0)
		sTemp.Printf("total weight:%s(kg)",(char*)plateInfo.sSumPartWeight);
	else if(stricmp(sPropName,"Tower Type")==0)
#else
	else if(stricmp(sPropName,"面积")==0)
		sTemp.Printf("面积:%s(mm2)",(char*)plateInfo.sArea);
	else if(stricmp(sPropName,"单重")==0)
		sTemp.Printf("单重:%s(kg)",(char*)plateInfo.sPartWeight);
	else if(stricmp(sPropName,"总重")==0)
		sTemp.Printf("总重:%s(kg)",(char*)plateInfo.sSumPartWeight);
	else if(stricmp(sPropName,"塔型")==0)
#endif
		sTemp.Printf("%s ",(char*)Ta.m_sTaType);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Spec")==0)
#else
	else if(stricmp(sPropName,"规格")==0)
#endif
		sTemp.Printf("%s ",(char*)plateInfo.sGuiGeIncMat);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Stamp No.")==0)
#else
	else if(stricmp(sPropName,"钢印号")==0)
#endif
		sTemp.Printf("%s ",Ta.m_sTaStampNo);
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Number")==0)
		sTemp.Printf("%d foundation ",plateInfo.m_nTaNum);
	else if(stricmp(sPropName,"Number Of Bolts")==0)
#else
	else if(stricmp(sPropName,"基数")==0)
		sTemp.Printf("%d ",plateInfo.m_nTaNum);
	else if(stricmp(sPropName,"螺栓数")==0)
#endif
	{	//孔径X孔数
		sTemp.Empty();
		if(plateInfo.sM12HoleNum.GetLength()>0)
			sTemp.Append(CXhChar16("Ф13.5x%s",(char*)plateInfo.sM12HoleNum));
		if(plateInfo.sM16HoleNum.GetLength()>0)
			sTemp.Append(CXhChar16("Ф17.5x%s",(char*)plateInfo.sM16HoleNum));
		if(plateInfo.sM18HoleNum.GetLength()>0)
			sTemp.Append(CXhChar16("Ф19.5x%s",(char*)plateInfo.sM18HoleNum));
		if(plateInfo.sM20HoleNum.GetLength()>0)
			sTemp.Append(CXhChar16("Ф21.5x%s",(char*)plateInfo.sM20HoleNum));
		if(plateInfo.sM22HoleNum.GetLength()>0)
			sTemp.Append(CXhChar16("Ф23.5x%s",(char*)plateInfo.sM22HoleNum));
		if(plateInfo.sM24HoleNum.GetLength()>0)
			sTemp.Append(CXhChar16("Ф25.5x%s",(char*)plateInfo.sM24HoleNum));
		if(plateInfo.sMXHoleNum.GetLength()>0)
			sTemp.Append(CXhChar16("其它:%s",(char*)plateInfo.sMXHoleNum));
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(stricmp(sPropName,"Number Of Edges")==0)
		sTemp.Printf("%s",(char*)plateInfo.sEdgeNum);
	else if (stricmp(sPropName,"Welding Edge Length")==0)
		sTemp.Printf("%s",(char*)plateInfo.sWeldLength);
#else
	else if(stricmp(sPropName,"边数")==0)
		sTemp.Printf("%s",(char*)plateInfo.sEdgeNum);
	else if (stricmp(sPropName,"焊缝边长")==0)
		sTemp.Printf("%s",(char*)plateInfo.sWeldLength);
	else if(stricmp(sPropName,"总件数")==0)
		sTemp.Copy(plateInfo.sSumPartNum);
	else if(stricmp(sPropName,"焊接")==0)
		sTemp.Copy(plateInfo.sWeld);
	else if(stricmp(sPropName,"制弯")==0)
		sTemp.Copy(plateInfo.sHuoQuFst);
	else if(stricmp(sPropName,"试组装")==0)
		sTemp.Copy("√");
	else if(stricmp(sPropName,"公差")==0)
		sTemp.Copy(g_sTolerance);
	else if(stricmp(sPropName,"任务单号")==0)
		sTemp.Copy(g_sTaskNo);
#endif
	return sTemp;
}

static	RECT DrawPlateInfoTbl(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint pos,
							  ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> *pTblRowList=NULL,
							  PART_PLANK_CARD_INFO *pPlateInfo=NULL,double zoomCoef=1.0)
{
	CFormatTbl xPartTbl;
	if(pTblRowList&&pTblRowList->GetNodeNum()>0)
	{
		xPartTbl.Create(pTblRowList->GetNodeNum(),2);
		int i=0;
		double fColWidth1=0,fColWidth2=0;
		CPlankPartParaExportDlg::TABLE_ROW *pRow=NULL;
		double txt_size=sys.dim_map.PartNo.fPartNoTextSize;
		for(pRow=pTblRowList->GetFirst();pRow;pRow=pTblRowList->GetNext())
		{
			xPartTbl.SetRowHigh(i++,sys.dim_map.PartNo.fPartNoTextSize+1);
			double fWidth=TestDrawTextLength(pRow->sDisplayName,txt_size,TextStyleTable::hzfs.textStyleId);
			if(fColWidth1<fWidth)
				fColWidth1=fWidth;
			fWidth=TestDrawTextLength(GetPropValueByName(*pPlateInfo,pRow->sRowName),txt_size,TextStyleTable::hzfs.textStyleId);
			if(fColWidth2<fWidth)
				fColWidth2=fWidth;
		}
		xPartTbl.SetColumnWide(0,min(fColWidth1+1,sys.part_map.plate.nPartInfoTblColumnWidth));
		xPartTbl.SetColumnWide(1,min(fColWidth2+1,sys.part_map.plate.nPartInfoTblColumnWidth));
	}
	RECT rect;
	rect.left=(long)pos.x;
	rect.top=(long)pos.y;
	rect.bottom=rect.top+(long)xPartTbl.GetTableHeight();
	rect.right=rect.left+(long)xPartTbl.GetTableWide();
	if(pPlateInfo==NULL||pTblRowList==NULL)
		return rect;
	DRAGSET.BeginBlockRecord();
	CPlankPartParaExportDlg::TABLE_ROW *pRow=NULL;
	int i=0;
	for(pRow=pTblRowList->GetFirst();pRow;pRow=pTblRowList->GetNext())
	{
		xPartTbl.GetGridAt(i,0)->data.SetGridString((char*)pRow->sDisplayName);
		xPartTbl.GetGridAt(i,0)->feature=TEXTALIGN_LEFT_CENTER;
		xPartTbl.GetGridAt(i,0)->textsize=sys.dim_map.PartNo.fPartNoTextSize;
		//
		xPartTbl.GetGridAt(i,1)->data.SetGridString((char*)GetPropValueByName(*pPlateInfo,pRow->sRowName));
		xPartTbl.GetGridAt(i,1)->feature=TEXTALIGN_LEFT_CENTER;
		xPartTbl.GetGridAt(i,1)->textsize=sys.dim_map.PartNo.fPartNoTextSize;
		i++;
	}
	xPartTbl.SetBasePos(f2dPoint(0,0));
	DrawTable(DRAGSET.RecordingBlockTableRecord(),&xPartTbl,FALSE,1,-1,0.5);
	DRAGSET.EndBlockRecord(pBlockTableRecord,pos,zoomCoef);
	return rect;
}
struct BOLT_ITEM
{
	double fDiameter;
	int nNum;
	BOLT_ITEM(){memset(this,0,sizeof(BOLT_ITEM));}
};
BOOL IsHasArcEdge(CLDSGeneralPlate* pPlate)
{
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(pVertex->type>0)
			return TRUE;
	}
	return FALSE;
}
BOOL IsHasRollEdge(CLDSGeneralPlate* pPlate)
{
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(pVertex->m_bRollEdge&&abs(pVertex->manu_space)>0)
			return TRUE;
	}
	return FALSE;
}
//识别钢板是否有焊接关系
bool IdentifyWeldPlate(CLDSGeneralPlate* pGeneralPlate)
{
	if(pGeneralPlate==NULL)
		return FALSE;
	//判断是否为焊接子构件（焊接于其他构件上）
	CLDSPart *pFatherPart=Ta.FromPartHandle(pGeneralPlate->m_hPartWeldParent);
	bool bWeldPlate=pFatherPart?true:false;
	if(!bWeldPlate)
	{	//是否有焊接边
		PROFILE_VERTEX* pVertex=NULL,*pPrevVertex=pGeneralPlate->vertex_list.GetTail();
		for(pVertex=pGeneralPlate->vertex_list.GetFirst();pVertex;pVertex=pGeneralPlate->vertex_list.GetNext())
		{
			if(bWeldPlate=pPrevVertex->m_bWeldEdge)
				break;
			pPrevVertex=pVertex;
		}
	}
	//判断是否为焊接父构件
	if(bWeldPlate==FALSE)
	{
		for(CLDSPart *pSonPart=Ta.EnumPartFirst();pSonPart;pSonPart=Ta.EnumPartNext())
		{
			if(pSonPart->m_hPartWeldParent==pGeneralPlate->handle)
				bWeldPlate=true;
			else if(pGeneralPlate->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate* pPlate=(CLDSPlate*)pGeneralPlate;
				bWeldPlate=pPlate->m_bWeldPart?true:false;
			}
			else if(pGeneralPlate->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pSonPart;
				long hDatumPart=0,hAssPart1=0;
				if(pParamPlate->m_iParamType==TYPE_RIBPLATE)
				{
					pParamPlate->GetDesignItemValue('A',&hDatumPart);//基准构件
					pParamPlate->GetDesignItemValue('C',&hAssPart1); //辅助构件1
				}
				else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
				{
					pParamPlate->GetDesignItemValue('C',&hDatumPart);//基准构件
					pParamPlate->GetDesignItemValue('G',&hAssPart1); //辅助构件1
				}
				else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
				{
					pParamPlate->GetDesignItemValue('L',&hDatumPart);//基准构件
				}
				else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				{
					CLDSGeneralPlate::DESIGN_PARAM itemparam;
					pParamPlate->GetDesignItemValue('E',&itemparam);
					if(itemparam.cValueType==3)	//句柄+数值
						hDatumPart=itemparam.value.hPart;
					else
						hDatumPart=itemparam.ToLong();
					//
					pParamPlate->GetDesignItemValue('F',&itemparam);
					if(itemparam.cValueType==3)	//句柄+数值
						hAssPart1=itemparam.value.hPart;
					else 
						hAssPart1=itemparam.ToLong();
				}
				if(abs(hDatumPart)==pGeneralPlate->handle || abs(hAssPart1)==pGeneralPlate->handle)
					bWeldPlate=true;
			}
			if(bWeldPlate)
				break;
		}
	}
	return bWeldPlate;
}
//this function from SegMatTable.cpp
SCOPE_STRU GetRectFromPoly(ATOM_LIST<PROFILE_VERTEX> &vertex_list);
double _LocalRoundingDrawScale(double scaleOfModel2Draw);
void DrawPlankTypePartMap(CLDSPart *pPlankTypePart,char *export_info,BOOL *para,int iOrder,
						  int nHits,BOOL bInProcessCard/*=TRUE*/,BOOL bPatternDrawing/*=TRUE*/,
						  int nPageNum/*=1*/,int nTaNum/*=1*/,f3dPoint *pOrigin/*=NULL*/,BOOL bDimPartInfo/*=TRUE*/,
						  ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> *pTblRowList/*=NULL*/)
{
	int nEdgeNum = ((CLDSGeneralPlate*)pPlankTypePart)->GetEdgeNum(1);	//长度大于1mm的边才算作一个切割边，否则估计到时候也会一刀切掉
	double fWeldLength = ((CLDSGeneralPlate*)pPlankTypePart)->CalWeldEdgeLength();
	int i,n;
	f3dLine line;
	f3dArcLine arcline;
	f3dCircle Cir;
	TXTDIM_STRU txt_dim;
	ANGULARDIM_STRU angular_dim;
	DIAMETERDIM_STRU diam_dim;
	ROTATEDIM_STRU align_dim,rotate_dim;
	CPlankDrawing xPlankDraw;
	f3dPoint origin;	//绘图原点
	double zoomCoef=1.0/sys.general_set.PartMapScale.fRealToDraw;
	if(bPatternDrawing)	//1:1钢板大样图不需要工艺卡图框来回设很麻烦 wjh-2015.2.3
		bInProcessCard=FALSE;
	//设置钢板的标注样式
	if(sys.part_map.plate.bDimLsToWeldEdge)
		xPlankDraw.dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToWeldEdge);
	if(sys.part_map.plate.bDimLsToHuoquLine)
		xPlankDraw.dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToHuoQuLine);
	if(sys.part_map.plate.bDimDiffRayLsSpace)
		xPlankDraw.dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToNextLsRay);
	if(sys.part_map.plate.bDimBoltEdgeSpace)
		xPlankDraw.dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToEdgeLine);
	if(sys.part_map.plate.bDimPlateBoltAngle)
		xPlankDraw.dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltAngle);
	//根据钢板长度与宽度中较小值确定钢板上标注字高缩放系数 
	double dim_txt_size=sys.fonts.plate.fDimTextSize;		//长度尺寸标注文本高
	double partno_txt_size=sys.fonts.plate.fPartNoTextSize;	//构件编号文字高
	double dim_angle_size=sys.fonts.plate.fDimAngleTextSize;//角度尺寸标注文本高
	//double dim_arrow_size=sys.dim_map.fDimArrowSize;		//箭头大小
	if(sys.part_map.plate.bZoomDimTextSizeByWidth)
	{	//默认宽度与字高比例为3:100 即宽度为100时 字高为3	wht 10-02-23
		double plate_width=min(pPlankTypePart->GetWidth(),pPlankTypePart->GetLength())*zoomCoef;
		dim_txt_size=fto_halfi(plate_width*sys.part_map.plate.fTextSizeZoomCoef+0.5);		//字体高度
		partno_txt_size=fto_halfi(plate_width*sys.part_map.plate.fTextSizeZoomCoef+0.5);	//字体高度
		dim_angle_size=fto_halfi(plate_width*sys.part_map.plate.fTextSizeZoomCoef+0.5);		//字体高度
	}
	//对标注样式风格进行设置，统一对标注字体高度进行设置
	DimStyleTable::dimStyle.InitDimStyle(dim_txt_size);			//尺寸标注样式
	DimStyleTable::dimStyle3.InitDimStyle(dim_txt_size);		//尺寸标注样式（文字在尺寸线外）
	DimStyleTable::dimRadiusStyle.InitDimStyle(dim_txt_size);	//半径|直径标注样式
	DimStyleTable::dimAngleStyle.InitDimStyle(dim_angle_size);	//角度标注样式
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord=NULL;//定义块表记录指针
	try{
		if(iOrder>0)
		{
			if(sys.general_set.bCardLayoutBaseRow)
			{	//构件图以行为基准进行排列
				origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
				origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
			}
			else
			{	//构件图以列为基准进行排列
				origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
				origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
			}
		}
		else if(pOrigin) //外部指定构件图初始位置(智能绘图时使用) wht 11-06-29
			origin=*pOrigin;
		if(!xPlankDraw.Create(pPlankTypePart,bPatternDrawing))
		{	//钢板生成失败是提示用户出错句柄 wht 10-08-04
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("\nplate's parameter is wrong whose handle is:0X%x,please confirm!\n",pPlankTypePart->handle);
#else
			CXhChar100 sPrompt("\n句柄为:0X%x的钢板参数不正确，请确认!\n",pPlankTypePart->handle);
#endif
#ifdef _ARX_2007
			acedPrompt((ACHAR*)_bstr_t(sPrompt));
#else
			acedPrompt(sPrompt);
#endif
		}
		//-------------------------------------------------------------------------------
		//生成此构件的注释内容
		int thick,wide,length;
		CXhChar50 sPartNo,guige;
		CSuperSmartPtr<CLDSPart>pPlate = pPlankTypePart;
		f2dRect rect;
		double area = 0;
		thick = ftoi(pPlate->GetThick());
		if(pPlate->GetClassTypeId()==CLS_PLATE)
		{
			if(CLDSPlate::m_iPlankWeightStatType==2)
				area = pPlate.PlatePointer()->CalAreaOfNetWrap();
			else
				area = pPlate.PlatePointer()->CalAreaOfMinWrapRect();
		}
		else
			area = pPlate->GetWidth()*pPlate->GetLength();
		wide = ftoi(pPlate->GetWidth());
		length = ftoi(pPlate->GetLength());
		strcpy(sPartNo,GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial));
		CXhChar100 sNotes,sTemp;
		CXhChar16 sMat;
		QuerySteelMatMark(pPlate->cMaterial,sMat);
		//----------------------------------------------------------------------------
		//获得当前图形的块表指针
		AcDbBlockTable *pBlockTable;
		GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
		//以写方式打开模型空间，获得块表记录指针
		pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
		pBlockTable->close();//关闭块表
		PART_PLANK_CARD_INFO PlankInfo;
		int nM12Ls=0,nM16Ls=0,nM18Ls=0,nM20Ls=0,nM22Ls=0,nM24Ls=0,nMXLs=0,nSumLs=0;
		if(pPlate->GetClassTypeId()==CLS_PLATE)
		{
			nM12Ls=pPlate.PlatePointer()->GetLsCount(12);	//M12螺栓孔数
			nM16Ls=pPlate.PlatePointer()->GetLsCount(16);	//M16螺栓孔数
			nM18Ls=pPlate.PlatePointer()->GetLsCount(18);	//M18螺栓孔数
			nM20Ls=pPlate.PlatePointer()->GetLsCount(20);	//M20螺栓孔数
			nM22Ls=pPlate.PlatePointer()->GetLsCount(22);	//M22螺栓孔数
			nM24Ls=pPlate.PlatePointer()->GetLsCount(24);	//M24螺栓孔数
			nMXLs =pPlate.PlatePointer()->GetLsCount()-nM12Ls-nM16Ls-nM18Ls-nM20Ls-nM22Ls-nM24Ls;//非标准螺栓孔数
			nSumLs=pPlate.PlatePointer()->GetLsCount();		//总螺栓孔数
		}
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			nM12Ls=pPlate.ParamPlatePointer()->GetLsCount(12);	//M12螺栓孔数
			nM16Ls=pPlate.ParamPlatePointer()->GetLsCount(16);	//M16螺栓孔数
			nM18Ls=pPlate.ParamPlatePointer()->GetLsCount(18);	//M18螺栓孔数
			nM20Ls=pPlate.ParamPlatePointer()->GetLsCount(20);	//M20螺栓孔数
			nM22Ls=pPlate.ParamPlatePointer()->GetLsCount(22);	//M22螺栓孔数
			nM24Ls=pPlate.ParamPlatePointer()->GetLsCount(24);	//M24螺栓孔数
			nMXLs =pPlate.ParamPlatePointer()->GetLsCount()-nM12Ls-nM16Ls-nM18Ls-nM20Ls-nM22Ls-nM24Ls;//非标准螺栓孔数
			nSumLs=pPlate.ParamPlatePointer()->GetLsCount();	//总螺栓孔数
			if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_UEND)
			{
				nM12Ls*=2;
				nM16Ls*=2;
				nM18Ls*=2;
				nM20Ls*=2;
				nM22Ls*=2;
				nM24Ls*=2;
				nMXLs*=2;
				nSumLs*=2;
			}
		}
		int face_N=1;
		if(pPlate->GetClassTypeId()==CLS_PLATE)
			face_N=pPlate.PlatePointer()->face_N;
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_UEND)
				face_N=2;
			else if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_ROLLEND)
				face_N=3;
			else
				face_N=1;
		}
		if(bInProcessCard||sys.part_map.plate.bDisplaypPartInfoTbl)
		{
			CFlexTbl xPlankProcessCard;
			//f2dRect draw_rect = CreatePlankProcessCardTbl(&xPlankProcessCard,f2dPoint(origin.x,origin.y),sys.part_map.nColumnWidth,sys.part_map.nRowHigh);
			PlankInfo.m_nPageNum=nPageNum;		//总页数
			if(iOrder<0)
				PlankInfo.m_nPageIndex=1;			//第几页
			else
				PlankInfo.m_nPageIndex=iOrder+1;	//第几页
			memset(&PlankInfo,'\0',sizeof(PART_PLANK_CARD_INFO));
			strcpy(PlankInfo.sPartNo,GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial));//构件编号
			PlankInfo.sSeg=pPlate->iSeg.ToString();
			PlankInfo.sLength.Printf("%d",length);//长度
			
			PlankInfo.sPartNum.Printf("%d",nHits);	//单基数
			PlankInfo.sSumPartNum.Printf("%d",nHits*nTaNum);//总加工数
			
			double weight = CalSteelPlankWei(wide*length*thick);
			PlankInfo.sPartWeight.Printf("%.1f",weight);//单件重
			PlankInfo.sSumPartWeight.Printf("%.1f",weight*nHits*nTaNum);	//总重
			PlankInfo.sNotes.Printf("%s",(char*)pPlate->sNotes);//备注
			
			if(nM12Ls>0)
				PlankInfo.sM12HoleNum.Printf("%d",nM12Ls);	//M12螺栓孔数
			if(nM16Ls>0)
				PlankInfo.sM16HoleNum.Printf("%d",nM16Ls);	//M16螺栓孔数
			if(nM18Ls>0)
				PlankInfo.sM18HoleNum.Printf("%d",nM18Ls);	//M18螺栓孔数
			if(nM20Ls>0)
				PlankInfo.sM20HoleNum.Printf("%d",nM20Ls);	//M20螺栓孔数
			if(nM22Ls>0)
				PlankInfo.sM22HoleNum.Printf("%d",nM22Ls);	//M22螺栓孔数
			if(nM24Ls>0)
				PlankInfo.sM24HoleNum.Printf("%d",nM24Ls);	//M24螺栓孔数
			if(nMXLs>0)
				PlankInfo.sMXHoleNum.Printf("%d",nMXLs);	//MX螺栓孔数
			if(nSumLs>0)
				PlankInfo.sSumHoleNum.Printf("%d",nSumLs);	//总螺栓孔数
			//统计钻孔数
			if(gxDrwLocalizer.ciCustomerSerial==5)	
			{	//青岛东方定制,根据特定条件判断钢板加工工艺
				if((pPlate->cMaterial==CSteelMatLibrary::Q235BriefMark()&&thick<=16)||
					(pPlate->cMaterial==CSteelMatLibrary::Q345BriefMark()&&thick<=14)||
					(pPlate->cMaterial==CSteelMatLibrary::Q420BriefMark()&&thick<=12))
					PlankInfo.m_bPunchShear=TRUE;
				else
					PlankInfo.m_bGasCutting=TRUE;
				//判断螺栓孔加工工艺
				if(PlankInfo.m_bPunchShear)	//冲剪
					PlankInfo.sChongKongNum.Copy("1");
				if(PlankInfo.m_bGasCutting)	//气割
					PlankInfo.sZuanKongNum.Copy("1");
			}
			else
			{
				int nLsN=0;
				CLsRefList *pLsRefList=pPlate->GetLsRefList();
				if(pLsRefList)
				{
					for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
					{
						if(pLsRef->cFlag==CLsRef::DRILL_HOLE)
							nLsN++;
					}
				}
				if(nLsN>0)
					PlankInfo.sZuanKongNum.Printf("%d",nLsN);
				int nChongKongNum=nSumLs-nLsN;
				if(nChongKongNum>0)
					PlankInfo.sChongKongNum.Printf("%d",nChongKongNum);
			}
			PlankInfo.sWeldLength.Printf("%.f",fWeldLength);
			PlankInfo.sArea.Printf("%.0f(mm2)",area);
			PlankInfo.sEdgeNum.Printf("%d",nEdgeNum);

			if(pPlate->m_hPartWeldParent>0x20)
				PlankInfo.sWeld.Copy(GetMarkSymbol());
			else
				PlankInfo.sWeld.Empty();

			if(pPlate->GetClassTypeId()==CLS_PLATE&&pPlate.PlatePointer()->m_bNeedFillet)
				PlankInfo.sFillet.Copy(GetMarkSymbol());
			else
				PlankInfo.sFillet.Empty();
			if(IsHasArcEdge(pPlate.GeneralPlatePointer()))
				PlankInfo.sCutArc.Copy(GetMarkSymbol());
			else
				PlankInfo.sCutArc.Empty();
			if(IsHasRollEdge(pPlate.GeneralPlatePointer()))
			{	//一般情况，钢板卷边时，默认有火曲工艺
				PlankInfo.sWrapEdge.Copy(GetMarkSymbol());
				if(gxDrwLocalizer.ciCustomerSerial!=5)	//青岛东方需分开处理
					PlankInfo.sHuoQuFst.Copy(GetMarkSymbol());
			}
			else
				PlankInfo.sWrapEdge.Empty();
			if(pPlate->GetClassTypeId()==CLS_PLATE&&pPlate.PlatePointer()->m_bReplPart)	//代用构件
			{
				QuerySteelMatMark(pPlate->cMaterial,PlankInfo.sReplMaterial);
				QuerySteelMatMark(pPlate.PlatePointer()->org_material,PlankInfo.sDesMaterial);
			}
			else
				QuerySteelMatMark(pPlate->cMaterial,PlankInfo.sDesMaterial);
			PlankInfo.sDesGuiGe.Printf("-%dX%d",thick,wide);	//规格
			PlankInfo.sThick.Printf("%d",thick);
			PlankInfo.sGuiGeIncMat.Printf("-%d %s",thick,(char*)PlankInfo.sDesMaterial);
			PlankInfo.sMapScale.Printf("1:%.2f",sys.general_set.PartMapScale.fRealToDraw);
			if(nTaNum>0)
				PlankInfo.m_nTaNum = nTaNum;//塔的基数
			if(face_N>1)
				PlankInfo.sHuoQuFst.Copy(GetMarkSymbol());
			if(face_N>2)
				PlankInfo.sHuoQuSec.Copy(GetMarkSymbol());
			if(bInProcessCard)
			{
				f2dRect actual_rect = xPlankDraw.GetDrawRegion(1);	//1/zoomCoef 待工艺卡时忽略设置的出图比例 wht 18-09-18
				f2dRect draw_rect = GetProcessCardDrawingRect(2);//int iProcessCardType=0);提前获取钢板工艺卡绘图区域
				bool blSecondZoom=false;
				if(draw_rect.Width()>EPS2&&draw_rect.Height()>EPS2)
				{
					double scale1 = (draw_rect.Width()-20)/actual_rect.Width();
					double scale2 = (draw_rect.Height()-20)/actual_rect.Height();
					double dfScaleOfModel2Drwing=1.0/min(scale1,scale2);
					dfScaleOfModel2Drwing=_LocalRoundingDrawScale(dfScaleOfModel2Drwing);
					zoomCoef=dfScaleOfModel2Drwing/sys.general_set.PartMapScale.fRealToDraw;
					PlankInfo.sMapScale.Printf("1:%g",dfScaleOfModel2Drwing);
					zoomCoef=1.0/dfScaleOfModel2Drwing;
					blSecondZoom=true;
				}
				AcDbObjectId scale_id;
				draw_rect = InsertPlankProcessCardTbl(pBlockTableRecord,origin,&PlankInfo,scale_id);
				if(blSecondZoom)
				{
					origin.x = draw_rect.topLeft.x+10-actual_rect.topLeft.x*zoomCoef;
					origin.y = draw_rect.bottomRight.y+10-actual_rect.bottomRight.y*zoomCoef;
				}
				if(sys.general_set.bHasFrame)
					InsertBlock(pBlockTableRecord,origin,"frame",1,0);
			}
		}
		n = xPlankDraw.GetEdgeCount();
		for(i=0;i<n;i++)
		{
			xPlankDraw.GetEdgeAt(i,arcline);
			arcline.ZoomScale(zoomCoef);
			trans_pos(arcline,origin);
			if(arcline.pen.style==PS_SOLID)
			{
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				if(para[0]&&!bInProcessCard)
					InsertBlock(pBlockTableRecord,arcline.End(),"VERTEX",zoomCoef);
			}
			else if(arcline.pen.style==PS_DASH)
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			else if(arcline.pen.style==PS_DASHDOT)
				GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			else if(arcline.pen.style==PS_DAMAGELINE)
				GetCurDwg()->setClayer(LayerTable::DamagedSymbolLine.layerId);
			else if(arcline.pen.style==PS_TUBEBREAK)	//钢管破断线
			{
				GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
				double t=arcline.feature;
				t=fabs(t)*zoomCoef;
				NewTubeBreakFaceLine(pBlockTableRecord,arcline.Start(),arcline.End(),t);
				continue;
			}
			else if(arcline.pen.style==PS_WELDLINE)
			{
				if(arcline.SectorAngle()==0)
					DrawWeldLineSymbol(pBlockTableRecord,arcline.Start(),arcline.End(),sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				else
					DrawWeldLineSymbol(pBlockTableRecord,arcline,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			}
			else if(arcline.pen.style==PS_OTHERLINE)
				GetCurDwg()->setClayer(LayerTable::CommonLayer.layerId);
			else //if(arcline.feature==5||arcline.pen.style==PS_DASHDOTDOT)
				GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
			if(arcline.feature<0)
				arcline.feature=0;
			if(arcline.SectorAngle()==0)
				CreateAcadLine(pBlockTableRecord,arcline.Start(),arcline.End(),arcline.feature);
			else if(arcline.WorkNorm()==arcline.ColumnNorm())
			{
				if(fabs(arcline.SectorAngle())>=2*Pi-EPS)
					CreateAcadCircle(pBlockTableRecord,arcline.Center(),arcline.Radius(),arcline.feature);
				else
					CreateAcadArcLine(pBlockTableRecord,arcline.Center(),arcline.Start(),arcline.SectorAngle(),arcline.WorkNorm(),arcline.feature);
			}
			else
			{
				CreateAcadEllipseLine(pBlockTableRecord,arcline);
			}
		}
		//绘制火曲标记线（样条曲线）及焊缝标识线
		for(IDbEntity* pEntity=xPlankDraw.EnumFirstDbEntity();pEntity;pEntity=xPlankDraw.EnumNextDbEntity())
		{
			if(pEntity->GetPen().style==PS_INSIDEFRAME)
				GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			else
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			if(pEntity->GetDbEntType()==IDbEntity::DbLine)
			{
				IDbLine* pLine=(IDbLine*)pEntity;
				pLine->ZoomScale(zoomCoef);
				pLine->Offset(origin);
				CreateAcadLine(pBlockTableRecord,pLine->StartPosition(),pLine->EndPosition());
			}
			else if(pEntity->GetDbEntType()==IDbEntity::DbSpline)
			{	//样条曲线
				IDbSpline* pSpline=(IDbSpline*)pEntity;
				pSpline->ZoomScale(zoomCoef);
				pSpline->Offset(origin);
				GEPOINT fitPt;
				AcGePoint3dArray fitPoints;
				int nNum=pSpline->GetNumFitPoints();
				for(int i=0;i<4;i++)
				{
					pSpline->GetFitPointAt(i,fitPt);
					fitPoints.append(AcGePoint3d(fitPt.x,fitPt.y,fitPt.z));
				}
				CreateAcadSpline(pBlockTableRecord,fitPoints);
			}
		}
		//设置为文字标注图层(与火曲字符串同在文字标注图层) wht 11-07-04
		if(sys.part_map.plate.bDrawHuoQuMarkLine)
		{
			GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
			for(i=0;i<2;i++)
			{
				if(xPlankDraw.huoqu_mark[i].origin.IsZero())
					break;
				xPlankDraw.huoqu_mark[i].origin*=zoomCoef;
				xPlankDraw.huoqu_mark[i].origin+=origin;
				f3dPoint offset_vec=xPlankDraw.huoqu_mark[i].offset_dist*xPlankDraw.huoqu_mark[i].prj_vec*zoomCoef;
				for(int j=0;j<5;j++)
				{
					line.startPt=xPlankDraw.huoqu_mark[i].line_arr[j].startPt+xPlankDraw.huoqu_mark[i].origin+offset_vec;
					line.endPt=xPlankDraw.huoqu_mark[i].line_arr[j].endPt+xPlankDraw.huoqu_mark[i].origin+offset_vec;
					//TODO:为什么乘zoomCoef
					//line.startPt=xPlankDraw.huoqu_mark[i].line_arr[j].startPt*zoomCoef+xPlankDraw.huoqu_mark[i].origin+offset_vec*zoomCoef;
					//line.endPt=xPlankDraw.huoqu_mark[i].line_arr[j].endPt*zoomCoef+xPlankDraw.huoqu_mark[i].origin+offset_vec*zoomCoef;		
					CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
				}
			}
		}
		//绘制螺栓
		CXhChar16 sM12("M12"),sM16("M16"),sM18("M18"),sM20("M20"),sM22("M22"),sM24("M24");
		if(sys.part_map.plate.bPlateUseSimpleLsMap)
		{
			sM12.Copy("M12_Jg");
			sM16.Copy("M16_Jg");
			sM18.Copy("M18_Jg");
			sM20.Copy("M20_Jg");
			sM22.Copy("M22_Jg");
			sM24.Copy("M24_Jg");
		}
		//绘制螺栓时使用螺栓图层，某些厂家使用机器自动做板，需要将板轮廓边与螺栓在不同图层显示 wht 11-06-01
		GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
		//江苏华灿要求同一种孔径至少强制直接标注一次螺栓孔径,此属性用于筛选不同孔径　wjh-2016.9.21
		CHashList<UINT> hashHoleDiameters;
		n = xPlankDraw.GetLsCirCount();
		for(i=0;i<n;i++)
		{
			xPlankDraw.GetLsCirAt(i,Cir);
			Cir.centre*=zoomCoef;
			Cir.centre+=origin;
			BOOL bRet=FALSE;
			//得到螺栓孔腰圆长度 画笔默认宽度为1 wht 10-02-25
			int waist_len=Cir.pen.width;
			if(waist_len>1)	
			{
				f3dPoint waist_vec=Cir.norm;	//腰圆方向
				Cir.radius*=zoomCoef;
				if(NewWaistHole(pBlockTableRecord,Cir,waist_vec,waist_len*zoomCoef))
				{	//标注螺栓的腰圆长度  wxc-2017.3.7
					CXhChar16 sText("%d",waist_len);
					if(waist_vec.x<0)	//保证腰圆孔方向与X轴方向相同
						waist_vec*=-1.0;
					double half_len=0.5*waist_len*zoomCoef;
					f3dPoint dimStart=Cir.centre-waist_vec*half_len;
					f3dPoint dimEnd=Cir.centre+waist_vec*half_len;
					f3dPoint dimPos=(dimStart+dimEnd)*0.5;
					double angle=Cal2dLineAng(0,0,waist_vec.x,waist_vec.y);
					DimAngleSize(pBlockTableRecord,dimStart,dimEnd,dimPos,sText,DimStyleTable::dimStyle.dimStyleId,angle,dim_txt_size);//sys.fonts.plate.fDimTextSize);
					continue;
				}
			}
			double hole_d=2*Cir.radius+Cir.feature*0.1;	//实际孔径
			double holeDrawD=hole_d;
			if(sys.general_set.PartMapScale.iBoltSymbolStyle==1)	//图符处理
			{
				if(Cir.pen.style>1)
				{	//特殊功用螺栓孔----根据刘伟要求，凡是特殊功用螺栓孔直接绘制圆，并标注孔径	wxc-2017.8.25
					bRet=FALSE;
				}
				else
				{	//连接螺栓
					if(fabs(2*Cir.radius-12)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM12,1.0,0);
					else if(fabs(2*Cir.radius-16)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM16,1.0,0);
					else if(fabs(2*Cir.radius-18)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM18,1.0,0);
					else if(fabs(2*Cir.radius-20)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM20,1.0,0);
					else if(fabs(2*Cir.radius-22)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM22,1.0,0);
					else if(fabs(2*Cir.radius-24)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM24,1.0,0);
					else
					{
						CXhChar16 ls_str;
						ls_str.Printf("M%d",ftol(2*Cir.radius));
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,1.0,0);
						if(!bRet)
							bRet=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
					}
				}
			}
			else if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)	//比例图符处理
			{
				double fZoomScale=zoomCoef*sys.general_set.PartMapScale.fBoltSymbolFactor;
				if(Cir.pen.style>1)
				{	//特殊功用螺栓孔----根据刘伟要求，凡是特殊功用螺栓孔直接绘制圆，并标注孔径	wxc-2017.8.25
					bRet=FALSE;
				}
				else
				{	//连接螺栓
					if(fabs(2*Cir.radius-12)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM12,fZoomScale,0);
					else if(fabs(2*Cir.radius-16)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM16,fZoomScale,0);
					else if(fabs(2*Cir.radius-18)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM18,fZoomScale,0);
					else if(fabs(2*Cir.radius-20)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM20,fZoomScale,0);
					else if(fabs(2*Cir.radius-22)<EPS)																					   
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM22,fZoomScale,0);
					else if(fabs(2*Cir.radius-24)<EPS)																					   
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM24,fZoomScale,0);
					else
					{
						CXhChar16 ls_str;
						ls_str.Printf("M%d",ftol(2*Cir.radius));
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,fZoomScale,0);
						if(!bRet)
							bRet=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
					}
				}
			}
			else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)	//比例直径图符处理
			{
				double fZoomScale=(Cir.radius*2+Cir.feature/10.0+sys.general_set.fHoleAugmentInNC)*zoomCoef*sys.general_set.PartMapScale.fBoltDiaFactor;
				if(Cir.pen.style>1)
				{	//特殊功用螺栓孔----根据刘伟要求，凡是特殊功用螺栓孔直接绘制圆，并标注孔径	wxc-2017.8.25
					bRet=FALSE;
				}
				else
				{
					if(fabs(2*Cir.radius-12)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM12,fZoomScale,0);
					else if(fabs(2*Cir.radius-16)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM16,fZoomScale,0);
					else if(fabs(2*Cir.radius-18)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM18,fZoomScale,0);
					else if(fabs(2*Cir.radius-20)<EPS)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM20,fZoomScale,0);
					else if(fabs(2*Cir.radius-22)<EPS)																					   
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM22,fZoomScale,0);
					else if(fabs(2*Cir.radius-24)<EPS)																					   
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM24,fZoomScale,0);
					else
					{
						CXhChar16 ls_str;
						ls_str.Printf("M%d",ftol(2*Cir.radius));
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,fZoomScale,0);
						if(!bRet)
							bRet=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
					}	
				}
			}
			BOOL dimHoleDiameterAlways=xPlankDraw.dimStyleFlag.IsHasFlag(CPlankDrawing::DimHoleDiameterAlways);
			if(dimHoleDiameterAlways)
			{	//强制绘制螺栓直径标注
				dimHoleDiameterAlways=(hashHoleDiameters.GetValue(ftoi(hole_d*10))==NULL);
				hashHoleDiameters.SetValue(ftoi(hole_d*10),1);
			}
			if(sys.general_set.PartMapScale.iBoltSymbolStyle==0||!bRet||dimHoleDiameterAlways)	//圆孔处理
			{
				double real_radius=Cir.radius+Cir.feature/20.0+sys.general_set.fHoleAugmentInNC/2.0;
				Cir.radius=real_radius*zoomCoef;
				NewBoltCircle(pBlockTableRecord,Cir);
				if(sys.general_set.PartMapScale.iBoltSymbolStyle!=0)
				{	//标注螺栓图符时，对于特殊孔应标出孔直径，以免错打
					CXhChar16 sText;
					sText.Printf("%%%%c%f",real_radius*2);
					SimplifiedNumString(sText);
					if(Cir.centre.pen.style==CLsRef::DRILL_HOLE)	//钻孔
						sText.Append("钻");
					f3dPoint vec(Cir.radius*0.707,Cir.radius*0.707);
					f3dPoint chordPos(Cir.centre.x-vec.x,Cir.centre.y-vec.y),farChordPos(Cir.centre.x+vec.x,Cir.centre.y+vec.y);
					double leaderLength=2;//*zoomCoef;
					DimDiameter(pBlockTableRecord,chordPos,farChordPos,leaderLength,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
				}
			}
		}
		//设置为尺寸标注图层 wht 11-07-04
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		if(sys.general_set.iDimPrecision==0)		//精确到1mm
			ROTATEDIM_STRU::PRECISION=1.0;
		else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
			ROTATEDIM_STRU::PRECISION=0.5;
		else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
			ROTATEDIM_STRU::PRECISION=0.1;
		n = xPlankDraw.GetRotateDimCount();
		for(i=0;i<n;i++)
		{
			xPlankDraw.GetRotateDimAt(i,rotate_dim);
			rotate_dim.startPos*=zoomCoef;
			rotate_dim.endPos*=zoomCoef;
			rotate_dim.dimPos*=zoomCoef;
			rotate_dim.startPos+=origin;
			rotate_dim.endPos+=origin;
			rotate_dim.dimPos+=origin;
			if(rotate_dim.bDisplayDimLine)
			{
				AcDbObjectId dimStyleId=DimStyleTable::dimStyle.dimStyleId;
				if(!rotate_dim.bCanTextOutter)	//dimStyle3专门用于标注文字可以在外部进行标注的情况 wht 11-07-12
					dimStyleId=DimStyleTable::dimStyle3.dimStyleId;
				DimAngleSize(pBlockTableRecord,rotate_dim.startPos,rotate_dim.endPos,
					rotate_dim.dimPos,rotate_dim.DimText(),dimStyleId,rotate_dim.angle,dim_txt_size);//sys.fonts.plate.fDimTextSize);
			}
			else
			{
				if(rotate_dim.angle>=1.5*Pi||rotate_dim.angle<Pi/2.0)
					rotate_dim.dimPos=GetPtInPolar(rotate_dim.dimPos,rotate_dim.angle-Pi/2,1.5);
				else
					rotate_dim.dimPos=GetPtInPolar(rotate_dim.dimPos,rotate_dim.angle+Pi/2,1.5);
				if(rotate_dim.angle>=1.5*Pi||rotate_dim.angle<Pi/2.0)
					DimText(pBlockTableRecord,rotate_dim.dimPos,rotate_dim.DimText(),TextStyleTable::hzfs.textStyleId,
					dim_txt_size,rotate_dim.angle,AcDb::kTextCenter,AcDb::kTextTop);//sys.dim_map.fDimTextSize*zoomCoef*3
				else
					DimText(pBlockTableRecord,rotate_dim.dimPos,rotate_dim.DimText(),TextStyleTable::hzfs.textStyleId,
					dim_txt_size,rotate_dim.angle+Pi,AcDb::kTextCenter,AcDb::kTextTop);//sys.dim_map.fDimTextSize*zoomCoef*3
			}
		}
		n = xPlankDraw.GetAngularDimCount();
		for(i=0;i<n;i++)
		{
			xPlankDraw.GetAngularDimAt(i,angular_dim);
			angular_dim.vertex*=zoomCoef;
			angular_dim.startPos*=zoomCoef;
			angular_dim.endPos*=zoomCoef;
			angular_dim.dimPos*=zoomCoef;
			angular_dim.vertex+=origin;
			angular_dim.startPos+=origin;
			angular_dim.endPos+=origin;
			angular_dim.dimPos+=origin;
			//角度夹角过小(45度),根据字体高度调整角度标注位置
			f3dPoint rayVecS=angular_dim.startPos-angular_dim.vertex;
			normalize(rayVecS);
			f3dPoint rayVecE=angular_dim.endPos-angular_dim.vertex;
			normalize(rayVecE);
			double half_angle=0.5*ACOS(rayVecS*rayVecE);
			if(half_angle<0.78)
			{
				double fDimR=DISTANCE(angular_dim.vertex,angular_dim.startPos);
				fDimR=max((dim_angle_size*0.7*3)/tan(half_angle),fDimR);
				angular_dim.startPos=angular_dim.vertex+rayVecS*fDimR;
				angular_dim.endPos=angular_dim.vertex+rayVecE*fDimR;
				angular_dim.dimPos=0.5*(angular_dim.startPos+angular_dim.endPos);
			}
			DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,
				angular_dim.vertex,angular_dim.startPos,
				angular_dim.endPos,angular_dim.dimPos,
				angular_dim.sDimText,dim_angle_size);
		}
		n = xPlankDraw.GetDiameterDimCount();
		for(i=0;i<n;i++)
		{
			xPlankDraw.GetDiameterDimAt(i,diam_dim);
			diam_dim.chordPos*=zoomCoef;
			diam_dim.farChordPos*=zoomCoef;
			diam_dim.chordPos+=origin;
			diam_dim.farChordPos+=origin;
			CXhChar50 sText;
			//使用半径标注样式标注半径或直径 wht 11-07-12
			if(diam_dim.bDimRadial)
			{	//
				sText.Printf("R%f",diam_dim.dist);
				SimplifiedNumString(sText);
				DimRadial(pBlockTableRecord,diam_dim.farChordPos,diam_dim.chordPos,
					diam_dim.leaderLength,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
			}
			else
			{	//
				sText.Printf("%%%%c%f",diam_dim.dist);
				SimplifiedNumString(sText);
				DimDiameter(pBlockTableRecord,diam_dim.chordPos,diam_dim.farChordPos,
					diam_dim.leaderLength,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
			}
		}
		//设置为文字标注图层 wht 11-07-04
		GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
		n = xPlankDraw.GetTextDimCount();
		for(i=0;i<n;i++)
		{
			xPlankDraw.GetTextDimAt(i,txt_dim);
			txt_dim.dimPos*=zoomCoef;
			txt_dim.dimPos+=origin;
			double rotAngle=Cal2dLineAng(0,0,txt_dim.dimVec.x,txt_dim.dimVec.y);
			bool reversed=false;
			if(rotAngle>Pi/2&&rotAngle<3*Pi/2)
			{
				rotAngle-=Pi;
				reversed=true;
				if(txt_dim.vertAlignMode==AcDb::kTextTop)
					txt_dim.vertAlignMode=AcDb::kTextBottom;
				else if(txt_dim.vertAlignMode==AcDb::kTextBottom)
					txt_dim.vertAlignMode=AcDb::kTextTop;
				if(txt_dim.horzAlignMode==AcDb::kTextLeft)
					txt_dim.horzAlignMode=AcDb::kTextRight;
				else if(txt_dim.horzAlignMode==AcDb::kTextRight)
					txt_dim.horzAlignMode=AcDb::kTextLeft;
			}
			f3dPoint norm(cos(rotAngle+Pi/2),sin(rotAngle+Pi/2));
			if(txt_dim.vertAlignMode==AcDb::kTextTop)
				txt_dim.dimPos-=norm*1.0;//*zoomCoef;
			else if(txt_dim.vertAlignMode==AcDb::kTextBottom)
				txt_dim.dimPos+=norm*1.0;//*zoomCoef;
			if(txt_dim.txtHeight==0)
				txt_dim.txtHeight=partno_txt_size;//*zoomCoef;
			DimText(pBlockTableRecord,txt_dim.dimPos,txt_dim.dimText,TextStyleTable::hzfs.textStyleId,
				txt_dim.txtHeight,rotAngle,txt_dim.horzAlignMode,txt_dim.vertAlignMode);//AcDb::kTextCenter,AcDb::kTextBottom);//sys.dim_map.fPartNoTextSize
		}
		if(sys.part_map.plate.bDimPlateRayLs)
		{
			n = xPlankDraw.GetLsRayCount();
			for(i=0;i<n;i++)
			{
				DWORD dwDrawFlag=0;
				if(xPlankDraw.GetLsRayLineAt(i,line,&dwDrawFlag))
				{
					//if(!sys.part_map.plate.bDimCoord)
					/* 刘伟说客户普遍反应增加这个延伸量无任何益出，建议删除掉　wjh-2015.3.7
					{	//调整螺栓射线长度
						f3dPoint vec;
						Sub_Pnt(vec,line.endPt,line.startPt);
						normalize(vec);
						if(dwDrawFlag&0x01)
							Sub_Pnt(line.startPt,line.startPt,vec*20);
						if(dwDrawFlag&0x02)
							Add_Pnt(line.endPt,line.endPt,vec*20);
					}*/
					line.startPt*=zoomCoef;
					line.endPt*=zoomCoef;
					trans_pos(line,origin);
					if(line.pen.style==PS_SOLID)
						GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
					else 
						GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
					CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
				}
			}
			n = xPlankDraw.GetAssistLsRayCount();
			for(i=0;i<n;i++)
			{
				if(xPlankDraw.GetAssLsRayLineAt(i,line))
				{
					line.startPt*=zoomCoef;
					line.endPt*=zoomCoef;
					trans_pos(line,origin);
					if(line.pen.style==PS_SOLID)
						GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
					else 
						GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
					CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
				}
			}
		}
		//在文字标注图层中标注构件信息,主要用来区分文字标注与钢板轮廓边的颜色(福建坚石用) wht 11-07-04
		GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
		f3dPoint dim_pos;
		if(!bInProcessCard&&bDimPartInfo)
		{
			//1、初始化文字标注内容
			strcpy(sNotes,"");
			char *token;
			CXhChar200 key_str;
			CStringArray str_arr,sNoteArr;
			key_str.Copy(export_info);
			for(token=strtok(key_str,"\n");token;token=strtok(NULL,"\n"))
				str_arr.Add(token);
			for(i=0;i<str_arr.GetSize();i++)
			{
				sNotes.Empty();
				key_str.Copy(str_arr[i]);
				for(token=strtok(key_str,"&");token;token=strtok(NULL,"&"))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(stricmp(token,"Project Name")==0)
#else
					if(stricmp(token,"工程名称")==0)
#endif
					{
						sTemp.Printf("%s ",Ta.m_sPrjName);
						sNotes.Append(sTemp);
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Tower Type Code")==0)
#else
					else if(stricmp(token,"塔型代号")==0)
#endif
					{
						sTemp.Printf("%s ",Ta.m_sTaAlias);
						sNotes.Append(sTemp); 
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Material")==0)
#else
					else if(stricmp(token,"材质")==0)
#endif
					{
						QuerySteelMatMark(pPlate->cMaterial,sTemp);
						sTemp.Append(" ");
						sNotes.Append(sTemp);
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Part No.")==0)
#else
					else if(stricmp(token,"件号")==0)
#endif
					{
						if(gxDrwLocalizer.ciCustomerSerial==11)
						{	//长春聚德龙要求件号后添加火区信息
							if(xPlankDraw.m_ciHuoQuType==1)			//反曲
								sTemp.Printf("%s#反曲 ",(char*)sPartNo);
							else if(xPlankDraw.m_ciHuoQuType==2)	//正曲
								sTemp.Printf("%s#正曲 ",(char*)sPartNo);
							else
								sTemp.Printf("%s#",(char*)sPartNo);
						}
						else
							sTemp.Printf("%s#",(char*)sPartNo);
						sNotes.Append(sTemp);
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Number Of Parts")==0)
					{
						sTemp.Printf("%d pieces",nHits);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"Thickness")==0)
#else
					else if(stricmp(token,"件数")==0)
					{
						sTemp.Printf("%d件",nHits);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"厚度")==0)
#endif
					{
						sTemp.Printf("-%d ",thick);
						sNotes.Append(sTemp);
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Area")==0)
					{
						sTemp.Printf("area:%.0f(mm2) ",area);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"Piece Weight")==0)
					{
						sTemp.Printf("piece weight:%.2f(kg) ",area*thick*7.85e-6);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"Total Weight")==0)
					{
						sTemp.Printf("total weight:%.2f(kg) ",nHits*area*thick*7.85e-6);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"Tower Type")==0)
#else
					else if(stricmp(token,"面积")==0)
					{
						sTemp.Printf("面积:%.0f(mm2) ",area);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"单重")==0)
					{
						sTemp.Printf("单重:%.2f(kg) ",area*thick*7.85e-6);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"总重")==0)
					{
						sTemp.Printf("总重:%.2f(kg) ",nHits*area*thick*7.85e-6);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"塔型")==0)
#endif
					{
						sTemp.Printf("%s ",(char*)Ta.m_sTaType);
						sNotes.Append(sTemp);
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Spec")==0)
#else
					else if(stricmp(token,"规格")==0)
#endif
					{
						sTemp.Printf("%d*%d(mm) ",wide,length);
						sNotes.Append(sTemp);
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Stamp No.")==0)
#else
					else if(stricmp(token,"钢印号")==0)
#endif
					{
						sTemp.Printf("%s ",Ta.m_sTaStampNo);
						sNotes.Append(sTemp);
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Number")==0)
					{	//cardinal number 
						sTemp.Printf("%d foundation ",nTaNum);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"Number Of Bolts")==0)
#else
					else if(stricmp(token,"基数")==0)
					{	//基数
						sTemp.Printf("%d基 ",nTaNum);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"螺栓数")==0)
#endif
					{
						//孔径X孔数
						sTemp.Empty();
						CHashList<BOLT_ITEM> hashList;
						hashList.Empty();
						CLsRefList* pLsRefList=pPlate->GetLsRefList();
						double L=0;
						if(pPlate&&pPlate->GetClassTypeId()==CLS_PARAMPLATE&&pPlate.ParamPlatePointer()->m_iParamType==TYPE_XEND)
							pPlate.ParamPlatePointer()->GetDesignItemValue('L',&L);
						for(CLsRef* pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
						{
							if(L>0)
							{	//十字插板螺栓
								f3dPoint ls_pos=(*pLsRef)->ucs.origin;
								coord_trans(ls_pos,pPlate->ucs,FALSE);
								if(fabs(pPlate->ucs.axis_z*(*pLsRef)->ucs.axis_z)<EPS_COS2||ls_pos.x>L)
									continue;
							}
							double fDiameter=(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment;
							BOLT_ITEM* pBoltItem=hashList.GetValue((int)fDiameter*10);
							if(pBoltItem==NULL)
							{
								pBoltItem=hashList.Add(ftoi(fDiameter*10));
								pBoltItem->fDiameter=fDiameter;	//实际孔径
							}
							pBoltItem->nNum++;										
						}
						for(BOLT_ITEM* pBoltItem=hashList.GetFirst();pBoltItem;pBoltItem=hashList.GetNext())
						{
#ifdef AFX_TARG_ENU_ENGLISH
							sTemp.Printf("%%%%c%gX%d ",pBoltItem->fDiameter,pBoltItem->nNum);
#else
							sTemp.Printf("%%%%c%gX%d ",pBoltItem->fDiameter,pBoltItem->nNum);
#endif
							sNotes.Append(sTemp);
						}
					}
#ifdef AFX_TARG_ENU_ENGLISH
					else if(stricmp(token,"Number Of Edges")==0)
					{
						sTemp.Printf("number of edges:%d ",nEdgeNum);
						sNotes.Append(sTemp);
					}
					else if (stricmp(token,"Welding Edge Length")==0)
					{
						sTemp.Printf("length of welding edge：%.0f",fWeldLength);
						sNotes.Append(sTemp);
					}
#else
					else if(stricmp(token,"边数")==0)
					{
						sTemp.Printf("边数:%d ",nEdgeNum);
						sNotes.Append(sTemp);
					}
					else if (stricmp(token,"焊缝边长")==0)
					{
						sTemp.Printf("焊缝边长：%.0f",fWeldLength);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"总件数")==0)
					{
						sTemp.Printf("总件数:%d",nHits*nTaNum);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"焊接")==0)
						sNotes.Append("焊接");
					else if(stricmp(token,"制弯")==0)
						sNotes.Append("制弯");
					else if(stricmp(token,"试组装")==0)
						sNotes.Append("试组装");
					else if(stricmp(token,"公差")==0)
					{
						sTemp.Printf("材料公差标准:%s ",g_sTolerance);
						sNotes.Append(sTemp);
					}
					else if(stricmp(token,"任务单号")==0)
					{
						sTemp.Printf("任务单编号:%s ",g_sTaskNo);
						sNotes.Append(sTemp);
					}
#endif
				}
				sNoteArr.Add(sNotes);
			}
			//2、初步确定文字的标注起始位置
			if(sys.part_map.plate.bDimCoord)	//标注相对坐标时，构件明细标注在几何中心
				txt_dim.dimPos=xPlankDraw.Get2dGeomCenter();
			else
				txt_dim.dimPos=xPlankDraw.long_edge_mid;
			txt_dim.dimPos*=zoomCoef;
			txt_dim.dimPos+=origin;
			double angle=0,fTextH=0,fTextW=0;
			angle=xPlankDraw.long_edge_angle;
			if(xPlankDraw.long_edge_angle>Pi/2&&xPlankDraw.long_edge_angle<Pi)
				angle+=Pi;
			else if(angle>=Pi&&angle<1.5*Pi)
				angle-=Pi;
			for(int i=0;i<sNoteArr.GetSize();i++)
			{
				sNotes.Copy(sNoteArr[i]);
				double len=TestDrawTextLength(sNotes,partno_txt_size,TextStyleTable::hzfs.textStyleId);
				fTextW=max(fTextW,len);
				fTextH+=partno_txt_size*1.4;
			}
			f3dPoint long_edge_norm(-sin(xPlankDraw.long_edge_angle),cos(xPlankDraw.long_edge_angle));
			dim_pos=GetPtInPolar(txt_dim.dimPos,angle,-fTextW*0.5);
			int BTM_MARGIN=partno_txt_size>5?(int)partno_txt_size:5;
			dim_pos+=long_edge_norm*BTM_MARGIN;	//文字标注到底边距离最小为5
			//3、初始化标注文字的的矩形区域，并搜索合适的标注位置
			BOOL bSearchPos=FALSE;
			f3dPoint base_pos=dim_pos;
			f3dPoint rgn_vert[4];
			f3dPoint verify_norm(-sin(angle),cos(angle));
			if(verify_norm.x*long_edge_norm.x+verify_norm.y*long_edge_norm.y>0)
			{	//文字走向逆时针转90°方向与long_edge_norm一至
				rgn_vert[0]=base_pos;
				rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,fTextW);
				rgn_vert[2]=rgn_vert[1]+long_edge_norm*fTextH;
				rgn_vert[3]=rgn_vert[0]+long_edge_norm*fTextH;
			}
			else
			{	//文字走向逆时针转90°方向与long_edge_norm相反
				rgn_vert[0]=base_pos-long_edge_norm*partno_txt_size;
				rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,fTextW);
				rgn_vert[2]=rgn_vert[1]+long_edge_norm*fTextH;
				rgn_vert[3]=rgn_vert[0]+long_edge_norm*fTextH;
			}
			int quad_order[4]={1,2,0,0};
			CSearchBlankPosition search;
			search.SetBasePointAddr(base_pos);
			GEPOINT vHoriSearchVec(long_edge_norm.y,-long_edge_norm.x);
			//if(SearchNoneIntersPos(&base_pos,rgn_vert,NULL,quad_order,40,2)==0)
			if( search.SearchNoneIntersPos(rgn_vert,vHoriSearchVec,15,40,0.2,0.8,0x07)||		//0x07=DIRECT_X|DIRECT_X_INVERSE|DIRECT_Y
				search.SearchNoneIntersPos(rgn_vert,vHoriSearchVec,15,BTM_MARGIN-1,0.2,0.8,0x0b))//0x0b=DIRECT_X|DIRECT_X_INVERSE|DIRECT_Y_INVERSE
			{
				f3dPoint restore(base_pos);
				restore-=origin;
				restore/=zoomCoef;
				bSearchPos=xPlankDraw.PtInPlateRgn(restore);
			}
			if(bSearchPos)
				dim_pos=base_pos;
			//4、进行文字标注
			//福建坚石反映，由于板线识别问题需要将钢板轮廓边与标注线分开图层 wjh-2014.9.17
			//DRAGSET.BeginBlockRecord();
			GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
			double fLineHeight=fTextH/sNoteArr.GetSize(),fDist=0;
			int n=sNoteArr.GetSize();
			for(int i=0;i<n;i++)
			{
				AcDb::TextVertMode vertMode=AcDb::kTextBottom;
				if(xPlankDraw.long_edge_angle>Pi/2&&xPlankDraw.long_edge_angle<1.5*Pi)
				{
					fDist=fLineHeight*i;
					vertMode=AcDb::kTextTop;
				}
				else
					fDist=fLineHeight*(n-1-i);
				f3dPoint dimPos=dim_pos+long_edge_norm*fDist;
				sNotes.Copy(sNoteArr[i]);
				DimText(pBlockTableRecord,dimPos,sNotes,TextStyleTable::hzfs.textStyleId,
					partno_txt_size,angle,AcDb::kTextLeft,vertMode);//AcDb::kTextBottom);
				//f3dPoint dimPos=long_edge_norm*fDist;
				//sNotes.Copy(sNoteArr[i]);
				//DimText(DRAGSET.RecordingBlockTableRecord(),dimPos,sNotes,TextStyleTable::hzfs.textStyleId,
				//	partno_txt_size,angle,AcDb::kTextLeft,vertMode);//AcDb::kTextBottom);
			}
			//DRAGSET.EndBlockRecord(pBlockTableRecord,dim_pos);
			if(sys.part_map.plate.bDisplaypPartInfoTbl)			
				DrawPlateInfoTbl(pBlockTableRecord,dim_pos,pTblRowList,&PlankInfo);
		}
		//电焊标记,与标注字体保持一致高度
		if(para[1]&&!bInProcessCard&&IdentifyWeldPlate(pPlate.GeneralPlatePointer()))
		{
			double fScale=partno_txt_size/5;
			double angle=xPlankDraw.long_edge_angle;
			if(xPlankDraw.long_edge_angle>Pi/2&&xPlankDraw.long_edge_angle<Pi)
				angle+=Pi;
			else if(angle>=Pi&&angle<1.5*Pi)
				angle-=Pi;
			f3dPoint long_edge_norm(-sin(xPlankDraw.long_edge_angle),cos(xPlankDraw.long_edge_angle));
			f3dPoint dimPos=xPlankDraw.long_edge_mid*zoomCoef+origin;
			dimPos+=long_edge_norm*10*fScale;
			if(!dim_pos.IsZero())
			{	//
				dimPos=GetPtInPolar(dim_pos,angle,-10*fScale);
				dimPos+=long_edge_norm*partno_txt_size*0.5;
			}
			InsertBlock(pBlockTableRecord,dimPos,"weld",fScale,angle);
		}
		pBlockTableRecord->close();
	}
	catch(char* sError)
	{
		if(pBlockTableRecord)
			pBlockTableRecord->close();
		AfxMessageBox(sError);
	}
}

CStatStdPartGroupDlg statStdPartGroupDlg;
void StdPartGroupProcessCard()
{
	sys.m_iCurMapType = 5;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	//标准组焊件可分三大类：1.两端均为插板 2.两端均为法兰 3.一端法兰一端插板
	if(statStdPartGroupDlg.DoModal()!=IDOK)
		return;
	SPECIAL_PROCESS_STAT_CARD_INFO processInfo;
	CFormatTbl xPartTbl;
	ATOM_LIST<COLUMN_INFO> title_list;
	double tbl_width=0;
#ifdef AFX_TARG_ENU_ENGLISH
	processInfo.sProcessName.Printf("Standard welding parts processing figure");
#else
	processInfo.sProcessName.Printf("标准组焊件加工图");
#endif
	if(statStdPartGroupDlg.m_iStartPartType<=3&&statStdPartGroupDlg.m_iEndPartType<=3)
		processInfo.sSketchMap.Printf("StdPartGroupCC");	//两端均为插板
	else if(statStdPartGroupDlg.m_iStartPartType>3&&statStdPartGroupDlg.m_iEndPartType>3)
		processInfo.sSketchMap.Printf("StdPartGroupFF");	//两端均为法兰
	else 
		processInfo.sSketchMap.Printf("StdPartGroupFC");	//一端为法兰，一端为插板
#ifdef AFX_TARG_ENU_ENGLISH
	title_list.append(COLUMN_INFO("Tube No.",20));
	title_list.append(COLUMN_INFO("Material",13));
	title_list.append(COLUMN_INFO("tube specification",20));
	title_list.append(COLUMN_INFO("number A",20));
	title_list.append(COLUMN_INFO("number B",20));
	title_list.append(COLUMN_INFO("tube length L",20));
	title_list.append(COLUMN_INFO("two bolts distance M",20));
	title_list.append(COLUMN_INFO("Number",12));
	title_list.append(COLUMN_INFO("total number",10));
	title_list.append(COLUMN_INFO("remark",45));
#else
	title_list.append(COLUMN_INFO("钢管编号",20));
	title_list.append(COLUMN_INFO("材质",13));
	title_list.append(COLUMN_INFO("钢管规格",20));
	title_list.append(COLUMN_INFO("A编号",20));
	title_list.append(COLUMN_INFO("B编号",20));
	title_list.append(COLUMN_INFO("钢管长度L",20));
	title_list.append(COLUMN_INFO("两螺栓距离M",20));
	title_list.append(COLUMN_INFO("单基数",12));
	title_list.append(COLUMN_INFO("总数",10));
	title_list.append(COLUMN_INFO("备注",45));
#endif
	tbl_width=200;
	int i,iPart=0,nPartNum=statStdPartGroupDlg.partList.GetNodeNum();
	processInfo.m_nPageIndex=1;
	processInfo.m_nPageNum=nPartNum/15;
	if(nPartNum%15>0)
		processInfo.m_nPageNum+=1;
	
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表

	PARTSET partset;
	f2dPoint insert_pos;
	xPartTbl.SetLocateStyle(TOP_LEFT);
	for(CStatStdPartGroupDlg::STDPART_ITEM *pItem=statStdPartGroupDlg.partList.GetFirst();pItem;
	pItem=statStdPartGroupDlg.partList.GetNext())
	{
		if(pItem->pPart==NULL)
			continue;
		CLDSLineTube *pLineTube=NULL;
		if(pItem->pPart->GetClassTypeId()==CLS_LINETUBE)
			pLineTube=(CLDSLineTube*)pItem->pPart;
		pItem->pPart->feature=pItem->num;
		partset.append(pItem->pPart);
		if(partset.GetNodeNum()==15||iPart==nPartNum-1)
		{
			f2dRect draw_rect=InsertSpecialProcessStatCardTbl(pBlockTableRecord,insert_pos,&processInfo);
			double scale=draw_rect.Width()/tbl_width;
			f2dPoint tbl_pos=draw_rect.topLeft;
			insert_pos.x+=sys.general_set.nColumnWidth;
			processInfo.m_nPageIndex++;
			xPartTbl.Destroy();
			xPartTbl.Create(partset.GetNodeNum()+1,title_list.GetNodeNum());
			i=0;
			for(COLUMN_INFO *pColInfo=title_list.GetFirst();pColInfo;pColInfo=title_list.GetNext())
			{
				xPartTbl.SetGrid(0,i,pColInfo->title);
				xPartTbl.SetColumnWide(i,pColInfo->width*scale);
				i++;
			}
			xPartTbl.SetRowHigh(0,8);
			i=1;
			for(CLDSPart* pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(pPart->GetClassTypeId()!=CLS_LINETUBE)
					continue;
				CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
				CLDSParamPlate *pStartPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				CLDSParamPlate *pEndPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pStartPlate==NULL||pEndPlate==NULL)
					continue;	//钢管非两端连有参数化板
				CXhChar100 spec,tm_str;
				pLineTube->GetSpecification(spec,FALSE);
				f3dPoint startPos,endPos;
				double start_dist,end_dist;
				//钢管一端为法兰一端为插板时要确保始端为法兰
				if(pStartPlate->IsInsertPlate()&&pEndPlate->IsFL())
				{
					CLDSParamPlate *pParamPlate=pStartPlate;
					pStartPlate=pEndPlate;
					pEndPlate=pParamPlate;
				}
				xPartTbl.SetGrid(i,0,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
				QuerySteelMatMark(pLineTube->cMaterial,tm_str);	//材质
				xPartTbl.SetGrid(i,1,tm_str);
				xPartTbl.SetGrid(i,2,spec);
				xPartTbl.SetGrid(i,3,GetPartNoIncMat(pStartPlate->GetPartNo(),pStartPlate->cMaterial));
				xPartTbl.SetGrid(i,4,GetPartNoIncMat(pEndPlate->GetPartNo(),pEndPlate->cMaterial));
				tm_str.Printf("%.1f",pLineTube->GetLength());
				xPartTbl.SetGrid(i,5,tm_str);
				//始端长度
				if(pStartPlate->m_iParamType==TYPE_ROLLEND||pStartPlate->m_iParamType==TYPE_UEND||pStartPlate->m_iParamType==TYPE_XEND)
				{	//查找插板上距离钢管最近的螺栓的位置
					double M=0,min_x=1000;
					pStartPlate->GetDesignItemValue('M',&M);
					for(CLsRef *pLsRef=pStartPlate->GetFirstLsRef();pLsRef;pLsRef=pStartPlate->GetNextLsRef())
					{
						f3dPoint ls_pos=(*pLsRef)->ucs.origin;
						coord_trans(ls_pos,pStartPlate->ucs,FALSE);
						if(ls_pos.x<min_x)
						{
							min_x=ls_pos.x;
							startPos=(*pLsRef)->ucs.origin;
						}
					}
					start_dist=min_x-M;
				}
				else if(pStartPlate->m_iParamType==TYPE_FL||
					pStartPlate->m_iParamType==TYPE_FLD||
					pStartPlate->m_iParamType==TYPE_FLG||
					pStartPlate->m_iParamType==TYPE_FLR)
					pStartPlate->GetDesignItemValue('H',&start_dist);
				else if(pStartPlate->m_iParamType==TYPE_FLP)
					pStartPlate->GetDesignItemValue('O',&start_dist);	//变量'O'中存储H1
				//终端长度
				if(pEndPlate->m_iParamType==TYPE_ROLLEND||pEndPlate->m_iParamType==TYPE_UEND||pEndPlate->m_iParamType==TYPE_XEND)
				{	//查找插板上距离钢管最近的螺栓的位置
					double M=0,min_x=1000;
					pEndPlate->GetDesignItemValue('M',&M);
					for(CLsRef *pLsRef=pEndPlate->GetFirstLsRef();pLsRef;pLsRef=pEndPlate->GetNextLsRef())
					{
						f3dPoint ls_pos=(*pLsRef)->ucs.origin;
						coord_trans(ls_pos,pEndPlate->ucs,FALSE);
						if(ls_pos.x<min_x)
						{
							min_x=ls_pos.x;
							endPos=(*pLsRef)->ucs.origin;
						}
					}
					end_dist=min_x-M;
				}
				else if(pEndPlate->m_iParamType==TYPE_FL||
					pEndPlate->m_iParamType==TYPE_FLG||
					pEndPlate->m_iParamType==TYPE_FLR||
					pEndPlate->m_iParamType==TYPE_FLD)
					pEndPlate->GetDesignItemValue('H',&end_dist);
				else if(pEndPlate->m_iParamType==TYPE_FLP)
					pEndPlate->GetDesignItemValue('O',&end_dist);	//变量'O'中存储H1
				tm_str.Printf("%.1f",pLineTube->GetLength()+start_dist+end_dist);
				xPartTbl.SetGrid(i,6,tm_str);
				tm_str.Printf("%d",pLineTube->feature);			//单基数
				xPartTbl.SetGrid(i,7,tm_str);
				tm_str.Printf("%d",pLineTube->feature*Ta.m_nTaNum);//总数
				xPartTbl.SetGrid(i,8,tm_str);
				if(pStartPlate->m_iParamType==pEndPlate->m_iParamType)
				{
					if(pStartPlate->m_iParamType==TYPE_FL)
#ifdef AFX_TARG_ENU_ENGLISH
						tm_str.Printf("%s","Both AB are ordinary flanges");
					else if(pStartPlate->m_iParamType==TYPE_FLG)
						tm_str.Printf("%s","Both AB are rigid flanges");
					else if(pStartPlate->m_iParamType==TYPE_FLR)
						tm_str.Printf("%s","Both AB are flexible flange");
					else if(pStartPlate->m_iParamType==TYPE_FLP)
						tm_str.Printf("%s","Both AB are flat welding flange");
					else if(pStartPlate->m_iParamType==TYPE_FLD)
						tm_str.Printf("%s","Both AB are butt welding flange");
					else if(pStartPlate->m_iParamType==TYPE_UEND)
						tm_str.Printf("%s","Both AB are U shape flashboard");
					else if(pStartPlate->m_iParamType==TYPE_XEND)
						tm_str.Printf("%s","Both AB are cross shape board");
					else if(pStartPlate->m_iParamType==TYPE_ROLLEND)
					{
						tm_str.Printf("%s","Both AB are U shpae trough board");
						if(pStartPlate->ucs.axis_z*pEndPlate->ucs.axis_z<0)
							tm_str.Printf("%s","Both AB are U shpae trough board and their edge in the opposite direction");
#else
						tm_str.Printf("%s","AB均为普通法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLG)
						tm_str.Printf("%s","AB均为刚性法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLR)
						tm_str.Printf("%s","AB均为柔性法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLP)
						tm_str.Printf("%s","AB均为平焊法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLD)
						tm_str.Printf("%s","AB均为对焊法兰");
					else if(pStartPlate->m_iParamType==TYPE_UEND)
						tm_str.Printf("%s","AB均为U型插板");
					else if(pStartPlate->m_iParamType==TYPE_XEND)
						tm_str.Printf("%s","AB均为十字型插板");
					else if(pStartPlate->m_iParamType==TYPE_ROLLEND)
					{
						tm_str.Printf("%s","AB均为槽型插板");
						if(pStartPlate->ucs.axis_z*pEndPlate->ucs.axis_z<0)
							tm_str.Printf("%s","AB均为槽型插板且卷边方向相反");
#endif
					}
				}
				else
				{
					CXhChar100 start_name,end_name;
					if(pStartPlate->m_iParamType==TYPE_FL)
#ifdef AFX_TARG_ENU_ENGLISH
						start_name.Printf("%s","A is common flange");
					else if(pStartPlate->m_iParamType==TYPE_FLG)
						tm_str.Printf("%s","A is rigid flange");
					else if(pStartPlate->m_iParamType==TYPE_FLR)
						tm_str.Printf("%s","A is flexible flange");
					else if(pStartPlate->m_iParamType==TYPE_FLP)
						start_name.Printf("%s","A is flat welding flange");
					else if(pStartPlate->m_iParamType==TYPE_FLD)
						start_name.Printf("%s","A is butt welding flange");
					else if(pStartPlate->m_iParamType==TYPE_UEND)
						start_name.Printf("%s","A is U shape board");
					else if(pStartPlate->m_iParamType==TYPE_XEND)
						start_name.Printf("%s","A is cross shape board");
					else if(pStartPlate->m_iParamType==TYPE_ROLLEND)
						start_name.Printf("%s","A is U shape trough plate");
					if(pEndPlate->m_iParamType==TYPE_FL)
						end_name.Printf("%s","B is common flange");
					else if(pEndPlate->m_iParamType==TYPE_FLG)
						tm_str.Printf("%s","B is rigid flange");
					else if(pEndPlate->m_iParamType==TYPE_FLR)
						tm_str.Printf("%s","B is flexible flange");
					else if(pEndPlate->m_iParamType==TYPE_FLP)
						end_name.Printf("%s","B is flat welding flange");
					else if(pEndPlate->m_iParamType==TYPE_FLD)
						end_name.Printf("%s","B is butt welding of the flange");
					else if(pEndPlate->m_iParamType==TYPE_UEND)
						end_name.Printf("%s","B is U shape board");
					else if(pEndPlate->m_iParamType==TYPE_XEND)
						end_name.Printf("%s","B is cross shape board");
					else if(pEndPlate->m_iParamType==TYPE_ROLLEND)
						end_name.Printf("%s","B is U shape trough plate");
#else
						start_name.Printf("%s","A为普通法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLG)
						tm_str.Printf("%s","A为刚性法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLR)
						tm_str.Printf("%s","A为柔性法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLP)
						start_name.Printf("%s","A为平焊法兰");
					else if(pStartPlate->m_iParamType==TYPE_FLD)
						start_name.Printf("%s","A为对焊法兰");
					else if(pStartPlate->m_iParamType==TYPE_UEND)
						start_name.Printf("%s","A为U型插板");
					else if(pStartPlate->m_iParamType==TYPE_XEND)
						start_name.Printf("%s","A为十字型插板");
					else if(pStartPlate->m_iParamType==TYPE_ROLLEND)
						start_name.Printf("%s","A为槽型插板");
					if(pEndPlate->m_iParamType==TYPE_FL)
						end_name.Printf("%s","B为普通法兰");
					else if(pEndPlate->m_iParamType==TYPE_FLG)
						tm_str.Printf("%s","B为刚性法兰");
					else if(pEndPlate->m_iParamType==TYPE_FLR)
						tm_str.Printf("%s","B为柔性法兰");
					else if(pEndPlate->m_iParamType==TYPE_FLP)
						end_name.Printf("%s","B为平焊法兰");
					else if(pEndPlate->m_iParamType==TYPE_FLD)
						end_name.Printf("%s","B为对焊法兰");
					else if(pEndPlate->m_iParamType==TYPE_UEND)
						end_name.Printf("%s","B为U型插板");
					else if(pEndPlate->m_iParamType==TYPE_XEND)
						end_name.Printf("%s","B为十字型插板");
					else if(pEndPlate->m_iParamType==TYPE_ROLLEND)
						end_name.Printf("%s","B为槽型插板");
#endif
					tm_str.Printf("%s%s",(char*)start_name,(char*)end_name);
				}
				xPartTbl.SetGrid(i,9,tm_str);
				i++;
			}
			partset.Empty();
			xPartTbl.SetBasePos(tbl_pos);
			DrawTable(pBlockTableRecord,&xPartTbl,FALSE,1);
		}
		iPart++;
	}
	pBlockTableRecord->close();
}

void DrawSetupAidedPlatePartMap(AIDEDPLATE_ITEM *pItem,int iOrder,BOOL bInProcessCard/*=TRUE*/,BOOL bPatternDrawing/*=TRUE*/)
{
	f3dLine line;
	f3dPoint origin;	//绘图原点
	double zoomCoef = 1/sys.general_set.PartMapScale.fRealToDraw;
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord=NULL;//定义块表记录指针
	try{
		if(iOrder>0)
		{
			if(sys.general_set.bCardLayoutBaseRow)
			{	//构件图以行为基准进行排列
				origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
				origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
			}
			else
			{	//构件图以列为基准进行排列
				origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
				origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
			}
		}
		
		//----------------------------------------------------------------------------
		//获得当前图形的块表指针
		AcDbBlockTable *pBlockTable;
		GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
		//以写方式打开模型空间，获得块表记录指针
		pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
		pBlockTable->close();//关闭块表
		//if(!bInProcessCard)
		{
			double plate_width=pItem->radius+pItem->width;//默认刚装辅助钢板的宽度为300
			f3dPoint start_firs,end_firs,start_sec,end_sec;
			//绘制起始钢棒和终止钢板的中心线
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			start_firs.Set(-200,0,0);
			start_firs*=zoomCoef;

			end_firs.Set(plate_width+200,0,0);
			end_firs*=zoomCoef;

			start_sec=start_firs;
			end_sec=end_firs;
			start_firs+=origin;
			end_firs+=origin;
			CreateAcadLine(pBlockTableRecord,start_firs,end_firs);
			rotate_point_around_axis(start_sec,pItem->angle*RADTODEG_COEF,f3dPoint(0,0,0),f3dPoint(0,0,1));
			rotate_point_around_axis(end_sec,pItem->angle*RADTODEG_COEF,f3dPoint(0,0,0),f3dPoint(0,0,1));
			start_sec+=origin;
			end_sec+=origin;
			CreateAcadLine(pBlockTableRecord,start_sec,end_sec);
			//绘制工装辅助钢板
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			double inner_angle,outer_angle;	//内圆弧夹角，外圆弧夹角
			double inner_start_offset_angle,inner_end_offset_angle; //内圆弧始/终端偏移角度
			double outer_start_offset_angle,outer_end_offset_angle; //外圆弧始/终端偏移角度
			inner_start_offset_angle=asin(pItem->start_half_t/pItem->radius);
			inner_end_offset_angle=asin(pItem->end_half_t/pItem->radius);
			outer_start_offset_angle=asin(pItem->start_half_t/plate_width);
			outer_end_offset_angle=asin(pItem->end_half_t/plate_width);
			inner_angle=pItem->angle*RADTODEG_COEF-inner_start_offset_angle-inner_end_offset_angle;
			outer_angle=pItem->angle*RADTODEG_COEF-outer_start_offset_angle-outer_end_offset_angle;
			//计算工装辅助钢板的四个定位点
			start_firs.Set(pItem->radius,0,0);
			rotate_point_around_axis(start_firs,inner_start_offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
			end_firs=start_firs;
			rotate_point_around_axis(end_firs,inner_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
			start_sec.Set(plate_width,0,0);
			rotate_point_around_axis(start_sec,outer_start_offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
			end_sec=start_sec;
			rotate_point_around_axis(end_sec,outer_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
			//坐标转换
			start_firs*=zoomCoef;
			start_firs+=origin;

			end_firs*=zoomCoef;
			end_firs+=origin;

			start_sec*=zoomCoef;
			start_sec+=origin;

			end_sec*=zoomCoef;
			end_sec+=origin;
			//绘制钢板轮廓线
			if(pItem->profile_style==0)
			{
				f3dPoint line_start_firs,line_start_sec,line_start_third;
				f3dPoint line_end_firs,line_end_sec,line_end_third;
				double half_cos_inner=fabs(cos(inner_angle*0.5)),half_cos_outer=fabs(cos(outer_angle*0.5));
				f3dPoint start_to_end = end_sec/half_cos_outer-start_sec/half_cos_outer;
				f3dPoint vector_offset,tmp_line_sec;
				vector_offset.Set(pItem->end_half_t,pItem->start_half_t,0);
				vector_offset*=zoomCoef;

				line_start_firs = (start_firs-origin-vector_offset)/half_cos_inner+origin+vector_offset;
				tmp_line_sec=(start_sec-origin-vector_offset)/half_cos_outer+origin+vector_offset;//钢板上第二个点的位置
				line_start_sec = (tmp_line_sec-line_start_firs)*0.7+line_start_firs;
				line_start_third = tmp_line_sec+start_to_end*0.2;//切角

				line_end_firs = (end_firs-origin-vector_offset)/half_cos_inner+origin+vector_offset;
				tmp_line_sec=(end_sec-origin-vector_offset)/half_cos_outer+origin+vector_offset;//钢板上第二个点的位置
				line_end_sec = (tmp_line_sec-line_end_firs)*0.7+line_end_firs;
				line_end_third = tmp_line_sec-start_to_end*0.2;//切角
				
				CreateAcadLine(pBlockTableRecord,line_start_firs,line_start_sec);
				CreateAcadLine(pBlockTableRecord,line_start_sec,line_start_third);
				CreateAcadLine(pBlockTableRecord,line_start_third,line_end_third);
				CreateAcadLine(pBlockTableRecord,line_end_third,line_end_sec);
				CreateAcadLine(pBlockTableRecord,line_end_sec,line_end_firs);
				CreateAcadLine(pBlockTableRecord,line_end_firs,line_start_firs);
			}
			else
			{
				CreateAcadLine(pBlockTableRecord,start_firs,start_sec);
				CreateAcadLine(pBlockTableRecord,end_firs,end_sec);
				CreateAcadArcLine(pBlockTableRecord,origin,start_firs,inner_angle,f3dPoint(0,0,1));
				CreateAcadArcLine(pBlockTableRecord,origin,start_sec,outer_angle,f3dPoint(0,0,1));
			}
			//标注
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			DimStyleTable::dimAngleStyle.InitDimStyle(sys.fonts.basic.fDimTextSize);//sys.dim_map.fDimAngleTextSize);
			f3dPoint inters_pt,dim_start,dim_end,dim_pos,mid_line_vec;
			//标注夹角
			Int3dll(start_firs,start_sec,end_firs,end_sec,inters_pt);
			mid_line_vec.Set(1,0,0);
			//inters_pt = origin;
			RotateVectorAroundVector(mid_line_vec,pItem->angle*RADTODEG_COEF*0.5,f3dPoint(0,0,1));
			dim_pos=origin+(plate_width+15)*zoomCoef*mid_line_vec;
			DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,inters_pt,start_sec,end_sec,dim_pos);
			//标注半径
			CXhChar50 sText;
			if(pItem->profile_style==1)
			{	//使用半径标注样式标注半径或直径 wht 11-07-12
				DimStyleTable::dimRadiusStyle.InitDimStyle(sys.dim_map.fDimTextSize);
				sText.Printf("R%f",pItem->radius);
				SimplifiedNumString(sText);
				dim_pos.Set(pItem->radius*zoomCoef);
				rotate_point_around_axis(dim_pos,inner_angle*0.25,f3dPoint(0,0,0),f3dPoint(0,0,1));
				dim_pos+=origin;
				DimRadial(pBlockTableRecord,origin,dim_pos,5,sText,DimStyleTable::dimRadiusStyle.dimStyleId,sys.fonts.basic.fDimTextSize);
				sText.Printf("R%f",plate_width);
				SimplifiedNumString(sText);
				dim_pos.Set(plate_width*zoomCoef);
				rotate_point_around_axis(dim_pos,outer_angle*0.75,f3dPoint(0,0,0),f3dPoint(0,0,1));
				dim_pos+=origin;
				DimRadial(pBlockTableRecord,origin,dim_pos,5,sText,DimStyleTable::dimRadiusStyle.dimStyleId,sys.fonts.basic.fDimTextSize);
			}
			//标注半个始/终钢板厚度的一半
			dim_start.Set(plate_width*zoomCoef,0,0);
			dim_pos.Set((plate_width-50)*zoomCoef,0,0);
			dim_start+=origin;
			dim_pos+=origin;
			dim_end=start_sec;
			sText.Printf("R%f",pItem->start_half_t);
			SimplifiedNumString(sText);
			DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sText,DimStyleTable::dimAngleStyle.dimStyleId,1,sys.fonts.basic.fDimTextSize);
			f3dPoint end_line_vec=end_sec-end_firs;
			normalize(end_line_vec);
			dim_start.Set();
			dim_start+=end_line_vec*plate_width*zoomCoef;
			dim_pos=dim_start-(end_line_vec*50)*zoomCoef;
			dim_start+=origin;
			dim_pos+=origin;
			dim_end=end_sec;
			sText.Printf("R%f",pItem->end_half_t);
			SimplifiedNumString(sText);
			DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sText,DimStyleTable::dimAngleStyle.dimStyleId,2,sys.fonts.basic.fDimTextSize);
			//标注构件编号
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			sText.Printf("%s",pItem->sPartNo);
			dim_pos.Set();
			dim_pos+=(mid_line_vec*(pItem->radius+(plate_width-pItem->radius)*0.5))*zoomCoef;
			dim_pos+=origin;
			DimText(pBlockTableRecord,dim_pos,sText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,
				pItem->angle*RADTODEG_COEF*0.5,AcDb::kTextAlign,AcDb::kTextBottom);	
		}
		pBlockTableRecord->close();
	}
	catch(char* sError)
	{
		if(pBlockTableRecord)
			pBlockTableRecord->close();
		AfxMessageBox(sError);
	}
}

void SetupAidedPlate()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CAcModuleResourceOverride resOverride;
	InitDrawingEnv();
	int i=0;
	CSetupAidedPlateDlg aided_plate_dlg;
	if(aided_plate_dlg.DoModal()!=IDOK)
		return;
	if(aided_plate_dlg.m_nPlateNum<=0)
		return;	//列表中无数据
	sprintf(sys.general_set.m_sSetupAidedPlate,"%s",aided_plate_dlg.m_sAidedPlateFilePath);
	//保存工装辅助钢板库文件目录到配置文件中 
	SysSetExportDefault(FALSE);
	BOOL bPatternDrawing=FALSE;
	//if(MessageBox(AfxGetMainWnd()->GetSafeHwnd(),"需要输出的是零件详图(Y)还是加工大样图(N)?","LatticeTower",MB_YESNO)==IDNO)
	//	bPatternDrawing=TRUE;
	CPartDrawingArraySpaceDlg spaceDlg;
	spaceDlg.m_nRowHigh=sys.general_set.nRowHigh;
	spaceDlg.m_nColumnWidth=sys.general_set.nColumnWidth;
	if(spaceDlg.DoModal()!=IDOK)
		return;
	sys.general_set.nRowHigh=spaceDlg.m_nRowHigh;
	sys.general_set.nColumnWidth=spaceDlg.m_nColumnWidth;
	DRAGSET.ClearEntSet();
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(i=0;i<aided_plate_dlg.m_nPlateNum;i++)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(NULL,"Being generated in the first %d artifacts!",i+1);
#else
		PutMessage(NULL,"正在生成第%d个构件!",i+1);
#endif
		DrawSetupAidedPlatePartMap(&aided_plate_dlg.aided_plate[i],i,TRUE,TRUE);	//sys.part_map.bProcessCardTbl,bPatternDrawing);
	}
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
	PutMessage(NULL,NULL);
}

//读取库文件生成标准构件零件图
void StdPartDrawingByReadLibFile()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CAcModuleResourceOverride resOverride;
	InitDrawingEnv();
	static CStdPartSetDlg stdPartDlg;
	if(stdPartDlg.DoModal()!=IDOK)
		return;
	BOOL bPatternDrawing=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	if(MessageBox(AfxGetMainWnd()->GetSafeHwnd(),"the output needed is detail (Y) or processing details of spare parts(N)?","LatticeTower",MB_YESNO)==IDNO)
#else
	if(MessageBox(AfxGetMainWnd()->GetSafeHwnd(),"需要输出的是零件详图(Y)还是加工大样图(N)?","LatticeTower",MB_YESNO)==IDNO)
#endif
		bPatternDrawing=TRUE;
	if(stdPartDlg.partList.GetNodeNum()>1)
	{
		CPartDrawingArraySpaceDlg spaceDlg;
		spaceDlg.m_nRowHigh=sys.general_set.nRowHigh;
		spaceDlg.m_nColumnWidth=sys.general_set.nColumnWidth;
		if(spaceDlg.DoModal()!=IDOK)
			return;
		sys.general_set.nRowHigh=spaceDlg.m_nRowHigh;
		sys.general_set.nColumnWidth=spaceDlg.m_nColumnWidth;
	}
	DRAGSET.ClearEntSet();
	BOOL bDisplayed=TRUE,para[10]={FALSE};
	para[9]=TRUE;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	int i=0,nPageNum=1;
	nPageNum=stdPartDlg.partList.GetNodeNum();
	CXhChar200 export_info;
	ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> *pTblRowList=NULL;
	for(CStdPartSetDlg::STDPART_ITEM *pItem=stdPartDlg.partList.GetFirst();pItem;pItem=stdPartDlg.partList.GetNext())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(NULL,"generating part  %d!",i+1);
#else
		PutMessage(NULL,"正在生成第%d个构件!",i+1);
#endif
		if(pItem->pFlParam==NULL&&pItem->pInsertPlate==NULL&&pItem->pFlParam2==NULL)
			continue;
		CLDSParamPlate paramPlate;
		paramPlate.m_bStdPart=TRUE;
		paramPlate.SetBelongModel(&Ta);
		paramPlate.m_iParamType=pItem->iStdType;
		if(pItem->iStdType==TYPE_ROLLEND)
			paramPlate.m_cRollProcessType=stdPartDlg.m_iRollType;
		int nBoltD=20;
		if((pItem->iStdType==TYPE_ROLLEND||pItem->iStdType==TYPE_UEND||pItem->iStdType==TYPE_XEND)&&pItem->pInsertPlate)
		{
			paramPlate.SetPartNo(pItem->pInsertPlate->codeName,true);
			paramPlate.DesignPlate();
			if(pItem->pInsertPlate->iBoltRect>0)
				nBoltD=pItem->pInsertPlate->ls_param[0].iBoltD;
		}
		else if((pItem->iStdType==TYPE_FLP||pItem->iStdType==TYPE_FLD)&&pItem->pFlParam)
		{
			paramPlate.SetPartNo(pItem->pFlParam->codeName,true);
			paramPlate.DesignPlate();
			nBoltD=pItem->pFlParam->nBoltD;
		}
		else if((pItem->iStdType==TYPE_FLG||pItem->iStdType==TYPE_FLR)&&pItem->pFlParam2)
		{
			paramPlate.SetPartNo(pItem->pFlParam2->codeName,true);
			paramPlate.DesignPlate();
			nBoltD=pItem->pFlParam2->nBoltD;
		}
		else
			continue;
		paramPlate.thick=(long)paramPlate.GetThick();
		CLDSStdPartModel stdPartModel;
		paramPlate.GetStdPartModel(&stdPartModel);
		stdPartModel.LayoutBolt();
		int nBoltNum=0;
		CLDSBolt boltArr[100];
		for(f3dPoint *pPt=stdPartModel.GetFirstBoltPos();pPt;pPt=stdPartModel.GetNextBoltPos())
		{
			boltArr[nBoltNum].SetBelongModel(&Ta);
			boltArr[nBoltNum].set_d(nBoltD);
			if(pPt->feature==31||pPt->feature==41)
				boltArr[nBoltNum].set_norm(f3dPoint(0,1,0));
			else
				boltArr[nBoltNum].set_norm(f3dPoint(0,0,1));
			boltArr[nBoltNum].feature=pPt->feature;
			boltArr[nBoltNum].ucs.origin.Set(pPt->x,pPt->y,pPt->z);
			paramPlate.AppendLsRef(boltArr[nBoltNum].GetLsRef());
			nBoltNum++;
			if(nBoltNum>99)
				break;
		}
		if((!sys.general_set.bProcessCardTbl||partSetDlg.m_bPatternDrawing)&&bDisplayed)
		{
			if(sys.part_map.plate.bUseDimStyleConfigFile==FALSE||sys.part_map.plate.bShowPlateCfgInfoDlg)
			{
				CPlankPartParaExportDlg plank_para_dlg;
				plank_para_dlg.DoModal();
			}
			strcpy(export_info,CPlankPartParaExportDlg::m_sExportPartInfoKeyStr);
			pTblRowList=&CPlankPartParaExportDlg::m_xTblRowList;
			bDisplayed=FALSE;
		}
		DrawPlankTypePartMap(&paramPlate,export_info,para,i,1,sys.general_set.bProcessCardTbl,bPatternDrawing,nPageNum,1,NULL,TRUE,pTblRowList);
		if(paramPlate.m_iParamType==TYPE_XEND&&bPatternDrawing&&pItem->pInsertPlate)
		{	//生成十字插板立板以及包板大样图
			CLDSPlate weldPlate,wrapPlate;
			weldPlate.SetBelongModel(&Ta);
			weldPlate.SetPartNo(pItem->pInsertPlate->weldPlatePartNo);
			weldPlate.ucs.origin.Set(0,0,0.5*pItem->pInsertPlate->Thick);
			weldPlate.ucs.axis_x=paramPlate.ucs.axis_x;
			weldPlate.ucs.axis_y=paramPlate.ucs.axis_z;
			weldPlate.ucs.axis_z=-paramPlate.ucs.axis_y;
			weldPlate.Thick = pItem->pInsertPlate->Thick;

			wrapPlate.SetBelongModel(&Ta);
			wrapPlate.SetPartNo(pItem->pInsertPlate->wrapPlatePartNo);
			wrapPlate.Thick = ftoi(pItem->pInsertPlate->W);
			wrapPlate.ucs.origin.Set(pItem->pInsertPlate->M+pItem->pInsertPlate->A,
				0,pItem->pInsertPlate->R);
			wrapPlate.ucs.axis_x=paramPlate.ucs.axis_x;
			wrapPlate.ucs.axis_y=paramPlate.ucs.axis_z;
			wrapPlate.ucs.axis_z=-paramPlate.ucs.axis_y;
			
			int j;
			for(j=0;j<nBoltNum;j++)
			{
				if(boltArr[j].feature!=31)
					continue;
				if(boltArr[j].ucs.origin.x<pItem->pInsertPlate->L)
					weldPlate.AppendLsRef(boltArr[j].GetLsRef());
				wrapPlate.AppendLsRef(boltArr[j].GetLsRef());
			}
			for(j=0;j<2;j++)
			{
				CLDSPlate *pPlate=&weldPlate;
				double L=pItem->pInsertPlate->L;
				double H=0.5*(pItem->pInsertPlate->H-pItem->pInsertPlate->Thick);
				if(j==1)
				{
					pPlate=&wrapPlate;
					L=pItem->pInsertPlate->S;
					H=pItem->pInsertPlate->N;
				}
				//钢板顶点链表
				pPlate->vertex_list.Empty();
				PROFILE_VERTEX *pNewVertex=pPlate->vertex_list.append();
				pNewVertex->vertex.Set(0,0,0);
				pNewVertex->vertex.feature=1;
				pNewVertex=pPlate->vertex_list.append();
				pNewVertex->vertex.Set(L,0,0);
				pNewVertex->vertex.feature=1;
				pNewVertex=pPlate->vertex_list.append();
				pNewVertex->vertex.Set(L,H,0);
				pNewVertex->vertex.feature=1;
				pNewVertex=pPlate->vertex_list.append();
				pNewVertex->vertex.Set(0,H,0);
				pNewVertex->vertex.feature=1;
				//
				i++;
				DrawPlankTypePartMap(pPlate,export_info,para,i,1,sys.general_set.bProcessCardTbl,bPatternDrawing,nPageNum,1);
			}
		}
		i++;
	}
	if(stdPartDlg.partList.GetNodeNum()==1)
	{
		ads_point base;
		base[X] = 0;
		base[Y] = 0;
		base[Z] = 0;
#ifdef _ARX_2007
		ads_command(RTSTR,L"ZOOM",RTSTR,L"e",RTNONE);
#else
		ads_command(RTSTR,"ZOOM",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
		DragEntSet(base,"Please select the the insertion point of part map scale");
#else
		DragEntSet(base,"请点取构件图的插入点");
#endif
	}
	else
#ifdef _ARX_2007
		ads_command(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
		ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
	PutMessage(NULL,NULL);
}

//
typedef struct tagMID_SLOT_INFO
{
	CLDSPlate *pPlate;
	double dist;	//开口槽原点到钢管起始端的距离
	double min_y;	
	double max_y;
	tagMID_SLOT_INFO() 
	{pPlate=NULL; min_y=1000000; max_y=-1000000; }
}MID_SLOT_INFO;
struct TIP_TRANSCUT{	//定义此类主要是为了将来规范钢管相贯绘制代码，当前暂还未启用 wjh-2019.12.13
	//double short_edge_len;	//平面切割后较短边长度
	double dfTipCutOuterDrwLength;	//绘制钢管简图外壁相对基准端面轴线切割长度偏差值
	double dfTipCutInnerDrwLength;	//绘制钢管简图内壁相对基准端面轴线切割长度偏差值
	double xarrEdgeLength[6];		//依次用来记录 上外,上内,轴线,下内,下外以及轴线位于相贯钢管内部的长度
	TIP_TRANSCUT() {
		dfTipCutOuterDrwLength=dfTipCutInnerDrwLength=0;
		memset(xarrEdgeLength,0,sizeof(double)*6);
	}
};
struct DRAW_TUBE_SIZE {
	double fDrawTubeD;	//钢管绘制直径
	double fDrawTubeT;	//钢管绘制厚度
	double fDrawTubeL;	//钢管绘制长度
	//
	DRAW_TUBE_SIZE(double tube_d = 0, double tube_t = 0, double tube_len = 0) 
	{
		fDrawTubeD = tube_d;
		fDrawTubeT = tube_t;
		fDrawTubeL = tube_len;
	}
};
//绘制钢管两端相贯示意图
void DrawTubeIntersectMap(CLDSLineTube *pLineTube, GEPOINT draw_org, DRAW_TUBE_SIZE& xDrawTubeSize,
				AcDbBlockTableRecord *pTempBlockTableRecord=NULL)
{
	if (pLineTube == NULL)
		return;
	AcDbBlockTableRecord *pBlockTableRecord = pTempBlockTableRecord;
	if(pTempBlockTableRecord==NULL)
		pBlockTableRecord = GetBlockTableRecord();
	if (pBlockTableRecord == NULL)
		return;
	//初始化相贯特征轮廓点及长度
	GECS cs;
	double edge_len_S[6] = { 0 }, edge_len_E[6] = { 0 };
	double fTubeR = pLineTube->GetDiameter()*0.5;
	double fTubeLen = pLineTube->GetLength();
	double ctanaS = 1, ctanaE = 1;
	CLDSLineTube *pTransTubeArr[2] = { NULL };
	for (int i = 0; i < 2; i++)
	{
		TUBEJOINT *pCurJoint = (i == 0) ? &pLineTube->m_tJointStart : &pLineTube->m_tJointEnd;
		CLDSLineTube* pTransTube = (CLDSLineTube*)Ta.FromPartHandle(pCurJoint->hLinkObj, CLS_LINETUBE);
		if (pCurJoint->type != 1 || pTransTube == NULL)
			return;
		pTransTubeArr[i] = pTransTube;
		pTransTube->BuildUCS();
		if (cs.axis_x.IsZero())
		{
			cs.origin = (pLineTube->Start() + pLineTube->End())*0.5;
			cs.axis_x = pLineTube->ucs.axis_z;
			cs.axis_z = pLineTube->ucs.axis_z^pTransTube->ucs.axis_z;
			cs.axis_y = cs.axis_z^cs.axis_x;
			normalize(cs.axis_y);
			cs.axis_z = cs.axis_x^cs.axis_y;
			normalize(cs.axis_z);
		}
		GEPOINT vTransTube = pTransTube->ucs.axis_z;
		if (vTransTube*cs.axis_y < 0)
			vTransTube *= -1;
		vTransTube = cs.TransVToCS(vTransTube);
		double fTransTubeR = pTransTube->GetDiameter()*0.5, fDist = 0;
		double cosa = fabs(pLineTube->ucs.axis_z*pTransTube->ucs.axis_z);
		double sina = sqrt(1 - cosa * cosa);
		double ctana = 1;
		if (fabs(cosa) < EPS_COS)
			ctana = cosa / sina;
		else
			sina = 1;
		//依次用来记录 上外,上内,轴线,下内,下外以及轴线位于相贯钢管内部的长度
		f3dLine datum_line(pTransTube->Start(), pTransTube->End());
		f3dPoint perp_pt;
		SnapPerp(&perp_pt, datum_line, cs.origin, &fDist);
		double fAxisLen = fDist * (1.0 / sina);	//轴线长度
		double fAxisInsertLen = fTransTubeR * (1.0 / sina);//轴线位于相贯钢管内部的长度
		double fAxisValidLen = fAxisLen - fAxisInsertLen;	//轴线有效长度
		double fOuterInsertLen = fTubeR * ctana;
		double fInnerInsertLen = (fTubeR - pLineTube->GetThick())*ctana;
		if (i == 0)
		{	//始端相贯特征长度
			ctanaS = ctana;
			edge_len_S[0] = (vTransTube.x > 0) ? (fAxisValidLen + fOuterInsertLen) : (fAxisValidLen - fOuterInsertLen);
			edge_len_S[1] = (vTransTube.x > 0) ? (fAxisValidLen + fInnerInsertLen) : (fAxisValidLen - fInnerInsertLen);
			edge_len_S[2] = fAxisValidLen;
			edge_len_S[3] = (vTransTube.x > 0) ? (fAxisValidLen - fInnerInsertLen) : (fAxisValidLen + fInnerInsertLen);
			edge_len_S[4] = (vTransTube.x > 0) ? (fAxisValidLen - fOuterInsertLen) : (fAxisValidLen + fOuterInsertLen);
			edge_len_S[5] = fAxisLen;
		}
		else
		{	//终端相贯特征长度
			ctanaE = ctana;
			edge_len_E[0] = (vTransTube.x < 0) ? (fAxisValidLen + fOuterInsertLen) : (fAxisValidLen - fOuterInsertLen);
			edge_len_E[1] = (vTransTube.x < 0) ? (fAxisValidLen + fInnerInsertLen) : (fAxisValidLen - fInnerInsertLen);
			edge_len_E[2] = fAxisValidLen;
			edge_len_E[3] = (vTransTube.x < 0) ? (fAxisValidLen - fInnerInsertLen) : (fAxisValidLen + fInnerInsertLen);
			edge_len_E[4] = (vTransTube.x < 0) ? (fAxisValidLen - fOuterInsertLen) : (fAxisValidLen + fOuterInsertLen);
			edge_len_E[5] = fAxisLen;
		}
	}
	//计算实际相贯钢管加工特征长度
	double fUpOuterLen = edge_len_S[0] + edge_len_E[0];
	double fUpInnerLen = edge_len_S[1] + edge_len_E[1];
	double fDwInnerLen = edge_len_S[3] + edge_len_E[3];
	double fDwOuterLen = edge_len_S[4] + edge_len_E[4];
	double fAxisValidLen = edge_len_S[2] + edge_len_E[2];
	double fAxisTotalLen = edge_len_S[5] + edge_len_E[5];
	//计算相贯钢管绘制长度
	double fDrawTubeR = xDrawTubeSize.fDrawTubeD*0.5;
	double fDrawTubeT = xDrawTubeSize.fDrawTubeT;
	double dfLenDrwScale = 1.0 / sys.general_set.PartMapScale.fRealToDraw;
	if (sys.general_set.bProcessCardTbl)	//使用工艺卡
		dfLenDrwScale = xDrawTubeSize.fDrawTubeL / fTubeLen;
	//始端绘制长度
	double fOuterInsertLen = fDrawTubeR * ctanaS;
	double fInnerInsertLen = (fDrawTubeR - fDrawTubeT)*ctanaS;
	BOOL bAdd = (edge_len_S[0] > edge_len_S[2]) ? TRUE : FALSE;
	edge_len_S[2] *= dfLenDrwScale;
	edge_len_S[0] = (bAdd) ? (edge_len_S[2] + fOuterInsertLen) : (edge_len_S[2] - fOuterInsertLen);
	edge_len_S[1] = (bAdd) ? (edge_len_S[2] + fInnerInsertLen) : (edge_len_S[2] - fInnerInsertLen);
	edge_len_S[3] = (bAdd) ? (edge_len_S[2] - fInnerInsertLen) : (edge_len_S[2] + fInnerInsertLen);
	edge_len_S[4] = (bAdd) ? (edge_len_S[2] - fOuterInsertLen) : (edge_len_S[2] + fOuterInsertLen);
	//终端绘制长度
	fOuterInsertLen = fDrawTubeR * ctanaE;
	fInnerInsertLen = (fDrawTubeR - fDrawTubeT)*ctanaE;
	bAdd = (edge_len_E[0] > edge_len_E[2]) ? TRUE : FALSE;
	edge_len_E[2] *= dfLenDrwScale;
	edge_len_E[0] = (bAdd) ? (edge_len_E[2] + fOuterInsertLen) : (edge_len_E[2] - fOuterInsertLen);
	edge_len_E[1] = (bAdd) ? (edge_len_E[2] + fInnerInsertLen) : (edge_len_E[2] - fInnerInsertLen);
	edge_len_E[3] = (bAdd) ? (edge_len_E[2] - fInnerInsertLen) : (edge_len_E[2] + fInnerInsertLen);
	edge_len_E[4] = (bAdd) ? (edge_len_E[2] - fOuterInsertLen) : (edge_len_E[2] + fOuterInsertLen);
	//绘制轮廓边
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	f3dLine up_edge_line, dw_edge_line;
	up_edge_line.startPt.Set(draw_org.x - edge_len_S[0], draw_org.y + fDrawTubeR);
	up_edge_line.endPt.Set(draw_org.x + edge_len_E[0], draw_org.y + fDrawTubeR);
	dw_edge_line.startPt.Set(draw_org.x - edge_len_S[4], draw_org.y - fDrawTubeR);
	dw_edge_line.endPt.Set(draw_org.x + edge_len_E[4], draw_org.y - fDrawTubeR);
	CreateAcadLine(pBlockTableRecord, up_edge_line.startPt, up_edge_line.endPt);
	CreateAcadLine(pBlockTableRecord, dw_edge_line.startPt, dw_edge_line.endPt);
	CreateAcadLine(pBlockTableRecord, up_edge_line.startPt, dw_edge_line.startPt);
	CreateAcadLine(pBlockTableRecord, up_edge_line.endPt, dw_edge_line.endPt);
	//绘制钢管轴线(与相贯钢管轴线相交)
	GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
	f3dLine axis_line;
	axis_line.startPt.Set(draw_org.x - edge_len_S[2], draw_org.y);
	axis_line.endPt.Set(draw_org.x + edge_len_E[2], draw_org.y);
	//绘制厚度线
	GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
	f3dLine up_thick_line, dw_thick_line;
	up_thick_line.startPt.Set(draw_org.x - edge_len_S[1], draw_org.y + fDrawTubeR - fDrawTubeT);
	up_thick_line.endPt.Set(draw_org.x + edge_len_E[1], draw_org.y + fDrawTubeR - fDrawTubeT);
	dw_thick_line.startPt.Set(draw_org.x - edge_len_S[3], draw_org.y - fDrawTubeR + fDrawTubeT);
	dw_thick_line.endPt.Set(draw_org.x + edge_len_E[3], draw_org.y - fDrawTubeR + fDrawTubeT);
	Int3dll(up_thick_line, f3dLine(up_edge_line.startPt, dw_edge_line.startPt), up_thick_line.startPt);
	Int3dll(dw_thick_line, f3dLine(up_edge_line.startPt, dw_edge_line.startPt), dw_thick_line.startPt);
	Int3dll(up_thick_line, f3dLine(up_edge_line.endPt, dw_edge_line.endPt), up_thick_line.endPt);
	Int3dll(dw_thick_line, f3dLine(up_edge_line.endPt, dw_edge_line.endPt), dw_thick_line.endPt);
	CreateAcadLine(pBlockTableRecord, up_thick_line.startPt, up_thick_line.endPt);
	CreateAcadLine(pBlockTableRecord, dw_thick_line.startPt, dw_thick_line.endPt);
	//标注相贯钢管的直径
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	double fGuigeTextSize = sys.fonts.weld.fPartGuigeTextSize;		//规格标注字体高度
	double fAngleTextSize = sys.fonts.weld.fDimAngleTextSize;		//角度标注字体高度
	double fDimTextSize = sys.fonts.weld.fDimTextSize;				//长度标注字体高度
	CXhChar100 sDimText;
	sDimText.Printf("Φ%.0fX%.0f", pLineTube->GetWidth(), pLineTube->GetThick());
	GEPOINT dimPtS, dimPtE, dimPos, dimVec;
	dimPtS.Set(draw_org.x - fDrawTubeR, draw_org.y + fDrawTubeR);//dimPtS.Set(draw_org.x, draw_org.y + fDrawTubeR);
	dimPtE.Set(draw_org.x - fDrawTubeR, draw_org.y - fDrawTubeR);//dimPtE.Set(draw_org.x, draw_org.y - fDrawTubeR);
	dimPos = (dimPtS + dimPtE)*0.5;
	dimPos.x -= fGuigeTextSize * 2;
	DimAngleSize(pBlockTableRecord, dimPtS, dimPtE, dimPos, sDimText, DimStyleTable::dimStyle.dimStyleId, -0.5*Pi, fGuigeTextSize);
	//上侧外皮长度
#ifdef AFX_TARG_ENU_ENGLISH
	sDimText.Printf("outer plane:%.1f", fUpOuterLen);
#else
	sDimText.Printf("外皮:%.1f", fUpOuterLen);
#endif
	SimplifiedNumString(sDimText);
	dimPtS = up_edge_line.startPt;
	dimPtE = up_edge_line.endPt;
	dimPos = (dimPtS + dimPtE)*0.5;
	dimPos.y += (fDimTextSize *2);
	DimAngleSize(pBlockTableRecord, dimPtS, dimPtE, dimPos, sDimText, DimStyleTable::dimStyle.dimStyleId, 0, fDimTextSize);
	//下侧里皮长度
#ifdef AFX_TARG_ENU_ENGLISH
	sDimText.Printf("inner plane:%.1f", fDwInnerLen);
#else
	sDimText.Printf("里皮:%.1f", fDwInnerLen);
#endif
	SimplifiedNumString(sDimText);
	dimPtS = dw_thick_line.startPt;
	dimPtE = dw_thick_line.endPt;
	dimPos = (dimPtS + dimPtE)*0.5;
	dimPos.y -= (fDimTextSize * 2 + fDrawTubeT);
	DimAngleSize(pBlockTableRecord, dimPtS, dimPtE, dimPos, sDimText, DimStyleTable::dimStyle4.dimStyleId, 0, fDimTextSize);
	//绘制辅助钢管,绘制区域左右侧各有3*fDrawTubeD的空白区
	double offset_len = xDrawTubeSize.fDrawTubeL * 0.2;
	offset_len = min(offset_len, xDrawTubeSize.fDrawTubeD * 1.5);
	for (int i = 0; i < 2; i++)
	{
		GEPOINT up_pt = (i == 0) ? up_edge_line.startPt : up_edge_line.endPt;
		GEPOINT dw_pt = (i == 0) ? dw_edge_line.startPt : dw_edge_line.endPt;
		GEPOINT vTransTube = (i == 0) ? (up_pt - dw_pt) : (dw_pt - up_pt);
		normalize(vTransTube);
		GEPOINT vNormOff(-vTransTube.y, vTransTube.x, 0);
		GEPOINT vertex_arr[10];
		if (i == 0)
		{	//始端相贯钢管
			vertex_arr[0] = up_pt + vTransTube * offset_len + vNormOff * fDrawTubeT;
			vertex_arr[1] = dw_pt - vTransTube * offset_len + vNormOff * fDrawTubeT;
			vertex_arr[2] = up_pt + vTransTube * offset_len + vNormOff * (fDrawTubeR * 2 - fDrawTubeT);
			vertex_arr[3] = dw_pt - vTransTube * offset_len + vNormOff * (fDrawTubeR * 2 - fDrawTubeT);
			vertex_arr[4] = up_pt + vTransTube * offset_len;
			vertex_arr[5] = dw_pt - vTransTube * offset_len;
			vertex_arr[6] = up_pt + vTransTube * offset_len + vNormOff * fDrawTubeR * 2;
			vertex_arr[7] = dw_pt - vTransTube * offset_len + vNormOff * fDrawTubeR * 2;
			vertex_arr[8] = up_pt + vTransTube * (offset_len + 2 * fDrawTubeT) + vNormOff * fDrawTubeR;
			vertex_arr[9] = dw_pt - vTransTube * (offset_len + 2 * fDrawTubeT) + vNormOff * fDrawTubeR;
			//
			Int3dll(axis_line, f3dLine(vertex_arr[8], vertex_arr[9]), axis_line.startPt);
		}
		else
		{	//终端相贯钢管
			vertex_arr[0] = up_pt - vTransTube * offset_len + vNormOff * fDrawTubeT;
			vertex_arr[1] = dw_pt + vTransTube * offset_len + vNormOff * fDrawTubeT;
			vertex_arr[2] = up_pt - vTransTube * offset_len + vNormOff * (fDrawTubeR * 2 - fDrawTubeT);
			vertex_arr[3] = dw_pt + vTransTube * offset_len + vNormOff * (fDrawTubeR * 2 - fDrawTubeT);
			vertex_arr[4] = up_pt - vTransTube * offset_len;
			vertex_arr[5] = dw_pt + vTransTube * offset_len;
			vertex_arr[6] = up_pt - vTransTube * offset_len + vNormOff * fDrawTubeR * 2;
			vertex_arr[7] = dw_pt + vTransTube * offset_len + vNormOff * fDrawTubeR * 2;
			vertex_arr[8] = up_pt - vTransTube * (offset_len + 2 * fDrawTubeT) + vNormOff * fDrawTubeR;
			vertex_arr[9] = dw_pt + vTransTube * (offset_len + 2 * fDrawTubeT) + vNormOff * fDrawTubeR;
			//
			Int3dll(axis_line, f3dLine(vertex_arr[8], vertex_arr[9]), axis_line.endPt);
		}
		//厚度线
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord, vertex_arr[0], vertex_arr[1]);
		CreateAcadLine(pBlockTableRecord, vertex_arr[2], vertex_arr[3]);
		//轮廓线
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord, vertex_arr[4], vertex_arr[5]);
		CreateAcadLine(pBlockTableRecord, vertex_arr[6], vertex_arr[7]);
		//破断线
		NewTubeBreakFaceLine(pBlockTableRecord, vertex_arr[4], vertex_arr[6], fDrawTubeT);
		NewTubeBreakFaceLine(pBlockTableRecord, vertex_arr[5], vertex_arr[7], fDrawTubeT);
		//轴线
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord, vertex_arr[8], vertex_arr[9]);
		//标注相贯钢管父钢管的直径
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		sDimText.Printf("Φ%.0fX%.0f", pTransTubeArr[i]->GetWidth(), pTransTubeArr[i]->GetThick());
		dimPtS = (i == 0) ? vertex_arr[5] : vertex_arr[4];
		dimPtE = (i == 0) ? vertex_arr[7] : vertex_arr[6];
		dimPos = (dimPtS + dimPtE)*0.5;
		dimPos -= vTransTube * fGuigeTextSize * 2;
		double angle = Cal2dLineAng(dimPtS.x, dimPtS.y, dimPtE.x, dimPtE.y);
		AcDbObjectId dimStyleId = (i == 0) ? DimStyleTable::dimStyle4.dimStyleId : DimStyleTable::dimStyle.dimStyleId;
		DimAngleSize(pBlockTableRecord, dimPtS, dimPtE, dimPos, sDimText, dimStyleId, angle, fGuigeTextSize);
		//标注两钢管夹角
		GEPOINT vertex, tube_vec;
		if (i == 0)
		{
			vertex = up_edge_line.startPt;	//axis_line.startPt;
			tube_vec.Set(1, 0, 0);
		}
		else
		{
			vertex = dw_edge_line.endPt;	//axis_line.endPt;
			tube_vec.Set(-1, 0, 0);
		}
		dimPtS = vertex + tube_vec * 1;
		dimPtE = vertex + vTransTube * 1;
		dimVec = (tube_vec + vTransTube)*0.5;
		normalize(dimVec);
		dimPos = vertex + dimVec * offset_len*0.5;
		DimAngle(pBlockTableRecord, DimStyleTable::dimAngleStyle.dimStyleId, vertex, dimPtS, dimPtE, dimPos, NULL, fAngleTextSize);
	}
	//绘制钢管轴线
	GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
	CreateAcadLine(pBlockTableRecord, axis_line.startPt, axis_line.endPt);
	//标注轴线距离
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	sDimText.Printf("%.1f", fAxisTotalLen);
	SimplifiedNumString(sDimText);
	dimPtS = axis_line.startPt;
	dimPtE = axis_line.endPt;
	dimPos = (dimPtS + dimPtE)*0.5;
	dimPos.y += fDimTextSize;
	DimAngleSize(pBlockTableRecord, dimPtS, dimPtE, dimPos, sDimText, DimStyleTable::dimStyle.dimStyleId, 0, fDimTextSize);
	//
	if(pTempBlockTableRecord==NULL)
		pBlockTableRecord->close();
}
void DrawTubeTypeMap(CLDSLineTube *pLineTube,f2dRect *pDrawRect/*=NULL*/,BOOL bIntersected/*=FALSE*/,
					 int iOrder/*=-1*/,int nHits/*=1*/,char *layer/*=NULL*/,int nPageNum/*=1*/,
					 int nTaNum/*=1*/,BOOL bDimPartInfo/*=TRUE*/)
{
	if(pLineTube==NULL)
		return;
	if(pLineTube->GetClassTypeId()!=CLS_LINETUBE)
		return;
	pLineTube->BuildUCS();
	f3dPoint origin;	//原点坐标为(0,0,0)
	if(iOrder>0)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}
	
	//生成此构件的需要注释的内容
	double length,weight;
	CXhChar100 sPartNo,guige,guige2,ls_str,ls_str2;
	guige.Printf("Φ%.0fX%.0f",pLineTube->GetWidth(),pLineTube->GetThick());
	length=pLineTube->GetLength();
	//添加以下代码主要是为了使工艺卡上的角钢长度与中间大样图上的角钢总长标注小数点后一位也一致
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		length = ftoi(length);
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
		length = ftoi(length*2)/2.0;
	else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
		length = ftol(length*10)/10.0;	
	weight = pLineTube->GetWeight();//length*GetJgUnitLenWei(pLineTube->GetWidth(),pLineTube->GetThick())/1000;
	strcpy(sPartNo,GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
	//获得当前图形的块表指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	f3dPoint draw_origin;
	//绘制钢管示意图
	f2dRect draw_rect;
	double draw_slot_len[2]={0,0},draw_slot_width[2]={0,0};	//始端开槽长度、宽度
	double draw_axis_len=0;	//相贯钢管轴线有效绘制长度
	double draw_tube_d=0,draw_tube_t=0,draw_tube_len=0;	//钢管绘图长度
	if(!sys.general_set.bProcessCardTbl)
	{
		draw_tube_d=pLineTube->GetDiameter()*(1.0/sys.general_set.PartMapScale.fRealToDraw);	//钢管绘图直径
		draw_tube_t=pLineTube->GetThick()*(1.0/sys.general_set.PartMapScale.fRealToDraw);		//钢管绘图厚度
		draw_tube_len=pLineTube->GetLength()*(1.0/sys.general_set.PartMapScale.fRealToDraw);	//钢管绘图长度
		draw_origin=origin;
		if(pDrawRect)
		{	//绘制钢管简图列表
			draw_rect=*pDrawRect;
			draw_tube_d=draw_rect.Height()/3;				//钢管绘图直径
			draw_tube_t=draw_tube_d*0.05;					//钢管绘图厚度
			draw_tube_len=draw_rect.Width()-draw_tube_d*6;	//钢管绘图长度
			draw_slot_len[0]=draw_tube_len/6.0;				//始端开槽长度
			draw_slot_len[1]=draw_tube_len/6.0;				//终端开槽长度
			draw_slot_width[0]=draw_tube_t;					//始端开槽宽度 wht 10-10-25
			draw_slot_width[1]=draw_tube_t;					//终端开槽宽度
			draw_origin.Set(draw_rect.topLeft.x+draw_tube_d*1.5,draw_rect.topLeft.y-0.5*draw_rect.Height(),0);
		}
	}
	else
	{	
		if(pDrawRect==NULL)
		{	//填充钢管工艺卡信息
			PART_JG_CARD_INFO TubeInfo;
			memset(&TubeInfo,'\0',sizeof(PART_JG_CARD_INFO));
			TubeInfo.m_nPageNum=nPageNum;		//总页数
			if(iOrder<0)
				TubeInfo.m_nPageIndex=1;		//第几页
			else
				TubeInfo.m_nPageIndex=iOrder+1;//第几页
			if(nTaNum>0)
				TubeInfo.m_nTaNum = nTaNum;
			TubeInfo.sPartNo.Copy(GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial));
			if(length-(int)length>EPS)
				TubeInfo.sLength.Printf("%.1f",length);
			else
				TubeInfo.sLength.Printf("%.0f",length);
			if(pLineTube->m_hPartWeldParent>0x20)
				TubeInfo.sWeld.Copy(GetMarkSymbol());
			else
				TubeInfo.sWeld.Empty();
			TubeInfo.sPartNum.Printf("%d",nHits);
			TubeInfo.sSumPartNum.Printf("%d",nHits*TubeInfo.m_nTaNum);
			TubeInfo.sPartWeight.Printf("%.1f",weight);
			TubeInfo.sSumPartWeight.Printf("%.1f",weight*nHits*TubeInfo.m_nTaNum);
			TubeInfo.sNotes.Printf("%s",pLineTube->sNotes);
			
			CXhChar16 steelmark;
			QuerySteelMatMark(pLineTube->cMaterial,steelmark);
			TubeInfo.sDesMaterial.Copy(steelmark);
			if(gxDrwLocalizer.ciCustomerSerial==3)	//青岛武晓要求工艺卡中的钢管规格为外径X壁厚X长度
				TubeInfo.sDesGuiGe.Printf("Φ%.0fX%.0fX%.0f",pLineTube->GetWidth(),pLineTube->GetThick(),length);
			else
				TubeInfo.sDesGuiGe.Copy(guige);	
			//
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			draw_rect=InsertPartProcessCardTbl(pBlockTableRecord,origin,&TubeInfo,CLS_LINETUBE);
			draw_tube_d=draw_rect.Height()/8;			//钢管绘图直径
		}
		else
		{
			draw_rect=*pDrawRect;
			draw_tube_d=draw_rect.Height()/3;			//钢管绘图直径
		}
		draw_tube_t=draw_tube_d*0.05;					//钢管绘图厚度
		draw_tube_len=draw_rect.Width()-draw_tube_d*6;	//钢管绘图长度
		draw_slot_len[0]=draw_tube_len/6.0;				//始端开槽长度
		draw_slot_len[1]=draw_tube_len/6.0;				//终端开槽长度
		draw_slot_width[0]=draw_tube_t;					//始端开槽宽度 wht 10-10-25
		draw_slot_width[1]=draw_tube_t;					//终端开槽宽度
		draw_origin.Set(draw_rect.topLeft.x+draw_tube_d*1.5,draw_rect.topLeft.y-0.5*draw_rect.Height(),0);
		//钢管直径大于钢板卷管直径阈值时绘制钢管展开图,调整绘图原点 wht 10-10-20
		if(pLineTube->GetDiameter()>sys.part_map.tube.fReelTubeThresholdD&&pDrawRect==NULL)
			draw_origin.y+=(0.2*draw_rect.Height());
	}
	CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
	pStartParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
	pEndParamPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
	//绘制普通钢管轴线
	f3dPoint start_pt(draw_origin.x-draw_tube_d,draw_origin.y);
	f3dPoint end_pt(draw_origin.x+draw_tube_len+4*draw_tube_d,draw_origin.y);
	if(!bIntersected||(bIntersected&&pLineTube->m_tJointStart.hLinkObj<0x20&&pLineTube->m_tJointEnd.hLinkObj<0x20))
	{
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		CreateAcadLine(pBlockTableRecord,start_pt,end_pt);
	}
	f3dPoint vertex_arr[16];
	CXhChar100 sDimText;
	f3dPoint dim_pos,dim_start,dim_end;
	DimStyleTable::dimStyle.InitDimStyle(sys.dim_map.fDimTextSize);
	AcDbObjectId dimStyleId=DimStyleTable::dimStyle.dimStyleId;
	double short_edge_len=length;	//平面切割后较短边长度
	double out_offset_dist[2]={0,0},in_offset_dist[2]={0,0};//始终端由于平面切割造成的偏移量
	double half_d=draw_tube_d*0.5;
	int plate_type[2]={0,0};
	CLDSLineTube *pTransTube=NULL;	//相贯钢管 
	BOOL bIntersectS = FALSE, bIntersectE = FALSE;
	double edge_len[6]={ 0 };	//依次用来记录 上外,上内,轴线,下内,下外以及轴线位于相贯钢管内部的长度
	//绘制钢管端面图，标注钢管直径
	GEPOINT vEndPlaneNorm;
	for(int j=0;j<2;j++)
	{
		f3dPoint draw_center=draw_origin;
		CLDSParamPlate *pCurParamPlate=pStartParamPlate;
		TUBEJOINT *pCurJoint=&pLineTube->m_tJointStart;
		double half_len=1.5*draw_tube_d;
		if(pDrawRect)
			half_len=1.2*draw_tube_d;
		if(j==1)
		{	//终端
			draw_center.Set(draw_origin.x+draw_tube_len+3*draw_tube_d,draw_origin.y);
			pCurParamPlate=pEndParamPlate;
			pCurJoint=&pLineTube->m_tJointEnd;
			if(pStartParamPlate && pEndParamPlate)
			{
				GECS cs;
				cs.axis_z=pLineTube->End()-pLineTube->Start();
				normalize(cs.axis_z);
				cs.axis_x=pStartParamPlate->ucs.axis_z;
				cs.axis_y=cs.axis_x;
				RotateVectorAroundVector(cs.axis_y,1.0,0.0,cs.axis_z);
				vEndPlaneNorm=cs.TransVToCS(pEndParamPlate->ucs.axis_z);
				normalize(vEndPlaneNorm);
			}
		}
		//获取不同插板的开槽数据
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
		{	//根据不同厂家的定制动态库获取不同类型开槽的容差值（富裕量） wjh-2016.8.15
			int iPlateType=pCurParamPlate->m_iParamType;
			GETSLOTLENGTHERROR GetSlotLenError=(GETSLOTLENGTHERROR)GetProcAddress(hInstance,"GetSlotLengthError");
			if(GetSlotLenError!=NULL)
				sys.part_map.tube.fSlotLenProcessError=GetSlotLenError(iPlateType);
			GETSLOTWIDTHERROR GetSlotWidthError=(GETSLOTWIDTHERROR) GetProcAddress(hInstance,"GetSlotWidthError");
			if(GetSlotWidthError!=NULL)
				sys.part_map.tube.fSlotWidthProcessError=GetSlotWidthError(iPlateType);
			FreeLibrary(hInstance);
		}
		if(pCurParamPlate&&(pCurParamPlate->m_iParamType==TYPE_ROLLEND||pCurParamPlate->m_iParamType==TYPE_UEND))
		{	//槽型插板或U型插板
			plate_type[j]=pCurParamPlate->m_iParamType;	//记录插板类型
			double M=0,W=0;
			pCurParamPlate->GetDesignItemValue('M',&M);
			pCurParamPlate->GetDesignItemValue('W',&W);
			if(pCurParamPlate->m_iParamType==TYPE_UEND)
				draw_slot_width[j]=draw_tube_t;	//适当调整U型插板开口槽宽度 wht 10-10-25
			if(!sys.general_set.bProcessCardTbl&&pDrawRect==NULL)	//无工艺卡图框时调整开槽长度按比例显示
			{
				draw_slot_len[j]=M*(1.0/sys.general_set.PartMapScale.fRealToDraw);	//开槽长度
				draw_slot_width[j]=(pCurParamPlate->GetThick()
					+sys.part_map.tube.fSlotWidthProcessError)*(1.0/sys.general_set.PartMapScale.fRealToDraw);	//开槽宽度
				if(pCurParamPlate->m_iParamType==TYPE_UEND)
					draw_slot_width[j]=(W+pCurParamPlate->GetThick()*2
					+sys.part_map.tube.fSlotWidthProcessError)*(1.0/sys.general_set.PartMapScale.fRealToDraw);	//按比例显示U型插板开口槽宽度
			}

			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			start_pt.Set(draw_center.x,draw_center.y+half_len);
			end_pt.Set(draw_center.x,draw_center.y-half_len);
			CreateAcadLine(pBlockTableRecord,start_pt,end_pt);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			//
			//修正half_d因开槽宽度导致的微细小偏移 wjh-2016.8.15
			half_d=SQRT(draw_tube_d*draw_tube_d*0.25-0.25*draw_slot_width[j]*draw_slot_width[j]);
			vertex_arr[0].Set(+half_d,+0.5*draw_slot_width[j]);
			vertex_arr[1].Set(-half_d,+0.5*draw_slot_width[j]);
			vertex_arr[2].Set(-half_d,-0.5*draw_slot_width[j]);
			vertex_arr[3].Set(+half_d,-0.5*draw_slot_width[j]);
			vertex_arr[4].Set(+half_d-draw_tube_t,+0.5*draw_slot_width[j]);
			vertex_arr[5].Set(-half_d+draw_tube_t,+0.5*draw_slot_width[j]);
			vertex_arr[6].Set(-half_d+draw_tube_t,-0.5*draw_slot_width[j]);
			vertex_arr[7].Set(+half_d-draw_tube_t,-0.5*draw_slot_width[j]);
			for(int k=0;k<8;k++)
			{
				if(!vEndPlaneNorm.IsZero())
					RotateVectorAroundVector(vertex_arr[k],vEndPlaneNorm.y,vEndPlaneNorm.x,GEPOINT(0,0,-1));
				vertex_arr[k]+=draw_center;
			}
			for(int i=0;i<4;i++)
			{
				CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+4]);
				//根据圆弧圆心、起始点以及终止点绘制圆弧。
				CreateAcadArcLine(pBlockTableRecord,draw_center,vertex_arr[2*i],vertex_arr[2*i+1]);
			}
			//标注开槽宽度 考虑钢管开槽宽度加工误差值 wht 10-08-18
			double fSoltWidth=pCurParamPlate->thick+sys.part_map.tube.fSlotWidthProcessError;
			if(pCurParamPlate->m_iParamType==TYPE_UEND)
				fSoltWidth=pCurParamPlate->thick*2+W+sys.part_map.tube.fSlotWidthProcessError;	//U型插板开口槽宽度 2*T+W
			sDimText.Printf("2X%.f",fSoltWidth*0.5);
			f3dPoint off_vec,dim_vec;
			if(j==0)
			{
				dim_vec=(vertex_arr[3]-vertex_arr[0]).normalized();
				off_vec.Set(-dim_vec.y,dim_vec.x,0);
				dim_start=vertex_arr[3]+off_vec*sys.dim_map.fDimTextSize;
				dim_end=vertex_arr[0]+off_vec*sys.dim_map.fDimTextSize;
			}
			else
			{
				dim_vec=(vertex_arr[1]-vertex_arr[2]).normalized();
				off_vec.Set(-dim_vec.y,dim_vec.x,0);
				dim_start=vertex_arr[1]+off_vec*sys.dim_map.fDimTextSize;
				dim_end=vertex_arr[2]+off_vec*sys.dim_map.fDimTextSize;
			}
			double angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
			dim_pos=0.5*(dim_start+dim_end);
			DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,angle,sys.fonts.angle.fDimTextSize);
		}
		else if(pCurParamPlate&&pCurParamPlate->m_iParamType==TYPE_XEND)
		{	//十字插板
			plate_type[j]=pCurParamPlate->m_iParamType;	//记录插板类型
			double M=0;
			pCurParamPlate->GetDesignItemValue('M',&M);
			if(!sys.general_set.bProcessCardTbl&&pDrawRect==NULL)	//无工艺卡图框时调整开槽长度按比例显示
			{
				draw_slot_len[j]=M*(1.0/sys.general_set.PartMapScale.fRealToDraw);	//开槽长度
				draw_slot_width[j]=(pCurParamPlate->GetThick()+
					sys.part_map.tube.fSlotWidthProcessError)*(1.0/sys.general_set.PartMapScale.fRealToDraw);	//开槽宽度
			}
			double half_slot_w=draw_slot_width[j]*0.5;
			//修正half_d因开槽宽度导致的微细小偏移 wjh-2016.8.15
			half_d=SQRT(draw_tube_d*draw_tube_d*0.25-half_slot_w*half_slot_w);
			//绘制轴线
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			start_pt.Set(draw_center.x,draw_center.y+half_len);
			end_pt.Set(draw_center.x,draw_center.y-half_len);
			CreateAcadLine(pBlockTableRecord,start_pt,end_pt);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			//应支持立板不垂直于基板的十字插板的开口槽绘制 wht 10-08-17
			CLDSPlate *pUpPlate=NULL,*pDownPlate=NULL;
			pCurParamPlate->DesignPlate();
			pCurParamPlate->GetVertPlateInXEndPlate(&pUpPlate,&pDownPlate);
			double cos_up=1.0,cos_down=1.0;
			f3dPoint up_vec(0,1,0),down_vec(0,-1,0);
			if(pUpPlate&&pDownPlate)
			{
				cos_up=fabs(pUpPlate->ucs.axis_z*pCurParamPlate->ucs.axis_y);
				cos_down=fabs(pDownPlate->ucs.axis_z*pCurParamPlate->ucs.axis_y);
				up_vec=pUpPlate->ucs.axis_y;
				down_vec=pDownPlate->ucs.axis_y;
				vector_trans(up_vec,pCurParamPlate->ucs,FALSE);
				up_vec.Set(up_vec.y,up_vec.z);
				vector_trans(down_vec,pCurParamPlate->ucs,FALSE);
				down_vec.Set(down_vec.y,down_vec.z);
			}
			if(fabs(cos_up)>EPS_COS&&fabs(cos_down)>EPS_COS)
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
				for(int k=0;k<16;k++)
				{
					if(!vEndPlaneNorm.IsZero())
						RotateVectorAroundVector(vertex_arr[k],vEndPlaneNorm.y,vEndPlaneNorm.x,GEPOINT(0,0,-1));
					vertex_arr[k]+=draw_center;
				}
				for(int i=0;i<8;i++)
				{
					CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+8]);
					CreateAcadArcLine(pBlockTableRecord,draw_center,vertex_arr[2*i],vertex_arr[2*i+1]);
				}
			}
			else
			{	//绘制非90°开口槽
				vertex_arr[0].Set(draw_center.x+half_d,draw_center.y+half_slot_w);
				f3dPoint norm_vec(-up_vec.y,up_vec.x),temp_pt=draw_center+up_vec*half_d;
				vertex_arr[1]=temp_pt-norm_vec*half_slot_w;
				vertex_arr[2]=temp_pt+norm_vec*half_slot_w;
				vertex_arr[3].Set(draw_center.x-half_d,draw_center.y+half_slot_w);
				vertex_arr[4].Set(draw_center.x-half_d,draw_center.y-half_slot_w);
				norm_vec.Set(-down_vec.y,down_vec.x);
				temp_pt=draw_center+down_vec*half_d;
				vertex_arr[5]=temp_pt-norm_vec*half_slot_w;
				vertex_arr[6]=temp_pt+norm_vec*half_slot_w;
				vertex_arr[7].Set(draw_center.x+half_d,draw_center.y-half_slot_w);
				vertex_arr[8].Set(draw_center.x+half_d-draw_tube_t,draw_center.y+half_slot_w);
				norm_vec.Set(-up_vec.y,up_vec.x);
				temp_pt=draw_center+up_vec*(half_d-draw_tube_t);
				vertex_arr[9]=temp_pt-norm_vec*half_slot_w;
				vertex_arr[10]=temp_pt+norm_vec*half_slot_w;
				vertex_arr[11].Set(draw_center.x-half_d+draw_tube_t,draw_center.y+half_slot_w);
				vertex_arr[12].Set(draw_center.x-half_d+draw_tube_t,draw_center.y-half_slot_w);
				norm_vec.Set(-down_vec.y,down_vec.x);
				temp_pt=draw_center+down_vec*(half_d-draw_tube_t);
				vertex_arr[13]=temp_pt-norm_vec*half_slot_w;
				vertex_arr[14]=temp_pt+norm_vec*half_slot_w;
				vertex_arr[15].Set(draw_center.x+half_d-draw_tube_t,draw_center.y-half_slot_w);
				for(int i=0;i<8;i++)
				{
					CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[i+8]);
					CreateAcadArcLine(pBlockTableRecord,draw_center,vertex_arr[2*i],vertex_arr[2*i+1]);
					//CreateAcadArcLine(pBlockTableRecord,draw_center,vertex_arr[2*i],vertex_arr[2*i+1]);
				}
				
				//标注角度
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
			}
			//标注开槽宽度 考虑钢管开槽宽度加工误差值 wht 10-08-18
			double fSoltWidth=pCurParamPlate->thick+sys.part_map.tube.fSlotWidthProcessError;	
			sDimText.Printf("2X%.f",fSoltWidth*0.5);
			f3dPoint dim_vec,off_vec;
			//第一个开槽口
			if(j==0)
			{
				dim_vec=(vertex_arr[7]-vertex_arr[0]).normalized();
				off_vec.Set(-dim_vec.y,dim_vec.x,0);
				dim_start=vertex_arr[0]+off_vec*sys.dim_map.fDimTextSize;
				dim_end=vertex_arr[7]+off_vec*sys.dim_map.fDimTextSize;
			}
			else
			{
				dim_vec=(vertex_arr[3]-vertex_arr[4]).normalized();
				off_vec.Set(-dim_vec.y,dim_vec.x,0);
				dim_start=vertex_arr[3]+off_vec*sys.dim_map.fDimTextSize;
				dim_end=vertex_arr[4]+off_vec*sys.dim_map.fDimTextSize;
			}
			dim_pos=0.5*(dim_start+dim_end);
			double angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
			DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,angle,sys.fonts.angle.fDimTextSize);
			//第二个开槽口
			dim_vec=(vertex_arr[5]-vertex_arr[6]).normalized();
			off_vec.Set(-dim_vec.y,dim_vec.x,0);
			dim_start=vertex_arr[5]+off_vec*sys.dim_map.fDimTextSize;
			dim_end=vertex_arr[6]+off_vec*sys.dim_map.fDimTextSize;
			dim_pos=0.5*(dim_start+dim_end);
			angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
			DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,angle,sys.fonts.angle.fDimTextSize);
		}
		else
		{
			if((pCurJoint->type==0&&!pCurJoint->cutPlaneNorm.IsZero())		//平面切割
				||(pCurJoint->type==1&&!pCurJoint->cutPlaneNorm.IsZero()&&pCurJoint->hViceLinkObj<0))	//对接相贯
			{	//平面切割
				double cosa=fabs(pLineTube->ucs.axis_z*pCurJoint->cutPlaneNorm);
				double tana=sqrt(1-cosa*cosa)/cosa;
				out_offset_dist[j]=draw_tube_d*tana;
				in_offset_dist[j]=(draw_tube_d-draw_tube_t)*tana;
				short_edge_len=length-pLineTube->GetDiameter()*tana;
			}
			else if(pCurJoint->type==1&&pCurJoint->hLinkObj>0x20&&bIntersected)
			{	//相贯 一般情况仅有一端相贯，暂时仅处理一端相贯的钢管
				pTransTube=(CLDSLineTube*)Ta.FromPartHandle(pCurJoint->hLinkObj,CLS_LINETUBE);
				if(pTransTube==NULL)
					continue;
				if (j == 0)
					bIntersectS = TRUE;
				else
					bIntersectE = TRUE;
				pTransTube->BuildUCS();
				double cosa=fabs(pLineTube->ucs.axis_z*pTransTube->ucs.axis_z);
				double sina=sqrt(1-cosa*cosa);
				double ctana=1;
				if(fabs(cosa)<EPS_COS)
					ctana=cosa/sina;
				else
					sina=1;
				//依次用来记录 上外,上内,轴线,下内,下外以及轴线位于相贯钢管内部的长度
				f3dLine datum_line(pTransTube->Start(),pTransTube->End());
				f3dPoint perp_pt,pick_pt=pLineTube->End()+pLineTube->ucs.axis_z*pLineTube->endOdd();
				if(j==1)	//终端相贯取始端端点
					pick_pt=pLineTube->Start()-pLineTube->ucs.axis_z*pLineTube->startOdd();
				double dist=0;
				SnapPerp(&perp_pt,datum_line,pick_pt,&dist);
				edge_len[2]=dist*(1.0/sina);	//轴线长度
				edge_len[5]=pTransTube->GetDiameter()*0.5*(1.0/sina);//轴线位于相贯钢管内部的长度
				double outter_len=pLineTube->GetDiameter()*0.5*ctana;
				double inner_len=(pLineTube->GetDiameter()*0.5-pLineTube->GetThick())*ctana;
				edge_len[0]=edge_len[2]-edge_len[5]-outter_len;
				edge_len[1]=edge_len[2]-edge_len[5]-inner_len;
				edge_len[3]=edge_len[2]-edge_len[5]+inner_len;
				edge_len[4]=edge_len[2]-edge_len[5]+outter_len;
				//计算绘制长度
				if(sys.general_set.bProcessCardTbl)
				{	//使用工艺卡
					draw_tube_len=0.5*(draw_rect.Width()-draw_tube_d*6);	//钢管绘图长度
					out_offset_dist[0]=draw_tube_d*ctana;				//上侧外壁线收缩量
					in_offset_dist[0]=(draw_tube_d-draw_tube_t)*ctana;	//上侧内壁线收缩量
					in_offset_dist[1]=draw_tube_t*ctana;				//相贯时用来保存下侧钢管内壁收缩量
					out_offset_dist[1]=draw_tube_len+draw_tube_d*0.5*(1.0/sina)-0.5*draw_tube_d*ctana;//轴线长度
					draw_axis_len=draw_tube_len-0.5*draw_tube_d*ctana;//轴线有效长度
				}
				else
				{
					double draw_scale=1.0/sys.general_set.PartMapScale.fRealToDraw;
					draw_tube_len=edge_len[4]*draw_scale;					//下侧轮廓边长度
					out_offset_dist[0]=(edge_len[4]-edge_len[0])*draw_scale;//上侧外壁线收缩量
					in_offset_dist[0]=(edge_len[4]-edge_len[1])*draw_scale;	//上侧内壁线收缩量
					in_offset_dist[1]=(edge_len[4]-edge_len[3])*draw_scale; //相贯时用来保存下侧钢管内壁收缩量
					//out_offset_dist[1]=edge_len[2]*draw_scale;				//轴线长度
					out_offset_dist[1]=draw_tube_len+draw_tube_d*0.5*(1.0/sina)-0.5*draw_tube_d*ctana;//轴线长度
					draw_axis_len=(draw_tube_len-0.5*draw_tube_d*ctana)*draw_scale;//轴线有效长度
				}
			}
			if(j==0&&(!bIntersected||(bIntersected&&pTransTube==NULL&&pLineTube->m_tJointEnd.hLinkObj<0x20)))
			{	//绘制钢管截面图
				GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
				start_pt.Set(draw_center.x,draw_center.y+half_len);
				end_pt.Set(draw_center.x,draw_center.y-half_len);
				CreateAcadLine(pBlockTableRecord,start_pt,end_pt);
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				//
				CreateAcadCircle(pBlockTableRecord,draw_center,draw_tube_d*0.5);
				CreateAcadCircle(pBlockTableRecord,draw_center,(draw_tube_d-draw_tube_t)*0.5);
			}
		}
	}
	//钢管轮廓边
	// 0					1
	// |￣￣￣￣￣￣￣￣￣￣|　
	//4|--------------------|5
	// |                    |　
	//6|--------------------|7 
	// |____________________| 
	// 3                     2  
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	f3dPoint tube_origin(draw_origin.x+draw_tube_d*1.5,draw_origin.y,0);
	if (bIntersected && bIntersectS && bIntersectE)
	{	//钢管两端相贯
		if (sys.general_set.bProcessCardTbl)
			draw_tube_len = draw_rect.Width() - draw_tube_d * 6;
		else
			draw_tube_len = length/ sys.general_set.PartMapScale.fRealToDraw;
		DRAW_TUBE_SIZE xDrawTubeSize(draw_tube_d, draw_tube_t, draw_tube_len);
		GEPOINT draw_org;
		if (sys.general_set.bProcessCardTbl)
		{
			draw_org.x = draw_rect.topLeft.x + draw_rect.Width()*0.5;
			draw_org.y = draw_rect.topLeft.y - draw_rect.Height()*0.5;
		}
		else
			draw_org = tube_origin;
		DrawTubeIntersectMap(pLineTube, draw_org, xDrawTubeSize, pBlockTableRecord);
	}
	else if(bIntersected&&pTransTube)
	{	//钢管一端相贯
		//钢管轮廓边
		vertex_arr[0].Set(tube_origin.x,tube_origin.y+draw_tube_d*0.5);	
		vertex_arr[1].Set(tube_origin.x+draw_tube_len-out_offset_dist[0],tube_origin.y+draw_tube_d*0.5);
		vertex_arr[2].Set(tube_origin.x+draw_tube_len,tube_origin.y-draw_tube_d*0.5);
		vertex_arr[3].Set(tube_origin.x,tube_origin.y-draw_tube_d*0.5);
		vertex_arr[4].Set(tube_origin.x,tube_origin.y+draw_tube_d*0.5-draw_tube_t);
		vertex_arr[5].Set(tube_origin.x+draw_tube_len-in_offset_dist[0],tube_origin.y+draw_tube_d*0.5-draw_tube_t);
		vertex_arr[6].Set(tube_origin.x,tube_origin.y-draw_tube_d*0.5+draw_tube_t);
		vertex_arr[7].Set(tube_origin.x+draw_tube_len-in_offset_dist[1],tube_origin.y-draw_tube_d*0.5+draw_tube_t);
		//轴线
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		f3dPoint axis_start(tube_origin.x-2*draw_tube_t,tube_origin.y);
		f3dPoint axis_end(tube_origin.x+out_offset_dist[1],tube_origin.y);
		CreateAcadLine(pBlockTableRecord,axis_start,axis_end);	//钢管轴线
		//轮廓线
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		for(int i=0;i<4;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[(i+1)%4]);
		//厚度线
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[5]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[6],vertex_arr[7]);
		//绘制辅助钢管
		f3dPoint trans_tube_vec=vertex_arr[2]-vertex_arr[1];
		normalize(trans_tube_vec);
		f3dPoint norm_vec(-trans_tube_vec.y,trans_tube_vec.x,0);
		double offset_len=draw_tube_len*0.4;
		vertex_arr[8]=vertex_arr[1]-trans_tube_vec*offset_len+norm_vec*draw_tube_t;	//左侧内壁
		vertex_arr[9]=vertex_arr[2]+trans_tube_vec*offset_len+norm_vec*draw_tube_t;
		vertex_arr[10]=vertex_arr[1]-trans_tube_vec*offset_len+norm_vec*(draw_tube_d-draw_tube_t); //右侧内壁
		vertex_arr[11]=vertex_arr[2]+trans_tube_vec*offset_len+norm_vec*(draw_tube_d-draw_tube_t);
		vertex_arr[12]=vertex_arr[1]-trans_tube_vec*offset_len;	//左侧外壁
		vertex_arr[13]=vertex_arr[2]+trans_tube_vec*offset_len;
		vertex_arr[14]=vertex_arr[1]-trans_tube_vec*offset_len+norm_vec*draw_tube_d;	//右侧外壁
		vertex_arr[15]=vertex_arr[2]+trans_tube_vec*offset_len+norm_vec*draw_tube_d;
		//厚度线
		CreateAcadLine(pBlockTableRecord,vertex_arr[8],vertex_arr[9]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[10],vertex_arr[11]);
		//轮廓线
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[12],vertex_arr[13]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[14],vertex_arr[15]);
		//破断线
		NewTubeBreakFaceLine(pBlockTableRecord,vertex_arr[12],vertex_arr[14],draw_tube_t);
		NewTubeBreakFaceLine(pBlockTableRecord,vertex_arr[13],vertex_arr[15],draw_tube_t);
		//轴线
		GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		f3dPoint trans_axis_start(vertex_arr[1]-trans_tube_vec*(offset_len+2*draw_tube_t)+norm_vec*draw_tube_d*0.5);
		f3dPoint trans_axis_end(vertex_arr[2]+trans_tube_vec*(offset_len+2*draw_tube_t)+norm_vec*draw_tube_d*0.5);
		CreateAcadLine(pBlockTableRecord,trans_axis_start,trans_axis_end);
		//标注相贯钢管的直径、里外皮长度以及轴线长度
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		//规格
		sDimText.Printf("%s",&guige);
		dim_pos=0.5*(vertex_arr[0]+vertex_arr[3]);
		dim_pos.x-=sys.dim_map.fDimTextSize*2;
		DimAngleSize(pBlockTableRecord,f3dPoint(vertex_arr[0].x-1,vertex_arr[0].y),
			f3dPoint(vertex_arr[3].x-1,vertex_arr[3].y),dim_pos,sDimText,dimStyleId,-0.5*Pi,sys.fonts.angle.fPartGuigeTextSize);
		//上侧里皮长度
		//sDimText.Printf("里皮:%.1f",edge_len[1]);
		//上侧外皮长度
#ifdef AFX_TARG_ENU_ENGLISH
		sDimText.Printf("outer plane:%.1f",edge_len[0]);
#else
		sDimText.Printf("外皮:%.1f",edge_len[0]);
#endif
		SimplifiedNumString(sDimText);
		f3dPoint dim_start(vertex_arr[0].x,vertex_arr[0].y);
		f3dPoint dim_end(vertex_arr[1].x,vertex_arr[1].y);
		dim_pos=0.5*(dim_start+dim_end);
		dim_pos.y+=(sys.dim_map.fDimTextSize*1.5+draw_tube_t);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		//下侧里皮长度
#ifdef AFX_TARG_ENU_ENGLISH
		sDimText.Printf("inner plane:%.1f",edge_len[3]);
#else
		sDimText.Printf("里皮:%.1f",edge_len[3]);
#endif
		SimplifiedNumString(sDimText);
		dim_start.Set(vertex_arr[6].x,vertex_arr[6].y);
		dim_end.Set(vertex_arr[7].x,vertex_arr[7].y);
		dim_pos=0.5*(dim_start+dim_end);
		dim_pos.y-=(sys.dim_map.fDimTextSize*1.5+draw_tube_t);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		//轴线长度
		sDimText.Printf("%.1f",edge_len[2]);
		SimplifiedNumString(sDimText);
		dim_start.Set(vertex_arr[6].x,tube_origin.y);
		dim_end.Set(axis_end.x,tube_origin.y);
		dim_pos=0.5*(dim_start+dim_end);
		dim_pos.y-=(sys.dim_map.fDimTextSize*2+draw_tube_d);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		//轴线有效长度
		/*sDimText.Printf("%.1f",edge_len[2]-edge_len[5]);
		SimplifiedNumString(sDimText);
		dim_start=tube_origin;
		dim_end.Set(tube_origin.x+draw_axis_len,tube_origin.y);
		dim_pos=0.5*(dim_start+dim_end);
		dim_pos.y+=1;
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);*/
		//标注相贯钢管父钢管的直径以及两钢管夹角
		sDimText.Printf("Φ%.0fX%.0f",pTransTube->GetWidth(),pTransTube->GetThick());
		dim_pos=0.5*(vertex_arr[12]+vertex_arr[14]);
		dim_pos-=trans_tube_vec*sys.dim_map.fDimTextSize*2;
		dim_start.Set(vertex_arr[12].x,vertex_arr[12].y);
		dim_end.Set(vertex_arr[14].x,vertex_arr[14].y);
		double angle = Cal2dLineAng(dim_start.x,dim_start.y,dim_end.x,dim_end.y);
		DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,angle,sys.fonts.angle.fPartGuigeTextSize);
		//
		dim_start=axis_end;
		dim_start.x+=1;
		dim_end=axis_end+trans_tube_vec*1;
		f3dPoint dim_vec=0.5*(f3dPoint(0,0,1)+trans_tube_vec);
		normalize(dim_vec);
		dim_pos=axis_end+dim_vec*0.5*draw_tube_len;
		DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,axis_end,dim_start,dim_end,dim_pos,NULL,sys.fonts.angle.fDimAngleTextSize);
	}
	else
	{	//绘制普通钢管
		vertex_arr[0].Set(draw_origin.x+draw_tube_d*1.5+out_offset_dist[0],draw_origin.y+draw_tube_d*0.5);	
		vertex_arr[1].Set(draw_origin.x+draw_tube_d*1.5+draw_tube_len-out_offset_dist[1],draw_origin.y+draw_tube_d*0.5);
		vertex_arr[2].Set(draw_origin.x+draw_tube_d*1.5+draw_tube_len,draw_origin.y-draw_tube_d*0.5);
		vertex_arr[3].Set(draw_origin.x+draw_tube_d*1.5,draw_origin.y-draw_tube_d*0.5);
		vertex_arr[4].Set(draw_origin.x+draw_tube_d*1.5+in_offset_dist[0],draw_origin.y+draw_tube_d*0.5-draw_tube_t);
		vertex_arr[5].Set(draw_origin.x+draw_tube_d*1.5+draw_tube_len-in_offset_dist[1],draw_origin.y+draw_tube_d*0.5-draw_tube_t);
		vertex_arr[6].Set(draw_origin.x+draw_tube_d*1.5,draw_origin.y-draw_tube_d*0.5+draw_tube_t);
		vertex_arr[7].Set(draw_origin.x+draw_tube_d*1.5+draw_tube_len,draw_origin.y-draw_tube_d*0.5+draw_tube_t);
		//绘制开口槽
		if(plate_type[0]==TYPE_XEND||plate_type[1]==TYPE_XEND)
		{	//存在十字插板时绘制上侧凹槽
			for(int i=0;i<2;i++)
			{
				int nFlag=1;
				if(i==1)
					nFlag=-1;
				//始端
				if(plate_type[0]==TYPE_XEND)
				{
					if(i==0)
						vertex_arr[8]=vertex_arr[4];
					else
						vertex_arr[8]=vertex_arr[6];
					vertex_arr[9].Set(vertex_arr[8].x+draw_slot_len[0],vertex_arr[8].y);
					vertex_arr[10].Set(vertex_arr[9].x,vertex_arr[9].y+draw_tube_t*nFlag);
					if(i==1&&pStartParamPlate)	//在下侧标注
					{	//标注开槽长度
						double M=0;
						pStartParamPlate->GetDesignItemValue('M',&M);
						//标注开槽宽度 考虑钢管开槽长度加工误差值 wht 10-08-18
						double fSoltLen=M+sys.part_map.tube.fSlotLenProcessError;	
						sDimText.Printf("%.f",fSoltLen);
						SimplifiedNumString(sDimText);
						f3dPoint dim_start(vertex_arr[8].x,vertex_arr[8].y-draw_tube_t-1);
						f3dPoint dim_end(vertex_arr[9].x,vertex_arr[9].y-draw_tube_t-1);
						dim_pos=0.5*(dim_start+dim_end);
						if(pDrawRect==NULL)
							dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
						else 
							dim_pos.y-=sys.dim_map.fDimTextSize;
						DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
				}
				else if(i==0)
					vertex_arr[8]=vertex_arr[9]=vertex_arr[10]=vertex_arr[0];
				else
					vertex_arr[8]=vertex_arr[9]=vertex_arr[10]=vertex_arr[3];
				//终端
				if(plate_type[1]==TYPE_XEND)
				{
					if(i==0)
						vertex_arr[13]=vertex_arr[5];
					else
						vertex_arr[13]=vertex_arr[7];
					vertex_arr[12].Set(vertex_arr[13].x-draw_slot_len[1],vertex_arr[13].y);
					vertex_arr[11].Set(vertex_arr[12].x,vertex_arr[12].y+draw_tube_t*nFlag);
					if(i==1&&pEndParamPlate)	//在下侧标注
					{	//标注开槽长度
						double M=0;
						pEndParamPlate->GetDesignItemValue('M',&M);
						//标注开槽宽度 考虑钢管开槽长度加工误差值 wht 10-08-18
						double fSoltLen=M+sys.part_map.tube.fSlotLenProcessError;	
						sDimText.Printf("%.f",fSoltLen);
						SimplifiedNumString(sDimText);
						f3dPoint dim_start(vertex_arr[12].x,vertex_arr[12].y-draw_tube_t-1);
						f3dPoint dim_end(vertex_arr[13].x,vertex_arr[13].y-draw_tube_t-1);
						dim_pos=0.5*(dim_start+dim_end);
						if(pDrawRect==NULL)
							dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
						else 
							dim_pos.y-=sys.dim_map.fDimTextSize;
						DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
					}
				}
				else if(i==0)
					vertex_arr[11]=vertex_arr[12]=vertex_arr[13]=vertex_arr[1];
				else 
					vertex_arr[11]=vertex_arr[12]=vertex_arr[13]=vertex_arr[2];
				for(int j=8;j<13;j++)
					CreateAcadLine(pBlockTableRecord,vertex_arr[j],vertex_arr[j+1]);
			}
		}
		else
		{
			if(fabs(vEndPlaneNorm.y)>0.707)
			{	//开口槽终端绘制示意思调转90°
				vertex_arr[1].x-=draw_slot_len[1];
				vertex_arr[2].x-=draw_slot_len[1];
			}
			CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1]);
			CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[3]);
			if(fabs(vEndPlaneNorm.y)>0.707)
			{	//开口槽终端绘制示意思调转90°
				vertex_arr[1].x+=draw_slot_len[1];
				vertex_arr[2].x+=draw_slot_len[1];
			}
		}
		//在钢管轮廓边上绘制开口槽
		if(plate_type[0]!=0)
		{
			if(plate_type[0]==TYPE_XEND)
			{
				vertex_arr[8]=vertex_arr[4];
				vertex_arr[13]=vertex_arr[6];
			}
			else
			{
				vertex_arr[8]=vertex_arr[0];
				vertex_arr[13]=vertex_arr[3];
			}
			double fUSlotArcR=0;	//U型开口槽圆弧半径
			if(plate_type[0]==TYPE_UEND)
				fUSlotArcR=0.5*draw_slot_width[0];
			vertex_arr[9].Set(vertex_arr[8].x,draw_origin.y+0.5*draw_slot_width[0]);
			vertex_arr[10].Set(vertex_arr[9].x+draw_slot_len[0]-fUSlotArcR,vertex_arr[9].y); //考虑U型开口槽半径fUSlotArcR
			vertex_arr[11].Set(vertex_arr[10].x,vertex_arr[9].y-draw_slot_width[0]);
			vertex_arr[12].Set(vertex_arr[9].x,vertex_arr[11].y);
			for(int j=8;j<13;j++)
			{
				if(j==10&&plate_type[0]==TYPE_UEND)	//绘制U型开口槽圆弧 wht 10-11-19
					CreateAcadArcLine(pBlockTableRecord,f3dPoint(vertex_arr[10].x,draw_origin.y),vertex_arr[11],vertex_arr[10]);
				else
					CreateAcadLine(pBlockTableRecord,vertex_arr[j],vertex_arr[j+1]);
			}
			if(pStartParamPlate)	//在下侧标注
			{	//标注开槽长度
				double M=0;
				pStartParamPlate->GetDesignItemValue('M',&M);
				//标注开槽宽度 考虑钢管开槽长度加工误差值 wht 10-08-18
				double fSoltLen=M+sys.part_map.tube.fSlotLenProcessError;	
				sDimText.Printf("%.f",fSoltLen);
				SimplifiedNumString(sDimText);
				//考虑U型开口槽半径fUSlotArcR
				f3dPoint dim_start(vertex_arr[11].x+fUSlotArcR,vertex_arr[3].y-1);
				f3dPoint dim_end(vertex_arr[3].x,vertex_arr[3].y-1);
				dim_pos=0.5*(dim_start+dim_end);
				if(pDrawRect==NULL)
					dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
				else 
					dim_pos.y-=sys.dim_map.fDimTextSize;
				DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
			}
		}
		else
			CreateAcadLine(pBlockTableRecord,vertex_arr[3],vertex_arr[0]);
		if(plate_type[1]!=0)
		{
			if(plate_type[1]==TYPE_XEND)
			{
				vertex_arr[8]=vertex_arr[5];
				vertex_arr[13]=vertex_arr[7];
			}
			else
			{
				vertex_arr[8]=vertex_arr[1];
				vertex_arr[13]=vertex_arr[2];
			}
			double fUSlotArcR=0;	//U型开口槽圆弧半径
			if(plate_type[1]==TYPE_UEND)
				fUSlotArcR=0.5*draw_slot_width[1];
			if(plate_type[1]==TYPE_XEND||fabs(vEndPlaneNorm.y)<=0.707)
			{
				vertex_arr[9].Set(vertex_arr[8].x,draw_origin.y+0.5*draw_slot_width[0]);
				vertex_arr[10].Set(vertex_arr[9].x-draw_slot_len[1]+fUSlotArcR,vertex_arr[9].y);//考虑U型开口槽半径fUSlotArcR
				vertex_arr[11].Set(vertex_arr[10].x,vertex_arr[9].y-draw_slot_width[0]);
				vertex_arr[12].Set(vertex_arr[9].x,vertex_arr[11].y);
				for(int j=8;j<13;j++)
				{
					if(j==10&&plate_type[1]==TYPE_UEND) //绘制U型开口槽圆弧 wht 10-11-19
						CreateAcadArcLine(pBlockTableRecord,f3dPoint(vertex_arr[10].x,draw_origin.y),vertex_arr[10],vertex_arr[11]);
					else
						CreateAcadLine(pBlockTableRecord,vertex_arr[j],vertex_arr[j+1]);
				}
			}
			if(pEndParamPlate)	//在下侧标注
			{	//标注开槽长度
				double M=0;
				pEndParamPlate->GetDesignItemValue('M',&M);
				//标注开槽宽度 考虑钢管开槽长度加工误差值 wht 10-08-18
				double fSoltLen=M+sys.part_map.tube.fSlotLenProcessError;	
				sDimText.Printf("%.f",fSoltLen);
				SimplifiedNumString(sDimText);
				//考虑U型开口槽半径fUSlotArcR
				f3dPoint dim_start(vertex_arr[11].x-fUSlotArcR,vertex_arr[2].y-1);
				f3dPoint dim_end(vertex_arr[2].x,vertex_arr[2].y-1);
				dim_pos=0.5*(dim_start+dim_end);
				if(pDrawRect==NULL)
					dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
				else 
					dim_pos.y-=sys.dim_map.fDimTextSize;
				DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
			}
		}
		else
			CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[2]);
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		if(plate_type[1]!=TYPE_XEND&&fabs(vEndPlaneNorm.y)>0.707)
		{	//开口槽终端绘制示意思调转90°
			vertex_arr[5].x-=draw_slot_len[1];
			vertex_arr[7].x-=draw_slot_len[1];
		}
		CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[5]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[6],vertex_arr[7]);
		if(plate_type[1]!=TYPE_XEND&&fabs(vEndPlaneNorm.y)>0.707)
		{	//开口槽终端绘制示意思调转90°
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,vertex_arr[5]+f3dPoint(0,-0.5),vertex_arr[5]+f3dPoint(draw_slot_len[1],-0.5));
			CreateAcadLine(pBlockTableRecord,vertex_arr[7]+f3dPoint(0, 0.5),vertex_arr[7]+f3dPoint(draw_slot_len[1], 0.5));
			CreateAcadLine(pBlockTableRecord,vertex_arr[5]+f3dPoint(0,-0.5),f3dPoint(vertex_arr[5].x,vertex_arr[1].y));
			CreateAcadLine(pBlockTableRecord,vertex_arr[7]+f3dPoint(0,+0.5),f3dPoint(vertex_arr[7].x,vertex_arr[2].y));
			CreateAcadLine(pBlockTableRecord,f3dPoint(vertex_arr[1].x,vertex_arr[5].y-0.5),f3dPoint(vertex_arr[1].x,vertex_arr[7].y+0.5));
			vertex_arr[5].x-=draw_slot_len[1];
			vertex_arr[7].x-=draw_slot_len[1];
		}
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		//标注平面切割角度
		DimStyleTable::dimAngleStyle.InitDimStyle(sys.dim_map.fDimAngleTextSize);
		if(out_offset_dist[0]>EPS2)
		{	//始端平面切割
			double dim_r=DISTANCE(vertex_arr[0],vertex_arr[3]);
			f3dPoint dim_start=vertex_arr[0];
			f3dPoint dim_end(vertex_arr[3].x+dim_r,vertex_arr[3].y);
			f3dPoint dim_pos=0.5*(dim_start+dim_end);
			if(pDrawRect==NULL)
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,vertex_arr[3],dim_start,dim_end,dim_pos,NULL);
		}
		if(out_offset_dist[1]>EPS)
		{	//终端平面切割
			double dim_r=DISTANCE(vertex_arr[2],vertex_arr[1]);
			f3dPoint dim_start=vertex_arr[1];
			f3dPoint dim_end(vertex_arr[2].x-dim_r,vertex_arr[2].y);
			f3dPoint dim_pos=0.5*(dim_start+dim_end);
			if(pDrawRect==NULL)
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,vertex_arr[2],dim_start,dim_end,dim_pos,NULL);
		}
		//标注长度
		if(out_offset_dist[0]>EPS2||out_offset_dist[1]>EPS2)
		{
			sDimText.Printf("%.1f",short_edge_len);
			SimplifiedNumString(sDimText);
			dim_pos=0.5*(vertex_arr[0]+vertex_arr[1]);
			if(pDrawRect==NULL)
				dim_pos.y+=sys.dim_map.fDimTextSize*1.5;
			DimAngleSize(pBlockTableRecord,f3dPoint(vertex_arr[0].x,vertex_arr[0].y+1),
				f3dPoint(vertex_arr[1].x,vertex_arr[1].y+1),dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
			sDimText.Printf("%.1f",length);
			SimplifiedNumString(sDimText);
			dim_pos=0.5*(vertex_arr[3]+vertex_arr[2]);
			if(pDrawRect==NULL)
				dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
			else	
				dim_pos.y-=sys.dim_map.fDimTextSize;
			DimAngleSize(pBlockTableRecord,f3dPoint(vertex_arr[3].x,vertex_arr[3].y-1),
				f3dPoint(vertex_arr[2].x,vertex_arr[2].y-1),dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		}
		else
		{
			sDimText.Printf("%.1f",length);
			SimplifiedNumString(sDimText);
			dim_pos=0.5*(vertex_arr[0]+vertex_arr[1]);
			if(pDrawRect==NULL)
				dim_pos.y+=sys.dim_map.fDimTextSize*1.5;
			else 
				dim_pos.y+=1;
			DimAngleSize(pBlockTableRecord,f3dPoint(vertex_arr[0].x,vertex_arr[0].y+1),
				f3dPoint(vertex_arr[1].x,vertex_arr[1].y+1),dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		}
		//在钢管终端标注钢管规格
		sDimText.Printf("%s",&guige);
		dim_pos=0.5*(f3dPoint(vertex_arr[2].x,vertex_arr[1].y)+vertex_arr[2]);
		dim_pos.x+=sys.dim_map.fDimTextSize*2;
		if(pDrawRect==NULL)
			DimAngleSize(pBlockTableRecord,f3dPoint(vertex_arr[2].x+1,vertex_arr[1].y),
			f3dPoint(vertex_arr[2].x+1,vertex_arr[2].y),dim_pos,sDimText,dimStyleId,-0.5*Pi,sys.fonts.angle.fPartGuigeTextSize);
		//标注钢管直径
		f3dPoint dim_vec(1,1,0);
		normalize(dim_vec);
		f3dPoint chordPos=draw_origin+dim_vec*half_d;
		f3dPoint farChordPos=draw_origin-dim_vec*half_d;
		double leaderLength=5;
		sDimText.Printf("Φ%.0f",pLineTube->GetDiameter());
		if(pDrawRect==NULL)
		{
			DimStyleTable::dimRadiusStyle.InitDimStyle(sys.dim_map.fDimTextSize);
			DimDiameter(pBlockTableRecord,farChordPos,chordPos,leaderLength,sDimText,DimStyleTable::dimRadiusStyle.dimStyleId);
		}
	}

	//判断钢管是否存在中间开口槽 wht 10-10-22
	PARTSET partSet;
	//得到组焊件构件(钢板)集合
	Ta.GetPartGroupPartSet(pLineTube->handle,partSet,CLS_PLATE);
	f3dLine datum_line;
	datum_line.startPt=pLineTube->Start()-pLineTube->ucs.axis_z*pLineTube->startOdd();
	datum_line.endPt=pLineTube->End()+pLineTube->ucs.axis_z*pLineTube->endOdd();
	CLDSPart *pPart=NULL;
	ATOM_LIST<MID_SLOT_INFO> slotList;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE||pPart->m_hPartWeldParent!=pLineTube->handle)
			continue;	//非钢板或焊接父构件不为当前钢管
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		//目前所谓的贯通板都是指与钢管轴线方向平行的钢板，即钢板法线应垂直与钢管轴线 wht 11-07-18
		if(fabs(pLineTube->ucs.axis_z*pPlate->ucs.axis_z)>0)
			continue;
		//判断钢板是否为贯穿钢管的钢板
		UCS_STRU temp_ucs=pPlate->ucs;
		SnapPerp(&temp_ucs.origin,datum_line,pPlate->ucs.origin);
		temp_ucs.axis_y=pLineTube->ucs.axis_z;
		temp_ucs.axis_z=pPlate->ucs.axis_z;
		temp_ucs.axis_x=temp_ucs.axis_y^temp_ucs.axis_z;
		normalize(temp_ucs.axis_x);
		temp_ucs.axis_z=temp_ucs.axis_x^temp_ucs.axis_y;
		normalize(temp_ucs.axis_z);
		//
		PROFILE_VERTEX *pVertex=NULL;
		MID_SLOT_INFO slotInfo;
		BOOL bRightVertex=FALSE,bLeftVertex=FALSE;
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlate->ucs,TRUE);
			coord_trans(vertex,temp_ucs,FALSE);
			if(vertex.x>0)
				bLeftVertex=TRUE;
			else if(vertex.x<=0)
				bRightVertex=TRUE;
			if(vertex.y>slotInfo.max_y)
				slotInfo.max_y=vertex.y;
			if(vertex.y<slotInfo.min_y)
				slotInfo.min_y=vertex.y;
		}
		if(bRightVertex&&bLeftVertex)
		{	//贯穿钢管的钢板
			slotInfo.pPlate=pPlate;
			f3dPoint origin=temp_ucs.origin;
			coord_trans(origin,pLineTube->ucs,FALSE);
			slotInfo.dist=origin.z;
			slotList.append(slotInfo);
		}
	}
	//绘制钢管中间开口槽
	int nMidSlot=slotList.GetNodeNum();
	MID_SLOT_INFO *pSlotInfo=NULL;
	//绘图比例
	double draw_scale=draw_tube_len/pLineTube->GetLength();
	for(pSlotInfo=slotList.GetFirst();pSlotInfo;pSlotInfo=slotList.GetNext())
	{	
		if(pSlotInfo->pPlate==NULL)
			continue;
		double slot_width=pSlotInfo->pPlate->GetThick()*draw_scale;
		double slot_pos_x=tube_origin.x+pSlotInfo->dist*draw_scale;
		f3dPoint slot_vertex_arr[4];
		slot_vertex_arr[0].Set(slot_pos_x+pSlotInfo->min_y*draw_scale,tube_origin.y+slot_width*0.5);
		slot_vertex_arr[1].Set(slot_pos_x+pSlotInfo->min_y*draw_scale,tube_origin.y-slot_width*0.5);
		slot_vertex_arr[2].Set(slot_pos_x+pSlotInfo->max_y*draw_scale,tube_origin.y-slot_width*0.5);
		slot_vertex_arr[3].Set(slot_pos_x+pSlotInfo->max_y*draw_scale,tube_origin.y+slot_width*0.5);
		for(int i=0;i<4;i++)
			CreateAcadLine(pBlockTableRecord,slot_vertex_arr[i],slot_vertex_arr[(i+1)%4]);

		//标注开槽起始位置
		sDimText.Printf("%.2f",(pSlotInfo->dist-fabs(pSlotInfo->min_y)));
		SimplifiedNumString(sDimText);
		dim_start.Set(tube_origin.x,tube_origin.y);
		dim_end.Set(slot_vertex_arr[0].x,tube_origin.y);
		dim_pos=0.5*(dim_start+dim_end);
		dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
		dim_pos.y-=slot_width*0.5;	//与开槽长度标注对齐
		DimAngleSize(pBlockTableRecord,f3dPoint(dim_start.x,dim_start.y-1),
			f3dPoint(dim_end.x,dim_end.y-1),dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		//标注开槽长度
		sDimText.Printf("%.2f",fabs(pSlotInfo->min_y)+fabs(pSlotInfo->max_y));
		SimplifiedNumString(sDimText);
		dim_start=slot_vertex_arr[1];
		dim_end=slot_vertex_arr[2];
		dim_pos=0.5*(dim_start+dim_end);
		dim_pos.y-=sys.dim_map.fDimTextSize*1.5;
		DimAngleSize(pBlockTableRecord,f3dPoint(dim_start.x,dim_start.y-1),
			f3dPoint(dim_end.x,dim_end.y-1),dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		//标注开槽宽度
		sDimText.Printf("%.2f",pSlotInfo->pPlate->GetThick());
		SimplifiedNumString(sDimText);
		dim_start=slot_vertex_arr[0];
		dim_end=slot_vertex_arr[1];
		dim_pos=0.5*(dim_start+dim_end);
		dim_pos.x-=sys.dim_map.fDimTextSize*1.5;
		DimAngleSize(pBlockTableRecord,f3dPoint(dim_start.x-1,dim_start.y),
			f3dPoint(dim_end.x-1,dim_end.y),dim_pos,sDimText,dimStyleId,-0.5*Pi,sys.fonts.angle.fDimTextSize);
	}
	//钢管直径大于钢板卷管直径阈值时绘制钢管展开图 wht 10-10-21
	if(pLineTube->GetDiameter()>sys.part_map.tube.fReelTubeThresholdD
		&&pDrawRect==NULL&&sys.general_set.bProcessCardTbl)
	{
		f3dPoint new_draw_origin(draw_origin.x+draw_tube_d*1.5,draw_origin.y-0.4*draw_rect.Height(),0);
		vertex_arr[0].Set(new_draw_origin.x,new_draw_origin.y+draw_tube_d*0.8);	
		vertex_arr[1].Set(new_draw_origin.x+draw_tube_len,new_draw_origin.y+draw_tube_d*0.8);
		vertex_arr[2].Set(new_draw_origin.x+draw_tube_len,new_draw_origin.y-draw_tube_d*0.8);
		vertex_arr[3].Set(new_draw_origin.x,new_draw_origin.y-draw_tube_d*0.8);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		for(int i=0;i<4;i++)
			CreateAcadLine(pBlockTableRecord,vertex_arr[i],vertex_arr[(i+1)%4]);
		//标注展开后的长度宽度以及对角线长度
		double d=pLineTube->GetDiameter();
		double t=pLineTube->GetThick();
		double plate_width=(d-t)*Pi-0.5*t;	//钢管周长计算公式(青岛武晓)
		double xie_len=sqrt(plate_width*plate_width+length*length);	//对角线长度
		//标注钢管长度
		sDimText.Printf("%.2f",length);
		SimplifiedNumString(sDimText);
		dim_pos=0.5*(vertex_arr[0]+vertex_arr[1]);
		dim_pos.y+=sys.dim_map.fDimTextSize*1.5;
		DimAngleSize(pBlockTableRecord,f3dPoint(vertex_arr[0].x,vertex_arr[0].y+1),
			f3dPoint(vertex_arr[1].x,vertex_arr[1].y+1),dim_pos,sDimText,dimStyleId,0,sys.fonts.angle.fDimTextSize);
		//标注钢管展开后的宽度
		sDimText.Printf("%.2f",plate_width);
		dim_pos=0.5*(vertex_arr[0]+vertex_arr[3]);
		dim_pos.x-=sys.dim_map.fDimTextSize*2;
		DimAngleSize(pBlockTableRecord,f3dPoint(vertex_arr[0].x-1,vertex_arr[0].y),
			f3dPoint(vertex_arr[3].x-1,vertex_arr[3].y),dim_pos,sDimText,dimStyleId,-0.5*Pi,sys.fonts.angle.fDimTextSize);
		//标注对角线长度
		sDimText.Printf("%.2f",xie_len);
		dim_pos=0.5*(vertex_arr[0]+vertex_arr[2]);
		f3dPoint dim_vec=vertex_arr[0]-vertex_arr[2];
		normalize(dim_vec);
		f3dPoint dim_norm(dim_vec.y,-dim_vec.x);
		double angle=Cal2dLineAng(f2dPoint(vertex_arr[0].x,vertex_arr[0].y),f2dPoint(vertex_arr[2].x,vertex_arr[2].y));
		DimAngleSize(pBlockTableRecord,vertex_arr[0]+dim_norm*1,vertex_arr[2]+dim_norm*1,dim_pos,sDimText,dimStyleId,angle,sys.fonts.angle.fDimTextSize);
	}
	//无工艺卡时标注构件明细
	if(!sys.general_set.bProcessCardTbl&&(pDrawRect==NULL||bDimPartInfo))
	{
		CXhChar100 sNotes;
		f3dPoint base;
		if(length-(int)length>EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			sNotes.Printf("Tower type-%s part number. %s*%d piece *%d foundation   %s length=%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("Tower type-%s part number. %s*%d piece *%d foundation  %s length=%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基   %s 长度=%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基  %s 长度=%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#endif
		
		origin.x += sys.general_set.nColumnWidth/10;
		origin.y -=250.0/sys.general_set.PartMapScale.fRealToDraw;
		base=origin;
		base.x+=draw_tube_d*2.0;
		DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
	}
	pBlockTableRecord->close();
}

//计算螺栓在环形构件上的位置 该函数已移至JgDrawing.cpp中 wht 15-03-18
//static BOOL CalLsPosInArcPart(CLDSArcPart* pArcPart,CLsRef *pLsRef,f3dPoint& ls_pos);

void TransArcAngleToLineAngle(CLDSArcAngle* pArcAngle,CLDSLineAngle& lineAngle)
{
	if(pArcAngle==NULL)
		return;
	UCS_STRU ucs;
	double length=pArcAngle->baseline.Radius()*pArcAngle->baseline.SectorAngle();
	lineAngle.set_wing_wide(pArcAngle->GetWidth());
	lineAngle.set_wing_thick(pArcAngle->GetThick());
	lineAngle.iSeg=pArcAngle->iSeg;
	lineAngle.SetPartNo(pArcAngle->GetPartNo());
	lineAngle.cMaterial=pArcAngle->cMaterial;
	lineAngle.cfgword=pArcAngle->cfgword;
	lineAngle.SetLayer(pArcAngle->layer());
	lineAngle.SetStart(f3dPoint(0,0,0));
	lineAngle.SetEnd(f3dPoint(length,0,0));
	lineAngle.set_norm_x_wing(f3dPoint(0,1,0));
	lineAngle.getUCS(ucs);
	lineAngle.cal_y_wing_norm();
	lineAngle.cal_x_wing_norm();
	f3dPoint ls_pos;
	for(CLsRef* pLsRef=pArcAngle->GetFirstLsRef();pLsRef;pLsRef=pArcAngle->GetNextLsRef())
	{
		f3dPoint pt,norm;
		CalLsPosInArcPart(pArcAngle,pLsRef,ls_pos);
		pt.z=ls_pos.x;
		if(ls_pos.y<0)	//X肢螺栓
		{
			pt.x=-ls_pos.y;
			pt.y=0;
			norm=lineAngle.get_norm_x_wing();
		}
		else			//Y肢螺栓
		{
			pt.x=0;
			pt.y=ls_pos.y;
			norm=lineAngle.get_norm_y_wing();
		}
		coord_trans(pt,ucs,TRUE);
		CLDSBolt* pBolt=(CLDSBolt*)Ta.AppendPart(CLS_BOLT);
		pBolt->SetBelongModel(&Ta);
		pBolt->CopyProperty(pLsRef->GetLsPtr());
		pBolt->set_norm(norm);
		pBolt->ucs.origin=pt;
		lineAngle.AppendStartLsRef(pBolt->GetLsRef());
	}
}
//绘制环形构件
void DrawArcPartTypeMap(CLDSArcPart *pArcPart,f2dRect *pDrawRect/*=NULL*/,int iOrder/*=-1*/,
						 int nHits/*=1*/,char *layer/*=NULL*/,int nPageNum/*=1*/,int nTaNum/*=1*/,BOOL bDimPartInfo/*=TRUE*/,int iPage/*=-1*/)
{
	if(pArcPart==NULL)
		return;
	if(pArcPart->GetClassTypeId()!=CLS_ARCANGLE &&
		pArcPart->GetClassTypeId()!=CLS_ARCFLAT &&
		pArcPart->GetClassTypeId()!=CLS_ARCSLOT)
		return;
	if(pArcPart->GetClassTypeId()==CLS_ARCANGLE)
	{
		CLDSArcAngle* pArcAngle=(CLDSArcAngle*)pArcPart;
		CLDSLineAngle lineAngle;
		lineAngle.SetBelongModel(&Ta);
		TransArcAngleToLineAngle(pArcAngle,lineAngle);
		CLDSPart *pPart=&(CLDSPart)lineAngle;
		DrawJgTypeMap(&CProcessAngle(pPart),NULL,NULL,-1,nHits,0,"temp",1,partSetDlg.m_nTaNum);
		for(CLsRef* pLsRef=lineAngle.GetFirstLsRef();pLsRef;pLsRef=lineAngle.GetNextLsRef())
		{	
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			//TODO: 将来应该通过优化CJgDrawing来避免这里向主数据库添加螺栓后再删除　wjh--2013.11.16
			Ta.DeletePart(pBolt->handle);
		}
		return;
	}
	else if(pArcPart->GetClassTypeId()==CLS_ARCSLOT||pArcPart->GetClassTypeId()==CLS_ARCFLAT)
	{
		DrawSlotOrFlatTypeMap(pArcPart,NULL,iOrder,nHits,layer,nPageNum,nTaNum,bDimPartInfo);
		return;
	}
	UCS_STRU ucs;
	pArcPart->getUCS(ucs);
	f3dPoint origin;	//原点坐标为(0,0,0)
	if(iOrder>0)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}
	//生成此构件的需要注释的内容
	double length,weight;
	CXhChar100 sPartNo,guige,guige2,ls_str,ls_str2;
	guige.Printf("L%.0fX%.0f",pArcPart->GetWidth(),pArcPart->GetThick());
	length=pArcPart->baseline.Radius()*pArcPart->baseline.SectorAngle();
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		length = ftoi(length);
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
		length = ftoi(length*2)/2.0;
	else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
		length = ftol(length*10)/10.0;	
	weight = pArcPart->GetWeight();
	strcpy(sPartNo,GetPartNoIncMat(pArcPart->GetPartNo(),pArcPart->cMaterial));
	//绘制示意图
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	f2dRect draw_rect;
	double leftGDimMargin=6;
	double draw_w=0,draw_t=0,draw_h=0,draw_len=0;
	double draw2real_scale=(1.0/sys.general_set.PartMapScale.fRealToDraw);		//绘图比例
	draw_w=pArcPart->GetWidth()*draw2real_scale;		//环形构件宽度
	draw_t=pArcPart->GetThick()*draw2real_scale;		//环形构件厚度
	draw_h=pArcPart->GetHeight()*draw2real_scale;	//环形构件侧高
	draw_len=length*draw2real_scale;					//环形构件长度
	if(sys.general_set.bProcessCardTbl)
	{	//填充工艺卡信息
		PART_JG_CARD_INFO arcPartInfo;
		memset(&arcPartInfo,'\0',sizeof(PART_JG_CARD_INFO));
		arcPartInfo.m_nPageNum=nPageNum;		//总页数
		if(iPage==-1&&iOrder<0)
			arcPartInfo.m_nPageIndex=1;		//第几页
		else if(iPage!=-1)
			arcPartInfo.m_nPageIndex=iPage+1;//第几页
		else
			arcPartInfo.m_nPageIndex=iOrder+1;//第几页
		if(nTaNum>0)
			arcPartInfo.m_nTaNum = nTaNum;
		arcPartInfo.sPartNo.Copy(GetPartNoIncMat(pArcPart->GetPartNo(),pArcPart->cMaterial));
		if(length-(int)length>EPS)
			arcPartInfo.sLength.Printf("%.1f",length);
		else
			arcPartInfo.sLength.Printf("%.0f",length);
		if(pArcPart->m_hPartWeldParent>0x20)
			arcPartInfo.sWeld.Copy(GetMarkSymbol());
		else
			arcPartInfo.sWeld.Empty();
		arcPartInfo.sPartNum.Printf("%d",nHits);
		arcPartInfo.sSumPartNum.Printf("%d",nHits*arcPartInfo.m_nTaNum);
		arcPartInfo.sPartWeight.Printf("%.1f",weight);
		arcPartInfo.sSumPartWeight.Printf("%.1f",weight*nHits*arcPartInfo.m_nTaNum);
		arcPartInfo.sNotes.Printf("%s",pArcPart->sNotes);

		CXhChar16 steelmark;
		QuerySteelMatMark(pArcPart->cMaterial,steelmark);
		arcPartInfo.sDesMaterial.Copy(steelmark);
		arcPartInfo.sDesGuiGe.Copy(guige);	
		//
		draw_rect=InsertPartProcessCardTbl(pBlockTableRecord,origin,&arcPartInfo,CLS_ARCANGLE);
		pDrawRect=&draw_rect;	//绘图区域
	}
	if(pDrawRect)
	{	
		draw_rect=*pDrawRect;
		double scale_x=1.0,scale_y=1.0;
		if(draw_len>draw_rect.Width()*0.9||draw_w>draw_rect.Height()*0.9)
		{
			double scale_y = (draw_rect.Height()*0.9)/draw_w;
			double scale_x = (draw_rect.Width()*0.9)/draw_len;
			double zoom_coef = min(scale_x,scale_y);
			draw2real_scale*=zoom_coef;
			draw_w*=zoom_coef;		//环形构件宽度
			draw_h*=zoom_coef;		//环形构件测高
			draw_t*=zoom_coef;		//环形构件厚度
			draw_len*=zoom_coef;	//环形构件长度
		}
		origin.x = draw_rect.topLeft.x;
		origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
	}
	//绘制环形构件
	f3dPoint vertex_arr[16];
	if(pArcPart->GetClassTypeId()==CLS_ARCANGLE)
	{	//环形角钢
		vertex_arr[0].Set(origin.x+leftGDimMargin,origin.y+draw_w,0);
		vertex_arr[1].Set(origin.x+draw_len+leftGDimMargin,origin.y+draw_w,0);
		vertex_arr[2].Set(origin.x+draw_len+leftGDimMargin,origin.y-draw_w,0);
		vertex_arr[3].Set(origin.x+leftGDimMargin,origin.y-draw_w,0);
		vertex_arr[4].Set(origin.x+leftGDimMargin,origin.y,0);
		vertex_arr[5].Set(origin.x+draw_len+leftGDimMargin,origin.y,0);
		vertex_arr[6].Set(origin.x+leftGDimMargin,origin.y+draw_t,0);
		vertex_arr[7].Set(origin.x+draw_len+leftGDimMargin,origin.y+draw_t,0);
		vertex_arr[8].Set(origin.x+leftGDimMargin,origin.y-draw_t,0);
		vertex_arr[9].Set(origin.x+draw_len+leftGDimMargin,origin.y-draw_t,0);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[2]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[3]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[3],vertex_arr[0]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[5]);
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[6],vertex_arr[7]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[8],vertex_arr[9]);
	}
	else if(pArcPart->GetClassTypeId()==CLS_ARCFLAT)
	{	//环形扁铁
		vertex_arr[0].Set(origin.x+leftGDimMargin,origin.y+draw_w*0.5);
		vertex_arr[1].Set(origin.x+draw_len+leftGDimMargin,origin.y+draw_w*0.5);
		vertex_arr[2].Set(origin.x+draw_len+leftGDimMargin,origin.y-draw_w*0.5);
		vertex_arr[3].Set(origin.x+leftGDimMargin,origin.y-draw_w*0.5);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[2]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[3]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[3],vertex_arr[0]);
	}
	else if(pArcPart->GetClassTypeId()==CLS_ARCSLOT)
	{	//环形槽钢
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		vertex_arr[0].Set(origin.x+leftGDimMargin,origin.y+draw_h*0.5+draw_w,0);
		vertex_arr[1].Set(origin.x+draw_len+leftGDimMargin,origin.y+draw_h*0.5+draw_w,0);
		vertex_arr[2].Set(origin.x+draw_len+leftGDimMargin,origin.y-draw_h*0.5-draw_w,0);
		vertex_arr[3].Set(origin.x+leftGDimMargin,origin.y-draw_h*0.5-draw_w,0);
		vertex_arr[4].Set(origin.x+leftGDimMargin,origin.y+draw_h*0.5,0);
		vertex_arr[5].Set(origin.x+draw_len+leftGDimMargin,origin.y+draw_h*0.5,0);
		vertex_arr[6].Set(origin.x+leftGDimMargin,origin.y-draw_h*0.5,0);
		vertex_arr[7].Set(origin.x+draw_len+leftGDimMargin,origin.y-draw_h*0.5,0);
		vertex_arr[8].Set(origin.x+leftGDimMargin,origin.y+draw_h*0.5+draw_t,0);
		vertex_arr[9].Set(origin.x+draw_len+leftGDimMargin,origin.y+draw_h*0.5+draw_t,0);
		vertex_arr[10].Set(origin.x+leftGDimMargin,origin.y+draw_h*0.5-draw_t,0);
		vertex_arr[11].Set(origin.x+draw_len+leftGDimMargin,origin.y+draw_h*0.5-draw_t,0);
		vertex_arr[12].Set(origin.x+leftGDimMargin,origin.y-draw_h*0.5+draw_t,0);
		vertex_arr[13].Set(origin.x+draw_len+leftGDimMargin,origin.y-draw_h*0.5+draw_t,0);
		vertex_arr[14].Set(origin.x+leftGDimMargin,origin.y-draw_h*0.5-draw_t,0);
		vertex_arr[15].Set(origin.x+draw_len+leftGDimMargin,origin.y-draw_h*0.5-draw_t,0);
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[0],vertex_arr[1]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[1],vertex_arr[2]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[2],vertex_arr[3]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[3],vertex_arr[0]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[4],vertex_arr[5]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[6],vertex_arr[7]);
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		CreateAcadLine(pBlockTableRecord,vertex_arr[8],vertex_arr[9]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[10],vertex_arr[11]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[12],vertex_arr[13]);
		CreateAcadLine(pBlockTableRecord,vertex_arr[14],vertex_arr[15]);
	}
	//绘制螺栓时使用螺栓图层
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	CXhChar16 sM12("M12"),sM16("M16"),sM18("M18"),sM20("M20"),sM22("M22"),sM24("M24");
	if(sys.part_map.plate.bPlateUseSimpleLsMap)
	{
		sM12.Copy("M12_Jg");
		sM16.Copy("M16_Jg");
		sM18.Copy("M18_Jg");
		sM20.Copy("M20_Jg");
		sM22.Copy("M22_Jg");
		sM24.Copy("M24_Jg");
	}
	//绘制螺栓孔
	CArray<double,double&> dimLsDistArr;
	CArray<double,double&> dimXLsDistArr;
	CArray<double,double&> dimYLsDistArr;
	f3dPoint ls_pos,centre;
	for(CLsRef *pLsRef=pArcPart->GetFirstLsRef();pLsRef;pLsRef=pArcPart->GetNextLsRef())
	{
		CalLsPosInArcPart(pArcPart,pLsRef,ls_pos);
		if(pArcPart->GetClassTypeId()==CLS_ARCANGLE)
		{
			if(ls_pos.y>0)	//X肢螺栓
				dimXLsDistArr.Add(ls_pos.x);
			else			//Y肢螺栓
				dimYLsDistArr.Add(ls_pos.x);
		}
		else
			dimLsDistArr.Add(ls_pos.x);
		centre.x=origin.x+ls_pos.x*draw2real_scale+leftGDimMargin;
		centre.y=origin.y+ls_pos.y*draw2real_scale;
		double radius=pLsRef->GetLsPtr()->get_d()*0.5;
		BOOL bRet=FALSE;
		if(sys.general_set.PartMapScale.iBoltSymbolStyle==1)	//图符处理
		{
			if(fabs(2*radius-12)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM12,1.0,0);
			else if(fabs(2*radius-16)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM16,1.0,0);
			else if(fabs(2*radius-18)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM18,1.0,0);
			else if(fabs(2*radius-20)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM20,1.0,0);
			else if(fabs(2*radius-22)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM22,1.0,0);
			else if(fabs(2*radius-24)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM24,1.0,0);
			else
			{
				CXhChar16 ls_str;
				ls_str.Printf("M%d",ftol(2*radius));
				bRet=InsertBlock(pBlockTableRecord,centre,ls_str,1.0,0);
			}
		}
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)	//比例图符处理
		{
			double fZoomScale=draw2real_scale*sys.general_set.PartMapScale.fBoltSymbolFactor;
			if(fabs(2*radius-12)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM12,fZoomScale,0);
			else if(fabs(2*radius-16)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM16,fZoomScale,0);
			else if(fabs(2*radius-18)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM18,fZoomScale,0);
			else if(fabs(2*radius-20)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM20,fZoomScale,0);
			else if(fabs(2*radius-22)<EPS)							
				bRet=InsertBlock(pBlockTableRecord,centre,sM22,fZoomScale,0);
			else if(fabs(2*radius-24)<EPS)							
				bRet=InsertBlock(pBlockTableRecord,centre,sM24,fZoomScale,0);
			else
			{
				CXhChar16 ls_str;
				ls_str.Printf("M%d",ftol(2*radius));
				bRet=InsertBlock(pBlockTableRecord,centre,ls_str,fZoomScale,0);
			}
		}
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)	//比例直径图符处理
		{
			double fZoomScale=(radius*2+(*pLsRef)->hole_d_increment+sys.general_set.fHoleAugmentInNC)*draw2real_scale*sys.general_set.PartMapScale.fBoltDiaFactor;
			if(fabs(2*radius-12)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM12,fZoomScale,0);
			else if(fabs(2*radius-16)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM16,fZoomScale,0);
			else if(fabs(2*radius-18)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM18,fZoomScale,0);
			else if(fabs(2*radius-20)<EPS)
				bRet=InsertBlock(pBlockTableRecord,centre,sM20,fZoomScale,0);
			else if(fabs(2*radius-22)<EPS)							
				bRet=InsertBlock(pBlockTableRecord,centre,sM22,fZoomScale,0);
			else if(fabs(2*radius-24)<EPS)							
				bRet=InsertBlock(pBlockTableRecord,centre,sM24,fZoomScale,0);
			else
			{
				CXhChar16 ls_str;
				ls_str.Printf("M%d",ftol(2*radius));
				bRet=InsertBlock(pBlockTableRecord,centre,ls_str,fZoomScale,0);
			}										
		}											
		if(sys.general_set.PartMapScale.iBoltSymbolStyle==0||!bRet)	//圆孔处理				
		{		
			f3dCircle Cir;
			Cir.centre=centre;
			Cir.radius=radius;
			Cir.radius+=sys.general_set.fHoleAugmentInNC/2.0;
			Cir.radius*=draw2real_scale;
			NewBoltCircle(pBlockTableRecord,Cir);
		}
	}
	dimXLsDistArr.Add(length);
	CQuickSort<double>::QuickSort(dimXLsDistArr.GetData(),dimXLsDistArr.GetSize(),compare_double);
	dimYLsDistArr.Add(length);
	CQuickSort<double>::QuickSort(dimYLsDistArr.GetData(),dimYLsDistArr.GetSize(),compare_double);
	dimLsDistArr.Add(length);
	CQuickSort<double>::QuickSort(dimLsDistArr.GetData(),dimLsDistArr.GetSize(),compare_double);
	//标注螺栓相对距离
	//GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	/*GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	AcDbObjectId dimStyleId=DimStyleTable::dimStyle.dimStyleId;
	double dim_dist=1,ls_dist,pre_dist=0;
	f3dPoint dim_start,dim_end,dim_pos;
	CXhChar100 sDimText;
	if(dimXLsDistArr.GetSize()>1)
	{
		for(int i=0;i<dimXLsDistArr.GetSize();i++)
		{
			ls_dist=dimXLsDistArr[i]-pre_dist;
			sDimText.Printf("%.0f",ls_dist);
			SimplifiedNumString(sDimText);
			dim_start.x=origin.x+pre_dist*scale+leftGDimMargin;
			dim_start.y=origin.y+draw_w+dim_dist;
			dim_end.x=dim_start.x+ls_dist*scale;
			dim_end.y=dim_start.y;
			dim_pos=0.5*(dim_start+dim_end);
			dim_pos.y+=dim_dist;
			DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,-Pi);
			pre_dist=dimXLsDistArr[i];
		}
	}
	if(dimYLsDistArr.GetSize()>1)
	{
		pre_dist=0;
		for(int i=0;i<dimYLsDistArr.GetSize();i++)
		{
			ls_dist=dimYLsDistArr[i]-pre_dist;
			sDimText.Printf("%.0f",ls_dist);
			SimplifiedNumString(sDimText);
			dim_start.x=origin.x+pre_dist*scale+leftGDimMargin;
			dim_start.y=origin.y-draw_w-dim_dist;
			dim_end.x=dim_start.x+ls_dist*scale;
			dim_end.y=dim_start.y;
			dim_pos=0.5*(dim_start+dim_end);
			dim_pos.y-=dim_dist*5;
			DimAngleSize(pBlockTableRecord,dim_start,dim_end,dim_pos,sDimText,dimStyleId,0);
			pre_dist=dimYLsDistArr[i];
		}
	}*/
	//无工艺卡时标注构件明细
	if(!sys.general_set.bProcessCardTbl&&bDimPartInfo)
	{
		CXhChar100 sNotes;
		f3dPoint base;
		if(length-(int)length>EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			sNotes.Printf("Tower type-%s part number. %s*%d piece *%d foundation  %s length =%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("tower type-%s part number. %s*%d piece *%d foundation  %s length =%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基  %s 长度=%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基  %s 长度=%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#endif
		origin.x += sys.general_set.nColumnWidth/10;
		origin.y -=250.0/sys.general_set.PartMapScale.fRealToDraw;
		base=origin;
		base.x+=draw_w*2.0;
		DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
	}
	pBlockTableRecord->close();
}

//绘制槽钢
void DrawSlotOrFlatTypeMap(CLDSPart *pPart,f2dRect *pDrawRect/*=NULL*/,int iOrder/*=-1*/,
					 int nHits/*=1*/,char *layer/*=NULL*/,int nPageNum/*=1*/,int nTaNum/*=1*/,
					 BOOL bDimPartInfo/*=TRUE*/,int iPage/*=-1*/)
{
	if(  pPart==NULL||
		(pPart->GetClassTypeId()!=CLS_LINESLOT&&pPart->GetClassTypeId()!=CLS_ARCSLOT&&
		 pPart->GetClassTypeId()!=CLS_LINEFLAT&&pPart->GetClassTypeId()!=CLS_ARCFLAT))
		return;
	f3dPoint origin;	//原点坐标为(0,0,0)
	if(iOrder>0)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (iOrder%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (iOrder/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(iOrder%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
	}
	
	//生成此构件的需要注释的内容
	double length,weight;
	CXhChar100 sPartNo,guige,guige2,ls_str,ls_str2;
	pPart->GetSpecification(guige);
	length=pPart->GetLength();
	//添加以下代码主要是为了使工艺卡上的角钢长度与中间大样图上的角钢总长标注小数点后一位也一致
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		length = ftoi(length);
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
		length = ftoi(length*2)/2.0;
	else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
		length = ftol(length*10)/10.0;	
	weight = pPart->GetWeight();
	strcpy(sPartNo,GetPartNoIncMat(pPart->GetPartNo(),pPart->cMaterial));
	//获得当前图形的块表指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	f2dRect draw_rect;
	if(sys.general_set.bProcessCardTbl)
	{	//填充工艺卡信息
		PART_JG_CARD_INFO SlotInfo;
		memset(&SlotInfo,'\0',sizeof(PART_JG_CARD_INFO));
		SlotInfo.m_nPageNum=nPageNum;		//总页数
		if(iPage==-1&&iOrder<0)
			SlotInfo.m_nPageIndex=1;		//第几页
		else if(iPage!=-1)
			SlotInfo.m_nPageIndex=iPage+1;//第几页
		else
			SlotInfo.m_nPageIndex=iOrder+1;//第几页
		if(nTaNum>0)
			SlotInfo.m_nTaNum = nTaNum;
		SlotInfo.sPartNo.Copy(GetPartNoIncMat(pPart->GetPartNo(),pPart->cMaterial));
		if(length-(int)length>EPS)
			SlotInfo.sLength.Printf("%.1f",length);
		else
			SlotInfo.sLength.Printf("%.0f",length);
		if(pPart->m_hPartWeldParent>0x20)
			SlotInfo.sWeld.Copy(GetMarkSymbol());
		else
			SlotInfo.sWeld.Empty();
		SlotInfo.sPartNum.Printf("%d",nHits);
		SlotInfo.sSumPartNum.Printf("%d",nHits*SlotInfo.m_nTaNum);
		SlotInfo.sPartWeight.Printf("%.1f",weight);
		SlotInfo.sSumPartWeight.Printf("%.1f",weight*nHits*SlotInfo.m_nTaNum);
		SlotInfo.sNotes.Printf("%s",pPart->sNotes);
		
		CXhChar16 steelmark;
		QuerySteelMatMark(pPart->cMaterial,steelmark);
		SlotInfo.sDesMaterial.Copy(steelmark);
		SlotInfo.sDesGuiGe.Copy(guige);	
		//
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		draw_rect=InsertPartProcessCardTbl(pBlockTableRecord,origin,&SlotInfo,CLS_LINESLOT);
	}
	else if(pDrawRect)
		draw_rect=*pDrawRect;	//绘图区域
	else 
		draw_rect=GetProcessCardDrawingRect(5);
	origin.x = draw_rect.topLeft.x;
	origin.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)/2;
	if(pPart->GetClassTypeId()==CLS_LINESLOT||pPart->GetClassTypeId()==CLS_ARCSLOT)
	{
		CSlotDrawing xSlotDraw;
		xSlotDraw.CreateSlotDrawing(pPart,draw_rect.Width(),draw_rect.Height(),sys.dim_map.fDimTextSize);
		DrawJgMap(&xSlotDraw,origin,FALSE,pBlockTableRecord);
	}
	else 
	{
		CFlatDrawing xFlatDraw;
		xFlatDraw.CreateFlatDrawing(pPart,draw_rect.Width(),draw_rect.Height(),sys.dim_map.fDimTextSize);
		DrawJgMap(&xFlatDraw,origin,FALSE,pBlockTableRecord);
	}
	//无工艺卡时标注构件明细
	if(!sys.general_set.bProcessCardTbl&&bDimPartInfo)
	{
		CXhChar100 sNotes;
		f3dPoint base;
		if(length-(int)length>EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			sNotes.Printf("tower type-%s piece number. %s*%d piece *%d foundation  %s length =%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("Tower type-%s part number. %s*%d piece *%d foundation  %s Length=%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基  %s 长度=%.1f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
		else
			sNotes.Printf("塔型-%s 件号. %s*%d件*%d基  %s 长度=%.0f mm",Ta.m_sTaType,(char*)sPartNo,nHits,nTaNum,(char*)guige,length);
#endif
		
		origin.x += sys.general_set.nColumnWidth/10;
		origin.y -=250.0/sys.general_set.PartMapScale.fRealToDraw;
		base=origin;
		base.y -= (pPart->GetWidth()+100)/sys.general_set.PartMapScale.fRealToDraw+sys.dim_map.PartNo.fPartNoTextSize*6;
		if(pDrawRect)
		{	//根据指定的矩形框,计算备注标注位置 wht 11-06-02
			base.x = sNotes.GetLength()*sys.dim_map.PartNo.fPartNoTextSize*0.5;
			base.y = sys.dim_map.PartNo.fPartNoTextSize*5;
		}
		DimText(pBlockTableRecord,base,sNotes,TextStyleTable::hzfs.textStyleId,sys.dim_map.PartNo.fPartNoTextSize,0);
	}
	pBlockTableRecord->close();
}

//排列钢板
#include "LogFile.h"
void DisplayProcess(int percent,const char *sTitle);
f2dRect GetMinWrapRect(CLDSPart *pPart,UCS_STRU *pRectucs/*=NULL*/,double minDistance/*=0*/);
//ciArrangeMode 0:自动排版|1:按段分组|2:按设置
void ArrangeAndDrawPlate(PARTSET &partList,CXhChar200 export_info,int nPaperL/*=0*/,int nPaperW/*=1000*/,int minDistance/*=0*/,
						int nTaNum/*=1*/,BOOL bPatternDrawing/*TRUE*/,BYTE ciArrangeMode/*=0*/)
{
	CHashStrList<CDrawingRect> hashDrawingRectByLabel;
	ATOM_LIST<CPlankDrawing>plateDrawList;
	CHashList<int> hashPartCountBySegI;
	DYN_ARRAY<SEGREC> segarr;
	//初始化钢板的绘图区域信息
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"create plate drawing");
#else
	DisplayProcess(0,"生成钢板大样图");
#endif
	int n=partList.GetNodeNum(),i=0;
	for (CLDSPart *pPart=partList.GetFirst();pPart;pPart=partList.GetNext())
	{
		i++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(ftoi(100*i/n),"create plate drawing");
#else
		DisplayProcess(ftoi(100*i/n),"生成钢板大样图");
#endif
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
		if(pPlate->vertex_list.GetNodeNum()==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sPrompt("\n0X%x plate vertex number is 0，please confirm!\n",pPart->handle);
#else
			CXhChar100 sPrompt("\n0X%x的钢板顶点数为0，请确认!\n",pPart->handle);
#endif
#ifdef _ARX_2007
			acedPrompt((ACHAR*)_bstr_t(sPrompt));
#else
			acedPrompt(sPrompt);
#endif
			continue;
		}
		//统计段号
		int *pnPartNum=hashPartCountBySegI.GetValue((DWORD)pPlate->iSeg);
		if(pnPartNum==NULL)
			hashPartCountBySegI.SetValue((DWORD)pPlate->iSeg,1);
		else
			hashPartCountBySegI.SetValue((DWORD)pPlate->iSeg,*pnPartNum+1);
		//统计钢板的绘制图元信息
		CPlankDrawing *pPlateDraw = plateDrawList.append();
		if(ciArrangeMode==0)
		{	//自动排版时,指定钢板的绘图坐标系
			UCS_STRU Rectucs;	
			pPlate->GetMinWrapRect(&Rectucs,0,sys.part_map.bExportDeformedProfile);
			pPlateDraw->SetNewUcs(pPlate,Rectucs);
		}
		//设置钢板的标注样式
		if(sys.part_map.plate.bDimLsToWeldEdge)
			pPlateDraw->dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToWeldEdge);
		if(sys.part_map.plate.bDimLsToHuoquLine)
			pPlateDraw->dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToHuoQuLine);
		if(sys.part_map.plate.bDimDiffRayLsSpace)
			pPlateDraw->dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToNextLsRay);
		if(sys.part_map.plate.bDimBoltEdgeSpace)
			pPlateDraw->dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltSpaceToEdgeLine);
		if(sys.part_map.plate.bDimPlateBoltAngle)
			pPlateDraw->dimStyleFlag.SetFlagDword(CPlankDrawing::DimBoltAngle);
		pPlateDraw->Create(pPlate,bPatternDrawing);
		//获取绘制区域
		f2dRect minRect=pPlateDraw->GetDrawRegion(1);
		if(ciArrangeMode==0 && minDistance>0)
		{	//自动排版时需将区域等距扩大指定偏移量，便于布局
			f3dPoint ptArr[4],extendPtArr[4];
			ptArr[0].Set(minRect.topLeft.x,minRect.topLeft.y,0);
			ptArr[1].Set(minRect.bottomRight.x,minRect.topLeft.y,0);
			ptArr[2].Set(minRect.bottomRight.x,minRect.bottomRight.y,0);
			ptArr[3].Set(minRect.topLeft.x,minRect.bottomRight.y,0);
			for(int i=0;i<4;i++)
			{
				f3dPoint pre_vec=(ptArr[i]-ptArr[(i+3)%4]).normalized();
				f3dPoint nxt_vec=(ptArr[i]-ptArr[(i+1)%4]).normalized();
				f3dPoint off_vec=(pre_vec+nxt_vec).normalized();
				double angle=cal_angle_of_2vec(pre_vec,nxt_vec);
				double offset=minDistance/sin(angle/2);
				extendPtArr[i]= ptArr[i]+off_vec*offset;
			}
			minRect.topLeft.Set(extendPtArr[0].x,extendPtArr[0].y);
			minRect.bottomRight.Set(extendPtArr[2].x,extendPtArr[2].y);
		}
		//
		CDrawingRect *pRect=hashDrawingRectByLabel.Add(pPlate->GetPartNo());
		pRect->m_pDrawing=pPlateDraw;
		pRect->height=minRect.Height();
		pRect->width=minRect.Width();
		pRect->initTopLeft.Set(minRect.topLeft.x,minRect.topLeft.y);
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"create plate drawing");
#else
	DisplayProcess(100,"生成钢板大样图");
#endif
	if(plateDrawList.GetNodeNum()<=0)
		return;
	if(ciArrangeMode==1)
	{	//按段分组
		int i=0;
		segarr.Resize(hashPartCountBySegI.GetNodeNum());
		for(int *piPartCount=hashPartCountBySegI.GetFirst();piPartCount;piPartCount=hashPartCountBySegI.GetNext())
		{
			segarr[i].iSeg=hashPartCountBySegI.GetCursorKey();
			segarr[i].nPageCount=*piPartCount;
			i++;
		}
		CQuickSort<SEGREC>::QuickSort(segarr,segarr.Size(),_LocalCompareSegI);
	}
	else if(ciArrangeMode==2)
	{
		segarr.Resize(1);
		segarr[0].iSeg=-1;
	}
	//排版并绘制
	DRAGSET.ClearEntSet();
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	int nCurProcess=0,nNum=hashDrawingRectByLabel.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"diagram showing discharge results");
#else
	DisplayProcess(0,"显示排料结果图");
#endif
	if(ciArrangeMode==0)		
	{	//自动排版
		int nPage=0;
		double fPaperX=0,fMapL=nPaperL;
		double paperLen=(nPaperL<=0)?10000000000000000:(double)nPaperL;
		while(hashDrawingRectByLabel.GetNodeNum()>0)
		{
			nPage++;
			CDrawingRectLayout rectLayout;
			for(CDrawingRect *pRect=hashDrawingRectByLabel.GetFirst();pRect;pRect=hashDrawingRectByLabel.GetNext())
				rectLayout.drawRectArr.Add(*pRect);
			if(rectLayout.RelayoutTemp(nPaperW,paperLen)==FALSE)
			{
				for(CDrawingRect *pRect=hashDrawingRectByLabel.GetFirst();pRect;pRect=hashDrawingRectByLabel.GetNext())
				{	
					CPlankDrawing *pPlateDraw=(CPlankDrawing*)pRect->m_pDrawing;
					if(pPlateDraw)
						logerr.Log("钢板%s排版失败!",(char*)pPlateDraw->GetPlate()->GetPartNo());
				}
				break;
			}
			else
			{
				//绘制钢板
				for(int i=0;i<rectLayout.drawRectArr.GetSize();i++)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					DisplayProcess(ftoi(100*nCurProcess/nNum),"diagram showing discharge results");
#else
					DisplayProcess(ftoi(100*nCurProcess/nNum),"显示排料结果图");
#endif
					CDrawingRect drawingRect=rectLayout.drawRectArr[i];	
					if(!drawingRect.IsLayout())
						continue;
					f3dPoint topLeft(drawingRect.topLeft.x+fPaperX,drawingRect.topLeft.y);
					f3dPoint origin=topLeft-drawingRect.initTopLeft;
					CPlankDrawing *pPlateDraw=(CPlankDrawing*)drawingRect.m_pDrawing;
					if(pPlateDraw)
						DrawPlateMap(pPlateDraw,pBlockTableRecord,origin,export_info,nTaNum);
					//
					hashDrawingRectByLabel.DeleteNode(pPlateDraw->GetPlate()->GetPartNo());
					nCurProcess++;
					if(nPaperL<=0)	//不指定图纸长度
						fMapL=max(fMapL,origin.x+drawingRect.width);
				}
				//输出钢板布局
				CreateAcadLine(pBlockTableRecord,f3dPoint(fPaperX,0),f3dPoint(fPaperX+fMapL,0));
				CreateAcadLine(pBlockTableRecord,f3dPoint(fPaperX,-nPaperW),f3dPoint(fPaperX+fMapL,-nPaperW));
				CreateAcadLine(pBlockTableRecord,f3dPoint(fPaperX,0),f3dPoint(fPaperX,-nPaperW));
				CreateAcadLine(pBlockTableRecord,f3dPoint(fPaperX+fMapL,0),f3dPoint(fPaperX+fMapL,-nPaperW));
				GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
				f3dPoint center(fPaperX+fMapL-8,-nPaperW+8);
				CreateAcadCircle(pBlockTableRecord,center,8);
				DimText(pBlockTableRecord,center,CXhChar16("%d",nPage),TextStyleTable::hzfs.textStyleId,15,0,AcDb::kTextCenter,AcDb::kTextVertMid);
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
				//
				fPaperX+=(paperLen+50);
			}
		}
	}
	else if(ciArrangeMode!=0)
	{	//按行、列排版
		CDrawingRectLayout rectLayout;
		for(CDrawingRect *pRect=hashDrawingRectByLabel.GetFirst();pRect;pRect=hashDrawingRectByLabel.GetNext())
			rectLayout.drawRectArr.Add(*pRect);
		if(!rectLayout.RelayoutPlateDraw(minDistance,segarr))
			logerr.Log("按行或列排版失败!");
		else
		{
			for(int i=0;i<nNum;i++)
			{
				nCurProcess++;
#ifdef AFX_TARG_ENU_ENGLISH
				DisplayProcess(ftoi(100*nCurProcess/nNum),"diagram showing discharge results");
#else
				DisplayProcess(ftoi(100*nCurProcess/nNum),"显示排料结果图");
#endif
				CDrawingRect drawingRect=rectLayout.drawRectArr[i];
				f3dPoint origin=drawingRect.topLeft-drawingRect.initTopLeft;
				CPlankDrawing *pPlateDraw=(CPlankDrawing*)drawingRect.m_pDrawing;
				if(pPlateDraw)
					DrawPlateMap(pPlateDraw,pBlockTableRecord,origin,export_info,nTaNum);
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"diagram showing discharge results");
#else
	DisplayProcess(100,"显示排料结果图");
#endif
	pBlockTableRecord->close();
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
	ads_command(RTSTR,L"re",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
	ads_command(RTSTR,"re",RTNONE);
#endif
	ads_point basepnt;
	basepnt[X]=0;
	basepnt[Y]=0;
	basepnt[Z]=0;
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point\n");
#else
	DragEntSet(basepnt,"请输入插入点\n");
#endif
}

//绘制制弯角钢拓印图
void PolyJgBendDrawing()
{
	sys.m_iCurMapType = 4;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	//
	CAcModuleResourceOverride resOverride;
	InitDrawingEnv();
	CPolyJgBendDrawDlg polyJgDlg;
	if(polyJgDlg.DoModal()!=IDOK)
		return;
	double minDistance=polyJgDlg.m_fMinDist;
	double zoomCoef=1.0/polyJgDlg.m_fDrawScale;
	CDrawingRectLayout rectLayout;
	ATOM_LIST<CPolyAngleDrawing> polyJgDrawList;
	for(CLDSPart *pPart=polyJgDlg.partSet.GetFirst();pPart;pPart=polyJgDlg.partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_POLYPART)
			continue;
		CLDSPolyPart* pPolyPart=(CLDSPolyPart*)pPart;
		//统计钢板的绘制图元信息
		CPolyAngleDrawing *pPolyAngleDraw = polyJgDrawList.append();
		pPolyAngleDraw->CreatePolyAngleBendDrawing(pPolyPart);
		//获取绘制区域,进行等距扩大指定偏移量，便于布局
		f2dRect minRect=pPolyAngleDraw->GetDrawRegion(1);
		f3dPoint ptArr[4],extendPtArr[4];
		ptArr[0].Set(minRect.topLeft.x,minRect.topLeft.y,0);
		ptArr[1].Set(minRect.bottomRight.x,minRect.topLeft.y,0);
		ptArr[2].Set(minRect.bottomRight.x,minRect.bottomRight.y,0);
		ptArr[3].Set(minRect.topLeft.x,minRect.bottomRight.y,0);
		for(int i=0;i<4;i++)
		{
			f3dPoint pre_vec=(ptArr[i]-ptArr[(i+3)%4]).normalized();
			f3dPoint nxt_vec=(ptArr[i]-ptArr[(i+1)%4]).normalized();
			f3dPoint off_vec=(pre_vec+nxt_vec).normalized();
			double angle=cal_angle_of_2vec(pre_vec,nxt_vec);
			double offset=minDistance/sin(angle/2);
			extendPtArr[i]= ptArr[i]+off_vec*offset;
		}
		minRect.topLeft.Set(extendPtArr[0].x,extendPtArr[0].y);
		minRect.bottomRight.Set(extendPtArr[2].x,extendPtArr[2].y);
		//
		CDrawingRect drawingRect;
		drawingRect.m_pDrawing=pPolyAngleDraw;
		drawingRect.height=minRect.Height();
		drawingRect.width=minRect.Width();
		drawingRect.initTopLeft.Set(minRect.topLeft.x,minRect.topLeft.y);
		rectLayout.drawRectArr.Add(drawingRect);
	}
	if(!rectLayout.RelayoutDraw(minDistance))
		return;
	DRAGSET.ClearEntSet();
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(0,"diagram showing discharge results");
#else
	DisplayProcess(0,"生成火曲角钢拓印图");
#endif
	int nWidth=0,nCurProcess=0,nNum=rectLayout.drawRectArr.GetSize();
	for(int j=0;j<nNum;j++)
	{
		nCurProcess++;
#ifdef AFX_TARG_ENU_ENGLISH
		DisplayProcess(ftoi(100*nCurProcess/nNum),"diagram showing discharge results");
#else
		DisplayProcess(ftoi(100*nCurProcess/nNum),"生成火曲角钢拓印图");
#endif
		CDrawingRect drawingRect=rectLayout.drawRectArr[j];	
		if(drawingRect.topLeft.x>nWidth)
			nWidth=(int)drawingRect.topLeft.x;
		f3dPoint origin=drawingRect.topLeft-drawingRect.initTopLeft;
		CPolyAngleDrawing* pHuoJgDraw=(CPolyAngleDrawing*)drawingRect.m_pDrawing;
		//轮廓线
		f3dLine line;
		for(int i=0;i<pHuoJgDraw->GetLineCount();i++)
		{
			pHuoJgDraw->GetLineAt(line,i);
			trans_pos(line,origin);
			line.startPt*=zoomCoef;
			line.endPt*=zoomCoef;
			if(line.feature==0)
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			else
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
		}
		//螺栓
		GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
		CXhChar16 sM12("M12"),sM16("M16"),sM18("M18"),sM20("M20"),sM22("M22"),sM24("M24");
		f3dCircle Cir;
		for(int i=0;i<pHuoJgDraw->GetXLsCount();i++)
		{
			pHuoJgDraw->GetXLsCircle(Cir,i);
			if(Cir.radius<EPS)
				continue;	//非螺栓孔特征点,如楞线上的制弯点
			double fZoomScale=1;
			if(sys.general_set.PartMapScale.iBoltSymbolStyle==1)	//图符处理
				fZoomScale=1;
			else if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)	//比例图符处理
				fZoomScale=zoomCoef*sys.general_set.PartMapScale.fBoltSymbolFactor;
			else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)	//比例直径图符处理
				fZoomScale=(Cir.radius*2+Cir.feature/10.0+sys.general_set.fHoleAugmentInNC)*zoomCoef*sys.general_set.PartMapScale.fBoltDiaFactor;
			Cir.centre+=origin;
			BOOL bRetCode=FALSE;
			if(fabs(2*Cir.radius-12)<EPS)	//按3mm螺栓直径绘制
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,fZoomScale,0);
			else if(fabs(2*Cir.radius-16)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,fZoomScale,0);
			else if(fabs(2*Cir.radius-20)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,fZoomScale,0);
			else if(fabs(2*Cir.radius-22)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,fZoomScale,0);
			else if(fabs(2*Cir.radius-24)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,fZoomScale,0);
			else
				NewBoltCircle(pBlockTableRecord,Cir);
		}
		for(int i=0;i<pHuoJgDraw->GetYLsCount();i++)
		{
			pHuoJgDraw->GetYLsCircle(Cir,i);
			if(Cir.radius<EPS)
				continue;	//非螺栓孔特征点,如楞线上的制弯点
			double fZoomScale=1;
			if(sys.general_set.PartMapScale.iBoltSymbolStyle==1)	//图符处理
				fZoomScale=1;
			else if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)	//比例图符处理
				fZoomScale=zoomCoef*sys.general_set.PartMapScale.fBoltSymbolFactor;
			else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)	//比例直径图符处理
				fZoomScale=(Cir.radius*2+Cir.feature/10.0+sys.general_set.fHoleAugmentInNC)*zoomCoef*sys.general_set.PartMapScale.fBoltDiaFactor;
			Cir.centre+=origin;
			BOOL bRetCode=FALSE;
			if(fabs(2*Cir.radius-12)<EPS)	//按3mm螺栓直径绘制
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM12,fZoomScale,0);
			else if(fabs(2*Cir.radius-16)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM16,fZoomScale,0);
			else if(fabs(2*Cir.radius-20)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM20,fZoomScale,0);
			else if(fabs(2*Cir.radius-22)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM22,fZoomScale,0);
			else if(fabs(2*Cir.radius-24)<EPS)
				bRetCode=InsertBlock(pBlockTableRecord,Cir.centre,sM24,fZoomScale,0);
			else
				NewBoltCircle(pBlockTableRecord,Cir);
		}
		//间距标注
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		CSizeTextDim sizeDim;
		for(i=0;i<pHuoJgDraw->GetDimCount();i++)
		{
			pHuoJgDraw->GetDimAt(i,&sizeDim);
			sizeDim.dimPos+=origin;
			sizeDim.dimStartPos+=origin;
			sizeDim.dimEndPos+=origin;
			DimAngleSize(pBlockTableRecord,sizeDim.dimStartPos,sizeDim.dimEndPos,sizeDim.dimPos,CXhChar16("%.1f",sizeDim.dist),
				DimStyleTable::dimStyle.dimStyleId,sizeDim.angle,sys.fonts.angle.fDimTextSize);
		}
		//角度标注
		ANGLE_DIM_STRU angular_dim;
		for(i=0;i<pHuoJgDraw->GetAngleDimCount();i++)
		{
			pHuoJgDraw->GetAngleDimAt(i,&angular_dim);
			angular_dim.vertex*=zoomCoef;
			angular_dim.startPos*=zoomCoef;
			angular_dim.endPos*=zoomCoef;
			angular_dim.dimPos*=zoomCoef;
			angular_dim.vertex+=origin;
			angular_dim.startPos+=origin;
			angular_dim.endPos+=origin;
			angular_dim.dimPos+=origin;
			DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,angular_dim.vertex,angular_dim.startPos,
				angular_dim.endPos,angular_dim.dimPos,angular_dim.sDimText);
		}
		//文字标注
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		for(i=0;i<pHuoJgDraw->GetHuoQuTextCount();i++)
		{
			CTextOnlyDrawing* pTextDim=pHuoJgDraw->GetHuoQuTxtDimAt(i);
			pTextDim->dimPos+=origin;
			DimText(pBlockTableRecord,pTextDim->dimPos,pTextDim->dimText,TextStyleTable::hzfs.textStyleId,pTextDim->textHeight,pTextDim->angle,AcDb::kTextLeft);
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	DisplayProcess(100,"diagram showing discharge results");
#else
	DisplayProcess(100,"生成火曲角钢拓印图");
#endif
	pBlockTableRecord->close();
	logerr.ShowToScreen();
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
	ads_command(RTSTR,L"re",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
	ads_command(RTSTR,"re",RTNONE);
#endif
	ads_point basepnt;
	basepnt[X]=0;
	basepnt[Y]=0;
	basepnt[Z]=0;
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point\n");
#else
	DragEntSet(basepnt,"请输入插入点\n");
#endif
}

void DrawPlateMap(CPlankDrawing *pPlateDraw,AcDbBlockTableRecord *pBlockTableRecord,f3dPoint origin,
							CXhChar200 export_info,int nTaNum/*=1*/,double zoomCoef/*=1.0*/)
{
	if(pBlockTableRecord==NULL || pPlateDraw==NULL)
		return;
	CSuperSmartPtr<CLDSPart> pPlate=pPlateDraw->GetPlate();
	if(pPlate.IsNULL())
		return;
	//初始化钢板信息
	int thick=ftoi(pPlate->GetThick());
	int wide=ftoi(pPlate->GetWidth());
	int length=ftoi(pPlate->GetLength());
	int nEdgeNum=pPlate.GeneralPlatePointer()->GetEdgeNum(1);
	double fWeldLength=pPlate.GeneralPlatePointer()->CalWeldEdgeLength();
	double area=0;
	if(pPlate->GetClassTypeId()==CLS_PLATE)
	{
		if(CLDSPlate::m_iPlankWeightStatType==2)//直接取记录的值避免重复计算
			area=pPlate.PlatePointer()->CalAreaOfNetWrap();
		else
			area=pPlate.PlatePointer()->CalAreaOfMinWrapRect();
	}
	else
		area = pPlate->GetWidth()*pPlate->GetLength();
	CXhChar50 sPartNo,guige,sNotes,sTemp,sMat;
	strcpy(sPartNo,GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial));
	QuerySteelMatMark(pPlate->cMaterial,sMat);
	int face_N=1;
	if(pPlate->GetClassTypeId()==CLS_PLATE)
		face_N=pPlate.PlatePointer()->face_N;
	else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_UEND)
			face_N=2;
		else if(pPlate.ParamPlatePointer()->m_iParamType==TYPE_ROLLEND)
			face_N=3;
		else
			face_N=1;
	}
	//对标注样式风格进行设置，统一对标注字体高度进行设置
	double dim_txt_size=sys.fonts.plate.fDimTextSize;		//长度尺寸标注文本高
	double partno_txt_size=sys.fonts.plate.fPartNoTextSize;	//构件编号文字高
	double dim_angle_size=sys.fonts.plate.fDimAngleTextSize;//角度尺寸标注文本高
	if(sys.part_map.plate.bZoomDimTextSizeByWidth)
	{	//默认宽度与字高比例为3:100 即宽度为100时 字高为3	wht 10-02-23
		double plate_width=min(wide,length)*zoomCoef;
		dim_txt_size=fto_halfi(plate_width*sys.part_map.plate.fTextSizeZoomCoef+0.5);		//字体高度
		partno_txt_size=fto_halfi(plate_width*sys.part_map.plate.fTextSizeZoomCoef+0.5);	//字体高度
		dim_angle_size=fto_halfi(plate_width*sys.part_map.plate.fTextSizeZoomCoef+0.5);		//字体高度
	}
	DimStyleTable::dimStyle.InitDimStyle(dim_txt_size);			//尺寸标注样式
	DimStyleTable::dimStyle3.InitDimStyle(dim_txt_size);		//尺寸标注样式（文字在尺寸线外）
	DimStyleTable::dimRadiusStyle.InitDimStyle(dim_txt_size);	//半径|直径标注样式
	DimStyleTable::dimAngleStyle.InitDimStyle(dim_angle_size);	//角度标注样式
	//绘制火曲特征线
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(sys.part_map.plate.bDrawHuoQuMarkLine)
	{
		f3dLine line;
		for(int m=0;m<2;m++)
		{
			if(pPlateDraw->huoqu_mark[m].origin.IsZero())
				break;
			pPlateDraw->huoqu_mark[m].origin*=zoomCoef;
			pPlateDraw->huoqu_mark[m].origin+=origin;
			f3dPoint offset_vec=pPlateDraw->huoqu_mark[m].offset_dist*pPlateDraw->huoqu_mark[m].prj_vec;
			for(int j1=0;j1<5;j1++)
			{
				line.startPt=pPlateDraw->huoqu_mark[m].line_arr[j1].startPt+pPlateDraw->huoqu_mark[m].origin+offset_vec;
				line.endPt=pPlateDraw->huoqu_mark[m].line_arr[j1].endPt+pPlateDraw->huoqu_mark[m].origin+offset_vec;
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
			}
		}
	}
	//绘制轮廓边
	for(int i=0;i<pPlateDraw->GetEdgeCount();i++)
	{
		f3dArcLine arcline;
		pPlateDraw->GetEdgeAt(i,arcline);
		arcline.ZoomScale(zoomCoef);
		trans_pos(arcline,origin);
		if(arcline.pen.style==PS_SOLID)
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else if(arcline.pen.style==PS_DASH)
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		else if(arcline.pen.style==PS_DASHDOT)
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		else if(arcline.pen.style==PS_DAMAGELINE)
			GetCurDwg()->setClayer(LayerTable::DamagedSymbolLine.layerId);
		else if(arcline.pen.style==PS_TUBEBREAK)	//钢管破断线
		{
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
			double t=arcline.feature;
			t=fabs(t)*zoomCoef;
			NewTubeBreakFaceLine(pBlockTableRecord,arcline.Start(),arcline.End(),t);
			continue;
		}
		else if(arcline.pen.style==PS_WELDLINE)
		{
			if(arcline.SectorAngle()==0)
				DrawWeldLineSymbol(pBlockTableRecord,arcline.Start(),arcline.End(),sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
			else
				DrawWeldLineSymbol(pBlockTableRecord,arcline,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		}
		else if(arcline.pen.style==PS_OTHERLINE)
			GetCurDwg()->setClayer(LayerTable::CommonLayer.layerId);
		else //if(arcline.feature==5||arcline.pen.style==PS_DASHDOTDOT)
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
		if(arcline.feature<0)
			arcline.feature=0;
		if(arcline.SectorAngle()==0)
			CreateAcadLine(pBlockTableRecord,arcline.Start(),arcline.End(),arcline.feature);
		else if(arcline.WorkNorm()==arcline.ColumnNorm())
		{
			if(fabs(arcline.SectorAngle())>=2*Pi-EPS)
				CreateAcadCircle(pBlockTableRecord,arcline.Center(),arcline.Radius(),arcline.feature);
			else
				CreateAcadArcLine(pBlockTableRecord,arcline.Center(),arcline.Start(),arcline.SectorAngle(),arcline.WorkNorm(),arcline.feature);
		}
		else
			CreateAcadEllipseLine(pBlockTableRecord,arcline);
	}
	//绘制火曲标记线（样条曲线）
	for(IDbEntity* pEntity=pPlateDraw->EnumFirstDbEntity();pEntity;pEntity=pPlateDraw->EnumNextDbEntity())
	{
		if(pEntity->GetPen().style==PS_INSIDEFRAME)
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		else
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(pEntity->GetDbEntType()==IDbEntity::DbLine)
		{
			IDbLine* pLine=(IDbLine*)pEntity;
			pLine->ZoomScale(zoomCoef);
			pLine->Offset(origin);
			CreateAcadLine(pBlockTableRecord,pLine->StartPosition(),pLine->EndPosition());
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbSpline)
		{
			IDbSpline* pSpline=(IDbSpline*)pEntity;
			pSpline->ZoomScale(zoomCoef);
			pSpline->Offset(origin);
			GEPOINT fitPt;
			AcGePoint3dArray fitPoints;
			int nNum=pSpline->GetNumFitPoints();
			for(int i=0;i<4;i++)
			{
				pSpline->GetFitPointAt(i,fitPt);
				fitPoints.append(AcGePoint3d(fitPt.x,fitPt.y,fitPt.z));
			}
			CreateAcadSpline(pBlockTableRecord,fitPoints);
		}
	}
	//绘制螺栓
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	CXhChar16 sM12("M12"),sM16("M16"),sM18("M18"),sM20("M20"),sM22("M22"),sM24("M24");
	if(sys.part_map.plate.bPlateUseSimpleLsMap)
	{
		sM12.Copy("M12_Jg");
		sM16.Copy("M16_Jg");
		sM18.Copy("M18_Jg");
		sM20.Copy("M20_Jg");
		sM22.Copy("M22_Jg");
		sM24.Copy("M24_Jg");
	}
	int n=pPlateDraw->GetLsCirCount();
	for(int m=0;m<n;m++)
	{
		f3dCircle Cir;
		pPlateDraw->GetLsCirAt(m,Cir);
		Cir.centre*=zoomCoef;
		Cir.centre+=origin;
		BOOL bRet=FALSE;
		//得到螺栓孔腰圆长度 画笔默认宽度为1 wht 10-02-25
		int waist_len=Cir.pen.width;
		if(waist_len>1)	
		{
			f3dPoint waist_vec=Cir.norm;	//腰圆方向
			Cir.radius*=zoomCoef;
			if(NewWaistHole(pBlockTableRecord,Cir,waist_vec,waist_len*zoomCoef))
			{	//标注螺栓的腰圆长度  wxc-2017.3.7
				CXhChar16 sText("%d",waist_len);
				if(waist_vec.x<0)	//保证腰圆孔方向与X轴方向相同
					waist_vec*=-1.0;
				double half_len=0.5*waist_len*zoomCoef;
				f3dPoint dimStart=Cir.centre-waist_vec*half_len;
				f3dPoint dimEnd=Cir.centre+waist_vec*half_len;
				f3dPoint dimPos=(dimStart+dimEnd)*0.5;
				double angle=Cal2dLineAng(0,0,waist_vec.x,waist_vec.y);
				DimAngleSize(pBlockTableRecord,dimStart,dimEnd,dimPos,sText,DimStyleTable::dimStyle.dimStyleId,angle,dim_txt_size);//sys.fonts.plate.fDimTextSize);
				continue;
			}
		}
		if(sys.general_set.PartMapScale.iBoltSymbolStyle==1)	//图符处理
		{
			if(Cir.pen.style>1)
			{	//特殊功用螺栓孔----根据刘伟要求，凡是特殊功用螺栓孔直接绘制圆，并标注孔径	wxc-2017.8.25
				bRet=FALSE;
			}
			else
			{	//连接螺栓
				if(fabs(2*Cir.radius-12)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM12,1.0,0);
				else if(fabs(2*Cir.radius-16)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM16,1.0,0);
				else if(fabs(2*Cir.radius-18)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM18,1.0,0);
				else if(fabs(2*Cir.radius-20)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM20,1.0,0);
				else if(fabs(2*Cir.radius-22)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM22,1.0,0);
				else if(fabs(2*Cir.radius-24)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM24,1.0,0);
				else
				{
					CXhChar16 ls_str;
					ls_str.Printf("M%d",ftol(2*Cir.radius));
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,1.0,0);
					if(!bRet)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
				}
			}
		}
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==2)	//比例图符处理
		{
			double fZoomScale=zoomCoef*sys.general_set.PartMapScale.fBoltSymbolFactor;
			if(Cir.pen.style>1)
			{	//特殊功用螺栓孔----根据刘伟要求，凡是特殊功用螺栓孔直接绘制圆，并标注孔径	wxc-2017.8.25
				bRet=FALSE;
			}
			else
			{	//连接螺栓
				if(fabs(2*Cir.radius-12)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM12,fZoomScale,0);
				else if(fabs(2*Cir.radius-16)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM16,fZoomScale,0);
				else if(fabs(2*Cir.radius-18)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM18,fZoomScale,0);
				else if(fabs(2*Cir.radius-20)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM20,fZoomScale,0);
				else if(fabs(2*Cir.radius-22)<EPS)																					   
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM22,fZoomScale,0);
				else if(fabs(2*Cir.radius-24)<EPS)																					   
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM24,fZoomScale,0);
				else
				{
					CXhChar16 ls_str;
					ls_str.Printf("M%d",ftol(2*Cir.radius));
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,fZoomScale,0);
					if(!bRet)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
				}
			}
		}
		else if(sys.general_set.PartMapScale.iBoltSymbolStyle==3)	//比例直径图符处理
		{
			double fZoomScale=(Cir.radius*2+Cir.feature/10.0+sys.general_set.fHoleAugmentInNC)*zoomCoef*sys.general_set.PartMapScale.fBoltDiaFactor;
			if(Cir.pen.style>1)
			{	//特殊功用螺栓孔----根据刘伟要求，凡是特殊功用螺栓孔直接绘制圆，并标注孔径	wxc-2017.8.25
				bRet=FALSE;
			}
			else
			{
				if(fabs(2*Cir.radius-12)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM12,fZoomScale,0);
				else if(fabs(2*Cir.radius-16)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM16,fZoomScale,0);
				else if(fabs(2*Cir.radius-18)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM18,fZoomScale,0);
				else if(fabs(2*Cir.radius-20)<EPS)
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM20,fZoomScale,0);
				else if(fabs(2*Cir.radius-22)<EPS)																					   
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM22,fZoomScale,0);
				else if(fabs(2*Cir.radius-24)<EPS)																					   
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,sM24,fZoomScale,0);
				else
				{
					CXhChar16 ls_str;
					ls_str.Printf("M%d",ftol(2*Cir.radius));
					bRet=InsertBlock(pBlockTableRecord,Cir.centre,ls_str,fZoomScale,0);
					if(!bRet)
						bRet=InsertBlock(pBlockTableRecord,Cir.centre,"MX",1.0,0);
				}	
			}
		}
		if(sys.general_set.PartMapScale.iBoltSymbolStyle==0||!bRet)	//圆孔处理
		{
			double real_radius=Cir.radius+Cir.feature/20.0+sys.general_set.fHoleAugmentInNC/2.0;
			Cir.radius=real_radius*zoomCoef;
			NewBoltCircle(pBlockTableRecord,Cir);
			if(sys.general_set.PartMapScale.iBoltSymbolStyle!=0)
			{	//标注螺栓图符时，对于特殊孔应标出孔直径，以免错打
				CXhChar16 sText;
				sText.Printf("%%%%c%f",real_radius*2);
				SimplifiedNumString(sText);
				if(Cir.centre.pen.style==CLsRef::DRILL_HOLE)	//钻孔
					sText.Append("钻");
				f3dPoint vec(Cir.radius*0.707,Cir.radius*0.707);
				f3dPoint chordPos(Cir.centre.x-vec.x,Cir.centre.y-vec.y),farChordPos(Cir.centre.x+vec.x,Cir.centre.y+vec.y);
				double leaderLength=2;//*zoomCoef;
				DimDiameter(pBlockTableRecord,chordPos,farChordPos,leaderLength,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
			}
		}
	}
	//绘制尺寸标注
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	if(sys.general_set.iDimPrecision==0)		//精确到1mm
		ROTATEDIM_STRU::PRECISION=1.0;
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
		ROTATEDIM_STRU::PRECISION=0.5;
	else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
		ROTATEDIM_STRU::PRECISION=0.1;
	//线性标注
	n = pPlateDraw->GetRotateDimCount();
	for(int m=0;m<n;m++)
	{
		ROTATEDIM_STRU rotate_dim;
		pPlateDraw->GetRotateDimAt(m,rotate_dim);
		rotate_dim.startPos*=zoomCoef;
		rotate_dim.endPos*=zoomCoef;
		rotate_dim.dimPos*=zoomCoef;
		rotate_dim.startPos+=origin;
		rotate_dim.endPos+=origin;
		rotate_dim.dimPos+=origin;
		if(rotate_dim.bDisplayDimLine)
		{
			AcDbObjectId dimStyleId=DimStyleTable::dimStyle.dimStyleId;
			if(!rotate_dim.bCanTextOutter)	//dimStyle3专门用于标注文字可以在外部进行标注的情况 wht 11-07-12
				dimStyleId=DimStyleTable::dimStyle3.dimStyleId;
			DimAngleSize(pBlockTableRecord,rotate_dim.startPos,rotate_dim.endPos,
				rotate_dim.dimPos,rotate_dim.DimText(),dimStyleId,rotate_dim.angle,dim_txt_size);
		}
		else
		{
			if(rotate_dim.angle>=1.5*Pi||rotate_dim.angle<Pi/2.0)
				rotate_dim.dimPos=GetPtInPolar(rotate_dim.dimPos,rotate_dim.angle-Pi/2,1.5);
			else
				rotate_dim.dimPos=GetPtInPolar(rotate_dim.dimPos,rotate_dim.angle+Pi/2,1.5);
			if(rotate_dim.angle>=1.5*Pi||rotate_dim.angle<Pi/2.0)
				DimText(pBlockTableRecord,rotate_dim.dimPos,rotate_dim.DimText(),TextStyleTable::hzfs.textStyleId,
				dim_txt_size,rotate_dim.angle,AcDb::kTextCenter,AcDb::kTextTop);//sys.dim_map.fDimTextSize*zoomCoef*3
			else
				DimText(pBlockTableRecord,rotate_dim.dimPos,rotate_dim.DimText(),TextStyleTable::hzfs.textStyleId,
				dim_txt_size,rotate_dim.angle+Pi,AcDb::kTextCenter,AcDb::kTextTop);//sys.dim_map.fDimTextSize*zoomCoef*3
		}
	}
	//角度标注
	n = pPlateDraw->GetAngularDimCount();
	for(int m=0;m<n;m++)
	{
		ANGULARDIM_STRU angular_dim;
		pPlateDraw->GetAngularDimAt(m,angular_dim);
		angular_dim.vertex*=zoomCoef;
		angular_dim.startPos*=zoomCoef;
		angular_dim.endPos*=zoomCoef;
		angular_dim.dimPos*=zoomCoef;
		angular_dim.vertex+=origin;
		angular_dim.startPos+=origin;
		angular_dim.endPos+=origin;
		angular_dim.dimPos+=origin;
		DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,
			angular_dim.vertex,angular_dim.startPos,
			angular_dim.endPos,angular_dim.dimPos,angular_dim.sDimText);
	}
	//直径标注
	n = pPlateDraw->GetDiameterDimCount();
	for(int m=0;m<n;m++)
	{
		DIAMETERDIM_STRU diam_dim;
		pPlateDraw->GetDiameterDimAt(m,diam_dim);
		diam_dim.chordPos*=zoomCoef;
		diam_dim.farChordPos*=zoomCoef;
		diam_dim.chordPos+=origin;
		diam_dim.farChordPos+=origin;
		CXhChar50 sText;
		//使用半径标注样式标注半径或直径 wht 11-07-12
		if(diam_dim.bDimRadial)
		{	//
			sText.Printf("R%f",diam_dim.dist);
			SimplifiedNumString(sText);
			DimRadial(pBlockTableRecord,diam_dim.farChordPos,diam_dim.chordPos,
				diam_dim.leaderLength,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
		}
		else
		{	//
			sText.Printf("%%%%c%f",diam_dim.dist);
			SimplifiedNumString(sText);
			DimDiameter(pBlockTableRecord,diam_dim.chordPos,diam_dim.farChordPos,
				diam_dim.leaderLength,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
		}
	}
	//文本标注
	GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
	n = pPlateDraw->GetTextDimCount();
	for(int m=0;m<n;m++)
	{
		TXTDIM_STRU txt_dim;
		pPlateDraw->GetTextDimAt(m,txt_dim);
		txt_dim.dimPos*=zoomCoef;
		txt_dim.dimPos+=origin;
		double rotAngle=Cal2dLineAng(0,0,txt_dim.dimVec.x,txt_dim.dimVec.y);
		bool reversed=false;
		if(rotAngle>Pi/2&&rotAngle<3*Pi/2)
		{
			rotAngle-=Pi;
			reversed=true;
			if(txt_dim.vertAlignMode==AcDb::kTextTop)
				txt_dim.vertAlignMode=AcDb::kTextBottom;
			else if(txt_dim.vertAlignMode==AcDb::kTextBottom)
				txt_dim.vertAlignMode=AcDb::kTextTop;
			if(txt_dim.horzAlignMode==AcDb::kTextLeft)
				txt_dim.horzAlignMode=AcDb::kTextRight;
			else if(txt_dim.horzAlignMode==AcDb::kTextRight)
				txt_dim.horzAlignMode=AcDb::kTextLeft;
		}
		f3dPoint norm(cos(rotAngle+Pi/2),sin(rotAngle+Pi/2));
		if(txt_dim.vertAlignMode==AcDb::kTextTop)
			txt_dim.dimPos-=norm*1.0;//*zoomCoef;
		else if(txt_dim.vertAlignMode==AcDb::kTextBottom)
			txt_dim.dimPos+=norm*1.0;//*zoomCoef;
		if(txt_dim.txtHeight==0)
			txt_dim.txtHeight=partno_txt_size;//*zoomCoef;
		DimText(pBlockTableRecord,txt_dim.dimPos,txt_dim.dimText,TextStyleTable::hzfs.textStyleId,
			txt_dim.txtHeight,rotAngle,txt_dim.horzAlignMode,txt_dim.vertAlignMode);
	}
	//标注螺栓射线
	if(sys.part_map.plate.bDimPlateRayLs)
	{
		n = pPlateDraw->GetLsRayCount();
		for(int m=0;m<n;m++)
		{
			f3dLine line;
			if(pPlateDraw->GetLsRayLineAt(m,line))
			{
				line.startPt*=zoomCoef;
				line.endPt*=zoomCoef;
				trans_pos(line,origin);
				if(line.pen.style==PS_SOLID)
					GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
				else 
					GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
			}
		}
	}
	//
	GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
	strcpy(sNotes,"");
	char *token;
	CXhChar200 key_str;
	CStringArray str_arr,sNoteArr;
	key_str.Copy(export_info);
	for(token=strtok(key_str,"\n");token;token=strtok(NULL,"\n"))
		str_arr.Add(token);
	for(int i=0;i<str_arr.GetSize();i++)
	{
		sNotes.Empty();
		key_str.Copy(str_arr[i]);
		for(token=strtok(key_str,"&");token;token=strtok(NULL,"&"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(stricmp(token,"Project Name")==0)
#else
			if(stricmp(token,"工程名称")==0)
#endif
			{
				sTemp.Printf("%s ",Ta.m_sPrjName);
				sNotes.Append(sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Tower Type Code")==0)
#else
			else if(stricmp(token,"塔型代号")==0)
#endif
			{
				sTemp.Printf("%s ",Ta.m_sTaAlias);
				sNotes.Append(sTemp); 
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Material")==0)
#else
			else if(stricmp(token,"材质")==0)
#endif
			{
				QuerySteelMatMark(pPlate->cMaterial,sTemp);
				sTemp.Append(" ");
				sNotes.Append(sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Part No.")==0)
#else
			else if(stricmp(token,"件号")==0)
#endif
			{
				if(gxDrwLocalizer.ciCustomerSerial==11)
				{	//长春聚德龙要求件号后添加火区信息
					if(pPlateDraw->m_ciHuoQuType==1)		//反曲
						sTemp.Printf("%s#反曲 ",(char*)sPartNo);
					else if(pPlateDraw->m_ciHuoQuType==2)	//正曲
						sTemp.Printf("%s#正曲 ",(char*)sPartNo);
					else
						sTemp.Printf("%s#",(char*)sPartNo);
				}
				else
					sTemp.Printf("%s#",(char*)sPartNo);
				sNotes.Append(sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Number Of Parts")==0)
			{
				sTemp.Printf("%d piece",pPlate->feature);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"Thickness")==0)
#else
			else if(stricmp(token,"件数")==0)
			{
				sTemp.Printf("%d件",pPlate->feature);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"厚度")==0)
#endif
			{
				sTemp.Printf("-%d ",thick);
				sNotes.Append(sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Area")==0)
			{
				sTemp.Printf("area:%.0f(mm2) ",area);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"Piece Weight")==0)
			{
				sTemp.Printf("piece weight:%.2f(kg) ",area*thick*7.85e-6);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"Total Weight")==0)
			{
				sTemp.Printf("total weight:%.2f(kg) ",pPlate->feature*area*thick*7.85e-6);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"Tower Type")==0)
#else
			else if(stricmp(token,"面积")==0)
			{
				sTemp.Printf("面积:%.0f(mm2) ",area);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"单重")==0)
			{
				sTemp.Printf("单重:%.2f(kg) ",area*thick*7.85e-6);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"总重")==0)
			{
				sTemp.Printf("总重:%.2f(kg) ",pPlate->feature*area*thick*7.85e-6);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"塔型")==0)
#endif
			{
				sTemp.Printf("%s ",(char*)Ta.m_sTaType);
				sNotes.Append(sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Spec")==0)
#else
			else if(stricmp(token,"规格")==0)
#endif
			{
				sTemp.Printf("%d*%d(mm) ",wide,length);
				sNotes.Append(sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Stamp No.")==0)
#else
			else if(stricmp(token,"钢印号")==0)
#endif
			{
				sTemp.Printf("%s ",Ta.m_sTaStampNo);
				sNotes.Append(sTemp);
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(stricmp(token,"Number")==0)
			{	//cardinal number 
				sTemp.Printf("%d foundation ",nTaNum);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"Number Of Bolts")==0)
#else
			else if(stricmp(token,"基数")==0)
			{	//基数
				sTemp.Printf("%d基 ",nTaNum);
				sNotes.Append(sTemp);
			}
			else if(stricmp(token,"螺栓数")==0)
#endif
			{
				//孔径X孔数
				sTemp.Empty();
				CHashList<BOLT_ITEM> hashList;
				hashList.Empty();
				CLsRefList* pLsRefList=pPlate->GetLsRefList();
				for(CLsRef* pLsRef=pLsRefList->GetFirst();pLsRef!=NULL;pLsRef=pLsRefList->GetNext())
				{
					double fDiameter=(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment;
					BOLT_ITEM* pBoltItem=hashList.GetValue(ftoi(fDiameter*10));
					if(pBoltItem==NULL)
					{
						pBoltItem=hashList.Add((int)fDiameter*10);
						pBoltItem->fDiameter=fDiameter;	//实际孔径
					}
					pBoltItem->nNum++;										
				}
				for(BOLT_ITEM* pBoltItem=hashList.GetFirst();pBoltItem;pBoltItem=hashList.GetNext())
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sTemp.Printf("%%%%c%gX%d ",pBoltItem->fDiameter,pBoltItem->nNum);
#else
					sTemp.Printf("%%%%c%gX%d ",pBoltItem->fDiameter,pBoltItem->nNum);
#endif
					sNotes.Append(sTemp);
				}
			}
			else if(stricmp(token,"边数")==0)
			{
				sTemp.Printf("边数:%d ",nEdgeNum);
				sNotes.Append(sTemp);
			}
			else if (stricmp(token,"焊缝边长")==0)
			{
				sTemp.Printf("焊缝边长：%.0f",fWeldLength);
				sNotes.Append(sTemp);
			}
		}
		sNoteArr.Add(sNotes);
	}
	//
	TXTDIM_STRU txt_dim;
	if(sys.part_map.plate.bDimCoord)	//标注相对坐标时，构件明细标注在几何中心
		txt_dim.dimPos=pPlateDraw->Get2dGeomCenter();
	else
		txt_dim.dimPos=pPlateDraw->long_edge_mid;
	txt_dim.dimPos*=zoomCoef;
	txt_dim.dimPos+=origin;
	double angle=0,fTextH=0,fTextW=0;
	angle=pPlateDraw->long_edge_angle;
	if(pPlateDraw->long_edge_angle>Pi/2&&pPlateDraw->long_edge_angle<Pi)
		angle+=Pi;
	else if(angle>=Pi&&angle<1.5*Pi)
		angle-=Pi;
	for(int i=0;i<sNoteArr.GetSize();i++)
	{
		sNotes.Copy(sNoteArr[i]);
		double len=TestDrawTextLength(sNotes,partno_txt_size,TextStyleTable::hzfs.textStyleId);
		fTextW=max(fTextW,len);
		fTextH+=partno_txt_size*1.4;
	}
	f3dPoint long_edge_norm(-sin(pPlateDraw->long_edge_angle),cos(pPlateDraw->long_edge_angle));
	f3dPoint dim_pos=GetPtInPolar(txt_dim.dimPos,angle,-fTextW*0.5);
	int BTM_MARGIN=partno_txt_size>5?(int)partno_txt_size:5;
	dim_pos+=long_edge_norm*BTM_MARGIN;	//文字标注到底边距离最小为5
	//初始化标注文字的的矩形区域，并搜索合适的标注位置
	BOOL bSearchPos=FALSE;
	f3dPoint base_pos=dim_pos;
	f3dPoint rgn_vert[4];
	f3dPoint verify_norm(-sin(angle),cos(angle));
	if(verify_norm.x*long_edge_norm.x+verify_norm.y*long_edge_norm.y>0)
	{	//文字走向逆时针转90°方向与long_edge_norm一至
		rgn_vert[0]=base_pos;
		rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,fTextW);
		rgn_vert[2]=rgn_vert[1]+long_edge_norm*fTextH;
		rgn_vert[3]=rgn_vert[0]+long_edge_norm*fTextH;
	}
	else
	{	//文字走向逆时针转90°方向与long_edge_norm相反
		rgn_vert[0]=base_pos-long_edge_norm*partno_txt_size;
		rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,fTextW);
		rgn_vert[2]=rgn_vert[1]+long_edge_norm*fTextH;
		rgn_vert[3]=rgn_vert[0]+long_edge_norm*fTextH;
	}
	int quad_order[4]={1,2,0,0};
	CSearchBlankPosition search;
	search.SetBasePointAddr(base_pos);
	GEPOINT vHoriSearchVec(long_edge_norm.y,-long_edge_norm.x);
	//if(SearchNoneIntersPos(&base_pos,rgn_vert,NULL,quad_order,40,2)==0)
	if( search.SearchNoneIntersPos(rgn_vert,vHoriSearchVec,15,40,0.2,0.8,0x07)||		//0x07=DIRECT_X|DIRECT_X_INVERSE|DIRECT_Y
		search.SearchNoneIntersPos(rgn_vert,vHoriSearchVec,15,BTM_MARGIN-1,0.2,0.8,0x0b))//0x0b=DIRECT_X|DIRECT_X_INVERSE|DIRECT_Y_INVERSE
	{
		f3dPoint restore(base_pos);
		restore-=origin;
		restore/=zoomCoef;
		bSearchPos=pPlateDraw->PtInPlateRgn(restore);
	}
	if(bSearchPos)
		dim_pos=base_pos;
	//福建坚石反映，由于板线识别问题需要将钢板轮廓边与标注线分开图层 wjh-2014.9.17
	//DRAGSET.BeginBlockRecord();
	GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
	double fLineHeight=fTextH/sNoteArr.GetSize(),fDist=0;
	n=sNoteArr.GetSize();
	for(int i=0;i<n;i++)
	{
		AcDb::TextVertMode vertMode=AcDb::kTextBottom;
		if(pPlateDraw->long_edge_angle>Pi/2&&pPlateDraw->long_edge_angle<1.5*Pi)
		{
			fDist=fLineHeight*i;
			vertMode=AcDb::kTextTop;
		}
		else
			fDist=fLineHeight*(n-1-i);
		f3dPoint dimPos=dim_pos+long_edge_norm*fDist;
		sNotes.Copy(sNoteArr[i]);
		DimText(pBlockTableRecord,dimPos,sNotes,TextStyleTable::hzfs.textStyleId,
			partno_txt_size,angle,AcDb::kTextLeft,vertMode);//AcDb::kTextBottom);
		//f3dPoint dimPos=long_edge_norm*fDist;
		//sNotes.Copy(sNoteArr[i]);
		//DimText(DRAGSET.RecordingBlockTableRecord(),dimPos,sNotes,TextStyleTable::hzfs.textStyleId,
		//	partno_txt_size,angle,AcDb::kTextLeft,vertMode);//AcDb::kTextBottom);
	}
	//DRAGSET.EndBlockRecord(pBlockTableRecord,dim_pos);
	//电焊标记,与标注字体保持一致高度
	if(CPlankPartParaExportDlg::m_bMartWeld && IdentifyWeldPlate(pPlate.GeneralPlatePointer()))
	{
		double fScale=partno_txt_size/5;
		f3dPoint dimPos=GetPtInPolar(dim_pos,angle,-10*fScale);
		dimPos+=long_edge_norm*partno_txt_size*0.5;
		InsertBlock(pBlockTableRecord,dimPos,"weld",fScale,angle);
	}
}

#ifdef __PNC_FUNC__
#include "PlateProcess.h"
BOOL GetCurWorkPath(CString& file_path,BOOL bEscapeChar=TRUE);
void LayoutPlates()
{
	CLogErrorLife logErrLife;
	CAcModuleResourceOverride resOverride;
	//0.生成PPI文件，目前只有调用生成PPI文件才能初始化钢板坐标系
	CString file_path;
	GetCurWorkPath(file_path);
	for(CPlateProcessInfo* pPlateProcess=model.EnumFirstPlate();pPlateProcess;pPlateProcess=model.EnumNextPlate())
	{	//生成PPI文件,保存到到当前工作路径下
		if(pPlateProcess->vertexList.GetNodeNum()>3)
			pPlateProcess->CreatePPiFile(file_path);
	}
	//1.钢板布局
	CArrangeAndDrawPlateSetDlg setDlg;
	if(setDlg.DoModal()!=IDOK)
		return;
	double hight = setDlg.m_nPaperW;
	double minDistance = setDlg.m_nMinDistance;
	model.LayoutPlates(hight,minDistance);
}
void ExtractAndLayoutPlates()
{
	CLogErrorLife logErrLife;
	InitDrawingEnv();
	CAcModuleResourceOverride resOverride;
	model.m_hashPlateInfo.Empty();
	if(model.ExtractPlateInfo())
		LayoutPlates();
}
#endif

#else
void PartDrawingOneByOne(){;}
void BatchPartDrawing(){;}
void PartDrawingClassicAngle(){;}
void SetupAidedPlate(){;}
void SpecialProcessStatCard(){;}
void StdPartGroupProcessCard(){;}
void StdPartDrawingByReadLibFile(){;}
#endif