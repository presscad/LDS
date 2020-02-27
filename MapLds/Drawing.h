// Drawing.h: interface for the CDrawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWING_H__0EBE37F4_B612_4A73_9CB4_D45B171C1F29__INCLUDED_)
#define AFX_DRAWING_H__0EBE37F4_B612_4A73_9CB4_D45B171C1F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "StdArx.h"
#include "Tower.h"
#include "HashTable.h"
#include "XhCharString.h"
#include "DrawUnit.h"
#include "FormatTable.h"
#include "DragEntSet.h"
#include "LocalDrawing.h"
#include "UnifyModelLayer.h"
#include "../IntelliAgent/IntelliCore/IPreview.h"

#ifdef __SMART_DRAW_INC_
//清单类型
enum DRAW_BOM_TYPE{
	BOM_SUMPART		= 1,	//材料汇总表
	BOM_SEGPART		= 2,	//分段构件明细表
	BOM_SEGBOLT		= 3,	//分段螺栓明细表
	BOM_PARTGROUP	= 4,	//组焊件明细表
	BOM_FL			= 5,	//法兰明细表
	BOM_GUIGE		= 6,	//简化规格对照表
	BOM_JGZJ		= 7,	//角钢准据表
	BOM_GENKAI		= 8,	//铁塔根开表
};

//标准视图类型
//TODO：目前由于哈希表键值原因STD_VIEW_TYPE必须从9开始，未来应仔细审查架构问题 wjh-2018.5.18
enum STD_VIEW_TYPE{
	STD_VIEW_FILLPLATE	= 9,	//填板
	STD_VIEW_JOINT		=10,	//接头
	STD_VIEW_FOOTNAIL	=11,	//脚钉
};

//绘图视图类型
enum DRAW_COMPONENT_TYPE{
	DRAW_COM_LINEVIEW		= 1,	//单线图
	DRAW_COM_STRUVIEW		= 2,	//结构图
	DRAW_COM_STDVIEW		= 3,	//标准图
	DRAW_COM_PARTVIEW		= 4,	//零件图
	DRAW_COM_BOM			= 5,	//物料清单
	DRAW_COM_OTHER			= 100,	//其他
};

enum DRAW_CMD_TYPE{
	CMD_DRAW_PART=1,		//绘制构件结构图
	CMD_DRAW_TABLE=2,		//绘制表格
	CMD_DIM_PARTSPEC=3,		//标注构件明细
	CMD_DIM_ODDMENT=4,		//标注正负头
	CMD_DIM_BOLTPAD=5,		//标注垫圈信息
	CMD_DIM_BOLTINFO=6,		//标注螺栓信息
	CMD_DIM_SIMPOLY=7,		//标注相似形
	CMD_DIM_MODELSIZE=8,	//标注模型尺寸
	CMD_DIM_MAPSIZE=9,		//标注图面尺寸
	CMD_DIM_SPECIAL_LSSPACE=10,	//标注螺栓特殊间距
	CMD_DIM_SPECIAL_JGZHUN=11,	//标注角钢特殊准据
	CMD_DRAW_LINEPART=12,		//绘制杆件单线图
	CMD_DIM_SEGI=13,			//标注段号	
	CMD_DIM_FOOTNAIL_SPACE=14,	//脚钉间距
	CMD_DRAW_ASSIST_LINE=15,	//绘制辅助线
	CMD_DIM_BOLTORFL_INFO=16,	//司令图中标注杆件始端或终端螺栓信息或法兰信息 wht 12-06-01
	CMD_DIM_FILLPLATE_SPACE=17,	//填板间距
	CMD_DIM_ANGLEJOINT=18,		//标注司令图中的接头信息 wht 14-07-02
	CMD_DRAW_JG_JOINT=19,		//绘制角钢接头
	CMD_OTHER=100,				//其他
};
//模型尺寸标注类型
enum DIM_MODE_SIZE_TYPE{
	DIM_NODE_VSPACE=0,		//节间垂距     
	DIM_HENGDAN_VSPACE=1,	//横担挂线点间垂直间距
	DIM_SEG_VSPACE=2,		//分段间距
	DIM_TOWERHIGH=3,		//塔高
	DIM_SLOPE_HSPACE=4,		//变坡开口
	DIM_HENGDAN_HSPACE=5,	//挂线点水平间距
};
//相似形轮廓点
struct ANGLEWING_OFFSET{
	WORD wiData;
public:
	ANGLEWING_OFFSET(){wiData=0;}
	BYTE get_ciStyleG();	//
	BYTE set_ciStyleG(BYTE _ciStyleG);
	__declspec(property(put=set_ciStyleG,get=get_ciStyleG)) BYTE ciStyleG;	//角钢肢偏移量:0.标准准距;1:g1;2:g2;3:g3;4:自定义
	BYTE get_ciSectStyle();
	BYTE set_ciSectStyle(BYTE _ciSectStyle);
	__declspec(property(put=set_ciSectStyle,get=get_ciSectStyle)) BYTE _ciSectStyle;//0.节点垂向截面心线点;1.节点X-Y平面心线点;2.节点Y-Z平面心线点;3.节点X-Z平面心线点
	short get_siOffsetDist();
	short set_siOffsetDist(short gDist);
	__declspec(property(put=set_siOffsetDist,get=get_siOffsetDist)) short siDistG;	//心距(-511mm ~ 511mm)
};
struct SIMPOLY_VERTEX
{
	//hNextPole>0时:标准心线交点;其余参照hDatumRod及wxDatumRodOffset确定
	ANGLEWING_OFFSET wxDatumRodOffset;
	long hDatumRod;
	long hViceRod;
	GEPOINT vertex;
	SIMPOLY_VERTEX(){memset(this,0,sizeof(SIMPOLY_VERTEX));}
};
//剖视图符号
class CStruView;
class CSectionViewSymbol
{
public:
	double fLenght;			//长度
	double fTextSize;		//字体高度
	double fPolyLineWidth;	//剖视图多段线宽度
	f3dPoint startPt;		//剖视图始端位置
	f3dPoint endPt;			//剖视图终端位置
	BOOL bUpturned;			//startPt->entPt顺时针旋转90°为下,逆时针旋转90°为上
	CXhChar16 viewName;		//剖视图名称
	//
	long hAttachViewKey;	//从属视图(即包含剖视图符的视图)key值
	long hViewKey;			//剖视图描述的视图
	ATOM_LIST<AcDbObjectId> startEntIds;//起点关联实体(不存储)
	ATOM_LIST<AcDbObjectId> endEntIds;	//终点关联实体(不存储)
public:
	CSectionViewSymbol();
	CSectionViewSymbol(f3dPoint start,f3dPoint end,CXhChar16 name);
	void Draw();								//绘制剖视图符
	void ModifySymbolName(char *symbol_name);	//修改剖视图名称
	void EraseRelativeAcadEntity();				//删除相关的CAD实体
	void UpdatePosition(double fOffsetX,double fOffsetY);//更新剖视图符位置
	f2dRect GetDrawRegion();					//得到绘图区域
	SCOPE_STRU GetEntScope(int start0_end1_all2);
	void GetEntSet(ads_name &drag_ent_set,int start0_end1_all2=2,BOOL bSelectEnts=FALSE);//获得剖视图符对应的实体集合
	static CSectionViewSymbol* AddSectionViewSymbol(CStruView *pMainView,CStruView *pStruView);
	void CopyProperty(CSectionViewSymbol *pSymbol);
	//
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};
//角钢接头俯视图
struct JOINT_RELA_PARTS
{
	CLDSLineAngle *m_pShortJg;
	CLDSLineAngle *m_pDependJgS,*m_pDependJgE;
	CLDSPlate *m_pXWrapPlate,*m_pYWrapPlate;
	CLDSPlate *m_pXPadPlate,*m_pYPadPlate;
	BOOL m_bStartHasPad,m_bEndHasPad;
	//
	JOINT_RELA_PARTS()
	{
		m_pShortJg=NULL;
		m_pDependJgS=NULL;
		m_pDependJgE=NULL;
		m_pXWrapPlate=NULL;
		m_pYWrapPlate=NULL;
		m_pXPadPlate=NULL;
		m_pYPadPlate=NULL;
		m_bStartHasPad=FALSE;
		m_bEndHasPad=FALSE;
	}
};
class CJointWrapperView
{
	UCS_STRU ucs;
	int m_nGap;	//间隙值
	int x_off_vec,y_off_vec;
	f3dPoint datumPt1,datumPt2,datumPt3,datumPt4;
	BOOL m_bInnerWrap,m_bGroupJoint;	//
	ATOM_LIST<f3dArcLine> edgeList;
	ATOM_LIST<CSizeTextDim> spaceDimList;	//相对尺寸
	ATOM_LIST<CTextFrame> dataPointList;	//数据点
public:
	CLDSLineAngle *m_pShortJg;
private:
	BOOL ExtractRelaParts(CLDSLineAngle* pShortJg,JOINT_RELA_PARTS& relaParts);
	int CalGapDist(JOINT_RELA_PARTS& relaParts);
	void UpdateOffVec(f3dPoint wingX_vec,f3dPoint wingY_vec);
	CLDSLineAngle* FindOherShortJg(JOINT_RELA_PARTS& relaParts);
public:
	CJointWrapperView();
	~CJointWrapperView();
	//
	void CreateDrawing();
	void BuildDrawCs();
	void DrawLineAngleSection(CLDSLineAngle* pAngle,f3dPoint base_pnt,BOOL bShortJg=TRUE);
	void DrawPlateSection(CLDSPlate* pPlate,f3dPoint base_pnt,BOOL bPad=FALSE);
	void DimAngleLsSpace(CLDSLineAngle* pShortJg,CLDSLineAngle* pAngle,f3dPoint base_pnt,double plank_thick=2);
	//
	f3dArcLine* EnumFirstArc(){return edgeList.GetFirst();}
	f3dArcLine* EnumNextArc(){return edgeList.GetNext();}
	CSizeTextDim* EnumFirstSizeDim(){return spaceDimList.GetFirst();}
	CSizeTextDim* EnumNextSizeDim(){return spaceDimList.GetNext();}
	CTextFrame* EnumFirstTextFrm(){return dataPointList.GetFirst();}
	CTextFrame* EnumNextTextFrm(){return dataPointList.GetNext();}
};
//命令行状态
#define CMD_STATE_FREE			0x0		//空闲
#define CMD_STATE_NEXTPOS		0x01	//下一步要执行的命令
#define CMD_STATE_BREAKPOINT	0x02	//断点
#define CMD_STATE_BOOKMARK		0x04	//书签

