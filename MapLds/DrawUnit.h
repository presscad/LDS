// DrawUnit.h: interface for the CDrawUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWUNIT_H__A9AC6807_68A8_4062_910D_1834A20E79DF__INCLUDED_)
#define AFX_DRAWUNIT_H__A9AC6807_68A8_4062_910D_1834A20E79DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ptr_list.h"
#include "ldsptr_list.h"
#include "XhCharString.h"
#include "RxTools.h"
#include "SysPara.h"
#include "DrawDim.h"
#include "MemberProperty.h"

//wbt:CTextFrame、CDimStyle、CTextOnlyDrawing、CSizeTextDim、CZhunXianTextDim、CKaiHeAngleDim已经移到DrawDim.h中
//CSizeAngleDim 移至DrawDim.h中 wht 18-08-23
struct PAGE_INDEX{
//protected:
	long _liPage;
public:
	BYTE ciPagesOfSerial;	//当前序号包含的总页数
	BYTE ciIndexInSerial;	//当前页在当前序号中的索引序号，0为起始索引序号
	WORD wiSerial;			//当前工艺卡的序号，0为起始序号
	long get_liPage();
	__declspec(property(get=get_liPage)) long liPage;			//当前页在当前输出中的总页码序号，0为起始页码序号
	PAGE_INDEX(){_liPage=wiSerial=ciIndexInSerial=0;ciPagesOfSerial=1;}
	void IncrementSerial();
	void IncrementLiPage();
};

class CDrawingRect
{
	friend class CDrawingRectLayout;
	friend class CDrawingRectArray;
	BOOL m_bLayout;			//内部优化布局时用于标识是否已经布置位置
	f3dPoint m_PrevTopLeft;	//内部优化布局时用于存储前一布局方案的左上角位置,以便于回滚布局
public:
	int m_iPageIndex;		//0为起始页码索引
	f3dPoint topLeft;		//包容矩形重新布局后的左上角坐标位置
	f3dPoint initTopLeft;	//在重新布局前视图包容矩形的初始左上角坐标，用于与重新布局后topLeft最终值一起确定包容矩形的偏移量
	double width,height;	//草图绘制矩形的宽度和高度
	//指向关联的CSectionViewDrawing对象
	//无关联对象时表示该矩形为固定矩形
	void *m_pDrawing;
	//
	CDrawingRect(){m_pDrawing=NULL;m_iPageIndex=0;m_bLayout=FALSE;}
	BOOL IsLayout(){return m_bLayout;}
};
struct SEGREC{SEGI iSeg;int nPageCount;};
class CDrawingRectLayout
{
	double m_fFrameWidth;	//边框宽度
	double m_fFrameHeight;	//边框高度
	double m_fBlkZoomcoef;	//原始图元尺寸/绘制图元尺寸
	bool IsDefiniteNotEnoughSpace(double width,double height,bool canExtendByX=false,bool canExtendByY=false);
	BOOL GetBlankRectTopLeft(double width,double height,f3dPoint *topLeft,double marginSpace,bool bSearchHori=true);
	BOOL GetBlankRectTopLeftTemp(CArray<double,double&>&startXArray,double width,double height,f3dPoint *topLeft);
	CArray<double,double&>m_arrRowHeight;	//
	//根据实际绘图区域大小重新设置绘图矩形区域的位置
	void ResetDrawingRectPosition();	
public:
	PRESET_ARRAY4<GEPOINT2D> arrOrgOfSubpages;
	bool m_bAutoZoomFixRect;//随动比例缩放固定位置矩形
	bool m_bPlaceFixRect;	//固定区域宽度高度仅用来占位，分页时便于调整主视图长度一致 wht 18-08-03
	double m_fFixRectWidth;	//固定位置矩形宽
	double m_fFixRectHeight;//固定位置矩形高
	f3dPoint m_FixRectTopLeft;
	DIMALIGN_TYPE fixRectAlign;	//固定位置矩形的定位方式
	CDrawingRectLayout();
	CArray<CDrawingRect,CDrawingRect&> drawRectArr;
	void Relayout(double h_to_w,double marginSpace);
	void Relayout3ViewsPartGroup(double h_to_w,double marginSpace);
	void AdjustTubeOneFrontViewPartGroupLayout(double marginSpace,double limitWidth=0,double maxHeight=0);
	bool RelayoutTubeCommonPartGroup(double h_to_w,double marginSpace,double limitWidth=0,double *pfPageCount=NULL,bool bZoomRelayout=false);
	void RelayoutPlateCommonPartGroup(double h_to_w,double marginSpace);
#ifdef __COMMON_PART_INC_
	void RelayoutSphereGroup(double h_to_w,double marginSpace);
#endif
	BOOL RelayoutTemp(double hight=1000,double width=10000000000000000);
	BOOL RelayoutPlateDraw(double marginSpace,DYN_ARRAY<SEGREC>& segarr);
	BOOL RelayoutDraw(double marginSpace);
	double GetFrameWidth(){return m_fFrameWidth;}
	double GetFrameHeight(){return m_fFrameHeight;}
	double GetRealDrawingHeight();
};

