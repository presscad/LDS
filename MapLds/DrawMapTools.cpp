//<LOCALE_TRANSLATE/>
//DrawMapTools.cpp
//对应工具菜单下的命令

#include "stdafx.h"
#include "stdarx.h"
#include "Tower.h"
#include "MenuFunc.h"
#include "RxTools.h"
#include "SysPara.h"
#include "DragEntSet.h"
#include "CalTwoTubeAngleDlg.h"
#include "DecarlToPolarDlg.h"
#include "BreakLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//计算钢管之间的夹角
void CalTwoTubeAngle()
#ifndef __TSA_FILE_
{
    CAcModuleResourceOverride resOverride;
	CCalTwoTubeAngleDlg cal_angle_dlg;
	CLDSLineTube *pStartTube=NULL, *pEndTube=NULL;
	//通过选择实体获取实体的ID
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	struct resbuf *filter;
	AcDbLine *pStartLine=NULL, *pEndLine=NULL;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);
	filter = acutBuildList(RTDXF0,L"POINT,LINE",0);
#else
	acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);
	filter = acutBuildList(RTDXF0,"POINT,LINE",0);
#endif
	if(acedSSGet(NULL,NULL,NULL,NULL,ent_sel_set)==RTNORM)
	{
		long i,ll;
		acedSSLength(ent_sel_set,&ll);
		for(i=0;i<ll;i++)
		{
			AcDbEntity *pEnt;
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			acdbOpenObject(pEnt,entId,AcDb::kForRead);
			CAcDbObjLife entLife(pEnt);
			long handle=GetTaAtomHandle(pEnt);
			CLDSPart *pPart=Ta.FromPartHandle(handle);
			if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)
			{
				CSuperSmartPtr<CLDSPart> pLinePart=pPart;
				if(pLinePart.IsNULL()||!pLinePart->IsLinePart())
					continue;	//找不到或找到的不是线型构件
				if(pEnt->isKindOf(AcDbLine::desc()))
				{
					if(pStartTube==NULL)
					{
						pStartLine=(AcDbLine*)pEnt;
						pStartTube=pLinePart.LineTubePointer();
						continue;
					}
					else if(pStartTube&&pEndTube==NULL)
					{
						pEndLine=(AcDbLine*)pEnt;
						pEndTube=pLinePart.LineTubePointer();
						continue;
					}
					else if(pStartTube&&pEndTube)
						break;	//找到两根钢管
				}
				else
					continue;
			}
			pEnt->close();
		}
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"1",RTNONE);
#else
	acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);
#endif
	cal_angle_dlg.m_pStartTube=pStartTube;
	cal_angle_dlg.m_pEndTube=pEndTube;
	if(cal_angle_dlg.DoModal()!=IDOK)
		return;
	//标注钢管夹角
	if(cal_angle_dlg.m_bDimTubeAngle&&cal_angle_dlg.m_bCanDimAngle&&pStartLine&&pEndLine)
	{	
		//获得当前图形的块表指针
		AcDbBlockTable *pBlockTable;
		GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
		pBlockTable->close();//关闭块表
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		char sDimText[100]="";
		DimStyleTable::dimAngleStyle.InitDimStyle(sys.dim_map.fDimAngleTextSize);
		f3dPoint startPt,endPt,dimStart,dimEnd,dimPos,vertex;
		//求两直线的交点
		AcGePoint3d acad_pnt;
		acad_pnt=pStartLine->startPoint();
		Cpy_Pnt(startPt,acad_pnt);
		acad_pnt=pStartLine->endPoint();
		Cpy_Pnt(endPt,acad_pnt);
		acad_pnt=pEndLine->startPoint();
		Cpy_Pnt(dimStart,acad_pnt);
		acad_pnt=pEndLine->endPoint();
		Cpy_Pnt(dimEnd,acad_pnt);
		int nRet=Int3dll(startPt,endPt,dimStart,dimEnd,vertex);
		if(nRet==-1||nRet==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			acedPrompt(L"There is not intersection point of two straight line\n");
#else
			acedPrompt("There is not intersection point of two straight line\n");
#endif
#else
#ifdef _ARX_2007
			acedPrompt(L"两直线无交点\n");
#else
			acedPrompt("两直线无交点\n");
#endif
#endif
			return;
		}
		f3dPoint start_line_vec,end_line_vec;
		f3dLine start_line(startPt,endPt);
		f3dLine end_line(dimStart,dimEnd);
		start_line_vec=endPt-startPt;
		normalize(start_line_vec);
		end_line_vec=dimEnd-dimStart;
		normalize(end_line_vec);
		dimStart.Set();
		dimEnd.Set();
		double angle=cal_angle_of_2vec(start_line_vec,end_line_vec)*DEGTORAD_COEF;
		if((angle>90&&cal_angle_dlg.m_fTubeAngle<90)||(angle<90&&cal_angle_dlg.m_fTubeAngle>90))
			start_line_vec*=-1.0;	//调整角度标注线位置
		dimStart=vertex+start_line_vec*cal_angle_dlg.m_fDimRadius;
		dimEnd=vertex+end_line_vec*cal_angle_dlg.m_fDimRadius;
		if(start_line.PtInLine(dimStart)<1||end_line.PtInLine(dimEnd)<1)
		{	//调整角度标注线位置
			dimStart=vertex-start_line_vec*cal_angle_dlg.m_fDimRadius;
			dimEnd=vertex-end_line_vec*cal_angle_dlg.m_fDimRadius;
		}
		sprintf(sDimText,"%.1f",cal_angle_dlg.m_fTubeAngle);
		dimPos=(dimStart+dimEnd)/2.0;
		DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,vertex,dimStart,dimEnd,dimPos,sDimText);
		pBlockTableRecord->close();
	}
}
#else
{;}
#endif

