#ifndef __PART_GROUP_DRAWING_H_
#define __PART_GROUP_DRAWING_H_

#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "PartLib.h"
#include "XhLdsLm.h"
#include "Buffer.h"
#include "XhCharString.h"
#include "DrawUnit.h"
#include "LocalDrawing.h"
#include "FormatTable.h"
#include "list.h"
#include "MemberProperty.h"
#include "RectLayout.h"
#include "DbEntContext.h"
#include "PlateDrawer.h"
#include "JgDrawing.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifndef __TSA_FILE_
class CLDSPart;
class CLDSPartGroup;
typedef struct tagPART_GROUP_CARD_INFO
{
	CXhChar16	m_sPartNo;		//件号
	CXhChar16   m_sSegStr;		//段号标识
	CXhChar16	m_sLength;		//长度
	CXhChar16	m_sMaterial;	//设计材质
	CXhChar16	m_sGuiGe;		//设计规格
	CXhChar16	m_sPartNum;		//单基数
	CXhChar16	m_sSumPartNum;	//总加工数=单基数*基数
	CXhChar16	m_sCutRoot;		//刨根
	CXhChar16	m_sCutBer;		//铲背
	int			m_nCutBerR;		//铲背圆弧半径mm,一般相联主材的角钢内弧值r+2
	CXhChar16	m_sCutAngSX;	//始端X肢切角	
	CXhChar16	m_sCutAngSY;	//始端Y肢切角
	CXhChar16	m_sCutAngEX;	//终端X肢切角
	CXhChar16	m_sCutAngEY;	//终端Y肢切角
	CXhChar16	m_sCutAngParaSX;//始端X肢切角参数显示
	CXhChar16	m_sCutAngParaSY;//始端Y肢切角参数显示
	CXhChar16	m_sCutAngParaEX;//终端X肢切角参数显示
	CXhChar16	m_sCutAngParaEY;//终端Y肢切角参数显示
	CXhChar16	m_sPushFlat;	//压扁
	CXhChar16	m_sWeld;		//焊接
	CXhChar16	m_sKaiJiao;		//开角
	CXhChar16	m_sHeJiao;		//合角
	CXhChar16	m_sKaiHeJiao;	//开合角具体度数
	CXhChar50	m_sNotes;		//备注
	CXhChar50	m_sFootNailPlate;//脚钉底座材料表
	int m_nPageNum,m_nPageIndex;//共 页,第 页
	int m_nTaNum;				//基数 
}PART_GROUP_CARD_INFO;

class CAttachPartsSection
{
public:
	double m_fStartX,m_fEndX;
	CAttachPartsSection(){m_fStartX = m_fEndX=0;}
};


/*typedef struct tagPARTGROUP_ANGULARDIM_STRU
{
	f3dPoint vertex;
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
}PARTGROUP_ANGULARDIM_STRU;*/

//扩展数据点用来保存构件相关信息
/*typedef struct tagDATAPOINT_STRU
{
	f3dPoint	dim_pos;	//标注位置
	f3dPoint	data_pos;	//数据位置
	char		sPartNo[16];//构件编号
	int			cur_layer;	//当前层
	long		hPart;		//相关构件句柄
	int			cls_id;		//相关构件类型
}DATAPOINT_STRU;*/
//CDrawingEntsContext 移至DbEntContext.h wht 18-08-23
class CSectionViewDrawing;
class CSectionDrawingEntsManager : public CDrawingEntsContext{
	CLDSPart* m_pRootPart;	//组焊件的基准构件
public:
	struct VERTICAL_PLATE{
		CLDSGeneralPlate *pPlate;
		f3dLine face_line[3][2];	//[0~2][0]为面
		double fParallelDist;		//平行线与钢板距离
		f3dLine parallel_line;		//基面平行线
		GEPOINT startBoltPos,endBoltPos;	//z>0表示未初始化
		BOOL bDimStart,bDimEnd;
		VERTICAL_PLATE(){pPlate=NULL; fParallelDist=0;startBoltPos.z=endBoltPos.z=1000;bDimStart=bDimEnd=TRUE;}
	};
	f3dLine tube_edge_line[2];	//钢管轮廓线，修订塔脚连接板与肋板外形时用到 wht 18-08-25
	CSectionViewDrawing* ValidSectDrawing();
	CHashList<VERTICAL_PLATE> m_hashVertPlateList;
	struct HORZ_PLATE{
		CLDSGeneralPlate *pPlate;
		ARRAY_LIST<f3dLine> edgeLineList;
	};
	CHashList<HORZ_PLATE> m_hashFootConnectPlateList;
	void UpdateVerticalPlateParallelLine(GELINE line);
	void UpdateVerticalPlateLocateBoltPos(GEPOINT bolt_pos);
public:
	CSectionDrawingEntsManager(CLDSPart* pRootPart=NULL,CSectionViewDrawing* pSectDrawing=NULL,STORE_MODE cStoreMode=STORE_ACAD){
		InitContext(pRootPart,pSectDrawing,cStoreMode);
	}
	CSectionDrawingEntsManager(CLDSPart* pRootPart,IDrawingCommand* pXeroCmd,STORE_MODE cStoreMode=STORE_ACAD){
		InitContext(pRootPart,pXeroCmd,cStoreMode);
	}
	CSectionDrawingEntsManager(CLDSPart* pRootPart,IDrawing* pXeroDrawing,STORE_MODE cStoreMode=STORE_ACAD){
		InitContext(pRootPart,pXeroDrawing,cStoreMode);
	}
	~CSectionDrawingEntsManager(){;}
	void InitContext(CLDSPart* pRootPart,CSectionViewDrawing* pSectDrawing,STORE_MODE cStoreMode=STORE_ACAD);
	void InitContext(CLDSPart* pRootPart,IDrawingCommand* pXeroCmd,STORE_MODE cStoreMode=STORE_ACAD);
	void InitContext(CLDSPart* pRootPart,IDrawing* pXeroDrawing,STORE_MODE cStoreMode=STORE_ACAD);
	void AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,double fDimR=0,char *dimText=NULL,double fDimAngleTextSize=3.0);
	void AppendAngularDim(CSizeAngleDim& angularDim);
	void AppendLine(f3dLine& line);
	void AppendLine(f3dArcLine& arcline);
	void AppendSpLine(fPtList& vertex_list);
	void AppendCircle(f3dCircle& circle);
	void AppendSizeTextDim(CSizeTextDim& sizeTextDim);
	void AppendTextDim(CTextOnlyDrawing& pureTextDim);
	//构件数据标识点
	void AppendDataPoint(CTextFrame& dataPoint);
	DATAPOINT_TAG* AppendDataPoineTag();
	virtual double ZoomToModelTextSize(double fSysFontSize);
	//绘制焊缝线
	void DrawWeldLineSymbol(f3dLine line,f3dPoint pickPt,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dPoint start,f3dPoint end,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dArcLine arcline,double width=0,double height=0);
	double TransToModelLength(double scr_draw_length);
	//钢管端面图的环向角标注基准线
	void AppendTubeBaseLine(BYTE iMode,CLDSLineTube* pBaseTube,f3dPoint arrowVec,f3dPoint arrowBtm,f3dPoint arrowHead);
	void AppendTubeTaCoreVecLine(f3dPoint vec,f3dPoint xBtm,f3dPoint xHead);

