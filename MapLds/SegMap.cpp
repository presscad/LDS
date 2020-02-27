//<LOCALE_TRANSLATE/>
//�ṹͼ
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

UCS_STRU seg_map_base_face_ucs;	//�ֶ�ͼ�����棨��һ������������ϵ��
#ifndef __TSA_FILE_
//
//From PaperPatternDrawing.cpp
AcDbObjectId NewBoltCircle(AcDbBlockTableRecord *pBlockTableRecord,f3dCircle Cir,BOOL bFootNail=FALSE);
//from ProcessCard.cpp
bool GetProcessCardRegion(const char* szCardFilePathName,f2dRect* prcCard);
f2dRect GetProcessCardDrawingRect(int iProcessCardType=0);
f2dRect InsertPartGroupProcessCardTbl(AcDbBlockTableRecord *pBlockTableRecord,f2dPoint insert_pos,
									  PART_GROUP_CARD_INFO *pPartGroupInfo,BOOL bPrompt = FALSE);
//x_scale,y_scaleΪX�����Y������ķŴ����
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
	{	//ͼ����ͼ
		CHashList<SEGI> hashIncSegs;	//��ǰ��ͼ�������ķֶμ��� wjh-2017.11.15
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
		//�����ͼ������ƥ����©����˨
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
						continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
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
				if(pPart==pAfterPart)	//ɾ��������Ĳ�©�������µ��ظ���������
					partset.DeleteCursor();
			}
			if(bPush)
				partset.pop_stack();
		}
	}
	else if(pCurView->m_iViewType==1)
	{	//������ͼ
		for(CViewPart *pViewPart=pCurView->GetFirstViewPart(); pViewPart; pViewPart=pCurView->GetNextViewPart())
		{
			CLDSNode *pNode = Ta.FromNodeHandle(pViewPart->handle);
			if(pNode)
				scope.VerifyVertex(pNode->Position(false));
			pPart = Ta.FromPartHandle(pViewPart->handle);
			if(pPart)
			{
				pViewPart->pDisplayView=pCurView;
				pPart->feature=(long)pViewPart;	//������ص�CViewPartָ�� wht 10-12-10
				partset.append(pPart);
			}
		}
	}
	if(partset.GetNodeNum()<1&&nodeset.GetNodeNum()<1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("%s SegI There are no enough legal rods or nodes, Please check whether to open the right data file!",facedlg.m_sSegNo);
#else
		logerr.Log("%s����û��ѡ���㹻�ĺϷ��˼���ڵ�,�����Ƿ������ȷ�������ļ�!",facedlg.m_sSegNo);
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
	//�����ͼ����ϵ
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

	//�����Ʋ�����ʾ�ķ����ϵ���˨ wht 10-03-04
	for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(sys.stru_map.bOnlyDrawHorizontalPlate)
			{	//�����Ƶ�ǰ��ͼ��ˮƽ��ʾ�ĸְ� wht 10-06-15
				if(fabs(ucs.axis_z*pPlate->ucs.axis_z)<0.707&&
					fabs(pPlate->ucs.axis_z*f3dPoint(0,0,1))<EPS_COS2)	//���Ű��ţ��嶼Ӧ�û���
				{
					CViewPart *pViewPart=(CViewPart*)pPlate->feature;
					if(pViewPart&&pViewPart->cFlag=='S')
						continue;	//����Ҫ�Ƴ�չ�����Ƶĸְ� wht 10-12-11
					partset.DeleteNode(pPlate->handle);
					continue; //��ֱ����
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
				&&pParamPlate->m_iParamType!=TYPE_WATERSHIELDEDPLATE)	//��ˮ��
				continue;
			if(fabs(ucs.axis_z*pParamPlate->ucs.axis_z)>0.707)
				continue; //ˮƽ����
			CLsRef *pLsRef=NULL;
			for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				partset.DeleteNode(pLsRef->GetLsPtr()->handle);
		}
	}
	for(CLDSNode *pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		pNode->feature=0;
	//��չ����������־ ��ձ�עλ�󽫲�֧���û�ָ���Ļ����� wht 10-11-19
	//for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	//	pPart->feature=0;
	/*����λ��Ӧ����ͼ��������ϵԭ�㶨λ����ʹʹ�õ�һ��������ҲҪ���ǽǸֹ�����ʼucs.originΪ�գ�Ӧȡʼ�˵����ꡡwjh-2011.7.2
	CLDSPart *pFirstPart=partset.GetFirst();
	if(pFirstPart)
	ucs.origin=pFirstPart->ucs.origin;*/
	DRAGSET.ClearEntSet();
	//�Ǹֺ͸ְ����������Ϳ�Ϊ����߻���ֱͨ�� wht 10-06-15
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
	DragEntSet(basepnt,"��������");
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
		AfxMessageBox("û��ѡ���㹻�ĺϷ��˼���ڵ�,�����Ƿ������ȷ�������ļ�!");
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
	DragEntSet(basepnt,"��������");
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
	{	//���ƿ�ģ�Ǹ���ͼ wht 15-09-09
		CSectionKmAngleViewDrawing *pKmAngleViewDrawing=(CSectionKmAngleViewDrawing*)pSectionViewDraw;
		double km_angle_draw_scale=(1.0/pKmAngleViewDrawing->GetInternalDrawingScale())*zoomdraw_coef;	//���Ʊ���Ҫ�����麸���ڲ����Ʊ������������ű���
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
	//�����麸��ͼ���¼
	DRAGSET.BeginBlockRecord();
	AcDbBlockTableRecord *pBlockTableRecord=DRAGSET.RecordingBlockTableRecord();//�������¼ָ��
	f3dPoint blk_insert_pos=base_pnt;
	base_pnt.Set();
	//����ͼ��������
	if(fabs(zoomdraw_coef-1)>0.1)
		pSectionViewDraw->ZoomDrawing(zoomdraw_coef);
	//ͳһ�ڿ�ʼλ�ñ�ע�ߴ��ָߣ�IDbEntityģʽ��ԭ��ģʽ��Ҫʹ�� wht 16-10-18
	//��ʼ���ߴ��ע
	double glbalWeldDrawingFontZoomCoef=pSectionViewDraw->ZoomToModelTextSize(1.0);
	double fDimTextSize		=max(sys.part_group_map.fMinFontSize,sys.fonts.weld.fDimTextSize*zoomdraw_coef*glbalWeldDrawingFontZoomCoef);
	double fDimAngleTextSize=max(sys.part_group_map.fMinFontSize,sys.fonts.weld.fDimAngleTextSize*zoomdraw_coef*glbalWeldDrawingFontZoomCoef);
	double fDimAngleArrowSize=sys.dim_map.fDimArrowSize*zoomdraw_coef*glbalWeldDrawingFontZoomCoef;

	DimStyleTable::dimStyle.InitDimStyle(fDimTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	DimStyleTable::dimStyle4.InitDimStyle(fDimTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	//�뾶��ע��ʽ
	DimStyleTable::dimRadiusStyle.InitDimStyle(fDimTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	//��ʼ���Ƕȱ�ע
	if(!sys.general_set.bProcessCardTbl)	
	{	//1:1�麸ͼ �Ŵ�����߶� wht 10-09-14
		double fTextHeight=sys.fonts.weld.fDimAngleTextSize;
		DimStyleTable::dimAngleStyle.InitDimStyle(fTextHeight,sys.dim_map.fDimArrowSize*zoomdraw_coef);
	}
	else
	{
		if(zoomdraw_coef>=1)
			DimStyleTable::dimAngleStyle.InitDimStyle(sys.fonts.weld.fDimAngleTextSize,sys.dim_map.fDimArrowSize);
		else
		{	//��֤������С�߶�Ϊ fMinFontSize  wht 10-05-31
			//double fTextSize=sys.dim_map.fDimAngleTextSize*scale;
			double fTextSize=max(sys.part_group_map.fMinFontSize,sys.fonts.weld.fDimAngleTextSize*zoomdraw_coef);
			DimStyleTable::dimAngleStyle.InitDimStyle(fTextSize,sys.dim_map.fDimArrowSize*zoomdraw_coef);
		}
	}
	//��������˨��С��ʱ��1:20ʱ��ʾ�ĳߴ�,������1:1�麸ͼʱ��Ҫ�Ŵ�����߶�,��˨ֱ�� wht 10-09-06
	double fInternalZoomScale=zoomdraw_coef*sys.general_set.WeldMapScale.fBoltSymbolFactor/pSectionViewDraw->GetInternalDrawingScale();
	//����CSectionViewDrawing��IDbEntity����
	for(IDbEntity* pEntity=pSectionViewDraw->EnumFirstDbEntity();pEntity;pEntity=pSectionViewDraw->EnumNextDbEntity())
	{
	//��ȡ����ͼԪ�����ӵ�Id��ʶ����
		long hPart=0;
		ILifeDataObject* pAttachData=pEntity->GetLifeDataObject();
		if(pAttachData&&pAttachData->GetType()==ILifeDataObject::INTEGER)
			hPart=((IAttachIntegerData*)pAttachData)->GetIntPara();
		
		PEN_STRU pen = pEntity->GetPen();
		if(pen.style==PS_SOLID)			//ͼ��0
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else if(pen.style==PS_DASH)		//ͼ��1
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		else if(pen.style==PS_DOT)		//ͼ��2 
			GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
		else if(pen.style==PS_DASHDOT)	//ͼ��3
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		else if(pen.style==PS_DASHDOTDOT)//ͼ��4
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		else if(pen.style==PS_NULL)		//ͼ��5
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
		else if(pen.style==PS_INSIDEFRAME)//ͼ��6
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		if(pEntity->Layer().GetLength()>0)
			SetCurLayer(pEntity->Layer());
		if(pEntity->GetDbEntType()==IDbEntity::DbPoint)				//��
		{
			IDbPoint* pPoint=(IDbPoint*)pEntity;
			f3dPoint point=pPoint->Position();
			trans_pos(point,base_pnt);
			AcDbPoint *pAcDbPoint=new AcDbPoint(AcGePoint3d(point.x,point.y,point.z));
			AcDbObjectId pointId;
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pAcDbPoint))
				pAcDbPoint->close();
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbLine)			//�߶�
		{
			IDbLine* pLine=(IDbLine*)pEntity;
			f3dPoint startPt(pLine->StartPosition());
			f3dPoint endPt(pLine->EndPosition());
			trans_pos(startPt,base_pnt);
			trans_pos(endPt,base_pnt);
			CreateAcadLine(pBlockTableRecord,startPt,endPt,hPart);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbArcline)		//��������
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
		else if(pEntity->GetDbEntType()==IDbEntity::DbCircle)		//Բ
		{
			IDbCircle* pDbCircle=(IDbCircle*)pEntity;
			pDbCircle->Offset(base_pnt);
			CreateAcadCircle(pBlockTableRecord,pDbCircle->Center(),pDbCircle->Radius(),hPart);	
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbPolyline)		//����
		{
			IDbPolyline* pPolyLine=(IDbPolyline*)pEntity;
			int nLineNum=pPolyLine->numVerts();
			AcDbPolyline *pAcDbPolyLine=new AcDbPolyline(nLineNum);//����LINE����
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
		else if(pEntity->GetDbEntType()==IDbEntity::DbSpline)		//��������
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
			AcDbSpline *pAcDbSpline=new AcDbSpline(fitPoints,4,0);				//����SPline�������߶���
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,splineId,pAcDbSpline))
				pAcDbSpline->close();
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbText)
		{
			IDbText* pDbText=(IDbText*)pEntity;
			AcDbObjectId textStyleId;
			textStyleId.setFromOldId(pDbText->GetTextStyleId());
			//2017.8.26֮��İ汾IDbMText::ZoomScale()���ѿ����ָ߳�����ϵ�� wjh-2017.8.26
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
			//2017.8.26֮��İ汾IDbMText::ZoomScale()���ѿ����ָ߳�����ϵ�� wjh-2017.8.26
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
		else if(pEntity->GetDbEntType()==IDbEntity::DbRadialDimension)	//�뾶��ע
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
		else if(pEntity->GetDbEntType()==IDbEntity::DbAngularDimension)	//�Ƕȱ�ע
		{
			GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
			IDbAngularDimension* pAngularDim=(IDbAngularDimension*)pEntity;
			pAngularDim->Offset(base_pnt);
			GEPOINT vertex,startPt,endPt,dimPos;
			pAngularDim->GetLine1Start(vertex);
			pAngularDim->GetLine1End(startPt);
			pAngularDim->GetLine2End(endPt);
			//TODO:IDbAngularDimension�е�ArcPointû�н������ź�ƫ�ƣ��ڴ��ֶ���������
			pAngularDim->GetArcPoint(dimPos);
			double fArrowSize=pAngularDim->GetArrowSize();
			CXhChar50 dimText;
			dimText.Copy(pAngularDim->GetDimText());
			if(dimText.GetLength()>0)
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,vertex,startPt,endPt,dimPos,dimText,fDimAngleTextSize,fArrowSize);
			else
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,vertex,startPt,endPt,dimPos,NULL,fDimAngleTextSize,fArrowSize);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbAlignedDimension)	//����ߴ��ע
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
		else if(pEntity->GetDbEntType()==IDbEntity::DbRotatedDimension)	//��תͶӰ�ߴ��ע
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
	//�����麸ͼ����������
	for(f3dArcLine *pLine=pSectionViewDraw->GetEdgeLineList()->GetFirst();pLine;pLine=pSectionViewDraw->GetEdgeLineList()->GetNext())
	{
		if(pLine->pen.style==PS_SOLID)			//ͼ��0
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else if(pLine->pen.style==PS_DASH)		//ͼ��1
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		else if(pLine->pen.style==PS_DOT)		//ͼ��2 
			GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
		else if(pLine->pen.style==PS_DASHDOT)	//ͼ��3
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		else if(pLine->pen.style==PS_DASHDOTDOT)//ͼ��4
			GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
		else if(pLine->pen.style==PS_NULL)		//ͼ��5
			GetCurDwg()->setClayer(LayerTable::BendLineLayer.layerId);
		else if(pLine->pen.style==PS_INSIDEFRAME)//ͼ��6
			GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
		f3dArcLine arcline=*pLine;
		if(pLine->pen.style==PS_USERSTYLE)//Spline��������
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
			if(arcline.ColumnNorm()==arcline.WorkNorm())	//Բ��
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
	//������˨�� Ĭ��������˨ͼ��ֱ����Ϊ2mm ��˨����������(ֱ��/2)�� wht 09-10-28
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	for(f3dCircle *pCir=pSectionViewDraw->GetLsCirList()->GetFirst();pCir;pCir=pSectionViewDraw->GetLsCirList()->GetNext())
	{
		f3dPoint centre=pCir->centre;
		trans_pos(centre, base_pnt);
		int nBoltD=ftoi(2*pCir->radius*pSectionViewDraw->GetInternalDrawingScale());//sys.general_set.WeldMapScale.fRealToDraw);
		//������˨ֱ���������ű���
		//ls_scale=scale*pCir->radius;
		BOOL bRet=FALSE;
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle==1)	//ͼ������
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
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==2)	//����ͼ������
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
		else if(sys.general_set.WeldMapScale.iBoltSymbolStyle==3)	//����ֱ��ͼ������
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
		if(sys.general_set.WeldMapScale.iBoltSymbolStyle==0||!bRet)	//Բ�״���
		{
			f3dCircle Cir;
			Cir.centre=centre;
			Cir.radius=zoomdraw_coef*(pCir->radius+pCir->feature/(20.0*pSectionViewDraw->GetInternalDrawingScale()));
			NewBoltCircle(pBlockTableRecord,Cir);
		}
	}
	//��ע��Գߴ�/���Գߴ�
	if(sys.part_group_map.bDimBeforeWeldLocatSpace||sys.part_group_map.bDimAfterWeldCheckLine)
	{	//��ע��Գߴ�
		GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
		if(sys.general_set.iDimPrecision==0)		//��ȷ��1mm
			CSizeTextDim::PRECISION=1.0;
		else if(sys.general_set.iDimPrecision==1)	//��ȷ��0.5mm
			CSizeTextDim::PRECISION=0.5;
		else if(sys.general_set.iDimPrecision==2)	//��ȷ��0.1mm
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
			if(abs(pSpaceDim->iOrder)==3&&!sys.part_group_map.bDimBeforeWeldLocatSpace)//����ǰ��λ�ߴ�
				continue;
			else if(pSpaceDim->iOrder==0&&!sys.part_group_map.bDimAfterWeldCheckLine)	//���Ӻ���ߴ�
				continue;
			//ĿǰbCanTextOutter��iTextDimSylte==1ͬ����dimStyle4��ע��ʽ�У�������������Ƿ���Ҫ�� wjh-2019.5.12
			if(pSpaceDim->bCanTextOutter|| pSpaceDim->iTextDimSylte==1)
				DimAngleSize(pBlockTableRecord,dimStartPos,dimEndPos,dimPos,pSpaceDim->DimText(),DimStyleTable::dimStyle4.dimStyleId,pSpaceDim->angle,fDimTextSize);
			else
				DimAngleSize(pBlockTableRecord,dimStartPos,dimEndPos,dimPos,pSpaceDim->DimText(),DimStyleTable::dimStyle.dimStyleId,pSpaceDim->angle,fDimTextSize);
		}
	}
	
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	if(sys.general_set.iDimPrecision==0)		//��ȷ��1mm
		CTextOnlyDrawing::PRECISION=1.0;
	else if(sys.general_set.iDimPrecision==1)	//��ȷ��0.5mm
		CTextOnlyDrawing::PRECISION=0.5;
	else if(sys.general_set.iDimPrecision==2)	//��ȷ��0.1mm
		CTextOnlyDrawing::PRECISION=0.1;
	CTextOnlyDrawing *pPrevLocatePointDim=NULL,*pLocatePointDim=NULL;
	for(pLocatePointDim=pSectionViewDraw->GetPureTextDimList()->GetFirst();pLocatePointDim;
		pLocatePointDim=pSectionViewDraw->GetPureTextDimList()->GetNext())
	{
		if(pLocatePointDim==NULL)
			continue;
		double fTextHeight=sys.part_group_map.fMinFontSize;
		//���������ű���zoomdraw_coef��������
		if(!sys.general_set.bProcessCardTbl)	
			fTextHeight=pLocatePointDim->textHeight*glbalWeldDrawingFontZoomCoef*zoomdraw_coef;//fInternalZoomScale;
		else	//��֤������С�߶�Ϊ fMinFontSize  wht 10-05-31
			fTextHeight=max(sys.part_group_map.fMinFontSize,pLocatePointDim->textHeight*glbalWeldDrawingFontZoomCoef*zoomdraw_coef);
		f3dPoint dimPos=pLocatePointDim->dimPos+pLocatePointDim->origin;
		trans_pos(dimPos,base_pnt);
		double fTextHeightOffset=fTextHeight*0.7;
		if(abs(pLocatePointDim->iOrder)==3)	//��������ͼ��
		{
			AcDbObjectId LineId;//�����ʶ��
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

			AcDbPolyline *pPolyLine=new AcDbPolyline(2);//����LINE����
			double fPolyLineWidth=sys.stru_map.fPolyLineWidth*zoomdraw_coef;
			pPolyLine->addVertexAt(0,acad_start,0,fPolyLineWidth,fPolyLineWidth);
			pPolyLine->addVertexAt(0,acad_end,0,fPolyLineWidth,fPolyLineWidth);
			pPolyLine->addVertexAt(0,acad_arrow,0,fPolyLineWidth,fPolyLineWidth);
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//��ʵ��д�����¼
				pPolyLine->close();//���ùرն���ĳ�Ա����
			DimText(pBlockTableRecord,dimPos,pLocatePointDim->dimText,
				TextStyleTable::hzfs.textStyleId,fTextHeight,xDimSectSymbol.fSymbolFlowAngle,
				(AcDb::TextHorzMode)xDimSectSymbol.hAcadMode,(AcDb::TextVertMode)xDimSectSymbol.vAcadMode);//AcDb::kTextCenter,AcDb::kTextVertMid);*/
		}
		else if(sys.part_group_map.bDimAbsoluteDist||	//���Գߴ�
				pLocatePointDim->iOrder==0||			//���ӷ�������
				abs(pLocatePointDim->iOrder)==2||		//��ͼ����
				abs(pLocatePointDim->iOrder)==4)		//�����ַ�
		{	//��ע���Գߴ����ͼ����
			if(pLocatePointDim->iOrder==0&&pLocatePointDim->angle>Pi*0.5&&pLocatePointDim->angle<1.5*Pi)
				pLocatePointDim->angle=pLocatePointDim->angle-Pi;
			//TODO:�赥������,��Ӧ��CTextOnlyDrawing::GetDimRect����һ���Դ��� wjh-2018.8.1
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
	//��ע�Ƕ���
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
				continue;	//����ע0�Ȼ���180��
			if(pAngularDim==NULL)
				continue;
			trans_pos(pAngularDim->vertex,base_pnt);
			trans_pos(pAngularDim->dimStartPos,base_pnt);
			trans_pos(pAngularDim->dimEndPos,base_pnt);
			trans_pos(pAngularDim->dimPos,base_pnt);
			if(pAngularDim->dimText.GetLength()>0)	//ָ����ע����
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,pAngularDim->vertex,
					pAngularDim->dimStartPos,pAngularDim->dimEndPos,pAngularDim->dimPos,pAngularDim->dimText,fDimAngleTextSize,fDimAngleArrowSize);	
			else	//�Զ������ע�Ƕ�
				DimAngle(pBlockTableRecord,DimStyleTable::dimAngleStyle.dimStyleId,pAngularDim->vertex,
					pAngularDim->dimStartPos,pAngularDim->dimEndPos,pAngularDim->dimPos,NULL,fDimAngleTextSize,fDimAngleArrowSize);	
		}
	}
	//��չ���ݵ�
	CTextFrame *pDataPoint=NULL;
	for(pDataPoint=pSectionViewDraw->GetDataPointDimList()->GetFirst();pDataPoint;pDataPoint=pSectionViewDraw->GetDataPointDimList()->GetNext())
	{
		PART_DIMnDRAW* pDimDraw=pSectionViewDraw->m_pPartGroupDrawing->GetPartDimDraw(pView?pView->GetId():0,pDataPoint->hPart);
		if(pDimDraw&&!pDimDraw->bDimPartNo&&!pDimDraw->bMandatoryDimLabel)
			continue;	//����ɸѡ�ڵ�ǰ��ͼ�иù����Ļ��Ʋ�ͼ����ע���
		AcDbXrecord *pXrec;
		AcDbObjectId dictObjId, xrecObjId,pointId;
		AcDbDictionary *pDict;
		f3dPoint data_pos=pDataPoint->dataPos;
		f3dPoint dim_pos=pDataPoint->dimPos;
		//���ݱ����õ��������ն�λ�� wht 10-09-17
		//TODO:δ����ʱ��Ӧ��ǰ��GetRefLineEndPosʱ���ǵ��������������ӵ���Բ�߶ȴ�����Ӱ�졡wjh-2016.10.19
		f3dPoint refEndPos=pDataPoint->GetRefLineEndPos(1.0);
		trans_pos(dim_pos,base_pnt);
		trans_pos(refEndPos,base_pnt);
		trans_pos(data_pos,base_pnt);
		AcDbPoint *pPoint=new AcDbPoint(AcGePoint3d(data_pos.x,data_pos.y,data_pos.z));
		if(pBlockTableRecord->appendAcDbEntity(pointId,pPoint)==Acad::eOk)//��ʵ��д�����¼
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
		//�������ݵ�λ�ñ�ע�������
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(strlen(pDataPoint->Text())>0&&sys.part_group_map.bAutoDimPartNo)
		{
			//fTextSizeӦȡCTextFrameֵ, ������ܻ���ֱ�ע���ָ߶�����Ȧ��С��һ�����
			if(zoomdraw_coef<1)
				pDataPoint->ZoomFrameSize(zoomdraw_coef,sys.part_group_map.fMinFontSize);
			double fTextSize=pDataPoint->TextHeight();
			double fHalfWidth=0.5*pDataPoint->Width();
			double fHalfHeight=0.5*pDataPoint->Height();
			if(pDataPoint->iCurLayer!=-1||pDimDraw->ciLeadLineMode>0)//����������
				CreateAcadLine(pBlockTableRecord,data_pos,refEndPos);
			f3dPoint text_pos(dim_pos.x,dim_pos.y+0.5*fTextSize);
			if(pDataPoint->iCurLayer!=-1)
			{
				DimText(pBlockTableRecord,text_pos,pDataPoint->Text(),TextStyleTable::hzfs.textStyleId,
					fTextSize,0,AcDb::kTextCenter,AcDb::kTextTop);
				//�����ı���Ŀ���
				if(pDataPoint->Style()==0)
					CreateAcadCircle(pBlockTableRecord,dim_pos,fHalfWidth);
				else 
				{	
					if(zoomdraw_coef<1)	//����������ͼ��С���´�ӡʱ��Բ���±߽������ص���wjh-2016.10.19
						fHalfHeight+=(1-zoomdraw_coef)*0.5;
					f3dPoint topLeft(dim_pos.x-fHalfWidth,dim_pos.y+fHalfHeight);
					if(pDataPoint->Style()==1)	//��Բ���ο�
						CreateAcadRect(pBlockTableRecord,topLeft,2*fHalfWidth,2*fHalfHeight,TRUE);
					else	//ֱ�Ǿ��ο�
						CreateAcadRect(pBlockTableRecord,topLeft,2*fHalfWidth,2*fHalfHeight);
				}
			}
		}
	}
	//���
	for(ACAD_HATCH *pHatch=pSectionViewDraw->GetHatchList()->GetFirst();pHatch;pHatch=pSectionViewDraw->GetHatchList()->GetNext())
		CreateAcadHatch(pBlockTableRecord,pHatch->vertexList,pHatch->sHatchStyle,pHatch->fScale,pHatch->fAngle,pHatch->color);
	//������ͼ������
	DRAGSET.EndBlockRecord(pExterBlockTableRecord,blk_insert_pos,1);
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>
//�ж����Ǹ��Ƿ�������֫����һ��,���ںϲ�����ģ�Ǹ� wht 15-11-06
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
	{	//����ͬ��
		if( pLineAngle->get_norm_x_wing()*pCoupleAngle->get_norm_x_wing()>EPS_COS2&&
			pLineAngle->get_norm_y_wing()*pCoupleAngle->get_norm_y_wing()>EPS_COS2)
			return true;
	}
	else if(cosa<-EPS_COS2)
	{	//���߷���
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
	//��ͷλ���������࿿ģ�Ǹ־���Ҫ���ƣ���ȡʮ�ֲ����ͼʱ��������һ��ĽǸ�
	//��Ҫ��pTopView->SonParts�в�������һ��ĽǸֲ����ƿ�ģ�Ǹ� wht 15-10-30
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
	//double km_angle_draw_scale=(1/sys.general_set.WeldMapScale.fRealToDraw)*draw_scale;	//���Ʊ���Ҫ�����麸���ڲ����Ʊ������������ű��� wht 15-11-06
	double km_angle_draw_scale=(1/sys.general_set.WeldMapScale.fRealToDraw);	//���Ʊ���Ҫ�����麸���ڲ����Ʊ������������ű��� wht 15-11-06
	km_angle_draw_scale = min(km_angle_draw_scale,draw_scale);
	for(CLDSLineAngle *pAngle=(CLDSLineAngle*)kmAngleSet.GetFirst();pAngle;pAngle=(CLDSLineAngle*)kmAngleSet.GetNext(),i++)
	{	//1.�����ְ���˨��ȡ�Ǹֹ�����˨
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
//���ƿ�ģ�Ǹ�
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
			sKmLabel.Append(CXhChar16(" ��%.1f*%d",hole_d,hits));
		}
	}
	if(strlen(kmAngle.sKaiHeAngle)>0)
		sKmLabel.Append(CXhChar16(" ���Ͻ�%s",(char*)kmAngle.sKaiHeAngle));
	f2dRect rect=xAngleDrawing.GetDrawRegion(draw_scale);
	f3dPoint dimPos(0.5*kmAngle.length*draw_scale,rect.bottomRight.y-3*sys.fonts.weld.fPartNoTextSize);
	DimText(DRAGSET.RecordingBlockTableRecord(),dimPos,sKmLabel,TextStyleTable::hzfs.textStyleId,
		sys.fonts.weld.fPartNoTextSize,0,AcDb::kTextMid,AcDb::kTextTop);
	DRAGSET.EndBlockRecord(pExterBlockTableRecord,GEPOINT(top_left.x,top_left.y));
}