class CDrawingComponent;
class CDrawingCommand : public CXeroDrawingCommand
{
	BOOL m_bFlag;	//用来表示该命令是否已经执行过
	long m_nDrawingSerialNo;	//构件绘制顺序号
public:
	static CHashList<AcDbObjectId>hashHighlightEnts;
	long feature;
	CDrawPartTagInfo drawPartInfo;
	CDrawLineAngleTagInfo drawAngleInfo;
	CDrawingComponent *m_pDrawingCom;
public:
	CDrawingCommand();
	virtual ~CDrawingCommand();
	virtual IDrawingComponent* BelongDrawingComponent(){return (IDrawingComponent*)m_pDrawingCom;}
	typedef struct tagPART_PARA
	{
		CLDSPart *pPart;		//读取构件句柄后得到pPart指针
		long hPart;				//构件句柄,文件关闭再打开后指针失效故改用句柄
		double fPoleAxisLineStartOutSpreadLen;	//杆件心线始端延伸长度
		double fPoleAxisLineEndOutSpreadLen;	//杆件心线终端延伸长度
		f2dPoint drawStartBasePos,drawEndBasePos;//杆件始末端的绘制基点,暂时记录用于标注正负头时用
		char cDrawFlag;			//构件绘制面 'X'.X截面 'Y'.Y截面,'Z'.Z截面 自动判断 '0'.平铺绘制 '1'.竖直绘制
		struct PARA
		{	
			//角钢绘制参数
			struct{
				int nXWingDatumAxisG;	//角钢X肢基准心距
				int nYWingDatumAxisG;	//角钢Y肢基准心距
				BOOL bDrawAxisLine;		//绘制角钢准线，暂保留以备将来设置角钢是否绘制基准线开关时不用更新文件版本 wjh-2011.8.31
				short extendStyle;		//肢宽夸大绘制时的拓展方式,0:两侧拓展;1:向肢翼侧拓展;2:向楞线侧拓展
			}angle;
			//螺栓绘制参数
			struct{
				BOOL bOriginal;		//是否正本螺栓
				bool bDimHoleDiameter;	//对于虚螺栓孔是否标注螺栓孔直径
				bool bDrawSideBoltLine;	//平放绘制等长直线
				short dimAngle;		//标注旋转角度(°)
				short dimFarOffset;	//标注文字偏移距离
				int front0_side1;	//正面螺栓还是侧面螺栓
			}bolt;
		}para;
	}PART_PARA;
	typedef struct tagLINEPART_PARA
	{
		CLDSLinePart *pLinePart;//读取杆件句柄后得到pLinePart指针
		long hLinePart;			//构件句柄,文件关闭再打开后指针失效故改用句柄
		BOOL bSumMap;			//是否为总图
		bool bAdjoinRod;		//绘制横担同时需要绘制的毗邻塔身段杆件简化线，应绘为虚线 wjh-2018.5.22
		BOOL bDimByAnalysisMode;//是否为力学分析模式
		f2dPoint drawStartBasePos,drawEndBasePos;//杆件始末端的绘制基点,暂时记录用于标注螺栓信息或法兰信息时用
	}LINEPART_PARA;
	typedef struct tagPART_SPEC_PARA{
		CLDSDbObject *pObj;		//读取对象句柄后得到pObj指针
		long hObj;				//构件句柄,文件关闭再打开后指针失效故改用句柄
		double fBasePosOffsetX;	//数据点位置X坐标偏移量	wht 11-07-25
		double fBasePosOffsetY;	//数据点位置Y坐标偏移量
		double fDimPosOffsetX;	//标注位置X坐标偏移值
		double fDimPosOffsetY;	//标注位置Y坐标偏移值
		double fGuigeOffsetX;	//钢板规格标注位置X坐标偏移值
		double fGuigeOffsetY;	//钢板规格标注位置Y坐标偏移值
		int  iDimPosType;		//0.自动搜索位置 1.指定位置(保存指定位置与原标注位置的偏移值) 2.选择位置
		BOOL bOnlyDimPartNo;	//仅标注杆件编号
		//新加变量 wht 10-12-03
		BOOL bDrawRefLine;		//是否绘制引出线
		double fPoleDimPosCoef;	//杆件标注位置系数 //LTMA 0.3  TAS 0.5
		int iMirMsg;			//0:X轴对称；1:Y轴对称;其余不对称处理
	}PART_SPEC_PARA;
	typedef struct tagODDMENT_PARA{
		CLDSLinePart *pLinePart;//读取杆件句柄后得到pLinePart指针
		long hLinePart;			//构件句柄,文件关闭再打开后指针失效故改用句柄
		//新加属性,用于实现正负头标注位置可调整 wht 11-07-26
		int iDimPosType;		//0.自动搜索位置 1.指定位置
		BOOL bDimStartOdd;
		BOOL bDimEndOdd;
		double fStartOddBasePosX;	//始端正负头标注基点X分量(临时存储方便手动偏移,不存入脚本文件)
		double fStartOddBasePosY;	//始端正负头标注基点Y分量(临时存储方便手动偏移,不存入脚本文件)
		double fEndOddBasePosX;		//始端正负头标注基点X分量(临时存储方便手动偏移,不存入脚本文件)
		double fEndOddBasePosY;		//始端正负头标注基点Y分量(临时存储方便手动偏移,不存入脚本文件) -wjh 2011.9.25
		double fStartOddPosOffsetX;	//相对于杆件端头基准位置(角钢楞点或,钢管中心线端点,未计正负头)
		double fStartOddPosOffsetY;	//相对于杆件端头基准位置(角钢楞点或,钢管中心线端点,未计正负头)
		double fEndOddPosOffsetX;	//相对于杆件端头基准位置(角钢楞点或,钢管中心线端点,未计正负头)
		double fEndOddPosOffsetY;	//相对于杆件端头基准位置(角钢楞点或,钢管中心线端点,未计正负头)
		double fPoleAngle;
	}ODDMENT_PARA;
	typedef struct tagBOLTPAD_PARA{
		CLDSBolt *pBolt;		//读取螺栓句柄后得到pBolt指针
		long hBolt;				//构件句柄,文件关闭再打开后指针失效故改用句柄
		int iDimPosType;		//0.自动搜索位置 1.指定位置
		double fDimPosOffsetX;	//标注位置X坐标偏移值
		double fDimPosOffsetY;	//标注位置Y坐标偏移值
	}BOLTPAD_PARA;
	typedef struct tagBOLTINFO_PARA{
		CLsGroupDimUnit lsGroupDim;
		int iDimPosType;		//0.自动搜索位置 1.指定位置
		double fDimPosOffsetX;	//标注位置X坐标偏移值
		double fDimPosOffsetY;	//标注位置Y坐标偏移值
	}BOLTINFO_PARA;
	typedef struct tagSIMPOLY_PARA{
		int nSimPolyCmdType;	//相似形命令类型 0.点选拾取点添加相似形 1.选择轮廓点添加相似形 2.已指定插入点的相似形 3.未指定插入点的相似形
		double fScale;			//指定相似形比例
		double fPickPtX;		//拾取点X坐标(相对坐标)
		double fPickPtY;		//拾取点Y坐标(相对坐标)
		bool   bPickPosIsLocalCoord;	//拾取点坐标是相对于视图原点局部坐标，而不是相对于整张图纸原点，绘制时需要转为基于图纸原点 wjh-2018.5.22
		bool   bDrawRefLine;	//当存在相似形偏移时，是否绘制引出线 wjh-2017.8.15
		double fDimPosOffsetX;	//相似形标注位置X轴方向偏移量		
		double fDimPosOffsetY;	//相似形标注位置Y轴方向偏移量
		tagSIMPOLY_PARA(){bDrawRefLine=bPickPosIsLocalCoord=false;fDimPosOffsetX=fDimPosOffsetY=0;fScale=0.3;}
		ATOM_LIST<SIMPOLY_VERTEX> simPolyVertexList;	
	}SIMPOLY_PARA;
	typedef struct tagBOM_TABLE_PARA{
		int nLocatePos;		//0.左上 1.右上 2.左下 3.右下
		BOOL bSelectPos;	//选择表格位置
		BOOL bDisplayDlg;	//显示选择定位定位置的对话框
		double fDimPosX;	//表格位置X坐标		
		double fDimPosY;	//表格位置Y坐标
		double fTableWidth;	//表格长度
		double fTableHeight;//
		int iTableType;		//表格类型 
		int iFormatTbl;		//iFormatTbl=1、2表示该命令与命令所属图元的xFormatTbl1、xFormatTbl2对应。 wht 11-07-14
	}BOM_TABLE_PARA;
	//标注模型尺寸,图面尺寸
	typedef struct tagDIMSIZE_PARA{
		//图面尺寸标注起始点(相对坐标系下)
		double fDimStartX;
		double fDimStartY;
		//图面尺寸标注起始点(相对坐标系下)
		double fDimEndX;
		double fDimEndY;
		//图面尺寸标注位置(相对坐标系下)
		double fDimPosX;
		double fDimPosY;
		double fScale;	//尺寸比例
		int iDimDirect;	//标注方向 0.X轴方向 1.Y轴方向 2.延伸方向		
	}DIMSIZE_PARA;
	//标注模型尺寸
	struct DIMNODE{
		long hNode;
		CLDSNode* pNode;
		DIMNODE(CLDSNode* _pNode=NULL) {
			pNode=_pNode;
			hNode=_pNode!=NULL?_pNode->handle:0;
		}
	};
	struct DIM_MODELSIZE_PARA{
		static CLDSNode xMidAxisNode;
		ATOM_LIST<DIMNODE> listDimNodes;	//节点句柄链表(文件关闭后再打开节点指针发生变化,故此处保存节点链表)
		int iDimType;		//标准类型0为节间垂距 1为横担挂线点间垂直间距 2为分段间距 3为塔高 4变坡开口 5为挂线点水平间距
		double fDimDistance;//标注文字与标注起始点间距值
		double fSpace;		//标注起始点与塔身间距值
		double fMinX;       //纵向标注时最左边节点的X坐标
		BOOL bUpDim;        //水平标注时标注偏移方向
	public:
		DIMNODE* AppendDimNode(CLDSNode* _pNode=NULL);
	};
	//标注角钢特殊准据
	typedef struct tagDIM_JGZHUN_PARA
	{	
		CLDSLineAngle *pLineAngle;	//读取角钢句柄后得到pLineAngle指针
		long hLineAngle;			//构件句柄,文件关闭再打开后指针失效故改用句柄
		CZhunXianTextDim zhun_dim;
		double fDimDistance;		//标注文字与标注起始点的间距值
		double fSpace;				//标注起始点与螺栓的间距值
		BOOL bInside;				//控制标注方向
	}DIM_JGZHUN_PARA;
	//标注螺栓特殊间距
	typedef struct tagDIM_LSSPACE_PARA
	{	
		CLDSPart *pPlate;			//读取构件句柄后得到pPart指针
		long hPlate;				//构件句柄,文件关闭再打开后指针失效故改用句柄
		CLsSpaceDimUnit spaceDim;	//螺栓间距记录
		double fDimDistance;		//标注文字与标注起始点的间距值
		double fSpace;				//标注起始点与螺栓的间距值
		BOOL bInside;				//控制标注方向
	}DIM_LSSPACE_PARA;
	//标注段号
	typedef struct tagDIM_SEGI_PARA
	{
		int bSelectPos;		//0.已设定位置;1.选择位置;2.自动初始化为分段中心点为基准的位置偏移
		SEGI nSegI;			//关联段号
		double fDimPosX;	//标注位置X坐标(绘图坐标系下，参照原点取决于bSelectPos)
		double fDimPosY;	//标注位置Y坐标(绘图坐标系下，参照原点取决于bSelectPos)
	}DIM_SEGI_PARA;
	typedef struct tagDIM_FOOTNAILSPACE_PARA{
		CLDSLinePart *pLinePart;//读取杆件句柄后得到pLinePart指针
		long hLinePart;			//构件句柄,文件关闭再打开后指针失效故改用句柄
		double fDimDistance;	//标注文字与标注起始点的间距值
		double fSpace;			//标注起始点与杆件外侧的间距值
	}DIM_FOOTNAILSPACE_PARA;
	typedef struct tagDRAW_ASSIST_LINE
	{
		int iNo;	// 辅助线索引号
		int nStyle;	// 1.辅助心线 2.辅助火曲线
	}DRAW_ASSIST_LINE;
	typedef struct tagANGLEJOINT_PARA{
		ANGLE_JOINT *pJoint;	//读取角钢接头id后得到角钢接头指针
		long idAngleJoint;		//接头id,文件关闭再打开后指针失效故改用句柄
		int iDimPosType;		//0.自动搜索位置 1.指定位置
		double fDimPosOffsetX;	//标注位置X坐标偏移值
		double fDimPosOffsetY;	//标注位置Y坐标偏移值
	}ANGLEJOINT_PARA;
public:
	DRAW_CMD_TYPE m_eCmdType;	//命令类型
	BYTE m_stateCmd;			//命令状态
	BOOL m_bExecute;			//是否执行该命令
	ATOM_LIST<AcDbObjectId> entIdList;	//命令关联的实体ID列表
	//命令参数
	PART_PARA m_paraPart;
	LINEPART_PARA m_paraLinePart;
	PART_SPEC_PARA m_paraPartSpec;
	ODDMENT_PARA m_paraOddment;
	BOLTPAD_PARA m_paraBoltPad;
	BOLTINFO_PARA m_paraBoltGroupInfo;
	SIMPOLY_PARA m_paraSimPoly;
	BOM_TABLE_PARA m_paraBOMTable;
	DIMSIZE_PARA m_paraDimSize;
	DIM_JGZHUN_PARA m_paraDimJgZhun;
	DIM_LSSPACE_PARA m_paraDimLsSpace;
	DIM_MODELSIZE_PARA m_paraDimModelSize;
	DIM_SEGI_PARA m_paraDimSegI;
	DIM_FOOTNAILSPACE_PARA m_paraDimFootNailSpace,m_paraDimFillPlateSpace;
	DRAW_ASSIST_LINE m_paraDrawAssistLine;
	ODDMENT_PARA m_paraBoltOrFlInfo;	//司令图中螺栓或法兰信息 wht 12-08-01
	ANGLEJOINT_PARA m_paraAngleJoint;	//司令图中标注角钢接头信息 wht 14-07-03
public:
	void InitCommandPara(int eDrawCmdType=0);	//根据命令类型初始化命令参数
	int GetDrawType(char *sDrawType=NULL);	//0.平铺绘制 1.竖直绘制 2.端面绘制 wht 11-07-22
	long GetCmdInfo(char *sCmdId,char *sHelp,char *sPartNo=NULL);//得到命令信息 标识字符串以及描述信息
	CString GetDrawPartName();			//获取当前绘制构件的名称
	BOOL GetDCS(CDrawCoordTrans &dcs);			//由绘图坐标系与平移偏移量计算得到新的坐标系
	void SetCmdFlag(BOOL bFalg=TRUE){m_bFlag=bFalg;} //设置命令标志位 用来表示该命令是否已执行
	BOOL IsNeedExecute() {return !m_bFlag&&m_bExecute;}	//判断该命令是否需要执行
	int Run();				//执行该命令
	int Rerun();			//重新运行该命令
	void EmptyEntList();	//删除由该命令生成的实体
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
	void CopyProperty(CDrawingCommand *pCommand);
	SCOPE_STRU GetCADEntScope();
	void UpdateCommandPara(double fOffsetX,double fOffsetY);
	//获得命令对应的实体集合
	int GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts=FALSE);
	//设置命令对应的实体集合为高亮显示
	void SetEntSetHighlight(int type=0);	//type仅暂时用于高亮显示角钢正负头,0.表示全部,1.表示始端,2.表示终端
	static void ReleaseHighlightEnts();
	//通过句柄更新图纸中与塔相关的指针,保存后再次打开文件时使用 wht 11-07-22
	void UpdatePtrByHandle();
	//设置构件绘制顺序号
	int SetDrawingSerialNo(int serialNo);
	//得到构件绘制顺序号
	int GetDrawingSerialNo();