public:
	virtual bool IsSectionDrawing(){return true;}
	virtual bool IsBranchRod(CLDSLinePart* pBranchRod){
		if(m_pRootPart==NULL||!m_pRootPart->IsLinePart())
			return false;
		return pBranchRod!=m_pRootPart;
	}
	virtual CLDSPart* WeldRootPart(){return m_pRootPart;}
};
class CPlateDrawing{
	void AnalizePoleRay(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext);//分析钢板射线杆件
	void DimVerticalPlateLocateSize(CLDSPlate *pPlate,IDbEntsContext* pContext);	//标注立板定位尺寸
public:
	struct PLATE_DRAWING_STYLE{
		BYTE geomcenter0_left1_right2;	//信息标记点位置=0
		bool bDrawWeldLine;	//=TRUE绘制焊缝线标记
		bool bDimBoltSpace;		//标注螺栓间距
		bool bDimBoltEdgeSpace;	//标注螺栓边距
		bool bDimRayRodAngle;	//是否需要标注板的射线方向或角度
		bool bDrawInnerCircle;	//圆形底脚钢板的内圆弧在钢管外壁已绘制情况下，不再绘制
		bool bMandatoryDimLabel;//强制性标注编号
		bool bSpreadBendPlane;	//绘制火曲面螺栓和顶点时，按火曲线展开绘制（一般不需要展开，尤其对于构架梁组焊图绘制时，涉及检测线标注不需要展开) wjh-2017.10.24
		char ciDrawModeH0V1;
		BYTE cbDrawPlanes;
		short penStyle;	//对应原bCoplanar=TRUE时用虚线PS_DASH绘制
		short siMinFilterZ,siMaxFilterZ;	//绘制钢板截面图时轮廓点的Z坐标（绘图坐标系下）范围 wjh-2018.3.10
	}style;
	CPlateDrawing();
	BOOL CreatePlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL,int h0v1=-1);
	//变电构架横梁折叠板绘制需要单独处理
	BOOL CreateSubstationFoldPlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL);
	//变电构架顶梁八字火曲板绘制需要单独处理
	BOOL CreateSubstationSplayFoldPlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL);
};
struct ACAD_HATCH
{
	fPtList vertexList;
	CXhChar50 sHatchStyle;
	double fScale;
	double fAngle;
	COLORREF color;
	void ZoomScale(double zoomCoef);
	ACAD_HATCH(){fScale=fAngle=0; color=RGB(255,255,255);sHatchStyle.Copy("SOLID");fScale=1;}
};
struct RADIAL_ITEM{
	long hObj;
	BYTE ciObjType;			//射线方向的类型（目前保留用于将来拓展存储射线特殊类别的标识)
	bool bDualDirection;	//正反双向的射线
	double radAngle;
	GEPOINT rayVec,rayOrg;	//模型坐标系下坐标
public:
	RADIAL_ITEM(){radAngle=hObj=ciObjType=0;bDualDirection=false;}
};
class CDimAngularManager{
public:
	struct ANGULARDIM_ITEM{
		long hStartObj,hEndObj;
		double  fDimRadius;			//从顶点到标注圆弧的距离
		GEPOINT vTipPoint;			//标注角的顶点
		GEPOINT vStartVec,vEndVec;	//标注角的起始终止边（模型坐标系下坐标值）
		double degSectAngle;
	};
private:
	CXhSimpleList<ANGULARDIM_ITEM> m_xAngularDimList;
public:
	GEPOINT m_vNorm;
	GEPOINT m_vStartEdgeAxisVec;
	double m_fDimAngleTextSize;
public:
	CDimAngularManager(double fDimTextSize=2);
	~CDimAngularManager();
	void CreateAngularItems(const double* normal,const double* cStartEdgeAxisVec,const double* vTubeLenVec,RADIAL_ITEM* items,int count);
	ANGULARDIM_ITEM* EnumFirstDimItem(){return m_xAngularDimList.EnumObjectFirst();}
	ANGULARDIM_ITEM* EnumNextDimItem(){return m_xAngularDimList.EnumObjectNext();}
};
class CPartGroupDrawing;
struct PART_DIMnDRAW{
	long hPart,idView;
	char ciDrawModeH0V1;
	BYTE ciAttachPlateType;	//附属构件，处理时视同参数化板类型(如十字插板中立板、底脚肋板等)
	BYTE ciLeadLineMode;	//0.无引出线;1.引出线;2.自动判断引出线
	BYTE cbDrawPlanes;	//仅对钢板有效，0x01:绘制基面;0x02:绘制1号火曲面;0x04:绘制2号火曲面
	bool bDrawSketch;	//true:在hView视图中绘制;false:不绘制
	bool bDimPartNo;	//true:在hView视图中标注构件编号
	bool bMandatoryDimLabel;//强制性必须标注编号
	CLDSPart* pPart;	//hPart指向的构件对象
	GEPOINT vPreferDimFlowVec;	//构件编号的优先自动标注的搜索方向
	PART_DIMnDRAW();
};
class CSectionViewDrawing : public CXeroDrawingComponent
{
	friend class CPartGroupDrawing;
public:
	struct SEARCH_CELL{
		int xi,yj;
		double weight;//搜索优先权重，越小越优先
		static double fHoriPriorCoef;
		static double fVertPriorCoef;
	public:
		SEARCH_CELL(int x=0,int y=0);
		double CalPriorWeight(int x,int y);
	};
	struct SECT_SYMBOL_ENT
	{
		CTextOnlyDrawing *pText;		//视图名称
		CTextOnlyDrawing *pScaleDimText;//视图绘制比例
		IDbPolyline* pLine1;	//粗线
		IDbLine* pLine2;		//细线
		SECT_SYMBOL_ENT(){pText=pScaleDimText=NULL;pLine1=NULL;pLine2=NULL;}
	} sectionEnt;
	IDbLine *m_pArrowLine1,*m_pArrowLine2;				//普通箭头
	IDbPolyline *m_pArrowPolyLine1,*m_pArrowPolyLine2;	//特殊箭头
	//布局之后动态确认的绘制相关的参数
	DRAWRECT drawingRect;
protected:
	UCS_STRU cs;
	IModel *m_pTower;
	CLDSPart *_pRootPart;
	//CLDSPartGroup *m_pPartGroup;
	CSectionView *m_pSectionView;
	BOOL m_bSamePartNoDimOnce;				//同一件号仅标注一次,默认启用,特殊视图(如塔脚板俯视图)时需要关闭 wht 15-04-09
	double m_fUser2ScrDrawScale;			//绘图比例
	bool m_bFinalDrawing;	//是否已执行了RelayoutDrawing()
	//草图轮廓线
	ATOM_LIST<f3dCircle>cirList;			//圆
	ATOM_LIST<f3dArcLine>edgeList;			//边线
	ATOM_LIST<ACAD_HATCH>hatchList;			//填充
	//草图中的各项标注
	ATOM_LIST<CTextFrame>dataPointList;			//构件数据点
	ATOM_LIST<CSizeAngleDim>angularDimList;		//角度标注
	ATOM_LIST<CSizeTextDim>spaceDimList;		//相对尺寸
	ATOM_LIST<CTextOnlyDrawing>pureTextDimList;	//绝对尺寸
	CHashList<DATAPOINT_TAG> m_xDataPointTagHash;
	//void AnalizePoleRay(CLDSPlate *pPlate,CDrawCoordTrans& dcs);//分析钢板射线杆件
	PART_DIMnDRAW* RegisterPartDimDraw(CLDSPart *pPart,int idPartClsType,char ciDrawModeH0V1,bool bMandatoryDimLabel=false);
	BOOL CreatePlateDrawing(CLDSPlate *pPlate,CDrawCoordTrans& dcs,BOOL bCoplanar=TRUE,BOOL bDrawWeldLineByWeldProp=TRUE,BYTE geomcenter0_left1_right2=0,int h0v1=-1);
	BOOL CreateParamPlateDrawing(CLDSParamPlate *pParamPlate,CDrawCoordTrans& dcs,BOOL bCoplanar=TRUE,BOOL bDrawWeldLineByWeldProp=TRUE,int h0v1=-1);
	/*static*/ BOOL CreateLineAngleDrawing(CLDSLineAngle *pLineAngle,CDrawCoordTrans& dcs);
	//生成角钢上焊接的角钢肋板的轮廓线
	void CreateJgRibPlateEdge(CLDSLineAngle *pJg,f3dLine ber_line,f3dPoint wing_vec,BOOL bInsideJg);
	/*static*/ BOOL CreateLineTubeDrawing(CLDSLineTube *pLineTube,CDrawCoordTrans& dcs,BOOL bDrawWeldLineByWeldProp=TRUE);
	BOOL CreateLineSlotDrawing(CLDSLineSlot *pLineSlot,CDrawCoordTrans& dcs);
	BOOL CreateCutTubeDrawing(CLDSLineTube *pLineTube,BOOL bCoplanar=TRUE);		//球节点组焊件中的钢管示意图
	BOOL CreateSphereDrawing(CLDSSphere* pSphere);			//球节点阻焊件中的球体示意图
	BOOL CreateSpecialSectionView();	//生成特殊的视图 wht 10-08-13
	void DimCheckLine();	//标注检测线
	void DimFlCheckLine();	//在球节点组焊件中添加法兰检测线
	void DimAssemblyPos();	//在球节点组焊件中添加装配点标记
	void DimAngle();		//标注角度
	void AppendDefaultAngularDimension(CAngularDimension* pAngular,CLDSLineTube* pDatumLineTube,
		const double* vWeldRoadDirect,BOOL bIncludeDatumTube,CLDSLineTube *pJointTube);
	void DimSectionViewSymbol();//标注剖视图符
	bool DimSectionViewSymbol2();
	CSizeTextDim* NewSizeDimByLocatePoint(CSizeTextDim *pPrevSizeDim,int iOrderType,
		CPartGroupLocatePoint *pPrevLocate,CPartGroupLocatePoint *pLocate);
	void StatSizeDimsByLocatePoint(CPartGroupLocatePoint locatePointArr[],int num,int iOrderType);
	void DimLocatePoint(CLDSLinePart *pDatumPart);	//标注标定点位置
	void ResetPosition();	//重置坐标原点
	//为了提高效率输入的竖直搜索方向必须是单位化向量,假定basePos位于矩形中心
	bool IsHasIntersObj(f3dPoint rgnVert[4],void *pExcusiveSelfDrawObj=NULL);
	bool SearchNoneIntersPos(f3dPoint rgnVert[4],const double* vcVertSearchVec,
		double maxHroiSearchStep=5.0,int maxVertSearchStep=5,bool bEnableHoriSearch=true,bool bSearchVertPrefer=true,bool bDuoDirect=true);
	bool SearchNoneIntersPosEx(f3dPoint rgnVert[4],const double* vcSearchFlowDirection,
		double maxHoriSearchDist=5.0,double maxVertSearchDist=5,double fHoriPriorCoef=0.5,double fVertPriorCoef=0.5,
		char cbDualDirectMode=0x0f,void *pExcusiveSelfDrawObj=NULL);
	void RelayoutDrawing(double fSpecZoomCoef=0.0);	//fSpecZoomCoef<=0.0表示根据m_fUser2ScrScale计算 wjh-2018.3.7
	void ClearDimItemFlag();
public:
	CSectionDrawingEntsManager m_xEntManager;
	CLDSPartGroup *m_pPartGroup;
	//剖视图符标注位置计算
	struct SECTSYMBOLDIM{
		GEPOINT xLineStart,xLineEnd,xPrjTipPoint;
		GEPOINT xTextDimPos;
		GEPOINT vFlowDirect,vVertDirect;
		double fSymbolFlowAngle;	//弧度制
		int hAcadMode,vAcadMode;	//ACAD标注时的文本对齐方式
		SECTSYMBOLDIM();
	};
	static bool CalSectViewSymbolDimPos(CTextOnlyDrawing* pSectSymbol,
		f3dPoint rgnVert[4],double fSymbolFontSize=2.0,SECTSYMBOLDIM* pxDimPos=NULL);

