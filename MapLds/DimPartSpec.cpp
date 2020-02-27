//<LOCALE_TRANSLATE BY hxr /> 2015-04-27
#include "stdafx.h"
#include "stdarx.h"
//#include "TMA.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "adstools.h"
#include "rxtools.h"
#include "SysPara.h"
#include "Tower.h"
#include "DrawUnit.h"
#include "DragEntSet.h"
#include "BackMirStyleDlg.h"

/*static void DimSpecialText(f3dPoint text_pos,f3dPoint start,double radius,double height,char *sPartNo,int thick=0)
{
	ads_point ads_text_pos;
	f3dPoint end,thick_pos;
	AcDbObjectId lineId,TextId,thickTextId,circleId;
	int type, track = 1;//设置鼠标拖动控制
	struct resbuf result;
	AcDbLine *pLine;
	AcDbCircle *pCircle;
	f3dPoint vec,thick_direct;
	char sThickText[20];
	AcDbText *pText,*pThickText=NULL;

	if(thick==0)
		strcpy(sThickText,"");	//置空字符串
	else
		sprintf(sThickText,"-%d",thick);
	start.z = 0;
	Cpy_Pnt(end, start);
	end.x += end.x+20;
	Sub_Pnt(vec,text_pos,start);
	normalize(vec);
	Sub_Pnt(end,text_pos,vec*radius);
	lineId = DrawLine(start,end,0);
	circleId = DrawCircle(text_pos.x,text_pos.y,radius);
	start.y += 0.5;
	thick_direct.Set(0.707,0.707,0);
	Add_Pnt(thick_pos,text_pos,thick_direct*radius);
	ads_text_pos[X] = text_pos.x;
	ads_text_pos[Y] = text_pos.y;
	ads_text_pos[Z] = 0;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	TextId = DimText(pBlockTableRecord,text_pos,sPartNo,TextStyleTable::hzfs.textStyleId,height,0);
	acdbOpenObject(pText,TextId,AcDb::kForWrite);
	pText->setVerticalMode(AcDb::kTextVertMid);
	pText->setHorizontalMode(AcDb::kTextCenter);
	pText->close();
	if(strcmp(sThickText,"")!=0)
	{	//有厚度标注
		thickTextId = DimText(pBlockTableRecord,thick_pos,sThickText,TextStyleTable::hzfs.textStyleId,height,0);
	}
 	while(track>0)
	{
		ads_grread(track,&type,&result);
		AcGePoint3d curPos(result.resval.rpoint[X],result.resval.rpoint[Y],0);
	
		acdbOpenObject(pText,TextId,AcDb::kForWrite);
		pText->setAlignmentPoint(curPos);
		pText->close();
		Sub_Pnt(vec,curPos,start);
		normalize(vec);

		if(strcmp(sThickText,"")!=0)
		{
			Add_Pnt(thick_pos,curPos,thick_direct*radius);
			acdbOpenObject(pThickText,thickTextId,AcDb::kForWrite);
			pThickText->setPosition(AcGePoint3d(thick_pos.x,thick_pos.y,0));
			pThickText->close();
		}
		acdbOpenObject(pLine,lineId,AcDb::kForWrite);
		Sub_Pnt(end,curPos,vec*radius);
		pLine->setEndPoint(AcGePoint3d(end.x,end.y,0));
		pLine->close();
		acdbOpenObject(pCircle,circleId,AcDb::kForWrite);
		pCircle->setCenter(curPos);
		pCircle->close();
		if(type==3)
			break;
	}
	pBlockTableRecord->close();
}*/

//标注指定构件明细 wht 10-12-02
CDimPartSpec * AddSinglePartSpec(AcDbObjectId entId,double fDimPosCoef,BOOL bDimByAnalysisMode=FALSE,
								 BOOL bDimPoleSpec=TRUE,BOOL bDrawRefLine=FALSE)
{
	CDimPartSpec *pPartSpec=NULL;
	int cls_typeid=0;
	AcDbEntity *pEnt=NULL;
	if(acdbOpenObject(pEnt,entId,AcDb::kForRead)!=Acad::eOk)
		return NULL;
	CAcDbObjLife entLife(pEnt);
	long drawStyle=0;
	f3dPoint line_vec;
	long handle=GetTaAtomHandle(pEnt,&drawStyle);
	CLDSDbObject *pDbObj=(CLDSDbObject*)Ta.FromHandle(handle,TRUE);
	if(pDbObj==NULL)
		return NULL;
	if(pDbObj->IsPart())
	{
		CLDSPart *pPart=(CLDSPart*)pDbObj;
		if(bDimByAnalysisMode)
		{	//TSA
			if(!(pPart->IsLinePart()&&bDimPoleSpec))
				return NULL;
		}
		cls_typeid=pPart->GetClassTypeId();
		if(pPart->IsLinePart())
		{
			double angle;
			f3dPoint dim_pos;
			BOOL bReverseLsInfo=FALSE;
			CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
			if(pEnt->isKindOf(AcDbLine::desc()))
			{
				AcGePoint3d acad_pnt;
				f3dPoint start,end;
				acad_pnt=((AcDbLine*)pEnt)->startPoint();
				Cpy_Pnt(start,acad_pnt);
				acad_pnt=((AcDbLine*)pEnt)->endPoint();
				Cpy_Pnt(end,acad_pnt);
				angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					angle-=Pi;
					dim_pos=end+(start-end)*fDimPosCoef;
					bReverseLsInfo=TRUE;
				}
				else
					dim_pos=start+(end-start)*fDimPosCoef;
				line_vec.Set(end.x-start.x,end.y-start.y);
			}
			else if(pEnt->isKindOf(AcDbPolyline::desc()))
			{	//多段线，绘制角钢截面图时角钢句柄附加在多段线上 wht 11-07-11
				AcGePoint3d acad_pnt;
				f3dPoint start,end;
				((AcDbPolyline*)pEnt)->getPointAt(0,acad_pnt);
				Cpy_Pnt(start,acad_pnt);
				((AcDbPolyline*)pEnt)->getPointAt(1,acad_pnt);
				Cpy_Pnt(end,acad_pnt);
				angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					angle-=Pi;
					dim_pos=end+(start-end)*fDimPosCoef;
					bReverseLsInfo=TRUE;
				}
				else
					dim_pos=start+(end-start)*fDimPosCoef;
			}
			else
				return NULL;
			pPartSpec=dimpartspec.AppendPartSpec(pLinePart,dim_pos,bDimByAnalysisMode,bDrawRefLine,angle);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20&&!line_vec.IsZero())
			{
				normalize(line_vec);
				if(drawStyle=='X'||drawStyle=='y')		//延伸方向顺时针转90°为肢翼展开方向(即明细标注搜索方向)
					pPartSpec->searchDirect.Set(line_vec.y,-line_vec.x);
				else if(drawStyle=='x'||drawStyle=='Y')	//延伸方向逆时针转90°为肢翼展开方向(即明细标注搜索方向)
					pPartSpec->searchDirect.Set(-line_vec.y,line_vec.x);
				//
				CLDSGroupLineAngle *pGroupJg=NULL;
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
				if(pLineAngle->IsAngle()&&pLineAngle->group_father_jg_h>0x20)
					pGroupJg=(CLDSGroupLineAngle*)Ta.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
				if(pGroupJg&&pGroupJg->group_style==2)
				{	//四主材组合角钢,无论是否启用对称标注都使用后侧编号标注当前子角钢背面的子角钢 wht 11-08-03
					CLDSLineAngle *pSonJg=NULL;
					if(pLineAngle->handle==pGroupJg->son_jg_h[0])
					{	//1号子角钢
						if(drawStyle=='x')		//绘制肢为X肢且为外铁,背面为son_jg_h[1]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
						else if(drawStyle=='y')	//绘制肢为Y肢且为外铁,背面为son_jg_h[3]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
					}
					else if(pLineAngle->handle==pGroupJg->son_jg_h[1])
					{	//2号子角钢
						if(drawStyle=='x')		//绘制肢为X肢且为外铁,背面为son_jg_h[2]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
						else if(drawStyle=='y')	//绘制肢为Y肢且为外铁,背面为son_jg_h[0]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
					}
					else if(pLineAngle->handle==pGroupJg->son_jg_h[2])
					{	//3号子角钢
						if(drawStyle=='x')		//绘制肢为X肢且为外铁,背面为son_jg_h[3]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
						else if(drawStyle=='y')	//绘制肢为Y肢且为外铁,背面为son_jg_h[1]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
					}
					else if(pLineAngle->handle==pGroupJg->son_jg_h[3])
					{	//4号子角钢
						if(drawStyle=='x')		//绘制肢为X肢且为外铁,背面为son_jg_h[0]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
						else if(drawStyle=='y')	//绘制肢为Y肢且为外铁,背面为son_jg_h[2]
							pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
					}
					if(pSonJg)
						strcpy(pPartSpec->sBackPartNo,pSonJg->GetPartNo());
				}
			}
