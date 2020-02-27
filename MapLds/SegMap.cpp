//<LOCALE_TRANSLATE/>
//结构图
#include "stdafx.h"
#include "stdarx.h"
#include "MenuFunc.h"
#include "SelMapSegDlg.h"
#include "Tower.h"
#include "GlobalFunc.h"
#include "DrawTaMap.h"
#include "InputDrawingUcsDlg.h"
#include "adstools.h"
#include "RxTools.h"
#include "geassign.h"
#include "DragEntSet.h"
#include "SelCurFaceFlagDlg.h"
#include "FootViewSelDlg.h"
#include "SysPara.h"
#include "DrawUnit.h"
#include "SelCurFaceDlg.h"
#include "SelPartGroupDlg.h"
#include "DrawingVectorDlg.h"
#include "PartGroupDrawing.h"
#include "ScrTools.h"
#include "PartDrawingArraySpaceDlg.h"
#include "dbxrecrd.h"
#include "DrawUnit.h"
#include "SortFunc.h"
#include "ComparePartNoString.h"
#include "NewMapFrameDlg.h"
#include "LogFile.h"
#include "JgDrawing.h"
#include "ClientId.h"
#include ".\LdsDrwLocale.h"

UCS_STRU seg_map_base_face_ucs;	//分段图基本面（第一面绘制相对坐标系）
#ifndef __TSA_FILE_
//
//From PaperPatternDrawing.cpp
AcDbObjectId NewBoltCircle(AcDbBlockTableRecord *pBlockTableRecord,f3dCircle Cir,BOOL bFootNail=FALSE);
//from ProcessCard.cpp
bool GetProcessCardRegion(const char* szCardFilePathName,f2dRect* prcCard);
f2dRect GetProcessCardDrawingRect(int iProcessCardType=0);
f2dRect InsertPartGroupProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,
									  PART_GROUP_CARD_INFO *pPartGroupInfo,BOOL bPrompt = FALSE);
//x_scale,y_scale为X坐标项、Y坐标项的放大比例
static void trans_pos(double* ptcoords,f3dPoint origin,double zoomScaleCoef=1)
{
	GEPOINT pt(ptcoords);
	pt.x*=zoomScaleCoef;
	pt.y*=zoomScaleCoef;
	pt+=origin;
	ptcoords[0]=pt.x;
	ptcoords[1]=pt.y;
}
static void trans_pos(f3dLine &line,f3dPoint origin,double zoomScaleCoef=1)
{
	line.startPt = line.startPt;
	line.startPt.x*=zoomScaleCoef;
	line.startPt.y*=zoomScaleCoef;
	Add_Pnt(line.startPt,line.startPt,origin);
	line.endPt = line.endPt;
	line.endPt.x*=zoomScaleCoef;
	line.endPt.y*=zoomScaleCoef;
	Add_Pnt(line.endPt,line.endPt,origin);
}
static void trans_pos(f3dArcLine *pLine,f3dPoint origin,double zoomScaleCoef=1)
{
	if(pLine==NULL)
		return;
	pLine->ZoomScale(zoomScaleCoef);
	pLine->Offset(origin);
}

void SegStru()
{
	sys.m_iCurMapType = 3;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CAcModuleResourceOverride resOverride;
	CSelCurFaceDlg facedlg;
	if(facedlg.DoModal()!=IDOK || facedlg.m_pSelectedView==NULL)
		return;
	CLogErrorLife logErrLife;
	CDisplayView *pCurView = facedlg.m_pSelectedView;
	CLDSNode *pNode=NULL;
	CLDSPart *pPart=NULL;
	SCOPE_STRU scope;
	PARTSET partset;
	NODESET nodeset;
	if(pCurView->m_iViewType==0)
	{	//图层视图
		CHashList<SEGI> hashIncSegs;	//当前绘图所包括的分段集合 wjh-2017.11.15
		for(SEGI* pSegI=facedlg.m_xSegNoList.GetFirst();pSegI;pSegI=facedlg.m_xSegNoList.GetNext())
			hashIncSegs.SetValue(*pSegI,*pSegI);
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pCurView->filter.IsValidLayer(pNode->layer())&&hashIncSegs.GetValue(pNode->iSeg.iSeg)!=NULL)//==Ta.m_iCurSeg.iSeg)
				nodeset.append(pNode);
		}
		for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			pPart->feature=0;
			if(pCurView->filter.IsValidLayer(pPart->layer())&&hashIncSegs.GetValue(pPart->iSeg.iSeg)!=NULL)//==Ta.m_iCurSeg.iSeg)
				partset.append((CLDSLinePart*)pPart);
		}
		//添加因图层名不匹配遗漏的螺栓
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			CLsRef *pLsRef=NULL;
			if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				continue;
			BOOL bPush=partset.push_stack();
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
			if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pLineAngle;
				for(int i=0;i<4;i++)
				{
					if(pGroupLineAngle->group_style<2&&i>=2)
						continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在 wht 11-08-05
					if(pGroupLineAngle->son_jg_h[i]<0x20)
						continue;
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)Ta.FromPartHandle(pGroupLineAngle->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg==NULL)
						continue;
					for(pLsRef=pSonJg->GetFirstLsRef();pLsRef;pLsRef=pSonJg->GetNextLsRef())
						partset.append(pLsRef->GetLsPtr());
				}
			}
			else 
			{
				for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
					partset.append(pLsRef->GetLsPtr());
			}
			if(bPush)
				partset.pop_stack();
		}
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			BOOL bPush=partset.push_stack();
			for(CLDSPart *pAfterPart=partset.GetNext();pAfterPart;pAfterPart=partset.GetNext())
			{
				if(pPart==pAfterPart)	//删除因上面的补漏操作导致的重复构件问题
					partset.DeleteCursor();
			}
			if(bPush)
				partset.pop_stack();
		}
	}
	else if(pCurView->m_iViewType==1)
	{	//定制视图
		for(CViewPart *pViewPart=pCurView->GetFirstViewPart(); pViewPart; pViewPart=pCurView->GetNextViewPart())
		{
			CLDSNode *pNode = Ta.FromNodeHandle(pViewPart->handle);
			if(pNode)
				scope.VerifyVertex(pNode->Position(false));
			pPart = Ta.FromPartHandle(pViewPart->handle);
			if(pPart)
			{
				pViewPart->pDisplayView=pCurView;
				pPart->feature=(long)pViewPart;	//保存相关的CViewPart指定 wht 10-12-10
				partset.append(pPart);
			}
		}
	}
	if(partset.GetNodeNum()<1&&nodeset.GetNodeNum()<1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("%s SegI There are no enough legal rods or nodes, Please check whether to open the right data file!",facedlg.m_sSegNo);
#else
		logerr.Log("%s段中没有选到足够的合法杆件或节点,请检查是否打开了正确的数据文件!",facedlg.m_sSegNo);
#endif
		return;
	}
	//
	GetNodeSetSizeScope(nodeset,scope);
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart->IsLinePart())
		{
			scope.VerifyVertex(((CLDSLinePart*)pPart)->Start());
			scope.VerifyVertex(((CLDSLinePart*)pPart)->End());
		}
		else
			scope.VerifyVertex(pPart->ucs.origin);
	}
	f3dPoint f_base;
	f_base.x=scope.fMinX+(scope.fMaxX-scope.fMinX)/2.0;
	f_base.y=scope.fMinY+(scope.fMaxY-scope.fMinY)/2.0;
	f_base.z=scope.fMinZ+(scope.fMaxZ-scope.fMinZ)/2.0;
	//计算绘图坐标系
	UCS_STRU ucs=CalDrawingUcs(partset,pCurView,FALSE);
	CInputDrawingUcsDlg dlg;
	dlg.m_fAxisX_X = ucs.axis_x.x;
	dlg.m_fAxisX_Y = ucs.axis_x.y;
	dlg.m_fAxisX_Z = ucs.axis_x.z;
	dlg.m_fAxisZ_X = ucs.axis_z.x;
	dlg.m_fAxisZ_Y = ucs.axis_z.y;
	dlg.m_fAxisZ_Z = ucs.axis_z.z;
	dlg.m_fOriginPosX=ucs.origin.x;
	dlg.m_fOriginPosY=ucs.origin.y;
	dlg.m_fOriginPosZ=ucs.origin.z;
	if(dlg.DoModal()==IDOK)
	{
		ucs.origin.x=dlg.m_fOriginPosX;
		ucs.origin.y=dlg.m_fOriginPosY;
		ucs.origin.z=dlg.m_fOriginPosZ;
		ucs.axis_x.Set(dlg.m_fAxisX_X,dlg.m_fAxisX_Y,dlg.m_fAxisX_Z);
		ucs.axis_z.Set(dlg.m_fAxisZ_X,dlg.m_fAxisZ_Y,dlg.m_fAxisZ_Z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
	}
	else
		return;

	//不绘制侧面显示的法兰上的螺栓 wht 10-03-04
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(sys.stru_map.bOnlyDrawHorizontalPlate)
			{	//仅绘制当前视图中水平显示的钢板 wht 10-06-15
				if(fabs(ucs.axis_z*pPlate->ucs.axis_z)<0.707&&
					fabs(pPlate->ucs.axis_z*f3dPoint(0,0,1))<EPS_COS2)	//塔脚板或牛蹄板都应该绘制
				{
					CViewPart *pViewPart=(CViewPart*)pPlate->feature;
					if(pViewPart&&pViewPart->cFlag=='S')
						continue;	//不需要移除展开绘制的钢板 wht 10-12-11
					partset.DeleteNode(pPlate->handle);
					continue; //竖直绘制
				}
			}
			if(fabs(pPlate->m_fInnerRadius)<EPS2)
				continue;
			CLsRef *pLsRef=NULL;
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				partset.DeleteNode(pLsRef->GetLsPtr()->handle);
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(!pParamPlate->IsFL()
				&&pParamPlate->m_iParamType!=TYPE_WATERSHIELDEDPLATE)	//遮水板
				continue;
			if(fabs(ucs.axis_z*pParamPlate->ucs.axis_z)>0.707)
				continue; //水平绘制
			CLsRef *pLsRef=NULL;
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				partset.DeleteNode(pLsRef->GetLsPtr()->handle);
		}
	}
	for(CLDSNode *pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		pNode->feature=0;
	//清空构件绘制面标志 清空标注位后将不支持用户指定的绘制面 wht 10-11-19
	//for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	//	pPart->feature=0;
	/*截面位置应由视图绘制坐标系原点定位，即使使用第一个构件，也要考虑角钢构件初始ucs.origin为空，应取始端点坐标　wjh-2011.7.2
	CLDSPart *pFirstPart=partset.GetFirst();
	if(pFirstPart)
	ucs.origin=pFirstPart->ucs.origin;*/
	DRAGSET.ClearEntSet();
	//角钢和钢板轮廓边类型可为多段线或普通直线 wht 10-06-15
	DrawStruDrawing(partset,nodeset,ucs,0,sys.general_set.StruMapScale.fRealToDraw,FALSE);
	coord_trans(f_base,ucs,FALSE);
	f_base = f_base*(1.0/sys.general_set.StruMapScale.fRealToDraw);
	ads_point basepnt;
	basepnt[X] = f_base.x;
	basepnt[Y] = f_base.y;
	basepnt[Z] = f_base.z;
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point");
#else
	DragEntSet(basepnt,"输入插入点");
#endif
	seg_map_base_face_ucs = ucs;
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}

void PartGroupStru()
{
	/*UCS_STRU ucs;
	SCOPE_STRU scope;
	f3dPoint f_base;
	ads_point basepnt;
	double scale = sys.general_set.StruMapScale;
	CLDSPart *pPart=NULL;
	NODESET nodeset;
	PARTSET partset;
    CAcModuleResourceOverride resOverride;
	CSelPartGroupDlg partgroupdlg;
	if(Ta.PartGroup.GetNodeNum()==0||partgroupdlg.DoModal()!=IDOK)
		return;

	CLDSPartGroup *pPartGroup=NULL;
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		if(pPartGroup->handle==partgroupdlg.m_hSelPartGroup)
			break;
	}
	CDrawingVectorDlg dlg;
	if(partgroupdlg.m_pSelSectionView>0)
	{
		CSectionView *pView=partgroupdlg.m_pSelSectionView;
		for(CSectionView::PART_ITEM *pItem=pView->SonParts.GetFirst();pItem;pItem=pView->SonParts.GetNext())
		{
			CLDSPart *pPart=Ta.FromPartHandle(pItem->hSonPart);
			if(pPart)
			{
				pPart->feature=pItem->cFlag;
				partset.append(pPart);
			}
		}
		dlg.axis_x=pView->ucs.axis_x;
		dlg.axis_z=pView->ucs.axis_z;
	}
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		CSuperSmartPtr<CLDSPart>pSuperPart=pPart;
		if(pSuperPart->IsLinePart())
		{
			scope.VerifyVertex(pSuperPart.LinePartPointer()->Start());
			scope.VerifyVertex(pSuperPart.LinePartPointer()->End());
		}
		else
			scope.VerifyVertex(pSuperPart->ucs.origin);
	}
	f_base.x=scope.fMinX+(scope.fMaxX-scope.fMinX)/2.0;
	f_base.y=scope.fMinY+(scope.fMaxY-scope.fMinY)/2.0;
	f_base.z=scope.fMinZ+(scope.fMaxZ-scope.fMinZ)/2.0;
	if(partset.GetNodeNum()<1)
	{
		AfxMessageBox("没有选到足够的合法杆件或节点,请检查是否打开了正确的数据文件!");
		return;
	}
	if(dlg.DoModal()==IDOK)
	{
		ucs.axis_x=dlg.axis_x;
		ucs.axis_z=dlg.axis_z;
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
	}
	else
		return;
	ucs.origin=f_base;
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		pPart->feature=0;
	DRAGSET.ClearEntSet();
	DrawStruDrawing(partset,nodeset,ucs,0,scale,FALSE);
	coord_trans(f_base,ucs,FALSE);
	f_base = f_base*(1.0/scale);
	basepnt[X] = f_base.x;
	basepnt[Y] = f_base.y;
	basepnt[Z] = f_base.z;
	DragEntSet(basepnt,"输入插入点");
	seg_map_base_face_ucs = ucs;
	ads_command(RTSTR,"RE",RTNONE);*/
}