	MASKFLAG drawStyleFlag;	//组焊图绘制风格选项
	struct FL_SECT_AXIS_OFFSET{
		short siMinFilterZ,siMaxFilterZ;	//绘制钢板截面图时轮廓点的Z坐标（绘图坐标系下）范围 wjh-2018.3.10
		short siDistToUpper,siDistToBottom;
		GEPOINT vxTubeStdUpperAxisVec;		//模型坐标系下的钢管单位化轴向（朝当前视图Z轴正向一侧）
		FL_SECT_AXIS_OFFSET(){siMinFilterZ=siMaxFilterZ=siDistToUpper=siDistToBottom=0;}
	}xFlSectNormalOffset;

	READONLY_PROPERTY(CPartGroupDrawing*,m_pPartGroupDrawing);
	GET(m_pPartGroupDrawing){return (CPartGroupDrawing*)m_pParentDrawing;}
	//CPartGroupDrawing *m_pPartGroupDrawing;	//仅用于长度方向压缩用
	READONLY_PROPERTY(CLDSPart*,m_pRootPart);
	GET(m_pRootPart){return _pRootPart;}//CLDSPart *m_pRootPart;

	CSectionViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionViewDrawing();
	virtual UCS_STRU UpdateDCS();	//参照宿视图生成附属视图的绘图坐标系
	BOOL GetDCS(long hPart,CDrawCoordTrans &dcs);
	virtual void FinalDrawing();
	virtual void ZoomDrawing(double fZoomCoef=0);	//0表示保持原绘图比例不变，否则表示对现状进行比例缩放绘制 wjh-2018.3.7
	//添加角度标注线 标注半径默认为100 主要用来表示方向
	void AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,double fDimR=0,char *dimText=NULL);
	bool CheckSameAngularDim(double* vertex,double* start_vec,double* end_vec);
	//绘制构件轮廓
	/*static*/ void CreatePartSketch(CSectionView::PART_ITEM* pPartItem,BOOL bCoplanar=TRUE);
	//绘制靠模角钢的简易视图
	BOOL IsPartGroupHasKmAngleView();
	void CreateKaoMeAngleSketch(CKaoMeAngleSectionView* pKmAngleView);
	//完善钢管塔脚构件简图 wht 18-08-24
	// 1>. 绘制肋板图
	// 2>. 修正连接板与底板严丝合缝
	void CompleteTubeTowerFootViewSketch();
	//绘制焊缝线
	void DrawWeldLineSymbol(f3dPoint start,f3dPoint end,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dArcLine arcline,double width=0,double height=0);
	double TransToModelLength(double scr_draw_length){return scr_draw_length*m_fUser2ScrDrawScale;}
	virtual double ZoomToModelTextSize(double fSysFontSize);	//fSysFontSize是系统设定的字体高度，返回值是考虑到绘图比例后的模型尺寸范畴的字体高度　wjh-2017.11.9
	virtual FRAMESIZE ZoomToModelTextSize(FRAMESIZE& framesize);	//fSysFontSize是系统设定的字体高度，返回值是考虑到绘图比例后的模型尺寸范畴的字体高度　wjh-2017.11.9
	virtual BOOL CreateSectionViewDrawing(CSectionView *pSectionView,IModel *pTower,CLDSPartGroup *pPartGroup,bool bFinalDrawing=true);		//生成视图草图
	BOOL CreateSphereSectionViewDrawing(CSectionView* pSectionVIew,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);		//生成球节点组焊件视图草图
	virtual f2dRect GetDrawRegion(bool bIncSectSymbol=false);	//区域是否包括剖视图底部的图符及横线 wjh-2018.8.1
	double GetInternalDrawingScale() const { return m_fUser2ScrDrawScale; }
	__declspec( property(get=GetInternalDrawingScale)) double dfScaleOfModel2Draw;
	CSectionView* GetSectionView(){return m_pSectionView;}
	__declspec( property(get=GetSectionView)) CSectionView* pSectionView;
	UCS_STRU GetCS() {return cs;}	//返回绘图坐标系

	void AppendAngularDim(CSizeAngleDim& angularDim){angularDimList.append(angularDim);}
	void AppendLine(f3dLine& line);
	void AppendLine(f3dArcLine& arcline){edgeList.append(arcline);}
	void AppendCircle(f3dCircle& circle){cirList.append(circle);}
	void AppendSizeTextDim(CSizeTextDim& sizeTextDim){spaceDimList.append(sizeTextDim);}
	//添加尺寸标注
	CSizeTextDim* AppendSizeTextDim(const double* dimStartCoords,const double* dimEndCoords,const double* dimPosCoords,
		double fDist,double angle,BOOL bAllowRelayout=FALSE,BOOL bCanTextOutter=FALSE){
		AppendSizeTextDim(CSizeTextDim(dimStartCoords,dimEndCoords,dimPosCoords,fDist,angle,bAllowRelayout,bCanTextOutter));
		return spaceDimList.GetCursor();
	}
	void AppendTextDim(CTextOnlyDrawing& pureTextDim){pureTextDimList.append(pureTextDim);}
	void AppendDataPoint(CTextFrame& dataPoint){dataPointList.append(dataPoint);}
	DATAPOINT_TAG* AppendDataPoineTag(){
		DATAPOINT_TAG* pTag=m_xDataPointTagHash.Add(0);
		pTag->key_tag=m_xDataPointTagHash.GetCursorKey();
		return pTag;
	}
	ATOM_LIST<f3dCircle> *GetLsCirList() {return &cirList;}
	ATOM_LIST<f3dArcLine> *GetEdgeLineList() {return &edgeList;}
	ATOM_LIST<ACAD_HATCH> *GetHatchList() {return &hatchList;}
	ATOM_LIST<CSizeTextDim> *GetSpaceDimList() {return &spaceDimList;}
	ATOM_LIST<CTextOnlyDrawing> *GetPureTextDimList() {return &pureTextDimList;}
	ATOM_LIST<CTextFrame> *GetDataPointDimList() {return &dataPointList;}
	ATOM_LIST<CSizeAngleDim> *GetAngularDimList() {return &angularDimList;}
