// PlankDrawing.h: interface for the CPlankDrawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANKDRAWING_H__A1203127_5F46_4E86_95F5_4F3EEA09FFE5__INCLUDED_)
#define AFX_PLANKDRAWING_H__A1203127_5F46_4E86_95F5_4F3EEA09FFE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "XhCharString.h"
#include "LocalDrawing.h"
#include "PartGroupDrawing.h"

#ifndef __TSA_FILE_
#define PS_WELDLINE		7
#define PS_TUBEBREAK	8
#define PS_DAMAGELINE	9
#define PS_OTHERLINE	10	//TODO:必须注释,否则谁能知道这个类型是干嘛的? wjh-2017.1.11

typedef struct tagALIGNDIM_STRU
{
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double dist;
	int nLsSpaceNum;
	tagALIGNDIM_STRU(){dist=0;nLsSpaceNum=1;}
}ALIGNDIM_STRU;
struct ANGULARDIM_STRU
{
	f3dPoint vertex;
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double angle;
	double fRadius;
	CXhChar16 sDimText;
	ANGULARDIM_STRU(){angle=0;fRadius=0;sDimText.Copy("<>");}
};
typedef struct tagROTATEDIM_STRU
{
	static double PRECISION;		//生成文字距离标注时的精度,只取1.0,0.5,0.1三种值
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double angle;
	double dist;
	CXhChar16 sDimText;		//设置标注内容时dist属性无效
	BOOL bCanTextOutter;
	BOOL bDiameterStyle;
	BOOL bDisplayDimLine;	//是否显示尺寸线
	int nHits;				//连续的等距为S的标注，可以合并为一个S X nHits的标注
	CXhChar16 DimText();
	tagROTATEDIM_STRU(){memset(this,0,sizeof(tagROTATEDIM_STRU));}
}ROTATEDIM_STRU;
struct RAYROD_AXIS
{
	bool bDimAxisLine;			//需要标注丁字竖线(角钢心线)
	bool bDimAxisEndPerpLine;	//需要标注与心线垂直的丁字头顶线
	bool bDatumRod;
	UINT hRod;
	UINT nSubAxisCount;	//该杆件所统管的子心线数目(大于1时,需绘制丁字心线图及标注)
	CDesignLjPartPara* pLjPart;
	BYTE ciWorkWingX0Y1;
	int start_end,iFaceNo;
	GEPOINT org;
	GEPOINT axis_vec,wing_vec;
	double dfOrgTipExtend;	//射线杆件当前连接端从定位端（基）点向外延伸量 wjh-2019.5.19
	double ber_space,wing_space;	//基准心线到楞线、肢翼线的平行距离
	double ray_dist,ber_dist,wing_dist;		//丁字头顶线的楞线侧及肢翼侧的最大距离
	double fAngle,fRadius;
	void SetKey(DWORD key){hRod=key;}
	RAYROD_AXIS(){
		bDatumRod=false;
		iFaceNo=1;
		ciWorkWingX0Y1=0;
		start_end=0;
		pLjPart=NULL;
		nSubAxisCount=0;bDimAxisLine=bDimAxisEndPerpLine=true;
		dfOrgTipExtend=ber_space=wing_space=ber_dist=wing_dist=fAngle=fRadius=0;
		ray_dist=100000;//默认赋一大值
	}
};
typedef struct tagLSRAY_STRU
{
	int iRayNo,featrue;
	GEPOINT rayOrg;			//射线的起始点
	GEPOINT rayResideVec;	//该射线所驻存构件的射线方向(与螺栓数无关,一般为其寄生杆件的心线方向) wjh-2015.2.27
	fPtList ls_arr;
	//bIsDoubleLsRay目前仅在CDimPlankLs中有用,未来应统一到CPlankDrawing后去掉 wjh-2015.2.27
	BOOL bIsDoubleLsRay;//是否为双排螺栓射线 wht 11-06-30
	BOOL bDatumLsRay;	//是否为基准螺栓射线
	tagLSRAY_STRU(){featrue=0;bIsDoubleLsRay=FALSE;bDatumLsRay=FALSE;}
	void CloneMember(tagLSRAY_STRU* pSrcLsRay,BOOL bCopyLsArr=TRUE)
	{
		iRayNo=pSrcLsRay->iRayNo;
		rayOrg=pSrcLsRay->rayOrg;
		rayResideVec=pSrcLsRay->rayResideVec;
		bIsDoubleLsRay=pSrcLsRay->bIsDoubleLsRay;
		bDatumLsRay=pSrcLsRay->bDatumLsRay;
		if(bCopyLsArr)
		{
			for(int i=0;i<pSrcLsRay->ls_arr.GetNodeNum();i++)
				ls_arr.append(pSrcLsRay->ls_arr[i]);
		}
	}
}LSRAY_STRU;
struct TXTDIM_STRU
{
	f3dPoint dimPos;
	f3dPoint dimVec,dimPtS,dimPtE;	//不是必须用
	double txtHeight;
	AcDb::TextHorzMode horzAlignMode;	//水平对齐模式
	AcDb::TextVertMode vertAlignMode;	//竖直对章模式
	CXhChar100 dimText;	
	TXTDIM_STRU(){
		txtHeight=0;	//表示系统默认配置值
		horzAlignMode=AcDb::kTextLeft;
		vertAlignMode=AcDb::kTextBottom;
	}
};
struct PT_ITEM{
	f3dPoint origPt,extendPt;
	int nNum;
	PT_ITEM(){nNum=0;}
};
class CPlankDrawing;
class CWeldPlateDrawing : public CXeroDrawing
{
private:
	CPlankDrawing* m_pBelongDrawing;
	CLDSPart* m_pPlate;
	CDrawingEntsContext m_xEntContext;
public:
	TXTDIM_STRU m_xTxtDim;
	ROTATEDIM_STRU m_xRotateDim;
	double m_fWeldThick;
	double m_fWeldLen;
	double m_fTxtOff;
	BOOL m_bFrontWeld;
	f3dPoint topLeft,topRight,btmLeft,btmRight;
public:
	CWeldPlateDrawing();
	CWeldPlateDrawing(CPlankDrawing* pParentDrawing);
	~CWeldPlateDrawing();
	//
	void InitWeldDrawing(CPlankDrawing* pParentDrawing=NULL);
	void ClipLine(f3dLine &line);
	void PerfectWeldDrawing();
	BOOL CreatePlateSideEdge(CLDSGeneralPlate *pSubPlate,long hDatumPlate,UCS_STRU& draw_ucs);	
	//BOOL CreateParaPlateSideEdge(CLDSParamPlate* pSubParamPlate,long hDatumPlate,UCS_STRU& draw_ucs);
	BOOL IsOverlap(CWeldPlateDrawing& weld_drawing);
	void CopyMember(CWeldPlateDrawing& weld_drawing);
};
typedef struct tagDIAMETERDIM_STRU
{
	f3dPoint chordPos;
	f3dPoint farChordPos;	//标注半径时表示圆心
	double leaderLength;
	double dist;			//标注半径时表示半径
	BOOL bDimRadial;		//TRUE:标注的是半径;FALSE:标注的是直径
	tagDIAMETERDIM_STRU(){memset(this,0,sizeof(tagDIAMETERDIM_STRU));}
}DIAMETERDIM_STRU;
typedef struct tagPART_PLANK_CARD_INFO
{
	CXhChar16 sSeg;				//段号
	CXhChar16 sPartNo;			//件号
	CXhChar16 sLength;			//长度
	CXhChar16 sThick;			//厚度
	CXhChar16 sDesMaterial;		//设计材质
	CXhChar16 sReplMaterial;	//借用材质
	CXhChar16 sGuiGeIncMat;		//规格包括材质
	CXhChar16 sDesGuiGe;		//设计规格
	CXhChar16 sReplGuiGe;		//借用规格
	CXhChar16 sPartNum;			//单基数
	CXhChar16 sSumPartNum;		//总加工数=单基数*基数
	CXhChar16 sPartWeight;		//单重
	CXhChar16 sSumPartWeight;	//总重
	CXhChar16 sM12HoleNum;		//M12螺栓孔数
	CXhChar16 sM16HoleNum;		//M16螺栓孔数
	CXhChar16 sM18HoleNum;		//M18螺栓孔数
	CXhChar16 sM20HoleNum;		//M20螺栓孔数
	CXhChar16 sM22HoleNum;		//m22螺栓孔数
	CXhChar16 sM24HoleNum;		//M24螺栓孔数
	CXhChar16 sMXHoleNum;		//MX螺栓孔数
	CXhChar16 sSumHoleNum;		//总孔数
	CXhChar16 sChongKongNum;	//冲孔数
	CXhChar16 sZuanKongNum;		//钻孔数
	CXhChar16 sCutEdge;			//铲边
	CXhChar16 sWrapEdge;		//卷边
	CXhChar16 sHuoQuFst;		//首次火曲
	CXhChar16 sHuoQuSec;		//二次火曲
	CXhChar16 sMapScale;		//图面比例
	CXhChar16 sWeld;			//焊接
	CXhChar50 sNotes;			//备注
	CXhChar16 sWeldLength;		//焊缝长度
	CXhChar16 sEdgeNum;			//边数
	CXhChar16 sArea;			//面积
	CXhChar16 sFillet;			//坡口
	CXhChar16 sCutArc;			//切弧
	int m_nPageNum,m_nPageIndex;//共 页,第 页
	int m_nTaNum;				//基数	wht 09-12-31
	BOOL m_bPunchShear;			//冲剪
	BOOL m_bGasCutting;			//气割
	//
	tagPART_PLANK_CARD_INFO(){
		m_bPunchShear=FALSE;
		m_bGasCutting=FALSE;
	}
}PART_PLANK_CARD_INFO;
typedef struct{	int iRayNo,iPolarOrigin;}POLAR_MAP_STRU;
typedef struct{
	f3dLine line_arr[5];
	f3dPoint origin,offset_vec,prj_vec;
	double offset_dist;	//基点偏移方向
	double plane_angle;
}HUOQU_MARK;