#endif
			pPartSpec->bReverseLsInfo=bReverseLsInfo;
		}
#ifndef __TSA_FILE_
		else if(cls_typeid==CLS_PLATE)
		{
			f3dPoint dim_pos;
			AcGePoint3d acad_pnt;
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(pPlate==NULL)
				return NULL;
			if(pEnt->isKindOf(AcDbLine::desc()))
			{
				f3dPoint start,end;
				acad_pnt=((AcDbLine*)pEnt)->startPoint();
				Cpy_Pnt(start,acad_pnt);
				acad_pnt=((AcDbLine*)pEnt)->endPoint();
				Cpy_Pnt(end,acad_pnt);
				dim_pos=(start+end)/2.0;
			}
			else if(pEnt->isKindOf(AcDbPolyline::desc()))
			{
				f3dPoint start,end;
				AcGePoint2d acad_point;
				((AcDbPolyline*)pEnt)->getPointAt(0,acad_point);
				start.Set(acad_point.x,acad_point.y);
				((AcDbPolyline*)pEnt)->getPointAt(1,acad_point);
				end.Set(acad_point.x,acad_point.y);
				dim_pos=(start+end)/2.0;
			}
			else if(pEnt->isKindOf(AcDbPoint::desc()))
			{
				acad_pnt=((AcDbPoint*)pEnt)->position();
				Cpy_Pnt(dim_pos,acad_pnt);
			}
			else
				return NULL;
			pPartSpec=dimpartspec.AppendPartSpec(pPlate,dim_pos,bDimByAnalysisMode,bDrawRefLine);
		}
		else if(cls_typeid==CLS_PARAMPLATE)
		{
			f3dPoint dim_pos;
			AcGePoint3d acad_pnt;
			CLDSParamPlate *pPlate=(CLDSParamPlate*)pPart;
			if(pPlate==NULL)
				return NULL;
			if(pEnt->isKindOf(AcDbLine::desc()))
			{
				f3dPoint start,end;
				acad_pnt=((AcDbLine*)pEnt)->startPoint();
				Cpy_Pnt(start,acad_pnt);
				acad_pnt=((AcDbLine*)pEnt)->endPoint();
				Cpy_Pnt(end,acad_pnt);
				dim_pos=(start+end)/2.0;
			}
			else if(pEnt->isKindOf(AcDbPolyline::desc()))
			{
				f3dPoint start,end;
				AcGePoint2d acad_point;
				((AcDbPolyline*)pEnt)->getPointAt(0,acad_point);
				start.Set(acad_point.x,acad_point.y);
				((AcDbPolyline*)pEnt)->getPointAt(1,acad_point);
				end.Set(acad_point.x,acad_point.y);
				dim_pos=(start+end)/2.0;
			}
			else if(pEnt->isKindOf(AcDbPoint::desc()))
			{
				acad_pnt=((AcDbPoint*)pEnt)->position();
				Cpy_Pnt(dim_pos,acad_pnt);
			}
			else if(pEnt->isKindOf(AcDbCircle::desc()))
			{	//法兰标注的位置 
				f3dPoint center, norm;
				acad_pnt=((AcDbCircle*)pEnt)->center();
				Cpy_Pnt(center,acad_pnt);
				AcGeVector3d work_norm=((AcDbCircle*)pEnt)->normal();
				norm.Set(-work_norm.y,work_norm.x,0);
				dim_pos=center+norm*((AcDbCircle*)pEnt)->radius();
			}
			else
				return NULL;
			pPartSpec=dimpartspec.AppendPartSpec(pPlate,dim_pos,bDimByAnalysisMode,bDrawRefLine);
		}
#endif
	}
	else if(pDbObj->IsNode()&&bDimByAnalysisMode)
	{
		f3dPoint dim_pos;
		AcGePoint3d acad_pnt;
		CLDSNode *pNode=(CLDSNode*)pDbObj;
		if(pEnt->isKindOf(AcDbPoint::desc()))
		{
			acad_pnt=((AcDbPoint*)pEnt)->position();
			Cpy_Pnt(dim_pos,acad_pnt);
		}
		else
			return NULL;
		pPartSpec=dimpartspec.AppendPartSpec(pNode,dim_pos,bDimByAnalysisMode,bDrawRefLine);
	}
	return pPartSpec;
}

void DimPartSpec()
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	//通过选择实体获取实体的ID
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	static CBackMirStyleDlg backmirstyledlg;
	struct resbuf *filter;
    CAcModuleResourceOverride resOverride;
	//
	BOOL bDimByAnalysisMode=FALSE;	//是否以应力简图形式标注
#if defined(__TSA_FILE_)
	bDimByAnalysisMode=TRUE;		//节点	
#elif defined(__LDS_FILE_)
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
#else //defined(__LMA_FILE_)||defined(__TMA_FILE_)
	bDimByAnalysisMode=FALSE;
#endif

	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	dimpartspec.EmptyPartSpecDim();
#ifdef _ARX_2007
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);
#else
	acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);
#endif
#ifdef _MAPTMA_GRX
	acedCommand(RTSTR,"RE",RTNONE);	//在ICAD下PDMODE后暂时需要重新生成
#endif
	backmirstyledlg.m_iBackMirStyle=sys.dim_map.PartNo.iMirMsg;		//对称方式
	backmirstyledlg.m_bDrawRefLine=sys.dim_map.PartNo.bDrawRefLine;	//绘制引出线  wht 11-03-29
#ifdef _ARX_2007
	filter = acutBuildList(RTDXF0,L"POINT,LINE",0);
