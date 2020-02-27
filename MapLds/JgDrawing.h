// JgDrawing.h: interface for the CJgDrawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JGDRAWING_H__7471C63E_4F7B_494A_B082_15571080C7D3__INCLUDED_)
#define AFX_JGDRAWING_H__7471C63E_4F7B_494A_B082_15571080C7D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef __DRAWING_CONTEXT_
#include "Tower.h"
#include "TowerType.h"
#else 
#include "ArrayList.h"
#endif
#include "XhCharString.h"
#include "DrawDim.h"
#include "ArrayPtrList.h"
#include "ProcessPart.h"

class CJgDrawingPara
{
public:
	double	fDimTextSize;		//长度尺寸标注文本高
	int iDimPrecision;			//尺寸精确度
	double fRealToDraw;			//基础绘图比例尺＝实际尺寸/绘图尺寸，如1:20时，fRealToDraw=20
	double	fDimArrowSize;		//尺寸标注箭头长
	double fTextXFactor;
	double	fPartNoTextSize;	//构件编号文字高
	int		iPartNoFrameStyle;	//编号框类型 //0.圆圈 1.带圆弧的矩形框 2.矩形框	3.自动判断
	double	fPartNoMargin;		//构件编号与编号框之间的间隙值 
	double	fPartNoCirD;		//构件编号圈直径
	double fPartGuigeTextSize;	//构件规格文字高
	int iMatCharPosType;
	char cMatCharSeparator;

	BOOL bModulateLongJg;		//调整角钢长度 暂未使用，由iJgZoomSchema代替该变量 wht 11-05-07
	int iJgZoomSchema;			//角钢绘制方案，0.1:1绘制 1.使用构件图比例 2.长宽同比缩放 3.长宽分别缩放
	BYTE ciJgLenDrawMode;		//角钢长度超出工艺卡处理模式：0.向右延伸 1.中间挤压
	BOOL bMaxExtendAngleLength;	//最大限度延伸角钢绘制长度
	BOOL bOneCardMultiPart;		//角钢允许一卡多件情况
	int  iJgGDimStyle;			//0.始端标注  1.中间标注 2.自动判断
	int  nMaxBoltNumStartDimG;	//集中在始端标注准距支持的最大螺栓数
	int  iLsSpaceDimStyle;		//0.X轴方向	  1.Y轴方向  2.自动判断 3.不标注  4.无标注内容(X轴方向)  4.仅标尺寸线，无标注内容(X轴方向)主要用于江津(北)塔厂质检是盲检填数
	int  nMaxBoltNumAlongX;		//沿X轴方向标注支持的最大螺栓个数
	BOOL bDimCutAngle;			//标注角钢切角
	BOOL bDimCutAngleMap;		//标注角钢切角示意图
	BOOL bDimPushFlatMap;		//标注压扁示意图
	BOOL bJgUseSimpleLsMap;		//角钢使用简化螺栓图符
	BOOL bDimLsAbsoluteDist;	//标注螺栓绝对尺寸
	BOOL bMergeLsAbsoluteDist;	//合并相邻等距螺栓绝对尺寸 江津及增立提出:有时也需要标 wjh-2014.6.9
	BOOL bDimRibPlatePartNo;	//标注角钢肋板编号
	BOOL bDimRibPlateMaterial;	//标注角钢肋板材质
	BOOL bDimRibPlateSetUpPos;	//标注角钢肋板安装位置
	//切角标注样式一
	//切角标注样式二 B:端头尺寸 L:肢边尺寸 C:筋边尺寸 
	//BXL 切角  CXL 切肢 BXC 切筋  切大角=切筋+切肢
	int	 iCutAngleDimType;		//切角标注样式 0.样式一  1.样式二 wht 10-11-01
	int  iCutWingDimType;		//切肢参数标注类型
	//
	BOOL bDimKaiHe;				//标注角钢开合角
	BOOL bDimKaiheAngleMap;		//标注角钢开合角示意图
	double fKaiHeJiaoThreshold; //开合角标注阈值(°) wht 11-05-06
	double fKaiHeJiaoMinDeita;	//开合角外侧最小间隙 wjh 2015.2.6
	//新增开合角标注开关 wht 12-03-13
	BOOL bDimKaiheSumLen;		//标注开合区域总长
	BOOL bDimKaiheAngle;		//标注开合度数	
	BOOL bDimKaiheSegLen;		//标注开合区域分段长
	BOOL bDimKaiheScopeMap;		//标注开合区域标识符
	//
	BOOL bSplitAngleSketchDrawing;
	CJgDrawingPara();
};