f3dArcLine CreateIsometricEdgeLine(PROFILE_VERTEX* pPrev,PROFILE_VERTEX* pCurrent,CLDSPart* pPart,UCS_STRU *dcs=NULL,double inner_offset=0);
class CPlankDrawing  : public CXeroDrawing
{
	CDrawingEntsContext m_xEntContext;
	CLDSPart *m_pPlate;		
	BOOL m_bSpecifyUCS;		//是否为外部指定坐标系 wht 10-08-13
	f3dPoint geom_center;	//钢板几何中心(绝对坐标)
	UCS_STRU new_ucs;		//标注相对坐标时需要调整钢板坐标系，new_ucs用来记录调整后的坐标系
	f3dPoint polar_origin;
	POLYGON profileRegion;
	bool m_bPrevIsRollEdgeLine;
	GEPOINT m_xRollEdgeLineStart,m_xRollEdgeLineEnd;
	struct LSRAYLINE_PARA{
		bool reverseLenvec;
		long hRod;
		double coord_x,coord_y;
		LSRAYLINE_PARA(long rod=0,double coordx=0,double coordy=0)
		{
			reverseLenvec=false;
			hRod=rod;
			coord_x=coordx;
			coord_y=coordy;
		}
	};
	CArray<TXTDIM_STRU,TXTDIM_STRU&> text_dim_arr;
	CArray<ROTATEDIM_STRU,ROTATEDIM_STRU&>rotate_dim_arr;
	CArray<ANGULARDIM_STRU,ANGULARDIM_STRU&>angular_dim_arr;
	CArray<DIAMETERDIM_STRU,DIAMETERDIM_STRU&>diameter_dim_arr;
	CArray<f3dCircle,f3dCircle&>cir_arr;
	CArray<f3dArcLine,f3dArcLine&>edge_arr;
	CArray<LSRAY_STRU,LSRAY_STRU>ls_ray_arr;
	CArray<f3dLine,f3dLine>assist_ls_ray_arr;
	BOOL CreateParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateUEndOrRollEndParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateFLParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateStdFLParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);	//生成高颈法兰
	BOOL CreateElbowParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateFootParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreatePedalParamPlate(CLDSParamPlate *pParamPlate, BOOL bPatternDrawing = TRUE);
	BOOL CreateCircularParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateCircularParamPlate2(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL CreateRelativePlateInCirParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing,f3dPoint origin);
	BOOL CreateWaterShieldedPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing=TRUE);
	BOOL DrawPlateLocateFlag(CLDSPlate *pPlank,BOOL bAdjustPlateUcs);
	BOOL Create1FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL Create2FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL Create3FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL CreateFoldPlank(CLDSPlate *pPlank,BOOL bPatternDrawing=TRUE);
	BOOL CreatePlateInnerCir(CLDSPlate *pPlate,BOOL bPatternDrawing,BOOL bAdjustPlateUcs);	//生成钢板内圆环
	void DimCoordValue(CLDSPart *pPart,f3dPoint pos);	//标注相对坐标
	void DimPlateEdgeLen(f3dLine line);	//标注板边长
	void MergeParallelEdgeDim(int iStart);	//合并共线的边长标注项目
	void DimArcLineCoordValue(CLDSPart *pPart, f3dArcLine arcLine); //在圆弧上标注nCount个点的坐标
	void ProcessPlateWeldEdge(PROFILE_VERTEX *pFeatureVertex,f3dArcLine &arcline);
	void DrawSlopeCutLine(f3dArcLine arcLine);		//绘制坡口线(焊接边平行线)
	void DimSlopeCutLineText(f3dArcLine arcLine);	//标注焊缝文本
	void CreateLineOnWeldCorner(CLDSGeneralPlate* pPart,ATOM_LIST<PROFILE_VERTEX> *pVertexList);//处理焊烧死角（切角或切弧）
	void DrawInnerEdge(CLDSGeneralPlate* pPart,ATOM_LIST<PROFILE_VERTEX> *pVertexList);	//绘制内侧平行轮廓边
	void DrawWeldSubPlate(CLDSGeneralPlate* pDatumPlate,BOOL bAdjustPlateUcs);	//绘制焊接子构件轮廓边
	void MarkHuoquLine(f3dLine line);
	//根据卷边高度以及卷边外移量绘制卷边相关线段 wht 11-04-09
	void DrawRollEdgeLine(PROFILE_VERTEX *pStartVertex,PROFILE_VERTEX *pEndVertex,
						  PROFILE_VERTEX *pPreVertex,PROFILE_VERTEX *pNextVertex,
						  CLDSPlate *pPlate,f3dLine &roll_edge_line,f3dLine &huoqu_line,BOOL bPatternDrawing);
	//钢板上的椭圆弧需根据底部及底部情况进行分段绘制
	void DrawPlateEllipseLine(CLDSGeneralPlate* pPlate,f3dArcLine arcline,BOOL bOriInBtmPlane=TRUE);
	//标注风格设定