//截断指定直线
void BreakSpecLine()
{
	f3dLine inter_line;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();//定义块表记录指针
	ads_point ads_line_start,ads_line_end;
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	acedGetPoint(NULL, L"Please input the start point of truncate line\\t",ads_line_start);
	acedGetPoint(ads_line_start, L"Please input the end point of truncate line\\t",ads_line_end);
#else
	acedGetPoint(NULL, "Please input the start point of truncate line\\t",ads_line_start);
	acedGetPoint(ads_line_start, "Please input the end point of truncate line\\t",ads_line_end);
#endif
#else
#ifdef _ARX_2007
	acedGetPoint(NULL, L"请输入截断线起始点\\t",ads_line_start);
	acedGetPoint(ads_line_start, L"请输入截断线起终点\\t",ads_line_end);
#else
	acedGetPoint(NULL, "请输入截断线起始点\\t",ads_line_start);
	acedGetPoint(ads_line_start, "请输入截断线起终点\\t",ads_line_end);
#endif
#endif
	inter_line.startPt.Set(ads_line_start[X],ads_line_start[Y],ads_line_start[Z]);
	inter_line.endPt.Set(ads_line_end[X],ads_line_end[Y],ads_line_end[Z]);
	
	//截断与直线相交的直线
	f3dLine line;
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
#ifdef _ARX_2007
	acedSSGet(L"C",ads_line_start,ads_line_end,NULL,ent_sel_set);
#else
	acedSSGet("C",ads_line_start,ads_line_end,NULL,ent_sel_set);
#endif
	long i,ll;
	acedSSLength(ent_sel_set,&ll);
	for(i=0;i<ll;i++)
	{
		AcDbEntity *pEnt;
		AcDbLine *pLine;
		acedSSName(ent_sel_set,i,entname);
		acdbGetObjectId(entId,entname);
		acdbOpenObject(pEnt,entId,AcDb::kForWrite);
		if(!pEnt)
			continue;
		if(pEnt->isKindOf(AcDbLine::desc()))
		{
			pLine=(AcDbLine*)pEnt;
			AcGePoint3d acad_start=pLine->startPoint();
			AcGePoint3d acad_end=pLine->endPoint();
			line.startPt.Set(acad_start.x,acad_start.y);
			line.endPt.Set(acad_end.x,acad_end.y);
			f3dPoint inter_pos;
			int retCode=Int3dll(line,inter_line,inter_pos);
			if(retCode==1||retCode==2)	
			{
				Cpy_Pnt(acad_end,inter_pos);
#ifdef _ARX_2007
				SetCurLayer(_bstr_t(pLine->layer()));
#else
				SetCurLayer(pLine->layer());
#endif
				AcDbLine *pBreakLine=new AcDbLine(acad_end,pLine->endPoint());
				DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pBreakLine);
				pBreakLine->close();
				pLine->setEndPoint(acad_end);
			}
		}	
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			AcDbPolyline* pPolyline=(AcDbPolyline*)pEnt;
			AcGePoint2d acad_start,acad_end,acad_inters;
			pPolyline->getPointAt(0,acad_start);
			line.startPt.Set(acad_start.x,acad_start.y);
			pPolyline->getPointAt(1,acad_end);
			line.endPt.Set(acad_end.x,acad_end.y);
			f3dPoint inter_pos;
			int retCode=Int3dll(line,inter_line,inter_pos);
			if(retCode==1||retCode==2)	
			{
				acad_inters.x=inter_pos.x;
				acad_inters.y=inter_pos.y;
#ifdef _ARX_2007
				SetCurLayer(_bstr_t(pPolyline->layer()));
#else
				SetCurLayer(pPolyline->layer());
#endif
				AcDbPolyline *pBreakLine=new AcDbPolyline(2);
				double startWidth,endWidth;
				pPolyline->getWidthsAt(0,startWidth,endWidth);
				pBreakLine->addVertexAt(0,acad_inters,0,startWidth,endWidth);
				pBreakLine->addVertexAt(1,acad_end,0,startWidth,endWidth);
				if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pBreakLine))
					pBreakLine->close();
				pPolyline->setPointAt(1,acad_inters);
			}
		}
		pEnt->close();
	}
	acedSSFree(ent_sel_set);
	pBlockTableRecord->close();
}