private:
	//得到段号对应的实体集合及所在区域
	BOOL GetEntSetAboutDimSegICmd(ATOM_LIST<AcDbObjectId> &entIdList,BOOL bHighlight,SCOPE_STRU *pScope=NULL);
public:
	SEGI GetSegI(void);
};

typedef struct tagCAD_OBJECT_ID
{
	int entType;	//1.视图名称 2.上接段号
	AcDbObjectId entId;
	DWORD dwDataRef;	
	tagCAD_OBJECT_ID(){entId.setNull();entType=0;dwDataRef=0;}
	tagCAD_OBJECT_ID(int ent_type,AcDbObjectId ent_id,DWORD ref_data=0)
	{	
		dwDataRef=ref_data; 
		entType=ent_type;
		entId=ent_id;
	}
}CAD_OBJECT_ID;
//绘图规则:
//1.同一张视图中涉及关联（杆件端点）节点仅允许绘制一次
//2.同一张视图中涉及接头划线符号仅允许绘制一次
//3.一张司令图纸（含多个视图)中有关联对称的杆件及接头至少绘制一次，每关联对称组合仅允许标注一次,且必须标注一次
struct DRAWN_STATE{	//一张图纸中每根杆件最多绘制一次，有关联对称的杆件至少绘制一根，规格只能标注一次
	bool drawn;		//已绘制
	bool dimspec;	//杆件规格已标注过
	DRAWN_STATE(){drawn=dimspec=false;}
};
class CDrawing;
class CDrawingComponent : public CXeroDrawingComponent
{
protected:
	PARTSET partSet;
	NODESET nodeSet;
	ATOM_LIST<long> partHandleList;
	ATOM_LIST<long> nodeHandleList;
	f2dRect draw_rect; 
	virtual void GetDrawingObjectSet(int bInitDCS){;}	//获取需要绘制的对象集合
	UCS_STRU dcs;				//绘图坐标系,由塔坐标系转到Acad世界坐标系的转换坐标系
	char m_cCurrSegType;	//'T','S','L'
public:
	CHashList<CDrawLineAngleTagInfo>hashAngleDrawInfo;
	f3dPoint drawing_offset;	//由Acad世界坐标系默认绘制基点平移到图面目标基点所需的偏移矢量（只涉及X和Y向偏移）
	long feature;				//临时使用不存储
	//CDrawing *m_pDrawing;		//所属图纸
	long m_hAttachView;			//关联视图句柄
	CDisplayView *m_pAttachView;//关联视图
	long m_hModule;				//关联呼高句柄
	CLDSModule *m_pModule;		//司令图或总图中绘制非当前呼高塔腿单线图
	CXhChar16 m_sDrawingLayer;	//图层范围
	double m_fNamePosSpace;		//视图名称与视图底部的间隙值