//From DrawMapTool.cpp 
//<DEVELOP_PROCESS_MARK nameId="SegMap::DrawSectionViewMap">
BOOL DrawJgMap(CRodDrawing *pJgDraw,f3dPoint origin,BOOL bDimStyle2=FALSE,AcDbBlockTableRecord *pExterBlockTableRecord=NULL);
void DrawKaoMoAngle(AcDbBlockTableRecord *pExterBlockTableRecord,f3dPoint top_left,double draw_scale,CKaoMeAngle &kmAngle);
BOOL DrawSectionViewMap(AcDbBlockTableRecord *pExterBlockTableRecord,CSectionViewDrawing *pSectionViewDraw,
					  f3dPoint base_pnt,double zoomdraw_coef=1.0)//,BOOL bDimStyle2=FALSE)
{
	CSectionView *pView=pSectionViewDraw->GetSectionView();
	if(pSectionViewDraw->GetComponentType()==CSectionViewDrawing::CLS_KM_ANGLE_VIEW)
	{	//绘制靠模角钢视图 wht 15-09-09
		CSectionKmAngleViewDrawing *pKmAngleViewDrawing=(CSectionKmAngleViewDrawing*)pSectionViewDraw;
		double km_angle_draw_scale=(1.0/pKmAngleViewDrawing->GetInternalDrawingScale())*zoomdraw_coef;	//绘制比例要考虑组焊件内部绘制比例及布局缩放比例
		DrawKaoMoAngle(pExterBlockTableRecord,base_pnt,km_angle_draw_scale,pKmAngleViewDrawing->m_xKaoMeAngle);
		return TRUE;
	}
	else if(pView&&pView->GetViewTypeId()==CSectionView::CLS_ANGLE_SPREAD_VIEW)
	{
		CSectionAngleViewDrawing* pAngleViewDrawing = (CSectionAngleViewDrawing*)pSectionViewDraw;
		CLDSPartGroup *pPartGroup=pView->BelongPartGroup();
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPartGroup->BelongModel()->FromPartHandle(pPartGroup->handle);
		BOOL bDimRibPlatePartNo=sys.part_map.angle.bDimRibPlatePartNo;
		BOOL bDimRibPlateSetUpPos=sys.part_map.angle.bDimRibPlateSetUpPos;
		sys.part_map.angle.bDimRibPlatePartNo=TRUE;
		sys.part_map.angle.bDimRibPlateSetUpPos=TRUE;
		//
		DRAGSET.BeginBlockRecord();
		DrawJgMap(&pAngleViewDrawing->m_xAngleDrawing, f3dPoint(0, 0, 0), FALSE, DRAGSET.RecordingBlockTableRecord());
		CXhChar100 sKmLabel("%s,L%.fx%.fx%.f",(char*)pLineAngle->GetPartNo(),
			pLineAngle->GetWidth(),pLineAngle->GetThick(),pLineAngle->GetLength());
		f2dRect rect=pAngleViewDrawing->m_xAngleDrawing.GetDrawRegion(1.0);
		f3dPoint dimPos(0.5*rect.Width(), rect.bottomRight.y - 3 * sys.fonts.weld.fPartNoTextSize);
		DimText(DRAGSET.RecordingBlockTableRecord(), dimPos, sKmLabel, TextStyleTable::hzfs.textStyleId,
			sys.fonts.weld.fPartNoTextSize, 0, AcDb::kTextMid, AcDb::kTextTop);
		DRAGSET.EndBlockRecord(pExterBlockTableRecord, GEPOINT(base_pnt.x, base_pnt.y));
		//
		sys.part_map.angle.bDimRibPlatePartNo=bDimRibPlatePartNo;
		sys.part_map.angle.bDimRibPlateSetUpPos=bDimRibPlateSetUpPos;
		return TRUE;
	}
	//创建组焊视图块记录
	DRAGSET.BeginBlockRecord();
	AcDbBlockTableRecord *pBlockTableRecord=DRAGSET.RecordingBlockTableRecord();//定义块表记录指针
	f3dPoint blk_insert_pos=base_pnt;
	base_pnt.Set();
	//对视图进行缩放
	if(fabs(zoomdraw_coef-1)>0.1)
		pSectionViewDraw->ZoomDrawing(zoomdraw_coef);
	//统一在开始位置标注尺寸字高，IDbEntity模式与原有模式都要使用 wht 16-10-18
	//初始化尺寸标注
	double glbalWeldDrawingFontZoomCoef=pSectionViewDraw->ZoomToModelTextSize(1.0);
	double fDimTextSize		=max(sys.part_group_map.fMinFontSize,sys.fonts.weld.fDimTextSize*zoomdraw_coef*glbalWeldDrawingFontZoomCoef);
	double fDimAngleTextSize=max(sys.part_group_map.fMinFontSize,sys.fonts.weld.fDimAngleTextSize*zoomdraw_coef*glbalWeldDrawingFontZoomCoef);
	double fDimAngleArrowSize=sys.dim_map.fDimArrowSize*zoomdraw_coef*glbalWeldDrawingFontZoomCoef;

	DimStyleTable::dimStyle.InitDimStyle(fDimTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	DimStyleTable::dimStyle4.InitDimStyle(fDimTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	//半径标注样式
	DimStyleTable::dimRadiusStyle.InitDimStyle(fDimTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	//初始化角度标注
	if(!sys.general_set.bProcessCardTbl)	
	{	//1:1组焊图 放大字体高度 wht 10-09-14
		double fTextHeight=sys.fonts.weld.fDimAngleTextSize;
		DimStyleTable::dimAngleStyle.InitDimStyle(fTextHeight,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	}
	else
	{
		if(zoomdraw_coef>=1)
			DimStyleTable::dimAngleStyle.InitDimStyle(sys.fonts.weld.fDimAngleTextSize,sys.dim_map.fDimArrowSize);
		else
		{	//保证字体最小高度为 fMinFontSize  wht 10-05-31
			//double fTextSize=sys.dim_map.fDimAngleTextSize*scale;
			double fTextSize=max(sys.part_group_map.fMinFontSize,sys.fonts.weld.fDimAngleTextSize*zoomdraw_coef);
			DimStyleTable::dimAngleStyle.InitDimStyle(fTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
		}
	}
	//字体与螺栓大小都时在1:20时显示的尺寸,当生成1:1组焊图时需要放大字体高度,螺栓直径 wht 10-09-06
	double fInternalZoomScale=zoomdraw_coef*sys.general_set.WeldMapScale.fBoltSymbolFactor/pSectionViewDraw->GetInternalDrawingScale();
	//绘制CSectionViewDrawing下IDbEntity绘制
	for(IDbEntity* pEntity=pSectionViewDraw->EnumFirstDbEntity();pEntity;pEntity=pSectionViewDraw->EnumNextDbEntity())
	{
	//提取几何图元所附加的Id标识数据
		long hPart=0;
		ILifeDataObject* pAttachData=pEntity->GetLifeDataObject();
		if(pAttachData&&pAttachData->GetType()==ILifeDataObject::INTEGER)
			hPart=((IAttachIntegerData*)pAttachData)->GetIntPara();
		
		PEN_STRU pen = pEntity->GetPen();
		if(pen.style==PS_SOLID)			//图层0
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else if(pen.style==PS_DASH)		//图层1
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		else if(pen.style==PS_DOT)		//图层2 
			GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
		else if(pen.style==PS_DASHDOT)	//图层3
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		else if(pen.style==PS_DASHDOTDOT)//图层4
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		else if(pen.style==PS_NULL)		//图层5
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
		else if(pen.style==PS_INSIDEFRAME)//图层6
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		if(pEntity->Layer().GetLength()>0)
			SetCurLayer(pEntity->Layer());
		if(pEntity->GetDbEntType()==IDbEntity::DbPoint)				//点
		{
			IDbPoint* pPoint=(IDbPoint*)pEntity;
			f3dPoint point=pPoint->Position();
			trans_pos(point,base_pnt);
			AcDbPoint *pAcDbPoint=new AcDbPoint(AcGePoint3d(point.x,point.y,point.z));
			AcDbObjectId pointId;
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pAcDbPoint))
				pAcDbPoint->close();
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbLine)			//线段
		{
			IDbLine* pLine=(IDbLine*)pEntity;
			f3dPoint startPt(pLine->StartPosition());
			f3dPoint endPt(pLine->EndPosition());
			trans_pos(startPt,base_pnt);
			trans_pos(endPt,base_pnt);
			CreateAcadLine(pBlockTableRecord,startPt,endPt,hPart);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbArcline)		//常规曲线
		{
			IDbArcline* pArcLine=(IDbArcline*)pEntity;
			pArcLine->Offset(base_pnt);
			if(pArcLine->ColumnNorm()==pArcLine->WorkNorm())
			{
				if(fabs(pArcLine->SectorAngle()-2*Pi)<EPS)
					CreateAcadCircle(pBlockTableRecord,pArcLine->Center(),pArcLine->Radius(),hPart);	
				else
					CreateAcadArcLine(pBlockTableRecord,pArcLine->Center(),pArcLine->StartPosition(),pArcLine->SectorAngle(),pArcLine->WorkNorm(),hPart);
			}
			else 
			{
				f3dArcLine arcline=pArcLine->GetArcLine();
				CreateAcadEllipseLine(pBlockTableRecord,arcline);
			}
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbCircle)		//圆
		{
			IDbCircle* pDbCircle=(IDbCircle*)pEntity;
			pDbCircle->Offset(base_pnt);
			CreateAcadCircle(pBlockTableRecord,pDbCircle->Center(),pDbCircle->Radius(),hPart);	
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbPolyline)		//多线
		{
			IDbPolyline* pPolyLine=(IDbPolyline*)pEntity;
			int nLineNum=pPolyLine->numVerts();
			AcDbPolyline *pAcDbPolyLine=new AcDbPolyline(nLineNum);//创建LINE对象
			double startWidth=0,endWidth=0;
			GEPOINT vertice;
			for(int i=0;i<nLineNum;i++)
			{
				pPolyLine->GetVertexAt(i,vertice);
				pPolyLine->GetWidthsAt(i,startWidth,endWidth);
				f3dPoint point(vertice);
				trans_pos(point,base_pnt);
				pAcDbPolyLine->addVertexAt(i,AcGePoint2d(point.x,point.y),0,startWidth,endWidth);
			}
			AcDbObjectId lineId;
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,lineId,pAcDbPolyLine))
				pAcDbPolyLine->close();
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbSpline)		//样条曲线
		{
			IDbSpline* pSpline=(IDbSpline*)pEntity;
			pSpline->Offset(base_pnt);
			GEPOINT fitPt;
			AcGePoint3dArray fitPoints;
			for(int i=0;i<pSpline->GetNumFitPoints();i++)
			{
				pSpline->GetFitPointAt(i,fitPt);
				fitPoints.append(AcGePoint3d(fitPt.x,fitPt.y,fitPt.z));
			}
			AcDbObjectId splineId;
			AcDbSpline *pAcDbSpline=new AcDbSpline(fitPoints,4,0);				//创建SPline样条曲线对象
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,splineId,pAcDbSpline))
				pAcDbSpline->close();
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbText)
		{
			IDbText* pDbText=(IDbText*)pEntity;
			AcDbObjectId textStyleId;
			textStyleId.setFromOldId(pDbText->GetTextStyleId());
			//2017.8.26之后的版本IDbMText::ZoomScale()中已考虑字高乘缩放系数 wjh-2017.8.26
			double fTextSize=max(sys.part_group_map.fMinFontSize,pDbText->GetHeight());//*zoomdraw_coef);
			if(pDbText->GetAlignment()==IDbText::AlignTopLeft)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextLeft,AcDb::kTextTop);
			else if(pDbText->GetAlignment()==IDbText::AlignTopCenter)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextCenter,AcDb::kTextTop);
			else if(pDbText->GetAlignment()==IDbText::AlignTopRight)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextRight,AcDb::kTextTop);
			else if(pDbText->GetAlignment()==IDbText::AlignMiddleLeft)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextLeft,AcDb::kTextVertMid);
			else if(pDbText->GetAlignment()==IDbText::AlignMiddleCenter)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextCenter,AcDb::kTextVertMid);
			else if(pDbText->GetAlignment()==IDbText::AlignMiddleRight)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextRight,AcDb::kTextVertMid);
			else if(pDbText->GetAlignment()==IDbText::AlignBottomLeft)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextLeft,AcDb::kTextBottom);
			else if(pDbText->GetAlignment()==IDbText::AlignBottomCenter)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextCenter,AcDb::kTextBottom);
			else if(pDbText->GetAlignment()==IDbText::AlignBottomRight)
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
					pDbText->GetRotation(),AcDb::kTextRight,AcDb::kTextBottom);
			else
				DimText(pBlockTableRecord,pDbText->GetPosition(),pDbText->GetTextString(),textStyleId,fTextSize,
						pDbText->GetRotation());
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbMText)
		{
			IDbMText* pDbMText=(IDbMText*)pEntity;
			AcDbObjectId textStyleId;
			textStyleId.setFromOldId(pDbMText->GetTextStyleId());
			AcDbMText::AttachmentPoint align_type;
			AcDbMText::FlowDirection flowDirection;
			if(pDbMText->GetAlignment()==IDbText::AlignTopLeft)
				align_type=AcDbMText::kTopLeft;
			else if(pDbMText->GetAlignment()==IDbText::AlignTopCenter)
				align_type=AcDbMText::kTopCenter;
			else if(pDbMText->GetAlignment()==IDbText::AlignTopRight)
				align_type=AcDbMText::kTopRight;
			else if(pDbMText->GetAlignment()==IDbText::AlignMiddleLeft)
				align_type=AcDbMText::kMiddleLeft;
			else if(pDbMText->GetAlignment()==IDbText::AlignMiddleCenter)
				align_type=AcDbMText::kMiddleCenter;
			else if(pDbMText->GetAlignment()==IDbText::AlignMiddleRight)
				align_type=AcDbMText::kMiddleRight;
			else if(pDbMText->GetAlignment()==IDbText::AlignBottomLeft)
				align_type=AcDbMText::kBottomLeft;
			else if(pDbMText->GetAlignment()==IDbText::AlignBottomCenter)
				align_type=AcDbMText::kBottomCenter;
			else if(pDbMText->GetAlignment()==IDbText::AlignBottomRight)
				align_type=AcDbMText::kBottomRight;
			else
				align_type=AcDbMText::kBaseLeft;
			if(pDbMText->GetFlowDirection()==IDbMText::kLtoR)
				flowDirection=AcDbMText::kLtoR;
			else if(pDbMText->GetFlowDirection()==IDbMText::kTtoB)
				flowDirection=AcDbMText::kTtoB;
			else if(pDbMText->GetFlowDirection()==IDbMText::kByStyle)
				flowDirection=AcDbMText::kByStyle;
			//2017.8.26之后的版本IDbMText::ZoomScale()中已考虑字高乘缩放系数 wjh-2017.8.26
			double fTextSize=max(sys.part_group_map.fMinFontSize,pDbMText->GetHeight()); //*zoomdraw_coef); 
			DimMText(pBlockTableRecord,pDbMText->GetTextString(),pDbMText->GetPosition(),pDbMText->GetWidth(),fTextSize,textStyleId,
				align_type,flowDirection,pDbMText->GetRotation());
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbDiametricDimension)
		{
			IDbDiametricDimension* pDiaDimension=(IDbDiametricDimension*)pEntity;
			pDiaDimension->Offset(base_pnt);
			GEPOINT chordPoint,farChordPoint;
			pDiaDimension->GetChordPoint(chordPoint);
			pDiaDimension->GetFarChordPoint(farChordPoint);
			double leaderLen=pDiaDimension->GetLeaderLength();
			CXhChar50 dimText;
			dimText.Copy(pDiaDimension->GetDimText());
			DimDiameter(pBlockTableRecord,chordPoint,farChordPoint,leaderLen,dimText,DimStyleTable::dimRadiusStyle.dimStyleId,fDimTextSize);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbRadialDimension)	//半径标注
		{
			IDbRadialDimension* pRadialDim=(IDbRadialDimension*)pEntity;
			pRadialDim->Offset(base_pnt);
			GEPOINT centerPt,chordPt;
			pRadialDim->GetCenterPoint(centerPt);
			pRadialDim->GetChordPoint(chordPt);
			double leaderLen=pRadialDim->GetLeaderLength();
			CXhChar50 dimText;
			dimText.Copy(pRadialDim->GetDimText());
			DimRadial(pBlockTableRecord,centerPt,chordPt,leaderLen,dimText,DimStyleTable::dimRadiusStyle.dimStyleId,fDimTextSize);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbAngularDimension)	//角度标注
		{
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			IDbAngularDimension* pAngularDim=(IDbAngularDimension*)pEntity;
			pAngularDim->Offset(base_pnt);
			GEPOINT vertex,startPt,endPt,dimPos;
			pAngularDim->GetLine1Start(vertex);
			pAngularDim->GetLine1End(startPt);
			pAngularDim->GetLine2End(endPt);
			//TODO:IDbAngularDimension中的ArcPoint没有进行缩放和偏移，在此手动进行缩放
			pAngularDim->GetArcPoint(dimPos);
			double fArrowSize=pAngularDim->GetArrowSize();
			CXhChar50 dimText;
			dimText.Copy(pAngularDim->GetDimText());
			if(dimText.GetLength()>0)
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,vertex,startPt,endPt,dimPos,dimText,fDimAngleTextSize,fArrowSize);
			else
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,vertex,startPt,endPt,dimPos,NULL,fDimAngleTextSize,fArrowSize);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbAlignedDimension)	//对齐尺寸标注
		{
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			IDbAlignedDimension* pAlignedDim=(IDbAlignedDimension*)pEntity;
			pAlignedDim->Offset(base_pnt);
			GEPOINT startPt,endPt,dimPos;
			pAlignedDim->GetLine1Point(startPt);
			pAlignedDim->GetLine2Point(endPt);
			pAlignedDim->GetDimLinePoint(dimPos);
			CXhChar50 dimText;
			dimText.Copy(pAlignedDim->GetDimText());
			DimAlignedSize(pBlockTableRecord,DimStyleTable::dimStyle.dimStyleId,startPt,endPt,dimPos,dimText,fDimTextSize);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbRotatedDimension)	//旋转投影尺寸标注
		{
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			IDbRotatedDimension* pRotateDim=(IDbRotatedDimension*)pEntity;
			pRotateDim->Offset(base_pnt);
			GEPOINT startPt,endPt,dimPos;
			pRotateDim->GetLine1Point(startPt);
			pRotateDim->GetLine2Point(endPt);
			pRotateDim->GetDimLinePoint(dimPos);
			double angle=pRotateDim->GetRotationRadian();
			CXhChar50 dimText;
			dimText.Copy(pRotateDim->GetDimText());
			//if(pRotateDim->iTextDimSylte)
			DimAngleSize(pBlockTableRecord,startPt,endPt,dimPos,dimText,DimStyleTable::dimStyle.dimStyleId,angle,fDimTextSize);
		}
	}
	//绘制组焊图构件轮廓线
	for(f3dArcLine *pLine=pSectionViewDraw->GetEdgeLineList()->GetFirst();pLine;pLine=pSectionViewDraw->GetEdgeLineList()->GetNext())
	{
		if(pLine->pen.style==PS_SOLID)			//图层0
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else if(pLine->pen.style==PS_DASH)		//图层1
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		else if(pLine->pen.style==PS_DOT)		//图层2 
			GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
		else if(pLine->pen.style==PS_DASHDOT)	//图层3
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		else if(pLine->pen.style==PS_DASHDOTDOT)//图层4
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		else if(pLine->pen.style==PS_NULL)		//图层5
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
		else if(pLine->pen.style==PS_INSIDEFRAME)//图层6
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		f3dArcLine arcline=*pLine;
		if(pLine->pen.style==PS_USERSTYLE)//Spline样条曲线
		{
			f3dPoint start_pt,mid_pt,end_pt;
			start_pt=arcline.Start();
			mid_pt=arcline.Center();
			end_pt=arcline.End();
			trans_pos(start_pt,base_pnt);
			trans_pos(mid_pt,base_pnt);
			trans_pos(end_pt,base_pnt);
			CreateAcadSpline(pBlockTableRecord,start_pt,mid_pt,end_pt);
		}
		else if(fabs(arcline.SectorAngle()-2*Pi)<EPS)
		{
			f3dPoint center;
			center=arcline.Center();
			trans_pos(center,base_pnt);
			CreateAcadCircle(pBlockTableRecord,center,arcline.Radius(),arcline.ID);	
		}
		else if(arcline.SectorAngle()>0)
		{
			f3dPoint center,startPt;
			trans_pos(&arcline,base_pnt);
			center=arcline.Center();
			startPt=arcline.Start();
			if(arcline.ColumnNorm()==arcline.WorkNorm())	//圆弧
				CreateAcadArcLine(pBlockTableRecord,center,startPt,
				arcline.SectorAngle(),arcline.WorkNorm(),arcline.ID);
			else
				CreateAcadEllipseLine(pBlockTableRecord,arcline);
		}
		else 
		{
			trans_pos(&arcline,base_pnt);
			CreateAcadLine(pBlockTableRecord,arcline.Start(),arcline.End(),arcline.ID);
		}
	}
	//绘制螺栓孔 默认所有螺栓图符直径均为2mm 螺栓本身已缩放(直径/2)倍 wht 09-10-28
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	for(f3dCircle *pCir=pSectionViewDraw->GetLsCirList()->GetFirst();pCir;pCir=pSectionViewDraw->GetLsCirList()->GetNext())
	{
		f3dPoint centre=pCir->centre;
		trans_pos(centre, base_pnt);
		int nBoltD=ftoi(2*pCir->radius*pSectionViewDraw->GetInternalDrawingScale());//sys.general_set.WeldMapScale.fRealToDraw);
		//根据螺栓直径调整缩放比例
		//ls_scale=scale*pCir->radius;
		BOOL bRet=FALSE;
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle==1)	//图符处理
		{
			if(nBoltD==12)	
				bRet=InsertBlock(pBlockTableRecord,centre,"M12",1.0,0);
			else if(nBoltD==16)
				bRet=InsertBlock(pBlockTableRecord,centre,"M16",1.0,0);
			else if(nBoltD==18)
				bRet=InsertBlock(pBlockTableRecord,centre,"M18",1.0,0);
			else if(nBoltD==20)
				bRet=InsertBlock(pBlockTableRecord,centre,"M20",1.0,0);
			else if(nBoltD==22)
				bRet=InsertBlock(pBlockTableRecord,centre,"M22",1.0,0);
			else if(nBoltD==24)
				bRet=InsertBlock(pBlockTableRecord,centre,"M24",1.0,0);
			else
			{
				char ls_str[8];
				sprintf(ls_str,"M%d",ftol(2*pCir->radius));
				bRet=InsertBlock(pBlockTableRecord,centre,ls_str,1.0,0);
			}
		}
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==2)	//比例图符处理
		{
			if(nBoltD==12)	
				bRet=InsertBlock(pBlockTableRecord,centre,"M12",fInternalZoomScale,0);
			else if(nBoltD==16)
				bRet=InsertBlock(pBlockTableRecord,centre,"M16",fInternalZoomScale,0);
			else if(nBoltD==18)
				bRet=InsertBlock(pBlockTableRecord,centre,"M18",fInternalZoomScale,0);
			else if(nBoltD==20)
				bRet=InsertBlock(pBlockTableRecord,centre,"M20",fInternalZoomScale,0);
			else if(nBoltD==22)
				bRet=InsertBlock(pBlockTableRecord,centre,"M22",fInternalZoomScale,0);
			else if(nBoltD==24)
				bRet=InsertBlock(pBlockTableRecord,centre,"M24",fInternalZoomScale,0);
			else
			{
				char ls_str[8];
				sprintf(ls_str,"M%d",ftol(2*pCir->radius));
				bRet=InsertBlock(pBlockTableRecord,centre,ls_str,fInternalZoomScale,0);
			}
		}
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==3)	//比例直径图符处理
		{
			double holed=(2*pCir->radius*pSectionViewDraw->GetInternalDrawingScale()+pCir->feature/10.0);//+sys.general_set.fHoleAugmentInNC);
			double zoomScale=holed*zoomdraw_coef*sys.general_set.WeldMapScale.fBoltDiaFactor/pSectionViewDraw->GetInternalDrawingScale();
			if(nBoltD==12)	
				bRet=InsertBlock(pBlockTableRecord,centre,"M12",zoomScale,0);
			else if(nBoltD==16)
				bRet=InsertBlock(pBlockTableRecord,centre,"M16",zoomScale,0);
			else if(nBoltD==18)
				bRet=InsertBlock(pBlockTableRecord,centre,"M18",zoomScale,0);
			else if(nBoltD==20)
				bRet=InsertBlock(pBlockTableRecord,centre,"M20",zoomScale,0);
			else if(nBoltD==22)
				bRet=InsertBlock(pBlockTableRecord,centre,"M22",zoomScale,0);
			else if(nBoltD==24)
				bRet=InsertBlock(pBlockTableRecord,centre,"M24",zoomScale,0);
			else
			{
				char ls_str[8];
				sprintf(ls_str,"M%d",ftol(2*pCir->radius));
				bRet=InsertBlock(pBlockTableRecord,centre,ls_str,zoomScale,0);
			}
		}
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle==0||!bRet)	//圆孔处理
		{
			f3dCircle Cir;
			Cir.centre=centre;
			Cir.radius=zoomdraw_coef*(pCir->radius+pCir->feature/(20.0*pSectionViewDraw->GetInternalDrawingScale()));
			NewBoltCircle(pBlockTableRecord,Cir);
		}
	}
	//标注相对尺寸/绝对尺寸
	if(sys.part_group_map.bDimBeforeWeldLocatSpace||sys.part_group_map.bDimAfterWeldCheckLine)
	{	//标注相对尺寸
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		if(sys.general_set.iDimPrecision==0)		//精确到1mm
			CSizeTextDim::PRECISION=1.0;
		else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
			CSizeTextDim::PRECISION=0.5;
		else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
			CSizeTextDim::PRECISION=0.1;
		CSizeTextDim *pSpaceDim=NULL;
		for(pSpaceDim=pSectionViewDraw->GetSpaceDimList()->GetFirst();pSpaceDim;pSpaceDim=pSectionViewDraw->GetSpaceDimList()->GetNext())
		{
			if(pSpaceDim==NULL||pSpaceDim->dist==0)
				continue;
			f3dPoint dimStartPos=pSpaceDim->dimStartPos+pSpaceDim->origin;
			f3dPoint dimEndPos=pSpaceDim->dimEndPos+pSpaceDim->origin;
			f3dPoint dimPos=pSpaceDim->dimPos+pSpaceDim->origin;
			trans_pos(dimPos,base_pnt);
			trans_pos(dimStartPos,base_pnt);
			trans_pos(dimEndPos,base_pnt);
			if(abs(pSpaceDim->iOrder)==3&&!sys.part_group_map.bDimBeforeWeldLocatSpace)//焊接前定位尺寸
				continue;
			else if(pSpaceDim->iOrder==0&&!sys.part_group_map.bDimAfterWeldCheckLine)	//焊接后检测尺寸
				continue;
			//目前bCanTextOutter与iTextDimSylte==1同置于dimStyle4标注样式中，将来视情况看是否需要拆开 wjh-2019.5.12
			if(pSpaceDim->bCanTextOutter|| pSpaceDim->iTextDimSylte==1)
				DimAngleSize(pBlockTableRecord,dimStartPos,dimEndPos,dimPos,pSpaceDim->DimText(),DimStyleTable::dimStyle4.dimStyleId,pSpaceDim->angle,fDimTextSize);
			else
				DimAngleSize(pBlockTableRecord,dimStartPos,dimEndPos,dimPos,pSpaceDim->DimText(),DimStyleTable::dimStyle.dimStyleId,pSpaceDim->angle,fDimTextSize);
		}
	}
	
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(sys.general_set.iDimPrecision==0)		//精确到1mm
		CTextOnlyDrawing::PRECISION=1.0;
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
		CTextOnlyDrawing::PRECISION=0.5;
	else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
		CTextOnlyDrawing::PRECISION=0.1;
	CTextOnlyDrawing *pPrevLocatePointDim=NULL,*pLocatePointDim=NULL;
	for(pLocatePointDim=pSectionViewDraw->GetPureTextDimList()->GetFirst();pLocatePointDim;
		pLocatePointDim=pSectionViewDraw->GetPureTextDimList()->GetNext())
	{
		if(pLocatePointDim==NULL)
			continue;
		double fTextHeight=sys.part_group_map.fMinFontSize;
		//字体随缩放比例zoomdraw_coef进行缩放
		if(!sys.general_set.bProcessCardTbl)	
			fTextHeight=pLocatePointDim->textHeight*glbalWeldDrawingFontZoomCoef*zoomdraw_coef;//fInternalZoomScale;
		else	//保证字体最小高度为 fMinFontSize  wht 10-05-31
			fTextHeight=max(sys.part_group_map.fMinFontSize,pLocatePointDim->textHeight*glbalWeldDrawingFontZoomCoef*zoomdraw_coef);
		f3dPoint dimPos=pLocatePointDim->dimPos+pLocatePointDim->origin;
		trans_pos(dimPos,base_pnt);
		double fTextHeightOffset=fTextHeight*0.7;
		if(abs(pLocatePointDim->iOrder)==3)	//绘制剖视图符
		{
			AcDbObjectId LineId;//定义标识符
			f3dPoint rgnVert[4];
			AcGePoint2d acad_start,acad_end,acad_arrow;
			CSectionViewDrawing::SECTSYMBOLDIM xDimSectSymbol;
			CTextOnlyDrawing localDim=*pLocatePointDim;
			trans_pos(localDim.dimPos,base_pnt);
			trans_pos(localDim.origin,base_pnt);
			CSectionViewDrawing::CalSectViewSymbolDimPos(&localDim,rgnVert,fTextHeight,&xDimSectSymbol);
			dimPos=xDimSectSymbol.xTextDimPos;
			acad_start.x=xDimSectSymbol.xLineStart.x;
			acad_start.y=xDimSectSymbol.xLineStart.y;
			acad_end.x=xDimSectSymbol.xLineEnd.x;
			acad_end.y=xDimSectSymbol.xLineEnd.y;
			acad_arrow.x=xDimSectSymbol.xPrjTipPoint.x;
			acad_arrow.y=xDimSectSymbol.xPrjTipPoint.y;

			AcDbPolyline *pPolyLine=new AcDbPolyline(2);//创建LINE对象
			double fPolyLineWidth=sys.stru_map.fPolyLineWidth*zoomdraw_coef;
			pPolyLine->addVertexAt(0,acad_start,0,fPolyLineWidth,fPolyLineWidth);
			pPolyLine->addVertexAt(0,acad_end,0,fPolyLineWidth,fPolyLineWidth);
			pPolyLine->addVertexAt(0,acad_arrow,0,fPolyLineWidth,fPolyLineWidth);
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//将实体写入块表记录
				pPolyLine->close();//调用关闭对象的成员函数
			DimText(pBlockTableRecord,dimPos,pLocatePointDim->dimText,
				TextStyleTable::hzfs.textStyleId,fTextHeight,xDimSectSymbol.fSymbolFlowAngle,
				(AcDb::TextHorzMode)xDimSectSymbol.hAcadMode,(AcDb::TextVertMode)xDimSectSymbol.vAcadMode);//AcDb::kTextCenter,AcDb::kTextVertMid);*/
		}
		else if(sys.part_group_map.bDimAbsoluteDist||	//绝对尺寸
				pLocatePointDim->iOrder==0||			//焊接方向文字
				abs(pLocatePointDim->iOrder)==2||		//视图名称
				abs(pLocatePointDim->iOrder)==4)		//特殊字符
		{	//标注绝对尺寸或视图名称
			if(pLocatePointDim->iOrder==0&&pLocatePointDim->angle>Pi*0.5&&pLocatePointDim->angle<1.5*Pi)
				pLocatePointDim->angle=pLocatePointDim->angle-Pi;
			//TODO:需单独处理,且应与CTextOnlyDrawing::GetDimRect进行一致性处理 wjh-2018.8.1
			if(pLocatePointDim->alignType==MiddleCenter)
				DimText(pBlockTableRecord,dimPos,pLocatePointDim->GetPrecisionDimText(),
				TextStyleTable::hzfs.textStyleId,fTextHeight,pLocatePointDim->angle,
				AcDb::kTextCenter,AcDb::kTextVertMid);
			else if(pLocatePointDim->alignType==TopLeft)
				DimText(pBlockTableRecord,dimPos,pLocatePointDim->GetPrecisionDimText(),
				TextStyleTable::hzfs.textStyleId,fTextHeight,pLocatePointDim->angle,
				AcDb::kTextLeft,AcDb::kTextTop);
			else if(pLocatePointDim->alignType==TopCenter)
				DimText(pBlockTableRecord,dimPos,pLocatePointDim->GetPrecisionDimText(),
				TextStyleTable::hzfs.textStyleId,fTextHeight,pLocatePointDim->angle,
				AcDb::kTextCenter,AcDb::kTextTop);
			else if(pLocatePointDim->alignType==BottomCenter)
				DimText(pBlockTableRecord,dimPos,pLocatePointDim->GetPrecisionDimText(),
				TextStyleTable::hzfs.textStyleId,fTextHeight,pLocatePointDim->angle,
				AcDb::kTextCenter,AcDb::kTextBottom);
			else if(pLocatePointDim->alignType==MiddleLeft)
				DimText(pBlockTableRecord,dimPos,pLocatePointDim->GetPrecisionDimText(),
				TextStyleTable::hzfs.textStyleId,fTextHeight,pLocatePointDim->angle,
				AcDb::kTextLeft,AcDb::kTextVertMid);
			else if(pLocatePointDim->alignType==MiddleRight)
				DimText(pBlockTableRecord,dimPos,pLocatePointDim->GetPrecisionDimText(),
				TextStyleTable::hzfs.textStyleId,fTextHeight,pLocatePointDim->angle,
				AcDb::kTextRight,AcDb::kTextVertMid);
			else
			{
				DimText(pBlockTableRecord,dimPos,pLocatePointDim->GetPrecisionDimText(),
				TextStyleTable::hzfs.textStyleId,fTextHeight,pLocatePointDim->angle);
			}
		}
		pPrevLocatePointDim=pLocatePointDim;	
	}
	//标注角度线
	if(sys.part_group_map.bAngularDimension)
	{
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		CSizeAngleDim *pAngularDim=NULL;
		for(pAngularDim=pSectionViewDraw->GetAngularDimList()->GetFirst();pAngularDim;pAngularDim=pSectionViewDraw->GetAngularDimList()->GetNext())
		{
			f3dPoint start_vec=pAngularDim->dimStartPos-pAngularDim->vertex;
			f3dPoint end_vec=pAngularDim->dimEndPos-pAngularDim->vertex;
			normalize(start_vec);
			normalize(end_vec);
			if(fabs(start_vec*end_vec)>EPS_COS)
				continue;	//不标注0度或者180度
			if(pAngularDim==NULL)
				continue;
			trans_pos(pAngularDim->vertex,base_pnt);
			trans_pos(pAngularDim->dimStartPos,base_pnt);
			trans_pos(pAngularDim->dimEndPos,base_pnt);
			trans_pos(pAngularDim->dimPos,base_pnt);
			if(pAngularDim->dimText.GetLength()>0)	//指定标注内容
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,pAngularDim->vertex,
					pAngularDim->dimStartPos,pAngularDim->dimEndPos,pAngularDim->dimPos,pAngularDim->dimText,fDimAngleTextSize,fDimAngleArrowSize);	
			else	//自动计算标注角度
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,pAngularDim->vertex,
					pAngularDim->dimStartPos,pAngularDim->dimEndPos,pAngularDim->dimPos,NULL,fDimAngleTextSize,fDimAngleArrowSize);	
		}
	}
	//扩展数据点
	CTextFrame *pDataPoint=NULL;
	for(pDataPoint=pSectionViewDraw->GetDataPointDimList()->GetFirst();pDataPoint;pDataPoint=pSectionViewDraw->GetDataPointDimList()->GetNext())
	{
		PART_DIMnDRAW* pDimDraw=pSectionViewDraw->m_pPartGroupDrawing->GetPartDimDraw(pView?pView->GetId():0,pDataPoint->hPart);
		if(pDimDraw&&!pDimDraw->bDimPartNo&&!pDimDraw->bMandatoryDimLabel)
			continue;	//经过筛选在当前视图中该构件的绘制草图不标注编号
		AcDbXrecord *pXrec;
		AcDbObjectId dictObjId, xrecObjId,pointId;
		AcDbDictionary *pDict;
		f3dPoint data_pos=pDataPoint->dataPos;
		f3dPoint dim_pos=pDataPoint->dimPos;
		//根据比例得到引出线终端位置 wht 10-09-17
		//TODO:未来有时间应提前在GetRefLineEndPos时考虑到后续代码中增加的腰圆高度带来的影响　wjh-2016.10.19
		f3dPoint refEndPos=pDataPoint->GetRefLineEndPos(1.0);
		trans_pos(dim_pos,base_pnt);
		trans_pos(refEndPos,base_pnt);
		trans_pos(data_pos,base_pnt);
		AcDbPoint *pPoint=new AcDbPoint(AcGePoint3d(data_pos.x,data_pos.y,data_pos.z));
		if(pBlockTableRecord->appendAcDbEntity(pointId,pPoint)==Acad::eOk)//将实体写入块表记录
		{
			pPoint->createExtensionDictionary();
			dictObjId = pPoint->extensionDictionary();
			pPoint->close();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32,pDataPoint->hPart,AcDb::kDxfInt32,pDataPoint->cls_id,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		//根据数据点位置标注构件编号
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(strlen(pDataPoint->Text())>0&&sys.part_group_map.bAutoDimPartNo)
		{
			//fTextSize应取CTextFrame值, 否则可能会出现标注文字高度与外圈大小不一致情况
			if(zoomdraw_coef<1)
				pDataPoint->ZoomFrameSize(zoomdraw_coef,sys.part_group_map.fMinFontSize);
			double fTextSize=pDataPoint->TextHeight();
			double fHalfWidth=0.5*pDataPoint->Width();
			double fHalfHeight=0.5*pDataPoint->Height();
			if(pDataPoint->iCurLayer!=-1||pDimDraw->ciLeadLineMode>0)//绘制引出线
				CreateAcadLine(pBlockTableRecord,data_pos,refEndPos);
			f3dPoint text_pos(dim_pos.x,dim_pos.y+0.5*fTextSize);
			if(pDataPoint->iCurLayer!=-1)
			{
				DimText(pBlockTableRecord,text_pos,pDataPoint->Text(),TextStyleTable::hzfs.textStyleId,
					fTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
				//生成文本框的框线
				if(pDataPoint->Style()==0)
					CreateAcadCircle(pBlockTableRecord,dim_pos,fHalfWidth);
				else 
				{	
					if(zoomdraw_coef<1)	//减少由于视图缩小导致打印时腰圆上下边界与编号重叠　wjh-2016.10.19
						fHalfHeight+=(1-zoomdraw_coef)*0.5;
					f3dPoint topLeft(dim_pos.x-fHalfWidth,dim_pos.y+fHalfHeight);
					if(pDataPoint->Style()==1)	//腰圆矩形框
						CreateAcadRect(pBlockTableRecord,topLeft,2*fHalfWidth,2*fHalfHeight,TRUE);
					else	//直角矩形框
						CreateAcadRect(pBlockTableRecord,topLeft,2*fHalfWidth,2*fHalfHeight);
				}
			}
		}
	}
	//填充
	for(ACAD_HATCH *pHatch=pSectionViewDraw->GetHatchList()->GetFirst();pHatch;pHatch=pSectionViewDraw->GetHatchList()->GetNext())
		CreateAcadHatch(pBlockTableRecord,pHatch->vertexList,pHatch->sHatchStyle,pHatch->fScale,pHatch->fAngle,pHatch->color);
	//生成视图块引用
	DRAGSET.EndBlockRecord(pExterBlockTableRecord,blk_insert_pos,1);
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>
//判断两角钢是否共线且两肢方向一致,用于合并两靠模角钢 wht 15-11-06
static bool IsCollinearCoupleAngle(CLDSLineAngle *pLineAngle,CLDSLineAngle *pCoupleAngle)
{
	if(pLineAngle==NULL||pCoupleAngle==NULL)
		return false;
	f3dPoint line_vec=pLineAngle->End()-pLineAngle->Start();
	f3dPoint couple_line_vec=pCoupleAngle->End()-pCoupleAngle->Start();
	normalize(line_vec);
	normalize(couple_line_vec);
	double cosa=line_vec*couple_line_vec;
	if(cosa>EPS_COS2)
	{	//共线同向
		if( pLineAngle->get_norm_x_wing()*pCoupleAngle->get_norm_x_wing()>EPS_COS2&&
			pLineAngle->get_norm_y_wing()*pCoupleAngle->get_norm_y_wing()>EPS_COS2)
			return true;
	}
	else if(cosa<-EPS_COS2)
	{	//共线反向
		if( pLineAngle->get_norm_x_wing()*pCoupleAngle->get_norm_y_wing()>EPS_COS2&&
			pLineAngle->get_norm_y_wing()*pCoupleAngle->get_norm_x_wing()>EPS_COS2)
			return true;
	}
	return false;
}
void DrawPartGroupTopViewKaoMoMap(AcDbBlockTableRecord *pExterBlockTableRecord,CSectionView *pTopView,f3dPoint top_left,double draw_scale)
{
	if(pTopView==NULL)
		return;
	double marginSpace=30;
	CLDSLineAngle* anglePtrArr[2]={NULL,NULL};
	CLDSGeneralPlate* platePtrArr[4]={NULL,NULL,NULL,NULL};
	CHashSet<CLDSLineAngle*> hashAngleSet;
	if(pTopView->GetViewTypeId()==CSectionView::CLS_STDX_PLATES_VIEW)
	{	
		CStdXPlatesView *pStdXPlatesView=(CStdXPlatesView*)pTopView;
		platePtrArr[0]=(CLDSGeneralPlate*)Ta.FromPartHandle(pStdXPlatesView->m_hBasePlate,CLS_PLATE);
		platePtrArr[1]=(CLDSGeneralPlate*)Ta.FromPartHandle(pStdXPlatesView->m_hFrontVertPlate,CLS_PLATE);
		platePtrArr[2]=(CLDSGeneralPlate*)Ta.FromPartHandle(pStdXPlatesView->m_hBackVertPlate,CLS_PLATE);
		int index=0;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pStdXPlatesView->m_arrAngleHandle[i],CLS_LINEANGLE);
			if(pAngle&&!pAngle->m_bVirtualPart&&index<2)
				anglePtrArr[index++]=pAngle;
			if(pAngle)
				hashAngleSet.SetValue(pAngle->handle,pAngle);
		}
	}
	else if(pTopView->GetViewTypeId()==CSectionView::CLS_ANGLE_FOOT_VIEW)
	{
		CAngleFootView *pAngleFootView=(CAngleFootView*)pTopView;
		if(pAngleFootView->m_cSubType=='U')
		{
			platePtrArr[0]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hDownLeftShoePlate,CLS_PLATE,CLS_PARAMPLATE);
			platePtrArr[1]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hDownFrontShoePlate,CLS_PLATE,CLS_PARAMPLATE);
			platePtrArr[2]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hDownBackShoePlate,CLS_PLATE,CLS_PARAMPLATE);
			platePtrArr[3]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hDownRightShoePlate,CLS_PLATE,CLS_PARAMPLATE);
		}
		else
		{
			platePtrArr[0]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hShoePlate,CLS_PLATE,CLS_PARAMPLATE);
			platePtrArr[1]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hFrontShoePlate,CLS_PLATE,CLS_PARAMPLATE);
			platePtrArr[2]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hBackShoePlate,CLS_PLATE,CLS_PARAMPLATE);
			platePtrArr[3]=(CLDSGeneralPlate*)Ta.FromPartHandle(pAngleFootView->m_hRightShoePlate,CLS_PLATE,CLS_PARAMPLATE);
		}
		int index=0;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle* pAngle=NULL;
			if(pAngleFootView->m_cSubType=='U')
				pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pAngleFootView->m_arrDownAngleHandle[i],CLS_LINEANGLE);
			else
				pAngle=(CLDSLineAngle*)Ta.FromPartHandle(pAngleFootView->m_arrAngleHandle[i],CLS_LINEANGLE);
			if(pAngle&&!pAngle->m_bVirtualPart&&index<2)
				anglePtrArr[index++]=pAngle;
			if(pAngle)
				hashAngleSet.SetValue(pAngle->handle,pAngle);
		}
	}
	else 
		return;
	int i=0;
	CXhPtrSet<CLDSLineAngle> kmAngleSet;
	CHashSet<CLDSLineAngle*> hashCoupleAngleByMainAngleHandle;
	for(i=0;i<2;i++)
	{
		if(anglePtrArr[i])
			kmAngleSet.append(anglePtrArr[i]);
	}
	//接头位置上下两侧靠模角钢均需要绘制，提取十字插板视图时仅包含了一侧的角钢
	//需要从pTopView->SonParts中查找另外一侧的角钢并绘制靠模角钢 wht 15-10-30
	for(CSectionView::PART_ITEM *pItem=pTopView->SonParts.GetFirst();pItem;pItem=pTopView->SonParts.GetNext())
	{
		CLDSPart *pPart=Ta.Parts.FromHandle(pItem->hSonPart);
		if(pPart==NULL||!pPart->IsAngle()||pPart->m_bVirtualPart)
			continue;
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pPart;
			for(int i=0;i<4;i++)
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
				if(pAngle==NULL||pAngle->m_bVirtualPart)
					continue;
				if(hashAngleSet.GetValue(pAngle->handle)==NULL)
				{
					if(IsCollinearCoupleAngle(anglePtrArr[0],pAngle))
						hashCoupleAngleByMainAngleHandle.SetValue(anglePtrArr[0]->handle,pAngle);
					else if(IsCollinearCoupleAngle(anglePtrArr[1],pAngle))
						hashCoupleAngleByMainAngleHandle.SetValue(anglePtrArr[1]->handle,pAngle);
					else
						kmAngleSet.append(pAngle);
				}
			}
		}
		else if(hashAngleSet.GetValue(pPart->handle)==NULL)
		{
			if(IsCollinearCoupleAngle(anglePtrArr[0],(CLDSLineAngle*)pPart))
				hashCoupleAngleByMainAngleHandle.SetValue(anglePtrArr[0]->handle,(CLDSLineAngle*)pPart);
			else if(IsCollinearCoupleAngle(anglePtrArr[1],(CLDSLineAngle*)pPart))
				hashCoupleAngleByMainAngleHandle.SetValue(anglePtrArr[1]->handle,(CLDSLineAngle*)pPart);
			else
				kmAngleSet.append((CLDSLineAngle*)pPart);
		}
	}
	i=0;
	CKaoMeAngle kmAngle;
	//double km_angle_draw_scale=(1/sys.general_set.WeldMapScale.fRealToDraw)*draw_scale;	//绘制比例要考虑组焊件内部绘制比例及布局缩放比例 wht 15-11-06
	double km_angle_draw_scale=(1/sys.general_set.WeldMapScale.fRealToDraw);	//绘制比例要考虑组焊件内部绘制比例及布局缩放比例 wht 15-11-06
	km_angle_draw_scale = min(km_angle_draw_scale,draw_scale);
	for(CLDSLineAngle *pAngle=(CLDSLineAngle*)kmAngleSet.GetFirst();pAngle;pAngle=(CLDSLineAngle*)kmAngleSet.GetNext(),i++)
	{	//1.遍历钢板螺栓获取角钢关联螺栓
		long nBoltCount=0;
		int nKmAngleLen=0,nKmAngleWidth=0,nInitBoltCount=pAngle->GetLsCount();
		CLDSLineAngle *pCoupleAngle=hashCoupleAngleByMainAngleHandle.GetValue(pAngle->handle);
		if(pCoupleAngle)
			nInitBoltCount+=pCoupleAngle->GetLsCount();
		DYN_ARRAY<long> boltHandleArr(nInitBoltCount);
		for(int j=0;j<4;j++)
		{
			if(platePtrArr[j]==NULL)
				continue;
			for(CLsRef *pLsRef=platePtrArr[j]->GetFirstLsRef();pLsRef;pLsRef=platePtrArr[j]->GetNextLsRef())
			{
				if(pAngle->FindLsByHandle((*pLsRef)->handle)==NULL&&(pCoupleAngle==NULL||pCoupleAngle->FindLsByHandle((*pLsRef)->handle)==NULL))
					continue;
				boltHandleArr[nBoltCount]=(*pLsRef)->handle;
				nBoltCount++;
			}
		}
		CSectionKmAngleViewDrawing::InitKaoMeAngle(kmAngle,pAngle,boltHandleArr,nBoltCount,pCoupleAngle);
		nKmAngleLen=kmAngle.length;
		nKmAngleWidth=kmAngle.wide;
		f3dPoint top_left_temp(top_left.x+i*(nKmAngleLen*km_angle_draw_scale+marginSpace),top_left.y-1.5*nKmAngleWidth*km_angle_draw_scale);
		DrawKaoMoAngle(pExterBlockTableRecord,top_left_temp,km_angle_draw_scale,kmAngle);
	}
}
//绘制靠模角钢
void DrawKaoMoAngle(AcDbBlockTableRecord *pExterBlockTableRecord,f3dPoint top_left,double draw_scale,CKaoMeAngle &kmAngle)
{
	CProcessAngle processAngle;
	processAngle.m_wLength=kmAngle.length;
	processAngle.m_fWidth=kmAngle.wide;
	processAngle.m_fThick=kmAngle.thick;
	processAngle.m_xBoltInfoList.Empty();
	for(CKaoMeAngle::BOLTHOLE *pHole=kmAngle.listHoles.GetFirst();pHole;pHole=kmAngle.listHoles.GetNext())
	{
		BOLT_INFO *pInfo=processAngle.m_xBoltInfoList.Add(0);
		pInfo->posX=pHole->pos.x;
		pInfo->posY=pHole->pos.y;
		pInfo->bolt_d=pHole->d;
	}
	CLineAngleDrawing xAngleDrawing;
	xAngleDrawing.dimStyleFlag.SetFlagDword(CAngleDrawing::SpecifyDrawscale);
	xAngleDrawing.SetVertScaleCoef(draw_scale);
	xAngleDrawing.SetLengthScaleCoef(draw_scale);
	xAngleDrawing.CreateLineAngleDrawing(&processAngle,kmAngle.length,kmAngle.wide);
	DRAGSET.BeginBlockRecord();
	DrawJgMap(&xAngleDrawing,f3dPoint(0,0,0),FALSE,DRAGSET.RecordingBlockTableRecord());
	CHashList<short>holedarr;
	CXhChar100 sKmLabel("%s,L%dx%dx%dx1",(char*)kmAngle.sLabel,kmAngle.wide,kmAngle.thick,kmAngle.length);
	for(CKaoMeAngle::BOLTHOLE* pHole=kmAngle.listHoles.GetFirst();pHole;pHole=kmAngle.listHoles.GetNext())
	{
		double hole_d=pHole->d+1.5;
		if(holedarr.GetValue(pHole->d)==NULL)
		{
			holedarr.SetValue(pHole->d,pHole->d);
			int hits=1;
			BOOL pushed=kmAngle.listHoles.push_stack();
			for(CKaoMeAngle::BOLTHOLE* pAfterHole=kmAngle.listHoles.GetNext();pAfterHole;pAfterHole=kmAngle.listHoles.GetNext())
			{
				if(pHole->d==pAfterHole->d)
					hits++;
			}
			kmAngle.listHoles.pop_stack(pushed);
			sKmLabel.Append(CXhChar16(" Φ%.1f*%d",hole_d,hits));
		}
	}
	if(strlen(kmAngle.sKaiHeAngle)>0)
		sKmLabel.Append(CXhChar16(" 开合角%s",(char*)kmAngle.sKaiHeAngle));
	f2dRect rect=xAngleDrawing.GetDrawRegion(draw_scale);
	f3dPoint dimPos(0.5*kmAngle.length*draw_scale,rect.bottomRight.y-3*sys.fonts.weld.fPartNoTextSize);
	DimText(DRAGSET.RecordingBlockTableRecord(),dimPos,sKmLabel,TextStyleTable::hzfs.textStyleId,
		sys.fonts.weld.fPartNoTextSize,0,AcDb::kTextMid,AcDb::kTextTop);
	DRAGSET.EndBlockRecord(pExterBlockTableRecord,GEPOINT(top_left.x,top_left.y));
}

