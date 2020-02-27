#ifndef __DRAW_TA_MAP_H_
#define __DRAW_TA_MAP_H_
#include "stdafx.h"
#include "stdarx.h"
#include "Tower.h"
#include "DrawUnit.h"
#include "Drawing.h"
#include "DragEntSet.h"

class CDrawPartInfo{
public:
	CLDSPart *pPart;
	CViewPart *pViewPart;
	void* pDrawingCmd;
	CDrawPartInfo(){pPart=NULL;pViewPart=NULL;pDrawingCmd=NULL;}
	CDrawPartInfo(CLDSPart *pDrawPart,CViewPart *pRelaViewPart=NULL,void* pRelaDrawingCmd=NULL)
	{
		pPart=pDrawPart;
		pViewPart=pRelaViewPart;
		pDrawingCmd=pRelaDrawingCmd;
	}
};
extern CTower Ta;
UCS_STRU CalDrawingUcs(PARTSET &partset,CDisplayView *pView,BOOL bSingleLineMap);
#ifdef __SMART_DRAW_INC_
//绘制杆件单线图,返回杆件ID以及绘制节点的ID
BOOL DrawRodLine(CLDSLinePart *pLinePart,CDrawCoordTrans& dcs,BOOL bSumMap,BOOL bDimByAnalysisMode,
	AcDbObjectId *pPoleId,AcDbObjectId *pStartId,AcDbObjectId *pEndId,AcDbObjectId *pStartJointId,AcDbObjectId *pEndJointId,CDrawingCommand *pCmd=NULL);
#endif
void DrawSingleLineZhunMap(PARTSET &partset, NODESET&,UCS_STRU ucs,double scale=25,BOOL bSumMap=FALSE);
void DrawSingLine3DMap(PARTSET& partset,UCS_STRU ucs,double scale);
#ifndef __TSA_FILE_
void DrawStruDrawing(PARTSET& partset,NODESET& nodeset,UCS_STRU ucs,
					 int nFaceFlag,double scale,BOOL bSetupDrawing);
void DrawLinePartGroupSetupDrawing(CLDSLinePart *pDatumLinePart,PARTSET& partset,NODESET& nodeset,
						   UCS_STRU ucs,double scale,BOOL bSetupDrawing);
AcDbObjectId DrawPart(AcDbBlockTableRecord *pBlockTableRec,CDrawPartInfo &drawPartInfo,CDrawCoordTrans dcs);	//绘制指定构件
//计算构件绘制方式,传入定制视图用于展开绘制 wht 11-07-22
int CalPartDrawType(CLDSPart *pPart,UCS_STRU dcs,CDisplayView *pDefinedView,CDrawPartTagInfo *pDrawInfo=NULL);
void DrawLsRefList(CLsRefList *pLsRefList,CDrawCoordTrans dcs,BOOL bDrawAllLs=FALSE,COORD3D *pNorm=NULL);
#endif
#endif