#ifndef __TSA_FILE_
typedef struct tagPART_JG_CARD_INFO
{
	CXhChar16 sSeg;				//段号
	CXhChar16 sPartNo;			//件号
	CXhChar16 sLength;			//长度
	CXhChar16 sDesMaterial;		//设计材质
	CXhChar16 sReplMaterial;	//借用材质
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
	CXhChar16 sM22HoleNum;		//M22螺栓孔数
	CXhChar16 sM24HoleNum;		//M24螺栓孔数
	CXhChar16 sMXHoleNum;		//其它螺栓孔数
	CXhChar16 sSumHoleNum;		//总孔数
	CXhChar16 sChongKongNum;	//冲孔数
	CXhChar16 sZuanKongNum;		//钻孔数
	CXhChar16 sCutRoot;			//刨根
	CXhChar16 sCutBer;			//铲背
	CXhChar16 sCutRootLen;		//刨根长度	wht 10-11-17
	CXhChar16 sCutBerLen;		//铲背长度
	CXhChar16 sHuoQuFst;		//首次火曲
	CXhChar16 sHuoQuSec;		//二次火曲
	CXhChar16 sCutAngSX;		//始端X肢切角	
	CXhChar16 sCutAngSY;		//始端Y肢切角
	CXhChar16 sCutAngEX;		//终端X肢切角
	CXhChar16 sCutAngEY;		//终端Y肢切角
	CXhChar50 sCutAngParaSX;	//始端X肢切角参数显示
	CXhChar50 sCutAngParaSY;	//始端Y肢切角参数显示
	CXhChar50 sCutAngParaEX;	//终端X肢切角参数显示
	CXhChar50 sCutAngParaEY;	//终端Y肢切角参数显示
	CXhChar50 sPushFlat;		//压扁 实时数据标注格式为:始-Y/220终-Y/230 16个字符不够用故修改为CXhChar50 wht10-11-03
	CXhChar16 sPushFlatParaSX;	//始端X肢压扁参数显示 wht 10-11-03
	CXhChar16 sPushFlatParaSY;	//始端Y肢压扁参数显示
	CXhChar16 sPushFlatParaEX;	//终端X肢压扁参数显示
	CXhChar16 sPushFlatParaEY;	//终端Y肢压扁参数显示
	CXhChar16 sPushFlatParaMX;	//中间X肢压扁参数显示
	CXhChar16 sPushFlatParaMY;	//中间Y肢压扁参数显示
	CXhChar16 sWeld;			//焊接
	CXhChar16 sKaiJiao;			//开角
	CXhChar16 sHeJiao;			//合角
	CXhChar16 sWingAngle;		//开合角具体度数
	CXhChar50 sNotes;			//备注
	int  m_nCutBerR;			//铲背圆弧半径mm,一般相联主材的角钢内弧值r+2
	int m_nPageNum,m_nPageIndex;//共 页,第 页
	int m_nTaNum;				//基数  wht 09-12-31
	BOOL m_bPunchShear;			//冲剪
	BOOL m_bGasCutting;			//气割
	BOOL m_bOneShot;			//一次完成
	//
	tagPART_JG_CARD_INFO(){
		m_bPunchShear=FALSE;
		m_bGasCutting=FALSE;
		m_bOneShot=FALSE;
	}
}PART_JG_CARD_INFO;
typedef struct tagSPECIAL_PROCESS_STAT_CARD_INFO
{
	CXhChar200 sProcessName;
	CXhChar200 sSketchMap;
	int  m_nPageNum,m_nPageIndex; 
}SPECIAL_PROCESS_STAT_CARD_INFO;
//
struct FILLET_ARC
{
	BYTE m_ciType;	//0.SX|1.SY|2.EX|3.EY
	double m_fFilletR;
	f3dPoint m_ptS, m_ptE;
public:
	FILLET_ARC() {
		m_ciType = 0; m_fFilletR = 0;
	}
	FILLET_ARC(GEPOINT ptS, GEPOINT ptE, BYTE iType, double fFilletR) {
		m_ptS = ptS;
		m_ptE = ptE;
		m_ciType = iType;
		m_fFilletR = fFilletR;
	}
	BOOL ExtractArc(f3dArcLine& arcLine, f3dLine& line);
};
//返回值为0表示位于X肢，1表示位于Y肢
//-1表示螺栓方向与角钢肢法线不匹配
#ifdef __DRAWING_CONTEXT_
int ProjectJgLsToMap(UCS_STRU &ucs,CLDSBolt *pLs,f3dPoint norm_X,f3dPoint norm_Y,f3dPoint &prj_pos);
//从PaperPatternDrawing.cpp移动到此处 wht 15-03-18
BOOL CalLsPosInArcPart(CLDSArcPart* pArcPart,CLsRef *pLsRef,f3dPoint& ls_pos);
#endif
int compareBoltPos(const f3dPoint &centre1,const f3dPoint &centre2);
class CRodDrawing  
{
	//把统计在一起的螺栓(存在boltCenterArr中)进行分析以确定绘图时的缩放方案
	//此函数必须在确定好左边距之后调用
	void MakeZoomPlanByBoltsInfo();
	bool IsHasIntersObj(CZhunXianTextDim &zhunDim);
protected:
	bool m_blScatterBoltLocaTolerance;
public:
	static const int MAX_G_COUNT = 15;
	struct LsPosMark
	{
		int wingX0Y1Both2;		//螺栓孔位所对应的肢
		double realCoordX;		//实际X坐标分量
		double drawCoordX;		//绘图时的绘制位置X坐标分量