//绘制指定线段的焊缝线
void DrawSpecialWeld()
{	//在选中的直线或圆弧上添加焊缝线 wht 12-08-31
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	//通过选择实体获取实体的ID
	int nLineCount=0;
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	struct resbuf *filter=NULL;
	ATOM_LIST<f3dArcLine> arcLineList;
	GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
	if(acedSSGet(NULL,NULL,NULL,filter,ent_sel_set)==RTNORM)
	{
		long ll;
		acedSSLength(ent_sel_set,&ll);
		for(long i=0;i<ll;i++)
		{
			AcDbEntity *pEnt=NULL;
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			acdbOpenObject(pEnt,entId,AcDb::kForRead);
			if(pEnt==NULL)
				continue;
			AcGePoint3d acad_pnt;
			if(pEnt->isKindOf(AcDbLine::desc()))
			{
				f3dPoint start,end;
				acad_pnt=((AcDbLine*)pEnt)->startPoint();
				Cpy_Pnt(start,acad_pnt);
				acad_pnt=((AcDbLine*)pEnt)->endPoint();
				Cpy_Pnt(end,acad_pnt);
				f3dArcLine *pArcLine = arcLineList.append();
				pArcLine->CreateMethod2(start,end,f3dPoint(0,0,1),0);
				nLineCount++;
			}
			else if(pEnt->isKindOf(AcDbArc::desc()))
			{
				AcDbArc *pArc=(AcDbArc*)pEnt;
				f3dPoint center,norm;
				acad_pnt = pArc->center();
				Cpy_Pnt(center,acad_pnt);
				AcGeVector3d acad_vec = pArc->normal();
				Cpy_Pnt(norm,acad_vec);
				double fStartAngle = pArc->startAngle();
				double fEndAngle = pArc->endAngle();
				double r = pArc->radius();
				f3dPoint startPt=center+f3dPoint(1,0,0)*r;
				f3dPoint endPt=startPt;
				rotate_point_around_axis(startPt,fStartAngle,center,center+f3dPoint(0,0,1));
				rotate_point_around_axis(endPt,fEndAngle,center,center+f3dPoint(0,0,1));

				f3dArcLine *pArcLine = arcLineList.append();
				pArcLine->CreateMethod3(startPt,endPt,norm,r,center);
			}
			pEnt->close();
		}
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);

	//
	f3dPoint pickPt;
	if(nLineCount>0)
	{	//所选选段存在直线时需要选择拾取点
		ads_point pick_pt;
		while(1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			if(acedGetPoint(NULL,L"\nPlease specify the direction of weld line to pick point,<Enter Exit>:",pick_pt)==RTNORM)
#else
			if(acedGetPoint(NULL,"\nPlease specify the direction of weld line to pick point,<Enter Exit>:",pick_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
			if(acedGetPoint(NULL,L"\n请指定焊缝线方向拾取点,<Enter退出>:",pick_pt)==RTNORM)
#else
			if(acedGetPoint(NULL,"\n请指定焊缝线方向拾取点,<Enter退出>:",pick_pt)==RTNORM)
#endif
#endif
			{
				pickPt.Set(pick_pt[X],pick_pt[Y],pick_pt[Z]);
				break;		
			}
		}
	}
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	for(f3dArcLine *pLine=arcLineList.GetFirst();pLine;pLine=arcLineList.GetNext())
	{
		if(pLine->SectorAngle()==0)
			DrawWeldLineSymbol(pBlockTableRecord,pLine->Start(),pLine->End(),pickPt,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
		else
			DrawWeldLineSymbol(pBlockTableRecord,*pLine,sys.general_set.fWeldLineSymbolSpace,sys.general_set.fWeldLineSymbolHigh);
	}
	pBlockTableRecord->close();
	/*
	//如何根据指定的两点画出正确的焊缝线
	ads_point start_pt,end_pt;
	f3dPoint weldStart, weldEnd;
	while(1)
	{
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\n请指定焊缝线的起始点,<Enter退出>:",start_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\n请指定焊缝线的起始点,<Enter退出>:",start_pt)==RTNORM)
#endif
		{
			weldStart.Set(start_pt[X],start_pt[Y],start_pt[Z]);
			break;		
		}
	}
	while(1)
	{
#ifdef _ARX_2007
		if(acedGetPoint(start_pt,L"\n请指定焊缝线的终止点,<Enter退出>:",end_pt)==RTNORM)
#else
		if(acedGetPoint(start_pt,"\n请指定焊缝线的终止点,<Enter退出>:",end_pt)==RTNORM)
#endif
		{
			weldEnd.Set(end_pt[X],end_pt[Y],end_pt[Z]);
			break;		
		}
	}
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();//定义块表记录指针
	DrawWeldLineSymbol(pBlockTableRecord,weldStart,weldEnd,sys.general_set.fWeldLineSymbolHigh);
	pBlockTableRecord->close();
	*/
}

//绘制指定钢管的破断线
void DrawSpecTubeBreakLine()
{	//如何根据指定的两点画出正确的钢管破断线 
	ads_point start_pt,end_pt;
	f3dPoint breakStart, breakEnd;
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\nPlease specify the start point of tube broken line,<Enter Exit>:",start_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\nPlease specify the start point of tube broken line,<Enter Exit>:",start_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\n请指定钢管破断线的起始点,<Enter退出>:",start_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\n请指定钢管破断线的起始点,<Enter退出>:",start_pt)==RTNORM)
#endif
#endif
		{
			breakStart.Set(start_pt[X],start_pt[Y],start_pt[Z]);
			break;		
		}
		else 
			return;
	}
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if(acedGetPoint(start_pt,L"\nPlease specify the end point of tube broken line,<Enter Exit>:",end_pt)==RTNORM)
#else
		if(acedGetPoint(start_pt,"\nPlease specify the end point of tube broken line,<Enter Exit>:",end_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if(acedGetPoint(start_pt,L"\n请指定钢管破断线的终止点,<Enter退出>:",end_pt)==RTNORM)
#else
		if(acedGetPoint(start_pt,"\n请指定钢管破断线的终止点,<Enter退出>:",end_pt)==RTNORM)
#endif
#endif
		{
			breakEnd.Set(end_pt[X],end_pt[Y],end_pt[Z]);
			break;		
		}
		else 
			return;
	}
	CBreakLineDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();//定义块表记录指针
	DrawTubeBreakLine(pBlockTableRecord,breakStart,breakEnd,dlg.m_fBreakLineLen,dlg.m_fLenThreshold);
	pBlockTableRecord->close();
}

void DrawSectionViewSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint startPt,f3dPoint endPt,const char *sViewName)
{
	//获得当前图形块表记录指针
	f3dPoint line_vec,line_norm;
	line_vec=endPt-startPt;
	normalize(line_vec);	
	//直接取start->end顺时针旋转90°为剖视图开口方向 wht 11-07-15
	line_norm.Set(line_vec.y,-line_vec.x);
	/*line_norm.Set(-line_vec.y,line_vec.x);
	f3dPoint temp_vec=vecPt-startPt;
	normalize(temp_vec);	//视图方向
	if(temp_vec*line_norm<0)
		line_norm*=-1.0;*/
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	double fTextSize=sys.dim_map.fCutawayViewSymbolTextSize;//剖视图字体高度
	double symbol_len=sys.dim_map.fCutawayViewSymbolLen;	//剖视图符号大小
	double text_rot_angle=Cal2dLineAng(f2dLine(f2dPoint(0,0),f2dPoint(line_norm.x,line_norm.y)))-0.5*Pi;
	if(text_rot_angle>=0.5*Pi)	//保证字体朝上
		text_rot_angle-=Pi;
	CreateAcadPolyLine(pBlockTableRecord,startPt,startPt+symbol_len*line_vec,sys.dim_map.fCutawayViewPolyLineWidth);
	CreateAcadPolyLine(pBlockTableRecord,endPt,endPt-symbol_len*line_vec,sys.dim_map.fCutawayViewPolyLineWidth);
	CreateAcadLine(pBlockTableRecord,startPt,startPt+symbol_len*line_norm);
	CreateAcadLine(pBlockTableRecord,endPt,endPt+symbol_len*line_norm);
	f3dPoint dimPos=startPt+line_vec*0.5*symbol_len+line_norm*0.5*symbol_len;
	DimText(pBlockTableRecord,dimPos,sViewName,TextStyleTable::hzfs.textStyleId,
		fTextSize,text_rot_angle,AcDb::kTextCenter,AcDb::kTextVertMid);
	dimPos=endPt-line_vec*0.5*symbol_len+line_norm*0.5*symbol_len;
	DimText(pBlockTableRecord,dimPos,sViewName,TextStyleTable::hzfs.textStyleId,
		fTextSize,text_rot_angle,AcDb::kTextCenter,AcDb::kTextVertMid);
}