//用于记录图框情况
class CDrawingRectArray
{
	double m_fFrameWidth;	//边框宽度
	double m_fFrameHeight;	//边框高度
	double m_fMarginSpace;	//间距
	CArray<CDrawingRect,CDrawingRect&> drawRectArr;
public:
	CDrawingRectArray();
	CDrawingRectArray& operator = (CDrawingRectArray &drawRectArr);
	void CopyProperty(CDrawingRectArray *pDrawingRectArray);
	void InitDrawingRectArray(double width,double height,double marginSpace);
	int AddUsedDrawingRect(f2dPoint topLeft,double width,double height);
	void EmptyDrawRectArray(){drawRectArr.RemoveAll();}
	void RemoveDrawRectAt(int i){drawRectArr.RemoveAt(i);}
	int GetDrawRectArrSize(){return (int)drawRectArr.GetSize();}
	CDrawingRect GetDrawRectAt(int i){return drawRectArr.GetAt(i);}
	BOOL GetBlankRectTopLeft(double width,double height,f3dPoint *topLeft,BOOL bSearchHori=TRUE);
};

#ifndef __TSA_FILE_
//用于轮廓线消隐
extern LINEPARTSET OUTSIDEJGSETDRAW;
extern LINEPARTSET INSIDEJGSETDRAW;
void DrawAngleToMap(LINEPARTSET &partset,BOOL bInside,AcDbBlockTableRecord *pBlockTableRecord,
					CDrawCoordTrans dcs, BOOL bSetupDrawing,AcDbObjectId *pOutEntId=NULL);