		double minPrevDimSpace;	//距离同肢前一标注最小绘制距离
		double needExtendDist;	//需要扩充的间距值
		double forewardMoveDist;//只用于记录最后一个孔距末端标注距离不够时需要前移的绘制距离
		CSizeTextDim *pDimSpaceSizeX, *pDimSpaceSizeY;
		CTextOnlyDrawing *pDimSpaceTextX, *pDimSpaceTextY;
		LsPosMark(){
			pDimSpaceSizeX=pDimSpaceSizeY=NULL;
			pDimSpaceTextX=pDimSpaceTextY=NULL;
			wingX0Y1Both2=0;
			realCoordX=drawCoordX=0;
			minPrevDimSpace=needExtendDist=forewardMoveDist=0;
		}
	};
	typedef struct LsMark
	{
		int wingX0Y1;			//螺栓孔位所对应的肢
		int iRow;				//螺栓所在行索引号，用于分行标注螺栓 wht 05-07
		BYTE cFuncType;			//螺栓孔功用类型 wxc 15-2-13
		BYTE cFlag;				//螺栓孔加工工艺
		f3dPoint realPos;
		f3dCircle dimCircle;
		CSizeTextDim* pDimSpaceSize;
		CTextOnlyDrawing* pDimSpaceText;
		LsMark(){cFlag=0;cFuncType=0;iRow=1;pDimSpaceSize=NULL;pDimSpaceText=NULL;}
	}*LsMarkPtr;
	struct LsGroup
	{
		int m_nLsNum;
		LsPosMark* lsMarkArr[500];
		int groupTypeStart0End1Mid2;
	};
	static const BYTE CONTEXT_PARTDRAW = 0;	//0.构件图环境
	static const BYTE CONTEXT_WELDDRAW = 1;	//1.组焊图环境
	static const BYTE CONTEXT_STRUDRAW = 2;	//2.结构图环境
	BYTE m_ciResideContext;	//0.构件图环境;1.组焊图环境;2.结构图环境
protected:
	CXhChar50 sPartNo;		//CRodDrawing::CreateLsSpaceDimList()出错报警用　wjh-2015.11.30
	double PRECISION;		//只能取1,0.5,0.1三个值
	double leftGDimMargin;	//
	double rightMargin;
	double minDrawingSpace;	//螺栓图符间的最小间隔，一般由螺栓图符的显示直径决定
	double WtoH;			//字符高宽比
	double maxDrawingLength;
	double maxDrawingHeight;
	double length;			//角钢实际长度
	double wingWide;		//角钢肢宽
	double wingThick;		//角钢肢厚
	int m_nWingAxisGX;		//X肢心线数量
	int m_nWingAxisGY;		//Y肢心线数量
	int wingAxisGArrX[MAX_G_COUNT];	//X肢心线的心距数组
	int wingAxisGArrY[MAX_G_COUNT];	//Y肢心线的心距数组
	double wingAxisCoordArrX[MAX_G_COUNT];	//X肢心线的绘制位置Y坐标
	double wingAxisCoordArrY[MAX_G_COUNT];	//Y肢心线的绘制位置Y坐标
	double m_fLengthScaleCoef;	//考虑孔距标注的缩放比例
	double m_fVertScaleCoef;	//肢宽方向(Y轴)的缩放比例
	BYTE m_ciBoltSpaceDimExtraStyle;//0.继承原模式;1.不标注螺栓间距;2.仅标尺寸线 wjh-2017.11.28
	static CJgDrawingPara drawPara;//替换原先CDrawSysPara信息
	static void InitJgDrawingPara();//初始化CJgDrawingPara
	//UCS_STRU angleDrawUcs;
	//角钢轮廓线
	CArray <f3dLine,f3dLine&>edgeLineArr;	//长度方向边线
	CArray<f3dLine,f3dLine&>lineArr;//角钢示意草图中除以上五条长度方向边线以外的线图元
	//角钢上的螺栓孔
	ARRAY_LIST<LsMarkPtr>LsCenterArr;	//角钢上所有螺栓孔
	ARRAY_LIST<LsPosMark>LsPosArr;	//螺栓孔位(长度方向)数(双肢累计)
	ARRAYPTR_LIST<LsMark>LsCirX;				//对应真实的螺栓,用于绘图时标注螺栓图符
	ARRAYPTR_LIST<LsMark>LsCirY;				//对应真实的螺栓,用于绘图时标注螺栓图符
	//CArray<f3dPoint,f3dPoint&>LsCenterArr;	//角钢上所有螺栓孔
	//CArray<LsPosMark,LsPosMark&>LsPosArr;	//螺栓孔位(长度方向)数(双肢累计)
	//CArray<LsMark,LsMark&>LsCirX;			//对应真实的螺栓,用于绘图时标注螺栓图符
	//CArray<LsMark,LsMark&>LsCirY;			//对应真实的螺栓,用于绘图时标注螺栓图符
	//草图中的各项标注
	//CArray<CSizeTextDim, CSizeTextDim&>dim_arr;
	//CArray<CTextOnlyDrawing,CTextOnlyDrawing&>pure_txt_arr;
	ARRAYPTR_LIST<CSizeTextDim>dim_arr;
	ARRAYPTR_LIST<CTextOnlyDrawing>pure_txt_arr;
	CArray<CZhunXianTextDim,CZhunXianTextDim&>zhun_dim_arr;
protected:
	//把所有段角钢的螺栓统计在一起,
	void StatBoltsInfo(ATOM_LIST<BOLT_INFO> &bolt_list,long mid_flat_pos_z=0);
	void CreateLsSpaceDimList();
	void InitWingAxisCoordY();
	void CalDrawingScaleCoef(bool bEndCutAngleOrWing,bool bStartCutAngleOrWing);
	BOOL DimLsZhun(f3dPoint ls_prj_pos);
	double TransCutWingOrAngleDrawDist(double dist);
	void CreateJgLsCircleList();
	virtual SCOPE_STRU RelayoutDrawing();
	void InitDimStyleBySysSetting(int nBoltNum);
	void CreateLsFuncTextDim();
	void AdjustLsPosX(ARRAY_LIST<double>& posXInWx,ARRAY_LIST<double>& posXInWy);
	void AppendLine(f3dPoint ptS,f3dPoint ptE,short style=PS_SOLID);
public:
	double TransCoordX(double realPosX);
	double TransCoordY(double realPosY);
	//标注风格设定
public:
	MASKFLAG dimStyleFlag;
	static DWORD DimSpaceTextAlongX;	//0x00 0000 01:00000000 0000000000000000 00000001
	static DWORD DimSpaceTextAlongY;	//0x80 0000 01:10000000 0000000000000000 00000001
	static DWORD DimPosTextByOneLayer;	//0x00 0000 02:00000000 0000000000000000 00000010
	static DWORD DimPosTextByMultiLayer;//0x40 0000 02:01000000 0000000000000000 00000010
	static DWORD DimAxisGTextAtStart;	//0x00 0000 04:00000000 0000000000000000 00000100
	static DWORD DimAxisGTextAtMiddle;	//0x20 0000 04:00100000 0000000000000000 00000100
	static DWORD CutAngleDimByImage;	//0x00 0000 08:00000000 0000000000000000 00001000
	static DWORD CutAngleDimByText;		//0x10 0000 08:00010000 0000000000000000 00001000
	static DWORD BriefDrawing;			//0x00 0001 00:00000000 0000000000000001 00000000 绘制角钢简图
	static DWORD OneCardMultiPart;		//0x00 0002 00:00000000 0000000000000010 00000000 一卡多件
	static DWORD ModulateLongJg;		//0x00 0004 00:00000000 0000000000000100 00000000 调整角钢长度
	static DWORD SpecifyDrawscale;		//0x00 0008 00:00000000 0000000000001000 00000000 指定绘图比例
	//长宽方向比例均使用两者中较小的比例(即不用保证角钢填充满绘图区域) wht 11-05-07
	static DWORD UseMinDrawscale;		//0x00 0010 00:00000000 0000000000010000 00000000 
	static DWORD WrapAngleDrawing;		//0x00 0020 00:00000000 0000000000100000 00000000 绘制包铁简图
	static DWORD OnlyDrawEndBolt;		//0x00 0040 00:00000000 0000000001000000 00000000 仅绘制端头螺栓,绘制角钢切角简图时使用 wht 18-09-21
public:
	f2dRect GetDrawRegion(double PART_MAP_SCALE);
	CRodDrawing();
	virtual ~CRodDrawing();
	int GetZEdgeCount();
	int  GetLineCount();
	int GetDimCount();
	int GetZhunDimCount();
	int GetXLsCount();
	int GetYLsCount();
	int GetPureTxtCount();
	double GetWingWide(){return wingWide;}
	double GetWingThick(){return wingThick;}
	BOOL GetZEdge(f3dLine &line,int index);
	BOOL GetLineAt(f3dLine &line,int index);
	BOOL GetXLsCircle(f3dCircle &Cir,int index);
	BOOL GetYLsCircle(f3dCircle &Cir,int index);
	BOOL GetDimAt(int index,CSizeTextDim *pDim);
	BOOL GetZhunDimAt(int index,CZhunXianTextDim *pDim);
	BOOL GetPureTxtDimAt(int index,CTextOnlyDrawing *pDim);
	void SetLengthScaleCoef(double scale){ m_fLengthScaleCoef=scale; }	//考虑孔距标注的缩放比例
	void SetVertScaleCoef(double scale){ m_fVertScaleCoef=scale;  }		//肢宽方向(Y轴)的缩放比例