public:
	struct TUBE_BASELINE{
		BYTE m_iDraw;	//0.不绘制 1.三角箭头 2.标准箭头
		double m_fArrowH;	//箭头长度
		f3dPoint m_xArrowBtm;
		f3dPoint m_xArrowHead;
		f3dPoint m_xArrowStdVec;
		f3dPoint m_xTaCoreVec;	//塔心朝向
		f3dPoint m_xTaCoreVecHead;
		f3dPoint m_xTaCoreVecBtm;
		CLDSLineTube* m_pBaseTube;
		TUBE_BASELINE(){m_iDraw=1;m_fArrowH=20;m_pBaseTube=NULL;}
	}tube_base_line;
protected:
	void CreateTubeFeatureLine(SCOPE_STRU& region);
	void CreateDimAngleFeatureLine(SCOPE_STRU& region);
	GEPOINT GetFeatureLineTipPoint(const double* vRaylineVec,bool bAlongVec=true);
	bool CreateProcessAidedPlate(SCOPE_STRU& region);	//生成辅助工装卡板
public:
	static const int CLS_BASIC_VIEW = 0;
	static const int CLS_SLAVE_VIEW = 1;
	static const int CLS_FL_VIEW	= 2;
	static const int CLS_BOM_VIEW   = 3;
	static const int CLS_JOINT_VIEW = 4;
	static const int CLS_STDX_PLATES_VIEW = 5;
	static const int CLS_ANGLE_FOOT_VIEW = 6;
	static const int CLS_KM_ANGLE_VIEW = 7;
	static const int CLS_ANGLE_SPREAD_VIEW = 8;
	static const int CLS_BRIEFTUBE_VIEW = 9;	//两端插板或法兰的钢管标准简图
	static const int CLS_TUBE_ENDPLATE_VIEW = 10;	//钢管端板视图
};
class CSectionBriefTubeDrawing : public CSectionViewDrawing
{
	CLDSGeneralPlate *pStartPlate,*pEndPlate;
public:
	CSectionBriefTubeDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionBriefTubeDrawing(){;}
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//生成视图草图
};
class CSectionSlaveDrawing : public CSectionViewDrawing
{
public:
	CSectionSlaveDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionSlaveDrawing(){;}
	virtual UCS_STRU UpdateDCS();	//参照宿视图生成附属视图的绘图坐标系
};
class CSectionFlDrawing : public CSectionSlaveDrawing
{
	CLDSLineTube* m_pDrawTube;
	CLDSParamPlate* m_pDrawFL;
public:
	CSectionFlDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionFlDrawing(){;}
	virtual BOOL CreateSectionViewDrawing(CSectionView *pSectionView,IModel *pTower,CLDSPartGroup *pPartGroup,bool bFinalDrawing=true);	//生成视图草图
	virtual void FinalDrawing();
};
class CSectionJointDrawing : public CSectionViewDrawing
{
	GEPOINT m_vAngularJointPlaneNorm;
	CLDSLineTube* m_pRootTube,*m_pJointTube;
public:
	CSectionJointDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionJointDrawing(){;}
	READONLY_PROPERTY(CLDSLineTube*,pRootTube);
	GET(pRootTube){return m_pRootTube;}
	READONLY_PROPERTY(CLDSLineTube*,pJointTube);
	GET(pJointTube){return m_pJointTube;}
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//生成视图草图
	virtual void FinalDrawing();
};
class CSectionStdXPlatesDrawing : public CSectionViewDrawing
{
	STDX_PLATES_PARTGROUP m_xParam;
	BOOL CreateFrontViewDrawing();
	BOOL CreateSideViewDrawing();
	BOOL CreateTopViewDrawing();
	void DimViewHorizontalLsSpace(CDrawCoordTrans &dcs,BOOL bFrontView,const double* pxRayOrg);
	void DimViewVerticalLsSpace(CDrawCoordTrans &dcs,BOOL bFrontView,const double* pxRayOrg);
	void DrawLineAngleSection(f3dPoint ber_pos,f3dPoint x_vec_wing_pos,f3dPoint y_vec_wing_pos,double thick);
	void DrawSectionViewSymbol(CDrawCoordTrans &dcs);
public:
	static const BYTE WELDLINE_FRONT_ZZ = 0x01;		//前侧立板Z轴正方向有焊缝
	static const BYTE WELDLINE_FRONT_ZF = 0x02;		//前侧立板Z轴负方向有焊缝
	static const BYTE WELDLINE_BACK_ZZ  = 0x04;		//后侧立板Z轴正方向有焊缝
	static const BYTE WELDLINE_BACK_ZF  = 0x08;		//后侧立板Z轴负方向有焊缝
	BYTE m_cWeldLinePos;
	BYTE m_cKmAngle1Pos,m_cKmAngle2Pos;				//记录需要绘制靠模角钢的位置
public:
	CSectionStdXPlatesDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionStdXPlatesDrawing(){;}
	//提取十字焊接板处各象限中心角焊缝坡口类型,主要用于进行十字焊接截面图坡口标注　wjh-2015.4.24
	static BYTE InitWeldLinePos(CLDSGeneralPlate* platePtrArr[3],CLDSLineAngle* arrAnglePtr[4],BYTE *pcKmAngle1Pos,BYTE *pcKmAngle2Pos);
	static void DimPlateLsSpace(CDrawCoordTrans &dcs,CLDSGeneralPlate *pPlate,ATOM_LIST<CSizeTextDim> &spaceDimList);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//生成视图草图
	virtual void FinalDrawing();
};

