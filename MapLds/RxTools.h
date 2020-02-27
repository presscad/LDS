#ifndef __RXTOOLS_H_
#define __RXTOOLS_H_

/*#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#define NDEBUG
#endif*/
#include "dbents.h"
#include "dbelipse.h"
#include "dbsymtb.h"
#include "dbmtext.h"
#include "dbspline.h"
#include "acdocman.h"

/*#ifdef _DEBUG_WAS_DEFINED
#undef NDEBUG
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif*/

#include "TblDef.h"
#include "Tower.h"
#include "XhCharString.h"
#include "XhRptTbl.h"
#include "FormatTable.h"
extern CTower Ta;

#ifndef _CLS_ACDB_OBJ_LIFE_DEF_
#define _CLS_ACDB_OBJ_LIFE_DEF_
class CAcDbObjLife
{
	AcDbObject *pCadDbObj;
public:
	CAcDbObjLife(AcDbObject* pDbObj){pCadDbObj=pDbObj;}
	~CAcDbObjLife(){
		if(pCadDbObj)
			pCadDbObj->close();
	}
};
#endif
typedef struct tagTMADRAWING_LAYER
{
	int colorIndex;				//ͼ��Ĭ�ϵ�ͼԪ��ʾ��ɫ����
	AcDb::LineWeight lineWeight;//���Ϳ�
	CXhChar16 layerName;		//ͼ������Ӧ��һ���ַ�����
	CXhChar16 lineTypeName;		//ͼ��Ĭ�����͵��ַ�������
	AcDbObjectId layerId;		//ͼ���Ӧ��ACAD�����ʶId
	AcDbObjectId lineTypeId;	//ͼ��Ĭ�����͵�ACAD�����ʶId
	bool CopyProperty(tagTMADRAWING_LAYER *pLayer);
}TMADRAWING_LAYER;
class LayerTable
{
public:
	static CStringKeyHashList<TMADRAWING_LAYER> layerHashList;	//ͼ���ϣ����ͼ����Ϊ��ֵ
	static TMADRAWING_LAYER VisibleProfileLayer;	//�ṹͼ�пɼ�������ͼ��
	static TMADRAWING_LAYER BriefLineLayer;			//�����ĵ���ͼ
	static TMADRAWING_LAYER AuxLineLayer;			//���ĵ���ͼ
	static TMADRAWING_LAYER UnvisibleProfileLayer;	//�ṹͼ�в��ɼ�������ͼ��
	static TMADRAWING_LAYER AxisLineLayer;			//�����ߡ��Գ��ߡ���λ����
	static TMADRAWING_LAYER SonJgAxisLineLayer;		//�ӽǸ�����ͼ�� 
	static TMADRAWING_LAYER BreakLineLayer;			//�Ͽ�����
	static TMADRAWING_LAYER BendLineLayer;			//�Ǹֻ������ְ����
	static TMADRAWING_LAYER AssistHuoQuLineLayer;	//����������
	static TMADRAWING_LAYER SimPolyLayer;			//�����α�ע
	static TMADRAWING_LAYER BoltSymbolLayer;		//��˨ͼ��
	static TMADRAWING_LAYER BoltLineLayer;			//��˨��
	static TMADRAWING_LAYER DimSizeLayer;			//�ߴ��ע wht 11-07-04
	static TMADRAWING_LAYER DimTextLayer;			//���ֱ�ע
	static TMADRAWING_LAYER DamagedSymbolLine;		//�����������
	static TMADRAWING_LAYER CommonLayer;			//���������������ͼԪ����ͼ��
};
typedef struct tagTMADRAWING_TEXT_STYLE
{
	CXhChar16 textStyleName;	//������ʽ����
	CXhChar16 bigFontName;		//���������ļ�
	AcDbObjectId textStyleId;	//������ʽ
}TMADRAWING_TEXT_STYLE;
class TextStyleTable
{
public:
	static TMADRAWING_TEXT_STYLE hzfs;
};
struct TMADRAWING_DIM_STYLE
{
	CXhChar16 dimStyleName;	//������ʽ����
	double arrowSize;		//��ͷ��С
	AcDbObjectId dimStyleId;	//������ʽ
	TMADRAWING_DIM_STYLE();
	void InitDimStyle(double textHeight=0,double arrowSize=0,double gap=0);
	double get_Gap();
	double set_Gap(double dimgap);
	__declspec(property(get=get_Gap,put=set_Gap)) double Gap;
	static double CalGap(double text_height);
protected:
	double m_fInnerSetTextSize;
	double gap;				//�������׼�߼�϶
};
class DimStyleTable
{
public:
	static TMADRAWING_DIM_STYLE dimStyle;
	static TMADRAWING_DIM_STYLE dimStyle2;
	static TMADRAWING_DIM_STYLE dimStyle3;	//��ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	static TMADRAWING_DIM_STYLE dimStyle4;	//���ַ��ڳߴ������ wht 15-04-28
	static TMADRAWING_DIM_STYLE dimAngleStyle;
	static TMADRAWING_DIM_STYLE dimRadiusStyle;
};
class CNewDatabaseLife
{
	AcDbDatabase newDb;
	AcDbDatabase* pOldDb;
public:
	CNewDatabaseLife();
	~CNewDatabaseLife();
	//
	BOOL SaveAs(const char* sFullPath);
};
//��RGB�õ�cad��ɫ����ֵ 
int GetNearestACI(COLORREF color);
//��CAD����ɫ�õ�RGB
COLORREF GetColorFromIndex(int color_index);
AcDbDatabase *GetCurDwg();
AcApDocument *OpenDwgFile(const char* sDwgFile);
void GetSlotDBFilePath(char* slotguige_path);
void GetJgGuiGeDBFilePath(char* jgguige_path);
void GetTubeGuiGeDBFilePath(char* tubeguige_path);
void GetFlatGuiGeDBFilePath(char* flatguige_path);
void GetSysPath(char* sys_path);
void GetAppPath(char* app_path);
void InitDrawingEnv();
bool load_linetype(char *LineType);
bool SetEntLineType(AcDbEntity* pEnt, char* line_type);
bool SetEntLayer(AcDbEntity* pEnt, char *layer);
bool SetCurLayer(AcDbObjectId& layerId,AcDbEntity* pAcadDbEnt=NULL);
bool SetCurLayer(const char *cur_layer);
bool CreateNewLayer(char *newlayer, char* line_type,AcDb::LineWeight line_thick,
					int color_i,AcDbObjectId &layerId,AcDbObjectId &lineTypeId);