//绘制剖视图符
void DrawSpecCutawayViewSymbol()
{	//如何根据指定的两点画出剖视图图符 
	ads_point start_pt,end_pt;//,snap_pt
	f3dPoint ptStart, ptEnd;
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\nPlease specify the start point of cutaway view symbol,<Enter Exit>:",start_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\nPlease specify the start point of cutaway view symbol,<Enter Exit>:",start_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\n请指定剖视图符号的的起始点,<Enter退出>:",start_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\n请指定剖视图符号的起始点,<Enter退出>:",start_pt)==RTNORM)
#endif
#endif
		{
			ptStart.Set(start_pt[X],start_pt[Y],start_pt[Z]);
			break;		
		}
	}
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if(acedGetPoint(start_pt,L"\nPlease specify the end point of cutaway view symbol,<Enter Exit>:",end_pt)==RTNORM)
#else
		if(acedGetPoint(start_pt,"\nPlease specify the end point of cutaway view symbol,<Enter Exit>:",end_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if(acedGetPoint(start_pt,L"\n请指定剖视图符号的终止点,<Enter退出>:",end_pt)==RTNORM)
#else
		if(acedGetPoint(start_pt,"\n请指定剖视图符号的终止点,<Enter退出>:",end_pt)==RTNORM)
#endif
#endif
		{
			ptEnd.Set(end_pt[X],end_pt[Y],end_pt[Z]);
			break;		
		}
	}
	/*
	while(1)
	{
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\n请指定一个确定剖视图符号朝向的定位点,<Enter退出>:",snap_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\n请指定一个确定剖视图符号朝向的定位点,<Enter退出>:",snap_pt)==RTNORM)
#endif
		{
			ptVec.Set(snap_pt[X],snap_pt[Y],snap_pt[Z]);
			break;		
		}
	}*/
	char strName[150]="";
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		ACHAR sName[150]={0};
		if(acedGetString(0,L"\nPlease inupt a character,<Enter Exit>:",sName)==RTNORM)
#else
		if(acedGetString(0,"\nPlease inupt a character,<Enter Exit>:",strName)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		ACHAR sName[150]={0};
		if(acedGetString(0,L"\n请输入一个字符,<Enter退出>:",sName)==RTNORM)
#else
		if(acedGetString(0,"\n请输入一个字符,<Enter退出>:",strName)==RTNORM)
#endif
#endif
		{
		#ifdef _ARX_2007
			strcpy(strName,_bstr_t(sName));
		#endif
			break;
		}
	}
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();//定义块表记录指针
	DrawSectionViewSymbol(pBlockTableRecord,ptStart,ptEnd,strName);
	pBlockTableRecord->close();
}