public:
	static MASKFLAG PREFER_STYLEFLAG;
	MASKFLAG dimStyleFlag;
	static DWORD DimBoltSpaceToWeldEdge;	//0x00 0001 00:00000000 0000000000000001 00000000
	static DWORD DimHoleDiameterAlways;		//0x00 0002 00:00000000 0000000000000010 00000000
	static DWORD DimBoltSpaceToHuoQuLine;	//0x00 0004 00:00000000 0000000000000100 00000000
	static DWORD DimBoltSpaceToEdgeLine;	//0x00 0008 00:00000000 0000000000001000 00000000
	static DWORD DimBoltSpaceToNextLsRay;	//0x00 0010 00:00000000 0000000000010000 00000000
	static DWORD DimBoltAngle;				//0x00 0020 00:00000000 0000000000100000 00000000
public:
	BYTE m_ciHuoQuType;	//0.无火曲|1.内曲(反曲)|2.外曲(正曲)
	HUOQU_MARK huoqu_mark[2];
	f3dPoint long_edge_mid,huoqu_edge_mid[2];	//钢板最长边的中心及火曲线的中点
	double long_edge_angle;	//钢板最长边的角度，构件明细一般沿钢板最长边方向标注
	f3dLine datumLine;		//钢板的基准线，一般螺栓射线应该延伸至此基准线（以板相对坐标存储）