#else
	filter = acutBuildList(RTDXF0,"POINT,LINE",0);
#endif
	if(acedSSGet(NULL,NULL,NULL,NULL,ent_sel_set)==RTNORM)
	{
		long ll;
		acedSSLength(ent_sel_set,&ll);
		if(!bDimByAnalysisMode)
		{
			if(backmirstyledlg.DoModal()==IDOK)
			{
				dimpartspec.SetBackMirStyle(backmirstyledlg.m_iBackMirStyle);
				dimpartspec.m_bMergeNearSampePartNoDim=backmirstyledlg.m_bDimOnceRoundSamePart;
			}
			else
			{
				acedSSFree(ent_sel_set);
			#ifdef _ARX_2007
				acedCommand(RTSTR,L"PDMODE",RTSTR,L"1",RTNONE);
				acedCommand(RTSTR,L"RE",RTNONE);	//在ICAD下PDMODE后暂时需要重新生成
			#else
				acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);
				acedCommand(RTSTR,"RE",RTNONE);	//在ICAD下PDMODE后暂时需要重新生成
			#endif
				acutRelRb(filter);
				return;
			}
		}
		double fDimPosCoef=0.3;	//LTMA 0.3  TAS 0.5
		BOOL bDimPoleSpec=TRUE;
		if(bDimByAnalysisMode)
		{
#ifdef _ARX_2007
			ACHAR result[20]={0};
#ifdef AFX_TARG_ENU_ENGLISH
			acedGetString(FALSE,L"Dimension rod information or node serial at the same time or not or [Only dimension node serial(N)]:",_bstr_t(result));
#else
			acedGetString(FALSE,L"是否将杆件信息与节点编号同时标注或 [只标注节点编号(N)]:",(ACHAR*)_bstr_t(result));
#endif
			if(wcsicmp(result,L"S")==0)
#else
			char result[20]="";
#ifdef AFX_TARG_ENU_ENGLISH
			acedGetString(FALSE,"Dimension rod information or node serial at the same time or not or [Only dimension node serial(N)]:",result);
#else
			acedGetString(FALSE,"是否将杆件信息与节点编号同时标注或 [只标注节点编号(N)]:",result);
#endif
			if(stricmp(result,"N")==0)
#endif
				bDimPoleSpec=FALSE;
			else
				bDimPoleSpec=TRUE;
			fDimPosCoef=0.5;	//LTMA 0.3  TAS 0.5
		}
		for(long i=0;i<ll;i++)
		{
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			//添加单个构件明细标注
			AddSinglePartSpec(entId,fDimPosCoef,bDimByAnalysisMode,bDimPoleSpec,backmirstyledlg.m_bDrawRefLine);
		}
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		dimpartspec.DimToMap(pBlockTableRecord,bDimByAnalysisMode);
		pBlockTableRecord->close();
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"1",RTNONE);
#else
	acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);
#endif
#ifdef _MAPTMA_GRX
	acedCommand(RTSTR,"RE",RTNONE);	//在ICAD下PDMODE后暂时需要重新生成
#endif
}