class CSectionAngleFootDrawing : public CSectionViewDrawing
{
	ANGLE_FOOT_PARTGROUP m_xParam,m_xDownParam;
	BOOL CreateFrontViewDrawing();
	BOOL CreateSideViewDrawing();
	BOOL CreateTopViewDrawing();
	void DimFootPlateLsSpaceAndShoePlatePos(f3dPoint foot_center,GEPOINT plateDimPosArr[4]);
	BOOL DrawVerticalShoePlate(CLDSGeneralPlate *pPlate,f3dLine inters_line,f3dPoint base_plate_center,
							   CXhPtrSet<ACAD_HATCH> &hatchSet,GEPOINT &key_pt_for_dim);
	void DrawSectionViewSymbol(CDrawCoordTrans &dcs);
public:
	static const BYTE WELDLINE_FRONT_ZZ = 0x01;		//前侧立板Z轴正方向有焊缝
	static const BYTE WELDLINE_FRONT_ZF = 0x02;		//前侧立板Z轴负方向有焊缝
	static const BYTE WELDLINE_BACK_ZZ  = 0x04;		//后侧立板Z轴正方向有焊缝
	static const BYTE WELDLINE_BACK_ZF  = 0x08;		//后侧立板Z轴负方向有焊缝
	BYTE m_cWeldLinePos;
	BYTE m_cKmAngle1Pos,m_cKmAngle2Pos;				//记录需要绘制靠模角钢的位置
public:
	CSectionAngleFootDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionAngleFootDrawing(){;}
	bool IsHoofViewDrawing(){return m_xDownParam.m_pFootPlate!=NULL;}
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//生成视图草图
	virtual void FinalDrawing();
};
typedef struct tagSPHEREPOINT
{
	long hTube;					//钢管句柄
	CXhChar16 sSonTag;			//所属子视图
	SPHERE_COORD  intersPt1;	//交点1
	SPHERE_COORD  intersPt2;	//交点2
	double fEdgeDist;			//球心到法兰外端面距离

}SPHERE_POINT;
class CPartGroupTblDrawing : public CSectionViewDrawing
{
public:
	static const int PARTGROUP_BOM		= 1;	//组焊件明细表
	static const int SPHEREINTERS_TBL	= 2;	//装配点信息表
public:
	int m_nTableType;
	CFormatTbl m_xPartTbl;
	CHashList<SPHERE_POINT> m_pointInfoHash;
protected:
	void InitSpherePartGroupInsters();
	void CreatePartGroupBom(double scale);		//生成组焊件明细表
	void CreateSphereIntersTbl(double scale);	//生成球节点组焊件装配点信息表
public:
	CPartGroupTblDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	~CPartGroupTblDrawing();
	void CreatePartGroupTable(double scale);
	double GetTblWidth(double zoomscale=1.0);
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
};
class CSectionKmAngleViewDrawing : public CSectionViewDrawing
{
public:
	CKaoMeAngle m_xKaoMeAngle;
	static void InitKaoMeAngle(CKaoMeAngle &kaoMeAngle,CLDSLineAngle *pAngle,long* boltHandleArr,int nBoltCount,CLDSLineAngle *pCoupleAngle=NULL);
public:
	CSectionKmAngleViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionKmAngleViewDrawing(){;}
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//生成视图草图
};
class CSectionAngleViewDrawing : public CSectionViewDrawing
{
public:
	CLineAngleDrawing m_xAngleDrawing;
public:
	CSectionAngleViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionAngleViewDrawing(){;}
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//生成视图草图
};
class CSectionTubeEndPlateViewDrawing : public CSectionViewDrawing
{
	CPlateDrawer plateDrawer;
	CSlaveSectionView m_tempView;
public:
	CLDSParamPlate *m_pParamPlate;
	CSectionTubeEndPlateViewDrawing(int id=0,IDrawing* pParentDrawing=NULL);
	virtual ~CSectionTubeEndPlateViewDrawing(){;}
	f2dRect GetDrawRegion(bool bIncSectSymbol=false);
	virtual BOOL CreateSectionViewDrawing(CSectionView* pSectionView,IModel* pTower,CLDSPartGroup* pPartGroup,bool bFinalDrawing=true);	//生成视图草图
};
class CPartGroupDrawing : public CXeroDrawing
{
	friend class CPartGroupRectLayout;
	friend class CPartGroupDrawingLayoutOper;