bool ModifySpecialLayer(TMADRAWING_LAYER cur_layer,AcDbObjectId &lineTypeId);
THANDLE GetTaAtomHandle(AcDbEntity* pEnt,long* drawStyle=NULL);
AcDbBlockTableRecord *GetBlockTableRecord();
struct ACADDRAW_CONTEXT{
protected:
	AcDbBlockTableRecord* _pBlockTableRecord;
public:
	AcDbObjectId currLayerId;
	ACADDRAW_CONTEXT(AcDbBlockTableRecord* pBlockTableRecord,AcDbObjectId layerId=NULL);
	operator AcDbBlockTableRecord*(){return _pBlockTableRecord;}
	AcDbBlockTableRecord* operator->(){return _pBlockTableRecord;}
};
//handle���ڼ�¼��ֱ�����������ľ��,style����ù����Ļ��Ʒ�ʽ
//���ڽǸֹ�����style:0.����Ϣ;'X',��ʾX֫��������;-'x',��ʾX֫��������;'Y'��ʾY֫��������;'y'��ʾY֫��������;'Z'��ʾ����ͼ
AcDbObjectId CreateAcadLine(ACADDRAW_CONTEXT context,
							f3dPoint start, f3dPoint end,long handle=NULL,long style=0);
void CreateAcadRect(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint topLeft,double rect_width,
					double rect_height,BOOL bHasBulge=FALSE,THANDLE handle=NULL,
					f3dPoint waist_vec=f3dPoint(0,0,0),double line_width=0);
