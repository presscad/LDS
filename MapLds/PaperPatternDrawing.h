#ifndef _PAPER_PATTERN_DRAWING_H__
#define _PAPER_PATTERN_DRAWING_H__
#include "lds_part.h"
#include "PlankPartParaExportDlg.h"

#ifndef __TSA_FILE_
CLDSPart* FindTaPartByCode(char *sPartCode,int *pHits=NULL);
//参数bDimPartInfo用来控制无工艺卡图框时是否需要标注构件明细(角钢一图一文件时用到) wht 11-07-04
//增加f2dRect *pDrawRect参数用于外界指定绘图区域 wht 11-06-02
int DrawJgTypeMap(CProcessPart *pJgTypePart,CProcessPart *pJgTypePart2,f2dRect *pDrawRect,
			int iOrder=-1,int nHits=1,int nHits2=1,char *layer=NULL,int nPageNum=1,
			int nTaNum=1,BOOL bDimPartInfo=TRUE,int iPage=-1);
BOOL DrawHuoQuJg(CLDSPolyPart *pHuoQuJg,f2dRect *pDrawRect,
			int iOrder=-1,int nHits=1,char *layer=NULL,int nPageNum=1,
			int nTaNum=1,BOOL bDimPartInfo=TRUE,int iPage=-1);
void DrawPlankTypePartMap(CLDSPart *pPlankTypePart,char *export_info,BOOL *para=NULL,
			int iOrder=-1,int nHits=1,BOOL bInProcessCard=TRUE,BOOL bPatternDrawing=TRUE,
			int nPageNum=1,int nTaNum=1,f3dPoint *pOrigin=NULL,BOOL bDimPartInfo=TRUE,
			ATOM_LIST<CPlankPartParaExportDlg::TABLE_ROW> *pTblRowList=NULL);
//para[0]:输出内壁相贯展开线;para[1]:输出外壁相贯展开线;para[2]:输出内外壁合成的内外壁相贯切割线
void DrawTubeSpreadMap(CLDSLineTube *pTube,BOOL *para=NULL,int iOrder=-1,
					   int nHits=1,char *layer=NULL,int nPageNum=1,
					   int nTaNum=1,BOOL bDimPartInfo=TRUE);
void DrawTubeSpreadMap2(CLDSLineTube *pTube,BOOL *para=NULL,int iOrder=-1,
	int nHits=1,char *layer=NULL,int nPageNum=1,
	int nTaNum=1,BOOL bDimPartInfo=TRUE);
void CustomDrawTubeSpreadMap(CLDSLineTube *pTube,BOOL *para=NULL,int iOrder=-1,
	int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE);
//pDrawRect：指定绘图矩形  bIntersected：是否绘制钢管简图
void DrawTubeTypeMap(CLDSLineTube *pLineTube,f2dRect *pDrawRect=NULL,BOOL bIntersected=FALSE,
					 int iOrder=-1,int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE);
//绘制槽钢
void DrawSlotOrFlatTypeMap(CLDSPart *pPart,f2dRect *pDrawRect=NULL,int iOrder=-1,
					 int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE,int iPage=-1);
//绘制环形构件
void DrawArcPartTypeMap(CLDSArcPart *pArcPart,f2dRect *pDrawRect=NULL,int iOrder=-1,
						int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE,int iPage=-1);
#endif
#endif