	IModel *m_pTower;
	CLDSPartGroup *m_pPartGroup;
	CLDSPart *m_pRootPart;
	PARTSET SonParts;
	ATOM_LIST<CAttachPartsSection> sectionList;
	UCS_STRU cs;	//drawing coordinate system 绘图坐标系
	double AnalyzeLenModulate();
	//组焊件利用自动调整长度画法时的尺寸换算
	//fromReal=TRUE:输入实际长度,输出显示长度
	//fromReal=FALSE:输入显示长度,输出实际长度
	double LenCompressTrans(double len,BOOL fromReal=TRUE,double zoomThreshold=0.0);
	double m_fMaxDrawingLength;
	double m_fMaxDrawingHeight;
protected:
	CDrawSysPara::WELDDRAWING xDrawSet;
	bool m_bAutoLayoutViewIgnoreScale;//不考虑指定绘图比例自动布局各组焊子视图(可能重新计算绘图比例)
	double m_fRealToDraw;//基础绘图(Y轴向或钢板)比例尺＝实际尺寸/绘图尺寸，如1:20时，fRealToDraw=20
	double m_fSectViewScaleRealToDraw;
	double m_fMajorRodViewScaleRealToDraw;
	void IntelliCalDrawScale(CHashList<double>* pHashScaleOfView);	//智能计算横截面图及杆件主视图的绘图比例
	void InitDrawSetting();
	struct RESETSYSPARA_LIFEOBJ{
		RESETSYSPARA_LIFEOBJ(int mode=0);
		~RESETSYSPARA_LIFEOBJ();
	protected:
		bool bDrawWeldLineSymbol;
		bool bZoomFontSizeWithDrawScale;
	};
public:
	double get_fRealToDraw(){return m_fRealToDraw;}
	__declspec(property(get=get_fRealToDraw)) double scaleOfRealToDraw;
	CHashStrList<PART_DIMnDRAW*> hashDimLabels;
	PART_DIMnDRAW* RegisterPartDimDraw(long idView,long hPart);
	bool StatSectionViewDimDraws(CSectionView* pView);
	PART_DIMnDRAW* GetPartDimDraw(long idView,long hPart){return hashDimDrawParts.GetValue(idView,hPart);}
	//double AutoCalUser2ScrDrawScale();
	double GetMaxDrawingLength() { return m_fMaxDrawingLength; }
	double GetMaxDrawingHeight() { return m_fMaxDrawingHeight; }
private:
	CMapList<PART_DIMnDRAW> hashDimDrawParts;
public:
	MASKFLAG drawStyleFlag;	//组焊图绘制风格选项
	static DWORD CPartGroupDrawing::DimPartNo;				//自动标注构件编号
	static DWORD CPartGroupDrawing::LayoutViews;			//自动组合组焊件视图
	static DWORD CPartGroupDrawing::DimAbsoluteDist;		//标注定位点绝对尺寸
	static DWORD CPartGroupDrawing::DimPlateBetweenAngle;	//标注角度线
	static DWORD CPartGroupDrawing::DimWeldPosLocateLine;	//标注焊接前定位线
	static DWORD CPartGroupDrawing::DimWeldPosCheckLine;	//标注焊接后检测线
	static DWORD CPartGroupDrawing::DimPlateRayLineAngle;	//标注钢板射线以及射线与基准线之间的夹角
	static DWORD CPartGroupDrawing::ModulatePartGroupLen;	//自动压缩组焊图长度
	static DWORD CPartGroupDrawing::CreatePartGroupBOM;		//生成构件明细表
	static DWORD CPartGroupDrawing::DispPartGroupNo;		//组焊件编号
	static DWORD CPartGroupDrawing::DispPartGroupNum;		//组焊件数量
	static DWORD CPartGroupDrawing::DispPartNo;				//构件编号
	static DWORD CPartGroupDrawing::DispPartNum;			//构件数量
	static DWORD CPartGroupDrawing::DispPartGuiGe;			//构件规格
	static DWORD CPartGroupDrawing::DispPartLen;			//构件长度
	static DWORD CPartGroupDrawing::DispPartWeight;			//构件重量以及同一编号构件重量小计 总重
	static DWORD CPartGroupDrawing::DispNotes;				//备注
	static DWORD CPartGroupDrawing::DispInternal;			//焊缝级别－内在
	static DWORD CPartGroupDrawing::DispAppearance;			//焊缝级别－外观
public:
	//ATOM_LIST<CSectionViewDrawing> viewDrawingList;
	CPartGroupDrawing(void);
	~CPartGroupDrawing(void);
	UCS_STRU GetCS() {return cs;}	//返回绘图坐标系
	double TransCoordX(double real_x);
	bool SelectFootPartGroupView(CSectionView *pSectionView);	//生成塔脚板组焊图 wht 10-01-14
	CSectionTubeEndPlateViewDrawing* CreateTubeEndPlateViewDrawing(CLDSParamPlate *pPlate,double scaleOfReal2Draw=0);
	CSectionViewDrawing* CreateSectionViewDrawing(CSectionView *pSectionView,double scaleOfReal2Draw=0);
	bool CreatePartGroupDrawing();
	f2dRect GetDrawRegion(double PART_MAP_SCALE=1.0);
	bool InitPartGroupDrawing(CLDSPartGroup *pPartGroup,double maxDrawingLength,double maxDrawingHeight);
	int Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr);
	//插入钢管端头插板零件图
	BYTE InitEndInsertPlateDrawing(CSectionTubeEndPlateViewDrawing *pStartDrawing,CSectionTubeEndPlateViewDrawing *pEndDrawing);
};
#include "LifeObj.h"
struct TAG_INFO_PART_DRAWCMD_TUBE_SECT;
struct TAG_INFO_PART_DRAWCMD_FL_FLAT;
class CStruDrawingCommand;
struct TAG_INFO_PART_DRAWCMD : public TAG_INFO<CStruDrawingCommand>{
public:
	long hPart;
	TAG_INFO_PART_DRAWCMD(){hPart=0;}
	virtual ~TAG_INFO_PART_DRAWCMD(){DetachObject();}
	//标签对象类型转换
	TAG_INFO_PART_DRAWCMD_FL_FLAT* FlatFL();
	TAG_INFO_PART_DRAWCMD_TUBE_SECT* TubeSect();
public:
	static const BYTE PART_TUBE_SECT= 1;
	static const BYTE PART_FL_FLAT	= 2;
};
struct TAG_INFO_PART_DRAWCMD_TUBE_SECT : public TAG_INFO_PART_DRAWCMD
{
	bool bDrawExterCircle;	//需要绘制钢管外壁圆
	TAG_INFO_PART_DRAWCMD_TUBE_SECT(){
		m_iTagType=TAG_INFO_PART_DRAWCMD::PART_TUBE_SECT;
		bDrawExterCircle=true;
	}
};
struct TAG_INFO_PART_DRAWCMD_FL_FLAT : public TAG_INFO_PART_DRAWCMD
{
	bool bDrawInnerCircle;	//需要绘制法兰内圆弧
	TAG_INFO_PART_DRAWCMD_FL_FLAT(){
		m_iTagType=TAG_INFO_PART_DRAWCMD::PART_FL_FLAT;
		bDrawInnerCircle=true;
	}
};
class CStruDrawingCommand : public CXeroDrawingCommand
{
protected:
	void *pTagInfo; //临时使用不存储，可附加一些与绘图命令执行过程相关的临时参数
public:
	CLDSPart* m_pPart;
	CStruDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CStruDrawingCommand(){;}
	void SetTagInfo(void* pTag){pTagInfo=pTag;}
	TAG_INFO_PART_DRAWCMD* TagInfo(){return (TAG_INFO_PART_DRAWCMD*)pTagInfo;}
	//BOOL GetDCS(CDrawCoordTrans &dcs);
	virtual void Redraw();
public:
	static const int CLS_BASIC_CMD = 0;
	static const int CLS_PART_GENERAL_DRAW_CMD = 1;	//构件普通平铺结构视图绘制
	static const int CLS_PART_UPRIGHT_DRAW_CMD = 2;	//构件竖立截面结构视图绘制
	static const int CLS_JOINT_TUBE_DIM_CMD	   = 3;	//相贯视图尺寸与角度的标注
};
class CFLDrawing{
public:
	struct FL_DRAWING_STYLE{
		bool bDrawInnerWeldCircle;	//=true绘制与钢管焊接圆环边
		char ciDrawModeH0V1;
		short penStyle;	//对应原bCoplanar=TRUE时用虚线PS_DASH绘制
		FL_DRAWING_STYLE(){bDrawInnerWeldCircle=true;ciDrawModeH0V1=0;penStyle=PS_SOLID;}
	}style;

	void DrawFLStruDrawing(CLDSParamPlate* pFL,CDrawCoordTrans& dcs,IDbEntsContext* pContext);
};
//绘制构件的正常视图{钢板平铺视图、杆件平躺视图}的构件结构图
class CPartGeneralDrawingCommand : public CStruDrawingCommand
{
protected:
public:
	//CLDSPart* m_pPart;
	CPartGeneralDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CPartGeneralDrawingCommand(){;}
	virtual void Redraw();
};
//绘制构件的正常视图{钢板平铺视图、杆件平躺视图}的构件结构图
class CPartUprightDrawingCommand : public CStruDrawingCommand
{
protected:
public:
	//CLDSPart* m_pPart;
	CPartUprightDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CPartUprightDrawingCommand(){;}
	virtual void Redraw();
	void DrawTubeStruDrawing();
};
class CJointTubeDimDrawingCommand : public CStruDrawingCommand
{
protected:
public:
	CJointTubeDimDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CJointTubeDimDrawingCommand(){;}
	virtual void Redraw();
	void CreateJointTubeDims();
};
#endif
#endif