class CPartDrawUnit
{
public:
	CLDSPart *pPart;
	f3dLine m_xRotateAxis;	//展开时的旋转轴线
	double m_fRotateAngle;	//民形时的旋转角度,弧度制
	CPartDrawUnit(){pPart=NULL;m_fRotateAngle=0;}
};
class CLineAngleDrawUnit : public CPartDrawUnit
{
public:
};
class CPlateDrawUnit : public CPartDrawUnit
{
public:
	CLDSPlate *pPlate;
	int front0_side1;	//正面螺栓还是侧面螺栓
	CPlateDrawUnit(){pPlate=NULL;front0_side1=0;}
};
extern ATOM_LIST<CPlateDrawUnit>PLATESETDRAW;
class CLsDrawUnit
{
public:
	CLDSBolt *pLs;
	BOOL bOriginal;		//是否正本螺栓
	bool bDimHoleDiameter;
	bool bDrawSideLine;	//平放螺栓绘制为等长直线
	int front0_side1;	//正面螺栓还是侧面螺栓
	f3dPoint pos;		//螺栓标注位置
	f3dPoint work_norm;	//绘图投影方向
	f3dLine spreadLine;	//展开线 wht 10-12-10
	double rotate_angle;//旋转角度大于0时需要旋转绘制
	CLsDrawUnit(){pLs=NULL;bOriginal=TRUE;front0_side1=0;rotate_angle=0;bDrawSideLine=false;bDimHoleDiameter=false;}
};
class CLsListDraw
{
	ATOM_LIST<CLsDrawUnit> drawls;
public:
	CLsListDraw(){;}
	CLsDrawUnit *AppendLsRef(CLDSBolt *pBolt, BOOL bOriginal,f3dPoint view_norm,
		f3dLine *pSpreadLine=NULL,double rotate_angle=0);
	BOOL DelLsRef(long handle);
	void EmptyDrawList();
	CLsDrawUnit* GetFirst();
	CLsDrawUnit* GetNext();
	void PushStack();
	void PopStack();
	void DrawToMap(AcDbBlockTableRecord *pBlockTableRecord,UCS_STRU &ucs,double scale);
	void InsertPadPoint(AcDbBlockTableRecord *pBlockTableRecord,CLsDrawUnit *pLsDraw,
		f3dPoint pos,AcDbObjectId *pOutEntId=NULL);
	//绘制单个螺栓
	void DrawBolt(AcDbBlockTableRecord *pBlockTableRecord,CLsDrawUnit *pLsDraw,CDrawCoordTrans dcs,AcDbObjectId *pOutEntId=NULL);
};
extern CLsListDraw LSDRAW;
class CLsSpaceDimUnit
{
public:
	long hFatherPlate;		//螺栓所属钢板句柄 wht 11-07-23
	long hStartLs,hEndLs;
	AcDbObjectId StartLsId,EndLsId;
	double dist;
	BOOL bCalDimAngle;
	double dim_angle;
	int nLsSpaceNum;		//包含的螺栓间距数
	BOOL bMergeDimLsSpace;	//合并标注特殊螺栓间距
	CLDSLinePart* GetDatumRod(BYTE dimvec0_plate1=0);
	CLsSpaceDimUnit(){hStartLs=hEndLs=0;dist=0.0;bCalDimAngle=TRUE;dim_angle=0;nLsSpaceNum=1;bMergeDimLsSpace=TRUE;}
};
extern ATOM_LIST<CLsSpaceDimUnit> LSSPACEDIM;
void DimLsSpaceToMap(AcDbBlockTableRecord *pBlockTableRecord,UCS_STRU &ucs);
//螺栓组标注单元
enum LSGROUP_POS{GROUP_MID=0,GROUP_START,GROUP_END};	//螺栓组在位置
class CLsGroupDimUnit
{
public:
	struct BOLTINFO{
	private:
		CLDSBolt *pBolt;
	public:
		long hBolt;
		f2dPoint pos2d;
		BOLTINFO(){hBolt=0;pBolt=NULL;}
		BOLTINFO(long boltHandle){hBolt=boltHandle;pBolt=NULL;}
		BOLTINFO(CLDSBolt *boltPtr){hBolt=boltPtr->handle;pBolt=boltPtr;}
		CLDSBolt *GetBolt()
		{
			if(pBolt==NULL)
				pBolt=(CLDSBolt*)Ta.FromPartHandle(hBolt,CLS_BOLT);
			return pBolt;
		}
		long GetBoltD()
		{
			if(pBolt==NULL)
				pBolt=(CLDSBolt*)Ta.FromPartHandle(hBolt,CLS_BOLT);
			if(pBolt)
				return pBolt->get_d();
			return 0;
		}
		long GetBoltL()
		{
			if(pBolt==NULL)
				pBolt=(CLDSBolt*)Ta.FromPartHandle(hBolt,CLS_BOLT);
			if(pBolt)
				return ftoi(pBolt->get_L());
			return 0;
		}
	};
private:
	long hDatumRod;
	long hDatumBolt;
	long d,L;	//螺栓标称直径及有效长,用以计算规格
	CHashList<BOLTINFO> hashBoltsByH;
	//
	CLDSBolt *pDatumBolt;
	CLDSLinePart *pDatumRod;
	//
	//BOOL m_bInitAxisXByRod;	//是否已经根据基准杆件初始化坐标系Ｘ轴
	//初始化螺栓组基准杆件
	void InitDatumRod(CLDSBolt *pBolt);
	//获取螺栓标注方向基准杆件
	static CLDSLinePart* GetDimVecDatumRod(CLDSBolt *pBolt);
public:
	UCS_STRU ucs;			//螺栓组坐标系
	SCOPE_STRU group_scope;	//螺栓组所在区域
	CLsGroupDimUnit()
	{
		hDatumRod=hDatumBolt=0;
		pDatumBolt=NULL;
		pDatumRod=NULL;
		hashBoltsByH.SetHashTableGrowSize(10);	//默认螺栓组增长
		d=L=0;
		//m_bHasGroupUcs=FALSE;
	}
	long GetBoltD(){return d;}
	long GetBoltL(){return L;}
	void SetDatumRod(CLDSLinePart *pRod);
	f3dPoint TransToLocalCoord(f3dPoint pos);
	void GetDimText(char *sDimText);
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
	//
	void Init(CLDSBolt *pBolt);			//初始化螺栓组信息
	bool Init(CLDSPlate *pPlate,BOLTSET &boltSet);
	bool IsPlateLsGroup();
	void AddBolt(BOLTINFO boltInfo);	//添加螺栓至螺栓组
	BOOL RemoveBolt(CLDSBolt *pBolt);	//移除指定螺栓,并返回是否移除成功
	BOOL IsIncludeBolt(BOLTINFO boltInfo);//判断螺栓是否属于当前组
	void CopyProperty(CLsGroupDimUnit *pLsGroupDim);
	CLDSBolt *GetDatumBolt();
	CLDSLinePart *GetDatumRod();
	BOLTINFO *GetFirstBoltInfo() { return hashBoltsByH.GetFirst(); }
	BOLTINFO *GetNxetBoltInfo() { return hashBoltsByH.GetNext(); }
	int GetBoltNum() { return hashBoltsByH.GetNodeNum(); }
	BOOL ContainsBolt(long hBolt) { return hashBoltsByH.GetValue(hBolt)!=NULL;}
	//判断螺栓组在基准杆件始端、终端还是中间,自动查找位置时用来判断查找方向
	LSGROUP_POS GetLsGroupPos(f3dPoint base_pos,AcDbLine *pDatumLine,double fScale);
	CXhChar500 GetBoltIdKey();