void PartGroupSetupDrawing()
{
	CLogErrorLife logErrorLife;
	sys.m_iCurMapType = 5;  //现已转换为sys.fonts.模式，实际即使初始化也应为５（代表组焊图）
	//sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    InitDrawingEnv();
	CAcModuleResourceOverride resOverride;
	CSelPartGroupDlg part_group_dlg;
	CTmaPtrList<CLDSPartGroup,CLDSPartGroup*> partGroupList;
	part_group_dlg.m_pWeldPartSet=&partGroupList;
	//提供交互界面供用户选择当前需要绘制的焊接组合件
	if(Ta.PartGroup.GetNodeNum()==0||part_group_dlg.DoModal()!=IDOK)
		return;
	if(partGroupList.GetNodeNum()==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The suitable weld parts are not selected, please select again!");
#else
		AfxMessageBox("没选中合适的组焊件，请重新选择!");
#endif
		return;
	}
	//
	f2dRect rcWeldCard;
	if(GetProcessCardRegion(sys.general_set.m_sWeldPartGroupCard,&rcWeldCard))
	{
		int SPACE_HORIZON=3,SPACE_VERTICAL=3;
		if(partGroupList.GetNodeNum()>1)
		{
			CPartDrawingArraySpaceDlg spaceDlg;
			spaceDlg.m_iSpaceMode=1;	//显示间隙
			spaceDlg.m_nRowHigh=SPACE_HORIZON;
			spaceDlg.m_nColumnWidth=SPACE_VERTICAL;
			spaceDlg.DoModal();
			SPACE_HORIZON=spaceDlg.m_nRowHigh;
			SPACE_VERTICAL=spaceDlg.m_nColumnWidth;
		}
		rcWeldCard.topLeft.y=rcWeldCard.Height()+SPACE_VERTICAL;
		rcWeldCard.bottomRight.x=rcWeldCard.Width()+SPACE_HORIZON;
	}
	else
	{
		rcWeldCard.topLeft.y=sys.general_set.nRowHigh;
		rcWeldCard.bottomRight.x=sys.general_set.nColumnWidth;
	}
	CLogErrorLife life;
	//获得当前图形的块表指针
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pBlockTableRecord==NULL)
		return;
	BOOL bUseDrag=TRUE;//partGroupList.GetNodeNum()==1;
	if(bUseDrag)
		DRAGSET.ClearEntSet();
	f3dPoint origin;	//原点坐标为(0,0,0)
	PAGE_INDEX pagenumer;
	for(CLDSPartGroup *pPartGroup=partGroupList.GetFirst();pPartGroup;pPartGroup=partGroupList.GetNext(),pagenumer.IncrementSerial())
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=Ta.FromPartHandle(pPartGroup->handle);
		if(!pDatumPart.IsHasPtr())
			return;
		if(pDatumPart->IsLinePart())
		{	//如果定位基准构件是杆件，则记录钢管的长度延伸方向作为绘图坐标系的X轴方向
			DRAGSET.axis_vec=pDatumPart.LinePartPointer()->End()-pDatumPart.LinePartPointer()->Start();
			normalize(DRAGSET.axis_vec);
		}
		else
			DRAGSET.axis_vec.Set();
		CSectionView *pView=NULL;
		for(pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
		{
			if(pView->LocatePoints.GetNodeNum()==0)
				pView->AddDefaultLocatePoints(pDatumPart,&Ta);
			if(pView->CheckLines.GetNodeNum()==0)
				pView->AddDefaultCheckLines(pDatumPart,&Ta);
			if(pView->Angulars.GetNodeNum()==0)
				pView->AddDefaultAngulars(pDatumPart,&Ta);
		}
		CPartGroupDrawing xPartGroupDraw;
		//设置组焊件缩放阈值以及缩放比例
		f2dRect draw_rect=GetProcessCardDrawingRect(1);
		xPartGroupDraw.InitPartGroupDrawing(pPartGroup,draw_rect.Width(),draw_rect.Height());
		//生成此构件的需要注释的内容
		//GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		PART_GROUP_CARD_INFO PartGroupInfo;
		memset(&PartGroupInfo,'\0',sizeof(PART_GROUP_CARD_INFO));
		if(sys.general_set.bProcessCardTbl)
		{	//工艺卡信息
			CXhChar100 guige;
			pDatumPart->GetSpecification(guige,FALSE);
			PartGroupInfo.m_nPageIndex=1;//iOrder+1;
			PartGroupInfo.m_sPartNo.Copy(GetPartNoIncMat(pDatumPart->GetPartNo(),pDatumPart->cMaterial));
			LOCALE_PROPITEM* pWeldLabelPrefix = gxDrwLocalizer.GetLocaleItem("WeldLabelPrefix");
			if (pWeldLabelPrefix)
				PartGroupInfo.m_sPartNo.InsertBefore(pWeldLabelPrefix->szStrBuff, 0);
			PartGroupInfo.m_sSegStr = pDatumPart->iSeg.ToString();
			//添加以下代码主要是为了使工艺卡上的长度与中间大样图上的总长标注小数点后一位也一致
			double length=pDatumPart->GetLength();
			if(sys.general_set.iDimPrecision==0)		//精确到1mm
				length = ftoi(length);
			else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
				length = fto_halfi(length);
			else if(sys.general_set.iDimPrecision==2)	//精确到0.1mm
				length=ftol(length*2)/2.0;
			PartGroupInfo.m_sLength.Printf("%.1f",length);
			SimplifiedNumString(PartGroupInfo.m_sLength);
			LOCALE_PROPITEM* pProcessSymbolMark = gxDrwLocalizer.GetLocaleItem("ProcessCard.HasSymbol");
			if(pProcessSymbolMark)//gxDrwLocalizer.ciCustomerSerial==5)	//青岛东方定制
				PartGroupInfo.m_sWeld.Copy(pProcessSymbolMark->szStrBuff);//"○");
			else if(sys.part_map.iPartMapStyle==1)	//浙江盛达铁塔有限公司
				PartGroupInfo.m_sWeld.Copy("V");
			else
				PartGroupInfo.m_sWeld.Copy("√");
			PartGroupInfo.m_sPartNum.Printf("%d",pPartGroup->m_uStatMatCoef);
			PartGroupInfo.m_sSumPartNum.Printf("%d",pPartGroup->m_uStatMatCoef*Ta.m_nTaNum);
			PartGroupInfo.m_sNotes.Printf("%s",pDatumPart->sNotes);
			CXhChar16 steelmark;
			QuerySteelMatMark(pDatumPart->cMaterial,steelmark);
			PartGroupInfo.m_sMaterial.Copy(steelmark);
			PartGroupInfo.m_sGuiGe.Copy(guige);
			
			int num=0;
			double thick=0,h=0;
			PARTSET partset;
			Ta.GetPartGroupPartSet(pPartGroup->handle,partset);
			CLDSPart *pPart=NULL;
			for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
			{
				if(pPart->GetClassTypeId()!=CLS_LINESLOT)
					continue;
				CLDSLineSlot *pLineSlot=(CLDSLineSlot*)pPart;
				if(!pLineSlot->IsBaseSlot())
					continue;
				if(num==0)
				{
					thick=pLineSlot->GetThick();
					h=pLineSlot->GetHeight();
				}
				num++;
			}
			if(num>0)
#ifdef AFX_TARG_ENU_ENGLISH
				PartGroupInfo.m_sFootNailPlate.Printf("[%.fX%.fX%d piece/strip",thick,h,num);
#else
				PartGroupInfo.m_sFootNailPlate.Printf("[%.fX%.fX%d件/条",thick,h,num);
#endif
		}
		if(part_group_dlg.m_pSelSectionView)
		{	//单独生成每个组焊件视图
			xPartGroupDraw.EmptyDrawingComponents();
			CSectionViewDrawing *pViewDrawing=xPartGroupDraw.CreateSectionViewDrawing(part_group_dlg.m_pSelSectionView);
			if(pViewDrawing==NULL)
				continue;	//选择失败
			pViewDrawing->FinalDrawing();	//暂时在此处处理，以后应统筹考虑(不调用FinalDrawing(),单独生成视图时不按设置比例缩放) wht 16-11-09
			f3dPoint base_pnt=origin;
			double zoomdraw_coef=1.0;
			if(sys.general_set.bProcessCardTbl)
			{	//插入工艺卡图框
				draw_rect = InsertPartGroupProcessCardTbl(pBlockTableRecord,origin,&PartGroupInfo);
				if(draw_rect.Width()<EPS2||draw_rect.Height()<EPS2)
					continue;
				f2dRect actual_rect = pViewDrawing->GetDrawRegion();
				//适当缩小草图区域位置,与上下左右边之间留5mm间隙
				draw_rect.topLeft.x+=5;
				draw_rect.topLeft.y-=5;
				draw_rect.bottomRight.x-=5;
				draw_rect.bottomRight.y+=5;
				//根据工艺卡图框自动调整组焊图显示比例
				double zoom_scale1 = draw_rect.Width()/actual_rect.Width();
				double zoom_scale2 = draw_rect.Height()/actual_rect.Height();
				zoomdraw_coef=min(zoom_scale1,zoom_scale2);
				zoomdraw_coef=min(1,zoomdraw_coef);
				base_pnt.x = draw_rect.topLeft.x-actual_rect.topLeft.x*zoomdraw_coef;
				base_pnt.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)*0.5;
			}
			//绘制组焊图
			DrawSectionViewMap(pBlockTableRecord,pViewDrawing,base_pnt,zoomdraw_coef);
		}
		else if(xPartGroupDraw.CreatePartGroupDrawing())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(NULL,"Generating No. %d weld parts view(%s)!",pagenumer.wiSerial+1,pPartGroup->sGroupName);
