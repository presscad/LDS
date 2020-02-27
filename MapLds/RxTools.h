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
	int colorIndex;				//图层默认的图元显示颜色索引
	AcDb::LineWeight lineWeight;//线型宽
	CXhChar16 layerName;		//图层名均应以一个字符命名
	CXhChar16 lineTypeName;		//图层默认线型的字符串名称
	AcDbObjectId layerId;		//图层对应的ACAD对象标识Id
	AcDbObjectId lineTypeId;	//图层默认线型的ACAD对象标识Id
	bool CopyProperty(tagTMADRAWING_LAYER *pLayer);
}TMADRAWING_LAYER;
class LayerTable
{
public:
	static CStringKeyHashList<TMADRAWING_LAYER> layerHashList;	//图层哈希表以图层名为键值
	static TMADRAWING_LAYER VisibleProfileLayer;	//结构图中可见轮廓线图层
	static TMADRAWING_LAYER BriefLineLayer;			//受力材单线图
	static TMADRAWING_LAYER AuxLineLayer;			//辅材单线图
	static TMADRAWING_LAYER UnvisibleProfileLayer;	//结构图中不可见轮廓线图层
	static TMADRAWING_LAYER AxisLineLayer;			//中心线、对称线、定位轴线
	static TMADRAWING_LAYER SonJgAxisLineLayer;		//子角钢心线图层 
	static TMADRAWING_LAYER BreakLineLayer;			//断开界线
	static TMADRAWING_LAYER BendLineLayer;			//角钢火曲、钢板火曲
	static TMADRAWING_LAYER AssistHuoQuLineLayer;	//辅助火曲线
	static TMADRAWING_LAYER SimPolyLayer;			//相似形标注
	static TMADRAWING_LAYER BoltSymbolLayer;		//螺栓图符
	static TMADRAWING_LAYER BoltLineLayer;			//螺栓线
	static TMADRAWING_LAYER DimSizeLayer;			//尺寸标注 wht 11-07-04
	static TMADRAWING_LAYER DimTextLayer;			//文字标注
	static TMADRAWING_LAYER DamagedSymbolLine;		//板边破损标记线
	static TMADRAWING_LAYER CommonLayer;			//除以上以外的所有图元所在图层
};
typedef struct tagTMADRAWING_TEXT_STYLE
{
	CXhChar16 textStyleName;	//文字样式名称
	CXhChar16 bigFontName;		//汉字字体文件
	AcDbObjectId textStyleId;	//文字样式
}TMADRAWING_TEXT_STYLE;
class TextStyleTable
{
public:
	static TMADRAWING_TEXT_STYLE hzfs;
};
struct TMADRAWING_DIM_STYLE
{
	CXhChar16 dimStyleName;	//文字样式名称
	double arrowSize;		//箭头大小
	AcDbObjectId dimStyleId;	//文字样式
	TMADRAWING_DIM_STYLE();
	void InitDimStyle(double textHeight=0,double arrowSize=0,double gap=0);
	double get_Gap();
	double set_Gap(double dimgap);
	__declspec(property(get=get_Gap,put=set_Gap)) double Gap;
	static double CalGap(double text_height);
protected:
	double m_fInnerSetTextSize;
	double gap;				//文字与基准线间隙
};
class DimStyleTable
{
public:
	static TMADRAWING_DIM_STYLE dimStyle;
	static TMADRAWING_DIM_STYLE dimStyle2;
	static TMADRAWING_DIM_STYLE dimStyle3;	//不强制将尺寸放于尺寸界线内侧
	static TMADRAWING_DIM_STYLE dimStyle4;	//文字放在尺寸线外侧 wht 15-04-28
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
//从RGB得到cad颜色索引值 
int GetNearestACI(COLORREF color);
//从CAD的颜色得到RGB
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
//handle用于记录该直线所代表构件的句柄,style代表该构件的绘制方式
//对于角钢构件：style:0.无信息;'X',表示X肢里铁绘制;-'x',表示X肢外铁绘制;'Y'表示Y肢里铁绘制;'y'表示Y肢外铁绘制;'Z'表示截面图
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
//三点生成一段样条曲线
void CreateAcadSpline(AcDbBlockTableRecord *pBlockTableRecord,
					f3dPoint startPoint, f3dPoint midPoint, f3dPoint endPoint);
void CreateAcadSpline(AcDbBlockTableRecord *pBlockTableRecord,AcGePoint3dArray fitPoints);
//根据圆弧起点，终点以及半径生成圆弧 wht 10-08-18
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,f3dPoint endPoint,THANDLE handle=NULL);
//根据圆心，起始点及扇形角生成一段圆弧
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,double sectorAngle,THANDLE handle=NULL);
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
							   f3dPoint center,f3dPoint startPoint,double sectorAngle,f3dPoint worknorm,THANDLE handle=NULL);