	CXhChar50 m_sName;	//视图名称
	BOOL m_bDimName;	//标注视图名称
	CXhChar16 m_sSectionViewCode;		//剖视图代号
	DRAW_COMPONENT_TYPE m_eClassType;	//视图类型 
	TBL_LOCATE_STYLE m_nAnchorType;		//定位类型 0.左上 1.右上 2.左下 3.右下
	BOOL m_bSpecifyComPos;				//指定图元位置
	BOOL m_bNeedAdjust;					//该视图需要调整
	BOOL m_bFirstCreate;				//第一次生成该视图，需要自动计算初始装配位置
	double m_fScale;					//绘制比例
	BOOL m_bDimScale;					//标注绘制比例
	ATOM_LIST<CDrawingCommand> cmdList;	//绘图命令链表
	CHashList<AcDbObjectId> keyEntIdList;//对应的CAD中实体ID,标注构件明细时使用
	ATOM_LIST<CAD_OBJECT_ID> entIdList;	//实体ID列表 视图名称Id,上接段号Id 用于记录名称对应的实体ID便于调整位置 
public:
	CDrawingComponent(int id=0,IDrawing* pDrawing=NULL);
	virtual ~CDrawingComponent();
	
	UCS_STRU GetDCS() const{return dcs;}
	void SetDCS(UCS_STRU ucs){dcs=ucs;}
	CLDSPart *GetFirstPart(){return partSet.GetFirst();}
	CLDSPart *GetNextPart(){return partSet.GetNext();}
	long GetPartCount(){return partSet.GetNodeNum();}
	CLDSNode *GetFirstNode(){return nodeSet.GetFirst();}
	CLDSNode *GetNextNode(){return nodeSet.GetNext();}
	long GetNodeCount(){return nodeSet.GetNodeNum();}
	BOOL IsNeedSpecifyComPos();			//需要制定视图摆放位置
	f3dPoint GetMapFrmInsideTopLeft();	//得到基准点即图框内侧矩形左上角顶点
	AcDbEntity *FindCADEntByHandle(long hObj);	//根据构件句柄得到指定的CAD实体
	f3dPoint SpecifyAssemblyPosition();				//指定视图装配位置
	void CalInitAssemblyPosition();				//计算视图初始装配位置
	//void CalAssemblyPosByRelativePos();		//通过相对位置计算装配位置
	void InsertDrawingName();					//插入视图名称
	//修改定位点相对位置
	void ModifyAnchorPointRelativePos(double *pOffsetX,double *pOffsetY);
	f3dPoint GetAnchorPointRelativePos();				//获取定位点相对于图框左上角的相对位置
	void UpdatePtrByHandle();					//通过句柄更新图纸中与塔相关的指针,保存后再次打开文件时使用 wht 11-07-22
	void InitCmdFeature(int feature);			//初始化组件命令对应的feature wht 11-07-22
	//获取附属实体集
	void GetAttachEntSet(ads_name &drag_ent_set,int name1_seg2_all0=0,
		BOOL bSelectEnts=FALSE,SCOPE_STRU *pScope=NULL);
	void ModifyComponentName(char *com_name);	//修改组件名称
	//获得视图对应的实体集合
	virtual void GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts=FALSE);	
	virtual int UpdateDrawingRectArray();		//生成视图后需要更新绘图区域
	virtual f2dRect GetComponentRect();			//视图绘图区域
	virtual void Create();						//生成视图
	virtual void EraseRelaCadEnt();				//删除关联的CAD实体 wht 13-04-19
	virtual void ReCreate();					//删除当前视图关联数据重新生成
	//在计算视图真实绘图区域时通过bIncludePartEnt控制仅计算构件绘制区域(获取名称标注位置时使用)
	virtual f2dRect GetDrawRegion(BOOL bRealRect=TRUE,BOOL bOnlyIncludePartEnt=FALSE);	//得到绘图矩形区域
	virtual BOOL CreateCmdList(BOOL bInit=TRUE){return TRUE;}//生成命令链表
	virtual void CopyProperty(CDrawingComponent* pCom);	//复制属性
	virtual void UpdateCmdList(BOOL bUpdateViewDcs=FALSE){;}						//更新命令列表 wht 11-07-31
	//
	virtual void FromBuffer(CBuffer &buffer, long version=NULL);
	virtual void ToBuffer(CBuffer &buffer);