#else
			PutMessage(NULL,"正在生成第%d个组焊件视图(%s)!",pagenumer.wiSerial+1,pPartGroup->sGroupName);
#endif
			double zoomdraw_coef=1.0;
			ARRAY_LIST<double> zoomdraw_coef_arr;
			int nPageCount=xPartGroupDraw.Relayout(zoomdraw_coef_arr);
			pagenumer.ciPagesOfSerial=nPageCount;
			for(int i=0;i<nPageCount;i++)
			{
				zoomdraw_coef=zoomdraw_coef_arr[i];
				pagenumer.ciIndexInSerial=i;
				if(i>0)
					pagenumer.IncrementLiPage();
				f3dPoint base_pnt,cardTopLeft,cardTopRight,cardBtmRight;
				double scale_y=1.0;
				if(pagenumer.liPage>0)
				{
					if(sys.general_set.bCardLayoutBaseRow)
					{	//以行为基准进行排列
						origin.x = (pagenumer.liPage%sys.general_set.nPartRowsN)*rcWeldCard.Width();
						origin.y = -((pagenumer.liPage/sys.general_set.nPartRowsN)*rcWeldCard.Height());
					}
					else
					{	//以列为基准进行排列
						origin.x = (pagenumer.liPage/sys.general_set.nPartRowsN)*rcWeldCard.Width();
						origin.y = -((pagenumer.liPage%sys.general_set.nPartRowsN)*rcWeldCard.Height());
					}
				}
				if(sys.general_set.bProcessCardTbl)
				{	//插入工艺卡图框
					draw_rect = InsertPartGroupProcessCardTbl(pBlockTableRecord,origin,&PartGroupInfo);
					if(draw_rect.Width()<EPS2||draw_rect.Height()<EPS2)
						continue;
					cardTopLeft.Set(draw_rect.topLeft.x,draw_rect.topLeft.y);
					cardTopRight.Set(draw_rect.bottomRight.x,draw_rect.topLeft.y);
					cardBtmRight.Set(draw_rect.bottomRight.x,draw_rect.bottomRight.y);
					//f3dPoint cartTopRight(draw_rect.bottomRight.x,draw_rect.topLeft.y);
				}
				else
				{
					cardTopLeft=origin;
					cardTopRight.Set(origin.x+draw_rect.Width(),origin.y);
					cardBtmRight.Set(origin.x+draw_rect.Width(),origin.y-draw_rect.Height());
				}
				//绘制构件明细表
				f3dPoint xTableAnchorPoint=cardTopRight;//f3dPoint(drawingRectLayout.GetFrameWidth()*zoomdraw_coef,0,0)+cardTopLeft;
				if(gxDrwLocalizer.ciCustomerSerial==CLIENT_QingDao_DongFang)
					//(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES||pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)&&	//组合角钢十字插板
					
				{	//青岛东方组合角钢十字插板组焊图中明细表置于右下角
					xTableAnchorPoint=cardBtmRight;
				}
				SetCurLayer(LayerTable::VisibleProfileLayer.layerName);
				CSectionViewDrawing *pViewDrawing=NULL;
				CPartGroupTblDrawing *pTblDrawing=NULL;
				for(pViewDrawing=(CSectionViewDrawing*)xPartGroupDraw.EnumFirstDrawingComponent();pViewDrawing;pViewDrawing=(CSectionViewDrawing*)xPartGroupDraw.EnumNextDrawingComponent())
				{
					if(pViewDrawing->GetComponentType()!=CSectionViewDrawing::CLS_BOM_VIEW)
						continue;
					if(pViewDrawing->drawingRect.m_iPageIndex!=i)
						continue;
					pTblDrawing=(CPartGroupTblDrawing*)pViewDrawing;
					pTblDrawing->m_xPartTbl.SetBasePos(f2dPoint());
					DRAGSET.BeginBlockRecord();
					if(sys.general_set.bProcessCardTbl&&zoomdraw_coef>=1
						||!sys.part_group_map.part_table.bZoomInProcessCard)	//组焊件明细表不进行放大操作，强行放置在工艺卡右上角位置
						pTblDrawing->CreatePartGroupTable(1);
					else
						pTblDrawing->CreatePartGroupTable(zoomdraw_coef);
					int niTblLocateStyle=1;//右上角
					if(gxDrwLocalizer.ciCustomerSerial==CLIENT_QingDao_DongFang)	//青岛东方
						//(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES||pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)&&	//组合角钢十字插板
						niTblLocateStyle=3;//右下角
					DrawTable(DRAGSET.RecordingBlockTableRecord(),&pTblDrawing->m_xPartTbl,FALSE,1,niTblLocateStyle);
					DRAGSET.EndBlockRecord(pBlockTableRecord,GEPOINT(xTableAnchorPoint.x,xTableAnchorPoint.y));
					f2dRect table_rect=pTblDrawing->GetDrawRegion();
					if(niTblLocateStyle!=3)
						xTableAnchorPoint.y-=table_rect.Height();
					else
						xTableAnchorPoint.y+=table_rect.Height();
				}
				if(gxDrwLocalizer.ciCustomerSerial==CLIENT_QingDao_DongFang)
					//(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES||pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)&&	//组合角钢十字插板
				{	//青岛东方
					xTableAnchorPoint.x-=67;	//青岛东方组焊图中的工时定额框在十字插板组焊图中应置于构件明细表上方
					InsertBlock(pBlockTableRecord,xTableAnchorPoint,"QDDF-WorkCost#67x46",1,0);
				}
				//绘制组焊图
				for(pViewDrawing=(CSectionViewDrawing*)xPartGroupDraw.EnumFirstDrawingComponent();pViewDrawing;pViewDrawing=(CSectionViewDrawing*)xPartGroupDraw.EnumNextDrawingComponent())
				{
					if(pViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_BOM_VIEW)
						continue;
					DRAWRECT drawingRect=pViewDrawing->drawingRect;
					if(drawingRect.m_iPageIndex!=i)
						continue;
					//绘制组焊图
					base_pnt=f3dPoint(drawingRect.LocatePoint())*zoomdraw_coef+cardTopLeft;
					if(pViewDrawing->m_iComponentType==CSectionViewDrawing::CLS_KM_ANGLE_VIEW)
						base_pnt.y-=drawingRect.height*0.5;	//绘制角钢靠模视图时基准点不在矩形框左上角，而在角钢楞点位置即居中高度中点 wht 15-09-09
					////////////////////////////////////
					/*测试时绘制包络矩形
					ACADDRAW_CONTEXT context(pBlockTableRecord);
					f3dPoint upper(cardTopLeft.x+zoomdraw_coef*drawingRect.topLeft.x,cardTopLeft.y+zoomdraw_coef*drawingRect.topLeft.y);
					f3dPoint downr(upper.x,cardTopLeft.y+zoomdraw_coef*(drawingRect.topLeft.y-drawingRect.height));
					CreateAcadLine(context,upper,downr);
					f3dPoint upper2(cardTopLeft.x+zoomdraw_coef*(drawingRect.topLeft.x+drawingRect.width),upper.y);
					f3dPoint downr2(upper2.x,downr.y);
					CreateAcadLine(context,upper2,downr2);
					CreateAcadLine(context,upper,upper2);
					CreateAcadLine(context,downr,downr2);*/
					////////////////////////////////////
					//TODO:未处理完全部视图类型的统一比例 double dfActualScaleOfModel2Draw=pViewDrawing->dfScaleOfModel2Draw/zoomdraw_coef;　wjh-2019.5.5
					DrawSectionViewMap(pBlockTableRecord,pViewDrawing,base_pnt,zoomdraw_coef);
					//绘制靠摸角钢(东方工艺卡中不绘制靠摸角钢)
					if( CLDSPart::CustomerSerial!=CLIENT_QingDao_DongFang &&
						(pViewDrawing->m_iComponentType==CSectionViewDrawing::CLS_STDX_PLATES_VIEW||
						pViewDrawing->m_iComponentType==CSectionViewDrawing::CLS_ANGLE_FOOT_VIEW))
					{	//CSectionViewrawing::dimStyleFlag.KaoMeAngle
						double fDrawingScale=(1/pViewDrawing->GetInternalDrawingScale());
						CSectionView *pView=pViewDrawing->GetSectionView();
						if(pView&&(pView->GetViewSubType()=='D'||pView->GetViewSubType()=='U'))
							DrawPartGroupTopViewKaoMoMap(pBlockTableRecord,pView,f3dPoint(base_pnt.x+drawingRect.width,base_pnt.y+drawingRect.height*0.5), fDrawingScale);
							//DrawPartGroupTopViewKaoMoMap(pBlockTableRecord,pView,f3dPoint(base_pnt.x+drawingRect.width,base_pnt.y+drawingRect.height*0.5),zoomdraw_coef);
					}
					/*f3dPoint vertex_arr[4];
					vertex_arr[0].Set(base_pnt.x,base_pnt.y);
					vertex_arr[1].Set(base_pnt.x,base_pnt.y+drawingRect.height);
					vertex_arr[2].Set(base_pnt.x+drawingRect.width,base_pnt.y+drawingRect.height);
					vertex_arr[3].Set(base_pnt.x+drawingRect.width,base_pnt.y);
					CreateAcadRect(pBlockTableRecord,base_pnt,drawingRect.width,drawingRect.height);*/
				}
			}
			if(CLDSPart::CustomerSerial==CLIENT_ChangChun_JuDeLong)	//11
			{	//绘制插板零件图,初始化时必须设置归属图纸 wht 18-09-24
				CSectionTubeEndPlateViewDrawing startDrawing(0,&xPartGroupDraw),endDrawing(0,&xPartGroupDraw);
				BYTE cFlag=xPartGroupDraw.InitEndInsertPlateDrawing(&startDrawing,&endDrawing);
				if(cFlag>0)
				{	//
					GEPOINT base_pnt(draw_rect.topLeft.x+100,draw_rect.bottomRight.y);
					if(cFlag&0x01)
					{
						f2dRect rect=startDrawing.GetDrawRegion();
						DrawSectionViewMap(pBlockTableRecord,&startDrawing,base_pnt);
						base_pnt.x+=rect.Width();
					}
					if(cFlag&0x02)
					{
						f2dRect rect=endDrawing.GetDrawRegion();
						DrawSectionViewMap(pBlockTableRecord,&endDrawing,base_pnt);
					}
				}
			}
			if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES)
			{	//标注十字插板组焊件加工工艺文字
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 dimTextArr[4]={"process:",
										  "  1.leveling all plate before combination",
									      "  2.locating weld parts by module,flat weld near the module",
									      "  3.weld line totally enclose,level 2 weld line"};
#else
				CXhChar100 dimTextArr[4]={"加工工艺:",
										  "  1.所有板组合前校平",
									      "  2.用靠模定位组焊，靠模处平焊",
									      "  3.焊缝全封闭，二级焊缝"};
				if (gxDrwLocalizer.ciCustomerSerial == XHCID::CID_QingDao_DongFang)
					dimTextArr[3] = "  3.焊缝全封闭";	//青岛东方不需要标“二级焊缝”字样 wjh-2019.5.20
#endif
				DRAGSET.BeginBlockRecord();
				f3dPoint dimPos;
				for(int i=3;i>=0;i--)
				{
					dimPos.y+=sys.fonts.weld.fPartNoTextSize+1;
					DimText(DRAGSET.RecordingBlockTableRecord(),dimPos,dimTextArr[i],TextStyleTable::hzfs.textStyleId,
						    sys.fonts.weld.fPartNoTextSize,0,AcDb::kTextLeft,AcDb::kTextBottom);
				}
				DRAGSET.EndBlockRecord(pBlockTableRecord,GEPOINT(draw_rect.bottomRight.x-draw_rect.Width(),draw_rect.bottomRight.y));
			}
		}
	}
	pBlockTableRecord->close();
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"a",RTNONE);
	ads_command(RTSTR,L"re",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"a",RTNONE);
	ads_command(RTSTR,"re",RTNONE);
