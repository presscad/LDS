//<LOCALE_TRANSLATE/>
#include "stdafx.h"
#include "Tower.h"
#include "TubeDrawing.h"
#include "partgroupdrawing.h"
#include "SortFunc.h"
#include "DrawUnit.h"
#include "SysPara.h"
#include "LogFile.h"
#include "PartStatRecord.h"
#include "DimPlankLs.h"
#include ".\JgDrawing.h"
#include "DimPlateBolts.h"
#include "ArrayList.h"
#include "ComparePartNoString.h"
#include "btc.h"
#include "SortFunc.h"
#include "dbpl.h"
#include "DrawDim.h"
#include "PartGroupRectLayout.h"
#include ".\LdsDrwLocale.h"
#include "ClientId.h"

#ifndef __TSA_FILE_
//////////////////////////////////////////////////////////////////////
// ȫ�ֺ���
//////////////////////////////////////////////////////////////////////
typedef CPartGroupLocatePoint* LOCATEPOINT_PTR;
//���궨������ת������ͼ����ϵ��
static BOOL TransLocatePointPosTocs(CTower *pTower, UCS_STRU cs, const LOCATEPOINT_PTR &pLocatePoint, f3dPoint &locate_point)
{
	if(pTower==NULL||pLocatePoint==NULL)
		return FALSE;
	locate_point=pLocatePoint->m_xLocalCoordPos;
	CLDSPart *pPart=pTower->FromPartHandle(pLocatePoint->m_hPart);
	if(pPart)
	{
		if(pPart->GetClassTypeId()==CLS_LINETUBE)
			((CLDSLineTube*)pPart)->BuildUCS();
		else if(pPart->GetClassTypeId()==CLS_LINESLOT)
			((CLDSLineSlot*)pPart)->BuildUCS();
		else if(pPart->GetClassTypeId()==CLS_BOLT)
			((CLDSBolt*)pPart)->GetUCS();
		coord_trans(locate_point,pPart->ucs,TRUE);
		/*if(pPart->IsLinePart())
		{
			if(pLocatePoint->m_fStartOffset!=0)
				locate_point+=pPart->ucs.axis_z*pLocatePoint->m_fStartOffset;
			if(pLocatePoint->m_fEndOffset!=0)
				locate_point+=pPart->ucs.axis_z*pLocatePoint->m_fEndOffset;
		}*/
	}
	coord_trans(locate_point,cs,FALSE);
	locate_point.y=locate_point.z=0;
	return TRUE;
}
static int compareLocatePoint(const CPartGroupLocatePoint &locatePoint1,const CPartGroupLocatePoint &locatePoint2)
{
	//if(pLocatePoint1==NULL||pLocatePoint2==NULL||pLocatePoint1->pTower==NULL||pLocatePoint2->pTower==NULL)
	//	return 0;
	//f3dPoint point1,point2;
	//TransLocatePointPosTocs(pLocatePoint1->pTower,pLocatePoint1->cs,pLocatePoint1,point1);
	//TransLocatePointPosTocs(pLocatePoint2->pTower,pLocatePoint2->cs,pLocatePoint2,point2);
	if(locatePoint1.m_fLenPosToDatumPart>=locatePoint2.m_fLenPosToDatumPart)
		return 1;
	else if(locatePoint1.m_fLenPosToDatumPart<locatePoint2.m_fLenPosToDatumPart)
		return -1;
	return 0;
}

static f3dArcLine LineToArc(f3dLine &line)
{
	f3dArcLine arcline;
	arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
	arcline.feature=line.feature;
	arcline.pen=line.pen;
	arcline.ID=line.ID;
	return arcline;
}

static SCOPE_STRU GetViewDrawingScope(ATOM_LIST<f3dArcLine> &edgeList)
{	
	SCOPE_STRU scope;
	for(f3dArcLine *pLine=edgeList.GetFirst();pLine;pLine=edgeList.GetNext())
	{
		scope.VerifyVertex(pLine->Start());
		scope.VerifyVertex(pLine->End());
		if(pLine->SectorAngle()>0&&!pLine->WorkNorm().IsZero())
		{	//���ΪԲ�����㷶Χʱ����Բ����
			scope.VerifyVertex(pLine->Center());
			f3dPoint point=pLine->Start();
			for(int i=0;i<8;i++)
			{	//���Բ���ϵİ˸���
				rotate_point_around_axis(point,Pi/4.0,pLine->Center(),pLine->Center()+pLine->WorkNorm()*1000);
				scope.VerifyVertex(point);
			}
		}
	}
	scope.fMaxX+=20;
	scope.fMinX-=20;
	scope.fMaxY+=20;
	scope.fMinY-=20;
	return scope;
}
///////////////////////////////////////////////////////////////////////
// ACAD_HATCH
///////////////////////////////////////////////////////////////////////
void ACAD_HATCH::ZoomScale(double zoomCoef)
{
	for(f3dPoint *pPt=vertexList.GetFirst();pPt;pPt=vertexList.GetNext())
		*pPt*=zoomCoef;
	fScale*=zoomCoef;
}
//////////////////////////////////////////////////////////////////////
// CPartGroupDrawing
//////////////////////////////////////////////////////////////////////
DWORD CPartGroupDrawing::DimPartNo			 = 0x00000001;	//�Զ���ע�������
DWORD CPartGroupDrawing::LayoutViews		 = 0x00000002;	//�Զ�����麸����ͼ
DWORD CPartGroupDrawing::DimAbsoluteDist	 = 0x00000004;	//��ע��λ����Գߴ�
DWORD CPartGroupDrawing::DimPlateBetweenAngle= 0x00000008;	//��ע�Ƕ���
DWORD CPartGroupDrawing::DimWeldPosLocateLine= 0x00000010;	//��ע����ǰ��λ��
DWORD CPartGroupDrawing::DimWeldPosCheckLine = 0x00000020;	//��ע���Ӻ�����
DWORD CPartGroupDrawing::DimPlateRayLineAngle= 0x00000040;	//��ע�ְ������Լ��������׼��֮��ļн�
DWORD CPartGroupDrawing::ModulatePartGroupLen= 0x00000080;	//�Զ�ѹ���麸ͼ����
DWORD CPartGroupDrawing::CreatePartGroupBOM	 = 0x00000100;	//���ɹ�����ϸ��
DWORD CPartGroupDrawing::DispPartGroupNo	 = 0x00000200;	//�麸�����
DWORD CPartGroupDrawing::DispPartGroupNum	 = 0x00000400;	//�麸������
DWORD CPartGroupDrawing::DispPartNo			 = 0x00000800;	//�������
DWORD CPartGroupDrawing::DispPartNum		 = 0x00001000;	//��������
DWORD CPartGroupDrawing::DispPartGuiGe		 = 0x00002000;	//�������
DWORD CPartGroupDrawing::DispPartLen		 = 0x00004000;	//��������
DWORD CPartGroupDrawing::DispPartWeight		 = 0x00008000;	//���������Լ�ͬһ��Ź�������С�� ����
DWORD CPartGroupDrawing::DispNotes			 = 0x00010000;	//��ע
DWORD CPartGroupDrawing::DispInternal		 = 0x00020000;	//���켶������
DWORD CPartGroupDrawing::DispAppearance		 = 0x00040000;	//���켶�����

//�麸�������Զ��������Ȼ���ʱ�ĳߴ绻��
//fromReal=TRUE:����ʵ�ʳ���,�����ʾ����
//fromReal=FALSE:������ʾ����,���ʵ�ʳ���
double CPartGroupDrawing::LenCompressTrans(double len,BOOL fromReal/*=TRUE*/,double zoomThreshold/*=0.0*/)
{
	double zoomcoef=xDrawSet.fPartGroupDistZoomCoef;
	if(len<=zoomThreshold)
		return  len;
	else
	{
		if(fromReal)
		{
			if(len>0)
				return zoomcoef*(len-zoomThreshold)+zoomThreshold;
			else	//<0
				return zoomcoef*(len+zoomThreshold)-zoomThreshold;
		}
		else
		{
			if(len>0)
				return (len-zoomThreshold)/zoomcoef+zoomThreshold;
			else
				return (len+zoomThreshold)/zoomcoef-zoomThreshold;
		}
	}
}
IDrawingComponent* CreateSectionViewDrawing(int idViewType,DWORD key,void* pContext)
{
	IDrawingComponent* pDrawComponent=NULL;//new CSectionViewDrawing(key);
	DrawingComponentContext* pDrawingContext=(DrawingComponentContext*)pContext;
	if(idViewType==CSectionViewDrawing::CLS_FL_VIEW)
		pDrawComponent = new CSectionFlDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_SLAVE_VIEW)
		pDrawComponent = new CSectionSlaveDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_BOM_VIEW)
		pDrawComponent = new CPartGroupTblDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_JOINT_VIEW)
		pDrawComponent = new CSectionJointDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_STDX_PLATES_VIEW)
		pDrawComponent = new CSectionStdXPlatesDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_ANGLE_FOOT_VIEW)
		pDrawComponent = new CSectionAngleFootDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_KM_ANGLE_VIEW)
		pDrawComponent = new CSectionKmAngleViewDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_ANGLE_SPREAD_VIEW)
		pDrawComponent = new CSectionAngleViewDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_BRIEFTUBE_VIEW)
		pDrawComponent = new CSectionBriefTubeDrawing(key,pDrawingContext->pParentDrawing);
	else if(idViewType==CSectionViewDrawing::CLS_TUBE_ENDPLATE_VIEW)
		pDrawComponent = new CSectionTubeEndPlateViewDrawing(key,pDrawingContext->pParentDrawing);
	else
		pDrawComponent = new CSectionViewDrawing(key,pDrawingContext->pParentDrawing);
	return pDrawComponent;
}
BOOL DeleteSectionViewDrawing(IDrawingComponent* pSectionViewDrawing)
{
	if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_FL_VIEW)
		delete (CSectionFlDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_SLAVE_VIEW)
		delete (CSectionSlaveDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_BOM_VIEW)
		delete (CPartGroupTblDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_JOINT_VIEW)
		delete (CSectionJointDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_STDX_PLATES_VIEW)
		delete (CSectionStdXPlatesDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_ANGLE_FOOT_VIEW)
		delete (CSectionAngleFootDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_KM_ANGLE_VIEW)
		delete (CSectionKmAngleViewDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_ANGLE_SPREAD_VIEW)
		delete (CSectionAngleViewDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_BRIEFTUBE_VIEW)
		delete (CSectionBriefTubeDrawing*)pSectionViewDrawing;
	else if(pSectionViewDrawing->GetComponentType()==CSectionViewDrawing::CLS_TUBE_ENDPLATE_VIEW)
		delete (CSectionTubeEndPlateViewDrawing*)pSectionViewDrawing;
	else
		delete (CSectionViewDrawing*)pSectionViewDrawing;
	return TRUE;
}
IDrawingCommand* CreateStruDrawingCommand(int idCmdType,DWORD key,void* pContext)
{
	IDrawingCommand* pDrawCmd=NULL;//new CSectionViewDrawing(key);
	DrawingCommandContext* pDrawingContext=(DrawingCommandContext*)pContext;
	if(idCmdType==CStruDrawingCommand::CLS_PART_GENERAL_DRAW_CMD)
		pDrawCmd = new CPartGeneralDrawingCommand(key,pDrawingContext);
	else if(idCmdType==CStruDrawingCommand::CLS_PART_UPRIGHT_DRAW_CMD)
		pDrawCmd = new CPartUprightDrawingCommand(key,pDrawingContext);
	else if(idCmdType==CStruDrawingCommand::CLS_JOINT_TUBE_DIM_CMD)
		pDrawCmd = new CJointTubeDimDrawingCommand(key,pDrawingContext);
	else //if(idViewType==CStruDrawingCommand::CLS_BASIC_CMD)
		pDrawCmd = new CStruDrawingCommand(key,pDrawingContext);
	return pDrawCmd;
}
BOOL DeleteStruDrawingCommand(IDrawingCommand* pDrawingCommand)
{
	if(pDrawingCommand->GetDrawCmdType()==CStruDrawingCommand::CLS_PART_GENERAL_DRAW_CMD)
		delete (CPartGeneralDrawingCommand*)pDrawingCommand;
	else if(pDrawingCommand->GetDrawCmdType()==CStruDrawingCommand::CLS_PART_UPRIGHT_DRAW_CMD)
		delete (CPartUprightDrawingCommand*)pDrawingCommand;
	else if(pDrawingCommand->GetDrawCmdType()==CStruDrawingCommand::CLS_JOINT_TUBE_DIM_CMD)
		delete (CJointTubeDimDrawingCommand*)pDrawingCommand;
	else //if(pDrawingCommand->GetDrawCmdType()==CStruDrawingCommand::CLS_BASIC_CMD)
		delete (CStruDrawingCommand*)pDrawingCommand;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////
//CSectionDrawingEntsManager
/////////////////////////////////////////////////////////////////////////////////
CSectionViewDrawing* CSectionDrawingEntsManager::ValidSectDrawing(){
	CSectionViewDrawing* pValidSectDrawing=NULL;
	if(cStoreType==XERO_COMMAND)
		pValidSectDrawing=(CSectionViewDrawing*)context.pXeroCmd->BelongDrawingComponent();
	else if(cStoreType==XERO_COMPONENT)
		pValidSectDrawing=(CSectionViewDrawing*)context.pSectDrawing;
	return pValidSectDrawing;
}
void CSectionDrawingEntsManager::InitContext(CLDSPart* pRootPart,IDrawingCommand* pXeroCmd,STORE_MODE cStoreMode/*=STORE_ACAD*/)
{
	m_pRootPart=pRootPart;
	CDrawingEntsContext::InitContext(pXeroCmd,cStoreMode);
	if(pXeroCmd)
		m_fUser2ScrDrawScale=((CSectionViewDrawing*)pXeroCmd->BelongDrawingComponent())->GetInternalDrawingScale();
}
void CSectionDrawingEntsManager::InitContext(CLDSPart* pRootPart,CSectionViewDrawing* pXeroComponent,STORE_MODE cStoreMode/*=STORE_ACAD*/)
{
	m_pRootPart=pRootPart;
	CDrawingEntsContext::InitContext(pXeroComponent,cStoreMode);
	if(pXeroComponent)
		m_fUser2ScrDrawScale=pXeroComponent->GetInternalDrawingScale();
}
void CSectionDrawingEntsManager::InitContext(CLDSPart* pRootPart,IDrawing* pXeroComponent,STORE_MODE cStoreMode/*=STORE_ACAD*/)
{
	m_pRootPart=pRootPart;
	CDrawingEntsContext::InitContext(pXeroComponent,cStoreMode);
}
void CSectionDrawingEntsManager::AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,
												  double fDimR/*=0*/,char *dimText/*=NULL*/,double fDimAngleTextSize/*=3.0*/)
{
	CSectionViewDrawing* pSectDrawing=NULL;
	if(m_cStoreMode!=STORE_ACAD)
		return CDrawingEntsContext::AppendAngularDim(vertex,start_vec,end_vec,fDimR,dimText,fDimAngleTextSize);
	else
		pSectDrawing=ValidSectDrawing();
	if(pSectDrawing==NULL)
		return ;//NULL;
	UCS_STRU cs=pSectDrawing->GetCS();
	CSizeAngleDim angularDim;
	CSizeAngleDim *pAngularDim=&angularDim;
	double cosa=start_vec*end_vec;
	//��������ظ��Ƕȱ�ע wht 10-08-08
	f3dPoint cur_start_vec=start_vec,cur_end_vec=end_vec;
	vector_trans(cur_start_vec,cs,FALSE);
	vector_trans(cur_end_vec,cs,FALSE);
	cur_start_vec.z=cur_end_vec.z=0;
	f3dPoint cur_vertex=vertex;
	coord_trans(cur_vertex,cs,FALSE);
	cur_vertex.z=0;
	if(pSectDrawing->CheckSameAngularDim(cur_vertex,cur_start_vec,cur_end_vec))
		return;
	double angle=ACOS(start_vec*end_vec);
	if(fabs(angle)<2*RADTODEG_COEF)
		return;	//�нǶ�����С
	if(fDimR<=0)	//Pi=3.14
	{
		double half_angle=0.5*ACOS(start_vec*end_vec);
		if(half_angle<1.57)
			fDimR = TransToModelLength(sys.dim_map.fDimAngleTextSize*0.7*4)/tan(half_angle);
	}

	//�Ƕ����ǵ�
	pAngularDim->vertex=vertex;
	coord_trans(pAngularDim->vertex,cs,FALSE);
	pAngularDim->vertex.z=0;
	//����ʼ�߱�ǵ�
	pAngularDim->dimStartPos = vertex+start_vec*fDimR*0.4;
	coord_trans(pAngularDim->dimStartPos,cs,FALSE);
	pAngularDim->dimStartPos.z=0;
	//��ֹ�߱�ǵ�
	pAngularDim->dimEndPos = vertex+end_vec*fDimR*0.4;
	coord_trans(pAngularDim->dimEndPos,cs,FALSE);
	pAngularDim->dimEndPos.z=0;
	//��עλ��Բ����ǵ�
	f3dPoint startPos=vertex+start_vec*fDimR;
	f3dPoint endPos=vertex+end_vec*fDimR;
	f3dPoint dim_vec=(startPos+endPos)/2.0-vertex;
	normalize(dim_vec);
	pAngularDim->dimPos=vertex+dim_vec*fDimR;
	coord_trans(pAngularDim->dimPos,cs,FALSE);
	pAngularDim->dimPos.z=0;
	if(dimText)//ָ����ע����
		pAngularDim->dimText.Copy(dimText);
	pSectDrawing->AppendAngularDim(angularDim);
}
void CSectionDrawingEntsManager::AppendAngularDim(CSizeAngleDim& angularDim)
{
	if (m_cStoreMode == STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing = ValidSectDrawing();
		if (pSectDrawing)
			return pSectDrawing->AppendAngularDim(angularDim);
		else
			return;
	}
	else
		return CDrawingEntsContext::AppendAngularDim(angularDim);
}
void CSectionDrawingEntsManager::AppendLine(f3dLine& line)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		if(pSectDrawing)
			return pSectDrawing->AppendLine(line);
		else
			return;
	}
	else 
		return CDrawingEntsContext::AppendLine(line);
}
void CSectionDrawingEntsManager::AppendLine(f3dArcLine& arcline)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		if(pSectDrawing)
			return pSectDrawing->AppendLine(arcline);
		else
			return;
	}
	else
		return CDrawingEntsContext::AppendLine(arcline);
}
void CSectionDrawingEntsManager::AppendSpLine(fPtList& vertex_list)
{
	if(m_cStoreMode==STORE_ACAD)
		return;
	else 
		return CDrawingEntsContext::AppendSpLine(vertex_list);
}
void CSectionDrawingEntsManager::AppendCircle(f3dCircle& circle)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		if(pSectDrawing)
			return pSectDrawing->AppendCircle(circle);
		else
			return;
	}
	else 
		return CDrawingEntsContext::AppendCircle(circle);
}
void CSectionDrawingEntsManager::AppendSizeTextDim(CSizeTextDim& sizeTextDim)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		if(pSectDrawing)
			pSectDrawing->AppendSizeTextDim(sizeTextDim);
		//else
			return;
	}
	else 
		return CDrawingEntsContext::AppendSizeTextDim(sizeTextDim);
}
void CSectionDrawingEntsManager::AppendTextDim(CTextOnlyDrawing& pureTextDim)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		if(pSectDrawing)
			return pSectDrawing->AppendTextDim(pureTextDim);
		else
			return;
	}
	else
		return CDrawingEntsContext::AppendTextDim(pureTextDim);
}
//�������ݱ�ʶ��
void CSectionDrawingEntsManager::AppendDataPoint(CTextFrame& dataPoint)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		if(pSectDrawing)
			return pSectDrawing->AppendDataPoint(dataPoint);
		else
			return;
	}
	else 
		return CDrawingEntsContext::AppendDataPoint(dataPoint);
}
DATAPOINT_TAG* CSectionDrawingEntsManager::AppendDataPoineTag()
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		if(pSectDrawing)
			return pSectDrawing->AppendDataPoineTag();
		else
			return NULL;
	}
	else 
		return CDrawingEntsContext::AppendDataPoineTag();
}
//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
double CSectionDrawingEntsManager::ZoomToModelTextSize(double fSysFontSize)
{
	if(cStoreType==XERO_COMPONENT&&context.pSectDrawing!=NULL)
		return ((CSectionViewDrawing*)context.pSectDrawing)->ZoomToModelTextSize(fSysFontSize);
	else if(cStoreType==XERO_COMMAND&&context.pXeroCmd!=NULL)
		return ((CSectionViewDrawing*)context.pXeroCmd->BelongDrawingComponent())->ZoomToModelTextSize(fSysFontSize);
	else
		return fSysFontSize;
}
//�ֹܶ���ͼ�Ļ���Ǳ�ע��׼��
void CSectionDrawingEntsManager::AppendTubeBaseLine(BYTE iDrawMode,CLDSLineTube* pBaseTube,f3dPoint arrowVec,f3dPoint arrowBtm,f3dPoint arrowHead)
{
	CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
	if(pSectDrawing==NULL)
		return;
	pSectDrawing->tube_base_line.m_pBaseTube=pBaseTube;
	pSectDrawing->tube_base_line.m_iDraw=iDrawMode;
	pSectDrawing->tube_base_line.m_xArrowStdVec=arrowVec;
	pSectDrawing->tube_base_line.m_xArrowBtm=arrowBtm;
	pSectDrawing->tube_base_line.m_xArrowHead=arrowHead;
}
void CSectionDrawingEntsManager::AppendTubeTaCoreVecLine(f3dPoint vec,f3dPoint xBtm,f3dPoint xHead)
{
	CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
	if(pSectDrawing==NULL)
		return;
	pSectDrawing->tube_base_line.m_xTaCoreVec=vec;
	pSectDrawing->tube_base_line.m_xTaCoreVecBtm=xBtm;
	pSectDrawing->tube_base_line.m_xTaCoreVecHead=xHead;
}
//���ƺ�����
void CSectionDrawingEntsManager::DrawWeldLineSymbol(f3dPoint start, f3dPoint end,double width/*=0*/,double height/*=0*/)	
{	
	if(sys.m_iCurMapType==5&&!sys.part_group_map.bDrawWeldLineSymbol)
		return;	//�����麸ͼ�л��ƺ����߱��
	CDrawingEntsContext::DrawWeldLineSymbol(f3dLine(start,end),start,width,height);
}
void CSectionDrawingEntsManager::DrawWeldLineSymbol(f3dArcLine arcline,double width/*=0*/,double height/*=0*/)
{
	if(sys.m_iCurMapType==5&&!sys.part_group_map.bDrawWeldLineSymbol)
		return;	//�����麸ͼ�л��ƺ����߱��
	CDrawingEntsContext::DrawWeldLineSymbol(arcline,width,height);
}
double CSectionDrawingEntsManager::TransToModelLength(double scr_draw_length)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		CSectionViewDrawing* pSectDrawing=ValidSectDrawing();
		 if(pSectDrawing==NULL)
			return 1.0;
		 else
			return pSectDrawing->TransToModelLength(scr_draw_length);
	}
	else
		return CDrawingEntsContext::TransToModelLength(scr_draw_length);
}
void CSectionDrawingEntsManager::UpdateVerticalPlateParallelLine(GELINE line)
{
	f3dPoint line_vec=line.end-line.start;
	normalize(line_vec);
	VERTICAL_PLATE *pVertPlate=NULL;
	for(pVertPlate=m_hashVertPlateList.GetFirst();pVertPlate;pVertPlate=m_hashVertPlateList.GetNext())
	{
		f3dPoint base_line_vec=pVertPlate->face_line[0][0].startPt-pVertPlate->face_line[0][0].endPt;
		normalize(base_line_vec);
		double dd=base_line_vec*line_vec;
		if(fabs(base_line_vec*line_vec)<0.9999)	//EPS_COS
			continue;
		//����ֱ����ְ�ƽ��
		f3dPoint perp;
		double fDist=0;
		if(SnapPerp(&perp,pVertPlate->face_line[0][0],line.start,&fDist))
		{
			if(pVertPlate->fParallelDist==0||pVertPlate->fParallelDist>fDist)
			{
				pVertPlate->fParallelDist=fDist;
				pVertPlate->parallel_line.startPt=line.start;
				pVertPlate->parallel_line.endPt=line.end;
			}
		}
	}
}
void CSectionDrawingEntsManager::UpdateVerticalPlateLocateBoltPos(GEPOINT bolt_pos)
{
	VERTICAL_PLATE *pVertPlate=NULL;
	f3dPoint startPt,endPt;
	for(pVertPlate=m_hashVertPlateList.GetFirst();pVertPlate;pVertPlate=m_hashVertPlateList.GetNext())
	{
		startPt=pVertPlate->face_line[0][0].startPt;
		endPt=pVertPlate->face_line[0][0].endPt;
		if(pVertPlate->pPlate->GetFaceN()==3)
			endPt=pVertPlate->face_line[2][0].endPt;
		else if(pVertPlate->pPlate->GetFaceN()==2)
			endPt=pVertPlate->face_line[1][0].endPt;
		if(pVertPlate->startBoltPos.z>0||DISTANCE(startPt,pVertPlate->startBoltPos)>DISTANCE(startPt,bolt_pos))
			pVertPlate->startBoltPos=bolt_pos;
		if(pVertPlate->endBoltPos.z>0||DISTANCE(endPt,pVertPlate->endBoltPos)>DISTANCE(endPt,bolt_pos))
			pVertPlate->endBoltPos=bolt_pos;
	}
}
//////////////////////////////////////////////////////////////////////////////////
//CPartGroupDrawing
//////////////////////////////////////////////////////////////////////////////////
CPartGroupDrawing::CPartGroupDrawing(void)
{
	m_pPartGroup=NULL;
	m_pRootPart=NULL;
	m_pTower=NULL;
	drawStyleFlag.Clear();
	drawStyleFlag.SetMaskBitCount(0);
	m_bAutoLayoutViewIgnoreScale=false;//������ָ����ͼ�����Զ����ָ��麸����ͼ(�������¼����ͼ����)
	//��ʼ���麸�����Ʒ��
	if(sys.part_group_map.bAutoDimPartNo)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DimPartNo);			//�Զ���ע�������
	if(sys.part_group_map.bAutoMatchView)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::LayoutViews);			//�Զ�����麸����ͼ
	if(sys.part_group_map.bDimAbsoluteDist)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DimAbsoluteDist);		//��ע��λ����Գߴ�
	if(sys.part_group_map.bAngularDimension)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DimPlateBetweenAngle);//��ע�Ƕ���
	if(sys.part_group_map.bDimBeforeWeldLocatSpace)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DimWeldPosLocateLine);//��ע����ǰ��λ��
	if(sys.part_group_map.bDimAfterWeldCheckLine)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DimWeldPosCheckLine);	//��ע���Ӻ�����
	if(sys.part_group_map.bDimPlatePoleRayAndAngle)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DimPlateRayLineAngle);//��ע�ְ������Լ��������׼��֮��ļн�
	if(sys.part_group_map.bModulatePartGroupLen)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::ModulatePartGroupLen);//�Զ�ѹ���麸ͼ����
	if(sys.part_group_map.bCreatePartGroupBOM)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::CreatePartGroupBOM);	//���ɹ�����ϸ��
	if(sys.part_group_map.part_table.bDispPartGroupNo)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispPartGroupNo);		//�麸�����
	if(sys.part_group_map.part_table.bDispPartGroupNum)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispPartGroupNum);	//�麸������
	if(sys.part_group_map.part_table.bDispPartNo)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispPartNo);			//�������
	if(sys.part_group_map.part_table.bDispPartNum)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispPartNum);			//��������
	if(sys.part_group_map.part_table.bDispPartGuiGe)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispPartGuiGe);		//�������
	if(sys.part_group_map.part_table.bDispPartLen)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispPartLen);			//��������
	if(sys.part_group_map.part_table.bDispPartWeight)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispPartWeight);		//���������Լ�ͬһ��Ź�������С�� ����
	if(sys.part_group_map.part_table.bDispNotes)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispNotes);			//��ע
	if(sys.part_group_map.part_table.bDispInternal)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispInternal);		//���켶������
	if(sys.part_group_map.part_table.bDispAppearance)
		drawStyleFlag.SetFlagDword(CPartGroupDrawing::DispAppearance);		//���켶�����
	listComponents.CreateNewAtom=::CreateSectionViewDrawing;
	listComponents.DeleteAtom=DeleteSectionViewDrawing;
	listCmds.CreateNewAtom=CreateStruDrawingCommand;
	listCmds.DeleteAtom=DeleteStruDrawingCommand;
	InitDrawSetting();
}
void CPartGroupDrawing::InitDrawSetting()
{
	xDrawSet=sys.part_group_map;
	m_fRealToDraw=sys.general_set.WeldMapScale.fRealToDraw;
	m_fMajorRodViewScaleRealToDraw=m_fSectViewScaleRealToDraw=m_fRealToDraw;
	m_bAutoLayoutViewIgnoreScale=sys.general_set.bProcessCardTbl!=0;
}

CPartGroupDrawing::~CPartGroupDrawing(void)
{
}
static BOOL IsIncludeSectionPart(CAttachPartsSection *pSection,CAttachPartsSection *pPartSection,double threshold)
{	
	if(pSection->m_fStartX<=pPartSection->m_fStartX&&pPartSection->m_fEndX<=pSection->m_fEndX)
		return TRUE;		//�Ѱ��������й���������
	if(pSection->m_fStartX-pPartSection->m_fEndX>threshold)
		return FALSE;		//δ����ƥ��Ĺ�������
	else if(pPartSection->m_fStartX-pSection->m_fEndX>threshold)
		return FALSE;		//δ����ƥ��Ĺ�������
	else
	{	//��ǰ�������Ѵ��ڵ�ĳ����������ص����� 
		//�����������С��������ֵ���Ժϲ�Ϊһ������ 
		pSection->m_fStartX=min(pSection->m_fStartX,pPartSection->m_fStartX);
		pSection->m_fEndX=max(pSection->m_fEndX,pPartSection->m_fEndX);
		return TRUE;
	}
}
double CPartGroupDrawing::AnalyzeLenModulate()
{
	int i;
	double threshold=sys.part_group_map.fPartGroupDistThreshold;// 100;
	f3dPoint pt_pos,jg_vec;
	sectionList.Empty();
	if(m_pRootPart==NULL||!m_pRootPart->IsLinePart())
		return 0;
	CLogErrorLife logErrorLife;
	ATOM_LIST<CAttachPartsSection> partSectionList;
	CSuperSmartPtr<CLDSPart>pPart;
	PROFILE_VERTEX *pVertex;
	f3dPoint vertice_up,vertice_down;
	for(pPart=SonParts.GetFirst();pPart.IsHasPtr();pPart=SonParts.GetNext())
	{
		if(pPart.IsEqualPtr(m_pRootPart))
			continue;	//��׼����
		double start_x=0,end_x=0;
		bool bNeedInit=true;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pPart.ParamPlatePointer()->vertex_list.GetNodeNum()==0)
				pPart.ParamPlatePointer()->DesignPlate();
		}
		if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			ATOM_LIST<PROFILE_VERTEX> *pVertexList=NULL;
			if(pPart->GetClassTypeId()==CLS_PLATE)
				pVertexList=&pPart.PlatePointer()->vertex_list;
			else
				pVertexList=&pPart.ParamPlatePointer()->vertex_list;
			for(pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext())
			{
				vertice_down=pVertex->vertex;
				if(pPart->GetClassTypeId()==CLS_PLATE)
					vertice_down.z=pPart.PlatePointer()->m_fNormOffset;
				else
					vertice_down.z=pPart.ParamPlatePointer()->GetNormOffset();
				coord_trans(vertice_down,pPart->ucs,TRUE);
				if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&(pPart.ParamPlatePointer()->m_iParamType==TYPE_FLD
					||pPart.ParamPlatePointer()->m_iParamType==TYPE_FLP))
				{
					double H=0;
					pPart.ParamPlatePointer()->GetStdPartParamValue('H',&H);
					vertice_up=vertice_down+pPart->ucs.axis_z*H;
				}
				else 
					vertice_up=vertice_down + pPart->ucs.axis_z*pPart->GetThick();
				coord_trans(vertice_up,cs,FALSE);
				coord_trans(vertice_down,cs,FALSE);
				if(bNeedInit)
				{
					start_x=end_x=vertice_up.x;
					bNeedInit=false;
				}
				else
				{
					start_x=min(start_x,vertice_up.x);
					end_x  =max(end_x,vertice_up.x);
				}
				start_x=min(start_x,vertice_down.x);
				end_x  =max(end_x,vertice_down.x);
			}
			double ddx=pPart->ucs.axis_z*cs.axis_x;
			if( pPart->GetClassTypeId()==CLS_PLATE&&pPart.PlatePointer()->m_fInnerRadius>0&&
				fabs(pPart->ucs.axis_z*cs.axis_z)<0.707&&fabs(ddx)<EPS_COS)
			{	//��ֱ�����������ܲ���ֱ�Ļ����,�����ͶԽ�����ͼ���ƣ��˴��赥�����㹹������ wht 18-08-22
				f3dPoint axis_y(0,1,0);
				double fAngle=cal_angle_of_2vec(cs.axis_x,pPart->ucs.axis_z);
				RotateVectorAroundVector(axis_y,fAngle,GEPOINT(0,0,1));
				f3dPoint plateOrg=pPart->ucs.origin+pPart->ucs.axis_z*pPart.GeneralPlatePointer()->GetNormOffset();
				if(ddx>0)
					plateOrg+=(pPart->ucs.axis_z*pPart->GetThick());
				coord_trans(plateOrg,cs,FALSE);
				double fHalfWidth=pPart->GetWidth()*0.5;
				vertice_up=plateOrg+axis_y*fHalfWidth;
				vertice_down=plateOrg-axis_y*fHalfWidth;
				start_x=min(start_x,vertice_up.x);
				end_x  =max(end_x,vertice_up.x);
				start_x=min(start_x,vertice_down.x);
				end_x  =max(end_x,vertice_down.x);
			}
		}
		else if(pPart->GetClassTypeId()==CLS_LINETUBE||pPart->GetClassTypeId()==CLS_LINESLOT||pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			f3dPoint line_vec,norm_vec,offset_vec;
			line_vec=pPart.LinePartPointer()->End()-pPart.LinePartPointer()->Start();
			if(fabs(line_vec*cs.axis_x)>EPS_COS)
				norm_vec=cs.axis_y;
			else
				norm_vec=line_vec^cs.axis_x;
			offset_vec=norm_vec^line_vec;
			normalize(line_vec);
			normalize(offset_vec);
			double offset_dist=0;
			if(pPart->GetClassTypeId()==CLS_LINETUBE)
				offset_dist=pPart.LineTubePointer()->GetDiameter()*0.5;
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				offset_dist=pPart.LineSlotPointer()->GetWidth()*0.5;
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				offset_dist=pPart.LineAnglePointer()->GetWidth()*0.5;
			vertice_down=pPart.LinePartPointer()->Start()-line_vec*pPart.LinePartPointer()->startOdd()-offset_vec*offset_dist;
			vertice_up=vertice_down+offset_vec*offset_dist;
			coord_trans(vertice_down,cs,FALSE);
			coord_trans(vertice_up,cs,FALSE);
			if(bNeedInit)
			{
				start_x=end_x=vertice_up.x;
				bNeedInit=false;
			}
			else
			{
				start_x=min(start_x,vertice_up.x);
				end_x  =max(end_x,vertice_up.x);
			}
			start_x=min(start_x,vertice_down.x);
			end_x  =max(end_x,vertice_down.x);

			vertice_down=pPart.LinePartPointer()->End()+line_vec*pPart.LinePartPointer()->startOdd()-offset_vec*offset_dist;
			vertice_up=vertice_down+offset_vec*offset_dist;
			coord_trans(vertice_down,cs,FALSE);
			coord_trans(vertice_up,cs,FALSE);
			if(bNeedInit)
			{
				start_x=end_x=vertice_up.x;
				bNeedInit=false;
			}
			else
			{
				start_x=min(start_x,vertice_up.x);
				end_x  =max(end_x,vertice_up.x);
			}
			start_x=min(start_x,vertice_down.x);
			end_x  =max(end_x,vertice_down.x);
		}
		CAttachPartsSection *pSectionPart=NULL,sectionPart;
		sectionPart.m_fStartX=start_x;
		sectionPart.m_fEndX=end_x;
		for(pSectionPart=partSectionList.GetFirst();pSectionPart;pSectionPart=partSectionList.GetNext())
		{
			if(pSectionPart->m_fStartX>start_x)
			{
				partSectionList.insert(sectionPart);
				break;
			}
		}
		if(pSectionPart==NULL)
			partSectionList.append(sectionPart);
	}
	
	//����ָ������ֵ��SectionParts���ֵ���ͬ�����
	sectionList.Empty();
	for(CAttachPartsSection *pTempSectionPart=partSectionList.GetFirst();pTempSectionPart;pTempSectionPart=partSectionList.GetNext())
	{
		CAttachPartsSection *pSectionSeg=sectionList.GetTail();
		if(pSectionSeg==NULL||!IsIncludeSectionPart(pSectionSeg,pTempSectionPart,threshold))
		{
			CAttachPartsSection *pNewSectionPart=sectionList.append();
			pNewSectionPart->m_fStartX=pTempSectionPart->m_fStartX;
			pNewSectionPart->m_fEndX=pTempSectionPart->m_fEndX;
		}
	}
	double length=((CLDSLinePart*)m_pRootPart)->GetLength();
	double majorsect_length=0;	//�ؼ�����(һ���а��������Ҫ���Ӽ�)
	for(i=0;i<sectionList.GetNodeNum();i++)
		majorsect_length+=(sectionList[i].m_fEndX-sectionList[i].m_fStartX);
	bool bFindProperDrawScale=false;
	do{
		double margin=10;
		double majorsect_drawlength=majorsect_length/m_fRealToDraw;
		//����ɹ��������ŵĻ�ͼ���򳤶�
		double remnant_zoomrgn_length=m_fMaxDrawingLength-2*margin-majorsect_drawlength;
		//���㰴��ǰ�����ؼ�����ͼ���ȵĳ�������
		double extra_length=0.1*m_fMaxDrawingLength-remnant_zoomrgn_length;	//0.1*m_fMaxDrawingLengthΪԤ�豣������С����������
		if(extra_length>0)//m_fMaxDrawingLength-2*margin-majorsect_drawlength<0.1*m_fMaxDrawingLength)
		{	//���¼���ؼ�����ͼ����
			m_fRealToDraw=majorsect_length/(majorsect_drawlength-extra_length);
			majorsect_drawlength-=extra_length;
			bFindProperDrawScale=true;
		}
		//TODO:���ܼ���xDrawSet.fPartGroupDistZoomCoef����Ӧ������һ������ wjh-2018.8.2
		xDrawSet.fPartGroupDistZoomCoef=(m_fMaxDrawingLength-2*margin-majorsect_drawlength)/(length-majorsect_length);
		if(xDrawSet.fPartGroupDistZoomCoef>0.15)//�趨�������(���ȷ���)ѹ���������Է�ֹһЩ�̸˼����ƹ��� wjh-2013.10.24
			xDrawSet.fPartGroupDistZoomCoef=0.15;
		if(xDrawSet.fPartGroupDistZoomCoef<0.015)
			xDrawSet.fPartGroupDistZoomCoef=0.015;//�趨��Сѹ���������Է�һЩ���˼����ƹ��̻�Ŷ����Ƽ�϶����
		//�����RelayoutDrawingʱ��ͳһ��sys.general_set.WeldMapScale.fRealToDrawϵ��
		xDrawSet.fPartGroupDistZoomCoef*=m_fRealToDraw;
		SmartPartPtr pRootRod=(CLDSLinePart*)m_pRootPart;
		if(bFindProperDrawScale || pRootRod->IsAngle())
			break;	//�ݲ�����Ǹ�
		if(m_bAutoLayoutViewIgnoreScale&&pRootRod->IsTube())//&&pRootRod->GetLength()>4000
		{
			int nFrontView=0,nSlaveView=0;
			for(CSectionView *pView=m_pPartGroup->SonViews.GetFirst();pView;pView=m_pPartGroup->SonViews.GetNext())
			{
				if(pView->m_iViewType==VIEW_MAIN||pView->m_iViewType==VIEW_JOINTTUBE||pView->m_iViewType==VIEW_FRONT)
					nFrontView++;
				else 
					nSlaveView++;
			}
			if(nFrontView>2||nSlaveView>3)	//ָһ����������м�����һ����������麸ͼ
				bFindProperDrawScale=true;
			else
			{	//���麸����1-2������ͼ&&3�����½���ͼ),���Ƹֹܸ߶Ȼ��Ʒ�Χ	wht 18-08-15
				//1������ͼ(��С��0.1  ���0.3)
				//2������ͼ(��С��0.1  ���0.2)
				double fRealDrawHeight=m_pRootPart->GetWidth()/m_fRealToDraw;
				double MIN_DRAW_HEIGHT=0.1*m_fMaxDrawingHeight;
				double MAX_DRAW_HEIGHT=0.3*m_fMaxDrawingHeight;
				if(nFrontView==2)
					MAX_DRAW_HEIGHT=0.2*m_fMaxDrawingHeight;
				if(fRealDrawHeight>MAX_DRAW_HEIGHT)
					m_fRealToDraw=m_pRootPart->GetWidth()/MAX_DRAW_HEIGHT;
				else if(fRealDrawHeight<MIN_DRAW_HEIGHT)
					m_fRealToDraw=m_pRootPart->GetWidth()/MIN_DRAW_HEIGHT;
				else
					bFindProperDrawScale=true;
			}
		}
	}while(m_bAutoLayoutViewIgnoreScale&&!bFindProperDrawScale);
	double zoomlen= 0;
	for(i=0;i<sectionList.GetNodeNum();i++)
	{
		if(i==0)
			zoomlen=LenCompressTrans(sectionList[0].m_fStartX,TRUE);
		else
			zoomlen+=LenCompressTrans(sectionList[i].m_fStartX-sectionList[i-1].m_fEndX,TRUE);
		zoomlen+=(sectionList[i].m_fEndX-sectionList[i].m_fStartX)/sys.general_set.WeldMapScale.fRealToDraw;
	}
	if(sectionList.GetNodeNum()>0)
		zoomlen+=LenCompressTrans(((CLDSLinePart*)m_pRootPart)->GetLength()-sectionList[sectionList.GetNodeNum()-1].m_fEndX);
	double dd=TransCoordX(7200);
	return zoomlen;
}

//����ʵ��Xֵ(�Ӹ˼���ʼ�˼���(��������ͷ)),����������Xֵ
double CPartGroupDrawing::TransCoordX(double real_x)
{
	int n=sectionList.GetNodeNum();
	double cur_pos_x=0,z = 0;
	//��׼�˼��Ǹ˼����ߵ����������Ϊ0
	if(n<=0||!m_pRootPart->IsLinePart())
		return real_x;
	//��ͼ����ϵX�᷽�����׼�˼����췽����ͬ���෴ʱ����Ҫ��������
	//��׼����Ϊ�˼���Ϊ��ͼ����ϵZ�᷽����˼����췽����ͬ���෴ʱ����Ҫ��������
	f3dPoint len_vec=((CLDSLinePart*)m_pRootPart)->End()-((CLDSLinePart*)m_pRootPart)->Start();
	normalize(len_vec);
	if(fabs(len_vec*cs.axis_z)>EPS_COS)
		return real_x;
	CAttachPartsSection *pSection=NULL,*pPrevSection=NULL;
	for(pSection=sectionList.GetFirst();pSection;pSection=sectionList.GetNext())
	{
		if(pPrevSection==NULL)
		{
			if(real_x<pSection->m_fStartX)
				return LenCompressTrans(real_x);
			else if(real_x<pSection->m_fEndX)
				return LenCompressTrans(pSection->m_fStartX)+(real_x-pSection->m_fStartX);
			else
			{
				cur_pos_x=LenCompressTrans(pSection->m_fStartX)+pSection->m_fEndX-pSection->m_fStartX;
				pPrevSection=pSection;
				continue;
			}
		}
		else if(real_x<pSection->m_fStartX)	//�ڵ�ǰ��������֮ǰ
			return cur_pos_x+LenCompressTrans(real_x-pPrevSection->m_fEndX);
		else if(real_x<pSection->m_fEndX)	//�ڵ�ǰ��������֮��
			return cur_pos_x+LenCompressTrans(pSection->m_fStartX-pPrevSection->m_fEndX)+(real_x-pSection->m_fStartX);
		else								//�ڵ�ǰ��������֮��
			cur_pos_x+=LenCompressTrans(pSection->m_fStartX-pPrevSection->m_fEndX)+(pSection->m_fEndX-pSection->m_fStartX);
		pPrevSection=pSection;
	}
	if(pSection==NULL&&pPrevSection)
		return cur_pos_x+LenCompressTrans(real_x-pPrevSection->m_fEndX);
	return cur_pos_x;
}

bool CPartGroupDrawing::InitPartGroupDrawing(CLDSPartGroup *pPartGroup,double maxDrawingLength,double maxDrawingHeight)
{
	m_pPartGroup=pPartGroup;
	if(m_pPartGroup==NULL)
		return false;
	m_pTower=pPartGroup->BelongModel();
	if(m_pTower==NULL)
		return false;
	m_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	if(m_pRootPart==NULL)
		return false;
	m_fMaxDrawingLength=maxDrawingLength;	//����ͼ����
	m_fMaxDrawingHeight=maxDrawingHeight;	//����ͼ�߶�
	if(m_pRootPart->IsLinePart())
	{
		CSectionView *pSectView,*pMainView=NULL;
		for(pSectView=pPartGroup->SonViews.GetFirst();pSectView;pSectView=pPartGroup->SonViews.GetNext())
		{
			if(pSectView->IsMainView())
			{
				pMainView=pSectView;
				break;
			}
		}
		CLDSLinePart *pRootLinePart=(CLDSLinePart*)m_pRootPart;
		cs.axis_x=pRootLinePart->End()-pRootLinePart->Start();
		normalize(cs.axis_x);
		cs.origin=pRootLinePart->Start()-cs.axis_x*pRootLinePart->startOdd();
		if(pMainView)
		{	//������ͼX��������˼�ʼ->�շ���һ��ʱ�������麸ͼ����ϵ
			UCS_STRU cs2=pMainView->UpdateUCS(m_pRootPart->BelongModel(),pPartGroup);
			if(cs.axis_x*cs2.axis_x<0)
			{
				cs.origin=pRootLinePart->xPosEnd+cs.axis_x*pRootLinePart->endOdd();
				cs.axis_x*=-1.0;
			}
		}
		cs.axis_y = inters_vec(cs.axis_x);
		cs.axis_z = cs.axis_x^cs.axis_y;
		cs.axis_y = cs.axis_z^cs.axis_x;
		normalize(cs.axis_y);
		normalize(cs.axis_z);
		m_pTower->GetPartGroupPartSet(pPartGroup->handle,SonParts);
		//ԭ��ȡֵ4000,�ܶ���麸�����������ű������޷���֤��С�������ƿ�� wht 18-08-15
		const double TUBE_LEN_THRESHOLD=2000;	//4000
		if(xDrawSet.bModulatePartGroupLen&&pRootLinePart->GetLength()>TUBE_LEN_THRESHOLD)
			AnalyzeLenModulate();
	}
	else
		cs = m_pRootPart->ucs;
	return true;
}

#include "lds_part.h"
void CPartGroupDrawing::IntelliCalDrawScale(CHashList<double>* pHashScaleOfView)	//���ܼ�������ͼ���˼�����ͼ�Ļ�ͼ����
{
	//TODO:���ܼ��������ͼ��˼�����ͼ��ǡ����ͼ����
	//m_fSectViewScaleRealToDraw=sys.part_group_map.fwe;
	//m_fMajorRodViewScaleRealToDraw;
}
CPartGroupDrawing::RESETSYSPARA_LIFEOBJ::RESETSYSPARA_LIFEOBJ(int mode/*=0*/)
{
	bDrawWeldLineSymbol=sys.part_group_map.bDrawWeldLineSymbol!=0;
	bZoomFontSizeWithDrawScale=sys.welding.bZoomAdaptiveWithScale;
	if(sys.general_set.bProcessCardTbl)
	{
		sys.part_group_map.bDrawWeldLineSymbol=false;
		sys.welding.bZoomAdaptiveWithScale=false;
	}
}
CPartGroupDrawing::RESETSYSPARA_LIFEOBJ::~RESETSYSPARA_LIFEOBJ()
{
	sys.part_group_map.bDrawWeldLineSymbol=bDrawWeldLineSymbol;
	sys.welding.bZoomAdaptiveWithScale=bZoomFontSizeWithDrawScale;
}
bool CPartGroupDrawing::CreatePartGroupDrawing()
{
	if(m_pPartGroup==NULL||m_pTower==NULL)
		return false;
	BOOL bAutoMatchView=FALSE;
	if(drawStyleFlag.IsHasFlag(CPartGroupDrawing::LayoutViews))
		bAutoMatchView=TRUE;//�Զ�ƥ����ͼ
	if(!bAutoMatchView)
		return false;
	CHashList<double> hashScaleOfViews;
	IntelliCalDrawScale(&hashScaleOfViews);	//���ܼ�������ͼ���˼�����ͼ�Ļ�ͼ����
	RESETSYSPARA_LIFEOBJ resetsyspara;
	CSectionView *pSectionView=NULL;
	//��ʼ������ͼ����ͼ����
	/*int i=0;
	for(pSectionView=m_pPartGroup->SonViews.GetFirst();pSectionView;pSectionView=m_pPartGroup->SonViews.GetNext())
	{
		if(m_pPartGroup->m_iPartGroupType!=CLDSPartGroup::GROUP_SPHERE)
		{
			if(pSectionView->m_iViewType==VIEW_MAIN || pSectionView->m_iViewType==VIEW_FRONT)
				continue;
			pSectionView->symbol.Printf("%c",char('A'+i));
			i++;
		}
		else	//������ڵ��麸���������Ƴ�ʼ����ͼ���
			pSectionView->symbol.Printf("%s",pSectionView->sName);
	}*/
	EmptyDrawingComponents();
	//��ǰͳ�Ƹ���ͼ�л��ƹ�������Ϣ
	for(pSectionView=m_pPartGroup->SonViews.GetFirst();pSectionView;pSectionView=m_pPartGroup->SonViews.GetNext())
		StatSectionViewDimDraws(pSectionView);
	for(pSectionView=m_pPartGroup->SonViews.GetFirst();pSectionView;pSectionView=m_pPartGroup->SonViews.GetNext())
	{
		double* pfScale=hashScaleOfViews.GetValue(pSectionView->Id);
		if(pfScale==NULL)
			pfScale=&this->m_fRealToDraw;
		CreateSectionViewDrawing(pSectionView,*pfScale);
	}
	for(CSectionViewDrawing* pViewDrawing=(CSectionViewDrawing*)EnumFirstDrawingComponent();pViewDrawing;pViewDrawing=(CSectionViewDrawing*)EnumNextDrawingComponent())
		pViewDrawing->FinalDrawing();
	/*
	if(m_pRootPart&&m_pRootPart->IsTube())
	{
		CLDSLineTube *pDatumTube=(CLDSLineTube*)m_pRootPart;
		CLDSParamPlate *pStartPlate=(CLDSParamPlate*)m_pRootPart->BelongModel()->FromPartHandle(pDatumTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		CLDSParamPlate *pEndPlate=(CLDSParamPlate*)m_pRootPart->BelongModel()->FromPartHandle(pDatumTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pStartPlate&&pStartPlate->m_iParamType==TYPE_ROLLEND)
			CreateTubeEndPlateViewDrawing(pStartPlate,m_fRealToDraw);
		if(pEndPlate&&pEndPlate->m_iParamType==TYPE_ROLLEND)
		{
			if(pStartPlate==NULL||stricmp(pEndPlate->Label,pStartPlate->Label)!=0)
				CreateTubeEndPlateViewDrawing(pEndPlate,m_fRealToDraw);
		}
	}*/
	//��ʼ�������
	CPartGroupTblDrawing* pTblDrawing=NULL;
	if(sys.part_group_map.bCreatePartGroupBOM)
	{	//�麸����ϸ��
		pTblDrawing=(CPartGroupTblDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_BOM_VIEW);
		pTblDrawing->m_pPartGroup=m_pPartGroup;
		pTblDrawing->m_nTableType=CPartGroupTblDrawing::PARTGROUP_BOM;
	}
	if(m_pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_SPHERE)
	{	//��ڵ��麸��װ����Ϣ��
		pTblDrawing=(CPartGroupTblDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_BOM_VIEW);
		pTblDrawing->m_pPartGroup=m_pPartGroup;
		pTblDrawing->m_nTableType=CPartGroupTblDrawing::SPHEREINTERS_TBL;
	}
	return true;
}
CSectionTubeEndPlateViewDrawing* CPartGroupDrawing::CreateTubeEndPlateViewDrawing(CLDSParamPlate *pPlate,double scaleOfReal2Draw/*=0*/)
{
	CSectionTubeEndPlateViewDrawing *pViewDrawing=(CSectionTubeEndPlateViewDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_TUBE_ENDPLATE_VIEW);
	pViewDrawing->m_fUser2ScrDrawScale=m_fRealToDraw;
	pViewDrawing->drawStyleFlag=drawStyleFlag;	//��ʼ���麸ͼ���Ʒ��
	pViewDrawing->m_pParamPlate=pPlate;
	pViewDrawing->CreateSectionViewDrawing(NULL,m_pTower,m_pPartGroup,false);
	pViewDrawing->ZoomDrawing(0.1);
	return pViewDrawing;
}
//��ʼ���ֹܶ�ͷ������ͼ
BYTE CPartGroupDrawing::InitEndInsertPlateDrawing(CSectionTubeEndPlateViewDrawing *pStartDrawing,CSectionTubeEndPlateViewDrawing *pEndDrawing)
{
	BYTE cFlag=0;
	CLDSParamPlate *pStartPlate=NULL,*pEndPlate=NULL;
	if(m_pRootPart&&m_pRootPart->IsTube())
	{
		CLDSLineTube *pDatumTube=(CLDSLineTube*)m_pRootPart;
		pStartPlate=(CLDSParamPlate*)m_pRootPart->BelongModel()->FromPartHandle(pDatumTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		pEndPlate=(CLDSParamPlate*)m_pRootPart->BelongModel()->FromPartHandle(pDatumTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pStartPlate&&pStartPlate->m_iParamType==TYPE_ROLLEND)
			cFlag|=0x01;
		if(pEndPlate&&pEndPlate->m_iParamType==TYPE_ROLLEND)
		{
			if(pStartPlate==NULL||stricmp(pEndPlate->Label,pStartPlate->Label)!=0)
				cFlag|=0x02;
		}
	}
	if(cFlag&0x01)
	{
		pStartDrawing->m_fUser2ScrDrawScale=m_fRealToDraw;
		pStartDrawing->drawStyleFlag=drawStyleFlag;	//��ʼ���麸ͼ���Ʒ��
		pStartDrawing->m_pParamPlate=pStartPlate;
		pStartDrawing->CreateSectionViewDrawing(NULL,m_pTower,m_pPartGroup,false);
		pStartDrawing->ZoomDrawing(0.1);
	}
	if(cFlag&0x02)
	{
		pEndDrawing->m_fUser2ScrDrawScale=m_fRealToDraw;
		pEndDrawing->drawStyleFlag=drawStyleFlag;	//��ʼ���麸ͼ���Ʒ��
		pEndDrawing->m_pParamPlate=pEndPlate;
		pEndDrawing->CreateSectionViewDrawing(NULL,m_pTower,m_pPartGroup,false);
		pEndDrawing->ZoomDrawing(0.1);
	}
	return cFlag;
}
PART_DIMnDRAW::PART_DIMnDRAW()
{
	ciAttachPlateType=0;
	bDrawSketch=bDimPartNo=true;
	bMandatoryDimLabel=false;
	ciDrawModeH0V1=0;
	cbDrawPlanes=0xff;
	pPart=NULL;
	ciLeadLineMode=1;
}
PART_DIMnDRAW* CPartGroupDrawing::RegisterPartDimDraw(long idView,long hPart)
{
	CLDSPart* pPart=Ta.Parts.FromHandle(hPart);
	if(pPart==NULL)
		return NULL;
	PART_DIMnDRAW* pDimDraw=hashDimDrawParts.Add(idView,hPart);
	pDimDraw->idView=idView;
	pDimDraw->hPart=hPart;
	pDimDraw->pPart=pPart;
	if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate* pPlate=((CLDSParamPlate*)pPart);
		for(ATTACHPART_ITEM* pItem=pPlate->AttachPart.GetFirst();pItem;pItem=pPlate->AttachPart.GetNext())
		{
			PART_DIMnDRAW* pSonDimDraw=hashDimDrawParts.Add(idView,pItem->h);
			pSonDimDraw->ciAttachPlateType=(BYTE)pPlate->m_iParamType;
		}
	}
	return pDimDraw;
}
bool CPartGroupDrawing::StatSectionViewDimDraws(CSectionView* pView)
{
	for(CSectionView::PART_ITEM* pItem=pView->SonParts.GetFirst();pItem;pItem=pView->SonParts.GetNext())
		RegisterPartDimDraw(pView->GetId(),pItem->hSonPart);
	return true;
}
CSectionViewDrawing* CPartGroupDrawing::CreateSectionViewDrawing(CSectionView *pSectionView,double scaleOfReal2Draw/*=0*/)
{
	if(pSectionView==NULL||m_pTower==NULL||m_pPartGroup==NULL||m_pRootPart==NULL)
		return NULL;
	CSectionViewDrawing *pViewDrawing=NULL;
	if(pSectionView->m_iViewType==VIEW_FL)
		pViewDrawing=(CSectionFlDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_FL_VIEW);
	else if(pSectionView->m_iViewType==VIEW_JOINTTUBE)
		pViewDrawing=(CSectionJointDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_JOINT_VIEW);
	//����CSectionView::m_idViewType�������ɵ�ͼֽ
	else if(pSectionView->GetViewTypeId()==CSectionView::CLS_SLAVE_VIEW)
		pViewDrawing=(CSectionSlaveDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_SLAVE_VIEW);
	else if(pSectionView->GetViewTypeId()==CSectionView::CLS_STDX_PLATES_VIEW)
		pViewDrawing=(CSectionStdXPlatesDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_STDX_PLATES_VIEW);
	else if(pSectionView->GetViewTypeId()==CSectionView::CLS_ANGLE_FOOT_VIEW)
		pViewDrawing=(CSectionAngleFootDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_ANGLE_FOOT_VIEW);
	else if(pSectionView->GetViewTypeId()==CSectionView::CLS_KAOME_ANGLE_VIEW)
		pViewDrawing=(CSectionKmAngleViewDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_KM_ANGLE_VIEW);
	else if(pSectionView->GetViewTypeId()==CSectionView::CLS_ANGLE_SPREAD_VIEW)
		pViewDrawing=(CSectionAngleViewDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_ANGLE_SPREAD_VIEW);
	else if(pSectionView->IsBriefTubeView(m_pTower))
		pViewDrawing=(CSectionBriefTubeDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_BRIEFTUBE_VIEW);
	else
		pViewDrawing=(CSectionViewDrawing*)AppendDrawingComponent(CSectionViewDrawing::CLS_BASIC_VIEW);
	if(scaleOfReal2Draw>0)
		pViewDrawing->m_fUser2ScrDrawScale=scaleOfReal2Draw;
	else
		pViewDrawing->m_fUser2ScrDrawScale=m_fRealToDraw;
	pViewDrawing->drawStyleFlag=drawStyleFlag;	//��ʼ���麸ͼ���Ʒ��
	if(m_pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_SPHERE)	//��ڵ��麸������ͼ��ͼ
		pViewDrawing->CreateSphereSectionViewDrawing(pSectionView,m_pTower,m_pPartGroup);
	else
		pViewDrawing->CreateSectionViewDrawing(pSectionView,m_pTower,m_pPartGroup,false);
	return pViewDrawing;
}

f2dRect CPartGroupDrawing::GetDrawRegion(double PART_MAP_SCALE)
{
	f2dRect rect;
	return rect;
}
int CPartGroupDrawing::Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr)
{	
	CPartGroupDrawingLayoutOper layoutOper(this);
	return layoutOper.Relayout(zoomdraw_coef_arr);
}
//////////////////////////////////////////////////////////////////////////
//CDimAngleManager
CDimAngularManager::CDimAngularManager(double fDimTextSize/*=2*/)
{
	m_fDimAngleTextSize=fDimTextSize;
}
CDimAngularManager::~CDimAngularManager()
{
	m_xAngularDimList.DeleteList();
}
static int _LocalCompareRadialAngleFunc(const RADIAL_ITEM& radialItem1,const RADIAL_ITEM& radialItem2)
{
	return radialItem1.radAngle>radialItem2.radAngle?1:-1;
}
GEPOINT ProjectByColumnAxis(const double* direction,const double* vColAxisVec,const double* vPrjPlaneNormal)
{
	GEPOINT inters,xOrgCenter,vColLenVec(vColAxisVec);
	if(vColLenVec.IsZero())
		return GEPOINT(direction);
	normalize(vColLenVec);
	Int3dlf(inters,GEPOINT(direction),vColLenVec,xOrgCenter,GEPOINT(vPrjPlaneNormal));
	GEPOINT vPrjVec=inters-xOrgCenter;
	normalize(vPrjVec);
	return vPrjVec;
}
void CDimAngularManager::CreateAngularItems(const double* normal,const double* cStartEdgeVec,const double* vTubeLenVec,RADIAL_ITEM* items,int count)
{
	int i;
	//�趨��ע����ϵ����ʼ���Ƕȱ�ע��
	GECS cs;
	m_vStartEdgeAxisVec=ProjectByColumnAxis(cStartEdgeVec,vTubeLenVec,normal);
	m_vNorm=GEPOINT(normal);
	normalize(m_vNorm);
	cs.axis_z=m_vNorm;
	cs.axis_x=m_vStartEdgeAxisVec;
	cs.axis_x-=(cs.axis_x*m_vNorm)*m_vNorm;
	normalize(cs.axis_x);
	cs.axis_y=cs.axis_z^cs.axis_x;
	for(i=0;i<count;i++)
	{
		items[i].rayVec=ProjectByColumnAxis(items[i].rayVec,vTubeLenVec,normal);
		items[i].rayVec=cs.TransVToCS(items[i].rayVec);
		normalize(items[i].rayVec);
		items[i].radAngle=Cal2dLineAng(0,0,items[i].rayVec.x,items[i].rayVec.y);
		items[i].rayVec=cs.TransVFromCS(items[i].rayVec);
	}
	//�жϵ�һ���Ƿ�����ʼ��,���û�н���ʼ����Ϣ���뵽��һ��
	CQuickSort<RADIAL_ITEM>::QuickSort(items,count,&_LocalCompareRadialAngleFunc);
	ARRAY_LIST<RADIAL_ITEM> itemArr;
	for(i=0;i<count;i++)
	{
		if(i==0)
		{
			RADIAL_ITEM start_item;
			start_item.rayVec=m_vStartEdgeAxisVec;
			start_item.rayOrg=items[0].rayOrg;
			start_item.radAngle=0;//items[i].radAngle;
			itemArr.append(start_item);
		}
		if(items[i].radAngle<0.01744||(Pi+Pi-items[i].radAngle)<0.01744)	//0.01744��1��
			continue;
		else if(items[i].radAngle<(Pi+Pi-0.01744))		//����ʼ���غϲ�������
			itemArr.append(items[i]);
	}
	//�������Ļ���нǣ��Ա��������Ƕȱ�עʱ�޳�
	double maxAngle=0,fMaxDimR=0;
	int maxAngleIndex=-1,nSize=itemArr.GetSize();;
	for(i=0;i<nSize;i++)
	{
		RADIAL_ITEM* pRadialItem=&itemArr[i];
		RADIAL_ITEM* pNextRadialItem = &itemArr[(i+1)%nSize];
		double fRadAngle=pNextRadialItem->radAngle-pRadialItem->radAngle;
		if(fabs(fRadAngle)<0.01744)	//1��=0.01744 |0.1��=0.00177
			continue;
		if(fRadAngle<0)
			fRadAngle+=2*3.14;
		if(maxAngle<fRadAngle)
		{
			maxAngle=fRadAngle;
			maxAngleIndex=i;
		}
		//���������С�����ע�뾶
		double fDimR=0;
		double half_angle=0.5*ACOS(pRadialItem->rayVec*pNextRadialItem->rayVec);
		half_angle=max(half_angle,0.26);	//0.26��15��
		if(half_angle<1.57)
			fDimR = min((m_fDimAngleTextSize*0.7*6)/tan(half_angle),100);
		if(fMaxDimR<fDimR)
			fMaxDimR=fDimR;
	}
	for(i=0;i<nSize;i++)
	{
		RADIAL_ITEM* pRadialItem=&itemArr[i];
		RADIAL_ITEM* pNextRadialItem = &itemArr[(i+1)%nSize];
		if(fabs(pNextRadialItem->radAngle-pRadialItem->radAngle)<0.01744)	//1��=0.01744 | 0.1��=0.00177
			continue;
		if(i==maxAngleIndex)
			continue;	//�޳�������νǱ�ע
		ANGULARDIM_ITEM* pAngularItem=m_xAngularDimList.AttachObject();
		pAngularItem->hStartObj=pRadialItem->hObj;
		pAngularItem->vStartVec=pRadialItem->rayVec;
		pAngularItem->hEndObj=pNextRadialItem->hObj;
		pAngularItem->vEndVec=pNextRadialItem->rayVec;
		pAngularItem->vTipPoint=pRadialItem->rayOrg;
		pAngularItem->fDimRadius=fMaxDimR;
		pAngularItem->degSectAngle=(pNextRadialItem->radAngle-pRadialItem->radAngle)*DEGTORAD_COEF;
	}
}
//////////////////////////////////////////////////////////////////
//CSectionViewDrawing
//////////////////////////////////////////////////////////////////

CSectionViewDrawing::CSectionViewDrawing(int id/*=0*/,IDrawing* pParentDrawing/*=NULL*/)
	: CXeroDrawingComponent(id,pParentDrawing)
{
	m_iComponentType=CLS_BASIC_VIEW;
	m_pTower=NULL;
	m_pPartGroup=NULL;
	_pRootPart=NULL;
	m_pSectionView=NULL;
	m_bSamePartNoDimOnce=TRUE;
	m_bFinalDrawing=false;
	m_fUser2ScrDrawScale=sys.general_set.WeldMapScale.fRealToDraw;	//1:20;
	listCmds.CreateNewAtom=CreateStruDrawingCommand;
	listCmds.DeleteAtom=DeleteStruDrawingCommand;
	m_xEntManager.InitContext(m_pRootPart,this,STORE_ACAD);
	m_pArrowLine1=m_pArrowLine2=NULL;
	m_pArrowPolyLine1=m_pArrowPolyLine2=NULL;
}

CSectionViewDrawing::~CSectionViewDrawing()
{
}
UCS_STRU CSectionViewDrawing::UpdateDCS()	//��������ͼ���ɸ�����ͼ�Ļ�ͼ����ϵ
{
	cs=m_pSectionView->UpdateUCS(m_pTower,m_pPartGroup);
	//TODO:m_pPartGroupDrawing->cs��ʱ��cs��һ��
	if(m_pSectionView->IsDualTubeJointView())//m_idViewType==CSectionView::CLS_HORIZ_BEAM_VIEW)
		return cs;
	else if(m_pPartGroupDrawing&&(m_pSectionView->m_iViewType==VIEW_MAIN||m_pSectionView->m_iViewType==VIEW_FRONT))
		cs.origin=m_pPartGroupDrawing->GetCS().origin;
	return cs;
}
//�ɻ�ͼ����ϵ��չ�������¼���õ��µ�����ϵ
BOOL CSectionViewDrawing::GetDCS(long hPart,CDrawCoordTrans &dcs)
{
	CSectionView::PART_ITEM* pItem=NULL;
	dcs.SetCS(GetCS());
	pItem=m_pSectionView->SonParts.GetValue(hPart);
	if(pItem==NULL)
		return FALSE;
	SmartPartPtr pCurPart=m_pSectionView->LoadTagInfo(pItem)->pCurPart;
	if(m_pSectionView->LoadTagInfo(pItem)->pCurPart==NULL)
		pCurPart=m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(hPart);
	if(m_pSectionView->m_idViewType==CSectionView::CLS_JOINT_TUBES_VIEW)
	{
		CJointTubesView* pJointView=(CJointTubesView*)m_pSectionView;
		CSectionJointDrawing* pJointDrawing=(CSectionJointDrawing*)this;
		if(pJointView->m_ciJointsViewType>0)
		{
			GEPOINT lenRootVec,lenJointVec,xJointPosition;
			//if(hPart==pJointView->m_hJointTube)
			{
				lenRootVec =pJointDrawing->pRootTube->End() - pJointDrawing->pRootTube->Start();
				if(pJointDrawing->pRootTube->pEnd==pJointDrawing->pJointTube->pStart||pJointDrawing->pRootTube->pEnd==pJointDrawing->pJointTube->pEnd)
					lenRootVec*=-1;	//�ն˶Խ�
				normalize(lenRootVec);
				lenJointVec=pJointDrawing->pJointTube->End()-pJointDrawing->pJointTube->Start();
				xJointPosition=pJointDrawing->pJointTube->Start();
				if(pJointDrawing->pJointTube->pEnd==pJointDrawing->pRootTube->pStart||pJointDrawing->pJointTube->pEnd==pJointDrawing->pRootTube->pEnd)
				{
					xJointPosition=pJointDrawing->pJointTube->End();
					lenJointVec*=-1;	//�ն˶Խ�
				}
				GECS rotcs;
				rotcs.axis_z=lenRootVec;
				if(pJointView->m_ciJointsViewType==1)
				{	//ƽ��ͼ
					rotcs.axis_x=cs.axis_z;
					rotcs.axis_y=rotcs.axis_z^rotcs.axis_x;
				}
				else if(pJointView->m_ciJointsViewType==2)
				{	//������������ͼ
					rotcs.axis_y=cs.axis_z;
					rotcs.axis_x=rotcs.axis_y^rotcs.axis_z;
				}
				//TODO:�淶����ת����ϵ
				GEPOINT lenPrjJointVec=rotcs.TransVToCS(lenJointVec);
				lenPrjJointVec.z=0;
				normalize(lenPrjJointVec);
				double rotRadAngle=ACOS(lenPrjJointVec.x);
				if(lenPrjJointVec.x<0)
					rotRadAngle=Pi-rotRadAngle;
				if(-lenPrjJointVec.y*lenPrjJointVec.x<0)
					rotRadAngle*=-1;	//��lenRotVec�������תչ����
				dcs.m_xSpreadLine.startPt=pJointDrawing->pRootTube->Start();
				dcs.m_xSpreadLine.endPt +=dcs.m_xSpreadLine.startPt+lenRootVec*100;
				dcs.m_fSpreadAngle=rotRadAngle;
				dcs.m_xSpreadPlaneNorm=cs.axis_z;
				RotateVectorAroundVector(dcs.m_xSpreadPlaneNorm,-rotRadAngle,lenRootVec);
			}
			if(pCurPart.IsHasPtr()&&pCurPart->m_hPartWeldParent==pJointView->m_hJointTube)
			{
				if(pCurPart->GetClassTypeId()==CLS_PLATE)
				{
					normalize(lenJointVec);
					GEPOINT vPlateNormal=pCurPart.pPlate->ucs.axis_z;
					double justify,justify2;
					justify=vPlateNormal*lenJointVec;
					if(pCurPart.pPlate->designInfo.iFaceType==6&&pCurPart.pPlate->face_N>1)
					{	//��繹���۵���
						for(int i=1;i<pCurPart.pPlate->face_N;i++)
						{
							GEPOINT vBendPlaneNormal=pCurPart.pPlate->GetHuoquFaceNorm(i-1);
							justify2=vBendPlaneNormal*lenJointVec;
							if(fabs(justify2)<fabs(justify))
							{
								justify=justify2;
								vPlateNormal=vBendPlaneNormal;
							}
						}
					}
					RotateVectorAroundVector(vPlateNormal,dcs.m_fSpreadAngle,lenRootVec);
					rotate_point_around_axis(xJointPosition,dcs.m_fSpreadAngle,dcs.m_xSpreadLine.startPt,dcs.m_xSpreadLine.endPt);
					RotateVectorAroundVector(lenJointVec,dcs.m_fSpreadAngle,lenRootVec);
					GEPOINT prjAxisZ=dcs.cs.axis_z-(dcs.cs.axis_z*lenJointVec)*lenJointVec;
					normalize(prjAxisZ);
					justify=vPlateNormal*prjAxisZ;
					if(fabs(justify)>EPS_COS)	//�ְ��ڵ�ǰ����ƽ����
						dcs.second.fRotateAngle=0;
					else if(fabs(justify)<0.707)//��ֱ���ƣ�����Ҫ���Ƶĸְ壬��˲���Ҫչ��
						dcs.second.fRotateAngle=0;
					else	//��Ҫչ�����Ƶĸְ�
					{
						dcs.second.fRotateAngle=ACOS(fabs(justify));	//չ���Ǳ�������ǣ������ת����һ���ˣ���ֵ��Ҫͨ��չ���᷽��������
						dcs.second.vNormal=vPlateNormal;
						f3dPoint rot_axis_vec=dcs.second.vNormal^prjAxisZ;
						if(justify>0)	//
							rot_axis_vec/= rot_axis_vec.mod();	//��λ��
						else
							rot_axis_vec/=-rot_axis_vec.mod();
						dcs.second.xRotAxisStart=xJointPosition;
						dcs.second.xRotAxisEnd=dcs.second.xRotAxisStart+rot_axis_vec;
					}
				}
				//else if(pCurPart->GetClassTypeId()==CLS_PARAMPLATE) ��δʵ�ֲ�������
			}
			else if(hPart!=pJointView->m_hJointTube)
				dcs.m_fSpreadAngle=0;
		}
	}
	else if(m_pSectionView->IsDualTubeJointView())//m_idViewType==CSectionView::CLS_DUALTUBE_JOINT_VIEW||m_pSectionView->m_idViewType==CSectionView::CLS_HORIZ_BEAM_VIEW)
	{
		dcs.m_fSpreadAngle=0;
		//����չ������Ϣ
		CDrawingAssistant* pAssist=m_pSectionView->hashAssistPlanes.GetValue(pItem->iResidePlaneNo);
		if(pAssist!=NULL)
		{
			if(pAssist->m_nStyle==0&&pAssist->iResidePlaneNo==0)
			{
				pAssist->GetRotateAxisAndAngle(dcs.cs.axis_z,dcs.m_xSpreadLine,dcs.m_fSpreadAngle);
				dcs.m_xSpreadPlaneNorm=pAssist->m_xFaceNorm.vector;
			}
			else if(pAssist->m_nStyle==0)
			{
				GECS rotcs;
				m_pSectionView->GetSpreadPlaneRotCS(pAssist->iNo,&rotcs,&Ta);
				//�ϲ���ͼ����ϵ��dcs����תչ������ϵrotcs;
				coord_trans(rotcs.origin,dcs.cs,FALSE,TRUE);
				vector_trans(rotcs.axis_x,dcs.cs,FALSE,TRUE);
				vector_trans(rotcs.axis_y,dcs.cs,FALSE,TRUE);
				vector_trans(rotcs.axis_z,dcs.cs,FALSE,TRUE);
				ReverseUCS(&rotcs);
				dcs.SetCS(UCS_STRU(rotcs));
			}
			else if(pAssist->m_nStyle==3)
			{
				dcs.m_fSpreadAngle=pAssist->m_fRotRadAng;
				pAssist->m_xLineStart.UpdatePos(&Ta);
				pAssist->m_xFaceNorm.UpdateVector(&Ta);
				dcs.m_xSpreadLine.startPt=pAssist->m_xLineStart.Position();
				dcs.m_xSpreadLine.endPt=dcs.m_xSpreadLine.startPt+pAssist->m_xFaceNorm.vector*1000;//pAssist->m_xLineEnd.Position();
				dcs.m_xSpreadPlaneNorm=dcs.cs.axis_z;
				double sina=sin(dcs.m_fSpreadAngle);
				double cosa=SQRT(1-sina*sina);
				RotateVectorAroundVector(dcs.m_xSpreadPlaneNorm,-sina,cosa,pAssist->m_xFaceNorm.vector);
			}
		}
	}
	else
	{
		dcs.m_fSpreadAngle=0;
		//����չ������Ϣ
		CDrawingAssistant* pAssist=m_pSectionView->hashAssistPlanes.GetValue(pItem->iResidePlaneNo);
		if(pAssist)
		{
			pAssist->GetRotateAxisAndAngle(dcs.cs.axis_z,dcs.m_xSpreadLine,dcs.m_fSpreadAngle);
			dcs.m_xSpreadPlaneNorm=pAssist->m_xFaceNorm.vector;
		}
	}
	return TRUE;
}
CPlateDrawing::CPlateDrawing()
{
	style.penStyle=PS_SOLID;;	//��ӦԭbCoplanar=TRUE,FALSEʱ������PS_DASH����
	style.bDrawWeldLine=true;	//=TRUE���ƺ����߱��
	style.bDimBoltSpace=false;
	style.bDimRayRodAngle=false;
	style.bDrawInnerCircle=true;
	style.bMandatoryDimLabel=false;
	style.bSpreadBendPlane=false;
	style.ciDrawModeH0V1=0;		//Ĭ��ˮƽ���ƣ�ʵ�ʻ��ƹ������ٸ���ʵ���������״̬
	style.cbDrawPlanes=0xff;
	style.geomcenter0_left1_right2=0;	//��Ϣ��ǵ�λ��=0
	style.siMinFilterZ=style.siMaxFilterZ=0;
}

//�ж����߶��Ƿ��ཻ��������
BOOL IsIntersectOrTooClose(f3dLine base_line,f3dLine vice_line)
{
	const int DIST_EPS=1;
	f3dPoint perp;
	double fDist1=0,fDist2=0;
	if( SnapPerp(&perp,base_line,vice_line.startPt,&fDist1)&&
		SnapPerp(&perp,base_line,vice_line.endPt,&fDist2)&&
		(fDist1<DIST_EPS||fDist2<DIST_EPS))
		return TRUE;
	else if(Int3dll(base_line,vice_line,perp)>0)
		return TRUE;
	else
		return FALSE;
}
static void UpdateVerticalPlateDimParam(CSectionDrawingEntsManager::VERTICAL_PLATE *pVertPlate,f3dLine line)
{
	if(!pVertPlate->bDimStart&&!pVertPlate->bDimEnd)
		return;
	int nFace=pVertPlate->pPlate->GetFaceN();
	f3dLine startLine(pVertPlate->face_line[0][0].startPt,pVertPlate->face_line[0][1].startPt);
	f3dLine endLine(pVertPlate->face_line[nFace-1][0].endPt,pVertPlate->face_line[nFace-1][1].endPt);
	if(pVertPlate->bDimStart&&IsIntersectOrTooClose(line,startLine))
		pVertPlate->bDimStart=FALSE;
	if(pVertPlate->bDimEnd&&IsIntersectOrTooClose(line,endLine))
		pVertPlate->bDimEnd=FALSE;
}
//��ע���嶨λ�ߴ�
void CPlateDrawing::DimVerticalPlateLocateSize(CLDSPlate *pPlate,IDbEntsContext* pContext)
{
	if(pPlate==NULL||!pContext->IsSectionDrawing())
		return;
	CSizeTextDim space_dim;
	CSectionDrawingEntsManager *pEntManager=(CSectionDrawingEntsManager*)pContext;
	//1.�ж�����ʼ\�ն��Ƿ��������������������������ʱ����Ҫ��ע��ǰ�˶�λ�ߴ�
	CSectionDrawingEntsManager::VERTICAL_PLATE *pVertPlate=NULL,*pOtherPlate=NULL;
	for(pVertPlate=pEntManager->m_hashVertPlateList.GetFirst();pVertPlate;pVertPlate=pEntManager->m_hashVertPlateList.GetNext())
	{
		int nPush=pEntManager->m_hashVertPlateList.push_stack();
		for(pOtherPlate=pEntManager->m_hashVertPlateList.GetFirst();pOtherPlate;pOtherPlate=pEntManager->m_hashVertPlateList.GetNext())
		{
			if(pVertPlate==pOtherPlate)
				continue;
			for(int i=0;i<pOtherPlate->pPlate->GetFaceN();i++)
			{
				UpdateVerticalPlateDimParam(pVertPlate,pOtherPlate->face_line[i][0]);
				UpdateVerticalPlateDimParam(pVertPlate,pOtherPlate->face_line[i][1]);
			}
		}
		pEntManager->m_hashVertPlateList.pop_stack(nPush);
	}
	//2.��ע���嶨λ�ߴ�
	for(pVertPlate=pEntManager->m_hashVertPlateList.GetFirst();pVertPlate;pVertPlate=pEntManager->m_hashVertPlateList.GetNext())
	{
		//2.1 ��ע�����׼����ƽ�б�֮��ľ���
		if(pVertPlate->fParallelDist>0&&DISTANCE(pVertPlate->parallel_line.startPt,pVertPlate->parallel_line.endPt)>0)
		{	
			f3dPoint mid_pt[2]={0.5*(pVertPlate->face_line[0][0].startPt+pVertPlate->face_line[0][0].endPt),
								0.5*(pVertPlate->face_line[0][1].startPt+pVertPlate->face_line[0][1].endPt)};
			space_dim.dimStartPos=mid_pt[0];
			if(DISTANCE(mid_pt[0],pVertPlate->parallel_line.startPt)>DISTANCE(mid_pt[1],pVertPlate->parallel_line.startPt))
				space_dim.dimStartPos=mid_pt[1];
			SnapPerp(&space_dim.dimEndPos,pVertPlate->parallel_line,space_dim.dimStartPos,&pVertPlate->fParallelDist);
			space_dim.dimStartPos.z=space_dim.dimEndPos.z=0;
			space_dim.dist=pVertPlate->fParallelDist;
			space_dim.bAllowRelayout=FALSE;
			space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
			space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
			space_dim.iOrder = 0;	//װ�����߳ߴ�
			pContext->AppendSizeTextDim(space_dim);
		}
		//2.2 �����������������˨����ע
		int nFace=pVertPlate->pPlate->GetFaceN();
		f3dPoint boltPosArr[2]={pVertPlate->startBoltPos,pVertPlate->endBoltPos};
		f3dPoint basePosArr[2][2]={{pVertPlate->face_line[0][0].startPt,pVertPlate->face_line[0][1].startPt},
								   {pVertPlate->face_line[nFace-1][0].endPt,pVertPlate->face_line[nFace-1][1].endPt}};
		if(pVertPlate->startBoltPos.IsEqual(pVertPlate->endBoltPos.x,pVertPlate->endBoltPos.y,pVertPlate->endBoltPos.z))
		{	//ʼ�ն�λ�ò��յ���ͬʱ����ע����յ�Ͻ���һ��
			if(DISTANCE(pVertPlate->startBoltPos,basePosArr[0][0])<DISTANCE(pVertPlate->endBoltPos,basePosArr[1][0]))
				pVertPlate->bDimEnd=FALSE;
			else
				pVertPlate->bDimStart=FALSE;
		}
		for(int i=0;i<2;i++)
		{
			if(boltPosArr[i].z>0)
				continue;
			if((i==0&&!pVertPlate->bDimStart)||(i==1&&!pVertPlate->bDimEnd))
				continue;
			space_dim.dimStartPos=boltPosArr[i];
			space_dim.dimEndPos=basePosArr[i][0];
			if(DISTANCE(boltPosArr[i],basePosArr[i][0])>DISTANCE(boltPosArr[i],basePosArr[i][1]))
				space_dim.dimEndPos=basePosArr[i][1];
			space_dim.dimStartPos.z=space_dim.dimEndPos.z=0;
			space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
			space_dim.bAllowRelayout=FALSE;
			space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
			space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
			space_dim.iOrder = 0;	//װ�����߳ߴ�
			pContext->AppendSizeTextDim(space_dim);
		}
	}
}
BOOL IsVisiblePlate(GECS& dcs,CLDSGeneralPlate* pPlate)
{
	PROFILE_VERTEX* pPrevVertex=pPlate->vertex_list.GetTail();
	if(pPrevVertex==NULL)
		return FALSE;
	GEPOINT start,end;
	start=pPlate->GetRealVertex(pPrevVertex->vertex);
	coord_trans(start,pPlate->ucs,TRUE,TRUE);
	if((start-dcs.origin)*dcs.axis_z>0)	//�ɼ�
		return TRUE;
	for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		end=pPlate->GetRealVertex(pVertex->vertex);
		coord_trans(end,pPlate->ucs,TRUE,TRUE);
		if((end-dcs.origin)*dcs.axis_z>0)	//�ɼ�
			return TRUE;
		/*if(pPrevVertex->type>0)
		{	//�㷨�����Ͻ����ݲ����ţ��������Ӱ��Ҳ����,��Ҫʱ������ wjh-2016.10.15
			f3dArcLine arcline;
			pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&pPlate->ucs);
			if(arcline.SectorAngle()>Pi)
				return TRUE
			arcline.CoordTrans(pPlate->ucs,true);
			arcline.CoordTrans(dcs,false);
		}*/
		pPrevVertex=pVertex;
	}
	return FALSE;
}
BOOL CPlateDrawing::CreatePlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId/*=NULL*/,int h0v1/*=-1*/)
{
	style.cbDrawPlanes=0x07;	//0x01:���ƻ���;0x02:����1�Ż�����;0x04:����2�Ż�����
	if(pPlate->face_N>1 && (pPlate->IsTubeFoldPlate()||pPlate->IsSplayFoldPlate()))
	{	//��繹�ܺ����۵��弰�������ֻ����������Ҫ��������wjh-2016.11.22
		style.cbDrawPlanes=0;
		BYTE byteConst[3]={0x01,0x02,0x04};
		for(int i=1;i<pPlate->face_N;i++)
		{
			GEPOINT vWorkNorm=pPlate->GetHuoquFaceNorm(i-1);
			if(fabs(vWorkNorm*dcs.PrjNorm())<EPS2||fabs(dcs.second.fRotateAngle)>EPS)
				style.cbDrawPlanes|=byteConst[i];	//�ڵ����ͼ����һ�����淨�߶�Ӧ���ͼƽ�淨�ߴ�ֱ
		}
		if(  style.cbDrawPlanes>0 && (
			(pPlate->designInfo.iFaceType==6&&CreateSubstationFoldPlateDrawing(pPlate,dcs,pContext,pOutEntId))||
			(pPlate->designInfo.iFaceType==7&&CreateSubstationSplayFoldPlateDrawing(pPlate,dcs,pContext,pOutEntId))))
			return TRUE;
		style.cbDrawPlanes=0x07;
		//else if(style.cbDrawPlanes==0)
		//	return FALSE;
	}
	f3dLine line;		//��ʱ������
	f3dArcLine arcline;	//��ʱ������
	CTextFrame data_point;	//��չ���ݵ� �������湹��������Ϣ
	UCS_STRU cs=dcs.cs;
	//��ʼ�����߿��ı�
	data_point.InitTextFrame(sys.fonts.weld.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	int i,n,plate_thick=ftoi(pPlate->GetThick());
	f3dPoint f_start,f_end;
	f3dPoint point,start_firs,end_firs,start_sec,end_sec,
		start_thir,end_thir,axis_start,axis_end,ext_vec,len_vec;
	int base_start_feature,base_end_feature,qu1_start_feature,qu1_end_feature,qu2_start_feature,qu2_end_feature;
	n = pPlate->vertex_list.GetNodeNum();
	if(n<3)
		return FALSE;
	//ʮ�ֲ�帽�Ӹְ�	wht 09-11-23
	BOOL bXEndAttachPlate=FALSE;
	if(pPlate->designInfo.origin.datum_pos_style==9)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pPlate->designInfo.origin.des_para.hPart,CLS_PARAMPLATE);
		if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND&&pPlate->designInfo.origin.datum_pos.x>0)
			bXEndAttachPlate=TRUE;
	}
	f3dPoint geom_center;	//�弸������
	f3dLine left_edge,right_edge;	//ˮƽ����ʱ�ְ����������Ҳ�ı�,��ע����ʱʹ��(��ͼ����ϵ�µ�����) wht 15-04-01
	UCS_STRU prj_ucs,prj_qu1_ucs,prj_qu2_ucs;	//ͶӰ����ϵ���ڻ��ư��ͶӰͼ
	double m_fPrjRotAngle=0;	//��ԭʼ�ְ�װ������ϵ��ת����ͼ����ϵʱ����ת��
	BOOL bPrjQuFace1=FALSE,bPrjQuFace2=FALSE;
	double sum_len=0;
	//���ڵ������
	f3dLine base_face_line,qu_face1_line,qu_face2_line;
	f3dPoint prjAxisZ=pPlate->ucs.axis_z;
	if(style.cbDrawPlanes&0x01)
		prjAxisZ=pPlate->ucs.axis_z;
	else if(style.cbDrawPlanes&0x02)
		prjAxisZ=pPlate->GetHuoquFaceNorm(0);
	else if(style.cbDrawPlanes&0x04)
		prjAxisZ=pPlate->GetHuoquFaceNorm(1);
	if(h0v1<0)
	{
		if(fabs(dcs.PrjNorm()*prjAxisZ)>=0.707)
			h0v1=0;	//ˮƽ����
		else
			h0v1=1;	//��ֱ����
		if(fabs(dcs.PrjNorm()*cs.axis_z)<0.707)
			h0v1=1-h0v1;	//dcs.PrjNormΪչ����ʱ,��Ҫ���������� wht 18-03-10
	}
	style.ciDrawModeH0V1=h0v1;
	GEPOINT vTubeLenVec;
	if(h0v1==1)
	{	//�����ͶӰ����ϵ,����ԭ��λ�������ཻ��
		f3dPoint pick_pt,origin=pPlate->ucs.origin+pPlate->m_fNormOffset*pPlate->ucs.axis_z;
		f3dPoint weldline_vec=pPlate->ucs.axis_z^cs.axis_z;
		f3dPoint extend_vec=pPlate->ucs.axis_z^weldline_vec;
		Int3dlf(prj_ucs.origin,origin,extend_vec,cs.origin,cs.axis_z);	//ʹͶӰԭ��λ�����潻����
		//���¼�����ת��ĸְ�Z�᷽����Ϊ�˾��ְ��Z��Ϊ�ϸ��뵱ǰ��ͼ����ϵZ�ᴹֱ,�Ա�ʡ��"�ֹܶ�ͷ�װ��и�����ͼ" wjh-2015.10.12
		if(pContext->WeldRootPart()&&pContext->WeldRootPart()->GetClassTypeId()==CLS_LINETUBE)
		{	//��׼����Ϊ�ֹ�ʱ��Ҫ��ת���������ͶӰ�����λ��
			CLDSLineTube* pRootTube=(CLDSLineTube*)pContext->WeldRootPart();
			vTubeLenVec=pRootTube->End()-pRootTube->Start();
			normalize(vTubeLenVec);
			if(fabs(vTubeLenVec*cs.axis_z)<EPS2)
			{
				prjAxisZ=pPlate->ucs.axis_z-cs.axis_x*(pPlate->ucs.axis_z*cs.axis_x);
				double rotAngle=Cal2dLineAng(0,0,prjAxisZ*cs.axis_y,prjAxisZ*cs.axis_z);
				double halfPi=0.5*Pi;
				if(rotAngle<=halfPi)
					rotAngle*=-rotAngle;
				else if(rotAngle<=1.5*Pi)
					rotAngle=Pi-rotAngle;
				else
					rotAngle=2*Pi-rotAngle;
				prjAxisZ=pPlate->ucs.axis_z;
				RotateVectorAroundVector(prjAxisZ,rotAngle,cs.axis_x);
				m_fPrjRotAngle=rotAngle;
			}
			//else
			//	prjAxisZ=pPlate->ucs.axis_z;
		}
		prj_ucs.axis_y=prjAxisZ;
		prj_ucs.axis_z=cs.axis_z;
		prj_ucs.axis_x=prj_ucs.axis_y^prj_ucs.axis_z;
		prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
		normalize(prj_ucs.axis_x);
		normalize(prj_ucs.axis_y);
		if(pPlate->face_N>1&&style.cbDrawPlanes&0x02)
		{
			f3dPoint vec=pPlate->ucs.axis_z^pPlate->huoqufacenorm[0];
			normalize(vec);
			if(fabs(vec*cs.axis_z)>0.9)
			{
				bPrjQuFace1=TRUE;
				f3dLine tmp_line(pPlate->HuoQuLine[0].startPt,pPlate->HuoQuLine[0].endPt);
				tmp_line.startPt.z+=pPlate->GetNormOffset();
				tmp_line.endPt.z  +=pPlate->GetNormOffset();
				coord_trans(tmp_line.startPt,pPlate->ucs,TRUE);
				coord_trans(tmp_line.endPt,pPlate->ucs,TRUE);
				Int3dlf(origin,tmp_line,cs.origin,cs.axis_z);	//Int3dlfʧ��ʱ�������originֵ
				prj_qu1_ucs.origin=origin;
				prj_qu1_ucs.axis_y=pPlate->huoqufacenorm[0];
				prj_qu1_ucs.axis_z=cs.axis_z;
				prj_qu1_ucs.axis_x=prj_qu1_ucs.axis_y^prj_qu1_ucs.axis_z;
				prj_qu1_ucs.axis_y=prj_qu1_ucs.axis_z^prj_qu1_ucs.axis_x;
				normalize(prj_qu1_ucs.axis_x);
				normalize(prj_qu1_ucs.axis_y);
			}
		}
		if(pPlate->face_N>2&&style.cbDrawPlanes&0x04)
		{
			f3dPoint vec=pPlate->ucs.axis_z^pPlate->huoqufacenorm[1];
			normalize(vec);
			if(fabs(vec*cs.axis_z)>0.9)
			{
				bPrjQuFace2=TRUE;
				f3dLine tmp_line(pPlate->HuoQuLine[1].startPt,pPlate->HuoQuLine[1].endPt);
				tmp_line.startPt.z+=pPlate->GetNormOffset();
				tmp_line.endPt.z  +=pPlate->GetNormOffset();
				coord_trans(tmp_line.startPt,pPlate->ucs,TRUE);
				coord_trans(tmp_line.endPt,pPlate->ucs,TRUE);
				Int3dlf(origin,tmp_line,cs.origin,cs.axis_z);
				prj_qu2_ucs.origin=origin;
				prj_qu2_ucs.axis_y=pPlate->huoqufacenorm[1];
				prj_qu2_ucs.axis_z=cs.axis_z;
				prj_qu2_ucs.axis_x=prj_qu2_ucs.axis_y^prj_qu2_ucs.axis_z;
				prj_qu2_ucs.axis_y=prj_qu2_ucs.axis_z^prj_qu2_ucs.axis_x;
				normalize(prj_qu2_ucs.axis_x);
				normalize(prj_qu2_ucs.axis_y);
			}
		}
	}
	if(h0v1==0&&pContext->IsHideFuncSetted())
	{
		fPtList vertexlist;
		//TODO:δ��ɣ���������ƽṹͼ�иְ���ƴ��뼯��ʱһ����� wjh-2016.3.19
		/*for(i=0;i<n;i++)
		{
			f_start = pPlate->GetRealVertex(pPlate->vertex_list [i].vertex);
			f_end   = pPlate->GetRealVertex(pPlate->vertex_list [(i+1)%n].vertex);
			coord_trans(f_start,pPlate->ucs,TRUE);
			coord_trans(f_start,ucs,FALSE);
			f_start = f_start*(1.0/scale);
			f_start.z = 0;
			vertexlist.append(f_start);
		}
		if(vertexlist.GetNodeNum()>0)	//���������
		{
			pContext->HideLegacyLines(vertexlist,"1.������");
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		}*/
	}
	//short penStyle=PS_SOLID;
	//if(!bCoplanar)	//�ǹ���ĸ˼�ͳһ�����߱�ʶ
	short penStyle=style.penStyle;//PS_DASH;
	BOOL bDimHandle=TRUE;
	PROFILE_VERTEX* pVertex;
	ARRAY_LIST<PROFILE_VERTEX*>verticePtrs(0,pPlate->vertex_list.GetNodeNum());
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		verticePtrs.append(pVertex);
	CSectionDrawingEntsManager::HORZ_PLATE *pHorzPlate=NULL;
	CSectionDrawingEntsManager *pEntsManager=NULL;
	if(pContext->IsSectionDrawing()&&h0v1==0)
	{
		if(pContext->WeldRootPart()==pPlate)
			pEntsManager=(CSectionDrawingEntsManager*)pContext;
		else if(pContext->WeldRootPart()->IsTube())
			pHorzPlate=((CSectionDrawingEntsManager*)pContext)->m_hashFootConnectPlateList.Add(pPlate->handle);
	}

	DATAPOINT_TAG* pLabelTag=pContext->AppendDataPoineTag();
	data_point.dwKeyTag=pLabelTag->key_tag;
	GEPOINT rightest,leftest,xArcTipPoint[2];
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=verticePtrs[i];
		PROFILE_VERTEX *pEndVertex=verticePtrs[(i+1)%n];
		if(!style.bSpreadBendPlane)
		{
			f_start = pPlate->GetRealVertex(pStartVertex->vertex);
			f_end   = pPlate->GetRealVertex(pEndVertex->vertex);
		}
		else
		{
			f_start = pStartVertex->vertex;
			f_end   = pEndVertex->vertex;
		}
		BYTE cArcTipFlag=0;
		if(pStartVertex->type>0)
		{
			f3dArcLine arcline;
			pStartVertex->RetrieveArcLine(arcline,pEndVertex->vertex,&pPlate->ucs);
			UCS_STRU tcs=pPlate->ucs;
			if(!prj_ucs.axis_x.IsZero())
			{	//ֻ����prj_ucs��ʼ����ִ��(h0v1==1ʱ�� wjh-2018.7.31
				arcline.CoordTrans(pPlate->ucs,true);
				tcs.axis_x=prj_ucs.axis_x;
				RotateVectorAroundVector(tcs.axis_x,-m_fPrjRotAngle,cs.axis_x);
				tcs.axis_y=tcs.axis_z^tcs.axis_x;
				tcs.axis_z=tcs.axis_x^tcs.axis_y;
				arcline.CoordTrans(tcs,false);
			}
			GEPOINT center=arcline.Center(),xArcStart=arcline.Start();
			double startAngle=Cal2dLineAng(center.x,center.y,xArcStart.x,xArcStart.y);
			if(startAngle<EPS||fabs(startAngle-Pi)<EPS)
			{
				xArcTipPoint[0]=arcline.Start();
				cArcTipFlag=1;
				if(!prj_ucs.axis_x.IsZero())
				{
					coord_trans(xArcTipPoint[0],tcs,TRUE);
					rotate_point_around_axis(xArcTipPoint[0],m_fPrjRotAngle,cs.origin,cs.origin+cs.axis_x);
					coord_trans(xArcTipPoint[0],prj_ucs,FALSE);
				}
			}
			else if(Pi>startAngle&&Pi<startAngle+arcline.SectorAngle())
			{
				xArcTipPoint[0]=arcline.PositionInAngle(Pi-startAngle);
				cArcTipFlag=1;
				if(!prj_ucs.axis_x.IsZero())
				{	//ֻ����prj_ucs��ʼ����ִ��(h0v1==1ʱ�� wjh-2018.7.31
					coord_trans(xArcTipPoint[0],tcs,TRUE);
					rotate_point_around_axis(xArcTipPoint[0],m_fPrjRotAngle,cs.origin,cs.origin+cs.axis_x);
					coord_trans(xArcTipPoint[0],prj_ucs,FALSE);
				}
			}
			if(startAngle+arcline.SectorAngle()>2*Pi)
			{
				xArcTipPoint[1]=arcline.PositionInAngle(2*Pi-startAngle);
				cArcTipFlag+=2;
				if(!prj_ucs.axis_x.IsZero())
				{	//ֻ����prj_ucs��ʼ����ִ��(h0v1==1ʱ�� wjh-2018.7.31
					coord_trans(xArcTipPoint[1],tcs,TRUE);
					rotate_point_around_axis(xArcTipPoint[1],m_fPrjRotAngle,cs.origin,cs.origin+cs.axis_x);
					coord_trans(xArcTipPoint[1],prj_ucs,FALSE);
				}
			}
			xArcTipPoint[0].y=xArcTipPoint[1].y=xArcTipPoint[0].z=xArcTipPoint[1].z=0;
		}
		double len=DISTANCE(f_start,f_end);
		geom_center =geom_center+len*(f_start+f_end);
		sum_len+=DISTANCE(f_start,f_end);
		coord_trans(f_start,pPlate->ucs,TRUE);
		coord_trans(f_end,pPlate->ucs,TRUE);
		if(h0v1==0)
		{	//ˮƽ����
			f_start=dcs.TransToDraw(f_start);//coord_trans(f_start,cs,FALSE);f_start.z = 0;
			f_end=dcs.TransToDraw(f_end);//coord_trans(f_end,cs,FALSE);f_end.z   = 0;
			bool isLeft=false,isRight=false;
			if(left_edge.startPt.IsZero()&&left_edge.endPt.IsZero())
			{	//��ʼ�����Ҳ�������
				right_edge.startPt=left_edge.startPt=f_start;
				right_edge.endPt=left_edge.endPt=f_end;
				rightest=leftest=(f_start+f_end)*0.5;
				isLeft=isRight=true;
			}
			else 
			{
				double left_x=0.5*(left_edge.startPt.x+left_edge.endPt.x);
				double right_x=0.5*(right_edge.startPt.x+right_edge.endPt.x);
				double cur_x=0.5*(f_start.x+f_end.x);
				if(cur_x>right_x)
				{
					right_edge.startPt=f_start;
					right_edge.endPt=f_end;
					rightest=(f_start+f_end)*0.5;
					isRight=true;
				}
				if(cur_x<left_x)
				{
					left_edge.startPt=f_start;
					left_edge.endPt=f_end;
					leftest=(f_start+f_end)*0.5;
					isLeft=true;
				}
			}
			pLabelTag->vertexArr.append(f_start);
			if(pStartVertex->type==0)
			{
				line.startPt=f_start;
				line.endPt=f_end;
				line.pen.style=penStyle;
				/*if(bDimHandle&&((bXEndAttachPlate&&i!=0)||!bXEndAttachPlate))
				{	//�ְ���Ϣ����¼�ڵ�һ��ֱ�߱��� ��Ҫ�ǿ���ʮ�ֲ�帽�Ӱ��ע�غ�����
					line.ID=pPlate->handle;
					bDimHandle=FALSE;
				}
				else*/
				line.ID=0;
				pContext->AppendLine(line);
				if(pEntsManager) //���ݻ��������߸��������ע���� wht 16-03-29
					pEntsManager->UpdateVerticalPlateParallelLine(GELINE(line.startPt,line.endPt));
				if(pHorzPlate)	//��¼ˮƽ���Ƹְ��������  wht 18-08-25
					pHorzPlate->edgeLineList.append(line);
				if(style.bDrawWeldLine&&pStartVertex->m_bWeldEdge)
				{
					if(pPlate->ucs.axis_z*dcs.PrjNorm()>0)
						pContext->DrawWeldLineSymbol(f_start,f_end);
					else 
						pContext->DrawWeldLineSymbol(f_end,f_start);
				}
			}
			else if(pStartVertex->type<3)	//Բ��
			{
				PROFILE_VERTEX arcStart=*pStartVertex;
				arcStart.vertex=f_start;
				arcStart.center=pStartVertex->center;
				coord_trans(arcStart.center,pPlate->ucs,TRUE);
				arcStart.center=dcs.TransToDraw(arcStart.center);	//Բ��ʰȡ������ת��wjh 2011-11-09
				if(dcs.PrjNorm()*pPlate->ucs.axis_z<0)	//ͶӰ������巨�߷�����
				{
					if(arcStart.type==1)
						arcStart.sector_angle*=-1.0;
					else
						arcStart.radius*=-1.0;
				}
				arcStart.RetrieveArcLine(arcline,f_end,NULL);
				if(isLeft||isRight)
				{
					double midangle=arcline.SectorAngle()*0.634;
					GEPOINT pos=arcline.PositionInAngle(midangle);
					if(isLeft)
						leftest=pos;
					if(isRight)
						rightest=pos;
				}
				/*if(bDimHandle)
				{
					arcline.ID=pPlate->handle;
					bDimHandle=FALSE;
				}
				else*/
				arcline.ID=0;
				arcline.pen.style=penStyle;
				pContext->AppendLine(arcline);
				if(style.bDrawWeldLine&&pStartVertex->m_bWeldEdge)
					pContext->DrawWeldLineSymbol(arcline);
			}
			else if(pStartVertex->type==3)	//��Բ��
			{
				PROFILE_VERTEX arcStart=*pStartVertex;
				arcStart.vertex=f_start;
				if(dcs.PrjNorm()*pPlate->ucs.axis_z<0)	//ͶӰ������巨�߷�����
					arcStart.radius*=-1.0;
				normalize(arcStart.column_norm);
				//double tolerance=100;
				double tcosa=fabs(arcStart.column_norm*pPlate->ucs.axis_z);
				if(tcosa<EPS2)
				{	//Ŀǰ��Ϊֱ��ͨ��tcosa<EPS2�жϻ��������㹤����������Ҫ�������������� wjh-2017.11.29
				//	double distance=DISTANCE(f_start,f_end);
				//	double halfa=tcosa*0.5*distance;	//������ư��ҳ�
				//	tolerance=fabs(arcStart.radius)*(1-SQRT(1-halfa*halfa));
				//}
				//if(tolerance<1)	//Բ��������ְ�ӽ�ƽ��ʱ���ְ���Բ���߽���Ϊֱ�� wjh-2017.11.29
				//{
					f3dLine line(f_start,f_end);
					pContext->AppendLine(line);
					if(style.bDrawWeldLine&&pStartVertex->m_bWeldEdge)
						pContext->DrawWeldLineSymbol(line);
				}
				else
				{
					//���ͶӰ������巨�߷���ƽ�У���Բ������Ϣ��ʧ�棬������������	wxc 2016-8-26
					if(fabs(pPlate->ucs.axis_z*dcs.cs.axis_z)>EPS_COS2)	//��ʱ����EPS_COS�Ժ��кý��������˵ wjh-2018.8.1
						arcStart.RetrieveArcLine(arcline,f_end,&cs);	//Ӧת������ͼ����ϵ��,�Ǹְ�����ϵ wht 10-08-12
					//else if(arcline)
						//TODO:�����Խǹ��ð����Բ����ʱ�ᱻ������Ƴ�ֱ�� wjh-2018.7.31
					else
						arcline=f3dArcLine(f3dLine(f_start,f_end));
					if((isLeft||isRight)&&arcline.SectorAngle()>0)
					{
						double midangle=arcline.SectorAngle()*0.634;
						GEPOINT pos=arcline.PositionInAngle(midangle);
						if(isLeft)
							leftest=pos;
						if(isRight)
							rightest=pos;
					}
					arcline.ID=0;
					arcline.pen.style=penStyle;
					pContext->AppendLine(arcline);
					if(style.bDrawWeldLine&&pStartVertex->m_bWeldEdge)
						pContext->DrawWeldLineSymbol(arcline);
				}
			}
		}
		else
		{	//��ֱ����
			int judge,min_face,max_face;
			f_start+=pPlate->ucs.axis_z*pPlate->GetNormOffset();
			f_end  +=pPlate->ucs.axis_z*pPlate->GetNormOffset();
			min_face=min(f_start.feature/10,f_start.feature%10);
			max_face=max(f_start.feature/10,f_start.feature%10);
			if((style.cbDrawPlanes&0x01)&&(min_face==1||max_face==1))
			{	//��һ���ϵİ嶥��
				f3dPoint start=f_start;
				f3dPoint end=f_end;
				double zCoordVerify=(start-cs.origin)*cs.axis_z;
				//��֤startλ��ͶӰ����ϵ��������
				if(style.siMinFilterZ!=style.siMaxFilterZ&&(zCoordVerify<style.siMinFilterZ||zCoordVerify>style.siMaxFilterZ))
					continue;//�ݵװ��뵱ǰ��ͼ����ϵԭ����ȷ�������Z���趨��Χ,������Χ�ĸְ��������㣬������ͶӰ����Ҫ���ڴ���׽Ŵ���忿�ӱ���бӰ��ͶӰ���ȣ�wjh-2018.3.12
				//TODO:��Ҫ����
				//if(start.z>0)
				//	start+=xFlSectNormalOffset.vxTubeStdUpperAxisVec*xFlSectNormalOffset.siMaxFilterZ;
				//else if(start.z<0)
				//	start+=xFlSectNormalOffset.vxTubeStdUpperAxisVec*xFlSectNormalOffset.siMinFilterZ;
				coord_trans(start,prj_ucs,FALSE);
				start.y=start.z=0;
				if(base_face_line.startPt==base_face_line.endPt)	//ȱʡδ��ֵ
				{
					coord_trans(end,prj_ucs,FALSE);
					end.y=end.z=0;
					if(start.x<end.x)
					{
						base_face_line.startPt=start;
						base_face_line.endPt = end;
						base_start_feature=start.feature;
						base_end_feature=end.feature;
					}
					else
					{
						base_face_line.startPt=end;
						base_face_line.endPt = start;
						base_start_feature=end.feature;
						base_end_feature=start.feature;
					}
				}
				else
				{
					//judge=base_face_line.PtInLine(start);
					if(start.x>base_face_line.endPt.x)		//�յ����
					{
						base_face_line.endPt=start;
						base_end_feature=start.feature;
					}
					else if(start.x<base_face_line.startPt.x)	//������
					{
						base_face_line.startPt=start;
						base_start_feature=start.feature;
					}
				}
			}
			if((style.cbDrawPlanes&0x02)&&bPrjQuFace1&&(min_face==2||max_face==2))
			{	//�ڶ����ϵİ嶥��
				f3dPoint start=f_start;
				f3dPoint end=f_end;
				//TODO:δ����pPlate->m_fNormOffsetӰ�� wjh-2016.3.16
				coord_trans(start,prj_qu1_ucs,FALSE);
				start.y=start.z=0;
				if(qu_face1_line.startPt==qu_face1_line.endPt)	//ȱʡδ��ֵ
				{
					qu_face1_line.startPt=start;
					coord_trans(end,prj_qu1_ucs,FALSE);
					end.y=end.z=0;
					qu_face1_line.endPt = end;
					qu1_start_feature=start.feature;
					qu1_end_feature=end.feature;
				}
				else
				{
					judge=qu_face1_line.PtInLine(start);
					if(judge==-1)		//�յ����
					{
						qu_face1_line.endPt=start;
						qu1_end_feature=start.feature;
					}
					else if(judge==-2)	//������
					{
						qu_face1_line.startPt=start;
						qu1_start_feature=start.feature;
					}
				}
			}
			if((style.cbDrawPlanes&0x04)&&bPrjQuFace2&&(min_face==3||max_face==3))
			{	//�������ϵİ嶥��
				f3dPoint start=f_start;
				f3dPoint end=f_end;
				//TODO:δ����pPlate->m_fNormOffsetӰ�� wjh-2016.3.16
				coord_trans(start,prj_qu2_ucs,FALSE);
				start.y=start.z=0;
				if(qu_face2_line.startPt==qu_face2_line.endPt)	//ȱʡδ��ֵ
				{
					qu_face2_line.startPt=start;
					coord_trans(end,prj_qu2_ucs,FALSE);
					end.y=end.z=0;
					qu_face2_line.endPt = end;
					qu2_start_feature=start.feature;
					qu2_end_feature=end.feature;
				}
				else
				{
					judge=qu_face2_line.PtInLine(start);
					if(judge==-1)		//�յ����
					{
						qu_face2_line.endPt=start;
						qu2_end_feature=start.feature;
					}
					else if(judge==-2)	//������
					{
						qu_face2_line.startPt=start;
						qu2_start_feature=start.feature;
					}
				}
			}
			//����Բ����͹�������ͶӰ������Ӱ��
			const BYTE byteContArr[3]={1,2,4};
			for(int j=0;j<2;j++)
			{
				if((cArcTipFlag&byteContArr[j])==0)
					continue;
				if(xArcTipPoint[j].x>base_face_line.endPt.x)		//�յ����
				{
					base_face_line.endPt=xArcTipPoint[j];
					base_end_feature=pStartVertex->vertex.feature;
				}
				else if(xArcTipPoint[j].x<base_face_line.startPt.x)	//������
				{
					base_face_line.startPt=xArcTipPoint[j];
					base_start_feature=pStartVertex->vertex.feature;
				}
			}
		}
	}
	if(h0v1==0)
	{
		if(pPlate->m_fInnerRadius>0&&style.bDrawInnerCircle)
		{
			if(!pPlate->m_tInnerColumnNorm.IsZero()&&fabs(pPlate->m_tInnerColumnNorm*pPlate->ucs.axis_z)<EPS_COS)	//Բ��
			{	//��Բ	�ڻ����麸��ʱ������ֵ���ڵ�ǰ����ϵ�µĿ�����Ч��ͼ  wht 09-08-28
				f3dPoint workNorm(0,0,1),columnNorm=pPlate->m_tInnerColumnNorm;
				if(pPlate->ucs.axis_z*dcs.PrjNorm()<0)	//�巨���뵱ǰ��ͼ����ϵZ�᷽����
					workNorm.z*=-1.0;
				vector_trans(columnNorm,pPlate->ucs,FALSE);
				f3dPoint minorAxis=columnNorm^workNorm;	
				normalize(minorAxis);	//��Բ���᷽��
				//˳ʱ����ת90�� wht 11-07-19
				f3dPoint majorAxis(minorAxis.y,-minorAxis.x,minorAxis.z);	
				normalize(majorAxis);	//��Բ���᷽��
				double radiusRatio=fabs(columnNorm*workNorm);
				double minorRadius=pPlate->m_fInnerRadius;				//��Բ�̰��᳤��
				double majorRadius=pPlate->m_fInnerRadius/radiusRatio;	//��Բ�����᳤��
				f3dPoint firstPt=pPlate->m_tInnerOrigin+majorAxis*majorRadius;
				f3dPoint secPt = pPlate->m_tInnerOrigin+minorAxis*minorRadius;
				f3dPoint thirdPt=pPlate->m_tInnerOrigin-majorAxis*majorRadius;
				f3dPoint fourPt = pPlate->m_tInnerOrigin-minorAxis*minorRadius;
				f3dPoint inner_origin=pPlate->m_tInnerOrigin;
				//ת������������ϵ��
				coord_trans(inner_origin,pPlate->ucs,TRUE);
				coord_trans(firstPt,pPlate->ucs,TRUE);
				coord_trans(secPt,pPlate->ucs,TRUE);
				coord_trans(thirdPt,pPlate->ucs,TRUE);
				coord_trans(fourPt,pPlate->ucs,TRUE);
				vector_trans(columnNorm,pPlate->ucs,TRUE);
				//ת������ͼ����ϵ��
				inner_origin=dcs.TransToDraw(inner_origin);
				firstPt=dcs.TransToDraw(firstPt);
				secPt=dcs.TransToDraw(secPt);
				thirdPt=dcs.TransToDraw(thirdPt);
				fourPt=dcs.TransToDraw(fourPt);
				vector_trans(columnNorm,dcs,FALSE,TRUE);
				inner_origin.z=firstPt.z=secPt.z=thirdPt.z=fourPt.z=0;	//Z����������
				//
				arcline.pen.style=penStyle;
				arcline.ID=0;
				arcline.CreateEllipse(inner_origin,firstPt,secPt,columnNorm,workNorm,pPlate->m_fInnerRadius);
				pContext->AppendLine(arcline);
				arcline.CreateEllipse(inner_origin,secPt,thirdPt,columnNorm,workNorm,pPlate->m_fInnerRadius);
				pContext->AppendLine(arcline);
				arcline.CreateEllipse(inner_origin,thirdPt,fourPt,columnNorm,workNorm,pPlate->m_fInnerRadius);
				pContext->AppendLine(arcline);
				arcline.CreateEllipse(inner_origin,fourPt,firstPt,columnNorm,workNorm,pPlate->m_fInnerRadius);
				pContext->AppendLine(arcline);
			}
			else
			{
				f3dPoint center=pPlate->m_tInnerOrigin;	//pPlate->ucs.origin;
				coord_trans(center,pPlate->ucs,TRUE);
				f3dPoint start_pt=center+pPlate->ucs.axis_x*pPlate->m_fInnerRadius;
				center=dcs.TransToDraw(center);
				start_pt=dcs.TransToDraw(start_pt);
				center.z=start_pt.z=0;	//Z�������
				arcline.CreateMethod1(center,start_pt,f3dPoint(0,0,1),2*Pi);
				arcline.ID=0;
				arcline.pen.style=penStyle;
				pContext->AppendLine(arcline);
			}
		}
		//������չ���ݵ㣬�Դ洢���Ӱ���Ϣ
		geom_center/=2*sum_len;
		coord_trans(geom_center,pPlate->ucs,TRUE);
		geom_center=dcs.TransToDraw(geom_center);//coord_trans(geom_center,cs,FALSE);geom_center.z=0;
		data_point.cls_id=pPlate->GetClassTypeId();
		data_point.hPart=pPlate->handle;
		if(style.geomcenter0_left1_right2==0)
		{	//���ݵ��������������λ��
			data_point.dataPos=geom_center;
			data_point.dimPos=geom_center;
		}
		else if(style.geomcenter0_left1_right2==1)
		{	//���ݵ�������������е�
			data_point.dataPos=leftest;//0.5*(left_edge.startPt+left_edge.endPt);
			data_point.dimPos=data_point.dataPos;
			data_point.dimPos.x-=sys.dim_map.PartNo.fPartNoTextSize*1.5;
		}
		else if(style.geomcenter0_left1_right2==2)
		{	//���ݵ���������Ҳ���е�
			data_point.dataPos=rightest;//0.5*(right_edge.startPt+right_edge.endPt);
			data_point.dimPos=data_point.dataPos;
			data_point.dimPos.x+=sys.dim_map.PartNo.fPartNoTextSize*1.5;
		}
		data_point.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	}
	else if(h0v1==1)
	{
		//ȷ���ְ�����������
		f3dPoint center_pos=geom_center;	//�ְ弸������
		center_pos/=2*sum_len;
		coord_trans(center_pos,pPlate->ucs,TRUE);
		f3dPoint extended_vec=center_pos-cs.origin;//pPlate->ucs.origin;
		normalize(extended_vec);
		if(pContext->WeldRootPart()&&pContext->WeldRootPart()->GetClassTypeId()==CLS_LINETUBE&&fabs(vTubeLenVec*cs.axis_z)<EPS2)
		{	//����ͼ�������ĸְ������߱�ʾ
			//if(extended_vec*cs.axis_z<0&&fabs(cs.axis_z*pContext->WeldRootPart()->ucs.axis_z)<EPS2)
			if(!IsVisiblePlate(GECS(cs.origin,cs.axis_x,cs.axis_y,cs.axis_z),pPlate))
			//if(!IsVisiblePlate(cs,pPlate))
				penStyle=PS_DASH;
		}
		else if(pContext->WeldRootPart()&&pContext->WeldRootPart()->IsPlate()&&
			fabs(pPlate->ucs.axis_z*pContext->WeldRootPart()->ucs.axis_z)<0.5&&	//cos(60��)=0.5
			extended_vec*cs.axis_z<0)	//�ְ��ں��ӵװ屳��
			penStyle=PS_DASH;
		line.ID=0;
		line.pen.style=penStyle;
		CSectionDrawingEntsManager::VERTICAL_PLATE *pVertPlate=NULL;
		if(pContext->IsSectionDrawing())
		{
			pVertPlate=((CSectionDrawingEntsManager*)pContext)->m_hashVertPlateList.GetValue(pPlate->handle);
			if(pVertPlate==NULL)
			{	
				pVertPlate=((CSectionDrawingEntsManager*)pContext)->m_hashVertPlateList.Add(pPlate->handle);
				pVertPlate->pPlate=pPlate;
			}
		}
		//���Ƹְ��������
		f3dLine base_offset_line;
		//prj_ucs.origin���ѿ���pPlate->m_fNormOffset,�����ظ����� wjh-2016.10.15
		//base_face_line.startPt.y+=pPlate->m_fNormOffset;
		//base_face_line.endPt.y+=pPlate->m_fNormOffset;
		coord_trans(base_face_line.startPt,prj_ucs,TRUE);
		coord_trans(base_face_line.endPt,prj_ucs,TRUE);
		base_face_line.startPt=dcs.TransToDraw(base_face_line.startPt);
		base_face_line.endPt=dcs.TransToDraw(base_face_line.endPt);
		base_face_line.startPt.z=base_face_line.endPt.z=0;
		f3dPoint base_vec(prj_ucs.axis_y);
		base_vec=dcs.TransVToDraw(base_vec);
		normalize(base_vec);		
		base_offset_line.startPt=base_face_line.startPt+base_vec*plate_thick;
		base_offset_line.endPt=base_face_line.endPt+base_vec*plate_thick;
		double minDrawModelThick=pContext->TransToModelLength(sys.part_group_map.fMinDrawThick);
		//��ʱ��������С���ƺ�ȣ�������С���ƺ�Ⱥ���ܵ��³�ͼ��Э��(���磺�������߰��м�Ľڵ���϶,ʮ�ֲ�����ͼ) wht 18-03-14
		/*if(minDrawModelThick>plate_thick)
		{
			GEPOINT vSideExpand=((minDrawModelThick-plate_thick)*0.5)*base_vec;
			base_face_line.startPt	-=vSideExpand;
			base_face_line.endPt	-=vSideExpand;
			base_offset_line.startPt+=vSideExpand;
			base_offset_line.endPt	+=vSideExpand;
		}*/
		for(int i=0;i<2;i++)
		{
			UCS_STRU qu_ucs;
			f3dPoint offset_vec;
			int tagQS=0,tagQE=0;
			f3dLine qu_offset_line,qu_face_line;
			if(i==0 && bPrjQuFace1)
			{
				qu_face_line=qu_face1_line;
				qu_ucs=prj_qu1_ucs;
				offset_vec=prj_qu1_ucs.axis_y;
				tagQS=qu1_start_feature;
				tagQE=qu1_end_feature;
			}
			else if(i==1 && bPrjQuFace2)
			{
				qu_face_line=qu_face2_line;
				qu_ucs=prj_qu2_ucs;
				offset_vec=prj_qu2_ucs.axis_y;
				tagQS=qu2_start_feature;
				tagQE=qu2_end_feature;
			}
			else
				break;
			coord_trans(qu_face_line.startPt,qu_ucs,TRUE);
			coord_trans(qu_face_line.endPt,qu_ucs,TRUE);
			qu_face_line.startPt=dcs.TransToDraw(qu_face_line.startPt);
			qu_face_line.endPt=dcs.TransToDraw(qu_face_line.endPt);
			qu_face_line.startPt.z=qu_face_line.endPt.z=0;
			offset_vec=dcs.TransVToDraw(offset_vec);
			normalize(offset_vec);
			qu_offset_line.startPt=qu_face_line.startPt+offset_vec*plate_thick;
			qu_offset_line.endPt=qu_face_line.endPt+offset_vec*plate_thick;
			if(minDrawModelThick>plate_thick)
			{
				GEPOINT vSideExpand=((minDrawModelThick-plate_thick)*0.5)*offset_vec;
				qu_face_line.startPt	-=vSideExpand;
				qu_face_line.endPt	-=vSideExpand;
				qu_offset_line.startPt+=vSideExpand;
				qu_offset_line.endPt	+=vSideExpand;
			}
			f3dPoint vertex1,vertex2;
			int ret1=Int3dll(base_face_line,qu_face_line,vertex1);
			int ret2=Int3dll(base_offset_line,qu_offset_line,vertex2);
			if(tagQS>10)
			{
				qu_face_line.startPt=vertex1;
				qu_offset_line.startPt=vertex2;
			}
			else if(tagQE>10)
			{
				qu_face_line.endPt=vertex1;
				qu_offset_line.endPt=vertex2;
			}
			int max_face_S=max(base_start_feature/10,base_start_feature%10);
			int max_face_E=max(base_end_feature/10,base_end_feature%10);
			if(i==0)
			{
				if(max_face_S==2)
				{
					base_face_line.startPt=vertex1;
					base_offset_line.startPt=vertex2;
				}
				if(max_face_E==2)
				{	
					base_face_line.endPt=vertex1;
					base_offset_line.endPt=vertex2;
				}
			}
			else
			{
				if(max_face_S==3)
				{
					base_face_line.startPt=vertex1;
					base_offset_line.startPt=vertex2;
				}
				if(max_face_E==3)
				{	
					base_face_line.endPt=vertex1;
					base_offset_line.endPt=vertex2;
				}
			}
			//���ƻ�����������
			line.startPt=qu_face_line.startPt;
			line.endPt=qu_face_line.endPt;
			pLabelTag->vertexArr.append(line.startPt);
			pLabelTag->vertexArr.append(line.endPt);
			pContext->AppendLine(line);
			line.startPt=qu_offset_line.startPt;
			line.endPt=qu_offset_line.endPt;
			pLabelTag->vertexArr.append(line.endPt);
			pLabelTag->vertexArr.append(line.startPt);
			pContext->AppendLine(line);
			if(pVertPlate)
			{	//��¼�������������ߣ����ڱ�ע����λ�� wht 16-03-28
				pVertPlate->face_line[i+1][0]=qu_face_line;
				pVertPlate->face_line[i+1][1]=qu_offset_line;
			}
			if(tagQS<10)
			{
				line.startPt=qu_face_line.startPt;
				line.endPt=qu_offset_line.startPt;
				pContext->AppendLine(line);
			}
			if(tagQE<10)
			{
				line.startPt=qu_face_line.endPt;
				line.endPt=qu_offset_line.endPt;
				pContext->AppendLine(line);
			}
		}
		//��׼��������
		line.startPt=base_face_line.startPt;
		line.endPt=base_face_line.endPt;
		if(!bPrjQuFace1)
		{
			pLabelTag->vertexArr.append(line.startPt);
			pLabelTag->vertexArr.append(line.endPt);
		}
		pContext->AppendLine(line);
		line.startPt=base_offset_line.startPt;
		line.endPt=base_offset_line.endPt;
		if(!bPrjQuFace1)
		{
			pLabelTag->vertexArr.append(line.endPt);
			pLabelTag->vertexArr.append(line.startPt);
		}
		pContext->AppendLine(line);
		if(pVertPlate)
		{	//��¼�����������ߣ����ڱ�ע����λ�� wht 16-03-28
			pVertPlate->face_line[0][0]=base_face_line;
			pVertPlate->face_line[0][1]=base_offset_line;
		}
		//���ƺ����
		int max_face=max(base_start_feature/10,base_start_feature%10);
		if(base_start_feature<10||(max_face==2&&!bPrjQuFace1)||(max_face==3&&!bPrjQuFace2))
		{
			line.startPt=base_face_line.startPt;
			line.endPt=base_offset_line.startPt;
			pContext->AppendLine(line);
		}
		max_face=max(base_end_feature/10,base_end_feature%10);
		if(base_end_feature<10||(max_face==2&&!bPrjQuFace1)||(max_face==3&&!bPrjQuFace2))
		{
			line.startPt=base_face_line.endPt;
			line.endPt=base_offset_line.endPt;
			pContext->AppendLine(line);
		}
		//������ݱ궨��
		data_point.iCurLayer=penStyle;	//������ڲ�
		data_point.cls_id=pPlate->GetClassTypeId();
		data_point.hPart=pPlate->handle;
		data_point.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
		f3dPoint dim_vec=base_face_line.endPt-base_face_line.startPt;
		normalize(dim_vec);
		if(bXEndAttachPlate)
		{	//ʮ�ֲ�帽�Ӱ�
			data_point.dataPos=0.5*(base_offset_line.endPt+base_face_line.endPt);
			data_point.dimPos=data_point.dataPos+dim_vec*plate_thick*2;
		}
		else
		{
			if(pLabelTag)
			{
				data_point.dataPos.Set();
				for(WORD k=0;k<pLabelTag->vertexArr.GetSize();k++)
					data_point.dataPos+=pLabelTag->vertexArr[k]/pLabelTag->vertexArr.GetSize();
				data_point.dimPos=data_point.dataPos;
			}
			else
			{	//TODO:���������� wjh-2018.3.9
				data_point.dataPos=base_offset_line.startPt+0.382*dim_vec*DISTANCE(base_offset_line.startPt,base_offset_line.endPt);
				data_point.dimPos=data_point.dataPos+base_vec*plate_thick;
			}
		}
	}
	pContext->AppendDataPoint(data_point);
	//dataPointList.append(data_point);
	
	if(h0v1==0)
	{
		for(i=0;i<pPlate->face_N-1;i++)
		{
			f_start = pPlate->HuoQuLine[i].startPt;
			f_end   = pPlate->HuoQuLine[i].endPt;
			CPtInLineCheck lineCheck(f_start,f_end);
			CMinDouble start_p;
			CMaxDouble end_p;
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				bool online=false;
				double len_factor=lineCheck.CheckPointEx(pVertex->vertex,&online);
				if(online)
				{
					start_p.Update(len_factor,pVertex);
					end_p.Update(len_factor,pVertex);
				}
			}
			if(start_p.IsInited())
				f_start=((PROFILE_VERTEX*)start_p.m_pRelaObj)->vertex;
			if(end_p.IsInited()&&end_p.m_pRelaObj!=start_p.m_pRelaObj)
				f_end=((PROFILE_VERTEX*)end_p.m_pRelaObj)->vertex;
			//TODO:δ����pPlate->m_fNormOffset wjh-2016.3.19
			coord_trans(f_start,pPlate->ucs,TRUE);
			coord_trans(f_end,pPlate->ucs,TRUE);
			f_start=dcs.TransToDraw(f_start);	//coord_trans(f_start,cs,FALSE);
			f_end=dcs.TransToDraw(f_end);		//coord_trans(f_end,cs,FALSE);f_start.z = f_end.z = 0;
			line.startPt=f_start;
			line.endPt=f_end;
			line.ID=0;
			line.pen.style=PS_NULL;	//ͼ��5
			pContext->AppendLine(line);
		}
	}
	//��ע�Ի����Ϣ wht 10-08-27
	if(h0v1==0&&sys.part_group_map.iPlateLocateDrawStyle>0)
	{	//ˮƽ����
		PROFILE_VERTEX *pPrevPnt=pPlate->vertex_list.GetTail();
		CLDSLineTube *pLineTube=NULL;
		pLineTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
		f3dPoint tube_len_vec;	//���Ӹ��˼� ʼ-->�� ����
		if(pLineTube)
		{
			tube_len_vec=pLineTube->End()-pLineTube->Start();
			normalize(tube_len_vec);
			vector_trans(tube_len_vec,pPlate->ucs,FALSE);	//ת����������ϵ��
		}
		for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(!pPrevPnt->m_bWeldEdge)
			{
				pPrevPnt = pVertex;
				continue;
			}
			line.pen.width=2;
			if( sys.part_group_map.iPlateLocateDrawStyle>0&&(
				pLineTube&&(pPrevPnt->local_point_vec==1||pPrevPnt->local_point_vec==2)))
			{	//���Ӹ�����Ϊ�ֹ��Ҵ��ڶԻ����Ϣ����Ҫ��ע�Ի��
				f3dPoint line_vec=pVertex->vertex-pPrevPnt->vertex;
				normalize(line_vec);
				f3dPoint N_vec(-line_vec.y,line_vec.x);	//�ְ嶨λ��׼�����ƫ�Ʒ�����
				f3dPoint local_point=pPrevPnt->RetrieveWeldLocation(pVertex->vertex,tube_len_vec);
				if(sys.part_group_map.iPlateLocateDrawStyle==2)
				{	//�������ı��
					f3dPoint pt[4],temPt;
					IDbPolyline* pPolyline=(IDbPolyline*)pContext->AppendDbEntity(IDbEntity::DbPolyline);
					pt[0]=local_point+N_vec*pContext->TransToModelLength(sys.part_group_map.iLocateSymboleHeight);
					pt[1]=local_point;
					temPt=0.5*(pt[0]+pt[1]);
					pt[2]=temPt+line_vec*pContext->TransToModelLength(min(2,sys.part_group_map.iLocateSymboleHeight*0.2));
					pt[3]=temPt;
					for(int i=0;i<4;i++)
					{
						coord_trans(pt[i],pPlate->ucs,TRUE);
						pt[i]=dcs.TransToDraw(pt[i]);
						line.endPt=dcs.TransToDraw(line.endPt);
						if(i==0)
							pPolyline->AddVertexAt(0,pt[i],1.5,1.5);
						else
							pPolyline->AddVertexAt(i,pt[i]);
					}
				}
				else
				{	//�����߱�ע��ʽ
				//��λ��ǵ�
				f3dLine startLine,midLine,endLine;
				double sign=1.0;
				//ʼ-->�� �Ի��50-->100�����뺸�Ӹ��˼�ʼ-->�շ�����ͬ	sign=1.0;
				//��-->ʼ �Ի��50-->100�����뺸�Ӹ��˼�ʼ-->�շ����෴ sign=-1.0;
				//��ʼ-->�շ����뺸�Ӹ��˼�ʼ-->�շ�����ͬ�ҶԻ�㷽��Ϊʼ-->��
				//���߱���˼��෴�ҶԻ�㷽��Ϊ��-->ʼ
				if((pPrevPnt->local_point_vec==1&&tube_len_vec*line_vec>EPS_COS2)||
					(pPrevPnt->local_point_vec==2&&(-tube_len_vec)*line_vec>EPS_COS2))
					sign=1.0;
				//��ʼ-->�շ����뺸�Ӹ��˼�ʼ-->�շ�����ͬ�ҶԻ�㷽��Ϊ��-->ʼ
				//���߱���˼������෴�ҶԻ�㷽��Ϊʼ-->��
				else if((pPrevPnt->local_point_vec==2&&tube_len_vec*line_vec>EPS_COS2)||
					(pPrevPnt->local_point_vec==1&&(-tube_len_vec)*line_vec>EPS_COS2))
					sign=-1.0;
				midLine.startPt=local_point;
				midLine.endPt=midLine.startPt+N_vec*pContext->TransToModelLength(5);
				startLine.startPt=local_point-sign*line_vec*50;
				startLine.endPt=startLine.startPt+N_vec*pContext->TransToModelLength(2.5);
				endLine.startPt=local_point+sign*line_vec*100;
				endLine.endPt=endLine.startPt+N_vec*pContext->TransToModelLength(2.5);
				coord_trans(midLine.startPt,pPlate->ucs,TRUE);
				coord_trans(midLine.endPt,pPlate->ucs,TRUE);
				coord_trans(startLine.startPt,pPlate->ucs,TRUE);
				coord_trans(startLine.endPt,pPlate->ucs,TRUE);
				coord_trans(endLine.startPt,pPlate->ucs,TRUE);
				coord_trans(endLine.endPt,pPlate->ucs,TRUE);
				midLine.startPt=dcs.TransToDraw(midLine.startPt);	//coord_trans(startPt,cs,FALSE);
				midLine.endPt=dcs.TransToDraw(midLine.endPt);	//coord_trans(startPt,cs,FALSE);
				startLine.startPt=dcs.TransToDraw(startLine.startPt);	//coord_trans(startPt,cs,FALSE);
				startLine.endPt=dcs.TransToDraw(startLine.endPt);	//coord_trans(startPt,cs,FALSE);
				endLine.startPt=dcs.TransToDraw(endLine.startPt);	//coord_trans(startPt,cs,FALSE);
				endLine.endPt=dcs.TransToDraw(endLine.endPt);	//coord_trans(startPt,cs,FALSE);
				//��ע50 100
				double fSPtx=startLine.startPt.x;
				double fMPtx=midLine.startPt.x;
				double fEPtx=endLine.startPt.x;
				CTextOnlyDrawing locate_point;
				locate_point.angle=0;
				//50 100ͳһ���ڸֹ��ڲ� wht 18-03-16
				locate_point.dimPos=midLine.startPt-(sign*pContext->TransToModelLength(sys.dim_map.fDimTextSize*0.7))*line_vec;//startLine.startPt;
				if((fSPtx<fEPtx&&fabs(fMPtx-fSPtx)<fabs(fMPtx-fEPtx))||(fSPtx>fEPtx&&fabs(fMPtx-fSPtx)>fabs(fMPtx-fEPtx)))
					locate_point.dimText.Copy("50 100");
				else
					locate_point.dimText.Copy("100 50");
				locate_point.alignType=MiddleCenter;
				locate_point.iOrder=0;	//���ӷ������ֱ�ע
				locate_point.bNeedConvertDistToText=false;
				locate_point.bInXWing=false;
				pContext->AppendTextDim(locate_point);

				startLine.ID=0;
				startLine.pen.style=PS_NULL;
				pContext->AppendLine(startLine);
				endLine.ID=0;
				endLine.pen.style=PS_NULL;
				pContext->AppendLine(endLine);
				midLine.ID=0;
				midLine.feature=-1;	//�����ײʱΪ�������ٶȿɲ���⺸����
				midLine.pen.style=PS_NULL;
				pContext->AppendLine(midLine);
			}
			}
			pPrevPnt = pVertex;
		}
	}
	CLDSLineTube *pDatumTube=NULL;
	if(pPlate->m_hPartWeldParent>0x20)
		pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
	//�������Ӱ��ϵ���˨ʱ,������˨����Ϊ�ζ�Ҫ����,��Ϊ�������
	//��˨�ķ��߱仯��Χ�Ƚϴ�
	if(h0v1==0)
	{
		CDimPlateBolts xLsDimManager;
		xLsDimManager.m_bDimRayLsSpace = style.bDimBoltSpace;
		xLsDimManager.m_bDimLsRayAngle = style.bDimRayRodAngle;
		LOCALE_PROPITEM* pLocaleItem = NULL;
		pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimRayLsSpace");
		if (pLocaleItem && !xLsDimManager.m_bDimRayLsSpace)
			xLsDimManager.m_bDimRayLsSpace = pLocaleItem->bVal;
		pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimLsRayAngle");
		if (pLocaleItem && !xLsDimManager.m_bDimLsRayAngle)
			xLsDimManager.m_bDimLsRayAngle = pLocaleItem->bVal;
		pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimSpaceToEdgeLine");
		if (pLocaleItem && !xLsDimManager.m_bDimSpaceToEdgeLine)
			xLsDimManager.m_bDimSpaceToEdgeLine = pLocaleItem->bVal;
		pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimSpaceToWeldEdge");
		if (pLocaleItem && !xLsDimManager.m_bDimSpaceToWeldEdge)
			xLsDimManager.m_bDimSpaceToWeldEdge = pLocaleItem->bVal;
		pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimFeatueLsSpace");
		if (pLocaleItem && !xLsDimManager.m_bDimFeatureLsSpace)
			xLsDimManager.m_bDimFeatureLsSpace = pLocaleItem->bVal;
		if (pDatumTube&&style.bDimRayRodAngle)	//�����ְ����߸˼�
		{	//Ϊ��ɴ���̳У�����������д�����AnalizePoleRay�������ظ����˴��Ͳ��ٷ������߽Ƕȱ�ע wjh-2019.5.15
			xLsDimManager.m_bDimLsRayAngle = false;
			AnalizePoleRay(pPlate, dcs, pContext);
		}
		xLsDimManager.CreateEx(pPlate,dcs);
		//
		for(f3dCircle* pCir=xLsDimManager.EnumFirstLsCir();pCir;pCir=xLsDimManager.EnumNextLsCir())
		{
			pContext->AppendCircle(*pCir);
			if (pEntsManager)
				pEntsManager->UpdateVerticalPlateLocateBoltPos(GEPOINT(pCir->centre.x, pCir->centre.y, 0));
		}
		for(ROTATEDIM_STRU* pDim=xLsDimManager.EnumFirstRotateDim();pDim;pDim=xLsDimManager.EnumNextRotateDim())
		{
			CSizeTextDim space_dim;
			space_dim.dimStartPos = pDim->startPos;
			space_dim.dimEndPos = pDim->endPos;
			space_dim.dimStartPos.z = space_dim.dimEndPos.z = 0;
			space_dim.dist = pDim->dist;
			space_dim.bAllowRelayout = FALSE;
			space_dim.dimPos = 0.5*(space_dim.dimStartPos + space_dim.dimEndPos);
			space_dim.angle = atan((space_dim.dimEndPos.y - space_dim.dimStartPos.y) / (space_dim.dimEndPos.x - space_dim.dimStartPos.x));
			space_dim.iOrder = 0;	//װ�����߳ߴ�
			space_dim.nHits = pDim->nHits;
			pContext->AppendSizeTextDim(space_dim);
		}
		for (ANGULARDIM_STRU* pDim = xLsDimManager.EnumFirstAngleDim(); pDim; pDim = xLsDimManager.EnumNextAngleDim())
		{
			CSizeAngleDim angle_dim;
			angle_dim.dimStartPos = pDim->startPos;
			angle_dim.dimEndPos = pDim->endPos;
			angle_dim.dimPos = pDim->dimPos;
			angle_dim.vertex = pDim->vertex;
			pContext->AppendAngularDim(angle_dim);
		}
		if (pEntsManager)
		{
			for(IDimPlateBolts::BOLTRAY* pLsRay=xLsDimManager.EnumNextBoltRay();pLsRay;pLsRay=xLsDimManager.EnumNextBoltRay())
			{
				if(pLsRay->ls_arr.GetSize()<2)
					continue;
				GELINE line;
				line.start.Set(pLsRay->ls_arr[0].location.x, pLsRay->ls_arr[0].location.y);
				line.end.Set(pLsRay->ls_arr[1].location.x, pLsRay->ls_arr[1].location.y);
				pEntsManager->UpdateVerticalPlateParallelLine(line);
			}
		}
		if(pEntsManager)	//��ע���嶨λ�ߴ� wht 16-03-29
			DimVerticalPlateLocateSize(pPlate,pContext);
	}
	else if(h0v1==1&&pDatumTube)
	{	
		f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		double dd=fabs(datum_line_vec*pPlate->ucs.axis_z);
		//������õĸֹ������������Ƶİ����䲻��ֱʱ��Ҫ��ע�����׼�ֹܵĽǶ�
		if(fabs(datum_line_vec*cs.axis_x)>EPS_COS&&dd>EPS&&dd<EPS_COS)
		{	//�����׼�ֹܲ���ֱ��ƽ�У���Ҫ��ע�Ƕ�
			f3dPoint vertex,start_vec,end_vec;
			vertex=pPlate->ucs.origin;
			start_vec=datum_line_vec;	//��ʼ�߷���
		//���¼�����ת��ĸְ�Z�᷽����Ϊ�˾��ְ��Z��Ϊ�ϸ��뵱ǰ��ͼ����ϵZ�ᴹֱ,�Ա�ʡ��"�ֹܶ�ͷ�װ��и�����ͼ" wjh-2015.10.12
			if(prjAxisZ.IsZero())
				prjAxisZ=pPlate->ucs.axis_z;
			f3dPoint norm=datum_line_vec^prjAxisZ;
			f3dPoint prj_line_vec=norm^prjAxisZ;
			normalize(prj_line_vec);
			if(prj_line_vec.x<0)
				prj_line_vec*=-1.0;
			end_vec=prj_line_vec;		//�Ƕȱ�ע��ֹ�߷���
			pContext->AppendAngularDim(vertex,start_vec,end_vec);
		}
	}
	return TRUE;
}
//��繹�ܺ����۵��������Ҫ��������
BOOL CPlateDrawing::CreateSubstationFoldPlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,
													 AcDbObjectId *pOutEntId/*=NULL*/)
{
	UCS_STRU cs=dcs.cs;
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	UCS_STRU prj_ucs;	//ͶӰ����ϵ���ڻ��ư��ͶӰͼ
	f3dPoint prjAxisZ=pPlate->ucs.axis_z;
	if(style.cbDrawPlanes&0x02)
		prjAxisZ=pPlate->GetHuoquFaceNorm(0);
	else if(style.cbDrawPlanes&0x04)
		prjAxisZ=pPlate->GetHuoquFaceNorm(1);
	if(fabs(dcs.PrjNorm()*prjAxisZ)>=0.707)
		return FALSE;	//��繹���۵���ˮƽ����ʱ����ͨ�����

	style.ciDrawModeH0V1=1;
	//�����ͶӰ����ϵ,����ԭ��λ�������ཻ��
	f3dPoint pick_pt,origin=pPlate->ucs.origin+pPlate->m_fNormOffset*pPlate->ucs.axis_z;
	f3dPoint vExtendVec=prjAxisZ^cs.axis_z;	//�ְ����չ����
	CLDSLineTube* pDatumTube=NULL;
	GEPOINT verifyExtendVec,vTubeLenVec;
	if(pPlate->m_hPartWeldParent>0x20)
	{
		pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
		if(pDatumTube)
		{
			vTubeLenVec=pDatumTube->End()-pDatumTube->Start();
			normalize(vTubeLenVec);
			verifyExtendVec=pDatumTube->GetPlateVertSpreadVec(pPlate);
		}
	}
	char ciDrawModeH0V1or2=1;	//�ֹܽ���ͼ
	if(!vTubeLenVec.IsZero()&&fabs(vTubeLenVec*dcs.PrjNorm())<0.707)
	{
		ciDrawModeH0V1or2=2;	//˫������ͶӰ
	}
	else
	{
	f3dPoint weldline_vec=pPlate->ucs.axis_z^vExtendVec;//���������ְ����ͼƽ�治�ϸ�ֱʱ���Ӹְ崹ֱ����ķ������ͶӰ
	Int3dlf(prj_ucs.origin,origin,weldline_vec,cs.origin,cs.axis_z);	//ʹͶӰԭ��λ�����潻����
	prj_ucs.axis_z=cs.axis_z;
	prj_ucs.axis_x=vExtendVec;
	prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
	normalize(prj_ucs.axis_x);
	normalize(prj_ucs.axis_y);

	CMinDouble minX;
	CMaxDouble maxZ,maxX;
	PROFILE_VERTEX* pVertex;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		int iResidePlane=pVertex->vertex.feature>10?1:pVertex->vertex.feature;
		if( (iResidePlane==1&&!(style.cbDrawPlanes&0x01))||
			(iResidePlane==2&&!(style.cbDrawPlanes&0x02))||(iResidePlane==3&&!(style.cbDrawPlanes&0x04)))
			continue;
		GEPOINT pos = pPlate->TransToActual(pVertex->vertex,iResidePlane,CLDSPlate::FROM_ACS|CLDSPlate::IS_VERTEX|CLDSPlate::TO_MCS);
		coord_trans(pos,prj_ucs,FALSE,TRUE);
		maxZ.Update(pos.z,pVertex);
		minX.Update(pos.x,pVertex);
		maxX.Update(pos.x,pVertex);
	}
	pVertex=(PROFILE_VERTEX*)maxZ.m_pRelaObj;
	double fDrawSectLength=maxX.number-minX.number;
	int i,IndexOfUpBendPlane=max(1,pVertex->vertex.feature%10-1);	//1Ϊ��һ������;2Ϊ�ڶ�������
	//�����۵�����������Ľ���ͶӰ
	f3dLine line,lineUp,lineDown;
	GEPOINT xPrevInternalStart,xPrevInternalEnd;
	double fTubeRadius=pDatumTube->GetDiameter()*0.5;
	double halfThick=pPlate->Thick*0.5;
	double fPrevRotAngle=0,fCurrRotAngle=0;
	bool prevIsFirstPlane=false;
	//������ݱ궨��
	CTextFrame data_point;	//��չ���ݵ� �������湹��������Ϣ
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	data_point.iCurLayer=PS_SOLID;	//������ڲ�
	data_point.cls_id=pPlate->GetClassTypeId();
	data_point.hPart=pPlate->handle;
	data_point.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	bool dataPointDimPosInited=false;
	for(i=1;i<pPlate->face_N;i++)
	{
		if((i==1&&!(style.cbDrawPlanes&0x02))||(i==2&&!(style.cbDrawPlanes&0x04)))
			continue;

		f3dPoint vWorkNorm=pPlate->GetHuoquFaceNorm(i-1);
		vExtendVec=vWorkNorm^cs.axis_z;
		if(vExtendVec*verifyExtendVec<0)
			vExtendVec*=-1;
		vector_trans(vExtendVec,cs,FALSE,TRUE);
		normalize(vExtendVec);
		if((style.cbDrawPlanes&0x06)==0x06)
		{
			fCurrRotAngle=Cal2dLineAng(0,0,vExtendVec.x,vExtendVec.y);
			if(i==1)
				fPrevRotAngle=fCurrRotAngle;
			if(fPrevRotAngle-fCurrRotAngle>EPS&&fPrevRotAngle-fCurrRotAngle<Pi||fCurrRotAngle-fPrevRotAngle>Pi)
			{	//1�Ż�������ת�Ƕȸ���
				xPrevInternalStart=lineDown.startPt;
				xPrevInternalEnd  =lineDown.endPt;
				prevIsFirstPlane=true;
			}
			else if(fabs(fPrevRotAngle-fCurrRotAngle)>EPS)
			{
				xPrevInternalStart=lineUp.startPt;
				xPrevInternalEnd  =lineUp.endPt;
			}
			line.pen.style=lineUp.pen.style=lineDown.pen.style=(IndexOfUpBendPlane==i?PS_SOLID:PS_DASH);//style.penStyle
		}
		else
			line.pen.style=lineUp.pen.style=lineDown.pen.style=PS_SOLID;
		f3dPoint vNormal(-vExtendVec.y,vExtendVec.x);
		lineUp.startPt=vExtendVec*fTubeRadius+vNormal*halfThick;
		lineUp.endPt  =vExtendVec*(fTubeRadius+fDrawSectLength)+vNormal*halfThick;
		pContext->AppendLine(lineUp);
		lineDown.startPt=lineUp.startPt-vNormal*pPlate->Thick;
		lineDown.endPt  =lineUp.endPt  -vNormal*pPlate->Thick;
		pContext->AppendLine(lineDown);
		line.startPt=lineUp.startPt;
		line.endPt=lineDown.startPt;
		pContext->AppendLine(line);
		line.startPt=lineUp.endPt;
		line.endPt=lineDown.endPt;
		pContext->AppendLine(line);
		if(!dataPointDimPosInited)
		{
			dataPointDimPosInited=true;
			data_point.dataPos=vExtendVec*(fTubeRadius+fDrawSectLength);
			data_point.dimPos=data_point.dataPos+vExtendVec*pPlate->Thick*2;
		}
	}
	pContext->AppendDataPoint(data_point);
	if((style.cbDrawPlanes&0x06)==0x06)
	{
		if(prevIsFirstPlane)
		{
			line.pen.style=PS_SOLID;
			line.startPt=xPrevInternalStart;
			line.endPt=lineUp.startPt;
			pContext->AppendLine(line);
			line.startPt=xPrevInternalEnd;
			line.endPt=lineUp.endPt;
			pContext->AppendLine(line);
		}
		else
		{
			line.pen.style=PS_SOLID;
			line.startPt=xPrevInternalStart;
			line.endPt=lineDown.startPt;
			pContext->AppendLine(line);
			line.startPt=xPrevInternalEnd;
			line.endPt=lineDown.endPt;
			pContext->AppendLine(line);
		}
	}
	}
	return TRUE;
}
//��繹�ܶ������ֻ����������Ҫ��������
BOOL CPlateDrawing::CreateSubstationSplayFoldPlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId/*=NULL*/)
{
	UCS_STRU cs=dcs.cs;
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	UCS_STRU prj_ucs;	//ͶӰ����ϵ���ڻ��ư��ͶӰͼ
	f3dPoint prjAxisZ=pPlate->ucs.axis_z;
	if(style.cbDrawPlanes&0x02)
		prjAxisZ=pPlate->GetHuoquFaceNorm(0);
	else if(style.cbDrawPlanes&0x04)
		prjAxisZ=pPlate->GetHuoquFaceNorm(1);
	if(fabs(dcs.PrjNorm()*prjAxisZ)>=0.707)
		return FALSE;	//��繹���۵���ˮƽ����ʱ����ͨ�����

	style.ciDrawModeH0V1=1;
	//�����ͶӰ����ϵ,����ԭ��λ�������ཻ��
	f3dPoint pick_pt,origin=pPlate->ucs.origin+pPlate->m_fNormOffset*pPlate->ucs.axis_z;
	f3dPoint vExtendVec=prjAxisZ^cs.axis_z;	//�ְ����չ����
	CLDSLineTube* pDatumTube=NULL;
	GEPOINT vTubeLenVec;
	if(pPlate->m_hPartWeldParent>0x20)
	{
		pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
		if(pDatumTube)
		{
			vTubeLenVec=pDatumTube->End()-pDatumTube->Start();
			normalize(vTubeLenVec);
			vExtendVec=pDatumTube->GetPlateVertSpreadVec(pPlate);
		}
	}
	f3dPoint weldline_vec=pPlate->ucs.axis_z^vExtendVec;//���������ְ����ͼƽ�治�ϸ�ֱʱ���Ӹְ崹ֱ����ķ������ͶӰ
	Int3dlf(prj_ucs.origin,origin,weldline_vec,cs.origin,cs.axis_z);	//ʹͶӰԭ��λ�����潻����
	prj_ucs.axis_z=cs.axis_z;
	prj_ucs.axis_x=vExtendVec;
	prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
	normalize(prj_ucs.axis_x);
	normalize(prj_ucs.axis_y);

	CMinDouble minX;
	CMaxDouble maxZ,maxX;
	PROFILE_VERTEX* pVertex;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		int iResidePlane=pVertex->vertex.feature>10?1:pVertex->vertex.feature;
		GEPOINT pos = pPlate->TransToActual(pVertex->vertex,iResidePlane,CLDSPlate::FROM_ACS|CLDSPlate::IS_VERTEX|CLDSPlate::TO_MCS);
		coord_trans(pos,prj_ucs,FALSE,TRUE);
		maxZ.Update(pos.z,pVertex);
		minX.Update(pos.x,pVertex);
		maxX.Update(pos.x,pVertex);
	}
	pVertex=(PROFILE_VERTEX*)maxZ.m_pRelaObj;
	double fDrawSectLength=maxX.number-minX.number;
	double fTubeRadius=pDatumTube->GetDiameter()*0.5;
	double halfThick=pPlate->Thick*0.5;
	bool prevIsFirstPlane=false;
	//������ݱ궨��
	CTextFrame data_point;	//��չ���ݵ� �������湹��������Ϣ
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	data_point.iCurLayer=PS_SOLID;	//������ڲ�
	data_point.cls_id=pPlate->GetClassTypeId();
	data_point.hPart=pPlate->handle;
	data_point.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	bool dataPointDimPosInited=false;

	vector_trans(vExtendVec,cs,FALSE,TRUE);
	normalize(vExtendVec);
	double fCurrRotAngle=Cal2dLineAng(0,0,vExtendVec.x,vExtendVec.y);
	//���ư��ְ����Ľ���ͶӰ
	f3dLine line,lineUp,lineDown;
	line.pen.style=lineUp.pen.style=lineDown.pen.style=PS_SOLID;
	f3dPoint vNormal(-vExtendVec.y,vExtendVec.x);
	lineUp.startPt=vExtendVec*fTubeRadius+vNormal*halfThick;
	lineUp.endPt  =vExtendVec*(fTubeRadius+fDrawSectLength)+vNormal*halfThick;
	pContext->AppendLine(lineUp);
	lineDown.startPt=lineUp.startPt-vNormal*pPlate->Thick;
	lineDown.endPt  =lineUp.endPt  -vNormal*pPlate->Thick;
	pContext->AppendLine(lineDown);
	line.startPt=lineUp.startPt;
	line.endPt=lineDown.startPt;
	pContext->AppendLine(line);
	line.startPt=lineUp.endPt;
	line.endPt=lineDown.endPt;
	pContext->AppendLine(line);

	dataPointDimPosInited=true;
	data_point.dataPos=vExtendVec*(fTubeRadius+fDrawSectLength);
	data_point.dimPos=data_point.dataPos+vExtendVec*pPlate->Thick*2;
	pContext->AppendDataPoint(data_point);
	return TRUE;
}
PART_DIMnDRAW* CSectionViewDrawing::RegisterPartDimDraw(CLDSPart *pPart,int idPartClsType,char ciDrawModeH0V1,bool bMandatoryDimLabel/*=false*/)
{
	PART_DIMnDRAW* pDimDraw=m_pPartGroupDrawing->GetPartDimDraw(m_pSectionView->GetId(),pPart->handle);
	if(pDimDraw==NULL)
		pDimDraw=m_pPartGroupDrawing->RegisterPartDimDraw(m_pSectionView->Id,pPart->handle);
	if(idPartClsType==CLS_LINESLOT)
	{	//����ע�͡��Ŷ���������Ҫ��λ��������ߣ������Ե�̫���ˡ�wjh-2016.10.9��,�ʽ��۸ֵ����ı�ע����Ϊ��������
		CLDSLineSlot* pSlotNail=(CLDSLineSlot*)pPart;
		if(pSlotNail->IsBaseSlot())
			pDimDraw->ciLeadLineMode=0;
	}
	pDimDraw->bMandatoryDimLabel=bMandatoryDimLabel;
	if(pDimDraw->ciAttachPlateType>0&&idPartClsType==CLS_PLATE)	//�����ְ�Ӧ��ͬ��������һ���������ڶ���ͼ�б�ע
	{
		CLDSPlate* pPlate=(CLDSPlate*)pPart;
		idPartClsType=CLS_PARAMPLATE;
		if(pDimDraw->ciAttachPlateType==TYPE_UEND||pDimDraw->ciAttachPlateType==TYPE_ROLLEND||pDimDraw->ciAttachPlateType==TYPE_XEND)
		{	//δָ�����Ʒ�ʽ�ķ����������壬��ǰ�Զ��жϻ��Ʒ�ʽ
			if(fabs(cs.axis_z*pPlate->ucs.axis_x)>EPS_COS2)
				ciDrawModeH0V1=2;		//ʮ�ֲ�����Ͳ�����U�Ͳ����Ҫ���ƶ���ͼ����ֱ����
		}
		else if(pDimDraw->ciAttachPlateType!=TYPE_ELBOWPLATE && fabs(cs.axis_z*pPlate->ucs.axis_x)<EPS2)
			ciDrawModeH0V1=2;		//�����Ҫ���ƶ���ͼ����ֱ����
	}
	pDimDraw->ciDrawModeH0V1=ciDrawModeH0V1;
	CXhChar50 label(pPart->GetPartNo());
	if(idPartClsType==CLS_PLATE)
	{
		label.Append(CXhChar16("#0X%X",(char*)pPart->GetPartNo(),pPart->handle));
		PART_DIMnDRAW** ppLegacyLabelDimDraw=m_pPartGroupDrawing->hashDimLabels.GetValue(label);
		PART_DIMnDRAW* pLegacyLabelDimDraw=ppLegacyLabelDimDraw!=NULL ? *ppLegacyLabelDimDraw : NULL;
		if(pLegacyLabelDimDraw)
		{
			if(pLegacyLabelDimDraw->ciDrawModeH0V1>pDimDraw->ciDrawModeH0V1&&!pLegacyLabelDimDraw->bMandatoryDimLabel)
				pLegacyLabelDimDraw->bDimPartNo=false;
			else if(!pDimDraw->bMandatoryDimLabel)//if(pLegacyLabelDimDraw->ciDrawModeH0V1<=pDimDraw->ciDrawModeH0V1)
				pDimDraw->bDimPartNo=false;
			if(pDimDraw->bDimPartNo)
				m_pPartGroupDrawing->hashDimLabels.SetValue(label,pDimDraw);
		}
		else
			m_pPartGroupDrawing->hashDimLabels.SetValue(label,pDimDraw);
	}
	else if(idPartClsType==CLS_PARAMPLATE)
	{
		PART_DIMnDRAW** ppLegacyLabelDimDraw=m_pPartGroupDrawing->hashDimLabels.GetValue(label);
		PART_DIMnDRAW* pLegacyLabelDimDraw=ppLegacyLabelDimDraw!=NULL ? *ppLegacyLabelDimDraw : NULL;
		if(pLegacyLabelDimDraw)
		{
			if(pLegacyLabelDimDraw->ciDrawModeH0V1!=2&&pDimDraw->ciDrawModeH0V1==2&&!pLegacyLabelDimDraw->bMandatoryDimLabel)
				pLegacyLabelDimDraw->bDimPartNo=false;	//�����ڵ�ǰ����ͼ�б�ע�����
			else if(pLegacyLabelDimDraw->ciDrawModeH0V1==2&&pDimDraw->ciDrawModeH0V1!=2&&!pDimDraw->bMandatoryDimLabel)
				pDimDraw->bDimPartNo=false;	//�����ڵ�ǰ����ͼ�б�ע�����
			else if(pLegacyLabelDimDraw->ciDrawModeH0V1>pDimDraw->ciDrawModeH0V1&&!pLegacyLabelDimDraw->bMandatoryDimLabel)
				pLegacyLabelDimDraw->bDimPartNo=false;
			else if((pDimDraw->ciDrawModeH0V1==1||pDimDraw->idView==pLegacyLabelDimDraw->idView)&&!pDimDraw->bMandatoryDimLabel)
				pDimDraw->bDimPartNo=false;
			if(pDimDraw->bDimPartNo)
				m_pPartGroupDrawing->hashDimLabels.SetValue(pPart->GetPartNo(),pDimDraw);
		}
		else
			m_pPartGroupDrawing->hashDimLabels.SetValue(label,pDimDraw);
	}
	return pDimDraw;
}
BOOL CSectionViewDrawing::CreatePlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,BOOL bCoplanar/*=TRUE*/,
											 BOOL bDrawWeldLineByWeldProp/*=TRUE*/,
											 BYTE geomcenter0_left1_right2/*=0*/,int h0v1/*=-1*/)
{
	CPlateDrawing drawer;
	if(!bCoplanar)
		drawer.style.penStyle=PS_DASH;
	if(m_pSectionView->m_iViewType==VIEW_FL&&m_pSectionView->IsSlaveView())
	{	//TODO:��ʱ�趨Ϊ-50~50,����Ӧ���ݵװ��뵱ǰ��ͼ����ϵԭ����ȷ�� wjh-2018.3.10
		drawer.style.siMinFilterZ=xFlSectNormalOffset.siDistToBottom-25;
		drawer.style.siMaxFilterZ=xFlSectNormalOffset.siDistToUpper+25;
	}
	drawer.style.geomcenter0_left1_right2=geomcenter0_left1_right2;
	drawer.style.bDrawWeldLine=bDrawWeldLineByWeldProp!=FALSE;
	if(m_pRootPart&&m_pRootPart->GetClassTypeId()==CLS_LINETUBE&&(m_pSectionView->IsMainView()||m_pSectionView->m_iViewType==VIEW_FRONT))
	{	//�Ըֹ�Ϊ�麸����׼����ʱ��Ҫ��������չ��ת����
		CJointTubesView* pJointView=(m_pSectionView->m_idViewType==CSectionView::CLS_JOINT_TUBES_VIEW)?(CJointTubesView*)m_pSectionView:NULL;
		if(pPlate->handle==m_pRootPart->handle)
		{
			double justify=pPlate->ucs.axis_z*dcs.cs.axis_z;
			if(fabs(justify)>EPS_COS)	//�ְ��ڵ�ǰ����ƽ����
				dcs.m_fSpreadAngle=0;
			else if(fabs(justify)<0.707)//��ֱ���ƣ�����Ҫ���Ƶĸְ壬��˲���Ҫչ��
				dcs.m_fSpreadAngle=0;
			else	//��Ҫչ�����Ƶĸְ�
			{
				dcs.m_fSpreadAngle=ACOS(fabs(justify));	//չ���Ǳ�������ǣ������ת����һ���ˣ���ֵ��Ҫͨ��չ���᷽��������
				dcs.m_xSpreadPlaneNorm=pPlate->ucs.axis_z;
				f3dPoint rot_axis_vec=dcs.m_xSpreadPlaneNorm^dcs.cs.axis_z;
				if(justify>0)	//
					rot_axis_vec/= rot_axis_vec.mod();	//��λ��
				else
					rot_axis_vec/=-rot_axis_vec.mod();
				dcs.m_xSpreadLine.startPt=dcs.cs.origin;
				dcs.m_xSpreadLine.endPt=dcs.m_xSpreadLine.startPt+rot_axis_vec;
			}
		}
		else if(pJointView&&pPlate->handle==pJointView->m_hJointTube)
		{
		}
	}
	if(m_pRootPart&&m_pRootPart->IsPlate()&&(m_iComponentType==CLS_BASIC_VIEW||m_iComponentType==CLS_ANGLE_FOOT_VIEW))
		drawer.style.bDimBoltSpace=true;
	drawer.style.bDimRayRodAngle=false;
	if(drawStyleFlag.IsHasFlag(CPartGroupDrawing::DimPlateRayLineAngle))
		drawer.style.bDimRayRodAngle=true;
	if(m_pRootPart && m_pRootPart->GetClassTypeId()==CLS_SPHERE)	//��ڵ��躸���еĵ���岻���ע������߷����Ƕ�
		drawer.style.bDimRayRodAngle=false;
	BOOL rslt=drawer.CreatePlateDrawing(pPlate,dcs,&m_xEntManager,NULL,h0v1);
	PART_DIMnDRAW* pDimDraw=RegisterPartDimDraw(pPlate,CLS_PLATE,drawer.style.ciDrawModeH0V1,drawer.style.bMandatoryDimLabel);
	if(m_pSectionView->GetViewTypeId()==CSectionView::CLS_SLAVE_NODESECT_VIEW&&drawer.style.ciDrawModeH0V1==1)
		pDimDraw->bMandatoryDimLabel=true;//�ֹ��Ͻڵ�����ͼ�еĸְ�����ͼ�����ע�ְ���
	pDimDraw->cbDrawPlanes=drawer.style.cbDrawPlanes;
	return rslt;
}

//���㻷��������������ʾ����Ľ����������������Ч��ʾ����������ʼ��ʾ��λ�Ƕ�����Ч��ʾ�����νǶ�
double IntersSectionDegAngle(double sect_start,double sect_end,double display_start,double display_end,double* valid_display_start)
{
	//����ԭʼ���ν�
	double sect_angle=sect_end-sect_start;
	while(sect_angle<0)
		sect_angle+=360;
	//����ʾ��������ʼ����ֹ��ת������ԭʼ������ʼ��Ϊ��׼��0~360�㷶Χ��
	double displaystart=display_start-sect_start;
	double displayend=display_end-sect_start;
	while(displaystart<0)
		displaystart+=360;
	while(displayend<0)
		displayend+=360;
	while(displaystart>=360)
		displaystart-=360;
	while(displayend>=360)
		displayend-=360;
	//������Ч��ʾ����
	if(displayend>displaystart)
	{
		if(displaystart>sect_angle)
			return 0;
		else
		{
			if(valid_display_start)
			{
				*valid_display_start=displaystart+sect_start;
				while(*valid_display_start<0)
					*valid_display_start+=360;
				while(*valid_display_start>=360)
					*valid_display_start-=360;
			}
			return min(displayend,sect_angle)-displaystart;
		}
	}
	else
	{
		if(displaystart>sect_angle)
		{
			if(valid_display_start)
				*valid_display_start=sect_start;
			return displayend;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The fan-shaped area overlap of this uncommon types has not be treated");
#else
			logerr.Log("����ϣ�����͵��������򽻼���δ����");
#endif
			if(valid_display_start)
				*valid_display_start=sect_start;
			return sect_angle;
		}
	}
}
BOOL CSectionViewDrawing::CreateParamPlateDrawing(CLDSParamPlate *pPlate,CDrawCoordTrans& dcs,BOOL bCoplanar/*=TRUE*/,BOOL bDrawWeldLineByWeldProp/*=TRUE*/,int h0v1/*=-1*/)
{
	bool bDrawWeldLine=bDrawWeldLineByWeldProp!=FALSE;	//��ʱ���ӱ�������Ӧ��CPlateDrawing���ɲ�������ʱ�ӵ�CPlateDrawing��,���ڸ��ݴ������������麸ͼ���ޱ�Ҫ���ƺ����߹��麸ͼ��һ�ɲ����ƺ����� wjh-2016.4.30
	f3dLine line;	//��ʱ������
	f3dCircle ls_cir;
	f3dArcLine arcline;
	CTextFrame data_point;	//��չ���ݵ� �������湹��������Ϣ
	//Ŀǰ���ָ��������,������һ���������߰�Ҳ��Ҫչ�������,Ӧ���һЩ�����������Խ���չ������ wjh-2017.7.26
	if(m_pRootPart&&m_pRootPart->GetClassTypeId()==CLS_LINETUBE&&pPlate->IsElbowRibPlate()&&
		(m_pSectionView->m_iViewType==VIEW_MAIN||m_pSectionView->m_iViewType==VIEW_FRONT))
	{	//�Ըֹ�Ϊ�麸����׼����ʱ��Ҫ��������չ��ת����
		double justify=pPlate->ucs.axis_z*dcs.cs.axis_z;
		if(fabs(justify)>EPS_COS)	//�ְ��ڵ�ǰ����ƽ����
			dcs.m_fSpreadAngle=0;
		else if(fabs(justify)<0.707)//��ֱ���ƣ�����Ҫ���Ƶĸְ壬��˲���Ҫչ��
			dcs.m_fSpreadAngle=0;
		else	//��Ҫչ�����Ƶĸְ�
		{
			dcs.m_fSpreadAngle=acos(fabs(justify));	//չ���Ǳ�������ǣ������ת����һ���ˣ���ֵ��Ҫͨ��չ���᷽��������
			dcs.m_xSpreadPlaneNorm=pPlate->ucs.axis_z;
			f3dPoint rot_axis_vec=dcs.m_xSpreadPlaneNorm^dcs.cs.axis_z;
			if(justify>0)	//
				rot_axis_vec/= rot_axis_vec.mod();	//��λ��
			else
				rot_axis_vec/=-rot_axis_vec.mod();
			dcs.m_xSpreadLine.startPt=dcs.cs.origin;
			dcs.m_xSpreadLine.endPt=dcs.m_xSpreadLine.startPt+rot_axis_vec;
		}
	}
	//��ʼ�����߿��ı�
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	f3dPoint point,start_firs,end_firs,start_sec,end_sec,start_thir,end_thir,start_four,
			end_four,start_fif,end_fif,start_six,end_six,len_vec,pre_vec;
	f3dPoint start_seven,end_seven,start_eight,end_eight;//��������߰�ľ��
	double plate_thick=pPlate->GetThick();
	PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=NULL;
	if(pPlate->vertex_list.GetNodeNum()==0)
		pPlate->DesignPlate();
	if(!pPlate->m_bStdPart&&pPlate->vertex_list.GetNodeNum()==0)
		return FALSE;	//��������û�����ζ�������
	//��Ϊһ��ѥ���ϵ���˨���߽����ڰ�����֮�⣬�����Ҳ�ϣ����˨��ע������������Χ�������Ҫ��֤ wjh-2019.8.9
	f3dPoint verticesPool[16];
	ARRAY_LIST<f3dPoint> profileVertexList(verticesPool,16,4);
	int i;//,h0v1=-1;
	if(h0v1<0)
	{
	if(pPlate->m_iParamType!=TYPE_ELBOWPLATE)
	{	//δָ�����Ʒ�ʽ�ķ����������壬��ǰ�Զ��жϻ��Ʒ�ʽ
		if((pPlate->m_iParamType==TYPE_UEND||pPlate->m_iParamType==TYPE_ROLLEND||pPlate->m_iParamType==TYPE_XEND)
			&&fabs(dcs.PrjNorm()*pPlate->ucs.axis_x)>EPS_COS2)
			h0v1=2;		//ʮ�ֲ�����Ͳ�����U�Ͳ����Ҫ���ƶ���ͼ����ֱ����
		else if(fabs(dcs.PrjNorm()*pPlate->ucs.axis_z)>0.707)
			h0v1=0;	//ƽ�̻���
		else
			h0v1=1;	//�������
	}
	else 
	{
		long hDatumPlate=0;
		double offset_angle=30*RADTODEG_COEF;	//ȡ30��Ϊ����׼�ְ���ֹ�֮��нǵļ���ֵ
		pPlate->GetDesignItemValue('L',&hDatumPlate);
		CLDSPart *pTempPlate=pPlate->BelongModel()->FromPartHandle(hDatumPlate,CLS_PLATE,CLS_PARAMPLATE);
		if(pTempPlate)
			offset_angle=ACOS(fabs(pTempPlate->ucs.axis_z*cs.axis_z));
		offset_angle=0.5*Pi-offset_angle;
		if(m_pRootPart&&!(fabs(m_pRootPart->ucs.axis_z*cs.axis_z)<EPS2))
		{	//����ͼ�Ž��д����ж�
			if(fabs(dcs.PrjNorm()*pPlate->ucs.axis_x)-fabs(cos(offset_angle))<EPS2)
				h0v1=2;		//�����Ҫ���ƶ���ͼ����ֱ����
		}
		else if(fabs(dcs.PrjNorm()*pPlate->ucs.axis_z)>EPS_COS2)
			h0v1=0;	//ƽ�̻���
		else
			h0v1=1;	//�������
	}
	}
	if(pPlate->m_iParamType==TYPE_ELBOWPLATE&&h0v1==1)	//��ʱֻ����ˮƽ���û�����ֱ���õ����	
		return FALSE;
	PART_DIMnDRAW* pDimDraw=RegisterPartDimDraw(pPlate,CLS_PARAMPLATE,h0v1);
	if(m_pSectionView->IsMainView()&&h0v1==0&&pPlate->IsInsertPlate())
		pDimDraw->bMandatoryDimLabel=true;
	if(gxDrwLocalizer.ciCustomerSerial==3)
	{	//�ൺ����Ҫ���麸ͼ��ע�����ţ������Ż����� wxc-2018.10.26
		if(pPlate->IsFL()||pPlate->m_iParamType==TYPE_CIRRIBPLATE||
			pPlate->m_iParamType==TYPE_CIRCOVERPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
			pDimDraw->bMandatoryDimLabel=true;
	}
	short penStyle=PS_SOLID;
	if(!bCoplanar)		//�ǹ���ĸ˼�ͳһ�����߱�ʾ
		penStyle=PS_DASH;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
	if(pPlate->m_iParamType==TYPE_UEND||pPlate->m_iParamType==TYPE_ROLLEND)
	{
		//U�Ͱ�
		//5________________4 
		//|��		        \3
		//|					|
		//|-.��mid_line��.��|	mark_point_arr[0]:���ԭ��mark_point_arr[1]:�������˨��
		//|                 |2	
		//0				   1/ 
		// ����������������   
		f3dPoint vertex_arr[6],mark_point_arr[2];
		f3dLine up_roll_edge,down_roll_edge;
		double L=0,N=0,W=0,S=0,H=0,M=0;
		pPlate->GetDesignItemValue('L', &L);
		pPlate->GetDesignItemValue('N', &N);
		pPlate->GetDesignItemValue('W', &W);
		pPlate->GetDesignItemValue('S', &S);
		pPlate->GetDesignItemValue('H', &H);
		pPlate->GetDesignItemValue('M', &M);
		if(h0v1==0)	//ˮƽ����
		{
			plate_thick = pPlate->thick;
			point=pPlate->ucs.origin;
			vertex_arr[0]= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_y;
			vertex_arr[1]= vertex_arr[0] + (L-N)*pPlate->ucs.axis_x;
			vertex_arr[2]= vertex_arr[0] + L*pPlate->ucs.axis_x + S*pPlate->ucs.axis_y;
			vertex_arr[3]= vertex_arr[0] + L*pPlate->ucs.axis_x + (H-S)*pPlate->ucs.axis_y;
			vertex_arr[4]= vertex_arr[0] + (L-N)*pPlate->ucs.axis_x + H*pPlate->ucs.axis_y;
			vertex_arr[5]= vertex_arr[0] + H*pPlate->ucs.axis_y;
			if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				up_roll_edge.startPt  = vertex_arr[5] - plate_thick*pPlate->ucs.axis_y;
				up_roll_edge.endPt    = vertex_arr[4] - plate_thick*pPlate->ucs.axis_y;
				down_roll_edge.startPt= vertex_arr[0] + plate_thick*pPlate->ucs.axis_y;
				down_roll_edge.endPt  = vertex_arr[1] + plate_thick*pPlate->ucs.axis_y;
			}
			//ת������ͼ����ϵ��
			point=dcs.TransToDraw(point);
			for(i=0;i<6;i++)
				vertex_arr[i]=dcs.TransToDraw(vertex_arr[i]);
			if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				up_roll_edge.startPt  =dcs.TransToDraw(up_roll_edge.startPt);
				up_roll_edge.endPt    =dcs.TransToDraw(up_roll_edge.endPt);
				down_roll_edge.startPt=dcs.TransToDraw(down_roll_edge.startPt);
				down_roll_edge.endPt  =dcs.TransToDraw(down_roll_edge.endPt);
			}
			//����U�Ͱ�
			line.startPt=vertex_arr[0];
			line.endPt=vertex_arr[1];
			line.pen.style=penStyle;
			line.ID=0;	//pPlate->handle;
			edgeList.append(LineToArc(line));
			line.startPt=vertex_arr[1];
			line.endPt=vertex_arr[2];
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=vertex_arr[2];
			line.endPt=vertex_arr[3];
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=vertex_arr[3];
			line.endPt=vertex_arr[4];
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=vertex_arr[4];
			line.endPt=vertex_arr[5];
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=vertex_arr[5];
			line.endPt=vertex_arr[0];
			line.ID=0;
			edgeList.append(LineToArc(line));

			if(pPlate->m_iParamType==TYPE_ROLLEND)
			{	//���ƾ�߰�ľ��
				if(pPlate->ucs.axis_z*dcs.PrjNorm()>0)
					line.pen.style=PS_DASH;
				else
					line.pen.style=PS_SOLID;
				line.startPt=up_roll_edge.startPt;
				line.endPt=up_roll_edge.endPt;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=down_roll_edge.startPt;
				line.endPt=down_roll_edge.endPt;
				line.ID=0;
				edgeList.append(LineToArc(line));
			}
			//������Ϣ��չ��λ��
			f3dPoint refLineVec;
			if(vertex_arr[0].y>vertex_arr[5].y)
			{
				data_point.dataPos=vertex_arr[5];
				refLineVec=vertex_arr[5]-vertex_arr[4];
			}
			else
			{
				data_point.dataPos=vertex_arr[0];
				refLineVec=vertex_arr[0]-vertex_arr[1];
			}
			normalize(refLineVec);
			data_point.dimPos=data_point.dataPos+refLineVec*pPlate->thick*3;
			DATAPOINT_TAG* pTag=AppendDataPoineTag();
			pTag->vertexArr.SetSize(0,6);
			data_point.dwKeyTag=pTag->key_tag;
			for(int i=0;i<6;++i)
				pTag->vertexArr.append(vertex_arr[i]);
		}
		else if(h0v1==1)	//���Ʋ���
		{
			plate_thick = pPlate->thick;
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
				//ת������ͼ����ϵ��
				point	  =dcs.TransToDraw(point);
				start_firs=dcs.TransToDraw(start_firs);
				end_firs  =dcs.TransToDraw(end_firs);
				start_sec =dcs.TransToDraw(start_sec);
				end_sec   =dcs.TransToDraw(end_sec);
				start_thir=dcs.TransToDraw(start_thir);
				end_thir  =dcs.TransToDraw(end_thir);
				start_four=dcs.TransToDraw(start_four);
				end_four  =dcs.TransToDraw(end_four);
				//����U�Ͱ����ͼ
				line.startPt=start_firs;
				line.endPt=end_firs;
				line.pen.style=penStyle;
				line.ID=0;	//pPlate->handle;
				edgeList.append(LineToArc(line));
				line.startPt=start_sec;
				line.endPt=end_sec;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_thir;
				line.endPt=end_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_four;
				line.endPt=end_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_firs;
				line.endPt=end_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_sec;
				line.endPt=end_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				int sign=1;
				if(start_sec.x>end_sec.x)
				{
					if(start_sec.y<start_four.y)
						sign*=-1;
				}
				else
				{
					if(start_sec.y>start_four.y)
						sign*=-1;
				}
				arcline.CreateMethod1(point,start_sec,f3dPoint(0,0,sign),Pi);
				arcline.pen.style=penStyle;
				edgeList.append(arcline);
				arcline.CreateMethod1(point,start_four,f3dPoint(0,0,sign),Pi);
				arcline.pen.style=penStyle;
				edgeList.append(arcline);
			}
			else if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				point		= pPlate->ucs.origin;
				start_firs	= pPlate->ucs.origin + 0.5*plate_thick*pPlate->ucs.axis_z;
				end_firs	= start_firs + L*pPlate->ucs.axis_x;	
				start_sec	= start_firs - (W+plate_thick)*pPlate->ucs.axis_z;
				end_sec		= start_sec + (L-N)*pPlate->ucs.axis_x;
				start_thir	= end_sec + W*pPlate->ucs.axis_z;
				end_thir	= start_thir + N*pPlate->ucs.axis_x;
				//ת������ͼ����ϵ��
				point	  =dcs.TransToDraw(point);
				start_firs=dcs.TransToDraw(start_firs);
				end_firs  =dcs.TransToDraw(end_firs);
				start_sec =dcs.TransToDraw(start_sec);
				end_sec   =dcs.TransToDraw(end_sec);
				start_thir=dcs.TransToDraw(start_thir);
				end_thir  =dcs.TransToDraw(end_thir);
				//���ƾ�߰����ͼ
				line.startPt=start_firs;
				line.endPt=end_firs;
				line.pen.style=penStyle;
				line.ID=0;	//pPlate->handle;
				edgeList.append(LineToArc(line));
				line.startPt=start_sec;
				line.endPt=end_sec;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_thir;
				line.endPt=end_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_firs;
				line.endPt=start_sec;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_sec;
				line.endPt=start_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_thir;
				line.endPt=end_firs;
				line.ID=0;
				edgeList.append(LineToArc(line));
			}
			//������Ϣ��չ��λ��
			data_point.dataPos=point+(1.5*M)*pPlate->ucs.axis_x;
			data_point.dimPos=data_point.dataPos+pPlate->ucs.axis_z*W;
			DATAPOINT_TAG* pTag=AppendDataPoineTag();
			pTag->vertexArr.SetSize(0,6);
			data_point.dwKeyTag=pTag->key_tag;
			pTag->vertexArr.append(start_firs);
			pTag->vertexArr.append(end_firs);
			pTag->vertexArr.append(start_sec);
			pTag->vertexArr.append(end_sec);
			pTag->vertexArr.append(start_thir);
			pTag->vertexArr.append(end_thir);
		}
		else if(h0v1==2)
		{	//���ƶ���
			plate_thick = pPlate->thick;
			if(pPlate->m_iParamType==TYPE_UEND)
			{
				start_firs	= pPlate->ucs.origin + 0.5*H*pPlate->ucs.axis_y + 0.5*W*pPlate->ucs.axis_z;
				end_firs	= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_y + 0.5*W*pPlate->ucs.axis_z;;
				start_sec	= pPlate->ucs.origin + 0.5*H*pPlate->ucs.axis_y + (0.5*W+pPlate->GetThick())*pPlate->ucs.axis_z;
				end_sec		= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_y + (0.5*W+pPlate->GetThick())*pPlate->ucs.axis_z;
				start_thir	= pPlate->ucs.origin + 0.5*H*pPlate->ucs.axis_y - (0.5*W+pPlate->GetThick())*pPlate->ucs.axis_z;
				end_thir	= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_y - (0.5*W+pPlate->GetThick())*pPlate->ucs.axis_z;
				start_four	= pPlate->ucs.origin + 0.5*H*pPlate->ucs.axis_y - 0.5*W*pPlate->ucs.axis_z;
				end_four	= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_y - 0.5*W*pPlate->ucs.axis_z;
				//ת������ͼ����ϵ��
				point	  =dcs.TransToDraw(point);
				start_firs=dcs.TransToDraw(start_firs);
				end_firs  =dcs.TransToDraw(end_firs);
				start_sec =dcs.TransToDraw(start_sec);
				end_sec   =dcs.TransToDraw(end_sec);
				start_thir=dcs.TransToDraw(start_thir);
				end_thir  =dcs.TransToDraw(end_thir);
				start_four=dcs.TransToDraw(start_four);
				end_four  =dcs.TransToDraw(end_four);
				//����U�Ͱ����ͼ
				line.startPt=start_firs;
				line.endPt=end_firs;
				line.pen.style=penStyle;
				line.ID=0;	//pPlate->handle;
				edgeList.append(LineToArc(line));
				line.startPt=start_sec;
				line.endPt=end_sec;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_thir;
				line.endPt=end_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_four;
				line.endPt=end_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_firs;
				line.endPt=start_sec;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_firs;
				line.endPt=end_sec;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_thir;
				line.endPt=start_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_thir;
				line.endPt=end_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				//��עU�Ͳ��Wֵ wht 10-10-29
				CSizeTextDim space_dim;
				space_dim.dimStartPos=end_firs;
				space_dim.dimEndPos=end_four;
				space_dim.dist=W;
				space_dim.bAllowRelayout=FALSE;
				space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
				space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
				space_dim.iOrder = 0;	//װ�����߳ߴ�
				space_dim.nHits=1;
				spaceDimList.append(space_dim);
				//������Ϣ��չ��λ��
				f3dPoint dim_vec=start_firs-start_four;
				normalize(dim_vec);
				data_point.dataPos=start_firs;
				data_point.dimPos=data_point.dataPos+dim_vec*W;
			}
			else if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				start_firs	= pPlate->ucs.origin + 0.5*plate_thick*pPlate->ucs.axis_z + 0.5*H*pPlate->ucs.axis_y;
				end_firs	= start_firs -  H*pPlate->ucs.axis_y;
				start_sec	= pPlate->ucs.origin - 0.5*plate_thick*pPlate->ucs.axis_z + (0.5*H-plate_thick)*pPlate->ucs.axis_y;
				end_sec		= start_sec - (H-2*plate_thick)*pPlate->ucs.axis_y;
				if(W>0)
				{
					start_thir	= start_firs - W*pPlate->ucs.axis_z;
					end_thir	= start_sec - (W-plate_thick)*pPlate->ucs.axis_z;
					start_four	= end_firs - W*pPlate->ucs.axis_z;
					end_four	= end_sec - (W-plate_thick)*pPlate->ucs.axis_z;
				}
				else 
				{
					start_thir	= start_firs - plate_thick*pPlate->ucs.axis_z;
					end_thir	= start_sec;
					start_four	= end_firs - plate_thick*pPlate->ucs.axis_z;
					end_four	= end_sec;
				}
				//ת������ͼ����ϵ��
				start_firs=dcs.TransToDraw(start_firs);
				end_firs  =dcs.TransToDraw(end_firs);
				start_sec =dcs.TransToDraw(start_sec);
				end_sec   =dcs.TransToDraw(end_sec);
				start_thir=dcs.TransToDraw(start_thir);
				end_thir  =dcs.TransToDraw(end_thir);
				start_four=dcs.TransToDraw(start_four);
				end_four  =dcs.TransToDraw(end_four);
				//���ƾ�߰����ͼ
				line.startPt=start_firs;
				line.endPt=end_firs;
				line.pen.style=penStyle;
				line.ID=0;	//pPlate->handle;
				edgeList.append(LineToArc(line));
				line.startPt=start_sec;
				line.endPt=end_sec;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_thir;
				line.endPt=end_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_four;
				line.endPt=end_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_firs;
				line.endPt=start_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_firs;
				line.endPt=start_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=start_sec;
				line.endPt=end_thir;
				line.ID=0;
				edgeList.append(LineToArc(line));
				line.startPt=end_sec;
				line.endPt=end_four;
				line.ID=0;
				edgeList.append(LineToArc(line));
				//������Ϣ��չ��λ��
				f3dPoint dim_vec=start_thir-start_firs;
				normalize(dim_vec);
				data_point.dataPos=start_thir;
				data_point.dimPos=data_point.dataPos+dim_vec*W;
				DATAPOINT_TAG* pTag=AppendDataPoineTag();
				pTag->vertexArr.SetSize(0,6);
				data_point.dwKeyTag=pTag->key_tag;
				pTag->vertexArr.append(start_firs);
				pTag->vertexArr.append(start_thir);
				pTag->vertexArr.append(end_thir);
				pTag->vertexArr.append(start_sec);
				pTag->vertexArr.append(end_sec);
				pTag->vertexArr.append(end_four);
				pTag->vertexArr.append(start_four);
				pTag->vertexArr.append(end_firs);
			}
		}
	}
	else if(pPlate->m_iParamType==TYPE_XEND)
	{
		f3dPoint vertex_arr[8];
		f3dLine up_roll_edge,down_roll_edge;
		double L=0,N=0,W=0,S=0,H=0,A=0,R=0,M=0;
		if(pPlate->m_bStdPart)
		{	//��׼����
			pPlate->GetStdPartParamValue('L', &L);
			pPlate->GetStdPartParamValue('N', &N);
			pPlate->GetStdPartParamValue('W', &W);
			pPlate->GetStdPartParamValue('S', &S);
			pPlate->GetStdPartParamValue('H', &H);
			pPlate->GetStdPartParamValue('A', &A);
			pPlate->GetStdPartParamValue('R', &R);
			pPlate->GetStdPartParamValue('M', &M);
		}
		else
		{	//�Ǳ�׼����
			pPlate->GetDesignItemValue('L', &L);
			pPlate->GetDesignItemValue('N', &N);
			pPlate->GetDesignItemValue('W', &W);
			pPlate->GetDesignItemValue('S', &S);
			pPlate->GetDesignItemValue('H', &H);
			pPlate->GetDesignItemValue('A', &A);
			pPlate->GetDesignItemValue('R', &R);
			pPlate->GetDesignItemValue('M', &M);
		}
		plate_thick = pPlate->thick;
		point=pPlate->ucs.origin;
		if(h0v1==0)	//ˮƽ����
		{
			vertex_arr[0]= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_y;
			vertex_arr[1]= vertex_arr[0] + L*pPlate->ucs.axis_x;
			vertex_arr[2]= vertex_arr[0] + L*pPlate->ucs.axis_x + H*pPlate->ucs.axis_y;
			vertex_arr[3]= vertex_arr[0] + H*pPlate->ucs.axis_y;
			vertex_arr[4]= pPlate->ucs.origin - 0.5*W*pPlate->ucs.axis_y;
			vertex_arr[5]= vertex_arr[4] + L*pPlate->ucs.axis_x;
			vertex_arr[6]= vertex_arr[4] + L*pPlate->ucs.axis_x + W*pPlate->ucs.axis_y;
			vertex_arr[7]= vertex_arr[4] + W*pPlate->ucs.axis_y;
		}
		else if(h0v1==1) //�������
		{
			vertex_arr[0]= pPlate->ucs.origin - 0.5*plate_thick*pPlate->ucs.axis_z;
			vertex_arr[1]= vertex_arr[0] + L*pPlate->ucs.axis_x;
			vertex_arr[2]= vertex_arr[0] + L*pPlate->ucs.axis_x + plate_thick*pPlate->ucs.axis_z;
			vertex_arr[3]= vertex_arr[0] + plate_thick*pPlate->ucs.axis_z;
			vertex_arr[4]= pPlate->ucs.origin - 0.5*H*pPlate->ucs.axis_z;
			vertex_arr[5]= vertex_arr[4] + L*pPlate->ucs.axis_x;
			vertex_arr[6]= vertex_arr[4] + L*pPlate->ucs.axis_x + H*pPlate->ucs.axis_z;
			vertex_arr[7]= vertex_arr[4] + H*pPlate->ucs.axis_z;
		}
		else if(h0v1==2) //����ͼ
		{
			vertex_arr[0]= pPlate->ucs.origin + 0.5*H*pPlate->ucs.axis_y + 0.5*plate_thick*pPlate->ucs.axis_z;
			vertex_arr[1]= vertex_arr[0] - H*pPlate->ucs.axis_y;
			vertex_arr[2]= vertex_arr[1] - plate_thick*pPlate->ucs.axis_z;
			vertex_arr[3]= vertex_arr[2] + H*pPlate->ucs.axis_y;
			vertex_arr[4]= pPlate->ucs.origin + 0.5*H*pPlate->ucs.axis_z + 0.5*plate_thick*pPlate->ucs.axis_y;
			vertex_arr[5]= vertex_arr[4] - H*pPlate->ucs.axis_z;
			vertex_arr[6]= vertex_arr[5] - plate_thick*pPlate->ucs.axis_y;
			vertex_arr[7]= vertex_arr[6] + H*pPlate->ucs.axis_z;
		}
		else
			return FALSE;
		//ת������ͼ����ϵ��
		point=dcs.TransToDraw(point);
		for(i=0;i<8;i++)
			vertex_arr[i]=dcs.TransToDraw(vertex_arr[i]);
		//����ʮ�ֲ��
		line.startPt=vertex_arr[0];
		line.endPt=vertex_arr[1];
		line.pen.style=penStyle;
		line.ID=0;	//pPlate->handle;
		edgeList.append(LineToArc(line));
		line.startPt=vertex_arr[1];
		line.endPt=vertex_arr[2];
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=vertex_arr[2];
		line.endPt=vertex_arr[3];
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=vertex_arr[3];
		line.endPt=vertex_arr[0];
		line.ID=0;
		edgeList.append(LineToArc(line));
		/*line.startPt=vertex_arr[4];
		line.endPt=vertex_arr[5];
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=vertex_arr[5];
		line.endPt=vertex_arr[6];
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=vertex_arr[6];
		line.endPt=vertex_arr[7];
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=vertex_arr[7];
		line.endPt=vertex_arr[4];
		line.ID=0;
		edgeList.append(LineToArc(line));*/
		//������Ϣ��չ��λ��
		f3dPoint dim_vec=vertex_arr[0]-vertex_arr[3];
		normalize(dim_vec);
		if(h0v1==2 && vertex_arr[0].y<0)
			data_point.dataPos=vertex_arr[1];
		else
			data_point.dataPos=vertex_arr[0];
		data_point.dimPos=data_point.dataPos+dim_vec*plate_thick;
		DATAPOINT_TAG* pTag=AppendDataPoineTag();
		pTag->vertexArr.SetSize(0,6);
		data_point.dwKeyTag=pTag->key_tag;
		pTag->vertexArr.append(vertex_arr[1]);
		pTag->vertexArr.append(vertex_arr[2]);
		pTag->vertexArr.append(vertex_arr[3]);
		pTag->vertexArr.append(vertex_arr[0]);
	}
	else if(pPlate->m_iParamType==TYPE_FL||pPlate->m_iParamType==TYPE_FLG||
		pPlate->m_iParamType==TYPE_FLR)
	{
		double R=0,W=0;
		pPlate->GetDesignItemValue('R', &R);
		pPlate->GetDesignItemValue('W', &W);
		if(h0v1==1)	//���Ʋ���ͼ
		{	//��������ͼ
			f3dPoint pre_vec = pPlate->ucs.axis_z;
			RotateVectorAroundVector(pre_vec,-Pi*0.5,cs.axis_z);//f3dPoint(0,1,0));
			plate_thick = pPlate->GetThick();
			start_firs	= pPlate->ucs.origin + 0.5*W*pre_vec;
			end_firs	= pPlate->ucs.origin - 0.5*W*pre_vec;
			start_sec	= start_firs + plate_thick*pPlate->ucs.axis_z;
			end_sec		= end_firs + plate_thick*pPlate->ucs.axis_z;
			//ת������ͼ����ϵ��
			start_firs=dcs.TransToDraw(start_firs);
			end_firs  =dcs.TransToDraw(end_firs);
			start_sec =dcs.TransToDraw(start_sec);
			end_sec   =dcs.TransToDraw(end_sec);
			//���Ʒ�������ͼ
			line.startPt=start_firs;
			line.endPt=end_firs;
			line.pen.style=penStyle;
			line.ID=0;	//pPlate->handle;
			edgeList.append(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=end_sec;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_firs;
			line.endPt=start_sec;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=end_firs;
			line.endPt=end_sec;
			line.ID=0;
			edgeList.append(LineToArc(line));
			//������Ϣ��չ��λ��
			f3dPoint dim_vec=start_sec-start_firs;
			normalize(dim_vec);
			f3dPoint line_vec=end_sec-start_sec;
			normalize(line_vec);
			data_point.dataPos=start_sec+line_vec*DISTANCE(start_sec,end_sec)*0.382;
			data_point.dimPos=data_point.dataPos+dim_vec*plate_thick*3;
			DATAPOINT_TAG* pTag=AppendDataPoineTag();
			pTag->vertexArr.SetSize(0,6);
			data_point.dwKeyTag=pTag->key_tag;
			pTag->vertexArr.append(start_firs);
			pTag->vertexArr.append(start_sec);
			pTag->vertexArr.append(end_sec);
			pTag->vertexArr.append(end_firs);
		}
		else if(h0v1==0)	//ˮƽ����
		{	
			point = pPlate->ucs.origin;
			start_firs	= pPlate->ucs.origin + 0.5*W*pPlate->ucs.axis_x;
			start_sec	= pPlate->ucs.origin + R*pPlate->ucs.axis_x;
			//ת������ͼ����ϵ��
			point=dcs.TransToDraw(point);
			start_firs=dcs.TransToDraw(start_firs);
			start_sec =dcs.TransToDraw(start_sec);
			//���Ʒ���ˮƽ��
			arcline.CreateMethod1(point,start_firs,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=penStyle;
			arcline.ID=0;	//pPlate->handle;
			edgeList.append(arcline);
			arcline.CreateMethod1(point,start_sec,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=penStyle;
			arcline.ID=0;
			edgeList.append(arcline);
			//��ע�����⾶����˨����ֱ���Լ���˨ֱ���͸��� wht 10-10-29
			//��ע����ֱ��ʱ����ײ����δ���...........................
			CSizeTextDim space_dim;
			/*space_dim.dimStartPos.Set(point.x+0.5*W,point.y);
			space_dim.dimEndPos.Set(point.x-0.5*W,point.y);
			space_dim.dist=W;
			space_dim.bAllowRelayout=FALSE;
			space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
			space_dim.dimPos.y-=W;
			space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
			space_dim.iOrder = 0;	//װ�����߳ߴ�
			space_dim.nHits=1;
			spaceDimList.append(space_dim);*/
			CLsRef *pLsRef=pPlate->GetFirstLsRef();
			CLDSBolt *pBolt=NULL;
			if(pLsRef)
				pBolt=pLsRef->GetLsPtr();
			double fBoltR=0;
			if(pBolt)
			{
				f3dPoint ls_pos=pBolt->ucs.origin;
				coord_trans(ls_pos,pPlate->ucs,FALSE);
				ls_pos.z=0;
				fBoltR=ls_pos.mod();
				//������˨����Բ
				arcline.CreateMethod1(point,f3dPoint(point.x+fBoltR,point.y),f3dPoint(0,0,1),2*Pi);
				arcline.pen.style=PS_DASH;
				arcline.ID=0;
				edgeList.append(arcline);
				//��˨���ڰ뾶
				/*space_dim.dimStartPos.Set(point.x+fBoltR,point.y);
				space_dim.dimEndPos.Set(point.x-fBoltR,point.y);
				space_dim.dist=2*fBoltR;
				space_dim.bAllowRelayout=FALSE;
				space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
				space_dim.dimPos.y-=(2*fBoltR);
				space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
				space_dim.iOrder = 0;	//װ�����߳ߴ�
				space_dim.nHits=1;
				spaceDimList.append(space_dim);*/
			}
			//������Ϣ��չ��λ��
			f3dPoint dim_vec=start_firs-start_sec;
			normalize(dim_vec);
			data_point.dimPos=data_point.dataPos=start_firs+dim_vec*plate_thick;
		}
	}
	else if(pPlate->m_iParamType==TYPE_FLP||pPlate->m_iParamType==TYPE_FLD)
	{	//��׼���� ƽ������/�Ժ�����
		//	1----------0----------1
		//	|		   			  |
		//	2____             ____2
		//		3\	   	     /3
		//		  |5   	   5|			
		//		  4---------4 

		CLDSStdPartModel stdPartModel;
		if(!pPlate->GetStdPartModel(&stdPartModel))
			return FALSE;
		double B=0,C=0,D=0,H=0,N=0,S=0,R=0,H1=0;
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
			S+=stdPartModel.m_fRelateTubeThick;
		if(h0v1==1)	//���Ʋ���ͼ
		{	//��������ͼ
			f3dPoint pre_vec = pPlate->ucs.axis_z;
			RotateVectorAroundVector(pre_vec,-Pi*0.5,cs.axis_z);//f3dPoint(0,1,0));
			start_firs	= pPlate->ucs.origin + 0.5*D*pre_vec;
			end_firs	= pPlate->ucs.origin - 0.5*D*pre_vec;
			start_sec	= start_firs + C*pPlate->ucs.axis_z;
			end_sec		= end_firs + C*pPlate->ucs.axis_z;
			start_thir	= start_sec - 0.5*(D-N)*pre_vec;
			end_thir	= end_sec + 0.5*(D-N)*pre_vec;
			start_four	= pPlate->ucs.origin + (0.5*B+S)*pre_vec + H*pPlate->ucs.axis_z;
			end_four	= pPlate->ucs.origin - (0.5*B+S)*pre_vec + H*pPlate->ucs.axis_z;
			double H2=H1;	//�Ժ�����
			if(pPlate->m_iParamType==TYPE_FLP)
				H2=H-(C+R);	//ƽ������
			start_fif	= start_four - H2*pPlate->ucs.axis_z;
			end_fif		= end_four - H2*pPlate->ucs.axis_z;
			//ת������ͼ����ϵ��
			start_firs =dcs.TransToDraw(start_firs);
			end_firs   =dcs.TransToDraw(end_firs);
			start_sec  =dcs.TransToDraw(start_sec);
			end_sec    =dcs.TransToDraw(end_sec);
			start_thir =dcs.TransToDraw(start_thir);
			end_thir   =dcs.TransToDraw(end_thir);
			start_four =dcs.TransToDraw(start_four);
			end_four   =dcs.TransToDraw(end_four);
			start_fif  =dcs.TransToDraw(start_fif);
			end_fif    =dcs.TransToDraw(end_fif);
			//���Ʒ�������ͼ
			line.startPt=start_firs;
			line.endPt=end_firs;
			line.pen.style=penStyle;
			line.ID=0;	//pPlate->handle;
			edgeList.append(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=start_firs;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=end_sec;
			line.endPt=end_firs;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=start_thir;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=end_sec;
			line.endPt=end_thir;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_thir;
			line.endPt=start_fif;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=end_thir;
			line.endPt=end_fif;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_fif;
			line.endPt=start_four;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=end_fif;
			line.endPt=end_four;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_four;
			line.endPt=end_four;
			line.ID=0;
			edgeList.append(LineToArc(line));
			//������Ϣ��չ��λ��
			f3dPoint dim_vec=start_firs-start_sec;
			normalize(dim_vec);
			data_point.dataPos.Set(start_firs.x,0.5*(start_firs.y+end_firs.y));
			data_point.dimPos=data_point.dataPos+dim_vec*plate_thick;
		}
		else if(h0v1==0)	//ˮƽ����
		{	
			point = pPlate->ucs.origin;
			start_firs	= pPlate->ucs.origin + 0.5*B*pPlate->ucs.axis_x;
			start_sec	= pPlate->ucs.origin + 0.5*D*pPlate->ucs.axis_x;
			//ת������ͼ����ϵ��
			point=dcs.TransToDraw(point);
			start_firs=dcs.TransToDraw(start_firs);
			start_sec =dcs.TransToDraw(start_sec);
			//���Ʒ�������ͼ
			arcline.CreateMethod1(point,start_firs,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=penStyle;
			arcline.ID=0;	//pPlate->handle;
			edgeList.append(arcline);
			arcline.CreateMethod1(point,start_sec,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=penStyle;
			arcline.ID=0;
			edgeList.append(arcline);
			//������Ϣ��չ��λ��
			f3dPoint dim_vec,pt=start_sec;
			rotate_point_around_axis(pt,0.25*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
			dim_vec=pt;
			normalize(dim_vec);
			data_point.dataPos=pt;
			data_point.dimPos=pt+dim_vec*plate_thick;
			if(pPlate->GetLsCount()==0)
			{
				CLDSStdPartModel stdPartModel;
				if(pPlate->GetStdPartModel(&stdPartModel))
				{
					stdPartModel.LayoutBolt();
					for(f3dPoint* pBoltPos=stdPartModel.GetFirstBoltPos();pBoltPos;pBoltPos=stdPartModel.GetNextBoltPos())
					{
						ls_cir.centre=*pBoltPos;
						ls_cir.radius=stdPartModel.param.fl_param.nBoltD*0.5;
						cirList.append(ls_cir);
					}
				}
			}
		}
	}
	else if(pPlate->m_iParamType==TYPE_RIBPLATE||pPlate->m_iParamType==TYPE_BASEPLATE||pPlate->m_iParamType==TYPE_SHOEPLATE||
			pPlate->m_iParamType== TYPE_LINEANGLERIBPLATE)
	{	//�����߰� wht 11-05-25
		UCS_STRU prj_ucs;
		f3dLine base_face_line;
		if(h0v1==1)
		{	//�����ͶӰ����ϵ
			prj_ucs.origin=pPlate->ucs.origin+pPlate->m_fNormOffset*pPlate->ucs.axis_z;
			prj_ucs.axis_y=pPlate->ucs.axis_z;
			prj_ucs.axis_z=cs.axis_z;
			prj_ucs.axis_x=prj_ucs.axis_y^prj_ucs.axis_z;
			prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
			normalize(prj_ucs.axis_x);
			normalize(prj_ucs.axis_y);
		}
		f3dPoint geom_center;
		int n = pPlate->vertex_list.GetNodeNum();
		for(i=0;i<n;i++)
		{
			PROFILE_VERTEX *pStartVertex=pPlate->vertex_list.GetByIndex(i);
			PROFILE_VERTEX *pEndVertex=pPlate->vertex_list.GetByIndex((i+1)%n);
			f3dPoint f_start = pStartVertex->vertex;
			f3dPoint f_end   = pEndVertex->vertex;
			coord_trans(f_start,pPlate->ucs,TRUE);
			coord_trans(f_end,pPlate->ucs,TRUE);
			geom_center+=f_start;
			if(h0v1==0)
			{	//ˮƽ����
				f_start=dcs.TransToDraw(f_start);
				f_end=dcs.TransToDraw(f_end);
				profileVertexList.append(f_start);
				if(pStartVertex->type==0)
				{
					line.startPt=f_start;
					line.endPt=f_end;
					line.pen.style=penStyle;
					line.ID=0;
					edgeList.append(LineToArc(line));
					if(bDrawWeldLine&&pStartVertex->m_bWeldEdge)
					{
						if(pPlate->ucs.axis_z*dcs.PrjNorm()>0)
							DrawWeldLineSymbol(f_start,f_end);
						else 
							DrawWeldLineSymbol(f_end,f_start);
					}
				}
				else if(pStartVertex->type<3)	//Բ��
				{
					PROFILE_VERTEX arcStart=*pStartVertex;
					arcStart.vertex=f_start;
					arcStart.center=pStartVertex->center;
					coord_trans(arcStart.center,pPlate->ucs,TRUE);
					arcStart.center=dcs.TransToDraw(arcStart.center);
					int sign=1;
					if(dcs.PrjNorm()*pPlate->ucs.axis_z<0)	//ͶӰ������巨�߷�����
						sign=-1;
					if(dcs.PrjNorm()*cs.axis_z<0)
						sign*=-1;
					if(sign<0)
					{
						if(arcStart.type==1)
							arcStart.sector_angle*=-1.0;
						else
							arcStart.radius*=-1.0;
					}
					arcStart.RetrieveArcLine(arcline,f_end,NULL);
					arcline.ID=0;
					arcline.pen.style=penStyle;
					edgeList.append(arcline);
				}
				else if(pStartVertex->type==3)	//��Բ��
				{
					PROFILE_VERTEX arcStart=*pStartVertex;
					arcStart.vertex=f_start;
					if(dcs.PrjNorm()*pPlate->ucs.axis_z<0)	//ͶӰ������巨�߷�����
						arcStart.radius*=-1.0;
					arcStart.RetrieveArcLine(arcline,f_end,&cs);//Ӧת������ͼ����ϵ��,�Ǹְ�����ϵ wht 10-08-12
					arcline.ID=0;
					arcline.pen.style=penStyle;
					edgeList.append(arcline);
				}
			}
			else
			{	//��ֱ����
				f3dPoint start=f_start;
				f3dPoint end=f_end;
				double zCoordVerify=(start-cs.origin)*cs.axis_z;
				if( m_pSectionView->m_iViewType==VIEW_FL&&(
					zCoordVerify<xFlSectNormalOffset.siMinFilterZ||zCoordVerify>xFlSectNormalOffset.siMaxFilterZ))
					continue;	//�ݵװ��뵱ǰ��ͼ����ϵԭ����ȷ�������Z���趨��Χ,������Χ�ĸְ��������㣬������ͶӰ����Ҫ���ڴ���׽Ŵ���忿�ӱ���бӰ��ͶӰ���ȣ�wjh-2018.3.12
				if(start.z>0)
					start-=xFlSectNormalOffset.vxTubeStdUpperAxisVec*xFlSectNormalOffset.siMaxFilterZ;
				else if(start.z<0)
					start-=xFlSectNormalOffset.vxTubeStdUpperAxisVec*xFlSectNormalOffset.siMinFilterZ;
				coord_trans(start,prj_ucs,FALSE);
				start.y=start.z=0;
				if(base_face_line.startPt==base_face_line.endPt)	//ȱʡδ��ֵ
				{
					base_face_line.startPt=start;
					coord_trans(end,prj_ucs,FALSE);
					end.y=end.z=0;
					base_face_line.endPt = end;
				}
				else
				{
					int judge=base_face_line.PtInLine(start);
					if(judge==-1)		//�յ����
						base_face_line.endPt=start;
					else if(judge==-2)	//������
						base_face_line.startPt=start;
				}
			}
		}
		//����ְ弸������
		geom_center/=pPlate->vertex_list.GetNodeNum();
		project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
		geom_center=dcs.TransToDraw(geom_center);
		if(h0v1==0) //ˮƽ����---���������Ϣ��չ��
			data_point.dataPos=data_point.dimPos=geom_center; 
		else if(h0v1==1)
		{	//��ֱ����
			f3dPoint base_vec,vec;	
			coord_trans(base_face_line.startPt,prj_ucs,TRUE);
			coord_trans(base_face_line.endPt,prj_ucs,TRUE);
			base_face_line.startPt=dcs.TransToDraw(base_face_line.startPt);
			base_face_line.endPt  =dcs.TransToDraw(base_face_line.endPt);
			//ƫ��ʸ���͵Ⱦ�ƫ����
			base_vec=prj_ucs.axis_y;
			vector_trans(base_vec,cs,FALSE);
			normalize(base_vec);
			f3dLine offset_line;
			if(pPlate->m_iParamType==TYPE_RIBPLATE)
			{
				base_face_line.startPt=base_face_line.startPt-base_vec*0.5*plate_thick;
				base_face_line.endPt=base_face_line.endPt-base_vec*0.5*plate_thick;
			}
			offset_line.startPt=base_face_line.startPt+base_vec*plate_thick;
			offset_line.endPt=base_face_line.endPt+base_vec*plate_thick;
			short pen_style=PS_SOLID;
			PROFILE_VERTEX *pVertex=NULL;
			f3dPoint center_pos=geom_center;	//�ְ弸������
			coord_trans(center_pos,pPlate->ucs,TRUE);
			f3dPoint extended_vec=center_pos-pPlate->ucs.origin;
			normalize(extended_vec);
			if(m_pRootPart&&m_pRootPart->GetClassTypeId()==CLS_LINETUBE)
			{	//����ͼ�������ĸְ������߱�ʾ
				if(extended_vec*cs.axis_z<0&&fabs(cs.axis_z*m_pRootPart->ucs.axis_z)<EPS2)
					pen_style=PS_DASH;
			}
			line.startPt=base_face_line.startPt;
			line.endPt=base_face_line.endPt;
			line.ID=0;
			line.pen.style=pen_style;
			edgeList.append(LineToArc(line));
			CSectionDrawingEntsManager::VERTICAL_PLATE *pVertPlate=m_xEntManager.m_hashVertPlateList.GetValue(pPlate->handle);
			if(pVertPlate==NULL)
			{
				pVertPlate=m_xEntManager.m_hashVertPlateList.Add(pPlate->handle);
				pVertPlate->pPlate=pPlate;
			}
			pVertPlate->face_line[0][0]=base_face_line;
			pVertPlate->face_line[0][1]=offset_line;
			//����ƫ��ʸ���͵Ⱦ�ƫ����
			line.startPt=offset_line.startPt;
			line.endPt=offset_line.endPt;
			line.ID=0;
			line.pen.style=pen_style;
			edgeList.append(LineToArc(line));
			//���ƺ����
			line.startPt=base_face_line.startPt;
			line.endPt=offset_line.startPt;
			line.ID=0;
			line.pen.style=pen_style;
			edgeList.append(LineToArc(line));
			//
			line.startPt=offset_line.endPt;
			line.endPt=base_face_line.endPt;
			line.ID=0;
			line.pen.style=pen_style;
			edgeList.append(LineToArc(line));
			//������ݱ궨��
			f3dPoint dim_vec=base_face_line.endPt-base_face_line.startPt;
			normalize(dim_vec);
			data_point.dataPos=offset_line.startPt+0.382*dim_vec*DISTANCE(offset_line.startPt,offset_line.endPt);
			data_point.dimPos=data_point.dataPos+base_vec*plate_thick*2;
		}
	}
	else if(pPlate->m_iParamType==TYPE_ELBOWPLATE)
	{
		double dd=fabs(pPlate->ucs.axis_z*dcs.PrjNorm());
		if(h0v1==0)
		{	//ƽ�̷���
			f3dPoint geom_center;
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlate->ucs,TRUE);
				geom_center+=vertex;
				if(pPrevVertex==NULL)
				{
					pPrevVertex=pVertex;
					line.startPt=pPrevVertex->vertex;
					coord_trans(line.startPt,pPlate->ucs,TRUE);
					line.startPt=dcs.TransToDraw(line.startPt);
					continue;
				}
				else
				{
					line.endPt=pVertex->vertex;
					coord_trans(line.endPt,pPlate->ucs,TRUE);
					line.endPt=dcs.TransToDraw(line.endPt);
					line.pen.style=PS_SOLID;
					line.ID=0;
					edgeList.append(LineToArc(line));
					line.startPt=line.endPt;
					pPrevVertex=pVertex;
				}
			}
			if(pPrevVertex)
			{	//���һ��ֱ�߱�
				pVertex=pPlate->vertex_list.GetFirst();
				line.endPt=pVertex->vertex;
				coord_trans(line.endPt,pPlate->ucs,TRUE);
				line.endPt=dcs.TransToDraw(line.endPt);
				line.pen.style=PS_SOLID;
				line.ID=0;	//pPlate->handle;
				edgeList.append(LineToArc(line));
			}
			//����ְ弸������
			geom_center/=pPlate->vertex_list.GetNodeNum();
			project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
			geom_center=dcs.TransToDraw(geom_center);
			//���������Ϣ��չ��
			data_point.dataPos=data_point.dimPos=geom_center;
		}
		else if(h0v1==1)
		{	//��崹ֱ����ʱ��װ������ϵX�����ͼ������ͶӰ
			double A=0;
			pPlate->GetDesignItemValue('A', &A);	//�߰��ڲ೤��
			line.startPt=pPlate->ucs.origin;
			line.endPt=line.startPt+A*pPlate->ucs.axis_y;
			f3dPoint prj_vec=pPlate->ucs.axis_x;
			vector_trans(prj_vec,cs,FALSE);
			line.startPt=dcs.TransToDraw(line.startPt);
			line.endPt=dcs.TransToDraw(line.endPt);
			normalize(prj_vec);
			line.startPt-=(line.startPt.z/prj_vec.z)*prj_vec;
			line.endPt-=(line.endPt.z/prj_vec.z)*prj_vec;
			f3dPoint norm_vec=pPlate->ucs.axis_z;
			norm_vec=dcs.TransVToDraw(norm_vec);
			normalize(norm_vec);
			double draw_thick=pPlate->thick;
			start_firs=line.startPt+norm_vec*(0.5*draw_thick);
			end_firs=line.endPt+norm_vec*(0.5*draw_thick);
			start_sec=start_firs-norm_vec*draw_thick;
			end_sec=end_firs-norm_vec*draw_thick;
			start_thir=start_sec;
			end_thir=start_sec+norm_vec*draw_thick;
			start_four=end_sec;
			end_four=end_sec+norm_vec*draw_thick;
			
			//�����߰�
			line.startPt=start_firs;
			line.endPt=end_firs;
			if(pPlate->ucs.axis_x*dcs.PrjNorm()<0)
			{
				line.pen.style=PS_DASH;		//������ʾ
				data_point.iCurLayer=PS_DASH;
			}
			else
				line.pen.style=PS_SOLID;	//������ʾ
			line.ID=0;	//pPlate->handle;
			edgeList.append(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=end_sec;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_thir;
			line.endPt=end_thir;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_four;
			line.endPt=end_four;
			line.ID=0;
			edgeList.append(LineToArc(line));
			//������Ϣ��չ��λ��
			data_point.dimPos=data_point.dataPos=0.5*(start_firs+end_firs)+norm_vec*plate_thick;
		}
		else if(h0v1==2)
		{	//���ƶ���
			double B=0;
			pPlate->GetDesignItemValue('B', &B);	//�߰�ײ����
			double draw_thick=pPlate->thick;
			start_firs=pPlate->ucs.origin+pPlate->ucs.axis_z*0.5*draw_thick;
			end_firs=start_firs+pPlate->ucs.axis_x*B;
			start_sec=pPlate->ucs.origin-pPlate->ucs.axis_z*0.5*draw_thick;
			end_sec=start_sec+pPlate->ucs.axis_x*B;
			start_firs=dcs.TransToDraw(start_firs);
			end_firs  =dcs.TransToDraw(end_firs);
			start_sec =dcs.TransToDraw(start_sec);
			end_sec   =dcs.TransToDraw(end_sec);
			//�����߰�
			line.startPt=start_firs;
			line.endPt=end_firs;
			line.pen.style=PS_SOLID;
			line.ID=0;	//pPlate->handle;
			edgeList.append(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=end_sec;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=start_firs;
			line.endPt=start_sec;
			line.ID=0;
			edgeList.append(LineToArc(line));
			line.startPt=end_firs;
			line.endPt=end_sec;
			line.ID=0;
			edgeList.append(LineToArc(line));
			//���������Ϣ��չ��
			f3dPoint dim_vec=end_firs-start_firs;
			normalize(dim_vec);
			data_point.dataPos=0.5*(start_firs+start_sec)+dim_vec*0.8*B;
			data_point.dimPos=data_point.dataPos+dim_vec*B*0.8;
			DATAPOINT_TAG* pTag=AppendDataPoineTag();
			pTag->vertexArr.SetSize(0,6);
			data_point.dwKeyTag=pTag->key_tag;
			pTag->vertexArr.append(start_firs);
			pTag->vertexArr.append(start_sec);
			pTag->vertexArr.append(end_sec);
			pTag->vertexArr.append(end_firs);
		}
	}
	else if(pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_CIRCOVERPLATE||
			pPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	//�����߰�ͷ��
		BOOL bDimHandle=FALSE;
		if(h0v1==0)
		{	//ƽ�̷���
			pPrevVertex=NULL;
			int sign=1,initSign=1;
			if(pPlate->ucs.axis_z*dcs.PrjNorm()>0)
				initSign=1;
			else
				initSign=-1;
			f3dPoint geom_center;
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlate->ucs,TRUE);
				geom_center+=vertex;
			}
			for(pVertex=pPlate->vertex_list.GetFirst();;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pPrevVertex==NULL)
				{
					pPrevVertex=pVertex;
					line.startPt=pVertex->vertex;
					coord_trans(line.startPt,pPlate->ucs,TRUE);
					line.startPt=dcs.TransToDraw(line.startPt);
					continue;
				}
				else
				{
					sign=initSign;
					BOOL bFinalEdge=FALSE;
					f3dArcLine arcLine;
					if(pVertex==NULL)
					{
						pVertex=pPlate->vertex_list.GetFirst();
						bFinalEdge=TRUE;
					}
					line.endPt=pVertex->vertex;
					coord_trans(line.endPt,pPlate->ucs,TRUE);
					line.endPt=dcs.TransToDraw(line.endPt);
					if(pPrevVertex->type<=0)
					{
						line.ID=0;
						line.pen.style=PS_SOLID;
						edgeList.append(LineToArc(line));
					}
					else
					{
						f3dPoint worknorm(0,0,sign);
						double fPara;
						if(pPrevVertex->type==1)	//ָ��Բ���н�
						{
							if(pPrevVertex->sector_angle<0)
							{
								fPara=-pPrevVertex->sector_angle;
								worknorm*=-1;
							}
							else
								fPara=pPrevVertex->sector_angle;
							if(!line.startPt.IsEqual(line.endPt))
								arcLine.CreateMethod2(line.startPt,line.endPt,worknorm,fPara);
							else
								logerr.Log("0X%X��Բ����ʼ������ֹ���غ�!",pPlate->handle);
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
							f3dPoint center=pPrevVertex->center;
							//���Ƚ�Բ��ʰȡ��ת������������ϵ�£�Ȼ����ת������ͼ����ϵ�� wht 12-06-14
							coord_trans(center,pPlate->ucs,TRUE);
							center=dcs.TransToDraw(center);
							if(!line.startPt.IsEqual(line.endPt))
								arcLine.CreateMethod3(line.startPt,line.endPt,worknorm,fPara,center);
							else
								logerr.Log("0X%X��Բ����ʼ������ֹ���غ�!",pPlate->handle);
							//������CreateMethod2,����ʧ��ʰȡ����Ϣ�����ڴ���180��Ļ��Ǵ�ġ� wht 12-06-14
							//arcLine.CreateMethod2(line.startPt,line.endPt,worknorm,sec_angle);
						}
						else //if(pPrevVertex->type==3)
						{	//��Բ��
							double radius=fabs(pPrevVertex->radius);
							if(pPrevVertex->radius<0)
								sign=-1;
							f3dPoint columnnorm=pPrevVertex->column_norm;
							f3dPoint center=pPrevVertex->center;
							if(pPrevVertex->vertex.feature%10==2&&pVertex->vertex.feature%10==2)
							{	//�������ϵ���Բ��������Ӧ��Բ�ı궨�㼰���淨����תչ�����������ٽ�������ϵת�� wht 11-05-31
								coord_trans(center,pPlate->ucs,FALSE);
								vector_trans(columnnorm,pPlate->ucs,FALSE);
								center=pPlate->SpreadTransV(center);
								columnnorm=pPlate->SpreadTransV(columnnorm,false);
								coord_trans(center,pPlate->ucs,TRUE);
								vector_trans(columnnorm,pPlate->ucs,TRUE);
							}
							center=dcs.TransToDraw(center);
							vector_trans(columnnorm,cs,FALSE);
							if(!line.startPt.IsEqual(line.endPt))
								arcLine.CreateEllipse(center,line.startPt,line.endPt,columnnorm,f3dPoint(0,0,sign),radius);
							else
								logerr.Log("0X%X��Բ����ʼ������ֹ���غ�!",pPlate->handle);
						}
						arcLine.ID=0;
						arcLine.pen.style=PS_SOLID;
						edgeList.append(arcLine);
					}
					if(bDrawWeldLine&&pPrevVertex->m_bWeldEdge)
					{
						if(pPrevVertex->type==0)
						{
							if(sign>0)
								DrawWeldLineSymbol(line.startPt,line.endPt);
							else
								DrawWeldLineSymbol(line.endPt,line.startPt);
						}
						else
							DrawWeldLineSymbol(arcLine);
					}
					line.startPt=line.endPt;
					pPrevVertex=pVertex;
					if(bFinalEdge)
						break;
				}
			}
			//���ڻ����ʹ�øְ�ԭ����Ϊ�����ķ���ȼ������ĵ�����ʡ�wjh-2016.10.18
			geom_center=pPlate->ucs.origin;
			geom_center=dcs.TransToDraw(geom_center);
			//���������Ϣ��չ��,��Բ����ʼ����Ϊ��ע��
			double fMaxDist=0;
			f3dPoint data_pos,dim_vec;
			pPrevVertex=pPlate->vertex_list.GetTail();
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=pPlate->vertex_list.GetNext())
			{
				if(pPrevVertex->m_bWeldEdge)
					continue;	//�����ܼӺ������
				GEPOINT vertex;
				if(pPrevVertex->type==0)
					vertex=pPrevVertex->vertex+(pVertex->vertex-pPrevVertex->vertex)*0.618;	//�ƽ�ָ����=0.618
				else
				{
					f3dArcLine arcline;
					pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&pPlate->ucs);
					double angle=arcline.SectorAngle();
					vertex=arcline.PositionInAngle(angle*0.618);	//�ƽ�ָ����=0.618
				}
				coord_trans(vertex,pPlate->ucs,TRUE);
				vertex=dcs.TransToDraw(vertex);
				double fDist=DISTANCE(vertex,geom_center);
				if(fDist>fMaxDist)
				{
					fMaxDist=fDist;
					data_pos=vertex;
				}
			}
			dim_vec=data_pos-geom_center;
			normalize(dim_vec);
			data_point.dataPos=data_pos;
			data_point.dimPos=data_pos+dim_vec*pPlate->GetThick();
		}
		else
		{	//��ֱ����
			double D=0,W=0;
			pPlate->GetDesignItemValue('D', &D);
			pPlate->GetDesignItemValue('W', &W);
			CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent);
			BOOL bIsZhiWan=pPlate->IsZhiWan();
			if(bIsZhiWan&&pPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{	//����Ŀ�Խ����
				f3dPoint huoqu_norm=pPlate->HuoquFaceNorm();
				f3dPoint huoqu_axis_vec=pPlate->HuoquAxisVec();
				CIRCULAR_PLATE_VERTEXSET vertexSet=pPlate->GetCirPlateVertexSet();
				if(fabs(dcs.PrjNorm()*huoqu_axis_vec)>0.707)
				{	//���ƻ����߷���Ľ��� �����ؼ���(������,W,N) wht 11-05-31
					f3dPoint vertex_arr[6];
					vertex_arr[0]=vertexSet.weld_outer_w;
					vertex_arr[1]=vertexSet.weld_outer_huoqu;
					vertex_arr[2]=vertexSet.weld_outer_n;
					f3dPoint axis_z1=pPlate->ucs.axis_z,axis_z2=huoqu_norm;
					vector_trans(axis_z1,cs,FALSE);
					vector_trans(axis_z2,cs,FALSE);
					for(i=0;i<3;i++)
					{
						f3dPoint axis_z=axis_z1;
						f3dPoint vertex=vertex_arr[i];
						if(i==2)
						{
							vertex=pPlate->GetRealVertex(vertex);
							axis_z=axis_z2;
						}
						vertex=dcs.TransToDraw(vertex);
						if(i==1)
						{	//������
							axis_z=axis_z1+axis_z2;
							normalize(axis_z);
							double cosa=axis_z1*axis_z;
							vertex_arr[i]=vertex+axis_z*plate_thick*cosa*0.5;
							vertex_arr[i+3]=vertex-axis_z*plate_thick*cosa*0.5;
						}
						else 
						{
							vertex_arr[i]=vertex+axis_z*plate_thick*0.5;
							vertex_arr[i+3]=vertex-axis_z*plate_thick*0.5;
						}
					}
					if(pPlate->ucs.axis_y*dcs.PrjNorm()>0)
						line.pen.style=PS_SOLID;
					else 
						line.pen.style=PS_DASH;	//���ز�
					
					line.startPt=vertex_arr[0];
					line.endPt=vertex_arr[1];
					edgeList.append(LineToArc(line));
					line.startPt=vertex_arr[1];
					line.endPt=vertex_arr[2];
					edgeList.append(LineToArc(line));
					line.startPt=vertex_arr[3];
					line.endPt=vertex_arr[4];
					edgeList.append(LineToArc(line));
					line.startPt=vertex_arr[4];
					line.endPt=vertex_arr[5];
					edgeList.append(LineToArc(line));
					line.startPt=vertex_arr[0];
					line.endPt=vertex_arr[3];
					edgeList.append(LineToArc(line));
					line.startPt=vertex_arr[2];
					line.endPt=vertex_arr[5];
					edgeList.append(LineToArc(line));
					//������Ϣ��չ��λ��
					data_point.iCurLayer=line.pen.style;
					data_point.dataPos=vertex_arr[1];
					data_point.dimPos=data_point.dataPos+axis_z1*plate_thick;
				}
				else 
				{	//���ƶ���(N��W���������յ�)
					f3dPoint axis_z=pPlate->ucs.axis_z;
					line.startPt=vertexSet.weld_outer_huoqu;
					if(dcs.PrjNorm()*pPlate->ucs.axis_x>0)
						line.endPt=vertexSet.weld_outer_w;
					else 
					{
						line.endPt=vertexSet.weld_outer_n;
						axis_z=pPlate->HuoquFaceNorm();
						line.endPt=pPlate->GetRealVertex(line.endPt);
					}
					axis_z=dcs.TransVToDraw(axis_z);
					line.startPt=dcs.TransToDraw(line.startPt);
					line.endPt=dcs.TransToDraw(line.endPt);
					//
					f3dLine offse_line;
					offse_line.startPt=line.startPt+axis_z*plate_thick;
					offse_line.endPt=line.endPt+axis_z*plate_thick;
					//
					edgeList.append(LineToArc(line));
					edgeList.append(LineToArc(offse_line));
					edgeList.append(LineToArc(f3dLine(line.startPt,offse_line.startPt)));
					edgeList.append(LineToArc(f3dLine(line.endPt,offse_line.endPt)));
					//������Ϣ��չ��λ��
					data_point.iCurLayer=line.pen.style;
					data_point.dataPos=0.5*(line.startPt+line.endPt);
					data_point.dimPos=data_point.dataPos-axis_z*plate_thick;
				}
			}
			else if(pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{
				f3dPoint perp,vec,norm_vec,mark_line_vec=dcs.PrjNorm()^pPlate->ucs.axis_z;
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
					norm_vec=dcs.TransVToDraw(norm_vec);
					normalize(norm_vec);
					line.startPt=pPlate->ucs.origin+mark_line_vec*min_pos_len;
					line.endPt=pPlate->ucs.origin+mark_line_vec*max_pos_len;
					line.startPt=dcs.TransToDraw(line.startPt);
					line.endPt  =dcs.TransToDraw(line.endPt);
					f3dLine line_arr[4];
					double draw_thick=pPlate->thick;
					start_firs=line.startPt+norm_vec*(0.5*draw_thick);
					end_firs=line.endPt+norm_vec*(0.5*draw_thick);
					start_sec=start_firs-norm_vec*draw_thick;
					end_sec=end_firs-norm_vec*draw_thick;
					start_thir=start_sec;
					end_thir=start_sec+norm_vec*draw_thick;
					start_four=end_sec;
					end_four=end_sec+norm_vec*draw_thick;
					line_arr[0].startPt=start_firs;
					line_arr[0].endPt=end_firs;
					line_arr[1].startPt=start_sec;
					line_arr[1].endPt=end_sec;
					line_arr[2].startPt=start_thir;
					line_arr[2].endPt=end_thir;
					line_arr[3].startPt=start_four;
					line_arr[3].endPt=end_four;
					//���ְ�ľ����Ϣ�����������������
					int i,max_len_index=0;
					double max_edge_len=-10000;
					for(i=0;i<4;i++)
					{
						double cur_edge_len=DISTANCE(line_arr[i].startPt,line_arr[i].endPt);
						if(cur_edge_len>max_edge_len)
						{
							max_edge_len=cur_edge_len;
							max_len_index=i;
						}
					}
					for(i=0;i<4;i++)
					{
						//TODO:Ī������,��֪��������ǰΪʲô�ѻ����߰���Ϊ��������״̬ wjh-2017.7.26
						//if(pPlate->m_iParamType==TYPE_CIRRIBPLATE)
						//	line_arr[i].pen.style=PS_DASH;
						//else
							line_arr[i].pen.style=PS_SOLID;
						line_arr[i].ID=0;
						edgeList.append(LineToArc(line_arr[i]));
						if(bDrawWeldLine&&i==3)
							DrawWeldLineSymbol(line_arr[i].startPt,line_arr[i].endPt);
					}
					if(pPlate->GetLsCount()>2)
					{	//�����߰�������˨���ڲ���ͼ�в������¸�����������˨ͼ��
						CLDSBolt *pBolt=pPlate->GetFirstLsRef()->GetLsPtr();
						f3dPoint ls_pos=pBolt->ucs.origin;
						coord_trans(ls_pos,pPlate->ucs,FALSE);
						ls_pos.z=0;
						double fBoltR=ls_pos.mod();
						f3dPoint top_pt=pPlate->ucs.origin+mark_line_vec*fBoltR;
						f3dPoint btm_pt=pPlate->ucs.origin-mark_line_vec*fBoltR;
						top_pt=dcs.TransToDraw(top_pt);
						btm_pt=dcs.TransToDraw(btm_pt);
						//
						line.pen.style=PS_DASH;
						line.ID=0;
						line.startPt=top_pt+norm_vec*(0.5*draw_thick);
						line.endPt  =top_pt-norm_vec*(0.5*draw_thick);
						edgeList.append(LineToArc(line));
						line.startPt=btm_pt+norm_vec*(0.5*draw_thick);
						line.endPt  =btm_pt-norm_vec*(0.5*draw_thick);
						edgeList.append(LineToArc(line));
					}
					//������Ϣ��չ��λ��
					data_point.iCurLayer=line_arr[i].pen.style;
					data_point.dataPos=0.5*(line_arr[0].startPt+line_arr[0].endPt);
					data_point.dimPos=data_point.dataPos+norm_vec*plate_thick;
					DATAPOINT_TAG* pTag=AppendDataPoineTag();
					data_point.dwKeyTag=pTag->key_tag;
					pTag->vertexArr.append(start_firs);
					pTag->vertexArr.append(end_firs);
					pTag->vertexArr.append(end_sec);
					pTag->vertexArr.append(end_thir);
				}
			}
			else if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			{
				double A=0,B=0,S=0;
				pPlate->GetDesignItemValue('A', &A);	//������ת�Ƕȡ�
				pPlate->GetDesignItemValue('B', &B);	//���μнǡ�
				pPlate->GetDesignItemValue('S', &S);	//��ֵ��ʾ�Ϸ�壬��ֵ��ʾ�·��
				double max_pos_len=0,min_pos_len=0;
				f3dPoint norm_vec=cs.axis_z;
				vector_trans(norm_vec,pPlate->ucs,FALSE);
				double norm_angle_pos=Cal2dLineAng(0,0,norm_vec.x,norm_vec.y)*DEGTORAD_COEF;
				double display_angle_start=norm_angle_pos-90;
				double display_angle_end=norm_angle_pos+90;
				//����ƻ���������ϵʱ�ѿ�������ת��A
				double section_angle_start=-B/2.0;		//A-B/2.0;
				double section_angle_end=B/2.0;			//A+B/2.0;
				double displaysectangle=IntersSectionDegAngle(section_angle_start,section_angle_end,display_angle_start,display_angle_end,&display_angle_start);
				display_angle_end=display_angle_start+displaysectangle;
				f3dLine prj_line;
				
				//��������ȫ�ڱ���ʱҲ��Ҫ����ʾ��ͼ,�����޷���ע�������
				//��ʱ���������ߵ���Ϊ���ز�,������ʾ������׼ȷ����ʱ��˴���  wht 10-08-06
				if(fabs(display_angle_end-display_angle_start)>EPS)
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
					coord_trans(prj_line2.startPt,pPlate->ucs,TRUE);
					coord_trans(prj_line2.endPt,pPlate->ucs,TRUE);
					prj_line.startPt =dcs.TransToDraw(prj_line.startPt);
					prj_line.endPt   =dcs.TransToDraw(prj_line.endPt);
					prj_line2.startPt=dcs.TransToDraw(prj_line2.startPt);
					prj_line2.endPt  =dcs.TransToDraw(prj_line2.endPt);
					norm_vec=pPlate->ucs.axis_z;
					norm_vec=dcs.TransVToDraw(norm_vec);
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
					if(vec.z<0)	//ʼ�ն˷���
					{
						f3dPoint perp=prj_line.startPt;
						prj_line.startPt=prj_line.endPt;
						prj_line.endPt=perp;
					}
					//���㻷�η���ڻ�ͼ����ϵ�µĽ�����Ʒ���
					f3dLine line_arr[4];
					double minDrwLen=TransToModelLength(1);
					double draw_thick=max(minDrwLen,plate_thick);
					if(S>0)
					{	//�·�壨����Ϊ��λ�����棩
						start_firs=prj_line.startPt+norm_vec*(draw_thick-0.5*plate_thick);
						end_firs=prj_line.endPt+norm_vec*(draw_thick-0.5*plate_thick);
					}
					else //if(S<0)
					{	//�Ϸ�壨����Ϊ��λ�����棩
						start_firs=prj_line.startPt+norm_vec*(0.5*plate_thick);
						end_firs=prj_line.endPt+norm_vec*(0.5*plate_thick);
					}
					line_arr[0].startPt=start_firs;
					line_arr[0].endPt=end_firs;
					start_sec=start_firs-norm_vec*draw_thick;
					end_sec=end_firs-norm_vec*draw_thick;
					line_arr[1].startPt=start_sec;
					line_arr[1].endPt=end_sec;
					start_thir=start_sec;
					end_thir=start_sec+norm_vec*draw_thick;
					line_arr[2].startPt=start_thir;
					line_arr[2].endPt=end_thir;
					start_four=end_sec;
					end_four=end_sec+norm_vec*draw_thick;
					line_arr[3].startPt=start_four;
					line_arr[3].endPt=end_four;
					//���ְ�ľ����Ϣ�����������������
					int i,max_len_index=0;
					double max_edge_len=-10000;
					for(i=0;i<4;i++)
					{
						double cur_edge_len=DISTANCE(line_arr[i].startPt,line_arr[i].endPt);
						if(cur_edge_len>max_edge_len)
						{
							max_edge_len=cur_edge_len;
							max_len_index=i;
						}
					}
					for(i=0;i<4;i++)
					{
						line_arr[i].pen.style=PS_SOLID;
						line_arr[i].ID=0;
						//if(i==max_len_index)
						//	line_arr[i].ID=pPlate->handle;
						if(display_angle_end>display_angle_start)
							line_arr[i].pen.style=PS_SOLID;	//�ɼ���
						else
							line_arr[i].pen.style=PS_DASH;	//���ز�
						edgeList.append(LineToArc(line_arr[i]));
					}
					//������Ϣ��չ��λ��
					data_point.dataPos=0.5*(line_arr[0].startPt+line_arr[0].endPt);
					data_point.dimPos=data_point.dataPos+norm_vec*plate_thick;
					DATAPOINT_TAG* pTag=AppendDataPoineTag();
					pTag->vertexArr.SetSize(0,6);
					data_point.dwKeyTag=pTag->key_tag;
					for(int i=0;i<4;++i)
					{
						pTag->vertexArr.append(line_arr[i].startPt);
						pTag->vertexArr.append(line_arr[i].endPt);
					}
				}
			}
		}
	}
	else if(pPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{
		//��ӻ�����ˮ����� wjh-2010-03-08
		if(pPlate==NULL)
			return FALSE;
		f3dPoint startPos;
		f3dPoint endPos;
		f3dPoint pre_vec = pPlate->ucs.axis_z;
		RotateVectorAroundVector(pre_vec,-Pi*0.5,cs.axis_z);
		double A=0,D=0,H=0,L=0,W=0,O=0,K=0;
		long C=0,E=0;
		pPlate->GetDesignItemValue('D', &D);	//��Բֱ��
		pPlate->GetDesignItemValue('W', &W);	//��Բֱ��
		pPlate->GetDesignItemValue('K', &K);	//��ˮ�ۿ��
		pPlate->GetDesignItemValue('A', &A);	//����ˮ��֮��н�
		pPlate->GetDesignItemValue('H', &H);	//��ˮ�����
		pPlate->GetDesignItemValue('O', &O);	//����ˮ�ۼнǵ�ƽ�������׼��֮��ļн�
		pPlate->GetDesignItemValue('L', &L);	//��ˮ�۳���
		pPlate->GetDesignItemValue('E', &E);	//��׼�߱�־ 0.��׼�� 1.������
		pPlate->GetDesignItemValue('C', &C);	//��׼�ֹܾ��
		CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(C,CLS_LINETUBE);
		if(pDatumTube==NULL||D>0)	//��ˮ���ڻ�ֱ�������ܴ���0
			return FALSE;
		pDatumTube->BuildUCS();
		double T=pPlate->thick;
		if(h0v1 == 1)
		{
			//�����������ͼ����
			startPos = pPlate->ucs.origin - 0.5*W*pre_vec;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin + 0.5*W*pre_vec;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin - 0.5*W*pre_vec;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin - 0.5*W*pre_vec + T*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));
		
			startPos = pPlate->ucs.origin + 0.5*W*pre_vec;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin + 0.5*W*pre_vec + T*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));
			//������ˮ�۽���
			startPos = pPlate->ucs.origin - 0.5*W*pre_vec + T*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin - 0.25*W*pre_vec + T*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin - 0.25*W*pre_vec + T*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin - 0.25*W*pre_vec + (T-H)*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin - 0.25*W*pre_vec + (T-H)*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin - (0.25*W-K)*pre_vec + (T-H)*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin - (0.25*W-K)*pre_vec + (T-H)*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin - (0.25*W-K)*pre_vec + H*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin - (0.25*W-K)*pre_vec + T*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin + (0.25*W-K)*pre_vec + T*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin + (0.25*W-K)*pre_vec + T*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin + (0.25*W-K)*pre_vec + (T-H)*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin + (0.25*W-K)*pre_vec + (T-H)*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin + (0.25*W)*pre_vec + (T-H)*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));

			startPos = pPlate->ucs.origin + (0.25*W)*pre_vec + (T-H)*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin + (0.25*W)*pre_vec + T*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));
			//���ݱ�ע��
			data_point.dataPos = startPos;
			data_point.dimPos=data_point.dataPos-5*pre_vec*pPlate->GetThick();

			startPos = pPlate->ucs.origin + (0.25*W)*pre_vec + T*pPlate->ucs.axis_z;
			startPos=dcs.TransToDraw(startPos);
			endPos = pPlate->ucs.origin + (0.5*W)*pre_vec + T*pPlate->ucs.axis_z;
			endPos=dcs.TransToDraw(endPos);
			line.startPt=startPos;
			line.endPt=endPos;
			edgeList.append(LineToArc(line));
		}
		else if(h0v1 == 0)
		{
			f3dPoint vertex;
			//ˮƽ����ͼ����
			if(D>0)
			{	
				endPos = pPlate->ucs.origin - D*pPlate->ucs.axis_x;
				endPos=dcs.TransToDraw(endPos);
				arcline.CreateMethod1(f3dPoint(0,0,0),endPos,f3dPoint(0,0,1),2*Pi);
				edgeList.append(arcline);
			}
			endPos = pPlate->ucs.origin - 0.5*W*pPlate->ucs.axis_x;
			endPos=dcs.TransToDraw(endPos);
			arcline.CreateMethod1(f3dPoint(0,0,0),endPos,f3dPoint(0,0,1),2*Pi);
			arcline.feature=pPlate->handle;
			edgeList.append(arcline);
			//����ˮƽ/��ֱ����
			/*CTextOnlyDrawing locate_point;
			locate_point.dimText.Printf("��׼�߷���");
			f3dPoint datum_edge_vec=pDatumTube->ucs.axis_x;
			if(E==1&&pDatumTube->m_bHasWeldRoad)	//�Ժ�����Ϊ��׼
				locate_point.dimText.Printf("����(����)����");
			//��ע��׼��
			if(O==0)
			{	
				//����ˮ�۽�ƽ�������׼��֮��н�Ϊ0
				locate_point.dimPos=pPlate->ucs.origin+pPlate->ucs.axis_x+(0.5*W+50);
				coord_trans(locate_point.dimPos, cs, FALSE);
				locate_point.angle=0;
				pureTextDimList.append(locate_point);
			}
			else 
			{
				O*=RADTODEG_COEF;	//ת��Ϊ����
				double sinb=sin(O);
				double cosb=cos(O);
				line.startPt=pPlate->ucs.origin - (0.5*W+30)*cosb - (0.5*W+30)*sinb*pPlate->ucs.axis_z;
				coord_trans(line.startPt, cs, FALSE);
				line.endPt=pPlate->ucs.origin + (0.5*W+30)*cosb + (0.5*W+30)*sinb*pPlate->ucs.axis_z;
				coord_trans(line.endPt, cs, FALSE);
				line.pen.style=PS_DASHDOT;
				edgeList.append(LineToArc(line));
				locate_point.dimPos=line.endPt;
				locate_point.angle=O;
				pureTextDimList.append(locate_point);
				//��ע��ˮ�ۼн����������׼��֮��ļн�
				startPos=pPlate->ucs.origin + (0.5*W+20)*cosb + (0.5*W+20)*sinb*pPlate->ucs.axis_z;
				coord_trans(startPos, cs, FALSE);
				endPos=pPlate->ucs.origin + (0.5*W+20);
				coord_trans(endPos, cs, FALSE);
				AppendAngularDim(f3dPoint(0,0,0),startPos,endPos);
			}*/
			//��ˮƽ���������ע��ˮ��
			A*=RADTODEG_COEF;	//ת��Ϊ����
			double sina=sin(A*0.5);
			double cosa=cos(A*0.5);
			line.startPt=pPlate->ucs.origin;
			line.startPt=dcs.TransToDraw(line.startPt);
			line.endPt = pPlate->ucs.origin+0.5*W*cosa*pPlate->ucs.axis_x+0.5*W*sina*pPlate->ucs.axis_y;
			line.endPt=dcs.TransToDraw(line.endPt);
			line.pen.style=PS_SOLID;
			edgeList.append(LineToArc(line));
			
			line.endPt = pPlate->ucs.origin+0.5*W*cosa*pPlate->ucs.axis_x-0.5*W*sina*pPlate->ucs.axis_y;
			line.endPt=dcs.TransToDraw(line.endPt);
			edgeList.append(LineToArc(line));
			if(cosa!=0&&sina!=0)
			{	//������ˮ���ڲ��
				double a=0,b=0,c=0;
				c=K/sina;
				a=c*cosa;
				b=0.5*W-c;	
				line.startPt = pPlate->ucs.origin + c*pPlate->ucs.axis_x;
				line.startPt=dcs.TransToDraw(line.startPt);
				line.endPt = pPlate->ucs.origin + (c+b*cosa)*pPlate->ucs.axis_x + b*sina*pPlate->ucs.axis_y;
				line.endPt=dcs.TransToDraw(line.endPt);
				edgeList.append(LineToArc(line));
				line.endPt = pPlate->ucs.origin + (c+b*cosa)*pPlate->ucs.axis_x - b*sina*pPlate->ucs.axis_y;
				line.endPt=dcs.TransToDraw(line.endPt);
				edgeList.append(LineToArc(line));
			}
			//��ע����ˮ��֮��ļн�0.5*(W+10)*
			f3dPoint startVec = cosa*pPlate->ucs.axis_x - sina*pPlate->ucs.axis_y;
			f3dPoint endPosVec = cosa*pPlate->ucs.axis_x + sina*pPlate->ucs.axis_y;
			AppendAngularDim(pPlate->ucs.origin,startVec,endPosVec,(W+10)*0.5);
			CSizeTextDim space_dim;
			
			//��ע��˨�ֲ��뾶
			/*f3dPoint tmp_vector;
			tmp_vector = pPlate->ucs.axis_y;
			RotateVectorAroundVector(tmp_vector,-Pi*0.2,pPlate->ucs.axis_z);
			f3dPoint ls_pos = pPlate->GetFirstLsRef()->GetLsPtr()->ucs.origin;
			coord_trans(ls_pos,cs,FALSE);
			double R_bolt = ls_pos.mod();
			startPos = pPlate->ucs.origin + R_bolt*tmp_vector;
			coord_trans(startPos, cs, FALSE);
			space_dim.dimStartPos = startPos;
			endPos = pPlate->ucs.origin  - R_bolt*tmp_vector;
			coord_trans(endPos, cs, FALSE);
			space_dim.dimEndPos = endPos;
			space_dim.dimPos = (startPos+endPos)/3.0;	
			space_dim.dist = R_bolt;
			space_dim.iOrder=0;
			space_dim.angle = cal_angle_of_2vec(tmp_vector,cs.axis_x);
			spaceDimList.append(space_dim);
			//��ע��Բֱ��
			startPos = pPlate->ucs.origin + 0.5*W*pPlate->ucs.axis_y;
			coord_trans(startPos, cs, FALSE);
			space_dim.dimStartPos = startPos;
			endPos = pPlate->ucs.origin  - 0.5*W*pPlate->ucs.axis_y;
			coord_trans(endPos, cs, FALSE);
			space_dim.dimEndPos = endPos;
			space_dim.dimPos = (startPos+endPos)/3.0;	
			space_dim.dist = W;
			space_dim.iOrder=0;
			space_dim.angle = cal_angle_of_2vec(pPlate->ucs.axis_y,cs.axis_x);
			spaceDimList.append(space_dim);*/
		}
	}
	//���������Ϣ��չ��
	data_point.hPart=pPlate->handle;
	data_point.cls_id=pPlate->GetClassTypeId();
	int iPartNoFrameStyle=sys.dim_map.PartNo.iPartNoFrameStyle;
	if(pPlate->m_bStdPart&&pPlate->IsInsertPlate())
		iPartNoFrameStyle=sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle;	//��׼��嵥��ָ����ſ����� wht 12-03-19
	else if (pPlate->IsFL())
		iPartNoFrameStyle=sys.dim_map.PartNo.iFlPartNoFrameStyle;				//��������ָ����ſ����� wht 12-03-19
	data_point.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,iPartNoFrameStyle);
	if(pPlate->m_iParamType==TYPE_ELBOWPLATE)
	{	//����ͼ��ֻ��ע������ʾ���߰�
		if(m_pSectionView->m_iViewType==VIEW_MAIN||m_pSectionView->m_iViewType==VIEW_FRONT)
		{	//����ͼ������ͼ
			if(h0v1==0)	//ˮƽ���Ƶ��߰�
				dataPointList.append(data_point);
		}
		else 
			dataPointList.append(data_point);
	}
	else
		dataPointList.append(data_point);
	if(h0v1==0&&pPlate->m_iParamType!=TYPE_ELBOWPLATE)
	{	//�麸���� �ݲ���ʾ����ϵĿ� wht 09-12-20
		double L=1000000;
		pPlate->GetDesignItemValue('L',&L);
		CXhSimpleList<CLDSPart::HOLE> hole_list;
		pPlate->GetHoleSet(&hole_list);
		for(CLDSPart::HOLE* pHole=hole_list.EnumObjectFirst();pHole;pHole=hole_list.EnumObjectNext())
		{
			f3dCircle ls_cir;
			if(pHole->pBolt)
			{
				ls_cir.centre=pHole->pBolt->ucs.origin;
				ls_cir.radius=pHole->pBolt->get_d()*0.5;
				ls_cir.feature=ftoi(pHole->pBolt->hole_d_increment*10);
			}
			else
			{
				ls_cir.centre=pHole->xMcsPosition;
				ls_cir.radius= pHole->wBoltD*0.5;
				ls_cir.feature=ftoi(pHole->fHoleMargin*10);
			}
			if(pPlate->m_iParamType==TYPE_XEND)
			{
				f3dPoint ls_pos=ls_cir.centre;
				coord_trans(ls_pos,pPlate->ucs,FALSE);
				if(ls_pos.x>L)
					continue;	//�����Ʋ���ʮ�ֲ����峤�ȷ�Χ�ڵ���˨ wht 10-08-06
			}
			f3dPoint ls_norm=(pHole->pBolt)?pHole->pBolt->ucs.axis_z:pHole->vHoleAxisVec;
			if(fabs(ls_norm*pPlate->ucs.axis_z)>EPS_COS2)
			{
				Int3dlf(ls_cir.centre,ls_cir.centre,ls_norm,pPlate->ucs.origin,pPlate->ucs.axis_z);
				ls_cir.centre=dcs.TransToDraw(ls_cir.centre);
				cirList.append(ls_cir);
			}
		}
		if(pPlate->IsInsertPlate())
		{	//��ע�����˨�о� wht 18-09-16
			CDimPlateBolts dimLs;
			dimLs.Create((CLDSParamPlate*)pPlate);
			CSizeTextDim space_dim;
			for(ROTATEDIM_STRU* pRotateDim=dimLs.EnumFirstRotateDim();pRotateDim;pRotateDim=dimLs.EnumNextRotateDim())
			{
				space_dim.dimStartPos=pRotateDim->startPos;
				space_dim.dimEndPos=pRotateDim->endPos;
				space_dim.dimPos=pRotateDim->dimPos;
				coord_trans(space_dim.dimStartPos,pPlate->ucs,TRUE);
				coord_trans(space_dim.dimEndPos,pPlate->ucs,TRUE);
				coord_trans(space_dim.dimPos,pPlate->ucs,TRUE);
				space_dim.dimStartPos=dcs.TransToDraw(space_dim.dimStartPos);
				space_dim.dimEndPos=dcs.TransToDraw(space_dim.dimEndPos);
				space_dim.dimPos=dcs.TransToDraw(space_dim.dimPos);
				space_dim.dimStartPos.z=space_dim.dimEndPos.z=space_dim.dimPos.z=0;
				space_dim.initDimPos=space_dim.dimPos;
				space_dim.dist=pRotateDim->dist;
				space_dim.bAllowRelayout=TRUE;
				space_dim.bShortenDimStartLine=space_dim.bShortenDimEndLine=TRUE;
				space_dim.angle = pRotateDim->angle;
				if(space_dim.dimStartPos.y>space_dim.dimEndPos.y)
					space_dim.bCanTextOutter=TRUE;
				space_dim.iOrder = 1;	//�ֲ���ע�ߴ�
				space_dim.nHits=pRotateDim->nHits;
				AppendSizeTextDim(space_dim);
			}
		}
		else if(pPlate->m_iParamType== TYPE_SHOEPLATE)
		{	//��עѥ����˨
			CDimPlateBolts dimLs;
			LOCALE_PROPITEM* pLocaleItem = NULL;
			pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimRayLsSpace");
			if (pLocaleItem && !dimLs.m_bDimRayLsSpace)
				dimLs.m_bDimRayLsSpace = pLocaleItem->bVal;
			pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimLsRayAngle");
			if (pLocaleItem && !dimLs.m_bDimLsRayAngle)
				dimLs.m_bDimLsRayAngle = pLocaleItem->bVal;
			pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimSpaceToEdgeLine");
			if (pLocaleItem && !dimLs.m_bDimSpaceToEdgeLine)
				dimLs.m_bDimSpaceToEdgeLine = pLocaleItem->bVal;
			pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimSpaceToWeldEdge");
			if (pLocaleItem && !dimLs.m_bDimSpaceToWeldEdge)
				dimLs.m_bDimSpaceToWeldEdge = pLocaleItem->bVal;
			pLocaleItem = gxDrwLocalizer.GetLocaleItem("DimFeatueLsSpace");
			if (pLocaleItem && !dimLs.m_bDimFeatureLsSpace)
				dimLs.m_bDimFeatureLsSpace = pLocaleItem->bVal;
			if (profileVertexList.GetSize() > 0)
				dimLs.InitPlateRgn(profileVertexList.m_pData, profileVertexList.GetSize());
			dimLs.CreateEx(pPlate, dcs);
			CSizeTextDim space_dim;
			for (ROTATEDIM_STRU* pDim = dimLs.EnumFirstRotateDim(); pDim; pDim = dimLs.EnumNextRotateDim())
			{
				CSizeTextDim space_dim;
				space_dim.dimStartPos = pDim->startPos;
				space_dim.dimEndPos = pDim->endPos;
				space_dim.dimStartPos.z = space_dim.dimEndPos.z = 0;
				space_dim.dist = pDim->dist;
				space_dim.bAllowRelayout = FALSE;
				space_dim.dimPos = 0.5*(space_dim.dimStartPos + space_dim.dimEndPos);
				space_dim.angle = atan((space_dim.dimEndPos.y - space_dim.dimStartPos.y) / (space_dim.dimEndPos.x - space_dim.dimStartPos.x));
				space_dim.iOrder = 0;	//װ�����߳ߴ�
				space_dim.nHits = pDim->nHits;
				AppendSizeTextDim(space_dim);
			}
			for (ANGULARDIM_STRU* pDim = dimLs.EnumFirstAngleDim(); pDim; pDim = dimLs.EnumNextAngleDim())
			{
				CSizeAngleDim angle_dim;
				angle_dim.dimStartPos = pDim->startPos;
				angle_dim.dimEndPos = pDim->endPos;
				angle_dim.dimPos = pDim->dimPos;
				angle_dim.vertex = pDim->vertex;
				AppendAngularDim(angle_dim);
			}
		}
	}
	if(h0v1==1&&pDatumTube&&pPlate->m_iParamType==TYPE_TUBERIBPLATE&&fabs(pPlate->ucs.axis_y*cs.axis_z)>0.707)
	{	//��ʱ����ע�ֹ��߰���ֹ�֮��ļн�
		f3dPoint datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		double dd=fabs(datum_line_vec*pPlate->ucs.axis_z);
		//������õĸֹ������������Ƶİ����䲻��ֱʱ��Ҫ��ע�����׼�ֹܵĽǶ�
		if(fabs(datum_line_vec*cs.axis_x)>EPS_COS&&dd>EPS&&dd<EPS_COS)
		{	//�����׼�ֹܲ���ֱ��ƽ�У���Ҫ��ע�Ƕ�
			f3dPoint vertex,start_vec,end_vec;
			vertex=pPlate->ucs.origin;
			start_vec=datum_line_vec;	//��ʼ�߷���
			f3dPoint norm=datum_line_vec^pPlate->ucs.axis_z;
			f3dPoint prj_line_vec=norm^pPlate->ucs.axis_z;
			normalize(prj_line_vec);
			if(prj_line_vec.x<0)
				prj_line_vec*=-1.0;
			end_vec=prj_line_vec;		//�Ƕȱ�ע��ֹ�߷���
			AppendAngularDim(vertex,start_vec,end_vec);
		}
	}
	return FALSE;
}

BOOL CSectionViewDrawing::CreateLineTubeDrawing(CLDSLineTube *pLineTube,CDrawCoordTrans& dcs,BOOL bDrawWeldLine/*=TRUE*/)
{
	CTubeDrawing drawer;
	//CSectionDrawingEntsManager manager(m_pRootPart,this,STORE_ACAD);
	drawer.style.bDrawWeldline=bDrawWeldLine!=FALSE;	//��������Զ�ͷΪ���ʱ���ƺ�����
	if(m_pSectionView->m_iViewType==VIEW_INSERTPLATE)
	{
		for(CSectionView::PART_ITEM* pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=pItem=m_pSectionView->SonParts.GetNext())
		{
			if(pLineTube->m_tJointStart.hLinkObj==pItem->hSonPart)
				drawer.style.hSectSlotPlate=pItem->hSonPart;
			else if(pLineTube->m_tJointEnd.hLinkObj==pItem->hSonPart)
				drawer.style.hSectSlotPlate=pItem->hSonPart;
			else
				continue;
			CLDSPart* pPart=pLineTube->BelongModel()->FromPartHandle(drawer.style.hSectSlotPlate);
			if(!pPart->IsPlate())
				drawer.style.hSectSlotPlate=0;
			break;
		}
	}
	BOOL rslt=drawer.CreateTubeDrawing(pLineTube,dcs,&m_xEntManager);
	PART_DIMnDRAW* pDimDraw=RegisterPartDimDraw(pLineTube,CLS_LINETUBE,drawer.style.ciDrawModeH0V1,drawer.style.bMandatoryDimLabel);
	if(m_pSectionView->GetViewTypeId()==CSectionView::CLS_SLAVE_NODESECT_VIEW)
	{
		CLDSNode* pBaseNode=Ta.Node.FromHandle(((CSlaveNodeSectionView*)m_pSectionView)->m_hBaseNode);
		CLDSLineTube* pBaseTube=(CLDSLineTube*)Ta.Parts.FromHandle(((CSlaveNodeSectionView*)m_pSectionView)->m_hVertRod,CLS_LINETUBE);
		if(pBaseNode&&pBaseTube&&(	//��ͷ���ĸֹ��Ͻڵ�����ͼ�����ע�ֹܱ��
			(pBaseTube->pStart==pBaseNode&&pBaseTube->m_tJointStart.type==1)||(pBaseTube->pEnd==pBaseNode&&pBaseTube->m_tJointEnd.type==1)))
			pDimDraw->bMandatoryDimLabel=true;
	}
	if(gxDrwLocalizer.ciCustomerSerial!=3)
	{	//�ൺ����Ҫ���麸ͼ��ע�ֹܱ�ţ������Ż����� wxc-2018.10.26
		CLDSLineTube* pRootSectTube=NULL;
		if(m_pSectionView->IsTubeSectionView(&pRootSectTube)&&pRootSectTube==m_pRootPart)
			pDimDraw->bDimPartNo=false;	//����˵��Դ�ܷ��иֹܽ���ͼ�б�ע���ܱ�� wjh-2018.3.6
		else if(m_pRootPart&&m_pRootPart->IsLinePart())
		{
			PART_DIMnDRAW* pDimDraw=m_pPartGroupDrawing->GetPartDimDraw(m_pSectionView->GetId(),m_pRootPart->handle);
			if(pDimDraw)
				pDimDraw->bDimPartNo=false;
		}
	}
	return rslt;
}
//<DEVELOP_PROCESS_MARK nameId="CSectionViewDrawing::CreateCutTubeDrawing">
BOOL CalEdgeLine(f3dPoint startPt,f3dPoint endPt,double R,UCS_STRU ucs,f3dLine& line)
{
	f2dCircle circle;
	double p1X,p1Y,p2X,p2Y;
	circle.centre.Set(0,0);
	circle.radius=R;
	coord_trans(startPt,ucs,FALSE);
	startPt.z=0;
	coord_trans(endPt,ucs,FALSE);
	endPt.z=0;
	if(Int2dlc(f2dLine(startPt,endPt),circle,p1X,p1Y,p2X,p2Y)!=2)	//ֱ����Բ��
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("There is not valid intersection point of tube and ball!");
#else
		logerr.Log("�ֹ�������û����Ч����!");
#endif
		return FALSE;
	}
	f3dPoint inters1(p1X,p1Y,0),inters2(p2X,p2Y,0);
	f3dPoint vec=endPt-startPt;
	f3dPoint tem_vec=inters1-inters2;
	if(vec*tem_vec)
		line.startPt=inters1;
	else
		line.startPt=inters2;
	line.endPt=endPt;
	return TRUE;
}
BOOL CSectionViewDrawing::CreateCutTubeDrawing(CLDSLineTube *pLineTube,BOOL bCoplanar/*=TRUE*/)
{
	//����ֹ�ƽ�̵�������
	double tube_thick=pLineTube->GetThick();
	double R=0,radius=pLineTube->GetDiameter()/2.0;
	f3dLine upOutterLine,upInnerLine,axisLine,downInnerLine,downOutterLine;
	BOOL bJointStart=FALSE;
	if(m_pRootPart->GetClassTypeId()==CLS_SPHERE)
	{
		CLDSSphere* pDatumSphere=(CLDSSphere*)m_pRootPart;
		R=pDatumSphere->D*0.5;
		if(pDatumSphere->hDatumNode==pLineTube->pStart->handle)
			bJointStart=TRUE;
		else
			bJointStart=FALSE;
	}
	else if(m_pRootPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube* pDatumTube=(CLDSLineTube*)m_pRootPart;
		R=pDatumTube->GetDiameter()*0.5;
		if(pLineTube->m_tJointStart.hLinkObj==pDatumTube->handle)
			bJointStart=TRUE;
		else
			bJointStart=FALSE;
	}
	f3dPoint startPt,endPt,len_vec,norm_vec;
	len_vec=pLineTube->End()-pLineTube->Start();
	normalize(len_vec);
	if(bJointStart)
	{
		startPt=pLineTube->Start()-len_vec*pLineTube->startOdd();
		endPt  =pLineTube->End()+len_vec*pLineTube->endOdd();
	}
	else
	{
		startPt=pLineTube->End()+len_vec*pLineTube->endOdd();
		endPt  =pLineTube->Start()-len_vec*pLineTube->startOdd();
	}
	len_vec=endPt-startPt;		//�˼����ȷ���(��ʼ��ָ��ĩ��)
	normalize(len_vec);
	norm_vec=len_vec;
	RotateVectorAroundVector(norm_vec,Pi/2,cs.axis_z);	//��ǰ����ƽ���ϴ�ֱ�ڸ˼���ƫ�Ʒ���
	//�ֹ��������
	f3dPoint line_start,line_end;
	line_start=startPt+norm_vec*radius-len_vec*R*2;
	line_end  =endPt+norm_vec*radius;
	if(!CalEdgeLine(line_start,line_end,R,cs,upOutterLine))
		return FALSE;
	line_start=startPt-norm_vec*radius-len_vec*R*2;
	line_end=endPt-norm_vec*radius;
	if(!CalEdgeLine(line_start,line_end,R,cs,downOutterLine))
		return FALSE;
	//�ֹ��ڲ�������
	line_start=startPt+norm_vec*(radius-tube_thick)-len_vec*R*2;
	line_end=endPt+norm_vec*(radius-tube_thick);
	if(!CalEdgeLine(line_start,line_end,R,cs,upInnerLine))
		return FALSE;
	line_start=startPt-norm_vec*(radius-tube_thick)-len_vec*R*2;
	line_end=endPt-norm_vec*(radius-tube_thick);
	if(!CalEdgeLine(line_start,line_end,R,cs,downInnerLine))
		return FALSE;
	//�ֹ���������(׼��)��
	line_start=startPt-len_vec*R*2;
	line_end=endPt;
	if(!CalEdgeLine(line_start,line_end,R,cs,axisLine))
		return FALSE;
	vector_trans(len_vec,cs,FALSE);
	len_vec.z=0;
	normalize(len_vec);
	axisLine.endPt+=len_vec*30;		//׼��ĩ�����30mm	
	//���ֹ���������ӵ���ͼ�б���
	short penStyle=PS_SOLID;
	if(!bCoplanar)		//�ǹ���Ĺ���ͳһ�����߱�ʶ
		penStyle=PS_DASH;
	f3dLine line;
	line.startPt=upOutterLine.endPt;
	line.endPt=downOutterLine.endPt;
	line.pen.style=penStyle;			
	line.ID=0;
	edgeList.append(LineToArc(line));
	upOutterLine.pen.style=penStyle;	
	upOutterLine.ID=0;
	edgeList.append(LineToArc(upOutterLine));
	downOutterLine.pen.style=penStyle;	
	downOutterLine.ID=0;
	edgeList.append(LineToArc(downOutterLine));
	upInnerLine.pen.style=PS_DASH;		
	upInnerLine.ID=0;
	edgeList.append(LineToArc(upInnerLine));
	downInnerLine.pen.style=PS_DASH;	
	downInnerLine.ID=0;
	edgeList.append(LineToArc(downInnerLine));
	axisLine.pen.style=PS_DASHDOTDOT;	
	line.ID=0;
	edgeList.append(LineToArc(axisLine));
	//���������Ϣ��չ��
	CTextFrame data_point;	//���ݵ�
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);	//��ʼ�����߿��ı�
	data_point.cls_id=pLineTube->GetClassTypeId();
	data_point.hPart=pLineTube->handle;
	data_point.BuildFrame(GetPartNoIncMat(pLineTube->GetPartNo(),pLineTube->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	data_point.dataPos=0.5*(axisLine.startPt+axisLine.endPt);
	int str_len=(int)strlen(pLineTube->GetPartNo());
	data_point.dimPos=data_point.dataPos+cs.axis_y*str_len*0.5;
	dataPointList.append(data_point);
	//��¼�ֹ��������� wht 18-08-25
	m_xEntManager.tube_edge_line[0]=upOutterLine;
	m_xEntManager.tube_edge_line[1]=downOutterLine;
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>
//<DEVELOP_PROCESS_MARK nameId="CSectionViewDrawing::CreateSphereDrawing">
BOOL CSectionViewDrawing::CreateSphereDrawing(CLDSSphere* pSphere)
{
	if(pSphere==NULL)
		return FALSE;
	f3dArcLine arc_line;
	f3dPoint center,startPt,vec;
	center=pSphere->ucs.origin;
	startPt=center+cs.axis_x*pSphere->D*0.5;
	coord_trans(center,cs,FALSE);
	coord_trans(startPt,cs,FALSE);
	arc_line.CreateMethod1(center,startPt,cs.axis_z,2*Pi);
	edgeList.append(arc_line);
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>
//���ɽǸ��Ϻ��ӵĽǸ��߰��������
void CSectionViewDrawing::CreateJgRibPlateEdge(CLDSLineAngle *pJg,f3dLine perp_line,f3dPoint wing_vec,BOOL bInsideJg)
{
	f3dPoint origin,len_vec=perp_line.endPt-perp_line.startPt;
	f3dLine leftEdge,rightEdge;
	normalize(len_vec);
	int nPush=Ta.Parts.push_stack();
	//���Ǹ��߰嵱����˨��������
	for(CLDSParamPlate* pRibPlate=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);pRibPlate;pRibPlate=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
	{	//���ƽǸ��߰�������
		if(pRibPlate->m_iParamType!=TYPE_LINEANGLERIBPLATE||pRibPlate->m_hPartWeldParent!=pJg->handle)
			continue;
		origin=pRibPlate->ucs.origin+pRibPlate->m_fNormOffset*pRibPlate->ucs.axis_z;
		SnapPerp(&leftEdge.startPt,perp_line,origin);
		leftEdge.endPt=leftEdge.startPt+wing_vec*(pJg->GetWidth()-pJg->GetThick());
		leftEdge.startPt-=len_vec*pRibPlate->GetThick()*0.5;
		leftEdge.endPt-=len_vec*pRibPlate->GetThick()*0.5;
		rightEdge.startPt=leftEdge.startPt+len_vec*pRibPlate->GetThick()*0.5;
		rightEdge.endPt=leftEdge.endPt+len_vec*pRibPlate->GetThick()*0.5;
		coord_trans(leftEdge.startPt,cs,FALSE);
		coord_trans(leftEdge.endPt,cs,FALSE);
		coord_trans(rightEdge.startPt,cs,FALSE);
		coord_trans(rightEdge.endPt,cs,FALSE);
		leftEdge.startPt.z=leftEdge.endPt.z=rightEdge.startPt.z=rightEdge.endPt.z=0;
		if(bInsideJg)
			leftEdge.feature=rightEdge.feature=1;		//�ڲ���߰���������
		edgeList.append(leftEdge);	//�����
		edgeList.append(rightEdge);	//�����
		//�߰���
		CTextFrame data_point;
		//��ʼ�����߿��ı�
		data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
		data_point.cls_id=CLS_PARAMPLATE;
		data_point.hPart=pRibPlate->handle;
		data_point.BuildFrame(GetPartNoIncMat(pRibPlate->GetPartNo(),pRibPlate->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
		data_point.dataPos=leftEdge.endPt;//.x,leftEdge.endPt.y,-sys.dim_map.PartNo.fPartNoTextSize*3);
		double fTextLen=strlen(pRibPlate->GetPartNo())*sys.dim_map.PartNo.fPartNoTextSize*0.7;
		data_point.dimPos.Set(data_point.dataPos.x+fTextLen,data_point.dataPos.y);
		DATAPOINT_TAG* pTag=AppendDataPoineTag();
		data_point.dwKeyTag=pTag->key_tag;
		pTag->vertexArr.append(rightEdge.startPt);
		pTag->vertexArr.append(leftEdge.startPt);
		pTag->vertexArr.append(leftEdge.endPt);
		pTag->vertexArr.append(rightEdge.endPt);
		dataPointList.append(data_point);
	}
	Ta.Parts.pop_stack(nPush);
}

BOOL CSectionViewDrawing::CreateLineAngleDrawing(CLDSLineAngle *pLineAngle,CDrawCoordTrans& dcs)
{
	if(pLineAngle==NULL)
		return FALSE;
	JGZJ jgzj;
	f3dLine line;
	BOOL bInsideJg=FALSE;
	double wing_wide,wing_thick;
	f3dPoint work_wing_norm;
	int x_wing0_y_wing1=-1;
	//|��������������������|��first line: start_firs,end_firs
	//|_ _ _ _ _ _ _ _ _ _ |  second line:start_sec,end_sec
	//|____________________|  third line: start_thir,end_thir
	//axis_start ___ _ ____ _ ____axis_end
	f3dPoint point,start_firs,end_firs,start_sec,end_sec,
			start_thir,end_thir,axis_start,axis_end,wing_vec,len_vec;
	pLineAngle->getUCS(pLineAngle->ucs);
	getjgzj(jgzj,pLineAngle->GetWidth());
	wing_wide = pLineAngle->GetWidth();
	wing_thick= pLineAngle->GetThick();
	double ddx,ddy;
	ddx = pLineAngle->get_norm_x_wing()*dcs.PrjNorm();
	ddy = pLineAngle->get_norm_y_wing()*dcs.PrjNorm();
	f3dPoint cur_jg_vec=pLineAngle->End()-pLineAngle->Start();
	normalize(cur_jg_vec);
	if(fabs(cur_jg_vec*dcs.PrjNorm())>0.70710)
		x_wing0_y_wing1=2;	//�Ǹֽ���
	else if(fabs(ddx)>fabs(ddy))
		x_wing0_y_wing1=0;
	else
		x_wing0_y_wing1=1;
	//
	if(x_wing0_y_wing1==0)
	{
		if(dcs.PrjNorm()*pLineAngle->get_norm_x_wing()>0)
			bInsideJg=TRUE;		//����
		else
			bInsideJg=FALSE;	//����
		if(pLineAngle->m_bEnableTeG)
			jgzj = pLineAngle->xWingXZhunJu;
		wing_vec = pLineAngle->GetWingVecX();
		work_wing_norm = pLineAngle->get_norm_x_wing();
	}
	else if(x_wing0_y_wing1==1)
	{
		if(dcs.PrjNorm()*pLineAngle->get_norm_y_wing()>0)
			bInsideJg=TRUE;		//����
		else
			bInsideJg=FALSE;	//����
		if(pLineAngle->m_bEnableTeG)
			jgzj = pLineAngle->xWingYZhunJu;
		wing_vec = pLineAngle->GetWingVecY();
		work_wing_norm = pLineAngle->get_norm_y_wing();
	}
	else if(x_wing0_y_wing1==2)
	{	//���ƽ���ͼ
		if(!pLineAngle->m_bEnableTeG)
		{
			pLineAngle->xWingYZhunJu = jgzj;
			pLineAngle->xWingXZhunJu = jgzj;
		}
		f3dPoint line_vec,x_out_vertex,x_in_vertex,y_out_vertex,y_in_vertex,ber_out_vertex,ber_in_vertex;
		//��ȡ�Ǹֽ���ͼ
		Int3dlf(ber_out_vertex,f3dLine(pLineAngle->Start(),pLineAngle->End()),pLineAngle->ucs.origin,dcs.PrjNorm());
		x_out_vertex=ber_out_vertex+pLineAngle->GetWingVecX()*wing_wide;
		y_out_vertex=ber_out_vertex+pLineAngle->GetWingVecY()*wing_wide;
		x_in_vertex=x_out_vertex-pLineAngle->get_norm_x_wing()*wing_thick;
		y_in_vertex=y_out_vertex-pLineAngle->get_norm_y_wing()*wing_thick;
		line_vec=pLineAngle->End()-pLineAngle->Start();
		normalize(line_vec);
		Int3dlf(x_out_vertex,x_out_vertex,line_vec,pLineAngle->ucs.origin,dcs.PrjNorm());
		Int3dlf(y_out_vertex,y_out_vertex,line_vec,pLineAngle->ucs.origin,dcs.PrjNorm());
		Int3dlf(x_in_vertex,x_in_vertex,line_vec,pLineAngle->ucs.origin,dcs.PrjNorm());
		Int3dlf(y_in_vertex,y_in_vertex,line_vec,pLineAngle->ucs.origin,dcs.PrjNorm());
		//ת������ͼ����ϵ��
		ber_out_vertex=dcs.TransToDraw(ber_out_vertex);
		x_out_vertex=dcs.TransToDraw(x_out_vertex);
		y_out_vertex=dcs.TransToDraw(y_out_vertex);
		x_in_vertex=dcs.TransToDraw(x_in_vertex);
		y_in_vertex=dcs.TransToDraw(y_in_vertex);
		ber_out_vertex.z=x_out_vertex.z=y_out_vertex.z=x_in_vertex.z=y_in_vertex.z=0;
		//�ռ�ֱ����ȷ���Ǹ������
		f3dLine x_in_line,y_in_line,x_axis_line,y_axis_line;
		x_in_line.startPt=x_in_vertex;
		y_in_line.startPt=y_in_vertex;
		x_in_line.endPt=x_in_vertex-x_out_vertex+ber_out_vertex;
		y_in_line.endPt=y_in_vertex-y_out_vertex+ber_out_vertex;
		Int3dll(x_in_line,y_in_line,x_in_line.endPt);
		y_in_line.endPt=x_in_line.endPt;
		line.pen.style=PS_SOLID;//ͼ��0	ʵ��
		if(pLineAngle->m_bFootNail&&pLineAngle->ucs.axis_z*dcs.PrjNorm()<0)
			line.pen.style=PS_DASH;	//ͼ��1 ����
		line.startPt=ber_out_vertex;
		line.endPt=x_out_vertex;
		line.ID=pLineAngle->handle;
		edgeList.append(LineToArc(line));
		line.startPt=ber_out_vertex;
		line.endPt=y_out_vertex;
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=x_in_vertex;
		line.endPt=x_out_vertex;
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=y_in_vertex;
		line.endPt=y_out_vertex;
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=x_in_line.startPt;
		line.endPt=x_in_line.endPt;
		line.ID=0;
		edgeList.append(LineToArc(line));
		line.startPt=y_in_line.startPt;
		line.endPt=y_in_line.endPt;
		line.ID=0;
		edgeList.append(LineToArc(line));
		return TRUE;
	}

	start_thir = pLineAngle->Start();
	end_thir   = pLineAngle->End();
	Sub_Pnt(len_vec,end_thir,start_thir);
	normalize(len_vec);//�Ǹֳ��ȷ���(��ʼ��ָ��ĩ��)
	Add_Pnt(start_thir,pLineAngle->Start(),-1.0*len_vec*pLineAngle->startOdd());
	Add_Pnt(end_thir,pLineAngle->End(),len_vec*pLineAngle->endOdd());

	Add_Pnt(start_firs,start_thir,wing_vec*wing_wide);
	Add_Pnt(end_firs,end_thir,wing_vec*wing_wide);
	Add_Pnt(start_sec,start_thir,wing_vec*wing_thick);
	Add_Pnt(end_sec,end_thir,wing_vec*wing_thick);

	Add_Pnt(axis_start,start_thir,wing_vec*jgzj.g);
	Add_Pnt(axis_end,end_thir,wing_vec*jgzj.g);
	f3dLine perp_line(start_sec,end_sec);
	CreateJgRibPlateEdge(pLineAngle,perp_line,wing_vec,bInsideJg);
	if(!pLineAngle->m_bFootNail)
	{
		if(pLineAngle->startOdd()>0)//׼�����˶����30mm
			Sub_Pnt(axis_start,axis_start,len_vec*(30+pLineAngle->startOdd()));
		else
			Sub_Pnt(axis_start,axis_start,len_vec*30);//׼�����˶����30mm
		if(pLineAngle->endOdd()>0)//׼�����˶����30mm
			Add_Pnt(axis_end  ,axis_end  ,len_vec*(30+pLineAngle->endOdd()));
		else
			Add_Pnt(axis_end  ,axis_end  ,len_vec*30);//׼�����˶����30mm
	}
	//ת������ͼ����ϵ��
	start_firs=dcs.TransToDraw(start_firs);
	end_firs=dcs.TransToDraw(end_firs);
	start_sec=dcs.TransToDraw(start_sec);
	end_sec=dcs.TransToDraw(end_sec);
	start_thir=dcs.TransToDraw(start_thir);
	end_thir=dcs.TransToDraw(end_thir);
	axis_start=dcs.TransToDraw(axis_start);
	axis_end=dcs.TransToDraw(axis_end);
	vector_trans(wing_vec,dcs,FALSE);
	start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=axis_start.z=axis_end.z=0;
	line.pen.style=PS_SOLID;//ͼ��0	ʵ��
	line.startPt=start_firs;
	line.endPt=end_firs;
	line.ID=0;
	edgeList.append(LineToArc(line));
	line.startPt=start_thir;
	line.endPt=end_thir;
	line.ID=0;
	edgeList.append(LineToArc(line));
	line.startPt=start_firs;
	line.endPt=start_thir;
	line.ID=0;
	edgeList.append(LineToArc(line));
	line.startPt=end_firs;
	line.endPt=end_thir;
	line.ID=0;
	edgeList.append(LineToArc(line));
	if(bInsideJg)	//����
		line.pen.style=PS_DASH;	//ͼ��1 ����
	line.startPt=start_sec;
	line.endPt=end_sec;
	line.ID=0;
	if(m_pSectionView->m_iViewType==VIEW_FOOTNAILANGLE)
	{	//�ǸֽŶ���ͼ���������Ϣ��չ��
		CTextFrame data_point;
		//��ʼ�����߿��ı�
		data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
		data_point.cls_id=pLineAngle->GetClassTypeId();
		data_point.hPart=pLineAngle->handle;
		data_point.BuildFrame(GetPartNoIncMat(pLineAngle->GetPartNo(),pLineAngle->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
		data_point.dataPos=0.5*(start_firs+end_firs);
		int str_len=(int)strlen(pLineAngle->GetPartNo());
		f3dPoint dim_vec=start_firs-start_thir;
		normalize(dim_vec);
		data_point.dimPos=data_point.dataPos+dim_vec*str_len;
		DATAPOINT_TAG* pTag=AppendDataPoineTag();
		data_point.dwKeyTag=pTag->key_tag;
		pTag->vertexArr.append(start_firs);
		pTag->vertexArr.append(start_thir);
		pTag->vertexArr.append(end_thir);
		pTag->vertexArr.append(end_firs);
		dataPointList.append(data_point);
	}
	else
		line.ID=pLineAngle->handle;
	edgeList.append(LineToArc(line));
	line.pen.style=PS_DASHDOTDOT;
	line.startPt=axis_start;
	line.endPt=axis_end;
	line.ID=0;
	edgeList.append(LineToArc(line));
	
	for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
	{
		if(fabs(pLsRef->GetLsPtr()->get_norm()*dcs.PrjNorm())>EPS_COS2)
		{
			f3dCircle ls_cir;
			ls_cir.centre=(*pLsRef)->ucs.origin;
			ls_cir.centre=dcs.TransToDraw(ls_cir.centre);
			ls_cir.centre.z=0;	//z�������
			ls_cir.radius=(*pLsRef)->get_d()*0.5;
			ls_cir.feature=ftoi((*pLsRef)->hole_d_increment*10);
			cirList.append(ls_cir);
		}
		else if(pLineAngle->m_bFootNail)
		{
			f3dCircle ls_cir;
			ls_cir.centre=(*pLsRef)->ucs.origin;
			line.startPt=ls_cir.centre+wing_thick*(*pLsRef)->get_norm()*3;
			line.endPt=ls_cir.centre-wing_thick*(*pLsRef)->get_norm();
			line.startPt=dcs.TransToDraw(line.startPt);
			line.endPt=dcs.TransToDraw(line.endPt);
			line.startPt.z=line.endPt.z=0;
			line.pen.style=PS_SOLID;
			line.ID=0;
			edgeList.append(LineToArc(line));
		}
	}
	return TRUE;
}

BOOL CSectionViewDrawing::CreateLineSlotDrawing(CLDSLineSlot *pLineSlot,CDrawCoordTrans& dcs)
{
	if(pLineSlot==NULL)
		return FALSE;
	pLineSlot->BuildUCS();
	CTextFrame data_point;//������Ϣ��չ��
	//��ʼ�����߿��ı�
	data_point.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	f3dArcLine arc_line;
	f3dLine axis_line,line;	
	f3dCircle ls_cir;
	f3dPoint len_vec=pLineSlot->End()-pLineSlot->Start();
	normalize(len_vec);
	double foot_nail_len=16;	//���ƽŶ�ͼ��ʱ�ĽŶ�����
	f3dPoint start_firs,end_firs,start_sec,end_sec,start_thir,end_thir,start_four,end_four;
	int h0v1s2=-1;	//ƽ�̻���-0 �������-1 �������-2
	if(fabs(pLineSlot->ucs.axis_z*cs.axis_z)>0.707)
		h0v1s2=2;	//����
	else if(fabs(pLineSlot->ucs.axis_y*cs.axis_z)>0.707)
		h0v1s2=0;	//ƽ��
	else 
		h0v1s2=1;	//����
	RegisterPartDimDraw(pLineSlot,CLS_LINESLOT,h0v1s2);
	//��ʱֻ�ڲ۸���ʾ����ͼʱ���������չ��  wht 09-12-17
	f3dPoint center,line_norm_vec;
	BOOL bFootNailPlate=FALSE;
	if(pLineSlot->GetLsCount()==1)
	{	//�۸��ڴ˴�Ϊ�Ŷ���
		CLDSBolt *pBolt=pLineSlot->GetFirstLsRef()->GetLsPtr();
		if(pBolt&&pBolt->IsFootNail())
			bFootNailPlate=TRUE;	
	}
	if(h0v1s2==0)
	{	//ƽ��
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
		center=(axis_line.startPt+axis_line.endPt)*0.5;

		if(pLineSlot->startOdd()<0)
			axis_line.startPt+=len_vec*pLineSlot->startOdd();
		axis_line.startPt-=len_vec*30;//׼�����˶����30mm
		if(pLineSlot->endOdd()<0)
			axis_line.endPt-=len_vec*pLineSlot->endOdd();
		axis_line.endPt+=len_vec*30;//׼�����˶����30mm
		//ת������ͼ����ϵ��
		axis_line.startPt=dcs.TransToDraw(axis_line.startPt);	//coord_trans(axis_line.startPt, cs, FALSE);
		axis_line.endPt=dcs.TransToDraw(axis_line.endPt);		//coord_trans(axis_line.endPt, cs, FALSE);
		start_firs=dcs.TransToDraw(start_firs);	//coord_trans(start_firs,cs,FALSE);
		end_firs=dcs.TransToDraw(end_firs);		//coord_trans(end_firs,cs,FALSE);
		start_sec=dcs.TransToDraw(start_sec);	//coord_trans(start_sec,cs,FALSE);
		end_sec=dcs.TransToDraw(end_sec);		//coord_trans(end_sec,cs,FALSE);
		start_thir=dcs.TransToDraw(start_thir);	//coord_trans(start_thir,cs,FALSE);
		end_thir=dcs.TransToDraw(end_thir);		//coord_trans(end_thir,cs,FALSE);
		start_four=dcs.TransToDraw(start_four);	//coord_trans(start_four,cs,FALSE);
		end_four=dcs.TransToDraw(end_four);		//coord_trans(end_four,cs,FALSE);
		axis_line.startPt.z=axis_line.endPt.z=start_firs.z=end_firs.z=0;
		start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=0;
		if(bFootNailPlate)
		{	//���ƽŶ�ͼ��
			short style=PS_DASH;
			if(pLineSlot->ucs.axis_y*cs.axis_z<0)
				style=PS_DOT;
			line_norm_vec.Set(-len_vec.y,len_vec.x);
			normalize(line_norm_vec);
			if(line_norm_vec*cs.axis_y<0)
				line_norm_vec*=-1.0;
			f3dLine line1,line2;
			line1.startPt=center+line_norm_vec*foot_nail_len*0.5;
			line1.endPt=center-line_norm_vec*(foot_nail_len*0.5+2);
			line2.startPt=center+line_norm_vec*foot_nail_len*0.5+len_vec*(foot_nail_len*0.5-2);
			line2.endPt=center+line_norm_vec*foot_nail_len*0.5-len_vec*(foot_nail_len*0.5-2);
			center=dcs.TransToDraw(center);	//coord_trans(center,cs,FALSE);
			line1.startPt=dcs.TransToDraw(line1.startPt);	//coord_trans(line1.startPt,cs,FALSE);
			line1.endPt=dcs.TransToDraw(line1.endPt);		//coord_trans(line1.endPt,cs,FALSE);
			line2.startPt=dcs.TransToDraw(line2.startPt);	//coord_trans(line2.startPt,cs,FALSE);
			line2.endPt=dcs.TransToDraw(line2.endPt);		//coord_trans(line2.endPt,cs,FALSE);
			center.z=line1.startPt.z=line1.endPt.z=line2.startPt.z=line2.endPt.z=0;
			arc_line.CreateMethod1(center,center+f3dPoint(1)*10,f3dPoint(0,0,1),2*Pi);	//ԲȦ
			line1.ID=line2.ID=arc_line.ID=0;
			line1.pen.style=line2.pen.style=arc_line.pen.style=style;
			m_xEntManager.AppendLine(LineToArc(line1));
			m_xEntManager.AppendLine(LineToArc(line2));
			m_xEntManager.AppendLine(arc_line);
		}
		//���Ʋ۸�
		if(!bFootNailPlate)
		{	//�Ŷ��۸ֵ���������������
			axis_line.ID=0;
			axis_line.pen.style=PS_DASHDOTDOT;
			m_xEntManager.AppendLine(LineToArc(axis_line));	//����
		}
		short pen_sytle=PS_SOLID;
		//��Ҫ�ر�ע��۸ֵĹ����淨����ֲ�����ϵ�ģ��ᷴ��wjh-2016.10.18
		CTextOnlyDrawing sideSymbol;
		sideSymbol.angle=0;
		sideSymbol.iOrder=4;
		sideSymbol.bNeedConvertDistToText=false;
		sideSymbol.dimPos.x=(start_firs.x+end_firs.x)*0.5;
		//ע������ZoomToModelTextSize�����ͼ������������߶ȣ���m_xEntManager.TransToModelLength��ģ�ͳߴ����ͼ�ߴ��ı���ת���� wjh-2018.3.1
		sideSymbol.textHeight=ZoomToModelTextSize(sys.fonts.weld.fPartNoTextSize);
		bool bOnFrontSide=true;
		if(pLineSlot->ucs.axis_y*cs.axis_z>0)
		{
			bOnFrontSide=false;
			pen_sytle=PS_DASH;
		}
		if(!bOnFrontSide||m_pSectionView->IsDualSideSlotNail(pLineSlot->handle))
		{
			sideSymbol.dimText="��";
			sideSymbol.alignType=TopCenter;
			sideSymbol.dimPos.y =(start_firs.y<start_four.y)?start_firs.y:start_four.y;
			sideSymbol.dimPos.y-=m_xEntManager.TransToModelLength(0.75);	//'��'���泬�����ƻ���������Ҫ���Ӽ�϶ֵ wjh-2018.3.1
			m_xEntManager.AppendTextDim(sideSymbol);
		}
		if( bOnFrontSide||m_pSectionView->IsDualSideSlotNail(pLineSlot->handle))
		{
			sideSymbol.dimText="��";
			sideSymbol.alignType=BottomCenter;
			sideSymbol.dimPos.y =(start_firs.y>start_four.y)?start_firs.y:start_four.y;
			sideSymbol.dimPos.y+=m_xEntManager.TransToModelLength(0.25);
			m_xEntManager.AppendTextDim(sideSymbol);
		}
		line.pen.style=pen_sytle;
		line.startPt=start_firs;
		line.endPt=end_firs;
		line.ID=pLineSlot->handle;
		m_xEntManager.AppendLine(LineToArc(line));
		line.ID=0;
		line.startPt=start_four;
		line.endPt=end_four;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_firs;
		line.endPt=start_four;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=end_firs;
		line.endPt=end_four;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_sec;
		line.endPt=end_sec;
		line.ID=pLineSlot->handle;
		line.pen.style=PS_DASH;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_thir;
		line.endPt=end_thir;
		line.pen.style=PS_DASH;
		m_xEntManager.AppendLine(LineToArc(line));
	}
	else if(h0v1s2==1)
	{	//����
		start_firs=pLineSlot->Start()-len_vec*pLineSlot->startOdd ();
		end_firs=pLineSlot->End()+len_vec*pLineSlot->endOdd();
		start_sec=start_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		end_sec=end_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		center=(start_firs+end_firs)*0.5;
		start_firs=dcs.TransToDraw(start_firs);	//coord_trans(start_firs,cs,FALSE);
		end_firs=dcs.TransToDraw(end_firs);		//coord_trans(end_firs,cs,FALSE);
		start_sec=dcs.TransToDraw(start_sec);	//coord_trans(start_sec,cs,FALSE);
		end_sec=dcs.TransToDraw(end_sec);		//coord_trans(end_sec,cs,FALSE);
		start_firs.z=end_firs.z=start_sec.z=end_sec.z=0;
		if(bFootNailPlate)
		{	//���ƽŶ�ͼ��
			line_norm_vec.Set(-len_vec.y,len_vec.x);
			normalize(line_norm_vec);
			if(line_norm_vec*pLineSlot->ucs.axis_y>0)
				line_norm_vec*=-1.0;
			f3dLine line1,line2;
			line1.startPt=center+line_norm_vec*foot_nail_len*1.5;
			line1.endPt=center-line_norm_vec*5;
			line2.startPt=line1.startPt+len_vec*(foot_nail_len*0.5-2);
			line2.endPt=line1.startPt-len_vec*(foot_nail_len*0.5-2);
			line1.startPt=dcs.TransToDraw(line1.startPt);	//coord_trans(line1.startPt,cs,FALSE);
			line1.endPt=dcs.TransToDraw(line1.endPt);		//coord_trans(line1.endPt,cs,FALSE);
			line2.startPt=dcs.TransToDraw(line2.startPt);	//coord_trans(line2.startPt,cs,FALSE);
			line2.endPt=dcs.TransToDraw(line2.endPt);		//coord_trans(line2.endPt,cs,FALSE);
			line1.startPt.z=line1.endPt.z=line2.startPt.z=line2.endPt.z=0;
			line1.ID=line2.ID=0;
			line1.pen.style=line2.pen.style=PS_DOT;
			m_xEntManager.AppendLine(LineToArc(line1));
			m_xEntManager.AppendLine(LineToArc(line2));
		}
		//���Ʋ۸�
		line.pen.style=PS_SOLID;
		line.startPt=start_firs;
		line.endPt=end_firs;
		line.ID=pLineSlot->handle;
		m_xEntManager.AppendLine(LineToArc(line));
		line.ID=0;
		line.startPt=start_sec;
		line.endPt=end_sec;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_firs;
		line.endPt=start_sec;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=end_firs;
		line.endPt=end_sec;
		m_xEntManager.AppendLine(LineToArc(line));
	}
	else if(h0v1s2==2)
	{	//����
		start_firs=pLineSlot->Start()-len_vec*pLineSlot->startOdd ()-pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		end_firs=pLineSlot->Start()-len_vec*pLineSlot->startOdd()+pLineSlot->ucs.axis_x*0.5*pLineSlot->GetHeight();
		start_sec=start_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		end_sec=end_firs+pLineSlot->ucs.axis_y*pLineSlot->GetWidth();
		start_thir=start_sec+pLineSlot->ucs.axis_x*pLineSlot->GetThick();
		end_thir=end_sec-pLineSlot->ucs.axis_x*pLineSlot->GetThick();
		start_four=start_thir-pLineSlot->ucs.axis_y*(pLineSlot->GetWidth()-pLineSlot->GetThick());
		end_four=end_thir-pLineSlot->ucs.axis_y*(pLineSlot->GetWidth()-pLineSlot->GetThick());
		center=(start_firs+end_firs)*0.5;
		len_vec=start_firs-end_firs;
		normalize(len_vec);
		//ת������ͼ����ϵ��
		start_firs=dcs.TransToDraw(start_firs);	//coord_trans(start_firs,cs,FALSE);
		end_firs=dcs.TransToDraw(end_firs);		//coord_trans(end_firs,cs,FALSE);
		start_sec=dcs.TransToDraw(start_sec);	//coord_trans(start_sec,cs,FALSE);
		end_sec=dcs.TransToDraw(end_sec);		//coord_trans(end_sec,cs,FALSE);
		start_thir=dcs.TransToDraw(start_thir);	//coord_trans(start_thir,cs,FALSE);
		end_thir=dcs.TransToDraw(end_thir);		//coord_trans(end_thir,cs,FALSE);
		start_four=dcs.TransToDraw(start_four);	//coord_trans(start_four,cs,FALSE);
		end_four=dcs.TransToDraw(end_four);		//coord_trans(end_four,cs,FALSE);
		start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=0;
		if(bFootNailPlate)
		{	//���ƽŶ�ͼ��
			line_norm_vec.Set(-len_vec.y,len_vec.x);
			normalize(line_norm_vec);
			if(line_norm_vec*pLineSlot->ucs.axis_y>0)
				line_norm_vec*=-1.0;
			f3dLine line1,line2;
			line1.startPt=center+line_norm_vec*foot_nail_len*1.5;
			line1.endPt=center-line_norm_vec*10;
			line2.startPt=line1.startPt+len_vec*(foot_nail_len*0.5-2);
			line2.endPt=line1.startPt-len_vec*(foot_nail_len*0.5-2);
			line1.startPt=dcs.TransToDraw(line1.startPt);	//coord_trans(line1.startPt,cs,FALSE);
			line1.endPt=dcs.TransToDraw(line1.endPt);		//coord_trans(line1.endPt,cs,FALSE);
			line2.startPt=dcs.TransToDraw(line2.startPt);	//coord_trans(line2.startPt,cs,FALSE);
			line2.endPt=dcs.TransToDraw(line2.endPt);		//coord_trans(line2.endPt,cs,FALSE);
			line1.startPt.z=line1.endPt.z=line2.startPt.z=line2.endPt.z=0;
			line1.ID=line2.ID=0;
			line1.pen.style=line2.pen.style=PS_DOT;
			m_xEntManager.AppendLine(LineToArc(line1));
			m_xEntManager.AppendLine(LineToArc(line2));
		}
		//���Ʋ۸�
		line.pen.style=PS_SOLID;//ͼ��0
		line.startPt=start_firs;
		line.endPt=end_firs;
		line.ID=pLineSlot->handle;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_four;
		line.endPt=end_four;
		line.ID=0;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_firs;
		line.endPt=start_sec;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=end_firs;
		line.endPt=end_sec;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_sec;
		line.endPt=start_thir;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=end_sec;
		line.endPt=end_thir;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=start_thir;
		line.endPt=start_four;
		m_xEntManager.AppendLine(LineToArc(line));
		line.startPt=end_thir;
		line.endPt=end_four;
		m_xEntManager.AppendLine(LineToArc(line));
		//���������Ϣ��չ��
		data_point.cls_id=pLineSlot->GetClassTypeId();
		data_point.hPart=pLineSlot->handle;
		data_point.BuildFrame(GetPartNoIncMat(pLineSlot->GetPartNo(),pLineSlot->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));
		data_point.dataPos=(start_firs+end_firs)*0.5;
		int str_len=(int)strlen(pLineSlot->GetPartNo());
		f3dPoint dim_vec=end_firs-start_sec;
		normalize(dim_vec);
		data_point.dimPos=data_point.dataPos+dim_vec*str_len;
		DATAPOINT_TAG* pTag=AppendDataPoineTag();
		data_point.dwKeyTag=pTag->key_tag;
		pTag->vertexArr.append(start_firs);
		pTag->vertexArr.append(end_firs);
		pTag->vertexArr.append(end_sec);
		pTag->vertexArr.append(end_thir);
		pTag->vertexArr.append(end_four);
		pTag->vertexArr.append(start_four);
		pTag->vertexArr.append(start_thir);
		pTag->vertexArr.append(start_sec);
		dataPointList.append(data_point);
	}
	if(!bFootNailPlate)
	{	//������˨
		for(CLsRef *pLsRef=pLineSlot->GetFirstLsRef();pLsRef;pLsRef=pLineSlot->GetNextLsRef())
		{
			if(fabs(pLsRef->GetLsPtr()->get_norm()*cs.axis_z)>EPS_COS2)
			{
				f3dCircle ls_cir;
				ls_cir.centre=(*pLsRef)->ucs.origin;
				ls_cir.centre=dcs.TransToDraw(ls_cir.centre);	//coord_trans(ls_cir.centre,cs,FALSE);
				ls_cir.centre.z=0;	//z�������
				ls_cir.radius=(*pLsRef)->get_d()*0.5;
				ls_cir.feature=ftoi((*pLsRef)->hole_d_increment*10);
				m_xEntManager.AppendCircle(ls_cir);
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
				line.startPt=dcs.TransToDraw(line.startPt);	//coord_trans(line.startPt,cs,FALSE);
				line.endPt=dcs.TransToDraw(line.endPt);		//coord_trans(line.endPt,cs,FALSE);
				line.startPt.z=line.endPt.z=0;
				line.pen.style=PS_SOLID;
				m_xEntManager.AppendLine(LineToArc(line));
			}
		}
	}
	return TRUE;
}

//�����������ͼ wht 10-08-13
typedef struct tagSORTEDPOINT_ITEM
{
	f3dPoint pos;
	double distToStart;
}SORTEDPOINT_ITEM;
BOOL CSectionViewDrawing::CreateSpecialSectionView()	
{
	if(m_pSectionView==NULL||m_pTower==NULL||m_pPartGroup==NULL)
		return FALSE;
	f3dPoint len_vec;
	f3dLine line,axis_line,rayLine[10];
	if(m_pRootPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pRootTube=(CLDSLineTube*)m_pRootPart;
		len_vec=pRootTube->End()-pRootTube->Start();
		normalize(len_vec);
		CLDSParamPlate *pStartFl=(CLDSParamPlate*)m_pTower->FromPartHandle(pRootTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		CLDSParamPlate *pEndFl=(CLDSParamPlate*)m_pTower->FromPartHandle(pRootTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		double start_h=0,end_h=0;
		if(pStartFl&&(pStartFl->m_iParamType==TYPE_FL||
			pStartFl->m_iParamType==TYPE_FLG||pStartFl->m_iParamType==TYPE_FLR))
			pStartFl->GetDesignItemValue('H',&start_h);
		if(pEndFl&&(pEndFl->m_iParamType==TYPE_FL||
			pEndFl->m_iParamType==TYPE_FLG||pEndFl->m_iParamType==TYPE_FLR))
			pEndFl->GetDesignItemValue('H',&end_h);
		axis_line.startPt=pRootTube->Start()-len_vec*(pRootTube->startOdd()+start_h);
		axis_line.endPt  =pRootTube->End()+len_vec*(pRootTube->endOdd()+end_h);
	}
	//
	int nRayCount=0,nPosNum=2;
	f3dPoint ray_pos[10];	//
	SORTEDPOINT_ITEM inters_pos[10];
	CSectionView::PART_ITEM *pItem=NULL;
	inters_pos[0].pos=axis_line.startPt;
	inters_pos[0].distToStart=0;
	nPosNum=1;
	for(pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		if(TagPart(pItem)==NULL)
			m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
		if(TagPart(pItem)==NULL)
			continue;
		if(m_pSectionView->m_iViewType==VIEW_KNODESKETCH)
		{	//K�ڵ����ֹܼ�ͼ 
			if(pItem->hSonPart==m_pRootPart->handle)
			{
				line.startPt=axis_line.startPt;
				line.endPt=axis_line.endPt;
				coord_trans(line.startPt,cs,FALSE);
				coord_trans(line.endPt,cs,FALSE);
				line.startPt.z=line.endPt.z=0;
				line.ID=0;
				line.pen.style=PS_DASHDOTDOT;
				edgeList.append(LineToArc(line));
				continue;
			}
			else if(TagPart(pItem)->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			CLDSParamPlate *pFl=(CLDSParamPlate*)TagPart(pItem);
			if(pFl->m_iParamType!=TYPE_FL&&pFl->m_iParamType!=TYPE_FLG&&
				pFl->m_iParamType!=TYPE_FLR)
				continue;
			CreatePartSketch(pItem);
			//
			CLDSLineTube *pFatherTube=NULL;
			pFatherTube=(CLDSLineTube*)m_pTower->FromPartHandle(TagPart(pItem)->m_hPartWeldParent,CLS_LINETUBE);
			if(pFatherTube==NULL||(pFatherTube&&pFatherTube->handle==m_pRootPart->handle))
				continue;
			double H=0;
			pFl->GetDesignItemValue('H',&H);
			f3dPoint pos,ray_vec=pFatherTube->End()-pFatherTube->Start();
			normalize(ray_vec);
			rayLine[nRayCount].startPt=pFatherTube->Start()-len_vec*pFatherTube->startOdd();
			rayLine[nRayCount].endPt  =pFatherTube->End()+len_vec*pFatherTube->endOdd();
			int nRetCode=Int3dpl(axis_line,rayLine[nRayCount],pos);
			if(nRetCode>0)
			{
				if(pFl->handle==pFatherTube->m_tJointStart.hLinkObj)
				{
					rayLine[nRayCount].startPt-=len_vec*H;
					rayLine[nRayCount].endPt  =pos;
					ray_pos[nRayCount]=rayLine[nRayCount].startPt;	//��¼���߸ֹܲ��������ཻ��һ��
				}
				else
				{
					rayLine[nRayCount].startPt=pos;
					rayLine[nRayCount].endPt  +=len_vec*H;
					ray_pos[nRayCount]=rayLine[nRayCount].endPt;	//��¼���߸ֹܲ��������ཻ��һ��
				}
				if(nRayCount==0)	//��¼���߸ֹ������ܵĽ���
				{
					inters_pos[1].pos=pos;
					inters_pos[1].distToStart=DISTANCE(inters_pos[0].pos,pos);
					nPosNum=2;
				}
			}
			line.startPt=rayLine[nRayCount].startPt;
			line.endPt=rayLine[nRayCount].endPt;
			coord_trans(line.startPt,cs,FALSE);
			coord_trans(line.endPt,cs,FALSE);
			line.startPt.z=line.endPt.z=0;
			line.ID=0;
			line.pen.style=PS_DASHDOTDOT;
			edgeList.append(LineToArc(line));
			nRayCount++;
		}
	}
	//��ע�ߴ�
	if(m_pSectionView->m_iViewType==VIEW_KNODESKETCH)
	{	//
		int i=0,n=nRayCount;
		for(i=0;i<n;i++)
		{
			f3dLine temp_line;
			temp_line.startPt=ray_pos[i];
			temp_line.endPt=ray_pos[(i+1)%n];
			f3dPoint pos;
			int nRetCode=Int3dll(axis_line,temp_line,pos);
			if(nRetCode==1)
			{
				double dist=DISTANCE(inters_pos[0].pos,pos);
				if(dist<inters_pos[nPosNum-1].distToStart)
				{
					inters_pos[nPosNum].pos=inters_pos[nPosNum-1].pos;
					inters_pos[nPosNum].distToStart=inters_pos[nPosNum-1].distToStart;
					inters_pos[nPosNum-1].pos=pos;
					inters_pos[nPosNum-1].distToStart=dist;
				}
				else
				{
					inters_pos[nPosNum].pos=pos;
					inters_pos[nPosNum].distToStart=dist;
				}
				nPosNum++;
				//�����ֱ����ı�ע��
				rayLine[nRayCount].startPt=temp_line.startPt;
				rayLine[nRayCount].endPt=pos;
				nRayCount++;
				rayLine[nRayCount].startPt=pos;
				rayLine[nRayCount].endPt=temp_line.endPt;
				nRayCount++;
				//����ֱ��
				line.startPt=temp_line.startPt;
				line.endPt=temp_line.endPt;
				coord_trans(line.startPt,cs,FALSE);
				coord_trans(line.endPt,cs,FALSE);
				line.startPt.z=line.endPt.z=0;
				line.ID=0;
				line.pen.style=PS_SOLID;
				edgeList.append(LineToArc(line));
			}
		}
		//��ע����
		CSizeTextDim space_dim;
		for(i=0;i<nRayCount;i++)
		{
			space_dim.dimStartPos=rayLine[i].startPt;
			space_dim.dimEndPos=rayLine[i].endPt;
			space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
			coord_trans(space_dim.dimStartPos, cs, FALSE);
			coord_trans(space_dim.dimEndPos, cs, FALSE);
			space_dim.dimStartPos.z=space_dim.dimEndPos.z=0;
			space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
			space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
			space_dim.iOrder = 0;	//װ�����߳ߴ�
			space_dim.nHits=1;
			space_dim.bAllowRelayout=FALSE;	//�������Զ�������עλ��
			spaceDimList.append(space_dim);
		}
		//
		inters_pos[nPosNum].pos=axis_line.endPt;
		nPosNum++;
		for(i=0;i<nPosNum-1;i++)
		{
			space_dim.dimStartPos=inters_pos[i].pos;
			space_dim.dimEndPos=inters_pos[i+1].pos;
			space_dim.dist=DISTANCE(space_dim.dimStartPos,space_dim.dimEndPos);
			coord_trans(space_dim.dimStartPos, cs, FALSE);
			coord_trans(space_dim.dimEndPos, cs, FALSE);
			space_dim.dimStartPos.z=space_dim.dimEndPos.z=0;
			space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
			space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
			space_dim.iOrder = 0;	//װ�����߳ߴ�
			space_dim.nHits=1;
			space_dim.bAllowRelayout=FALSE;	//�������Զ�������עλ��
			spaceDimList.append(space_dim);
		}
	}
	return TRUE;
}
BOOL CSectionViewDrawing::IsPartGroupHasKmAngleView()
{
	if(m_pPartGroup==NULL)
		return FALSE;
	CSectionView* pView=NULL;
	BOOL pushed=m_pPartGroup->SonViews.push_stack();
	for(pView=m_pPartGroup->SonViews.GetFirst();pView;pView=m_pPartGroup->SonViews.GetNext())
	{
		if(pView->GetViewTypeId()==CSectionView::CLS_KAOME_ANGLE_VIEW)
			break;
	}
	m_pPartGroup->SonViews.pop_stack(pushed);
	if(pView)
		return TRUE;
	else
		return FALSE;
}
void CSectionViewDrawing::CreateKaoMeAngleSketch(CKaoMeAngleSectionView* pKmAngleView)
{
	CDrawCoordTrans dcs;
	if(!m_pRootPart->IsPlate() || !GetDCS(m_pRootPart->handle,dcs) || pKmAngleView->m_pAngle==NULL)
		return;
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)m_pRootPart;
	if(fabs(dcs.PrjNorm()*pPlate->ucs.axis_z)<0.707)
		return;
	CLDSLineAngle* pKaoMeJg=pKmAngleView->m_pAngle;
	double wing_wide = pKaoMeJg->GetWidth();
	double wing_thick= pKaoMeJg->GetThick();
	//�жϽǸֵ���֫
	short penStyle=PS_SOLID;
	f3dPoint wing_norm,wing_vec,len_vec,jg_start,jg_end;
	len_vec=pKaoMeJg->End()-pKaoMeJg->Start();
	normalize(len_vec);
	jg_start=pKaoMeJg->Start()-len_vec*pKaoMeJg->startOdd();
	jg_end=pKaoMeJg->End()+len_vec*pKaoMeJg->endOdd();
	double ddx=dcs.PrjNorm()*pKaoMeJg->get_norm_x_wing();
	double ddy=dcs.PrjNorm()*pKaoMeJg->get_norm_y_wing();
	if(fabs(ddx)>fabs(ddy))
	{
		wing_norm=pKaoMeJg->get_norm_x_wing();
		wing_vec=pKaoMeJg->GetWingVecX();
		if(ddx>0)
			penStyle=PS_DASH;
	}
	else
	{	
		wing_norm=pKaoMeJg->get_norm_y_wing();
		wing_vec=pKaoMeJg->GetWingVecY();
		if(ddy>0)
			penStyle=PS_DASH;
	}
	//������˨������Ƶ�ʼ�ն�
	UCS_STRU ucs;
	pKaoMeJg->getUCS(ucs);
	f3dPoint startPt,endPt,pt,offset_vec;
	double fMaxZ=0,fMinZ=10000000;
	for(int i=0;i<2;i++)
	{
		for(CLDSBolt* pLs=pKmAngleView->EnumFirstBolt(i);pLs;pLs=pKmAngleView->EnumNextBolt(i))
		{
			f3dPoint ls_pt=pLs->ucs.origin;
			if(i==0)
			{	//Y֫��˨
				Int3dlf(pt,ls_pt,pLs->ucs.axis_z,ucs.origin,ucs.axis_x);
				offset_vec=ucs.axis_y;
			}
			else
			{	//Xֵ��˨
				Int3dlf(pt,ls_pt,pLs->ucs.axis_z,ucs.origin,ucs.axis_y);
				offset_vec=ucs.axis_x;
			}
			//����˨�����������ϵ�λ�ã����ҵ���ʼ���������Զ�ĵ���Ϊ��ģ�Ǹֵ�ʼ�ն�
			Int3dll(f3dLine(jg_start,jg_end),f3dLine(pt,pt-offset_vec*wing_wide),pt);
			double fDist=DISTANCE(jg_start,pt);
			if(fDist<fMinZ)
			{
				startPt=pt;
				fMinZ=fDist;
			}
			if(fDist>fMaxZ)
			{
				endPt=pt;
				fMaxZ=fDist;
			}
		}
	}
	startPt-=len_vec*30;
	endPt+=len_vec*30;
	//���ƽǸּ�����ͼ
	f3dLine line;
	line.pen.style=penStyle;
	//����
	line.startPt=startPt;
	line.endPt=endPt;
	line.startPt=dcs.TransToDraw(line.startPt);
	line.endPt=dcs.TransToDraw(line.endPt);
	line.startPt.z=line.endPt.z=0;
	m_xEntManager.AppendLine(line);
	//֫��ͶӰ��
	line.startPt=startPt+wing_vec*wing_thick;
	line.endPt=endPt+wing_vec*wing_thick;
	line.startPt=dcs.TransToDraw(line.startPt);
	line.endPt=dcs.TransToDraw(line.endPt);
	line.startPt.z=line.endPt.z=0;
	m_xEntManager.AppendLine(line);
	//֫����
	line.startPt=startPt+wing_vec*wing_wide;
	line.endPt=endPt+wing_vec*wing_wide;
	line.startPt=dcs.TransToDraw(line.startPt);
	line.endPt=dcs.TransToDraw(line.endPt);
	line.startPt.z=line.endPt.z=0;
	m_xEntManager.AppendLine(line);
	//ʼ��֫����
	line.startPt=startPt;
	line.endPt=startPt+wing_vec*wing_wide;
	line.startPt=dcs.TransToDraw(line.startPt);
	line.endPt=dcs.TransToDraw(line.endPt);
	line.startPt.z=line.endPt.z=0;
	m_xEntManager.AppendLine(line);
	//�ն�֫����
	line.startPt=endPt;
	line.endPt=endPt+wing_vec*wing_wide;
	line.startPt=dcs.TransToDraw(line.startPt);
	line.endPt=dcs.TransToDraw(line.endPt);
	line.startPt.z=line.endPt.z=0;
	m_xEntManager.AppendLine(line);
	//������ݱ궨��
	CTextFrame data_point;
	data_point.iCurLayer=penStyle;	//������ڲ�
	data_point.cls_id=pKaoMeJg->GetClassTypeId();
	data_point.hPart=pKaoMeJg->handle;
	data_point.BuildFrame(GetPartNoIncMat(pKaoMeJg->GetPartNo(),pKaoMeJg->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	data_point.dataPos=0.5*(startPt+wing_vec*wing_wide+endPt+wing_vec*wing_wide);
	data_point.dimPos=data_point.dataPos-wing_vec*wing_thick;
	data_point.dataPos=dcs.TransToDraw(data_point.dataPos);
	data_point.dimPos=dcs.TransToDraw(data_point.dimPos);
	data_point.dataPos.z=data_point.dimPos.z=0;
	m_xEntManager.AppendDataPoint(data_point);
}


BOOL RetrievedTubeTowerFoot(CSectionView *pView,CLDSLineTube **ppDatumTube,CLDSPlate **ppFootPlate,
						    CLDSPlate **ppConnectPlate1,CLDSPlate **ppConnectPlate2,PARTSET *pRibPlateSet);	//From PartGroup.cpp
struct PLATE_EDGE_LINE{
	f3dLine *m_pLine;
	double m_fCosa;
	PLATE_EDGE_LINE(f3dLine *pLine=NULL,double cosa=0){
		m_pLine=pLine;
		m_fCosa=cosa;
	}
};
static int ComparePlateEdgeLine(const PLATE_EDGE_LINE &item1, const PLATE_EDGE_LINE &item2)
{
	if(item1.m_fCosa>item2.m_fCosa)
		return -1;
	else if(item1.m_fCosa<item2.m_fCosa)
		return 1;
	else
		return 0;
}
struct TUBE_FOOT_FEATURE_LINE{
private:
	BOOL m_bInit;
	CSectionViewDrawing *m_pViewDrawing;
	//���Ź���
	CLDSLineTube *m_pDatumTube;
	CLDSGeneralPlate *m_pFootBasePlate;
	//
	f3dPoint up_base_point,down_base_point;
	f3dLine foot_plate_base_line;
	f3dLine up_tube_edge_line,down_tube_edge_line;
	f3dPoint up_tube_line_point,down_tube_line_point;
	//
	f3dPoint plate_extend_vec;
	f3dPoint tube_extend_vec;
	//
	BOOL InitFeatureLine(){
		if(m_pViewDrawing==NULL||m_pFootBasePlate==NULL||m_pDatumTube==NULL)
			return FALSE;
		CLDSNode *pBaseNode=m_pFootBasePlate->GetBaseNode();
		CSectionDrawingEntsManager::VERTICAL_PLATE *pVertPlate=m_pViewDrawing->m_xEntManager.m_hashVertPlateList.GetValue(m_pFootBasePlate->handle);
		if(pVertPlate==NULL||pBaseNode==NULL)
			return FALSE;
		//1.��ʼ�����ŵװ���ƻ�׼�߼���׼��
		tube_extend_vec=m_pDatumTube->Start()-m_pDatumTube->End();
		if(m_pDatumTube->pStart==pBaseNode)
			tube_extend_vec*=-1.0;
		normalize(tube_extend_vec);
		UCS_STRU cs=m_pViewDrawing->GetCS();
		vector_trans(tube_extend_vec,cs,FALSE);
		tube_extend_vec.z=0;
		//1.1 ��ʼ�����Ż�׼�ߡ��ְ����췽��
		foot_plate_base_line=pVertPlate->face_line[0][0];
		plate_extend_vec=pVertPlate->face_line[0][0].startPt-pVertPlate->face_line[0][1].startPt;
		normalize(plate_extend_vec);
		if(plate_extend_vec*tube_extend_vec<0)
		{
			foot_plate_base_line=pVertPlate->face_line[0][1];
			plate_extend_vec*=-1.0;
		}
		//1.2 ��׼�㡢�ְ�ָ��ֹ������ֱ����
		if(foot_plate_base_line.startPt.y>foot_plate_base_line.endPt.y)
		{
			up_base_point=foot_plate_base_line.startPt;
			down_base_point=foot_plate_base_line.endPt;
		}
		else
		{
			up_base_point=foot_plate_base_line.endPt;
			down_base_point=foot_plate_base_line.startPt;
		}
		//2.��ʼ���ֹܻ�׼��
		up_tube_edge_line=m_pViewDrawing->m_xEntManager.tube_edge_line[0];
		down_tube_edge_line=m_pViewDrawing->m_xEntManager.tube_edge_line[1];
		if(DISTANCE(up_tube_edge_line.startPt,up_base_point)<DISTANCE(up_tube_edge_line.endPt,up_base_point))
			up_tube_line_point=up_tube_edge_line.startPt;
		else
			up_tube_line_point=up_tube_edge_line.endPt;
		if(DISTANCE(down_tube_edge_line.startPt,down_base_point)<DISTANCE(down_tube_edge_line.endPt,down_base_point))
			down_tube_line_point=down_tube_edge_line.startPt;
		else
			down_tube_line_point=down_tube_edge_line.endPt;
		return TRUE;
	}
public:
	TUBE_FOOT_FEATURE_LINE(CSectionViewDrawing *pViewDrawing,CLDSGeneralPlate *pFootBasePlate,CLDSLineTube *pDatumTube){
		m_pViewDrawing=pViewDrawing;
		m_pFootBasePlate=pFootBasePlate;
		m_pDatumTube=pDatumTube;
		m_bInit=InitFeatureLine();
	}
	void DrawFootRibPlateSketch(CLDSGeneralPlate *pRibPlate,BOOL bUpSidePlate){
		if(!m_bInit)
			return;
		if(pRibPlate==NULL)
			return;
		f3dPoint base_point=bUpSidePlate?up_base_point:down_base_point;
		f3dPoint tube_edge_point=bUpSidePlate?up_tube_line_point:down_tube_line_point;
		f3dPoint line_vert_vec=bUpSidePlate?f3dPoint(-plate_extend_vec.y,plate_extend_vec.x):f3dPoint(plate_extend_vec.y,-plate_extend_vec.x);
		//
		double fStepHeight=30;
		double fLen=max(pRibPlate->GetWidth(),pRibPlate->GetLength());
		f3dLine line1,line2;
		line1.startPt=base_point;
		line1.endPt=base_point+plate_extend_vec*fStepHeight;
		m_pViewDrawing->m_xEntManager.AppendLine(line1);
		line2.startPt=tube_edge_point+tube_extend_vec*fLen;
		line2.endPt=line2.startPt-line_vert_vec*fStepHeight;
		m_pViewDrawing->m_xEntManager.AppendLine(line2);
		m_pViewDrawing->m_xEntManager.AppendLine(f3dLine(line1.endPt,line2.endPt));
		//
		//���������Ϣ��չ��
		CTextFrame data_point;
		data_point.cls_id=pRibPlate->GetClassTypeId();
		data_point.hPart=pRibPlate->handle;
		double fWidth=DISTANCE(tube_edge_point,base_point);
		data_point.dataPos=tube_edge_point-line_vert_vec*fWidth*0.3+tube_extend_vec*fLen*0.4;
		data_point.dimPos=data_point.dataPos;
		//data_point.dataPos.y-=sys.dim_map.PartNo.fPartNoTextSize*1.5;
		//data_point.dimPos.y+=sys.dim_map.PartNo.fPartNoTextSize*1.5;
		data_point.BuildFrame(GetPartNoIncMat(pRibPlate->GetPartNo(),pRibPlate->cMaterial),m_pViewDrawing->ZoomToModelTextSize(sys.fonts.weld.labelFrame));
		DATAPOINT_TAG* pTag=m_pViewDrawing->m_xEntManager.AppendDataPoineTag();
		data_point.dwKeyTag=pTag->key_tag;
		pTag->vertexArr.append(down_tube_line_point);
		pTag->vertexArr.append(base_point);
		pTag->vertexArr.append(line1.endPt);
		pTag->vertexArr.append(line2.endPt);
		pTag->vertexArr.append(line2.startPt);
		m_pViewDrawing->m_xEntManager.AppendDataPoint(data_point);
	}
	void CorrectFootConnectPlateSketch(CLDSGeneralPlate *pConnectPlate,BOOL bUpSidePlate){
		if(!m_bInit)
			return;
		if(pConnectPlate==NULL)
			return;
		CSectionDrawingEntsManager::HORZ_PLATE *pHorzPlate=m_pViewDrawing->m_xEntManager.m_hashFootConnectPlateList.GetValue(pConnectPlate->handle);
		if(pHorzPlate==NULL)
			return;
		//1.��װ��׼�߼��㽻��
		BOOL bStart[2]={FALSE,FALSE};
		f3dLine line_arr[3];
		f3dPoint inters_pt,inters_pt_arr[2],org_end_pt_arr[2];
		int hits=0;
		const double cos75=0.25881904510252076234889883762405;
		ARRAY_LIST<PLATE_EDGE_LINE> plateEdgeList;
		for(f3dLine *pLine=pHorzPlate->edgeLineList.GetFirst();pLine;pLine=pHorzPlate->edgeLineList.GetNext())
		{
			f3dPoint line_vec=pLine->startPt-pLine->endPt;
			normalize(line_vec);
			double cosa=fabs(line_vec*tube_extend_vec);
			plateEdgeList.append(PLATE_EDGE_LINE(pLine,cosa));
		}
		CHeapSort<PLATE_EDGE_LINE>::HeapSort(plateEdgeList.m_pData,plateEdgeList.GetSize(),ComparePlateEdgeLine);
		for(PLATE_EDGE_LINE *pEdgeLine=plateEdgeList.GetFirst();pEdgeLine;pEdgeLine=plateEdgeList.GetNext())
		{
			f3dLine *pLine=pEdgeLine->m_pLine;
			int nRetCode=Int3dll(foot_plate_base_line,*pLine,inters_pt);
			if(nRetCode>0||nRetCode==-2)
			{
				line_arr[hits]=*pLine;
				inters_pt_arr[hits]=inters_pt;
				if(DISTANCE(pLine->startPt,inters_pt)<DISTANCE(pLine->endPt,inters_pt))
				{
					bStart[hits]=TRUE;
					org_end_pt_arr[hits]=pLine->startPt;
				}
				else
				{
					bStart[hits]=FALSE;
					org_end_pt_arr[hits]=pLine->endPt;
				}
				hits++;
			}
			if(hits==2)
				break;
		}
		if(hits==2)
		{	//���Ҵ��޸������ߣ��м��������
			for(f3dLine *pLine=pHorzPlate->edgeLineList.GetFirst();pLine;pLine=pHorzPlate->edgeLineList.GetNext())
			{
				if( (pLine->startPt.IsEqual(org_end_pt_arr[0])&&pLine->endPt.IsEqual(org_end_pt_arr[1]))||
					(pLine->startPt.IsEqual(org_end_pt_arr[1])&&pLine->endPt.IsEqual(org_end_pt_arr[0])))
				{
					line_arr[2]=*pLine;
					hits++;
					break;
				}
			}
		}
		//2.����ͼ������ʵ���в�����Ӧֱ��
		typedef f3dArcLine* LINE_PTR;
		LINE_PTR line_ptr_arr[3]={NULL,NULL,NULL};
		if(hits==3)
		{	
			hits=0;
			ATOM_LIST<f3dArcLine> *pEdgeList=m_pViewDrawing->GetEdgeLineList();
			for(f3dArcLine *pArcLine=pEdgeList->GetFirst();pArcLine;pArcLine=pEdgeList->GetNext())
			{
				for(int i=0;i<3;i++)
				{
					if(line_ptr_arr[i])
						continue;
					if(pArcLine->Start().IsEqual(line_arr[i].startPt)&&pArcLine->End().IsEqual(line_arr[i].endPt))
					{
						line_ptr_arr[i]=pArcLine;
						hits++;
					}
				}
				if(hits==3)
					break;
			}
		}
		//3.�����ְ�������
		if(line_ptr_arr[0]&&line_ptr_arr[1]&&line_ptr_arr[2])
		{
			if(bStart[0])
				line_ptr_arr[0]->SetStart(inters_pt_arr[0]);
			else
				line_ptr_arr[0]->SetEnd(inters_pt_arr[0]);
			if(bStart[1])
				line_ptr_arr[1]->SetStart(inters_pt_arr[1]);
			else
				line_ptr_arr[1]->SetEnd(inters_pt_arr[1]);
			line_ptr_arr[2]->SetStart(inters_pt_arr[0]);
			line_ptr_arr[2]->SetEnd(inters_pt_arr[1]);
		}
	}
};

void CSectionViewDrawing::CompleteTubeTowerFootViewSketch()
{
	if(m_pSectionView==NULL||m_pTower==NULL||m_pPartGroup==NULL)
		return;
	PARTSET ribPlateSet;
	CLDSLineTube *pDatumTube=NULL;
	CLDSPlate *pFootPlate=NULL,*pConnectPlate1=NULL,*pConnectPlate2=NULL;
	if(!RetrievedTubeTowerFoot(m_pSectionView,&pDatumTube,&pFootPlate,&pConnectPlate1,&pConnectPlate2,&ribPlateSet))
		return;
	// 1. �ҳ�������Ƶ����²�����ְ�
	typedef CLDSGeneralPlate* PLATE_PTR;
	GECS acs=m_pSectionView->UpdateUCS(m_pTower,m_pPartGroup);
	PLATE_PTR plateArr[2]={NULL,NULL};
	for(int i=0;i<2;i++)
	{
		CLDSPlate *pPlate=(i==0)?pConnectPlate1:pConnectPlate2;
		if(pPlate==NULL)
			continue;
		GEPOINT vAxisY=pDatumTube->GetPlateVertSpreadVec(pPlate);
		double cosa_y=vAxisY*acs.axis_y;
		double cosa_z=vAxisY*acs.axis_z;
		if(cosa_y>0.8)
			plateArr[0]=pPlate;
		else if(cosa_y<-0.8)
			plateArr[1]=pPlate;
	}
	for(CSectionView::PART_ITEM *pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		if(pItem->cFlag!='W')
			continue;
		CLDSPart *pPart=ribPlateSet.FromHandle(pItem->hSonPart);
		if(pPart==NULL||!pPart->IsPlate())
			continue;
		CLDSGeneralPlate *pPlate=(CLDSGeneralPlate*)pPart;
		GEPOINT vAxisY=pDatumTube->GetPlateVertSpreadVec(pPlate);
		double cosa_y=vAxisY*acs.axis_y;
		double cosa_z=vAxisY*acs.axis_z;
		if(cosa_y>0.8&&plateArr[0]==NULL)
			plateArr[0]=pPlate;
		else if(cosa_y<-0.8&&plateArr[1]==NULL)
			plateArr[1]=pPlate;
		if(plateArr[0]&&plateArr[1])
			break;
	}
	// 2. ���ƻ������ְ�
	TUBE_FOOT_FEATURE_LINE footFeature(this,pFootPlate,pDatumTube);
	for(int i=0;i<2;i++)
	{
		if(plateArr[i]==NULL)
			continue;
		if(ribPlateSet.FromHandle(plateArr[i]->handle))	//1>.�����߰�ͼ
			footFeature.DrawFootRibPlateSketch(plateArr[i],i==0);
		else											//2>.�������Ӱ���װ���˿�Ϸ�
			footFeature.CorrectFootConnectPlateSketch(plateArr[i],i==0);
	}
}

//������ͨ��ͼ��ͼ
BOOL CSectionViewDrawing::CreateSectionViewDrawing(CSectionView *pSectionView,IModel *pTower,CLDSPartGroup *pPartGroup,bool bFinalDrawing/*=true*/)	
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL)
		return FALSE;
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	//������ͼ����ϵ��Ʋ������¼�������ϵ wht 12-09-07
	//cs=m_pSectionView->UpdateUCS(pTower,pPartGroup);
	m_pPartGroup=pPartGroup;
	UpdateDCS();	//������ͼԭ�㲻�ڻ�׼�ֹ����ʱ��������ԭ��ᵼ�º���ѹ���㷨���� wjh-2014.4.09
	if(m_pSectionView->IsSlaveView())
	{
		CSlaveSectionView *pSlaveView=(CSlaveSectionView*)m_pSectionView;
		CSectionView *pMainView=pPartGroup->SonViews.GetValue(pSlaveView->m_idHostView);
		if(pMainView&&!pSlaveView->direction.IsZero()&&fabs(pSlaveView->direction.y)<EPS)
		{	//����Ӧ����CSlaveSectionView::m_idHostViewȥ����������ͼ
			UCS_STRU host_cs=pMainView->UpdateUCS(pTower,pPartGroup);
			//GEPOINT sectViewAxisZ=-pSlaveView->direction;
			//GEPOINT axis_y(-pSlaveView->direction.y,pSlaveView->direction.x,pSlaveView->direction.z);	//direction��ʱ��ת90ΪY�᷽��
			GEPOINT axis_z(-pSlaveView->direction.x,-pSlaveView->direction.y);
			normalize(axis_z);
			GEPOINT rotvec(axis_z.y,-axis_z.x);//=axis_z^GEPOINT(0,0,1);
			GEPOINT axis_x(1,0,0);
			RotateVectorAroundVector(axis_x,-1,0,rotvec);
			GEPOINT axis_y=axis_z^axis_x;
			vector_trans(axis_x,host_cs,TRUE,TRUE);
			vector_trans(axis_y,host_cs,TRUE,TRUE);
			vector_trans(axis_z,host_cs,TRUE,TRUE);
			cs.axis_x=axis_x;
			cs.axis_y=axis_y;
			cs.axis_z=axis_z;
			//project_vector(axis_y,cs.axis_z);
			//TODO:�ⲿ�ִ��������CSlaveSectionView::UpdateDCS���ظ���wjh-2016.4.12
			/*if(!axis_y.IsZero())	//����ͼ���ӷ������ʱ��Ӧ��ԭ��ͼ��������ϵ����
			{
				if(axis_z*cs.axis_z>0)	//��ͼ�淨�������߷���Ӧ�෴
					cs.axis_z*=-1.0;
				cs.axis_y=axis_y;
				normalize(cs.axis_y);
				cs.axis_x=cs.axis_y^cs.axis_z;
			}*/
		}
	}
	else if(m_pSectionView->m_iViewType==VIEW_CIRCOVERPLATE||m_pSectionView->m_iViewType==VIEW_FL)
	{	//������ͼ������Ӧת��ΪCSlateSectionView wjh-2014.4.1
		BOOL pushed=pPartGroup->SonViews.push_stack();
		CSectionView *pMainView=pPartGroup->SonViews.GetFirst();
		while(pMainView&&pMainView->m_iViewType!=VIEW_MAIN)
			pMainView=pPartGroup->SonViews.GetNext();
		if(pushed)
			pPartGroup->SonViews.pop_stack();
		if(pMainView)	//����Ӧ����CSlaveSectionView::m_idHostViewȥ����������ͼ
		{
			UCS_STRU host_cs=pMainView->UpdateUCS(pTower,pPartGroup);
			GEPOINT axis_y=host_cs.axis_y;	//����ӦΪaxis_y=host_cs.TransVFromCS(CSlaveSectionView::direction��ʱ��ת90��)
			GEPOINT axis_z=host_cs.axis_x;	//����ӦΪaxis_z=host_cs.TransVFromCS(CSlaveSectionView::direction);
			project_vector(axis_y,cs.axis_z);
			//TODO:�ⲿ�ִ��������CSlaveSectionView::UpdateDCS���ظ���wjh-2016.4.12
			if(!axis_y.IsZero())	//����ͼ���ӷ������ʱ��Ӧ��ԭ��ͼ��������ϵ����
			{
				if(axis_z*cs.axis_z>0)	//��ͼ�淨�������߷���Ӧ�෴
					cs.axis_z*=-1.0;
				cs.axis_y=axis_y;
				normalize(cs.axis_y);
				cs.axis_x=cs.axis_y^cs.axis_z;
			}
		}
	}
	m_pPartGroup=pPartGroup;
	_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	m_xEntManager.InitContext(m_pRootPart,this,STORE_ACAD);
	if(m_pRootPart==NULL)
		return FALSE;
	if(m_pRootPart->IsLinePart())
	{
		CLDSLinePart *pRootLinePart=(CLDSLinePart*)m_pRootPart;
		if(m_pSectionView->m_iViewType==VIEW_MAIN||m_pSectionView->m_iViewType==VIEW_FRONT)
		{	//����ͼ ��ͼ����ϵX�ἰԭ���������麸ͼCPartGroupDrawing��csһ���Ա���������
			f3dPoint axis_x=pRootLinePart->End()-pRootLinePart->Start();
			normalize(axis_x);
			//Z�������˼����췽���ߣ�ӦΪ����ͼ������ͼ�����Ǵ�����ͼ������ͼ�����������˴�����������ϵ wht 12-08-30
			if(fabs(cs.axis_z*axis_x)>EPS_COS)
				cs.origin=pRootLinePart->Start()-cs.axis_x*pRootLinePart->startOdd();
			/*{		
				cs.axis_y=cs.axis_z^cs.axis_x;
				cs.axis_z=cs.axis_x^cs.axis_y;
			}*/
		}
	}
	//
	if(m_pSectionView->m_iViewType==VIEW_KNODESKETCH)	//K�ڵ�����ͼ
		CreateSpecialSectionView();	//TODO:Ӧ���ٸģ�������ԭ����й淶�����ġ�wjh-2014.7.31
	else
	{	//������ͨ��ͼ�е��ӹ�������������
		if(m_pSectionView->m_iViewType==VIEW_FL&&m_pSectionView->IsSlaveView())
		{	//���ݵװ��뵱ǰ��ͼ����ϵԭ����ȷ�� wjh-2018.3.10
			CSlaveSectionView* pSlaveView=(CSlaveSectionView*)m_pSectionView;
			CLDSGeneralPlate* pDatumPlate=pSlaveView->GetDatumPlateOfFLSectView();
			CLDSLineTube* pSectTube=NULL;
			pSlaveView->IsTubeSectionView(&pSectTube);
			if(pSectTube)
			{
				xFlSectNormalOffset.vxTubeStdUpperAxisVec=pSectTube->xPosEnd-pSectTube->xPosStart;
				normalize(xFlSectNormalOffset.vxTubeStdUpperAxisVec);
				if(xFlSectNormalOffset.vxTubeStdUpperAxisVec*cs.axis_z<0)
					xFlSectNormalOffset.vxTubeStdUpperAxisVec*=-1;
			}
			if(pDatumPlate==NULL)
			{
				xFlSectNormalOffset.siMinFilterZ=-25;
				xFlSectNormalOffset.siMaxFilterZ= 25;
			}
			else
			{
				f3dPoint xOrg=pDatumPlate->ucs.origin+pDatumPlate->ucs.axis_z*pDatumPlate->GetNormOffset();;
				GEPOINT dv=xOrg-cs.origin;
				if(pDatumPlate->ucs.axis_z*cs.axis_z>0)
				{
					xFlSectNormalOffset.siDistToBottom=(short)ftoi(dv*cs.axis_z);
					xFlSectNormalOffset.siDistToUpper=xFlSectNormalOffset.siDistToBottom+(short)pDatumPlate->GetThick();
				}
				else
				{
					xFlSectNormalOffset.siDistToUpper=(short)ftoi(dv*cs.axis_z);
					xFlSectNormalOffset.siDistToBottom=xFlSectNormalOffset.siDistToUpper-(short)pDatumPlate->GetThick();
				}
				//xFlSectNormalOffset.siMinFilterZ=xFlSectNormalOffset.siDistToBottom-25;
				//xFlSectNormalOffset.siMaxFilterZ=xFlSectNormalOffset.siDistToUpper+25;
			}
		}
		PARTSET filter;
		m_pSectionView->SmartFilterNeedDrawParts(&Ta,filter);
		CLDSPlate *pRootPlate=NULL;
		for(CLDSPart *pPart=filter.GetFirst();pPart;pPart=filter.GetNext())
		{
			if(m_pRootPart->GetClassTypeId()==CLS_PLATE&&pPart==m_pRootPart)
				pRootPlate=(CLDSPlate*)pPart;	//�����Ƹְ��麸�����壬���ڱ�ע����ߴ� wht 16-03-29
			else
				CreatePartSketch(m_pSectionView->SonParts.GetValue(pPart->handle));
		}
		if(pRootPlate)
			CreatePartSketch(m_pSectionView->SonParts.GetValue(pRootPlate->handle));
		if(m_pRootPart->IsPlate() && m_pSectionView->m_iViewType==VIEW_FRONT && IsPartGroupHasKmAngleView())
		{	//��ͨ����ģ�Ǹֵĸְ��麸������ǰ��ͼ����ӿ�ģ�Ǹֵļ�����ͼ
			BOOL pushed=m_pPartGroup->SonViews.push_stack();
			for(CSectionView* pView=m_pPartGroup->SonViews.GetFirst();pView;pView=m_pPartGroup->SonViews.GetNext())
			{
				if(pView->GetViewTypeId()!=CSectionView::CLS_KAOME_ANGLE_VIEW)
					continue;
				CreateKaoMeAngleSketch((CKaoMeAngleSectionView*)pView);
			}
			m_pPartGroup->SonViews.pop_stack(pushed);
		}
	}
	if(m_pSectionView&&m_pSectionView->IsTubeTowerFootView(pTower))
	{	//�ֹ����ŵװ���ͼ�����������߰�ʾ��ͼ���޶���������� wht 18-08-24
		CompleteTubeTowerFootViewSketch();
	}
	DimAngle();
	DimCheckLine();
	if(m_pRootPart->IsLinePart())
		DimLocatePoint((CLDSLinePart*)m_pRootPart);
	//����ͼ��
	bool bDimFinished=false;
	if(pSectionView->IsHostView())
		bDimFinished=DimSectionViewSymbol2();
	if(!bDimFinished&&pSectionView->m_iViewType==VIEW_MAIN)
		DimSectionViewSymbol();
	if(m_pRootPart->IsLinePart()&&sys.part_group_map.bModulatePartGroupLen&&m_pPartGroupDrawing&&
		(m_pSectionView->m_iViewType==VIEW_MAIN||m_pSectionView->m_iViewType==VIEW_FRONT))
	{	//�����˼�Ϊ��׼����������ͼ�е�ͼԪ��������
		for(f3dCircle *pCir=GetLsCirList()->GetFirst();pCir;pCir=GetLsCirList()->GetNext())
			pCir->centre.x=m_pPartGroupDrawing->TransCoordX(pCir->centre.x);
		for(f3dArcLine *pArcLine=GetEdgeLineList()->GetFirst();pArcLine;pArcLine=GetEdgeLineList()->GetNext())
		{
			f3dPoint startPt=pArcLine->Start();
			f3dPoint endPt=pArcLine->End();
			f3dPoint center=pArcLine->Center();
			double radius=pArcLine->Radius();
			startPt.x=m_pPartGroupDrawing->TransCoordX(startPt.x);
			endPt.x=m_pPartGroupDrawing->TransCoordX(endPt.x);
			center.x=m_pPartGroupDrawing->TransCoordX(center.x);
			if(fabs(pArcLine->SectorAngle())<EPS)
				pArcLine->CreateMethod2(startPt,endPt,pArcLine->WorkNorm(),0);
			else if(pArcLine->ColumnNorm()==pArcLine->WorkNorm())	//Բ��
				pArcLine->CreateMethod1(center,startPt,pArcLine->WorkNorm(),pArcLine->SectorAngle());
			else 	//��Բ
				pArcLine->CreateEllipse(center,startPt,endPt,pArcLine->ColumnNorm(),pArcLine->WorkNorm(),radius);
		}
		for(CSizeTextDim *pSpaceDim=GetSpaceDimList()->GetFirst();pSpaceDim;pSpaceDim=GetSpaceDimList()->GetNext())
		{
			pSpaceDim->dimPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->dimPos.x);
			pSpaceDim->dimStartPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->dimStartPos.x);
			pSpaceDim->dimEndPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->dimEndPos.x);
			pSpaceDim->initDimPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->initDimPos.x);
		}
		for(CTextOnlyDrawing *pPureTextDim=GetPureTextDimList()->GetFirst();pPureTextDim;pPureTextDim=GetPureTextDimList()->GetNext())
			pPureTextDim->dimPos.x=m_pPartGroupDrawing->TransCoordX(pPureTextDim->dimPos.x);
		for(CSizeAngleDim *pAngularDim=GetAngularDimList()->GetFirst();pAngularDim;pAngularDim=GetAngularDimList()->GetNext())
		{
			pAngularDim->dimPos.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->dimPos.x);
			pAngularDim->dimStartPos.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->dimStartPos.x);
			pAngularDim->dimEndPos.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->dimEndPos.x);
			pAngularDim->vertex.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->vertex.x);
		}
		//��չ���ݵ�
		for(CTextFrame *pDataPoint=GetDataPointDimList()->GetFirst();pDataPoint;pDataPoint=GetDataPointDimList()->GetNext())
		{
			pDataPoint->dimPos.x=m_pPartGroupDrawing->TransCoordX(pDataPoint->dimPos.x);
			pDataPoint->dataPos.x=m_pPartGroupDrawing->TransCoordX(pDataPoint->dataPos.x);
			DATAPOINT_TAG* pTag=m_xDataPointTagHash.GetValue(pDataPoint->dwKeyTag);
			if(!pTag)
				continue;
			for(int i=0;i<pTag->vertexArr.GetSize();i++)
				pTag->vertexArr[i].x=m_pPartGroupDrawing->TransCoordX(pTag->vertexArr[i].x);
		}
	}
	if(m_bFinalDrawing=bFinalDrawing)
		RelayoutDrawing();
	return TRUE;
}
void CSectionViewDrawing::FinalDrawing()
{
	if(!m_bFinalDrawing)
	{
		RelayoutDrawing();
		if(m_pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_SPHERE)
			ResetPosition();
		m_bFinalDrawing=true;
	}
}
//0��ʾ����ԭ��ͼ�������䣬�����ʾ����״���б������Ż��� wjh-2018.3.7
void CSectionViewDrawing::ZoomDrawing(double fZoomCoef/*=0*/)
{
	if(fZoomCoef<EPS||fZoomCoef==1)
		return;
	m_fUser2ScrDrawScale/=fZoomCoef;
	if(m_iComponentType==CSectionSlaveDrawing::CLS_BOM_VIEW)
		return;	//�麸����ϸ����ͼ
	RelayoutDrawing(fZoomCoef);
	if(m_pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_SPHERE)
		ResetPosition();
	m_bFinalDrawing=true;
}
//<DEVELOP_PROCESS_MARK nameId="CSectionViewDrawing::DimFlCheckLine">
struct FL_CHECK_INFO 
{
	f3dPoint leftPt;
	f3dPoint rightPt;
	double angle;
};
static int compare_func1(const FL_CHECK_INFO &info1,const FL_CHECK_INFO &info2)
{	
	return compare_double(info1.angle,info2.angle);
}
//��ڵ��麸���ļ���ߣ����ڷ���֮��ľ����ע
void CSectionViewDrawing::DimFlCheckLine()
{
	if(m_pSectionView->m_iViewType==VIEW_FL)
		return;
	CSectionView::PART_ITEM* pItem=NULL;
	CLDSSphere* pSphere=(CLDSSphere*)m_pRootPart;
	ARRAY_LIST<FL_CHECK_INFO> flCheckArr;
	for(pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		if(pItem->cFlag=='S')	//�ǹ��湹��
			continue;
		m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
		if(TagPart(pItem)==NULL ||	TagPart(pItem)->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		//������ͼ�еĹ���ֹ�
		CLDSLineTube* pTube=(CLDSLineTube*)TagPart(pItem);
		CLDSParamPlate* pFl=NULL;
		f3dPoint startPt,endPt,vec;
		if(pTube->pStart->handle==pSphere->hDatumNode)
		{
			pFl=(CLDSParamPlate*)Ta.FromHandle(pTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
			startPt=pTube->Start();
			endPt=pTube->End();
		}
		else
		{	
			pFl=(CLDSParamPlate*)Ta.FromHandle(pTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
			startPt=pTube->End();
			endPt=pTube->Start();
		}
		vec=endPt-startPt;
		normalize(vec);
		if(pFl==NULL || pFl->m_iParamType!=TYPE_FL)
			continue;
		//���ݸֹܲ��Ҷ����ӷ���
		FL_CHECK_INFO flCheckInfo;
		double W=0,thick;
		pFl->GetDesignItemValue('W', &W);
		thick=pFl->GetThick();
		f3dPoint startVec,endVec,offset_vec;
		startVec=-cs.axis_y;
		endVec=pFl->ucs.axis_z;
		if(vec*endVec<0)
			endVec*=-1;
		offset_vec=endVec;
		RotateVectorAroundVector(offset_vec,-Pi*0.5,cs.axis_z);
		//�������
		flCheckInfo.leftPt=pFl->ucs.origin +0.5*W*offset_vec +thick*endVec;
		flCheckInfo.rightPt=flCheckInfo.leftPt-W*offset_vec;
		/*�����ڶ�
		flCheckInfo.leftPt=pFl->ucs.origin+0.5*W*offset_vec;
		flCheckInfo.rightPt=flCheckInfo.leftPt-W*offset_vec;
		*/
		coord_trans(flCheckInfo.leftPt, cs, FALSE);
		coord_trans(flCheckInfo.rightPt, cs, FALSE);
		flCheckInfo.leftPt.z=flCheckInfo.rightPt.z=0;
		flCheckInfo.angle=CalRotateAngle(cs.axis_z,startVec,endVec);
		flCheckArr.append(flCheckInfo);
	}
	//������ת�ǶȶԼ����Ϣ��������
	CQuickSort<FL_CHECK_INFO>::QuickSort(flCheckArr.m_pData,flCheckArr.GetSize(),compare_func1);
	FL_CHECK_INFO* pPreInfo=flCheckArr.GetFirst();
	for(FL_CHECK_INFO* pCurInfo=flCheckArr.GetNext();pCurInfo;pCurInfo=flCheckArr.GetNext())
	{
		//��Ӽ���߳ߴ�
		double fDist=DistOf3dPtLine(f3dPoint(0,0,0),pPreInfo->rightPt,pCurInfo->leftPt);
		if(fDist<=pSphere->D*0.5)
		{
			pPreInfo=pCurInfo;
			continue;
		}
		CSizeTextDim space_dim;
		space_dim.dimStartPos=pPreInfo->rightPt;
		space_dim.dimEndPos=pCurInfo->leftPt;
		space_dim.dist=DISTANCE(pPreInfo->rightPt,pCurInfo->leftPt);
		space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
		space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
		space_dim.iOrder = 0;	//װ�����߳ߴ�
		space_dim.nHits=1;
		spaceDimList.append(space_dim);
		pPreInfo=pCurInfo;
	}	
}
//</DEVELOP_PROCESS_MARK>
void CSectionViewDrawing::DimAssemblyPos()
{
	if(m_pRootPart==NULL||m_pTower==NULL||m_pSectionView==NULL)
		return;
	if(m_pSectionView->m_iViewType!=VIEW_FL)
		return;
	CLDSPart *pMainPart=m_pSectionView->GetMainPartHandle(m_pTower);
	if(pMainPart==NULL)
		return;
	CLDSParamPlate* pFl=(CLDSParamPlate*)pMainPart;
	CLDSPart *pFatherPart=m_pTower->FromPartHandle(pMainPart->m_hPartWeldParent);
	if(pFatherPart==NULL)
		return;
	CLDSLineTube* pLineTube=(CLDSLineTube*)pFatherPart;
	f3dPoint start_vec,end_vec,vec;
	start_vec=pLineTube->ucs.axis_x;
	if(pLineTube->m_bHasWeldRoad)
	{
		double angle=pLineTube->CalWeldRoadAngle();
		RotateVectorAroundVector(start_vec,angle,pLineTube->ucs.axis_z);
	}
	end_vec=cs.axis_x;
	//TODO: װ��㣺һ���ں����ߣ���һ��Ӧ�ڵ�ǰ��������ͼX�᷽��wjh-2014.3.26
	/*���װ��㣺һװ����ں����ߣ���һװ����ڻ�׼���ϣ�
		������������׼�߹��ߣ�����һװ������뺸������ת90���λ��*/
	CXhChar50 sValue;
	sValue.Copy(m_pSectionView->Symbol);
	if(sValue[0]!='A'&& sValue[1]=='1')
		return;
	double R=pLineTube->GetDiameter();
	f3dPoint orign,startPt,endPt;
	f3dPoint offset_vec;
	f3dLine dim_line;
	if(fabs(start_vec*end_vec)>0.866025403)	//cos30
	{
		end_vec=start_vec;
		RotateVectorAroundVector(end_vec,Pi/2,pLineTube->ucs.axis_z);
	}
	for(int i=0;i<2;i++)
	{
		if(i==0)
		{
			vec=start_vec;
			sValue.Copy("1");
		}
		else
		{	
			vec=end_vec;
			sValue.Copy("2");
		}
		//װ������
		orign=cs.origin+vec*R*0.5;
		offset_vec=vec;
		RotateVectorAroundVector(offset_vec,Pi/8,cs.axis_z);
		startPt=orign+offset_vec*40;
		dim_line.startPt=orign;
		dim_line.endPt=startPt;
		coord_trans(dim_line.startPt,cs,FALSE);
		coord_trans(dim_line.endPt,cs,FALSE);
		dim_line.pen.style=PS_SOLID;
		dim_line.ID=0;
		edgeList.append(LineToArc(dim_line));
		//
		offset_vec=vec;
		RotateVectorAroundVector(offset_vec,-Pi/8,cs.axis_z);
		endPt=orign+offset_vec*40;
		dim_line.startPt=orign;
		dim_line.endPt=endPt;
		coord_trans(dim_line.startPt,cs,FALSE);
		coord_trans(dim_line.endPt,cs,FALSE);
		dim_line.pen.style=PS_SOLID;
		dim_line.ID=0;
		edgeList.append(LineToArc(dim_line));
		//
		dim_line.startPt=startPt;
		dim_line.endPt=endPt;
		coord_trans(dim_line.startPt,cs,FALSE);
		coord_trans(dim_line.endPt,cs,FALSE);
		dim_line.pen.style=PS_SOLID;
		dim_line.ID=0;
		edgeList.append(LineToArc(dim_line));
		//װ��������
		CTextOnlyDrawing textDraing;
		coord_trans(orign,cs,FALSE);
		vector_trans(vec,cs,FALSE);
		textDraing.dimPos=orign;//-vec*sys.dim_map.fDimTextSize*3;
		textDraing.dimText=sValue;
		textDraing.angle=0;//CalRotateAngle(f3dPoint(0,0,1),f3dPoint(1,0,0),vec);
		textDraing.iOrder=0;
		textDraing.bNeedConvertDistToText=false;
		textDraing.bInXWing=false;
		pureTextDimList.append(textDraing);
	}
}
//<DEVELOP_PROCESS_MARK nameId="CSectionViewDrawing::CreateSphereSectionViewDrawing">
//��ڵ��麸������ͼ��ͼ
BOOL CSectionViewDrawing::CreateSphereSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	m_pSectionView=pSectionView;
	m_pTower=pTower;
	m_pPartGroup=pPartGroup;
	_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	if(m_pRootPart==NULL)
		return FALSE;
	m_pSectionView->des_cs.SetModifiedState();
	cs=m_pSectionView->UpdateUCS(pTower,pPartGroup);		//����ϵ
	CSectionView::PART_ITEM *pItem=NULL;
	for(pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
		if(TagPart(pItem)==NULL)
			continue;
		BOOL bCoplanar=TRUE;
		if(pItem->cFlag=='S')	//���ƹ���˼�
			bCoplanar=FALSE;
		CreatePartSketch(pItem,bCoplanar);
	}
	//�Ƕȱ�ע
	DimAngle();			
	if(pSectionView->m_iViewType==VIEW_MAIN||pSectionView->m_iViewType==VIEW_FRONT)
	{
		DimFlCheckLine();		//�����
		DimSectionViewSymbol();	//����ͼ��
	}
	else if(pSectionView->m_iViewType==VIEW_FL)
		DimAssemblyPos();		//װ���
	if(m_bFinalDrawing=bFinalDrawing)
	{
		RelayoutDrawing();
		ResetPosition();
	}
	return TRUE;
}
//</DEVELOP_PROCESS_MARK>
//������ͼ��С��ͼ��������
f2dRect CSectionViewDrawing::GetDrawRegion(bool bIncSectSymbol/*=false*/)
{
	f2dRect rect;
	SCOPE_STRU scope;
	double fZoomFontCoef=1;//20.0/m_fUser2ScrDrawScale; //����Ŵ��� ���������߶�Ϊ1:20ʱ������߶� wht 10-09-16
	int i=0;
	f3dArcLine *pArcLine=NULL;
	for(pArcLine=edgeList.GetFirst();pArcLine;pArcLine=edgeList.GetNext())
	{
		scope.VerifyVertex(pArcLine->Start());
		scope.VerifyVertex(pArcLine->End());
		if(pArcLine->SectorAngle()>0&&!pArcLine->WorkNorm().IsZero())
		{	//���ΪԲ�����㷶Χʱ����Բ����
			f3dPoint center=pArcLine->Center();
			double radius=pArcLine->Radius();
			scope.VerifyVertex(f3dPoint(center.x+radius,center.y));
			scope.VerifyVertex(f3dPoint(center.x,center.y+radius));
			scope.VerifyVertex(f3dPoint(center.x-radius,center.y));
			scope.VerifyVertex(f3dPoint(center.x,center.y-radius));
		}
	}
	f3dCircle *pCir=NULL;
	for(pCir=cirList.GetFirst();pCir;pCir=cirList.GetNext())
	{	//��˨�뾶���õ�������˨��λ��ĳ�������� 
		scope.VerifyVertex(f3dPoint(pCir->centre.x+pCir->radius,pCir->centre.y));
		scope.VerifyVertex(f3dPoint(pCir->centre.x,pCir->centre.y+pCir->radius));
		scope.VerifyVertex(f3dPoint(pCir->centre.x-pCir->radius,pCir->centre.y));
		scope.VerifyVertex(f3dPoint(pCir->centre.x,pCir->centre.y-pCir->radius));
	}
	f3dPoint rgnVert[4];
	for(CSizeTextDim *pSpaceDim=spaceDimList.GetFirst();pSpaceDim;pSpaceDim=spaceDimList.GetNext())
	{
		pSpaceDim->GetDimRect(rgnVert,FALSE,fZoomFontCoef);
		for(int i=0;i<4;i++)
			scope.VerifyVertex(rgnVert[i]);
	}
	//
	for(CTextOnlyDrawing *pPureTextDim=pureTextDimList.GetFirst();pPureTextDim;pPureTextDim=pureTextDimList.GetNext())
	{
		if(m_bFinalDrawing&&(sectionEnt.pText==pPureTextDim||sectionEnt.pScaleDimText==pPureTextDim))
			continue;	//������ͼ��Χʱ����������ͼ���ű��� wht 18-03-16
		pPureTextDim->GetDimRect(rgnVert,fZoomFontCoef);
		for(int i=0;i<4;i++)
			scope.VerifyVertex(rgnVert[i]);
	}
	CTextFrame *pDataPoint=NULL;
	for(pDataPoint=dataPointList.GetFirst();pDataPoint;pDataPoint=dataPointList.GetNext())
	{
		pDataPoint->GetDimRect(rgnVert,fZoomFontCoef);
		for(int i=0;i<4;i++)
			scope.VerifyVertex(rgnVert[i]);
	}

	for(CSizeAngleDim *pAngleDim=angularDimList.GetFirst();pAngleDim;pAngleDim=angularDimList.GetNext())
	{
		pAngleDim->GetDimRect(rgnVert,fZoomFontCoef);
		for(int i=0;i<4;i++)
			scope.VerifyVertex(rgnVert[i]);
	}
	for(IDbEntity* pEntity=EnumFirstDbEntity();pEntity;pEntity=EnumNextDbEntity())
	{
		if( !bIncSectSymbol&&m_bFinalDrawing&&
			((sectionEnt.pLine1&&sectionEnt.pLine1->GetId()==pEntity->GetId())||
			(sectionEnt.pLine2&&sectionEnt.pLine2->GetId()==pEntity->GetId())))
			continue;	//������ͼ��Χʱ����������ͼ���ű��� wht 18-03-16
		if(pEntity->GetDbEntType()==IDbEntity::DbLine)
		{
			scope.VerifyVertex(((IDbLine*)pEntity)->StartPosition());
			scope.VerifyVertex(((IDbLine*)pEntity)->EndPosition());
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbArcline)
			scope.VerityArcLine(((IDbArcline*)pEntity)->GetArcLine());
		else if(pEntity->GetDbEntType()==IDbEntity::DbCircle)
		{
			GEPOINT centre=((IDbCircle*)pEntity)->Center();
			double radius=((IDbCircle*)pEntity)->Radius();
			GEPOINT worknorm=((IDbCircle*)pEntity)->Normal();
			if(worknorm.IsEqual(0,0,1))//��˨�뾶���õ�������˨��λ��ĳ�������� 
			{
				scope.VerifyVertex(f3dPoint(centre.x+radius,centre.y));
				scope.VerifyVertex(f3dPoint(centre.x,centre.y+radius));
				scope.VerifyVertex(f3dPoint(centre.x-radius,centre.y));
				scope.VerifyVertex(f3dPoint(centre.x,centre.y-radius));
			}
			else
			{
				f3dArcLine circle;
				GEPOINT arcstart=centre+worknorm.Perpendicular(true)*radius;
				circle.CreateMethod1(centre,arcstart,worknorm,Pi*2);
				scope.VerityArcLine(circle);
			}
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbAngularDimension||
			pEntity->GetDbEntType()==IDbEntity::DbAlignedDimension||
			pEntity->GetDbEntType()==IDbEntity::DbRotatedDimension)
		{
			GEPOINT rgnVert[4];
			if(((IDbDimension*)pEntity)->GetDimRect(rgnVert,1.0))
			{
				for(int i=0;i<4;i++)
					scope.VerifyVertex(rgnVert[i]);
			}
		}
	}
	if((scope.fMaxX-scope.fMinX)>10000||(scope.fMaxX-scope.fMinX)<-10000)
		scope.VerifyVertex(f3dPoint(0,0,0));	//��ֹ�����޻���ͼԪ��������������������º��沼��ʱ��ѭ��(��������) wjh-2016.4.13
	if(scope.wide()>0&&scope.high()>0)
	{
		rect.topLeft.Set(scope.fMinX,scope.fMaxY);
		rect.bottomRight.Set(scope.fMaxX,scope.fMinY);
	}
	else
	{
		rect.topLeft.Set(0,0);
		rect.bottomRight.Set(0,0);
	}
	return rect;
}

void CSectionViewDrawing::AppendLine(f3dLine& line)
{
	edgeList.append(LineToArc(line));
}
//���ƺ���---��ֱ�����������ߣ���������յ�����Ϊ��ͼ����ϵ�µ����꣬widthΪ�����ǵĿ��(mm)
void CSectionViewDrawing::DrawWeldLineSymbol(f3dPoint start, f3dPoint end,double width/*=0*/,double height/*=0*/)	
{
	CSectionDrawingEntsManager manager(m_pRootPart,this);
	manager.DrawWeldLineSymbol(start,end,width,height);
}
void CSectionViewDrawing::DrawWeldLineSymbol(f3dArcLine arcline,double width/*=0*/,double height/*=0*/)
{
	CSectionDrawingEntsManager manager(m_pRootPart,this);
	manager.DrawWeldLineSymbol(arcline,width,height);
}
double CSectionViewDrawing::ZoomToModelTextSize(double fSysFontSize)	//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
{
	double fZoomAdaptive=1.0;
	if(sys.welding.bZoomAdaptiveWithScale&&sys.part_group_map.part_table.bZoomInProcessCard)
		fZoomAdaptive=sys.welding.wiScaleModel2Draw/m_fUser2ScrDrawScale;
	return fSysFontSize*fZoomAdaptive;
}
FRAMESIZE CSectionViewDrawing::ZoomToModelTextSize(FRAMESIZE& framesize)	//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
{
	FRAMESIZE frame=framesize;
	frame.fTextHeight=ZoomToModelTextSize(framesize.fTextHeight);
	frame.fFrameWidth=ZoomToModelTextSize(framesize.fFrameWidth);
	frame.fMargin    =ZoomToModelTextSize(framesize.fMargin);
	return frame;
}
//��Ҫ���LMA��׼����Ϊ�ֹܵĸְ�
void CPlateDrawing::AnalizePoleRay(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext)
{	
	UCS_STRU cs=dcs.cs;
	if(pPlate==NULL||cs.axis_x.IsZero()||cs.axis_y.IsZero()||cs.axis_z.IsZero())
		return;
	if(pPlate->vertex_list.GetNodeNum()<3)
		return;
	if(fabs(pPlate->ucs.axis_z*cs.axis_z)<EPS_COS2)
		return;	//�������ڵ�ǰ��ͼ����ʾΪ����ͼ�ĸְ�
	CLDSLineTube *pDatumLineTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumLineTube==NULL)
		return;
	int iCount=0;
	f3dLine long_edge,edge_line;		
	f3dPoint inter_pos[5];	//������������ְ������ߵĽ���
	f3dPoint startPt,endPt;
	int i=0, j=0, iRayNo=0;
	CDesignLjPartPara *pLjPart=NULL;
	f3dLine datum_line(pDatumLineTube->Start(),pDatumLineTube->End());
	f3dPoint datum_line_vec=pDatumLineTube->End()-pDatumLineTube->Start();
	normalize(datum_line_vec);
	vector_trans(datum_line_vec,cs,FALSE);	//ת�����ְ�����ϵ��
	f3dPoint ray_pole_vec[10];	//��¼���߸˼������췽�� ��������������߸˼������෴������ĵ���ƴ�ֱ������ߵ�ֱ��
	for(pLjPart=pPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pPlate->designInfo.partList.GetNext())
	{	
		if(pLjPart->hPart==pPlate->designInfo.m_hBasePart)
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPlate->BelongModel()->FromPartHandle(pLjPart->hPart,CLS_LINEANGLE,CLS_LINETUBE);
		if(pLinePart==NULL)
			continue;
		//ȷ�����߸˼�����������ֱ��
		f3dLine line(pLinePart->Start(),pLinePart->End());
		f3dPoint line_vec=pLinePart->End()-pLinePart->Start();
		normalize(line_vec);
		BOOL bDrawDatumLine=FALSE;	//�Ƿ���Ҫ���ƻ�׼��
		for(j=0;j<iRayNo;j++)
		{
			if(fabs(ray_pole_vec[j]*line_vec)>EPS_COS)
			{
				bDrawDatumLine=TRUE;
				break;
			}
		}
		ray_pole_vec[iRayNo]=line_vec;
		//���ҽǸ���Z����������С����˨(�Ǹ�����ϵ��)
		double max_z=-10000000,min_z=1000000000;
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLsRef *pLsRef=NULL;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
			pLineAngle->getUCS(pLineAngle->ucs);
			for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
			{
				if(pPlate->FindLsByHandle(pLsRef->GetLsPtr()->handle))
				{
					f3dPoint ls_pos=(*pLsRef)->ucs.origin;
					coord_trans(ls_pos,pLineAngle->ucs,FALSE);
					if(ls_pos.z>max_z)
						max_z=ls_pos.z;
					if(ls_pos.z<min_z)
						min_z=ls_pos.z;
				}
			}
		}
		if(pLjPart->start0_end1==0)
		{
			startPt=pLinePart->Start();
			endPt=pLinePart->Start()-line_vec*pLinePart->startOdd();
		}
		else 
		{
			startPt=pLinePart->End();
			endPt=pLinePart->End()+line_vec*pLinePart->endOdd();
		}
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			JGZJ jgzj;
			f3dPoint wing_vec;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
			getjgzj(jgzj,pLineAngle->GetWidth());
			if(pLjPart->angle.cur_wing_x0_y1==0)
			{
				if(pLineAngle->m_bEnableTeG)
					jgzj = pLineAngle->xWingXZhunJu;
				wing_vec = pLineAngle->GetWingVecX();
			}
			else
			{
				if(pLineAngle->m_bEnableTeG)
					jgzj = pLineAngle->xWingYZhunJu;
				wing_vec = pLineAngle->GetWingVecY();
			}
			Add_Pnt(startPt,startPt,wing_vec*jgzj.g);
			Add_Pnt(endPt,endPt,wing_vec*jgzj.g);
			double dist=max_z-min_z+80;
			if(dist<pLineAngle->GetLength()&&dist>0)
			{	//�����Ǹ����߳���
				if(pLjPart->start0_end1==0)
					endPt=endPt+line_vec*dist;
				else if(pLjPart->start0_end1==1)
					endPt=endPt-line_vec*dist;
			}
		}
		startPt=dcs.TransToDraw(startPt);	//coord_trans(startPt,cs,FALSE);
		endPt=dcs.TransToDraw(endPt);		//coord_trans(endPt,cs,FALSE);startPt.z=endPt.z=0;
		//�����߷���ת������ǰ��������
		line_vec=dcs.TransVToDraw(line_vec);//vector_trans(line_vec,cs,FALSE);
		//��ӻ�׼������
		if(bDrawDatumLine)
		{
			f3dPoint ray_vec(-line_vec.y,line_vec.x,0);
			double offset_dist=DISTANCE(startPt,endPt);
			edge_line.startPt=startPt+ray_vec*offset_dist;
			edge_line.endPt=startPt-ray_vec*offset_dist;
			edge_line.pen.style=PS_DASHDOTDOT;	//ͼ��4
			//edgeList.append(LineToArc(edge_line));
			pContext->AppendLine(LineToArc(edge_line));
		}
		BOOL bTransPlate=FALSE;
		//��ͨ����
		SCOPE_STRU scope;
		PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=pPlate->vertex_list.GetTail();
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
		if(scope.IsIncludePoint(f3dPoint(0,0,0)))
			bTransPlate=TRUE;
		//������Ӱ�������
		if(iRayNo==0&&!bTransPlate)
		{	
			iCount=0;	//��ʼ��
			f3dPoint center_line_vec(-datum_line_vec.y,datum_line_vec.x,0);
			double cosa=line_vec*center_line_vec;
			if(pLjPart->start0_end1==1)	//�ն�����
				cosa*=-1.0;	
			if(cosa<EPS)
				center_line_vec*=-1.0;
			f3dPoint center_line_end=startPt+center_line_vec*pDatumLineTube->GetDiameter();
			edge_line.startPt=startPt;
			edge_line.endPt=center_line_end;
			edge_line.pen.style=PS_DASHDOT;	//ͼ��3
			//edgeList.append(LineToArc(edge_line));
			pContext->AppendLine(LineToArc(edge_line));
			//������������ְ������ߵĽ���
			f3dLine center_line(startPt,center_line_end);
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pPrevVertex)
				{
					f3dLine cur_line(pPrevVertex->vertex,pVertex->vertex);
					coord_trans(cur_line.startPt,pPlate->ucs,TRUE);
					coord_trans(cur_line.endPt,pPlate->ucs,TRUE);
					cur_line.startPt=dcs.TransToDraw(cur_line.startPt);//coord_trans(cur_line.startPt,cs,FALSE);
					cur_line.endPt=dcs.TransToDraw(cur_line.endPt);		//coord_trans(cur_line.endPt,cs,FALSE);cur_line.startPt.z=cur_line.endPt.z=0;
					if(Int3dll(center_line,cur_line,inter_pos[iCount])>0)
					{
						long_edge=cur_line;
						iCount++;
					}
				}
				pPrevVertex=pVertex;
			}
		}
		//��ע���߽Ƕ�
		if(iCount>0)
		{
			f3dPoint inters1;
			if(Int3dll(startPt,endPt,long_edge.startPt,long_edge.endPt,inters1)>0)
			{
				f3dPoint start_vec=inters1-inter_pos[0];
				normalize(start_vec);
				f3dPoint end_vec=line_vec;
				if(pLjPart->start0_end1==1)
					end_vec*=-1.0;
				//�˴����ܴ�������չ��ת��������δ���� wjh 2011-11-09
				coord_trans(inters1,cs,TRUE);
				vector_trans(start_vec,cs,TRUE);	
				vector_trans(end_vec,cs,TRUE);
				//AppendAngularDim(inters1,start_vec,end_vec);
				pContext->AppendAngularDim(inters1,start_vec,end_vec);
			}
		}
		//��Ӹ˼�����
		edge_line.startPt=startPt;
		edge_line.endPt=endPt;
		edge_line.pen.style=PS_DASHDOTDOT;	//ͼ��4
		//edgeList.append(LineToArc(edge_line));
		pContext->AppendLine(LineToArc(edge_line));
		iRayNo++;
	}
}

//��ӽǶȱ�ע��
void CSectionViewDrawing::AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,
													 double fDimR/*=100*/,char *dimText/*=NULL*/)
{
	double cosa=start_vec*end_vec;
	//��������ظ��Ƕȱ�ע wht 10-08-08
	f3dPoint cur_start_vec=start_vec,cur_end_vec=end_vec;
	vector_trans(cur_start_vec,cs,FALSE);
	vector_trans(cur_end_vec,cs,FALSE);
	cur_start_vec.z=cur_end_vec.z=0;
	f3dPoint cur_vertex=vertex;
	coord_trans(cur_vertex,cs,FALSE);
	cur_vertex.z=0;
	for(CSizeAngleDim *pTempDim=angularDimList.GetFirst();pTempDim;pTempDim=angularDimList.GetNext())
	{
		if(pTempDim->vertex!=cur_vertex)
			continue;	//�Ƕȱ�ע���㲻�غ�
		f3dPoint temp_start_vec=pTempDim->dimStartPos-pTempDim->vertex;
		f3dPoint temp_end_vec=pTempDim->dimEndPos-pTempDim->vertex;
		normalize(temp_start_vec);
		normalize(temp_end_vec);
		double ss=temp_start_vec*cur_start_vec;
		double ee=temp_end_vec*cur_end_vec;
		double se=temp_start_vec*cur_end_vec;
		double es=temp_end_vec*cur_start_vec;
		//EPS_COS=0.999999999 �˴���ȷ�ȿɲ���̫�� wht 10-08-12
		if(ss*ee>0.99999||se*es>0.99999)	//���߷�����ͬ���෴��Ϊ�ظ��Ƕȱ�ע
			return;// pTempDim;
	}
	CSizeAngleDim *pAngularDim=angularDimList.append();
	if(fDimR<=0)	//Pi=3.14
	{
		double half_angle=0.5*ACOS(start_vec*end_vec);
		if(half_angle<1.57)	//ʵ��֤��4���ַ���������Ҫ��6���ַ��������� wjh-2018.3.6
			fDimR = TransToModelLength(ZoomToModelTextSize(sys.dim_map.fDimAngleTextSize)*0.7*6)/tan(half_angle);
	}
	//�Ƕ����ǵ�
	vector_trans(start_vec,cs,FALSE);
	vector_trans(end_vec,cs,FALSE);
	start_vec.z=end_vec.z=0;
	pAngularDim->vertex=vertex;
	coord_trans(pAngularDim->vertex,cs,FALSE);
	pAngularDim->vertex.z=0;
	//����ʼ�߱�ǵ�
	pAngularDim->dimStartPos = pAngularDim->vertex+start_vec*fDimR*0.4;
	pAngularDim->dimStartPos.z=0;
	//��ֹ�߱�ǵ�
	pAngularDim->dimEndPos = pAngularDim->vertex+end_vec*fDimR*0.4;
	//coord_trans(pAngularDim->dimEndPos,cs,FALSE);
	pAngularDim->dimEndPos.z=0;
	//��עλ��Բ����ǵ�
	//f3dPoint startPos=vertex+start_vec*fDimR;
	//f3dPoint endPos=vertex+end_vec*fDimR;
	f3dPoint dim_vec=(pAngularDim->dimStartPos+pAngularDim->dimEndPos)/2.0-pAngularDim->vertex;
	normalize(dim_vec);
	pAngularDim->dimPos=pAngularDim->vertex+dim_vec*fDimR;
	//coord_trans(pAngularDim->dimPos,cs,FALSE);
	pAngularDim->dimPos.z=0;
	//ָ����ע����
	if(dimText)
		pAngularDim->dimText.Copy(dimText);
	//return pAngularDim;
}
#include "list.h"
struct TEMP_ANGULAR_DIM{GEPOINT vertex,start_vec,end_vec;};
bool CSectionViewDrawing::CheckSameAngularDim(double* vertex,double* start_vec,double* end_vec)
{
	CXhSimpleList<TEMP_ANGULAR_DIM> listDims;
	for(CSizeAngleDim *pTempDim=angularDimList.GetFirst();pTempDim;pTempDim=angularDimList.GetNext())
	{
		TEMP_ANGULAR_DIM angular;
		angular.vertex=pTempDim->vertex;
		angular.start_vec=pTempDim->dimStartPos-pTempDim->vertex;
		angular.end_vec=pTempDim->dimEndPos-pTempDim->vertex;
		listDims.AttachNode(angular);
	}
	for(IDbEntity* pEnt=EnumFirstDbEntity();pEnt;pEnt=EnumNextDbEntity())
	{
		if(pEnt->GetDbEntType()!=IDbEntity::DbAngularDimension)
			continue;
		IDbAngularDimension* pAngularDimEnt=(IDbAngularDimension*)pEnt;
		TEMP_ANGULAR_DIM angular;
		GEPOINT line1End,line2End;
		pAngularDimEnt->GetLine1Start(angular.vertex);
		pAngularDimEnt->GetLine1End(line1End);
		pAngularDimEnt->GetLine2End(line2End);
		angular.start_vec=line1End-angular.vertex;
		angular.end_vec=line2End-angular.vertex;
		listDims.AttachNode(angular);
	}
	for(LIST_NODE<TEMP_ANGULAR_DIM>* pListNode=listDims.EnumFirst();pListNode;pListNode=listDims.EnumNext())
	{
		if(!pListNode->data.vertex.IsEqual(vertex[0],vertex[1],vertex[2]))
			continue;	//�Ƕȱ�ע���㲻�غ�
		normalize(pListNode->data.start_vec);
		normalize(pListNode->data.end_vec);
		double ss=pListNode->data.start_vec*GEPOINT(start_vec);
		double ee=pListNode->data.end_vec*GEPOINT(end_vec);
		double se=pListNode->data.start_vec*GEPOINT(end_vec);
		double es=pListNode->data.end_vec*GEPOINT(start_vec);
		//EPS_COS=0.999999999 �˴���ȷ�ȿɲ���̫�� wht 10-08-12
		if(ss*ee>0.99999||se*es>0.99999)	//���߷�����ͬ���෴��Ϊ�ظ��Ƕȱ�ע
			return true;// pTempDim;
	}
	return false;
}

//����߱�ע
void CSectionViewDrawing::DimCheckLine()
{
	if(m_pTower==NULL||m_pSectionView==NULL)
		return;
	BOOL bDimBeforeWeldCheckLine=FALSE;
	if(drawStyleFlag.IsHasFlag(CPartGroupDrawing::DimWeldPosLocateLine))
		bDimBeforeWeldCheckLine=TRUE;
	CCheckLineDimension *pCheckLine=NULL;
	CSizeTextDim space_dim, *pPrevSpaceDim=NULL;
	CHashSet<CLDSPlate*> hashBoltResidePlates;	//��ֵΪ��˨���
	for(CSectionView::PART_ITEM* pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		SmartPartPtr pCurPart=m_pSectionView->LoadTagInfo(pItem)->pCurPart;
		if(pCurPart.IsNULL())
			pCurPart=m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
		if(pCurPart.IsHasPtr()&&pCurPart->GetClassTypeId()==CLS_PLATE)
		{
			for(CLsRef* pLsRef=pCurPart.pPlate->GetFirstLsRef();pLsRef;pLsRef=pCurPart.pPlate->GetNextLsRef())
				hashBoltResidePlates.SetValue(pLsRef->GetLsPtr()->handle,pCurPart.pPlate);
		}
	}
	int AFTER_WELD_LINE_INIT_OFFSET_DIST = 50;	//�������߳�ʼƫ�ƾ���
	if(!m_pSectionView->IsMainView())
		AFTER_WELD_LINE_INIT_OFFSET_DIST = 5;
	SAFE_TEXTSIZE safeTextSize;
	SAFE_TEXTSIZE::minFontSize=sys.part_group_map.fMinFontSize;
	double text_height=safeTextSize.EnsureSize(sys.fonts.weld.fDimTextSize);
	for(pCheckLine=m_pSectionView->CheckLines.GetFirst();pCheckLine;pCheckLine=m_pSectionView->CheckLines.GetNext())
	{
		double dimDist=0;
		f3dPoint startPos,endPos,dimPos;
		CLDSPart *pStartPart=NULL, *pEndPart=NULL;
		pStartPart=m_pTower->FromPartHandle(pCheckLine->m_hStartPart);
		pEndPart=m_pTower->FromPartHandle(pCheckLine->m_hEndPart);
		f3dPoint offset_vec(1,0,0);	//��ע��ǰ�����ʱ��ƫ�Ʒ���
		CDrawCoordTrans dcs(cs);
		CDrawCoordTrans dcsEnd=dcs;
		if(pCheckLine->m_bBeforeWeld&&bDimBeforeWeldCheckLine)
		{	//����ǰ����� ��˨==���ְ塢�ֹ�
			if(pStartPart==NULL||pEndPart==NULL)
				continue;
			CLDSBolt *pBolt=NULL;
			CLDSPart *pOthPart=NULL;
			if(pStartPart->GetClassTypeId()==CLS_BOLT)
			{
				pBolt=(CLDSBolt*)pStartPart;
				pOthPart=pEndPart;
			}
			else if(pEndPart->GetClassTypeId()==CLS_BOLT)
			{
				pBolt=(CLDSBolt*)pEndPart;
				pOthPart=pStartPart;
			}
			if(pBolt&&(pOthPart->GetClassTypeId()==CLS_PLATE||pOthPart->GetClassTypeId()==CLS_LINETUBE
				||pOthPart->GetClassTypeId()==CLS_PARAMPLATE))
			{
				//��ǰ�����ƫ�Ʒ���
				for(CSectionView::PART_ITEM *pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
				{
					CLDSPlate *pPlate=(CLDSPlate*)m_pTower->FromPartHandle(pItem->hSonPart,CLS_PLATE);
					if(pPlate==NULL)
						continue;
					if(pPlate->FindLsByHandle(pBolt->handle))
					{
						f3dPoint verfiy_vec=pBolt->ucs.origin-pPlate->ucs.origin;
						normalize(verfiy_vec);
						if(verfiy_vec*cs.axis_x<0)
							offset_vec.Set(-1,0,0);
						break;
					}
				}
				f3dPoint perp,vec;
				startPos=pBolt->ucs.origin;
				if(pOthPart->GetClassTypeId()==CLS_LINETUBE)
				{
					CLDSPlate* pResidePlate=hashBoltResidePlates.GetValue(pBolt->handle);
					if(pResidePlate)
					{
						pResidePlate->GetBoltIntersPos(pBolt,startPos);
						GetDCS(pResidePlate->handle,dcs);
						dcsEnd=dcs;
					}
					double dist=0;
					CLDSLineTube *pLineTube=(CLDSLineTube*)pOthPart;
					SnapPerp(&perp,pLineTube->Start(),pLineTube->End(),startPos,&dist);
					endPos=perp;
					if(dist>(pLineTube->GetDiameter()*0.5))
					{
						vec=startPos-perp;
						normalize(vec);
						endPos=perp+vec*pLineTube->GetDiameter()*0.5;
					}
				}
				else if(pOthPart->GetClassTypeId()==CLS_PLATE)
				{	//ԭ������ϽǸ�������Ĭ�ϵļ���ߣ��󾭺�ʵԭ��ش��������ѹ�ʱ��׼ȷ�����ã��μ�PartGroup.cpp#AddDefaultCheckLines wjh-2017.10.24
					CLDSPlate *pPlate=(CLDSPlate*)pOthPart;
					vec=startPos-pPlate->ucs.origin;
					normalize(vec);
					f3dPoint basePt;
					if(vec*pPlate->ucs.axis_z>0)	//��˨λ�ڰ�������һ��
						basePt=pPlate->ucs.origin+pPlate->ucs.axis_z*(pPlate->GetThick()+pPlate->m_fNormOffset);
					else 
						basePt=pPlate->ucs.origin+pPlate->ucs.axis_z*pPlate->m_fNormOffset;
					CLDSPlate *pFatherPlate=(CLDSPlate*)m_pTower->FromPartHandle(pPlate->m_hPartWeldParent,CLS_PLATE);
					if(pFatherPlate)
					{
						//�����彻��
						f3dPoint inters_pick,inters_line_vec;
						if(Int3d2f(&inters_pick,&inters_line_vec,pFatherPlate->ucs.origin,
							pFatherPlate->ucs.axis_z,basePt,pPlate->ucs.axis_z)==1)
						{
							project_point(startPos,pFatherPlate->ucs.origin,pFatherPlate->ucs.axis_z);
							SnapPerp(&endPos,inters_pick-inters_line_vec*10000,inters_pick+inters_line_vec*10000,startPos);
						}
						else
							continue;
					}
					else
						continue;
					//endPos=startPos;
					//project_point(endPos,basePt,pPlate->ucs.axis_z);
				}
			}
			else
				continue;
		}
		else if(!pCheckLine->m_bBeforeWeld)
		{	//���Ӻ�����
			startPos=pCheckLine->m_xStartLocalCoordPos;
			endPos=pCheckLine->m_xEndLocalCoordPos;
			//ת������������ϵ��
			if(pStartPart)
			{
				if(pStartPart->GetClassTypeId()==CLS_BOLT)
					((CLDSBolt*)pStartPart)->GetUCS();
				else if(pStartPart->GetClassTypeId()==CLS_LINETUBE)
					((CLDSLineTube*)pStartPart)->BuildUCS();
				else if(pStartPart->GetClassTypeId()==CLS_LINESLOT)
					((CLDSLineSlot*)pStartPart)->BuildUCS();
				coord_trans(startPos,pStartPart->ucs,TRUE);
				if(pStartPart->GetClassTypeId()==CLS_BOLT)
				{
					CLDSPlate* pResidePlate=hashBoltResidePlates.GetValue(pStartPart->handle);
					if(pResidePlate)
					{
						pResidePlate->GetBoltIntersPos((CLDSBolt*)pStartPart,startPos);
						GetDCS(pResidePlate->handle,dcs);
					}
				}
			}
			if(pEndPart)
			{
				if(pEndPart->GetClassTypeId()==CLS_BOLT)
					((CLDSBolt*)pEndPart)->GetUCS();
				else if(pEndPart->GetClassTypeId()==CLS_LINETUBE)
					((CLDSLineTube*)pEndPart)->BuildUCS();
				else if(pEndPart->GetClassTypeId()==CLS_LINESLOT)
					((CLDSLineSlot*)pEndPart)->BuildUCS();
				coord_trans(endPos,pEndPart->ucs,TRUE);
				if(pEndPart->GetClassTypeId()==CLS_BOLT)
				{
					CLDSPlate* pResidePlate=hashBoltResidePlates.GetValue(pEndPart->handle);
					if(pResidePlate)
					{
						pResidePlate->GetBoltIntersPos((CLDSBolt*)pEndPart,endPos);
						GetDCS(pResidePlate->handle,dcsEnd);
					}
				}
			}
		}
		else 
			continue;
		dimDist=DISTANCE(startPos,endPos);
		//ת�����������ϵ��
		if(pStartPart&&pEndPart&&pStartPart->GetClassTypeId()==CLS_BOLT&&pEndPart->IsPlate())
		{	//ʼ��Ϊ��˨���ն�Ϊ�ְ壨�����Ƿ�������������ʼ����˨�������ĸְ��ͼʱ����ת�����뷨���ϵ��������㲻ƥ�䣬��Ҫ���⴦�� wjh-2017.11.27
			startPos=dcs.TransToDraw(startPos,true);
			endPos=dcs.TransToDraw(endPos,true);
		}
		else if(pStartPart&&pEndPart&&pStartPart->IsPlate()&&pEndPart->GetClassTypeId()==CLS_BOLT)
		{
			startPos=dcsEnd.TransToDraw(startPos,true);
			endPos=dcsEnd.TransToDraw(endPos,true);
		}
		else
		{
			startPos=dcs.TransToDraw(startPos,true);
			endPos=dcsEnd.TransToDraw(endPos,true);
		}
		startPos.z=endPos.z=0;
		//��Գߴ��עλ��
		dimPos=(startPos+endPos)/2.0;
		//��ӱ�ע
		space_dim.dimStartPos=startPos;
		space_dim.dimEndPos=endPos;
		space_dim.dimPos=dimPos;
		space_dim.bAllowRelayout=TRUE;
		if(!pCheckLine->m_bBeforeWeld)
		{	//�������Ӻ�����λ��
			space_dim.bAllowRelayout=m_pSectionView->IsMainView();
			//������б��ע����б�Ƕ�
			if(fabs(endPos.x-startPos.x)<EPS)
				space_dim.angle = 0.5*Pi;
			else
				space_dim.angle = atan((endPos.y-startPos.y)/(endPos.x-startPos.x));
			f3dPoint vertVec(-sin(space_dim.angle),cos(space_dim.angle));
			if(space_dim.dimPos.y>0)	//�ϲ�����
				space_dim.dimPos+=vertVec*text_height;
			else						//�²�����
				space_dim.dimPos-=vertVec*text_height;
			/*if(space_dim.dimPos.y<0)
				space_dim.dimPos.y-=AFTER_WELD_LINE_INIT_OFFSET_DIST;
			else 
				space_dim.dimPos.y+=AFTER_WELD_LINE_INIT_OFFSET_DIST;*/
		}
		else
		{	//��������ǰ�����λ��
			space_dim.dimPos+=(offset_vec*80);
			GEPOINT vertvec=endPos-startPos;
			if(fabs(vertvec.x)<EPS)
				space_dim.angle=0.5*Pi;	//��ֱ��ע
			else
			{
				if(vertvec.y<0)
					vertvec*=-1;
				normalize(vertvec);
				space_dim.angle=ACOS(vertvec.x);
			}
		}
		space_dim.iOrder=0;	//װ�����߳ߴ�
		space_dim.dist=dimDist;
		space_dim.nHits=1;
		//�����ʼ��λ�����ն�λ���غϵĳߴ��ע wht 10-04-19
		if(space_dim.dimEndPos!=space_dim.dimStartPos)
			spaceDimList.append(space_dim);
	}	
}
//<DEVELOP_PROCESS_MARK nameId="CSectionViewDrawing::DimAngle">
struct DIMANGLE_INFO{
	f3dPoint vec,inters;
	double angle;
	double fLen;
	double R;
};
static int compare_func(const DIMANGLE_INFO &dimInfo1,const DIMANGLE_INFO &dimInfo2)
{	
	return compare_double(dimInfo1.angle,dimInfo2.angle);
}
//��ע�Ƕ���
void CSectionViewDrawing::AppendDefaultAngularDimension(CAngularDimension* pAngular,CLDSLineTube* pDatumLineTube,
	const double* vWeldRoadDirect,BOOL bIncludeDatumTube,CLDSLineTube *pJointTube)
{
	CLDSPlate *pStartPlate=NULL, *pEndPlate=NULL;
	CLDSLineSlot *pStartLineSlot=NULL, *pEndLineSlot=NULL;
	CLDSLineTube *pStartLineTube=NULL, *pEndLineTube=NULL;
	CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;	
	CLDSPart *pStartPart=m_pTower->FromPartHandle(pAngular->m_hStartPart);
	CLDSPart *pEndPart=m_pTower->FromPartHandle(pAngular->m_hEndPart);
	f3dPoint axis_x1,axis_x2,verify_vec,vertice,datum_edge_vec;
	GEPOINT weld_road_vec(vWeldRoadDirect);
	f3dPoint datum_line_vec=pDatumLineTube->End()-pDatumLineTube->Start();
	normalize(datum_line_vec);
	if(pAngular->m_bStartLineIsDatumLine)//�Ի�׼��Ϊ��׼
		datum_edge_vec=pDatumLineTube->ucs.axis_x;	
	else	//�Ժ�����Ϊ��׼		
		datum_edge_vec=weld_road_vec;
	if(pStartPart==NULL&&pEndPart==NULL)
		return;
	if(pStartPart==NULL&&pEndPart)
	{	//����һ������ʱȷ������������pStartPart��
		pStartPart=pEndPart;
		pEndPart=NULL;
	}
	if(pStartPart)
	{
		if(pStartPart->GetClassTypeId()==CLS_PLATE)
			pStartPlate=(CLDSPlate*)pStartPart;
		else if(pStartPart->GetClassTypeId()==CLS_LINESLOT)
			pStartLineSlot=(CLDSLineSlot*)pStartPart;
		else if(pStartPart->GetClassTypeId()==CLS_LINETUBE)
			pStartLineTube=(CLDSLineTube*)pStartPart;
		else if(pStartPart->GetClassTypeId()==CLS_PARAMPLATE)
			pStartParamPlate=(CLDSParamPlate*)pStartPart;
	}
	if(pEndPart)
	{
		if(pEndPart->GetClassTypeId()==CLS_PLATE)
			pEndPlate=(CLDSPlate*)pEndPart;
		else if(pEndPart->GetClassTypeId()==CLS_LINESLOT)
			pEndLineSlot=(CLDSLineSlot*)pEndPart;
		else if(pEndPart->GetClassTypeId()==CLS_LINETUBE)
			pEndLineTube=(CLDSLineTube*)pEndPart;
		else if(pEndPart->GetClassTypeId()==CLS_PARAMPLATE)
			pEndParamPlate=(CLDSParamPlate*)pEndPart;
	}
	//��ע����������ְ�֮��ļнǣ���Ҫ���ʮ�ֲ�����������֮��ĽǶȱ�ע wht 10-06-30
	if((pStartParamPlate&&pEndPlate)||(pStartPlate&&pEndParamPlate))
	{	
		CLDSPlate *pPlate=pStartPlate;
		CLDSParamPlate *pParamPlate=pEndParamPlate;
		if(pStartParamPlate)
		{
			pParamPlate=pStartParamPlate;
			pPlate=pEndPlate;
		}
		AppendAngularDim(pParamPlate->ucs.origin,pPlate->ucs.axis_y,pParamPlate->ucs.axis_y,pAngular->m_fDimR);
	}
	PROFILE_VERTEX* pVertex;
	//��ע�ְ���ְ�֮��н��Լ��ְ�����ʼ��֮��ļн�
	if(pStartPlate||pEndPlate)
	{
		//���ݸְ����ļ��������췽��
		f3dPoint angle_vertex,axis_z;
		if(pStartPlate)
		{	
			axis_z=pStartPlate->ucs.axis_z;
			if(pStartPlate->designInfo.iFaceType==6)
				axis_z=pStartPlate->GetHuoquFaceNorm(0);
			if((bIncludeDatumTube||pJointTube)&&fabs(axis_z*datum_line_vec)>EPS)
				return;	//��׼�ְ岻�Ǹֹ������
			if(pStartPlate->vertex_list.GetNodeNum()<=0)
				return;	//�Ƿ��ְ�
			//TODO:δ���������Ժ������GetPlateVertSpreadVec�������¼���axis_x1�Ĵ��롡wjh-2016.11.1
			//axis_x1=pDatumLineTube->GetPlateVertSpreadVec(pStartPlate);
			axis_x1=pStartPlate->ucs.axis_z^datum_line_vec;
			normalize(axis_x1);
			//���������λ��
			for(pVertex=pStartPlate->vertex_list.GetFirst();pVertex;pVertex=pStartPlate->vertex_list.GetNext())
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pStartPlate->ucs,TRUE);
				verify_vec+=vertice;
			}
			//���ݰ�����ĵ���������췽��
			verify_vec/=pStartPlate->vertex_list.GetNodeNum();
			project_point(verify_vec,pStartPlate->ucs.origin,pStartPlate->ucs.axis_z);
			verify_vec-=pStartPlate->ucs.origin;
			normalize(verify_vec);
			//�����췽��
			if(axis_x1*verify_vec<0)
				axis_x1*=-1.0;
			//���ֹ�����ϵԭ�����亸�Ӹ���������ͶӰ wht 10-08-12
			angle_vertex=pStartPlate->ucs.origin;
			if(pStartPlate->m_hPartWeldParent==pDatumLineTube->handle)
				SnapPerp(&angle_vertex,pDatumLineTube->Start(),pDatumLineTube->End(),angle_vertex);
			else 
			{
				CLDSLineTube *pFatherTube=(CLDSLineTube*)m_pTower->FromPartHandle(pStartPlate->m_hPartWeldParent,CLS_LINETUBE);
				if(pFatherTube)
					SnapPerp(&angle_vertex,pFatherTube->Start(),pFatherTube->End(),angle_vertex);
			}
		}
		if(pEndPlate)
		{	
			axis_z=pStartPlate->ucs.axis_z;
			if(pEndPlate->designInfo.iFaceType==6)
				axis_z=pEndPlate->GetHuoquFaceNorm(0);
			if((bIncludeDatumTube||pJointTube)&&fabs(axis_z*datum_line_vec)>EPS)
				return;	//��׼�ְ岻�Ǹֹ������
			if(pEndPlate->vertex_list.GetNodeNum()<=0)
				return;	//�Ƿ��ְ�
			axis_x2=pEndPlate->ucs.axis_z^datum_line_vec;
			normalize(axis_x2);
			verify_vec.Set();//��ʼ��
			//���������λ��
			for(pVertex=pEndPlate->vertex_list.GetFirst();pVertex;pVertex=pEndPlate->vertex_list.GetNext())
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pEndPlate->ucs,TRUE);
				verify_vec+=vertice;
			}
			//���ݰ�����ĵ���������췽��
			verify_vec/=pEndPlate->vertex_list.GetNodeNum();
			project_point(verify_vec,pEndPlate->ucs.origin,pEndPlate->ucs.axis_z);
			verify_vec-=pEndPlate->ucs.origin;
			normalize(verify_vec);
			//�����췽��
			if(axis_x2*verify_vec<0)
				axis_x2*=-1.0;
		}
		if(pStartPlate&&pEndPlate)
		{	//��ע����֮��ļнǣ����ұ�ע����һ���ְ���ֹܻ�׼�߻��ߺ�����֮��ļн�
			//��ע����֮��ļн�
			//if(pStartPlate->designInfo.iFaceType==6&&face_N>1)
			int i,nFaceN=1;
			if(pStartPlate->designInfo.iFaceType==6)
				nFaceN=pStartPlate->face_N;
			AppendAngularDim(angle_vertex,axis_x1,axis_x2,pAngular->m_fDimR);
			for(i=0;i<nFaceN;i++)
			{
				if(pStartPlate->designInfo.iFaceType==6)
				{
					GEPOINT axis_x=axis_x1;
					GEPOINT vWorkNorm=pStartPlate->ucs.axis_z;
					if(i>0)
						vWorkNorm=pStartPlate->GetHuoquFaceNorm(i-1);
					if(fabs(vWorkNorm*datum_line_vec)>EPS2)//�����淨����ֹ����򲻴�ֱ����ע�Ƕ�������
						continue;
					if(i==0)
						continue;
					axis_x=vWorkNorm^datum_line_vec;
					if(axis_x*axis_x1<0)
						axis_x*=-1.0;
					AppendAngularDim(angle_vertex,datum_edge_vec,axis_x,pAngular->m_fDimR*0.8);
					continue;
				}
				//��ע���׼��/�����߽Ͻ���һ������׼��/������֮��ļн�
				if(fabs(datum_edge_vec*axis_x1)<EPS_COS2&&fabs(datum_edge_vec*axis_x2)<EPS_COS2)
				{	//����ְ�����ڻ�׼��/������λ��ʱ����Ҫ��ע�����׼��/������֮��ļн�
					//��ֹ�߷���
					verify_vec=axis_x1;
					double dd1=datum_edge_vec*axis_x1;
					double dd2=datum_edge_vec*axis_x2;
					//ͬ��ȡ����ֵС�� ���ȡ����ֵΪ����
					if((dd1*dd2>0&&dd1>dd2)||(dd1*dd2<0&&dd2>0))
						verify_vec=axis_x2;
					AppendAngularDim(angle_vertex,datum_edge_vec,verify_vec,pAngular->m_fDimR*0.8);
				}
			}
			if(pEndPlate->designInfo.iFaceType==6)
			{	//��ע��繹���۵����������滷��λ�Ƕȡ�wjh-2016.11.1
				for(int i=0;i<pEndPlate->face_N;i++)
				{
					GEPOINT axis_x=axis_x2;
					GEPOINT vWorkNorm=pEndPlate->ucs.axis_z;
					if(i>0)
						vWorkNorm=pEndPlate->GetHuoquFaceNorm(i-1);
					if(fabs(vWorkNorm*datum_line_vec)>EPS2)//�����淨����ֹ����򲻴�ֱ����ע�Ƕ�������
						continue;
					if(i==0)
						continue;
					axis_x=vWorkNorm^datum_line_vec;
					if(axis_x*axis_x1<0)
						axis_x*=-1.0;
					AppendAngularDim(angle_vertex,datum_edge_vec,axis_x,pAngular->m_fDimR*0.8);
					continue;
				}
			}
		}
		else if(pStartPlate&&pEndPlate==NULL)
		{	//ֻ��һ���׼�ְ� ��ע��׼�ְ���ֹܻ�׼�߻��߸ֹܺ�����֮��ļн�
			if((pStartPlate&&fabs(datum_edge_vec*axis_x1)>EPS_COS2))
				return;	//�ְ��ڻ�׼����ʱ���ñ�ע�Ƕ�
			if(pStartPlate->designInfo.iFaceType==6)
			{	//ֻ���۵���ʱ����ע�ֹܻ�׼�ߵ���������ļн�
				for(int i=1;i<pStartPlate->face_N;i++)
				{
					GEPOINT vWorkNorm=pStartPlate->GetHuoquFaceNorm(i-1);
					if(fabs(vWorkNorm*datum_line_vec)>EPS2)//�����淨����ֹ����򲻴�ֱ����ע�Ƕ�������
						continue;
					GEPOINT axis_x=vWorkNorm^datum_line_vec;
					if(axis_x*axis_x1<0)
						axis_x*=-1.0;
					AppendAngularDim(angle_vertex,datum_edge_vec,axis_x,pAngular->m_fDimR*0.8);
				}
			}
			else
				AppendAngularDim(angle_vertex,datum_edge_vec,axis_x1,pAngular->m_fDimR);
		}
	}
	//��ע�Ŷ���Ŷ�֮��н��Լ��Ŷ�����ʼ��֮��ļн�
	if(pStartLineSlot||pEndLineSlot)
	{
		//�Ŷ����߷���
		f3dPoint dim_origin;
		CLDSBolt *pStartBolt=NULL, *pEndBolt=NULL;
		CLDSBolt startBoltTemp,endBoltTemp;
		if(pStartLineSlot)
		{	
			pStartLineSlot->BuildUCS();
			if(!pStartLineSlot->IsBaseSlot())
				return;
			if(pStartLineSlot->GetLsCount()==0)
			{
				startBoltTemp.ucs.origin=0.5*(pStartLineSlot->Start()+pStartLineSlot->End());
				startBoltTemp.ucs.axis_z=pStartLineSlot->WorkPlaneNorm();
				startBoltTemp.m_dwFlag.SetFlagDword(CLDSBolt::FOOT_NAIL);
				pStartBolt=&startBoltTemp;
			}
			else
				pStartBolt=pStartLineSlot->GetFirstLsRef()->GetLsPtr();
			if(pStartBolt==NULL||(pStartBolt&&!pStartBolt->IsFootNail()))
				return;
			pStartBolt->GetUCS();
			axis_x1=pStartBolt->get_norm();
			normalize(axis_x1);
		}
		if(pEndLineSlot)
		{	
			pEndLineSlot->BuildUCS();
			if(!pEndLineSlot->IsBaseSlot())
				return;
			if(pEndLineSlot->GetLsCount()==0)
			{
				endBoltTemp.ucs.origin=0.5*(pEndLineSlot->Start()+pEndLineSlot->End());
				endBoltTemp.ucs.axis_z=pEndLineSlot->WorkPlaneNorm();
				endBoltTemp.m_dwFlag.SetFlagDword(CLDSBolt::FOOT_NAIL);
				pEndBolt=&endBoltTemp;
			}
			else
				pEndBolt=pEndLineSlot->GetFirstLsRef()->GetLsPtr();
			if(pEndBolt==NULL||(pEndBolt&&!pEndBolt->IsFootNail()))
				return;
			pEndBolt->GetUCS();
			axis_x2=pEndBolt->get_norm();
			normalize(axis_x2);
		}
		if(pStartBolt&&pEndBolt)
		{	//��ע���Ŷ�֮��ļнǣ����ұ�ע����һ���Ŷ���ֹܻ�׼�߻��ߺ�����֮��ļн�
			//��ע���Ŷ�֮��ļн�
			//�Ƕ����ǵ�
			SnapPerp(&dim_origin,pDatumLineTube->Start(),pDatumLineTube->End(),pStartBolt->ucs.origin);
			AppendAngularDim(dim_origin,axis_x1,axis_x2,pAngular->m_fDimR);
			//��ע���׼��/�����߽Ͻ��ĽŶ����׼��/������֮��ļн�
			if(fabs(datum_edge_vec*axis_x1)<EPS_COS2&&fabs(datum_edge_vec*axis_x2)<EPS_COS2)
			{	//���Ŷ������ڻ�׼��/������λ��ʱ����Ҫ��ע�Ŷ����׼��/������֮��ļн�
				//�Ƕ����ǵ�
				SnapPerp(&dim_origin,pDatumLineTube->Start(),pDatumLineTube->End(),pStartBolt->ucs.origin);
				//��ֹ�߱�ǵ㷽��
				verify_vec=axis_x1;
				double dd1=datum_edge_vec*axis_x1;
				double dd2=datum_edge_vec*axis_x2;
				//ͬ��ȡ����ֵС�� ���ȡ����ֵΪ����
				if((dd1*dd2>0&&dd1>dd2)||(dd1*dd2<0&&dd2>0))
					verify_vec=axis_x2;
				AppendAngularDim(dim_origin,datum_edge_vec,verify_vec,pAngular->m_fDimR*0.8);
			}
		}
		else if((pStartBolt&&pEndBolt==NULL)||(pStartBolt==NULL&&pEndBolt))
		{	//ֻ��һ����׼�Ŷ� ��ע��׼�Ŷ���ֹܻ�׼�߻��߸ֹܺ�����֮��ļн�
			if(pStartBolt==NULL)
				pStartBolt=pEndBolt;
			if((pStartBolt&&fabs(datum_edge_vec*axis_x1)>EPS_COS2))
				return;	//�Ŷ��ڻ�׼����ʱ���ñ�ע�Ƕ�
			//�Ƕ����ǵ�
			SnapPerp(&dim_origin,pDatumLineTube->Start(),pDatumLineTube->End(),pStartBolt->ucs.origin);
			AppendAngularDim(dim_origin,datum_edge_vec,axis_x1,pAngular->m_fDimR);
		}
	}
	//��ע�ֹ���ֹ�֮��н�
	f3dLine datum_line(pDatumLineTube->pStart->Position(false),pDatumLineTube->pEnd->Position(false));
	if(pStartLineTube||pEndLineTube)
	{	
		f3dPoint near_axis_vec;
		pStartLineTube->BuildUCS();
		if(datum_line.PtInLine(pStartLineTube->pStart->Position(false))>0)		//ʼ������
			axis_x1=pStartLineTube->End()-pStartLineTube->Start();
		else if(datum_line.PtInLine(pStartLineTube->pEnd->Position(false))>0)	//�ն�����
			axis_x1=pStartLineTube->Start()-pStartLineTube->End();
		else
			return;
		vector_trans(axis_x1,m_pSectionView->ucs,FALSE);
		normalize(axis_x1);
		axis_x1.z=0;
		if(pEndLineTube)
		{	//����ֹ����췽��
			if(datum_line.PtInLine(pEndLineTube->pStart->Position(false))>0)	//ʼ������
				axis_x2=pEndLineTube->End()-pEndLineTube->Start();
			else if(datum_line.PtInLine(pEndLineTube->pEnd->Position(false))>0)	//�ն�����
				axis_x2=pEndLineTube->Start()-pEndLineTube->End();
			else
				return;
			vector_trans(axis_x2,m_pSectionView->ucs,FALSE);
			normalize(axis_x2);
			axis_x2.z=0;
			double fAngle1=Cal2dLineAng(f2dPoint(0,0),f2dPoint(axis_x1.x,axis_x1.y));
			double fAngle2=Cal2dLineAng(f2dPoint(0,0),f2dPoint(axis_x2.x,axis_x2.y));
			if(fabs(fAngle1-fAngle2)<EPS2)
				near_axis_vec=axis_x1;
			else
			{	//��ע���ֹ�֮��ļн�
				if(fAngle1<fAngle2)
					near_axis_vec=axis_x1;
				else
					near_axis_vec=axis_x2;
				vector_trans(axis_x1,m_pSectionView->ucs,TRUE);
				vector_trans(axis_x2,m_pSectionView->ucs,TRUE);
				AppendAngularDim(m_pSectionView->ucs.origin,axis_x1,axis_x2,pAngular->m_fDimR);
			}
		}
		else
			near_axis_vec=axis_x1;
		//��ע���׼�߽Ͻ��ĸֹ����׼��֮��ļн�
		vector_trans(near_axis_vec,m_pSectionView->ucs,TRUE);
		AppendAngularDim(m_pSectionView->ucs.origin,near_axis_vec,datum_edge_vec,pAngular->m_fDimR);
	}
}
void CSectionViewDrawing::DimAngle()
{
	if(m_pRootPart==NULL||m_pTower==NULL||m_pSectionView==NULL)
		return;
	if(m_pRootPart->GetClassTypeId()==CLS_LINETUBE)
	{	//��׼����Ϊ�ֹ�ʱ�ĽǶȱ�ע	
		CLDSLineTube *pDatumLineTube=(CLDSLineTube*)m_pRootPart;
		if(m_pSectionView->GetViewTypeId()==CSectionView::CLS_SLAVE_NODESECT_VIEW)
			pDatumLineTube=(CLDSLineTube*)Ta.FromPartHandle(((CSlaveNodeSectionView*)m_pSectionView)->m_hVertRod,CLS_LINETUBE);
		else if(pSectionView->IsSlaveView())
		{
			if(((CSlaveSectionView*)pSectionView)->m_hRootRod>0)
				pDatumLineTube=(CLDSLineTube*)m_pTower->FromPartHandle(((CSlaveSectionView*)pSectionView)->m_hRootRod,CLS_LINETUBE);
		}
		if(pDatumLineTube==NULL)
			pDatumLineTube=(CLDSLineTube*)m_pRootPart;
		CSizeAngleDim angular_dim;
		f3dPoint datum_line_vec=pDatumLineTube->End()-pDatumLineTube->Start();
		BOOL bIncludeDatumTube=FALSE;		//��ǰ��ͼ���Ƿ������׼�ֹ� 
		CSectionView::PART_ITEM *pPartItem=NULL;
		for(pPartItem=m_pSectionView->SonParts.GetFirst();pPartItem;pPartItem=m_pSectionView->SonParts.GetNext())
		{
			if(pPartItem->hSonPart==m_pRootPart->handle)
			{
				bIncludeDatumTube=TRUE;	
				break;
			}
		}
		CLDSLineTube *pJointTube=NULL;	//�������׼�ֹܶԽ����ĸֹ� wht 10-08-12
		if(!bIncludeDatumTube)
		{
			for(pPartItem=m_pSectionView->SonParts.GetFirst();pPartItem;pPartItem=m_pSectionView->SonParts.GetNext())
			{	//ͳ�������иְ�Ľڵ���Ŀ�Լ������ĸְ�
				CLDSPart *pTempPart=m_pTower->FromPartHandle(pPartItem->hSonPart);
				if(pTempPart==NULL)
					continue;
				if(pTempPart->GetClassTypeId()!=CLS_LINETUBE)
					continue;
				CLDSLineTube *pTempTube=(CLDSLineTube*)pTempPart;
				if(pDatumLineTube->m_tJointStart.hLinkObj!=pTempTube->handle
					&&pDatumLineTube->m_tJointEnd.hLinkObj!=pTempTube->handle)
					continue;
				pJointTube=pTempTube;
				pJointTube->BuildUCS();
				datum_line_vec=pJointTube->End()-pJointTube->Start();
				break;
			}
		}
		normalize(datum_line_vec);
		PROFILE_VERTEX *pVertex=NULL;
		f3dPoint weld_road_vec;
		if( m_pSectionView->m_iViewType==VIEW_FL && pDatumLineTube->m_bHasWeldRoad &&
			fabs(pDatumLineTube->ucs.axis_z*m_pSectionView->ucs.axis_z)>EPS_COS)
		{	//����ͼ ������ں��������ע���������׼��֮��ļн�
			double weldRoadAngle=pDatumLineTube->CalWeldRoadAngle();
			weld_road_vec.Set(cos(weldRoadAngle),sin(weldRoadAngle));
			vector_trans(weld_road_vec,pDatumLineTube->ucs,TRUE);	//�ɻ�׼�ֹ��������ϵתΪȫ������ϵ

			double degWeldRoadAngle=weldRoadAngle*DEGTORAD_COEF;
			if(fabs(degWeldRoadAngle-ftol(degWeldRoadAngle))>1)	//���������׼�ߴ���1��ʱ���б�Ҫ��ע������ wjh-2016.10.13
				AppendAngularDim(pDatumLineTube->ucs.origin,pDatumLineTube->ucs.axis_x,weld_road_vec,pDatumLineTube->GetDiameter()*0.9);
			else	//���к��������������׼�߻����ص�
				weld_road_vec.Set();
		}
		
		f3dLine datum_line(pDatumLineTube->pStart->Position(false),pDatumLineTube->pEnd->Position(false));
		GEPOINT datum_axis_vec=pDatumLineTube->End()-pDatumLineTube->Start();
		normalize(datum_axis_vec);
		double justify=cs.axis_z*datum_axis_vec;
		if(fabs(justify)>EPS_COS2)//||fabs(justify)<EPS2)
		{	//�ݲ�֧��ƽ�Ÿֹ�ͼ�еĽǶ��Ż�����
			GEPOINT vDimStartEdgeVec=pDatumLineTube->ucs.axis_x;	//Ĭ��fabs(justify)>EPS2,�ֹ���ֱ����ͼ
			GEPOINT center=cs.origin;
			if(pSectionView->SonParts.GetValue(pDatumLineTube->handle)!=NULL)
				Int3dlf(center,pDatumLineTube->Start(),datum_axis_vec,cs.origin,cs.axis_z);
			CDimAngularManager angulars(ZoomToModelTextSize(sys.fonts.weld.fDimAngleTextSize));
			ARRAY_LIST<RADIAL_ITEM> radialItems(0,m_pSectionView->Angulars.GetNodeNum()*2);
			CHashSet<RADIAL_ITEM*>hashRadialItems;	//��ֵΪ���ߴ���Ĺ������
			for(CAngularDimension *pAngular=m_pSectionView->Angulars.GetFirst();pAngular;pAngular=m_pSectionView->Angulars.GetNext())
			{
				SmartPartPtr pStartPart=m_pTower->FromPartHandle(pAngular->m_hStartPart);
				SmartPartPtr pEndPart=m_pTower->FromPartHandle(pAngular->m_hEndPart);
				if(pStartPart.IsNULL() && pEndPart.IsNULL())
					continue;
				//��ע����������ְ�֮��ļнǣ���Ҫ���ʮ�ֲ�����������֮��ĽǶȱ�ע wht 10-06-30
				if(pStartPart.IsHasPtr()&&(pStartPart->IsPlate()||pStartPart->GetClassTypeId()==CLS_LINESLOT)&&
					pEndPart.IsHasPtr()&&(pEndPart->IsPlate()||pEndPart->GetClassTypeId()==CLS_LINESLOT))
				{
					RADIAL_ITEM* pStartItem=NULL,*pEndItem=NULL;
					if((pStartItem=hashRadialItems.GetValue(pAngular->m_hStartPart))==NULL)
					{
						pStartItem=radialItems.append();
						hashRadialItems.SetValue(pAngular->m_hStartPart,pStartItem);
						pStartItem->hObj=pAngular->m_hStartPart;
						pStartItem->rayOrg=center;
						if(pStartPart->GetClassTypeId()==CLS_LINESLOT)
							pStartItem->rayVec=pStartPart.pSlot->WorkPlaneNorm();
						else if(pStartPart->GetClassTypeId()==CLS_PLATE)
						{
							pStartItem->rayVec=pDatumLineTube->GetPlateVertSpreadVec(pStartPart.pPlate);
							GEPOINT rayVec=pStartItem->rayVec;
							if(pStartPart.pPlate->designInfo.iFaceType==6)
							{	//��ע��繹���۵����������滷��λ�Ƕȡ�wjh-2016.11.1
								int index=0;
								BYTE byteConstArr[3]={0x01,0x02,0x04};
								PART_DIMnDRAW* pDimDraw=m_pPartGroupDrawing->GetPartDimDraw(m_pSectionView->Id,pStartPart->handle);
								for(int i=0;i<pStartPart.pPlate->face_N;i++)
								{
									if(pDimDraw&&!(pDimDraw->cbDrawPlanes&byteConstArr[i]))
										continue;
									GEPOINT rayVec=pStartItem->rayVec;
									GEPOINT vWorkNorm=(i==0)?pStartPart.pPlate->ucs.axis_z:pStartPart.pPlate->GetHuoquFaceNorm(i-1);
									if(fabs(vWorkNorm*datum_axis_vec)>EPS2)//�����淨����ֹ����򲻴�ֱ����ע�Ƕ�������
										continue;
									if(index>0)
									{
										pStartItem=radialItems.append();
										//TODO:δ�����ͬһ�����������ϻ���λ�ǵ����
										//hashRadialItems.SetValue(pAngular->m_hStartPart,pStartItem);
										pStartItem->hObj=pAngular->m_hEndPart;
										pStartItem->rayOrg=center;
									}
									pStartItem->rayVec=vWorkNorm^datum_line_vec;
									if(pStartItem->rayVec*rayVec<0)
										pStartItem->rayVec*=-1.0;
									index++;
								}
							}
						}
						else	//��������
							pStartItem->rayVec=pStartPart->ucs.axis_y;
					}
					if((pEndItem=hashRadialItems.GetValue(pAngular->m_hEndPart))==NULL)
					{
						pEndItem=radialItems.append();
						hashRadialItems.SetValue(pAngular->m_hEndPart,pEndItem);
						pEndItem->hObj=pAngular->m_hEndPart;
						pEndItem->rayOrg=center;
						if(pEndPart->GetClassTypeId()==CLS_LINESLOT)
							pEndItem->rayVec=pEndPart.pSlot->WorkPlaneNorm();
						else if(pEndPart->GetClassTypeId()==CLS_PLATE)
						{
							pEndItem->rayVec=pDatumLineTube->GetPlateVertSpreadVec(pEndPart.pPlate);
							if(pEndPart.pPlate->designInfo.iFaceType==6)
							{	//��ע��繹���۵����������滷��λ�Ƕȡ�wjh-2016.11.1
								int index=0;
								BYTE byteConstArr[3]={0x01,0x02,0x04};
								PART_DIMnDRAW* pDimDraw=m_pPartGroupDrawing->GetPartDimDraw(m_pSectionView->Id,pStartPart->handle);
								for(int i=0;i<pEndPart.pPlate->face_N;i++)
								{
									if(pDimDraw&&!(pDimDraw->cbDrawPlanes&byteConstArr[i]))
										continue;
									GEPOINT rayVec=pEndItem->rayVec;
									GEPOINT vWorkNorm=(i==0)?pEndPart.pPlate->ucs.axis_z:pEndPart.pPlate->GetHuoquFaceNorm(i-1);
									if(fabs(vWorkNorm*datum_axis_vec)>EPS2)//�����淨����ֹ����򲻴�ֱ����ע�Ƕ�������
										continue;
									if(index>0)
									{
										pEndItem=radialItems.append();
										//TODO:δ�����ͬһ�����������ϻ���λ�ǵ����
										//hashRadialItems.SetValue(pAngular->m_hEndPart,pEndItem);
										pEndItem->hObj=pAngular->m_hEndPart;
										pEndItem->rayOrg=center;
									}
									pEndItem->rayVec=vWorkNorm^datum_line_vec;
									if(pEndItem->rayVec*rayVec<0)
										pEndItem->rayVec*=-1.0;
									index++;
								}
							}
						}
						else	//��������
							pEndItem->rayVec=pEndPart->ucs.axis_y;
					}
					if(pStartItem==NULL)
					{
						pStartItem=pEndItem;
						pEndItem=NULL;
						pStartPart=pEndPart;
						pEndPart=NULL;
					}
				}
				else
					AppendDefaultAngularDimension(pAngular,pDatumLineTube,weld_road_vec,bIncludeDatumTube,pJointTube);
			}
			GEPOINT vTubeLenVec;
			if(tube_base_line.m_pBaseTube)
				vTubeLenVec=tube_base_line.m_pBaseTube->End()-tube_base_line.m_pBaseTube->Start();
			if(!tube_base_line.m_xTaCoreVec.IsZero())
			{	//��Դ��ϰ����ֱ����ָ�����ĵĺ�����Ϊ����λ��׼ wjh-2018.3.7
				vDimStartEdgeVec=tube_base_line.m_xTaCoreVec;
				vector_trans(vDimStartEdgeVec,cs,TRUE);
			}
			//����vDimStartEdgeVecֵΪģ������ wjh-2018.3.8
			angulars.CreateAngularItems(cs.axis_z,vDimStartEdgeVec,vTubeLenVec,radialItems.m_pData,radialItems.GetSize());
			double fExistAngleDimR=0;
			for(CDimAngularManager::ANGULARDIM_ITEM* pDimItem=angulars.EnumFirstDimItem();pDimItem;pDimItem=angulars.EnumNextDimItem())
			{
				double fScaleDimRadius=TransToModelLength(pDimItem->fDimRadius);
				fExistAngleDimR=max(fExistAngleDimR,fScaleDimRadius);
				AppendAngularDim(pDimItem->vTipPoint,pDimItem->vStartVec,pDimItem->vEndVec,fScaleDimRadius);
			}
			//������������ģ��������ߵĶ�λ�Ƕȱ�ע���������ɣ����ǵ����ķ���Ҫ�󲢲���ȷһ��ֻ��ʾ���ԣ�����Ҫ�������Ƕ�ֵ wjh-2018.3.6
			/*if(!tube_base_line.m_xTaCoreVec.IsZero())
			{	//��Ӹֹܻ�׼����ָ�����ķ���ĽǶȱ�ע��
				GEPOINT vDimEndVec=tube_base_line.m_xTaCoreVec;
				vector_trans(vDimEndVec,cs,TRUE);
				double increment=TransToModelLength(angulars.m_fDimAngleTextSize*2);
				AppendAngularDim(center,vDimStartEdgeVec,vDimEndVec,fExistAngleDimR+increment);
			}*/
			//
		}
		else
		{
		//logerr.Log("%s���麸����{%s}��ͼ�Ƕȱ�ע,���޷��ҵ�������ʼ��׼��,���ܲ�׼ȷ!",
		//	(char*)m_pRootPart->GetPartNo(),GetSectionView()->sName);
			for(CAngularDimension *pAngular=m_pSectionView->Angulars.GetFirst();pAngular;pAngular=m_pSectionView->Angulars.GetNext())
				AppendDefaultAngularDimension(pAngular,pDatumLineTube,weld_road_vec,bIncludeDatumTube,pJointTube);
		}
	}
	else if(m_pRootPart->GetClassTypeId()==CLS_PLATE)//||m_pRootPart->GetClassTypeId()==CLS_PARAMPLATE)
	{	//��׼����Ϊ�ְ�ĽǶȱ�ע  ��ע�ְ���ְ�֮��ļн�
		CLDSPlate *pDatumPlate=(CLDSPlate*)m_pRootPart;
		for(CAngularDimension *pAngular=m_pSectionView->Angulars.GetFirst();pAngular;pAngular=m_pSectionView->Angulars.GetNext())
		{
			CLDSPlate *pStartPlate=NULL, *pEndPlate=NULL;
			pStartPlate=(CLDSPlate*)m_pTower->FromPartHandle(pAngular->m_hStartPart,CLS_PLATE);
			pEndPlate=(CLDSPlate*)m_pTower->FromPartHandle(pAngular->m_hEndPart,CLS_PLATE);
			if(pStartPlate==NULL||pEndPlate==NULL)
				continue;
			PROFILE_VERTEX *pVertex=NULL;
			CLDSGroupLineAngle *pGroupLineAngle=NULL;
			pGroupLineAngle=(CLDSGroupLineAngle*)m_pTower->FromPartHandle(pDatumPlate->designInfo.m_hBasePart,CLS_GROUPLINEANGLE);
			if((pStartPlate->handle==m_pRootPart->handle||pEndPlate->handle==m_pRootPart->handle)&&pGroupLineAngle)
			{	//��ע��ϽǸ����֮��ļн�
				f3dPoint len_vec,datum_line_vec,axis_x,verify_vec,vertice;
				len_vec=pGroupLineAngle->End()-pGroupLineAngle->Start();
				normalize(len_vec);
				if(pEndPlate->handle==m_pRootPart->handle)
				{	//��֤��׼�ְ����pStartPlate��
					CLDSPlate *pTempPlate=pStartPlate;
					pStartPlate=pEndPlate;
					pEndPlate=pStartPlate;
				}
				//��������ְ�����췽��
				datum_line_vec=pStartPlate->ucs.axis_z^len_vec;
				normalize(datum_line_vec);
				axis_x=pEndPlate->ucs.axis_z^len_vec;
				normalize(axis_x);
				//��������λ�þ����Ӹְ�����췽��
				for(pVertex=pEndPlate->vertex_list.GetFirst();pVertex;pVertex=pEndPlate->vertex_list.GetNext())
				{
					vertice=pVertex->vertex;
					coord_trans(vertice,pEndPlate->ucs,TRUE);
					verify_vec+=vertice;
				}
				//���ݰ�����ĵ���������췽��
				verify_vec/=pEndPlate->vertex_list.GetNodeNum();
				project_point(verify_vec,pEndPlate->ucs.origin,pEndPlate->ucs.axis_z);
				verify_vec-=pEndPlate->ucs.origin;
				normalize(verify_vec);
				//�����췽��
				if(axis_x*verify_vec<0)
					axis_x*=-1.0;
				//�������ְ�Ľ���
				f3dPoint inter_pos,line_vec,line_pick;
				Int3d2f(&line_pick,&line_vec,pStartPlate->ucs.origin,pStartPlate->ucs.axis_z,
					pEndPlate->ucs.origin,pEndPlate->ucs.axis_z);
				f3dLine line1(pStartPlate->ucs.origin+datum_line_vec*10000,pStartPlate->ucs.origin-datum_line_vec*10000);
				f3dLine line2(line_pick,line_pick+line_vec*10000);
				int nRet=Int3dll(line1,line2,inter_pos);
				if(nRet>0||nRet==-2)
				{	//��ע���ְ�֮��ļн�
					AppendAngularDim(inter_pos,axis_x,datum_line_vec,pAngular->m_fDimR);
					AppendAngularDim(inter_pos,axis_x,-datum_line_vec,pAngular->m_fDimR*0.8);
				}
			}
			else 
			{	//��ע��������֮��ļн�
				//.......
			}
		}
	}
	else if(m_pRootPart->GetClassTypeId()==CLS_SPHERE)
	{
		f3dPoint start_vec,end_vec,inter_pos;
		double dist=0;
		if(m_pSectionView->m_iViewType==VIEW_MAIN || m_pSectionView->m_iViewType==VIEW_FRONT)
		{	//��ע�ֹ�֮��ĽǶ�
			CSectionView::PART_ITEM* pItem=NULL;
			CLDSSphere* pSphere=(CLDSSphere*)m_pRootPart;
			ARRAY_LIST<DIMANGLE_INFO> dimAngleInfoList;
			//1�����ݽǶȶԸֹܽ�������
			for(pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
			{
				if(pItem->cFlag=='S')	//���ƹ���˼�
					continue;
				m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
				if(TagPart(pItem)==NULL)
					continue;
				if(TagPart(pItem)->GetClassTypeId()!=CLS_LINETUBE)
					continue;
				DIMANGLE_INFO dim_angle_info;
				CLDSLineTube* pLineTube=(CLDSLineTube*)TagPart(pItem);
				start_vec=-cs.axis_y;
				f3dPoint startPt,endPt;
				if(pLineTube->pStart->handle==pSphere->hDatumNode)
				{
					startPt=pLineTube->Start();
					endPt=pLineTube->End();
				}
				else
				{
					startPt=pLineTube->End();
					endPt=pLineTube->Start();
				}
				end_vec=endPt-startPt;
				normalize(end_vec);
				dim_angle_info.vec=end_vec;
				dim_angle_info.inters=endPt;
				dim_angle_info.angle=CalRotateAngle(cs.axis_z,start_vec,end_vec);
				dim_angle_info.fLen=pLineTube->GetLength();
				dim_angle_info.R=pLineTube->GetDiameter()*0.5;
				dimAngleInfoList.append(dim_angle_info);
			}
			CQuickSort<DIMANGLE_INFO>::QuickSort(dimAngleInfoList.m_pData,dimAngleInfoList.GetSize(),compare_func);
			//2�����иֹܼ�ĽǶȱ�ע
			DIMANGLE_INFO* pPreDimInfo=dimAngleInfoList.GetFirst();
			for(DIMANGLE_INFO* pCurDimInfo=dimAngleInfoList.GetNext();pCurDimInfo;pCurDimInfo=dimAngleInfoList.GetNext())
			{	
				f3dPoint offset_vec;
				f3dLine line1,line2;
				start_vec=pPreDimInfo->vec;
				end_vec=pCurDimInfo->vec;
				inter_pos=cs.origin;
				AppendAngularDim(inter_pos,start_vec,end_vec);
				pPreDimInfo=pCurDimInfo;
			}
		}
		else if(m_pSectionView->m_iViewType==VIEW_FL)
		{	//��ע�����߽Ƕ�
			CLDSPart *pMainPart=m_pSectionView->GetMainPartHandle(m_pTower);
			if(pMainPart==NULL)
				return;
			CLDSParamPlate* pFl=(CLDSParamPlate*)pMainPart;
			double R,D;
			pFl->GetDesignItemValue('R',&R);	//��ȡ�ھ�
			pFl->GetDesignItemValue('W',&D);	//��ȡ�⾶
			CLDSPart *pFatherPart=m_pTower->FromPartHandle(pMainPart->m_hPartWeldParent);
			if(pFatherPart==NULL)
				return;
			CXhChar50 sValue,sDimText;
			CLDSLineTube* pLineTube=(CLDSLineTube*)pFatherPart;
			start_vec=cs.axis_x;
			pLineTube->BuildUCS();	//CLDSLinePart�������������ϵucs������ʱ����ʹ��ǰӦ�Ƚ��г�ʼ�����㣬������������� wjh-2014.3.28
			end_vec=pLineTube->ucs.axis_x;
#ifdef AFX_TARG_ENU_ENGLISH
			sValue.Copy("starting edge");
#else
			sValue.Copy("��ʼ��");
#endif
			if(pLineTube->m_bHasWeldRoad)
			{
				double angle=pLineTube->CalWeldRoadAngle();
				RotateVectorAroundVector(end_vec,angle,pLineTube->ucs.axis_z);
#ifdef AFX_TARG_ENU_ENGLISH
				sValue.Copy("weld road line");
#else
				sValue.Copy("������");
#endif
			}
			inter_pos=cs.origin;
			double fDimR=0;
			double half_angle=ACOS(start_vec*end_vec)*0.5;
			if(half_angle<1.57)
			{
				fDimR = TransToModelLength(sys.dim_map.fDimAngleTextSize*0.7*4)/tan(half_angle);
				if(	fDimR+TransToModelLength(sys.dim_map.fDimAngleTextSize+1)>R && //���ֱ�ע���������ھ�
					fDimR+TransToModelLength(2.0)<0.5*D)	//���ֱ�עδ���������⾶
					fDimR=0.5*D+TransToModelLength(2.0);	//�Ƕ��߱굽������Բ���
			}
			AppendAngularDim(inter_pos,start_vec,end_vec,fDimR);
			//������
			f3dLine dim_line;
			//TODO: Ӧ�Է���Բ��Ϊ��׼���ƣ������Ǵ�cs.origin�棨Ӧ���ǵ����߲�һ�£�cs.origin�������ԣ���wjh-2014.3.26
			dim_line.startPt=cs.origin;
			dim_line.endPt=cs.origin+start_vec*(D*0.5+50);
			coord_trans(dim_line.startPt,cs,FALSE);
			coord_trans(dim_line.endPt,cs,FALSE);
			dim_line.pen.style=PS_DASHDOTDOT;
			dim_line.ID=0;
			edgeList.append(LineToArc(dim_line));
			//������
			f3dPoint vec,offset_vec;
			dim_line.startPt=cs.origin;
			//TODO: end_vec��ȫ������ϵ�µ�ʸ��������ֱ���ڴ�Ӧ�á�wjh-2014.3.26
			dim_line.endPt=cs.origin+end_vec*(D*0.5+50);
			coord_trans(dim_line.startPt,cs,FALSE);
			coord_trans(dim_line.endPt,cs,FALSE);
			dim_line.pen.style=PS_DASHDOTDOT;
			dim_line.ID=0;
			edgeList.append(LineToArc(dim_line));
			//���ֱ�ע
			offset_vec=end_vec;
			vector_trans(offset_vec,cs,FALSE);
			CTextOnlyDrawing textDraing;
			textDraing.dimPos=dim_line.endPt-offset_vec*sys.dim_map.fDimTextSize*3;
			textDraing.dimText=sValue;
			textDraing.angle=CalRotateAngle(f3dPoint(0,0,1),f3dPoint(1,0,0),offset_vec);
			textDraing.iOrder=0;
			textDraing.bNeedConvertDistToText=false;
			textDraing.bInXWing=false;
			pureTextDimList.append(textDraing);
		}
	}
}
//</DEVELOP_PROCESS_MARK>
CSizeTextDim* CSectionViewDrawing::NewSizeDimByLocatePoint(CSizeTextDim *pPrevSizeDim,int iOrderType,
		CPartGroupLocatePoint *pPrevLocatePoint,CPartGroupLocatePoint *pLocatePoint)
{
	if(pPrevLocatePoint==NULL)
		return NULL;
	if(pPrevLocatePoint->m_bDimSpace!=pLocatePoint->m_bDimSpace)
		return NULL;
	f3dPoint startPt=pPrevLocatePoint->m_xLocalCoordPos;
	f3dPoint endPt=pLocatePoint->m_xLocalCoordPos;
	double dimDist=0;
	if(pPrevLocatePoint->m_bDimSpace==TRUE && pLocatePoint->m_bDimSpace==TRUE)	//���Գߴ�
		dimDist=DISTANCE(startPt,endPt);		
	else		//��Գߴ�
		dimDist=pLocatePoint->m_fLenPosToDatumPart-pPrevLocatePoint->m_fLenPosToDatumPart;
	//��Գߴ��עλ��
	BOOL bNeedMerge=(abs(iOrderType)!=2);	//�Ŷ��ߴ��ע������Ҫ�ϲ� wht 18-09-22
	if(bNeedMerge&&pPrevSizeDim&&pPrevLocatePoint&&fabs(dimDist-pPrevSizeDim->dist)<0.5)//&&pPrevLocatePoint->m_hPart==pLocatePoint->m_hPart
	{	//�ϲ������ص��ߴ�
		pPrevSizeDim->nHits++;
		//����ǰһ����ע���ն�λ���Լ���עλ��
		pPrevSizeDim->dimEndPos=pLocatePoint->m_xLocalCoordPos;
		pPrevSizeDim->dimPos.x=(pPrevSizeDim->dimStartPos.x+pPrevSizeDim->dimEndPos.x)/2.0;
	}
	else
	{	//����±�ע
		//�����ʼ��λ�����ն�λ���غϵĳߴ��ע wht 10-04-19
		if(pPrevLocatePoint->m_xLocalCoordPos==pLocatePoint->m_xLocalCoordPos)
			return pPrevSizeDim;	//����ǰһ����ע
		pPrevSizeDim=spaceDimList.append();
		pPrevSizeDim->dimStyle.style=DimStyleTable::dimStyle;
		pPrevSizeDim->dimStyle.textHeight = sys.dim_map.fDimTextSize;
		pPrevSizeDim->dimStartPos=startPt;
		pPrevSizeDim->dimEndPos=endPt;
		if(pPrevLocatePoint->m_bDimSpace==FALSE)
		{
			pPrevSizeDim->dimPos.x=(startPt.x+endPt.x)*0.5;
			if(iOrderType>0)
				pPrevSizeDim->dimPos.y=max(startPt.y,endPt.y);
			else
				pPrevSizeDim->dimPos.y=min(startPt.y,endPt.y);
			pPrevSizeDim->angle=0;
		}
		else
		{
			pPrevSizeDim->dimPos=(startPt+endPt)*0.5;
			pPrevSizeDim->angle=atan((endPt.y-startPt.y)/(endPt.x-startPt.x));
		}
		pPrevSizeDim->dist=dimDist;
		pPrevSizeDim->iOrder=iOrderType;
		pPrevSizeDim->nHits=1;
	}
	return pPrevSizeDim;
}
void CSectionViewDrawing::StatSizeDimsByLocatePoint(CPartGroupLocatePoint locatePointArr[],int num,int iOrderType)
{
	CPartGroupLocatePoint *pPrevLocatePoint=NULL;
	CHashTable<long>finishLocalDimTbl;
	finishLocalDimTbl.CreateHashTable(10);
	long m_hCurDimPart=0;
	CSizeTextDim *pPrevSpaceDim=NULL;	//���ߴ��ע
	for(int i=0;i<num;i++)
	{
		if(abs(iOrderType)==2)	//�ֲ���϶��ע
		{
			if(locatePointArr[i].m_hPart<=0||
				(locatePointArr[i].m_iOrderType!=0&&locatePointArr[i].m_iOrderType!=iOrderType))
				continue;	//�Ǿֲ���϶��ر궨��
			long v=0;
			if(finishLocalDimTbl.GetValueAt(locatePointArr[i].m_hPart,v))
				continue;	//�Ѵ������λ��
			m_hCurDimPart=locatePointArr[i].m_hPart;
			pPrevLocatePoint=&locatePointArr[i];
			pPrevSpaceDim=NULL;
			CXhPtrSet<CSizeTextDim> curPartDimSet;
			double maxDimPosY=-10000,minDimPosY=10000;
			for(int j=i+1;j<num;j++)
			{
				if(locatePointArr[j].m_hPart!=m_hCurDimPart||
					(pPrevLocatePoint->m_iOrderType!=iOrderType&&locatePointArr[j].m_iOrderType!=iOrderType)||
					(locatePointArr[j].m_iOrderType!=0&&locatePointArr[j].m_iOrderType!=iOrderType))
					continue;	//���ǵ�ǰҪ��ע�ļ�϶�궨��
				pPrevSpaceDim=NewSizeDimByLocatePoint(pPrevSpaceDim,1,pPrevLocatePoint,&locatePointArr[j]);
				if(pPrevSpaceDim&&iOrderType<0)
				{
					pPrevSpaceDim->iOrder*=-1;
					if(pPrevSpaceDim->nHits==1)	//ֻ�ڵ�һ����Ӽ���עʱ������עλ�ã������ε������³ߴ����޷��������� wht 18-09-18
					{
						pPrevSpaceDim->dimPos.y-=2;
						curPartDimSet.append(pPrevSpaceDim);
						if(pPrevSpaceDim->dimPos.y<minDimPosY)
							minDimPosY=pPrevSpaceDim->dimPos.y;
					}
				}
				else if(pPrevSpaceDim&&pPrevSpaceDim->nHits==1)
				{
					pPrevSpaceDim->dimPos.y+=2;
					curPartDimSet.append(pPrevSpaceDim);
					if(pPrevSpaceDim->dimPos.y>maxDimPosY)
						maxDimPosY=pPrevSpaceDim->dimPos.y;
				}
				pPrevLocatePoint=&locatePointArr[j];
			}
			finishLocalDimTbl.SetValueAt(m_hCurDimPart,1);
			//ʮ�ֲ��ֻ��һ����˨ʱ���ֲ��ߴ��ע�޷����루����ʮ�ֲ��ֲ��ߴ��ע�� wht 18-09-19
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(m_hCurDimPart,CLS_PARAMPLATE);
			if(pParamPlate&&pParamPlate->IsInsertPlate())
			{
				for(CSizeTextDim *pSize=curPartDimSet.GetFirst();pSize;pSize=curPartDimSet.GetNext())
				{
					if(iOrderType>0)
						pSize->dimPos.y=maxDimPosY;
					else
						pSize->dimPos.y=minDimPosY;
				}
			}
		}
		else
		{
			if(pPrevLocatePoint==NULL)
			{
				if(locatePointArr[i].m_iOrderType==iOrderType||locatePointArr[i].m_iOrderType==0)
					pPrevLocatePoint=&locatePointArr[i];
				continue;
			}
			else if(pPrevLocatePoint->m_iOrderType!=iOrderType&&locatePointArr[i].m_iOrderType!=iOrderType)
				continue;
			if(iOrderType==0)
			{	//���浽����֮��ߴ��ע
				if(pPrevLocatePoint->m_iOrderType!=iOrderType||locatePointArr[i].m_iOrderType!=iOrderType)
				{
					if(locatePointArr[i].m_iOrderType==iOrderType)
						pPrevLocatePoint=&locatePointArr[i];
					continue;
				}
				pPrevSpaceDim=NewSizeDimByLocatePoint(pPrevSpaceDim,4,pPrevLocatePoint,&locatePointArr[i]);
			}
			else if(abs(iOrderType)==1)	//������λ��֮����ߴ��ע
			{	//ֻ�Զ��涨λ���ͬ�๹����λ����Ч
				if(locatePointArr[i].m_iOrderType!=iOrderType&&locatePointArr[i].m_iOrderType!=0)
					continue;
				pPrevSpaceDim=NewSizeDimByLocatePoint(pPrevSpaceDim,3,pPrevLocatePoint,&locatePointArr[i]);
			}
			else if(abs(iOrderType)==3)	//�Ŷ�����ע
			{	//ֻ�Զ��涨λ��ͽŶ���λ����Ч
				if(locatePointArr[i].m_iOrderType!=iOrderType&&locatePointArr[i].m_iOrderType!=0)
					continue;
				pPrevSpaceDim=NewSizeDimByLocatePoint(pPrevSpaceDim,2,pPrevLocatePoint,&locatePointArr[i]);
			}
			if(pPrevSpaceDim!=NULL&&iOrderType<0)
			{
				pPrevSpaceDim->iOrder=abs(pPrevSpaceDim->iOrder)*-1;
				pPrevSpaceDim->dimPos.y-=2;
			}
			else if(pPrevSpaceDim)
				pPrevSpaceDim->dimPos.y+=1;
			pPrevLocatePoint=&locatePointArr[i];
		}
	}
}
//��ע��Գߴ��Լ����Գߴ�
void CSectionViewDrawing::DimLocatePoint(CLDSLinePart *pDatumPart)
{
	CTextOnlyDrawing locatepos_dim;	//��������λ�ı���ע
	locatepos_dim.angle=Pi*0.5;		//��ת��
	locatepos_dim.textHeight=sys.dim_map.fDimTextSize;
	CPartGroupLocatePoint *pPrevLocatePoint=NULL, *pLocatePoint=NULL,*pFirstLocatePoint=NULL;
	f3dPoint datumOrg,datumVec=pDatumPart->End()-pDatumPart->Start();
	normalize(datumVec);
	if(datumVec*cs.axis_x>0)
		datumOrg=pDatumPart->Start()-datumVec*pDatumPart->startOdd();
	else
	{
		datumOrg=pDatumPart->End()+datumVec*pDatumPart->endOdd();
		datumVec*=-1;
	}
	//��λ������
	int i=0,j=0;
	ATOM_LIST<LOCATEPOINT_PTR> upLocatePoints, downLocatePoints, temp_locate_point;
	CPartGroupLocatePoint *locate_point_arr = new CPartGroupLocatePoint[m_pSectionView->LocatePoints.GetNodeNum()];
	for(pLocatePoint=m_pSectionView->LocatePoints.GetFirst(),i=0;
		pLocatePoint;pLocatePoint=m_pSectionView->LocatePoints.GetNext(),i++)
		locate_point_arr[i]=*pLocatePoint;
	CQuickSort<CPartGroupLocatePoint>::QuickSort(
		locate_point_arr,m_pSectionView->LocatePoints.GetNodeNum(),compareLocatePoint);
	//Ԥ��ͳ���²�궨�����,����²�궨�����С�ڵ���0,���������²��ע���Գߴ� wht 10-08-08
	int nDownPointNum=0;
	for(i=0;i<m_pSectionView->LocatePoints.GetNodeNum();i++)
	{	//ͳ���²�궨����� �������ֲ��ߴ�궨��
		if(locate_point_arr[i].m_iOrderType<0&&abs(locate_point_arr[i].m_iOrderType)!=2)	
			nDownPointNum++;
	}
	for(i=0;i<m_pSectionView->LocatePoints.GetNodeNum();i++)
	{
		locate_point_arr[i].m_xLocalCoordPos=datumOrg+
			locate_point_arr[i].m_fLenPosToDatumPart*datumVec+locate_point_arr[i].m_fVertPosToDatumPart*cs.axis_y;
		coord_trans(locate_point_arr[i].m_xLocalCoordPos,cs,FALSE);
		if(abs(locate_point_arr[i].m_iOrderType)<=1)
		{	//��Ӿ�������λ�ı���ע
			locatepos_dim.dist=locate_point_arr[i].m_fLenPosToDatumPart+m_pSectionView->m_fOrgLenOffset;
			locatepos_dim.dimPos=locate_point_arr[i].m_xLocalCoordPos;
			if(locate_point_arr[i].m_iOrderType>=0)
			{	//����ϲ���Գߴ��ע
				locatepos_dim.iOrder=1;	//װ�䶨λ�ϲ���Գߴ�
				pureTextDimList.append(locatepos_dim);
			}
			if(locate_point_arr[i].m_iOrderType<=0&&nDownPointNum>0)
			{	//����²���Գߴ��ע
				locatepos_dim.iOrder=-1;	//װ�䶨λ�²���Գߴ�-1;
				pureTextDimList.append(locatepos_dim);
			}
		}
	}

	//------------V-�����Գߴ��ע-V-----------------
	StatSizeDimsByLocatePoint(locate_point_arr,m_pSectionView->LocatePoints.GetNodeNum(),0);	//���浽����ߴ�
	StatSizeDimsByLocatePoint(locate_point_arr,m_pSectionView->LocatePoints.GetNodeNum(),1);	//������λ���ߴ�
	StatSizeDimsByLocatePoint(locate_point_arr,m_pSectionView->LocatePoints.GetNodeNum(),-1);	//������λ���ߴ�
	StatSizeDimsByLocatePoint(locate_point_arr,m_pSectionView->LocatePoints.GetNodeNum(),2);	//�ֲ���϶�ߴ�
	StatSizeDimsByLocatePoint(locate_point_arr,m_pSectionView->LocatePoints.GetNodeNum(),-2);	//�ֲ���϶�ߴ�
	StatSizeDimsByLocatePoint(locate_point_arr,m_pSectionView->LocatePoints.GetNodeNum(),-3);	//�Ŷ��ߴ�
	delete []locate_point_arr;
}

//���㷨���뾶
static double GetFlR(CLDSParamPlate *pPlate)
{
	if(pPlate==NULL||!pPlate->IsFL())
		return 0;
	double R=0;
	if(pPlate->m_iParamType==TYPE_FL)
	{
		pPlate->GetDesignItemValue('W',&R);
		R*=0.5;
	}
	else if(pPlate->m_bStdPart)
	{
		CLDSStdPartModel stdPartModel;
		if(pPlate->GetStdPartModel(&stdPartModel))
		{
			if(pPlate->m_iParamType==TYPE_FLD||pPlate->m_iParamType==TYPE_FLP)
				R=stdPartModel.param.fl_param.D*0.5;		//��/ƽ������
			else if(pPlate->m_iParamType==TYPE_FLG||pPlate->m_iParamType==TYPE_FLR)
				R=stdPartModel.param.fl_param2.fOuterD*0.5;	//��/���Է���
		}
	}
	return R;
}
//<DEVELOP_PROCESS_MARK nameId="CSectionViewDrawing::DimSectionViewSymbol">
//��ע����ͼ��
void CSectionViewDrawing::DimSectionViewSymbol()
{
	if(m_pTower==NULL||m_pPartGroup==NULL||m_pSectionView==NULL)
		return;
	if(!(m_pSectionView->m_iViewType==VIEW_MAIN || 
		(m_pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_SPHERE && m_pSectionView->m_iViewType==VIEW_FRONT)))
		return;
	//������ͼ�б�ע����ͼ��
	int i=0;
	const int SYMBOL_OFFSET_DIST=10+(int)sys.stru_map.fPolyLineWidth;	//��ע���ر�ע�������ƫ�ƾ���
	CSectionView *pView=NULL;
	BOOL bPush=m_pPartGroup->SonViews.push_stack();
	for(pView=m_pPartGroup->SonViews.GetFirst();pView;pView=m_pPartGroup->SonViews.GetNext())
	{	
		if(pView->m_iViewType==VIEW_MAIN||pView->m_iViewType==VIEW_FRONT||pView->m_iViewType==VIEW_USERDEF)
			continue;
		//��ڵ��麸��������ͼ��ǰ��ͼ�������ͼ�����������Ӧ������ͼ
		if(m_pRootPart->GetClassTypeId()==CLS_SPHERE && 
			strstr(pView->Symbol,m_pSectionView->Symbol)==NULL)	
			continue;
		//���Ҷ���ͼ������
		CLDSPart *pMainPart=pView->GetMainPartHandle(m_pTower);
		if(pMainPart)
		{
			CLDSPart *pFatherPart=m_pTower->FromPartHandle(pMainPart->m_hPartWeldParent);
			if(pFatherPart==NULL)
				continue;
			f3dPoint offset_vec,dim_vec,dim_pos;
			dim_vec=-pView->ucs.axis_z;			
			vector_trans(dim_vec,cs,FALSE);	//ת������ǰ��ͼ����ϵ��
			dim_vec.z=0;
			offset_vec=dim_vec;
			RotateVectorAroundVector(offset_vec,0.5*Pi,f3dPoint(0,0,1));
			double offset_dist=0.6*pMainPart->GetWidth(); //ƫ�ƾ����ʼֵΪ0.6*���������
			dim_pos=pMainPart->ucs.origin+pView->ucs.axis_z*SYMBOL_OFFSET_DIST;
			if(pFatherPart->IsLinePart())
			{	
				offset_dist=0.5*pFatherPart->GetWidth();
				if(pMainPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate *pMainParamPlate=(CLDSParamPlate*)pMainPart;
					if(pMainParamPlate->IsInsertPlate())
					{
						double L=0;
						pMainParamPlate->GetDesignItemValue('L',&L);
						if(pView->ucs.axis_z*pMainParamPlate->ucs.axis_x>0)
							dim_pos+=pView->ucs.axis_z*L;
						else
							dim_pos-=pView->ucs.axis_z*L;
					}
					offset_dist=max(offset_dist,GetFlR((CLDSParamPlate*)pMainPart));
					
				}
				//���������Ӹ�����Ϊ�˼����Ҹ˼����������ͼZ�᷽����
				CLDSLinePart *pLinePart=(CLDSLinePart*)pFatherPart;
				f3dPoint len_vec=pLinePart->End()-pLinePart->Start();
				normalize(len_vec);
				vector_trans(len_vec,cs,FALSE);
				if(fabs(len_vec*dim_vec)>EPS_COS2)
				{
					f3dPoint perp_pos=dim_pos;
					SnapPerp(&perp_pos,pLinePart->Start(),pLinePart->End(),dim_pos);
					dim_pos=perp_pos;
				}
			}
			coord_trans(dim_pos,cs,FALSE);
			dim_pos.z=0;
			double offset_angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
			//���ֱ�ע����Ӧͷ���ϣ���ֱ���ֱ�עӦͷ����wjh-2014.3.26
			offset_angle=ReviseTextAngleToHeadUpward(offset_angle);
			for (i=0;i<2;i++)
			{
				CTextOnlyDrawing *pSectionViewDim=pureTextDimList.append();
				pSectionViewDim->iOrder=3;	///����ͼ��
				pSectionViewDim->bNeedConvertDistToText=false;
				pSectionViewDim->dimText.Copy(pView->Symbol); 
				pSectionViewDim->angle=offset_angle;
				pSectionViewDim->textHeight=1.5*sys.dim_map.fDimTextSize;	//�ݶ�����ͼ����Ϊ1.5����ͨ��ע�ָ�
				if(i==0)
				{
					pSectionViewDim->bInXWing=FALSE; //�ϲ�
					pSectionViewDim->dimPos=dim_pos+offset_vec*offset_dist*1.3;
				}
				else
				{
					pSectionViewDim->bInXWing=TRUE; //�²�
					pSectionViewDim->dimPos=dim_pos-offset_vec*offset_dist*1.3;
				}
			}
		}
	}
	if(bPush)
		m_pPartGroup->SonViews.pop_stack();
}
bool CSectionViewDrawing::DimSectionViewSymbol2()
{
	CSectionView* pView=NULL;
	bool bFinished=false;
	BOOL bPush=m_pPartGroup->SonViews.push_stack();
	for(pView=m_pPartGroup->SonViews.GetFirst();pView;pView=m_pPartGroup->SonViews.GetNext())
	{
		if(!pView->IsSlaveView())
			continue;
		CSlaveSectionView* pSlaveView=(CSlaveSectionView*)pView;
		if(pSlaveView->m_idHostView!=m_pSectionView->Id)
			continue;
		CSectionView* pMainSection=m_pPartGroup->SonViews.GetValue(pSlaveView->m_idHostView);
		UCS_STRU host_cs=pMainSection->UpdateUCS(m_pTower,m_pPartGroup);
		f3dPoint dim_vec,offset_vec,dim_pos;
		dim_vec=pSlaveView->direction;
		dim_pos=pSlaveView->location;
		coord_trans(dim_pos,host_cs,TRUE);
		vector_trans(dim_vec,host_cs,TRUE);
		coord_trans(dim_pos,cs,FALSE);
		vector_trans(dim_vec,cs,FALSE);
		dim_pos+=dim_vec*pSlaveView->direct_offset;
		offset_vec=dim_vec;
		RotateVectorAroundVector(offset_vec,0.5*Pi,f3dPoint(0,0,1));
		double offset_angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
		for (int i=0;i<2;i++)
		{
			CTextOnlyDrawing *pSectionViewDim=pureTextDimList.append();
			pSectionViewDim->iOrder=3;	///����ͼ��
			pSectionViewDim->bNeedConvertDistToText=false;
			pSectionViewDim->dimText.Copy(pView->Symbol); 
			pSectionViewDim->angle=offset_angle;
			pSectionViewDim->textHeight=1.5*sys.dim_map.fDimTextSize;	//�ݶ�����ͼ����Ϊ1.5����ͨ��ע�ָ�
			if(i==0)
			{
				pSectionViewDim->bInXWing=FALSE; //�ϲ�
				pSectionViewDim->dimPos=dim_pos+offset_vec*pSlaveView->symbol_dist;
			}
			else
			{
				pSectionViewDim->bInXWing=TRUE; //�²�
				pSectionViewDim->dimPos=dim_pos-(offset_vec*pSlaveView->symbol_dist);
			}
		}
		bFinished=true;
	}
	if(bPush)
		m_pPartGroup->SonViews.pop_stack();
	return bFinished;
}
//</DEVELOP_PROCESS_MARK>

void CSectionViewDrawing::CreatePartSketch(CSectionView::PART_ITEM* pPartItem,BOOL bCoplanar/*=TRUE*/)
{
	CLDSPart *pPart=TagPart(pPartItem);;
	//1����ȡ��ͼ����ϵ
	CDrawCoordTrans dcs;
	if(pPart==NULL||!GetDCS(pPart->handle,dcs))
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("fail to get the convert coordinate system of part drawing!");
#else
		logerr.Log("��ȡ������ͼת������ϵʧ�ܣ�");
#endif
	BYTE cExecuteDrawFlag;
	CStruDrawingCommand* pStruCmd=NULL;
	int h0v1e2=DRAWFLAG(pPartItem->cFlag).GetPartDrawType(pPart,dcs.PrjNorm(),&cExecuteDrawFlag);
	if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		CreateLineAngleDrawing((CLDSLineAngle*)pPart,dcs);
	else if(pPart->GetClassTypeId()==CLS_LINETUBE)
	{
		if(m_pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_SPHERE &&
			(m_pSectionView->m_iViewType==VIEW_MAIN || m_pSectionView->m_iViewType==VIEW_FRONT))
			CreateCutTubeDrawing((CLDSLineTube*)pPart,bCoplanar);
		else if(m_pSectionView->m_iViewType==VIEW_JOINSECTION&&h0v1e2==0)
			CreateCutTubeDrawing((CLDSLineTube*)pPart,TRUE);
		else
			CreateLineTubeDrawing((CLDSLineTube*)pPart,dcs,FALSE);
	}
	else if(pPart->IsPlate())
	{
		h0v1e2=-1;
		if(m_pRootPart&&m_pRootPart->IsTube()&&fabs(dcs.PrjNorm()*pPart->ucs.axis_z)>EPS_COS2)
		{	//չ���淨���뵱ǰ��������һ��ʱ��ָ��������ƣ�������ڽǶȹ�С�ְ����������жϴ���(��:��90�ȵ����ְ�չ������) wht 18-03-14
			if( pPartItem->ciSpreadDirection==CSectionView::PART_ITEM::SPREAD_TO_UNDER||
				pPartItem->ciSpreadDirection==CSectionView::PART_ITEM::SPREAD_TO_UPPER)
				h0v1e2=0;
			else if( pPartItem->ciSpreadDirection==CSectionView::PART_ITEM::SPREAD_TO_TOP||
					 pPartItem->ciSpreadDirection==CSectionView::PART_ITEM::SPREAD_TO_BACK)
				h0v1e2=1;
		}
		if(pPartItem->cFlag=='W')
			return;	//�ֹ����ŵװ��߰岻�ڴ˴����ƣ����������߰�ʾ��ͼ wht 18-08-24
		if(pPart->GetClassTypeId()==CLS_PLATE)
			CreatePlateDrawing((CLDSPlate*)pPart,dcs,bCoplanar,FALSE,0,h0v1e2);
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			CreateParamPlateDrawing((CLDSParamPlate*)pPart,dcs,bCoplanar,FALSE,h0v1e2);
	}
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
		CreateLineSlotDrawing((CLDSLineSlot*)pPart,dcs);
	else if(pPart->GetClassTypeId()==CLS_SPHERE)
		CreateSphereDrawing((CLDSSphere*)pPart);
}

void CSectionViewDrawing::ResetPosition()
{
	f2dRect draw_rect=GetDrawRegion();
	f3dPoint base_pnt(-draw_rect.topLeft.x,-draw_rect.topLeft.y);
	//�����麸ͼ����������
	for(IDbEntity* pEnt=EnumFirstDbEntity();pEnt;pEnt=EnumNextDbEntity())
		pEnt->Offset(base_pnt);
	for(f3dArcLine *pLine=GetEdgeLineList()->GetFirst();pLine;pLine=GetEdgeLineList()->GetNext())
		pLine->Offset(base_pnt);
	for(ACAD_HATCH *pHatch=hatchList.GetFirst();pHatch;pHatch=hatchList.GetNext())
	{
		for(f3dPoint *pPt=pHatch->vertexList.GetFirst();pPt;pPt=pHatch->vertexList.GetNext())
			*pPt+=base_pnt;
	}
	f3dCircle *pCir=NULL;
	for(pCir=GetLsCirList()->GetFirst();pCir;pCir=GetLsCirList()->GetNext())
		pCir->centre+=base_pnt;
	for(CSizeTextDim *pSpaceDim=GetSpaceDimList()->GetFirst();pSpaceDim;pSpaceDim=GetSpaceDimList()->GetNext())
	{
		if(pSpaceDim->dist==0)
			continue;
		pSpaceDim->dimPos+=base_pnt;
		pSpaceDim->dimStartPos+=base_pnt;
		pSpaceDim->dimEndPos+=base_pnt;
	}
	for(CTextOnlyDrawing *pLocatePointDim=GetPureTextDimList()->GetFirst();pLocatePointDim;pLocatePointDim=GetPureTextDimList()->GetNext())
		pLocatePointDim->dimPos+=base_pnt;
	//��ע�Ƕ���
	for(CSizeAngleDim *pAngularDim=GetAngularDimList()->GetFirst();pAngularDim;pAngularDim=GetAngularDimList()->GetNext())
	{
		pAngularDim->vertex+=base_pnt;
		pAngularDim->dimStartPos+=base_pnt;
		pAngularDim->dimEndPos+=base_pnt;
		pAngularDim->dimPos+=base_pnt;
	}
	//��չ���ݵ�
	for(CTextFrame *pDataPoint=GetDataPointDimList()->GetFirst();pDataPoint;pDataPoint=GetDataPointDimList()->GetNext())
	{
		pDataPoint->dataPos+=base_pnt;
		pDataPoint->dimPos+=base_pnt;
		pDataPoint->center+=base_pnt;
	}
}

//from DrawUnit.cpp
BOOL IsOrthoRectInters(f3dPoint topLeft1,double width1,double height1,f3dPoint topLeft2,double width2,double height2);
BOOL IsPtInRect(f3dPoint rgnVertArr[4],const f3dPoint &pick)
{
	for(int i=0;i<4;i++)
	{
		f3dPoint v1=rgnVertArr[(i+1)%4]-rgnVertArr[i];
		f3dPoint v2=pick-rgnVertArr[i];
		if(v1.x*v2.y-v1.y*v2.x<=0) 
			return FALSE;	//����ĳ���ߵ��Ҳ��߿���,�ʲ��Ǿ��ο���ڵ�
	}
	return TRUE;	//�������бߵ������Ǿ��ο���ڵ�
}
BOOL IsRectInters(f3dPoint rgnVertArr1[4],f3dPoint rgnVertArr2[2])
{
	f3dPoint v,v1,v2;
	for(int i=0;i<4;i++)
	{
		bool ddRight1[4],ddRight2[4];
		v1=rgnVertArr1[(i+1)%4]-rgnVertArr1[i];
		v2=rgnVertArr2[(i+1)%4]-rgnVertArr2[i];
		for(int j=0;j<4;j++)
		{
			v=rgnVertArr2[j]-rgnVertArr1[i];
			if(v1.x*v.y-v1.y*v.x<=0) 
				ddRight1[j]=true;	//����ĳ���ߵ��Ҳ�
			else
				ddRight1[j]=false;	//����ĳ���ߵ����
			v=rgnVertArr1[j]-rgnVertArr2[i];
			if(v2.x*v.y-v2.y*v.x<=0) 
				ddRight2[j]=true;	//����ĳ���ߵ��Ҳ�
			else
				ddRight2[j]=false;	//����ĳ���ߵ����
		}
		if(!ddRight1[0]||!ddRight1[1]||!ddRight1[2]||!ddRight1[3])
			continue;
		else
			return false;	//����2ȫ�����ھ���1��i���ߵĵ��Ҳ�
		if(!ddRight2[0]||!ddRight2[1]||!ddRight2[2]||!ddRight2[3])
			continue;
		else
			return false;	//����1ȫ�����ھ���2��i���ߵĵ��Ҳ�
	}
	return true;	//�������ص���
}
bool IsInLineLeft(f2dPoint pick,f2dPoint lineStart,f2dPoint lineEnd)
{
	f2dPoint v1(lineEnd.x-lineStart.x,lineEnd.y-lineStart.y);
	f2dPoint v2(pick.x-lineStart.x,pick.y-lineStart.y);
	if(v1.x*v2.y-v1.y*v2.x>=0) 
		return true;	//����ĳ���ߵ����
	else
		return false;	//����ĳ���ߵ��Ҳ�
}
bool CSectionViewDrawing::IsHasIntersObj(f3dPoint rgnVert[4],void *pExcusiveSelfDrawObj)
{
	double zoomScaleCoef=1.0/m_fUser2ScrDrawScale;
	double fZoomFontCoef=ZoomToModelTextSize(1.0);	//��������߶ȵ���ͼԪλ�ã��轫����߶ȸ����涯���ű�����������
	SAFE_TEXTSIZE safeTextSize;
	safeTextSize.minFontSize=sys.part_group_map.fMinFontSize;
	//TODO:Ӧȷ��rgnVert����������������
	AcDbBlockTableRecord *pBlockTableRecord=new AcDbBlockTableRecord;//�������¼ָ��
	for(IDbEntity* pEntity=EnumFirstDbEntity();pEntity;pEntity=EnumNextDbEntity())
	{
		if(pEntity->GetDbEntType()==IDbEntity::DbLine)			//�߶�
		{
			IDbLine* pLine=(IDbLine*)pEntity;
			f3dPoint startPt=pLine->StartPosition();
			f3dPoint endPt=pLine->EndPosition();
			f2dLine line(f2dPoint(startPt.x,startPt.y),f2dPoint(endPt.x,endPt.y));
			bool dd[4];
			int flag=4;
			for(int i=0;i<4;i++)
				dd[i]=IsInLineLeft(rgnVert[i],line.startPt,line.endPt);
			for(int i=0;i<4;i++)
			{
				if(dd[i]!=dd[(i+1)%4])
				{
					flag=i;
					break;
				}
			}
			if(flag<4)
			{	//���ο���ֱ���н��㣬����һ�����߶��н���
				f2dPoint inters;
				for(int i=0;i<4;i++)
				{
					if(Int2dll(f2dLine(rgnVert[i],rgnVert[(i+1)%4]),line,inters.x,inters.y)>=1)
					{	//ֱ���뵱ǰ�߿��߶��н���
						return true;
					}
				}
				for(flag=0;flag<4;flag++)
				{
					if(!IsInLineLeft(line.startPt,rgnVert[flag],rgnVert[(flag+1)%4])||!IsInLineLeft(line.endPt,rgnVert[flag],rgnVert[(flag+1)%4]))
						break;	//�߶�������һ��˵㲻�ھ��ο���
				}
				if(flag==4)
				{	//ȫ���߶ζ�λ�ھ��ο���
					return true;
				}
			}
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbPolyline)			//�߶�
		{
			GEPOINT startPt,endPt;
			IDbPolyline* pPolyline=(IDbPolyline*)pEntity;
			for(int i=1;i<pPolyline->numVerts();i++)
			{
				if(i==1)
					pPolyline->GetVertexAt(0,startPt);
				else
					startPt=endPt;
				pPolyline->GetVertexAt(i,endPt);
				f2dLine line(f2dPoint(startPt.x,startPt.y),f2dPoint(endPt.x,endPt.y));
				bool dd[4];
				int flag=4;
				for(int i=0;i<4;i++)
					dd[i]=IsInLineLeft(rgnVert[i],line.startPt,line.endPt);
				for(int i=0;i<4;i++)
				{
					if(dd[i]!=dd[(i+1)%4])
					{
						flag=i;
						break;
					}
				}
				if(flag<4)
				{	//���ο���ֱ���н��㣬����һ�����߶��н���
					f2dPoint inters;
					for(int i=0;i<4;i++)
					{
						if(Int2dll(f2dLine(rgnVert[i],rgnVert[(i+1)%4]),line,inters.x,inters.y)>=1)
						{	//ֱ���뵱ǰ�߿��߶��н���
							return true;
						}
					}
					for(flag=0;flag<4;flag++)
					{
						if(!IsInLineLeft(line.startPt,rgnVert[flag],rgnVert[(flag+1)%4])||!IsInLineLeft(line.endPt,rgnVert[flag],rgnVert[(flag+1)%4]))
							break;	//�߶�������һ��˵㲻�ھ��ο���
					}
					if(flag==4)
					{	//ȫ���߶ζ�λ�ھ��ο���
						return true;
					}
				}
			}
		}
		/*else if(pEntity->GetDbEntType()==IDbEntity::DbText)			//�߶�
		{
			//TODO:����IDbText��CTextOnlyDrawing��ת�����ٽ�����ײ��� wjh-2018.3.11
			IDbText* pDbText=(IDbText*)pEntity;
			CTextOnlyDrawing text;
			text.dimPos=pDbText->GetPosition();
			text.dimText=pDbText->GetTextString();
			text.angle=pDbText->GetRotation();
			//text.alignType=pDbText->GetAlignment();
			//if(pPureTextDim->cFlag==0)
			//	text.dimPos*=zoomScaleCoef;
			text.GetDimRect(rgnTempVert,fZoomFontCoef);
			if(IsRectInters(rgnTempVert,rgnVert))
			{
				if(bPush)
					pureTextDimList.pop_stack();
				return true;
			}
		}*/
		else if(pEntity->GetDbEntType()==IDbEntity::DbArcline)		//��������
		{
			IDbArcline* pArcLine=(IDbArcline*)pEntity;
			f3dArcLine arcline;
			arcline.SetColumnNorm(pArcLine->ColumnNorm());
			arcline.SetWorkNorm(pArcLine->WorkNorm());
			arcline.SetStart(pArcLine->StartPosition());
			arcline.SetEnd(pArcLine->EndPosition());
			arcline.SetCenter(pArcLine->Center());
			for(int i=0;i<4;i++)
			{
				f2dLine line(f2dPoint(rgnVert[i].x,rgnVert[i].y),f2dPoint(rgnVert[(i+1)%4].x,rgnVert[(i+1)%4].y));
				f2dArc arc;
				double PX[2],PY[2];
				arc.centre=arcline.Center();
				arc.startPt=arcline.Start();
				arc.angle=arcline.SectorAngle();
				if(Int2dla(line,arc,PX[0],PY[0],PX[1],PY[1])>0)
					return true;	//�ҵ���Ч����
			}
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbCircle)		//Բ
		{
			IDbCircle* pDbCircle=(IDbCircle*)pEntity;
			double radius_2=pDbCircle->Radius()*pDbCircle->Radius();
			double double_radius_2=2*radius_2;
			//TODO:Ӧ������������е����н��㷨���иĽ� wjh-2016.11.16
			for(int i=0;i<4;i++)
			{
				double dx=rgnVert[i].x-pDbCircle->Center().x;
				double dy=rgnVert[i].y-pDbCircle->Center().y;
				double dist_2=dx*dx+dy*dy;
				if(dist_2>radius_2&&dist_2<double_radius_2)
					return true;	//����
			}
		}
	}
	BOOL bPush=edgeList.push_stack();
	for(f3dArcLine *pArcLine=edgeList.GetFirst();pArcLine;pArcLine=edgeList.GetNext())
	{
		if(pArcLine->feature==-1)
			continue;	//��ʾ��������ײ���,�纸����,���Ӷ�λ��ʶ�ߵ�
		if(pArcLine->SectorAngle()>0&&!pArcLine->WorkNorm().IsZero())
		{	//Բ��
			for(int i=0;i<4;i++)
			{
				f2dLine line;
				line.startPt=rgnVert[i];
				line.endPt=rgnVert[(i+1)%4];
				f2dArc arc;
				double PX[2],PY[2];
				arc.centre=pArcLine->Center();
				arc.startPt=pArcLine->Start();
				arc.angle=pArcLine->SectorAngle();
				if(Int2dla(line,arc,PX[0],PY[0],PX[1],PY[1])>0)
				{
					if(bPush)
						edgeList.pop_stack();
					return true;	//�ҵ���Ч����
				}
			}
		}
		else
		{	//ֱ��
			f2dLine line;
			line.startPt=pArcLine->Start();
			line.endPt=pArcLine->End();
			f3dPoint v1(line.endPt.x-line.startPt.x,line.endPt.y-line.startPt.y);
			bool dd[4];
			int i;
			for(i=0;i<4;i++)
				dd[i]=IsInLineLeft(rgnVert[i],line.startPt,line.endPt);
			for(i=0;i<4;i++)
			{
				if(dd[i]!=dd[(i+1)%4])
					break;
			}
			if(i<4)
			{	//���ο���ֱ���н��㣬����һ�����߶��н���
				f2dPoint inters;
				for(i=0;i<4;i++)
				{
					if(Int2dll(f2dLine(rgnVert[i],rgnVert[(i+1)%4]),line,inters.x,inters.y)>=1)
					{	//ֱ���뵱ǰ�߿��߶��н���
						if(bPush)
							edgeList.pop_stack();
						return true;
					}
				}
				for(i=0;i<4;i++)
				{
					if(!IsInLineLeft(line.startPt,rgnVert[i],rgnVert[(i+1)%4])||!IsInLineLeft(line.endPt,rgnVert[i],rgnVert[(i+1)%4]))
						break;	//�߶�������һ��˵㲻�ھ��ο���
				}
				if(i==4)
				{	//ȫ���߶ζ�λ�ھ��ο���
					if(bPush)
						edgeList.pop_stack();
					return true;
				}
			}
		}
	}
	if(bPush)
		edgeList.pop_stack();
	bPush=cirList.push_stack();
	for(f3dCircle *pCir=cirList.GetFirst();pCir;pCir=cirList.GetNext())
	{
		double radius_2=pCir->radius*pCir->radius;
		double double_radius_2=2*radius_2;
		bool inside=true;
		for(int i=0;i<4;i++)
		{
			GEPOINT curr=rgnVert[i];
			GEPOINT next=rgnVert[(i+1)%4];
			double scale_s2e=-1;
			double dist=DistOf2dPtLine(pCir->centre.x,pCir->centre.y,curr.x,curr.y,next.x,next.y,&scale_s2e);
			if(dist<0)	//Բ������ھ��ο���,��һ�������бߵ����
				inside=false;
			if(fabs(dist)<pCir->radius&&scale_s2e>-EPS&&scale_s2e<1+EPS)
			{
				if(bPush)
					cirList.pop_stack();
				return true;	//����
			}
			/*double dx=rgnVert[i].x-pCir->centre.x;
			double dy=rgnVert[i].y-pCir->centre.y;
			double dist_2=dx*dx+dy*dy;
			if(dist_2>radius_2&&dist_2<double_radius_2)
			{
				if(bPush)
					cirList.pop_stack();
				return true;	//����
			}*/
		}
		if(inside)
		{
			if(bPush)
				cirList.pop_stack();
			return true;	//����
		}
	}
	if(bPush)
		cirList.pop_stack();
	bPush=pureTextDimList.push_stack();
	for(CTextOnlyDrawing *pPureTextDim=pureTextDimList.GetFirst();pPureTextDim;pPureTextDim=pureTextDimList.GetNext())
	{
		if(pPureTextDim==pExcusiveSelfDrawObj)
			continue;
		f3dPoint rgnTempVert[4];
		CTextOnlyDrawing text=*pPureTextDim;
		if(pPureTextDim->cFlag==0)
			text.dimPos*=zoomScaleCoef;
		text.GetDimRect(rgnTempVert,safeTextSize);
		if(IsRectInters(rgnTempVert,rgnVert))
		{
			if(bPush)
				pureTextDimList.pop_stack();
			return true;
		}
	}
	if(bPush)
		pureTextDimList.pop_stack();
	bPush=dataPointList.push_stack();
	for(CTextFrame *pDataPoint=dataPointList.GetFirst();pDataPoint;pDataPoint=dataPointList.GetNext())
	{
		if(pDataPoint==pExcusiveSelfDrawObj)
			continue;
		f3dPoint rgnTempVert[4];
		CTextFrame frame=*pDataPoint;
		if(pDataPoint->cFlag==0)
			frame.dimPos*=zoomScaleCoef;
		frame.GetDimRect(rgnTempVert);
		if(IsRectInters(rgnTempVert,rgnVert))
		{
			if(bPush)
				dataPointList.pop_stack();
			return true;
		}
	}
	if(bPush)
		dataPointList.pop_stack();
	bPush=spaceDimList.push_stack();
	for(CSizeTextDim *pSpaceDim=spaceDimList.GetFirst();pSpaceDim;pSpaceDim=spaceDimList.GetNext())
	{
		if(pSpaceDim==pExcusiveSelfDrawObj)
			continue;
		f3dPoint rgnTempVert[4];
		//��ǰ���麸ͼ�������б�עλ������
		CSizeTextDim sizedim=*pSpaceDim;
		if(pSpaceDim->cFlag==0)
		{
			sizedim.dimPos*=zoomScaleCoef;
			sizedim.dimStartPos*=zoomScaleCoef;
			sizedim.dimEndPos*=zoomScaleCoef;
			sizedim.origin*=zoomScaleCoef;
		}
		sizedim.GetDimRect(rgnTempVert,TRUE,safeTextSize);
		if(IsRectInters(rgnTempVert,rgnVert))
		{
			if(bPush)
				spaceDimList.pop_stack();
			return true;
		}
	}
	if(bPush)
		spaceDimList.pop_stack();
	bPush=angularDimList.push_stack();
	
	for(CSizeAngleDim *pAngular=angularDimList.GetFirst();pAngular;pAngular=angularDimList.GetNext())
	{
		if(pAngular==pExcusiveSelfDrawObj)
			continue;
		f3dPoint rgnTempVert[4];
		CSizeAngleDim angledim=*pAngular;
		if(pAngular->cFlag==0)
		{
			angledim.dimPos*=zoomScaleCoef;
			angledim.dimStartPos*=zoomScaleCoef;
			angledim.dimEndPos*=zoomScaleCoef;
		}
		angledim.GetDimTextRect(rgnTempVert,safeTextSize);
		if(IsRectInters(rgnTempVert,rgnVert))
		{
			if(bPush)
				angularDimList.pop_stack();
			return true;
		}
		f2dArc arc=pAngular->GetDimArcLine();
		double PX,PY;
		for(int j=0;j<4;j++)
		{
			f2dLine line2d(rgnVert[j],rgnVert[(j+1)%4]);
			if(Int2dla(line2d,arc,PX,PY,PX,PY)>0)
			{
				if(bPush)
					angularDimList.pop_stack();
				return true;
			}
		}
	}
	if(bPush)
		angularDimList.pop_stack();
	return false;
}
CSectionViewDrawing::SEARCH_CELL::SEARCH_CELL(int x/*=0*/,int y/*=0*/)
{
	xi=x;yj=y;
	weight=CalPriorWeight(x,y);
}
double CSectionViewDrawing::SEARCH_CELL::CalPriorWeight(int x,int y)
{
	double dx=x*fHoriPriorCoef;
	double dy=y*fVertPriorCoef;
	return (dx*dx+dy*dy);
}
double CSectionViewDrawing::SEARCH_CELL::fHoriPriorCoef=0.5;
double CSectionViewDrawing::SEARCH_CELL::fVertPriorCoef=0.5;
static int _LocalCompareSearchCell(const CSectionViewDrawing::SEARCH_CELL& cell1,const CSectionViewDrawing::SEARCH_CELL& cell2)
{
	if(cell1.weight<cell2.weight)
		return -1;
	else if(cell1.weight>cell2.weight)
		return 1;
	else if(cell1.xi>cell2.xi)
		return -1;	//Ȩ����ͬʱ������X������
	else if(cell1.xi<cell2.xi)
		return 1;
	else if(cell1.yj>cell2.yj)
		return -1;	//Ȩ�ؼ�Xƫ����ͬʱ������Y������
	else if(cell1.yj<cell2.yj)
		return 1;
	return 0;
}
bool CSectionViewDrawing::SearchNoneIntersPosEx(f3dPoint rgnSrcVert[4],const double* vcSearchFlowDirection,
	double maxHoriSearchDist/*=5.0*/,double maxVertSearchDist/*=5*/,
	double fHoriPriorCoef/*=0.5*/,double fVertPriorCoef/*=0.5*/,char cbDualDirectMode/*=0x0f*/,void *pExcusiveSelfDrawObj/*=NULL*/)
{
	const BYTE DIRECT_X			=0x01;
	const BYTE DIRECT_X_INVERSE =0x02;
	const BYTE DIRECT_Y			=0x04;
	const BYTE DIRECT_Y_INVERSE =0x08;
	int i,j;
	SEARCH_CELL::fHoriPriorCoef=fHoriPriorCoef>EPS?1/fHoriPriorCoef:1e10;
	SEARCH_CELL::fVertPriorCoef=fVertPriorCoef>EPS?1/fVertPriorCoef:1e10;
	CXhSimpleList<SEARCH_CELL> listCells;
	for(i=0;i<=maxHoriSearchDist;i++)
	{
		for(j=0;j<=maxVertSearchDist;j++)
		{
			if(i==0&&j==0)
				continue;
			if(i==0||(i>=0&&(cbDualDirectMode&DIRECT_X)))
			{
				if(j==0||(j>=0&&(cbDualDirectMode&DIRECT_Y)))
					listCells.AttachObject(SEARCH_CELL(i,j));
				if(j>0&&(cbDualDirectMode&DIRECT_Y_INVERSE))
					listCells.AttachObject(SEARCH_CELL(i,-j));
			}
			if(i>0&&(cbDualDirectMode&DIRECT_X_INVERSE))
			{
				if(j==0||(j>=0&&(cbDualDirectMode&DIRECT_Y)))
					listCells.AttachObject(SEARCH_CELL(-i,j));
				if(j>0&&(cbDualDirectMode&DIRECT_Y_INVERSE))
					listCells.AttachObject(SEARCH_CELL(-i,-j));
			}
		}
	}
	i=0;
	DYN_ARRAY<SEARCH_CELL> cells(listCells.Count);
	for(SEARCH_CELL* pCell=listCells.EnumObjectFirst();pCell;pCell=listCells.EnumObjectNext(),i++)
		cells[i]=*pCell;
	CQuickSort<SEARCH_CELL>::QuickSort(cells,cells.Size(),_LocalCompareSearchCell);
	f3dPoint origin,rgnVert[4];
	GEPOINT horiSearchVec(vcSearchFlowDirection[0],vcSearchFlowDirection[1]);	//������������
	normalize(horiSearchVec);
	GEPOINT vertSearchVec(-horiSearchVec.y,horiSearchVec.x);;
	for(i=0;i<(int)cells.Size();i++)
	{
		origin=cells[i].xi*horiSearchVec+cells[i].yj*vertSearchVec;
		rgnVert[0]=rgnSrcVert[0]+origin;
		rgnVert[1]=rgnSrcVert[1]+origin;
		rgnVert[2]=rgnSrcVert[2]+origin;
		rgnVert[3]=rgnSrcVert[3]+origin;
		
		if(IsHasIntersObj(rgnVert,pExcusiveSelfDrawObj))
			continue;
		else
		{
			rgnSrcVert[0]=rgnVert[0];
			rgnSrcVert[1]=rgnVert[1];
			rgnSrcVert[2]=rgnVert[2];
			rgnSrcVert[3]=rgnVert[3];
			return true;
		}
	}
	return false;
}
//Ϊ�����Ч���������ֱ������������ǵ�λ������,�ٶ�basePosλ�ھ�������
bool CSectionViewDrawing::SearchNoneIntersPos(f3dPoint rgnSrcVert[4],const double* vcVertSearchVec,
		double maxHroiSearchStep/*=5.0*/,int maxVertSearchStep/*=5*/,bool bEnableHoriSearch/*=true*/,
		bool bSearchVertPrefer/*=true*/,bool bDuoDirect/*=true*/)
{
	f2dRect rect;
	bool bInters;
	SCOPE_STRU scope;
	f3dPoint origin,rgnVert[4];
	f3dPoint vertSearchVec(vcVertSearchVec);
	f3dPoint horiSearchVec(vertSearchVec.y,-vertSearchVec.x);	//������������
	int sumSearchStep=maxVertSearchStep*8;
	if(!IsHasIntersObj(rgnSrcVert))
		return true;
	if(bEnableHoriSearch&&bSearchVertPrefer)
	{	//��ֱ������������
		for(int i=0;i<maxVertSearchStep;i++)
		{
			for(int signHori=0;signHori<2;signHori++)
			{
				if(signHori==0)
				{
					origin.x=i*horiSearchVec.x;
					origin.y=i*horiSearchVec.y;
				}
				else if(bDuoDirect&&signHori==1)
				{	//˫������ʱ�ķ�������
					origin.x=-i*horiSearchVec.x;
					origin.y=-i*horiSearchVec.y;
				}
				for(int j=0;j<maxVertSearchStep;j++)
				{
					for(int signVert=0;signVert<2;signVert++)
					{
						if(signVert==0)
						{
							origin.x+=j*vertSearchVec.x;
							origin.y+=j*vertSearchVec.y;
						}
						else if(signVert==1)
						{
							origin.x-=j*vertSearchVec.x;
							origin.y-=j*vertSearchVec.y;
						}
						rgnVert[0]=rgnSrcVert[0]+origin;
						rgnVert[1]=rgnSrcVert[1]+origin;
						rgnVert[2]=rgnSrcVert[2]+origin;
						rgnVert[3]=rgnSrcVert[3]+origin;
						bInters=IsHasIntersObj(rgnVert);
						if(j==0)
							break;
						if(bInters)
							continue;
						else
						{
							rgnSrcVert[0]=rgnVert[0];
							rgnSrcVert[1]=rgnVert[1];
							rgnSrcVert[2]=rgnVert[2];
							rgnSrcVert[3]=rgnVert[3];
							return true;
						}
					}
				}
				if(i==0)
					break;
			}
		}
	}
	else
	{
		for(int i=0;i<sumSearchStep;i++)
		{
			int stepI=i/8+1;
			if(!bEnableHoriSearch)	//ֻ����ֱ��������
			{
				if(i%8==0)
				{
					origin.x=stepI*vertSearchVec.x;
					origin.y=stepI*vertSearchVec.y;
				}
				else if(bDuoDirect&&i%8==1)
				{
					origin.x=-stepI*vertSearchVec.x;
					origin.y=-stepI*vertSearchVec.y;
				}
				else
					continue;
				rgnVert[0]=rgnSrcVert[0]+origin;
				rgnVert[1]=rgnSrcVert[1]+origin;
				rgnVert[2]=rgnSrcVert[2]+origin;
				rgnVert[3]=rgnSrcVert[3]+origin;
				bInters=IsHasIntersObj(rgnVert);
				if(bInters)
					continue;
				else
				{
					rgnSrcVert[0]=rgnVert[0];
					rgnSrcVert[1]=rgnVert[1];
					rgnSrcVert[2]=rgnVert[2];
					rgnSrcVert[3]=rgnVert[3];
					return true;
				}
			}
			else//˫�������ֻ�����:  6 0 4
			{	//		             3 o 2
			    //		             5 1 7
				if(i%8==0)
				{
					origin.x=stepI*vertSearchVec.x;
					origin.y=stepI*vertSearchVec.y;
				}
				else if(i%8==1)
				{
					origin.x=-stepI*vertSearchVec.x;
					origin.y=-stepI*vertSearchVec.y;
				}
				else if(i%8==2)
				{
					origin.x=stepI*horiSearchVec.x;
					origin.y=stepI*horiSearchVec.y;
				}
				else if(i%8==3)
				{
					origin.x=-stepI*horiSearchVec.x;
					origin.y=-stepI*horiSearchVec.y;
				}
				else if(i%8==4)
				{
					origin.x=stepI*horiSearchVec.x+stepI*vertSearchVec.x;
					origin.y=stepI*horiSearchVec.y+stepI*vertSearchVec.y;
				}					  
				else if(i%8==5)		  
				{					  
					origin.x=-stepI*horiSearchVec.x-stepI*vertSearchVec.x;
					origin.y=-stepI*horiSearchVec.y-stepI*vertSearchVec.y;
				}					  
				else if(i%8==6)		  
				{					  
					origin.x=-stepI*horiSearchVec.x+stepI*vertSearchVec.x;
					origin.y=-stepI*horiSearchVec.y+stepI*vertSearchVec.y;
				}					  
				else //if(i%8==7)		  
				{					  
					origin.x=stepI*horiSearchVec.x-stepI*vertSearchVec.x;
					origin.y=stepI*horiSearchVec.y-stepI*vertSearchVec.y;
				}
				rgnVert[0]=rgnSrcVert[0]+origin;
				rgnVert[1]=rgnSrcVert[1]+origin;
				rgnVert[2]=rgnSrcVert[2]+origin;
				rgnVert[3]=rgnSrcVert[3]+origin;
				bInters=IsHasIntersObj(rgnVert);
				if(bInters)
					continue;
				else
				{
					rgnSrcVert[0]=rgnVert[0];
					rgnSrcVert[1]=rgnVert[1];
					rgnSrcVert[2]=rgnVert[2];
					rgnSrcVert[3]=rgnVert[3];
					return true;
				}
			}
		}
	}
	return false;
}

//����ı���ע�������״̬
void CSectionViewDrawing::ClearDimItemFlag()
{
	//���Գߴ��ע
	for(CTextOnlyDrawing *pTextDrawing=pureTextDimList.GetFirst();pTextDrawing;pTextDrawing=pureTextDimList.GetNext())
		pTextDrawing->cFlag=0;
	//��Գߴ��ע
	for(CSizeTextDim *pSizeDim=spaceDimList.GetFirst();pSizeDim;pSizeDim=spaceDimList.GetNext())
		pSizeDim->cFlag=0;
	//�Ƕȱ�ע
	for(CSizeAngleDim *pAngleDim=angularDimList.GetFirst();pAngleDim;pAngleDim=angularDimList.GetNext())
		pAngleDim->cFlag=0;
	//�������ݵ��ע
	for(CTextFrame *pTextFrame=dataPointList.GetFirst();pTextFrame;pTextFrame=dataPointList.GetNext())
		pTextFrame->cFlag=0;
}
CSectionViewDrawing::SECTSYMBOLDIM::SECTSYMBOLDIM()
{
	fSymbolFlowAngle=0;	//������
	hAcadMode=AcDb::kTextCenter;	//ACAD��עʱ���ı����뷽ʽ
	vAcadMode=AcDb::kTextMid;
}
bool CSectionViewDrawing::CalSectViewSymbolDimPos(CTextOnlyDrawing* pSectSymbol,
			f3dPoint rgnVert[4],double fSymbolFontSize/*=2.0*/,SECTSYMBOLDIM* pxDimPos/*=NULL*/)
{
	if(fSymbolFontSize<EPS)
		fSymbolFontSize=pSectSymbol->textHeight;
	double margin=fSymbolFontSize*0.25;//����������ͼ�����߼�ļ�϶
	f3dPoint vSectPrjVec(1,0,0);
	RotateVectorAroundVector(vSectPrjVec,pSectSymbol->angle,f3dPoint(0,0,1));
	f3dPoint vertSearchVec(-vSectPrjVec.y,vSectPrjVec.x);	//��ֱ��������
	if(pSectSymbol->bInXWing)
		vertSearchVec*=-1;	//�����ͶӰ�����²�(��ת90��Ϊ�ϲ�,��ת90��Ϊ�²�) wjh-2018.3.13
	GEPOINT xOrg=pSectSymbol->dimPos+pSectSymbol->origin;
	rgnVert[0]=xOrg;
	rgnVert[1]=rgnVert[0]+vertSearchVec*(fSymbolFontSize+margin+margin);
	rgnVert[2]=rgnVert[1]+vSectPrjVec*(fSymbolFontSize+margin+margin);
	rgnVert[3]=rgnVert[0]+vSectPrjVec*(fSymbolFontSize+margin+margin);
	//�Ŵ�������Χ
	double MARGIN=3;
	rgnVert[0]+=(-vSectPrjVec*MARGIN-vertSearchVec*MARGIN);
	rgnVert[1]+=(-vSectPrjVec*MARGIN);
	rgnVert[2]+=( vSectPrjVec*MARGIN);
	rgnVert[3]+=( vSectPrjVec*MARGIN-vertSearchVec*MARGIN);
	if(pxDimPos)
	{
		pxDimPos->vFlowDirect=vSectPrjVec;
		pxDimPos->vVertDirect=vertSearchVec;
		pxDimPos->xLineStart=xOrg;
		pxDimPos->xLineEnd=xOrg+ vertSearchVec*(fSymbolFontSize+margin*2);
		pxDimPos->xPrjTipPoint=pxDimPos->xLineEnd+vSectPrjVec*(fSymbolFontSize+margin*2);
		if(pSectSymbol->angle<=0.5||pSectSymbol->angle>=Pi+Pi-0.5)	//0.5��30��
		{	//�ӽ�ˮƽX+���������ͼ,���尴ԭʼͶӰ�����ע
			pxDimPos->fSymbolFlowAngle=pSectSymbol->angle;	//����ͼ���ı���ע����Ƕ�,������
			pxDimPos->hAcadMode=AcDb::kTextLeft;
			pxDimPos->vAcadMode=AcDb::kTextVertMid;	//ACAD��עʱ���ı����뷽ʽ
			pxDimPos->xTextDimPos=xOrg+vSectPrjVec*margin+vertSearchVec*(fSymbolFontSize*0.5+margin);
		}
		else if(pSectSymbol->angle<Pi-0.5)
		{	//���ӽ�ˮƽ����,�ҳ�Y+���������ͼ,���尴ԭʼͶӰ����˳ʱ��ת90���ע�ı�
			pxDimPos->fSymbolFlowAngle=pSectSymbol->angle-Pi/2;	//����ͼ���ı���ע����Ƕ�,������
			pxDimPos->hAcadMode=AcDb::kTextCenter;
			pxDimPos->vAcadMode=AcDb::kTextBottom;	//ACAD��עʱ���ı����뷽ʽ
			pxDimPos->xTextDimPos=xOrg+vSectPrjVec*margin+vertSearchVec*(fSymbolFontSize*0.5+margin);
		}
		else if(pSectSymbol->angle<=Pi+0.5)
		{	//�ӽ�ˮƽX-���������ͼ,���尴ԭʼͶӰ�����ע
			pxDimPos->fSymbolFlowAngle=pSectSymbol->angle-Pi;	//����ͼ���ı���ע����Ƕ�,������
			pxDimPos->hAcadMode=AcDb::kTextRight;
			pxDimPos->vAcadMode=AcDb::kTextVertMid;	//ACAD��עʱ���ı����뷽ʽ
			pxDimPos->xTextDimPos=xOrg+vSectPrjVec*margin+vertSearchVec*(fSymbolFontSize*0.5+margin);
		}
		else //if(pSectSymbol->angle<Pi+Pi-0.5)
		{	//���ӽ�ˮƽ����,�ҳ�Y-���������ͼ,���尴ԭʼͶӰ������ʱ��ת90���ע�ı�
			pxDimPos->fSymbolFlowAngle=pSectSymbol->angle+Pi*0.5;	//����ͼ���ı���ע����Ƕ�,������
			if(pxDimPos->fSymbolFlowAngle>Pi*2)
				pxDimPos->fSymbolFlowAngle=Pi*2;
			pxDimPos->hAcadMode=AcDb::kTextCenter;
			pxDimPos->vAcadMode=AcDb::kTextTop;	//ACAD��עʱ���ı����뷽ʽ
			pxDimPos->xTextDimPos=xOrg+vSectPrjVec*margin+vertSearchVec*(fSymbolFontSize*0.5+margin);
		}
	}
	return true;
}
void CSectionViewDrawing::RelayoutDrawing(double fSpecZoomCoef/*=0.0*/)
{
	//���ӷ����ʶ���ֱ�ע(����ֱ����)
	ClearDimItemFlag();
	CSizeTextDim *pSizeDim;
	CTextFrame *pTextFrame;
	f3dPoint rgnVertArr[4];
	CTextOnlyDrawing *pTextDrawing;
	SCOPE_STRU usedDimRegion,dimRegion;
	double zoomScaleCoef=fSpecZoomCoef<=EPS?1.0/m_fUser2ScrDrawScale:fSpecZoomCoef;
	double fZoomFontCoef=ZoomToModelTextSize(1.0);	//��������߶ȵ���ͼԪλ�ã��轫����߶ȸ����涯���ű�����������
	SAFE_TEXTSIZE safeTextSize;
	SAFE_TEXTSIZE::minFontSize=sys.part_group_map.fMinFontSize;
	for(IDbEntity* pEnt=this->EnumFirstDbEntity();pEnt;pEnt=this->EnumNextDbEntity())
	{
		if( m_bFinalDrawing&&((sectionEnt.pLine1&&sectionEnt.pLine1->GetId()==pEnt->GetId())||
			(sectionEnt.pLine2&&sectionEnt.pLine2->GetId()==pEnt->GetId())))
			continue;	//������ͼ��Χʱ����������ͼ���ű��� wht 18-03-16
		pEnt->ZoomScale(zoomScaleCoef);
		if(pEnt->GetDbEntType()==IDbEntity::DbLine)
		{
			usedDimRegion.VerifyVertex(((IDbLine*)pEnt)->StartPosition());
			usedDimRegion.VerifyVertex(((IDbLine*)pEnt)->EndPosition());
		}
		else if(pEnt->GetDbEntType()==IDbEntity::DbArcline)
		{
			f3dArcLine arcLine=((IDbArcline*)pEnt)->GetArcLine();
			usedDimRegion.VerifyVertex(arcLine.Start());
			usedDimRegion.VerifyVertex(arcLine.End());
			if(arcLine.SectorAngle()>0&&!arcLine.WorkNorm().IsZero())
			{	//���ΪԲ�����㷶Χʱ����Բ����
				f3dPoint center=arcLine.Center();
				double radius=arcLine.Radius();
				usedDimRegion.VerifyVertex(f3dPoint(center.x+radius,center.y));
				usedDimRegion.VerifyVertex(f3dPoint(center.x,center.y+radius));
				usedDimRegion.VerifyVertex(f3dPoint(center.x-radius,center.y));
				usedDimRegion.VerifyVertex(f3dPoint(center.x,center.y-radius));
			}
		}
		else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
		{
			GEPOINT centre=((IDbCircle*)pEnt)->Center();
			double radius=((IDbCircle*)pEnt)->Radius();
			usedDimRegion.VerifyVertex(f3dPoint(centre.x+radius,centre.y));
			usedDimRegion.VerifyVertex(f3dPoint(centre.x,centre.y+radius));
			usedDimRegion.VerifyVertex(f3dPoint(centre.x-radius,centre.y));
			usedDimRegion.VerifyVertex(f3dPoint(centre.x,centre.y-radius));
		}
		else if(pEnt->GetDbEntType()==IDbEntity::DbAngularDimension||
			pEnt->GetDbEntType()==IDbEntity::DbAlignedDimension||
			pEnt->GetDbEntType()==IDbEntity::DbRotatedDimension)
		{
			GEPOINT rgnVert[4];
			if(((IDbDimension*)pEnt)->GetDimRect(rgnVert,1.0))
			{
				for(int i=0;i<4;i++)
					usedDimRegion.VerifyVertex(rgnVert[i]);
			}
		}
	}
	for(ACAD_HATCH *pHatch=hatchList.GetFirst();pHatch;pHatch=hatchList.GetNext())
		pHatch->ZoomScale(zoomScaleCoef);
	for(f3dArcLine *pArcLine=edgeList.GetFirst();pArcLine;pArcLine=edgeList.GetNext())
	{
		pArcLine->ZoomScale(zoomScaleCoef);
		usedDimRegion.VerifyVertex(pArcLine->Start());
		usedDimRegion.VerifyVertex(pArcLine->End());
		if(pArcLine->SectorAngle()>0&&!pArcLine->WorkNorm().IsZero())
		{	//���ΪԲ�����㷶Χʱ����Բ����
			f3dPoint center=pArcLine->Center();
			double radius=pArcLine->Radius();
			usedDimRegion.VerifyVertex(f3dPoint(center.x+radius,center.y));
			usedDimRegion.VerifyVertex(f3dPoint(center.x,center.y+radius));
			usedDimRegion.VerifyVertex(f3dPoint(center.x-radius,center.y));
			usedDimRegion.VerifyVertex(f3dPoint(center.x,center.y-radius));
		}
	}
	for(f3dCircle *pCir=cirList.GetFirst();pCir;pCir=cirList.GetNext())
	{
		pCir->centre*=zoomScaleCoef;
		pCir->radius*=zoomScaleCoef;
		usedDimRegion.VerifyVertex(f3dPoint(pCir->centre.x+pCir->radius,pCir->centre.y));
		usedDimRegion.VerifyVertex(f3dPoint(pCir->centre.x,pCir->centre.y+pCir->radius));
		usedDimRegion.VerifyVertex(f3dPoint(pCir->centre.x-pCir->radius,pCir->centre.y));
		usedDimRegion.VerifyVertex(f3dPoint(pCir->centre.x,pCir->centre.y-pCir->radius));
	}
	//�ֹܶ���ͼ�У����ƽǶȱ�ע����������
	if(tube_base_line.m_iDraw>0&&!m_bFinalDrawing)
		CreateDimAngleFeatureLine(usedDimRegion);
	for(pTextDrawing=pureTextDimList.GetFirst();pTextDrawing;pTextDrawing=pureTextDimList.GetNext())
	{
		if(pTextDrawing->iOrder!=0)
			continue;	//���Ǻ��ӷ����ʶ���ֱ�ע
		pTextDrawing->dimPos*=zoomScaleCoef;
		pTextDrawing->cFlag=1;
		//50,100������λ��
		/*
		//����������õ����� wht 10-09-16
		pTextDrawing->GetDimRect(rgnVertArr,safeTextSize);
		f3dPoint searchPos,startPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
		BOOL bPush=pureTextDimList.push_stack();
		double fTextHeight=safeTextSize.EnsureSize(pTextDrawing->textHeight); //�����������߶� 
		if(SearchNoneIntersPos(rgnVertArr,f3dPoint(0,1,0),fTextHeight/2,4,false))
		{
			searchPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
			pTextDrawing->dimPos+=searchPos-startPos;
		}
		else
		{
			if(pTextDrawing->dimPos.y>0)
				pTextDrawing->dimPos.y+=fTextHeight*2;
			else
				pTextDrawing->dimPos.y-=fTextHeight*2;
		}
		if(bPush)
			pureTextDimList.pop_stack();*/
		//����λ�ú����»�þ��ζ��� wht 10-09-16
		pTextDrawing->GetDimRect(rgnVertArr,safeTextSize);
		for(int i=0;i<4;i++)
			usedDimRegion.VerifyVertex(rgnVertArr[i]);
	}
	//����ͼ��
	dimRegion=usedDimRegion;
	for(pTextDrawing=pureTextDimList.GetFirst();pTextDrawing;pTextDrawing=pureTextDimList.GetNext())
	{
		if(pTextDrawing->iOrder==4)
			pTextDrawing->dimPos*=zoomScaleCoef;
		if(abs(pTextDrawing->iOrder)!=3)
			continue;	//��������ͼ��
		BOOL pushed=pureTextDimList.push_stack();
		CTextOnlyDrawing* pCoupleDrawing=pureTextDimList.GetNext();
		if(pCoupleDrawing&&!pTextDrawing->dimText.Equal(pCoupleDrawing->dimText))
			pCoupleDrawing=NULL;
		pureTextDimList.pop_stack(pushed);

		CTextOnlyDrawing* textDrawingArr[2]={pTextDrawing,pCoupleDrawing};
		pushed=pureTextDimList.push_stack();//CSectionViewDrawing::IsHasIntersObj�л����pureTextDimList��wjh-2016.10.19
		double fSearchStep=pTextDrawing->textHeight*fZoomFontCoef*0.5;
		for(int k=0;k<2;k++)
		{
			if(textDrawingArr[k]==NULL)
				break;
			double fSymbolFontSize=safeTextSize.EnsureSize(textDrawingArr[k]->textHeight);
			textDrawingArr[k]->origin*=zoomScaleCoef;
			textDrawingArr[k]->dimPos*=zoomScaleCoef;
			for(int k2=0;k2<100;k2++)
			{
				SECTSYMBOLDIM dimsymbol;
				CalSectViewSymbolDimPos(textDrawingArr[k],rgnVertArr,fSymbolFontSize,&dimsymbol);
				f3dPoint searchPos,startPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
				bool hasIntersObj=IsHasIntersObj(rgnVertArr,textDrawingArr[k]);
				if(!hasIntersObj)
					break;
				for(int j=0;j<4;j++)
					rgnVertArr[j]+=fSearchStep*dimsymbol.vVertDirect;
				textDrawingArr[k]->dimPos+=fSearchStep*dimsymbol.vVertDirect;
			}
			textDrawingArr[k]->ResetPosition(AlignDefault,m_bFinalDrawing);
		}
		pureTextDimList.pop_stack(pushed);
		if(textDrawingArr[1])
			pureTextDimList.GetNext();
	}
	//װ����ߴ�
	double text_height=safeTextSize.EnsureSize(sys.fonts.weld.fDimTextSize);
	for(pSizeDim=spaceDimList.GetFirst();pSizeDim;pSizeDim=spaceDimList.GetNext())
	{
		if(pSizeDim->iOrder!=0)
			continue;	//����װ����ߴ�
		pSizeDim->dimPos*=zoomScaleCoef;
		pSizeDim->dimStartPos*=zoomScaleCoef;
		pSizeDim->dimEndPos*=zoomScaleCoef;
		pSizeDim->origin*=zoomScaleCoef;
		pSizeDim->cFlag=1;
		//����������õ����� wht 10-09-16
		pSizeDim->GetDimRect(rgnVertArr,TRUE,safeTextSize);
		f3dPoint searchPos,startPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
		if(pSizeDim->bAllowRelayout)
		{	//֧���Զ�������עλ�� wht 10-08-15
			BOOL bPush=spaceDimList.push_stack();
			//double text_height=CSizeTextDim::dimStyle.textHeight*fZoomFontCoef;
			if(SearchNoneIntersPos(rgnVertArr,f3dPoint(0,1,0),text_height/2,8,false))
			{
				searchPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
				pSizeDim->dimPos+=searchPos-startPos;
			}
			else
			{
				//�ر�ע�Ƕ���ʱ����ת90��
				f3dPoint vertVec(-sin(pSizeDim->angle),cos(pSizeDim->angle));
				if(pSizeDim->dimPos.y>0)	//�ϲ�����
					pSizeDim->dimPos+=vertVec*(text_height*2);
				else						//�²�����
					pSizeDim->dimPos-=vertVec*(text_height*2);
			}
			if(bPush)
				spaceDimList.pop_stack();
			//����λ�ú����»�þ��ζ��� wht 10-09-16
			pSizeDim->GetDimRect(rgnVertArr,TRUE,safeTextSize);
		}
		for(int i=0;i<4;i++)
			usedDimRegion.VerifyVertex(rgnVertArr[i]);
	}
	//�ֲ�ƫ�Ƽ��ߴ�(���޸�λ��)
	for(pSizeDim=spaceDimList.GetFirst();pSizeDim;pSizeDim=spaceDimList.GetNext())
	{	//�ֲ�ƫ�Ƴߴ��ע������²� wht 10-08-11
		if(abs(pSizeDim->iOrder)!=1)
			continue;	//�ֲ�ƫ�Ƽ��ߴ�
		pSizeDim->initDimPos*=zoomScaleCoef;
		pSizeDim->dimPos*=zoomScaleCoef;
		pSizeDim->dimStartPos*=zoomScaleCoef;
		pSizeDim->dimEndPos*=zoomScaleCoef;
		pSizeDim->origin*=zoomScaleCoef;
		pSizeDim->cFlag=1;
		if(pSizeDim->bAllowRelayout&&fabs(pSizeDim->angle-0.5*Pi)<EPS)
		{	//ֻ������ֱ��ע��Ŀǰ����������ֹܶ�ͷ�����˨�м���ע wht 18-09-17
			f3dPoint vertVec(-sin(pSizeDim->angle),cos(pSizeDim->angle));
			f3dPoint verfiyVec=pSizeDim->initDimPos-0.5*(pSizeDim->dimStartPos+pSizeDim->dimEndPos);
			normalize(verfiyVec);
			if(verfiyVec*vertVec<0)
				vertVec*=-1.0;
			pSizeDim->dimPos=pSizeDim->initDimPos+vertVec*1.5*text_height;
		}
		else
			pSizeDim->origin.y=text_height;//CSizeTextDim::dimStyle.style.arrowSize;
		//����������õ����� wht 10-09-16
		pSizeDim->GetDimRect(rgnVertArr,FALSE,safeTextSize);
		for(int i=0;i<4;i++)
			usedDimRegion.VerifyVertex(rgnVertArr[i]);
	}
	double dimSizeGap=TMADRAWING_DIM_STYLE::CalGap(text_height);
	double dimSizeTextHeight=sys.fonts.weld.fDimTextSize;//CSizeTextDim::dimStyle.textHeight;
	//�Ŷ�������·���ע
	dimRegion=usedDimRegion;
	safeTextSize.textsize=sys.fonts.weld.fDimTextSize;
	for(pSizeDim=spaceDimList.GetFirst();pSizeDim;pSizeDim=spaceDimList.GetNext())
	{
		if(abs(pSizeDim->iOrder)!=2)
			continue;	//�Ŷ�����ע
		pSizeDim->dimPos*=zoomScaleCoef;
		pSizeDim->dimStartPos*=zoomScaleCoef;
		pSizeDim->dimEndPos*=zoomScaleCoef;
		pSizeDim->origin*=zoomScaleCoef;
		pSizeDim->cFlag=1;
		pSizeDim->ResetPosition(AlignDefault,m_bFinalDrawing);
		double text_height=sys.fonts.weld.fDimTextSize;//CSizeTextDim::dimStyle.textHeight;
		pSizeDim->origin.y=dimRegion.fMinY-text_height*0.5-dimSizeGap;
		pSizeDim->GetDimRect(rgnVertArr,FALSE,safeTextSize);
		for(int j=0;j<4;j++)
			usedDimRegion.VerifyVertex(rgnVertArr[j]);
	}
	//�Ƕȱ�ע
	for(CSizeAngleDim *pAngleDim=angularDimList.GetFirst();pAngleDim;pAngleDim=angularDimList.GetNext())
	{
		f3dPoint vertVec=pAngleDim->dimPos-pAngleDim->vertex;
		f3dPoint startVec=pAngleDim->dimStartPos-pAngleDim->vertex;
		f3dPoint endVec=pAngleDim->dimEndPos-pAngleDim->vertex;
		normalize(vertVec);
		normalize(startVec);
		normalize(endVec);
		pAngleDim->vertex*=zoomScaleCoef;
		pAngleDim->dimPos*=zoomScaleCoef;
		pAngleDim->dimStartPos*=zoomScaleCoef;
		pAngleDim->dimEndPos*=zoomScaleCoef;
		pAngleDim->cFlag=1;
		pAngleDim->GetDimTextRect(rgnVertArr,safeTextSize);
		/*f3dPoint searchPos,startPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
		BOOL bPush=angularDimList.push_stack();
		if(SearchNoneIntersPos(rgnVertArr,vertVec,text_height/2,30*ftoi(fZoomFontCoef),false,true,false))
		{
			searchPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
			pAngleDim->dimPos+=searchPos-startPos;
			//����λ�ú����»�þ��ζ��� wht 10-09-16
			pAngleDim->GetDimTextRect(rgnVertArr,fZoomFontCoef);
		}
		if(bPush)
			angularDimList.pop_stack();*/
		for(int i=0;i<4;i++)
			usedDimRegion.VerifyVertex(rgnVertArr[i]);
		double dimRadius=DISTANCE(pAngleDim->vertex,pAngleDim->dimPos);
		f3dPoint verifyStart=pAngleDim->vertex+startVec*dimRadius;
		f3dPoint verifyEnd=pAngleDim->vertex+endVec*dimRadius;
		usedDimRegion.VerifyVertex(verifyStart);
		usedDimRegion.VerifyVertex(verifyEnd);
	}
	//װ�䶨λ��Լ��ߴ�
	dimRegion=usedDimRegion;
	for(pSizeDim=spaceDimList.GetFirst();pSizeDim;pSizeDim=spaceDimList.GetNext())
	{
		if(pSizeDim->iOrder!=3&&pSizeDim->iOrder!=-3)
			continue;	//װ�䶨λ��Լ��ߴ�
		pSizeDim->dimPos*=zoomScaleCoef;
		pSizeDim->dimStartPos*=zoomScaleCoef;
		pSizeDim->dimEndPos*=zoomScaleCoef;
		pSizeDim->origin*=zoomScaleCoef;
		pSizeDim->cFlag=1;
		pSizeDim->ResetPosition(AlignDefault,m_bFinalDrawing);
		//��������������߶� wht 10-09-14
		//double text_height=CSizeTextDim::dimStyle.textHeight*fZoomFontCoef;
		if(pSizeDim->iOrder==3)
			pSizeDim->origin.y=dimRegion.fMaxY+dimSizeGap*2;
		else if(pSizeDim->iOrder==-3)
			pSizeDim->origin.y=dimRegion.fMinY-text_height*0.5-dimSizeGap;//*0.5;
		pSizeDim->GetDimRect(rgnVertArr,FALSE,safeTextSize);
		for(int j=0;j<4;j++)
			usedDimRegion.VerifyVertex(rgnVertArr[j]);
	}
	//����浽�����ߴ�
	dimRegion=usedDimRegion;
	for(pSizeDim=spaceDimList.GetFirst();pSizeDim;pSizeDim=spaceDimList.GetNext())
	{
		if(pSizeDim->iOrder!=4)
			continue;	//����浽�����ߴ�
		pSizeDim->dimPos*=zoomScaleCoef;
		pSizeDim->dimStartPos*=zoomScaleCoef;
		pSizeDim->dimEndPos*=zoomScaleCoef;
		pSizeDim->origin*=zoomScaleCoef;
		pSizeDim->cFlag=1;
		pSizeDim->ResetPosition(AlignDefault,m_bFinalDrawing);
		//��������������߶� wht 10-09-14
		//double text_height=CSizeTextDim::dimStyle.textHeight*fZoomFontCoef;
		if(pSizeDim->iOrder==4)
			pSizeDim->origin.y=dimRegion.fMaxY+dimSizeGap*2;
		pSizeDim->GetDimRect(rgnVertArr,FALSE,safeTextSize);
		for(int j=0;j<4;j++)
			usedDimRegion.VerifyVertex(rgnVertArr[j]);
	}
	//װ�䶨λ����λ�ö�λ�ߴ�
	dimRegion=usedDimRegion;
	CTextOnlyDrawing *pPrevTextDrawing=NULL;
	for(pTextDrawing=pureTextDimList.GetFirst();pTextDrawing;pTextDrawing=pureTextDimList.GetNext())
	{
		if(pTextDrawing->iOrder!=1&&pTextDrawing->iOrder!=-1)
			continue;	//���Ǻ��ӷ����ʶ���ֱ�ע
		pTextDrawing->origin*=zoomScaleCoef;
		pTextDrawing->dimPos*=zoomScaleCoef;
		pTextDrawing->cFlag=1;
		//��ʱdimPos.y=0���޷��Զ��ж϶������ͣ�������ǰͨ��iOrder�ж� wjh-2018.8.1
		DIMALIGN_TYPE alignType=pTextDrawing->iOrder>0?MiddleLeft:MiddleRight;
		pTextDrawing->ResetPosition(alignType,m_bFinalDrawing);
		//
		safeTextSize.textsize=pTextDrawing->textHeight;
		double text_height=safeTextSize.EnsureSize(); 
		if(pTextDrawing->iOrder>0)
			pTextDrawing->origin.y=dimRegion.fMaxY+text_height*0.1;
		//�²����ֶ��뷽ʽ�޸�ΪMiddleRight֮�󣬱���0.1�ָ߼�϶ֵ���� wht 18-08-26
		else if(pTextDrawing->iOrder<0)	//�²���Գߴ���Ĭ��ƫ��4���ָ�
			pTextDrawing->origin.y=dimRegion.fMinY-text_height*0.1;	//text_height*(4*0.7+0.1);
		//�������������ĳߴ� wht 10-09-17
		if(pPrevTextDrawing&&pPrevTextDrawing->iOrder==pTextDrawing->iOrder
			&&pTextDrawing->origin.x-pPrevTextDrawing->origin.x<text_height)
			pTextDrawing->origin.x=pPrevTextDrawing->origin.x+text_height;
		pTextDrawing->GetDimRect(rgnVertArr,safeTextSize);
		for(int i=0;i<4;i++)
			usedDimRegion.VerifyVertex(rgnVertArr[i]);
		pPrevTextDrawing=pTextDrawing;
	}
	//�ڸֹܶ�����ͼ�У���ͼԪ���в��ִ����ͳһ���ƸֹܵĻ�׼��
	if(tube_base_line.m_iDraw>0&&!m_bFinalDrawing)
	{
		tube_base_line.m_xArrowHead*=zoomScaleCoef;
		tube_base_line.m_xArrowBtm*=zoomScaleCoef;
		tube_base_line.m_xTaCoreVecHead*=zoomScaleCoef;
		tube_base_line.m_xTaCoreVecBtm*=zoomScaleCoef;
		tube_base_line.m_fArrowH*=zoomScaleCoef;
		CreateTubeFeatureLine(usedDimRegion);
		usedDimRegion.VerifyVertex(f3dPoint(tube_base_line.m_xArrowBtm));
		usedDimRegion.VerifyVertex(f3dPoint(tube_base_line.m_xArrowHead));
	}
	//�ڶ��ε���Relayoutʱ,ֻ��Ҫ���������� wht 18-03-16
	if(m_bFinalDrawing)
	{	//����֮��֤��ͷ��С����
		const double MIN_ARROW_LINE_LEN=1.5;
		GEPOINT start,end,len_vec;
		for(int i=0;i<=2;i++)
		{
			IDbLine *pLine=(i==0)?m_pArrowLine1:m_pArrowLine2;
			IDbPolyline *pPolyLine=(i==0)?m_pArrowPolyLine1:m_pArrowPolyLine2;
			if(pLine)
			{
				len_vec=pLine->EndPosition()-pLine->StartPosition();
				if(len_vec.mod()<MIN_ARROW_LINE_LEN)
				{
					normalize(len_vec);
					pLine->SetEndPosition(pLine->StartPosition()+len_vec*2.5);
				}
			}
			if(pPolyLine&&pPolyLine->GetVertexAt(1,start))
			{
				pPolyLine->GetVertexAt(0,end);
				len_vec=end-start;
				if(len_vec.mod()<MIN_ARROW_LINE_LEN)
				{
					normalize(len_vec);
					pPolyLine->ModifyVertexAt(0,GEPOINT(start+len_vec*MIN_ARROW_LINE_LEN));
					pPolyLine->GetVertexAt(2,end);
					len_vec=end-start;
					normalize(len_vec);
					pPolyLine->ModifyVertexAt(2,GEPOINT(start+len_vec*MIN_ARROW_LINE_LEN));
				}
			}
		}
	}
	//��ű�ע(����ҷ��)
	for(pTextFrame=dataPointList.GetFirst();pTextFrame;pTextFrame=dataPointList.GetNext())
	{
		pTextFrame->dataPos*=zoomScaleCoef;
		pTextFrame->dimPos*=zoomScaleCoef;
		pTextFrame->center*=zoomScaleCoef;
		pTextFrame->cFlag=1;
		DATAPOINT_TAG* pTag=m_xDataPointTagHash.GetValue(pTextFrame->dwKeyTag);
		if(!pTag)
			continue;
		for(int i=0;i<pTag->vertexArr.GetSize();i++)
			pTag->vertexArr[i]*=zoomScaleCoef;
	}
	for(pTextFrame=dataPointList.GetFirst();pTextFrame;pTextFrame=dataPointList.GetNext())
	{
		if(pTextFrame->iCurLayer==-1)
			continue;	//�Ѵ�����Ĺ������
		PART_DIMnDRAW* pDimDraw=m_pPartGroupDrawing->GetPartDimDraw(m_pSectionView->GetId(),pTextFrame->hPart);
		if(pDimDraw&&!pDimDraw->bDimPartNo&&!pDimDraw->bMandatoryDimLabel)
			continue;	//����ɸѡ�ڵ�ǰ��ͼ�иù����Ļ��Ʋ�ͼ����ע���
		//����1:1�麸ͼʱ��ʱʹ��ͨ�������������������ұ��λ�� wht 10-09-17
		pTextFrame->ZoomFrameSize(fZoomFontCoef,sys.part_group_map.fMinFontSize);
		pTextFrame->GetDimRect(rgnVertArr);		//CTextFrame��BuildFrame()ʱ�Ѿ����������ű��� wxc-2017.11.21
		f3dPoint searchPos,startPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
		BOOL bPush=dataPointList.push_stack();
		GEPOINT center,vPreferSearchFlowDirection,vVertSearchVec(0,1,0);
		DATAPOINT_TAG* pTag=m_xDataPointTagHash.GetValue(pTextFrame->dwKeyTag);
		if(pTag&&m_pSectionView->IsTubeSectionView())
		{
			POLYGON poly;
			poly.CreatePolygonRgn(pTag->vertexArr.m_pData,pTag->vertexArr.GetSize());
			poly.CalArea(center);
			vPreferSearchFlowDirection=center;

			//f3dCircle cir;cir.centre=center;cir.radius=1;
			//this->AppendCircle(cir);
			//cir.centre=pTextFrame->dataPos;
			//cir.radius=0.5;
			//this->AppendCircle(cir);

			normalize(vPreferSearchFlowDirection);
		}
		double hSearchScope=20,vSearchScope=20;
		if(vPreferSearchFlowDirection.IsZero()&&this->m_pSectionView->IsTubeSectionView())
		{
			vPreferSearchFlowDirection.Set(1,0,0);
			hSearchScope=30;//*sys.fonts.weld.fPartNoTextSize*fZoomFontCoef;
			vSearchScope=2;//0;
		}
		else
			vPreferSearchFlowDirection.Set(1,0,0);
		//f3dCircle cir2;
		//this->AppendLine(f3dLine(rgnVertArr[0],rgnVertArr[1]));
		//this->AppendLine(f3dLine(rgnVertArr[1],rgnVertArr[2]));
		//this->AppendLine(f3dLine(rgnVertArr[2],rgnVertArr[3]));
		//this->AppendLine(f3dLine(rgnVertArr[3],rgnVertArr[0]));
		//cir2.centre=pTextFrame->dimPos;cir2.radius=0.3;
		//this->AppendCircle(cir2);
		if(SearchNoneIntersPosEx(rgnVertArr,vPreferSearchFlowDirection,hSearchScope,vSearchScope,0.9,0.1,0x0f,pTextFrame))
		{
			searchPos=(rgnVertArr[0]+rgnVertArr[2])*0.5;
			pTextFrame->dimPos+=searchPos-startPos;
			//pTextFrame->GetDimRect(rgnVertArr);
			pTextFrame->UpdateDataPos(m_xDataPointTagHash);
			//cir2.centre=pTextFrame->dimPos;cir2.radius=0.2;
			//this->AppendCircle(cir2);
		}
		if(bPush)
			dataPointList.pop_stack();
		if(m_bSamePartNoDimOnce)
		{
			bPush=dataPointList.push_stack();
			for(CTextFrame *pSamePartNoFrame=dataPointList.GetNext();pSamePartNoFrame;pSamePartNoFrame=dataPointList.GetNext())
			{
				PART_DIMnDRAW* pDimDraw=m_pPartGroupDrawing->GetPartDimDraw(pSectionView->Id,pTextFrame->hPart);
				//TODO:pDimDraw��֪Ϊʲô��Ϊ�գ���ǰ��������wjh-2017.10.23
				if(pDimDraw==NULL||pDimDraw->bMandatoryDimLabel)
					continue;
				if(stricmp(pTextFrame->Text(),pSamePartNoFrame->Text())!=0)
					continue;
				if(DISTANCE(pTextFrame->dataPos,pSamePartNoFrame->dataPos)<pTextFrame->Width()*zoomScaleCoef)
				{	//�ҵ�����ͬ��ű�ע���Ҿ���Ͻ�ʱֻ����������߼���
					pSamePartNoFrame->dimPos=pTextFrame->dimPos;
					pSamePartNoFrame->iCurLayer=-1;	//������ı�ű�ע���б��
				}
			}
			if(bPush)
				dataPointList.pop_stack();
		}
		for(int i=0;i<4;i++)
			usedDimRegion.VerifyVertex(rgnVertArr[i]);
	}
	/*���������ע�Ļ�ͼ��������
	IDbEntity* pEnt;
	ATOM_LIST<IDbEntity*> list;
	for(pEnt=this->EnumFirstDbEntity();pEnt;pEnt=this->EnumNextDbEntity())
		list.append(pEnt);
	for(IDbEntity** ppEnt=list.GetFirst();ppEnt;ppEnt=list.GetNext())
	{
		pEnt=*ppEnt;
		if(pEnt->GetDbEntType()!=IDbEntity::DbRotatedDimension)
			continue;
		IDbRotatedDimension* pDimension=(IDbRotatedDimension*)pEnt;
		GEPOINT rgnVert[4];
		pDimension->GetDimRect(rgnVert);
		for(int j=0;j<4;j++)
		{
			IDbLine* pLine=(IDbLine*)AppendDbEntity(IDbEntity::DbLine);
			pLine->SetStartPosition(rgnVert[j]);
			pLine->SetEndPosition(rgnVert[(j+1)%4]);
		}
	}*/
	//��ǰ��ͼ���Ʊ�ע
	f2dRect draw_rect=GetDrawRegion();//usedDimRegion;
	dimRegion.VerifyVertex(f3dPoint(draw_rect.topLeft.x,draw_rect.topLeft.y));
	dimRegion.VerifyVertex(f3dPoint(draw_rect.bottomRight.x,draw_rect.bottomRight.y));
	LOCALE_PROPITEM* pLocaleItem=gxDrwLocalizer.GetLocaleItem("WeldMasterMap.DrawSectSymbol");
	bool blMasterMapSectSymbol=pLocaleItem?pLocaleItem->bVal:false;
	if(strlen(m_pSectionView->Symbol)>0 || blMasterMapSectSymbol)
	{	//������ͼ���ƻ��߻�ͼ����
		if(sectionEnt.pLine1==NULL||sectionEnt.pLine2==NULL||sectionEnt.pText==NULL)
		{
			sectionEnt.pText=pureTextDimList.append();
			sectionEnt.pText->angle=0;		//��ת��
			sectionEnt.pText->bNeedConvertDistToText=false;
			sectionEnt.pText->iOrder=2;		//��ͼ����
			sectionEnt.pText->alignType=MiddleCenter;
			sectionEnt.pText->textHeight=1.2*sys.dim_map.fDimTextSize;	//�ݶ�����ͼ����Ϊ1.2����ͨ��ע�ָ�
			//��һ����ӦΪCreateAcadPolyArcLine������ɫPolyline���ߡ�wjh-2014.3.26
			sectionEnt.pLine1=(IDbPolyline*)AppendDbEntity(IDbEntity::DbPolyline);
			sectionEnt.pLine1->SetLayer(LayerTable::VisibleProfileLayer.layerName);
			sectionEnt.pLine1->AddVertexAt(0, GEPOINT(), 0.3,0.3);
			sectionEnt.pLine1->AddVertexAt(1, GEPOINT(), 0.3,0.3);
			sectionEnt.pLine2=(IDbLine*)AppendDbEntity(IDbEntity::DbLine);
			sectionEnt.pLine2->SetLayer(LayerTable::VisibleProfileLayer.layerName);
		}
		//��������������߶� wht 10-09-14
		double text_height=safeTextSize.EnsureSize(sectionEnt.pText->GetHeight());
		sectionEnt.pText->dimPos.Set(0,dimRegion.fMinY-text_height*2);
		if(strlen(m_pSectionView->Symbol)>0)
			sectionEnt.pText->dimText.Printf("%s-%s",(char*)m_pSectionView->Symbol,(char*)m_pSectionView->Symbol);
		else	//���ƻ�ͼ����
			sectionEnt.pText->dimText.Printf(CXhChar16("1:%g",m_fUser2ScrDrawScale));
		//����ͼ�����µĺ���
		GEPOINT lineStart(sectionEnt.pText->dimPos.x-text_height*2,sectionEnt.pText->dimPos.y-text_height);
		GEPOINT lineEnd(sectionEnt.pText->dimPos.x+text_height*2,sectionEnt.pText->dimPos.y-text_height);
		sectionEnt.pLine1->ModifyVertexAt(0,lineStart);
		sectionEnt.pLine1->ModifyVertexAt(1,lineEnd);
		dimRegion.VerifyVertex(lineStart);
		dimRegion.VerifyVertex(lineEnd);
		lineStart.y=lineEnd.y=sectionEnt.pText->dimPos.y-text_height-1;
		sectionEnt.pLine2->SetStartPosition(lineStart);
		sectionEnt.pLine2->SetEndPosition(lineEnd);
		dimRegion.VerifyVertex(lineStart);
		dimRegion.VerifyVertex(lineEnd);
		//�ں�������ʾ��ͼ����
		if(strlen(m_pSectionView->Symbol)>0)// && CLDSPart::CustomerSerial==CLIENT_QingDao_DongFang)
		{
			if(sectionEnt.pScaleDimText==NULL)
				sectionEnt.pScaleDimText=pureTextDimList.append();
			sectionEnt.pScaleDimText->angle=0;
			sectionEnt.pScaleDimText->bNeedConvertDistToText=false;
			sectionEnt.pScaleDimText->iOrder=2;
			sectionEnt.pScaleDimText->alignType=MiddleCenter;
			sectionEnt.pScaleDimText->textHeight=1.2*sys.dim_map.fDimTextSize;
			sectionEnt.pScaleDimText->dimPos.Set(0,dimRegion.fMinY-text_height);
			sectionEnt.pScaleDimText->dimText.Printf("1:%g",m_fUser2ScrDrawScale);
		}
	}
	CreateProcessAidedPlate(usedDimRegion);
}
GEPOINT GetFeatureVecPoint(f3dPoint ray_vec,SCOPE_STRU& region,BOOL bAlongVec=TRUE)
{
	//��ʼ��ָ�����������������ֱ��
	double fMaxY=max(fabs(region.fMinY),fabs(region.fMaxY));
	double fMaxX=max(fabs(region.fMinX),fabs(region.fMaxX));
	double r=max(fMaxX,fMaxY);
	GEPOINT startPt,endPt,intersPt;
	startPt=f3dPoint(0,0,0)+f3dPoint(ray_vec)*(r+10);
	endPt  =f3dPoint(0,0,0)-f3dPoint(ray_vec)*(r+10);
	f2dLine line2d(f2dPoint(startPt.x,startPt.y),f2dPoint(endPt.x,endPt.y));
	//ͨ������Բ�󽻣�����������
	ray_vec*=bAlongVec?1:-1;
	double posX1=0,posX2=0,posY1=0,posY2=0;
	double dd1=ray_vec*f3dPoint(1,0,0);
	double dd2=ray_vec*f3dPoint(0,1,0);
	if(fabs(dd1)>fabs(dd2))
	{
		if(dd1>0)
			Int2dplc(line2d,f2dCircle(fabs(region.fMaxX),f2dPoint(0,0)),posX1,posY1,posX2,posY2);
		else
			Int2dplc(line2d,f2dCircle(fabs(region.fMinX),f2dPoint(0,0)),posX1,posY1,posX2,posY2);
	}
	else
	{
		if(dd2>0)
			Int2dplc(line2d,f2dCircle(fabs(region.fMaxY),f2dPoint(0,0)),posX1,posY1,posX2,posY2);
		else
			Int2dplc(line2d,f2dCircle(fabs(region.fMinY),f2dPoint(0,0)),posX1,posY1,posX2,posY2);
	}
	if(f3dPoint(posX1,posY1,0)*ray_vec>0)
		intersPt=f3dPoint(posX1,posY1,0);
	else
		intersPt=f3dPoint(posX2,posY2,0);
	return intersPt;
}
GEPOINT CSectionViewDrawing::GetFeatureLineTipPoint(const double* vRaylineVec,bool bAlongVec/*=true*/)
{
	f3dArcLine *pLine;
	CXhSimpleList<f3dArcLine>listEdgeLines;
	for(pLine=GetEdgeLineList()->GetFirst();pLine;pLine=GetEdgeLineList()->GetNext())
		listEdgeLines.AttachObject(*pLine);
	for(IDbEntity* pEntity=EnumFirstDbEntity();pEntity;pEntity=EnumNextDbEntity())
	{
		if(pEntity->GetDbEntType()==IDbEntity::DbLine)			//�߶�
		{
			IDbLine* pLine=(IDbLine*)pEntity;
			f3dArcLine arcline=LineToArc(f3dLine(pLine->StartPosition(),pLine->EndPosition()));
			listEdgeLines.AttachObject(arcline);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbArcline)		//��������
		{
			IDbArcline* pArcLine=(IDbArcline*)pEntity;
			listEdgeLines.AttachObject(pArcLine->GetArcLine());
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbCircle)		//Բ
		{
			IDbCircle* pDbCircle=(IDbCircle*)pEntity;
			f3dArcLine arcline;
			GEPOINT center=pDbCircle->Center();
			GEPOINT startPt(center.x+pDbCircle->Radius(),center.y);
			arcline.CreateMethod1(center,startPt,f3dPoint(0,0,1),Pi+Pi);
			listEdgeLines.AttachObject(arcline);
		}
		else if(pEntity->GetDbEntType()==IDbEntity::DbPolyline)		//����
		{
			IDbPolyline* pPolyLine=(IDbPolyline*)pEntity;
			int nLineNum=pPolyLine->numVerts();
			AcDbPolyline *pAcDbPolyLine=new AcDbPolyline(nLineNum);//����LINE����
			double startWidth=0,endWidth=0;
			GEPOINT xPrevVertice,xVertice;
			for(int i=1;i<nLineNum;i++)
			{
				pPolyLine->GetVertexAt(i-1,xPrevVertice);
				pPolyLine->GetVertexAt(i,xVertice);
				f3dArcLine arcline=LineToArc(f3dLine(xPrevVertice,xVertice));
				listEdgeLines.AttachObject(arcline);
			}
		}
	}
	int retcode;
	double farest=0;
	GEPOINT inters,inters2,farestInters;
	f2dLine rayline(f2dPoint(0,0),f2dPoint(vRaylineVec[0]*10000,vRaylineVec[1]*10000));
	GEPOINT vxRaylineVec(vRaylineVec);
	if(!bAlongVec)
		vxRaylineVec*=-1.0;
	normalize(vxRaylineVec);
	for(pLine=listEdgeLines.EnumObjectFirst();pLine;pLine=listEdgeLines.EnumObjectNext())
	{
		if(pLine->SectorAngle()<EPS)
		{	//ֱ��
			f2dLine edgeline((f3dPoint)pLine->Start(),(f3dPoint)pLine->End());
			GEPOINT edgevec=pLine->End()-pLine->Start();
			normalize(edgevec);
			if(fabs(edgevec*vxRaylineVec)>EPS_COS2)
				continue;	//�ӽ�ƽ�е��߲���⽻��
			if(retcode=Int2dpl(rayline,edgeline,inters.x,inters.y)>0)
			{
				GEPOINT vs(inters.x-edgeline.startPt.x,inters.y-edgeline.startPt.y);
				GEPOINT ve(inters.x-edgeline.endPt.x,inters.y-edgeline.endPt.y);
				if(vs*ve>0)	//���㲻��ֱ�߶���
					continue;
				double dist=vxRaylineVec*inters;
				if(dist>farest)
				{
					farest=dist;
					farestInters=inters;
				}
			}
		}
		else if(pLine->SectorAngle()>Pi+Pi-EPS)
		{	//Բ
			f2dCircle cir(pLine->Radius(),(f3dPoint)pLine->Center());
			if((retcode=Int2dlc(rayline,cir,inters.x,inters.y,inters2.x,inters2.y))>0)
			{
				double dist=vxRaylineVec*inters;
				if(dist>farest)
				{
					farest=dist;
					farestInters=inters;
				}
				if(retcode==2)
				{
					dist=vxRaylineVec*inters2;
					if(dist>farest)
					{
						farest=dist;
						farestInters=inters2;
					}
				}
			}
		}
		else
		{
			f2dArc arc;
			arc.startPt=(f3dPoint)pLine->Start();
			arc.centre=(f3dPoint)pLine->Center();
			arc.angle=pLine->SectorAngle();
			if((retcode=Int2dla(rayline,arc,inters.x,inters.y,inters2.x,inters2.y))>0)
			{
				double dist=vxRaylineVec*inters;
				if(dist>farest)
				{
					farest=dist;
					farestInters=inters;
				}
				if(retcode==2)
				{
					dist=vxRaylineVec*inters2;
					if(dist>farest)
					{
						farest=dist;
						farestInters=inters2;
					}
				}
			}
		}
		//listEdgeLines.AttachObject(*pLine);
	}
	return farestInters;
}
//�ֹܶ���ͼ�У����Ƹֹܻ�׼�ߵ�������
void CSectionViewDrawing::CreateTubeFeatureLine(SCOPE_STRU& region)
{
	if(tube_base_line.m_iDraw==0||tube_base_line.m_xArrowStdVec.IsZero())
		return;
	PEN_STRU penStyle;
	penStyle.width=0;
	penStyle.crColor=0;
	double fArrowH=tube_base_line.m_fArrowH;
	if(tube_base_line.m_iDraw==1)
	{	//�ֶ����Ƽ�ͷģʽ
		GEPOINT arrowOffsetVec;
		arrowOffsetVec=tube_base_line.m_xArrowStdVec;
		rotate_point_around_axis(arrowOffsetVec,Pi/2,f3dPoint(0,0,0),f3dPoint(0,0,1));
		//�����׼��������
		GEPOINT arrowHead,arrowBtm,arrowRoot,orthoLineStart,orthoLineEnd;
		//arrowHead=GetFeatureVecPoint(tube_base_line.m_xArrowStdVec,region);
		//arrowBtm =GetFeatureVecPoint(tube_base_line.m_xArrowStdVec,region,FALSE);
		arrowHead=GetFeatureLineTipPoint(tube_base_line.m_xArrowStdVec,true);
		arrowBtm =GetFeatureLineTipPoint(tube_base_line.m_xArrowStdVec,false);
		arrowHead+=tube_base_line.m_xArrowStdVec*(fArrowH*2);
		arrowBtm-=tube_base_line.m_xArrowStdVec*fArrowH;
		arrowRoot=arrowHead+tube_base_line.m_xArrowStdVec*(-1*fArrowH);
		orthoLineStart=GetFeatureLineTipPoint(arrowOffsetVec,true);
		orthoLineEnd  =GetFeatureLineTipPoint(arrowOffsetVec,false);
		orthoLineStart+=arrowOffsetVec*2;
		orthoLineEnd-=arrowOffsetVec*2;
		//����
		penStyle.style=PS_DASHDOTDOT;
		IDbLine* pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(arrowHead);
		pLine->SetEndPosition(arrowBtm);
		pLine->SetPen(penStyle);
		//����������ֱ��
		pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(orthoLineStart);
		pLine->SetEndPosition(orthoLineEnd);
		pLine->SetPen(penStyle);
		//���ͷ��
		GEPOINT lineStart=arrowHead;
		GEPOINT lineEnd=arrowRoot+arrowOffsetVec*fArrowH*0.3;
		pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(lineStart);
		pLine->SetEndPosition(lineEnd);
		pLine->SetPen(penStyle);
		m_pArrowLine1=pLine;
		//�Ҽ�ͷ��
		lineStart=arrowHead;
		lineEnd=arrowRoot-arrowOffsetVec*fArrowH*0.3;
		pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(lineStart);
		pLine->SetEndPosition(lineEnd);
		pLine->SetPen(penStyle);
		m_pArrowLine2=pLine;
	}
	else //if(tube_base_line.m_iDraw==2)
	{	//CAD��׼��ͷ
		GEPOINT arrowRoot=tube_base_line.m_xArrowHead-tube_base_line.m_xArrowStdVec*fArrowH;
		penStyle.style=PS_SOLID;
		IDbPolyline* pPolyline=(IDbPolyline*)m_xEntManager.AppendDbEntity(IDbEntity::DbPolyline);
		pPolyline->SetPen(penStyle);
		pPolyline->AddVertexAt(0,tube_base_line.m_xArrowBtm,0,0);
		pPolyline->AddVertexAt(1,arrowRoot,fArrowH*0.5,0);
		pPolyline->AddVertexAt(2,tube_base_line.m_xArrowHead);
		m_pArrowPolyLine1=pPolyline;
	}
	if(!tube_base_line.m_xTaCoreVec.IsZero())
	{	//���Ƴ������ĵ�ʾ��ͼ
		f3dPoint xRootHead=GetFeatureLineTipPoint(tube_base_line.m_xTaCoreVec);//GetFeatureVecPoint(tube_base_line.m_xTaCoreVecHead,region);
		double labelDrawLength=ZoomToModelTextSize(sys.fonts.weld.fCutawayViewSymbolTextSize)*2;
		xRootHead+=tube_base_line.m_xTaCoreVec*(fArrowH*2+labelDrawLength);
		f3dPoint xRoot=xRootHead-tube_base_line.m_xTaCoreVec*fArrowH;
		//GEPOINT xRoot=tube_base_line.m_xTaCoreVecHead+tube_base_line.m_xTaCoreVec*(-fArrowH);
		penStyle.style=PS_SOLID;
		IDbPolyline* pPolyline=(IDbPolyline*)m_xEntManager.AppendDbEntity(IDbEntity::DbPolyline);
		pPolyline->SetPen(penStyle);
		pPolyline->AddVertexAt(0,tube_base_line.m_xTaCoreVecBtm,0,0);
		pPolyline->AddVertexAt(1,xRoot,fArrowH*0.5,0);
		pPolyline->AddVertexAt(2,xRootHead);//tube_base_line.m_xTaCoreVecHead);
		m_pArrowPolyLine1=pPolyline;
		IDbText* pText=(IDbText*)m_xEntManager.AppendDbEntity(IDbEntity::DbText);
		pText->SetPen(penStyle);
		pText->SetTextString("������");
		pText->SetHeight(ZoomToModelTextSize(sys.fonts.weld.fCutawayViewSymbolTextSize));
		double rotAngle=Cal2dLineAng(0,0,tube_base_line.m_xTaCoreVecHead.x,tube_base_line.m_xTaCoreVecHead.y);
		if(rotAngle>0.5*Pi&&rotAngle<=1.5*Pi)
		{
			rotAngle-=Pi;
			pText->SetAlignment(IDbText::AlignMiddleRight);
		}
		else
			pText->SetAlignment(IDbText::AlignMiddleLeft);
		pText->SetRotation(rotAngle);
		//��Ҫ��������߶����ͼ�����涯�������2017.11.8
		f3dPoint dimpos=xRootHead+tube_base_line.m_xTaCoreVec*0.5;
		pText->SetPosition(dimpos);
	}
}
//�ֹܶ���ͼ�У����ƽǶȱ�ע��������
void CSectionViewDrawing::CreateDimAngleFeatureLine(SCOPE_STRU& region)
{
	CLDSLineTube* pTube=NULL;
	if(m_pSectionView->IsSlaveView()&&((CSlaveSectionView*)m_pSectionView)->m_hRootRod>0)
		pTube=(CLDSLineTube*)Ta.FromPartHandle(((CSlaveSectionView*)m_pSectionView)->m_hRootRod,CLS_LINETUBE);
	else if(m_pRootPart&&m_pRootPart->GetClassTypeId()==CLS_LINETUBE)
		pTube=(CLDSLineTube*)m_pRootPart;
	if(pTube==NULL)
		return;
	if(pTube->m_cFuncType == CLDSLineTube::FUNC_WIREBUSH || (pTube->pStart == NULL && pTube->pEnd == NULL))
		return;	//�������׹�������
	PEN_STRU penStyle;
	penStyle.width=0;
	penStyle.crColor=0;
	CHashSet<BOOL> hashHasRayVecs;
	ARRAY_LIST<GEPOINT> rayVecs;
	GEPOINT ray_vec,base_vec=tube_base_line.m_xArrowStdVec;
	GEPOINT vStdTubeLenVec=pTube->End()-pTube->Start();
	Standarized(vStdTubeLenVec);
	for(CAngularDimension *pAngular=m_pSectionView->Angulars.GetFirst();pAngular;pAngular=m_pSectionView->Angulars.GetNext())
	{
		SmartPartPtr pStartPart=m_pTower->FromPartHandle(pAngular->m_hStartPart);
		SmartPartPtr pEndPart=m_pTower->FromPartHandle(pAngular->m_hEndPart);
		if(pStartPart.IsHasPtr()&&hashHasRayVecs.GetValue(pAngular->m_hStartPart)==FALSE)
		{
			hashHasRayVecs.SetValue(pAngular->m_hStartPart,TRUE);
			GEPOINT inters,linePick;
			if(pStartPart->GetClassTypeId()==CLS_PLATE&&pStartPart.pPlate->designInfo.iFaceType==6)
			{	//��ע��繹���۵����������滷��λ�Ƕȡ�wjh-2016.11.2
				GEPOINT vVerifyRayVec=pTube->GetPlateVertSpreadVec(pStartPart.pPlate);
				int index=0;
				BYTE byteConstArr[3]={0x01,0x02,0x04};
				PART_DIMnDRAW* pDimDraw=m_pPartGroupDrawing->GetPartDimDraw(m_pSectionView->Id,pStartPart->handle);
				for(int i=0;i<pStartPart.pPlate->face_N;i++)
				{
					if(pDimDraw&&!(pDimDraw->cbDrawPlanes&byteConstArr[i]))
						continue;
					GEPOINT rayVec=vVerifyRayVec;
					GEPOINT vWorkNorm=(i==0)?pStartPart.pPlate->ucs.axis_z:pStartPart.pPlate->GetHuoquFaceNorm(i-1);
					if(fabs(vWorkNorm*vStdTubeLenVec)>EPS2)//�����淨����ֹ����򲻴�ֱ����ע�Ƕ�������
						continue;
					rayVec=vWorkNorm^vStdTubeLenVec;
					if(rayVec*vVerifyRayVec<0)
						rayVec*=-1.0;
					linePick=rayVec*(pTube->GetDiameter()*0.5)+cs.origin;
					Int3dlf(inters,linePick,vStdTubeLenVec,cs.origin,cs.axis_z);
					rayVec=inters-(GEPOINT)cs.origin;
					vector_trans(rayVec,cs,FALSE,TRUE);
					rayVecs.append(rayVec);
					index++;
				}
			}
			else
			{
				if(pStartPart->GetClassTypeId()==CLS_LINESLOT)
					ray_vec=pStartPart.pSlot->WorkPlaneNorm();
				else if(pStartPart->GetClassTypeId()==CLS_PLATE)
					ray_vec=pTube->GetPlateVertSpreadVec(pStartPart.pPlate);
				else	//��������
					ray_vec=pStartPart->ucs.axis_y;
				//ray_vec����ֱ����ǰ��ͼƽ��ͶӰ�������ظֹ��������ͼƽ��ͶӰ������׼ȷ�ĽǸֱ�ע��
				linePick=ray_vec*(pTube->GetDiameter()*0.5)+cs.origin;
				Int3dlf(inters,linePick,vStdTubeLenVec,cs.origin,cs.axis_z);
				ray_vec=inters-(GEPOINT)cs.origin;
				vector_trans(ray_vec,cs,FALSE,TRUE);
				rayVecs.append(ray_vec);
			}
		}
		if(pEndPart.IsHasPtr()&&hashHasRayVecs.GetValue(pAngular->m_hEndPart)==FALSE)
		{
			hashHasRayVecs.SetValue(pAngular->m_hEndPart,TRUE);
			GEPOINT inters,linePick;
			if(pEndPart->GetClassTypeId()==CLS_PLATE&&pEndPart.pPlate->designInfo.iFaceType==6)
			{	//��ע��繹���۵����������滷��λ�Ƕȡ�wjh-2016.11.2
				GEPOINT vVerifyRayVec=pTube->GetPlateVertSpreadVec(pEndPart.pPlate);
				int index=0;
				for(int i=0;i<pEndPart.pPlate->face_N;i++)
				{
					GEPOINT rayVec=vVerifyRayVec;
					GEPOINT vWorkNorm=pEndPart.pPlate->ucs.axis_z;
					if(i>0)
						vWorkNorm=pEndPart.pPlate->GetHuoquFaceNorm(i-1);
					if(fabs(vWorkNorm*vStdTubeLenVec)>EPS2)//�����淨����ֹ����򲻴�ֱ����ע�Ƕ�������
						continue;
					rayVec=vWorkNorm^vStdTubeLenVec;
					if(rayVec*vVerifyRayVec<0)
						rayVec*=-1.0;
					linePick=rayVec*(pTube->GetDiameter()*0.5)+cs.origin;
					Int3dlf(inters,linePick,vStdTubeLenVec,cs.origin,cs.axis_z);
					rayVec=inters-(GEPOINT)cs.origin;
					vector_trans(rayVec,cs,FALSE,TRUE);
					rayVecs.append(rayVec);
					index++;
				}
			}
			else
			{
				if(pEndPart->GetClassTypeId()==CLS_LINESLOT)
					ray_vec=pEndPart.pSlot->WorkPlaneNorm();
				else if(pEndPart->GetClassTypeId()==CLS_PLATE)
					ray_vec=pTube->GetPlateVertSpreadVec(pEndPart.pPlate);
				else	//��������
					ray_vec=pEndPart->ucs.axis_y;
				linePick=ray_vec*(pTube->GetDiameter()*0.5)+cs.origin;
				Int3dlf(inters,linePick,vStdTubeLenVec,cs.origin,cs.axis_z);
				ray_vec=inters-(GEPOINT)cs.origin;
				vector_trans(ray_vec,cs,FALSE,TRUE);
				rayVecs.append(ray_vec);
			}
		}
	}
	for(int i=0;i<rayVecs.GetSize();i++)
	{
		normalize(rayVecs[i]);
		ray_vec=rayVecs[i];
		if(fabs(ray_vec*base_vec)>EPS_COS)	//��ע�������׼���غ�
			continue;
		GEPOINT intersPt=GetFeatureLineTipPoint(ray_vec);//GetFeatureVecPoint(ray_vec,region);
		intersPt+=ray_vec*1;
		penStyle.style=PS_DASHDOTDOT;
		IDbLine* pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(f3dPoint(0,0,0));
		pLine->SetEndPosition(intersPt);
		pLine->SetPen(penStyle);
	}
	if(!tube_base_line.m_xTaCoreVec.IsZero()&&fabs(ray_vec*base_vec)<EPS2)
	{	//
		ray_vec=tube_base_line.m_xTaCoreVec;
		GEPOINT intersPt=GetFeatureLineTipPoint(ray_vec);//GetFeatureVecPoint(ray_vec,region);
		intersPt+=ray_vec*1;
		penStyle.style=PS_DASHDOTDOT;
		IDbLine* pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(f3dPoint(0,0,0));
		pLine->SetEndPosition(intersPt);
		pLine->SetPen(penStyle);
	}
}
bool CSectionViewDrawing::CreateProcessAidedPlate(SCOPE_STRU& region)	//���ɸ�����װ����
{	//����Ӧ�ú���λ��RelayoutDrawing()ĩβ���ã�δ������������⣬����Ҫ�ڸú����ڲ��ֶ������ͼ���� wjh-2017.11.21
	if(m_pSectionView->m_idViewType!=CSectionView::CLS_SLAVE_NODESECT_VIEW)
		return false;
	CSectionView::PART_ITEM* pItem;
	SmartPartPtr pCurrPart;
	CLDSPlate *pSprayPlate1=NULL,*pSprayPlate2=NULL;
	for(pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		if(TagPart(pItem)==NULL)
			pCurrPart=m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
		if(TagPart(pItem)==NULL)
			continue;
		pCurrPart=TagPart(pItem);
		if(pCurrPart->GetClassTypeId()==CLS_PLATE&&pCurrPart.pPlate->designInfo.iFaceType==7)
		{
			if(pSprayPlate1==NULL)
				pSprayPlate1=pCurrPart.pPlate;
			else if(pSprayPlate2==NULL)
			{
				pSprayPlate2=pCurrPart.pPlate;
				break;
			}
		}
		if(pSprayPlate1&&pSprayPlate2)
			break;
	}
	if(pSprayPlate1==NULL||pSprayPlate2==NULL)
		return false;
	CLDSLineTube* pVertTube=(CLDSLineTube*)m_pTower->FromPartHandle(((CSlaveNodeSectionView*)m_pSectionView)->m_hVertRod,CLS_LINETUBE);
	if(pVertTube==NULL)
		return false;
	double radius=pVertTube->size_wide*0.5;
	int i,indexOfUpBendPlane[2]={0},indexOfDownBendPlane[2]={0};	//����ȡֵ0,1,2
	btc::WORKPLANE upperPlanes[2],downPlanes[2];
	GEPOINT upper,down,direct,upperDirect[2],downDirect[2];
	PROFILE_VERTEX* pVertex,*pUpperVertex=NULL,*pDownVertex=NULL;
	for(i=0;i<2;i++)
	{
		upper.Set();
		down.Set();
		pUpperVertex=pDownVertex=NULL;
		CLDSPlate* pSprayPlate=i==0?pSprayPlate1:pSprayPlate2;
		for(pVertex=pSprayPlate->vertex_list.GetFirst();pVertex;pVertex=pSprayPlate->vertex_list.GetNext())
		{
			int iResidePlane=pVertex->vertex.feature<10?pVertex->vertex.feature:pVertex->vertex.feature%10;
			GEPOINT point=pSprayPlate->TransToActual(pVertex->vertex,iResidePlane,CLDSPlate::TO_MCS);
			double dist=DistOf3dPtLine(point,pVertTube->Start(),pVertTube->End());
			if(dist>radius+pVertTube->Thick*2)
				continue;	//ֻ���������ֹܱ�һ�����������
			coord_trans(point,cs,FALSE,TRUE);
			if(pUpperVertex==NULL||point.z>upper.z)
			{
				pUpperVertex=pVertex;
				indexOfUpBendPlane[i]=iResidePlane-1;
				upper=point;
			}
			if(pDownVertex==NULL||point.z<down.z)
			{
				pDownVertex=pVertex;
				indexOfDownBendPlane[i]=iResidePlane-1;
				down=point;
			}
		}
		GEPOINT direct=pVertTube->GetPlateVertSpreadVec(pSprayPlate);
		if(indexOfUpBendPlane[i]>0)
		{
			upperPlanes[i].pick=upper;
			coord_trans(upperPlanes[i].pick,cs,TRUE,TRUE);
			upperPlanes[i].norm=pSprayPlate->GetHuoquFaceNorm(indexOfUpBendPlane[i]-1);
			upperDirect[i]=upperPlanes[i].norm^cs.axis_z;
			if(!direct.IsZero()&&upperDirect[i]*direct<0)
				upperDirect[i]*=-1;
		}
		if(indexOfDownBendPlane[i]>0)
		{
			downPlanes[i].pick=upper;
			coord_trans(downPlanes[i].pick,cs,TRUE,TRUE);
			downPlanes[i].norm=pSprayPlate->GetHuoquFaceNorm(indexOfDownBendPlane[i]-1);
			downDirect[i]=downPlanes[i].norm^cs.axis_z;
			if(!direct.IsZero()&&downDirect[i]*direct<0)
				downDirect[i]*=-1;
		}
	}
	//btc::WORKPLANE upperPlanes[2],downPlanes[2]ֻ���л������ʱ����и�ֵ�������û�н��д�����ֹ�����˴������ж�    wxc-2017.4.6
	if((upperPlanes[0].norm.IsZero()&&upperPlanes[0].pick.IsZero())||(downPlanes[0].norm.IsZero()&&downPlanes[0].pick.IsZero()))
		return false;
	bool reverse12=false;
	if(indexOfUpBendPlane[0]>0&&indexOfUpBendPlane[1]>0)
	{
		double angle1=Cal2dLineAng(0,0,upperDirect[0].x,upperDirect[0].y);
		double angle2=Cal2dLineAng(0,0,upperDirect[1].x,upperDirect[1].y);
		if(angle2<angle1)
			reverse12=true;
		if(fabs(angle2-angle1)>Pi)
			reverse12=!reverse12;
	}
	for(int i=0;i<2;i++)
	{	//�ϲ࿨��
		GEPOINT sectorStart,sectorEnd,sectorMiddleTip;
		f3dPoint inters[2],inters2,center=cs.origin;
		Int3dlf(center,pVertTube->Start(),pVertTube->End()-pVertTube->Start(),cs.origin,cs.axis_z);
		sectorMiddleTip=center;
		coord_trans(center,cs,FALSE,TRUE);

		for(int j=0;j<2;j++)
		{
			if(i==0)
			{
				int hits=Int3dlc(&inters[j],&inters2,upperPlanes[j].pick,upperDirect[j],pVertTube->Start(),pVertTube->End(),radius,radius);
				if(hits==2&&(inters2-inters[j])*upperDirect[j]>0)
					inters[j]=inters2;
			}
			else
			{
				int hits=Int3dlc(&inters[j],&inters2,downPlanes[j].pick,downDirect[j],pVertTube->Start(),pVertTube->End(),radius,radius);
				if(hits==2&&(inters2-inters[j])*downDirect[j]>0)
					inters[j]=inters2;
			}
		}
		GEPOINT extendDirect[2];
		if(i==0)
			memcpy(extendDirect,upperDirect,sizeof(GEPOINT)*2);
		else
			memcpy(extendDirect,downDirect,sizeof(GEPOINT)*2);
		if(reverse12)
		{
			inters2=inters[0];
			inters[0]=inters[1];
			inters[1]=inters2;
			direct=extendDirect[0];
			extendDirect[0]=extendDirect[1];
			extendDirect[1]=direct;
		}
		CXhSimpleList<PROFILE_VERTEX> listVertex;
		pVertex=listVertex.AttachObject();
		pVertex->vertex=inters[0];
		pVertex->type=2;
		pVertex->center=center;
		pVertex->radius=-radius;
		//pVertex->center
		f3dPoint sectorTipStart,sectorTipEnd;
		GEPOINT midDirection=extendDirect[0]+extendDirect[1];
		normalize(midDirection);
		sectorMiddleTip+=midDirection*(radius+50);
		GEPOINT normal=midDirection^extendDirect[0];
		GEPOINT vertDirect=midDirection^normal;
		Int3dll(sectorMiddleTip,sectorMiddleTip+vertDirect,inters[0],inters[0]+extendDirect[0],sectorTipStart);
		Int3dll(sectorMiddleTip,sectorMiddleTip+vertDirect,inters[1],inters[1]+extendDirect[1],sectorTipEnd);
		double tipEdgeLength=DISTANCE(sectorTipStart,sectorTipEnd);//SQRT(edgeLength*edgeLength-(radius+50)*(radius+50))*2;
		pVertex=listVertex.AttachObject();
		pVertex->vertex=inters[1];
		pVertex=listVertex.AttachObject();
		pVertex->vertex=sectorTipEnd;
		pVertex=listVertex.AttachObject();
		pVertex->vertex=sectorTipStart;
		SCOPE_STRU scope;
		for(pVertex=listVertex.EnumObjectFirst();pVertex;pVertex=listVertex.EnumObjectNext())
		{
			coord_trans(pVertex->vertex,cs,FALSE,TRUE);
			pVertex->vertex.z=0;
			pVertex->vertex.x/=m_fUser2ScrDrawScale;
			pVertex->vertex.y/=m_fUser2ScrDrawScale;
			pVertex->radius/=m_fUser2ScrDrawScale;
			scope.VerifyVertex(pVertex->vertex);
		}
		double CLEARANCE=ZoomToModelTextSize(sys.fonts.weld.fDimTextSize*4);	//��̬������нǱ�ע����ķ�Χ��������
		center.x+=region.fMaxX-scope.fMinX+CLEARANCE;
		for(pVertex=listVertex.EnumObjectFirst();pVertex;pVertex=listVertex.EnumObjectNext())
		{
			pVertex->vertex.x+=region.fMaxX-scope.fMinX+CLEARANCE;
			if(pVertex->type==2)
				pVertex->center.x+=region.fMaxX-scope.fMinX+CLEARANCE;
		}
		PEN_STRU pen;
		pen.width=1;
		pen.style=PS_SOLID;
		pen.crColor=RGB(255,255,255);
		IDbLine* pLine;
		PROFILE_VERTEX* pPrevVertex=listVertex.EnumObjectTail();
		for(pVertex=listVertex.EnumObjectFirst();pVertex;pVertex=listVertex.EnumObjectNext())
		{
			region.VerifyVertex(pVertex->vertex);
			if(pPrevVertex->type==0)
			{
				m_xEntManager.AppendLine(f3dLine(pPrevVertex->vertex,pVertex->vertex));
				pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
				pLine->SetStartPosition(pPrevVertex->vertex);
				pLine->SetEndPosition(pVertex->vertex);
				pLine->SetPen(pen);
			}
			else
			{
				f3dArcLine arcline;
				pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,NULL);
				IDbArcline* pArcline=(IDbArcline*)m_xEntManager.AppendDbEntity(IDbEntity::DbArcline);
				pArcline->CreateMethod3(arcline.Start(),arcline.End(),arcline.WorkNorm(),arcline.Radius(),arcline.Center());
				pArcline->SetPen(pen);
				//pArcline->SetLayer(LayerTable::AuxLineLayer.layerName);
				if(arcline.WorkNorm().z>0)
				{
					sectorStart=pPrevVertex->vertex;
					sectorEnd=pVertex->vertex;
				}
				else
				{
					sectorStart=pVertex->vertex;
					sectorEnd=pPrevVertex->vertex;
				}
			}
			pPrevVertex=pVertex;
		}
		vector_trans(midDirection,cs,FALSE,TRUE);
		//�������ν�����
		pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(center);
		pLine->SetEndPosition(sectorStart);
		pLine->SetLayer(LayerTable::AxisLineLayer.layerName);
		pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(center);
		pLine->SetEndPosition(sectorEnd);
		pLine->SetLayer(LayerTable::AxisLineLayer.layerName);
		pLine=(IDbLine*)m_xEntManager.AppendDbEntity(IDbEntity::DbLine);
		pLine->SetStartPosition(center);
		pLine->SetEndPosition(center+midDirection*(1+(radius+50)/m_fUser2ScrDrawScale));
		pLine->SetLayer(LayerTable::AxisLineLayer.layerName);
		//��עԲ�ĵ����ߵĸ߶�
		IDbRotatedDimension* pDimension=(IDbRotatedDimension*)m_xEntManager.AppendDbEntity(IDbEntity::DbRotatedDimension);
		GEPOINT horiVec(-midDirection.y,midDirection.x);
		pDimension->SetLine1Point(center);
		pDimension->SetLine2Point(center-f3dPoint(horiVec)*(tipEdgeLength*0.5/m_fUser2ScrDrawScale)+midDirection*((radius+50)/m_fUser2ScrDrawScale));
		double DIM_EXTENT=ZoomToModelTextSize(1.0);	//Ĭ�ϱ�ע�ߴӶ�����������1.0mm(���ע����߶Ƚ����涯���ţ�wjh-2017.11.21
		double topDimHoriOffset=tipEdgeLength*0.5/m_fUser2ScrDrawScale+DIM_EXTENT;
		pDimension->SetDimLinePoint(center-f3dPoint(horiVec)*topDimHoriOffset-f3dPoint(midDirection)*((radius*0.5+25)/m_fUser2ScrDrawScale));
		pDimension->SetDimText(CXhChar16("%.1f",radius+50));
		double angle=Cal2dLineAng(0,0,midDirection.x,midDirection.y);
		pDimension->SetRotationRadian(angle);
		pDimension->SetLayer(LayerTable::DimSizeLayer.layerName);
		//��ע�����߳��ȵ�����
		IDbText* pDbText=(IDbText*)m_xEntManager.AppendDbEntity(IDbEntity::DbText);
		pDbText->SetHeight(ZoomToModelTextSize(sys.fonts.weld.fDimTextSize));	//���ȳߴ��ע���ָ�
		pDbText->SetAlignment(IDbText::AlignBottomCenter);
		pDbText->SetTextString(CXhChar16("%.1f",tipEdgeLength));
		GEPOINT dimTxtPos(center+midDirection*(1+(radius+50)/m_fUser2ScrDrawScale));
		if(midDirection.y<0)	//ԭ������ͷ���򣬵�ת180�Ⱥ�Ӧ�ٶ���һ���ָ߶��� wjh-2017.11.21
			dimTxtPos+=midDirection*pDbText->GetHeight();
		pDbText->SetPosition(dimTxtPos);
		double rotAngle=Cal2dLineAng(0,0,horiVec.x,horiVec.y);
		if(rotAngle>Pi/2&&rotAngle<3*Pi/2)
			rotAngle-=Pi;
		pDbText->SetRotation(rotAngle);
		pDbText->SetTextStyleId(TextStyleTable::hzfs.textStyleId.asOldId());
		//��ע���μн�
		IDbAngularDimension* pAngularDim=(IDbAngularDimension*)m_xEntManager.AppendDbEntity(IDbEntity::DbAngularDimension);
		pAngularDim->SetLine1Start(center);
		pAngularDim->SetLine1End(sectorStart);
		pAngularDim->SetLine2Start(center);
		pAngularDim->SetLine2End(sectorEnd);
		GEPOINT xArcPoint(center+midDirection*(pDbText->GetHeight()*2+(radius+50)/m_fUser2ScrDrawScale));
		if(midDirection.y<0)	//ԭ������ͷ���򣬵�ת180�Ⱥ�Ӧ�ٶ���һ���ָ߶��� wjh-2017.11.21
			xArcPoint+=midDirection*(pDbText->GetHeight()*1.5);
		pAngularDim->SetArcPoint(xArcPoint);
		pAngularDim->SetTextHeight(ZoomToModelTextSize(sys.fonts.weld.fDimAngleTextSize));	//�Ƕȳߴ��ע�ָ�
		pAngularDim->SetArrowSize(ZoomToModelTextSize(sys.dim_map.fDimArrowSize));
		pAngularDim->SetDimStyleId(DimStyleTable::dimAngleStyle.dimStyleId.asOldId());
		pAngularDim->SetLayer(LayerTable::DimSizeLayer.layerName);
		//��ע˵������
		pDbText=(IDbText*)m_xEntManager.AppendDbEntity(IDbEntity::DbText);
		pDbText->SetPosition(center-f3dPoint(midDirection)*(ZoomToModelTextSize(sys.dim_map.fDimTextSize+3)));
		pDbText->SetHeight(ZoomToModelTextSize(sys.fonts.weld.fOtherTextSize));
		pDbText->SetAlignment(IDbText::AlignBottomCenter);
		if(i==0)
			pDbText->SetTextString("ǰ�࿨��");
		else
			pDbText->SetTextString("��࿨��");
		pDbText->SetTextStyleId(TextStyleTable::hzfs.textStyleId.asOldId());
	}
	return true;
}
//////////////////////////////////////////////////////////////////////
// CSectionSlaveDrawing
//////////////////////////////////////////////////////////////////////
CSectionBriefTubeDrawing::CSectionBriefTubeDrawing(int id/*=0*/,IDrawing* pParentDrawing/*=NULL*/)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionSlaveDrawing::CLS_BRIEFTUBE_VIEW;
}
//������ͼ��ͼ
BOOL CSectionBriefTubeDrawing::CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL)
		return FALSE;
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	m_pPartGroup=pPartGroup;
	UpdateDCS();
	_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	m_xEntManager.InitContext(m_pRootPart,this,STORE_ACAD);
	if(m_pRootPart==NULL || m_pRootPart->GetClassTypeId()!=CLS_LINETUBE)
		return FALSE;
	CLDSLineTube* pRootTube=(CLDSLineTube*)m_pRootPart;
	//������ͼ�е��ӹ�������������
	PARTSET filter;
	m_pSectionView->SmartFilterNeedDrawParts(&Ta,filter);
	for(CLDSPart *pPart=filter.GetFirst();pPart;pPart=filter.GetNext())
		CreatePartSketch(m_pSectionView->SonParts.GetValue(pPart->handle));
	DimAngle();
	DimCheckLine();
	if(m_pRootPart->IsLinePart())
		DimLocatePoint((CLDSLinePart*)m_pRootPart);
	//����ͼ��
	bool bDimFinished=false;
	if(pSectionView->IsHostView())
		bDimFinished=DimSectionViewSymbol2();
	if(!bDimFinished&&pSectionView->m_iViewType==VIEW_MAIN)
		DimSectionViewSymbol();
	if(sys.part_group_map.bModulatePartGroupLen&&m_pPartGroupDrawing)
	{	//�����˼�Ϊ��׼����������ͼ�е�ͼԪ��������
		for(f3dCircle *pCir=GetLsCirList()->GetFirst();pCir;pCir=GetLsCirList()->GetNext())
			pCir->centre.x=m_pPartGroupDrawing->TransCoordX(pCir->centre.x);
		for(f3dArcLine *pArcLine=GetEdgeLineList()->GetFirst();pArcLine;pArcLine=GetEdgeLineList()->GetNext())
		{
			f3dPoint startPt=pArcLine->Start();
			f3dPoint endPt=pArcLine->End();
			f3dPoint center=pArcLine->Center();
			double radius=pArcLine->Radius();
			startPt.x=m_pPartGroupDrawing->TransCoordX(startPt.x);
			endPt.x=m_pPartGroupDrawing->TransCoordX(endPt.x);
			center.x=m_pPartGroupDrawing->TransCoordX(center.x);
			if(fabs(pArcLine->SectorAngle())<EPS)
				pArcLine->CreateMethod2(startPt,endPt,pArcLine->WorkNorm(),0);
			else if(pArcLine->ColumnNorm()==pArcLine->WorkNorm())	//Բ��
				pArcLine->CreateMethod1(center,startPt,pArcLine->WorkNorm(),pArcLine->SectorAngle());
			else 	//��Բ
				pArcLine->CreateEllipse(center,startPt,endPt,pArcLine->ColumnNorm(),pArcLine->WorkNorm(),radius);
		}
		for(CSizeTextDim *pSpaceDim=GetSpaceDimList()->GetFirst();pSpaceDim;pSpaceDim=GetSpaceDimList()->GetNext())
		{
			pSpaceDim->initDimPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->initDimPos.x);
			pSpaceDim->dimPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->dimPos.x);
			pSpaceDim->dimStartPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->dimStartPos.x);
			pSpaceDim->dimEndPos.x=m_pPartGroupDrawing->TransCoordX(pSpaceDim->dimEndPos.x);
		}
		for(CTextOnlyDrawing *pPureTextDim=GetPureTextDimList()->GetFirst();pPureTextDim;pPureTextDim=GetPureTextDimList()->GetNext())
			pPureTextDim->dimPos.x=m_pPartGroupDrawing->TransCoordX(pPureTextDim->dimPos.x);
		for(CSizeAngleDim *pAngularDim=GetAngularDimList()->GetFirst();pAngularDim;pAngularDim=GetAngularDimList()->GetNext())
		{
			pAngularDim->dimPos.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->dimPos.x);
			pAngularDim->dimStartPos.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->dimStartPos.x);
			pAngularDim->dimEndPos.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->dimEndPos.x);
			pAngularDim->vertex.x=m_pPartGroupDrawing->TransCoordX(pAngularDim->vertex.x);
		}
		//��չ���ݵ�
		for(CTextFrame *pDataPoint=GetDataPointDimList()->GetFirst();pDataPoint;pDataPoint=GetDataPointDimList()->GetNext())
		{
			pDataPoint->dimPos.x=m_pPartGroupDrawing->TransCoordX(pDataPoint->dimPos.x);
			pDataPoint->dataPos.x=m_pPartGroupDrawing->TransCoordX(pDataPoint->dataPos.x);
			DATAPOINT_TAG* pTag=m_xDataPointTagHash.GetValue(pDataPoint->dwKeyTag);
			if(!pTag)
				continue;
			for(int i=0;i<pTag->vertexArr.GetSize();i++)
				pTag->vertexArr[i].x=m_pPartGroupDrawing->TransCoordX(pTag->vertexArr[i].x);
		}
	}
	if(m_bFinalDrawing=bFinalDrawing)
		RelayoutDrawing();
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// CSectionSlaveDrawing
//////////////////////////////////////////////////////////////////////
CSectionSlaveDrawing::CSectionSlaveDrawing(int id/*=0*/,IDrawing* pParentDrawing/*=NULL*/)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionSlaveDrawing::CLS_SLAVE_VIEW;
}
UCS_STRU CSectionSlaveDrawing::UpdateDCS()	//��������ͼ���ɸ�����ͼ�Ļ�ͼ����ϵ
{
	cs=m_pSectionView->UpdateUCS(m_pTower,m_pPartGroup);
	if(m_pSectionView->m_iViewType!=VIEW_FL)
	{	//����ԭ������ͼ��Ϣʾ���������Ϣ,�ݽ������⴦��,�Ժ�Ӧȫ��תΪ����ı�׼���� wjh-2014.4.03
		CSlaveSectionView *pSlaveView=(CSlaveSectionView*)m_pSectionView;
		CSectionView *pMainView=m_pPartGroup->SonViews.GetValue(pSlaveView->m_idHostView);
		if(pMainView&&!pSlaveView->direction.IsZero())	//����Ӧ����CSlaveSectionView::m_idHostViewȥ����������ͼ
		{
			UCS_STRU host_cs=pMainView->UpdateUCS(m_pTower,m_pPartGroup);
			GEPOINT axis_y(-pSlaveView->direction.y,pSlaveView->direction.x,pSlaveView->direction.z);	//direction��ʱ��ת90ΪY�᷽��
			GEPOINT axis_z=pSlaveView->direction;
			vector_trans(axis_y,host_cs,TRUE,TRUE);
			vector_trans(axis_z,host_cs,TRUE,TRUE);
			project_vector(axis_y,cs.axis_z);
			if(axis_y.IsZero())	//����ͼ���ӷ������ʱ��Ӧ��ԭ��ͼ��������ϵ����
				logerr.Log("%s�麸��'%s'����ͼ�����ӷ�������",pSlaveView->BelongPartGroup()->sGroupName,pSlaveView->sName);
			else
			{
				if(axis_z*cs.axis_z>0)	//��ͼ�淨�������߷���Ӧ�෴
					cs.axis_z*=-1.0;
				cs.axis_y=axis_y;
				normalize(cs.axis_y);
				cs.axis_x=cs.axis_y^cs.axis_z;
			}
		}
	}
	else //if(m_pSectionView->m_iViewType==VIEW_FL)
	{	//������ͼ������Ӧת��ΪCSlateSectionView wjh-2014.4.1
		BOOL pushed=m_pPartGroup->SonViews.push_stack();
		CSectionView *pMainView=m_pPartGroup->SonViews.GetFirst();
		while(pMainView&&pMainView->m_iViewType!=VIEW_MAIN)
			pMainView=m_pPartGroup->SonViews.GetNext();
		if(pushed)
			m_pPartGroup->SonViews.pop_stack();
		if(pMainView)	//����Ӧ����CSlaveSectionView::m_idHostViewȥ����������ͼ
		{
			UCS_STRU host_cs=pMainView->UpdateUCS(m_pTower,m_pPartGroup);
			GEPOINT axis_y=host_cs.axis_y;	//����ӦΪaxis_y=host_cs.TransVFromCS(CSlaveSectionView::direction��ʱ��ת90��)
			GEPOINT axis_z=host_cs.axis_x;	//����ӦΪaxis_z=host_cs.TransVFromCS(CSlaveSectionView::direction);
			project_vector(axis_y,cs.axis_z);
			if(axis_z*cs.axis_z>0)	//��ͼ�淨�������߷���Ӧ�෴
				cs.axis_z*=-1.0;
			cs.axis_y=axis_y;
			normalize(cs.axis_y);
			cs.axis_x=cs.axis_y^cs.axis_z;
		}
	}
	return cs;
}
//////////////////////////////////////////////////////////////////////
// CSectionFlDrawing
//////////////////////////////////////////////////////////////////////
CSectionFlDrawing::CSectionFlDrawing(int id/*=0*/,IDrawing* pParentDrawing/*=NULL*/)
	: CSectionSlaveDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionViewDrawing::CLS_FL_VIEW;
	m_pDrawTube=NULL;
	m_pDrawFL=NULL;
}
BOOL CSectionFlDrawing::CreateSectionViewDrawing(CSectionView *pSectionView,IModel *pTower,CLDSPartGroup *pPartGroup,bool bFinalDrawing/*=true*/)	
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL||pSectionView->m_iViewType!=VIEW_FL)
		return FALSE;
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	m_pPartGroup=pPartGroup;
	UpdateDCS();	//������ͼ����ϵ��Ʋ������¼�������ϵ wht 12-09-07
	_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	m_xEntManager.InitContext(m_pRootPart,this,STORE_ACAD);
	if(m_pRootPart==NULL)
		return FALSE;
	CSectionView::PART_ITEM *pItem=NULL;
	//������ͨ��ͼ�е��ӹ�������������
	PARTSET filter;
	m_pSectionView->SmartFilterNeedDrawParts(&Ta,filter);
	m_pDrawTube=NULL;
	m_pDrawFL=NULL;
	CSuperSmartPtr<CLDSPart> pPart;
	for(pPart=filter.GetFirst();pPart.IsHasPtr();pPart=filter.GetNext())
	{
		pPart->feature=0;
		if(m_pDrawTube==NULL&&pPart->GetClassTypeId()==CLS_LINETUBE)
			m_pDrawTube=pPart.LineTubePointer();
		else if(m_pDrawFL==NULL&&pPart->GetClassTypeId()==CLS_PARAMPLATE&&pPart.ParamPlatePointer()->IsFL())
			m_pDrawFL=pPart.ParamPlatePointer();
		if(m_pDrawTube&&m_pDrawFL)
			break;
	}
	CLDSGeneralPlate* pBtmPlate=NULL;	//δ�ҵ���������²�����ͨ�װ�
	for(pPart=filter.GetFirst();pPart.IsHasPtr()&&m_pDrawTube&&m_pDrawFL==NULL;pPart=filter.GetNext())
	{
		if(!pPart->IsPlate())
			continue;
		CSuperSmartPtr<CLDSGeneralPlate> pPlate=pPart.GeneralPlatePointer();
		if(pPlate->GetBaseNode()!=m_pDrawTube->pStart&&pPlate->GetBaseNode()!=m_pDrawTube->pEnd)
			continue;
		if(pPlate->GetClassTypeId()==CLS_PLATE)
		{
			if(pPlate.pPlate->m_fInnerRadius<EPS)
				continue;
			pBtmPlate=pPlate;
			break;
		}
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			double B=0;
			if(pPlate.pParamPlate->m_iParamType!=TYPE_CIRRIBPLATE||
				!pPlate.pParamPlate->GetDesignItemValue('B',&B,NULL)||fabs(B-360)>EPS_COS)	//B:��,�������ν�(���β���)
				continue;
			pBtmPlate=pPlate;
			break;
		}
	}
	TAG_INFO_PART_DRAWCMD_TUBE_SECT drawTubeCmdTag;
	TAG_INFO_PART_DRAWCMD_FL_FLAT drawFlagFLCmdTag;
	if(m_pDrawFL && m_pDrawTube)
	{
		if(m_pDrawFL->m_iParamType==TYPE_FLD||m_pDrawFL->m_iParamType==TYPE_FLP)
			drawFlagFLCmdTag.bDrawInnerCircle=false;
		else
		{
			double FL_RI=0;	//������Բ
			m_pDrawFL->GetDesignItemValue('R',&FL_RI);
			double TUBE_RO=m_pDrawTube->GetDiameter()*0.5;
			if(fabs(FL_RI-TUBE_RO)<TransToModelLength(0.5))	//������Բ��ֹ���ڲ���С���ɺϲ�����Ϊһ����Բ��(�Է�����Բ��Ϊ���ƻ�׼)
				drawTubeCmdTag.bDrawExterCircle=false;
		}
	}
	else if(pBtmPlate && m_pDrawTube)
	{
		drawFlagFLCmdTag.bDrawInnerCircle=false;
	}
	long hDatumTube=0,hDatumBtmPlate=0;
	if(m_pDrawTube)
	{
		CPartUprightDrawingCommand* pDrawTubeCmd=(CPartUprightDrawingCommand*)AppendDrawingCommand(CStruDrawingCommand::CLS_PART_UPRIGHT_DRAW_CMD);
		pDrawTubeCmd->m_pPart=m_pDrawTube;
		pDrawTubeCmd->SetTagInfo(&drawTubeCmdTag);
		pDrawTubeCmd->Redraw();
		m_pDrawTube->feature=1;	//���û��Ʊ��
		hDatumTube=m_pDrawTube->handle;
	}
	if(m_pDrawFL)
	{
		CPartGeneralDrawingCommand* pDrawFlCmd=(CPartGeneralDrawingCommand*)AppendDrawingCommand(CStruDrawingCommand::CLS_PART_GENERAL_DRAW_CMD);
		pDrawFlCmd->m_pPart=m_pDrawFL;
		pDrawFlCmd->SetTagInfo(&drawFlagFLCmdTag);
		pDrawFlCmd->Redraw();
		m_pDrawFL->feature=1;	//���û��Ʊ��
		RegisterPartDimDraw(m_pDrawFL,CLS_PARAMPLATE,0);
		if(m_pDrawFL->ucs.axis_z*cs.axis_z<0)
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		hDatumBtmPlate=m_pDrawFL->handle;
	}
	else if(pBtmPlate)
	{
		CPartGeneralDrawingCommand* pDrawFlCmd=(CPartGeneralDrawingCommand*)AppendDrawingCommand(CStruDrawingCommand::CLS_PART_GENERAL_DRAW_CMD);
		pDrawFlCmd->m_pPart=pBtmPlate;
		pDrawFlCmd->SetTagInfo(&drawFlagFLCmdTag);
		pDrawFlCmd->Redraw();
		pBtmPlate->feature=1;	//���û��Ʊ��
		RegisterPartDimDraw(pBtmPlate,pBtmPlate->GetClassTypeId(),0);
		if(pBtmPlate->ucs.axis_z*cs.axis_z<0)
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		else
			GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
		hDatumBtmPlate=pBtmPlate->handle;
	}
	//���ݵװ��뵱ǰ��ͼ����ϵԭ����ȷ�� wjh-2018.3.12
	CSlaveSectionView* pSlaveView=(CSlaveSectionView*)m_pSectionView;
	CLDSGeneralPlate* pDatumPlate=m_pDrawFL?m_pDrawFL:pBtmPlate;
	if(m_pDrawTube)
	{
		xFlSectNormalOffset.vxTubeStdUpperAxisVec=m_pDrawTube->xPosEnd-m_pDrawTube->xPosStart;
		normalize(xFlSectNormalOffset.vxTubeStdUpperAxisVec);
		if(xFlSectNormalOffset.vxTubeStdUpperAxisVec*cs.axis_z<0)
			xFlSectNormalOffset.vxTubeStdUpperAxisVec*=-1;
	}
	if(pDatumPlate!=NULL)
	{
		f3dPoint xOrg=pDatumPlate->ucs.origin+pDatumPlate->ucs.axis_z*pDatumPlate->GetNormOffset();;
		GEPOINT dv=xOrg-cs.origin;
		if(pDatumPlate->ucs.axis_z*cs.axis_z>0)
		{
			xFlSectNormalOffset.siDistToBottom=(short)ftoi(dv*cs.axis_z);
			xFlSectNormalOffset.siDistToUpper=xFlSectNormalOffset.siDistToBottom+(short)pDatumPlate->GetThick();
		}
		else
		{
			xFlSectNormalOffset.siDistToUpper=(short)ftoi(dv*cs.axis_z);
			xFlSectNormalOffset.siDistToBottom=xFlSectNormalOffset.siDistToUpper-(short)pDatumPlate->GetThick();
		}
		xFlSectNormalOffset.siMinFilterZ=xFlSectNormalOffset.siDistToBottom-25;
		xFlSectNormalOffset.siMaxFilterZ=xFlSectNormalOffset.siDistToUpper+25;
	}
	for(pPart=filter.GetFirst();pPart.IsHasPtr();pPart=filter.GetNext())
	{
		if(pPart->feature!=0)
			continue;	//�Ѿ����ƵĹ���
		if(pPart->m_hPartWeldParent!=hDatumTube&&pPart->m_hPartWeldParent!=hDatumBtmPlate)
			continue;	//û��ֱ�Ӻ��ӵ��������׼�ֹ��ϵĹ�������������
		CreatePartSketch(GetSectionView()->SonParts.GetValue(pPart->handle));
	}

	DimAngle();
	int nBolt=0;
	if(m_pDrawFL)
		nBolt=m_pDrawFL->GetLsCount();
	else if(pBtmPlate)
		nBolt=pBtmPlate->GetLsCount();
	if(nBolt>0)
	{	//�Ƕȱ�ע�й��˱�ע�Ƕ�==�߰�ı�׼�Ƕ�
		double fRibAngle=360/nBolt,fDimAngle=0;
		for(CSizeAngleDim *pAngularDim=angularDimList.GetFirst();pAngularDim;pAngularDim=angularDimList.GetNext())
		{
			fDimAngle=pAngularDim->GetDimAngle()*DEGTORAD_COEF;
			if(fabs(fDimAngle-fRibAngle)<0.5)
				angularDimList.DeleteCursor();
		}
	}
	angularDimList.Clean();
	if(m_bFinalDrawing=bFinalDrawing)
	{
		RelayoutDrawing();
		//ResetPosition();	//����������ԭ��Ϊ���Ͻǣ�������ͼ����ק������
	}
	return TRUE;
}
void CSectionFlDrawing::FinalDrawing()
{
	if(!m_bFinalDrawing)
	{
		RelayoutDrawing();
		//ResetPosition();
		m_bFinalDrawing=true;
	}
}
//////////////////////////////////////////////////////////////////////
//CSectionJointDrawing
///////////////////////////////////////////////////////////////////////
CSectionJointDrawing::CSectionJointDrawing(int id/* =0 */,IDrawing* pParentDrawing/* =NULL */)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionViewDrawing::CLS_JOINT_VIEW;
	m_pRootTube=m_pJointTube=NULL;
}
BOOL CSectionJointDrawing::CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL)
		return FALSE;
	//1����ʼ��������Ϣ
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	m_pPartGroup=pPartGroup;
	UpdateDCS();
	_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	m_xEntManager.InitContext(m_pRootPart,this,STORE_ACAD);
	if(m_pRootPart==NULL)
		return FALSE;
	CLDSLinePart* pRootRod =m_pRootTube =(CLDSLineTube*)m_pTower->FromRodHandle(((CJointTubesView*)m_pSectionView)->m_hRootTube);
	CLDSLinePart* pJointRod=m_pJointTube=(CLDSLineTube*)m_pTower->FromRodHandle(((CJointTubesView*)m_pSectionView)->m_hJointTube);
	m_vAngularJointPlaneNorm=cs.axis_z;
	if(pRootRod&&pJointRod)
	{
		GEPOINT lenRootVec =pRootRod->End() - pRootRod->Start();
		if(pRootRod->pEnd==pJointRod->pStart||pRootRod->pEnd==pJointRod->pEnd)
			lenRootVec*=-1;	//�ն˶Խ�
		GEPOINT lenJointVec=pJointRod->End()-pJointRod->Start();
		if(pJointRod->pEnd==pRootRod->pStart||pJointRod->pEnd==pRootRod->pEnd)
			lenJointVec*=-1;	//�ն˶Խ�
		normalize(lenRootVec);
		normalize(lenJointVec);
		double jointRadAngle=ACOS(-(lenRootVec*lenJointVec));
		GEPOINT lenPrjRootVec=lenRootVec;
		GEPOINT lenPrjJointVec=lenJointVec;
		vector_trans(lenPrjRootVec,cs,FALSE,TRUE);
		vector_trans(lenPrjJointVec,cs,FALSE,TRUE);
		lenPrjRootVec.z=lenPrjJointVec.z=0;
		normalize(lenPrjRootVec);
		normalize(lenPrjJointVec);
		double jointPrjRadAngle=ACOS(-(lenPrjRootVec*lenPrjJointVec));
		if(jointPrjRadAngle<=0.6*jointRadAngle)
			((CJointTubesView*)m_pSectionView)->m_ciJointsViewType=1;	//ͶӰ��Խӽ�С��ʵ�ʶԽӽ�һ�룬��ƽ��ͼֱ�ߴ���
		else if(fabs(jointPrjRadAngle-jointRadAngle)<0.000001)
			((CJointTubesView*)m_pSectionView)->m_ciJointsViewType=0;	//չʾ�ԽӸֹܼнǵĸ���ͼ
		else
			((CJointTubesView*)m_pSectionView)->m_ciJointsViewType=2;	//�������ĶԽ���������ͼ
		double justify=0;
		m_vAngularJointPlaneNorm=lenRootVec^lenJointVec;
		normalize(m_vAngularJointPlaneNorm);
	}
	//2��������Ҫ���ƵĹ�������
	PARTSET filter;
	m_pSectionView->SmartFilterNeedDrawParts(&Ta,filter);
	//2����ʼ���Խ����ͼԪ�������б������������������ߴ��ע���Ƕȱ�ע��
	//2.1 ���ɹ���������������
	listCmds.Empty();
	for(CLDSPart *pPart=filter.GetFirst();pPart;pPart=filter.GetNext())
		CreatePartSketch(m_pSectionView->SonParts.GetValue(pPart->handle));
	//2.2 ���ɳߴ��ע����
	CJointTubeDimDrawingCommand* pDimCmd=NULL;
	pDimCmd=(CJointTubeDimDrawingCommand*)AppendDrawingCommand(CStruDrawingCommand::CLS_JOINT_TUBE_DIM_CMD);
	pDimCmd->m_pPart=m_pRootPart;
	for(IDrawingCommand* pCmd=EnumFirstDrawingCommand();pCmd;pCmd=EnumNextDrawingCommand())
		pCmd->Redraw();
	//3������ͼ���в���
	DimCheckLine();
	if(m_pRootPart->IsLinePart())
		DimLocatePoint((CLDSLinePart*)m_pRootPart);
	DimSectionViewSymbol2();
	if(m_bFinalDrawing=bFinalDrawing)
	{
		RelayoutDrawing();
		//ResetPosition();
	}
	return TRUE;
}
void CSectionJointDrawing::FinalDrawing()
{
	if(!m_bFinalDrawing)
	{
		RelayoutDrawing();
		//ResetPosition();
		m_bFinalDrawing=true;
	}
}
//////////////////////////////////////////////////////////////////////////
// CSectionStdXPlatesDrawing
CSectionStdXPlatesDrawing::CSectionStdXPlatesDrawing(int id/* =0 */,IDrawing* pParentDrawing/* =NULL */)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionViewDrawing::CLS_STDX_PLATES_VIEW;
	m_cWeldLinePos=0;
}

void CSectionStdXPlatesDrawing::DimPlateLsSpace(CDrawCoordTrans &dcs,CLDSGeneralPlate *pPlate,
												ATOM_LIST<CSizeTextDim> &spaceDimList)
{
	CDimPlankLs dimLs;
	if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		dimLs.Create((CLDSParamPlate*)pPlate,TRUE);
	else 
		dimLs.Create((CLDSPlate*)pPlate,TRUE);
	CSizeTextDim space_dim;
	ALIGNDIM_STRU align_dim;
	for(int i=0;i<dimLs.GetAlignDimCount();i++)
	{
		dimLs.GetAlignDimAt(i,align_dim);
		//
		space_dim.dimStartPos=align_dim.startPos;
		space_dim.dimEndPos=align_dim.endPos;
		coord_trans(space_dim.dimStartPos,pPlate->ucs,TRUE);
		coord_trans(space_dim.dimEndPos,pPlate->ucs,TRUE);
		space_dim.dimStartPos=dcs.TransToDraw(space_dim.dimStartPos);
		space_dim.dimEndPos=dcs.TransToDraw(space_dim.dimEndPos);
		space_dim.dimStartPos.z=space_dim.dimEndPos.z=0;
		space_dim.dist=align_dim.dist;
		space_dim.bAllowRelayout=FALSE;
		space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
		space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
		space_dim.iOrder = 0;	//װ�����߳ߴ�
		space_dim.nHits=align_dim.nLsSpaceNum;
		spaceDimList.append(space_dim);
	}
}
void CSectionStdXPlatesDrawing::DimViewHorizontalLsSpace(CDrawCoordTrans &dcs,BOOL bFrontView,const double* pxRayOrg)
{	//1.ͳ����˨λ�ò�X��������
	f3dPoint plate_pos(pxRayOrg);
	CLDSPlate* platePtrArr[2]={NULL,NULL};
	double fPlateHalfThick=m_xParam.m_pBasePlate->Thick*0.5;
	if (bFrontView)
	{
		platePtrArr[0]=m_xParam.m_pBasePlate;
		fPlateHalfThick=m_xParam.m_pFrontVertPlate->Thick*0.5;
	}
	else
	{
		platePtrArr[0]=m_xParam.m_pFrontVertPlate;
		platePtrArr[1]=m_xParam.m_pBackVertPlate;
	}
	double _arrXOfPoints[16],_arrXOfPoints2[16];
	ARRAY_LIST<double> xarrKeyPoints(_arrXOfPoints,16,2),xarrTopKeyPoints(_arrXOfPoints2,16,2);
	xarrKeyPoints.append(plate_pos.x-fPlateHalfThick);
	xarrKeyPoints.append(plate_pos.x+fPlateHalfThick);
	xarrTopKeyPoints.append(plate_pos.x-fPlateHalfThick);
	xarrTopKeyPoints.append(plate_pos.x+fPlateHalfThick);
	for(int i=0;i<2;i++)
	{
		if(platePtrArr[i]==NULL)
			continue;
		for(CLsRef *pLsRef=platePtrArr[i]->GetFirstLsRef();pLsRef;pLsRef=platePtrArr[i]->GetNextLsRef())
		{
			int j=0;
			for(j=0;j<4;j++)
			{
				if(m_xParam.m_arrAnglePtr[j]==NULL)
					continue;
				if(m_xParam.m_arrAnglePtr[j]->FindLsByHandle((*pLsRef)->handle))
					break;
			}
			if(j>=4)	//��˨���ڽǸ���
				continue;
			f3dPoint ls_pos=(*pLsRef)->ucs.origin;
			ls_pos=dcs.TransToDraw(ls_pos);
			for(j=0;j<xarrKeyPoints.Count;j++)
			{
				if(fabs(ls_pos.x-xarrKeyPoints[j])<2)
					break;
				if(ls_pos.x<xarrKeyPoints[j])
				{
					xarrKeyPoints.insert(ls_pos.x,j);
					break;
				}
			}
			if(j==xarrKeyPoints.Count)
				xarrKeyPoints.append(ls_pos.x);
		}
	}
	//2.��ע��˨���
	f2dRect rect=GetDrawRegion();
	double fOffsetDist=TransToModelLength(sys.fonts.weld.fDimTextSize);//CSizeTextDim::dimStyle.textHeight);
	for(int i=1;i<xarrKeyPoints.Count;i++)
	{
		double dfPrevCoordX=xarrKeyPoints[i-1];
		double dfCurrCoordX=xarrKeyPoints[i];
		f3dPoint dimStart(dfPrevCoordX,rect.bottomRight.y-10,0),dimEnd(dfCurrCoordX,dimStart.y,0);
		f3dPoint dimPos(0.5*(dfPrevCoordX+dfCurrCoordX),dimStart.y-fOffsetDist,0);
		double fDist=dfCurrCoordX-dfPrevCoordX;
		CSizeTextDim* pSpaceDim = NULL;
		if( fabs(dfPrevCoordX-(plate_pos.x-fPlateHalfThick))<EPS&&
			fabs(dfCurrCoordX-(plate_pos.x+fPlateHalfThick))<EPS)
			pSpaceDim = AppendSizeTextDim(dimStart,dimEnd,dimPos,fDist,0,FALSE,TRUE);
		else 
			pSpaceDim = AppendSizeTextDim(dimStart,dimEnd,dimPos,fDist,0);
		if (pSpaceDim)//�����ע���ڳߴ����²�
			pSpaceDim->iTextDimSylte = 1;
	}
}

void CSectionStdXPlatesDrawing::DimViewVerticalLsSpace(CDrawCoordTrans &dcs,BOOL bFrontView,const double* pxRayOrg)
{	//1.ͳ����˨λ�ò�X��������
	GEPOINT xRayOrg(pxRayOrg);
	CLDSPlate* xarrTiledPlates[2]={ NULL,NULL };	//��ǰ��ͼ�е�ƽ�̸ְ壨����Ҫ�沯��˨�ĸְ壩
	double fPlateHalfThick=m_xParam.m_pBasePlate->Thick*0.5;
	if (bFrontView)
	{	//����ͼ������ƽ�̵���ͼ
		xarrTiledPlates[0]=m_xParam.m_pBasePlate;
		fPlateHalfThick=m_xParam.m_pFrontVertPlate->Thick*0.5;
	}
	else
	{	//����ͼ���ضϰ�ƽ�̵���ͼ
		xarrTiledPlates[0]=m_xParam.m_pFrontVertPlate;
		xarrTiledPlates[1]=m_xParam.m_pBackVertPlate;
	}
	GEPOINT2D _arrYOfPoints[16],_arrYOfPoints2[16];
	ARRAY_LIST<GEPOINT2D> xarrLeftKeyPoints(_arrYOfPoints,16,2),xarrRightKeyPoints(_arrYOfPoints2,16,2);
	xarrLeftKeyPoints.append(GEPOINT2D(0,xRayOrg.y));
	xarrRightKeyPoints.append(GEPOINT2D(0,xRayOrg.y));
	int i,j;
	for (i=0;i<2;i++)
	{
		if (xarrTiledPlates[i]==NULL)
			continue;
		for (CLsRef *pLsRef=xarrTiledPlates[i]->GetFirstLsRef();pLsRef;pLsRef=xarrTiledPlates[i]->GetNextLsRef())
		{
			f3dPoint ls_pos=(*pLsRef)->ucs.origin;
			ls_pos=dcs.TransToDraw(ls_pos);
			for (j=0;j<xarrLeftKeyPoints.Count&&ls_pos.x<0;j++)
			{
				if (fabs(ls_pos.y-xarrLeftKeyPoints[j].y)<20)
				{
					if (ls_pos.x<xarrLeftKeyPoints[j].x)
						xarrLeftKeyPoints[j]=GEPOINT2D(ls_pos.x,ls_pos.y);	//Yֵ�ӽ�������ȡ�����˨��
					break;
				}
				if (ls_pos.y<xarrLeftKeyPoints[j].y)
				{	//��С�����������ϣ�
					xarrLeftKeyPoints.insert(GEPOINT2D(ls_pos.x,ls_pos.y),j);//dimPosList.GetCurrentIndex());
					break;
				}
			}
			if (ls_pos.x<0&&j==xarrLeftKeyPoints.Count)
				xarrLeftKeyPoints.append(GEPOINT2D(ls_pos.x,ls_pos.y));
			for (j=0;j<xarrRightKeyPoints.Count&&ls_pos.x>0;j++)
			{
				if (fabs(ls_pos.y-xarrRightKeyPoints[j].y)<20)
				{
					if (ls_pos.x>xarrRightKeyPoints[j].x)
						xarrRightKeyPoints[j]=GEPOINT2D(ls_pos.x,ls_pos.y);	//Yֵ�ӽ�������ȡ�����˨��
					break;
				}
				if (ls_pos.y<xarrRightKeyPoints[j].y)
				{	//��С�����������ϣ�
					xarrRightKeyPoints.insert(GEPOINT2D(ls_pos.x,ls_pos.y),j);//dimPosList.GetCurrentIndex());
					break;
				}
			}
			if (ls_pos.x>0&&j==xarrRightKeyPoints.Count)
				xarrRightKeyPoints.append(GEPOINT2D(ls_pos.x,ls_pos.y));
		}
	}
	//2.��ע���������˨���
	f2dRect rect=GetDrawRegion();
	double fOffsetDist=TransToModelLength(sys.fonts.weld.fDimTextSize);//CSizeTextDim::dimStyle.textHeight);
	for (int i=1;i<xarrLeftKeyPoints.Count;i++)
	{
		double dfPrevCoordY=xarrLeftKeyPoints[i-1].y;
		double dfCurrCoordY=xarrLeftKeyPoints[i].y;
		GEPOINT dimStart(rect.topLeft.x-10,dfPrevCoordY),dimEnd(dimStart.x,dfCurrCoordY);
		GEPOINT dimPos(dimStart.x-fOffsetDist,0.5*(dfPrevCoordY+dfCurrCoordY),0);
		double fDist=dfCurrCoordY-dfPrevCoordY;
		CSizeTextDim* pSpaceDim = AppendSizeTextDim(dimStart,dimEnd,dimPos,fDist,Pi/2);
		//if (pSpaceDim)//��������ע���ڳߴ������ϣ���
		//	pSpaceDim->iTextDimSylte = 1;
	}
	for (int i=1;i<xarrRightKeyPoints.Count;i++)
	{
		double dfPrevCoordY=xarrRightKeyPoints[i-1].y;
		double dfCurrCoordY=xarrRightKeyPoints[i].y;
		GEPOINT dimStart(rect.bottomRight.x+10,dfPrevCoordY),dimEnd(dimStart.x,dfCurrCoordY);
		GEPOINT dimPos(dimStart.x+fOffsetDist,0.5*(dfPrevCoordY+dfCurrCoordY),0);
		double fDist=dfCurrCoordY-dfPrevCoordY;
		CSizeTextDim* pSpaceDim = AppendSizeTextDim(dimStart,dimEnd,dimPos,fDist,Pi/2);
		if (pSpaceDim !=NULL)
			pSpaceDim->iTextDimSylte = 1;//�Ҳ������ע���ڳߴ����ң��£���
	}
}
//��ȡʮ�ֺ��Ӱ崦���������ĽǺ����¿����͡�wjh-2015.4.24
BYTE CSectionStdXPlatesDrawing::InitWeldLinePos(CLDSGeneralPlate* platePtrArr[3],CLDSLineAngle* arrAnglePtr[4],
												BYTE *pcKmAngle1Pos,BYTE *pcKmAngle2Pos)
{
	BYTEFLAG cWeldLinePos=0;
	f3dPoint geom_center;
	CLDSGeneralPlate *pBasePlate=platePtrArr[0],*pFrontVertPlate=platePtrArr[1],*pBackVertPlate=platePtrArr[2];
	if(pBackVertPlate==NULL||pFrontVertPlate==NULL||pBackVertPlate==NULL||arrAnglePtr[0]==NULL)
		return cWeldLinePos;
	for(PROFILE_VERTEX *pVertex=pFrontVertPlate->vertex_list.GetFirst();pVertex;pVertex=pFrontVertPlate->vertex_list.GetNext())
		geom_center+=pVertex->vertex;
	geom_center/=pFrontVertPlate->vertex_list.GetNodeNum();
	coord_trans(geom_center,pFrontVertPlate->ucs,TRUE);
	f3dPoint front_vec=geom_center-pBasePlate->ucs.origin;
	if(front_vec*pBasePlate->ucs.axis_z>0)
		front_vec= pBasePlate->ucs.axis_z;
	else 
		front_vec=-pBasePlate->ucs.axis_z;
	//Ĭ�ϴ����������죬�����Ƿ���ڶ�Ӧ�ĽǸ��Ƴ���Ӧ�ĺ���
	cWeldLinePos=WELDLINE_FRONT_ZZ|WELDLINE_FRONT_ZF|WELDLINE_BACK_ZZ|WELDLINE_BACK_ZF;
	BYTE kmAnglePos[2]={0};
	for (int i=0;i<2;i++)
	{
		if(arrAnglePtr[i]==0)
			continue;
		double ddx=arrAnglePtr[i]->get_norm_x_wing()*pBasePlate->ucs.axis_z;
		double ddy=arrAnglePtr[i]->get_norm_y_wing()*pBasePlate->ucs.axis_z;
		f3dPoint cur_wing_norm=arrAnglePtr[i]->get_norm_x_wing();
		f3dPoint other_wing_norm=arrAnglePtr[i]->get_norm_y_wing();
		if(fabs(ddy)>fabs(ddx))
		{
			cur_wing_norm=arrAnglePtr[i]->get_norm_y_wing();
			other_wing_norm=arrAnglePtr[i]->get_norm_x_wing();
		}
		if(cur_wing_norm*front_vec<0)
		{	//ǰ����������Ǹ�
			if(other_wing_norm*pFrontVertPlate->ucs.axis_z>0)
			{
				cWeldLinePos.RemoveBitState(WELDLINE_FRONT_ZF);
				//cWeldLinePos&=~WELDLINE_FRONT_ZF;	//cWeldLinePos|=WELDLINE_FRONT_ZZ;
				kmAnglePos[i]=WELDLINE_FRONT_ZF;
			}
			else
			{
				cWeldLinePos.RemoveBitState(WELDLINE_FRONT_ZZ);
				//cWeldLinePos&=~WELDLINE_FRONT_ZZ;	//cWeldLinePos|=WELDLINE_FRONT_ZF;
				kmAnglePos[i]=WELDLINE_FRONT_ZZ;
			}
		}
		else 
		{	//�����������Ǹ�
			if(other_wing_norm*pBackVertPlate->ucs.axis_z>0)
			{
				cWeldLinePos.RemoveBitState(WELDLINE_BACK_ZF);
				//cWeldLinePos&=~WELDLINE_BACK_ZF;	//cWeldLinePos|=WELDLINE_BACK_ZZ;
				kmAnglePos[i]=WELDLINE_BACK_ZF;
			}
			else
			{
				cWeldLinePos.RemoveBitState(WELDLINE_BACK_ZZ);
				//cWeldLinePos&=~WELDLINE_BACK_ZZ;	//cWeldLinePos|=WELDLINE_BACK_ZF;
				kmAnglePos[i]=WELDLINE_BACK_ZZ;
			}
		}
	}
	if(pcKmAngle1Pos)
		*pcKmAngle1Pos=kmAnglePos[0];
	if(pcKmAngle1Pos)
		*pcKmAngle2Pos=kmAnglePos[1];
	if(arrAnglePtr[0]&&arrAnglePtr[1]&&arrAnglePtr[2]&&arrAnglePtr[3])
	{	//�����ĸ��Ǹ�ʱ����Ҫ��ע������
		cWeldLinePos=0;
		return cWeldLinePos;
	}
	return cWeldLinePos;
}
void CSectionStdXPlatesDrawing::DrawSectionViewSymbol(CDrawCoordTrans &dcs)
{
	CStdXPlatesView *pStdXPlatesView=(CStdXPlatesView*)m_pSectionView;
	if(pStdXPlatesView==NULL||pStdXPlatesView->m_cSubType!='F')
		return;
	f2dRect rect=GetDrawRegion();
	//��ǰ��ͼ�л��Ʋ���ͼ������ͼ��
	double symbol_dist=rect.Height()*0.3;
	f3dPoint dim_vec,offset_vec,dim_pos;
	dim_vec.Set(1,0,0);
	dim_pos.Set(rect.topLeft.x,rect.topLeft.y-rect.Height()*0.5);
	dim_pos-=dim_vec*50;
	offset_vec=dim_vec;
	RotateVectorAroundVector(offset_vec,0.5*Pi,f3dPoint(0,0,1));
	double offset_angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
	for(int i=0;i<2;i++)
	{
		CTextOnlyDrawing *pSectionViewDim=pureTextDimList.append();
		pSectionViewDim->iOrder=3;	///����ͼ��
		pSectionViewDim->bNeedConvertDistToText=false;
		pSectionViewDim->dimText.Copy("A"); 
		pSectionViewDim->angle=offset_angle;
		pSectionViewDim->textHeight=1.5*sys.dim_map.fDimTextSize;
		if(i==0)
		{
			pSectionViewDim->bInXWing=FALSE; //�ϲ�
			pSectionViewDim->dimPos=dim_pos+offset_vec*symbol_dist;
		}
		else
		{
			pSectionViewDim->bInXWing=TRUE; //�²�
			pSectionViewDim->dimPos=dim_pos-(offset_vec*symbol_dist);
		}
	}
	//��ǰ��ͼ�л��Ƹ���ͼ������ͼ��
	symbol_dist=rect.Width()*0.3;
	dim_vec.Set(0,-1,0);
	dim_pos.Set(rect.topLeft.x+rect.Width()*0.5,rect.topLeft.y);
	dim_pos-=dim_vec*50;
	offset_vec=dim_vec;
	RotateVectorAroundVector(offset_vec,0.5*Pi,f3dPoint(0,0,1));
	offset_angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
	for(int i=0;i<2;i++)
	{
		CTextOnlyDrawing *pSectionViewDim=pureTextDimList.append();
		pSectionViewDim->iOrder=3;	///����ͼ��
		pSectionViewDim->bNeedConvertDistToText=false;
		pSectionViewDim->dimText.Copy("B"); 
		pSectionViewDim->angle=offset_angle;
		pSectionViewDim->textHeight=1.5*sys.dim_map.fDimTextSize;
		if(i==0)
		{
			pSectionViewDim->bInXWing=FALSE; //�ϲ�
			pSectionViewDim->dimPos=dim_pos+offset_vec*symbol_dist;
		}
		else
		{
			pSectionViewDim->bInXWing=TRUE; //�²�
			pSectionViewDim->dimPos=dim_pos-(offset_vec*symbol_dist);
		}
	}
}
BOOL CSectionStdXPlatesDrawing::CreateFrontViewDrawing()
{
	CDrawCoordTrans dcs(cs,1.0);	//չ����ʽ������ת�������ݲ����ǻ��Ʊ������ڲ���ʱ����
	CStdXPlatesView *pStdXPlatesView=(CStdXPlatesView*)m_pSectionView;
	CLDSPlate *pBasePlate=m_xParam.m_pBasePlate;
	CLDSPlate *pFrontVertPlate=m_xParam.m_pFrontVertPlate;
	//0. �����׼����ǰ�������׼����
	f3dPoint inters_pick,inters_vec;
	f3dPoint base_middle_org=pBasePlate->ucs.origin+pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick()*0.5);
	f3dPoint front_middle_org=pFrontVertPlate->ucs.origin+pFrontVertPlate->ucs.axis_z*(pFrontVertPlate->GetNormOffset()+pFrontVertPlate->GetThick()*0.5);
	Int3d2f(&inters_pick,&inters_vec,base_middle_org,pBasePlate->ucs.axis_z,front_middle_org,pFrontVertPlate->ucs.axis_z);
	GECS tmpcs=cs;
	GEPOINT xTmpInters=tmpcs.TransPToCS(inters_pick);
	cs.origin+=cs.axis_x*xTmpInters.x+cs.axis_y*xTmpInters.y;	//��ʮ�ֲ���м佻�߻���Ϊ����ԭ�㣨���ھ��У� wjh-2019.5.12
	dcs.SetCS(cs);
	//1   ���ƹ���������������庸���ʶ
	//1.1 ���ƻ���
	if(m_xParam.m_pBasePlate&&m_xParam.m_pBasePlate->GetClassTypeId()==CLS_PLATE)
		CreatePlateDrawing(m_xParam.m_pBasePlate,dcs,TRUE,FALSE,1);
	//1.2 ��������
	//1.2.1 ����������������󽻻�ȡ���������彻��
	coord_trans(inters_pick,pBasePlate->ucs,FALSE);
	vector_trans(inters_vec,pBasePlate->ucs,FALSE);
	inters_pick.z=inters_vec.z=0;
	f3dPoint norm_vec(-inters_vec.y,inters_vec.x,0);
	//��֤norm_vec�����巨�߷���ͬ�򣬻��ƺ����ʶʱ��Ҫ����������λ��
	f3dPoint verify_vec=pFrontVertPlate->ucs.axis_z;
	vector_trans(verify_vec,pBasePlate->ucs,FALSE);
	if(norm_vec*verify_vec<0)
		norm_vec*=-1;
	f3dPoint f_start,f_end,f_inters;
	int inters_count[2]={0};
	f3dLine init_inters_line[2],inters_line[2];	//0.ǰ������Z�᷽���� 1.ǰ������Z�Ḻ������
	const int PLATE_WIDTH_MAX=100000;
	init_inters_line[0].startPt=inters_pick+norm_vec*pFrontVertPlate->GetThick()*0.5+inters_vec*PLATE_WIDTH_MAX;
	init_inters_line[0].endPt=inters_pick+norm_vec*pFrontVertPlate->GetThick()*0.5-inters_vec*PLATE_WIDTH_MAX;
	init_inters_line[1].startPt=inters_pick-norm_vec*pFrontVertPlate->GetThick()*0.5+inters_vec*PLATE_WIDTH_MAX;
	init_inters_line[1].endPt=inters_pick-norm_vec*pFrontVertPlate->GetThick()*0.5-inters_vec*PLATE_WIDTH_MAX;
	int n=m_xParam.m_pBasePlate->vertex_list.GetNodeNum();
	for(int i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=m_xParam.m_pBasePlate->vertex_list.GetByIndex(i);
		PROFILE_VERTEX *pEndVertex=m_xParam.m_pBasePlate->vertex_list.GetByIndex((i+1)%n);
		f_start = m_xParam.m_pBasePlate->GetRealVertex(pStartVertex->vertex);
		f_end   = m_xParam.m_pBasePlate->GetRealVertex(pEndVertex->vertex);
		for(int j=0;j<2;j++)
		{
			if(inters_count[j]==2)
				continue;
			if(Int3dll(f_start,f_end,init_inters_line[j].startPt,init_inters_line[j].endPt,f_inters)>0)
			{
				if(inters_count[j]==0)
					inters_line[j].startPt=f_inters;
				else if(inters_count[j]==1)
					inters_line[j].endPt=f_inters;
				inters_count[j]++;
			}
		}
		if(inters_count[0]==2&&inters_count[1]==2)
			break;
	}
	if(inters_count[0]!=2||inters_count[1]!=2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("fail to calculate intersection line of standard cross board through plate and front side plate!");
#else
		logerr.Log("��׼ʮ�ֲ���ͨ����ǰ�����彻�߼���ʧ�ܣ�");
#endif
		return FALSE;
	}
	//1.2.2 �������������߲����ƺ����ʶ
	verify_vec=pFrontVertPlate->ucs.axis_z;
	vector_trans(verify_vec,cs,FALSE,TRUE);
	verify_vec.Set(verify_vec.y,-verify_vec.x,0);	//˳ʱ����ת90��
	for(int i=0;i<2;i++)
	{
		coord_trans(inters_line[i].startPt,pBasePlate->ucs,TRUE);
		coord_trans(inters_line[i].endPt,pBasePlate->ucs,TRUE);
		inters_line[i].startPt=dcs.TransToDraw(inters_line[i].startPt);
		inters_line[i].endPt=dcs.TransToDraw(inters_line[i].endPt);
		inters_line[i].pen.style=PS_SOLID;
		inters_line[i].ID=0;
		edgeList.append(LineToArc(inters_line[i]));
		f3dPoint line_vec=inters_line[i].endPt-inters_line[i].startPt;
		normalize(line_vec);
		if(i==0&&(m_cWeldLinePos&WELDLINE_FRONT_ZZ))
		{
			if(line_vec*verify_vec>0)
				DrawWeldLineSymbol(inters_line[i].startPt,inters_line[i].endPt);
			else 
				DrawWeldLineSymbol(inters_line[i].endPt,inters_line[i].startPt);
		}
		else if(i==1&&(m_cWeldLinePos&WELDLINE_FRONT_ZF))
		{
			if(line_vec*verify_vec<0)
				DrawWeldLineSymbol(inters_line[i].startPt,inters_line[i].endPt);
			else 
				DrawWeldLineSymbol(inters_line[i].endPt,inters_line[i].startPt);
		}
	}
	//����������ݵ�
	CTextFrame data_point;
	data_point.cls_id=pFrontVertPlate->GetClassTypeId();
	data_point.hPart=pFrontVertPlate->handle;
	//���ݵ�λ�á���עλ�ø����������ϲ��˵����ߵ��е�Ϊ��׼����
	data_point.dataPos=((inters_line[0].startPt.y>inters_line[0].endPt.y?inters_line[0].startPt:inters_line[0].endPt)
		+(inters_line[1].startPt.y>inters_line[1].endPt.y?inters_line[1].startPt:inters_line[1].endPt))*0.5;
	data_point.dimPos=data_point.dataPos;
	data_point.dataPos.y-=sys.dim_map.PartNo.fPartNoTextSize*1.5;
	data_point.dimPos.y+=sys.dim_map.PartNo.fPartNoTextSize*1.5;
	data_point.BuildFrame(GetPartNoIncMat(pFrontVertPlate->GetPartNo(),pFrontVertPlate->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle,sys.fonts.weld.fPartNoTextSize,sys.dim_map.PartNo.fPartNoMargin);
	DATAPOINT_TAG* pTag=AppendDataPoineTag();
	data_point.dwKeyTag=pTag->key_tag;
	pTag->vertexArr.append(inters_line[0].startPt);
	pTag->vertexArr.append(inters_line[1].startPt);
	pTag->vertexArr.append(inters_line[1].endPt);
	pTag->vertexArr.append(inters_line[0].endPt);
	dataPointList.append(data_point);
	//2.��ע��˨��༰������
	f3dPoint plate_pos=inters_pick;
	coord_trans(plate_pos,pBasePlate->ucs,TRUE);
	plate_pos=dcs.TransToDraw(plate_pos);
	DimViewHorizontalLsSpace(dcs,TRUE,plate_pos);
	LOCALE_PROPITEM* pLocaleItem=gxDrwLocalizer.GetLocaleItem("StdXPlates.blDimVertBoltSpace");
	if (pLocaleItem!=NULL&&pLocaleItem->bVal)
	{	//�ൺ����Ҫ��ʮ�ֲ���麸ͼ�б�עʮ�ֲ��������˨���
		DimViewVerticalLsSpace(dcs,TRUE,plate_pos);
	}
	//3.��ע�ְ���˨���
	//DimPlateLsSpace(dcs,pBasePlate,spaceDimList);
	//4.��������ͼ��
	DrawSectionViewSymbol(dcs);
	return TRUE;
}
BOOL CSectionStdXPlatesDrawing::CreateSideViewDrawing()
{
	CDrawCoordTrans dcs(cs,1.0);	//չ����ʽ������ת�������ݲ����ǻ��Ʊ������ڲ���ʱ����
	CStdXPlatesView *pStdXPlatesView=(CStdXPlatesView*)m_pSectionView;
	//1.���ƹ���������������庸���ʶ
	//1.1 ���ƻ������ͼ
	//1.1.1 �����׼����ǰ�������׼����
	CLDSPlate *pBasePlate=m_xParam.m_pBasePlate,*pFrontVertPlate=m_xParam.m_pFrontVertPlate,*pBackVertPlate=m_xParam.m_pBackVertPlate;
	f3dPoint inters_pick,inters_vec;
	f3dPoint base_middle_org=pBasePlate->ucs.origin+pBasePlate->ucs.axis_z*(pBasePlate->GetNormOffset()+pBasePlate->GetThick()*0.5);
	f3dPoint front_middle_org=pFrontVertPlate->ucs.origin+pFrontVertPlate->ucs.axis_z*(pFrontVertPlate->m_fNormOffset+pFrontVertPlate->GetThick()*0.5);
	Int3d2f(&inters_pick,&inters_vec,base_middle_org,pBasePlate->ucs.axis_z,front_middle_org,pFrontVertPlate->ucs.axis_z);
	GECS tmpcs=cs;
	GEPOINT xTmpInters=tmpcs.TransPToCS(inters_pick);
	cs.origin+=cs.axis_x*xTmpInters.x+cs.axis_y*xTmpInters.y;	//��ʮ�ֲ���м佻�߻���Ϊ����ԭ�㣨���ھ��У� wjh-2019.5.12
	dcs.SetCS(cs);
	//1.1.2 ����������������󽻻�ȡ���������彻��
	coord_trans(inters_pick,pFrontVertPlate->ucs,FALSE);
	vector_trans(inters_vec,pFrontVertPlate->ucs,FALSE);
	inters_pick.z=inters_vec.z=0;
	f3dPoint f_start,f_end,line_vec;
	f3dLine inters_line[2];	//0.ǰ����һ��Ľ��� 1.������һ��Ľ���
	CLDSPlate* platePtrArr[2]={m_xParam.m_pFrontVertPlate,m_xParam.m_pBackVertPlate};
	for(int i=0;i<2;i++)
	{
		int n=platePtrArr[i]->vertex_list.GetNodeNum();
		for(int j=0;j<n;j++)
		{
			PROFILE_VERTEX *pStartVertex=platePtrArr[i]->vertex_list.GetByIndex(j);
			PROFILE_VERTEX *pEndVertex=platePtrArr[i]->vertex_list.GetByIndex((j+1)%n);
			f_start = platePtrArr[i]->GetRealVertex(pStartVertex->vertex);
			f_end   = platePtrArr[i]->GetRealVertex(pEndVertex->vertex);
			line_vec=f_end-f_start;
			normalize(line_vec);
			double cosa=line_vec*inters_vec;
			if(fabs(cosa)<EPS_COS2)
				continue;
			if(cosa<0)
			{
				f3dPoint temp=f_start;
				f_start=f_end;
				f_end=temp;
			}
			if(inters_line[i].startPt.IsZero()&&inters_line[i].endPt.IsZero())
			{	//��ʼ������
				inters_line[i].startPt=f_start;
				inters_line[i].endPt=f_end;
			}
			else 
			{
				f3dPoint perp;
				double fDist1=0,fDist2=0;
				SnapPerp(&perp,inters_line[i].startPt,inters_line[i].endPt,inters_pick,&fDist1);
				SnapPerp(&perp,f_start,f_end,inters_pick,&fDist2);
				if(fDist2<fDist1)
				{
					inters_line[i].startPt=f_start;
					inters_line[i].endPt=f_end;
				}
			}
		}
		//������ת������ͼ����ϵ��
		coord_trans(inters_line[i].startPt,platePtrArr[i]->ucs,TRUE);
		coord_trans(inters_line[i].endPt,platePtrArr[i]->ucs,TRUE);
		inters_line[i].startPt=dcs.TransToDraw(inters_line[i].startPt);
		inters_line[i].endPt=dcs.TransToDraw(inters_line[i].endPt);
	}
	if( inters_line[0].startPt.IsZero()||inters_line[0].endPt.IsZero()||
		inters_line[1].startPt.IsZero()||inters_line[1].endPt.IsZero())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("fail to calculate intersection line of standard cross board through plate and front side plate!");
#else
		logerr.Log("��׼ʮ�ֲ���ͨ����ǰ�����彻�߼���ʧ�ܣ�");
#endif
		return FALSE;
	}
	//1.1.3 �������������߻��ƺ����ʶ
	f3dLine line,lineArr[2];
	for(int i=0;i<2;i++)
	{
		if(i==0)
		{
			line.startPt=inters_line[0].startPt;
			line.endPt=inters_line[1].startPt;
		}
		else 
		{
			line.startPt=inters_line[0].endPt;
			line.endPt=inters_line[1].endPt;
		}
		line.pen.style=PS_SOLID;
		line.ID=0;
		edgeList.append(LineToArc(line));
	}
	//1.1.4 ���ƺ����ʶ
	BYTE front_left1_right2=0,back_left1_right2=0;
	if( (cs.axis_z*pFrontVertPlate->ucs.axis_z>0&&(m_cWeldLinePos&WELDLINE_FRONT_ZZ))||
		(cs.axis_z*pFrontVertPlate->ucs.axis_z<0&&(m_cWeldLinePos&WELDLINE_FRONT_ZF)))
	{	//�ж�ǰ����һ���Ƿ���Ҫ���ƺ�����
		if(inters_line[0].startPt.x>inters_line[1].startPt.x)
		{	//ǰ���������Ҳ����
			front_left1_right2=2;
			back_left1_right2=1;
			if(inters_line[0].startPt.y>inters_line[0].endPt.y)
				DrawWeldLineSymbol(inters_line[0].startPt,inters_line[0].endPt);
			else 
				DrawWeldLineSymbol(inters_line[0].endPt,inters_line[0].startPt);
		}
		else 
		{	//ǰ��������������
			front_left1_right2=1;
			back_left1_right2=2;
			if(inters_line[0].startPt.y<inters_line[0].endPt.y)
				DrawWeldLineSymbol(inters_line[0].startPt,inters_line[0].endPt);
			else 
				DrawWeldLineSymbol(inters_line[0].endPt,inters_line[0].startPt);
		}
	}
	if( (cs.axis_z*pBackVertPlate->ucs.axis_z>0&&(m_cWeldLinePos&WELDLINE_BACK_ZZ))||
		(cs.axis_z*pBackVertPlate->ucs.axis_z<0&&(m_cWeldLinePos&WELDLINE_BACK_ZF)))
	{	//�жϺ�����һ���Ƿ���Ҫ���ƺ�����
		if(inters_line[0].startPt.x>inters_line[1].startPt.x)
		{	//���������������
			back_left1_right2=1;
			front_left1_right2=2;
			if(inters_line[1].startPt.y<inters_line[1].endPt.y)
				DrawWeldLineSymbol(inters_line[1].startPt,inters_line[1].endPt);
			else 
				DrawWeldLineSymbol(inters_line[1].endPt,inters_line[1].startPt);
		}
		else 
		{	//����������Ҳ����
			back_left1_right2=2;
			front_left1_right2=1;
			if(inters_line[1].startPt.y>inters_line[1].endPt.y)
				DrawWeldLineSymbol(inters_line[1].startPt,inters_line[1].endPt);
			else 
				DrawWeldLineSymbol(inters_line[1].endPt,inters_line[1].startPt);
		}
	}
	//1.1.5 ��ӹ�ͨ�����ݵ�
	CTextFrame data_point;
	data_point.cls_id=pBasePlate->GetClassTypeId();
	data_point.hPart=pBasePlate->handle;
	//���ݵ�λ�á���עλ�ø����������ϲ��˵����ߵ��е�Ϊ��׼����
	data_point.dataPos=((inters_line[0].startPt.y>inters_line[0].endPt.y?inters_line[0].startPt:inters_line[0].endPt)
						+(inters_line[1].startPt.y>inters_line[1].endPt.y?inters_line[1].startPt:inters_line[1].endPt))*0.5;
	data_point.dimPos=data_point.dataPos;
	data_point.dataPos.y-=sys.dim_map.PartNo.fPartNoTextSize*1.5;
	data_point.dimPos.y+=sys.dim_map.PartNo.fPartNoTextSize*1.5;
	data_point.BuildFrame(GetPartNoIncMat(pBasePlate->GetPartNo(),pBasePlate->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	DATAPOINT_TAG* pTag=AppendDataPoineTag();
	data_point.dwKeyTag=pTag->key_tag;
	pTag->vertexArr.append(inters_line[0].startPt);
	pTag->vertexArr.append(inters_line[1].startPt);
	pTag->vertexArr.append(inters_line[1].endPt);
	pTag->vertexArr.append(inters_line[0].endPt);
	dataPointList.append(data_point);
	//1.2 ��������ˮƽ����
	CLDSPart *pPart=TagPart(pStdXPlatesView->SonParts.GetValue(pStdXPlatesView->m_hFrontVertPlate));
	if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
		CreatePlateDrawing((CLDSPlate*)pPart,dcs,TRUE,FALSE,front_left1_right2);
	pPart=TagPart(pStdXPlatesView->SonParts.GetValue(pStdXPlatesView->m_hBackVertPlate));
	if(pPart&&pPart->GetClassTypeId()==CLS_PLATE)
		CreatePlateDrawing((CLDSPlate*)pPart,dcs,TRUE,FALSE,back_left1_right2);
	//2.��ע��˨��༰������
	f3dPoint plate_pos=inters_pick;
	coord_trans(plate_pos,pFrontVertPlate->ucs,TRUE);
	plate_pos=dcs.TransToDraw(plate_pos);
	DimViewHorizontalLsSpace(dcs, FALSE, plate_pos);
	LOCALE_PROPITEM* pLocaleItem=gxDrwLocalizer.GetLocaleItem("StdXPlates.blDimVertBoltSpace");
	if (pLocaleItem!=NULL&&pLocaleItem->bVal)
	{	//�ൺ����Ҫ��ʮ�ֲ���麸ͼ�б�עʮ�ֲ��������˨���
		DimViewVerticalLsSpace(dcs,FALSE,plate_pos);
	}
	//3.��ע�ְ���˨���
	//DimPlateLsSpace(dcs,pFrontVertPlate,spaceDimList);
	//DimPlateLsSpace(dcs,pBackVertPlate,spaceDimList);
	return TRUE;
}
void CSectionStdXPlatesDrawing::DrawLineAngleSection(f3dPoint ber_pos,f3dPoint x_vec_wing_pos,f3dPoint y_vec_wing_pos,double thick)
{
	int y_norm_flag=1,x_norm_flag=1;
	if(y_vec_wing_pos.y<ber_pos.y)
		x_norm_flag=-1;
	if(x_vec_wing_pos.x<ber_pos.x)
		y_norm_flag=-1;
	//
	f3dLine line;
	f3dArcLine arcLine;
	f3dPoint arc_start,arc_end,center_pick;
	line.pen.style=PS_SOLID;
	line.ID=0;
	//
	line.startPt=ber_pos;
	line.endPt=x_vec_wing_pos;
	edgeList.append(LineToArc(line));
	//
	arc_start=x_vec_wing_pos;
	arc_end.Set(x_vec_wing_pos.x-y_norm_flag*thick,x_vec_wing_pos.y+x_norm_flag*thick);
	center_pick.Set(x_vec_wing_pos.x-y_norm_flag*thick,x_vec_wing_pos.y);
	arcLine.CreateMethod3(arc_start,arc_end,f3dPoint(0,0,x_norm_flag*y_norm_flag),thick,center_pick);
	edgeList.append(arcLine);
	//
	line.startPt=arc_end;
	line.endPt.Set(ber_pos.x+2*y_norm_flag*thick,arc_end.y);
	edgeList.append(LineToArc(line));
	//
	arc_start=line.endPt;
	arc_end.Set(arc_start.x-y_norm_flag*thick,arc_start.y+x_norm_flag*thick);
	center_pick.Set(arc_end.x+y_norm_flag*thick,arc_end.y);
	arcLine.CreateMethod3(arc_start,arc_end,f3dPoint(0,0,-1*x_norm_flag*y_norm_flag),thick,center_pick);
	edgeList.append(arcLine);
	//
	line.startPt=arc_end;
	line.endPt.Set(y_vec_wing_pos.x+y_norm_flag*thick,y_vec_wing_pos.y-x_norm_flag*thick);
	edgeList.append(LineToArc(line));
	//
	arc_start=line.endPt;
	arc_end=y_vec_wing_pos;
	center_pick.Set(arc_end.x,arc_end.y-x_norm_flag*thick);
	arcLine.CreateMethod3(arc_start,arc_end,f3dPoint(0,0,x_norm_flag*y_norm_flag),thick,center_pick);
	edgeList.append(arcLine);
	//
	line.startPt=y_vec_wing_pos;
	line.endPt=ber_pos;
	edgeList.append(LineToArc(line));
}
BOOL CSectionStdXPlatesDrawing::CreateTopViewDrawing()
{
	CDrawCoordTrans dcs(cs,1.0);	//չ����ʽ������ת�������ݲ����ǻ��Ʊ������ڲ���ʱ����
	CStdXPlatesView *pStdXPlatesView=(CStdXPlatesView*)m_pSectionView;
	CLDSPlate *pBasePlate=m_xParam.m_pBasePlate,*pFrontVertPlate=m_xParam.m_pFrontVertPlate,*pBackVertPlate=m_xParam.m_pBackVertPlate;
	f3dPoint inters_pick,inters_vec,inters_pt;
	f3dPoint face1_norm=pBasePlate->ucs.axis_z,face2_norm=pFrontVertPlate->ucs.axis_z;
	f3dPoint face1_pick=pBasePlate->ucs.origin+face1_norm*(pBasePlate->m_fNormOffset+pBasePlate->GetThick()*0.5);
	f3dPoint face2_pick=pFrontVertPlate->ucs.origin+face2_norm*(pFrontVertPlate->m_fNormOffset+pFrontVertPlate->GetThick()*0.5);
	Int3d2f(&inters_pick,&inters_vec,face1_pick,face1_norm,face2_pick,face2_norm);
	GECS tmpcs=cs;
	GEPOINT xTmpInters=tmpcs.TransPToCS(inters_pick);
	cs.origin+=cs.axis_x*xTmpInters.x+cs.axis_y*xTmpInters.y;	//��ʮ�ֲ���м佻�߻���Ϊ����ԭ�㣨���ھ��У� wjh-2019.5.12
	dcs.SetCS(cs);
	//1.����ͨ��
	CreatePartSketch(pStdXPlatesView->SonParts.GetValue(pStdXPlatesView->m_hBasePlate));
	//2.�������弰�����ʶ
	//1.2.1 �����׼����ǰ�������׼����
	Int3dlf(inters_pt,inters_pick,inters_vec,cs.origin,cs.axis_z);
	inters_pt=dcs.TransToDraw(inters_pt);
	//1.2.2 �������峤��
	double fFrontVertWidth=0,fBackVertWidth=0;
	CLDSPlate* platePtrArr[2]={pFrontVertPlate,pBackVertPlate};
	f3dPoint geom_center[2];//={};
	for(int i=0;i<2;i++)
	{
		CMaxDouble max_y(0);
		CMinDouble min_y(0);
		for(PROFILE_VERTEX *pVertex=platePtrArr[i]->vertex_list.GetFirst();pVertex;pVertex=platePtrArr[i]->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,platePtrArr[i]->ucs,TRUE);
			geom_center[i]+=vertex;
			coord_trans(vertex,cs,FALSE);
			vertex.z=0;
			max_y.Update(vertex.y);
			min_y.Update(vertex.y);
		}
		if (i == 0)
			fFrontVertWidth = max_y.number - min_y.number;
		else
			fBackVertWidth = max_y.number - min_y.number;
		geom_center[i] /= platePtrArr[i]->vertex_list.GetNodeNum();
	}
	const int WELD_SPACE = 2;
	f3dPoint ber_pos, x_wing_pos, y_wing_pos;
	double wing_wide = m_xParam.m_arrAnglePtr[0]->GetWidth();
	double wing_thik = m_xParam.m_arrAnglePtr[0]->GetThick();

	//��������
	for (int i = 0; i < 2; i++)
	{
		double fPlateWidth = fFrontVertWidth;
		BYTE cWeldZZ = WELDLINE_FRONT_ZZ, cWeldZF = WELDLINE_FRONT_ZF;
		if (i == 1)
		{
			cWeldZZ = WELDLINE_BACK_ZZ;
			cWeldZF = WELDLINE_BACK_ZF;
			fPlateWidth = fBackVertWidth;
		}
		f3dPoint verify_vec = geom_center[i] - pBasePlate->ucs.origin;
		normalize(verify_vec);
		double fHalfThick = platePtrArr[i]->GetThick()*0.5;
		int x_flag = 1, y_flag = 1;
		if (platePtrArr[i]->ucs.axis_z*cs.axis_x < 0)
			x_flag = -1;
		f3dPoint base_pt(inters_pt.x, inters_pt.y + 0.5*pBasePlate->GetThick());
		if (verify_vec*cs.axis_y < 0)
		{
			y_flag = -1;
			base_pt.Set(inters_pt.x, inters_pt.y - 0.5*pBasePlate->GetThick());
		}
		f3dPoint left_top, right_top, left_bottom, right_bottom, corner_pt1, corner_pt2;
		left_top.Set(base_pt.x - x_flag * fHalfThick, base_pt.y + y_flag * fPlateWidth);
		right_top.Set(left_top.x + x_flag * 2 * fHalfThick, left_top.y);
		if (!(m_cWeldLinePos&cWeldZZ) && !(m_cWeldLinePos&cWeldZF))
		{	//�ְ������Ϊƽ��
			corner_pt1.Set(base_pt.x - x_flag * WELD_SPACE*0.5, base_pt.y);
			corner_pt2.Set(base_pt.x + x_flag * WELD_SPACE*0.5, base_pt.y);
			left_bottom.Set(base_pt.x - x_flag * fHalfThick, base_pt.y + y_flag * (fHalfThick - WELD_SPACE * 0.5));
			right_bottom.Set(base_pt.x + x_flag * fHalfThick, left_bottom.y);
			//���ְ��¿�
			ACAD_HATCH *pHatch = hatchList.append();
			pHatch->vertexList.append(left_bottom);
			pHatch->vertexList.append(base_pt.x - x_flag * fHalfThick, base_pt.y, 0);
			pHatch->vertexList.append(corner_pt1);
			//�ְ庸��
			pHatch = hatchList.append();
			pHatch->vertexList.append(corner_pt2);
			pHatch->vertexList.append(base_pt.x + x_flag * fHalfThick, base_pt.y, 0);
			pHatch->vertexList.append(right_bottom);
		}
		//�����Ϊ�����ߣ��˴������ܳ��ָ���� wht 15-04-05
		//else if(((m_cWeldLinePos&cWeldZZ)&&(m_cWeldLinePos&cWeldZF)))
		else if (m_cWeldLinePos&cWeldZF)
		{
			left_bottom.Set(base_pt.x - x_flag * fHalfThick, base_pt.y);
			corner_pt1.Set(left_bottom.x + x_flag * WELD_SPACE, base_pt.y);
			corner_pt2 = corner_pt1;
			right_bottom.Set(base_pt.x + x_flag * fHalfThick, base_pt.y + y_flag * (2 * fHalfThick - WELD_SPACE));
			//���ְ��¿�(�ڲ�ƽ��)
			ACAD_HATCH *pHatch = hatchList.append();
			f3dPoint *pPt1 = pHatch->vertexList.append(corner_pt2);
			f3dPoint *pPt2 = pHatch->vertexList.append(right_bottom);
			f3dPoint *pPt3 = pHatch->vertexList.append(base_pt.x + x_flag * fHalfThick, base_pt.y, 0);
			//�ְ庸��(���ϱߵ�pHatchƽ�Ƹְ���)
			pHatch = hatchList.append();
			pHatch->vertexList.append(pPt1->x - x_flag * fHalfThick * 2, pPt1->y, pPt1->z);
			pHatch->vertexList.append(pPt2->x - x_flag * fHalfThick * 2, pPt2->y, pPt2->z);
			pHatch->vertexList.append(pPt3->x - x_flag * fHalfThick * 2, pPt3->y, pPt3->z);
		}
		else if (m_cWeldLinePos&cWeldZZ)
		{
			right_bottom.Set(base_pt.x + x_flag * fHalfThick, base_pt.y);
			corner_pt1.Set(right_bottom.x - x_flag * WELD_SPACE, base_pt.y);
			corner_pt2 = corner_pt1;
			left_bottom.Set(base_pt.x - x_flag * fHalfThick, base_pt.y + y_flag * (2 * fHalfThick - WELD_SPACE));
			//���ְ��¿�(�ڲ�ƽ��)
			ACAD_HATCH *pHatch = hatchList.append();
			f3dPoint *pPt1 = pHatch->vertexList.append(corner_pt1);
			f3dPoint *pPt2 = pHatch->vertexList.append(base_pt.x - x_flag * fHalfThick, base_pt.y, 0);
			f3dPoint *pPt3 = pHatch->vertexList.append(left_bottom);
			//�ְ庸��(���ϱߵ�pHatchƽ�Ƹְ���)
			pHatch = hatchList.append();
			pHatch->vertexList.append(pPt1->x + x_flag * fHalfThick * 2, pPt1->y, pPt1->z);
			pHatch->vertexList.append(pPt2->x + x_flag * fHalfThick * 2, pPt2->y, pPt2->z);
			pHatch->vertexList.append(pPt3->x + x_flag * fHalfThick * 2, pPt3->y, pPt3->z);
		}
		//���ƿ�ģ�Ǹ�
		if (m_cWeldLinePos == 0 && m_xParam.m_arrAnglePtr[0] && m_xParam.m_arrAnglePtr[1] && m_xParam.m_arrAnglePtr[2] && m_xParam.m_arrAnglePtr[3])
		{	//ʮ����ϽǸ�
			CTextFrame data_point;
			wing_thik = m_xParam.m_arrAnglePtr[0]->GetThick();
			wing_wide = m_xParam.m_arrAnglePtr[0]->GetWidth();
			if (i == 0)
			{
				ber_pos.Set(base_pt.x + x_flag * fHalfThick, base_pt.y);
				x_wing_pos.Set(ber_pos.x + x_flag * wing_wide, ber_pos.y);
				y_wing_pos.Set(ber_pos.x, ber_pos.y + y_flag * wing_wide);
				DrawLineAngleSection(ber_pos, x_wing_pos, y_wing_pos, wing_thik);
				//��ӿ�ģ�Ǹ����ݵ�
				data_point.cls_id = CLS_LINEANGLE;
				data_point.hPart = m_xParam.m_arrAnglePtr[0]->handle;
				data_point.dataPos.Set(ber_pos.x + x_flag * wing_wide*0.5, ber_pos.y + y_flag * wing_thik*0.5);
				data_point.dimPos.Set(ber_pos.x + x_flag * wing_wide*0.7, ber_pos.y + y_flag * wing_wide*0.7);
				data_point.BuildFrame(GetPartNoIncMat(m_xParam.m_arrAnglePtr[0]->GetPartNo(), m_xParam.m_arrAnglePtr[0]->cMaterial), ZoomToModelTextSize(sys.fonts.weld.labelFrame));
				dataPointList.append(data_point);
				//
				ber_pos.Set(base_pt.x - x_flag * fHalfThick, base_pt.y);
				x_wing_pos.Set(ber_pos.x - x_flag * wing_wide, ber_pos.y);
				y_wing_pos.Set(ber_pos.x, ber_pos.y + y_flag * wing_wide);
				DrawLineAngleSection(ber_pos, x_wing_pos, y_wing_pos, wing_thik);
				//��ӿ�ģ�Ǹ����ݵ�
				data_point.cls_id = CLS_LINEANGLE;
				data_point.hPart = m_xParam.m_arrAnglePtr[1]->handle;
				data_point.dataPos.Set(ber_pos.x - x_flag * wing_wide*0.5, ber_pos.y + y_flag * wing_thik*0.5);
				data_point.dimPos.Set(ber_pos.x - x_flag * wing_wide*0.7, ber_pos.y + y_flag * wing_wide*0.7);
				data_point.BuildFrame(GetPartNoIncMat(m_xParam.m_arrAnglePtr[1]->GetPartNo(), m_xParam.m_arrAnglePtr[1]->cMaterial), ZoomToModelTextSize(sys.fonts.weld.labelFrame));
				dataPointList.append(data_point);
			}
			else
			{
				ber_pos.Set(base_pt.x - x_flag * fHalfThick, base_pt.y);
				x_wing_pos.Set(ber_pos.x - x_flag * wing_wide, ber_pos.y);
				y_wing_pos.Set(ber_pos.x, ber_pos.y + y_flag * wing_wide);
				DrawLineAngleSection(ber_pos, x_wing_pos, y_wing_pos, wing_thik);
				//��ӿ�ģ�Ǹ����ݵ�
				data_point.cls_id = CLS_LINEANGLE;
				data_point.hPart = m_xParam.m_arrAnglePtr[2]->handle;
				data_point.dataPos.Set(ber_pos.x - x_flag * wing_wide*0.5, ber_pos.y + y_flag * wing_thik*0.5);
				data_point.dimPos.Set(ber_pos.x - x_flag * wing_wide*0.7, ber_pos.y + y_flag * wing_wide*0.7);
				data_point.BuildFrame(GetPartNoIncMat(m_xParam.m_arrAnglePtr[2]->GetPartNo(), m_xParam.m_arrAnglePtr[2]->cMaterial), ZoomToModelTextSize(sys.fonts.weld.labelFrame));
				dataPointList.append(data_point);
				//
				ber_pos.Set(base_pt.x + x_flag * fHalfThick, base_pt.y);
				x_wing_pos.Set(ber_pos.x + x_flag * wing_wide, ber_pos.y);
				y_wing_pos.Set(ber_pos.x, ber_pos.y + y_flag * wing_wide);
				DrawLineAngleSection(ber_pos, x_wing_pos, y_wing_pos, wing_thik);
				//��ӿ�ģ�Ǹ����ݵ�
				data_point.cls_id = CLS_LINEANGLE;
				data_point.hPart = m_xParam.m_arrAnglePtr[3]->handle;
				data_point.dataPos.Set(ber_pos.x + x_flag * wing_wide*0.5, ber_pos.y + y_flag * wing_thik*0.5);
				data_point.dimPos.Set(ber_pos.x + x_flag * wing_wide*0.7, ber_pos.y + y_flag * wing_wide*0.7);
				data_point.BuildFrame(GetPartNoIncMat(m_xParam.m_arrAnglePtr[3]->GetPartNo(), m_xParam.m_arrAnglePtr[3]->cMaterial), ZoomToModelTextSize(sys.fonts.weld.labelFrame));
				dataPointList.append(data_point);
			}
		}
		else
		{	//�Խ���ϽǸ�
			if (m_cKmAngle1Pos == cWeldZZ || m_cKmAngle2Pos == cWeldZZ)
			{	//���ƽǸֽ���
				CLDSLineAngle *pKmAngle = m_xParam.m_arrAnglePtr[0];
				if (m_cKmAngle2Pos == cWeldZZ)
					pKmAngle = m_xParam.m_arrAnglePtr[1];
				wing_thik = pKmAngle->GetThick();
				wing_wide = pKmAngle->GetWidth();
				ber_pos.Set(base_pt.x + x_flag * fHalfThick, base_pt.y);
				x_wing_pos.Set(ber_pos.x + x_flag * wing_wide, ber_pos.y);
				y_wing_pos.Set(ber_pos.x, ber_pos.y + y_flag * wing_wide);
				DrawLineAngleSection(ber_pos, x_wing_pos, y_wing_pos, wing_thik);
				CTextFrame data_point;
				data_point.cls_id = pKmAngle->GetClassTypeId();
				data_point.hPart = pKmAngle->handle;
				data_point.dataPos.Set(ber_pos.x + x_flag * wing_wide*0.5, ber_pos.y + y_flag * wing_thik*0.5);
				data_point.dimPos.Set(ber_pos.x + x_flag * wing_wide*0.7, ber_pos.y + y_flag * wing_wide*0.7);
				data_point.BuildFrame(GetPartNoIncMat(pKmAngle->GetPartNo(), pKmAngle->cMaterial), ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
				dataPointList.append(data_point);
			}
			if (m_cKmAngle1Pos == cWeldZF || m_cKmAngle2Pos == cWeldZF)
			{	//���ƽǸֽ���
				CLDSLineAngle *pKmAngle = m_xParam.m_arrAnglePtr[0];
				if (m_cKmAngle2Pos == cWeldZF)
					pKmAngle = m_xParam.m_arrAnglePtr[1];
				wing_thik = pKmAngle->GetThick();
				wing_wide = pKmAngle->GetWidth();
				ber_pos.Set(base_pt.x - x_flag * fHalfThick, base_pt.y);
				x_wing_pos.Set(ber_pos.x - x_flag * wing_wide, ber_pos.y);
				y_wing_pos.Set(ber_pos.x, ber_pos.y + y_flag * wing_wide);
				DrawLineAngleSection(ber_pos, x_wing_pos, y_wing_pos, wing_thik);
				//��ӿ�ģ�Ǹ����ݵ�
				CTextFrame data_point;
				data_point.cls_id = pKmAngle->GetClassTypeId();
				data_point.hPart = pKmAngle->handle;
				data_point.dataPos.Set(ber_pos.x - x_flag * wing_wide*0.5, ber_pos.y + y_flag * wing_thik*0.5);
				data_point.dimPos.Set(ber_pos.x - x_flag * wing_wide*0.7, ber_pos.y + y_flag * wing_wide*0.7);
				data_point.BuildFrame(GetPartNoIncMat(pKmAngle->GetPartNo(), pKmAngle->cMaterial), ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
				dataPointList.append(data_point);
			}
		}		
		f3dLine line;
		line.pen.style=PS_SOLID;
		line.ID=0;
		line.startPt=left_top;
		line.endPt=left_bottom;
		edgeList.append(LineToArc(line));
		line.startPt=left_top;
		line.endPt=right_top;
		edgeList.append(LineToArc(line));
		if(corner_pt1.IsEqual(left_bottom))
		{
			line.startPt=left_bottom;
			line.endPt=right_bottom;
			edgeList.append(LineToArc(line));
		}
		else
		{
			line.startPt=left_bottom;
			line.endPt=corner_pt1;
			edgeList.append(LineToArc(line));
			if(!corner_pt1.IsEqual(corner_pt2))
			{
				line.startPt=corner_pt1;
				line.endPt=corner_pt2;
				edgeList.append(LineToArc(line));
			}
			line.startPt=corner_pt2;
			line.endPt=right_bottom;
			edgeList.append(LineToArc(line));
		}
		line.startPt=right_top;
		line.endPt=right_bottom;
		edgeList.append(LineToArc(line));
		//��Ӹְ����ݵ�
		CTextFrame data_point;
		data_point.cls_id=platePtrArr[i]->GetClassTypeId();
		data_point.hPart=platePtrArr[i]->handle;
		data_point.dataPos.Set(base_pt.x,0.5*(left_top.y+left_bottom.y));
		data_point.dimPos.Set(base_pt.x-x_flag*wing_wide*0.5,0.5*(left_top.y+left_bottom.y));
		data_point.BuildFrame(GetPartNoIncMat(platePtrArr[i]->GetPartNo(),platePtrArr[i]->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
		DATAPOINT_TAG* pTag=AppendDataPoineTag();
		data_point.dwKeyTag=pTag->key_tag;
		pTag->vertexArr.append(right_top);
		pTag->vertexArr.append(left_top);
		pTag->vertexArr.append(left_bottom);
		pTag->vertexArr.append(right_bottom);
		dataPointList.append(data_point);
	}
	return TRUE;
}

BOOL CSectionStdXPlatesDrawing::CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL||pSectionView->GetViewTypeId()!=CSectionView::CLS_STDX_PLATES_VIEW)
		return FALSE;
	//1����ʼ��������Ϣ
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	m_pPartGroup=pPartGroup;
	//
	CStdXPlatesView *pStdXPlatesView=(CStdXPlatesView*)pSectionView;
	m_xParam.m_pBasePlate=(CLDSPlate*)m_pTower->FromPartHandle(pStdXPlatesView->m_hBasePlate,CLS_PLATE);
	m_xParam.m_pFrontVertPlate=(CLDSPlate*)m_pTower->FromPartHandle(pStdXPlatesView->m_hFrontVertPlate,CLS_PLATE);
	m_xParam.m_pBackVertPlate=(CLDSPlate*)m_pTower->FromPartHandle(pStdXPlatesView->m_hBackVertPlate,CLS_PLATE);
	int index=0;
	m_xParam.m_arrAnglePtr[0]=m_xParam.m_arrAnglePtr[1]=m_xParam.m_arrAnglePtr[2]=m_xParam.m_arrAnglePtr[3]=NULL;
	for(int i=0;i<4;i++)
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)m_pTower->FromPartHandle(pStdXPlatesView->m_arrAngleHandle[i],CLS_LINEANGLE);
		if(pAngle&&!pAngle->m_bVirtualPart)
			m_xParam.m_arrAnglePtr[index++]=pAngle;
	}
	_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	m_xEntManager.InitContext(m_pRootPart,this,STORE_ACAD);
	if( m_pRootPart==NULL||m_xParam.m_pBasePlate==NULL||
		m_xParam.m_pFrontVertPlate==NULL||m_xParam.m_pBackVertPlate==NULL||
		m_xParam.m_arrAnglePtr[0]==NULL||m_xParam.m_arrAnglePtr[1]==NULL)
	{
		if(m_xParam.m_pBasePlate)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0x%X weld plates{%s} lost data of relative weld rib plate or angle��fail to generate weld map",pStdXPlatesView->m_hBasePlate,(char*)m_xParam.m_pBasePlate->GetPartNo());
		else
			logerr.Log("0x%X weld plates lost data of weld datum plate, fail to generate weld map",pStdXPlatesView->m_hBasePlate);
#else
			logerr.Log("0x%X�麸��{%s}��ʧ���������߰�򿿽ӽǸ����ݣ��麸ͼ����ʧ��",pStdXPlatesView->m_hBasePlate,(char*)m_xParam.m_pBasePlate->GetPartNo());
		else
			logerr.Log("0x%X�麸�嶪ʧ���ӻ�׼�ְ����ݣ��麸ͼ����ʧ��",pStdXPlatesView->m_hBasePlate);
#endif
		return FALSE;
	}
	m_pSectionView->des_cs.SetModifiedState();
	cs=m_pSectionView->UpdateUCS(pTower,pPartGroup);		//����ϵ
	CSectionView::PART_ITEM *pItem=NULL;
	for(pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
		m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
	CLDSGeneralPlate *platePtrArr[3]={m_xParam.m_pBasePlate,m_xParam.m_pFrontVertPlate,m_xParam.m_pBackVertPlate};
	m_cWeldLinePos=InitWeldLinePos(platePtrArr,m_xParam.m_arrAnglePtr,&m_cKmAngle1Pos,&m_cKmAngle2Pos);
	//2����ʼ���Խ����ͼԪ�������б������������������ߴ��ע���Ƕȱ�ע��
	if(pStdXPlatesView->m_cSubType=='F')
		CreateFrontViewDrawing();
	else if(pStdXPlatesView->m_cSubType=='S')
	{
		pStdXPlatesView->symbol.Copy("A");
		CreateSideViewDrawing();
	}
	else if(pStdXPlatesView->m_cSubType=='D')
	{	
		pStdXPlatesView->symbol.Copy("B");
		CreateTopViewDrawing();
	}
	else 
		return FALSE;
	//3������ͼ���в���
	if(m_bFinalDrawing=bFinalDrawing)
		RelayoutDrawing();
	return TRUE;
}
void CSectionStdXPlatesDrawing::FinalDrawing()
{
	if(!m_bFinalDrawing)
	{
		RelayoutDrawing();
		m_bFinalDrawing=true;
	}
}

//////////////////////////////////////////////////////////////////////////
// CSectionAngleFootDrawing
CSectionAngleFootDrawing::CSectionAngleFootDrawing(int id/* =0 */,IDrawing* pParentDrawing/* =NULL */)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionViewDrawing::CLS_ANGLE_FOOT_VIEW;
	m_cWeldLinePos=0;
	m_bSamePartNoDimOnce=FALSE;
}
void CSectionAngleFootDrawing::DrawSectionViewSymbol(CDrawCoordTrans &dcs)
{
	CAngleFootView *pAngleFootView=(CAngleFootView*)pSectionView;
	if(pAngleFootView==NULL||pAngleFootView->m_cSubType!='F')
		return;
	f2dRect rect=GetDrawRegion();
	//��ǰ��ͼ�л��Ʋ���ͼ������ͼ��
	double symbol_dist=rect.Height()*0.3;
	f3dPoint dim_vec,offset_vec,dim_pos;
	dim_vec.Set(1,0,0);
	dim_pos.Set(rect.topLeft.x,rect.topLeft.y-rect.Height()*0.5);
	dim_pos-=dim_vec*50;
	offset_vec=dim_vec;
	RotateVectorAroundVector(offset_vec,0.5*Pi,f3dPoint(0,0,1));
	double offset_angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
	for(int i=0;i<2;i++)
	{
		CTextOnlyDrawing *pSectionViewDim=pureTextDimList.append();
		pSectionViewDim->iOrder=3;	///����ͼ��
		pSectionViewDim->bNeedConvertDistToText=false;
		pSectionViewDim->dimText.Copy("A"); 
		pSectionViewDim->angle=offset_angle;
		pSectionViewDim->textHeight=1.5*sys.dim_map.fDimTextSize;
		if(i==0)
		{
			pSectionViewDim->bInXWing=FALSE; //�ϲ�
			pSectionViewDim->dimPos=dim_pos+offset_vec*symbol_dist;
		}
		else
		{
			pSectionViewDim->bInXWing=TRUE; //�²�
			pSectionViewDim->dimPos=dim_pos-(offset_vec*symbol_dist);
		}
	}
	//��ǰ��ͼ�л��Ƹ���ͼ������ͼ��
	symbol_dist=rect.Width()*0.3;
	dim_vec.Set(0,-1,0);
	dim_pos.Set(rect.topLeft.x+rect.Width()*0.5,rect.topLeft.y);
	dim_pos-=dim_vec*50;
	offset_vec=dim_vec;
	RotateVectorAroundVector(offset_vec,0.5*Pi,f3dPoint(0,0,1));
	offset_angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
	for(int i=0;i<2;i++)
	{
		CTextOnlyDrawing *pSectionViewDim=pureTextDimList.append();
		pSectionViewDim->iOrder=3;	///����ͼ��
		pSectionViewDim->bNeedConvertDistToText=false;
		pSectionViewDim->dimText.Copy("B"); 
		pSectionViewDim->angle=offset_angle;
		pSectionViewDim->textHeight=1.5*sys.dim_map.fDimTextSize;
		if(i==0)
		{
			pSectionViewDim->bInXWing=FALSE; //�ϲ�
			pSectionViewDim->dimPos=dim_pos+offset_vec*symbol_dist;
		}
		else
		{
			pSectionViewDim->bInXWing=TRUE; //�²�
			pSectionViewDim->dimPos=dim_pos-(offset_vec*symbol_dist);
		}
	}
	if(pAngleFootView->IsHoofView())
	{	//ţ��壬��Ҫ��ǰ��ͼ�л��Ƹ���ͼ������ͼ��
		symbol_dist=rect.Width()*0.3;
		dim_vec.Set(0,1,0);
		dim_pos.Set(rect.bottomRight.x-rect.Width()*0.5,rect.bottomRight.y);
		dim_pos-=dim_vec*50;
		offset_vec=dim_vec;
		RotateVectorAroundVector(offset_vec,0.5*Pi,f3dPoint(0,0,1));
		offset_angle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
		for(int i=0;i<2;i++)
		{
			CTextOnlyDrawing *pSectionViewDim=pureTextDimList.append();
			pSectionViewDim->iOrder=3;	///����ͼ��
			pSectionViewDim->bNeedConvertDistToText=false;
			pSectionViewDim->dimText.Copy("C"); 
			pSectionViewDim->angle=offset_angle;
			pSectionViewDim->textHeight=1.5*sys.dim_map.fDimTextSize;
			if(i==0)
			{
				pSectionViewDim->bInXWing=FALSE; //�ϲ�
				pSectionViewDim->dimPos=dim_pos+offset_vec*symbol_dist;
			}
			else
			{
				pSectionViewDim->bInXWing=TRUE; //�²�
				pSectionViewDim->dimPos=dim_pos-(offset_vec*symbol_dist);
			}
		}
	}
}
BOOL CSectionAngleFootDrawing::CreateFrontViewDrawing()
{
	CDrawCoordTrans dcs(cs,1.0);	//չ����ʽ������ת�������ݲ����ǻ��Ʊ������ڲ���ʱ����
	//1.�������Ű�
	for(CAngleFootView::PART_ITEM *pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		CLDSPart *pPart=TagPart(pItem);
		if(pPart==NULL||!pPart->IsPlate())
			continue;
		if( pPart==m_xParam.m_pFootPlate||pPart==m_xParam.m_pShoePlate||pPart==m_xParam.m_pFrontShoePlate||
			pPart==m_xParam.m_pBackShoePlate||pPart==m_xParam.m_pRightShoePlate)
		{
			if(pPart!=m_xParam.m_pBackShoePlate)
				CreatePartSketch(pItem);	//ǰ��ͼ�����ƺ��ѥ��
		}
		else if( pPart==m_xDownParam.m_pShoePlate||pPart==m_xDownParam.m_pFrontShoePlate||
			pPart==m_xDownParam.m_pBackShoePlate||pPart==m_xDownParam.m_pRightShoePlate)
		{
			if(pPart!=m_xDownParam.m_pBackShoePlate)
				CreatePartSketch(pItem);	//ǰ��ͼ�����ƺ��ѥ��
		}
		else if(fabs(pPart->ucs.axis_z*dcs.cs.axis_z)<0.707)
			CreatePartSketch(pItem);		//�����Ű��ѥ��֮���������Ҫ��ֱ���Ƶĸְ�
	}
	//2.��ע�ְ���˨���
	/*CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xParam.m_pShoePlate,spaceDimList);
	if(m_xParam.m_pRightShoePlate)
		CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xParam.m_pRightShoePlate,spaceDimList);
	if(IsHoofViewDrawing())
	{
		if(m_xDownParam.m_pShoePlate)
			CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xDownParam.m_pShoePlate,spaceDimList);
		if(m_xDownParam.m_pRightShoePlate)
			CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xDownParam.m_pRightShoePlate,spaceDimList);
	}*/
	//3.��������ͼ��
	DrawSectionViewSymbol(dcs);
	return TRUE;
}
BOOL CSectionAngleFootDrawing::CreateSideViewDrawing()
{
	CDrawCoordTrans dcs(cs,1.0);	//չ����ʽ������ת�������ݲ����ǻ��Ʊ������ڲ���ʱ����
	//1.�������Ű�
	for(CAngleFootView::PART_ITEM *pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		CLDSPart *pPart=TagPart(pItem);
		if(pPart==NULL||!pPart->IsPlate())
			continue;
		if( pPart==m_xParam.m_pFootPlate||pPart==m_xParam.m_pShoePlate||pPart==m_xParam.m_pFrontShoePlate||
			pPart==m_xParam.m_pBackShoePlate||pPart==m_xParam.m_pRightShoePlate)
		{
			if(pPart!=m_xParam.m_pRightShoePlate)
				CreatePartSketch(pItem);	//����ͼ�������Ҳ�ѥ��
		}
		if( pPart==m_xDownParam.m_pShoePlate||pPart==m_xDownParam.m_pFrontShoePlate||
			pPart==m_xDownParam.m_pBackShoePlate||pPart==m_xDownParam.m_pRightShoePlate)
		{
			if(pPart!=m_xDownParam.m_pRightShoePlate)
				CreatePartSketch(pItem);	//ǰ��ͼ�����ƺ��ѥ��
		}
		else if(fabs(pPart->ucs.axis_z*dcs.cs.axis_z)<0.707)
			CreatePartSketch(pItem);		//�����Ű��ѥ��֮���������Ҫ��ֱ���Ƶĸְ�
	}
	//2.��ע�ְ���˨���
	/*CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xParam.m_pFrontShoePlate,spaceDimList);
	CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xParam.m_pBackShoePlate,spaceDimList);
	if(IsHoofViewDrawing())
	{
		if(m_xDownParam.m_pFrontShoePlate)
			CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xDownParam.m_pFrontShoePlate,spaceDimList);
		if(m_xDownParam.m_pBackShoePlate)
			CSectionStdXPlatesDrawing::DimPlateLsSpace(dcs,m_xDownParam.m_pBackShoePlate,spaceDimList);
	}*/
	return TRUE;
}
//�������ְ彻��
static bool Cal2PlatesIntersLine(CLDSGeneralPlate *pFootPlate,CLDSGeneralPlate *pVertPlate,f3dLine &inters_line1,f3dLine &inters_line2)
{
	f3dPoint geom_center;
	for(PROFILE_VERTEX *pVertex=pVertPlate->vertex_list.GetFirst();pVertex;pVertex=pVertPlate->vertex_list.GetNext())
		geom_center+=pVertex->vertex;
	geom_center/=pVertPlate->vertex_list.GetNodeNum();
	coord_trans(geom_center,pVertPlate->ucs,TRUE);
	f3dPoint plate_reside_vec=geom_center-pFootPlate->ucs.origin;
	//1.���㽻�����췽�򣬵װ��׼��
	f3dPoint face_pick,face_norm=pFootPlate->ucs.axis_z;
	if(pFootPlate->ucs.axis_z*plate_reside_vec>pFootPlate->GetNormOffset()+pFootPlate->GetThick()*0.5)	//��ƽ��
		face_pick=pFootPlate->ucs.origin+pFootPlate->ucs.axis_z*(pFootPlate->GetThick()+pFootPlate->GetNormOffset());
	else
		face_pick=pFootPlate->ucs.origin+pFootPlate->ucs.axis_z*pFootPlate->GetNormOffset();
	//2.ȷ���װ�����Χ
	SCOPE_STRU scope;
	UCS_STRU ucs=pFootPlate->ucs;
	ucs.origin=face_pick;
	for(PROFILE_VERTEX *pVertex=pFootPlate->vertex_list.GetFirst();pVertex;pVertex=pFootPlate->vertex_list.GetNext())
	{
		f3dPoint vertex=pVertex->vertex;
		coord_trans(vertex,ucs,TRUE);
		scope.VerifyVertex(f3dPoint(vertex.x,vertex.y,vertex.z+500));
		scope.VerifyVertex(f3dPoint(vertex.x,vertex.y,vertex.z-500));
	}
	//3.�����ְ������߲��������췽�����Ҿ���װ������������
	//TODO:Ϊʲô�����ҵ��ױߺ�ֱ������ͶӰ��ȡ���ߣ��������������ַǳ��������㷨��wjh-2015.4.26
	f3dLine prev_prev_line,prev_line,next_line,next_next_line;
	int n=pVertPlate->vertex_list.GetNodeNum();
	f3dPoint startPt,endPt,line_vec;
	f3dPoint inters_vec=pFootPlate->ucs.axis_z^pVertPlate->ucs.axis_z;
	normalize(inters_vec);
	vector_trans(inters_vec,pVertPlate->ucs,FALSE);
	vector_trans(plate_reside_vec,pVertPlate->ucs,FALSE);
	for(int i=0;i<n;i++)
	{
		startPt=pVertPlate->vertex_list.GetByIndex(i)->vertex;
		endPt=pVertPlate->vertex_list.GetByIndex((i+1)%n)->vertex;
		line_vec=endPt-startPt;
		normalize(line_vec);
		if(fabs(line_vec*inters_vec)>EPS_COS2)
		{	
			f3dPoint verify_vec=0.5*(inters_line1.startPt+inters_line1.endPt)-0.5*(startPt+endPt);
			BOOL bEqual=inters_line1.startPt.IsEqual(inters_line1.endPt);
			if(bEqual||(!bEqual&&verify_vec*plate_reside_vec>0))
			{
				inters_line1.startPt=startPt;
				inters_line1.endPt=endPt;
				prev_prev_line.startPt=pVertPlate->vertex_list.GetByIndex((i+n-2)%n)->vertex;
				prev_prev_line.endPt=pVertPlate->vertex_list.GetByIndex((i+n-1)%n)->vertex;
				prev_line.startPt=prev_prev_line.endPt;
				prev_line.endPt=inters_line1.startPt;
				next_line.startPt=inters_line1.endPt;
				next_line.endPt=pVertPlate->vertex_list.GetByIndex((i+2)%n)->vertex;
				next_next_line.startPt=next_line.endPt;
				next_next_line.endPt=pVertPlate->vertex_list.GetByIndex((i+3)%n)->vertex;
			}
		}
	}
	if(inters_line1.startPt.IsZero()&&inters_line1.endPt.IsZero())
		return false;
	//4.ȡ2���ҵ������ߵ�ǰһ�ߺͺ�һ����װ���
	coord_trans(prev_line.startPt,pVertPlate->ucs,TRUE);
	coord_trans(prev_line.endPt,pVertPlate->ucs,TRUE);
	coord_trans(prev_prev_line.startPt,pVertPlate->ucs,TRUE);
	coord_trans(prev_prev_line.endPt,pVertPlate->ucs,TRUE);
	coord_trans(next_line.startPt,pVertPlate->ucs,TRUE);
	coord_trans(next_line.endPt,pVertPlate->ucs,TRUE);
	coord_trans(next_next_line.startPt,pVertPlate->ucs,TRUE);
	coord_trans(next_next_line.endPt,pVertPlate->ucs,TRUE);
	double fNormOffset=0;
	if(pVertPlate->GetClassTypeId()==CLS_PLATE)
		fNormOffset=pVertPlate->GetThick()*0.5+pVertPlate->GetNormOffset();
	else if(pVertPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pVertPlate;
		if(pParamPlate->m_iParamType==TYPE_BASEPLATE||pParamPlate->m_iParamType==TYPE_SHOEPLATE)
			fNormOffset=pVertPlate->GetThick()*0.5;
	}
	f3dLine prev_line_arr[2]={prev_line,prev_line};
	f3dLine prev_prev_line_arr[2]={prev_prev_line,prev_prev_line};
	f3dLine next_line_arr[2]={next_line,next_line};
	f3dLine next_next_line_arr[2]={next_next_line,next_next_line};
	f3dLine inters_line_arr[2];
	for(int i=0;i<2;i++)
	{
		double fOffset=fNormOffset-0.5*pVertPlate->GetThick();
		if(i==1)
			fOffset=fNormOffset+0.5*pVertPlate->GetThick();
		prev_line_arr[i].startPt+=pVertPlate->ucs.axis_z*fOffset;
		prev_line_arr[i].endPt+=pVertPlate->ucs.axis_z*fOffset;
		prev_prev_line_arr[i].startPt+=pVertPlate->ucs.axis_z*fOffset;
		prev_prev_line_arr[i].endPt+=pVertPlate->ucs.axis_z*fOffset;
		next_line_arr[i].startPt+=pVertPlate->ucs.axis_z*fOffset;
		next_line_arr[i].endPt+=pVertPlate->ucs.axis_z*fOffset;
		next_next_line_arr[i].startPt+=pVertPlate->ucs.axis_z*fOffset;
		next_next_line_arr[i].endPt+=pVertPlate->ucs.axis_z*fOffset;
		//
		Int3dlf(inters_line_arr[i].startPt,prev_line_arr[i],face_pick,face_norm);
		Int3dlf(inters_line_arr[i].endPt,next_line_arr[i],face_pick,face_norm);
		//�����н�ʱʹ��ǰһ�߻��һ�߼������׼ȷ����Ҫ����ǰǰ�߻������
		f3dPoint prev_vec=prev_line_arr[i].startPt-prev_line_arr[i].endPt;
		f3dPoint prev_prev_vec=prev_prev_line_arr[i].startPt-prev_prev_line_arr[i].endPt;
		normalize(prev_vec);
		normalize(prev_prev_vec);
		if( fabs(prev_prev_vec*pFootPlate->ucs.axis_z)>fabs(prev_vec*pFootPlate->ucs.axis_z)&&
			Int3dlf(startPt,prev_prev_line_arr[i],face_pick,face_norm)==1&&scope.IsIncludePoint(startPt))
			inters_line_arr[i].startPt=startPt;
		f3dPoint next_vec=next_line_arr[i].startPt-next_line_arr[i].endPt;
		f3dPoint next_next_vec=next_line_arr[i].startPt-next_next_line_arr[i].endPt;
		normalize(next_vec);
		normalize(next_next_vec);
		if( fabs(next_next_vec*pFootPlate->ucs.axis_z)>fabs(next_vec*pFootPlate->ucs.axis_z)&&
			Int3dlf(endPt,next_next_line_arr[i],face_pick,face_norm)==1&&scope.IsIncludePoint(endPt))
			inters_line_arr[i].endPt=endPt;
	}
	inters_line1=inters_line_arr[0];
	inters_line2=inters_line_arr[1];
	return true;
}
//����ǰ�����ְ�
//����: pPlate		�����Ƶĸְ�
//      inters_line �ְ������ߣ�Ϊ��ͼ����ϵ�����ֵ��
//		base_plate_center	��ͨ�����ĵ�(���Ƴ����ϽǺ���תѥ��ʱ����ת���ģ������ж���һ����Ҫ���ƺ����ʶ)
//�����CXhPtrSet<ACAD_HATCH> &hatchSet	��Ҫƽ����ACAD_HATCH,��עƽ������ʱʹ��
//      dim_key_pt	���ڱ�עѥ��λ�õĹؼ���
BOOL CSectionAngleFootDrawing::DrawVerticalShoePlate(CLDSGeneralPlate *pPlate,f3dLine inters_line,
													 f3dPoint base_plate_center,CXhPtrSet<ACAD_HATCH> &hatchSet,
													 GEPOINT &key_pt_for_dim)
{
	BYTE cWeldZZ=0,cWeldZF=0;
	CAngleFootView *pAngleFootView=(CAngleFootView*)m_pSectionView;
	CLDSGeneralPlate *pFrontShoePlate=m_xParam.m_pFrontShoePlate;
	CLDSGeneralPlate *pBackShoePlate=m_xParam.m_pBackShoePlate;
	if(pAngleFootView->m_cSubType=='U')
	{
		if(m_xDownParam.m_pFrontShoePlate==NULL||m_xDownParam.m_pBackShoePlate==NULL)
		{	//����ͼǰ�����ѥ��ΪNULL����ʾǰ��෽���ϵİ�Ϊͨ��
			//�˴����뱣֤pFrontShoePlate==platePtrArr[0]&&pBackShoePlate==platePtrArr[1]
			//(platePtrArrΪ����InitWeldLinePosʱ����İ�����),����m_cWeldLinePos�����׼��һ�¶�ʧЧ wht 15-04-28
			pFrontShoePlate=m_xDownParam.m_pShoePlate;
			pBackShoePlate=m_xDownParam.m_pRightShoePlate;
		}
		else 
		{
			pFrontShoePlate=m_xDownParam.m_pFrontShoePlate;
			pBackShoePlate=m_xDownParam.m_pBackShoePlate;
		}
	}
	if(pPlate==pFrontShoePlate)
	{
		cWeldZZ=WELDLINE_FRONT_ZZ;
		cWeldZF=WELDLINE_FRONT_ZF;
	}
	else if(pPlate==pBackShoePlate)
	{
		cWeldZZ=WELDLINE_BACK_ZZ;
		cWeldZF=WELDLINE_BACK_ZF;
	}
	else
		return FALSE;
	const int WELD_SPACE=2;		//�����϶
	f3dPoint base_pt,top_pt;
	if(DISTANCE(base_plate_center,inters_line.startPt)<DISTANCE(base_plate_center,inters_line.endPt))
	{
		base_pt=inters_line.startPt;
		top_pt=inters_line.endPt;
	}
	else
	{
		base_pt=inters_line.endPt;
		top_pt=inters_line.startPt;
	}
	double fHalfThick=pPlate->GetThick()*0.5;
	f3dPoint line_vec=top_pt-base_pt;
	normalize(line_vec);
	f3dPoint norm_vec(-line_vec.y,line_vec.x);
	f3dPoint plate_norm=pPlate->ucs.axis_z;
	normalize(plate_norm);
	vector_trans(plate_norm,cs,FALSE);
	BOOL bAnticlockwiseNorm=TRUE;
	if(plate_norm*norm_vec<0)
	{
		bAnticlockwiseNorm=FALSE;
		norm_vec*=-1;	//��֤norm_vec��巨�߷���һ��(��Ϊm_cWeldLinePos���ڰ巨�߷�����е��ж�)
	}
	f3dPoint top_pt_arr[2],base_pt_arr[2],corner_pt1,corner_pt2;
	top_pt_arr[0]=top_pt-norm_vec*fHalfThick;
	top_pt_arr[1]=top_pt+norm_vec*fHalfThick;
	if(!(m_cWeldLinePos&cWeldZZ)&&!(m_cWeldLinePos&cWeldZF))
	{	//�ְ�����ƽ��
		corner_pt1=base_pt-norm_vec*WELD_SPACE*0.5;
		corner_pt2=base_pt+norm_vec*WELD_SPACE*0.5;
		base_pt_arr[0]=base_pt-norm_vec*fHalfThick+line_vec*(fHalfThick-WELD_SPACE*0.5);
		base_pt_arr[1]=base_pt+norm_vec*fHalfThick+line_vec*(fHalfThick-WELD_SPACE*0.5);
		//
		ACAD_HATCH *pHatch=hatchList.append();
		pHatch->vertexList.append(base_pt_arr[0]);
		pHatch->vertexList.append(base_pt-norm_vec*fHalfThick);
		pHatch->vertexList.append(corner_pt1);
		hatchSet.append(pHatch);
		//
		pHatch=hatchList.append();
		pHatch->vertexList.append(corner_pt2);
		pHatch->vertexList.append(base_pt+norm_vec*fHalfThick);
		pHatch->vertexList.append(base_pt_arr[1]);
		hatchSet.append(pHatch);
	}
	else if((m_cWeldLinePos&cWeldZZ)&&(m_cWeldLinePos&cWeldZF))
	{	//�ְ������Ϊ����
		base_pt_arr[0]=base_pt-norm_vec*fHalfThick;
		base_pt_arr[1]=base_pt+norm_vec*fHalfThick;
		corner_pt1=corner_pt2=base_pt_arr[0];	//�޹յ�
		//
		ACAD_HATCH *pHatch=hatchList.append();
		f3dPoint *pPt1=pHatch->vertexList.append(base_pt_arr[0]-norm_vec*(2*fHalfThick-WELD_SPACE));
		f3dPoint *pPt2=pHatch->vertexList.append(base_pt_arr[0]);
		f3dPoint *pPt3=pHatch->vertexList.append(base_pt_arr[0]+line_vec*(2*fHalfThick-WELD_SPACE));
		//
		pHatch=hatchList.append();
		pHatch->vertexList.append(base_pt_arr[1]+norm_vec*(2*fHalfThick-WELD_SPACE));
		pHatch->vertexList.append(base_pt_arr[1]+line_vec*(2*fHalfThick-WELD_SPACE));
		pHatch->vertexList.append(base_pt_arr[1]);
	}
	else if(m_cWeldLinePos&cWeldZF)
	{	//���߸������¿ں� ������ƽ��
		base_pt_arr[0]=base_pt-norm_vec*fHalfThick;
		corner_pt1=base_pt_arr[0]+norm_vec*WELD_SPACE;
		corner_pt2=corner_pt1;
		base_pt_arr[1]=base_pt+norm_vec*fHalfThick+line_vec*(2*fHalfThick-WELD_SPACE);
		//���ְ��¿�(ƽ��)
		ACAD_HATCH *pHatch=hatchList.append();
		f3dPoint *pPt1=pHatch->vertexList.append(corner_pt2);
		f3dPoint *pPt2=pHatch->vertexList.append(base_pt_arr[1]);
		f3dPoint *pPt3=pHatch->vertexList.append(base_pt+norm_vec*fHalfThick);
		hatchSet.append(pHatch);
		//�ְ庸��
		pHatch=hatchList.append();
		pHatch->vertexList.append(*pPt1-norm_vec*fHalfThick*2);
		pHatch->vertexList.append(*pPt2-norm_vec*fHalfThick*2);
		pHatch->vertexList.append(*pPt3-norm_vec*fHalfThick*2);
	}
	else if(m_cWeldLinePos&cWeldZZ)
	{	//�����������¿ں� ������ƽ��
		base_pt_arr[1]=base_pt+norm_vec*fHalfThick;
		corner_pt1=base_pt_arr[1]-norm_vec*WELD_SPACE;
		corner_pt2=corner_pt1;
		base_pt_arr[0]=base_pt-norm_vec*fHalfThick+line_vec*(2*fHalfThick-WELD_SPACE);
		//���ְ��¿�
		ACAD_HATCH *pHatch=hatchList.append();
		f3dPoint *pPt1=pHatch->vertexList.append(corner_pt1);
		f3dPoint *pPt2=pHatch->vertexList.append(base_pt-norm_vec*fHalfThick);
		f3dPoint *pPt3=pHatch->vertexList.append(base_pt_arr[0]);
		hatchSet.append(pHatch);
		//�ְ庸��
		pHatch=hatchList.append();
		pHatch->vertexList.append(*pPt1+norm_vec*fHalfThick*2);
		pHatch->vertexList.append(*pPt2+norm_vec*fHalfThick*2);
		pHatch->vertexList.append(*pPt3+norm_vec*fHalfThick*2);
	}
	f3dLine line;
	line.pen.style=PS_SOLID;
	line.ID=0;
	line.startPt=top_pt_arr[0];
	line.endPt=base_pt_arr[0];
	edgeList.append(LineToArc(line));
	line.startPt=top_pt_arr[0];
	line.endPt=top_pt_arr[1];
	edgeList.append(LineToArc(line));
	if(corner_pt1.IsEqual(base_pt_arr[0]))
	{
		line.startPt=base_pt_arr[0];
		line.endPt=base_pt_arr[1];
		edgeList.append(LineToArc(line));
	}
	else
	{
		line.startPt=base_pt_arr[0];
		line.endPt=corner_pt1;
		edgeList.append(LineToArc(line));
		if(!corner_pt1.IsEqual(corner_pt2))
		{
			line.startPt=corner_pt1;
			line.endPt=corner_pt2;
			edgeList.append(LineToArc(line));
		}
		line.startPt=corner_pt2;
		line.endPt=base_pt_arr[1];
		edgeList.append(LineToArc(line));
	}
	line.startPt=top_pt_arr[1];
	line.endPt=base_pt_arr[1];
	edgeList.append(LineToArc(line));
	if(bAnticlockwiseNorm)
	{
		DrawWeldLineSymbol(top_pt_arr[0],base_pt_arr[0]);
		DrawWeldLineSymbol(base_pt_arr[1],top_pt_arr[1]);
	}
	else 
	{
		DrawWeldLineSymbol(top_pt_arr[1],base_pt_arr[1]);
		DrawWeldLineSymbol(base_pt_arr[0],top_pt_arr[0]);
	}
	//
	if(key_pt_for_dim)
	{	//ȡ���������ߵĵ���Ϊ��ע�� wht 16-04-05
		if(fabs(top_pt_arr[0].x)<fabs(top_pt_arr[1].x))
			key_pt_for_dim=GEPOINT(top_pt_arr[0]);
		else
			key_pt_for_dim=GEPOINT(top_pt_arr[1]);
	}
	return TRUE;
}
void CSectionAngleFootDrawing::DimFootPlateLsSpaceAndShoePlatePos(f3dPoint foot_center,GEPOINT plateDimPosArr[4])
{	
	CDrawCoordTrans dcs(cs,1.0);
	//1.ͳ����˨λ�ò�X���ꡢY����ֱ�����
	ATOM_LIST<double> dimPosList[2];
	double fHalfFootPlateLen=m_xParam.m_pFootPlate->GetLength()*0.5;
	dimPosList[0].append(-fHalfFootPlateLen);
	//plateDimPosArr[4] �ֱ��Ӧ���������ĸ���ע��
	if(fabs(plateDimPosArr[1].x-foot_center.x)>CSizeTextDim::PRECISION)
	{
		if(plateDimPosArr[1].x<foot_center.x)
		{	
			dimPosList[0].append(plateDimPosArr[1].x);
			dimPosList[0].append(foot_center.x);
		}
		else
		{
			dimPosList[0].append(foot_center.x);
			dimPosList[0].append(plateDimPosArr[1].x);
		}
	}
	else
		dimPosList[0].append(foot_center.x);
	dimPosList[0].append(fHalfFootPlateLen);
	dimPosList[1].append(-fHalfFootPlateLen);
	if(fabs(plateDimPosArr[3].y-foot_center.y)>CSizeTextDim::PRECISION)
	{
		if(plateDimPosArr[3].y<foot_center.y)
		{
			dimPosList[1].append(plateDimPosArr[3].y);
			dimPosList[1].append(foot_center.y);
		}
		else
		{
			dimPosList[1].append(foot_center.y);
			dimPosList[1].append(plateDimPosArr[3].y);
		}
	}
	else
		dimPosList[1].append(foot_center.y);
	dimPosList[1].append(fHalfFootPlateLen);
	for(CLsRef *pLsRef=m_xParam.m_pFootPlate->GetFirstLsRef();pLsRef;pLsRef=m_xParam.m_pFootPlate->GetNextLsRef())
	{
		f3dPoint ls_pos=(*pLsRef)->ucs.origin;
		ls_pos=dcs.TransToDraw(ls_pos);
		for(int i=0;i<2;i++)
		{
			double fValue=ls_pos.x;
			if(i==1)
				fValue=ls_pos.y;
			double *pValue=NULL;
			for(pValue=dimPosList[i].GetFirst();pValue;pValue=dimPosList[i].GetNext())
			{
				if(fabs(fValue-*pValue)<2)
					break;
				if(fValue<*pValue)
				{
					dimPosList[i].insert(fValue,dimPosList[i].GetCurrentIndex());
					break;
				}
			}
			if(pValue==NULL)
				dimPosList[i].append(fValue);
		}
	}
	//2.��ע��˨���
	double fOffsetDist=TransToModelLength(sys.fonts.weld.fDimTextSize);//CSizeTextDim::dimStyle.textHeight);
	for(int i=0;i<2;i++)
	{
		for(int j=1;j<dimPosList[i].GetNodeNum();j++)
		{
			double *pPrevValue=dimPosList[i].GetByIndex(j-1);
			double *pValue=dimPosList[i].GetByIndex(j);
			f3dPoint dimStart,dimEnd,dimPos;
			if(i==0)
			{	//��עX������
				dimStart.Set(*pPrevValue,-fHalfFootPlateLen-50,0);
				dimEnd.Set(*pValue,dimStart.y,0);
				dimPos.Set(0.5*(*pPrevValue+*pValue),-fHalfFootPlateLen-50-fOffsetDist,0);
			}
			else 
			{	//��עY������
				dimStart.Set(fHalfFootPlateLen+50,*pPrevValue,0);
				dimEnd.Set(dimStart.x,*pValue,0);
				dimPos.Set(fHalfFootPlateLen+50+fOffsetDist,0.5*(*pPrevValue+*pValue),0);
			}
			double fDist=*pValue-*pPrevValue;
			if(i==0)
				AppendSizeTextDim(dimStart,dimEnd,dimPos,fDist,0);
			else
				AppendSizeTextDim(dimStart,dimEnd,dimPos,fDist,0.5*Pi);
		}
	}
	//3.����ϲ�����ѥ�嶨λ�ߴ�
	f3dPoint dimStart,dimEnd,dimPos;
	//�ϲ��ע(X������)
	if(fabs(foot_center.x-plateDimPosArr[0].x)>CSizeTextDim::PRECISION)
	{
		dimStart.Set(foot_center.x,fHalfFootPlateLen+50,0);
		dimEnd.Set(plateDimPosArr[0].x,dimStart.y,0);
		dimPos=0.5*(dimStart+dimEnd);
		dimPos.y+=fOffsetDist;
		AppendSizeTextDim(dimStart,dimEnd,dimPos,fabs(foot_center.x-plateDimPosArr[0].x),0);
	}
	//����ע(Y������)
	if(fabs(foot_center.y-plateDimPosArr[2].y)>CSizeTextDim::PRECISION)
	{
		dimStart.Set(-fHalfFootPlateLen-50,foot_center.y,0);
		dimEnd.Set(dimStart.x,plateDimPosArr[2].y,0);
		dimPos=0.5*(dimStart+dimEnd);
		dimPos.x-=fOffsetDist;
		AppendSizeTextDim(dimStart,dimEnd,dimPos,fabs(foot_center.y-plateDimPosArr[2].y),0.5*Pi);
	}
}
BOOL CSectionAngleFootDrawing::CreateTopViewDrawing()
{
	CDrawCoordTrans dcs(cs,1.0);	//չ����ʽ������ת�������ݲ����ǻ��Ʊ������ڲ���ʱ����
	CAngleFootView *pAngleFootView=(CAngleFootView*)m_pSectionView;
	ANGLE_FOOT_PARTGROUP* pCurrFootParam=&m_xParam;
	if(pAngleFootView->m_cSubType=='U' && IsHoofViewDrawing())
		pCurrFootParam=&m_xDownParam;	//ţ���
	f3dPoint wingXVecArr[2],wingYVecArr[2],rotate_center;
	//1.�������ŵװ岢��������ʮ������
	f3dPoint face_norm=cs.axis_z;
	f3dPoint face_pick=m_xParam.m_pFootPlate->ucs.origin+m_xParam.m_pFootPlate->GetNormOffset()*m_xParam.m_pFootPlate->ucs.axis_z;
	if(pAngleFootView->m_cSubType=='D' && m_xParam.m_pFootPlate->ucs.axis_z.z<0)
		face_pick.z-=m_xParam.m_pFootPlate->GetThick();	//����ͼ����ȡ�װ���ƽ��
	else if(pAngleFootView->m_cSubType=='U' && m_xParam.m_pFootPlate->ucs.axis_z.z>0)
		face_pick.z+=m_xParam.m_pFootPlate->GetThick();	//����ͼ����ȡ�װ���ƽ��
	//1.1 �������ŵװ�
	CreatePartSketch(m_pSectionView->SonParts.GetValue(pCurrFootParam->m_pFootPlate->handle));
	//1.2 ����������
	f3dLine line;
	line.pen.style=PS_DASHDOTDOT;	//�㻮��������
	line.ID=0;
	//TODO:�װ��ʮ�����ı궨�㲻Ӧȡ���ģ���Ӧȡԭ��λ�ã�������ţ��塡wjh-2015.4.26
	double fHalfFootPlateLen=pCurrFootParam->m_pFootPlate->GetLength()*0.5;
	//1.2.1 ������������
	f3dPoint foot_center=face_pick;
	coord_trans(foot_center,cs,FALSE);
	//1.2.2 ������������ʮ����
	line.startPt.Set(-fHalfFootPlateLen,foot_center.y,0);
	line.endPt.Set(fHalfFootPlateLen,foot_center.y,0);
	edgeList.append(LineToArc(line));
	line.startPt.Set(foot_center.x,-fHalfFootPlateLen,0);
	line.endPt.Set(foot_center.x,fHalfFootPlateLen,0);
	edgeList.append(LineToArc(line));
	//2.���ƽǸֽ���ͼ(�Ƴ��Ǹֿ��Ͻ�)
	for(int iAngle=0;iAngle<2;iAngle++)
	{
		CLDSLineAngle *pAngle=pCurrFootParam->m_arrAnglePtr[iAngle];
		if(pAngle==NULL)
			continue;
		//�򵥿�¡�Ǹ�������Ϣ
		CLDSLineAngle localAngle;
		localAngle.size_wide=pAngle->size_wide;
		localAngle.size_thick=pAngle->size_thick;
		localAngle.SetStart(pAngle->Start());
		localAngle.SetEnd(pAngle->End());
		localAngle.set_norm_x_wing(pAngle->get_norm_x_wing());
		localAngle.set_norm_y_wing(pAngle->get_norm_y_wing());
		//2.1 �Ƴ��Ǹֿ��Ͻ�
		localAngle.RemoveKaiHeJiao();
		//2.2 ����Ǹֹؼ���
		double thick=localAngle.GetThick(),width=localAngle.GetWidth();
		f3dPoint wing_vec_x=localAngle.GetWingVecX(),wing_vec_y=localAngle.GetWingVecY();
		f3dPoint norm_x=localAngle.get_norm_x_wing(),norm_y=localAngle.get_norm_y_wing();
		f3dPoint keyPtArr[6];
		keyPtArr[5]=localAngle.Start();
		keyPtArr[0]=keyPtArr[5]+wing_vec_x*width;
		keyPtArr[1]=keyPtArr[0]-wing_vec_x*thick-norm_x*thick;
		keyPtArr[2]=keyPtArr[1]-wing_vec_x*(width-2*thick);
		keyPtArr[3]=keyPtArr[2]+wing_vec_y*(width-2*thick);
		keyPtArr[4]=keyPtArr[5]+wing_vec_y*width;
		f3dPoint line_vec=localAngle.End()-localAngle.Start();
		normalize(line_vec);
		for(int i=0;i<6;i++)
		{
			Int3dlf(keyPtArr[i],keyPtArr[i],line_vec,face_pick,face_norm);
			keyPtArr[i]=dcs.TransToDraw(keyPtArr[i]);
		}
		//2.3 ���ƽǸֽ�������
		f3dArcLine arcLine;
		line.pen.style=PS_SOLID;
		for(int i=0;i<6;i++)
		{
			line.startPt=keyPtArr[i];
			line.endPt=keyPtArr[(i+1)%6];
			edgeList.append(LineToArc(line));
		}
		//����Ǹ�X\Yֵ��װ彻�߷�����������ѥ����ת�Ƕ�
		if(iAngle==0)
		{	//�ֱ�����Ƴ����Ͻ�֮ǰ��֮��ĽǸ�X\Y֫(��װ彻��)�����췽��
			wingXVecArr[0]=keyPtArr[0]-keyPtArr[5];
			wingYVecArr[0]=keyPtArr[4]-keyPtArr[5];
			f3dPoint wing_x_pt=pAngle->Start()+pAngle->GetWingVecX()*width;
			f3dPoint wing_y_pt=pAngle->Start()+pAngle->GetWingVecY()*width;
			Int3dlf(wing_x_pt,wing_x_pt,line_vec,face_pick,face_norm);
			Int3dlf(wing_y_pt,wing_y_pt,line_vec,face_pick,face_norm);
			wing_x_pt=dcs.TransToDraw(wing_x_pt);
			wing_y_pt=dcs.TransToDraw(wing_y_pt);
			wingXVecArr[1]=wing_x_pt-keyPtArr[5];
			wingYVecArr[1]=wing_y_pt-keyPtArr[5];
			//������һ���Ǹ�ʱ�ԽǸ������װ彻��Ϊ��ת��
			rotate_center=keyPtArr[5];
		}
		else 
		{	//���������Ǹ֣�ȡ���Ǹ������װ彻�����ߵ��е�Ϊ��ת��
			rotate_center+=keyPtArr[5];
			rotate_center*=0.5;
		}
	}
	//3.����ѥ�弰�߰�
	//3.1 ��ѥ�弰�߰��Ϊ����(�ֱ�Ϊ��ǰ��ѥ�弰�丽���߰壬����ѥ�弰���߰�)
	CHashSet<CLDSGeneralPlate*> frontBackGroup,leftRightGroup;
	if(pCurrFootParam->m_pFrontShoePlate)
		frontBackGroup.SetValue(pCurrFootParam->m_pFrontShoePlate->handle,pCurrFootParam->m_pFrontShoePlate);
	if(pCurrFootParam->m_pBackShoePlate)
		frontBackGroup.SetValue(pCurrFootParam->m_pBackShoePlate->handle,pCurrFootParam->m_pBackShoePlate);
	if(pCurrFootParam->m_pShoePlate)
		leftRightGroup.SetValue(pCurrFootParam->m_pShoePlate->handle,pCurrFootParam->m_pShoePlate);
	if(pCurrFootParam->m_pRightShoePlate)
		leftRightGroup.SetValue(pCurrFootParam->m_pRightShoePlate->handle,pCurrFootParam->m_pRightShoePlate);
	for(CAngleFootView::PART_ITEM *pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
	{
		CLDSPart *pPart=TagPart(pItem);
		if( pPart==NULL||!pPart->IsPlate()||pPart==pCurrFootParam->m_pFootPlate||						//����Ϊ�ջ򹹼����Ǹְ�򹹼������ŵװ�
			frontBackGroup.GetValue(pPart->handle)||leftRightGroup.GetValue(pPart->handle)||	//�����������������
			fabs(pPart->ucs.axis_z*dcs.cs.axis_z)>0.707)										//ˮƽ���ƵĹ���
			continue;
		double dd1=0;
		if(pCurrFootParam->m_pFrontShoePlate)
			dd1=pPart->ucs.axis_z*pCurrFootParam->m_pFrontShoePlate->ucs.axis_z;
		else if(pCurrFootParam->m_pBackShoePlate)
			dd1=pPart->ucs.axis_z*pCurrFootParam->m_pBackShoePlate->ucs.axis_z;
		double dd2=pPart->ucs.axis_z*pCurrFootParam->m_pShoePlate->ucs.axis_z;
		if(fabs(dd1)>fabs(dd2))
			leftRightGroup.SetValue(pPart->handle,(CLDSGeneralPlate*)pPart);
		else 
			frontBackGroup.SetValue(pPart->handle,(CLDSGeneralPlate*)pPart);
	}
	//3.2 �������ѥ�弰�߰�(������ת�ְ�)
	//TODO:�ж�ÿ������ת���򣬹��򲻹��Ͻ�����Ӧ���ݹ��������жϣ���Ӧ���ݽǸ�ȥ�����Ͻ�֮ǰ��֮���֫���߷����жϡ�wjh-2015.4.26
	double rotate_angle=cal_angle_of_2vec(wingXVecArr[0],wingXVecArr[1]);	//�ְ���ת�Ƕ�
	f3dPoint rotat_axis_vec=wingXVecArr[1]^wingXVecArr[0];					//��ת�᷽��
	normalize(rotat_axis_vec);
	CTextFrame data_point;
	CXhPtrSet<ACAD_HATCH> hatchSet;	//��Ҫƽ����HACTCH
	GEPOINT plateDimPosArr[4];		//ѥ����(0)��(1)��(2)��(3)�ĸ��ؼ���ע��
	for(int i=0;i<2;i++)
	{	
		CHashSet<CLDSGeneralPlate*> *pSet=&frontBackGroup;
		f3dPoint axis_z;
		if(pCurrFootParam->m_pFrontShoePlate)
			axis_z=pCurrFootParam->m_pFrontShoePlate->ucs.axis_z;
		else if(pCurrFootParam->m_pBackShoePlate)
			axis_z=pCurrFootParam->m_pBackShoePlate->ucs.axis_z;
		if(i==1)
		{
			axis_z=pCurrFootParam->m_pShoePlate->ucs.axis_z;
			pSet=&leftRightGroup;
		}
		if(fabs(axis_z*pCurrFootParam->m_arrAnglePtr[0]->get_norm_x_wing())>fabs(axis_z*pCurrFootParam->m_arrAnglePtr[0]->get_norm_y_wing()))
			rotat_axis_vec=wingXVecArr[1]^wingXVecArr[0];
		else 
			rotat_axis_vec=wingYVecArr[1]^wingYVecArr[0];
		for(CLDSGeneralPlate *pPlate=pSet->GetFirst();pPlate;pPlate=pSet->GetNext())
		{
			f3dLine inters_line_arr[2];
			//����ְ���װ�Ľ���
			if(!Cal2PlatesIntersLine(pCurrFootParam->m_pFootPlate,pPlate,inters_line_arr[0],inters_line_arr[1]))
				continue;
			inters_line_arr[0].startPt=dcs.TransToDraw(inters_line_arr[0].startPt);
			inters_line_arr[0].endPt=dcs.TransToDraw(inters_line_arr[0].endPt);
			inters_line_arr[1].startPt=dcs.TransToDraw(inters_line_arr[1].startPt);
			inters_line_arr[1].endPt=dcs.TransToDraw(inters_line_arr[1].endPt);
			//��ת�ְ彻��
			//TODO:�󽻵���ת���ĵ�foot_center,������ϽǸּ���Ҳ�����⡡wjh-2015.4.26
			//TODO:��Բ����������Ҳ��Ӧ�ø�����з�����ת��ֻ�Ժ���Խӵ���б��α�����ת����Բ���Ž������Ҫ���û�ȥ�����ϽǺ���������š�
			if(fabs(rotate_angle)>EPS)
			{
				if( pPlate==pCurrFootParam->m_pShoePlate||pPlate==pCurrFootParam->m_pRightShoePlate||
					pPlate==pCurrFootParam->m_pFrontShoePlate||pPlate==pCurrFootParam->m_pBackShoePlate)
				{
					rotate_point_around_axis(inters_line_arr[0].startPt,rotate_angle,rotate_center,rotat_axis_vec);
					rotate_point_around_axis(inters_line_arr[0].endPt,rotate_angle,rotate_center,rotat_axis_vec);
					rotate_point_around_axis(inters_line_arr[1].startPt,rotate_angle,rotate_center,rotat_axis_vec);
					rotate_point_around_axis(inters_line_arr[1].endPt,rotate_angle,rotate_center,rotat_axis_vec);
				}
				else 
				{
					f3dLine center_inters_line;
					center_inters_line.startPt=0.5*(inters_line_arr[0].startPt+inters_line_arr[1].startPt);
					center_inters_line.endPt=0.5*(inters_line_arr[0].endPt+inters_line_arr[1].endPt);
					f3dPoint inters_line_vec=center_inters_line.endPt-center_inters_line.startPt;
					f3dPoint verfiy_vec=center_inters_line.endPt-rotate_center;
					if(inters_line_vec*verfiy_vec<0)
					{
						f3dPoint temp=center_inters_line.startPt;
						center_inters_line.startPt=center_inters_line.endPt;
						center_inters_line.endPt=temp;
						inters_line_vec*=-1;
					}
					rotate_point_around_axis(center_inters_line.startPt,rotate_angle,rotate_center,rotat_axis_vec);
					center_inters_line.endPt=center_inters_line.startPt+inters_line_vec;
					normalize(inters_line_vec);
					f3dPoint norm_vec(-inters_line_vec.y,inters_line_vec.x);
					inters_line_arr[0].startPt=center_inters_line.startPt+norm_vec*pPlate->GetThick()*0.5;
					inters_line_arr[0].endPt=center_inters_line.endPt+norm_vec*pPlate->GetThick()*0.5;
					inters_line_arr[1].startPt=center_inters_line.startPt-norm_vec*pPlate->GetThick()*0.5;
					inters_line_arr[1].endPt=center_inters_line.endPt-norm_vec*pPlate->GetThick()*0.5;
				}
			}
			//���Ƹְ�����
			double fHalfThick=pPlate->GetThick()*0.5;
			f3dLine inters_line;
			inters_line.startPt=(inters_line_arr[0].startPt+inters_line_arr[1].startPt)*0.5;
			inters_line.endPt=(inters_line_arr[0].endPt+inters_line_arr[1].endPt)*0.5;
			f3dLine line1,line2;
			line1.pen.style=line2.pen.style=PS_SOLID;
			GEPOINT key_dim_pt;
			if(!DrawVerticalShoePlate(pPlate,inters_line,rotate_center,hatchSet,key_dim_pt))
			{
				edgeList.append(LineToArc(inters_line_arr[0]));
				edgeList.append(LineToArc(inters_line_arr[1]));
				//��Ӻ�����
				DrawWeldLineSymbol(inters_line_arr[0].startPt,inters_line_arr[0].endPt);
				DrawWeldLineSymbol(inters_line_arr[1].endPt,inters_line_arr[1].startPt);
				/*double dd=norm_vec*f3dPoint(1,0,0);
				if((dd>0&&inters_line.startPt.y>inters_line.endPt.y)||(dd<0&&inters_line.startPt.y<inters_line.endPt.y))
				{
				DrawWeldLineSymbol(line1.endPt,line1.startPt);
				DrawWeldLineSymbol(line2.startPt,line2.endPt);
				}
				else 
				{
				DrawWeldLineSymbol(line2.endPt,line2.startPt);
				DrawWeldLineSymbol(line1.startPt,line1.endPt);
				}*/
				//
				line.startPt=inters_line_arr[0].startPt;
				line.endPt=inters_line_arr[1].startPt;
				edgeList.append(line);
				line.startPt=inters_line_arr[0].endPt;
				line.endPt=inters_line_arr[1].endPt;
				edgeList.append(line);
				if(pPlate==pCurrFootParam->m_pShoePlate)
				{
					plateDimPosArr[2]=fabs(inters_line_arr[0].startPt.y)<fabs(inters_line_arr[1].startPt.y)?
						inters_line_arr[0].startPt:inters_line_arr[1].startPt;
					plateDimPosArr[3]=fabs(inters_line_arr[0].endPt.y)<fabs(inters_line_arr[1].endPt.y)?
						inters_line_arr[0].endPt:inters_line_arr[1].endPt;
					if(inters_line_arr[0].startPt.x>inters_line_arr[0].endPt.x)
					{	//startPtΪ��,endPtΪ��
						GEPOINT temp_pt=plateDimPosArr[2];
						plateDimPosArr[2]=plateDimPosArr[3];
						plateDimPosArr[3]=temp_pt;
					}
				}
			}
			else 
			{
				if(pPlate==pCurrFootParam->m_pBackShoePlate)
					plateDimPosArr[0]=key_dim_pt;
				else if(pPlate==pCurrFootParam->m_pFrontShoePlate)
					plateDimPosArr[1]=key_dim_pt;
			}
			//��Ӹְ����ݵ�(���ѥ�塢���ѥ���עλ�������ڵװ���࣬�Ҳ�ѥ�弰ǰ��ѥ����Ϊ�Ҳ���²�Ҫ��ע��˨��⣬�����������յ��ע)
			data_point.cls_id=pPlate->GetClassTypeId();
			data_point.hPart=pPlate->handle;
			if(pPlate==pCurrFootParam->m_pShoePlate)
			{
				if(inters_line.startPt.x<inters_line.endPt.x)
					data_point.dataPos.Set(inters_line.startPt.x+3,inters_line.startPt.y);
				else
					data_point.dataPos.Set(inters_line.endPt.x+3,inters_line.endPt.y);
				data_point.dimPos.Set(data_point.dataPos.x-sys.dim_map.PartNo.fPartNoTextSize*2,data_point.dataPos.y);
			}
			else if(pPlate==pCurrFootParam->m_pBackShoePlate)
			{
				if(inters_line.startPt.y>inters_line.endPt.y)
					data_point.dataPos.Set(inters_line.startPt.x,inters_line.startPt.y-3);
				else
					data_point.dataPos.Set(inters_line.endPt.x,inters_line.endPt.y-3);
				data_point.dimPos.Set(data_point.dataPos.x,data_point.dataPos.y+sys.dim_map.PartNo.fPartNoTextSize*2);
			}
			else
			{
				data_point.dataPos=0.5*(inters_line.startPt+inters_line.endPt);
				data_point.dimPos=data_point.dataPos;
				if(i==0)
				{	//ǰ������	
					if(data_point.dimPos.y>0)
						data_point.dimPos.y+=sys.dim_map.PartNo.fPartNoTextSize*2;
					else
						data_point.dimPos.y-=sys.dim_map.PartNo.fPartNoTextSize*2;
				}
				else
				{	//���Ҳ����
					if(data_point.dimPos.x>0)
						data_point.dimPos.x+=sys.dim_map.PartNo.fPartNoTextSize*2;
					else
						data_point.dimPos.x-=sys.dim_map.PartNo.fPartNoTextSize*2;
				}
			}
			data_point.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),ZoomToModelTextSize(sys.fonts.weld.labelFrame));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
			DATAPOINT_TAG* pTag=AppendDataPoineTag();
			data_point.dwKeyTag=pTag->key_tag;
			pTag->vertexArr.append(inters_line_arr[0].startPt);
			pTag->vertexArr.append(inters_line_arr[1].startPt);
			pTag->vertexArr.append(inters_line_arr[1].endPt);
			pTag->vertexArr.append(inters_line_arr[0].endPt);
			dataPointList.append(data_point);
		}
	}
	//3.3 ����ƽ����ʶ��
	if(hatchSet.GetNodeNum()>0)
	{
		f3dPoint dim_pos(0.25*fHalfFootPlateLen,0.25*fHalfFootPlateLen,0);
		CTextOnlyDrawing textDim;
		textDim.alignType=TopLeft;
		textDim.angle=0;
		textDim.dimPos=dim_pos;
#ifdef AFX_TARG_ENU_ENGLISH
		textDim.dimText.Printf("inside weld flat");
#else
		textDim.dimText.Printf("�ڲຸƽ");
#endif
		textDim.iOrder=0;	//���ӷ������ֱ�ע
		textDim.bNeedConvertDistToText=false;
		textDim.bInXWing=false;
		pureTextDimList.append(textDim);
		for(ACAD_HATCH *pHatch=hatchSet.GetFirst();pHatch;pHatch=hatchSet.GetNext())
		{
			f3dPoint center;
			for(f3dPoint *pPt=pHatch->vertexList.GetFirst();pPt;pPt=pHatch->vertexList.GetNext())
				center+=*pPt;
			center/=3;
			line.startPt=center;
			line.endPt=dim_pos;
			edgeList.append(line);
		}
		line.startPt=dim_pos;
		line.endPt.Set(dim_pos.x+sys.dim_map.PartNo.fPartNoTextSize*strlen(textDim.dimText),dim_pos.y);
		edgeList.append(line);
	}
	//3.4 ��ע������˨�׼�༰ѥ����׽Ű������߾���
	if(plateDimPosArr[0].y<plateDimPosArr[1].y)
	{	//���¿��ܵߵ�
		GEPOINT temp_pt=plateDimPosArr[0];
		plateDimPosArr[0]=plateDimPosArr[1];
		plateDimPosArr[1]=temp_pt;
	}
	DimFootPlateLsSpaceAndShoePlatePos(foot_center,plateDimPosArr);
	return TRUE;
}

BOOL CSectionAngleFootDrawing::CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL||pSectionView->GetViewTypeId()!=CSectionView::CLS_ANGLE_FOOT_VIEW)
		return FALSE;
	int i;
	//1����ʼ��������Ϣ
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	m_pPartGroup=pPartGroup;
	//
	CAngleFootView *pAngleFootView=(CAngleFootView*)pSectionView;
	m_xParam.m_pFootPlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hFootPlate,CLS_PLATE,CLS_PARAMPLATE);
	m_xParam.m_pShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hShoePlate,CLS_PLATE,CLS_PARAMPLATE);
	m_xParam.m_pRightShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hRightShoePlate,CLS_PLATE,CLS_PARAMPLATE);
	m_xParam.m_pFrontShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hFrontShoePlate,CLS_PLATE,CLS_PARAMPLATE);
	m_xParam.m_pBackShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hBackShoePlate,CLS_PLATE,CLS_PARAMPLATE);
	for(i=0;i<4;i++)
		m_xParam.m_arrAnglePtr[i]=(CLDSLineAngle*)m_pTower->FromPartHandle(pAngleFootView->m_arrAngleHandle[i],CLS_LINEANGLE);
	if(pAngleFootView->IsHoofView())
	{	//ţ����²���Ϣ
		m_xDownParam.m_pFootPlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hFootPlate,CLS_PLATE,CLS_PARAMPLATE);
		m_xDownParam.m_pShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hDownLeftShoePlate,CLS_PLATE,CLS_PARAMPLATE);
		m_xDownParam.m_pRightShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hDownRightShoePlate,CLS_PLATE,CLS_PARAMPLATE);
		m_xDownParam.m_pFrontShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hDownFrontShoePlate,CLS_PLATE,CLS_PARAMPLATE);
		m_xDownParam.m_pBackShoePlate=(CLDSGeneralPlate*)m_pTower->FromPartHandle(pAngleFootView->m_hDownBackShoePlate,CLS_PLATE,CLS_PARAMPLATE);
		for(i=0;i<4;i++)
			m_xDownParam.m_arrAnglePtr[i]=(CLDSLineAngle*)m_pTower->FromPartHandle(pAngleFootView->m_arrDownAngleHandle[i],CLS_LINEANGLE);
	}
	_pRootPart=m_pTower->FromPartHandle(pPartGroup->handle);
	m_xEntManager.InitContext(m_pRootPart,this,STORE_ACAD);
	if( m_pRootPart==NULL||m_xParam.m_pFootPlate==NULL||m_xParam.m_pShoePlate==NULL||
		m_xParam.m_pFrontShoePlate==NULL||m_xParam.m_pBackShoePlate==NULL||m_xParam.m_arrAnglePtr[0]==NULL)
		return FALSE;
	m_pSectionView->des_cs.SetModifiedState();
	cs=m_pSectionView->UpdateUCS(pTower,pPartGroup);		//����ϵ
	CSectionView::PART_ITEM *pItem=NULL;
	for(pItem=m_pSectionView->SonParts.GetFirst();pItem;pItem=m_pSectionView->SonParts.GetNext())
		m_pSectionView->LoadTagInfo(pItem)->pCurPart=m_pTower->FromPartHandle(pItem->hSonPart);
	if(pAngleFootView->IsHoofView()&&pAngleFootView->m_cSubType=='U')
	{
		CLDSGeneralPlate* platePtrArr[3]={m_xDownParam.m_pShoePlate,m_xDownParam.m_pFrontShoePlate,m_xDownParam.m_pBackShoePlate};
		if(m_xDownParam.m_pFrontShoePlate==NULL||m_xDownParam.m_pBackShoePlate==NULL)
		{
			if(m_xDownParam.m_pFrontShoePlate==NULL)
				platePtrArr[0]=m_xDownParam.m_pBackShoePlate;
			else 
				platePtrArr[0]=m_xDownParam.m_pFrontShoePlate;
			//����ͼǰ�����ѥ��ΪNULL����ʾǰ��෽���ϵİ�Ϊͨ��
			platePtrArr[1]=m_xDownParam.m_pShoePlate;
			platePtrArr[2]=m_xDownParam.m_pRightShoePlate;
		}
		m_cWeldLinePos=CSectionStdXPlatesDrawing::InitWeldLinePos(platePtrArr,m_xDownParam.m_arrAnglePtr,&m_cKmAngle1Pos,&m_cKmAngle2Pos);
	}
	else
	{
		CLDSGeneralPlate* platePtrArr[3]={m_xParam.m_pShoePlate,m_xParam.m_pFrontShoePlate,m_xParam.m_pBackShoePlate};
		m_cWeldLinePos=CSectionStdXPlatesDrawing::InitWeldLinePos(platePtrArr,m_xParam.m_arrAnglePtr,&m_cKmAngle1Pos,&m_cKmAngle2Pos);
	}
	//2����ʼ���Խ����ͼԪ�������б������������������ߴ��ע���Ƕȱ�ע��
	if(pAngleFootView->m_cSubType=='F')
		CreateFrontViewDrawing();
	else if(pAngleFootView->m_cSubType=='S')
	{
		pAngleFootView->symbol.Copy("A");
		CreateSideViewDrawing();
	}
	else if(pAngleFootView->m_cSubType=='D')
	{
		pAngleFootView->symbol.Copy("B");
		CreateTopViewDrawing();
	}
	else if(pAngleFootView->m_cSubType=='U')
	{
		pAngleFootView->symbol.Copy("C");
		CreateTopViewDrawing();
	}
	else 
		return FALSE;
	//3������ͼ���в���
	if(m_bFinalDrawing=bFinalDrawing)
		RelayoutDrawing();
	return TRUE;
}
void CSectionAngleFootDrawing::FinalDrawing()
{
	if(!m_bFinalDrawing)
	{
		RelayoutDrawing();
		m_bFinalDrawing=true;
	}
}

//////////////////////////////////////////////////////////////////////
// TAG_INFO_PART_DRAWCMD
//////////////////////////////////////////////////////////////////////
TAG_INFO_PART_DRAWCMD_FL_FLAT* TAG_INFO_PART_DRAWCMD::FlatFL()
{
	if(TagType()==TAG_INFO_PART_DRAWCMD::PART_FL_FLAT)
		return (TAG_INFO_PART_DRAWCMD_FL_FLAT*)this;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("fail to convert structure view drawing command of flange laying flat!");
#else
		logerr.Log("����ƽ�Žṹ��ͼ��������ת��ʧ��!");
#endif
		return NULL;
	}
}
TAG_INFO_PART_DRAWCMD_TUBE_SECT* TAG_INFO_PART_DRAWCMD::TubeSect(){
	if(TagType()==TAG_INFO_PART_DRAWCMD::PART_TUBE_SECT)
		return (TAG_INFO_PART_DRAWCMD_TUBE_SECT*)this;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("fail to convert tube's cross-section view drawing command!");
#else
		logerr.Log("�ֹܽ�����ͼ��������ת��ʧ��!");
#endif
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////
// CStruDrawingCommand
//////////////////////////////////////////////////////////////////////
CStruDrawingCommand::CStruDrawingCommand(int id/*=0*/,DrawingCommandContext* pContext/*=NULL*/)
	: CXeroDrawingCommand(id,pContext)
{
	m_iDrawCmdType=CLS_BASIC_CMD;
	pTagInfo=NULL;
	m_pPart=NULL;
}
void CStruDrawingCommand::Redraw()
{
	CSectionViewDrawing* pSectionViewDrawing=(CSectionViewDrawing*)BelongDrawingComponent();
	if(pSectionViewDrawing==NULL||pSectionViewDrawing->GetSectionView()==NULL)
		return;
	CSectionView::PART_ITEM* pPartItem=pSectionViewDrawing->GetSectionView()->SonParts.GetValue(m_pPart->handle);
	pSectionViewDrawing->CreatePartSketch(pPartItem);
}
//////////////////////////////////////////////////////////////////////
// CPartGeneralDrawingCommand
//////////////////////////////////////////////////////////////////////
CPartGeneralDrawingCommand::CPartGeneralDrawingCommand(int id/*=0*/,DrawingCommandContext* pContext/*=NULL*/)
	: CStruDrawingCommand(id,pContext)
{
	m_pPart=NULL;
	m_iDrawCmdType=CStruDrawingCommand::CLS_PART_GENERAL_DRAW_CMD;
}
struct DIM_BOLT_DIAMETER{
	int hits;
	IDbDiametricDimension* pDiaDim;
};
void CFLDrawing::DrawFLStruDrawing(CLDSParamPlate* pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext)
{
	UCS_STRU cs=dcs.cs;//pDrawingView->GetCS();
	f3dLine line;
	f3dCircle circle;
	f3dArcLine arcline;
	CTextFrame datapoint;
	datapoint.InitTextFrame(sys.dim_map.PartNo.fPartNoTextSize,sys.dim_map.PartNo.fPartNoCirD,-1,sys.dim_map.PartNo.fPartNoMargin);
	GEPOINT center = pPlate->ucs.origin;
	if(pPlate->m_iParamType==TYPE_FLP||pPlate->m_iParamType==TYPE_FLD)
	{	//��׼���� ƽ������/�Ժ�����
		//	1----------0----------1
		//	|		   			  |
		//	2____             ____2
		//		3\	   	     /3
		//		  |5   	   5|			
		//		  4---------4 

		CLDSStdPartModel stdPartModel;
		if(!pPlate->GetStdPartModel(&stdPartModel))
			return;
		double B=0,C=0,D=0,H=0,N=0,S=0,R=0,H1=0;
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
			S+=stdPartModel.m_fRelateTubeThick;
		if(style.ciDrawModeH0V1==1)	//���Ʋ���ͼ
		{	//��������ͼ
			f3dPoint pre_vec = pPlate->ucs.axis_z;
			RotateVectorAroundVector(pre_vec,-Pi*0.5,cs.axis_z);//f3dPoint(0,1,0));
			f3dPoint start_firs	= pPlate->ucs.origin + 0.5*D*pre_vec;
			f3dPoint end_firs	= pPlate->ucs.origin - 0.5*D*pre_vec;
			f3dPoint start_sec	= start_firs + C*pPlate->ucs.axis_z;
			f3dPoint end_sec		= end_firs + C*pPlate->ucs.axis_z;
			f3dPoint start_thir	= start_sec - 0.5*(D-N)*pre_vec;
			f3dPoint end_thir	= end_sec + 0.5*(D-N)*pre_vec;
			f3dPoint start_four	= pPlate->ucs.origin + (0.5*B+S)*pre_vec + H*pPlate->ucs.axis_z;
			f3dPoint end_four	= pPlate->ucs.origin - (0.5*B+S)*pre_vec + H*pPlate->ucs.axis_z;
			double H2=H1;	//�Ժ�����
			if(pPlate->m_iParamType==TYPE_FLP)
				H2=H-(C+R);	//ƽ������
			f3dPoint start_fif	= start_four - H2*pPlate->ucs.axis_z;
			f3dPoint end_fif		= end_four - H2*pPlate->ucs.axis_z;
			//ת������ͼ����ϵ��
			coord_trans(start_firs, cs, FALSE);
			coord_trans(end_firs, cs, FALSE);
			coord_trans(start_sec, cs, FALSE);
			coord_trans(end_sec, cs, FALSE);
			coord_trans(start_thir, cs, FALSE);
			coord_trans(end_thir, cs, FALSE);
			coord_trans(start_four, cs, FALSE);
			coord_trans(end_four, cs, FALSE);
			coord_trans(start_fif, cs, FALSE);
			coord_trans(end_fif, cs, FALSE);
			start_firs.z=end_firs.z=start_sec.z=end_sec.z=start_thir.z=end_thir.z=start_four.z=end_four.z=start_fif.z=end_fif.z=0;
			
			//���Ʒ�������ͼ
			line.startPt=start_firs;
			line.endPt=end_firs;
			line.pen.style=style.penStyle;
			line.ID=0;	//pPlate->handle;
			pContext->AppendLine(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=start_firs;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=end_sec;
			line.endPt=end_firs;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=start_thir;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=end_sec;
			line.endPt=end_thir;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=start_thir;
			line.endPt=start_fif;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=end_thir;
			line.endPt=end_fif;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=start_fif;
			line.endPt=start_four;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=end_fif;
			line.endPt=end_four;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=start_four;
			line.endPt=end_four;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			//������Ϣ��չ��λ��
			f3dPoint dim_vec=start_firs-start_sec;
			normalize(dim_vec);
			datapoint.dataPos.Set(start_firs.x,0.5*(start_firs.y+end_firs.y));
			datapoint.dimPos=datapoint.dataPos+dim_vec*pPlate->GetThick();
			DATAPOINT_TAG* pTag=pContext->AppendDataPoineTag();
			datapoint.dwKeyTag=pTag->key_tag;
			pTag->vertexArr.append(start_firs);
			pTag->vertexArr.append(start_sec);
			pTag->vertexArr.append(start_thir);
			pTag->vertexArr.append(start_fif);
			pTag->vertexArr.append(start_four);
			pTag->vertexArr.append(end_four);
			pTag->vertexArr.append(end_fif);
			pTag->vertexArr.append(end_thir);
			pTag->vertexArr.append(end_sec);
			pTag->vertexArr.append(end_firs);
			pContext->AppendDataPoint(datapoint);
		}
		else if(style.ciDrawModeH0V1==0)	//ˮƽ����
		{	
			f3dPoint start_firs	= pPlate->ucs.origin + 0.5*B*pPlate->ucs.axis_x;
			f3dPoint start_sec	= pPlate->ucs.origin + 0.5*D*pPlate->ucs.axis_x;
			//ת������ͼ����ϵ��
			coord_trans(center, cs, FALSE);
			coord_trans(start_firs, cs, FALSE);
			coord_trans(start_sec, cs, FALSE);
			center.z = start_firs.z=start_sec.z=0;
			
			if(style.bDrawInnerWeldCircle)
			{//���ƶԺ������ԽӾ���������Բ
				arcline.CreateMethod1(center,start_firs,f3dPoint(0,0,1),2*Pi);
				arcline.pen.style=style.penStyle;
				arcline.ID=0;	//pPlate->handle;
				pContext->AppendLine(arcline);
			}
			//���ƶԺ�����������Բͼ
			arcline.CreateMethod1(center,start_sec,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=style.penStyle;
			arcline.ID=0;
			pContext->AppendLine(arcline);
			//������Ϣ��չ��λ��
			f3dPoint dim_vec,dim_pos;
			dim_vec=dim_pos=start_sec;
			normalize(dim_vec);
			if(dim_vec.IsEqual(f3dPoint(0,1,0))||dim_vec.IsEqual(f3dPoint(0,-1,0)))
				rotate_point_around_axis(dim_pos,0.25*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
			else
				dim_pos=f3dPoint(0,0,0)+f3dPoint(0,1,0)*start_sec.mod();
			dim_vec=dim_pos;
			normalize(dim_vec);
			datapoint.dataPos=dim_pos;
			datapoint.dimPos=dim_pos+dim_vec*pPlate->GetThick();
			CLDSStdPartModel stdPartModel;
			if(pPlate->GetStdPartModel(&stdPartModel))
			{
				stdPartModel.LayoutBolt();
				for(f3dPoint* pBoltPos=stdPartModel.GetFirstBoltPos();pBoltPos;pBoltPos=stdPartModel.GetNextBoltPos())
				{
					circle.centre=*pBoltPos;
					coord_trans(circle.centre,pPlate->ucs,TRUE,TRUE);
					coord_trans(circle.centre, cs, FALSE,TRUE);
					circle.centre.z=0;
					circle.radius=stdPartModel.param.fl_param.nBoltD*0.5;
					pContext->AppendCircle(circle);
				}
			}
			datapoint.cls_id=pPlate->GetClassTypeId();
			int iPartNoFrameStyle=sys.dim_map.PartNo.iPartNoFrameStyle;
			//if(pPlate->m_bStdPart&&pPlate->IsInsertPlate())
			//	iPartNoFrameStyle=sys.dim_map.PartNo.iStdInsertPlatePartNoFrameStyle;	//��׼��嵥��ָ����ſ����� wht 12-03-19
			//else if (pPlate->IsFL())
				iPartNoFrameStyle=sys.dim_map.PartNo.iFlPartNoFrameStyle;				//��������ָ����ſ����� wht 12-03-19
			datapoint.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrameFL));//NULL,0,iPartNoFrameStyle);
			datapoint.hPart=pPlate->handle;
			pContext->AppendDataPoint(datapoint);
		}
		return;
	}
#ifdef __OPEN_TEMP_CODE_
	else if(pPlate->m_iParamType==TYPE_FL||pPlate->m_iParamType==TYPE_FLG||pPlate->m_iParamType==TYPE_FLR)
	{
		double R=0,W=0;
		pPlate->GetDesignItemValue('R', &R);
		pPlate->GetDesignItemValue('W', &W);
		if(h0v1==1)	//���Ʋ���ͼ
		{	//��������ͼ
			f3dPoint pre_vec = pPlate->ucs.axis_z;
			RotateVectorAroundVector(pre_vec,-Pi*0.5,cs.axis_z);//f3dPoint(0,1,0));
			plate_thick = pPlate->GetThick();
			start_firs	= pPlate->ucs.origin + 0.5*W*pre_vec;
			end_firs	= pPlate->ucs.origin - 0.5*W*pre_vec;
			start_sec	= start_firs + plate_thick*pPlate->ucs.axis_z;
			end_sec		= end_firs + plate_thick*pPlate->ucs.axis_z;
			//ת������ͼ����ϵ��
			coord_trans(start_firs, cs, FALSE);
			coord_trans(end_firs, cs, FALSE);
			coord_trans(start_sec, cs, FALSE);
			coord_trans(end_sec, cs, FALSE);
			start_firs.z=end_firs.z=start_sec.z=end_sec.z=0;
			
			//���Ʒ�������ͼ
			line.startPt=start_firs;
			line.endPt=end_firs;
			line.pen.style=penStyle;
			line.ID=0;	//pPlate->handle;
			pContext->AppendLine(LineToArc(line));
			line.startPt=start_sec;
			line.endPt=end_sec;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=start_firs;
			line.endPt=start_sec;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			line.startPt=end_firs;
			line.endPt=end_sec;
			line.ID=0;
			pContext->AppendLine(LineToArc(line));
			//������Ϣ��չ��λ��
			f3dPoint dim_vec=start_sec-start_firs;
			normalize(dim_vec);
			f3dPoint line_vec=end_sec-start_sec;
			normalize(line_vec);
			data_point.dataPos=start_sec+line_vec*DISTANCE(start_sec,end_sec)*0.382;
			data_point.dimPos=data_point.dataPos+dim_vec*plate_thick*3;
		}
		else if(h0v1==0)	//ˮƽ����
		{	
			point = pPlate->ucs.origin;
			start_firs	= pPlate->ucs.origin + 0.5*W*pPlate->ucs.axis_x;
			start_sec	= pPlate->ucs.origin + R*pPlate->ucs.axis_x;
			//ת������ͼ����ϵ��
			coord_trans(point, cs, FALSE);
			coord_trans(start_firs, cs, FALSE);
			coord_trans(start_sec, cs, FALSE);
			point.z = start_firs.z=start_sec.z=0;
			
			//���Ʒ���ˮƽ��
			arcline.CreateMethod1(point,start_firs,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=penStyle;
			arcline.ID=0;	//pPlate->handle;
			pContext->AppendLine(arcline);
			arcline.CreateMethod1(point,start_sec,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=penStyle;
			arcline.ID=0;
			pContext->AppendLine(arcline);
			//��ע�����⾶����˨����ֱ���Լ���˨ֱ���͸��� wht 10-10-29
			//��ע����ֱ��ʱ����ײ����δ���...........................
			CSizeTextDim space_dim;
			/*space_dim.dimStartPos.Set(point.x+0.5*W,point.y);
			space_dim.dimEndPos.Set(point.x-0.5*W,point.y);
			space_dim.dist=W;
			space_dim.bAllowRelayout=FALSE;
			space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
			space_dim.dimPos.y-=W;
			space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
			space_dim.iOrder = 0;	//װ�����߳ߴ�
			space_dim.nHits=1;
			spaceDimList.append(space_dim);*/
			CLsRef *pLsRef=pPlate->GetFirstLsRef();
			CLDSBolt *pBolt=NULL;
			if(pLsRef)
				pBolt=pLsRef->GetLsPtr();
			double fBoltR=0;
			if(pBolt)
			{
				f3dPoint ls_pos=pBolt->ucs.origin;
				coord_trans(ls_pos,pPlate->ucs,FALSE);
				ls_pos.z=0;
				fBoltR=ls_pos.mod();
				//������˨����Բ
				arcline.CreateMethod1(point,f3dPoint(point.x+fBoltR,point.y),f3dPoint(0,0,1),2*Pi);
				arcline.pen.style=PS_DASH;
				arcline.ID=0;
				pContext->AppendLine(arcline);
				//��˨���ڰ뾶
				/*space_dim.dimStartPos.Set(point.x+fBoltR,point.y);
				space_dim.dimEndPos.Set(point.x-fBoltR,point.y);
				space_dim.dist=2*fBoltR;
				space_dim.bAllowRelayout=FALSE;
				space_dim.dimPos=0.5*(space_dim.dimStartPos+space_dim.dimEndPos);
				space_dim.dimPos.y-=(2*fBoltR);
				space_dim.angle = atan((space_dim.dimEndPos.y-space_dim.dimStartPos.y)/(space_dim.dimEndPos.x-space_dim.dimStartPos.x));
				space_dim.iOrder = 0;	//װ�����߳ߴ�
				space_dim.nHits=1;
				spaceDimList.append(space_dim);*/
			}
			//������Ϣ��չ��λ��
			f3dPoint dim_vec=start_firs-start_sec;
			normalize(dim_vec);
			data_point.dimPos=data_point.dataPos=start_firs+dim_vec*plate_thick;
		}
	}
#endif
	double A=360,B=0,C=0,D=0,E=0,F=0,G=0,H=0,L=0,M=0,N=0,R=0,S=0,W=0;
	//��÷����Ļ�����Ʋ���
	pPlate->GetDesignItemValue('A', &A);	//������ʷ�ļ���û��A��������AĬ��Ϊ360�� wjh-2012-9-27
	pPlate->GetDesignItemValue('R', &R);	//�����ھ�
	pPlate->GetDesignItemValue('W', &W);	//�����⾶
	pPlate->GetDesignItemValue('H', &H);
	double T=pPlate->thick;
	double RO=W*0.5;	//��Բ�뾶

	coord_trans(center,cs,FALSE,TRUE);
	center.z=0;
	ATTACHPART_ITEM *pItem=NULL;

	PEN_STRU pen;
	pen.width=0;
	pen.style=PS_SOLID;
	//���Ʒ�������Բƽ��ͼ
	double arc_angle=A*RADTODEG_COEF;
	if(A>=360)	//��Բ
	{
		//��Բ��
		IDbArcline* pInnerCir=(IDbArcline*)pContext->AppendDbEntity(IDbEntity::DbArcline);
		GEPOINT startPoint(center.x+R,center.y);
		pInnerCir->CreateMethod1(center,startPoint,f3dPoint(0,0,1),Pi+Pi);
		pInnerCir->SetPen(pen);
		//��Բ��
		IDbArcline* pOuterCir=(IDbArcline*)pContext->AppendDbEntity(IDbEntity::DbArcline);	
		startPoint.Set(center.x+RO,center.y);
		pOuterCir->CreateMethod1(center,startPoint,f3dPoint(0,0,1),Pi+Pi);
		pOuterCir->SetPen(pen);
		//��Բ������
		f3dArcLine circle;
		circle.CreateMethod1(center,pInnerCir->StartPosition(),f3dPoint(0,0,1),Pi+Pi);
		pContext->DrawWeldLineSymbol(circle);
		//
		IAttachIntegerData* pAttachDataId = (IAttachIntegerData*)pOuterCir->AppendLifeDataObject(ILifeDataObject::INTEGER);
		pAttachDataId->SetIntPara(pPlate->handle);
	}
	else
	{	//����ĳЩ��������������ܱ��ƻ�������ƺ󽫵ò�����ȷ������ 
		//������С��3ʱ��������Ƹò�������	wht 11-07-06
		if(pPlate->vertex_list.GetNodeNum()<3)
			pPlate->DesignPlate();
		BOOL bFinal=FALSE;
		IDbEntity* pEntity=NULL;
		PROFILE_VERTEX *pPrevVertex=pPlate->vertex_list.GetTail();
		for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			//TODO:�������������ʶ������δ���������Ӧ��ͨ��ILifeDataObject�����
			//if(bFinal)
			//	line.feature=pPlate->handle;
			//if(pPrevVertex->m_bWeldEdge&&bPatternDrawing&&sys.part_map.plate.bDrawSlopeCutLine)//����ͼ�����¿���
			//	DrawSlopeCutLine(*pPrevVertex,*pVertex,pPlate); 
			pen.style=PS_SOLID;
			if(pPrevVertex->type==0)
			{
				pEntity=pContext->AppendDbEntity(IDbEntity::DbLine);
				((IDbLine*)pEntity)->SetStartPosition(pPrevVertex->vertex);
				((IDbLine*)pEntity)->SetEndPosition(pVertex->vertex);
			}
			else
			{
				pEntity=pContext->AppendDbEntity(IDbEntity::DbArcline);
				f3dArcLine arcline;
				pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&pPlate->ucs);
				if(pPrevVertex->type<3)
					((IDbArcline*)pEntity)->CreateMethod1(arcline.Center(),arcline.Start(),arcline.WorkNorm(),arcline.SectorAngle());
				else
					((IDbArcline*)pEntity)->CreateEllipse(arcline.Center(),arcline.Start(),arcline.End(),arcline.ColumnNorm(),arcline.WorkNorm(),arcline.Radius());
			}
			pEntity->Offset(center);
			if(pPrevVertex->m_bWeldEdge)//&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
			{
				if(pEntity->GetDbEntType()==IDbEntity::DbLine)
					pContext->DrawWeldLineSymbol(((IDbLine*)pEntity)->StartPosition(),((IDbLine*)pEntity)->EndPosition());
				else
				{
					f3dArcLine arcline=((IDbArcline*)pEntity)->GetArcLine();
					pContext->DrawWeldLineSymbol(arcline);
				}
			}
			pPrevVertex=pVertex;
		}
	}
	//������Ϣ��չ��
	datapoint.cls_id=pPlate->GetClassTypeId();
	datapoint.hPart=pPlate->handle;
	datapoint.BuildFrame(GetPartNoIncMat(pPlate->GetPartNo(),pPlate->cMaterial),pContext->ZoomToModelTextSize(sys.fonts.weld.labelFrameFL));//NULL,0,sys.dim_map.PartNo.iPartNoFrameStyle);
	f3dPoint dim_pos(center.x+RO,center.y),dim_vec;
	dim_vec=dim_pos-f3dPoint(center);
	normalize(dim_vec);
	datapoint.dataPos=dim_pos;
	datapoint.dimPos=dim_pos+dim_vec*pPlate->GetThick();
	pContext->AppendDataPoint(datapoint);
	//��ע�����⾶
	double fDimTextSize=pContext->IsSectionDrawing()?sys.fonts.weld.fDimTextSize:CSizeTextDim::dimStyle.textHeight;
	double fDimGap=max(sys.dim_map.fDimGap,0.25*fDimTextSize); //from RxTools.cpp wjh-2018.3.7
	double fOffsetDist=pContext->TransToModelLength(fDimGap+pContext->ZoomToModelTextSize(fDimTextSize));
	f3dPoint diameter_vec,offset_vec;
	offset_vec=diameter_vec=f3dPoint(1,0,0);
	RotateVectorAroundVector(offset_vec,-Pi/2,f3dPoint(0,0,1));
	double angle=0,fDist=0;
	f3dPoint dimStart,dimEnd,dimPos;
	dimStart=f3dPoint(center)+diameter_vec*RO;
	dimEnd=f3dPoint(center)-diameter_vec*RO;
	dimPos=f3dPoint(center)+offset_vec*(RO+4.5*fOffsetDist);
	fDist=W;
	angle=atan((dimEnd.y-dimStart.y)/(dimEnd.x-dimStart.x));
	pContext->AppendSizeTextDim(CSizeTextDim(dimStart,dimEnd,dimPos,fDist,angle));
	//��ע�����ھ�
	dimStart=f3dPoint(center)+diameter_vec*R;
	dimEnd=f3dPoint(center)-diameter_vec*R;
	dimPos=f3dPoint(center)+offset_vec*(RO+1.5*fOffsetDist);
	fDist=R+R;
	angle=atan((dimEnd.y-dimStart.y)/(dimEnd.x-dimStart.x));
	pContext->AppendSizeTextDim(CSizeTextDim(dimStart,dimEnd,dimPos,fDist,angle));
	//������˨�׻�ͼ��������ע�׾�����˨Բֱ��
	double R_bolt=0,new_R=0;
	CHashList<double>hashBoltPosRadius;
	CHashList<DIM_BOLT_DIAMETER>hashBoltHitsByBoltD_10;	//��Ϊ��˨��ֱ��*10
	int nBolt=pPlate->GetLsCount();
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		f3dCircle ls_cir;
		ls_cir.centre=pLsRef->GetLsPtr()->ucs.origin;
		coord_trans(ls_cir.centre,cs,FALSE);
		ls_cir.centre.z=0;//����뾶ǰZ����Ӧ���� wht 10-10-14
		new_R=ls_cir.centre.mod();
		ls_cir.centre+=center;
		{
			IDbCircle* pCircle=(IDbCircle*)pContext->AppendDbEntity(IDbEntity::DbCircle);
			pCircle->SetCenter(ls_cir.centre);
			double hole_d=pLsRef->pBolt->get_d()+pLsRef->pBolt->hole_d_increment;
			pCircle->SetRadius(hole_d/2);
			pen.style=PS_SOLID;
			pCircle->SetPen(pen);
			DIM_BOLT_DIAMETER* pDimHits=hashBoltHitsByBoltD_10.GetValue(ftoi(hole_d*10));
			if(pDimHits)
			{
				pDimHits->hits++;
				//���ڷ�����ע�󲿷�λ���²��������˨�׾���ע��ʹ���������Ͻǵ���˨���� wjh-2017.11.21
				GEPOINT xFarChordPoint,xChordPoint,xCenter;
				pDimHits->pDiaDim->GetChordPoint(xChordPoint,false);
				pDimHits->pDiaDim->GetFarChordPoint(xFarChordPoint,false);
				xCenter=(xFarChordPoint+xChordPoint)*0.5;
				if(xCenter.x+xCenter.y<ls_cir.centre.x+ls_cir.centre.y)
				{
					GEPOINT chordPoint(ls_cir.centre.x+0.3536*hole_d,ls_cir.centre.y+0.3536*hole_d);
					GEPOINT farChordPoint(ls_cir.centre.x-0.3536*hole_d,ls_cir.centre.y-0.3536*hole_d);
					pDimHits->pDiaDim->SetChordPoint(chordPoint);
					pDimHits->pDiaDim->SetFarChordPoint(farChordPoint);
				}
			}
			else
			{
				pDimHits=hashBoltHitsByBoltD_10.Add(ftoi(hole_d*10));
				pDimHits->hits=1;
				pDimHits->pDiaDim=(IDbDiametricDimension*)pContext->AppendDbEntity(IDbEntity::DbDiametricDimension);
				f3dPoint chordPoint(ls_cir.centre.x+0.3536*hole_d,ls_cir.centre.y+0.3536*hole_d);
				f3dPoint farChordPoint(ls_cir.centre.x-0.3536*hole_d,ls_cir.centre.y-0.3536*hole_d);
				pDimHits->pDiaDim->SetChordPoint(chordPoint);
				pDimHits->pDiaDim->SetFarChordPoint(farChordPoint);
				pDimHits->pDiaDim->SetLeaderLength(5);
			}
		}
		//������˨����Բ��
		double bolt_pos_D=new_R*2;
		if(hashBoltPosRadius.GetValue(ftoi(bolt_pos_D))==NULL)
		{	//���ó�ʼ��˨�����ľ��룬��������˨����Բ��
			IDbCircle* pCircle=(IDbCircle*)pContext->AppendDbEntity(IDbEntity::DbCircle);
			pCircle->SetCenter(center);
			pCircle->SetRadius(new_R);
			pen.style=PS_DASHDOTDOT;	//�㻮��������
			pCircle->SetPen(pen);
			//��˨����Բֱ���ߴ��ע
			hashBoltPosRadius.SetValue(ftoi(bolt_pos_D),bolt_pos_D);
			dimStart=f3dPoint(center)+diameter_vec*new_R;
			dimEnd=f3dPoint(center)-diameter_vec*new_R;
			dimPos=f3dPoint(center)+offset_vec*(RO+3*fOffsetDist);
			fDist=bolt_pos_D;
			angle=atan((dimEnd.y-dimStart.y)/(dimEnd.x-dimStart.x));
			pContext->AppendSizeTextDim(CSizeTextDim(dimStart,dimEnd,dimPos,fDist,angle));
		}
	}
	for(DIM_BOLT_DIAMETER* pBoltDiaDim=hashBoltHitsByBoltD_10.GetFirst();pBoltDiaDim;pBoltDiaDim=hashBoltHitsByBoltD_10.GetNext())
	{
		CXhChar50 dimText;
		if(pBoltDiaDim->hits>1)
			dimText.Printf("%d",pBoltDiaDim->hits);
		dimText.Append("%%C");
		dimText.Append(CXhChar16(hashBoltHitsByBoltD_10.GetCursorKey()*0.1));
		pBoltDiaDim->pDiaDim->SetDimText(dimText);
	}
	//���Ʒ����ϵ���帩��ͼ
	if(pPlate->ucs.axis_z*cs.axis_z>0)
		pen.style=PS_DASH;
	else
		pen.style=PS_SOLID;
	for(ATTACHPART_ITEM* pItem=pPlate->AttachPart.GetFirst();pItem;pItem=pPlate->AttachPart.GetNext())
	{
		CSuperSmartPtr<CLDSPart> pPart=pPlate->BelongModel()->FromPartHandle(pItem->h,CLS_PARAMPLATE,CLS_PLATE);
		if(pPart.IsNULL())
			continue;
		double fNormOffset=0,half_thick=0.5*pPart->GetThick();
		if(pPart->GetClassTypeId()==CLS_PLATE)
			fNormOffset=pPart.PlatePointer()->m_fNormOffset;
		else
			fNormOffset=pPart.ParamPlatePointer()->GetNormOffset();
		f3dPoint checkOrgPoint=pPlate->ucs.origin;
		coord_trans(checkOrgPoint,pPart->ucs,FALSE,TRUE);
		f3dPoint rayVec;
		if(fabs(checkOrgPoint.z-fNormOffset-half_thick)<1)	//1mmԲ�����
		{	//��(��)���ڷ������߷�����
			rayVec=pPart->ucs.axis_z^pPlate->ucs.axis_z;
			normalize(rayVec);
			f3dPoint checkVertex;
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{
				for(PROFILE_VERTEX* pVertex=pPart.PlatePointer()->vertex_list.GetFirst();pVertex;pVertex=pPart.PlatePointer()->vertex_list.GetNext())
				{
					checkVertex=pPart.PlatePointer()->GetRealVertex(pVertex->vertex);
					coord_trans(checkVertex,pPart->ucs,TRUE,TRUE);
					f3dPoint checkVec=checkVertex-pPlate->ucs.origin;
					double justify=checkVec*pPlate->ucs.axis_z;
					if(justify>pPlate->GetNormOffset()-pPart->GetThick() && justify<pPlate->GetNormOffset()+2*pPart->GetThick())
					{	//����߽��
						if(rayVec*checkVec<0)
							rayVec*=-1.0;
						break;
					}
				}
			}
			else
			{
				if(pPart.ParamPlatePointer()->vertex_list.GetNodeNum()<0)
					pPart.ParamPlatePointer()->DesignPlate(false);
				for(PROFILE_VERTEX* pVertex=pPart.ParamPlatePointer()->vertex_list.GetFirst();pVertex;pVertex=pPart.ParamPlatePointer()->vertex_list.GetNext())
				{
					checkVertex=pVertex->vertex;
					coord_trans(checkVertex,pPart->ucs,TRUE,TRUE);
					f3dPoint checkVec=checkVertex-pPlate->ucs.origin;
					double justify=checkVec*pPlate->ucs.axis_z;
					if(justify>pPlate->GetNormOffset()-pPart->GetThick() && justify<pPlate->GetNormOffset()-pPart->GetThick())
					{	//����߽��
						if(rayVec*checkVec<0)
							rayVec*=-1.0;
						break;
					}
				}
			}
		}
		else
			continue;
		pPart->feature=1;	//���û��Ʊ��
		//ת������ͼ����ϵ��
		vector_trans(rayVec,cs,FALSE,TRUE);
		GEPOINT rayVertVec(-rayVec.y, rayVec.x);
		GEPOINT midLineStart(center.x+rayVec.x*R,center.y+rayVec.y*R);
		GEPOINT midLineEnd(center.x+rayVec.x*RO,center.y+rayVec.y*RO);
		double halfDrawThick=pContext->TransToModelLength(sys.part_group_map.fMinDrawThick*0.5);
		if(half_thick>halfDrawThick)
			halfDrawThick=half_thick;
		IDbLine* pLine=(IDbLine*)pContext->AppendDbEntity(IDbEntity::DbLine);
		pLine->SetPen(pen);
		pLine->SetStartPosition(midLineStart+rayVertVec*halfDrawThick);
		pLine->SetEndPosition(midLineEnd+rayVertVec*halfDrawThick);
		pContext->DrawWeldLineSymbol(pLine->StartPosition(),pLine->EndPosition());
		pLine=(IDbLine*)pContext->AppendDbEntity(IDbEntity::DbLine);
		pLine->SetPen(pen);
		pLine->SetStartPosition(midLineStart-rayVertVec*halfDrawThick);
		pLine->SetEndPosition(midLineEnd-rayVertVec*halfDrawThick);
		pContext->DrawWeldLineSymbol(pLine->EndPosition(),pLine->StartPosition());
		//
		IAttachIntegerData* pAttachIdPara=(IAttachIntegerData*)pLine->AppendLifeDataObject(ILifeDataObject::INTEGER);
		pAttachIdPara->SetIntPara(pPart->handle);
		//TODO:�������Ƕȱ�ע����
	}
}

void CPartGeneralDrawingCommand::Redraw()
{
	if(m_pPart==NULL)
		return;
	CSectionViewDrawing* pDrawingView=(CSectionViewDrawing*)BelongDrawingComponent();
	CSectionDrawingEntsManager manager(pDrawingView->m_pRootPart,this);
	if(m_pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate* pParamPlate=(CLDSParamPlate*)m_pPart;
		CFLDrawing drawer;
		if(TagInfo()!=NULL)
			drawer.style.bDrawInnerWeldCircle=TagInfo()->FlatFL()->bDrawInnerCircle;
		drawer.DrawFLStruDrawing(pParamPlate,CDrawCoordTrans(pDrawingView->GetCS()),&manager);
		return;
	}
	else if(m_pPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate* pPlate=(CLDSPlate*)m_pPart;
		CPlateDrawing drawer;
		drawer.style.geomcenter0_left1_right2=2;
		drawer.style.bDrawWeldLine=false;
		drawer.style.bDimBoltSpace=false;
		drawer.style.bDimRayRodAngle=false;
		drawer.style.bDrawInnerCircle=false;
		BOOL rslt=drawer.CreatePlateDrawing(pPlate,CDrawCoordTrans(pDrawingView->GetCS()),&manager);
		//RegisterPartDimDraw(pPlate,CLS_PLATE,drawer.style.ciDrawModeH0V1);
		return;
	}
	else if(m_pPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSPart* m_pRootPart=NULL;	//��ʱ���룬�Ժ������ú�ֵ��wjh-2014.8.3
		CSectionDrawingEntsManager manager(m_pRootPart,pDrawingView);
		CTubeDrawing drawer;
		if(TagInfo()!=NULL)
			drawer.style.bDrawExterCircle=TagInfo()->TubeSect()->bDrawExterCircle;
		CLDSLineTube* pTube=(CLDSLineTube*)m_pPart;
		UCS_STRU cs=pDrawingView->GetCS();
		drawer.CreateFlatLayingDrawing(pTube,CDrawCoordTrans(cs),&manager);
		//DrawTubeStruDrawing((CLDSLineTube*)m_pPart,dcs,&manager,0);
	}
	else
		return;
}
//////////////////////////////////////////////////////////////////////
// CPartUprightDrawingCommand
//////////////////////////////////////////////////////////////////////
CPartUprightDrawingCommand::CPartUprightDrawingCommand(int id/*=0*/,DrawingCommandContext* pContext/*=NULL*/)
	: CStruDrawingCommand(id,pContext)
{
	m_pPart=NULL;
	m_iDrawCmdType=CStruDrawingCommand::CLS_PART_UPRIGHT_DRAW_CMD;
}
void CPartUprightDrawingCommand::DrawTubeStruDrawing()
{
	CSectionDrawingEntsManager manager(m_pPart,this);//,STORE_ACAD);
	CTubeDrawing drawer;
	if(TagInfo()!=NULL)
		drawer.style.bDrawExterCircle=TagInfo()->TubeSect()->bDrawExterCircle;
	CSectionViewDrawing* pDrawingView=(CSectionViewDrawing*)BelongDrawingComponent();
	CLDSLineTube* pTube=(CLDSLineTube*)m_pPart;
	UCS_STRU cs=pDrawingView->GetCS();
	drawer.CreateTubeDrawing(pTube,CDrawCoordTrans(cs),&manager);
	return;
	//���´�������ֲ��CTubeDrawing::CreateUprightLayingDrawing()�У�����Ӧȥ�� wjh-2016.8.11
	
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
	if(radius*(1-justify)<pDrawingView->TransToModelLength(1))
		justify=1.0;	//(��)Բ��������ʱ������1mm���ɰ���׼Բ�򻯻���
	if(pDrawingView->TransToModelLength(0.5)>pTube->GetThick())		//�ֹܱں�Ӧ��֤���Ƽ�϶��С��1mm;
		drawThick=pDrawingView->TransToModelLength(0.5);
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
	if(justify==1)	//��׼Բ����
	{
		IDbArcline* pArcline=(IDbArcline*)AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ��ڱڽ���Բ
		pArcline->CreateMethod1(center,center+f3dPoint(radius-drawThick),f3dPoint(0,0,1),Pi+Pi);
		pArcline->SetPen(penStyle);
		if(TagInfo()==NULL || TagInfo()->TubeSect()->bDrawExterCircle)
		{
			pArcline=(IDbArcline*)AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ���ڽ���Բ
			pArcline->CreateMethod1(center,center+f3dPoint(radius),f3dPoint(0,0,1),Pi+Pi);
			pArcline->SetPen(penStyle);
		}
	}
	else
	{	//��б��̫�󣬱��밴��ʵ����Բ�������
		IDbArcline* pArcline=(IDbArcline*)AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ���ڽ�����Բ
		pArcline->CreateMethod1(center,center+cs.axis_x*radius,f3dPoint(0,0,1),Pi+Pi);
		pArcline->SetPen(penStyle);
		pArcline=(IDbArcline*)AppendDbEntity(IDbEntity::DbArcline);	//���Ƹֹ��ڱڽ�����Բ
		pArcline->CreateMethod1(center,center+cs.axis_x*(radius-drawThick),f3dPoint(0,0,1),Pi+Pi);
		pArcline->SetPen(penStyle);
	}
	//ת������ͼ����ϵ��
	f3dPoint len_vec=pTube->ucs.axis_z;
	vector_trans(len_vec,cs,FALSE,TRUE);				//��ͼ����ϵ�µĸֹ����߷���
	{	//���Ƹֹܻ���Ƕ�λ��׼�߼�ͷ��Ƿ���
		GEPOINT datum_edgevec=pTube->ucs.axis_x;	//�ֹ�ʵ�ʼӹ�ʱ�ĽǶȻ�׼��(һ��Ϊ�����ߣ����޺��������Ըֹܻ�׼��Ϊ��׼)
		GEPOINT datum_edgevec_outer=datum_edgevec*(radius-pTube->size_thick);
		//����<����->��׼��>�ظֹ��������ͼƽ���ͶӰʸ��
		Int3dlf(datum_edgevec_outer,datum_edgevec_outer,pTube->ucs.axis_z,GEPOINT(0,0,0),pTube->ucs.axis_z);
		//datum_edgevec_outer.Set(datum_edgevec_outer.x-len_vec.x*datum_edgevec_outer.z/len_vec.z,datum_edgevec_outer.y-len_vec.y*datum_edgevec_outer.z/len_vec.z);
		vector_trans(datum_edgevec_outer,cs,FALSE,TRUE);	//��ͼ����ϵ�µĸֹ�<����->��ں���>������������ʸ��
		GEPOINT datum_chord_point=center+datum_edgevec_outer;
		double extend_len=pDrawingView->TransToModelLength(25);	//��׼������X��Yʵ�ʻ��Ƴ���Ӧ��Բ������5mm
		double arrowHeight=pDrawingView->TransToModelLength(2);
		GEPOINT arrowStdVec=datum_edgevec_outer;
		normalize(arrowStdVec);
		IDbLine* pLine=(IDbLine*)this->AppendDbEntity(IDbEntity::DbLine);
		GEPOINT lineStart=center-(datum_edgevec_outer+arrowStdVec*(extend_len*0.2));
		GEPOINT lineEnd  =center+(datum_edgevec_outer+arrowStdVec*(extend_len*0.2));
		pLine->SetStartPosition(lineStart);
		pLine->SetEndPosition(lineEnd);
		penStyle.style=PS_DASHDOTDOT;
		pLine->SetPen(penStyle);

		IDbPolyline* pPolyline=(IDbPolyline*)AppendDbEntity(IDbEntity::DbPolyline);
		penStyle.style=PS_SOLID;
		pPolyline->SetPen(penStyle);
		GEPOINT arrowHeadTip=datum_chord_point;
		GEPOINT arrowRoot=arrowHeadTip-arrowStdVec*arrowHeight;
		GEPOINT arrowStartTip=arrowHeadTip-arrowStdVec*(arrowHeight*2);//extend_len;
		pPolyline->AddVertexAt(0,arrowStartTip,0,0);
		pPolyline->AddVertexAt(1,arrowRoot,arrowHeight*0.5,0);
		pPolyline->AddVertexAt(2,arrowHeadTip);
		//��ע��׼�߱�ʶ�ı�
		/** Ŀǰ��Ϊ�麸ͼӦ������ı�עΪ׼���ʡ���׼�ߡ����ֱ�ע�Ǳ�Ҫ��ע��wjh-2016.8.11
		CTextOnlyDrawing textDraing;
		textDraing.angle=CalRotateAngle(f3dPoint(0,0,1),f3dPoint(1,0,0),arrowStdVec);
		textDraing.dimPos=arrowStartTip;
		double angle=ReviseTextAngleToHeadUpward(textDraing.angle,true);
		if(angle!=textDraing.angle)
		{
			textDraing.angle=angle;
			textDraing.alignType=MiddleRight;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		textDraing.dimText="welding line";
#else
		textDraing.dimText="��׼��";
#endif
		textDraing.iOrder=0;
		textDraing.bNeedConvertDistToText=false;
		textDraing.bInXWing=false;
		pDrawingView->GetPureTextDimList()->append(textDraing);
		*/
		/*
		IDbRadialDimension* pDatumEdgeArrow=(IDbRadialDimension*)AppendDbEntity(IDbEntity::DbRadialDimension);
		penStyle.style=PS_SOLID;
		pDatumEdgeArrow->SetPen(penStyle);
		pDatumEdgeArrow->SetDimText("��׼��");
		pDatumEdgeArrow->SetCenterPoint(center);
		pDatumEdgeArrow->SetChordPoint(datum_chord_point);
		*/
	}
	f3dPoint datum_axis_x=pTube->ucs.axis_x;	//�ֹ�ʵ�ʼӹ�ʱ�ĽǶȻ�׼��(һ��Ϊ�����ߣ����޺��������Ըֹܻ�׼��Ϊ��׼)
	if(pTube->m_bHasWeldRoad)
	{	//�ֹ��к�����ʱ�Ժ�����Ϊ��ת�ǶȻ�׼��
		double weldRoadAngle=pTube->CalWeldRoadAngle();
		RotateVectorAroundVector(datum_axis_x,weldRoadAngle,pTube->ucs.axis_z);
	}
	f3dPoint datum_line_vec_outer=datum_axis_x*radius;
	f3dPoint datum_line_vec_inner=datum_axis_x*(radius-drawThick);
	vector_trans(datum_line_vec_outer,cs,FALSE,TRUE);	//��ͼ����ϵ�µĸֹ�<����->��ں���>������������ʸ��
	vector_trans(datum_line_vec_inner,cs,FALSE,TRUE);	//��ͼ����ϵ�µĸֹ�<����->�ڱں���>������������ʸ��
	//����<����->����>�ظֹ��������ͼƽ���ͶӰʸ��
	datum_line_vec_outer.Set(datum_line_vec_outer.x-len_vec.x*datum_line_vec_outer.z/len_vec.z,datum_line_vec_outer.y-len_vec.y*datum_line_vec_outer.z/len_vec.z);
	datum_line_vec_inner.Set(datum_line_vec_inner.x-len_vec.x*datum_line_vec_inner.z/len_vec.z,datum_line_vec_inner.y-len_vec.y*datum_line_vec_inner.z/len_vec.z);

	double axis_len=datum_line_vec_outer.mod();
	f3dPoint datum_line_vec=datum_line_vec_outer/axis_len;		//��ȡ<����->����>�ĵ�λ����������
	double extend_len=pDrawingView->TransToModelLength(5);	//��׼������X��Yʵ�ʻ��Ƴ���Ӧ��Բ������5mm
	//������ת�Ƕ���ʼ��׼�ߣ������ߣ�
	IDbLine* pLine=(IDbLine*)this->AppendDbEntity(IDbEntity::DbLine);
	GEPOINT lineStart=center-datum_line_vec_outer;
	lineStart-=datum_line_vec*extend_len;
	GEPOINT lineEnd  =center+datum_line_vec_outer;
	lineEnd+=datum_line_vec*extend_len;
	pLine->SetStartPosition(lineStart);
	pLine->SetEndPosition(lineEnd);
	penStyle.style=PS_DASHDOTDOT;
	pLine->SetPen(penStyle);
	/* Ϊ���麸ͼ������Ŀǰ��Ϊ����Ҫ��ͼԪ���ɴ����Ƴ� wjh-2016.6.20
	//������ת�Ƕ���ʼ��׼�ߣ������ߣ����Ӧ�Ĵ�ֱ������
	f3dPoint axis_y(-datum_line_vec.y,datum_line_vec.x);
	pLine=(IDbLine*)this->AppendDbEntity(IDbEntity::DbLine);
	pLine->SetStartPosition(center-axis_y*(axis_len+extend_len));
	pLine->SetEndPosition(center+axis_y*(axis_len+extend_len));
	pLine->SetPen(penStyle);
	*/
	//���ƺ��������Ǳ�Ƿ���
	if(pTube->m_bHasWeldRoad)	//Ϊ�˷���ʶ��ֹܻ���ǶȻ�׼���޺���Ҳ���ƻ�׼�߱��
	{	//�ֹ��к�����ʱͨ�������߻��ƺ�����
		IDbPolyline* pPolyline=(IDbPolyline*)AppendDbEntity(IDbEntity::DbPolyline);
		penStyle.style=PS_SOLID;
		pPolyline->SetPen(penStyle);
		double radius=pTube->GetWidth()*0.5;
		f3dPoint weldSymbolStart=center+datum_line_vec_inner;
		f3dPoint weldSymbolEnd=center+datum_line_vec_outer;
		pPolyline->AddVertexAt(0,weldSymbolStart,drawThick*0.2,drawThick*1.6);
		pPolyline->AddVertexAt(1,weldSymbolEnd,0,0);
		/* Ϊ���麸ͼ������Ŀǰ��Ϊ����Ҫ��ͼԪ���ɴ����Ƴ� wjh-2016.6.20
		if(pTube->m_bHasWeldRoad)
		{
			CTextOnlyDrawing textDraing;
			textDraing.angle=CalRotateAngle(f3dPoint(0,0,1),f3dPoint(1,0,0),datum_line_vec);
			textDraing.dimPos=weldSymbolEnd+datum_line_vec*pDrawingView->TransToModelLength(2);
			double angle=ReviseTextAngleToHeadUpward(textDraing.angle,true);
			if(angle!=textDraing.angle)
			{
				textDraing.angle=angle;
				textDraing.alignType=MiddleRight;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			textDraing.dimText="welding line";
#else
			textDraing.dimText="������";
#endif
			textDraing.iOrder=0;
			textDraing.bNeedConvertDistToText=false;
			textDraing.bInXWing=false;
			pDrawingView->GetPureTextDimList()->append(textDraing);
		}*/
	}
}
void CPartUprightDrawingCommand::Redraw()
{
	if(m_pPart==NULL)
		return;
	if(m_pPart->GetClassTypeId()==CLS_LINETUBE)
	{
		DrawTubeStruDrawing();
		return;
	}
	else
		return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CPartGroupBOMDrawing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPartGroupTblDrawing::CPartGroupTblDrawing(int id/*=0*/,IDrawing* pParentDrawing/*=NULL*/)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionSlaveDrawing::CLS_BOM_VIEW;
	m_nTableType=1;
}
CPartGroupTblDrawing::~CPartGroupTblDrawing()
{

}
void CPartGroupTblDrawing::CreatePartGroupTable(double scale)
{
	if(m_pPartGroup==NULL)
		return;
	if(m_nTableType==CPartGroupTblDrawing::PARTGROUP_BOM)
		CreatePartGroupBom(scale);
	else if(m_nTableType==CPartGroupTblDrawing::SPHEREINTERS_TBL)
		CreateSphereIntersTbl(scale);
}
//�����麸����ϸ������SegMap.cpp���ƶ���
//��Ű���������
static int comparePartStatRecord(const CPartStatRecord &rec1,const CPartStatRecord &rec2)
{
	return ComparePartNoString(rec1.sPartNo,rec2.sPartNo,"H");
	/*
	if(strlen(rec1.sPartNo)>strlen(rec2.sPartNo))
		return 1;
	else if(strlen(rec1.sPartNo)<strlen(rec2.sPartNo))
		return -1;
	else
		return stricmp(rec1.sPartNo,rec2.sPartNo);
	*/
}
static int FindPartByPartNo(const char* sPartNo,DBOBJSET* pFindedObjSet = NULL)
{
	if (strlen(sPartNo) <= 0)
		return 0;
	ATOM_LIST<long> handleList;
	for (CLDSPart* pPart = Ta.Parts.GetFirst(); pPart; pPart = Ta.Parts.GetNext())
	{
		if (pPart->m_bVirtualPart)
			continue;
		if (stricmp(pPart->GetPartNo(), sPartNo) != 0)
			continue;
		if (pFindedObjSet)
			pFindedObjSet->append(pPart);
		handleList.append(pPart->handle);
	}
	return handleList.GetNodeNum();
}
double CPartGroupTblDrawing::GetTblWidth(double zoomscale/*=1.0*/)
{
	//���ñ������߶�
	double fZoomFontCoef=ZoomToModelTextSize(1.0)*zoomscale;
	//����������ָ�Ϊsys.fonts.weld.fNumericTextSize;
	double fDimTextSize=sys.fonts.weld.fNumericTextSize*fZoomFontCoef;

	//��ʱȫ�����¼�����п�ȣ��ԸĽ��麸ͼͼ��Ч�� wjh-2018.7.31
	double fNumTextWidth=fDimTextSize*0.7;
	double fGPartNoW=6*fNumTextWidth;	//9�麸�������
	double fPartNoW=12*fNumTextWidth;	//15���������������
	double fGuiGeW =15*fNumTextWidth;	//27�������
	double fLenW=6*fNumTextWidth;		//15��������
	double fNumW=5*fNumTextWidth;		//6��������
	double fWeightW=8*fNumTextWidth;		//15��������
	double fGNumW=5*fNumTextWidth;		//9�麸����������
	double fNoteW=(fNumTextWidth+18);		//20��ע����
	double fOtherW=(fNumTextWidth+2);		//4��������

	//�������������п��Լ������
	double sumwidth=0;
	if(sys.part_group_map.part_table.bDispPartGroupNo)
		sumwidth+=fGPartNoW;
	if(sys.part_group_map.part_table.bDispPartNo)
		sumwidth+=fPartNoW;	//�������������
	if(sys.part_group_map.part_table.bDispPartGuiGe)
		sumwidth+=fGuiGeW;	//�����//ԭΪ25������--Q345��Щ���Ų���,�ʷŴ�Ϊ27 WJH--01.11.01
	if(sys.part_group_map.part_table.bDispPartLen)
		sumwidth+=fLenW;	//������
	if(sys.part_group_map.part_table.bDispPartNum)
		sumwidth+=fNumW;	//������
	if(sys.part_group_map.part_table.bDispPartWeight)
		sumwidth+=fWeightW*2;	//����֮"һ��&С��"��
	if(sys.part_group_map.part_table.bDispNotes)
		sumwidth+=fNoteW;	//��ע��
	if(sys.part_group_map.part_table.bDispPartGroupNum)
		sumwidth+=fGNumW;	//��Ϲ�����������
	return sumwidth;
}
void CPartGroupTblDrawing::CreatePartGroupBom(double scale)
{
	if(m_pPartGroup==NULL)
		return;
	PARTSET partset;
	Ta.GetPartGroupPartSet(m_pPartGroup->handle,partset);
	CLDSPart *pRootPart=Ta.FromPartHandle(m_pPartGroup->handle);
	if(pRootPart)
		partset.insert(pRootPart,0);
	CPartStatRecord *pRec=NULL;
	ATOM_LIST<CPartStatRecord>matStat;
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		if(strlen(pPart->GetPartNo())==0)
			continue;	//�ޱ�Ź�����ȥ��ͳ��
		else if(pPart->m_bVirtualPart)
			continue;	//���⸨������
		for(pRec=matStat.GetFirst();pRec;pRec=matStat.GetNext())
		{
			if(stricmp(pRec->sPartNo,GetPartNoIncMat(pPart->GetPartNo(),pPart->cMaterial))==0)
			{
				pRec->num+=pPart->m_uStatMatCoef;
				break;
			}
		}
		if(pRec==NULL)
		{
			pRec=matStat.append();
			strcpy(pRec->sPartNo,GetPartNoIncMat(pPart->GetPartNo(),pPart->cMaterial));
			pRec->cMaterial = pPart->cMaterial;
			pRec->ciQualityLevel = pPart->m_cQualityLevel;
			pRec->num+=pPart->m_uStatMatCoef;
			pRec->numPerTower = FindPartByPartNo(pPart->GetPartNo());
			pRec->idClassType=pPart->GetClassTypeId();
			pPart->GetSpecification(pRec->spec);
			pRec->width=pPart->GetWidth();
			pRec->length=pPart->GetLength();
			pRec->piece_weight=pPart->GetWeight();
			strcpy(pRec->sNotes,pPart->sNotes);
			if(gxDrwLocalizer.ciCustomerSerial==10)
				strcat(pRec->sNotes," �纸");	//�ൺ���ͨ�麸����ϸ��ע������ʾ���纸�� wht 18-09-27
		}
	}
	int i=0,nRecordN=0;
	CPartStatRecord *recArr=new CPartStatRecord[matStat.GetNodeNum()];
	for(pRec=matStat.GetFirst(),i=0;pRec;pRec=matStat.GetNext(),i++)
		recArr[i]=*pRec;
	CHeapSort<CPartStatRecord>::HeapSort(recArr,matStat.GetNodeNum(),comparePartStatRecord);
	nRecordN = matStat.GetNodeNum();

	//-------���ϻ��ܱ�-----------
	//��������
	int nRowsN = nRecordN+2;//��������(��¼��+������+�ܼ���)
	if(sys.part_group_map.part_table.bDispPartWeight
		||sys.part_group_map.part_table.bDispPartGroupNo
		||sys.part_group_map.part_table.bDispPartGroupNum)
		nRowsN+=1;	//�������зֳ�����
	int nColumnsN=0,iCurCol=0;	//��������
	//ͳ������
	if(sys.part_group_map.part_table.bDispPartGroupNo)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispPartNo)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispPartGuiGe)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispPartLen)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispPartNum)
		nColumnsN++;
	LOCALE_PROPITEM* pStampLabelItem = gxDrwLocalizer.GetLocaleItem("WeldBOM.AddPartStampLabel");
	if(pStampLabelItem)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispPartWeight)
		nColumnsN+=2;
	if(sys.part_group_map.part_table.bDispNotes)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispPartGroupNum)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispInternal)
		nColumnsN++;
	if(sys.part_group_map.part_table.bDispAppearance)
		nColumnsN++;
	m_xPartTbl.Create(nRowsN,nColumnsN);
	//���ñ������߶�
	double fTblTextH=sys.fonts.weld.fNumericTextSize;	//����������ָ�
	double fZoomFontCoef=ZoomToModelTextSize(1.0)*scale;
	double fDimTextSize=fTblTextH*fZoomFontCoef;
	for(int m=0;m<nRowsN;m++)
	{
		for(int n=0;n<nColumnsN;n++)
			m_xPartTbl.GetGridAt(m,n)->textsize=fDimTextSize;
	}
	//�������������и�
	for(i=0;i<nRowsN;i++)			//������
		m_xPartTbl.SetRowHigh(i,(fTblTextH+2)*fZoomFontCoef);
	//�������������п��Լ������
	//�����Ź�˵���ߴ�ɷ�̶��м���Ϊ4����ȴ���������Ϊ15��6��4��4 ���������Ǻ��켶�������Լ����� wjh-2013.12.06
	double fGPartNoW=(fTblTextH+7)*fZoomFontCoef;	//9�麸�������
	double fPartNoW=(fTblTextH+13)*fZoomFontCoef;	//15���������������
	double fGuiGeW=(fTblTextH+25)*fZoomFontCoef;	//27�������
	double fLenW=(fTblTextH+13)*fZoomFontCoef;		//15��������
	double fNumW=(fTblTextH+4)*fZoomFontCoef;		//6��������
	double fStampLabelW = pStampLabelItem==NULL?0:(fTblTextH+48)*fZoomFontCoef;
	double fWeightW=(fTblTextH+13)*fZoomFontCoef;	//15��������
	double fNoteW=(fTblTextH+18)*fZoomFontCoef;		//20��ע����
	double fGNumW=(fTblTextH+7)*fZoomFontCoef;		//9�麸����������
	double fOtherW=(fTblTextH+2)*fZoomFontCoef;		//4��������
	if(true)	//CLDSPart::CustomerSerial==7)
	{	//��ʱȫ�����¼�����п�ȣ��ԸĽ��麸ͼͼ��Ч�� wjh-2018.7.31
		double fNumTextWidth=fTblTextH*0.7;
		fGPartNoW=5*fNumTextWidth*fZoomFontCoef;	//9�麸�������
		fPartNoW=9*fNumTextWidth*fZoomFontCoef;		//15���������������
		fGuiGeW =13*fNumTextWidth*fZoomFontCoef;	//27�������
		fLenW=5*fNumTextWidth*fZoomFontCoef;		//15��������
		fNumW=4*fNumTextWidth*fZoomFontCoef;		//6��������
		if(pStampLabelItem!=NULL)
			fStampLabelW = 25 * fNumTextWidth*fZoomFontCoef;
		fWeightW=6*fNumTextWidth*fZoomFontCoef;		//15��������
		//fNoteW=(fTblTextH+18)*fZoomFontCoef;		//20��ע����
		fGNumW=5*fNumTextWidth*fZoomFontCoef;		//9�麸����������
		//fOtherW=(fTblTextH+2)*fZoomFontCoef;		//4��������
	}
	if(gxDrwLocalizer.ciCustomerSerial==10)
		fNoteW=fLenW;	//�ൺ���ͨ��ע�����̣��ұ�ע�纸���� wht 18-09-27
	if(sys.part_group_map.part_table.bDispPartGroupNo)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fGPartNoW);	//�麸�����
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartNo)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fPartNoW);	//�������������
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartGuiGe)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fGuiGeW);	//�����//ԭΪ25������--Q345��Щ���Ų���,�ʷŴ�Ϊ27 WJH--01.11.01
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartLen)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fLenW);	//������
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartNum)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fNumW);	//������
		iCurCol++;
	}
	if(fStampLabelW>0)
	{	//��ӡ��
		m_xPartTbl.SetColumnWide(iCurCol, fStampLabelW);
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartWeight)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fWeightW);	//����֮һ����
		iCurCol++;
		m_xPartTbl.SetColumnWide(iCurCol,fWeightW);	//����֮С����
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispNotes)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fNoteW);	//��ע��
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartGroupNum)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fGNumW);	//��Ϲ�����������
		iCurCol++;
	};
	//�����Ź�˵���ߴ�ɷ�̶��м���Ϊ4����ȴ���������Ϊ15��6��4��4 ���������Ǻ��켶�������Լ����� wjh-2013.12.06
	if(sys.part_group_map.part_table.bDispInternal)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fOtherW);	//���켶������
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispAppearance)
	{
		m_xPartTbl.SetColumnWide(iCurCol,fOtherW);	//���켶�����
		iCurCol++;
	}
	//���Ʊ��
	//����е�����ȱʡ�����½Ƕ��뼴feature = 0
	//feature��λ����ʾ�����ֵĶ��뷽ʽ0:������ 1��ʾ������ 2��ʾ���½�
	//ʮλ��ʾ������д����0:������1:���ϵ���
	//����--����������ű��Ԫ
	iCurCol=0;
	int iStartRow=1,iEndRow=nRowsN-1;	//��ʼ�н�����
	BOOL bNeedVMerger=FALSE; //��Ҫ��ֱ�ϲ���Ԫ��
	if(sys.part_group_map.part_table.bDispPartWeight
		||sys.part_group_map.part_table.bDispPartGroupNo
		||sys.part_group_map.part_table.bDispPartGroupNum|| fStampLabelW > 0)
	{
		iStartRow=2;
		bNeedVMerger=TRUE;
	}
	for(i=iStartRow+1;i<nRowsN;i++)//��ֱ�ϲ������Ϲ������һ�к��Ҳ���Ϲ�������һ��
	{
		if(sys.part_group_map.part_table.bDispPartGroupNo)
			m_xPartTbl.GetGridAt(i,0)->m_bVGridVirtual = TRUE;
		if(sys.part_group_map.part_table.bDispPartGroupNum)
			m_xPartTbl.GetGridAt(i,nColumnsN-1)->m_bVGridVirtual = TRUE;
	}
	if(sys.part_group_map.part_table.bDispPartGroupNo)
	{
		//����--��Ϲ�����ű��Ԫ
		m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("group weld \\P number");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("�麸\\P���");
#endif
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;	//����Գ�MC
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartNo)
	{
		if(bNeedVMerger)
		{
			m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("part\\P number");
		}
		else
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("number");
#else
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("����\\P���");
		}
		else
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("���");
#endif
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;	//����Գ�MC
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartGuiGe)
	{
		if(bNeedVMerger)
			m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("spec");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("��    ��");
#endif
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartLen)
	{
		if(bNeedVMerger)
		{
			m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("length \\P(mm)");
		}
		else
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("length");
#else
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("��  ��\\P(mm)");
		}
		else
			m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("��  ��");
#endif
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartNum)
	{
		if(bNeedVMerger)
			m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("amount");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("����");
#endif
		iCurCol++;
	}
	if (fStampLabelW > 0)
	{	//��ӡ��
		if (bNeedVMerger)
			m_xPartTbl.GetGridAt(1, iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0, iCurCol)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0, iCurCol)->data.SetGridString("STAMP LABEL");
#else
		m_xPartTbl.GetGridAt(0, iCurCol)->data.SetGridString("��ӡ��");
#endif
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartWeight)
	{
		m_xPartTbl.GetGridAt(0,iCurCol+1)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("weight(kg)");
		//weight of one part table unit
		m_xPartTbl.GetGridAt(1,iCurCol)->feature = 1;
		m_xPartTbl.GetGridAt(1,iCurCol)->data.SetGridString("one");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("��  ��(kg)");
		//����֮һ�����Ԫ
		m_xPartTbl.GetGridAt(1,iCurCol)->feature = 1;
		m_xPartTbl.GetGridAt(1,iCurCol)->data.SetGridString("һ ��");
#endif
		iCurCol++;
		//����֮С�Ʊ��Ԫ
		m_xPartTbl.GetGridAt(1,iCurCol)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(1,iCurCol)->data.SetGridString("subtotal");
#else
		m_xPartTbl.GetGridAt(1,iCurCol)->data.SetGridString("С ��");
#endif
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispNotes)
	{
		if(bNeedVMerger)
			m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "notes");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "��    ע");
#endif
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispPartGroupNum)
	{
		m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "group weld \\P amount");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "�麸\\P����");
#endif
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispInternal)
	{
		//xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1; //�����������
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "inner");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "����");
#endif
		iCurCol++;
	}
	if(sys.part_group_map.part_table.bDispAppearance)
	{
		//xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
		m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1; //�����������
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "appearance");
#else
		m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString( "���");
#endif
		iCurCol++;
	}
	//����--������
	CXhChar100 tem_str;
	//��ϱ����
	if(sys.part_group_map.part_table.bDispPartGroupNo)
	{//�麸����в���ʾ�����ַ� wjh-2013.5.20
		CXhChar50 szWeldLabel=m_pPartGroup->sGroupName;
		LOCALE_PROPITEM* pWeldLabelPrefix = gxDrwLocalizer.GetLocaleItem("WeldLabelPrefix");
		if (pWeldLabelPrefix)
			szWeldLabel.InsertBefore(pWeldLabelPrefix->szStrBuff,0);
		m_xPartTbl.GetGridAt(iStartRow,0)->data.SetGridString(szWeldLabel);
		m_xPartTbl.GetGridAt(iStartRow,0)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		m_xPartTbl.GetGridAt(iStartRow,0)->text_angle=Pi/2;
	}
	for(i=iStartRow;i<iEndRow;i++)
	{
		if(sys.part_group_map.part_table.bDispPartGroupNo)
			iCurCol=1;
		else
			iCurCol=0;
		if(sys.part_group_map.part_table.bDispPartNo)
		{	//���������
			tem_str.Copy(recArr[i-iStartRow].sPartNo);
			m_xPartTbl.GetGridAt(i,iCurCol)->data.SetGridString(tem_str);
			m_xPartTbl.GetGridAt(i,iCurCol)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
			iCurCol++;
		}
		if(sys.part_group_map.part_table.bDispPartGuiGe)
		{	//�����
			m_xPartTbl.GetGridAt(i,iCurCol)->data.SetGridString(recArr[i-iStartRow].spec);
			m_xPartTbl.GetGridAt(i,iCurCol)->feature=TEXTALIGN_LEFT_CENTER;	//���������
			iCurCol++;
		}
		if(sys.part_group_map.part_table.bDispPartLen)
		{	//������
			tem_str.Printf("%.0f",recArr[i-iStartRow].length);	
			m_xPartTbl.GetGridAt(i,iCurCol)->data.SetGridString(tem_str);
			m_xPartTbl.GetGridAt(i,iCurCol)->feature=TEXTALIGN_RIGHT_CENTER;
			iCurCol++;
		}
		if(sys.part_group_map.part_table.bDispPartNum)
		{	//������
			tem_str.Printf("%d",recArr[i-iStartRow].num);
			m_xPartTbl.GetGridAt(i,iCurCol)->data.SetGridString(tem_str);
			m_xPartTbl.GetGridAt(i,iCurCol)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
			iCurCol++;
		}
		if (fStampLabelW > 0&& pStampLabelItem!=NULL)
		{	//��ӡ��
			//TODO:Ӧ����pStampLabelItem->szStrBuffָ����ʽ������ӡ�Ÿ�ʽ
			CPartStatRecord* pRec = &recArr[i - iStartRow];
			CXhChar16 szMatIncQuality("%C",pRec->cMaterial);
			tem_str.Printf("DF-%s-%s-%s-%d", Ta.m_sTaType,pRec->sPartNo,(char*)szMatIncQuality,pRec->numPerTower);
			m_xPartTbl.GetGridAt(i, iCurCol)->data.SetGridString(tem_str);
			m_xPartTbl.GetGridAt(i, iCurCol)->feature = TEXTALIGN_MIDDLE_CENTER;	//���������
			iCurCol++;
		}
		if(sys.part_group_map.part_table.bDispPartWeight)
		{	//����֮һ����
			tem_str.Printf("%.2f",recArr[i-iStartRow].piece_weight);
			m_xPartTbl.GetGridAt(i,iCurCol)->data.SetGridString(tem_str);
			m_xPartTbl.GetGridAt(i,iCurCol)->feature=TEXTALIGN_RIGHT_CENTER;
			iCurCol++;
			//����֮С����
			tem_str.Printf("%.1f",recArr[i-iStartRow].num*recArr[i-iStartRow].piece_weight);
			m_xPartTbl.GetGridAt(i,iCurCol)->data.SetGridString(tem_str);
			m_xPartTbl.GetGridAt(i,iCurCol)->feature=TEXTALIGN_RIGHT_CENTER;
			iCurCol++;
		}
		if(sys.part_group_map.part_table.bDispNotes)
		{	//��ע��
			m_xPartTbl.GetGridAt(i,iCurCol)->data.SetGridString(recArr[i-iStartRow].sNotes);
			m_xPartTbl.GetGridAt(i,iCurCol)->feature=TEXTALIGN_LEFT_CENTER;	//���������
			iCurCol++;
		}
	}
	if(sys.part_group_map.part_table.bDispPartGroupNum)
	{	//��Ϲ�������
		tem_str.Printf("%d",m_pPartGroup->m_uStatMatCoef);
		m_xPartTbl.GetGridAt(iStartRow,nColumnsN-1)->data.SetGridString(tem_str);
		m_xPartTbl.GetGridAt(iStartRow,nColumnsN-1)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
	}
	//�麸������������Ҫ��ʾ����Ҫ����ѡ�������豸 wht 18-03-22
	BOOL bDispPartWeight=TRUE;
	if(gxDrwLocalizer.ciCustomerSerial==10)
		bDispPartWeight=FALSE;	//���ͨ�麸ͼ����ʾ���� wht 18-09-27
	if(bDispPartWeight)	//sys.part_group_map.part_table.bDispPartWeight
	{	//����ϼ���
		int j=0,iStartCol=0,iEndCol=nColumnsN;
		if(sys.part_group_map.part_table.bDispPartGroupNo)
			iStartCol=1;
		if(sys.part_group_map.part_table.bDispPartGroupNum)
			iEndCol=nColumnsN-1;
		//m_xPartTbl.GetGridAt(nRowsN-1,iStartCol+1)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
		m_xPartTbl.GetGridAt(nRowsN-1,iStartCol)->data.SetGridString("total");
#else
		m_xPartTbl.GetGridAt(nRowsN-1,iStartCol)->data.SetGridString("��      ��");
#endif
		m_xPartTbl.GetGridAt(nRowsN-1,iStartCol)->feature = 1; //�����������
		for(j=(iStartCol+2);j<iEndCol;j++)
			m_xPartTbl.GetGridAt(nRowsN-1,j)->m_bHGridVirtual = TRUE;//ˮƽ�ϲ���Ԫ��
		double weight = 0;
		for(i=0;i<matStat.GetNodeNum();i++)
			weight += recArr[i].num*recArr[i].piece_weight;
		tem_str.Printf("%.1fkg",weight);
		m_xPartTbl.GetGridAt(nRowsN-1,iStartCol+1)->data.SetGridString(tem_str);
		m_xPartTbl.GetGridAt(nRowsN-1,iStartCol+1)->feature = 1;	//�����������
	}
	m_xPartTbl.SetLocateStyle(TOP_RIGHT);	//���Ͻ�Ϊ��λ��
	delete []recArr;
}
//��ڵ��麸��װ�����Ϣ�����SegMap.cpp ���ƶ���
static CXhChar16 GetRelationSonViewNo(long hTube,const char* sKey,CLDSPartGroup* pPartGroup)
{
	CXhChar16 sValue;
	if(pPartGroup==NULL)
		return sValue;
	BOOL bPush=pPartGroup->SonViews.push_stack();
	for(CSectionView* pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
	{
		if(pView->m_iViewType==VIEW_MAIN || pView->m_iViewType==VIEW_FRONT)
			continue;
		if(strstr(pView->Symbol,sKey)==NULL)
			continue;
		CLDSPart *pMainPart=pView->GetMainPartHandle(&Ta);
		CLDSPart *pFatherPart=Ta.FromPartHandle(pMainPart->m_hPartWeldParent);
		if(pFatherPart->handle==hTube)
		{
			sValue.Copy(pView->Symbol);
			break;
		}
	}
	if(bPush)
		pPartGroup->SonViews.pop_stack();
	return sValue;
}
//��ʼ����ڵ��麸��������ֹܵĽ�����Ϣ
void CPartGroupTblDrawing::InitSpherePartGroupInsters()
{
	if(m_pPartGroup==NULL)
		return;
	CLDSSphere* pSphere=(CLDSSphere*)Ta.FromPartHandle(m_pPartGroup->handle);
	if(pSphere==NULL)
		return;
	for(CSectionView* pView=m_pPartGroup->SonViews.GetFirst();pView;pView=m_pPartGroup->SonViews.GetNext())
	{
		if(pView->m_iViewType!=VIEW_MAIN && pView->m_iViewType!=VIEW_FRONT)
			continue;
		for(CSectionView::PART_ITEM* pItem=pView->SonParts.GetFirst();pItem;pItem=pView->SonParts.GetNext())
		{
			if(pItem->cFlag=='S')
				continue;
			CLDSPart* pCurPart=Ta.FromPartHandle(pItem->hSonPart);
			if(pCurPart==NULL || pCurPart->GetClassTypeId()!=CLS_LINETUBE)
				continue;
			CLDSLineTube* pLineTube=(CLDSLineTube*)pCurPart;
			SPHERE_POINT* pSpherePoint=m_pointInfoHash.GetValue(pLineTube->handle);
			if(pSpherePoint)
				continue;
			//ȷ���˼������췽�����ӷ���
			CLDSParamPlate* pFl=NULL;
			f3dPoint startPt,endPt,line_vec,vec1,vec2;
			if(pLineTube->pStart->handle==pSphere->hDatumNode)
			{
				pFl=(CLDSParamPlate*)Ta.FromHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				startPt=pLineTube->Start();
				endPt=pLineTube->End();
			}
			else
			{	
				pFl=(CLDSParamPlate*)Ta.FromHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				startPt=pLineTube->End();
				endPt=pLineTube->Start();
			}
			line_vec=endPt-startPt;
			normalize(line_vec);
			/*��ʼ��װ�����Ϣ��һ���ں������ϣ���һ����׼��
			������������׼�߹��ߣ�����һ��װ����ں�����ƫ��90���λ��
			*/
			pSpherePoint=m_pointInfoHash.Add(pLineTube->handle);
			pSpherePoint->hTube=pLineTube->handle;
			double angle1=0,angle2=0,R=pLineTube->GetDiameter()*0.5;
			vec1=pLineTube->ucs.axis_x;
			if(pLineTube->m_bHasWeldRoad)
			{
				angle1=pLineTube->CalWeldRoadAngle();
				RotateVectorAroundVector(vec1,angle1,pLineTube->ucs.axis_z);
			}
			vec2=pView->ucs.axis_x;
			if(fabs(vec1*vec2)>EPS_COS)	//���������׼�߹���
			{
				angle2=0.5*Pi;
				vec2=vec1;
				RotateVectorAroundVector(vec2,angle2,pLineTube->ucs.axis_z);
			}
			//����ֹ�������Ľ���
			f3dPoint inters,inters1,inters2,offset_vec;
			f3dLine line;
			line.startPt=endPt+vec1*R;
			line.endPt=line.startPt-line_vec*(pLineTube->GetLength()+pSphere->D*2);
			coord_trans(line.startPt,pSphere->ucs,FALSE);
			coord_trans(line.endPt,pSphere->ucs,FALSE);
			if(Int2dls(inters1,inters2,line,pSphere->D*0.5)!=2)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("there is not valid intersection point of tube and ball");
#else
				logerr.Log("�ֹ�����û����Ч����");
#endif
				return;
			}
			f3dPoint tmp_vec=line_vec;
			vector_trans(tmp_vec,pSphere->ucs,FALSE);
			if(tmp_vec*(inters1-inters2)>0)
				inters=inters1;
			else
				inters=inters2;
			pSpherePoint->intersPt1=SPHERE_COORD::TransToSphereCoord(inters);

			line.startPt=endPt+vec2*R;
			line.endPt=line.startPt-line_vec*(pLineTube->GetLength()+pSphere->D*2);
			coord_trans(line.startPt,pSphere->ucs,FALSE);
			coord_trans(line.endPt,pSphere->ucs,FALSE);
			if(Int2dls(inters1,inters2,line,pSphere->D*0.5)!=2)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("there is not valid intersection point of tube and ball");
#else
				logerr.Log("�ֹ�����û����Ч����");
#endif
				return;
			}
			if(tmp_vec*(inters1-inters2)>0)
				inters=inters1;
			else
				inters=inters2;
			pSpherePoint->intersPt2=SPHERE_COORD::TransToSphereCoord(inters);
			//�������ĵ��ֹܷ�����������
			double fThick=0;
			startPt=pSphere->ucs.origin;
			if(pFl && pFl->GetClassTypeId()==CLS_PARAMPLATE)
			{	
				fThick=pFl->GetThick();
				endPt = pFl->ucs.origin;
			}
			pSpherePoint->fEdgeDist=DISTANCE(startPt,endPt)+0.5*fThick;
			//��ȡ��ֹ������������ͼ���
			pSpherePoint->sSonTag=GetRelationSonViewNo(pCurPart->handle,pView->Symbol,m_pPartGroup);
		}
	}
}
void CPartGroupTblDrawing::CreateSphereIntersTbl(double scale)
{
	if(m_pPartGroup==NULL)
		return;
	CLDSPart* pRootPart=Ta.FromPartHandle(m_pPartGroup->handle);
	if(pRootPart->GetClassTypeId()!=CLS_SPHERE)
		return;
	InitSpherePartGroupInsters();			//��ʼ��������Ϣ������������
	//-------������Ϣ��ϸ��-----------
	int nRowsN = 2*m_pointInfoHash.GetNodeNum()+2;//��������
	int nColumnsN=5;						//��������
	m_xPartTbl.Create(nRowsN,nColumnsN);
	//���ñ������߶�
	double fTblTextH=sys.fonts.weld.fNumericTextSize;	//����������ָ�
	double fZoomFontCoef=ZoomToModelTextSize(1.0)*scale;
	double fDimTextSize=fTblTextH*fZoomFontCoef;
	for(int m=0;m<nRowsN;m++)
	{
		for(int n=0;n<nColumnsN;n++)
			m_xPartTbl.GetGridAt(m,n)->textsize=fDimTextSize;
	}
	//�����и�
	int i=0;
	for(i=0;i<nRowsN;i++)			//������
		m_xPartTbl.SetRowHigh(i,(fTblTextH+2)*fZoomFontCoef);
	//�������������п��Լ������
	m_xPartTbl.SetColumnWide(0,(fTblTextH+10)*fZoomFontCoef);	//�ֹܱ����
	m_xPartTbl.SetColumnWide(1,(fTblTextH+15)*fZoomFontCoef);	//������ͼ�����
	m_xPartTbl.SetColumnWide(2,(fTblTextH+20)*fZoomFontCoef);	//���ĵ�������������
	m_xPartTbl.SetColumnWide(3,(fTblTextH+10)*fZoomFontCoef);	//������װ������
	m_xPartTbl.SetColumnWide(4,(fTblTextH+37)*fZoomFontCoef);	//������װ�����Ϣ��
	//���Ʊ��
	int iCurCol=0;
	m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("tube number");
#else
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("�ֹܱ��");
#endif
	m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;	//����Գ�MC?
	iCurCol++;
	m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("subordinate view");
#else
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("������ͼ");
#endif
	m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;	//����Գ�MC
	iCurCol++;
	m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("the margin of ball centre and flange");
#else
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("����-�����߾�");
#endif
	m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
	iCurCol++;
	m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("assemble point sequence number");
#else
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("װ������");
#endif
	m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
	iCurCol++;
	m_xPartTbl.GetGridAt(1,iCurCol)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
#ifdef AFX_TARG_ENU_ENGLISH
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("assemble point coordinate");
#else
	m_xPartTbl.GetGridAt(0,iCurCol)->data.SetGridString("װ�������");
#endif
	m_xPartTbl.GetGridAt(0,iCurCol)->feature = 1;
	iCurCol++;
	//����--������
	i=2;
	for(SPHERE_POINT* pSpherePoint=m_pointInfoHash.GetFirst();pSpherePoint;pSpherePoint=m_pointInfoHash.GetNext())
	{
		//�ֹܱ����
		CLDSPart* pPart=Ta.FromPartHandle(pSpherePoint->hTube);
		m_xPartTbl.GetGridAt(i+1,0)->m_bVGridVirtual=TRUE;
		m_xPartTbl.GetGridAt(i,0)->data.SetGridString(GetPartNoIncMat(pPart->GetPartNo(),pPart->cMaterial));
		m_xPartTbl.GetGridAt(i,0)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		//������ͼ
		m_xPartTbl.GetGridAt(i+1,1)->m_bVGridVirtual=TRUE;
		m_xPartTbl.GetGridAt(i,1)->data.SetGridString(pSpherePoint->sSonTag);
		m_xPartTbl.GetGridAt(i,1)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		//������߾�
		CXhChar100 tem_str;
		tem_str.Printf("%f",pSpherePoint->fEdgeDist);
		SimplifiedNumString(tem_str);
		m_xPartTbl.GetGridAt(i+1,2)->m_bVGridVirtual=TRUE;
		m_xPartTbl.GetGridAt(i,2)->data.SetGridString(tem_str);
		m_xPartTbl.GetGridAt(i,2)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		//װ������
		m_xPartTbl.GetGridAt(i,3)->data.SetGridString("1");
		m_xPartTbl.GetGridAt(i,3)->feature=TEXTALIGN_MIDDLE_CENTER;
		m_xPartTbl.GetGridAt(i+1,3)->data.SetGridString("2");
		m_xPartTbl.GetGridAt(i+1,3)->feature=TEXTALIGN_MIDDLE_CENTER;
		//װ�������
		m_xPartTbl.GetGridAt(i,4)->data.SetGridString(pSpherePoint->intersPt1.ToDegreeString());
		m_xPartTbl.GetGridAt(i,4)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		m_xPartTbl.GetGridAt(i+1,4)->data.SetGridString(pSpherePoint->intersPt2.ToDegreeString());
		m_xPartTbl.GetGridAt(i+1,4)->feature=TEXTALIGN_MIDDLE_CENTER;	//���������
		i+=2;
	}
	m_xPartTbl.SetLocateStyle(TOP_RIGHT);	//���Ͻ�Ϊ��λ��
}
f2dRect CPartGroupTblDrawing::GetDrawRegion(bool bIncSectSymbol/*=false*/)
{
	f2dRect rect;
	rect.topLeft.Set(0,0);
	rect.bottomRight.Set(m_xPartTbl.GetTableWide(),-m_xPartTbl.GetTableHeight());
	return rect;
}
///////////////////////////////////////////////////////////////////////
// CSectionKmAngleViewDrawing
CSectionKmAngleViewDrawing::CSectionKmAngleViewDrawing(int id/* =0 */,IDrawing* pParentDrawing/* =NULL */)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_iComponentType=CSectionViewDrawing::CLS_KM_ANGLE_VIEW;
	m_bFinalDrawing=true;
}
f2dRect CSectionKmAngleViewDrawing::GetDrawRegion(bool bIncSectSymbol/*=false*/)
{
	double zoomScaleCoef=1.0/m_fUser2ScrDrawScale;
	f2dRect rect;
	rect.topLeft.Set(0,0);
	double fWidthZoomCoef=1.6;							//ȡ��ģ�Ǹֿ�ȵ�1.6��
	double fZhunJuDimLen=sys.dim_map.fDimTextSize*5;	//Ԥ������׼�ݱ�ע���
	rect.bottomRight.Set(m_xKaoMeAngle.length*zoomScaleCoef+fZhunJuDimLen,m_xKaoMeAngle.wide*2*fWidthZoomCoef*zoomScaleCoef);
	return rect;
}
BOOL CSectionKmAngleViewDrawing::CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL)
		return FALSE;
	if(pSectionView->GetViewTypeId()!=CSectionView::CLS_KAOME_ANGLE_VIEW)
		return FALSE;
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	m_pPartGroup=pPartGroup;
	//��ʼ����ģ�Ǹ�
	int i=0;
	CLDSBolt *pBolt=NULL;
	CKaoMeAngleSectionView *pKmAngleView=(CKaoMeAngleSectionView*)pSectionView;
	DYN_ARRAY<long> boltHandleArr(pKmAngleView->boltSetInWingY.GetNodeNum()+pKmAngleView->boltSetInWingX.GetNodeNum());
	for(pBolt=pKmAngleView->boltSetInWingX.GetFirst();pBolt;pBolt=pKmAngleView->boltSetInWingX.GetNext())
		boltHandleArr[i++]=pBolt->handle;
	for(pBolt=pKmAngleView->boltSetInWingY.GetFirst();pBolt;pBolt=pKmAngleView->boltSetInWingY.GetNext())
		boltHandleArr[i++]=pBolt->handle;
	InitKaoMeAngle(m_xKaoMeAngle,pKmAngleView->m_pAngle,boltHandleArr,boltHandleArr.Size());
	return TRUE;
}
void CSectionKmAngleViewDrawing::InitKaoMeAngle(CKaoMeAngle &kaoMeAngle,CLDSLineAngle *pAngle,long* boltHandleArr,
												int nBoltCount,CLDSLineAngle *pCoupleAngle/*=NULL*/)
{
	pAngle->RetrieveKaoMeAngle(&kaoMeAngle,boltHandleArr,nBoltCount,pCoupleAngle);
	if(sys.part_group_map.nKmJgGIncrement>0)
	{	//������ģ�Ǹ��ľ�
		for(CKaoMeAngle::BOLTHOLE *pHole=kaoMeAngle.listHoles.GetFirst();pHole;pHole=kaoMeAngle.listHoles.GetNext())
		{
			if(pHole->pos.y>0)
				pHole->pos.y+=sys.part_group_map.nKmJgGIncrement;
			else
				pHole->pos.y-=sys.part_group_map.nKmJgGIncrement;
		}
	}
	kaoMeAngle.wide =(short)pAngle->GetWidth(),kaoMeAngle.thick=(short)pAngle->GetThick();
	//11��Ϊ���ÿ�ģ�Ǹ� ������������ccs 2016-4-20
	CSize	kaoMoJgGuiGeArr[11]={CSize(63,5),CSize(70,5),CSize(75,5),	// CSize��63,5��->(��ȣ����):L63X5
								 CSize(80,6),CSize(90,7), CSize(100,7),
								 CSize(125,8),CSize(140,10),CSize(160,10),
								 CSize(180,12),CSize(200,14)};
	//ԭ�򣺿�ģ�Ǹֹ�񲻳�����
	//�����ڳ��ÿ�ģ�Ǹֹ����Ҳ�����񣬼�ʹ�����ĽǸֹ��
	for(int j=0;j<11;j++)
	{
		if(kaoMoJgGuiGeArr[j].cx<kaoMeAngle.wide)
			continue;
		if(kaoMeAngle.wide==kaoMoJgGuiGeArr[j].cx)
		{
			if(kaoMeAngle.thick>kaoMoJgGuiGeArr[j].cy)
				kaoMeAngle.thick=(short)kaoMoJgGuiGeArr[j].cy;
		}
	}
}

///////////////////////////////////////////////////////////////////////
// CSectionAngleViewDrawing
CSectionAngleViewDrawing::CSectionAngleViewDrawing(int id/* =0 */,IDrawing* pParentDrawing/* =NULL */)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_bFinalDrawing=true;
	m_iComponentType=CSectionViewDrawing::CLS_ANGLE_SPREAD_VIEW;
}
f2dRect CSectionAngleViewDrawing::GetDrawRegion(bool bIncSectSymbol/*=false*/)
{
	return m_xAngleDrawing.GetDrawRegion(1.0);
}
BOOL CSectionAngleViewDrawing::CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	if(pSectionView==NULL||pTower==NULL||pPartGroup==NULL)
		return FALSE;
	if(pSectionView->GetViewTypeId()!=CSectionView::CLS_ANGLE_SPREAD_VIEW)
		return FALSE;
	m_pTower=pTower;
	m_pSectionView=pSectionView;
	m_pPartGroup=pPartGroup;
	CPartGroupDrawing* pParentDrawing = (CPartGroupDrawing*)BelongDrawing();
	if (pParentDrawing == NULL)
		return FALSE;
	CLDSLineAngle *pLineAngle = (CLDSLineAngle*)Ta.Parts.FromHandle(m_pPartGroup->handle, CLS_LINEANGLE);
	if (pLineAngle == NULL)
		return FALSE;
	double fCardL=pParentDrawing->GetMaxDrawingLength()-70;
	double fCardW=pParentDrawing->GetMaxDrawingHeight()-40;
	m_xAngleDrawing.dimStyleFlag.SetFlagDword(CAngleDrawing::ModulateLongJg);	//���ݻ�ͼ��������Ǹֳ���
	m_xAngleDrawing.CreateLineAngleDrawing(pLineAngle, fCardL, fCardW);
	return TRUE;
}
///////////////////////////////////////////////////////////////////////
// CSectionTubeEndPlateViewDrawing
CSectionTubeEndPlateViewDrawing::CSectionTubeEndPlateViewDrawing(int id/* =0 */,IDrawing* pParentDrawing/* =NULL */)
	: CSectionViewDrawing(id,pParentDrawing)
{
	m_pParamPlate=NULL;
	m_bFinalDrawing=true;
	m_iComponentType=CSectionViewDrawing::CLS_TUBE_ENDPLATE_VIEW;
}
f2dRect CSectionTubeEndPlateViewDrawing::GetDrawRegion(bool bIncSectSymbol/*=false*/)
{
	return CSectionViewDrawing::GetDrawRegion(bIncSectSymbol);;
}
BOOL CSectionTubeEndPlateViewDrawing::CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing/*=true*/)
{
	if(pTower==NULL||pPartGroup==NULL)
		return FALSE;
	m_tempView.SetBelongPartGroup(pPartGroup);
	m_pTower=pTower;
	m_pSectionView=&m_tempView;
	m_pPartGroup=pPartGroup;
	if(m_pParamPlate)
	{
		m_tempView.ucs=m_pParamPlate->ucs;
		plateDrawer.Init(m_pParamPlate);
		plateDrawer.Draw(this);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
//CJointTubeDimDrawingCommand
//////////////////////////////////////////////////////////////////////
CJointTubeDimDrawingCommand::CJointTubeDimDrawingCommand(int id/* =0 */,DrawingCommandContext* pContext/* =NULL */)
	: CStruDrawingCommand(id,pContext)
{
	m_pPart=NULL;
	m_iDrawCmdType=CStruDrawingCommand::CLS_JOINT_TUBE_DIM_CMD;
}
void CJointTubeDimDrawingCommand::Redraw()
{
	if(m_pPart==NULL)
		return;
	if(m_pPart->GetClassTypeId()==CLS_LINETUBE)
		CreateJointTubeDims();
	return;
}
extern CLDSPart* GetTubeJointPart(CLDSLineTube* pLineTube,BOOL bIntersect);
extern BOOL GetTubeCheckPoint(CLDSLineTube* pLineTube,BOOL bLeft,f3dPoint workPlaneNorm,f3dPoint &checkPt,BOOL bStart=FALSE);
void CJointTubeDimDrawingCommand::CreateJointTubeDims()
{
	CSectionViewDrawing* pDrawingView=(CSectionViewDrawing*)BelongDrawingComponent();
	CLDSLineTube* pDatumTube=(CLDSLineTube*)m_pPart;
	CLDSPart* pPart=GetTubeJointPart(pDatumTube,TRUE);
	CSectionView *pSectionView=pDrawingView->GetSectionView();
	if(pSectionView&&pSectionView->m_idViewType==CSectionView::CLS_JOINT_TUBES_VIEW)
	{
		CJointTubesView *pJointTubes=(CJointTubesView*)pSectionView;
		if(pDatumTube->handle==pJointTubes->m_hRootTube)
			pPart=pDatumTube->BelongModel()->FromRodHandle(pJointTubes->m_hJointTube);
		else if(pDatumTube->handle==pJointTubes->m_hJointTube)
			pPart=pDatumTube->BelongModel()->FromRodHandle(pJointTubes->m_hRootTube);
	}
	if(pPart==NULL || pPart->GetClassTypeId()!=CLS_LINETUBE)
		return;
	IDbRotatedDimension* pSpaceDim=NULL;
	CLDSLineTube* pJointTube=(CLDSLineTube*)pPart;
	UCS_STRU ucs=pDrawingView->GetCS();
	double fTextHight=sys.fonts.weld.fDimTextSize*20;//CSizeTextDim::dimStyle.textHeight*20;
	f3dPoint line_vec,line_norm,origin,cutPlanePos,cutPlaneNorm;
	//1����ӳߴ��ע
	f3dPoint startPt,endPt,dimPos,dimVec;
	double fOffDist=0,fDimDist=0,angle=0;
	CXhChar50 dimText;
	CLDSLineTube* pLineTube=NULL;
	for(int i=0;i<2;i++)
	{
		if(i==0)
			pLineTube=pDatumTube;
		else
			pLineTube=pJointTube;
		//��ʼ���ֹܵ�ʸ����Ϣ
		line_vec=pLineTube->End()-pLineTube->Start();
		normalize(line_vec);
		TUBEJOINT *pTubeCutJoint=NULL;
		if(pLineTube->m_tJointStart.type<=1 && pLineTube->m_tJointEnd.type>=2)
		{
			origin=pLineTube->End()+line_vec*pLineTube->endOdd();
			cutPlanePos=pLineTube->m_tJointStart.cutPlanePos;
			cutPlaneNorm=pLineTube->m_tJointStart.cutPlaneNorm;
		}
		else if(pLineTube->m_tJointEnd.type<=1 && pLineTube->m_tJointStart.type>=2)
		{	
			origin=pLineTube->Start()-line_vec*pLineTube->startOdd();
			cutPlanePos=pLineTube->m_tJointEnd.cutPlanePos;
			cutPlaneNorm=pLineTube->m_tJointEnd.cutPlaneNorm;
		}
		else
			continue;
		if(line_vec*ucs.axis_x<0)
			line_vec*=-1;
		line_norm=line_vec;
		RotateVectorAroundVector(line_norm,Pi/2,ucs.axis_z);
		double R=pLineTube->GetDiameter()*0.5;
		//��Ӹֹ���Գߴ��ע
		for(int j=0;j<3;j++)
		{
			if(j==0)
			{
				startPt=origin;
				fOffDist=fTextHight*1.5;
			}
			else if(j==1)
			{	
				startPt=origin+line_norm*R;
				fOffDist=fTextHight*1.5;
			}
			else 
			{	
				startPt=origin-line_norm*R;
				fOffDist=-fTextHight*1.5;
			}
			Int3dlf(endPt,startPt,line_vec,cutPlanePos,cutPlaneNorm);
			pSpaceDim=(IDbRotatedDimension*)AppendDbEntity(IDbEntity::DbRotatedDimension);
			fDimDist=DISTANCE(startPt,endPt);		//��ע���ݣ����룩
			dimText.Printf("%.0f",fDimDist);
			pSpaceDim->SetDimText(dimText);
			coord_trans(startPt,ucs,FALSE);			//��ע���
			startPt.z=0;
			pSpaceDim->SetLine1Point(startPt);
			coord_trans(endPt,ucs,FALSE);			//��ע�յ�
			endPt.z=0;
			pSpaceDim->SetLine2Point(endPt);
			dimPos=(startPt+endPt)*0.5;				//��עλ��
			dimPos.y+=fOffDist;
			pSpaceDim->SetDimLinePoint(dimPos);
			angle=atan((endPt.y-startPt.y)/(endPt.x-startPt.x));	//��ע�Ƕ�
			pSpaceDim->SetRotationRadian(angle);
		}
		//��ӷ�������ע
		CLDSParamPlate* pFl=(CLDSParamPlate*)GetTubeJointPart(pLineTube,FALSE);
		if(pFl==NULL || !pFl->IsFL())
			continue;
		double H=0;
		int sign=(i>0)?1:-1; 
		pSpaceDim=(IDbRotatedDimension*)AppendDbEntity(IDbEntity::DbRotatedDimension);
		pFl->GetDesignItemValue('H',&H);		//��ע����
		pSpaceDim->SetDimText(CXhChar50(H));
		startPt=origin;							//��ע���
		coord_trans(startPt,ucs,FALSE);
		startPt.z=0;
		pSpaceDim->SetLine1Point(startPt);
		endPt=origin+line_vec*sign*H;			//��ע�յ�
		coord_trans(endPt,ucs,FALSE);
		endPt.z=0;
		pSpaceDim->SetLine2Point(endPt);
		dimPos=(startPt+endPt)*0.5;				//��עλ��
		dimPos.y+=fTextHight*1.5;
		pSpaceDim->SetDimLinePoint(dimPos);
		angle=atan((endPt.y-startPt.y)/(endPt.x-startPt.x));	//��ע�Ƕ�
		pSpaceDim->SetRotationRadian(angle);
	}
	//2����ӽǶȱ�ע
	/*f3dPoint vertex,start_vec,end_vec;
	if(pDatumTube->m_tJointStart.type==1)
		start_vec=pDatumTube->End()-pDatumTube->Start();
	else
		start_vec=pDatumTube->Start()-pDatumTube->End();
	normalize(start_vec);
	if(pLineTube->m_tJointStart.type==1)
		end_vec=pLineTube->End()-pLineTube->Start();
	else
		end_vec=pLineTube->Start()-pLineTube->End();
	normalize(end_vec);
	Int3dlf(vertex,ucs.origin,start_vec,cutPlanePos,cutPlaneNorm);
	vector_trans(start_vec,ucs,FALSE);
	vector_trans(end_vec,ucs,FALSE);
	coord_trans(vertex,ucs,FALSE);
	CSectionDrawingEntsManager manager(pDatumTube,this);
	manager.AppendAngularDim(vertex,start_vec,end_vec,0.5*pDatumTube->GetDiameter());*/
	//3����Ӽ���߱�ע
	/*GetTubeCheckPoint(pDatumTube,FALSE,ucs.axis_z,startPt);
	GetTubeCheckPoint(pJointTube,TRUE,ucs.axis_z,endPt);
	if(!startPt.IsZero() && !endPt.IsZero())
	{	//�ϼ����
		pSpaceDim=(IDbRotatedDimension*)AppendDbEntity(IDbEntity::DbRotatedDimension);
		fDimDist=DISTANCE(startPt,endPt);			//��ע����
		dimText.Printf("%.0f",fDimDist);
		pSpaceDim->SetDimText(dimText);
		startPt.z=0;
		pSpaceDim->SetLine1Point(startPt);
		endPt.z=0;
		pSpaceDim->SetLine2Point(endPt);
		angle=atan((endPt.y-startPt.y)/(endPt.x-startPt.x));	//��ע�Ƕ�
		pSpaceDim->SetRotationRadian(angle);
		dimVec=f3dPoint(-sin(angle),cos(angle),0);	//��עλ��
		dimPos=(startPt+endPt)*0.5;
		dimPos+=dimVec*(fTextHight*2);
		pSpaceDim->SetDimLinePoint(dimPos);
	}
	GetTubeCheckPoint(pDatumTube,TRUE,ucs.axis_z,startPt);
	GetTubeCheckPoint(pJointTube,FALSE,ucs.axis_z,endPt);
	if(!startPt.IsZero() && !endPt.IsZero())
	{	//�¼����
		pSpaceDim=(IDbRotatedDimension*)AppendDbEntity(IDbEntity::DbRotatedDimension);
		fDimDist=DISTANCE(startPt,endPt);		//��ע����
		dimText.Printf("%.0f",fDimDist);
		pSpaceDim->SetDimText(dimText);	
		startPt.z=0;
		pSpaceDim->SetLine1Point(startPt);
		endPt.z=0;
		pSpaceDim->SetLine2Point(endPt);
		angle=atan((endPt.y-startPt.y)/(endPt.x-startPt.x));	//��ע�Ƕ�
		pSpaceDim->SetRotationRadian(angle);
		dimVec=f3dPoint(-sin(angle),cos(angle),0);	//��עλ��
		dimPos=(startPt+endPt)*0.5;
		dimPos-=dimVec*(fTextHight*2);
		pSpaceDim->SetDimLinePoint(dimPos);
	}*/
}
#endif