public:	//便于减少代码量而写的简易封装函数
	CDrawing* ParentDrawing(){return (CDrawing*)m_pParentDrawing;}
private:
	CHashList<DRAWN_STATE> hashRodDrawnStates;
	CHashList<DRAWN_STATE> hashJointDrawnStates;
	CHashList<DRAWN_STATE> hashNodesDrawn;	//标记指定节点是否已经在当前视图中绘制过(目前是通过CLDSNode::is_visible设定来实现)
public:
	static const int TYPE_ROD	= 1;
	static const int TYPE_JOINT = 2;
	static const int TYPE_NODE	= 3;
	void ClearDrawnStates(int stateType=TYPE_ROD);
	bool IsItemDrawn(int stateType,long idItem);	//返回指定项目是否在当前图纸中绘制过
	bool IsItemDimmed(int stateType,long idItem);	//返回指定项目是否在当前图纸中标注过
	void MarkItemDrawnState(int stateType,long idItem,bool drawn=true);
	void MarkItemDimSpecState(int stateType,long idItem,bool dimmed=true);
};

class CDrawingBOM : public CDrawingComponent
{
public:
	DRAW_BOM_TYPE m_eBOMType;	//表格类型
	ATOM_LIST<SPEC_MARK> SpecList;	//简化规格时使用
	BOOL m_bAutoNewTable;		//表格过长时自动生成新的表格
	BOOL m_bHasTableHead;		//新表格是否带表头
	CFormatTbl xFormatTbl1,xFormatTbl2;
	int m_nBriefGuigeN;			//简化规格数，杆件简化表格表使用 wht 11-06-15
public:
	CDrawingBOM(int id=0,IDrawing* pDrawing=NULL);
	~CDrawingBOM();
	//
	BOOL PrepareTable();
	void Create();				//生成视图
	BOOL CreateCmdList(BOOL bInit=TRUE);//生成命令链表
	void UpdateTableInitPos();	//更新表格初始位置
	f2dRect GetComponentRect();	//视图绘图区域
	int UpdateDrawingRectArray();//生成视图后需要更新绘图区域
	void CopyProperty(CDrawingComponent* pCom);	//复制属性
	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
};