//多段线圆弧线 wht 10-12-21
AcDbObjectId CreateAcadPolyArcLine(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcLine,
								   double line_width,THANDLE handle=NULL);
//多段线圆弧线,输入起点、终点以及扇形角 wht 11-07-09
AcDbObjectId CreateAcadPolyArcLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,f3dPoint end,
								   double sector_angle,double line_width,THANDLE handle=NULL);
//绘制钢管剖面			//scale为圆弧半径与输入的两点间距离的比值
void CreateAcadSectionLine(AcDbBlockTableRecord *pBlockTableRecord,	//还有一个参数
					f3dPoint startPoint, f3dPoint endPoint, double scale=0.5);
//获得表格单元数据
BOOL GetGridKey(AcDbEntity* pPoint, GRID_DATA_STRU *grid_data);
BOOL GetPadText(AcDbPoint* pPadPoint, char *sPadText);
void TextStyle(AcDbTextStyleTableRecord *pTextStyleTblRec);
void setDimVar(AcDbDimStyleTableRecord *pDimStyleTblRcd,double txt_height=2.5,BOOL bInside=TRUE,BYTE cVerticalPosType=1);
AcDbObjectId 
DrawLine(f3dPoint start,f3dPoint end, THANDLE Handle=NULL);//角钢句柄
AcDbObjectId DrawCircle(double x0,double y0,double r);
//去掉bInside参数,dimStyle3专门用于[不强制将尺寸放于尺寸线之间]的标注
//如果使用同一标注样式，在修改该属性时会影响其他使用该样式的标注  wht 11-07-12
AcDbObjectId 
DimSize(AcDbBlockTableRecord *pBlocTableRecord, 
		f3dPoint start,f3dPoint end,//标注起始/终止点
		f3dPoint dimpos,const char *dimText,//尺寸线位置及内容
		AcDbObjectId dimStyleId,	//=NULL标注样式记录ID号
		int dimtype,double txt_height/*=0/*,BOOL bInside=TRUE*/);	//=1 垂直 =2水平  =3倾斜
AcDbObjectId 
DimAngleSize(AcDbBlockTableRecord *pBlockTableRecord,
		f3dPoint start,f3dPoint end,//标注起始/终止点
		f3dPoint dimpos,const char *dimText,//尺寸线位置及内容
		AcDbObjectId dimStyleId,	//标注样式记录ID号
		double angle,double txt_height=0/*,BOOL bInside=TRUE*/);
AcDbObjectId
DimAlignedSize(AcDbBlockTableRecord *pBlockTableRecord,
		AcDbObjectId dimStyleId,		//标注样式记录ID号
		f3dPoint start,f3dPoint end,	//标注起始/终止点
		f3dPoint dimpos,char *dimText="<>",double txt_height=0);	//尺寸线位置及内容
AcDbObjectId
DimAngle(AcDbBlockTableRecord *pBlockTableRecord,
		AcDbObjectId dimStyleId,		//标注样式记录ID号
		f3dPoint vertex,				//三点定角的角顶点
		f3dPoint start,f3dPoint end,	//标注起始/终止点
		f3dPoint dimpos,char *dimText="<>",	//尺寸线位置及内容
		double txt_height=0,double arrowsize=0);
AcDbObjectId 
DimDiameter(AcDbBlockTableRecord *pBlockTableRecord, 
		double* chordPos,double* farChordPos,//标注起始/终止点
		double leaderLen,char *dimText,
		AcDbObjectId dimStyleId,double txt_height=0);			   //标注样式记录ID号
AcDbObjectId DimRadial(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint center,
					   f3dPoint chordPoint,double leaderLen,char *dimText,
					   AcDbObjectId dimStyleId,double txt_height=0);//标注样式记录ID号
		
double DistOf2NodeByHandle(THANDLE handle1, THANDLE handle2,int style);
double ReviseTextAngleToHeadUpward(double angle,bool radiusAngleMode=true);
double TestDrawTextLength(const char* dimtext,double height,AcDbObjectId textStyleId);
AcDbObjectId 
DimText(AcDbBlockTableRecord *pBlockTableRecord,
		f3dPoint base,//标注起始/终止点
		const char *dimText,//尺寸线位置及内容
		AcDbObjectId textStyleId,	//=NULL标注样式记录ID号
		double height,double rotation,
		AcDb::TextHorzMode hMode=AcDb::kTextLeft,AcDb::TextVertMode vMode=AcDb::kTextBase,
		AcDbObjectId layerId=AcDbObjectId::kNull);