AcDbObjectId CreateAcadPolyLine(ACADDRAW_CONTEXT context,f3dPoint start,
								f3dPoint end,double line_width,THANDLE handle=NULL);
AcDbObjectId CreateAcadPolyLine(ACADDRAW_CONTEXT context,f3dPoint* vertex_arr,int vertex_num,double line_width,THANDLE handle=NULL);
AcDbObjectId CreateAcadCircle(ACADDRAW_CONTEXT context,
							  f3dPoint centre, double radius,THANDLE handle=NULL);
void CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint center,
				   f3dPoint normal,f3dPoint majorAxis,double radiusRatio,double startAngle,double endAngle,THANDLE handle=NULL);
void CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcline,THANDLE handle=NULL);
//��������һ����������
void CreateAcadSpline(AcDbBlockTableRecord *pBlockTableRecord,
					f3dPoint startPoint, f3dPoint midPoint, f3dPoint endPoint);
void CreateAcadSpline(AcDbBlockTableRecord *pBlockTableRecord,AcGePoint3dArray fitPoints);
//����Բ����㣬�յ��Լ��뾶����Բ�� wht 10-08-18
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,f3dPoint endPoint,THANDLE handle=NULL);
//����Բ�ģ���ʼ�㼰���ν�����һ��Բ��
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,double sectorAngle,THANDLE handle=NULL);
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,double sectorAngle,f3dPoint worknorm,THANDLE handle=NULL);
//�����Բ���� wht 10-12-21
AcDbObjectId CreateAcadPolyArcLine(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcLine,
								   double line_width,THANDLE handle=NULL);
//�����Բ����,������㡢�յ��Լ����ν� wht 11-07-09
AcDbObjectId CreateAcadPolyArcLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,f3dPoint end,
								   double sector_angle,double line_width,THANDLE handle=NULL);
//���Ƹֹ�����			//scaleΪԲ���뾶���������������ı�ֵ
void CreateAcadSectionLine(AcDbBlockTableRecord *pBlockTableRecord,	//����һ������
					f3dPoint startPoint, f3dPoint endPoint, double scale=0.5);
//��ñ��Ԫ����
BOOL GetGridKey(AcDbEntity* pPoint, GRID_DATA_STRU *grid_data);
BOOL GetPadText(AcDbPoint* pPadPoint, char *sPadText);
void TextStyle(AcDbTextStyleTableRecord *pTextStyleTblRec);
void setDimVar(AcDbDimStyleTableRecord *pDimStyleTblRcd,double txt_height=2.5,BOOL bInside=TRUE,BYTE cVerticalPosType=1);
AcDbObjectId 
DrawLine(f3dPoint start,f3dPoint end, THANDLE Handle=NULL);//�Ǹ־��
AcDbObjectId DrawCircle(double x0,double y0,double r);
//ȥ��bInside����,dimStyle3ר������[��ǿ�ƽ��ߴ���ڳߴ���֮��]�ı�ע
//���ʹ��ͬһ��ע��ʽ�����޸ĸ�����ʱ��Ӱ������ʹ�ø���ʽ�ı�ע  wht 11-07-12
AcDbObjectId 
DimSize(AcDbBlockTableRecord *pBlocTableRecord, 
		f3dPoint start,f3dPoint end,//��ע��ʼ/��ֹ��
		f3dPoint dimpos,const char *dimText,//�ߴ���λ�ü�����
		AcDbObjectId dimStyleId,	//=NULL��ע��ʽ��¼ID��
		int dimtype,double txt_height/*=0/*,BOOL bInside=TRUE*/);	//=1 ��ֱ =2ˮƽ  =3��б
AcDbObjectId 
DimAngleSize(AcDbBlockTableRecord *pBlockTableRecord,
		f3dPoint start,f3dPoint end,//��ע��ʼ/��ֹ��
		f3dPoint dimpos,const char *dimText,//�ߴ���λ�ü�����
		AcDbObjectId dimStyleId,	//��ע��ʽ��¼ID��
		double angle,double txt_height=0/*,BOOL bInside=TRUE*/);