//单线图
class CLineView : public CDrawingComponent
{
	double TOPEST_Z;			//杆塔最高点
	double TOPEST_TRUNK_Z;		//塔身最高点
	double LOWEST_WIREPOINT_z;	//最低挂点
protected:
	void GetAjointSegInfo();
	void GetDrawingObjectSet(int bInitDCS);	//获取需要绘制的对象集合
	//返回标注时X坐标
	//iType: 0.为节间垂距 1.横担挂线点间垂直间距 2.分段间距 
	//		 3.塔高 4.变坡开口 5.挂线点水平间距 
	double ChooseFeatPt(NODESET &nodesetSeleted,int iType=0,char cDrawAxisX='X');
	void CreateDimPartSpecCmdList();	//生成构件明细标注命令列表
	void CreateDimBoltOrFlInfoCmdList();	//生成螺栓明细标注命令列表	
	void CreateAngleJointCmdList();			//生成角钢接头标注命令列表	
public:
	struct RODLINE{
		long hRod;
		GELINE briefline;
		RODLINE(){hRod=0;}
		RODLINE(GELINE line,long _hRod=0)
		{
			briefline=line;
			hRod=_hRod;
		}
		RODLINE(const double* lineStart,const double* lineEnd,long _hRod=0)
		{
			briefline.start=lineStart;
			briefline.end=lineEnd;
			hRod=_hRod;
		}
	};
	struct DIMSIZE_NODE{
		long hNode;
		char cbDimSizeFlag;					//一览图/总图/司令图中需要进行的尺寸标注项，0x01:pos.x;0x02;pos.y;0x04:pos.z;
		long hDimCmdCoordX;
		long hDimCmdCoordY;
		long hDimCmdCoordZ;
	public:
		DIMSIZE_NODE();
		void SetKey(DWORD key){hNode=(long)key;}
	public:
		bool get_blNeedAddDimCoordX();
		bool get_blNeedAddDimCoordY();
		bool get_blNeedAddDimCoordZ();
		__declspec(property(get=get_blNeedAddDimCoordX)) bool blNeedAddDimCoordX;
		__declspec(property(get=get_blNeedAddDimCoordY)) bool blNeedAddDimCoordY;
		__declspec(property(get=get_blNeedAddDimCoordZ)) bool blNeedAddDimCoordZ;
	};
	CHashListEx<DIMSIZE_NODE> hashNodeDimensions;
public:
//iType: 0.为节间垂距 1.横担挂线点间垂直间距 2.分段间距 
//		 3.塔高 4.水平标注(塔头及塔身开口尺寸) 5.挂线点水平间距 
	static const int DIM_SECTION_Z			= 0;	//为节间垂距标注
	static const int DIM_SEG_SPACE_Z		= 1;	//分段间距
	static const int DIM_GUAXIAN_SPACE_Z	= 2;	//横担挂线点间垂直间距
	static const int DIM_TOWER_HEIGHT		= 3;	//塔高
	static const int DIM_SHANK_SPACE_XY		= 4;	//塔身坡段开口尺寸
	static const int DIM_GUAXIAN_SPACE_XY	= 5;	//塔头(含挂线点)水平间距标注
	static BOOL m_bUsePolyLineSumMap;	//
	static BOOL m_bUsePolyLineChiefMap;
	BOOL m_bAnalysisMode;	//力学分析模式
	BOOL m_bDimNodeNo;		//标注节点编号,力学分析时该变量有效 wht 11-06-14
	BOOL m_bDimPoleGuige;	//标注杆件规格,力学分析时该变量有效
	CHashList<AcDbObjectId> jointEntIdList;			//接头CAD实体id，司令图中标注接头信息时使用 wht 14-07-03
	PREVIEW preview;
	CUnifyModelLayer trunk;
	void CreateHorzDimensions();	//生成水平尺寸标注
	void CreateVertDimensions(double BASE_ZERO_Z=0);	//生成垂直尺寸标注
public:
	CLineView(int id=0,IDrawing* pDrawing=NULL);
	~CLineView();
	
	void EraseRelaCadEnt();				//删除关联的CAD实体 wht 14-07-03
	void Create();						//生成视图
	void IntelliCreateDimensions();		//根据塔头/横担类型识别的结果智能识别尺寸标注
	void CreateDimensions();
	void CreateSegmentLabelDimensions(SEGI *pOnlyDrawSegI=NULL,SEGI *pAdjoinTrunkSegI=NULL);
	BOOL CreateCmdList(BOOL bInit=TRUE);//生成命令链表
	void UpdateCmdList(BOOL bUpdateViewDcs=FALSE);	//更新命令列表 wht 11-07-31
public:	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
};