	virtual int GetRibPlateDimCount(){return 0;} 
	virtual int GetKaiHeDimCount(){return 0;}
	virtual int GetPartNoCount(){return 0;}
	virtual int GetFilletArcCount() { return 0; }
	virtual BOOL GetFilletArcAt(int index, FILLET_ARC &line) { return  FALSE; }
	virtual BOOL GetRibPlateDimAt(int index,CSizeTextDim* pDim){return FALSE;}
	virtual BOOL GetKaiHeDimAt(int index,CKaiHeAngleDim *pDim){return FALSE;}
	virtual BOOL GetPartNoDimAt(int index,CTextFrame *pDim){return FALSE;}
	virtual BOOL IsPolyAngleDrawing(){return FALSE;}
};

class CAngleDrawing : public CRodDrawing
{
protected:
	double wingWideY;		//角钢Y肢宽
	double dfFilletROfSX, dfFilletROfSY, dfFilletROfEX, dfFilletROfEY;
	ARRAY_LIST<FILLET_ARC> filletArcArr;	//肢边倒R圆弧
	ARRAY_LIST<int> rib_thick_arr;		//肋板厚度
	ARRAYPTR_LIST<CSizeTextDim> rib_dim_arr;
	ARRAYPTR_LIST<LsMark> rib_mark_arr;		//肋板位置标记
	CArray<CTextFrame, CTextFrame&>part_no_arr;	//肋板编号
	CArray<CKaiHeAngleDim,CKaiHeAngleDim&>kaihe_angle_dim_arr;
protected:
	double GetBoltMaxY(int wingX0Y1);
	BOOL FindNearLsMark(int *piWingX0Y1,double realCoordX,BOOL bLeft,LsMark* pOutBoltMark);
	BOOL IsLsMarkPosOverlap(f3dPoint ls_pos);
	void CreateJgRibPlateDimList(ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list);
	void CreateJgRibPlateEdge(ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list);
	void CreateJgProfile(double cut_wing_para[2][3],long cut_wing[2],double cut_angle[4][2],double xWingWide,double yWingWide,
		double wingThick,BOOL bIncStartDim/*=TRUE*/,BOOL bIncEndDim/*=TRUE*/);
#ifdef  __DRAWING_CONTEXT_
	BOOL ProcessKaiHeAngle(CLDSLineAngle *pJg,UCS_STRU &angleUcs,BOOL bUcsIncStartOdd);
#endif
	BOOL ProcessKaiHeAngle(CProcessAngle *pJg);
	virtual SCOPE_STRU RelayoutDrawing();
public:
	CAngleDrawing();
	virtual ~CAngleDrawing();
	int GetRibPlateDimCount();
	int GetKaiHeDimCount();
	int GetPartNoCount();
	int GetFilletArcCount();
	BOOL GetFilletArcAt(int index, FILLET_ARC &line);
	BOOL GetRibPlateDimAt(int index,CSizeTextDim* pDim);
	BOOL GetKaiHeDimAt(int index,CKaiHeAngleDim *pDim);
	BOOL GetPartNoDimAt(int index,CTextFrame *pDim);
	BOOL GetMidFlatPos(f3dPoint &startPos,f3dPoint &endPos,f3dPoint &midPos,int wing_x0_y1);
public:	//标注样式中字高设定
	DECLARE_READONLY_PROPERTY(double,fDimTextSize);			//长度尺寸标注文本高
	DECLARE_READONLY_PROPERTY(double,fDimAngleTextSize);	//角度尺寸标注文本高
	DECLARE_READONLY_PROPERTY(double,fDimLabelTextSize);	//构件编号文字高
	DECLARE_READONLY_PROPERTY(double,fDimSizeSpecTextSize);	//构件规格文字高
};