AcDbObjectId
DimAlignedSize(AcDbBlockTableRecord *pBlockTableRecord,
		AcDbObjectId dimStyleId,		//��ע��ʽ��¼ID��
		f3dPoint start,f3dPoint end,	//��ע��ʼ/��ֹ��
		f3dPoint dimpos,char *dimText="<>",double txt_height=0);	//�ߴ���λ�ü�����
AcDbObjectId
DimAngle(AcDbBlockTableRecord *pBlockTableRecord,
		AcDbObjectId dimStyleId,		//��ע��ʽ��¼ID��
		f3dPoint vertex,				//���㶨�ǵĽǶ���
		f3dPoint start,f3dPoint end,	//��ע��ʼ/��ֹ��
		f3dPoint dimpos,char *dimText="<>",	//�ߴ���λ�ü�����
		double txt_height=0,double arrowsize=0);
AcDbObjectId 
DimDiameter(AcDbBlockTableRecord *pBlockTableRecord, 
		double* chordPos,double* farChordPos,//��ע��ʼ/��ֹ��
		double leaderLen,char *dimText,
		AcDbObjectId dimStyleId,double txt_height=0);			   //��ע��ʽ��¼ID��
AcDbObjectId DimRadial(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint center,
					   f3dPoint chordPoint,double leaderLen,char *dimText,
					   AcDbObjectId dimStyleId,double txt_height=0);//��ע��ʽ��¼ID��
		
double DistOf2NodeByHandle(THANDLE handle1, THANDLE handle2,int style);
double ReviseTextAngleToHeadUpward(double angle,bool radiusAngleMode=true);
double TestDrawTextLength(const char* dimtext,double height,AcDbObjectId textStyleId);
AcDbObjectId 
DimText(AcDbBlockTableRecord *pBlockTableRecord,
		f3dPoint base,//��ע��ʼ/��ֹ��
		const char *dimText,//�ߴ���λ�ü�����
		AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
		double height,double rotation,
		AcDb::TextHorzMode hMode=AcDb::kTextLeft,AcDb::TextVertMode vMode=AcDb::kTextBase,
		AcDbObjectId layerId=AcDbObjectId::kNull);
AcDbObjectId 
DimMText(AcDbBlockTableRecord *pBlockTableRecord,//����¼ָ��
		char *dimText,//�ߴ���λ�ü�����
		f3dPoint base,//��ע��ʼ/��ֹ��
		double width,	//ÿ�е�����
		double texheight,//���ָ�
		AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
		AcDbMText::AttachmentPoint align_type,	//���ֶ��뷽ʽ
		AcDbMText::FlowDirection flowDirection,double angle=0);
#ifdef __COMMON_PART_INC_
void CalArcLineVertexList(f3dArcLine arcLine,fPtList &vertex_list);
AcDbObjectId AddEntityToDbs(AcDbEntity *pEntity,AcDbBlockTableRecord *pTemRecord=NULL);
#endif
BOOL InsertBlock(AcDbBlockTableRecord *pBlockTableRecord,
					f3dPoint insert_pos,char *blkname,
					double scale=1,double angle=0,
					long handle=0,AcDbObjectId *pOutEntId=NULL);
//�����µ�DWG�ļ�
void NewDwgFile(char *filename);

//���������ʰȡ��pickPt�ж�����һ����ƺ����� wht 12-08-31
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,
						f3dPoint end,f3dPoint pickPt,double width=0.8,double height=0.7);
//���ƺ�����--��ֱ�����
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start, 
						f3dPoint end,double width=0.8,double height=0.7,BOOL bClockwise=FALSE);
//���ƺ�����--��Բ�����
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcline,
						double width=0.8,double height=0.7);