//////////////////////////////////////////////////////////////////////////
//展开线
class CDrawingSpreadLine
{
	CLDSLineAngle *m_pLineAngle;	//基准角钢，避免重复查找角钢
public:
	CXhChar50 m_sLineName;	//展开线名称
	BYTE m_iDefType;		//0.指定杆件 1.直接指定
	long m_hDatumPole;		//基准杆件 
	BYTE m_iCurWing;		//0.X肢 1.Y肢
	double m_fOffsetDist;	//偏移距离
	f3dLine m_lineSpread;	//展开线
	
	CDrawingSpreadLine();
	CLDSLineAngle *GetDatumPole();
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};
//////////////////////////////////////////////////////////////////////////
//杆件标注位置，判断同件号构件那个应该标注规格和长度时使用
class CRodDimPos
{
private:
	long hRod;
	f3dPoint startPos,endPos,dimPos;
	f3dPoint CalRodDimPos()
	{
		double fDimPosCoef=0.3;
		double angle=Cal2dLineAng(startPos.x,startPos.y,endPos.x,endPos.y);
		if(angle>Pi/2.0&&angle<=1.5*Pi)
		{
			angle-=Pi;
			dimPos=endPos+(startPos-endPos)*fDimPosCoef;
		}
		else
			dimPos=startPos+(endPos-startPos)*fDimPosCoef;
		return dimPos;
	}	
public:
	void InitRodDimPos(long hPart,f3dPoint start,f3dPoint end)
	{
		hRod=hPart;
		startPos=start;
		endPos=end;
		dimPos=CalRodDimPos();
	}
	CRodDimPos(long hPart,f2dPoint start,f2dPoint end)
	{
		f3dPoint start_pt(start.x,start.y,0),end_pt(end.x,end.y,0);
		InitRodDimPos(hPart,start_pt,end_pt);
	}
	CRodDimPos(long hPart,f3dPoint start,f3dPoint end)
	{
		InitRodDimPos(hPart,start,end);
	}
	CRodDimPos(){;}
	long GetRodHandle(){return hRod;}
	f3dPoint GetDimPos(){return dimPos;}
};
//////////////////////////////////////////////////////////////////////////
//结构图
class CStruView : public CDrawingComponent
{
	POLYPARTLIST polyPartlist;
	void GetDrawingObjectSet(BOOL bInitDCS);	//获取需要绘制的对象集合
	void CreateDrawingPartCmdList();//绘制构件命令列表
	void CreateDimOddmentCmdList();	//生成正负头标注命令
	void CreateDimPartSpecCmdList();//生成构件明细标注
	void CreateDimFootNailSpaceCmdList();	//生成脚钉间距标注命令
	void CreateDimFillPlateSpaceCmdList();	//生成填板间距标注命令
	void CreateDimSpecialLsSpaceCmdList();	//生成螺栓特殊间距标注命令
	void CreateDimSpecialJgZhunJuCmdList();	//生成角钢特殊准据标注命令
	void CreateDimBoltInfoCmdList();		//生成螺栓信息标注命令列表
	void CreateDimBoltPadCmdList();			//生成螺栓垫圈标注命令列表
	void CreateDrawAssistLineCmdList();		//绘制辅助线命令列表
	int  CalDualRodIntersPoint(CLDSLinePart* pPrevRod,CLDSLinePart* pNextRod,SIMPOLY_VERTEX* pBaseVertex,SIMPOLY_VERTEX* pInsertVertex);
	void CreateSimPolyDimensions();			//智能提取添加相似形标注命令
	PREVIEW preview;
public:
	static BOOL m_bDimPartSpec;
	static BOOL m_bDimPoleOddment;
	static BOOL m_bDimFootNailSpace;
	static BOOL m_bDimFillPlateSpace;
	static BOOL m_bDimLsSpace;
	static BOOL m_bDimJgZhunJu;
	static BOOL m_bDimBoltInfo;
	static BOOL m_bDimBoltPad;
	static BOOL m_bDimSimPoly;
public: 
	BOOL m_bDrawRefLine;	//是否绘制引出线
	int m_iMirMsg;			//0:X轴对称；1:Y轴对称;其余不对称处理
public:
	CStruView(int id=0,IDrawing* pDrawing=NULL);
	~CStruView();

	void Create();			//生成视图
	BOOL CreateCmdList(BOOL bInit=TRUE);//生成命令链表
	CString MakeSpreadFaceSetStr();
	CDrawingAssistant *GetDrawingAssistantByNo(int iNo);
	CDrawingAssistant *GetDrawingAssistantByHandle(long hPart);
	CLDSPolyPart *GetFirstPolyPart(){return polyPartlist.GetFirst();}
	CLDSPolyPart *GetNextPolyPart(){return polyPartlist.GetNext();}
	int GetPolyPartCount(){return polyPartlist.GetNodeNum();}
	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
	//标注剖视图符
	CSectionViewSymbol *GetSectionViewSymbol();
	//获得视图对应的实体集合
	void GetEntSet(ads_name &drag_ent_set,BOOL bSelectEnts=FALSE);
	//图面法线发生变化之后需要更新螺栓绘制方式
	void UpdatePartDrawTypeAndSpecialDim();
	void UpdateCmdList(BOOL bUpdateViewDcs=FALSE);	//更新命令列表 wht 11-07-31
	//得到命令关联实体集合 wht 12-03-22
	void GetDimBoltInfoCmdAttachEntSet(CDrawingCommand *pMainCmd, ATOM_LIST<AcDbObjectId> &entIdList);
	//根据构件句柄得到对应的绘制命令 wht 12-03-22
	CDrawingCommand* GetDrawingPartCmdFromHandle(long hPart);
	//从(除当前命令外的)螺栓标注命令中移除指定螺栓，并返回关联命令 
	void RemovBoltFromDimBoltInfoCmd(CDrawingCommand *pCurCmd,BOLTSET &boltSet,
									CHashList<CDrawingCommand*> &cmdHashList);
	//将指定命令移动到目标命令之后
	BOOL MoveSpecCmdAfterDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd);
	//将指定命令移动到目标命令之前
	BOOL MoveSpecCmdBeforeDestCmd(CDrawingCommand *pSpecCmd,CDrawingCommand *pDestCmd);
	//将指定序号对应的命令调整至目的序号对应的命令之后
	BOOL MoveSpecCmdSerialNoAfterDestCmdSerialNo(int specSerialNo,int destSerialNo);
	//
	void ReDimBoltInfo();
};