public:
	f2dRect GetDrawRegion(double PART_MAP_SCALE);
	//标注螺栓射线及其上的螺栓间距
	void DimLsRay(CLDSPart *pPart=NULL,BOOL bAdjustPlateUcs=FALSE);
	//标注外形特征螺栓间距
	void DimWrapLsSpace(CLDSGeneralPlate* pGeneralPlate);
	//norm为连接板相对坐标系下的矢量
	void AddHuoQuPrjMap(f3dLine huoqu_line,HUOQU_MARK mark,char cMark='A');
	void AnalizeLsRay(f3dPoint ls_centre,int iRayNo,const double* resideRayVec=NULL);
	BOOL FindLsRayOrgAndDatumVec(f3dPoint &origin,f3dPoint &datum_vec);
	void CorrectLsRays();
	void CompleteLsRayInfo(CLDSGeneralPlate* pGeneralPlate,BOOL bAdjustPlateUcs=FALSE);
	LSRAY_STRU* FindLsRay(f3dPoint* lsPos1,f3dPoint* lsPos2);
	//标注构件坐标时调整坐标系后需要分析杆件射线
	void AnalizePoleRay(CLDSPlate *pPlate);
	CPlankDrawing();
	virtual ~CPlankDrawing();
	BOOL Create(CLDSPart *pPlankTypePart,BOOL bPatternDrawing=TRUE);
	int GetLsCirCount();
	int GetLsRayCount();
	int GetAssistLsRayCount();
	int GetEdgeCount();
	int GetAlignDimCount();
	int GetAngularDimCount();
	int GetRotateDimCount();
	int GetDiameterDimCount();
	int GetTextDimCount();
	BOOL GetAngularDimAt(int index, ANGULARDIM_STRU &dim);
	BOOL GetRotateDimAt(int index, ROTATEDIM_STRU &dim);
	BOOL GetDiameterDimAt(int index, DIAMETERDIM_STRU&dim);
	BOOL GetTextDimAt(int index, TXTDIM_STRU&dim);
	BOOL GetEdgeAt(int index, f3dArcLine &line);
	BOOL GetLsCirAt(int index, f3dCircle &Cir);
	BOOL GetLsRayLineAt(int index, f3dLine &line,DWORD* pwDrawFlag=NULL);
	BOOL GetAssLsRayLineAt(int index, f3dLine &line);
	f3dPoint GetGeomCenter() {return geom_center;}
	f3dPoint Get2dGeomCenter();
	bool PtInPlateRgn(const double* poscoord);
	GEPOINT GetPlateRgnPt(int index){return profileRegion.GetVertexAt(index);}
	int GetPlateRgnPtNum(){return profileRegion.GetVertexCount();}
	UCS_STRU GetNewUcs() {return new_ucs;}
	//设置绘图坐标系 wht 10-08-13
	void SetNewUcs(CLDSPart *pPart,UCS_STRU plate_ucs);	
	//查找钢板文字说明标注位置
	BOOL FindRectValidPos(f3dPoint &dimPos,CLDSPart *pPlate,double fWidth,double fHeight,double zoomCoef=1.0);
	CLDSPart *GetPlate(){return m_pPlate;}
};
#endif
#endif // !defined(AFX_PLANKDRAWING_H__A1203127_5F46_4E86_95F5_4F3EEA09FFE5__INCLUDED_)