//��������ͼ��
//�ض�ָ��ֱ��
//�����ƶ���
void DrawTubeBreakLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint startPt,
					   f3dPoint endPt,double fBreakLen=1.4,double fLenCoefThreshold=8);
//������ת��
//������ӡ

AcDbObjectId SearchBlock(const char *blkname);
//
void TransPtToCadUcs(f3dPoint& pt,UCS_STRU* pUcs=NULL,double scale=1);
void TransVtToCadUcs(f3dPoint& vect, UCS_STRU* pUcs = NULL);
//ͨ����������ʵ�壨����壩
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,f3dPoint plane_norm,
								  f3dLine path,COLORREF crSolid,THANDLE handle=NULL);
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,
								  f3dLine path,COLORREF crSolid,THANDLE handle=NULL);
//ͨ����������ʵ�壨Բ���壩
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,double fRadius,f3dPoint center,f3dPoint norm,
								  f3dLine path,COLORREF crSolid,THANDLE handle=NULL);
//ͨ����ת����ʵ��
AcDbObjectId CreateSolidByRevolve(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,f3dPoint axisPoint,
								  f3dPoint axisDir,double angle,COLORREF crSolid,THANDLE handle=NULL);
//��ʵ���м�ȥ��ʵ��,ciType==0��������ʵ��,ciType==1��ת����ʵ��
void SubtractSubSolid(AcDbObjectId mainSolidId,fPtList &vertex_list,f3dLine path,int ciType,COLORREF crSolid,
							AcDbBlockTableRecord *pTemRecord=NULL,double angle=2*Pi);
//ͨ���������ת����ʵ��,nManner==0��������ʵ��,nManner==1��ת����ʵ��
AcDbObjectId CreateSolidByManner(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint pt3d[],int n,int nManner,COLORREF crSolid,
								 THANDLE handle=NULL,double angle=2*Pi);
//
TMADRAWING_DIM_STYLE GetDimStyle(char *dimstyle=NULL,double txt_height=2.0,BOOL bInside=TRUE,BYTE cVerticalPosType=1);
TMADRAWING_DIM_STYLE GetDimStyle2(char *dimstyle=NULL,double txt_height=2.0);
TMADRAWING_DIM_STYLE GetDimAngleStyle(char *dimstyle=NULL,double txt_height=2.0);
TMADRAWING_DIM_STYLE GetDimRadiusStyle(char *dimstyle=NULL,double txt_height=2.0);
void ZoomAcadView(AcGePoint2d Pt1, AcGePoint2d Pt2, double scale=1.0);
void ZoomAcadView(f2dRect rect,double extendLen,double scale=1.0);
void ZoomAcadView(SCOPE_STRU scope,double extendLen,double scale=1.0);
//Ϊָ����CAD���������չ��¼,���ر�pObj wht 11-05-26
AcDbObjectId InsertExtensionRecord(AcDbObject *pObj,long handle,int cls_id=0);
//����ָ�����������չ��¼ wht 11-05-26
AcDbObjectId InsertExtensionRecord(AcDbBlockTableRecord *pBlockTableRecord,
						   f3dPoint point,long handle,int cls_id=0);
AcDbObjectId CreateAcadHatch(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertexList,char *sHatchStyle,
							 double scale,double angle=0,COLORREF color=RGB(255,255,255));
//����CADʵ��ID����scope wht 11-08-01
BOOL VerifyVertexByCADEntId(SCOPE_STRU &scope,AcDbObjectId entId,bool blFilterOnlyBomTblLine=false);
//����GUID�ַ���
CXhChar200 CreateGuidStr();
//���Ʊ��
//from DrawTable.cpp
void DrawTable(AcDbBlockTableRecord *pBlockTableRecord, CFormatTbl *pTable, BOOL bPromptPos=TRUE,
	double scale=1, int nLocatePos=-1, const double text_margin=1.5);
void DrawTable(CXhTable *pTable, BOOL bPromptPos = TRUE, double scale = 1, int nLocatePos = -1,
	AcDbBlockTableRecord *pTemRecord = NULL);
#endif