//零件图
class CPartDrawing : public CDrawingComponent
{
public:
	long m_hPart;
	CLDSPart *m_pPart;
	CLDSPolyPart polyPart;	//临时使用不存储
	BOOL m_bPolyPart;	
	CXhChar16 sPartNo;	//构件编号，便于根据构件编号自动更新构件句柄
	ATOM_LIST<long> sonPoleHandleList;
public:
	CPartDrawing(int id=0,IDrawing* pDrawing=NULL);
	~CPartDrawing();
	
	void Create();			//生成视图
	BOOL CreateCmdList(BOOL bInit=TRUE);//生成绘制零件图命令
	void CopyProperty(CDrawingComponent* pCom);	//复制属性
	//
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};
//标准视图
class CStdView : public CDrawingComponent
{
public:
	STD_VIEW_TYPE m_eStdViewType;	//标准视图类型
	CLDSPart *m_pPart;
public:
	CStdView(int id=0,IDrawing* pDrawing=NULL);
	~CStdView();

	void Create();			//生成视图
	BOOL CreateCmdList(BOOL bInit=TRUE);		//生成命令链表
	void CopyProperty(CDrawingComponent* pCom);	//复制属性
	//
	void FromBuffer(CBuffer &buffer, long version=NULL);
	void ToBuffer(CBuffer &buffer);
};

class CDrawing : public CXeroDrawing
{
public:
	long feature;				//临时使用不存储
	SEGI m_nSegI;				//关联段号 -1表示司令图,-2表示总图
	SEGI m_nAdjoinSegI;			//毗连段（一般为与横担直接相连的塔身段），0表示不绘制毗连段
	union{
		long liDrwPageSerial;		//同一类别或分段图纸中的子序号（0 based serial)
		struct{
			short liPageSerialInAll;	//在所有图纸中的序号（0 based serial)
			short liPageSerialInDrw;	//在同一分段(类)图纸中的子序号（0 based serial)
		};
	};
	long get_liDrwType() const{return m_nSegI.iSeg;}
	long set_liDrwType(long liDrawingType){return m_nSegI.iSeg=liDrawingType;}
	__declspec(property(put=set_liDrwType,get=get_liDrwType)) long liDrwType;	//图纸类别 -1表示司令图,-2表示总图
	UCS_STRU dcs;				//坐标系
	int m_nDrawingNumber;		//图纸数量
	MAP_FRAME_SIZE m_xMapFrm;	//图框
	CXhChar50 m_sDrawingName;	//图纸名称
	CXhChar50 m_sDrawingNo;		//图号
	BOOL m_bSpecifyMapFrmPos;	//指定图框位置
	long m_hActiveModule;		//当前激活呼高句柄
	CLDSModule *m_pActiveModule;//当前激活呼高
	BOOL m_bAutoLayoutComponent;//自动布局
	double m_fComponentSpace;	//相邻视图之间的绘图间隙
	//视图哈希链表key值规则:
	//1.总图、司令图  主体图	key=pView->handle-abs(m_nSegI);
	//				  呼高图	key=pView->handle+pModule->handle;
	//				  定制视图	key=pView->handle;
	//				  表格		key=表格类型 (1-15)
	//2.结构图	单线图			key=(pView->handle+m_nSegI)%16+16 (16-31)
	//			图层视图结构图  key=pView->handle+m_nSegI;
	//			定制视图结构图	key=pView->handle;
	//			表格			key=表格类型 (1-15) 大于15时可能会出现重复key值
	//CSuperHashList<CDrawingComponent> hashListComponent;	//视图列表
	ATOM_LIST<CSectionViewSymbol> sectionViewSymbols;		//剖视图符
	CDrawingRectArray drawingRectArr;	//已布置矩形数组
public:
	CDrawing(int id=0);
	virtual ~CDrawing();
	CHashList<CLDSPartPtr> hashProcessedParts;	//用于对一张图纸刷新时，记录

	// 根据指定的视图在图纸中生成对应的视图
	// pView 指定视图
	// pModule 生成司令图或总图时该变量有效,用来指定呼高分组
	// bFlag   结构图:表示生成单线图还是结构图 司令图\总图:表示正面视图侧面视图
	CDrawingComponent *AddComponentByView(CDisplayView *pView,CLDSModule *pModule,BOOL bFlag=FALSE);
	CDrawing& operator = (const CDrawing &drawing);
	//void CopyProperty(CDrawing *pDrawing);
	void InitPartDrawing(CStruView *pStruView,CHashStrList<CLDSPart*>& part_list);
	static const long DARWING_MECHANICS	= 0xffffffff;	//-1：力学设计司令图
	static const long DARWING_SUMMARY	= 0xfffffffe;	//-2：施工图总图
	void InitDrawing(long liDrwTypeOrSegI);		//初始化图纸
	void CreateComponents();			//生成图纸内的视图
	void InsertMapFrm();				//插入图框
	void DimSectionViewSymbol();		//标注剖视图符
	f3dPoint GetMapFrmInsideTopLeft();	//得到基准点即图框内侧矩形左上角顶点
	void UpdateBOMTableInitPos(CDrawingBOM *pDrawingBOM);	//根据已生成表格重新计算当前表格初始位置
	CString MakeComponentSetStr();
	void UpdateSimpleSpecList();
	void UpdatePtrByHandle();			//通过句柄更新图纸中与塔相关的指针,保存后再次打开文件时使用 wht 11-07-22
	void UpdateCmdList();				//更新图纸内所有图元的命令列表	wht 11-07-31
	void EraseRelaCadEnt();				//删除关联的CAD实体				wht 13-04-19
	//
	void ToBuffer(CBuffer &buffer);
	void FromBuffer(CBuffer &buffer, long version=NULL);
public:	//便于减少代码量而写的简易封装函数
	CDrawingComponent* AddComponent(DWORD key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true){
		return (CDrawingComponent*)listComponents.Add(key,CLSTYPE_ID,loadDefaultObjInfo);
	}
	CDrawingComponent* GetFirstDrawingComponent(){return (CDrawingComponent*)listComponents.GetFirst();}
	CDrawingComponent* GetNextDrawingComponent(){return (CDrawingComponent*)listComponents.GetNext();}
public:
	void ClearDrawnStates(int stateType=CDrawingComponent::TYPE_ROD);
	bool IsItemDrawn(int stateType,long idItem);	//返回指定项目是否在当前图纸中绘制过
	bool IsItemDimmed(int stateType,long idItem);	//返回指定项目是否在当前图纸中标注过
	void MarkItemDrawnState(CDrawingComponent *pComponent,int stateType,long idItem,bool drawn=true);
	void MarkItemDimSpecState(CDrawingComponent *pComponent,int stateType,long idItem,bool dimmed=true);
};

//图纸
extern CSuperHashList<CDrawing> drawingList; //方便移动元素位置
#endif
#endif // !defined(AFX_DRAWING_H__0EBE37F4_B612_4A73_9CB4_D45B171C1F29__INCLUDED_)