//标注指定指定杆件正负头 wht 10-12-03
#ifdef __SMART_DRAW_INC_
BOOL DimSinglePoleOddment(AcDbBlockTableRecord *pBlockTableRecord,CDrawingCommand *pCmd)
{
	if(pCmd==NULL||pBlockTableRecord==NULL)
		return FALSE;
	if(pCmd->m_eCmdType!=CMD_DIM_ODDMENT)
		return FALSE;
	if(pCmd->m_paraOddment.pLinePart==NULL)
		pCmd->m_paraOddment.pLinePart=(CLDSLinePart*)Ta.FromPartHandle(pCmd->m_paraOddment.hLinePart,CLS_LINEPART);
	if(pCmd->m_paraOddment.pLinePart==NULL)
		return FALSE;
	if(pCmd->m_pDrawingCom==NULL)
		return FALSE;
	CLDSLinePart *pLinePart=pCmd->m_paraOddment.pLinePart;
	if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
		return FALSE;
	CDrawingComponent *pDrawing=pCmd->m_pDrawingCom;
	double draw_scale=0.05;
	if(pDrawing)
		draw_scale=pDrawing->m_fScale;
	double wide=pLinePart->GetWidth();
	double start_odd=pLinePart->startOdd();
	double end_odd=pLinePart->endOdd();
	int start_std_odd=0,end_std_odd=0;
	if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
	{
		int x_wing0_y_wing1=-1;
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
		//计算角钢当前绘制肢
		CDrawLineAngleTagInfo drawInfo;
		CDrawLineAngleTagInfo *pInfo=NULL;
		if(pDrawing)
			pInfo=pDrawing->hashAngleDrawInfo.GetValue(pLinePart->handle);
		if(pInfo==NULL)
		{
			CalPartDrawType(pLinePart,pCmd->m_pDrawingCom->GetDCS(),pCmd->m_pDrawingCom->m_pAttachView,&drawInfo);
			pInfo=&drawInfo;
			int start_ls_d,end_ls_d;
			if(pLineAngle->GetStartEndBoltD(&start_ls_d,&end_ls_d))
			{
				LSSPACE_STRU LsSpace;
				GetLsSpace(LsSpace,start_ls_d);
				start_std_odd=LsSpace.EndSpace;
				GetLsSpace(LsSpace,end_ls_d);
				end_std_odd=LsSpace.EndSpace;
			}
		}
		else
		{
			start_std_odd=pInfo->start_std_odd;
			end_std_odd=pInfo->end_std_odd;
		}
		if(toupper(pInfo->m_cDrawStyle)=='X')
			x_wing0_y_wing1=0;
		else if(toupper(pInfo->m_cDrawStyle)=='Y')
			x_wing0_y_wing1=1;
		if(x_wing0_y_wing1==0||x_wing0_y_wing1==1)	
		{	//考虑角钢固定正负头 wht 11-08-02
			start_odd+=pLineAngle->GetStartExtraOdd(x_wing0_y_wing1);
			end_odd+=pLineAngle->GetEndExtraOdd(x_wing0_y_wing1);
		}
	}
	CXhChar16 sStartOdd,sEndOdd;
	if(ftoi(start_odd)!=0&&ftoi(start_odd)!=start_std_odd)
		sStartOdd.Printf("%+d",ftoi(start_odd));
	if(ftoi(end_odd)!=0&&ftoi(end_odd)!=end_std_odd)
		sEndOdd.Printf("%+d",ftoi(end_odd));
	//在CAD中进行正负头标注绘制
	if(pCmd->m_paraOddment.iDimPosType!=0)
	{
		if(pCmd->m_paraOddment.fPoleAngle<0)
		{
			AcDbEntity *pEnt=NULL;
			AcDbObjectId *pEntId=NULL;
			pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraOddment.hLinePart);
			if(pEntId!=NULL)
			{
				acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
				CAcDbObjLife entLife(pEnt);
				if(pEnt!=NULL&&pEnt->isKindOf(AcDbLine::desc()))
				{
					f3dPoint start,end;
					AcGePoint3d acad_pnt=((AcDbLine*)pEnt)->startPoint();
					Cpy_Pnt(start,acad_pnt);
					acad_pnt=((AcDbLine*)pEnt)->endPoint();
					Cpy_Pnt(end,acad_pnt);
					pCmd->m_paraOddment.fPoleAngle=Cal2dLineAng(start.x,start.y,end.x,end.y);
				}
			}
		}
		double dim_angle=pCmd->m_paraOddment.fPoleAngle;
		AcDb::TextHorzMode hStartMode=AcDb::kTextLeft;
		AcDb::TextHorzMode hEndMode=AcDb::kTextRight;
		if(pCmd->m_paraOddment.fPoleAngle>Pi/2.0&&pCmd->m_paraOddment.fPoleAngle<=1.5*Pi)
		{
			hStartMode=AcDb::kTextRight;
			hEndMode=AcDb::kTextLeft;
			dim_angle=pCmd->m_paraOddment.fPoleAngle-Pi;
		}
		double height=sys.dim_map.fJgOddmentTextSize;
		//标注起始端正负头
		if(strlen(sStartOdd)>0&&pCmd->m_paraOddment.bDimStartOdd)
		{
			f3dPoint start(pCmd->m_paraOddment.fStartOddBasePosX+pCmd->m_paraOddment.fStartOddPosOffsetX,
				pCmd->m_paraOddment.fStartOddBasePosY+pCmd->m_paraOddment.fStartOddPosOffsetY);
			DimText(pBlockTableRecord,start,sStartOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,hStartMode,AcDb::kTextVertMid);
		}
		else
			pCmd->m_paraOddment.bDimStartOdd=false;
		//标注终止端正负头
		if(strlen(sEndOdd)>0&&pCmd->m_paraOddment.bDimEndOdd)
		{
			f3dPoint end(pCmd->m_paraOddment.fEndOddBasePosX+pCmd->m_paraOddment.fEndOddPosOffsetX,
				pCmd->m_paraOddment.fEndOddBasePosY+pCmd->m_paraOddment.fEndOddPosOffsetY);
			DimText(pBlockTableRecord,end,sEndOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,hEndMode,AcDb::kTextVertMid);
		}
		else
			pCmd->m_paraOddment.bDimEndOdd=false;
	}
	else 
	{
		AcDbEntity *pEnt=NULL;
		AcDbObjectId *pEntId=NULL;
		pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraOddment.hLinePart);
		if(pEntId==NULL)
			return FALSE;
		acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
		if(pEnt==NULL)
			return FALSE;
		double angle=0;
		f3dPoint dim_pos;
		if(pLinePart->IsLinePart()&&pEnt->isKindOf(AcDbLine::desc()))	//pEnt为AcDbLine wht 11-07-13
		{
			AcGePoint3d acad_pnt;
			f3dPoint start,end;
			acad_pnt=((AcDbLine*)pEnt)->startPoint();
			Cpy_Pnt(start,acad_pnt);
			acad_pnt=((AcDbLine*)pEnt)->endPoint();
			Cpy_Pnt(end,acad_pnt);
			angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
			//记录杆件倾斜角 wht 11-07-25
			pCmd->m_paraOddment.fPoleAngle=angle;
			double dim_angle;
			if(angle>Pi/2.0&&angle<=1.5*Pi)
				dim_angle=angle-Pi;
			else
				dim_angle=angle;
			f3dPoint vec=end-start;
			normalize(vec);
			if(pLinePart->startOdd()>=0)
				start=start+vec*90*draw_scale;							//因为角钢轴线两端缺省就多划30mm
			else
				start=start+vec*(90-pLinePart->startOdd())*draw_scale;	//因为角钢轴线两端缺省就多划30mm
			if(pLinePart->endOdd()>=0)
				end=end-vec*90*draw_scale;								//再加上两个角钢端距和正负头
			else
				end=end-vec*(90-pLinePart->endOdd())*draw_scale;		//再加上两个角钢端距和正负头
			UCS_STRU ucs=pLinePart->ucs;
			long start_ls_d,end_ls_d;
			double min_ls_z,max_ls_z;
			CLsRef *pLsRef=NULL;
			pLsRef=pLinePart->GetFirstLsRef();
			if(pLsRef)
			{
				f3dPoint ls_pos=(*pLsRef)->ucs.origin;
				start_ls_d=end_ls_d=(*pLsRef)->get_d();
				coord_trans(ls_pos,ucs,FALSE);
				min_ls_z=max_ls_z=ls_pos.z;
			}
			else
			{
				start_ls_d=end_ls_d=20;
				min_ls_z=max_ls_z=0;
			}
			for(pLsRef=pLinePart->GetFirstLsRef();pLsRef;pLsRef=pLinePart->GetNextLsRef())
			{
				f3dPoint pos((*pLsRef)->ucs.origin);
				coord_trans(pos,ucs,FALSE);
				if(pos.z<min_ls_z)
				{
					start_ls_d=(*pLsRef)->get_d();
					min_ls_z=pos.z;
				}
				if(pos.z>max_ls_z)
				{
					end_ls_d=(*pLsRef)->get_d();
					max_ls_z=pos.z;
				}
			}
			LSSPACE_STRU LsSpace;
			f3dPoint dimPos,rgn_vert[4];
			double height=sys.dim_map.fJgOddmentTextSize;
			double length;
			//标注起始端正负头
			GetLsSpace(LsSpace,start_ls_d);
			int std_start_odd=LsSpace.EndSpace;
			if((pLinePart->IsAngle()&&ftoi(start_odd)!=std_start_odd&&ftoi(start_odd)!=0)
				||(pLinePart->IsTube()&&fabs(start_odd)>=0.5))	//标注钢管正负头时无标准端距限制 wht 11-08-01
			{
				if(fabs(start_odd)>=100)
					length=3*height*0.7;
				else
					length=1.5*height;
				if(start_odd<0)
					length+=2.25;
				
				int quad_order[4]={0};
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					start=GetPtInPolar(start,angle-Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(start,angle+Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				else
				{
					start=GetPtInPolar(start,angle+Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(start,angle-Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				char sOdd[8];
				sprintf(sOdd,"%+d",ftoi(start_odd));
				if(SearchNoneIntersPos(&start,rgn_vert,NULL,quad_order)!=0)
				{
					DRAGSET.ClearEntSet();
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
					f3dPoint box_vertex[4];
					ads_point L_T,R_B;
					SCOPE_STRU scope;
					box_vertex[0]=GetPtInPolar(start,angle-Pi/2,20);
					box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
					box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
					box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
					for(int jj=0;jj<4;jj++)
						scope.VerifyVertex(box_vertex[jj]);
					L_T[X]=scope.fMinX;
					L_T[Y]=scope.fMaxY;
					L_T[Z]=0;
					R_B[X]=scope.fMaxX;
					R_B[Y]=scope.fMinY;
					R_B[Z]=0;
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
#ifdef AFX_TARG_ENU_ENGLISH
					DragEntSet(start,"\n Please select a right dimension place:\n");
#else
					DragEntSet(start,"\n请选择合适的标注位置:\n");
#endif
					//必须恢复原有的视图状态，不然acedSSGet函数调用失败
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
				#else
					acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
				#endif
				}
				else
				{
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
				}
				//记录杆件始端正负头标注位置 wht 11-07-25
				pCmd->m_paraOddment.fStartOddPosOffsetX=start.x-pCmd->m_paraOddment.fStartOddBasePosX;
				pCmd->m_paraOddment.fStartOddPosOffsetY=start.y-pCmd->m_paraOddment.fStartOddBasePosY;
				pCmd->m_paraOddment.bDimStartOdd=TRUE;
			}
			else	//不需要标注始端正负头
				pCmd->m_paraOddment.bDimStartOdd=FALSE;
			
			//标注终止端正负头
			GetLsSpace(LsSpace,end_ls_d);
			int std_end_odd=LsSpace.EndSpace;
			if((pLinePart->IsAngle()&&ftoi(end_odd)!=std_end_odd&&ftoi(end_odd)!=0)
				||(pLinePart->IsTube()&&ftoi(end_odd)!=0))	//标注钢管正负头时无标准端距限制 wht 11-08-01
			{
				if(fabs(end_odd)>=100)
					length=3*height*0.7;
				else
					length=1.5*height;
				if(end_odd<0)
					length+=2.25;
				
				int quad_order[4]={0};
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					end=GetPtInPolar(end,angle-Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(end,angle+Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				else
				{
					end=GetPtInPolar(end,angle+Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(end,angle-Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				char sOdd[8];
				sprintf(sOdd,"%+d",ftoi(end_odd));
				if(SearchNoneIntersPos(&end,rgn_vert,NULL,quad_order)!=0)
				{
					DRAGSET.ClearEntSet();
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
					f3dPoint box_vertex[4];
					ads_point L_T,R_B;
					SCOPE_STRU scope;
					box_vertex[0]=GetPtInPolar(end,angle-Pi/2,20);
					box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
					box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
					box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
					for(int jj=0;jj<4;jj++)
						scope.VerifyVertex(box_vertex[jj]);
					L_T[X]=scope.fMinX;
					L_T[Y]=scope.fMaxY;
					L_T[Z]=0;
					R_B[X]=scope.fMaxX;
					R_B[Y]=scope.fMinY;
					R_B[Z]=0;
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
#ifdef AFX_TARG_ENU_ENGLISH
					DragEntSet(end,"\n Please select a right dimension place:\n");
#else
					DragEntSet(end,"\n请选择合适的标注位置:\n");
#endif
					//必须恢复原有的视图状态，不然acedSSGet函数调用失败
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
				#else
					acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
				#endif
				}
				else
				{
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
				}
				//记录杆件终端正负头标注位置 wht 11-07-25
				pCmd->m_paraOddment.fEndOddPosOffsetX=end.x-pCmd->m_paraOddment.fEndOddBasePosX;
				pCmd->m_paraOddment.fEndOddPosOffsetY=end.y-pCmd->m_paraOddment.fEndOddBasePosY;
				pCmd->m_paraOddment.bDimEndOdd=TRUE;
			}
			else //不需要终端标注正负头
				pCmd->m_paraOddment.bDimEndOdd=FALSE;
		}
		pEnt->close();
	}
	return TRUE;
}
//沿基准杆件方向标注构件明细(主要用于法兰及标准插板)
BOOL DimPartSpecByDatumRod(AcDbBlockTableRecord *pBlockTableRecord,CDrawingCommand *pCmd,CDimPartSpec *pPartSpec)
{
	if(pCmd==NULL||pBlockTableRecord==NULL)
		return FALSE;
	if(pCmd->m_eCmdType!=CMD_DIM_PARTSPEC)
		return FALSE;
	if(pCmd->m_pDrawingCom==NULL)
		return FALSE;
	AcDbEntity *pRodEnt=NULL;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pCmd->m_paraPartSpec.pObj;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)Ta.FromPartHandle(pParamPlate->m_hPartWeldParent);
	if(pDatumTube==NULL)
		return FALSE;
	bool bDimStartPartSpec=false;
	if(pDatumTube->m_tJointStart.hLinkObj==pParamPlate->handle)
		bDimStartPartSpec=true;	//板在始端侧
	AcDbObjectId *pId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pParamPlate->m_hPartWeldParent);
	if(pId)
	{	
		acdbOpenObject(pRodEnt,*pId,AcDb::kForRead);
		if(pRodEnt)
			pRodEnt->close();
	}
	if(pRodEnt&&pRodEnt->isKindOf(AcDbLine::desc()))
	{
		AcDbLine *pLine=(AcDbLine*)pRodEnt;
		pPartSpec->rod_angle=Cal2dLineAng(pLine->startPoint().x,pLine->startPoint().y,
			pLine->endPoint().x,pLine->endPoint().y);
		Sub_Pnt(pPartSpec->rod_vec,pLine->endPoint(),pLine->startPoint());
		normalize(pPartSpec->rod_vec);
	}
	
	CDrawingComponent *pDrawing=pCmd->m_pDrawingCom;
	double draw_scale=0.05;
	if(pDrawing)
		draw_scale=pDrawing->m_fScale;
	double wide=pDatumTube->GetWidth();
	double start_odd=pDatumTube->startOdd();
	double end_odd=pDatumTube->endOdd();
	//在CAD中进行正负头标注绘制
	if(pCmd->m_paraPartSpec.iDimPosType!=0)	//指定绘制位置
	{
		if(pCmd->m_paraOddment.fPoleAngle<0)
		{
			AcDbEntity *pEnt=NULL;
			AcDbObjectId *pEntId=NULL;
			pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraOddment.hLinePart);
			if(pEntId!=NULL)
			{
				acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
				CAcDbObjLife entLife(pEnt);
				if(pEnt!=NULL&&pEnt->isKindOf(AcDbLine::desc()))
				{
					f3dPoint start,end;
					AcGePoint3d acad_pnt=((AcDbLine*)pEnt)->startPoint();
					Cpy_Pnt(start,acad_pnt);
					acad_pnt=((AcDbLine*)pEnt)->endPoint();
					Cpy_Pnt(end,acad_pnt);
					pCmd->m_paraOddment.fPoleAngle=Cal2dLineAng(start.x,start.y,end.x,end.y);
					//pEnt->close();
				}
			}
		}
		double dim_angle=pCmd->m_paraOddment.fPoleAngle;
		AcDb::TextHorzMode hStartMode=AcDb::kTextLeft;
		AcDb::TextHorzMode hEndMode=AcDb::kTextRight;
		if(pCmd->m_paraOddment.fPoleAngle>Pi/2.0&&pCmd->m_paraOddment.fPoleAngle<=1.5*Pi)
		{
			hStartMode=AcDb::kTextRight;
			hEndMode=AcDb::kTextLeft;
			dim_angle=pCmd->m_paraOddment.fPoleAngle-Pi;
		}
		double height=sys.dim_map.fJgOddmentTextSize;
		/*//标注起始端正负头
		if(strlen(sStartOdd)>0&&pCmd->m_paraOddment.bDimStartOdd)
		{
			f3dPoint start(pCmd->m_paraOddment.fStartOddBasePosX+pCmd->m_paraOddment.fStartOddPosOffsetX,
				pCmd->m_paraOddment.fStartOddBasePosY+pCmd->m_paraOddment.fStartOddPosOffsetY);
			DimText(pBlockTableRecord,start,sStartOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,hStartMode,AcDb::kTextVertMid);
		}
		else
			pCmd->m_paraOddment.bDimStartOdd=false;
		//标注终止端正负头
		if(strlen(sEndOdd)>0&&pCmd->m_paraOddment.bDimEndOdd)
		{
			f3dPoint end(pCmd->m_paraOddment.fEndOddBasePosX+pCmd->m_paraOddment.fEndOddPosOffsetX,
				pCmd->m_paraOddment.fEndOddBasePosY+pCmd->m_paraOddment.fEndOddPosOffsetY);
			DimText(pBlockTableRecord,end,sEndOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,hEndMode,AcDb::kTextVertMid);
		}
		else
			pCmd->m_paraOddment.bDimEndOdd=false;*/
	}
	else	//自动搜索位置
	{
		AcDbEntity *pEnt=NULL;
		AcDbObjectId *pEntId=NULL;
		pEntId=pCmd->m_pDrawingCom->keyEntIdList.GetValue(pCmd->m_paraOddment.hLinePart);
		if(pEntId==NULL)
			return FALSE;
		acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
		if(pEnt==NULL)
			return FALSE;
		double angle=0;
		f3dPoint dim_pos;
		if(pDatumTube->IsLinePart()&&pEnt->isKindOf(AcDbLine::desc()))	//pEnt为AcDbLine wht 11-07-13
		{
			AcGePoint3d acad_pnt;
			f3dPoint start,end;
			acad_pnt=((AcDbLine*)pEnt)->startPoint();
			Cpy_Pnt(start,acad_pnt);
			acad_pnt=((AcDbLine*)pEnt)->endPoint();
			Cpy_Pnt(end,acad_pnt);
			angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
			//记录杆件倾斜角 wht 11-07-25
			pCmd->m_paraOddment.fPoleAngle=angle;
			double dim_angle;
			if(angle>Pi/2.0&&angle<=1.5*Pi)
				dim_angle=angle-Pi;
			else
				dim_angle=angle;
			f3dPoint vec=end-start;
			normalize(vec);
			if(pDatumTube->startOdd()>=0)
				start=start+vec*90*draw_scale;							//因为角钢轴线两端缺省就多划30mm
			else
				start=start+vec*(90-pDatumTube->startOdd())*draw_scale;	//因为角钢轴线两端缺省就多划30mm
			if(pDatumTube->endOdd()>=0)
				end=end-vec*90*draw_scale;								//再加上两个角钢端距和正负头
			else
				end=end-vec*(90-pDatumTube->endOdd())*draw_scale;		//再加上两个角钢端距和正负头
			UCS_STRU ucs=pDatumTube->ucs;
			long start_ls_d,end_ls_d;
			double min_ls_z,max_ls_z;
			CLsRef *pLsRef=NULL;
			pLsRef=pDatumTube->GetFirstLsRef();
			if(pLsRef)
			{
				f3dPoint ls_pos=(*pLsRef)->ucs.origin;
				start_ls_d=end_ls_d=(*pLsRef)->get_d();
				coord_trans(ls_pos,ucs,FALSE);
				min_ls_z=max_ls_z=ls_pos.z;
			}
			else
			{
				start_ls_d=end_ls_d=20;
				min_ls_z=max_ls_z=0;
			}
			for(pLsRef=pDatumTube->GetFirstLsRef();pLsRef;pLsRef=pDatumTube->GetNextLsRef())
			{
				f3dPoint pos((*pLsRef)->ucs.origin);
				coord_trans(pos,ucs,FALSE);
				if(pos.z<min_ls_z)
				{
					start_ls_d=(*pLsRef)->get_d();
					min_ls_z=pos.z;
				}
				if(pos.z>max_ls_z)
				{
					end_ls_d=(*pLsRef)->get_d();
					max_ls_z=pos.z;
				}
			}
			LSSPACE_STRU LsSpace;
			f3dPoint dimPos,rgn_vert[4];
			double height=sys.dim_map.fJgOddmentTextSize;
			double length;
			//标注起始端正负头
			GetLsSpace(LsSpace,start_ls_d);
			int std_start_odd=LsSpace.EndSpace;
			if((pDatumTube->IsAngle()&&ftoi(start_odd)!=std_start_odd&&ftoi(start_odd)!=0)
				||(pDatumTube->IsTube()&&fabs(start_odd)>=0.5))	//标注钢管正负头时无标准端距限制 wht 11-08-01
			{
				if(fabs(start_odd)>=100)
					length=3*height*0.7;
				else
					length=1.5*height;
				if(start_odd<0)
					length+=2.25;
				
				int quad_order[4]={0};
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					start=GetPtInPolar(start,angle-Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(start,angle+Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				else
				{
					start=GetPtInPolar(start,angle+Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(start,angle-Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				char sOdd[8];
				sprintf(sOdd,"%+d",ftoi(start_odd));
				if(SearchNoneIntersPos(&start,rgn_vert,NULL,quad_order)!=0)
				{
					DRAGSET.ClearEntSet();
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
					f3dPoint box_vertex[4];
					ads_point L_T,R_B;
					SCOPE_STRU scope;
					box_vertex[0]=GetPtInPolar(start,angle-Pi/2,20);
					box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
					box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
					box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
					for(int jj=0;jj<4;jj++)
						scope.VerifyVertex(box_vertex[jj]);
					L_T[X]=scope.fMinX;
					L_T[Y]=scope.fMaxY;
					L_T[Z]=0;
					R_B[X]=scope.fMaxX;
					R_B[Y]=scope.fMinY;
					R_B[Z]=0;
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
#ifdef AFX_TARG_ENU_ENGLISH
					DragEntSet(start,"\n Please select a right dimension place:\n");
#else
					DragEntSet(start,"\n请选择合适的标注位置:\n");
#endif
					//必须恢复原有的视图状态，不然acedSSGet函数调用失败
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
				#else
					acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
				#endif
				}
				else
				{
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
				}
				//记录杆件始端正负头标注位置 wht 11-07-25
				pCmd->m_paraOddment.fStartOddPosOffsetX=start.x-pCmd->m_paraOddment.fStartOddBasePosX;
				pCmd->m_paraOddment.fStartOddPosOffsetY=start.y-pCmd->m_paraOddment.fStartOddBasePosY;
				pCmd->m_paraOddment.bDimStartOdd=TRUE;
			}
			else	//不需要标注始端正负头
				pCmd->m_paraOddment.bDimStartOdd=FALSE;
			
			//标注终止端正负头
			GetLsSpace(LsSpace,end_ls_d);
			int std_end_odd=LsSpace.EndSpace;
			if((pDatumTube->IsAngle()&&ftoi(end_odd)!=std_end_odd&&ftoi(end_odd)!=0)
				||(pDatumTube->IsTube()&&ftoi(end_odd)!=0))	//标注钢管正负头时无标准端距限制 wht 11-08-01
			{
				if(fabs(end_odd)>=100)
					length=3*height*0.7;
				else
					length=1.5*height;
				if(end_odd<0)
					length+=2.25;
				
				int quad_order[4]={0};
				if(angle>Pi/2.0&&angle<=1.5*Pi)
				{
					end=GetPtInPolar(end,angle-Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(end,angle+Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				else
				{
					end=GetPtInPolar(end,angle+Pi/2,0.5*wide*draw_scale);
					rgn_vert[0]=GetPtInPolar(end,angle-Pi/2,height/2+0.3);
					rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
					rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
					rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
					quad_order[0]=1;quad_order[1]=4;
				}
				char sOdd[8];
				sprintf(sOdd,"%+d",ftoi(end_odd));
				if(SearchNoneIntersPos(&end,rgn_vert,NULL,quad_order)!=0)
				{
					DRAGSET.ClearEntSet();
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
					f3dPoint box_vertex[4];
					ads_point L_T,R_B;
					SCOPE_STRU scope;
					box_vertex[0]=GetPtInPolar(end,angle-Pi/2,20);
					box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
					box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
					box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
					for(int jj=0;jj<4;jj++)
						scope.VerifyVertex(box_vertex[jj]);
					L_T[X]=scope.fMinX;
					L_T[Y]=scope.fMaxY;
					L_T[Z]=0;
					R_B[X]=scope.fMaxX;
					R_B[Y]=scope.fMinY;
					R_B[Z]=0;
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#else
					acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
				#endif
#ifdef AFX_TARG_ENU_ENGLISH
					DragEntSet(end,"\n Please select a right dimension place:\n");
#else
					DragEntSet(end,"\n请选择合适的标注位置:\n");
#endif
					//必须恢复原有的视图状态，不然acedSSGet函数调用失败
				#ifdef _ARX_2007
					acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
				#else
					acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
				#endif
				}
				else
				{
					if(angle>Pi/2.0&&angle<=1.5*Pi)
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
					else
						DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
				}
				//记录杆件终端正负头标注位置 wht 11-07-25
				pCmd->m_paraOddment.fEndOddPosOffsetX=end.x-pCmd->m_paraOddment.fEndOddBasePosX;
				pCmd->m_paraOddment.fEndOddPosOffsetY=end.y-pCmd->m_paraOddment.fEndOddBasePosY;
				pCmd->m_paraOddment.bDimEndOdd=TRUE;
			}
			else //不需要终端标注正负头
				pCmd->m_paraOddment.bDimEndOdd=FALSE;
		}
		pEnt->close();
	}
	return TRUE;
}
#endif

void DimOddment()
#ifndef __TSA_FILE_
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	//通过选择实体获取实体的ID
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	struct resbuf *filter;

	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	dimpartspec.EmptyPartSpecDim();
#ifdef _ARX_2007
	filter = acutBuildList(RTDXF0,L"LINE",0);
#else
	filter = acutBuildList(RTDXF0,"LINE",0);
#endif
	if(acedSSGet(NULL,NULL,NULL,filter,ent_sel_set)==RTNORM)
	{
		long ll;
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		acedSSLength(ent_sel_set,&ll);
		for(long i=0;i<ll;i++)
		{
			int cls_typeid=0;
			AcDbEntity *pEnt;
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			acdbOpenObject(pEnt,entId,AcDb::kForRead);
			if(pEnt==NULL)
				continue;
			long handle=GetTaAtomHandle(pEnt);
			double angle;
			f3dPoint dim_pos;
			CSuperSmartPtr<CLDSPart> pDimPart=Ta.FromPartHandle(handle);
			if(pDimPart.IsHasPtr()&&pDimPart->IsLinePart()
				&&pEnt->isKindOf(AcDbLine::desc()))	//pEnt为AcDbLine wht 11-07-13
			{
				if(pDimPart.LinePartPointer()->pStart==NULL||pDimPart.LinePartPointer()->pEnd==NULL)
				{	//不需要标注短角钢的正负头信息 wht 11-01-10
					pEnt->close();
					continue;
				}
				AcGePoint3d acad_pnt;
				f3dPoint start,end;
				acad_pnt=((AcDbLine*)pEnt)->startPoint();
				Cpy_Pnt(start,acad_pnt);
				acad_pnt=((AcDbLine*)pEnt)->endPoint();
				Cpy_Pnt(end,acad_pnt);
				angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
				double dim_angle;
				if(angle>Pi/2.0&&angle<=1.5*Pi)
					dim_angle=angle-Pi;
				else
					dim_angle=angle;
				f3dPoint vec=end-start;
				normalize(vec);
				if(pDimPart.LinePartPointer()->startOdd()>=0)
					start=start+vec*90/sys.general_set.StruMapScale.fRealToDraw;	//因为角钢轴线两端缺省就多划30mm
				else
					start=start+vec*(90-pDimPart.LinePartPointer()->startOdd())/sys.general_set.StruMapScale.fRealToDraw;	//因为角钢轴线两端缺省就多划30mm
				if(pDimPart.LinePartPointer()->endOdd()>=0)
					end=end-vec*90/sys.general_set.StruMapScale.fRealToDraw;		//再加上两个角钢端距和正负头
				else
					end=end-vec*(90-pDimPart.LinePartPointer()->endOdd())/sys.general_set.StruMapScale.fRealToDraw;		//再加上两个角钢端距和正负头
				UCS_STRU ucs=pDimPart->ucs;
				long start_ls_d,end_ls_d;
				double min_ls_z,max_ls_z;
				CLsRef *pLsRef=NULL;
				pLsRef=pDimPart.LinePartPointer()->GetFirstLsRef();
				if(pLsRef)
				{
					f3dPoint ls_pos=(*pLsRef)->ucs.origin;
					start_ls_d=end_ls_d=(*pLsRef)->get_d();
					coord_trans(ls_pos,ucs,FALSE);
					min_ls_z=max_ls_z=ls_pos.z;
				}
				else
				{
					start_ls_d=end_ls_d=20;
					min_ls_z=max_ls_z=0;
				}
				for(pLsRef=pDimPart.LinePartPointer()->GetFirstLsRef();pLsRef;
					pLsRef=pDimPart.LinePartPointer()->GetNextLsRef())
				{
					f3dPoint pos((*pLsRef)->ucs.origin);
					coord_trans(pos,ucs,FALSE);
					if(pos.z<min_ls_z)
					{
						start_ls_d=(*pLsRef)->get_d();
						min_ls_z=pos.z;
					}
					if(pos.z>max_ls_z)
					{
						end_ls_d=(*pLsRef)->get_d();
						max_ls_z=pos.z;
					}
				}
				LSSPACE_STRU LsSpace;
				f3dPoint dimPos,rgn_vert[4];
				double height=sys.dim_map.fJgOddmentTextSize;
				double length;
				//标注起始端正负头
				double wide=0;
				GetLsSpace(LsSpace,start_ls_d);
				int std_start_odd=LsSpace.EndSpace;
				if(pDimPart->GetClassTypeId()==CLS_LINEANGLE)
					wide=pDimPart.LineAnglePointer()->GetWidth();
				else if(pDimPart->GetClassTypeId()==CLS_LINETUBE)
				{
					wide=pDimPart.LineTubePointer()->GetDiameter();
					std_start_odd=0;
				}
				double start_odd=pDimPart.LinePartPointer()->startOdd();
				if(pDimPart->IsAngle())	//角钢需要考虑固定正负头 wht 11-04-28
				{
					if(pDimPart.LineAnglePointer()->m_cPosCtrlDatumLineType=='X')
						start_odd+=pDimPart.LineAnglePointer()->GetStartExtraOdd(0);
					else if(pDimPart.LineAnglePointer()->m_cPosCtrlDatumLineType=='Y')
						start_odd+=pDimPart.LineAnglePointer()->GetStartExtraOdd(1);
					else
						start_odd+=pDimPart.LineAnglePointer()->GetStartExtraOdd(2);
				}
				if(ftoi(start_odd)!=std_start_odd&&ftoi(start_odd)!=0)
				{
					if(fabs(pDimPart.LinePartPointer()->startOdd())>=100)
						length=3*height*0.7;
					else
						length=1.5*height;
					if(pDimPart.LinePartPointer()->startOdd()<0)
						length+=2.25;

					int quad_order[4]={0};
					if(angle>Pi/2.0&&angle<=1.5*Pi)
					{
						start=GetPtInPolar(start,angle-Pi/2,0.5*wide/sys.general_set.StruMapScale.fRealToDraw);
						rgn_vert[0]=GetPtInPolar(start,angle+Pi/2,height/2+0.3);
						rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
						rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
						rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
						quad_order[0]=1;quad_order[1]=4;
					}
					else
					{
						start=GetPtInPolar(start,angle+Pi/2,0.5*wide/sys.general_set.StruMapScale.fRealToDraw);
						rgn_vert[0]=GetPtInPolar(start,angle-Pi/2,height/2+0.3);
						rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle,length);
						rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
						rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
						quad_order[0]=1;quad_order[1]=4;
					}
					char sOdd[8];
					sprintf(sOdd,"%+d",ftoi(start_odd));
					if(SearchNoneIntersPos(&start,rgn_vert,NULL,quad_order)!=0)
					{
						DRAGSET.ClearEntSet();
						if(angle>Pi/2.0&&angle<=1.5*Pi)
							DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
						else
							DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
						f3dPoint box_vertex[4];
						ads_point L_T,R_B;
						SCOPE_STRU scope;
						box_vertex[0]=GetPtInPolar(start,angle-Pi/2,20);
						box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
						box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
						box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
						for(int jj=0;jj<4;jj++)
							scope.VerifyVertex(box_vertex[jj]);
						L_T[X]=scope.fMinX;
						L_T[Y]=scope.fMaxY;
						L_T[Z]=0;
						R_B[X]=scope.fMaxX;
						R_B[Y]=scope.fMinY;
						R_B[Z]=0;
					#ifdef _ARX_2007
						acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
					#else
						acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
					#endif
#ifdef AFX_TARG_ENU_ENGLISH
						DragEntSet(start,"\n Please select a right dimension place:\n");
#else
						DragEntSet(start,"\n请选择合适的标注位置:\n");
#endif
						//必须恢复原有的视图状态，不然acedSSGet函数调用失败
					#ifdef _ARX_2007
						acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
					#else
						acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
					#endif
					}
					else
					{
						if(angle>Pi/2.0&&angle<=1.5*Pi)
							DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
						else
							DimText(pBlockTableRecord,start,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
					}
				}
				
				//标注终止端正负头
				GetLsSpace(LsSpace,end_ls_d);
				int std_end_odd=LsSpace.EndSpace;
				if(pDimPart->GetClassTypeId()==CLS_LINETUBE)
					std_end_odd=0;
				double end_odd=pDimPart.LinePartPointer()->endOdd();
				if(pDimPart->IsAngle())	//角钢需要考虑固定正负头 wht 11-04-28
				{
					if(pDimPart.LineAnglePointer()->m_cPosCtrlDatumLineType=='X')
						end_odd+=pDimPart.LineAnglePointer()->GetEndExtraOdd(0);
					else if(pDimPart.LineAnglePointer()->m_cPosCtrlDatumLineType=='Y')
						end_odd+=pDimPart.LineAnglePointer()->GetEndExtraOdd(1);
					else
						end_odd+=pDimPart.LineAnglePointer()->GetEndExtraOdd(2);
				}
				if(ftoi(end_odd)!=std_end_odd&&fabs(end_odd)!=0)
				{
					if(fabs(pDimPart.LinePartPointer()->endOdd())>=100)
						length=3*height*0.7;
					else
						length=1.5*height;
					if(pDimPart.LinePartPointer()->endOdd()<0)
						length+=2.25;

					int quad_order[4]={0};
					if(angle>Pi/2.0&&angle<=1.5*Pi)
					{
						end=GetPtInPolar(end,angle-Pi/2,0.5*wide/sys.general_set.StruMapScale.fRealToDraw);
						rgn_vert[0]=GetPtInPolar(end,angle+Pi/2,height/2+0.3);
						rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
						rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle-Pi/2,height+0.6);
						rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle-Pi/2,height+0.6);
						quad_order[0]=1;quad_order[1]=4;
					}
					else
					{
						end=GetPtInPolar(end,angle+Pi/2,0.5*wide/sys.general_set.StruMapScale.fRealToDraw);
						rgn_vert[0]=GetPtInPolar(end,angle-Pi/2,height/2+0.3);
						rgn_vert[1]=GetPtInPolar(rgn_vert[0],angle-Pi,length);
						rgn_vert[2]=GetPtInPolar(rgn_vert[1],angle+Pi/2,height+0.6);
						rgn_vert[3]=GetPtInPolar(rgn_vert[0],angle+Pi/2,height+0.6);
						quad_order[0]=1;quad_order[1]=4;
					}
					ads_name ss_inters;
					char sOdd[8];
					sprintf(sOdd,"%+d",ftoi(end_odd));
					if(SearchNoneIntersPos(&end,rgn_vert,NULL,quad_order)!=0)
					{
						DRAGSET.ClearEntSet();
						if(angle>Pi/2.0&&angle<=1.5*Pi)
							DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
						else
							DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
						f3dPoint box_vertex[4];
						ads_point L_T,R_B;
						SCOPE_STRU scope;
						box_vertex[0]=GetPtInPolar(end,angle-Pi/2,20);
						box_vertex[1]=GetPtInPolar(box_vertex[0],angle,20);
						box_vertex[2]=GetPtInPolar(box_vertex[1],angle+Pi/2,40);
						box_vertex[3]=GetPtInPolar(box_vertex[0],angle+Pi/2,40);
						for(int jj=0;jj<4;jj++)
							scope.VerifyVertex(box_vertex[jj]);
						L_T[X]=scope.fMinX;
						L_T[Y]=scope.fMaxY;
						L_T[Z]=0;
						R_B[X]=scope.fMaxX;
						R_B[Y]=scope.fMinY;
						R_B[Z]=0;
					#ifdef _ARX_2007
						acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
					#else
						acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
					#endif
#ifdef AFX_TARG_ENU_ENGLISH
						DragEntSet(end,"\n Please select a right dimension place:\n");
#else
						DragEntSet(end,"\n请选择合适的标注位置:\n");
#endif
						//必须恢复原有的视图状态，不然acedSSGet函数调用失败
					#ifdef _ARX_2007
						acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
					#else
						acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
					#endif
					}
					else
					{
						if(angle>Pi/2.0&&angle<=1.5*Pi)
							DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextLeft,AcDb::kTextVertMid);
						else
							DimText(pBlockTableRecord,end,sOdd,TextStyleTable::hzfs.textStyleId,height,dim_angle,AcDb::kTextRight,AcDb::kTextVertMid);
					}
					acedSSFree(ss_inters);
				}
			}
			else
			{
				pEnt->close();
				continue;
			}
			pEnt->close();
		}
		pBlockTableRecord->close();
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);
}
#else
{;}
#endif