class CLineAngleDrawing : public CAngleDrawing
{
public:
	void EmptyEnts();
#ifdef __DRAWING_CONTEXT_
	BOOL CreateLineAngleDrawing(CLDSLineAngle *pJg,double maxDrawingLength,double maxDrawingHeight,
		double textHeight=2.5,double WtoH=0.7,double minSpace=3.5);
#endif
	BOOL CreateLineAngleDrawing(CProcessAngle *pAngle,double max_drawing_length,double max_drawing_height,
		double text_height=2.5,double w_to_h=0.7,double min_space=3.5);
};

struct HUOQU_LINE
{
	f3dPoint startPt,endPt;
	int huoqu_type;		//0.X肢内曲  1.X肢外曲  2.Y肢内曲  3.Y肢外曲
	double huoqu_angle;
};
struct ANGLE_DIM_STRU
{
	f3dPoint vertex;
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double angle;
	double fRadius;
	CXhChar16 sDimText;
	ANGLE_DIM_STRU(){angle=0;fRadius=0;sDimText.Copy("<>");}
};
class CPolyAngleDrawing : public CAngleDrawing
{
	CArray<HUOQU_LINE,HUOQU_LINE&>huoqu_line;//火曲线
	CArray<CLDSLineAngle,CLDSLineAngle&>segArr; //分段角钢
	ARRAYPTR_LIST<CTextOnlyDrawing> huoqu_txt_arr;	//火曲信息
	CArray<ANGLE_DIM_STRU,ANGLE_DIM_STRU&>angle_dim_arr;
protected:
	virtual SCOPE_STRU RelayoutDrawing();
public:
	CPolyAngleDrawing(){;}
	virtual ~CPolyAngleDrawing(){;}
	virtual BOOL IsPolyAngleDrawing(){return TRUE;}
#ifdef  __DRAWING_CONTEXT_
	BOOL CreatePolyAngleDrawing(CLDSPolyPart *pHuoQuJg,double maxDrawingLength,double maxDrawingHeight,
		double textHeight=2.5,double WtoH=0.7,double minSpace=3.5);
	BOOL CreatePolyAngleBendDrawing(CLDSPolyPart *pHuoQuJg);
#endif
	int GetHuoQuLineCount(){return (int)huoqu_line.GetSize();}
	BOOL GetHuoQuLine(f3dLine &line,int index);
	int GetSegAngleCount(){return (int)segArr.GetSize();}
	CLDSLineAngle* GetSegAngle(int index){return &segArr[index];}
	HUOQU_LINE GetHuoQuLine(int index){return huoqu_line[index];}
	int GetHuoQuTextCount(){return huoqu_txt_arr.GetNodeNum();}
	CTextOnlyDrawing* GetHuoQuTxtDimAt(int index){return &huoqu_txt_arr[index];}
	int GetAngleDimCount(){return (int)angle_dim_arr.GetSize();}
	void GetAngleDimAt(int index,ANGLE_DIM_STRU* pAngleDim){memcpy(pAngleDim,&angle_dim_arr[index],sizeof(ANGLE_DIM_STRU));}
};

class CSlotDrawing : public CRodDrawing
{
	double wingWide2;		//肢宽2
	void CreateProfile();
public:
	CSlotDrawing();
	~CSlotDrawing();
#ifdef __DRAWING_CONTEXT_
	BOOL CreateSlotDrawing(CLDSPart *pSlot,double maxDrawingLength,double maxDrawingHeight,
		double textHeight=2.5,double WtoH=0.7,double minSpace=3.5);
#endif
};

class CFlatDrawing : public CRodDrawing
{
	void CreateProfile();
public:
	CFlatDrawing();
	~CFlatDrawing();
#ifdef __DRAWING_CONTEXT_
	BOOL CreateFlatDrawing(CLDSPart *pFlat,double maxDrawingLength,double maxDrawingHeight,
		double textHeight=2.5,double WtoH=0.7,double minSpace=3.5);
#endif
};
#endif
#endif // !defined(AFX_JGDRAWING_H__7471C63E_4F7B_494A_B082_15571080C7D3__INCLUDED_)