#endif
	PutMessage(NULL,NULL);
	ads_point basepnt;
	basepnt[X]=origin.x;
	basepnt[Y]=origin.y;
	basepnt[Z]=0;
	if(bUseDrag)	//启用拖放功能
#ifdef AFX_TARG_ENU_ENGLISH
		DragEntSet(basepnt,"input the insertion point\n");
#else
		DragEntSet(basepnt,"请输入插入点\n");
#endif
}
//
static void DrawJgPlanView(AcDbBlockTableRecord *pBlockTableRecord,
	CLDSLineAngle* pCommJg,long hBaseNode,UCS_STRU& ucs,double draw_scale)
{
	if(pCommJg==NULL)
		return;
	LINEPARTSET partset;
	if(pCommJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pCommJg;
		CLDSLineAngle *pSonJg[4]={NULL};
		switch(pGroupJg->group_style)
		{
		case 0:	//对角组合
			pSonJg[0]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pSonJg[1]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			partset.append(pSonJg[0]);
			partset.append(pSonJg[1]);
			break;
		case 1:	//T型组合
			pSonJg[0]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pSonJg[1]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			partset.append(pSonJg[0]);
			partset.append(pSonJg[1]);
			break;
		case 2:	//十字交叉组合
			pSonJg[0]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pSonJg[1]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			pSonJg[2]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
			pSonJg[3]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
			partset.append(pSonJg[0]);
			partset.append(pSonJg[1]);
			partset.append(pSonJg[2]);
			partset.append(pSonJg[3]);
			break;
		default:
			break;
		}
	}
	else
		partset.append(pCommJg);
	for(CLDSLinePart *pLinePart=partset.GetFirst();pLinePart;pLinePart=partset.GetNext())
	{
		pCommJg=(CLDSLineAngle*)pLinePart;
		f3dPoint line_vec;
		if(pCommJg->pStart->handle==hBaseNode)
		{	//始端连于节点
			line_vec=pCommJg->End()-pCommJg->Start();
			if(line_vec*ucs.axis_z>0)	//底板上侧基准角钢
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			else
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);	//底板下侧基准角钢
		}
		else
		{	//终端连于节点
			line_vec=pCommJg->Start()-pCommJg->End();
			if(line_vec*ucs.axis_z>0)	//底板上侧基准角钢
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			else
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);	//底板下侧基准角钢
		}
		normalize(line_vec);
		f3dPoint vertex[6];
		pCommJg->Create3dSolidModel();
		if(pCommJg->pSolidBody->KeyPointNum()>=6)
		{
			int j;
			for(j=0;j<6;j++)
			{
				GEPOINT vertice=pCommJg->pSolidBody->GetKeyPointAt(j);
				Int3dlf(vertex[j],vertice,line_vec,ucs.origin,ucs.axis_z);
				coord_trans(vertex[j],ucs,FALSE);
				vertex[j]/=draw_scale;
			}
			for(j=0;j<6;j++)
				CreateAcadLine(pBlockTableRecord,vertex[j],vertex[(j+1)%6]);
		}
	}
}
static void DrawFootPlanView(AcDbBlockTableRecord *pBlockTableRecord, 
	long hFoot,PARTSET& plateSet, UCS_STRU &ucs,double draw_scale)
{	//绘制底脚板的俯视/仰视图
	f3dLine line;
	for(CLDSPart* pPart=plateSet.GetFirst();pPart;pPart=plateSet.GetNext())
	{
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate* pPlank=(CLDSGeneralPlate*)pPart;
		if(pPlank->vertex_list.GetNodeNum()<=0)
			continue;
		if(fabs(pPlank->ucs.axis_z*ucs.axis_z)>EPS_COS2)	//底座板
		{
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			int nNum=pPlank->vertex_list.GetNodeNum();
			for(int i=0;i<nNum;i++)
			{
				line.startPt=pPlank->vertex_list[i].vertex;
				line.endPt=pPlank->vertex_list[(i+1)%nNum].vertex;
				coord_trans(line.startPt,pPlank->ucs,TRUE);
				coord_trans(line.startPt,ucs,FALSE);
				coord_trans(line.endPt,pPlank->ucs,TRUE);
				coord_trans(line.endPt,ucs,FALSE);
				line.startPt.z=line.endPt.z=0;
				line.startPt/=draw_scale;
				line.endPt/=draw_scale;
				CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
			}
			double ls_d;
			GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
			for(CLsRef *pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
			{
				f3dCircle circle;
				circle.centre=(*pLsRef)->ucs.origin;
				circle.radius=((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2.0;
				coord_trans(circle.centre,ucs,FALSE);
				circle.centre.z=0;
				circle.centre/=draw_scale;
				CreateAcadCircle(pBlockTableRecord,circle.centre,circle.radius/draw_scale);
				ls_d=(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment;
			}
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
			double scale=5.0/draw_scale;
			line.startPt.Set(-ls_d*scale,0,0);
			line.endPt.Set(ls_d*scale,0,0);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
			line.startPt.Set(0,-ls_d*scale,0);
			line.endPt.Set(0,ls_d*scale,0);
			CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
		}
		else
		{	//靴板或肋板
			double start_dist=100000,end_dist=-100000;
			f3dPoint inters_line_vec,vertex,perp,vec;
			f3dLine base_line,offset_line,segline;
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			Int3d2f(&base_line.startPt,&inters_line_vec,ucs.origin,ucs.axis_z,
				pPlank->ucs.origin,pPlank->ucs.axis_z);
			base_line.endPt=base_line.startPt+100*inters_line_vec;
			vector_trans(inters_line_vec,ucs,FALSE);
			normalize(inters_line_vec);
			coord_trans(base_line.startPt,ucs,FALSE);
			coord_trans(base_line.endPt,ucs,FALSE);
			//绘制牛蹄板时 仰视图仅绘制下侧钢板 俯视图仅绘制上侧钢板
			BOOL bDrawPlate=FALSE;	//钢板所有顶点均不合法时不需要绘制
			double fBaseThick=Ta.FromPartHandle(hFoot)->GetThick();
			for(PROFILE_VERTEX *pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlank->ucs,TRUE);
				coord_trans(vertex,ucs,FALSE);
				if(fabs(vertex.z)>(pPlank->GetThick()))
					continue;
				if(!bDrawPlate)
					bDrawPlate=TRUE;
				SnapPerp(&perp,base_line,vertex,NULL);	//点到线的投影
				vec=perp-base_line.startPt;
				if(vec*inters_line_vec>0)	//始点右侧
				{
					double dd=vec.mod();
					if(start_dist>dd)
						start_dist=dd;
					if(end_dist<dd)
						end_dist=dd;
				}
				else	//始点左侧
				{
					double dd=-vec.mod();
					if(start_dist>dd)
						start_dist=dd;
					if(end_dist<dd)
						end_dist=dd;
				}
			}
			if(!bDrawPlate)
				continue;
			perp=base_line.startPt;
			base_line.startPt=perp+inters_line_vec*start_dist;
			base_line.endPt=perp+inters_line_vec*end_dist;
			double angle=cal_angle_of_2vec(ucs.axis_z,pPlank->ucs.axis_z);
			vec=pPlank->ucs.axis_z;
			vector_trans(vec,ucs,FALSE);
			vec.z=0;
			normalize(vec);
			double sina=sin(angle);
			if(pPlank->GetClassTypeId()==CLS_PARAMPLATE)
			{	//参数化肋板的基准线在板的中间，此处需做特殊处理
				CLDSParamPlate* pRibPlate=(CLDSParamPlate*)pPlank;
				if(pRibPlate->m_iParamType==TYPE_RIBPLATE)
				{
					base_line.startPt-=vec*pPlank->GetThick()*0.5;
					base_line.endPt-=vec*pPlank->GetThick()*0.5;
				}
			}
			offset_line.startPt=base_line.startPt+vec*pPlank->GetThick()/sina;
			offset_line.endPt=base_line.endPt+vec*pPlank->GetThick()/sina;
			base_line.startPt/=draw_scale;
			base_line.endPt/=draw_scale;
			offset_line.startPt/=draw_scale;
			offset_line.endPt/=draw_scale;
			CreateAcadLine(pBlockTableRecord,base_line.startPt,base_line.endPt,pPlank->handle);
			CreateAcadLine(pBlockTableRecord,offset_line.startPt,offset_line.endPt);
			CreateAcadLine(pBlockTableRecord,base_line.startPt,offset_line.startPt);
			CreateAcadLine(pBlockTableRecord,base_line.endPt,offset_line.endPt);
		}
	}
}
void DrawFooStruView(AcDbBlockTableRecord *pBlockTableRecord, PARTSET& partSet, UCS_STRU &ucs,double scale);
void DrawParamFootPlanform(CLDSParamPlate* pParaBasePlate,double draw_scale,int iViewStyle)
{	//参数化踏脚板的视图	    2015-3-23
	if(pParaBasePlate==NULL || pParaBasePlate->m_iParamType!=TYPE_BASEPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Fail to find the base plate of the specified handle!");
#else
		AfxMessageBox("没有找到指定句柄的底脚板!");
#endif
		return;
	}
	PARTSET plateSet;
	plateSet.append(pParaBasePlate);
	Ta.GetPartGroupPartSet(pParaBasePlate->handle,plateSet);
	//
	LSDRAW.EmptyDrawList();
	InitDrawingEnv();
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);	//获得当前图形的块表指针
	AcDbBlockTableRecord *pBlockTableRecord;				//定义块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);//以写方式打开模型空间，获得块表记录指针
	pBlockTable->close();//关闭块表
	DRAGSET.ClearEntSet();
	long nNormOffset=0;
	pParaBasePlate->GetDesignItemValue('N',&nNormOffset);		//指定法向偏移
	UCS_STRU ucs=pParaBasePlate->ucs;
	ucs.origin+=ucs.axis_z*pParaBasePlate->GetThick();
	if(iViewStyle==4||iViewStyle==5)
	{	//俯视图/仰视图
		if(iViewStyle==5)
		{	//仰视图
			ucs.axis_z*=-1.0;
			ucs.axis_y*=-1.0;
			ucs.origin+=ucs.axis_z*pParaBasePlate->GetThick();
		}
		DrawFootPlanView(pBlockTableRecord,pParaBasePlate->handle,plateSet,ucs,draw_scale);
		//绘制角钢视图
		long hBaseNode=0;
		pParaBasePlate->GetDesignItemValue('A',&hBaseNode);
		for(int i=0;i<2;i++)
		{
			long hBaseJg=0;
			if(i==0)
				pParaBasePlate->GetDesignItemValue('L',&hBaseJg);
			else
				continue;
			CLDSLineAngle *pCommJg=(CLDSLineAngle*)Ta.FromPartHandle(hBaseJg,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pCommJg==NULL)
				continue;
			DrawJgPlanView(pBlockTableRecord,pCommJg,hBaseNode,ucs,draw_scale);
		}
	}
	else	
	{	//绘制正侧面图
		if(iViewStyle==0)	//前面视图
		{
			ucs.axis_x.Set(1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,1,0);
		}
		else if(iViewStyle==1)	//后面视图
		{
			ucs.axis_x.Set(-1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,-1,0);
		}
		else if(iViewStyle==2)	//左面视图
		{
			ucs.axis_x.Set(0,1,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(-1,0,0);
		}
		else //if(iViewStyle==3)	//右面视图
		{
			ucs.axis_x.Set(0,-1,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(1,0,0);
		}
		DrawFooStruView(pBlockTableRecord,plateSet,ucs,draw_scale);
		LSDRAW.DrawToMap(pBlockTableRecord,ucs,draw_scale);
	}
	pBlockTableRecord->close();
	ads_point basepnt;
	basepnt[X] = basepnt[Y] = basepnt[Z] = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point");
#else
	DragEntSet(basepnt,"输入插入点");
#endif
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
//from DrawTaMap.cpp
//void DrawFoot(AcDbBlockTableRecord *pBlockTableRecord,CFoot *pFoot, UCS_STRU &ucs,double scale);
void DrawFootPlanform()
{	
	sys.m_iCurMapType = 3;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    CAcModuleResourceOverride resOverride;
	static CFootViewSelDlg foot_dlg;
	if(foot_dlg.DoModal()!=IDOK)
		return;
	THANDLE hFoot;
	sscanf(foot_dlg.m_hFootHandle,"%X",&hFoot);
	CLDSPart* pPart=(CLDSPart*)Ta.FromPartHandle(hFoot,CLS_PLATE,CLS_PARAMPLATE);
	if(pPart==NULL || !pPart->IsPlate())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Fail to find the base plate of the specified handle!");
#else
		AfxMessageBox("找不到指定句柄的底脚板！");
#endif
		return;
	}
	if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		return DrawParamFootPlanform((CLDSParamPlate*)pPart,foot_dlg.m_nScale,foot_dlg.m_iViewStyle);
	CLDSPlate* pDiBan=(CLDSPlate*)pPart;
	if(pDiBan->designInfo.m_hBasePart<0X20)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("base plates lack of dependent parts!");
#else
		AfxMessageBox("底脚板缺少依赖构件！");
#endif
		return;
	}
	CFoot foot;
	foot.SetBelongModel(&Ta);
	foot.base_node_handle=pDiBan->designInfo.m_hBaseNode;
	foot.attach_jg_handle[0]=pDiBan->designInfo.m_hBasePart;
	foot.CreateRefAxis();
	foot.ucs.origin=pDiBan->ucs.origin+foot.ucs.axis_z*(pDiBan->GetThick()+pDiBan->m_fNormOffset);
	foot.plank_list->append(pDiBan->handle);
	PARTSET plateSet;
	Ta.GetPartGroupPartSet(hFoot,plateSet);
	bool bHoofFoot=false;	//牛蹄板(含底座板上下两个方向连接角钢和连接板
	for(CLDSPart *pPart=plateSet.GetFirst();pPart;pPart=plateSet.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			foot.plank_list->append(pPart->handle);
			if(!bHoofFoot)
			{
				f3dPoint geom_center=CLDSPlate::GetGeomCenter(pPart);
				if((geom_center-foot.ucs.origin)*foot.ucs.axis_z<pDiBan->GetThick())
					bHoofFoot=true;
			}
		}
	}
	if(bHoofFoot)	//查找牛蹄板另一方向的连接角钢
	{
		CSuperSmartPtr<CLDSPart>pBaseAngle;
		pBaseAngle=Ta.FromPartHandle(foot.attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseAngle.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Fail to find the base plate of the specified handle!");
#else
			AfxMessageBox("找不到指定句柄的底脚板！");
#endif
			return;
		}
		f3dPoint base_line_vec=pBaseAngle.LineAnglePointer()->End()-pBaseAngle.LineAnglePointer()->Start();
		normalize(base_line_vec);
		for(pBaseAngle=Ta.Parts.GetFirst(CLS_GROUPLINEANGLE);pBaseAngle.IsHasPtr();pBaseAngle=Ta.Parts.GetNext(CLS_GROUPLINEANGLE))
		{
			if(pBaseAngle->handle==foot.attach_jg_handle[0])
				continue;
			if(pBaseAngle.LinePartPointer()->pStart==NULL||pBaseAngle.LinePartPointer()->pEnd==NULL)
				continue;
			if(pBaseAngle.LinePartPointer()->pStart->handle!=foot.base_node_handle&&
				pBaseAngle.LinePartPointer()->pEnd->handle!=foot.base_node_handle)
				continue;
			f3dPoint line_vec=pBaseAngle.LinePartPointer()->End()-pBaseAngle.LinePartPointer()->Start();
			normalize(line_vec);
			if(fabs(line_vec*base_line_vec)>EPS_COS)
				break;
		}
		if(pBaseAngle.IsHasPtr())
			foot.attach_jg_handle[1]=pBaseAngle->handle;
	}
	//
	plateSet.Empty();
	for(CLDSPlate *pPlank=foot.GetFirstPlank();pPlank;pPlank=foot.GetNextPlank())
		plateSet.append(pPlank);
	double draw_scale=foot_dlg.m_nScale;
	LSDRAW.EmptyDrawList();
	InitDrawingEnv();
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表
	DRAGSET.ClearEntSet();
	if(foot_dlg.m_iViewStyle==4||foot_dlg.m_iViewStyle==5)
	{	//4.俯视图 5.仰视图
		if(foot_dlg.m_iViewStyle==4)
		{
			foot.ucs.origin+=foot.ucs.axis_z*(pDiBan->GetNormOffset()+pDiBan->Thick);
		}
		else if(foot_dlg.m_iViewStyle==5)
		{
			foot.ucs.origin+=foot.ucs.axis_z*pDiBan->GetNormOffset();
			foot.ucs.axis_z*=-1.0;
			foot.ucs.axis_y*=-1.0;
		}
		DrawFootPlanView(pBlockTableRecord,hFoot,plateSet,foot.ucs,draw_scale);
		//绘制角钢俯视/仰视图
		for(int i=0;i<2;i++)
		{
			CLDSLineAngle *pCommJg=(CLDSLineAngle*)Ta.FromPartHandle(foot.attach_jg_handle[i],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pCommJg==NULL)
				continue;
			if(pCommJg==NULL)
				continue;
			DrawJgPlanView(pBlockTableRecord,pCommJg,foot.base_node_handle,foot.ucs,draw_scale);
		}
	}
	else	//绘制正侧面图
	{
		UCS_STRU ucs;
		ucs.origin=foot.ucs.origin;
		if(foot_dlg.m_iViewStyle==0)	//前面视图
		{
			ucs.axis_x.Set(1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,1,0);
		}
		else if(foot_dlg.m_iViewStyle==1)	//后面视图
		{
			ucs.axis_x.Set(-1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,-1,0);
		}
		else if(foot_dlg.m_iViewStyle==2)	//左面视图
		{
			ucs.axis_x.Set(0,1,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(-1,0,0);
		}
		else //if(foot_dlg.m_iViewStyle==3)	//右面视图
		{
			ucs.axis_x.Set(0,-1,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(1,0,0);
		}
		DrawFooStruView(pBlockTableRecord,plateSet,ucs,draw_scale);
		LSDRAW.DrawToMap(pBlockTableRecord,ucs,draw_scale);
	}
	pBlockTableRecord->close();
	ads_point basepnt;
	basepnt[X] = basepnt[Y] = basepnt[Z] = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point");
#else
	DragEntSet(basepnt,"输入插入点");
#endif
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
#else
void SegStru(){;}
void PartGroupStru(){;}
void PartGroupSetupDrawing(){;}
void DrawFoot(){;}
void DrawFootPlanform(){;}
#endif

#ifndef __TSA_FILE_
//选择段号
void SelSegI()
{
    CAcModuleResourceOverride resOverride;
	CSelMapSegDlg dlg;
	dlg.DoModal();
	SEGI iCurSeg = SEGI(dlg.m_sStruSegNo.GetBuffer());
	if(iCurSeg.iSeg<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("input illegal segment number");
#else
		AfxMessageBox("输入了非法段号");
#endif
		Ta.m_iCurSeg = SEGI(1);
	}
	else if(dlg.m_nSegNum<0)	
		Ta.m_iCurSeg = SEGI();
	else
		Ta.m_iCurSeg=iCurSeg;
}
#else
void SelSegI(){;}
#endif
//分段单线图
void SegLine()
{
	//特殊设置赋值
	sys.m_iCurMapType = 3;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	CAcModuleResourceOverride resOverride;
	CSelCurFaceDlg facedlg;
	if(facedlg.DoModal()!=IDOK||facedlg.m_pSelectedView==NULL)
		return;
	CLogErrorLife logErrLife;
	CDisplayView *pCurView = facedlg.m_pSelectedView;
	PARTSET partset;
	NODESET nodeset;
	if(pCurView->m_iViewType==0)
	{
		CHashList<SEGI> hashIncSegs;	//当前绘图所包括的分段集合 wjh-2017.11.15
		for(SEGI* pSegI=facedlg.m_xSegNoList.GetFirst();pSegI;pSegI=facedlg.m_xSegNoList.GetNext())
			hashIncSegs.SetValue(*pSegI,*pSegI);
		for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
#ifndef __TSA_FILE_
			if(pCurView->filter.IsValidLayer(pPart->layer())&&pPart->IsLinePart()&&hashIncSegs.GetValue(pPart->iSeg.iSeg)!=NULL)//==Ta.m_iCurSeg.iSeg)
#else
			if(pCurView->filter.IsValidLayer(pPart->layer())&&pPart->IsLinePart())
#endif
				partset.append(pPart);
		}
		for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pCurView->filter.IsValidLayer(pNode->layer())&&hashIncSegs.GetValue(pNode->iSeg.iSeg)!=NULL)//==Ta.m_iCurSeg.iSeg)
				nodeset.append(pNode);
		}
	}
	else if(pCurView->m_iViewType==1)
	{	//定制视图不用进行段号判断 wht 10-11-23
		for(CViewPart *pViewPart=pCurView->GetFirstViewPart(); pViewPart; pViewPart=pCurView->GetNextViewPart())
		{
			CLDSNode *pNode = Ta.FromNodeHandle(pViewPart->handle);
			if(pNode)
				nodeset.append(pNode);
			CLDSLinePart *pLinePart = (CLDSLinePart*)Ta.FromPartHandle(pViewPart->handle,CLS_LINEPART);
			if(pLinePart)	
				partset.append(pLinePart);
		}
	}
	if(partset.GetNodeNum()<1&&nodeset.GetNodeNum()<1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("%s SegI There are no enough legal rods or nodes, Please check whether to open the right data file!",(char*)Ta.m_iCurSeg.ToString());
#else
		logerr.Log("%s段中没有选到足够的合法杆件或节点,请检查是否打开了正确的数据文件!",(char*)Ta.m_iCurSeg.ToString());
#endif
		return;
	}
	SCOPE_STRU scope;
	GetNodeSetSizeScope(nodeset,scope);
	f3dPoint f_base;
	f_base.x=scope.fMinX+(scope.fMaxX-scope.fMinX)/2.0;
	f_base.y=scope.fMinY+(scope.fMaxY-scope.fMinY)/2.0;
	f_base.z=scope.fMinZ+(scope.fMaxZ-scope.fMinZ)/2.0;
	//
	UCS_STRU ucs=CalDrawingUcs(partset,pCurView,TRUE);
	CInputDrawingUcsDlg dlg;
	dlg.m_fAxisX_X = ucs.axis_x.x;
	dlg.m_fAxisX_Y = ucs.axis_x.y;
	dlg.m_fAxisX_Z = ucs.axis_x.z;
	dlg.m_fAxisZ_X = ucs.axis_z.x;
	dlg.m_fAxisZ_Y = ucs.axis_z.y;
	dlg.m_fAxisZ_Z = ucs.axis_z.z;
	if(dlg.DoModal()==IDOK)
	{
		ucs.axis_x.Set(dlg.m_fAxisX_X,dlg.m_fAxisX_Y,dlg.m_fAxisX_Z);
		ucs.axis_z.Set(dlg.m_fAxisZ_X,dlg.m_fAxisZ_Y,dlg.m_fAxisZ_Z);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
	}
	else
		return;
	for(CLDSNode *pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		pNode->feature=0;
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		pPart->feature=0;
	DRAGSET.ClearEntSet();
	DrawSingleLineZhunMap(partset,nodeset,ucs,sys.general_set.LineMapScale);
#ifdef _ARX_2007
	ads_command(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	ads_command(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
	coord_trans(f_base,ucs,FALSE);
	f_base = f_base*(1.0/sys.general_set.LineMapScale);
	ads_point basepnt;
	basepnt[X] = f_base.x;
	basepnt[Y] = f_base.y;
	basepnt[Z] = f_base.z;
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point");
#else
	DragEntSet(basepnt,"输入插入点");
#endif
}

void InsertMapFrm()
{
    CAcModuleResourceOverride resOverride;
	CNewMapFrameDlg map_dlg;
	map_dlg.m_bNewMapFrm=FALSE;
	if(map_dlg.DoModal()!=IDOK)
		return;
	f3dPoint basepnt;
	InitDrawingEnv();
	//获得当前图形的块表指针
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//获得当前图形块表记录指针
	AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
	//以写方式打开模型空间，获得块表记录指针
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//关闭块表
	DRAGSET.ClearEntSet();
	CXhChar100 sMapFrmName;
	sMapFrmName.Printf("%s",map_dlg.m_sCmbMapFrmName);
	if(!InsertBlock(pBlockTableRecord,basepnt,(char*)sMapFrmName,1.0,0,0))
	{
		MAP_FRAME_SIZE* pMapFrm=sys.m_hashMapFrameList.GetValue((char*)sMapFrmName);
		if(pMapFrm)//插入自定义图框 wht 16-01-28
		{
			int i=1,nUnitSize=100;
			f3dPoint topLeft;
			//内侧图框
			CreateAcadRect(pBlockTableRecord,topLeft,pMapFrm->nLength-pMapFrm->nLeftMargin-pMapFrm->nRightMargin
				,pMapFrm->nWidth-pMapFrm->nTopMargin-pMapFrm->nBottomMargin
				,FALSE,NULL,f3dPoint(0,0,0),sys.stru_map.fPolyLineWidth);
			double txt_height=min(pMapFrm->nTopMargin,pMapFrm->nBottomMargin)*0.5;
			for(i=1;i<=pMapFrm->GetInsideLen()/nUnitSize;i++)
			{	//上侧刻度线
				f3dPoint start(topLeft.x+i*nUnitSize,topLeft.y);
				f3dPoint end(start.x,topLeft.y+pMapFrm->nTopMargin);
				f3dPoint dim_pos(start.x-0.5*nUnitSize,start.y+0.5*txt_height);
				CreateAcadLine(pBlockTableRecord,start,end);
				CXhChar16 dim_txt("%d",i);
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
				//下侧刻度线
				start.y-=(pMapFrm->GetInsideWidth()+pMapFrm->nBottomMargin);
				end.y=start.y+pMapFrm->nBottomMargin;
				dim_pos.y=start.y+0.5*txt_height;
				CreateAcadLine(pBlockTableRecord,start,end);
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
			}
			txt_height=min(pMapFrm->nLeftMargin,pMapFrm->nRightMargin)*0.5;
			for(i=1;i<=pMapFrm->GetInsideWidth()/nUnitSize;i++)
			{	//左侧刻度线
				f3dPoint start(topLeft.x,topLeft.y-i*nUnitSize);
				f3dPoint end(start.x-pMapFrm->nLeftMargin,start.y);
				f3dPoint dim_pos(start.x-0.5*txt_height,start.y+0.5*nUnitSize);
				CreateAcadLine(pBlockTableRecord,start,end);
				CXhChar16 dim_txt("%c",(int)('A'+i-1));
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextRight,AcDb::kTextVertMid);
				//右侧刻度线
				start.x+=pMapFrm->GetInsideLen();
				end.x=start.x+pMapFrm->nRightMargin;
				dim_pos.x=start.x+0.5*txt_height;
				CreateAcadLine(pBlockTableRecord,start,end);
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextLeft,AcDb::kTextVertMid);
			}
			//外侧图框
			topLeft.x-=pMapFrm->nLeftMargin;
			topLeft.y+=pMapFrm->nTopMargin;
			CreateAcadRect(pBlockTableRecord,topLeft,pMapFrm->nLength,pMapFrm->nWidth
				,FALSE,NULL,f3dPoint(0,0,0),sys.stru_map.fPolyLineWidth);
		}
		else
		{
			pBlockTableRecord->close();
			return;
		}
	}
	pBlockTableRecord->close();
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point");
#else
	DragEntSet(basepnt,"输入插入点");
#endif
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
//插入钢印图块
#include "InputMKRectDlg.h"
AcDbObjectId SearchBlock(const char *blkname);
void InsertMKRect()
{
	CAcModuleResourceOverride resOverride;
	CXhChar16 blkname("MK");
	AcDbObjectId blockId=SearchBlock(blkname);
	if(blockId.isNull())
	{	//新建块表记录指针，生成钢印标记图元
		double txt_height=2;
		int nRectL=60,nRectW=30;
		CInputMKRectDlg dlg;
		dlg.m_nRectL=nRectL;
		dlg.m_nRectW=nRectW;
		dlg.m_fTextH=txt_height;
		if(dlg.DoModal()==IDOK)
		{
			nRectL=dlg.m_nRectL;
			nRectW=dlg.m_nRectW;
			txt_height=dlg.m_fTextH;
		}
		DRAGSET.ClearEntSet();
		AcDbBlockTable *pTempBlockTable;
		GetCurDwg()->getBlockTable(pTempBlockTable,AcDb::kForWrite);
		AcDbBlockTableRecord *pTempBlockTableRecord=new AcDbBlockTableRecord();//定义块表记录指针
#ifdef _ARX_2007
		pTempBlockTableRecord->setName((ACHAR*)_bstr_t(CXhChar16("MK")));
#else
		pTempBlockTableRecord->setName(CXhChar16("MK"));
#endif
		pTempBlockTable->add(blockId,pTempBlockTableRecord);
		pTempBlockTable->close();
		//生成钢印标记图元
		f3dPoint topLeft(0,nRectW),dim_pos(nRectL*0.5,nRectW*0.5,0);
		CreateAcadRect(pTempBlockTableRecord,topLeft,nRectL,nRectW);	//插入矩形区
		DimText(pTempBlockTableRecord,dim_pos,CXhChar16("钢印区"),TextStyleTable::hzfs.textStyleId,txt_height,0,AcDb::kTextCenter,AcDb::kTextVertMid);
		pTempBlockTableRecord->close();
	}
	//将钢印图元生成图块，提示用户插入指定位置
	if(LayerTable::DimSizeLayer.layerId.isValid())
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	AcDbBlockTableRecord* pBlockTableRecord=GetBlockTableRecord();
	while(1)
	{
		DRAGSET.ClearEntSet();
		f3dPoint insert_pos;
		AcDbObjectId newEntId;
		AcDbBlockReference *pBlkRef = new AcDbBlockReference;
		AcGeScale3d scaleXYZ(1.0,1.0,1.0);
		pBlkRef->setBlockTableRecord(blockId);
		pBlkRef->setPosition(AcGePoint3d(insert_pos.x,insert_pos.y,0));
		pBlkRef->setRotation(0);
		pBlkRef->setScaleFactors(scaleXYZ);
		DRAGSET.AppendAcDbEntity(pBlockTableRecord,newEntId,pBlkRef);
		pBlkRef->close();
#ifdef AFX_TARG_ENU_ENGLISH
		int retCode=DragEntSet(insert_pos,"input the insertion point\n");
#else
		int retCode=DragEntSet(insert_pos,"输入插入点\n");
#endif
		if(retCode==RTCAN)
			break;
	}
	pBlockTableRecord->close();
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
#ifdef __SMART_DRAW_INC_
//插入接头包板俯视图
#include "InputPartNoDlg.h"
void InsertWrapperSideView()
{
	//选择需要绘制接头俯视图的短角钢
	CInputPartNoDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return;
	CXhChar50 sPartNo(dlg.m_sPartNo);
	CLDSLineAngle* pShortJg=(CLDSLineAngle*)Ta.FromPartNo(sPartNo);
	if(pShortJg==NULL || (pShortJg->pStart&&pShortJg->pEnd))
		return;
	CJointWrapperView xWrapperView;
	xWrapperView.m_pShortJg=pShortJg;
	xWrapperView.CreateDrawing();
	//绘制视图图元
	DRAGSET.ClearEntSet();
	double zoomdraw_coef=0.1;
	f3dPoint base_pnt;
	AcDbBlockTableRecord* pBlockTableRecord=GetBlockTableRecord();
	for(f3dArcLine *pLine=xWrapperView.EnumFirstArc();pLine;pLine=xWrapperView.EnumNextArc())
	{
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		f3dArcLine arcline=*pLine;
		arcline.ZoomScale(zoomdraw_coef);
		if(arcline.SectorAngle()>0)
			CreateAcadArcLine(pBlockTableRecord,arcline.Center(),arcline.Start(),arcline.SectorAngle(),arcline.WorkNorm());
		else 
			CreateAcadLine(pBlockTableRecord,arcline.Start(),arcline.End());
	}
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	DimStyleTable::dimStyle.InitDimStyle(sys.fonts.segment.fDimTextSize);
	DimStyleTable::dimStyle4.InitDimStyle(sys.fonts.segment.fDimTextSize);
	for(CSizeTextDim* pSizeDim=xWrapperView.EnumFirstSizeDim();pSizeDim;pSizeDim=xWrapperView.EnumNextSizeDim())
	{
		pSizeDim->dimStartPos*=zoomdraw_coef;
		pSizeDim->dimEndPos*=zoomdraw_coef;
		pSizeDim->dimPos*=zoomdraw_coef;
		AcDbObjectId dimStyleId=DimStyleTable::dimStyle.dimStyleId;
		if(pSizeDim->angle==0)
			dimStyleId=DimStyleTable::dimStyle4.dimStyleId;
		DimAngleSize(pBlockTableRecord,pSizeDim->dimStartPos,pSizeDim->dimEndPos,pSizeDim->dimPos,pSizeDim->DimText(),dimStyleId,pSizeDim->angle);
	}
	//根据数据点位置标注构件编号
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(CTextFrame* pDataPoint=xWrapperView.EnumFirstTextFrm();pDataPoint;pDataPoint=xWrapperView.EnumNextTextFrm())
	{
		f3dPoint data_pos=pDataPoint->dataPos;
		f3dPoint dim_pos=pDataPoint->dimPos;
		f3dPoint refEndPos=pDataPoint->GetRefLineEndPos(1.0);
		dim_pos*=zoomdraw_coef;
		refEndPos*=zoomdraw_coef;
		data_pos*=zoomdraw_coef;
		double fTextSize=sys.fonts.segment.fPartNoTextSize*zoomdraw_coef;
		double fHalfWidth=0.5*pDataPoint->Width()*zoomdraw_coef;
		double fHalfHeight=0.5*pDataPoint->Height()*zoomdraw_coef;
		CreateAcadLine(pBlockTableRecord,data_pos,refEndPos);
		f3dPoint text_pos(dim_pos.x,dim_pos.y+0.5*fTextSize);
		DimText(pBlockTableRecord,text_pos,pDataPoint->Text(),TextStyleTable::hzfs.textStyleId,fTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
		CreateAcadCircle(pBlockTableRecord,dim_pos,fHalfWidth);
	}
	pBlockTableRecord->close();
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(base_pnt,"input the insertion point\n");
#else
	DragEntSet(base_pnt,"输入插入点\n");
#endif
}
#endif
#include "BomCfgDlg.h"
#include "InputAnValDlg.h"
#include "RecogPartBom.h"
struct TEXT_CIRCLE{
	bool bHasDualLabel;
	BYTE cnLeastLabelCount;
	double fRadius;
	double fCenterX,fCenterY;
	//
	TEXT_CIRCLE(){fRadius=fCenterX=fCenterY=0;bHasDualLabel=false;cnLeastLabelCount=1;}
	BOOL IsInCircle(f3dPoint pt)
	{
		double RADIUS=0.7*fRadius;
		if(fabs(pt.x-fCenterX)<RADIUS&&fabs(pt.y-fCenterY)<RADIUS)
			return TRUE;
		else
			return FALSE;
	}
};
void CheckDwgFilePartNo()
{
	CAcModuleResourceOverride resOverride;
	//导入BOM文件
	if(bom.Parts.GetNodeNum()<=0)
	{
		CBomCfgDlg bomDlg;
		if(bomDlg.DoModal()==IDOK)
			bom.CopyFrom(bomDlg.m_xBomObj);
	}
	if(bom.Parts.GetNodeNum()<=0)
		return;
	for(CManuPart* pPart=bom.Parts.GetFirst();pPart;pPart=bom.Parts.GetNext())
		pPart->nCount=0;
	//待检测段号
	CInputAnIntegerValDlg dlg;
	dlg.m_sValTitle="输入待检测段号:";
	if(dlg.DoModal()!=IDOK)
		return;
	int iSelSeg=dlg.m_nVal;
	//选择当前打开DWG文件中的所有图元
	CHashSet<AcDbObjectId> xTextEntIdSet;
	CXhSimpleList<TEXT_CIRCLE> listDimCircles;
	AcDbObjectId entId;
	AcDbEntity *pEnt=NULL;
	ads_name ent_sel_set,entname;
#ifdef _ARX_2007
	acedSSGet(L"ALL",NULL,NULL,NULL,ent_sel_set);
#else
	acedSSGet("ALL",NULL,NULL,NULL,ent_sel_set);
#endif
	long ll;
	acedSSLength(ent_sel_set,&ll);
	for(long i=0;i<ll;i++)
	{	//初始化实体集合
		acedSSName(ent_sel_set,i,entname);
		acdbGetObjectId(entId,entname);
		acdbOpenAcDbEntity(pEnt,entId,AcDb::kForRead);
		if(pEnt==NULL)
			continue;
		if(pEnt->isKindOf(AcDbCircle::desc()))
		{	//件号标注圆圈(R=4|6|7)
			AcDbCircle* pCircle=(AcDbCircle*)pEnt;
			double fDiametre=pCircle->radius()*2;
			f3dPoint center(pCircle->center().x,pCircle->center().y,pCircle->center().z);
			//if(fDiametre==sys.dim_map.PartNo.fPartNoCirD||fDiametre==sys.dim_map.PartNo.fMainPoleNoOuterCirD)
			if(fDiametre>=4.5&&fDiametre<=sys.dim_map.PartNo.fMainPoleNoInnerCirD)
			{	//普通编号圆|主材编号外圆
				TEXT_CIRCLE* pTextCir=listDimCircles.AttachObject();
				pTextCir->fCenterX=center.x;
				pTextCir->fCenterY=center.y;
				pTextCir->fRadius=pCircle->radius();
			}
		}
		if(pEnt->isKindOf(AcDbText::desc())||pEnt->isKindOf(AcDbMText::desc()))
			xTextEntIdSet.SetValue(entId.handle(),entId);
		pEnt->close();
	}
	acedSSFree(ent_sel_set);
	//提取材料表中的构件编号
	CHashStrList <CManuPart> hashPartNo;
	while(1)
	{
		f3dPoint origin;
		ads_point org_L_T,org_R_B;
#ifdef _ARX_2007
		if(ads_getpoint(NULL,L"\n请选择构件明细表的左上角,<Enter退出>: ",org_L_T)!=RTNORM)
			break;
		if(ads_getpoint(NULL,L"\n请选择构件明细表的右下角,<Enter退出>: ",org_R_B)!=RTNORM)
			break;
		ads_command(RTSTR,L"ZOOM",RTSTR,L"e",RTNONE);	//不全显示可能导致未显示在屏幕中的区域提取失败 wjh-2016.12.16
#else
		if(ads_getpoint(NULL,"\n请选择构件明细表的左上角,<Enter退出>: ",org_L_T)!=RTNORM)
			break;
		if(ads_getpoint(NULL,"\n请选择构件明细表的右下角,<Enter退出>: ",org_R_B)!=RTNORM)
			break;
		ads_command(RTSTR,"ZOOM",RTSTR,"e",RTNONE);
#endif
		double gap=0.5;
		GEPOINT rgn_vert[4];
		rgn_vert[0].Set(org_L_T[0]-gap,org_L_T[1]+gap);
		rgn_vert[1].Set(org_L_T[0]-gap,org_R_B[1]-gap);
		rgn_vert[2].Set(org_R_B[0]+gap,org_R_B[1]-gap);
		rgn_vert[3].Set(org_R_B[0]+gap,org_L_T[1]+gap);
		resbuf verts4[4];
		for(int i=0;i<4;i++)
		{
			verts4[i].restype=5002;
			verts4[i].resval.rpoint[X] = rgn_vert[i].x;
			verts4[i].resval.rpoint[Y] = rgn_vert[i].y;
			verts4[i].resval.rpoint[Z] = rgn_vert[i].z;
			if(i<3)
				verts4[i].rbnext=&verts4[i+1];
			else
				verts4[3].rbnext=NULL;
		}
		ads_name ss_name,entname;
#if defined(_ARX_2007)&&!defined(_ZRX_2012)
		if (acedSSGet(L"cp",&verts4[0],NULL,NULL,ss_name)!=RTNORM)
#else
		if (acedSSGet("cp",&verts4[0],NULL,NULL,ss_name)!=RTNORM)
#endif
		{
			acedSSFree(ss_name);
			break;
		}
#ifdef _ARX_2007
		ads_command(RTSTR,L"ZOOM",RTSTR,"P",RTNONE);
#else
		ads_command(RTSTR,"ZOOM",RTSTR,"P",RTNONE);
#endif
		CPartBomTable xPartBomTable(rgn_vert);
		long ll=0;
		acedSSLength(ss_name,&ll);
		for(i=0;i<ll;i++)
		{
			AcDbObjectId entId;
			AcDbEntity *pEnt;
			acedSSName(ss_name,i,entname);
			acdbGetObjectId(entId,entname);
			acdbOpenObject(pEnt,entId,AcDb::kForRead);
			if(pEnt==NULL)
				continue;
			pEnt->close();
			xPartBomTable.m_xSelEntIdSet.SetValue(entId.handle(),entId);
		}
		acedSSFree(ss_name);
		xPartBomTable.RecogizeBomTable();
		xPartBomTable.RetrieveParts();
		for(CManuPart* pBomPart=xPartBomTable.EnumFirstPart();pBomPart;pBomPart=xPartBomTable.EnumNextPart())
		{
			CManuPart* pTemPart=hashPartNo.GetValue(pBomPart->sPartNo);
			if(pTemPart!=NULL)
				pTemPart->nCount++;
			else
			{
				pTemPart=hashPartNo.SetValue(pBomPart->sPartNo,*pBomPart);
				pTemPart->nCount=1;
			}
		}
	}
	//从所有图元中检测件号情况，匹配所有件号圆圈
	ARRAY_LIST<CXhChar50> partNoArr;
	partNoArr.SetSize(0,listDimCircles.GetCount()*2);
	for(AcDbObjectId objId=xTextEntIdSet.GetFirst();objId;objId=xTextEntIdSet.GetNext())
	{
		acdbOpenAcDbEntity(pEnt,objId,AcDb::kForRead);
		if(pEnt)
			pEnt->close();
		if(!pEnt->isKindOf(AcDbText::desc()))
			continue;
		AcDbText* pText=(AcDbText*)pEnt;
#ifdef _ARX_2007
		CXhChar50 sText(_bstr_t(pText->textString()));
#else
		CXhChar50 sText(pText->textString());
#endif
		double fWidth=TestDrawTextLength(sText,pText->height(),pText->textStyle());
		f3dPoint dim_norm(-sin(pText->rotation()),cos(pText->rotation()));
		f3dPoint dim_vec(cos(pText->rotation()),sin(pText->rotation()));
		f3dPoint pt(pText->position().x,pText->position().y,pText->position().z);
		pt+=dim_vec*fWidth*0.5;
		pt+=dim_norm*pText->height()*0.5;
		//
		TEXT_CIRCLE* pTextCir=NULL;
		for(pTextCir=listDimCircles.EnumObjectFirst();pTextCir;pTextCir=listDimCircles.EnumObjectNext())
		{
			if(pTextCir->cnLeastLabelCount>0&&pTextCir->IsInCircle(pt))
			{
				if(strstr(sText,"前"))
				{
					sText.Replace("前","");
					if(!pTextCir->bHasDualLabel)	//双编号中的第一个编号标注
						pTextCir->bHasDualLabel=true;
					else	//双编号中的第二个编号标注
						pTextCir->cnLeastLabelCount=0;
				}
				else if(strstr(sText,"后"))
				{
					sText.Replace("后","");
					if(!pTextCir->bHasDualLabel)	//双编号中的第一个编号标注
						pTextCir->bHasDualLabel=true;
					else	//双编号中的第二个编号标注
						pTextCir->cnLeastLabelCount=0;
				}
				else	//单编号
					pTextCir->cnLeastLabelCount=0;
				if(pTextCir->cnLeastLabelCount==0||!pTextCir->bHasDualLabel)
					listDimCircles.DeleteCursor();
				partNoArr.append(sText);
				break;
			}
		}
	}
	for(CManuPart* pPart=bom.Parts.GetFirst();pPart;pPart=bom.Parts.GetNext())
	{
		if(pPart->nSeg.iSeg!=iSelSeg)
			continue;
		for(int i=0;i<partNoArr.GetSize();i++)
		{
			if(stricmp(partNoArr[i],pPart->sPartNo)==0)
				pPart->nCount++;
		}
	}
	//从材料表中检测件号情况
	CLogErrorLife logErrLife;
	for(CManuPart* pPart=bom.Parts.GetFirst();pPart;pPart=bom.Parts.GetNext())
	{
		if(pPart->nSeg.iSeg!=iSelSeg)
			continue;
		CXhChar500 line_text(" %s\t%s\t\t次数:%d\t\t",pPart->sPartNo,pPart->sSpec,pPart->nCount);
		CManuPart* pTemPart=hashPartNo.GetValue(pPart->sPartNo);
		if(pTemPart)
		{
			line_text.Append(CXhChar100("明细表:%d\t",pTemPart->nCount));
			if(stricmp(pTemPart->sSpec,pPart->sSpec)!=0)
				line_text.Append(CXhChar100("规格:%s≠%s\t\t",pTemPart->sSpec,pPart->sSpec));
			if(fabs(pTemPart->length-pPart->length)>1)
				line_text.Append(CXhChar100("长度误差:%.2f\t\t",fabs(pTemPart->length-pPart->length)));
			if(pTemPart->nPartNum!=pPart->nPartNum)
				line_text.Append(CXhChar100("件数差:%d\t\t",abs(pTemPart->nPartNum-pPart->nPartNum)));
			if(fabs(pTemPart->singWeight-pPart->singWeight)>0.1)
				line_text.Append(CXhChar100("单重误差:%.2f",fabs(pTemPart->singWeight-pPart->singWeight)));
			if(fabs(pTemPart->sumWeight-pPart->sumWeight)>0.1)
				line_text.Append(CXhChar100("总重误差:%.2f",fabs(pTemPart->sumWeight-pPart->sumWeight)));
		}
		logerr.Log(line_text);
	}
}
//移到角钢到前台
void MoveTop()
{
	while(1)
	{
		f3dLine line;
		ads_name entres;
		ads_point ptres;
#ifdef _ARX_2007
		int retCode=ads_entsel(L"\n选择待置顶的角钢心线,<Enter退出>:",entres,ptres);
#else
		int retCode=ads_entsel( "\n选择待置顶的角钢心线,<Enter退出>:",entres,ptres);
#endif
		if(retCode==RTCAN)
			return;
		//1.根据line识别一定范围内的始末端端线（条件：与心线垂直；一定纵向及横向范围内；始末端一至横向范围一致）
		//2.根据识别出的端线识别出两端六点（每端三点）
		//3.根据line识别出心线外的三线（两侧轮廓线<在0图层pline>及肢厚线<在1图层>）
		//4.根据@3识别出的三线及心线识别出两端八点（每端四点）
		//5.提取出角钢轮廓点集合ptlist及轮廓集合肢厚线集合（仅处理外铁，里铁可传入NULL)
	}
}

void TruncateAndHideLines(AcDbBlockTableRecord* pBlockRecord,const double* startCoords,const double* endCoords,IXhArray<AcDbObjectId>& lineArr, char ciRemove0Truncate1)
{
	GEPOINT start(startCoords[0],startCoords[1],0),end(endCoords[0],endCoords[1]);
	GEPOINT lineStdVec=end-start;
	double length=lineStdVec.mod();
	lineStdVec/=length;
	AcDbEntity *pEnt=NULL;
	for(UINT i=0;i<lineArr.Size();i++)
	{
		acdbOpenObject(pEnt,lineArr.At(i),AcDb::kForWrite);
		if(pEnt==NULL)
			continue;
		CAcDbObjLife objLife(pEnt);
		if(pEnt->isKindOf(AcDbLine::desc()))
		{
			AcDbLine* pLine=(AcDbLine*)pEnt;
			AcGePoint3d xCadStart=pLine->startPoint();
			AcGePoint3d xCadEnd = pLine->endPoint();
			GEPOINT xStart(&xCadStart.x),xEnd(&xCadEnd.x);
			double justS=(xStart-start)*lineStdVec;
			double justE=(xEnd-start)*lineStdVec;
			if(justS>justE)
			{	//调整CAD线段与基准线方向一致
				pLine->setStartPoint(xCadEnd);
				pLine->setEndPoint(xCadStart);
				xStart=GEPOINT(&xCadEnd.x);
				xEnd=GEPOINT(&xCadStart.x);
				double temp=justE;
				justE=justS;
				justS=temp;
			}
			if(ciRemove0Truncate1==0)
			{
				if(justS>-EPS2&&justS<length+EPS2&&justE>-EPS2&&justE<length+EPS2)
					pLine->erase();	//删除
			}
			else if(ciRemove0Truncate1==1)
			{	//截出其中的部分作为消隐线，未在当前范围的部分依旧保持原状态
				if(justS<-EPS2)
				{
					pLine->setEndPoint(AcGePoint3d(start.x,start.y,0));
					if(justE>length+EPS2)
					{
						GetCurDwg()->setClayer(pLine->layerId());
						CreateAcadLine(pBlockRecord,end,xEnd);
					}
				}
				else if(justE>length+EPS2)
					pLine->setStartPoint(AcGePoint3d(end.x,end.y,0));
			}
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			AcDbPolyline* pPolyLine=(AcDbPolyline*)pEnt;
			if(pPolyLine->numVerts()!=2)
				continue;
			AcGePoint3d xCadStart,xCadEnd;
			pPolyLine->getPointAt(0,xCadStart);
			pPolyLine->getPointAt(1,xCadEnd);
			GEPOINT xStart(&xCadStart.x),xEnd(&xCadEnd.x);
			double justS=(xStart-start)*lineStdVec;
			double justE=(xEnd-start)*lineStdVec;
			if(justS>justE)
			{	//调整CAD线段与基准线方向一致
				pPolyLine->setPointAt(0,AcGePoint2d(xCadEnd.x,xCadEnd.y));
				pPolyLine->setPointAt(1,AcGePoint2d(xCadStart.x,xCadStart.y));
				xStart=GEPOINT(&xCadEnd.x);
				xEnd=GEPOINT(&xCadStart.x);
				double temp=justE;
				justE=justS;
				justS=temp;
			}
			if(ciRemove0Truncate1==0)
			{
				if(justS>-EPS2&&justS<length+EPS2&&justE>-EPS2&&justE<length+EPS2)
					pPolyLine->erase();	//删除
			}
			else if(ciRemove0Truncate1==1)
			{	//截出其中的部分作为消隐线，未在当前范围的部分依旧保持原状态
				if(justS<-EPS2)
				{
					pPolyLine->setPointAt(1,AcGePoint2d(start.x,start.y));
					if(justE>length+EPS2)
					{
						GetCurDwg()->setClayer(pPolyLine->layerId());
						CreateAcadPolyLine(pBlockRecord,end,xEnd,sys.stru_map.fPolyLineWidth);
					}
				}
				else if(justE>length+EPS2)
					pPolyLine->setPointAt(0,AcGePoint2d(end.x,end.y));
				else if(justS>-EPS2&&justS<length+EPS2&&justE>-EPS2&&justE<length+EPS2)
					pPolyLine->erase();	//删除

			}
		}
	}
}
static BOOL ProcessLineByVertexs(AcDbBlockTableRecord *pBlockRecord,const double* startCoords,const double* endCoords,BYTE ciTypeHide0Frame1Line2)
{	//1.根据始终端节点生成选择区域轮廓点链表
	f3dPoint startPt(startCoords),endPt(endCoords);
	f3dPoint cur_vec,line_vec=endPt-startPt;
	normalize(line_vec);
	f3dPoint norm_vec(-line_vec.y,line_vec.x,0);
	fPtList vertex_list;
	const int EXTEND_LEN=1;
	f3dPoint start_extend=startPt;//-line_vec*EXTEND_LEN;
	f3dPoint end_extend=endPt;//+line_vec*EXTEND_LEN;
	vertex_list.append(start_extend-norm_vec*EXTEND_LEN);
	vertex_list.append(end_extend-norm_vec*EXTEND_LEN);
	vertex_list.append(end_extend+norm_vec*EXTEND_LEN);
	vertex_list.append(start_extend+norm_vec*EXTEND_LEN);
	//2.根据轮廓点链表选择关联的CAD实体
	AcDbObjectId entId;
	ads_name ss_name,ent_name;
	struct resbuf *poly=NULL,*plist=NULL,*filter;
	for(f3dPoint *pPt=vertex_list.GetFirst();pPt;pPt=vertex_list.GetNext())
	{	//应使用acutNewRb()函数而不是new struct resbuf新建返回值缓存，否则使用acutRelRb()释放内存时出错 wht 12-09-21
		if(plist==NULL)
			poly=plist=acutNewRb(RTPOINT);	//new struct resbuf;//acutBuildList(RTDXF0,"LINE",0);
		else
		{
			plist->rbnext=acutNewRb(RTPOINT);//new struct resbuf;
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
#if defined(_ARX_2007)&&!defined(_ZRX_2012)
	filter = acutBuildList(RTDXF0,L"lwpolyline",8,(ACHAR*)_bstr_t((char*)LayerTable::VisibleProfileLayer.layerName),0);
	int retCode=acedSSGet(L"cp",poly,NULL,NULL,ss_name);////NULL,NULL,NULL,NULL,ss_name);
#else
	filter = acutBuildList(8,(char*)LayerTable::VisibleProfileLayer.layerName,0);
	int retCode=acedSSGet("cp",poly,NULL,NULL,ss_name);////NULL,NULL,NULL,NULL,ss_name);
#endif
	acutRelRb(poly);
	acutRelRb(filter);
	if(retCode!=RTNORM)
		return FALSE;
	ARRAY_LIST<AcDbObjectId> lineIdArr;
	ARRAY_LIST<AcDbObjectId> polyLineIdArr;
	long i=0,ll=0;
	acedSSLength(ss_name,&ll);
	for(int i=0;i<ll;i++)
	{
		AcDbEntity *pEnt=NULL;
		AcDbLine *pLine=NULL;
		AcDbPolyline *pPolyline=NULL;
		acedSSName(ss_name,i,ent_name);
		acdbGetObjectId(entId,ent_name);
		acdbOpenObject(pEnt,entId,AcDb::kForWrite);
		bool bFindLine=false;
		AcGePoint3d acad_start,acad_end;
		AcDbPolyline::SegType segType=AcDbPolyline::kLine;
		if(pEnt->isKindOf(AcDbLine::desc()))
		{
			pLine=(AcDbLine*)pEnt;
			Sub_Pnt(cur_vec,pLine->startPoint(),pLine->endPoint());
			normalize(cur_vec);
			if(fabs(cur_vec*line_vec)>EPS_COS)
				lineIdArr.append(entId);
		}
		else if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			pPolyline=(AcDbPolyline*)pEnt;
			pPolyline->getPointAt(0,acad_start);
			pPolyline->getPointAt(1,acad_end);
			Sub_Pnt(cur_vec,acad_start,acad_end);
			normalize(cur_vec);
			if(fabs(cur_vec*line_vec)>EPS_COS)
				polyLineIdArr.append(entId);
		}
		pEnt->close();
	}
	//3.处理区域内直线
	if(ciTypeHide0Frame1Line2>0)
	{	//移除线段区域内原始线条集合
		TruncateAndHideLines(pBlockRecord,startPt,endPt,lineIdArr,0);
		//移除线段后再对应区域绘制直线
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(ciTypeHide0Frame1Line2==1)
			CreateAcadPolyLine(pBlockRecord,startPt,endPt,sys.stru_map.fPolyLineWidth);
		else if(ciTypeHide0Frame1Line2==2)
			CreateAcadLine(pBlockRecord,startPt,endPt);
	}
	else //if(ciTypeHide0Frame1Line2==0)
	{	//截断处理相应直线
		if(lineIdArr.Size()>0)
			TruncateAndHideLines(pBlockRecord,startPt,endPt,lineIdArr,1);
		if(polyLineIdArr.Size()>0)
			TruncateAndHideLines(pBlockRecord,startPt,endPt,polyLineIdArr,1);
		//绘制消隐线
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		//截取直线的一段进行消隐
		double fNewLineLen=DISTANCE(startPt,endPt)*(1.0/3.0);
		f3dPoint startOffsetPt=startPt+line_vec*fNewLineLen;
		f3dPoint endOffsetPt=endPt-line_vec*fNewLineLen;
		CreateAcadLine(pBlockRecord,startPt,startOffsetPt);
		CreateAcadLine(pBlockRecord,endOffsetPt,endPt);
	}
	return TRUE;
}

static BYTE g_ciTypeHide0Frame1Line2=0;
void ProcessLine()
{	//1.选择轮廓点集合
	ads_point snap_pt;
	fPtList ptList;
	while(1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\n Please pick any point in order,<Enter Exit>:",snap_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\n Please pick any point in order,<Enter Exit>:",snap_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if(acedGetPoint(NULL,L"\n请顺序捕捉任一点,<Enter退出>:",snap_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\n请顺序捕捉任一点,<Enter退出>:",snap_pt)==RTNORM)
#endif
#endif
		{
			ptList.append(snap_pt[X],snap_pt[Y],snap_pt[Z]);
			continue;
		} 
		else
		{		
			if(ptList.GetNodeNum()<2)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if (IDNO==AfxMessageBox("\n The selected nodes less than 2,exit or not?",MB_YESNO))
#else
				if (IDNO==AfxMessageBox("\n选中点少于2个，退出吗?",MB_YESNO))
#endif
					continue;
				else
					return;
			}
			break;
		}
	}
	CXhChar100 sPrompt;
#ifdef AFX_TARG_ENU_ENGLISH
	sPrompt.Printf("\nSelect Process Type[0.Hide Line/1.Draw Poly Line/2.Draw Line]<%d>:",g_ciTypeHide0Frame1Line2);
#else
	sPrompt.Printf("\n选择处理类型[0.消隐/1.画多段线/2.画直线]<%d>:",g_ciTypeHide0Frame1Line2);
#endif
	int retCode=0;
#ifdef _ARX_2007
	ACHAR sResult[20]={0};
	retCode=acedGetString(FALSE,_bstr_t(sPrompt),sResult);
	if(wcsicmp(sResult,L"0")==0)
		g_ciTypeHide0Frame1Line2=0;
	else if(wcsicmp(sResult,L"1")==0)
		g_ciTypeHide0Frame1Line2=1;
	else if(wcsicmp(sResult,L"2")==0)
		g_ciTypeHide0Frame1Line2=2;
#else
	char sResult[20]="";
	retCode=acedGetString(FALSE,sPrompt,sResult);
	if(stricmp(sResult,"0")==0)
		g_ciTypeHide0Frame1Line2=0;
	else if(stricmp(sResult,"1")==0)
		g_ciTypeHide0Frame1Line2=1;
	else if(stricmp(sResult,"2")==0)
		g_ciTypeHide0Frame1Line2=2;
#endif
	if(g_ciTypeHide0Frame1Line2<0||g_ciTypeHide0Frame1Line2>2)
		return;
	AcDbBlockTableRecord *pBlockRecord=GetBlockTableRecord();
	//2.遍历轮廓点处理轮廓点关联直线
	f3dPoint *pPt=NULL,*pPrevPt=ptList.GetFirst();
	for(pPt=ptList.GetNext();pPt;pPt=ptList.GetNext())
	{
		ProcessLineByVertexs(pBlockRecord,*pPrevPt,*pPt,g_ciTypeHide0Frame1Line2);
		pPrevPt=pPt;
	}
	pBlockRecord->close();
}
//base为ucs相对坐标系的坐标值
static void DrawFaceLineMap(NODESET &nodeset, PARTSET &partset,const UCS_STRU &ucs,f3dPoint base)
{
	double scale = sys.general_set.LineMapScale;
	ads_point basepnt;
	coord_trans(base,ucs,FALSE);
	basepnt[X]=base.x/scale; 
	basepnt[Y]=base.y/scale;
	basepnt[Z]=0;//base.z/scale;
	DRAGSET.ClearEntSet();
	DrawSingleLineZhunMap(partset,nodeset,ucs,scale,TRUE);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point");
#else
	DragEntSet(basepnt,"输入插入点");
#endif
	seg_map_base_face_ucs = ucs;
}
//
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
static void DrawFaceStruMap(NODESET &nodeset, PARTSET &partset,const UCS_STRU &ucs,f3dPoint base)
{
	double scale = sys.general_set.StruMapScale.fRealToDraw;
	ads_point basepnt;
	coord_trans(base,ucs,FALSE);
	basepnt[X]=base.x/scale; 
	basepnt[Y]=base.y/scale;
	basepnt[Z]=0;//base.z/scale;
	DRAGSET.ClearEntSet();
	DrawStruDrawing(partset,nodeset,ucs,0,scale,FALSE);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"input the insertion point");
#else
	DragEntSet(basepnt,"输入插入点");
#endif
	seg_map_base_face_ucs = ucs;
}
#endif

//获得任意面（展开面）上的构件集合（主要是角钢,钢管和节点，其余构件由
//这两种构件可获得）
//[输入]direct表示用户视向矢量，是在塔坐标系的矢量是输入
//[输出]nodeset,partset,ucs
static BOOL GetFacePartSet(CLDSLinePart *pSelLinePart, NODESET &nodeset,PARTSET& partset,
						   f3dPoint direct, UCS_STRU &ucs,BOOL bOnlyGetPole)
{
	CLDSLinePart *pPole;
	CLDSNode *pNode,*pStart, *pEnd,*pMirStart=NULL,*pMirEnd=NULL;
	//pole_vec则表示用户所选中角钢的角钢线矢量
	f3dPoint pole_vec,axis_z(0,1,0);
	f3dLine line;
	double angle;//展开面在展开时的旋转角度（转轴为pSelJg）
	/*面展开方式{
		0：表示展开前后或左右对称角钢间的面；
		1：表示展开腿部V面或任意面
				}*/
	int spread_style = 0;
	int nCounts = 4;
	f3dPoint pPoints[4],origin;
	fPolygonRgn selRgn;

	partset.Empty();
	nodeset.Empty();

	normalize(direct);	//视向矢量单位化
	pStart = pSelLinePart->pStart;
	pEnd   = pSelLinePart->pEnd;
	
	if(spread_style==0)//展开一般对称面
	{
		direct.y = 0;	//用户视向认为应在X-Z坐标平面上（塔坐标系）
		pMirStart = pStart->GetMirNode(MIRMSG(1));
		pMirEnd   = pEnd->GetMirNode(MIRMSG(1));
		//Sub_Pnt(pole_vec,pStart->Position(false),pEnd->Position(false));
		//求任意面的用户相对坐标系(相对原塔坐标系)
		ucs.origin = pStart->Position(false);
		angle = cal_angle_of_2vec(-1.0*direct,axis_z);
		f3dPoint axis;
		axis = cross_prod(axis_z,-1.0*direct);
		ucs.axis_x.Set(1,0,0);
		ucs.axis_y.Set(0,0,-1);
		RotateVectorAroundVector(ucs.axis_x,angle,axis);
		RotateVectorAroundVector(ucs.axis_y,angle,axis);
		//用户视向的反方向就是展开面相对坐标系的Z坐标轴方向
		ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
	}
	
	pPoints[0] = pStart->Position(false);
	pPoints[1] = pEnd->Position(false);
	if(pMirEnd!=NULL)
	{
		pPoints[2] = pMirEnd->Position(false);
		if(pMirStart!=NULL) //起点不重合
			pPoints[3] = pMirStart->Position(false);
		else				//起点重合
			nCounts = 3;
	}
	else
	{
		pPoints[2] = pMirStart->Position(false);
		nCounts = 3;
	}
	selRgn.CreatePolygonRgn(pPoints, nCounts);
	nodeset.Empty();
	for(pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
	{
		if(selRgn.PtInRgn(pNode->Position(false))==1)//点在多边形域面内（包括边界在内）
		{
			pNode->feature=0;
			nodeset.append(pNode);
		}
	}
	partset.Empty();
	if(bOnlyGetPole)
	{	//仅包括杆件
		for(pPole=Ta.Parts.GetFirstLinePart();pPole!=NULL;pPole=Ta.Parts.GetNextLinePart())
		{
			if(pPole->pStart==NULL||pPole->pEnd==NULL)
				continue;
			pStart = pPole->pStart;
			pEnd   = pPole->pEnd;
			line.startPt = pStart->Position(false);
			line.endPt   = pEnd->Position(false);
			if(selRgn.LineInRgn(line)==1) //线在多边形域面内（包括边界在内）
			{
				pPole->feature=0;
				partset.append(pPole);
			}
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else
	{
		for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(!pPart->IsLinePart())
			{	//板
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{	//参数化板
					CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pPart;
					if(pParamPlate->IsFL()||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
						continue;	//在下边处理(根据钢管添加端连接构件)	
					CLDSNode *pBaseNode=pParamPlate->GetBaseNode();
					if(pBaseNode)
					{	//参数板基准节点不在多边形域面内(包括边界在内)则继续
						if(selRgn.PtInRgn(pBaseNode->Position(false))!=1)	
							continue;
					}
					else	
					{	//参数板原点不在多边形域面内(包括边界在内)
						if(selRgn.PtInRgn(pParamPlate->ucs.origin)!=1)
							continue;	
					}
					partset.append(pParamPlate);
				}
				else if(pPart->GetClassTypeId()==CLS_PLATE)
				{	//普通钢板
					CLDSPlate *pPlate = (CLDSPlate*)pPart;
					CLDSNode *pBaseNode=Ta.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
					if(pBaseNode)
					{	//钢板基准节点不在多边形域面内(包括边界在内)则继续
						if(selRgn.PtInRgn(pBaseNode->Position(false))!=1)	
							continue;
					}
					else	
					{	//钢板原点不在多边形域面内(包括边界在内)则继续
						if(selRgn.PtInRgn(pPlate->ucs.origin)!=1)
							continue;
					}
					partset.append(pPlate);
					for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
						partset.append(pLsRef->GetLsPtr());
				}
				else	
				{	//其他构件	球体，环形构件等等
					if(selRgn.PtInRgn(pPart->ucs.origin)!=1)//构件原点不在多边形域面内(包括边界在内)则继续
						continue;
					partset.append(pPart);
				}
			}
			else
			{	//杆件
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				pStart = pLinePart->pStart;
				pEnd   = pLinePart->pEnd;
				if(pStart==NULL||pEnd==NULL)
					continue;	//非正常杆件
				line.startPt = pStart->Position(false);
				line.endPt   = pEnd->Position(false);
				if(selRgn.LineInRgn(line)!=1) //线不在多边形域面内(包括边界在内)则继续
					continue;
				partset.append(pLinePart);
				if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
				{	//添加端板及其附带构件到显示列表
					CLDSLineTube *pLineTube = (CLDSLineTube*)pLinePart;
					Ta.Parts.push_stack();
					CLDSParamPlate *pStartPlate = (CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
					if(pStartPlate)
					{	
						partset.append(pStartPlate);
						for(CLsRef *pLsRef=pStartPlate->GetFirstLsRef();pLsRef;pLsRef=pStartPlate->GetNextLsRef())
							partset.append(pLsRef->GetLsPtr());
						for(ATTACHPART_ITEM *pItem=pStartPlate->GetFirstAttachPart();pItem;pItem=pStartPlate->GetNextAttachPart())
						{
							CLDSPart *pTempPart = Ta.FromPartHandle(pItem->h);
							if(pPart)
								partset.append(pPart);
						}
					}
					CLDSParamPlate *pEndPlate = (CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
					if(pEndPlate)
					{
						partset.append(pEndPlate);
						for(CLsRef *pLsRef=pEndPlate->GetFirstLsRef();pLsRef;pLsRef=pEndPlate->GetNextLsRef())
							partset.append(pLsRef->GetLsPtr());
						for(ATTACHPART_ITEM *pItem=pEndPlate->GetFirstAttachPart();pItem;pItem=pEndPlate->GetNextAttachPart())
						{
							CLDSPart *pTempPart = Ta.FromPartHandle(pItem->h);
							if(pPart)
								partset.append(pPart);
						}
					}
					Ta.Parts.pop_stack();
				}
				else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
				{	//添加角钢上的螺栓到显示列表
					CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pLinePart;
					for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
						partset.append(pLsRef->GetLsPtr());
				}
			}
		}
	}
#endif
	return TRUE;
}
void SpreadFaceLine()
{
	CLDSLinePart *pSelPole;
	THANDLE handle;//HANDLE数组
	//通过选择实体获取实体的ID
	AcDbObjectId entId,dimStyleId;
	AcDbEntity *pEnt;
	AcGePoint3d sel_pnt,start,end;				    //用户输入的点 
	f3dPoint f_sel_pnt,f_start,f_end,direct;
	ads_name entres;
    ads_point ptres;
	UCS_STRU ucs;
	PARTSET poleset;
	NODESET nodeset;

aa:
	//选择分段图中正面单线图中的一根角钢（也是展开面上的角钢）并存于entres
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	if (ads_entsel(L"\nPlease select a angle line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\nPlease select a angle line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#endif
#else
#ifdef _ARX_2007
	if (ads_entsel(L"\n请在单线正面图中选择展开面上的一根角钢线,<Enter退出>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\n请在单线正面图中选择展开面上的一根角钢线,<Enter退出>:",entres,ptres)==RTNORM)
#endif
#endif
	{
		//取得該节点的HANDLE号
		acdbGetObjectId(entId, entres);	//获得捕捉到的图元的ID号
		acdbOpenObject(pEnt,entId,AcDb::kForRead);//打开图元线
		if(!pEnt->isKindOf(AcDbLine::desc()))//判断是否是一根直线
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\n The selected line is not angle line");
#else
			AfxMessageBox("\n选中的不是角钢线");
#endif
			pEnt->close();
			goto aa;
		}
		handle=GetTaAtomHandle(pEnt);	//获取选择的图元句柄
		pSelPole = (CLDSLinePart*)Ta.FromPartHandle(handle,CLS_LINEPART);
		if(pSelPole!=NULL&&pSelPole->IsPart())	//用户选中的确实是一根角钢
		{
			pEnt->highlight();
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			if (ads_getpoint(NULL,L"\nplease input the point of view:",asDblArray(sel_pnt))==RTNORM)
#else
			if (ads_getpoint(NULL,"\nplease input the point of view:",asDblArray(sel_pnt))==RTNORM)
#endif
#else
#ifdef _ARX_2007
			if (ads_getpoint(NULL,L"\n请输入视点位置:",asDblArray(sel_pnt))==RTNORM)
#else
			if (ads_getpoint(NULL,"\n请输入视点位置:",asDblArray(sel_pnt))==RTNORM)
#endif
#endif
			{
				f3dPoint perp;	//用户捡取点在角钢线上的垂足点
				f3dPoint base;	//任意面展开时的插入点坐标(ucs坐标系)
				Cpy_Pnt(f_sel_pnt,sel_pnt);
				start = ((AcDbLine*)pEnt)->startPoint();
				end   = ((AcDbLine*)pEnt)->endPoint();
				Cpy_Pnt(f_start,start);
				Cpy_Pnt(f_end,end);
				SnapPerp(&perp,f_start,f_end,f_sel_pnt,NULL);
				Sub_Pnt(direct,perp,f_sel_pnt);
				direct.z = -direct.y;
				direct.y = 0;
				GetFacePartSet(pSelPole,nodeset,poleset,direct,ucs,TRUE);
				f3dPoint pole_vec,perp_vec;
				
				double scale;
				Sub_Pnt(pole_vec,f_end,f_start);
				Sub_Pnt(perp_vec,perp,f_start);
				scale = perp_vec.mod()/pole_vec.mod();
				Sub_Pnt(pole_vec,pSelPole->pEnd->Position(false),pSelPole->pStart->Position(false));
				Add_Pnt(base,pSelPole->pStart->Position(false),pole_vec*scale);
				coord_trans(base,ucs,FALSE);
				DrawFaceLineMap(nodeset,poleset,ucs,base);
			}
			pEnt->unhighlight();
			pEnt->close();
		}
		else	//用户选中的直线不是带有句柄信息的角钢线
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\nThe selected line is wrong angle line");
#else
			AfxMessageBox("\n选中的不是正确的角钢线");
#endif
			pEnt->unhighlight();
			pEnt->close();
			goto aa;
		}
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("\n No proper angle lines selected");
#else
		AfxMessageBox("\n没有选中合适的角钢线");
#endif
}


void SpreadFaceStru()
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
{
	CLDSLinePart *pSelPole=NULL;
	THANDLE handle;//HANDLE数组
	//通过选择实体获取实体的ID
	AcDbObjectId entId,dimStyleId;
	AcDbEntity *pEnt;
	AcGePoint3d sel_pnt,start,end;				    //用户输入的点 
	f3dPoint f_sel_pnt,f_start,f_end,direct;
	ads_name entres;
    ads_point ptres;
	UCS_STRU ucs;
	PARTSET partset;
	NODESET nodeset;
	
aa:
	//选择分段图中正面单线图中的一根杆件（也是展开面上的杆件）并存于entres
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	if (ads_entsel(L"\nPlease select a rod line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\nPlease select a rod line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#endif
#else
#ifdef _ARX_2007
	if (ads_entsel(L"\n请在单线正面图中选择展开面上的一根杆件线,<Enter退出>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\n请在单线正面图中选择展开面上的一根杆件线,<Enter退出>:",entres,ptres)==RTNORM)
#endif
#endif
	{
		//取得該节点的HANDLE号
		acdbGetObjectId(entId, entres);	//获得捕捉到的图元的ID号
		acdbOpenObject(pEnt,entId,AcDb::kForRead);//打开图元线
		if(!pEnt->isKindOf(AcDbLine::desc()))//判断是否是一根直线
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\nThe selected line is not straight line");
#else
			AfxMessageBox("\n选中的不是直线");
#endif
			pEnt->close();
			goto aa;
		}
		handle=GetTaAtomHandle(pEnt);	//获取选择的图元句柄
		pSelPole = (CLDSLinePart*)Ta.FromPartHandle(handle,CLS_LINEPART);
		if(pSelPole!=NULL&&pSelPole->IsLinePart())	//用户选中的确实是一根杆件
		{
			pEnt->highlight();
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			if (ads_getpoint(NULL,L"\nplease input the point of view:",asDblArray(sel_pnt))==RTNORM)
#else
			if (ads_getpoint(NULL,"\nplease input the point of view:",asDblArray(sel_pnt))==RTNORM)
#endif
#else
#ifdef _ARX_2007
			if (ads_getpoint(NULL,L"\n请输入视点位置:",asDblArray(sel_pnt))==RTNORM)
#else
			if (ads_getpoint(NULL,"\n请输入视点位置:",asDblArray(sel_pnt))==RTNORM)
#endif
#endif
			{
				f3dPoint perp;	//用户捡取点在角钢线上的垂足点
				f3dPoint base;	//任意面展开时的插入点坐标(ucs坐标系)
				Cpy_Pnt(f_sel_pnt,sel_pnt);
				start = ((AcDbLine*)pEnt)->startPoint();
				end   = ((AcDbLine*)pEnt)->endPoint();
				Cpy_Pnt(f_start,start);
				Cpy_Pnt(f_end,end);
				SnapPerp(&perp,f_start,f_end,f_sel_pnt,NULL);
				Sub_Pnt(direct,perp,f_sel_pnt);
				direct.z = -direct.y;
				direct.y = 0;
				GetFacePartSet(pSelPole,nodeset,partset,direct,ucs,FALSE);
				f3dPoint pole_vec,perp_vec;
				
				double scale;
				Sub_Pnt(pole_vec,f_end,f_start);
				Sub_Pnt(perp_vec,perp,f_start);
				scale = perp_vec.mod()/pole_vec.mod();
				pole_vec=pSelPole->pEnd->Position(false)-pSelPole->pStart->Position(false);
				base=pSelPole->pStart->Position(false)+pole_vec*scale;
				coord_trans(base,ucs,FALSE);
				DrawFaceStruMap(nodeset,partset,ucs,base);
			}
			pEnt->unhighlight();
			pEnt->close();
		}
		else	//用户选中的直线不是带有句柄信息的角钢线
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\n The selected rod is wrong");
#else
			AfxMessageBox("\n选中的不是正确的杆件");
#endif
			pEnt->unhighlight();
			pEnt->close();
			goto aa;
		}
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("\n No proper rods selected");
#else
		AfxMessageBox("\n没有选中合适的杆件");
#endif
}
#else
{}
#endif