AcDbObjectId 
DimMText(AcDbBlockTableRecord *pBlockTableRecord,//块表记录指针
		char *dimText,//尺寸线位置及内容
		f3dPoint base,//标注起始/终止点
		double width,	//每行的最大宽
		double texheight,//文字高
		AcDbObjectId textStyleId,	//=NULL标注样式记录ID号
		AcDbMText::AttachmentPoint align_type,	//文字对齐方式
		AcDbMText::FlowDirection flowDirection,double angle=0);
#ifdef __COMMON_PART_INC_
void CalArcLineVertexList(f3dArcLine arcLine,fPtList &vertex_list);
AcDbObjectId AddEntityToDbs(AcDbEntity *pEntity,AcDbBlockTableRecord *pTemRecord=NULL);
#endif
BOOL InsertBlock(AcDbBlockTableRecord *pBlockTableRecord,
					f3dPoint insert_pos,char *blkname,
					double scale=1,double angle=0,
					long handle=0,AcDbObjectId *pOutEntId=NULL);
//生成新的DWG文件
void NewDwgFile(char *filename);

//根据输入的拾取点pickPt判断在那一侧绘制焊缝线 wht 12-08-31
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start,
						f3dPoint end,f3dPoint pickPt,double width=0.8,double height=0.7);
//绘制焊缝线--在直线左测
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint start, 
						f3dPoint end,double width=0.8,double height=0.7,BOOL bClockwise=FALSE);
//绘制焊缝线--在圆弧外侧
void DrawWeldLineSymbol(AcDbBlockTableRecord *pBlockTableRecord,f3dArcLine arcline,
						double width=0.8,double height=0.7);
//绘制剖视图符
//截断指定直线
//绘制破断线
void DrawTubeBreakLine(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint startPt,
					   f3dPoint endPt,double fBreakLen=1.4,double fLenCoefThreshold=8);
//极坐标转换
//批量打印

AcDbObjectId SearchBlock(const char *blkname);
//
void TransPtToCadUcs(f3dPoint& pt,UCS_STRU* pUcs=NULL,double scale=1);
void TransVtToCadUcs(f3dPoint& vect, UCS_STRU* pUcs = NULL);
//通过拉伸生成实体（多边体）
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,f3dPoint plane_norm,
								  f3dLine path,COLORREF crSolid,THANDLE handle=NULL);
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,
								  f3dLine path,COLORREF crSolid,THANDLE handle=NULL);
//通过拉伸生成实体（圆柱体）
AcDbObjectId CreateSolidByExtrude(AcDbBlockTableRecord *pBlockTableRecord,double fRadius,f3dPoint center,f3dPoint norm,
								  f3dLine path,COLORREF crSolid,THANDLE handle=NULL);
//通过旋转生成实体
AcDbObjectId CreateSolidByRevolve(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertex_list,f3dPoint axisPoint,
								  f3dPoint axisDir,double angle,COLORREF crSolid,THANDLE handle=NULL);
//主实体中减去子实体,ciType==0拉伸生成实体,ciType==1旋转生成实体
void SubtractSubSolid(AcDbObjectId mainSolidId,fPtList &vertex_list,f3dLine path,int ciType,COLORREF crSolid,
							AcDbBlockTableRecord *pTemRecord=NULL,double angle=2*Pi);
//通过拉伸或旋转生成实体,nManner==0拉伸生成实体,nManner==1旋转生成实体
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
//为指定的CAD对象插入扩展记录,不关闭pObj wht 11-05-26
AcDbObjectId InsertExtensionRecord(AcDbObject *pObj,long handle,int cls_id=0);
//根据指定坐标插入扩展记录 wht 11-05-26
AcDbObjectId InsertExtensionRecord(AcDbBlockTableRecord *pBlockTableRecord,
						   f3dPoint point,long handle,int cls_id=0);
AcDbObjectId CreateAcadHatch(AcDbBlockTableRecord *pBlockTableRecord,fPtList &vertexList,char *sHatchStyle,
							 double scale,double angle=0,COLORREF color=RGB(255,255,255));
//根据CAD实体ID更新scope wht 11-08-01
BOOL VerifyVertexByCADEntId(SCOPE_STRU &scope,AcDbObjectId entId,bool blFilterOnlyBomTblLine=false);
//生成GUID字符串
CXhChar200 CreateGuidStr();
//绘制表格
//from DrawTable.cpp
void DrawTable(AcDbBlockTableRecord *pBlockTableRecord, CFormatTbl *pTable, BOOL bPromptPos=TRUE,
	double scale=1, int nLocatePos=-1, const double text_margin=1.5);
void DrawTable(CXhTable *pTable, BOOL bPromptPos = TRUE, double scale = 1, int nLocatePos = -1,
	AcDbBlockTableRecord *pTemRecord = NULL);
#endif