//极坐标转换
CDecarlToPolarDlg decarl_dlg;
void PolarCsys()
{
	decarl_dlg.DoModal();
}

//批量打印
void BatchPlotPartDrawing()
{
	f3dPoint origin;
	ads_point org_L_T,org_R_B;
	int nRepeat;
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	if (ads_getpoint(NULL,L"\nPlease select the left top corner of first print unit,<Enter Exit>: ",org_L_T)!=RTNORM)
		return;
	if (ads_getpoint(NULL,L"\nPlease select the right down corner of first print unit,<Enter Exit>: ",org_R_B)!=RTNORM)
		return;
	if (ads_getint(L"\nPlease select the quantity of print unit,<Enter Exit>: ",&nRepeat)!=RTNORM)
#else
	if (ads_getpoint(NULL,"\nPlease select the left top corner of first print unit,<Enter Exit>: ",org_L_T)!=RTNORM)
		return;
	if (ads_getpoint(NULL,"\nPlease select the right down corner of first print unit,<Enter Exit>: ",org_R_B)!=RTNORM)
		return;
	if (ads_getint("\nPlease select the quantity of print unit,<Enter Exit>: ",&nRepeat)!=RTNORM)
#endif
#else
#ifdef _ARX_2007
	if (ads_getpoint(NULL,L"\n请选择第一个打印单元的左上角,<Enter退出>: ",org_L_T)!=RTNORM)
		return;
	if (ads_getpoint(NULL,L"\n请选择第一个打印单元的右下角,<Enter退出>: ",org_R_B)!=RTNORM)
		return;
	if (ads_getint(L"\n请选择需要打印的单元数,<Enter退出>: ",&nRepeat)!=RTNORM)
#else
	if (ads_getpoint(NULL,"\n请选择第一个打印单元的左上角,<Enter退出>: ",org_L_T)!=RTNORM)
		return;
	if (ads_getpoint(NULL,"\n请选择第一个打印单元的右下角,<Enter退出>: ",org_R_B)!=RTNORM)
		return;
	if (ads_getint("\n请选择需要打印的单元数,<Enter退出>: ",&nRepeat)!=RTNORM)
#endif
#endif
		return;
	int i=0;
	for(i=0;i<nRepeat;i++)
	{
		if(sys.general_set.bCardLayoutBaseRow)
		{	//构件图以行为基准进行排列
			origin.x = (i%sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(i/sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		else
		{	//构件图以列为基准进行排列
			origin.x = (i/sys.general_set.nPartRowsN)*sys.general_set.nColumnWidth;
			origin.y = -(i%sys.general_set.nPartRowsN)*sys.general_set.nRowHigh;
		}
		ads_point L_T,R_B;
		L_T[0]=origin.x+org_L_T[0];
		L_T[1]=origin.y+org_L_T[1];
		L_T[2]=org_L_T[2];
		R_B[0]=origin.x+org_R_B[0];
		R_B[1]=origin.y+org_R_B[1];
		R_B[2]=org_R_B[2];
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"CMDECHO",RTLONG,0,RTNONE);
		acedCommand(RTSTR,L"-plot",RTSTR,L"y",	//是否需要详细打印配置[是(Y)/否(N)]
			RTSTR,L"",								//输入布局名
			RTSTR,L"",							//输入页面设置名
			RTSTR,L"",							//输入输出设备的名称
			RTSTR,L"",							//输入图纸单位
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//是否反向打印
			RTSTR,L"w",							//输入打印区域
			RTPOINT,L_T,RTPOINT,R_B,
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTSTR,L"",							//输入图形方向
			RTNONE);
	#else
		acedCommand(RTSTR,"CMDECHO",RTLONG,0,RTNONE);
		acedCommand(RTSTR,"-plot",RTSTR,"y",	//是否需要详细打印配置[是(Y)/否(N)]
			RTSTR,"",								//输入布局名
			RTSTR,"",							//输入页面设置名
			RTSTR,"",							//输入输出设备的名称
			RTSTR,"",							//输入图纸单位
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//是否反向打印
			RTSTR,"w",							//输入打印区域
			RTPOINT,L_T,RTPOINT,R_B,
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTSTR,"",							//输入图形方向
			RTNONE);
#endif
	}
}

//切换出图模块 0.TMA 1.LMA 2.LDS 3.TSA wht 10-11-16
void ShiftMapArx(int iSoftType)	
{
#ifdef _ARX_2007
	//首先卸载所有出图模块
	acedRegCmds->removeGroup(L"TMA_MENU");
	acedRegCmds->removeGroup(L"LMA_MENU");
	acedRegCmds->removeGroup(L"LDS_MENU");
	acedRegCmds->removeGroup(L"TSA_MENU");
	//加载指定出图模块
	if(iSoftType==0)
	{
		acedRegCmds->removeGroup(L"ShiftMapTMAArx");
		acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTMA.arx",RTNONE);
		acedCommand(RTSTR,L"arx",RTSTR,L"L",RTSTR,sys.general_set.m_sTMAMapArxPath,RTNONE);
		acedRegCmds->removeGroup(L"ShiftMapLMAArx");
		acedRegCmds->removeGroup(L"ShiftMapLDSArx");
		acedRegCmds->removeGroup(L"ShiftMapTSAArx");
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLMA.arx",RTNONE);
		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLDS.arx",RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTSA.arx",RTNONE);
	}
	if(iSoftType==1)
	{
		acedRegCmds->removeGroup(L"ShiftMapLMAArx");
		acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLMA.arx",RTNONE);
		acedCommand(RTSTR,L"arx",RTSTR,L"L",RTSTR,sys.general_set.m_sLMAMapArxPath,RTNONE);
		acedRegCmds->removeGroup(L"ShiftMapTMAArx");
		acedRegCmds->removeGroup(L"ShiftMapLDSArx");
		acedRegCmds->removeGroup(L"ShiftMapTSAArx");
		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTMA.arx",RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLDS.arx",RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTSA.arx",RTNONE);
	}
	if(iSoftType==2)
	{
		acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLDS.arx",RTNONE);
		acedCommand(RTSTR,L"arx",RTSTR,"L",RTSTR,sys.general_set.m_sLDSMapArxPath,RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLMA.arx",RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTMA.arx",RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTSA.arx",RTNONE);
	}
	if(iSoftType==3)
	{
		acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTSA.arx",RTNONE);
		acedCommand(RTSTR,L"arx",RTSTR,L"L",RTSTR,sys.general_set.m_sTSAMapArxPath,RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLMA.arx",RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapTMA.arx",RTNONE);
 		//acedCommand(RTSTR,L"arx",RTSTR,L"U",RTSTR,L"MapLDS.arx",RTNONE);
	}
#else
	//首先卸载所有出图模块
	acedRegCmds->removeGroup("TMA_MENU");
	acedRegCmds->removeGroup("LMA_MENU");
	acedRegCmds->removeGroup("LDS_MENU");
	acedRegCmds->removeGroup("TSA_MENU");
	//加载指定出图模块
	if(iSoftType==0)
	{
		acedRegCmds->removeGroup("ShiftMapTMAArx");
		acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTMA.arx",RTNONE);
		acedCommand(RTSTR,"arx",RTSTR,"L",RTSTR,sys.general_set.m_sTMAMapArxPath,RTNONE);
		acedRegCmds->removeGroup("ShiftMapLMAArx");
		acedRegCmds->removeGroup("ShiftMapLDSArx");
		acedRegCmds->removeGroup("ShiftMapTSAArx");
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLMA.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLDS.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTSA.arx",RTNONE);
	}
	if(iSoftType==1)
	{
		acedRegCmds->removeGroup("ShiftMapLMAArx");
		acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLMA.arx",RTNONE);
		acedCommand(RTSTR,"arx",RTSTR,"L",RTSTR,sys.general_set.m_sLMAMapArxPath,RTNONE);
		acedRegCmds->removeGroup("ShiftMapTMAArx");
		acedRegCmds->removeGroup("ShiftMapLDSArx");
		acedRegCmds->removeGroup("ShiftMapTSAArx");
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTMA.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLDS.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTSA.arx",RTNONE);
	}
	if(iSoftType==2)
	{
		acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLDS.arx",RTNONE);
		acedCommand(RTSTR,"arx",RTSTR,"L",RTSTR,sys.general_set.m_sLDSMapArxPath,RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLMA.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTMA.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTSA.arx",RTNONE);
	}
	if(iSoftType==3)
	{
		acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTSA.arx",RTNONE);
		acedCommand(RTSTR,"arx",RTSTR,"L",RTSTR,sys.general_set.m_sTSAMapArxPath,RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLMA.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapTMA.arx",RTNONE);
		//acedCommand(RTSTR,"arx",RTSTR,"U",RTSTR,"MapLDS.arx",RTNONE);
	}
#endif
}

void LoadTMAMapArx() //加载TMA出图模块 
{
	ShiftMapArx(0);
}
void LoadLMAMapArx() //加载LMA出图模块
{
	ShiftMapArx(1);
}
void LoadLDSMapArx() //加载LDS出图模块
{
	ShiftMapArx(2);
}
void LoadTSAMapArx() //加载TSA出图模块
{
	ShiftMapArx(3);
}

#ifdef  afdfjldkjljdlfajkdjdk
//该函数暂时无用
void BreakSpecScopeLine()
{
	AcDbObjectId entId;
	CLDSPart *pPart=NULL;
	f3dPoint startPt,endPt,startPt1,endPt1,startPt2,endPt2;
	ads_point start_pt, end_pt;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();//定义块表记录指针
#ifdef _ARX_2007
#ifdef AFX_TARG_ENU_ENGLISH
	if (acedGetPoint(NULL,L"\nselect the start point of straight line,<Enter Exit>: ",start_pt)!=RTNORM)
		return;
	if (acedGetPoint(NULL,L"\nselect the end point of straight line,<Enter Exit>: ",end_pt)!=RTNORM)
#else
	if (acedGetPoint(NULL,L"\n选择直线的起点,<Enter退出>: ",start_pt)!=RTNORM)
		return;
	if (acedGetPoint(NULL,L"\n选择直线的终点,<Enter退出>: ",end_pt)!=RTNORM)
#endif
#else
#ifdef AFX_TARG_ENU_ENGLISH
	if (acedGetPoint(NULL,"\nselect the start point of straight line,<Enter Exit>: ",start_pt)!=RTNORM)
		return;
	if (acedGetPoint(NULL,"\nselect the end point of straight line,<Enter Exit>: ",end_pt)!=RTNORM)
#else
	if (acedGetPoint(NULL,"\n选择直线的起点,<Enter退出>: ",start_pt)!=RTNORM)
		return;
	if (acedGetPoint(NULL,"\n选择直线的终点,<Enter退出>: ",end_pt)!=RTNORM)
#endif
#endif
		return;
	startPt.Set(start_pt[X],start_pt[Y],start_pt[Z]);
	endPt.Set(end_pt[X],end_pt[Y],end_pt[Z]);
	startPt.z=endPt.z=0;
	//CreateAcadLine(pBlockTableRecord,startPt,endPt);
	AcGePoint3d acad_start,acad_end;
	Cpy_Pnt(acad_start,startPt);
	Cpy_Pnt(acad_end,endPt);
	GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
	AcDbLine *pBreakLine=new AcDbLine(acad_start,acad_end);
	//DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pBreakLine);
	pBreakLine->close();
	f3dPoint line_vec,line_norm;
	line_vec=endPt-startPt;
	normalize(line_vec);
	line_norm.Set(-line_vec.y,line_vec.x);
	startPt1=startPt+line_norm*10;
	endPt1=endPt+line_norm*10;
	CreateAcadLine(pBlockTableRecord,startPt1,endPt1);
	CreateAcadLine(pBlockTableRecord,endPt,endPt1);
	CreateAcadLine(pBlockTableRecord,startPt1,startPt);
	fPtList vertex_list;
	vertex_list.append(startPt);
	vertex_list.append(endPt);
	vertex_list.append(endPt1);
	vertex_list.append(startPt1);
	//在指定区域中找到要截断的杆件 
	long i,n,ll;
	ads_name ss_name,ent_name;
	struct resbuf *plist=NULL,*filter;
	if(vertex_list.GetNodeNum()<=1)
		return;
	f3dPoint *pPt=NULL;
	for(pPt=vertex_list.GetFirst();pPt;pPt=vertex_list.GetNext())
	{	//应使用acutNewRb()函数而不是new struct resbuf新建返回值缓存，否则使用acutRelRb()释放内存时出错 wht 12-09-21
		if(plist==NULL)
			plist=acutNewRb(RTPOINT);	//new struct resbuf;//acutBuildList(RTDXF0,"LINE",0);
		else
		{
			plist->rbnext=acutNewRb(RTPOINT);	//new struct resbuf;
			plist=plist->rbnext;
		}
		plist->restype=RTPOINT;
		plist->resval.rpoint[X]=pPt->x;
		plist->resval.rpoint[Y]=pPt->y;
		plist->resval.rpoint[Z]=0;
	}
	//结束符
	plist->rbnext=NULL;
	//plist->rbnext->restype=RTNONE;
	
#ifdef _ARX_2007
	filter = acutBuildList(RTDXF0,L"LINE",0);
	int retCode=acedSSGet(L"all",plist,NULL,filter,ss_name);////NULL,NULL,NULL,NULL,ss_name);
#else
	filter = acutBuildList(RTDXF0,"LINE",0);
	int retCode=acedSSGet("all",plist,NULL,filter,ss_name);////NULL,NULL,NULL,NULL,ss_name);
#endif
	
	//acutRelRb(plist);
	//acutRelRb(filter);
	f3dPoint startInterPt,endInterPt,startInterPt1,endInterPt1;
	if(retCode==RTNORM)
	{
		acedSSLength(ss_name,&ll);
		AcDbObjectId entId;
		fPolygonRgn rgn;
		n=vertex_list.GetNodeNum();
		f3dPoint *PtArr=new f3dPoint[n];
		if(ll>0)
		{
			for(i=0;i<n;i++)
				PtArr[i]=vertex_list[i];
			if(rgn.CreatePolygonRgn(PtArr,n))
			{	//不能构成一个合法的平面
				for(i=0;i<ll;i++)
				{
					AcDbEntity *pEnt;
					acedSSName(ss_name,i,ent_name);
					//if(!DRAGSET.IsInDragSet(ent_name))
					//	continue;
					acdbGetObjectId(entId,ent_name);
					acdbOpenObject(pEnt,entId,AcDb::kForWrite);
					long handle = GetTaAtomHandle(pEnt);
					pPart=Ta.FromPartHandle(handle);
					if(pPart&&pPart->GetClassTypeId()==CLS_LINETUBE)//||pPart->GetClassTypeId()==CLS_LINEANGLE)) //pPart->IsLinePart()) //pEnt->isKindOf(AcDbLine::desc()))
					{
						AcGePoint3d acad_pnt;
						f3dPoint start,end;
						acad_pnt=((AcDbLine*)pEnt)->startPoint();
						Cpy_Pnt(start,acad_pnt);
						acad_pnt=((AcDbLine*)pEnt)->endPoint();
						Cpy_Pnt(end,acad_pnt);
						f3dPoint vec=end-start;
						normalize(vec);	//杆件方向
						f3dPoint norm;	
						norm.Set(-vec.y,vec.x);	//垂直与杆件的方向
						f3dLine line1(startPt,startPt+vec*1000);
						f3dLine line2(endPt,endPt+norm*1000);
						Int3dll(line1,line2,startPt2);
						line1.endPt=startPt+norm*1000;
						line2.endPt=endPt+vec*1000;
						Int3dll(line1,line2,endPt2);
						//求边界线与钢管的交点用来绘制破断线
						f3dLine baseline(start,end);
						Int3dll(line1,baseline,startInterPt);
						startInterPt+=norm*((CLDSLineTube*)pPart)->d*0.5*(1.0/sys.StruMapScale);
						endInterPt=startInterPt-norm*((CLDSLineTube*)pPart)->d*(1.0/sys.StruMapScale);
						line2.endPt=endPt+norm*1000;
						Int3dll(line2,baseline,startInterPt1);
						startInterPt1+=norm*((CLDSLineTube*)pPart)->d*0.5*(1.0/sys.StruMapScale);
						endInterPt1=startInterPt1-norm*((CLDSLineTube*)pPart)->d*(1.0/sys.StruMapScale);
						pEnt->close();
						break;
					}
					pEnt->close();
				}
			}
		}
	}
	vertex_list.Empty();
	vertex_list.append(startPt);
	vertex_list.append(startPt2);
	vertex_list.append(endPt);
	vertex_list.append(endPt2);
	CreateAcadLine(pBlockTableRecord,startPt,startPt2);
	CreateAcadLine(pBlockTableRecord,startPt2,endPt);
	CreateAcadLine(pBlockTableRecord,endPt,endPt2);
	CreateAcadLine(pBlockTableRecord,endPt2,startPt);
	//删除指定区域内的线段
	//ProcessHideLine(pBlockTableRecord,vertex_list,"1.隐藏线",TRUE);
	//绘制破断线
	DrawTubeBreakLine(pBlockTableRecord,startInterPt,endInterPt);
	DrawTubeBreakLine(pBlockTableRecord,startInterPt1,endInterPt1);
	pBlockTableRecord->close();
	acedSSFree(ss_name);
	acutRelRb(plist);
	acutRelRb(filter);
}
#endif