	static double fLsSpaceVerticalThreshold;	//螺栓间距水平方向阈值
	static double fLsSpaceHorizontalThreshold;	//螺栓间距竖直方向阈值
};
#endif

typedef struct
{
	double wide;    //
	double thick;
	char cMark;		//规格标识符
	char spec[50];	//规格
	int  N;			//此规格杆件数量
}SPEC_MARK;

class CDimPartSpec
{
public:
	CXhChar16 sPartNo;
	CXhChar16 sBackPartNo;
	CXhChar50 guige,sLength;
	long hPart;
	double text_angle;
	int cls_typeid; 
	long iNo;				//对应TSA节点编号
	f3dPoint searchDirect;	//对组合角钢子角钢应指定编号标注的搜索方向,默认空矢量方向表示不指定
	//
	BOOL bDimPartNoOnly;	//仅标注构件编号
	BOOL m_bDimRefLineOnly;	//只绘图引用线
	BOOL bReverseLsInfo;	//TSA出图使用
	f3dPoint base_pos,dim_pos;
	//新加变量 wht 10-12-03
	int  bAutoSearchDimPos;	//自动查找标注位置,无合适位置时要求用户指定,不启用时直接使用用户指定的标注位置
	BOOL bDimJgLength;		//标注角钢长度
	BOOL bDrawRefLine;		//是否绘制引出线
	double fPoleDimPosCoef;	//杆件标注位置系数 //LTMA 0.3  TAS 0.5
	//使用绘图脚本标注编号时使用
	double fBasePosOffsetX;	//数据点位置X坐标偏移量	wht 11-07-25
	double fBasePosOffsetY;	//数据点位置Y坐标偏移量
	double fDimPosOffsetX;	//标注位置X轴方向偏移量
	double fDimPosOffsetY;	//标注位置Y轴方向偏移量
	double fGuigeOffsetX;	//规格标注位置X轴方向偏移量
	double fGuigeOffsetY;	//规格标注位置Y轴方向偏移量
	//杆件方向与X轴的夹角，标注法兰及标准插板时用于调整编号沿杆件方向标注 wht 12-03-22
	double rod_angle;		//杆件方向与X轴的夹角
	f3dPoint rod_vec;		//杆件方向,绘制带角度腰圆矩形或矩形时使用
	int iMirMsg;			//0:X轴对称；1:Y轴对称;其余不对称处理
	long hMirPart;			//对称构件句柄，判断Y轴对称构件是左还是右时需要根据句柄找到位置 wht 12-08-19
	BOOL bLeftPart;			//TRUE:当前构件为左构件,对称构件为右构件 FALSE:反之 
	CDimPartSpec()
	{
		memset(this,'\0',sizeof(CDimPartSpec));
		bAutoSearchDimPos=TRUE;	//默认自动搜索构件标注位置
		bDimJgLength=sys.stru_map.bDimStruJgLen;
		rod_angle=0;
		iMirMsg=-1;
		hMirPart=0;
		bLeftPart=TRUE;
#if defined(__TSA_FILE_)
		fPoleDimPosCoef=0.5;
#else
		fPoleDimPosCoef=0.3;
#endif
	}
};
class CDimPartSpecList
{
	ATOM_LIST<CDimPartSpec> dimpart;
	int back_mir_style;	//0:X轴对称；1:Y轴对称;其余不对称处理
	CDimPartSpec *AppendNodeSpec(CLDSNode *pNode,f3dPoint dim_pos,BOOL bDimByAnalysisMode,BOOL bDrawRefLine);	
	CDimPartSpec *AppendLinePartSpec(CLDSLinePart *pLinePart,f3dPoint dim_pos,BOOL bDimByAnalysisMode,
									BOOL bDrawRefLine,double angle);
#ifndef __TSA_FILE_
	CDimPartSpec *AppendPlatePartSpec(CLDSPlate *pPlank,f3dPoint dim_pos,BOOL bDrawRefLine);
	CDimPartSpec *AppendParamPlatePartSpec(CLDSParamPlate *pPlate,f3dPoint dim_pos,BOOL bDrawRefLine);
#endif
	void DimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,CDimPartSpec* pPartSpec, CTextFrame* pTextFrame);
