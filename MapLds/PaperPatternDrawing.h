#ifndef _PAPER_PATTERN_DRAWING_H__
#define _PAPER_PATTERN_DRAWING_H__
#include "lds_part.h"
#include "PlankPartParaExportDlg.h"

#ifndef __TSA_FILE_
CLDSPart* FindTaPartByCode(char *sPartCode,int *pHits=NULL);
//����bDimPartInfo���������޹��տ�ͼ��ʱ�Ƿ���Ҫ��ע������ϸ(�Ǹ�һͼһ�ļ�ʱ�õ�) wht 11-07-04
//����f2dRect *pDrawRect�����������ָ����ͼ���� wht 11-06-02
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
//para[0]:����ڱ����չ����;para[1]:���������չ����;para[2]:�������ںϳɵ����������и���
void DrawTubeSpreadMap(CLDSLineTube *pTube,BOOL *para=NULL,int iOrder=-1,
					   int nHits=1,char *layer=NULL,int nPageNum=1,
					   int nTaNum=1,BOOL bDimPartInfo=TRUE);
void DrawTubeSpreadMap2(CLDSLineTube *pTube,BOOL *para=NULL,int iOrder=-1,
	int nHits=1,char *layer=NULL,int nPageNum=1,
	int nTaNum=1,BOOL bDimPartInfo=TRUE);
void CustomDrawTubeSpreadMap(CLDSLineTube *pTube,BOOL *para=NULL,int iOrder=-1,
	int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE);
//pDrawRect��ָ����ͼ����  bIntersected���Ƿ���Ƹֹܼ�ͼ
void DrawTubeTypeMap(CLDSLineTube *pLineTube,f2dRect *pDrawRect=NULL,BOOL bIntersected=FALSE,
					 int iOrder=-1,int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE);
//���Ʋ۸�
void DrawSlotOrFlatTypeMap(CLDSPart *pPart,f2dRect *pDrawRect=NULL,int iOrder=-1,
					 int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE,int iPage=-1);
//���ƻ��ι���
void DrawArcPartTypeMap(CLDSArcPart *pArcPart,f2dRect *pDrawRect=NULL,int iOrder=-1,
						int nHits=1,char *layer=NULL,int nPageNum=1,int nTaNum=1,BOOL bDimPartInfo=TRUE,int iPage=-1);
#endif
#endif