void PartGroupSetupDrawing()
{
	CLogErrorLife logErrorLife;
	sys.m_iCurMapType = 5;  //����ת��Ϊsys.fonts.ģʽ��ʵ�ʼ�ʹ��ʼ��ҲӦΪ���������麸ͼ��
	//sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    InitDrawingEnv();
	CAcModuleResourceOverride resOverride;
	CSelPartGroupDlg part_group_dlg;
	CTmaPtrList<CLDSPartGroup,CLDSPartGroup*> partGroupList;
	part_group_dlg.m_pWeldPartSet=&partGroupList;
	//�ṩ�������湩�û�ѡ��ǰ��Ҫ���Ƶĺ�����ϼ�
	if(Ta.PartGroup.GetNodeNum()==0||part_group_dlg.DoModal()!=IDOK)
		return;
	if(partGroupList.GetNodeNum()==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The suitable weld parts are not selected, please select again!");
#else
		AfxMessageBox("ûѡ�к��ʵ��麸����������ѡ��!");
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
			spaceDlg.m_iSpaceMode=1;	//��ʾ��϶
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
	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(pBlockTableRecord==NULL)
		return;
	BOOL bUseDrag=TRUE;//partGroupList.GetNodeNum()==1;
	if(bUseDrag)
		DRAGSET.ClearEntSet();
	f3dPoint origin;	//ԭ������Ϊ(0,0,0)
	PAGE_INDEX pagenumer;
	for(CLDSPartGroup *pPartGroup=partGroupList.GetFirst();pPartGroup;pPartGroup=partGroupList.GetNext(),pagenumer.IncrementSerial())
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=Ta.FromPartHandle(pPartGroup->handle);
		if(!pDatumPart.IsHasPtr())
			return;
		if(pDatumPart->IsLinePart())
		{	//�����λ��׼�����Ǹ˼������¼�ֹܵĳ������췽����Ϊ��ͼ����ϵ��X�᷽��
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
		//�����麸��������ֵ�Լ����ű���
		f2dRect draw_rect=GetProcessCardDrawingRect(1);
		xPartGroupDraw.InitPartGroupDrawing(pPartGroup,draw_rect.Width(),draw_rect.Height());
		//���ɴ˹�������Ҫע�͵�����
		//GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		PART_GROUP_CARD_INFO PartGroupInfo;
		memset(&PartGroupInfo,'\0',sizeof(PART_GROUP_CARD_INFO));
		if(sys.general_set.bProcessCardTbl)
		{	//���տ���Ϣ
			CXhChar100 guige;
			pDatumPart->GetSpecification(guige,FALSE);
			PartGroupInfo.m_nPageIndex=1;//iOrder+1;
			PartGroupInfo.m_sPartNo.Copy(GetPartNoIncMat(pDatumPart->GetPartNo(),pDatumPart->cMaterial));
			LOCALE_PROPITEM* pWeldLabelPrefix = gxDrwLocalizer.GetLocaleItem("WeldLabelPrefix");
			if (pWeldLabelPrefix)
				PartGroupInfo.m_sPartNo.InsertBefore(pWeldLabelPrefix->szStrBuff, 0);
			PartGroupInfo.m_sSegStr = pDatumPart->iSeg.ToString();
			//������´�����Ҫ��Ϊ��ʹ���տ��ϵĳ������м����ͼ�ϵ��ܳ���עС�����һλҲһ��
			double length=pDatumPart->GetLength();
			if(sys.general_set.iDimPrecision==0)		//��ȷ��1mm
				length = ftoi(length);
			else if(sys.general_set.iDimPrecision==1)	//��ȷ��0.5mm
				length = fto_halfi(length);
			else if(sys.general_set.iDimPrecision==2)	//��ȷ��0.1mm
				length=ftol(length*2)/2.0;
			PartGroupInfo.m_sLength.Printf("%.1f",length);
			SimplifiedNumString(PartGroupInfo.m_sLength);
			LOCALE_PROPITEM* pProcessSymbolMark = gxDrwLocalizer.GetLocaleItem("ProcessCard.HasSymbol");
			if(pProcessSymbolMark)//gxDrwLocalizer.ciCustomerSerial==5)	//�ൺ��������
				PartGroupInfo.m_sWeld.Copy(pProcessSymbolMark->szStrBuff);//"��");
			else if(sys.part_map.iPartMapStyle==1)	//�㽭ʢ���������޹�˾
				PartGroupInfo.m_sWeld.Copy("V");
			else
				PartGroupInfo.m_sWeld.Copy("��");
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
				PartGroupInfo.m_sFootNailPlate.Printf("[%.fX%.fX%d��/��",thick,h,num);
#endif
		}
		if(part_group_dlg.m_pSelSectionView)
		{	//��������ÿ���麸����ͼ
			xPartGroupDraw.EmptyDrawingComponents();
			CSectionViewDrawing *pViewDrawing=xPartGroupDraw.CreateSectionViewDrawing(part_group_dlg.m_pSelSectionView);
			if(pViewDrawing==NULL)
				continue;	//ѡ��ʧ��
			pViewDrawing->FinalDrawing();	//��ʱ�ڴ˴������Ժ�Ӧͳ�￼��(������FinalDrawing(),����������ͼʱ�������ñ�������) wht 16-11-09
			f3dPoint base_pnt=origin;
			double zoomdraw_coef=1.0;
			if(sys.general_set.bProcessCardTbl)
			{	//���빤�տ�ͼ��
				draw_rect = InsertPartGroupProcessCardTbl(pBlockTableRecord,origin,&PartGroupInfo);
				if(draw_rect.Width()<EPS2||draw_rect.Height()<EPS2)
					continue;
				f2dRect actual_rect = pViewDrawing->GetDrawRegion();
				//�ʵ���С��ͼ����λ��,���������ұ�֮����5mm��϶
				draw_rect.topLeft.x+=5;
				draw_rect.topLeft.y-=5;
				draw_rect.bottomRight.x-=5;
				draw_rect.bottomRight.y+=5;
				//���ݹ��տ�ͼ���Զ������麸ͼ��ʾ����
				double zoom_scale1 = draw_rect.Width()/actual_rect.Width();
				double zoom_scale2 = draw_rect.Height()/actual_rect.Height();
				zoomdraw_coef=min(zoom_scale1,zoom_scale2);
				zoomdraw_coef=min(1,zoomdraw_coef);
				base_pnt.x = draw_rect.topLeft.x-actual_rect.topLeft.x*zoomdraw_coef;
				base_pnt.y = (draw_rect.topLeft.y+draw_rect.bottomRight.y)*0.5;
			}
			//�����麸ͼ
			DrawSectionViewMap(pBlockTableRecord,pViewDrawing,base_pnt,zoomdraw_coef);
		}
		else if(xPartGroupDraw.CreatePartGroupDrawing())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(NULL,"Generating No. %d weld parts view(%s)!",pagenumer.wiSerial+1,pPartGroup->sGroupName);
#else
			PutMessage(NULL,"�������ɵ�%d���麸����ͼ(%s)!",pagenumer.wiSerial+1,pPartGroup->sGroupName);
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
					{	//����Ϊ��׼��������
						origin.x = (pagenumer.liPage%sys.general_set.nPartRowsN)*rcWeldCard.Width();
						origin.y = -((pagenumer.liPage/sys.general_set.nPartRowsN)*rcWeldCard.Height());
					}
					else
					{	//����Ϊ��׼��������
						origin.x = (pagenumer.liPage/sys.general_set.nPartRowsN)*rcWeldCard.Width();
						origin.y = -((pagenumer.liPage%sys.general_set.nPartRowsN)*rcWeldCard.Height());
					}
				}
				if(sys.general_set.bProcessCardTbl)
				{	//���빤�տ�ͼ��
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
				//���ƹ�����ϸ��
				f3dPoint xTableAnchorPoint=cardTopRight;//f3dPoint(drawingRectLayout.GetFrameWidth()*zoomdraw_coef,0,0)+cardTopLeft;
				if(gxDrwLocalizer.ciCustomerSerial==CLIENT_QingDao_DongFang)
					//(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES||pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)&&	//��ϽǸ�ʮ�ֲ��
					
				{	//�ൺ������ϽǸ�ʮ�ֲ���麸ͼ����ϸ���������½�
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
						||!sys.part_group_map.part_table.bZoomInProcessCard)	//�麸����ϸ�����зŴ������ǿ�з����ڹ��տ����Ͻ�λ��
						pTblDrawing->CreatePartGroupTable(1);
					else
						pTblDrawing->CreatePartGroupTable(zoomdraw_coef);
					int niTblLocateStyle=1;//���Ͻ�
					if(gxDrwLocalizer.ciCustomerSerial==CLIENT_QingDao_DongFang)	//�ൺ����
						//(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES||pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)&&	//��ϽǸ�ʮ�ֲ��
						niTblLocateStyle=3;//���½�
					DrawTable(DRAGSET.RecordingBlockTableRecord(),&pTblDrawing->m_xPartTbl,FALSE,1,niTblLocateStyle);
					DRAGSET.EndBlockRecord(pBlockTableRecord,GEPOINT(xTableAnchorPoint.x,xTableAnchorPoint.y));
					f2dRect table_rect=pTblDrawing->GetDrawRegion();
					if(niTblLocateStyle!=3)
						xTableAnchorPoint.y-=table_rect.Height();
					else
						xTableAnchorPoint.y+=table_rect.Height();
				}
				if(gxDrwLocalizer.ciCustomerSerial==CLIENT_QingDao_DongFang)
					//(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_STDX_PLATES||pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ANGLEFOOT)&&	//��ϽǸ�ʮ�ֲ��
				{	//�ൺ����
					xTableAnchorPoint.x-=67;	//�ൺ�����麸ͼ�еĹ�ʱ�������ʮ�ֲ���麸ͼ��Ӧ���ڹ�����ϸ���Ϸ�
					InsertBlock(pBlockTableRecord,xTableAnchorPoint,"QDDF-WorkCost#67x46",1,0);
				}
				//�����麸ͼ
				for(pViewDrawing=(CSectionViewDrawing*)xPartGroupDraw.EnumFirstDrawingComponent();pViewDrawing;pViewDrawing=(CSectionViewDrawing*)xPartGroupDraw.EnumNextDrawingComponent())
				{
					if(pViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_BOM_VIEW)
						continue;
					DRAWRECT drawingRect=pViewDrawing->drawingRect;
					if(drawingRect.m_iPageIndex!=i)
						continue;
					//�����麸ͼ
					base_pnt=f3dPoint(drawingRect.LocatePoint())*zoomdraw_coef+cardTopLeft;
					if(pViewDrawing->m_iComponentType==CSectionViewDrawing::CLS_KM_ANGLE_VIEW)
						base_pnt.y-=drawingRect.height*0.5;	//���ƽǸֿ�ģ��ͼʱ��׼�㲻�ھ��ο����Ͻǣ����ڽǸ����λ�ü����и߶��е� wht 15-09-09
					////////////////////////////////////
					/*����ʱ���ư������
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
					//TODO:δ������ȫ����ͼ���͵�ͳһ���� double dfActualScaleOfModel2Draw=pViewDrawing->dfScaleOfModel2Draw/zoomdraw_coef;��wjh-2019.5.5
					DrawSectionViewMap(pBlockTableRecord,pViewDrawing,base_pnt,zoomdraw_coef);
					//���ƿ����Ǹ�(�������տ��в����ƿ����Ǹ�)
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
			{	//���Ʋ�����ͼ,��ʼ��ʱ�������ù���ͼֽ wht 18-09-24
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
			{	//��עʮ�ֲ���麸���ӹ���������
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 dimTextArr[4]={"process:",
										  "  1.leveling all plate before combination",
									      "  2.locating weld parts by module,flat weld near the module",
									      "  3.weld line totally enclose,level 2 weld line"};
#else
				CXhChar100 dimTextArr[4]={"�ӹ�����:",
										  "  1.���а����ǰУƽ",
									      "  2.�ÿ�ģ��λ�麸����ģ��ƽ��",
									      "  3.����ȫ��գ���������"};
				if (gxDrwLocalizer.ciCustomerSerial == XHCID::CID_QingDao_DongFang)
					dimTextArr[3] = "  3.����ȫ���";	//�ൺ��������Ҫ�ꡰ�������족���� wjh-2019.5.20
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
	if(bUseDrag)	//�����ϷŹ���
#ifdef AFX_TARG_ENU_ENGLISH
		DragEntSet(basepnt,"input the insertion point\n");
#else
		DragEntSet(basepnt,"����������\n");
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
		case 0:	//�Խ����
			pSonJg[0]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pSonJg[1]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			partset.append(pSonJg[0]);
			partset.append(pSonJg[1]);
			break;
		case 1:	//T�����
			pSonJg[0]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pSonJg[1]=(CLDSLineAngle*)Ta.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			partset.append(pSonJg[0]);
			partset.append(pSonJg[1]);
			break;
		case 2:	//ʮ�ֽ������
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
		{	//ʼ�����ڽڵ�
			line_vec=pCommJg->End()-pCommJg->Start();
			if(line_vec*ucs.axis_z>0)	//�װ��ϲ��׼�Ǹ�
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			else
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);	//�װ��²��׼�Ǹ�
		}
		else
		{	//�ն����ڽڵ�
			line_vec=pCommJg->Start()-pCommJg->End();
			if(line_vec*ucs.axis_z>0)	//�װ��ϲ��׼�Ǹ�
				GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
			else
				GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);	//�װ��²��׼�Ǹ�
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
{	//���Ƶ׽Ű�ĸ���/����ͼ
	f3dLine line;
	for(CLDSPart* pPart=plateSet.GetFirst();pPart;pPart=plateSet.GetNext())
	{
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate* pPlank=(CLDSGeneralPlate*)pPart;
		if(pPlank->vertex_list.GetNodeNum()<=0)
			continue;
		if(fabs(pPlank->ucs.axis_z*ucs.axis_z)>EPS_COS2)	//������
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
		{	//ѥ����߰�
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
			//����ţ���ʱ ����ͼ�������²�ְ� ����ͼ�������ϲ�ְ�
			BOOL bDrawPlate=FALSE;	//�ְ����ж�������Ϸ�ʱ����Ҫ����
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
				SnapPerp(&perp,base_line,vertex,NULL);	//�㵽�ߵ�ͶӰ
				vec=perp-base_line.startPt;
				if(vec*inters_line_vec>0)	//ʼ���Ҳ�
				{
					double dd=vec.mod();
					if(start_dist>dd)
						start_dist=dd;
					if(end_dist<dd)
						end_dist=dd;
				}
				else	//ʼ�����
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
			{	//�������߰�Ļ�׼���ڰ���м䣬�˴��������⴦��
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
{	//������̤�Ű����ͼ	    2015-3-23
	if(pParaBasePlate==NULL || pParaBasePlate->m_iParamType!=TYPE_BASEPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Fail to find the base plate of the specified handle!");
#else
		AfxMessageBox("û���ҵ�ָ������ĵ׽Ű�!");
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
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTableRecord *pBlockTableRecord;				//�������¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	pBlockTable->close();//�رտ��
	DRAGSET.ClearEntSet();
	long nNormOffset=0;
	pParaBasePlate->GetDesignItemValue('N',&nNormOffset);		//ָ������ƫ��
	UCS_STRU ucs=pParaBasePlate->ucs;
	ucs.origin+=ucs.axis_z*pParaBasePlate->GetThick();
	if(iViewStyle==4||iViewStyle==5)
	{	//����ͼ/����ͼ
		if(iViewStyle==5)
		{	//����ͼ
			ucs.axis_z*=-1.0;
			ucs.axis_y*=-1.0;
			ucs.origin+=ucs.axis_z*pParaBasePlate->GetThick();
		}
		DrawFootPlanView(pBlockTableRecord,pParaBasePlate->handle,plateSet,ucs,draw_scale);
		//���ƽǸ���ͼ
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
	{	//����������ͼ
		if(iViewStyle==0)	//ǰ����ͼ
		{
			ucs.axis_x.Set(1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,1,0);
		}
		else if(iViewStyle==1)	//������ͼ
		{
			ucs.axis_x.Set(-1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,-1,0);
		}
		else if(iViewStyle==2)	//������ͼ
		{
			ucs.axis_x.Set(0,1,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(-1,0,0);
		}
		else //if(iViewStyle==3)	//������ͼ
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
	DragEntSet(basepnt,"��������");
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
		AfxMessageBox("�Ҳ���ָ������ĵ׽Ű壡");
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
		AfxMessageBox("�׽Ű�ȱ������������");
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
	bool bHoofFoot=false;	//ţ���(�����������������������ӽǸֺ����Ӱ�
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
	if(bHoofFoot)	//����ţ�����һ��������ӽǸ�
	{
		CSuperSmartPtr<CLDSPart>pBaseAngle;
		pBaseAngle=Ta.FromPartHandle(foot.attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseAngle.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Fail to find the base plate of the specified handle!");
#else
			AfxMessageBox("�Ҳ���ָ������ĵ׽Ű壡");
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
	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//��õ�ǰͼ�ο���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//�رտ��
	DRAGSET.ClearEntSet();
	if(foot_dlg.m_iViewStyle==4||foot_dlg.m_iViewStyle==5)
	{	//4.����ͼ 5.����ͼ
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
		//���ƽǸָ���/����ͼ
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
	else	//����������ͼ
	{
		UCS_STRU ucs;
		ucs.origin=foot.ucs.origin;
		if(foot_dlg.m_iViewStyle==0)	//ǰ����ͼ
		{
			ucs.axis_x.Set(1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,1,0);
		}
		else if(foot_dlg.m_iViewStyle==1)	//������ͼ
		{
			ucs.axis_x.Set(-1,0,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(0,-1,0);
		}
		else if(foot_dlg.m_iViewStyle==2)	//������ͼ
		{
			ucs.axis_x.Set(0,1,0);
			ucs.axis_y.Set(0,0,-1);
			ucs.axis_z.Set(-1,0,0);
		}
		else //if(foot_dlg.m_iViewStyle==3)	//������ͼ
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
	DragEntSet(basepnt,"��������");
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
//ѡ��κ�
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
		AfxMessageBox("�����˷Ƿ��κ�");
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
//�ֶε���ͼ
void SegLine()
{
	//�������ø�ֵ
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
		CHashList<SEGI> hashIncSegs;	//��ǰ��ͼ�������ķֶμ��� wjh-2017.11.15
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
	{	//������ͼ���ý��жκ��ж� wht 10-11-23
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
		logerr.Log("%s����û��ѡ���㹻�ĺϷ��˼���ڵ�,�����Ƿ������ȷ�������ļ�!",(char*)Ta.m_iCurSeg.ToString());
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
	DragEntSet(basepnt,"��������");
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
	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//��õ�ǰͼ�ο���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//�رտ��
	DRAGSET.ClearEntSet();
	CXhChar100 sMapFrmName;
	sMapFrmName.Printf("%s",map_dlg.m_sCmbMapFrmName);
	if(!InsertBlock(pBlockTableRecord,basepnt,(char*)sMapFrmName,1.0,0,0))
	{
		MAP_FRAME_SIZE* pMapFrm=sys.m_hashMapFrameList.GetValue((char*)sMapFrmName);
		if(pMapFrm)//�����Զ���ͼ�� wht 16-01-28
		{
			int i=1,nUnitSize=100;
			f3dPoint topLeft;
			//�ڲ�ͼ��
			CreateAcadRect(pBlockTableRecord,topLeft,pMapFrm->nLength-pMapFrm->nLeftMargin-pMapFrm->nRightMargin
				,pMapFrm->nWidth-pMapFrm->nTopMargin-pMapFrm->nBottomMargin
				,FALSE,NULL,f3dPoint(0,0,0),sys.stru_map.fPolyLineWidth);
			double txt_height=min(pMapFrm->nTopMargin,pMapFrm->nBottomMargin)*0.5;
			for(i=1;i<=pMapFrm->GetInsideLen()/nUnitSize;i++)
			{	//�ϲ�̶���
				f3dPoint start(topLeft.x+i*nUnitSize,topLeft.y);
				f3dPoint end(start.x,topLeft.y+pMapFrm->nTopMargin);
				f3dPoint dim_pos(start.x-0.5*nUnitSize,start.y+0.5*txt_height);
				CreateAcadLine(pBlockTableRecord,start,end);
				CXhChar16 dim_txt("%d",i);
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
				//�²�̶���
				start.y-=(pMapFrm->GetInsideWidth()+pMapFrm->nBottomMargin);
				end.y=start.y+pMapFrm->nBottomMargin;
				dim_pos.y=start.y+0.5*txt_height;
				CreateAcadLine(pBlockTableRecord,start,end);
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextCenter,AcDb::kTextBottom);
			}
			txt_height=min(pMapFrm->nLeftMargin,pMapFrm->nRightMargin)*0.5;
			for(i=1;i<=pMapFrm->GetInsideWidth()/nUnitSize;i++)
			{	//���̶���
				f3dPoint start(topLeft.x,topLeft.y-i*nUnitSize);
				f3dPoint end(start.x-pMapFrm->nLeftMargin,start.y);
				f3dPoint dim_pos(start.x-0.5*txt_height,start.y+0.5*nUnitSize);
				CreateAcadLine(pBlockTableRecord,start,end);
				CXhChar16 dim_txt("%c",(int)('A'+i-1));
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextRight,AcDb::kTextVertMid);
				//�Ҳ�̶���
				start.x+=pMapFrm->GetInsideLen();
				end.x=start.x+pMapFrm->nRightMargin;
				dim_pos.x=start.x+0.5*txt_height;
				CreateAcadLine(pBlockTableRecord,start,end);
				DimText(pBlockTableRecord,dim_pos,dim_txt,TextStyleTable::hzfs.textStyleId,
					txt_height,0,AcDb::kTextLeft,AcDb::kTextVertMid);
			}
			//���ͼ��
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
	DragEntSet(basepnt,"��������");
#endif
#ifdef _ARX_2007
	ads_command(RTSTR,L"RE",RTNONE);
#else
	ads_command(RTSTR,"RE",RTNONE);
#endif
}
//�����ӡͼ��
#include "InputMKRectDlg.h"
AcDbObjectId SearchBlock(const char *blkname);
void InsertMKRect()
{
	CAcModuleResourceOverride resOverride;
	CXhChar16 blkname("MK");
	AcDbObjectId blockId=SearchBlock(blkname);
	if(blockId.isNull())
	{	//�½�����¼ָ�룬���ɸ�ӡ���ͼԪ
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
		AcDbBlockTableRecord *pTempBlockTableRecord=new AcDbBlockTableRecord();//�������¼ָ��
#ifdef _ARX_2007
		pTempBlockTableRecord->setName((ACHAR*)_bstr_t(CXhChar16("MK")));
#else
		pTempBlockTableRecord->setName(CXhChar16("MK"));
#endif
		pTempBlockTable->add(blockId,pTempBlockTableRecord);
		pTempBlockTable->close();
		//���ɸ�ӡ���ͼԪ
		f3dPoint topLeft(0,nRectW),dim_pos(nRectL*0.5,nRectW*0.5,0);
		CreateAcadRect(pTempBlockTableRecord,topLeft,nRectL,nRectW);	//���������
		DimText(pTempBlockTableRecord,dim_pos,CXhChar16("��ӡ��"),TextStyleTable::hzfs.textStyleId,txt_height,0,AcDb::kTextCenter,AcDb::kTextVertMid);
		pTempBlockTableRecord->close();
	}
	//����ӡͼԪ����ͼ�飬��ʾ�û�����ָ��λ��
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
		int retCode=DragEntSet(insert_pos,"��������\n");
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
//�����ͷ���帩��ͼ
#include "InputPartNoDlg.h"
void InsertWrapperSideView()
{
	//ѡ����Ҫ���ƽ�ͷ����ͼ�Ķ̽Ǹ�
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
	//������ͼͼԪ
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
	//�������ݵ�λ�ñ�ע�������
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
	DragEntSet(base_pnt,"��������\n");
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
	//����BOM�ļ�
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
	//�����κ�
	CInputAnIntegerValDlg dlg;
	dlg.m_sValTitle="��������κ�:";
	if(dlg.DoModal()!=IDOK)
		return;
	int iSelSeg=dlg.m_nVal;
	//ѡ��ǰ��DWG�ļ��е�����ͼԪ
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
	{	//��ʼ��ʵ�弯��
		acedSSName(ent_sel_set,i,entname);
		acdbGetObjectId(entId,entname);
		acdbOpenAcDbEntity(pEnt,entId,AcDb::kForRead);
		if(pEnt==NULL)
			continue;
		if(pEnt->isKindOf(AcDbCircle::desc()))
		{	//���ű�עԲȦ(R=4|6|7)
			AcDbCircle* pCircle=(AcDbCircle*)pEnt;
			double fDiametre=pCircle->radius()*2;
			f3dPoint center(pCircle->center().x,pCircle->center().y,pCircle->center().z);
			//if(fDiametre==sys.dim_map.PartNo.fPartNoCirD||fDiametre==sys.dim_map.PartNo.fMainPoleNoOuterCirD)
			if(fDiametre>=4.5&&fDiametre<=sys.dim_map.PartNo.fMainPoleNoInnerCirD)
			{	//��ͨ���Բ|���ı����Բ
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
	//��ȡ���ϱ��еĹ������
	CHashStrList <CManuPart> hashPartNo;
	while(1)
	{
		f3dPoint origin;
		ads_point org_L_T,org_R_B;
#ifdef _ARX_2007
		if(ads_getpoint(NULL,L"\n��ѡ�񹹼���ϸ������Ͻ�,<Enter�˳�>: ",org_L_T)!=RTNORM)
			break;
		if(ads_getpoint(NULL,L"\n��ѡ�񹹼���ϸ������½�,<Enter�˳�>: ",org_R_B)!=RTNORM)
			break;
		ads_command(RTSTR,L"ZOOM",RTSTR,L"e",RTNONE);	//��ȫ��ʾ���ܵ���δ��ʾ����Ļ�е�������ȡʧ�� wjh-2016.12.16
#else
		if(ads_getpoint(NULL,"\n��ѡ�񹹼���ϸ������Ͻ�,<Enter�˳�>: ",org_L_T)!=RTNORM)
			break;
		if(ads_getpoint(NULL,"\n��ѡ�񹹼���ϸ������½�,<Enter�˳�>: ",org_R_B)!=RTNORM)
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
	//������ͼԪ�м����������ƥ�����м���ԲȦ
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
				if(strstr(sText,"ǰ"))
				{
					sText.Replace("ǰ","");
					if(!pTextCir->bHasDualLabel)	//˫����еĵ�һ����ű�ע
						pTextCir->bHasDualLabel=true;
					else	//˫����еĵڶ�����ű�ע
						pTextCir->cnLeastLabelCount=0;
				}
				else if(strstr(sText,"��"))
				{
					sText.Replace("��","");
					if(!pTextCir->bHasDualLabel)	//˫����еĵ�һ����ű�ע
						pTextCir->bHasDualLabel=true;
					else	//˫����еĵڶ�����ű�ע
						pTextCir->cnLeastLabelCount=0;
				}
				else	//�����
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
	//�Ӳ��ϱ��м��������
	CLogErrorLife logErrLife;
	for(CManuPart* pPart=bom.Parts.GetFirst();pPart;pPart=bom.Parts.GetNext())
	{
		if(pPart->nSeg.iSeg!=iSelSeg)
			continue;
		CXhChar500 line_text(" %s\t%s\t\t����:%d\t\t",pPart->sPartNo,pPart->sSpec,pPart->nCount);
		CManuPart* pTemPart=hashPartNo.GetValue(pPart->sPartNo);
		if(pTemPart)
		{
			line_text.Append(CXhChar100("��ϸ��:%d\t",pTemPart->nCount));
			if(stricmp(pTemPart->sSpec,pPart->sSpec)!=0)
				line_text.Append(CXhChar100("���:%s��%s\t\t",pTemPart->sSpec,pPart->sSpec));
			if(fabs(pTemPart->length-pPart->length)>1)
				line_text.Append(CXhChar100("�������:%.2f\t\t",fabs(pTemPart->length-pPart->length)));
			if(pTemPart->nPartNum!=pPart->nPartNum)
				line_text.Append(CXhChar100("������:%d\t\t",abs(pTemPart->nPartNum-pPart->nPartNum)));
			if(fabs(pTemPart->singWeight-pPart->singWeight)>0.1)
				line_text.Append(CXhChar100("�������:%.2f",fabs(pTemPart->singWeight-pPart->singWeight)));
			if(fabs(pTemPart->sumWeight-pPart->sumWeight)>0.1)
				line_text.Append(CXhChar100("�������:%.2f",fabs(pTemPart->sumWeight-pPart->sumWeight)));
		}
		logerr.Log(line_text);
	}
}
//�Ƶ��Ǹֵ�ǰ̨
void MoveTop()
{
	while(1)
	{
		f3dLine line;
		ads_name entres;
		ads_point ptres;
#ifdef _ARX_2007
		int retCode=ads_entsel(L"\nѡ����ö��ĽǸ�����,<Enter�˳�>:",entres,ptres);
#else
		int retCode=ads_entsel( "\nѡ����ö��ĽǸ�����,<Enter�˳�>:",entres,ptres);
#endif
		if(retCode==RTCAN)
			return;
		//1.����lineʶ��һ����Χ�ڵ�ʼĩ�˶��ߣ������������ߴ�ֱ��һ�����򼰺���Χ�ڣ�ʼĩ��һ������Χһ�£�
		//2.����ʶ����Ķ���ʶ����������㣨ÿ�����㣩
		//3.����lineʶ�������������ߣ�����������<��0ͼ��pline>��֫����<��1ͼ��>��
		//4.����@3ʶ��������߼�����ʶ������˰˵㣨ÿ���ĵ㣩
		//5.��ȡ���Ǹ������㼯��ptlist����������֫���߼��ϣ������������������ɴ���NULL)
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
			{	//����CAD�߶����׼�߷���һ��
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
					pLine->erase();	//ɾ��
			}
			else if(ciRemove0Truncate1==1)
			{	//�س����еĲ�����Ϊ�����ߣ�δ�ڵ�ǰ��Χ�Ĳ������ɱ���ԭ״̬
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
			{	//����CAD�߶����׼�߷���һ��
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
					pPolyLine->erase();	//ɾ��
			}
			else if(ciRemove0Truncate1==1)
			{	//�س����еĲ�����Ϊ�����ߣ�δ�ڵ�ǰ��Χ�Ĳ������ɱ���ԭ״̬
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
					pPolyLine->erase();	//ɾ��

			}
		}
	}
}
static BOOL ProcessLineByVertexs(AcDbBlockTableRecord *pBlockRecord,const double* startCoords,const double* endCoords,BYTE ciTypeHide0Frame1Line2)
{	//1.����ʼ�ն˽ڵ�����ѡ����������������
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
	//2.��������������ѡ�������CADʵ��
	AcDbObjectId entId;
	ads_name ss_name,ent_name;
	struct resbuf *poly=NULL,*plist=NULL,*filter;
	for(f3dPoint *pPt=vertex_list.GetFirst();pPt;pPt=vertex_list.GetNext())
	{	//Ӧʹ��acutNewRb()����������new struct resbuf�½�����ֵ���棬����ʹ��acutRelRb()�ͷ��ڴ�ʱ���� wht 12-09-21
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
	//������
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
	//3.����������ֱ��
	if(ciTypeHide0Frame1Line2>0)
	{	//�Ƴ��߶�������ԭʼ��������
		TruncateAndHideLines(pBlockRecord,startPt,endPt,lineIdArr,0);
		//�Ƴ��߶κ��ٶ�Ӧ�������ֱ��
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(ciTypeHide0Frame1Line2==1)
			CreateAcadPolyLine(pBlockRecord,startPt,endPt,sys.stru_map.fPolyLineWidth);
		else if(ciTypeHide0Frame1Line2==2)
			CreateAcadLine(pBlockRecord,startPt,endPt);
	}
	else //if(ciTypeHide0Frame1Line2==0)
	{	//�ضϴ�����Ӧֱ��
		if(lineIdArr.Size()>0)
			TruncateAndHideLines(pBlockRecord,startPt,endPt,lineIdArr,1);
		if(polyLineIdArr.Size()>0)
			TruncateAndHideLines(pBlockRecord,startPt,endPt,polyLineIdArr,1);
		//����������
		GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		//��ȡֱ�ߵ�һ�ν�������
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
{	//1.ѡ�������㼯��
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
		if(acedGetPoint(NULL,L"\n��˳��׽��һ��,<Enter�˳�>:",snap_pt)==RTNORM)
#else
		if(acedGetPoint(NULL,"\n��˳��׽��һ��,<Enter�˳�>:",snap_pt)==RTNORM)
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
				if (IDNO==AfxMessageBox("\nѡ�е�����2�����˳���?",MB_YESNO))
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
	sPrompt.Printf("\nѡ��������[0.����/1.�������/2.��ֱ��]<%d>:",g_ciTypeHide0Frame1Line2);
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
	//2.���������㴦�����������ֱ��
	f3dPoint *pPt=NULL,*pPrevPt=ptList.GetFirst();
	for(pPt=ptList.GetNext();pPt;pPt=ptList.GetNext())
	{
		ProcessLineByVertexs(pBlockRecord,*pPrevPt,*pPt,g_ciTypeHide0Frame1Line2);
		pPrevPt=pPt;
	}
	pBlockRecord->close();
}
//baseΪucs�������ϵ������ֵ
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
	DragEntSet(basepnt,"��������");
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
	DragEntSet(basepnt,"��������");
#endif
	seg_map_base_face_ucs = ucs;
}
#endif

//��������棨չ���棩�ϵĹ������ϣ���Ҫ�ǽǸ�,�ֹܺͽڵ㣬���๹����
//�����ֹ����ɻ�ã�
//[����]direct��ʾ�û�����ʸ��������������ϵ��ʸ��������
//[���]nodeset,partset,ucs
static BOOL GetFacePartSet(CLDSLinePart *pSelLinePart, NODESET &nodeset,PARTSET& partset,
						   f3dPoint direct, UCS_STRU &ucs,BOOL bOnlyGetPole)
{
	CLDSLinePart *pPole;
	CLDSNode *pNode,*pStart, *pEnd,*pMirStart=NULL,*pMirEnd=NULL;
	//pole_vec���ʾ�û���ѡ�нǸֵĽǸ���ʸ��
	f3dPoint pole_vec,axis_z(0,1,0);
	f3dLine line;
	double angle;//չ������չ��ʱ����ת�Ƕȣ�ת��ΪpSelJg��
	/*��չ����ʽ{
		0����ʾչ��ǰ������ҶԳƽǸּ���棻
		1����ʾչ���Ȳ�V���������
				}*/
	int spread_style = 0;
	int nCounts = 4;
	f3dPoint pPoints[4],origin;
	fPolygonRgn selRgn;

	partset.Empty();
	nodeset.Empty();

	normalize(direct);	//����ʸ����λ��
	pStart = pSelLinePart->pStart;
	pEnd   = pSelLinePart->pEnd;
	
	if(spread_style==0)//չ��һ��Գ���
	{
		direct.y = 0;	//�û�������ΪӦ��X-Z����ƽ���ϣ�������ϵ��
		pMirStart = pStart->GetMirNode(MIRMSG(1));
		pMirEnd   = pEnd->GetMirNode(MIRMSG(1));
		//Sub_Pnt(pole_vec,pStart->Position(false),pEnd->Position(false));
		//����������û��������ϵ(���ԭ������ϵ)
		ucs.origin = pStart->Position(false);
		angle = cal_angle_of_2vec(-1.0*direct,axis_z);
		f3dPoint axis;
		axis = cross_prod(axis_z,-1.0*direct);
		ucs.axis_x.Set(1,0,0);
		ucs.axis_y.Set(0,0,-1);
		RotateVectorAroundVector(ucs.axis_x,angle,axis);
		RotateVectorAroundVector(ucs.axis_y,angle,axis);
		//�û�����ķ��������չ�����������ϵ��Z�����᷽��
		ucs.axis_z = cross_prod(ucs.axis_x,ucs.axis_y);
	}
	
	pPoints[0] = pStart->Position(false);
	pPoints[1] = pEnd->Position(false);
	if(pMirEnd!=NULL)
	{
		pPoints[2] = pMirEnd->Position(false);
		if(pMirStart!=NULL) //��㲻�غ�
			pPoints[3] = pMirStart->Position(false);
		else				//����غ�
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
		if(selRgn.PtInRgn(pNode->Position(false))==1)//���ڶ���������ڣ������߽����ڣ�
		{
			pNode->feature=0;
			nodeset.append(pNode);
		}
	}
	partset.Empty();
	if(bOnlyGetPole)
	{	//�������˼�
		for(pPole=Ta.Parts.GetFirstLinePart();pPole!=NULL;pPole=Ta.Parts.GetNextLinePart())
		{
			if(pPole->pStart==NULL||pPole->pEnd==NULL)
				continue;
			pStart = pPole->pStart;
			pEnd   = pPole->pEnd;
			line.startPt = pStart->Position(false);
			line.endPt   = pEnd->Position(false);
			if(selRgn.LineInRgn(line)==1) //���ڶ���������ڣ������߽����ڣ�
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
			{	//��
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{	//��������
					CLDSParamPlate *pParamPlate = (CLDSParamPlate*)pPart;
					if(pParamPlate->IsFL()||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND)
						continue;	//���±ߴ���(���ݸֹ���Ӷ����ӹ���)	
					CLDSNode *pBaseNode=pParamPlate->GetBaseNode();
					if(pBaseNode)
					{	//�������׼�ڵ㲻�ڶ����������(�����߽�����)�����
						if(selRgn.PtInRgn(pBaseNode->Position(false))!=1)	
							continue;
					}
					else	
					{	//������ԭ�㲻�ڶ����������(�����߽�����)
						if(selRgn.PtInRgn(pParamPlate->ucs.origin)!=1)
							continue;	
					}
					partset.append(pParamPlate);
				}
				else if(pPart->GetClassTypeId()==CLS_PLATE)
				{	//��ͨ�ְ�
					CLDSPlate *pPlate = (CLDSPlate*)pPart;
					CLDSNode *pBaseNode=Ta.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
					if(pBaseNode)
					{	//�ְ��׼�ڵ㲻�ڶ����������(�����߽�����)�����
						if(selRgn.PtInRgn(pBaseNode->Position(false))!=1)	
							continue;
					}
					else	
					{	//�ְ�ԭ�㲻�ڶ����������(�����߽�����)�����
						if(selRgn.PtInRgn(pPlate->ucs.origin)!=1)
							continue;
					}
					partset.append(pPlate);
					for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
						partset.append(pLsRef->GetLsPtr());
				}
				else	
				{	//��������	���壬���ι����ȵ�
					if(selRgn.PtInRgn(pPart->ucs.origin)!=1)//����ԭ�㲻�ڶ����������(�����߽�����)�����
						continue;
					partset.append(pPart);
				}
			}
			else
			{	//�˼�
				CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
				pStart = pLinePart->pStart;
				pEnd   = pLinePart->pEnd;
				if(pStart==NULL||pEnd==NULL)
					continue;	//�������˼�
				line.startPt = pStart->Position(false);
				line.endPt   = pEnd->Position(false);
				if(selRgn.LineInRgn(line)!=1) //�߲��ڶ����������(�����߽�����)�����
					continue;
				partset.append(pLinePart);
				if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
				{	//��Ӷ˰弰�丽����������ʾ�б�
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
				{	//��ӽǸ��ϵ���˨����ʾ�б�
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
	THANDLE handle;//HANDLE����
	//ͨ��ѡ��ʵ���ȡʵ���ID
	AcDbObjectId entId,dimStyleId;
	AcDbEntity *pEnt;
	AcGePoint3d sel_pnt,start,end;				    //�û�����ĵ� 
	f3dPoint f_sel_pnt,f_start,f_end,direct;
	ads_name entres;
    ads_point ptres;
	UCS_STRU ucs;
	PARTSET poleset;
	NODESET nodeset;

aa:
	//ѡ��ֶ�ͼ�����浥��ͼ�е�һ���Ǹ֣�Ҳ��չ�����ϵĽǸ֣�������entres
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	if (ads_entsel(L"\nPlease select a angle line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\nPlease select a angle line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#endif
#else
#ifdef _ARX_2007
	if (ads_entsel(L"\n���ڵ�������ͼ��ѡ��չ�����ϵ�һ���Ǹ���,<Enter�˳�>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\n���ڵ�������ͼ��ѡ��չ�����ϵ�һ���Ǹ���,<Enter�˳�>:",entres,ptres)==RTNORM)
#endif
#endif
	{
		//ȡ��ԓ�ڵ��HANDLE��
		acdbGetObjectId(entId, entres);	//��ò�׽����ͼԪ��ID��
		acdbOpenObject(pEnt,entId,AcDb::kForRead);//��ͼԪ��
		if(!pEnt->isKindOf(AcDbLine::desc()))//�ж��Ƿ���һ��ֱ��
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\n The selected line is not angle line");
#else
			AfxMessageBox("\nѡ�еĲ��ǽǸ���");
#endif
			pEnt->close();
			goto aa;
		}
		handle=GetTaAtomHandle(pEnt);	//��ȡѡ���ͼԪ���
		pSelPole = (CLDSLinePart*)Ta.FromPartHandle(handle,CLS_LINEPART);
		if(pSelPole!=NULL&&pSelPole->IsPart())	//�û�ѡ�е�ȷʵ��һ���Ǹ�
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
			if (ads_getpoint(NULL,L"\n�������ӵ�λ��:",asDblArray(sel_pnt))==RTNORM)
#else
			if (ads_getpoint(NULL,"\n�������ӵ�λ��:",asDblArray(sel_pnt))==RTNORM)
#endif
#endif
			{
				f3dPoint perp;	//�û���ȡ���ڽǸ����ϵĴ����
				f3dPoint base;	//������չ��ʱ�Ĳ��������(ucs����ϵ)
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
		else	//�û�ѡ�е�ֱ�߲��Ǵ��о����Ϣ�ĽǸ���
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\nThe selected line is wrong angle line");
#else
			AfxMessageBox("\nѡ�еĲ�����ȷ�ĽǸ���");
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
		AfxMessageBox("\nû��ѡ�к��ʵĽǸ���");
#endif
}


void SpreadFaceStru()
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
{
	CLDSLinePart *pSelPole=NULL;
	THANDLE handle;//HANDLE����
	//ͨ��ѡ��ʵ���ȡʵ���ID
	AcDbObjectId entId,dimStyleId;
	AcDbEntity *pEnt;
	AcGePoint3d sel_pnt,start,end;				    //�û�����ĵ� 
	f3dPoint f_sel_pnt,f_start,f_end,direct;
	ads_name entres;
    ads_point ptres;
	UCS_STRU ucs;
	PARTSET partset;
	NODESET nodeset;
	
aa:
	//ѡ��ֶ�ͼ�����浥��ͼ�е�һ���˼���Ҳ��չ�����ϵĸ˼���������entres
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	if (ads_entsel(L"\nPlease select a rod line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\nPlease select a rod line of spread plane in line front map,<Enter Exit>:",entres,ptres)==RTNORM)
#endif
#else
#ifdef _ARX_2007
	if (ads_entsel(L"\n���ڵ�������ͼ��ѡ��չ�����ϵ�һ���˼���,<Enter�˳�>:",entres,ptres)==RTNORM)
#else
	if (ads_entsel("\n���ڵ�������ͼ��ѡ��չ�����ϵ�һ���˼���,<Enter�˳�>:",entres,ptres)==RTNORM)
#endif
#endif
	{
		//ȡ��ԓ�ڵ��HANDLE��
		acdbGetObjectId(entId, entres);	//��ò�׽����ͼԪ��ID��
		acdbOpenObject(pEnt,entId,AcDb::kForRead);//��ͼԪ��
		if(!pEnt->isKindOf(AcDbLine::desc()))//�ж��Ƿ���һ��ֱ��
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\nThe selected line is not straight line");
#else
			AfxMessageBox("\nѡ�еĲ���ֱ��");
#endif
			pEnt->close();
			goto aa;
		}
		handle=GetTaAtomHandle(pEnt);	//��ȡѡ���ͼԪ���
		pSelPole = (CLDSLinePart*)Ta.FromPartHandle(handle,CLS_LINEPART);
		if(pSelPole!=NULL&&pSelPole->IsLinePart())	//�û�ѡ�е�ȷʵ��һ���˼�
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
			if (ads_getpoint(NULL,L"\n�������ӵ�λ��:",asDblArray(sel_pnt))==RTNORM)
#else
			if (ads_getpoint(NULL,"\n�������ӵ�λ��:",asDblArray(sel_pnt))==RTNORM)
#endif
#endif
			{
				f3dPoint perp;	//�û���ȡ���ڽǸ����ϵĴ����
				f3dPoint base;	//������չ��ʱ�Ĳ��������(ucs����ϵ)
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
		else	//�û�ѡ�е�ֱ�߲��Ǵ��о����Ϣ�ĽǸ���
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("\n The selected rod is wrong");
#else
			AfxMessageBox("\nѡ�еĲ�����ȷ�ĸ˼�");
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
		AfxMessageBox("\nû��ѡ�к��ʵĸ˼�");
#endif
}
#else
{}
#endif