public:
	ATOM_LIST<SPEC_MARK> SpecList;	//简化规格时使用
public:
	CDimPartSpec *AppendPartSpec(CLDSDbObject *pDbObj,f3dPoint dim_pos,BOOL bDimByAnalysisMode,
								BOOL bDrawRefLine=FALSE,double angle=0);
	CDimPartSpecList() : m_bMergeNearSampePartNoDim(TRUE)
	{back_mir_style=-1;}
	void SetBackMirStyle(int AXISX0_AXISY1);
	int BackMirStyle();	//0:X轴对称；1:Y轴对称;其余不对称处理
	BOOL DelPartSpecDim(long handle);
	void EmptyPartSpecDim();
	CDimPartSpec* GetFirst();
	CDimPartSpec* GetNext();
	void PushStack();
	void PopStack();
	void DimToMap(AcDbBlockTableRecord *pBlockTableRecord,BOOL bDimByAnalysisMode=FALSE);
	// 合并位置近邻的同编号标注
	BOOL m_bMergeNearSampePartNoDim;
	// 分析并标注指定构件 wht 10-11-29
	void AnalyseAndDimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,
		CDimPartSpec *pPartSpec,BOOL bDimByAnalysisMode,bool bDimStartPartSpec=false);
#ifdef __SMART_DRAW_INC_
	//根据记录的数据标注构件明细
	void AutoDimPartSpec(AcDbBlockTableRecord *pBlockTableRecord,CDimPartSpec *pPartSpec,
		BOOL bDimByAnalysisMode,BOOL bSelectPos=FALSE,bool bDimStartPartSpec=false);
#endif
};

//球面坐标
//1.以XY平面为赤道面
//	Z>0表示北纬，纬度范围为(0~90)
//	Z<0表示南纬，纬度范围为(0~-90)
//2.以XZ平面X+为本初子午线即经度为0
//	Y+方向为东经(0~180)
//	Y-方向为西经(0~-180)
struct SPHERE_COORD{
	double longitude;	//经度
	double latitude;	//纬度
	//
	SPHERE_COORD(){memset(this,0,sizeof(SPHERE_COORD));}
	SPHERE_COORD(double fLongitude,double fLatitude)
	{
		longitude=fLongitude;
		latitude=fLatitude;
	}
	BOOL IsEqual(SPHERE_COORD sphereCoord);
	CXhChar200 ToDegreeString();	//角度制字符串
	CXhChar200 ToRadianString();	//弧度制字符串
	//直角坐标转换为经纬度坐标
	static SPHERE_COORD TransToSphereCoord(f3dPoint pt);
	//经纬度坐标转换为直角坐标
	static f3dPoint TransToRectangularCoord(SPHERE_COORD sphereCoord,double R);
};
/*直线与球求交点
  return value:
	0:直线与球无交点
	1:两交点重合为一个切点
	2:找到两个有效交点
	-1:直线或球定义有误
*/
int Int2dls(f3dPoint &inters1,f3dPoint &inters2,f3dLine line,double sphere_r);
//
extern CDimPartSpecList dimpartspec;
#endif // !defined(AFX_DRAWUNIT_H__A9AC6807_68A8_4062_910D_1834A20E79DF__INCLUDED_)
